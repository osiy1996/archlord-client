// CTransparentListBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TransparentListBox.h"
#include ".\transparentlistbox.h"

#include "PatchClientDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransparentListBox
const int	Offset		= 6;
const int	FirstBlank	= 5;

CTransparentListBox::CTransparentListBox()
:  m_ItemHeight(20),
m_HasBackGround(FALSE),
m_Color(RGB(0,0,0)),
m_SelColor(RGB(255,0,0)),
m_ShadowColor(RGB(0,0,0)),
m_Shadow(FALSE),
m_ShadowOffset(1)
{
	m_SelColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_Color = GetSysColor(COLOR_WINDOWTEXT);
}

CTransparentListBox::~CTransparentListBox()
{
	m_arSubString.clear();
}


BEGIN_MESSAGE_MAP(CTransparentListBox, CListBox)
	//{{AFX_MSG_MAP(CTransparentListBox)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_CONTROL_REFLECT_EX(LBN_SELCHANGE, OnLbnSelchange)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_SIZE()
//	ON_WM_MOUSEMOVE()
//ON_WM_TIMER()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTransparentListBox message handlers

void CTransparentListBox::PreSubclassWindow() 
{
	CListBox::PreSubclassWindow();

	ModifyStyle(0,SS_NOTIFY|LBS_NOSEL);

	CWnd *pParent = GetParent();
	if (pParent)
	{
		CRect Rect;
		GetClientRect(&Rect);
		ClientToScreen(&Rect);
		pParent->ScreenToClient(&Rect);
		CDC *pDC = pParent->GetDC();
		m_Bmp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
	}
}

void CTransparentListBox::SetFont(int nPointSize, CString FaceName,COLORREF Color,COLORREF SelColor,BOOL Shadow,int SOffset,COLORREF ShadowColor)
{
	m_Shadow = Shadow;
	m_ShadowColor = ShadowColor;
	m_ShadowOffset = SOffset;
	m_Color = Color;
	m_SelColor = SelColor;

	m_PointSize = nPointSize;

	CDC *DC = GetDC();
	m_Font.DeleteObject();
	m_Font.CreateFont(nPointSize,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,FaceName);
	if (GetCount())
	{
		CRect Rect;
		Rect.SetRectEmpty();

		CFont *oldFont = DC->SelectObject(&m_Font);
		CString Text;
		GetText(0,Text);
		DC->DrawText(Text,&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
		m_ItemHeight = static_cast<long>(Rect.Height());
		DC->SelectObject(oldFont);
	}
	ReleaseDC(DC);

}

void CTransparentListBox::SetColor(COLORREF Color,COLORREF SelColor,COLORREF ShadowColor)
{
	m_ShadowColor = ShadowColor;
	m_Color = Color;
	m_SelColor = SelColor;
}

void CTransparentListBox::ResetContent(BOOL bInvalidate)
{
	Default();
	if ( bInvalidate ) 
	{
		Invalidate();
		UpdateWindow();
	}
}

void CTransparentListBox::AddSubString(CString Text)
{
	m_arSubString.push_back(Text);
}

int CTransparentListBox::AddString(CString Text,DWORD ItemData,BOOL Enabled)
{
	if (!GetCount())
	{
		CDC *DC = GetDC();
		CRect Rect;
		Rect.SetRectEmpty();

		CFont *oldFont = DC->SelectObject(GetFont());
		DC->DrawText(Text,&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
		m_ItemHeight = static_cast<long>(Rect.Height());
		DC->SelectObject(oldFont);
		ReleaseDC(DC);
	}

	int Index = CListBox::AddString(Text);
	CListBox::SetItemData(Index,ItemData);
	CListBox::SetItemHeight(Index, m_bmpItemBase.GetHeight() + Offset);

	return Index;
}

int CTransparentListBox::InsertString(int Index,CString Text, DWORD ItemData,BOOL Enabled)
{
	if (!GetCount())
	{
		CDC *DC = GetDC();
		CRect Rect;
		Rect.SetRectEmpty();

		CFont *oldFont = DC->SelectObject(GetFont());
		DC->DrawText(Text,&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
		m_ItemHeight = static_cast<long>(Rect.Height());
		DC->SelectObject(oldFont);
		ReleaseDC(DC);
	}

	Index = CListBox::InsertString(Index,Text);
	CListBox::SetItemData(Index,ItemData);
	return Index;
}



BOOL CTransparentListBox::OnEraseBkgnd(CDC* pDC) 
{
	if (!m_HasBackGround)
	{
		CWnd *pParent = GetParent();
		if (pParent)
		{
			CRect Rect;
			GetClientRect(&Rect);
			ClientToScreen(&Rect);
			pParent->ScreenToClient(&Rect);
			CDC *pDC = pParent->GetDC();
			CDC memdc;
			memdc.CreateCompatibleDC(pDC);
			CBitmap *oldbmp = memdc.SelectObject(&m_Bmp);
			memdc.BitBlt(0,0,Rect.Width(),Rect.Height(),pDC,Rect.left,Rect.top,SRCCOPY);
			memdc.SelectObject(oldbmp);
			m_HasBackGround = TRUE;
			pParent->ReleaseDC(pDC);
		}
	}

	return TRUE;

}

CFont *CTransparentListBox::GetFont()
{
	if (m_Font.m_hObject == NULL)
	{
		return CListBox::GetFont();
	}

	return &m_Font;
}

void CTransparentListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
}


void CTransparentListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CDC *DC = GetDC();
	CRect Rect;
	Rect.SetRectEmpty();

	CFont *oldFont = DC->SelectObject(GetFont());
	DC->DrawText("XYZ",&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
	m_ItemHeight = lpMeasureItemStruct->itemHeight = static_cast<long>(Rect.Height());
	DC->SelectObject(oldFont);
	ReleaseDC(DC);
}


void CTransparentListBox::DrawItem(CDC &Dc,int Index,CRect &Rect,BOOL Selected)
{
	if (Index == LB_ERR || Index >= GetCount())
		return;

	Dc.SetBkMode(TRANSPARENT);

	CDC memdc;
	memdc.CreateCompatibleDC(&Dc);

	CFont *pFont = GetFont();
	CFont *oldFont = Dc.SelectObject(pFont);
	CBitmap *oldbmp = memdc.SelectObject(&m_Bmp);
	Dc.BitBlt(Rect.left,Rect.top,Rect.Width(),Rect.Height(),&memdc,Rect.left,Rect.top,SRCCOPY);
	CString Text, subText;
	CListBox::GetText(Index,Text);

	//. substring ó��.
	int size = (int) m_arSubString.size();
	if(size)
		subText = m_arSubString[Index];

	m_bmpItemBase.Draw(&Dc, Selected ? 1 : 0, Rect.left ,Rect.top, 0, NULL);

	CRect		rcFont;

	rcFont.left		= Rect.left;
	rcFont.top		= Rect.top;
	rcFont.right	= m_bmpItemBase.GetSliceWidth();
	rcFont.bottom	= m_bmpItemBase.GetHeight() + Rect.top;

	rcFont.left += FirstBlank;

	if (m_Shadow)
	{
		if (IsWindowEnabled())
		{
			Dc.SetTextColor(m_ShadowColor);
		}
		else
		{
			Dc.SetTextColor(RGB(255,255,255));
		}
		rcFont.OffsetRect(m_ShadowOffset,m_ShadowOffset);
		Dc.DrawText(Text,rcFont,DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		rcFont.OffsetRect(-m_ShadowOffset,-m_ShadowOffset);
	}

	if (IsWindowEnabled())
	{
		if (Selected)
		{
			Dc.SetTextColor(m_SelColor);
		}
		else
		{
			Dc.SetTextColor(m_Color);
		}
	}
	else
	{
		Dc.SetTextColor(RGB(140,140,140));
	}

	Dc.DrawText(Text,rcFont,DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	if(subText.GetLength())
	{
		rcFont.left -= FirstBlank;
		Dc.DrawText(subText,rcFont,DT_RIGHT | DT_VCENTER | DT_SINGLELINE );
	}

	Dc.SelectObject(oldFont);
	memdc.SelectObject(oldbmp);
	
}

void CTransparentListBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect Rect;
	GetClientRect(&Rect);

	int Width = Rect.Width();
	int Height = Rect.Height();

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap MemBmp;
	MemBmp.CreateCompatibleBitmap(&dc,Width,Height);

	CBitmap *pOldMemBmp = MemDC.SelectObject(&MemBmp);

	CDC MemDC2;
	MemDC2.CreateCompatibleDC(&dc);
	CBitmap *pOldbmp = MemDC2.SelectObject(&m_Bmp);
	MemDC.BitBlt(0,0,Width,Height,&MemDC2,0,0,SRCCOPY);
	MemDC2.SelectObject(pOldbmp);

	int			ImageHeight = m_bmpItemBase.GetHeight();

	Rect.top = 0;
	Rect.left = 0;
	Rect.bottom = Rect.top + ImageHeight;
	Rect.right = Width;

	int size = GetCount();
	int sel = GetCurSel();
	for (int i = GetTopIndex(); i < size && Rect.top <= Height;++i)
	{
		DrawItem(MemDC,i,Rect,i == sel/*GetSel(i)*/);
		Rect.OffsetRect(0, ImageHeight + Offset);//GetItemHeight(i));
	}

	dc.BitBlt(0,0,Width,Height,&MemDC,0,0,SRCCOPY);

	MemDC.SelectObject(pOldMemBmp);
}

void CTransparentListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetRedraw(FALSE);
	CListBox::OnVScroll(nSBCode,nPos,pScrollBar);
	SetRedraw(TRUE);

	RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
}

BOOL CTransparentListBox::OnLbnSelchange()
{
	//. ���⼭ ������Ʈ�� ó��~
	CWnd* pParent = GetParent();
	CPatchClientDlg*	pParentDlg = (CPatchClientDlg*)pParent;

	CString Text;
	int nCurSel = GetCurSel();

	if (nCurSel < 0)
		return FALSE;

	CListBox::GetText(nCurSel, Text);

	pParentDlg->SetArea( (LPSTR)(LPCTSTR)Text );
	pParentDlg->PatchThreadStart();

	RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
	return FALSE;
}

int CTransparentListBox::SetTopIndex(int Index)
{
	SetRedraw(FALSE);
	int Ret = CListBox::SetTopIndex(Index);
	SetRedraw(TRUE);
	RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
	return Ret;
}

int CTransparentListBox::ScrollUp(int Lines)
{
	int Index = GetTopIndex();
	if (Index-Lines < 0)
	{
		Index = Lines;
	}
	return SetTopIndex(Index-Lines);
}

int CTransparentListBox::ScrollDown(int Lines)
{
	int Index = GetTopIndex();
	if (Index+Lines > GetCount()-1)
	{
		Index = GetCount()-Lines;
	}
	return SetTopIndex(Index+Lines);
}

void CTransparentListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetRedraw(FALSE);
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	SetRedraw(TRUE);
}

void CTransparentListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SetRedraw(FALSE);
	CListBox::OnChar(nChar, nRepCnt, nFlags);
	SetRedraw(TRUE);
}

void CTransparentListBox::OnMove(int x, int y)
{
	CListBox::OnMove(x, y);

	ShowWindow(SW_HIDE);
	m_HasBackGround = FALSE;
	Invalidate();
	UpdateWindow();
	ShowWindow(SW_SHOW);

}

void CTransparentListBox::OnSize(UINT nType, int cx, int cy)
{
	CListBox::OnSize(nType, cx, cy);

	CWnd *pParent = GetParent();
	if (pParent)
	{
		m_Bmp.DeleteObject();
		CRect Rect;
		GetClientRect(&Rect);
		ClientToScreen(&Rect);
		pParent->ScreenToClient(&Rect);
		CDC *pDC = pParent->GetDC();
		m_Bmp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
	}
	ShowWindow(SW_HIDE);
	m_HasBackGround = FALSE;
	Invalidate();
	UpdateWindow();
	ShowWindow(SW_SHOW);
}