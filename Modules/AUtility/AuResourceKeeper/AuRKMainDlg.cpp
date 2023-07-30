// AuRKMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AuResourceKeeper.h"
#include "AuRKMainDlg.h"
#include "AuRKSingleton.h"
#include "AuUsersDlg.h"
#include "RKInclude.h"
#include "AuRK_API.h"

#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AuRKMainDlg dialog

static UINT BASED_CODE indicators[] =
{
    ID_INDICATOR_FILE,
    ID_INDICATOR_STATUS
};

BOOL PeekAndPump()
{
	static MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
		if (!AfxGetApp()->PumpMessage()) 
		{
			::PostQuitMessage(0);
			return FALSE;
		}	
	}

	return TRUE;
}
/*
DWORD CALLBACK CopyProgressRoutine(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred,
                        LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred,
						DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile,
						HANDLE hDestinationFile, LPVOID lpData)
{
	ProgressParam *pParam = (ProgressParam*)lpData;
	pParam->pBar->SetPos(*pParam->pCurrent + TotalBytesTransferred.LowPart);

	PeekAndPump();
	return PROGRESS_CONTINUE;
}
*/


AuRKMainDlg::AuRKMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AuRKMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AuRKMainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void AuRKMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AuRKMainDlg)
	DDX_Control(pDX, IDC_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, IDC_TREE, m_ctrlTree);
	DDX_Control(pDX, IDC_FILE, m_ctrlFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AuRKMainDlg, CDialog)
	//{{AFX_MSG_MAP(AuRKMainDlg)
	ON_NOTIFY(NM_RCLICK, IDC_FILE, OnRclickFile)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnRclickTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
	ON_COMMAND(ID_ADD_FOLDER, OnAddFolder)
	ON_NOTIFY(NM_CLICK, IDC_FILE, OnClickFile)
	ON_COMMAND(ID_OPEN_REPOSITORY, OnOpenRepository)
	ON_COMMAND(ID_SET_WORKING_FOLDER, OnSetWorkingFolder)
	ON_COMMAND(ID_CREATE_REPORITORY, OnCreateReporitory)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_SHOW_HISTORY, OnShowHistory)
	ON_COMMAND(ID_CHECK_IN, OnCheckIn)
	ON_COMMAND(ID_CHECK_OUT, OnCheckOut)
	ON_COMMAND(ID_GET_LATEST_VERSION, OnGetLatestVersion)
	ON_COMMAND(ID_UNDO_CHECK_OUT, OnUndoCheckOut)
	ON_COMMAND(ID_ADD_FILE, OnAddFile)
	ON_COMMAND(ID_USER_MANAGER, OnUserManager)
	ON_COMMAND(ID_FILE_DELETE, OnFileDelete)
	ON_COMMAND(ID_LOGOUT, OnLogout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AuRKMainDlg message handlers

BOOL AuRKMainDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_iRClickIndex = -1;
	m_hRClickItem = NULL;
	m_eLastClickCtrl = CTRL_NULL;

	// �̹��� ����Ʈ ����
	m_imgDrives.Create(IDB_DRIVES, 16, 1, RGB(0, 128, 128));
	// �̹��� ����Ʈ�� Ʈ�� ��Ʈ�� ����
	m_ctrlTree.SetImageList(&m_imgDrives, TVSIL_NORMAL);

	InitTreeView();
	InitListView();
	InitMainMenu();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AuRKMainDlg::InitTreeView()
{
	// Working Folder�� �����Ǿ� ���� ������ ����Ʈ ��� Ʈ���並 ������� �ʴ´�.
	if (AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()) return;

	m_ctrlTree.DeleteAllItems();
	HTREEITEM hItem = m_ctrlTree.InsertItem(RKROOT, ID_OPENFOLDER, ID_OPENFOLDER);
	FindSubFolder(hItem, AuRKSingleton::Instance()->m_strRepoRootPath + PATH(REMOTE_ROOT_PATH));
	m_ctrlTree.Expand(hItem, TVE_EXPAND);	
}
/*
void AuRKMainDlg::ConvertSystemPath(CString &rPath)
{
	if (rPath.Find(RKROOT) != -1)
	{
		rPath.Delete(0, ::strlen(RKROOT));
		if (rPath.GetLength() && (rPath[0] == '\\')) rPath.Delete(0, 1);
		rPath = AuRKSingleton::Instance()->m_strRepoRootPath + PATH(REMOTE_ROOT_PATH) + PATH(rPath);
		if (rPath[rPath.GetLength() - 1] == '\\') rPath.Delete(rPath.GetLength() - 1, 1);
	}
}
*/
void AuRKMainDlg::ConvertLocalPath(CString &rPath)
{
	if (rPath.Find(RKROOT) != -1)
	{
		rPath.Delete(0, ::strlen(RKROOT));
		if (rPath.GetLength() && (rPath[0] == '\\')) rPath.Delete(0, 1);
		rPath = AuRKSingleton::Instance()->m_strWorkingFolder + PATH(rPath);
		if (rPath[rPath.GetLength() - 1] == '\\') rPath.Delete(rPath.GetLength() - 1, 1);
	}
}

// ������ Node�� Sub Tree�� �����Ѵ�.
void AuRKMainDlg::ExpandFolder(HTREEITEM hItem, LPCTSTR lpPath)
{
	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	CString strPath = lpPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strPath);

	cRKEntryManager.GetEntryList(strPath + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if (RK_FOLDER == stRKEntryInfo.szType)
		{
			// Tree View�� ���� ����Ʈ �߰�
			FindSubFolder(m_ctrlTree.InsertItem(stRKEntryInfo.szFileName, ID_CLOSEDFOLDER, ID_OPENFOLDER, hItem),
							strPath + PATH(stRKEntryInfo.szFileName));
		}
	}
}

void AuRKMainDlg::InitListView()
{
	// List View�� Header �ʱ�ȭ
	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (int i = 0; i < 3; i++)
	{
		lvcolumn.iSubItem = i;
		switch(i)
		{
		case 0:
			lvcolumn.cx = 120;
			lvcolumn.pszText = "Filename";
			break;

		case 1:
			lvcolumn.cx = 120;
			lvcolumn.pszText = "Date-Time";
			break;

		case 2:
			lvcolumn.cx = 62;
			lvcolumn.pszText = "User";
			break;
		}

		m_ctrlFile.InsertColumn(i, &lvcolumn);
	}	

	m_ctrlFile.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

void AuRKMainDlg::InitMainMenu()
{
	m_csMenuChecker.Init();
	m_csMenuChecker.SetMenu(CWnd::GetMenu());

	m_csMenuChecker.SetAdmin(AuRKSingleton::Instance()->m_bAdmin);
	m_csMenuChecker.ReloadAll();
}

void AuRKMainDlg::FindSubFolder(HTREEITEM hItem, LPCTSTR lpPath)
{
	// ���� ������ �ϳ��� �����ϸ� ���� ��� ����
	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;
	cRKEntryManager.GetEntryList(CString(lpPath) + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();

	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if (RK_FOLDER == stRKEntryInfo.szType)
		{
			m_ctrlTree.InsertItem("", ID_CLOSEDFOLDER, ID_OPENFOLDER, hItem);
			break;
		}
	}
}

void AuRKMainDlg::OnRclickFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// ���õ� �������� ������ ����
	if (!m_ctrlFile.GetFirstSelectedItemPosition()) return; 

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Ŭ�� ����
	m_eLastClickCtrl = CTRL_LISTVIEW;
	m_iRClickIndex = pNMListView->iItem;

	// �˾��� ���콺�� ��ǥ�� ����
	CPoint ptMouse;
	::GetCursorPos(&ptMouse);

	// �˾�
	PopupMenu(ptMouse);

	*pResult = 0;
}

void AuRKMainDlg::PopupMenu(CPoint &rPoint)
{
	CMenu PopupMenu, *pPupupMenu;
	PopupMenu.LoadMenu(IDR_MAIN_POPUP);

	pPupupMenu = PopupMenu.GetSubMenu(0);
	int nCmd = pPupupMenu->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | 
		TPM_RIGHTBUTTON, rPoint.x, rPoint.y, this ); 

	if (nCmd) this->SendMessage(WM_COMMAND, nCmd);
}

void AuRKMainDlg::OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	// Click�� �������� ������
	CPoint ptTree;
	::GetCursorPos(&ptTree);
	m_ctrlTree.ScreenToClient(&ptTree);
	HTREEITEM hRClickItem = m_ctrlTree.HitTest(ptTree);

	if (!hRClickItem) return;

	// Ŭ�� ����
	m_eLastClickCtrl = CTRL_TREEVIEW;
	m_hRClickItem = hRClickItem;

	// �˾��� ���콺�� ��ǥ�� ����
	CPoint ptMouse;
	::GetCursorPos(&ptMouse);

	// �˾�
	PopupMenu(ptMouse);

	*pResult = 0;
}

void AuRKMainDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Working Folder�� �����Ǿ� ���� ������ ����Ʈ ��� Ʈ���並 ������� �ʴ´�.
	if (AuRKSingleton::Instance()->m_strWorkingFolder.IsEmpty()) return;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	m_eLastClickCtrl = CTRL_TREEVIEW;
	m_hRClickItem = pNMTreeView->itemNew.hItem;
	CString strPath = GetPathFromNode(pNMTreeView->itemNew.hItem);
//	ConvertSystemPath(strPath);
	ViewFileList(strPath);

	*pResult = 0;
}

// ������ Node���� Root Node������ Path�� �����Ѵ�.
CString AuRKMainDlg::GetPathFromNode(HTREEITEM hItem)
{
	CString strResult = m_ctrlTree.GetItemText(hItem);

	HTREEITEM hParent;
	// ��Ʈ ��带 ���� ������ ����...
	while((hParent = m_ctrlTree.GetParentItem(hItem)) != NULL)
	{
		CString str = m_ctrlTree.GetItemText(hParent);
		if(str.Right(1) != "\\")
			str += "\\";
		strResult = str + strResult;
		hItem = hParent;
	}
	return strResult;
}

void AuRKMainDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// Ȯ��ǰų� ��ҵ� ��带 ����
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	// Ȯ��ǰų� ��ҵ� ��尡 ��Ÿ���� ���丮���� ����
	CString str = GetPathFromNode(hItem);
	
	*pResult = FALSE;

	// ��尡 Ȯ��Ǵ� ���
	if(pNMTreeView->action == TVE_EXPAND)
	{
		// �ռ� �߰��ߴ� ���� ��� ����
		//DeleteFirstChild(hItem);
		DeleteAllChildren(hItem);
		// ��¥ ���丮 ������ �о� ǥ��
		ExpandFolder(hItem, str);
//		*pResult = TRUE;
	}
	else	// ��尡 ��ҵǴ� ���
	{
		// ��� ���� ��带 ����
		DeleteAllChildren(hItem);

		AuRKSingleton::Instance()->ConvertRemotePath(str);
		FindSubFolder(hItem, str);
		// ���ڰ� ����� ��ư ǥ�ø� ���� ���� ��� �߰�
	}
}

// �ϳ��� �ڽ� ��� �����ϱ�
void AuRKMainDlg::DeleteFirstChild(HTREEITEM hParent)
{
	HTREEITEM hItem;
	if((hItem = m_ctrlTree.GetChildItem(hParent)) != NULL)
		m_ctrlTree.DeleteItem(hItem);
}

// ��� �ڽ� ��� �����ϱ�
void AuRKMainDlg::DeleteAllChildren(HTREEITEM hParent)
{
	HTREEITEM hItem;
	if((hItem = m_ctrlTree.GetChildItem(hParent)) == NULL)
		return;

	do
	{
		HTREEITEM hNextItem = m_ctrlTree.GetNextSiblingItem(hItem);
		m_ctrlTree.DeleteItem(hItem);
		hItem = hNextItem;
	} while (hItem != NULL);
}
/*
BOOL AuRKMainDlg::FindSubFolder(HTREEITEM hItem, LPCTSTR lpPathName)
{
	HTREEITEM hItem;
	if((hItem = m_ctrlTree.GetChildItem(hParent)) != NULL) return;

	do 
	{
		HTREEITEM hNextItem = m_ctrlTree.GetNextSiblingItem(hItem);
	} while (hItem != NULL);
	GetPathFromNode(
}
*/
// m_ctrlFile�� Item�� �ϳ� �߰��Ѵ�.
void AuRKMainDlg::AddListItem(LPCTSTR lpFileName, LPCTSTR lpDate, LPCTSTR lpUser)
{
	LV_ITEM lvItem;
	lvItem.iItem = m_ctrlFile.GetItemCount();
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvItem.iSubItem = 0;
	lvItem.stateMask = LVIS_STATEIMAGEMASK;
	lvItem.state = 0;
	lvItem.pszText = (LPSTR)(LPCTSTR)lpFileName;

	m_ctrlFile.InsertItem(&lvItem);

	m_ctrlFile.SetItemText(lvItem.iItem, 1, lpDate);
	m_ctrlFile.SetItemText(lvItem.iItem, 2, lpUser);
}

void AuRKMainDlg::ViewFileList(LPCTSTR lpPath)
{
	m_ctrlFile.DeleteAllItems();
	m_strRefreshPath = lpPath;

	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	CString strPath = lpPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strPath);

	CString strLocalPath = lpPath;
	ConvertLocalPath(strLocalPath);

	cRKEntryManager.GetEntryList(strPath + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);

		if (RK_FILE == stRKEntryInfo.szType)
		{
			// List View�� ���� ����Ʈ �߰�
//			if (!AuRKSingleton::Instance()->IsFileExist(strLocalPath + PATH(stRKEntryInfo.szFileName)))
//				stRKEntryInfo.szOwner = "N " + stRKEntryInfo.szOwner;

			CRKEntryManager cFileVersion;
			if ((!cFileVersion.CheckLatestVersion(strPath + PATH(stRKEntryInfo.szFileName), strLocalPath + PATH(stRKEntryInfo.szFileName))) ||
				(!AuRKSingleton::Instance()->IsFileExist(strLocalPath + PATH(stRKEntryInfo.szFileName))))
				stRKEntryInfo.szOwner = "O " + stRKEntryInfo.szOwner;

			AddListItem(stRKEntryInfo.szFileName, stRKEntryInfo.szDate, stRKEntryInfo.szOwner);
		}
	}
}

void AuRKMainDlg::OnCheckOut() 
{
	// JNY TODO : �켱�� ���ϸ� üũ ��/�ƿ� ����
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		CString strLocalPath = strRepoPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
		ConvertLocalPath(strLocalPath);
		

		CRKEntryManager cRKEntryManager;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));

		BOOL bCheckOut = cRKEntryManager.CheckOut(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)),
			strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), AuRKSingleton::Instance()->m_strUserID);
		if (bCheckOut) 
		{
			m_ctrlFile.SetItemText(m_iRClickIndex, USERNAME, AuRKSingleton::Instance()->m_strUserID);
//			m_ctrlMessage.AddString("Check Out : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
		else
		{
//			m_ctrlMessage.AddString("Check Out Failed : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock Failed : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
	}
}

void AuRKMainDlg::OnAddFolder() 
{
	// TODO: Add your command handler code here
	if (AuRKSingleton::Instance()->m_bLogin)
	{
		CString strPath = GetPathFromNode(m_hRClickItem);
		CString strSelectedPath = strPath;
		strPath = "������ġ : " + strPath;

		AuRKSingleton::Instance()->ConvertRemotePath(strSelectedPath);

		CString strMsg;

		if (AddFolder(strPath, strSelectedPath, strMsg))
		{
			InitTreeView();
//			if (strMsg.GetLength())
//				m_ctrlTree.InsertItem(strMsg, ID_CLOSEDFOLDER, ID_OPENFOLDER, m_hRClickItem);
/*
			// TRUE�̸� �߰��� ������ �̸��� strMsg�� �������ش�.
			// ���� strMsg�� IsEmpty�̸� ���丮 �߰��۾��� ����Ѱ����� �����Ѵ�.
			if (!strMsg.IsEmpty())
			{
				CString strTreePath = GetPathFromNode(m_hRClickItem);
				AuRKSingleton::Instance()->ConvertRemotePath(strTreePath);
				FindSubFolder(m_ctrlTree.InsertItem(strMsg, ID_CLOSEDFOLDER, ID_OPENFOLDER, m_hRClickItem), 
								strTreePath + PATH(strMsg));
			}
*/
		}
		else
		{
			// FALSE�̸� ���� ������ ������ ������ �˷��ش�.
			m_ctrlMessage.AddString(strMsg);
		}
	}
}

void AuRKMainDlg::OnClickFile(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// ���õ� �������� ������ ����
	if (!m_ctrlFile.GetFirstSelectedItemPosition()) return; 

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_eLastClickCtrl = CTRL_LISTVIEW;
	m_iRClickIndex = pNMListView->iItem;

	*pResult = 0;
}

void AuRKMainDlg::OnCheckIn() 
{
	// JNY TODO : �켱�� ���ϸ� üũ ��/�ƿ� ����
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		CString strLocalPath = strRepoPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
		ConvertLocalPath(strLocalPath);

		CRKEntryManager cRKEntryManager;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));

		BOOL bCheckIn = cRKEntryManager.CheckIn(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)),
			strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), AuRKSingleton::Instance()->m_strUserID);
		if (bCheckIn) 
		{
			m_ctrlFile.SetItemText(m_iRClickIndex, USERNAME, "");
//			m_ctrlMessage.AddString("Check In : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
		else
		{
//			m_ctrlMessage.AddString("Check In Failed: " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString("Lock Failed : " + m_ctrlFile.GetItemText(m_iRClickIndex, 0));
		}
	}	
}

void AuRKMainDlg::OnOpenRepository() 
{
	// JNY TODO : Login ��� �����ؾߵ�
	if (OpenLoginDlg())
	{
		// ����
	}
}

void AuRKMainDlg::OnSetWorkingFolder() 
{
	// TODO: Add your command handler code here
	if (SetWorkingFolder())
	{
		InitTreeView();
	}
}

void AuRKMainDlg::OnCreateReporitory() 
{
	// TODO: Add your command handler code here
	AuRKSingleton::Instance()->CreateRepository();
}

void AuRKMainDlg::OnRefresh() 
{
	// TODO: Add your command handler code here
	RefreshFileList();
}

void AuRKMainDlg::OnShowHistory() 
{
	// List View�� ����
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strPath = GetPathFromNode(m_hRClickItem);
		AuRKSingleton::Instance()->ConvertRemotePath(strPath);
		AuRKSingleton::Instance()->ViewHistory(strPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)));
	}
}

void AuRKMainDlg::OnGetLatestVersion() 
{
	CString strRepoPath, strLocalPath;
	strRepoPath = GetPathFromNode(m_hRClickItem);
	strLocalPath = strRepoPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
	ConvertLocalPath(strLocalPath);

	CProgressDlg dlg;
	// List View�� ��쿣 ���� ī�� �� List View�� Index ��ȿ�� �˻�
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		dlg.GetLatestFile(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, FILENAME)));
		if (IDOK == dlg.DoModal())
		{
		}
	}
	// Tree View�� ��쿣 ���� ī�� �� 
	else if ((CTRL_TREEVIEW == m_eLastClickCtrl) && (m_hRClickItem))
	{
		dlg.GetLatestFolder(strLocalPath);
		if (IDOK == dlg.DoModal())
		{
		}
	}

	RefreshFileList();
}

void AuRKMainDlg::OnUndoCheckOut() 
{
	BOOL bRefresh = FALSE;
	// List View�� �����ϸ� ��
	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		CString strLocalPath = strRepoPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);
		ConvertLocalPath(strLocalPath);
		

		CRKEntryManager cRKEntryManager;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));
		
		if (AuRKSingleton::Instance()->m_bAdmin)
		{
			// ������ ��� ������ Unlock�Ѵ�.
			stRKEntryInfo stEntryInfo;
			if (cRKEntryManager.GetEntryFileInfo(m_ctrlFile.GetItemText(m_iRClickIndex, 0), &stEntryInfo))
			{
				CRKWriter csWriter;
				csWriter.SetFileName(strRepoPath + PATH(ENTRIES));
				csWriter.Open();
				csWriter.WriteOwner(stEntryInfo.szFileName, atoi(stEntryInfo.szSize), 
									stEntryInfo.szDate, CString(""), atoi(stEntryInfo.szVer));

				bRefresh = TRUE;
			}
		}
		else
		{
			CString strMsg;
			if (cRKEntryManager.UndoCheckOut(strLocalPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), 
										strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)),
										AuRKSingleton::Instance()->m_strUserID))
			{
				bRefresh = TRUE;
				strMsg.Format("Unlock : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
				m_ctrlMessage.AddString(strMsg);
			}
			else
			{
				strMsg.Format("Unlock Failed : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
				m_ctrlMessage.AddString(strMsg);
			}
		}
	}

	if (bRefresh) RefreshFileList();
}

void AuRKMainDlg::OnAddFile() 
{
	// TODO: Add your command handler code here
	CFileDialog FileDlg(TRUE, "*.*");
	if (IDOK == FileDlg.DoModal())
	{
		// ��� ���
		CString strPathName = AuRKSingleton::Instance()->GetFolderPath(FileDlg.GetPathName());

		// ���� �̸� ���
		CString strFileName = FileDlg.GetFileName();
		CString strFileName2 = strFileName;

		// Repository ��� ���
		CString strRepoPath = GetPathFromNode(m_hRClickItem);
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

		AuRKSingleton::Instance()->AddFile(strPathName + PATH(strFileName2), strRepoPath + PATH(strFileName2));
		RefreshFileList();
	}
}

void AuRKMainDlg::OnUserManager() 
{
	// TODO: Add your command handler code here
	AuUsersDlg UsersDlg;
	UsersDlg.DoModal();
}

void AuRKMainDlg::RefreshFileList()
{
	ViewFileList(m_strRefreshPath);
}

void AuRKMainDlg::OnFileDelete() 
{

	if ((CTRL_LISTVIEW == m_eLastClickCtrl) && ( m_iRClickIndex > -1))
	{
		// ���� ���� ����
		CRKEntryManager cRKEntryManager;
		CString strPath = GetPathFromNode(m_hRClickItem);
		CString strRepoPath = strPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

		CString strFileName = m_ctrlFile.GetItemText(m_iRClickIndex, 0);

		if (IDNO == MessageBox("���� : " + strFileName + "\r\n�� ������ ������?", "����", MB_YESNO)) return;

		CString strMsg;
		cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
		cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));
		if (cRKEntryManager.RemoveEntry(strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), 
					AuRKSingleton::Instance()->m_strUserID))
		{
			// Local ���ϵ� ����
//			ConvertLocalPath(strPath);
//			EnableReadOnly(strPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), FALSE);
//			cRKEntryManager.RemoveFile(strPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)));

			// ���� ���� ����
			strMsg.Format("���� ���� ���� : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString(strMsg);

			RefreshFileList();
		}
		else
		{
			// ���� ���� ����
			strMsg.Format("���� ���� ���� : %s", m_ctrlFile.GetItemText(m_iRClickIndex, 0));
			m_ctrlMessage.AddString(strMsg);
		}

	}
	else if ((CTRL_TREEVIEW == m_eLastClickCtrl) && (m_hRClickItem))
	{
		CString strPath = GetPathFromNode(m_hRClickItem);
		CString strRepoPath = strPath;
		AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

		if (IDNO == MessageBox("���� : " + strRepoPath + "\r\n�� ������ ������?", "����", MB_YESNO)) return;

		CProgressDlg dlg;
		dlg.DeleteFolder(strRepoPath);
		if (IDOK == dlg.DoModal())
		{
			InitTreeView();
		}
	}
}

void AuRKMainDlg::OnLogout()
{
	RKLogout();
	OnCancel();
}

BOOL AuRKMainDlg::AddFolder(LPCTSTR lpTitle, CString &rSelectedPath, CString& rMsg)
{
	// Repository�� ����� ���� ����
	CHAR pszPathName[MAX_PATH];

	CString strFolderName;
	if (AuRKSingleton::Instance()->GetFolderNameDlg(lpTitle, pszPathName, MAX_PATH))
	{
		strFolderName = pszPathName;
		int iPos = strFolderName.ReverseFind('\\');
		strFolderName.Delete(0, iPos + 1);
		rMsg = strFolderName;
		BOOL bResult = AuRKSingleton::Instance()->AddFolder(pszPathName, rSelectedPath + PATH(strFolderName));
		return bResult;
	}

	return FALSE;
}
