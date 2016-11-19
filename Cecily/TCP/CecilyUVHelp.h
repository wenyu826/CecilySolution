/********************************************************************
	created:	  2016/10/09
	filename: 	  CecilyUVHelp.h
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_UV_Help_h
#define Cecily_UV_Help_h
#include "uv.h"
#include "CecilyMemoryPool.h"
#include <string>
#pragma comment(lib,"libuv1.8.0.lib")
using namespace std;

#define MAX_TCP_BUFFER_SIZE			1024*32
extern long g_ClientID;


namespace clear
{
	#if defined (WIN32) || defined(_WIN32)
		#define uv_thread_close(t) (CloseHandle(t)!=FALSE)
		#define uv_thread_sleep(ms) Sleep(ms)
		#define uv_thread_id GetCurrentThreadId
	#endif

enum DataType
{
	D_XML,
	D_TEXT,
	D_IMAGE,
	D_BINARY
};



#pragma pack(1)
	const unsigned char header = 'a';
	const unsigned char tail = 'b';
	const int g_version = 0x01;
	typedef struct _MessageHeader
	{
		int version;				//版本
		int type;					//数据类型
		int packagelength;			//包长度
		int reserve;				//保留字段
		_MessageHeader()
		{
			version = g_version;
			type = D_TEXT;
		}
	}MessageHeader;
#define Message_Header_Size sizeof(MessageHeader)
#pragma pack()

	
	typedef void (*TCPClientRecvCallBack)(char* buf, int nRead,void* userData);		//客户端收到消息的回调函数 userData为 CecilyUVTCPClient 对象
	typedef void (*TCPCloseCallBack)(int nClientID, void* user_data);				//客户端主动关闭连接的回调函数  userData为 CecilyUVTCPClient 对象
	typedef void (*TCPNewConnectCallBack)(int nClientID, void * user_data);			//TCPServer 收到新的客户端连接的回调
	typedef void (*TCPServerRecvCallBack)(int nClientID, char* buf, int nRead, void* user_data);	//TCPServer收到客户端发送的消息的回调函数
	typedef void(*TCPServerStopCallBack)();       //TCPServer服务停止通知

	inline std::string GetUVErrorMsg(int nErrorCode)
	{
		if ( 0 == nErrorCode )
		{
			return "";
		}
		std::string strError;
		const char* lpbuf = uv_err_name(nErrorCode);
		if ( lpbuf )
		{
			strError = lpbuf;
			strError += ":";
		}else
		{
			strError = "unknow system error code :";
		}
		lpbuf = uv_strerror(nErrorCode);
		if ( lpbuf )
		{
			strError += lpbuf;
		}
		return strError;
	}

	enum UVTCPStatus
	{
		S_FINISH,
		S_TIME_OUT,
		S_ERROR,
		S_DISCONNECT,
	};

	

	//发送数据的缓冲区结构
	typedef struct tagUVTCPWriteParam
	{
		uv_write_t  write_req;
		uv_buf_t write_buf;
		CCecilyMemoryPool* lpMemoryPool;
		tagUVTCPWriteParam(CCecilyMemoryPool* lpPool = NULL )
		{
			lpMemoryPool = lpPool;
			write_buf.base = NULL;
			write_buf.len = 0;
		}
		~tagUVTCPWriteParam()
		{
			if ( NULL != lpMemoryPool )
			{
				lpMemoryPool->Deallocate((void*)write_buf.base,write_buf.len);
			}
			else
			{
				delete []write_buf.base;
				write_buf.base = NULL;
				write_buf.len = 0 ;
			}
		}
		bool WriteBuffer(const char* lpBuffer,unsigned int nSize)
		{
			if ( NULL != lpMemoryPool )
			{
				write_buf.base = (char*)lpMemoryPool->Allocate(nSize);
				write_buf.len = nSize;
			}
			else
			{
				write_buf.base = new char[nSize];
				write_buf.len = nSize;
			}
			if ( NULL == write_buf.base )
			{
				return false;
			}
			errno_t ret = memcpy_s(write_buf.base,write_buf.len,lpBuffer,nSize);
			if ( ret == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
			
		}
	}UVTCPWriteParam;


	//用于客户端的TCP Client的上下文描述
	typedef struct tagUVTCPClientCtx
	{
		uv_tcp_t tcpHandle;
		uv_write_t write_req;
		int clientID;
		void* parent;			//用于TCPClient时 指向 CecilyUVTCPClient, 用于TCPServer时 指向 客户端Connect
		void* server;			//用于TCPServer
		tagUVTCPClientCtx()
		{
			this->clientID = -1;
			this->parent = NULL;
			this->server = NULL;
		}
		~tagUVTCPClientCtx()
		{
			this->clientID = -1;
			this->parent = NULL;
			this->server = NULL;
		}

	}UVTCPClientCtx;
	UVTCPClientCtx* AllocateUVTCPClientCtx(void *parent,int nRole = 0);// nRole == 0 use for TCPClient  nRole != 0 user for TCPServer
	void FreeUVTCPClientCtx(UVTCPClientCtx* lpCtx);

	inline UVTCPClientCtx* AllocateUVTCPClientCtx(void *parent,int nRole )
	{
		UVTCPClientCtx* lpCtx = (UVTCPClientCtx*)malloc(sizeof(*lpCtx));
		//UVTCPClientCtx* lpCtx = NULL;
		//lpCtx = new UVTCPClientCtx(MAX_TCP_BUFFER_SIZE);
		if ( NULL == lpCtx )
		{
			return lpCtx;
		}
		lpCtx->write_req.data = lpCtx;
		if ( !nRole )
		{
			lpCtx->parent = parent;
			lpCtx->server = NULL;
		}else
		{
			lpCtx->server = parent;
			lpCtx->parent = NULL;
		}
		
		lpCtx->clientID = InterlockedIncrement(&g_ClientID);
		return lpCtx;
	}
	inline void FreeUVTCPClientCtx(UVTCPClientCtx* lpCtx)
	{
		free(lpCtx);
// 		if ( NULL == lpCtx )
// 		{
// 			return ;
// 		}
// 		delete lpCtx;
// 		lpCtx = NULL;
	}

	class CUVMutex
	{
	public:
		explicit CUVMutex()
		{
			uv_mutex_init(&m_mutex);
		}
		~CUVMutex()
		{
			uv_mutex_destroy(&m_mutex);
		}
		void Lock()
		{
			uv_mutex_lock(&m_mutex);
		}
		void UnLock()
		{
			uv_mutex_unlock(&m_mutex);
		}
		bool TryLock()
		{
			return uv_mutex_trylock(&m_mutex) == 0 ;
		}
	private:
		uv_mutex_t m_mutex;
		friend class CUVCond;
		friend class CUVAutoLock;
		CUVMutex(const CUVMutex&);
		CUVMutex& operator=(const CUVMutex&);
	};

	class CUVAutoLock
	{
	public:
		explicit CUVAutoLock(uv_mutex_t* lpParma):m_lpMutex(lpParma)
		{
			uv_mutex_lock(m_lpMutex);
		}
		explicit CUVAutoLock(CUVMutex* lpUVMutex):m_lpMutex(&lpUVMutex->m_mutex)
		{
			uv_mutex_lock(m_lpMutex);
		}
		~CUVAutoLock()
		{
			uv_mutex_unlock(m_lpMutex);
		}
		
	private:
		uv_mutex_t* m_lpMutex;
		CUVAutoLock(const CUVAutoLock&);
		CUVAutoLock& operator = (const CUVAutoLock&);
	};

	class CUVCond
	{
	public:
		explicit CUVCond()
		{
			uv_cond_init(&m_cond);
		}
		~CUVCond()
		{
			uv_cond_destroy(&m_cond);
		}
		void Signal()
		{
			uv_cond_signal(&m_cond);
		}
		void BroadCast()
		{
			uv_cond_broadcast(&m_cond);
		}
		void Wait(CUVMutex* lpMutex)
		{
			if ( NULL == lpMutex )
			{
				return;
			}
			uv_cond_wait(&m_cond,&lpMutex->m_mutex);
		}
		int Wait(CUVMutex* lpMutex, uint64_t timeout)
		{
			return uv_cond_timedwait(&m_cond,&lpMutex->m_mutex,timeout);
		}
		int Wait(uv_mutex_t* lpmutex, uint64_t timeout)
		{
			return uv_cond_timedwait(&m_cond,lpmutex,timeout);
		}

	private:
		uv_cond_t m_cond;
		CUVCond(const CUVCond&);
		CUVCond& operator=(const CUVCond&);
	};

	class CUVSem
	{
	public:
		explicit CUVSem(int initValue = 0)
		{
			uv_sem_init(&sem_, initValue);
		}
		~CUVSem(void)
		{
			uv_sem_destroy(&sem_);
		}
		void Post()
		{
			uv_sem_post(&sem_);
		}
		void Wait()
		{
			uv_sem_wait(&sem_);
		}
		bool TryWait()
		{
			return uv_sem_trywait(&sem_) == 0;
		}
	private:
		uv_sem_t sem_;
	private:
		CUVSem(const CUVSem&);
		CUVSem& operator =(const CUVSem&);
	};

	class CUVRWLock
	{
	public:
		explicit CUVRWLock()
		{
			uv_rwlock_init(&rwlock_);
		}
		~CUVRWLock(void)
		{
			uv_rwlock_destroy(&rwlock_);
		}
		void ReadLock()
		{
			uv_rwlock_rdlock(&rwlock_);
		}
		void ReadUnLock()
		{
			uv_rwlock_rdunlock(&rwlock_);
		}
		bool ReadTryLock()
		{
			return uv_rwlock_tryrdlock(&rwlock_) == 0;
		}
		void WriteLock()
		{
			uv_rwlock_wrlock(&rwlock_);
		}
		void WriteUnLock()
		{
			uv_rwlock_wrunlock(&rwlock_);
		}
		bool WriteTryLock()
		{
			return uv_rwlock_trywrlock(&rwlock_) == 0;
		}
	private:
		uv_rwlock_t rwlock_;
	private:
		CUVRWLock(const CUVRWLock&);
		CUVRWLock& operator =(const CUVRWLock&);
	};

	class CUVBarrier
	{
	public:
		explicit CUVBarrier(int count)
		{
			uv_barrier_init(&barrier_, count);
		}
		~CUVBarrier(void)
		{
			uv_barrier_destroy(&barrier_);
		}
		void Wait()
		{
			uv_barrier_wait(&barrier_);
		}
	private:
		uv_barrier_t barrier_;
	private:
		CUVBarrier(const CUVBarrier&);
		CUVBarrier& operator =(const CUVBarrier&);
	};

	typedef void (*UVThreadEntry)(void* arg);
	class CUVThread
	{
	public:
		explicit CUVThread(UVThreadEntry lpfunc,void* arg):m_lpEntry(lpfunc),
			m_arg(arg),
			m_bIsRunning(false)
		{

		}
		~CUVThread()
		{
			if ( m_bIsRunning )
			{
				uv_thread_join(&m_thread);
			}
			m_bIsRunning = false;
		}

		void Start()
		{
			if ( m_bIsRunning )
			{
				return ;
			}
			uv_thread_create(&m_thread,m_lpEntry,m_arg);
			m_bIsRunning = true;
		}

		void Stop()
		{
			if ( !m_bIsRunning )
			{
				return ;
			}
			uv_thread_join(&m_thread);
			m_bIsRunning = false;
		}
		void Sleep(uint64_t millsec)
		{
			uv_thread_sleep(millsec);
		}
		int GetThreadID()
		{
			return uv_thread_id();
		}
		bool IsRunning()
		{
			return m_bIsRunning;
		}

	private:
		UVThreadEntry		m_lpEntry;
		uv_thread_t		m_thread;
		void* m_arg;
		bool m_bIsRunning;
	};

}

#endif