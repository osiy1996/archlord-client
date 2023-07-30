#include "StdAfx.h"
#include "UIToolUI.h"
#include "MainFrm.h"

CUIToolUI::CUIToolUI(void)
:m_strType(_T("Normal")), m_bLogIn(false), m_bMainUI(false), m_bAutoClose(false), m_bTransperent(false), m_bResoultion_Move(false), m_bResoultion_size(false), m_bInitStatus(false)
{
	m_PositionType = _T("Relative");
}

CUIToolUI::~CUIToolUI(void)
{
}
void CUIToolUI::InitProperty()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;
	
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("특성"));
	pGroup1->SetData(100);

	CMFCPropertyGridProperty* pSub = new CMFCPropertyGridProperty(_T("UI Type"),_T("Normal"), _T("normal, Popup Vertically, Popup Horizontally 중 하나를 지정합니다."));
 	pSub->AddOption(_T("Normal"));
 	pSub->AddOption(_T("Popup Veritcally"));
	pSub->AddOption(_T("Popup Horizontally"));
 	pSub->AllowEdit(FALSE);
 	pSub->SetData(101);
 	pGroup1->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("MainUI"), (_variant_t) m_bMainUI, _T("항상 표시되는 UI를 지정합니다."));
	pSub->SetData(102);
	pGroup1->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("AutoClose"), (_variant_t) m_bAutoClose, _T("Main UI를 제외한 것들 중 esc키 입력시 자동으로 닫히는 UI를 지정합니다."));
	pSub->SetData(103);
	pGroup1->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("Transperent"), (_variant_t) m_bTransperent, _T("부모 UI에 포함된 컨트롤은 선택이 가능하고 현 UI는 선택되지 않도록 지정합니다."));
	pSub->SetData(104);
	pGroup1->AddSubItem(pSub);
	
	pSub = new CMFCPropertyGridProperty(_T("ResoultionSize"), (_variant_t) m_bResoultion_size, _T("해상도에 따른 UI확대 변경을 지정합니다."));
	pSub->SetData(105);
	pGroup1->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("ResoultionMove"), (_variant_t) m_bResoultion_Move, _T("해상도에 따라 각 컨트롤을 자동으로 위치이동 할지를 지정합니다."));
	pSub->SetData(106);
	pGroup1->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("InitStatus"), (_variant_t) m_bInitStatus, _T("UI 열 때 모든 컨트롤을 초기화 시킨다."));
	pSub->SetData(107);
	pGroup1->AddSubItem(pSub);

	pGrid->AddProperty(pGroup1);
}
void CUIToolUI::RemoveProperties()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(100);

	if(!pGrid || !pGroup)
		return;

	/*for(int i = 101; i < 110; i++)
	{
		CMFCPropertyGridProperty* pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(i);
		if(pSub != NULL)
			pGroup->RemoveSubItem(pSub);
	}*/

	if(pGroup != NULL)
		pGrid->DeleteProperty(pGroup);
}

void CUIToolUI::FindPropertyValueChange()
{
	CUIToolBaseObject::FindPropertyValueChange();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;
	
	CMFCPropertyGridProperty* pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(101);
	if(pFile != NULL)
	{
		//LPCTSTR str = pFile->GetOption(m_eType);
		pFile->SetValue(m_strType);
	}
	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(102);
	if(pFile != NULL)
		pFile->SetValue((_variant_t)m_bMainUI);

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(103);
	if(pFile != NULL)
		pFile->SetValue((_variant_t)m_bAutoClose);

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(104);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_bTransperent));

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(105);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_bResoultion_size));

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(106);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_bResoultion_Move));

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(107);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_bInitStatus));

}

void CUIToolUI::PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp)
{
	CUIToolBaseObject::PropertyChange(pStr, pProp);

	if(pStr.Compare(TEXT("UI Type")) == 0)
	{
		/*pStr = _variant_t(pProp->GetValue());
		if(pStr.Compare(_T("Normal")) == 0)
			m_eType = EU_Normal;
		else if(pStr.Compare(_T("Popup Veritcally")) == 0)
			m_eType = EU_Popup_v;
		else
			m_eType = EU_Popup_H;*/
		m_strType = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("MainUI")) == 0)
	{
		m_bMainUI = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("AutoClose")) == 0)
	{
		m_bAutoClose = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("Transperent")) == 0)
	{
		m_bTransperent = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("ResoultionSize")) == 0)
	{
		m_bResoultion_size = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("ResoultionMove")) == 0)
	{
		m_bResoultion_Move = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("InitStatus")) == 0)
	{
		m_bInitStatus = _variant_t(pProp->GetValue());
	}
}