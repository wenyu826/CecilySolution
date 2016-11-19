
// TestCecilyTCPServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestCecilyTCPServer.h"
#include "TestCecilyTCPServerDlg.h"
#include "CecilyLogSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CTestCecilyTCPServerDlg dialog




CTestCecilyTCPServerDlg::CTestCecilyTCPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestCecilyTCPServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nOnlines = 0;
	m_bEnableLog = false;
}

void CTestCecilyTCPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_IP_ADDRESS, m_edAddress);
	DDX_Control(pDX, IDC_EDIT2, m_edPort);
	DDX_Control(pDX, IDC_RICHEDIT_MESSAGE, m_richMessage);
	// 	DDX_Control(pDX,IDC_BTN_START_TCP_SERVER,m_btnStartServer);
	// 	DDX_Control(pDX,IDC_BTN_STOP_TCP_SERVER,m_btnStopServer);
	DDX_Control(pDX, IDC_ST_CURRENT_ONLINES, m_onlines);
	DDX_Control(pDX, IDC_RICHEDIT21, m_RichEditSend);
	DDX_Control(pDX, IDC_EDIT_CLIENT, m_edClientID);
}

BEGIN_MESSAGE_MAP(CTestCecilyTCPServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START_TCP_SERVER, &CTestCecilyTCPServerDlg::OnBnClickedBtnStartTcpServer)
	ON_BN_CLICKED(IDC_BTN_STOP_TCP_SERVER, &CTestCecilyTCPServerDlg::OnBnClickedBtnStopTcpServer)
	ON_BN_CLICKED(IDC_BTN_SEND, &CTestCecilyTCPServerDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_OPEN_LOG, &CTestCecilyTCPServerDlg::OnBnClickedBtnOpenLog)
END_MESSAGE_MAP()


// CTestCecilyTCPServerDlg message handlers

BOOL CTestCecilyTCPServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//设置回调信息
	m_richMessage.InitCtrl();
	m_richMessage.SetBackgroundColor(FALSE,RGB(0x00,0x00,0x00));
	m_btnStartServer.Attach(IDC_BTN_START_TCP_SERVER,this);
	m_btnStopServer.Attach(IDC_BTN_STOP_TCP_SERVER,this);
	SetNotifyServerStatusCallBack(NotifyServerStatusCB,this);
	SetServerRecvCallBack(RecvDataFromClientCB,this);
	m_edAddress.SetWindowText("0.0.0.0");
	m_edPort.SetWindowText("12345");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestCecilyTCPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestCecilyTCPServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestCecilyTCPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestCecilyTCPServerDlg::RecvDataFromClientCB(int nClientID,const char* buf,int nRead,void* user_data)
{
	//客户端发来的消息通知
	CTestCecilyTCPServerDlg* lpServerDlg = (CTestCecilyTCPServerDlg*)user_data;
	if ( NULL != user_data )
	{
// 		CString strMsg;
// 		char *szBuffer = new char[nRead+1];
// 		memset(szBuffer,0,nRead+1);
// 		sprintf_s(szBuffer,nRead+1,buf,nRead);
// 		strMsg.Format("收到客户端ID:%d 消息:%s\n",nClientID,szBuffer);
// 		lpServerDlg->m_richMessage.AddMessageInfo(strMsg.GetBuffer());
// 		strMsg.ReleaseBuffer();
// 		delete []szBuffer;
// 		szBuffer = NULL;
		if ( lpServerDlg->m_bEnableLog )
		{
			WriteLog("cleartcp",LOGGER_DEBUG,buf);
		}
	}
	
}

void CTestCecilyTCPServerDlg::NotifyServerStatusCB(int nCliendID,int bNotify,void* user_data)
{
	CTestCecilyTCPServerDlg* lpServerDlg = (CTestCecilyTCPServerDlg*)user_data;
	if ( NULL == lpServerDlg )
	{
		return ;
	}
	if ( nCliendID == -1 )
	{
		lpServerDlg->m_richMessage.AddMessageInfo("TCPServer Stopped");
		return ;
	}
	if ( bNotify )
	{
		CString strMsg;
		strMsg.Format("新客户端连入，ID : %d\n",nCliendID);
		lpServerDlg->m_richMessage.AddMessageInfo(strMsg.GetBuffer());
		lpServerDlg->m_nOnlines++;
		CString str;
		str.Format("当前在线人数：%d",lpServerDlg->m_nOnlines);
		lpServerDlg->m_onlines.SetWindowText(str.GetBuffer());
		
	}else
	{
		CString strMsg;
 		strMsg.Format("客户端断开连接，ID : %d\n",nCliendID);
 		lpServerDlg->m_richMessage.AddMessageInfo(strMsg.GetBuffer());
		lpServerDlg->m_nOnlines--;
		CString str;
		str.Format("当前在线人数：%d",lpServerDlg->m_nOnlines);
		lpServerDlg->m_onlines.SetWindowText(str.GetBuffer());
	}

}


void CTestCecilyTCPServerDlg::OnBnClickedBtnStartTcpServer()
{
	CString strIPAddress , strPort;
	m_edAddress.GetWindowText(strIPAddress);
	m_edPort.GetWindowText(strPort);
	int nPort = atoi(strPort.GetBuffer());
	CString strMsg;
	m_richMessage.AddMessageInfo("准备开启TCP服务");
	bool bStart = StartServer(strIPAddress.GetBuffer(),nPort);
	if ( bStart )
	{
		strMsg.Format("TCPServer监听地址：%s 端口：%d 服务开启成功\n",strIPAddress.GetBuffer(),nPort);
		m_richMessage.AddMessageInfo(strMsg.GetBuffer());
	}
	else
	{
		strMsg.Format("TCPServer监听地址：%s 端口：%d 服务开启失败\n",strIPAddress.GetBuffer(),nPort);
		m_richMessage.AddMessageInfo(strMsg.GetBuffer(),RGB(255,0,0));
	}
	

}

void CTestCecilyTCPServerDlg::OnBnClickedBtnStopTcpServer()
{
	m_richMessage.AddMessageInfo("停止TCP服务");
	StopServer();
}

void CTestCecilyTCPServerDlg::OnBnClickedBtnSend()
{
	// TODO: Add your control notification handler code here
	CString strSendMsg, strClientID;
	m_RichEditSend.GetWindowText(strSendMsg);
	m_edClientID.GetWindowText(strClientID);
	int nClientID = atoi(strClientID.GetBuffer());
	SendDataToClient(nClientID,strSendMsg.GetBuffer(),strSendMsg.GetLength());
}

void CTestCecilyTCPServerDlg::OnBnClickedBtnOpenLog()
{
	// TODO: Add your control notification handler code here
	ConnectToLogServer("127.0.0.1",9000);
	m_bEnableLog = true;
}
