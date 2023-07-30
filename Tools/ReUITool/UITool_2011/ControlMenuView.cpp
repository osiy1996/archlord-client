// ControlMenuView.cpp : 구현 파일입니다.
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



// CControlMenuView 메시지 처리기입니다.



int CControlMenuView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	//const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	//DWORD dwStyle = GetControlBarStyle();
	//dwStyle &= ~( AFX_CBRS_CLOSE | AFX_CBRS_FLOAT );
	//SetControlBarStyle( dwStyle );

	

	if (!m_CtrlMenu.Create(IDD_CONTROLMENUDLG, this))
	{
		TRACE0("파일 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	AdjustLayout();

	m_CtrlMenu.ShowWindow(SW_SHOW);
	return 0;
}

void CControlMenuView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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