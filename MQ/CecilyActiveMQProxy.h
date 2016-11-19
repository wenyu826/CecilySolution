/********************************************************************
	created:	  2016/11/01
	filename: 	  CecilyActiveMQProxy.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_ActiveMQ_Proxy_H
#define Cecily_ActiveMQ_Proxy_H
#include "CecilyMQ.h"
#include "CecilyBaseProxy.h"
#include "CecilyActiveMQHelp.h"
#include "CecilyActiveMQConnection.h"
#include "SmartLocker.h"
#include "CecilyMemoryPool.h"

#include <map>
#include "Log.h"

using namespace std;





class CecilyActiveMQProxy : public CecilyBaseProxy
{
public:
	CecilyActiveMQProxy(void);
	virtual ~CecilyActiveMQProxy(void);

	static void InitActiveMQ();
	static void UnInitActiveMQ();

	virtual void MQConn(char* ip, int port, char* user, char* password, CLEARMQ_HCONN* hConn, long* code, long* reason);
	virtual void MQDisconn(CLEARMQ_HCONN* hConn, long options, long* code, long* reason);
	virtual void MQOpen(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason );
	virtual void MQPut(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason);
	virtual void MQGet(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* datalen, long* code, long* reason);
	virtual void MQReconnect(CLEARMQ_HCONN hConn,long* code, long* reason);
	virtual void MQPublish(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason);
	virtual void MQOpen_PubSub(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason);
	virtual void MQ_Recv_CB(CLEARMQ_HCONN hconn, RecvCallBack lpfunc,long *code, long* reason,void* user_data);

private:
	CecilyActiveMQConnection* GetMQConnection(long hConn);
	void SetMQConnection(long hConn, CecilyActiveMQConnection* lpConnection);
	CCecilyMemoryPool* GetMemoryPool()
	{
		return m_lpMemoryPool;
	}
private:
	static bool m_sInitActiveMQ;
	static long m_sHConn;

	CLocker	m_csMapConnection;
	map<long,CecilyActiveMQConnection*>	m_mapConnection;

	CCecilyMemoryPool* m_lpMemoryPool;
	
};

#endif
