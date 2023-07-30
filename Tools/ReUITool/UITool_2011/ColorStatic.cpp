// ColorStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "ColorStatic.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CWnd)

CColorStatic::CColorStatic()
:m_LogFont(NULL)
{
	m_RGB = RGB( 0, 0, 0 );
	m_bkRGB = RGB(255,255,0);
}

CColorStatic::~CColorStatic()
{ 

}


BEGIN_MESSAGE_MAP(CColorStatic, CWnd)
END_MESSAGE_MAP()



// CColorStatic 메시지 처리기입니다.
void CColorStatic::SetColor(COLORREF p_rgb, COLORREF p_bkrgb)
{
	m_RGB = p_rgb;
	m_bkRGB = p_bkrgb;
}

BOOL CColorStatic::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//if( message == WM_CTLCOLORSTATIC )
	//{
		CDC* pDC = CDC::FromHandle((HDC)wParam);
		
		HFONT hFont;
		if(m_LogFont)
		{
			hFont = CreateFontIndirect(m_LogFont);
			pDC->SelectObject(hFont);
		}
		
		pDC->SetBkMode(TRANSPARENT);
		//pDC->SetBkColor(m_bkRGB);
		pDC->SetTextColor( m_RGB );
		return TRUE;
	//}

	return CStatic::OnChildNotify(message, wParam, lParam, pLResult);
}

