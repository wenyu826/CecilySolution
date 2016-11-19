
// TestCecilyTCPServer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTestCecilyTCPServerApp:
// See TestCecilyTCPServer.cpp for the implementation of this class
//

class CTestCecilyTCPServerApp : public CWinAppEx
{
public:
	CTestCecilyTCPServerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
private:
	HMODULE hMod;
};

extern CTestCecilyTCPServerApp theApp;