/********************************************************************
	created:	  2016/10/20
	filename: 	  ClearActiveMQHelp.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Clear_ActiveMQ_Help_H
#define Clear_ActiveMQ_Help_H

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

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

#define Clear_Delete_Ptr(ptr)\
	if(NULL != ptr){\
	delete ptr;\
	ptr = NULL;\
	}

enum ActiveType
{
	A_Topic,
	A_Queue
};


typedef struct _ClearMessage 
{
	string strDestionNation;
	ActiveType nType;
	cms::Message* message;
	_ClearMessage()
	{
		message = NULL;
	}
	~_ClearMessage()
	{
		if ( NULL != message )
		{
			delete message;
			message = NULL;
		}
	}
}ClearMessage;

typedef void(*activemq_recv_cb)(ClearMessage* lpClearMessage,void* user_data);


#endif