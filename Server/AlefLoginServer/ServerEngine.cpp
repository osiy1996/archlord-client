#include "ApBase.h"

#include <stdio.h>
#include "ServerEngine.h"
#include "ExceptionHandler.h"
#include "AgpmBattleGround.h"
#include "AgsmBattleGround.h"
#include "AuGameEnv.h"

#define	__DB2_VERSION__

#ifdef ENC_SVR_DATA
	#define ENC_SVR_DATA	1
#else
	#define ENC_SVR_DATA	0
#endif

extern char *	g_szIPBlockFileName;
extern LPCTSTR g_szOptionIPBlock;

//AuDatabaseManager2*	g_pcsAuDatabaseManager2;
ApmMap*				g_pcsApmMap;
ApmObject*			g_pcsApmObject;
AgpmFactors*		g_pcsAgpmFactors;
AgpmCharacter*		g_pcsAgpmCharacter;
AgpmAI2*			g_pcsAgpmAI2;
AgpmUnion*			g_pcsAgpmUnion;
AgpmGrid*			g_pcsAgpmGrid;
AgpmItem*			g_pcsAgpmItem;
AgpmItemConvert*	g_pcsAgpmItemConvert;
AgpmCombat*			g_pcsAgpmCombat;
//AgpmItemLog*		g_pcsAgpmItemLog;
AgpmChatting*		g_pcsAgpmChatting;
AgpmPathFind*		g_pcsAgpmPathFind;
AgpmParty*			g_pcsAgpmParty;
AgpmSummons*		g_pcsAgpmSummons;
AgpmTimer*			g_pcsAgpmTimer;
AgpmResourceInfo*	g_pcsAgpmResourceInfo;
AgpmConfig			*g_pcsAgpmConfig;			// kelovon, 20051007
AgpmSystemMessage	*g_pcsAgpmSystemMessage;	// kelovon, 20051212
AgpmProduct*		g_pcsAgpmProduct;

AgpmLog*			g_pcsAgpmLog;
AgsmLog*			g_pcsAgsmLog;

ApmEventManager*	g_pcsApmEventManager;
AgpmEventNature*	g_pcsAgpmEventNature;
AgpmEventSpawn*		g_pcsAgpmEventSpawn;
AgpmSkill*			g_pcsAgpmSkill;
AgpmShrine*			g_pcsAgpmShrine;
AgpmEventBinding*	g_pcsAgpmEventBinding;
AgpmAdmin*			g_pcsAgpmAdmin;
AgpmUIStatus*		g_pcsAgpmUIStatus;
AgpmGuild*			g_pcsAgpmGuild;
AgpmWorld*			g_pcsAgpmWorld;
//AgpmOptimizedPacket*g_pcsAgpmOptimizedPacket;
AgpmDropItem2*		g_pcsAgpmDropItem2;
AgpmQuest*			g_pcsAgpmQuest;
AgpmPvP*			g_pcsAgpmPvP;
AgpmBillInfo*		g_pcsAgpmBillInfo;
AgpmLogin*			g_pcsAgpmLogin;
//AgpmScript*			g_pcsAgpmScript;
AgpmEventCharCustomize*	g_pcsAgpmEventCharCustomize;

AgsmSystemMessage	*g_pcsAgsmSystemMessage;
AgsmMap*			g_pcsAgsmMap;
AgsmAOIFilter*		g_pcsAgsmAOIFilter;
AgsmServerManager*	g_pcsAgsmServerManager;
AgsmWorld*			g_pcsAgsmWorld;
AgsmCharacter*		g_pcsAgsmCharacter;
AgsmZoning*			g_pcsAgsmZoning;
AgsmItem*			g_pcsAgsmItem;
AgsmItemConvert*	g_pcsAgsmItemConvert;
AgsmItemManager*	g_pcsAgsmItemManager;
//AgsmItemLog*		g_pcsAgsmItemLog;
AgsmParty*			g_pcsAgsmParty;
AgsmCombat*			g_pcsAgsmCombat;
AgsmDeath*			g_pcsAgsmDeath;
AgsmSkill*			g_pcsAgsmSkill;
AgsmSkillManager*	g_pcsAgsmSkillManager;
AgsmSystemInfo*		g_pcsAgsmSystemInfo;
AgsmFactors*		g_pcsAgsmFactors;
AgsmAccountManager*	g_pcsAgsmAccountManager;
AgsmCharManager*	g_pcsAgsmCharManager;
AgsmLoginServer*	g_pcsAgsmLoginServer;
AgsmLoginClient*	g_pcsAgsmLoginClient;
AgsmLoginDB*		g_pcsAgsmLoginDB;
//AgsmConnectionSetupLogin*	g_pcsAgsmConnectionSetupLogin;
AgsmInterServerLink*g_pcsAgsmInterServerLink;
AgsmUIStatus*		g_pcsAgsmUIStatus;
AgsmChatting*		g_pcsAgsmChatting;
AgsmGlobalChatting*	g_pcsAgsmGlobalChatting;
AgsmGuild*			g_pcsAgsmGuild;
//AgsmAdminDB*		g_pcsAgsmAdminDB;
AgsmAdmin*			g_pcsAgsmAdmin;
AgsmDropItem2*		g_pcsAgsmDropItem2;
AgsmQuest*			g_pcsAgsmQuest;
//AsEchoServer*		g_pcsEchoServer;
AgsmPvP*			g_pcsAgsmPvP;
AgsmSummons*		g_pcsAgsmSummons;
AgpmEventQuest*		g_pcsAgpmEventQuest;

AgpmEventSkillMaster*	g_pcsAgpmEventSkillMaster;
AgsmEventSkillMaster*	g_pcsAgsmEventSkillMaster;

AgpmEventTeleport*	g_pcsAgpmEventTeleport;
AgsmEventTeleport*	g_pcsAgsmEventTeleport;

AgpmAuction*		g_pcsAgpmAuction;
AgsmAuction*		g_pcsAgsmAuction;

AgsmBillInfo*		g_pcsAgsmBillInfo;
AgsmGK*				g_pcsAgsmGK;

AgsmUsedCharDataPool*	g_pcsAgsmUsedCharDataPool;

AgpmReturnToLogin*	g_pcsAgpmReturnToLogin;
AgsmReturnToLogin*	g_pcsAgsmReturnToLogin;

AgpmStartupEncryption* g_pcsAgpmStartupEncryption;
AgsmStartupEncryption* g_pcsAgsmStartupEncryption;

AgsmConfig*			g_pcsAgsmConfig;

AgpmSiegeWar*		g_pcsAgpmSiegeWar;
AgsmServerStatus*	g_pcsAgsmServerStatus;

AgpmArchlord*		g_pcsAgpmArchlord;
//AgsmArchlord*		g_pcsAgsmArchlord;

AgpmBattleGround*	g_pcsAgpmBattleGround;
AgsmBattleGround*	g_pcsAgsmBattleGround;
AgpmRide*			g_pcsAgpmRide;
AgpmCashMall*		g_pcsAgpmCashMall;

AgpmTitle*			g_pcsAgpmTitle;
AgsmTitle*			g_pcsAgsmTitle;

AgsmPrivateTrade*		g_pcsAgsmPrivateTrade;//JK_�ŷ��߱���


ServerEngine::ServerEngine(GUID guidApp)
{
#ifdef _AREA_CHINA_
	m_bIPBlock	 = FALSE;
#endif
}

ServerEngine::~ServerEngine()
{
}

void ServerEngine::OpenIPInfo( bool &bUseIPBlock, bool &bOpenPCRoom )
{
	//	char			cData;

	//while( 1 )
	//{
	//	printf( "\n[Start] �����Դϴ�.\n" );
	//	printf( "[Step-1] Use IPBlock ?(y/n)" );
	//	scanf( "%c", &cData );
	//	getchar();

	//	if( cData == 'y' || cData == 'Y' )
	//	{
	//		printf( "[Step-2] Really Use IPBlock ?(y/n)" );
	//		scanf( "%c", &cData );
	//		getchar();

	//		if( cData == 'y' || cData == 'Y' )
	//		{
	//			printf( "[Step-3] ** Using IPBlock **\n" );
	//			bUseIPBlock = true;
	//			break;
	//		}
	//	}
	//	else if( cData == 'n' || cData == 'N' )
	//	{
	//		printf( "[Step-2] Not Use IPBlock ?(y/n)" );
	//		scanf( "%c", &cData );
	//		getchar();

	//		if( cData == 'y' || cData == 'Y' )
	//		{
	//			printf( "[Step-3] -- Not Using IPBlock --\n" );
	//			bUseIPBlock = false;
	//			break;
	//		}
	//	}
	//}

	if (GetCommandLine() && strstr(GetCommandLine(), g_szOptionIPBlock))
		bUseIPBlock = true;
	else
		bUseIPBlock = false;

	bOpenPCRoom	= false;

	/*
	while( 1 )
	{
		printf( "\n[Start] �����Դϴ�.\n" );
		printf( "[Step-1] PC�� IP�� ���ðڽ��ϱ�?(y/n)" );
		scanf( "%c", &cData );
		getchar();

		if( cData == 'y' || cData == 'Y' )
		{
			printf( "[Step-2] ������ PC�� IP�� ���ðڽ��ϱ�?(y/n)" );
			scanf( "%c", &cData );
			getchar();

			if( cData == 'y' || cData == 'Y' )
			{
				printf( "[Step-3] **������ PC���� ���� �����ϸ� �׽��Ͱ� �ƴϾ ���� �����մϴ�.\n" );
				bOpenPCRoom = true;
				break;
			}
		}
		else if( cData == 'n' || cData == 'N' )
		{
			printf( "[Step-2] PC������ IP�� �߰����� �ʽ��ϴ�.(y/n)" );
			scanf( "%c", &cData );
			getchar();

			if( cData == 'y' || cData == 'Y' )
			{
				printf( "[Step-3] --PC���� ���� ������ �Ұ����մϴ�.--\n" );
				bOpenPCRoom = false;
				break;
			}
		}
	}
	*/
}

BOOL ServerEngine::OnRegisterModule()
{
	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
		printf_s("Service Area : China\n");
	else if (g_eServiceArea == AP_SERVICE_AREA_TAIWAN)
		printf_s("Service Area : TAIWAN\n");
	else if (g_eServiceArea == AP_SERVICE_AREA_JAPAN)
		printf_s("Service Area : Japan\n");
	else if (g_eServiceArea == AP_SERVICE_AREA_GLOBAL)
		printf("Service Area : Global\n");
	else
		printf_s("Service Area : Korea\n");

	g_iServerClientType = ASSM_SERVER_TYPE_LOGIN_SERVER;

	//g_PacketPool.Initialize(sizeof(AuPacketBuffer), PACKET_POOL_COUNT);
	g_AuCircularBuffer.Init(100 * 1024 * 1024);
	g_AuCircularOutBuffer.Init(100 * 1024 * 1024);

	//REGISTER_MODULE(g_pcsAuDatabaseManager2, AuDatabaseManager2);
	REGISTER_MODULE(g_pcsAgpmConfig, AgpmConfig);
	//REGISTER_MODULE(g_pcsAgpmItemLog, AgpmItemLog);
	REGISTER_MODULE(g_pcsAgpmLog, AgpmLog);
	REGISTER_MODULE(g_pcsApmMap, ApmMap);
	REGISTER_MODULE(g_pcsApmObject, ApmObject);
	REGISTER_MODULE(g_pcsAgpmPathFind, AgpmPathFind);
	REGISTER_MODULE(g_pcsAgpmFactors, AgpmFactors);
	REGISTER_MODULE(g_pcsAgpmCharacter, AgpmCharacter);
	//REGISTER_MODULE(g_pcsAgpmScript, AgpmScript);
	REGISTER_MODULE(g_pcsAgpmBillInfo, AgpmBillInfo);
	REGISTER_MODULE(g_pcsAgpmUnion, AgpmUnion);
	REGISTER_MODULE(g_pcsAgpmGrid, AgpmGrid);
	REGISTER_MODULE(g_pcsAgpmItem, AgpmItem);
	REGISTER_MODULE(g_pcsAgpmParty, AgpmParty);
	REGISTER_MODULE(g_pcsApmEventManager, ApmEventManager);
	REGISTER_MODULE(g_pcsAgpmSummons, AgpmSummons);
	REGISTER_MODULE(g_pcsAgpmSkill, AgpmSkill);
	REGISTER_MODULE(g_pcsAgpmItemConvert, AgpmItemConvert);
	REGISTER_MODULE(g_pcsAgpmChatting, AgpmChatting);
	REGISTER_MODULE(g_pcsAgpmTimer, AgpmTimer);
	REGISTER_MODULE(g_pcsAgpmEventNature, AgpmEventNature);
	REGISTER_MODULE(g_pcsAgpmEventSpawn, AgpmEventSpawn);
	//REGISTER_MODULE(g_pcsAgpmAI2, AgpmAI2);
	REGISTER_MODULE(g_pcsAgpmCombat, AgpmCombat);
	REGISTER_MODULE(g_pcsAgpmShrine, AgpmShrine);
	REGISTER_MODULE(g_pcsAgpmEventBinding, AgpmEventBinding);
	REGISTER_MODULE(g_pcsAgpmGuild, AgpmGuild);
	REGISTER_MODULE(g_pcsAgpmAdmin, AgpmAdmin);
	REGISTER_MODULE(g_pcsAgpmUIStatus, AgpmUIStatus);
	REGISTER_MODULE(g_pcsAgpmWorld, AgpmWorld);
//	REGISTER_MODULE(g_pcsAgpmOptimizedPacket, AgpmOptimizedPacket);
	REGISTER_MODULE(g_pcsAgpmQuest, AgpmQuest);
	REGISTER_MODULE(g_pcsAgpmResourceInfo, AgpmResourceInfo);
	REGISTER_MODULE(g_pcsAgpmEventSkillMaster, AgpmEventSkillMaster);
	REGISTER_MODULE(g_pcsAgpmPvP, AgpmPvP);
	REGISTER_MODULE(g_pcsAgpmAuction, AgpmAuction);
	REGISTER_MODULE(g_pcsAgpmDropItem2, AgpmDropItem2);
	REGISTER_MODULE(g_pcsAgpmLogin, AgpmLogin);
	REGISTER_MODULE(g_pcsAgpmSystemMessage, AgpmSystemMessage);
	REGISTER_MODULE(g_pcsAgpmEventCharCustomize, AgpmEventCharCustomize);
	REGISTER_MODULE(g_pcsAgpmProduct, AgpmProduct);
	REGISTER_MODULE(g_pcsAgpmReturnToLogin, AgpmReturnToLogin);
	REGISTER_MODULE(g_pcsAgpmSiegeWar, AgpmSiegeWar);
	REGISTER_MODULE(g_pcsAgpmArchlord, AgpmArchlord);
	REGISTER_MODULE(g_pcsAgpmEventTeleport, AgpmEventTeleport);
	REGISTER_MODULE(g_pcsAgpmStartupEncryption, AgpmStartupEncryption);

	REGISTER_MODULE(g_pcsAgsmSystemMessage, AgsmSystemMessage);
	REGISTER_MODULE(g_pcsAgsmAOIFilter, AgsmAOIFilter);
	REGISTER_MODULE(g_pcsAgsmFactors, AgsmFactors);
	REGISTER_MODULE(g_pcsAgsmServerManager, AgsmServerManager);
	REGISTER_MODULE(g_pcsAgsmInterServerLink, AgsmInterServerLink);
	REGISTER_MODULE(g_pcsAgsmMap, AgsmMap);
	REGISTER_MODULE(g_pcsAgsmAccountManager, AgsmAccountManager);
	REGISTER_MODULE(g_pcsAgsmCharacter, AgsmCharacter);
	REGISTER_MODULE(g_pcsAgsmZoning, AgsmZoning);
	REGISTER_MODULE(g_pcsAgsmSystemInfo, AgsmSystemInfo);
	REGISTER_MODULE(g_pcsAgsmParty, AgsmParty);
	REGISTER_MODULE(g_pcsAgsmItem, AgsmItem);
	REGISTER_MODULE(g_pcsAgsmCharManager, AgsmCharManager);
	//REGISTER_MODULE(g_pcsAgsmItemLog, AgsmItemLog);
	REGISTER_MODULE(g_pcsAgsmCombat, AgsmCombat);
	REGISTER_MODULE(g_pcsAgsmSkill, AgsmSkill);
	REGISTER_MODULE(g_pcsAgsmSkillManager, AgsmSkillManager);
	REGISTER_MODULE(g_pcsAgsmItemConvert, AgsmItemConvert);
	REGISTER_MODULE(g_pcsAgsmItemManager, AgsmItemManager);
	REGISTER_MODULE(g_pcsAgsmSummons, AgsmSummons);
	REGISTER_MODULE(g_pcsAgsmUsedCharDataPool, AgsmUsedCharDataPool);
	REGISTER_MODULE(g_pcsAgsmDeath, AgsmDeath);
	REGISTER_MODULE(g_pcsAgsmChatting, AgsmChatting);
	REGISTER_MODULE(g_pcsAgsmEventSkillMaster, AgsmEventSkillMaster);
	REGISTER_MODULE(g_pcsAgsmUIStatus, AgsmUIStatus);
	REGISTER_MODULE(g_pcsAgsmGuild, AgsmGuild);
	REGISTER_MODULE(g_pcsAgsmGK, AgsmGK);
	REGISTER_MODULE(g_pcsAgsmGlobalChatting, AgsmGlobalChatting);
	REGISTER_MODULE(g_pcsAgsmLoginServer, AgsmLoginServer);
	REGISTER_MODULE(g_pcsAgsmLoginClient, AgsmLoginClient);
	REGISTER_MODULE(g_pcsAgsmWorld, AgsmWorld);
	REGISTER_MODULE(g_pcsAgsmDropItem2, AgsmDropItem2);
	REGISTER_MODULE(g_pcsAgsmQuest, AgsmQuest);
	REGISTER_MODULE(g_pcsAgsmAuction, AgsmAuction);
	REGISTER_MODULE(g_pcsAgpmEventQuest, AgpmEventQuest);
	REGISTER_MODULE(g_pcsAgsmBillInfo, AgsmBillInfo);
	//REGISTER_MODULE(g_pcsAgsmAdminDB, AgsmAdminDB);
	REGISTER_MODULE(g_pcsAgsmAdmin, AgsmAdmin);
	REGISTER_MODULE(g_pcsAgsmEventTeleport, AgsmEventTeleport);
	REGISTER_MODULE(g_pcsAgsmPvP, AgsmPvP);
	REGISTER_MODULE(g_pcsAgsmLog, AgsmLog);
	REGISTER_MODULE(g_pcsAgsmReturnToLogin, AgsmReturnToLogin);
	REGISTER_MODULE(g_pcsAgsmStartupEncryption, AgsmStartupEncryption);
	REGISTER_MODULE(g_pcsAgsmConfig, AgsmConfig);
	REGISTER_MODULE(g_pcsAgsmLoginDB, AgsmLoginDB);
//	REGISTER_MODULE(g_pcsAgsmArchlord, AgsmArchlord);
	REGISTER_MODULE(g_pcsAgsmServerStatus, AgsmServerStatus);
	REGISTER_MODULE(g_pcsAgpmBattleGround, AgpmBattleGround);
	REGISTER_MODULE(g_pcsAgsmBattleGround, AgsmBattleGround);
	REGISTER_MODULE(g_pcsAgpmRide, AgpmRide);
	REGISTER_MODULE(g_pcsAgpmCashMall, AgpmCashMall);

	REGISTER_MODULE(g_pcsAgpmTitle, AgpmTitle);
	REGISTER_MODULE(g_pcsAgsmTitle, AgsmTitle);

	REGISTER_MODULE(g_pcsAgsmPrivateTrade, AgsmPrivateTrade);
	
	InitMemoryPoolCount();

	// ServerStatus ��� �ٿ��� IPBlock���� ������ �� Start
#ifdef _AREA_CHINA_	
	g_pcsAgsmServerStatus->SetIPBlock(m_bIPBlock);
#endif
	g_pcsAgsmServerStatus->SetServerTypeLoginServer();
	g_pcsAgsmServerStatus->m_pServerMainClass = this;
	g_pcsAgsmServerStatus->Start();

	// disable idle events
	g_pcsAgpmCharacter->EnableIdle(FALSE);

	g_pcsApmMap->LoadAll(TRUE, TRUE);
	g_pcsApmMap->LoadTemplate("Ini\\RegionTemplate.ini", ENC_SVR_DATA);

	g_pcsApmObject->		SetMaxObject			( 500000	);
	g_pcsApmObject->		SetMaxObjectTemplate	( 10000	);

	g_pcsAgpmCharacter->	SetMaxCharacterTemplate	( 2000	);
	g_pcsAgpmCharacter->	SetMaxCharacter			( 200000);
	g_pcsAgpmCharacter->	SetMaxCharacterRemove	( 200000);
	g_pcsAgpmCharacter->	SetMaxIdleEvent			( 10000	);

	g_pcsAgpmItem->		SetMaxItem				( 5000000	);
	g_pcsAgpmItem->     SetMaxItemRemove(1000000);

	g_pcsAgpmSkill->		SetMaxSkill				( 10000	);
	g_pcsAgpmSkill->		SetMaxSkillTemplate		( 3000	);
	g_pcsAgpmSkill->		SetMaxSkillRemove		( 10000 );

	g_pcsAgpmWorld->SetMaxWorld(200);

	g_pcsAgpmEventBinding->	SetMaxBindingData		( 100	);

	g_pcsAgsmAccountManager->	SetMaxAccount		( 50000	);
	g_pcsAgsmAccountManager->	SetMaxAccountRemove	( 10000	);

	g_pcsAgsmCharacter->	SetMaxCheckRecvChar		( 10000	);
	g_pcsAgsmCharacter->	SetMaxWaitForRemoveChar	( 10000	);

	//g_pcsAgsmServerManager->	SetMaxServer	( 20		);
	//g_pcsAgsmServerManager->	SetMaxTemplate	( 100		);
	//g_pcsAgsmServerManager->	SetMaxIdleEvent	( 100		);

	g_pcsAgsmServerManager->	SetMaxServer	( AGSMSERVER_MAX_NUM_SERVER );
	g_pcsAgsmServerManager->	SetMaxIdleEvent2	( 100		);

	g_pcsAgsmInterServerLink->SetMaxIdleEvent2(200);

	g_pcsAgsmItem->SetMaxDBIDItem				( 5000000	);
	g_pcsAgpmAuction->SetMaxSales(500000);

	g_pcsAgpmTitle->SetMaxTitleTemplate(20000);

	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	{
		g_pcsAgpmResourceInfo->SetMajorVersion(31);
		g_pcsAgpmResourceInfo->SetMinorVersion(00);
	}
	else if(g_eServiceArea == AP_SERVICE_AREA_JAPAN)
	{
		g_pcsAgpmResourceInfo->SetMajorVersion(19);
		g_pcsAgpmResourceInfo->SetMinorVersion(00);
	}
	else if(g_eServiceArea == AP_SERVICE_AREA_TAIWAN)
	{
		g_pcsAgpmResourceInfo->SetMajorVersion(10);
		g_pcsAgpmResourceInfo->SetMinorVersion(00);
	}
	else if(g_eServiceArea == AP_SERVICE_AREA_GLOBAL)
	{
		g_pcsAgpmResourceInfo->SetMajorVersion(13);
		g_pcsAgpmResourceInfo->SetMinorVersion(00);
	}
	else
	{
		g_pcsAgpmResourceInfo->SetMajorVersion(12);
		g_pcsAgpmResourceInfo->SetMinorVersion(04);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	printf_s( "Start OnRegisterModule - Initailize()\n" );
	if(! Initialize() )
		return FALSE;

#ifndef _AREA_CHINA_
	if(! InitServerManager() )
		return FALSE;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// INI Directory �Ҵ�
	ApDirString szIniDir = g_pcsAgpmConfig->GetIniDir();


	printf_s("Server Version is [%d.%d]\n",g_pcsAgpmResourceInfo->GetMajorVersion(),g_pcsAgpmResourceInfo->GetMinorVersion());
#endif

	// module start
	/*	if( !g_csAgsmLoginClient.LoadCharNameFromExcel( "CharName.txt" ) )
	{
		return FALSE;
	}*/

	printf_s( "Start OnRegisterModule - CharType.ini\n" );
	if (!g_pcsAgpmFactors->		CharacterTypeStreamRead("Ini\\CharType.ini", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmFactors CharacterTypeStreamRead ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - SkillTemplate.ini\n" );
	if (!g_pcsAgpmSkill->			StreamReadTemplate("Ini\\SkillTemplate.ini"	, NULL, ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill StreamReadTemplate ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - CharacterTemplatePublic.ini\n" );
	if (!g_pcsAgpmCharacter->		StreamReadTemplate("Ini\\CharacterTemplatePublic.ini", NULL, ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadTemplate ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - LevelUpExp.txt\n" );
	if (!g_pcsAgpmCharacter->		StreamReadLevelUpExpTxt("Ini\\LevelUpExp.txt"))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadLevelUpExpTxt ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - GrowUpFactor.txt\n" );
	if (!g_pcsAgpmCharacter->		StreamReadCharGrowUpTxt("Ini\\GrowUpFactor.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharGrowUpTxt ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - CharKind.ini\n" );
	if (!g_pcsAgpmCharacter->		StreamReadCharKind("Ini\\CharKind.ini"				)	)
	{
		ASSERT( !"g_csAgpmCharacter StreamReadCharKind ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - Base.dat\n" );
	if (!g_pcsAgpmCharacter->		StreamReadChatFilterText("Ini\\Base.dat", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadChatFilterText ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - ItemTemplate.ini\n" );
	if (!g_pcsAgpmItem->			StreamReadTemplate("ini\\ItemTemplateAll.ini", NULL, ENC_SVR_DATA)	)
	{
		if (!g_pcsAgpmItem->			StreamReadTemplates("Ini\\ItemTemplate", "ini\\ItemTemplateEntry.ini", NULL, ENC_SVR_DATA)	)
		{
			ASSERT( !"g_csAgpmItem StreamReadTemplate ����" );
			return FALSE;
		}
	}

	// ĳ���� ������ �����ϴ� ������
	printf_s( "Start OnRegisterModule - CharacterDataTable.txt\n" );
	CHAR	szBuffer[512];
	if (!g_pcsAgpmCharacter->		StreamReadImportData("Ini\\CharacterDataTable.txt", szBuffer, ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadImportData ����" );
		return FALSE;
	}

#ifndef _AREA_CHINA_
	printf_s( "Start OnRegisterModule - BaseCharacterInfo.xml\n" );
	if (!g_pcsAgsmCharacter->		StreamReadBaseCharacterInfo("Ini\\BaseCharacterInfo.xml") )
	{
		ASSERT( !"g_csAgpmCharacter StreamReadBaseCharacterInfo ����" );
		return FALSE;
	}
#endif

	printf_s( "Start OnRegisterModule - ItemDataTable.txt\n" );
	if (!g_pcsAgpmItem->			StreamReadImportData("Ini\\ItemDataTable.txt", szBuffer, ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadImportData ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - ItemOptionTable.txt\n" );
	if (!g_pcsAgpmItem->StreamReadOptionData("Ini\\ItemOptionTable.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmCharacter StreamReadOptionData ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - Skill_Spec.txt\n" );
	if (!g_pcsAgpmSkill->			ReadSkillSpecTxt("Ini\\Skill_Spec.txt"	, ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillSpecTxt ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - Skill_Const.txt\n" );
	if (!g_pcsAgpmSkill->			ReadSkillConstTxt("Ini\\Skill_Const.txt", ENC_SVR_DATA)	)
	{
		ASSERT( !"g_csAgpmSkill ReadSkillConstTxt ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - CharacterCustomizeList.txt\n" );
	if (!g_pcsAgpmEventCharCustomize->StreamReadCustomizeList("Ini\\CharacterCustomizeList.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_pcsAgpmEventCharCustomize StreamReadCustomizeList ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - SkillMasteryEvolution.txt\n" );
	if (!g_pcsAgpmEventSkillMaster->StreamReadMasteryEvolution("Ini\\SkillMasteryEvolution.txt", ENC_SVR_DATA))
	{
		ASSERT( !"g_csAgpmSkill SkillMasteryEvolution ����" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - LoginCharPos.ini\n" );
	if (!g_pcsAgsmLoginClient->	StreamReadBaseCharPos("Ini\\LoginCharPos.ini"		)	)
	{
		ASSERT( !"failed g_csAgsmLoginClient StreamReadBaseCharPos()" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - RestrictedCharName.txt\n" );
	if (!g_pcsAgsmLoginClient->	LoadCharNameFromExcel("Ini\\RestrictedCharName.txt"		)	)
	{
		ASSERT( !"failed g_csAgsmLoginClient LoadCharNameFromExcel()" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - Skill_DefaultOwn.txt\n" );
	if (!g_pcsAgpmEventSkillMaster->StreamReadDefaultSkill("Ini\\Skill_DefaultOwn.txt", ENC_SVR_DATA))
	{
		ASSERT( !"failed m_pcsAgpmEventSkillMaster->StreamReadDefaultSkill()" );
		return FALSE;
	}

	printf_s( "Start OnRegisterModule - DBQueryLogin.ini\n" );
	if (!g_pcsAgsmLoginDB->LoadQuery("Ini\\DBQueryLogin.ini", AUDB_VENDER_ORACLE))
	{
		ASSERT( !"failed g_pcsAgsmLoginDB->LoadQuery()" );
		return FALSE;	
	}

	printf_s( "Start OnRegisterModule - DBQueryLogin_MS.ini\n" );
	if (!g_pcsAgsmLoginDB->LoadQuery("Ini\\DBQueryLogin_MS.ini", AUDB_VENDER_MSSQL))
	{
		ASSERT( !"failed g_pcsAgsmLoginDB->LoadQuery()" );
		return FALSE;
	}

	printf_s(" Start OnRegisterModule - TitleDataTable.txt\n");
	if(!g_pcsAgpmTitle->StreamReadTitleDataTemplate("ini\\titledatatable.txt", ENC_SVR_DATA))
	{
		ASSERT( !"failed g_pcsAgpmTitle->StreamReadTitleDataTemplate()" );
		return FALSE;
	}

	// Make Base CharacterInfo in ServerEngine
	printf_s("Make Base CharacterInfo\n");
	if(!g_pcsAgsmLoginClient->CreateBaseCharacterOfRace(1))
	{
		ASSERT( !"failed g_pcsAgsmLoginClient->CreateBaseCharacterOfRace()" );
		return FALSE;
	}

	printf_s( "INI load complete.\n" );

	return TRUE;
}

BOOL ServerEngine::OnTerminate()
{
	return Destroy();
}

BOOL ServerEngine::ConnectServers()
{
	AgsmGKReturn	eReturn	= g_pcsAgsmGK->ConnectLKServer();
	if (eReturn == AGSMGK_RETURN_NOT_EXIST_LKSERVER)
		return TRUE;
	else if (eReturn == AGSMGK_RETURN_CONNECT_FAIL)
		return FALSE;

	return g_pcsAgsmGK->WaitForConnectServer();
}

BOOL ServerEngine::DisconnectServers()
{
	return TRUE;
}

//DB ��� �����尡 ������ ��ٸ���.
BOOL ServerEngine::WaitDBThreads()
{
	g_pcsAgsmLoginDB->StopWorker();

	return TRUE;
}

BOOL ServerEngine::InitServerManager()
{
#ifndef _AREA_CHINA_
	if (!g_pcsAgsmServerManager->ReadServerInfo())
	{
		ASSERT( !"g_csAgsmServerManager StreamReadServerInfo ����" );
		printf_s( "Acquire db account info failed\n" );
		return FALSE;
	}

	if (!g_pcsAgsmServerManager->InitThisServer())
	{
		cout << "InitThisServer Failed" << endl;
		ASSERT( !"g_csAgsmServerManager InitThisServer ����" );
		return FALSE;
	}

	//Login DB ������ �������� �����带 ����. �켱 10��~ �������. ����
	if (!g_pcsAgsmLoginDB->StartWorker(10))
	{
		cout << "StartWorker Failed" << endl;
		ASSERT( !"g_pcsAgsmLoginDB->StartWorker() ����");
		_tprintf_s("Starting workers failed\n");
		return FALSE;		
	}

	Sleep(500);

	AgsdServer2 *pcsServer = g_pcsAgsmServerManager->GetThisServer();
	if (!pcsServer)
		return FALSE;

	INT32 lServerIndex = pcsServer->m_lServerID;

	g_pcsAgsmLoginDB->SetKeyRange(lServerIndex);				// ���� ������ instant key
	g_pcsAgsmItemManager->InitItemDBIDServer(lServerIndex);		// Item Sequence ��

	g_pcsAgsmLoginDB->LoadExpeditionList();
	
	// ���� config loading
	if (!g_pcsAgsmConfig->LoadConfig())
	{
		cout << "LoadConfig Done!" << endl;
		ASSERT( !"g_pcsAgsmConfig LoadConfig ����" );
		return FALSE;
	}

	// 2006.06.14. steeple
	// �˰��� �ʱ�ȭ ���⼭ �Ѵ�.
	//AuPacket::GetCryptManager().SetUseCrypt(FALSE);		// ���� �ּ��� Ǯ�� ��ȣȭ ���� �ʴ´�.
	g_pcsAgsmStartupEncryption->SetAlgorithm();
	g_pcsAgsmStartupEncryption->InitializePublicKey();

	// set connect level
	// Admin�� ������ ����ϴ����� ����, AccountLevel�� �������ش�.
	GetGameEnv().InitEnvironment();
	if(GetGameEnv().IsAdmin())
	{
		g_pcsAgsmAdmin->SetConnectableAccountLevel(88);
	}
	else
	{
		g_pcsAgsmAdmin->SetConnectableAccountLevel(0);
	}
	
#else
	CHAR szDSN[30];
	CHAR szUser[30];
	CHAR szPwd[30];
	CHAR szVender[30];

	ZeroMemory(szDSN, sizeof(szDSN));
	ZeroMemory(szUser, sizeof(szUser));
	ZeroMemory(szPwd, sizeof(szPwd));
	ZeroMemory(szVender, sizeof(szVender));

	if (!g_pcsAgsmServerManager->StreamReadDBServerInfo("CONFIG\\DBSERVERINFO.ini", TRUE, szDSN, szUser, szPwd, szVender))
	{
		ASSERT( !"g_csAgsmServerManager StreamReadDBServerInfo ����" );
		return FALSE;
	}

	printf( "Start OnRegisterModule - Get db account from DB\n" );
	//printf( "%s, %s, %s, %s\n", szVender, szDSN, szUser, szPwd);
	if (!g_pcsAgsmServerManager->StreamReadServerInfo(szVender, szDSN, szUser, szPwd, "SERVERCONFIG"))
	{
		ASSERT( !"g_csAgsmServerManager StreamReadServerInfo ����" );
		printf( "Acquire db account info failed\n" );
		return FALSE;
	}

	g_pcsAgsmServerManager->SetConfigDBAuthInfo(szDSN, szUser, szPwd);

	if (!g_pcsAgsmServerManager->InitThisServer())
	{
		ASSERT( !"g_csAgsmServerManager InitThisServer ����" );
		return FALSE;
	}

	//Login DB ������ �������� �����带 ����. �켱 10��~ �������. ����
	if (!g_pcsAgsmLoginDB->StartWorker(10))
	{
		ASSERT( !"g_pcsAgsmLoginDB->StartWorker() ����");
		_tprintf("Starting workers failed\n");
		return FALSE;		
	}

	Sleep(500);

	AgsdServer2 *pcsServer = g_pcsAgsmServerManager->GetThisServer();
	if (!pcsServer)
		return FALSE;

	INT32 lServerIndex = pcsServer->m_lServerID;

	g_pcsAgsmLoginDB->SetKeyRange(lServerIndex);				// ���� ������ instant key
	g_pcsAgsmItemManager->InitItemDBIDServer(lServerIndex);		// Item Sequence ��

	g_pcsAgsmLoginDB->LoadExpeditionList();
	
	// ���� config loading
	if (!g_pcsAgsmConfig->LoadConfig())
	{
		ASSERT( !"g_pcsAgsmConfig LoadConfig ����" );
		return FALSE;
	}

	// 2006.06.14. steeple
	// �˰��� �ʱ�ȭ ���⼭ �Ѵ�.
	//AuPacket::GetCryptManager().SetUseCrypt(FALSE);		// ���� �ּ��� Ǯ�� ��ȣȭ ���� �ʴ´�.
	g_pcsAgsmStartupEncryption->SetAlgorithm();
	g_pcsAgsmStartupEncryption->InitializePublicKey();

	// set connect level
	// Admin�� ������ ����ϴ����� ����, AccountLevel�� �������ش�.
	GetGameEnv().InitEnvironment();
	if(GetGameEnv().IsAdmin())
	{
		g_pcsAgsmAdmin->SetConnectableAccountLevel(88);
	}
	else
	{
		g_pcsAgsmAdmin->SetConnectableAccountLevel(0);
	}
#endif

	return TRUE;
}

void ServerEngine::InitMemoryPoolCount()
{
	GetGameEnv().InitEnvironment();
	if(GetGameEnv().IsAlpha())
		return;

	AuIniManagerA	m_csIniFile;
	m_csIniFile.SetPath("Ini\\MemoryPoolCount.ini");

	if (!m_csIniFile.ReadFile(0, FALSE))
		return;

	int pItemCount		= m_csIniFile.GetValueI("LoginServerPool", "Item");
	if(pItemCount > 0)
		g_pcsAgpmItem->InitMemoryPool(0, pItemCount, "g_pcsAgpmItem");

	int pCharacterCount	= m_csIniFile.GetValueI("LoginServerPool", "Character");
	if(pCharacterCount > 0)
		g_pcsAgpmCharacter->InitMemoryPool(0, pCharacterCount, "g_pcsAgpmCharacter");

	int pGridCount = m_csIniFile.GetValueI("LoginServerPool", "Grid");
	if(pGridCount > 0)
		g_pcsAgpmGrid->InitMemoryPool(0, pGridCount, "g_pcsAgpmGrid");

	int pGridMemoryPool = m_csIniFile.GetValueI("LoginServerPool", "GridMemoryPool");
	if(pGridMemoryPool > 0)
		g_pcsAgpmGrid->InitGridMemoryPool(pGridMemoryPool);
}
