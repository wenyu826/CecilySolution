
// TestCecilyTCPServerDlg.h : header file
//

#pragma once
#include "CecilyTCP.h"
#include "afxwin.h"
#include "MyButton.h"
#include "InfoDisplayCtrl.h"
#include "afxcmn.h"


// CTestCecilyTCPServerDlg dialog
class CTestCecilyTCPServerDlg : public CDialog
{
// Construction
public:
	CTestCecilyTCPServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTCLEARTCPSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	static void RecvDataFromClientCB(int nClientID,const char* buf,int nRead,void* user_data);
	static void NotifyServerStatusCB(int nCliendID,int bNotify,void* user_data);


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
	bool m_bEnableLog;
	CEdit m_edAddress;
	CEdit m_edPort;
// 	CButton m_btnStartServer;
// 	CButton m_btnStopServer;
	CMyButton m_btnStartServer;
	CMyButton m_btnStopServer;
	afx_msg void OnBnClickedBtnStartTcpServer();
	CInfoDisplayCtrl m_richMessage;
	afx_msg void OnBnClickedBtnStopTcpServer();
	int m_nOnlines;
	CStatic m_onlines;
	CRichEditCtrl m_RichEditSend;
	CEdit m_edClientID;
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnOpenLog();
};
