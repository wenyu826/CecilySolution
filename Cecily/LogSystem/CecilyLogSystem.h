#ifndef Cecily_Log_System_H
#define Cecily_Log_System_H


#ifdef CLEARLOGSYSTEM_EXPORTS
#define CLEAR_LOG_SYSTEM_API __declspec(dllexport)
#else
#define CLEAR_LOG_SYSTEM_API __declspec(dllimport)
#endif

enum Cecily_LogLevel
{
	LOGGER_DEBUG,
	LOGGER_INFO,
	LOGGER_WARN,
	LOGGER_ERROR,
	LOGGER_FATAL	
};


CLEAR_LOG_SYSTEM_API bool StartLogServer(int nProt);
CLEAR_LOG_SYSTEM_API void ConnectToLogServer(const char* lpAddress, int nPort);
CLEAR_LOG_SYSTEM_API void WriteLog(const char* logger,Cecily_LogLevel nlevel, const char* lpMsg);

#endif