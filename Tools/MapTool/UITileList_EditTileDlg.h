#if !defined(AFX_UITILELIST_EDITTILEDLG_H__2C7890A8_F3E0_409E_93A4_CCEE71FC1812__INCLUDED_)
#define AFX_UITILELIST_EDITTILEDLG_H__2C7890A8_F3E0_409E_93A4_CCEE71FC1812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileList_EditTileDlg.h : header file
//

#include <Aulist.h>

// �� Dialog ���� ó���ؾ��� ��
// - Ÿ�� �з�
// - Ÿ�� ī�װ� ����
// - Ÿ�� �ε��� ����
// - Ÿ�� ��ũ��Ʈ ���� ����.
// - Ÿ������ �߰� & ����..

class CUITileList_EditTileDlg : public CDialog
{
// Construction
public:
	CUITileList_EditTileDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUITileList_EditTileDlg)
	enum { IDD = IDD_TILEWINDOW_EDITCATEGORY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileList_EditTileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUITileList_EditTileDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELIST_EDITTILEDLG_H__2C7890A8_F3E0_409E_93A4_CCEE71FC1812__INCLUDED_)
