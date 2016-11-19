/********************************************************************
	created:	  2016/10/20
	filename: 	  CecilyCMSTemplate.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_CMS_Template_H
#define Cecily_CMS_Template_H
#include "CecilyActiveMQHelp.h"

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Long.h>
#include <decaf/util/Date.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/core/kernels/ActiveMQConsumerKernel.h>
#include <activemq/util/Config.h>
#include <activemq/library/ActiveMQCPP.h>
#include <activemq/cmsutil/CmsTemplate.h>
#include <activemq/cmsutil/MessageCreator.h>
#include <activemq/cmsutil/SessionCallback.h>
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
#include "SmartLocker.h"

#include <map>
#include <vector>
#define CLEAR_AMQCPP_UNUSED

using namespace activemq;
using namespace activemq::core;
using namespace activemq::cmsutil;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;
#pragma warning(disable:C4101) 

class TextMessageCreator: public activemq::cmsutil::MessageCreator {
public:

	TextMessageCreator() : MessageCreator() {}

	virtual ~TextMessageCreator() {}

	virtual cms::Message* createMessage( cms::Session* session ) {

		cms::Message* message = NULL;

		if( session != NULL ) {
			message = session->createTextMessage("test text message");
		}

		return message;
	}
};


class CecilyMessageListerner;

class CecilyCMSTemplate :public cmsutil::CmsTemplate
{
public:
	CecilyCMSTemplate(void);
	virtual ~CecilyCMSTemplate(void);


    class CecilyReceiveExecutor;
    friend class CecilyReceiveExecutor;
    class CecilyReceiveExecutor : public SessionCallback {
    protected:

        cms::Destination* destination;
        std::string selector;
        bool noLocal;
        cms::Message* message;
        CecilyCMSTemplate* parent;
		std::string strDestination;

    private:

        CecilyReceiveExecutor(const CecilyReceiveExecutor&);
        CecilyReceiveExecutor& operator=(const CecilyReceiveExecutor&);

    public:

        CecilyReceiveExecutor(CecilyCMSTemplate* parent, cms::Destination* destination,
			const std::string& selector, bool noLocal,const std::string& _strDestionation) :
            SessionCallback(), destination(destination), selector(selector), noLocal(noLocal), message(NULL), parent(parent),strDestination(_strDestionation) {
        }

        virtual ~CecilyReceiveExecutor() {}

		virtual void doInCms(cms::Session* session);

        virtual cms::Destination* getDestination(cms::Session* session CLEAR_AMQCPP_UNUSED) {
            return destination;
        }

        cms::Message* getMessage() {
            return message;
        }
		
    };


	class CecilyResolveReceiveExecutor;
	friend class CecilyResolveReceiveExecutor;
	class CecilyResolveReceiveExecutor : public CecilyReceiveExecutor {
	private:

		std::string ResolvedestinationName;

	private:

		CecilyResolveReceiveExecutor(const CecilyResolveReceiveExecutor&);
		CecilyResolveReceiveExecutor& operator=(const CecilyResolveReceiveExecutor&);

	public:

		CecilyResolveReceiveExecutor(CecilyCMSTemplate* parent, const std::string& selector,
			bool noLocal, const std::string& destinationName) :
		CecilyReceiveExecutor(parent, NULL, selector, noLocal,destinationName),ResolvedestinationName(destinationName) {
		}

		virtual ~CecilyResolveReceiveExecutor() {}

		virtual cms::Destination* getDestination(cms::Session* session);
	};


public:
	void Async_Receive(const std::string& destinationName);

	void SetActiveMQRecvCB(activemq_recv_cb lpFunc, void* user_data);
	activemq_recv_cb GetActiveMQRecvCB()
	{
		return m_lpActivemqRecvCB;
	}

	void* GetActiveMQRecvCBData()
	{
		return m_lpActivemqRecvCBData;
	}

	bool DeleteDestination(string strDestination,bool bSubPub);


public:
	void Register(const std::string &strDestionation,cms::MessageConsumer* consumer);
	void UnRegiser(const std::string &strDestionationName);
	bool IsRegistered(const std::string &strDestination);

private:
	void FreeResource();

private:
	std::map<string,cms::MessageConsumer*>	m_mapConsumer;
	std::vector<CecilyMessageListerner*>	m_vecListerner;
	CLocker	m_csMapLocker;
	activemq_recv_cb	m_lpActivemqRecvCB;
	void* m_lpActivemqRecvCBData;
};


#endif