#include "stdafx.h"
#include "Log.h"
#include <algorithm>
#pragma warning(disable :4996)




CLog* CLog::m_pInstance = NULL ;
int CLog::m_nConfigLogLevel =  LOGLEVEL_DEBUG ;

long CLog::GetCurSec()
{
#ifdef WIN32
	long lsec = GetTickCount() ;
#else
	long lsec = 0;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	lsec = (tv.tv_sec*1000)+(tv.tv_usec/1000);
#endif

	return lsec;
}

bool CLog::WriteLog(int nFileLen, int nWriteLogLevel,const char *str, ...) 
{
	if( nWriteLogLevel < m_nWriteLogLevel ) {return true;}
	//CLog *pLog = CLog::GetInstance() ;
	bool bRet = true ;

	try
	{
		char szBuffer[40960] ;
		string strTitle = "" ;

		if( LOGLEVEL_DEBUG == nWriteLogLevel )
		{
			strTitle = "DEBUG" ;
		}
		else if( LOGLEVEL_DATA == nWriteLogLevel )
		{
			strTitle = "DATA" ;
		}
		else if( LOGLEVEL_WARN == nWriteLogLevel )
		{
			strTitle = "WARN" ;
		}
		else if( LOGLEVEL_ERROR == nWriteLogLevel )
		{
			strTitle = "ERROR" ;
		}
		else if( LOGLEVEL_FATAL == nWriteLogLevel )
		{
			strTitle = "FATAL" ;
		}
		else
		{
			strTitle = "WARN" ;
		}
//add by liwenyu release模式下就不输出debug级别的信息
// #ifndef _DEBUG
// 		if (LOGLEVEL_DEBUG == nWriteLogLevel  )
// 		{
// 			return true;
// 		}
// #endif
//end
		//根据配置设置输出级别
		if (nWriteLogLevel<m_nConfigLogLevel )
		{
			return true;
		}
		string strDate = "";
		string strDateTime = "" ;
		GetSysDate( strDate, strDateTime ) ;

		char szFileName[256] ;
		memset(szFileName,0x00,sizeof(szFileName));
		sprintf( szFileName,"net%s.txt", strDate.c_str() ) ;

		long lsec = GetCurSec() ;
		memset(szBuffer,0x00,sizeof(szBuffer));
		sprintf( szBuffer, "[%s %ld][%s][%d]", strDateTime.c_str(), lsec, strTitle.c_str(), nFileLen) ;
		string strResult = "" ;
		strResult = szBuffer  ;

		memset(szBuffer,0x00,sizeof(szBuffer));
		va_list arg_ptr;
		va_start(arg_ptr, str);
		vsnprintf(szBuffer, sizeof(szBuffer)-1, str, arg_ptr);
		va_end(arg_ptr);
		strResult += szBuffer ;
		strResult += "\n" ;

#ifdef WIN32	
		EnterCriticalSection(&m_cs);
		writeonefile(szFileName,strResult.c_str());
		LeaveCriticalSection(&m_cs);
#else
		pthread_mutex_lock(&m_mutex);
		writeonefile(szFileName,strResult.c_str());
		pthread_mutex_unlock(&m_mutex);
#endif
	}
	catch ( ... )
	{
#ifdef WIN32	
		LeaveCriticalSection(&m_cs);	
#else
		pthread_mutex_unlock(&m_mutex);
#endif
		bRet = false ;
	}

	return bRet ;
}

//写日志文件
bool CLog::WriteLog(const char * szfilename,const char *str1, int nFileLen, int nWriteLogLevel )
{
	if( nWriteLogLevel < m_nWriteLogLevel ) {return true;}
	
	bool bRet = true ;
	try
	{
		char szBuffer[4096] ;
		string strTitle = "" ;
		if( LOGLEVEL_DEBUG == nWriteLogLevel )
		{
			strTitle = "DEBUG" ;
		}
		else if( LOGLEVEL_DATA == nWriteLogLevel )
		{
			strTitle = "DATA" ;
		}
		else if( LOGLEVEL_WARN == nWriteLogLevel )
		{
			strTitle = "WARN" ;
		}
		else if( LOGLEVEL_ERROR == nWriteLogLevel )
		{
			strTitle = "ERROR" ;
		}
		else if( LOGLEVEL_FATAL == nWriteLogLevel )
		{
			strTitle = "FATAL" ;
		}
		else
		{
			strTitle = "WARN" ;
		}

		string strDate = "";
		string strDateTime = "" ;
		GetSysDate( strDate, strDateTime ) ;

		char szFileName[256] ;
		memset(szFileName,0x00,sizeof(szFileName));
		sprintf( szFileName,"%s%s.txt",szfilename,strDate.c_str() ) ;
		long lsec = GetCurSec() ;
		memset(szBuffer,0x00,sizeof(szBuffer));
		sprintf( szBuffer, "[%s %ld][%s][%d]", strDateTime.c_str(), lsec, strTitle.c_str(), nFileLen) ;
		string strResult = "" ;
		strResult = szBuffer  ;
		strResult += str1 ;
		strResult += "\n" ;


#ifdef WIN32	
		EnterCriticalSection(&m_cs);
		writeonefile(szFileName,strResult.c_str());
		LeaveCriticalSection(&m_cs);
#else
		pthread_mutex_lock(&m_mutex);
		writeonefile(szFileName,strResult.c_str());
		pthread_mutex_unlock(&m_mutex);
#endif

	}
	catch ( ... )
	{
#ifdef WIN32	
		LeaveCriticalSection(&m_cs);	
#else
		pthread_mutex_unlock(&m_mutex);
#endif
		printf( "WriteLog error !  Line No = %d\n", __LINE__ ) ;
		bRet = false ;
	}

	return bRet ;
}



int CLog::writeonefile(const char *szname, const char *pbuff)
{
	if(NULL == pbuff) { return 0; }
	int len = 0;
	try
	{
		FILE *pfile = fopen(szname,"a+");
		int lensum = strlen(pbuff);
		int lenwriteonetime = 0;
		const char *pbuffwrite = pbuff; 
		int writebytes = (lensum>100)?100:lensum;

		while((lenwriteonetime=fwrite(pbuffwrite, sizeof(char), writebytes, pfile))>0
			&& (lensum>0))
		{
			pbuffwrite += lenwriteonetime;
			lensum -= lenwriteonetime;
			len += lenwriteonetime;
			writebytes = (lensum>100)?100:lensum;
		}
		fclose(pfile);
	}
	catch ( ... )
	{
		printf( "writeonefile error !  Line No = %d\n", __LINE__ ) ;
		len = 0 ;
	}

	return len;
}

bool CLog::GetSysDate( string &_strDate, string &_strDateTime )
{
	bool bRet = true ;

	try
	{
		time_t cur ;
		struct tm *pTime ;
		char szTmp[256] ;

		memset(szTmp,0x00,sizeof(szTmp));

		time(&cur) ;
		pTime = localtime(&cur) ;
		sprintf(szTmp, "%4d%02d%02d",(1900+pTime->tm_year), (1+pTime->tm_mon), pTime->tm_mday) ;

		_strDate = szTmp ;

		memset(szTmp,0x00,sizeof(szTmp));
		sprintf(szTmp, "%4d%02d%02d%02d%02d%02d",(1900+pTime->tm_year), (1+pTime->tm_mon), pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec ) ;
		_strDateTime = szTmp ;
	}
	catch ( ... )
	{
		printf( "GetSysDate error !  Line No = %d\n", __LINE__ ) ;
		bRet = false ;
	}

	return bRet ;
}

void CLog::SetLogLevel( string _strLogLevel)
{
	string str = _strLogLevel;
	if(0 == strcmp(str.c_str(),"DEBUG") )
	{
		m_nWriteLogLevel = LOGLEVEL_DEBUG;
	}
	else if( 0 == strcmp(str.c_str(),"DATA") ) 
	{
		m_nWriteLogLevel = LOGLEVEL_DATA;
	}
	else if( 0 == strcmp(str.c_str(),"WARN") ) 
	{
		m_nWriteLogLevel = LOGLEVEL_WARN;
	}
	else if( 0 == strcmp(str.c_str(),"ERROR") ) 
	{
		m_nWriteLogLevel = LOGLEVEL_ERROR;
	}
	else if( 0 == strcmp(str.c_str(),"FATAL") ) 
	{
		m_nWriteLogLevel = LOGLEVEL_FATAL;
	}
	else
	{
		m_nWriteLogLevel = LOGLEVEL_DEBUG;
	}
}

CLog::CLog()
{
	m_nWriteLogLevel = LOGLEVEL_DEBUG;
	//m_nConfigLogLevel = LOGLEVEL_DEBUG;
#ifdef  WIN32
	InitializeCriticalSection(&m_cs);
#else
	pthread_mutex_init(&m_mutex,NULL);
#endif
}

CLog::~CLog()
{
#ifdef  WIN32
	DeleteCriticalSection(&m_cs);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}

