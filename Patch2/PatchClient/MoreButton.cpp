// MoreButton.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "MoreButton.h"


// CMoreButton

IMPLEMENT_DYNAMIC(CMoreButton, CButton)

CMoreButton::CMoreButton()
{
	m_bCursorOnButton	= FALSE;

}

CMoreButton::~CMoreButton()
{
}


BEGIN_MESSAGE_MAP(CMoreButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CMoreButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	RECT		rect , rect2;
	POINT		point;
	CBrush		brush;
	CDC* pDC	= CDC::FromHandle (lpDrawItemStruct->hDC);

	GetClientRect( &rect );
	GetWindowRect( &rect2 );
	GetCursorPos( &point );

	pDC->SetBkMode( TRANSPARENT );
	pDC->SelectObject( GetStockObject(NULL_BRUSH) );
	pDC->SelectObject( GetStockObject(BLACK_PEN)	);
	//pDC->Rectangle( &rect );
	pDC->MoveTo( rect.left , rect.bottom - 1   );
	pDC->LineTo( rect.right , rect.bottom - 1);

	CPen		pen;

	

	if( PtInRect( &rect2 , point ) )
	{
		pen.CreatePen( PS_SOLID , 1 , RGB( 150 , 150 , 150 ) );
		pDC->SelectObject( pen	);
		pDC->MoveTo( rect.left , rect.bottom -1  );
		pDC->LineTo( rect.right , rect.bottom - 1);
	}
}

void CMoreButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if( m_bCursorOnButton == FALSE )
	{
		TRACKMOUSEEVENT tme;
		ZeroMemory(&tme,sizeof(TRACKMOUSEEVENT));
		tme.cbSize			= sizeof(tme);
		tme.hwndTrack		= m_hWnd;
		tme.dwFlags			= TME_LEAVE|TME_HOVER;
		tme.dwHoverTime		= 1;
		m_bCursorOnButton	= _TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}

BOOL CMoreButton::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	RECT rect;
	GetClientRect( &rect );
	pDC->SetBkMode( TRANSPARENT );
	pDC->SelectObject( GetStockObject(NULL_BRUSH) );
	pDC->SelectObject( GetStockObject(NULL_PEN ) );
	pDC->Rectangle( &rect );
	return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}

void CMoreButton::PreSubclassWindow()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	ModifyStyle( 0x0FL , BS_OWNERDRAW , SWP_FRAMECHANGED );

	CButton::PreSubclassWindow();
}

LRESULT CMoreButton::OnMouseHover( WPARAM wParam , LPARAM lParam )
{
	Invalidate();
	return 0L;
}

LRESULT CMoreButton::OnMouseLeave( WPARAM wPAram , LPARAM lParam )
{
	m_bCursorOnButton	=	FALSE;
	Invalidate();
	return 0L;
}
