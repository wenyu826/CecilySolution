/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/28
*  File:           CecilyAMQPMessage.cpp
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#include "StdAfx.h"
#include "CecilyAMQPMessage.h"
#include "CecilyMemoryPool.h"

CCecilyAMQPMessage::CCecilyAMQPMessage(CCecilyMemoryPool* lpMemoryPool)
{
	m_lpMemoryPool = lpMemoryPool;
	m_lpMessageBuffer = NULL;
	m_nBufferSize = 0;
}

CCecilyAMQPMessage::~CCecilyAMQPMessage(void)
{
	if ( NULL == m_lpMemoryPool )
	{
		delete []m_lpMessageBuffer;
		m_lpMessageBuffer = NULL;
	}else
	{
		m_lpMemoryPool->Deallocate(m_lpMessageBuffer,m_nBufferSize);
	}
}

bool CCecilyAMQPMessage::ApplyMemorySpace(int nMemroySize)
{
	try
	{
		if ( NULL == m_lpMemoryPool )
		{
			m_lpMessageBuffer = new char[nMemroySize];
			m_nBufferSize = nMemroySize;
		}
		else
		{
			m_lpMessageBuffer = (char*)m_lpMemoryPool->Allocate(nMemroySize);
			m_nBufferSize = nMemroySize;
		}
	}
	catch(...)
	{
		return false;
	}
	return true;	
}

bool CCecilyAMQPMessage::WriteMessageBuffer(void *lpBuffer, int nBufferSize)
{
	if ( NULL == lpBuffer || nBufferSize <= 0 )
	{
		return false;
	}
	errno_t t =  memcpy_s(m_lpMessageBuffer,m_nBufferSize,lpBuffer,nBufferSize);
	if ( t == 0 )
	{
		return true;
	}
	return false;
}

bool CCecilyAMQPMessage::SetQueueName(char* lpQueueName)
{
	if ( NULL == lpQueueName )
	{
		return false;
	}
	m_strQueueName = lpQueueName;
	return true;
}

bool CCecilyAMQPMessage::SetMessageID(char* szMessagID)
{
	if ( NULL == szMessagID )
	{
		return false;
	}
	m_strMessageID = szMessagID;
	return true;
}