// AgpmPvP.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 16.


#include <AgpmPvP/AgpmPvP.h>
#include <AgpmCharacter/ApAutoLockCharacter.h>
#include <AgpmBattleGround/AgpmBattleGround.h>
#include <AgpmConfig/AgpmConfig.h>
#include <AgpmSkill/AgpmSkill.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgpmParty/AgpmParty.h>
#include <AgpmSummons/AgpmSummons.h>
#include <AgpmCharacter/AgpmCharacter.h>

//////////////////////////////////////////////////////////////////////////

AgpmPvP::AgpmPvP()
{
	SetModuleName("AgpmPvP");
	SetModuleType(APMODULE_TYPE_PUBLIC);
	
	SetModuleData(sizeof(AgpdPvPCharInfo), AGPMPVP_DATA_TYPE_CHAR_INFO);
	SetModuleData(sizeof(AgpdPvPGuildInfo), AGPMPVP_DATA_TYPE_GUILD_INFO);
	SetModuleData(sizeof(AgpdPvPAreaDrop), AGPMPVP_DATA_TYPE_AREA_DROP);
	SetModuleData(sizeof(AgpdPvPItemDrop), AGPMPVP_DATA_TYPE_ITEM_DROP);
	SetModuleData(sizeof(AgpdPvPSkullDrop), AGPMPVP_DATA_TYPE_SKULL_DROP);

	EnableIdle(FALSE);
	
	m_papmMap = NULL;
	m_pagpmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmGuild = NULL;
	m_pagpmSkill = NULL;
	m_pagpmSummons = NULL;
	m_pagpmParty = NULL;
	m_pagpmBattleGround	= NULL;
	m_pagpmConfig = NULL;
	m_nIndexADCharacter = -1;

	m_lSkullDropMinLevelGap = 0;
	m_lSkullDropMaxLevelGap = 0;
    memset(&m_alSkullDropLevelBranch, 0, sizeof(CHAR) * AGPMPVP_MAX_SKULL_LEVEL_BRANCH);
	memset(&m_alSkullDropLevel, 0, sizeof(CHAR)* AGPMPVP_MAX_SKULL_LEVEL_BRANCH);
	
	m_ulLastClock = 0;

	SetPacketType(AGPMPVP_PACKET_TYPE); 

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_INT32,			1,	// CID
							AUTYPE_INT32,			1,	// Target CID
							AUTYPE_INT32,			1,	// Win
							AUTYPE_INT32,			1,	// Lose
							AUTYPE_INT8,			1,	// PvPMode
							AUTYPE_INT8,			1,	// PvPStatus
							AUTYPE_INT8,			1,	// Win
							AUTYPE_CHAR,			AGPMGUILD_MAX_GUILD_ID_LENGTH + 1,	// GuildID
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1,	// For System Message
							AUTYPE_END,				0
							);

	//if (g_eServiceArea == AP_SERVICE_AREA_KOREA || AP_SERVICE_AREA_JAPAN == g_eServiceArea)
	//	m_lMinPvPLevel = 30;
	//else
	//	m_lMinPvPLevel = 15;
	//if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
	//	m_lMinPvPLevel = 15;
	//else
		m_lMinPvPLevel = AGPMPVP_MINIMUM_ABLE_LEVEL;
	
	m_eRaceBattleStatus = AGPMPVP_RACE_BATTLE_STATUS_NONE;
	m_lRaceBattleWinner = AURACE_TYPE_NONE;
}

AgpmPvP::~AgpmPvP()
{
}

BOOL AgpmPvP::OnAddModule()
{
	m_papmMap = (ApmMap*)GetModule("ApmMap");
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pagpmGuild = (AgpmGuild*)GetModule("AgpmGuild");
	m_pagpmSkill = (AgpmSkill*)GetModule("AgpmSkill");
	m_pagpmSummons = (AgpmSummons*)GetModule("AgpmSummons");
	m_pagpmParty = (AgpmParty*)GetModule("AgpmParty");

	if(!m_papmMap || !m_pagpmCharacter || !m_pagpmGuild || !m_pagpmSkill || !m_pagpmSummons || !m_pagpmParty)
		return FALSE;

	m_nIndexADCharacter = m_pagpmCharacter->AttachCharacterData(this, sizeof(AgpdPvPADChar), ConAgpdPvPADChar, DesAgpdPvPADChar);
	if(m_nIndexADCharacter < 0)
		return FALSE;

	if(!m_pagpmCharacter->SetCallbackInitChar(CBInitCharacter, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackCheckValidNormalAttack(CBIsAttackable, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
		return FALSE;

	// 2005.06.01. steeple
	if(!m_pagpmCharacter->SetCallbackAddCharacterToMap(CBRegionChange, this))
		return FALSE;

	if(!m_pagpmSkill->SetCallbackCheckSkillPvP(CBIsSkillEnable, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmPvP::OnInit()
{
	m_pagpmBattleGround	= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagpmConfig = (AgpmConfig*)GetModule("AgpmConfig");
	
	if(!m_pagpmBattleGround || !m_pagpmConfig)
		return FALSE;
	
	return TRUE;
}

const INT32 AGPMPVP_INTERVAL = (1000 * 30);	// 30 seconds

BOOL AgpmPvP::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmPvP::OnDestroy()
{
	// Probability �����س��� �� �����.
	INT32 lIndex = 0;

	// Area Drop
	for(AgpdPvPAreaDrop** ppcsPvPAreaDrop = (AgpdPvPAreaDrop**)m_csAreaDropAdmin.GetObjectSequence(&lIndex); ppcsPvPAreaDrop;
			ppcsPvPAreaDrop = (AgpdPvPAreaDrop**)m_csAreaDropAdmin.GetObjectSequence(&lIndex))
	{
		DestroyModuleData((PVOID)*ppcsPvPAreaDrop, AGPMPVP_DATA_TYPE_AREA_DROP);
	}
	m_csAreaDropAdmin.RemoveObjectAll();		

	lIndex = 0;

	// Item Drop
	for(AgpdPvPItemDrop** ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex); ppcsPvPItemDrop;
			ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex))
	{
		DestroyModuleData((PVOID)*ppcsPvPItemDrop, AGPMPVP_DATA_TYPE_ITEM_DROP);
	}
	m_csItemDropAdmin.RemoveObjectAll();	

	lIndex = 0;

	// Skull Drop
	for(AgpdPvPSkullDrop** ppcsPvPSkullDrop = (AgpdPvPSkullDrop**)m_csSkullDropAdmin.GetObjectSequence(&lIndex); ppcsPvPSkullDrop;
			ppcsPvPSkullDrop = (AgpdPvPSkullDrop**)m_csSkullDropAdmin.GetObjectSequence(&lIndex))
	{
		DestroyModuleData((PVOID)*ppcsPvPSkullDrop, AGPMPVP_DATA_TYPE_SKULL_DROP);
	}
	m_csSkullDropAdmin.RemoveObjectAll();		
	
	return TRUE;
}

AgpdPvPADChar* AgpmPvP::GetADCharacter(AgpdCharacter* pData)
{
	AgpdPvPADChar* pcsPvPADChar = NULL;
	if(m_pagpmCharacter)
		pcsPvPADChar = (AgpdPvPADChar*)m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, (PVOID)pData);

	if(!pcsPvPADChar)
		return NULL;

	// 2005.01.12. PC �� ���� �˻縦 ����ȭ�Ѵ�.
	// PC �� ���� Valid �� ���� ���ϵ� �� �ִ�.
	if(m_pagpmCharacter && m_pagpmCharacter->IsPC(pData))
	{
		// ���� �ʱ�ȭ�� �ȵ� ���¶�� ��� �������ش�.
		if(!pcsPvPADChar->m_bInit)
			return pcsPvPADChar;

		// ������, ��, ������ ���, ����� ����Ʈ�� ��� Valid �� ���� return �����ش�.
		if(pcsPvPADChar->m_pFriendVector&& pcsPvPADChar->m_pFriendGuildVector &&
			pcsPvPADChar->m_pEnemyVector && pcsPvPADChar->m_pEnemyGuildVector)
			return pcsPvPADChar;
	}

	return NULL;
}

BOOL AgpmPvP::ConAgpdPvPADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmPvP::DesAgpdPvPADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmPvP::InitCharacter(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// PC �� �ƴ϶�� �� ����
	if(!IsPC(pcsCharacter))
		return TRUE;

	// ������ ����
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	pcsAttachedPvP->m_bInit = TRUE;

	pcsAttachedPvP->m_lWin = 0;
	pcsAttachedPvP->m_lLose = 0;
	pcsAttachedPvP->m_bDeadInCombatArea = FALSE;

	// 2007.01.30. steeple
	// AgpmPvPArray �� ������ �־ vector �� �� �ٲ��.
	pcsAttachedPvP->m_pFriendVector = new PvPCharVector;
	pcsAttachedPvP->m_pEnemyVector = new PvPCharVector;
	pcsAttachedPvP->m_pFriendGuildVector = new PvPGuildVector;
	pcsAttachedPvP->m_pEnemyGuildVector = new PvPGuildVector;

	// Memory Pool �� �Ⱦ��� ���� ���������� Reserve �س���
	// Ŭ���̾�Ʈ�� ���� ����. -_-;
	// ��� InitCharacter �ݹ��� �޾Ƽ� AgsmPvP ������ ����.

	InitFriendList(pcsCharacter);
	InitEnemyList(pcsCharacter);
	InitFriendGuildList(pcsCharacter);
	InitEnemyGuildList(pcsCharacter);

	return TRUE;
}

BOOL AgpmPvP::RemoveCharacter(AgpdCharacter* pcsCharacter)
{
	// PC �� �ƴ϶�� �� ����
	if(!IsPC(pcsCharacter))
		return TRUE;

	// ������ ����
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// �ʱ�ȭ �ȵǾ� ������ �� ������.
	if(!pcsAttachedPvP->m_bInit)
		return TRUE;

	InitFriendList(pcsCharacter);
	InitEnemyList(pcsCharacter);
	InitFriendGuildList(pcsCharacter);
	InitEnemyGuildList(pcsCharacter);

	if(pcsAttachedPvP->m_pFriendVector)
	{
		pcsAttachedPvP->m_pFriendVector->clear();
		delete pcsAttachedPvP->m_pFriendVector;
		pcsAttachedPvP->m_pFriendVector = NULL;
	}

	if(pcsAttachedPvP->m_pEnemyVector)
	{
		pcsAttachedPvP->m_pEnemyVector->clear();
		delete pcsAttachedPvP->m_pEnemyVector;
		pcsAttachedPvP->m_pEnemyVector = NULL;
	}

	if(pcsAttachedPvP->m_pFriendGuildVector)
	{
		pcsAttachedPvP->m_pFriendGuildVector->clear();
		delete pcsAttachedPvP->m_pFriendGuildVector;
		pcsAttachedPvP->m_pFriendGuildVector = NULL;
	}

	if(pcsAttachedPvP->m_pEnemyGuildVector)
	{
		pcsAttachedPvP->m_pEnemyGuildVector->clear();
		delete pcsAttachedPvP->m_pEnemyGuildVector;
		pcsAttachedPvP->m_pEnemyGuildVector = NULL;
	}

	return TRUE;
}

BOOL AgpmPvP::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmPvP::OnReceive");
	
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	INT32 lCID = -1;
	INT32 lTargetCID = -1;
	INT32 lWin = -1;
	INT32 lLose = -1;
	INT8 cPvPMode = -1;
	INT8 cPvPStatus = -1;
	INT8 cWin = -1;
	CHAR* szGuildID = NULL;
	CHAR* szMessage = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&lTargetCID,
						&lWin,
						&lLose,
						&cPvPMode,
						&cPvPStatus,
						&cWin,
						&szGuildID,
						&szMessage);

	switch(cOperation)
	{
		case AGPMPVP_PACKET_PVP_INFO:
			OnReceivePvPInfo(lCID, lTargetCID, lWin, lLose, cPvPMode, cPvPStatus, cWin);
			break;

		case AGPMPVP_PACKET_ADD_FRIEND:
			OnReceiveAddFriend(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_ADD_ENEMY:
			OnReceiveAddEnemy(lCID, lTargetCID, cPvPMode);
			break;

		case AGPMPVP_PACKET_REMOVE_FRIEND:
			OnReceiveRemoveFriend(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_REMOVE_ENEMY:
			OnReceiveRemoveEnemy(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_INIT_FRIEND:
			OnReceiveInitFriend(lCID);
			break;

		case AGPMPVP_PACKET_INIT_ENEMY:
			OnReceiveInitEnemy(lCID);
			break;

		case AGPMPVP_PACKET_UPDATE_FRIEND:
			OnReceiveUpdateFriend(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_UPDATE_ENEMY:
			OnReceiveUpdateEnemy(lCID, lTargetCID, cPvPMode);
			break;

		case AGPMPVP_PACKET_ADD_FRIEND_GUILD:
			OnReceiveAddFriendGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_ADD_ENEMY_GUILD:
			OnReceiveAddEnemyGuild(lCID, szGuildID, cPvPMode);
			break;

		case AGPMPVP_PACKET_REMOVE_FRIEND_GUILD:
			OnReceiveRemoveFriendGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_REMOVE_ENEMY_GUILD:
			OnReceiveRemoveEnemyGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_INIT_FRIEND_GUILD:
			OnReceiveInitFriendGuild(lCID);
			break;

		case AGPMPVP_PACKET_INIT_ENEMY_GUILD:
			OnReceiveInitEnemyGuild(lCID);
			break;

		case AGPMPVP_PACKET_UPDATE_FRIEND_GUILD:
			OnReceiveUpdateFriendGuild(lCID, szGuildID);
			break;

		case AGPMPVP_PACKET_UPDATE_ENEMY_GUILD:
			OnReceiveUpdateEnemyGuild(lCID, szGuildID, cPvPMode);
			break;

		case AGPMPVP_PACKET_CANNOT_USE_TELEPORT:
			OnReceiveCannotUseTeleport(lCID);
			break;

		case AGPMPVP_PACKET_SYSTEM_MESSAGE:
			OnReceiveSystemMessage(lCID, szGuildID, szMessage, lWin, lLose);
			break;

		case AGPMPVP_PACKET_REQUEST_DEAD_TYPE:
			OnReceiveRequestDeadType(lCID, lTargetCID);
			break;

		case AGPMPVP_PACKET_RESPONSE_DEAD_TYPE:
			if (pstCheckArg->bReceivedFromServer)
				OnReceiveResponseDeadType(lCID, lTargetCID, lWin, lLose);
			break;
		
		case AGPMPVP_PACKET_RACE_BATTLE:
			if (pstCheckArg->bReceivedFromServer)
				OnReceiveRaceBattle(cPvPStatus,		// race battle status
								 lCID,				// remain time
								 lTargetCID,		// point	human
								 lWin,				//			orc
								 lLose				//			moonelf
								 );
			break;
	}

	return TRUE;
}

// �������� ��쿡 �̸��� �´�.
// + ó�� �α��� �� lWin, lLose, cPvPMode, cPvPStatus �� �޾ƿ� ��.
// + ���� ������ ���ų� ���� �� cPvPMode �� ����.
// + ���� �����ϸ� lTargetCID, cPvPStatus �� ����.
// + ������ ������ lTargetCID, lWin, lLose, cPvPStatus, cWin �� ����.
BOOL AgpmPvP::OnReceivePvPInfo(INT32 lCID, INT32 lTargetCID, INT32 lWin, INT32 lLose, INT8 cPvPMode, INT8 cPvPStatus, INT8 cWin)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(lWin != -1)
		pcsAttachedPvP->m_lWin = lWin;
	if(lLose != -1)
		pcsAttachedPvP->m_lLose = lLose;
	if(cPvPMode != -1)
		pcsAttachedPvP->m_ePvPMode = (eAgpdPvPMode)cPvPMode;
	if(cPvPStatus != -1)
		pcsAttachedPvP->m_cPvPStatus = cPvPStatus;

	EnumCallback(AGPMPVP_CB_PVP_INFO, pcsCharacter, NULL);

	// cWin �� ���õǾ� �ִٸ� ����� �� ���̴�.
	if(cWin != -1)
	{
		PVOID pBuffer[2];
		pBuffer[0] = &lTargetCID;
		pBuffer[1] = &cWin;
		EnumCallback(AGPMPVP_CB_PVP_RESULT, pcsCharacter, pBuffer);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// ���� �� �߰�
BOOL AgpmPvP::OnReceiveAddFriend(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(AddFriend(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_ADD_FRIEND, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();
	return TRUE;
}

// �� �߰�
BOOL AgpmPvP::OnReceiveAddEnemy(INT32 lCID, INT32 lTargetCID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(ulLastCombatClock == 0)
		ulLastCombatClock = GetClockCount();

	if(AddEnemy(pcsCharacter, lTargetCID, cPvPMode, ulLastCombatClock))
		EnumCallback(AGPMPVP_CB_ADD_ENEMY, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// ���� �� ����
BOOL AgpmPvP::OnReceiveRemoveFriend(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(RemoveFriend(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_REMOVE_FRIEND, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// �� ����
BOOL AgpmPvP::OnReceiveRemoveEnemy(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(RemoveEnemy(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// ���� �� ����Ʈ ����.
// ���� ���������� ����/���� �� ������ �ִ� �͵��� ���� ���ؼ� �޴´�.
BOOL AgpmPvP::OnReceiveInitFriend(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitFriendList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_FRIEND;
		EnumCallback(AGPMPVP_CB_INIT_FRIEND, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// �� ����Ʈ ����.
// ���� ���������� ����/���� �� ������ �ִ� �͵��� ���� ���ؼ� �޴´�.
BOOL AgpmPvP::OnReceiveInitEnemy(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitEnemyList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_ENEMY;
		EnumCallback(AGPMPVP_CB_INIT_ENEMY, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// ���� �� �ѳ��� ������ ������Ʈ
BOOL AgpmPvP::OnReceiveUpdateFriend(INT32 lCID, INT32 lTargetCID)
{
	if(lCID == AP_INVALID_CID || lTargetCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateFriend(pcsCharacter, lTargetCID))
		EnumCallback(AGPMPVP_CB_UPDATE_FRIEND, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// �� �ѳ��� ������ ������Ʈ
BOOL AgpmPvP::OnReceiveUpdateEnemy(INT32 lCID, INT32 lTargetCID, INT8 cPvPMode, UINT32 ulClockCount)
{
	if(lCID == AP_INVALID_CID || lTargetCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateEnemy(pcsCharacter, lTargetCID, cPvPMode, ulClockCount))
		EnumCallback(AGPMPVP_CB_UPDATE_ENEMY, pcsCharacter, &lTargetCID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}


BOOL AgpmPvP::OnReceiveAddFriendGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(AddFriendGuild(pcsCharacter, szGuildID))
		EnumCallback(AGPMPVP_CB_ADD_FRIEND_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveAddEnemyGuild(INT32 lCID, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(AddEnemyGuild(pcsCharacter, szGuildID, cPvPMode, ulLastCombatClock))
		EnumCallback(AGPMPVP_CB_ADD_ENEMY_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveRemoveFriendGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	//if(RemoveFriendGuild(pcsCharacter, szGuildID))
	// �ڱ� ��尡 �� ���� �����Ƿ� ������ �ݹ� �θ���.
	RemoveFriendGuild(pcsCharacter, szGuildID);
	EnumCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveRemoveEnemyGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(RemoveEnemyGuild(pcsCharacter, szGuildID))
		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveInitFriendGuild(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitFriendGuildList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_FRIEND_GUILD;
		EnumCallback(AGPMPVP_CB_INIT_FRIEND_GUILD, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveInitEnemyGuild(INT32 lCID)
{
	if(lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(InitEnemyGuildList(pcsCharacter))
	{
		INT8 cInitType = (INT8)AGPMPVP_INIT_TYPE_ENEMY_GUILD;
		EnumCallback(AGPMPVP_CB_INIT_ENEMY_GUILD, pcsCharacter, &cInitType);
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveUpdateFriendGuild(INT32 lCID, CHAR* szGuildID)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateFriendGuild(pcsCharacter, szGuildID))
		EnumCallback(AGPMPVP_CB_UPDATE_FRIEND_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveUpdateEnemyGuild(INT32 lCID, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(lCID == AP_INVALID_CID || !szGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if(UpdateEnemyGuild(pcsCharacter, szGuildID, cPvPMode, ulLastCombatClock))
		EnumCallback(AGPMPVP_CB_UPDATE_ENEMY_GUILD, pcsCharacter, szGuildID);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveCannotUseTeleport(INT32 lCID)
{
	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	EnumCallback(AGPMPVP_CB_RECV_CANNOT_USE_TELEPORT, pcsCharacter, NULL);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveSystemMessage(INT32 lCode, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2)
{
	return ProcessSystemMessage(lCode, szData1, szData2, lData1, lData2);
}

// 2005.07.28. steeple
BOOL AgpmPvP::OnReceiveRequestDeadType(INT32 lCID, INT32 lResurrectionType)
{
	if(lCID < 1 || lResurrectionType < 0)
		return FALSE;

	EnumCallback(AGPMPVP_CB_REQUEST_DEAD_TYPE, &lCID, &lResurrectionType);
	return TRUE;
}

// 2005.07.28. steeple
BOOL AgpmPvP::OnReceiveResponseDeadType(INT32 lCID, INT32 lTargetCID, INT32 lDropExpRate, INT32 lTargetType)
{
	if(lCID < 1)
		return FALSE;

	INT32 alTmp[3];
	alTmp[0] = lTargetCID;
	alTmp[1] = lDropExpRate;
	alTmp[2] = lTargetType;

	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if (!pcsAttachedPvP)
		return FALSE;

	pcsAttachedPvP->m_cDeadType	= (INT8) lTargetType;

	EnumCallback(AGPMPVP_CB_RESPONSE_DEAD_TYPE, pcsCharacter, alTmp);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmPvP::OnReceiveRaceBattle(INT8 cStatus, INT32 lRemainTime, INT32 lHumanPoint, INT32 lOrcPoint, INT32 lMoonelfPoint)
{
	INT8 cPrevStatus = m_eRaceBattleStatus;
	m_eRaceBattleStatus = (eAGPMPVP_RACE_BATTLE_STATUS) cStatus;

	m_lRaceBattlePoint[AURACE_TYPE_HUMAN]	= lHumanPoint;
	m_lRaceBattlePoint[AURACE_TYPE_ORC]		= lOrcPoint;
	m_lRaceBattlePoint[AURACE_TYPE_MOONELF]	= lMoonelfPoint;
	
	if (AGPMPVP_RACE_BATTLE_STATUS_ENDED == m_eRaceBattleStatus)
		SetRaceBattleResult();
	
	EnumCallback(AGPMPVP_CB_RACE_BATTLE_STATUS, &cPrevStatus, &lRemainTime);
	
	return TRUE;
}

// �̹� FriendList �� ���� ApAdmin �� �޸� ������ ���� �����̰�,
// ����Ʈ�� ����ش�.
BOOL AgpmPvP::InitFriendList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pFriendVector)
		return FALSE;

	PvPCharIter iter = pcsAttachedPvP->m_pFriendVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendVector->end())
	{
		EnumCallback(AGPMPVP_CB_REMOVE_FRIEND, pcsCharacter, &iter->m_lCID);
		++iter;
	}

	pcsAttachedPvP->m_pFriendVector->clear();
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPCharInfo* pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObjectSequence(&lIndex))
	//{
	//	EnumCallback(AGPMPVP_CB_REMOVE_FRIEND, pcsCharacter, &pcsPvPCharInfo->m_lCID);

	//	if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolCharInfo.Free(pcsPvPCharInfo);
	//	else
	//		DestroyModuleData((PVOID)pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);
	//}

	//pcsAttachedPvP->m_pFriendList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return AddFriend(pcsCharacter, pcsFriend->m_lID);
}

BOOL AgpmPvP::AddFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// �̹� �ִ� �� �˻�
	if(IsFriend(pcsCharacter, lFriendCID))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// �ִ�ġ �˻�
	// ���� �� �ʿ� ������?
	//if(pcsAttachedPvP->m_pFriendVector->size() >= AGPMPVP_MAX_FRIEND_COUNT)
	//	return FALSE;

	AgpdPvPCharInfo csPvPCharInfo;
	csPvPCharInfo.m_lCID = lFriendCID;
	pcsAttachedPvP->m_pFriendVector->push_back(csPvPCharInfo);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	//
	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	pcsPvPCharInfo	= (AgpdPvPCharInfo *) m_csMemoryPoolCharInfo.Alloc();
	//else
	//	pcsPvPCharInfo	= (AgpdPvPCharInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_CHAR_INFO);

	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsPvPCharInfo->m_lCID = lFriendCID;
	//pcsPvPCharInfo->m_ulLastCombatClock = 0;		// ���� ���� �ʿ����.

	//return pcsAttachedPvP->m_pFriendList->AddObject((PVOID)pcsPvPCharInfo, lFriendCID) ? TRUE : FALSE;
}

BOOL AgpmPvP::RemoveFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return RemoveFriend(pcsCharacter, pcsFriend->m_lID);
}

BOOL AgpmPvP::RemoveFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	//// ���� �� �ƴ϶�� �� ������.
	//if(!IsFriend(pcsCharacter, lFriendCID))
	//	return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end(), lFriendCID);
	if(iter == pcsAttachedPvP->m_pFriendVector->end())
		return FALSE;

	pcsAttachedPvP->m_pFriendVector->erase(iter);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = GetFriendInfo(pcsCharacter, lFriendCID);
	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pFriendList->RemoveObject(lFriendCID);

	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolCharInfo.Free((PVOID)pcsPvPCharInfo);
	//else
	//	DestroyModuleData((PVOID)pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);
	
	//return TRUE;
}

inline BOOL AgpmPvP::UpdateFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPCharInfo* pcsCharInfo = GetFriendInfo(pcsCharacter, lFriendCID);
	if(!pcsCharInfo)
		return FALSE;

	// ���� �� ������ ���޸� �� �� ����.
	return TRUE;
}

inline BOOL AgpmPvP::IsFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return IsFriend(pcsCharacter, pcsFriend->m_lID);
}

inline BOOL AgpmPvP::IsFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetFriendInfo(pcsCharacter, lFriendCID) ? TRUE : FALSE;
}

inline AgpdPvPCharInfo* AgpmPvP::GetFriendInfo(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend)
{
	if(!pcsCharacter || !pcsFriend)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetFriendInfo(pcsCharacter, pcsFriend->m_lID);
}

// �׳� ����ü�� ��ȯ�ϰ� ������...
// �ϴ��� ������ �Լ��� �״�� ���󰣴�.
inline AgpdPvPCharInfo* AgpmPvP::GetFriendInfo(AgpdCharacter* pcsCharacter, INT32 lFriendCID)
{
	if(!pcsCharacter || !lFriendCID)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pFriendVector->begin(), pcsAttachedPvP->m_pFriendVector->end(), lFriendCID);
	if(iter == pcsAttachedPvP->m_pFriendVector->end())
		return NULL;

	return &(*iter);
	//return (AgpdPvPCharInfo*)pcsAttachedPvP->m_pFriendList->GetObject(lFriendCID);
}

// �̹� EnemyList �� ���� ApAdmin �� �޸� ������ ���� �����̰�,
// ����Ʈ�� ����ش�.
BOOL AgpmPvP::InitEnemyList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pEnemyVector)
		return FALSE;
	
	PvPCharIter iter = pcsAttachedPvP->m_pEnemyVector->begin();
	while(iter != pcsAttachedPvP->m_pEnemyVector->end())
	{
		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY, pcsCharacter, &iter->m_lCID);
		++iter;
	}

	pcsAttachedPvP->m_pEnemyVector->clear();
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPCharInfo* pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex); pcsPvPCharInfo;
	//		pcsPvPCharInfo = (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObjectSequence(&lIndex))
	//{
	//	EnumCallback(AGPMPVP_CB_REMOVE_ENEMY, pcsCharacter, &pcsPvPCharInfo->m_lCID);

	//	if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolCharInfo.Free((PVOID)pcsPvPCharInfo);
	//	else
	//		DestroyModuleData((PVOID)pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);
	//}

	//pcsAttachedPvP->m_pEnemyList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return AddEnemy(pcsCharacter, pcsEnemy->m_lID, ulLastCombatClock);
}

BOOL AgpmPvP::AddEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// �̹� �ִ� �� �˻�
	if(IsEnemy(pcsCharacter, lEnemyCID))
	{
		UpdateEnemy(pcsCharacter, lEnemyCID, cPvPMode, ulLastCombatClock);
		return FALSE;
	}

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPCharInfo csPvPCharInfo;
	csPvPCharInfo.m_lCID = lEnemyCID;
	csPvPCharInfo.m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	csPvPCharInfo.m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	pcsAttachedPvP->m_pEnemyVector->push_back(csPvPCharInfo);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = NULL;
	//
	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	pcsPvPCharInfo	= (AgpdPvPCharInfo *) m_csMemoryPoolCharInfo.Alloc();
	//else
 //       pcsPvPCharInfo	= (AgpdPvPCharInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_CHAR_INFO);

	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsPvPCharInfo->m_lCID = lEnemyCID;
	//pcsPvPCharInfo->m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	//pcsPvPCharInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	//return pcsAttachedPvP->m_pEnemyList->AddObject((PVOID)pcsPvPCharInfo, lEnemyCID) ? TRUE : FALSE;
}

BOOL AgpmPvP::RemoveEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return RemoveEnemy(pcsCharacter, pcsEnemy->m_lID);
}

BOOL AgpmPvP::RemoveEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pEnemyVector->begin(), pcsAttachedPvP->m_pEnemyVector->end(), lEnemyCID);
	if(iter == pcsAttachedPvP->m_pEnemyVector->end())
		return FALSE;

	pcsAttachedPvP->m_pEnemyVector->erase(iter);
	return TRUE;

	//AgpdPvPCharInfo* pcsPvPCharInfo = GetEnemyInfo(pcsCharacter, lEnemyCID);
	//if(!pcsPvPCharInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pEnemyList->RemoveObject(lEnemyCID);

	//if (m_csMemoryPoolCharInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolCharInfo.Free((PVOID)pcsPvPCharInfo);
	//else
	//	DestroyModuleData(pcsPvPCharInfo, AGPMPVP_DATA_TYPE_CHAR_INFO);

	//return TRUE;
}

inline BOOL AgpmPvP::UpdateEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPCharInfo* pcsCharInfo = GetEnemyInfo(pcsCharacter, lEnemyCID);
	if(!pcsCharInfo)
		return FALSE;

	if(cPvPMode != -1)
		pcsCharInfo->m_ePvPMode = (eAgpdPvPMode)cPvPMode;
	pcsCharInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;
	return TRUE;
}

BOOL AgpmPvP::IsEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return IsEnemy(pcsCharacter, pcsEnemy->m_lID);
}

BOOL AgpmPvP::IsEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID)
{
	if(!pcsCharacter || !lEnemyCID)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetEnemyInfo(pcsCharacter, lEnemyCID) ? TRUE : FALSE;
}

inline AgpdPvPCharInfo* AgpmPvP::GetEnemyInfo(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy)
{
	if(!pcsCharacter || !pcsEnemy)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetEnemyInfo(pcsCharacter, pcsEnemy->m_lID);
}

inline AgpdPvPCharInfo* AgpmPvP::GetEnemyInfo(AgpdCharacter* pcsCharacter, INT32 lEnemyCID)
{
	if(!pcsCharacter || !lEnemyCID)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPCharIter iter = std::find(pcsAttachedPvP->m_pEnemyVector->begin(), pcsAttachedPvP->m_pEnemyVector->end(), lEnemyCID);
	if(iter == pcsAttachedPvP->m_pEnemyVector->end())
		return NULL;

	return &(*iter);
	//return (AgpdPvPCharInfo*)pcsAttachedPvP->m_pEnemyList->GetObject(lEnemyCID);
}

BOOL AgpmPvP::InitFriendGuildList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pFriendGuildVector)
		return FALSE;

	PvPGuildIter iter = pcsAttachedPvP->m_pFriendGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pFriendGuildVector->end())
	{
		EnumCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pcsCharacter, iter->m_szGuildID);
		++iter;
	}

	pcsAttachedPvP->m_pFriendGuildVector->clear();
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObjectSequence(&lIndex))
	//{
	//	EnumCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pcsCharacter, pcsPvPGuildInfo->m_szGuildID);

	//	if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//	else
	//		DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);
	//}

	//pcsAttachedPvP->m_pFriendGuildList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	if(IsFriendGuild(pcsCharacter, szGuildID))
		return FALSE;

	AgpdPvPGuildInfo csGuildInfo;
	_tcscpy(csGuildInfo.m_szGuildID, szGuildID);
	csGuildInfo.m_ulLastCombatClock = GetClockCount();

	pcsAttachedPvP->m_pFriendGuildVector->push_back(csGuildInfo);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	//
	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo *)	m_csMemoryPoolGuildInfo.Alloc();
	//else
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_GUILD_INFO);

	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//strcpy(pcsPvPGuildInfo->m_szGuildID, szGuildID);
	//pcsPvPGuildInfo->m_ulLastCombatClock = GetClockCount();
	//pcsAttachedPvP->m_pFriendGuildList->AddObject((PVOID)pcsPvPGuildInfo, pcsPvPGuildInfo->m_szGuildID);
 //   
	//return TRUE;
}

BOOL AgpmPvP::RemoveFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pFriendGuildVector->begin(), pcsAttachedPvP->m_pFriendGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pFriendGuildVector->end())
		return FALSE;

	pcsAttachedPvP->m_pFriendGuildVector->erase(iter);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = GetFriendGuildInfo(pcsCharacter, szGuildID);
	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pFriendGuildList->RemoveObject(szGuildID);

	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//else
	//	DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);

	//return TRUE;
}

inline BOOL AgpmPvP::UpdateFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPGuildInfo* pcsPvPGuildInfo = GetFriendGuildInfo(pcsCharacter, szGuildID);
	if(!pcsPvPGuildInfo)
		return FALSE;

	// Update �� ������� ����(2005.01.12) �μ��� ������ CombatClock �� ������Ʈ ���ָ� �ȴ�.
	pcsPvPGuildInfo->m_ulLastCombatClock = GetClockCount();

	return TRUE;
}

inline BOOL AgpmPvP::IsFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// �ڱ� ��� üũ�� ���� �ʴ´�. -0-;;; 2005.01.19. steeple
	// �ڱ� ����̸� return TRUE
	//CHAR* szSelfGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	//if(szSelfGuildID && strlen(szSelfGuildID) > 0 && strcmp(szSelfGuildID, szGuildID) == 0)
	//	return TRUE;

	return GetFriendGuildInfo(pcsCharacter, szGuildID) ? TRUE : FALSE;
}

inline AgpdPvPGuildInfo* AgpmPvP::GetFriendGuildInfo(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pFriendGuildVector->begin(), pcsAttachedPvP->m_pFriendGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pFriendGuildVector->end())
		return NULL;

	return &(*iter);

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pFriendGuildList->GetObject(szGuildID);
	//return pcsPvPGuildInfo;
}

BOOL AgpmPvP::InitEnemyGuildList(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP || !pcsAttachedPvP->m_pEnemyGuildVector)
		return FALSE;

	// 2005.04.18. steeple
	// ����� ���϶��� ������� ���� �ȵȴ�.
	CHAR* szJoinedGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);

	PvPGuildIter iter = pcsAttachedPvP->m_pEnemyGuildVector->begin();
	while(iter != pcsAttachedPvP->m_pEnemyGuildVector->end())
	{
		// 2005.04.18. steeple
		// ����� ���϶��� ������� ���� �ȵȴ�.
		if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, iter->m_szGuildID))
		{
			++iter;
			continue;
		}

		EnumCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pcsCharacter, iter->m_szGuildID);

		iter = pcsAttachedPvP->m_pEnemyGuildVector->erase(iter);
	}

	// ��� vector clear ���� �ʴ´�.
	return TRUE;

	//INT32 lIndex = 0;
	//for(AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex); pcsPvPGuildInfo;
	//		pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObjectSequence(&lIndex))
	//{
	//	// 2005.04.18. steeple
	//	// ����� ���϶��� ������� ���� �ȵȴ�.
	//	if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, pcsPvPGuildInfo->m_szGuildID))
	//		continue;

	//	EnumCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pcsCharacter, pcsPvPGuildInfo->m_szGuildID);

	//	// 2005.04.18. steeple
	//	// Remove �� ���ش�.
	//	pcsAttachedPvP->m_pEnemyGuildList->RemoveObject(pcsPvPGuildInfo->m_szGuildID);
	//	//lIndex--;		// �̰� ������ ��� �ǳʶٰ� ��.

	//	if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//		m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//	else
	//		DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);

	//}

	//// 2005.04.18. steeple
	//// �̷��� �ϸ� �ȵǰ�
	////pcsAttachedPvP->m_pEnemyGuildList->Initialize();		
	//return TRUE;
}

BOOL AgpmPvP::AddEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	if(IsEnemyGuild(pcsCharacter, szGuildID))
	{
		UpdateEnemyGuild(pcsCharacter, szGuildID, cPvPMode, ulLastCombatClock);
		return TRUE;
	}

	AgpdPvPGuildInfo csPvPGuildInfo;
	_tcscpy(csPvPGuildInfo.m_szGuildID, szGuildID);
	csPvPGuildInfo.m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	csPvPGuildInfo.m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	pcsAttachedPvP->m_pEnemyGuildVector->push_back(csPvPGuildInfo);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = NULL;
	//
	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo *) m_csMemoryPoolGuildInfo.Alloc();
	//else
	//	pcsPvPGuildInfo	= (AgpdPvPGuildInfo*)CreateModuleData(AGPMPVP_DATA_TYPE_GUILD_INFO);

	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//strcpy(pcsPvPGuildInfo->m_szGuildID, szGuildID);
	//pcsPvPGuildInfo->m_ePvPMode = cPvPMode == -1 ? pcsAttachedPvP->m_ePvPMode : (eAgpdPvPMode)cPvPMode;
	//pcsPvPGuildInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;
	//pcsAttachedPvP->m_pEnemyGuildList->AddObject((PVOID)pcsPvPGuildInfo, pcsPvPGuildInfo->m_szGuildID);
 
	//return TRUE;
}

BOOL AgpmPvP::RemoveEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	// 2007.01.04. steeple
	// ����� ���̶�� ������ �ʴ´�.
	CHAR* szJoinedGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	if(szJoinedGuildID && m_pagpmGuild->IsEnemyGuild(szJoinedGuildID, szGuildID, TRUE))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pEnemyGuildVector->begin(), pcsAttachedPvP->m_pEnemyGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pEnemyGuildVector->end())
		return FALSE;

	pcsAttachedPvP->m_pEnemyGuildVector->erase(iter);
	return TRUE;

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = GetEnemyGuildInfo(pcsCharacter, szGuildID);
	//if(!pcsPvPGuildInfo)
	//	return FALSE;

	//pcsAttachedPvP->m_pEnemyGuildList->RemoveObject(szGuildID);

	//if (m_csMemoryPoolGuildInfo.GetTypeIndex() != (-1))
	//	m_csMemoryPoolGuildInfo.Free(pcsPvPGuildInfo);
	//else
	//	DestroyModuleData(pcsPvPGuildInfo, AGPMPVP_DATA_TYPE_GUILD_INFO);

	//return TRUE;
}

inline BOOL AgpmPvP::UpdateEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode, UINT32 ulLastCombatClock)
{
	if(!pcsCharacter || !szGuildID)
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	AgpdPvPGuildInfo* pcsPvPGuildInfo = GetEnemyGuildInfo(pcsCharacter, szGuildID);
	if(!pcsPvPGuildInfo)
		return FALSE;

	if(cPvPMode != -1)
		pcsPvPGuildInfo->m_ePvPMode = (eAgpdPvPMode)cPvPMode;
	pcsPvPGuildInfo->m_ulLastCombatClock = ulLastCombatClock == 0 ? GetClockCount() : ulLastCombatClock;

	return TRUE;
}

inline BOOL AgpmPvP::IsEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	return GetEnemyGuildInfo(pcsCharacter, szGuildID) ? TRUE : FALSE;
}

inline AgpdPvPGuildInfo* AgpmPvP::GetEnemyGuildInfo(AgpdCharacter* pcsCharacter, CHAR* szGuildID)
{
	if(!pcsCharacter || !szGuildID || strlen(szGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)
		return NULL;

	if(!IsPC(pcsCharacter))
		return NULL;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return NULL;

	PvPGuildIter iter = std::find(pcsAttachedPvP->m_pEnemyGuildVector->begin(), pcsAttachedPvP->m_pEnemyGuildVector->end(), szGuildID);
	if(iter == pcsAttachedPvP->m_pEnemyGuildVector->end())
		return NULL;

	return &(*iter);

	//AgpdPvPGuildInfo* pcsPvPGuildInfo = (AgpdPvPGuildInfo*)pcsAttachedPvP->m_pEnemyGuildList->GetObject(szGuildID);
	//return pcsPvPGuildInfo;
}







//////////////////////////////////////////////////////////////////////////
// Drop Probability

// pcsCharacter �� �̰��. pcsTarget �� Ȯ���� üũ�Ѵ�.
INT32 AgpmPvP::GetItemDropProbability(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return -1;

	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return -1;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedTarget = GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP || !pcsAttachedTarget)
		return -1;

	INT32 lProbability = -1;
	INT32 lRand = 0;

	// ���� ������ ���� Ȯ���� ���Ѵ�.
	switch(pcsAttachedPvP->m_ePvPMode)
	{
		case AGPDPVP_MODE_SAFE:
			break;

		case AGPDPVP_MODE_FREE:
		case AGPDPVP_MODE_COMBAT:
		{
			AgpdPvPAreaDrop* pcsPvPAreaDrop = GetAreaDrop(pcsAttachedPvP->m_ePvPMode);
			if(pcsPvPAreaDrop)
			{
				lRand = m_csRandom.randInt(100);
				if(lRand < pcsPvPAreaDrop->m_nAreaProbability)	// ���� ���� Ȯ�� üũ
				{
					// Equip ���� Inven ���� ����
					lRand = m_csRandom.randInt(100);
					if(lRand < pcsPvPAreaDrop->m_nEquipProbability)	// Equip
						lProbability = AGPMPVP_ITEM_DROP_POS_EQUIP;
					else if(lRand < (pcsPvPAreaDrop->m_nEquipProbability + pcsPvPAreaDrop->m_nInvenProbability))	// Inven
						lProbability = AGPMPVP_ITEM_DROP_POS_INVEN;
				}
			}
			break;
		}
	}

	// Drop ���ϸ� ����
	if(lProbability < 0)
		return -1;

	// ������ Drop �� Equip �Ǵ� Inven �� ��ġ Ȯ�� ���
	lRand = m_csRandom.randInt(100);
	AgpdPvPItemDrop** ppcsPvPItemDrop = NULL;
	AgpdPvPItemDrop* pcsPvPItemDrop = NULL;
	INT32 lIndex = 0, lPrevProbabilitySum = 0;
	for(ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex); ppcsPvPItemDrop;
		ppcsPvPItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObjectSequence(&lIndex))
	{
		pcsPvPItemDrop = *ppcsPvPItemDrop;
		if(!pcsPvPItemDrop)
			break;

		if(lProbability & AGPMPVP_ITEM_DROP_POS_EQUIP)		// Equip �� ��
		{
			if(pcsPvPItemDrop->m_nEquipProbability == 0)
				continue;

			if(lRand < (lPrevProbabilitySum + pcsPvPItemDrop->m_nEquipProbability))
			{
				lProbability |= pcsPvPItemDrop->m_lIndex;
				break;
			}

			// Ȯ���� �� �ɷ�����
			lPrevProbabilitySum += pcsPvPItemDrop->m_nEquipProbability;
		}
		else if(lProbability & AGPMPVP_ITEM_DROP_POS_INVEN)	// Inven �� ��
		{
			if(pcsPvPItemDrop->m_nInvenProbability == 0)
				continue;

			if(lRand < (lPrevProbabilitySum + pcsPvPItemDrop->m_nInvenProbability))
			{
				lProbability |= pcsPvPItemDrop->m_lIndex;
				break;
			}

			// Ȯ���� �� �ɷ�����
			lPrevProbabilitySum += pcsPvPItemDrop->m_nInvenProbability;
		}
	}

	return lProbability;
}

// pcsCharacter �� �̰��. ���� ���� ���̷� Ȯ�� ��� �� �ذ� ������ ����.
INT32 AgpmPvP::GetSkullDropLevel(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!m_pagpmFactors)
		return -1;

	if(!pcsCharacter || !pcsTarget)
		return -1;

	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return -1;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	AgpdPvPADChar* pcsAttachedTarget = GetADCharacter(pcsTarget);
	if(!pcsAttachedPvP || !pcsAttachedTarget)
		return -1;

	// ���� �� ĳ������ ������ ���Ѵ�.
	INT32 lLevel, lTargetLevel;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	m_pagpmFactors->GetValue(&pcsTarget->m_csFactor, &lTargetLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	// ���� ���̸� ���ϰ�
	INT32 lLevelGap = lLevel - lTargetLevel;
	if(lLevelGap < m_lSkullDropMinLevelGap)
		lLevelGap = m_lSkullDropMinLevelGap;
	if(lLevelGap > m_lSkullDropMaxLevelGap)
		lLevelGap = m_lSkullDropMaxLevelGap;

	AgpdPvPSkullDrop* pcsPvPSkullDrop = GetSkullDrop(lLevelGap);
	if(!pcsPvPSkullDrop)
		return -1;

	INT32 lRand = m_csRandom.randInt(100);
	if(lRand >= pcsPvPSkullDrop->m_lSkullProbability)
		return -1;

	INT32 lSkullLevel = -1;
	// ������� ������ �˻��ؼ� ��´�.
	for(int i = 0; i < AGPMPVP_MAX_SKULL_LEVEL_BRANCH; i++)
	{
		if(lTargetLevel <= m_alSkullDropLevelBranch[i])
		{
			lSkullLevel = m_alSkullDropLevel[i];
			break;
		}
	}

	return lSkullLevel;
}

AgpdPvPAreaDrop* AgpmPvP::GetAreaDrop(eAgpdPvPMode ePvPMode)
{
	AgpdPvPAreaDrop** ppcsAreaDrop = (AgpdPvPAreaDrop**)m_csAreaDropAdmin.GetObject((INT32)ePvPMode);
	if(ppcsAreaDrop && *ppcsAreaDrop)
		return *ppcsAreaDrop;
	else
		return NULL;
}

AgpdPvPItemDrop* AgpmPvP::GetItemDrop(INT32 lIndex)
{
	AgpdPvPItemDrop** ppcsItemDrop = (AgpdPvPItemDrop**)m_csItemDropAdmin.GetObject(lIndex);
	if(ppcsItemDrop && *ppcsItemDrop)
		return *ppcsItemDrop;
	else
		return NULL;
}

AgpdPvPSkullDrop* AgpmPvP::GetSkullDrop(INT32 lLevelGap)
{
	if(lLevelGap == 0)
		lLevelGap = AGPMPVP_LEVEL_GAP_ZERO;

	AgpdPvPSkullDrop** ppcsSkullDrop = (AgpdPvPSkullDrop**)m_csSkullDropAdmin.GetObject(lLevelGap);
	if(ppcsSkullDrop && *ppcsSkullDrop)
		return *ppcsSkullDrop;
	else
		return NULL;
}

// Region �� �ٲ� �� �Ҹ���.
BOOL AgpmPvP::RegionChange(AgpdCharacter* pcsCharacter, INT16 nPrevRegionIndex)
{
	if(!pcsCharacter)
		return FALSE;

	// ������ ���� ������.
	if(!m_pagpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	// �����϶��� ������.
	if(strlen(pcsCharacter->m_szID) == 0)
		return TRUE;

    AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	eAgpdPvPMode ePrevPvPMode = AGPDPVP_MODE_NONE;

	// 2005.06.01. steeple
	// ADD_CHARACTER_TO_MAP ���� �ҷȴٸ�, �� RegionIndex �� �������ش�.
	// �׸��� ���� PvPMode �� AttachedPvP �����Ϳ��� �����´�.
	if(nPrevRegionIndex == -1)
	{
		//pcsCharacter->m_nBindingRegionIndex = m_papmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);
		ePrevPvPMode = pcsAttachedPvP->m_ePvPMode;
	}
	else
		ePrevPvPMode = GetPvPModeByRegion(nPrevRegionIndex);

	eAgpdPvPMode eNowPvPMode = GetPvPModeByPosition(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);

	// �׸��� ���ο� �� ����
	pcsAttachedPvP->m_ePvPMode = eNowPvPMode;
	
	if(eNowPvPMode == AGPDPVP_MODE_SAFE && ePrevPvPMode != AGPDPVP_MODE_SAFE)
	{
		// ���������� ����
		EnumCallback(AGPMPVP_CB_ENTER_SAFE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode != AGPDPVP_MODE_SAFE && ePrevPvPMode == AGPDPVP_MODE_SAFE)
	{
		// ������������ ������
		EnumCallback(AGPMPVP_CB_LEAVE_SAFE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode == AGPDPVP_MODE_FREE && ePrevPvPMode != AGPDPVP_MODE_FREE)
	{
		// ���������� ����
		EnumCallback(AGPMPVP_CB_ENTER_FREE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode != AGPDPVP_MODE_FREE && ePrevPvPMode == AGPDPVP_MODE_FREE)
	{
		// ������������ ������
		EnumCallback(AGPMPVP_CB_LEAVE_FREE_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode == AGPDPVP_MODE_COMBAT && ePrevPvPMode != AGPDPVP_MODE_COMBAT)
	{
		// ���������� ����
		EnumCallback(AGPMPVP_CB_ENTER_COMBAT_PVP_AREA, pcsCharacter, NULL);
	}
	
	if(eNowPvPMode != AGPDPVP_MODE_COMBAT && ePrevPvPMode == AGPDPVP_MODE_COMBAT)
	{
		// ������������ ������
		EnumCallback(AGPMPVP_CB_LEAVE_COMBAT_PVP_AREA, pcsCharacter, NULL);
	}

	return TRUE;
}

// ���� ��带 ���ؼ� �����Ѵ�.
inline BOOL AgpmPvP::IsSafePvPMode(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	return pcsAttachedPvP->m_ePvPMode == AGPDPVP_MODE_SAFE ? TRUE : FALSE;
}

// ���� ��带 ���ؼ� �����Ѵ�.
inline BOOL AgpmPvP::IsFreePvPMode(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	return pcsAttachedPvP->m_ePvPMode == AGPDPVP_MODE_FREE ? TRUE : FALSE;
}
// ���� ��带 ���ؼ� �����Ѵ�.
inline BOOL AgpmPvP::IsCombatPvPMode(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	return pcsAttachedPvP->m_ePvPMode == AGPDPVP_MODE_COMBAT ? TRUE : FALSE;
}

// X, Z ��ǥ�� ���� ������ �� PvP ��带 �����Ѵ�.
inline eAgpdPvPMode AgpmPvP::GetPvPModeByPosition(FLOAT fX, FLOAT fZ)
{
	INT16 nRegionIndex = m_papmMap->GetRegion(fX, fZ);
	return GetPvPModeByRegion(nRegionIndex);
}

// Region Index �κ��� ���� ������ �� PvP ��带 �����Ѵ�.
inline eAgpdPvPMode AgpmPvP::GetPvPModeByRegion(INT16 nRegionIndex)
{
	ApmMap::RegionTemplate* pRegionTemplate = m_papmMap->GetTemplate(nRegionIndex);
	if(!pRegionTemplate)
		return AGPDPVP_MODE_NONE;

	switch(pRegionTemplate->ti.stType.uSafetyType)
	{
		case ApmMap::ST_SAFE:
			return AGPDPVP_MODE_SAFE;
		case ApmMap::ST_FREE:
			return AGPDPVP_MODE_FREE;
		case ApmMap::ST_DANGER:
			return AGPDPVP_MODE_COMBAT;
	}

	return AGPDPVP_MODE_NONE;
}

// ���� PvP �ϰ� �ִ��� ����
BOOL AgpmPvP::IsNowPvP(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// Status �� NOWPVP �̸�
	if(pcsAttachedPvP->m_cPvPStatus & AGPDPVP_STATUS_NOWPVP)
		return TRUE;

	// ���� ������
	if(!pcsAttachedPvP->m_pEnemyVector->empty() || !pcsAttachedPvP->m_pEnemyGuildVector->empty())
		return TRUE;

	return FALSE;
}

// ���� ���� �������� ����
inline BOOL AgpmPvP::IsInvincible(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	if(pcsAttachedPvP->m_cPvPStatus & AGPDPVP_STATUS_INVINCIBLE)
		return TRUE;

	return FALSE;
}

// PvP ���¸� ���� üũ�Ѵ�.
BOOL AgpmPvP::UpdatePvPStatus(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	// ���� ������
	if(!pcsAttachedPvP->m_pEnemyVector->empty() || !pcsAttachedPvP->m_pEnemyGuildVector->empty())
		pcsAttachedPvP->m_cPvPStatus |= AGPDPVP_STATUS_NOWPVP;
	else
	{
		if(pcsAttachedPvP->m_cPvPStatus & AGPDPVP_STATUS_NOWPVP)
			pcsAttachedPvP->m_cPvPStatus -= AGPDPVP_STATUS_NOWPVP;
	}

	return TRUE;
}

// CombatArea ���� �׾����� �����Ѵ�.
BOOL AgpmPvP::SetDeadInCombatArea(AgpdCharacter* pcsCharacter, BOOL bDead)
{
	if(!pcsCharacter)
		return FALSE;

	if(!IsPC(pcsCharacter))
		return FALSE;

	AgpdPvPADChar* pcsAttachedPvP = GetADCharacter(pcsCharacter);
	if(!pcsAttachedPvP)
		return FALSE;

	pcsAttachedPvP->m_bDeadInCombatArea = bDead;

	return TRUE;
}


eAGPMPVP_RACE_BATTLE_STATUS AgpmPvP::GetRaceBattleStatus()
{
	return m_eRaceBattleStatus;
}

void AgpmPvP::SetRaceBattleStatus(eAGPMPVP_RACE_BATTLE_STATUS eStatus)
{
	m_eRaceBattleStatus = eStatus;
}

BOOL AgpmPvP::IsInRaceBattle()
{
	eAGPMPVP_RACE_BATTLE_STATUS eStatus = GetRaceBattleStatus();
	if (AGPMPVP_RACE_BATTLE_STATUS_ING == eStatus)
		return TRUE;
	
	return FALSE;
}

BOOL AgpmPvP::IsInRaceBattleReward()
{
	eAGPMPVP_RACE_BATTLE_STATUS eStatus = GetRaceBattleStatus();
	if (eStatus == AGPMPVP_RACE_BATTLE_STATUS_REWARD)
		return TRUE;
	
	return FALSE;
}


void AgpmPvP::SetRaceBattleResult()
{
	INT32 lRace = AURACE_TYPE_NONE;
	INT32 lMax = 0;
	for (INT32 i = AURACE_TYPE_HUMAN; i < AURACE_TYPE_MAX; i++)
	{
		if (m_lRaceBattlePoint[i] > lMax)
		{
			lMax = m_lRaceBattlePoint[i];
			lRace = i;
		}
	}
	m_lRaceBattleWinner = lRace;
}


INT32 AgpmPvP::GetRaceBattleWinner()
{
	if (IsInRaceBattleReward())
		return m_lRaceBattleWinner;
	
	return AURACE_TYPE_NONE;
}




//////////////////////////////////////////////////////////////////////////
// Etc
inline BOOL AgpmPvP::IsPC(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(m_pagpmCharacter->IsPC(pcsCharacter) &&
		strlen(pcsCharacter->m_szID) > 0)	// �̸� üũ ����� �� �־���ȴ�. 2005.09.30. steeple
		return TRUE;
	else
		return FALSE;
}

// pcsTarget �� pcsCharacter �� ���������� Ȯ��. �Ϲ�+���
inline BOOL AgpmPvP::IsFriendCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// �Ѵ� PC ���� ��
	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return FALSE;

	if(IsFriend(pcsCharacter, pcsTarget->m_lID))
		return TRUE;

	CHAR* szGuildID1 = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	CHAR* szGuildID2 = m_pagpmGuild->GetJoinedGuildID(pcsTarget);
	if(szGuildID1 && szGuildID2 &&
		strlen(szGuildID1) > 0 && strlen(szGuildID2) > 0 && strcmp(szGuildID1, szGuildID2) == 0)
		return TRUE;

	if(szGuildID2 && strlen(szGuildID2) > 0 && IsFriendGuild(pcsCharacter, szGuildID2))
		return TRUE;

	//if(szGuildID1 && szGuildID2 && _tcslen(szGuildID1) > 0 && _tcslen(szGuildID2) > 0)
	//{
	//	// ������ ������ ����� �� �� �ִ�.
 //       if(m_pagpmGuild->IsJointGuild(szGuildID1, szGuildID2))
	//		return TRUE;
	//}

	return FALSE;
}

// pcsTarget �� pcsCharacter �� ������ Ȯ��. �Ϲ� �� + ��� ��
inline BOOL AgpmPvP::IsEnemyCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bServer )
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// �Ѵ� PC ���� ��
	if(!IsPC(pcsCharacter) || !IsPC(pcsTarget))
		return FALSE;

	if(IsEnemy(pcsCharacter, pcsTarget->m_lID))
		return TRUE;

	// EnemyGuild �̸� return TRUE
	CHAR* szGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	CHAR* szTargetGuildID = m_pagpmGuild->GetJoinedGuildID(pcsTarget);
	if(szTargetGuildID && _tcslen(szTargetGuildID) > 0)
	{
		// 2007.01.03. steeple
		// ����� ���̶�� ��ҿ� ������� ���� TRUE
		if(szGuildID && _tcslen(szGuildID) > 0 )
		{
			// ���� ���°� ����� ������ �ٽ� �ѹ�Ȯ��.
			// ���� �ؾ��ϱ� ������ �ʰ� ó���Ѵ�.
			// ������ �׳� �ϴ� �� ���� �ʾҴ�. ū ������ ���� �� ���� �ʾƼ�.
			AgpdGuild* pcsGuild			= m_pagpmGuild->GetGuild(szGuildID);
			AgpdGuild* pcsTargetGuild	= m_pagpmGuild->GetGuild( szTargetGuildID );

			if( m_pagpmGuild->IsBattleIngStatus(pcsGuild) && m_pagpmGuild->IsBattleIngStatus(pcsTargetGuild) 
				&& m_pagpmGuild->IsEnemyGuild(szGuildID, szTargetGuildID) 
				&& m_pagpmGuild->IsEnemyGuild(szTargetGuildID, szGuildID))
			{
				if( bServer )
				{
					if( pcsGuild->IsOnBattle( pcsCharacter->m_szID ) && pcsTargetGuild->IsOnBattle( pcsTarget->m_szID ) )
					{
						/*if( m_pagpmCharacter->IsInSiege(pcsCharacter) || m_pagpmCharacter->IsInSiege(pcsTarget) ||
							m_pagpmCharacter->IsInDungeon(pcsCharacter) || m_pagpmCharacter->IsInDungeon(pcsTarget))
							return FALSE;
						else*/
							return TRUE;
					}
				}
				else
					return FALSE;
			}
		}

		// 2007.01.03. steeple
		// ����.
		//
		// ������ �����ϰ�, ����� �Ǵ� �������� �ο�����.
		if(!IsSafePvPMode(pcsCharacter) && !IsSafePvPMode(pcsTarget) &&
			(IsEnemyGuild(pcsCharacter, szTargetGuildID) ||
			m_pagpmGuild->IsHostileGuild(szGuildID, szTargetGuildID)))
			return TRUE;
	}

	return FALSE;
}

// Attack ������ ĳ�������� �˻�
// pcsCharacter �� pcsTarget �� �д� ����.
BOOL AgpmPvP::IsAttackable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce)
{
	//STOPWATCH2(GetModuleName(), _T("IsAttackable"));

	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	if(pcsCharacter == pcsTarget)
		return TRUE;

	// PvP�� ����ε� PvP�� ���ϰ� �����Ǿ��ִ��� �˻��Ѵ�.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
	{
		if(m_pagpmConfig->IsEnablePvP() == FALSE)
			return FALSE;
	}

	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
	{
		// PVP�� ����ε� Volunteer��� ���ϰ� �Ѵ�.
		if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_VOLUNTEER ||
			pcsTarget->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_VOLUNTEER )
		{
			return FALSE;
		}
	}

	// � ������ �Ͼ�� �ȵǴ� ������ �˻��Ѵ�.
	if(!EnumCallback(AGPMPVP_CB_IS_ATTACKABLE_TIME, pcsCharacter, pcsTarget))
		return FALSE;

	// �����ڰ� ���� �Ұ��� �����̴�. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusDisableNormalATK(pcsCharacter))
	{
		if(m_pagpmCharacter->IsPC(pcsCharacter))
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_DISABLE_NORMAL_ATTACK, NULL, NULL, 0, 0, pcsCharacter);

		return FALSE;
	}

	// Disable �����̸� ������ �� ����. 2007.07.18. steeple
	if(m_pagpmCharacter->IsDisableCharacter(pcsCharacter) || m_pagpmCharacter->IsDisableCharacter(pcsTarget))
		return FALSE;

	// 2005.08.29. steeple
	// �� �� �ϳ��� Summons �̶�� ����üũ�� ����. Taming ����.
	if(m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusSummoner(pcsTarget) ||
		m_pagpmCharacter->IsStatusTame(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsTarget))
		return IsAttackableSummons(pcsCharacter, pcsTarget, bForce);

	// ������ ���� PC �� �ƴϸ�
	if(!m_pagpmCharacter->IsPC(pcsCharacter) || m_pagpmCharacter->IsGuard(pcsCharacter))
	{
		// ���� �����̸� �� ����
		if(m_pagpmCharacter->IsMonster(pcsCharacter) && m_pagpmCharacter->IsMonster(pcsTarget))
			return FALSE;
			
		// ��Ʋ �׶��� - arycoat 2008. 01.
		// ���� ������ �����Ҷ�...
		if( m_pagpmBattleGround->IsInBattleGround(pcsCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsTarget) )
		{
			return (!m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget));
		}
		
		return TRUE;
	}

	// 2006.09.19. steeple
	// ���Ż��� ũ�������� �˻�
	if((pcsCharacter->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsCharacter->m_pcsCharacterTemplate)) ||
		(pcsTarget->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsTarget->m_pcsCharacterTemplate)))
	{
		return FALSE;
	}

	// �´� ���� PC �� �ƴϸ� ���� üũ�� ����.
	if (!m_pagpmCharacter->IsPC(pcsTarget))
	{
		// Ÿ���� ���� ������Ʈ���� ���� ���� ������Ʈ��� ����, ������ �� ������ ���� ���� ���θ� Ȯ���Ѵ�.
		BOOL	bIsSiegeWarObject	= FALSE;
		BOOL	bCheckResult		= FALSE;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= pcsCharacter;
		pvBuffer[1]	= pcsTarget;
		pvBuffer[2]	= &bIsSiegeWarObject;
		pvBuffer[3]	= &bCheckResult;

		EnumCallback(AGPMPVP_CB_CHECK_NPC_ATTACKABLE_TARGET, pvBuffer, NULL);

		if (bIsSiegeWarObject)
			return bCheckResult;

		// Ʈ�� �߰� 2006.12.27. steeple
		if (!m_pagpmCharacter->IsAttackable(pcsTarget) || m_pagpmCharacter->IsTrap(pcsTarget))
			return FALSE;
			
		// ��Ʋ �׶��� - arycoat 2008. 01.
		// ������ ���� �����Ҷ�..
		if( m_pagpmBattleGround->IsInBattleGround(pcsCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsTarget) )
		{
			return (!m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget));
		}
		
		return TRUE;
	}
	else if(strlen(pcsTarget->m_szID) == 0)
	{
		// �̰�쿣 ���峪 NPC ��
		return FALSE;
	}

	// ��ų �����ڰ� �ڰ� ������ �Ұ�. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusSleep(pcsCharacter))
		return FALSE;

	// ��Ƽ���̰� Ÿ���� ��Ƽ���̸� ������.
	AgpdParty *pcsParty = m_pagpmParty->GetParty(pcsCharacter);
	if (NULL != pcsParty)
	{
		AuAutoLock LockParty(pcsParty->m_Mutex);
		if (LockParty.Result())
		{
			if (IsFreePvPMode(pcsCharacter) && m_pagpmParty->IsMember(pcsParty, pcsTarget->m_lID))
				return FALSE;
		}
	}

	// ������ �������¶�� return FALSE
	if(IsInvincible(pcsTarget) || m_pagpmCharacter->IsStatusInvincible(pcsTarget))
	{

		// ��Ʋ �׶����� ��� �ý��� �޼������� ĳ���͸���
		// ******* �� �ٲ۴�
		if(m_pagpmCharacter->IsPC(pcsCharacter))
		{

			string	strName		=	pcsTarget->m_szID;

			if( m_pagpmBattleGround->IsInBattleGround( pcsTarget ) || 
				APMMAP_PECULIARITY_RETURN_DISABLE_USE == m_papmMap->CheckRegionPerculiarity( pcsTarget->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME ) )
			{
				strName.replace( strName.begin() , strName.end() , strName.size() , '*' );
			}

			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_TARGET_INVINCIBLE, (char*)strName.c_str(), NULL, 0, 0, pcsCharacter);
		}

		return FALSE;
	}

	// ������ �����̸� �������� ���Ѵ�.
	if(m_pagpmCharacter->IsStatusFullTransparent(pcsTarget))
		return FALSE;
		
	// ��Ʋ �׶��� - arycoat 2008. 01.
	// ������ ������ �����Ҷ�...
	if( m_pagpmBattleGround->IsInBattleGround(pcsCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsTarget) )
	{
		return (!m_pagpmCharacter->IsSameRace(pcsCharacter, pcsTarget));
	}
		
	// ���� �������� ���������� �ϴ� ��� ���� ����. ���� �������״� �ѳ� �����Ѵ�.
	BOOL bIsInSiegeWarIngArea = FALSE;
	BOOL bIsParticipantSiegeWar = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = &bIsParticipantSiegeWar;
	EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsCharacter, pvBuffer);
	if (!bIsInSiegeWarIngArea)
	{
		// 2005.10.04. steeple
		if(IsEnemyCharacter(pcsCharacter, pcsTarget))
			return TRUE;

		// �ּ� PvP �������� ������ ���� �Ұ�
		// ������, �߱��� ��� �ٸ� ������ ���� �����ϴٴ� ��..
		INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
		if (m_lMinPvPLevel > lLevel)
		{
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
			return FALSE;
		}

		
		lLevel = m_pagpmCharacter->GetLevel(pcsTarget);
		if (m_lMinPvPLevel > lLevel)
		{
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
			return FALSE;
		}
	}
	// ���� ���� ����
	else
	{
		// 2006.06.20. steeple
		// �������� �� ���ݰ����� �� üũ
		// Ŭ���̾�Ʈ���� �� �����Ͱ� cm ��⿡ �־ �ε����ϰ� �̷��� ó��.
		if(EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsCharacter, pcsTarget))
			return TRUE;
		else if(m_pagpmGuild->IsSameGuild(pcsCharacter, pcsTarget))
			return FALSE;
		else if(EnumCallback(AGPMPVP_CB_IS_FRIEND_SIEGEWAR, pcsCharacter, pcsTarget) && TRUE != bForce)
			return FALSE;
		
		// �����³��� ������ �´³��� ������.
		if (bIsParticipantSiegeWar)
			return TRUE;

		return FALSE;
	}
	
	// ���� �ƴ϶�� ���� üũ

	// �� �� �ϳ��� ���������̸� ���ݺҰ�
	// ������, �߱��� �ٸ� �����̸� ������ �� �ִٴ� ��... -_-;
	if(IsSafePvPMode(pcsCharacter) || IsSafePvPMode(pcsTarget))
		return FALSE;

	// ------------  Friend Character �� ���ݰ����� Region�̸� ���ݰ����ϰ� 
	INT nTargetResult	=	m_papmMap->CheckRegionPerculiarity( pcsTarget->m_nBindingRegionIndex	, APMMAP_PECULIARITY_GUILD_PVP );
	INT nSelfResult		=	m_papmMap->CheckRegionPerculiarity( pcsCharacter->m_nBindingRegionIndex	, APMMAP_PECULIARITY_GUILD_PVP );

	if( nTargetResult	==	APMMAP_PECULIARITY_RETURN_DISABLE_USE &&
		nSelfResult		==	APMMAP_PECULIARITY_RETURN_DISABLE_USE	)
	{
		return TRUE;
	}
	// -------------------------------------------------------------------------

	// ���� ���� �������� ���� ���̸� return FALSE
	if(IsCombatPvPMode(pcsCharacter) && IsFriendCharacter(pcsCharacter, pcsTarget))
	{
		ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_FRIEND, NULL, NULL, NULL, NULL, pcsCharacter);
		return FALSE;
	}

	// 2005.04.08. steeple
	// ������������������ �������ݾƴϸ� ���� �ƴ� ĳ���͸� ���� ����.
	// ���������������� ���� �ƴ� �� ���������� �ƴϸ� ����
	if(/*IsFreePvPMode(pcsCharacter) && */!IsEnemyCharacter(pcsCharacter, pcsTarget) && bForce != TRUE) 
		return FALSE;

	if (!IsEnemyCharacter(pcsCharacter, pcsTarget) &&
		m_pagpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_FIRST_ATTACK))
		return FALSE;

	return TRUE;
}

// 2005.08.29. steeple
// Summons �迭 ������ ��� �߰� �� üũ ��� ��ȭ.
// Skill �� �� �ִ� ������� �˻�
// pcsCharacter �� pcsTarget ���� ���� ����
BOOL AgpmPvP::IsSkillEnable(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget, AgpdCharacter* pcsSummonsOwner)
{
	//STOPWATCH2(GetModuleName(), _T("IsSkillEnable"));
	AgpdCharacter * pcsCharacter;
	AgpdSkillTemplate * pcsTmp;

	if(!pcsSkill || !pcsTarget)
		return FALSE;
	pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	pcsTmp = (AgpdSkillTemplate *)pcsSkill->m_pcsTemplate;
	if(!pcsCharacter || !pcsTmp)
		return FALSE;

	if(pcsCharacter == pcsTarget)
		return TRUE;

	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
	{
		if(m_pagpmConfig->IsEnablePvP() == FALSE)
			return FALSE;
	}

	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
	{
		// PVP�� ����ε� Volunteer��� ���ϰ� �Ѵ�.
		if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_VOLUNTEER ||
			pcsTarget->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_VOLUNTEER )
		{
			return FALSE;
		}
	}

	// 2005.08.29. steeple
	// Owner �� NULL �̰� ���� �ϳ��� Summons �̶�� ���� üũ�� ����.
	if(!pcsSummonsOwner && (m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusSummoner(pcsTarget) ||
		m_pagpmCharacter->IsStatusTame(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsTarget)))
		return IsSkillEnableSummons(pcsSkill, pcsTarget);

	// 2005.08.29. steeple
	// Owner �� �ְ�, pcsCharacter �� Owner �� Summons ��� pcsCharacter �� pcsSummonsOwner �� �ٲ㼭 üũ�Ѵ�.
	if(pcsSummonsOwner && pcsCharacter != pcsSummonsOwner &&
		(m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsCharacter)) &&
		m_pagpmSummons->GetOwnerCID(pcsCharacter) == pcsSummonsOwner->m_lID)
		pcsCharacter = pcsSummonsOwner;

	// �ٽ��ѹ� üũ
	if(!pcsCharacter)
		return FALSE;

	// 2006.09.19. steeple
	// ���Ż��� ũ�������� �˻�
	if((pcsCharacter->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsCharacter->m_pcsCharacterTemplate)) ||
		(pcsTarget->m_bIsTrasform && m_pagpmCharacter->IsCreature(pcsTarget->m_pcsCharacterTemplate)))
	{
		return FALSE;
	}

	// ��ų �����ڰ� �Ұ��� �����̴�. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusDisableSkill(pcsCharacter) &&
		IsAttackSkill(pcsTmp)) {
		if(m_pagpmCharacter->IsPC(pcsCharacter))
			ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_DISABLE_SKILL_CAST, NULL, NULL, 0, 0, pcsCharacter);

		return FALSE;
	}

	// Disable �����̸� ĳ������ �� ����. 2007.07.18. steeple
	if(m_pagpmCharacter->IsDisableCharacter(pcsCharacter) || m_pagpmCharacter->IsDisableCharacter(pcsTarget))
		return FALSE;

	// ���ݽ�ų����
	BOOL bAttackSkill = IsAttackSkill(pcsSkill);

	// ��ų ���� ���� PC �� �ƴϸ�
	if(!m_pagpmCharacter->IsPC(pcsCharacter))
	{
		// ���� �����̸� �� ����
		if(m_pagpmCharacter->IsMonster(pcsCharacter) && m_pagpmCharacter->IsMonster(pcsTarget))
			return FALSE;

		// ���ݽ�ų�̰�, �´� ���� PC �̰� �����̸� �� ����
		if(bAttackSkill && m_pagpmCharacter->IsPC(pcsTarget) && m_pagpmCharacter->IsStatusInvincible(pcsTarget))
			return FALSE;

		return TRUE;
	}

	// ��ų �´� ���� PC �� �ƴϸ� ���� üũ�� ����.
	if(!m_pagpmCharacter->IsPC(pcsTarget))
	{
		// Ÿ���� ���� ������Ʈ���� ���� ���� ������Ʈ��� ����, ������ �� ������ ���� ���� ���θ� Ȯ���Ѵ�.
		BOOL	bIsSiegeWarObject	= FALSE;
		BOOL	bCheckResult		= FALSE;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= pcsCharacter;
		pvBuffer[1]	= pcsTarget;
		pvBuffer[2]	= &bIsSiegeWarObject;
		pvBuffer[3]	= &bCheckResult;

		EnumCallback(AGPMPVP_CB_CHECK_NPC_ATTACKABLE_TARGET, pvBuffer, NULL);

		if (bIsSiegeWarObject)
		{
			return bCheckResult;
		}
		else
		{
			// Ʈ�� �߰� 2006.12.27. steeple
			if (!m_pagpmCharacter->IsAttackable(pcsTarget) || m_pagpmCharacter->IsTrap(pcsTarget))
				return FALSE;
			else
				return TRUE;
		}
	}
	else if(strlen(pcsTarget->m_szID) == 0)
	{
		// �̰�쿣 ���峪 NPC ��
		return FALSE;
	}

	// ��ų �����ڰ� �ڰ� ������ �Ұ�. 2007.06.27. steeple
	if(m_pagpmCharacter->IsStatusSleep(pcsCharacter))
		return FALSE;

	// ���̾ ����, ������ �ƴϾ�� �Ѵ�. 2005.10.04. steeple
	if(IsEnemyCharacter(pcsCharacter, pcsTarget) &&
		m_pagpmCharacter->IsStatusFullTransparent(pcsTarget) == FALSE &&
		IsInvincible(pcsTarget) == FALSE &&
		m_pagpmCharacter->IsStatusInvincible(pcsTarget) == FALSE)
		return TRUE;

	if(bAttackSkill)
	{
		// 2005.10.04. steeple
		// ���� ��ų�� ��, ������ �ֵ鿡�Դ� ������ �ȵȴ�.
		if(m_pagpmCharacter->IsStatusInvincible(pcsTarget))
		{
			if(m_pagpmCharacter->IsPC(pcsCharacter))
			{
				string	strName		=	pcsTarget->m_szID;

				if( m_pagpmBattleGround->IsInBattleGround( pcsTarget ) ||
					APMMAP_PECULIARITY_RETURN_DISABLE_USE == m_papmMap->CheckRegionPerculiarity( pcsTarget->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME ) )
				{
					strName.replace( strName.begin() , strName.end() , strName.size() , '*' );
				}

				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_TARGET_INVINCIBLE, (char*)strName.c_str(), NULL, 0, 0, pcsCharacter);
			}

			return FALSE;
		}

		// ����Ʈ ��ų �߰� 2006.09.21. steeple
		if(!m_pagpmSkill->IsDetectSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) && 
			m_pagpmCharacter->IsStatusFullTransparent(pcsTarget))
			return FALSE;

		// 2006.06.20. steeple
		// �������� �� ���ݰ����� �� üũ
		// Ŭ���̾�Ʈ���� �� �����Ͱ� cm ��⿡ �־ �ε����ϰ� �̷��� ó��.
		if(EnumCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pcsCharacter, pcsTarget))
			return TRUE;
		else if(EnumCallback(AGPMPVP_CB_IS_FRIEND_SIEGEWAR, pcsCharacter, pcsTarget))
			return FALSE;

		// Attack Skill �� ����� �� ��������, �������� ������ �ƴϸ� return FALSE
		if(!IsCombatPvPMode(pcsCharacter) && !IsFreePvPMode(pcsCharacter))
			return FALSE;

		// �� �� �ϳ��� ���������̸� ���ݺҰ�
		if(IsSafePvPMode(pcsCharacter) || IsSafePvPMode(pcsTarget))
			return FALSE;

		// Attack Skill �� ����� �� ���� ���Դ� �� �� ����.
		// �������� ���������� üũ���ָ� �ȴ�.
		// pcsCharacter�� pcsTarget�� �Ѵ� ��Ʋ�׶��� ���̸� ���� ��峢���� �ο� ����
		if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter) == FALSE && m_pagpmBattleGround->IsInBattleGround(pcsTarget) == FALSE)
		{
			if(IsFriendCharacter(pcsCharacter, pcsTarget) && IsCombatPvPMode(pcsCharacter) && IsCombatPvPMode(pcsTarget))
			{	
				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_FRIEND, NULL, NULL, NULL, NULL, pcsCharacter);
				return FALSE;
			}
		}

		// ���� ��ų�� ��
		// ��Ƽ���̰� Ÿ���� ��Ƽ���̸� ������.
		AgpdParty *pcsParty = m_pagpmParty->GetParty(pcsCharacter);
		if (NULL != pcsParty)
		{
			AuAutoLock LockParty(pcsParty->m_Mutex);
			if (LockParty.Result())
			{
				if (IsFreePvPMode(pcsCharacter) && m_pagpmParty->IsMember(pcsParty, pcsTarget->m_lID))
					return FALSE;
			}
		}

		// 2008.6.18 arycoat - battleground
		if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		{
			if(!m_pagpmBattleGround->IsAttackable(pcsCharacter, pcsTarget))
				return FALSE;
		} 
		else if(/*IsFreePvPMode(pcsCharacter) &&*/ !IsEnemyCharacter(pcsCharacter, pcsTarget) && pcsSkill->m_bForceAttack != TRUE)
			return FALSE;

		BOOL bIsInSiegeWarIngArea = FALSE;
		BOOL bIsParticipantSiegeWar = FALSE;
		PVOID pvBuffer[2];
		pvBuffer[0] = &bIsInSiegeWarIngArea;
		pvBuffer[1] = &bIsParticipantSiegeWar;
		EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsCharacter, pvBuffer);
		if (!bIsInSiegeWarIngArea)
		{
			// ������ ���̳� �´� ���̳� ���� 6�̸��� PK�Ұ�.
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			if (m_lMinPvPLevel > lLevel)
			{
				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
				return FALSE;
			}
			
			lLevel = m_pagpmCharacter->GetLevel(pcsTarget);
			if (m_lMinPvPLevel > lLevel)
			{
				ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL, NULL, NULL, m_lMinPvPLevel, 0, pcsCharacter);
				return FALSE;
			}
		}
		else
		{
			// �����ϴ°�? ���� ����...
		}

		// ������ų����
		BOOL bAreaSkill = m_pagpmSkill->IsAreaSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate);


		// 2005.04.08. steeple
		// ������������������ �������ݾƴϸ� ���� �ƴ� ĳ���͸� ���� ����.
		//
		// ������ų�̰� ���ݽ�ų�̰� �������������϶���
		if(bAreaSkill)// && bAttackSkill && IsFreePvPMode(pcsCharacter) && IsFreePvPMode(pcsTarget))
		{
			// 2006.10.24. steeple
			// ����Ʈ ��ų ���� �߰�
			if(m_pagpmSkill->IsDetectSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate))
			{
				if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
				{
					if(!m_pagpmBattleGround->IsAttackable(pcsCharacter, pcsTarget))
						return FALSE;
				}
				else
				{
					// ���������� �ƴϰ� ���� �ƴ϶��
					if(!IsEnemyCharacter(pcsCharacter, pcsTarget) && !IsCombatPvPMode(pcsCharacter) && !IsCombatPvPMode(pcsTarget))
						return FALSE;
				}
			}
			else
			{
				if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
				{
					if(!m_pagpmBattleGround->IsAttackable(pcsCharacter, pcsTarget))
						return FALSE;
				}
				else
				{
					// ��ų�� �������� Ÿ�ٰ� ���� �ʰ� ���� �ƴϸ� ����
					if(pcsSkill->m_csTargetBase.m_lID != AP_INVALID_CID &&
						pcsSkill->m_csTargetBase.m_lID != pcsTarget->m_lID &&
						!IsEnemyCharacter(pcsCharacter, pcsTarget))
						return FALSE;
				}
			}
		}

		if(!m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		{
			if (!IsEnemyCharacter(pcsCharacter, pcsTarget) &&
				m_pagpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_FIRST_ATTACK))
				return FALSE;
		}
	}

	return TRUE;
}

// Attack ��ų���� Ȯ���Ѵ�.
// Debuf ��ų�� Attack ���� ����
inline BOOL AgpmPvP::IsAttackSkill(AgpdSkill* pcsSkill)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate)
		return FALSE;

	return IsAttackSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate);
}

inline BOOL AgpmPvP::IsAttackSkill(AgpdSkillTemplate* pcsSkillTemplate)
{
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bAttackSkill = FALSE;
    if(pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_BUFF_TYPE][1] == 1)
		bAttackSkill = TRUE;
	else if(m_pagpmSkill->IsAttackSkill(pcsSkillTemplate))
		bAttackSkill = TRUE;
	else if(m_pagpmSkill->GetActionOnActionType(pcsSkillTemplate) == 1)	// Action On Action Type1 (ex. Time-Attack)
		bAttackSkill = TRUE;

	return bAttackSkill;
}

// 2005.08.29. steeple
// pcsCharacter �� pcsTarget �� �ϳ��� Summons �̸� �ϴ� �̸��� �´�.
BOOL AgpmPvP::IsAttackableSummons(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce)
{
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// �Ѵ� PC ��. ���� �ͼ� �ȵȴ�.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
		return FALSE;

	// 2005.11.09. steeple
	// Fixed �� ���� �ʴ´�.
	if(m_pagpmCharacter->IsStatusFixed(pcsTarget))
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar1 = m_pagpmSummons->GetADCharacter(pcsCharacter);
	AgpdSummonsADChar* pcsSummonsADChar2 = m_pagpmSummons->GetADCharacter(pcsTarget);
	if(!pcsSummonsADChar1 || !pcsSummonsADChar2)
		return FALSE;

	// �Ѵ� Summons �̴�.
	if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// ������ ���� �� Pass
		if(pcsSummonsADChar1->m_lOwnerCID == pcsSummonsADChar2->m_lOwnerCID)
			return FALSE;

		// �̷� �� ������ �� IsAttackable �� �ҷ�����.
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		// �׳� IsAttackable �θ��� �ɵ�
		return IsAttackable(pcsOwner1, pcsOwner2, bForce);
	}
	// ������ �� Summons �̴�.
	else if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		// ������ Ÿ���̸� Pass
		if(pcsSummonsADChar1->m_lOwnerCID == pcsTarget->m_lID)
			return FALSE;

		// ������ ���� ���θ� ����
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		return IsAttackable(pcsOwner1, pcsTarget, bForce);
	}
	// �´� �� Summons �̴�.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// �ڱ� Summons �̶�� Pass
		if(pcsCharacter->m_lID == pcsSummonsADChar2->m_lOwnerCID)
			return FALSE;

		// �´� ���� ���θ� ����.
		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		return IsAttackable(pcsCharacter, pcsOwner2, bForce);
	}
	// 2005.10.08. steeple
	// �� ��ƾ�� ���µ�, �����ְ� �Ѵ� 0 �̸� Ŭ���̾�Ʈ���� �Ҹ��Ŵ�.
	// Ŭ���̾�Ʈ�� �ٸ� ��ȯ���� ������ �𸣹Ƿ�(2005.10.08 ����. ���߿� �˰� �� ���� ����)
	// �ϴ� return TRUE �� ���ְ�, �������� üũ�ϰ� ���ش�.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		return TRUE;
	}

	return FALSE;
}

// 2005.08.29. steeple
// pcsCharacter �� pcsTarget �� �ϳ��� Summons �̸� �ϴ� �̸��� �´�.
BOOL AgpmPvP::IsSkillEnableSummons(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget)
{
	if(!pcsSkill || !pcsSkill->m_pcsBase)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsSkill->m_pcsBase;
	if(!pcsCharacter || !pcsTarget)
		return FALSE;

	// �Ѵ� PC ��. ���� �ͼ� �ȵȴ�.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && m_pagpmCharacter->IsPC(pcsTarget))
		return FALSE;

	// 2005.11.09. steeple
	// Fixed �� ���� �ʴ´�.
	if(m_pagpmCharacter->IsStatusFixed(pcsTarget))
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar1 = m_pagpmSummons->GetADCharacter(pcsCharacter);
	AgpdSummonsADChar* pcsSummonsADChar2 = m_pagpmSummons->GetADCharacter(pcsTarget);
	if(!pcsSummonsADChar1 || !pcsSummonsADChar2)
		return FALSE;

	// �Ѵ� Summons �̴�.
	if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// ������ ���� ��
		if(pcsSummonsADChar1->m_lOwnerCID == pcsSummonsADChar2->m_lOwnerCID)
		{
			if(IsAttackSkill(pcsSkill))		// ���ݽ�ų�̸� Pass
				return FALSE;
			else							// �ƴϸ� ��밡��
				return TRUE;
		}

		// �� ��ų�� ������ Summons �̴� ����, �������� 
		// �̷� �� ������ �� IsSkillEnable �� �ҷ�����.
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		// �׳� IsSkillEnable �θ��� �ɵ�
		return IsSkillEnable(pcsSkill, pcsOwner2, pcsOwner1);
	}
	// ������ �� Summons �̴�.
	else if(pcsSummonsADChar1->m_lOwnerCID != 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		// ������ Ÿ���̸�
		if(pcsSummonsADChar1->m_lOwnerCID == pcsTarget->m_lID)
		{
			if(IsAttackSkill(pcsSkill))		// ���ݽ�ų�̸� Pass
				return FALSE;
			else							// �ƴϸ� ��밡��
				return TRUE;
		}

		// ������ ���� ���θ� ����
		ApAutoLockCharacter LockOwner1(m_pagpmCharacter, pcsSummonsADChar1->m_lOwnerCID);
		AgpdCharacter* pcsOwner1 = LockOwner1.GetCharacterLock();
		if(!pcsOwner1)
			return FALSE;

		return IsSkillEnable(pcsSkill, pcsTarget, pcsOwner1);
	}
	// �´� �� Summons �̴�.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID != 0)
	{
		// �ڱ� Summons �̶��
		if(pcsCharacter->m_lID == pcsSummonsADChar2->m_lOwnerCID)
		{
			if(IsAttackSkill(pcsSkill))		// ���ݽ�ų�̸� Pass
				return FALSE;
			else							// �ƴϸ� ��밡��
				return TRUE;
		}

		// �´� ���� ���θ� ����.
		ApAutoLockCharacter LockOwner2(m_pagpmCharacter, pcsSummonsADChar2->m_lOwnerCID);
		AgpdCharacter* pcsOwner2 = LockOwner2.GetCharacterLock();
		if(!pcsOwner2)
			return FALSE;

		return IsSkillEnable(pcsSkill, pcsOwner2, pcsCharacter);	// Owner ���ڿ� �׳� Summons �� �Ѱܹ�����.
	}
	// 2005.10.08. steeple
	// �� ��ƾ�� ���µ�, �����ְ� �Ѵ� 0 �̸� Ŭ���̾�Ʈ���� �Ҹ��Ŵ�.
	// Ŭ���̾�Ʈ�� �ٸ� ��ȯ���� ������ �𸣹Ƿ�(2005.10.08 ����. ���߿� �˰� �� ���� ����)
	// �ϴ� return TRUE �� ���ְ�, �������� üũ�ϰ� ���ش�.
	else if(pcsSummonsADChar1->m_lOwnerCID == 0 && pcsSummonsADChar2->m_lOwnerCID == 0)
	{
		return TRUE;
	}

	return FALSE;
}

// System Message �� �޾Ƽ� ó�����ش�.
BOOL AgpmPvP::ProcessSystemMessage(INT32 lCode, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2, AgpdCharacter* pcsCharacter)
{
	AgpdPvPSystemMessage stSystemMessage;
	memset(&stSystemMessage, 0, sizeof(AgpdPvPSystemMessage));

	stSystemMessage.m_lCode = lCode;
	stSystemMessage.m_aszData[0] = szData1;
	stSystemMessage.m_aszData[1] = szData2;
	stSystemMessage.m_alData[0] = lData1;
    stSystemMessage.m_alData[1] = lData2;
    
	EnumCallback(AGPMPVP_CB_SYSTEM_MESSAGE, &stSystemMessage, pcsCharacter);
	return TRUE;
}









//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgpmPvP::CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;

	return pThis->InitCharacter(pcsCharacter);
}

BOOL AgpmPvP::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	return pThis->RemoveCharacter(pcsCharacter);
}

BOOL AgpmPvP::CBIsAttackable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;
	
	AgpdCharacter* pcsTarget = (AgpdCharacter*)ppvBuffer[0];
	BOOL bForce = PtrToInt(ppvBuffer[1]);

	return  pThis->IsAttackable(pcsCharacter, pcsTarget, bForce);
}

BOOL AgpmPvP::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;
	INT16 nPrevRegionIndex = pCustData ? *(INT16*)pCustData : -1;	// NULL �� �ö��� ADD_CHARACTER_TO_MAP ���� �Ҹ�����.

    return pThis->RegionChange(pcsCharacter, nPrevRegionIndex);
}

BOOL AgpmPvP::CBIsSkillEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSkill* pcsSkill = (AgpdSkill*)pData;
	AgpmPvP* pThis = (AgpmPvP*)pClass;
	ApBase* pcsTarget = (ApBase*)pCustData;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return TRUE;

	return pThis->IsSkillEnable(pcsSkill, (AgpdCharacter*)pcsTarget);
}







//////////////////////////////////////////////////////////////////////////
// Callback Registration
BOOL AgpmPvP::SetCallbackPvPInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_PVP_INFO, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackPvPResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_PVP_RESULT, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_FRIEND, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_ENEMY, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackAddEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ADD_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRemoveEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REMOVE_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackInitEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_INIT_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_FRIEND_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackUpdateEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_UPDATE_ENEMY_GUILD, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRecvCannotUseTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_RECV_CANNOT_USE_TELEPORT, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackEnterSafePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ENTER_SAFE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackEnterFreePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ENTER_FREE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackEnterCombatPvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_ENTER_COMBAT_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackLeaveSafePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_LEAVE_SAFE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackLeaveFreePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_LEAVE_FREE_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackLeaveCombatPvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_LEAVE_COMBAT_PVP_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_SYSTEM_MESSAGE, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRequestDeadType(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_REQUEST_DEAD_TYPE, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackResponseDeadType(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_RESPONSE_DEAD_TYPE, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsFriendSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_FRIEND_SIEGEWAR, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsEnemySiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_ENEMY_SIEGEWAR, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsInSiegeWarIngArea(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackCheckNPCAttackableTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_CHECK_NPC_ATTACKABLE_TARGET, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackIsAttackableTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_IS_ATTACKABLE_TIME, pfCallback, pClass);
}

BOOL AgpmPvP::SetCallbackRaceBattleStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPVP_CB_RACE_BATTLE_STATUS, pfCallback, pClass);
}






//////////////////////////////////////////////////////////////////////////
// AgpmPvPArray 
AgpmPvPArray::AgpmPvPArray()
{
	lObjectCount	= 0;
}

AgpmPvPArray::~AgpmPvPArray()
{
}

BOOL	AgpmPvPArray::Initialize()
{
	m_csArrayID.MemSetAll();
	m_csArrayData.MemSetAll();

	ZeroMemory(m_csArrayName, sizeof(m_csArrayName));

	lObjectCount	= 0;

	return TRUE;
}

PVOID	AgpmPvPArray::AddObject(PVOID pObject, INT32 nKey)
{
	if (!pObject || nKey == 0)
		return NULL;

	if (lObjectCount == AGPMPVP_MAX_FRIEND_COUNT)
		return NULL;

	int i = 0;

	// ���� �̹� �ִ� ������ ����.
	for (i = 0; i < lObjectCount; ++i)
	{
		if (m_csArrayID[i] == nKey)
			return NULL;
	}

	m_csArrayID[i]		= nKey;
	m_csArrayData[i]	= pObject;

	++lObjectCount;

	return pObject;
}

PVOID	AgpmPvPArray::AddObject(PVOID pObject, CHAR *szName)
{
	if (!pObject || !szName || !szName[0])
		return NULL;

	if (lObjectCount == AGPMPVP_MAX_FRIEND_COUNT)
		return NULL;

	int i = 0;

	// ���� �̹� �ִ� ������ ����.
	for (i = 0; i < lObjectCount; ++i)
	{
		if (strncmp(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH) == 0)
			return NULL;
	}

	strncpy(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	m_csArrayData[i]	= pObject;

	++lObjectCount;

	return pObject;
}

BOOL	AgpmPvPArray::RemoveObject(INT32 nKey)
{
	if (nKey == 0)
		return FALSE;

	int i = 0;
	for (i = 0; i < lObjectCount; ++i)
	{
		if (m_csArrayID[i] == nKey)
			break;
	}

	if (i == lObjectCount)
		return FALSE;

	m_csArrayID.MemCopy(i, &m_csArrayID[i + 1], lObjectCount - i - 1);
	m_csArrayData.MemCopy(i, &m_csArrayData[i + 1], lObjectCount - i - 1);

	--lObjectCount;

	m_csArrayID[lObjectCount]		= 0;
	m_csArrayData[lObjectCount]		= NULL;

	return TRUE;
}

BOOL	AgpmPvPArray::RemoveObject(CHAR *szName)
{
	if (!szName || !szName[0])
		return FALSE;

	int i = 0;
	for (i = 0; i < lObjectCount; ++i)
	{
		if (strncmp(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH) == 0)
			break;
	}

	if (i == lObjectCount)
		return FALSE;

	CopyMemory(m_csArrayName[i], m_csArrayName[i + 1], (sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1)) * (lObjectCount - i - 1));
	m_csArrayData.MemCopy(i, &m_csArrayData[i + 1], lObjectCount - i - 1);

	--lObjectCount;

	ZeroMemory(m_csArrayName[lObjectCount], sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1));
	m_csArrayData[lObjectCount]		= NULL;

	return TRUE;
}

PVOID	AgpmPvPArray::GetObject(INT32 nKey)
{
	for (int i = 0; i < lObjectCount; ++i)
	{
		if (m_csArrayID[i] == nKey)
			return m_csArrayData[i];
	}

	return NULL;
}

PVOID	AgpmPvPArray::GetObject(CHAR *szName)
{
	if (!szName || !szName[0])
		return FALSE;

	for (int i = 0; i < lObjectCount; ++i)
	{
		if (strncmp(m_csArrayName[i], szName, AGPMGUILD_MAX_GUILD_ID_LENGTH) == 0)
			return m_csArrayData[i];
	}

	return NULL;
}

PVOID	AgpmPvPArray::GetObjectSequence(INT32* plIndex)
{
	if (!plIndex)
		return NULL;

	PVOID	pvData	= m_csArrayData[*plIndex];

	++(*plIndex);

	return pvData;
}

INT32	AgpmPvPArray::GetObjectCount()
{
	return lObjectCount;
}
