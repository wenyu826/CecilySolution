
// TestActiveMQDlg.h : header file
//

#pragma once
#include "ClearActiveMQManager.h"


// CTestActiveMQDlg dialog
class CTestActiveMQDlg : public CDialog
{
// Construction
public:
	CTestActiveMQDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTACTIVEMQ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();

	ClearActiveMQManager*	m_lpMQManager;
	afx_msg void OnBnClickedButton2();
};
