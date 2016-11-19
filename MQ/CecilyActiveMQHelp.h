/********************************************************************
	created:	  2016/10/20
	filename: 	  CecilyActiveMQHelp.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_ActiveMQ_Help_H
#define Cecily_ActiveMQ_Help_H

#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <activemq/cmsutil/CmsTemplate.h>
#include <activemq/cmsutil/MessageCreator.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/commands/ActiveMQMessage.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

#ifdef _DEBUG
#pragma comment(lib,"libactivemq-cppd.lib")
#else
#pragma comment(lib,"libactivemq-cpp.lib")
#endif

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

#define Cecily_Default_Toptic		"Topic.Cecily"
#define Cecily_Default_Queue			"Queue.Cecily"

#define Cecily_Delete_Ptr(ptr)\
	if(NULL != ptr){\
	delete ptr;\
	ptr = NULL;\
	}

enum ActiveType
{
	A_Topic,
	A_Queue
};


typedef struct _CecilyMessage 
{
	string strDestionNation;
	ActiveType nType;
	cms::Message* message;
	_CecilyMessage()
	{
		message = NULL;
	}
	~_CecilyMessage()
	{
		if ( NULL != message )
		{
			delete message;
			message = NULL;
		}
	}
}CecilyMessage;

typedef void(*activemq_recv_cb)(CecilyMessage* lpCecilyMessage,void* user_data);


#endif