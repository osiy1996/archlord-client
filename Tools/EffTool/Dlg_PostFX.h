#pragma once
#include "afxwin.h"


// Dlg_PostFX ��ȭ �����Դϴ�.

class Dlg_PostFX : public CDialog
{
	DECLARE_DYNAMIC(Dlg_PostFX)

public:
	Dlg_PostFX(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~Dlg_PostFX();

	enum { IDD = IDD_DLG_POSTFX };

	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboPostfx();

	BOOL		GetUnseenOthers	( VOID )		{	return m_bUnseenOthers;	}
	INT			GetCurrentSel	( VOID )		{	return m_nSelectSel;	}
	CString		GetSelPostFX	( VOID );

	FLOAT		GetPositionX	( VOID );
	FLOAT		GetPositionY	( VOID );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	INT			m_nSelectSel;
	BOOL		m_bUnseenOthers;
	CComboBox	m_comboPostFX;
	
	CEdit		m_editPosX;
	CEdit		m_editPosY;
};
