/********************************************************************
	created:	  2016/10/24
	filename: 	  CecilyBytesMessageCreator.h
	author:		  ÀîÎÄÓí
	E-Mail£º	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Bytes_Message_Creator_H
#define Cecily_Bytes_Message_Creator_H
#include "CecilyActiveMQHelp.h"

class CecilyBytesMessageCreator : public activemq::cmsutil::MessageCreator
{
public:
	CecilyBytesMessageCreator(unsigned char* lpBytes, int nSize);
	virtual ~CecilyBytesMessageCreator(void);

	virtual cms::Message* createMessage( cms::Session* session ); 

private:
	unsigned char* m_lpBytes;
	unsigned int m_nSize;
};

#endif
