/********************************************************************
	created:	  2016/10/10
	filename: 	  CecilyNetClient.h
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  服务端接收到的客户端连接对象
*********************************************************************/
#ifndef Cecily_Net_Client_H
#define Cecily_Net_Client_H
#include "CecilyUVHelp.h"
#include "CecilyMemoryPool.h"
#include "CecilyDataSerialize.h"
#include <deque>
#include <queue>
using namespace std;
using namespace clear;
typedef struct _NetPackage
{
	uv_work_t req;
	uv_buf_t buf;
	int nRead;
	int nClientID;
}NetPackage;

class CecilyNetClient
{
public:
	CecilyNetClient(UVTCPClientCtx* lpCtx,int nClientID, uv_loop_t* loop);
	virtual ~CecilyNetClient(void);



public:
	void SetCloseCallBack(TCPCloseCallBack lpFunc, void* user_data);
	void SetServerRecvCallBack(TCPServerRecvCallBack lpFunc, void* user_data);
	UVTCPClientCtx* GetUVTCPClientCtx() ;
	const char* GetLastErrorMsg()
	{
		return m_strErrorMsg.c_str();
	}
	void Close();
	bool SendData(const char* lpBuffer, unsigned int nLen);
	static void SetMemoryPool(CCecilyMemoryPool* lpMemoryPool);
	int BeginRecv();
	void DeSerialize(int nClientID, char* lpBuffer, int nRead);
	PackageStatus GetPackageStatus();


// 	void AddNotFreeMemory(uv_buf_t* buf);
// 	void FreeMemory(uv_buf_t* buf);
	void AddNotFreeMemory(NetPackage* lpPackage);
	void FreeMemory(NetPackage* lpPackage);
	void AddRecvPackage(NetPackage* lpPackage);

	//接收线程控制
	void PostThreadRecvSem()
	{
		m_threadRecvSem.Post();
	}

	void WaitForThreadRecvSem()
	{
		m_threadRecvSem.Wait();
	}
	//
	//发送线程控制

	void PostThreadSendSem()
	{
		m_threadSendSem.Post();
	}

	void WaitForThreadSendSem()
	{
		m_threadSendSem.Wait();
	}
	//

	//线程状态控制
	void SetThreadStatus();
	void ResetThreadStatus();
	bool GetThreadStatus();
	//


private:
	bool Init();
	void MakeSureSend(uv_write_t* write_req = NULL);
	void MakeSureDeSerialize();
	static void AfterClientClose(uv_handle_t* handle);
	static void AllocateBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void AfterRecv(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
	static void AfterSend(uv_write_t* req, int status);
	static void AsyncCallBack(uv_async_t* handle);
	static void AsyncThreadCallBack(uv_async_t* handle);
	void ReSend(UVTCPWriteParam* lpParam);

	static void ParsePackage(uv_work_t *req);
	static void AfterParsePackage(uv_work_t* req, int status);

	static void SendPackage(uv_work_t* req);
	static void AfterSendPackage(uv_work_t* req, int status);

	static void AfterRecvFullPackage(int nClientID, char* lpBuffer, int nlen,void* user_data);

public:
	TCPServerRecvCallBack	m_lpFuncTCPServerRecv;
	void*	m_lpTCPServerRecvUserData;
	

private:
	uv_loop_t*	m_NetClient_loop;
	unsigned int	m_nClientID;
	UVTCPClientCtx*		m_lpTcpClientCtx;
	bool	m_bIsClosed;
	

	TCPCloseCallBack		m_lpFuncTCPClose;
	void*	m_lpTCPCloseUserData;


	std::string	m_strErrorMsg;
	static CCecilyMemoryPool*	m_lpNetClientMemoryPool;

	CUVMutex	m_writeMutex;
	deque<UVTCPWriteParam*>		m_queueWriteParam;

	
	CUVMutex	m_recvpackageMutex;
	queue<NetPackage*>	m_queueRecvPackage;
	//接收数据解析线程控制变量
	uv_async_t	m_async_thread;
	CUVSem	m_threadRecvSem;

	bool m_bThreadRunning;			//是否有解析线程在运行
	CUVMutex m_ThreadStatusMutex;   //线程互斥量
	NetPackage* m_lastRecvPackage;

	//end

	//发送数据线程控制变量
	uv_async_t	m_async_send;
	CUVSem	m_threadSendSem;
	//end

	

	//CecilyDataSerialize*	m_lpSerialize;
	CecilyDataSerialize m_Serialize;
	CUVMutex	m_SerializeMutex;

	RecvFullPackageCallBack		m_lpFuncFullPackage;

	//vector<uv_buf_t*>		m_vecBuffers;
	vector<NetPackage*>		m_vecBuffers;
	CUVMutex	m_BufferMutex;

	//CUVMutex	m_ThreadMutex;
	


};



#endif