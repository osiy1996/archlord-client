// MyEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "MyEdit.h"


// CMyEdit

IMPLEMENT_DYNAMIC(CMyEdit, CWnd)

CMyEdit::CMyEdit()
:m_TextColor(RGB(0, 0, 0)), m_bgImagePath(_T("")), m_LogFont(NULL), m_bgColor(RGB(255,255,255))
{
}

CMyEdit::~CMyEdit()
{
	
}


BEGIN_MESSAGE_MAP(CMyEdit, CWnd)
	//ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	//ON_CONTROL_REFLECT(EN_UPDATE, &CMyEdit::OnEnUpdate)
END_MESSAGE_MAP()



// CMyEdit 메시지 처리기입니다.
// BOOL CMyEdit::OnEraseBkgnd(CDC* pDC)
// {
// 	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
// 	//if(m_bkImagePath.Compare(_T("")) != 0)
// 	//{
// // 		CBitmap* Btmap;
// // 		CImage tmpImg;
// // 		tmpImg.Load(/*m_bkImagePath*/_T("C:\\Users\\Public\\Pictures\\Sample Pictures\\Chrysanthemum.jpg"));
// // 		Btmap = (CBitmap*)(CBitmap::FromHandle(tmpImg.Detach()));
// // 		if(!tmpImg.IsNull())
// // 			tmpImg.Destroy();
// // 
// // 		CBrush Brush;
// // 		Brush.CreatePatternBrush(Btmap);
// // 		CRect rect;
// // 		GetClientRect(&rect);
// // 		pDC->FillRect(&rect, &Brush);
// 
// 		return TRUE;
// 	//}
// 	//else
// 	//	return CEdit::OnEraseBkgnd(pDC);
// }

HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  여기서 DC의 특성을 변경합니다.
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
		CBrush Brush;
		Brush.CreateSolidBrush(m_bgColor);
		pDC->FillRect(&rect, &Brush);
	}

	return (HBRUSH)::GetStockObject(NULL_BRUSH);
	// TODO:  부모 처리기가 호출되지 않을 경우 Null이 아닌 브러시를 반환합니다.
	//return NULL;
}

// void CMyEdit::OnEnUpdate()
// {
// 	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
// 	// CEdit::OnInitDialog() 함수를 재지정 
// 	//하여, IParam 마스크에 OR 연산하여 설정된 ENM_SCROLL 플래그를 지정하여 컨트롤에 EM_SETEVENTMASK 메시지를 보내지 않으면
// 	// 편집 컨트롤이 바뀐 텍스트를 표시하려고 함을 나타냅니다.
// 
// 	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	RedrawWindow();
// }