
// TestClearActiveMQDlg.h : 头文件
//

#pragma once
#include "ClearActiveMQ.h"
#include "afxwin.h"
#pragma comment(lib,"ClearActiveMQ.lib")


// CTestClearActiveMQDlg 对话框
class CTestClearActiveMQDlg : public CDialog
{
// 构造
public:
	CTestClearActiveMQDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTCLEARACTIVEMQ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edIP;
	CEdit m_edPort;
	CEdit m_edConsumerQueue;
	CEdit m_edConsumerTopic;
	afx_msg void OnBnClickedButton1();

	static void ActiveMQ_Receive_CB(const char* lpQueueName, void* lpBuffer, int nLen,int nType);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CEdit m_edSendTo;
	CEdit m_edSendMsg;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	CEdit m_edUser;
	CEdit m_edPassword;
};
