/********************************************************************
	created:	  2016/10/20
	filename: 	  CecilyMessageListerner.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Message_Listerner_H
#define Cecily_Message_Listerner_H
#include "CecilyActiveMQHelp.h"
#include "CecilyCMSTemplate.h"

class CecilyCMSTemplate;
class CecilyMessageListerner:public MessageListener,
	public ExceptionListener
{
public:
	CecilyMessageListerner(CecilyCMSTemplate* lpTemplate,char* lpDestionation);
	virtual ~CecilyMessageListerner(void);

	virtual void onMessage(const Message* message);
	virtual void onException(const cms::CMSException& ex);

	const char* GetDestionationName()
	{
		return m_strDestionation.c_str();
	}

private:
	CecilyCMSTemplate*	m_lpParentTemplate;
	string	m_strDestionation;
};

#endif