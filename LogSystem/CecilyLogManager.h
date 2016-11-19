/********************************************************************
	created:	  2016/10/17
	filename: 	  CecilyLogServer.h
	author:		  ¿ÓŒƒ”Ì
	E-Mail£∫	  hynulwy@126.com
	description:  
*********************************************************************/
#ifndef Cecily_Log_Manager_H
#define Cecily_Log_Manager_H
#include "CecilyLogHelp.h"
#include <list>
#include <iostream>
#include "CecilyReaper.h"
#include "CecilyLogSystem.h"

#define LOG_CONFIG_FILE		"log.conf"


class ServerThread
	: public log4cplus::thread::AbstractThread
{
public:
	ServerThread(log4cplus::helpers::Socket clientsock_, Reaper & reaper_)
		: self_reference (log4cplus::thread::AbstractThreadPtr (this))
		, clientsock(clientsock_)
		, reaper (reaper_)
	{
		//std::cout << "Received a client connection!!!!" << std::endl;
	}

	~ServerThread()
	{
		//std::cout << "Client connection closed." << std::endl;
	}

	virtual void run();

private:
	log4cplus::thread::AbstractThreadPtr self_reference;
	log4cplus::helpers::Socket clientsock;
	Reaper & reaper;
};




class CecilyLogManager
{
public:
	CecilyLogManager(void);
	virtual ~CecilyLogManager(void);

	bool Start(int nPort);

	bool ConnectToServer(const char* lpAddress, int nPort);

	std::vector<tstring> GetLoggers()
	{
		return m_loggers;
	}

	bool WriteLog(const char* logger,Cecily_LogLevel nlevel, const char* lpMsg);


private:
	bool m_bInit;

	static void InitLogger(const char*lpConfigFile);

	static DWORD WINAPI Thread_Func(LPVOID lParam);

	static vector<int> vecPortServer;

	static PropertyConfigurator* m_lpPropertyConfig;

	static std::vector<tstring> m_loggers;
};


#endif
