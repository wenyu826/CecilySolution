#ifndef CLEAR_MQ_H_15DFS1452
#define CLEAR_MQ_H_15DFS1452
#include <string>
using namespace std;

#ifdef CLEARMQ_EXPORTS
#define CLEAR_MQ_API extern "C"  __declspec(dllexport)
#else
#define CLEAR_MQ_API extern "C"  __declspec(dllimport)
#endif


typedef void(*clear_mq_receive)(const char* lpQueueName, void* lpBuffer,int nLen, int nType);

enum CecilyMQStatus
{
	Cecily_Success = 0,
	Cecily_Params_Error,
	Cecily_Connection_Init_Error,
	Cecily_Connection_Not_Inited,
	Cecily_Connection_Has_Connected,
	Cecily_Socket_Open_Failed,
	Cecily_Authority_Failed,
	Cecily_Library_Exception,
	Cecily_Connect_Failed,
	Cecily_Open_Channel_Failed,
	Cecily_Channel_Close_Failed,
	Cecily_Connection_Close_Failed,
	Cecily_Connection_Destory_Failed,
	Cecily_Put_Message_Error,
	Cecily_Get_Message_Error,
	Cecily_Server_Not_Connected,
	Cecily_Invalid_Connection,
	Cecily_GetMessage_OutTime,
	Cecily_UnKnow_Error,
	Cecily_MQ_Not_Selected
};

typedef long CLEARMQ_HCONN;
typedef long CLEARMQ_HMQ;
typedef unsigned char CLEARMQ_UC;

#ifdef _WIN32
#define CLEARMQ_INT32    int
#define CLEARMQ_UINT32	unsigned int
#else
#endif

#define CLEARMQ_QNAME_LENGTH  32
#define CLEARMQ_MSGID_LENGTH  40
#define CLEARMQ_MSGLBL_LENGTH 32
#define CLEARMQ_MSG_TYPE
#define Rabbit_ExchangeName_Len		32
#define Rabbit_QueueName_Len		32

enum MQ_Type
{
	MQ_Rabbit = 2000,
	MQ_Active
};

enum CecilyExchangeType
{
	Direct = 10000,   //指定队列模式  P2P
	Topic
};

enum CecilyMessageType
{
	M_LONG = 0,
	M_SHORT,
	M_CHAR,
	M_Text ,        //纯文本
	M_WText,
	M_Bytes,     //字节流
	M_Map
};


#pragma pack(1)
typedef struct tagCecilyMQMD
{
	char		body_type;
	char		message_id[CLEARMQ_MSGID_LENGTH];
	char		response_queue_name[CLEARMQ_QNAME_LENGTH];
	char		reply_to[CLEARMQ_QNAME_LENGTH];
	char		correlation_id[CLEARMQ_MSGID_LENGTH];    //关联的消息ID
	char		send_queue_name[CLEARMQ_QNAME_LENGTH];
	char		exchange_name[CLEARMQ_QNAME_LENGTH];
	char		message_priority;	//优先级
	CLEARMQ_UINT32	timestamp;			//消息时间戳
	CLEARMQ_INT32	delivery_mode;		//发送模式  
	CLEARMQ_INT32	expiration;			//消息生命周期
	CLEARMQ_INT32    distribution_type;				//队列 或者 发布订阅    参照CecilyExchangeType的值
	CLEARMQ_INT32	queue_type;   //0  put  非0 get  

	tagCecilyMQMD()
	{
		body_type = M_Bytes;
		memset(message_id,0,CLEARMQ_MSGID_LENGTH);
		memset(response_queue_name,0,CLEARMQ_QNAME_LENGTH);
		memset(reply_to,0,CLEARMQ_QNAME_LENGTH);
		memset(correlation_id,0,CLEARMQ_MSGID_LENGTH);
		memset(send_queue_name,0,CLEARMQ_QNAME_LENGTH);
		memset(exchange_name,0,CLEARMQ_QNAME_LENGTH);
		message_priority = 4;
		timestamp = 0;
		delivery_mode = 2;
		expiration = 60 * 10;
		distribution_type = Direct;
		queue_type = 0;
	}

}CecilyMQMD;

#pragma pack()

typedef void(*RecvCallBack)(CecilyMQMD* , void*, int, void* user_data);

//新增接口  设置MQ类型
CLEAR_MQ_API void Cecily_SetMQ_Type(MQ_Type type);
CLEAR_MQ_API void Cecily_MQPublish(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason);   //发布订阅
CLEAR_MQ_API void Cecily_MQ_Receive_CB(CLEARMQ_HCONN hconn, RecvCallBack lpfunc,long *code, long* reason,void *user_data);    //暂时仅仅适用于activemq
CLEAR_MQ_API void Cecily_MQOpen_PubSub(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason);	//用于打开发布订阅的队列信息
//新增接口  设置MQ类型 end
//兼容老版本的MQ
CLEAR_MQ_API void Cecily_MQConn(char* ip, int port, char* user, char* password, CLEARMQ_HCONN* hConn, long* code, long* reason);
CLEAR_MQ_API void Cecily_MQDisconn(CLEARMQ_HCONN* hConn, long options, long* code, long* reason);
CLEAR_MQ_API void Cecily_MQOpen(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason );
CLEAR_MQ_API void Cecily_MQPut(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason);
CLEAR_MQ_API void Cecily_MQGet(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* datalen, long* code, long* reason);
CLEAR_MQ_API void Cecily_MQReconnect(CLEARMQ_HCONN hConn,long* code, long* reason);
//兼容老版本的MQ  end

#endif