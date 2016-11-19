/********************************************************************
	created:	  2016/10/20
	filename: 	  ClearActiveMQManager.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Clear_ActiveMQ_Manager_H
#define Clear_ActiveMQ_Manager_H
#include "ClearActiveMQHelp.h"
#include "ClearCMSTemplate.h"


#define Clear_Administrator_Toptic		"administrator.clear"



class ClearActiveMQManager
{
public:
	ClearActiveMQManager(void);
	virtual ~ClearActiveMQManager(void);

	static void InitActiveMQ();
	static void UnInitActiveMQ();

	bool ConnectToMQ(const char* lpBrokerURL);

private:
	static void Received_Message_CB(ClearMessage* lpMessage, void* user_data);

private:
	std::string		m_strBrokerURL;
	ClearCMSTemplate*	m_TopicConsumer;
	ClearCMSTemplate*	m_QueueConsumer;
	ClearCMSTemplate*	m_TopicProduer;
	ClearCMSTemplate*	m_QueueProducer;

};


#endif