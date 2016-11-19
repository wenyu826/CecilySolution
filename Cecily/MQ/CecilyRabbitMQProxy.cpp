/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/26
*  File:           CecilyMQProxy.cpp
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#include "StdAfx.h"
#include "CecilyRabbitMQProxy.h"
#include "CecilyRabbitMQConnection.h"
#include "Log.h"

long CCecilyRabbitMQProxy::m_sHConn = 10000;

struct AMQPGetThreadParams
{
	string m_strServerIP;
	CCecilyRabbitMQConnection* m_lpConnection;
	CCecilyRabbitMQProxy* m_lpMQProxy;
};

CCecilyRabbitMQProxy::CCecilyRabbitMQProxy(void)
{
	m_MQType = MQ_Rabbit;
	m_bInit = false;
	m_nQueueDepth = 0;
	m_lpMemoryPool = new CCecilyMemoryPool();
}

CCecilyRabbitMQProxy::~CCecilyRabbitMQProxy(void)
{
	if ( NULL != m_lpMemoryPool )
	{
		delete m_lpMemoryPool;
		m_lpMemoryPool = NULL;
	}
}
DWORD WINAPI CCecilyRabbitMQProxy::RecvThread_MQ(LPVOID lparam)
{
	AMQPGetThreadParams* ThreadParam = (AMQPGetThreadParams*)lparam;
	if ( NULL == ThreadParam )
	{
		return 0;
	}
	CCecilyRabbitMQProxy *lpThis = ThreadParam->m_lpMQProxy;
	if ( NULL == lpThis )
	{
		delete ThreadParam;
		ThreadParam = NULL;
		return 0;
	}
	try
	{
		lpThis->RecvProc_MQ(ThreadParam->m_lpConnection);
	}
	catch (...)
	{
		if ( NULL != lpThis )
		{
			lpThis->ResetCurrentHandle(ThreadParam->m_lpConnection->GetHandle());
			ThreadParam->m_lpConnection->ResetConsumer();
		}
		delete ThreadParam;
		ThreadParam = NULL;
		return 0;
	}
	if ( NULL != lpThis )
	{
		lpThis->ResetCurrentHandle(ThreadParam->m_lpConnection->GetHandle());
		ThreadParam->m_lpConnection->ResetConsumer();
	}

	delete ThreadParam;
	ThreadParam = NULL;	
	return 1;

}


DWORD WINAPI CCecilyRabbitMQProxy::RecvThread(LPVOID lparam)
{
	AMQPGetThreadParams* ThreadParam = (AMQPGetThreadParams*)lparam;
	if ( NULL == ThreadParam )
	{
		return 0;
	}
	CCecilyRabbitMQProxy *lpThis = ThreadParam->m_lpMQProxy;
	if ( NULL == lpThis )
	{
		delete ThreadParam;
		ThreadParam = NULL;
		return 0;
	}
	try
	{
		lpThis->RecvProc((ThreadParam->m_strServerIP).c_str());
	}
	catch (...)
	{
		delete ThreadParam;
		ThreadParam = NULL;
		return 0;
	}
	delete ThreadParam;
	ThreadParam = NULL;	
	return 1;
}


CecilyMQStatus CCecilyRabbitMQProxy::GetMessage(char** lpQueueName, char** lpMessageBuffer, int timeout, const char* lpServerIP /* = NULL */)
{
	string strServerIP  = "";
	HANDLE hThread = GetCurrentHandle(lpServerIP);
	CCecilyRabbitMQConnection* lpConnection = NULL;
	if ( NULL == hThread )
	{
		//getmessage线程已经退出，要重新连接
		bool bFirst = true;
		if ( NULL == lpServerIP || ( strcmp(lpServerIP,"")==0 ) )
		{
			map<string,CCecilyRabbitMQConnection*>::iterator itMapConnection = m_mapConnection.begin();
			if ( itMapConnection != m_mapConnection.end() )
			{
				lpConnection = itMapConnection->second;
			}
			if ( NULL == lpConnection )
			{
				return Cecily_Invalid_Connection;
			}
			strServerIP = lpConnection->GetServerIP();
			vector<string>::iterator itVec = m_vecThreadFlag.begin();
			for ( itVec ; itVec != m_vecThreadFlag.end(); itVec++ )
			{
				string strServer = *itVec;
				if ( strcmp(strServer.c_str(),strServerIP.c_str()) == 0 )
				{
					bFirst = false;
				}
			}
		}else
		if ( NULL != lpServerIP && (strcmp(lpServerIP,"")!=0) )
		{
			strServerIP = lpServerIP;
			vector<string>::iterator itVec = m_vecThreadFlag.begin();
			for ( itVec ; itVec != m_vecThreadFlag.end(); itVec++ )
			{
				string strServer = *itVec;
				if ( strcmp(strServer.c_str(),lpServerIP) == 0 )
				{
					bFirst = false;
				}
			}
		}
		if ( !bFirst && NULL == lpConnection )
		{
			return Cecily_Invalid_Connection;
		}else
		{
			//第一次请求队列，创建get队列线程
			AMQPGetThreadParams* ThreadParam = new AMQPGetThreadParams();
			if ( NULL == ThreadParam )
			{
				return Cecily_UnKnow_Error;
			}
			ThreadParam->m_strServerIP = strServerIP;
			ThreadParam->m_lpMQProxy = this;
			HANDLE hThread = CreateThread(NULL,
				 			0,
							RecvThread,
				 			(LPVOID)ThreadParam,
				 			NULL,
				 			NULL);
			{
				CSmartLocker locker(&m_ThreadLocker,TRUE);
				m_mapGetThread.insert(pair<string,HANDLE>(strServerIP,hThread));
			}
			m_vecThreadFlag.push_back(strServerIP);
		}	
	}
// 	DWORD dwWait;
// 	if ( timeout <= 0  )
// 	{
// 		dwWait = WaitForMultipleObjects(AMQP_HANDLE_EVENTS,m_eventHandles,false,INFINITE);
// 	}else
// 	{
// 		dwWait = WaitForMultipleObjects(AMQP_HANDLE_EVENTS,m_eventHandles,false,timeout);
// 	}
// 	if( dwWait == WAIT_OBJECT_0)
// 	{
// 		CLog *pLog = CLog::GetInstance();
// 		if ( NULL != pLog )
// 		{
// 			static int nMessages = 0;
// 			pLog->WriteLog(__LINE__,LOGLEVEL_DEBUG,"接收消息数量%d",nMessages++);
// 		}
// 		CCecilyAMQPMessage* message = NULL;
// 		PopMessage(&message);
// 		if ( NULL != message )
// 		{
// 			string strQueueName = message->GetQueueName();
// 			unsigned int nBufferSize = message->GetMessageBufferSize();
// 			unsigned int nQueueLength = strlen(strQueueName.c_str()) +1;
// 			*lpQueueName = new char[nQueueLength+1];
// 			memset(*lpQueueName,0,nQueueLength+1);
// 			*lpMessageBuffer = new char[nBufferSize+1];
// 			memset(*lpMessageBuffer,0,nBufferSize+1);
// 			memcpy_s(*lpQueueName,nQueueLength+1,strQueueName.c_str(),strlen(strQueueName.c_str()));
// 			memcpy_s(*lpMessageBuffer,nBufferSize+1,message->GetMessageBufferPtr(),nBufferSize);
// 		}
// 		delete message;
// 		message = NULL;
// 		//ReleaseSemaphore(m_eventHandles[0] ,  1,  NULL);
// 		return Cecily_Success;
// 	}
// 	else if ( dwWait ==WAIT_OBJECT_0 + 1 )  //get线程退出了连接断开
// 	{
// 		ResetEvent(m_eventHandles[1]);
// 		return Cecily_Invalid_Connection;
// 	}
// 	else if ( dwWait == WAIT_TIMEOUT )
// 	{
// 		return Cecily_GetMessage_OutTime;
// 	}
// 	DWORD dwWait = WaitForSingleObject(m_messageEvent,INFINITE);
// 	if ( dwWait != WAIT_OBJECT_0 )
// 	{
// 		return Cecily_UnKnow_Error;
// 	}
// 	else
// 	{
// 		CCecilyAMQPMessage* message = NULL;
// 		PopMessage(&message);
// 		if ( NULL != message )
// 		{
// 			string strQueueName = message->GetQueueName();
// 			unsigned int nBufferSize = message->GetMessageBufferSize();
// 			unsigned int nQueueLength = strlen(strQueueName.c_str()) +1;
// 			*lpQueueName = new char[nQueueLength+1];
// 			memset(*lpQueueName,0,nQueueLength+1);
// 			*lpMessageBuffer = new char[nBufferSize+1];
// 			memset(*lpMessageBuffer,0,nBufferSize+1);
// 			memcpy_s(*lpQueueName,nQueueLength+1,strQueueName.c_str(),strlen(strQueueName.c_str()));
// 			memcpy_s(*lpMessageBuffer,nBufferSize+1,message->GetMessageBufferPtr(),nBufferSize);
// 		}
// 		delete message;
// 		message = NULL;
// 		ResetEvent(m_messageEvent);
// 	}
	return Cecily_Success;
}




CCecilyRabbitMQConnection* CCecilyRabbitMQProxy::InitMQConnection(const char* lpIPAddress, const char* lpVirtualHost, unsigned int nPort, const char* lpUser,const char* lpPassword)
{
	if ( NULL == lpIPAddress )
	{
		return NULL;
	}
	CCecilyRabbitMQConnection* lpNewConnection = NULL;
	lpNewConnection = new CCecilyRabbitMQConnection(lpIPAddress,lpVirtualHost,nPort,lpUser,lpPassword);
	bool bInti = lpNewConnection->Init();
	if ( !bInti )
	{
		delete lpNewConnection;
		lpNewConnection = NULL;
	}
	return lpNewConnection;
// 	{
// 		CSmartLocker locker(&m_ConnectionLocker,TRUE);
// 		map<string,CCecilyRabbitMQConnection*>::iterator itConnect  = m_mapConnection.find(lpIPAddress);
// 		if ( itConnect != m_mapConnection.end() )
// 		{
// 			return itConnect->second;
// 		}else
// 		{
// 			string strKey = lpIPAddress;
// 			lpNewConnection = new CCecilyRabbitMQConnection(lpIPAddress,lpVirtualHost,nPort,lpUser,lpPassword);
// 			bool bInti = lpNewConnection->Init();
// 			if ( bInti )
// 			{
// 				m_mapConnection.insert(pair<string,CCecilyRabbitMQConnection*>(strKey,lpNewConnection));
// 			}
// 			else
// 			{
// 				delete lpNewConnection;
// 				lpNewConnection = NULL;
// 				return NULL;
// 			}	
// 		}
// 	}
//	return lpNewConnection;
}

bool CCecilyRabbitMQProxy::RecvProc_MQ(CCecilyRabbitMQConnection* lpConnection)
{
	if ( NULL == lpConnection )
	{
		return false;
	}
	CecilyAmqpObject* lpObject = lpConnection->GetGetObject();
	if ( NULL == lpObject )
	{
		return false;
	}
	string strQueueName = "";
	amqp_basic_qos(lpObject->clear_conn,AMQP_GET_CHANNELS,0,AMQP_PREFETCH_COUNT,false);
	//map<long,vector<AMQPExchangeQueue>> m_longMapExchangeQueue;
	map<long,vector<AMQPExchangeQueue>>::iterator itMapExchangeQueue = m_longMapExchangeQueue.find(lpConnection->GetHandle());
	if ( itMapExchangeQueue != m_longMapExchangeQueue.end() )
	{
		vector<AMQPExchangeQueue>::iterator itVec = (itMapExchangeQueue->second).begin();
		for ( itVec; itVec != (itMapExchangeQueue->second).end(); itVec++ )
		{
			if ( (*itVec).nQueueType != 0  )
			{
				string strQueuName = (*itVec).queueName;
				amqp_basic_consume(lpObject->clear_conn,AMQP_GET_CHANNELS,amqp_cstring_bytes(strQueuName.c_str()),amqp_empty_bytes,0,false,0,amqp_empty_table );
				lpConnection->AddConsumer(strQueuName);
			}
		}
	}
	char szQueueName[AMQP_MAX_QUEUE_LENGTH] = {0};
	char szMessageID[CLEARMQ_MSGID_LENGTH] = {0};
	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	while (true)
	{
		amqp_rpc_reply_t ret;
		amqp_envelope_t envelop;	
		try
		{
			amqp_get_rpc_reply(lpObject->clear_conn);
			amqp_maybe_release_buffers(lpObject->clear_conn);
			ret = amqp_consume_message(lpObject->clear_conn,&envelop,&timeout,0);
		}
		catch(...)
		{
			int a = 10;
			break;
		}

		if ( ret.reply_type == AMQP_RESPONSE_NORMAL )
		{
			if ( lpConnection->IsStopConsumer() )
			{
				lpConnection->ResetConsumer();
				amqp_destroy_envelope(&envelop);
				break;
			}
			memset(szQueueName,0,AMQP_MAX_QUEUE_LENGTH);
			memset(szMessageID,0,CLEARMQ_MSGID_LENGTH);
			CCecilyAMQPMessage* lpMessage = new CCecilyAMQPMessage(m_lpMemoryPool);
			if ( NULL == lpMessage )  
			{
				break;
			}
			memcpy_s(szQueueName,AMQP_MAX_QUEUE_LENGTH,envelop.routing_key.bytes,envelop.routing_key.len);
			lpMessage->SetQueueName(szQueueName);
			int nBodySize = envelop.message.body.len;
			amqp_basic_properties_t properties = envelop.message.properties;
			int nContentType = properties.content_type.len;
// 			char *szContentType = new char[nContentType+1];
// 			memset(szContentType,0,nContentType+1);
// 			memcpy_s(szContentType,nContentType+1,properties.content_type.bytes,nContentType);

			int nMessageIdLength = envelop.message.properties.message_id.len;
			memcpy_s(szMessageID,CLEARMQ_MSGID_LENGTH,envelop.message.properties.message_id.bytes,nMessageIdLength);
			lpMessage->SetMessageID(szMessageID);
			lpMessage->ApplyMemorySpace(nBodySize+1);
			lpMessage->WriteMessageBuffer(envelop.message.body.bytes,envelop.message.body.len);
			bool bPush = lpConnection->AddCacheMessage(lpMessage);
			//bool bPush = PushMessage(lpMessage);
			if ( bPush )
			{
				if ( amqp_basic_ack(lpObject->clear_conn,AMQP_GET_CHANNELS,envelop.delivery_tag,false) > 0 )
				{
					//向服务器发送回馈失败
				}
			}	
			amqp_destroy_envelope(&envelop);
			while ( lpConnection->GetCacheDepth() > MAX_MSG_NUM - 10 )//达到了最大的队列深度
			{
				Sleep(1000);
			}
		}else if ( ret.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION )
		{
			//这里发生了异常，应该退出线程
			int nErrorCode = ret.library_error;
			if ( nErrorCode == AMQP_STATUS_TIMEOUT )   //超时
			{
				if ( !lpConnection->IsStopConsumer() )
				{
					amqp_destroy_envelope(&envelop);
					continue;
				}
				else
				{
					//退出接收线程
					amqp_destroy_envelope(&envelop);
					break;
				}
				
			}
			const char *lpErrorMsg = amqp_error_string2(nErrorCode);
			int a = 10;
			break;
		}
		else
		{
			amqp_destroy_envelope(&envelop);
			break;
		}
	}
	//CloseDeleteHandle(lpServerIP);
	//Disconnect(lpServerIP);
	//SetEvent(m_eventHandles[1]);
	SetEvent(lpConnection->m_connEventHandles[1]);
	return false;

}

bool CCecilyRabbitMQProxy::RecvProc(const char* lpServerIP)
{
	return true;
	CCecilyRabbitMQConnection* lpConnection = NULL;
	map<string,CCecilyRabbitMQConnection*>::iterator itMapConnection;
	if ( NULL == lpServerIP || ( strcmp(lpServerIP,"") == 0) )
	{
		itMapConnection = m_mapConnection.begin();
		if ( itMapConnection!= m_mapConnection.end() )
		{
			lpConnection = itMapConnection->second;
		}
	}else
	{
		itMapConnection = m_mapConnection.find(lpServerIP);
		if ( itMapConnection != m_mapConnection.end() )
		{
			lpConnection = itMapConnection->second;
		}
	}
	if ( NULL == lpConnection )
	{
		return false;
	}
	CecilyAmqpObject* lpObject = lpConnection->GetGetObject();
	if ( NULL == lpObject )
	{
		return false;
	}
	string strQueueName = "";
	amqp_basic_qos(lpObject->clear_conn,AMQP_GET_CHANNELS,0,AMQP_PREFETCH_COUNT,false);
	map<string,vector<AMQPExchangeQueue>>::iterator itMapExchangeQueue = m_mapExchangeQueue.find(lpServerIP);
	if ( itMapExchangeQueue != m_mapExchangeQueue.end() )
	{
		vector<AMQPExchangeQueue>::iterator itVec = (itMapExchangeQueue->second).begin();
		for ( itVec; itVec != (itMapExchangeQueue->second).end(); itVec++ )
		{
			if ( (*itVec).nQueueType != 0  )
			{
				string strQueuName = (*itVec).queueName;
				amqp_basic_consume(lpObject->clear_conn,AMQP_GET_CHANNELS,amqp_cstring_bytes(strQueuName.c_str()),amqp_empty_bytes,0,false,0,amqp_empty_table );
				lpConnection->AddConsumer(strQueuName);
			}
		}
	}
	char szQueueName[AMQP_MAX_QUEUE_LENGTH] = {0};
	while (true)
	{
		amqp_rpc_reply_t ret;
		amqp_envelope_t envelop;	
		try
		{
			amqp_get_rpc_reply(lpObject->clear_conn);
			amqp_maybe_release_buffers(lpObject->clear_conn);
			ret = amqp_consume_message(lpObject->clear_conn,&envelop,NULL,0);
		}
		catch(...)
		{
			int a = 10;
			break;
		}
		
		if ( ret.reply_type == AMQP_RESPONSE_NORMAL )
		{
			memset(szQueueName,0,AMQP_MAX_QUEUE_LENGTH);
			CCecilyAMQPMessage* lpMessage = new CCecilyAMQPMessage(m_lpMemoryPool);
			if ( NULL == lpMessage )  
			{
				break;
			}
			memcpy_s(szQueueName,AMQP_MAX_QUEUE_LENGTH,envelop.routing_key.bytes,envelop.routing_key.len);
			lpMessage->SetQueueName(szQueueName);
			int nBodySize = envelop.message.body.len;
			amqp_basic_properties_t properties = envelop.message.properties;
			int nContentType = properties.content_type.len;
			char *szContentType = new char[nContentType+1];
			memset(szContentType,0,nContentType+1);
			memcpy_s(szContentType,nContentType+1,properties.content_type.bytes,nContentType);
			
			lpMessage->ApplyMemorySpace(nBodySize+1);
			lpMessage->WriteMessageBuffer(envelop.message.body.bytes,envelop.message.body.len);
			bool bPush = lpConnection->AddCacheMessage(lpMessage);
			//bool bPush = PushMessage(lpMessage);
			if ( bPush )
			{
				if ( amqp_basic_ack(lpObject->clear_conn,AMQP_GET_CHANNELS,envelop.delivery_tag,false) > 0 )
				{
					//向服务器发送回馈失败
				}
			}	
			amqp_destroy_envelope(&envelop);
		}else if ( ret.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION )
		{
			//这里发生了异常，应该退出线程
			 int nErrorCode = ret.library_error;
			 const char *lpErrorMsg = amqp_error_string2(nErrorCode);
			 int a = 10;
			 break;
		}
		else
		{
			break;
		}
	}
	CloseDeleteHandle(lpServerIP);
	//Disconnect(lpServerIP);
	//SetEvent(m_eventHandles[1]);
	return false;
}

bool CCecilyRabbitMQProxy::DeclareExchangeQueue(CecilyExchangeType& exchangeType,const char* lpExchangeName,const char* lpQueueName, int QueueType,const char* lpServerIP /* = NULL */)
{
	if ( NULL == lpQueueName || ( strcmp(lpQueueName,"") == 0 ))
	{
		return false;
	}
	AMQPExchangeQueue aeq;
	switch(exchangeType)
	{
	case Direct:
		{
			aeq.exchangeType = "direct";
			break;
		}	
	case Topic:
		{
			aeq.exchangeType = "fanout";
			break;
		}
	default:
		aeq.exchangeType = "unknow";
		break;
	}
	aeq.exchangeName = lpExchangeName;
	aeq.nQueueType = QueueType;
	aeq.queueName = lpQueueName;
	CCecilyRabbitMQConnection* lpConnection = GetMQConnection(lpServerIP);
	if ( NULL == lpConnection )
	{
		return false;
	}
	CecilyAmqpObject *lpAMQPObject = lpConnection->GetPutObject();
	vector<AMQPExchangeQueue> vecExchangeQueue;
	string strServerIP = lpConnection->GetServerIP();
	map<string,vector<AMQPExchangeQueue>>::iterator itMapExchangeQueue = m_mapExchangeQueue.find(strServerIP.c_str());
	bool bHasExchange = false;
	bool bHasQueue = false;
	bool bExchangeCreate = false;
	bool bQueueCreate = false;
	amqp_response_type_enum arte;
	string strErrorMsg;
	if ( itMapExchangeQueue != m_mapExchangeQueue.end() )
	{
		vector<AMQPExchangeQueue>::iterator itVec = (itMapExchangeQueue->second).begin();
		for ( itVec; itVec != (itMapExchangeQueue->second).end(); itVec++ )
		{
			if ( strcmp( lpExchangeName,(*itVec).exchangeName.c_str() ) == 0 )
			{
				bHasExchange = true;
			}
			if ( strcmp( lpQueueName,(*itVec).queueName.c_str() ) ==0 )
			{
				bHasQueue = true;
			}
		}
	}
	if ( !bHasExchange && aeq.exchangeType != "unknow" )
	{
		if ( NULL != lpExchangeName && ( strcmp(lpExchangeName,"") != 0 ))
		{
			amqp_exchange_declare_ok_t* exRet =  amqp_exchange_declare(lpAMQPObject->clear_conn,
				AMQP_PUT_CHANNELS,
				amqp_cstring_bytes(aeq.exchangeName.c_str()),  //exchange name
				amqp_cstring_bytes(aeq.exchangeType.c_str()),           //exchange type
				false,		//passive
				true,		//durable
				false,		//auto_delete
				false,		//internal
				amqp_empty_table
				);
			arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
			if ( arte == AMQP_RESPONSE_NORMAL )
			{
				bExchangeCreate = true;
			}
		}
	}
	if ( !bHasQueue )
	{
		amqp_queue_declare_ok_t *r = amqp_queue_declare(lpAMQPObject->clear_conn,
			AMQP_PUT_CHANNELS,
			amqp_cstring_bytes(aeq.queueName.c_str()),
			false,
			true,
			false,
			false,
			amqp_empty_table
			);
		arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
		if ( AMQP_RESPONSE_NORMAL == arte )
		{
			bQueueCreate = true;
		}
	}
	if (  bQueueCreate )
	{
		if ( bHasExchange || bExchangeCreate )
		{
			amqp_queue_bind(lpAMQPObject->clear_conn,
				AMQP_PUT_CHANNELS,
				amqp_cstring_bytes(lpQueueName),
				amqp_cstring_bytes(lpExchangeName),
				amqp_cstring_bytes(lpQueueName),
				amqp_empty_table		
				);
			arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
			if ( AMQP_RESPONSE_NORMAL == arte )
			{
			}
		}
	}
	if ( !bExchangeCreate )
	{
		aeq.exchangeName = "";
	}
	if ( !bQueueCreate )
	{
		aeq.queueName = "";
	}
	if ( itMapExchangeQueue != m_mapExchangeQueue.end() )
	{
		(itMapExchangeQueue->second).push_back(aeq);
	}
	else{
		vecExchangeQueue.push_back(aeq);
		m_mapExchangeQueue.insert(pair<string,vector<AMQPExchangeQueue>>(strServerIP.c_str(),vecExchangeQueue));
	}
	return true;
}


bool CCecilyRabbitMQProxy::PurgeQueue(const string& strQueueName)
{
// 	if ( NULL == m_conn1 || strQueueName.empty() )
// 	{
// 		return false;
// 	}
// 	amqp_queue_purge_ok_t *ret = amqp_queue_purge(m_conn1, AMQP_PUT_CHANNELS, amqp_cstring_bytes(strQueueName.c_str()));
	return true;
}

amqp_response_type_enum CCecilyRabbitMQProxy::CheckLastOperation(const amqp_rpc_reply_t& rt,const char* lpOperation,string &strMsg)
{
	if ( rt.reply_type == AMQP_RESPONSE_NORMAL )
	{
		return rt.reply_type;
	}
	char szErrorMsg[ERROR_MSG_BUFFER] = {0};
	switch (rt.reply_type) 
	{
	case AMQP_RESPONSE_NONE:
		sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"%s: missing RPC reply type!\n", lpOperation);
		break;

	case AMQP_RESPONSE_LIBRARY_EXCEPTION:
		sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"%s: %s\n", lpOperation, amqp_error_string2(rt.library_error));
		break;

	case AMQP_RESPONSE_SERVER_EXCEPTION:
		switch (rt.reply.id) 
		{
		case AMQP_CONNECTION_CLOSE_METHOD: 
			{
				amqp_connection_close_t *m = (amqp_connection_close_t *) rt.reply.decoded;
				sprintf_s(szErrorMsg,ERROR_MSG_BUFFER, "%s: server connection error %uh, message: %.*s\n",
					lpOperation,
					m->reply_code,
					(int) m->reply_text.len, (char *) m->reply_text.bytes);
				break;
			}
		case AMQP_CHANNEL_CLOSE_METHOD: 
			{
				amqp_channel_close_t *m = (amqp_channel_close_t *) rt.reply.decoded;
				sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"%s: server channel error %uh, message: %.*s\n",
					lpOperation,
					m->reply_code,
					(int) m->reply_text.len, (char *) m->reply_text.bytes);
				break;
			}
		default:
			sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"%s: unknown server error, method id 0x%08X\n", lpOperation, rt.reply.id);
			break;
		}
		break;
	}
	strMsg = szErrorMsg;
	return rt.reply_type;
}

CecilyMQStatus CCecilyRabbitMQProxy::ConnectToMQ(const char* lpIPAddress, const char* lpVirtualHost, unsigned int nPort, const char* lpUser,const char* lpPassword)
{
	if ( NULL == lpIPAddress || NULL == lpUser || NULL == lpPassword ||  (nPort <= 0)  )
	{
		return Cecily_Params_Error;
	}
	CCecilyRabbitMQConnection* lpMQConnection = InitMQConnection(lpIPAddress,lpVirtualHost,nPort,lpUser,lpPassword);
	if ( NULL == lpMQConnection )
	{
		return Cecily_Connection_Init_Error;
	}
	CecilyMQStatus status =  lpMQConnection->TryConnect();
	if ( Cecily_Success == status )
	{
		AMQPAuthority authority;
		authority.strServerIP = lpIPAddress;
		authority.strVirtualHost = lpVirtualHost;
		authority.nPort = nPort;
		authority.strUser = lpUser;
		authority.strPassword = lpPassword;
		map<string,AMQPAuthority>::iterator itMapAuthority = m_mapAuthority.find(lpIPAddress);
		if ( itMapAuthority != m_mapAuthority.end() )
		{
			itMapAuthority->second = authority;
		}else
		{
			m_mapAuthority.insert(pair<string,AMQPAuthority>(lpIPAddress,authority));
		}
	}else
	{
		map<string,CCecilyRabbitMQConnection*>::iterator  itMapConnection = m_mapConnection.find(lpIPAddress);
		if ( itMapConnection != m_mapConnection.end() )
		{
			delete itMapConnection->second;
			itMapConnection->second = NULL;
			m_mapConnection.erase(itMapConnection);
		}
	}
	return status;
}



CecilyMQStatus CCecilyRabbitMQProxy::SendMessage( const char* lpQueueName,const void* lpBuffer, int nLen, CecilyMessageType nMessageType, const char* lpServerIP)
{
	if (  NULL == lpQueueName || NULL == lpBuffer )
	{
		return Cecily_Params_Error;
	}
	CCecilyRabbitMQConnection* lpConnection = NULL;
	map<string,CCecilyRabbitMQConnection*>::iterator itConnect ; 
	if ( NULL != lpServerIP )
	{
		itConnect = m_mapConnection.find(lpServerIP);
	}else
	{
		itConnect = m_mapConnection.begin();
	}
	if ( itConnect != m_mapConnection.end() )
	{
		lpConnection = itConnect->second;
		if ( NULL != lpConnection )
		{
			CecilyAmqpObject* lpPutObject = lpConnection->GetPutObject();
			if ( NULL == lpPutObject )
			{
				return Cecily_Invalid_Connection;
			}
			amqp_basic_properties_t_ props;
			props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
				AMQP_BASIC_DELIVERY_MODE_FLAG ;
			if ( nMessageType == M_Text )
			{
				props.content_type = amqp_cstring_bytes("text");
			}
			else if ( nMessageType == M_Bytes )
			{
				props.content_type = amqp_cstring_bytes("bytes");
			}
			props.delivery_mode = 2; /* persistent delivery mode */
			amqp_bytes_t body;
			body.len = nLen;
			body.bytes = (void*)lpBuffer;
			int status = amqp_basic_publish(lpPutObject->clear_conn,
				AMQP_PUT_CHANNELS,
				amqp_cstring_bytes(""),
				amqp_cstring_bytes(lpQueueName),
				0,
				0,
				&props,
				body);
			if ( status == AMQP_STATUS_OK)
			{
				return Cecily_Success;
			}
			else
			{
				return Cecily_Put_Message_Error;
			}
		}else
		{
			return Cecily_Invalid_Connection;
		}
	}
	else
	{
		return Cecily_Server_Not_Connected;
	}
	return Cecily_Success;
}

CecilyMQStatus CCecilyRabbitMQProxy::Disconnect(const char* lpIpAddress)
{
	map<string,CCecilyRabbitMQConnection*>::iterator itConnection;
	CCecilyRabbitMQConnection* lpConnection = NULL;
	{
		CSmartLocker locker(&m_ConnectionLocker,TRUE);
		if ( NULL == lpIpAddress || ( strcmp(lpIpAddress,"") == 0 )  )
		{
			itConnection = m_mapConnection.begin();
			for ( itConnection; itConnection != m_mapConnection.end();  itConnection++ )
			{
				lpConnection = itConnection->second;
				if ( NULL != lpConnection )
				{
					CecilyMQStatus status = lpConnection->TryDisconnect();
					if ( Cecily_Success == status )
					{
						delete lpConnection;
						lpConnection = NULL;
					}
					else
					{
						return status;
					}
					
				}
			}
			m_mapConnection.clear();
		}else
		{
			itConnection = m_mapConnection.find(lpIpAddress);
			if ( itConnection != m_mapConnection.end() )
			{
				lpConnection = itConnection->second;
				if ( NULL != lpConnection )
				{
					CecilyMQStatus status = lpConnection->TryDisconnect();
					if ( Cecily_Success == status )
					{
						delete lpConnection;
						lpConnection = NULL;
						m_mapConnection.erase(itConnection);
					}
					else
					{
						return status;
					}
				}
			}
		}	
	}
	
	return Cecily_Success;
}

// bool CCecilyRabbitMQProxy::PushMessage(CCecilyAMQPMessage* lpMessage)
// {
// 	if ( NULL == lpMessage )
// 	{
// 		return false;
// 	}
// 	{
// 		CSmartLocker locker(&m_QueueLocker,TRUE);
// 		m_AMQPMessageQueue.push(lpMessage);
// 		//SetEvent(m_eventHandles[0]);
// 		//ReleaseSemaphore(m_eventHandles[0] ,  1,  NULL);
// 		m_nQueueDepth = m_nQueueDepth +1;
// 	}
// 	return true;
// }

// bool CCecilyRabbitMQProxy::PopMessage(CCecilyAMQPMessage** lpMessage)
// {
// 	if ( m_AMQPMessageQueue.empty() )
// 	{
// 		return false;
// 	}
// 	{
// 		CSmartLocker locker(&m_QueueLocker,TRUE);
// 		*lpMessage = m_AMQPMessageQueue.front();
// 		m_AMQPMessageQueue.pop();
// 		m_nQueueDepth = m_nQueueDepth - 1;
// 		if ( m_nQueueDepth < 0 )
// 		{
// 			m_nQueueDepth = 0;
// 		}
// 	}
// 	return true;
// }

void CCecilyRabbitMQProxy::CloseDeleteHandle(const char* lpServerIP)
{
	{
		CSmartLocker locker(&m_ThreadLocker,TRUE);
		map<string,HANDLE>::iterator itMapHandle;
		if ( NULL == lpServerIP ||  ( strcmp(lpServerIP,"") == 0 ) )
		{
			itMapHandle = m_mapGetThread.begin();
			if ( itMapHandle != m_mapGetThread.end() )
			{
				CloseHandle(itMapHandle->second);
				m_mapGetThread.erase(itMapHandle);
			}

		}
		else
		{
			itMapHandle = m_mapGetThread.find(lpServerIP);
			if ( itMapHandle != m_mapGetThread.end() )
			{
				CloseHandle(itMapHandle->second);
				m_mapGetThread.erase(itMapHandle);
			}
		}
	}
	
}

HANDLE CCecilyRabbitMQProxy::GetCurrentHandle(long hConn)
{
	map<long,HANDLE>::iterator itHandle;
	HANDLE hTread = NULL;
	{
		CSmartLocker locker(&m_ThreadLocker,TRUE);
		itHandle = m_longGetThread.find(hConn);
		if ( itHandle != m_longGetThread.end() )
		{
			hTread = itHandle->second;
		}
	}
	return hTread;
}

void CCecilyRabbitMQProxy::SetCurrentHandle(long hConn,HANDLE hHandle)
{
	map<long,HANDLE>::iterator itHandle;
	{
		CSmartLocker locker(&m_ThreadLocker,TRUE);
		itHandle = m_longGetThread.find(hConn);
		if ( itHandle != m_longGetThread.end() )
		{
			itHandle->second = hHandle;
		}
		else
		{
			m_longGetThread.insert(pair<long,HANDLE>(hConn,hHandle));
		}
	}

}

void CCecilyRabbitMQProxy::ResetCurrentHandle(long hConn)
{
	map<long,HANDLE>::iterator itHandle;
	{
		CSmartLocker locker(&m_ThreadLocker,TRUE);
		itHandle = m_longGetThread.find(hConn);
		if ( itHandle != m_longGetThread.end() )
		{
			itHandle->second = NULL;
		}
	}
}

HANDLE CCecilyRabbitMQProxy::GetCurrentHandle(const char* lpServerIP)
{
	map<string,HANDLE>::iterator itmapHandle;
	HANDLE hTread = NULL;
	{
		CSmartLocker locker(&m_ThreadLocker,TRUE);
		if ( NULL == lpServerIP || ( strcmp(lpServerIP,"") == 0 ) )
		{
			itmapHandle = m_mapGetThread.begin();
			if ( itmapHandle != m_mapGetThread.end() )
			{
				hTread = itmapHandle->second;
			}	
		}
		else
		{
			itmapHandle = m_mapGetThread.find(lpServerIP);
			if( itmapHandle != m_mapGetThread.end() )
			{
				hTread = itmapHandle->second;
			}
		}
	}
	return hTread;
}

CCecilyRabbitMQConnection* CCecilyRabbitMQProxy::GetMQConnection(const char* lpServerIP)
{
	CCecilyRabbitMQConnection* lpConnection = NULL;
	if ( NULL == lpServerIP || ( strcmp(lpServerIP,"") ==0 ) )
	{
		map<string,CCecilyRabbitMQConnection*>::iterator itMapConnection = m_mapConnection.begin();
		if ( itMapConnection != m_mapConnection.end() )
		{
			lpConnection = itMapConnection->second;
		}
	}else
	{
		map<string,CCecilyRabbitMQConnection*>::iterator itMapConnection = m_mapConnection.find(lpServerIP);
		if ( itMapConnection != m_mapConnection.end() )
		{
			lpConnection = itMapConnection->second;
		}
	}
	return lpConnection;
}


CecilyMQStatus CCecilyRabbitMQProxy::ReConnectMQ(const char* lpServerIP)
{
	map<string,AMQPAuthority>::iterator itMapAuthority;
	string strServerIP;
	AMQPAuthority authority;
	if ( NULL == lpServerIP || (strcmp(lpServerIP,"") == 0 ))
	{
		itMapAuthority = m_mapAuthority.begin();
	}else
	{
		itMapAuthority = m_mapAuthority.find(lpServerIP);
	}
	if ( itMapAuthority != m_mapAuthority.end() )
	{
		strServerIP = itMapAuthority->first;
		authority = itMapAuthority->second;
	}
	else
	{
		return Cecily_Authority_Failed;
	}
	Disconnect(strServerIP.c_str());
	return ConnectToMQ(authority.strServerIP.c_str(),authority.strVirtualHost.c_str(),authority.nPort,authority.strUser.c_str(),authority.strPassword.c_str());
}



//兼容老版本的MQ
void CCecilyRabbitMQProxy::MQConn(char* ip, int port, char* user, char* password, CLEARMQ_HCONN* hConn, long* code, long* reason)
{
	if ( NULL == ip || NULL == user || NULL == password ||  (port <= 0)  )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	CCecilyRabbitMQConnection* lpMQConnection = InitMQConnection(ip,"/",port,user,password);
	if ( NULL == lpMQConnection )
	{
		*code = Cecily_Connection_Init_Error;
		*reason = Cecily_Connection_Init_Error;
		return;
	}
	*hConn = m_sHConn++;
	CecilyMQStatus status =  lpMQConnection->TryConnect();
	if ( status == Cecily_Success )
	{
		lpMQConnection->SetHandleID(*hConn);
		m_longMapConnection.insert(pair<long,CCecilyRabbitMQConnection*>(*hConn,lpMQConnection));
	}else
	{
		delete lpMQConnection;
		lpMQConnection = NULL;
	}
	*code = status;
	*reason = status;
}

void CCecilyRabbitMQProxy::MQDisconn(CLEARMQ_HCONN* hConn, long options, long* code, long* reason)
{
	long l_conn = *hConn;
	map<long,CCecilyRabbitMQConnection*>::iterator itLongMap = m_longMapConnection.find(l_conn);
	CCecilyRabbitMQConnection* lpConnection = NULL;
	if ( itLongMap != m_longMapConnection.end() )
	{
		lpConnection = itLongMap->second;
	}
	if ( NULL == lpConnection )
	{
		return ;
	}
	CecilyMQStatus status = lpConnection->TryDisconnect();
// 	if ( Cecily_Success == status )
// 	{
// 		delete lpConnection;
// 		lpConnection = NULL;
// 	}
	*code = status;
	*reason = status;
}

CCecilyRabbitMQConnection* CCecilyRabbitMQProxy::GetMQConnection(long hConn)
{
	if ( hConn < 0 )
	{
		return NULL;
	}
	map<long,CCecilyRabbitMQConnection*>::iterator itLongMap = m_longMapConnection.find(hConn);
	CCecilyRabbitMQConnection* lpConnection = NULL;
	if ( itLongMap != m_longMapConnection.end() )
	{
		lpConnection = itLongMap->second;
	}
	return lpConnection;
}

void CCecilyRabbitMQProxy::MQPublish(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason)
{
	CCecilyRabbitMQConnection* lpConnection = GetMQConnection(hConn);
	if ( NULL == lpConnection || !lpConnection->IsConnected() )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	CecilyMQMD* _lpMQMQ = (CecilyMQMD*)msg_desc;
	if ( NULL == _lpMQMQ || NULL == buffer || buflen <= 0 || NULL == code || NULL == reason )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	lpConnection->MQ_Publish(_lpMQMQ,buffer,buflen,code,reason);

}

void CCecilyRabbitMQProxy::MQ_Recv_CB(CLEARMQ_HCONN hconn, RecvCallBack lpfunc,long *code, long* reason,void* user_data)
{

}

void CCecilyRabbitMQProxy::MQOpen_PubSub(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason)
{
	CecilyMQMD* _mqmd = (CecilyMQMD*)mqDesc;
	if ( NULL == _mqmd )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	AMQPExchangeQueue aeq;
	aeq.exchangeType = "fanout";
	aeq.queueName = _mqmd->send_queue_name;
	aeq.exchangeName = _mqmd->exchange_name;
	aeq.nQueueType = _mqmd->queue_type;   //0 为put   非0为get   发布订阅默认为都是 get
	CCecilyRabbitMQConnection* lpConnection =GetMQConnection(hConn);
	if ( NULL == lpConnection ||  !lpConnection->IsConnected() )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	CecilyAmqpObject *lpAMQPObject = lpConnection->GetPutObject();
	vector<AMQPExchangeQueue> vecExchangeQueue;
	bool bHasExchange = false;
	bool bHasQueue = false;
	bool bExchangeCreate = false;
	bool bQueueCreate = false;
	amqp_response_type_enum arte;
	string strErrorMsg;
	map<long,vector<AMQPExchangeQueue>>::iterator itMapExchangeQueue = m_longMapExchangeQueue.find(hConn);
	if ( itMapExchangeQueue != m_longMapExchangeQueue.end() )
	{
		vector<AMQPExchangeQueue>::iterator itVec = (itMapExchangeQueue->second).begin();
		for ( itVec; itVec != (itMapExchangeQueue->second).end(); itVec++ )
		{
			if ( strcmp( _mqmd->exchange_name,(*itVec).exchangeName.c_str() ) == 0 )
			{
				bHasExchange = true;
			}
			if ( strcmp( _mqmd->send_queue_name,(*itVec).queueName.c_str() ) ==0 )
			{
				bHasQueue = true;
			}
		}
	}
	if ( NULL != aeq.exchangeName.c_str() && ( strcmp(aeq.exchangeName.c_str(),"") != 0 ))
	{
		amqp_exchange_declare_ok_t* exRet =  amqp_exchange_declare(lpAMQPObject->clear_conn,
			AMQP_PUT_CHANNELS,
			amqp_cstring_bytes(aeq.exchangeName.c_str()),  //exchange name
			amqp_cstring_bytes(aeq.exchangeType.c_str()),           //exchange type
			false,		//passive
			true,		//durable
			false,		//auto_delete
			false,		//internal
			amqp_empty_table
			);
		arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
		if ( arte == AMQP_RESPONSE_NORMAL )
		{
			bExchangeCreate = true;
		}
	}
	amqp_queue_declare_ok_t *r = amqp_queue_declare(lpAMQPObject->clear_conn,
		AMQP_PUT_CHANNELS,
		amqp_cstring_bytes(aeq.queueName.c_str()),
		false,
		true,
		false,
		false,
		amqp_empty_table
		);
	arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
	if ( AMQP_RESPONSE_NORMAL == arte )
	{
		bQueueCreate = true;
	}
	amqp_queue_bind(lpAMQPObject->clear_conn,
		AMQP_PUT_CHANNELS,
		amqp_cstring_bytes(aeq.queueName.c_str()),
		amqp_cstring_bytes(aeq.exchangeName.c_str()),
		amqp_cstring_bytes(""),
		amqp_empty_table		
		);
	arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
	if ( AMQP_RESPONSE_NORMAL == arte )
	{
	}
	if ( itMapExchangeQueue != m_longMapExchangeQueue.end()  )
	{
		if (!bHasQueue)
		{
			(itMapExchangeQueue->second).push_back(aeq);
			lpConnection->AddExchangeQueue(aeq);
		}

	}
	else{
		vecExchangeQueue.push_back(aeq);
		lpConnection->AddExchangeQueue(aeq);
		m_longMapExchangeQueue.insert(pair<long,vector<AMQPExchangeQueue>>(hConn,vecExchangeQueue));
	}
	*code = Cecily_Success;
	*reason = Cecily_Success;

}

void CCecilyRabbitMQProxy::MQOpen(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason )
{
	CecilyMQMD* _mqmd = (CecilyMQMD*)mqDesc;
	if ( NULL == _mqmd )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	AMQPExchangeQueue aeq;
	aeq.exchangeType = "direct";
	aeq.exchangeName = _mqmd->exchange_name;
	aeq.nQueueType = _mqmd->queue_type;
	aeq.queueName = _mqmd->send_queue_name;
	CCecilyRabbitMQConnection* lpConnection =GetMQConnection(hConn);
	if ( NULL == lpConnection ||  !lpConnection->IsConnected() )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	CecilyAmqpObject *lpAMQPObject = lpConnection->GetPutObject();
	vector<AMQPExchangeQueue> vecExchangeQueue;
	bool bHasExchange = false;
	bool bHasQueue = false;
	bool bExchangeCreate = false;
	bool bQueueCreate = false;
	amqp_response_type_enum arte;
	string strErrorMsg;


	map<long,vector<AMQPExchangeQueue>>::iterator itMapExchangeQueue = m_longMapExchangeQueue.find(hConn);

	if ( itMapExchangeQueue != m_longMapExchangeQueue.end() )
	{
		vector<AMQPExchangeQueue>::iterator itVec = (itMapExchangeQueue->second).begin();
		for ( itVec; itVec != (itMapExchangeQueue->second).end(); itVec++ )
		{
			if ( strcmp( _mqmd->exchange_name,(*itVec).exchangeName.c_str() ) == 0 )
			{
				bHasExchange = true;
			}
			if ( strcmp( _mqmd->send_queue_name,(*itVec).queueName.c_str() ) ==0 )
			{
				bHasQueue = true;
			}
		}
	}
	
	if ( NULL != aeq.exchangeName.c_str() && ( strcmp(aeq.exchangeName.c_str(),"") != 0 ))
	{
		amqp_exchange_declare_ok_t* exRet =  amqp_exchange_declare(lpAMQPObject->clear_conn,
			AMQP_PUT_CHANNELS,
			amqp_cstring_bytes(aeq.exchangeName.c_str()),  //exchange name
			amqp_cstring_bytes(aeq.exchangeType.c_str()),           //exchange type
			false,		//passive
			true,		//durable
			false,		//auto_delete
			false,		//internal
			amqp_empty_table
			);
		arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
		if ( arte == AMQP_RESPONSE_NORMAL )
		{
			bExchangeCreate = true;
		}
	}
	
	amqp_queue_declare_ok_t *r = amqp_queue_declare(lpAMQPObject->clear_conn,
		AMQP_PUT_CHANNELS,
		amqp_cstring_bytes(aeq.queueName.c_str()),
		false,
		true,
		false,
		false,
		amqp_empty_table
		);
	arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
	if ( AMQP_RESPONSE_NORMAL == arte )
	{
		bQueueCreate = true;
	}
	amqp_queue_bind(lpAMQPObject->clear_conn,
		AMQP_PUT_CHANNELS,
		amqp_cstring_bytes(aeq.queueName.c_str()),
		amqp_cstring_bytes(aeq.exchangeName.c_str()),
		amqp_cstring_bytes(aeq.queueName.c_str()),
		amqp_empty_table		
		);
	
	arte = CheckLastOperation(amqp_get_rpc_reply(lpAMQPObject->clear_conn),"declare exchange", strErrorMsg);
	if ( AMQP_RESPONSE_NORMAL == arte )
	{
	}
	if ( itMapExchangeQueue != m_longMapExchangeQueue.end()  )
	{
		if (!bHasQueue)
		{
			(itMapExchangeQueue->second).push_back(aeq);
			lpConnection->AddExchangeQueue(aeq);
		}
		
	}
	else{
		vecExchangeQueue.push_back(aeq);
		lpConnection->AddExchangeQueue(aeq);
		m_longMapExchangeQueue.insert(pair<long,vector<AMQPExchangeQueue>>(hConn,vecExchangeQueue));
	}
	*code = Cecily_Success;
	*reason = Cecily_Success;
}

void CCecilyRabbitMQProxy::MQPut(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason)
{
	CCecilyRabbitMQConnection* lpConnection = GetMQConnection(hConn);
	if ( NULL == lpConnection || !lpConnection->IsConnected() )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	CecilyMQMD* _lpMQMQ = (CecilyMQMD*)msg_desc;
	if ( NULL == _lpMQMQ || NULL == buffer || buflen <= 0 || NULL == code || NULL == reason )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	lpConnection->MQ_SendMessag(_lpMQMQ,buffer,buflen,code,reason);
}

void CCecilyRabbitMQProxy::MQGet(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* datalen, long* code, long* reason)
{
	CCecilyRabbitMQConnection* lpConnection = GetMQConnection(hConn);
	bool bConnected = false;
	bConnected = lpConnection->IsConnected();
	if ( NULL == lpConnection || !bConnected )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return;
	}
	HANDLE hThread = GetCurrentHandle(hConn);
	if ( NULL == hThread )
	{
		//getmessage线程已经退出，要重新连接
		bool bFirst = true;
		vector<long>::iterator itVec = m_longvecThreadFlag.begin();
		for ( itVec ; itVec != m_longvecThreadFlag.end(); itVec++ )
		{
			long l_conn = *itVec;
			if ( hConn == l_conn )
			{
				bFirst = false;
				break;
			}
		}
		if ( !bFirst && NULL == lpConnection )
		{
			*code =  Cecily_Invalid_Connection;
			*reason = Cecily_Invalid_Connection;
			return ;
		}else
		{
			//第一次请求队列，创建get队列线程
			AMQPGetThreadParams* ThreadParam = new AMQPGetThreadParams();
			if ( NULL == ThreadParam )
			{
				*code =  Cecily_UnKnow_Error;
				*reason = Cecily_UnKnow_Error;
				return ;
			}
			//ThreadParam->m_strServerIP = strServerIP;
			ThreadParam->m_lpConnection = lpConnection;
			ThreadParam->m_lpMQProxy = this;
			hThread = CreateThread(NULL,
				0,
				RecvThread_MQ,
				(LPVOID)ThreadParam,
				NULL,
				NULL);
			{
				CSmartLocker locker(&m_ThreadLocker,TRUE);
				SetCurrentHandle(hConn,hThread);
// 				CloseHandle(m_eventHandles[1]);
// 				m_eventHandles[1] = NULL;
// 				m_eventHandles[1] = CreateEvent(NULL, TRUE, FALSE,NULL);
				CloseHandle(lpConnection->m_connEventHandles[1]);
				lpConnection->m_connEventHandles[1] = NULL;
				lpConnection->m_connEventHandles[1] = CreateEvent(NULL, TRUE, FALSE,NULL);
			}
			if ( bFirst )
			{
				m_longvecThreadFlag.push_back(hConn);
			}
			
		}	
	}
	if ( NULL == hThread )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return;
	}
	DWORD dwWait;
	int timeout = 0;
	CecilyMQMD* _lpMQMD = (CecilyMQMD*)msg_desc;
	if ( timeout <= 0  )
	{
		dwWait = WaitForMultipleObjects(AMQP_HANDLE_EVENTS,lpConnection->m_connEventHandles,false,INFINITE);
	}else
	{
		dwWait = WaitForMultipleObjects(AMQP_HANDLE_EVENTS,lpConnection->m_connEventHandles,false,timeout);
	}
	if( dwWait == WAIT_OBJECT_0)
	{
		
		CCecilyAMQPMessage* message = NULL;
		message = lpConnection->GetCacheMessage();
		//PopMessage(&message);
		if ( NULL != message )
		{
			string strQueueName = message->GetQueueName();
			string strMessageID = message->GetMessageID();
			unsigned int nBufferSize = message->GetMessageBufferSize();
			unsigned int nQueueLength = strlen(strQueueName.c_str()) +1;
			unsigned int nMessageIDlength = strlen(strMessageID.c_str());
			memcpy(_lpMQMD->response_queue_name,strQueueName.c_str(),strQueueName.length());
			memcpy_s(_lpMQMD->message_id,CLEARMQ_MSGID_LENGTH,strMessageID.c_str(),strMessageID.length());
			memcpy(buffer,message->GetMessageBufferPtr(),nBufferSize);
			*datalen = nBufferSize;
		}
		delete message;
		message = NULL;
		return ;
	}
	else if ( dwWait ==WAIT_OBJECT_0 + 1 )  //get线程退出了连接断开
	{
		ResetEvent(lpConnection->m_connEventHandles[1]);
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	else if ( dwWait == WAIT_TIMEOUT )
	{
		*code = Cecily_GetMessage_OutTime;
		*reason = Cecily_GetMessage_OutTime;
		return ;
	}

}

void CCecilyRabbitMQProxy::MQReconnect(CLEARMQ_HCONN hConn,long* code, long* reason)
{
	CCecilyRabbitMQConnection* lpConnection = GetMQConnection(hConn);
	if ( NULL == lpConnection )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return;
	}
	lpConnection->TryDisconnect();
	lpConnection->TryConnect();
	lpConnection->ResetConsumer();

}


//兼容老版本MQ