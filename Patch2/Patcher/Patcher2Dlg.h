// Patcher2Dlg.h : header file
//

#if !defined(AFX_PATCHER2DLG_H__9F561C0E_1B34_43A4_8315_385F822A5D82__INCLUDED_)
#define AFX_PATCHER2DLG_H__9F561C0E_1B34_43A4_8315_385F822A5D82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPatcher2Dlg dialog

#include "ftp.h"
#include "AuThread.h"
#include "afxwin.h"
#include "auinimanager.h"
#include <map>
#include "afxcmn.h"
#include "ListBoxDrag.h"

#include "DirectoryChanges.h"
#include "DirectoryChangeHandler_LB.h"

#define PATCHER2_CONFIG_FILE		"Patch.cfg"

#define PATCHER2_CONFIG_JZP			"JZP"
#define PATCHER2_CONFIG_RES4REAL	"Res4Real"
#define PATCHER2_CONFIG_RES4TEST	"Res4Test"
#define PATCHER2_CONFIG_PATCH		"PATCH"

#define PATCHER2_CONFIG_SERVER		"FTPServer"
#define PATCHER2_CONFIG_PORT		"FTPPort"
#define PATCHER2_CONFIG_ID			"ID"
#define PATCHER2_CONFIG_PW			"PW"
#define PATCHER2_CONFIG_PASSIVE		"FTPPassive"

#define PATCHER2_CONFIG_PATCH_LASTYEAR		"LastYear"
#define PATCHER2_CONFIG_PATCH_LASTMONTH		"LastMonth"
#define PATCHER2_CONFIG_PATCH_LASTDAY		"LastDay"

#define WM_USERMESSAGE				(WM_USER+83)

// Thread���� ����� �۾� ����
enum eOperationType
{
	OPERATION_MAKE_PACKAGE_DATA	,
	OPERATION_BUILD_PATCH		,
	OPERATION_UPLOAD_JZP		,
	OPERATION_PATCH_DONE		,

	OPERATION_PATCH_ALL			,
	OPERATION_REBUILD			,

	OPERATION_FULLSCAN			,

	OPERATION_COUNT				,
};

// Patch ����
enum ePatchType
{
	PATCH_FAST					,	// �ش� ���ϸ� ������ ��
	PATCH_NORMAL				,	// ��ü���� �˻�

	PATCH_COUNT					,
};

class CPatcherFTPInfo
{
public:
	vector <string>			m_aszServers;
    int						m_nPort;
	string					m_strID;
	string					m_strPW;
	bool					m_bPassiveMode;
	bool					m_bEnabled;

	CPatcherFTPInfo()
	{
		m_nPort			= 21;
		m_bPassiveMode	= false;
		m_bEnabled		= false;
	}
};

class CPatcherConfig
{
public:
	CPatcherFTPInfo		m_csJZP;
	CPatcherFTPInfo		m_csRes4Real;
	CPatcherFTPInfo		m_csRes4Test;

	INT					m_csLastPatchDate;
};

#define  FileUpdateList		std::list< string >				
#define  FileUpdateListIter	std::list< string >::iterator		

class CPatcher2Dlg 
	:	public CDialog	,
		public AuThread			// Thread ���� Class
{
private:

	BOOL				m_bThreadRun;		// Thread ���� ����
	eOperationType		m_eOperation;		// Thread�� �����ɶ�.. �ϰ� �ִ� �۾�
	CPatcherConfig		m_csConfig;			// Patcher Configuration
	INT					m_PatchType;		// Patch Type

	//File Count
	INT					m_lMaxProcessFileCount;
	INT					m_lProcessFileCount;

	// Dirs
	CHAR				m_strPatchFolder		[ MAX_PATH ];
	CHAR				m_strSourceFolder		[ MAX_PATH ];
	CHAR				m_strResDir				[ MAX_PATH ];
	CHAR				m_strRawFileDir			[ MAX_PATH ];
	CHAR				m_strZipDir				[ MAX_PATH ];
	CHAR				m_strResortJZPDir		[ MAX_PATH ];
	CHAR				m_strPatchResBackupDir	[ MAX_PATH ];
	CHAR				m_strPackingDir			[ MAX_PATH ];
	CHAR				m_strRebuildDir			[ MAX_PATH ];
	CHAR				m_strDebugDir			[ MAX_PATH ];
	CHAR				m_strFileName			[ MAX_PATH ];

	bool				m_bAddList;
	bool				m_bBuildPatch;
	int					m_lCurrentDisplayVersion;

	//FTP Lib 4 JZP, RES, DAT, REF
	CPatcherFTP			m_cPatcherFTP;

	//Lib 4 Packing
	AuPackingManager	m_csAuPackingManager;

	//Lib 4 Compress
	CCompress			m_cCompress;


	// If you do FullScan option to check the date, the date in the files and folders is checked.
	INT					m_nFullScanType;		//	0 - by the last patch , 1 - All scan , 2 -  specify dates
	INT					m_nMaxYear;
	INT					m_nMaxMonth;
	INT					m_nMaxDay;
	INT					m_nMinYear;
	INT					m_nMinMonth;
	INT					m_nMinDay;

	// Finally, the date Patch
	INT					m_nLastPatchYear;
	INT					m_nLastPatchMonth;
	INT					m_nLastPatchDay;

	bool		ReadConfig				( VOID );
	bool		SaveConfig				( VOID );
	bool		ReadConfigFTP			( AuIniManagerA *pcsINI, int nSection, CPatcherFTPInfo *pcsInfo );
	bool		ReadConfigPatch			( AuIniManagerA *pcsINI, int nSection, INT *pcsInfo );

	
	bool		SaveConfigPatch			( AuIniManagerA *pcsINI, int nSection, INT *pcsInfo );

	//Export Res
	bool		BuildResFile			( CDList<CPatchVersionInfo *> *pcsPatchVersionInfo, char *pstrResFileName, char *pstrRawDir, char *pstrResDir );

	VOID		DisplayPatchedFiles		( VOID );

	VOID		UpdateListBox			( INT lVersion );
	VOID		DisplayPatchVersion		( INT lVersion );
	VOID		FindSubDirFiles			( LPCTSTR pstrSourcePath, LPCTSTR pstrCurrentPath, CListBox *pcListBox, bool bCompareCheckSum = false );
	VOID		AutoOneFileAdd			( LPCTSTR pFullPathFileName, LPCTSTR pstrCurrentPath, CListBox *pcListBox, bool bCompareCheckSum = false );
	VOID		OneFileAdd				( LPCTSTR pFullPathFileName, bool bCompareCheckSum	=	false );

	VOID		GetMaxProcessFileCount	( CHAR *pstrFolderName );

	BOOL		CopyFileToPatchFolder	( CHAR *pstrSPath , CHAR *pstrDPath , CHAR *pstrFullPath , BOOL bOverwrite );
	BOOL		Copy2BackupAndCompress	( CPatchVersionInfo *pcsPatchVersionInfo );
	BOOL		MakeCompressedFiles		( CPatchVersionInfo *pcsPatchVersionInfo );

	VOID		setResDir				( CHAR *pstrResDir			);
	VOID		setRawFileDir			( CHAR *pstrRawFileDir		);
	VOID		setZipDir				( CHAR *pstrZipDir			);
	VOID		setResortJZPDir			( CHAR *pstrResortJZPDir	);
	VOID		setPatchResBackupDir	( CHAR *pstrPackingDir		);
	VOID		setRebuildDir			( CHAR *pstrRebuildDir		);
	VOID		setDebugInfoDir			( CHAR *pstrDebugDir		);
	VOID		setPackingDir			( CHAR *pstrPackingDir		);

	bool		ProcessSortFiles		( VOID );
	bool		ProcessRebuild			( CDList<CPatchVersionInfo *> *pcsPatchVersionInfo );
 
	VOID		UploadJZP				( CPatcherFTPInfo *pcsFTPInfo, char *strResDir, int nStartVersion, int nEndVersion );
	VOID		UploadRes				( CPatcherFTPInfo *pcsFTPInfo, char *strResSourccePath );

	BOOL		_BuildPatch				( VOID );
	VOID		_PatchAll				( VOID );
	VOID		_MakePackage			( VOID );
	VOID		_UploadJZP				( VOID );
	VOID		_PatchDone				( VOID );
	VOID		_ReBuild				( VOID );
	VOID		_FullScan				( VOID );

	VOID		_FastMakeJZPInfo		( char *pstrResFileName, char *pstrRawDir, char *pstrResDir , FILE* pFile );
	VOID		_NormalMakeJZPInfo		( CDList<CPatchVersionInfo *> *pcsPatchVerionInfo , char *pstrResFileName, char *pstrRawDir, char *pstrResDir , FILE* pFile );

public:
	CPatcher2Dlg(CWnd* pParent = NULL);	// standard constructor

	enum { IDD = IDD_PATCHER2_DIALOG };

	CProgressCtrl	m_cPatchFileProgress;
	CListBox		m_cPatchedFilesList;
	CStatic			m_cPatchVersionStatic;
	CListBox		m_cRemoveFileList;
	CListBoxDrag	m_cFileInfoList;

protected:
	virtual void			DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual VOID			Run( VOID );

	VOID					SetThreadRun( BOOL bThreadRun )		{ m_bThreadRun	=	bThreadRun;	}
	BOOL					GetThreadRun( VOID )				{ return	m_bThreadRun;		}

protected:
	HICON				m_hIcon;

	BOOL				m_bExecutableAvailable	; 
	string				m_strRegion				;	// ��ġ�ϴ� ��������.

	virtual BOOL		OnInitDialog();

	afx_msg HCURSOR		OnQueryDragIcon();
	afx_msg void		OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void		OnPaint();
	afx_msg void		OnRemoveScanFolderButton();
	afx_msg void		OnScanFolderButton();
	afx_msg void		OnRegisterButton();
	afx_msg void		OnExitButton();
	afx_msg void		OnDisplayPrevVersionButton();
	afx_msg void		OnDisplayNextVersionButton();
	afx_msg void		OnBnClickedPatchfileremovebutton();
	afx_msg void		OnBnClickedPatchfileremoveresetbutton();
	afx_msg void		OnBnClickedRemoveresetbutton();
	afx_msg void		OnBnClickedResetbutton();
	afx_msg void		OnBnClickedRebuildbutton();
	
	afx_msg void		OnBnClickedUploadjzp();
	afx_msg void		OnBnClickedBuildpatch();
	afx_msg void		OnBnClickedPatchdone();
	afx_msg void		OnBnClickedUploadres();
	afx_msg void		OnBnClickedUploadalljzps();
	afx_msg void		OnBnClickedRebuilddone();
	afx_msg void		OnBnClickedPatchAll();

	// Ÿ ��⿡�� �޼����� �ޱ� ���� �Լ�
	afx_msg LRESULT		OnWriteLogFile( WPARAM wParam , LPARAM lPAram );
	afx_msg LRESULT		OnInitProgress( WPARAM wParam , LPARAM lPAram );

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL			m_bSyncExistingFile;
	BOOL			m_bCompareChecksum;

	CEdit			m_editComment;
	CListBox		m_lbProgress;

	CComboBox		m_comboList;				// Patch Server List
	CButton			m_cCheckBuildJZP;
	CButton			m_cCheckBuildPackingData;
	CButton			m_csCheckCopyRawFiles;
	CButton			m_btnBuildPatch;
	CButton			m_btnUploadJZP;
	CButton			m_btnPatchDone;
	CButton			m_btnPackageData;
	CButton			m_btnLog;					//	Log File ��� ���� üũ��ư	
	CButton			m_btnExit;

	CButton			m_btnSensorOn;				//	Folder Sensor On
	CButton			m_btnSensorOff;				//	Folder Sensor Off

	CStatic			m_staticPatchFile;			//	�߰��� ���� ���� ǥ��
	CStatic			m_staticDelFile;			//	������ ���� ���� ǥ��
	CStatic			m_staticSkipFile;			//	��ŵ�� ���� ���� ǥ��

	INT32			m_nSkilFileCount;			//	��ŵ�� ���� ����

	FileUpdateList	m_FileUpdateList;			//	������Ʈ ���� ����Ʈ

	std::map< CString , CString >	m_ServerMap;

	VOID			AddListString( const CString& str );	// List Box�� ����� �����

// Option CheckBox Button Clicked..
public:
	afx_msg VOID	OnBnClickedSynexistingfiles();
	afx_msg VOID	OnBnClickedMakePackage();
	afx_msg VOID	OnBnClickedBuildjzpcheckbutton();
	afx_msg VOID	OnBnClickedBuildpackingcheckbutton();
	afx_msg VOID	OnBnClickedCopyrawfilesButton();
	afx_msg VOID	OnBnClickedCompareChecksum();

	BOOL			CheckPatchFile( const char* szFileName );

	DWORD			GetFileSize( const char* szFileName );
	DWORD			GetCheckSum( const char* szFileName );

	BOOL			CheckAddFile( const char* szFileName , DWORD dwCheckSum );

	VOID			DragFileAdd( const char* szFolderName );
	VOID			RealTimeSensorFileAdd( const char* szFileName );

	VOID			UpdateFileList	( VOID );

private :
	VOID			_MakeFolder( char* pstrTargetPath, char* pFolderName );
	VOID			_CopyPathUntilUpperPath( char* pstrUpperPath, char* pCurrentPath );
	VOID			_CompressFolderFiles( char* pstrTargetPath, CPatchFolderInfo* pFolder );
	VOID			_CopyFolderFiles( char* pstrTargetPath, CPatchFolderInfo* pFolder );
	VOID			_ReadyReferenceFile( FILE* pRefFile, CPatchFolderInfo* pFolder );
	VOID			_PackingFile( FILE* pDatFile, FILE* pRefFile, const int nFileCount, CPackingFile* pPackFile, char* pOriginalFile );
	VOID			_CopyFile( char* pstrOriginalFile, char* pstrOutputDir, char* pTargetPath, char* pTargetName );
	VOID			_EncryptReferenceFile( char* pRefFileName );

	afx_msg VOID	OnBnClickedClearButton();
	afx_msg VOID	OnBnClickedSettingButton();
	afx_msg VOID	OnCbnDropdownListCombo();
	afx_msg VOID	OnBnClickedConnectButton();
	afx_msg VOID	OnBnClickedFastRadio();
	afx_msg VOID	OnBnClickedNormalRadio();
	afx_msg VOID	OnBnClickedPatchfileremovebutton2();
	afx_msg VOID	OnBnClickedRealtimeScan();
	afx_msg VOID	OnBnClickedRealtimeStop();
	afx_msg VOID	OnBnClickedScanfolder();

	BOOL			CheckLimitTime	( FILETIME&	rFileTime );

	VOID			_WriteLog( const char* szFileName , const char* pData , ... );

	CDirectoryChangeWatcher				m_DirectoryWatcher;
	CDirectoryChangeHandler_LB			m_DirectoryHandler;
	

	CString								m_RealTimeFolder;
	CString								m_DesFolder;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedDeletebutton();
	
	afx_msg void OnNMCustomdrawPatchfileprogress(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif // !defined(AFX_PATCHER2DLG_H__9F561C0E_1B34_43A4_8315_385F822A5D82__INCLUDED_)
