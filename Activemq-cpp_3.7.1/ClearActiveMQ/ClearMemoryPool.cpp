/********************************************************************
*  Copyright ( C )
*  Date:           2016/09/28
*  File:           ClearMemoryPool.cpp
*  Contents:       
*  Author:         liwenyu
*********************************************************************/
#include "stdafx.h"
#include "ClearMemoryPool.h"

void (*malloc_alloc_template::malloc_alloc_oom_handler)() = 0;

CClearMemoryPool::CClearMemoryPool(void)
{
	InitPool();
}


CClearMemoryPool::~CClearMemoryPool(void)
{
	vector<char*>::iterator itVecMemory = m_vecMemory.begin();
	for ( itVecMemory ; itVecMemory != m_vecMemory.end(); itVecMemory++ )
	{
		malloc_alloc_template::deallocate(*itVecMemory);
	}
	ClearAllBigMemory();
}

void* CClearMemoryPool::Allocate(size_t n)
{
	if ( n <= 0 )
	{
		return NULL;
	}
	if ( n > POOL_MAX_BYTES )
	{
		void *r = NULL;
		n = RoundUp(n);
		r = GetBigMemory(n);
		if ( NULL == r )
		{
			r = malloc_alloc_template::allocate(n);
		}
		memset(r,0,n);
		return r;
	}
	CSmartSafeCriticalSection	m_Locker(m_csPool);
	int reallyBytes = RoundUp(n);
	int nIndex = GetFreeListIndex(reallyBytes);
	obj* volatile* freelistmirror;
	obj *result;
	freelistmirror = m_free_list + nIndex;
	result = *freelistmirror;
	if ( NULL == result )
	{
		void *r = Refill(reallyBytes);
		memset(r,0,reallyBytes);
		return r;
	}
	*freelistmirror = result->free_list_link;
	memset(result,0,reallyBytes);
	return result;
}

void CClearMemoryPool::Deallocate(void *p, size_t n)
{
	if ( NULL == p )
	{
		return ;
	}
	obj* q = (obj*)p;
	obj* volatile* freelistmirror ;
	if ( n > POOL_MAX_BYTES )
	{
		if ( n >= POOL_BIG_BLOCK )
		{
			malloc_alloc_template::deallocate(p);
			return;
		}
		n = RoundUp(n);
		RecycleBigMemory(p,n);
		return ;
	}
	n = RoundUp(n);
	CSmartSafeCriticalSection	m_Locker(m_csPool);
	freelistmirror = m_free_list + GetFreeListIndex(n);
	q->free_list_link = *freelistmirror;
	*freelistmirror = q;
}

void CClearMemoryPool::InitPool()
{
	m_startfree = NULL;
	m_endfree	= NULL;
	m_heapsize	= 0;
	int nNuber = POOL_FREE_LIST_SIZE;
	for ( int i = 0 ; i < POOL_FREE_LIST_SIZE; i++ )
	{
		m_free_list[i] = NULL;
	}
	m_nBigMemorySize = 0;

}

size_t CClearMemoryPool::RoundUp(size_t nBytes)
{
	if ( nBytes < 0 )
	{
		return 0;
	}
	size_t nSize = nBytes / POOL_ALIGN ;
	size_t nRemainder = nBytes % POOL_ALIGN;
	if ( nRemainder > 0 )
	{
		nSize = nSize + 1;
	}
	return ( nSize * POOL_ALIGN );
}

size_t CClearMemoryPool::GetFreeListIndex(size_t nBytes)
{
	nBytes = RoundUp(nBytes);
	return (nBytes/POOL_ALIGN-1)>=0?(nBytes/POOL_ALIGN-1):0;
}

char* CClearMemoryPool::BlockAlloc(size_t n, int &nJobs)
{
	char *result = NULL;
	static BOOL bFirstAlloc = TRUE;
	size_t reallBytes = RoundUp(n);
	size_t totalBytes = reallBytes * nJobs;
	size_t leftBytes = m_endfree - m_startfree;
	if (leftBytes >= totalBytes)
	{
		result = m_startfree;
		m_startfree += totalBytes;
		return result;
	}
	else if ( leftBytes >= reallBytes )
	{
		nJobs = leftBytes / reallBytes;
		totalBytes = nJobs * reallBytes;
		result = m_startfree;
		m_startfree = m_startfree + totalBytes;
		return result;
	}
	else
	{
		size_t bytes_to_get;
		if ( bFirstAlloc )
		{
			bFirstAlloc = FALSE;
			bytes_to_get = POOL_INIT_MEMORY_SIZE;
		}
		else
		{
			bytes_to_get = 2 * totalBytes + RoundUp(m_heapsize>>4);
		}
		if ( leftBytes > 0 )
		{
			obj * volatile* freelistmirror = m_free_list + GetFreeListIndex(leftBytes);
			((obj*)m_startfree)->free_list_link = *freelistmirror;
			*freelistmirror = (obj*)m_startfree;
		}
		m_startfree = (char*)malloc(bytes_to_get);
		if ( NULL == m_startfree )
		{
			int i = 0;
			obj* volatile * freelistmirror , *p;
			for ( i = reallBytes; i <= POOL_MAX_BYTES; i= i+ POOL_ALIGN)
			{
				freelistmirror = m_free_list + GetFreeListIndex(i);
				p = *freelistmirror;
				if ( NULL != p )
				{
					*freelistmirror = p->free_list_link;
					m_startfree = (char*)p;
					m_endfree = m_startfree + i;
					return BlockAlloc(i,nJobs);
				}
			}
			m_endfree = NULL;
			m_startfree = (char*)malloc_alloc_template::allocate(bytes_to_get);
			return NULL;
		}
		m_vecMemory.push_back(m_startfree);
		m_heapsize += bytes_to_get;
		m_endfree = m_startfree + bytes_to_get;
		return BlockAlloc(n,nJobs);
	}
}


void* CClearMemoryPool::Refill(size_t n)
{
	int nJobs = POOL_RENEW_SIZE;
	size_t reallyBytes = RoundUp(n);
	char* pBlock = BlockAlloc(reallyBytes,nJobs);
	if ( NULL == pBlock )
	{
		return NULL;
	}
	obj* volatile * freelistmirror;
	obj* result;
	obj* currentObj, *nextObj;
	int i = 0;
	if ( 1 == nJobs )
	{
		return pBlock;
	}
	freelistmirror = m_free_list + GetFreeListIndex(reallyBytes);
	result = (obj*)pBlock;
	*freelistmirror = nextObj = (obj*)(pBlock+reallyBytes);
	for ( i = 1; ; i++ )
	{
		currentObj = nextObj;
		nextObj = (obj*)((char*)nextObj + reallyBytes);
		if ( nJobs - i == 1 )
		{
			currentObj->free_list_link = NULL;
			break;
		}else
		{
			currentObj->free_list_link = nextObj;
		}
	}
	return result;
}



void* CClearMemoryPool::GetBigMemory(size_t n)
{
	void* result = NULL;
	CSmartSafeCriticalSection	m_locker(m_csBigMemory);
	map<DWORD,obj*>::iterator itMap = m_mapBigMemory.find(n);
	int nSize = 0 ;
	if ( itMap != m_mapBigMemory.end() )
	{
		obj* p = (obj*)itMap->second;
		if ( NULL != p )
		{
			result = (void*)itMap->second;
		}
		if ( NULL != p->free_list_link )
		{
			itMap->second = p->free_list_link;
		}
		else
		{
			m_mapBigMemory.erase(itMap);
		}
		if ( NULL != result )
		{
			memset(result,0,n);
			m_nBigMemorySize = m_nBigMemorySize - n;
			return result;
		}
	}
	return result;
}


void CClearMemoryPool::RecycleBigMemory(void *p, size_t n)
{
	if ( NULL == p || n <= 0 )
	{
		return ;
	}
	CSmartSafeCriticalSection		m_Locker(m_csBigMemory);
	if ( m_nBigMemorySize >= POOL_MAX_BIG_MEMORY_SIZE )
	{
		malloc_alloc_template::deallocate(p);
		ClearAllBigMemory();
		return;
	}
	obj* pObj = (obj*)p;
	pObj->free_list_link = NULL;
	map<DWORD,obj*>::iterator itMap = m_mapBigMemory.find(n);
	if ( itMap != m_mapBigMemory.end() )
	{
		pObj->free_list_link = (obj*)itMap->second;
		itMap->second = pObj;
	}
	else
	{
		m_mapBigMemory[n] = pObj;
	}
	m_nBigMemorySize += n;
}

void CClearMemoryPool::ClearAllBigMemory()
{
	map<DWORD,obj*>::iterator itMap = m_mapBigMemory.begin();
	for ( itMap ; itMap != m_mapBigMemory.end(); itMap++ )
	{
		obj* pObj = (obj*)itMap->second;
		while (NULL != pObj->free_list_link )
		{
			obj* pCurrentObj = pObj;
			pObj = pCurrentObj->free_list_link;
			if ( NULL != pCurrentObj )
			{
				malloc_alloc_template::deallocate((void*)pCurrentObj);
			}
		}
		malloc_alloc_template::deallocate((void*)pObj);
		itMap->second = NULL;
	}
	m_mapBigMemory.clear();
}

