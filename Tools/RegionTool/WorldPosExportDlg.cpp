// WorldPosExportDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "WorldPosExportDlg.h"


// CWorldPosExportDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CWorldPosExportDlg, CDialog)

CWorldPosExportDlg::CWorldPosExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldPosExportDlg::IDD, pParent)
	, m_nDivision1(0)
	, m_nDivision2(0)
	, m_nX1(0)
	, m_nY1(0)
	, m_nX2(0)
	, m_nY2(0)
	, m_nDisX1(0)
	, m_nDisY1(0)
	, m_nDisX2(0)
	, m_nDisY2(0)
{

}

CWorldPosExportDlg::~CWorldPosExportDlg()
{
}

void CWorldPosExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DIVISION1, m_nDivision1);
	DDX_Text(pDX, IDC_DIVISION2, m_nDivision2);
	DDX_Text(pDX, IDC_X1_EDIT, m_nX1);
	DDX_Text(pDX, IDC_Y1_EDIT, m_nY1);
	DDX_Text(pDX, IDC_X2_EDIT, m_nX2);
	DDX_Text(pDX, IDC_Y2_EDIT, m_nY2);
	DDX_Text(pDX, IDC_X1_STATIC, m_nDisX1);
	DDX_Text(pDX, IDC_Y1_STATIC, m_nDisY1);
	DDX_Text(pDX, IDC_X2_STATIC, m_nDisX2);
	DDX_Text(pDX, IDC_Y2_STATIC, m_nDisY2);
}


BEGIN_MESSAGE_MAP(CWorldPosExportDlg, CDialog)
	ON_BN_CLICKED(ID_EXPORTBTN, &CWorldPosExportDlg::OnBnClickedExportbtn)
END_MESSAGE_MAP()


// CWorldPosExportDlg �޽��� ó�����Դϴ�.

void CWorldPosExportDlg::OnBnClickedExportbtn()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE );

	DOUBLE fDivX1 = GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( m_nDivision1 ) ) );
	DOUBLE fDivX2 = GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( m_nDivision2 ) ) );

	DOUBLE fDivY1 = GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( m_nDivision1 ) ) );
	DOUBLE fDivY2 = GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( m_nDivision2 ) ) );

	DOUBLE fStepSize = ( m_nX2 - m_nX1 ) / 800.0;

	DOUBLE fX1 = ( fDivX1 - m_nX1 ) / fStepSize;
	DOUBLE fY1 = ( fDivY1 - m_nY1 ) / fStepSize;

	DOUBLE fX2 = ( fDivX2 - fDivX1 )  / fStepSize + fX1;
	DOUBLE fY2 = ( fDivY2 - fDivY1 )  / fStepSize + fY1;

	m_nDisX1		=	fX1;
	m_nDisY1		=	fY1;
	m_nDisX2		=	fX2;
	m_nDisY2		=	fY2;

	UpdateData( FALSE );
}
