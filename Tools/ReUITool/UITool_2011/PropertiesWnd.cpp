
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
// CResourceViewBar �޽��� ó����

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

	// �޺� ���ڸ� ����ϴ�.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("�Ӽ� �޺� ���ڸ� ������ ���߽��ϴ�. \n");
		return -1;      // ������ ���߽��ϴ�.
	}

	m_wndObjectCombo.AddString(_T("���� ���α׷�"));
	m_wndObjectCombo.AddString(_T("�Ӽ� â"));
	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	m_wndObjectCombo.SetCurSel(0);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("�Ӽ� ǥ�� ������ ���߽��ϴ�. \n");
		return -1;      // ������ ���߽��ϴ�.
	}
	
	SetPropListFont();
	InitPropList();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ��� */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ��� */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// ��� ����� �θ� �������� �ƴ� �� ��Ʈ���� ���� ����õ˴ϴ�.
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
}

void CPropertiesWnd::InitPropList()
{
	//CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("����"));
	pGroup1->SetData(1);
	CMFCPropertyGridProperty* pSub = new CMFCPropertyGridProperty(_T("Parent"), (_variant_t) _T(""), _T("���� �ڽ��Դϴ�."));
	pGroup1->AddSubItem(pSub);
	pSub->SetData(12);
	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("���"));
	pGroup2->SetData(2);
	pSub = new CMFCPropertyGridProperty(_T("ID"), (_variant_t) _T(""), _T("������Ʈ �̸�"));
	pSub->SetData(22);
	pGroup2->AddSubItem(pSub);
	//pSub = new CMFCPropertyGridProperty(_T("ĸ��"), (_variant_t) _T(""), _T("â�� ���� ǥ���ٿ� ǥ�õǴ� �ؽ�Ʈ�� �����մϴ�."));
	pSub = new CMFCPropertyGridProperty(_T("Visible"), (_variant_t) true, _T("���� ��/���� �����մϴ�."));
	pSub->SetData(23);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("UserInput"), (_variant_t) true, _T("������� Ű���� Ȥ�� ���콺 �Է����� ��/���� �����մϴ�."));
	pSub->SetData(24);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("Movable"), (_variant_t) true, _T("�̵����� ��/���� �����մϴ�."));
	pSub->SetData(25);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("Modal"), (_variant_t) true, _T("�ٸ� ������Ʈ���� Ű �Է��� ���õǰ� ���� �ڽŸ� ��� ��/���� �����մϴ�."));
	pSub->SetData(26);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("ToolTip"), (_variant_t) _T(""), _T("����"));
	pSub->SetData(27);
	pGroup2->AddSubItem(pSub);
	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("â ũ��"));
	pGroup3->SetData(3);
	pSub = new CMFCPropertyGridProperty(_T("��ǥ"),_T("Absolute"), _T("Absolute, Relative �� �ϳ��� �����մϴ�."));
	pSub->AddOption(_T("Absolute"));
	pSub->AddOption(_T("Relative"));
	pSub->AllowEdit(FALSE);
	pSub->SetData(32);
	pGroup3->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("X"), (_variant_t) 250l, _T("â�� X���� �����մϴ�."));
	pSub->EnableSpinControl(TRUE, 50, 300);
	pSub->SetData(33);
	pGroup3->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty( _T("Y"), (_variant_t) 150l, _T("â�� Y���� �����մϴ�."));
	pSub->EnableSpinControl(TRUE, 50, 200);
	pSub->SetData(34);
	pGroup3->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("����"), (_variant_t) 250l, _T("â�� ���̸� �����մϴ�."));
	pSub->EnableSpinControl(TRUE, 50, 300);
	pSub->SetData(35);
	pGroup3->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty( _T("�ʺ�"), (_variant_t) 150l, _T("â�� �ʺ� �����մϴ�."));
	pSub->EnableSpinControl(TRUE, 50, 200);
	pSub->SetData(36);
	pGroup3->AddSubItem(pSub);
	pGroup3->Expand(TRUE);
	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("��� �̹���"));
	pGroup4->SetData(4);
	COLORREF hColor = RGB(255, 255, 255);//0xffff0000;
	CMFCPropertyGridColorProperty* pColor = new CMFCPropertyGridColorProperty(_T("����"), hColor, NULL, _T("������ �����մϴ�."));
	pColor->SetData(42);
	pColor->EnableOtherButton(_T("��Ÿ..."));	// "��Ÿ..." ��ư�� Ȱ��ȭ ��Ŵ
	pColor->AllowEdit(TRUE);
	pGroup4->AddSubItem(pColor);
	// File select
	static TCHAR BASED_CODE sz_Filter[] = _T("�̹��� ����(*.jpg)|*.jpg|�̹��� ����(*.bmp)|*.bmp|��� ����(*.*)|*.*||");
	CMFCPropertyGridFileProperty* pFileSelect = new CMFCPropertyGridFileProperty(_T("Default"), TRUE, _T(""), 0, 0, sz_Filter, _T("������Ʈ�� ��� �̹����� �����մϴ�."));
	pFileSelect->SetData(43);
	pGroup4->AddSubItem(pFileSelect);
	pFileSelect = new CMFCPropertyGridFileProperty(_T("Mouse On"), TRUE, _T(""), 0, 0, sz_Filter, _T("������Ʈ�� ���콺�� �ö������ �̹����� �����մϴ�."));
	pFileSelect->SetData(44);
	pGroup4->AddSubItem(pFileSelect);
	pFileSelect = new CMFCPropertyGridFileProperty(_T("Click"), TRUE, _T(""), 0, 0, sz_Filter, _T("������Ʈ�� ���콺�� Ŭ�������� ����̹����� �����մϴ�."));
	pFileSelect->SetData(45);
	pGroup4->AddSubItem(pFileSelect);
	pFileSelect = new CMFCPropertyGridFileProperty(_T("Disable"), TRUE, _T(""), 0, 0, sz_Filter, _T("������Ʈ�� ��Ȱ���Ǿ����� ����̹����� �����մϴ�."));
	pFileSelect->SetData(46);
	pGroup4->AddSubItem(pFileSelect);
	pSub = new CMFCPropertyGridProperty(_T("Shrink"), (_variant_t) true, _T("�̹��� ��� �ÿ� ũ�� �缳�� ��/���� �����մϴ�."));
	pSub->SetData(47);
	pGroup2->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("Clipping"), (_variant_t) true, _T("�ػ󵵿� ���� uv�� �缳�� ��/���� �����մϴ�."));
	pSub->SetData(48);
	pGroup2->AddSubItem(pSub);
	m_wndPropList.AddProperty(pGroup4);

	CMFCPropertyGridProperty* pGroup6 = new CMFCPropertyGridProperty(_T("���º� �׼� ����"));
	pGroup6->SetData(6);
	pGroup6->AddSubItem(new CMFCButtonProperty(_T("MessageEdit"), _T("")));
	m_wndPropList.AddProperty(pGroup6);
	pGroup6 = new CMFCPropertyGridProperty(_T("���º� �̹��� ����"));
	pGroup6->SetData(62);
	pGroup6->AddSubItem(new CMFCButtonProperty(_T("StateEdit"), _T("")));
	m_wndPropList.AddProperty(pGroup6);

	CMFCPropertyGridProperty* pGroup7 = new CMFCPropertyGridProperty(_T("�ؽ�Ʈ"));
	pGroup7->SetData(7);
	pSub = new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T(""), _T("â�� ���� ǥ���ٿ� ǥ�õǴ� �ؽ�Ʈ�� �����մϴ�."));
	pSub->SetData(71);
	pSub->Enable(FALSE);
	pGroup7->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("�۲�"), _T(""), _T("�۲��� �����մϴ�."));
	pSub->AddOption(_T("�������"));
	pSub->AddOption(_T("����������"));
	pSub->SetData(72);
	pSub->AllowEdit(FALSE);
	pGroup7->AddSubItem(pSub);

	hColor = RGB(0, 0, 0);//0xffff0000;
	pColor = new CMFCPropertyGridColorProperty(_T("�ؽ�Ʈ����"), hColor, NULL, _T("�ؽ�Ʈ ������ �����մϴ�."));
	pColor->SetData(73);
	pColor->EnableOtherButton(_T("��Ÿ..."));	// "��Ÿ..." ��ư�� Ȱ��ȭ ��Ŵ
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
