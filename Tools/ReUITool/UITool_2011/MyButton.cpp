// MyButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "MyButton.h"


// CMyButton

IMPLEMENT_DYNAMIC(CMyButton, CMFCButton)

CMyButton::CMyButton()
:m_LogFont(NULL), m_bgColor(RGB(255,255,255))
{
}

CMyButton::~CMyButton()
{
}


BEGIN_MESSAGE_MAP(CMyButton, CMFCButton)
 	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
 	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CMyButton 메시지 처리기입니다.

void CMyButton::SetBitmaps(CString pImagePath, CString pstr, CPoint pSize)
{
	if(pstr.Compare(_T("Default")) == 0)
	{
		m_bitmapDefault.Destroy();
		m_bitmapDefault.Load(pImagePath);
		if(m_bitmapDefault)
		//	m_bitmapDefault.BitBlt(m_dc->m_hDC, rt.left, rt.top, rt.right, rt.bottom, 0, 0, SRCCOPY);
		SetBitmap((HBITMAP)m_bitmapDefault);

		RedrawWindow();
	}
	else if(pstr.Compare(_T("Mouse On")) == 0)
	{
		m_bitmapMouseOn.Destroy();
		m_bitmapMouseOn.Load(pImagePath);
	}
	else if(pstr.Compare(_T("Click")) == 0)
	{
		m_bitmapClick.Destroy();
		m_bitmapClick.Load(pImagePath);
	}
	else if(pstr.Compare(_T("Disable")) == 0)
	{
		m_bitmapDisable.Destroy();
		m_bitmapDisable.Load(pImagePath);
	}
}

void CMyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CMFCButton::OnLButtonDown(nFlags, point);

	if(IsPressed())
	{
		SetBitmap((HBITMAP)m_bitmapClick);
	}

	RedrawWindow();
}
void CMyButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CMFCButton::OnLButtonUp(nFlags, point);

	if(!IsPressed())
	{
		SetBitmap((HBITMAP)m_bitmapDefault);
	}
		
	RedrawWindow();
}

void CMyButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CMFCButton::OnMouseMove(nFlags, point);
	
	if(IsHighlighted())
	{
		SetBitmap((HBITMAP)m_bitmapMouseOn);
	}
	else
	{
		SetBitmap((HBITMAP)m_bitmapDefault);
	}

	RedrawWindow();
}

HBRUSH CMyButton::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  여기서 DC의 특성을 변경합니다.
	HFONT hFont;
	if(m_LogFont)
	{
		hFont = CreateFontIndirect(m_LogFont);
		pDC->SelectObject(hFont);
	}

	
	//pDC->SetTextColor( m_TextColor );

// 	if(m_dc == NULL)
// 		m_dc = new CDC;
// 
// 	m_dc = pDC;

	
	GetClientRect(rt);

// 	if(m_bitmapDefault)
// 		m_bitmapDefault.BitBlt(pDC->m_hDC, rt.left, rt.top, rt.right, rt.bottom,0,0);
// 
// 	if(m_bitmapMouseOn)
// 		m_bitmapMouseOn.BitBlt(pDC->m_hDC, rt.left, rt.top, rt.right, rt.bottom,0,0);
// 
// 	if(m_bitmapClick)
// 		m_bitmapClick.BitBlt(pDC->m_hDC, rt.left, rt.top, rt.right, rt.bottom,0,0);
// 
// 	if(m_bitmapDisable)
// 		m_bitmapDisable.BitBlt(pDC->m_hDC, rt.left, rt.top, rt.right, rt.bottom,0,0);

	return (HBRUSH)::GetStockObject(NULL_BRUSH);
	// TODO:  부모 처리기가 호출되지 않을 경우 Null이 아닌 브러시를 반환합니다.
	//return NULL;
}
