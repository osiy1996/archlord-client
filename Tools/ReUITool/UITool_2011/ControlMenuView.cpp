// ControlMenuView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "ControlMenuView.h"


// CControlMenuView

IMPLEMENT_DYNAMIC(CControlMenuView, CDockablePane)

CControlMenuView::CControlMenuView()
{

}

CControlMenuView::~CControlMenuView()
{
}


BEGIN_MESSAGE_MAP(CControlMenuView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CControlMenuView �޽��� ó�����Դϴ�.



int CControlMenuView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// �並 ����ϴ�.
	//const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	//DWORD dwStyle = GetControlBarStyle();
	//dwStyle &= ~( AFX_CBRS_CLOSE | AFX_CBRS_FLOAT );
	//SetControlBarStyle( dwStyle );

	

	if (!m_CtrlMenu.Create(IDD_CONTROLMENUDLG, this))
	{
		TRACE0("���� �並 ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	AdjustLayout();

	m_CtrlMenu.ShowWindow(SW_SHOW);
	return 0;
}

void CControlMenuView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(m_CtrlMenu.GetSafeHwnd())
	{
		m_CtrlMenu.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
	}
}

void CControlMenuView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	SetWindowPos(NULL, rectClient.left+1, rectClient.top, rectClient.Width(), 70, SWP_NOACTIVATE | SWP_NOZORDER);
}