// EffectHelpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "agcmeffectdlg.h"
#include "EffectHelpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectHelpDlg dialog


CEffectHelpDlg::CEffectHelpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectHelpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectHelpDlg)
	m_strHelp = _T("");
	//}}AFX_DATA_INIT
}


void CEffectHelpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectHelpDlg)
	DDX_Text(pDX, IDC_EDIT_HELP, m_strHelp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEffectHelpDlg, CDialog)
	//{{AFX_MSG_MAP(CEffectHelpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectHelpDlg message handlers
VOID CEffectHelpDlg::InputString(CHAR *szString)
{
	CHAR	szTemp[256];
	sprintf(szTemp, "\r\n%s", szString);

	m_strHelp	+= szTemp;
}

BOOL CEffectHelpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_strHelp = "[Custom Data]\r\n";

	InputString("DestroyMatch=");
	InputString("����Ʈ �Ҹ�� ������ ����Ʈ�� �����մϴ�.");

	InputString(" ");
	InputString("TargetStruckAnim=");
	InputString("Target character�� �´� �ִϸ��̼��� �մϴ�.");
	InputString("(���� point ��������)");

	InputString(" ");
	InputString("ExtType=");
	InputString("���� ���¸� �����մϴ�.");
	InputString("1 - ������ ����Ʈ");
	InputString("2 - ������");
	InputString("3 - ������ ���¿��� ���ݽ�");
	InputString("4 - �ӽ�");
	InputString("5 - ȸ�� �ߵ���");
	InputString("6 - ������ �ݻ��");
	InputString("7 - �̵��ӵ� ��ȭ��");
	InputString("8 - ������ �����");

	InputString(" ");
	InputString("CheckTimeOn=");
	InputString("����Ʈ�� �����ñ⸦ ���մϴ�.");
	InputString("(�ô���)");

	InputString(" ");
	InputString("CheckTimeOff=");
	InputString("����Ʈ�� �Ҹ�ñ⸦ ���մϴ�.");
	InputString("(�ô���)");

	InputString(" ");
	InputString("TargetOption=");
	InputString("Ÿ���� ������ �����մϴ�.");
	InputString("1 - All");
	InputString("2 - First");
	InputString("3 - Chain");

	InputString(" ");
	InputString("CalcDistOption=");
	InputString("Ÿ�ٰ��� �Ÿ��� ����ؼ� Gap�� �߰��մϴ�.");
	InputString("(���� -> Gap(milliseconds) = ���� * offset)");

	InputString(" ");
	InputString("DirOption=");
	InputString("����Ʈ�� ������ �����մϴ�.");
	InputString("1 - Origin -> Target(Ignore height)");
	InputString("2 - Target -> Origin(Ignore height)");
	InputString("3 - Origin -> Target");
	InputString("4 - Target -> Origin");

	InputString(" ");
	InputString("LinkSkill=");
	InputString("��ų�� �ߵ��մϴ�.(SKILL TID)");



	InputString(" ");
	InputString("ExtTypeCustData=");
	InputString("Exception type�� �߰��� ����� ���Ǹ� �����մϴ�.");
	InputString("Exception type(������) - ������ ���");



	InputString(" ");
	InputString("CheckUser=");
	InputString("Effect�� ����ϴ� ��ü�� �����մϴ�.");
	InputString("1 - PC");
	InputString("2 - NPC");
	InputString("3 - Monster");
	InputString("4 - GM");

	InputString(" ");
	InputString("ActionObj=");
	InputString("Effect�� ����ϴ� ��ü�� Action�� �ʿ��� ��� ����մϴ�.");
	InputString("1 - Hide shield and weapons(���꽺ų�� ���)");

	//. 2005. 10. 06 Nonstopdj
	//. ��Ÿ���� -_-
	InputString(" ");
	InputString("NatureCondition=");
	InputString("��ī�̼¿� ���� ȿ���� ���¸� �����մϴ�.");
	InputString("0 - Normal <��������ʴ´�.>");
	InputString("1 - Cold");

	//@{ 2007/01/03 burumal
	InputString(" ");
	InputString("CastingEffect=");
	InputString("��ų�� ���Ǵ� Effect�� ĳ���ÿ� ���Ǵ� ����Ʈ���� ����Ѵ�.");	
	InputString("1 - ������ ĳ���� ����Ʈ�� Disable �ȴ�.");
	//@}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
