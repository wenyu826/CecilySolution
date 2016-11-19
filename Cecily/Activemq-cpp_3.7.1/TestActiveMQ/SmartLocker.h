/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/21
*  File:           SmartLocker.h
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#pragma once


class CSmartLocker;

class  CLocker
{
	friend class CSmartLocker;
private:
	CRITICAL_SECTION m_cs;
public:

	inline CLocker(void){InitializeCriticalSection(&m_cs);}
	inline ~CLocker(){DeleteCriticalSection(&m_cs);}
protected:

	inline void Lock(){	EnterCriticalSection(&m_cs);}
	inline void UnLock(){LeaveCriticalSection(&m_cs);}
};


class  CSmartLocker
{
public:

	inline CSmartLocker(CLocker *pLock,BOOL bAutoLock)
	{
		m_nRefCount = 0;
		m_pLock = pLock;
		if(bAutoLock)
			Lock();
	}

	inline ~CSmartLocker()
	{
		if(m_pLock)
		{
			while(m_nRefCount>0)
				UnLock();
		}
	}
public:
	inline void Lock()
	{
		if(m_pLock)
		{
			m_nRefCount++;
			m_pLock->Lock();
		}
	}
	inline void UnLock()
	{
		if(m_pLock)
		{
			m_nRefCount--;
			m_pLock->UnLock();
		}
	}
private:
	int				m_nRefCount;
	CLocker			*m_pLock;						
};