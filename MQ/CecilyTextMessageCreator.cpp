/********************************************************************
	created:	  2016/10/24
	filename: 	  CecilyTextMessageCreator.cpp
	author:		  ÀîÎÄÓí
	E-Mail£º	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "CecilyTextMessageCreator.h"

CecilyTextMessageCreator::CecilyTextMessageCreator(const char* lpText)
{
	if ( NULL == lpText )
	{
		m_strText = "";
	}else{
		m_strText = lpText;
	}
}

CecilyTextMessageCreator::~CecilyTextMessageCreator(void)
{
}

cms::Message* CecilyTextMessageCreator::createMessage( cms::Session* session )
{
	cms::Message* message = NULL;

	if( session != NULL ) {
		message = session->createTextMessage(m_strText.c_str());
	}

	return message;

}
