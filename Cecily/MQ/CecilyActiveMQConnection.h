/********************************************************************
	created:	  2016/11/01
	filename: 	  CecilyActiveMQConnection.h
	author:		  ������
	E-Mail��	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_ActiveMQ_Connection_H
#define Cecily_ActiveMQ_Connection_H
#include "CecilyMQ.h"
#include "CecilyActiveMQHelp.h"
#include "CecilyCMSTemplate.h"
#include "SmartLocker.h"
#include "CecilyMemoryPool.h"
#include <queue>

using namespace std;

typedef struct _MessageInfo
{
	CecilyMQMD*  lpMQMD;
	void*  buffer;
	int	   nlen;
}MessageInfo;

class CecilyActiveMQConnection
{
public:
	CecilyActiveMQConnection(CCecilyMemoryPool* lpMemoryPool, const string& strBroker, const string& user, const string& password);
	virtual ~CecilyActiveMQConnection();

	
	void InitCMSTemplates();
	void MQ_SendMessage(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason);
	void MQ_Publish(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason);

	void SetRecvCallBack(RecvCallBack lpfunc,void* user_data);
	RecvCallBack GetRecvCallBack()
	{
		return m_lpRecvCallBack;
	}
	void* GetRecvCBUserData()
	{
		return m_lpRecvCBUserData;
	}

	void OpenMQ(const char* queue, int nType);   // 0 Ϊ pubsub  ��0 λ ����


	void SetHandle(long hConn)
	{
		m_handle = hConn;
	}
	
	long GetHandle()
	{
		return m_handle;
	}

private:
	void SetMQConnectionFactory(const string& strBroker, const string& user, const string& password);
	static void Received_Message_CB(CecilyMessage* lpMessage, void* user_data);
	CCecilyMemoryPool* GetMemoryPool()
	{
		return m_ConnectionMemoryPool;
	}

	void EnQueue(MessageInfo* lpInfo);
	MessageInfo* DeQueue();



private:
	long	m_handle;
	std::string		m_strBrokerURL;
	CecilyCMSTemplate*	m_TopicConsumer;
	CecilyCMSTemplate*	m_QueueConsumer;
	CecilyCMSTemplate*	m_TopicProduer;
	CecilyCMSTemplate*	m_QueueProducer;
	CCecilyMemoryPool*	m_ConnectionMemoryPool;
	ActiveMQConnectionFactory* m_lpConnectionFactory;

	RecvCallBack	m_lpRecvCallBack;			//�Ե����������õĻص�
	void* m_lpRecvCBUserData;

	CLocker		m_csQueueLocker;
	queue<MessageInfo*>	m_queueMessages;		//�Ѿ����յ�����Ϣ���ݻ��棬��û�����ûص���ʱ�򶼴����������ս���˳�������

	
	
};

#endif
