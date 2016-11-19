
// TestClearActiveMQDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestClearActiveMQ.h"
#include "TestClearActiveMQDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestClearActiveMQDlg �Ի���




CTestClearActiveMQDlg::CTestClearActiveMQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestClearActiveMQDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestClearActiveMQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_IP, m_edIP);
	DDX_Control(pDX, IDC_EDIT2, m_edPort);
	DDX_Control(pDX, IDC_EDIT3, m_edConsumerQueue);
	DDX_Control(pDX, IDC_EDIT4, m_edConsumerTopic);
	DDX_Control(pDX, IDC_EDIT5, m_edSendTo);
	DDX_Control(pDX, IDC_EDIT6, m_edSendMsg);
	DDX_Control(pDX, IDC_EDIT1, m_edUser);
	DDX_Control(pDX, IDC_EDIT7, m_edPassword);
}

BEGIN_MESSAGE_MAP(CTestClearActiveMQDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTestClearActiveMQDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestClearActiveMQDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestClearActiveMQDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestClearActiveMQDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTestClearActiveMQDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestClearActiveMQDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CTestClearActiveMQDlg ��Ϣ�������

BOOL CTestClearActiveMQDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_edIP.SetWindowText("127.0.0.1");
	m_edPort.SetWindowText("10086");
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTestClearActiveMQDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestClearActiveMQDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestClearActiveMQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestClearActiveMQDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString strServer, strPort,strUser,strPassword;
	m_edIP.GetWindowText(strServer);
	m_edPort.GetWindowText(strPort);
	m_edUser.GetWindowText(strUser);
	m_edPassword.GetWindowText(strPassword);
	int nPort = atoi(strPort.GetBuffer());
	SetReceiveCallBack(CTestClearActiveMQDlg::ActiveMQ_Receive_CB);
	bool bConnect = ConnectToMQ(strServer.GetBuffer(),nPort,strUser.GetBuffer(),strPassword.GetBuffer());
	if ( bConnect )
	{
		MessageBox("ActiveMQ���ӳɹ�");
	}
}


void CTestClearActiveMQDlg::ActiveMQ_Receive_CB(const char* lpQueueName, void* lpBuffer, int nLen, int nType)
{
	char* buf = new char[nLen+1];
	memset(buf,0,nLen+1);
	memcpy(buf,lpBuffer,nLen);
	TRACE("%s : %s\n ",lpQueueName,buf);
}
void CTestClearActiveMQDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CString strQueue;
	m_edConsumerQueue.GetWindowText(strQueue);
	string strTemp = strQueue.GetBuffer();
	int nPos;
	while ( nPos = strTemp.find(";") != string::npos )
	{
		string str = strTemp.substr(0,nPos+1);
		OpenMQ(str.c_str(),1);
		strTemp = strTemp.substr(nPos+2,strTemp.length());
	}
	if ( strTemp != "" )
	{
		OpenMQ(strTemp.c_str(),1);
	}
	
}

void CTestClearActiveMQDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	CString strQueue;
	m_edConsumerTopic.GetWindowText(strQueue);
	string strTemp = strQueue.GetBuffer();
	int nPos;
	while ( nPos = strTemp.find(";") != string::npos )
	{
		string str = strTemp.substr(0,nPos+1);
		OpenMQ(str.c_str(),0);
		strTemp = strTemp.substr(nPos+2,strTemp.length());
	}
	if ( strTemp != "" )
	{
		OpenMQ(strTemp.c_str(),0);
	}

}

void CTestClearActiveMQDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	int nCheck = ((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck();
	CString strSendMsg,strSendTo;
	m_edSendMsg.GetWindowText(strSendMsg);
	m_edSendTo.GetWindowText(strSendTo);
	if ( nCheck )  //����ģʽ
	{
		PutMessage(strSendTo.GetBuffer(),strSendMsg.GetBuffer(),strSendMsg.GetLength(),M_Text,1);
	}else{
		PutMessage(strSendTo.GetBuffer(),strSendMsg.GetBuffer(),strSendMsg.GetLength(),M_Text,0);
	}
}

void CTestClearActiveMQDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	CString strQueue;
	m_edConsumerQueue.GetWindowText(strQueue);
	string strTemp = strQueue.GetBuffer();
	int nPos;
	while ( nPos = strTemp.find(";") != string::npos )
	{
		string str = strTemp.substr(0,nPos+1);
		DeleteMQ(str.c_str(),false);
		strTemp = strTemp.substr(nPos+2,strTemp.length());
	}
	if ( strTemp != "" )
	{
		DeleteMQ(strTemp.c_str(),false);
	}
}

void CTestClearActiveMQDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	CString strQueue;
	m_edConsumerTopic.GetWindowText(strQueue);
	string strTemp = strQueue.GetBuffer();
	int nPos;
	while ( nPos = strTemp.find(";") != string::npos )
	{
		string str = strTemp.substr(0,nPos+1);
		DeleteMQ(str.c_str(),true);
		strTemp = strTemp.substr(nPos+2,strTemp.length());
	}
	if ( strTemp != "" )
	{
		DeleteMQ(strTemp.c_str(),true);
	}
}
