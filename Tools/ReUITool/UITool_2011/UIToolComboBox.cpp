#include "StdAfx.h"
#include "UIToolComboBox.h"
#include "MainFrm.h"
#include "MFCComboBoxProperty.h"

CUIToolComboBox::CUIToolComboBox(void)
:m_Row(10), m_ComboBox(NULL), m_RowHeigth(20)
{
}

CUIToolComboBox::~CUIToolComboBox(void)
{
}

void CUIToolComboBox::Render(CBufferDC* pDC)
{
	Graphics gdi(pDC->m_hDC); // gdi 객체 선언

	CString str_img = m_DefaultImg;

	if(m_ObjState == EOS_MOUSEON && m_MouseOnImg != "")
		str_img = m_MouseOnImg;
	else if(m_ObjState == EOS_CLICK && m_ClickImg != "")
		str_img = m_ClickImg;
	else if(m_ObjState == EOS_DISABLE && m_DisableImg != "")
		str_img = m_DisableImg;

	SetImage(str_img);

	if(m_ComboBox == NULL)
	{
		m_ComboBox = new CMyComboBox;
		m_ComboBox->Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST , CRect(m_Position.x, m_Position.y, m_Position.x + m_Size.x , m_Position.y + m_Size.y), pDC->m_pParent, 1232);
	
		/*CWnd* p_combo = pDC->m_pParent->GetDlgItem(1232);
 		HWND h_wnd = ::FindWindowEx(p_combo->m_hWnd, NULL, L"Button", NULL);
  		if(h_wnd != NULL)
 		{
 			//((CListBox*)CWnd::FromHandle(h_wnd))->Imag
				//->SetReadOnly(TRUE);
//			((CEdit*)CWnd::FromHandle(h_wnd))->SetReadOnly(TRUE);
			((CButton*)CWnd::FromHandle(h_wnd))->SetWindowTextW(L"minasang");
 		}
		*/
		m_ComboBox->MoveWindow(m_Position.x, m_Position.y, m_Size.x, m_Size.y);
		m_pCwndObj= m_ComboBox;
	}

	for(std::list<CUIToolBaseObject*>::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++)
	{
		(*iter)->Render(pDC);
	}
}
void CUIToolComboBox::InitProperty()
{
	CUIToolBaseObject::InitProperty();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("특성"));
	pGroup->SetData(500);

	static TCHAR BASED_CODE sz_Filter[] = _T("이미지 파일(*.jpg)|*.jpg|이미지 파일(*.bmp)|*.bmp|모든 파일(*.*)|*.*||");
	CMFCPropertyGridFileProperty* pFileSelect = new CMFCPropertyGridFileProperty(_T("ListBoxBGImage"), TRUE, _T(""), 0, 0, sz_Filter, _T("콤보박스의 리스트박스 배경 이미지를 지정합니다."));
	pFileSelect->SetData(501);
	pGroup->AddSubItem(pFileSelect);

	CMFCPropertyGridProperty* pSub = new CMFCComboBoxProperty(_T("AddString"), _T(""));
	//pSub->SetData(501);
	pGroup->AddSubItem(pSub);
	
	pSub = new CMFCPropertyGridProperty(_T("Row"), (_variant_t) m_Row, _T("아이템의 열 수를 지정합니다."));
	pSub->SetData(502);
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

void CUIToolComboBox::RemoveProperties(void)
{
	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	CMFCPropertyGridProperty* pGroup = (CMFCPropertyGridProperty*)pGrid->FindItemByData(500);

	if(!pGrid || !pGroup)
		return;

	if(pGroup != NULL)
		pGrid->DeleteProperty(pGroup);
}
void CUIToolComboBox::FindPropertyValueChange(void)
{
	CUIToolBaseObject::FindPropertyValueChange();

	CMFCPropertyGridCtrl* pGrid = ((CMainFrame*)AfxGetMainWnd())->m_wndProperties.GetPropertyList();

	if(!pGrid)
		return;

	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)pGrid->FindItemByData(502);
	if(pProp != NULL)
	{
		pProp->SetValue(_variant_t(m_Row));
	}

	CMFCPropertyGridFileProperty* pFile = (CMFCPropertyGridFileProperty*)pGrid->FindItemByData(501);
	if(pFile != NULL)
		pFile->SetValue(_variant_t(m_BGImagePath));

}
void CUIToolComboBox::PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp)
{
	CUIToolBaseObject::PropertyChange(pStr, pProp);

	if(pStr.Compare(TEXT("AddString")) == 0)
	{
		if(m_ComboBox != NULL)
		{
			CString str = _variant_t(pProp->GetValue());
			m_ComboBox->AddString(str);
		}
	}
	else if(pStr.Compare(TEXT("Row")) == 0)
	{
		m_Row = _variant_t(pProp->GetValue());
	}
	else if(pStr.Compare(TEXT("ListBoxBGImage")) == 0)
	{
		m_BGImagePath = GetFilePath(_variant_t(pProp->GetValue()));
		if(m_ComboBox)
		{
			m_ComboBox->SetBGImagePath(m_BGImagePath);
			m_ComboBox->Invalidate();
		}
	}
}

void CUIToolComboBox::SetFontColor(COLORREF p_color)
{
	CUIToolBaseObject::SetFontColor(p_color);

	if(m_ComboBox)
	{
		m_ComboBox->SetEditTextColor(m_FontColor);
		m_ComboBox->Invalidate();
	}
}
void CUIToolComboBox::SetImage(CString p_strimgpath)
{
	if(m_ComboBox)
	{
		m_ComboBox->SetImagePath(p_strimgpath);
		m_ComboBox->Invalidate();
	}
}

