/********************************************************************
	created:	  2016/11/01
	filename: 	  CecilyActiveMQConnection.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "CecilyActiveMQConnection.h"
#include "CecilyTextMessageCreator.h"
#include "CecilyBytesMessageCreator.h"

CecilyActiveMQConnection::CecilyActiveMQConnection(CCecilyMemoryPool* lpMemoryPool,const string& strBroker, const string& user, const string& password)
{
	m_lpConnectionFactory = NULL;
	m_TopicConsumer = NULL;
	m_QueueConsumer = NULL;
	m_TopicProduer = NULL;
	m_QueueProducer = NULL;
	m_handle = 0;
	m_lpRecvCallBack = NULL;
	m_ConnectionMemoryPool = lpMemoryPool;
	SetMQConnectionFactory(strBroker,user,password);

}

CecilyActiveMQConnection::~CecilyActiveMQConnection()
{

}


void CecilyActiveMQConnection::SetMQConnectionFactory(const string& strBroker, const string& user, const string& password)
{
	if ( NULL == m_lpConnectionFactory )
	{
		m_lpConnectionFactory = new ActiveMQConnectionFactory(strBroker,user,password);
		m_strBrokerURL = strBroker;
	}
}

void CecilyActiveMQConnection::InitCMSTemplates()
{
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

void CecilyActiveMQConnection::MQ_SendMessage(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason)
{
	if ( NULL == lpMQMD || NULL == buffer || nBufferLen <= 0  )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	try
	{
		CecilyMessageType nMessageType = (CecilyMessageType)lpMQMD->body_type;
		if ( NULL != m_QueueProducer )
		{
			if ( nMessageType == M_Text ) //暂时只支持Text 和 Bytes 类型
			{
				CecilyTextMessageCreator *lpTextMessageCreator = new CecilyTextMessageCreator((const char*)buffer);
				try {
					m_QueueProducer->send(lpMQMD->send_queue_name,lpTextMessageCreator);
				} catch( cms::CMSException& ex ) {
					delete lpTextMessageCreator;
					lpTextMessageCreator = NULL;
				}
				delete lpTextMessageCreator;
				lpTextMessageCreator = NULL;
			}
			else if ( nMessageType == M_Bytes )
			{
				CecilyBytesMessageCreator* lpBytesMessageCreator = new CecilyBytesMessageCreator((unsigned char*)buffer,nBufferLen);
				try
				{
					m_QueueProducer->send(lpMQMD->send_queue_name,lpBytesMessageCreator);
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
				*code = Cecily_Put_Message_Error;
				*reason = Cecily_Put_Message_Error;
				return;
			}
		}
	}
	catch(...)
	{
		*code = Cecily_Put_Message_Error;
		*reason = Cecily_Put_Message_Error;
		return;
	}
	
	return ;

}

void CecilyActiveMQConnection::OpenMQ(const char* queue, int nType)
{
	if ( NULL == queue || strlen(queue) == 0 )
	{
		return ;
	}
	if ( 0 == nType )  // 发布订阅类型
	{
		if ( NULL == m_TopicConsumer )
		{
			return ;
		}
		m_TopicConsumer->Async_Receive(queue);
	}
	else{//队列类型
		if ( NULL == m_QueueConsumer )
		{
			return ;
		}
		m_QueueConsumer->Async_Receive(queue);
	}
}

void CecilyActiveMQConnection::SetRecvCallBack(RecvCallBack lpfunc,void* user_data)
{
	if ( NULL == lpfunc )
	{
		return ;
	}
	m_lpRecvCallBack = lpfunc;
	m_lpRecvCBUserData = user_data;
	MessageInfo* info = NULL;
	info = DeQueue();
	while( NULL != info )
	{
		m_lpRecvCallBack(info->lpMQMD,info->buffer,info->nlen,m_lpRecvCBUserData);
		delete info->lpMQMD;
		info->lpMQMD = NULL;
		m_ConnectionMemoryPool->Deallocate(info->buffer,info->nlen);
		delete info;
		info = NULL;
		info = DeQueue();
	}
}

void CecilyActiveMQConnection::MQ_Publish(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason)
{
	if ( NULL == lpMQMD || NULL == buffer || nBufferLen <= 0  )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	try
	{
		CecilyMessageType nMessageType = (CecilyMessageType)lpMQMD->body_type;
		if ( NULL != m_TopicProduer )
		{
			if ( nMessageType == M_Text ) //暂时只支持Text 和 Bytes 类型
			{
				CecilyTextMessageCreator *lpTextMessageCreator = new CecilyTextMessageCreator((const char*)buffer);
				try {
					m_TopicProduer->send(lpMQMD->send_queue_name,lpTextMessageCreator);
				} catch( cms::CMSException& ex ) {
					delete lpTextMessageCreator;
					lpTextMessageCreator = NULL;
				}
				delete lpTextMessageCreator;
				lpTextMessageCreator = NULL;
			}
			else if ( nMessageType == M_Bytes )
			{
				CecilyBytesMessageCreator* lpBytesMessageCreator = new CecilyBytesMessageCreator((unsigned char*)buffer,nBufferLen);
				try
				{
					m_TopicProduer->send(lpMQMD->send_queue_name,lpBytesMessageCreator);
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
				*code = Cecily_Put_Message_Error;
				*reason = Cecily_Put_Message_Error;
				return;
			}
		}
	}
	catch(...)
	{
		*code = Cecily_Put_Message_Error;
		*reason = Cecily_Put_Message_Error;
		return;
	}

}

void CecilyActiveMQConnection::EnQueue(MessageInfo* lpInfo)
{
	if ( NULL == lpInfo )
	{
		return ;
	}
	{
		CSmartLocker locker(&m_csQueueLocker,TRUE);
		m_queueMessages.push(lpInfo);
	}
}

MessageInfo* CecilyActiveMQConnection::DeQueue()
{
	MessageInfo* info = NULL;
	{
		CSmartLocker locker(&m_csQueueLocker,TRUE);
		if ( m_queueMessages.empty() )
		{
			return NULL;
		}
		info = m_queueMessages.front();
		m_queueMessages.pop();
	}
	return info;
}

void CecilyActiveMQConnection::Received_Message_CB(CecilyMessage* lpMessage, void* user_data)
{
	CecilyActiveMQConnection* lpConnection = (CecilyActiveMQConnection*)user_data;
	if ( NULL == lpConnection )
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
	const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
	if ( NULL != textMessage )
	{
		CecilyMQMD* _mqmd =new CecilyMQMD;
		string strtext = textMessage->getText();
		string strGUID1 = textMessage->getCMSMessageID();
		strcpy_s(_mqmd->response_queue_name,sizeof(_mqmd->response_queue_name),lpMessage->strDestionNation.c_str());
		_mqmd->body_type = M_Text;
		int nlen = strtext.length();
		CCecilyMemoryPool* lpMemoryPool = lpConnection->GetMemoryPool();
		void * lpBuffer = (void*)lpMemoryPool->Allocate(nlen);
		memcpy_s(lpBuffer,nlen,strtext.c_str(),nlen);
		RecvCallBack cb = lpConnection->GetRecvCallBack();
		void* user_data = lpConnection->GetRecvCBUserData();
		if ( NULL != cb )
		{
			cb(_mqmd,lpBuffer,nlen,user_data);
			lpMemoryPool->Deallocate(lpBuffer,nlen);
			delete _mqmd;
			_mqmd = NULL;
		}
		else
		{
			//添加进入队列
			MessageInfo* _Info = new MessageInfo();
			_Info->lpMQMD = _mqmd;
			_Info->buffer = lpBuffer;
			_Info->nlen = nlen;
			lpConnection->EnQueue(_Info);
		}
		message->acknowledge();
		delete lpMessage;
		lpMessage = NULL;
		return ;
	}

	const BytesMessage* byteMessage = dynamic_cast<const BytesMessage*> (message);
	if ( NULL != byteMessage )
	{
		CecilyMQMD* _mqmd =new CecilyMQMD;
		string strGUID1 = byteMessage->getCMSMessageID();
		strcpy_s(_mqmd->response_queue_name,sizeof(_mqmd->response_queue_name),lpMessage->strDestionNation.c_str());
		_mqmd->body_type = M_Bytes;
		unsigned char* bytesPtr = byteMessage->getBodyBytes();
		int nlen = byteMessage->getBodyLength();
		CCecilyMemoryPool* lpMemoryPool = lpConnection->GetMemoryPool();
		void * lpBuffer = (void*)lpMemoryPool->Allocate(nlen);
		memcpy_s(lpBuffer,nlen,bytesPtr,nlen);
		RecvCallBack cb = lpConnection->GetRecvCallBack();
		void* user_data = lpConnection->GetRecvCBUserData();
		if ( NULL != cb )
		{
			cb(_mqmd,lpBuffer,nlen,user_data);
			lpMemoryPool->Deallocate(lpBuffer,nlen);
			delete _mqmd;
			_mqmd = NULL;
		}
		else
		{
			//添加进入队列
			MessageInfo* _Info = new MessageInfo();
			_Info->lpMQMD = _mqmd;
			_Info->buffer = lpBuffer;
			_Info->nlen = nlen;
			lpConnection->EnQueue(_Info);
		}
		delete []bytesPtr;
		bytesPtr = NULL;
		message->acknowledge();
		delete lpMessage;
		lpMessage = NULL;
		return ;
	}
	delete lpMessage;
	lpMessage = NULL;

}