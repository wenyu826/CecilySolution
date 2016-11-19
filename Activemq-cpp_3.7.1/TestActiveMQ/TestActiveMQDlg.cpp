
// TestActiveMQDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestActiveMQ.h"
#include "TestActiveMQDlg.h"
#include "ClearCMSTemplate.h"

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


// CTestActiveMQDlg dialog




CTestActiveMQDlg::CTestActiveMQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestActiveMQDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_lpMQManager = new ClearActiveMQManager();
}

void CTestActiveMQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestActiveMQDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTestActiveMQDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestActiveMQDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CTestActiveMQDlg message handlers

BOOL CTestActiveMQDlg::OnInitDialog()
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestActiveMQDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestActiveMQDlg::OnPaint()
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
HCURSOR CTestActiveMQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestActiveMQDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if ( NULL == m_lpMQManager )
	{
		return ;
	}
	ClearCMSTemplate* lpTemPlate = new ClearCMSTemplate();
	m_lpMQManager->InitActiveMQ();
	std::string brokerURI =
		"failover:(tcp://localhost:10086"
		//        "?wireFormat=openwire"
		//        "&transport.useInactivityMonitor=false"
		//        "&connection.alwaysSyncSend=true"
		//        "&connection.useAsyncSend=true"
		//        "?transport.commandTracingEnabled=true"
		//        "&transport.tcpTracingEnabled=true"
		//        "&wireFormat.tightEncodingEnabled=true"
		")";
	ActiveMQConnectionFactory* connectionFactory =
		new ActiveMQConnectionFactory( "failover://(tcp://127.0.0.1:100861)" );
// 	auto_ptr<ConnectionFactory> connectionFactory(
// 		ConnectionFactory::createCMSConnectionFactory(brokerURI));
	lpTemPlate->setConnectionFactory(connectionFactory);
	lpTemPlate->setDefaultDestinationName("CMSTemplateExamples");
	lpTemPlate->setPubSubDomain(true);
	lpTemPlate->Async_Receive("CMSTemplateExamples");
// 	TextMessageCreator* textMessageCreator = new TextMessageCreator();
// 	lpTemPlate->send(textMessageCreator);
return;

	// Create a Connection
	cms::Connection* connection = connectionFactory->createConnection();
	connection->start();
	//connection->setExceptionListener(this);

	// Create a Session
	cms:Session*session = connection->createSession(Session::AUTO_ACKNOWLEDGE);

	// Create the destination (Topic or Queue)
	cms::Destination* destination = session->createTopic("TEST.FOO");
	// Create a MessageConsumer from the Session to the Topic or Queue
	cms::MessageConsumer* consumer = session->createConsumer(destination);

	//consumer->setMessageListener(this);
}

void CTestActiveMQDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	m_lpMQManager->InitActiveMQ();
	std::string brokerURI =
		"failover:(tcp://localhost:10086?soConnectTimeout=2000)";
		//        "?wireFormat=openwire"
		//        "&transport.useInactivityMonitor=false"
		//        "&connection.alwaysSyncSend=true"
		//        "&connection.useAsyncSend=true"
		//        "?transport.commandTracingEnabled=true"
		//        "&transport.tcpTracingEnabled=true"
		//        "&wireFormat.tightEncodingEnabled=true"   soConnectTimeout
		
	m_lpMQManager->ConnectToMQ(brokerURI.c_str());
}
