// AgcmAI2TemplateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmai2dlg.h"
#include "AgcmAI2TemplateDlg.h"

#include "AgcmAI2Dialog.h"

//AgcmAI2Dialog				g_csAgcmAI2Dialog;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmAI2TemplateDlg dialog


AgcmAI2TemplateDlg::AgcmAI2TemplateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AgcmAI2TemplateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmAI2TemplateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AgcmAI2TemplateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmAI2TemplateDlg)
	DDX_Control(pDX, IDC_AI2TemplateNameEdit, m_AI2TemplateNameEdit);
	DDX_Control(pDX, IDC_AI2TemplateCombo, m_cAI2TemplateCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmAI2TemplateDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmAI2TemplateDlg)
	ON_BN_CLICKED(IDC_AI2TemplateSelectButton, OnAI2TemplateSelectButton)
	ON_BN_CLICKED(IDC_AI2TemplateRemoveButton, OnAI2TemplateRemoveButton)
	ON_BN_CLICKED(IDC_AI2ExitButton, OnAI2ExitButton)
	ON_BN_CLICKED(IDC_AI2TemplateAddButton, OnAI2TemplateAddButton)
	ON_BN_CLICKED(IDC_AI2TemplateUpdateButton, OnAI2TemplateUpdateButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void AgcmAI2TemplateDlg::InitData( AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2 )
{
	m_pcsAgpmItem = pcsAgpmItem;
	m_pcsAgpmSkill = pcsAgpmSkill;
	m_pcsAgpmAI2 = pcsAgpmAI2;
}

void AgcmAI2TemplateDlg::SetTemplateCombo()
{
	AgpaAI2Template			*pcsAgpaAI2Template;
	AgpdAI2Template			**ppcsAI2Template;

	int				iIndex;

	iIndex = 0;

	//��~ ������~
	m_cAI2TemplateCombo.ResetContent();

	//���ø��̸�/TID�� ��~ �־��.
	pcsAgpaAI2Template = &m_pcsAgpmAI2->m_aAI2Template;

	while( 1 ) 
	{
		ppcsAI2Template = (AgpdAI2Template **)pcsAgpaAI2Template->GetObjectSequence( &iIndex );

		if( ppcsAI2Template == NULL )
		{
			break;
		}
		else
		{
			INT32			lResultIndex;

			lResultIndex = m_cAI2TemplateCombo.AddString( (*ppcsAI2Template)->m_strTemplateName );

			if( lResultIndex != CB_ERR )
			{
				m_cAI2TemplateCombo.SetItemData( lResultIndex, (*ppcsAI2Template)->m_lTID );
			}
		}
	}
}

// AgcmAI2TemplateDlg message handlers
BOOL AgcmAI2TemplateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetTemplateCombo();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AgcmAI2TemplateDlg::OnAI2TemplateAddButton() 
{
	// TODO: Add your control notification handler code here
	char			strAITemplateName[255];

	m_AI2TemplateNameEdit.GetWindowText( strAITemplateName, sizeof(strAITemplateName) );

	if( !strlen( strAITemplateName ) )
	{
		MessageBox( "Template�̸��� �������ֽʽÿ�." );
	}
	else
	{
		INT32			lResult;

		lResult = m_cAI2TemplateCombo.FindStringExact( -1, strAITemplateName );

		if( lResult != CB_ERR )
		{
			MessageBox( "�̹� �����ϴ� ���ø��̸��Դϴ�." );
		}
		else
		{
			//�̷����� �۾��� ���ְ�~
			AgpdAI2Template		*pcsAI2Template;

			pcsAI2Template = new AgpdAI2Template;

			pcsAI2Template->m_lTID = m_pcsAgpmAI2->GetEmptyTemplateID();

			if( m_pcsAgpmAI2->m_aAI2Template.AddAITemplate( pcsAI2Template ) )
			{
				strcat( pcsAI2Template->m_strTemplateName, strAITemplateName );
//				g_csAgcmAI2Dialog.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, pcsAI2Template );

//				if( g_csAgcmAI2Dialog.DoModal() == IDOK )
				{
					//���α׸���.
					SetTemplateCombo();
				}
			}
			else
			{
				MessageBox( "���ø��� ���������߽��ϴ�." );
			}
		}
	}	
}

void AgcmAI2TemplateDlg::OnAI2TemplateUpdateButton() 
{
	// TODO: Add your control notification handler code here
	INT32				lCurSel;

	lCurSel = m_cAI2TemplateCombo.GetCurSel();

	if( lCurSel == CB_ERR )
	{
		MessageBox( "���ø��� �������ֽʽÿ�." );
	}
	else
	{
		//�̷����� �۾��� ���ְ� ����.
		AgpdAI2Template		*pcsAI2Template;

		INT32				lItemData;

		lItemData = m_cAI2TemplateCombo.GetItemData( lCurSel );

		pcsAI2Template = m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( lItemData );;

		if( pcsAI2Template != NULL )
		{
//			g_csAgcmAI2Dialog.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, pcsAI2Template );

//			g_csAgcmAI2Dialog.DoModal();
		}
		else
		{
			MessageBox( "TID�� ã���� �����ϴ�." );
		}
	}
}

void AgcmAI2TemplateDlg::OnAI2TemplateSelectButton() 
{
	// TODO: Add your control notification handler code here
	
}

void AgcmAI2TemplateDlg::OnAI2TemplateRemoveButton() 
{
	// TODO: Add your control notification handler code here
	INT32				lCurSel;

	lCurSel = m_cAI2TemplateCombo.GetCurSel();

	if( lCurSel == CB_ERR )
	{
		MessageBox( "���ø��� �������ֽʽÿ�." );
	}
	else
	{
		m_cAI2TemplateCombo.DeleteString( lCurSel );
	}
}

void AgcmAI2TemplateDlg::OnAI2ExitButton()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

BOOL AgcmAI2TemplateDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP )
	{
		if( pMsg->wParam == VK_RETURN )
		{
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
