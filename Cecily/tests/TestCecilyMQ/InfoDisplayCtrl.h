#ifndef C_Info_Display_Ctrl_H
#define C_Info_Display_Ctrl_H
#include <afxcmn.h>
#include <RichOle.h>
#include <string>
using namespace std;

class CInfoDisplayCtrl :public CRichEditCtrl
{
public:
	CInfoDisplayCtrl(void);
	virtual ~CInfoDisplayCtrl(void);

public:
	BOOL InitCtrl();
	void AddMessageInfo(const char* lpText,COLORREF clTextColor = RGB(0xff,0xff,0xff), bool bLink = false, UINT nSize = 0, const char* lpFontName = NULL);

public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void DoCecilyAll();
	afx_msg void DoSelectAll();
	afx_msg void DoCopy();
	afx_msg void DoCecilySelected();
	afx_msg void OnCommandRange(UINT nCommandID);
	DECLARE_MESSAGE_MAP()

	void DoSwitchShowStatus();
	void DoSwitchScrollStatus();


private:
	BOOL			m_bScroll;
	BOOL			m_bMenu;
	BOOL			m_bCanInsert;
	HCURSOR			m_hCursor;
	std::string		m_strFont;
	IRichEditOle	*m_lpRichEditOLE;
	COLORREF		m_bkColor;
	int				m_nKeepLines;
};

#endif

