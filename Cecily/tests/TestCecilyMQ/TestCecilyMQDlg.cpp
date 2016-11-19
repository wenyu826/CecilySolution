
// TestCecilyMQDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestCecilyMQ.h"
#include "TestCecilyMQDlg.h"


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



const char *szExchangeQueue[][5] = {
	"10000","clear.exchange","put_1","0","127.0.0.1",
	"10000","clear.direct","req_bengine","0","127.0.0.1",
	"10000","clear.direct","ans_bengine","1","127.0.0.1"
};

// CTestCecilyMQDlg dialog
const char* szCols[] = {
	"交换机类型",
	"交换机名称",
	"队列名称",
	"队列类型",
	"服务器IP"
};

bool CTestCecilyMQDlg::ThreadProc_MQ()
{
	char* lpBuffer = new char[1024*1024*10];
	
	while (true)
	{
		memset(lpBuffer,0,1024*1024*10);
		char *lpQueueName =NULL;
		char* lpMessageBody = NULL;
		CecilyMQMD _mqmd;
		long dataLen = 0;
		long code,reason;

		Cecily_MQGet(this->m_hConn,NULL,&_mqmd,NULL,0,lpBuffer,&dataLen,&code,&reason);
		if ( code == Cecily_Success )
		{
			char szMsg[256] = {0};
			static int nCount = 0;
			int dwTicouts = GetTickCount();
			sprintf(szMsg,"收到消息数量:%d 时间戳%d",nCount++,dwTicouts);
			m_RicheditLogs.AddMessageInfo(szMsg,RGB(0,0,255));
			//m_RicheditLogs.AddMessageInfo(lpBuffer,RGB(0,255,0));
			if ( NULL != lpQueueName )
			{
				delete []lpQueueName;
				lpQueueName = NULL;
			}
			if ( NULL != lpMessageBody )
			{
				delete []lpMessageBody;
				lpMessageBody = NULL;
			}
		}else
		{
			if ( code == Cecily_Invalid_Connection )
			{
				m_RicheditLogs.AddMessageInfo("连接断开，请重新连接",RGB(255,0,0));
				Sleep(5000);
				long code, reason;
				Cecily_MQReconnect(m_hConn,&code,&reason);
				if ( code == Cecily_Success )
				{
					m_RicheditLogs.AddMessageInfo("重连成功",RGB(255,0,0));
				}
// 				CecilyMQStatus  status = ReConnectMQ();
// 				if ( Cecily_Success == status )
// 				{
// 					m_RicheditLogs.AddMessageInfo("重连成功");
// 				}
			}
			else if( Cecily_GetMessage_OutTime == code )
			{
				m_RicheditLogs.AddMessageInfo("获取队列信息超时",RGB(255,0,0));
			}
			else
			{
// 				const char* lpErrorMsg = GetErrorMsg((CecilyMQStatus)&code);
// 				if ( NULL != lpErrorMsg )
// 				{
// 					m_RicheditLogs.AddMessageInfo(lpErrorMsg,RGB(255,0,0));
// 				}

			}

		}
	}
	if ( NULL != lpBuffer )
	{
		delete []lpBuffer;
		lpBuffer = NULL;
	}


}

bool CTestCecilyMQDlg::ThreadProc()
{
	/*
	while (true)
	{
		char *lpQueueName =NULL;
		char* lpMessageBody = NULL;
		CecilyMQStatus status = GetMessage_(&lpQueueName,&lpMessageBody);
		if ( status == Cecily_Success )
		{
			m_RicheditLogs.AddMessageInfo(lpMessageBody,RGB(0,255,0));
			if ( NULL != lpQueueName )
			{
				delete []lpQueueName;
				lpQueueName = NULL;
			}
			if ( NULL != lpMessageBody )
			{
				delete []lpMessageBody;
				lpMessageBody = NULL;
			}
		}else
		{
			if ( status == Cecily_Invalid_Connection )
			{
				m_RicheditLogs.AddMessageInfo("连接断开，请重新连接",RGB(255,0,0));
				Sleep(5000);
				CecilyMQStatus  status = ReConnectMQ();
				if ( Cecily_Success == status )
				{
					m_RicheditLogs.AddMessageInfo("重连成功");
				}
			}
			else if( Cecily_GetMessage_OutTime == status )
			{
				m_RicheditLogs.AddMessageInfo("获取队列信息超时",RGB(255,0,0));
			}
			else
			{
				const char* lpErrorMsg = GetErrorMsg(status);
				if ( NULL != lpErrorMsg )
				{
					m_RicheditLogs.AddMessageInfo(lpErrorMsg,RGB(255,0,0));
				}
				
			}
			
		}
		
		
	}
	*/
	return true;
}

DWORD WINAPI CTestCecilyMQDlg::ThreadRecv(LPVOID lparam)
{
	/*
	CTestCecilyMQDlg *lpThis = (CTestCecilyMQDlg*)lparam;
	try
	{
		lpThis->ThreadProc();
	}
	catch (...)
	{
	}
	*/
	
	return 1;
}


DWORD WINAPI CTestCecilyMQDlg::Thread_MQ(LPVOID lparam)
{
	CTestCecilyMQDlg *lpThis = (CTestCecilyMQDlg*)lparam;
	try
	{
		lpThis->ThreadProc_MQ();
	}
	catch (...)
	{
	}

	return 1;

}


CTestCecilyMQDlg::CTestCecilyMQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestCecilyMQDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hThreadHandle = NULL;
}

void CTestCecilyMQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_GridControl(pDX, IDC_CONFIG_LIST_CTRL, m_ConfigCtrl);
	DDX_Control(pDX, IDC_ED_SERVER, m_edServer);
	DDX_Control(pDX, IDC_ED_PORT, m_edPort);
	DDX_Control(pDX, IDC_ED_VIRTUALHOST, m_edVirtualHost);
	DDX_Control(pDX, IDC_ED_USER, m_edUser);
	DDX_Control(pDX, IDC_ED_PASSWORD, m_edPassWord);
	DDX_Control(pDX, IDC_RICHEDIT_LOGS, m_RicheditLogs);
	DDX_Control(pDX, IDC_RICHEDIT_WRITE, m_RicheditWrite);
	//DDX_Control(pDX, IDC_BTN_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_STATIC_SERVER, m_staticServer);
	DDX_Control(pDX, IDC_STATIC_PORT, m_staticPort);
	DDX_Control(pDX, IDC_STATIC_VIRTUAL_HOST, m_staticVirtualHost);
	DDX_Control(pDX, IDC_STATIC_USER, m_staticUser);
	DDX_Control(pDX, IDC_STATIC_PASSWORD, m_staticPassword);
	DDX_Control(pDX, IDC_ED_PUT_QUEUE, m_edPutQueue);
	DDX_Control(pDX, IDC_EDIT1, m_edExchange);
}

void CTestCecilyMQDlg::InitConfigGridCtrl()
{
	int nCols = sizeof(szCols)/sizeof(szCols[0]);
	CRect rc;
	m_ConfigCtrl.GetClientRect(&rc);
	int nWidth = rc.Width();
	int nColWidth = nWidth/nCols;
	m_ConfigCtrl.SetEditable(true);
	m_ConfigCtrl.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));//黄色背景
	m_ConfigCtrl.SetRowCount(7); //
	m_ConfigCtrl.SetColumnCount(nCols); //
	m_ConfigCtrl.SetFixedRowCount(1); //表头为一行
	//m_ConfigCtrl.SetFixedColumnCount(1); //表头为一列
	int nExchangeQueueRow = sizeof(szExchangeQueue)/sizeof(szExchangeQueue[0]);
	for (int row = 0; row < m_ConfigCtrl.GetRowCount(); row++)
		for (int col = 0; col < m_ConfigCtrl.GetColumnCount(); col++)
		{ 
			//设置表格显示属性
			GV_ITEM Item; 
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			m_ConfigCtrl.SetRowHeight(row,25); //设置各行高          
			m_ConfigCtrl.SetColumnWidth(col,nColWidth); //设置各列宽
			if ( row == 0 && col < nCols)
			{
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				Item.strText.Format(_T(szCols[col]),2);
			}
			if ( row >=1 && row < nExchangeQueueRow+1 && col < nCols)
			{
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				Item.strText.Format(_T(szExchangeQueue[row-1][col]),2);
			}
			m_ConfigCtrl.SetItem(&Item); 
		}

}

BEGIN_MESSAGE_MAP(CTestCecilyMQDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTestCecilyMQDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestCecilyMQDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestCecilyMQDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestCecilyMQDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BTN_SET_QUEUE_INFO, &CTestCecilyMQDlg::OnBnClickedBtnSetQueueInfo)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CTestCecilyMQDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_START_GET_MESSAGE, &CTestCecilyMQDlg::OnBnClickedBtnStartGetMessage)
	ON_BN_CLICKED(IDC_BTN_PUT_MESSAGE, &CTestCecilyMQDlg::OnBnClickedBtnPutMessage)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CTestCecilyMQDlg::OnBnClickedBtnDisconnect)
	ON_BN_CLICKED(IDC_BTN_CYCLE_SEND, &CTestCecilyMQDlg::OnBnClickedBtnCycleSend)
	ON_BN_CLICKED(IDC_BUTTON5, &CTestCecilyMQDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestCecilyMQDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CTestCecilyMQDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CTestCecilyMQDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CTestCecilyMQDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CTestCecilyMQDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CTestCecilyMQDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CTestCecilyMQDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_RADIO2, &CTestCecilyMQDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON13, &CTestCecilyMQDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_RADIO1, &CTestCecilyMQDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO3, &CTestCecilyMQDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CTestCecilyMQDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_BUTTON14, &CTestCecilyMQDlg::OnBnClickedButton14)
END_MESSAGE_MAP()


// CTestCecilyMQDlg message handlers

BOOL CTestCecilyMQDlg::OnInitDialog()
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
	InitConfigGridCtrl();

	m_edVirtualHost.SetWindowText("/");
	m_edServer.SetWindowText("127.0.0.1");
	m_edPort.SetWindowText("5672");
	m_edUser.SetWindowText("clear");
	m_edPassWord.SetWindowText("clear");


	m_RicheditLogs.InitCtrl();
	//m_RicheditLogs.SetBackgroundColor(FALSE,RGB(0x00,0x00,0x00));
	//m_RicheditLogs.AddMessageInfo("系统启动......",RGB(0,255,0));
	m_RicheditWrite.InitCtrl();
	m_RicheditWrite.SetBackgroundColor(FALSE,RGB(174,236,236));
	m_btnConnect.Attach(IDC_BTN_CONNECT,this);
	m_btnSetQueueInfos.Attach(IDC_BTN_SET_QUEUE_INFO,this);
	m_btnStartGetMessage.Attach(IDC_BTN_START_GET_MESSAGE,this);
	m_btnPutMessage.Attach(IDC_BTN_PUT_MESSAGE,this);
	m_btnDisconnect.Attach(IDC_BTN_DISCONNECT,this);
	m_btnCloseMQ.Attach(IDC_BTN_CLOSE_MQ,this);
	m_btnCycleSend.Attach(IDC_BTN_CYCLE_SEND,this);

	m_btn5.Attach(IDC_BUTTON5,this);
	m_btn6.Attach(IDC_BUTTON6,this);
	m_btn7.Attach(IDC_BUTTON7,this);
	m_btn8.Attach(IDC_BUTTON8,this);
	m_btn9.Attach(IDC_BUTTON9,this);
	m_btn10.Attach(IDC_BUTTON10,this);
	m_btn11.Attach(IDC_BUTTON11,this);
	m_btn12.Attach(IDC_BUTTON12,this);
	m_btn13.Attach(IDC_BUTTON13,this);

	m_bRabbitmq = false;
	m_bPubSub = false;
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestCecilyMQDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestCecilyMQDlg::OnPaint()
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
HCURSOR CTestCecilyMQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestCecilyMQDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	//HMODULE hModule = LoadLibrary("CecilyMQ.dll");
// 	CecilyMQStatus status = ConnectMQ("127.0.0.1","/",5672,"clear","clear");
// 	if ( Cecily_Success == status )
// 	{
// 		MessageBox("connect succeed");
// 	}else
// 	{
// 		const char *lpErrorMsg = GetErrorMsg(status);
// 		MessageBox(lpErrorMsg);
// 	}
	
}

void CTestCecilyMQDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
// 	CreateThread(NULL,
// 		0,
// 		ThreadRecv,
// 		(void*)this,
// 		NULL,NULL);
}

void CTestCecilyMQDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
// 	string strqueue = "test_get1";
// 	string strmsg = "hello rabbitmq";
// 	char szBuffer[1024] = {0};
// 	
// 	for ( int i = 0; i < 1; i++ )
// 	{
// 		memset(szBuffer,0,1024);
// 		sprintf_s(szBuffer,1024,"hello rabbitmq at time: %d",GetTickCount());
// 		strmsg = szBuffer;
// 		//PutMessage(strqueue,strmsg);
// 	}
	
}

void CTestCecilyMQDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
// 	CecilyMQStatus status = Cecily_Library_Exception;
// 	const char * lpMsg = GetErrorMsg(status);
// 	int a = 10;
}

void CTestCecilyMQDlg::OnBnClickedBtnSetQueueInfo()
{
	// TODO: Add your control notification handler code here
	 
// 	for (int row = 1; row < m_ConfigCtrl.GetRowCount(); row++)
// 	{
// 		ExchangeQueue eq;
// 		bool bGet = true;
// 		for (int col = 0; col < m_ConfigCtrl.GetColumnCount(); col++)
// 		{
// 			CString strItem = m_ConfigCtrl.GetItemText(row,col);
// 			if ( col == 0 )
// 			{
// 				if ( strItem == "")
// 				{
// 					bGet = false;
// 				}
// 				eq.nExchangeType = atoi(strItem.GetBuffer());
// 			}
// 			if ( col == 1 )
// 			{
// 				eq.strExchangeName = strItem.GetBuffer();
// 			}
// 			if ( col == 2 )
// 			{
// 				eq.strQueueName = strItem.GetBuffer();
// 			}
// 			if ( col == 3 )
// 			{
// 				eq.nQueueType = atoi(strItem.GetBuffer());
// 			}
// 			if ( col == 4 )
// 			{
// 				eq.strServerIP = strItem.GetBuffer();
// 			}
// 		}
// 		if ( bGet )
// 		{
// 			vecs.push_back(eq);
// 		}
// 	}
// 	vector<ExchangeQueue>::iterator itVec = vecs.begin();
// 	for ( itVec; itVec != vecs.end(); itVec++ )
// 	{
// 		ExchangeQueue temp = *itVec;
// 		bool bSucceed = DeclareExchangeQueue((CecilyExchangeType)temp.nExchangeType,temp.strExchangeName.c_str(),temp.strQueueName.c_str(),temp.nQueueType,temp.strServerIP.c_str());
// 		if ( bSucceed )
// 		{
// 			m_RicheditLogs.AddMessageInfo("DeclareExchangeQueue succeed",RGB(0,255,0));
// 		}
// 	}
}

void CTestCecilyMQDlg::OnBnClickedBtnConnect()
{
	// TODO: Add your control notification handler code here
	/*
	CString strServerIP;
	CString strVirtualHost, strPort, strUser, strPassword;
	m_edServer.GetWindowText(strServerIP);
	m_edVirtualHost.GetWindowText(strVirtualHost);
	m_edPort.GetWindowText(strPort);
	m_edUser.GetWindowText(strUser);
	m_edPassWord.GetWindowText(strPassword);
	CecilyMQStatus status = ConnectMQ(strServerIP.GetBuffer(),strVirtualHost.GetBuffer(),atoi(strPort.GetBuffer()),strUser.GetBuffer(),strPassword.GetBuffer());
	if ( status == Cecily_Success )
	{
		m_RicheditLogs.AddMessageInfo("连接成功",RGB(0,255,0));
	}else
	{
		const char* szErrorMsg = GetErrorMsg(status);
		char szBuffer[1024] = {0};
		sprintf(szBuffer,"连接失败，失败原因：%s",szErrorMsg);
		m_RicheditLogs.AddMessageInfo(szBuffer,RGB(255,0,0));
	}
	*/
}

void CTestCecilyMQDlg::OnBnClickedBtnStartGetMessage()
{
	// TODO: Add your control notification handler code here
	/*
	if ( NULL == m_hThreadHandle )
	{
		m_hThreadHandle = CreateThread(NULL,
			0,
			ThreadRecv,
			(void*)this,
			NULL,
			NULL);
	}
	*/

}

void CTestCecilyMQDlg::OnBnClickedBtnPutMessage()
{
	// TODO: Add your control notification handler code here
	/*
	CString strQueue, strMsg;
	m_edPutQueue.GetWindowText(strQueue);
	m_RicheditWrite.GetWindowText(strMsg);
	CecilyMQStatus status = PutMessage(strQueue.GetBuffer(),(const void*)strMsg.GetBuffer(strMsg.GetLength()),strMsg.GetLength(),M_Text);
	if ( status == Cecily_Success )
	{
		m_RicheditLogs.AddMessageInfo("消息发送成功",RGB(0,255,0));
	}
	else
	{
		m_RicheditLogs.AddMessageInfo("消息发送失败",RGB(255,0,0));
	}
	strMsg.ReleaseBuffer(strMsg.GetLength());
	*/
}

void CTestCecilyMQDlg::OnBnClickedBtnDisconnect()
{
	// TODO: Add your control notification handler code here
	/*
	CecilyMQStatus status = DisconnectMQ();
	if ( Cecily_Success == status )
	{
		m_RicheditLogs.AddMessageInfo("MQ连接断开");
	}
	*/
}


void CTestCecilyMQDlg::OnBnClickedBtnCycleSend()
{
	// TODO: 在此添加控件通知处理程序代码
	/*
	for ( int i = 0 ; i < 10000; i++ )
	{
		OnBnClickedBtnPutMessage();
	}
	*/
}

void CTestCecilyMQDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	long code, reason;
	CString strServerIP;
	CString strVirtualHost, strPort, strUser, strPassword;
	m_edServer.GetWindowText(strServerIP);
	m_edPort.GetWindowText(strPort);
	
	if ( m_bRabbitmq )
	{
		Cecily_MQConn(strServerIP.GetBuffer(),atoi(strPort.GetBuffer()),"clear","clear",&m_hConn,&code,&reason);
	}
	else
	{
		Cecily_MQConn(strServerIP.GetBuffer(),atoi(strPort.GetBuffer()),"","",&m_hConn,&code,&reason);
	}
	
	if ( code == Cecily_MQ_Not_Selected )
	{
		m_RicheditLogs.AddMessageInfo("请先设置MQ类型",RGB(0,0,0));
	}
	if ( code == Cecily_Success )
	{
		if ( m_bRabbitmq )
		{
			m_RicheditLogs.AddMessageInfo("RabbitMQ连接成功",RGB(0,0,0));
		}
		else
		{
			m_RicheditLogs.AddMessageInfo("ActiveMQ连接成功",RGB(0,0,0));
		}	
	}
	else
	{
		if ( m_bRabbitmq )
		{
			m_RicheditLogs.AddMessageInfo("RabbitMQ连接失败",RGB(255,0,0));
		}
		else
		{
			m_RicheditLogs.AddMessageInfo("ActiveMQ连接失败",RGB(255,0,0));
		}	

	}
}

void CTestCecilyMQDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	long code, reason;
	Cecily_MQDisconn(&m_hConn,0,&code,&reason);
}

void CTestCecilyMQDlg::OnBnClickedButton7()
{
	for (int row = 1; row < m_ConfigCtrl.GetRowCount(); row++)
	{
		ExchangeQueue eq;
		CecilyMQMD mqmd;
		bool bGet = true;
		for (int col = 0; col < m_ConfigCtrl.GetColumnCount(); col++)
		{
			CString strItem = m_ConfigCtrl.GetItemText(row,col);
			if ( col == 0 )
			{
				if ( strItem == "")
				{
					bGet = false;
				}
				eq.nExchangeType = atoi(strItem.GetBuffer());
				mqmd.distribution_type = atoi(strItem.GetBuffer());
			}
			if ( col == 1 )
			{
				eq.strExchangeName = strItem.GetBuffer();
				//RQ.szExchange = strItem.GetBuffer();
				//strcpy_s(RQ.szExchange,eq.strExchangeName.length(),eq.strExchangeName.c_str());
				memcpy(mqmd.exchange_name,eq.strExchangeName.c_str(),eq.strExchangeName.length());
			}
			if ( col == 2 )
			{
				eq.strQueueName = strItem.GetBuffer();
				memcpy(mqmd.send_queue_name,eq.strQueueName.c_str(),eq.strQueueName.length());
				//strcpy_s(RQ.szQueueName,eq.strQueueName.length(),eq.strQueueName.c_str());
			}
			if ( col == 3 )
			{
				eq.nQueueType = atoi(strItem.GetBuffer());
				mqmd.queue_type = atoi(strItem.GetBuffer());
			}
			if ( col == 4 )
			{
				eq.strServerIP = strItem.GetBuffer();
			}
		}
		if ( bGet )
		{
			vecs.push_back(eq);
			vecRabbitmq.push_back(mqmd);
		}
	}
	vector<CecilyMQMD>::iterator itVec = vecRabbitmq.begin();
	for ( itVec; itVec != vecRabbitmq.end(); itVec++ )
	{
		CecilyMQMD _rq = *itVec;
		long code,reason;
		if ( _rq.distribution_type == Topic )
		{
			Cecily_MQOpen_PubSub(m_hConn,&_rq,NULL,NULL,&code,&reason);
		}else
		{

			Cecily_MQOpen(m_hConn,&_rq,NULL,NULL,&code,&reason);
		}
		
	}
	
}

void CTestCecilyMQDlg::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	if ( NULL == m_hThreadHandle )
	{
		m_hThreadHandle = CreateThread(NULL,
			0,
			Thread_MQ,
			(void*)this,
			NULL,
			NULL);
	}
	
}

/*
	FILE* file = fopen("C:\\Users\\jd\\Desktop\\SDK\\A.txt", "r");  
	char* buf = NULL;  //要获取的字符串  
	int len;    //获取的长度  
	if (file) {  
	

		fseek(file, 0, SEEK_END);   //移到尾部  
		len = ftell(file);          //提取长度  
		rewind(file);               //回归原位  
		buf = (char*)malloc(sizeof(char) * len + 1); 
		int rLen = fread(buf, sizeof(char), len, file);  
		buf[rLen] = '\0';  
		string result = buf;  
		fclose(file);  
	}
	Cecily_MQPut(m_hConn,NULL,&_mqmd,NULL,len,buf,&code,&reason);
	if ( code == Cecily_Success )
	{
		m_RicheditLogs.AddMessageInfo("消息发送成功",RGB(0,255,0));
	}
	else
	{
		m_RicheditLogs.AddMessageInfo("消息发送失败",RGB(255,0,0));
	}
		
	return;
*/

void CTestCecilyMQDlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here



	CString strQueue, strMsg, strExchange;
	m_edPutQueue.GetWindowText(strQueue);
	m_RicheditWrite.GetWindowText(strMsg);
	m_edExchange.GetWindowText(strExchange);
	if ( m_bPubSub )
	{
		if ( strExchange.IsEmpty() )
		{
			MessageBox("交换器不能为空");
			return ;
		}
	}
	CecilyMQMD _mqmd;
	memcpy(_mqmd.exchange_name,strExchange.GetBuffer(),strExchange.GetLength());
	memcpy(_mqmd.send_queue_name,strQueue.GetBuffer(),strQueue.GetLength());
	_mqmd.body_type = M_Text;
	long code,reason;
	if ( !m_bPubSub )
	{
		Cecily_MQPut(m_hConn,NULL,&_mqmd,NULL,strMsg.GetLength(),strMsg.GetBuffer(),&code,&reason);
	}
	else
	{
		Cecily_MQPublish(m_hConn,NULL,&_mqmd,NULL,strMsg.GetLength(),strMsg.GetBuffer(),&code,&reason);
	}
	
	if ( code == Cecily_Success )
	{
		m_RicheditLogs.AddMessageInfo("消息发送成功",RGB(0,255,0));
	}
	else
	{
		m_RicheditLogs.AddMessageInfo("消息发送失败",RGB(255,0,0));
	}
	//CecilyMQStatus status = PutMessage(strQueue.GetBuffer(),(const void*)strMsg.GetBuffer(strMsg.GetLength()),strMsg.GetLength(),M_Text);
}

void CTestCecilyMQDlg::OnBnClickedButton10()
{
	// TODO: Add your control notification handler code here
	FILE* file = fopen("C:\\Users\\jd\\Desktop\\nasm-2.12.02-win64\\a.txt", "r");   
	int len;    //获取的长度  
	unsigned char* lpSendBuffer = new unsigned char[Message_Size];
	if (file) {  
		fseek(file, 0, SEEK_END);   //移到尾部  
		len = ftell(file);          //提取长度  
		rewind(file);               //回归原位  
	}
	int nLeftBytes = len;
	CecilyMQMD _mqmd;
	int nSizePackage = len / Message_Size;
	if ( len % Message_Size > 0 )
	{
		nSizePackage += 1;
	}
	_mqmd.body_type = M_Bytes;
	int index = 0;
	long code,reason;
	while ( nLeftBytes > 0 )
	{
		int nReads = nLeftBytes>Message_Size?Message_Size:nLeftBytes;
		fread(lpSendBuffer, sizeof(unsigned char), nReads, file);
		Cecily_MQPut(m_hConn,NULL,&_mqmd,NULL,nReads,lpSendBuffer,&code,&reason);
		nLeftBytes = nLeftBytes - nReads;
	}
	
}

void CTestCecilyMQDlg::OnBnClickedButton11()
{
	// TODO: Add your control notification handler code here
	long code, reason;
	Cecily_MQReconnect(m_hConn,&code,&reason);
	if ( code == Cecily_Success)
	{
		m_RicheditLogs.AddMessageInfo("重新连接至rabbitmq",RGB(0,255,0));
	}
}

void CTestCecilyMQDlg::OnBnClickedButton12()
{
	// TODO: Add your control notification handler code here
	m_bRabbitmq = ((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck();
	if ( m_bRabbitmq )
	{
		Cecily_SetMQ_Type(MQ_Rabbit);
	}
	else
	{
		Cecily_SetMQ_Type(MQ_Active);
	}
}

void CTestCecilyMQDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here   
	((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE);

}

void CTestCecilyMQDlg::OnBnClickedButton13()
{
	// TODO: Add your control notification handler code here
	long code,reason;
	Cecily_MQ_Receive_CB(m_hConn,Active_Recv_CB,&code,&reason,(void*)this);
	if ( code == Cecily_Success )
	{
		m_RicheditLogs.AddMessageInfo("activemq接收回调设置成功",RGB(0,255,0));
	}
}

void CTestCecilyMQDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
}

void CTestCecilyMQDlg::Active_Recv_CB(CecilyMQMD* _mqmd, void* buffer, int nlen,void* user_data)
{
	CTestCecilyMQDlg* lpThis = (CTestCecilyMQDlg*)user_data;
	if ( NULL == lpThis )
	{
		return ;
	}
	char *szBuffer = new char[nlen+1];
	memset(szBuffer,0,nlen+1);
	memcpy(szBuffer,buffer,nlen);
	lpThis->m_RicheditLogs.AddMessageInfo(szBuffer,RGB(0,255,0));
	delete []szBuffer;
	szBuffer = NULL;
}
void CTestCecilyMQDlg::OnBnClickedRadio3()
{
	// TODO: Add your control notification handler code here
	m_bPubSub = false;
}

void CTestCecilyMQDlg::OnBnClickedRadio4()
{
	// TODO: Add your control notification handler code here
	m_bPubSub = true;
}

void CTestCecilyMQDlg::OnBnClickedButton14()
{
	// TODO: Add your control notification handler code here
	CString strQueue, strMsg, strExchange;
	m_edPutQueue.GetWindowText(strQueue);
	m_RicheditWrite.GetWindowText(strMsg);
	m_edExchange.GetWindowText(strExchange);
	if ( m_bPubSub )
	{
		if ( strExchange.IsEmpty() )
		{
			MessageBox("交换器不能为空");
			return ;
		}
	}
	CecilyMQMD _mqmd;
	memcpy(_mqmd.exchange_name,strExchange.GetBuffer(),strExchange.GetLength());
	memcpy(_mqmd.send_queue_name,strQueue.GetBuffer(),strQueue.GetLength());
	_mqmd.body_type = M_Text;
	long code,reason;
	char szTmp[30] = {0};
	sprintf(szTmp,"时间戳:%d",GetTickCount());
	m_RicheditLogs.AddMessageInfo(szTmp,RGB(0,255,0));
	for(int i = 0 ;i < 10000; i++)
	{
		
		if ( !m_bPubSub )
		{
			Cecily_MQPut(m_hConn,NULL,&_mqmd,NULL,strMsg.GetLength(),strMsg.GetBuffer(),&code,&reason);
		}
		else
		{
			Cecily_MQPublish(m_hConn,NULL,&_mqmd,NULL,strMsg.GetLength(),strMsg.GetBuffer(),&code,&reason);
		}
	}
	

	if ( code == Cecily_Success )
	{
		m_RicheditLogs.AddMessageInfo("消息发送成功",RGB(0,255,0));
	}
	else
	{
		m_RicheditLogs.AddMessageInfo("消息发送失败",RGB(255,0,0));
	}
}
