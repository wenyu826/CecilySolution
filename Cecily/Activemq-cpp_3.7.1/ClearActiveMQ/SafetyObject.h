#ifndef Safe_Object_H
#define Safe_Object_H

class CSafeCriticalSection
{
public:
	CSafeCriticalSection()
	{
		::InitializeCriticalSection(&m_sec);
	}
	~CSafeCriticalSection()
	{
		::DeleteCriticalSection(&m_sec);
	}

	void Lock()
	{
		::EnterCriticalSection(&m_sec);
	}
	void UnLock()
	{
		::LeaveCriticalSection(&m_sec);
	}
public:
	CSafeCriticalSection(const CSafeCriticalSection& cs);
	CSafeCriticalSection operator =(const CSafeCriticalSection& cs);
	CRITICAL_SECTION		m_sec;
};

class CSafeCriticalForHeapAlloc
{
public:
	CSafeCriticalForHeapAlloc(BOOL bInit = TRUE)
	{
		m_pSec = NULL;
		m_bInit = bInit;
		if ( m_bInit )
		{
			m_pSec = new CRITICAL_SECTION;
			::InitializeCriticalSection(m_pSec);
		}
	}
	~CSafeCriticalForHeapAlloc()
	{
		Reset();
	}

	void Attach(CRITICAL_SECTION *pSec)
	{
		Reset();
		m_pSec = pSec;
	}

	CRITICAL_SECTION* Detach()
	{
		CRITICAL_SECTION* pSec = m_pSec;
		m_pSec = NULL;
		return pSec;
	}

	void Lock()
	{
		::EnterCriticalSection(m_pSec);
	}
	void UnLock()
	{
		::LeaveCriticalSection(m_pSec);
	}

private:
	void Reset()
	{
		if ( m_bInit )
		{
			if ( NULL != m_pSec )
			{
				::DeleteCriticalSection(m_pSec);
				delete m_pSec;
				m_pSec = NULL;
			}
			m_bInit = FALSE;
		}
	}

private:
	CSafeCriticalForHeapAlloc(const CSafeCriticalForHeapAlloc& cs);
	CSafeCriticalForHeapAlloc operator = (const CSafeCriticalForHeapAlloc& cs);
	CRITICAL_SECTION		*m_pSec;
	BOOL					m_bInit;
};

class CSafeMutex
{
public:
	CSafeMutex(BOOL bInitialOwner = FALSE, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES pSecurity = NULL)	
	{
		m_hMutex = ::CreateMutex(pSecurity, bInitialOwner, pszName);
	}
	~CSafeMutex()
	{
		if(IsValid())
			::CloseHandle(m_hMutex);
	}

	BOOL IsValid()	{return m_hMutex != NULL;}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
	{
		if ( IsValid() )
		{
			::CloseHandle(m_hMutex);
		}
		m_hMutex = ::OpenMutex(dwAccess,bInheritHandle,pszName);
		return IsValid();
	}

	void Lock(DWORD dwMillIseconds = INFINITE)
	{
		::WaitForSingleObject(m_hMutex, dwMillIseconds);
	}

	void UnLock()
	{
		::ReleaseMutex(m_hMutex);
	}

	HANDLE& GetHandle() 	
	{
		return m_hMutex;
	}

	operator HANDLE ()	
	{
		return m_hMutex;
	}

	HANDLE* operator &()	
	{
		return &m_hMutex;
	}

private:
	CSafeMutex(const CSafeMutex& obj);
	CSafeMutex operator = (const CSafeMutex& obj);
	HANDLE			m_hMutex;
};

template<class T>
class CSmartLock
{
public:
	CSmartLock(T &obj):m_lock(obj)
	{
		m_lock.Lock();
	}
	~CSmartLock()
	{
		m_lock.UnLock();
	}

private:
	T&			m_lock;
};

typedef CSmartLock<CSafeCriticalSection>	CSmartSafeCriticalSection;
typedef CSmartLock<CSafeMutex>				CSmartSafeMutex;

class CSafeEvent
{
public:
	CSafeEvent(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR pszName = NULL, LPSECURITY_ATTRIBUTES pSecurity = NULL)
	{
		m_hEvent = ::CreateEvent(pSecurity, bManualReset, bInitialState, pszName);
	}
	~CSafeEvent()
	{
		if(IsValid())
		{
			::CloseHandle(m_hEvent);
		}
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
	{
		if(IsValid())
		{
			::CloseHandle(m_hEvent);
		}
		m_hEvent = ::OpenEvent(dwAccess, bInheritHandle, pszName);
		return(IsValid());
	}

	BOOL Pulse(){ return(::PulseEvent(m_hEvent)); }
	BOOL Reset(){ return(::ResetEvent(m_hEvent)); }
	BOOL Set(){ return(::SetEvent(m_hEvent)); }
	HANDLE& GetHandle(){ return m_hEvent; }
	operator HANDLE(){ return m_hEvent;}
	HANDLE* operator &(){return &m_hEvent;}
	BOOL IsValid(){return m_hEvent != NULL;}

private:
	CSafeEvent(const CSafeEvent&);
	CSafeEvent operator =(const CSafeEvent&);
	HANDLE			m_hEvent;
};


class CSafeSemaphore
{
public:
	CSafeSemaphore(LONG lMaximumCount, LONG lInitialCount = 0, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES pSecurity = NULL)
	{
		m_hsem = ::CreateSemaphore(pSecurity, lInitialCount, lMaximumCount, lpName);
	}

	~CSafeSemaphore()
	{
		if(IsValid())
		{
			::CloseHandle(m_hsem);
		}
	}

	BOOL Open(DWORD dwAccess, BOOL bInheritHandle, LPCTSTR pszName)
	{
		if(IsValid())
		{
			::CloseHandle(m_hsem);
		}

		m_hsem = ::OpenSemaphore(dwAccess, bInheritHandle, pszName);
		return(IsValid());
	}

	void Wait(DWORD dwMilliseconds = INFINITE)
	{
		::WaitForSingleObject(m_hsem, dwMilliseconds);
	}

	BOOL Release(LONG lReleaseCount = 1, LPLONG lpPreviousCount = NULL)
	{
		return ::ReleaseSemaphore(m_hsem, lReleaseCount, lpPreviousCount);
	}

	HANDLE& GetHandle	() 	{return m_hsem;}
	operator HANDLE		()	{return m_hsem;}
	HANDLE* operator &	()	{return &m_hsem;}
	BOOL IsValid		()	{return m_hsem != NULL;}

private:
	CSafeSemaphore(const CSafeSemaphore& sem);
	CSafeSemaphore operator = (const CSafeSemaphore& sem);
private:
	HANDLE m_hsem;
};


#endif