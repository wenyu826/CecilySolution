/********************************************************************
	created:	  2016/10/20
	filename: 	  ClearActiveMQManager.cpp
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "ClearActiveMQManager.h"


ClearActiveMQManager::ClearActiveMQManager(void)
{
}

ClearActiveMQManager::~ClearActiveMQManager(void)
{
}


void ClearActiveMQManager::InitActiveMQ()
{
	activemq::library::ActiveMQCPP::initializeLibrary();
}

void ClearActiveMQManager::UnInitActiveMQ()
{
	activemq::library::ActiveMQCPP::shutdownLibrary();
}

VOID ClearActiveMQManager::Received_Message_CB(ClearMessage* lpMessage, void* user_data)
{
	ClearActiveMQManager* lpMQManager = (ClearActiveMQManager*)user_data;
	if ( NULL == lpMQManager )
	{
		if ( NULL != lpMessage )
		{
			delete lpMessage;
			lpMessage = NULL;
		}
	}
}

bool ClearActiveMQManager::ConnectToMQ(const char* lpBrokerURL)
{
	if ( NULL == lpBrokerURL || strlen(lpBrokerURL) == 0 )
	{
		return false;
	}
	auto_ptr<ConnectionFactory> connectionFactory(
		ConnectionFactory::createCMSConnectionFactory(lpBrokerURL));

	// Create a Connection
	cms::Connection* connection = NULL;
	try
	{
		connection = connectionFactory->createConnection();
		if ( NULL == connection )
		{
			return false;
		}
	}
	catch (activemq::exceptions::ActiveMQException* e)
	{
		e->getCause();
		std::string strExceptionMsg = e->getMessage();
		int a = 10;
	}
	catch(cms::CMSException* e1)
	{
		string strMsg = e1->getMessage();
		int b = 20;
	}
	catch(cms::CMSException& e2)
	{
		string strMsg = e2.getMessage();
		int a = 20;
	}
	catch(...)
	{
		int b = 20;
	}
	try
	{
		connection->start();
	}
	catch(...)
	{

		int a = 10;
	}
	
	cms::Session* session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
	if ( NULL == session )
	{
		return false;
	}

	return true;
}