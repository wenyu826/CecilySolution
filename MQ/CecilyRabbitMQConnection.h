/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/27
*  File:           CecilyMQConnection.h
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#pragma once
#include <string>
#include "CecilyRabbitMQProxy.h"
#include "AssistUtility.h"
using namespace std;
typedef struct CLEARAMQPOBJECT
{
	amqp_socket_t *clear_socket;
	amqp_connection_state_t clear_conn;
	int nChannel;
	CLEARAMQPOBJECT()
	{
		clear_socket = NULL;
		clear_conn = NULL;
		nChannel = -1;
	}
	~CLEARAMQPOBJECT()
	{
		if ( NULL != clear_conn )
		{
			amqp_channel_close(clear_conn,nChannel,AMQP_REPLY_SUCCESS);
			amqp_connection_close(clear_conn, AMQP_REPLY_SUCCESS);
			amqp_destroy_connection(clear_conn);
			clear_socket = NULL;
			clear_conn = NULL;
		}
	}

}CecilyAmqpObject;

class CCecilyRabbitMQConnection
{
public:
	CCecilyRabbitMQConnection(const char* lpIpAddress, const char* lpVirtualHost, unsigned int nPor , const char* lpUser,const char* lpPassword);
	virtual ~CCecilyRabbitMQConnection(void);

public:
	bool Init();
	CecilyMQStatus TryConnect();
	CecilyMQStatus TryDisconnect();
	CecilyAmqpObject* GetPutObject()
	{
		return m_putObject;
	}
	CecilyAmqpObject* GetGetObject()
	{
		return m_getObject;
	}
	string GetServerIP()
	{
		return m_strIP;
	}

	void AddConsumer(string strConsumer)
	{
		m_vecConsumerQueues.push_back(strConsumer);
	}

	void AddExchangeQueue(AMQPExchangeQueue eq)
	{
		m_vecExchangeQueues.push_back(eq);
	}
	vector<AMQPExchangeQueue> GetExchangeQueue()
	{
		return m_vecExchangeQueues;
	}
	/*兼容老版本API*/
	void MQ_SendMessag(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason);
	void MQ_Publish(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason);
	void SetHandleID(int nhconn)
	{
		m_lphandle = nhconn;
	}
	long GetHandle()
	{
		return m_lphandle;
	}
	/*兼容老版本API*/
	void StopConsumer()
	{
		{
			CSmartLocker locker(&m_ConsumerLocker,TRUE);
			m_bStopConsumer = true;
		}
		
	}
	void ResetConsumer()
	{
		{
			CSmartLocker locker(&m_ConsumerLocker,TRUE);
			m_bStopConsumer = false;
		}
	}
	bool IsStopConsumer()
	{
		{
			CSmartLocker locker(&m_ConsumerLocker,TRUE);
			return m_bStopConsumer;
		}
	}

	bool IsConnected()
	{
		return m_bConnect;
	}

	bool AddCacheMessage(CCecilyAMQPMessage* lpMessage)
	{
		if ( NULL == lpMessage )
		{
			return false;
		}
		CSmartLocker locker(&m_cacheLocker,TRUE);
		m_CacheMessage.push(lpMessage);
		m_nCacheDepth = m_nCacheDepth +1;
		ReleaseSemaphore(m_connEventHandles[0] ,  1,  NULL);
		return true;
	}

	int GetCacheDepth()
	{
		return m_nCacheDepth;
	}

	CCecilyAMQPMessage* GetCacheMessage()
	{
		CSmartLocker locker(&m_cacheLocker,TRUE);
		CCecilyAMQPMessage* tmpMessage = NULL;
		if ( m_CacheMessage.size() > 0 )
		{
			tmpMessage = m_CacheMessage.front();
			m_CacheMessage.pop();
			m_nCacheDepth = m_nCacheDepth -1;
			if ( m_nCacheDepth < 0 )
			{
				m_nCacheDepth = 0;
			}
		}
		return tmpMessage;
	}
public:
	HANDLE m_connEventHandles[AMQP_HANDLE_EVENTS];

private:
	long			m_lphandle;
	CecilyAmqpObject	*m_putObject;	
	CecilyAmqpObject	*m_getObject;	
	bool			m_bInit;
	bool			m_bConnect;
	vector<string> m_vecConsumerQueues;
	vector<AMQPExchangeQueue> m_vecExchangeQueues;
	string			m_strIP;              
	string			m_strUser;
	string			m_strPwd;
	unsigned int	m_nPort;
	string			m_strVirtualHost;
	bool			m_bStopConsumer;
	CLocker			m_ConsumerLocker;

	CLocker			m_cacheLocker;
	queue<CCecilyAMQPMessage*>	m_CacheMessage;     //缓存到链接里面的队列
	int m_nCacheDepth;
	

};
