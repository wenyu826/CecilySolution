/********************************************************************
	created:	  2016/10/20
	filename: 	  CecilyActiveMQManager.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "CecilyActiveMQManager.h"
#include "CecilyMemoryPool.h"


bool CecilyActiveMQManager::bInit = false;


CecilyActiveMQManager::CecilyActiveMQManager(void):
m_TopicConsumer(NULL),
m_TopicProduer(NULL),
m_QueueConsumer(NULL),
m_QueueProducer(NULL),
m_lpConnectionFactory(NULL),
m_lpMemoryPool(NULL)
{
	try{
		if ( NULL == m_lpMemoryPool )
		{
			m_lpMemoryPool =  new CCecilyMemoryPool();
		}		
		CecilyActiveMQManager::InitActiveMQ();
	}
	catch(...)
	{

	}
	
}

CecilyActiveMQManager::~CecilyActiveMQManager(void)
{
	try
	{
		m_lpCecilyMQReceive = NULL;
		if ( NULL != m_lpMemoryPool )
		{
			delete m_lpMemoryPool;
			m_lpMemoryPool = NULL;
		}
		Cecily_Delete_Ptr(m_TopicConsumer);
		Cecily_Delete_Ptr(m_TopicProduer);
		Cecily_Delete_Ptr(m_QueueConsumer);
		Cecily_Delete_Ptr(m_QueueProducer);	
		Cecily_Delete_Ptr(m_lpConnectionFactory);
		CecilyActiveMQManager::UnInitActiveMQ();
	}
	catch(...)
	{

	}

}


void CecilyActiveMQManager::InitActiveMQ()
{
	if ( !bInit )
	{
		activemq::library::ActiveMQCPP::initializeLibrary();
		bInit = true;
	}
	
}

void CecilyActiveMQManager::UnInitActiveMQ()
{
	if ( bInit )
	{
		activemq::library::ActiveMQCPP::shutdownLibrary();
		bInit = false;
	}
	
}

void CecilyActiveMQManager::Received_Message_CB(CecilyMessage* lpMessage, void* user_data)
{
	CecilyActiveMQManager* lpMQManager = (CecilyActiveMQManager*)user_data;
	if ( NULL == lpMQManager )
	{
		if ( NULL != lpMessage )
		{
			delete lpMessage;
			lpMessage = NULL;
		}
	}
	const char* lpDestination = (lpMessage->strDestionNation).c_str();
	int nType = lpMessage->nType;
	Message* message = lpMessage->message;
// 	activemq::commands::ActiveMQMessage* lpTmpMessage = (activemq::commands::ActiveMQMessage* )message;
// 	string strType = lpTmpMessage->getCMSType();
	const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
	//暂时先只支持 Text 类型
	if ( NULL == textMessage )
	{
		delete lpMessage;
		lpMessage = NULL;
		return ;
	}
	string strtext = textMessage->getText();
	string strGUID1 = textMessage->getCMSMessageID();
	int nlen = strtext.length();
	CCecilyMemoryPool* lpMemoryPool = lpMQManager->GetMemoryPool();
	void * lpBuffer = (void*)lpMemoryPool->Allocate(nlen);
	memcpy_s(lpBuffer,nlen,strtext.c_str(),nlen);
	if ( NULL != lpMQManager->m_lpCecilyMQReceive )  
	{
		lpMQManager->m_lpCecilyMQReceive(lpDestination,lpBuffer,nlen,nType);
	}

	const BytesMessage* byteMessage = dynamic_cast<const BytesMessage*> (message);
	if ( NULL != byteMessage )
	{
		unsigned char* bytesPtr = byteMessage->getBodyBytes();
		//需要释放内存, 回调

		delete []bytesPtr;
		bytesPtr = NULL;
	}
	message->acknowledge();
	
	delete lpMessage;
	lpMessage = NULL;
}

bool CecilyActiveMQManager::ConnectToMQ(const char* lpAddress, int nPort,const char* username, const char* password)
{
	if ( NULL == lpAddress || nPort <= 0   )
	{
		return false;
	}
	char szBrokerURI[256] = {0};
	try
	{
		sprintf_s(szBrokerURI,256,"tcp://%s:%d",lpAddress,nPort);
		ConnectionFactory* connectionFactory = ConnectionFactory::createCMSConnectionFactory(szBrokerURI);
		cms::Connection* connection = NULL;
		connection = connectionFactory->createConnection();
		if ( NULL == connection )
		{
			return false;
		}
		cms::Session* session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
		if ( NULL == session )
		{
			return false;
		}
		Cecily_Delete_Ptr(session);
		Cecily_Delete_Ptr(connection);
		Cecily_Delete_Ptr(connectionFactory);

	}
	catch(CMSException& ex)
	{
		return false;
	}
	catch(...)
	{
		return false;
	}
	memset(szBrokerURI,0,256);
	sprintf_s(szBrokerURI,256,"failover:(tcp://%s:%d)",lpAddress,nPort);
	try
	{
		if ( NULL == m_lpConnectionFactory )
		{
			m_lpConnectionFactory = new ActiveMQConnectionFactory(szBrokerURI,username,password);
		}
		if ( NULL == m_lpConnectionFactory )
		{
			return false;
		}
		m_lpConnectionFactory->setUsername(username);
		m_lpConnectionFactory->setPassword(password);
		if ( NULL == m_TopicConsumer )
		{
			m_TopicConsumer = new CecilyCMSTemplate();
			//属性测试设置
			m_TopicConsumer->setSessionAcknowledgeMode(Session::INDIVIDUAL_ACKNOWLEDGE);		//单挑消息确认，需要调用message.acknowledge()
			//
			m_TopicConsumer->setConnectionFactory(m_lpConnectionFactory);
			m_TopicConsumer->setDefaultDestinationName(Cecily_Default_Toptic);
			m_TopicConsumer->setPubSubDomain(true);
			m_TopicConsumer->Async_Receive(Cecily_Default_Toptic);
		}
		if ( NULL == m_TopicProduer )
		{
			m_TopicProduer = new CecilyCMSTemplate();
			m_TopicProduer->setConnectionFactory(m_lpConnectionFactory);
			m_TopicProduer->setDefaultDestinationName(Cecily_Default_Toptic);
			//属性测试设置
			m_TopicProduer->setExplicitQosEnabled(true);
			m_TopicProduer->setTimeToLive(300*1000);   //消息生命周期  毫秒   this is only used when "isExplicitQosEnabled" equals "true"
			m_TopicProduer->setPriority(6);   //设置优先级   this is only used when "isExplicitQosEnabled" equals "true"
			m_TopicProduer->setDeliveryMode(cms::DeliveryMode::PERSISTENT);  //设置消息持久性  this is only used when "isExplicitQosEnabled" equals "true"
			//
			m_TopicProduer->setPubSubDomain(true);
		}
		if ( NULL == m_QueueConsumer )
		{
			m_QueueConsumer = new CecilyCMSTemplate();
			m_QueueConsumer->setSessionAcknowledgeMode(Session::INDIVIDUAL_ACKNOWLEDGE);
			m_QueueConsumer->setConnectionFactory(m_lpConnectionFactory);
			m_QueueConsumer->setDefaultDestinationName(Cecily_Default_Queue);
			m_QueueConsumer->setPubSubDomain(false);
			m_QueueConsumer->Async_Receive(Cecily_Default_Queue);
		}
		if ( NULL == m_QueueProducer )
		{
			m_QueueProducer = new CecilyCMSTemplate();
			m_QueueProducer->setConnectionFactory(m_lpConnectionFactory);
			m_QueueProducer->setDefaultDestinationName(Cecily_Default_Queue);
			m_QueueProducer->setPubSubDomain(false);
		}
		m_TopicConsumer->SetActiveMQRecvCB(Received_Message_CB,this);
		m_QueueConsumer->SetActiveMQRecvCB(Received_Message_CB,this);

	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool CecilyActiveMQManager::OpenMQ(const char* queue, int nType)
{
	if ( NULL == queue )
	{
		return false;
	}
	if ( 0 == nType )  // 发布订阅类型
	{
		if ( NULL == m_TopicConsumer )
		{
			return false;
		}
		m_TopicConsumer->Async_Receive(queue);
	}
	else{//队列类型
		if ( NULL == m_QueueConsumer )
		{
			return false;
		}
		m_QueueConsumer->Async_Receive(queue);
	}
	return true;
}

bool CecilyActiveMQManager::PutMessage(const char* queue, void*lpBuffer, int nLen, int nMessageType, int nType)
{
	if ( nType == 0 )//发布订阅模式
	{
		if ( NULL != m_TopicProduer )
		{
			if ( nMessageType == M_Text ) //暂时只支持Text 和 Bytes 类型
			{
				CecilyTextMessageCreator *lpTextMessageCreator = new CecilyTextMessageCreator((const char*)lpBuffer);
				try {
					m_TopicProduer->send(queue,lpTextMessageCreator);
				} catch( cms::CMSException& ex ) {
					delete lpTextMessageCreator;
					lpTextMessageCreator = NULL;
				}
				delete lpTextMessageCreator;
				lpTextMessageCreator = NULL;
			}
			else if ( nMessageType == M_Bytes )
			{
				CecilyBytesMessageCreator* lpBytesMessageCreator = new CecilyBytesMessageCreator((unsigned char*)lpBuffer,nLen);
				try
				{
					m_TopicProduer->send(queue,lpBytesMessageCreator);
				}
				catch(cms::CMSException& ex)
				{
					delete lpBytesMessageCreator;
					lpBytesMessageCreator = NULL;
				}
				delete lpBytesMessageCreator;
				lpBytesMessageCreator = NULL;
			}
			else{
				return false;
			}
		}
	}
	else{
		if ( NULL != m_QueueProducer )
		{
			if ( nMessageType == M_Text ) //暂时只支持Text 和 Bytes 类型
			{
				CecilyTextMessageCreator *lpTextMessageCreator = new CecilyTextMessageCreator((const char*)lpBuffer);
				try {
					m_QueueProducer->send(queue,lpTextMessageCreator);
				} catch( cms::CMSException& ex ) {
					delete lpTextMessageCreator;
					lpTextMessageCreator = NULL;
				}
				delete lpTextMessageCreator;
				lpTextMessageCreator = NULL;
			}
			else if ( nMessageType == M_Bytes )
			{
				CecilyBytesMessageCreator* lpBytesMessageCreator = new CecilyBytesMessageCreator((unsigned char*)lpBuffer,nLen);
				try
				{
					m_QueueProducer->send(queue,lpBytesMessageCreator);
				}
				catch(cms::CMSException& ex)
				{
					delete lpBytesMessageCreator;
					lpBytesMessageCreator = NULL;
				}
				delete lpBytesMessageCreator;
				lpBytesMessageCreator = NULL;
			}
			else{
				return false;
			}
		}
	}
	return true;
}

bool CecilyActiveMQManager::DeleteMQ(const char* queue, bool bPubSub)
{
	if ( bPubSub )
	{
		return m_TopicProduer->DeleteDestination(queue,bPubSub);
	}
	else{
		return m_QueueProducer->DeleteDestination(queue,bPubSub);
	}
}