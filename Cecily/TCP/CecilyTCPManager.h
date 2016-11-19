/********************************************************************
	created:	  2016/10/10
	filename: 	  CecilyTCPManager.h
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_TCP_Manager_H
#define Cecily_TCP_Manager_H
#include "CecilyUVTCPClient.h"
#include "CecilyUVTCPServer.h"
#include "CecilyDataSerialize.h"
#include "CecilyTCP.h"

//服务器在线人数更新
typedef void (*UpdateOnlineUsers)(int nOnlineuser);

class CecilyTCPManager
{
public:
	static CecilyTCPManager* GetInstance()
	{
		if ( NULL == m_lpTCPMgrInstance )
		{
			m_lpTCPMgrInstance = new CecilyTCPManager();
		}
		return m_lpTCPMgrInstance;
	}

	static void ReleageInstance()
	{
		if ( NULL != m_lpTCPMgrInstance )
		{
			delete m_lpTCPMgrInstance;
			m_lpTCPMgrInstance = NULL;
		}
	}

	void SetUpdateOnlineUsers(UpdateOnlineUsers lpFunc);

	//As For Server
	bool StartServer(const char* lpAddress, int Port);
	void StopServer();
	bool SendDataToClient(int nClientID, const char* lpBuffer, int nLen);
	//end
	//As For Client
	bool ConnectToServer(const char* lpAddress, int nPort);
	bool SendDataToServer(const char*lpBuffer, int nLen,bool bSerialize);
	void Disconnect();
	//end

	void SetServerRecvCallBack(RecvDataFromClient lpFunc,void* user_data);
	void SetNotifyServerStatusCallBack(NotifyServerStatus lpFunc,void* user_data);
	void SetClientRecvCallBack(RecvDataFromServer lpFunc,void* user_data);

protected:
	CecilyTCPManager(void);
	virtual ~CecilyTCPManager(void);

private:
	static void ServerRecvCallBack(int nCliend, char* buf,int nRead, void* user_data);
	static void ServerNewConnectCallBack(int clientid, void* userdata);
	static void ServerClientClose(int nClientid,void* userdata);

	static void ClientRecvCallBack(char* buf, int nSize,void *userData);
	static void ClientCloseCallBack(int clientid, void* userdata);


private:
	static CecilyTCPManager* m_lpTCPMgrInstance;
	CecilyUVTCPServer*	m_lpTCPServer;
	CecilyUVTCPClient*   m_lpTCPClient;
	CCecilyMemoryPool*	m_lpMgrPool;
	UpdateOnlineUsers	m_lpFuncUpdateOnLineUsers;

	static unsigned int m_nOnlineUser;

	//动态库接口要用的回调函数
	NotifyServerStatus	m_lpFuncNotifyServerStatus;
	void* m_lpNotifyUserData;
	RecvDataFromServer m_lpFuncRecvDataFromServer;
	void* m_lpRecvDataFromServerUserData;
	RecvDataFromClient m_lpFuncRecvDataFromClient;
	void* m_lpRecvDataFromClientUserData;

	
};



#endif