/********************************************************************
	created:	  2016/10/20
	filename: 	  ClearMessageListerner.cpp
	author:		  ÀîÎÄÓí
	E-Mail£º	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "ClearMessageListerner.h"

ClearMessageListerner::ClearMessageListerner(ClearCMSTemplate* lpTemplate,char* lpDestionation):
m_lpParentTemplate(lpTemplate),
m_strDestionation(lpDestionation)
{
}

ClearMessageListerner::~ClearMessageListerner(void)
{
}

void ClearMessageListerner::onMessage(const Message* message)
{
	ClearMessage* lpMessage = new ClearMessage();
	if ( NULL != m_lpParentTemplate )
	{
		if ( m_lpParentTemplate->isPubSubDomain() )
		{
			lpMessage->nType = A_Topic;
		}else{
			lpMessage->nType = A_Queue;
		}
		lpMessage->strDestionNation = m_strDestionation;
		lpMessage->message = message->clone();	
		activemq_recv_cb func = m_lpParentTemplate->GetActiveMQRecvCB();
		void* data = m_lpParentTemplate->GetActiveMQRecvCBData();
		if ( NULL != func )
		{
			func(lpMessage,data);
		}
	}else
	{
		delete lpMessage;
		lpMessage = NULL;
	}
}

void ClearMessageListerner::onException(const cms::CMSException& ex)
{
	string strMsg = ex.getMessage();
	if ( NULL != m_lpParentTemplate )
	{
		string strDestionationName = m_strDestionation;
		m_lpParentTemplate->UnRegiser(strDestionationName);
		m_lpParentTemplate->Async_Receive(strDestionationName);
	}
}
