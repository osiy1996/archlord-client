#include "StdAfx.h"

#ifdef _KOR

#include "KoreaPatchClient.h"
#include "Hyperlinks.h"
#include "PatchClientDlg.h"
#include "AuIgnoringHelper.h"

#include "ImageHlp.h"
#pragma comment( lib, "Imagehlp.lib" )


KoreaPatchClient::KoreaPatchClient( CDialog* pDlg )
:	BasePatchClient( pDlg )
{
	m_ulClientFileCRC = 0;
	memset( m_strPatchCodeString, 0, sizeof( char ) * LENGTH_PATCH_CODE_STRING );
}

KoreaPatchClient::~KoreaPatchClient(void)
{
}

BOOL KoreaPatchClient::OnInitDialog( VOID )
{
	wchar_t		strMsg[ 256 ];
	BITMAP		bitmap;
	CHAR		szBackFileName[ MAX_PATH ];

	ZeroMemory( szBackFileName , MAX_PATH );

	m_cPatchClientLib.LoadReferenceFile();

	//. Force setting Dialog size/position
	int nPosX = GetSystemMetrics ( SM_CXSCREEN );
	int nPosY = GetSystemMetrics ( SM_CYSCREEN );

	nPosX = ( nPosX / 2 ) - 296;
	nPosY = ( nPosY / 2 ) - 199;

	//. window size locale string table resource.
	m_pDlg->MoveWindow( nPosX , nPosY, 
						GetIntFromStringTable(IDS_WIDTHSIZE), 
						GetIntFromStringTable(IDS_HEIGHTSIZE));

	m_pDlg->ModifyStyle(0 , WS_CLIPCHILDREN );

	// â �̸� ����
	m_pDlg->SetWindowText( "Archlord Patch Client" );

	//��Ʈ����
	m_cDrawFont.CreateFont       ( 13, 0, 0, 0, FW_BOLD,   0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "����" );
	m_cDrawTitle.CreateFont      ( 12, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "����" );
	m_cDrawFontInfo.CreateFont   ( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "����" );
	m_cDrawHyperLink.CreateFont  ( 12, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "����" );
	m_cDrawProgressMsg.CreateFont( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "����" );

	m_cOldPoint.x = -1;
	m_cOldPoint.y = -1;

	m_cBMPBackground.Attach( LoadBitmapResource( "images\\BaseBackground.bm1" ) );
	m_cBMPBackground.GetBitmap( &bitmap );

	// 4 ���߿� �ϳ��� ���Ƿ� �����ִ� ��� �׸� �ε�
	unsigned int nRandomNum = (timeGetTime() % 4)+1;
	sprintf_s( szBackFileName , MAX_PATH , "images\\RandomBackground%02d.bm1" , nRandomNum);
	m_cBmpBackgroundEx.Attach( LoadBitmapResource(szBackFileName) );

	

	m_cBMPBackground.GetBitmap( &bitmap );

	m_pDlg->MoveWindow( nPosX , nPosY, bitmap.bmWidth , bitmap.bmHeight );
	
	CString	strJoin;
	CString	strOption;
	CString	strHomepage;
	CString	strStart;

	strJoin.LoadString(IDS_MSG_JOIN);
	strOption.LoadString(IDS_MSG_OPTION);
	strHomepage.LoadString(IDS_MSG_HOMEPAGE);
	strStart.LoadString(IDS_MSG_START);

	//Button
	m_cKbcExitButton.SetBmpButtonImage( IDB_Button_Close_Set, RGB(0,0,255) );
	m_cKbcOptionButton.SetBmpButtonImage( IDB_BUTTON_OPTION, RGB(0,0,255) );
	m_cKbcStartgameButton.SetBmpButtonImage( IDB_BUTTON_START, RGB(0,0,255) );

	CWnd		*pcTempWnd = 0;
	RECT		rect;

	pcTempWnd = m_pDlg->GetDlgItem( IDC_EXitButton );
	pcTempWnd->SetWindowPos( NULL, 
		bitmap.bmWidth	- 22	,
		GetIntFromStringTable(IDS_EXITBTNPOSY), 
		16, 
		16, 
		SWP_SHOWWINDOW );
	

	m_cKbcOptionButton.GetClientRect( &rect );
	pcTempWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_OPTIONBTNPOSX), 
		GetIntFromStringTable(IDS_OPTIONBTNPOSY), 
		rect.right	,
		rect.bottom	,
		SWP_SHOWWINDOW );


	m_cKbcStartgameButton.GetClientRect( &rect );
	pcTempWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_STARTBTNPOSX), 
		GetIntFromStringTable(IDS_STARTBTNPOSY), 
		rect.right	,
		rect.bottom	,
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_Static_DetailInfo );
	CStatic* pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawProgressMsg);
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_PROGRESSMSG1X), 
		GetIntFromStringTable(IDS_PROGRESSMSG1Y), 
		GetIntFromStringTable(IDS_PROGRESSMSGWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSMSGHEIGHT), 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem(	IDC_Static_Status );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawProgressMsg);
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_PROGRESSMSG1X), 
		GetIntFromStringTable(IDS_PROGRESSMSG2Y), 
		GetIntFromStringTable(IDS_PROGRESSMSGWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSMSGHEIGHT), 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_Static_Progress_Percent);
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawProgressMsg);
	pcTempWnd->SetWindowPos( NULL, 2000, 4000, 20, 10, SWP_SHOWWINDOW);


	//. Hyperlink static text ��ġ�� ��ũ����
	pcTempWnd = m_pDlg->GetDlgItem( IDC_HOMEPAGE );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawHyperLink);
	pcTempWnd->SetWindowPos( NULL, 563, 231, 49, 12, SWP_SHOWWINDOW );
	ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_HOMEPAGE);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_JOIN );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 410, 27, 49, 12, SWP_SHOWWINDOW );
	//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_JOIN);

	pcTempWnd = m_pDlg->GetDlgItem( IDC_CUSTOMER );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawHyperLink);
	pcTempWnd->SetWindowPos( NULL, 627, 231, 49, 12, SWP_SHOWWINDOW );
	ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_CUSTOMER);

	pcTempWnd = m_pDlg->GetDlgItem( IDC_DRIVER );
	pStaticText = (CStatic*)pcTempWnd;
	pStaticText->SetFont(&m_cDrawHyperLink);
	pcTempWnd->SetWindowPos( NULL, 691, 231, 49, 12, SWP_SHOWWINDOW );
	ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_DRIVER);

	//pcTempWnd = m_pDlg->GetDlgItem( IDC_LINE );
	//pStaticText = (CStatic*)pcTempWnd;
	//pStaticText->SetFont(&m_cDrawHyperLink);
	//pcTempWnd->SetWindowPos( NULL, 400, 27, 200, 12, SWP_SHOWWINDOW );
	
	//ini\\COption.ini�� �д´�.
	m_cPatchOptionFile.Init();
	m_cPatchOptionFile.Load( PATCHCLIENT_OPTION_FILENAME );

	//m_staticMainBack.Create( "Main Back" , WS_CHILD | WS_VISIBLE | SS_BITMAP | WS_CLIPCHILDREN , rect , m_pDlg  );
	//m_staticMainBack.SetWindowPos( NULL , 0 , 0 , bitmap.bmWidth , bitmap.bmHeight , NULL );
	//m_staticMainBack.ShowWindow( SW_SHOW );

	HDC	hdc		=	GetDC( m_pDlg->m_hWnd );

	// ���÷��� DC�� ȣȯ�Ǵ� ��Ʈ�ʰ� DC �����
	m_hbmBackBuffer			= CreateCompatibleBitmap(hdc, bitmap.bmWidth,bitmap.bmHeight);
	m_hdcBack				= CreateCompatibleDC(hdc);

	// DC ���� �Ӽ� ����
	m_hbmOld				= (HBITMAP)SelectObject(m_hdcBack, m_hbmBackBuffer);	

	//Html�� ���δ�.
	CCreateContext pContext;
	CWnd* pFrameWnd = m_pDlg;

	pContext.m_pCurrentDoc = NULL;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CTestHtmlView);

	m_pWeb =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);
	ASSERT(m_pWeb);

	//. ��ũ�ѹٸ� ���ش�.
	//m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL, 0, 0 );
	//m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR, 0, 0 );
	//m_pWeb->SetWindowPos(NULL,380,390,424,128,NULL);
	//m_pWeb->ShowWindow(SW_HIDE);
	//m_pWeb->Navigate(m_cPatchOptionFile.GetNoticeURL(), 0, NULL, NULL);		
	///////////////////////////////////////////////////////////////////////////////////

	m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL  , 0 , 0 );
	m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR , 0 , 0 );
	m_pWeb->SetWindowPos( NULL , 4 , 23 , 786 , 504 , NULL );
	m_pWeb->ShowWindow( SW_HIDE );
	//m_pWeb->Navigate( "http://archlord.webzen.co.kr/news/notice/list.aspx" );
	m_pWeb->Navigate( "http://archlord.webzen.co.kr/launcher/" );
	m_pWeb->SetParent( m_pDlg );

	m_staticBackColor.Create( "Back Rotate" , WS_CHILD | WS_VISIBLE | SS_BITMAP  , rect , m_pWeb );
	m_staticBackColor.SetBitmap( m_cBmpBackgroundEx );
	m_staticBackColor.SetWindowPos( NULL , 0 , 0 , 787 , 190 , NULL );
	m_staticBackColor.ShowWindow( SW_SHOW );

	/*m_pWebAdvertisement	=	(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
	ASSERT( m_pWebAdvertisement );
	m_pWebAdvertisement->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL , 0 , 0 );
	m_pWebAdvertisement->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR , 0 , 0 );
	m_pWebAdvertisement->SetWindowPos( NULL , 52 , 254 , 321 ,241 , NULL );
	m_pWebAdvertisement->ShowWindow( SW_HIDE );
	m_pWebAdvertisement->Navigate( ARCHLORD_ADVERTISEMENT );

	for( INT i = 0 ; i < 3 ; ++i )
	{
		m_pWebInside[ i ]	=	(CTestHtmlView*)((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
		ASSERT( m_pWebInside[ i ] );
		m_pWebInside[ i ]->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL , 0 , 0 );
		m_pWebInside[ i ]->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR , 0 , 0 );
		m_pWebInside[ i ]->SetWindowPos( NULL, 416 + 106*i , 287 , 97 , 60 , NULL );
		m_pWebInside[ i	]->ShowWindow( SW_HIDE );
		m_pWebInside[ i ]->Navigate( m_cPatchOptionFile.GetNoticeURL() );
	}*/

	//�� �������� �� �ð� 0.5�ʰ� ��ٷ��ش�.
	Sleep( 500 );

	//TempDir�� �������ش�.
	if( m_cPatchClientLib.SetTempDir( "Temp" ) == false )
	{
		::LoadStringW(g_resInst, IDS_ERROR_MAKE_PATCH_FOLDER, strMsg, 256);
		MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
		return FALSE;
	}

	//. update/dowinload Progress bar 
	CRect			cTempRect;

	//. update progress bar.
	::SetRect( &cTempRect, 
		GetIntFromStringTable(IDS_PROGRESSBAR1X), 
		GetIntFromStringTable(IDS_PROGRESSBAR1Y),
		GetIntFromStringTable(IDS_PROGRESSBAR1X) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSBAR1Y) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH) );
	m_cCustomProgressRecvBlock.Init( &cTempRect, 0 );

	// download progress bar.
	::SetRect( &cTempRect, 
		GetIntFromStringTable(IDS_PROGRESSBAR1X), 
		GetIntFromStringTable(IDS_PROGRESSBAR2Y),
		GetIntFromStringTable(IDS_PROGRESSBAR1X) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH), 
		GetIntFromStringTable(IDS_PROGRESSBAR2Y) + GetIntFromStringTable(IDS_PROGRESSBARWIDTH) );
	m_cCustomProgressRecvFile.Init( &cTempRect, 0 );

	m_StaticStatus.ShowWindow( SW_HIDE );
	m_StaticDetailInfo.ShowWindow( SW_HIDE );


	//��ġ�����߿� ���α׷����ٸ� �׸������� �޾Ҵ�. -_-;
	m_cPatchClientLib.SetDialogInfo( m_pDlg->FromHandle( m_pDlg->GetSafeHwnd() ), &m_cCustomProgressRecvFile, &m_cCustomProgressRecvBlock, &m_StaticStatus, NULL, &m_StaticDetailInfo );

	m_pDlg->SetActiveWindow();
	m_pDlg->SetTimer( KOREA_TIMER, 180000, NULL );

	m_cPatchClientLib.ReadConfig("Ini\\PatchConfig.ini");

	PatchThreadStart();

	return TRUE;
}

VOID KoreaPatchClient::OnPaint( VOID )
{
	BITMAP		bitmap;
	BITMAP		bitmapBack;
	CDC			memDC;
	CDC			BackDC;
	CRect		rect;
	memDC.Attach( CreateCompatibleDC( m_hdcBack ) );
	
	HBITMAP		hbmOld	=	(HBITMAP)memDC.SelectObject( m_cBMPBackground );

	m_cBMPBackground.GetBitmap( &bitmap );

	BitBlt(m_hdcBack, 0, 0, bitmap.bmWidth, bitmap.bmHeight , memDC, 0 , 0 , SRCCOPY);

	BackDC.Attach( m_hdcBack );
	m_cCustomProgressRecvBlock.Draw( &BackDC  );
	m_cCustomProgressRecvFile.Draw( &BackDC   );

	CStringW strWindowText;
	

	BackDC.SetBkMode( TRANSPARENT );
	BackDC.SetTextColor( RGB(255,255,255) );

	(CFont*)BackDC.SelectObject( m_StaticDetailInfo.GetFont() );
	
	strWindowText	=	m_StaticDetailInfo.GetText();
	DrawTextW( BackDC , strWindowText , strWindowText.GetLength() , m_cCustomProgressRecvBlock.GetRect() , DT_CENTER );

	strWindowText	=	m_StaticStatus.GetText();
	DrawTextW( BackDC , strWindowText	, strWindowText.GetLength() , m_cCustomProgressRecvFile.GetRect() , DT_CENTER );

	memDC.SelectObject(hbmOld);

	HDC hdc = GetDC(m_pDlg->m_hWnd);
	BitBlt(hdc, 0 , 0 ,  bitmap.bmWidth ,bitmap.bmHeight, BackDC,  0 , 0 , SRCCOPY);

	BackDC.Detach();
	memDC.Detach();

	ReleaseDC(m_pDlg->m_hWnd, hdc);
	
}

VOID KoreaPatchClient::OnTimer( UINT nIDEvent )
{
	if( nIDEvent == 5 )
	{
		m_pDlg->InvalidateRect( m_cCustomProgressRecvFile.GetRect(), FALSE );
		m_pDlg->InvalidateRect( m_cCustomProgressRecvBlock.GetRect(), FALSE );
	}
}

VOID KoreaPatchClient::DoDataExchange( CDataExchange* pDX )
{
	DDX_Control(pDX, IDC_Static_Status		, m_StaticStatus		);
	DDX_Control(pDX, IDC_Static_DetailInfo	, m_StaticDetailInfo	);
	DDX_Control(pDX, IDC_EXitButton			, m_cKbcExitButton		);
	DDX_Control(pDX, IDC_RegisterButton		, m_cKbcRegisterButton	);
	DDX_Control(pDX, IDC_OptionButton		, m_cKbcOptionButton	);
	DDX_Control(pDX, IDC_HomepageButton		, m_cKbcHomepageButton	);
	DDX_Control(pDX, IDC_START_GAME			, m_cKbcStartgameButton );
}

VOID KoreaPatchClient::OnStartGame( VOID )
{
	bool checkCRC = true;

	if( AuIgnoringHelper::IgnoreToFile( "NotCheckCRC.arc" ) )
	{
		checkCRC = false;
	}

	if( checkCRC )
	{
		// ���������� CRC üũ�� ���� ������ CRC �� ��û�ϰ� ��ٸ���.
		if( !_GetClientFileCRCFromPatchServer() )
		{
			char strMessage[ 256 ] = { 0, };
			::LoadString( NULL, IDS_ERROR_PATCH_CONNECTION, strMessage, sizeof( char ) * 256 );
			::MessageBox( NULL, strMessage, "Network Error", MB_OK );

			// ��ġŬ���̾�Ʈ�� �����Ų��.
			DoCloseDialog();
		}
	}
	else
	{
		DoStartGame();
		DoCloseDialog();
	}
}

BOOL KoreaPatchClient::OnReceiveClientFileCRC( void* pPacket )
{
	DisConnect();

	CPacketTypeCS_0x05* ppdPacket = ( CPacketTypeCS_0x05* )pPacket;
	if( !ppdPacket ) return FALSE;

	m_ulClientFileCRC = ppdPacket->m_ulClientFileCRC;

	// �����κ��� CRC ������ �޾����� CRC �˻縦 �����Ѵ�.
	if( !_CheckClientFileCRC( "AlefClient.exe", m_ulClientFileCRC ) )
	{
		FILE * f = fopen( "NotCheckCRC.arc", "rb" );

		if( !f )
		{
			char strMessage[ 256 ] = { 0, };
			::LoadString( NULL, IDS_ERROR_CLIENTFILE_CURRUPTED, strMessage, sizeof( char ) * 256 );
			::MessageBox( NULL, strMessage, "Client File Error!", MB_OK );

			// ��ġŬ���̾�Ʈ�� �����Ų��.
			DoCloseDialog();
			return TRUE;
		}
		else
		{
			fclose( f );
		}		
	}

	// ��ġŬ���̾�Ʈ�� ����Ǿ����� �ٽ� �����϶�� �޼����� ������ �����Ѵ�.
	if( _IsUpdatePatchClientFile( "Archlord.bak" ) )
	{
		char strMessage[ 256 ] = { 0, };
		::LoadString( NULL, IDS_NOTICE_DO_RESTART, strMessage, sizeof( char ) * 256 );
		::MessageBox( NULL, strMessage, "Notice", MB_OK );

		// ��ġŬ���̾�Ʈ�� �����Ų��.
		DoCloseDialog();
		return TRUE;
	}

	// ������ ���� Ŭ���̾�Ʈ�� �⵿��Ų��.
	DoStartGame();
	return TRUE;
}

BOOL KoreaPatchClient::DoStartGame( void )
{
	// �������Ͽ� �Ѱ��� ��ġ�ڵ带 ����..
	if( !_MakePatchCodeString() ) return FALSE;

	// �������Ͽ� ��ġ�ڵ带 �Ѱܼ� �����Ű��,..
	m_cPatchClientLib.StartGame( ( char* )m_strPatchCodeString );

	// ��ġŬ���̾�Ʈ�� �����Ų��.
	DoCloseDialog();
	return TRUE;
}

BOOL KoreaPatchClient::DoCloseDialog( void )
{
	( ( CPatchClientDlg* )m_pDlg )->CloseDlg();
	return TRUE;
}

BOOL KoreaPatchClient::_GetClientFileCRCFromPatchServer( void )
{
	if( !Connect() ) return FALSE;
	if( !SendRequestClientFileCRC() ) return FALSE;
	if( !Listen( false ) ) return FALSE;
	return TRUE;
}

BOOL KoreaPatchClient::_CheckClientFileCRC( char* pFileName, unsigned long ulCRC )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	// NotCheckPatchCode.arc ������ �����ϸ� �����Ѵ�.
	if( IsExistFile( "NotCheckPatchCode.arc" ) ) return TRUE;

	char strFullPathFileName[ 256 ] = { 0, };
	::GetCurrentDirectory( 256, strFullPathFileName );
	strcat_s( strFullPathFileName, sizeof( char ) * 256, "\\" );
	strcat_s( strFullPathFileName, sizeof( char ) * 256, pFileName );

	DWORD dwHeaderSum = 0;
	DWORD dwCheckSum = 0;

	DWORD dwResult = ::MapFileAndCheckSum( strFullPathFileName, &dwHeaderSum, &dwCheckSum );
	if( dwResult != CHECKSUM_SUCCESS ) return FALSE;

	return ulCRC == dwCheckSum ? TRUE : FALSE;
}

BOOL KoreaPatchClient::_MakePatchCodeString( void )
{
	HKEY hKey = NULL;
	//LONG nResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\ArchLord_KR_Alpha", 0, KEY_READ, &hKey );
	//LONG nResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\ArchLord_KR", 0, KEY_READ, &hKey );
	LONG nResult = ::RegOpenKeyEx( HKEY_CURRENT_USER, m_cPatchClientLib.m_cPatchReg.GetRegKeyBase(), 0, KEY_READ, &hKey );

	
	if( nResult != ERROR_SUCCESS ) return FALSE;

	DWORD dwValueType = 0;
	DWORD dwValue = 0;
	DWORD dwValueLength = sizeof( DWORD );

	nResult = ::RegQueryValueEx( hKey, "Code", 0, &dwValueType, ( LPBYTE )( &dwValue ), &dwValueLength );
	if( nResult != ERROR_SUCCESS ) return FALSE;

	RegCloseKey( hKey );

	/*
	DWORD dwTime = timeGetTime();
	memset( m_strPatchCodeString, 0, sizeof( char ) * LENGTH_PATCH_CODE_STRING );
	sprintf_s( m_strPatchCodeString, sizeof( char ) * LENGTH_PATCH_CODE_STRING, "%lu", dwTime );

	int nTimeLength = ( int )strlen( m_strPatchCodeString );
	memset( m_strPatchCodeString, 0, sizeof( char ) * LENGTH_PATCH_CODE_STRING );
	sprintf_s( m_strPatchCodeString, sizeof( char ) * LENGTH_PATCH_CODE_STRING, "%02d%lu%lu", nTimeLength, dwTime, dwValue );

	int nCodeLength = ( int )strlen( m_strPatchCodeString );
	for( int nCount = 0 ; nCount < nCodeLength ; nCount++ )
	{
		if( nCount != 0 && nCount % 2 == 0 )
		{
			char cTemp = m_strPatchCodeString[ nCount - 1 ];
			m_strPatchCodeString[ nCount - 1 ] = m_strPatchCodeString[ nCount ];
			m_strPatchCodeString[ nCount ] = cTemp;
		}
	}
	*/

	memset( m_strPatchCodeString, 0, sizeof(m_strPatchCodeString) );

	sprintf_s( m_strPatchCodeString, "%u", dwValue );

	return TRUE;
}

BOOL KoreaPatchClient::_IsUpdatePatchClientFile( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;
	BOOL bHaveNewPatchClientFile = FALSE;

	// ������ �����ϴ°�
	FILE* pFile = ::fopen( pFileName, "rb" );
	if( pFile )
	{
		bHaveNewPatchClientFile = TRUE;
		::fclose( pFile );
	}

	if( IsUpdatePatchClient() && bHaveNewPatchClientFile ) return TRUE;
	return FALSE;
}

#endif