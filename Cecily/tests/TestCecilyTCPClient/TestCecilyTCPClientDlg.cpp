
// TestCecilyTCPClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestCecilyTCPClient.h"
#include "TestCecilyTCPClientDlg.h"

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


// CTestCecilyTCPClientDlg dialog




CTestCecilyTCPClientDlg::CTestCecilyTCPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestCecilyTCPClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestCecilyTCPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_SERVER, m_edServer);
	DDX_Control(pDX, IDC_ED_PORT, m_edPort);
	DDX_Control(pDX, IDC_RICHEDIT_LOG, m_RichEditLog);
	DDX_Control(pDX, IDC_RICHEDIT22, m_RichEditSend);
}

BEGIN_MESSAGE_MAP(CTestCecilyTCPClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CTestCecilyTCPClientDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_SEND, &CTestCecilyTCPClientDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CTestCecilyTCPClientDlg::OnBnClickedBtnDisconnect)
	ON_BN_CLICKED(IDC_BTN_CYCLE_SEND, &CTestCecilyTCPClientDlg::OnBnClickedBtnCycleSend)
END_MESSAGE_MAP()


// CTestCecilyTCPClientDlg message handlers

BOOL CTestCecilyTCPClientDlg::OnInitDialog()
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
	m_edServer.SetWindowText("127.0.0.1");
	m_edPort.SetWindowText("12345");
	m_btnConnect.Attach(IDC_BTN_CONNECT,this);
	m_btnSend.Attach(IDC_BTN_SEND,this);
	m_btnDisconnect.Attach(IDC_BTN_DISCONNECT,this);
	m_RichEditLog.InitCtrl();
	m_RichEditLog.SetBackgroundColor(FALSE,RGB(0x00,0x00,0x00));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestCecilyTCPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestCecilyTCPClientDlg::OnPaint()
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
HCURSOR CTestCecilyTCPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestCecilyTCPClientDlg::RecvFromServer(const char* buf, int nRead,void* user_data)
{
	CTestCecilyTCPClientDlg* lpDlg = (CTestCecilyTCPClientDlg*)user_data;
	if ( NULL != lpDlg )
	{
		lpDlg->AddLogMessage((char*)buf);
	}

}

void CTestCecilyTCPClientDlg::OnBnClickedBtnConnect()
{
	// TODO: Add your control notification handler code here
	CString strServer,strPort;
	m_edServer.GetWindowText(strServer);
	m_edPort.GetWindowText(strPort);
	SetClientRecvCallBack(RecvFromServer,this);
	bool bConnect = ConnectToServer(strServer.GetBuffer(),atoi(strPort.GetBuffer()));
	if ( bConnect )
	{
		AddLogMessage("服务器连接成功");
	}
}

void CTestCecilyTCPClientDlg::OnBnClickedBtnSend()
{
	// TODO: Add your control notification handler code here
	CString strSendMsg;
	m_RichEditSend.GetWindowText(strSendMsg);
	bool bSend = SendDataToServer(strSendMsg.GetBuffer(),strSendMsg.GetLength());
	if ( !bSend )
	{
		AddLogMessage("消息发送失败");
	}
}

void CTestCecilyTCPClientDlg::AddLogMessage(char* lpBuffer)
{
	if ( NULL == lpBuffer )
	{
		return ;
	}
	m_RichEditLog.AddMessageInfo(lpBuffer);
}

void CTestCecilyTCPClientDlg::OnBnClickedBtnDisconnect()
{
	Disconnect();
}

void CTestCecilyTCPClientDlg::OnBnClickedBtnCycleSend()
{
	// TODO: Add your control notification handler code here
	CString strSendMsg;
	m_RichEditSend.GetWindowText(strSendMsg);
	int nLen = strSendMsg.GetLength();
	const char* lpMsg = strSendMsg.GetBuffer(nLen);
	for(int i = 0 ; i < 10000; i++ )
	{
		SendDataToServer(lpMsg,nLen);
	}
	strSendMsg.ReleaseBuffer(nLen);
	
}
