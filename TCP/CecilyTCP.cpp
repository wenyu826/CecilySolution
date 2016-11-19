/********************************************************************
	created:	  2016/10/12
	filename: 	  CecilyTCP.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "stdafx.h"
#include "CecilyTCPManager.h"

CecilyTCPManager* lpTCPMgr = CecilyTCPManager::GetInstance();

//服务端
CLEAR_TCP_API bool StartServer(const char* lpAddress, int nPort)
{
	if ( NULL == lpTCPMgr )
	{
		return false;
	}
	return lpTCPMgr->StartServer(lpAddress,nPort);
}

CLEAR_TCP_API void StopServer()
{
	if ( NULL == lpTCPMgr )
	{
		return ;
	}
	lpTCPMgr->StopServer();
}

CLEAR_TCP_API bool SendDataToClient(int nClientID,const char* lpData, int nlen)
{
	if ( NULL == lpTCPMgr )
	{
		return false;
	}
	return lpTCPMgr->SendDataToClient(nClientID,lpData,nlen);
}

CLEAR_TCP_API void SetServerRecvCallBack(RecvDataFromClient lpFunc,void* user_data)
{
	if ( NULL == lpTCPMgr )
	{
		return;
	}
	lpTCPMgr->SetServerRecvCallBack(lpFunc,user_data);
}
CLEAR_TCP_API void SetNotifyServerStatusCallBack(NotifyServerStatus lpFunc,void* user_data)
{
	if ( NULL == lpTCPMgr )
	{
		return ;
	}
	lpTCPMgr->SetNotifyServerStatusCallBack(lpFunc,user_data);
}
//

//客户端
CLEAR_TCP_API bool ConnectToServer(const char* lpAddress, int nPort)
{
	if ( NULL == lpTCPMgr )
	{
		return false;
	}
	return lpTCPMgr->ConnectToServer(lpAddress,nPort);
}

CLEAR_TCP_API bool SendDataToServer(const char* lpBuffer, int nlen,bool bSeralize)
{
	if ( NULL == lpTCPMgr )
	{
		return false;
	}
	return lpTCPMgr->SendDataToServer(lpBuffer,nlen,bSeralize);
}

CLEAR_TCP_API void SetClientRecvCallBack(RecvDataFromServer lpFunc,void* user_data)
{
	if ( NULL == lpTCPMgr )
	{
		return ;
	}
	lpTCPMgr->SetClientRecvCallBack(lpFunc,user_data);
}

CLEAR_TCP_API void Disconnect()
{
	if ( NULL == lpTCPMgr )
	{
		return ;
	}
	lpTCPMgr->Disconnect();
}






