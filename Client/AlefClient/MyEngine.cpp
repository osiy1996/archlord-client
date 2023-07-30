#include "resource.h"
#include <rwcore.h>
#include <rpworld.h>
#include <rtcharse.h>
#include <rpcollis.h>
#include <rpusrdat.h>
#include "rpspline.h"
#include <rtanim.h>
#include <rplodatm.h>
#include "rpmatfx.h"
#include "rpmorph.h"
#include "rpuvanim.h"
#include "rptoon.h"
#include <rprandom.h>
#include <rppvs.h>
#include <rtbmp.h>
#include <rtpng.h>
#include "rpUVAnim.h"
#include <rplodatm.h>
#ifdef _DPVS
#include "rpdpvs.h"
#endif //_DPVS
#include <AgcSkeleton/skeleton.h>
#include <AgcSkeleton/menu.h>
#include <AgcSkeleton/camera.h>
#include "time.h"
#include <AcuRpMTexture/AcuRpMTexture.h>
#include <AcuRpMatFxEx/AcuRpMatFxEx.h>
#include <AcuRpUVAnimData/AcuRpUVAnimData.h>
#include <AcuObject/AcuObject.h>
#include <AcuRtAmbOcclMap/AcuRtAmbOcclMap.h>
#include <AcuMyFirewall/AcuMyFirewall.h>
#include <AcuRpMatD3DFx/AcuRpMatD3DFx.h>
#include <AgcSkeleton/win.h>
#include <wchar.h>
#include <io.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AgpmPathFind/AgpmPathFind.h>
#include <AuPacket/AuPacket.h>
#include <AuStrTable/AuStrTable.h>
#include <AuIniManager/AuIniManager.h>
#include <AuFileFind/AuFileFind.h>
#include <AuProcessManager/AuProcessManager.h>
#include <AgpmTitle/AgpmTitle.h>
#include <AgcmTitle/AgcmTitle.h>
#include "MyEngine.h"
#include "MainWindow.h"
#include "LuaFunc.h"
#include <AgpmGuild/AgpmGuild.h>
#include <AgcmTargeting/AgcmTargeting.h>
#include <AgpmEventTeleport/AgpmEventTeleport.h>
#include <AgpmSkill/AgpmSkill.h>
#include <AgcmRender/AgcmRender.h>
#include <AgpmAdmin/AgpmAdmin.h>
#include <AgcmFont/AgcmFont.h>
#include <AgcmEff2/AgcmEff2.h>
#include <DbgHelp.h>
#include <AgcmUIConsole/AgcmUIConsole.h>
#include <AgcmChatting2/AgcmChatting2.h>
#include <AgcmSound/AgcmSound.h>
#include <AgcmPostFX/AgcmPostFX.h>
#include <AgcmUIOption/AgcmUIOption.h>
#include <AgcmGrass/AgcmGrass.h>
#include <AgcmWater/AgcmWater.h>
#include <AgcmLensFlare/AgcmLensFlare.h>
#include <AgcmUIEventNPCDialog/AgcmUIEventNPCDialog.h>
#include <AgcmEventNature/AgcmEventNature.h>
#include <AgcmNatureEffect/AgcmNatureEffect.h>
#include <AgcmTextBoard/AgcmTextBoardMng.h>
#include <AgpmEventNature/AgpmEventNature.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmUIGuild/AgcmUIGuild.h>
#include <ApmObject/ApmObject.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmItem/AgcmItem.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmSkill/AgpmSkill.h>
#include <AgcmSkill/AgcmSkill.h>
#include <AgcmEventEffect/AgcmEventEffect.h>
#include <AgpmAuctionCategory/AgpmAuctionCategory.h>
#include <AgpmQuest/AgpmQuest.h>
#include <AgcmQuest/AgcmQuest.h>
#include <AgpmEventSkillMaster/AgpmEventSkillMaster.h>
#include <AgpmItemConvert/AgpmItemConvert.h>
#include <AgpmProduct/AgpmProduct.h>
#include <AgpmRefinery/AgpmRefinery.h>
#include <AgcmUICharacter/AgcmUICharacter.h>
#include <AgcmUIItem/AgcmUIItem.h>
#include <AgcmEventCharCustomize/AgcmEventCharCustomize.h>
#include <AgpmEventCharCustomize/AgpmEventCharCustomize.h>
#include <AgcmUITips/AgcmUITips.h>
#include <AgcmUIHelp/AgcmUIHelp.h>
#include <AgcmLODManager/AgcmLODManager.h>
#include <AgcmUIAuction/AgcmUIAuction.h>
#include <AgpmSiegeWar/AgpmSiegeWar.h>
#include <AgcmUIHotkey/AgcmUIHotkey.h>
#include <AgpmTimer/AgpmTimer.h>
#include <AgcmUILogin/AgcmUILogin.h>
#include <AgcmUIMain/AgcmUIMain.h>
#include <AgcmUISystemMessage/AgcmUISystemMessage.h>
#include <AgcmUIDebugInfo/AgcmUIDebugInfo.h>
#include <AgcmAdmin/AgcmAdmin.h>
#include <AgcmMap/AgcmMap.h>
#include <AgcmShadow/AgcmShadow.h>
#include <AgcmObject/AgcmObject.h>
#include <AgpmParty/AgpmParty.h>
#include <AgpmAuction/AgpmAuction.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <AgcmCustomizeRender/AgcmCustomizeRender.h>
#include <AgcmShadow2/AgcmShadow2.h>
#include <AgcmEff2/AgcmEff2.h>
#include <AgcmDynamicLightmap/AgcmDynamicLightmap.h>
#include <ApmEventManager/ApmEventManager.h>
#include <AgcmGlyph/AgcmGlyph.h>
#include <AgpmMailBox/AgpmMailBox.h>
#include <AgpmCashMall/AgpmCashMall.h>
#include <AgcmConnectManager/AgcmConnectManager.h>
#include <AgpmScript/AgpmScript.h>
#include <AgpmSummons/AgpmSummons.h>
#include <AgpmPvP/AgpmPvP.h>
#include <AgpmOptimizedPacket2/AgpmOptimizedPacket2.h>
#include <AgpmReturnToLogin/AgpmReturnToLogin.h>
#include <AgcmCamera2/AgcmCamera2.h>
#include <AgpmReturnToLogin/AgpmReturnToLogin.h>
#include <AgcmShadowmap/AgcmShadowmap.h>
#include <AgpmBillInfo/AgpmBillInfo.h>
#include <AgcmEff2/AgcdEffGlobal.h>
#include <AgpmConfig/AgpmConfig.h>
#include <AgpmResourceInfo/AgpmResourceInfo.h>
#include <AgcmAmbOcclMap/AgcmAmbOcclMap.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmUnion/AgpmUnion.h>
#include <AgpmUIStatus/AgpmUIStatus.h>
#include <AgpmSystemMessage/AgpmSystemMessage.h>
#include <AgpmArchlord/AgpmArchlord.h>
#include <AgpmCombat/AgpmCombat.h>
#include <AgpmRide/AgpmRide.h>
#include <AgcmUICooldown/AgcmUICooldown.h>
#include <AgpmEventSiegeWarNPC/AgpmEventSiegeWarNPC.h>
#include <AgpmTax/AgpmTax.h>
#include <AgpmGuildWarehouse/AgpmGuildWarehouse.h>
#include <AgpmNatureEffect/AgpmNatureEffect.h>
#include <AgpmEventNPCDialog/AgpmEventNPCDialog.h>
#include <AgpmEventNPCTrade/AgpmEventNPCTrade.h>
#include <AgpmEventItemRepair/AgpmEventItemRepair.h>
#include <AgpmEventItemConvert/AgpmEventItemConvert.h>
#include <AgpmEventBank/AgpmEventBank.h>
#include <AgpmEventGuild/AgpmEventGuild.h>
#include <AgpmEventQuest/AgpmEventQuest.h>
#include <AgpmEventRefinery/AgpmEventRefinery.h>
#include <AgpmEventProduct/AgpmEventProduct.h>
#include <AgpmChatting/AgpmChatting.h>
#include <AgpmAreaChatting/AgpmAreaChatting.h>
#include <AgcmRecruit/AgcmRecruit.h>
#include <AgcmItemConvert/AgcmItemConvert.h>
#include <AgcmPrivateTrade/AgcmPrivateTrade.h>
#include <AgcmParty/AgcmParty.h>
#include <AgcmRide/AgcmRide.h>
#include <AgcmProduct/AgcmProduct.h>
#include <AgcmRefinery/AgcmRefinery.h>
#include <AgcmCashMall/AgcmCashMall.h>
#include <AgcmSiegeWar/AgcmSiegeWar.h>
#include <AgcmEventManager/AgcmEventManager.h>
#include <AgcmArchlord/AgcmArchlord.h>
#include <AgcmAuction/AgcmAuction.h>
#include <AgcmEventItemRepair/AgcmEventItemRepair.h>
#include <AgcmEventBank/AgcmEventBank.h>
#include <AgcmEventItemConvert/AgcmEventItemConvert.h>
#include <AgcmEventGuild/AgcmEventGuild.h>
#include <AgcmEventSiegeWarNPC/AgcmEventSiegeWarNPC.h>
#include <AgcmEventSkillMaster/AgcmEventSkillMaster.h>
#include <AgcmEventProduct/AgcmEventProduct.h>
#include <AgcmEventRefinery/AgcmEventRefinery.h>
#include <AgcmEventQuest/AgcmEventQuest.h>
#include <AgcmBillInfo/AgcmBillInfo.h>
#include <AgcmEventTeleport/AgcmEventTeleport.h>
#include <AgcmEventNPCDialog/AgcmEventNPCDialog.h>
#include <AgcmEventNPCTrade/AgcmEventNPCTrade.h>
#include <AgcmUVAnimation/AgcmUVAnimation.h>
#include <AgcmWorld/AgcmWorld.h>
#include <AgpmBuddy/AgpmBuddy.h>
#include <AgpmChannel/AgpmChannel.h>
#include <AgpmStartupEncryption/AgpmStartupEncryption.h>
#include <AgcmReturnToLogin/AgcmReturnToLogin.h>
#include <AgcmUISplitItem/AgcmUISplitItem.h>
#include <AgcmUICashInven/AgcmUICashInven.h>
#include <AgcmUISkill2/AgcmUISkill2.h>
#include <AgcmUIChatting2/AgcmUIChatting2.h>
#include <AgcmUIEventItemRepair/AgcmUIEventItemRepair.h>
#include <AgcmUIEventTeleport/AgcmUIEventTeleport.h>
#include <AgcmUIParty/AgcmUIParty.h>
#include <AgcmUIPartyOption/AgcmUIPartyOption.h>
#include <AgcmUIEventBank/AgcmUIEventBank.h>
#include <AgcmUIEventItemConvert/AgcmUIEventItemConvert.h>
#include <AgcmUIQuest2/AgcmUIQuest2.h>
#include <AgcmUIBuddy/AgcmUIBuddy.h>
#include <AgcmUIChannel/AgcmUIChannel.h>
#include <AgcmUIPvP/AgcmUIPvP.h>
#include <AgcmUIProduct/AgcmUIProductSkill.h>
#include <AgcmUIProduct/AgcmUIProduct.h>
#include <AgcmProductMed/AgcmProductMed.h>
#include <AgcmUIRefinery/AgcmUIRefinery.h>
#include <AgpmRemission/AgpmRemission.h>
#include <AgcmUIRemission/AgcmUIRemission.h>
#include <AgcmUICashMall/AgcmUICashMall.h>
#include <AgcmUIMailBox/AgcmUIMailBox.h>
#include <AgpmWantedCriminal/AgpmWantedCriminal.h>
#include <AgcmUIWantedCriminal/AgcmUIWantedCriminal.h>
#include <AgcmUIArchlord/AgcmUIArchlord.h>
#include <AgcmUITax/AgcmUITax.h>
#include <AgcmUIGuildWarehouse/AgcmUIGuildWarehouse.h>
#include <AgpmSearch/AgpmSearch.h>
#include <AgcmUISearch/AgcmUISearch.h>
#include <AgcmGlare/AgcmGlare.h>
#include <AgcmTuner/AgcmTuner.h>
#include <AgcmUIStatus/AgcmUIStatus.h>
#include <AgcmUISiegeWar/AgcmUISiegeWar.h>
#include <AgpmEventPointLight/AgpmEventPointLight.h>
#include <AgcmEventPointLight/AgcmEventPointLight.h>
#include <AgcmUICustomizing/AgcmUICustomizing.h>
#include <AgcmStartupEncryption/AgcmStartupEncryption.h>
#include <AgcmUISocialAction/AgcmUISocialAction.h>
#include <AgpmCasper/AgpmCasper.h>
#include <AgpmEventGacha/AgpmEventGacha.h>
#include <AgcmEventGacha/AgcmEventGacha.h>
#include <AgcmUIEventGacha/AgcmUIEventGacha.h>
#include <AgpmBattleGround/AgpmBattleGround.h>
#include <AgcmBattleGround/AgcmBattleGround.h>
#include <AcuRWUtil/AgcdGeoTriAreaTbl.h>
#include <AgcmUIOption/AgcdUIOption.h>
#include <AgpmGameEvent/AgpmGameEvent.h>
#include <AgcmUIGameEvent/AgcmUIGameEvent.h>

#ifdef _AREA_JAPAN_
	#include "AuJapaneseClientAuth.h"
	#include "AuGameInfoJpn.h"
	#include "libHGPSHELP.hpp"	//�Ϻ� ���� ���μ��� ���� ������Ʈ

	HMODULE g_hgpshelp = NULL;

	#ifndef _DEBUG
		#include "AuNPGameLib.h"
		#pragma comment(lib, "NPGameLib_MTDLL.lib")

		BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);
		char* GetNPMessage(DWORD dwResult);
	#endif
#endif

#ifdef _AREA_KOREA_
	#include "AuHanAuth.h"
#endif

#include <AuGameGuard/AuNProtectAuth.h>
#include <AuGameGuard/AuNPGameLib.h>
#include <AgcAuth/AgcAuth.h>
#include <AgcmRegistryManager/AgcmRegistryManager.h>
#include <AgcmImGui/AgcmImGui.h>
#include <DbgHelp.h>
#include <AgcmArena/AgcmArena.h>
#include <AgcmBattle/AgcmBattle.h>

#define PATCH_CLIENT_NAME			"archlord.exe"
#define DOWNLOAD_PATCH_CLIENT_NAME	"archlord.bak"
#define TEMP_PATCH_CLIENT_NAME		"_archlord.exe"

extern MINIDUMP_TYPE	g_eMiniDumpType	;//
extern AuCircularBuffer	g_AuCircularBuffer;
extern psGlobalType		PsGlobal;
extern BOOL				g_bUseTrace;

MyEngine				g_cMyEngine;

AgpmConfig*				g_pcsAgpmConfig = NULL;

AgcmResourceLoader*		g_pcsAgcmResourceLoader = NULL;

// #ifdef _AREA_GLOBAL_
// AgcmHealthCheck*		g_pcsAgcmHealthCheck	= NULL;
// #endif

AgpmResourceInfo*		g_pcsAgpmResourceInfo	= NULL;

AgpmFactors*			g_pcsAgpmFactors	 = NULL;

ApmMap*					g_pcsApmMap = NULL;
/*
#ifdef _AREA_GLOBAL_
AgcmPathFinder*			g_pcsAgcmPathFinder = NULL;
#endif
*/
ApmObject*				g_pcsApmObject = NULL;
AgcmObject*				g_pcsAgcmObject = NULL;

AgpmCharacter*			g_pcsAgpmCharacter	 = NULL;
AgcmCharacter*			g_pcsAgcmCharacter	 = NULL;

AgpmUnion*				g_pcsAgpmUnion		 = NULL;

AgpmItem*				g_pcsAgpmItem	 = NULL;
AgcmItem*				g_pcsAgcmItem	 = NULL;

AgpmItemConvert*		g_pcsAgpmItemConvert	= NULL;
AgcmItemConvert*		g_pcsAgcmItemConvert	= NULL;

AgpmCombat*				g_pcsAgpmCombat	 = NULL;

AgcmPrivateTrade*		g_pcsAgcmPrivateTrade	 = NULL;

AgcmConnectManager*		g_pcsAgcmConnectManager	 = NULL;
AgpmLogin*				g_pcsAgpmLogin = NULL;
AgcmLogin*				g_pcsAgcmLogin = NULL;

AgpmAuctionCategory*	g_pcsAgpmAuctionCategory	= NULL;
AgpmAuction*			g_pcsAgpmAuction			= NULL;
AgcmAuction*			g_pcsAgcmAuction			 = NULL;
AgcmRecruit*			g_pcsAgcmRecruit			 = NULL;

AgpmRide*				g_pcsAgpmRide	= NULL;
AgcmRide*				g_pcsAgcmRide	= NULL;

AgcmCamera2*			g_pcsAgcmCamera2			 = NULL;

AgcmRender*				g_pcsAgcmRender			 = NULL;

ApmEventManager*		g_pcsApmEventManager		 = NULL;
AgcmEventManager*		g_pcsAgcmEventManager	 = NULL;

AgpmTimer*				g_pcsAgpmTimer			 = NULL;

AgpmEventNature*		g_pcsAgpmEventNature		 = NULL;
AgcmEventNature*		g_pcsAgcmEventNature		 = NULL;

AgpmEventNPCDialog*		g_pcsAgpmEventNPCDialog	 = NULL;
AgcmEventNPCDialog*		g_pcsAgcmEventNPCDialog	 = NULL;

AgpmEventNPCTrade*		g_pcsAgpmEventNPCTrade	 = NULL;
AgcmEventNPCTrade*		g_pcsAgcmEventNPCTrade	 = NULL;

AgpmEventItemRepair*	g_pcsAgpmEventItemRepair	 = NULL;
AgcmEventItemRepair*	g_pcsAgcmEventItemRepair	 = NULL;

AgpmEventTeleport*		g_pcsAgpmEventTeleport	 = NULL;
AgcmEventTeleport*		g_pcsAgcmEventTeleport	 = NULL;

AgpmEventBank*			g_pcsAgpmEventBank		= NULL;
AgcmEventBank*			g_pcsAgcmEventBank		= NULL;

AgpmEventItemConvert*	g_pcsAgpmEventItemConvert	= NULL;
AgcmEventItemConvert*	g_pcsAgcmEventItemConvert	= NULL;

AgpmEventGuild*			g_pcsAgpmEventGuild = NULL;
AgcmEventGuild*			g_pcsAgcmEventGuild = NULL;

AgpmEventProduct*		g_pcsAgpmEventProduct = NULL;
AgcmEventProduct*		g_pcsAgcmEventProduct = NULL;

AgpmEventQuest*			g_pcsAgpmEventQuest = NULL;
AgcmEventQuest*			g_pcsAgcmEventQuest = NULL;

AgcmEventEffect*		g_pcsAgcmEventEffect	 = NULL;
AgcmEff2*				g_pcsAgcmEff2			 = NULL;
AgcmSound*				g_pcsAgcmSound			 = NULL;

AgpmParty*				g_pcsAgpmParty			 = NULL;
AgcmParty*				g_pcsAgcmParty			 = NULL;

AgcmUIManager2*			g_pcsAgcmUIManager2		 = NULL;


AgcmWater*				g_pcsAgcmWater			 = NULL;
AgcmLODManager*			g_pcsAgcmLODManager		 = NULL;
AgcmFont*				g_pcsAgcmFont			 = NULL;
AgcmLensFlare*			g_pcsAgcmLensFlare		 = NULL;

AgpmSkill*				g_pcsAgpmSkill			 = NULL;
AgcmSkill*				g_pcsAgcmSkill			 = NULL;

AgpmEventSkillMaster*	g_pcsAgpmEventSkillMaster	= NULL;
AgcmEventSkillMaster*	g_pcsAgcmEventSkillMaster	= NULL;

AgcmUIMain*				g_pcsAgcmUIMain			 = NULL;
AgcmUIControl*			g_pcsAgcmUIControl		 = NULL;
AgcmChatting2*			g_pcsAgcmChatting2		 = NULL;
AgpmChatting*			g_pcsAgpmChatting		 = NULL;
AgcmTextBoardMng*		g_pcsAgcmTextBoardMng		 = NULL;
AgcmTuner*				g_pcsAgcmTuner			 = NULL;
AgcmGlyph*				g_pcsAgcmGlyph			 = NULL;
AgcmTargeting*			g_pcsAgcmTargeting		 = NULL;

AgpmGrid*				g_pcsAgpmGrid			 = NULL;

AgcmGlare*				g_pcsAgcmGlare			 = NULL;
AgpmWorld*				g_pcsAgpmWorld			 = NULL;
AgcmWorld*				g_pcsAgcmWorld			 = NULL;

AgpmAdmin*		g_pcsAgpmAdmin = NULL;	// 2003. 10. 03 steeple
AgcmAdmin*		g_pcsAgcmAdmin = NULL;	// 2003. 10. 03 steeple

AgpmQuest*				g_pcsAgpmQuest = NULL;
AgcmQuest*				g_pcsAgcmQuest = NULL;
AgpmBuddy*				g_pcsAgpmBuddy = NULL;
AgpmChannel*			g_pcsAgpmChannel = NULL;

AgcmUILogin*			g_pcsAgcmUILogin = NULL;
AgcmUIChatting2*		g_pcsAgcmUIChatting2 = NULL;

AgcmUICharacter*		g_pcsAgcmUICharacter	 = NULL;
AgcmUICooldown*			g_pcsAgcmUICooldown		= NULL;
AgcmUIAuction*			g_pcsAgcmUIAuction		= NULL;
AgcmUIItem*				g_pcsAgcmUIItem		 = NULL;
AgcmUISkill2*			g_pcsAgcmUISkill2		 = NULL;
AgcmUIEventItemRepair*	g_pcsAgcmUIEventItemRepair		 = NULL;
AgcmUIEventTeleport*	g_pcsAgcmUIEventTeleport	 = NULL;
AgcmUIParty*			g_pcsAgcmUIParty = NULL;
AgcmUIPartyOption*		g_pcsAgcmUIPartyOption = NULL;
AgcmUIEventBank*		g_pcsAgcmUIEventBank	 = NULL;
AgcmUIEventNPCDialog*	g_pcsAgcmUIEventNPCDialog = NULL;
AgcmUIEventItemConvert*	g_pcsAgcmUIEventItemConvert	= NULL;
AgcmUIGuild*			g_pcsAgcmUIGuild = NULL;
AgcmUISplitItem*		g_pcsAgcmUISplitItem = NULL;
AgcmUIQuest2*			g_pcsAgcmUIQuest2 = NULL;
AgcmUIBuddy*			g_pcsAgcmUIBuddy = NULL;

AgcmUIChannel*			g_pcsAgcmUIChannel = NULL;

AgcmMinimap*			g_pcsAgcmMinimap		 = NULL;

AgcmShadow*				g_pcsAgcmShadow	 = NULL;
AgcmShadow2*			g_pcsAgcmShadow2 = NULL;
AgcmUIOption*			g_pcsAgcmUIOption = NULL;

ApmOcTree*				g_pcsApmOcTree = NULL;
AgcmOcTree*				g_pcsAgcmOcTree = NULL;

AgpmUIStatus*			g_pcsAgpmUIStatus	= NULL;
AgcmUIStatus*			g_pcsAgcmUIStatus	= NULL;

AgcmGrass*				g_pcsAgcmGrass = NULL;

AgpmGuild*				g_pcsAgpmGuild = NULL;
AgcmGuild*				g_pcsAgcmGuild = NULL;

AgpmProduct*			g_pcsAgpmProduct = NULL;
AgcmProduct*			g_pcsAgcmProduct = NULL;
AgcmUIProductSkill*		g_pcsAgcmUIProductSkill = NULL;
AgcmUIProduct*			g_pcsAgcmUIProduct = NULL;
AgcmProductMed*			g_pcsAgcmProductMed = NULL;

AgpmRefinery*			g_pcsAgpmRefinery = NULL;
AgcmRefinery*			g_pcsAgcmRefinery = NULL;
AgpmEventRefinery*		g_pcsAgpmEventRefinery = NULL;
AgcmEventRefinery*		g_pcsAgcmEventRefinery = NULL;
AgcmUIRefinery*			g_pcsAgcmUIRefinery = NULL;

AgpmSummons*			g_pcsAgpmSummons = NULL;
AgpmPvP*				g_pcsAgpmPvP = NULL;
AgcmUIPvP*				g_pcsAgcmUIPvP = NULL;

AgpmRemission*			g_pcsAgpmRemission = NULL;
AgcmUIRemission*		g_pcsAgcmUIRemission = NULL;

AgcmUVAnimation*		g_pcsAgcmUVAnimation = NULL;

AgcmDynamicLightmap*	g_pcsAgcmDynamicLightmap = NULL;

AgcmShadowmap*			g_pcsAgcmShadowmap = NULL;

AgcmPostFX*				g_pcsAgcmPostFX = NULL;

AgcmCustomizeRender*	g_pcsAgcmCustomizeRender = NULL;

AgpmOptimizedPacket2*	g_pcsAgpmOptimizedPacket2	= NULL;

AgcmUIDebugInfo*		g_pcsAgcmUIDebugInfo	= NULL;
AgpmNatureEffect*		g_pcsAgpmNatureEffect	= NULL;
AgcmNatureEffect*		g_pcsAgcmNatureEffect	= NULL;

AgpmAreaChatting*		g_pcsAgpmAreaChatting	= NULL;

AgcmUIConsole*			g_pcsAgcmUIConsole = NULL;

AgpmEventCharCustomize	*g_pcsAgpmEventCharCustomize	= NULL;
AgcmEventCharCustomize	*g_pcsAgcmEventCharCustomize	= NULL;
AgcmUICustomizing		*g_pcsAgcmUICustomizing	= NULL;

AgcmUISystemMessage		*g_pcsAgcmUISystemMessage = NULL;

AgcmUITips				*g_pcsAgcmUITips		= NULL;
AgcmUIHelp				*g_pcsAgcmUIHelp		= NULL;

AgpmBillInfo			*g_pcsAgpmBillInfo		= NULL;
AgcmBillInfo			*g_pcsAgcmBillInfo		= NULL;

AgpmPathFind			*g_pcsAgpmPathFind = NULL;

AgpmSystemMessage		*g_pcsAgpmSystemMessage = NULL;
AgcmAmbOcclMap			*g_pcsAgcmAmbOcclMap = NULL;

AgpmEventPointLight		*g_pcsAgpmEventPointLight = NULL;
AgcmEventPointLight		*g_pcsAgcmEventPointLight = NULL;

AgpmScript				*g_pcsAgpmScript = NULL;

AgpmCashMall			*g_pcsAgpmCashMall		= NULL;
AgcmCashMall			*g_pcsAgcmCashMall		= NULL;
AgcmUICashMall			*g_pcsAgcmUICashMall	= NULL;
AgcmUICashInven			*g_pcsAgcmUICashInven	= NULL;

#ifdef _AREA_TAIWAN_
AgcmUICouponBox			*g_pcsAgcmUICouponBox	= NULL;
#endif

AgpmMailBox				*g_pcsAgpmMailBox		= NULL;
AgcmUIMailBox			*g_pcsAgcmUIMailBox		= NULL;

AgpmReturnToLogin		*g_pcsAgpmReturnToLogin	= NULL;
AgcmReturnToLogin		*g_pcsAgcmReturnToLogin	= NULL;

AgpmWantedCriminal		*g_pcsAgpmWantedCriminal = NULL;
AgcmUIWantedCriminal	*g_pcsAgcmUIWantedCriminal = NULL;

AgpmStartupEncryption	*g_pcsAgpmStartupEncryption = NULL;
AgcmStartupEncryption	*g_pcsAgcmStartupEncryption = NULL;

AgpmSiegeWar			*g_pcsAgpmSiegeWar		= NULL;
AgpmEventSiegeWarNPC	*g_pcsAgpmEventSiegeWarNPC	= NULL;
AgcmSiegeWar			*g_pcsAgcmSiegeWar		= NULL;
AgcmEventSiegeWarNPC	*g_pcsAgcmEventSiegeWarNPC	= NULL;
AgcmUISiegeWar			*g_pcsAgcmUISiegeWar	= NULL;

AgpmGuildWarehouse		*g_pcsAgpmGuildWarehouse = NULL;
AgcmUIGuildWarehouse	*g_pcsAgcmUIGuildWarehouse = NULL;

//�߱��� ���� ����̳� ���� �߱��� �Ⱦ���. tj@081013
//AgpmGamble				*g_pcsAgpmGamble = NULL;

AgcmUISearch			*g_pcsAgcmUISearch		= NULL;
AgpmSearch				*g_pcsAgpmSearch		= NULL;

AgcmUIHotkey			*g_pcsAgcmUIHotkey = NULL;

AgcmUISocialAction		*g_pcsAgcmUISocialAction = NULL;


AgpmTax					*g_pcsAgpmTax = NULL;
AgcmUITax				*g_pcsAgcmUITax = NULL;

AgpmCasper				*g_pcsAgpmCasper	= NULL;

AgpmArchlord			*g_pcsAgpmArchlord = NULL;
AgcmArchlord			*g_pcsAgcmArchlord = NULL;
AgcmUIArchlord			*g_pcsAgcmUIArchlord = NULL;

AgpmEventGacha			*g_pcsAgpmEventGacha		= NULL;
AgcmEventGacha			*g_pcsAgcmEventGacha		= NULL;
AgcmUIEventGacha		*g_pcsAgcmUIEventGacha		= NULL;

AgpmBattleGround		*g_pcsAgpmBattleGround	= NULL;
AgcmBattleGround		*g_pcsAgcmBattleGround	= NULL;

AgpmTitle				*g_pcsAgpmTitle = NULL;

AgcmImGui *				g_pcsAgcmImGui = NULL;
AgcmArena *				g_pcsAgcmArena = NULL;
AgcmBattle *			g_pcsAgcmBattle = NULL;

AgpmGameEvent *			g_pcsAgpmGameEvent = NULL;
AgcmUIGameEvent *		g_pcsAgcmUIGameEvent = NULL;

// exe, dat
void DeleteAndRenameFile(char* oldone, char* newone)
{
	if ( 0 == ::DeleteFile( oldone ) )
	{
		DWORD dwAttr = GetFileAttributes(oldone);
		
		if(dwAttr & FILE_ATTRIBUTE_READONLY)
		{
			::SetFileAttributes(oldone, dwAttr & ~FILE_ATTRIBUTE_READONLY);
			::DeleteFile( oldone );
		}
		else
		{
			Sleep(500);

			if ( 0 == ::DeleteFile( oldone ) )
			{
				Sleep(500);
				if ( 0 == ::DeleteFile( oldone ) ) {
					Sleep(500);
					::DeleteFile( oldone );
				}
			}
		}
	}

	if ( 0 != ::rename(newone, oldone) )
	{
		Sleep(0);
		::rename(newone, oldone);
	}
}


BOOL CALLBACK FindAutoProgram( HWND hWnd, LPARAM lParam )
{
	CHAR*	 szName	= (CHAR *) lParam;
	GetWindowText( hWnd, szName, 127 );
	if( strstr(szName, "ť���콺") || strstr(szName, "qmouse") || strstr(szName, "��ũ��") || strstr(szName, "ArcTerm") )
		return FALSE;

	return TRUE;
}

BOOL	SearchAutoProgram(CHAR* szWindowName)
{
	return EnumWindows( FindAutoProgram, (LPARAM)szWindowName ) ? FALSE : TRUE;
}

BOOL	KillAutoPrograms()
{
	CHAR	szName[128];
	memset( szName, 0, sizeof(char) * 128 );

	if ( SearchAutoProgram( szName ) )
	{
		char szText[256];
		sprintf( szText, "���������� ���α׷� (%s)�� �����Ͽ� ������ �����մϴ�.", szName );
		g_pEngine->MessageDialog( NULL, szText );
		RsEventHandler(rsQUITAPP, NULL);
	}

	return TRUE;
}

BOOL	__AgcmRender_DebugInfo_PreRenderCallback( PVOID pData, PVOID pClass, PVOID pCustData )
{
	g_cMyEngine.m_pMainWindow->DrawGeometryGrid();
	return TRUE;
}

MyEngine::MyEngine() : CMyException( this )
{
	m_bEmulationFullscreen =  (g_eServiceArea == AP_SERVICE_AREA_CHINA || g_eServiceArea == AP_SERVICE_AREA_TAIWAN) ? TRUE : FALSE;	//�߱��� IME������ ���� Ǯ��ũ��

	// ����׸޽����� �ʹ� ���Ƽ� �ʿ��Ҷ� �Ѽ� ������ �մϴ�. by ������
	g_bUseTrace = FALSE;

	PatchPatcher();
	SetupLanguage();
	KillAutoPrograms();

	m_pMainWindow	= new MainWindow( *this );
	m_hCursor		= NULL;
	m_szLoginServer = NULL;

	// Lua Glue ��� ���..
	AuLua * pLua = AuLua::GetPointer();
	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction( MVLuaGlue[i].name, MVLuaGlue[i].func );
	}

	#ifdef _USE_HACKSHIELD_
	// ������ (2005-04-20 ���� 11:23:20) : 
	// HackShield ���� �ڵ� �ּ� 
	int nResult;
	nResult = _AhnHS_Initialize(
		"EHSvc.dll",
		AhnHS_Callback,
		2901,
		"46300B2E4F216FE0",
		AHNHS_CHKOPT_SPEEDHACK
		//| AHNHS_CHKOPT_READWRITEPROCESSMEMORY
		//| AHNHS_CHKOPT_KDTARCER
		//| AHNHS_CHKOPT_OPENPROCESS
		| AHNHS_CHKOPT_AUTOMOUSE
		| AHNHS_CHKOPT_MESSAGEHOOK
		//| AHNHS_CHKOPT_PROCESSSCAN
		//| AHNHS_CHKOPT_HSMODULE_CHANGE
		//| AHNHS_USE_LOG_FILE
		,
		AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL );

	switch( nResult )
	{
	case HS_ERR_OK:
		// no error

		break;
	default:
		// Error!
		::MessageBox( NULL , "HShield �ʱ�ȭ �������� ������ �߻��߽��ϴ�. Ŭ���̾�Ʈ�� ���� �����մϴ�." , "Archlord" ,  MB_ICONWARNING );
		ExitProcess( 1 );
		break;
	}

	_AhnHS_StartService	();
	#endif// _USE_HACKSHIELD_

#ifdef _AREA_JAPAN_
	g_hgpshelp = LoadLibrary(_T("HGPSHELP.DLL"));
#endif
	PostInit();
}

MyEngine::~MyEngine()
{
	if( m_pMainWindow )
		delete m_pMainWindow;
	if( g_pcsAgcmUIOption )
		delete g_pcsAgcmUIOption;
#ifdef _AREA_JAPAN_
	if(g_hgpshelp)
		FreeLibrary(g_hgpshelp);
#endif
}

BOOL		MyEngine::OnAttachPlugins	()	// �÷����� ���� �� ���⼭
{
    if( !RpWorldPluginAttach() )		return FALSE;
	if( !RpMatFXPluginAttach() )		return FALSE;
	if( !RpHAnimPluginAttach() )		return FALSE;
	if( !RpSkinPluginAttach() )			return FALSE;
    if( !RpCollisionPluginAttach() )	return FALSE;
	if( !RpSplinePluginAttach() )		return FALSE;
	if( !RpMorphPluginAttach() )		return FALSE;
	if( !RpMTexturePluginAttach() )		return FALSE;
	if( !RpUserDataPluginAttach() )		return FALSE;
	if( !RpLODAtomicPluginAttach() )	return FALSE;
	if( !RpUVAnimPluginAttach() )		return FALSE;
	if( !RpUVAnimDataPluginAttach() )	return FALSE;
	if( !RpMatFxExPluginAttach() )		return FALSE;
	if( !RpMaterialD3DFxPluginAttach() )return FALSE;
	if( !RpRandomPluginAttach() )		return FALSE;
	if( !RpAmbOcclMapPluginAttach() )	return FALSE;
	if( !RpPVSPluginAttach() )			return FALSE;
	if( !RpGeoTriAreaTblPluginAttach() )return FALSE;
	if( !RtAnimInitialize() )			return FALSE;
	if( !RpToonPluginAttach() )			return FALSE;

#ifdef _DPVS
	if( !RpDPVSPluginAttach() )			return FALSE;
#endif //_DPVS

	return TRUE;
}

RpWorld *	MyEngine::OnCreateWorld()
{
	SetupOption();

	AgcdUIOptionTexture	eTQCharacter	= g_pcsAgcmUIOption->GetTextureQualityCharacter();
	AgcdUIOptionTexture	eTQObject		= g_pcsAgcmUIOption->GetTextureQualityObject();
	AgcdUIOptionTexture	eTQEffect		= g_pcsAgcmUIOption->GetTextureQualityEffect();
	AgcdUIOptionTexture	eTQWorld		= g_pcsAgcmUIOption->GetTextureQualityWorld();

	CHAR	szTemp[256];

#ifndef _BIN_EXEC_
	sprintf(szTemp, "Texture\\Effect\\%s;Texture\\UI\\;Texture\\Character\\%s;Texture\\Object\\%s;Texture\\World\\%s;Texture\\UI\\Base\\;Texture\\SKILL\\;Texture\\Item\\;Texture\\Etc\\;Texture\\Minimap\\;Texture\\NotPacked\\;Texture\\AmbOccl\\",
			eTQEffect == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQEffect == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"),
			eTQCharacter == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQCharacter == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"),
			eTQObject == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQObject == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"),
			eTQWorld == AGCD_OPTION_TEXTURE_HIGH ? "" : (eTQWorld == AGCD_OPTION_TEXTURE_MEDIUM ? "Medium\\" : "Low\\"));
#else
	sprintf(szTemp, "Texture\\Effect\\%s;Texture\\UI\\;Texture\\Character\\%s;Texture\\Object\\%s;Texture\\World\\%s;Texture\\UI\\Base\\;Texture\\SKILL\\;Texture\\Item\\;Texture\\Etc\\;Texture\\Minimap\\;Texture\\NotPacked\\;Texture\\AmbOccl\\",
		"", "", "", "");
#endif

	RwChar* path = RsPathnameCreate( RWSTRING( szTemp ) );
    RwImageSetPath( path );
    RsPathnameDestroy( path );

	RpMaterialD3DFxSetSearchPath( "Effect\\" );
	return AGCMMAP_THIS->GetWorld();
}

RwCamera *	MyEngine::OnCreateCamera( RpWorld * pWorld )
{
	m_fFarClipPlane = 50000.0f;
	m_fNearClipPlane = 100.0f;

	return CreateCamera( pWorld );
}

BOOL		MyEngine::OnCreateLight		()	// ������ �����ؼ�. �� ������ �������ش�. m_pLightAmbient , m_pLightDirect
{
	ASSERT( m_pWorld );

	m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
	if( m_pLightAmbient )
	{
		float	ambiantvalue		= 0.0f;//0.4f;
		float	directionalvalue	= 0.5f;

		RwRGBAReal color = {ambiantvalue, ambiantvalue, ambiantvalue, 1.0f};

		RpLightSetColor(m_pLightAmbient, &color);
		RpLightSetFlags(m_pLightAmbient, rpLIGHTLIGHTATOMICS | rpLIGHTLIGHTWORLD);
		RpWorldAddLight(m_pWorld, m_pLightAmbient);
	}

    m_pLightDirect = RpLightCreate(rpLIGHTDIRECTIONAL);
    if( m_pLightDirect )
	{
		RwFrame *pstFrame = RwFrameCreate();
		if( !pstFrame )
		{
	        RsErrorMessage( RWSTRING( "Cannot create light frame." ) );
			return FALSE;
		}

		RpLightSetFrame( m_pLightDirect , pstFrame );

		RwV3d v = { 0.0f , 0.0f , 0.0f };
		RwFrameTranslate( pstFrame, &v, rwCOMBINEREPLACE );

		RwMatrix* pMatrix = RwFrameGetLTM( pstFrame	);
		RwV3d* pAt = RwMatrixGetAt( pMatrix );
		pAt->x	= 0.556f;
		pAt->y	= -0.658f;
		pAt->z	= 0.056f;

		RwMatrixOrthoNormalize( pMatrix,pMatrix );

		RwRGBAReal color = {(240.0f/255.0f), (202.0f/255.0f), (116.0f/255.0f), 1.0f };
		RpLightSetColor( m_pLightDirect, &color );
		RpLightSetFlags( m_pLightDirect, rpLIGHTLIGHTATOMICS | rpLIGHTLIGHTWORLD );

		RpWorldAddLight( m_pWorld, m_pLightDirect );
	}

	return TRUE;
}

BOOL MyEngine::OnRegisterModule()
{
	_set_SSE2_enable(1);

#ifndef _AREA_CHINA_
	if ( !GetNProtect().Init() )
	{
		// ���� ����..
		//::ExitProcess( 1 );
		RsEventHandler( rsQUITAPP, 0 );
		return FALSE;
	}

	GetNProtect().SetWndHandle( g_pEngine->GethWnd() );
#endif

	g_AuCircularBuffer.Init(2048 * 1024);
	g_AuCircularOutBuffer.Init(100 * 1024);
	
	RemoveDebugLogFile();	// ���� ������ �� ���� ����� ������ ������ �����.
	SetMaxConnection(10, 2048000);	// Client Socket�� ���� ������ �ִ� ������ �����Ѵ�.

	REGISTER_MODULE( g_pcsAgpmConfig		, AgpmConfig			)
	REGISTER_MODULE( g_pcsAgcmResourceLoader, AgcmResourceLoader	)
	REGISTER_MODULE( g_pcsAgpmResourceInfo	, AgpmResourceInfo		)
	REGISTER_MODULE( g_pcsApmMap			, ApmMap				)
	REGISTER_MODULE( g_pcsApmOcTree			, ApmOcTree				)
	if (g_pcsApmOcTree)
		g_pcsApmOcTree->m_bOcTreeEnable = TRUE;			// �ٸ� ���� onAddModule���� �����Ҽ� �ִ�..
	REGISTER_MODULE( g_pcsAgcmOcTree		, AgcmOcTree			)
	REGISTER_MODULE( g_pcsAgcmPostFX		, AgcmPostFX			)
	REGISTER_MODULE( g_pcsAgcmCustomizeRender, AgcmCustomizeRender	)
	REGISTER_MODULE( g_pcsAgcmRender		, AgcmRender			)
	REGISTER_MODULE( g_pcsAgcmLODManager	, AgcmLODManager		)
	REGISTER_MODULE( g_pcsAgcmFont			, AgcmFont				)
	REGISTER_MODULE( g_pcsAgcmSound			, AgcmSound				)
	REGISTER_MODULE( g_pcsAgpmFactors		, AgpmFactors			)
	REGISTER_MODULE_SINGLETON( AgcmMap )
	/*
#ifdef _AREA_GLOBAL_
	REGISTER_MODULE( g_pcsAgcmPathFinder	, AgcmPathFinder		)
#endif
	*/
	REGISTER_MODULE( g_pcsAgcmShadow		, AgcmShadow			)
	REGISTER_MODULE( g_pcsAgcmShadow2		, AgcmShadow2			)
	REGISTER_MODULE( g_pcsAgcmDynamicLightmap, AgcmDynamicLightmap	)
	REGISTER_MODULE( g_pcsAgcmShadowmap		, AgcmShadowmap	)
	REGISTER_MODULE( g_pcsApmObject			, ApmObject				)
	REGISTER_MODULE( g_pcsAgcmObject		, AgcmObject			)
	REGISTER_MODULE( g_pcsAgcmAmbOcclMap	, AgcmAmbOcclMap		)
	REGISTER_MODULE( g_pcsAgpmCharacter		, AgpmCharacter			)
	REGISTER_MODULE( g_pcsAgcmConnectManager, AgcmConnectManager	)
	REGISTER_MODULE( g_pcsAgpmScript		, AgpmScript			)
	REGISTER_MODULE( g_pcsAgpmBillInfo		, AgpmBillInfo			)
	REGISTER_MODULE( g_pcsAgpmGrid			, AgpmGrid				)
	REGISTER_MODULE( g_pcsAgpmItem			, AgpmItem				)
	REGISTER_MODULE( g_pcsAgpmUnion			, AgpmUnion				)
	REGISTER_MODULE( g_pcsAgpmParty			, AgpmParty				)
	REGISTER_MODULE( g_pcsApmEventManager	, ApmEventManager		)
	REGISTER_MODULE( g_pcsAgpmSummons		, AgpmSummons			)
	REGISTER_MODULE( g_pcsAgpmSkill			, AgpmSkill				)
	REGISTER_MODULE( g_pcsAgpmItemConvert	, AgpmItemConvert		)
	REGISTER_MODULE( g_pcsAgpmGuild, AgpmGuild		)
	REGISTER_MODULE( g_pcsAgpmPvP			, AgpmPvP				)
	REGISTER_MODULE( g_pcsAgpmUIStatus		, AgpmUIStatus			)
	REGISTER_MODULE( g_pcsAgpmOptimizedPacket2	, AgpmOptimizedPacket2	)
	REGISTER_MODULE( g_pcsAgpmSystemMessage, AgpmSystemMessage )
	REGISTER_MODULE( g_pcsAgpmReturnToLogin	, AgpmReturnToLogin		)
	REGISTER_MODULE( g_pcsAgpmArchlord,	AgpmArchlord )
	REGISTER_MODULE( g_pcsAgpmPathFind		, AgpmPathFind			)
	REGISTER_MODULE( g_pcsAgcmCharacter		, AgcmCharacter			)
	REGISTER_MODULE( g_pcsAgcmCamera2		, AgcmCamera2			)
	REGISTER_MODULE( g_pcsAgpmLogin			, AgpmLogin				)
	REGISTER_MODULE( g_pcsAgcmLogin			, AgcmLogin				)
	REGISTER_MODULE( g_pcsAgcmUIControl		, AgcmUIControl			)
	REGISTER_MODULE( g_pcsAgcmUIManager2	, AgcmUIManager2		)
	REGISTER_MODULE( g_pcsAgcmUICooldown	, AgcmUICooldown		)
	REGISTER_MODULE( g_pcsAgpmAuctionCategory, AgpmAuctionCategory	)
	REGISTER_MODULE( g_pcsAgpmAuction		, AgpmAuction			)
	REGISTER_MODULE( g_pcsAgpmRide			, AgpmRide				)
	REGISTER_MODULE( g_pcsAgpmProduct, AgpmProduct	)
	REGISTER_MODULE( g_pcsAgpmRefinery		, AgpmRefinery			)
	REGISTER_MODULE( g_pcsAgpmCombat		, AgpmCombat			)
	REGISTER_MODULE( g_pcsAgpmTimer			, AgpmTimer				)
	REGISTER_MODULE( g_pcsAgpmCashMall		, AgpmCashMall			)
	REGISTER_MODULE( g_pcsAgpmAdmin			, AgpmAdmin				)
	REGISTER_MODULE( g_pcsAgpmSiegeWar, AgpmSiegeWar )
	REGISTER_MODULE( g_pcsAgpmEventSiegeWarNPC, AgpmEventSiegeWarNPC )
	REGISTER_MODULE( g_pcsAgpmTax, AgpmTax)
	REGISTER_MODULE( g_pcsAgpmGuildWarehouse, AgpmGuildWarehouse)
	//�߱��� ���� ����̳� ���� �߱��� �Ⱦ���. tj@081013
	//if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	//	REGISTER_MODULE( g_pcsAgpmGamble, AgpmGamble)
	REGISTER_MODULE( g_pcsAgpmNatureEffect	, AgpmNatureEffect		)
	REGISTER_MODULE( g_pcsAgpmEventNature	, AgpmEventNature		)
	REGISTER_MODULE( g_pcsAgpmEventNPCDialog, AgpmEventNPCDialog	)
	REGISTER_MODULE( g_pcsAgpmEventNPCTrade	, AgpmEventNPCTrade		)
	REGISTER_MODULE( g_pcsAgpmEventItemRepair, AgpmEventItemRepair	)
	REGISTER_MODULE( g_pcsAgpmEventBank		, AgpmEventBank			)
	REGISTER_MODULE( g_pcsAgpmEventItemConvert,	AgpmEventItemConvert	)
	REGISTER_MODULE( g_pcsAgpmEventGuild, AgpmEventGuild		)
	REGISTER_MODULE( g_pcsAgpmEventSkillMaster	,	AgpmEventSkillMaster	)
	REGISTER_MODULE( g_pcsAgpmEventProduct, AgpmEventProduct	)
	REGISTER_MODULE( g_pcsAgpmEventRefinery	, AgpmEventRefinery		)
	REGISTER_MODULE( g_pcsAgpmEventQuest, AgpmEventQuest )
	REGISTER_MODULE( g_pcsAgpmEventTeleport	, AgpmEventTeleport		)
	REGISTER_MODULE( g_pcsAgpmChatting		, AgpmChatting			)
	REGISTER_MODULE( g_pcsAgpmAreaChatting	, AgpmAreaChatting		)
	REGISTER_MODULE( g_pcsAgcmItem			, AgcmItem				)
	REGISTER_MODULE( g_pcsAgcmItemConvert	, AgcmItemConvert		)
	REGISTER_MODULE( g_pcsAgcmRecruit		, AgcmRecruit			)
	REGISTER_MODULE( g_pcsAgcmPrivateTrade	, AgcmPrivateTrade		)
	REGISTER_MODULE( g_pcsAgcmParty			, AgcmParty				)
	REGISTER_MODULE( g_pcsAgcmRide			, AgcmRide				)
	REGISTER_MODULE( g_pcsAgcmProduct		, AgcmProduct			)
	REGISTER_MODULE( g_pcsAgcmRefinery		, AgcmRefinery			)
	REGISTER_MODULE( g_pcsAgcmSkill			, AgcmSkill				)
	REGISTER_MODULE( g_pcsAgcmGlyph			, AgcmGlyph				)
	REGISTER_MODULE( g_pcsAgcmCashMall		, AgcmCashMall			)
	REGISTER_MODULE( g_pcsAgcmMinimap		, AgcmMinimap			)
	REGISTER_MODULE( g_pcsAgcmSiegeWar, AgcmSiegeWar )
	REGISTER_MODULE( g_pcsAgcmNatureEffect	, AgcmNatureEffect		)
	REGISTER_MODULE( g_pcsAgcmEventManager	, AgcmEventManager		)
	REGISTER_MODULE( g_pcsAgcmArchlord		, AgcmArchlord			)
	REGISTER_MODULE( g_pcsAgcmAuction		, AgcmAuction			)
	REGISTER_MODULE( g_pcsAgcmEventItemRepair, AgcmEventItemRepair	)
	REGISTER_MODULE( g_pcsAgcmEventBank		, AgcmEventBank			)
	REGISTER_MODULE( g_pcsAgcmEventItemConvert,	AgcmEventItemConvert	)
	REGISTER_MODULE( g_pcsAgcmEventGuild, AgcmEventGuild		)
	REGISTER_MODULE( g_pcsAgcmEventSiegeWarNPC, AgcmEventSiegeWarNPC )
	REGISTER_MODULE( g_pcsAgcmEventSkillMaster	,	AgcmEventSkillMaster	)
	REGISTER_MODULE( g_pcsAgcmEventProduct, AgcmEventProduct	)
	REGISTER_MODULE( g_pcsAgcmEventRefinery	, AgcmEventRefinery		)
	REGISTER_MODULE( g_pcsAgcmEventQuest, AgcmEventQuest )
	REGISTER_MODULE( g_pcsAgcmBillInfo	, AgcmBillInfo	)
	REGISTER_MODULE( g_pcsAgcmEventTeleport	, AgcmEventTeleport		)
	REGISTER_MODULE( g_pcsAgcmEventNPCDialog, AgcmEventNPCDialog	)
	REGISTER_MODULE( g_pcsAgcmEventNPCTrade	, AgcmEventNPCTrade		)
	REGISTER_MODULE( g_pcsAgcmUVAnimation	, AgcmUVAnimation		)
	REGISTER_MODULE( g_pcsAgcmEff2			, AgcmEff2				)
	REGISTER_MODULE( g_pcsAgcmEventEffect	, AgcmEventEffect		)
	REGISTER_MODULE( g_pcsAgcmEventNature	, AgcmEventNature		)
	REGISTER_MODULE( g_pcsAgcmTextBoardMng	, AgcmTextBoardMng		)
	REGISTER_MODULE( g_pcsAgcmGuild			, AgcmGuild				)
	REGISTER_MODULE( g_pcsAgcmChatting2		, AgcmChatting2			)
	REGISTER_MODULE( g_pcsAgcmLensFlare		, AgcmLensFlare			)
	REGISTER_MODULE( g_pcsAgcmGrass			, AgcmGrass				)
	REGISTER_MODULE( g_pcsAgpmWorld, AgpmWorld		)
	REGISTER_MODULE( g_pcsAgcmWorld, AgcmWorld		)
	REGISTER_MODULE( g_pcsAgpmQuest, AgpmQuest		)
	REGISTER_MODULE( g_pcsAgcmQuest, AgcmQuest		)
	REGISTER_MODULE( g_pcsAgpmBuddy, AgpmBuddy		)
	REGISTER_MODULE( g_pcsAgpmChannel, AgpmChannel		)
	REGISTER_MODULE( g_pcsAgpmStartupEncryption, AgpmStartupEncryption	)
	REGISTER_MODULE( g_pcsAgcmReturnToLogin	, AgcmReturnToLogin		)
	REGISTER_MODULE( g_pcsAgcmUIMain		, AgcmUIMain			)
	REGISTER_MODULE( g_pcsAgcmUICharacter	, AgcmUICharacter		)
	REGISTER_MODULE( g_pcsAgcmUISplitItem	, AgcmUISplitItem		)
	REGISTER_MODULE( g_pcsAgcmUIAuction		, AgcmUIAuction			)
	REGISTER_MODULE( g_pcsAgcmUIItem		, AgcmUIItem			)
#ifdef _AREA_TAIWAN_
	REGISTER_MODULE( g_pcsAgcmUICouponBox	, AgcmUICouponBox		)
#endif
	REGISTER_MODULE( g_pcsAgcmUICashInven	, AgcmUICashInven		)
	REGISTER_MODULE( g_pcsAgcmUISkill2		, AgcmUISkill2			)
	REGISTER_MODULE( g_pcsAgcmUIChatting2	, AgcmUIChatting2		)
	REGISTER_MODULE( g_pcsAgcmUIEventItemRepair, AgcmUIEventItemRepair)
	REGISTER_MODULE( g_pcsAgcmUIEventTeleport, AgcmUIEventTeleport	)
	REGISTER_MODULE( g_pcsAgcmUIParty		, AgcmUIParty			)
	REGISTER_MODULE( g_pcsAgcmUIPartyOption	, AgcmUIPartyOption		)
	REGISTER_MODULE( g_pcsAgcmUIEventBank	, AgcmUIEventBank		)
	REGISTER_MODULE( g_pcsAgcmUIEventNPCDialog, AgcmUIEventNPCDialog)
	REGISTER_MODULE( g_pcsAgcmUIEventItemConvert,	AgcmUIEventItemConvert	)
	REGISTER_MODULE( g_pcsAgcmUIGuild,	AgcmUIGuild	)
	REGISTER_MODULE( g_pcsAgcmUIQuest2, AgcmUIQuest2 )
	REGISTER_MODULE( g_pcsAgcmUIBuddy, AgcmUIBuddy )
	REGISTER_MODULE( g_pcsAgcmUIChannel, AgcmUIChannel )
	REGISTER_MODULE( g_pcsAgcmUIPvP			, AgcmUIPvP				)
	REGISTER_MODULE( g_pcsAgcmUIProductSkill, AgcmUIProductSkill)
	REGISTER_MODULE( g_pcsAgcmUIProduct, AgcmUIProduct)
	REGISTER_MODULE( g_pcsAgcmProductMed, AgcmProductMed)
	REGISTER_MODULE( g_pcsAgcmUIRefinery	, AgcmUIRefinery		)
	REGISTER_MODULE( g_pcsAgpmRemission		, AgpmRemission			)
	REGISTER_MODULE( g_pcsAgcmUIRemission	, AgcmUIRemission		)
	REGISTER_MODULE( g_pcsAgcmUICashMall	, AgcmUICashMall		)
	REGISTER_MODULE( g_pcsAgpmMailBox		, AgpmMailBox			)
	REGISTER_MODULE( g_pcsAgcmUIMailBox		, AgcmUIMailBox			)
	REGISTER_MODULE( g_pcsAgpmWantedCriminal, AgpmWantedCriminal	)
	REGISTER_MODULE( g_pcsAgcmUIWantedCriminal, AgcmUIWantedCriminal)
	REGISTER_MODULE( g_pcsAgcmUIArchlord, AgcmUIArchlord)
	REGISTER_MODULE( g_pcsAgcmUITax			, AgcmUITax				)
	REGISTER_MODULE( g_pcsAgcmUIGuildWarehouse, AgcmUIGuildWarehouse)
	REGISTER_MODULE( g_pcsAgpmSearch, AgpmSearch					)
	REGISTER_MODULE( g_pcsAgcmUISearch, AgcmUISearch				)
	REGISTER_MODULE( g_pcsAgcmWater			, AgcmWater				)
	REGISTER_MODULE( g_pcsAgcmGlare			, AgcmGlare				)
	REGISTER_MODULE( g_pcsAgcmTuner			, AgcmTuner				)
	REGISTER_MODULE( g_pcsAgcmUIStatus		, AgcmUIStatus			)
	REGISTER_MODULE( g_pcsAgcmUIHelp				, AgcmUIHelp					)
	REGISTER_MODULE( g_pcsAgcmUITips				, AgcmUITips					)
	REGISTER_MODULE( g_pcsAgcmUISiegeWar, AgcmUISiegeWar )
	VERIFY( g_pcsAgcmUIOption->AddModule( this ) );
	REGISTER_MODULE( g_pcsAgpmEventCharCustomize	, AgpmEventCharCustomize		)
	REGISTER_MODULE( g_pcsAgpmEventPointLight		, AgpmEventPointLight			)
	REGISTER_MODULE( g_pcsAgcmEventPointLight		, AgcmEventPointLight			)
	REGISTER_MODULE( g_pcsAgcmUILogin		, AgcmUILogin			)
	REGISTER_MODULE( g_pcsAgcmTargeting		, AgcmTargeting			)
	REGISTER_MODULE( g_pcsAgcmUIDebugInfo	, AgcmUIDebugInfo		)
	REGISTER_MODULE( g_pcsAgcmAdmin			, AgcmAdmin				)
	REGISTER_MODULE( g_pcsAgcmUIConsole		, AgcmUIConsole			)
	REGISTER_MODULE( g_pcsAgcmEventCharCustomize	, AgcmEventCharCustomize		)
	REGISTER_MODULE( g_pcsAgcmUICustomizing			, AgcmUICustomizing				)
	REGISTER_MODULE( g_pcsAgcmUISystemMessage		, AgcmUISystemMessage			)
	REGISTER_MODULE( g_pcsAgcmStartupEncryption		, AgcmStartupEncryption			)
	REGISTER_MODULE( g_pcsAgcmUIHotkey, AgcmUIHotkey );
	REGISTER_MODULE( g_pcsAgcmUISocialAction, AgcmUISocialAction );
	REGISTER_MODULE( g_pcsAgpmCasper, AgpmCasper	);
	REGISTER_MODULE( g_pcsAgpmEventGacha, AgpmEventGacha	);
	REGISTER_MODULE( g_pcsAgcmEventGacha, AgcmEventGacha	);
	REGISTER_MODULE( g_pcsAgcmUIEventGacha, AgcmUIEventGacha	);
	REGISTER_MODULE( g_pcsAgpmBattleGround, AgpmBattleGround	);
	REGISTER_MODULE( g_pcsAgcmBattleGround, AgcmBattleGround	);
	REGISTER_MODULE( g_pcsAgpmTitle, AgpmTitle );
	REGISTER_MODULE(g_pcsAgcmImGui, AgcmImGui);
	REGISTER_MODULE(g_pcsAgcmArena, AgcmArena);
	REGISTER_MODULE(g_pcsAgcmBattle, AgcmBattle);
	REGISTER_MODULE(g_pcsAgpmGameEvent, AgpmGameEvent);
	REGISTER_MODULE(g_pcsAgcmUIGameEvent, AgcmUIGameEvent);
	this->m_pcsAgcmImGui = g_pcsAgcmImGui;

	AuPackingManager*	pPackingManager = 	AuPackingManager::MakeInstance();
	
	BOOL	bUsePathSearch = FALSE;
#ifdef _DEBUG
	UINT32	uStart = ::GetTickCount();
#endif

	if( bUsePathSearch )
	{
		// ������ (2005-05-02 ���� 3:34:36) : 
		// ���丮�� ���ýú��ϰ� �˻���.
		pPackingManager->LoadReferenceFile(NULL);			// NULL�̸� ���� ���丮�� ����Ѵ�..
	}
	else
	{
		// ������ (2005-05-02 ���� 3:34:47) : 
		// ������ ���丮�� ���۷����� �о����.

		// ���� ���丮 ���..
		char	strCurrentFolder[255];
		GetCurrentDirectory( sizeof(strCurrentFolder), strCurrentFolder );

		char	strPath[ 1024 ];

		#define	SETDATADIRECTORY( x )										\
			sprintf( strPath , "%s\\%s" , strCurrentFolder , x );			\
			pPackingManager->LoadReferenceFile( strPath , true , false )

		SETDATADIRECTORY( "character"					);
		SETDATADIRECTORY( "character\\defaulthead\\face");
		SETDATADIRECTORY( "character\\defaulthead\\hair");
		SETDATADIRECTORY( "character\\animation"		);
		SETDATADIRECTORY( "effect\\animation"			);
		SETDATADIRECTORY( "effect\\clump"				);
		SETDATADIRECTORY( "ini\\itemtemplate"			);
		SETDATADIRECTORY( "object"						);
		SETDATADIRECTORY( "object\\animation"			);
		SETDATADIRECTORY( "sound\\ui"					);
		SETDATADIRECTORY( "texture\\character"			);
		SETDATADIRECTORY( "texture\\character\\low"		);
		SETDATADIRECTORY( "texture\\character\\medium"	);
		SETDATADIRECTORY( "texture\\effect"				);
		SETDATADIRECTORY( "texture\\effect\\low"		);
		SETDATADIRECTORY( "texture\\effect\\medium"		);
		SETDATADIRECTORY( "texture\\etc"				);
		SETDATADIRECTORY( "texture\\item"				);
		SETDATADIRECTORY( "texture\\minimap"			);
		SETDATADIRECTORY( "texture\\object"				);
		SETDATADIRECTORY( "texture\\object\\low"		);
		SETDATADIRECTORY( "texture\\object\\medium"		);
		SETDATADIRECTORY( "texture\\skill"				);
		SETDATADIRECTORY( "texture\\ui"					);
		SETDATADIRECTORY( "texture\\ui\\base"			);
		SETDATADIRECTORY( "texture\\ui_type1"			);
		SETDATADIRECTORY( "texture\\ui_type1\\base"		);
		SETDATADIRECTORY( "texture\\world"				);
		SETDATADIRECTORY( "texture\\world\\low"			);
		SETDATADIRECTORY( "texture\\world\\medium"		);
		SETDATADIRECTORY( "texture\\worldmap"			);
		SETDATADIRECTORY( "texture\\worldmap\\type01"	);
		SETDATADIRECTORY( "world\\grass"				);
		SETDATADIRECTORY( "world\\octree"				);
		SETDATADIRECTORY( "world\\water"				);
	}

	#ifdef _DEBUG
	{
		char str[ 256 ];
		sprintf( str , "LoadReferenceFile �۾� : %.1f �� �ɸ�\n" , ( FLOAT )( ::GetTickCount() - uStart ) / 1000.0f );
		OutputDebugString( str );
	}
	#endif

	pPackingManager->SetFilePointer();

	g_pcsAgcmSound->SetReadFromPack( TRUE );

	if (g_pcsAgcmUIManager2)
	{
		g_pcsAgcmUIManager2->SetMainWindow( m_pMainWindow );
		g_pcsAgcmUIManager2->SetSoundPath( "Sound\\UI\\" );
		g_pcsAgcmUIManager2->SetTexturePath( "Texture\\UI\\" );

#ifdef _AREA_CHINA_
if (g_pcsAgcmUIOption->m_iScreenWidth == 1280)
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1280_1024 );
		else if (g_pcsAgcmUIOption->m_iScreenWidth == 1600)
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1600_1200 );
		else
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1024_768 );
	#else
		if (g_pcsAgcmUIOption->m_iScreenHeight >= 1200 && g_pcsAgcmUIOption->m_iScreenWidth >= 1600 )
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1600_1200 );
		else if (g_pcsAgcmUIOption->m_iScreenHeight >= 1024 && g_pcsAgcmUIOption->m_iScreenHeight < 1200)
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1280_1024 );
		else
			g_pcsAgcmUIManager2->SetUIMode( AGCDUI_MODE_1024_768 );
	#endif

		g_pcsAgcmUIManager2->SetMaxUIMessage(1000);
	}

	if (g_pcsAgpmResourceInfo)
	{
		switch( g_eServiceArea )
		{
		case AP_SERVICE_AREA_KOREA :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 12 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 04 );
			}
			break;

#ifdef _AREA_GLOBAL_
		case AP_SERVICE_AREA_GLOBAL :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion(13);
				g_pcsAgpmResourceInfo->SetMinorVersion(0);
			}
		break;
#endif

		/*
		case AP_SERVICE_AREA_WESTERN :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 12 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 02 );
			}
			break;
		*/

		case AP_SERVICE_AREA_JAPAN :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 19 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 0 );
			}
			break;

		case AP_SERVICE_AREA_CHINA :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 31 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 00 );
			}
			break;

		case AP_SERVICE_AREA_TAIWAN :
			{
				g_pcsAgpmResourceInfo->SetMajorVersion( 10 );
				g_pcsAgpmResourceInfo->SetMinorVersion( 1 );
			}
			break;
		}
	}

	if (g_pcsAgcmRender)
	{
		g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame(m_pCamera));		// modify by gemani
		g_pcsAgcmRender->SetWorldCamera( m_pWorld, m_pCamera);
		g_pcsAgcmRender->SetLight( m_pLightAmbient, m_pLightDirect );

		g_pcsAgcmRender->SetCallbackPostRenderOcTreeClear	( __AgcmRender_DebugInfo_PreRenderCallback , this );
	}

	if (g_pcsAgcmWater)
	{
		g_pcsAgcmWater->SetWorldCamera( m_pCamera );
	}

	if (g_pcsApmMap)
	{
		g_pcsApmMap->SetLoadingMode( TRUE, FALSE );
		g_pcsApmMap->SetAutoLoadData( TRUE );

		if( !g_pcsApmMap->Init( NULL ) )
			return FALSE;
	}

	if (AGCMMAP_THIS)
	{
		// ������ (2003-12-09 ���� 2:51:36) : �ݸ��� �ݺ��߰�!
		AGCMMAP_THIS->SetCollisionAtomicCallback( AgcmObject::CBGetCollisionAtomic );
		AGCMMAP_THIS->SetCollisionAtomicCallbackFromAtomic( AgcmObject::CBGetCollisionAtomicFromAtomic );

		AGCMMAP_THIS->SetAutoLoadRange( 8, 6, 2 );

		char strClientMapDataDirectory[ 1024 ];
		GetCurrentDirectory( 1024, strClientMapDataDirectory );
		AGCMMAP_THIS->SetDataPath				( strClientMapDataDirectory );

		AGCMMAP_THIS->InitMapClientModule(m_pWorld);

		AGCMMAP_THIS->SetCamera( m_pCamera );

		AGCMMAP_THIS->SetTexDict( "Texture\\Tiles.txd" );

#ifdef _BIN_EXEC_
		AGCMMAP_THIS->SetTexturePath( "Texture\\World\\" );
#else
		if (g_pcsAgcmUIOption->GetTextureQualityWorld() == AGCD_OPTION_TEXTURE_MEDIUM)
			AGCMMAP_THIS->	SetTexturePath			( "Texture\\World\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityWorld() == AGCD_OPTION_TEXTURE_LOW)
			AGCMMAP_THIS->	SetTexturePath			( "Texture\\World\\Low\\" );
		else
			AGCMMAP_THIS->	SetTexturePath			( "Texture\\World\\" );
#endif
	}

	if (g_pcsApmObject)
	{
		g_pcsApmObject->		SetMaxObjectTemplate	( 10000							);
		g_pcsApmObject->		SetMaxObject			( 10000							);
		g_pcsApmObject->		SetMaxObjectRemove		( 7000							);
	}

	if (g_pcsAgcmObject)
	{
		g_pcsAgcmObject->		SetClumpPath			( "Object\\"					);
		g_pcsAgcmObject->		SetAnimationPath		( "Object\\Animation\\"		);
		g_pcsAgcmObject->		SetMaxAnimation			( 500							);
		g_pcsAgcmObject->		SetSetupObject			( TRUE			, FALSE			);

		g_pcsAgcmObject->		SetTexDictFile			( "Texture\\Object.txd"		);

#ifdef _BIN_EXEC_
		g_pcsAgcmObject->SetTexturePath( "Texture\\Object\\" );
#else
		if (g_pcsAgcmUIOption->GetTextureQualityObject() == AGCD_OPTION_TEXTURE_MEDIUM)
			g_pcsAgcmObject->	SetTexturePath			( "Texture\\Object\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityObject() == AGCD_OPTION_TEXTURE_LOW)
			g_pcsAgcmObject->	SetTexturePath			( "Texture\\Object\\Low\\" );
		else
			g_pcsAgcmObject->	SetTexturePath			( "Texture\\Object\\" );
#endif

#ifndef _BIN_EXEC_
		g_pcsAgcmObject->AddFlags(E_AGCM_OBJECT_FLAGS_EXPORT);
#endif
	}

	if (g_pcsAgpmCharacter)
	{
		g_pcsAgpmCharacter->	SetMaxCharacterTemplate	( 3000							);
		g_pcsAgpmCharacter->	SetMaxCharacter			( 1500							);
		g_pcsAgpmCharacter->	SetMaxCharacterRemove	( 1000							);
		g_pcsAgpmCharacter->	SetProcessMoveFollowInterval	( 200					);
		#ifndef _AREA_CHINA_
		g_pcsAgpmCharacter->SetCallbackGameguardAuth( CBGameguardAuth, NULL );
		#endif
	}

	if (g_pcsAgcmCharacter)
	{
		g_pcsAgcmCharacter->	SetMaxAnimations		( 20000							);

		g_pcsAgcmCharacter->	SetClumpPath			( "Character\\"				);
		g_pcsAgcmCharacter->	SetAnimationPath		( "Character\\Animation\\"	);
		g_pcsAgcmCharacter->	SetSendMoveDelay		( 500							);

		g_pcsAgcmCharacter->	SetMaxIdleEvent			( 500							);

		g_pcsAgcmCharacter->	SetTexDictFile			( "Texture\\Character.txd"	);

#ifdef _BIN_EXEC_
		g_pcsAgcmCharacter->	SetTexturePath			( "Texture\\Character\\" );
#else
		if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_MEDIUM)
			g_pcsAgcmCharacter->	SetTexturePath			( "Texture\\Character\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_LOW)
			g_pcsAgcmCharacter->	SetTexturePath			( "Texture\\Character\\Low\\" );
		else
			g_pcsAgcmCharacter->	SetTexturePath			( "Texture\\Character\\" );
#endif

#ifndef _BIN_EXEC_
		g_pcsAgcmCharacter->AddFlags(E_AGCM_CHAR_FLAGS_EXPORT);
#endif
		
		g_pcsAgcmCharacter->SetCallbackSettingCharacterOK(CBGetMyCharacterName, NULL);
	}

	if (g_pcsAgpmItem)
	{
		g_pcsAgpmItem->		SetMaxItem				( 3000							);
		g_pcsAgpmItem->		SetMaxItemRemove		( 3000							);
		g_pcsAgpmItem->		SetCompareTemplate();
	}

	if (g_pcsAgcmItem)
	{
		g_pcsAgcmItem->		SetClumpPath			( "Character\\"				);
		g_pcsAgcmItem->		SetMaxItemClump			( 2000							);

		g_pcsAgcmItem->		SetTexDictFile			( "Texture\\Item.txd"		);

		g_pcsAgcmItem->		SetIconTexturePath		( "Texture\\Item\\"			);

#ifdef _BIN_EXEC_
		g_pcsAgcmItem->SetTexturePath( "Texture\\Character\\" );
#else
		if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_MEDIUM)
			g_pcsAgcmItem->	SetTexturePath			( "Texture\\Character\\Medium\\" );
		else if (g_pcsAgcmUIOption->GetTextureQualityCharacter() == AGCD_OPTION_TEXTURE_LOW)
			g_pcsAgcmItem->	SetTexturePath			( "Texture\\Character\\Low\\" );
		else
			g_pcsAgcmItem->	SetTexturePath			( "Texture\\Character\\" );
#endif

		g_pcsAgcmItem->AddFlags(E_AGCM_ITEM_FLAGS_EXPORT);
	}

	if (g_pcsAgpmParty)
	{
		g_pcsAgpmParty->		SetMaxParty(1);
	}

	if (g_pcsAgpmSkill)
	{
		g_pcsAgpmSkill->		SetMaxSkill				( 2000							);
		g_pcsAgpmSkill->		SetMaxSkillTemplate		( 3000							);
		g_pcsAgpmSkill->		SetMaxSkillSpecializeTemplate	( 10					);
		g_pcsAgpmSkill->		SetMaxSkillTooltipTemplate	( 2000						);
	}

	if (g_pcsAgcmSkill)
	{
		g_pcsAgcmSkill->		SetMaxIdleEvent			( 500							);
		g_pcsAgcmSkill->		SetIconTexturePath		( "Texture\\Skill\\"			);
	}

	if (g_pcsAgpmChannel)
	{
		g_pcsAgpmChannel->SetMaxChannel(10);
	}

	if (g_pcsAgcmGuild)
	{
		g_pcsAgcmGuild->		SetTexturePath			( "Texture\\UI\\Base\\"		);
	}

	if (g_pcsAgpmProduct)
	{
		g_pcsAgpmProduct->SetMaxSkillFactor(10);
		g_pcsAgpmProduct->SetMaxComposeTemplate(300);
	}
		
	if (g_pcsAgpmRefinery)
	{
		g_pcsAgpmRefinery->SetMaxRefineItem(10000);
	}

	if (g_pcsAgpmAuction)
	{
		g_pcsAgpmAuction->SetMaxSales(10);
	}

	if(g_pcsAgpmGuild)
	{
		g_pcsAgpmGuild->SetMaxGuild(5);		// 1���δ� ������ ���� ���� �ִ� 5������
	}

	if (g_pcsAgpmWorld)
	{
		g_pcsAgpmWorld->SetMaxWorld(200);
	}

	if (g_pcsAgcmResourceLoader)
	{
		g_pcsAgcmResourceLoader->SetTexDictPath("Texture\\Object\\");

		if (!g_pcsAgcmResourceLoader->AddDefaultTexturePath("Texture\\Etc\\"))
			return FALSE;

		if (!g_pcsAgcmResourceLoader->AddDefaultTexturePath("Texture\\UI\\"))
			return FALSE;

		if (!g_pcsAgcmResourceLoader->AddDefaultTexturePath("Texture\\UI\\Base\\"))
			return FALSE;
	}

	if( g_pcsAgcmCustomizeRender ) {
		g_pcsAgcmCustomizeRender->SetWorld( m_pWorld );
	}

	if (g_pcsAgcmEff2)
	{
		g_pcsAgcmEff2->SetGlobalVariable( NULL , NULL, m_pWorld, m_pCamera);

#ifdef _DEBUG
		AuAutoFile	fp( "eorb.txt", "rt" );
		if( fp )
		{
			AgcdEffGlobal::bGetInst().bFlagOff(E_GFLAG_USEPACK);
		}
#endif
	}

	if (g_pcsAgpmEventNature)
	{
		g_pcsAgpmEventNature->AddDefaultSkySet();
		g_pcsAgpmEventNature->SetSpeedRate( 5 );
		g_pcsAgpmEventNature->StartTimer();
	}

	if (g_pcsApmEventManager)
	{
		g_pcsApmEventManager->	SetMaxEvent		( 1000		);
	}

	if (g_pcsAgpmEventTeleport)
	{
		g_pcsAgpmEventTeleport->SetMaxTeleportPoint(300);
		g_pcsAgpmEventTeleport->SetMaxTeleportGroup(300);
	}

	if (g_pcsAgcmEventEffect)
	{
		g_pcsAgcmEventEffect->SetSoundDataNum(1000);
	}

	if (g_pcsAgcmGlyph)
	{
		g_pcsAgcmGlyph->SetWorldCamera(m_pCamera);
	}

	if (g_pcsAgcmTargeting)
	{
		g_pcsAgcmTargeting->SetRpWorld(m_pWorld);
		g_pcsAgcmTargeting->SetRwCamera(m_pCamera);
	}

	if (g_pcsAgpmQuest)
	{
		g_pcsAgpmQuest->SetMaxTemplateCount(AGPDQUEST_MAX_QUEST_TEMPLATE * 2);
		g_pcsAgpmQuest->SetMaxGroupCount(AGPDQUEST_MAX_QUEST * 2);
	}
	
	if (g_pcsAgpmMailBox)
	{
		g_pcsAgpmMailBox->SetMaxMail(50);
	}

	if (g_pcsAgcmLogin)
	{
		g_pcsAgcmLogin->SetMainWindow( m_pMainWindow );
		if (m_szLoginServer)
			g_pcsAgcmLogin->SetLoginServer(m_szLoginServer);
		#if defined( _AREA_JAPAN_ ) && !defined( _DEBUG )
		g_pcsAgcmLogin->SetCallbackCharacterName( CBGetMyCharacterName , NULL );
		#endif
	}

	if (g_pcsAgcmUITips)
	{
		g_pcsAgcmUITips->SetTexturePath("Texture\\UI\\Base\\");
	}

	if (g_pcsAgpmCashMall)
		g_pcsAgpmCashMall->SetMaxProduct(200);

	m_cAcuIMDraw.SetWorldCamera(m_pCamera);
	g_pcsAgcmOcTree->SetRpWorld(m_pWorld);

	AgcmTitle* pcmTitle = GetAgcmTitle();
	if( pcmTitle )
	{
		pcmTitle->OnInitialize();
	}

	// Device check�� shader����
	m_cAcuDeviceCheck.InitMyDevice();

	m_cAcuParticleDraw.Init();
	m_cAcuParticleDraw.SetWorldCamera( m_pCamera );

	RpMTextureEnableLinearFog(TRUE);
	RpMTextureCreateShaders();

	// ��� ����� �غ�� �������� �ٽ� �ѹ� �ɼ��� �ε�������.. ���� ������ �ʹ� ���� ����� ����..
	g_pcsAgcmUIOption->InitGameOption();
	if (!AgcdUIOption::Init())
		return FALSE;
	return TRUE;
}

BOOL MyEngine::OnPostInitialize()
{
	// 2004.12.7 ���� size(�� 12.0mb)
	VERIFY(g_pcsAgpmSkill->InitMemoryPool(0,100,"AgpmSkill(1)"));		// 216 byte * 100 =			  21600
	VERIFY(g_pcsAgpmItem->InitMemoryPool(0,800,"AgpmItem(1)"));			// 1008 byte * 800 =		 806400
	VERIFY(g_pcsApmObject->InitMemoryPool(0,3500,"ApmObject(1)"));		// 1340 byte * 3500 =		4690000
	VERIFY(g_pcsApmMap->InitMemoryPool(0,600,"ApmMap(1)"));				// 836 byte * 2500 =		2090000
	VERIFY(g_pcsAgpmCharacter->InitMemoryPool(0, 1000,"AgpmCharacter(1)"));	// 8296	byte * 300 =	2488800
	
	//. 2006. 2. 14. Nonstopdj
	//. ApmOctree ����size�� �ø�. 32000 -> 44000
	VERIFY(g_pcsApmOcTree->InitMemoryPool(0,44000,"ApmOcTree(1)"));			// 80 byte * 44000(rootlist ����)
						//			(�Ѱ��� rootlist�� ������ �ִ� �ִ� node �� 64..)) =			2560000
	return	TRUE;
}

BOOL MyEngine::OnInit()
{
	m_cAcuFrameMemory.InitManager(1u << 24); // 16MB

	myfirewall::On();

	m_hCursor = ::LoadCursor( PsGlobal.instance , "CURSOR_M1_16.CUR" );

#ifdef _AREA_JAPAN_

	char* cmdLine = GetCommandLine();
	if ( false == g_jAuth.Init( cmdLine ) ) return FALSE;

	if ( false == g_jGameInfo.Init( g_jAuth.GetGameString(), g_jAuth.IsRealService() ) )
	{
		if ( false == g_jAuth.IsRealService() )
			MessageBox( NULL, "HanGameInfoInit Fail", "Warning", MB_OK );
	}

#elif _AREA_KOREA_

	char* cmdLine = GetCommandLine();
	if( !g_cHanAuthKor.Initialize( TRUE, cmdLine, CAuHangAuth::eHangAuthClient ) )
	{
		char* szString = "2007.11.22 ���� ��ũ�ε尡 [�Ѱ���]���� ���� ��ȯ�Ǿ����ϴ�.\n\
��ũ�ε� ������ ����Ȩ�������� ���� �����մϴ�.\n\
����, ���� �Ѱ��� ��ȯ�� ���� ������ �в����� �ݵ�� '�Ѱ��� ��ȯ ��û'�� ���ּž� ��ũ�ε� ������ �����մϴ�.";

		if( MessageBox( NULL, szString, "��ũ�ε�", MB_OK ) )
			::ShellExecute( NULL, "open", "http://archlord.hangame.com/", NULL, NULL, SW_SHOW );

		return FALSE;
	}

#else
#ifndef _AREA_CHINA_
	if( !AuthManager().init( GetCommandLine() ) )
	{
		AuthManager().error_handling();
		return FALSE;
	}
	#endif

#endif

	return AgcEngine::OnInit();
}

void	MyEngine::OnTerminate()
{
	// �켱 �ε� �����带 ���߰� �Ѵ�.
	g_pcsAgcmResourceLoader->OnPreDestroy();

	INT16	nSize;
	PVOID	pvPacket = MakeSPClientExit(&nSize);

	//@{ 2006/07/31 burumal
	AuExcelTxtLibHelper::GetSingletonPtr()->SaveIniFile();
	//@}

#ifdef _AREA_JAPAN_
	// �Ѱ��� ��ǥ ���� ���. ������ ������ ������ �����Ѵ�.
	g_jGameInfo.ExitGame( g_jAuth.GetMemberID() );
#elif _AREA_KOREA_
	// �Ѱ��� ��ǥ ���� ���. ������ ������ ������ �����Ѵ�.
	g_cHanAuthKor.Destory();
#else
	AuthManager().final_process();
#endif // _AREA_JAPAN_

	g_pcsAgcmSound->OnDestroy();
	g_pcsAgcmUIOption->OnDestroy();

#ifdef _DEBUG
	ApMemoryManager::GetInstance().ReportMemoryInfo();
#endif

	if (pvPacket)
	{
		SendPacket(pvPacket, nSize);

		FreeSystemPacket(pvPacket);
	}

	if (g_pcsAgcmResourceLoader)
		g_pcsAgcmResourceLoader->m_bForceImmediate = TRUE;

	myfirewall::Off();

	m_cAcuFrameMemory.Release();

	m_cAcuParticleDraw.Release();

#ifdef _AP_MEMORY_TRACKER_
	ApMemoryTracker::GetInstance().ReportLeaks(REPORT_MODE_FILE);
	ApMemoryTracker::GetInstance().DestroyInstance();
#endif

	ApMemoryManager::GetInstance().DestroyInstance();

	RsRwTerminate();
	//RwEngineClose();
	//���� ���� �� �ϴ°���??
	//�̰Ŷ����� ��⿡�� ���� �޸� ���� ���� ���ϰ� ���� �Ǵ� ����;;
//	::ExitProcess( 0 );
}

VOID	MyEngine::OnEndIdle()
{
	static UINT32	s_ulTick	= GetClockCount();
	UINT32			ulCurTick	= GetClockCount();

	if (ulCurTick - s_ulTick >= 5000)
	{
		KillAutoPrograms();
		s_ulTick = ulCurTick;
	}

	m_cAcuFrameMemory.Clear();

#ifdef _AREA_KOREA_
	g_cHanAuthKor.Refresh();
#endif
}

VOID MyEngine::OnWorldRender()
{
	m_cAcuParticleDraw.CameraMatrixUpdate();
	g_pcsAgcmRender->Render();
}

BOOL MyEngine::OnRenderPreCameraUpdate()
{
	if (g_pcsAgcmShadow2)
		g_pcsAgcmShadow2->ShadowTextureRender();

	if(g_pcsAgcmPostFX)
		g_pcsAgcmPostFX->changeRenderTarget();

	return TRUE;
}

BOOL MyEngine::OnAddModule()
{
	return TRUE;
}

#define WM_ALEFDETAILCHANGE			(WM_USER + 13 )		// wParam : Rough,Load , lParam : Detail
#define WM_ALEFTIMEFLOWCHANGE		(WM_USER + 14 )		// wParam : value
#define WM_ALEFSKYTEMPLATECHANGE	(WM_USER + 15 )		// wParam : value
#define WM_ALEFPOSITIONWARP			(WM_USER + 17 )

UINT32	MyEngine::OnWindowEventHandler	(HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_ALEFPOSITIONWARP:
		{
			// �ش� ��ǥ�� ����~

			FLOAT	x , z;
			x = * ( ( FLOAT * ) & wParam );
			z = * ( ( FLOAT * ) & lParam );

			char strMessage[ 256 ];
			sprintf( strMessage , "/move %f,%f" , x , z );

			g_pcsAgcmChatting2->SendChatMessage( AGPDCHATTING_TYPE_NORMAL , strMessage , strlen( strMessage ) , NULL );
		}
		break;
	case WM_DROPFILES:
		{
			HDROP	hDropInfo = ( HDROP ) wParam;
			
			// TODO: Add your message handler code here and/or call default
			char	lpstr[ 1024 ];
			// �켱 ��� ������ �巡�� �Ǵ� �� Ȯ���Ѵ�.
			UINT dragcount = ::DragQueryFile( hDropInfo , 0xffffffff , lpstr , 1024 );

			if( dragcount > 0 )
			{
				// ��δ� �˻� �Ϸ���
				for ( int i = 0 ; i < ( int )  dragcount ; i ++ )
				{
					::DragQueryFile( hDropInfo , i , lpstr , 1024 );
					TRACE( "��ӵ� %d ��° ���� -'%s'\n" , i + 1 , lpstr );
				}

				// �� ���α׷������� ���� ó���͸� �����Ѵ�.
				::DragQueryFile( hDropInfo , 0 , lpstr , 1024 );
				
				char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
				_splitpath( lpstr , drive, dir, fname, ext );

				// �����̸� üũ..
				int nLen = 0;
				while( nLen < (int)strlen( ext ) )
				{
					ext[ nLen ] = toupper( ext[ nLen ] );
					nLen ++;
				}
			}
		}
		break;

	case WM_SETCURSOR:
		{
			if( g_pcsAgcmUIManager2 && g_pcsAgcmUIManager2->IsShowCursor() )
			{
				HCURSOR hCursor = g_pcsAgcmUIManager2->GetCurrentCursorHandle();
				::SetCursor( hCursor ? hCursor : m_hCursor );
			}
			else
				::SetCursor( NULL );
		}
		return 1;

	default:
		break; 
		
	}

	return 0;
}

BOOL MyEngine::OnCameraResize( RwRect * pRect )
{
	BOOL bOn = FALSE;
	if(g_pcsAgcmPostFX && g_pcsAgcmPostFX->IsFxOn())
	{
		g_pcsAgcmPostFX->PostFX_OFF();
		bOn = TRUE;
	}
		
	BOOL bResult = AgcEngine::OnCameraResize( pRect );

	if(g_pcsAgcmPostFX && bOn)
		g_pcsAgcmPostFX->PostFX_ON();

	return bResult;
}

// 2004.01.15. ������
BOOL MyEngine::RemoveDebugLogFile()
{
	// ���� Debug File Log �� ������ �����.
	char szPath[512];
	if(GetCurrentDirectory(512, szPath) == 0)
		return FALSE;

	strcat(szPath, "\\");
	strcat(szPath, ALEF_ERROR_FILENAME);
	
	if(_access(szPath, 0) != -1)
	{
		if(!DeleteFile(ALEF_ERROR_FILENAME))
		{
			// �б� �����̸� Ǯ�� �����.
			DWORD dwAttr = GetFileAttributes(ALEF_ERROR_FILENAME);
			if(dwAttr & FILE_ATTRIBUTE_READONLY)
			{
				SetFileAttributes(szPath, dwAttr & ~FILE_ATTRIBUTE_READONLY);
				DeleteFile(ALEF_ERROR_FILENAME);
			}
		}
	}

	// Stack Walker Debug ���ϵ� �����.
	memset(szPath, 0, 512);
	if(GetCurrentDirectory(512, szPath) == 0)
		return FALSE;

	strcat(szPath, "\\");
	strcat(szPath, ALEF_STACKWALK_FILENAME);
	
	if(_access(szPath, 0) != -1)
	{
		if(!DeleteFile(ALEF_STACKWALK_FILENAME))
		{
			// �б� �����̸� Ǯ�� �����.
			DWORD dwAttr = GetFileAttributes(ALEF_STACKWALK_FILENAME);
			if(dwAttr & FILE_ATTRIBUTE_READONLY)
			{
				SetFileAttributes(szPath, dwAttr & ~FILE_ATTRIBUTE_READONLY);
				DeleteFile(ALEF_STACKWALK_FILENAME);
			}
		}
	}

	return TRUE;
}

BOOL MyEngine::OnSelectDevice()
{
	BOOL	bSelectWindowMode = FALSE;

	FILE*	fp = fopen("iwantwindow.txt","r");
	if(fp)
	{
		bSelectWindowMode = TRUE;
		fclose(fp);
	}

	fp = fopen("fulldump.txt","r");
	if(fp)
	{
		g_eMiniDumpType	= MiniDumpWithFullMemory;
		fclose(fp);
	}
	else
	{
		g_eMiniDumpType	= MiniDumpNormal;
	}

	if( m_bEmulationFullscreen && g_pcsAgcmUIOption->m_bWindowed )
	{
		// �׳� â���� ���.
		if( !RwEngineSetSubSystem(0) )		return FALSE;
		if( !RwEngineSetVideoMode(0) )		return FALSE;

		return TRUE;
	}

#ifdef _DEBUG
	//bSelectWindowMode = TRUE;
#endif	// _DEBUG

	if( m_bEmulationFullscreen && !g_pcsAgcmUIOption->m_bWindowed )
		bSelectWindowMode = TRUE;

	if( bSelectWindowMode )		return FALSE;
	if( !RwEngineSetSubSystem( RwEngineGetCurrentSubSystem() ) )
		return FALSE;

	SetupOption();

	INT32 lScreenWidth	= g_pcsAgcmUIOption->m_iScreenWidth;
	INT32 lScreenHeight	= g_pcsAgcmUIOption->m_iScreenHeight;
	INT32 lColorDepth	= g_pcsAgcmUIOption->m_iScreenDepth;
	INT32 lVideoMode	= RwEngineGetNumVideoModes();

	RwVideoMode			stVideoModeIt;
	RwVideoMode			stVideoMode;
	INT32				lProperVideoMode = -1;

	if ( g_pcsAgcmUIOption->m_bWindowed )
	{
		for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
		{
			if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
				continue;

			if ( ((stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) == 0) && 
					stVideoModeIt.width >= lScreenWidth && 
					stVideoModeIt.height >= lScreenHeight &&
					stVideoModeIt.depth >= lColorDepth )
			{
				stVideoMode = stVideoModeIt;
				lProperVideoMode = lIndex;
				break;
			}
		}
		
		if ( lProperVideoMode == -1 )
		{
			lScreenWidth = 1024;
			lScreenHeight = 768;

			for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
			{
				if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
					continue;

				if ( ((stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) == 0) && 
					stVideoModeIt.width == lScreenWidth && 
					stVideoModeIt.height == lScreenHeight &&
					stVideoModeIt.depth >= lColorDepth )
				{
					stVideoMode = stVideoModeIt;
					lProperVideoMode = lIndex;
					break;
				}
			}
		}
	}
	else
	{
		for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
		{
			if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
				continue;
			
			if ( (stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) && 
					stVideoModeIt.width == lScreenWidth && 
					stVideoModeIt.height == lScreenHeight )
			{
				stVideoMode = stVideoModeIt;
				lProperVideoMode = lIndex;

				if (stVideoModeIt.depth == lColorDepth)
				{
					break;
				}
			}
		}

		if ( lProperVideoMode == -1 )
		{
			lScreenWidth = 1024;
			lScreenHeight = 768;

			for (int lIndex = 0; lIndex < lVideoMode; ++lIndex)
			{
				if (!RwEngineGetVideoModeInfo(&stVideoModeIt, lIndex))
					continue;

				if ( (stVideoModeIt.flags & rwVIDEOMODEEXCLUSIVE) && 
					stVideoModeIt.width == lScreenWidth && 
					stVideoModeIt.height == lScreenHeight )
				{
					stVideoMode = stVideoModeIt;
					lProperVideoMode = lIndex;

					if (stVideoModeIt.depth == lColorDepth)
					{
						break;
					}
				}
			}
		}
	}

	if( lProperVideoMode == -1 )					return FALSE;
    if( !RwEngineSetVideoMode( lProperVideoMode ) )	return FALSE;
        

	RwD3D9EngineSetRefreshRate(60);

    if (stVideoMode.flags & rwVIDEOMODEEXCLUSIVE)
    {
        RsGlobal.maximumWidth	= stVideoMode.width;
        RsGlobal.maximumHeight	= stVideoMode.height;
		PsGlobal.fullScreen		= TRUE;
    }

	return TRUE;
}

BOOL	MyEngine::OnInitialize			()
{	
	SetupOption();

	RsGlobal.maximumWidth = g_pcsAgcmUIOption->m_iScreenWidth;
	RsGlobal.maximumHeight = g_pcsAgcmUIOption->m_iScreenHeight;
	PsGlobal.framed = g_pcsAgcmUIOption->m_bFramed;
	if (!adjust_resolution())
		return FALSE;

	//if( g_eServiceArea == AP_SERVICE_AREA_CHINA )
	{
		if( m_bEmulationFullscreen )
			m_pMainWindow->ToggleFullScreenTest();
	}
	return AgcEngine::OnInitialize();
}

BOOL MyEngine::SetupOption()
{
	if( g_pcsAgcmUIOption )		return TRUE;

	g_pcsAgcmUIOption = new AgcmUIOption;

	TCHAR*	szCommandLine = GetCommandLine();

	AgcmOptionStartup eStartupOption = AGCM_OPTION_STARTUP_NORMAL;
	if (strstr(szCommandLine, PATCHCLIENT_OPTION_HIGH))
		eStartupOption = AGCM_OPTION_STARTUP_HIGH;
	else if (strstr(szCommandLine, PATCHCLIENT_OPTION_LOW))
		eStartupOption = AGCM_OPTION_STARTUP_LOW;

	BOOL	bOptionSaveLoad = strstr( szCommandLine, PATCHCLIENT_OPTION_NOSAVE ) ? FALSE : TRUE;
	TCHAR* szLoginServer = strstr(szCommandLine, PATCHCLIENT_LOGIN_SERVER);
	if( szLoginServer )
		m_szLoginServer = szLoginServer + strlen(PATCHCLIENT_LOGIN_SERVER);

	g_pcsAgcmUIOption->EnableSaveLoad( bOptionSaveLoad );
	g_pcsAgcmUIOption->SetStartMode( eStartupOption);
	g_pcsAgcmUIOption->InitGameOption();

	// �߱��϶� �׽�Ʈ�� ����� ���ϵ��� /w ���ڷ� Ŭ�� ���� â���� �����ϰ� ��.
	// ����� ������ ����Ǯ��ũ������ �߰� �Ǿ�����. ���� ���ٿ��� ���� ���� â��� ����
	// ��û������ �̰ŷ� ���� �ϸ� �ɵ�...
	if( g_eServiceArea == AP_SERVICE_AREA_CHINA )
		m_bEmulationFullscreen = strstr( szCommandLine, "/w") ? FALSE :TRUE;

	return TRUE;
}

// MyEngine::PatchPatcher
//
// ��ġ Ŭ���̾�Ʈ�� �����θ� ��ġ�ϴµ� �����������
// DOWNLOAD_PATCH_CLIENT_NAME(archlord.bak)�� �����Ѵ�.
//
// 1. PATCH_CLIENT_NAME�� TEMP_PATCH_CLIENT_NAME�� �ٲ۴�.
// 2. DOWNLOAD_PATCH_CLIENT_NAME�� PATCH_CLIENT_NAME���� �ٲ۴�.
// 3. TEMP_PATCH_CLIENT_NAME�� �����Ѵ�.
//
BOOL MyEngine::PatchPatcher()
{
	// �ٿ�ε� ���� ��ġ Ŭ���̾�Ʈ�� ������ �׳� �Ѿ��
	if ( !DoesFileExist(DOWNLOAD_PATCH_CLIENT_NAME) )
		return TRUE;

	// 98���� patch client�� ���� �������϶� �̸��� �ٲ���� ��Ȯ���� �ʴ�.
	// 2000�̻��̶�� �������� ������ �̸��� ���������ϴ�.
	MoveFile( PATCH_CLIENT_NAME, TEMP_PATCH_CLIENT_NAME );

	if (0 == MoveFile( DOWNLOAD_PATCH_CLIENT_NAME, PATCH_CLIENT_NAME ))
	{	// DOWNLOAD_PATCH_CLIENT_NAME�� PATCH_CLIENT_NAME���� �ٲٴµ� �����ϸ� ���� ����
		MoveFile( TEMP_PATCH_CLIENT_NAME, PATCH_CLIENT_NAME );
	}

	// ���α׷��� �������̿��� ������ �����Ѵٸ� ���� ��ٸ��Ŀ� �ٽ� ������ �õ��Ѵ�.
	// 2000 �̻󿡼��� �������� �����̶� ������ �����ϴ�. 98�� ���� �ڵ�
	if (0 == DeleteFile( TEMP_PATCH_CLIENT_NAME ) && GetLastError() == ERROR_ACCESS_DENIED)
	{
		Sleep(200);
		DeleteFile( TEMP_PATCH_CLIENT_NAME );	// �׷��� ���������� ������ȸ��
	}

	return TRUE;
}

VOID MyEngine::SetupLanguage()
{
	memset( m_szLanguage, 0, 128 );

	if ( g_eServiceArea != AP_SERVICE_AREA_GLOBAL )
		return;



	bool isTest = false;

	FILE* pFile = fopen( ".\\Archlordtest.ini", "r" );

	if( pFile )
	{
		isTest = true;
		fclose(pFile);
	}

	TCHAR szTemp[256] = {0,};

	//if( isTest )
	//{
	//	strcpy_s( szTemp, sizeof(szTemp), "English" );
	//}
	//else // �׽�Ʈ ������ ������ ������Ʈ������ �д´�
	{
		/*
		TCHAR			szBuffer[256];
		_stprintf( szBuffer, _T("SOFTWARE\\ArchLord") );

		HKEY	hRegKey;
#ifdef _AREA_GLOBAL_
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGIKEY_ARCHLORD, 0, KEY_READ, &hRegKey ) )
#else
		if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, szBuffer, 0, KEY_READ, &hRegKey ) )
#endif
		{
			MessageBox(NULL, "Error Open Registry !!!", "Archlord", MB_OK);
			return;
		}

		// read IP
		DWORD dwType;
		DWORD dwLength = 256;

		if( ERROR_SUCCESS != RegQueryValueEx(hRegKey,
#ifdef _AREA_GLOBAL_
			REGIKEY_LANGUAGE
#else
			_T("Language")
#endif
			, 0, &dwType, (LPBYTE) szTemp, &dwLength ) )
			return;

		RegCloseKey(hRegKey); 
		*/

		std::string language = AgcmRegistryManager::GetProperty< std::string >( REGIKEY_LANGUAGE );

		if( !language.empty() )
			strcpy_s( szTemp, sizeof(szTemp), language.c_str() );
		else
			strcpy_s( szTemp, sizeof(szTemp), "English" );
	}

	

	if (!stricmp(szTemp, "English"))
	{
		g_INIFileName = "ini\\sysstr.txt";
	}
	else if (!stricmp(szTemp, "French"))
	{
		strcpy(m_szLanguage, "fr\\");
		g_INIFileName = "ini\\fr\\sysstr.txt";
	}
	else if (!stricmp(szTemp, "German"))
	{
		strcpy(m_szLanguage, "de\\");
		g_INIFileName = "ini\\de\\sysstr.txt";
	}

	else if (!stricmp(szTemp, "Portuguese"))
	{
		strcpy(m_szLanguage, "portu\\");
		g_INIFileName = "ini\\portu\\sysstr.txt";
	}

	else if (!stricmp(szTemp, "Spanish"))
	{
		strcpy(m_szLanguage, "esp\\");
		g_INIFileName = "ini\\esp\\sysstr.txt";
	}

	
}

void	MyEngine::LuaErrorMessage( const char * pStr )
{
	#ifdef _DEBUG
	OutputDebugString( pStr );
	#endif

	if( g_pcsAgcmChatting2 )
	{
		g_pcsAgcmChatting2->AddSystemMessage( ( char * ) pStr );
	}
	else
	{
		// do nothing..
	}
}

// ���⼭ ���� ���� ��������� ó������.
void MyEngine::PostInit(void)
{
	if (AP_SERVICE_AREA_GLOBAL == g_eServiceArea)
	{
		if ( DoesFileExist("archlord_de.dat") )
			DeleteAndRenameFile("archlord_de.exe", "archlord_de.dat");

		if ( DoesFileExist("archlord_fr.dat") )
			DeleteAndRenameFile("archlord_fr.exe", "archlord_fr.dat");
	}

	::DeleteFile("ArchlordJPTest.ini");
}

BOOL		MyEngine::CBGetMyCharacterName(PVOID pvData, PVOID pClass, PVOID pCustData)
{
	#if defined( _AREA_JAPAN_ )
		GetNProtect().Send( (TCHAR*)pvData );
	#else
		GetNProtect().Send( (TCHAR*)pCustData );
	#endif

	return TRUE;
}

BOOL MyEngine::CBGameguardAuth( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if ( /*!pData || !pClass ||*/ !pCustData ) 
		return FALSE;

	GetNProtect().Auth( pCustData );

	return TRUE;
}

TeleportInfo *	MyEngine::GetTeleportPoint( INT32 nIndex )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	iter = m_mapTeleportPoint.find( nIndex );
	if( iter == m_mapTeleportPoint.end() )
	{
		return NULL;
	}
	else
	{
		// iter->first �� INT32
		// iter->second �� TeleportInfo
		return &iter->second;
	}
}

TeleportInfo *	MyEngine::GetTeleportPoint( string strName )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		if( strName == stInfo.strName )
			return &stInfo;
	}

	// ������..
	int nMatch = 0;
	TeleportInfo *	pPoint;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		if( strstr( stInfo.strName.c_str() , strName.c_str() ) )
		{
			nMatch++;
			pPoint = &stInfo;
		}
	}

	if( nMatch == 1 )
	{
		return pPoint;
	}
	else
	{
		nMatch = 0;
		// ���� �ϳ� �˻�.. (2����Ʈ)
		for( iter = m_mapTeleportPoint.begin() ;
			iter != m_mapTeleportPoint.end();
			iter++ )
		{
			INT32			nIndex	= iter->first	;
			TeleportInfo	&stInfo	= iter->second	;

			if( !strncmp( stInfo.strName.c_str() , strName.c_str() , 2 ) )
			{
				nMatch++;
				pPoint = &stInfo;
			}
		}

		if( nMatch == 1 ) return pPoint;
	}

	return NULL;
}

BOOL			MyEngine::AddTeleportPoint( INT32 nIndex , TeleportInfo &stInfo )
{
	// �ִ��͵� �����.
	m_mapTeleportPoint[ nIndex ] = stInfo;
	return TRUE;
}

INT32			MyEngine::GetTeleportEmptyIndex()
{
	for( INT32 nIndex = 1 ; nIndex < 65535 ; nIndex++ )
	{
		if( !GetTeleportPoint( nIndex ) )
		{
			return nIndex;
		}
	}

	return -1;
}

const string	_strKeyName	= "Name";
const string	_strKeyPos	= "Pos";

BOOL			MyEngine::LoadTelportPoint( string strName )
{
	AuIniManagerA	iniManager;
	iniManager.SetPath(	strName.c_str()	);

	if( iniManager.ReadFile(0, FALSE) )
	{
		m_mapTeleportPoint.clear();

		int		nSectionCount	;

		int		nKeyName	;
		int		nKeyPos		;

		nSectionCount	= iniManager.GetNumSection();	

		// �ױ��� �߰���..

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			nKeyName	= iniManager.FindKey( i , _strKeyName.c_str()	);
			nKeyPos		= iniManager.FindKey( i , _strKeyPos.c_str()	);

			int nSection = atoi( iniManager.GetSectionName( i ) );

			string	strName , strPos;
			strName	= iniManager.GetValue( i , nKeyName );
			strPos	= iniManager.GetValue( i , nKeyPos );
			
			FLOAT	x = 0.0f ,z = 0.0f;
			sscanf( strPos.c_str() , "%f,%f" , &x , &z );

			AddTeleportPoint( nSection , TeleportInfo( strName.c_str() , x , z ) );
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL			MyEngine::SaveTelportPoint( string strName )
{
	AuIniManagerA	ini;
	ini.SetPath( strName.c_str() );

	map< INT32 , TeleportInfo >::iterator	iter;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		// ���� ����.
		char	strSection[ 256 ];
		sprintf( strSection , "%d" , nIndex );
		char	strPos[ 256 ];
		sprintf( strPos , "%.0f,%.0f" , stInfo.pos.x , stInfo.pos.z );

		ini.SetValue( strSection , _strKeyName.c_str() , stInfo.strName.c_str() );
		ini.SetValue( strSection , _strKeyPos.c_str() , strPos );
	}

	ini.WriteFile(0, FALSE);

	return TRUE;
}

BOOL			MyEngine::DeleteTeleportPoint( INT32 nIndex )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	iter = m_mapTeleportPoint.find( nIndex );
	if( iter == m_mapTeleportPoint.end() )
	{
		return FALSE;
	}
	else
	{
		m_mapTeleportPoint.erase( iter );
		return TRUE;
	}
}

BOOL			MyEngine::DeleteTeleportPoint( string strName )
{
	map< INT32 , TeleportInfo >::iterator	iter;
	for( iter = m_mapTeleportPoint.begin() ;
		iter != m_mapTeleportPoint.end();
		iter++ )
	{
		INT32			nIndex	= iter->first	;
		TeleportInfo	&stInfo	= iter->second	;

		if( strName == stInfo.strName )
		{
			m_mapTeleportPoint.erase( iter );
			return TRUE;
		}
	}

	return FALSE;
}
