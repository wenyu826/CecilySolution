// LogServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
using namespace std;
#include "CecilyLogSystem.h"
#pragma comment(lib,"CecilyLogSystem.lib")


int _tmain(int argc, _TCHAR* argv[])
{
	bool bStart = StartLogServer(9000);
	if ( bStart )
	{
		cout<< "日志服务端口：9000"<<endl;
	}
	else
	{
		system("pause");
		return 1;
	}
	while (true)
	{
		char a;
		cin>>a;
	}
	return 0;
}

