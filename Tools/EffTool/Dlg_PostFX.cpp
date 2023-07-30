// Dlg_PostFX.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EffTool.h"
#include "Dlg_PostFX.h"


// Dlg_PostFX ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(Dlg_PostFX, CDialog)

Dlg_PostFX::Dlg_PostFX(CWnd* pParent /*=NULL*/)
	: CDialog(Dlg_PostFX::IDD, pParent)
	, m_bUnseenOthers(TRUE)
{

}

Dlg_PostFX::~Dlg_PostFX()
{
}

void Dlg_PostFX::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_POSTFX, m_comboPostFX);
	DDX_Check(pDX, IDC_CHECK1, m_bUnseenOthers);
	DDX_Control(pDX, IDC_EDIT3, m_editPosX);
	DDX_Control(pDX, IDC_EDIT4, m_editPosY);
}


BEGIN_MESSAGE_MAP(Dlg_PostFX, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_POSTFX, &Dlg_PostFX::OnCbnSelchangeComboPostfx)
END_MESSAGE_MAP()


// Dlg_PostFX �޽��� ó�����Դϴ�.

BOOL Dlg_PostFX::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_comboPostFX.ResetContent();
	
	m_comboPostFX.InsertString( 0 , "Wave" );
	m_comboPostFX.InsertString( 1 , "ShockWave" );
	m_comboPostFX.InsertString( 2 , "Ripple" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void Dlg_PostFX::OnCbnSelchangeComboPostfx()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_nSelectSel	=	m_comboPostFX.GetCurSel();
}

CString Dlg_PostFX::GetSelPostFX( VOID )
{
	INT			nCurSel	=	m_comboPostFX.GetCurSel();
	CString		strPostFX;

	if( nCurSel == -1 )
		return strPostFX;

	m_comboPostFX.GetLBText( nCurSel , strPostFX );

	return strPostFX;
}

FLOAT Dlg_PostFX::GetPositionX( VOID )
{
	CString str;

	m_editPosX.GetWindowText( str );

	return atof( str.GetBuffer() );
}

FLOAT Dlg_PostFX::GetPositionY( VOID )
{
	CString str;

	m_editPosY.GetWindowText( str );

	return atof( str.GetBuffer() );
}

