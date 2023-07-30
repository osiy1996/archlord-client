
#include <AgcmUIControl/AcUIMessageDialog.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AuStrTable/AuStrTable.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmFont/AgcmFont.h>



//------------------------- AcUIWaitingDialog ------------------------- 
const DWORD AcUIWaitingDialog::s_dwWaitMaxTick = 1000 * 60 * 1;	//1��


AcUIWaitingDialog::AcUIWaitingDialog( void )
{
	memset( m_szButtonName, 0, sizeof( m_szButtonName ) );
}

AcUIWaitingDialog::~AcUIWaitingDialog( void )
{
}

BOOL AcUIWaitingDialog::OnInit( void )
{
	m_clClose.MoveWindow( ACUIMESSAGEDIALOG_OK_BUTTON_X, ACUIMESSAGEDIALOG_OK_BUTTON_Y, ACUIMESSAGEDIALOG_OK_BUTTON_WIDTH, ACUIMESSAGEDIALOG_OK_BUTTON_HEIGHT );
	AddChild( &m_clClose, ACUIMESSAGEDIALOG_OK_BUTTON );

	m_clClose.ShowWindow( FALSE );
	m_clClose.SetStaticStringExt( m_szButtonName, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clClose.m_lButtonDownStringOffsetY = 1;
	return TRUE;
}

void  AcUIWaitingDialog::SetMessage( char* szMessage )
{
	AgcModalWindow::SetMessage( szMessage );
	if( !szMessage ) return;

	INT32 lIndex;
	INT32 lLen;

	m_lLine = 0;

	while( TRUE )
	{
		lLen = m_pAgcmFont->GetTextExtent( 0, szMessage,strlen( szMessage ) );
		if( !lLen )	break;

		++m_lLine;
		if( m_lLine > ACUI_MESSAGE_MAX_LINE ) break;

		// ���� String�� ���̰� �� ���ο� ���� ������ ���ο� Copy�ϰ� ��.
		if( lLen < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE )
		{
			strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lLen );
			m_aszMessageLine[ m_lLine - 1 ][ lLen ] = 0;
			break;
		}

		// ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE ������ ���̸� ���� char array�� index�� ã�´�.
		// �� �ڿ������� ������� ã�´�. binary search�� �ϸ� �� ������... �ٵ� �ȴ�. �޷�~~~
		INT32 lMaxIndex = strlen( szMessage );
		while( TRUE )
		{
			INT32 l = m_pAgcmFont->GetTextExtent( 0, szMessage, lMaxIndex );
			if( l < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE ) break;

			lMaxIndex--;
		}

		// ���� ���������� ������ ������ ã�Ƶ���.
		for( lIndex = lMaxIndex - 1 ; lIndex > 0 ; --lIndex )
		{
			if( szMessage[ lIndex ] == ' ' ) break;
		}

		// ������ ���� ���� �׳� �߰��� ���´�.
		if( lIndex == 0 )
		{
			lIndex = lMaxIndex - 1;
		}

		// ������ Copy
		strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lIndex );
		m_aszMessageLine[ m_lLine - 1 ][ lIndex ] = 0;
		szMessage += lIndex;

		while( szMessage[ 0 ] == ' ' )
		{
			++szMessage;
		}
	}

	m_clClose.ShowWindow( FALSE );
	m_dwOpenTick = GetTickCount();
}

void AcUIWaitingDialog::OnWindowRender( void )	
{
	PROFILE( "AcUIWaitingDialog::OnWindowRender" );
	if( m_listTexture.GetCount() <= 0 ) return;

	//�̰�����... Ȱ��ȭ�� ������� ���� ���ƾ�����...
	if( !m_clClose.m_Property.bVisible )
	{
		DWORD dwDiff = GetTickCount() - m_dwOpenTick;
		if( dwDiff > s_dwWaitMaxTick )
		{
			AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );

			memset( m_szButtonName, 0, sizeof( m_szButtonName ) );

			char* pMsg = pcmUIManager ? pcmUIManager->GetUIMessage( "UI_Name_OK" ) : "Exit";
			if( pMsg && strlen( pMsg ) > 0 )
			{
				strcpy( m_szButtonName, pMsg );
			}

			m_clClose.SetStaticStringExt( m_szButtonName, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
			m_clClose.m_lButtonDownStringOffsetY = 1;

			memset( m_szMessage, 0, sizeof( m_szMessage ) );
			pMsg = pcmUIManager ? pcmUIManager->GetUIMessage( "LOGIN_WAITING_FAILE" ) : "Connection Failed.";
			if( pMsg && strlen( pMsg ) > 0 )
			{
				SetMessage( pMsg );
			}			

			m_clClose.ShowWindow( TRUE );
		}
	}
	
	INT32 nX = 0, nY = 0;
	ClientToScreen( &nX, &nY );

	RwTexture* pTexture = m_listTexture.GetHead();
	if( pTexture && pTexture->raster )
	{
		g_pEngine->DrawRwTexture( pTexture, nX, nY );
	}

	if( m_pAgcmFont )
	{
		m_pAgcmFont->FontDrawStart( 0 );
		for( INT32 lIndex = 0 ; lIndex < m_lLine ; ++lIndex )
		{
			float fX = MagIToF( nX + ACUIMESSAGEDIALOG_WIDTH / 2 - m_pAgcmFont->GetTextExtent( 0, m_aszMessageLine[ lIndex ], strlen( m_aszMessageLine[ lIndex ] ) ) / 2 );
			float fY = MagIToF( nY + 70 - ( m_lLine - 1 ) * ( ACUI_MESSAGE_FONT_HEIGHT / 2 ) + lIndex * ACUI_MESSAGE_FONT_HEIGHT );
			m_pAgcmFont->DrawTextIM2D( fX, fY, m_aszMessageLine[ lIndex ], 0 );
		}

		m_pAgcmFont->FontDrawEnd();
	}
}

BOOL AcUIWaitingDialog::OnCommand( INT32 nID, PVOID pParam )
{
	AgcWindow* pcsWindow = ( AgcWindow* )pParam;
	AcUIBase* pcsBase = ( AcUIBase* )pParam;

	if( *( INT32* )pParam == m_clClose.m_lControlID )
	{
		if( nID == UICM_BUTTON_MESSAGE_CLICK )
		{
			RsEventHandler( rsQUITAPP, NULL );
			return TRUE;
		}
	}

	return TRUE;
}


//------------------------- AcUIMessageDialog ------------------------- 
AcUIMessageDialog::AcUIMessageDialog( void )
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );
	strcpy( m_szNameOK, "OK");
}

AcUIMessageDialog::~AcUIMessageDialog( void )
{
}

BOOL AcUIMessageDialog::OnInit( void )
{
	m_clOK.MoveWindow( ACUIMESSAGEDIALOG_OK_BUTTON_X, ACUIMESSAGEDIALOG_OK_BUTTON_Y, ACUIMESSAGEDIALOG_OK_BUTTON_WIDTH, ACUIMESSAGEDIALOG_OK_BUTTON_HEIGHT );
	AddChild( &m_clOK, ACUIMESSAGEDIALOG_OK_BUTTON );

	sprintf_s( m_szNameOK , ACUI_MESSAGE_BUTTONG_LENGTH , ClientStr().GetStr( STI_OK ) );

	m_clOK.ShowWindow( TRUE );
	m_clOK.SetStaticStringExt( m_szNameOK, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clOK.m_lButtonDownStringOffsetY = 1;
	return TRUE;
}

void  AcUIMessageDialog::SetMessage( char* szMessage )
{
	if( NULL == szMessage ) 
	{
		szMessage = "szMessage is NULL";
	}

	AgcModalWindow::SetMessage( szMessage );

	INT32 lIndex;
	INT32 lLen;

	m_lLine = 0;
	while( TRUE )
	{
		// clear memory
		ZeroMemory( m_aszMessageLine[ m_lLine ], ACUI_MESSAGE_MAX_STRING_PER_LINE + 1 );

		lLen = m_pAgcmFont->GetTextExtent( 0, szMessage, strlen( szMessage ) );
		if( !lLen ) break;

		++m_lLine;
		if( m_lLine > ACUI_MESSAGE_MAX_LINE ) break;

		// '___' line feed�� ������ ������ ��Ȳ - ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE����
		// �۴ٸ� �Ѷ������� �����
		INT32 newLine = strcspn( szMessage, "___" );
		if( newLine != strlen( szMessage ) && m_pAgcmFont->GetTextExtent( 0, szMessage, newLine ) < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE )
		{
			lIndex = newLine;

			// ������ Copy
			strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lIndex );
			m_aszMessageLine[ m_lLine - 1 ][ lIndex ] = 0;

			// "___"��ŭ �ڷ�
			lIndex += 3;
		}
		else
		{
			// ���� String�� ���̰� �� ���ο� ���� ������ ���ο� Copy�ϰ� ��.
			if( lLen < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE )
			{
				strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lLen );
				m_aszMessageLine[ m_lLine - 1 ][ lLen ] = 0;
				break;
			}

			// ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE ������ ���̸� ���� char array�� index�� ã�´�.
			// �� �ڿ������� ������� ã�´�. binary search�� �ϸ� �� ������... �ٵ� �ȴ�. �޷�~~~
			INT32 lMaxIndex = strlen( szMessage );
			while( TRUE )
			{
				INT32 l = m_pAgcmFont->GetTextExtent( 0, szMessage, lMaxIndex );
				if( l < ACUI_MESSAGE_MAX_STRING_PIXEL_PER_LINE ) break;
				--lMaxIndex;
			}

			// ���� ���������� ������ ������ ã�Ƶ���.
			for( lIndex = lMaxIndex - 1; lIndex > 0; --lIndex )
			{
				if( szMessage[ lIndex ] == ' ' ) break;
			}

			// ������ ���� ���� �׳� �߰��� ���´�.
			if (lIndex == 0)
			{
				if( lMaxIndex > 0 && isleadbyte( szMessage[ lMaxIndex - 1 ] ) )
				{
					lIndex = lMaxIndex - 1;
				}
				else
				{
					lIndex = lMaxIndex - 2;
				}
			}

			// ������ Copy
			strncpy( &m_aszMessageLine[ m_lLine - 1 ][ 0 ], szMessage, lIndex );
			m_aszMessageLine[ m_lLine - 1 ][ lIndex ] = 0;

		}

		szMessage += lIndex;
		while( szMessage[ 0 ] == ' ')
		{
			++szMessage;
		}
	}
}

void AcUIMessageDialog::OnWindowRender( void )
{
	PROFILE("AcUIMessageDialog::OnWindowRender");
	if( m_listTexture.GetCount() <= 0 ) return;
	
	INT32 nX = 0 , nY = 0;
	ClientToScreen( &nX, &nY );

	RwTexture* pTexture = m_listTexture.GetHead();
	if( pTexture && pTexture->raster )
	{
		g_pEngine->DrawRwTexture( pTexture, nX, nY );
	}

	if( m_pAgcmFont )
	{
		m_pAgcmFont->FontDrawStart( 0 );

		for( INT32 lIndex = 0; lIndex < m_lLine; ++lIndex )
		{
			float fX = MagIToF( nX + ACUIMESSAGEDIALOG_WIDTH / 2 - m_pAgcmFont->GetTextExtent( 0, m_aszMessageLine[ lIndex ], strlen( m_aszMessageLine[ lIndex ] ) ) / 2 );
			float fY = MagIToF( nY + 70 - ( m_lLine - 1 ) * ( ACUI_MESSAGE_FONT_HEIGHT / 2 ) + lIndex * ACUI_MESSAGE_FONT_HEIGHT );
			m_pAgcmFont->DrawTextIM2D( fX, fY, m_aszMessageLine[ lIndex ], 0 );
		}

		m_pAgcmFont->FontDrawEnd();
	}
}

BOOL AcUIMessageDialog::OnCommand( INT32 nID, PVOID pParam )
{
	AgcWindow* pcsWindow = ( AgcWindow* )pParam;
	AcUIBase* pcsBase = ( AcUIBase* )pParam;

	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( nID == UICM_BUTTON_MESSAGE_CLICK )
		{
			EndDialog( 1 );
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AcUIMessageDialog::OnKeyDown( RsKeyStatus *ks	)
{
	if( rsENTER == ks->keyCharCode || ' ' == ks->keyCharCode )
	{
		EndDialog( 1 );
	}

	return TRUE;
}

VOID AcUIMessageDialog::SetButtonName( CHAR* szOK )
{
	if( szOK )
	{
		strcpy( m_szNameOK, szOK );
	}
	else
	{
		m_szNameOK[ 0 ] = NULL;
	}
}

//------------------------- AcUIOKCancelDialog ------------------------- 
AcUIOKCancelDialog::AcUIOKCancelDialog( void )
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );
	m_lOKCancelMode	= 0;

	strcpy( m_szNameOK, "OK" );
	strcpy( m_szNameCancel, "Cancel" );
}

AcUIOKCancelDialog::~AcUIOKCancelDialog( void )
{
}

BOOL AcUIOKCancelDialog::OnInit( void )
{
	m_clOK.MoveWindow( ACUIOKCANCELDIALOG_OK_BUTTON_X, ACUIOKCANCELDIALOG_OK_BUTTON_Y, ACUIOKCANCELDIALOG_BUTTON_WIDTH, ACUIOKCANCELDIALOG_BUTTON_HEIGHT );
	AddChild( &m_clOK, ACUIOKCANCELDIALOG_OK_BUTTON );
	m_clOK.ShowWindow( TRUE );

	m_clCancel.MoveWindow( ACUIOKCANCELDIALOG_CANCEL_BUTTON_X, ACUIOKCANCELDIALOG_CANCEL_BUTTON_Y, ACUIOKCANCELDIALOG_BUTTON_WIDTH, ACUIOKCANCELDIALOG_BUTTON_HEIGHT );
	AddChild( &m_clCancel, ACUIOKCANCELDIALOG_CANCEL_BUTTON );
	m_clCancel.ShowWindow( TRUE );

	sprintf_s( m_szNameOK		, ACUI_MESSAGE_BUTTONG_LENGTH , ClientStr().GetStr( STI_OK ) );
	sprintf_s( m_szNameCancel	, ACUI_MESSAGE_BUTTONG_LENGTH , ClientStr().GetStr( STI_CANCEL ) );
	
	m_clOK.SetStaticStringExt( m_szNameOK, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clCancel.SetStaticStringExt( m_szNameCancel, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );

	m_clOK.m_lButtonDownStringOffsetY = 1;
	m_clCancel.m_lButtonDownStringOffsetY = 1;
	return TRUE;
}

BOOL AcUIOKCancelDialog::OnCommand( INT32 nID, PVOID pParam )
{
	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			EndDialog( 1 );
			return TRUE;
		}
	}
	else if( *( INT32* )pParam == m_clCancel.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			EndDialog( 0 );
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AcUIOKCancelDialog::OnKeyDown( RsKeyStatus *ks	)
{
	if( rsENTER == ks->keyCharCode || ' ' == ks->keyCharCode )
	{
		EndDialog( 1 );
	}
	else if( rsESC == ks->keyCharCode )
	{
		EndDialog( 0 );
	}

	return TRUE;	
}

VOID AcUIOKCancelDialog::SetButtonName( CHAR* szOK, CHAR* szCancel )
{
	if( szOK )
	{
		strcpy( m_szNameOK, szOK );
	}
	else
	{
		m_szNameOK[ 0 ] = NULL;
	}

	if( szOK )
	{
		strcpy( m_szNameCancel, szCancel );
	}
	else
	{
		m_szNameCancel[ 0 ] = NULL;
	}
}
#ifdef _AREA_GLOBAL_
//------------------------- AcUIThreeBtnDialog ------------------------- 
AcUIThreeBtnDialog::AcUIThreeBtnDialog( void )
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );

	strcpy( m_szNameOK, "btn1" );
	strcpy( m_szOneBtn, "btn2" );
	strcpy( m_szTwoBtn, "btn3" );
}

AcUIThreeBtnDialog::~AcUIThreeBtnDialog( void )
{
}

BOOL AcUIThreeBtnDialog::OnInit( void )
{
	int iPosX = ACUIOKCANCELDIALOG_FIRST_BTN_X;

	m_clOneBtn.MoveWindow( iPosX, ACUIOKCANCELDIALOG_FIRST_BTN_Y, ACUIOKCANCELDIALOG_BUTTON_WIDTH, ACUIOKCANCELDIALOG_BUTTON_HEIGHT );
	AddChild( &m_clOneBtn);
	m_clOneBtn.ShowWindow( TRUE );

	iPosX += 75;
	m_clTwoBtn.MoveWindow( iPosX, ACUIOKCANCELDIALOG_FIRST_BTN_Y, ACUIOKCANCELDIALOG_BUTTON_WIDTH, ACUIOKCANCELDIALOG_BUTTON_HEIGHT );
	AddChild( &m_clTwoBtn);
	m_clTwoBtn.ShowWindow( TRUE );

	iPosX += 75;
	m_clOK.MoveWindow( iPosX, ACUIOKCANCELDIALOG_FIRST_BTN_Y, ACUIOKCANCELDIALOG_BUTTON_WIDTH, ACUIOKCANCELDIALOG_BUTTON_HEIGHT );
	AddChild( &m_clOK);
	m_clOK.ShowWindow( TRUE );

	m_clOK.SetStaticStringExt( m_szNameOK, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clOneBtn.SetStaticStringExt( m_szOneBtn, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clTwoBtn.SetStaticStringExt( m_szTwoBtn, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );

	m_clOK.m_lButtonDownStringOffsetY = 1;
	m_clOneBtn.m_lButtonDownStringOffsetY = 1;
	m_clTwoBtn.m_lButtonDownStringOffsetY = 1;

	return TRUE;
}

BOOL AcUIThreeBtnDialog::OnCommand( INT32 nID, PVOID pParam )
{
	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			EndDialog( -1 );
			return TRUE;
		}
	}
	else if( *( INT32* )pParam == m_clOneBtn.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			EndDialog( 0 );
			return TRUE;
		}
	}
	else if( *( INT32* )pParam == m_clTwoBtn.m_lControlID )
	{
		if( UICM_BUTTON_MESSAGE_CLICK == nID )
		{
			EndDialog( 1 );
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AcUIThreeBtnDialog::OnKeyDown( RsKeyStatus *ks	)
{
	if( rsENTER == ks->keyCharCode || ' ' == ks->keyCharCode )
	{
		EndDialog( 1 );
	}
	else if( rsESC == ks->keyCharCode )
	{
		EndDialog( 0 );
	}

	return TRUE;	
}
#endif
//---------------------- AcUIEditOKDialog -------------------------
AcUIEditOKDialog::AcUIEditOKDialog( void )
{
	memset( m_szMessage, 0, sizeof( m_szMessage ) );
	strcpy( m_szNameOK, "OK");
}

AcUIEditOKDialog::~AcUIEditOKDialog( void )
{
}

BOOL AcUIEditOKDialog::OnInit( void )
{
	m_clOK.MoveWindow( ACUIMESSAGEDIALOG_OK_BUTTON_X, ACUIMESSAGEDIALOG_OK_BUTTON_Y, ACUIMESSAGEDIALOG_OK_BUTTON_WIDTH, ACUIMESSAGEDIALOG_OK_BUTTON_HEIGHT );
	AddChild( &m_clOK, ACUIMESSAGEDIALOG_OK_BUTTON );
	sprintf_s( m_szNameOK , ACUI_MESSAGE_BUTTONG_LENGTH , ClientStr().GetStr( STI_OK ) );

	m_clOK.ShowWindow( TRUE );
	m_clOK.SetStaticStringExt( m_szNameOK, 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clOK.m_lButtonDownStringOffsetY = 1;

	m_clEdit.MoveWindow( 60, 105, 200, 30 );
	AddChild( &m_clEdit, 3 );

	m_clEdit.ShowWindow( TRUE );
	m_clEdit.SetStaticStringExt( "", 1.0f, 0, 0xffffffff, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	m_clEdit.SetTextMaxLength( 256 );
	m_clEdit.SetFocus();
	return TRUE;
}

void AcUIEditOKDialog::OnWindowRender( void )
{
	PROFILE("AcUIMessageDialog::OnWindowRender");
	if( m_listTexture.GetCount() <= 0 ) return;
	
	INT32 nX = 0 , nY = 0;
	ClientToScreen( &nX, &nY );

	RwTexture* pTexture = m_listTexture.GetHead();
	if( pTexture && pTexture->raster )
	{
		g_pEngine->DrawRwTexture( pTexture, nX, nY );
	}

	if( m_pAgcmFont )
	{
		m_pAgcmFont->FontDrawStart( 0 );

		for( INT32 lIndex = 0; lIndex < m_lLine; ++lIndex )
		{
			float fX = MagIToF( nX + ACUIMESSAGEDIALOG_WIDTH / 2 - m_pAgcmFont->GetTextExtent( 0, m_aszMessageLine[ lIndex ], strlen( m_aszMessageLine[ lIndex ] ) ) / 2 );
			float fY = MagIToF( nY + 70 - ( m_lLine - 1 ) * ( ACUI_MESSAGE_FONT_HEIGHT / 2 ) + lIndex * ACUI_MESSAGE_FONT_HEIGHT );
			m_pAgcmFont->DrawTextIM2D( fX, fY, m_aszMessageLine[ lIndex ], 0 );
		}

		m_pAgcmFont->FontDrawEnd();
	}

	m_clEdit.OnWindowRender();
}

BOOL AcUIEditOKDialog::OnCommand( INT32 nID, PVOID pParam )
{
	AgcWindow* pcsWindow = ( AgcWindow* )pParam;
	AcUIBase* pcsBase = ( AcUIBase* )pParam;

	if( *( INT32* )pParam == m_clOK.m_lControlID )
	{
		if( nID == UICM_BUTTON_MESSAGE_CLICK )
		{
			EndDialog( 1 );
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AcUIEditOKDialog::OnKeyDown( RsKeyStatus* ks )
{
	if( m_clEdit.IsFocused() )
	{
		m_clEdit.OnKeyDown( ks );
	}

	return TRUE;
}

VOID AcUIEditOKDialog::SetButtonName( CHAR* szOK )
{
	if( szOK )
	{
		strcpy( m_szNameOK, szOK );
	}
	else
	{
		m_szNameOK[ 0 ] = NULL;
	}
}


