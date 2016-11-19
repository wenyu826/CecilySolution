// CecilyMQ.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CecilyMQ.h"
#include "CecilyRabbitMQProxy.h"
#include "CecilyActiveMQProxy.h"

#define Cecily_Check_MQ_Selected(proxy,code,reason)\
	if(NULL ==code || NULL == reason){\
	return ;\
	}\
	if( NULL == proxy ){\
	*code = Cecily_MQ_Not_Selected;\
	*reason = Cecily_MQ_Not_Selected;\
	return;\
	}\
	*code = Cecily_Success;\
	*reason =Cecily_Success;


CecilyBaseProxy* g_lpMQProxy = NULL;

CCecilyRabbitMQProxy MQProxy;

const char* clear_error_msg[] = {
	"success",						//Cecily_Success
	"The input parameter errors",					//Cecily_Params_Error
	"occur a error during Initialize the Connection Object",  //Cecily_Connection_Init_Error
	"Connection has not been Initialized",                      //Cecily_Connection_Not_Inited
	"Connection has not been established with Server",			//Cecily_Connection_Has_Connected
	"generate new socket error",								//
	"ACCESS_REFUSED --Login was refused  please check your authentication",							//Cecily_Authority_Failed
	"occur a library exception ",						//Cecily_Library_Exception
	"Connect to Server Failed",							//Cecily_Connect_Failed
	"Open channel Failed",								//Cecily_Open_Channel_Failed
	"Close channel Failed",								//Cecily_Channel_Close_Failed
	"Destory Connection Failed",						//Cecily_Connection_Destory_Failed
	"Put Message into the queue is failed",				//Cecily_Put_Message_Error
	"Get Message from queue is failed",					//Cecily_Get_Message_Error
	"Server has not been connected",					//Cecily_Server_Not_Connected
	"The Connection is invalid",						//Cecily_Invalid_Connection
	"Get Message outtime",								//Cecily_GetMessage_OutTime
	"Unknow error",										//Cecily_UnKnow_Error
	"Not Select MQ Type"								//Cecily_MQ_Not_Selected

};

/*
Cecily_Authority_Failed    =   server connection error 403h, message: ACCESS_REFUSED - Login was refused using authentication mechanism PLAIN. For details see the broker logfile.
          

*/


CLEAR_MQ_API void Cecily_SetMQ_Type(MQ_Type type)
{
	if ( NULL != g_lpMQProxy )
	{
		return ;
	}
	switch(type)
	{
	case MQ_Rabbit:
		g_lpMQProxy = new CCecilyRabbitMQProxy();
		break;
	case MQ_Active:
		g_lpMQProxy = new CecilyActiveMQProxy();
		break;
	default:
		break;
	}
}

//¼æÈÝ¾É°æ±¾MQ
CLEAR_MQ_API void Cecily_MQConn(char* ip, int port, char* user, char* password, CLEARMQ_HCONN* hConn, long* code, long* reason)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( NULL == ip || port <= 0 || NULL == user || NULL == password || NULL == hConn || NULL == code )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQConn(ip,port,user,password,hConn,code,reason);
}

CLEAR_MQ_API void Cecily_MQDisconn(CLEARMQ_HCONN* hConn, long options, long* code, long* reason)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( NULL == hConn )
	{
		return;
	}
	g_lpMQProxy->MQDisconn(hConn,options,code,reason);
}

CLEAR_MQ_API void Cecily_MQOpen(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason )
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if (  NULL == mqDesc )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQOpen(hConn,mqDesc,options,hMQ,code,reason);
}

CLEAR_MQ_API void Cecily_MQPut(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( NULL == msg_desc )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQPut(hConn,hMQ,msg_desc,msg_options,buflen,buffer,code,reason);
}

CLEAR_MQ_API void Cecily_MQGet(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* datalen, long* code, long* reason)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( NULL == buffer || NULL == datalen )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQGet(hConn,hMQ,msg_desc,msg_options,buflen,buffer,datalen,code,reason);
}

CLEAR_MQ_API void Cecily_MQReconnect(CLEARMQ_HCONN hConn,long* code, long* reason)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( hConn  <= 0 ||  NULL == code ||  NULL == reason )
	{
		*code = Cecily_Params_Error;
		*code = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQReconnect(hConn,code,reason);
}

CLEAR_MQ_API void Cecily_MQPublish(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( NULL == msg_desc || NULL == buffer || buflen <= 0 )
	{
		*code = Cecily_Params_Error;
		*reason = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQPublish(hConn,hMQ,msg_desc,msg_options,buflen,buffer,code,reason);

}

CLEAR_MQ_API void Cecily_MQ_Receive_CB(CLEARMQ_HCONN hconn, RecvCallBack lpfunc,long *code, long* reason,void* user_data)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( NULL == lpfunc || hconn <= 0 )
	{
		*code = Cecily_Params_Error;
		*code = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQ_Recv_CB(hconn,lpfunc,code,reason,user_data);
}

CLEAR_MQ_API void Cecily_MQOpen_PubSub(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason)
{
	Cecily_Check_MQ_Selected(g_lpMQProxy,code,reason);
	if ( NULL == mqDesc || hConn <= 0 )
	{
		*code = Cecily_Params_Error;
		*code = Cecily_Params_Error;
		return ;
	}
	g_lpMQProxy->MQOpen_PubSub(hConn,mqDesc,NULL,NULL,code,reason);

}