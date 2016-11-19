/********************************************************************
	created:	  2016/10/20
	filename: 	  ClearActiveMQManager.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Clear_ActiveMQ_Manager_H
#define Clear_ActiveMQ_Manager_H
#include "ClearActiveMQ.h"
#include "ClearActiveMQHelp.h"
#include "ClearCMSTemplate.h"
#include "ClearMemoryPool.h"
#include "ClearTextMessageCreator.h"
#include "ClearBytesMessageCreator.h"



#define Clear_Default_Toptic		"Topic.Clear"
#define Clear_Default_Queue			"Queue.Clear"



class ClearActiveMQManager
{
public:
	ClearActiveMQManager(void);
	virtual ~ClearActiveMQManager(void);

	static void InitActiveMQ();
	static void UnInitActiveMQ();

	bool ConnectToMQ(const char* lpAddress, int nPort,const char* username, const char* password);
	void SetRecvCB(clear_mq_receive lpFunc)
	{
		m_lpClearMQReceive = lpFunc;
	}
	bool OpenMQ(const char* queue, int nType);
	bool PutMessage(const char* queue, void*lpBuffer, int nLen, int nMessageType, int nType);
	bool DeleteMQ(const char* queue, bool bPubSub);

private:
	static void Received_Message_CB(ClearMessage* lpMessage, void* user_data);
	CClearMemoryPool* GetMemoryPool()
	{
		return m_lpMemoryPool;
	}


private:
	static bool bInit;
	std::string		m_strBrokerURL;
	ClearCMSTemplate*	m_TopicConsumer;
	ClearCMSTemplate*	m_QueueConsumer;
	ClearCMSTemplate*	m_TopicProduer;
	ClearCMSTemplate*	m_QueueProducer;
	ActiveMQConnectionFactory* m_lpConnectionFactory;
	clear_mq_receive m_lpClearMQReceive;

	CClearMemoryPool* m_lpMemoryPool;

};


#endif