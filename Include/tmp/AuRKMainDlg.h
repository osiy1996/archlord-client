#if !defined(AFX_AURKMAINDLG_H__4A174695_528E_4395_8955_D7C438927197__INCLUDED_)
#define AFX_AURKMAINDLG_H__4A174695_528E_4395_8955_D7C438927197__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuRKMainDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AuRKMainDlg dialog

#include "ProgressBar.h"
#include "MenuChecker.h"

#define ID_CLOSEDFOLDER	4
#define ID_OPENFOLDER	5
#define RKROOT "$RK"
#define LISTVIEW 0
#define TREEVIEW 1
#define PROGRESS_WIDTH 95

#define FILENAME 0	// List Control�� ù��° ���������
#define DATETIME 1	// List Control�� �ι�° ���������
#define USERNAME 2	// List Control�� ����° ���������

enum EnumCtrl
{
	CTRL_NULL		= 0,
	CTRL_LISTVIEW,
	CTRL_TREEVIEW,
};

class AuRKMainDlg : public CDialog
{
private:
	// �ʱ�ȭ
	void InitTreeView();
	void InitListView();
	void InitMainMenu();
	void InitStatusBar();

	void PopupMenu(CPoint &rPoint);

	// Tree View�� ���õ� �޼ҵ�
	CString GetPathFromNode(HTREEITEM hItem);
	void DeleteFirstChild(HTREEITEM hParent);
	void DeleteAllChildren(HTREEITEM hParent);
	void ExpandFolder(HTREEITEM hItem, LPCTSTR lpPath);
	void FindSubFolder(HTREEITEM hItem, LPCTSTR lpPath);
//	BOOL FindSubFolder(HTREEITEM hItem, LPCTSTR lpPathName);

	// List View�� ���õ� �޼ҵ�
	void AddListItem(LPCTSTR lpFileName, LPCTSTR lpDate, LPCTSTR lpUser);
	void ViewFileList(LPCTSTR lpPath);
	void RefreshFileList();

//	void EnableReadOnly(LPCTSTR lpExistingFile, BOOL bEnable);
//	BOOL GetLatestFile(LPCTSTR lpExistingFile, LPCTSTR lpNewFile, ProgressParam *pParam);
//	void GetLatestFolder(LPCTSTR lpExistingPath, LPCTSTR lpNewPath, ProgressParam *pParam);
//INT32 GetRepoFolderSize(LPCTSTR lpExistingPath);
//	UINT32 GetLocalFolderSize(LPCTSTR lpExistingPath);
//INT32 GetFileSize(LPCTSTR lpExistingFile);

	BOOL AddFolder(LPCTSTR lpTitle, CString &rSelectedPath, CString& rMsg);
//	BOOL DirectoryCopy(LPCTSTR lpExistingPath, LPCTSTR lpNewPath, LPCTSTR lpExt, ProgressParam *pParam);

public:
	INT32 m_iRClickIndex;			// List View�� ��Ŭ�� �Ǿ������� Index
	HTREEITEM m_hRClickItem;		// Tree View�� ��Ŭ�� �Ǿ������� Path;
	EnumCtrl m_eLastClickCtrl;		// ���������� ���� ��Ʈ��
	CImageList m_imgDrives;
	CString m_strRefreshPath;

//	CStatusBar  m_wndStatusBar;
	CMenuChecker m_csMenuChecker;

// Construction
public:
	AuRKMainDlg(CWnd* pParent = NULL);   // standard constructor
//	void ConvertSystemPath(CString &rPath);
	void ConvertLocalPath(CString &rPath);

// Dialog Data
	//{{AFX_DATA(AuRKMainDlg)
	enum { IDD = IDD_MAIN };
	CListBox	m_ctrlMessage;
	CTreeCtrl	m_ctrlTree;
	CListCtrl	m_ctrlFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AuRKMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AuRKMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRclickFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddFolder();
	afx_msg void OnClickFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOpenRepository();
	afx_msg void OnSetWorkingFolder();
	afx_msg void OnCreateReporitory();
	afx_msg void OnRefresh();
	afx_msg void OnShowHistory();
	afx_msg void OnCheckIn();
	afx_msg void OnCheckOut();
	afx_msg void OnGetLatestVersion();
	afx_msg void OnUndoCheckOut();
	afx_msg void OnAddFile();
	afx_msg void OnUserManager();
	afx_msg void OnFileDelete();
	afx_msg void OnLogout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AURKMAINDLG_H__4A174695_528E_4395_8955_D7C438927197__INCLUDED_)
