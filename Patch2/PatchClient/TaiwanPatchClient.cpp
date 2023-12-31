#include "StdAfx.h"

#ifdef _TIW

#include "TaiwanPatchClient.h"
#include "Hyperlinks.h"
#include "PatchClientDlg.h"
#include "AuIgnoringHelper.h"

#include "ImageHlp.h"
#pragma comment( lib, "Imagehlp.lib" )


#define TAIWAN_WEB_LEFT_URL "http://al.wpark.com.tw/LUNCHER/left.aspx"
#define TAIWAN_WEB_RIGHT_URL "http://al.wpark.com.tw/LUNCHER/right.aspx"


TaiwanPatchClient::TaiwanPatchClient( CDialog* pDlg )
:	BasePatchClient( pDlg )
{
	m_ulClientFileCRC = 0;
	memset( m_strPatchCodeString, 0, sizeof( char ) * LENGTH_PATCH_CODE_STRING );
}

TaiwanPatchClient::~TaiwanPatchClient(void)
{
}

BOOL TaiwanPatchClient::OnInitDialog( VOID )
{
	wchar_t		strMsg[ 256 ];
	BITMAP		bitmap;
	CHAR		szBackFileName[ MAX_PATH ];
	ZeroMemory( szBackFileName , MAX_PATH );

	m_cPatchClientLib.LoadReferenceFile();

	m_cBMPBackground.Attach( LoadBitmapResource( "images\\BaseBackground.bm1" ) );
	m_cBMPBackground.GetBitmap( &bitmap );

	// 4 개중에 하나를 임의로 보여주는 배경 그림 로드
	unsigned int nRandomNum = (timeGetTime() % 4)+1;
	sprintf_s( szBackFileName , MAX_PATH , "images\\RandomBackground%02d.bm1" , nRandomNum);
	m_cBmpBackgroundEx.Attach( LoadBitmapResource(szBackFileName) );

	HDC	hdc		=	GetDC( m_pDlg->m_hWnd );

	// 디스플레이 DC와 호환되는 비트맵과 DC 만들기
	m_hbmBackBuffer			= CreateCompatibleBitmap(hdc, bitmap.bmWidth,bitmap.bmHeight);
	m_hdcBack				= CreateCompatibleDC(hdc);

	// DC 내부 속성 설정
	m_hbmOld				= (HBITMAP)SelectObject(m_hdcBack, m_hbmBackBuffer);	


	m_pDlg->ModifyStyle( 0 , WS_CLIPCHILDREN );


	//. Force setting Dialog size/position
	int nPosX = GetSystemMetrics ( SM_CXSCREEN );
	int nPosY = GetSystemMetrics ( SM_CYSCREEN );

	nPosX = ( nPosX / 2 ) - (bitmap.bmWidth/2);
	nPosY = ( nPosY / 2 ) - (bitmap.bmHeight/2);

	//. window size locale string table resource.
	m_pDlg->MoveWindow( nPosX , nPosY, 
		bitmap.bmWidth, 
		bitmap.bmHeight);

	m_cKbcExitButton.SetBmpButtonImage( IDB_Button_Close_Set, RGB(0,0,255) );
	m_cKbcOptionButton.SetBmpButtonImage( IDB_BUTTON_OPTION_SET, RGB(0,0,255) );
	m_cKbcStartgameButton.SetBmpButtonImage( IDB_BUTTON_STARTGAME_SET, RGB(0,0,255) );

	m_cDrawFont.CreateFont       ( 13, 0, 0, 0, FW_BOLD,   0, 0, 0, THAI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "PMing LiU" );
	m_cDrawTitle.CreateFont      ( 15, 0, 0, 0, FW_NORMAL, 0, 0, 0, THAI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "PMing LiU" );
	m_cDrawFontInfo.CreateFont   ( 12, 0, 0, 0, FW_NORMAL, 0, 0, 0, THAI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "PMing LiU" );
	m_cDrawHyperLink.CreateFont  ( 13, 0, 0, 0, FW_NORMAL, 0, 0, 0, THAI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "PMing LiU" );
	m_cDrawProgressMsg.CreateFont( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, THAI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "PMing LiU" );

	m_pDlg->SetWindowText( "帝王戰記" );

	m_cOldPoint.x = -1;
	m_cOldPoint.y = -1;

	BITMAP	RotateBitmap;

	m_cBmpBackgroundEx.GetBitmap( &RotateBitmap );

	m_staticBackColor.Create( "Back Rotate" , WS_CHILD | WS_VISIBLE | SS_BITMAP  , CRect() , m_pDlg );
	m_staticBackColor.SetBitmap( m_cBmpBackgroundEx );
	m_staticBackColor.SetWindowPos( NULL , 7 , 26 , RotateBitmap.bmWidth , RotateBitmap.bmHeight , NULL );
	m_staticBackColor.ShowWindow( SW_SHOW );


	CWnd *pcTempWnd = 0;

	pcTempWnd = m_pDlg->GetDlgItem( IDC_EXitButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_EXITBTNPOSX), 
		GetIntFromStringTable(IDS_EXITBTNPOSY), 
		16, 
		16, 
		SWP_SHOWWINDOW );

	pcTempWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_OPTIONBTNPOSX), 
		GetIntFromStringTable(IDS_OPTIONBTNPOSY), 
		GetIntFromStringTable(IDS_OPTIONBTNWIDTH), 
		GetIntFromStringTable(IDS_OPTIONBTNHEIGHT), 
		SWP_SHOWWINDOW );


	pcTempWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
	pcTempWnd->SetWindowPos( NULL, 
		GetIntFromStringTable(IDS_STARTBTNPOSX), 
		GetIntFromStringTable(IDS_STARTBTNPOSY), 
		GetIntFromStringTable(IDS_STARTBTNWIDTH), 
		GetIntFromStringTable(IDS_STARTBTNHEIGHT), 
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


	if (g_lt == LT_FRENCH)
	{
		//. Hyperlink static text 위치와 링크설정
		pcTempWnd = m_pDlg->GetDlgItem( IDC_HOMEPAGE );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);
		//pcTempWnd->SetWindowPos( NULL, 300+33, 27, 86, 12, SWP_SHOWWINDOW );
		pcTempWnd->SetWindowPos( NULL, 296, 27, 86, 12, SWP_SHOWWINDOW );
		ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_HOMEPAGE);

		pcTempWnd = m_pDlg->GetDlgItem( IDC_JOIN );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);
		pcTempWnd->SetWindowPos( NULL, 396-1, 27, 68, 12, SWP_SHOWWINDOW );
		ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_JOIN);

		pcTempWnd = m_pDlg->GetDlgItem( IDC_CUSTOMER );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);
		pcTempWnd->SetWindowPos( NULL, 478, 27, 116, 12, SWP_SHOWWINDOW );
		ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_CUSTOMER);

		pcTempWnd = m_pDlg->GetDlgItem( IDC_LINE );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);	
		pcTempWnd->SetWindowPos( NULL, 390-4, 27, 200, 12, SWP_SHOWWINDOW );
	}
	else if (g_lt == LT_GERMAN)
	{
		//. Hyperlink static text 위치와 링크설정
		pcTempWnd = m_pDlg->GetDlgItem( IDC_HOMEPAGE );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);
		//pcTempWnd->SetWindowPos( NULL, 300+33, 27, 86, 12, SWP_SHOWWINDOW );
		pcTempWnd->SetWindowPos( NULL, 300+33-6, 27, 86, 12, SWP_SHOWWINDOW );
		ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_HOMEPAGE);

		pcTempWnd = m_pDlg->GetDlgItem( IDC_JOIN );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);
		pcTempWnd->SetWindowPos( NULL, 396-1, 27, 68, 12, SWP_SHOWWINDOW );
		ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_JOIN);

		pcTempWnd = m_pDlg->GetDlgItem( IDC_CUSTOMER );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);
		pcTempWnd->SetWindowPos( NULL, 478, 27, 116, 12, SWP_SHOWWINDOW );
		ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_CUSTOMER);

		pcTempWnd = m_pDlg->GetDlgItem( IDC_LINE );
		pStaticText = (CStatic*)pcTempWnd;
		pStaticText->SetFont(&m_cDrawHyperLink);	
		pcTempWnd->SetWindowPos( NULL, 390-4, 27, 200, 12, SWP_SHOWWINDOW );
	}
	else
	{
		////. Hyperlink static text 위치와 링크설정
		//pcTempWnd = m_pDlg->GetDlgItem( IDC_HOMEPAGE );
		//pStaticText = (CStatic*)pcTempWnd;
		//pStaticText->SetFont(&m_cDrawHyperLink);
		////pcTempWnd->SetWindowPos( NULL, 300+33, 27, 86, 12, SWP_SHOWWINDOW );
		//pcTempWnd->SetWindowPos( NULL, 300+33-4, 27, 86, 12, SWP_SHOWWINDOW );
		//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_HOMEPAGE);

		//pcTempWnd = m_pDlg->GetDlgItem( IDC_JOIN );
		//pStaticText = (CStatic*)pcTempWnd;
		//pStaticText->SetFont(&m_cDrawHyperLink);
		//pcTempWnd->SetWindowPos( NULL, 396+15, 27, 68, 12, SWP_SHOWWINDOW );
		//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_JOIN);

		//pcTempWnd = m_pDlg->GetDlgItem( IDC_CUSTOMER );
		//pStaticText = (CStatic*)pcTempWnd;
		//pStaticText->SetFont(&m_cDrawHyperLink);
		//pcTempWnd->SetWindowPos( NULL, 478-2, 27, 116, 12, SWP_SHOWWINDOW );
		//ConvertStaticToHyperlink(m_pDlg->GetSafeHwnd(), IDC_CUSTOMER);

		//pcTempWnd = m_pDlg->GetDlgItem( IDC_LINE );
		//pStaticText = (CStatic*)pcTempWnd;
		//pStaticText->SetFont(&m_cDrawHyperLink);	
		//pcTempWnd->SetWindowPos( NULL, 390-4+14, 27, 200, 12, SWP_SHOWWINDOW );

		//Html을 보인다.
		CCreateContext pContext;
		CWnd* pFrameWnd = m_pDlg;

		pContext.m_pCurrentDoc = NULL;
		pContext.m_pNewViewClass = RUNTIME_CLASS(CTestHtmlView);

		m_pWeb =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);

		ASSERT(m_pWeb);

		m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL  , 0 , 0 );
		m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR , 0 , 0 );
		m_pWeb->SetWindowPos( NULL , 43 , 253 , 450 , 246 , NULL );
		m_pWeb->ShowWindow( SW_HIDE );
		m_pWeb->Navigate( TAIWAN_WEB_LEFT_URL );
		m_pWeb->SetParent( m_pDlg );

		pContext.m_pCurrentDoc = NULL;
		pContext.m_pNewViewClass = RUNTIME_CLASS(CTestHtmlView);

		m_pWeb2 =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);
		ASSERT(m_pWeb2);

		m_pWeb2->ModifyStyle( WS_BORDER  , 0 , 0 );
		m_pWeb2->ModifyStyleEx( WS_EX_CLIENTEDGE  , 0 , 0 );
		m_pWeb2->SetWindowPos( NULL , 510 , 253 , 245 , 246 , NULL );
		m_pWeb2->ShowWindow( SW_HIDE );
		m_pWeb2->Navigate( TAIWAN_WEB_RIGHT_URL );
		m_pWeb2->SetParent( m_pDlg );
	}

	//ini\\COption.ini를 읽는다.
	m_cPatchOptionFile.Init();
	m_cPatchOptionFile.Load( PATCHCLIENT_OPTION_FILENAME );

	CCreateContext pContext;
	CWnd* pFrameWnd = m_pDlg;

	pContext.m_pCurrentDoc = NULL;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CTestHtmlView);

	m_pWeb =(CTestHtmlView*) ((CFrameWnd*)pFrameWnd)->CreateView(&pContext);

	ASSERT(m_pWeb);

	//m_pWeb->ModifyStyle( WS_BORDER | WS_HSCROLL | WS_VSCROLL, 0, 0 );
	//m_pWeb->ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_LEFTSCROLLBAR | WS_EX_RIGHTSCROLLBAR, 0, 0 );
	//m_pWeb->SetWindowPos(NULL,307,290,310,150,NULL);
	//m_pWeb->ShowWindow(SW_HIDE);
	//m_pWeb->Navigate(m_cPatchOptionFile.GetNoticeURL(), 0, NULL, NULL);		

	//웹 페이지가 뜰 시간 0.5초간 기다려준다.
	Sleep( 500 );

	//TempDir을 세팅해준다.
	if( m_cPatchClientLib.SetTempDir( "Temp" ) == false )
	{
		::LoadStringW(g_resInst, IDS_ERROR_MAKE_PATCH_FOLDER, strMsg, 256);
		MessageBoxW( m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
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

	//패치진행중에 프로그래스바를 그리기위해 받았다. -_-;
	m_cPatchClientLib.SetDialogInfo( m_pDlg->FromHandle( m_pDlg->GetSafeHwnd() ), &m_cCustomProgressRecvFile, &m_cCustomProgressRecvBlock, &m_StaticStatus, NULL, &m_StaticDetailInfo );

	m_pDlg->SetActiveWindow();

	//m_cPatchClientLib.ReadConfig("Ini\\PatchConfig.ini");
	
	m_ArchlordInfo.LoadFile( TRUE );


	PatchThreadStart();

	return TRUE; 
}

VOID TaiwanPatchClient::OnPaint( VOID )
{
	BITMAP		bitmap;
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

VOID TaiwanPatchClient::OnTimer( UINT nIDEvent )
{
	if( nIDEvent == 5 )
	{
		OnPaint();
	}
}

VOID TaiwanPatchClient::DoDataExchange( CDataExchange* pDX )
{
	DDX_Control(pDX, IDC_Static_Status, m_StaticStatus);
	DDX_Control(pDX, IDC_Static_DetailInfo, m_StaticDetailInfo);
	DDX_Control(pDX, IDC_EXitButton, m_cKbcExitButton );
	DDX_Control(pDX, IDC_RegisterButton, m_cKbcRegisterButton );
	DDX_Control(pDX, IDC_OptionButton, m_cKbcOptionButton );
	DDX_Control(pDX, IDC_HomepageButton, m_cKbcHomepageButton );
	DDX_Control(pDX, IDC_START_GAME, m_cKbcStartgameButton );
}

VOID TaiwanPatchClient::OnStartGame( VOID )
{
	bool checkCRC = true;

	if( AuIgnoringHelper::IgnoreToFile( "NotCheckCRC.arc" ) )
	{
		checkCRC = false;
	}

	if( checkCRC )
	{
		// 실행파일의 CRC 체크를 위해 서버로 CRC 를 요청하고 기다린다.
		if( !_GetClientFileCRCFromPatchServer() )
		{
			char strMessage[ 256 ] = { 0, };
			::LoadString( NULL, IDS_ERROR_PATCH_CONNECTION, strMessage, sizeof( char ) * 256 );
			::MessageBox( NULL, strMessage, "Network Error", MB_OK );

			// 패치클라이언트는 종료시킨다.
			DoCloseDialog();
		}
	}
	else
	{
		DoStartGame();
		DoCloseDialog();
	}
}

BOOL TaiwanPatchClient::OnReceiveClientFileCRC( void* pPacket )
{
	DisConnect();

	CPacketTypeCS_0x05* ppdPacket = ( CPacketTypeCS_0x05* )pPacket;
	if( !ppdPacket ) return FALSE;

	m_ulClientFileCRC = ppdPacket->m_ulClientFileCRC;

	// 서버로부터 CRC 정보를 받았으니 CRC 검사를 수행한다.
	if( !_CheckClientFileCRC( "AlefClient.exe", m_ulClientFileCRC ) )
	{
		FILE * f = fopen( "NotCheckCRC.arc", "rb" );

		if( !f )
		{
			char strMessage[ 256 ] = { 0, };
			::LoadString( NULL, IDS_ERROR_CLIENTFILE_CURRUPTED, strMessage, sizeof( char ) * 256 );
			::MessageBox( NULL, strMessage, "Client File Error!", MB_OK );

			// 패치클라이언트는 종료시킨다.
			DoCloseDialog();
			return TRUE;
		}
		else
		{
			fclose( f );
		}		
	}

	// 패치클라이언트가 변경되었으면 다시 시작하라는 메세지를 보내고 종료한다.
	if( _IsUpdatePatchClientFile( "Archlord.bak" ) )
	{
		char strMessage[ 256 ] = { 0, };
		::LoadString( NULL, IDS_NOTICE_DO_RESTART, strMessage, sizeof( char ) * 256 );
		::MessageBox( NULL, strMessage, "Notice", MB_OK );

		// 패치클라이언트는 종료시킨다.
		DoCloseDialog();
		return TRUE;
	}

	// 실제로 게임 클라이언트를 기동시킨다.
	DoStartGame();
	return TRUE;
}

BOOL TaiwanPatchClient::DoStartGame( void )
{
	// 실행파일에 넘겨줄 패치코드를 만들어서..
	if( !_MakePatchCodeString() ) return FALSE;

	// 실행파일에 패치코드를 넘겨서 실행시키고,..
	m_cPatchClientLib.StartGame( ( char* )m_strPatchCodeString );

	// 패치클라이언트는 종료시킨다.
	DoCloseDialog();
	return TRUE;
}

BOOL TaiwanPatchClient::DoCloseDialog( void )
{
	( ( CPatchClientDlg* )m_pDlg )->CloseDlg();
	return TRUE;
}

BOOL TaiwanPatchClient::_GetClientFileCRCFromPatchServer( void )
{
	if( !Connect() ) return FALSE;
	if( !SendRequestClientFileCRC() ) return FALSE;
	if( !Listen( false ) ) return FALSE;
	return TRUE;
}

BOOL TaiwanPatchClient::_CheckClientFileCRC( char* pFileName, unsigned long ulCRC )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	// NotCheckPatchCode.arc 파일이 존재하면 무시한다.
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

BOOL TaiwanPatchClient::_MakePatchCodeString( void )
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

BOOL TaiwanPatchClient::_IsUpdatePatchClientFile( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;
	BOOL bHaveNewPatchClientFile = FALSE;

	// 파일이 존재하는가
	FILE* pFile = ::fopen( pFileName, "rb" );
	if( pFile )
	{
		bHaveNewPatchClientFile = TRUE;
		::fclose( pFile );
	}

	if( IsUpdatePatchClient() && bHaveNewPatchClientFile ) return TRUE;
	return FALSE;
}

bool TaiwanPatchClient::Connect( void )
{
	bool bConnect	=	false;

	for( INT i = 0 ; i < m_ArchlordInfo.GetPatchServerCount() && !bConnect ; ++i )
	{
		stLoginServerInfo*	pInfo		=	m_ArchlordInfo.GetPatchServer( i );
		if( !pInfo )
			return false;

		CString	strFullIPPort	=	pInfo->m_strServerIP.c_str();
		CString	strIP;
		CString	strPort;

		AfxExtractSubString( strIP		, strFullIPPort , 0 , _T(':') );
		AfxExtractSubString( strPort	, strFullIPPort , 1 , _T(':') );

		bConnect	= m_cPatchClientLib.initConnect( 0x0202, false, (CHAR*)strIP.GetString() , atoi(strPort.GetString()) );
	}

	return bConnect;
}

#endif