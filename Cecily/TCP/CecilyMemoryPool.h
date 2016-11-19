/********************************************************************
	created:	  2016/09/28
	filename: 	  CecilyMemoryPool.h
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Memory_Pool_H
#define Cecily_Memory_Pool_H
#include <iostream>
#include "SafetyObject.h"
#include <vector>
#include <map>
using namespace std;

#if 0
#   include <new>
#   define __THROW_BAD_ALLOC   throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#   define __THROW_BAD_ALLOC cerr<<"out of memory" <<endl;   exit(1);
#endif

enum {  POOL_ALIGN = 4*1024   };                                    //内存分配单元最小区块大小 4KB
enum {  POOL_MAX_BYTES = 256*1024  };                               //内存池中内存区块大小的最大边界  256KB
enum {  POOL_FREE_LIST_SIZE = POOL_MAX_BYTES/POOL_ALIGN   };         //内存链表中 free_list 个数
enum {  POOL_INIT_MEMORY_SIZE   =  50 * 1024 * 1024  };			     //内存池第一次分配空间的大小   50M
enum {  POOL_RENEW_SIZE = 10 };     
enum {  POOL_MAX_BIG_MEMORY_SIZE = 1024*1024*20 };
enum {  POOL_BIG_BLOCK =1024*1024 };


//重新封装 malloc  和 realloc  增加类似 C++ 的 set new handler 机制 在内存不足的情况下的处理
class malloc_alloc_template
{
public:
	static void *allocate(size_t n)
	{
		void *lpresult = malloc(n);
		if ( NULL == lpresult )
		{
			lpresult = oom_malloc(n);
		}
		return lpresult;
	}

	static void deallocate(void *p )
	{
		free(p);
	}

	static void *reallocate(void *p, size_t /*old_size*/ ,size_t new_size)
	{
		void *lpresult = realloc(p,new_size);
		if ( NULL == lpresult )
		{
			lpresult = oom_relloc(p,new_size);
		}
		return lpresult;
	}

	static void (*set_malloc_handler(void(*f)()))()
	{
		void (* old)() = malloc_alloc_oom_handler;
		malloc_alloc_oom_handler = f;
		return old;
	}



private:
	static void *oom_malloc(size_t n)
	{
		void (*my_malloc_handler)();
		void *lpresult;
		while (true)
		{
			my_malloc_handler =  malloc_alloc_oom_handler;
			if ( NULL == my_malloc_handler )
			{
				__THROW_BAD_ALLOC;
			}
			(*my_malloc_handler)();
			lpresult = malloc(n);
			if ( NULL == lpresult )
			{
				return lpresult;
			}
		}
	}
	static void *oom_relloc(void* p,size_t n)
	{
		void (*my_malloc_handler)();
		void *lpresult;
		while (true)
		{
			my_malloc_handler = malloc_alloc_oom_handler;
			if ( NULL == my_malloc_handler )
			{
				__THROW_BAD_ALLOC;
			}
			(*my_malloc_handler)();
			lpresult = realloc(p,n);
			if ( NULL == lpresult )
			{
				return lpresult;
			}	
		}
	}
	static void (*malloc_alloc_oom_handler)();
};


class CCecilyMemoryPool
{
public:
	static CCecilyMemoryPool* GetInstance()
	{
		if ( NULL == m_instance )
		{
			m_instance =  new CCecilyMemoryPool();
		}
		return m_instance;
	}

	static void ReleaseInstance()
	{
		if ( NULL != m_instance )
		{
			delete m_instance;
			m_instance =  NULL;
		}
	}

protected:
	CCecilyMemoryPool(void);
	~CCecilyMemoryPool(void);

public:
	void * Allocate(size_t n);
	void Deallocate(void *p, size_t n);

private:
	union obj
	{
		union obj* free_list_link;
		char client_data[1];
	};

private:
	void   InitPool();
	size_t RoundUp(size_t nBytes);
	size_t GetFreeListIndex(size_t nBytes);
	char*  BlockAlloc(size_t n, int &nJobs);
	void*  Refill(size_t n);

	void*  GetBigMemory(size_t n);
	void   RecycleBigMemory(void *p, size_t n);
	void   CecilyAllBigMemory();


private:
	CSafeCriticalSection			m_csPool;
	CSafeCriticalSection			m_csBigMemory;
	obj *volatile					m_free_list[POOL_FREE_LIST_SIZE];
	char*							m_startfree;   
	char*							m_endfree;
	size_t							m_heapsize;	
	vector<char*>					m_vecMemory;
	map<DWORD,obj*>					m_mapBigMemory;
	size_t							m_nBigMemorySize;
	static CCecilyMemoryPool*		m_instance;
};

#endif
