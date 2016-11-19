/********************************************************************
	created:	  2016/10/20
	filename: 	  ClearCMSTemplate.cpp
	author:		  ÀîÎÄÓí
	E-Mail£º	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "ClearCMSTemplate.h"
#include "ClearMessageListerner.h"

ClearCMSTemplate::ClearCMSTemplate(void):
m_lpActivemqRecvCB(NULL),
m_lpActivemqRecvCBData(NULL)
{
}

ClearCMSTemplate::~ClearCMSTemplate(void)
{
}

void ClearCMSTemplate::ClearReceiveExecutor::doInCms(cms::Session* session)
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

cms::Destination* ClearCMSTemplate::ClearResolveReceiveExecutor::getDestination(cms::Session* session) {

	try {
		return parent->resolveDestinationName(session, ResolvedestinationName);
	}
	catch(...)
	{
		return NULL;
	}
// 	CMSTEMPLATE_CATCH(IllegalStateException, parent)
// 		CMSTEMPLATE_CATCHALL(parent)
}


void ClearCMSTemplate::Async_Receive(const std::string& destinationName)
{
	try
	{
		bool bRegistered = IsRegistered(destinationName);
		if ( bRegistered )
		{
			return ;
		}
		ClearResolveReceiveExecutor receiveExecutor(this, "", isNoLocal(), destinationName);
		execute(&receiveExecutor);
	}
	catch (...)
	{
		int a = 10;
	}

}

void ClearCMSTemplate::SetActiveMQRecvCB(activemq_recv_cb lpFunc, void* user_data)
{
	m_lpActivemqRecvCB = lpFunc;
	m_lpActivemqRecvCBData = user_data;
}

void ClearCMSTemplate::Register(const std::string& strDestionation,cms::MessageConsumer* consumer)
{
	if ( strDestionation == "" || NULL == consumer )
	{
		return ;
	}
	ClearMessageListerner* lpListerner = new ClearMessageListerner(this,(char*)strDestionation.c_str());
	consumer->setMessageListener(lpListerner);
	bool bRegistered = IsRegistered(strDestionation);
	if (!bRegistered)
	{
		CSmartLocker locker(&m_csMapLocker,TRUE);
		m_mapConsumer.insert(pair<string,cms::MessageConsumer*>(strDestionation,consumer));
	}		
}

void ClearCMSTemplate::UnRegiser(const std::string &strDestionationName)
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
		destroyConsumer(consumer);
		m_mapConsumer.erase(itMap);
		vector<ClearMessageListerner*>::iterator itVec = m_vecListerner.begin();
		for ( itVec ; itVec != m_vecListerner.end(); itVec++ )
		{
			ClearMessageListerner* lpListerner = *itVec;
			char* lpDestionation = lpListerner->GetDestionationName();
			if ( strcmp(lpDestionation,strDestionationName.c_str()) == 0 )
			{
				m_vecListerner.erase(itVec);
			}
		}
	}
	
}

bool ClearCMSTemplate::IsRegistered(const std::string &strDestination)
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