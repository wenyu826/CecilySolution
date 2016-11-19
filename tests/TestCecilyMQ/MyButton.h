#pragma once

#include "afxwin.h"

class CMyButton : public CButton

{

	//DECLARE_DYNAMIC(CMyButton)

private:

	bool _isCustom;

	COLORREF upBorderColor,downBorderColor,focusBorderColor,hoverBorderColor,diableBorderColor; //���嵯����¡����㡢�����롢���õı߿���ɫ

	COLORREF disableBgColor;//������õı�����ɫ

	int hoverBgColorR0,hoverBgColorG0,hoverBgColorB0,hoverBgColorR1,hoverBgColorG1,hoverBgColorB1;//�ֱ�����������ϱ�RGB���±�RGB��ɫ��ʵ�ֽ��䣩

	int upBgColorR0,upBgColorG0,upBgColorB0,upBgColorR1,upBgColorG1,upBgColorB1;

	int downBgColorR0,downBgColorG0,downBgColorB0,downBgColorR1,downBgColorG1,downBgColorB1;//ͬ�ϣ���������ɫ

public:

	CMyButton();

	virtual ~CMyButton();

	void SetDownColor(COLORREF color); //����Button Down�ı�����ɫ

	void SetUpColor(COLORREF color);//���õ�����ɫ

	void SetFont(CFont* font);//��������

	void SetFontColor(COLORREF color);//����������ɫ

	CWnd* cWndParent;

	BOOL Attach(int nID, CWnd* pParent);//������ԴID�����Զ��尴ť

protected:

	//�������صĺ���

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);//�ذ����ƺ���

public:

	WNDPROC OldWndProc;

	//CtoolTipCtrl m_Mytip;

	BOOL m_bTracking;

	LRESULT   CALLBACK WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	//������ɫ�ֱ�Ϊ���֣�Button Down�ı�����ɫ��Button Up�ı�����ɫ

	COLORREF m_TextColor, m_DownColor, m_UpColor,m_TempColor;

protected:

	// Generated message map functions

	//{{AFX_MSG(CMyButton)

	//}}AFX_MSG

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);//����뿪�¼�

	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);//�������¼�

	DECLARE_MESSAGE_MAP()

};