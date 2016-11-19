/********************************************************************
	created:	  2016/10/24
	filename: 	  ClearTextMessageCreator.cpp
	author:		  ÀîÎÄÓí
	E-Mail£º	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "ClearTextMessageCreator.h"

ClearTextMessageCreator::ClearTextMessageCreator(const char* lpText)
{
	if ( NULL == lpText )
	{
		m_strText = "";
	}else{
		m_strText = lpText;
	}
}

ClearTextMessageCreator::~ClearTextMessageCreator(void)
{
}

cms::Message* ClearTextMessageCreator::createMessage( cms::Session* session )
{
	cms::Message* message = NULL;

	if( session != NULL ) {
		message = session->createTextMessage(m_strText.c_str());
	}

	return message;

}
