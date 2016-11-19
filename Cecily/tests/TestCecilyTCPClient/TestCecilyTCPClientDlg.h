
// TestCecilyTCPClientDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "MyButton.h"
#include "CecilyTCP.h"
#include "InfoDisplayCtrl.h"
#include "afxcmn.h"


// CTestCecilyTCPClientDlg dialog
class CTestCecilyTCPClientDlg : public CDialog
{
// Construction
public:
	CTestCecilyTCPClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTCLEARTCPCLIENT_DIALOG };

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
	CEdit m_edServer;
	CEdit m_edPort;
	CMyButton m_btnConnect;
	CMyButton m_btnSend;
	CMyButton m_btnDisconnect;

	static void RecvFromServer(const char* buf, int nRead,void* user_data);
	afx_msg void OnBnClickedBtnConnect();
	CInfoDisplayCtrl m_RichEditLog;

	CRichEditCtrl m_RichEditSend;
	afx_msg void OnBnClickedBtnSend();

	void AddLogMessage(char* lpBuffer);

	afx_msg void OnBnClickedBtnDisconnect();
	afx_msg void OnBnClickedBtnCycleSend();
};
