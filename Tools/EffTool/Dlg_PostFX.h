#pragma once
#include "afxwin.h"


// Dlg_PostFX 대화 상자입니다.

class Dlg_PostFX : public CDialog
{
	DECLARE_DYNAMIC(Dlg_PostFX)

public:
	Dlg_PostFX(CWnd* pParent = NULL);   // 표준 생성자입니다.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	INT			m_nSelectSel;
	BOOL		m_bUnseenOthers;
	CComboBox	m_comboPostFX;
	
	CEdit		m_editPosX;
	CEdit		m_editPosY;
};
