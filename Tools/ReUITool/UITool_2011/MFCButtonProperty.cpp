#include "StdAfx.h"
#include "MFCButtonProperty.h"
#include "resource.h"


CMFCButtonProperty::CMFCButtonProperty(const CString& strName, const COleVariant& varValue): CMFCPropertyGridProperty(strName, varValue)
{
	if(strName.Compare(_T("StateEdit")) == 0)
	{
		m_bMessageDlg = FALSE;
	}
	else
	{
		m_bMessageDlg = TRUE;
	}
}

CMFCButtonProperty::~CMFCButtonProperty(void)
{
}
void CMFCButtonProperty::AdjustButtonRect()
{
	CMFCPropertyGridProperty::AdjustButtonRect();
}
void CMFCButtonProperty::OnClickButton(CPoint point)
{
	CMFCPropertyGridProperty::OnClickButton(point);
	
	if(m_bMessageDlg)
	{
		CMessageEditDlg* MsgDlg = new CMessageEditDlg;
	
		MsgDlg->Create( IDD_MESSAGEEDITDLG );
		MsgDlg->ShowWindow( SW_SHOW );
	}
	else
	{	
		CStateEditDlg* StaDlg = new CStateEditDlg;
		StaDlg->Create( IDD_STATEEDITDLG );
		StaDlg->ShowWindow( SW_SHOW );
	}
}
void CMFCButtonProperty::OnDrawButton(CDC* pDC, CRect rectButton)
{
	CRect rect = rectButton;
	rect.DeflateRect(1,1,1,1);
	CMFCToolBarButton button;
	CMFCVisualManager::AFX_BUTTON_STATE state = CMFCVisualManager::ButtonsIsHighlighted;
	CMFCVisualManager::GetInstance()->OnFillButtonInterior(pDC, &button, rect, state);
	CMFCVisualManager::GetInstance()->OnDrawButtonBorder(pDC, &button, rect, state);
}