/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/27
*  File:           CecilyMQConnection.cpp
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#include "StdAfx.h"

#include "CecilyRabbitMQConnection.h"
#include "log.h"



CCecilyRabbitMQConnection::CCecilyRabbitMQConnection(const char* lpIpAddress, const char* lpVirtualHost, unsigned int nPort , const char* lpUser,const char* lpPassword)
{
	m_lphandle = -1;
	m_strIP = lpIpAddress;
	m_strVirtualHost = lpVirtualHost;
	m_nPort = nPort;
	m_strUser = lpUser;
	m_strPwd = lpPassword;
	m_putObject = new CecilyAmqpObject();
	m_putObject->nChannel = AMQP_PUT_CHANNELS;
	m_getObject = new CecilyAmqpObject();
	m_getObject->nChannel = AMQP_GET_CHANNELS;
	m_bInit = false;
	m_bConnect = false;
	m_bStopConsumer = false;
	m_connEventHandles[0] = CreateSemaphore(NULL,  0,  MAX_MSG_NUM,  NULL);
	m_connEventHandles[1] = CreateEvent(NULL, TRUE, FALSE,NULL);
	m_nCacheDepth = 0;
}

CCecilyRabbitMQConnection::~CCecilyRabbitMQConnection(void)
{
	if ( NULL != m_putObject )
	{
		delete m_putObject;
		m_putObject = NULL;
	}
	if ( NULL != m_getObject )
	{
		delete m_getObject;
		m_getObject = NULL;
	}
}

bool CCecilyRabbitMQConnection::Init()
{
	if ( m_bInit )
	{
		return true;
	}
	if ( NULL == m_putObject || NULL == m_getObject )
	{
		return false;
	}

	m_putObject->clear_conn = amqp_new_connection();
	if ( NULL == m_putObject->clear_conn )
	{
		return false;
	}
	m_putObject->clear_socket = amqp_tcp_socket_new(m_putObject->clear_conn);
	if ( !m_putObject->clear_socket )
	{
		return false;
	}

	m_getObject->clear_conn = amqp_new_connection();
	if ( NULL == m_getObject->clear_conn )
	{
		return false;
	}
	m_getObject->clear_socket = amqp_tcp_socket_new(m_getObject->clear_conn);
	if ( !m_getObject->clear_socket )
	{
		return false;
	}
	m_bInit = true;
	return true;
}


CecilyMQStatus CCecilyRabbitMQConnection::TryConnect()
{
	Init();
	if ( !m_bInit )
	{
		return Cecily_Connection_Not_Inited;
	}
	if ( m_bConnect )
	{
		return Cecily_Connection_Has_Connected;
	}	
	
	int status = amqp_socket_open(m_putObject->clear_socket, m_strIP.c_str(), m_nPort);
	if ( status != AMQP_STATUS_OK )
	{
		return Cecily_Socket_Open_Failed;
	}
	amqp_rpc_reply_t rt = amqp_login(m_putObject->clear_conn, m_strVirtualHost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, m_strUser.c_str(), m_strPwd.c_str());
	if ( rt.reply_type !=  AMQP_RESPONSE_NORMAL )
	{
		if ( rt.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION )
		{
			char szErrorMsg[ERROR_MSG_BUFFER] = {0};
			if ( rt.reply.id == AMQP_CONNECTION_CLOSE_METHOD )
			{
				amqp_connection_close_t *m = (amqp_connection_close_t *) rt.reply.decoded;
				sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"server connection error %uh, message: %.*s\n",m->reply_code,
					(int)m->reply_text.len,
					(char*)m->reply_text.bytes);
			}
			else if ( rt.reply.id == AMQP_CHANNEL_CLOSE_METHOD )
			{
				amqp_channel_close_t *m = (amqp_channel_close_t *) rt.reply.decoded;
				sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"server channel error %uh, message: %.*s\n",
					m->reply_code,
					(int) m->reply_text.len, (char *) m->reply_text.bytes);
			}
			return Cecily_Authority_Failed;
		}
		else if ( rt.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION )
		{
			int nErrorCode = rt.library_error;
			const char* lpErrorMsg = amqp_error_string2(nErrorCode);
			return Cecily_Library_Exception;
		}
		return Cecily_Connect_Failed;		
	}
	amqp_channel_open(m_putObject->clear_conn,AMQP_PUT_CHANNELS);
	rt = amqp_get_rpc_reply(m_putObject->clear_conn);
	if ( rt.reply_type != AMQP_RESPONSE_NORMAL )
	{
		return Cecily_Open_Channel_Failed;
	}

	status = amqp_socket_open(m_getObject->clear_socket, m_strIP.c_str(), m_nPort);
	if ( status != AMQP_STATUS_OK )
	{
		return Cecily_Socket_Open_Failed;
	}
	rt = amqp_login(m_getObject->clear_conn, m_strVirtualHost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, m_strUser.c_str(), m_strPwd.c_str());
	if ( rt.reply_type !=  AMQP_RESPONSE_NORMAL )
	{
		if ( rt.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION )
		{
			char szErrorMsg[ERROR_MSG_BUFFER] = {0};
			if ( rt.reply.id == AMQP_CONNECTION_CLOSE_METHOD )
			{
				amqp_connection_close_t *m = (amqp_connection_close_t *) rt.reply.decoded;
				sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"server connection error %uh, message: %.*s\n",m->reply_code,
					(int)m->reply_text.len,
					(char*)m->reply_text.bytes);
			}
			else if ( rt.reply.id == AMQP_CHANNEL_CLOSE_METHOD )
			{
				amqp_channel_close_t *m = (amqp_channel_close_t *) rt.reply.decoded;
				sprintf_s(szErrorMsg,ERROR_MSG_BUFFER,"server channel error %uh, message: %.*s\n",
					m->reply_code,
					(int) m->reply_text.len, (char *) m->reply_text.bytes);
			}
			return Cecily_Authority_Failed;
		}
		else if ( rt.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION )
		{
			int nErrorCode = rt.library_error;
			const char* lpErrorMsg = amqp_error_string2(nErrorCode);
			return Cecily_Library_Exception;
		}
		return Cecily_Connect_Failed;
	}
	amqp_channel_open(m_getObject->clear_conn,AMQP_GET_CHANNELS);
	rt = amqp_get_rpc_reply(m_getObject->clear_conn);
	if ( rt.reply_type != AMQP_RESPONSE_NORMAL )
	{
		return Cecily_Open_Channel_Failed;
	}
	m_bConnect = true;
	return Cecily_Success;
}

CecilyMQStatus CCecilyRabbitMQConnection::TryDisconnect()
{
	amqp_rpc_reply_t rt;
	StopConsumer();
	if ( NULL != m_putObject )
	{
		if ( NULL != m_putObject->clear_conn )
		{
			try
			{
				rt = amqp_channel_close(m_putObject->clear_conn,AMQP_PUT_CHANNELS,AMQP_REPLY_SUCCESS);
// 				if ( rt.reply_type != AMQP_RESPONSE_NORMAL )
// 				{
// 					return Cecily_Channel_Close_Failed;
// 				}
				rt = amqp_connection_close(m_putObject->clear_conn, AMQP_REPLY_SUCCESS);
// 				if ( rt.reply_type != AMQP_RESPONSE_NORMAL )
// 				{
// 					return Cecily_Connection_Close_Failed;
// 				}
				int status = amqp_destroy_connection(m_putObject->clear_conn);
// 				if ( status != AMQP_STATUS_OK )
// 				{
// 					return Cecily_Connection_Destory_Failed;
// 				}
				m_putObject->clear_socket = NULL;
				m_putObject->clear_conn = NULL;

			}
			catch(...)
			{
				int a = 10;

			}
			
		}
// 		delete m_putObject;
// 		m_putObject = NULL;
		
	}
	if ( NULL != m_getObject )
	{
		if ( NULL != m_getObject->clear_conn )
		{
			try
			{
// 				vector<string>::iterator itVec = m_vecConsumerQueues.begin();
// 				for ( itVec; itVec != m_vecConsumerQueues.end(); itVec++ )
// 				{
// 					string strConsumer = *itVec;
// 					amqp_basic_cancel(m_getObject->clear_conn,AMQP_GET_CHANNELS,amqp_cstring_bytes(strConsumer.c_str()));
// 				}
				while(true)
				{
					if ( !IsStopConsumer() )    //表示消费者线程已经退出了
					{
						break;
					}
					Sleep(1000);
				}
				
				rt = amqp_channel_close(m_getObject->clear_conn,AMQP_GET_CHANNELS,AMQP_REPLY_SUCCESS);
// 				if ( rt.reply_type != AMQP_RESPONSE_NORMAL )
// 				{
// 					return Cecily_Channel_Close_Failed;
// 				}
				rt = amqp_connection_close(m_getObject->clear_conn, AMQP_REPLY_SUCCESS);
// 				if ( rt.reply_type != AMQP_RESPONSE_NORMAL )
// 				{
// 					return Cecily_Connection_Close_Failed;
// 				}
				int status = amqp_destroy_connection(m_getObject->clear_conn);
// 				if (status != AMQP_STATUS_OK )
// 				{
// 					return Cecily_Connection_Destory_Failed;
// 				}
				m_getObject->clear_socket = NULL;
				m_getObject->clear_conn = NULL;
			}
			catch(...)
			{
				int a = 10;
			}
			
		}
// 		delete m_getObject;
// 		m_getObject = NULL;
// 		
	}
	m_bConnect = false;
	m_bInit = false;
	return Cecily_Success;
}


/*兼容老版本MQ*/
void CCecilyRabbitMQConnection::MQ_SendMessag(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason)
{
	if ( !IsConnected() )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return;
	}
	vector<AMQPExchangeQueue>::iterator itVec = m_vecExchangeQueues.begin();
	bool bExistExchange = false;
	bool bExistQueue = false;
	for ( itVec; itVec != m_vecExchangeQueues.end(); itVec++ )
	{
		AMQPExchangeQueue aeq = *itVec;
		if ( strcmp(aeq.exchangeName.c_str(),lpMQMD->exchange_name) == 0 || strcmp("",lpMQMD->exchange_name) == 0 || strcmp("amq.fanout",lpMQMD->exchange_name) == 0  )
		{
			bExistExchange = true;
		}
// 		if ( strcmp(aeq.queueName.c_str(),lpMQMD->szSendQueueName) == 0 )
// 		{
// 			bExistQueue = true;
// 		}
	}
	if ( !bExistExchange /*|| !bExistQueue*/)
	{
		*code =  Cecily_Put_Message_Error;
		*reason = Cecily_Put_Message_Error;
		return ;
	}
	CecilyAmqpObject* lpPutObject = GetPutObject();
	if ( NULL == lpPutObject )
	{
		*code =  Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	amqp_basic_properties_t_ props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
		AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_MESSAGE_ID_FLAG;
	if ( lpMQMD->body_type == M_Text )
	{
		props.content_type = amqp_cstring_bytes("text");
	}
	else if ( lpMQMD->body_type == M_Bytes )
	{
		props.content_type = amqp_cstring_bytes("bytes");
	}
	CLog *pLog = CLog::GetInstance();

// 	props.headers.num_entries = 2;
// 	string strTotalPackage = "total_packages";
// 	props.headers.entries = new struct amqp_table_entry_t_[2];
// 	props.headers.entries[0].key = amqp_cstring_bytes(strTotalPackage.c_str());
// 	props.headers.entries[0].value.value.i32 = lpMQMD->total_package;
// 
// 	string strIndex = "package_index";
// 	props.headers.entries[1].key = amqp_cstring_bytes(strIndex.c_str());
// 	props.headers.entries[1].value.value.i32 = lpMQMD->index;
	if (strcmp(lpMQMD->message_id,"") == 0 )
	{
		string strMessageID = CAssistUtility::GenerateGUID();
		memcpy_s(lpMQMD->message_id,CLEARMQ_MSGID_LENGTH,strMessageID.c_str(),strMessageID.length());
	}
	amqp_bytes_t messageid =amqp_cstring_bytes(lpMQMD->message_id);
	props.message_id = messageid;

	props.delivery_mode = 2; /* persistent delivery mode */
	amqp_bytes_t body;
	body.len = nBufferLen;
	body.bytes = (void*)buffer;
	int status;
	try
	{
		
		status = amqp_basic_publish(lpPutObject->clear_conn,
			AMQP_PUT_CHANNELS,
			amqp_cstring_bytes(lpMQMD->exchange_name),
			amqp_cstring_bytes(lpMQMD->send_queue_name),
			0,
			0,
			&props,
			body);
		if ( status == AMQP_STATUS_OK)
		{
			static int nCount = 1;
			if ( NULL != pLog )
			{
				pLog->WriteLog(__LINE__,LOGLEVEL_DEBUG,"发送成功数量%d\n",nCount++);
			}
			//ATLTRACE("发送成功数量%d\n",nCount++);
			*code = Cecily_Success;
			*reason = Cecily_Success;
		}
		else
		{
			static int nFailed = 1;
			if ( NULL != pLog )
			{
				pLog->WriteLog(__LINE__,LOGLEVEL_DEBUG,"发送失败%d\n",nFailed++);
			}
			//TRACE("发送失败%d\n",nFailed++);
			*code = Cecily_Put_Message_Error;
			*reason = Cecily_Put_Message_Error;
		}

	}
	catch(...)
	{
		*code = Cecily_Put_Message_Error;
		*reason = Cecily_Put_Message_Error;
	}
	

}

void CCecilyRabbitMQConnection::MQ_Publish(CecilyMQMD* lpMQMD,void * buffer, int nBufferLen, long* code, long* reason)
{
	if ( !IsConnected() )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return;
	}
	vector<AMQPExchangeQueue>::iterator itVec = m_vecExchangeQueues.begin();
	bool bExistExchange = false;
	bool bExistQueue = false;
	for ( itVec; itVec != m_vecExchangeQueues.end(); itVec++ )
	{
		AMQPExchangeQueue aeq = *itVec;
		if ( strcmp(aeq.exchangeName.c_str(),lpMQMD->exchange_name) == 0 || strcmp("",lpMQMD->exchange_name) == 0 || strcmp("amq.fanout",lpMQMD->exchange_name) == 0  )
		{
			bExistExchange = true;
		}
		// 		if ( strcmp(aeq.queueName.c_str(),lpMQMD->szSendQueueName) == 0 )
		// 		{
		// 			bExistQueue = true;
		// 		}
	}
	if ( !bExistExchange /*|| !bExistQueue*/)
	{
		*code =  Cecily_Put_Message_Error;
		*reason = Cecily_Put_Message_Error;
		return ;
	}
	CecilyAmqpObject* lpPutObject = GetPutObject();
	if ( NULL == lpPutObject )
	{
		*code =  Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	amqp_basic_properties_t_ props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG |
		AMQP_BASIC_DELIVERY_MODE_FLAG ;
	if ( lpMQMD->body_type == M_Text )
	{
		props.content_type = amqp_cstring_bytes("text");
	}
	else if ( lpMQMD->body_type == M_Bytes )
	{
		props.content_type = amqp_cstring_bytes("bytes");
	}

	props.delivery_mode = 2; /* persistent delivery mode */
	amqp_bytes_t body;
	body.len = nBufferLen;
	body.bytes = (void*)buffer;
	int status;
	try
	{
		status = amqp_basic_publish(lpPutObject->clear_conn,
			AMQP_PUT_CHANNELS,
			amqp_cstring_bytes(lpMQMD->exchange_name),
			amqp_cstring_bytes(lpMQMD->send_queue_name),
			0,
			0,
			&props,
			body);
		if ( status == AMQP_STATUS_OK)
		{
			*code = Cecily_Success;
			*reason = Cecily_Success;
		}
		else
		{
			*code = Cecily_Put_Message_Error;
			*reason = Cecily_Put_Message_Error;
		}

	}
	catch(...)
	{
		*code = Cecily_Put_Message_Error;
		*reason = Cecily_Put_Message_Error;
	}

}





/*兼容老版本MQ*/