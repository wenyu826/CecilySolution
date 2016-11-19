/********************************************************************
	created:	  2016/10/12
	filename: 	  CecilyTCP.h
	author:		  ������
	E-Mail��	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_TCP_H_LWY
#define Cecily_TCP_H_LWY

#ifdef CLEARTCP_EXPORTS
#define CLEAR_TCP_API __declspec(dllexport)
#else
#define CLEAR_TCP_API __declspec(dllimport)
#endif

typedef void (*RecvDataFromClient)(int nClientID,const char* buf,int nRead,void* user_data); //֪ͨ�����ߣ��ͻ��˷�����Ϣ�Ļص�
typedef void (*RecvDataFromServer)(const char* buf, int nRead,void* user_data);  //֪ͨ�����ߣ�����˷�����Ϣ�����ͻ���ʹ��
typedef void (*NotifyServerStatus)(int nCliendID,int bNotify,void* user_data);		//֪ͨ�����߿ͻ���������߶Ͽ��Ļص�֪ͨ  bNotify = 1 ���� , bNotify = 0 �Ͽ�,��ID Ϊ -1ʱ ��ʾTCPServer Stop ���� bNotify

//�����
CLEAR_TCP_API bool StartServer(const char* lpAddress, int nPort);     //����TCPServer
CLEAR_TCP_API void StopServer();				//ֹͣTCPServer
CLEAR_TCP_API bool SendDataToClient(int nClientID,const char* lpData, int nlen);	//����ID ��������Ϣ��ָ���ͻ���
CLEAR_TCP_API void SetServerRecvCallBack(RecvDataFromClient lpFunc,void* user_data);   //���ý�����Ϣ�Ļص�
CLEAR_TCP_API void SetNotifyServerStatusCallBack(NotifyServerStatus lpFunc,void* user_data);

//�ͻ���
CLEAR_TCP_API bool ConnectToServer(const char* lpAddress, int nPort);
CLEAR_TCP_API bool SendDataToServer(const char* lpBuffer, int nlen,bool bSerialize = true);   //bSerizlize = true ����Ϣ���
CLEAR_TCP_API void SetClientRecvCallBack(RecvDataFromServer lpFunc,void *user_data);
CLEAR_TCP_API void Disconnect();




#endif