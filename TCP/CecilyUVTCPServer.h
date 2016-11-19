/********************************************************************
	created:	  2016/10/10
	filename: 	  CecilyUVTCPServer.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_UVTCP_Server_H
#define Cecily_UVTCP_Server_H
#include "CecilyNetClient.h"
#include <map>
using namespace std;
using namespace clear;

class CecilyUVTCPServer
{
public:
	CecilyUVTCPServer(CCecilyMemoryPool* lpMemoryPool = NULL);
	~CecilyUVTCPServer(void);

public:
	static void SetMemoryPool(CCecilyMemoryPool* lpMemoryPool);
	bool Start(const char* lpIPAddress, int nPort);
	void Close();
	void SetRecvCallBack(int nClientID, TCPServerRecvCallBack lpFunc,void* userdata);
	void SetCloseCallBack(TCPCloseCallBack lpFunc, void* userdata);
	void SetNewConnectCallBack(TCPNewConnectCallBack lpFunc, void* userdata);
	bool SetNoDelay(bool bDelay);
	bool SetKeepAlive(bool bEnable,unsigned int nDelay);
	bool SendData(int nClientID, const char* lpbuffer, int nLen);

	void AddClient(int nClientID, CecilyNetClient* lpNetClient);
	void RemoveClient(int nClientID);

	bool IsClosed()
	{
		return m_bIsClose;
	}
	const char* GetLastErrorMsg()
	{
		return m_strErrorMsg.c_str();
	}


private:
	
	bool Init();
	bool Run(int status = UV_RUN_DEFAULT);
	bool Bind(const char* lpIpAddress, int nPort);
	bool Listen(int backlog = SOMAXCONN);

	void MakeSureClose();

	static void StartThread(void* arg);

	static void CloseWalkCallBack(uv_handle_t* handle, void* arg);
	static void AfterServerClose(uv_handle_t* handle);
	static void AsyncCallBack(uv_async_t* handle);
	static void AcceptConnection(uv_stream_t* server, int status);
	static void AllocBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void AfterRecv(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	static void AcceptCloseClient(uv_handle_t* handle);
	static void NetClientClosed(int clientid, void* userdata); 

public:
	map<int,CecilyNetClient*>	m_mapClient;
	CUVMutex	m_clientMutex;

private:
	uv_loop_t	m_serverLoop;
	uv_tcp_t	m_tcpHandle;
	uv_async_t	m_async_handle_close;
	bool		m_bIsClose;
	bool		m_bIsForceClose;
	UVTCPStatus	m_serverStatus;
	std::string	m_strErrorMsg;

	uv_thread_t start_handle;

	

	TCPNewConnectCallBack		m_lpFuncTCPNewConnect;
	void*	m_lpTCPNewConnectUserData;

	TCPCloseCallBack			m_lpFuncTCPClose;
	void*	m_lpTCPCloseUserData;


	std::string m_strServerIP;
	unsigned int m_nPort;

	static CCecilyMemoryPool* m_lpServerMemoryPool;


};

#endif