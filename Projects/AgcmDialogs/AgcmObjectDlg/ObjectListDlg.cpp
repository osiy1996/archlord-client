// ObjectListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "ObjectListDlg.h"
#include "EditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectListDlg dialog


CObjectListDlg::CObjectListDlg(ObjectListDlgCallback pfInitCallback, ObjectListDlgCallback pfEndCallback, PVOID pvCustClass, CHAR *pszDest, CWnd* pParent /*=NULL*/)
	: CDialog(CObjectListDlg::IDD, pParent),m_bListUpdated( FALSE )
{
	//{{AFX_DATA_INIT(CObjectListDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pvCustClass		= pvCustClass;
	m_pfInitCallback	= pfInitCallback;
	m_pfEndCallback		= pfEndCallback;
	m_pszDest			= pszDest;
}


void CObjectListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectListDlg)
	DDX_Control(pDX, IDC_TREE_OBJECT_LIST, m_csList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectListDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectListDlg)
	ON_BN_CLICKED(IDC_BUTTON_OL_ADD, OnButtonOlAdd)
	ON_BN_CLICKED(IDC_BUTTON_OL_REMOVE, OnButtonOlRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectListDlg message handlers

BOOL CObjectListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_pfInitCallback)
		m_pfInitCallback(this, m_pvCustClass);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HTREEITEM CObjectListDlg::_InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD dwData, BOOL bEnsureVisible)
{
	HTREEITEM hThis = m_csList.InsertItem(lpszItem, hParent, TVI_SORT);
	if(!hThis)
		return NULL;

	if(dwData)
	{
		if(!m_csList.SetItemData(hThis, dwData))
			return NULL;
	}

	if(bEnsureVisible)
		m_csList.EnsureVisible(hThis);

	return hThis;
}

HTREEITEM CObjectListDlg::_FindItem(LPCSTR lpszItem)
{
	HTREEITEM hItem = m_csList.GetRootItem();

	HTREEITEM	hTemp;
	CString		str;
	while(hItem)
	{
		while(hItem)
		{
			str = m_csList.GetItemText(hItem);
			if(str == lpszItem)
				return hItem;

			hTemp = m_csList.GetNextItem(hItem, TVGN_CHILD);
			if(!hTemp)
			{
				break;
			}

			hItem = hTemp;
		}

		while(hItem)
		{
			hTemp = m_csList.GetNextItem(hItem, TVGN_NEXT);
			if(hTemp)
			{
				hItem = hTemp;
				break;
			}

			hItem = m_csList.GetParentItem(hItem);
			if(!hItem)
			{
				return NULL;
			}
		}
	}

	return NULL;
}

void CObjectListDlg::OnOK() 
{
	// TODO: Add extra validation here
	HTREEITEM hSelected = m_csList.GetSelectedItem();
	if(!hSelected)
	{
		MessageBox("���� �����ϼ���.", "Error");
		return;
	}

	if(m_csList.GetItemData(hSelected) != OBJECGT_LIST_DLG_TREE_ITEM_SELECT)
	{
		MessageBox("ī�װ��� �����Ͻ� �� �����ϴ�.", "Error");
		return;
	}

	if(m_pfEndCallback)
	{
		if( m_pfEndCallback(this, m_pvCustClass) && m_bListUpdated )
		{
			// �̰� ���Ͽ����� �����ϴ°Ŷ� ���� ��ü�� ������Ʈ�� �Ǿ����� �ʴ�.
			// �׷��� ����Ʈ�� ����� ����� �������� LoadTemplate�� �ٽ� �ؼ�
			// ����Ʈ�� ������ ���־�� �ϴµ� �����Ƽ�
			// �ٽ� ����� �޽����� ����
			// ��ü ÷�� ���� �̷��� �����! ����
			// by ������ 2007/12/03

			MessageBox( "������Ʈ ī�װ��� ������Ʈ �Ǿ����ϴ�. ���� �ٽ� �����Ű����" );
		}
	}

	strcpy(m_pszDest, m_csList.GetItemText(hSelected));

	CDialog::OnOK();
}

void CObjectListDlg::OnButtonOlAdd() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM	hParent	= m_csList.GetSelectedItem();
	if(!hParent)
	{
		MessageBox("���� �������� �����ϼ���.", "Error");
		return;
	}

	CHAR	szNewName[256];
	CHAR	szCategory[256];
	CEditDlg	dlg(szNewName);
	if(dlg.DoModal() == IDOK)
	{
		if(!strcmp(szNewName, ""))
		{
			MessageBox("ī�װ� �̸��� �ٽ� �����ϼ���!", "Error");
			return;
		}

		sprintf(szCategory, "[%s]", szNewName);
		_InsertItem(szCategory, hParent, OBJECGT_LIST_DLG_TREE_ITEM_SELECT, TRUE);

		m_bListUpdated	= TRUE;
	}
}

void CObjectListDlg::OnButtonOlRemove() 
{
	// TODO: Add your control notification handler code here
	HTREEITEM	hParent	= m_csList.GetSelectedItem();
	if(!hParent)
	{
		MessageBox("���� �������� �����ϼ���.", "Error");
		return;
	}

	m_csList.DeleteItem(hParent);
	m_bListUpdated	= TRUE;
}

VOID CObjectListDlg::_EnableButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_OL_ADD)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_OL_REMOVE)->EnableWindow(bEnable);
}