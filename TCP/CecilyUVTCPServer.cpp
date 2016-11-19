/********************************************************************
	created:	  2016/10/10
	filename: 	  CecilyUVTCPServer.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "CecilyUVTCPServer.h"

CCecilyMemoryPool* CecilyUVTCPServer::m_lpServerMemoryPool = NULL;

CecilyUVTCPServer::CecilyUVTCPServer(CCecilyMemoryPool* lpMemoryPool):
m_lpFuncTCPClose(NULL),
m_lpFuncTCPNewConnect(NULL),
m_lpTCPCloseUserData(NULL),
m_lpTCPNewConnectUserData(NULL),
m_bIsClose(true),
m_bIsForceClose(false),
m_serverStatus(S_DISCONNECT)
{
	int iRet = uv_loop_init(&m_serverLoop);
	if ( iRet )
	{
		m_strErrorMsg = GetUVErrorMsg(iRet);
	}
}

CecilyUVTCPServer::~CecilyUVTCPServer(void)
{
}

bool CecilyUVTCPServer::Start(const char* lpIPAddress, int nPort)
{
	if ( NULL == lpIPAddress || nPort <=0 )
	{
		return false;
	}
	m_strServerIP = lpIPAddress;
	m_nPort = nPort;
	MakeSureClose();
	if ( !Init())
	{
		return false;
	}
	if ( !Bind(lpIPAddress,nPort) )
	{
		return false;
	}
	if (!Listen(SOMAXCONN)) {
		return false;
	}
	int iret = uv_thread_create(&start_handle, StartThread, this);//use thread to wait for start succeed.
	if (iret) {
		m_strErrorMsg = GetUVErrorMsg(iret);
		return false;
	}
	int wait_count = 0;
	while (m_serverStatus == S_DISCONNECT) {
		uv_thread_sleep(100);
		if (++wait_count > 100) {
			m_serverStatus = S_TIME_OUT;
			break;
		}
	}
	return m_serverStatus == S_FINISH;
}

void CecilyUVTCPServer::Close()
{
	if ( m_bIsClose )
	{
		return ;
	}
	m_bIsForceClose = true;
	uv_async_send(&m_async_handle_close);
}

void CecilyUVTCPServer::SetRecvCallBack(int nClientID, TCPServerRecvCallBack lpFunc,void* userdata)
{
	{
		CUVAutoLock locker(&m_clientMutex);
		map<int,CecilyNetClient*>::iterator itClient = m_mapClient.find(nClientID);
		if ( itClient != m_mapClient.end() )
		{
			if ( NULL != itClient->second )
			{
				(itClient->second)->SetServerRecvCallBack(lpFunc,userdata);
			}			
		}
	}
}

void CecilyUVTCPServer::SetCloseCallBack(TCPCloseCallBack lpFunc, void* userdata)
{
	m_lpFuncTCPClose = lpFunc;
	m_lpTCPCloseUserData = userdata;
}

void CecilyUVTCPServer::SetNewConnectCallBack(TCPNewConnectCallBack lpFunc, void* userdata)
{
	m_lpFuncTCPNewConnect = lpFunc;
	m_lpTCPNewConnectUserData = userdata;
}


bool CecilyUVTCPServer::SetNoDelay(bool bDelay)
{
	int iRet = uv_tcp_nodelay((uv_tcp_t*)&m_tcpHandle,bDelay?1:0);
	if ( iRet )
	{
		m_strErrorMsg = GetUVErrorMsg(iRet);
		return false;
	}
	return true;
}

bool CecilyUVTCPServer::SetKeepAlive(bool bEnable,unsigned int nDelay)
{
	int iRet = uv_tcp_keepalive((uv_tcp_t*)&m_tcpHandle,bEnable,nDelay);
	if ( iRet )
	{
		m_strErrorMsg = GetUVErrorMsg(iRet);
		return false;
	}
	return true;
}

void CecilyUVTCPServer::RemoveClient(int nClientID)
{
	CUVAutoLock locker(&m_clientMutex);
	map<int,CecilyNetClient*>::iterator itClient = m_mapClient.find(nClientID);
	CecilyNetClient* lpNetClient = NULL;
	if ( itClient != m_mapClient.end() )
	{
		lpNetClient = itClient->second;
		if ( NULL != m_lpFuncTCPClose )
		{
			m_lpFuncTCPClose(nClientID,m_lpTCPCloseUserData);
		}
		if ( NULL != lpNetClient )
		{
			lpNetClient->Close();
// 			//有问题不知道为什么这里释放的话，调试过程中会出现内存申请释放异常，
// 			//但是非调试模式下也可以正常运行,会有内存泄漏，后续有时间继续修复2016.10.10
// 			FreeUVTCPClientCtx(lpNetClient->GetUVTCPClientCtx());
// 			SetLastNetClient(lpNetClient);				
		}
		m_mapClient.erase(itClient);
	}

}

void CecilyUVTCPServer::AddClient(int nClientID, CecilyNetClient* lpNetClient)
{
	{
		CUVAutoLock locker(&m_clientMutex);
		m_mapClient.insert(pair<int,CecilyNetClient*>(nClientID,lpNetClient));
	}
}

bool CecilyUVTCPServer::SendData(int nClientID, const char* lpbuffer, int nLen)
{
	{
		CUVAutoLock locker(&m_clientMutex);
		map<int,CecilyNetClient*>::iterator itClient = m_mapClient.find(nClientID);
		CecilyNetClient* lpNetClient = NULL;
		if ( itClient != m_mapClient.end() )
		{
			lpNetClient = itClient->second;
			if ( NULL == lpNetClient )
			{
				return false;
			}
			lpNetClient->SendData(lpbuffer,nLen);
		}else
		{
			return false;
		}
	}
	return true;
}

void CecilyUVTCPServer::SetMemoryPool(CCecilyMemoryPool* lpMemoryPool)
{
	m_lpServerMemoryPool = lpMemoryPool;
}

bool CecilyUVTCPServer::Init()
{
	if ( !m_bIsClose  )
	{
		return true;
	}
	int iRet = uv_async_init(&m_serverLoop,&m_async_handle_close,AsyncCallBack);
	if ( iRet )
	{
		m_strErrorMsg = GetUVErrorMsg(iRet);
		return false;
	}
	m_async_handle_close.data = this;
	iRet = uv_tcp_init(&m_serverLoop,&m_tcpHandle);
	if ( iRet )
	{
		m_strErrorMsg = GetUVErrorMsg(iRet);
		return false;
	}
	m_tcpHandle.data = this;
	bool bSet = SetNoDelay(true);
	if ( !bSet )
	{
		return false;
	}
	m_bIsClose = false;
	return true;
}

bool CecilyUVTCPServer::Run(int status /* = UV_RUN_DEFAULT */)
{
	int iret = uv_run(&m_serverLoop, (uv_run_mode)status);
	if (iret) {
		m_strErrorMsg = GetUVErrorMsg(iret);
		return false;
	}
	return true;
}

bool CecilyUVTCPServer::Bind(const char* lpIpAddress, int nPort)
{
	struct sockaddr_in bind_addr;
	int iret = uv_ip4_addr(lpIpAddress, nPort, &bind_addr);
	if (iret) {
		m_strErrorMsg = GetUVErrorMsg(iret);
		return false;
	}
	iret = uv_tcp_bind(&m_tcpHandle, (const struct sockaddr*)&bind_addr, 0);
	if (iret) {
		m_strErrorMsg = GetUVErrorMsg(iret);
		return false;
	}
	return true;
}

bool CecilyUVTCPServer::Listen(int backlog )
{
	int iret = uv_listen((uv_stream_t*) &m_tcpHandle, backlog, AcceptConnection);
	if (iret) {
		m_strErrorMsg = GetUVErrorMsg(iret);
		return false;
	}
	return true;
}

void CecilyUVTCPServer::MakeSureClose()
{
	if ( m_bIsClose )
	{
		return;
	}
	{
		CUVAutoLock locker(&m_clientMutex);
		map<int,CecilyNetClient*>::iterator itClient = m_mapClient.begin();
		for ( itClient; itClient != m_mapClient.end(); itClient++ )
		{
			CecilyNetClient* lpClient = itClient->second;
			if ( NULL != lpClient )
			{
				lpClient->Close();
			}
		}
	}
	uv_walk(&m_serverLoop,CloseWalkCallBack,this);
}

void CecilyUVTCPServer::StartThread(void* arg)
{
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)arg;
	lpServer->m_serverStatus = S_FINISH;
	lpServer->Run();
	lpServer->m_bIsClose = true;
	lpServer->m_bIsForceClose = false;
	if (lpServer->m_lpFuncTCPClose) {
		lpServer->m_lpFuncTCPClose(-1, lpServer->m_lpTCPCloseUserData);
	}
}

void CecilyUVTCPServer::CloseWalkCallBack(uv_handle_t* handle, void* arg)
{
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)arg;
	if ( NULL != lpServer )
	{
		if ( !uv_is_closing(handle) )
		{
			uv_close(handle,AfterServerClose);
		}
	}
}

void CecilyUVTCPServer::AfterServerClose(uv_handle_t* handle)
{
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)handle->data;
// 	if ( NULL != lpServer )
// 	{
// 		if ( NULL != lpServer->m_lpFuncTCPStop )
// 		{
// 			lpServer->m_lpFuncTCPStop();
// 		}
// 	}
}

void CecilyUVTCPServer::AsyncCallBack(uv_async_t* handle)
{
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)handle->data;
	if ( lpServer->m_bIsForceClose )
	{
		lpServer->MakeSureClose();
	}
	return ;
}

void CecilyUVTCPServer::AcceptConnection(uv_stream_t* server, int status)
{
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)server->data;
	if ( NULL == lpServer )
	{
		return ;
	}
	int iRet = 0;
	if ( status == 0 )
	{
		UVTCPClientCtx* lpCtx = NULL;
		lpCtx = AllocateUVTCPClientCtx(lpServer,1);
		if ( NULL == lpCtx )
		{
			return ;
		}
		iRet = uv_tcp_init(&lpServer->m_serverLoop,&lpCtx->tcpHandle);
		if ( iRet )
		{
			FreeUVTCPClientCtx(lpCtx);
			lpServer->m_strErrorMsg = GetUVErrorMsg(iRet);
			return ;
		}
		lpCtx->tcpHandle.data = lpCtx;
		iRet = uv_accept(server,(uv_stream_t*)&lpCtx->tcpHandle);
		if ( iRet )
		{
			FreeUVTCPClientCtx(lpCtx);
			lpServer->m_strErrorMsg = GetUVErrorMsg(iRet);
			return ;
		}
// 		iRet = uv_read_start((uv_stream_t*)&lpCtx->tcpHandle, AllocBufferForRecv, AfterRecv);
// 		if ( iRet )
// 		{
// 			uv_close((uv_handle_t*)&lpCtx->tcpHandle,AcceptCloseClient);
// 			lpServer->m_strErrorMsg = GetUVErrorMsg(iRet);
// 			return;
// 		}
		CecilyNetClient* lpNetClient = new CecilyNetClient(lpCtx,lpCtx->clientID,&lpServer->m_serverLoop);
		if ( NULL == lpNetClient )
		{
			return;
		}
		int iRet = lpNetClient->BeginRecv();
		if ( iRet )
		{
			lpServer->m_strErrorMsg = GetUVErrorMsg(iRet);
			return ;
		}
		lpNetClient->SetCloseCallBack(NetClientClosed,lpServer);
		{
			lpServer->AddClient(lpCtx->clientID,lpNetClient);
		}
		if ( NULL != lpServer->m_lpFuncTCPNewConnect )
		{
			lpServer->m_lpFuncTCPNewConnect(lpCtx->clientID,lpServer->m_lpTCPNewConnectUserData);
		}
	}
	if ( status < 0 )
	{
		lpServer->m_strErrorMsg = GetUVErrorMsg(status);
		return ;
	}

}

void CecilyUVTCPServer::AllocBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	if ( NULL != m_lpServerMemoryPool )
	{
		buf->base = (char*)m_lpServerMemoryPool->Allocate(MAX_TCP_BUFFER_SIZE);
		buf->len = MAX_TCP_BUFFER_SIZE;
	}
	else
	{
		buf->base = new char[MAX_TCP_BUFFER_SIZE];
		memset(buf->base,0,MAX_TCP_BUFFER_SIZE);
		buf->len = MAX_TCP_BUFFER_SIZE;
	}
}

void CecilyUVTCPServer::AfterRecv(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	UVTCPClientCtx* lpCtx = (UVTCPClientCtx*)client->data;
	if ( NULL == lpCtx )
	{
		return ;
	}
	CecilyNetClient* lpNetClient = NULL;
	lpNetClient = (CecilyNetClient*)lpCtx->parent;
	if ( nread < 0 )
	{
		if (nread == UV_EOF) {
			
		} else if (nread == UV_ECONNRESET) {
			
		} else {
			
		}
		if ( NULL != lpNetClient )
		{
			lpNetClient->Close();
		}	
	}
	if ( nread > 0 )
	{
		if ( NULL != lpNetClient )
		{
			lpNetClient->DeSerialize(lpCtx->clientID,(char*)buf->base,nread);
		}
	}

}

void CecilyUVTCPServer::AcceptCloseClient(uv_handle_t* handle)
{
	UVTCPClientCtx* lpCtx = (UVTCPClientCtx*)handle->data;
	if ( NULL == lpCtx )
	{
		return ;
	}
	FreeUVTCPClientCtx(lpCtx);
}


void CecilyUVTCPServer::NetClientClosed(int clientid, void* userdata)
{
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)userdata;
	if ( NULL == lpServer )
	{
		return ;
	}
	lpServer->RemoveClient(clientid);
// 	{
// 		CUVAutoLock locker(&lpServer->m_clientMutex);
// 		map<int,CecilyNetClient*>::iterator itClient = lpServer->m_mapClient.find(clientid);
// 		CecilyNetClient* lpNetClient = NULL;
// 		if ( itClient != lpServer->m_mapClient.end() )
// 		{
// 			lpNetClient = itClient->second;
// 			lpServer->m_mapClient.erase(itClient);
// 			if ( NULL != lpServer->m_lpFuncTCPClose )
// 			{
// 				lpServer->m_lpFuncTCPClose(clientid,lpServer->m_lpTCPCloseUserData);
// 			}
// 			if ( NULL != lpNetClient )
// 			{
// 				lpNetClient->Close();
// 				//FreeUVTCPClientCtx(lpNetClient->GetUVTCPClientCtx());
// #ifndef _DEBUG
// 				delete lpNetClient;
// 				lpNetClient = NULL;
// #endif
// 			}
// 			
// 		}
// 	}

}