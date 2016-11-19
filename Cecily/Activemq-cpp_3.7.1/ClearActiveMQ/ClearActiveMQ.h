/********************************************************************
	created:	  2016/10/24
	filename: 	  ClearActiveMQ.h
	author:		  ������
	E-Mail��	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Clear_Active_MQ_H
#define Clear_Active_MQ_H
#include <string>
using namespace std;


#ifdef CLEARACTIVEMQ_EXPORTS
#define ACTIVEME_API  __declspec(dllexport)
#else
#define ACTIVEME_API  __declspec(dllimport)
#endif

enum MessageType
{
	M_Text = 0,
	M_Map,
	M_Bytes
};

typedef void(*clear_mq_receive)(const char* lpQueueName, void* lpBuffer,int nLen, int nType);

ACTIVEME_API bool  ConnectToMQ(const char* lpAddress, int nPort, const char* username, const char* password);
ACTIVEME_API bool  OpenMQ(const char* queue, int nType =1 );    //nType : 0  �������� topic   nType :1 ����   �˽ӿ�ֻ�����򿪽��ն���
ACTIVEME_API bool  DeleteMQ(const char* queue, bool bPubSub);
//ACTIVEME_API bool  GetMessage(char** lpQueueName, char** lpMsg );
ACTIVEME_API void  SetReceiveCallBack(clear_mq_receive lpFunc);
ACTIVEME_API bool PutMessage(const char* queue, void*lpBuffer, int nLen, int nMessageType, int nType);

#endif
