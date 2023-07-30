#ifndef __AGCM_UI_LOGIN_SETTING_H__
#define __AGCM_UI_LOGIN_SETTING_H__


#include "AgcmAdmin.h"
#include "AgcmUILoginSettingCamera.h"
#include "AgcmUILoginSettingCharacter.h"


struct String_stream
{
	char* pString;
	char* pCurrent;
	size_t nBufferSize;
	bool isZpackStream_;
	std::wstring packName_;

	//String_stream( char* pBufferInput, size_t size )
	//	: pString( pBufferInput ), pCurrent( pBufferInput ), nBufferSize( size ), isZpackStream_(true)
	//{
	//}

	String_stream( void )
		: pString( NULL ), pCurrent( NULL ), nBufferSize( 0 ), isZpackStream_(true)
	{
	}

	~String_stream( void )
	{
		Empty();
	}

	void Empty( void );
	bool readfile( const char* pFilename, bool bDecryption, char* pReadType = "rt" );
	char* sgetf( char* pBuffer );
	bool seof( void );
};



class AgcmUILoginSetting : public AgcmUILoginSettingCamera, public AgcmUILoginSettingCharacter
{
private :
	//ContainerMap< std::string, LoginSettingDataType >		m_mapHeader;
	//ContainerMap< std::string, AuRaceType >					m_mapRaceType;
	//ContainerMap< std::string, AuCharClassType >			m_mapClassType;
	typedef std::map< std::string, LoginSettingDataType >	Headers;
	typedef std::map< std::string, AuRaceType >				RaceTypes;
	typedef std::map< std::string, AuCharClassType >		CharClassTypes;
	Headers													headers_;
	RaceTypes												race_types_;
	CharClassTypes											class_types_;


	UINT32													m_nLoginDivision;

public :
	AgcmUILoginSetting( void )								{	}
	~AgcmUILoginSetting( void )								{	}

public :
	BOOL					LoadSettingFromFile				( CHAR* pFileName, BOOL bDescript = FALSE );
	UINT32					GetLoginDivision				( void ) { return m_nLoginDivision; }

	void					TestFunction					( void );

private :
	void					_ClearData						( void );
	void					_RegisterDataHeaders			( void );	

	LoginSettingDataType	_ParseHeaderDataType			( CHAR* pString );
	AuRaceType				_ParseRaceName					( CHAR* pString );
	AuCharClassType			_ParseClassName					( CHAR* pString );

	void					_ReadLoginDivisionNumber		( void* pStream );

	void					_AddCharacterSelectPosition		( void* pStream );
	void					_AddCharacterSelectMoveOffset	( void* pStream );
	void					_AddCharacterCreatePosition		( void* pStream );

	void					_AddCameraLoginTM				( void* pStream );
	void					_AddCameraCharacterSelectTM		( void* pStream );
	void					_AddCameraCharacterCreateTM		( void* pStream );
	void					_AddCameraCharacterZoomTM		( void* pStream );
};


AgcmUILoginSetting* GetUILoginSetting( void );
void DestroyLoginSettingData( void );


#endif