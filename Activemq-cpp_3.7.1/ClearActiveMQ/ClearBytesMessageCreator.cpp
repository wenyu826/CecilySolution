/********************************************************************
	created:	  2016/10/24
	filename: 	  ClearBytesMessageCreator.cpp
	author:		  ������
	E-Mail��	  hynulwy@126.com
	description:  
*********************************************************************/
#include "StdAfx.h"
#include "ClearBytesMessageCreator.h"

ClearBytesMessageCreator::ClearBytesMessageCreator(unsigned char* lpBytes, int nSize)
{
	m_lpBytes = NULL;
	m_nSize = 0;
	if ( NULL != lpBytes && nSize > 0 )
	{
		m_lpBytes = new unsigned char[nSize+1];
		memset(m_lpBytes,0,nSize+1);
		m_nSize = nSize;
		memcpy(m_lpBytes,lpBytes,nSize);
	}
}

ClearBytesMessageCreator::~ClearBytesMessageCreator(void)
{
	if ( NULL != m_lpBytes )
	{
		delete []m_lpBytes;
		m_lpBytes = NULL;
	}
}

cms::Message* ClearBytesMessageCreator::createMessage( cms::Session* session )
{
	cms::Message* message = NULL;
	if ( NULL != session )
	{
		message = session->createBytesMessage(m_lpBytes,m_nSize);
	}
	return message;
}
