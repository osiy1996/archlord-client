// FindObjectSettingDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "FindObjectSettingDlg.h"


// CFindObjectSettingDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CFindObjectSettingDlg, CDialog)
CFindObjectSettingDlg::CFindObjectSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindObjectSettingDlg::IDD, pParent)
	, m_nEventID(0)
{
}

CFindObjectSettingDlg::~CFindObjectSettingDlg()
{
}

void CFindObjectSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EVENTID, m_nEventID);
}


BEGIN_MESSAGE_MAP(CFindObjectSettingDlg, CDialog)
END_MESSAGE_MAP()


// CFindObjectSettingDlg �޽��� ó�����Դϴ�.
