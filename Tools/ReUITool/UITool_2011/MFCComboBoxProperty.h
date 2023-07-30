#pragma once
#include "afxpropertygridctrl.h"
#include "AddStringDlg.h"

// CMFCComboBoxProperty ��� ����Դϴ�.

class CMFCComboBoxProperty : public CMFCPropertyGridProperty
{
public:
	CMFCComboBoxProperty(const CString& strName, const COleVariant& varValue);
	virtual ~CMFCComboBoxProperty();

	virtual BOOL HasButton() const{return TRUE;}
	virtual void OnClickButton(CPoint point);
	virtual void OnDrawButton(CDC* pDC, CRect rectButton);

	//CAddStringDlg* AddDlg;
};


