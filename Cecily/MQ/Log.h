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

#define  LOGLEVEL_DEBUG		0x10            //����־��д������Ϣ�Լ����ϼ���
#define  LOGLEVEL_DATA		0x20            //����־��д�����������ִ�е�SQL�Լ����ϼ���
#define  LOGLEVEL_WARN		0x30            //����־��д��������Ϣ�Լ����ϼ���
#define  LOGLEVEL_ERROR		0x40            //����־��д����ͨ������Ϣ�Լ����ϼ����������л�������Ӱ��
#define  LOGLEVEL_FATAL		0x50            //����־��д������������Ϣ�Լ����ϼ��𣬳����Զ�ֹͣ����

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



