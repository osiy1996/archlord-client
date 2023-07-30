#ifndef			_AGCMUIOPTION_H_
#define			_AGCMUIOPTION_H_

#include <AgcModule/AgcModule.h>
#include <AgcmUIControl/AcUIToolTip.h>
#include <AgcmUIOption/AgcmUIOptionDefine.h>
#include <AgcmUIManager2/AgcdUIManager2.h>
#include <d3d9.h>

enum eSoundSpeakerMode;
enum AgcdItemDropCategoryId;

#define	AGCMOPTION_UI_SKIN_TYPE1_PATH		_T("_TYPE1")
#define AGCMOPTION_MAX_SPEAKER_TYPE			6

class AgcmUIOption;

class CMyCheckBox
{
public:
	CMyCheckBox();
	~CMyCheckBox();

	BOOL	Refresh();
	void	Update();

	BOOL			m_bState;
	AgcdUIUserData*	m_pUserData;
};

enum AgcdUIOptionListItemIndex {
	AGCDUIOPTION_LII_CATEGORY_LENS_EFFECT,
	AGCDUIOPTION_LII_LENS_ON_NORMAL_DAMAGE,
	AGCDUIOPTION_LII_LENS_ON_SKILL_DAMAGE,
	AGCDUIOPTION_LII_LENS_ON_SKILL_EFFECT,
	AGCDUIOPTION_LII_CATEGORY_MOVEMENT,
	AGCDUIOPTION_LII_MOVEMENT_RMB_DBLCLICK,
	AGCDUIOPTION_LII_COUNT
};

enum AgcdUIOptionListItemType {
	AGCDUIOPTION_ADVANCED_OPTION_TYPE_CATEGORY,
	AGCDUIOPTION_ADVANCED_OPTION_TYPE_BOOLEAN,
};

struct AgcdUIOptionListItem {
	typedef void(*CBOptionChanged)(
		AgcmUIOption *			pcsAgcmUIOption,
		AgcdUIOptionListItem *	pstListItem,
		PVOID					pData);

	char						szLabel[128];
	AgcdUIOptionListItemIndex	eIndex;
	AgcdUIOptionListItemType	eType;
	union {
		struct {
			char				szUDName[128];
			char				szINILabel[128];
			BOOL				bValue;
			AgcdUIUserData *	pstUD;
			CBOptionChanged		pfnCallback;
			PVOID				pCBData;
		} boolean;
	} data;

	void SetCategory(const char * szLabel_)
	{
		eType = AGCDUIOPTION_ADVANCED_OPTION_TYPE_CATEGORY;
		strcpy_s(szLabel, szLabel_);
	}

	void SetBoolean(
		const char *	szLabel_,
		const char *	szUDName,
		const char *	szINILabel,
		BOOL			bValue,
		CBOptionChanged	pfnCallback,
		PVOID			pCBData)
	{
		eType = AGCDUIOPTION_ADVANCED_OPTION_TYPE_BOOLEAN;
		strcpy_s(szLabel, szLabel_);
		strcpy_s(data.boolean.szUDName, szUDName);
		strcpy_s(data.boolean.szINILabel, szINILabel);
		data.boolean.bValue = bValue;
		data.boolean.pstUD = NULL;
		data.boolean.pfnCallback = pfnCallback;
		data.boolean.pCBData = pCBData;
	}
};

class AgcmUIOption : public AgcModule
{
public:
	class AgcmUIManager2 *		m_pcsAgcmUIManager2;
	class AgcmTuner*			m_pcsAgcmTuner;
	class AgcmShadow*			m_pcsAgcmShadow;
	class AgcmShadow2*			m_pcsAgcmShadow2;
	class AgcmSound*			m_pcsAgcmSound;
	class AgcmEff2*				m_pcsAgcmEff2;
	class AgcmTextBoardMng*		m_pcsAgcmTextBoardMng;
	class AgcmRender*			m_pcsAgcmRender;
	class AgcmLogin*			m_pcsAgcmLogin;
	class AgcmGrass*			m_pcsAgcmGrass;
	class AgcmLensFlare*		m_pcsAgcmLensFlare;
	class AgcmEventNature*		m_pcsAgcmEventNature;
	class AgcmWater*			m_pcsAgcmWater;
	class AgcmShadowmap*		m_pcsAgcmShadowMap;
	class AgcmMap*				m_pcsAgcmMap;
	class AgcmCharacter*		m_pcsAgcmCharacter;
	class AgpmCharacter*		m_pcsAgpmCharacter;
	class AgpmGuild*			m_pcsAgpmGuild;
	class AgcmGuild*			m_pcsAgcmGuild;
	class AgcmUIParty*			m_pcsAgcmUIParty;
	class AgcmUIStatus*			m_pcsAgcmUIStatus;
	class AgcmUITips*			m_pcsAgcmUITips;
	class AgcmMinimap*			m_pcsAgcmMinimap;
	class AgcmPostFX*			m_pcsAgcmPostFX;
	class AgcmEventEffect *		m_pcsAgcmEventEffect;
	class AgcmItem *			m_pcsAgcmItem;

	AcUIToolTip*		m_pcsAcUIToolTip;
	CHAR *				m_aszNameView[AGCD_OPTION_MAX_VIEW];
	CHAR *				m_aszNameToggle[AGCD_OPTION_MAX_TOGGLE];
	CHAR *				m_aszNameNature[AGCD_OPTION_MAX_NATURE];
	CHAR *				m_aszNameEffect[AGCD_OPTION_MAX_EFFECT];
	CHAR *				m_aszNameEffect2[AGCD_OPTION_MAX_EFFECT2];

	// 화면 설정
	INT32				m_lEventIDAutoTuningEnable;				// 비활성화
	INT32				m_lEventIDAutoTuningDisable;			// 비활성화

	BOOL				m_bAutoTuning;							// 자동 최적화(비활성화)
	AgcdUIUserData*		m_pstUDAutoTuning;						//

	BOOL				m_bSpeedTuning;							// 게임 속도 최적화
	AgcdUIUserData*		m_pstUDSpeedTuning;

	// Video 관련 HW에 따른 Option
	BOOL				m_bWaterHigh;
	BOOL				m_bBloom1, m_bBloom2;
	BOOL				m_bAutoExposure;

	// Video (Main) Setting
	AgcdUIOptionView	m_eCharView;
	AgcdUIUserData*		m_pstUDCharView;

	AgcdUIOptionView	m_eMapView;
	AgcdUIUserData*		m_pstUDMapView;

	AgcdUIOptionToggle	m_eWideView;
	AgcdUIUserData*		m_pstUDWideView;

	AgcdUIOptionNature	m_eNature;
	AgcdUIUserData*		m_pstUDNature;

	AgcdUIOptionEffect	m_eShadow;
	AgcdUIUserData*		m_pstUDShadow;

	AgcdUIOptionEffect	m_eEffect;
	AgcdUIUserData*		m_pstUDEffect;

	AgcdUIOptionEffect	m_eWater;
	AgcdUIUserData*		m_pstUDWater;

	AgcdUIOptionEffect2	m_eBloom;
	AgcdUIUserData*		m_pstUDBloom;

	AgcdUIOptionToggle	m_eAutoExposure;
	AgcdUIUserData*		m_pstUDAutoExposure;

	AgcdUIOptionToggle	m_eMatEffect;
	AgcdUIUserData*		m_pstUDMatEffect;

	AgcdUIOptionToggle	m_eAntiAliasing;
	AgcdUIUserData*		m_pstUDAntiAliasing;

	AgcdUIOptionToggle	m_eImpact;
	AgcdUIUserData*		m_pstUDImpact;

// #ifdef _AREA_GLOBAL_
// 	UINT32				m_iMouseSpeedOriginal;
// 	UINT32				m_iMouseSpeedMax, m_iMouseSpeedCurrent;
// 	AgcdUIUserData		*m_pstUDMouseSpeedMax, *m_pstUDMouseSpeedCurrent;
// #endif

	AgcdUIOptionToggle	m_eCriminal;
	AgcdUIOptionToggle	m_eMurderer;
	AgcdUIOptionToggle	m_eWanted;
	UINT32				m_iScreenBrightMax, m_iScreenBrightCurrent;
	AgcdUIUserData		*m_pstUDScreenBrightMax, *m_pstUDScreenBrightCurrent;

	//. 2005. 9. 20 Nonstopdj
	//. contrast, brightness 값 추가.
	//. 저장되는 곳은 ./ini/COption.ini
	UINT32				m_iScreenContrastMax, m_iScreenContrastCurrent;
	UINT32				m_iScreenBrightnessMax, m_iScreenBrightnessCurrent;

	UINT32				m_iBloomMax, m_iBloomCurrent;
	AgcdUIUserData		*m_pstUDBloomMax, *m_pstUDBloomCurrent;

	// Video (Nature) Setting
	INT32				m_lEventUpdateWater;
	INT32				m_lEventUpdateBloom;
	INT32				m_lEventUpdateAutoExposure;

	AgcdUIOptionNature	m_eNaturalObj;
	AgcdUIUserData*		m_pstUDNaturalObj;

	AgcdUIOptionToggle	m_eLensFlare;
	AgcdUIUserData*		m_pstUDLensFlare;

	AgcdUIOptionToggle	m_eFog;
	AgcdUIUserData*		m_pstUDFog;

	AgcdUIOptionToggle	m_eSky;
	AgcdUIUserData*		m_pstUDSky;
	
	// Video (Shadow) Setting
	AgcdUIOptionEffect	m_eShadowChar;
	AgcdUIUserData*		m_pstUDShadowChar;

	AgcdUIOptionToggle	m_eShadowSelf;
	AgcdUIUserData*		m_pstUDShadowSelf;

	AgcdUIOptionToggle	m_eShadowObject;
	AgcdUIUserData*		m_pstUDShadowObject;

	// Video (Effect) Setting
	AgcdUIOptionEffect	m_eEffectField;
	AgcdUIUserData*		m_pstUDEffectField;

	AgcdUIOptionEffect	m_eEffectCombat;
	AgcdUIUserData*		m_pstUDEffectCombat;

	AgcdUIOptionToggle	m_eEffectUI;
	AgcdUIUserData*		m_pstUDEffectUI;

	AgcdUIOptionToggle	m_eEffectSkill;
	AgcdUIUserData*		m_pstUDEffectSkill;

	// 사운드 설정
	INT32				m_lEventIDBGMEnable, m_lEventIDBGMDisable, m_lEventIDESEnable, m_lEventIDESDisable;

	BOOL				m_bBackgroundSound;
	AgcdUIUserData*		m_pstUDBackgroundSound;
	UINT32				m_iBackgroundSoundMax, m_iBackgroundSoundCurrent;
	AgcdUIUserData		*m_pstUDBackgroundSoundMax, *m_pstUDBackgroundSoundCurrent;
	
	BOOL				m_bEffectSound;
	AgcdUIUserData*		m_pstUDEffectSound;
	UINT32				m_iEffectSoundMax, m_iEffectSoundCurrent;
	AgcdUIUserData		*m_pstUDEffectSoundMax, *m_pstUDEffectSoundCurrent;
	
	BOOL				m_bEnvSound;
	AgcdUIUserData*		m_pstUDEnvSound;
	UINT32				m_iEnvSoundMax, m_iEnvSoundCurrent;
	AgcdUIUserData		*m_pstUDEnvSoundMax, *m_pstUDEnvSoundCurrent;

	eSoundSpeakerMode	m_eSpeakerType;
	CHAR *				m_aszSpeakerType[AGCMOPTION_MAX_SPEAKER_TYPE];
	AgcdUIUserData*		m_pstUDSpeakerType;

	INT32				m_l3DProvider;
	AgcdUIUserData*		m_pstUD3DProvider;

	BOOL				m_bUseEAX;
	AgcdUIUserData*		m_pstUDUseEAX;

	BOOL				m_bViewNameMine, m_bViewNameMonster, m_bViewNameGuild, m_bViewNameParty, m_bViewNameOthers;
	BOOL				m_bRefuseTrade, m_bRefusePartyIn, m_bRefuseGuildIn, m_bRefuseGuildBattle, m_bRefuseBattle;

	BOOL				m_bRefuseGuildRelation;
	BOOL				m_bRefuseBuddy;

	BOOL				m_bVisiblePartyBuffIcon;
	BOOL				m_bVisibleViewHelmet;
	BOOL				m_bVisibleViewItem;

	BOOL				m_bIsAutoPickItem;

	BOOL				m_bMurderer;
	BOOL				m_bWanted;
	BOOL				m_bCriminal;

	AgcdUIUserData		*m_pstUDViewNameMine, *m_pstUDViewNameMonster, *m_pstUDViewNameGuild, *m_pstUDViewNameParty, *m_pstUDViewNameOthers;
	BOOL				m_bViewHelpBaloonTip, m_bViewHelpBaloonChat, m_bViewHelpUclie;
	AgcdUIUserData		*m_pstUDViewHelpBaloonTip, *m_pstUDViewHelpBaloonChat, *m_pstUDViewHelpUclie;
	BOOL				m_bViewSelfBars;
	AgcdUIUserData		*m_pstUDViewSelfBars;
	AgcdUIUserData		*m_pstUDRefuseTrade, *m_pstUDRefusePartyIn, *m_pstUDRefuseGuildIn, *m_pstUDRefuseGuildBattle, *m_pstUDRefuseBattle;

	BOOL				m_bDisableSkillEffect;
	AgcdUIUserData*		m_pstUDDisableSkillEffect;

	AgcdUIUserData*		m_pstUDRefuseGuildRelation;
	AgcdUIUserData*		m_pstUDRefuseBuddy;
	AgcdUIUserData		*m_pstUDVisiblePartyBuffIcon;
	AgcdUIUserData		*m_pstUDVisibleViewHelmet;
	AgcdUIUserData		*m_pstUDVisibleViewItem;

	AgcdUIUserData*		m_pstUDVisibleCriminal;
	AgcdUIUserData*		m_pstUDVisibleWanted;
	AgcdUIUserData*		m_pstUDVisibleMurderer;
		
	// Patch Backup용
	UINT32				m_iScreenWidth;
	UINT32				m_iScreenHeight;
	UINT32				m_iScreenDepth;

	AgcdUIOptionTexture	m_eTQCharacter;
	AgcdUIOptionTexture	m_eTQObject;
	AgcdUIOptionTexture	m_eTQEffect;
	AgcdUIOptionTexture	m_eTQWorld;

	CHAR				m_strSound[64];
	CHAR				m_strSpeaker[64];

	BOOL				m_bEnableGamma;

	BOOL				m_bDisableOtherPlayerSkillEffect;
	AgcdUIUserData*		m_pstUDDisableOtherPlayerSkillEffect;

	AgcdUIOptionListItem m_stAdvOptions[AGCDUIOPTION_LII_COUNT];
	UINT32				m_ulAdvOptIndices[AGCDUIOPTION_LII_COUNT];
	AgcdUIUserData *	m_pstAdvOptUD;
	BOOL				m_bAdvOptionsSetup;

	BOOL				m_bSave;			// 내부적으로 사용
	BOOL				m_bSaveLoad;		// 외부에서 세팅
	BOOL				m_bAutoPatchInitialized;

	BOOL				m_bWindowed;
	BOOL				m_bFramed;

	UINT32				m_ulUISkinType;

	AgcmOptionStartup	m_eStartupMode;

	INT32	m_nNearRough;
	INT32	m_nNearDetail;
	INT32	m_nNormalRough;
	INT32	m_nNormalDetail;
	INT32	m_nFarRough;
	INT32	m_nFarDetail;
	INT32	m_nAllRough;
	INT32	m_nAllDetail;

	BOOL					m_bEnableItemDropSettings;
	AgcdUIUserData *		m_pstItemDropEnableUD;
	AgcdUIUserData *		m_pstItemDropCategoryUD;
	AgcdUIUserData *		m_pastItemDropSliderUD[3];
	AgcdUIUserData *		m_pstItemDropPreviewUD;
	UINT32					m_ulItemDropSliderValue[3];
	UINT32					m_ulItemDropSliderMax;
	AgcdItemDropCategoryId	m_eItemDropCategory;

public:
	AgcmUIOption();
	~AgcmUIOption();

public:
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	AgcdUIUserData*	AddOptionUserData(AgcdUIOptionType eType, CHAR *szName, AgcUICallBack fnCallback);

	BOOL	LoadFromINI( char* szFile);
	BOOL	SaveToINI( char* szFileName);

	void	InitGameOption();
	void	UpdateGameOption();

	void	UpdateAutoTuning(BOOL	bRefresh);
	void	UpdateSpeedTuning(BOOL	bRefresh);
	void	UpdateDisableOtherPlayerSkillEffect( BOOL bRefresh );
	void	UpdateCharView(BOOL	bRefresh);
	void	UpdateMapView(BOOL	bRefresh);
	void	UpdateWideView(BOOL	bRefresh);
	void	UpdateNature(BOOL	bRefresh);
	void	UpdateShadow(BOOL	bRefresh);
	void	UpdateEffect(BOOL	bRefresh);
	void	UpdateWater(BOOL	bRefresh);
	void	UpdateBloom(BOOL	bRefresh);
	void	UpdateAutoExposure(BOOL	bRefresh);
	void	UpdateMatEffect(BOOL	bRefresh);
	void	UpdateAntiAliasing(BOOL	bRefresh);
	void	UpdateImpact(BOOL	bRefresh);
	void	UpdateBloomVal(BOOL	bRefresh);
// #ifdef _AREA_GLOBAL_
// 	void	UpdateMouseSpeedVal(BOOL bRefresh);
// #endif
	void	UpdateBrightVal(BOOL	bRefresh);

	void	UpdateNaturalObj(BOOL	bRefresh);
	void	UpdateLensFlare(BOOL	bRefresh);
	void	UpdateFog(BOOL	bRefresh);
	void	UpdateSky(BOOL	bRefresh);

	void	UpdateShadowChar(BOOL	bRefresh);
	void	UpdateShadowSelf(BOOL	bRefresh);
	void	UpdateShadowObject(BOOL	bRefresh);

	void	UpdateEffectField	(BOOL	bRefresh);
	void	UpdateEffectCombat	(BOOL	bRefresh);
	void	UpdateEffectUI		(BOOL	bRefresh);
	void	UpdateEffectSkill	(BOOL	bRefresh);
	
	void	UpdateBGMSoundEnable(BOOL	bRefresh);
	void	UpdateBGMSoundVal(BOOL	bRefresh);
	void	UpdateEffSoundEnable(BOOL	bRefresh);
	void	UpdateEffSoundVal(BOOL	bRefresh);
	void	UpdateEnvSoundEnable(BOOL	bRefresh);
	void	UpdateEnvSoundVal(BOOL	bRefresh);
	void	UpdateUseEAX(BOOL	bRefresh);
	void	UpdateSpeakerType(BOOL	bRefresh);
	void	Update3DProvider(BOOL	bRefresh);
	
	void	UpdateViewNameMine(BOOL	bRefresh);
	void	UpdateViewNameMonster(BOOL	bRefresh);
	void	UpdateViewNameGuild(BOOL	bRefresh);
	void	UpdateViewNameParty(BOOL	bRefresh);
	void	UpdateViewNameOthers(BOOL	bRefresh);
	void	UpdateViewHelpBaloonTip(BOOL	bRefresh);
	void	UpdateViewHelpBaloonChat(BOOL	bRefresh);
	void	UpdateViewHelpUclie(BOOL	bRefresh);
	void	UpdateViewSelfBars(BOOL	bRefresh);
	void	UpdatePartyBuffIcon(BOOL	bRefresh);
	void	UpdateViewHelmet(BOOL	bRefresh);
	void	UpdateViewItem(BOOL	bRefresh);

	void	UpdateCriminal	(BOOL	bRefresh);
	void	UpdateWanted	(BOOL	bRefresh);
	void	UpdateMurderer	(BOOL	bRefresh);

	VOID	RefreshRefuseControl(INT32 lOptionFlag);
	VOID	RefreshRefuseControl();

	void	EnableSaveLoad(BOOL bSaveLoad = TRUE)			{	m_bSaveLoad = bSaveLoad;		}
	void	SetStartMode(AgcmOptionStartup eStartupMode)	{	m_eStartupMode = eStartupMode;	}

	// 항목별로 Texture 품질을 return한다.
	AgcdUIOptionTexture	GetTextureQualityCharacter()	{ return m_eTQCharacter;	}
	AgcdUIOptionTexture	GetTextureQualityObject()		{ return m_eTQObject;		}
	AgcdUIOptionTexture	GetTextureQualityEffect()		{ return m_eTQEffect;		}
	AgcdUIOptionTexture	GetTextureQualityWorld()		{ return m_eTQWorld;		}

	// 내 캐릭터의 스킬이펙트를 끌 것인가?
	BOOL	IsDisableMySkillEffect( void )				{ return m_bDisableSkillEffect; }

	// 다른 플레이어의 스킬이펙트를 끌 것인가?
	BOOL	IsDisableOtherSkillEffect( void )			{ return m_bDisableOtherPlayerSkillEffect; }

	void	AddSystemMessageForSkillEffect( BOOL bIsOFF );
	void	AddSystemMessageForOtherSkillEffect( BOOL bIsOFF );

	static BOOL		CBLoginProcessEnd( PVOID pData, PVOID pClass, PVOID pCustData )		;	// Login 과정이 끝났다

	static BOOL		CBAutoTuning(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSpeedTuning(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCharView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBMapView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBWideView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBNature(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBShadow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBWater(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBBloom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBAutoExposure(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBMatEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBAntiAliasing(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBImpact(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBBloomVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBNaturalObj(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBLensFlare(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBFog(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSky(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBCriminal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBWanted(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBMurderer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBShadowChar(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBShadowSelf(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBShadowObject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBEffectField	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectCombat	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectUI		(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectSkill	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
// #ifdef _AREA_GLOBAL_
// 	static BOOL		CBMouseSpeedVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
// #endif
	static BOOL		CBBrightVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBBackgroundSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBBackgroundSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEnvSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEnvSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBUseEAX(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSpeakerType(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CB3DProvider(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBViewNameMine(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameMonster(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameGuild(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameOthers(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewHelpBaloonTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewHelpBaloonChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewHelpUclie(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewSelfBars(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBRefuseTrade(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefusePartyIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseGuildIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseGuildBattle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseBattle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseGuildRelation(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseBuddy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBDisableSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBVisiblePartyBuffIcon(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBVisibleViewHelmet(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBVisibleViewItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	static BOOL		CBSettingCharacterOK( PVOID pData, PVOID pClass, PVOID pCustData );	
	static BOOL		CBUpdateOptionFlag( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL		CBSystemMessage( PVOID pData, PVOID pClass, PVOID pCustData );

	/*
	 * UI display functions:
	 */

	static BOOL		CBDisplay3DProvider(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayAdvOptCategory(
		PVOID			pClass, 
		PVOID			pData, 
		AgcdUIDataType	eType, 
		INT32			lID, 
		CHAR *			szDisplay, 
		INT32 *			plValue, 
		AgcdUIControl *	pcsSourceControl);
	static BOOL CBDisplayAdvOptName(
		PVOID			pClass, 
		PVOID			pData, 
		AgcdUIDataType	eType, 
		INT32			lID, 
		CHAR *			szDisplay, 
		INT32 *			plValue, 
		AgcdUIControl *	pcsSourceControl);
	static BOOL CBDisplayAdvOptCheckbox(
		PVOID			pClass, 
		PVOID			pData, 
		AgcdUIDataType	eType, 
		INT32			lID, 
		CHAR *			szDisplay, 
		INT32 *			plValue, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBAdvOpCheckboxChanged(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL CBAdvOpCheckboxDefault(
		PVOID			pClass, 
		PVOID			pData, 
		AgcdUIDataType	eType, 
		AgcdUIControl *	pcsSourceControl);

	/* Item drop settings callbacks */

	/*
	 * Display callback for item drop color slider preview.
	 */
	static BOOL CBDisplayItemDropPreview(
		PVOID			pClass, 
		PVOID			pData, 
		AgcdUIDataType	eType, 
		INT32			lID, 
		CHAR *			szDisplay, 
		INT32 *			plValue, 
		AgcdUIControl *	pcsSourceControl);

	/*
	 * Item drop category combo button callback.
	 */
	static BOOL CBChangedItemDropCategory(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	/*
	 * Item drop (red) slider callback.
	 */
	static BOOL CBChangedItemDropSliderR(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	/*
	 * Item drop (green) slider callback.
	 */
	static BOOL CBChangedItemDropSliderG(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	/*
	 * Item drop (blue) slider callback.
	 */
	static BOOL CBChangedItemDropSliderB(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	/*
	 * Item drop settings enable checkbox callback.
	 */
	static BOOL CBChangedItemDropEnable(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	/*
	 * Item drop settings save button callback.
	 */
	static BOOL CBSaveItemDropSettings(
		PVOID			pClass, 
		PVOID			pData1, 
		PVOID			pData2, 
		PVOID			pData3, 
		PVOID			pData4, 
		PVOID			pData5, 
		ApBase *		pcsTarget, 
		AgcdUIControl *	pcsSourceControl);

	static BOOL		CBDisableOtherPlayerSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);	

protected:
	VOID			SendOptionFlag(INT32 lOptionFlag, BOOL bClick);
	VOID			SendOptionFlag();

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//. 2005. 09. 20 Nonstopdj
	//. script 엔진에 등록될 gamma correction method
	void			SetScreenContrast( int nContrast )
	{
		m_iScreenContrastCurrent = nContrast;
		UpdateBrightVal(true);	//. 적용
	}

	void			SetScreenBrightness( int nBrightness )
	{
		m_iScreenBrightnessCurrent = nBrightness;
		UpdateBrightVal(true);	//. 적용
	}

	void			SetScreenGamma( int nGamma )
	{
		m_iScreenBrightCurrent = nGamma;
		UpdateBrightVal(true);	//. 적용
	}
};

#endif       //_AGCMUIOPTION_H_	