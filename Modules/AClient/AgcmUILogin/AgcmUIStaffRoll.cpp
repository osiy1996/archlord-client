#include "AgcmUIStaffRoll.h"
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AuMD5EnCrypt/AuMD5EnCrypt.h>
#include <AgcModule/AgcEngine.h>


void stStaffRollImageEntry::Clear( void )
{
	m_pTexture = NULL;
	memset( m_strFileName, 0, sizeof( char ) * STRING_LENGTH_FILENAME );

	m_nWidth = 0;
	m_nHeight = 0;

	m_nPosX = 0;
	m_nPosY = 0;

	m_eState = ImageState_UnKnown;

	m_nTimeStart = 0;
	m_nTimeFadeInEnd = 0;
	m_nDuration = 0;

	m_nAlpha = 0;
}

BOOL stStaffRollImageEntry::LoadFromXML( void* pNode )
{
	Clear();

	TiXmlNode* pXmlNode = ( TiXmlNode* )pNode;
	if( !pXmlNode ) return FALSE;

	const char* pFileName = pXmlNode->ToElement()->Attribute( "FileName" );
	if( pFileName && strlen( pFileName ) >= 0 )
	{
		strcpy_s( m_strFileName, sizeof( char ) * STRING_LENGTH_FILENAME, pFileName );
		m_pTexture = RwTextureRead( m_strFileName, NULL );
		if( !m_pTexture )
		{
#ifdef _DEBUG
			char strDebug[ 256 ] = { 0, };
			sprintf_s( m_strFileName, sizeof( char ) * STRING_LENGTH_FILENAME, "Failed to load StaffRoll Image, FileName = %s\n", m_strFileName );
			OutputDebugString( strDebug );
#endif
			return FALSE;
		}
	}

	const char* pWidth = pXmlNode->ToElement()->Attribute( "Width" );
	if( pWidth && strlen( pWidth ) >= 0 )
	{
		m_nWidth = atoi( pWidth );
	}

	const char* pHeight = pXmlNode->ToElement()->Attribute( "Height" );
	if( pHeight && strlen( pHeight ) >= 0 )
	{
		m_nHeight = atoi( pHeight );
	}

	const char* pPosX = pXmlNode->ToElement()->Attribute( "PosX" );
	if( pPosX && strlen( pPosX ) >= 0 )
	{
		m_nPosX = atoi( pPosX );
	}

	const char* pPosY = pXmlNode->ToElement()->Attribute( "PosY" );
	if( pPosY && strlen( pPosY ) >= 0 )
	{
		m_nPosY = atoi( pPosY );
	}

	const char* pDuration = pXmlNode->ToElement()->Attribute( "Duration" );
	if( pDuration && strlen( pDuration ) >= 0 )
	{
		m_nDuration = _atoi64( pDuration );
	}

	return TRUE;
}

BOOL stStaffRollImageEntry::Start( __int64 nTime )
{
	m_eState = ImageState_FadeIn;
	m_nTimeStart = nTime;
	m_nTimeFadeInEnd = 0;
	m_nAlpha = 0;
	return TRUE;
}

BOOL stStaffRollImageEntry::Update( __int64 nTime, __int64 nFadeSpeed )
{
	switch( m_eState )
	{
	case ImageState_FadeIn :
		{
			if( nFadeSpeed <= 0 ) return FALSE;

			m_nAlpha = ( int )( ( nTime - m_nTimeStart ) / nFadeSpeed ) * 255;
			if( m_nAlpha >= 255 )
			{
				m_nAlpha = 255;
				m_eState = ImageState_Idle;
				m_nTimeFadeInEnd = nTime;
			}
		}
		break;

	case ImageState_Idle :
		{
			__int64 nElapsedTime = nTime - m_nTimeFadeInEnd;
			if( nElapsedTime >= m_nDuration )
			{
				m_eState = ImageState_FadeOut;
				m_nTimeStart = nTime;
				m_nTimeFadeInEnd = 0;
			}
		}
		break;

	case ImageState_FadeOut :
		{
			if( nFadeSpeed <= 0 ) return FALSE;

			m_nAlpha = 255 - ( int )( ( ( nTime - m_nTimeStart ) / nFadeSpeed ) * 255 );
			if( m_nAlpha <= 0 )
			{
				m_nAlpha = 0;
				m_eState = ImageState_UnKnown;
				m_nTimeStart = 0;
				m_nTimeFadeInEnd = 0;
			}
		}
		break;
	}

	return TRUE;
}

void stUiStaffRoll_TextStyleEntry::Clear( void )
{
	memset( m_strName, 0, sizeof( char ) * STRING_LENGTH_NAME );

	m_eHAlign = HAlign_None;
	m_nFontType = 0;
	m_nFontSize = 0;
	m_dwFontColor = 0xFFFFFFFF;
}

BOOL stUiStaffRoll_TextStyleEntry::LoadFromXML( void* pNode )
{
	Clear();

	TiXmlNode* pXmlNode = ( TiXmlNode* )pNode;
	if( !pXmlNode ) return FALSE;

	const char* pStyleName = pXmlNode->ToElement()->Attribute( "Name" );
	if( pStyleName && strlen( pStyleName ) > 0 )
	{
		strcpy_s( m_strName, sizeof( char ) * STRING_LENGTH_NAME, pStyleName );
	}

	const char* pTextHAlign = pXmlNode->ToElement()->Attribute( "HAlign" );
	if( pTextHAlign && strlen( pTextHAlign ) > 0 )
	{
		if( strcmp( pTextHAlign, "Left" ) == 0 ) m_eHAlign = HAlign_Left;
		else if( strcmp( pTextHAlign, "Center" ) == 0 ) m_eHAlign = HAlign_Center;
		else if( strcmp( pTextHAlign, "Right" ) == 0 ) m_eHAlign = HAlign_Right;
		else m_eHAlign = HAlign_None;
	}

	const char* pFontType = pXmlNode->ToElement()->Attribute( "FontType" );
	if( pFontType && strlen( pFontType ) > 0 )
	{
		m_nFontType = atoi( pFontType );
	}

	const char* pFontSize = pXmlNode->ToElement()->Attribute( "FontSize" );
	if( pFontSize && strlen( pFontSize ) > 0 )
	{
		m_nFontSize = atoi( pFontSize );
	}

	const char* pFontColorR = pXmlNode->ToElement()->Attribute( "ColorRed" );
	const char* pFontColorG = pXmlNode->ToElement()->Attribute( "ColorGreen" );
	const char* pFontColorB = pXmlNode->ToElement()->Attribute( "ColorBlue" );
	if( pFontColorR && strlen( pFontColorR ) > 0 && pFontColorG && strlen( pFontColorG ) > 0 && pFontColorB && strlen( pFontColorB ) > 0 )
	{
		int nRed = atoi( pFontColorR );
		int nGreen = atoi( pFontColorG );
		int nBlue = atoi( pFontColorB );
		int nAlpha = 255;

		m_dwFontColor = nAlpha << 24 | nRed << 16 | nGreen << 8 | nBlue;
	}
	return TRUE;
}

void stUiStaffRoll_TextEntry::Clear( void )
{
	memset( m_strStyleName, 0, sizeof( char ) * STRING_LENGTH_NAME );
	memset( m_strString, 0, sizeof( char ) * STRING_LENGTH_LINETEXT );
}

BOOL stUiStaffRoll_TextEntry::LoadFromXML( void* pNode )
{
	Clear();

	TiXmlNode* pXmlNode = ( TiXmlNode* )pNode;
	if( !pXmlNode ) return FALSE;

	const char* pStyleName = pXmlNode->ToElement()->Attribute( "StyleName" );
	if( pStyleName && strlen( pStyleName ) > 0 )
	{
		strcpy_s( m_strStyleName, sizeof( char ) * STRING_LENGTH_NAME, pStyleName );
	}

	const char* pString = pXmlNode->ToElement()->Attribute( "String" );
	if( pString && strlen( pString ) > 0 )
	{
		strcpy_s( m_strString, sizeof( char ) * STRING_LENGTH_LINETEXT, pString );
	}

	return TRUE;
}




AgcmUIStaffRoll::AgcmUIStaffRoll( void )
{
	memset( m_strSettingFileName, 0, sizeof( char ) * STRING_LENGTH_FILENAME );
	m_bIsEncryption = FALSE;

	m_eSwitchType = SwitchType_UnKnown;
	m_nScreenWidth = 0;
	m_nScreenHeight = 0;

	m_nTextAreaPosX = 0;
	m_nTextAreaPosY = 0;
	m_nTextAreaWidth = 0;
	m_nTextAreaHeight = 0;

	m_nScrollSpeed = 0;
	m_nFadeSpeed = 0;

	m_ePlayType = ImagePlayType_UnKnown;
	m_nCurrImage = 0;
	m_nNextImage = 0;
}

AgcmUIStaffRoll::~AgcmUIStaffRoll( void )
{
}

BOOL AgcmUIStaffRoll::OnLoad( char* pFileName, BOOL bIsEncryption )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	if( bIsEncryption )
	{
		HANDLE hFile = ::CreateFile( pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		if( hFile == INVALID_HANDLE_VALUE ) return FALSE;

		// 마지막에 NULL 문자열을 추가해야 하니까 파일사이즈 + 1 해서 초기화한다.
		DWORD dwBufferSize = ::GetFileSize( hFile, NULL ) + 1;
		char* pBuffer = new char[ dwBufferSize ];
		memset( pBuffer, 0, sizeof( char ) * dwBufferSize );

		DWORD dwReadByte = 0;
		if( ::ReadFile( hFile, pBuffer, dwBufferSize, &dwReadByte, NULL ) )
		{
			AuMD5Encrypt Cryptor;
			if( Cryptor.DecryptString( "1111", pBuffer, dwReadByte ) )
			{
				m_Doc.Parse( pBuffer );
				if( !m_Doc.Error() )
				{
					delete[] pBuffer;
					pBuffer = NULL;

					::CloseHandle( hFile );
					hFile = NULL;
					return FALSE;
				}
			}
		}
		else
		{
			delete[] pBuffer;
			pBuffer = NULL;

			::CloseHandle( hFile );
			hFile = NULL;
			return FALSE;
		}

		delete[] pBuffer;
		pBuffer = NULL;

		::CloseHandle( hFile );
		hFile = NULL;
	}
	else
	{
		if( !m_Doc.LoadFile( pFileName ) ) return FALSE;
	}

	if( !_ParseXmlDocument() ) return FALSE;

	memset( m_strSettingFileName, 0, sizeof( char ) * STRING_LENGTH_FILENAME );
	strcpy_s( m_strSettingFileName, sizeof( char ) * STRING_LENGTH_FILENAME, pFileName );
	m_bIsEncryption = bIsEncryption;

	return TRUE;
}

BOOL AgcmUIStaffRoll::OnReload( void )
{
	m_Doc.Clear();
	return OnLoad( m_strSettingFileName, m_bIsEncryption );
}

BOOL AgcmUIStaffRoll::OnEnter( __int64 nTime )
{
	m_nCurrImage = 0;
	m_nNextImage = 1;

	stStaffRollImageEntry* pEntryCurr = m_vecImages.Get( m_nCurrImage );
	if( !pEntryCurr ) return FALSE;

	pEntryCurr->Start( nTime );
	return TRUE;
}

BOOL AgcmUIStaffRoll::OnUpdate( __int64 nTime )
{
	_UpdateImage( nTime );
	_UpdateScroll( nTime );
	return TRUE;
}

BOOL AgcmUIStaffRoll::OnRender( void )
{
	return TRUE;
}

BOOL AgcmUIStaffRoll::OnLeave( void )
{
	m_nCurrImage = 0;
	m_nNextImage = 0;
	return TRUE;
}

BOOL AgcmUIStaffRoll::_ParseXmlDocument( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUIMode eCurrentUIMode = pcmUIManager->GetUIMode();

	TiXmlNode* pRoot = m_Doc.FirstChild( "StaffRollSetting" );
	if( !pRoot ) return FALSE;

	TiXmlNode* pLayout = pRoot->FirstChild( "Layout" );
	while( pLayout )
	{
		const char* pWidth = pLayout->ToElement()->Attribute( "Width" );
		const char* pHeight = pLayout->ToElement()->Attribute( "Height" );
		if( pWidth && strlen( pWidth ) > 0 && pHeight && strlen( pHeight ) > 0  )
		{
			int nWidth = atoi( pWidth );
			int nHeight = atoi( pHeight );

			switch( eCurrentUIMode )
			{
			case AGCDUI_MODE_1024_768 :		if( nWidth != 1024 || nHeight != 768 ) continue;	break;
			case AGCDUI_MODE_1280_1024 :	if( nWidth != 1280 || nHeight != 1024 ) continue;	break;
			case AGCDUI_MODE_1600_1200 :	if( nWidth != 1600 || nHeight != 1200 ) continue;	break;
			default :						return FALSE;										break;			
			}

			m_nScreenWidth = nWidth;
			m_nScreenHeight = nHeight;

			if( !_LoadStaffRollLayout( pLayout ) ) return FALSE;
			if( !_LoadStaffRollText( pLayout ) ) return FALSE;
			return TRUE;
		}		

		pLayout = pLayout->NextSibling();
	}

	return FALSE;
}

BOOL AgcmUIStaffRoll::_LoadStaffRollLayout( void* pLayout )
{
	TiXmlNode* pNode = ( TiXmlNode* )pLayout;
	if( !pNode ) return FALSE;

	TiXmlNode* pNodeImage = pNode->FirstChild( "Image" );
	if( !pNodeImage ) return FALSE;

	TiXmlNode* pNodeImageBG = pNode->FirstChild( "ImageBG" );
	if( !m_stmageBG.LoadFromXML( pNodeImageBG ) ) return FALSE;
	
	TiXmlNode* pNodeImageAlphaTop = pNode->FirstChild( "ImageAlphaTop" );
	if( !m_stmageAlphaTop.LoadFromXML( pNodeImageAlphaTop ) ) return FALSE;

	TiXmlNode* pNodeImageAlphaBottom = pNode->FirstChild( "ImageAlphaBottom" );
	if( !m_stImageAlphaBottom.LoadFromXML( pNodeImageAlphaBottom ) ) return FALSE;

	TiXmlNode* pNodeImageList = pNode->FirstChild( "ImageList" );
	if( !_LoadStaffRollImageList( pNodeImageList ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIStaffRoll::_LoadStaffRollImageList( void* pImageList )
{
	TiXmlNode* pNode = ( TiXmlNode* )pImageList;
	if( !pNode ) return FALSE;

	const char* pSwitchType = pNode->ToElement()->Attribute( "SwitchType" );
	if( !pSwitchType || strlen( pSwitchType ) <= 0 ) return FALSE;

	if( strcmp( pSwitchType, "Fade" ) == 0 ) m_eSwitchType = SwitchType_Fade;
	else if( strcmp( pSwitchType, "CrossFade" ) == 0 ) m_eSwitchType = SwitchType_CrossFade;

	const char* pFadeSpeed = pNode->ToElement()->Attribute( "FadeSpeed" );
	if( pFadeSpeed && strlen( pFadeSpeed ) > 0 )
	{
		m_nFadeSpeed = _atoi64( pFadeSpeed );
	}

	m_vecImages.Clear();
	TiXmlNode* pNodeImage = pNode->FirstChild( "ImageEntry" );
	while( pNodeImage )
	{
		stStaffRollImageEntry NewEntry;
		NewEntry.LoadFromXML( pNodeImage );

		m_vecImages.Add( NewEntry );
		pNodeImage = pNodeImage->NextSibling();
	}

	return TRUE;
}

BOOL AgcmUIStaffRoll::_LoadStaffRollText( void* pText )
{
	TiXmlNode* pNode = ( TiXmlNode* )pText;
	if( !pNode ) return FALSE;

	const char* pPosX = pNode->ToElement()->Attribute( "PosX" );
	if( pPosX && strlen( pPosX ) > 0 )
	{
		m_nTextAreaPosX = atoi( pPosX );
	}

	const char* pPosY = pNode->ToElement()->Attribute( "PosY" );
	if( pPosY && strlen( pPosY ) > 0 )
	{
		m_nTextAreaPosX = atoi( pPosY );
	}

	const char* pWidth = pNode->ToElement()->Attribute( "Width" );
	if( pWidth && strlen( pWidth ) > 0 )
	{
		m_nTextAreaPosX = atoi( pWidth );
	}

	const char* pHeight = pNode->ToElement()->Attribute( "Height" );
	if( pHeight && strlen( pHeight ) > 0 )
	{
		m_nTextAreaPosX = atoi( pHeight );
	}

	const char* pScrollSpeed = pNode->ToElement()->Attribute( "ScrollSpeed" );
	if( pScrollSpeed && strlen( pScrollSpeed ) > 0 )
	{
		m_nScrollSpeed = atoi( pScrollSpeed );
	}

	TiXmlNode* pNodeStyleList = pNode->FirstChild( "StyleList" );
	if( !_LoadTextStyles( pNodeStyleList ) ) return FALSE;

	TiXmlNode* pNodeTextList = pNode->FirstChild( "TextList" );
	if( !_LoadTextList( pNodeTextList ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIStaffRoll::_LoadTextStyles( void* pStyle )
{
	TiXmlNode* pXmlNode = ( TiXmlNode* )pStyle;
	if( !pXmlNode ) return FALSE;

	m_mapTextStyle.Clear();

	TiXmlNode* pNodeStyle = pXmlNode->FirstChild( "Style" );
	while( pNodeStyle )
	{
		stUiStaffRoll_TextStyleEntry NewStyle;
		NewStyle.LoadFromXML( pNodeStyle );

		m_mapTextStyle.Add( NewStyle.m_strName, NewStyle );
		pNodeStyle = pNodeStyle->NextSibling();
	}

	return TRUE;
}

BOOL AgcmUIStaffRoll::_LoadTextList( void* pText )
{
	TiXmlNode* pXmlNode = ( TiXmlNode* )pText;
	if( !pXmlNode ) return FALSE;

	m_mapTextStyle.Clear();

	TiXmlNode* pNodeText = pXmlNode->FirstChild( "Text" );
	while( pNodeText )
	{
		stUiStaffRoll_TextEntry NewText;
		NewText.LoadFromXML( pNodeText );

		m_vecTexts.Add( NewText );
		pNodeText = pNodeText->NextSibling();
	}

	return TRUE;
}

BOOL AgcmUIStaffRoll::_UpdateImage( __int64 nTime )
{
	switch( m_eSwitchType )
	{
	case SwitchType_Fade :			return _UpdateFade( nTime );		break;
	case SwitchType_CrossFade :		return _UpdateCrossFade( nTime );	break;
	}

	return TRUE;
}

BOOL AgcmUIStaffRoll::_UpdateScroll( __int64 nTime )
{

	return TRUE;
}

BOOL AgcmUIStaffRoll::_UpdateFade( __int64 nTime )
{
	stStaffRollImageEntry* pEntryCurr = m_vecImages.Get( m_nCurrImage );
	if( !pEntryCurr ) return FALSE;

	// 지금거 업데이트 하고..
	pEntryCurr->Update( nTime, m_nFadeSpeed );

	// 재생이 끝났으면..
	if( pEntryCurr->m_eState == ImageState_UnKnown )
	{
		// 다음거 찾아보고..
		m_nCurrImage = _GetNextPlayImage();
		pEntryCurr = m_vecImages.Get( m_nCurrImage );
		if( pEntryCurr )
		{
			// 다음 것이 있으면 다음거 재생..
			pEntryCurr->Start( nTime );
		}
		else
		{
			// 더이상 재생할 게 없다면 나가자..
			OnLeave();
		}
	}

	return TRUE;
}

BOOL AgcmUIStaffRoll::_UpdateCrossFade( __int64 nTime )
{
	stStaffRollImageEntry* pEntryCurr = m_vecImages.Get( m_nCurrImage );
	if( !pEntryCurr ) return FALSE;

	// 지금거 업데이트 하고..
	pEntryCurr->Update( nTime, m_nFadeSpeed );

	// 지금 것이 FadeOut 상태에 들어갔으면..
	if( pEntryCurr->m_eState == ImageState_FadeOut )
	{
		// 다음거 플레이 시작
		m_nNextImage = _GetNextPlayImage();
		stStaffRollImageEntry* pEntryNext = m_vecImages.Get( m_nNextImage );
		if( pEntryNext )
		{
			// 다음 것이 있으면 다음거 재생..
			pEntryNext->Start( nTime );
		}
	}

	// 지금것의 재생이 종료되었으면..
	if( pEntryCurr->m_eState == ImageState_UnKnown )
	{
		// 플레이할 이미지의 인덱스 증가
		int nNextImage = _GetNextPlayImage();
		m_nCurrImage = m_nNextImage;
		m_nNextImage = nNextImage;

		// 지금 것이 종료되었는데 다음것이 없다면..
		stStaffRollImageEntry* pEntryNext = m_vecImages.Get( m_nNextImage );
		if( !pEntryNext )
		{
			// 더이상 재생할게 없으니 나간다.
			OnLeave();
		}
	}

	return TRUE;
}

int AgcmUIStaffRoll::_GetNextPlayImage( void )
{
	int nImageIndex = -1;

	switch( m_ePlayType )
	{
	case ImagePlayType_Once :
		{
			nImageIndex = m_nCurrImage + 1;
		}
		break;

	case ImagePlayType_Loop :
		{
			nImageIndex = m_nCurrImage + 1;
			if( nImageIndex >= m_vecImages.GetSize() )
			{
				nImageIndex = 0;
			}
		}
		break;

	case ImagePlayType_Suffle :
		{
			srand( timeGetTime() );

			int nImageCount = m_vecImages.GetSize();
			nImageIndex = rand() % nImageCount;
		}
		break;
	}

	return nImageIndex;
}
