#include "stdafx.h"
#include "AssistUtility.h"
#include <direct.h>


CAssistUtility::CAssistUtility(void)
{
}


CAssistUtility::~CAssistUtility(void)
{
}

string CAssistUtility::GetNowTime()
{
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	static char szTime[40] = {0};
	sprintf_s(szTime,sizeof(szTime)/sizeof(szTime[0]),"%04d%02d%02d%02d:%02d:%02d.%03d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	return szTime;
}

string CAssistUtility::GenerateGUID()
{
	static char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf_s(buf, sizeof(buf)
			, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1], guid.Data4[2], 
			guid.Data4[3], guid.Data4[4], guid.Data4[5], 
			guid.Data4[6], guid.Data4[7]);
	}
	string strGuid = buf;
	return strGuid;
}


void CAssistUtility::AutoCreateDirectory(const std::string &strFilePath)
{
	int nCount = 0;
	string strTempPath = "";
	for(size_t i=0; i< strFilePath.size(); ++i)
	{
		if(strFilePath[i] == '/' || strFilePath[i] == '\\')
		{
			nCount ++;
			if(nCount > 1)
			{
				_mkdir(strTempPath.c_str());
			}
		}
		strTempPath += strFilePath[i];
	}
}

