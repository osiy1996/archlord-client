// MyEdit.cpp : ���� �����Դϴ�.
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



// CMyEdit �޽��� ó�����Դϴ�.
// BOOL CMyEdit::OnEraseBkgnd(CDC* pDC)
// {
// 	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
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
	// TODO:  �θ� ó���Ⱑ ȣ����� ���� ��� Null�� �ƴ� �귯�ø� ��ȯ�մϴ�.
	//return NULL;
}

// void CMyEdit::OnEnUpdate()
// {
// 	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
// 	// CEdit::OnInitDialog() �Լ��� ������ 
// 	//�Ͽ�, IParam ����ũ�� OR �����Ͽ� ������ ENM_SCROLL �÷��׸� �����Ͽ� ��Ʈ�ѿ� EM_SETEVENTMASK �޽����� ������ ������
// 	// ���� ��Ʈ���� �ٲ� �ؽ�Ʈ�� ǥ���Ϸ��� ���� ��Ÿ���ϴ�.
// 
// 	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	RedrawWindow();
// }