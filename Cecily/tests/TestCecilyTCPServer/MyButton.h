#pragma once

#include "afxwin.h"

class CMyButton : public CButton

{

	//DECLARE_DYNAMIC(CMyButton)

private:

	bool _isCustom;

	COLORREF upBorderColor,downBorderColor,focusBorderColor,hoverBorderColor,diableBorderColor; //定义弹起、铵下、焦点、鼠标进入、禁用的边框颜色

	COLORREF disableBgColor;//定义禁用的背景颜色

	int hoverBgColorR0,hoverBgColorG0,hoverBgColorB0,hoverBgColorR1,hoverBgColorG1,hoverBgColorB1;//分别定义鼠标进入的上边RGB和下边RGB颜色（实现渐变）

	int upBgColorR0,upBgColorG0,upBgColorB0,upBgColorR1,upBgColorG1,upBgColorB1;

	int downBgColorR0,downBgColorG0,downBgColorB0,downBgColorR1,downBgColorG1,downBgColorB1;//同上，鼠标铵下颜色

public:

	CMyButton();

	virtual ~CMyButton();

	void SetDownColor(COLORREF color); //设置Button Down的背景颜色

	void SetUpColor(COLORREF color);//设置弹起颜色

	void SetFont(CFont* font);//设置字体

	void SetFontColor(COLORREF color);//设置字体颜色

	CWnd* cWndParent;

	BOOL Attach(int nID, CWnd* pParent);//根据资源ID创建自定义按钮

protected:

	//必需重载的函数

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);//重案绘制函数

public:

	WNDPROC OldWndProc;

	//CtoolTipCtrl m_Mytip;

	BOOL m_bTracking;

	LRESULT   CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	//三种颜色分别为文字，Button Down的背景颜色，Button Up的背景颜色

	COLORREF m_TextColor, m_DownColor, m_UpColor,m_TempColor;

protected:

	// Generated message map functions

	//{{AFX_MSG(CMyButton)

	//}}AFX_MSG

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);//鼠标离开事件

	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);//鼠标进入事件

	DECLARE_MESSAGE_MAP()

};