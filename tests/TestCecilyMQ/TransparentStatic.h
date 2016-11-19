#if !defined(AFX_TRANSPARENTSTATIC_H__D75D406C_DCDD_4FFA_80BB_D4203039BCDB__INCLUDED_)
#define AFX_TRANSPARENTSTATIC_H__D75D406C_DCDD_4FFA_80BB_D4203039BCDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransparentStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransparentStatic window

class CTransparentStatic : public CStatic
{
DECLARE_DYNAMIC(CTransparentStatic)
// Construction
public:
	CTransparentStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentStatic)

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTransparentStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransparentStatic)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg LRESULT OnSetText(WPARAM,LPARAM);
   afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_Bmp;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPARENTSTATIC2_H__2BA20A5D_1669_4A9D_B718_28515A217794__INCLUDED_)
