#include "StdAfx.h"
#include "UIToolListBox.h"
#include "MainFrm.h"
#include "MFCComboBoxProperty.h"

CUIToolListBox::CUIToolListBox(void)
:m_ListBox(NULL)
{
}

CUIToolListBox::~CUIToolListBox(void)
{
}
void CUIToolListBox::InitProperty()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("특성"));
	pGroup->SetData(600);

	CMFCPropertyGridProperty* pSub = new CMFCComboBoxProperty(_T("AddString"), _T(""));
	//pSub->SetData(501);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("Row"), (_variant_t) m_iRow, _T("아이템의 열 수를 지정합니다."));
	pSub->SetData(601);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("Column"), (_variant_t) m_iColumn, _T("아이템의 행 수를 지정합니다."));
	pSub->SetData(602);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("시작위치"), _T("Top"), _T("리스트가 쌓이는 순서를 Top/Bottom으로 지정합니다."));
	pSub->AddOption(_T("Top"));
	pSub->AddOption(_T("Bottom"));
	pSub->SetData(603);
	pSub->AllowEdit();
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("선택아이템"), (_variant_t) m_iColumn, _T("아이템의 행 수를 지정합니다."));
	pSub->SetData(604);
	pGroup->AddSubItem(pSub);

	pGrid->AddProperty(pGroup);

	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(71);
	if(pSub != NULL)
		pSub->Enable(TRUE);
	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(72);
	if(pSub != NULL)
		pSub->Enable(TRUE);
	CMFCPropertyGridColorProperty* pColor = (CMFCPropertyGridColorProperty*)pGrid->FindItemByData(73);
	if(pColor != NULL)
		pColor->Enable(TRUE);
	pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(7);
	if(pGroup != NULL)
		pGroup->Expand(TRUE);
}
void CUIToolListBox::RemoveProperties()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(600);

	if(!pGrid || !pGroup)
		return;

	if(pGroup != NULL)
		pGrid->DeleteProperty(pGroup);

	CMFCPropertyGridProperty* pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(71);
	if(pSub != NULL)
		pSub->Enable(FALSE);
	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(72);
	if(pSub != NULL)
		pSub->Enable(FALSE);
	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(73);
	if(pSub != NULL)
		pSub->Enable(FALSE);

	pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(7);
	if(pGroup != NULL)
		pGroup->Expand(FALSE);
}

void CUIToolListBox::FindPropertyValueChange()
{
	CUIToolBaseObject::FindPropertyValueChange();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(601);
	if(pProp != NULL)
	{
		//pProp->SetValue();
	}

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(602);
	if(pProp != NULL)
	{
		//pProp->SetValue();
	}

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(603);
	//if(pProp != NULL)
		//pProp->SetValue(_variant_t(m_bNumberComma));

	pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(604);
	//if(pProp != NULL)
		//pProp->SetValue(_variant_t(m_bShadow));
}

void CUIToolListBox::PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp)
{
	CUIToolBaseObject::PropertyChange(pStr, pProp);

	if(pStr.Compare(TEXT("Row")) == 0)
	{
		
	}
	else if(pStr.Compare(TEXT("Column")) == 0)
	{
		
	}
	else if(pStr.Compare(TEXT("시작위치")) == 0)
	{
		CString str = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("선택아이템")) == 0)
	{
		
	}
}

void CUIToolListBox::Render(CBufferDC* pDC)
{
	//if(!m_bVisible)
	//	return;

	Graphics gdi(pDC->m_hDC); // gdi 객체 선언

	CString str_img = m_DefaultImg;

	if(m_ObjState == EOS_MOUSEON && m_MouseOnImg != "")
		str_img = m_MouseOnImg;
	else if(m_ObjState == EOS_CLICK && m_ClickImg != "")
		str_img = m_ClickImg;
	else if(m_ObjState == EOS_DISABLE && m_DisableImg != "")
		str_img = m_DisableImg;

	SetImage(str_img);

	if(m_ListBox == NULL)
	{
		m_ListBox = new CMyListBox;
		m_ListBox->Create(WS_CHILD | WS_VISIBLE | LBS_USETABSTOPS | LBS_DISABLENOSCROLL, CRect(0, 0, 100, 300), pDC->m_pParent, 12336);

		LOGFONT lf;
		CFont* FONT = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		FONT->GetLogFont(&lf);
		lstrcpy(lf.lfFaceName, _T("맑은 고딕"));
		m_logfont = lf;

		m_ListBox->SetFont(FONT);

		m_ListBox->MoveWindow(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
		m_pCwndObj = m_ListBox;
	}

	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		(*iter)->Render(pDC);
	}
}

void CUIToolListBox::SetFontColor(COLORREF p_color)
{
	CUIToolBaseObject::SetFontColor(p_color);

	if(m_ListBox)
	{
		m_ListBox->SetTextColor_static(p_color);
		m_ListBox->Invalidate();
	}
}
void CUIToolListBox::SetImage(CString p_strimgpath)
{
	if(m_ListBox)
	{
		m_ListBox->SetImagePath(p_strimgpath);
		m_ListBox->Invalidate();
	}
}
