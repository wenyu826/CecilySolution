/********************************************************************
	created:	  2016/11/01
	filename: 	  CecilyBaseProxy.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Base_Proxy_H
#define Cecily_Base_Proxy_H
#include "CecilyMQ.h"

class CecilyBaseProxy
{
public:
	CecilyBaseProxy()
	{
		m_MQType = MQ_Rabbit;
	}
	virtual ~CecilyBaseProxy()
	{

	}

	virtual void MQConn(char* ip, int port, char* user, char* password, CLEARMQ_HCONN* hConn, long* code, long* reason) = 0;
	virtual void MQDisconn(CLEARMQ_HCONN* hConn, long options, long* code, long* reason) = 0;
	virtual void MQOpen(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason ) = 0;
	virtual void MQPut(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason) = 0;
	virtual void MQGet(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* datalen, long* code, long* reason) = 0;
	virtual void MQReconnect(CLEARMQ_HCONN hConn,long* code, long* reason) = 0;
	virtual void MQPublish(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason) = 0;
	virtual void MQOpen_PubSub(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason) = 0;
	virtual void MQ_Recv_CB(CLEARMQ_HCONN hconn, RecvCallBack lpfunc,long *code, long* reason,void* user_data) = 0;

public:
	MQ_Type GetMQType()
	{
		return m_MQType;
	}

protected:
	MQ_Type		m_MQType;

	
};

#endif