/********************************************************************
	created:	  2016/10/24
	filename: 	  ClearBytesMessageCreator.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Clear_Bytes_Message_Creator_H
#define Clear_Bytes_Message_Creator_H
#include "ClearActiveMQHelp.h"

class ClearBytesMessageCreator : public activemq::cmsutil::MessageCreator
{
public:
	ClearBytesMessageCreator(unsigned char* lpBytes, int nSize);
	virtual ~ClearBytesMessageCreator(void);

	virtual cms::Message* createMessage( cms::Session* session ); 

private:
	unsigned char* m_lpBytes;
	unsigned int m_nSize;
};

#endif
