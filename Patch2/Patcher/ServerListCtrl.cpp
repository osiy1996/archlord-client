// ServerListCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "ServerListCtrl.h"


// CServerListCtrl

IMPLEMENT_DYNAMIC(CServerListCtrl, CListCtrl)

CServerListCtrl::CServerListCtrl()
{

}

CServerListCtrl::~CServerListCtrl()
{
}


BEGIN_MESSAGE_MAP(CServerListCtrl, CListCtrl)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// CServerListCtrl �޽��� ó�����Դϴ�.



void CServerListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CMenu	menu;
	CMenu	*pMenu;
	POINT	point;

	menu.LoadMenu( IDR_LISTMENU );
	pMenu	=	menu.GetSubMenu(0);

	GetCursorPos( &point );

	pMenu->TrackPopupMenu( TPM_LEFTBUTTON , point.x , point.y , GetParent()  );
	
}
