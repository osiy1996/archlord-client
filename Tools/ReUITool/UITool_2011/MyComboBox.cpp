// MyComboBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "MyComboBox.h"


// CMyComboBox

IMPLEMENT_DYNAMIC(CMyComboBox, CComboBox)

CMyComboBox::CMyComboBox()
:m_BGImagePath(_T("")), m_ButtonImagePath(_T(""))
{
	m_EditfontColor = RGB(0, 0, 0);
}

CMyComboBox::~CMyComboBox()
{
}


BEGIN_MESSAGE_MAP(CMyComboBox, CComboBox)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()



// CMyComboBox 메시지 처리기입니다.
HBRUSH CMyComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if(nCtlColor == CTLCOLOR_LISTBOX)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_EditfontColor);

		if(m_BGImagePath.Compare(_T("")) != 0)
		{
			CBitmap* Btmap;
			CImage tmpImg;
			tmpImg.Load(m_BGImagePath);

			Btmap = (CBitmap*)(CBitmap::FromHandle(tmpImg.Detach()));

			if(!tmpImg.IsNull())
				tmpImg.Destroy();

			m_BkBrush.DeleteObject();
			m_BkBrush.CreatePatternBrush(Btmap);

			hbr = (HBRUSH)m_BkBrush;
		}
	}
	
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

BOOL CMyComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	CDC* pDC = CDC::FromHandle((HDC)wParam);

	if(message == WM_CTLCOLOREDIT)
	{
		CRect rect;
		GetClientRect(rect);

		Graphics gdi(pDC->m_hDC);
		if(m_ButtonImagePath.Compare(_T("")) != 0)
		{
			Image img(m_ButtonImagePath);
			gdi.DrawImage(&img, rect.left, rect.top, rect.right, rect.bottom);
		}
		
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor( m_EditfontColor );

		return TRUE;
	}

	return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
}
