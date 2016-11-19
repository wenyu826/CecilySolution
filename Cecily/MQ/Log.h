#ifndef _KDPARSER_LOG_H_
#define _KDPARSER_LOG_H_
#include<stdio.h>
#include "time.h"
#include <stdio.h>
#include<vector>
#include<string>
#include<map>
using namespace std;

#ifdef WIN32
#include "windows.h"
#else
#include <string.h>
#include <sys/time.h>
#include <stdarg.h>
#include <pthread.h>
#endif

#define  LOGLEVEL_DEBUG		0x10            //在日志中写所有信息以及以上级别
#define  LOGLEVEL_DATA		0x20            //在日志中写出输入参数和执行的SQL以及以上级别
#define  LOGLEVEL_WARN		0x30            //在日志中写出警告信息以及以上级别
#define  LOGLEVEL_ERROR		0x40            //在日志中写出普通错误信息以及以上级别，正常运行基本不受影响
#define  LOGLEVEL_FATAL		0x50            //在日志中写出致命错误信息以及以上级别，程序自动停止运行

//int UTF82MultiChar(unsigned char *inp, char *outp, int size);
//int MultiChar2UTF8(char *inp, unsigned char *outp, int size);

class CLog
{
public:
	static CLog* GetInstance(){
		if( NULL == m_pInstance ){ 
			m_pInstance = new CLog ; 
		}
		return m_pInstance ;
	} ;
	static void ReleaseInstance(){
		if ( NULL != m_pInstance ) {
			delete m_pInstance ;
			m_pInstance = NULL ;
		}
	} ;
	int       writeonefile(const char *szname, const char *pbuff) ;
	bool      GetSysDate( string &_strDate, string &_strDateTime );

	bool      WriteLog(int nFileLen, 
		               int nWriteLogLevel,
					   const char *str, ...) ;
	bool      WriteLog(const char* filename,
		               const char *str1, 
					   int nFileLen, 
					   int nWriteLogLevel = LOGLEVEL_DEBUG ) ;

	void      SetLogLevel( string _strLogLevel);
	void      SetLogLevel( int iLevel)      {  m_nWriteLogLevel = iLevel;  };	
	int       GetLogLevel()                 {  return m_nWriteLogLevel;  };

	void      SetLogPath(string strPath)    {  m_strConfigPath = strPath;  };
	string    GetLogPath()                  {  return m_strConfigPath; };

	long GetCurSec();

	void SetIsWriteDebugLog(bool bWrite);

private:
	string            m_strConfigPath ;     //log path 
	int               m_nWriteLogLevel ;    //log level
	static CLog*      m_pInstance ;
	CLog();
	virtual ~CLog();
	//add by liwenyu 2015-02-26
	static int m_nConfigLogLevel;
	//end

#ifdef WIN32
	CRITICAL_SECTION m_cs;
#else
	pthread_mutex_t m_mutex;
#endif
};

#endif



