#pragma once


// CUploadDataDlg ��ȭ �����Դϴ�.

class CUploadDataDlg : public CDialog
{
	DECLARE_DYNAMIC(CUploadDataDlg)

public:
	CUploadDataDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CUploadDataDlg();


	enum { IDD = IDD_UPLOAD_DATA };

	CString m_strTitle;
	INT32	DoModal( BOOL bUpload );

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bAI2Template;
	BOOL m_bArtist;
	BOOL m_bCharacterTemplate;
	BOOL m_bDesign;
	BOOL m_bObjectTemplate;
	BOOL m_bRegionTool;
	BOOL m_bSkySet;
	BOOL m_bSpawnGroup;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
