/********************************************************************
	created:	  2016/10/20
	filename: 	  CecilyCMSTemplate.cpp
	author:		  ÀîÎÄÓí
	E-Mail£º	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "CecilyCMSTemplate.h"
#include "CecilyMessageListerner.h"

CecilyCMSTemplate::CecilyCMSTemplate(void):
m_lpActivemqRecvCB(NULL),
m_lpActivemqRecvCBData(NULL)
{
}

CecilyCMSTemplate::~CecilyCMSTemplate(void)
{
	FreeResource();
}

void CecilyCMSTemplate::CecilyReceiveExecutor::doInCms(cms::Session* session)
{
	cms::MessageConsumer* consumer = NULL;

	try {
		consumer = parent->createConsumer(session, getDestination(session), selector, noLocal);
		if ( strDestination == "" )
		{
			strDestination = parent->getDefaultDestinationName();
		}
		if ( strDestination != "" && NULL != consumer)
		{
			parent->Register(strDestination,consumer);
		}
		

	} catch (CMSException& e) {

		e.setMark(__FILE__, __LINE__);

		throw;
	}
}

cms::Destination* CecilyCMSTemplate::CecilyResolveReceiveExecutor::getDestination(cms::Session* session) {

	try {
		return parent->resolveDestinationName(session, ResolvedestinationName);
	}
	catch(...)
	{
		return NULL;
	}
}


void CecilyCMSTemplate::Async_Receive(const std::string& destinationName)
{
	try
	{
		bool bRegistered = IsRegistered(destinationName);
		if ( bRegistered )
		{
			return ;
		}
		CecilyResolveReceiveExecutor receiveExecutor(this, "", isNoLocal(), destinationName);
		execute(&receiveExecutor);
	}
	catch (...)
	{
		int a = 10;
	}

}

void CecilyCMSTemplate::SetActiveMQRecvCB(activemq_recv_cb lpFunc, void* user_data)
{
	m_lpActivemqRecvCB = lpFunc;
	m_lpActivemqRecvCBData = user_data;
}

void CecilyCMSTemplate::Register(const std::string& strDestionation,cms::MessageConsumer* consumer)
{
	if ( strDestionation == "" || NULL == consumer )
	{
		return ;
	}
	CecilyMessageListerner* lpListerner = new CecilyMessageListerner(this,(char*)strDestionation.c_str());
	cms::Connection* lpCurrentConnection =  getConnection();
	lpCurrentConnection->setExceptionListener(lpListerner);
	consumer->setMessageListener(lpListerner);
	bool bRegistered = IsRegistered(strDestionation);
	if (!bRegistered)
	{
		CSmartLocker locker(&m_csMapLocker,TRUE);
		m_mapConsumer.insert(pair<string,cms::MessageConsumer*>(strDestionation,consumer));
		m_vecListerner.push_back(lpListerner);
	}		
}

void CecilyCMSTemplate::UnRegiser(const std::string &strDestionationName)
{
	bool bRegistered = IsRegistered(strDestionationName);
	if (!bRegistered )
	{
		return ;
	}
	MessageConsumer* consumer;
	{
		CSmartLocker locker(&m_csMapLocker,TRUE);
		map<string,cms::MessageConsumer*>::iterator itMap = m_mapConsumer.find(strDestionationName);
		if ( itMap != m_mapConsumer.end() )
		{
			consumer = itMap->second;
		}
		if ( NULL == consumer )
		{
			return ;
		}
// 		activemq::core::ActiveMQConnection* lpCurrentConnection =  dynamic_cast<activemq::core::ActiveMQConnection*>(getConnection());
// 		if ( NULL != lpCurrentConnection )
// 		{
// 			activemq::core::kernels::ActiveMQSessionKernel* lpSession = (activemq::core::kernels::ActiveMQSessionKernel*)takeSession();
// 			activemq::core::kernels::ActiveMQConsumerKernel* lpKernelConsumer = (activemq::core::kernels::ActiveMQConsumerKernel* )consumer;
// 			if ( NULL != lpSession && NULL != lpKernelConsumer )
// 			{
// 				Pointer<activemq::core::kernels::ActiveMQConsumerKernel> tmp = Pointer<activemq::core::kernels::ActiveMQConsumerKernel>(lpKernelConsumer);
// 				lpSession->removeConsumer(tmp);
// 			}
// 		}
		destroyConsumer(consumer);
		m_mapConsumer.erase(itMap);
		vector<CecilyMessageListerner*>::iterator itVec = m_vecListerner.begin();
		for ( itVec ; itVec != m_vecListerner.end(); itVec++ )
		{
			CecilyMessageListerner* lpListerner = *itVec;
			const char* lpDestionation = lpListerner->GetDestionationName();
			if ( strcmp(lpDestionation,strDestionationName.c_str()) == 0 )
			{
				break;
			}
		}
		if ( itVec != m_vecListerner.end() )
		{
			CecilyMessageListerner* lpListerner = *itVec;
			delete lpListerner;
			lpListerner = NULL;
			m_vecListerner.erase(itVec);
		}
	}
	
}

bool CecilyCMSTemplate::IsRegistered(const std::string &strDestination)
{
	{
		CSmartLocker locker(&m_csMapLocker,TRUE);
		map<string,cms::MessageConsumer*>::iterator itMap = m_mapConsumer.find(strDestination);
		if ( itMap != m_mapConsumer.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void CecilyCMSTemplate::FreeResource()
{
	MessageConsumer* consumer;
	{
		CSmartLocker locker(&m_csMapLocker,TRUE);
		string strDestionationName;
		map<string,cms::MessageConsumer*>::iterator itMap = m_mapConsumer.begin();
		for ( itMap; itMap != m_mapConsumer.end(); itMap ++ )
		{
			strDestionationName = itMap->first;
			consumer = itMap->second;
			destroyConsumer(consumer);
		}
		vector<CecilyMessageListerner*>::iterator itVec = m_vecListerner.begin();
		for ( itVec ; itVec != m_vecListerner.end(); itVec++ )
		{
			CecilyMessageListerner* lpListerner = *itVec;
			delete lpListerner;
			lpListerner = NULL;
		}		
		m_mapConsumer.clear();	
		m_vecListerner.clear();
	}

}

bool CecilyCMSTemplate::DeleteDestination(string strDestination,bool bSubPub)
{
	cms::Destination* lpDestination = NULL;
	try
	{
		activemq::core::ActiveMQConnection* lpCurrentConnection =  dynamic_cast<activemq::core::ActiveMQConnection*>(getConnection());
		cms::Session* lpSession = takeSession();
		if ( NULL == lpCurrentConnection ||  NULL == lpSession )
		{
			return false;
		}
		if ( bSubPub )
		{
			lpDestination = lpSession->createTopic(strDestination.c_str());
		}
		else{
			lpDestination = lpSession->createQueue(strDestination.c_str());
		}
		UnRegiser(strDestination);
		if ( NULL != lpDestination )
		{
			lpCurrentConnection->destroyDestination(lpDestination);
		}
		else{
			return false;
		}
	}
	catch(...)
	{
		delete lpDestination;
		lpDestination =NULL;
		return false;

	}
	delete lpDestination;
	lpDestination =NULL;
	return true;

}