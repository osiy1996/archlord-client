
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "UITool_2011.h"
#include "UITool_2011Doc.h"
#include "MFCButtonProperty.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 콤보 상자를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("속성 콤보 상자를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndObjectCombo.AddString(_T("응용 프로그램"));
	m_wndObjectCombo.AddString(_T("속성 창"));
	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}
	
	SetPropListFont();
	InitPropList();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::InitPropList()
{
	//CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("계층"));
	pGroup1->SetData(1);
	CMFCPropertyGridProperty* pSub = new CMFCPropertyGridProperty(_T("Parent"), (_variant_t) _T(""), _T("누구 자식입니다."));
	pGroup1->AddSubItem(pSub);
	pSub->SetData(12);
	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("모양"));
	pGroup2->SetData(2);
	pSub = new CMFCPropertyGridProperty(_T("ID"), (_variant_t) _T(""), _T("오브젝트 이름"));
	pSub->SetData(22);
	pGroup2->AddSubItem(pSub);
	//pSub = new CMFCPropertyGridProperty(_T("캡션"), (_variant_t) _T(""), _T("창의 제목 표시줄에 표시되는 텍스트를 지정합니다."));
	pSub = new CMFCPropertyGridProperty(_T("Visible"), (_variant_t) true, _T("가시 유/무를 지정합니다."));
	pSub->SetData(23);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("UserInput"), (_variant_t) true, _T("사용자의 키보드 혹은 마우스 입력적용 유/무를 지정합니다."));
	pSub->SetData(24);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("Movable"), (_variant_t) true, _T("이동가능 유/무를 지정합니다."));
	pSub->SetData(25);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("Modal"), (_variant_t) true, _T("다른 오브젝트에는 키 입력이 무시되고 오직 자신만 허용 유/무를 지정합니다."));
	pSub->SetData(26);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("ToolTip"), (_variant_t) _T(""), _T("설명"));
	pSub->SetData(27);
	pGroup2->AddSubItem(pSub);
	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("창 크기"));
	pGroup3->SetData(3);
	pSub = new CMFCPropertyGridProperty(_T("좌표"),_T("Absolute"), _T("Absolute, Relative 중 하나를 지정합니다."));
	pSub->AddOption(_T("Absolute"));
	pSub->AddOption(_T("Relative"));
	pSub->AllowEdit(FALSE);
	pSub->SetData(32);
	pGroup3->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("X"), (_variant_t) 250l, _T("창의 X값을 지정합니다."));
	pSub->EnableSpinControl(TRUE, 50, 300);
	pSub->SetData(33);
	pGroup3->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty( _T("Y"), (_variant_t) 150l, _T("창의 Y값을 지정합니다."));
	pSub->EnableSpinControl(TRUE, 50, 200);
	pSub->SetData(34);
	pGroup3->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("높이"), (_variant_t) 250l, _T("창의 높이를 지정합니다."));
	pSub->EnableSpinControl(TRUE, 50, 300);
	pSub->SetData(35);
	pGroup3->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty( _T("너비"), (_variant_t) 150l, _T("창의 너비를 지정합니다."));
	pSub->EnableSpinControl(TRUE, 50, 200);
	pSub->SetData(36);
	pGroup3->AddSubItem(pSub);
	pGroup3->Expand(TRUE);
	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("배경 이미지"));
	pGroup4->SetData(4);
	COLORREF hColor = RGB(255, 255, 255);//0xffff0000;
	CMFCPropertyGridColorProperty* pColor = new CMFCPropertyGridColorProperty(_T("색상"), hColor, NULL, _T("색상을 지정합니다."));
	pColor->SetData(42);
	pColor->EnableOtherButton(_T("기타..."));	// "기타..." 버튼을 활성화 시킴
	pColor->AllowEdit(TRUE);
	pGroup4->AddSubItem(pColor);
	// File select
	static TCHAR BASED_CODE sz_Filter[] = _T("이미지 파일(*.jpg)|*.jpg|이미지 파일(*.bmp)|*.bmp|모든 파일(*.*)|*.*||");
	CMFCPropertyGridFileProperty* pFileSelect = new CMFCPropertyGridFileProperty(_T("Default"), TRUE, _T(""), 0, 0, sz_Filter, _T("오브젝트의 배경 이미지를 지정합니다."));
	pFileSelect->SetData(43);
	pGroup4->AddSubItem(pFileSelect);
	pFileSelect = new CMFCPropertyGridFileProperty(_T("Mouse On"), TRUE, _T(""), 0, 0, sz_Filter, _T("오브젝트에 마우스가 올라왔을때 이미지를 지정합니다."));
	pFileSelect->SetData(44);
	pGroup4->AddSubItem(pFileSelect);
	pFileSelect = new CMFCPropertyGridFileProperty(_T("Click"), TRUE, _T(""), 0, 0, sz_Filter, _T("오브젝트를 마우스로 클릭했을때 배경이미지를 지정합니다."));
	pFileSelect->SetData(45);
	pGroup4->AddSubItem(pFileSelect);
	pFileSelect = new CMFCPropertyGridFileProperty(_T("Disable"), TRUE, _T(""), 0, 0, sz_Filter, _T("오브젝트가 비활성되었을때 배경이미지를 지정합니다."));
	pFileSelect->SetData(46);
	pGroup4->AddSubItem(pFileSelect);
	pSub = new CMFCPropertyGridProperty(_T("Shrink"), (_variant_t) true, _T("이미지 축소 시에 크기 재설정 유/무를 지정합니다."));
	pSub->SetData(47);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("Clipping"), (_variant_t) true, _T("해상도에 따라 uv값 재설정 유/무를 지정합니다."));
	pSub->SetData(48);
	pGroup2->AddSubItem(pSub);
	m_wndPropList.AddProperty(pGroup4);

	CMFCPropertyGridProperty* pGroup6 = new CMFCPropertyGridProperty(_T("상태별 액션 설정"));
	pGroup6->SetData(6);
	pGroup6->AddSubItem(new CMFCButtonProperty(_T("MessageEdit"), _T("")));
	m_wndPropList.AddProperty(pGroup6);
	pGroup6 = new CMFCPropertyGridProperty(_T("상태별 이미지 설정"));
	pGroup6->SetData(62);
	pGroup6->AddSubItem(new CMFCButtonProperty(_T("StateEdit"), _T("")));
	m_wndPropList.AddProperty(pGroup6);

	CMFCPropertyGridProperty* pGroup7 = new CMFCPropertyGridProperty(_T("텍스트"));
	pGroup7->SetData(7);
	pSub = new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T(""), _T("창의 제목 표시줄에 표시되는 텍스트를 지정합니다."));
	pSub->SetData(71);
	pSub->Enable(FALSE);
	pGroup7->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("글꼴"), _T(""), _T("글꼴을 지정합니다."));
	pSub->AddOption(_T("맑은고딕"));
	pSub->AddOption(_T("ㅁㅇㄻㅇㄹ"));
	pSub->SetData(72);
	pSub->AllowEdit(FALSE);
	pGroup7->AddSubItem(pSub);

	hColor = RGB(0, 0, 0);//0xffff0000;
	pColor = new CMFCPropertyGridColorProperty(_T("텍스트색상"), hColor, NULL, _T("텍스트 색상을 지정합니다."));
	pColor->SetData(73);
	pColor->EnableOtherButton(_T("기타..."));	// "기타..." 버튼을 활성화 시킴
	pColor->AllowEdit(TRUE);
	pColor->Enable(FALSE);
	pGroup7->AddSubItem(pColor);

	pGroup7->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup7);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

LRESULT CPropertiesWnd::OnPropertyChanged( WPARAM wparam, LPARAM lparam)
{
	CMFCPropertyGridProperty *pProp = (CMFCPropertyGridProperty*)lparam;
	if (!pProp) return 0;

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	if(!pDoc || !pDoc->GetPickUpObject()) return 0;

	CString str;
	str.Format(TEXT("%s"), pProp->GetName());
	
	pDoc->GetPickUpObject()->PropertyChange(str, pProp);

	CMainFrame* mFrm = (CMainFrame*)AfxGetMainWnd();
	mFrm->Invalidate();

	return 0;
}
