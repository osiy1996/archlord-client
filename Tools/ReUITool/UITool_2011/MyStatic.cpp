// MyStatic.cpp : 구현 파일입니다.
//
//#pragma comment(lib, "msing32.lib")

#include "stdafx.h"
#include "UITool_2011.h"
#include "MyStatic.h"


// CMyStatic

IMPLEMENT_DYNAMIC(CMyStatic, CStatic)

CMyStatic::CMyStatic()
:m_TextColor(RGB(0, 0, 0)), m_bgImagePath(_T("")), m_LogFont(NULL), m_bgColor(RGB(255,255,255))
{
}

CMyStatic::~CMyStatic()
{
}

BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CMyStatic::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	HFONT hFont;
	if(m_LogFont)
	{
		hFont = CreateFontIndirect(m_LogFont);
		pDC->SelectObject(hFont);
	}

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor( m_TextColor );

 	CRect rect;
 	GetClientRect(&rect);
 
 	Graphics gdi(pDC->m_hDC);
 	if(m_bgImagePath.Compare(_T("")) != 0)
 	{
		Image img(m_bgImagePath);
 		gdi.DrawImage(&img, rect.left, rect.top, rect.right, rect.bottom);
	}
 	else
 	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	// TODO:  여기서 DC의 특성을 변경합니다.
	return NULL;
	// TODO:  부모 처리기가 호출되지 않을 경우 Null이 아닌 브러시를 반환합니다.
}