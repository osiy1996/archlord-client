#include <AgcmUILogin/AgcmUILoginSetting.h>
#include <AuMD5EnCrypt/AuMD5EnCrypt.h>
#include "rwplcore.h"
#include <AuZpackLoader/AuZpackLoader.h>


#ifndef USE_MFC
#ifdef _AREA_KOREA_
	#include "AuHanAuth.h"
#endif
#ifdef _AREA_JAPAN_
	#include "AuJapaneseClientAuth.h"
#endif
#endif

#ifndef _AREA_CHINA_
#define HASH_KEY_STRING "1111"
#else
#define HASH_KEY_STRING "Protect_No_666_HevensDoor"
#endif


// string stream loader

void String_stream::Empty( void )
{
	if( pString )
	{
		if( isZpackStream_ && !packName_.empty() )
			ReleaseZpackStream( (wchar_t*)packName_.c_str(), pString );
		else
			delete[] pString;
		pString = NULL;
	}

	pCurrent = NULL;
	nBufferSize	= 0;
	isZpackStream_ = true;
	packName_ = L"";
}

bool String_stream::readfile( const char* pFilename, bool bDecryption, char* pReadType )
{
	Empty();

	std::string fileName = pFilename;

	size_t zpackNameIdx1 = fileName.find( "\\" );
	size_t zpackNameIdx2 = fileName.find( "/" );

	zpackNameIdx1 = zpackNameIdx1 < zpackNameIdx2 ? zpackNameIdx1 : zpackNameIdx2;

	std::string packName = fileName.substr( 0, zpackNameIdx1 );

	wchar_t wpackName[1024] = {0,};

	if( !packName.empty() )
	{
		MultiByteToWideChar( CP_ACP, 0, packName.c_str(), packName.length(), wpackName, 1024 );
		wcscat_s( wpackName, 1024, L".zp" );
		pString = (char*)GetZpackStream( wpackName, fileName.c_str(), nBufferSize );
		packName_ = wpackName;
	}

	if( !pString )
	{
		isZpackStream_ = false;

		size_t lReadBytes = 0;

		FILE* pFile	= NULL;
		fopen_s( &pFile, pFilename , pReadType );
		if( !pFile )
		{
			return false;
		}

		fseek( pFile, 0 , SEEK_END );
		nBufferSize = ftell( pFile );
		fseek( pFile, 0 , SEEK_SET );

		pString = new char[ nBufferSize ];
		memset( pString, 0, nBufferSize );
		lReadBytes = fread( pString, sizeof( char ), nBufferSize, pFile );

		fclose( pFile );
	}	

	if( bDecryption )
	{
		AuMD5Encrypt MD5;

#ifdef _AREA_CHINA_
		if( !MD5.DecryptString( MD5_HASH_KEY_STRING, pString, ( unsigned long )nBufferSize ) )
#else
		if( !MD5.DecryptString( HASH_KEY_STRING, pString, ( unsigned long )nBufferSize ) )
#endif
		{
			Empty();
			return false;
		}
	}

	pCurrent = pString;
	return true;
}

char* String_stream::sgetf( char* pBuffer )
{
	char* pNext = strstr( pCurrent, "\n" );

	if( !pNext )
	{
		strcpy( pBuffer, pCurrent );
		pCurrent = NULL;
	}
	else
	{
		int	nSize = pNext - pCurrent;
		strncpy( pBuffer, pCurrent , nSize );
		
		pBuffer[ nSize ] = 0;

		if( pBuffer[ nSize-1 ] == 13 )
			pBuffer[nSize - 1 ] = 0;

		pCurrent = pNext + 1 ;
	}

	return pCurrent;
}

bool String_stream::seof( void )
{
	if( ( size_t )( pCurrent - pString ) >= nBufferSize || pCurrent == NULL ) return true;
	else return false;
}	





static AgcmUILoginSetting* g_pagcmUILoginSetting = NULL;
AgcmUILoginSetting* GetUILoginSetting( void )
{
	if( !g_pagcmUILoginSetting )
	{
		g_pagcmUILoginSetting = new AgcmUILoginSetting;
	}

	return g_pagcmUILoginSetting;
}

void DestroyLoginSettingData( void )
{
	if( g_pagcmUILoginSetting )
	{
		delete g_pagcmUILoginSetting;
		g_pagcmUILoginSetting = NULL;
	}
}



BOOL AgcmUILoginSetting::LoadSettingFromFile( CHAR* pFileName, BOOL bDescript )
{
	_ClearData();
	_RegisterDataHeaders();	

	String_stream Stream;
	if( !Stream.readfile( pFileName, bDescript ? true : false, "rt" ) ) return FALSE;

	CHAR strBufferLine[ MAX_PATH ] = { 0, };
	CHAR strBufferWord[ MAX_PATH ] = { 0, };

	LoginSettingDataType eDataType = NoData;

	while( !Stream.seof() )
	{
		memset( strBufferLine, 0, sizeof( CHAR ) * MAX_PATH );
		if( !Stream.sgetf( strBufferLine ) )
		{
			Stream.Empty();
			return FALSE;
		}

		int nLength = strlen( strBufferLine );
		if( strBufferLine[ nLength - 1 ] == '\n' )
		{
			strBufferLine[ nLength - 1 ] = '\0';
		}

		// if line is comment, pass this line
		if( strBufferLine[ 0 ] == '/' && strBufferLine[ 1 ] == '/' ) continue;

		// if line has no characters, pass this line
		if( strBufferLine[ 0 ] == '\0' ) continue;

		// if line is data's header
		if( strBufferLine[ 0 ] == '[' )
		{
			eDataType = _ParseHeaderDataType( strBufferLine );

			switch( eDataType )
			{
			case LoginDivision :				_ReadLoginDivisionNumber( &Stream );		break;
			case CharacterSelect_Position :		_AddCharacterSelectPosition( &Stream );		break;
			case CharacterSelect_MoveOffSet :	_AddCharacterSelectMoveOffset( &Stream );	break;
			case CharacterCreate_Position :		_AddCharacterCreatePosition( &Stream );		break;
			case CameraTM_Login :				_AddCameraLoginTM( &Stream );				break;
			case CameraTM_CharacterSelect :		_AddCameraCharacterSelectTM( &Stream );		break;
			case CameraTM_CharacterCreate :		_AddCameraCharacterCreateTM( &Stream );		break;
			case CameraTM_CharacterCreateZoom :	_AddCameraCharacterZoomTM( &Stream );		break;
			}
		}
	}

	Stream.Empty();
	return TRUE;
}

void AgcmUILoginSetting::_ClearData( void )
{
	headers_.clear();
	race_types_.clear();
	class_types_.clear();

	m_nLoginDivision = 0;

	ClearCameraData();
	ClearCharacterData();
}

void AgcmUILoginSetting::_RegisterDataHeaders( void )
{

	headers_[ "[ NoData ]" ]						=		NoData;
	headers_[ "[ LoginDivision ]" ]					=		LoginDivision;
	headers_[ "[ CharacterSelect_Position ]" ]		=		CharacterSelect_Position;
	headers_[ "[ CharacterSelect_MoveOffSet ]" ]	=		CharacterSelect_MoveOffSet;
	headers_[ "[ CharacterCreate_Position ]" ]		=		CharacterCreate_Position;
	headers_[ "[ CameraTM_Login ]" ]				=		CameraTM_Login;
	headers_[ "[ CameraTM_CharacterSelect ]" ]		=		CameraTM_CharacterSelect;
	headers_[ "[ CameraTM_CharacterCreate ]" ]		=		CameraTM_CharacterCreate;
	headers_[ "[ CameraTM_CharacterCreateZoom ]" ]	=		CameraTM_CharacterCreateZoom;

	race_types_[ "Human" ]							=		AURACE_TYPE_HUMAN;
	race_types_[ "Orc" ]							=		AURACE_TYPE_ORC;
	race_types_[ "MoonElf" ]						=		AURACE_TYPE_MOONELF;
	race_types_[ "DragonScion" ]					=		AURACE_TYPE_DRAGONSCION;
			   
	class_types_[ "Knight" ]						=		AUCHARCLASS_TYPE_KNIGHT;
	class_types_[ "Ranger" ]						=		AUCHARCLASS_TYPE_RANGER;
	class_types_[ "Scion" ]							=		AUCHARCLASS_TYPE_SCION;
	class_types_[ "Mage" ]							=		AUCHARCLASS_TYPE_MAGE;
}

LoginSettingDataType AgcmUILoginSetting::_ParseHeaderDataType( CHAR* pString )
{
	if( !pString ) return NoData;

	//Headers::iterator iter = headers_.find( pString );

	//return iter == headers_.end() ? NoData : iter->second;

	for( Headers::iterator iter = headers_.begin(); iter != headers_.end(); ++iter )
	{
		char const * header = iter->first.c_str();

		if( strcmp( header, pString ) == 0 )
			return iter->second;
	}
	return NoData;
}

AuRaceType AgcmUILoginSetting::_ParseRaceName( CHAR* pString )
{
	if( !pString ) return AURACE_TYPE_NONE;

	RaceTypes::iterator iter = race_types_.find( pString );

	return iter == race_types_.end() ? AURACE_TYPE_NONE : iter->second;
}

AuCharClassType AgcmUILoginSetting::_ParseClassName( CHAR* pString )
{
	if( !pString ) return AUCHARCLASS_TYPE_NONE;

	CharClassTypes::iterator iter = class_types_.find( pString );

	return iter == class_types_.end() ? AUCHARCLASS_TYPE_NONE : iter->second;
}

void AgcmUILoginSetting::_ReadLoginDivisionNumber( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer[ MAX_PATH ] = { 0, };
	if( !pData->sgetf( strBuffer ) ) return;

	sscanf( strBuffer, "DivisionNumber=%d", &m_nLoginDivision );
}

void AgcmUILoginSetting::_AddCharacterSelectPosition( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	// read 3 lines for 3 characters
	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;

	RwV3d vPos1 = { 0.0f, 0.0f, 0.0f };
	float fRotate1 = 0.0f;
	sscanf( strBuffer1, "Pos : X=%f, Y=%f, Z=%f, Rotate : %f", &vPos1.x, &vPos1.y, &vPos1.z, &fRotate1 );

	RwV3d vPos2 = { 0.0f, 0.0f, 0.0f };
	float fRotate2 = 0.0f;
	sscanf( strBuffer2, "Pos : X=%f, Y=%f, Z=%f, Rotate : %f", &vPos2.x, &vPos2.y, &vPos2.z, &fRotate2 );

	RwV3d vPos3 = { 0.0f, 0.0f, 0.0f };
	float fRotate3 = 0.0f;
	sscanf( strBuffer3, "Pos : X=%f, Y=%f, Z=%f, Rotate : %f", &vPos3.x, &vPos3.y, &vPos3.z, &fRotate3 );

	AddSelectPosition( 0, vPos1, fRotate1 );
	AddSelectPosition( 1, vPos2, fRotate2 );
	AddSelectPosition( 2, vPos3, fRotate3 );
}

void AgcmUILoginSetting::_AddCharacterSelectMoveOffset( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer[ MAX_PATH ] = { 0, };
	if( !pData->sgetf( strBuffer ) ) return;

	while( strBuffer[ 0 ] == 'R' )
	{
		CHAR strRaceName[ 16 ] = { 0, };
		CHAR strClassName[ 16 ] = { 0, };
		float fOffSet = 0.0f;

		sscanf( strBuffer, "Race=%s Class=%s OffSet=%f", strRaceName, strClassName, &fOffSet );

		AuRaceType eRaceType = _ParseRaceName( strRaceName );
		AuCharClassType eClassType = _ParseClassName( strClassName );

		AddMoveOffset( eRaceType, eClassType, fOffSet );
		if( !pData->sgetf( strBuffer ) ) return;
	}
}

void AgcmUILoginSetting::_AddCharacterCreatePosition( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer[ MAX_PATH ] = { 0, };
	if( !pData->sgetf( strBuffer ) ) return;

	while( strBuffer[ 0 ] == 'R' )
	{
		CHAR strRaceName[ 16 ] = { 0, };
		RwV3d vPos = { 0.0f, 0.0f, 0.0f };
		RwReal rRotate = 0.0f;

		sscanf( strBuffer, "Race=%s X=%f Y=%f Z=%f Rotate=%f", strRaceName, &vPos.x, &vPos.y, &vPos.z, &rRotate );

		AuRaceType eRaceType = _ParseRaceName( strRaceName );
		AddCreatePosition( eRaceType, vPos, rRotate );
		if( !pData->sgetf( strBuffer ) ) return;
	}
}

void AgcmUILoginSetting::_AddCameraLoginTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer1, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer2, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer3, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	SetLoginTM( ViewSet.m_matTM, fPerspective );
}

void AgcmUILoginSetting::_AddCameraCharacterSelectTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer1, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer2, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer3, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	SetSelectTM( ViewSet.m_matTM, fPerspective );
}

void AgcmUILoginSetting::_AddCameraCharacterCreateTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };
	CHAR strBuffer4[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;
	if( !pData->sgetf( strBuffer4 ) ) return;

	CHAR strRaceName[ 32 ] = { 0, };
	sscanf( strBuffer1, "Race=%s", strRaceName );
	AuRaceType eRace = _ParseRaceName( strRaceName );

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer2, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer3, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer4, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	AddCreateTM( eRace, ViewSet.m_matTM, fPerspective );
}

void AgcmUILoginSetting::_AddCameraCharacterZoomTM( void* pStream )
{
	if( !pStream ) return;
	String_stream* pData = ( String_stream* )pStream;

	CHAR strBuffer1[ MAX_PATH ] = { 0, };
	CHAR strBuffer2[ MAX_PATH ] = { 0, };
	CHAR strBuffer3[ MAX_PATH ] = { 0, };
	CHAR strBuffer4[ MAX_PATH ] = { 0, };
	CHAR strBuffer5[ MAX_PATH ] = { 0, };

	if( !pData->sgetf( strBuffer1 ) ) return;
	if( !pData->sgetf( strBuffer2 ) ) return;
	if( !pData->sgetf( strBuffer3 ) ) return;
	if( !pData->sgetf( strBuffer4 ) ) return;
	if( !pData->sgetf( strBuffer5 ) ) return;

	CHAR strRaceName[ 32 ] = { 0, };
	sscanf( strBuffer1, "Race=%s", strRaceName );
	AuRaceType eRace = _ParseRaceName( strRaceName );

	CHAR strClassName[ 32 ] = { 0, };
	sscanf( strBuffer2, "Class=%s", strClassName );
	AuCharClassType eClass = _ParseClassName( strClassName );

	RwV3d vPos = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer3, "Pos : X=%f, Y=%f, Z=%f", &vPos.x, &vPos.y, &vPos.z );

	RwV3d vLookAt = { 0.0f, 0.0f, 0.0f };
	sscanf( strBuffer4, "LookAt : X=%f, Y=%f, Z=%f", &vLookAt.x, &vLookAt.y, &vLookAt.z );

	float fPerspective = 1.0f;
	sscanf( strBuffer5, "Perspective=%f", &fPerspective );

	stViewSet ViewSet;

	ViewSet.Initialize();
	ViewSet.MoveTo( vPos );
	ViewSet.LookAt( vLookAt );

	AddZoomTM( eRace, eClass, ViewSet.m_matTM, fPerspective );
}
