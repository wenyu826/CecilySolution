/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/28
*  File:           CecilyAMQPMessage.h
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#pragma once
#include <string>
using namespace std;

class CCecilyMemoryPool;

class CCecilyAMQPMessage
{
public:
	CCecilyAMQPMessage(CCecilyMemoryPool* lpMemoryPool = NULL);
	virtual ~CCecilyAMQPMessage(void);

public:
	bool ApplyMemorySpace(int nMemorySize);
	bool WriteMessageBuffer(void *lpBuffer, int nBufferSize);
	bool SetQueueName(char* lpQueueName);
	bool SetMessageID(char* szMessagID);
	unsigned int GetMessageBufferSize()
	{
		return m_nBufferSize;
	}
	string GetQueueName()
	{
		return m_strQueueName;
	}
	string GetMessageID()
	{
		return m_strMessageID;
	}
	char* GetMessageBufferPtr()
	{
		return m_lpMessageBuffer;
	}

private:
	CCecilyMemoryPool * m_lpMemoryPool;
	string m_strQueueName;
	string m_strMessageID;
	char *m_lpMessageBuffer;
	unsigned int m_nBufferSize;
};
