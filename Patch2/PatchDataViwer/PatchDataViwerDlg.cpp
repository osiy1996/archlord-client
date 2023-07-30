// PatchDataViwerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "PatchDataViwer.h"
#include "PatchDataViwerDlg.h"
#include ".\patchdataviwerdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPatchDataViwerDlg ��ȭ ����



CPatchDataViwerDlg::CPatchDataViwerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchDataViwerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPatchDataViwerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Check_PackingExtractor_SubDir, m_cPackingExtractSubDir);
	DDX_Control(pDX, IDC_Check_Compare_SubDir, m_cCompareSubDir);
	DDX_Control(pDX, IDC_Static_JZPExtractor_SourcePath, m_cJZPExtractorScrPath);
	DDX_Control(pDX, IDC_Edit_JZPExtractor_DestPath, m_cJZPExtractorDestPath);
	DDX_Control(pDX, IDC_Static_PackingExtractor_ScrPath, m_cDatExtractorScrPath);
	DDX_Control(pDX, IDC_Edit_PackingExtractor_DestPath, m_cDatExtractorDestPath);
	DDX_Control(pDX, IDC_Static_Compare_SrcPath, m_cCompareScrPath);
	DDX_Control(pDX, IDC_Static_Compare_DestPath, m_cCompareDestPath);
}

BEGIN_MESSAGE_MAP(CPatchDataViwerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_Button_JZPExtractor_Scan_SrcFile, OnBnClickedButtonJzpextractorScanSrcfile)
	ON_BN_CLICKED(IDC_Button_JZPExtractor_Scan_DestPath, OnBnClickedButtonJzpextractorScanDestpath)
	ON_BN_CLICKED(IDC_Button_JZPExtractor, OnBnClickedButtonJzpextractor)
	ON_BN_CLICKED(IDC_Button_PackingExtractor_Scan_SrcPath, OnBnClickedButtonPackingextractorScanSrcpath)
	ON_BN_CLICKED(IDC_Button_PackingExtractor_Scan_DestPath, OnBnClickedButtonPackingextractorScanDestpath)
	ON_BN_CLICKED(IDC_Button_PackingExtractor, OnBnClickedButtonPackingextractor)
	ON_BN_CLICKED(IDC_Button_Compare_Scan_SrcPath, OnBnClickedButtonCompareScanSrcpath)
	ON_BN_CLICKED(IDC_Button_Compare_Scan_DestPath, OnBnClickedButtonCompareScanDestpath)
	ON_BN_CLICKED(IDC_Button_Compare, OnBnClickedButtonCompare)
	ON_BN_CLICKED(IDC_Button_Exit, OnBnClickedButtonExit)
END_MESSAGE_MAP()


// CPatchDataViwerDlg �޽��� ó����

BOOL CPatchDataViwerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	memset( m_strJZPSrcPath, 0, sizeof(m_strJZPSrcPath) );
	memset( m_strJZPDestPath, 0, sizeof(m_strJZPDestPath) );

	memset( m_strPackingSrcPath, 0, sizeof(m_strPackingSrcPath) );
	memset( m_strPackingDestPath, 0, sizeof(m_strPackingDestPath) );

	memset( m_strCompareSrcPath, 0, sizeof(m_strCompareSrcPath) );
	memset( m_strCompareDestPath, 0, sizeof(m_strCompareDestPath) );
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CPatchDataViwerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CPatchDataViwerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CPatchDataViwerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CPatchDataViwerDlg::ScanFolder( char *pstrFolder )
{
	// TODO: Add your control notification handler code here
	bool			bResult;

	bResult = false;

	if( pstrFolder )
	{
		LPITEMIDLIST pidlRoot = NULL;
		LPITEMIDLIST pidlSelected = NULL;
		BROWSEINFO bi = {0};
		LPMALLOC pMalloc = NULL;
		char		pszDisplayName[256];

		SHGetMalloc(&pMalloc);

		bi.hwndOwner = this->m_hWnd;
		bi.pidlRoot = pidlRoot;
		bi.pszDisplayName = pszDisplayName;
		bi.lpszTitle = "Choose a Folder";
		bi.ulFlags = 0;
		bi.lpfn = NULL;
		bi.lParam = 0;

		pidlSelected = SHBrowseForFolder(&bi);

		if(pidlRoot)
		{
			pMalloc->Free(pidlRoot);
		}

		pMalloc->Release();

		if( pidlSelected != NULL )
		{
			SHGetPathFromIDList(pidlSelected, pstrFolder );
			bResult = true;
		}
	}

    return bResult;
}

bool CPatchDataViwerDlg::DepressJZP( char *pstrJZPFileName, char *pstrDestPath )
{
	FILE			*file;
	bool			bResult;
	bool			bPacking;
	int				lVersion;
	int				lMaxFolderCount;
	int				lFolderCount;
	int				lMaxFileCount;
	int				lFileCount;
	int				lTotalFileCount;

	int				lFolderNameLen;
	char			*pstrFolderName;
	char			strCurrentDirectory[255];

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );

	SetCurrentDirectory( pstrDestPath );

	if( pstrJZPFileName )
	{
		file = fopen( pstrJZPFileName, "rb" );

		if( file )
		{
			//����
			fread( &lVersion, 1, sizeof(int), file );

			//��ü ���ϰ����� �о��.
			fread( &lTotalFileCount, 1, sizeof(int), file );

			//���� ����
			fread( &lMaxFolderCount, 1, sizeof(int), file );

			for( lFolderCount=0; lFolderCount<lMaxFolderCount; lFolderCount++ )
			{
				//�����̸� ���̷ε�
				fread( &lFolderNameLen, 1, sizeof(int), file );

				//�����̸� �ε�
				pstrFolderName = new char[lFolderNameLen];
				memset( pstrFolderName, 0, lFolderNameLen );
				fread( pstrFolderName, 1, lFolderNameLen, file );

				//�������̶�� �����Ѵ�.
				m_csAuPackingManager.MarkPacking( pstrFolderName );

				//���ϰ������
				fread( &lMaxFileCount, 1, sizeof(int), file );

				bool bPackingFolder;
				//�� ������ ��ŷ�� �ϴ� ������ �����ϴ°�?
				fread( &bPackingFolder, 1, sizeof(bool), file );

				for( lFileCount=0; lFileCount<lMaxFileCount ;lFileCount++ )
				{
					int				lOperation;
					int				lFileNameLength;
					int				lFileSize;
					char			*pstrFileName;
					char			*pstrBuffer;

					pstrFileName = NULL;
					pstrBuffer = NULL;

					fread( &bPacking, 1, sizeof(bool), file );

					//Operation
					fread( &lOperation, 1, sizeof(int), file );

					//FileNameLength
					fread( &lFileNameLength, 1, sizeof(int), file );

					//FileName
					pstrFileName = new char[lFileNameLength];
					memset( pstrFileName, 0, lFileNameLength );
					fread(pstrFileName, 1, lFileNameLength, file );

					FILE *fp	= fopen("JZPFiles.txt", "at");
					fprintf(fp, pstrFileName);
					fclose(fp);

					if( lOperation == Packing_Operation_Add )
					{
						//FileSize
						fread( &lFileSize, 1, sizeof(int), file );

						pstrBuffer = new char[lFileSize];

						//FileData
						fread( pstrBuffer, 1, lFileSize, file );

						//Packing�̸� Data.Dat�� �߰��Ѵ�.
						if( bPacking == true )
						{
							if( !strcmpi( "root", pstrFolderName ) )
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, pstrFileName );
							}
							else
							{
								char			strDestDir[255];

								sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
								m_csAuPackingManager.CreateFolder( pstrFolderName );

								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, strDestDir );
							}
						}
						//Packing�� �ƴϸ� �ش� ������ �׳� ������ �����Ѵ�.
						else
						{
							if( !strcmpi( "root", pstrFolderName ) )
							{
								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, pstrFileName );
							}
							else
							{
								char			strDestDir[255];

								sprintf( strDestDir, "%s\\%s", pstrFolderName, pstrFileName );
								m_csAuPackingManager.CreateFolder( pstrFolderName );

								m_cCompress.decompressMemory( pstrBuffer, lFileSize, pstrDestPath, strDestDir );
							}
						}
					}
					//�������� ����.
					else
					{
					}

					if( pstrFileName != NULL )
						delete [] pstrFileName;
					if( pstrBuffer != NULL )
						delete [] pstrBuffer;
				}
			}
		}

		bResult = true;
		fclose( file );
	}

	SetCurrentDirectory( strCurrentDirectory );

	return bResult;
}

bool CPatchDataViwerDlg::DepressDat( char *pstrSrcPath, char *pstrDestPath )
{
	ApAdmin			*pcsAdminFolderInfo;
	bool			bResult;
	char			strCurrentDirectory[255];

	bResult = false;

	GetCurrentDirectory( sizeof(strCurrentDirectory), strCurrentDirectory );

	SetCurrentDirectory( pstrSrcPath );	

	m_csAuPackingManager.LoadReferenceFile( pstrSrcPath, true, true ); //Read only, Load SubDir
	//m_csAuPackingManager.setLoadFromPackingData(true);
	//m_csAuPackingManager.m_bLoadFromRawFile = false;

	m_csAuPackingManager.SetFilePointer( m_cPackingExtractSubDir.GetCheck() );

	pcsAdminFolderInfo = m_csAuPackingManager.GetFolderInfo();

	if( pcsAdminFolderInfo )
	{
		CPackingFolder	**ppcsPackingFolder;
		int				lFolderIndex;

		lFolderIndex = 0;

		for( ppcsPackingFolder = (CPackingFolder **)pcsAdminFolderInfo->GetObjectSequence( &lFolderIndex ); ppcsPackingFolder; ppcsPackingFolder = (CPackingFolder **)pcsAdminFolderInfo->GetObjectSequence( &lFolderIndex ) )
		{
			if( ppcsPackingFolder && (*ppcsPackingFolder) )
			{
				CPackingFile	**ppcsPackingFile;
				int				lFileIndex;

				lFileIndex = 0;

				for( ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex); ppcsPackingFile; ppcsPackingFile = (CPackingFile **)(*ppcsPackingFolder)->m_csFilesInfo.GetObjectSequence(&lFileIndex) )
				{
                    if( ppcsPackingFile && (*ppcsPackingFile) )
					{
						ApdFile			csApdFile;

						if( m_csAuPackingManager.OpenFile( (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName, &csApdFile ) )
						{
							int				lFileSize;

							lFileSize = m_csAuPackingManager.GetFileSize( &csApdFile );

							if( lFileSize )
							{
								FILE			*file;
								char			*pstrBuffer;
								char			strDestPath[255];

								pstrBuffer = new char[lFileSize];

                                m_csAuPackingManager.ReadFile( pstrBuffer, lFileSize, &csApdFile );

								//���������� ������ ���丮�μ����Ѵ�.
								SetCurrentDirectory( pstrDestPath );	

								//�о���� ���� �ϵ��ũ�� �����Ѵ�.
								if( !stricmp( "root", (*ppcsPackingFolder)->m_pstrFolderName ) )
								{
									sprintf( strDestPath, "%s", (*ppcsPackingFile)->m_pstrFileName );
								}
								else
								{
									m_csAuPackingManager.CreateFolder( (*ppcsPackingFolder)->m_pstrFolderName );

									sprintf( strDestPath, "%s\\%s", (*ppcsPackingFolder)->m_pstrFolderName, (*ppcsPackingFile)->m_pstrFileName );
								}

								file = fopen( strDestPath, "wb" );

								if( file )
								{
                                    fwrite( pstrBuffer, 1, lFileSize, file );

									fclose( file );
								}

								delete [] pstrBuffer;

								//�� ������ �ҽ� ���丮�� ������.
								SetCurrentDirectory( pstrSrcPath );	
							}

							m_csAuPackingManager.CloseFile( &csApdFile );
						}
					}
				}
			}
		}

		bResult = true;
	}

	SetCurrentDirectory( strCurrentDirectory );

	return bResult;
}

void CPatchDataViwerDlg::OnBnClickedButtonJzpextractorScanSrcfile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog				cFileDialog( TRUE, "*.JZP", "*.JZP" );
	CString					cFilePath;

	if( cFileDialog.DoModal() == IDOK )
	{
		cFilePath = cFileDialog.GetPathName();

		sprintf( m_strJZPSrcPath, "%s", cFilePath.GetBuffer() );

		m_cJZPExtractorScrPath.SetWindowText( m_strJZPSrcPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonJzpextractorScanDestpath()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ScanFolder( m_strJZPDestPath ) )
	{
		m_cJZPExtractorDestPath.SetWindowText( m_strJZPDestPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonJzpextractor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_cJZPExtractorDestPath.GetWindowTextLength() )
	{
		char			strBuffer[255];
		
		memset( strBuffer, 0, sizeof(strBuffer) );
		m_cJZPExtractorDestPath.GetWindowText(strBuffer, sizeof(strBuffer) );

		sprintf( m_strJZPDestPath, "%s", strBuffer );
	}

	if( (strlen( m_strJZPSrcPath ) == 0) || (strlen(m_strJZPDestPath) == 0 ) )
	{
		MessageBox( "������ ��� �������ֽʽÿ�." );
	}
	else
	{
		if( !strcmp( m_strJZPSrcPath, m_strJZPDestPath ) )
		{
			MessageBox( "���� ������ JZP������ Ǯ�� �����ϴ�." );
		}
		else
		{
			if( DepressJZP( m_strJZPSrcPath, m_strJZPDestPath ) )
			{
				MessageBox( "JZP���� Ǯ�� ����" );
			}
			else
			{
				MessageBox( "JZP���� Ǯ�� ����" );
			}
		}
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonPackingextractorScanSrcpath()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ScanFolder( m_strPackingSrcPath ) )
	{
		m_cDatExtractorScrPath.SetWindowText( m_strPackingSrcPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonPackingextractorScanDestpath()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ScanFolder( m_strPackingDestPath ) )
	{
		m_cDatExtractorDestPath.SetWindowText( m_strPackingDestPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonPackingextractor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_cDatExtractorDestPath.GetWindowTextLength() )
	{
		char			strBuffer[255];
		
		memset( strBuffer, 0, sizeof(strBuffer) );
		m_cDatExtractorDestPath.GetWindowText(strBuffer, sizeof(strBuffer) );

		sprintf( m_strPackingDestPath, "%s", strBuffer );
	}

	if( (strlen( m_strPackingSrcPath ) == 0) || (strlen(m_strPackingDestPath) == 0 ) )
	{
		MessageBox( "������ ��� �������ֽʽÿ�." );
	}
	else
	{
		if( !strcmp( m_strPackingSrcPath, m_strPackingDestPath ) )
		{
			MessageBox( "���� ������ Dat������ Ǯ�� �����ϴ�." );
		}
		else
		{
			if( DepressDat( m_strPackingSrcPath, m_strPackingDestPath ) )
			{
				MessageBox( "Dat���� Ǯ�� ����" );
			}
			else
			{
				MessageBox( "Dat���� Ǯ�� ����" );
			}
		}
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonCompareScanSrcpath()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ScanFolder( m_strCompareSrcPath ) )
	{
		m_cCompareScrPath.SetWindowText( m_strCompareSrcPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonCompareScanDestpath()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( ScanFolder( m_strCompareDestPath ) )
	{
		m_cCompareDestPath.SetWindowText( m_strCompareDestPath );
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonCompare()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( (strlen( m_strCompareSrcPath ) == 0) || (strlen(m_strCompareDestPath) == 0 ) )
	{
		MessageBox( "������ ��� �������ֽʽÿ�." );
	}
	else
	{
		if( !strcmp( m_strCompareSrcPath, m_strCompareDestPath ) )
		{
			MessageBox( "���� ������ ���� �� �����ϴ�." );
		}
	}
}

void CPatchDataViwerDlg::OnBnClickedButtonExit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnOK();
}
