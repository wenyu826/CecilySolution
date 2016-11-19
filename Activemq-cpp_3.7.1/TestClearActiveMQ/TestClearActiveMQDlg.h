
// TestClearActiveMQDlg.h : ͷ�ļ�
//

#pragma once
#include "ClearActiveMQ.h"
#include "afxwin.h"
#pragma comment(lib,"ClearActiveMQ.lib")


// CTestClearActiveMQDlg �Ի���
class CTestClearActiveMQDlg : public CDialog
{
// ����
public:
	CTestClearActiveMQDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTCLEARACTIVEMQ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
