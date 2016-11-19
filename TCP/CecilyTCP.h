/********************************************************************
	created:	  2016/10/12
	filename: 	  CecilyTCP.h
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_TCP_H_LWY
#define Cecily_TCP_H_LWY

#ifdef CLEARTCP_EXPORTS
#define CLEAR_TCP_API __declspec(dllexport)
#else
#define CLEAR_TCP_API __declspec(dllimport)
#endif

typedef void (*RecvDataFromClient)(int nClientID,const char* buf,int nRead,void* user_data); //通知调用者，客户端发送消息的回调
typedef void (*RecvDataFromServer)(const char* buf, int nRead,void* user_data);  //通知调用者，服务端发来消息，供客户端使用
typedef void (*NotifyServerStatus)(int nCliendID,int bNotify,void* user_data);		//通知调用者客户端连入或者断开的回调通知  bNotify = 1 连入 , bNotify = 0 断开,当ID 为 -1时 表示TCPServer Stop 忽略 bNotify

//服务端
CLEAR_TCP_API bool StartServer(const char* lpAddress, int nPort);     //开启TCPServer
CLEAR_TCP_API void StopServer();				//停止TCPServer
CLEAR_TCP_API bool SendDataToClient(int nClientID,const char* lpData, int nlen);	//根据ID ，发送信息给指定客户端
CLEAR_TCP_API void SetServerRecvCallBack(RecvDataFromClient lpFunc,void* user_data);   //设置接收消息的回调
CLEAR_TCP_API void SetNotifyServerStatusCallBack(NotifyServerStatus lpFunc,void* user_data);

//客户端
CLEAR_TCP_API bool ConnectToServer(const char* lpAddress, int nPort);
CLEAR_TCP_API bool SendDataToServer(const char* lpBuffer, int nlen,bool bSerialize = true);   //bSerizlize = true 对消息组包
CLEAR_TCP_API void SetClientRecvCallBack(RecvDataFromServer lpFunc,void *user_data);
CLEAR_TCP_API void Disconnect();




#endif