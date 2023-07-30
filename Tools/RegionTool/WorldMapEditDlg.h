#pragma once


// CWorldMapEditDlg ��ȭ �����Դϴ�.

class CWorldMapEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorldMapEditDlg)

public:
	CWorldMapEditDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CWorldMapEditDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WORLDMAP_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectItemId();
	afx_msg void OnBnClickedCalccoodinate();
	
public:
	float		m_fStartX;
	float		m_fStartZ;
	float		m_fEndX;
	float		m_fEndZ;
	int			m_nIndex;
	CString		m_strComment;
	CString		m_strItemID;
	ApmMap::WorldMap*	m_pstWorldMap;
public:
	afx_msg void OnBnClickedCalccoodinate2();
};
