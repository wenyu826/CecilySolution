/********************************************************************
	created:	  2016/10/24
	filename: 	  CecilyTextMessageCreator.h
	author:		  ÀîÎÄÓí
	E-Mail£º	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Text_Message_Creator_H
#define Cecily_Text_Message_Creator_H
#include "CecilyActiveMQHelp.h"

class CecilyTextMessageCreator : public activemq::cmsutil::MessageCreator
{
public:
	CecilyTextMessageCreator(const char* lpText);
	virtual ~CecilyTextMessageCreator(void);

	virtual cms::Message* createMessage( cms::Session* session ); 

private:
	string m_strText;
};

#endif