
// TestClearActiveMQ.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTestClearActiveMQApp:
// �йش����ʵ�֣������ TestClearActiveMQ.cpp
//

class CTestClearActiveMQApp : public CWinAppEx
{
public:
	CTestClearActiveMQApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTestClearActiveMQApp theApp;