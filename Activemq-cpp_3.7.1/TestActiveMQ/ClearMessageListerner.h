/********************************************************************
	created:	  2016/10/20
	filename: 	  ClearMessageListerner.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Clear_Message_Listerner_H
#define Clear_Message_Listerner_H
#include "ClearActiveMQHelp.h"
#include "ClearCMSTemplate.h"

class ClearCMSTemplate;
class ClearMessageListerner:public MessageListener,
	public ExceptionListener
{
public:
	ClearMessageListerner(ClearCMSTemplate* lpTemplate,char* lpDestionation);
	virtual ~ClearMessageListerner(void);

	virtual void onMessage(const Message* message);
	virtual void onException(const cms::CMSException& ex);

	char* GetDestionationName()
	{
		return m_lpDestionation;
	}

private:
	ClearCMSTemplate*	m_lpParentTemplate;
	char*	m_lpDestionation;
};

#endif