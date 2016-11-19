/********************************************************************
	created:	  2016/11/01
	filename: 	  CecilyActiveMQProxy.cpp
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#include "CecilyActiveMQProxy.h"

bool CecilyActiveMQProxy::m_sInitActiveMQ = false;
long CecilyActiveMQProxy::m_sHConn = 10000;

CecilyActiveMQProxy::CecilyActiveMQProxy(void)
{
	m_lpMemoryPool = NULL;
	m_MQType = MQ_Active;
	try{
		if ( NULL == m_lpMemoryPool )
		{
			m_lpMemoryPool =  new CCecilyMemoryPool();
		}		
		CecilyActiveMQProxy::InitActiveMQ();
	}
	catch(...)
	{

	}
}

CecilyActiveMQProxy::~CecilyActiveMQProxy(void)
{
}

void CecilyActiveMQProxy::InitActiveMQ()
{
	if ( !m_sInitActiveMQ )
	{
		activemq::library::ActiveMQCPP::initializeLibrary();
		m_sInitActiveMQ = true;
	}
}

void CecilyActiveMQProxy::UnInitActiveMQ()
{
	if ( m_sInitActiveMQ )
	{
		activemq::library::ActiveMQCPP::shutdownLibrary();
		m_sInitActiveMQ = false;
	}
}

void CecilyActiveMQProxy::MQConn(char* ip, int port, char* user, char* password, CLEARMQ_HCONN* hConn, long* code, long* reason)
{
	if ( NULL == ip || port <= 0   )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	char szBrokerURI[256] = {0};
	try
	{
		sprintf_s(szBrokerURI,256,"tcp://%s:%d",ip,port);
		ConnectionFactory* connectionFactory = ConnectionFactory::createCMSConnectionFactory(szBrokerURI);
		cms::Connection* connection = NULL;
		connection = connectionFactory->createConnection();
		if ( NULL == connection )
		{
			*code = Cecily_Server_Not_Connected;
			*reason = Cecily_Server_Not_Connected;
			return ;
		}
		cms::Session* session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
		if ( NULL == session )
		{
			*code = Cecily_Server_Not_Connected;
			*reason = Cecily_Server_Not_Connected;
			return ;
		}
		Cecily_Delete_Ptr(session);
		Cecily_Delete_Ptr(connection);
		Cecily_Delete_Ptr(connectionFactory);

	}
	catch(CMSException& ex)
	{
		*code = Cecily_Server_Not_Connected;
		*reason = Cecily_Server_Not_Connected;
		return ;
	}
	catch(...)
	{
		*code = Cecily_Server_Not_Connected;
		*reason = Cecily_Server_Not_Connected;
		return ;
	}
	try
	{
		memset(szBrokerURI,0,256);
		sprintf_s(szBrokerURI,256,"failover:(tcp://%s:%d)",ip,port);
		long nIndex = InterlockedIncrement(&m_sHConn);
		*hConn = nIndex;
		CecilyActiveMQConnection* MQConnection = new CecilyActiveMQConnection(m_lpMemoryPool,szBrokerURI,user,password);
		if ( NULL == MQConnection )
		{
			*code = Cecily_Server_Not_Connected;
			*reason = Cecily_Server_Not_Connected;
			return ;
		}
		MQConnection->InitCMSTemplates();
		SetMQConnection(nIndex,MQConnection);
	
	}
	catch(...)
	{
		*code = Cecily_Server_Not_Connected;
		*reason = Cecily_Server_Not_Connected;
		return ;

	}
}

void CecilyActiveMQProxy::MQDisconn(CLEARMQ_HCONN* hConn, long options, long* code, long* reason)
{


}

void CecilyActiveMQProxy::MQGet(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* datalen, long* code, long* reason)
{


}

void CecilyActiveMQProxy::MQOpen(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason )
{
	CecilyMQMD* _mqmd = (CecilyMQMD*)mqDesc;
	CecilyActiveMQConnection* MQConnection = GetMQConnection(hConn);
	if ( NULL == _mqmd || NULL == MQConnection )
	{
		*code = Cecily_Params_Error;
		*code = Cecily_Params_Error;
		return;
	}
	if ( _mqmd->queue_type == 0 )   
	{
		return ;
	}
	MQConnection->OpenMQ(_mqmd->send_queue_name,1);

}

void CecilyActiveMQProxy::MQPut(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason)
{
	CecilyActiveMQConnection* MQConnection = GetMQConnection(hConn);
	if ( NULL == MQConnection )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	CecilyMQMD* _lpMQMQ = (CecilyMQMD*)msg_desc;
	MQConnection->MQ_SendMessage(_lpMQMQ,buffer,buflen,code,reason);

}

void CecilyActiveMQProxy::MQReconnect(CLEARMQ_HCONN hConn,long* code, long* reason)
{

}

void CecilyActiveMQProxy::MQPublish(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason)
{
	CecilyActiveMQConnection* MQConnection = GetMQConnection(hConn);
	if ( NULL == MQConnection )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	CecilyMQMD* _lpMQMQ = (CecilyMQMD*)msg_desc;
	MQConnection->MQ_Publish(_lpMQMQ,buffer,buflen,code,reason);
}

void CecilyActiveMQProxy::MQOpen_PubSub(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason)
{
	CecilyMQMD* _mqmd = (CecilyMQMD*)mqDesc;
	CecilyActiveMQConnection* MQConnection = GetMQConnection(hConn);
	if ( NULL == _mqmd || NULL == MQConnection )
	{
		*code = Cecily_Params_Error;
		*code = Cecily_Params_Error;
		return;
	}
	MQConnection->OpenMQ(_mqmd->send_queue_name,0);
}

void CecilyActiveMQProxy::MQ_Recv_CB(CLEARMQ_HCONN hconn, RecvCallBack lpfunc,long *code, long* reason,void* user_data)
{
	CecilyActiveMQConnection* MQConnection = GetMQConnection(hconn);
	if ( NULL == MQConnection )
	{
		*code = Cecily_Invalid_Connection;
		*reason = Cecily_Invalid_Connection;
		return ;
	}
	MQConnection->SetRecvCallBack(lpfunc,user_data);
}

CecilyActiveMQConnection* CecilyActiveMQProxy::GetMQConnection(long hConn)
{
	CecilyActiveMQConnection* obj = NULL;
	{
		CSmartLocker locker(&m_csMapConnection,TRUE);
		map<long,CecilyActiveMQConnection*>::iterator itMap = m_mapConnection.find(hConn);
		if ( itMap != m_mapConnection.end() )
		{
			obj = itMap->second;
		}
	}
	return obj;
}

void CecilyActiveMQProxy::SetMQConnection(long hConn, CecilyActiveMQConnection* lpConnection)
{
	if ( hConn <= 0 || NULL == lpConnection )
	{
		return ;
	}
	{
		CSmartLocker locker(&m_csMapConnection,TRUE);
		map<long,CecilyActiveMQConnection*>::iterator itMap = m_mapConnection.find(hConn);
		if ( itMap != m_mapConnection.end() )
		{
			itMap->second = lpConnection;
		}
		else
		{
			m_mapConnection.insert(pair<long,CecilyActiveMQConnection*>(hConn,lpConnection));
		}
	}
}
