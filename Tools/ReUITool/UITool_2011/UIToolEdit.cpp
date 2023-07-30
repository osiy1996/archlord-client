#include "StdAfx.h"
#include "UIToolEdit.h"
#include "MainFrm.h"


CUIToolEdit::CUIToolEdit(void)
:m_Edit(NULL), m_bPassword(false), m_bMultiLine(false), m_bAutoLineFeed(true), m_style(0)
{	
	LOGFONT lf;
	CFont* FONT = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	FONT->GetLogFont(&lf);
	lstrcpy(lf.lfFaceName, _T("맑은 고딕"));
	m_logfont = lf;
	
	m_FontColor = RGB(0, 0, 0);
}

CUIToolEdit::~CUIToolEdit(void)
{
}
void CUIToolEdit::InitProperty()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("특성"));
	pGroup->SetData(300);

	CMFCPropertyGridProperty* pSub = new CMFCPropertyGridProperty(_T("Password"), (_variant_t)m_bPassword, _T("텍스트를 '*'표시 유/무를 지정합니다."));
	pSub->SetData(302);
	pGroup->AddSubItem(pSub);
	
	pSub = new CMFCPropertyGridProperty(_T("최대 글자수"), (_variant_t) m_iMaxLength, _T("텍스트의 최대 글자수를 지정합니다."));
	pSub->SetData(303);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("MultiLine"), (_variant_t) m_bMultiLine, _T("한 줄 이상표시 유/무를 지정합니다."));
	pSub->SetData(304);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("자동 줄바꿈"), (_variant_t) m_bAutoLineFeed, _T("텍스트가 길때 자동으로 줄바꿈 유/무를 지정합니다."));
	pSub->SetData(305);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("Char Filering"), (_variant_t) m_bCharFiltering, _T("첫글자만 대문자로 바꿔줌?! :( 유/무를 지정합니다."));
	pSub->SetData(306);
	pGroup->AddSubItem(pSub);

	pSub = new CMFCPropertyGridProperty(_T("Caption Release"), (_variant_t) m_bEndRelease, _T("엔터 입력후 텍스트 초기화 유/무를 지정합니다."));
	pSub->SetData(307);
	pGroup->AddSubItem(pSub);

	pGrid->AddProperty(pGroup);

	pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(71);
	if(pSub != NULL)
		pSub->Enable(FALSE);
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
void CUIToolEdit::RemoveProperties()
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(300);

	if(!pGrid || !pGroup)
		return;

	for(int i = 302; i < 308; i++)
	{
		CMFCPropertyGridProperty* pSub = (CMFCPropertyGridProperty*)pGrid->FindItemByData(i);
		if(pSub != NULL)
			pGroup->RemoveSubItem(pSub);
	}

	if(pGroup != NULL)
		pGrid->DeleteProperty(pGroup);
}

void CUIToolEdit::FindPropertyValueChange()
{
	CUIToolBaseObject::FindPropertyValueChange();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(302);
	if(pFile != NULL)
	{
		pFile->SetValue((_variant_t)m_bPassword);
	}

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(303);
	if(pFile != NULL)
	{
		pFile->SetValue((_variant_t)m_iMaxLength);
	}
	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(304);
	if(pFile != NULL)
	{
		pFile->SetValue((_variant_t)m_bMultiLine);
	}

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(305);
	if(pFile != NULL)
	{
		pFile->SetValue((_variant_t)m_bAutoLineFeed);
	}

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(306);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_bCharFiltering));

	pFile = (CMFCPropertyGridProperty*)pGrid->FindItemByData(307);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_bEndRelease));
}

void CUIToolEdit::PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp)
{
	CUIToolBaseObject::PropertyChange(pStr, pProp);

	if(pStr.Compare(TEXT("Password")) == 0)
	{
		m_bPassword = _variant_t(pProp->GetValue());

		if(m_bPassword)
			m_Edit->SetPasswordChar('*');
		else
			m_Edit->SetPasswordChar('\0');
	}
	else if(pStr.Compare(TEXT("최대 글자수")) == 0)
	{
		m_iMaxLength = _variant_t(pProp->GetValue());
		
		m_Edit->SetLimitText(m_iMaxLength);
	}
	else if(pStr.Compare(TEXT("MultiLine")) == 0)
	{
		m_bMultiLine = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("자동 줄바꿈")) == 0)
	{
		m_bAutoLineFeed = _variant_t(pProp->GetValue());
		if(m_Edit != NULL)
		{
			m_Edit->DestroyWindow();
			m_Edit = NULL;
		}

		if(m_bAutoLineFeed)
		{
			m_style = ES_MULTILINE | ES_WANTRETURN;
		}
		else
		{
			m_style = 0;
		}
	}
	else if(pStr.Compare(TEXT("Char Filering")) == 0)
	{
		m_bCharFiltering = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("Caption Release")) == 0)
	{
		m_bEndRelease = _variant_t(pProp->GetValue());
	}
}

void CUIToolEdit::Render(CBufferDC* pDC)
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

	if(m_Edit == NULL)
	{
		m_Edit = new CMyEdit;
		m_Edit->Create(WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL  | ES_OEMCONVERT| m_style, CRect(0, 0, 100, 10), pDC->m_pParent, 1230);

		LOGFONT lf;
		CFont* FONT = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		FONT->GetLogFont(&lf);
		lstrcpy(lf.lfFaceName, _T("맑은 고딕"));
		m_logfont = lf;

		m_Edit->SetFont(FONT);
		m_Edit->MoveWindow(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
		m_pCwndObj = m_Edit;
	}	

	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		(*iter)->Render(pDC);
	}
}

void CUIToolEdit::SetFontColor(COLORREF p_color)
{
	CUIToolBaseObject::SetFontColor(p_color);

	if(m_Edit)
	{
		m_Edit->SetTextColor_static(p_color);
		m_Edit->Invalidate();
	}
}
void CUIToolEdit::SetImage(CString p_strimgpath)
{
	if(m_Edit)
	{
		m_Edit->SetImagePath(p_strimgpath);
		m_Edit->Invalidate();
	}
}
