#include "StdAfx.h"
#include "MyButton.h"

BEGIN_MESSAGE_MAP(CMyButton, CButton)

	//{{AFX_MSG_MAP(CMyButton)

	ON_WM_MOUSEMOVE()

	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)//鼠标离开

	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)//鼠标悬挂

	//}}AFX_MSG_MAP

END_MESSAGE_MAP()



CMyButton::CMyButton(void)

{

	m_TempColor=m_TextColor=m_DownColor = m_UpColor = RGB(0,0,0);

	//m_Mytip.Create(this);

	m_bTracking=false;

	_isCustom=false;



	upBorderColor=RGB(172,172,172);

	upBgColorR0=upBgColorG0=upBgColorB0=240;

	upBgColorR1=upBgColorG1=upBgColorB1=229;



	hoverBorderColor=RGB(126,180,234);

	hoverBgColorR0=236;

	hoverBgColorG0=244;

	hoverBgColorB0=252;

	hoverBgColorR1=220;

	hoverBgColorG1=236;

	hoverBgColorB1=252;



	downBorderColor=RGB(86,157,229);

	downBgColorR0=218;

	downBgColorG0=232;

	downBgColorB0=252;

	downBgColorR1=196;

	downBgColorG1=224;

	downBgColorB1=252;



	focusBorderColor=RGB(51,153,255);



	diableBorderColor=RGB(217,217,217);

	disableBgColor=RGB(239,239,239);

}



CMyButton::~CMyButton(void){

}



BOOL CMyButton::Attach(int nID, CWnd* pParent){

	cWndParent=pParent->GetDlgItem(nID);

	cWndParent->ModifyStyle(0,BS_OWNERDRAW,0);

	if (!SubclassDlgItem(nID, pParent))

		return FALSE;

	return TRUE;

}



void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct){

	// TODO: Add your code to draw the specified item

	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);//得到绘制的设备环境CDC

	VERIFY(lpDrawItemStruct->CtlType==ODT_BUTTON);

	const int bufSize = 512;

	TCHAR buffer[bufSize];

	GetWindowText(buffer, bufSize);//获取按钮上的文字

	int size=strlen(buffer);   //得到文字长度

	SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);   //设置背景透明

	if (lpDrawItemStruct->itemState &ODS_SELECTED){  //当按下按钮时的处理

		CRect rect;

		GetClientRect(&rect);//获取按钮区域

		for(int i=0;i<rect.Height();i++){//处理渐变，如果是垂直方向则rect.Height(),下同 

			int r,g,b;

			r = downBgColorR0 + (i * (downBgColorR1-downBgColorR0) / rect.Height()); 

			g = downBgColorG0 + (i * (downBgColorG1-downBgColorG0) / rect.Height()); 

			b = downBgColorB0 + (i * (downBgColorB1-downBgColorB0) / rect.Height()); 

			dc.FillSolidRect(0,i,rect.Width(),1,RGB(r,g,b));//填充一条直线

		}

		DrawText(lpDrawItemStruct->hDC,buffer,size,&lpDrawItemStruct->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);//绘制文字

		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);//设置背景透明

		CBrush brush1(downBorderColor);//边框颜色

		dc.SetTextColor(RGB(0,0,0));//设置文字颜色

		dc.FrameRect(&(lpDrawItemStruct->rcItem),&brush1);//

	}

	else if(lpDrawItemStruct->itemState &ODS_FOCUS||lpDrawItemStruct->itemState &ODS_CHECKED){//获得焦点处理，同上

		CRect rect;

		GetClientRect(&rect); 

		for(int i=0;i<rect.Height();i++){    //如果是垂直方向则rect.Height(),下同 

			int r,g,b; 

			r = upBgColorR0 + (i * (upBgColorR1-upBgColorR0) / rect.Height()); 

			g = upBgColorG0 + (i * (upBgColorG1-upBgColorG0) / rect.Height()); 

			b = upBgColorB0 + (i * (upBgColorB1-upBgColorB0) / rect.Height()); 

			dc.FillSolidRect(0,i,rect.Width(),1,RGB(r,g,b));

		}                           
		DrawText(lpDrawItemStruct->hDC,buffer,size,&lpDrawItemStruct->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);                           

		DrawFocusRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);

		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);

		CBrush brush1(focusBorderColor);//边框颜色

		dc.SetTextColor(RGB(0,0,0));//设置文字颜色

		dc.FrameRect(&lpDrawItemStruct->rcItem,&brush1);//

	}

	else if(lpDrawItemStruct->itemState &ODS_DISABLED){//禁用处理，同上

		CBrush brush(disableBgColor);//背景颜色

		dc.FillRect(&(lpDrawItemStruct->rcItem),&brush);

		dc.SetTextColor(RGB(131,131,131));//设置文字颜色

		DrawText(lpDrawItemStruct->hDC,buffer,size,&lpDrawItemStruct->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);

		DrawFocusRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem);

		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);

		CBrush brush1(diableBorderColor);//边框颜色

		dc.FrameRect(&lpDrawItemStruct->rcItem,&brush1);//

	}

	else{//当按钮不操作或者弹起处理

		CRect rect; 

		GetClientRect(&rect); 

		for(int i=0;i<rect.Height();i++){    //如果是垂直方向则rect.Height(),下同 

			int r,g,b; 

			r = upBgColorR0 + (i * (upBgColorR1-upBgColorR0) / rect.Height()); 

			g = upBgColorG0 + (i * (upBgColorG1-upBgColorG0) / rect.Height()); 

			b = upBgColorB0 + (i * (upBgColorB1-upBgColorB0) / rect.Height()); 

			dc.FillSolidRect(0,i,rect.Width(),1,RGB(r,g,b));

		}

		DrawText(lpDrawItemStruct->hDC,buffer,size,&lpDrawItemStruct->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);   

		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);

		CBrush brush1(upBorderColor);//边框颜色

		dc.SetTextColor(RGB(0,0,0));//设置文字颜色

		dc.FrameRect(&lpDrawItemStruct->rcItem,&brush1);//

	}

	dc.Detach();

}



void CMyButton::OnMouseMove(UINT nFlags, CPoint point)

{

	if (!m_bTracking)

	{

		TRACKMOUSEEVENT tme;

		tme.cbSize = sizeof(tme);

		tme.hwndTrack = m_hWnd;

		tme.dwFlags = TME_LEAVE | TME_HOVER;

		tme.dwHoverTime = 10;

		m_bTracking = _TrackMouseEvent(&tme);

	}

}

LRESULT CMyButton::OnMouseLeave(WPARAM wParam, LPARAM lParam){

	m_bTracking=false;

	if(!_isCustom){                  

		upBorderColor=RGB(172,172,172);;

		upBgColorR0=upBgColorG0=upBgColorB0=240;

		upBgColorR1=upBgColorG1=upBgColorB1=229;

	}

	else{

		m_UpColor=m_TempColor;

		//m_TextColor=RGB(255,255,0);

	}

	Invalidate(); //重绘按钮

	return 0;

}



LRESULT CMyButton::OnMouseHover(WPARAM wParam, LPARAM lParam){

	if(!_isCustom){

		upBorderColor=hoverBorderColor;

		//      upBgColor=hoverBgColor;               

		upBgColorR0=hoverBgColorR0;

		upBgColorG0=hoverBgColorG0;

		upBgColorB0=hoverBgColorB0;

		upBgColorR1=hoverBgColorR1;

		upBgColorG1=hoverBgColorG1;

		upBgColorB1=hoverBgColorB1;

	}

	else{

		//m_TextColor=RGB(255,0,255);

		m_UpColor=m_DownColor;

	}

	Invalidate(); //重绘按钮

	return 0;

}