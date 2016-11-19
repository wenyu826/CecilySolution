/********************************************************************
	created:	  2016/10/10
	filename: 	  CecilyTCPManager.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "CecilyTCPManager.h"

CecilyTCPManager* CecilyTCPManager::m_lpTCPMgrInstance = NULL;
unsigned int CecilyTCPManager::m_nOnlineUser = 0;

CecilyTCPManager::CecilyTCPManager(void)
{
	m_lpMgrPool = CCecilyMemoryPool::GetInstance();
	m_lpTCPServer = new CecilyUVTCPServer();
	m_lpTCPClient = new CecilyUVTCPClient();
	CecilyUVTCPServer::SetMemoryPool(m_lpMgrPool);
	CecilyUVTCPClient::SetMemoryPool(m_lpMgrPool);
	CecilyNetClient::SetMemoryPool(m_lpMgrPool);
	CecilyDataSerialize::SetMemoryPool(m_lpMgrPool);
	m_lpFuncUpdateOnLineUsers = NULL;
	m_lpFuncNotifyServerStatus = NULL;
	m_lpFuncRecvDataFromServer = NULL;
	m_lpFuncRecvDataFromClient = NULL;
}

CecilyTCPManager::~CecilyTCPManager(void)
{
	if ( NULL != m_lpTCPServer )
	{
		delete m_lpTCPServer;
		m_lpTCPServer = NULL;
	}
	CCecilyMemoryPool::ReleaseInstance();
}

void CecilyTCPManager::SetUpdateOnlineUsers(UpdateOnlineUsers lpFunc)
{
	m_lpFuncUpdateOnLineUsers = lpFunc;
}

bool CecilyTCPManager::StartServer(const char* lpAddress, int Port)
{
	if ( NULL == lpAddress || 0 == strlen(lpAddress) || Port <= 0 )
	{
		return false;
	}
	m_nOnlineUser = 0;
	m_lpTCPServer->SetNewConnectCallBack(ServerNewConnectCallBack,m_lpTCPServer);
	m_lpTCPServer->SetCloseCallBack(ServerClientClose,m_lpTCPServer);
	return m_lpTCPServer->Start(lpAddress,Port);
}

void CecilyTCPManager::StopServer()
{
	if ( NULL != m_lpTCPServer )
	{
		m_lpTCPServer->Close();
	}
	m_nOnlineUser = 0;
}

bool CecilyTCPManager::SendDataToClient(int nClientID, const char* lpBuffer, int nLen)
{
	if ( nClientID < 0 ||  NULL == lpBuffer || nLen <= 0 )
	{
		return false;
	}
	if ( NULL != m_lpTCPServer )
	{
		m_lpTCPServer->SendData(nClientID,lpBuffer,nLen);
	}
	return true;
}

//接收客户端发来的消息回调
void CecilyTCPManager::ServerRecvCallBack(int nCliend, char* buf,int nRead, void* user_data)
{
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)user_data;
	CecilyTCPManager* lpMgr = CecilyTCPManager::GetInstance();
	if ( NULL != lpMgr )
	{
		if ( NULL != lpMgr->m_lpFuncRecvDataFromClient )
		{
			lpMgr->m_lpFuncRecvDataFromClient(nCliend,buf,nRead,lpMgr->m_lpRecvDataFromClientUserData);
		}
	}
// 	TRACE(buf);
// 	TRACE("\n");
// 	if ( NULL != lpServer )
// 	{
// 		string strMsg = "这是测试文本";
// 		lpServer->SendData(nCliend,strMsg.c_str(),strlen(strMsg.c_str()));
// 	}	
}

//新客户端接入的回调
void CecilyTCPManager::ServerNewConnectCallBack(int clientid, void* userdata)
{
	m_nOnlineUser++;
	int nNum = m_nOnlineUser;
	CecilyUVTCPServer* lpServer = (CecilyUVTCPServer*)userdata;
	if ( NULL == lpServer )
	{
		return ;
	}
	lpServer->SetRecvCallBack(clientid,ServerRecvCallBack,lpServer);
	CecilyTCPManager* lpMgr = CecilyTCPManager::GetInstance();
	if ( NULL != lpMgr )
	{
		if ( NULL != lpMgr->m_lpFuncUpdateOnLineUsers )
		{
			lpMgr->m_lpFuncUpdateOnLineUsers(m_nOnlineUser);
		}
		if ( NULL != lpMgr->m_lpFuncNotifyServerStatus )  //通知dll调用者
		{
			lpMgr->m_lpFuncNotifyServerStatus(clientid,true,lpMgr->m_lpNotifyUserData);
		}
	}
}

//有客户端断开连接的回调 nClientid = -1 表示服务停止
void CecilyTCPManager::ServerClientClose(int nClientid,void* userdata)
{
	if ( nClientid != -1 )
	{
		m_nOnlineUser--;
	}
	CecilyTCPManager* lpMgr = CecilyTCPManager::GetInstance();
	if ( NULL != lpMgr )
	{
		if ( NULL != lpMgr->m_lpFuncUpdateOnLineUsers )
		{
			lpMgr->m_lpFuncUpdateOnLineUsers(m_nOnlineUser);
		}
		if ( NULL != lpMgr->m_lpFuncNotifyServerStatus )    //通知dll调用者
		{
			lpMgr->m_lpFuncNotifyServerStatus(nClientid,0,lpMgr->m_lpNotifyUserData);
		}
// 		if ( NULL != lpMgr->m_lpTCPServer )
// 		{
// 			lpMgr->m_lpTCPServer->RemoveClient(nClientid);
// 
// 		}
	}

}

bool CecilyTCPManager::ConnectToServer(const char* lpAddress, int nPort)
{
	if ( NULL == lpAddress || nPort < 0 || NULL == m_lpTCPClient)
	{
		return false;
	}
	m_lpTCPClient->SetRecvCallBack(ClientRecvCallBack,m_lpTCPClient);
	m_lpTCPClient->SetCloseCallBack(ClientCloseCallBack,m_lpTCPClient);
	return m_lpTCPClient->Connect(lpAddress,nPort);
}

bool CecilyTCPManager::SendDataToServer(const char*lpBuffer, int nLen,bool bSerialize)
{
	if ( NULL == lpBuffer || nLen <= 0 )
	{
		return false;
	}
	return m_lpTCPClient->Send(lpBuffer,nLen,bSerialize);
}

void CecilyTCPManager::Disconnect()
{
	if ( NULL == m_lpTCPClient )
	{
		return ;
	}
	m_lpTCPClient->Close();
}

void CecilyTCPManager::ClientRecvCallBack(char* buf, int nSize,void *userData)
{
	CecilyUVTCPClient* lpTCPClient = (CecilyUVTCPClient*)userData;
	if ( NULL == lpTCPClient )
	{
		return ;
	}
	CecilyTCPManager* lpMgr = CecilyTCPManager::GetInstance();
	if ( NULL != lpMgr )
	{
		if ( NULL != lpMgr->m_lpFuncRecvDataFromServer)
		{
			lpMgr->m_lpFuncRecvDataFromServer(buf,nSize,lpMgr->m_lpRecvDataFromServerUserData);
		}
	}

}

void CecilyTCPManager::ClientCloseCallBack(int clientid, void* userdata)
{
	CecilyUVTCPClient* lpTCPClient = (CecilyUVTCPClient*)userdata;
	if ( NULL == lpTCPClient )
	{
		return ;
	}
	lpTCPClient->Close();
}

void CecilyTCPManager::SetServerRecvCallBack(RecvDataFromClient lpFunc,void* user_data)
{
	m_lpFuncRecvDataFromClient = lpFunc;
	m_lpRecvDataFromClientUserData = user_data;
}

void CecilyTCPManager::SetNotifyServerStatusCallBack(NotifyServerStatus lpFunc,void* user_data)
{
	m_lpFuncNotifyServerStatus = lpFunc;
	m_lpNotifyUserData = user_data;
}

void CecilyTCPManager::SetClientRecvCallBack(RecvDataFromServer lpFunc,void* user_data)
{
	m_lpFuncRecvDataFromServer = lpFunc;
	m_lpRecvDataFromServerUserData = user_data;
}