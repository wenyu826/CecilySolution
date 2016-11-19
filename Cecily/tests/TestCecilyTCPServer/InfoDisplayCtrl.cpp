#include "stdafx.h"
#include "InfoDisplayCtrl.h"

#define InfoDisplayID		1000

#define	ID_DO_COPY						WM_USER + InfoDisplayID + 1
#define ID_DO_CLEARSELECTED				WM_USER + InfoDisplayID + 2
#define ID_DO_CLEARALL					WM_USER + InfoDisplayID + 3
#define ID_DO_SELECTALL					WM_USER + InfoDisplayID + 4
#define ID_DO_SWITCH_SHOW_STATUS		WM_USER + InfoDisplayID + 5
#define ID_DO_AUTO_SCROLL				WM_USER + InfoDisplayID + 6



CInfoDisplayCtrl::CInfoDisplayCtrl(void)
{
	m_bScroll	=	TRUE;
	m_bMenu		=	TRUE;
	m_bCanInsert=	TRUE;
	m_hCursor = LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));
	m_strFont	=	TEXT("宋体");
	m_lpRichEditOLE = NULL;
	m_nKeepLines = 5000;
	
}


CInfoDisplayCtrl::~CInfoDisplayCtrl(void)
{
	if ( NULL != m_lpRichEditOLE )
	{
		m_lpRichEditOLE->Release();
		m_lpRichEditOLE = NULL;
	}
}

BEGIN_MESSAGE_MAP(CInfoDisplayCtrl,CRichEditCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	//ON_NOTIFY_REFLECT(EN_LINK,OnEnLink)
	ON_COMMAND_RANGE(ID_DO_COPY,ID_DO_AUTO_SCROLL,OnCommandRange)
END_MESSAGE_MAP()

BOOL CInfoDisplayCtrl::InitCtrl()
{
	if ( NULL == m_lpRichEditOLE )
	{
		m_lpRichEditOLE = GetIRichEditOle();
	}
	return (m_lpRichEditOLE != NULL);
}

void CInfoDisplayCtrl::AddMessageInfo(const char* lpText,COLORREF clTextColor, bool bLink , UINT nSize , const char* lpFontName )
{
	CHARRANGE	charRange;
	GetSel(charRange);
	int  nLines = GetLineCount();
	

	CHARFORMAT2		charFormat;
	ZeroMemory(&charFormat,sizeof(CHARFORMAT2));
	charFormat.cbSize	= sizeof(charFormat);
	charFormat.dwMask	= CFM_COLOR | CFM_FACE | CFM_SIZE;
	charFormat.crTextColor = clTextColor;
	charFormat.yHeight	= nSize * 20 + 180;
	if ( NULL != lpFontName )
	{
		lstrcpy(charFormat.szFaceName,lpFontName);
	}
	else
	{
		lstrcpy(charFormat.szFaceName,TEXT("宋体"));
	}
	if ( bLink )
	{
		charFormat.dwMask |= CFM_LINK;
		charFormat.dwEffects |= CFM_LINK;
	}
	if ( nLines > m_nKeepLines )
	{
		DoCecilyAll();
	}
	if ( m_bCanInsert )
	{
		SetSel(GetWindowTextLength(),GetWindowTextLength());
		SetSelectionCharFormat(charFormat);
		int nSize = strlen(lpText);
		char *lpBuffer= NULL;
		string strMsg = lpText;
		int nPos = strMsg.rfind("\n");
		if ( nPos == string::npos )
		{
			strMsg += "\n";
		}
		else
		{
			 if ( nPos != strMsg.length() -1 )
			 {
				 strMsg +="\n";
			 }
		}
		
		ReplaceSel(strMsg.c_str());
	}
	if ( !m_bScroll )
	{
		SetSel(charRange);
	}else
	{
		SendMessage(WM_VSCROLL,SB_BOTTOM,0);
	}

}

void CInfoDisplayCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ( m_bMenu )
	{
		SetFocus();
		CMenu MenuPop;
		MenuPop.CreatePopupMenu();
		MenuPop.AppendMenu(0,ID_DO_COPY,TEXT("复制"));
		MenuPop.AppendMenu(0,ID_DO_SELECTALL,"全选");
		MenuPop.AppendMenu(0,ID_DO_CLEARSELECTED,"删除所选");
		MenuPop.AppendMenu(0,ID_DO_CLEARALL,"全部清除");
		MenuPop.AppendMenu(0,MF_SEPARATOR);
		MenuPop.AppendMenu(0,ID_DO_SWITCH_SHOW_STATUS,"显示");
		MenuPop.AppendMenu(0,ID_DO_AUTO_SCROLL,"自动滚动");
		UINT nSel = (GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED;
		MenuPop.EnableMenuItem(ID_DO_COPY,MF_BYCOMMAND|nSel);
		MenuPop.EnableMenuItem(ID_DO_SELECTALL,MF_BYCOMMAND|MF_ENABLED);
		MenuPop.EnableMenuItem(ID_DO_CLEARSELECTED,MF_BYCOMMAND|nSel);
		MenuPop.EnableMenuItem(ID_DO_CLEARALL,MF_BYCOMMAND|MF_ENABLED);
		MenuPop.EnableMenuItem(ID_DO_SWITCH_SHOW_STATUS,MF_BYCOMMAND|MF_ENABLED);
		MenuPop.EnableMenuItem(ID_DO_AUTO_SCROLL,MF_BYCOMMAND|MF_ENABLED);

		MenuPop.CheckMenuItem(ID_DO_SWITCH_SHOW_STATUS,(m_bCanInsert?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
		MenuPop.CheckMenuItem(ID_DO_AUTO_SCROLL,(m_bScroll?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
		ClientToScreen(&point);
		MenuPop.TrackPopupMenu(TPM_RIGHTBUTTON,point.x,point.y,this);
		MenuPop.DestroyMenu();
	}
}

BOOL CInfoDisplayCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( nHitTest == HTCAPTION )
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CRichEditCtrl::OnSetCursor(pWnd,nHitTest,message);
}

void CInfoDisplayCtrl::OnDestroy()
{
	CRichEditCtrl::OnDestroy();
	if( NULL != m_lpRichEditOLE )
	{
		m_lpRichEditOLE->Release();
		m_lpRichEditOLE = NULL;
	}
}

void CInfoDisplayCtrl::DoCecilyAll()
{
	SetSel(0,GetWindowTextLength());
	ReplaceSel(NULL,1);
}

void CInfoDisplayCtrl::DoSelectAll()
{
	SetSel(0,-1);
}

void CInfoDisplayCtrl::DoCopy()
{
	Copy();
}

void CInfoDisplayCtrl::DoCecilySelected()
{
	ReplaceSel("");
}

void CInfoDisplayCtrl::DoSwitchShowStatus()
{
	m_bCanInsert = !m_bCanInsert;
}

void CInfoDisplayCtrl::DoSwitchScrollStatus()
{
	m_bScroll = !m_bScroll;
}

void CInfoDisplayCtrl::OnCommandRange(UINT nCommandID)
{
	switch ( nCommandID )
	{
	case ID_DO_COPY:
		DoCopy();
		break;
	case ID_DO_SELECTALL:
		DoSelectAll();
		break;
	case ID_DO_CLEARSELECTED:
		DoCecilySelected();
		break;
	case ID_DO_CLEARALL:
		DoCecilyAll();
		break;
	case ID_DO_SWITCH_SHOW_STATUS:
		DoSwitchShowStatus();
		break;
	case ID_DO_AUTO_SCROLL:
		DoSwitchScrollStatus();
		break;
	default:
		break;
	}
}