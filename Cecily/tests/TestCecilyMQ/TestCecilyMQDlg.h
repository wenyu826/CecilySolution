
// TestCecilyMQDlg.h : header file
//

#pragma once
#include "CecilyMQ.h"
#include "GridCtrl.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "InfoDisplayCtrl.h"
#include "MyButton.h"
#include "TransparentStatic.h"
#include "Digistatic.h"
//#include "CecilyRabbitMQ.h"
#include <vector>
using namespace std;

#define Message_Size		1024*1024


struct ExchangeQueue
{
	int nExchangeType;
	string strExchangeName;
	string strQueueName;
	int nQueueType;
	string strServerIP;
};

// CTestCecilyMQDlg dialog
class CTestCecilyMQDlg : public CDialog
{
// Construction
public:
	CTestCecilyMQDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTCLEARMQ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	static DWORD WINAPI ThreadRecv(LPVOID lparam);
	bool ThreadProc();

	static DWORD WINAPI Thread_MQ(LPVOID lparam);
	bool ThreadProc_MQ();

public:
// 	static DWORD WINAPI GetMessageThread(LPVOID lparam);
// 	bool ThreadProc();


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
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	CEdit m_edServer;
	CEdit m_edPort;
	CEdit m_edVirtualHost;
	CEdit m_edUser;
	CEdit m_edPassWord;
	CInfoDisplayCtrl m_RicheditLogs;
	CInfoDisplayCtrl m_RicheditWrite;
	CMyButton m_btnConnect;
	CMyButton m_btnSetQueueInfos;
	CMyButton m_btnStartGetMessage;
	CMyButton m_btnPutMessage;
	CMyButton m_btnDisconnect;
	CMyButton m_btnCloseMQ;
	CMyButton m_btnCycleSend;
	CTransparentStatic m_staticServer;
	CTransparentStatic m_staticPort;
	CTransparentStatic m_staticVirtualHost;
	CTransparentStatic m_staticUser;
	CTransparentStatic m_staticPassword;
	vector<ExchangeQueue> vecs;
	vector<CecilyMQMD>vecRabbitmq;
	CGridCtrl		m_ConfigCtrl;
	void InitConfigGridCtrl();
	afx_msg void OnBnClickedBtnSetQueueInfo();
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnStartGetMessage();
	HANDLE m_hThreadHandle;
	afx_msg void OnBnClickedBtnPutMessage();
	CEdit m_edPutQueue;
	afx_msg void OnBnClickedBtnDisconnect();
	afx_msg void OnBnClickedBtnCycleSend();
	afx_msg void OnBnClickedButton5();

	static void Active_Recv_CB(CecilyMQMD* _mqmd, void* buffer, int nlen,void* user_data);
	CLEARMQ_HCONN	m_hConn;
	CLEARMQ_HMQ		m_hMQ;
	bool	m_bRabbitmq;

	bool	m_bPubSub;

	CMyButton m_btn5;
	CMyButton m_btn6;
	CMyButton m_btn7;
	CMyButton m_btn8;
	CMyButton m_btn9;
	CMyButton m_btn10;
	CMyButton m_btn11;
	CMyButton m_btn12;
	CMyButton m_btn13;
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	CEdit m_edExchange;
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedButton14();
};
