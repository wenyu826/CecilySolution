/********************************************************************
	created:	  2016/10/09
	filename: 	  CecilyUVTCPClient.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_UVTCP_Client_H
#define Cecily_UVTCP_Client_H

#include "CecilyUVHelp.h"
#include "CecilyDataSerialize.h"
#include <deque>
using namespace std;
using namespace clear;



class CecilyUVTCPClient
{
public:
	CecilyUVTCPClient();
	virtual ~CecilyUVTCPClient(void);

public:
	static void SetMemoryPool(CCecilyMemoryPool* lpMemoryPool);
	void Close();
	bool Run(int status = UV_RUN_DEFAULT);
	bool SetNoDelay(bool bDelay);
	bool SetKeepAlive(bool bKeepAlive, unsigned int nDelay );
	void SetRecvCallBack(TCPClientRecvCallBack lpfunc, void* user_data);
	void SetCloseCallBack(TCPCloseCallBack lpfunc, void* user_data);
	bool Connect(const char* lpIPAddress, unsigned int nPort);
	int Send(const char*lpData, unsigned int len,bool bSerialize);
	const char*GetLastErrorMsg()
	{
		return m_strErrorMsg.c_str();
	}
	bool IsClosed()
	{
		return m_bIsClosed;
	}

private:
	bool Init();
	bool EnableReconnect();
	void DisableReconnect();
	void MakeSureClose();
	void MakeSureSend(uv_write_t* req = NULL);
	void ReSend(UVTCPWriteParam* lpParam);

	void DeSerialize(char* lpBuffer, int nLen);

	static void AsyncCallBack(uv_async_t* handle);
	static void CloseWalkCallBack(uv_handle_t* handle, void* arg);
	static void AfterClientClose(uv_handle_t* handle);
	static void ReconnectTimer(uv_timer_t* handle);
	static void AfterConnect(uv_connect_t* req, int status);
	static void AfterRecv(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	static void AfterSend(uv_write_t* req, int status);
	static void ConnectThread(void* arg);
	static void AllocateBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);

	static void RecvFullPackageCallBack(int nClientID,char* lpBuffer,int nLen,void* user_data);

private:
	clear::UVTCPClientCtx* m_tcpClientCtx;
	uv_async_t m_async_handle;
	uv_loop_t m_loop;
	bool m_bIsClosed;
	bool m_bIsForceClosed;
	uv_thread_t m_connect_thread_handle;
	uv_connect_t m_connect_req;
	clear::UVTCPStatus	m_status;
	std::string m_strErrorMsg;

	uv_mutex_t	m_mutex_writebuf;;

	TCPClientRecvCallBack	m_recvCallBack;
	void*	m_recv_user_data;

	TCPCloseCallBack	m_tcpCloseCallBack;
	void* m_tcp_close_user_data;

	uv_timer_t m_reconnect_timer;
	bool m_bEnableReconnect;
	int64_t m_reconnect_interval;

	std::string		m_strConnect;
	unsigned int	m_nPort;
	bool			m_bIsIPV6;

	static CCecilyMemoryPool*		m_lpMemoryPool;
	deque<UVTCPWriteParam*>		m_queueWriteParam;

	CecilyDataSerialize* m_lpSerialize;

};


#endif