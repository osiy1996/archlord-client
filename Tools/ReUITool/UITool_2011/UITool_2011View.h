
// UITool_2011View.h : CUITool_2011View 클래스의 인터페이스
//


#pragma once
#include "UITool_2011Doc.h"

enum E_MouseState
{
	EMS_NORMAL = 0,
	EMS_MOUSEON,
	EMS_CLICK,
	EMS_RESIZE,
	EMS_CREATEDRAG,
	EMS_SIZEDRAG,
	EMS_DRAG,
};

class CUITool_2011View : public CView
{
protected: // serialization에서만 만들어집니다.
	CUITool_2011View();
	DECLARE_DYNCREATE(CUITool_2011View)

// 특성입니다.
public:
	CUITool_2011Doc* GetDocument() const;

// 작업입니다.
public:
	CPoint m_StartMousePoint;
	CPoint m_EndMousePoint;

	E_MousePosition m_eMousePosition;
	E_MouseState m_eMouseState;

	CUIToolBaseObject* m_MouseOnObj;

	HCURSOR m_hCurrentCursor;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CUITool_2011View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
public:
	BOOL SetMouseCursor(CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // UITool_2011View.cpp의 디버그 버전
inline CUITool_2011Doc* CUITool_2011View::GetDocument() const
   { return reinterpret_cast<CUITool_2011Doc*>(m_pDocument); }
#endif

