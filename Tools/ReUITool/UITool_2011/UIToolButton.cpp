// UIToolButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "UIToolButton.h"
#include "MainFrm.h"

// CUIToolButton

CUIToolButton::CUIToolButton()
:m_bPushButton(false), m_bStartClicked(false), m_Button(NULL)
{
}

CUIToolButton::~CUIToolButton()
{
}

void CUIToolButton::InitProperty()
{
	CUIToolBaseObject::InitProperty();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("특성"));
	pGroup->SetData(400);

	CMFCPropertyGridProperty* pSub = new CMFCPropertyGridProperty(_T("PushButton"), (_variant_t)m_bPushButton, _T("푸쉬버튼 유/무를 지정합니다."));
	pSub->SetData(401);
	pGroup->AddSubItem(pSub);
	pSub = new CMFCPropertyGridProperty(_T("StartOnClicked"), (_variant_t)m_bStartClicked, _T("시작시 클릭된 상태를 지정합니다."));
	pSub->SetData(402);
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
void CUIToolButton::RemoveProperties()
{
	CUIToolBaseObject::RemoveProperties();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(400);

	if(!pGrid || !pGroup)
		return;

	/*CMFCPropertyGridProperty* pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(401);
	if(pSub != NULL)
		pGroup->RemoveSubItem(pSub);

	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(402);
	if(pSub != NULL)
		pGroup->RemoveSubItem(pSub);*/

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
void CUIToolButton::FindPropertyValueChange()
{
	CUIToolBaseObject::FindPropertyValueChange();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(401);
	if(pFile != NULL)
	{
		pFile->SetValue((_variant_t)m_bPushButton);
	}

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(402);
	if(pFile != NULL)
	{
		pFile->SetValue((_variant_t)m_bStartClicked);
	}
}
void CUIToolButton::PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp)
{
	CUIToolBaseObject::PropertyChange(pStr, pProp);
	
	if(pStr.Compare(TEXT("PushButton")) == 0)
	{
		m_bPushButton = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("StartOnClicked")) == 0)
	{
		m_bStartClicked = _variant_t(pProp->GetValue());
	}

	if(pStr.Compare(TEXT("Default")) == 0)
	{
		CString str = _variant_t(pProp->GetValue());
		m_DefaultImg = GetFilePath(str);
		if(m_Button)
			m_Button->SetBitmaps(m_DefaultImg, pStr, m_Size);
	}
	else if(pStr.Compare(TEXT("Mouse On")) == 0)
	{
		m_MouseOnImg = GetFilePath(_variant_t(pProp->GetValue()));
		if(m_Button)
			m_Button->SetBitmaps(m_MouseOnImg, pStr, m_Size);
	}
	else if(pStr.Compare(TEXT("Click")) == 0)
	{
		m_ClickImg = GetFilePath(_variant_t(pProp->GetValue()));
		if(m_Button)
			m_Button->SetBitmaps(m_ClickImg, pStr, m_Size);
	}
	else if(pStr.Compare(TEXT("Disable")) == 0)
	{
		m_DisableImg = GetFilePath(_variant_t(pProp->GetValue()));
		if(m_Button)
			m_Button->SetBitmaps(m_DisableImg, pStr, m_Size);
	}
}
void CUIToolButton::Render(CBufferDC* pDC)
{
	if(m_Button == NULL)
	{
		m_Button = new CMyButton;
		m_Button->Create(m_strCaption, WS_CHILD | WS_VISIBLE /*| BS_OWNERDRAW*/, CRect(0, 0, 100, 100), pDC->m_pParent, 1231);
		m_Button->MoveWindow(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
		m_pCwndObj= m_Button;
	}
	
	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		(*iter)->Render(pDC);
	}
}

void CUIToolButton::SetFontColor(COLORREF p_color)
{
	CUIToolBaseObject::SetFontColor(p_color);

	if(m_Button)
	{
		m_Button->SetTextColor(m_FontColor);
		m_Button->Invalidate();
	}
}

// CUIToolButton 메시지 처리기입니다.