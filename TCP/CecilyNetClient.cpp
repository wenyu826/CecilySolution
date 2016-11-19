/********************************************************************
	created:	  2016/10/10
	filename: 	  CecilyNetClient.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "CecilyNetClient.h"

CCecilyMemoryPool* CecilyNetClient::m_lpNetClientMemoryPool = NULL;

CecilyNetClient::CecilyNetClient(UVTCPClientCtx* lpCtx,int nClientID, uv_loop_t* loop):
m_lpTcpClientCtx(lpCtx),
m_nClientID(nClientID),
m_NetClient_loop(loop),
m_bIsClosed(true),
m_lpFuncTCPClose(NULL),
m_lpFuncTCPServerRecv(NULL),
m_lpTCPCloseUserData(NULL),
m_lpTCPServerRecvUserData(NULL),
m_lpFuncFullPackage(NULL),
m_lastRecvPackage(NULL)
{
	Init();
}

CecilyNetClient::~CecilyNetClient(void)
{
	//FreeUVTCPClientCtx(m_lpTcpClientCtx);
// 	if ( NULL == m_lpSerialize )
// 	{
// 		delete m_lpSerialize;
// 		m_lpSerialize = NULL;
// 	}
	//uv_close((uv_handle_t*) &m_async_send, NULL);
}


void CecilyNetClient::SetCloseCallBack(TCPCloseCallBack lpFunc, void* user_data)
{
	m_lpFuncTCPClose = lpFunc;
	m_lpTCPCloseUserData = user_data;
}

void CecilyNetClient::SetServerRecvCallBack(TCPServerRecvCallBack lpFunc, void* user_data)
{
	m_lpFuncTCPServerRecv = lpFunc;
	m_lpTCPServerRecvUserData = user_data;
}

UVTCPClientCtx* CecilyNetClient::GetUVTCPClientCtx() 
{
	return m_lpTcpClientCtx;
}

bool CecilyNetClient::Init()
{
	if ( !m_bIsClosed )
	{
		return true;
	}
	//m_lpSerialize = new CecilyDataSerialize();

	m_lpTcpClientCtx->parent = this;
	m_bIsClosed = false;
	int iRet = uv_async_init(m_NetClient_loop,&m_async_send,AsyncCallBack);
	if ( iRet )
	{
		m_strErrorMsg = GetUVErrorMsg(iRet);
		return false;
	}
	iRet = uv_async_init(m_NetClient_loop,&m_async_thread,AsyncThreadCallBack);
	if ( iRet )
	{
		m_strErrorMsg = GetUVErrorMsg(iRet);
		return false;
	}
	m_async_send.data = this;
	m_async_thread.data = this;
// 	if ( NULL != m_lpSerialize )
// 	{
// 		m_lpSerialize->SetFullRecvCallBack(AfterRecvFullPackage,this);
// 	}
	m_Serialize.SetFullRecvCallBack(AfterRecvFullPackage,this);
	m_threadRecvSem.Post();
	m_threadSendSem.Post();
	ResetThreadStatus();
	return true;
}

void CecilyNetClient::MakeSureSend(uv_write_t* write_req)
{
		UVTCPWriteParam* lpWriteParam = NULL;
		{
			CUVAutoLock locker(&m_writeMutex);
			if ( m_queueWriteParam.empty() )
			{
				return;
			}
			lpWriteParam = m_queueWriteParam.front();
			lpWriteParam->write_req.data = this;
			if ( NULL == lpWriteParam )
			{
				m_queueWriteParam.pop_front();
			}
			m_queueWriteParam.pop_front();
		}
		if ( NULL == lpWriteParam )
		{
			return;
		}
		WaitForThreadSendSem();
		NetPackage* lpPackage = new NetPackage;
		lpPackage->req.data = lpWriteParam;
		int iRet = uv_queue_work(m_NetClient_loop,(uv_work_t*)lpPackage,SendPackage,AfterSendPackage);
		//int iret = uv_write((uv_write_t*)&lpWriteParam->write_req, (uv_stream_t*)&m_lpTcpClientCtx->tcpHandle, &lpWriteParam->write_buf, 1, AfterSend);
}

void CecilyNetClient::MakeSureDeSerialize()
{
	{
		NetPackage* lpPackage  = NULL;
		{
			CUVAutoLock locker(&m_recvpackageMutex);
			if ( m_queueRecvPackage.empty())
			{
				return ;
			}
			lpPackage = m_queueRecvPackage.front();
			m_queueRecvPackage.pop();

		}
		WaitForThreadRecvSem();
		SetThreadStatus();
		if ( NULL != lpPackage )
		{
			uv_queue_work(m_NetClient_loop,(uv_work_t*)&lpPackage->req,ParsePackage,AfterParsePackage);
		}
	}

}

void CecilyNetClient::Close()
{
	if ( m_bIsClosed )
	{
		return ;
	}
	m_lpTcpClientCtx->tcpHandle.data = this;
	uv_close((uv_handle_t*)&m_lpTcpClientCtx->tcpHandle,AfterClientClose);
}

bool CecilyNetClient::SendData(const char* lpBuffer, unsigned int nLen)
{
	{
		//uv_async_send(&m_async_send);
		CUVAutoLock locker(&m_writeMutex);
		UVTCPWriteParam* lpParam = new UVTCPWriteParam(m_lpNetClientMemoryPool);
		lpParam->write_req.data = this;
		//char * lpSerializeBuffer = (char*)m_lpSerialize->Serialize(lpBuffer,nLen);
		char * lpSerializeBuffer = (char*)m_Serialize.Serialize(lpBuffer,nLen);
		int nSerializeBuffer = nLen + Message_Header_Size + 2;
		while ( nSerializeBuffer > 0 )
		{
			int nSize = MAX_TCP_BUFFER_SIZE < nSerializeBuffer ? MAX_TCP_BUFFER_SIZE : nSerializeBuffer;
			bool bWrite = lpParam->WriteBuffer(lpSerializeBuffer,nSize);
			if ( bWrite )
			{
				m_queueWriteParam.push_back(lpParam);
			}
			nSerializeBuffer = nSerializeBuffer - nSize;
			lpSerializeBuffer = lpSerializeBuffer + nSize;
		}
		uv_async_send(&m_async_send);
	}
	return true;

}

void CecilyNetClient::SetMemoryPool(CCecilyMemoryPool* lpMemoryPool)
{
	m_lpNetClientMemoryPool = lpMemoryPool;
	CecilyDataSerialize::SetMemoryPool(lpMemoryPool);
}
int CecilyNetClient::BeginRecv()
{
	int iRet = uv_read_start((uv_stream_t*)&m_lpTcpClientCtx->tcpHandle, AllocateBufferForRecv, AfterRecv);
	if ( iRet )
	{
		uv_close((uv_handle_t*)&m_lpTcpClientCtx->tcpHandle,AfterClientClose);
		return iRet;
	}
	return iRet;
}

PackageStatus CecilyNetClient::GetPackageStatus()
{
	return m_Serialize.GetPackageStatus();
}

void CecilyNetClient::AddNotFreeMemory(NetPackage* lpPackage)
{
	{
		CUVAutoLock locker(&m_BufferMutex);
		vector<NetPackage*>::iterator itVec = m_vecBuffers.begin();
		bool bFlag = false;
		for ( itVec; itVec != m_vecBuffers.end(); itVec++ )
		{
			if ( *itVec == lpPackage )
			{
				bFlag = true;
				break;
			}
		}
		if ( !bFlag )
		{
			m_vecBuffers.push_back(lpPackage);
		}
	}
}

void CecilyNetClient::FreeMemory(NetPackage* lpPackage)
{
	{
		CUVAutoLock locker(&m_BufferMutex);
		if ( m_vecBuffers.empty() )
		{
			return ;
		}
		vector<NetPackage*>::iterator itVec = m_vecBuffers.begin();
		for ( itVec; itVec != m_vecBuffers.end(); itVec++ )
		{
			if ( lpPackage == *itVec )
			{
				continue;
			}
			NetPackage* tmpPackage = *itVec;
			if ( NULL != m_lpNetClientMemoryPool )
			{
				m_lpNetClientMemoryPool->Deallocate((void*)tmpPackage->buf.base,tmpPackage->buf.len);
			}else
			{
				delete []tmpPackage->buf.base;
			}
			delete tmpPackage;
			tmpPackage = NULL;
		}
		m_vecBuffers.clear();
	}

}

// void CecilyNetClient::AddNotFreeMemory(uv_buf_t* buf)
// {
// 	{
// 		CUVAutoLock locker(&m_BufferMutex);
// 		vector<uv_buf_t*>::iterator itVec = m_vecBuffers.begin();
// 		bool bFlag = false;
// 		for ( itVec; itVec != m_vecBuffers.end(); itVec++ )
// 		{
// 			if ( *itVec == buf )
// 			{
// 				bFlag = true;
// 				break;
// 			}
// 		}
// 		if ( !bFlag )
// 		{
// 			m_vecBuffers.push_back(buf);
// 		}
// 		
// 	}
// }
// void CecilyNetClient::FreeMemory(uv_buf_t* buf)
// {
// 	{
// 		CUVAutoLock locker(&m_BufferMutex);
// 		if ( m_vecBuffers.empty() )
// 		{
// 			return ;
// 		}
// 		vector<uv_buf_t*>::iterator itVec = m_vecBuffers.begin();
// 		for ( itVec; itVec != m_vecBuffers.end(); itVec++ )
// 		{
// 			if ( buf == *itVec )
// 			{
// 				continue;;
// 			}
// 			uv_buf_t* tmpBuf = *itVec;
// 			if ( NULL != m_lpNetClientMemoryPool )
// 			{
// 				m_lpNetClientMemoryPool->Deallocate((void*)tmpBuf->base,tmpBuf->len);
// 			}else
// 			{
// 				delete []tmpBuf->base;
// 			}
// 		}
// 		m_vecBuffers.clear();
// 	}
// 	
// }

void CecilyNetClient::DeSerialize(int nClientID, char* lpBuffer, int nRead)
{
	m_Serialize.DeSerialize(nClientID,(const char*)lpBuffer,nRead);
}

void CecilyNetClient::AfterClientClose(uv_handle_t* handle)
{
	CecilyNetClient* lpThis = (CecilyNetClient*)handle->data;
	if ( NULL == lpThis )
	{
		return;
	}
	if ( handle == (uv_handle_t*)&lpThis->m_lpTcpClientCtx->tcpHandle )
	{
		lpThis->m_bIsClosed = true;
		if ( lpThis->m_lpFuncTCPClose )
		{
			lpThis->m_lpFuncTCPClose(lpThis->m_nClientID,lpThis->m_lpTCPCloseUserData);
		}
	}
}

void CecilyNetClient::AllocateBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	if ( NULL != m_lpNetClientMemoryPool )
	{
		buf->base = (char*)m_lpNetClientMemoryPool->Allocate(MAX_TCP_BUFFER_SIZE);
		buf->len = MAX_TCP_BUFFER_SIZE;
	}
	else
	{
		buf->base = new char[MAX_TCP_BUFFER_SIZE];
		memset(buf->base,0,MAX_TCP_BUFFER_SIZE);
		buf->len = MAX_TCP_BUFFER_SIZE;
	}
}

void CecilyNetClient::AfterRecv(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	UVTCPClientCtx* lpCtx = (UVTCPClientCtx*)client->data;
	if ( NULL == lpCtx )
	{
		return ;
	}
	CecilyNetClient* lpNetClient = (CecilyNetClient*)lpCtx->parent;
	if ( nread < 0 )
	{
		if (nread == UV_EOF) {

		} else if (nread == UV_ECONNRESET) {

		} else {
		}
		if ( NULL != lpNetClient )
		{
			lpNetClient->Close();
			return ;
		}
	}
	if ( nread > 0 )
	{
		if ( NULL!= lpNetClient && NULL != lpNetClient->m_lpFuncTCPServerRecv )
		{

			NetPackage* lpPackage = new NetPackage;
			lpPackage->req.data = lpNetClient;
			lpPackage->buf.base = buf->base;
			lpPackage->buf.len = buf->len;
			lpPackage->nClientID = lpNetClient->m_nClientID;
			lpPackage->nRead = nread;

			lpNetClient->AddRecvPackage(lpPackage);

// 			DWORD dwTicount1 = GetTickCount();
// 			lpNetClient->DeSerialize(lpNetClient->m_nClientID,buf->base,nread);
// 			DWORD dwTicount2 = GetTickCount();
//			DWORD dwMill = dwTicount2 - dwTicount1;
			
			
			//lpNetClient->m_lpFuncTCPServerRecv(lpNetClient->m_nClientID,(char*)buf->base,buf->len,lpNetClient->m_lpTCPServerRecvUserData);
		}
		
	}

}

void CecilyNetClient::AfterSend(uv_write_t* req, int status)
{
	CecilyNetClient* lpNetClient = (CecilyNetClient*)req->data;
	UVTCPWriteParam* lpParam = (UVTCPWriteParam*)req;
	if (status < 0) {
		//发送失败，加到队列缓冲区头部，继续发送
		lpNetClient->ReSend(lpParam);
		lpNetClient->PostThreadSendSem();
		return;
	}
	//发送成功，通知队列缓冲区，释放内存，移除出队列
	delete lpParam;
	lpParam = NULL;
	lpNetClient->PostThreadSendSem();
	lpNetClient->MakeSureSend(NULL);
}

void CecilyNetClient::AsyncCallBack(uv_async_t* handle)
{
	CecilyNetClient* lpNetClient = (CecilyNetClient*)handle->data;
	lpNetClient->MakeSureSend(NULL);
}

void CecilyNetClient::AsyncThreadCallBack(uv_async_t* handle)
{
	CecilyNetClient* lpNetClient = (CecilyNetClient*)handle->data;
	lpNetClient->MakeSureDeSerialize();

}

void CecilyNetClient::ReSend(UVTCPWriteParam* lpParam)
{
	if ( NULL != lpParam )
	{
		CUVAutoLock locker(&m_writeMutex);
		m_queueWriteParam.push_front(lpParam);
		MakeSureSend(NULL);
	}
}

void CecilyNetClient::AfterRecvFullPackage(int nClientID, char* lpBuffer, int nlen,void* user_data)
{
	CecilyNetClient* lpNetClient = (CecilyNetClient*)user_data;
	if ( NULL != lpNetClient )
	{	
		if ( NULL != lpNetClient->m_lpFuncTCPServerRecv )
		{
			lpNetClient->m_lpFuncTCPServerRecv(nClientID,lpBuffer,nlen,lpNetClient->m_lpTCPServerRecvUserData);
		}
	}
}

void CecilyNetClient::ParsePackage(uv_work_t *req)
{
	NetPackage* lpPackage = (NetPackage*)req;
	CecilyNetClient* lpNetClient = (CecilyNetClient*)lpPackage->req.data;
	if ( NULL != lpNetClient )
	{
		{
			{
				lpNetClient->DeSerialize(lpNetClient->m_nClientID,lpPackage->buf.base,lpPackage->nRead);
				PackageStatus status = lpNetClient->GetPackageStatus();
				if ( status != S_PackageOK )
				{
					lpNetClient->AddNotFreeMemory(lpPackage);
				}else
				{
					lpNetClient->FreeMemory(lpPackage);
					//这个NetPackage的内存不能在此处释放，后续的回调还需要用到
					lpNetClient->AddNotFreeMemory(lpPackage);
				}	
				lpNetClient->PostThreadRecvSem();				
			}
		}

	}
	int a = 10;

}

void CecilyNetClient::AfterParsePackage(uv_work_t* req, int status)
{
	NetPackage *lpPackage = (NetPackage*)req;
	CecilyNetClient* lpNetClient = (CecilyNetClient*)req->data;
	if ( NULL != lpNetClient )
	{
		lpNetClient->ResetThreadStatus();
	}
	if ( status )
	{
		lpNetClient->MakeSureDeSerialize();
		return ;
	}
	lpNetClient->MakeSureDeSerialize();

}

void CecilyNetClient::SendPackage(uv_work_t* req)
{
	UVTCPWriteParam* lpWriteParam = (UVTCPWriteParam*)req->data;
	if ( NULL == lpWriteParam )
	{
		return ;
	}
	CecilyNetClient* lpNetClient = (CecilyNetClient*)lpWriteParam->write_req.data;
	int iret = uv_write((uv_write_t*)&lpWriteParam->write_req, (uv_stream_t*)&lpNetClient->GetUVTCPClientCtx()->tcpHandle, &lpWriteParam->write_buf, 1, AfterSend);
}

void CecilyNetClient::AfterSendPackage(uv_work_t* req, int status)
{
// 	NetPackage* lpPackage = (NetPackage*)req;
// 	if ( NULL != lpPackage )
// 	{
// 		delete lpPackage;
// 		lpPackage = NULL;
// 	}

}

void CecilyNetClient::AddRecvPackage(NetPackage* lpPackage)
{
	if ( NULL == lpPackage )
	{
		return ;
	}
	{
		CUVAutoLock locker(&m_recvpackageMutex);
		m_queueRecvPackage.push(lpPackage);
		if ( !GetThreadStatus() )
		{
			uv_async_send(&m_async_thread);
		}
		
	}
}

void CecilyNetClient::SetThreadStatus()
{
	{
		CUVAutoLock locker(&m_ThreadStatusMutex);
		m_bThreadRunning = true;
	}
}

void CecilyNetClient::ResetThreadStatus()
{
	{
		CUVAutoLock locker(&m_ThreadStatusMutex);
		m_bThreadRunning = false;
	}
}

bool CecilyNetClient::GetThreadStatus()
{
	{
		CUVAutoLock locker(&m_ThreadStatusMutex);
		return m_bThreadRunning;
	}
}