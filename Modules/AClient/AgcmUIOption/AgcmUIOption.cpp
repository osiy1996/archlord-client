#include <AgcmUIOption/AgcmUIOption.h>
#include <AgcmUIConsole/AgcmUIConsole.h>
#include <ApModule/ApModuleStream.h>
#include <AgcmTextBoard/AgcmTextBoardMng.h>
#include <AgcmUIChatting2/AgcmUIChatting2.h>
#include <AgcmChatting2/AgcmChatting2.h>
#include <AgcmFPSManager/AgcmFPSManager.h>
#include <AgcmLogin/AgcmLogin.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmSound/AgcmSound.h>
#include <AgcmRender/AgcmRender.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmWater/AgcmWater.h>
#include <AgcmTuner/AgcmTuner.h>
#include <AgcmLODManager/AgcmLODManager.h>
#include <AgcmMap/AgcmMap.h>
#include <AgcmEventNature/AgcmEventNature.h>
#include <AgcmGrass/AgcmGrass.h>
#include <AgcmLensFlare/AgcmLensFlare.h>
#include <AgcmShadow/AgcmShadow.h>
#include <AgcmShadow2/AgcmShadow2.h>
#include <AgcmShadowmap/AgcmShadowmap.h>
#include <AgcmMinimap/AgcmMinimap.h>
#include <AgcmPostFX/AgcmPostFX.h>
#include <AgcmUITips/AgcmUITips.h>
#include <AgcmUIParty/AgcmUIParty.h>
#include <AgcmUIStatus/AgcmUIStatus.h>
#include <AgcmUIControl/AcUIButton.h>
#include <AgcmUIControl/AcUIScroll.h>
#include <AgcmUIControl/AcUICombo.h>
#include <AgcmUIOption/AgcdUIOption.h>
#include <AgcmEventEffect/AgcmEventEffect.h>
#include <AgcmTargeting/AgcmTargeting.h>
#include <AgcmItem/AgcmItem.h>

#define AGCD_OPTION_NAME_NEAR		"Option_Msg_NEAR"
#define AGCD_OPTION_NAME_NORMAL		"Option_Msg_NORMAL"
#define AGCD_OPTION_NAME_FAR		"Option_Msg_FAR"
#define AGCD_OPTION_NAME_ALL		"Option_Msg_ALL"
#define AGCD_OPTION_NAME_OFF		"Option_Msg_OFF"
#define AGCD_OPTION_NAME_ON			"Option_Msg_ON"
#define AGCD_OPTION_NAME_LOW		"Option_Msg_LOW"
#define AGCD_OPTION_NAME_MEDIUM		"Option_Msg_MEDIUM"
#define AGCD_OPTION_NAME_HIGH		"Option_Msg_HIGH"
#define AGCD_OPTION_NAME_TYPE1		"Option_Msg_TYPE1"
#define AGCD_OPTION_NAME_TYPE2		"Option_Msg_TYPE2"
#define AGCD_OPTION_NAME_TYPE3		"Option_Msg_TYPE3"
#define AGCD_OPTION_NAME_TYPE4		"Option_Msg_TYPE4"

#define AGCD_OPTION_BLOOM1_STRING		"DownSample-BoxBlur1-BoxBloomComp"
#define AGCD_OPTION_BLOOM2_STRING		"DownSample-BlurH-BlurV-BloomComp"
//#define AGCD_OPTION_BLOOM1_STRING		"DownSample-BlurH-BlurV-BloomComp"
//#define AGCD_OPTION_BLOOM2_STRING		"DownSample-BoxBlur1-BoxBloomComp"
#define AGCD_OPTION_AEXPOSURE_STRING	"LightAdapt"

static void
cbCheckboxLensNormalDmg(
	AgcmUIOption *			pcsAgcmUIOption,
	AgcdUIOptionListItem *	pstListItem,
	PVOID					pData)
{
	AgcmEventEffect * pcsAgcmEventEffect = (AgcmEventEffect *)pData;

	pcsAgcmEventEffect->DisplayLensNormalDamage(
		pstListItem->data.boolean.bValue);
}

static void
cbCheckboxLensSkillDmg(
	AgcmUIOption *			pcsAgcmUIOption,
	AgcdUIOptionListItem *	pstListItem,
	PVOID					pData)
{
	AgcmEventEffect * pcsAgcmEventEffect = (AgcmEventEffect *)pData;

	pcsAgcmEventEffect->DisplayLensSkillDamage(
		pstListItem->data.boolean.bValue);
}

static void
cbCheckboxLensSkillEffect(
	AgcmUIOption *			pcsAgcmUIOption,
	AgcdUIOptionListItem *	pstListItem,
	PVOID					pData)
{
	AgcmEventEffect * pcsAgcmEventEffect = (AgcmEventEffect *)pData;

	pcsAgcmEventEffect->DisplayLensSkillEffect(
		pstListItem->data.boolean.bValue);
}

static void
cbCheckboxMovementRMBDblClick(
	AgcmUIOption *			pcsAgcmUIOption,
	AgcdUIOptionListItem *	pstListItem,
	PVOID					pData)
{
	AgcmTargeting * pcsAgcmTargeting = 
		(AgcmTargeting *)pcsAgcmUIOption->GetModule("AgcmTargeting");

	pcsAgcmTargeting->EnableRMBDblClickMovement(
		pstListItem->data.boolean.bValue);
}

static void
cbComboItemDropCategory(
	AgcmUIOption *			pcsAgcmUIOption,
	AgcdUIOptionListItem *	pstListItem,
	PVOID					pData)
{
}

AgcmUIOption::AgcmUIOption()
{
	SetModuleName("AgcmUIOption");

	EnableIdle(TRUE);

	memset(m_aszNameView, 0, sizeof(CHAR *) * AGCD_OPTION_MAX_VIEW);
	memset(m_aszNameToggle, 0, sizeof(CHAR *) * AGCD_OPTION_MAX_TOGGLE);
	memset(m_aszNameNature, 0, sizeof(CHAR *) * AGCD_OPTION_MAX_NATURE);
	memset(m_aszNameEffect, 0, sizeof(CHAR *) * AGCD_OPTION_MAX_EFFECT);
	memset(m_aszNameEffect2, 0, sizeof(CHAR *) * AGCD_OPTION_MAX_EFFECT2);

	m_bWaterHigh	= TRUE;
	m_bBloom1		= TRUE;
	m_bBloom2		= TRUE;
	m_bAutoExposure = TRUE;

// #ifdef _AREA_GLOBA_
// 	m_iMouseSpeedMax		= 20;
// 	m_iMouseSpeedCurrent	= 10;
// 	m_iMouseSpeedOriginal	= 10;
// 	SystemParametersInfo(SPI_GETMOUSESPEED, 0, &m_iMouseSpeedOriginal, 0);
// #endif

	//. gamma��
	m_iScreenBrightMax		= 100;
	m_iScreenBrightCurrent	= 50;

	//. 2005. 9. 20 Nonstopdj
	m_iScreenContrastMax		= 100;
	m_iScreenContrastCurrent	= 100;
	m_iScreenBrightnessMax		= 100;
	m_iScreenBrightnessCurrent	= 100;


	m_iBloomMax		= 100;
	//@{ Jaewon 20050106
	// 20 == GlowIntensity 0.4
	m_iBloomCurrent	= 20;
	//@} Jaewon

	// Data Init(Sound)
	m_bBackgroundSound			= TRUE;
	m_iBackgroundSoundMax		= 100;
	m_iBackgroundSoundCurrent	= 100;

	m_bEffectSound				= TRUE;
	m_iEffectSoundMax			= 100;
	m_iEffectSoundCurrent		= 100;

	m_bEnvSound					= TRUE;
	m_iEnvSoundMax				= 100;
	m_iEnvSoundCurrent			= 100;

	m_aszSpeakerType[ SoundSpeakerMode_Default ]	= "2 Speakers";
	m_aszSpeakerType[ SoundSpeakerMode_HeadPhone ]	= "Headphones";
	m_aszSpeakerType[ SoundSpeakerMode_Surround ]	= "Surround";
	m_aszSpeakerType[ SoundSpeakerMode_Channel4 ]	= "4 Speakers";
	m_aszSpeakerType[ SoundSpeakerMode_Channel51 ]	= "5.1 Speakers";
	m_aszSpeakerType[ SoundSpeakerMode_Channel71 ]	= "7.1 Speakers";
	m_eSpeakerType				= SoundSpeakerMode_Default;
	m_l3DProvider				= -1;
	m_bUseEAX					= TRUE;

	// Data Init(ETC)
	m_bViewNameMine = TRUE;
	m_bViewNameMonster = FALSE;
	m_bViewNameGuild = TRUE;
	m_bViewNameParty = TRUE;
	m_bViewNameOthers = TRUE;
	m_bViewHelpBaloonTip = TRUE;
	m_bViewHelpBaloonChat = TRUE;
	m_bViewHelpUclie = TRUE;

	// 2005.06.02 By SungHoon
	m_bRefuseTrade = FALSE;
	m_bRefusePartyIn = FALSE;
	m_bRefuseGuildIn = FALSE;
	m_bRefuseGuildBattle = FALSE;
	m_bRefuseBattle = FALSE;

	m_bRefuseGuildRelation = FALSE;
	m_bRefuseBuddy = FALSE;

	m_bVisiblePartyBuffIcon = TRUE;	//	2005.06.16 By SungHoon
	m_bVisibleViewHelmet	= TRUE;
	m_bVisibleViewItem		= TRUE;

	m_bCriminal				= TRUE;
	m_bWanted				= TRUE;
	m_bMurderer				= TRUE;

	// Event IDs
	m_lEventIDAutoTuningEnable = 0;
	m_lEventIDAutoTuningDisable = 0;

	m_lEventIDBGMEnable = 0;
	m_lEventIDBGMDisable = 0;
	m_lEventIDESEnable = 0;
	m_lEventIDESDisable = 0;

	//-----------------------------------------------------------------------
	// ����Ʈ�� ���� �ػ�

	DEVMODE cDevMode;
	if( EnumDisplaySettings( NULL , ENUM_CURRENT_SETTINGS , & cDevMode ) )
	{
		m_iScreenDepth = cDevMode.dmBitsPerPel;
		m_iScreenHeight = cDevMode.dmPelsHeight;
		m_iScreenWidth = cDevMode.dmPelsWidth;
	}
	else
	{
		m_iScreenWidth = 1024;
		m_iScreenHeight = 768;
		m_iScreenDepth = 32;
	}

	//-----------------------------------------------------------------------

	strcpy(m_strSound,"2D Sound");
	strcpy(m_strSpeaker,"Stereo");

	m_ulUISkinType	= 1;

	m_pcsAgcmUIManager2 = NULL;
	m_pcsAgcmTuner = NULL;
	m_pcsAgcmShadow = NULL;
	m_pcsAgcmShadow2 = NULL;
	m_pcsAgcmEff2 = NULL;
	m_pcsAgcmSound = NULL;
	m_pcsAgcmTextBoardMng = NULL;
	m_pcsAcUIToolTip = NULL;
	m_pcsAgcmRender = NULL;
	m_pcsAgcmLogin = NULL;
	m_pcsAgcmUIParty = NULL;
	m_pcsAgcmUITips = NULL;

	m_pcsAgcmGrass = NULL;
	m_pcsAgcmLensFlare = NULL;
	m_pcsAgcmEventNature = NULL;
	m_pcsAgcmWater	= NULL;
	m_pcsAgcmShadowMap = NULL;

	m_pcsAgcmCharacter = NULL;		//	2005.06.02. By SungHoon
	m_pcsAgpmCharacter = NULL;		//	2005.06.02. By SungHoon

	m_pcsAgpmGuild = NULL;			//	2005.06.02. By SungHoon
	m_pcsAgcmGuild = NULL;			//	2005.06.02. By SungHoon
	m_pcsAgcmItem = NULL;

	m_bEnableGamma = FALSE;

	m_bSave = FALSE;
	m_bSaveLoad	= TRUE;
	m_bAutoPatchInitialized = FALSE;

	m_eStartupMode	= AGCM_OPTION_STARTUP_NORMAL;

	// ������ (2005-04-22 ���� 11:47:53) : 
	// �ε� ������ ����..
	m_nNearRough	= 4;
	m_nNearDetail	= 1;
	m_nNormalRough	= 6;
	m_nNormalDetail	= 1;
	m_nFarRough		= 7;
	m_nFarDetail	= 2;
	m_nAllRough		= 9;
	m_nAllDetail	= 3;

	//@{ 2006/10/20 burumal
	m_bWindowed = TRUE;
	m_bFramed = TRUE;
	//@}

	m_eEffectField	= AGCD_OPTION_EFFECT_HIGH;
	m_eEffectCombat	= AGCD_OPTION_EFFECT_HIGH;
	m_eEffectUI		= AGCD_OPTION_TOGGLE_ON;
	m_eEffectSkill	= AGCD_OPTION_TOGGLE_ON;

	m_bDisableSkillEffect = FALSE;
	m_bDisableOtherPlayerSkillEffect = FALSE;

	for (UINT32 i = 0; i < AGCDUIOPTION_LII_COUNT; i++)
		m_ulAdvOptIndices[i] = i;
	m_pstAdvOptUD = NULL;
	/*
	 * Init advanced options.
	 */
	memset(m_stAdvOptions, 0, sizeof(m_stAdvOptions));
	for (UINT32 i = 0; i < AGCDUIOPTION_LII_COUNT; i++)
		m_stAdvOptions[i].eIndex = (AgcdUIOptionListItemIndex)i;
	m_bAdvOptionsSetup = FALSE;

	m_bEnableItemDropSettings = FALSE;
	m_pstItemDropEnableUD = NULL;
	m_pstItemDropCategoryUD = NULL;
	memset(m_pastItemDropSliderUD, 0, sizeof(m_pastItemDropSliderUD));
	m_pstItemDropPreviewUD = NULL;
	memset(m_ulItemDropSliderValue, 0, sizeof(m_ulItemDropSliderValue));
	m_ulItemDropSliderMax = 255;
	m_eItemDropCategory = AGCD_ITEM_DROP_CATEGORYID_NORMAL_WEAPON;
}

AgcmUIOption::~AgcmUIOption()
{
}

BOOL AgcmUIOption::OnAddModule()
{
	AgcdUIOptionListItem * pstListItem;

	m_pcsAgcmUIManager2 = (AgcmUIManager2 *) GetModule("AgcmUIManager2");
	if (!m_pcsAgcmUIManager2)		return FALSE;

	m_pcsAgcmTuner = (AgcmTuner*) GetModule("AgcmTuner");
	m_pcsAgcmShadow = (AgcmShadow*) GetModule("AgcmShadow");
	m_pcsAgcmShadow2 = (AgcmShadow2*) GetModule("AgcmShadow2");
	m_pcsAgcmEff2 = (AgcmEff2*) GetModule("AgcmEff2");
	m_pcsAgcmSound = (AgcmSound*) GetModule("AgcmSound");
	m_pcsAgcmTextBoardMng = (AgcmTextBoardMng*) GetModule("AgcmTextBoardMng");
	m_pcsAgcmTextBoardMng->SetAgcmUIOption(	this );
	m_pcsAgcmRender = (AgcmRender*)	GetModule("AgcmRender");
	m_pcsAgcmLogin = (AgcmLogin*) GetModule("AgcmLogin");

	m_pcsAgcmGrass = (AgcmGrass*) GetModule("AgcmGrass");
	m_pcsAgcmLensFlare = (AgcmLensFlare*) GetModule("AgcmLensFlare");
	m_pcsAgcmEventNature = (AgcmEventNature*) GetModule("AgcmEventNature");
	m_pcsAgcmWater = (AgcmWater*) GetModule("AgcmWater");
	m_pcsAgcmShadowMap = (AgcmShadowmap*) GetModule("AgcmShadowmap");
	m_pcsAgcmMap = (AgcmMap*) GetModule("AgcmMap");
	
	m_pcsAgpmGuild = (AgpmGuild*) GetModule("AgpmGuild");	//2005.06.02 By SungHoon
	m_pcsAgcmGuild = (AgcmGuild*) GetModule("AgcmGuild");	//2005.06.02 By SungHoon

	m_pcsAgpmCharacter = (AgpmCharacter*) GetModule("AgpmCharacter");	//2005.06.02 By SungHoon
	m_pcsAgcmCharacter = (AgcmCharacter*) GetModule("AgcmCharacter");	//2005.06.02 By SungHoon

	m_pcsAgcmUIParty = ( AgcmUIParty *)GetModule("AgcmUIParty");		//	2005.06.16 By SungHoon
	m_pcsAgcmPostFX = (AgcmPostFX *) GetModule("AgcmPostFX");

	m_pcsAgcmUIStatus = (AgcmUIStatus *) GetModule("AgcmUIStatus");
	m_pcsAgcmUITips = (AgcmUITips *) GetModule("AgcmUITips");
	m_pcsAgcmEventEffect = (AgcmEventEffect *)GetModule("AgcmEventEffect");
	m_pcsAgcmItem = (AgcmItem *)GetModule("AgcmItem");
	if (m_pcsAgcmLogin)
		m_pcsAgcmLogin->SetCallbackEnterGameEnd(CBLoginProcessEnd,this);

	if (m_pcsAgcmCharacter)			//	2005.06.02. By SungHoon
	{		
		m_pcsAgcmCharacter->SetCallbackUpdateOptionFlag(CBUpdateOptionFlag,this);	
		m_pcsAgcmCharacter->SetCallbackSettingCharacterOK(CBSettingCharacterOK,this);
	}

	if (m_pcsAgpmGuild)				//	2005.06.07. By SungHoon
	{
		m_pcsAgpmGuild->SetCallbackSystemMessage(CBSystemMessage, this);
	}
	/*
	 * Setup advanced options.
	 */
	pstListItem = &m_stAdvOptions[AGCDUIOPTION_LII_CATEGORY_LENS_EFFECT];
	pstListItem->SetCategory("Lens Stone Effect Settings");
	pstListItem = &m_stAdvOptions[AGCDUIOPTION_LII_LENS_ON_NORMAL_DAMAGE];
	pstListItem->SetBoolean(
		"Display upon normal attack damage",
		"UD_DisplayLensNormalDmg",
		"ADV_LENS_NORMAL",
		TRUE,
		cbCheckboxLensNormalDmg,
		m_pcsAgcmEventEffect);
	pstListItem = &m_stAdvOptions[AGCDUIOPTION_LII_LENS_ON_SKILL_DAMAGE];
	pstListItem->SetBoolean(
		"Display upon skill damage",
		"UD_DisplayLensSkillDmg",
		"ADV_LENS_SKILL",
		TRUE,
		cbCheckboxLensSkillDmg,
		m_pcsAgcmEventEffect);
	pstListItem = &m_stAdvOptions[AGCDUIOPTION_LII_LENS_ON_SKILL_EFFECT];
	pstListItem->SetBoolean(
		"Display upon new skill effect",
		"UD_DisplayLensSkillEff",
		"ADV_LENS_SKILL_EFF",
		TRUE,
		cbCheckboxLensSkillEffect,
		m_pcsAgcmEventEffect);
	pstListItem = &m_stAdvOptions[AGCDUIOPTION_LII_CATEGORY_MOVEMENT];
	pstListItem->SetCategory("Movement Settings");
	pstListItem = &m_stAdvOptions[AGCDUIOPTION_LII_MOVEMENT_RMB_DBLCLICK];
	pstListItem->SetBoolean(
		"Move when right mouse button is double-clicked",
		"UD_EnableMoveRMBDblClick",
		"ADV_MOVEMENT_RMB_DBLCLICK",
		FALSE,
		cbCheckboxMovementRMBDblClick,
		NULL);
	m_bAdvOptionsSetup = TRUE;
	// Video Setting Event
	m_lEventUpdateWater = m_pcsAgcmUIManager2->AddEvent("OP_UpdateWater", NULL, this);
	if (!m_lEventUpdateWater)
		return FALSE;

	m_lEventUpdateBloom = m_pcsAgcmUIManager2->AddEvent("OP_UpdateBloom", NULL, this);
	if (!m_lEventUpdateBloom)
		return FALSE;

	m_lEventUpdateAutoExposure = m_pcsAgcmUIManager2->AddEvent("OP_UpdateAutoExposure", NULL, this);
	if (!m_lEventUpdateAutoExposure)
		return FALSE;

	// Video Setting UserData
	m_pstUDAutoTuning = m_pcsAgcmUIManager2->AddUserData("OP_AutoTuning", &m_bAutoTuning, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDAutoTuning < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_AutoTuning", CBAutoTuning, 0))		return FALSE;

	m_pstUDSpeedTuning = m_pcsAgcmUIManager2->AddUserData("OP_SpeedTuning", &m_bSpeedTuning, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDSpeedTuning < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_SpeedTuning", CBSpeedTuning, 0))		return FALSE;

	if (!(m_pstUDCharView = AddOptionUserData(AGCD_OPTION_TYPE_VIEW, "OP_CharView", CBCharView)))
		return FALSE;

	if (!(m_pstUDMapView = AddOptionUserData(AGCD_OPTION_TYPE_VIEW, "OP_MapView", CBMapView)))
		return FALSE;

	if (!(m_pstUDWideView = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_WideView", CBWideView)))
		return FALSE;

	if (!(m_pstUDNature = AddOptionUserData(AGCD_OPTION_TYPE_NATURE, "OP_Nature", CBNature)))
		return FALSE;

	if (!(m_pstUDShadow = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT, "OP_Shadow", CBShadow)))
		return FALSE;

	if (!(m_pstUDEffect = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT, "OP_Effect", CBEffect)))
		return FALSE;

	if (!(m_pstUDWater = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT, "OP_Water", CBWater)))
		return FALSE;

	if (!(m_pstUDBloom = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT2, "OP_Bloom", CBBloom)))
		return FALSE;
	m_pstUDBloom->m_stUserData.m_lCount = 3;	// Off, Type1, Type2 ������ ����.

	if (!(m_pstUDAutoExposure = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_AutoExposure", CBAutoExposure)))
		return FALSE;

	if (!(m_pstUDMatEffect = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_MatEffect", CBMatEffect)))
		return FALSE;

	if (!(m_pstUDAntiAliasing = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_AntiAliasing", CBAntiAliasing)))
		return FALSE;

	if (!(m_pstUDImpact = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_Impact", CBImpact)))
		return FALSE;

	// Video Setting (Nature) UserData
	if (!(m_pstUDNaturalObj = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT, "OP_NaturalObj", CBNaturalObj)))
		return FALSE;

	if (!(m_pstUDLensFlare = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_LensFlare", CBLensFlare)))
		return FALSE;

	if (!(m_pstUDFog = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_Fog", CBFog)))
		return FALSE;

	if (!(m_pstUDSky = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_Sky", CBSky)))
		return FALSE;

	// Video Setting (Shadow) UserData
	if (!(m_pstUDShadowChar = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT, "OP_ShadowChar", CBShadowChar)))
		return FALSE;

	if (!(m_pstUDShadowSelf = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_ShadowSelf", CBShadowSelf)))
		return FALSE;

	if (!(m_pstUDShadowObject = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_ShadowObject", CBShadowObject)))
		return FALSE;

	// Video Setting (Shadow) UserData
	if (!(m_pstUDEffectField = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT, "OP_EffectField", CBEffectField)))
		return FALSE;

	if (!(m_pstUDEffectCombat = AddOptionUserData(AGCD_OPTION_TYPE_EFFECT, "OP_EffectCombat", CBEffectCombat)))
		return FALSE;

	if (!(m_pstUDEffectUI = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_EffectUI", CBEffectUI)))
		return FALSE;

	if (!(m_pstUDEffectSkill = AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_EffectSkill", CBEffectSkill)))
		return FALSE;

// #ifdef _AREA_GLOBAL_
// 	m_pstUDMouseSpeedMax = m_pcsAgcmUIManager2->AddUserData("OP_MouseSpeedMax", &m_iMouseSpeedMax, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
// 	if (!m_pstUDMouseSpeedMax)		return FALSE;
// 	m_pstUDMouseSpeedCurrent = m_pcsAgcmUIManager2->AddUserData("OP_MouseSpeedCurrent", &m_iMouseSpeedCurrent, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
// 	if (!m_pstUDMouseSpeedCurrent)		return FALSE;
// 	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_MouseSpeedVal", CBMouseSpeedVal, 0))		return FALSE;
// #endif

	//if (!(m_pstUDCriminal	= AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_Criminal", CBCriminal)))
	//	return FALSE;
	//if (!(m_pstUDWanted		= AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_Wanted", CBWanted)))
	//	return FALSE;
	//if (!(m_pstUDMurderer	=	 AddOptionUserData(AGCD_OPTION_TYPE_TOGGLE, "OP_Murderer", CBMurderer)))
	//	return FALSE;
	m_pstUDScreenBrightMax = m_pcsAgcmUIManager2->AddUserData("OP_BrightMax", &m_iScreenBrightMax, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDScreenBrightMax)		return FALSE;
	m_pstUDScreenBrightCurrent = m_pcsAgcmUIManager2->AddUserData("OP_BrightCurrent", &m_iScreenBrightCurrent, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDScreenBrightCurrent)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_BrightVal", CBBrightVal, 0))		return FALSE;

	m_pstUDBloomMax = m_pcsAgcmUIManager2->AddUserData("OP_BloomMax", &m_iBloomMax, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDBloomMax)		return FALSE;
	m_pstUDBloomCurrent = m_pcsAgcmUIManager2->AddUserData("OP_BloomCurrent", &m_iBloomCurrent, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDBloomCurrent)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_BloomVal", CBBloomVal, 0))		return FALSE;

	// Sound
	m_pstUDBackgroundSound = m_pcsAgcmUIManager2->AddUserData("OP_BackgroundSoundEnable", &m_bBackgroundSound, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstUDBackgroundSound)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_BackgroundSoundEnable", CBBackgroundSoundEnable, 0))		return FALSE;

	m_pstUDBackgroundSoundMax = m_pcsAgcmUIManager2->AddUserData("OP_BackgroundSoundMax", &m_iBackgroundSoundMax, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDBackgroundSoundMax)		return FALSE;
	m_pstUDBackgroundSoundCurrent = m_pcsAgcmUIManager2->AddUserData("OP_BackgroundSoundCurrent", &m_iBackgroundSoundCurrent, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDBackgroundSoundCurrent)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_BackgroundSoundVal", CBBackgroundSoundVal, 0))		return FALSE;

	m_pstUDEffectSound = m_pcsAgcmUIManager2->AddUserData("OP_EffectSoundEnable", &m_bEffectSound, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstUDEffectSound)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_EffectSoundEnable", CBEffectSoundEnable, 0))		return FALSE;

	m_pstUDEffectSoundMax = m_pcsAgcmUIManager2->AddUserData("OP_EffectSoundMax", &m_iEffectSoundMax, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDEffectSoundMax)		return FALSE;
	m_pstUDEffectSoundCurrent = m_pcsAgcmUIManager2->AddUserData("OP_EffectSoundCurrent", &m_iEffectSoundCurrent, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDEffectSoundCurrent)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_EffectSoundVal", CBEffectSoundVal, 0))		return FALSE;

	m_pstUDEnvSound = m_pcsAgcmUIManager2->AddUserData("OP_EnvSoundEnable", &m_bEnvSound, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstUDEnvSound)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_EnvSoundEnable", CBEnvSoundEnable, 0))		return FALSE;

	m_pstUDEnvSoundMax = m_pcsAgcmUIManager2->AddUserData("OP_EnvSoundMax", &m_iEnvSoundMax, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDEnvSoundMax)		return FALSE;
	m_pstUDEnvSoundCurrent = m_pcsAgcmUIManager2->AddUserData("OP_EnvSoundCurrent", &m_iEnvSoundCurrent, sizeof(UINT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDEnvSoundCurrent)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_EnvSoundVal", CBEnvSoundVal, 0))		return FALSE;

	m_pstUDSpeakerType = m_pcsAgcmUIManager2->AddUserData("OP_SpeakerType", m_aszSpeakerType, sizeof(CHAR *), AGCMOPTION_MAX_SPEAKER_TYPE, AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pstUDSpeakerType)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_SpeakerType", CBSpeakerType, 0))		return FALSE;

	m_pstUDUseEAX = m_pcsAgcmUIManager2->AddUserData("OP_UseEAX", &m_bUseEAX, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstUDUseEAX)				return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_UseEAX", CBUseEAX, 0))		return FALSE;

	int nSoundDeviceCount = m_pcsAgcmSound->GetDeviceCount();
	m_pstUD3DProvider = m_pcsAgcmUIManager2->AddUserData("OP_3DProvider", m_pcsAgcmSound->m_stDevice, sizeof(stSoundDeviceEntry), nSoundDeviceCount, AGCDUI_USERDATA_TYPE_ETC);
	if (!m_pstUD3DProvider)			return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_3DProvider", CB3DProvider, 0))		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "OP_3DProvider", 0, CBDisplay3DProvider, AGCDUI_USERDATA_TYPE_ETC))	return FALSE;

	// ETC
	m_pstUDViewNameMine = m_pcsAgcmUIManager2->AddUserData("OP_ViewNameMine", &m_bViewNameMine, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewNameMine < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewNameMine", CBViewNameMine, 0))		return FALSE;

	m_pstUDViewNameMonster = m_pcsAgcmUIManager2->AddUserData("OP_ViewNameMonster", &m_bViewNameMonster, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewNameMonster < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewNameMonster", CBViewNameMonster, 0))		return FALSE;

	m_pstUDViewNameGuild = m_pcsAgcmUIManager2->AddUserData("OP_ViewNameGuild", &m_bViewNameGuild, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewNameGuild < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewNameGuild", CBViewNameGuild, 0))		return FALSE;

	m_pstUDViewNameParty = m_pcsAgcmUIManager2->AddUserData("OP_ViewNameParty", &m_bViewNameParty, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewNameParty < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewNameParty", CBViewNameParty, 0))		return FALSE;

	m_pstUDViewNameOthers = m_pcsAgcmUIManager2->AddUserData("OP_ViewNameOthers", &m_bViewNameOthers, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewNameOthers < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewNameOthers", CBViewNameOthers, 0))		return FALSE;

	m_pstUDViewHelpBaloonTip = m_pcsAgcmUIManager2->AddUserData("OP_ViewHelpBaloonTip", &m_bViewHelpBaloonTip, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewHelpBaloonTip < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewHelpBaloonTip", CBViewHelpBaloonTip, 0))		return FALSE;

	m_pstUDViewHelpBaloonChat = m_pcsAgcmUIManager2->AddUserData("OP_ViewBaloonChat", &m_bViewHelpBaloonChat, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewHelpBaloonChat < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewHelpBaloonChat", CBViewHelpBaloonChat, 0))		return FALSE;

	m_pstUDViewHelpUclie = m_pcsAgcmUIManager2->AddUserData("OP_ViewHelpUclie", &m_bViewHelpUclie, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewHelpUclie < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewHelpUclie", CBViewHelpUclie, 0))		return FALSE;

	m_pstUDViewSelfBars = m_pcsAgcmUIManager2->AddUserData("OP_ViewSelfBars", &m_bViewSelfBars, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDViewSelfBars < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_ViewSelfBars", CBViewSelfBars, 0))		return FALSE;

//	Start 2005.06.02. By SungHoon
	m_pstUDRefuseTrade = m_pcsAgcmUIManager2->AddUserData("OP_RefuseTrade", &m_bRefuseTrade, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDRefuseTrade < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_RefuseTrade", CBRefuseTrade, 0))		return FALSE;

	m_pstUDRefusePartyIn = m_pcsAgcmUIManager2->AddUserData("OP_RefusePartyIn", &m_bRefusePartyIn, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDRefusePartyIn < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_RefusePartyIn", CBRefusePartyIn, 0))		return FALSE;

	m_pstUDRefuseGuildIn = m_pcsAgcmUIManager2->AddUserData("OP_RefuseGuildIn", &m_bRefuseGuildIn, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDRefuseGuildIn < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_RefuseGuildIn", CBRefuseGuildIn, 0))		return FALSE;

	m_pstUDRefuseGuildBattle = m_pcsAgcmUIManager2->AddUserData("OP_RefuseGuildBattle", &m_bRefuseGuildBattle, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDRefuseGuildBattle < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_RefuseGuildBattle", CBRefuseGuildBattle, 0))		return FALSE;

	m_pstUDRefuseBattle = m_pcsAgcmUIManager2->AddUserData("OP_RefuseBattle", &m_bRefuseBattle, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDRefuseBattle < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_RefuseBattle", CBRefuseBattle, 0))		return FALSE;
//	Finish 2005.06.02 By SungHoon

	//@{ 2007/01/22 burumal
	m_pstUDDisableSkillEffect = m_pcsAgcmUIManager2->AddUserData("OP_DisableSkillEffect", &m_bDisableSkillEffect, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDDisableSkillEffect < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_DisableSkillEffect", CBDisableSkillEffect, 0))		return FALSE;
	//@}

	// 2007.07.11. steeple
	m_pstUDRefuseGuildRelation = m_pcsAgcmUIManager2->AddUserData("OP_RefuseGuildRelation", &m_bRefuseGuildRelation, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDRefuseGuildRelation < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_RefuseGuildRelation", CBRefuseGuildRelation, 0))		return FALSE;
	m_pstUDRefuseBuddy = m_pcsAgcmUIManager2->AddUserData("OP_RefuseBuddy", &m_bRefuseBuddy, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDRefuseBuddy < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_RefuseBuddy", CBRefuseBuddy, 0))		return FALSE;

//	Start 2005.06.02. By SungHoon
	m_pstUDVisiblePartyBuffIcon = m_pcsAgcmUIManager2->AddUserData("OP_VisiblePartyBuffIcon", &m_bVisiblePartyBuffIcon, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDVisiblePartyBuffIcon < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_VisiblePartyBuffIcon", CBVisiblePartyBuffIcon, 0))		return FALSE;
//	Finish 2005.06.02 By SungHoon

	m_pstUDVisibleViewHelmet = m_pcsAgcmUIManager2->AddUserData("OP_VisibleViewHelmet", &m_bVisibleViewHelmet, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDVisibleViewHelmet < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_VisibleViewHelmet", CBVisibleViewHelmet, 0))		return FALSE;

	m_pstUDVisibleViewItem = m_pcsAgcmUIManager2->AddUserData("OP_VisibleViewItem", &m_bVisibleViewItem, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDVisibleViewItem < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_VisibleViewItem", CBVisibleViewItem, 0))			return FALSE;

	m_pstUDVisibleCriminal = m_pcsAgcmUIManager2->AddUserData("OP_VisibleCriminal", &m_bCriminal, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDVisibleCriminal < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_VisibleCriminal", CBCriminal, 0))			return FALSE;

	m_pstUDVisibleWanted= m_pcsAgcmUIManager2->AddUserData("OP_VisibleWanted", &m_bWanted, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDVisibleWanted < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_VisibleWanted", CBWanted, 0))			return FALSE;

	m_pstUDVisibleMurderer= m_pcsAgcmUIManager2->AddUserData("OP_VisibleMurderer", &m_bMurderer, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDVisibleMurderer < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_VisibleMurderer", CBMurderer, 0))
		return FALSE;
	m_pstAdvOptUD = m_pcsAgcmUIManager2->AddUserData("OpAdv_ListUD",
		&m_ulAdvOptIndices[0],
		sizeof(UINT32),
		AGCDUIOPTION_LII_COUNT,
		AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstAdvOptUD)
		return FALSE;
	for (UINT32 i = 0; i < AGCDUIOPTION_LII_COUNT; i++) {
		AgcdUIOptionListItem * pstItem = &m_stAdvOptions[i];

		switch (pstItem->eType) {
		case AGCDUIOPTION_ADVANCED_OPTION_TYPE_BOOLEAN:
			pstItem->data.boolean.pstUD = m_pcsAgcmUIManager2->AddUserData(
				pstItem->data.boolean.szUDName,
				&pstItem->data.boolean.bValue,
				sizeof(BOOL),
				1,
				AGCDUI_USERDATA_TYPE_BOOL);
			if (!pstItem->data.boolean.pstUD)
				return FALSE;
			break;
		}
	}
	// Events
	m_lEventIDAutoTuningEnable = m_pcsAgcmUIManager2->AddEvent("OP_AutoTuning_Enable");
	if (m_lEventIDAutoTuningEnable < 0)		return FALSE;
	m_lEventIDAutoTuningDisable = m_pcsAgcmUIManager2->AddEvent("OP_AutoTuning_Disable");
	if (m_lEventIDAutoTuningDisable < 0)		return FALSE;

	/*
	// high quality water ���� set enable/disable(dependent on h/w spec)
	m_lEventHWaterEnable = m_pcsAgcmUIManager2->AddEvent("OP_HWater_Enable");
	if (m_lEventHWaterEnable < 0)		return FALSE;
	m_lEventHWaterDisable = m_pcsAgcmUIManager2->AddEvent("OP_HWater_Disable");
	if (m_lEventHWaterDisable < 0)		return FALSE;
	*/

	m_lEventIDBGMEnable = m_pcsAgcmUIManager2->AddEvent("OP_BGM_Enable");
	if (m_lEventIDBGMEnable < 0)		return FALSE;
	m_lEventIDBGMDisable = m_pcsAgcmUIManager2->AddEvent("OP_BGM_Disable");
	if (m_lEventIDBGMDisable < 0)		return FALSE;
	m_lEventIDESEnable = m_pcsAgcmUIManager2->AddEvent("OP_EffectSound_Enable");
	if (m_lEventIDESEnable < 0)		return FALSE;
	m_lEventIDESDisable = m_pcsAgcmUIManager2->AddEvent("OP_EffectSound_Disable");
	if (m_lEventIDESDisable < 0)		return FALSE;


	m_pstUDDisableOtherPlayerSkillEffect = m_pcsAgcmUIManager2->AddUserData( "OP_DisableOtherPlayerSkillEffect", &m_bDisableOtherPlayerSkillEffect, sizeof(BOOL), 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (m_pstUDDisableOtherPlayerSkillEffect < 0)		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "OP_DisableOtherPlayerSkillEffect", CBDisableOtherPlayerSkillEffect, 0)) return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, 
		"OpAdv_DisplayCategory", 0, CBDisplayAdvOptCategory,
		AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, 
		"OpAdv_DisplayName", 0, CBDisplayAdvOptName, 
		AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, 
		"OpAdv_DisplayCheckbox", 0, CBDisplayAdvOptCheckbox, 
		AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"OpAdv_CheckboxChanged", CBAdvOpCheckboxChanged, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddBoolean(this,
		"OpAdv_CheckboxDefault", CBAdvOpCheckboxDefault,
		AGCDUI_USERDATA_TYPE_NONE))
		return FALSE;
	/*
	 * Refresh advanced option list.
	 *
	 * Advanced options have a fixed number so this is needed 
	 * only once.
	 */
	m_pstAdvOptUD->m_stUserData.m_lCount = AGCDUIOPTION_LII_COUNT;
	m_pcsAgcmUIManager2->RefreshUserData(m_pstAdvOptUD);
	/*
	 * Setup item drop settings.
	 */
	m_pstItemDropEnableUD = m_pcsAgcmUIManager2->AddUserData(
		"DS_Enable",
		&m_bEnableItemDropSettings,
		sizeof(BOOL),
		1,
		AGCDUI_USERDATA_TYPE_BOOL);
	if (!m_pstItemDropEnableUD)
		return FALSE;
	m_pstItemDropCategoryUD = m_pcsAgcmUIManager2->AddUserData(
		"DS_Category",
		g_pszAgcdItemDropCategoryTags,
		sizeof(const char *),
		AGCD_ITEM_DROP_CATEGORYID_COUNT,
		AGCDUI_USERDATA_TYPE_STRING);
	if (!m_pstItemDropCategoryUD)
		return FALSE;
	m_pstItemDropCategoryUD->m_lSelectedIndex = 0;
	m_pastItemDropSliderUD[0] = m_pcsAgcmUIManager2->AddUserData(
		"DS_RSliderCurrent",
		&m_ulItemDropSliderValue[0],
		sizeof(UINT32),
		1,
		AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pastItemDropSliderUD[0])
		return FALSE;
	m_pastItemDropSliderUD[1] = m_pcsAgcmUIManager2->AddUserData(
		"DS_GSliderCurrent",
		&m_ulItemDropSliderValue[1],
		sizeof(UINT32),
		1,
		AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pastItemDropSliderUD[1])
		return FALSE;
	m_pastItemDropSliderUD[2] = m_pcsAgcmUIManager2->AddUserData(
		"DS_BSliderCurrent",
		&m_ulItemDropSliderValue[2],
		sizeof(UINT32),
		1,
		AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pastItemDropSliderUD[2])
		return FALSE;
	m_pcsAgcmUIManager2->AddUserData(
		"DS_SliderMax",
		&m_ulItemDropSliderMax,
		sizeof(UINT32),
		1,
		AGCDUI_USERDATA_TYPE_INT32);
	m_pstItemDropPreviewUD = m_pcsAgcmUIManager2->AddUserData(
		"DS_Preview",
		this,
		sizeof(this),
		1,
		AGCDUI_USERDATA_TYPE_ETC);
	if (!m_pstItemDropPreviewUD)
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, 
		"DS_Preview", 0, CBDisplayItemDropPreview, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this,
		"DS_Category", CBChangedItemDropCategory, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this,
		"DS_RSlider", CBChangedItemDropSliderR, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this,
		"DS_GSlider", CBChangedItemDropSliderG, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this,
		"DS_BSlider", CBChangedItemDropSliderB, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this,
		"DS_Enable", CBChangedItemDropEnable, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this,
		"DS_Save", CBSaveItemDropSettings, 0))
		return FALSE;
	return TRUE;
}

BOOL AgcmUIOption::OnInit()
{
	UINT32 ulColor;

	m_pcsAgcmMinimap = (AgcmMinimap *) GetModule("AgcmMinimap");

	if (m_bSaveLoad)
	{
		//������ ������ ���� �ѹ� ������
		FILE*	fp = fopen(PATCHCLIENT_OPTION_FILENAME,"r");
		if(!fp)
			SaveToINI(PATCHCLIENT_OPTION_FILENAME);
		else
			fclose(fp);
	}

	D3DCAPS9* 	pCaps = (D3DCAPS9*)RwD3D9GetCaps();	
	if(pCaps->Caps2 & D3DCAPS2_FULLSCREENGAMMA )
		m_bEnableGamma = TRUE;

	AgcdUIOptionNature	eNatureBackup = m_eNature;
	AgcdUIOptionView	eMapView = m_eMapView;
	BOOL				bViewNameOthersBackUp = m_bViewNameOthers;

//	m_eNature		= AGCD_OPTION_NATURE_HIGH;
	m_eMapView		= AGCD_OPTION_VIEW_ALL;
	m_bViewNameOthers	= TRUE;

	if (m_pcsAgcmSound)
	{
		m_pstUD3DProvider->m_stUserData.m_lCount = m_pcsAgcmSound->GetDeviceCount();
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUD3DProvider);
	}

	UpdateGameOption();

	m_pcsAgcmRender->m_iDrawRange = 10;

	m_eNature		= eNatureBackup;
	m_eMapView		= eMapView;
	m_bViewNameOthers =	bViewNameOthersBackUp;

	AS_REGISTER_TYPE_BEGIN( AgcmUIOption , AgcmUIOption );
		AS_REGISTER_VARIABLE( int , m_nNearRough	);
		AS_REGISTER_VARIABLE( int , m_nNearDetail	);
		AS_REGISTER_VARIABLE( int , m_nNormalRough	);
		AS_REGISTER_VARIABLE( int , m_nNormalDetail	);
		AS_REGISTER_VARIABLE( int , m_nFarRough		);
		AS_REGISTER_VARIABLE( int , m_nFarDetail	);
		AS_REGISTER_VARIABLE( int , m_nAllRough		);
		AS_REGISTER_VARIABLE( int , m_nAllDetail	);
		AS_REGISTER_METHOD1(void, SetScreenContrast, int);
		AS_REGISTER_METHOD1(void, SetScreenBrightness, int);
		AS_REGISTER_METHOD1(void, SetScreenGamma, int);
	AS_REGISTER_TYPE_END;

	/*
	 * By now, AgcmItem should've loaded item 
	 * drop configuration, so we can refresh UI controls.
	 */
	m_bEnableItemDropSettings = m_pcsAgcmItem->IsItemDropSettingsEnabled();
	ulColor = m_pcsAgcmItem->GetItemDropColor(m_eItemDropCategory);
	m_ulItemDropSliderValue[0] = (ulColor >> 16) & 0xFF;
	m_ulItemDropSliderValue[1] = (ulColor >> 8) & 0xFF;
	m_ulItemDropSliderValue[2] = ulColor & 0xFF;
	m_pcsAgcmUIManager2->RefreshUserData(m_pstItemDropEnableUD);
	m_pcsAgcmUIManager2->RefreshUserData(m_pastItemDropSliderUD[0]);
	m_pcsAgcmUIManager2->RefreshUserData(m_pastItemDropSliderUD[1]);
	m_pcsAgcmUIManager2->RefreshUserData(m_pastItemDropSliderUD[2]);
	return TRUE;
}


BOOL AgcmUIOption::OnDestroy()
{
	if(m_bSave)
		SaveToINI(PATCHCLIENT_OPTION_FILENAME);
// #ifdef _AREA_GLOBAL_
// 	SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID) m_iMouseSpeedOriginal, 0);
// #endif
	return TRUE;
}

BOOL AgcmUIOption::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

AgcdUIUserData *	AgcmUIOption::AddOptionUserData(AgcdUIOptionType eType, CHAR *szName, AgcUICallBack fnCallback)
{
	AgcdUIUserData *	pstUserData = NULL;

	switch (eType)
	{
	case AGCD_OPTION_TYPE_VIEW:
		pstUserData = m_pcsAgcmUIManager2->AddUserData(szName, m_aszNameView, sizeof(CHAR *), AGCD_OPTION_MAX_VIEW, AGCDUI_USERDATA_TYPE_STRING);
		break;

	case AGCD_OPTION_TYPE_TOGGLE:
		pstUserData = m_pcsAgcmUIManager2->AddUserData(szName, m_aszNameToggle, sizeof(CHAR *), AGCD_OPTION_MAX_TOGGLE, AGCDUI_USERDATA_TYPE_STRING);
		break;

	case AGCD_OPTION_TYPE_NATURE :
		pstUserData = m_pcsAgcmUIManager2->AddUserData(szName, m_aszNameNature, sizeof(CHAR *), AGCD_OPTION_MAX_NATURE, AGCDUI_USERDATA_TYPE_STRING);
		break;

	case AGCD_OPTION_TYPE_EFFECT:
		pstUserData = m_pcsAgcmUIManager2->AddUserData(szName, m_aszNameEffect, sizeof(CHAR *), AGCD_OPTION_MAX_EFFECT, AGCDUI_USERDATA_TYPE_STRING);
		break;

	case AGCD_OPTION_TYPE_EFFECT2:
		pstUserData = m_pcsAgcmUIManager2->AddUserData(szName, m_aszNameEffect2, sizeof(CHAR *), AGCD_OPTION_MAX_EFFECT2, AGCDUI_USERDATA_TYPE_STRING);
		break;
	}

	if (!pstUserData)
		return NULL;

	if (!m_pcsAgcmUIManager2->AddFunction(this, szName, fnCallback, 0))
		return NULL;

	return pstUserData;
}

void AgcmUIOption::InitGameOption()
{
	// Data Init(Video)
	m_bAutoTuning	= FALSE;
	m_bSpeedTuning	= FALSE;

	m_eCharView		= AGCD_OPTION_VIEW_NORMAL;
	m_eMapView		= AGCD_OPTION_VIEW_NORMAL;
	m_eWideView		= AGCD_OPTION_TOGGLE_OFF;
	m_eNature		= AGCD_OPTION_NATURE_MEDIUM;
	m_eShadow		= AGCD_OPTION_EFFECT_MEDIUM;
	m_eEffect		= AGCD_OPTION_EFFECT_MEDIUM;
	m_eWater		= AGCD_OPTION_EFFECT_MEDIUM;
	m_eBloom		= AGCD_OPTION_EFFECT2_OFF;
	m_eAutoExposure	= AGCD_OPTION_TOGGLE_OFF;
	m_eMatEffect	= AGCD_OPTION_TOGGLE_OFF;
	m_eAntiAliasing	= AGCD_OPTION_TOGGLE_OFF;
	m_eImpact		= AGCD_OPTION_TOGGLE_OFF;

	m_eTQCharacter	= AGCD_OPTION_TEXTURE_HIGH;
	m_eTQObject	= AGCD_OPTION_TEXTURE_HIGH;
	m_eTQEffect	= AGCD_OPTION_TEXTURE_HIGH;
	m_eTQWorld		= AGCD_OPTION_TEXTURE_HIGH;

	switch (m_eStartupMode)
	{
	case AGCM_OPTION_STARTUP_LOW:
		// Data Init(Video)
		m_bAutoTuning	= FALSE;
		m_bSpeedTuning	= TRUE;

		m_eCharView		= m_eMapView										= AGCD_OPTION_VIEW_NEAR;
		m_eNature		= AGCD_OPTION_NATURE_LOW;
		m_eShadow		= m_eEffect			= m_eWater		= AGCD_OPTION_EFFECT_LOW;
		m_eBloom															= AGCD_OPTION_EFFECT2_OFF;
		m_eAutoExposure	= m_eMatEffect	= m_eAntiAliasing	= m_eImpact		= AGCD_OPTION_TOGGLE_OFF;
		m_eTQCharacter	= m_eTQObject	= m_eTQEffect		= m_eTQWorld	= AGCD_OPTION_TEXTURE_LOW;
		break;

	case AGCM_OPTION_STARTUP_HIGH:
		m_bAutoTuning	= TRUE;
		m_bSpeedTuning	= FALSE;

		m_eCharView		= m_eMapView										= AGCD_OPTION_VIEW_ALL;
		m_eNature		= AGCD_OPTION_NATURE_HIGH;
		m_eShadow		= m_eEffect			= m_eWater		= AGCD_OPTION_EFFECT_HIGH;
		m_eBloom															= AGCD_OPTION_EFFECT2_TYPE1;
		m_eAutoExposure	= m_eMatEffect	= m_eAntiAliasing	= m_eImpact		= AGCD_OPTION_TOGGLE_ON;
		m_eTQCharacter	= m_eTQObject	= m_eTQEffect		= m_eTQWorld	= AGCD_OPTION_TEXTURE_HIGH;
		break;
	}

	// �ʱ�ȭ �ϰ� �׳� ������ �ȵǰ�.. ������ �������·� �ʱ�ȭ �Ǿ����..
	LoadFromINI( PATCHCLIENT_OPTION_FILENAME );
}

void AgcmUIOption::UpdateGameOption()
{
	if (m_pcsAgcmWater)
		m_bWaterHigh	= m_pcsAgcmWater->m_bEnableHWater;

	if (m_pcsAgcmPostFX)
	{
		//m_bBloom1		= m_pcsAgcmPostFX->isPipelineValid(AGCD_OPTION_BLOOM1_STRING);
		//m_bBloom2		= m_pcsAgcmPostFX->isPipelineValid(AGCD_OPTION_BLOOM2_STRING);
		m_bBloom1		=	TRUE;
		m_bBloom2		=	TRUE;

		m_bAutoExposure = m_bBloom1;
	}

	if (m_pcsAgcmSound)
	{
		m_l3DProvider = m_pcsAgcmSound->GetCurrentDeviceIndex();
	}

	// Video ó��
	UpdateAutoTuning(TRUE);
	UpdateSpeedTuning(TRUE);
	UpdateDisableOtherPlayerSkillEffect(TRUE);
	UpdateCharView(TRUE);
	UpdateMapView(TRUE);
	UpdateWideView(TRUE);
	UpdateNature(TRUE);
	UpdateShadow(TRUE);
	UpdateEffect(TRUE);
	UpdateWater(TRUE);
	UpdateAutoExposure(TRUE);
	UpdateBloom(TRUE);
	UpdateMatEffect(TRUE);
	UpdateAntiAliasing(TRUE);
	UpdateImpact(TRUE);
	UpdateBloomVal(TRUE);
// #ifdef _AREA_GLOBAL_
// 	UpdateMouseSpeedVal(TRUE);
// #endif
	UpdateBrightVal(TRUE);

	// Soundó��
	UpdateBGMSoundEnable(TRUE);
	UpdateBGMSoundVal(TRUE);
	UpdateEffSoundEnable(TRUE);
	UpdateEnvSoundEnable(TRUE);
	UpdateEnvSoundVal(TRUE);
	UpdateUseEAX(TRUE);
	UpdateSpeakerType(TRUE);
	Update3DProvider(TRUE);

	// ETC ó��
	UpdateViewNameMine(TRUE);
	UpdateViewNameMonster(TRUE);
	UpdateViewNameGuild(TRUE);
	UpdateViewNameParty(TRUE);
	UpdateViewNameOthers(TRUE);
	UpdateViewHelpBaloonTip(TRUE);
	UpdateViewHelpBaloonChat(TRUE);
	UpdateViewHelpUclie(TRUE);
	UpdateViewSelfBars(TRUE);
	UpdatePartyBuffIcon(TRUE);
	UpdateViewHelmet(TRUE);
	UpdateViewItem(TRUE);
}

void	AgcmUIOption::UpdateAutoTuning(BOOL	bRefresh)
{
	// �ϴ� ��Ȱ��ȭ
	m_bAutoTuning = FALSE;

	if(m_bAutoTuning)
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventIDAutoTuningEnable);
	}
	else 
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventIDAutoTuningDisable);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDAutoTuning);
	}
}

void	AgcmUIOption::UpdateSpeedTuning(BOOL	bRefresh)
{
	if(m_bSpeedTuning)
	{
		m_eCharView = AGCD_OPTION_VIEW_NEAR;
		m_eMapView = AGCD_OPTION_VIEW_NEAR;
		m_eNature = AGCD_OPTION_NATURE_NONE;
		m_eShadow = AGCD_OPTION_EFFECT_OFF;
		m_eEffect = AGCD_OPTION_EFFECT_OFF;
		m_eWater = AGCD_OPTION_EFFECT_OFF;
		m_eBloom = AGCD_OPTION_EFFECT2_OFF;
		m_eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
		m_eMatEffect = AGCD_OPTION_TOGGLE_OFF;
		m_eAntiAliasing = AGCD_OPTION_TOGGLE_OFF;
		m_eImpact = AGCD_OPTION_TOGGLE_OFF;
	}
	//else
	//{
	//	m_eCharView = AGCD_OPTION_VIEW_NORMAL;
	//	m_eMapView = AGCD_OPTION_VIEW_NORMAL;
	//	m_eNature = AGCD_OPTION_NATURE_MEDIUM;
	//	m_eShadow = AGCD_OPTION_EFFECT_MEDIUM;
	//	m_eEffect = AGCD_OPTION_EFFECT_MEDIUM;
	//	m_eWater = AGCD_OPTION_EFFECT_MEDIUM;
	//	m_eBloom = AGCD_OPTION_EFFECT2_TYPE1;
	//	m_eAutoExposure = AGCD_OPTION_TOGGLE_ON;
	//	m_eMatEffect = AGCD_OPTION_TOGGLE_ON;
	//	m_eAntiAliasing = AGCD_OPTION_TOGGLE_ON;
	//	m_eImpact = AGCD_OPTION_TOGGLE_ON;
	//}

	UpdateCharView(bRefresh);
	UpdateMapView(bRefresh);
	UpdateNature(bRefresh);
	UpdateShadow(bRefresh);
	UpdateEffect(bRefresh);
	UpdateWater(bRefresh);
	UpdateAutoExposure(bRefresh);
	UpdateBloom(bRefresh);
	UpdateMatEffect(bRefresh);
	UpdateAntiAliasing(bRefresh);
	UpdateImpact(bRefresh);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDSpeedTuning);
}

void AgcmUIOption::UpdateDisableOtherPlayerSkillEffect( BOOL bRefresh )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return;

	AgpdCharacter* ppdMyCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdMyCharacter ) return;

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDDisableOtherPlayerSkillEffect );
}

void	AgcmUIOption::UpdateCharView(BOOL	bRefresh)
{
	if(!m_bAutoTuning && m_pcsAgcmTuner && m_pcsAgcmTuner->m_pcmLODManager)
	{
		switch (m_eCharView)
		{
		case AGCD_OPTION_VIEW_NEAR:
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[1] = 0.25f;
			break;

		case AGCD_OPTION_VIEW_FAR:
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[1] = 0.875f;
			break;

		case AGCD_OPTION_VIEW_ALL:
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[1] = 999.0f;
			break;

		default:
			m_eCharView = AGCD_OPTION_VIEW_NORMAL;
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[1] = 0.45f;
		}

		if (m_bSpeedTuning && m_eCharView != AGCD_OPTION_VIEW_NEAR)
		{
			m_bSpeedTuning = FALSE;
			UpdateSpeedTuning(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDCharView->m_lSelectedIndex	= m_eCharView;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDCharView);
	}
}

void	AgcmUIOption::UpdateMapView(BOOL	bRefresh)
{
	if(!m_bAutoTuning && m_pcsAgcmTuner && m_pcsAgcmTuner->m_pcmLODManager && m_pcsAgcmMap)
	{
		/*
		// ������ (2005-04-22 ���� 11:49:29) : 
		�ʱ� ��..
		m_nNearRough	= 6;
		m_nNearDetail	= 2;
		m_nNormalRough	= 6;
		m_nNormalDetail	= 2;
		m_nFarRough		= 6;
		m_nFarDetail	= 4;
		m_nAllRough		= 6;
		m_nAllDetail	= 6;
		*/

		switch (m_eMapView)
		{
		case AGCD_OPTION_VIEW_NEAR:
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[0] = 0.5f;
			//m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_pcsAgcmMap->GetRoughLoadRadius(), 2 , 0 );
			m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_nNearRough, m_nNearDetail , 0 );
			m_pcsAgcmEventNature->m_bSkyFogDisable = FALSE;
			break;

		case AGCD_OPTION_VIEW_FAR:
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[0] = 1.3f;
			//m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_pcsAgcmMap->GetRoughLoadRadius(), m_pcsAgcmMap->GetRoughLoadRadius() - 2 , 2 );
			m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_nFarRough, m_nFarDetail , 2 );
			m_pcsAgcmEventNature->m_bSkyFogDisable = TRUE;
			break;

		case AGCD_OPTION_VIEW_ALL:
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[0] = 1.8f;
			//m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_pcsAgcmMap->GetRoughLoadRadius(), m_pcsAgcmMap->GetRoughLoadRadius() , 3 );
			m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_nAllRough, m_nAllDetail , 3 );
			m_pcsAgcmEventNature->m_bSkyFogDisable = TRUE;
			break;

		default:
			m_eMapView = AGCD_OPTION_VIEW_NORMAL;
			m_pcsAgcmTuner->m_pcmLODManager->m_fLODDistFactor[0] = 0.8f;
			//m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_pcsAgcmMap->GetRoughLoadRadius(), 2 , 1 );
			m_pcsAgcmMap->__SetAutoLoadRange(m_pcsAgcmMap->GetMapDataLoadRadius(), m_nNormalRough, m_nNormalDetail , 1 );
			m_pcsAgcmEventNature->m_bSkyFogDisable = TRUE;
			break;
		}

		if (m_bSpeedTuning && m_eMapView != AGCD_OPTION_VIEW_NEAR)
		{
			m_bSpeedTuning = FALSE;
			UpdateSpeedTuning(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDMapView->m_lSelectedIndex	= m_eMapView;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDMapView);
	}
}

void	AgcmUIOption::UpdateWideView(BOOL bRefresh)
{
	BOOL bWide = m_eWideView == AGCD_OPTION_TOGGLE_ON ? TRUE : FALSE;
	g_pEngine->SetWideScreen( bWide );

	if(bRefresh)
	{
		m_pstUDWideView->m_lSelectedIndex	= m_eWideView;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDWideView);
	}
}

void	AgcmUIOption::UpdateNature(BOOL	bRefresh)
{
	switch (m_eNature)
	{
	case AGCD_OPTION_NATURE_LOW:
		m_eNaturalObj = AGCD_OPTION_NATURE_LOW;
		m_eLensFlare = AGCD_OPTION_TOGGLE_ON;
		m_eFog = AGCD_OPTION_TOGGLE_ON;
		m_eSky = AGCD_OPTION_TOGGLE_OFF;
		break;

	case AGCD_OPTION_NATURE_MEDIUM:
		m_eNaturalObj = AGCD_OPTION_NATURE_MEDIUM;
		m_eLensFlare = AGCD_OPTION_TOGGLE_ON;
		m_eFog = AGCD_OPTION_TOGGLE_ON;
		m_eSky = AGCD_OPTION_TOGGLE_OFF;
		break;

	case AGCD_OPTION_NATURE_HIGH:
		m_eNaturalObj = AGCD_OPTION_NATURE_HIGH;
		m_eLensFlare = AGCD_OPTION_TOGGLE_ON;
		m_eFog = AGCD_OPTION_TOGGLE_ON;
		m_eSky = AGCD_OPTION_TOGGLE_ON;
		break;
	}

	if (m_bSpeedTuning && m_eNature != AGCD_OPTION_NATURE_NONE)
	{
		m_bSpeedTuning = FALSE;
		UpdateSpeedTuning(TRUE);
	}

	UpdateNaturalObj(TRUE);
	UpdateLensFlare(TRUE);
	UpdateFog(TRUE);
	UpdateSky(TRUE);

	if(bRefresh)
	{
		m_pstUDNature->m_lSelectedIndex	= m_eNature;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDNature);
	}
}

void	AgcmUIOption::UpdateNaturalObj(BOOL	bRefresh)
{
	if(m_pcsAgcmGrass)
	{
		switch (m_eNaturalObj)
		{
		case AGCD_OPTION_EFFECT_OFF:
			m_pcsAgcmGrass->SetGrassDetail(GRASS_DETAIL_OFF);
			break;

		case AGCD_OPTION_EFFECT_LOW:
			m_pcsAgcmGrass->SetGrassDetail(GRASS_DETAIL_LOW);
			break;

		case AGCD_OPTION_EFFECT_MEDIUM:
			m_pcsAgcmGrass->SetGrassDetail(GRASS_DETAIL_MEDIUM);
			break;

		case AGCD_OPTION_EFFECT_HIGH:
			m_pcsAgcmGrass->SetGrassDetail(GRASS_DETAIL_HIGH);
			break;
		}
	}

	if(bRefresh)
	{
		m_pstUDNaturalObj->m_lSelectedIndex	= m_eNaturalObj;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDNaturalObj);
	}
}

void	AgcmUIOption::UpdateLensFlare(BOOL	bRefresh)
{
	if(m_pcsAgcmLensFlare)
	{
		if (m_eLensFlare == AGCD_OPTION_TOGGLE_OFF)
		{
			m_pcsAgcmLensFlare->m_bActive = FALSE;
		}
		else
		{
			m_eLensFlare = AGCD_OPTION_TOGGLE_ON;
			m_pcsAgcmLensFlare->m_bActive = TRUE;
		}
	}

	if(bRefresh)
	{
		m_pstUDLensFlare->m_lSelectedIndex	= m_eLensFlare;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDLensFlare);
	}
}

void	AgcmUIOption::UpdateFog(BOOL	bRefresh)
{
	if(m_pcsAgcmEventNature)
	{
		if(m_eFog == AGCD_OPTION_TOGGLE_OFF)
		{
			m_pcsAgcmEventNature->EnableFog(FALSE);
		}
		else
		{
			m_eFog = AGCD_OPTION_TOGGLE_ON;
			m_pcsAgcmEventNature->EnableFog(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDFog->m_lSelectedIndex	= m_eFog;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDFog);
	}
}

void	AgcmUIOption::UpdateSky(BOOL	bRefresh)
{
	if(bRefresh)
	{
		m_pstUDSky->m_lSelectedIndex	= m_eSky;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDSky);
	}
}

void	AgcmUIOption::UpdateWater(BOOL	bRefresh)
{
	if(m_pcsAgcmWater)
	{
		switch (m_eWater)
		{
		case AGCD_OPTION_EFFECT_OFF:
			m_pcsAgcmWater->SetWaterDetail(WATER_DETAIL_OFF);
			break;

		case AGCD_OPTION_EFFECT_LOW:
			m_pcsAgcmWater->SetWaterDetail(WATER_DETAIL_LOW);
			break;

		case AGCD_OPTION_EFFECT_HIGH:
			m_pcsAgcmWater->SetWaterDetail(WATER_DETAIL_HIGH);
			break;

		default:
			m_eWater = AGCD_OPTION_EFFECT_MEDIUM;
			m_pcsAgcmWater->SetWaterDetail(WATER_DETAIL_MEDIUM);
			break;
		}

		if (m_bSpeedTuning && m_eWater != AGCD_OPTION_EFFECT_OFF)
		{
			m_bSpeedTuning = FALSE;
			UpdateSpeedTuning(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDWater->m_lSelectedIndex	= m_eWater;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDWater);
		m_pcsAgcmUIManager2->RefreshUserData(m_pstUDWater);
	}
}

void	AgcmUIOption::UpdateShadow(BOOL	bRefresh)
{
	switch (m_eShadow)
	{
	case AGCD_OPTION_EFFECT_OFF:
		m_eShadowChar = AGCD_OPTION_EFFECT_OFF;
		m_eShadowSelf = AGCD_OPTION_TOGGLE_OFF;
		m_eShadowObject = AGCD_OPTION_TOGGLE_OFF;
		break;

	case AGCD_OPTION_EFFECT_LOW:
		m_eShadowChar = AGCD_OPTION_EFFECT_LOW;
		m_eShadowSelf = AGCD_OPTION_TOGGLE_OFF;
		m_eShadowObject = AGCD_OPTION_TOGGLE_OFF;
		break;

	case AGCD_OPTION_EFFECT_MEDIUM:
		m_eShadowChar = AGCD_OPTION_EFFECT_MEDIUM;
		m_eShadowSelf = AGCD_OPTION_TOGGLE_OFF;
		m_eShadowObject = AGCD_OPTION_TOGGLE_ON;
		break;

	case AGCD_OPTION_EFFECT_HIGH:
		m_eShadowChar = AGCD_OPTION_EFFECT_HIGH;
		m_eShadowSelf = AGCD_OPTION_TOGGLE_ON;
		m_eShadowObject = AGCD_OPTION_TOGGLE_ON;
		break;
	}

	if (m_bSpeedTuning && m_eShadow != AGCD_OPTION_EFFECT_OFF)
	{
		m_bSpeedTuning = FALSE;
		UpdateSpeedTuning(TRUE);
	}

	UpdateShadowChar(TRUE);
	UpdateShadowSelf(TRUE);
	UpdateShadowObject(TRUE);

	if(bRefresh)
	{
		m_pstUDShadow->m_lSelectedIndex	= m_eShadow;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDShadow);
	}
}

void	AgcmUIOption::UpdateShadowChar(BOOL	bRefresh)
{
	if (m_pcsAgcmShadow)
	{
		switch (m_eShadowChar)
		{
		case AGCD_OPTION_EFFECT_OFF:
			m_pcsAgcmShadow->SetShadowEnable(FALSE);
			break;

		case AGCD_OPTION_EFFECT_LOW:
			m_pcsAgcmShadow->m_fShadowDistFactor = 0.4f;
			m_pcsAgcmShadow->m_iShadowDrawMaxNum = 3;
			m_pcsAgcmShadow->SetShadowEnable(TRUE);
			break;

		case AGCD_OPTION_EFFECT_MEDIUM:
			m_pcsAgcmShadow->m_iShadowDrawMaxNum = 10;
			m_pcsAgcmShadow->m_fShadowDistFactor = 1.0f;
			m_pcsAgcmShadow->SetShadowEnable(TRUE);
			break;

		case AGCD_OPTION_EFFECT_HIGH:
			m_pcsAgcmShadow->m_iShadowDrawMaxNum = 30;
			m_pcsAgcmShadow->m_fShadowDistFactor = 1.0f;
			m_pcsAgcmShadow->SetShadowEnable(TRUE);
			break;
		}
	}

	if(bRefresh)
	{
		m_pstUDShadowChar->m_lSelectedIndex	= m_eShadowChar;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDShadowChar);
	}
}

void	AgcmUIOption::UpdateShadowSelf(BOOL	bRefresh)
{
	if(m_pcsAgcmShadowMap)
	{
		if(m_eShadowSelf == AGCD_OPTION_TOGGLE_OFF)
		{
			m_pcsAgcmShadowMap->SetEnable(FALSE);
		}
		else
		{
			m_eShadowSelf = AGCD_OPTION_TOGGLE_ON;
			m_pcsAgcmShadowMap->SetEnable(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDShadowSelf->m_lSelectedIndex	= m_eShadowSelf;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDShadowSelf);
	}
}

void	AgcmUIOption::UpdateShadowObject(BOOL	bRefresh)
{
	if(m_pcsAgcmShadow2 && m_pcsAgcmRender)
	{
		if(m_eShadowObject == AGCD_OPTION_TOGGLE_OFF)
		{
			m_pcsAgcmShadow2->SetEnable(FALSE);
			m_pcsAgcmRender->m_bDrawShadow2 = FALSE;
		}
		else
		{
			m_eShadowObject = AGCD_OPTION_TOGGLE_ON;

			m_pcsAgcmShadow2->SetEnable(TRUE);
			m_pcsAgcmRender->m_bDrawShadow2 = TRUE;
		}
	}

	if(bRefresh)
	{
		m_pstUDShadowObject->m_lSelectedIndex	= m_eShadowObject;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDShadowObject);
	}
}

void	AgcmUIOption::UpdateEffect(BOOL	bRefresh)
{
	switch (m_eEffect)
	{
	case AGCD_OPTION_EFFECT_OFF:
		m_eEffectField	= AGCD_OPTION_EFFECT_OFF;
		m_eEffectCombat	= AGCD_OPTION_EFFECT_OFF;
		m_eEffectUI		= AGCD_OPTION_TOGGLE_OFF;
		m_eEffectSkill	= AGCD_OPTION_TOGGLE_OFF;
		break;

	case AGCD_OPTION_EFFECT_LOW:
		m_eEffectField	= AGCD_OPTION_EFFECT_LOW;
		m_eEffectCombat	= AGCD_OPTION_EFFECT_LOW;
		m_eEffectUI		= AGCD_OPTION_TOGGLE_OFF;
		m_eEffectSkill	= AGCD_OPTION_TOGGLE_ON;
		break;

	case AGCD_OPTION_EFFECT_MEDIUM:
		m_eEffectField	= AGCD_OPTION_EFFECT_MEDIUM;
		m_eEffectCombat	= AGCD_OPTION_EFFECT_MEDIUM;
		m_eEffectUI		= AGCD_OPTION_TOGGLE_ON;
		m_eEffectSkill	= AGCD_OPTION_TOGGLE_ON;
		break;

	case AGCD_OPTION_EFFECT_HIGH:
		m_eEffectField	= AGCD_OPTION_EFFECT_HIGH;
		m_eEffectCombat	= AGCD_OPTION_EFFECT_HIGH;
		m_eEffectUI		= AGCD_OPTION_TOGGLE_ON;
		m_eEffectSkill	= AGCD_OPTION_TOGGLE_ON;
		break;
	}

	if (m_bSpeedTuning && m_eEffect != AGCD_OPTION_EFFECT_OFF)
	{
		m_bSpeedTuning = FALSE;
		UpdateSpeedTuning(TRUE);
	}

	UpdateEffectField	(TRUE);
	UpdateEffectCombat	(TRUE);
	UpdateEffectUI		(TRUE);
	UpdateEffectSkill	(TRUE);
	
	if (bRefresh)
	{
		m_pstUDEffect->m_lSelectedIndex	= m_eEffect;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffect);
	}
}

void	AgcmUIOption::UpdateEffectField(BOOL	bRefresh)
{
	//if(m_pcsAgcmEff2)
	//{
	//	if(m_eEffectField == AGCD_OPTION_EFFECT_OFF)
	//	{
	//		m_pcsAgcmEff2->AgcmEff2::bEffOff();
	//	}
	//	else
	//	{
	//		m_pcsAgcmEff2->AgcmEff2::bEffOn();
	//	}
	//}	

	if(bRefresh)
	{
		m_pstUDEffectField->m_lSelectedIndex	= m_eEffectField;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffectField);
	}
}

void	AgcmUIOption::UpdateCriminal(BOOL	bRefresh)
{
	if (m_pcsAgcmMinimap)
	{
		m_pcsAgcmMinimap->m_bCriminal	=	m_bCriminal;
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDVisibleCriminal);
	}
}

void	AgcmUIOption::UpdateWanted(BOOL	bRefresh)
{
	if (m_pcsAgcmMinimap)
	{
		m_pcsAgcmMinimap->m_bEnemyGuild		=	m_bWanted;
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDVisibleWanted);
	}
}

void	AgcmUIOption::UpdateMurderer(BOOL	bRefresh)
{
	if (m_pcsAgcmMinimap)
	{
		m_pcsAgcmMinimap->m_bMurderer		=	m_bMurderer;
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDVisibleMurderer);
	}
}

void	AgcmUIOption::UpdateEffectCombat(BOOL	bRefresh)
{
	//if(m_pcsAgcmEff2)
	//{
	//	if(m_eEffectCombat == AGCD_OPTION_EFFECT_OFF)
	//	{
	//		m_pcsAgcmEff2->AgcmEff2::bEffOff();
	//	}
	//	else
	//	{
	//		m_pcsAgcmEff2->AgcmEff2::bEffOn();
	//	}
	//}	

	if(bRefresh)
	{
		m_pstUDEffectCombat->m_lSelectedIndex	= m_eEffectCombat;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffectCombat);
	}
}

void	AgcmUIOption::UpdateEffectUI(BOOL	bRefresh)
{
	if(bRefresh)
	{
		m_pstUDEffectUI->m_lSelectedIndex	= m_eEffectUI;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffectUI);
	}
}

void	AgcmUIOption::UpdateEffectSkill(BOOL	bRefresh)
{
	if(bRefresh)
	{
		m_pstUDEffectSkill->m_lSelectedIndex	= m_eEffectSkill;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffectSkill);

		if( m_eEffectSkill == AGCD_OPTION_TOGGLE_OFF )
		{
			m_bDisableSkillEffect = TRUE;
			m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDDisableSkillEffect );
			AddSystemMessageForSkillEffect( m_bDisableSkillEffect );
		}
		else if( m_eEffectSkill == AGCD_OPTION_TOGGLE_ON )
		{
			m_bDisableSkillEffect = FALSE;
			m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDDisableSkillEffect );
			AddSystemMessageForSkillEffect( m_bDisableSkillEffect );
		}
	}
}

void	AgcmUIOption::UpdateBloom(BOOL	bRefresh)
{
	if (m_pcsAgcmPostFX)
	{
		switch (m_eBloom)
		{
		case AGCD_OPTION_EFFECT2_OFF:
			if (!m_eAutoExposure)
			{
 				m_pcsAgcmPostFX->DeleteFX( "Light_Adapt" );
 				m_pcsAgcmPostFX->AutoExposureControlOff();

				//m_pcsAgcmPostFX->PostFX_OFF(m_eImpact == AGCD_OPTION_TOGGLE_ON);
				if( m_eImpact != AGCD_OPTION_TOGGLE_ON )
				{
					m_pcsAgcmPostFX->PostFX_OFF();
				}
				else
				{
					m_pcsAgcmPostFX->PostFX_ON();
				}
			}
			else
			{
				m_pcsAgcmPostFX->PostFX_ON();
			}

			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_DOWNSAMPLE"	);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLURH"			);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLURV"			);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLOOMCOMP"		);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BOXBLUR1"		);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BOXBLOOMCOMP"	);

			break;

		case AGCD_OPTION_EFFECT2_TYPE1:
			
			m_pcsAgcmPostFX->DeleteFX( "Light_Adapt" );

			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_DOWNSAMPLE"	);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLURH"			);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLURV"			);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLOOMCOMP"		);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BOXBLUR1"		);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BOXBLOOMCOMP"	);

			if(m_eAutoExposure)
			{
				//m_pcsAgcmPostFX->CreateFX( "Light_Adapt" , E_PIPELINE_LIGHTADAPT );
				m_pcsAgcmPostFX->AutoExposureControlOn();
			}
			else
			{
				m_pcsAgcmPostFX->AutoExposureControlOff();
			}

			m_pcsAgcmPostFX->CreateFX( "UIOPTION_BLOOMCOMP"		, E_PIPELINE_BLOOMCOMP	, TRUE);
			m_pcsAgcmPostFX->CreateFX( "UIOPTION_BLURV"			, E_PIPELINE_BLURV		, TRUE);
			m_pcsAgcmPostFX->CreateFX( "UIOPTION_BLURH"			, E_PIPELINE_BLURH		, TRUE);
			m_pcsAgcmPostFX->CreateFX( "UIOPTION_DOWNSAMPLE"	, E_PIPELINE_DOWNSAMPLE , TRUE);

			m_pcsAgcmPostFX->PostFX_ON();

			break;

		case AGCD_OPTION_EFFECT2_TYPE2:

			m_pcsAgcmPostFX->DeleteFX( "Light_Adapt" );

			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_DOWNSAMPLE"	);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLURH"			);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLURV"			);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BLOOMCOMP"		);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BOXBLUR1"		);
			m_pcsAgcmPostFX->DeleteFX( "UIOPTION_BOXBLOOMCOMP"	);

			if(m_eAutoExposure)
			{
				//m_pcsAgcmPostFX->CreateFX( "Light_Adapt" , E_PIPELINE_LIGHTADAPT );
				m_pcsAgcmPostFX->AutoExposureControlOn();
			}
			else
			{
				m_pcsAgcmPostFX->AutoExposureControlOff();
			}

			m_pcsAgcmPostFX->CreateFX( "UIOPTION_BOXBLOOMCOMP"	, E_PIPELINE_BOXBLOOMCOMP	, TRUE);
			m_pcsAgcmPostFX->CreateFX( "UIOPTION_BOXBLUR1"		, E_PIPELINE_BOXBLUR1		, TRUE);
			m_pcsAgcmPostFX->CreateFX( "UIOPTION_DOWNSAMPLE"	, E_PIPELINE_DOWNSAMPLE		, TRUE);
			
			m_pcsAgcmPostFX->PostFX_ON();

			break;
		}

		if (m_bSpeedTuning && m_eBloom != AGCD_OPTION_EFFECT2_OFF)
		{
			m_bSpeedTuning = FALSE;
			UpdateSpeedTuning(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDBloom->m_lSelectedIndex	= m_eBloom;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDBloom);
		m_pcsAgcmUIManager2->RefreshUserData(m_pstUDBloom);
	}
}

void	AgcmUIOption::UpdateAutoExposure(BOOL	bRefresh)
{
	if (m_pcsAgcmPostFX)
	{
		if (m_eAutoExposure == AGCD_OPTION_TOGGLE_OFF)
		{
			m_pcsAgcmPostFX->DeleteFX( "Light_Adapt" );
			m_pcsAgcmPostFX->AutoExposureControlOff();

			if (m_eBloom == AGCD_OPTION_EFFECT2_OFF)
			{
				if( m_eImpact == AGCD_OPTION_TOGGLE_OFF )
					m_pcsAgcmPostFX->PostFX_OFF();
			}
		}
		else
		{
			m_eAutoExposure = AGCD_OPTION_TOGGLE_ON;

			m_pcsAgcmPostFX->DeleteFX( "Light_Adapt" );

			//@{ Jaewon 20041122
			// if post fx is off, set the proper pipeline and turn the post fx on.
			if(m_pcsAgcmPostFX->IsFxOn() == FALSE)
			{
				m_pcsAgcmPostFX->CreateFX( "Light_Adapt" , E_PIPELINE_LIGHTADAPT );
				m_pcsAgcmPostFX->PostFX_ON();
			}
			m_pcsAgcmPostFX->AutoExposureControlOn();
		}

		if (m_bSpeedTuning && m_eAutoExposure != AGCD_OPTION_TOGGLE_OFF)
		{
			m_bSpeedTuning = FALSE;
			UpdateSpeedTuning(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDAutoExposure->m_lSelectedIndex	= m_eAutoExposure;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDAutoExposure);
		m_pcsAgcmUIManager2->RefreshUserData(m_pstUDAutoExposure);
	}
}

void	AgcmUIOption::UpdateMatEffect(BOOL	bRefresh)
{
	if (m_pcsAgcmRender)
	{
		if (m_eMatEffect == AGCD_OPTION_TOGGLE_OFF)
		{
			m_pcsAgcmRender->DisableMatD3DFx();
		}
		else
		{
			m_eMatEffect = AGCD_OPTION_TOGGLE_ON;

			m_pcsAgcmRender->EnableMatD3DFx();
		}

		m_pcsAgcmRender->AllAtomicFxCheck();

		if (m_bSpeedTuning && m_eMatEffect != AGCD_OPTION_TOGGLE_OFF)
		{
			m_bSpeedTuning = FALSE;
			UpdateSpeedTuning(TRUE);
		}
	}

	if(bRefresh)
	{
		m_pstUDMatEffect->m_lSelectedIndex	= m_eMatEffect;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDMatEffect);
	}
}

void	AgcmUIOption::UpdateAntiAliasing(BOOL	bRefresh)
{
	if (m_pcsAgcmRender)
	{
		if (m_eAntiAliasing == AGCD_OPTION_TOGGLE_OFF)
		{
			//m_pcsAgcmRender->SetMultiSampleLevel(1);

			//@{ Jaewon 20050907
			// ;)
			//RwD3D9ChangeMultiSamplingLevels(1);
			//@} Jaewon
		}
		else
		{
			m_eAntiAliasing = AGCD_OPTION_TOGGLE_ON;

			//m_pcsAgcmRender->SetMultiSampleLevel( RwD3D9EngineGetMaxMultiSamplingLevels() );
			//m_pcsAgcmRender->SetMultiSampleLevel( 4 );

			//@{ Jaewon 20050907
			// ;)
			//RwD3D9ChangeMultiSamplingLevels(6);
			//@} Jaewon
		}

		//RwD3D9ChangeMultiSamplingLevels( m_pcsAgcmRender->GetMultiSampleLevel() );
		//AgcmFPSManager::SetMultisampleLevel( m_pcsAgcmRender->GetMultiSampleLevel() );

		//@{ Jaewon 20050907
		// The render target of 'AgcmShadow2' have to be updated immediately.
		//if(m_pcsAgcmShadow2)
		//	m_pcsAgcmShadow2->m_iShadowTexUpdate = 1;
		//@} Jaewon

		//if (m_bSpeedTuning && m_eAntiAliasing != AGCD_OPTION_TOGGLE_OFF)
		//{
		//	m_bSpeedTuning = FALSE;
		//	UpdateSpeedTuning(TRUE);
		//}
	}

	if(bRefresh)
	{
		m_pstUDAntiAliasing->m_lSelectedIndex	= m_eAntiAliasing;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDAntiAliasing);
	}
}

void	AgcmUIOption::UpdateImpact(BOOL	bRefresh)
{
	if (m_pcsAgcmRender) {
		if (m_eImpact == AGCD_OPTION_TOGGLE_OFF) {
			// if necessary, destroy render targets.
			if(m_eBloom == AGCD_OPTION_EFFECT2_OFF && 
				m_eAutoExposure == AGCD_OPTION_TOGGLE_OFF && 
				m_pcsAgcmPostFX && m_pcsAgcmPostFX->isReady()) {
				//m_pcsAgcmPostFX->destroyRenderTargets();
			}
		}
		else {
			m_eImpact = AGCD_OPTION_TOGGLE_ON;
			// if necessary, create render targets.
			if (m_pcsAgcmPostFX && !m_pcsAgcmPostFX->isReady())
				m_pcsAgcmPostFX->createRenderTargets();
		}
		if (m_bSpeedTuning && m_eImpact != AGCD_OPTION_TOGGLE_OFF) {
			m_bSpeedTuning = FALSE;
			UpdateSpeedTuning(TRUE);
		}
	}
	if (bRefresh) {
		m_pstUDImpact->m_lSelectedIndex	= m_eImpact;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDImpact);
	}
}
// #ifdef _AREA_GLOBAL_
// void	AgcmUIOption::UpdateMouseSpeedVal(BOOL bRefresh)
// {
// 	INT32	lMouseSpeed = (20 * m_iMouseSpeedCurrent) / m_iMouseSpeedMax;
// 
// 	if (lMouseSpeed < 1)
// 		lMouseSpeed = 1;
// 	else if (lMouseSpeed > 20)
// 		lMouseSpeed = 20;
// 
// 	SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID) lMouseSpeed, 0);
// 
// 	if(bRefresh)
// 	{
// 		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDMouseSpeedMax);
// 		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDMouseSpeedCurrent);
// 	}
// 
// }
// #endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//. 2005. 09. 20. Nonstopdj
//. desc : brightness, contrast, gamma..IrrichtNX�� SetGammaRamp()���� ����.
//. brightness	:
//.	�� �״�� �̹��� ��ü�� ��⸦ ������ �� ���. 
//.	��� ������ ������ ���� ��Ⱚ�� ������ ����� ���� �ְų� ���ش�.
//. 
//.	contrast	:
//. �츮���� ��� �Ǵ� ������� ������, �̹����� ���� ������ ��ο� ������ ���̸� ������ �� ���.
//. ��, contrast�� ���δٴ� ���� ���� ������ �� ��� �ϰ� ��ο� ������ �� ��Ӱ� ���ִ� �����ν�, 
//. �����ϰ� ���� ��Ⱚ�� ������ ����� �����ִ� ������ ����.
//.
//. gamma		:
//. �̰͵� �̹����� ��⸦ ������ �� ���, ��Ⱚ�� ������ ���� ���� �ٸ� ũ��� ��⸦ �����ϴ� ���. 
//. ���� ���, ���� ������ ��ȭ�� �� �� ������ ���� ������ �״�� �ΰ� ��ο� ������ ��������� Ű���ָ� 
//. ������ �� ��������. ���������� ���� ����� �����մϴٸ�, �⺻�����δ� ���ϴ� ������ ��⸸�� ������ �� 
//. �ִ� ������ ���� �Լ��� ���� ������ ��Ⱚ�� ���������ν� ������ �� ����.
void	AgcmUIOption::UpdateBrightVal(BOOL	bRefresh)
{
	float	fval	= (float)m_iScreenBrightCurrent/(float)m_iScreenBrightMax;
	float	angle	= (float)m_iScreenContrastCurrent/(float)m_iScreenContrastMax; 
	float	offset	= ((float)m_iScreenBrightnessCurrent/(float)m_iScreenBrightnessMax  - 1) * 256; 
	
	if(m_bEnableGamma )
	{
		D3DGAMMARAMP		gamma;
		fval += 0.5f;		// 0.5f ~ 1.5f 
		float	t = 1.0f/fval;
		float	k;

		for(int i=0;i<256;++i)
		{
			k = (float)i / 255.0f;
			k = pow(k, t); 
			k *= 256; 
			//k *= 65535.0f;

			float value = k * angle * 256 + offset * 256;

			if (value > 65535) 
				value = 65535; 
			if (value < 0) 
				value = 0; 

			gamma.red[i]	= (WORD) value; 
			gamma.green[i]	= (WORD) value; 
			gamma.blue[i]	= (WORD) value; 
		}
		
		// ���� ����
		LPDIRECT3DDEVICE9	pDev = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
		pDev->SetGammaRamp(0,D3DSGR_CALIBRATE,&gamma);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDScreenBrightMax);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDScreenBrightCurrent);
	}
	/*
	float	fval = (float)m_iScreenBrightCurrent/(float)m_iScreenBrightMax;
	
	if(m_bEnableGamma )
	{
		fval += 0.5f;		// 0.5f ~ 1.5f 
		float	t = 1.0f/fval;
		D3DGAMMARAMP		gamma;
		float	s,r;
		
		for(int i=0;i<256;++i)
		{
			s = (float)i / 255.0f;
			r = pow(s,t);

			gamma.red[i] = (WORD)((float)65535.0f * r);
			gamma.green[i] = (WORD)((float)65535.0f * r);
			gamma.blue[i] = (WORD)((float)65535.0f * r);
		}
		
		// ���� ����
		LPDIRECT3DDEVICE9	pDev = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();
		pDev->SetGammaRamp (0,D3DSGR_CALIBRATE,&gamma);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDScreenBrightMax);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDScreenBrightCurrent);
	}
	*/
}

void	AgcmUIOption::UpdateBloomVal(BOOL	bRefresh)
{
	//@{ Jaewon 20050106
	// update the GlowIntensity parameter of the postfx.
	if(m_pcsAgcmPostFX && m_pcsAgcmPostFX->m_pd3dEffect)
	{
		m_pcsAgcmPostFX->m_pd3dEffect->SetFloat(m_pcsAgcmPostFX->m_pd3dEffect->GetParameterByName(NULL, "GlowIntensity"), 
										   //@{ Jaewon 20050116			
										   // 50.0f -> 100.0f
										   float(m_iBloomCurrent)/100.0f);
										   //@} Jaewon
	}
}

void	AgcmUIOption::UpdateBGMSoundEnable( BOOL bRefresh )
{
	if( m_pcsAgcmSound )
	{
		if( m_bBackgroundSound )
		{
			m_pcsAgcmSound->UnMuteSound( SoundType_BGM );

			// ���� ĳ���Ͱ� ��ġ�� ������ ��ī�̼����κ��� �÷����ؾ� �� BGM�� �����´�.
			AgcmEventNature* pcmEventNature = ( AgcmEventNature* )g_pEngine->GetModule( "AgcmEventNature" );
			if( pcmEventNature )
			{
				pcmEventNature->PlayBGM();
			}

			FLOAT fval = (FLOAT)m_iBackgroundSoundCurrent / (FLOAT)m_iBackgroundSoundMax;
			m_pcsAgcmSound->SetBGMVolume( fval );
		}
		else
		{
			m_pcsAgcmSound->MuteSound( SoundType_BGM );
		}
	}

	if( bRefresh )
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDBackgroundSound );
	}
}

void	AgcmUIOption::UpdateBGMSoundVal(BOOL	bRefresh)
{
	if (m_pcsAgcmSound)
	{
		FLOAT fval = (FLOAT)m_iBackgroundSoundCurrent / (FLOAT)m_iBackgroundSoundMax;
		if( m_bBackgroundSound )
		{
			m_pcsAgcmSound->SetBGMVolume( fval );
		}
		else
		{
			m_pcsAgcmSound->SetBGMVolumePrev( fval );
		}
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDBackgroundSoundMax);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDBackgroundSoundCurrent);
	}
}

void AgcmUIOption::UpdateEffSoundEnable(BOOL	bRefresh)
{
	if( m_pcsAgcmSound )
	{
		m_bEffectSound ? m_pcsAgcmSound->UnMuteSound( SoundType_Effect ) : m_pcsAgcmSound->MuteSound( SoundType_Effect );
		if( m_bEffectSound )
		{
			float fval = (FLOAT)m_iEffectSoundCurrent / (FLOAT)m_iEffectSoundMax;
			m_pcsAgcmSound->SetSampleVolume( fval );
			m_pcsAgcmSound->Set3DSampleVolume( fval );
		}
	}

	if( bRefresh )
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffectSound);
	}
}

void AgcmUIOption::UpdateEffSoundVal( BOOL bRefresh )
{
	if( m_pcsAgcmSound )
	{
		float fval = (FLOAT)m_iEffectSoundCurrent / (FLOAT)m_iEffectSoundMax;

		if( m_bEffectSound )
		{
			m_pcsAgcmSound->SetSampleVolume(fval);
			m_pcsAgcmSound->Set3DSampleVolume(fval);
		}
		else
		{
			m_pcsAgcmSound->SetSampleVolumePrev( fval );
			m_pcsAgcmSound->Set3DSampleVolumePrev( fval );
		}
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffectSoundMax);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEffectSoundCurrent);
	}
}

void	AgcmUIOption::UpdateEnvSoundEnable(BOOL	bRefresh)
{
	if( m_pcsAgcmSound )
	{
		if( m_bEnvSound )
		{
			m_pcsAgcmSound->UnMuteSound( SoundType_Environment );

			float fval = (FLOAT)m_iEnvSoundCurrent / (FLOAT)m_iEnvSoundMax;
			m_pcsAgcmSound->SetStreamVolume( fval );
		}
		else
		{
			m_pcsAgcmSound->MuteSound( SoundType_Environment );
		}
	}

	if( bRefresh )
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEnvSound);
	}
}

void	AgcmUIOption::UpdateEnvSoundVal(BOOL	bRefresh)
{
	if( m_pcsAgcmSound )
	{
		float fval = (FLOAT)m_iEnvSoundCurrent / (FLOAT)m_iEnvSoundMax;

		if( m_bEnvSound )
		{
			m_pcsAgcmSound->SetStreamVolume(fval);
		}
		else
		{
			m_pcsAgcmSound->SetStreamVolumePrev(fval);
		}
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEnvSoundMax);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDEnvSoundCurrent);
	}
}

void	AgcmUIOption::UpdateUseEAX(BOOL	bRefresh)
{
	if(m_pcsAgcmSound)
	{
		m_pcsAgcmSound->SetUseRoom(m_bUseEAX);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDUseEAX);
	}
}

void	AgcmUIOption::UpdateSpeakerType(BOOL	bRefresh)
{
	if(m_pcsAgcmSound)
	{
		m_pcsAgcmSound->SetSpeaker(m_eSpeakerType);
	}

	if(bRefresh)
	{
		m_pstUDSpeakerType->m_lSelectedIndex = m_eSpeakerType;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDSpeakerType);
	}
}

void	AgcmUIOption::Update3DProvider(BOOL	bRefresh)
{
	if(m_pcsAgcmSound)
	{
		m_pcsAgcmSound->OnSelectDevice(m_l3DProvider);
	}

	if(bRefresh)
	{
		m_pstUD3DProvider->m_lSelectedIndex = m_l3DProvider;
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUD3DProvider);
	}
}

void	AgcmUIOption::UpdateViewNameMine(BOOL	bRefresh)
{
	if (m_pcsAgcmTextBoardMng)
	{
		m_pcsAgcmTextBoardMng->SetDrawNameMine( m_bViewNameMine );
		m_pcsAgcmTextBoardMng->SetAllIDEnable(TB_MAINCHARACTER,m_bViewNameMine);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewNameMine);
	}
}

void	AgcmUIOption::UpdateViewNameMonster(BOOL	bRefresh)
{
	if (m_pcsAgcmTextBoardMng)
	{
		m_pcsAgcmTextBoardMng->SetDrawNameMonster( m_bViewNameMonster );
		m_pcsAgcmTextBoardMng->SetAllIDEnable(TB_MONSTER,m_bViewNameMonster);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewNameMonster);
	}
}

void	AgcmUIOption::UpdateViewNameGuild(BOOL	bRefresh)
{
	if (m_pcsAgcmTextBoardMng)
	{
		m_pcsAgcmTextBoardMng->SetDrawNameGuild	( m_bViewNameGuild	);
		m_pcsAgcmTextBoardMng->SetAllGuildEnable(m_bViewNameGuild	);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewNameGuild);
	}
}

void	AgcmUIOption::UpdateViewNameParty(BOOL	bRefresh)
{
	if (m_pcsAgcmTextBoardMng)
	{
		m_pcsAgcmTextBoardMng->SetDrawNameParty( m_bViewNameParty );
		m_pcsAgcmTextBoardMng->SetAllPartyEnable(m_bViewNameParty);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewNameParty);
	}
}

void	AgcmUIOption::UpdateViewNameOthers(BOOL	bRefresh)
{
	if (m_pcsAgcmTextBoardMng)
	{
		m_pcsAgcmTextBoardMng->SetDrawNameOther( m_bViewNameOthers );
		m_pcsAgcmTextBoardMng->SetAllIDEnable(TB_OTHERS,m_bViewNameOthers);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewNameOthers);
	}
}

void	AgcmUIOption::UpdateViewHelpBaloonTip(BOOL	bRefresh)
{
	AcUIToolTip::m_bViewToolTip = m_bViewHelpBaloonTip;

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewHelpBaloonTip);
	}
}

void	AgcmUIOption::UpdateViewHelpBaloonChat(BOOL	bRefresh)
{
	if (m_pcsAgcmTextBoardMng)
	{
		m_pcsAgcmTextBoardMng->SetMakeTextBaloon( m_bViewHelpBaloonChat );
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewHelpBaloonChat);
	}
}

void	AgcmUIOption::UpdateViewHelpUclie(BOOL	bRefresh)
{
	if (m_pcsAgcmUITips)
	{
		m_pcsAgcmUITips->EnableTipText(m_bViewHelpUclie);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDViewHelpUclie);
	}
}

void	AgcmUIOption::UpdateViewSelfBars(BOOL	bRefresh)
{
	if (m_pcsAgcmTextBoardMng)
	{
		m_pcsAgcmTextBoardMng->SetDrawHPMP( m_bViewSelfBars );
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(this->m_pstUDViewSelfBars);
	}
}

void	AgcmUIOption::UpdatePartyBuffIcon(BOOL	bRefresh)
{
	if (m_pcsAgcmUIParty)
	{
		m_pcsAgcmUIParty->SetVisibleBuffIcon(m_bVisiblePartyBuffIcon);
	}

	if(bRefresh)
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDVisiblePartyBuffIcon);
	}
}

void	AgcmUIOption::UpdateViewHelmet(BOOL	bRefresh)
{
	if (m_pcsAgcmUIStatus &&
		!m_pcsAgcmUIStatus->SendPacketUpdateViewHelmetOption(m_pcsAgcmCharacter->GetSelfCharacter(), (INT8) m_bVisibleViewHelmet))
		return;
	if(bRefresh)
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDVisibleViewHelmet);
}

void	AgcmUIOption::UpdateViewItem(BOOL bRefresh)
{
	if(bRefresh)
		m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstUDVisibleViewItem );
}

BOOL AgcmUIOption::CBLoginProcessEnd(PVOID pData,PVOID pClass,PVOID pCustData)
{
	AgcmUIOption*	pThis = (AgcmUIOption*) pClass;

	if (pThis->m_pcsAgcmUIManager2)
	{
		pThis->m_aszNameView[0] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_NEAR);
		pThis->m_aszNameView[1] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_NORMAL);
		pThis->m_aszNameView[2] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_FAR);
		pThis->m_aszNameView[3] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_ALL);

		pThis->m_aszNameToggle[0] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_OFF);
		pThis->m_aszNameToggle[1] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_ON);

		pThis->m_aszNameNature[0] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_LOW);
		pThis->m_aszNameNature[1] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_MEDIUM);
		pThis->m_aszNameNature[2] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_HIGH);

		pThis->m_aszNameEffect[0] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_OFF);
		pThis->m_aszNameEffect[1] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_LOW);
		pThis->m_aszNameEffect[2] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_MEDIUM);
		pThis->m_aszNameEffect[3] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_HIGH);

		pThis->m_aszNameEffect2[0] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_OFF);
		pThis->m_aszNameEffect2[1] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_TYPE1);
		pThis->m_aszNameEffect2[2] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_TYPE2);
		pThis->m_aszNameEffect2[3] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_TYPE3);
		pThis->m_aszNameEffect2[4] = pThis->m_pcsAgcmUIManager2->GetUIMessage(AGCD_OPTION_NAME_TYPE4);
	}
	if (pThis->m_bSave) {
		pThis->SaveToINI(PATCHCLIENT_OPTION_FILENAME);
	}
	else {
		pThis->LoadFromINI(PATCHCLIENT_OPTION_FILENAME);
		pThis->UpdateGameOption();
		pThis->m_bSave = TRUE;
	}
	return TRUE;
}

BOOL AgcmUIOption::CBAutoTuning(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	//pThis->m_bAutoTuning = !pThis->m_bAutoTuning;
	pThis->m_bAutoTuning = FALSE;
	
	// ó��
	pThis->UpdateAutoTuning(TRUE);

	return TRUE;
}

BOOL AgcmUIOption::CBSpeedTuning(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_bSpeedTuning = !pThis->m_bSpeedTuning;
	
	// ó��
	pThis->UpdateSpeedTuning(TRUE); 

	return TRUE;
}

BOOL	AgcmUIOption::CBCharView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eCharView = (AgcdUIOptionView) pThis->m_pstUDCharView->m_lSelectedIndex;
	pThis->UpdateCharView(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBMapView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eMapView = (AgcdUIOptionView) pThis->m_pstUDMapView->m_lSelectedIndex;
	pThis->UpdateMapView(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBWideView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eWideView = (AgcdUIOptionToggle) pThis->m_pstUDWideView->m_lSelectedIndex;
	pThis->UpdateWideView(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBNature(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		pThis->m_eNature = AGCD_OPTION_NATURE_NONE;
		pThis->UpdateNature(TRUE);
	}
	else
	{
		pThis->m_eNature = (AgcdUIOptionNature) pThis->m_pstUDNature->m_lSelectedIndex;
		pThis->UpdateNature(FALSE);
	}

	return TRUE;
}

BOOL	AgcmUIOption::CBShadow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		pThis->m_eShadow = AGCD_OPTION_EFFECT_NONE;
		pThis->UpdateShadow(TRUE);
	}
	else
	{
		pThis->m_eShadow = (AgcdUIOptionEffect) pThis->m_pstUDShadow->m_lSelectedIndex;
		pThis->UpdateShadow(FALSE);
	}

	return TRUE;
}

BOOL	AgcmUIOption::CBEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		pThis->m_eEffect = AGCD_OPTION_EFFECT_NONE;
		pThis->UpdateEffect(TRUE);
	}
	else
	{
		pThis->m_eEffect = (AgcdUIOptionEffect) pThis->m_pstUDEffect->m_lSelectedIndex;
		pThis->UpdateEffect(FALSE);
	}

	return TRUE;
}

BOOL	AgcmUIOption::CBWater(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eWater = (AgcdUIOptionEffect) pThis->m_pstUDWater->m_lSelectedIndex;
	pThis->UpdateWater(TRUE);

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_COMBO)
	{
		AcUICombo *	pcsCombo = (AcUICombo *) pcsSourceControl->m_pcsBase;

		if (!pThis->m_bWaterHigh)
			pcsCombo->EnableString(AGCD_OPTION_EFFECT_HIGH, FALSE);
	}

	return TRUE;
}

BOOL	AgcmUIOption::CBBloom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	int beforeBloom = pThis->m_eBloom;

	pThis->m_eBloom = (AgcdUIOptionEffect2) pThis->m_pstUDBloom->m_lSelectedIndex;

	if( pThis->m_eBloom == AGCD_OPTION_EFFECT2_OFF )
	{
		pThis->m_eAutoExposure = AGCD_OPTION_TOGGLE_OFF;
	}
	else
	{
		pThis->m_eAutoExposure = AGCD_OPTION_TOGGLE_ON;

		float val = 0.55f;
		pThis->m_iBloomCurrent = (INT32)((float)pThis->m_iBloomMax * val);

		pThis->UpdateBloomVal(FALSE);
	}

	pThis->UpdateAutoExposure(TRUE);

	pThis->UpdateBloom(TRUE);

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_COMBO)
	{
		AcUICombo *	pcsCombo = (AcUICombo *) pcsSourceControl->m_pcsBase;

		if (!pThis->m_bBloom1)
			pcsCombo->EnableString(AGCD_OPTION_EFFECT2_TYPE1, FALSE);

		if (!pThis->m_bBloom2)
			pcsCombo->EnableString(AGCD_OPTION_EFFECT2_TYPE2, FALSE);
	}

	return TRUE;
}

BOOL	AgcmUIOption::CBAutoExposure(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eAutoExposure = (AgcdUIOptionToggle) pThis->m_pstUDAutoExposure->m_lSelectedIndex;
	pThis->UpdateAutoExposure(TRUE);

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_COMBO)
	{
		AcUICombo *	pcsCombo = (AcUICombo *) pcsSourceControl->m_pcsBase;

		if (!pThis->m_bAutoExposure)
			pcsCombo->EnableString(AGCD_OPTION_TOGGLE_ON, FALSE);
	}

	return TRUE;
}

BOOL	AgcmUIOption::CBMatEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eMatEffect = (AgcdUIOptionToggle) pThis->m_pstUDMatEffect->m_lSelectedIndex;
	pThis->UpdateMatEffect(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBAntiAliasing(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eAntiAliasing = (AgcdUIOptionToggle) pThis->m_pstUDAntiAliasing->m_lSelectedIndex;
	pThis->UpdateAntiAliasing(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBImpact(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eImpact = (AgcdUIOptionToggle) pThis->m_pstUDImpact->m_lSelectedIndex;
	pThis->UpdateImpact(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBNaturalObj(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eNaturalObj = (AgcdUIOptionNature) pThis->m_pstUDNaturalObj->m_lSelectedIndex;
	pThis->UpdateNaturalObj(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBLensFlare(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eLensFlare = (AgcdUIOptionToggle) pThis->m_pstUDLensFlare->m_lSelectedIndex;
	pThis->UpdateLensFlare(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBFog(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eFog = (AgcdUIOptionToggle) pThis->m_pstUDFog->m_lSelectedIndex;
	pThis->UpdateFog(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBSky(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eSky = (AgcdUIOptionToggle) pThis->m_pstUDSky->m_lSelectedIndex;
	pThis->UpdateSky(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBShadowChar(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eShadowChar = (AgcdUIOptionEffect) pThis->m_pstUDShadowChar->m_lSelectedIndex;
	pThis->UpdateShadowChar(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBShadowSelf(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eShadowSelf = (AgcdUIOptionToggle) pThis->m_pstUDShadowSelf->m_lSelectedIndex;
	pThis->UpdateShadowSelf(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBShadowObject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eShadowObject = (AgcdUIOptionToggle) pThis->m_pstUDShadowObject->m_lSelectedIndex;
	pThis->UpdateShadowObject(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEffectField(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eEffectField = (AgcdUIOptionEffect) pThis->m_pstUDEffectField->m_lSelectedIndex;
	pThis->UpdateEffectField(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBCriminal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bCriminal = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateCriminal(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBWanted(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bWanted = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateWanted(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBMurderer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bMurderer = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateMurderer(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEffectCombat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eEffectCombat = (AgcdUIOptionEffect) pThis->m_pstUDEffectCombat->m_lSelectedIndex;
	pThis->UpdateEffectCombat(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEffectUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eEffectUI = (AgcdUIOptionToggle) pThis->m_pstUDEffectUI->m_lSelectedIndex;
	pThis->UpdateEffectUI(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEffectSkill(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eEffectSkill = (AgcdUIOptionToggle) pThis->m_pstUDEffectSkill->m_lSelectedIndex;
	pThis->UpdateEffectSkill(TRUE);

	return TRUE;
}

// #ifdef _AREA_GLOBAL_
// BOOL	AgcmUIOption::CBMouseSpeedVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
// {
// 	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;
// 
// 	float	val = ((AcUIScroll *) pcsSourceControl->m_pcsBase)->GetScrollValue();
// 	pThis->m_iMouseSpeedCurrent = (INT32)((float)pThis->m_iMouseSpeedMax * val);
// 
// 	pThis->UpdateMouseSpeedVal(FALSE);
// 
// 	return TRUE;
// }
// #endif
BOOL	AgcmUIOption::CBBrightVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	float	val = ((AcUIScroll *) pcsSourceControl->m_pcsBase)->GetScrollValue();
	pThis->m_iScreenBrightCurrent = (INT32)((float)pThis->m_iScreenBrightMax * val);

	pThis->UpdateBrightVal(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBBloomVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	float	val = ((AcUIScroll *) pcsSourceControl->m_pcsBase)->GetScrollValue();
	pThis->m_iBloomCurrent = (INT32)((float)pThis->m_iBloomMax * val);

	pThis->UpdateBloomVal(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBBackgroundSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bBackgroundSound = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateBGMSoundEnable(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBBackgroundSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	float	val = ((AcUIScroll *) pcsSourceControl->m_pcsBase)->GetScrollValue();
	pThis->m_iBackgroundSoundCurrent = (INT32)((float)pThis->m_iBackgroundSoundMax * val);
	
	pThis->UpdateBGMSoundVal(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEffectSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bEffectSound = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateEffSoundEnable(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEffectSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	float	val = ((AcUIScroll *) pcsSourceControl->m_pcsBase)->GetScrollValue();
	pThis->m_iEffectSoundCurrent = (INT32)((float)pThis->m_iEffectSoundMax * val);

	pThis->UpdateEffSoundVal(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEnvSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bEnvSound = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateEnvSoundEnable(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBEnvSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	float	val = ((AcUIScroll *) pcsSourceControl->m_pcsBase)->GetScrollValue();
	pThis->m_iEnvSoundCurrent = (INT32)((float)pThis->m_iEnvSoundMax * val);

	pThis->UpdateEnvSoundVal(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBUseEAX(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bUseEAX = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateUseEAX(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBSpeakerType(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_eSpeakerType = (eSoundSpeakerMode) pThis->m_pstUDSpeakerType->m_lSelectedIndex;
	pThis->UpdateSpeakerType(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CB3DProvider(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	pThis->m_l3DProvider = pThis->m_pstUD3DProvider->m_lSelectedIndex;
	pThis->Update3DProvider(TRUE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewNameMine(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewNameMine = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewNameMine(FALSE);
	
	return TRUE;
}

BOOL	AgcmUIOption::CBViewNameMonster(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewNameMonster = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewNameMonster(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewNameGuild(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewNameGuild = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewNameGuild(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewNameParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewNameParty = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewNameParty(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewNameOthers(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewNameOthers = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewNameOthers(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewHelpBaloonTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewHelpBaloonTip = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewHelpBaloonTip(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewHelpBaloonChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewHelpBaloonChat = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewHelpBaloonChat(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewHelpUclie(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewHelpUclie = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewHelpUclie(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::CBViewSelfBars(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bViewSelfBars = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);

	pThis->UpdateViewSelfBars(FALSE);

	return TRUE;
}

BOOL	AgcmUIOption::LoadFromINI(char*	szFile)
{
	if (!m_bSaveLoad)
		return TRUE;

	if (!szFile)
		return FALSE;

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	const CHAR		*szSectionName = NULL;
	CHAR			szValue[256];
	int				numKey;
	int				i,j;
	
	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if(!csStream.Open(szFile)) return FALSE;

	// szFile�� �д´�.
	//ASSERT(csStream.Open(szFile) && "Option INI������ ���� ���߽��ϴ�");

	numKey = csStream.GetNumSections();
	for(int index = 0;index<numKey;++index)
	{
		szSectionName = csStream.ReadSectionName(index);
		csStream.SetValueID(-1);

		// ��ȣ�� option 
		if(!strcmp(szSectionName,"Patch_Option"))
		{
			while(csStream.ReadNextValue())
			{
				szValueName = csStream.GetValueName();

				if(!strcmp(szValueName, "Width"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%d", &m_iScreenWidth);
				}
				else if(!strcmp(szValueName, "Height"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%d", &m_iScreenHeight);
				}
				else if(!strcmp(szValueName, "Bit"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%d", &m_iScreenDepth);
				}
				else if(!strcmp(szValueName, "Sound"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%s", &m_strSound);
				}
				else if(!strcmp(szValueName, "Speaker"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%s", &m_strSpeaker);
				}
				// Texture ǰ���� ���� �ڵ� (Character, Object, Effect, World) By Parn
				else if(!strcmp(szValueName, "TexCharacter"))
				{
					csStream.GetValue((INT32 *) &m_eTQCharacter);
				}
				else if(!strcmp(szValueName, "TexObject"))
				{
					csStream.GetValue((INT32 *) &m_eTQObject);
				}
				else if(!strcmp(szValueName, "TexEffect"))
				{
					csStream.GetValue((INT32 *) &m_eTQEffect);
				}
				else if(!strcmp(szValueName, "TexWorld"))
				{
					csStream.GetValue((INT32 *) &m_eTQWorld);
				}
				else if(!strcmp(szValueName, "OptionAutoInitialized"))
				{
					csStream.GetValue((INT32 *) &m_bAutoPatchInitialized);
				}
				else if(!strcmp(szValueName, "UISkinType"))
				{
					csStream.GetValue((INT32 *) &m_ulUISkinType);
					m_ulUISkinType = 1; //. 2006. 1. 9. Nonstopdj ui skin type fix..
				}
				else if(!strcmp(szValueName, "Windowed")) {
					csStream.GetValue((INT32 *) &m_bFramed);
				}
			}
		}
		else if(!strcmp(szSectionName,"Video_Option"))
		{
			while(csStream.ReadNextValue())
			{
				szValueName = csStream.GetValueName();

				if(!strcmp(szValueName, "AUTOTUNING"))
				{
					csStream.GetValue(&m_bAutoTuning);
				}
				else if(!strcmp(szValueName, "DisableOtherPlayerSkillEffect"))
				{
					csStream.GetValue(&m_bDisableOtherPlayerSkillEffect);
				}
				else if(!strcmp(szValueName, "CharView"))
				{
					csStream.GetValue((INT32 *) &m_eCharView);
				}
				else if(!strcmp(szValueName, "MapView"))
				{
					csStream.GetValue((INT32 *) &m_eMapView);
				}
				else if(!strcmp(szValueName, "WideView"))
				{
					csStream.GetValue((INT32 *) &m_eWideView);
				}
				else if(!strcmp(szValueName, "Nature"))
				{
					csStream.GetValue((INT32 *) &m_eNature);
				}
				else if(!strcmp(szValueName, "NaturalObj"))
				{
					csStream.GetValue((INT32 *) &m_eNaturalObj);
				}
				else if(!strcmp(szValueName, "LensFlare"))
				{
					csStream.GetValue((INT32 *) &m_eLensFlare);
				}
				else if(!strcmp(szValueName, "Fog"))
				{
					csStream.GetValue((INT32 *) &m_eFog);
				}
				else if(!strcmp(szValueName, "Sky"))
				{
					csStream.GetValue((INT32 *) &m_eSky);
				}
				else if(!strcmp(szValueName, "Shadow"))
				{
					csStream.GetValue((INT32 *) &m_eShadow);
				}
				else if(!strcmp(szValueName, "ShadowChar"))
				{
					csStream.GetValue((INT32 *) &m_eShadowChar);
				}
				else if(!strcmp(szValueName, "ShadowSelf"))
				{
					csStream.GetValue((INT32 *) &m_eShadowSelf);
				}
				else if(!strcmp(szValueName, "ShadowObject"))
				{
					csStream.GetValue((INT32 *) &m_eShadowObject);
				}
				else if(!strcmp(szValueName, "Effect"))
				{
					csStream.GetValue((INT32 *) &m_eEffect);
				}
				else if(!strcmp(szValueName, "EffectField"))
				{
					csStream.GetValue((INT32 *) &m_eEffectField);
				}
				else if(!strcmp(szValueName, "EffectCombat"))
				{
					csStream.GetValue((INT32 *) &m_eEffectCombat);
				}
				else if(!strcmp(szValueName, "EffectUI"))
				{
					csStream.GetValue((INT32 *) &m_eEffectUI);
				}
				else if(!strcmp(szValueName, "EffectSkill"))
				{
					csStream.GetValue((INT32 *) &m_eEffectSkill);
				}
				else if(!strcmp(szValueName, "Water"))
				{
					csStream.GetValue((INT32 *) &m_eWater);
				}
				else if(!strcmp(szValueName, "Bloom"))
				{
					csStream.GetValue((INT32 *) &m_eBloom);
				}
				else if(!strcmp(szValueName, "AutoExposure"))
				{
					csStream.GetValue((INT32 *) &m_eAutoExposure);
				}
				else if(!strcmp(szValueName, "MatEffect"))
				{
					csStream.GetValue((INT32 *) &m_eMatEffect);
				}
//#ifdef _AREA_GLOBAL_
//				else if(!strcmp(szValueName, "MOUSESPEED"))
//				{
//					csStream.GetValue(szValue, 256);

//					sscanf(szValue,"%d:%d",&i,&j);

//					m_iMouseSpeedMax = i;
//					m_iMouseSpeedCurrent = j;
//				}
//#endif
				else if(!strcmp(szValueName, "BRIGHT"))
				{
					csStream.GetValue(szValue, 256);

					sscanf(szValue,"%d:%d",&i,&j);

					m_iScreenBrightMax = i;
					m_iScreenBrightCurrent = j;
				}
				else if (!strcmp(szValueName, "Multisampling"))
				{
					csStream.GetValue((INT32 *) &m_eAntiAliasing);
				}
				else if(!strcmp(szValueName, "ImpactFX"))
				{
					csStream.GetValue((INT32 *) &m_eImpact);
				}
				else if(!strcmp(szValueName, "GlowIntensity"))
				{
					csStream.GetValue((INT32 *) &m_iBloomCurrent);
				}
			}
		}
		else if(!strcmp(szSectionName,"Sound_Option"))
		{
			while(csStream.ReadNextValue())
			{
				szValueName = csStream.GetValueName();

				if(!strcmp(szValueName, "BGM_ENABLE"))
				{
					csStream.GetValue(&m_bBackgroundSound);
					UpdateBGMSoundEnable( FALSE );
				}
				else if(!strcmp(szValueName, "BGM_VOLUME"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%d:%d", &i,&j ); 

					m_iBackgroundSoundMax = i;
					m_iBackgroundSoundCurrent = j;

					UpdateBGMSoundVal( FALSE );
				}
				else if(!strcmp(szValueName, "ES_ENABLE"))
				{
					csStream.GetValue(&m_bEffectSound);
					UpdateEffSoundEnable( FALSE );
				}
				else if(!strcmp(szValueName, "ES_VOLUME"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%d:%d", &i,&j ); 

					m_iEffectSoundMax = i;
					m_iEffectSoundCurrent = j;

					UpdateEffSoundVal( FALSE );
				}
				else if(!strcmp(szValueName, "ENV_ENABLE"))
				{
					csStream.GetValue(&m_bEnvSound);
					UpdateEnvSoundEnable( FALSE );
				}
				else if(!strcmp(szValueName, "ENV_VOLUME"))
				{
					csStream.GetValue(szValue, 256);
					sscanf(szValue, "%d:%d", &i,&j ); 

					m_iEnvSoundMax = i;
					m_iEnvSoundCurrent = j;

					UpdateEnvSoundVal( FALSE );
				}
				else if(!strcmp(szValueName, "SPEAKER_TYPE"))
				{
					csStream.GetValue((int *) &m_eSpeakerType);
				}
				else if(!strcmp(szValueName, "3DPROVIDER"))
				{
					csStream.GetValue(szValue, 256);

					if (m_pcsAgcmSound)
					{
						m_pcsAgcmSound->OnSelectDevice(szValue);
						m_l3DProvider = m_pcsAgcmSound->GetCurrentDeviceIndex();
					}
				}
				else if(!strcmp(szValueName, "EAX_ENABLE"))
				{
					csStream.GetValue(&m_bUseEAX);
				}
			}
		}
		else if(!strcmp(szSectionName,"ETC_Option"))
		{
			while(csStream.ReadNextValue())
			{
				szValueName = csStream.GetValueName();

				if(!strcmp(szValueName, "VN_MINE"))
				{
					csStream.GetValue(&m_bViewNameMine);
				}
				else if(!strcmp(szValueName, "VN_MONSTER"))
				{
					csStream.GetValue(&m_bViewNameMonster);
				}
				else if(!strcmp(szValueName, "VN_GUILD"))
				{
					csStream.GetValue(&m_bViewNameGuild);
				}
				else if(!strcmp(szValueName, "VN_PARTY"))
				{
					csStream.GetValue(&m_bViewNameParty);
				}
				else if(!strcmp(szValueName, "VN_OTHERS"))
				{
					csStream.GetValue(&m_bViewNameOthers);
				}
				else if(!strcmp(szValueName, "VH_TIP"))
				{
					csStream.GetValue(&m_bViewHelpBaloonTip);
				}
				else if(!strcmp(szValueName, "VH_BALOONCHAT"))
				{
					csStream.GetValue(&m_bViewHelpBaloonChat);
				}
				else if(!strcmp(szValueName, "VH_UCLIE"))
				{
					csStream.GetValue(&m_bViewHelpUclie);
				}
				else if(!strcmp(szValueName, "VS_BARS"))
				{
					csStream.GetValue(&m_bViewSelfBars);
				}
				else if(!strcmp(szValueName, "VS_PARTY"))
				{
					csStream.GetValue(&m_bVisiblePartyBuffIcon);
				}
				else if(!strcmp(szValueName, "VS_HELMET"))
				{
					csStream.GetValue(&m_bVisibleViewHelmet);
				}
				else if(!strcmp(szValueName, "VS_ITEM"))
				{
					csStream.GetValue(&m_bVisibleViewItem);
				}
				else if(!strcmp(szValueName, "VR_CRIMINAL"))
				{
					csStream.GetValue(&m_bCriminal);
				}
				else if(!strcmp(szValueName, "VS_WANTED"))
				{
					csStream.GetValue(&m_bWanted);
				}
				else if(!strcmp(szValueName, "VS_MURDERER"))
				{
					csStream.GetValue(&m_bMurderer);
				}
			}
		}
		else if(!strcmp(szSectionName,"Option_Option")) {
			while(csStream.ReadNextValue()) {
				szValueName = csStream.GetValueName();
				if(!strcmp(szValueName, "REF_TRADE"))
					csStream.GetValue(&m_bRefuseTrade);
				else if(!strcmp(szValueName, "REF_PIN"))
					csStream.GetValue(&m_bRefusePartyIn);
				else if(!strcmp(szValueName, "REF_GIN"))
					csStream.GetValue(&m_bRefuseGuildIn);
				else if(!strcmp(szValueName, "REF_GBAT"))
					csStream.GetValue(&m_bRefuseGuildBattle);
				else if(!strcmp(szValueName, "REF_BAT"))
					csStream.GetValue(&m_bRefuseBattle);
				else if(!strcmp(szValueName, "DIS_SKILL_EFF"))
					csStream.GetValue(&m_bDisableSkillEffect);
				else if(!strcmp(szValueName, "REF_GR"))
					csStream.GetValue(&m_bRefuseGuildRelation);
				else if(!strcmp(szValueName, "REF_BUDDY"))
					csStream.GetValue(&m_bRefuseBuddy);
				if (!m_bAdvOptionsSetup)
					continue;
				for (UINT32 i = 0; i < AGCDUIOPTION_LII_COUNT; i++) {
					AgcdUIOptionListItem * pstItem = &m_stAdvOptions[i];

					switch (pstItem->eType) {
					case AGCDUIOPTION_ADVANCED_OPTION_TYPE_BOOLEAN:
						if (pstItem->data.boolean.szINILabel[0] &&
							strcmp(pstItem->data.boolean.szINILabel,
							szValueName) == 0) {
							csStream.GetValue(&pstItem->data.boolean.bValue);
							m_pcsAgcmUIManager2->RefreshUserData(
								pstItem->data.boolean.pstUD, TRUE);
							if (pstItem->data.boolean.pfnCallback)
								pstItem->data.boolean.pfnCallback(this,
									pstItem, pstItem->data.boolean.pCBData);
							break;
						}
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL	AgcmUIOption::SaveToINI(char*		szFileName)
{
	if (!m_bSaveLoad)
		return TRUE;

	CHAR szValue[255];

	ApModuleStream csStream;
	if(!csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE))
		return FALSE;
	
	if(!csStream.SetSection("Patch_Option"))
		return FALSE;

	sprintf(szValue, "%d", m_iScreenWidth);
	if(!csStream.WriteValue("Width", szValue))		return FALSE;
	sprintf(szValue, "%d", m_iScreenHeight);
	if(!csStream.WriteValue("Height", szValue))		return FALSE;
	sprintf(szValue, "%d", m_iScreenDepth);
	if(!csStream.WriteValue("Bit", szValue))		return FALSE;
	sprintf(szValue, "%s", m_strSound);
	if(!csStream.WriteValue("Sound", szValue))		return FALSE;
	sprintf(szValue, "%s", m_strSpeaker);
	if(!csStream.WriteValue("Speaker", szValue))		return FALSE;

	// Texture ǰ���� ���� ���� (Character, Object, Effect, World)
	if(!csStream.WriteValue("TexCharacter", (INT32) m_eTQCharacter))	return FALSE;
	if(!csStream.WriteValue("TexObject", (INT32) m_eTQObject))	return FALSE;
	if(!csStream.WriteValue("TexEffect", (INT32) m_eTQEffect))	return FALSE;
	if(!csStream.WriteValue("TexWorld", (INT32) m_eTQWorld))	return FALSE;
	if(!csStream.WriteValue("OptionAutoInitialized", (INT32) m_bAutoPatchInitialized))	return FALSE;
	if(!csStream.WriteValue("UISkinType", (INT32) m_ulUISkinType))	return FALSE;
	if(!csStream.WriteValue("Windowed", (INT32) m_bFramed))	return FALSE;

	// ���� ����
	if(!csStream.SetSection("Video_Option"))
		return FALSE;

	if(!csStream.WriteValue("AUTOTUNING", m_bAutoTuning))		return FALSE;
	if(!csStream.WriteValue("DisableOtherPlayerSkillEffect", m_bDisableOtherPlayerSkillEffect)) return FALSE;

	if (!csStream.WriteValue("CharView", (INT32) m_eCharView))	return FALSE;
	if (!csStream.WriteValue("MapView", (INT32) m_eMapView))	return FALSE;
	if (!csStream.WriteValue("WideView", (INT32) m_eWideView))	return FALSE;
	if (!csStream.WriteValue("Nature", (INT32) m_eNature))	return FALSE;
	if (!csStream.WriteValue("Shadow", (INT32) m_eShadow))	return FALSE;
	if (!csStream.WriteValue("Effect", (INT32) m_eEffect))	return FALSE;
	if (!csStream.WriteValue("Water", (INT32) m_eWater))	return FALSE;
	if (!csStream.WriteValue("Bloom", (INT32) m_eBloom))	return FALSE;
	if (!csStream.WriteValue("AutoExposure", (INT32) m_eAutoExposure))	return FALSE;
	if (!csStream.WriteValue("MatEffect", (INT32) m_eMatEffect))	return FALSE;
	//@{ Jaewon 20050106
	if (!csStream.WriteValue("Multisampling", (INT32) m_eAntiAliasing)) return FALSE;
	if (!csStream.WriteValue("ImpactFX", (INT32) m_eImpact)) return FALSE;
	if (!csStream.WriteValue("GlowIntensity", (INT32) m_iBloomCurrent)) return FALSE;
	//@} Jaewon

	if (!csStream.WriteValue("NaturalObj", (INT32) m_eNaturalObj))	return FALSE;
	if (!csStream.WriteValue("LensFlare", (INT32) m_eLensFlare))	return FALSE;
	if (!csStream.WriteValue("Fog", (INT32) m_eFog))	return FALSE;
	if (!csStream.WriteValue("Sky", (INT32) m_eSky))	return FALSE;

	if (!csStream.WriteValue("ShadowChar", (INT32) m_eShadowChar))	return FALSE;
	if (!csStream.WriteValue("ShadowSelf", (INT32) m_eShadowSelf))	return FALSE;
	if (!csStream.WriteValue("ShadowObject", (INT32) m_eShadowObject))	return FALSE;

	if (!csStream.WriteValue("EffectField", (INT32) m_eEffectField))	return FALSE;
	if (!csStream.WriteValue("EffectCombat", (INT32) m_eEffectCombat))	return FALSE;
	if (!csStream.WriteValue("EffectUI", (INT32) m_eEffectUI))	return FALSE;
	if (!csStream.WriteValue("EffectSkill", (INT32) m_eEffectSkill))	return FALSE;
// #ifdef _AREA_GLOBAL_
// 	sprintf(szValue, "%d:%d", m_iMouseSpeedMax,m_iMouseSpeedCurrent);
// 	if(!csStream.WriteValue("MOUSESPEED", szValue))		return FALSE;
// #endif
	sprintf(szValue, "%d:%d", m_iScreenBrightMax,m_iScreenBrightCurrent);
	if(!csStream.WriteValue("BRIGHT", szValue))		return FALSE;

	// ���� ����
	if(!csStream.SetSection("Sound_Option"))		return FALSE;

	if(!csStream.WriteValue("BGM_ENABLE", m_bBackgroundSound))		return FALSE;
	sprintf(szValue, "%d:%d", m_iBackgroundSoundMax,m_iBackgroundSoundCurrent);
	if(!csStream.WriteValue("BGM_VOLUME", szValue))		return FALSE;

	if(!csStream.WriteValue("ES_ENABLE", m_bEffectSound))		return FALSE;
	sprintf(szValue, "%d:%d", m_iEffectSoundMax,m_iEffectSoundCurrent);
	if(!csStream.WriteValue("ES_VOLUME", szValue))		return FALSE;

	if(!csStream.WriteValue("ENV_ENABLE", m_bEnvSound))		return FALSE;
	sprintf(szValue, "%d:%d", m_iEnvSoundMax,m_iEnvSoundCurrent);
	if(!csStream.WriteValue("ENV_VOLUME", szValue))		return FALSE;

	if(!csStream.WriteValue("SPEAKER_TYPE", (eSoundSpeakerMode) m_eSpeakerType))	return FALSE;

	if (m_pcsAgcmSound)
	{
		CHAR *	szProvider = (CHAR *) m_pcsAgcmSound->GetCurrentDeviceName();

		if (szProvider && !csStream.WriteValue("3DPROVIDER", szProvider))	return FALSE;
	}

	if(!csStream.WriteValue("EAX_ENABLE", m_bUseEAX))	return FALSE;

	// ��Ÿ ����
	if(!csStream.SetSection("ETC_Option"))		return FALSE;

	if(!csStream.WriteValue("VN_MINE", m_bViewNameMine))			return FALSE;
	if(!csStream.WriteValue("VN_MONSTER", m_bViewNameMonster))		return FALSE;
	if(!csStream.WriteValue("VN_GUILD", m_bViewNameGuild))			return FALSE;
	if(!csStream.WriteValue("VN_PARTY", m_bViewNameParty))			return FALSE;
	if(!csStream.WriteValue("VN_OTHERS", m_bViewNameOthers))		return FALSE;
	if(!csStream.WriteValue("VH_TIP", m_bViewHelpBaloonTip))		return FALSE;
	if(!csStream.WriteValue("VH_BALOONCHAT", m_bViewHelpBaloonChat))return FALSE;
	if(!csStream.WriteValue("VH_UCLIE", m_bViewHelpUclie))			return FALSE;
	if(!csStream.WriteValue("VS_BARS", m_bViewSelfBars))			return FALSE;
	if(!csStream.WriteValue("VS_PARTY", m_bVisiblePartyBuffIcon))	return FALSE;
	if(!csStream.WriteValue("VS_HELMET", m_bVisibleViewHelmet))		return FALSE;
	if(!csStream.WriteValue("VS_ITEM", m_bVisibleViewItem))			return FALSE;
	if(!csStream.WriteValue("VR_CRIMINAL", m_bCriminal))			return FALSE;
	if(!csStream.WriteValue("VR_WANTED", m_bWanted))				return FALSE;
	if(!csStream.WriteValue("VS_MURDERER", m_bMurderer))			return FALSE;


//	2005.06.02	By SungHoon �ɼ�
	if(!csStream.SetSection("Option_Option"))		return FALSE;

	if(!csStream.WriteValue("REF_TRADE", m_bRefuseTrade))		return FALSE;
	if(!csStream.WriteValue("REF_PIN", m_bRefusePartyIn))		return FALSE;
	if(!csStream.WriteValue("REF_GIN", m_bRefuseGuildIn))		return FALSE;
	if(!csStream.WriteValue("REF_GBAT", m_bRefuseGuildBattle))	return FALSE;
	if(!csStream.WriteValue("REF_BAT", m_bRefuseBattle))		return FALSE;
	if(!csStream.WriteValue("DIS_SKILL_EFF", m_bDisableSkillEffect))	return FALSE;

	// 2007.07.12. steeple
	if(!csStream.WriteValue("REF_GR", m_bRefuseGuildRelation))	return FALSE;
	if(!csStream.WriteValue("REF_BUDDY", m_bRefuseBuddy))		return FALSE;
	if (m_bAdvOptionsSetup) {
		/*
		 * Write advanced options.
		 *
		 * We should write these in a new section but 
		 * current launcher cannot load unknown sections 
		 * so advanced options are temporarily written in 
		 * [Option_Option] section.
		 */
		for (UINT32 i = 0; i < AGCDUIOPTION_LII_COUNT; i++) {
			AgcdUIOptionListItem * pstItem = &m_stAdvOptions[i];

			switch (pstItem->eType) {
			case AGCDUIOPTION_ADVANCED_OPTION_TYPE_BOOLEAN:
				if (pstItem->data.boolean.szINILabel[0] &&
					!csStream.WriteValue(pstItem->data.boolean.szINILabel,
					pstItem->data.boolean.bValue))
					return FALSE;
				break;
			}
		}
	}
	return csStream.Write(szFileName);
}

/*
	2005.06.02. By SungHoon
	�ŷ��ź� ��ư�� ������ �� �Ҹ����� CallBack
*/
BOOL AgcmUIOption::CBRefuseTrade(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return FALSE;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		BOOL bClick = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
		pThis->SendOptionFlag(AGPDCHAR_OPTION_REFUSE_TRADE, bClick);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDRefuseTrade);
	}
	return FALSE;
}

/*
	2005.06.02. By SungHoon
	��Ƽ�ź� ��ư�� ������ �� �Ҹ����� CallBack
*/
BOOL AgcmUIOption::CBRefusePartyIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		BOOL bClick = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
		pThis->SendOptionFlag(AGPDCHAR_OPTION_REFUSE_PARTY_IN, bClick);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDRefusePartyIn);
	}
	return FALSE;
}

/*
	2005.06.02. By SungHoon
	����ʴ�ź� ��ư�� ������ �� �Ҹ����� CallBack
*/
BOOL AgcmUIOption::CBRefuseGuildIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		BOOL bClick = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
		pThis->SendOptionFlag(AGPDCHAR_OPTION_REFUSE_GUILD_IN, bClick);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDRefuseGuildIn);
	}
	return FALSE;
}

/*
	2005.06.02. By SungHoon
	������ź� ��ư�� ������ �� �Ҹ����� CallBack
*/
BOOL AgcmUIOption::CBRefuseGuildBattle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		BOOL bClick = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
		pThis->SendOptionFlag(AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE, bClick);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDRefuseGuildBattle);
	}
	return FALSE;
}

/*
	2005.06.02. By SungHoon
	�����ź� ��ư�� ������ �� �Ҹ����� CallBack
*/
BOOL AgcmUIOption::CBRefuseBattle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		BOOL bClick = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
		pThis->SendOptionFlag(AGPDCHAR_OPTION_REFUSE_BATTLE, bClick);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDRefuseBattle);
	}

	return FALSE;
}

//@{ 2007/01/22 burumal
//	 ��ų ����Ʈ ����� üũ�ڽ� �������� ȣ���
BOOL AgcmUIOption::CBDisableSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption*	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		pThis->m_bDisableSkillEffect = !pThis->m_bDisableSkillEffect;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDDisableSkillEffect);
		pThis->AddSystemMessageForSkillEffect( pThis->m_bDisableSkillEffect );
	}

	return FALSE;
}
//@}

// 2007.07.12. steeple
BOOL AgcmUIOption::CBRefuseGuildRelation(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption*	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		BOOL bClick = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
		pThis->SendOptionFlag(AGPDCHAR_OPTION_REFUSE_GUILD_RELATION, bClick);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDRefuseGuildRelation);
	}

	return FALSE;
}

BOOL AgcmUIOption::CBRefuseBuddy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption*	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
	{
		BOOL bClick = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
		pThis->SendOptionFlag(AGPDCHAR_OPTION_REFUSE_BUDDY, bClick);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDRefuseBuddy);
	}

	return FALSE;
}

/*
	2005.06.16. By SungHoon
	��Ƽ�� ��ų ���ǥ�� ��ư�� ������ ���
*/
BOOL AgcmUIOption::CBVisiblePartyBuffIcon(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bVisiblePartyBuffIcon = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
	pThis->UpdatePartyBuffIcon(FALSE);

	return FALSE;
}

BOOL AgcmUIOption::CBVisibleViewHelmet(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bVisibleViewHelmet = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
	pThis->UpdateViewHelmet(FALSE);

	return FALSE;
}

BOOL AgcmUIOption::CBVisibleViewItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *) pClass;

	if (pcsSourceControl && pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pThis->m_bVisibleViewItem = (((AcUIButton *) pcsSourceControl->m_pcsBase)->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
	pThis->UpdateViewItem(FALSE);

	return FALSE;
}

/*
	2005.06.02 By SungHoon
	�ɼ� ������ �Ͽ�ȭ �ϱ� ����
*/
VOID AgcmUIOption::SendOptionFlag(INT32 lOptionFlag, BOOL bClick)
{
	AgpdCharacter *pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter) return;

	if (bClick)
		m_pcsAgcmCharacter->SendPacketOptionFlag(pcsSelfCharacter, 
			m_pcsAgpmCharacter->SetOptionFlag(pcsSelfCharacter, (AgpdCharacterOptionFlag)lOptionFlag));
	else
		m_pcsAgcmCharacter->SendPacketOptionFlag(pcsSelfCharacter,
			m_pcsAgpmCharacter->UnsetOptionFlag(pcsSelfCharacter, (AgpdCharacterOptionFlag)lOptionFlag));

}

BOOL	AgcmUIOption::CBDisplay3DProvider(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIOption *		pThis = (AgcmUIOption *) pClass;
	stSoundDeviceEntry* pDevice = (stSoundDeviceEntry *) pData;

	sprintf(szDisplay, "%s", pDevice->m_strDeviceName);

	return TRUE;
}

BOOL
AgcmUIOption::CBDisplayAdvOptCategory(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption * pThis = (AgcmUIOption *)pClass;
	INT32 lIndex = *(INT32 *)pData;
	const AgcdUIOptionListItem * list_item;

	if (!pThis || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;
	if (lIndex >= AGCDUIOPTION_LII_COUNT)
		return FALSE;
	list_item = &pThis->m_stAdvOptions[lIndex];
	switch (list_item->eType) {
	case AGCDUIOPTION_ADVANCED_OPTION_TYPE_CATEGORY:
		strcpy(szDisplay, pThis->m_stAdvOptions[lIndex].szLabel);
		pcsSourceControl->m_pcsBase->ShowWindow(TRUE);
		break;
	default:
		pcsSourceControl->m_pcsBase->ShowWindow(FALSE);
		break;
	}
	return TRUE;
}

BOOL
AgcmUIOption::CBDisplayAdvOptName(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption * pThis = (AgcmUIOption *)pClass;
	INT32 lIndex = *(INT32 *)pData;
	const AgcdUIOptionListItem * list_item;

	if (!pThis || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;
	if (lIndex >= AGCDUIOPTION_LII_COUNT)
		return FALSE;
	list_item = &pThis->m_stAdvOptions[lIndex];
	switch (list_item->eType) {
	case AGCDUIOPTION_ADVANCED_OPTION_TYPE_BOOLEAN:
		strcpy(szDisplay, pThis->m_stAdvOptions[lIndex].szLabel);
		pcsSourceControl->m_pcsBase->ShowWindow(TRUE);
		break;
	default:
		pcsSourceControl->m_pcsBase->ShowWindow(FALSE);
		break;
	}
	return TRUE;
}

BOOL
AgcmUIOption::CBDisplayAdvOptCheckbox(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption * pThis = (AgcmUIOption *)pClass;
	INT32 lIndex = *(INT32 *)pData;
	const AgcdUIOptionListItem * list_item;

	if (!pThis || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;
	if (lIndex >= AGCDUIOPTION_LII_COUNT)
		return FALSE;
	list_item = &pThis->m_stAdvOptions[lIndex];
	if (list_item->eType != AGCDUIOPTION_ADVANCED_OPTION_TYPE_BOOLEAN)
		pcsSourceControl->m_pcsBase->ShowWindow(FALSE);
	else
		pcsSourceControl->m_pcsBase->ShowWindow(TRUE);
	return TRUE;
}

BOOL
AgcmUIOption::CBAdvOpCheckboxChanged(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption * pThis = (AgcmUIOption *)pClass;
	AgcdUIOptionListItem * pstListItem = 
		&pThis->m_stAdvOptions[pcsSourceControl->m_lUserDataIndex];
	AcUIButton * pcsBtn = (AcUIButton *)pcsSourceControl->m_pcsBase;
	BOOL bVal;

	bVal = (pcsBtn-> GetButtonMode() == ACUIBUTTON_MODE_CLICK);
	if (pcsSourceControl &&
		pcsSourceControl->m_lType == AcUIBase::TYPE_BUTTON)
		pstListItem->data.boolean.bValue = bVal;
	if (pstListItem->data.boolean.pfnCallback)
		pstListItem->data.boolean.pfnCallback(pThis, pstListItem, 
			pstListItem->data.boolean.pCBData);
	//pThis->UpdateViewNameMine(FALSE);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(
		pstListItem->data.boolean.pstUD);
	pThis->m_bSave = TRUE;
	return TRUE;
}

BOOL
AgcmUIOption::CBAdvOpCheckboxDefault(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption * pThis = (AgcmUIOption *)pClass;
	AgcdUIOptionListItem * pstItem = 
		&pThis->m_stAdvOptions[pcsSourceControl->m_lUserDataIndex];

	return pstItem->data.boolean.bValue;
}

BOOL
AgcmUIOption::CBDisplayItemDropPreview(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *)pClass;
	UINT32			ulColor;
	char			szEdit[128];
	
	ulColor = pThis->m_pcsAgcmItem->GetItemDropColor(
		pThis->m_eItemDropCategory);
	if (ulColor == 0)
		ulColor = 0x00FFFB96;
	sprintf_s(szEdit, sizeof(szEdit), "<C%d>[Item Drop Color Preview]", 
		ulColor & 0x00FFFFFF);
	((AcUIEdit *)pcsSourceControl->m_pcsBase)->SetText(szEdit);
	return TRUE;
}

BOOL
AgcmUIOption::CBChangedItemDropCategory(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption *			pThis = (AgcmUIOption *)pClass;
	AgcmUIManager2 *		pcsAgcmUIManager = pThis->m_pcsAgcmUIManager2;
	INT32					lSelected;
	AgcdItemDropCategoryId	id;
	UINT32					ulColor;
	
	lSelected = pThis->m_pstItemDropCategoryUD->m_lSelectedIndex;
	if (lSelected < 0 || lSelected >= AGCD_ITEM_DROP_CATEGORYID_COUNT)
		return FALSE;
	id = (AgcdItemDropCategoryId)lSelected;
	pThis->m_eItemDropCategory = id;
	ulColor = pThis->m_pcsAgcmItem->GetItemDropColor(id);
	pThis->m_ulItemDropSliderValue[0] = (ulColor >> 16) & 0xFF;
	pThis->m_ulItemDropSliderValue[1] = (ulColor >> 8) & 0xFF;
	pThis->m_ulItemDropSliderValue[2] = ulColor & 0xFF;
	pcsAgcmUIManager->RefreshUserData(pThis->m_pastItemDropSliderUD[0], TRUE);
	pcsAgcmUIManager->RefreshUserData(pThis->m_pastItemDropSliderUD[1], TRUE);
	pcsAgcmUIManager->RefreshUserData(pThis->m_pastItemDropSliderUD[2], TRUE);
	pcsAgcmUIManager->RefreshUserData(pThis->m_pstItemDropPreviewUD, TRUE);
	return TRUE;
}

BOOL
AgcmUIOption::CBChangedItemDropSliderR(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *)pClass;
	float			fVal = 
		((AcUIScroll *)pcsSourceControl->m_pcsBase)->GetScrollValue();
	UINT32			ulVal = (UINT32)(fVal * 255.f);
	UINT32			ulColor;

	if (ulVal > 255)
		ulVal = 255;
	pThis->m_ulItemDropSliderValue[0] = ulVal;
	ulColor = pThis->m_pcsAgcmItem->GetItemDropColor(
		pThis->m_eItemDropCategory);
	ulColor = (ulColor & 0xFF00FFFF) | (ulVal << 16);
	pThis->m_pcsAgcmItem->SetItemDropColor(pThis->m_eItemDropCategory, 
		ulColor);
	pThis->m_pcsAgcmUIManager2->RefreshUserData(
		pThis->m_pstItemDropPreviewUD, TRUE);
	return TRUE;
}

BOOL
AgcmUIOption::CBChangedItemDropSliderG(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *)pClass;
	float			fVal = 
		((AcUIScroll *)pcsSourceControl->m_pcsBase)->GetScrollValue();
	UINT32			ulVal = (UINT32)(fVal * 255.f);
	UINT32			ulColor;

	if (ulVal > 255)
		ulVal = 255;
	pThis->m_ulItemDropSliderValue[1] = ulVal;
	ulColor = pThis->m_pcsAgcmItem->GetItemDropColor(
		pThis->m_eItemDropCategory);
	ulColor = (ulColor & 0xFFFF00FF) | (ulVal << 8);
	pThis->m_pcsAgcmItem->SetItemDropColor(pThis->m_eItemDropCategory, 
		ulColor);
	pThis->m_pcsAgcmUIManager2->RefreshUserData(
		pThis->m_pstItemDropPreviewUD, TRUE);
	return TRUE;
}

BOOL
AgcmUIOption::CBChangedItemDropSliderB(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *)pClass;
	float			fVal = 
		((AcUIScroll *)pcsSourceControl->m_pcsBase)->GetScrollValue();
	UINT32			ulVal = (UINT32)(fVal * 255.f);
	UINT32			ulColor;

	if (ulVal > 255)
		ulVal = 255;
	pThis->m_ulItemDropSliderValue[2] = ulVal;
	ulColor = pThis->m_pcsAgcmItem->GetItemDropColor(
		pThis->m_eItemDropCategory);
	ulColor = (ulColor & 0xFFFFFF00) | ulVal;
	pThis->m_pcsAgcmItem->SetItemDropColor(pThis->m_eItemDropCategory, 
		ulColor);
	pThis->m_pcsAgcmUIManager2->RefreshUserData(
		pThis->m_pstItemDropPreviewUD, TRUE);
	return TRUE;
}

BOOL
AgcmUIOption::CBChangedItemDropEnable(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption *	pThis = (AgcmUIOption *)pClass;
	AcUIButton *	pcsBtn = (AcUIButton *)pcsSourceControl->m_pcsBase;

	pThis->m_bEnableItemDropSettings = 
		(pcsBtn->GetButtonMode() == ACUIBUTTON_MODE_CLICK);
	pThis->m_pcsAgcmItem->EnableItemDropSettings(
		pThis->m_bEnableItemDropSettings);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(
		pThis->m_pstItemDropEnableUD);
	return TRUE;
}

BOOL
AgcmUIOption::CBSaveItemDropSettings(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIOption * pThis = (AgcmUIOption *)pClass;

	pThis->m_pcsAgcmItem->WriteItemDropSettings();
	return TRUE;
}

BOOL AgcmUIOption::CBDisableOtherPlayerSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIOption* pThis = ( AgcmUIOption* )pClass;
	if( !pThis ) return FALSE;

	pThis->m_bDisableOtherPlayerSkillEffect = !pThis->m_bDisableOtherPlayerSkillEffect;
	pThis->UpdateDisableOtherPlayerSkillEffect( TRUE );
	pThis->AddSystemMessageForOtherSkillEffect( pThis->m_bDisableOtherPlayerSkillEffect );
	return TRUE;
}

/*
	2005.06.02. By SungHoon
	�Ϲ� ���� �ź� ��� �ʱ�ȭ
*/

BOOL AgcmUIOption::CBSettingCharacterOK( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pClass)
		return FALSE;
	AgcmUIOption		*pThis				= (AgcmUIOption *)		pClass;

	pThis->SendOptionFlag();

	pThis->m_bRefuseTrade		= FALSE;
	pThis->m_bRefusePartyIn		= FALSE;
	pThis->m_bRefuseGuildIn		= FALSE;
	pThis->m_bRefuseGuildBattle	= FALSE;
	pThis->m_bRefuseBattle		= FALSE;
	pThis->m_bRefuseGuildRelation	= FALSE;
	pThis->m_bRefuseBuddy		= FALSE;

	pThis->RefreshRefuseControl();

	if (pThis->m_pcsAgcmUIStatus &&
		!pThis->m_pcsAgcmUIStatus->SendPacketUpdateViewHelmetOption(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), (INT8) pThis->m_bVisibleViewHelmet))
		return FALSE;

	return TRUE;
}

/*
	2005.06.02 By SungHoon
	�ź� ����ũ�� ����� ��� �Ҹ���.
*/
BOOL AgcmUIOption::CBUpdateOptionFlag( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIOption		*pThis				= (AgcmUIOption *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	INT32				lOptionFlag			= *(INT32		 *)		pCustData;

	pThis->RefreshRefuseControl(lOptionFlag);
	return TRUE;
}

VOID AgcmUIOption::RefreshRefuseControl(INT32 lOptionFlag)
{
	m_bRefuseTrade			= m_pcsAgpmCharacter->IsOptionFlag( lOptionFlag, AGPDCHAR_OPTION_REFUSE_TRADE );
	m_bRefusePartyIn		= m_pcsAgpmCharacter->IsOptionFlag( lOptionFlag, AGPDCHAR_OPTION_REFUSE_PARTY_IN );
	m_bRefuseGuildBattle	= m_pcsAgpmCharacter->IsOptionFlag( lOptionFlag, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE );
	m_bRefuseBattle			= m_pcsAgpmCharacter->IsOptionFlag( lOptionFlag, AGPDCHAR_OPTION_REFUSE_BATTLE );	
	m_bRefuseGuildIn		= m_pcsAgpmCharacter->IsOptionFlag( lOptionFlag, AGPDCHAR_OPTION_REFUSE_GUILD_IN );
	m_bRefuseGuildRelation	= m_pcsAgpmCharacter->IsOptionFlag( lOptionFlag, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION );
	m_bRefuseBuddy			= m_pcsAgpmCharacter->IsOptionFlag( lOptionFlag, AGPDCHAR_OPTION_REFUSE_BUDDY );

	RefreshRefuseControl();
}

VOID AgcmUIOption::RefreshRefuseControl()
{
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDRefuseTrade);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDRefusePartyIn);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDRefuseGuildBattle);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDRefuseBattle);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDRefuseGuildIn);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDVisiblePartyBuffIcon);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDDisableSkillEffect);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDRefuseGuildRelation);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDRefuseBuddy);
}

VOID AgcmUIOption::SendOptionFlag()
{
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)	return;

	INT32 lOptionFlag = pcsSelfCharacter->m_lOptionFlag;
	if (m_bRefuseTrade)			lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_TRADE);
	if (m_bRefusePartyIn)		lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_PARTY_IN);
	if (m_bRefuseBattle)		lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_BATTLE);
	if (m_bRefuseGuildBattle)	lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
	if (m_bRefuseGuildIn)		lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
	if (m_bRefuseGuildRelation)	lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION);
	if (m_bRefuseBuddy)			lOptionFlag = m_pcsAgpmCharacter->SetOptionFlag(lOptionFlag, AGPDCHAR_OPTION_REFUSE_BUDDY);

	m_pcsAgcmCharacter->SendPacketOptionFlag(pcsSelfCharacter, lOptionFlag);
}

BOOL AgcmUIOption::CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuildSystemMessage* pstSystemMessage = (AgpdGuildSystemMessage*)pData;
	AgcmUIOption* pThis = (AgcmUIOption*)pClass;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)	return FALSE;

	switch(pstSystemMessage->m_lCode)
	{
		case AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY:
		case AGPMGUILD_SYSTEM_CODE_GUILD_LEAVE:
		case AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE:
		case AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE2:
		case AGPMGUILD_SYSTEM_CODE_GUILD_CREATE_COMPLETE:
		case AGPMGUILD_SYSTEM_CODE_GUILD_JOIN:
			pcsSelfCharacter->m_lOptionFlag = pThis->m_pcsAgpmCharacter->UnsetOptionFlag(pcsSelfCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
			pcsSelfCharacter->m_lOptionFlag = pThis->m_pcsAgpmCharacter->UnsetOptionFlag(pcsSelfCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
			pThis->RefreshRefuseControl(pcsSelfCharacter->m_lOptionFlag);				
			break;
	}

	return TRUE;
}

void AgcmUIOption::AddSystemMessageForSkillEffect( BOOL bIsOFF )
{
	CHAR *szMessage = NULL;
	CHAR *szColor = NULL;

	if( bIsOFF )
	{
		szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_SKILL_EFFECT_OFF);
		szColor = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
	}
	else
	{
		szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_SKILL_EFFECT_ON);
		szColor = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
	}

	if( szMessage )
	{
		DWORD dwColor = 0xFFFF0000;
		if( szColor )
		{
			dwColor = atol( szColor );
		}

		AgcmChatting2* pcmChatting = ( AgcmChatting2* )GetModule( "AgcmChatting2" );
		if( !pcmChatting ) return;

		pcmChatting->AddSystemMessage(szMessage, dwColor);
		pcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
	}
}

void AgcmUIOption::AddSystemMessageForOtherSkillEffect( BOOL bIsOFF )
{
	CHAR *szMessage = NULL;
	CHAR *szColor = NULL;

	if( bIsOFF )
	{
		szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OTHER_SKILL_EFFECT_OFF);
		szColor = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OFF_COLOR);
	}
	else
	{
		szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_OTHER_SKILL_EFFECT_ON);
		szColor = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OPTION_ON_COLOR);
	}

	if( szMessage )
	{
		DWORD dwColor = 0xFFFF0000;
		if( szColor )
		{
			dwColor = atol( szColor );
		}

		AgcmChatting2* pcmChatting = ( AgcmChatting2* )GetModule( "AgcmChatting2" );
		if( !pcmChatting ) return;

		pcmChatting->AddSystemMessage(szMessage, dwColor);
		pcmChatting->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
	}
}