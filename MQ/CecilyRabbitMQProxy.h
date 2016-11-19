/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/26
*  File:           CecilyMQProxy.h
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#ifndef Cecily_RabbitMQ_Proxy_H
#define Cecily_RabbitMQ_Proxy_H

#include <WinSock2.h>
#include "stdint.h"
#include "amqp_tcp_socket.h"
#include "amqp.h"
#include "amqp_framing.h"
#include <string>
#include <vector>
#include <queue>
#include <map>
#include "SmartLocker.h"
#include "CecilyMQ.h"
#include "CecilyMemoryPool.h"
#include "CecilyAMQPMessage.h"
#include "CecilyBaseProxy.h"

using namespace std;
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"rabbitmq-static.lib")


#define AMQP_PUT_CHANNELS		5
#define AMQP_GET_CHANNELS		AMQP_PUT_CHANNELS+1
#define ERROR_MSG_BUFFER		1024
#define AMQP_PREFETCH_COUNT		1
#define AMQP_MAX_QUEUE_LENGTH   50
#define AMQP_HANDLE_EVENTS		2
#define AMQP_DEFAULT_EXPIRATION	60*1000

#define MAX_MSG_NUM		10000

typedef struct tagAMQPExchangeQueue
{
	string exchangeName;
	string exchangeType;
	string queueName;
	int nQueueType;
	tagAMQPExchangeQueue()
	{
		exchangeName = "";
		exchangeType = "";
		queueName = "";
		int nQueueType = 0;
	}
	tagAMQPExchangeQueue& operator=(const tagAMQPExchangeQueue& other)
	{
		this->exchangeName = other.exchangeName;
		this->exchangeType = other.exchangeType;
		this->queueName = other.queueName;
		return *this;
	}
}AMQPExchangeQueue;

struct AMQPAuthority
{
	string strServerIP;
	string strVirtualHost;
	unsigned int nPort;
	string strUser;
	string strPassword;
};

class CCecilyRabbitMQConnection;

class CCecilyRabbitMQProxy : public CecilyBaseProxy
{
public:
	CCecilyRabbitMQProxy(void);
	virtual ~CCecilyRabbitMQProxy(void);

public:
	bool PurgeQueue(const string& strQueueName); //清洗队列
	CecilyMQStatus SendMessage(const char* lpQueueName,const void* lpBuffer, int nLen, CecilyMessageType nMessageType, const char* lpServerIP);
	CecilyMQStatus ConnectToMQ(const char* lpIPAddress, const char* lpVirtualHost, unsigned int nPort, const char* lpUser,const char* lpPassword);
	CecilyMQStatus Disconnect(const char* lpIpAddress);
	CecilyMQStatus GetMessage(char** lpQueueName, char** lpMessageBuffer, int timeout = 0, const char* lpServerIP = NULL);
	bool DeclareExchangeQueue(CecilyExchangeType& type,const char* lpExchangeName,const char* lpQueueName, int QueueType,const char* lpServerIP = NULL);
	CecilyMQStatus ReConnectMQ(const char* lpServerIP = NULL);


	/*兼容老版本的MQ*/
	virtual void MQConn(char* ip, int port, char* user, char* password, CLEARMQ_HCONN* hConn, long* code, long* reason);
	virtual void MQDisconn(CLEARMQ_HCONN* hConn, long options, long* code, long* reason);
	virtual void MQOpen(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason );
	virtual void MQPut(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason);
	virtual void MQGet(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* datalen, long* code, long* reason);
	virtual void MQReconnect(CLEARMQ_HCONN hConn,long* code, long* reason);
	virtual void MQPublish(CLEARMQ_HCONN hConn, CLEARMQ_HMQ hMQ, void* msg_desc, void* msg_options, long buflen, void* buffer, long* code, long* reason);
	virtual void MQOpen_PubSub(CLEARMQ_HCONN hConn, void* mqDesc, long options, CLEARMQ_HMQ* hMQ, long* code, long* reason);
	virtual void MQ_Recv_CB(CLEARMQ_HCONN hconn, RecvCallBack lpfunc,long *code, long* reason,void* user_data);
	//CecilyMQStatus MQ_SendMessage(const char* lpExchangeName, const char* lpQueueName,const void* lpBuffer, int nLen, CecilyMessageType nMessageType);
	/*兼容老版本的MQ*/

private:
	static DWORD WINAPI RecvThread(LPVOID lparam);
	amqp_response_type_enum CheckLastOperation(const amqp_rpc_reply_t& rt,const char* lpOperation,string &strMsg);
	bool RecvProc(const char* lpServerIP);
	CCecilyRabbitMQConnection* InitMQConnection(const char* lpIPAddress, const char* lpVirtualHost, unsigned int nPort, const char* lpUser,const char* lpPassword);
	//bool PushMessage(CCecilyAMQPMessage* lpMessage);
	//bool PopMessage(CCecilyAMQPMessage** lpMessage);
	void CloseDeleteHandle(const char* lpServerIP);
	HANDLE GetCurrentHandle(const char* lpServerIP);
	CCecilyRabbitMQConnection* GetMQConnection(const char* lpServerIP);

	/*兼容老版本的MQ*/
	CCecilyRabbitMQConnection* GetMQConnection(long hConn);
	HANDLE GetCurrentHandle(long hConn);
	void SetCurrentHandle(long hConn, HANDLE hHandle);
	void ResetCurrentHandle(long hConn);
	static DWORD WINAPI RecvThread_MQ(LPVOID lparam);
	bool RecvProc_MQ(CCecilyRabbitMQConnection* lpConnection);
	/*兼容老版本的MQ*/

private:
	CCecilyMemoryPool*		m_lpMemoryPool;
	bool		m_bInit;
	CLocker	m_QueueLocker;
	queue<CCecilyAMQPMessage*>	m_AMQPMessageQueue;    //队列消息缓存
	CLocker m_ConnectionLocker;
	map<string,CCecilyRabbitMQConnection*>	m_mapConnection;
	vector<string>				m_vecThreadFlag;	//标记该服务器MQ是否是第一次去队列取消息
	CLocker	m_ThreadLocker;
	map<string,HANDLE>			m_mapGetThread;		//记录get线程


	CLocker	m_handlerLocker;
	HANDLE m_eventHandles[AMQP_HANDLE_EVENTS];
	static long m_sHConn;

	int		m_nQueueDepth;

	map<string,vector<AMQPExchangeQueue>>	m_mapExchangeQueue;		//保存交换器和队列信息，用于get线程监听对应的队列
	map<string,AMQPAuthority>	m_mapAuthority;     //保存登录凭证，用于重新连接

	/*兼容老版本的MQ*/
	map<long,CCecilyRabbitMQConnection*>	m_longMapConnection;
	map<long,vector<AMQPExchangeQueue>> m_longMapExchangeQueue;
	map<long,AMQPAuthority>	m_longMapAuthority;
	map<long,HANDLE>		m_longGetThread;
	vector<long>				m_longvecThreadFlag;	//标记该服务器MQ是否是第一次去队列取消息
	/*兼容老版本的MQ*/
	
};

#endif