// CecilyLogSystem.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CecilyLogSystem.h"
#include "CecilyLogManager.h"

CecilyLogManager* lpLogMgr = new CecilyLogManager;

CLEAR_LOG_SYSTEM_API bool StartLogServer(int nProt)
{
	if ( nProt <= 0 )
	{
		return false;
	}
	return lpLogMgr->Start(nProt);
}

CLEAR_LOG_SYSTEM_API void ConnectToLogServer(const char* lpAddress, int nPort)
{
	if ( NULL == lpAddress || nPort <= 0 )
	{
		return;
	}
	lpLogMgr->ConnectToServer(lpAddress,nPort);

}

CLEAR_LOG_SYSTEM_API void WriteLog(const char* logger,Cecily_LogLevel nlevel, const char* lpMsg)
{
	if ( NULL == logger || NULL == lpMsg)
	{
		return ;
	}
	lpLogMgr->WriteLog(logger,nlevel,lpMsg);
}