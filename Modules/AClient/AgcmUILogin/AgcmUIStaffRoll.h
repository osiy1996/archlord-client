#ifndef __CLASS_AGCM_UI_STAFF_ROLL_H__
#define __CLASS_AGCM_UI_STAFF_ROLL_H__



#include <ApBase/ApBase.h>
#include <AgcmUILogin/ContainerUtil.h>
#include <AuXmlParser/TinyXml/tinyxml.h>


#define STRING_LENGTH_NAME											64
#define STRING_LENGTH_FILENAME										256
#define STRING_LENGTH_LINETEXT										256


enum eUiStaffRoll_ImageState
{
	ImageState_UnKnown = 0,
	ImageState_FadeIn,
	ImageState_Idle,
	ImageState_FadeOut,
};

enum eUiStaffRoll_ImagePlayType
{
	ImagePlayType_UnKnown = 0,
	ImagePlayType_Once,
	ImagePlayType_Loop,
	ImagePlayType_Suffle,
};

struct stStaffRollImageEntry
{
	void*															m_pTexture;
	char															m_strFileName[ STRING_LENGTH_FILENAME ];

	int																m_nWidth;
	int																m_nHeight;

	int																m_nPosX;
	int																m_nPosY;

	eUiStaffRoll_ImageState											m_eState;

	__int64															m_nTimeStart;
	__int64															m_nTimeFadeInEnd;
	__int64															m_nDuration;

	int																m_nAlpha;

	stStaffRollImageEntry( void )
	{
		Clear();
	}

	void						Clear								( void );
	BOOL						LoadFromXML							( void* pNode );
	BOOL						Start								( __int64 nTime );
	BOOL						Update								( __int64 nTime, __int64 nFadeSpeed );
};

enum eUiStaffRoll_ImageSwitchType
{
	SwitchType_UnKnown = 0,
	SwitchType_Fade,
	SwitchType_CrossFade,
};

enum eUiStaffRoll_TextHAlignType
{
	HAlign_None = 0,
	HAlign_Left,
	HAlign_Center,
	HAlign_Right,
};

struct stUiStaffRoll_TextStyleEntry
{
	char															m_strName[ STRING_LENGTH_NAME ];
	eUiStaffRoll_TextHAlignType										m_eHAlign;

	int																m_nFontType;
	int																m_nFontSize;
	DWORD															m_dwFontColor;

	stUiStaffRoll_TextStyleEntry( void )
	{
		Clear();
	}

	void						Clear								( void );
	BOOL						LoadFromXML							( void* pNode );
};

struct stUiStaffRoll_TextEntry
{
	char															m_strStyleName[ STRING_LENGTH_NAME ];
	char															m_strString[ STRING_LENGTH_LINETEXT ];

	stUiStaffRoll_TextEntry( void )
	{
		Clear();
	}

	void						Clear								( void );
	BOOL						LoadFromXML							( void* pNode );
};

class AgcmUIStaffRoll
{
private :
	stStaffRollImageEntry											m_stmageBG;
	stStaffRollImageEntry											m_stmageAlphaTop;
	stStaffRollImageEntry											m_stImageAlphaBottom;

	eUiStaffRoll_ImageSwitchType									m_eSwitchType;
	ContainerVector< stStaffRollImageEntry >						m_vecImages;

	ContainerMap< std::string, stUiStaffRoll_TextStyleEntry >		m_mapTextStyle;
	ContainerVector< stUiStaffRoll_TextEntry >						m_vecTexts;

	int																m_nTextAreaPosX;
	int																m_nTextAreaPosY;
	int																m_nTextAreaWidth;
	int																m_nTextAreaHeight;
	int																m_nScrollSpeed;

	char															m_strSettingFileName[ STRING_LENGTH_FILENAME ];
	BOOL															m_bIsEncryption;
	TiXmlDocument													m_Doc;

	int																m_nScreenWidth;
	int																m_nScreenHeight;

	__int64															m_nFadeSpeed;

	eUiStaffRoll_ImagePlayType										m_ePlayType;
	int																m_nCurrImage;
	int																m_nNextImage;

public :
	AgcmUIStaffRoll( void );
	~AgcmUIStaffRoll( void );

public :
	BOOL						OnLoad								( char* pFileName, BOOL bIsEncryption );
	BOOL						OnReload							( void );
	BOOL						OnEnter								( __int64 nTime );
	BOOL						OnUpdate							( __int64 nTime );
	BOOL						OnRender							( void );
	BOOL						OnLeave								( void );

private :
	BOOL						_ParseXmlDocument					( void );

	BOOL						_LoadStaffRollLayout				( void* pLayout );
	BOOL						_LoadStaffRollImageList				( void* pImageList );
	BOOL						_LoadStaffRollText					( void* pText );
	BOOL						_LoadTextStyles						( void* pStyle );
	BOOL						_LoadTextList						( void* pText );

	BOOL						_UpdateImage						( __int64 nTime );
	BOOL						_UpdateScroll						( __int64 nTime );
	BOOL						_UpdateFade							( __int64 nTime );
	BOOL						_UpdateCrossFade					( __int64 nTime );
	int							_GetNextPlayImage					( void );
};



#endif