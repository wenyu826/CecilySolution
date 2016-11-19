// TransparentStatic.cpp : implementation file
//

#include "stdafx.h"
#include "TransparentStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransparentStatic

IMPLEMENT_DYNAMIC(CTransparentStatic, CStatic)

CTransparentStatic::CTransparentStatic()
{
}

CTransparentStatic::~CTransparentStatic()
{
}


BEGIN_MESSAGE_MAP(CTransparentStatic, CStatic)
	//{{AFX_MSG_MAP(CTransparentStatic)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_MESSAGE(WM_SETTEXT,OnSetText)
		ON_WM_CTLCOLOR_REFLECT()
		ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransparentStatic message handlers
LRESULT CTransparentStatic::OnSetText(WPARAM wParam,LPARAM lParam)
{
   LRESULT Result = Default();
   Invalidate();
   UpdateWindow();
   return Result;
}

HBRUSH CTransparentStatic::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
   pDC->SetBkMode(TRANSPARENT);
   return (HBRUSH)GetStockObject(NULL_BRUSH);
}

BOOL CTransparentStatic::OnEraseBkgnd(CDC* pDC)
{
   if (m_Bmp.GetSafeHandle() == NULL)
   {
      CRect Rect;
      GetWindowRect(&Rect);
      CWnd *pParent = GetParent();
      ASSERT(pParent);
      pParent->ScreenToClient(&Rect);  //convert our corrdinates to our parents
      
      //copy what's on the parents at this point
      CDC *pDC = pParent->GetDC();
      CDC MemDC;
      MemDC.CreateCompatibleDC(pDC);
      m_Bmp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
      CBitmap *pOldBmp = MemDC.SelectObject(&m_Bmp);
      MemDC.BitBlt(0,0,Rect.Width(),Rect.Height(),pDC,Rect.left,Rect.top,SRCCOPY);
      MemDC.SelectObject(pOldBmp);
      pParent->ReleaseDC(pDC);
   }
   else //copy what we copied off the parent the first time back onto the parent
   {
      CRect Rect;
      GetClientRect(Rect);
      CDC MemDC;
      MemDC.CreateCompatibleDC(pDC);
      CBitmap *pOldBmp = MemDC.SelectObject(&m_Bmp);
      pDC->BitBlt(0,0,Rect.Width(),Rect.Height(),&MemDC,0,0,SRCCOPY);
      MemDC.SelectObject(pOldBmp);
   }

   return TRUE;
}
