// NewStatic.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "NewStatic.h"


// CNewStatic

IMPLEMENT_DYNAMIC(CNewStatic, CStatic)

CNewStatic::CNewStatic()
{
	m_TextData	=	"";
}

CNewStatic::~CNewStatic()
{
}


BEGIN_MESSAGE_MAP(CNewStatic, CStatic)
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CNewStatic::OnPaint( VOID )
{
	CPaintDC dc(this);

	UINT		nFormat = 0;
	CFont*		OldFont	= NULL;
	CRect		rect;
	ZeroMemory( rect , sizeof(rect) );
	GetClientRect( &rect );
	
	OldFont	=	(CFont*)dc.SelectObject( GetFont() );	//	��Ʈ ����

	dc.SetBkMode( TRANSPARENT );			//	��� ����
	dc.SetTextColor( RGB(255,255,255) );	//	���ڻ� ȭ��Ʈ
	
	DrawTextW( dc.m_hDC , m_TextData , m_TextData.GetLength() , &rect , DT_CENTER | DT_SINGLELINE  );

	dc.SelectObject( OldFont );
}

BOOL CNewStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	return CStatic::OnEraseBkgnd(pDC);
}
