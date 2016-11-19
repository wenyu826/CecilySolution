/********************************************************************
	created:	  2016/10/24
	filename: 	  ClearActiveMQ.cpp
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
// ClearActiveMQ.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ClearActiveMQ.h"
#include "ClearActiveMQManager.h"

ClearActiveMQManager* g_lpMQManager =  new ClearActiveMQManager();


ACTIVEME_API bool  ConnectToMQ(const char* lpAddress, int nPort,const char* username, const char* password)
{
	if ( NULL == lpAddress || nPort <= 0 )
	{
		return false;
	}
	return g_lpMQManager->ConnectToMQ(lpAddress,nPort,username,password);
}

ACTIVEME_API void SetReceiveCallBack(clear_mq_receive lpFunc)
{
	if ( NULL == lpFunc)
	{
		return ;
	}
	g_lpMQManager->SetRecvCB(lpFunc);
}

ACTIVEME_API bool OpenMQ(const char* queue, int nType /* =1 */ )
{
	return g_lpMQManager->OpenMQ(queue,nType);
}

ACTIVEME_API bool  DeleteMQ(const char* queue, bool bPubSub)
{
	return g_lpMQManager->DeleteMQ(queue,bPubSub);
}

ACTIVEME_API bool PutMessage(const char* queue, void*lpBuffer, int nLen, int nMessageType, int nType)
{
	if ( NULL == queue || lpBuffer == NULL || nLen <= 0 )
	{
		return false;
	}
	return g_lpMQManager->PutMessage(queue,lpBuffer,nLen,nMessageType,nType);
}