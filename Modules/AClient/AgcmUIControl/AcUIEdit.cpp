
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcSkeleton/AuInputComposer.h>
#include <AgcmUIOption/AgcmUIOption.h>
#include <AuStringConv/AuStringConv.h>
#include <AgcmFont/AgcmFont.h>
#include <AgcmUIControl/AcUIScroll.h>
#include <AgcModule/AgcEngine.h>

#define ACUIEDIT_TEXT_ROW_MARGIN									4

#define ACUIEDIT_TAG_COLOR											'C'
#define ACUIEDIT_TAG_HEX_COLOR										'H'
#define ACUIEDIT_TAG_FONTTYPE										'F'
#define ACUIEDIT_TAG_BOLD											'B'
#define ACUIEDIT_TAG_BOLD_END										'b'
#define ACUIEDIT_TAG_BG_COLOR										'O'
#define ACUIEDIT_TAG_BG_HEX_COLOR									'G'


// �Ʒ��ʿ� IME candidate window�� ȭ������� �����°��� �����ϱ�
// ���� �ڵ带 ���� ���������� �����ؾ� �Ѵ�
//extern AgcmUIOption* g_pcsAgcmUIOption;

//@{ 2006/06/30 burumal
CHAR* g_aszHotkeyNameTable[ 256 ] =
{
    NULL,		"ESC",		"1",		"2",		"3",			// 5
	"4",		"5",		"6",		"7",		"8",			// 10
	"9",		"0",		"-",		"=",		"BACKSP",		// 15
	"TAB",		"Q",		"W",		"E",		"R",			// 20
	"T",		"Y",		"U",		"I",		"O",			// 25
	"P",		"[",		"]",		"ENTER",	NULL,			// 30
	"A",		"S",		"D",		"F",		"G",			// 35
	"H",		"J",		"K",		"L",		";",			// 40
    "\"",		"~",		NULL,		"#",		"Z",			// 45
	"X",		"C",		"V",		"B",		"N",			// 50
	"M",		",",		".",		"/",		NULL,			// 55
	"TIMES",	NULL,		"SPACE",	"CAPSLK",	"F1",			// 60
	"F2",		"F3",		"F4",		"F5",		"F6",			// 65
	"F7",		"F8",		"F9",		"F10",		"NUMLOCK",		// 70
	NULL,		"PADHOME",	"PADUP",	"PADPGUP",	"PADMINUS",		// 75
	"PADLEFT",	"PAD5",		"PADRIGHT",	"PADPLUS",	"PADEND",		// 80
    "PADDOWN",	"PADPGDN",	"PADINS",	"PADDEL",	NULL,			// 85
	NULL,		"\\",		"F11",		"F12",		NULL,			// 90
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 95
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 100 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 105 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 110 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 115 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 120
    NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 125 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 130 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 135 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 140 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 145 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 150 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 155 
	NULL, 		"PADENTER", NULL, 		NULL, 		NULL,			// 160
    NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 165 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 170 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 175
	NULL, 		NULL,		NULL,		NULL,		NULL,			// 180
	NULL, 		"DIVIDE",	NULL,		NULL,		NULL,			// 185
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 190
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 195
	NULL, 		NULL, 		"NUMLOCK",	NULL, 		"HOME",			// 200
    "UP", 		"PGUP",		NULL,		"LEFT",		NULL,			// 205
	"RIGHT",	NULL,		"END",		"DOWN",		"PGDN",			// 210
	"INS",		"DEL",		NULL,		NULL,		NULL,			// 215 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 220 
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 225
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 230
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 235
	NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 240
    NULL, 		NULL, 		NULL, 		NULL, 		NULL,			// 245
	NULL, 		NULL, 		NULL, 		"<LClick>", "<RClick>",		// 250
	"<MClick>", NULL,		NULL,		NULL,		NULL,			// 255
	NULL	// 248����
};
//@}

//@{ 2006/07/02 burumal
AcUIDefalutCallback AcUIEdit::m_pCBHotkeyOverwriteCheck = NULL;
PVOID AcUIEdit::m_pHotkeyCheckTargetInstacne = NULL;

//
AcUIEdit::AcUIEdit( BOOL bPassword, BOOL bNumber )
: m_bChangeText(FALSE)
{
	m_nType = TYPE_EDIT;
	m_clProperty.bRenderBlank =	FALSE;

	m_pstStringInfoHead	= NULL;
	m_pstStringInfoTail	= NULL;

	m_szText = NULL;
	m_szTextDisplay = NULL;

	ClearText();

	m_lCursorCount = 0;
	m_lLastTick = 0;

	m_lBoxWidth = 100;
	m_lOriginalBoxWidth	= m_lBoxWidth;
	m_lFontType	= 0;

	m_bPasswordEdit	= bPassword;
	m_bForHotkey = FALSE;
	m_bNumberOnly =	bNumber;
	m_bEnableTag = FALSE;

	m_bMultiLine = FALSE;
	m_bReadOnly = FALSE;
	m_bAutoLF =	FALSE;

	m_bFilterCase =	FALSE;
	m_bReleaseEditInputEnd = TRUE;

	strcpy( &m_szLineDelimiter[0], "\\n" );

	m_lDelimiterLength = strlen( &m_szLineDelimiter[ 0 ] );
	m_ulTextMaxLength =	ACUI_EDIT_DEFAULT_BUFFER_SIZE;
	
	m_pcsVScroll = NULL;
	m_bVScrollLeft = FALSE;
	m_pstStringInfoCursor = NULL;

	m_bIsShowReadingWin = false;
	m_bIsShowCandidateWin = false;

	if(g_eServiceArea == AP_SERVICE_AREA_GLOBAL )
	{
		m_bEnableTextLapping = TRUE;
	}
	else
	{
		m_bEnableTextLapping = FALSE;
	}

	//@{ 2006/06/30 burumal
	/*
	m_lHotkey = 0;
	*/
	m_lHotkey = -1;
	m_eHotkeyType = AGCDUI_MAX_HOTKEY_TYPE;
	//@}

	InitFuncPtr();

	m_nOffSetFromLeftEdge = 0;
	m_nOffSetFromTopEdge = 0;
}

AcUIEdit::~AcUIEdit( void )
{
	if( m_szText )
	{
		delete[] m_szText;
		m_szText = NULL;
	}

	if( m_szTextDisplay )
	{
		delete[] m_szTextDisplay;
		m_szTextDisplay = NULL;
	}

	//@{ Jaewon 20051028
	// Fix the leak.
	ClearStringInfo();
	//@} Jaewon
}

VOID AcUIEdit::OnAddImage( RwRaster* pRaster )
{	
}

VOID AcUIEdit::OnWindowRender( void )
{
	PROFILE( "AcUIEdit::OnWindowRender" );

	AgcdFontClippingBox	stClippingBox;
	AgcdUIEditStringInfo* pstStringInfo;

	INT32 lOffsetX;
	INT32 lScrollMargin = 0;
	INT32 lFontType;

	INT32 abs_x = 0;
	INT32 abs_y = 0;
	ClientToScreen( &abs_x, &abs_y );

	AcUIBase::OnWindowRender();
	if( !m_pAgcmFont ) return;

	++m_lCursorCount;

	m_lBoxWidth = m_lOriginalBoxWidth;
	if( m_pcsVScroll )
	{
		m_lBoxWidth -= m_pcsVScroll->w;
	}

	stClippingBox.x	= ( float )abs_x;
	stClippingBox.y	= ( float )abs_y;
	stClippingBox.w	= ( float )m_lBoxWidth;
	stClippingBox.h	= ( float )h;

	if( m_bVScrollLeft && m_pcsVScroll )
	{
		lScrollMargin = m_pcsVScroll->w;
		stClippingBox.x += lScrollMargin;
	}

	lFontType = m_lFontType;

	m_pAgcmFont->FontDrawStart(lFontType);
	m_pAgcmFont->SetClippingArea(&stClippingBox);

	pstStringInfo = m_pstStringInfoHead;

	// �ؽ�Ʈ ���
	switch( m_eHAlign )
	{
		case ACUIBASE_HALIGN_CENTER:
			{
				_DrawStringAlignCenter( pstStringInfo, &stClippingBox, lFontType, abs_x, abs_y, lScrollMargin );

				if( m_lStartX )
				{
					lOffsetX = m_lBoxWidth - ACUI_EDIT_LEFT_MARGIN;
				}
				else
				{
					lOffsetX = ( m_lBoxWidth - _GetStringWidth( m_lCursorRow ) ) / 2;
					if( m_pstStringInfoCursor && m_pstStringInfoCursor->szString )
					{
						lOffsetX += m_pstStringInfoCursor->lOffsetX + m_pAgcmFont->GetTextExtent( m_pstStringInfoCursor->lFontType, m_pstStringInfoCursor->szString, m_lCursorColumn );
					}
				}
			}
			break;

		case ACUIBASE_HALIGN_RIGHT:
			{
				_DrawStringAlignRight( pstStringInfo, &stClippingBox, lFontType, abs_x, abs_y, lScrollMargin );

				lOffsetX = m_lBoxWidth - ACUI_EDIT_LEFT_MARGIN;
				if( m_pstStringInfoCursor && m_pstStringInfoCursor->szString )
				{
					lOffsetX = lOffsetX - _GetStringWidth( m_lCursorRow ) + m_pstStringInfoCursor->lOffsetX + m_pAgcmFont->GetTextExtent( m_pstStringInfoCursor->lFontType, m_pstStringInfoCursor->szString, m_lCursorColumn );
				}
			}
			break;

		default:
			{
				_DrawStringAlignLeft( pstStringInfo, &stClippingBox, lFontType, abs_x, abs_y, lScrollMargin );
				lOffsetX = m_lCursorPosX;
			}
	}

	// ĳ�� �׸���
	_DrawCaretComposer( abs_x, abs_y, lOffsetX, lScrollMargin );

	m_pAgcmFont->SetClippingArea( NULL );
	m_pAgcmFont->FontDrawEnd();
}

VOID AcUIEdit::OnPostRender( RwRaster* pRaster )	// ���� ������ ��.
{	
}

BOOL AcUIEdit::OnLButtonDown( RsMouseStatus *ms	)
{
	if( !m_bReadOnly )
	{
		SetMeActiveEdit();
	}
	
	return TRUE;
}

VOID AcUIEdit::OnClose( void )
{
	AcUIBase::OnClose();

	if( m_bActiveEdit )
	{
		ReleaseMeActiveEdit();
	}
}

BOOL AcUIEdit::OnChar( char* pChar, UINT lParam )
{
	if( m_bReadOnly ) return FALSE;				// �б����� ���¸� �ȉ�
	if( !m_szText ) return FALSE;				// ���ڿ� �޸𸮰� �غ�ȵǾ� ������ �ȉ�
	if( m_bForHotkey ) return TRUE;				// ����Ű �������¸� �ȵǱ� �ص� ������ �ƴ�

	// Multi Line�� �ƴѵ�, Enter�� �������.. Enter Ű�� ���� ��ó���۾�.. �Է� ���� �� ����ó��
	if( !_ProcessKeyEnter( pChar ) ) return FALSE;
	if( !m_bActiveEdit ) return FALSE;

	// ESC Ű ��ó���۾�.. �Է� ���ó��
	if( !_ProcessKeyESC( pChar ) ) return FALSE;

	// Back Space Ű ��ó�� �۾�.. �ձ��� �ϳ� ������
	if( !_ProcessKeyBackSpace( pChar ) ) return FALSE;

	// Tab�� ��� Message ������
	if( !_ProcessKeyTab( pChar ) ) return FALSE;

	// �׿� �Ϲ� �ؽ�Ʈ ���� ó��...
	return _ProcessKeyText( pChar );
}

BOOL AcUIEdit::OnKeyDown( RsKeyStatus *ks )
{
	if( !m_szText )	return FALSE;
	if( !m_bActiveEdit ) return FALSE;
	if( m_bForHotkey )
	{
		// ���Ű�� ��� �� �Ѱܹ�����.
		switch( ks->keyCharCode )
		{
		case rsLSHIFT:
		case rsRSHIFT:
		case rsLCTRL:
		case rsRCTRL:
		case rsLALT:
		case rsRALT:
			return TRUE;
		}

		//@{ 2006/06/30 burumal
		/*
		SetHotkey(ks->keyCharCode);
		*/
		SetHotkey( ks->keyScanCode );
		//@}

		ReleaseMeActiveEdit();
		return TRUE;
	}

	// 2005.2.17 gemani (Ime�����߿� OnKeyDown�� 2�� ������.. �ѱ� �����߿� ��������)
	if( strlen( m_szComposing ) ) 
	{
		SetText();
		return TRUE;
	}

	switch( ks->keyCharCode )
	{
	case rsLEFT :				return _ProcessKeyLeft();			break;
	case rsRIGHT :				return _ProcessKeyRight();			break;
	case rsUP :					return _ProcessKeyUp();				break;
	case rsDOWN	:				return _ProcessKeyDown();			break;
	case rsDEL :				return _ProcessKeyDelete();			break;
	default :					return TRUE;						break;
	}
}

BOOL AcUIEdit::OnIMEComposing( char* composing, UINT lParam )
{
	if( m_bActiveEdit )
	{
		// ��Ƽ���϶���..
		strcpy( m_szComposing, composing );
		return TRUE;
	}

	return FALSE;
}

// reading window ���̱�
BOOL AcUIEdit::OnIMEReading( void )
{
	m_bIsShowReadingWin = true;
	return true;
}

// reading window ���߱�
BOOL AcUIEdit::OnIMEHideReading( void )
{
	m_bIsShowReadingWin = false;
	return true;
}

// candidate window ���̱�
BOOL AcUIEdit::OnIMECandidate( void )
{
	m_bIsShowCandidateWin = true;
	return true;
}

// candidate window ���߱�
BOOL AcUIEdit::OnIMEHideCandidate( void )
{
	m_bIsShowCandidateWin = false;
	return true;
}

//reading window �׸���
VOID AcUIEdit::DrawReadingWindow( const float fPosX, const float fPosY )
{
	if( m_bIsShowReadingWin )
	{
		char strReading[ 256 ];
		memset( strReading, 0, sizeof( strReading ) );

		const float fFontHeight = static_cast< float >( m_pAgcmFont->m_astFonts[ m_lFontType ].lSize );
		AuInputComposer& refIMEComposer = AuInputComposer::GetInstance();
		int	textLen = refIMEComposer.GetReading( strReading, sizeof( strReading ) );

		m_pAgcmFont->DrawTextIM2D( fPosX, fPosY + fFontHeight, strReading, m_lFontType, 255, 0xFFFFFFFF, true, false, true, 0x7F7FFF7F );
	}
}

//candidate window �׸���
VOID AcUIEdit::DrawCandidateWindow( const float fPosX, const float fPosY )
{
	// �Ʒ��ʿ� IME candidate window�� ȭ������� �����°��� �����ϱ�
	// ���� �ڵ带 ���� ���������� �����ؾ� �Ѵ�
	static AgcmUIOption* pcsAgcmUIOption = NULL;
	if( NULL == pcsAgcmUIOption )
	{
		pcsAgcmUIOption = ( AgcmUIOption * )m_pAgcmFont->GetModule( "AgcmUIOption" );
	}

	if( m_bIsShowCandidateWin )
	{
		DWORD dwColor;
		const float fFontHeight = static_cast< float >( m_pAgcmFont->m_astFonts[ m_lFontType ].lSize );

		// candidate window�� ������ġ�� ������ ����.
		// reading window���� ��ĭ �� �Ʒ��� �ڸ�����.
		float fCurrentFontPosX = fPosX;
		float fCurrentFontPosY = fPosY + fFontHeight * 1.2f;

		AuInputComposer& ric = AuInputComposer::GetInstance();
		int count = min( ric.GetCandidateCount(), ric.GetCandidatePageStart() + ric.GetCandidatePageSize() );

		for( int nIndex = ric.GetCandidatePageStart(), i = 1 ; nIndex < count ; ++nIndex, ++i ) 
		{
			if( nIndex == ric.GetCandidateSelection() )	//���õ� candidate�� ��� backboard�� ������ �ٸ���.
			{
				dwColor = 0xFFACACAC;
			}
			else 
			{
				dwColor = 0xFF464646;
			}

			char strCandidate[ 64 ] = { 0, };
			char compose[ 64 ] = { 0, };

			INT32 lScreenWidth = 0;
			if( g_pEngine && g_pEngine->m_pCurrentFullUIModule )
			{
				lScreenWidth = g_pEngine->m_pCurrentFullUIModule->w;
			}

			ric.GetCandidate( nIndex, strCandidate, sizeof( strCandidate ) );
			int textLen = sprintf( compose, "%d. %s ", i, strCandidate );

			// candidate word�� �׷��� ��ġ�� ������
			float candidateLength = ( float )m_pAgcmFont->GetTextExtent( m_lFontType, compose, textLen );
			if( fCurrentFontPosX + candidateLength > ( float )pcsAgcmUIOption->m_iScreenWidth )
			{
				fCurrentFontPosY += fFontHeight;
				fCurrentFontPosX = fPosX;
			}

			// candidate word�� ������ �׸���
			AgcdFontClippingBox stCurrClippingBox = m_pAgcmFont->GetCurrentClippingArea();
			m_pAgcmFont->SetClippingArea( NULL );

			m_pAgcmFont->DrawTextIM2DEx( fCurrentFontPosX, fCurrentFontPosY, compose, textLen, m_lFontType, 255, 0xFFFFFFFF, true, false, true, dwColor );

			m_pAgcmFont->SetClippingArea( &stCurrClippingBox );
			fCurrentFontPosX += candidateLength;	// ���� candidate word�� �׷��� ��ġ
		}
	}
}

VOID AcUIEdit::OnMoveWindow( void )
{
	m_lOriginalBoxWidth = w;
	m_lBoxWidth	= m_lOriginalBoxWidth;

	if( m_pcsVScroll )
	{
		if( m_pcsVScroll->x + m_pcsVScroll->w / 2 < w / 2 )
		{
			m_bVScrollLeft = TRUE;
		}
		else
		{
			m_bVScrollLeft = FALSE;
		}

		m_lBoxWidth -= m_pcsVScroll->w;
	}

	AcUIBase::OnMoveWindow();
}

BOOL AcUIEdit::OnIdle( UINT32 ulClockCount )
{
	UINT32 tickdiff = ulClockCount - m_lLastTick;
	m_lCursorCount += tickdiff;
	m_lLastTick = ulClockCount;
	return TRUE;
}

VOID AcUIEdit::OnEditActive( void )
{
	// �н����� �Է�â�̸� ���� ���� ��ȯ����
	if( m_bPasswordEdit )
	{
		AuInputComposer::GetInstance().ToAlphaNumeric( g_pEngine->GethWnd() );
	}

	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_EDIT_ACTIVE, ( PVOID )&m_lControlID );
	}
}

VOID AcUIEdit::OnEditDeactive( void )
{
	// �н����� �Է�â�� ������ ���� �Է� ���� �ٲ���
	if( m_bPasswordEdit )
	{
		AuInputComposer::GetInstance().ToNative( g_pEngine->GethWnd() );
	}

	if( pParent )
	{
		pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_EDIT_DEACTIVE, ( PVOID )&m_lControlID );
	}
}

BOOL AcUIEdit::OnCommand( INT32	nID , PVOID pParam )
{
	PROFILE( "AcUIList::OnCommand" );
	if( !m_bActiveEdit && m_pcsVScroll && *( INT32* ) pParam == m_pcsVScroll->m_lControlID )
	{
		// Scroll�� ��������.
		switch( nID )
		{
		case UICM_SCROLL_CHANGE_SCROLLVALUE :
			{
				if( m_lStringHeight )
				{
					FLOAT fValue = m_pcsVScroll->GetScrollValue();
					if( ( int )m_ulLine * m_lStringHeight > ( int )h )
					{
						m_lStartLine = ( INT32 )( ( ( INT32 ) m_ulLine - h / m_lStringHeight ) * fValue );
						m_lStartY =	-m_lStringHeight * m_lStartLine;
					}
				}

				return TRUE;
			}
			break;
		}
	}
		
	return AcUIBase::OnCommand( nID, pParam );
}

BOOL AcUIEdit::CheckSOL( BOOL bFirstLine )
{
	// �� ù���� ������ SOL
	if( bFirstLine && !m_lCursorPoint ) return TRUE;
	if( m_lCursorPoint >= m_lDelimiterLength && 
		!strncmp( &m_szText[ m_lCursorPoint - m_lDelimiterLength ], &m_szLineDelimiter[ 0 ], m_lDelimiterLength ) )
		return TRUE;
	return FALSE;
}

BOOL AcUIEdit::CheckEOL( void )
{
	if( !strncmp( &m_szText[ m_lCursorPoint ], &m_szLineDelimiter[ 0 ], m_lDelimiterLength ) ) return TRUE;
	return FALSE;
}

VOID AcUIEdit::SetBoxWidth(INT32 w)
{
	m_lOriginalBoxWidth = w;
	if( m_pcsVScroll )
	{
		m_lBoxWidth = m_lOriginalBoxWidth - m_pcsVScroll->w;
	}
}

BOOL AcUIEdit::SetText( CHAR* szText , BOOL bChatting )
{
	if( !m_szText ) return FALSE;

	INT32 lIndex;
	INT32 lIndex2;

	m_pstStringInfoCursor = NULL;

	if( szText )
	{
		ClearText();

		m_lTextLength =	strlen( szText );
		m_lCursorPoint = m_lTextLength;

		if( m_lTextLength > ( INT32 )m_ulTextMaxLength )
		{
			m_lTextLength = m_ulTextMaxLength;
		}

		strncat( m_szText , szText, m_lTextLength );
	}
	else
	{
		m_ulLine = 0;
		m_lCursorPosY =	0;
		m_lStartX =	m_nOffSetFromLeftEdge;
		m_lStartY =	m_nOffSetFromTopEdge;
	}

	ClearStringInfo();
	m_lTextLength = strnlen( m_szText, m_ulTextMaxLength );

	if( m_bFilterCase )
	{
		ToLowerExceptFirst( m_szText, m_ulTextMaxLength );
	}

	memcpy( m_szTextDisplay, m_szText, sizeof( CHAR ) * ( m_lTextLength + 1 ) );
	m_lCursorPointDisplay =	m_lCursorPoint;
	lIndex2 = 0;

	BOOL bSingleByte = TRUE;
	for( lIndex = 0 ; lIndex <= m_lTextLength ; ++lIndex )
	{
		if( !m_szTextDisplay[ lIndex ] ) break;

		// 2����Ʈ�� �� ���ڰ� �������Ͱ� �Ǵ� ��쿡 ������ �ȴ�.
		// GetNext���� �Լ� ���� �����ϰ����� �ڵ��� �ϰ����� ���� -_-;;
		BOOL bNext = bSingleByte;
		if( bSingleByte )
		{
			if( AP_SERVICE_AREA_JAPAN == g_eServiceArea	&& m_szTextDisplay[ lIndex ] < 0 )
			{
				bSingleByte = FALSE;
				bNext = FALSE;
			}
		}
		else
		{
			bNext = TRUE;
		}

		if( bSingleByte && m_bMultiLine && 
			( !strncmp( &m_szLineDelimiter[ 0 ], m_szTextDisplay + lIndex, m_lDelimiterLength )	||
			lIndex == m_lTextLength ) )
		{
			m_szTextDisplay[ lIndex ] = 0;
			_ParseLine( m_szTextDisplay + lIndex2, lIndex - lIndex2 , bChatting );

			lIndex2 = lIndex + m_lDelimiterLength;
			lIndex += m_lDelimiterLength - 1;
		}
		else if( m_bPasswordEdit && lIndex < m_lTextLength )
		{
			m_szTextDisplay[ lIndex ] = '*';
		}

		bSingleByte = bNext;
	}

	_ParseLine( m_szTextDisplay + lIndex2 , -1 , bChatting );

	if( m_lStringHeight )
	{
		// �� ����Ʈ�ڽ��� ������ ǥ���Ҽ� �ִ°�..
		int nEditMaxLine = ( int )h / m_lStringHeight;

		// ���� ���μ��� ����Ʈ�ڽ� ǥ�ð����ټ����� ũ��..
		if( (int)m_ulLine >= nEditMaxLine )
		{
			// ���۶��μ��� �������ش�. �����ε����� 0���� �����ϴϱ� 1�� ���ش�. 
			m_lStartLine = ( m_ulLine - 1 ) - nEditMaxLine;
		}
		else
		{
			m_lStartLine = 0;
		}
	}

	// Cursor Column�� Row ����
	_UpdateCursorPosition( m_pstStringInfoHead );
	_UpdateScrollBarVert();

	return TRUE;
}

VOID AcUIEdit::_ParseLine( CHAR *szLine, INT32 lStringLength , BOOL bChatting )
{
	INT32 lOffsetX = 0;
	CHAR* szItemStart;
	CHAR* szTagStart;
	CHAR* szTagEnd;
	LineControl	eNewLine = NEWLINE;

	if( lStringLength == -1 )
	{
		lStringLength = strlen( szLine );
	}

	BOOL bBold		= m_pstStringInfoTail ? m_pstStringInfoTail->bBold		: FALSE;
	DWORD dwBGColor = m_pstStringInfoTail ? m_pstStringInfoTail->dwBGColor	: 0x00000000;
	DWORD dwColor	= m_pstStringInfoTail ? m_pstStringInfoTail->dwColor	: m_lColor;
	INT32 lFontType = m_pstStringInfoTail ? m_pstStringInfoTail->lFontType	: m_lFontType;

	// Text�� ���̸� ���Ѵ�. ('<>' �ȿ��� ������ ������� ���Ƿ� ���̿��� ����)
	// ��, '<'�� ǥ���ϰ��� �ϸ�, '<<' �̷� ������ ���� �ȴ�.
	szItemStart = szTagStart = szLine;
	if( m_bEnableTag )
	{
		while( ( szTagStart = strchr( szTagStart, '<' ) ) )
		{
			++szTagStart;

			if( *szTagStart == '<' )
			{
				memcpy( szTagStart, szTagStart + 1, strlen( szTagStart ) );
				
				if( m_szTextDisplay + m_lCursorPointDisplay > szTagStart )
				{
					--m_lCursorPointDisplay;
				}

				--lStringLength;
			}
			else
			{
				// '>'�� ã�Ƽ� �ű������ ���̸� ���Ѵ�.
				szTagEnd = strchr( szTagStart, '>' );
				if( szTagEnd )
				{
					AddStringItem( szItemStart, dwColor, lFontType, eNewLine, szTagStart - szItemStart - 1, dwBGColor, bBold , bChatting );
					eNewLine = CURRENTLINE;

					_ParseTag( szTagStart, &lFontType, &dwColor, &dwBGColor, &bBold );
					szItemStart = szTagStart = szTagEnd + 1;
				}
			}
		}
	}

	AddStringItem( szItemStart, dwColor, lFontType, eNewLine, lStringLength - ( szItemStart - szLine ), dwBGColor, bBold,  bChatting );
}

VOID AcUIEdit::_ParseTag( CHAR *szTag, INT32 *plFontType, DWORD *pdwColor, DWORD *pdwBGColor, BOOL *pbBold )
{
	if( !m_bEnableTag ) return;

	INT32 lIndex;
	CHAR* pEndChar;
	CHAR* pStartTag = szTag;

	*plFontType = -1;

	for( lIndex = 0 ; szTag[ lIndex ] && szTag[ lIndex ] != '>' ; ++lIndex )
	{
		pStartTag = szTag + lIndex;

		switch( szTag[ lIndex ] )
		{
		case ACUIEDIT_TAG_COLOR :
			{
				*pdwColor = strtoul( szTag + lIndex + 1, &pEndChar, 10 );
				lIndex += pEndChar - pStartTag - 1;
			}
			break;

		case ACUIEDIT_TAG_HEX_COLOR :
			{
				*pdwColor = strtoul( szTag + lIndex + 1, &pEndChar, 16 );	// convert hex string to dword
				lIndex += pEndChar - pStartTag - 1;
			}
			break;

		case ACUIEDIT_TAG_BOLD :
			{
				*pbBold = TRUE;
			}
			break;

		case ACUIEDIT_TAG_BOLD_END :
			{
				*pbBold = FALSE;
			}
			break;

		case ACUIEDIT_TAG_BG_COLOR :
			{
				*pdwBGColor = strtoul( szTag + lIndex + 1, &pEndChar, 10 );
				lIndex += pEndChar - pStartTag - 1;
			}
			break;

		case ACUIEDIT_TAG_BG_HEX_COLOR :
			{
				*pdwBGColor = strtoul( szTag + lIndex + 1, &pEndChar, 16 );	// convert hex string to dword
				lIndex += pEndChar - pStartTag - 1;
			}
			break;
		}
	}
}

AgcdUIEditStringInfo* AcUIEdit::AddStringItem( CHAR *szString, DWORD dwColor, INT32 lFontType, LineControl eNewLine, INT32 lStringLength, DWORD dwBGColor, BOOL bBold, BOOL bChatting )
{
	if( eNewLine && m_ulLine >= ACUI_EDIT_MAX_LINE ) return NULL;

	if( lStringLength == -1 )
	{
		lStringLength = strlen( szString );
	}

	AgcdUIEditStringInfo* pstNewStringInfo = new AgcdUIEditStringInfo;
	if( !pstNewStringInfo )	return NULL;

	if( m_pstStringInfoHead )
	{
		switch( eNewLine )
		{
		case CURRENTLINE :
			{
				pstNewStringInfo->lOffsetX = m_pstStringInfoTail->lOffsetX + m_pstStringInfoTail->lWidth;
				pstNewStringInfo->lOffsetY = m_pstStringInfoTail->lOffsetY;
			}
			break;

		case NEWLINE :
			{
				pstNewStringInfo->lOffsetY = m_pstStringInfoTail->lOffsetY + m_pstStringInfoTail->lHeight;
			}
			break;

		case CONTINOUSLINE :
			{
				// ������ �� �߶� �����ٰ�� , �տ� �ణ�� ������ �д�.
				if( bChatting )
				{
					// ä���϶���... 15�ȼ� �ɼ�..
					pstNewStringInfo->lOffsetX = 15;
				}

				pstNewStringInfo->lOffsetY = m_pstStringInfoTail->lOffsetY + m_pstStringInfoTail->lHeight;
			}
			break;
		}

		if (lFontType == -1)
		{
			if (m_pstStringInfoTail)
				lFontType = m_pstStringInfoTail->lFontType;
			else
				lFontType = m_pstStringInfoHead->lFontType;
		}

		m_pstStringInfoTail->pstNext = pstNewStringInfo;
		m_pstStringInfoTail = pstNewStringInfo;
	}
	else
	{
		m_pstStringInfoHead = pstNewStringInfo;
		m_pstStringInfoTail = pstNewStringInfo;

		if( lFontType == -1 )
		{
			lFontType = m_lFontType;
		}
	}

	pstNewStringInfo->szString		= szString;
	pstNewStringInfo->dwColor		= dwColor;
	pstNewStringInfo->dwBGColor		= dwBGColor;
	pstNewStringInfo->bBold			= bBold;
	pstNewStringInfo->lFontType		= lFontType;
	pstNewStringInfo->bNewLine		= ( eNewLine != CURRENTLINE ) ? TRUE : FALSE ;
	pstNewStringInfo->lStringLength	= lStringLength;

	if( m_pAgcmFont )
	{
		pstNewStringInfo->lWidth = m_pAgcmFont->GetTextExtent( lFontType, szString, lStringLength );
		pstNewStringInfo->lHeight = m_pAgcmFont->m_astFonts[ lFontType ].lSize + ACUIEDIT_TEXT_ROW_MARGIN;
	}
	else
	{
		pstNewStringInfo->lWidth = 0;
		pstNewStringInfo->lHeight = 0;
	}

	if( eNewLine )
	{
		++m_ulLine;
	}

	pstNewStringInfo->lLineIndex = m_ulLine - 1;

	// Height, Width, Length���� �����Ѵ�.
	if( _GetStringHeight( pstNewStringInfo->lLineIndex ) < pstNewStringInfo->lHeight )
	{
		m_alStringHeight[ pstNewStringInfo->lLineIndex ] = pstNewStringInfo->lHeight;
		m_alStringStartY[ pstNewStringInfo->lLineIndex + 1 ] = _GetStringStartPosY( pstNewStringInfo->lLineIndex ) + pstNewStringInfo->lHeight;
	}

	// �ڵ� �ٹٲ� ó��
	if( m_bAutoLF && pstNewStringInfo->lOffsetX + pstNewStringInfo->lWidth + ACUI_EDIT_LEFT_MARGIN * 2 > m_lBoxWidth )
	{
		CHAR* szNewLine;
		CHAR* szTemp = NULL;

		BOOL bWordIsNotEnd = FALSE;
		CHAR* szPrevWord = NULL;

		for( szNewLine = szString ; szNewLine - szString < lStringLength && szNewLine ; szNewLine = CharNextA( szNewLine ) )
		{
			if( m_bEnableTextLapping )
			{
				if( *szNewLine == ' ' )
				{
					// ��ĭ ����.
					bWordIsNotEnd = FALSE;
					szPrevWord = NULL;
				}
				else
				{
					if( !bWordIsNotEnd )
					{
						bWordIsNotEnd = TRUE;
						szPrevWord = szNewLine;
					}
				}
			}

			if( pstNewStringInfo->lOffsetX + m_pAgcmFont->GetTextExtent( lFontType, szString, szNewLine - szString + 1 ) + ACUI_EDIT_LEFT_MARGIN * 2 > m_lBoxWidth )
			{
				if( szTemp )
				{
					szNewLine = szTemp;
				}

				if( bWordIsNotEnd )
				{
					if( szPrevWord != szString )
					{
						szNewLine = szPrevWord;
						// ���ۺ��� �Ѵܾ��̹Ƿ� ���ѷ����� ��������
						// ������ �߶������.
					}
				}

				pstNewStringInfo->lStringLength = szNewLine - szString;
				pstNewStringInfo->lWidth = m_pAgcmFont->GetTextExtent( lFontType, pstNewStringInfo->szString, pstNewStringInfo->lStringLength );

				if( !AddStringItem( szNewLine, dwColor, lFontType, CONTINOUSLINE, lStringLength - ( szNewLine - szString ), 0x00000000, FALSE, bChatting ) )
				{
					return NULL;
				}

				break;
			}
			else
			{
				szTemp = CharNextA( szNewLine );
			}
		}
	}

	m_alStringLength[ pstNewStringInfo->lLineIndex ] += pstNewStringInfo->lStringLength;
	m_alStringWidth[ pstNewStringInfo->lLineIndex ] += pstNewStringInfo->lWidth;
	m_lTextDisplayLength += pstNewStringInfo->lStringLength;

	return pstNewStringInfo;
}

VOID AcUIEdit::ClearStringInfo( void )
{
	AgcdUIEditStringInfo* pstStringInfoNext;
	AgcdUIEditStringInfo* pstStringInfo	= m_pstStringInfoHead;

	//@{ Jaewon 20051028
	// if -> while (What a leak!)
	while( pstStringInfo )
	//@} Jaewon
	{
		pstStringInfoNext = pstStringInfo->pstNext;

		//@{ 2006/06/28 burumal
		if( m_pstStringInfoCursor && ( m_pstStringInfoCursor == pstStringInfo ) )
		{
			m_pstStringInfoCursor = NULL;
		}
		//@}

		delete pstStringInfo;
		pstStringInfo = pstStringInfoNext;
	}

	m_pstStringInfoHead	= m_pstStringInfoTail =	NULL;

	m_alStringHeight.MemSetAll();
	m_alStringStartY.MemSetAll();
	m_alStringWidth.MemSetAll();
	m_alStringLength.MemSetAll();
}

VOID AcUIEdit::ClearText( void )
{
	if( m_szText )
	{
		memset( m_szText, 0, sizeof( CHAR ) * ( m_ulTextMaxLength + 1 ) );
		memset( m_szTextDisplay, 0, sizeof( CHAR ) * ( m_ulTextMaxLength + 1 ) );
	}

	memset( m_szComposing, 0, sizeof( m_szComposing ) );

	m_lTextLength =	0;
	m_lTextDisplayLength = 0;

	m_lCursorPosX =	ACUI_EDIT_LEFT_MARGIN;
	m_lCursorPosY =	0;
	m_lCursorRow = 0;
	m_lCursorColumn	= 0;
	m_lCursorPoint = 0;

	m_ulLine = 0;
	m_lStartLine = 0;
	m_lStartX =	m_nOffSetFromLeftEdge;
	m_lStartY =	m_nOffSetFromTopEdge;

	ClearStringInfo();
}

VOID AcUIEdit::SetTextStartLine( INT32 nLine )
{
	m_ulLine = nLine;
	if( m_ulLine < 0 )
	{
		m_ulLine = 0;
	}

	OnMouseWheel( 1 );
}

BOOL AcUIEdit::SetMeActiveEdit( void )
{
	if( m_bReadOnly ) return FALSE;

	SetText();

	return g_pEngine->SetMeActiveEdit( this );
}

BOOL AcUIEdit::ReleaseMeActiveEdit( void )
{
	SetText();
	return g_pEngine->ReleaseMeActiveEdit( this );
}

BOOL AcUIEdit::SetLineDelimiter( CHAR *szDelimiter )
{
	if( !szDelimiter || strlen( szDelimiter ) >= ACUI_EDIT_LF_SIZE ) return FALSE;

	strcpy( &m_szLineDelimiter[ 0 ], szDelimiter );
	m_lDelimiterLength = strlen( &m_szLineDelimiter[ 0 ] );

	return TRUE;
}

VOID AcUIEdit::SetVScroll( AcUIScroll *pcsScroll )
{
	m_pcsVScroll = pcsScroll;

	if( m_pcsVScroll )
	{
		if( m_pcsVScroll->x + m_pcsVScroll->w / 2 < w / 2 )
		{
			m_bVScrollLeft = TRUE;
		}
		else
		{
			m_bVScrollLeft = FALSE;
		}
	}
}

BOOL AcUIEdit::OnMouseWheel( INT32 lDelta )
{
	if( !m_bMultiLine )	return TRUE;

	if( lDelta > 0 )
	{
		if( m_lStartLine > 0 )
		{
			m_lStartY += _GetStringHeight( m_lStartLine );
			--m_lStartLine;
		}
	}
	else if( lDelta < 0 )
	{
		if( m_lStartLine < ( INT32 )m_ulLine - 1 )
		{
			if( m_lStartY + _GetStringStartPosY( m_ulLine ) >= h )
			{
				m_lStartY -= _GetStringHeight( m_lStartLine );
				++m_lStartLine;
			}
		}
	}
	//else
	//{
	//	if( m_lStartY + _GetStringStartPosY( m_ulLine ) >= h )
	//	{
	//		m_lStartY -= _GetStringHeight( m_lStartLine );
 //           ++m_lStartLine;
	//	}
	//}

	//�θ� Scroll�̶�� ���̻�.. �̺�Ʈ�� ������
	_UpdateScrollBarVert();
	return TRUE;
}

VOID AcUIEdit::SetTextMaxLength( UINT32 ulLength )
{
	m_ulTextMaxLength = ulLength ? ulLength : ACUI_EDIT_DEFAULT_BUFFER_SIZE;

	if( m_szText )
	{
		delete[] m_szText;
		m_szText = NULL;
	}

	if( m_szTextDisplay )
	{
		delete[] m_szTextDisplay;
		m_szTextDisplay = NULL;
	}

	m_szText = new CHAR[ m_ulTextMaxLength + 1 ];
	m_szTextDisplay = new CHAR[ m_ulTextMaxLength + 1 ];

	ClearText();
}

//@{ 2006/06/30 burumal
BOOL AcUIEdit::SetHotkey( AgcdUIHotkeyType eType, INT32 lScanCode )
{
	if( !lScanCode ) return FALSE;

	CHAR* szKeyType = NULL;
	CHAR szHotkey[ 16 ];

	switch( eType )
	{
	case AGCDUI_HOTKEY_TYPE_SHIFT :		szKeyType = "Shift+";		break;
	case AGCDUI_HOTKEY_TYPE_CONTROL :	szKeyType = "Ctrl+";		break;
	case AGCDUI_HOTKEY_TYPE_ALT :		szKeyType = "Alt+";			break;
	default :							szKeyType = "";				break;
	}
	
	if( g_aszHotkeyNameTable[ lScanCode ] == NULL ) return FALSE;
	if( g_aszHotkeyNameTable[ lScanCode ][ 0 ] == '<' )	return FALSE; // <LClick> <RClick> <MClick>�� ����
	if( m_pCBHotkeyOverwriteCheck && m_pHotkeyCheckTargetInstacne )
	{
		if( m_pCBHotkeyOverwriteCheck( m_pHotkeyCheckTargetInstacne, ( PVOID )&eType, ( PVOID )&lScanCode, ( PVOID )this ) )
		{
			return FALSE;
		}
	}

	m_eHotkeyType = eType;
	m_lHotkey = lScanCode;

	sprintf( szHotkey, "%s%s", szKeyType, g_aszHotkeyNameTable[ m_lHotkey ] );
	SetText( szHotkey );

#ifdef _DEBUG
	TRACE( "ScanCode: %d\n", m_lHotkey );
#endif

	return TRUE;
}
//@}

BOOL AcUIEdit::SetHotkey( INT32 lScanCode )
{
	if( !lScanCode ) return FALSE;

	CHAR* szKeyType = NULL;
	CHAR szHotkey[ 16 ];
	AgcdUIHotkeyType eType;

	if( g_pEngine->IsShiftDown() )
	{
		eType = AGCDUI_HOTKEY_TYPE_SHIFT;
		szKeyType = "Shift+";
	}
	else if( g_pEngine->IsCtrlDown() )
	{
		eType = AGCDUI_HOTKEY_TYPE_CONTROL;
		szKeyType = "Ctrl+";
	}
	else if( g_pEngine->IsAltDown() )
	{
		eType = AGCDUI_HOTKEY_TYPE_ALT;
		szKeyType = "Alt+";
	}
	else
	{
		eType = AGCDUI_HOTKEY_TYPE_NORMAL;
		szKeyType = "";
	}

	if( g_aszHotkeyNameTable[ lScanCode ] == NULL ) return FALSE;
	if( g_aszHotkeyNameTable[ lScanCode ][ 0 ] == '<') return FALSE; 	// <LClick> <RClick> <MClick>�� ����
	if( m_pCBHotkeyOverwriteCheck && m_pHotkeyCheckTargetInstacne )
	{
		if( m_pCBHotkeyOverwriteCheck( m_pHotkeyCheckTargetInstacne, ( PVOID )&eType, ( PVOID )&lScanCode, ( PVOID )this ) )
		{
			return FALSE;
		}
	}

	m_lHotkey = lScanCode;
	m_eHotkeyType = eType;

	sprintf( szHotkey, "%s%s", szKeyType, g_aszHotkeyNameTable[ m_lHotkey ] );
	SetText( szHotkey );

#ifdef _DEBUG
	TRACE( "ScanCode: %d\n", m_lHotkey );
#endif

	return TRUE;
}

BOOL AcUIEdit::GetHotkey( INT32 *plScanCode, AgcdUIHotkeyType *peType )
{
	if( !m_bForHotkey || !m_lHotkey ) return FALSE;

	*plScanCode = m_lHotkey;
	*peType = m_eHotkeyType;

	return TRUE;
}

void AcUIEdit::InitFuncPtr( void )
{
	if( AP_SERVICE_AREA_KOREA == g_eServiceArea )
	{
		IsDBCPtr = &AcUIEdit::IsDBC_kr;
	}

	if( AP_SERVICE_AREA_CHINA == g_eServiceArea || AP_SERVICE_AREA_TAIWAN == g_eServiceArea )
	{
		IsDBCPtr = &AcUIEdit::IsDBC_cn;
	}

	if( AP_SERVICE_AREA_GLOBAL == g_eServiceArea )
	{
		IsDBCPtr = &AcUIEdit::IsDBC_en;
	}

	if( AP_SERVICE_AREA_JAPAN == g_eServiceArea )
	{
		IsDBCPtr = &AcUIEdit::IsDBC_jp;
	}
}

bool AcUIEdit::IsDBC_kr( unsigned char* str )
{
	if( ( *str >= 0x40 ) && ( *( str - 1 ) > 0x80 ) ) return true;
	return false;
}

bool AcUIEdit::IsDBC_cn( unsigned char* str )
{
	if( ( *str >= 0x40 ) && ( *( str - 1 ) > 0x80 ) ) return true;
	return false;
}

bool AcUIEdit::IsDBC_jp( unsigned char* str )
{
	if( *str >= 0x40 )
	{
		if( ( ( *( str - 1 ) > 0x80 ) && ( *( str - 1 ) <= 0x9F ) ) || ( *( str - 1 ) >= 0xE0 ) ) return true;
	}

	return false;
}

bool AcUIEdit::IsDBC_en( unsigned char* /*str*/ )
{
	return false;
}

INT32 AcUIEdit::_GetStringWidth( INT32 nLineIndex )
{
	if( nLineIndex < 0 ) return 0;
	if( nLineIndex >= ACUI_EDIT_MAX_LINE ) return 0;
	return m_alStringWidth[ nLineIndex ];
}

INT32 AcUIEdit::_GetStringHeight( INT32 nLineIndex )
{
	if( nLineIndex < 0 ) return 0;
	if( nLineIndex >= ACUI_EDIT_MAX_LINE ) return 0;
	return m_alStringHeight[ nLineIndex ];
}

INT32 AcUIEdit::_GetStringStartPosY( INT32 nLineIndex )
{
	if( nLineIndex < 0 ) return 0;
	if( nLineIndex >= ACUI_EDIT_MAX_LINE ) return 0;
	return m_alStringStartY[ nLineIndex ];
}
void AcUIEdit::_DrawStringAlignCenter( AgcdUIEditStringInfo* pStringList, AgcdFontClippingBox* pClippingBox,
									   INT32 nFontType, INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin )

{
	if( !pStringList ) return;
	if( !pClippingBox ) return;

	INT32 nStartX = 0;
	INT32 nStartY = 0;
	INT32 nOffSetX = 0;

	while( pStringList )
	{
		if( !m_bReadOnly && m_bActiveEdit && pStringList->lLineIndex == m_lCursorRow && m_lStartX )
		{
			nOffSetX = m_lStartX;
		}
		else
		{
			nOffSetX = ( m_lBoxWidth - ACUI_EDIT_LEFT_MARGIN * 2 - _GetStringWidth( pStringList->lLineIndex ) ) / 2;
		}

		nStartX = nAbsX + ACUI_EDIT_LEFT_MARGIN + nOffSetX;
		nStartY = nAbsY + m_lStartY + pStringList->lOffsetY;

		// ���ڿ� ��Ʈ�� ���� ��Ʈ�� �ٸ��� ��Ʈ����
		if( pStringList->lFontType != -1 && nFontType != pStringList->lFontType )
		{
			m_pAgcmFont->FontDrawEnd();
			m_pAgcmFont->FontDrawStart( pStringList->lFontType );
			nFontType = pStringList->lFontType;
		}

		// ���ڿ��� Ŭ���� �������� ���� �������� ��´�.
		if( nStartY + _GetStringHeight( pStringList->lLineIndex ) >= pClippingBox->y && nStartY <= pClippingBox->y + pClippingBox->h )
		{
			FLOAT fPosX = ( float )( nStartX + pStringList->lOffsetX + nScrollMargin );
			FLOAT fPosY = ( float )nStartY;
			UINT8 nAlpha = ( UINT8 )( 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1) );

			m_pAgcmFont->DrawTextIM2D( fPosX, fPosY, pStringList->szString, m_lFontType, nAlpha,
										pStringList->dwColor, true,
										pStringList->bBold ? true : false,
										pStringList->dwBGColor ? true : false,
										pStringList->dwBGColor,
										pStringList->lStringLength );
		}

		pStringList = pStringList->pstNext;
	}
}

void AcUIEdit::_DrawStringAlignRight( AgcdUIEditStringInfo* pStringList, AgcdFontClippingBox* pClippingBox,
										INT32 nFontType, INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin )
{
	if( !pStringList ) return;
	if( !pClippingBox ) return;

	INT32 nStartX = 0;
	INT32 nStartY = 0;
	INT32 nOffSetX = 0;

	while( pStringList )
	{
		nOffSetX = m_lBoxWidth - ACUI_EDIT_LEFT_MARGIN;
		nStartX = nAbsX + nOffSetX;

		nStartX = nStartX - _GetStringWidth( pStringList->lLineIndex );

		if( pStringList->lFontType != -1 && nFontType != pStringList->lFontType)
		{
			m_pAgcmFont->FontDrawEnd();
			m_pAgcmFont->FontDrawStart( pStringList->lFontType );
			nFontType = pStringList->lFontType;
		}

		nStartY = nAbsY + m_lStartY + pStringList->lOffsetY;

		if( nStartY + _GetStringHeight( pStringList->lLineIndex ) >= pClippingBox->y &&
			nStartY <= pClippingBox->y + pClippingBox->h )
		{
			FLOAT fPosX = ( float )( nStartX + pStringList->lOffsetX + nScrollMargin );
			FLOAT fPosY = ( float )nStartY;
			UINT8 nAlpha = ( UINT8 )( 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1) );

			m_pAgcmFont->DrawTextIM2D( fPosX, fPosY, pStringList->szString, m_lFontType, nAlpha,
										pStringList->dwColor, true,
										pStringList->bBold ? true : false,
										pStringList->dwBGColor ? true : false,
										pStringList->dwBGColor,
										pStringList->lStringLength );
		}

		pStringList = pStringList->pstNext;
	}
}

void AcUIEdit::_DrawStringAlignLeft( AgcdUIEditStringInfo* pStringList, AgcdFontClippingBox* pClippingBox,
										INT32 nFontType, INT32 nAbsX, INT32 nAbsY, INT32 nScrollMargin )
{
	if( !pStringList ) return;
	if( !pClippingBox ) return;

	INT32 nStartX = 0;
	INT32 nStartY = 0;
	INT32 nOffSetX = 0;

	while( pStringList )
	{
		nStartX = nAbsX + ACUI_EDIT_LEFT_MARGIN + m_lStartX;

		if( pStringList->lFontType != -1 && nFontType != pStringList->lFontType)
		{
			m_pAgcmFont->FontDrawEnd();
			m_pAgcmFont->FontDrawStart( pStringList->lFontType );
			nFontType = pStringList->lFontType;
		}

		nStartY = nAbsY + m_lStartY + pStringList->lOffsetY;

		if( nStartY + _GetStringHeight( pStringList->lLineIndex ) >= pClippingBox->y &&
			nStartY <= pClippingBox->y + pClippingBox->h)
		{
			FLOAT fPosX = ( float )( nStartX + pStringList->lOffsetX + nScrollMargin );
			FLOAT fPosY = ( float )nStartY;
			UINT8 nAlpha = ( UINT8 )( 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1) );

			m_pAgcmFont->DrawTextIM2D( fPosX, fPosY, pStringList->szString, m_lFontType, nAlpha,
										pStringList->dwColor, true,
										pStringList->bBold ? true : false,
										pStringList->dwBGColor ? true : false,
										pStringList->dwBGColor,
										pStringList->lStringLength );
		}

		pStringList = pStringList->pstNext;
	}
}

void AcUIEdit::_DrawCaretComposer( INT32 nAbsX, INT32 nAbsY, INT32 nOffSetX, INT32 nScrollMargin )
{
	FLOAT fPosX = ( FLOAT )( nAbsX + nOffSetX + nScrollMargin );
	FLOAT fPosY = ( FLOAT )nAbsY + m_lCursorPosY;
	UINT8 nAlpha = ( UINT8 )( 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1) );

	if( m_lCursorCount < 500 )
	{
		if( g_pEngine->GetIMEComposingState() )
		{
			m_pAgcmFont->DrawTextIM2D( fPosX, fPosY, m_szComposing, m_lFontType, nAlpha, m_lColor );
		}
	}
	else 
	{
		if( m_bActiveEdit )
		{
			if( g_pEngine->GetIMEComposingState() )
			{
				m_pAgcmFont->DrawCaret( fPosX, fPosY, m_lFontType, m_lColor,
										m_pAgcmFont->GetTextExtent( m_lFontType, m_szComposing, strlen( m_szComposing ) ) );
				m_pAgcmFont->DrawTextIM2D( fPosX, fPosY, m_szComposing, m_lFontType, nAlpha, m_lColor );
			}
			else
			{
				m_pAgcmFont->DrawCaretE( fPosX, fPosY, m_lFontType, m_lColor );
			}
		}

		if( m_lCursorCount > 1000 )
		{
			m_lCursorCount = 0;
		}
	}

	// �ĺ� ������ �׸���
	if( m_bActiveEdit )
	{
		AuInputComposer::GetInstance().SetEditPosition( ( INT32 )fPosX, ( INT32 )( fPosY + 20.0f ) );

		//if(g_eServiceArea != AP_SERVICE_AREA_GLOBAL)
		{
			// reading window �׸���
			//DrawReadingWindow( fPosX, fPosY );
			POINT & point = AuInputComposer::GetInstance().GetCandidatePos();
			DrawReadingWindow( (FLOAT)point.x, (FLOAT)point.y );

			// candidate window �׸���
			//DrawCandidateWindow( ( FLOAT )( nAbsX + nScrollMargin + ACUI_EDIT_LEFT_MARGIN ), fPosY );
			DrawCandidateWindow( (FLOAT)point.x, (FLOAT)point.y );
		}
	}
}

BOOL AcUIEdit::_ProcessKeyEnter( char* pInputChar )
{
	if( !pInputChar ) return FALSE;
	if( *pInputChar == VK_RETURN && !m_bMultiLine )
	{
		if( g_pEngine->GetCharCheckState() )
		{
			// ���� �Է����̴ٰ� �Է��� ����� ������ ó��..
			if( !m_bActiveEdit ) return FALSE; // ������ ������ Enter�� �ƴϴ� 
			if( pParent )
			{
				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_EDIT_INPUT_END, ( PVOID )&m_lControlID );
			}

			if( m_bReleaseEditInputEnd )
			{
				ReleaseMeActiveEdit();
			}
		}
		else if( !m_bReadOnly )
		{
			// �Է� ������ ���� �Է´�� ���·� ��ȯ
			SetMeActiveEdit();
		}

		return FALSE;
	}

	return TRUE;
}

BOOL AcUIEdit::_ProcessKeyESC( char* pInputChar )
{
	if( !pInputChar ) return FALSE;
	if( *pInputChar == VK_ESCAPE )
	{
		BOOL bRet = TRUE;
		ClearText();

		if( pParent )
		{
			bRet = pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_EDIT_INPUT_END, ( PVOID )&m_lControlID );
		}

		// ��Ƽ�� ������.
		ReleaseMeActiveEdit();
		return bRet;
	}

	return TRUE;
}

BOOL AcUIEdit::_ProcessKeyBackSpace( char* pInputChar )
{
	if( !pInputChar ) return FALSE;
	if( *pInputChar == VK_BACK )
	{
		// ������ �� �ѱ����� ���̸� ���Ѵ�.
		INT32 lCharLength = 0;

		// Start Of Line�� ���
		if( CheckSOL() )
		{
			if( m_lCursorRow )
			{
				lCharLength	= m_lDelimiterLength;
			}
		}
		// �ѱ��� ���
		else if( m_lCursorPoint > 1 && IsDBC( ( unsigned char* )&m_szText[ m_lCursorPoint - 1 ] ) )
		{
			lCharLength	= 2;
		}
		// '<<'�� ���
		else if( m_bEnableTag && m_lCursorPoint > 1 && m_szText[ m_lCursorPoint - 1 ] == '<' && m_szText[ m_lCursorPoint - 2 ] == '<' )
		{
			lCharLength = 2;
		}
		// �ƴ� ���
		else if( m_lCursorPoint > 0 )
		{
			lCharLength = 1;
		}

		// ����� Ŀ����ġ ����..
		strcpy( &m_szText[ m_lCursorPoint - lCharLength ], &m_szText[ m_lCursorPoint ] );
		m_lCursorPoint -= lCharLength;
	}

	return SetText();
}

BOOL AcUIEdit::_ProcessKeyTab( char* pInputChar )
{
	if( !pInputChar ) return FALSE;
	if( *pInputChar == VK_TAB )
	{
		if( pParent )
		{
			if( GetKeyState( VK_SHIFT ) < 0 )
			{
				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_EDIT_INPUT_SHIFT_TAB, &m_lControlID );
			}
			else
			{
				pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_EDIT_INPUT_TAB, &m_lControlID );
			}
		}
	}

	return SetText();
}

BOOL AcUIEdit::_ProcessKeyText( char* pInputChar )
{
	if( !pInputChar )
		return FALSE;

	if( !m_bChangeText )
		m_bChangeText = TRUE;
	if( strlen( pInputChar ) == 1 && *pInputChar >= 0 && *pInputChar < 0x20 && *pInputChar != VK_RETURN )
	{
		// ASCII �ڵ�¿��� 0x20 ���ϴ� ��Ʈ�� ���̹Ƿ�..
		// ó������ �����͵��� �׳� ��ŵ ���� ������.
		return SetText();
	}

	// �׿ܿ� Buffer�� �ְ� SetText
	CHAR* pString =	pInputChar;
	INT32 lStringLength;
	BOOL bNewLine =	FALSE;

	// ����Ű�� ��Ƽ������ ���Ǹ�..
	if( *pInputChar == VK_RETURN && m_bMultiLine )
	{
		// ���� �����ְ�..
		pString	= &m_szLineDelimiter[ 0 ];
		lStringLength = m_lDelimiterLength;
		bNewLine = TRUE;
	}
	// ����Ű�� ����ϴµ� ����Ű�� ������..
	else if( m_bNumberOnly && ( *pInputChar < 48 || *pInputChar > 57 ) )
	{
		// NumberOnly�ε� ���ڰ� �ƴϴ�. NOP
	}
	// 2����Ʈ ���ڸ�..
	else if( IsDBC( ( unsigned char* )( pInputChar + 1) ) ) // DBC, OnComposition�� ���ؼ� �̸��� ���´�. ���� ������ ���� ����.
	{
		lStringLength = strnlen( pInputChar, m_ulTextMaxLength );
		memset( m_szComposing, 0, sizeof( m_szComposing ) );
	}
	else
	{
		// �̵� ���� �ƴϸ� ���ڱ��̴� 1
		lStringLength =	1;
	}

	// �ִ� �ؽ�Ʈ �Է� ������ �̳�������..
	if( m_lCursorPoint + lStringLength <= ( int ) m_ulTextMaxLength )
	{
		INT32 lIndex;
		for( lIndex = m_lTextLength - 1; lIndex >= m_lCursorPoint; --lIndex )
		{
			m_szText[ lIndex + lStringLength ] = m_szText[ lIndex ];
		}

		memcpy( &m_szText[ m_lCursorPoint ], pString, lStringLength );
		//strncpy(&m_szText[m_lCursorPoint], pString, lStringLength);

		m_szText[ m_lTextLength + lStringLength ] =	0;
		m_lCursorPoint += lStringLength;
	}

	// Ȯ���� ���ڿ��� ���� ������ ����ش�.
	return SetText();
}

BOOL AcUIEdit::_ProcessKeyLeft( void )
{
	BOOL bExitLoop = FALSE;
	while( !bExitLoop )
	{
		bExitLoop = TRUE;

		// ���� ���� �� ó���� �ִ� ��� ���ٷ� �ö󰡾� �Ѵ�.
		if( CheckSOL() )
		{
			m_lCursorPoint	-=	m_lDelimiterLength;
		}
		// ���� ��ġ�κ��� ���� 2���� ���ڸ� �ϳ��� ����ó�� ����ؾ� �Ҷ�..
		else if( m_bEnableTag && m_lCursorPoint > 1 && m_szText[m_lCursorPoint - 1] == '>' )
		{
			INT32 lIndex;
			INT32 lIndex2;
			INT32 lNum = 0;

			for( lIndex = m_lCursorPoint - 1; lIndex >= 0; --lIndex )
			{
				if( m_szText[ lIndex ] == '<' )	break;
			}

			for( lIndex2 = lIndex; lIndex2 >= 0; --lIndex2 )
			{
				if( m_szText[lIndex2] == '<' )
				{
					++lNum;
				}
				else
				{
					break;
				}
			}

			// ���� '<'�� ¦������, '>'�� �ٸ� �ǹ̸� ������ �ʴ´�.
			if( lNum % 2 == 0 )
			{
				--m_lCursorPoint;
			}
			else
			{
				m_lCursorPoint = lIndex;
				bExitLoop = FALSE;
			}
		}
		// ���� Ŀ����ġ���� ���� ���ڰ� 2����Ʈ �����϶�..
		else if( m_lCursorPoint > 1 && IsDBC( ( unsigned char* )&m_szText[ m_lCursorPoint - 1 ] ) )
		{
			m_lCursorPoint	-=	2;
		}
		// �� ���� ��쿣 �� ��ĭ ������..
		else if( m_lCursorPoint > 0 )
		{
			--m_lCursorPoint;
		}
	}

	// ���ڿ� �����ؼ� ����ش�. ���ڿ� ������ ����� �� �ƴ����� ��ũ�ѵ��� ���
	// ȭ��� ���̴� ������ �޶����� ������ �������..
	return SetText();
}

BOOL AcUIEdit::_ProcessKeyRight( void )
{
	BOOL bExitLoop = FALSE;
	while( !bExitLoop )
	{
		bExitLoop = TRUE;

		// �Ʒ��ٷ� �Ѿ�� �ϴ°��..
		if( CheckEOL() )
		{
			m_lCursorPoint	+=	m_lDelimiterLength;
		}
		// 2���� ���ڸ� �ϳ�ó�� ó���ؾ� �ϴ� ���
		else if( m_bEnableTag && m_szText[ m_lCursorPoint ] == '<' )
		{
			INT32 lIndex;

			// '<<' �̸� ����
			if( m_lCursorPoint < m_lTextLength - 1 && m_szText[ m_lCursorPoint + 1 ] == '<' )
			{
				m_lCursorPoint += 2;
			}
			else
			{
				for( lIndex = m_lCursorPoint + 1 ; lIndex < m_lTextLength ; ++lIndex )
				{
					if( m_szText[ lIndex ] == '>' )
					{
						break;
					}
				}

				if( lIndex != m_lTextLength )
				{
					m_lCursorPoint = lIndex + 1;
					bExitLoop = FALSE;
				}
			}
		}
		// ���� ���ڰ� 2����Ʈ ������ ���
		else if( m_lCursorPoint < m_lTextLength - 1 && IsDBC( ( unsigned char* )&m_szText[ m_lCursorPoint + 1 ] ) )
		{
			m_lCursorPoint	+=	2;
		}
		// �� ���� ��쿡�� �� ��ĭ �ڷ� �̵�
		else if( m_lCursorPoint < m_lTextLength )
		{
			++m_lCursorPoint;
		}
	}

	return SetText();
}

BOOL AcUIEdit::_ProcessKeyUp( void )
{
	if( !m_pstStringInfoCursor ) return FALSE;

	INT32 lIndex;
	INT32 lIndexOK = 0;
	INT32 lCursorX = m_lCursorPosX - m_pstStringInfoCursor->lOffsetX - ACUI_EDIT_LEFT_MARGIN;

	AgcdUIEditStringInfo* pstIter;
	AgcdUIEditStringInfo* pstPrevLine = NULL;
	AgcdUIEditStringInfo* pstCurrLine = NULL;

	UINT32 ulCursorGap = 0xffffff;
	UINT32 ulTemp;
	BOOL bFound = FALSE;

	for( pstIter = m_pstStringInfoHead ; pstIter ; pstIter = pstIter->pstNext )
	{
		if( pstIter->bNewLine )
		{
			pstPrevLine = pstCurrLine;
			pstCurrLine = pstIter;
		}

		if( m_pstStringInfoCursor == pstIter ) break;
	}

	// ���� �ö� ���� ������ ( �������� �� �����̸� ) �Ѿ��.
	if( !pstPrevLine ) return FALSE;
	pstIter = pstPrevLine;

	do
	{
		// Ŀ���� ��ġ�ؾ� �� ���� ã�´�.
		if( lCursorX < pstIter->lOffsetX + pstIter->lWidth )
		{
			for( lIndex = 0 ; lIndex < pstIter->lStringLength ; ++lIndex )
			{
				if( pstIter->szString[ lIndex ] & 0x80 )
				{
					++lIndex;
				}

				ulTemp = abs( pstIter->lOffsetX + m_pAgcmFont->GetTextExtent( pstIter->lFontType, pstIter->szString, lIndex ) - lCursorX );
				if( ulCursorGap > ulTemp )
				{
					lIndexOK = lIndex;
					ulCursorGap = ulTemp;
				}
			}

			bFound = TRUE;
			break;
		}

		pstCurrLine = pstIter;
		pstIter = pstIter->pstNext;
	}
	while( pstIter && !pstIter->bNewLine );

	if( bFound )
	{
		m_lCursorPoint = m_lCursorPointDisplay = pstIter->szString + lIndexOK - m_szTextDisplay;
	}
	else
	{
		m_lCursorPoint = m_lCursorPointDisplay = pstCurrLine->szString + pstCurrLine->lStringLength - m_szTextDisplay;
	}

	return SetText();
}

BOOL AcUIEdit::_ProcessKeyDown( void )
{
	INT32 lIndex;
	INT32 lIndexOK = 0;
	INT32 lCursorX = 0;

	AgcdUIEditStringInfo* pstIter = m_pstStringInfoCursor;
	AgcdUIEditStringInfo* pstPrevLine = NULL;
	AgcdUIEditStringInfo* pstCurrLine = NULL;

	UINT32 ulCursorGap = 0xffffff;
	UINT32 ulTemp;
	BOOL bFound = FALSE;

	if( !pstIter )
	{
		pstIter = m_pstStringInfoHead;
	}
	else
	{
		lCursorX = m_lCursorPosX - m_pstStringInfoCursor->lOffsetX - ACUI_EDIT_LEFT_MARGIN;
	}

	// ������ ���� ����;;
	if( !pstIter ) return FALSE;
	for( pstIter = pstIter->pstNext ; pstIter ; pstIter = pstIter->pstNext )
	{
		if( pstIter->bNewLine ) break;
	}

	if( !pstIter ) return FALSE;

	// Ŀ�� ��ġ ã��..
	do
	{
		if( lCursorX < pstIter->lOffsetX + pstIter->lWidth )
		{
			for( lIndex = 0 ; lIndex < pstIter->lStringLength ; ++lIndex )
			{
				if( pstIter->szString[ lIndex ] & 0x80 )
				{
					++lIndex;
				}

				ulTemp = abs( pstIter->lOffsetX + m_pAgcmFont->GetTextExtent( pstIter->lFontType, pstIter->szString, lIndex ) - lCursorX );
				if( ulCursorGap > ulTemp )
				{
					lIndexOK = lIndex;
					ulCursorGap = ulTemp;
				}
			}

			bFound = TRUE;
			break;
		}

		pstCurrLine = pstIter;
		pstIter = pstIter->pstNext;
	}
	while( pstIter && !pstIter->bNewLine );

	if( bFound )
	{
		m_lCursorPoint = m_lCursorPointDisplay = pstIter->szString + lIndexOK - m_szTextDisplay;
	}
	else
	{
		m_lCursorPoint = m_lCursorPointDisplay = pstCurrLine->szString + pstCurrLine->lStringLength - m_szTextDisplay;
	}

	return SetText();
}

BOOL AcUIEdit::_ProcessKeyDelete( void )
{
	// �������� ���ڰ� ������..
	if( strlen( m_szComposing ) )		// �������� ���� �Է� ó��
	{
		// �����ϴ� ���ڸ� �����صд�.
		strncpy( &m_szText[ m_lCursorPoint ], m_szComposing, 2 );
		memset( m_szComposing, 0, sizeof( m_szComposing ) );
		m_lCursorPoint += 2;
	}

	// ���ڿ��� ������ Delete Ű�� �������� �ƴҶ��� ó��.. ���̶�� �Ұ� ����.
	if( m_szText[ m_lCursorPoint ] != '\0' )
	{
		if( m_szText[ m_lCursorPoint ] & 0x80 ) // �ѱ��̶��
		{
			// ���� Ŀ�� ��ġ�� �������� 2����Ʈ�� ������ �����.
			for( int i = m_lCursorPoint ; i < m_lTextLength ; ++i )
			{
				if( i + 2 < ( int )m_ulTextMaxLength )
				{
					m_szText[ i ] = m_szText[ i + 2 ];
				}
			}
		}
		else
		{
			// �ѱ��� �ƴϴ� 1����Ʈ�� �����.
			for( int i = m_lCursorPoint ; i < m_lTextLength ; ++i )
			{
				if( i + 1 < ( int )m_ulTextMaxLength )
				{
					m_szText[ i ] = m_szText[ i + 1 ];
				}
			}
		}
	}

	return SetText();
}

BOOL AcUIEdit::_UpdateCursorPosition( AgcdUIEditStringInfo* pStringList )
{
	INT32 lLine = 0;
	while( pStringList )
	{
		if( m_szTextDisplay + m_lCursorPointDisplay < pStringList->szString )
		{
			m_lCursorRow = lLine - 1;
			m_lCursorColumn = m_szTextDisplay + m_lCursorPointDisplay - m_pstStringInfoCursor->szString;
			break;
		}

		if( pStringList->bNewLine )
		{
			++lLine;
		}

		m_pstStringInfoCursor = pStringList;
		pStringList = pStringList->pstNext;
	}

	if( !pStringList )
	{
		m_lCursorRow = lLine - 1;
		m_lCursorColumn = m_szTextDisplay + m_lCursorPointDisplay - m_pstStringInfoCursor->szString;
	}

	// Cursor Position ����
	if( !m_bReadOnly )
	{
		// �ϴ� ������ ���..
		if( m_eVAlign == ACUIBASE_VALIGN_BOTTOM )
		{
			m_lStartY =	h - _GetStringStartPosY( m_ulLine );
			m_lCursorPosY =	m_lStartY + _GetStringStartPosY( m_lCursorRow );
		}
		else
		{
			// Y ��ǥ ��� (���� Y��ǥ�� window height���� ũ�� ����)
			m_lCursorPosY =	_GetStringStartPosY( m_lCursorRow );
			if( m_lCursorPosY + _GetStringHeight( m_lCursorRow ) > h )
			{
				m_lStartY =	h - _GetStringStartPosY( m_ulLine );
				m_lCursorPosY =	m_lStartY + _GetStringStartPosY( m_lCursorRow );
			}

			if( m_lCursorPosY < 0 )
			{
				m_lCursorPosY =	0;
				m_lStartY =	m_nOffSetFromTopEdge;
			}

			// �߾������� ���..
			if( m_lStartY == m_nOffSetFromTopEdge && m_eVAlign == ACUIBASE_VALIGN_CENTER )
			{
				m_lStartY = ( h - _GetStringStartPosY( m_ulLine ) ) / 2;
				m_lCursorPosY = m_lStartY + _GetStringStartPosY( m_lCursorRow );
			}
		}

		if( !m_pAgcmFont )
		{
			m_lCursorPosX = 0;
		}
		else if( !m_pstStringInfoCursor )
		{
			m_lCursorPosX = ACUI_EDIT_LEFT_MARGIN + 
				m_pAgcmFont->GetTextExtent( m_lFontType, m_szTextDisplay, m_lCursorColumn );
		}
		else
		{
			m_lCursorPosX = m_pstStringInfoCursor->lOffsetX + ACUI_EDIT_LEFT_MARGIN + 
				m_pAgcmFont->GetTextExtent( m_pstStringInfoCursor->lFontType, m_pstStringInfoCursor->szString, m_lCursorColumn );
		}

		if( m_lCursorPosX > m_lBoxWidth )
		{
			m_lStartX =	m_lBoxWidth - ACUI_EDIT_LEFT_MARGIN - m_lCursorPosX;
			m_lCursorPosX =	m_lBoxWidth - ACUI_EDIT_LEFT_MARGIN;
		}
		else
		{
			m_lStartX =	0;
		}
	}
	 //�б� ������ ���¿��� ���߿� �� ����� ���� �̸� ���� ����صд�.
	else
	{
		if( m_eVAlign == ACUIBASE_VALIGN_BOTTOM )
		{
			m_lStartY =	h - _GetStringStartPosY( m_ulLine );
			m_lCursorPosY =	m_lStartY + _GetStringStartPosY( m_lCursorRow );
		}
		else if( m_eVAlign == ACUIBASE_VALIGN_CENTER )
		{
			m_lStartY =	( h - _GetStringStartPosY( m_ulLine ) ) / 2;
			m_lCursorPosY = m_lStartY + _GetStringStartPosY( m_lCursorRow );
		}
	}

	return TRUE;
}

BOOL AcUIEdit::_UpdateScrollBarVert( void )
{
	if( m_pcsVScroll )
	{
		return m_pcsVScroll->SetScrollValue( -m_lStartY / ( _GetStringStartPosY( m_ulLine ) - h + 0.0f ) );
	}

	return FALSE;
}

