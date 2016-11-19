/********************************************************************
	created:	  2016/10/17
	filename: 	  CecilyLogServer.cpp
	author:		  李文禹
	E-Mail：	  hynulwy@126.com
	description:  
*********************************************************************/
#include "stdafx.h"
#include "CecilyLogManager.h"


void ServerThread::run()
{
	try
	{
		while (1)
		{
			if (!clientsock.isOpen())
				break;

			log4cplus::helpers::SocketBuffer msgSizeBuffer(sizeof(unsigned int));
			if (!clientsock.read(msgSizeBuffer))
				break;

			unsigned int msgSize = msgSizeBuffer.readInt();

			log4cplus::helpers::SocketBuffer buffer(msgSize);
			if (!clientsock.read(buffer))
				break;

			log4cplus::spi::InternalLoggingEvent event
				= log4cplus::helpers::readFromBuffer(buffer);
			log4cplus::Logger logger
				= log4cplus::Logger::getInstance(event.getLoggerName());
			logger.callAppenders(event);
		}
	}
	catch (...)
	{
		reaper.visit (self_reference);
		self_reference = log4cplus::thread::AbstractThreadPtr ();
		throw;
	}
	reaper.visit (self_reference);
	self_reference = log4cplus::thread::AbstractThreadPtr ();
}

vector<int> CecilyLogManager::vecPortServer;
std::vector<tstring> CecilyLogManager::m_loggers;

PropertyConfigurator* CecilyLogManager::m_lpPropertyConfig = NULL;

CecilyLogManager::CecilyLogManager(void)
{
	m_bInit = false;
}

CecilyLogManager::~CecilyLogManager(void)
{
}

DWORD WINAPI CecilyLogManager::Thread_Func(LPVOID lParam)
{
	int* lpPort = (int*)lParam;
	int nPort = *lpPort;
	log4cplus::helpers::ServerSocket serverSocket(nPort);
	if (!serverSocket.isOpen()) {
		cout<<"打开端口"<<nPort<<"失败，可能已经被占用"<<endl;
		return 0;
	}
	vecPortServer.push_back(nPort);
	delete lpPort;
	lpPort = NULL;
	Reaper reaper;
	for (;;)
	{
		ServerThread *thr =
			new ServerThread(serverSocket.accept(),reaper);
		cout<<"Accept a new Connection"<<endl;
		thr->start();
	}	
	log4cplus::Logger::shutdown();
	return 1;
}

void CecilyLogManager::InitLogger(const char*lpConfigFile)
{
	log4cplus::initialize ();
	if ( NULL == lpConfigFile )
	{
		return ;
	}
	if ( NULL == m_lpPropertyConfig )
	{
		m_lpPropertyConfig = new PropertyConfigurator(LOG4CPLUS_TEXT(lpConfigFile),log4cplus::getDefaultHierarchy(),0);
		m_lpPropertyConfig->configure();
		log4cplus::helpers::Properties properties = m_lpPropertyConfig->getProperties();
		helpers::Properties loggerProperties = properties.getPropertySubset(LOG4CPLUS_TEXT("logger."));
		m_loggers = loggerProperties.propertyNames();
	}	
}

bool CecilyLogManager::Start(int nPort)
{
	if ( !m_bInit )
	{
		InitLogger(LOG_CONFIG_FILE);
		m_bInit = true;
	}
	vector<int>::iterator itVec = vecPortServer.begin();
	for( itVec; itVec != vecPortServer.end(); itVec++ )
	{
		if ( *itVec == nPort )
		{
			return true;
		}
	}
	int *lpPort = new int(nPort);
	HANDLE hThread = CreateThread(NULL,
		0,
		Thread_Func,
		(void*)lpPort,
		NULL,
		NULL);	
	if ( hThread )
	{
		vecPortServer.push_back(nPort);
	}
	else
	{
		return false;
	}
	CloseHandle(hThread);
	return true;
}


bool CecilyLogManager::ConnectToServer(const char* lpAddress, int nPort)
{
	if ( NULL == lpAddress || nPort <= 0 )
	{
		return false;
	}
	if ( !m_bInit )
	{
		InitLogger(LOG_CONFIG_FILE);
		m_bInit = true;
	}
	SharedAppenderPtr _append(new SocketAppender(lpAddress, nPort, "ServerSocket")); 
	vector<tstring>::iterator itVecLogger = m_loggers.begin();
	for ( itVecLogger ; itVecLogger != m_loggers.end(); itVecLogger++ )
	{
		tstring strLogger = *itVecLogger;
		Logger tmpLogger = Logger::getInstance(LOG4CPLUS_TEXT(strLogger.c_str()));
		tmpLogger.addAppender(_append);
	}
	return true;
}

bool CecilyLogManager::WriteLog(const char* logger,Cecily_LogLevel nlevel, const char* lpMsg)
{
	if ( !m_bInit )
	{
		InitLogger(LOG_CONFIG_FILE);
		m_bInit = true;
	}
	Logger tmp = Logger::getInstance(LOG4CPLUS_TEXT(logger));
	switch(nlevel)
	{
	case LOGGER_DEBUG:
		LOG4CPLUS_DEBUG(tmp, lpMsg); 
		break;
	case LOGGER_INFO:
		LOG4CPLUS_INFO(tmp, lpMsg); 
		break;
	case LOGGER_WARN:
		LOG4CPLUS_WARN(tmp, lpMsg); 
		break;
	case LOGGER_ERROR:
		LOG4CPLUS_ERROR(tmp, lpMsg); 
		break;
	case LOGGER_FATAL:
		LOG4CPLUS_FATAL(tmp, lpMsg); 
		break;
	default:
		return false;
		break;
	}
	return true;
}