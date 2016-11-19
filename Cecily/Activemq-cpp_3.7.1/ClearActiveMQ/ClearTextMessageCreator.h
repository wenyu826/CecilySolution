/********************************************************************
	created:	  2016/10/24
	filename: 	  ClearTextMessageCreator.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Clear_Text_Message_Creator_H
#define Clear_Text_Message_Creator_H
#include "ClearActiveMQHelp.h"

class ClearTextMessageCreator : public activemq::cmsutil::MessageCreator
{
public:
	ClearTextMessageCreator(const char* lpText);
	virtual ~ClearTextMessageCreator(void);

	virtual cms::Message* createMessage( cms::Session* session ); 

private:
	string m_strText;
};

#endif