// MFCComboBoxProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "MFCComboBoxProperty.h"


// CMFCComboBoxProperty

CMFCComboBoxProperty::CMFCComboBoxProperty(const CString& strName, const COleVariant& varValue): CMFCPropertyGridProperty(strName, varValue)
{
}

CMFCComboBoxProperty::~CMFCComboBoxProperty()
{
}

void CMFCComboBoxProperty::OnClickButton(CPoint point)
{
	CMFCPropertyGridProperty::OnClickButton(point);

	//AddDlg.DoModal();

	CAddStringDlg* AddDlg = new CAddStringDlg;
	AddDlg->Create( IDD_ADDSTRING );
	AddDlg->ShowWindow( SW_SHOW );
}
void CMFCComboBoxProperty::OnDrawButton(CDC* pDC, CRect rectButton)
{
	CRect rect = rectButton;
	rect.DeflateRect(1,1,1,1);
	CMFCToolBarButton button;
	CMFCVisualManager::AFX_BUTTON_STATE state = CMFCVisualManager::ButtonsIsHighlighted;
	CMFCVisualManager::GetInstance()->OnFillButtonInterior(pDC, &button, rect, state);
	CMFCVisualManager::GetInstance()->OnDrawButtonBorder(pDC, &button, rect, state);
}
// CMFCComboBoxProperty 멤버 함수
