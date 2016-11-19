/********************************************************************
	created:	  2016/10/09
	filename: 	  CecilyUVTCPClient.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/

#include "StdAfx.h"
#include "CecilyUVTCPClient.h"

extern long g_ClientID = 1;

CCecilyMemoryPool* CecilyUVTCPClient::m_lpMemoryPool = NULL;

CecilyUVTCPClient::CecilyUVTCPClient()
{
	int nRet = uv_loop_init(&m_loop);
	if ( nRet )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(nRet);
	}
	nRet = uv_mutex_init(&m_mutex_writebuf);
	if ( nRet )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(nRet);
	}
	m_connect_req.data = this;
	m_bIsClosed = true;
	m_bIsForceClosed = false;
	m_recvCallBack = NULL;
	m_recv_user_data = NULL;
	m_tcpCloseCallBack = NULL;
	m_tcp_close_user_data = NULL;
	m_bEnableReconnect = false;
	m_bIsIPV6 = false;
	m_status = S_DISCONNECT;
	m_tcpClientCtx = clear::AllocateUVTCPClientCtx(this);
	m_lpSerialize = new CecilyDataSerialize();
}

CecilyUVTCPClient::~CecilyUVTCPClient(void)
{
	clear::FreeUVTCPClientCtx(m_tcpClientCtx);
}

void CecilyUVTCPClient::SetMemoryPool(CCecilyMemoryPool* lpMemoryPool)
{
	m_lpMemoryPool = lpMemoryPool;
}

void CecilyUVTCPClient::Close()
{
	if ( m_bIsClosed )
	{
		return ;
	}
	m_bIsForceClosed = true;
	uv_async_send(&m_async_handle);
}

bool CecilyUVTCPClient::Run(int status)
{
	int iret = uv_run(&m_loop,(uv_run_mode)status);
	m_bIsClosed = true;
	if ( NULL != m_tcpCloseCallBack )
	{
		m_tcpCloseCallBack(m_tcpClientCtx->clientID,m_tcp_close_user_data);
	}
	if ( iret )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(iret);
		return false;
	}
	return true;
}

bool CecilyUVTCPClient::SetNoDelay(bool bDelay)
{
	int nRet = uv_tcp_nodelay(&m_tcpClientCtx->tcpHandle,bDelay?1:0);
	if ( nRet )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(nRet);
		return false;
	}
	return true;
}

bool CecilyUVTCPClient::SetKeepAlive(bool bKeepAlive, unsigned int nDelay )
{
	int nRet = uv_tcp_keepalive(&m_tcpClientCtx->tcpHandle,bKeepAlive,nDelay);
	if ( nRet )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(nRet);
		return false;
	}
	return true;
}

void CecilyUVTCPClient::SetRecvCallBack(TCPClientRecvCallBack lpfunc, void* user_data)
{
	m_recvCallBack = lpfunc;
	m_recv_user_data = user_data;
}

void CecilyUVTCPClient::SetCloseCallBack(TCPCloseCallBack lpfunc, void* user_data)
{
	m_tcpCloseCallBack = lpfunc;
	m_tcp_close_user_data = user_data;
}

bool CecilyUVTCPClient::Connect(const char* lpIPAddress, unsigned int nPort)
{
	if ( NULL == lpIPAddress )
	{
		return false;
	}
	MakeSureClose();
	if ( !Init() )
	{
		return false;
	}
	m_strConnect = lpIPAddress;
	m_nPort = nPort;
	m_bIsIPV6 = false;
	struct sockaddr_in bind_addr;
	int iret = uv_ip4_addr(m_strConnect.c_str(), m_nPort, &bind_addr);
	if ( iret )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(iret);
		return false;
	}
	iret = uv_tcp_connect(&m_connect_req,&m_tcpClientCtx->tcpHandle,(const sockaddr*)&bind_addr,AfterConnect);
	if ( iret )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(iret);
		return false;
	}
	iret = uv_thread_create(&m_connect_thread_handle, ConnectThread, this);
	if ( iret )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(iret);
		return false;
	}
	int wait_count = 0;
	while (m_status == S_DISCONNECT) {
		uv_thread_sleep(100);
		if (++wait_count > 100) {
			m_status = S_TIME_OUT;
			break;
		}
	}
	if (S_FINISH != m_status) {
		m_strErrorMsg = "connect time out";
		return false;
	} else {
		m_bIsClosed = false;
		m_bIsForceClosed = false;
		return true;
	}
	return true;
}

int CecilyUVTCPClient::Send(const char*lpData, unsigned int len,bool bSerialize)
{
	if ( !lpData || len <= 0 || m_bIsClosed)
	{
		return 0;
	}
	
	if ( !m_bIsClosed )
	{
		uv_async_send(&m_async_handle);
		uv_mutex_lock(&m_mutex_writebuf);
		clear::UVTCPWriteParam* lpBufferObject = new clear::UVTCPWriteParam(m_lpMemoryPool);
		lpBufferObject->write_req.data = this;
		int nSerializeLength = 0;
		char* lpSerializeData = NULL;
		if ( bSerialize )
		{
			lpSerializeData = (char*)m_lpSerialize->Serialize(lpData,len);
			nSerializeLength = len+Message_Header_Size+2;
		}else
		{
			lpSerializeData = (char*)lpData;
			nSerializeLength = len;
		}
		while ( nSerializeLength > 0 )
		{
			int nSize = MAX_TCP_BUFFER_SIZE < nSerializeLength ? MAX_TCP_BUFFER_SIZE : nSerializeLength;
			bool bWrite = lpBufferObject->WriteBuffer(lpSerializeData,nSize);
			if ( bWrite )
			{
				m_queueWriteParam.push_back(lpBufferObject);
			}
			nSerializeLength = nSerializeLength - nSize;
			lpSerializeData = lpSerializeData + nSize;
		}		
		uv_mutex_unlock(&m_mutex_writebuf);
		uv_async_send(&m_async_handle);
	}
	
	return len;
}


bool CecilyUVTCPClient::Init()
{
	if ( !m_bIsClosed )
	{
		return true;
	}
	int iRet = uv_async_init(&m_loop,&m_async_handle,AsyncCallBack);
	if ( iRet )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(iRet);
		return false;
	}
	m_async_handle.data = this;
	iRet = uv_tcp_init(&m_loop,&m_tcpClientCtx->tcpHandle);
	if ( iRet )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(iRet);
		return false;
	}
	m_tcpClientCtx->tcpHandle.data = m_tcpClientCtx;
	m_tcpClientCtx->parent = this;
	iRet = uv_timer_init(&m_loop,&m_reconnect_timer);
	if ( iRet )
	{
		m_strErrorMsg = clear::GetUVErrorMsg(iRet);
		return false;
	}
	m_reconnect_timer.data = this;
	m_bIsClosed = false;
	if ( NULL != m_lpSerialize )
	{
		m_lpSerialize->SetFullRecvCallBack(RecvFullPackageCallBack,this);
	}
	return true;
}

bool CecilyUVTCPClient::EnableReconnect()
{
	m_bEnableReconnect = true;
	m_tcpClientCtx->tcpHandle.data = this;
	m_reconnect_interval = 1000;
	return m_bEnableReconnect;
}

void CecilyUVTCPClient::DisableReconnect()
{
	m_bEnableReconnect = false;
	m_tcpClientCtx->tcpHandle.data = m_tcpClientCtx;
	m_reconnect_interval = 1000;
	uv_timer_stop(&m_reconnect_timer);
}

void CecilyUVTCPClient::MakeSureClose()
{
	if ( m_bIsClosed )
	{
		return;
	}
	DisableReconnect();
	uv_walk(&m_loop,CloseWalkCallBack,this);
}

void CecilyUVTCPClient::ReSend(UVTCPWriteParam* lpParam)
{
	if ( NULL != lpParam )
	{
		uv_mutex_lock(&m_mutex_writebuf);
		m_queueWriteParam.push_front(lpParam);
		uv_mutex_unlock(&m_mutex_writebuf);
		MakeSureSend(NULL);
	}
}

void CecilyUVTCPClient::DeSerialize(char* lpBuffer, int nLen)
{
	m_lpSerialize->DeSerialize(this->m_tcpClientCtx->clientID,(const char*)lpBuffer,nLen);
}

void CecilyUVTCPClient::MakeSureSend(uv_write_t* req)
{
//	UVTCPWriteParam* lpLastWriteParam = (UVTCPWriteParam*)req;
// 	if ( NULL != lpLastWriteParam )
// 	{
// 		uv_mutex_lock(&m_mutex_writebuf);
// 		m_queueWriteParam.pop();
// 		uv_mutex_unlock(&m_mutex_writebuf);
// 		delete lpLastWriteParam;
// 		lpLastWriteParam = NULL;
// 	}
	uv_mutex_lock(&m_mutex_writebuf);
	if ( m_queueWriteParam.empty() )
	{
		uv_mutex_unlock(&m_mutex_writebuf);
		return;
	}
	UVTCPWriteParam* lpWriteParam = m_queueWriteParam.front();
	lpWriteParam->write_req.data = this;
	if ( NULL == lpWriteParam )
	{
		m_queueWriteParam.pop_front();
	}
	m_queueWriteParam.pop_front();
	uv_mutex_unlock(&m_mutex_writebuf);
	int iret = uv_write((uv_write_t*)&lpWriteParam->write_req, (uv_stream_t*)&m_tcpClientCtx->tcpHandle, &lpWriteParam->write_buf, 1, AfterSend);
	if (iret == 0 ) {

	}
}

void CecilyUVTCPClient::AsyncCallBack(uv_async_t* handle)
{
	CecilyUVTCPClient* theclass = (CecilyUVTCPClient*)handle->data;
	if (theclass->m_bIsForceClosed) {
		theclass->MakeSureClose();
		return;
	}
	theclass->MakeSureSend(NULL);

}

void CecilyUVTCPClient::CloseWalkCallBack(uv_handle_t* handle, void* arg)
{
	CecilyUVTCPClient* lpThis = (CecilyUVTCPClient*)arg;
	if ( NULL == lpThis )
	{
		return ;
	}
	if ( !uv_is_closing(handle) )
	{
		uv_close(handle,AfterClientClose);
	}
}

void CecilyUVTCPClient::AfterClientClose(uv_handle_t* handle)
{
	CecilyUVTCPClient* lpThis = (CecilyUVTCPClient*)handle->data;
	//lpThis->m_bIsClosed =  true;
	if ( handle == (uv_handle_t*)&lpThis->m_tcpClientCtx->tcpHandle && lpThis->m_bEnableReconnect  )
	{
		int nRet = 0;
		nRet = uv_timer_start(&lpThis->m_reconnect_timer,ReconnectTimer,lpThis->m_reconnect_interval,0);
		if ( nRet )
		{
			uv_close((uv_handle_t*)&lpThis->m_reconnect_timer,AfterClientClose);
			return ;
		}
	}
}

void CecilyUVTCPClient::ReconnectTimer(uv_timer_t* handle)
{
	CecilyUVTCPClient* lpThis = (CecilyUVTCPClient*)handle->data;
	if ( !lpThis->m_bEnableReconnect )
	{
		return ;
	}
	do 
	{
		int nRet = uv_tcp_init(&lpThis->m_loop,&lpThis->m_tcpClientCtx->tcpHandle);
		if ( nRet )
		{
			
			break;
		}
		lpThis->m_tcpClientCtx->tcpHandle.data = lpThis->m_tcpClientCtx;
		lpThis->m_tcpClientCtx->parent = lpThis;
		struct sockaddr* lpAddr;
		if ( lpThis->m_bIsIPV6 )
		{
			struct sockaddr_in6 bind_addr;
			int iret = uv_ip6_addr(lpThis->m_strConnect.c_str(), lpThis->m_nPort, &bind_addr);
			if (iret) {
				lpThis->m_strErrorMsg = clear::GetUVErrorMsg(nRet);
				uv_close((uv_handle_t*)&lpThis->m_tcpClientCtx->tcpHandle, NULL);
				break;
			}
			lpAddr = (struct sockaddr*)&bind_addr;
		}
		else
		{
			struct sockaddr_in bind_addr;
			int iret = uv_ip4_addr(lpThis->m_strConnect.c_str(), lpThis->m_nPort, &bind_addr);
			if (iret) {
				lpThis->m_strErrorMsg = clear::GetUVErrorMsg(nRet);
				uv_close((uv_handle_t*)&lpThis->m_tcpClientCtx->tcpHandle, NULL);
				break;
			}
			lpAddr = (struct sockaddr*)&bind_addr;
		}
		nRet = uv_tcp_connect(&lpThis->m_connect_req, &lpThis->m_tcpClientCtx->tcpHandle, (const sockaddr*)lpAddr, AfterConnect);
		if (nRet) {
			lpThis->m_strErrorMsg = clear::GetUVErrorMsg(nRet);
			uv_close((uv_handle_t*)&lpThis->m_tcpClientCtx->tcpHandle, NULL);
			break;
		}else
		{
			//lpThis->m_bIsClosed = false;
		}
		return;

	} while (0);
	uv_timer_stop(handle);
	//lpThis->m_reconnect_interval *= 2;
	uv_timer_start(handle, ReconnectTimer, lpThis->m_reconnect_interval, 0);
}

void CecilyUVTCPClient::AfterConnect(uv_connect_t* req, int status)
{
	UVTCPClientCtx* lpCtx= (UVTCPClientCtx*)req->handle->data;
	CecilyUVTCPClient* parent = (CecilyUVTCPClient*)lpCtx->parent;
	if ( status )
	{
		parent->m_status = S_ERROR;
		parent->m_strErrorMsg = clear::GetUVErrorMsg(status);
		if (parent->m_bEnableReconnect) {
			uv_timer_stop(&parent->m_reconnect_timer);
			//parent->m_reconnect_interval *= 2;
			uv_timer_start(&parent->m_reconnect_timer,ReconnectTimer, parent->m_reconnect_interval, 0);
		}
	}
	 int iret = uv_read_start(req->handle, AllocateBufferForRecv, AfterRecv);
	 if (iret) {
		 parent->m_strErrorMsg = clear::GetUVErrorMsg(iret);
		 parent->m_status = S_ERROR;
		 return;
	 }else
	 {
		 parent->m_status = S_FINISH;
	 }
}

void CecilyUVTCPClient::AfterRecv(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf)
{
	UVTCPClientCtx* lpCtx = (UVTCPClientCtx*)handle->data;
	if ( NULL == lpCtx )
	{
		return;
	}
	CecilyUVTCPClient* lpThis = (CecilyUVTCPClient*)lpCtx->parent;
	if ( nread < 0 )
	{
		if (!lpThis->EnableReconnect())
		{
			return ;
		}
		if ( nread == UV_EOF )
		{
		}else if (nread == UV_ECONNRESET)
		{
		}
		uv_close((uv_handle_t*)handle, AfterClientClose);
		return ;
	}
	//lpThis->MakeSureSend(NULL);
	if ( nread > 0 )
	{
		//lpThis->m_recvCallBack((char*)buf->base,nread,lpThis);
		lpThis->DeSerialize(buf->base,buf->len);
		if ( NULL !=  m_lpMemoryPool )
		{
			m_lpMemoryPool->Deallocate((void*)buf->base,buf->len);
		}
		else
		{
			delete []buf->base;
		}
		
	}
}

void CecilyUVTCPClient::AfterSend(uv_write_t* req, int status)
{
	CecilyUVTCPClient* theclass = (CecilyUVTCPClient*)req->data;
	UVTCPWriteParam* lpParam = (UVTCPWriteParam*)req;
	if (status < 0) {
		//发送失败，加到队列缓冲区头部，继续发送
		theclass->ReSend(lpParam);
		return;
	}
	//发送成功，通知队列缓冲区，释放内存，移除出队列
	delete lpParam;
	lpParam = NULL;
	theclass->MakeSureSend(NULL);
}

void CecilyUVTCPClient::ConnectThread(void* arg)
{
	CecilyUVTCPClient* lpThis = (CecilyUVTCPClient*)arg;
	if ( NULL != lpThis )
	{
		lpThis->Run();
	}
}

void CecilyUVTCPClient::AllocateBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	if ( NULL != m_lpMemoryPool )
	{
		buf->base = (char*)m_lpMemoryPool->Allocate(MAX_TCP_BUFFER_SIZE);
		buf->len = MAX_TCP_BUFFER_SIZE;
	}
	else
	{
		buf->base = new char[MAX_TCP_BUFFER_SIZE];
		memset(buf->base,0,MAX_TCP_BUFFER_SIZE);
		buf->len = MAX_TCP_BUFFER_SIZE;
	}
}

void CecilyUVTCPClient::RecvFullPackageCallBack(int nClientID,char* lpBuffer,int nLen,void* user_data)
{
	CecilyUVTCPClient* lpTCPClient = (CecilyUVTCPClient*)user_data;
	if ( NULL ==  lpTCPClient )
	{
		return ; 
	}
	if ( NULL != lpTCPClient->m_recvCallBack)
	{
		lpTCPClient->m_recvCallBack(lpBuffer,nLen,lpTCPClient->m_recv_user_data);
	}
}