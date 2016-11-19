/********************************************************************
	created:	  2016/10/20
	filename: 	  CecilyActiveMQManager.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_ActiveMQ_Manager_H
#define Cecily_ActiveMQ_Manager_H
#include "CecilyMQ.h"
#include "CecilyActiveMQHelp.h"
#include "CecilyCMSTemplate.h"
#include "CecilyMemoryPool.h"
#include "CecilyTextMessageCreator.h"
#include "CecilyBytesMessageCreator.h"
#include "CecilyActiveMQProxy.h"







class CecilyActiveMQManager
{
public:
	CecilyActiveMQManager(void);
	virtual ~CecilyActiveMQManager(void);

	static void InitActiveMQ();
	static void UnInitActiveMQ();

	bool ConnectToMQ(const char* lpAddress, int nPort,const char* username, const char* password);
	void SetRecvCB(clear_mq_receive lpFunc)
	{
		m_lpCecilyMQReceive = lpFunc;
	}
	bool OpenMQ(const char* queue, int nType);
	bool PutMessage(const char* queue, void*lpBuffer, int nLen, int nMessageType, int nType);
	bool DeleteMQ(const char* queue, bool bPubSub);

private:
	static void Received_Message_CB(CecilyMessage* lpMessage, void* user_data);
	CCecilyMemoryPool* GetMemoryPool()
	{
		return m_lpMemoryPool;
	}


private:
	static bool bInit;
	std::string		m_strBrokerURL;
	CecilyCMSTemplate*	m_TopicConsumer;
	CecilyCMSTemplate*	m_QueueConsumer;
	CecilyCMSTemplate*	m_TopicProduer;
	CecilyCMSTemplate*	m_QueueProducer;
	ActiveMQConnectionFactory* m_lpConnectionFactory;
	clear_mq_receive m_lpCecilyMQReceive;

	CCecilyMemoryPool* m_lpMemoryPool;

};


#endif