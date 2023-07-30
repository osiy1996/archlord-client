#pragma once
#include "afxpropertygridctrl.h"
#include "MessageEditDlg.h"
#include "StateEditDlg.h"

class CMFCButtonProperty :	public CMFCPropertyGridProperty
{
public:
	CMFCButtonProperty(const CString& strName, const COleVariant& varValue);
	~CMFCButtonProperty(void);

protected:
	virtual BOOL HasButton() const{return TRUE;}
	virtual void AdjustButtonRect();
	virtual void OnClickButton(CPoint point);
	virtual void OnDrawButton(CDC* pDC, CRect rectButton);
	
	BOOL m_bMessageDlg;
};
