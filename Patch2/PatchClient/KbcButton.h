#if !defined(AFX_KBCBUTTON_H__41D0AB53_20C4_4079_B3E2_3E8B6B337CD5__INCLUDED_)
#define AFX_KBCBUTTON_H__41D0AB53_20C4_4079_B3E2_3E8B6B337CD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KbcButton.h : header file
//

#include "KbcBmp.h"

/////////////////////////////////////////////////////////////////////////////
// CKbcButton window
// CKbcButton ����
// DDX_Control �� ����ؼ� ������ �ϴ� ������ CKbcButton���� �������ش���.. 
// SetBmpButtonImage("test.bmp",RGB(0,0,255)); �� ȣ���ϸ� �ȴ�....
//
// ���� - ������(kbckbc@postech.ac.kr | http://kbckbc.com)
// Ȥ�ö� ����Ͻô� �в��� Ŭ���� �̸��̳��� �������ֽñ� �ٶ��ϴ�.
// �׷��� ���� ����� ��� �Ŷ��� �������ֽñ� �ٶ��ϴ�.

class CKbcButton : public CButton
{
// Construction
public:
	CKbcButton();

	// ���� ��Ʈ���� �׷��ִ� ����
	CKbcBmp			m_bmpImage;
	UINT			m_nMask;
	char			m_strButtonText[255];

	// ��Ʈ������ �غ����
	void			SetBmpButtonImage(CString,UINT);
	void			SetBmpButtonImage(UINT32,UINT);

	// ��ư�� Ȱ��ȭ/��Ȱ��ȭ ��Ų��.
	void			SetButtonEnable();
	void			SetButtonDisable();

	// ���� �޼����� ���δ�.
	void			SetToolTipText(CString);

	// ��ư������ Ŀ���� �ٲ۴�.
	void			SetCursor(HCURSOR);

	//. 2006. 4. 11. nonstopdj
	//. fake toggle btn.
	BOOL			m_bToggle;
	BOOL			m_bClicked;
	BOOL			m_bLButtonDown;

protected:
	// Ȱ��ȭ���� ��Ȱ��ȭ���� ��� �ִ� ����
	// �ʱⰪ�� FALSE�� Ȱ��ȭ�� ���̴�.
	BOOL			m_bDisable;


	// Ŀ���� ��ư���� �ִ��� ������ �Ǵ�
	// WM_MOUSEHOVER �� WM_MOUSELEAVE �޼����� �ѹ����� �����ֱ� ���� ����.
	BOOL			m_bCursorOnButton;


	// ���� ��ư�� �Ϲ� �����̸� �Ϲ��� �׸��� �׸���.
	// DrawItem �ʿ��� ����.
	BOOL			m_bHover;


	// m_rectButton �� ��ư�� ũ�� 0,0,10,10 �̷� ���� ���̴�.
	// m_rectButtonPos �� ��ư�� �ö� �ִ� ���̾� �α׿��� ��ġ���̴�.
	CRect			m_rectButton;
	CRect			m_rectButtonPos;

	// ���� �޼����� ���� ��~~
	CToolTipCtrl	m_ToolTip;


	// ����ڰ� Ŀ���� �����Ҽ��� �ִµ� �׶� Ŀ���� ��� �ִ� ����
	HCURSOR			m_hCursor;	

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKbcButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKbcButton();
	void SetButtonText( char *pstrText );
	char *GetButtonText();

	// Generated message map functions
protected:
	//{{AFX_MSG(CKbcButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KBCBUTTON_H__41D0AB53_20C4_4079_B3E2_3E8B6B337CD5__INCLUDED_)
