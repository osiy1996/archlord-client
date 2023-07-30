#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4819 )

#include <ApBase/ApBase.h>
#include "MainWindow.h"
#include "MyException.h"
#include <AcuFrameMemory/AcuFrameMemory.h>
#include <AcuIMDraw/AcuIMDraw.h>
#include <AcuDeviceCheck/AcuDeviceCheck.h>
#include <AcuParticleDraw/AcuParticleDraw.h>
#include <map>

#pragma warning ( pop )

//#define _BIN_EXEC_


extern class AgpmConfig			*g_pcsAgpmConfig;
extern class AgcmResourceLoader	*g_pcsAgcmResourceLoader;
extern class AgpmFactors			*g_pcsAgpmFactors;
extern class AgpmCharacter		*g_pcsAgpmCharacter;
extern class AgcmCharacter		*g_pcsAgcmCharacter;
extern class AgpmUnion			*g_pcsAgpmUnion;
extern class AgpmItem				*g_pcsAgpmItem;
extern class AgcmItem				*g_pcsAgcmItem;
extern class AgpmItemConvert		*g_pcsAgpmItemConvert;
extern class AgcmItemConvert		*g_pcsAgcmItemConvert;
extern class AgcmPrivateTrade		*g_pcsAgcmPrivateTrade;
extern class AgpmAuctionCategory	*g_pcsAgpmAuctionCategory;
extern class AgpmAuction			*g_pcsAgpmAuction;
extern class AgcmAuction			*g_pcsAgcmAuction;
extern class AgcmRecruit			*g_pcsAgcmRecruit;
extern class AgpmRide*			g_pcsAgpmRide;
extern class AgcmRide*			g_pcsAgcmRide;
extern class ApmEventManager		*g_pcsApmEventManager	;
extern class AgpmEventNature		*g_pcsAgpmEventNature	;
extern class AgpmTimer			*g_pcsAgpmTimer;
extern class AgcmEventNature		*g_pcsAgcmEventNature	;
extern class AgpmEventTeleport	*g_pcsAgpmEventTeleport;
extern class AgcmEventTeleport	*g_pcsAgcmEventTeleport;
extern class AgpmEventNPCDialog	*g_pcsAgpmEventNPCDialog;
extern class AgpmEventNPCTrade	*g_pcsAgpmEventNPCTrade;
extern class ApmObject			*g_pcsApmObject		;
extern class AgcmObject			*g_pcsAgcmObject		;
extern class AgcmEventEffect		*g_pcsAgcmEventEffect	;
extern class AgcmEventNPCTrade	*g_pcsAgcmEventNPCTrade;
extern class AgpmEventItemRepair	*g_pcsAgpmEventItemRepair;
extern class AgcmEventItemRepair	*g_pcsAgcmEventItemRepair;
extern class AgpmEventBank		*g_pcsAgpmEventBank	;
extern class AgcmEventBank		*g_pcsAgcmEventBank	;
extern class AgpmEventItemConvert	*g_pcsAgpmEventItemConvert	;
extern class AgcmEventItemConvert	*g_pcsAgcmEventItemConvert	;
extern class AgcmSound			*g_pcsAgcmSound		;
extern class AgpmParty			*g_pcsAgpmParty		;
extern class AgcmParty			*g_pcsAgcmParty		;
extern class AgcmShadow			*g_pcsAgcmShadow	;
extern class AgcmShadow2*			g_pcsAgcmShadow2	;
extern class AgcmUIManager2		*g_pcsAgcmUIManager2;
extern class AgcmWater			*g_pcsAgcmWater		;
extern class AgpmSkill			*g_pcsAgpmSkill		;
extern class AgcmSkill			*g_pcsAgcmSkill		;
extern class AgpmEventSkillMaster	*g_pcsAgpmEventSkillMaster	;
extern class AgcmEventSkillMaster	*g_pcsAgcmEventSkillMaster	;
extern class AgcmLODManager		*g_pcsAgcmLODManager;
extern class AgcmFont				*g_pcsAgcmFont		;
extern class AgcmChatting2		*g_pcsAgcmChatting2	;
extern class AgpmChatting			*g_pcsAgpmChatting	;
extern class AgcmUIControl		*g_pcsAgcmUIControl	;
extern class AgcmLensFlare		*g_pcsAgcmLensFlare	;
extern class AgcmTextBoardMng		*g_pcsAgcmTextBoardMng	;
extern class AgcmRender			*g_pcsAgcmRender	;
extern class AgcmTuner			*g_pcsAgcmTuner		;
extern class AgpmGrid				*g_pcsAgpmGrid		;
extern class AgcmGlyph			*g_pcsAgcmGlyph		;
extern class AgcmTargeting		*g_pcsAgcmTargeting	;
extern class AgpmQuest			*g_pcsAgpmQuest		;
extern class AgcmQuest			*g_pcsAgcmQuest		;
extern class AgcmUVAnimation		*g_pcsAgcmUVAnimation	;

extern class AgcmCustomizeRender	*g_pcsAgcmCustomizeRender;

extern class AgpmAdmin			*g_pcsAgpmAdmin		;
extern class AgcmAdmin			*g_pcsAgcmAdmin		;

extern class AgcmUIMain			*g_pcsAgcmUIMain		;
extern class AgcmUILogin			*g_pcsAgcmUILogin		;
extern class AgcmLogin			*g_pcsAgcmLogin			;
extern class AgcmUIChatting2		*g_pcsAgcmUIChatting2	;

extern class AgcmUICharacter		*g_pcsAgcmUICharacter	;
extern class AgcmUICooldown		*g_pcsAgcmUICooldown	;
extern class AgcmUIAuction		*g_pcsAgcmUIAuction		;
extern class AgcmUIItem			*g_pcsAgcmUIItem		;
extern class AgcmUISkill2			*g_pcsAgcmUISkill2		;
extern class AgcmUIEventItemRepair	*g_pcsAgcmUIEventItemRepair	;
extern class AgcmUIEventTeleport	*g_pcsAgcmUIEventTeleport	;
extern class AgcmUIParty			*g_pcsAgcmUIParty;
extern class AgcmUIPartyOption	*g_pcsAgcmUIPartyOption;
extern class AgcmUIEventBank		*g_pcsAgcmUIEventBank	;
extern class AgcmUIEventNPCDialog *g_pcsAgcmUIEventNPCDialog;
extern class AgcmUIEventItemConvert	*g_pcsAgcmUIEventItemConvert;
extern class AgcmUISplitItem		*g_pcsAgcmUISplitItem;

extern class AgcmMinimap			*g_pcsAgcmMinimap	;
extern class ApmMap				*g_pcsApmMap		;

extern class AgcmUIBuddy			*g_pcsAgcmUIBuddy	;
extern class AgpmSearch			*g_pcsAgpmSearch	;
extern class AgcmUISearch			*g_pcsAgcmUISearch	;

extern class AgcmGlare			*g_pcsAgcmGlare		;
extern class AgcmUIOption			*g_pcsAgcmUIOption;
extern class ApmOcTree			*g_pcsApmOcTree;
extern class AgcmOcTree			*g_pcsAgcmOcTree;

extern class AgpmUIStatus			*g_pcsAgpmUIStatus;
extern class AgcmUIStatus			*g_pcsAgcmUIStatus;

extern class AgcmGrass			*g_pcsAgcmGrass;

extern class AgpmGuild			*g_pcsAgpmGuild;

extern class AgpmProduct			*g_pcsAgpmProduct;
extern class AgcmProduct			*g_pcsAgcmProduct;
extern class AgcmUIProductSkill	*g_pcsAgcmUIProductSkill;
extern class AgcmUIProduct		*g_pcsAgcmUIProduct;
extern class AgcmProductMed		*g_pcsAgcmProductMed;

extern class AgpmRefinery			*g_pcsAgpmRefinery;
extern class AgcmRefinery			*g_pcsAgcmRefinery;
extern class AgpmEventRefinery	*g_pcsAgpmEventRefinery;
extern class AgcmEventRefinery	*g_pcsAgcmEventRefinery;
extern class AgcmUIRefinery		*g_pcsAgcmUIRefinery;

extern class AgpmSummons			*g_pcsAgpmSummons;

extern class AgpmPvP				*g_pcsAgpmPvP;
extern class AgcmUIPvP			*g_pcsAgcmUIPvP;

extern class AgpmEventCharCustomize	*g_pcsAgpmEventCharCustomize;
extern class AgcmEventCharCustomize	*g_pcsAgcmEventCharCustomize;
extern class AgcmUICustomizing		*g_pcsAgcmUICustomizing;

extern class AgcmUISystemMessage		*g_pcsAgcmUISystemMessage;

extern class AgcmUITips				*g_pcsAgcmUITips;
extern class AgcmUIHelp				*g_pcsAgcmUIHelp;

extern class AgpmRemission			*g_pcsAgpmRemission;
extern class AgcmUIRemission			*g_pcsAgcmUIRemission;

extern class AgpmOptimizedPacket2	*g_pcsAgpmOptimizedPacket2;

extern class AgcmUIDebugInfo		*g_pcsAgcmUIDebugInfo;

extern class AgpmAreaChatting		*g_pcsAgpmAreaChatting;

extern class AgcmUIConsole		*g_pcsAgcmUIConsole;
extern class AgcmNatureEffect	*g_pcsAgcmNatureEffect;
extern class AgpmEventPointLight	*g_pcsAgpmEventPointLight;
extern class AgcmEventPointLight	*g_pcsAgcmEventPointLight;

extern class AgpmScript			*g_pcsAgpmScript;

extern class AgpmCashMall			*g_pcsAgpmCashMall;
extern class AgcmCashMall			*g_pcsAgcmCashMall;
extern class AgcmUICashMall		*g_pcsAgcmUICashMall;
extern class AgcmUICashInven		*g_pcsAgcmUICashInven;

#ifdef _AREA_TAIWAN_
extern class AgcmUICouponBox		*g_pcsAgcmUICoponBox;
#endif

extern class AgpmMailBox			*g_pcsAgpmMailBox;
extern class AgcmUIMailBox		*g_pcsAgcmUIMailBox;

extern class AgpmReturnToLogin	*g_pcsAgpmReturnToLogin;
extern class AgcmReturnToLogin	*g_pcsAgcmReturnToLogin;

extern class AgpmWantedCriminal	*g_pcsAgpmWantedCriminal;
extern class AgcmUIWantedCriminal	*g_pcsAgcmUIWantedCriminal;

extern class AgpmStartupEncryption *g_pcsAgpmStartupEncryption;
extern class AgcmStartupEncryption *g_pcsAgcmStartupEncryption;

extern class AgpmSiegeWar			*g_pcsAgpmSiegeWar;
extern class AgpmEventSiegeWarNPC	*g_pcsAgpmEventSiegeWarNPC;
extern class AgcmSiegeWar			*g_pcsAgcmSiegeWar;
extern class AgcmEventSiegeWarNPC	*g_pcsAgcmEventSiegeWarNPC;
extern class AgcmUISiegeWar		*g_pcsAgcmUISiegeWar;

extern class AgpmGuildWarehouse	*g_pcsAgpmGuildWarehouse;
extern class AgcmUIGuildWarehouse	*g_pcsAgcmUIGuildWarehouse;

//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//extern class AgpmGamble			*g_pcsAgpmGamble;

extern class AgcmUIGuild			*g_pcsAgcmUIGuild;

extern class AgcmUIHotkey			*g_pcsAgcmUIHotkey;

extern class AgcmUISocialAction	*g_pcsAgcmUISocialAction;

extern class AgpmCasper			*g_pcsAgpmCasper;

extern class AgpmArchlord			*g_pcsAgpmArchlord;
extern class AgcmArchlord			*g_pcsAgcmArchlord;
extern class AgcmUIArchlord		*g_pcsAgcmUIArchlord;

extern class AgcmConnectManager*	g_pcsAgcmConnectManager	;

extern class AgcmPostFX*			g_pcsAgcmPostFX;
extern class AgcmEff2*			g_pcsAgcmEff2;
extern class AgpmEventGacha*		g_pcsAgpmEventGacha;

extern class AgpmBattleGround*	g_pcsAgpmBattleGround;
extern class AgcmBattleGround*	g_pcsAgcmBattleGround;

// Point Info for debug teleport
class	TeleportInfo
{
public:
	string	strName;
	AuPOS	pos;

	TeleportInfo( const char * pStr , FLOAT x , FLOAT z ): strName( pStr )
	{	pos.x = x;		pos.y = 0.0f;		pos.z = z;	}
	TeleportInfo(): strName( "noname" ) 
	{	pos.x = 0.0f;	pos.y = 0.0f;		pos.z = 0.0f;	}
};

class MyEngine : public AgcEngine, public CMyException
{
public:
	MyEngine();
	virtual ~MyEngine();

	// Message
	BOOL				OnInit();
	BOOL				OnInitialize();
	void				OnTerminate();
	BOOL				OnAddModule();
	BOOL				OnAttachPlugins();	// 플러그인 연결 은 여기서
	RpWorld *			OnCreateWorld(); // 월드를 생성해서 리턴해준다.
	RwCamera *			OnCreateCamera( RpWorld * pWorld );	// 메인 카메라를 Create해준다.
	BOOL				OnCreateLight();	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
	AgcWindow *			OnSetMainWindow()	{	return m_pMainWindow;	}

	BOOL				OnRegisterModulePreCreateWorld()	{	return TRUE;	}
	BOOL				OnRegisterModule();

	VOID				OnEndIdle();
	VOID				OnWorldRender();
	BOOL				OnRenderPreCameraUpdate();

	BOOL				OnPostRegisterModule()				{	return TRUE;	}
	BOOL				OnPostInitialize();
	BOOL				OnCameraResize( RwRect * pRect );	

	UINT32				OnWindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam );

	BOOL				OnSelectDevice();
	void				LuaErrorMessage( const char * pStr );
	BOOL				SetupOption();

protected:
	BOOL				RemoveDebugLogFile();	// 2004.01.15. steeple
	BOOL				PatchPatcher();
	VOID				SetupLanguage();
	VOID				PostInit(void);

public:
	static BOOL		CBGetMyCharacterName(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGameguardAuth( PVOID pData, PVOID pClass, PVOID pCustData	);

	INT32			GetTeleportEmptyIndex();
	TeleportInfo *	GetTeleportPoint( INT32 nIndex );
	TeleportInfo *	GetTeleportPoint( string strName );
	BOOL			AddTeleportPoint( INT32 nIndex , TeleportInfo &stInfo );
	BOOL			DeleteTeleportPoint( INT32 nIndex );
	BOOL			DeleteTeleportPoint( string strName );
	BOOL			LoadTelportPoint( string strName = "ggpoint.ini" );
	BOOL			SaveTelportPoint( string strName = "ggpoint.ini" );

public:
	HCURSOR				m_hCursor;
	BOOL				m_bEmulationFullscreen;
	CHAR*				m_szLoginServer;
	MainWindow*			m_pMainWindow;

	AcuFrameMemory		m_cAcuFrameMemory;
	AcuIMDraw			m_cAcuIMDraw;
	AcuDeviceCheck		m_cAcuDeviceCheck;
	AcuParticleDraw		m_cAcuParticleDraw;

	map< INT32 , TeleportInfo >	m_mapTeleportPoint;
};

extern MyEngine g_cMyEngine;