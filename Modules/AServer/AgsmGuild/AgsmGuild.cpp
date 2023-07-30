// AgsmGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.


#include "AgsmGuild.h"
#include "ApAutoLockCharacter.h"

#include "AgsmSiegeWar.h"
#include "AgpmArchlord.h"
#include "AuTimeStamp.h"
#include "AgpmBattleGround.h"
#include "AgppGuild.h"
#include "AgspGuild.h"
#include "AgsmSystemMessage.h"
#include "ApmMap.h"

#define AGSMGUILD_MAX_MEMBER_COUNT							50			// ���� ����. AgpmGuild �� �ִ� ���� Hash �� ũ�⼼���̴�.
#define AGSMGUILD_IDLE_BATTLE_PROCESS_INTERVAL				1000		// ms. 1��
#define AGSMGUILD_IDLE_MEMBER_STATUS_REFRESH				1000 * 30	// ms. 30��. 2005.06.16.steeple

//////////////////////////////////////////////////////////////////////////

AgsmGuild::AgsmGuild()
{
	m_hEventGuildLoadComplete = NULL;

	SetModuleName("AgsmGuild");
	SetModuleType(APMODULE_TYPE_SERVER);

	SetModuleData(sizeof(AgsdGuildMember));
	SetModuleData(sizeof(AgsdGuildBattleProcess), AGSMGUILD_DATA_TYPE_BATTLE_PROCESS);

	EnableIdle2(TRUE);

	m_pagpmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmGuild = NULL;
	m_pagpmPvP = NULL;
	m_pagpmLog = NULL;
	
	m_pagsmServerManager = NULL;
	m_pagsmInterServerLink = NULL;
	m_pagsmAOIFilter = NULL;
	m_pagsmCharacter = NULL;
	m_pagsmChatting = NULL;
	m_pagsmCharManager = NULL;
	m_pagsmLogin = NULL;
	m_pagsmItem = NULL;
	m_pagpmSiegeWar = NULL;
	m_pagpmArchlord = NULL;
	m_pagpmBattleGround		= NULL;
	m_pagsmSystemMessage	= NULL;
	m_papmMap				= NULL;

	m_nIndexADMember = 0;

	// Packet Define
	SetPacketType(AGSMGUILD_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_MEMORY_BLOCK,	1,	// class AgpdGuild
							AUTYPE_MEMORY_BLOCK,	1,	// class AgpdGuildMember
							AUTYPE_MEMORY_BLOCK,	1,	// Notice
							AUTYPE_END,				0
							);

	m_ulBattleReadyTime = AGPMGUILD_BATTLE_READY_TIME;
	m_ulBattleCancelEnableTime = AGPMGUILD_BATTLE_CANCEL_ENABLE_TIME;
	m_lBattleNeedMemberCount = AGPMGUILD_BATTLE_NEED_MEMBER_COUNT;
	m_lBattleNeedLevelSum = AGPMGUILD_BATTLE_NEED_LEVEL_SUM;

	m_ulLastProcessBattleClock = 0;
	m_ulLastProcessLeaveGuildClock = 0;
}

AgsmGuild::~AgsmGuild()
{
	m_ApGuildLeaveList.DeleteAllMemory();
}

BOOL AgsmGuild::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pagpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pagpmItem = (AgpmItem *)GetModule("AgpmItem");
	m_pagpmGuild = (AgpmGuild*)GetModule("AgpmGuild");
	m_pagpmPvP = (AgpmPvP*)GetModule("AgpmPvP");
	m_pagpmLog = (AgpmLog*)GetModule("AgpmLog");

	m_pagsmServerManager = (AgsmServerManager*)GetModule("AgsmServerManager2");
	m_pagsmInterServerLink = (AgsmInterServerLink*)GetModule("AgsmInterServerLink");
	m_pagsmAOIFilter = (AgsmAOIFilter*)GetModule("AgsmAOIFilter");
	m_pagsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagsmChatting = (AgsmChatting*)GetModule("AgsmChatting");
	m_pagsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pagsmLogin = (AgsmLogin*)GetModule("AgsmLogin");
	m_pagsmItem = (AgsmItem*)GetModule("AgsmItem");
	m_pagpmSiegeWar = (AgpmSiegeWar*)GetModule("AgpmSiegeWar");
	//m_pagsmSiegeWar = (AgsmSiegeWar*)GetModule("AgsmSiegeWar");
	m_pagpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");
	m_papmMap		= (ApmMap*)GetModule("ApmMap");

	if(!m_pagpmCharacter || !m_pagpmFactors || !m_pagpmGuild || !m_pagpmPvP ||
		!m_pagsmServerManager || !m_pagsmInterServerLink ||
		!m_pagsmCharacter || !m_pagsmChatting || !m_pagsmCharManager || !m_pagpmSiegeWar || !m_pagpmArchlord || !m_papmMap)
		return FALSE;

	m_nIndexADMember = m_pagpmGuild->AttachMemberData(this, sizeof(AgsdGuildMember), ConAgsdGuildMember, DesAgsdGuildMember);
	if(m_nIndexADMember < 0)
		return FALSE;

	// AgpmGuild
	if(!m_pagpmGuild->SetCallbackGuildCreateCheck(CBCreateGuildCheck, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildCreateEnableCheck(CBCreateGuildCheckEnable, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildCreate(CBCreateGuildSuccess, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildCreateFail(CBCreateGuildFail, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinRequest(CBJoinRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinFail(CBJoinFail, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinEnableCheck(CBJoinCheckEnable, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoinReject(CBJoinReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildJoin(CBJoin, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildLeave(CBLeave, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildForcedLeave(CBForcedLeave, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildDestroy(CBDestroyGuild, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackCheckPassword(CBCheckPassword, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackUpdateNotice(CBUpdateNotice, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackSystemMessage(CBSystemMessage, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallbackBattlePerson(CBBattlePerson, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleRequest(CBBattleRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleAccept(CBBattleAccept, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleReject(CBBattleReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleCancelRequest(CBBattleCancelRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleCancelAccept(CBBattleCancelAccept, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleCancelReject(CBBattleCancelReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackBattleWithdraw(CBBattleWithdraw, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackGuildUpdateMaxMemberCount(CBGuildUpdateMaxMemberCount, this))
		return FALSE;
//	2005.07.28. By SungHoon
	if(!m_pagpmGuild->SetCallbackReceiveLeaveRequestSelf(CBReceiveLeaveRequestSelf, this))
		return FALSE;
	
	// AgpmCharacter
	if(!m_pagpmCharacter->SetCallbackUpdateLevel(CBCharacterLevelUp, this))
		return FALSE;
	if(!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))		// 2005.11.09. steeple �߰�~~
		return FALSE;

	if(!m_pagsmInterServerLink->SetCallbackConnect(CBServerConnect, this))
		return FALSE;
	//if(!m_pagsmInterServerLink->SetCallbackSetFlag(CBServerSetFlag, this))
	//	return FALSE;

	if(!m_pagsmCharacter->SetCallbackSendCharacterNewID(CBSendCharacterNewID, this))	// login server to game server
		return FALSE;	
	if(!m_pagsmCharacter->SetCallbackIsGuildMaster(CBIsGuildMaster, this))	// 2005.06.01 By SungHoon
		return FALSE;	

	if(!m_pagsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;
	
	// 2005.11.09. steeple. �������� �ݹ��� AgpmCharacter::RemoveCharacter �� �ٲ����. 
	//if(!m_pagsmCharManager->SetCallbackDisconnectCharacter(CBDisconnect, this))
	//	return FALSE;

	if(!m_pagsmCharManager->SetCallbackDeleteComplete(CBDeleteComplete, this))
		return FALSE;
	
	if(!m_pagsmChatting->SetCallbackGuildMemberInvite(CBGuildMemberInviteByCommand, this))
		return FALSE;
	if(!m_pagsmChatting->SetCallbackGuildLeave(CBGuildLeaveByCommand, this))
		return FALSE;
	if(!m_pagsmChatting->SetCallbackGuildJointMessage(CBGuildJointMessage, this))
		return FALSE;

	if(!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveCell, this))
		return FALSE;

	//if(!m_pagsmCharacter->SetCallbackSendCharView(CBSendCharView, this))
	//	return FALSE;
//	2005.07.08. By SungHoon
	if(!m_pagpmGuild->SetCallbackReceiveGuildList(CBReceiveGuildListInfo, this))
		return FALSE;
//	2005.08.02. By SungHoon
	if(!m_pagpmGuild->SetCallbackReceiveJoinRequestSelf(CBReceiveJoinRequestSelf, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackDestroyGuildCheckTime(CBDestroyGuildCheckTime, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackRenameGuildID(CBGuildRenameGuildID,this))
		return FALSE;

//	2005.09.05. By SungHoon
	if (m_pagsmLogin)
	{
		if(!m_pagsmLogin->SetCallbackCharacterRenamed(CBGuildRenameCharacterID,this))
			return FALSE;
	}

	if(!m_pagpmGuild->SetCallbackBuyGuildMark(CBGuildBuyGuildMark,this))
		return FALSE;

	if(!m_pagpmGuild->SetCallbackBuyGuildMarkForce(CBGuildBuyGuildMarkForce,this))
		return FALSE;
	
	if(!m_pagpmGuild->SetCallbackJointRequest(CBJointRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackJointReject(CBJointReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackJoint(CBJoint, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackJointLeave(CBJointLeave, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileRequest(CBHostileRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileReject(CBHostileReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostile(CBHostile, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileLeaveRequest(CBHostileLeaveRequest, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileLeaveReject(CBHostileLeaveReject, this))
		return FALSE;
	if(!m_pagpmGuild->SetCallbackHostileLeave(CBHostileLeave, this))
		return FALSE;

	if (!m_pagsmItem->SetCallbackPickupCheckItem(CBItemPickupCheck, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_REQUEST, OnOperationWorldChampionshipRequest, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_WORLD_CHAMPIONSHIP_ENTER, OnOperationWorldChampionshipEnter, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_REQUEST, OnOperationCSAppointmentRequest, this))
		return FALSE;
	
	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_APPOINTMENT_ANSWER, OnOperationCSAppointmentAnswer, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_REQUEST, OnOperationCSSuccessionRequest, this))
		return FALSE;

	if(!m_pagpmGuild->SetCallback(AGPMGUILD_CB_CLASSSOCIETY_SUCCESSION_ANSWER, OnOperationCSSuccessionAnswer, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmGuild::OnInit()
{
	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	if(!m_pagpmBattleGround)
		return FALSE;

	m_pagsmSystemMessage	= (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	if(!m_pagsmSystemMessage)
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackSendCharacterAllInfo(CBSendCharacterAllInfo, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmGuild::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmGuild::OnIdle2(UINT32 ulClockCount)
{
	//STOPWATCH2(GetModuleName(), _T("OnIdle2"));

	ProcessAllBattle(ulClockCount);

	//ProcessRefreshAllGuildMemberStatus(ulClockCount);

	CheckLeaveGuildList(ulClockCount);

	return TRUE;
}

BOOL AgsmGuild::OnDestroy()
{
	// ��Ʋ ����Ʈ�� ����.
	BattleListLock();

	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); ++Itr )
		RemoveBattleProcess( *Itr );

	m_listBattleProcess.clear();

	BattleListUnlock();

	return TRUE;
}

BOOL AgsmGuild::SetEventGuildLoadComplete(HANDLE hEvent)
{
	m_hEventGuildLoadComplete = hEvent;
	return TRUE;
}

AgsdGuildMember* AgsmGuild::GetADMember(AgpdGuildMember* pData)
{
	if(m_pagpmGuild)
		return (AgsdGuildMember*)m_pagpmGuild->GetAttachedModuleData(m_nIndexADMember, (PVOID)pData);

	return NULL;
}

BOOL AgsmGuild::ConAgsdGuildMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgsdGuildMember* pAttachedData = pThis->GetADMember((AgpdGuildMember*)pData);

	if(!pAttachedData)
		return FALSE;

	pAttachedData->m_lServerID = 0;
	pAttachedData->m_ulNID = 0;

	pAttachedData->m_pcsCharacter = NULL;

	return TRUE;
}

BOOL AgsmGuild::DesAgsdGuildMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}




//////////////////////////////////////////////////////////////////////////
// Operation

// 2005.03.18. steeple
// ������ ��ũ�� ���߱� ���ؼ� AgsmGuild �� ��Ŷ�� �ְ� �޴´�.
BOOL AgsmGuild::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgsmGuild::OnReceive");
	
	if(!pvPacket || nSize < 1)
		return FALSE;

	INT8 cOperation = -1;
	PVOID pvGuild = NULL;
	PVOID pvMember = NULL;
	PVOID pvNotice = NULL;

	INT16 nGuildLength = -1;
	INT16 nMemberLength = -1;
	INT16 nNoticeLength = -1;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&pvGuild, &nGuildLength,
						&pvMember, &nMemberLength,
						&pvNotice, &nNoticeLength
						);

	switch(cOperation)
	{
		case AGSMGUILD_PACKET_SYNC_CREATE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			OnReceiveSyncCreate(pcsGuild);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_JOIN:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			AgpdGuildMember* pcsMember = reinterpret_cast<AgpdGuildMember*>(pvMember);
			OnReceiveSyncJoin(pcsGuild, pcsMember);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_LEAVE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			AgpdGuildMember* pcsMember = reinterpret_cast<AgpdGuildMember*>(pvMember);
			OnReceiveSyncLeave(pcsGuild, pcsMember);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_FORCED_LEAVE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			AgpdGuildMember* pcsMember = reinterpret_cast<AgpdGuildMember*>(pvMember);
			OnReceiveSyncForcedLeave(pcsGuild, pcsMember);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_DESTROY:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			OnReceiveSyncDestroy(pcsGuild);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_UPDATE_MAX_MEMBER_COUNT:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			OnReceiveSyncUpdateMaxMemberCount(pcsGuild);
			break;
		}

		case AGSMGUILD_PACKET_SYNC_UPDATE_NOTICE:
		{
			AgpdGuild* pcsGuild = reinterpret_cast<AgpdGuild*>(pvGuild);
			CHAR* szNotice = reinterpret_cast<CHAR*>(pvNotice);
			OnReceiveSyncUpdateNotice(pcsGuild, szNotice, nNoticeLength);
			break;
		}
	}

	return TRUE;
}

// 2005.03.18. steeple
// �ٸ� ���� �������� ������ ��带 �޴´�.
BOOL AgsmGuild::OnReceiveSyncCreate(AgpdGuild* pcsGuild)
{
	if( !pcsGuild )			return FALSE;

	AgpdGuild* pcsNewGuild = m_pagpmGuild->CreateGuild(pcsGuild->m_szID, pcsGuild->m_lTID, pcsGuild->m_lRank, pcsGuild->m_lMaxMemberCount, pcsGuild->m_lUnionID);
	if( !pcsNewGuild )		return FALSE;
		
	ZeroMemory( pcsNewGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1) );
	strncpy( pcsNewGuild->m_szMasterID, pcsGuild->m_szMasterID, AGPACHARACTER_MAX_ID_STRING );

	// CreateionDate
	if(pcsGuild->m_lCreationDate != -1)		// ��� ���� ��¥�� �Ѿ�Դٸ� ���� (���� Ŭ���̾�Ʈ)
		pcsNewGuild->m_lCreationDate = pcsGuild->m_lCreationDate;
	else								// ��� ���� ��¥�� �Ѿ���� �ʾҴٸ� ���� �ð����� ���� (���� ����)
		pcsNewGuild->m_lCreationDate = m_pagpmGuild->GetCurrentTimeStamp();

	// Password
	ZeroMemory( pcsNewGuild->m_szPassword, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH + 1) );
	strncpy( pcsNewGuild->m_szPassword, pcsGuild->m_szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH );

	pcsNewGuild->m_szNotice = NULL;

	// Lock �� Ǯ��
	pcsNewGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.03.18. steeple
// �ٸ� ���� �������� ������ ����� �޴´�.
BOOL AgsmGuild::OnReceiveSyncJoin( AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember )
{
	if( !pcsGuild || !pcsMember )		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock( pcsGuild->m_szID );
	if( !pcsGuildIn )					return FALSE;
		
	// Join �� �����ϸ�
	if( m_pagpmGuild->JoinMember( pcsGuildIn, pcsMember->m_szID, pcsMember->m_lRank, pcsMember->m_lJoinDate, pcsMember->m_lLevel,
								pcsMember->m_lTID, pcsMember->m_cStatus))
	{
		AgpdGuildMember* pcsNewMember = m_pagpmGuild->GetMember(pcsGuildIn, pcsMember->m_szID);
		if(pcsNewMember)
		{
			// �ٸ� ������� ������ �˸���.
			SendNewMemberJoinToOtherMembers(pcsGuildIn, pcsNewMember);
		}
	}

	pcsGuildIn->m_Mutex.Release();

	return TRUE;
}

// 2005.03.18. steeple
// �ٸ� ���� �������� ��忡�� Ż������ �޴´�.
BOOL AgsmGuild::OnReceiveSyncLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	if(!m_pagpmGuild->LeaveMember(pcsGuild->m_szID, pcsMember->m_szID, FALSE))
		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if(!pcsGuildIn)
		return FALSE;

	SendLeaveAllowToOtherMembers(pcsGuildIn, pcsMember->m_szID);

	pcsGuildIn->m_Mutex.Release();

	LeaveChar(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsMember->m_szID);

	return TRUE;
}

// 2005.03.18. steeple
// �ٸ� ���� �������� ��忡�� ©���� �޴´�.
BOOL AgsmGuild::OnReceiveSyncForcedLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	if(!m_pagpmGuild->ForcedLeaveMember(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsMember->m_szID))
		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if(!pcsGuildIn)
		return FALSE;

	SendForcedLeaveToOtherMembers(pcsGuildIn, pcsMember->m_szID);

	pcsGuildIn->m_Mutex.Release();

	LeaveChar(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsMember->m_szID);

	return TRUE;
}

// 2005.03.18. steeple
// �ٸ� ���� �������� ��� ��ü�� �޴´�.
BOOL AgsmGuild::OnReceiveSyncDestroy(AgpdGuild* pcsGuild)
{
	if( !pcsGuild )		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if( !pcsGuildIn )	return FALSE;

	// Member �̸����� �̸� ����س��´�.
	INT32 i = 0;
	CHAR** pszMemberID = NULL;
	INT32 lMemberCount = pcsGuildIn->m_pMemberList->GetObjectCount();
	if(lMemberCount > 0)
	{
		pszMemberID = new CHAR*[lMemberCount];
		for(i = 0; i < lMemberCount; i++)
		{
			pszMemberID[i] = new CHAR[AGPACHARACTER_MAX_ID_STRING+1];
			memset(pszMemberID[i], 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));
		}
	}

	INT32 lIndex = 0; i = 0;
	for(AgpdGuildMember** ppcsMember = (AgpdGuildMember**)pcsGuildIn->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
				ppcsMember = (AgpdGuildMember**)pcsGuildIn->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(*ppcsMember)
			strncpy(pszMemberID[i], (*ppcsMember)->m_szID, AGPACHARACTER_MAX_ID_STRING);

		i++;
	}

	// Destroy �ҷ��ְ�~
	if(m_pagpmGuild->DestroyGuild(pcsGuildIn))
	{
		DestroyGuild(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsGuild->m_lMaxMemberCount, pszMemberID, FALSE, FALSE);
	}

	// ����� ���� Member �̸� ����
	if(pszMemberID && lMemberCount)
	{
		for(i = 0; i < lMemberCount; i++)
		{
			delete [] pszMemberID[i];
		}

		delete [] pszMemberID;
	}

	return TRUE;
}

// 2005.03.18. steeple
// �ٸ� ���� �������� ��� �ƽ� ��� ī��Ʈ�� �޾Ҵ�.
BOOL AgsmGuild::OnReceiveSyncUpdateMaxMemberCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if(!pcsGuildIn)
		return FALSE;

	UpdateMaxMemberCount(pcsGuildIn, pcsGuild->m_lMaxMemberCount, FALSE);

    pcsGuildIn->m_Mutex.Release();

	return TRUE;
}

// 2005.03.18. steeple
// �ٸ� ���� �������� ��� ���������� �޴´�.
BOOL AgsmGuild::OnReceiveSyncUpdateNotice(AgpdGuild* pcsGuild, CHAR* szNotice, INT16 nNoticeLength)
{
	if( !pcsGuild )		return FALSE;

	AgpdGuild* pcsGuildIn = m_pagpmGuild->GetGuildLock(pcsGuild->m_szID);
	if( !pcsGuildIn )	return FALSE;

	// ���� ���� �ִٸ� Clear
	if( pcsGuildIn->m_szNotice )
	{
		delete [] pcsGuildIn->m_szNotice;
		pcsGuildIn->m_szNotice = NULL;
	}

	// �������ش�.
	if( szNotice && nNoticeLength > 0 && nNoticeLength <= AGPMGUILD_MAX_NOTICE_LENGTH )
	{
		pcsGuildIn->m_szNotice = new CHAR[nNoticeLength+1];
		ZeroMemory(pcsGuildIn->m_szNotice, sizeof(CHAR) * (nNoticeLength + 1));
		memcpy( pcsGuildIn->m_szNotice, szNotice, nNoticeLength );
	}

	SendNoticeToAllMember( pcsGuildIn );

	pcsGuildIn->m_Mutex.Release();

	return TRUE;
}

// 2005.01.24. steeple
// Guild ���� �� �̸��� ��� �������� DB ���� üũ�Ѵ�.
BOOL AgsmGuild::CreateGuildCheck(AgpdGuild* pcsGuild, CHAR* szMasterID)
{
	if(!pcsGuild || !szMasterID)
		return FALSE;

	// �� Callback �θ��� �ȴ�.
	EnumCallback(AGSMGUILD_CB_DB_MASTER_CHECK, pcsGuild, szMasterID);

	return TRUE;
}

// Guild ���� �� ������ �� üũ�Ѵ�.
BOOL AgsmGuild::CreateGuildCheckEnable(CHAR* szGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szMasterID)
		return FALSE;

	INT8 cErrorCode = -1;
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT32 lData1 = 0;
	INT32 lData2 = 0;
	
	INT32 lTaxRatio = 0;

	BOOL bAlreadyMember = FALSE;

	// Character �� ������ ��´�.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
	if(!pcsCharacter)
		return FALSE;

	lCID = pcsCharacter->m_lID;
	ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	INT32 lLevel = 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	INT64 llMoney = 0;
	m_pagpmCharacter->GetMoney(pcsCharacter, &llMoney);

	AgpdGuildADChar* pcsADCharGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsADCharGuild)
	{
		if(strlen(pcsADCharGuild->m_szGuildID) != 0 || strlen(pcsADCharGuild->m_szRequestJoinGuildID) != 0)
			bAlreadyMember = TRUE;
	}

	// �������ϱ����� ������. -_-;
	lTaxRatio = m_pagpmCharacter->GetTaxRatio(pcsCharacter);

	pcsCharacter->m_Mutex.Release();

	// �̹� �ٸ� ����� ������ Error
	if(bAlreadyMember)
		cErrorCode = AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER;

	// Guild Name Ư������ üũ
	if(cErrorCode == -1)
	{
		if(!CheckGuildID(szGuildID))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_USE_SPECIAL_CHAR;
	}

	// Guild Name �ߺ� üũ
	if(cErrorCode == -1)
	{
		std::string szTempName = szGuildID;
		AgpdGuild* pcsGuild = NULL;
		INT32 Index = 0;

		for(pcsGuild = m_pagpmGuild->GetGuildSequence(&Index); pcsGuild; pcsGuild = m_pagpmGuild->GetGuildSequence(&Index))
		{
			std::string szID = pcsGuild->m_szID;
			if( szTempName == szID )
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID;
				break;
			}
		}
	}

	// ��� �̸��� ���� �̸��� ������ ��� �� �� ����.
	if (cErrorCode == -1)
	{
		for (int i = 0; i < AGPMSIEGEWAR_MAX_CASTLE; ++i)
		{
			if (COMPARE_EQUAL == m_pagpmSiegeWar->m_csSiegeWarInfo[i].m_strCastleName.CompareNoCase(szGuildID))
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID;
				break;
			}
		}
	}

	// ���� üũ..

	// Level Check
	if(cErrorCode == -1)
	{
		if(lLevel < AGPMGUILD_CREATE_REQUIRE_LEVEL)
			cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_MORE_LEVEL;
	}

	/* ���� �������� MaxMember���� �����ͷ� �˻��Ѵ�.
	// �� �˻�
	if(cErrorCode == -1)
	{
		if(llMoney < AGPMGUILD_CREATE_REQUIRE_MONEY)
			cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_MORE_MONEY;
	}
	*/

	AgpdRequireItemIncreaseMaxMember *pcsRequirement = m_pagpmGuild->GetRequireIncreaseMaxMember(AGSMGUILD_MAX_MEMBER_COUNT);
	if (cErrorCode == -1 && pcsRequirement)
	{
		// ��� ��å ���� - arycoat 2008.02
		/*if (pcsRequirement->m_lSkullTID)
		{
			AgpdItem *pcsItem = m_pagpmItem->GetInventoryItemByTID(pcsCharacter, pcsRequirement->m_lSkullTID);
			if(!pcsItem)
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_ITEM;
				lData1 = pcsRequirement->m_lSkullTID;
				lData2 = pcsRequirement->m_lSkullCount;
			}

			if (cErrorCode == -1 && pcsItem->m_nCount < pcsRequirement->m_lSkullCount)
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_ITEM;
				lData1 = pcsRequirement->m_lSkullTID;
				lData2 = pcsRequirement->m_lSkullCount;
			}
		}*/

		INT32 lGheld = pcsRequirement->m_lGheld;
		INT32 lTax = 0;
		if (lTaxRatio > 0)
		{
			lTax = (lGheld * lTaxRatio) / 100;
		}
		lGheld = lGheld + lTax;	

		if (cErrorCode == -1 && pcsCharacter->m_llMoney < lGheld)
		{
			cErrorCode = AGPMGUILD_SYSTEM_CODE_NEED_MORE_MONEY;
			lData1 = pcsRequirement->m_lGheld;
		}
	}

	// Error �� �߻�������, �޽����� ������.
	if(cErrorCode != -1)
		SendSystemMessage(cErrorCode, ulNID, NULL, NULL, lData1, lData2);

	return cErrorCode == -1 ? TRUE : FALSE;
}

BOOL AgsmGuild::CheckGuildKorID(CHAR* szGuildID, int idLength)
{
	//������ üũ�Ѵ�.
	BOOL bContainBanLetter = FALSE;

	unsigned char	cData;
	unsigned char	cData2;

	for( INT32 lCounter=0; lCounter<idLength; lCounter++ )
	{
		cData = szGuildID[lCounter];

		//������ �ҹ������� ����.
		if( ('a' <= cData) && (cData <= 'z' ) )
		{
			continue;
		}

		//������ �빮������ ����.
		if( ('A' <= cData) && (cData <= 'Z' ) )
		{
			continue;
		}

		//�������� ����.
		if( ('0' <= cData) && (cData <= '9' ) )
		{
			continue;
		}

		//�ϼ��� �ѱ����� ����. �ѱ��� 0x80���� ũ��.
		if( cData > 0x80 )
		{
			if( lCounter < idLength )
			{
				if( (0xB0 <= cData) && (cData <= 0xC8) )
				{
					cData2 = szGuildID[lCounter+1];

					if( (0xA1 <= cData2) && (cData2 <= 0xFE) )
					{
						//�ѱ��̴ϱ� lCounter++ �Ѵ�.
						lCounter++;
						continue;
					}
				}
			}
		}

		bContainBanLetter = TRUE;
		break;
	}
	
	return !bContainBanLetter;
}

//Ư�������̸� FALSE
BOOL AgsmGuild::CheckGuildCnID(CHAR* szGuildID, int idLength)
{
	unsigned char c1;

	for ( int i = 0; i < idLength; ++i )
	{
		c1 = szGuildID[i];

		if ( c1 < '0'
			|| ( c1 > '9' && c1 < 'A' )//Ư������üũ
			|| ( c1 > 'Z' && c1 < 'a' )
			|| ( c1 > 'z' && c1 <= 127 ) )
			return FALSE;
	}

	return TRUE;
}

// ��� �̸��� ����� �� �ִ� �� üũ�Ѵ�.
// AgsmLoginClient ���� Copy & Paste & Modify
BOOL AgsmGuild::CheckGuildID(CHAR* szGuildID)
{
	BOOL bResult = TRUE;
	INT32 lGuildIDLength = (INT32)strlen(szGuildID);

	if(lGuildIDLength > 0 && lGuildIDLength <= AGPMGUILD_MAX_MAKE_GUILD_ID_LENGTH)
	{
		if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
			bResult = CheckGuildCnID(szGuildID, lGuildIDLength);
		else
			bResult = CheckGuildKorID(szGuildID, lGuildIDLength);
	}
	else
	{
		bResult = FALSE;
	}

	// �弳���� Ȯ���Ѵ�. 2005.10.11. By SungHoon
	if (bResult == TRUE)
	{
		ApSafeArray<CHAR, 128> tempName;
		tempName.MemSetAll();

		strncpy(&tempName[0], szGuildID, 128);
		_mbslwr((unsigned char*)&tempName[0]);

		if (FALSE == m_pagpmCharacter->CheckFilterText(&tempName[0]))
			return FALSE;
	}

	return bResult;
}

// AgpmGuild ���� ��带 �� ���� �� �θ���.
BOOL AgsmGuild::CreateGuildSuccess(CHAR* szGuildID, CHAR* szMasterID)
{
	//STOPWATCH2(GetModuleName(), _T("CreateGuildSuccess"));

	if(!szGuildID || !szMasterID)
	{
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !szGuildID || !szMasterID\n");
		return FALSE;
	}

	// Character ������ �����´�.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
	if(!pcsCharacter)
	{
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !pcsCharacter\n");
		return FALSE;
	}

	INT32 lCID = pcsCharacter->m_lID;

	INT32 lLevel = 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	INT32 lRace = 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);

	INT32 lTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;

	INT32 lServerID = 0;
	UINT32 ulNID = 0;
	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(pcsAgsdCharacter)
	{
		lServerID = pcsAgsdCharacter->m_ulServerID;
		ulNID = pcsAgsdCharacter->m_dpnidCharacter;
	}

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
	{
		pcsCharacter->m_Mutex.Release();
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !pcsGuild\n");
		return FALSE;
	}

	// Max Member Count �� ����ؼ� �ִ´�.
	pcsGuild->m_lMaxMemberCount = CalcMaxMemberCount(pcsCharacter);

	// TID �־��ְ�
	pcsGuild->m_lTID = lTID;
	
	// Master Race �� Guild UnionID �� �־��ش�.
	pcsGuild->m_lUnionID = lRace;

	// Guild ���� ��Ŷ�� ������.
//	SendCreateGuild(pcsGuild, lCID, ulNID);		2005.08.16. By SungHoon

	// �⺻ ����� ���� ������ �����ͼ� ó���Ѵ�.
	AgpdRequireItemIncreaseMaxMember *pcsRequirement = m_pagpmGuild->GetRequireIncreaseMaxMember(AGSMGUILD_MAX_MEMBER_COUNT);
	if (pcsRequirement)
	{
		// �� ���ش�.
		INT32 lGheld = pcsRequirement->m_lGheld;
		INT32 lTaxRatio = m_pagpmCharacter->GetTaxRatio(pcsCharacter);
		INT32 lTax = 0;
		if (lTaxRatio > 0)
		{
			lTax = (lGheld * lTaxRatio) / 100;
		}
		lGheld = lGheld + lTax;	
	
		m_pagpmCharacter->SubMoney(pcsCharacter, lGheld);
		m_pagpmCharacter->PayTax(pcsCharacter, lTax);

		// ��� ��å ���� - arycoat 2008.02
		// �ʿ� �ذ� ����
		// �����۵� ���ش�.
		/*if (pcsRequirement->m_lSkullTID)
		{
			AgpdItem *pcsItem = m_pagpmItem->GetInventoryItemByTID(pcsCharacter, pcsRequirement->m_lSkullTID);
			if(pcsItem)
			{
				m_pagpmItem->SubItemStackCount(pcsItem, pcsRequirement->m_lSkullCount, TRUE);
			}
		}*/
	}

	// Master �� Member �� �־��ش�.
	UINT32 lCurrentTimeStamp = m_pagpmGuild->GetCurrentTimeStamp();
	m_pagpmGuild->JoinMember(pcsGuild, szMasterID, AGPMGUILD_MEMBER_RANK_MASTER, lCurrentTimeStamp,
												lLevel, lTID, AGPMGUILD_MEMBER_STATUS_ONLINE);

	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMasterID);
	if(!pcsMember)	// ���⼭ �� ������ ����
	{
		pcsGuild->m_Mutex.Release();
		pcsCharacter->m_Mutex.Release();
		m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | !pcsMember");
		return FALSE;
	}

	GuildJoinSuccess(pcsGuild, pcsMember, pcsCharacter, TRUE);

	// ����ʴ� �ź� �÷��׸� ���ش�		2005.06.02 By SungHoon
	pcsCharacter->m_lOptionFlag = m_pagpmCharacter->SetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmCharacter->MakePacketOptionFlag(pcsCharacter, pcsCharacter->m_lOptionFlag, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
/*	2005.08.16. By SungHoon
	// Attached Data Setting
	AgpdGuildADChar* pcsAttachedData = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsAttachedData)
	{
		ZeroMemory(pcsAttachedData->m_szGuildID, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1));
		strncpy(pcsAttachedData->m_szGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		pcsAttachedData->m_pMemberData = pcsMember;
	}

	// Master �� Guild �� Join �ߴٰ� �����ش�.
	SendJoinMember(pcsGuild, pcsMember, lCID, ulNID);

	// AgsdGuildMember Data Setting
	AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
	if(pcsAgsdMember)
		UpdateAgsdGuildMember(pcsAgsdMember, lServerID, ulNID, pcsCharacter);

	// Master ���� �ڱ� ��� ������ �ش�.
	SendCharGuildData(szGuildID, szMasterID, pcsMember, lCID, ulNID);

	// �ֺ��� �����̵� �Ѹ���.
	SendCharGuildIDToNear(szGuildID, szMasterID, pcsCharacter->m_stPos, ulNID);
*/
	//////////////////////////////////////////////////////////////////////////
	// DB �� ����.
	// Guild Inert
	EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT, pcsGuild, NULL);

	// Master Insert
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, szGuildID, pcsMember);

	// 2005.03.18. steeple
	// �ٸ� ������ Sync �Ѵ�.
	//SendSyncCreate(pcsGuild);
	//SendSyncJoin(pcsGuild, pcsMember);

	// Guild Unlock
	pcsGuild->m_Mutex.Release();

	// Character Unlock
	pcsCharacter->m_Mutex.Release();

	// ��� ������ �Ǿ��ٴ� �޽����� ������.
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_CREATE_COMPLETE, ulNID);

	// Log
	if (m_pagpmLog)
	{
		INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

		if( pcsAgsdCharacter)
		{
			m_pagpmLog->WriteLog_GuildCreate(0, 
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										lTID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										NULL,
										0									// gheld creation price
										);
		}
	}

	m_pagpmGuild->WriteGuildLog("AgsmGuild::CreateGuildSuccess | return TRUE ����");

	return TRUE;
}

// ����μ��� �Ҹ��� ������ ���߿��� �� �� ���� �߰�. 2004.06.23
BOOL AgsmGuild::CreateGuildFail(CHAR* szGuildID, CHAR* szMasterID)
{
	return TRUE;
}

// pcsCharacter �� Lock �Ǿ� �ִ�.
BOOL AgsmGuild::JoinRequest(CHAR* szGuildID, CHAR* szMasterID, AgpdCharacter* pcsCharacter)
{
	if(!szGuildID || !szMasterID || !pcsCharacter)
		return FALSE;

	if(m_pagpmGuild->IsMaster(szGuildID, szMasterID) == FALSE && m_pagpmGuild->IsSubMaster(szGuildID, szMasterID) == FALSE) 
		return FALSE;

	INT8 cErrorCode = -1;
	UINT32 ulMasterNID = 0;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(pcsGuild)
	{
		if(pcsGuild->m_lMaxMemberCount <= m_pagpmGuild->GetMemberCount(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_MAX_MEMBER;

		// 2005.04.21. steeple
		// ��Ʋ�߿��� �� �� ����~
		if(m_pagpmGuild->IsBattleStatus(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE;

		// 2006.11.14. parn
		// �߱��� ��� union�� �ٸ��� (��, �渶�� ������ �ٸ���) ������ �� ����~
		//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && !m_pagpmCharacter->IsSameRace(pcsCharacter, pcsGuild->m_lUnionID))
		//	cErrorCode = AGPMGUILD_SYSTEM_CODE_JOIN_FAIL;

		AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
		if(pcsMaster)
			ulMasterNID = GetMemberNID(pcsMaster);

		pcsGuild->m_Mutex.Release();
	}

	if(cErrorCode == -1)
	{
		AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
		if(pcsAttachedGuild && (strlen(pcsAttachedGuild->m_szGuildID) != 0 || strlen(pcsAttachedGuild->m_szRequestJoinGuildID) != 0))// �̹� ��Ÿ ��忡 ���� �Ǿ� �ִ� ������� Ȯ��
			cErrorCode = AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER2;
		else
		{
			if(m_pagpmCharacter->IsOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN) == TRUE)		//	2005.06.01 By SungHoon
			{
				SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REFUSE, ulMasterNID, pcsCharacter->m_szID );
				return FALSE;
			}
		}
	}

	if(cErrorCode == -1)
	{
		AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacterLock(szMasterID);
		if(pcsMaster)
		{
			// ������ ��Ʋ������ �ȿ� �ִ�.
			if(m_pagpmPvP->IsCombatPvPMode(pcsCharacter))
			{
				m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_MEMBER, NULL, NULL, 0, 0, pcsMaster);
				pcsMaster->m_Mutex.Release();
				return TRUE;
			}

			// �����Ͱ� ��Ʋ������ �ȿ� �ִ�.
			if(m_pagpmPvP->IsCombatPvPMode(pcsMaster))
			{
				m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_GUILD, NULL, NULL, 0, 0, pcsMaster);
				pcsMaster->m_Mutex.Release();
				return TRUE;
			}

			pcsMaster->m_Mutex.Release();
		}
	}
	if(cErrorCode != -1)
		return SendSystemMessage(cErrorCode, ulMasterNID);
	else
		// pcsCharacter ���� ��Ŷ�� �����ָ� �ȴ�.
		return SendJoinRequest(szGuildID, szMasterID, pcsCharacter->m_szID, pcsCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
}

// �ƹ��͵� Lock �ȵǾ� �ִ�.
BOOL AgsmGuild::JoinCheckEnable(CHAR* szGuildID, CHAR* szCharID, BOOL bSelfRequest )
{
	if(!szGuildID || !szCharID)
		return FALSE;

	INT8 cErrorCode = -1;

	CHAR szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szMasterID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));

	BOOL bJoinRequest = FALSE;
	UINT32 ulMasterNID = 0;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	INT32 lUnionID = -1;
	if(pcsGuild)
	{
		strcpy(szMasterID, pcsGuild->m_szMasterID);

		if(pcsGuild->m_lMaxMemberCount <= m_pagpmGuild->GetMemberCount(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_MAX_MEMBER;

		// 2005.04.21. steeple
		// ��Ʋ�߿��� �� �� ����~
		if(m_pagpmGuild->IsBattleStatus(pcsGuild))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE;

		AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
		if(pcsMaster)
			ulMasterNID = GetMemberNID(pcsMaster);

		if (pcsGuild->m_pGuildJoinList->GetObject(szCharID)) bJoinRequest = TRUE;

		lUnionID = pcsGuild->m_lUnionID;

		pcsGuild->m_Mutex.Release();
	}

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if (pcsCharacter)
	{
		// 2006.11.14. parn
		// �߱��� ��� union�� �ٸ��� (��, �渶�� ������ �ٸ���) ������ �� ����~
		//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && !m_pagpmCharacter->IsSameRace(pcsCharacter, lUnionID))
		//	cErrorCode = AGPMGUILD_SYSTEM_CODE_JOIN_FAIL;
	}

	if(cErrorCode == -1)
	{
		if(!bJoinRequest && pcsCharacter)
		{
			AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
			if (!pcsAttachedGuild)
			{
				if (strlen(pcsAttachedGuild->m_szGuildID) != 0 || strlen(pcsAttachedGuild->m_szRequestJoinGuildID) != 0)	// �̹� ��Ÿ ��忡 ���� �Ǿ� �ִ� ������� Ȯ��
					cErrorCode = AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER2;
			}
		}
	}

	AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacterLock(szMasterID);
	if(pcsMaster)
	{
		// ������ ��Ʋ������ �ȿ� �ִ�.
		if(pcsCharacter)
		{
			if(m_pagpmPvP->IsCombatPvPMode(pcsCharacter))
			{
				m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_MEMBER, NULL, NULL, 0, 0, pcsMaster);
				pcsCharacter->m_Mutex.Release();
				pcsMaster->m_Mutex.Release();
				return TRUE;
			}
		}
		// �����Ͱ� ��Ʋ������ �ȿ� �ִ�.
		if(m_pagpmPvP->IsCombatPvPMode(pcsMaster))
		{
			m_pagpmPvP->ProcessSystemMessage(AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_GUILD, NULL, NULL, 0, 0, pcsMaster);
			if (pcsCharacter) pcsCharacter->m_Mutex.Release();
			pcsMaster->m_Mutex.Release();
			return TRUE;
		}

		pcsMaster->m_Mutex.Release();
	}

	if (cErrorCode != -1)
	{
		// ���� �ý��� �޽����� �����Ϳ�, �����Ϸ��ߴ� ��� �θ��� �� �ش�.
		if (!bSelfRequest) SendSystemMessage(cErrorCode, ulMasterNID);

		if(pcsCharacter && (bSelfRequest || !bJoinRequest))		//	������ ���Խ�û���̰ų� ���Դ�� ���°� �ƴϰ� �α� �Ǿ��ٸ�
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOIN_FAIL, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
		}
	}
	if(pcsCharacter) pcsCharacter->m_Mutex.Release();


	return cErrorCode == -1 ? TRUE : FALSE;
}

// CharID �� Guild ������ �����ߴ�.
// �ƹ��͵� Lock �ȵǾ� ����.
BOOL AgsmGuild::JoinReject(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	UINT32 ulNID = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(pcsMaster)
	{
		ulNID = GetMemberNID(pcsMaster);
	}

	pcsGuild->m_Mutex.Release();
	
	return SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_JOIN_REJECT, ulNID, szCharID);
}

// �ƹ��͵� Lock �ȵǾ� �ְ�,
// AgpmGuild ���� �̹� ������ �� �����̰�, �� ���� ���� �ϸ� �ȴ�.
BOOL AgsmGuild::JoinMember(CHAR* szGuildID, CHAR* szMemberID)
{
	//STOPWATCH2(GetModuleName(), _T("JoinMember"));

	if(!szGuildID || !szMemberID)
		return FALSE;

	if (AllowJoinMember(szGuildID, szMemberID)) return TRUE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMemberID);
	if(!pcsCharacter)
	{
		// ���࿡ ���⼭ ĳ���͸� �� ����� ������ ������Դ� ������� ��.
		// SendNewMemberJoinToOtherMembers(...);
		return FALSE;
	}

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}
/*
	AgpdGuildRequestMember **pcsRequestMember = (AgpdGuildRequestMember **)pcsGuild->m_pGuildJoinList->GetObject(szMemberID);
	if (*pcsRequestMember)
	{
		if (pcsGuild->m_pGuildJoinList->RemoveObject(szMemberID))
			delete *pcsRequestMember;
	}
*/
	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(!pcsMember)
	{
		pcsGuild->m_Mutex.Release();
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	// Member Data �� ����� �������ش�.
	INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);;
	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	INT32 lTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;

	pcsMember->m_lLevel		= lLevel;
	pcsMember->m_lTID		= lTID;
	pcsMember->m_cStatus	= AGPMGUILD_MEMBER_STATUS_ONLINE;
	pcsMember->m_lJoinDate	= m_pagpmGuild->GetCurrentTimeStamp();

	GuildJoinSuccess(pcsGuild, pcsMember, pcsCharacter, TRUE);

	// ��� ������� Join ������ �˸���.
	SendNewMemberJoinToOtherMembers(pcsGuild, pcsMember);
	//////////////////////////////////////////////////////////////////////////
	// DB �� ����.
	// Member Insert
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, szGuildID, pcsMember);

	// 2005.03.18. steeple
	// �ٸ� ������ Sync �Ѵ�.
	//SendSyncJoin(pcsGuild, pcsMember);

	if (m_pagpmLog)
	{
		INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

		CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
		ZeroMemory(szDesc, sizeof(szDesc));
		sprintf(szDesc, "Master=[%s]", pcsGuild->m_szMasterID);

		AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if (pcsAgsdCharacter)
		{
			m_pagpmLog->WriteLog_GuildIn(0, 
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										lTID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										szDesc
										);
		}
	}

	// Unlock
	pcsGuild->m_Mutex.Release();
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// �ƹ��͵� Lock �ȵǾ� ����
// szCharID �� szGuildID ���� Ż���Ѵ�.
BOOL AgsmGuild::Leave(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	CHAR szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szMasterID, 0, AGPACHARACTER_MAX_ID_STRING+1);

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	if (m_pagpmGuild->IsMaster(pcsGuild, szCharID))		//	�渶�� Ż���Ҽ� ����.	2005.10.18. By SungHoon
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	else if (m_pagpmGuild->IsSubMaster(pcsGuild, szCharID))
	{
		memset(pcsGuild->m_szSubMasterID, 0, sizeof(pcsGuild->m_szSubMasterID));
	}

	strncpy(szMasterID, pcsGuild->m_szMasterID, AGPACHARACTER_MAX_ID_STRING);

	// 2005.04.21. steeple
	// ��Ʋ�߿��� �� �� ����~
/*	if(m_pagpmGuild->IsBattleStatus(pcsGuild))
	{
		pcsGuild->m_Mutex.Release();
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
		if(pcsCharacter)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsCharacter->m_Mutex.Release();
		}
		return FALSE;
	}
*/

	SendLeaveAllowToOtherMembers(pcsGuild, szCharID);

	// 2005.03.18. steeple
	// �ٸ� ������ Sync �Ѵ�.
	AgpdGuildMember csMember;
	memset(&csMember, 0, sizeof(csMember));		// ApBase �� ��ӹ��� �ʰ� �������̹Ƿ� memset �ص� �ȴ�.
	strcpy(csMember.m_szID, szCharID);
	//SendSyncLeave(pcsGuild, &csMember);

	// ��� �� Ǯ��
	pcsGuild->m_Mutex.Release();

	//	Ż�� ��� ����Ʈ�� ������ �����ϰ�
	RemoveLeaveMember(szCharID);

	// szCharID ���� ��� ��ü�� �˸���, �����Ѵ�.
	LeaveChar(szGuildID, szMasterID, szCharID);
	//////////////////////////////////////////////////////////////////////////
	// DB
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_DELETE, szGuildID, szCharID);
	
	if(m_pagpmLog)
	{
		// ĳ���� ������ ���� �׳� ���´�.
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(szCharID);
		if(pcsCharacter)
		{
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if(pcsAgsdCharacter)
			{
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

			CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
			ZeroMemory(szDesc, sizeof(szDesc));
			sprintf(szDesc, "Master=[%s]", szMasterID);

			m_pagpmLog->WriteLog_GuildOut(0,
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										szDesc
										);
			}
		}
	}
    
	return TRUE;
}

// �ƹ��͵� Lock �ȵǾ� ����
BOOL AgsmGuild::ForcedLeave(CHAR* szGuildID, CHAR* szCharID)
{
	if(!szGuildID || !szCharID)
		return FALSE;

	CHAR szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szMasterID, 0, AGPACHARACTER_MAX_ID_STRING+1);

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	if (m_pagpmGuild->IsMaster(pcsGuild, szCharID))		//	�渶�� Ż���Ҽ� ����.	2005.10.18. By SungHoon
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	else if (m_pagpmGuild->IsSubMaster(pcsGuild, szCharID))
	{
		memset(pcsGuild->m_szSubMasterID, 0, sizeof(pcsGuild->m_szSubMasterID));
	}

	strncpy(szMasterID, pcsGuild->m_szMasterID, AGPACHARACTER_MAX_ID_STRING);

	// 2005.04.21. steeple
	// ��Ʋ�߿��� �� �� ����~
/*	if(m_pagpmGuild->IsBattleStatus(pcsGuild))
	{
		pcsGuild->m_Mutex.Release();
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
		if(pcsCharacter)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_USABLE, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsCharacter->m_Mutex.Release();
		}
		return FALSE;
	}
*/
	SendForcedLeaveToOtherMembers(pcsGuild, szCharID);

	// 2005.03.18. steeple
	// �ٸ� ������ Sync �Ѵ�.
	AgpdGuildMember csMember;
	memset(&csMember, 0, sizeof(csMember));		// ApBase �� ��ӹ��� �ʰ� �������̹Ƿ� memset �ص� �ȴ�.
	strcpy(csMember.m_szID, szCharID);
	//SendSyncForcedLeave(pcsGuild, &csMember);

	pcsGuild->m_Mutex.Release();

	// szCharID ���� ��� ��ü�� �˸���, �����Ѵ�.
	LeaveChar(szGuildID, szMasterID, szCharID);

	// ©�� �𿡰� �޽����� �����ش�.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if(pcsCharacter)
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE2, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
		pcsCharacter->m_Mutex.Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// DB
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_DELETE, szGuildID, szCharID);

	if(m_pagpmLog)
	{
		// ĳ���� ������ ���� �׳� ���´�.
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(szCharID);
		if(pcsCharacter)
		{
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if(pcsAgsdCharacter)
			{
			INT32 lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

			CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
			ZeroMemory(szDesc, sizeof(szDesc));
			sprintf(szDesc, "Master=[%s] ����Ż��", szMasterID);

			m_pagpmLog->WriteLog_GuildOut(0,
										&pcsAgsdCharacter->m_strIPAddress[0],
										pcsAgsdCharacter->m_szAccountID,
										pcsAgsdCharacter->m_szServerName,
										pcsCharacter->m_szID,
										((AgpdCharacterTemplate *)(pcsCharacter->m_pcsCharacterTemplate))->m_lID,
										lLevel,
										llExp,
										pcsCharacter->m_llMoney,
										pcsCharacter->m_llBankMoney,
										szGuildID,
										szDesc
										);
			}
		}
	}

	return TRUE;
}

// �ƹ��͵� Lock �ȵǾ� ����
// szCharID �� ��带 �����ش�.
BOOL AgsmGuild::LeaveChar(CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID)
{
	if(!szCharID)
		return FALSE;

	// �ش� ĳ���Ϳ��� ��� ��ü�� �˸���, �ڱ� ��� ������ �������ش�.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szCharID);
	if(!pcsCharacter)
		return FALSE;

	INT32 lCID = pcsCharacter->m_lID;
	UINT32 ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsAttachedGuild)
	{
		memset(pcsAttachedGuild->m_szGuildID, 0, AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
		memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
		pcsAttachedGuild->m_pMemberData = NULL;
	}

	AuPOS stPos = pcsCharacter->m_stPos;

	//	��忡 Ż������ ��� ����ʴ� �ź� �÷��׸� �����Ѵ�.	2005.06.07 By SungHoon
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION);

	pcsCharacter->m_Mutex.Release();

	SendDestroyGuild(szGuildID, szMasterID, lCID, ulNID);
	SendCharGuildData(NULL, szCharID, NULL, lCID, ulNID);
	
	// �ֺ��� �����̵� �Ѹ���.
	SendCharGuildIDToNear(NULL, szCharID, stPos, NULL, NULL, FALSE, ulNID);

	return TRUE;
}

// pcsGuild �� AgpmGuild ���� Lock �Ǿ� �ִ�.
// Member �鿡�� DestroyGuild ��Ŷ�� ������.
BOOL AgsmGuild::DestroyGuild(CHAR* szGuildID, CHAR* szMasterID, INT32 lMemberCount, CHAR** pszMemberID, BOOL bSaveDB, BOOL bWriteLog)
{
	if(!szGuildID || !szMasterID || !lMemberCount || !pszMemberID)
		return FALSE;

	CHAR szAccountID[AGPDLOG_MAX_ACCOUNTID+1];
	ZeroMemory(szAccountID, sizeof(szAccountID));
	CHAR szIP[AGPDLOG_MAX_IPADDR_STRING+1];
	ZeroMemory(szIP, sizeof(szIP));
	CHAR szWorld[AGPDLOG_MAX_WORLD_NAME+1];
	ZeroMemory(szWorld, sizeof(szWorld));

	AgpdCharacter* pcsCharacter = NULL;
	AgsdCharacter* pcsAgsdCharacter = NULL;

	INT32 lMasterLevel = 0;
	INT32 lMasterTID = 0;
	INT64 llMasterExp = 0;
	INT64 llMasterGheldInven = 0;
	INT64 llMasterGheldBank = 0;

	for(INT32 i = 0; i < lMemberCount; i++)
	{
		if(!pszMemberID[i])
			continue;

		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pszMemberID[i]);
		if(!pcsCharacter)
			continue;

		pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pcsAgsdCharacter)
		{
			pcsCharacter->m_Mutex.Release();
			continue;
		}

		if (0 == strcmp(pcsCharacter->m_szID, szMasterID))
		{
			m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMasterLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
			llMasterExp	= m_pagpmFactors->GetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
			lMasterTID = ((AgpdCharacterTemplate *)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
			strcpy(szAccountID, pcsAgsdCharacter->m_szAccountID);
			strcpy(szIP, &pcsAgsdCharacter->m_strIPAddress[0]);
			strcpy(szWorld, pcsAgsdCharacter->m_szServerName);
			llMasterGheldInven = pcsCharacter->m_llMoney;
			llMasterGheldBank = pcsCharacter->m_llBankMoney;
		}
		//	��尡 ����� ��� ����ʴ� �ź� �÷��׸� �����Ѵ�.	2005.06.07 By SungHoon
		pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
		pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
		pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION);

		// �ش� Character �� Guild Attached Data �� �ʱ�ȭ �Ѵ�.
		m_pagpmGuild->SetCharAD(pcsCharacter, NULL, -1, -1, FALSE, NULL);

		// �ֺ��� �����̵� �Ѹ���.
		SendCharGuildIDToNear(NULL, pcsCharacter->m_szID, pcsCharacter->m_stPos, NULL, NULL, FALSE, pcsAgsdCharacter->m_dpnidCharacter);

		// Guild Destroy ��Ŷ�� ������.
		SendDestroyGuild(szGuildID, szMasterID, 0, pcsAgsdCharacter->m_dpnidCharacter);

		SendCharGuildData(NULL, pcsCharacter->m_szID, NULL, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);

		// ��� �޽����� ������.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_DESTROY, pcsAgsdCharacter->m_dpnidCharacter);

		pcsCharacter->m_Mutex.Release();
	}

	//////////////////////////////////////////////////////////////////////////
	// DB
	if(bSaveDB)
	{
		EnumCallback(AGSMGUILD_CB_DB_GUILD_DELETE, szGuildID, NULL);

		// 2005.03.18. steeple
		// �ٸ� ������ Sync �Ѵ�.
		AgpdGuild csGuild;		// �̳��� memset ���� ����
		strcpy(csGuild.m_szID, szGuildID);
		//SendSyncDestroy(&csGuild);
	}

	if(bWriteLog && m_pagpmLog)
	{
		INT64 llExp = m_pagpmCharacter->GetExp(pcsCharacter);

		m_pagpmLog->WriteLog_GuildRemove(0,
										szIP,
										szAccountID,
										szWorld,
										szMasterID,
										lMasterTID,
										lMasterLevel,
										llMasterExp,
										llMasterGheldInven,
										llMasterGheldBank,
										szGuildID,
										NULL
										);
	}

	return TRUE;
}

// pcsGuild �� AgpmGuild ���� Lock �Ǿ� �ִ�.
BOOL AgsmGuild::DestroyGuildFail(CHAR* szGuildID, CHAR* szMasterID)
{
	return TRUE;
}

// 2006.08.31. steeple
// szGuildID �� Guild �� ���� �� �̹� Lock �Ǿ� ������ �׳� ��⸸ �ϸ� �ȴ�.
BOOL AgsmGuild::CleanUpRelation(CHAR* szGuildID, BOOL bSaveDB)
{
	if(!szGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// Joint ����
	JointLeave(0, szGuildID, TRUE);

	// Hostile
	HostileVector vcHostile(pcsGuild->m_csRelation.m_pHostileVector->begin(), 
							pcsGuild->m_csRelation.m_pHostileVector->end());
	HostileIter iter = vcHostile.begin();
	while(iter != vcHostile.end())
	{
		HostileLeave(szGuildID, iter->m_szGuildID, TRUE);
		++iter;
	}

	return TRUE;
}

// pcsCharacter �� Lock �Ǿ� �ִ�.
// 2004.11.15. steeple - ���� ����
INT32 AgsmGuild::CalcMaxMemberCount(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	INT32 lMaxMemberCount = 0;

	//INT32 lCharisma = 0;
	INT32 lLevel = 0;

	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCharisma, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	//lMaxMemberCount = ((INT32)(lCharisma / 2) + (INT32)(lLevel / 10));
	//lMaxMemberCount = 4 + (INT32)(lLevel / 5);

	lMaxMemberCount = AGSMGUILD_MAX_MEMBER_COUNT;	// 50���� ����Ǿ���. 2005.06.09. steeple
	return lMaxMemberCount;
}

// ĳ���Ͱ� Level Up �Ǹ� �ٽ� ��� �Ǽ� ����Ǿ��� �� �Ҹ���.
// Client �� ��Ŷ �����ְ�, DB �� Update �� �����ش�.
BOOL AgsmGuild::UpdateMaxMemberCount(AgpdGuild* pcsGuild, INT32 lNewMaxMember, BOOL bSaveDB)
{
	if(!pcsGuild || lNewMaxMember == 0)
		return FALSE;

	pcsGuild->m_lMaxMemberCount = lNewMaxMember;

	// ������ �ִ� ��� ������� �Ѹ���,
	SendMaxMemberCountToAllMembers(pcsGuild);

	// DB ������Ʈ�� �Ѵ�.
	if(bSaveDB)
	{
		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);

		// 2005.03.18. steeple
		// �ٸ� ������ Sync �Ѵ�.
		//SendSyncUpdateMaxMemberCount(pcsGuild);
	}

	return TRUE;
}

// pcsGuild �� AgpmGuild ���� Lock �Ǿ� �ִ�.
BOOL AgsmGuild::CheckPassword(AgpdGuild* pcsGuild, CHAR* szPassword)
{
	if(!pcsGuild || !szPassword)
		return FALSE;

	if(strlen(szPassword) == 0 || strlen(szPassword) > AGPMGUILD_MAX_PASSWORD_LENGTH)
		return FALSE;

	BOOL bValidPassword = FALSE;
	if(strcmp(pcsGuild->m_szPassword, szPassword) == 0)
		bValidPassword = TRUE;

	if(!bValidPassword)
	{
		AgpdGuildMember* pcsMember = m_pagpmGuild->GetMaster(pcsGuild);
		if(pcsMember)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_INVALID_PASSWORD, GetMemberNID(pcsMember));
		}
	}

	return bValidPassword;
}

// AgsdGuildMember Data Update
BOOL AgsmGuild::UpdateAgsdGuildMember(CHAR* szGuildID, CHAR* szMemberID, INT32 lServerID, UINT32 ulNID, AgpdCharacter* pcsCharacter)
{
	if(!szGuildID || !szMemberID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(pcsMember)
	{
		AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
		UpdateAgsdGuildMember(pcsAgsdMember, lServerID, ulNID, pcsCharacter);
	}

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

// AgsdGuildMember Data Update
BOOL AgsmGuild::UpdateAgsdGuildMember(AgsdGuildMember* pcsAgsdMember, INT32 lServerID, UINT32 ulNID, AgpdCharacter* pcsCharacter)
{
	if(!pcsAgsdMember)
		return FALSE;

	pcsAgsdMember->m_lServerID = lServerID;
	pcsAgsdMember->m_ulNID = ulNID;

	pcsAgsdMember->m_pcsCharacter = pcsCharacter;

	return TRUE;
}

BOOL AgsmGuild::EnterGameWorld(AgpdCharacter* pcsCharacter)
{
	if( !pcsCharacter )								return FALSE;
	if( !m_pagpmCharacter->IsPC(pcsCharacter) )		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if( !pcsAgsdCharacter )							return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if( !pcsAttachedGuild )							return TRUE;
	if( !strlen(pcsAttachedGuild->m_szGuildID) )	return TRUE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock( pcsAttachedGuild->m_szGuildID );
	if( !pcsGuild )									return TRUE;

	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
	if( pcsMember )
	{
		// ������ ������� Guild Create �� �����ش�.
		pcsAttachedGuild->m_lGuildMarkTID	= pcsGuild->m_lGuildMarkTID;
		pcsAttachedGuild->m_lGuildMarkColor	= pcsGuild->m_lGuildMarkColor;
		pcsAttachedGuild->m_lBRRanking		= pcsGuild->m_lBRRanking;

		SendCreateGuild( pcsGuild, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter );
		
		UpdateAgsdGuildMember( GetADMember( pcsMember ), pcsAgsdCharacter->m_ulServerID, pcsAgsdCharacter->m_dpnidCharacter, pcsCharacter );

		pcsMember->m_cStatus	= AGPMGUILD_MEMBER_STATUS_ONLINE;
		pcsMember->m_lLevel		= m_pagpmCharacter->GetLevel(pcsCharacter);

		// ������ ������� ��� ����� Join ��Ų��.
		SendAllMemberJoin( pcsGuild, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter) ;

		// �ڽ��� ������ ��� ������� ������.
		SendMemberUpdateToMembers( pcsGuild, pcsMember );
		
		// ���� ����鿡�� ��� ���̵� �Ѹ���.
		SendCharGuildIDToNear(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID, pcsCharacter->m_stPos, 
			pcsAttachedGuild->m_lGuildMarkTID, pcsAttachedGuild->m_lGuildMarkColor, pcsAttachedGuild->m_lBRRanking, 
			pcsAgsdCharacter->m_dpnidCharacter);
		
		EnumCallback( AGSMGUILD_CB_SEND_GUILD_INFO, pcsCharacter, NULL );

		// 2005.04.17. steeple
		// ��尡 ����� ���̶�� ���������� ������ �Ѵ�.
		if( pcsGuild->IsBattle(pcsCharacter->m_szID) )
		{
			// ���� �� ��带 PvP �����Ϳ� ����
			EnumCallback( AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_BATTLE, pcsCharacter, pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID );	
		}
		
		// ��� ��Ʋ ���� ����
		SendBattleInfo( pcsGuild, pcsAgsdCharacter->m_dpnidCharacter );

		// ����, ���� ��� ���� �����ش�.
		SendJointAll(pcsGuild, pcsAgsdCharacter->m_dpnidCharacter);
		SendHostileAll(pcsGuild, pcsAgsdCharacter->m_dpnidCharacter);

	}		//	���� ��û��
	else
	{
		memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
		strncpy(pcsAttachedGuild->m_szRequestJoinGuildID, pcsAttachedGuild->m_szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		memset(pcsAttachedGuild->m_szGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));

		AgpdGuildRequestMember *pcsRequestMember = GetJoinRequestMember(pcsGuild, pcsCharacter->m_szID);
		if (pcsRequestMember)
		{
			pcsRequestMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;
			SendCreateGuild(pcsGuild,  pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
			SendSelfJoinMember(pcsAttachedGuild->m_szRequestJoinGuildID, pcsRequestMember, pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
			SendCharGuildData(pcsRequestMember,  pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}

	pcsGuild->m_Mutex.Release();
	
	return TRUE;
}

BOOL AgsmGuild::Disconnect(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// ���Ե� ��尡 ���ٸ� �׳� ������.
	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("Disconnect"));

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
	if(pcsGuild)
	{
		AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
		if(pcsMember)
		{
			// AgsdGuildMember Update
			AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
			if(pcsAgsdMember)
				UpdateAgsdGuildMember(pcsAgsdMember, 0, 0);

			// �α׾ƿ��� Member Status �� �ٲٰ�
			pcsMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			// �ٸ� ����鿡�� ������Ʈ�� �˸���.
			SendMemberUpdateToMembers(pcsGuild, pcsMember, FALSE);

			// ����� ���� ��� - arycoat 2007.10.30
			if( m_pagpmGuild->IsBattleStatus(pcsGuild) )
				CalcBattleScoreByDisconnect(pcsCharacter);				
		}
		AgpdGuildRequestMember* pcsRequestMember = GetJoinRequestMember(pcsGuild, pcsCharacter->m_szID);
		if(pcsRequestMember)
		{
			pcsRequestMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			SendRequestJoinMemberUpdateToMembers(pcsGuild, pcsRequestMember , FALSE);

		}
		pcsGuild->m_Mutex.Release();
	}
	
	return TRUE;
}

// From AgsmCharManager
// Login Server ���� ĳ���Ͱ� �������� �� �Ҹ���.
BOOL AgsmGuild::CharacterDeleteComplete(CHAR* szCharID)
{
	if(!szCharID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->FindGuildLockFromCharID(szCharID);
	if(!pcsGuild)
		return TRUE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, szCharID);
	if(!pcsGuildMember)
		return FALSE;

	CHAR szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1] = {0, };
	CHAR szPassword[AGPMGUILD_MAX_PASSWORD_LENGTH+1] = {0, };

	strncpy(szGuildID, pcsGuild->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	strncpy(szPassword, pcsGuild->m_szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH);

	BOOL bIsMaster = m_pagpmGuild->IsMaster(pcsGuild, szCharID);
	if (bIsMaster) // ���ֶ��
	{
		// ��Ʋ���̰� �����Ͷ�� GG �� ó�����ش�.
		if (m_pagpmGuild->IsBattleStatus(pcsGuild))
			BattleWithdraw(pcsGuild, szCharID);

		// ���ֶ�� ���� �ڰ� ��Ż
		if (AgpdSiegeWar* pcsSiegeWar = m_pagpmSiegeWar->IsOwnerGuild(szGuildID))
		{
			static AgsmSiegeWar *pagsmSiegeWar = (AgsmSiegeWar*)GetModule("AgsmSiegeWar");
			
			m_pagpmSiegeWar->SetNewOwner(pcsSiegeWar, NULL);
			pagsmSiegeWar->SendPacketCastleInfoToAll(pcsSiegeWar);
		}
	}

	// Guild Unlock
	pcsGuild->m_Mutex.Release();

	switch(pcsGuildMember->m_lRank)
	{
		case AGPMGUILD_MEMBER_RANK_MASTER:
		{
			// �����Ͷ�� ��� ��ü!!!
			m_pagpmGuild->OnOperationDestroy(1, szGuildID, szCharID, szPassword, TRUE);
		} break;
		case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			// ���긶���Ͷ�� ���긶���� �ڸ� ��������!!!
			ZeroMemory(pcsGuild->m_szSubMasterID, AGPACHARACTER_MAX_ID_STRING);
			m_pagpmGuild->OnOperationLeaveAllow(0, szGuildID, szCharID, TRUE, TRUE);
		} break;
		default:
		{
			// �׳� �����̸� Ż��!!!
			m_pagpmGuild->OnOperationLeaveAllow(0, szGuildID, szCharID, TRUE, TRUE);
		}
	}

	return TRUE;
}

// Character �� Level UP �ϸ� �Ҹ���.
BOOL AgsmGuild::CharacterLevelUp(AgpdCharacter* pcsCharacter, INT32 lLevelUpNum)
{
	//STOPWATCH2(GetModuleName(), _T("ChaacterLevelUp"));

	if(!pcsCharacter)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
	if(!pcsGuild)
		return TRUE;
/*		�����Ͱ� �������ص� ��� �ִ��ο��� ������ �ʴ´�.
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(pcsMaster)
	{
		// �����Ͱ� ������ �ߴ�!!!
		if(strcmp(pcsMaster->m_szID, pcsCharacter->m_szID) == 0)
		{
			INT32 lOldMaxMemberCount = pcsGuild->m_lMaxMemberCount;
			INT32 lNewMaxMemberCount = CalcMaxMemberCount(pcsCharacter);

			// Max Member Count �� ����Ǿ����� DB �� ���ش�.
			if(lOldMaxMemberCount != lNewMaxMemberCount)
				UpdateMaxMemberCount(pcsGuild, lNewMaxMemberCount);
		}
	}
*/
	// ��� ������ �ٲ��ش�.
	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
	if(pcsMember)
	{
		// Level ����
		//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &pcsMember->m_lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

		// 2005.06.27. steeple
		// ��¥ ���� �Ͼ�� ������ ������ 0 ���� �Ǳ淡 �ٲ���.
		pcsMember->m_lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

		// �ڱ⸦ ������ ��� ������� �˸���.
		SendMemberUpdateToMembers(pcsGuild, pcsMember);
	}

	AgpdGuildRequestMember* pcsRequestMember = GetJoinRequestMember(pcsGuild, pcsCharacter->m_szID);
	if(pcsRequestMember)
	{
		pcsRequestMember->m_lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

		SendRequestJoinMemberUpdateToMembers(pcsGuild,pcsRequestMember , FALSE);

	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::GuildMessage(AgpdCharacter* pcsCharacter, CHAR* szMessage, INT32 lLength)
{
	if(!pcsCharacter || !szMessage || lLength < 1 || lLength > 254)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
	if(!pcsGuild)
		return FALSE;
		
	if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		return TRUE;

	if(m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_GUILD_MESSAGE) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
		return TRUE;

	AgpdGuildMember** ppcsMember;
	AgsdGuildMember* pcsAgsdMember = NULL;

	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;
			
		AgsdGuildMember* pcsAgsdMember = GetADMember(*ppcsMember);
		if(!pcsAgsdMember) continue;
		if(m_pagpmBattleGround->IsInBattleGround(pcsAgsdMember->m_pcsCharacter)) continue;
		if(pcsAgsdMember->m_pcsCharacter)
		{
			if(m_papmMap->CheckRegionPerculiarity(pcsAgsdMember->m_pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_GUILD_MESSAGE) == APMMAP_PECULIARITY_RETURN_DISABLE_USE) continue;
		}

		m_pagsmChatting->SendMessage(pcsAgsdMember->m_ulNID, AGPDCHATTING_TYPE_GUILD, pcsCharacter->m_lID, pcsCharacter->m_szID,
															NULL, szMessage, lLength);
	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::GuildJointMessage(AgpdCharacter* pcsCharacter, CHAR* szMessage, INT32 lLength)
{
	if(!pcsCharacter || !szMessage || lLength < 1 || lLength > 254)
		return FALSE;

	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pcsAttachedGuild->m_szGuildID);
	if(!pcsGuild)
		return FALSE;

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		AgpdGuild* pcsJoint = NULL;
		if(_tcscmp(pcsGuild->m_szID, iter->m_szGuildID) == 0)
			pcsJoint = pcsGuild;
		else
		{
			pcsJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(!pcsJoint)
			{
				++iter;
				continue;
			}

			pcsJoint->m_Mutex.WLock();
		}

		AgpdGuildMember** ppcsMember;
		INT32 lIndex = 0;
		for(ppcsMember = (AgpdGuildMember**)pcsJoint->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
				ppcsMember = (AgpdGuildMember**)pcsJoint->m_pMemberList->GetObjectSequence(&lIndex))
		{
			if(!*ppcsMember)
				continue;

			m_pagsmChatting->SendMessage(GetMemberNID(*ppcsMember),
										AGPDCHATTING_TYPE_GUILD_JOINT,
										pcsCharacter->m_lID,
										pcsCharacter->m_szID,
										NULL,
										szMessage, lLength);
		}

		if(pcsJoint != pcsGuild)
			pcsJoint->m_Mutex.Release();

		++iter;
	}

	return TRUE;
}

BOOL AgsmGuild::IsGuildMaster(AgpdCharacter *pcsCharacter)
{
	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(NULL == pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) <= 0)
		return FALSE;

	return (m_pagpmGuild->IsMaster(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID));
}

BOOL AgsmGuild::IsSameGuild(AgpdCharacter *pcsChar1, AgpdCharacter *pcsChar2)
{
	AgpdGuildADChar *pcsGuild1 = m_pagpmGuild->GetADCharacter(pcsChar1);
	if( !pcsGuild1 )							return FALSE;
	if( strlen( pcsGuild1->m_szGuildID ) <= 0 )	return FALSE;

	AgpdGuildADChar *pcsGuild2 = m_pagpmGuild->GetADCharacter(pcsChar2);
	if( !pcsGuild2 )							return FALSE;
	if( strlen( pcsGuild2->m_szGuildID ) <= 0 )	return FALSE;

	return 0 == strcmp( pcsGuild1->m_szGuildID, pcsGuild2->m_szGuildID ) ? TRUE : FALSE;
}

UINT32 AgsmGuild::GetMemberNID(AgpdGuildMember* pcsMember)
{
	if(!pcsMember)
		return 0;

	AgsdGuildMember* pcsAgsdMember = GetADMember(pcsMember);
	if(!pcsAgsdMember)
		return 0;

	//STOPWATCH2(GetModuleName(), _T("GetMemberNID"));

	// 2007.04.11. steeple
	// ĳ���� �� ���ϰ� �׳� AgsdMember �� �ִ� �� �����ϰ� ����.
	return pcsAgsdMember->m_ulNID;

	// 2007.04.11. steeple
	//
	//// 2005.04.29. steeple
	//// ĳ���� �����͸� ��� �ִٰ� �װɷ� �ϴٺ��� ������ �׾���. �Ϸ翡 �ѹ� ����.
	//// ���߿� ������� Ŀ���� �Ǹ� �� ���� ���� ���� ���� �׳� �׶� �׶� ���ؼ� ���ϴ� �ɷ� �ٲ�.
	//// �� �۾��� �ð��� ����� ���� �ɸ��ٸ� �׶����� �ٸ� ����� ����غ���.
 //   UINT32 ulNID = 0;
	//AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(pcsMember->m_szID);
	//if(pcsCharacter)
	//{
	//	ulNID = m_pagsmCharacter->GetCharDPNID(pcsAgsdMember->m_pcsCharacter);
	//	pcsCharacter->m_Mutex.Release();
	//}

	//return ulNID;

	//// 2005.04.27. steeple
	//// ���� NID ���׷� ���ؼ� �׳� ĳ���� �����͸� ��� �ְ� �׶� �׶� �������ֱ�� ��.
	//if(pcsAgsdMember->m_pcsCharacter)
	//	return m_pagsmCharacter->GetCharDPNID(pcsAgsdMember->m_pcsCharacter);

	//// �׷��� ������ ���� ������ ����
	//return pcsAgsdMember->m_ulNID;
}

// 2005.06.16. steeple
// ���� ���´ٴ� ���� ���¿� ���� ����.
// �װ��� �����ϱ� ���ؼ� 30�ʿ� �ѹ��� ���������� ���ش�.
BOOL AgsmGuild::ProcessRefreshAllGuildMemberStatus(UINT32 ulClockCount)
{
	//STOPWATCH2(GetModuleName(), _T("ProcessRefreshAllGuildMemberStatus"));

	// ��� ��带 ����.
	AgpdGuild* pcsGuild = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	AgpdGuildRequestMember **ppcsRequestJoinMember = NULL;

	INT32 lIndex1 = 0, lIndex2 = 0;
	for(pcsGuild = m_pagpmGuild->GetGuildSequence(&lIndex1); pcsGuild; pcsGuild = m_pagpmGuild->GetGuildSequence(&lIndex1))
	{
		AuAutoLock csLock(pcsGuild->m_Mutex);
		if (!csLock.Result()) continue;

		if(pcsGuild->m_ulLastRefreshClock == 0)	// ó�� ���� ����.
		{
			pcsGuild->m_ulLastRefreshClock = ulClockCount;
			continue;
		}

		if(ulClockCount - pcsGuild->m_ulLastRefreshClock <= AGSMGUILD_IDLE_MEMBER_STATUS_REFRESH)
			continue;

		pcsGuild->m_ulLastRefreshClock = GetClockCount();		// ���� ���Ѵ�. ���ڷ� �Ѿ�� ���� ���� ����.

		lIndex2 = 0;
		for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex2))
		{
			if(!*ppcsMember)
				break;

			if(m_pagpmCharacter->GetCharacter((*ppcsMember)->m_szID))			// ĳ���Ͱ� �������ִ�.
				(*ppcsMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;
			else
				(*ppcsMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			SendMemberUpdateToMembers(pcsGuild, *ppcsMember);
		}
		lIndex2 = 0;
		for(ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex2); ppcsRequestJoinMember;
			ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex2))
		{
			if(!*ppcsRequestJoinMember)
				break;

			if(m_pagpmCharacter->GetCharacter((*ppcsRequestJoinMember)->m_szMemberID))			// ĳ���Ͱ� �������ִ�.
				(*ppcsRequestJoinMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;
			else
				(*ppcsRequestJoinMember)->m_cStatus = AGPMGUILD_MEMBER_STATUS_OFFLINE;

			SendRequestJoinMemberUpdateToMembers(pcsGuild, *ppcsRequestJoinMember);
		}

	}

	return TRUE;
}

// 2005.04.15. steeple
// �������� �����͸� �ٲ� �� �ֵ��� �������
BOOL AgsmGuild::SetCustomBattleTime(UINT32 ulReadyTime, UINT32 ulDurationMin, UINT32 ulDurationMax, UINT32 ulCancelEnableTime,
									INT32 lBattleNeedMemberCount, INT32 lBattleNeedLevelSum)
{
	m_ulBattleReadyTime			= ulReadyTime != 0 ? ulReadyTime : AGPMGUILD_BATTLE_READY_TIME;
	m_ulBattleCancelEnableTime	= ulCancelEnableTime != 0 ? ulCancelEnableTime : AGPMGUILD_BATTLE_CANCEL_ENABLE_TIME;
	m_lBattleNeedMemberCount	= lBattleNeedMemberCount != 0 ? lBattleNeedMemberCount : AGPMGUILD_BATTLE_NEED_MEMBER_COUNT;
	m_lBattleNeedLevelSum		= lBattleNeedLevelSum != 0 ? lBattleNeedLevelSum : AGPMGUILD_BATTLE_NEED_LEVEL_SUM;

	return TRUE;
}

// 2005.04.15. steeple
// ���� ��带 �� üũ�Ѵ�.
// -1 �� �����ؾ� ������ ����!!!! -_-;;; �򰥷��� �ҿ����~
INT32 AgsmGuild::CheckBattleEnable(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild)
{
	if(!pcsGuild || !pcsEnemyGuild)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_EXIST_GUILD;


	// ��� ��� �������� ���� üũ
	AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
	if(!pcsEnemyMaster || pcsEnemyMaster->m_cStatus != AGPMGUILD_MEMBER_STATUS_ONLINE)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;

	AgpdCharacter* pcsCharacterEnemy = m_pagpmCharacter->GetCharacter(pcsEnemyMaster->m_szID);
	if (!pcsCharacterEnemy)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		
	if(m_pagpmCharacter->IsOptionFlag(pcsCharacterEnemy, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE) == TRUE)		//	2005.06.01 By SungHoon
		return AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE;
		
		
	// ����� ���� ���� üũ
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(!pcsMaster || pcsMaster->m_cStatus != AGPMGUILD_MEMBER_STATUS_ONLINE)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(pcsMaster->m_szID);
	if (!pcsCharacter)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		
	/*if (m_pagpmSiegeWar->GetSiegeWarInfo(pcsCharacter))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS;
		
	if (m_pagpmSiegeWar->GetSiegeWarInfo(pcsCharacterEnemy))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2;
		
	if(m_pagpmCharacter->IsInDungeon(pcsCharacter))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS;
		
	if(m_pagpmCharacter->IsInDungeon(pcsCharacterEnemy))
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2;*/
		

	/*if(m_pagpmGuild->GetMemberCount(pcsGuild) < m_lBattleNeedMemberCount)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT;

	if(m_pagpmGuild->GetMemberLevelSum(pcsGuild) < m_lBattleNeedLevelSum)
		return AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL;

	if(m_pagpmGuild->GetMemberCount(pcsEnemyGuild) < m_lBattleNeedMemberCount)
		return AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_ENOUGH_MEMBER_COUNT2;

	if(m_pagpmGuild->GetMemberLevelSum(pcsEnemyGuild) < m_lBattleNeedLevelSum)
		return AGPMGUILD_SYSTME_CODE_BATTLE_NOT_ENOUGH_MEMBER_LEVEL2;*/

	return -1;
}

// 2005.04.15. steeple
// �� ��忡�� �ٸ� ��忡�� ��Ʋ ��û�� �ߴ�.
BOOL AgsmGuild::BattleRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson)
{
	if( !szGuildID || !szMasterID || !szEnemyGuildID )
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock( szGuildID );
	if( !pcsGuild )		return FALSE;

	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if( !pcsMaster || strcmp(pcsMaster->m_szID, szMasterID) != 0 )
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	INT32 lErrorCode		= -1;
	UINT32 ulEnemyMasterNID	= 0;
	UINT32 ulMasterNID		= GetMemberNID(pcsMaster);

	// ����� ��带 ���Ѵ�.
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID);
	if(!pcsEnemyGuild)
	{
		lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_EXIST_GUILD;
	}
	else
	{
		// ����� ���� üũ
		if(m_pagpmGuild->IsBattleStatus(pcsGuild))
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS;
		
		if(m_pagpmGuild->IsBattleStatus(pcsEnemyGuild))
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_NOT_REQUEST_STATUS2;
		
		if(lErrorCode == -1)
		{
			lErrorCode = CheckBattleEnable(pcsGuild, pcsEnemyGuild);
			ulEnemyMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsEnemyGuild));
		}

		pcsEnemyGuild->m_Mutex.Release();
	}
	pcsGuild->m_Mutex.Release();

	if(lErrorCode != -1)
	{
		// ������ �߻��ߴ�.
		if (lErrorCode == AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE)
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE, ulMasterNID, szEnemyGuildID); //, NULL, 0, 0);
		else
			SendSystemMessage(lErrorCode, ulMasterNID, NULL, NULL, m_lBattleNeedMemberCount, m_lBattleNeedLevelSum);
	}
	else
	{
		pcsGuild->m_cStatus			= AGPMGUILD_STATUS_BATTLE_DECLARE; // ���� ��û
		strcpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsEnemyGuild->m_szID);
		
		pcsEnemyGuild->m_cStatus	= AGPMGUILD_STATUS_BATTLE_DECLARE; // ���� ��û	
		strcpy(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild->m_szID);
		
		SendBattleRequest(szEnemyGuildID, szGuildID, lType, ulDuration, ulPerson, ulEnemyMasterNID);

		// ����� ��û�� ������� ��û�ߴٰ� �˷��ش�.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_REQUEST, ulMasterNID, NULL, NULL, 0, 0);
	}

	return TRUE;
}

// 2005.04.16. steeple
// ��Ʋ�� �����ߴ�. ���⼭ �˻縦 ����ϸ� ��Ʋ list ���� �����ϰ� �ȴ�.
// pcsGuild �� Lock �Ǿ �Ѿ�´�.
BOOL AgsmGuild::BattleAccept(CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson)
{
	if( !szGuildID || !szEnemyGuildID )	return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if(!pcsGuild)		return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID, pLock2);
	if(!pcsEnemyGuild)	return FALSE;
	

	if( !m_pagpmGuild->IsBattleDeclareStatus(pcsGuild) || strcmp(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsEnemyGuild->m_szID) != 0 ) 
		return FALSE;

	if( !m_pagpmGuild->IsBattleDeclareStatus(pcsEnemyGuild) || strcmp(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild->m_szID) != 0)
	{
		m_pagpmGuild->InitCurrentBattleInfo(pcsGuild);
		return FALSE;	
	}

	// �ٽ��ѹ� üũ���ش�.
	INT32 lErrorCode = CheckBattleEnable(pcsGuild, pcsEnemyGuild);
	if(lErrorCode == -1)
	{
		if( !m_pagpmGuild->CheckBattleType( pcsGuild, pcsEnemyGuild, (eGuildBattleType)lType ) )
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_BOTH_MEMBER_COUNT;
			
		AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
		if(!pcsEnemyMaster || pcsEnemyMaster->m_cStatus != AGPMGUILD_MEMBER_STATUS_ONLINE)
		{
			lErrorCode = AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE;
		}
	}
	
	if(lErrorCode != -1)	// 2006.04.19. steeple. -1 �� �ƴϸ� ������. �̤�
	{
		UINT32 ulNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild));

		if(lErrorCode != AGPMGUILD_SYSTEM_CODE_BATTLE_REFUSE)
			SendSystemMessage(lErrorCode, ulNID, szEnemyGuildID); //, NULL, 0, 0);

		return FALSE;
	}

	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	if( AddBattleProcess( szGuildID, szEnemyGuildID, AGPMGUILD_STATUS_BATTLE_READY, ulCurrentTime, m_ulBattleReadyTime, 
							ulCurrentTime + m_ulBattleReadyTime, lType, ulDuration, ulPerson ) )
	{
		// ���� �� ��带 �������ְ�
		strcpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szEnemyGuildID);
		strcpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, pcsEnemyGuild->m_szMasterID);
		strcpy(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, szGuildID);
		strcpy(pcsEnemyGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, pcsGuild->m_szMasterID);

		// ��Ʋ �����͸� �����Ѵ�.
		pcsGuild->m_cStatus = pcsEnemyGuild->m_cStatus	= AGPMGUILD_STATUS_BATTLE_READY;
		pcsGuild->m_csCurrentBattleInfo.m_eType			= pcsEnemyGuild->m_csCurrentBattleInfo.m_eType			= (eGuildBattleType)lType;
		pcsGuild->m_csCurrentBattleInfo.m_ulPerson		= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulPerson		= ulPerson;
		pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulAcceptTime	= ulCurrentTime;
		pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulReadyTime	= m_ulBattleReadyTime;
		pcsGuild->m_csCurrentBattleInfo.m_ulStartTime	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulStartTime	= ulCurrentTime + m_ulBattleReadyTime;
		pcsGuild->m_csCurrentBattleInfo.m_ulDuration	= pcsEnemyGuild->m_csCurrentBattleInfo.m_ulDuration		= ulDuration;
		pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime = pcsEnemyGuild->m_csCurrentBattleInfo.m_ulCurrentTime	= ulCurrentTime;

		// ��Ŷ�� �����ش�.
		SendBattleAcceptToAllMembers(pcsGuild);
		SendBattleAcceptToAllMembers(pcsEnemyGuild);

		// DB ������ ���� �ʴ´�.
		//EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);
		//EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsEnemyGuild, NULL);
		
		// Log
		/*
		if (m_pagpmLog)
		{
			AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
			m_pagpmLog->WriteLog_GuildBattleAccept();
		}
		*/
	}
	else
	{
		// �� �̷��� �̹� ��Ʋ ����Ʈ�� �ִ� �ǵ� �� �Լ��� ���ٴ� ���� ������ ���� -0-
	}

	return TRUE;
}

// 2005.04.16. steeple
// ��Ʋ�� �����ߴ�. �׳� ���� �޽����� �����ָ� �ɵ�.
BOOL AgsmGuild::BattleReject(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if( !szGuildID || !szMasterID || !szEnemyGuildID )		return FALSE;
	
	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if(!pcsGuild) return FALSE;
	if( !m_pagpmGuild->IsMaster(szGuildID, szMasterID) )	return FALSE;
		
	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID, pLock2);
	if( !pcsEnemyGuild )		return FALSE;

	AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
	if( pcsEnemyMaster )
	{
		SendSystemMessage( AGPMGUILD_SYSTEM_CODE_BATTLE_REJECT_BY_OTHER, GetMemberNID(pcsEnemyMaster), szGuildID );
	}

	if( m_pagpmGuild->IsBattleDeclareStatus(pcsGuild) )
		m_pagpmGuild->InitCurrentBattleInfo(pcsGuild);

	if( m_pagpmGuild->IsBattleDeclareStatus(pcsEnemyGuild) )
		m_pagpmGuild->InitCurrentBattleInfo(pcsEnemyGuild);

	return TRUE;
}

// 2005.04.16. steeple
// ��Ʋ ��Ҹ� ��û�Ѵ�. �ٰ����� ���۽ð��� 1�� �̸��̶�� �Ұ����ϴ�.
BOOL AgsmGuild::BattleCancelRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if( !szGuildID || !szMasterID || !szEnemyGuildID )		return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if( !pcsGuild )		return FALSE;

	// �����Ͱ� �ƴϸ� GG
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if( !pcsMaster || strcmp( pcsMaster->m_szID, szMasterID ) )
		return FALSE;
		
	if(m_pagpmGuild->IsBattleDeclareStatus(pcsGuild))
	{
		AuAutoLock pLock3;
		AgpdGuild* pcsGuild3 = m_pagpmGuild->GetGuildLock(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pLock3);
		if( pcsGuild3 )
		{
			m_pagpmGuild->InitCurrentBattleInfo(pcsGuild3);
			SendBattleInfoToAllMembers( pcsGuild3 );
		}
		
		m_pagpmGuild->InitCurrentBattleInfo(pcsGuild);
		SendBattleInfoToAllMembers( pcsGuild );
		
		return TRUE;
	}

	if( !m_pagpmGuild->IsEnemyGuild( pcsGuild, szEnemyGuildID ) )
		return FALSE;

	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	if(pcsGuild->m_csCurrentBattleInfo.m_ulStartTime - ulCurrentTime < m_ulBattleCancelEnableTime)
	{
		// ����� �� �ִ� �ð��� ����������
		SendSystemMessage( AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_PASSED_TIME, GetMemberNID(pcsMaster), NULL, NULL, m_ulBattleCancelEnableTime );

		return FALSE;
	}

	// ����� �渶���� ��ҽ�û�� �Ѵ�.
	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID, pLock2);
	if(pcsEnemyGuild)
	{
		AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
		if(pcsEnemyMaster)
		{
			SendBattleCancelRequest(szEnemyGuildID, pcsEnemyMaster->m_szID, szGuildID, GetMemberNID(pcsEnemyMaster));
		}
		else
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_ENEMY_MASTER_OFFLINE, GetMemberNID(pcsMaster));
		}
	}

	return TRUE;
}

/// 2005.04.16. steeple
BOOL AgsmGuild::BattleCancelAccept(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szMasterID || !szEnemyGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// �����Ͱ� �ƴϸ� GG
	AgpdGuildMember* pcsMaster = m_pagpmGuild->GetMaster(pcsGuild);
	if(!pcsMaster || strcmp(pcsMaster->m_szID, szMasterID) != 0)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	if(!m_pagpmGuild->IsEnemyGuild(pcsGuild, szEnemyGuildID))
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID);
	if(!pcsEnemyGuild)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// Battle List Lock
	BattleListLock();
	if( RemoveBattleProcessList( pcsGuild->m_szID, pcsEnemyGuild->m_szID ) )
	{
		// �긦 ���� ���� ������ �Ѵ�.
		SendBattleCancelAcceptToAllMembers( pcsGuild );
		SendBattleCancelAcceptToAllMembers( pcsEnemyGuild );

		// Guild Battle ������ ������.
		SendBattleInfoToAllMembers( pcsGuild );
		SendBattleInfoToAllMembers( pcsEnemyGuild );
	}
	BattleListUnlock();

	// �ʱ�ȭ �ع�����
	m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
	m_pagpmGuild->InitCurrentBattleInfo( pcsEnemyGuild );

	pcsEnemyGuild->m_Mutex.Release();
	pcsGuild->m_Mutex.Release();

	return TRUE;
}

/// 2005.04.16. steeple
BOOL AgsmGuild::BattleCancelReject(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID)
{
	if(!szGuildID || !szMasterID || !szEnemyGuildID)
		return FALSE;

	// ������ �ѹ� Ȯ�����ش�.
	if(!m_pagpmGuild->IsMaster(szGuildID, szMasterID))
		return FALSE;

	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(szEnemyGuildID);
	if(!pcsEnemyGuild)
		return FALSE;

	AgpdGuildMember* pcsEnemyMaster = m_pagpmGuild->GetMaster(pcsEnemyGuild);
	if(pcsEnemyMaster)
	{
		// �޶� �ý��� �޽��� �����ְ� �ƹ��� �����ٴ� �� ������.
		// ��� ��� ��Ʋ ���� ���°� ���ӵǴ� ����.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_BATTLE_CANCEL_REJECT_BY_OTHER, GetMemberNID(pcsEnemyMaster), szGuildID);
	}

	pcsEnemyGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::AddBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2, INT8 cStatus, UINT32 ulAcceptTime, UINT32 ulReadyTime, UINT32 ulStartTime, INT32 lType, UINT32 ulDuration, UINT32 lPerson)
{
	if( !szGuildID1 || !szGuildID2 )		return FALSE;

	BattleListLock();

	// �̹� �ȿ� �ִ�. 
	if( GetBattleProcess( szGuildID1, szGuildID2 ) )
	{
		BattleListUnlock();
		return FALSE;
	}

	AgsdGuildBattleProcess* pcsBattleProcess = (AgsdGuildBattleProcess*)CreateModuleData( AGSMGUILD_DATA_TYPE_BATTLE_PROCESS );
	if(!pcsBattleProcess)
	{
		BattleListUnlock();
		return FALSE;
	}

	pcsBattleProcess->m_eType			= (eGuildBattleType)lType;
	pcsBattleProcess->m_ulPerson		= lPerson;
	pcsBattleProcess->m_cStatus			= cStatus;
	pcsBattleProcess->m_ulLastCheckTime	= 0;
	pcsBattleProcess->m_ulAcceptTime	= ulAcceptTime;
	pcsBattleProcess->m_ulReadyTime		= ulReadyTime;
	pcsBattleProcess->m_ulStartTime		= ulStartTime;
	pcsBattleProcess->m_ulDuration		= ulDuration;
	
	strncpy( pcsBattleProcess->m_szGuildID1, szGuildID1, AGPMGUILD_MAX_GUILD_ID_LENGTH );
	strncpy( pcsBattleProcess->m_szGuildID2, szGuildID2, AGPMGUILD_MAX_GUILD_ID_LENGTH );

	m_listBattleProcess.push_back(pcsBattleProcess);

	BattleListUnlock();

	return TRUE;
}

// 2005.04.17. steeple
// Battle List Lock �Ǿ� �ִ� ���¿��� �Ҹ���.
BOOL AgsmGuild::RemoveBattleProcess(AgsdGuildBattleProcess* pcsGuildBattleProcess)
{
	if( !pcsGuildBattleProcess )		return FALSE;

	return DestroyModuleData(pcsGuildBattleProcess, AGSMGUILD_DATA_TYPE_BATTLE_PROCESS);
}

// 2005.04.17. steeple
// Battle List Lock �Ǿ� �ִ� ���¿��� �Ҹ���.
BOOL AgsmGuild::RemoveBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2)
{
	if( !szGuildID1 || !szGuildID2 )	return FALSE;

	AgsdGuildBattleProcess* pcsBattleProcess = GetBattleProcess(szGuildID1, szGuildID2);
	if( !pcsBattleProcess )		return FALSE;

	return RemoveBattleProcess(pcsBattleProcess);
}

// 2005.04.17. steeple
// Battle List Lock �Ǿ� �ִ� ���¿��� �Ҹ���.
BOOL AgsmGuild::RemoveBattleProcessList(CHAR* szGuildID1, CHAR* szGuildID2)
{
	if( !szGuildID1 || !szGuildID2 )	return FALSE;

	BOOL bRemove = FALSE;

	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); ++Itr )
	{
		if( !strcmp( (*Itr)->m_szGuildID1, szGuildID1) && !strcmp( (*Itr)->m_szGuildID2, szGuildID2) )
		{
			RemoveBattleProcess( *Itr );
			m_listBattleProcess.erase( Itr );
			bRemove = TRUE;
			break;
		}

		if( !strcmp( (*Itr)->m_szGuildID2, szGuildID1 ) && !strcmp( (*Itr)->m_szGuildID1, szGuildID2 ) )
		{
			RemoveBattleProcess( *Itr );
			m_listBattleProcess.erase( Itr );
			bRemove = TRUE;
			break;
		}
	}

	return bRemove;
}

// 2005.04.17. steeple
// Battle List Lock �Ǿ� �ִ� ���¿��� �Ҹ���.
AgsdGuildBattleProcess* AgsmGuild::GetBattleProcess(CHAR* szGuildID1, CHAR* szGuildID2)
{
	if( !szGuildID1 || !szGuildID2 )	return FALSE;

	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); ++Itr )
	{
		if( !strcmp( (*Itr)->m_szGuildID1, szGuildID1 ) && !strcmp( (*Itr)->m_szGuildID2, szGuildID2 ) )
			return (*Itr);

		if( !strcmp( (*Itr)->m_szGuildID2, szGuildID1 ) && !strcmp( (*Itr)->m_szGuildID1, szGuildID2 ) )
			return (*Itr);
	}

	return NULL;

}

// 2005.04.17. steeple
// Idle ���� �ҷ��ش�.
BOOL AgsmGuild::ProcessAllBattle(UINT32 ulClockCount)
{
	if(m_ulLastProcessBattleClock != 0)
	{
		// 1�ʿ� �ѹ��� ����.
		if(ulClockCount - m_ulLastProcessBattleClock < AGSMGUILD_IDLE_BATTLE_PROCESS_INTERVAL)
			return TRUE;
	}

	//STOPWATCH2(GetModuleName(), _T("ProcessAllBattle"));

	m_ulLastProcessBattleClock = ulClockCount;

	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();

	BattleListLock();
	for( GuildBattleListItr Itr = m_listBattleProcess.begin(); Itr != m_listBattleProcess.end(); )
	{
		if( !ProcessBattle( *Itr, ulCurrentTime ) )	
		{
			ClearBattleProcessEachGuild( *Itr );
			RemoveBattleProcess( *Itr );
			Itr = m_listBattleProcess.erase(Itr);
		}
		else
			++Itr;
	}
	BattleListUnlock();

	return TRUE;
}

// 2005.04.17. steeple
// �ð� �뺰 ó���� ���ְ�, �ð��� �� ���� ��쿡�� FALSE �������ش�. �׷��� list ���� ������ �ȴ�.
// BattleProcess �� List �� Lock �Ǿ� �ִ�.
BOOL AgsmGuild::ProcessBattle(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if( !pcsBattleProcess )		return FALSE;

	pcsBattleProcess->m_ulLastCheckTime = ulCurrentTime;

	if( IsBattleStart( pcsBattleProcess, ulCurrentTime ) )
		return BattleStart( pcsBattleProcess, ulCurrentTime );
	else if( IsBattleEnd( pcsBattleProcess, ulCurrentTime ) )
		return BattleEnd( pcsBattleProcess, ulCurrentTime );

	return BattleUpdateTime( pcsBattleProcess, ulCurrentTime );
}

// 2005.06.27. steeple
// ����� �߰��� ��� ���������� �𸣰��... ������� �ȳ����� ������ �δ޸��� �߻��Ͽ���.
// �ڵ带 �ƹ��� ������� Ư���� ������ ��ã�ھ, �׳� Exception ó���� �ϰԲ� ����.
//
// ���⼭ ������ ��Ʋ �߿� �ϳ��� ��尡 ����� ������ �Ǵ��ϰ� �װſ� ���� ó���� ����.
// �׷��� ���࿡ �׷��� ���� �߻��ϴ��� �������� ���� ���ִ� �ڵ���.
//
// �� �Լ��� �θ��� �������� �� �Լ��� �θ���, pcsBattleProcess �� Battle List ���� ����� �Ѵ�. (������ ���ְ� ����)
// 
// BattleProcess �� List �� Lock �Ǿ� �ִ�.
BOOL AgsmGuild::ProcessBattleException(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessBattleException"));

	switch(pcsBattleProcess->m_cStatus)
	{
		case AGPMGUILD_STATUS_BATTLE_READY:
		{
			// �� ���¿����� �׳� �����ش�.
			AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
			if(pcsGuild)
			{
				SendBattleCancelAcceptToAllMembers( pcsGuild );
				m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
				SendBattleInfoToAllMembers( pcsGuild );
				pcsGuild->m_Mutex.Release();
			}

			pcsGuild = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
			if( pcsGuild )
			{
				SendBattleCancelAcceptToAllMembers( pcsGuild );
				m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
				SendBattleInfoToAllMembers( pcsGuild );
				pcsGuild->m_Mutex.Release();
			}

			break;
		}

		case AGPMGUILD_STATUS_BATTLE:
		{
			// ���⼭�� ���ڿ� ���ڸ� ��������� ��.
			AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
			AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
			
			AgpdGuild* pcsWinner = NULL, *pcsLoser = NULL;
			if(pcsGuild1 && !pcsGuild2)
			{
				pcsWinner = pcsGuild1;
				pcsLoser = pcsGuild2;
			}
			else if(!pcsGuild1 && pcsGuild2)
			{
				pcsWinner = pcsGuild2;
				pcsLoser = pcsGuild1;
			}
			else if(!pcsGuild1 && !pcsGuild2)
				return TRUE;

			if(pcsWinner)
			{
				pcsWinner->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW;
				pcsWinner->m_lWin++;
				SendBattleEndToAllMembers(pcsWinner);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsWinner, NULL);	// �ڿ��ٰ� NULL �൵ �ȴ�. 2005.06.27. ����
				RemoveEnemyGuildToAllMembers(pcsWinner, pcsWinner->m_csCurrentBattleInfo.m_szEnemyGuildID);
				m_pagpmGuild->InitCurrentBattleInfo(pcsWinner);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsWinner, NULL);
				SendBattleInfoToAllMembers(pcsWinner);
			}

			if(pcsLoser)
			{
				pcsLoser->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW;
				pcsLoser->m_lLose++;
				SendBattleEndToAllMembers(pcsLoser);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsLoser, NULL);	// �ڿ��ٰ� NULL �൵ �ȴ�. 2005.06.27. ����
				RemoveEnemyGuildToAllMembers(pcsLoser, pcsLoser->m_csCurrentBattleInfo.m_szEnemyGuildID);
				m_pagpmGuild->InitCurrentBattleInfo(pcsLoser);
				EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsLoser, NULL);
				SendBattleInfoToAllMembers(pcsLoser);
			}

			if( pcsGuild1 )
				pcsGuild1->m_Mutex.Release();
			if( pcsGuild2 )
				pcsGuild2->m_Mutex.Release();

			break;
		}
	}

	return TRUE;
}

BOOL AgsmGuild::IsBattleStart(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if( pcsBattleProcess->m_cStatus == AGPMGUILD_STATUS_BATTLE_READY && ulCurrentTime > pcsBattleProcess->m_ulStartTime )
		return TRUE;

	return FALSE;
}

//��Ÿ�Ժ��� ������ �ǹ̰� �ٸ���..
BOOL AgsmGuild::IsBattleEnd(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if( !pcsBattleProcess )		return FALSE;

	switch( pcsBattleProcess->m_eType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleTotalSurvive:
	case eGuildBattleDeadMatch:
		if( AGPMGUILD_STATUS_BATTLE == pcsBattleProcess->m_cStatus && ulCurrentTime > pcsBattleProcess->m_ulStartTime + pcsBattleProcess->m_ulDuration )
			return TRUE;
		break;
	case eGuildBattlePrivateSurvive:
		{
			if( AGPMGUILD_STATUS_BATTLE != pcsBattleProcess->m_cStatus )
				return FALSE;
				
			if( ulCurrentTime > pcsBattleProcess->m_ulStartTime + pcsBattleProcess->m_ulDuration )
				return TRUE;

			AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
			if( pcsGuild1 )
			{
				/*UINT32 ulHalfPerson = (UINT32)((float)pcsBattleProcess->m_ulPerson * 0.5f);
				if( pcsGuild1->GetKillAll() > ulHalfPerson || pcsGuild1->GetDeathAll() > ulHalfPerson )
					return TRUE;*/
				
				if( pcsBattleProcess->m_ulPerson <= pcsGuild1->m_csCurrentBattleInfo.m_ulRound )	//������ ���ڰ� Ŀ����..
					return TRUE;
			}
		} break;
	}
	
	return FALSE;
}

// 2005.04.17. steeple
// Battle ���� �ð��� �Ǿ���. ����������!!
BOOL AgsmGuild::BattleStart(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	// ���� ��带 ���Ѵ�.
	AuAutoLock pLock1;
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1, pLock1);
	if(!pcsGuild1)
	{
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	AuAutoLock pLock2;
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2, pLock2);
	if(!pcsGuild2)
	{
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_BATTLE;
	
	pcsGuild1->BattleStart( ulCurrentTime );
	pcsGuild2->BattleStart( ulCurrentTime );
	
	AddEnemyGuildToAllMembers(pcsGuild1, pcsGuild2->m_szID);
	AddEnemyGuildToAllMembers(pcsGuild2, pcsGuild1->m_szID);

	//�������� �������� ��������..
	SendBattleMemberListInfoAllMembers(pcsGuild1, pcsGuild2);
	SendBattleMemberListInfoAllMembers(pcsGuild2, pcsGuild1);

	SendBattleStartToAllMembers(pcsGuild1);
	SendBattleStartToAllMembers(pcsGuild2);
	
	if(pcsGuild1->m_csCurrentBattleInfo.m_eType == eGuildBattlePrivateSurvive)
		BattleUpdatePrivateMode( pcsGuild1, pcsGuild2 );

	// DB ����
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild1, NULL);
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild2, NULL);

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::BattleEnd(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("BattleEnd"));

	// ���� ��带 ���Ѵ�.
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
	if(!pcsGuild1)
	{
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
	if(!pcsGuild2)
	{
		pcsGuild1->m_Mutex.Release();
		ProcessBattleException(pcsBattleProcess, ulCurrentTime);
		return FALSE;
	}

	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_NONE;
	pcsGuild1->BattleEnd( ulCurrentTime );
	pcsGuild2->BattleEnd( ulCurrentTime );
	pcsGuild1->BattleResult( pcsGuild2 );

	AgpdGuild* pcsWinGuild = NULL;
	AgpdGuild* pcsLoseGuild = NULL;
	switch( pcsGuild1->m_csCurrentBattleInfo.m_cResult )
	{
	case AGPMGUILD_BATTLE_RESULT_WIN:
		pcsWinGuild		= pcsGuild1;
		pcsLoseGuild	= pcsGuild2;
		break;
	case AGPMGUILD_BATTLE_RESULT_LOSE:
		pcsWinGuild		= pcsGuild2;
		pcsLoseGuild	= pcsGuild1;
		break;
	case AGPMGUILD_BATTLE_RESULT_DRAW:
		pcsWinGuild		= pcsGuild1;
		pcsLoseGuild	= pcsGuild2;
		break;
	}
	
	BattleEndProcess(pcsWinGuild, pcsLoseGuild);
	
	pcsGuild2->m_Mutex.Release();
	pcsGuild1->m_Mutex.Release();

	return FALSE;
}

BOOL AgsmGuild::BattleEndProcess(AgpdGuild* pcsWinGuild, AgpdGuild* pcsLoseGuild)
{
	if(!pcsWinGuild || !pcsLoseGuild)
		return FALSE;
		
	INT32 myUpPoint		= pcsWinGuild->m_csCurrentBattleInfo.GetGuildPoint();
	INT32 enemyUpPoint	= pcsLoseGuild->m_csCurrentBattleInfo.GetGuildPoint();

	pcsWinGuild->m_csCurrentBattleInfo.m_lEnemyUpPoint	= enemyUpPoint;
	pcsLoseGuild->m_csCurrentBattleInfo.m_lEnemyUpPoint	= myUpPoint;

	pcsWinGuild->m_lGuildPoint		+= myUpPoint;
	pcsLoseGuild->m_lGuildPoint		+= enemyUpPoint;
	
	// ó���ϴ� ������ �߿��ϴ�. 
	// DB ���� - History �� ���� �����ϰ� - �̱���� pData �� �ְ� �� ���� pCustData �� �ִ´�. ��� ���� �������.
	// ���� �������ε� �ѹ��� �����Ѵ�.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsWinGuild, pcsLoseGuild);
	EnumCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pcsLoseGuild, pcsWinGuild);

	SendBattleMemberListInfoAllMembers( pcsWinGuild, pcsLoseGuild );
	SendBattleMemberListInfoAllMembers( pcsLoseGuild, pcsWinGuild );

	SendBattleEndToAllMembers( pcsWinGuild );
	SendBattleEndToAllMembers( pcsLoseGuild );
	
	RemoveEnemyGuildToAllMembers( pcsWinGuild, pcsLoseGuild->m_szID );
	RemoveEnemyGuildToAllMembers( pcsLoseGuild, pcsWinGuild->m_szID );

	// ��� ���� PvP ������ EnemyGuild �� ���ش�.
	/*switch( pcsWinGuild->m_csCurrentBattleInfo.m_eType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		RemoveEnemyGuildToAllMembers( pcsWinGuild, pcsLoseGuild->m_szID );
		RemoveEnemyGuildToAllMembers( pcsLoseGuild, pcsWinGuild->m_szID );
		break;
	case eGuildBattleTotalSurvive:
		RemoveEnemyGuildToBattleAllMembers( pcsWinGuild, pcsLoseGuild );
		RemoveEnemyGuildToBattleAllMembers( pcsLoseGuild, pcsWinGuild );
		break;
	case eGuildBattlePrivateSurvive:
		break;
	}*/

	// �����͸� ����
	m_pagpmGuild->InitCurrentBattleInfo( pcsWinGuild );
	m_pagpmGuild->InitCurrentBattleInfo( pcsLoseGuild );
	
	// DB ����
	EnumCallback( AGSMGUILD_CB_DB_GUILD_UPDATE, pcsWinGuild, NULL );
	EnumCallback( AGSMGUILD_CB_DB_GUILD_UPDATE, pcsLoseGuild, NULL );

	// Guild Battle ������ �ѹ� �� �� ������.
	SendBattleInfoToAllMembers( pcsWinGuild );
	SendBattleInfoToAllMembers( pcsLoseGuild );
	
	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::BattleUpdateTime(AgsdGuildBattleProcess* pcsBattleProcess, UINT32 ulCurrentTime)
{
	if(!pcsBattleProcess)
		return FALSE;

	// ���� ��带 ���Ѵ�.
	AuAutoLock pLock1;
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1, pLock1);
	if(!pcsGuild1)
		return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2, pLock2);
	if(!pcsGuild2)
		return FALSE;

	pcsGuild1->BattleUpdate( ulCurrentTime );
	pcsGuild2->BattleUpdate( ulCurrentTime );

	SendBattleUpdateTimeToAllMembers(pcsGuild1);
	SendBattleUpdateTimeToAllMembers(pcsGuild2);

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::ClearBattleProcessEachGuild(AgsdGuildBattleProcess* pcsBattleProcess)
{
	if(!pcsBattleProcess)
		return FALSE;

	// ���� ��带 ���Ѵ�.
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID1);
	if(!pcsGuild1)
		return FALSE;

	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(pcsBattleProcess->m_szGuildID2);
	if(!pcsGuild2)
	{
		pcsGuild1->m_Mutex.Release();
		return FALSE;
	}

	BOOL bIsNeedUpdateDBGuild1 = strlen(pcsGuild1->m_csCurrentBattleInfo.m_szEnemyGuildID) > 0 ? TRUE : FALSE;
	BOOL bIsNeedUpdateDBGuild2 = strlen(pcsGuild2->m_csCurrentBattleInfo.m_szEnemyGuildID) > 0 ? TRUE : FALSE;

	m_pagpmGuild->InitCurrentBattleInfo(pcsGuild1);
	m_pagpmGuild->InitCurrentBattleInfo(pcsGuild2);

	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_NONE;

	SendBattleInfoToAllMembers(pcsGuild1);
	SendBattleInfoToAllMembers(pcsGuild2);

	// DB ����
	if( bIsNeedUpdateDBGuild1 )		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild1, NULL);
	if( bIsNeedUpdateDBGuild2 )		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild2, NULL);
		
	pcsGuild2->m_Mutex.Release();
	pcsGuild1->m_Mutex.Release();

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::AddEnemyGuildToAllMembers(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID)
{
	if(!pcsGuild || !szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		//if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;

		pcsCharacter = m_pagpmCharacter->GetCharacterLock((*ppcsMember)->m_szID);
		if(pcsCharacter)
		{
			// AgsmPvP �� ���� EnemyGuildID �� �� ���� �߰��ϰ� �ȴ�.
			EnumCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_BATTLE, pcsCharacter, szEnemyGuildID);
			pcsCharacter->m_Mutex.Release();
		}
	}

	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::RemoveEnemyGuildToAllMembers(AgpdGuild* pcsGuild, CHAR* szEnemyGuildID)
{
	if(!pcsGuild || !szEnemyGuildID)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		//if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;	

		pcsCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember)->m_szID );
		if( pcsCharacter )
		{
			EnumCallback( AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_BATTLE, pcsCharacter, szEnemyGuildID );
			pcsCharacter->m_Mutex.Release();
		}
	}

	return TRUE;
}

BOOL AgsmGuild::AddEnemyGuildToBattleAllMembers(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild)
{
	if( !pcsGuild || !pcsEnemyGuild )	return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	AgpdGuildMember** ppcsMember2 = NULL;
	INT32 lIndex2 = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;	

		pcsCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember)->m_szID );
		if( !pcsCharacter )	continue;

		//������� �ο�����. �� �������ڲٳ� ^^
		lIndex2 = 0;
		for( ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2);
			 ppcsMember2;
			 ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2))
		{
			if( !pcsEnemyGuild->IsBattle( *ppcsMember2 ) )	continue;

			AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember2)->m_szID );
			if( !pcsEnemyCharacter )						continue;

			EnumCallback( AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
			pcsEnemyCharacter->m_Mutex.Release();
		}

		pcsCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmGuild::RemoveEnemyGuildToBattleAllMembers(AgpdGuild* pcsGuild, AgpdGuild* pcsEnemyGuild)
{
	if( !pcsGuild || !pcsEnemyGuild )	return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	AgpdGuildMember** ppcsMember2 = NULL;
	INT32 lIndex2 = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !pcsGuild->IsBattle( *ppcsMember ) )	continue;	

		pcsCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember)->m_szID );
		if( !pcsCharacter )	continue;

		//������� �ο�����. �� �������ڲٳ� ^^
		lIndex2 = 0;
		for( ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2);
			 ppcsMember2;
			 ppcsMember2 = (AgpdGuildMember**)pcsEnemyGuild->m_pMemberList->GetObjectSequence(&lIndex2))
		{
			if( !pcsEnemyGuild->IsBattle( *ppcsMember2 ) )	continue;

			AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( (*ppcsMember2)->m_szID );
			if( !pcsEnemyCharacter )						continue;

			EnumCallback( AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
			pcsEnemyCharacter->m_Mutex.Release();
		}

		pcsCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmGuild::AddEnemyGuildToBattleMembers(CHAR* szMyID, CHAR* szEnemyID)
{
	if( !szMyID || !szEnemyID )	return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock( szMyID );
	if( !pcsCharacter )			return FALSE;

	AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( szEnemyID );
	if( pcsEnemyCharacter )
	{
		EnumCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
		pcsEnemyCharacter->m_Mutex.Release();
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::RemoveEnemyGuildToBattleMembers(CHAR* szMyID, CHAR* szEnemyID)
{
	if( !szMyID || !szEnemyID )	return FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock( szMyID );
	if( !pcsCharacter )			return FALSE;

	AgpdCharacter* pcsEnemyCharacter = m_pagpmCharacter->GetCharacterLock( szEnemyID );
	if( pcsEnemyCharacter )
	{
		EnumCallback(AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_SURVIVE_BATTLE, pcsCharacter, pcsEnemyCharacter );
		pcsEnemyCharacter->m_Mutex.Release();
	}
	
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmGuild::BattleUpdatePrivateMode(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if( !pcsGuild1 || !pcsGuild2 )		return FALSE;
	if( eGuildBattlePrivateSurvive != pcsGuild1->m_csCurrentBattleInfo.m_eType )
		return FALSE;

	while( 1 )
	{
		++pcsGuild1->m_csCurrentBattleInfo.m_ulRound;
		++pcsGuild2->m_csCurrentBattleInfo.m_ulRound;

		//��ü ������ڿ�.. ���� ���ǵ� �� ���ں�..

		AgpdGuildMember* pMember1 = pcsGuild1->GetCurRoundMember();
		AgpdGuildMember* pMember2 = pcsGuild2->GetCurRoundMember();
		if( pMember1 && pMember2 )
		{
			AgpdCharacter* pcsCharacter1 = m_pagpmCharacter->GetCharacter(pMember1->m_szID);
			AgpdCharacter* pcsCharacter2 = m_pagpmCharacter->GetCharacter(pMember2->m_szID);
			
			if( pcsCharacter1 && pcsCharacter2 )
			{
				SendBattleRoundToAllMembers(pcsGuild1);
				SendBattleRoundToAllMembers(pcsGuild2);
				return TRUE;
			}
			
			if( pcsCharacter1 && !pcsCharacter2 )
			{
				CalcBattleScoreProcess(pcsGuild1, pcsCharacter1, pcsGuild2, pcsCharacter2);
			}
			else if( !pcsCharacter1 && pcsCharacter2 )
			{
				CalcBattleScoreProcess(pcsGuild2, pcsCharacter2, pcsGuild1, pcsCharacter1);
			}
		}
		else
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmGuild::CalcBattleScoreProcess(AgpdGuild* pcsGuild1, AgpdCharacter* pcsWinner, AgpdGuild* pcsGuild2, AgpdCharacter* pcsLoser)
{
	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;
		
	INT32 lUpScore = 1;
	if( pcsWinner )
	{
		if(pcsGuild1->m_csCurrentBattleInfo.m_eType == eGuildBattlePointMatch)
		{
			INT32 lAttackerLevel	= m_pagpmCharacter->GetLevel(pcsWinner);
			INT32 lDeadLevel		= m_pagpmCharacter->GetLevel(pcsLoser);

			// ���� ���̴� ���𿡼� �̱���� ���� �ش�.	
			lUpScore = m_pagpmGuild->GetBattlePoint(lDeadLevel - lAttackerLevel);
		}

		AgpdGuildMember* pGuildAttack = pcsGuild1->GetMember( pcsWinner->m_szID );
		if( pGuildAttack )
		{
			pGuildAttack->m_ulKill++;
			pGuildAttack->m_ulScore += lUpScore;

			//�츮��� ���濡�� Update�� ������ ������ ^^
			SendBattleMemberInfoAllMembers(pcsGuild1, pcsGuild1->m_szID, pGuildAttack );
			SendBattleMemberInfoAllMembers(pcsGuild2, pcsGuild1->m_szID, pGuildAttack );
		}
	}
	
	if( pcsLoser )
	{
		AgpdGuildMember* pGuildDead = pcsGuild2->GetMember( pcsLoser->m_szID );
		if( pGuildDead )
			pGuildDead->m_ulDeath++;	
	}

	pcsGuild1->m_csCurrentBattleInfo.m_lMyScore			+= lUpScore;
	pcsGuild1->m_csCurrentBattleInfo.m_lMyUpScore		= lUpScore;

	pcsGuild2->m_csCurrentBattleInfo.m_lEnemyScore		+= lUpScore;
	pcsGuild2->m_csCurrentBattleInfo.m_lEnemyUpScore	= lUpScore;

	// ������.
	SendBattleUpdateScoreToAllMembers(pcsGuild1);
	SendBattleUpdateScoreToAllMembers(pcsGuild2);

	pcsGuild1->m_csCurrentBattleInfo.m_lMyUpScore		= 0;
	pcsGuild2->m_csCurrentBattleInfo.m_lEnemyUpScore	= 0;

	// ���� �ö� ������ DB �� ��� �Ѵ�.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild1, NULL);
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild2, NULL);
	
	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::CalcBattleScore(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsDead)
{
	if(!pcsAttacker || !pcsDead)
		return FALSE;

	CHAR* szGuildID1 = m_pagpmGuild->GetJoinedGuildID(pcsAttacker);
	CHAR* szGuildID2 = m_pagpmGuild->GetJoinedGuildID(pcsDead);
	if(!szGuildID1 || !szGuildID2)
		return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuildLock(szGuildID1, pLock1);
	if(!pcsGuild1)
		return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuildLock(szGuildID2, pLock2);
	if(!pcsGuild2)
		return FALSE;

	// ���� ���̰�, ���°� BATTLE �� �϶��� �ϸ� �ȴ�.
	if(!m_pagpmGuild->IsBattleIngStatus(pcsGuild1) || !m_pagpmGuild->IsBattleIngStatus(pcsGuild2) ||
		strcmp(pcsGuild1->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild2->m_szID) != 0 ||
		strcmp(pcsGuild2->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild1->m_szID) != 0)
		return FALSE;
		
	if( !pcsGuild1->IsBattle( pcsAttacker->m_szID ) || !pcsGuild2->IsBattle( pcsDead->m_szID ) )
		return FALSE;

	if( !pcsGuild1->IsPrivateSurvivePlayer( pcsGuild2, pcsAttacker->m_szID, pcsDead->m_szID ) )
		return FALSE;
		
	// ������������ ����� �� ���� �ȵ�
	/*if( m_pagpmSiegeWar->GetSiegeWarInfo(pcsAttacker) || m_pagpmSiegeWar->GetSiegeWarInfo(pcsDead) )
		return FALSE;*/
	
	// �������� ����� �� ���� �ȵ�	
	/*if( m_pagpmCharacter->IsInDungeon(pcsAttacker) || m_pagpmCharacter->IsInDungeon(pcsDead) )
		return FALSE;*/
		
	if( pcsGuild1->m_csCurrentBattleInfo.m_eType == eGuildBattleDeadMatch ) //�渶 ���� ���̱�
	{
		if (!m_pagpmGuild->IsMaster(pcsGuild2, pcsDead->m_szID))
			return TRUE;	// ����� �½��ϴ�. ������ ������ ���ݴϴ�.
	}
	
	// ���� ���
	CalcBattleScoreProcess(pcsGuild1, pcsAttacker, pcsGuild2, pcsDead);
	
	if( pcsGuild1->IsOnBattle(pcsAttacker->m_szID) && pcsGuild2->IsOnBattle(pcsDead->m_szID) )
		BattleUpdatePrivateMode( pcsGuild1, pcsGuild2 );
	
	return TRUE;
}

// 2005.04.17. steeple
BOOL AgsmGuild::CalcBattleScoreByDisconnect(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsMostDamager)
{
	if( !pcsCharacter )		return FALSE;

	CHAR* szGuildID = m_pagpmGuild->GetJoinedGuildID(pcsCharacter);
	if( !szGuildID )		return FALSE;

	AuAutoLock pLock1;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID, pLock1);
	if( !pcsGuild )			return FALSE;

	AuAutoLock pLock2;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pLock2);
	if(!pcsEnemyGuild)		return FALSE;
	
	if(m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
	{
		if(m_pagpmGuild->IsBattleDeclareStatus(pcsGuild))
		{
			BattleCancelAccept(pcsGuild->m_szID, pcsGuild->m_szMasterID, pcsEnemyGuild->m_szID);
			return TRUE;
		}
		
		if(m_pagpmGuild->IsBattleReadyStatus(pcsGuild) || m_pagpmGuild->IsBattleIngStatus(pcsGuild))
		{
			if( pcsGuild->m_csCurrentBattleInfo.m_eType == eGuildBattleDeadMatch ) //�渶 ���� ���̱⿡�� �渶�� ������
			{
				BattleWithdraw(pcsGuild, pcsCharacter);
				return TRUE;
			}
		}
	}
	else
	{
		if(m_pagpmGuild->IsBattleIngStatus(pcsGuild))
		{
			if( pcsGuild->m_csCurrentBattleInfo.m_eType == eGuildBattleDeadMatch )
			{
				return TRUE;
			}
		}
	}

	// ��Ʋ���� �ƴϸ� ������.
	if(!pcsGuild->IsOnBattle( pcsCharacter->m_szID ))
		return FALSE;
	
	CalcBattleScoreProcess(pcsEnemyGuild, pcsMostDamager, pcsGuild, pcsCharacter);
	
	BattleUpdatePrivateMode( pcsGuild, pcsEnemyGuild );
	
	return TRUE;
}

// 2005.04.17. steeple
// pcsGuild �� pcsCharacter �� Lock �Ǿ �Ѿ�´�.
BOOL AgsmGuild::BattleWithdraw(AgpdGuild* pcsGuild, AgpdCharacter* pcsCharacter)
{
	if(!pcsGuild || !pcsCharacter)
		return FALSE;

	return BattleWithdraw(pcsGuild, pcsCharacter->m_szID);
}

// 2005.04.21. steeple
// pcsGuild �� Lock �Ǽ� �Ѿ�´�.
BOOL AgsmGuild::BattleWithdraw(AgpdGuild* pcsGuild, CHAR* szMasterID)
{
	if(!pcsGuild || !szMasterID)
		return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild, szMasterID))
		return FALSE;

	// ���� ��尡 ������ �̱�ɷ� �ؼ� ������.
	AuAutoLock pLock;
	AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pLock);
	if(!pcsEnemyGuild)
		return FALSE;
		
	AgsdGuildBattleProcess* pcsBattleProcess = GetBattleProcess(pcsGuild->m_szID, pcsEnemyGuild->m_szID);
	if(!pcsBattleProcess) return FALSE;
	
	UINT32 ulCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	
	pcsBattleProcess->m_cStatus = AGPMGUILD_STATUS_NONE;
	pcsGuild->BattleEnd( ulCurrentTime );
	pcsEnemyGuild->BattleEnd( ulCurrentTime );

	pcsGuild->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW;
	pcsEnemyGuild->m_csCurrentBattleInfo.m_cResult = AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW;

	pcsEnemyGuild->m_lWin++;
	pcsGuild->m_lLose++;
	
	BattleEndProcess(pcsEnemyGuild, pcsGuild); // Enemy Win...

	// Battle List Lock
	BattleListLock();
	RemoveBattleProcessList(pcsGuild->m_szID, pcsEnemyGuild->m_szID);
	BattleListUnlock();

	return TRUE;
}
















// DB ���� Load �Ѵ�.
// �� �Լ��� �Ҹ��� �����ϰ� �ȴ�.
BOOL AgsmGuild::LoadAllGuildInfo()
{
	// ��ϵ� Callback �� �ҷ��ָ� �ȴ�.
	// Guild ������ �켱 ��û�ϸ� Relay ���� ��带 �� �����ְ�, ������� �����ְ� �ȴ�.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_SELECT, NULL, NULL);
	//EnumCallback(AGSMGUILD_CB_DB_MEMBER_SELECT, NULL, NULL);

	return TRUE;
}

// �ε�Ǿ� �ִ� ��� Guild Info �� �����.
BOOL AgsmGuild::ClearAllGuildInfo()
{
	if(m_pagpmGuild)
		m_pagpmGuild->OnDestroy();

	return TRUE;
}

// DB ���� Guild Data �� Load �ߴ�.
// ����� AgsmRelay ���� ���� �ҷ��ش�.
BOOL AgsmGuild::GuildLoadFromDB(AgsdRelay2GuildMaster* pAgsdRelay)
{
	if(!pAgsdRelay)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->CreateGuild(pAgsdRelay->m_szGuildID, pAgsdRelay->m_lGuildTID, pAgsdRelay->m_lGuildRank, pAgsdRelay->m_lMaxMemberCount, pAgsdRelay->m_lUnionID);
	if(pcsGuild)
	{
#ifdef	_DEBUG
		//printf("\n[AgsmGuild] GuildLoad :: GuildID:%s, MasterID:%s, Password:%s", szGuildID, szMasterID, szPassword);
#endif

		ZeroMemory(pcsGuild->m_szMasterID, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
		strncpy(pcsGuild->m_szMasterID, pAgsdRelay->m_szMasterID, AGPACHARACTER_MAX_ID_STRING);
		
		pcsGuild->m_lCreationDate	= pAgsdRelay->m_lCreationDate;
		pcsGuild->m_lUnionID		= pAgsdRelay->m_lUnionID;

		ZeroMemory( pcsGuild->m_szPassword, sizeof(CHAR) * (AGPMGUILD_MAX_PASSWORD_LENGTH + 1) );
		strncpy( pcsGuild->m_szPassword, pAgsdRelay->m_szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH );

		pcsGuild->m_cStatus			= pAgsdRelay->m_cStatus;
		pcsGuild->m_lWin			= pAgsdRelay->m_lWin;
		pcsGuild->m_lDraw			= pAgsdRelay->m_lDraw;
		pcsGuild->m_lLose			= pAgsdRelay->m_lLose;
		pcsGuild->m_lGuildMarkTID	= pAgsdRelay->m_lGuildMarkTID;
		pcsGuild->m_lGuildMarkColor	= pAgsdRelay->m_lGuildMarkColor;
		pcsGuild->m_lGuildPoint		= pAgsdRelay->m_lGuildPoint;		// 2007.10.17 arycoat

		if(pAgsdRelay->m_szEnemyGuildID)
			strncpy(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pAgsdRelay->m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
		if(pAgsdRelay->m_szBattleStartTime)
			pcsGuild->m_csCurrentBattleInfo.m_ulStartTime = AuTimeStamp::ConvertOracleTimeToTimeStamp(pAgsdRelay->m_szBattleStartTime);

		pcsGuild->m_csCurrentBattleInfo.m_ulDuration	= pAgsdRelay->m_ulBattleDuration;
		pcsGuild->m_csCurrentBattleInfo.m_lMyScore		= pAgsdRelay->m_lMyScore;
		//pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore = lEnemyScore;

		// Enemy Guild �� ���´�.
		AgpdGuild* pcsEnemyGuild = m_pagpmGuild->GetGuildLock(pAgsdRelay->m_szEnemyGuildID);
		if(pcsEnemyGuild)
		{
			// ���� �� ������ �־��ش�. ������~
			pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore = pcsEnemyGuild->m_csCurrentBattleInfo.m_lMyScore;
			pcsEnemyGuild->m_csCurrentBattleInfo.m_lEnemyScore = pcsGuild->m_csCurrentBattleInfo.m_lMyScore;

			pcsEnemyGuild->m_Mutex.Release();
		}

		//[KTH] ���� ������ �׾��µ� ������� �ٽ� �������� �ʿ�� ������..
		// Status �� Battle �̰�, Battle Process �� ������ ���� �߰����ش�.
		if(m_pagpmGuild->IsBattleIngStatus(pcsGuild))
		{
			BattleListLock();
			if(!GetBattleProcess(pcsGuild->m_szID, pAgsdRelay->m_szEnemyGuildID))
			{
				// �� ���¿����� �׳� �����ش�.
				if(pcsGuild)
				{
					m_pagpmGuild->InitCurrentBattleInfo( pcsGuild );
					// DB ����
					EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);
				}

				if( pcsEnemyGuild )
				{
					m_pagpmGuild->InitCurrentBattleInfo( pcsEnemyGuild );
					// DB ����
					EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsEnemyGuild, NULL);
				}
			}
			BattleListUnlock();
		}

		// 2005.06.09. steeple
		// ��� �ο��� 50������ �������ش�.
		// ��� �ο��� 50���� ������ �����ϹǷ� ó���Ѵ�.
/*		INT32 lOldMaxMemberCount = pcsGuild->m_lMaxMemberCount;
		if(lOldMaxMemberCount != AGSMGUILD_MAX_MEMBER_COUNT)
		{
			UpdateMaxMemberCount(pcsGuild, AGSMGUILD_MAX_MEMBER_COUNT);
		}
*/
		pcsGuild->m_Mutex.Release();
		
		PVOID pvBuffer[2];
		pvBuffer[0] = &pAgsdRelay->m_llMoney;
		pvBuffer[1] = &pAgsdRelay->m_lSlot;
		// ���߿� ���� �����κе� �Ϸ� ���� �ȴ�.
		
		EnumCallback(AGSMGUILD_CB_GUILD_LOAD, pcsGuild, pvBuffer);
	}

	return TRUE;
}

// DB ���� Member Data �� Load �ߴ�.
// ����� AgsmRelay ���� ���� �ҷ��ش�.
BOOL AgsmGuild::MemberLoadFromDB(CHAR* szMemberID, CHAR* szGuildID, INT32 lRank, INT32 lJoinDate, INT32 lLevel, INT32 lTID)
{
	if(!szMemberID || !szGuildID)
		return FALSE;

	// 2005.04.27. steeple
	// ����� �� �ε��ؼ� ���ƿ°��� Ȯ���ؾ� �Ѵ�.
	if(strcmp(szMemberID, AGSMGUILD_LOAD_COMPLETE_STRING) == 0)
	{
		if(m_hEventGuildLoadComplete)
		{
			m_pagpmGuild->m_csGuildAdmin.SortGuildMemberDesc( FALSE );

			SetEvent(m_hEventGuildLoadComplete);

			printf("\n\n[AgsmGuild] Guild Loading Complete !!!!!\n\n");

			// ��� ������ ��δ� ������ �����̹Ƿ� ����� ��带 ���Ϸ� ���� �о�鿩 �����Ѵ�.
			LoadWinnerGuild();
		}

		return TRUE;
	}

//	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);		//	2005.08.03. Delete By SungHoon
	if (lRank == AGPMGUILD_MEMBER_RANK_JOIN_REQUEST)
	{
		RequestJoinMemberSelf(szGuildID, szMemberID, lRank, lJoinDate, lLevel, lTID);
		return TRUE;
	}

	if(m_pagpmGuild->JoinMember(szGuildID, szMemberID, lRank, lJoinDate, lLevel, lTID, AGPMGUILD_MEMBER_STATUS_OFFLINE))
	{
		//printf("\n[AgsmGuild] MemberLoad :: GuildID:%s, MemberID:%s, Rank:%d, Level:%d", szGuildID, szMemberID, lRank, lLevel);
	}
	//	����Ż���� ���̶�� Ż���⸮��Ʈ���� �߰��Ѵ�.	2005.08.22. By SungHoon
	if (lRank == AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST)
		InsertLeaveMemberSelf(szGuildID, szMemberID, lJoinDate);

//	pcsGuild->m_Mutex.Release();	//	2005.08.03. Delete By SungHoon
	return TRUE;
}

// 2005.01.24. steeple
// DB ���� �ش� GuildID �� ��带 ����� �ִ� �� ���Ϲ޴´�.
BOOL AgsmGuild::GuildIDCheckFromDB(BOOL bResult, CHAR* szGuildID, CHAR* szMasterID, CHAR* szPassword)
{
	CHAR szMessage[255];
	sprintf(szMessage, "AgsmGuild::GuildIDCheckFromDB | bResult:%d, szGuildID:%s, szMasterID:%s, szPassword:%s\n", bResult, szGuildID, szMasterID, szPassword);
	m_pagpmGuild->WriteGuildLog(szMessage);

	if(bResult)	// ���� �� ����
	{
		m_pagpmGuild->OnOperationCreate(1, szGuildID, szMasterID, -1, -1, -1, -1, -1, szPassword, NULL, 0, -1, -1, -1, -1, -1, -1, FALSE);
	}
	else	// ���� �� ����
	{
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(szMasterID);
		if(pcsCharacter)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_EXIST_GUILD_ID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsCharacter->m_Mutex.Release();

		}
	}

	return TRUE;
}

// 2006.08.11. steeple
// Relation Guild �ε�.
BOOL AgsmGuild::RelationLoadFromDB(CHAR* szGuildID, CHAR* szRelationGuildID, INT8 cRelation, UINT32 ulDate)
{
	if(!szGuildID || !szRelationGuildID)
		return FALSE;
	
    AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szRelationGuildID);
	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	// cRelation ���� pcsGuild2 �� pcsGuild1 �� ���� ���̴�.
	switch((AgpmGuildRelation)cRelation)
	{
		case AGPMGUILD_RELATION_JOINT:
			m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			//m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			//m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			//m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			break;

		case AGPMGUILD_RELATION_JOINT_LEADER:
			// pcsGuild2 �� LEADER �̴�.
			m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
			m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
			m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
			break;

		case AGPMGUILD_RELATION_HOSTILE:
			m_pagpmGuild->AddHostileGuild(pcsGuild1, pcsGuild2->m_szID, ulDate);
			m_pagpmGuild->AddHostileGuild(pcsGuild2, pcsGuild1->m_szID, ulDate);
			break;
	}

	return TRUE;
}

// 2005.03.25. steeple
BOOL AgsmGuild::GetSelectQueryCharGuildID(CHAR* szQuery, INT32 lQueryLength)
{
	if(!szQuery)
		return FALSE;

	sprintf(szQuery, "SELECT guildid FROM guildmember WHERE charid=:1");

	if((INT32)strlen(szQuery) >= lQueryLength)
	{
		ASSERT(!"AgsmGuild::GetSelectQueryCharGuildID(CHAR* szQuery, INT32 lQueryLength) | szQuery >= lQueryLength");
		return FALSE;
	}

	return TRUE;
}

// 2005.03.25. steeple
BOOL AgsmGuild::GetSelectQueryCharGuildID(CHAR* szCharID, CHAR* szWorldDBName, CHAR* szQuery, INT32 lQueryLength)
{
	if(!szCharID || !szWorldDBName || !szQuery)
		return FALSE;

	sprintf(szQuery, "SELECT guildid FROM %s.guildmember WHERE charid='%s'", szWorldDBName, szCharID);

	if((INT32)strlen(szQuery) >= lQueryLength)
	{
		ASSERT(!"AgsmGuild::GetSelectQueryCharGuildID(CHAR* szCharID, CHAR* szWorldDBName, CHAR* szQuery, INT32 lQueryLength) | szQuery >= lQueryLength");
		return FALSE;
	}

	return TRUE;
}

// 2005.03.25. steeple
BOOL AgsmGuild::GetSelectResultCharGuildID5(AuDatabase2 *pDatabase, AgpdCharacter* pcsCharacter)
{
	if(!pDatabase || !pcsCharacter)
		return FALSE;

	if(!pDatabase->GetQueryResult(0))
		return TRUE;

		return m_pagpmGuild->SetCharAD(pcsCharacter, pDatabase->GetQueryResult(0), -1, -1 , FALSE, 0);
}







//////////////////////////////////////////////////////////////////////////
// Packet Send
BOOL AgsmGuild::SendSystemMessage(INT32 lCode, UINT32 ulNID, CHAR* szData1 /* = NULL */, CHAR* szData2 /* = NULL */, INT32 lData1 /* = 0 */, INT32 lData2 /* = 0 */)
{
	if(lCode < 0 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildSystemMessagePacket(&nPacketLength,
									&lCode,
									szData1,
									szData2,
									&lData1,
									&lData2
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendSystemMessageToAllMembers(INT32 lCode, AgpdGuild* pcsGuild, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2)
{
	if(lCode < 0 || !pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildSystemMessagePacket(&nPacketLength,
									&lCode,
									szData1,
									szData2,
									&lData1,
									&lData2
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendCreateGuild(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !ulNID)
		return FALSE;

	INT16 nNoticeLength = 0;
	if(pcsGuild->m_szNotice)
		nNoticeLength = (INT16)_tcslen(pcsGuild->m_szNotice);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCreatePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_szMasterID,
									&pcsGuild->m_lTID,
									&pcsGuild->m_lRank,
									&pcsGuild->m_lCreationDate,
									&pcsGuild->m_lMaxMemberCount,
									&pcsGuild->m_lUnionID,
									NULL,	// Ŭ���̾�Ʈ�� Password �� ���� �ʿ� ����.
									pcsGuild->m_szNotice, &nNoticeLength,
									&pcsGuild->m_cStatus,
									&pcsGuild->m_lWin,
									&pcsGuild->m_lDraw,
									&pcsGuild->m_lLose,
									&pcsGuild->m_lGuildPoint,
									&pcsGuild->m_lGuildMarkTID,
									&pcsGuild->m_lGuildMarkColor,
									&pcsGuild->m_lBRRanking
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendJoinRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szCharID, INT32 lCID, UINT32 ulNID)
{
	if(!szGuildID || !szMasterID || !szCharID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJoinRequestPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szMasterID,
									szCharID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendJoinMember(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !pcsMember || !ulNID)
		return FALSE;

	INT32 lJoinDate = m_pagpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJoinPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsMember->m_szID,
									&pcsMember->m_lRank,
									&lJoinDate,
									&pcsMember->m_lLevel,
									&pcsMember->m_lTID,
									&pcsMember->m_cStatus
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.01. By SungHoon
	���� ��û�� ������ �����ش�.
*/
BOOL AgsmGuild::SendSelfJoinMember(CHAR *szGuildID, AgpdGuildRequestMember *pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!szGuildID || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMGUILD_PACKET_JOIN;
	INT32 lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	PVOID pvPacket = m_pagpmGuild->MakeGuildMemberPacket(&nPacketLength, 
																&cOperation,
																&lCID,
																szGuildID,
																pcsMember->m_szMemberID,
																&lRank,
																&pcsMember->m_lLeaveRequestTime,
																&pcsMember->m_lLevel,
																&pcsMember->m_lTID,
																&pcsMember->m_cStatus
																);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendLeaveAllowMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !szCharID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildLeaveAllowPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									szCharID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.19. By SungHoon
	��� Ż�� ��û �ϱ�
*/
BOOL AgsmGuild::SendLeaveMember(AgpdGuild* pcsGuild, CHAR* szCharID, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !szCharID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildLeavePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									szCharID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}


BOOL AgsmGuild::SendDestroyGuild(CHAR* szGuildID, CHAR* szMasterID, INT32 lCID, UINT32 ulNID)
{
	if(!szGuildID || !szMasterID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildDestroyPacket(&nPacketLength, &lCID, szGuildID, szMasterID, NULL);	// Ŭ���̾�Ʈ�� �н������ ���� �ʿ� ����.
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendMaxMemberCount(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildUpdateMaxMemberCountPacket(&nPacketLength, &lCID, pcsGuild->m_szID, &pcsGuild->m_lMaxMemberCount);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Ŭ���̾�Ʈ���� szCharID �� Character �� ��� ������ ������ �Ѵ�.
// szGuildID, pcsMember �� NULL �� ���� �ִ�.
BOOL AgsmGuild::SendCharGuildData(CHAR* szGuildID, CHAR* szCharID, AgpdGuildMember* pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!szCharID || !ulNID)	// szGuildID, pcsMember �� NULL �� ���� �ִ�.
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = NULL;
	if(pcsMember)
	{
		INT32 lJoinDate = m_pagpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate;
		pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
								&lCID,
								szGuildID,
								pcsMember->m_szID,
								&pcsMember->m_lRank,
								&lJoinDate,
								&pcsMember->m_lLevel,
								&pcsMember->m_lTID,
								&pcsMember->m_cStatus,
								NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/, 
								NULL /*IsWinner*/
								);
	}
	else
	{
		pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
								&lCID,
								szGuildID,
								szCharID, NULL, NULL, NULL, NULL, NULL, NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
								NULL /*IsWinner*/);
	}

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.17. By SungHoon
	��� ���� ��� ������� ��� ó�� ���ӽ� �� �Լ��� ��� �ڱ� ��� ������ �޴´�.
*/
BOOL AgsmGuild::SendCharGuildData(AgpdGuildRequestMember* pcsMember, INT32 lCID, UINT32 ulNID)
{
	if(!ulNID ||!pcsMember)	// szGuildID, pcsMember �� NULL �� ���� �ִ�.
		return FALSE;

	INT32 lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
								&lCID,
								pcsMember->m_szGuildID,
								pcsMember->m_szMemberID,
								&lRank,
								&pcsMember->m_lLeaveRequestTime,
								&pcsMember->m_lLevel,
								&pcsMember->m_lTID,
								&pcsMember->m_cStatus,
								NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
								NULL /*IsWinner*/
								);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Notice �� ������.
BOOL AgsmGuild::SendNotice(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	if(!pcsGuild || !lCID || !ulNID)
		return FALSE;

	INT16 nNoticeLength = 0;
	if(pcsGuild->m_szNotice)
		nNoticeLength = (INT16)_tcslen(pcsGuild->m_szNotice);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildUpdateNoticePacket(&nPacketLength,
								&lCID,
								pcsGuild->m_szID,
								pcsGuild->m_szMasterID,
								pcsGuild->m_szNotice, &nNoticeLength
								);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// pcsGuild �� ��� ������� Notice �� ������.
BOOL AgsmGuild::SendNoticeToAllMember(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nNoticeLength = 0;
	if(pcsGuild->m_szNotice)
		nNoticeLength = (INT16)_tcslen(pcsGuild->m_szNotice);

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildUpdateNoticePacket(&nPacketLength,
								&lCID,
								pcsGuild->m_szID,
								pcsGuild->m_szMasterID,
								pcsGuild->m_szNotice, &nNoticeLength
								);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

/*
	2005.08.12. By SungHoon
	���Դ������ ����ڿ��� �渶�� �㰡�� �ߴ�.
*/
BOOL AgsmGuild::SendNewMemberJoinToOtherMembers(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		// ���� ������ ����� ��ŵ
		if(strcmp((*ppcsMember)->m_szID, pcsMember->m_szID) == 0)
			continue;

		SendJoinMember(pcsGuild, pcsMember, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_JOIN, GetMemberNID(*ppcsMember), pcsMember->m_szID);
	}

	return TRUE;
}
/*
	2005.08.01. By SungHoon
	���Խ�û�� ������ ������ ��� �������� �����ش�.
*/
BOOL AgsmGuild::SendNewMemberSelfJoinToOtherMembers(CHAR *szGuildID, AgpdGuildRequestMember *pcsMember, INT32 lCID)
{
	if(!szGuildID || !pcsMember)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendSelfJoinMember(szGuildID, pcsMember,  lCID,  GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOIN_REQUEST_SELF, GetMemberNID(*ppcsMember), pcsMember->m_szMemberID);
	}
	pcsGuild->m_Mutex.Release();

	return TRUE;
}
// lNID �� pcsGuild �� ��� ����� Join ��Ŷ�� ������.
// ���� ������ ������� ���� �� ���̰�, ó�� ���ӽÿ� ���δ�.
BOOL AgsmGuild::SendAllMemberJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendJoinMember(pcsGuild, *ppcsMember, lCID, ulNID);
	}
//	���Խ�û ���� List�� �����ش�.
	SendAllMemberSelfJoin(pcsGuild, lCID, ulNID);
	return TRUE;
}

// lNID �� pcsGuild �� ��� ����� Join ��Ŷ�� ������.
// ���� ������ ������� ���� �� ���̰�, ó�� ���ӽÿ� ���δ�.
BOOL AgsmGuild::SendOtherMemberMemberJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID, CHAR *szMemberID)
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		if (!strcmp((*ppcsMember)->m_szID, szMemberID)) continue;
		SendJoinMember(pcsGuild, *ppcsMember, lCID, ulNID);
	}
//	���Խ�û ���� List�� �����ش�.
	SendAllMemberSelfJoin(pcsGuild, lCID, ulNID);
	return TRUE;
}

/*
	2005.08.01. By SungHoon
	���Խ�û���� ����ڸ� �����ش�.
*/
BOOL AgsmGuild::SendAllMemberSelfJoin(AgpdGuild* pcsGuild, INT32 lCID, UINT32 ulNID)
{
	AgpdGuildRequestMember **ppcsMember = NULL;
	AgpdGuildRequestMember *pcsMember = NULL;
	INT32 lIndex = 0;

	AuAutoLock Lock(pcsGuild->m_pGuildJoinList->m_Mutex);
	if (!Lock.Result()) return FALSE;

	for(ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if (!*ppcsMember) break;
		pcsMember = *ppcsMember;
		SendSelfJoinMember(pcsMember->m_szGuildID, pcsMember, 1, ulNID);
	}

	return TRUE;
}

/*
	2005.08.19. By SungHoon
	��� Ż�� ��û �޼����� ������.
*/
BOOL AgsmGuild::SendLeaveToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID)
{
	if(!pcsGuild || !szCharID)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		if (!strcmp((*ppcsMember)->m_szID, szCharID)) continue;

		SendLeaveMember(pcsGuild, szCharID, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_LEAVE_REQUEST, GetMemberNID(*ppcsMember), szCharID);
	}

	return TRUE;
}

BOOL AgsmGuild::SendLeaveAllowToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID)
{
	if(!pcsGuild || !szCharID)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendLeaveAllowMember(pcsGuild, szCharID, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_LEAVE, GetMemberNID(*ppcsMember), szCharID);
	}

	return TRUE;
}

BOOL AgsmGuild::SendForcedLeaveToOtherMembers(AgpdGuild* pcsGuild, CHAR* szCharID)
{
	if(!pcsGuild || !szCharID)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendLeaveAllowMember(pcsGuild, szCharID, 1, GetMemberNID(*ppcsMember));
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_GUILD_FORCED_LEAVE, GetMemberNID(*ppcsMember), szCharID);
	}

	return TRUE;
}

BOOL AgsmGuild::SendMaxMemberCountToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!*ppcsMember)
			continue;

		SendMaxMemberCount(pcsGuild, 1, GetMemberNID(*ppcsMember));
	}

	return TRUE;
}

BOOL AgsmGuild::SendMemberUpdateToMembers(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember, BOOL bIncludeMe)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	INT32 lJoinDate = m_pagpmGuild->GetCurrentTimeStamp() - pcsMember->m_lJoinDate;		//	������ �ð��� �����ش�
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
							&lCID,
							pcsGuild->m_szID,
							pcsMember->m_szID,
							&pcsMember->m_lRank,
							&lJoinDate,
							&pcsMember->m_lLevel,
							&pcsMember->m_lTID,
							&pcsMember->m_cStatus,
							&pcsGuild->m_lGuildMarkTID,
							&pcsGuild->m_lGuildMarkColor,
							&pcsGuild->m_lBRRanking
							);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if (bIncludeMe) SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	else SendPacketToOtherMembers(pcsGuild, pcsMember->m_szID, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendRequestJoinMemberUpdateToMembers(AgpdGuild* pcsGuild, AgpdGuildRequestMember* pcsMember, BOOL bIncludeMe)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	INT32 lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
							&lCID,
							pcsGuild->m_szID,
							pcsMember->m_szMemberID,
							&lRank,
							&pcsMember->m_lLeaveRequestTime,
							&pcsMember->m_lLevel,
							&pcsMember->m_lTID,
							&pcsMember->m_cStatus,
							NULL/*GuildMarkTID*/, NULL /*GuildMarkColor*/,
							NULL /*IsWinner*/
							);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if (bIncludeMe) SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	else SendPacketToOtherMembers(pcsGuild, pcsMember->m_szMemberID, pvPacket, nPacketLength);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}
// �ڱ��� ��� �̸��� ��������鿡�� ������.
// lNID �� �ڱ��� NID �̸�, �ڱ����״� ������ �ʴ´�.
BOOL AgsmGuild::SendCharGuildIDToNear(CHAR* szGuildID, CHAR* szCharID, AuPOS stPos, INT32 lGuildMarkTID, INT32 lGuildMarkColor, INT32 lBRRanking, UINT32 ulNID)
{
	if(!szCharID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szCharID, NULL, NULL, NULL, NULL, NULL,
									&lGuildMarkTID, &lGuildMarkColor, &lBRRanking);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = m_pagsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, stPos, -1, ulNID, PACKET_PRIORITY_6);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.16. steeple
// Battle Request �� ������. �����Ϳ��� ����.
BOOL AgsmGuild::SendBattleRequest(CHAR* szGuildID, CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 lPerson, UINT32 ulNID)
{
	if(!szGuildID || !szEnemyGuildID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_REQUEST pPacketGuildBattleRequest(&nPacketLength, &lCID,
		szGuildID, NULL, szEnemyGuildID, &lType, &ulDuration, &lPerson);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleRequest);
									
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
// Battle Accept �� ��� ������� ������.
BOOL AgsmGuild::SendBattleAcceptToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_ACCEPT pPacketGuildBattleAccept(&nPacketLength,
		&lCID,
		pcsGuild->m_szID,
		pcsGuild->m_szMasterID,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
		&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
		(INT32*)&pcsGuild->m_csCurrentBattleInfo.m_eType,
		&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
		(UINT32*)&pcsGuild->m_csCurrentBattleInfo.m_ulPerson);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleAccept);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleCancelRequest(CHAR* szGuildID, CHAR* szMasterID, CHAR* szEnemyGuildID, UINT32 ulNID)
{
	if(!szGuildID || !szEnemyGuildID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleCancelReqeustPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szMasterID,
									szEnemyGuildID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleCancelAcceptToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleCancelAcceptPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_szMasterID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleStartToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleStartPacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID,
									&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
									&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleEndToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_RESULT pPacketGuildBattleResult(&nPacketLength, &lCID, 
		pcsGuild->m_szID, 
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, 
		&pcsGuild->m_csCurrentBattleInfo.m_lMyUpPoint, 
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpPoint, 
		&pcsGuild->m_csCurrentBattleInfo.m_cResult);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleResult);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleUpdateTimeToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleUpdateTimePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
									&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
									&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
									&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
BOOL AgsmGuild::SendBattleUpdateScoreToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleUpdateScorePacket(&nPacketLength,
									&lCID,
									pcsGuild->m_szID,
									pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
									&pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
									&pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
									&pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore,
									&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore
									);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendBattleRoundToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;

	PACKET_GUILD_BATTLE_ROUND pPacketGuildBattleRound(&nPacketLength, &lCID, 
		pcsGuild->m_szID, 
		&pcsGuild->m_csCurrentBattleInfo.m_ulRound);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattleRound);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

BOOL AgsmGuild::SendBattleMemberInfoAllMembers(AgpdGuild* pcsGuild, CHAR* szGuildID, AgpdGuildMember* pMember)
{
	if( !pcsGuild || !szGuildID || !pMember )		return FALSE;
	
	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleMemberPacket(&nPacketLength, &lCID, szGuildID, 
																pMember->m_szID, pMember->m_ulScore, pMember->m_ulKill, pMember->m_ulDeath);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SendBattleMemberListInfoAllMembers(AgpdGuild* pcsMyGuild, AgpdGuild* pcsEnemyGuild)
{
	if( !pcsMyGuild )		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattleMemberListPacket(&nPacketLength, &lCID, pcsMyGuild->m_szID, pcsEnemyGuild->m_szID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsMyGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
// ��� ��Ʋ�� ���õ� ��� ������ �� �����ش�.
// ���� ��Ʋ�� ������ �� �� BattleEnd �� ���� ������ ȣ��ǰ� �ȴ�.
BOOL AgsmGuild::SendBattleInfoToAllMembers(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_INFO pPacketGuildBattle(&nPacketLength,
		&lCID,
		pcsGuild->m_szID,
		pcsGuild->m_szMasterID,
		&pcsGuild->m_cStatus,
		&pcsGuild->m_lWin,
		&pcsGuild->m_lDraw,
		&pcsGuild->m_lLose,
		&pcsGuild->m_lGuildPoint,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID,		
		&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
		(INT32*)&pcsGuild->m_csCurrentBattleInfo.m_eType,
		&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
		&pcsGuild->m_csCurrentBattleInfo.m_ulPerson,
		&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_cResult
		);

	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattle);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength, TRUE);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2005.04.17. steeple
// Ư�� ulNID ��, ��� ��Ʋ ������ �����ش�.
BOOL AgsmGuild::SendBattleInfo(AgpdGuild* pcsGuild, UINT32 ulNID)
{
	if(!pcsGuild || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	
	PACKET_GUILD_BATTLE_INFO pPacketGuildBattle(&nPacketLength,
		&lCID,
		pcsGuild->m_szID,
		pcsGuild->m_szMasterID,
		&pcsGuild->m_cStatus,
		&pcsGuild->m_lWin,
		&pcsGuild->m_lDraw,
		&pcsGuild->m_lLose,
		&pcsGuild->m_lGuildPoint,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID,
		pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID,
		&pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime,
		&pcsGuild->m_csCurrentBattleInfo.m_ulStartTime,
		(INT32*)&pcsGuild->m_csCurrentBattleInfo.m_eType,
		&pcsGuild->m_csCurrentBattleInfo.m_ulDuration,
		&pcsGuild->m_csCurrentBattleInfo.m_ulPerson,
		&pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore,
		&pcsGuild->m_csCurrentBattleInfo.m_cResult
		);
		
	PVOID pvPacket = m_pagpmGuild->MakeGuildBattlePacket(pPacketGuildBattle);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}


// 2005.04.17. steeple
// ���� ��Ŷ�� �ϳ��� ��ü������� �� ������.
BOOL AgsmGuild::SendPacketToAllMembers(AgpdGuild* pcsGuild, PVOID pvPacket, INT16 nPacketLength, BOOL bBattle)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	AgpdCharacter* pcsCharacter = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppcsMember )		continue;
		UINT32	ulNID = GetMemberNID(*ppcsMember);
		if( !ulNID )			continue;
		//if( bBattle && !pcsGuild->IsBattle( (*ppcsMember)->m_szID ) )	continue;
			
		SendPacket(pvPacket, nPacketLength, ulNID);
	}

	return TRUE;
}

// 2008. 09. 18. iluvs
// ���� ��Ŷ�� ��ü ������� �� ������. (���ο� ��Ŷ����)
BOOL AgsmGuild::SendPacketToAllMembers(AgpdGuild* pcsGuild, PACKET_HEADER& pvPacket)
{
	if(!pcsGuild || pvPacket.unPacketLength == 0)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	AgpdCharacter* pcsCharacter = NULL;
	INT32 lIndex = 0;

	for( ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex);
		 ppcsMember;
		 ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if (!*ppcsMember)	continue;
		UINT32 ulNID = GetMemberNID(*ppcsMember);
		if (!ulNID)			continue;
		
		AgsEngine::GetInstance()->SendPacket(pvPacket, ulNID);
	}

	return TRUE;
}

/*
	2005.08.16. By SungHoon
	���� ��Ŷ�� �ϳ��� szMemberID �� ������ ��ü������� �� ������.
*/
BOOL AgsmGuild::SendPacketToOtherMembers(AgpdGuild* pcsGuild, CHAR *szMemberID, PVOID pvPacket, INT16 nPacketLength)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	AgpdGuildMember** ppcsMember = NULL;
	AgpdCharacter* pcsCharacter = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if( !*ppcsMember )		continue;
		UINT32	ulNID = GetMemberNID(*ppcsMember);
		if( !ulNID )			continue;
		if( !strcmp( (*ppcsMember)->m_szID, szMemberID ) )	continue;

		SendPacket(pvPacket, nPacketLength, ulNID);
	}

	return TRUE;
}

/*
	2005.09.05. By SungHoon
	���� ������� ����ڿ��� ��Ŷ�� �����ش�.
*/
BOOL AgsmGuild::SendPacketToAllJoinRequestMembers(AgpdGuild* pcsGuild, PVOID pvPacket, INT16 nPacketLength)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	UINT32 ulNID = 0;
	AgpdGuildRequestMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock((*ppcsMember)->m_szMemberID);
		if(!pcsCharacter) continue;

		ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);
		pcsCharacter->m_Mutex.Release();

		if(!ulNID) continue;
		SendPacket(pvPacket, nPacketLength, ulNID);
	}

	return TRUE;
}

// �ٸ� ���� ������ ��� ���� ��Ŷ�� ������.
BOOL AgsmGuild::SendSyncCreate(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketGuild(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_CREATE, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// �ٸ� ���� ������ ��� ������ ������.
BOOL AgsmGuild::SendSyncJoin(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketMember(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_JOIN, pcsGuild, pcsMember);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// �ٸ� ���� ������ ��� Ż�� ������.
BOOL AgsmGuild::SendSyncLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketMember(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_LEAVE, pcsGuild, pcsMember);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// �ٸ� ���� ������ ��� ©���� ������.
BOOL AgsmGuild::SendSyncForcedLeave(AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pcsGuild || !pcsMember)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketMember(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_FORCED_LEAVE, pcsGuild, pcsMember);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// �ٸ� ���� ������ ��� ��ü�� ������.
BOOL AgsmGuild::SendSyncDestroy(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketGuild(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_DESTROY, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// �ٸ� ���� ������ ��� �ƽ� ��� ������Ʈ�� ������.
BOOL AgsmGuild::SendSyncUpdateMaxMemberCount(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketGuild(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_UPDATE_MAX_MEMBER_COUNT, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// �ٸ� ���� ������ ��� ���������� ������.
BOOL AgsmGuild::SendSyncUpdateNotice(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakeSyncPacketNotice(&nPacketLength, (INT8)AGSMGUILD_PACKET_SYNC_UPDATE_NOTICE, pcsGuild);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	SendSyncPacketToOtherGameServers(pvPacket, nPacketLength);
	m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

// �ٸ� ���Ӽ����� ��Ŷ�� ������.
BOOL AgsmGuild::SendSyncPacketToOtherGameServers(PVOID pvPacket, INT16 nPacketLength)
{
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	AgsdServer* pcsThisServer = m_pagsmServerManager->GetThisServer();
	if(!pcsThisServer)
		return FALSE;

	AgsdServer* pcsAgsdServer = NULL;
	INT16 lIndex = 0;
	while((pcsAgsdServer = m_pagsmServerManager->GetGameServers(&lIndex)) != NULL)
	{
		SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer);
	}

	return TRUE;
}

PVOID AgsmGuild::MakeSyncPacketGuild(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild)
{
	if(!pnPacketLength || !pcsGuild)
		return NULL;

	INT16 nGuildSize = sizeof(AgpdGuild);
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								NULL,
								NULL);
}

PVOID AgsmGuild::MakeSyncPacketMember(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild, AgpdGuildMember* pcsMember)
{
	if(!pnPacketLength || !pcsGuild || !pcsMember)
		return NULL;

	INT16 nGuildSize = sizeof(AgpdGuild);
	INT16 nMemberSize = sizeof(AgpdGuildMember);
	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								pcsMember, &nMemberSize,
								NULL);
}

PVOID AgsmGuild::MakeSyncPacketNotice(INT16* pnPacketLength, INT8 cOperation, AgpdGuild* pcsGuild)
{
	if(!pnPacketLength || !pcsGuild)
		return NULL;

	INT16 nGuildSize = sizeof(AgpdGuild);
	INT16 nNoticeSize = pcsGuild->m_szNotice ? (INT16)_tcslen(pcsGuild->m_szNotice) : 0;
	if(nNoticeSize > 0)
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								NULL,
								pcsGuild->m_szNotice, &nNoticeSize);
	else
		return m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMGUILD_PACKET_TYPE,
								&cOperation,
								pcsGuild, &nGuildSize,
								NULL,
								NULL);
}









//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgsmGuild::CBCreateGuildCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szMasterID = (CHAR*)pCustData;

	return pThis->CreateGuildCheck(pcsGuild, szMasterID);
}

BOOL AgsmGuild::CBCreateGuildCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pResult = (BOOL*)pCustData;

	if(!ppvBuffer || !pThis || !pResult)
		return FALSE;

	*pResult = pThis->CreateGuildCheckEnable((CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);
	return TRUE;
}

BOOL AgsmGuild::CBCreateGuildSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szMasterID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szMasterID)
		return FALSE;

	return pThis->CreateGuildSuccess(szGuildID, szMasterID);
}

BOOL AgsmGuild::CBCreateGuildFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmGuild::CBJoinRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(!ppvBuffer || !pThis || !pcsCharacter)
		return FALSE;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];

	return pThis->JoinRequest(szGuildID, szMasterID, pcsCharacter);
}

BOOL AgsmGuild::CBJoinCheckEnable(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pbResult = (BOOL*)pCustData;
	if(!ppvBuffer || !pThis || !pbResult)
		return FALSE;

	BOOL bSelfRequest = *((BOOL*)ppvBuffer[2]);
	*pbResult = pThis->JoinCheckEnable((CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1], bSelfRequest);
	return *pbResult;
}

BOOL AgsmGuild::CBJoinReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szCharID)
		return FALSE;

	return pThis->JoinReject(szGuildID, szCharID);
}

BOOL AgsmGuild::CBJoin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szMemberID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szMemberID)
		return FALSE;

	return pThis->JoinMember(szGuildID, szMemberID);
}

BOOL AgsmGuild::CBJoinFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	INT8* pcErrorCode = (INT8*)pCustData;

	if(!pcsCharacter || !pThis || !pcErrorCode)
		return FALSE;

	return pThis->SendSystemMessage(*pcErrorCode, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));
}

BOOL AgsmGuild::CBLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	if(!szGuildID || !pThis || !szCharID)
		return FALSE;

	return pThis->Leave(szGuildID, szCharID);
}

BOOL AgsmGuild::CBForcedLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR* szCharID = (CHAR*)pCustData;

	if(!ppvBuffer || !pThis || !szCharID)
		return FALSE;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];

	return pThis->ForcedLeave(szGuildID, szCharID);
}

BOOL AgsmGuild::CBDestroyGuild(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR** pszMemberID = (CHAR**)pCustData;

	if(!ppvBuffer || !pThis || !pszMemberID)
		return FALSE;

	CHAR* szGuildID = (CHAR*)ppvBuffer[0];
	CHAR* szMasterID = (CHAR*)ppvBuffer[1];
	INT32* plMemberCount = (INT32*)ppvBuffer[2];

	return pThis->DestroyGuild(szGuildID, szMasterID, *plMemberCount, pszMemberID);
}

BOOL AgsmGuild::CBDestroyGuildFail(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmGuild::CBCheckPassword(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = (PVOID*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pbValidPassword = (BOOL*)pCustData;

	*pbValidPassword = pThis->CheckPassword((AgpdGuild*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// pcsGuild �� Lock �Ǿ �Ѿ�´�.
BOOL AgsmGuild::CBUpdateNotice(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	pThis->SendNoticeToAllMember(pcsGuild);

	// 2005.03.18. steeple
	// �ٸ� ������ Sync �Ѵ�.
	//pThis->SendSyncUpdateNotice(pcsGuild);
	return TRUE;
}

// 2005.04.17. steeple
// ������~ AgpmGuild ������ SystemMessage ���� �� �ְ� �ϱ� ���ؼ�
BOOL AgsmGuild::CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdGuildSystemMessage* pstSystemMessage = (AgpdGuildSystemMessage*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	return pThis->SendSystemMessage(pstSystemMessage->m_lCode,
									pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter),
									pstSystemMessage->m_aszData[0],
									pstSystemMessage->m_aszData[1],
									pstSystemMessage->m_alData[0],
									pstSystemMessage->m_alData[1]
									);
}

BOOL AgsmGuild::CBBattlePerson(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pData || !pClass )
		return FALSE;

	AgpdGuild* pcsGuild	= (AgpdGuild*)pData;
	AgsmGuild* pThis	= (AgsmGuild*)pClass;
	PVOID* ppvBuffer	= (PVOID*)pCustData;

	//������ Ư���� �ϴ����� ����. ����

	return TRUE;
}

// 2005.04.15. steeple
BOOL AgsmGuild::CBBattleRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleRequest(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1], *(INT32*)ppvBuffer[2], *(UINT32*)ppvBuffer[3], *(UINT32*)ppvBuffer[4] );

	return TRUE;
}

// 2005.04.15. steeple
BOOL AgsmGuild::CBBattleAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdGuildBattle* pstGuildBattle = (AgpdGuildBattle*)pCustData;

	pThis->BattleAccept(szGuildID, pstGuildBattle->m_szEnemyGuildID, pstGuildBattle->m_eType, pstGuildBattle->m_ulDuration, pstGuildBattle->m_ulPerson);

	return TRUE;
}

// 2005.04.15. steeple
BOOL AgsmGuild::CBBattleReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleReject(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::CBBattleCancelRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleCancelRequest(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::CBBattleCancelAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleCancelAccept(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.16. steeple
BOOL AgsmGuild::CBBattleCancelReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	pThis->BattleCancelReject(szGuildID, (CHAR*)ppvBuffer[0], (CHAR*)ppvBuffer[1]);

	return TRUE;
}

// 2005.04.17. steeple
// pcsGuild �� pcsCharacter �� Lock �Ǿ �Ѿ�´�.
BOOL AgsmGuild::CBBattleWithdraw(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	return pThis->BattleWithdraw(pcsGuild, pcsCharacter);
}







// �ٸ� Server �� ������ �������� �� �Ҹ���.
// Relay �������� Ȯ���ϰ�, Relay ������� DB �ε��� �����Ѵ�.
BOOL AgsmGuild::CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return TRUE;

	AgsdServer* pcsServer = (AgsdServer*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	AgsdServer* pcsRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if(!pcsRelayServer)
		return FALSE;

	if(pcsServer == pcsRelayServer)
		pThis->LoadAllGuildInfo();

	return TRUE;
}

// �ٸ� Server �� ������ �������� �� �Ҹ���.
// Relay �������� Ȯ���ϰ�, Relay ������� DB �ε��� �����Ѵ�.
BOOL AgsmGuild::CBServerSetFlag(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
	/*
	if(!pData || !pClass)
		return TRUE;

	AgsdServer* pcsServer = (AgsdServer*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	AgsdServer* pcsRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if(!pcsRelayServer)
		return FALSE;

	if(pcsServer == pcsRelayServer)
		pThis->LoadAllGuildInfo();

	return TRUE;
	*/
}

BOOL AgsmGuild::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	if(!pcsCharacter || !pThis)
		return FALSE;

	return pThis->EnterGameWorld(pcsCharacter);
}

BOOL AgsmGuild::CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	if(!pcsCharacter || !pThis)
		return FALSE;

	return pThis->Disconnect(pcsCharacter);
}

BOOL AgsmGuild::CBDeleteComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	CHAR* szCharID = (CHAR*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	return pThis->CharacterDeleteComplete(szCharID);
}

BOOL AgsmGuild::CBCharacterLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	INT32* plLeveUpNum = (INT32*)pCustData;

	pThis->CharacterLevelUp(pcsCharacter, *plLeveUpNum);
	return TRUE;
}

// 2005.11.09. steeple
BOOL AgsmGuild::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	// PC �� ���� ���ָ� �ȴ�.
	if(pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		pThis->Disconnect(pcsCharacter);

	return TRUE;
}

// From AgsmAOIFilter
// ĳ���Ͱ� Cell �� �̵��� ��, �ڱ��� ������ ���� ����鿡�� �Ѹ���.
BOOL AgsmGuild::CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return TRUE;
	
	if(strlen(pcsAttachedGuild->m_szGuildID) > 0)
	{
		AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(pcsAgsdCharacter)
			pThis->SendCharGuildIDToNear(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID, pcsCharacter->m_stPos, 
				pcsAttachedGuild->m_lGuildMarkTID, pcsAttachedGuild->m_lGuildMarkColor, pcsAttachedGuild->m_lBRRanking, 
				pcsAgsdCharacter->m_dpnidCharacter);
	}

	return TRUE;
}

// 2005.03.23. steeple
// AgpmOptimizedPacket2 ���� CharView �� �� ������.
// �� �� �Լ��� ȣ���� �Ǹ� �ι� ������ �Ǵ� ���̹Ƿ� ����. (�ݹ� ����� �ּ�ó�� ����)
//
// From AgsmCharacter
// �� ĳ���Ͱ� �̵��� ��, �� ������ �ִ� ĳ������ ������ �̵��ϴ� ĳ���Ϳ��� �����ش�.
// pcsCharacter �� ������ �ִ� ĳ�����̰�, �Ѿ�� NID �� �̵��� ĳ�����̴�.
// ��, pcsCharacter �� ��� ������ NID �� ������ ���̴�.
BOOL AgsmGuild::CBSendCharView(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;
	
	PROFILE("AgsmGuild::CBSendCharView");

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	// PC �� �ƴϸ� ������.
	if(pThis->m_pagpmCharacter->IsPC(pcsCharacter) == FALSE)
		return TRUE;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// ������ �ƴϸ� ������.
	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	UINT32 ulNID = PtrToUint(ppvBuffer[0]);
	BOOL bGroup = PtrToInt(ppvBuffer[1]);
	BOOL bIsExceptSelf = PtrToInt(ppvBuffer[2]);
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
													&pcsCharacter->m_lID,
													pcsAttachedGuild->m_szGuildID,
													pcsCharacter->m_szID, NULL, NULL, NULL, NULL, NULL,
													&pcsAttachedGuild->m_lGuildMarkTID,
													&pcsAttachedGuild->m_lGuildMarkColor,
													&pcsAttachedGuild->m_lBRRanking
													);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bSendResult = FALSE;
	if(bGroup)
	{
		if(bIsExceptSelf)
		{
			bSendResult = pThis->m_pagsmAOIFilter->SendPacketGroupExceptSelf(pvPacket, nPacketLength, ulNID, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_4);
		}
		else
		{
			bSendResult = pThis->m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);
		}
	}
	else
		bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, ulNID, PACKET_PRIORITY_4);

	pThis->m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	
	return bSendResult;
}

// 2005.03.25. steeple
// Login Server ���� Game Server �� �����͸� �Ѱ��� �� ����Ѵ�.
BOOL AgsmGuild::CBSendCharacterNewID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter	= (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	UINT32 ulNID = *(UINT32*)pCustData;

	// �����̵� ���� ���� ������ �ȴ�.
	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(pcsAttachedGuild && strlen(pcsAttachedGuild->m_szGuildID) > 0)
		pThis->SendCharGuildData(pcsAttachedGuild->m_szGuildID, pThis->m_pagsmCharacter->GetRealCharName(pcsCharacter->m_szID), NULL, 1, ulNID);

	return TRUE;
}

/*
	2005.06.07. By SungHoon
	��� ���������� Ȯ���Ѵ�.
	��忡 ���� �Ǿ��� ���� pCustData = 0
	��帶��Ʈ				pCustData = 1
	��忡 ���� �Ǿ� ����	pCustData = 2
*/

BOOL AgsmGuild::CBIsGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter	= (AgpdCharacter*)pData;
	AgsmGuild* pThis			= (AgsmGuild*)pClass;
	INT32 *plResult				= (BOOL *)pCustData;

	*plResult = 0;

	if (pThis->IsGuildMaster(pcsCharacter))
		*plResult = 1;
	else if (pThis->m_pagpmGuild->IsAnyGuildMember(pcsCharacter))
		*plResult = 2;

	return TRUE;
}

// From AgsmChatting
// pcsCharacter (�����Ͱ� ��) �� Lock �Ǿ� ����
BOOL AgsmGuild::CBGuildMemberInviteByCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	CHAR* szMessage = (CHAR*)ppvBuffer[0];
	INT32* plLength = (INT32*)ppvBuffer[1];

	// ��ɾ ���� ����� ��� ������ �����Ѵ�.
	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) <= 0)
		return FALSE;	

	if(!szMessage || *plLength < 1 || *plLength > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	CHAR szTargetID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szTargetID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING+1));		// 2005.06.07. By SungHoon

	memcpy(szTargetID, szMessage, *plLength);
	//pThis->m_pagpmCharacter->MakeGameID(szTargetID,szTargetID);

	// Target Character Lock
	AgpdCharacter* pcsTarget = pThis->m_pagpmCharacter->GetCharacterLock(szTargetID);
	if(!pcsTarget)
		return FALSE;

	pThis->JoinRequest(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID, pcsTarget);

	pcsTarget->m_Mutex.Release();
	
	return TRUE;
}

// From AgsmChatting
BOOL AgsmGuild::CBGuildLeaveByCommand(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;

	// ��ɾ ���� ����� ��� ������ �����Ѵ�.
	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	if(strlen(pcsAttachedGuild->m_szGuildID) <= 0)
		return FALSE;

	// Master �̸� �Ұ���
	if(pThis->m_pagpmGuild->IsMaster(pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID))
		return FALSE;

	// 2005.02.16. steeple
	// �Ʒ��Լ��� �θ��� �ش�ĳ���͸� ���ѹ� �� �ϰ� �ȴ�.
	// ���� ����� �ɸ��ٸ� ���� Ǯ�� �Լ��� �ҷ���� �Ѵ�.
	pThis->m_pagpmGuild->OnOperationLeave(pcsCharacter->m_lID, pcsAttachedGuild->m_szGuildID, pcsCharacter->m_szID);
	return TRUE;
}

BOOL AgsmGuild::CBGuildJointMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	CHAR* szMessage = (CHAR*)ppvBuffer[0];
	INT32* plLength = (INT32*)ppvBuffer[1];

	return pThis->GuildJointMessage(pcsCharacter, szMessage, *plLength);
}

BOOL AgsmGuild::CBItemPickupCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild *pThis = (AgsmGuild*)pClass;
	stAgpmItemCheckPickupItem *pPickupItem = (stAgpmItemCheckPickupItem*)pData;
	BOOL *pCanPickup = (BOOL*)pCustData;

	// �̹� ȹ�� �Ұ����� ���¶�� ����
	if (FALSE == *pCanPickup)
		return TRUE;

	// �̳� �ϵ� �ڵ�... �Ѥ�;
//	if (E_AGPMITEM_BIND_ON_GUILDMASTER != pPickupItem->pcsItem->m_pcsItemTemplate->m_lStatusFlag)
//		return TRUE;
	
	if (4608 != pPickupItem->pcsItem->m_pcsItemTemplate->m_lID)
		return TRUE;

	if (FALSE == pThis->IsGuildMaster(pPickupItem->pcsCharacter))
	{
		*pCanPickup = FALSE;

		INT16 nPacketLength = 0;
		PVOID pvPacket = pThis->m_pagpmArchlord->MakepacketMessageID(&nPacketLength, AGPMARCHLORD_MESSAGE_GUILD_MASTER_ITEM);

		if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pPickupItem->pcsCharacter->m_lID));
	}

	return TRUE;
}

BOOL AgsmGuild::SetCallbackDBGuildInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildInsertBattleHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_INSERT_BATTLE_HISTORY, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBRename(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_RENAME, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBJointInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBJointUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_JOINT_UPDATE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBJointDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBHostileInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_HOSTILE_INSERT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBHostileDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackAddEnemyGuildByBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackRemoveEnemyGuildByBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackAddEnemyGuildBySurviveBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_ADD_ENEMY_GUILD_BY_SURVIVE_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackRemoveEnemyGuildBySurviveBattle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMGUILD_CB_REMOVE_ENEMY_GUILD_BY_SURVIVE_BATTLE, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_GUILD_SELECT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBMemberSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MEMBER_SELECT, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackDBGuildIDCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_DB_MASTER_CHECK, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackGuildLoad(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_GUILD_LOAD, pfCallback, pClass);
}

BOOL AgsmGuild::SetCallbackSendGuildInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMGUILD_CB_SEND_GUILD_INFO, pfCallback, pClass);
}

/*
	2005.07.08. By SungHoon
	��� ����Ʈ�� �޾��� ��� Call-Back
*/
BOOL AgsmGuild::CBReceiveGuildListInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID *pvArray = (PVOID *)pData;
	INT32 *pCID	= ( INT32 *)pCustData;

	PVOID pvPacket = pvArray[ 0 ];
	CHAR *szGuildID = ( CHAR *)pvArray[ 1 ];

	if (!pvPacket && !szGuildID) return FALSE;

	INT32 lGuildListCurrentPage = 0;
	INT32 lMaxGuildCount = 0;
	INT16 nVisibleGuildCount = 0;

	INT16 nPacketLength = 0;
	PVOID pvGuildListPacket = NULL;

	UINT32 ulNID = 0;
	{
		ApAutoLockCharacter Lock(pThis->m_pagpmCharacter, *pCID);
		AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
		if (!pcsCharacter) return FALSE;

		AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if (!pcsAgsdCharacter) return FALSE;
		ulNID = pcsAgsdCharacter->m_dpnidCharacter;
	}

	if (szGuildID)
	{
		pvGuildListPacket = pThis->m_pagpmGuild->MakeGuildListPacket(pCID, szGuildID,&nPacketLength);
		if (pvGuildListPacket == NULL)
			pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NO_EXIST_SEARCH_GUILD, ulNID);
	}
	else
	{
		PVOID pvGuildListItemPacket[ AGPMGUILD_MAX_VISIBLE_GUILD_LIST ] = { 0 };
		pThis->m_pagpmGuild->m_csGuildListPacket.GetField(FALSE, pvPacket, 0,
							&lMaxGuildCount,
							&lGuildListCurrentPage,
							&nVisibleGuildCount,
							&pvGuildListItemPacket[0],
							&pvGuildListItemPacket[1],
							&pvGuildListItemPacket[2],
							&pvGuildListItemPacket[3],
							&pvGuildListItemPacket[4],
							&pvGuildListItemPacket[5],
							&pvGuildListItemPacket[6],
							&pvGuildListItemPacket[7],
							&pvGuildListItemPacket[8],
							&pvGuildListItemPacket[9],
							&pvGuildListItemPacket[10],
							&pvGuildListItemPacket[11],
							&pvGuildListItemPacket[12],
							&pvGuildListItemPacket[13],
							&pvGuildListItemPacket[14],
							&pvGuildListItemPacket[15],
							&pvGuildListItemPacket[16],
							&pvGuildListItemPacket[17],
							&pvGuildListItemPacket[18]
							);

		pvGuildListPacket = pThis->m_pagpmGuild->MakeGuildListPacket( pCID, lGuildListCurrentPage, &nPacketLength , NULL );
	}
	if (!pvGuildListPacket) return FALSE;
	pThis->m_pagpmGuild->m_csPacket.FreePacket( pvGuildListPacket );

	pThis->SendPacket( pvGuildListPacket, nPacketLength, ulNID );

	return TRUE;
}

/*
	2005.07.28. By SungHoon
	���Ż�� ��û�� �޾��� ��� �Ҹ���.
*/
BOOL AgsmGuild::CBReceiveLeaveRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	CHAR *szGuildID = (CHAR *)pData;
	CHAR *szMemberID = ( CHAR*)pCustData;

	pThis->RequestLeaveMemberSelf(szGuildID, szMemberID);
	return TRUE;
}

/*
	2005.08.01. By SungHoon
	��� ���� ��û�� �޾��� ��� �Ҹ���.
*/
BOOL AgsmGuild::CBReceiveJoinRequestSelf(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	PVOID **ppBuffer = (PVOID **)pData;
	BOOL *pResult= ( BOOL *)pCustData;
	CHAR *szGuildID = (CHAR *)ppBuffer[0];
	CHAR *szMemberID = (CHAR *)ppBuffer[1];
	INT32 *pMemberRank = (INT32 *)ppBuffer[2];
	INT32 *pJoinDate = (INT32 *)ppBuffer[3];
	INT32 *pLevel = (INT32 *)ppBuffer[4];
	INT32 *pTID = (INT32 *)ppBuffer[5];

	if (pThis->RequestJoinMemberSelf(szGuildID, szMemberID, *pMemberRank, *pJoinDate, *pLevel, *pTID))
	{
		*pResult = TRUE;

	//	���뿡�� DB�� �����Ѵ�.
		AgpdGuildMember cGuildMember;
		memset(&cGuildMember, 0, sizeof(AgpdGuildMember));
		strncpy( cGuildMember.m_szID, szMemberID, AGPDCHARACTER_MAX_ID_LENGTH);
		cGuildMember.m_lRank = AGPMGUILD_MEMBER_RANK_JOIN_REQUEST;

		pThis->EnumCallback(AGSMGUILD_CB_DB_MEMBER_INSERT, szGuildID, &cGuildMember);
	}
	return TRUE;
}
/*
	2005.07.13. By SungHoon
	��� �ִ� �ο��� ����ȴ�.
*/
BOOL AgsmGuild::CBGuildUpdateMaxMemberCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis = (AgsmGuild*)pClass;
	INT32 lCID = *( INT32 *)pData;
	INT32 lNewMemberCount = *( INT32 *)pCustData;

	ApAutoLockCharacter Lock(pThis->m_pagpmCharacter, lCID);
	AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
	if (!pcsCharacter) return FALSE;
	DPNID ulNID = pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	AgpdGuildADChar* pcsADCharGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsADCharGuild) return FALSE;

	
	AgpdRequireItemIncreaseMaxMember *pcsRequirement = pThis->m_pagpmGuild->GetRequireIncreaseMaxMember(lNewMemberCount);
	if(!pcsRequirement) return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->GetGuildLock(pcsADCharGuild->m_szGuildID);
	if(!pcsGuild) return FALSE;

	if(pThis->m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID) == FALSE)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	INT32 lMemberCount = pThis->m_pagpmGuild->GetMemberCount(pcsGuild);
	if( lMemberCount < AGPMGUILD_GUILDMEMBER_INCREASE_ENABLE_MEMBER_COUNT )
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// ��� ��å ���� - arycoat 2008.02
	// �ʿ��ذ� ����
	/*AgpdItem *pcsItem = pThis->m_pagpmItem->GetInventoryItemByTID(pcsCharacter,pcsRequirement->m_lSkullTID);
	if(!pcsItem)
	{
		pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX, ulNID);
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	if(pcsItem->m_nCount < pcsRequirement->m_lSkullCount)
	{
		pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_SKULL_FOR_INCREASE_MAX, ulNID);
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}*/
	
	if(pcsCharacter->m_llMoney < pcsRequirement->m_lGheld)
	{
		pThis->SendSystemMessage(AGPMGUILD_SYSTEM_CODE_NOT_ENOUGHT_GHELD_FOR_INCREASE_MAX, ulNID);
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}
	
	// ��� ��å ���� - arycoat 2008.02
	// �ʿ��ذ� ����
	/*if (pThis->m_pagsmItem)	// log
		pThis->m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsCharacter->m_lID, pcsItem, pcsRequirement->m_lSkullCount);
	pThis->m_pagpmItem->SubItemStackCount(pcsItem, pcsRequirement->m_lSkullCount, TRUE);*/
	
	pThis->m_pagpmCharacter->SubMoney(pcsCharacter,pcsRequirement->m_lGheld);
	pThis->UpdateMaxMemberCount(pcsGuild, lNewMemberCount, TRUE );

	AgsdCharacter *pAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	pThis->m_pagpmLog->WriteLog_GuildExpansion(0,
											pAgsdCharacter ? &pAgsdCharacter->m_strIPAddress[0] : "",
											pAgsdCharacter ? pAgsdCharacter->m_szAccountID : "",
											pAgsdCharacter ? pAgsdCharacter->m_szServerName : "",
											pcsCharacter->m_szID,
											pcsCharacter->m_pcsCharacterTemplate->m_lID,
											pThis->m_pagpmCharacter->GetLevel(pcsCharacter),
											pThis->m_pagpmCharacter->GetExp(pcsCharacter),
											pcsCharacter->m_llMoney,
											pcsCharacter->m_llBankMoney,
											pcsGuild->m_szID,
											pcsRequirement->m_lGheld,
											lNewMemberCount
											);

	pThis->SendMaxMemberCountToAllMembers(pcsGuild);

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

/*
	2005.07.26. By SungHoon
	Ż�� ����Ʈ�� ��ȸ�ϸ� 24�ð��� ���� ������ Ż���Ų��.

	���� ������ ������ �ٷ� �����ؾ���...Critical
*/
struct GuildMemberID
{
	CHAR m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR m_szMemberID[AGPACHARACTER_MAX_ID_STRING+1];
};

VOID AgsmGuild::CheckLeaveGuildList( UINT32 ulClockCount )
{
	if(m_ulLastProcessLeaveGuildClock == 0)
	{
		m_ulLastProcessLeaveGuildClock = ulClockCount;
		return;
	}

	// �ð� üũ
	if((ulClockCount - m_ulLastProcessLeaveGuildClock) < AGSMGUILD_IDLE_TIME_INTERVAL)
		return;

	m_ulLastProcessLeaveGuildClock = GetClockCount();

	UINT lCurrentTime = m_pagpmGuild->GetCurrentTimeStamp();
	AgpdGuildRequestMember *pcsMember = NULL;

	ApSort < GuildMemberID * > ApLeaveMember;

	{
		AuAutoLock Lock(m_ApGuildLeaveList.m_Mutex);
		if (!Lock.Result()) return;

		for(pcsMember = m_ApGuildLeaveList.GetHead();!m_ApGuildLeaveList.IsEnd();pcsMember = m_ApGuildLeaveList.GetNext())
		{
			if (!pcsMember) break;
			if (lCurrentTime - pcsMember->m_lLeaveRequestTime > AGPMGUILD_LEAVE_NEED_TIME)
			{
		//	���ð� 24�ð��� �������Ƿ� ��忡�� Ż����� ������.
				GuildMemberID *pGuildMemberID = new GuildMemberID;
				memset(pGuildMemberID, 0, sizeof(GuildMemberID));
				strncpy(pGuildMemberID->m_szGuildID, pcsMember->m_szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
				strncpy(pGuildMemberID->m_szMemberID, pcsMember->m_szMemberID, AGPACHARACTER_MAX_ID_STRING);
				ApLeaveMember.Insert(pGuildMemberID);
			}
		}
	}

	GuildMemberID *pGuildMemberID = NULL;
	for(pGuildMemberID = ApLeaveMember.GetHead();!ApLeaveMember.IsEnd();pGuildMemberID = ApLeaveMember.GetNext())
	{
		if(!pGuildMemberID) break;
		m_pagpmGuild->OnOperationLeaveAllow(1, pGuildMemberID->m_szGuildID, pGuildMemberID->m_szMemberID, FALSE );
		delete pGuildMemberID;
	}
}

/*
	2005.07.27. By SungHoon
	Ż�� ����Ʈ�� �߰��Ѵ�.
*/
BOOL AgsmGuild::RequestLeaveMemberSelf(CHAR *szGuildID, CHAR  *szMemberID)
{
	AgpdGuild *pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	AgpdGuildMember *pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(!pcsGuildMember)
	{
		pcsGuild->m_Mutex.Release();
		JoinRequestReject(szGuildID, szMemberID);
		return FALSE;
	}
	pcsGuildMember->m_lRank = AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST;
	pcsGuildMember->m_lJoinDate = m_pagpmGuild->GetCurrentTimeStamp();

	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, szGuildID, pcsGuildMember);

	SendLeaveToOtherMembers(pcsGuild, szMemberID);
	SendLeaveMember(pcsGuild, szMemberID, 1, GetMemberNID(pcsGuildMember));

	pcsGuild->m_Mutex.Release();

	return InsertLeaveMemberSelf(szGuildID, szMemberID, m_pagpmGuild->GetCurrentTimeStamp());
}

BOOL AgsmGuild::InsertLeaveMemberSelf(CHAR *szGuildID, CHAR *szMemberID , INT32 lLeaveRequestTime)
{
	AgpdGuildRequestMember *pcsMember = new AgpdGuildRequestMember;
	pcsMember->m_lLeaveRequestTime = lLeaveRequestTime;
	strncpy(pcsMember->m_szMemberID, szMemberID, AGPDCHARACTER_MAX_ID_LENGTH);
	strncpy(pcsMember->m_szGuildID, szGuildID,AGPMGUILD_MAX_GUILD_ID_LENGTH);

	m_ApGuildLeaveList.Insert(pcsMember);

	return TRUE;
}

/*
	2005.07.27. By SungHoon
	��� ���� ����Ʈ�� �߰��Ѵ�.
*/
BOOL AgsmGuild::RequestJoinMemberSelf(CHAR* szGuildID, CHAR* szCharID, INT32 lMemberRank, INT32 lJoinDate,INT32 lLevel, INT32 lTID)
{
	INT32 lCID = -1;
	UINT32 ulNID = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	{
		ApAutoLockCharacter Lock(m_pagpmCharacter, szCharID);
		AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();

		if (pcsCharacter)
		{
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (pcsAgsdCharacter) ulNID = pcsAgsdCharacter->m_dpnidCharacter;

			AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
			if(pcsAttachedGuild)
			{
				if (strlen(pcsAttachedGuild->m_szGuildID) > 0 || strlen(pcsAttachedGuild->m_szRequestJoinGuildID) > 0)
				{
					SendSystemMessage(AGPMGUILD_SYSTEM_CODE_ALREADY_MEMBER, ulNID);
					return TRUE;
				}
			}
			if (lLevel == -1) lLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			if (lTID == -1) lTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
			if (lJoinDate == -1) lJoinDate = m_pagpmGuild->GetCurrentTimeStamp();

			memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0 , AGPMGUILD_MAX_GUILD_ID_LENGTH + 1);
			strncpy(pcsAttachedGuild->m_szRequestJoinGuildID, szGuildID,AGPMGUILD_MAX_GUILD_ID_LENGTH); 
		}
	}
	AgpdGuildRequestMember *pcsRequestMember = new AgpdGuildRequestMember();
	pcsRequestMember->m_lTID = lTID;
	pcsRequestMember->m_lLevel = lLevel;
	pcsRequestMember->m_lLeaveRequestTime = lJoinDate;
	strncpy(pcsRequestMember->m_szMemberID, szCharID, AGPDCHARACTER_MAX_ID_LENGTH);
	strncpy(pcsRequestMember->m_szGuildID, szGuildID,AGPMGUILD_MAX_GUILD_ID_LENGTH);
	pcsRequestMember->m_cStatus = AGPMGUILD_MEMBER_STATUS_ONLINE;

	pcsGuild->m_pGuildJoinList->AddObject((PVOID)&pcsRequestMember, pcsRequestMember->m_szMemberID);

	SendCreateGuild(pcsGuild, lCID, ulNID);
	SendSelfJoinMember(szGuildID, pcsRequestMember, lCID, ulNID);
	SendCharGuildData(pcsRequestMember, lCID, ulNID);

	SendNewMemberSelfJoinToOtherMembers(szGuildID, pcsRequestMember, lCID);

	SendRequestJoinMemberUpdateToMembers(pcsGuild, pcsRequestMember, FALSE);

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

/*
	2005.08.01. By SungHoon
	��� ���� ��û ���� �������� �����㰡�� �� ���
*/
BOOL AgsmGuild::AllowJoinMember(CHAR *szGuildID, CHAR  *szMemberID)
{
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT32 lServerID = 0;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

//	���Դ�� ����Ʈ���� �����ϱ�
	AgpdGuildRequestMember *pcsRequestMember = GetJoinRequestMember(pcsGuild, szMemberID);
	if (!pcsRequestMember) return FALSE;

	// Member Data �� ����� �������ش�.
	INT32 lLevel = pcsRequestMember->m_lLevel;
	//m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	INT32 lTID = pcsRequestMember->m_lTID;

	INT8 cStatus = pcsRequestMember->m_cStatus;;

	if (pcsGuild->m_pGuildJoinList->RemoveObject(szMemberID) == TRUE)
		delete pcsRequestMember;

	pcsGuild->m_Mutex.Release();

	//	���� ��� ����Ʈ�� �߰�
	AgpdGuildMember* pcsMember = m_pagpmGuild->GetMember(pcsGuild, szMemberID);
	if(!pcsMember)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	pcsMember->m_lLevel = lLevel;
	pcsMember->m_lTID = lTID;
	pcsMember->m_cStatus = cStatus;
	pcsMember->m_lJoinDate = m_pagpmGuild->GetCurrentTimeStamp();
	pcsMember->m_lRank = AGPMGUILD_MEMBER_RANK_NORMAL;

	{
		ApAutoLockCharacter Lock(m_pagpmCharacter, szMemberID);
		AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
		if(pcsCharacter)
			GuildJoinSuccess(pcsGuild, pcsMember, pcsCharacter, FALSE);
	}

//	���ԵǾ��ٰ� �����ش�. Ŭ���̾�Ʈ������ ���� ����� ����ϹǷ� ���̻� Join���� �ʴ´�.
	SendMemberUpdateToMembers(pcsGuild, pcsMember);		

	//////////////////////////////////////////////////////////////////////////
	// DB �� ����.
	// Member Insert
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, szGuildID, pcsMember);

	if (m_pagpmLog)
	{
		CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1];
		ZeroMemory(szDesc, sizeof(szDesc));
		sprintf(szDesc, "Master=[%s]", pcsGuild->m_szMasterID);

		m_pagpmLog->WriteLog_GuildIn(0, 
										"",
										"",
										"",
										pcsMember->m_szID,
										pcsMember->m_lTID,
										pcsMember->m_lLevel,
										0,
										0,
										0,
										szGuildID,
										szDesc
										);
	}

	// Unlock
	pcsGuild->m_Mutex.Release();

	return TRUE;
}

/*
	2005.08.01. By SungHoon
	szMemberID�� ��� Ż�� ����Ʈ���� �����Ѵ�.
*/
BOOL AgsmGuild::RemoveLeaveMember(CHAR *szMemberID)
{
	AgpdGuildRequestMember *pcsMember = NULL;
	AuAutoLock Lock(m_ApGuildLeaveList.m_Mutex);
	if (!Lock.Result()) return FALSE;

	for(pcsMember = m_ApGuildLeaveList.GetHead();!m_ApGuildLeaveList.IsEnd();)
	{
		if (!pcsMember) break;
		if (!strcmp(pcsMember->m_szMemberID, szMemberID))
		{
			delete pcsMember;
			pcsMember = m_ApGuildLeaveList.Erase();
		}
		else pcsMember = m_ApGuildLeaveList.GetNext();
	}
	return FALSE;
}

/*
	2005.08.02. By SungHoon
	���� ��û�� ����Ѵ�.
*/
BOOL AgsmGuild::JoinRequestReject(CHAR *szGuildID, CHAR *szMemberID)
{
	ApAutoLockCharacter Lock(m_pagpmCharacter, szMemberID);
	AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
	if (!pcsCharacter) return FALSE;
	AgpdGuildADChar* pcsAgsdCharacter = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter) return FALSE;
	if (strcmp(szGuildID,pcsAgsdCharacter->m_szRequestJoinGuildID)) return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pcsAgsdCharacter->m_szRequestJoinGuildID);
	if(!pcsGuild)
		return FALSE;

//	���Դ�� ����Ʈ���� �����ϱ�
	AgpdGuildRequestMember *pcsRequestMember = GetJoinRequestMember(pcsGuild, szMemberID);
	if (!pcsRequestMember)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	if (pcsGuild->m_pGuildJoinList->RemoveObject(szMemberID) == TRUE)
		delete pcsRequestMember;

	pcsGuild->m_Mutex.Release();

	Leave(szGuildID, szMemberID);
	return TRUE;
}
/*
	2005.08.17. By SungHoon
	��� ��ü�� �������� üũ�Ѵ�.
*/
BOOL AgsmGuild::CBDestroyGuildCheckTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdGuild * pcsGuild = (AgpdGuild*)pData;
	AgsmGuild* pThis = (AgsmGuild*)pClass;
	BOOL* pResult = (BOOL*)pCustData;

	*pResult = TRUE;
	// �Ϸ簡 ������� �ʾ�����		2006.07.19. By SungHoon
	UINT32 lCurrentTime = pThis->m_pagpmGuild->GetCurrentTimeStamp();
	if ( lCurrentTime - pcsGuild->m_lCreationDate < AGPMGUILD_GUILD_DESTROY_TIME )
		*pResult = FALSE;
	return TRUE;
}

/*
	2005.08.16. By SungHoon
	�Լ� ȣ�� �Ͽ�ȭ�� ���� ��忡 ������ �������� ��� �Ҹ������� �͵��� ������.
*/
VOID AgsmGuild::GuildJoinSuccess(AgpdGuild *pcsGuild, AgpdGuildMember *pcsGuildMember, AgpdCharacter *pcsCharacter, BOOL bCreate)
{
	if (!pcsGuild || !pcsGuildMember || !pcsCharacter) return;
	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter) return;

	INT32 lCID = pcsCharacter->m_lID;
	UINT32 ulNID = pcsAgsdCharacter->m_dpnidCharacter;
	INT32 lServerID = pcsAgsdCharacter->m_ulServerID;

	// ���� ������ ������� Guild ���� ��Ŷ�� ������.
	if (bCreate) SendCreateGuild(pcsGuild, lCID, ulNID);

	// ���� ������ ���� �ڱ� ���� ����
	m_pagpmGuild->SetCharAD(pcsCharacter, pcsGuild->m_szID, pcsGuild->m_lGuildMarkTID, pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsGuildMember);
	AgsdGuildMember* pcsAgsdMember = GetADMember(pcsGuildMember);
	if(pcsAgsdMember)
		UpdateAgsdGuildMember(pcsAgsdMember, lServerID, ulNID, pcsCharacter);

	// ������ ������� ��� ����� ������ �����ش�. (Join Packet ����.. �ڱ� ����)
	if (bCreate) SendAllMemberJoin(pcsGuild, lCID, ulNID);
	else SendOtherMemberMemberJoin(pcsGuild, lCID, ulNID, pcsCharacter->m_szID);

	// ���� ������ ������� �ڱ��� ������ �����ش�.
	SendCharGuildData(pcsGuild->m_szID, pcsCharacter->m_szID, pcsGuildMember, lCID, ulNID);

	// �ֺ��� �����̵� �Ѹ���.
	SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
				pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, ulNID);

	//	��忡 �������� ��� ����ʴ� �ź� �÷��׸� �����Ѵ�.	2005.06.07 By SungHoon
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_IN);
	pcsCharacter->m_lOptionFlag  = m_pagpmCharacter->UnsetOptionFlag(pcsCharacter, AGPDCHAR_OPTION_REFUSE_GUILD_RELATION);

	EnumCallback(AGSMGUILD_CB_SEND_GUILD_INFO, pcsCharacter, NULL);
}

/*
	2005.08.18. By SungHoon
	���� ������� ����� ã�Ƴ���.
*/
AgpdGuildRequestMember *AgsmGuild::GetJoinRequestMember(AgpdGuild *pcsGuild, CHAR *szID)
{
	if(!pcsGuild || !szID)
		return NULL;

	AgpdGuildRequestMember **ppcsRequestMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObject(szID);
	if (!ppcsRequestMember) return NULL;
	return ( *ppcsRequestMember );
}

/*
	2005.08.24. By SungHoon
	��� �̸� ������ Ŭ���̾�Ʈ�� �����ش�.
*/
BOOL AgsmGuild::ProcessAllMemberRenameGuildID(INT32 lCID, CHAR *szOldGuildID, CHAR *szMemberID, AgpdGuild *pcsGuild, PVOID pvRenamePacket , INT16 nPacketLength )
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsMember)->m_szID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			// ���� ������ ���� �ڱ� ���� ����
			m_pagpmGuild->SetCharAD(pcsCharacter, pcsGuild->m_szID, pcsGuild->m_lGuildMarkTID, pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, *ppcsMember);
			AgsdGuildMember* pcsAgsdMember = GetADMember(*ppcsMember);
			if(pcsAgsdMember)
				UpdateAgsdGuildMember(pcsAgsdMember, pcsAgsdCharacter->m_ulServerID, pcsAgsdCharacter->m_dpnidCharacter, pcsCharacter);

			SendPacket(pvRenamePacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);		//	��� �̸� �ٲ��ٴ� ��Ŷ ����
			SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, 
				NULL, NULL, FALSE, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;
}

/*
	2005.08.24. By SungHoon
	��� �̸� ������ ���Դ�� ������ Ŭ���̾�Ʈ�� �����ش�.
*/
BOOL AgsmGuild::ProcessAllRequestJoinMemberRenameGuildID(INT32 lCID, CHAR *szOldGuildID, CHAR *szMemberID, AgpdGuild *pcsGuild, PVOID pvRenamePacket, INT16 nPacketLength )
{
	//STOPWATCH2(GetModuleName(), _T("ProcessAllRequestJoinMemberRenameGuildID"));

	INT32 lIndex = 0;
	AgpdGuildRequestMember **ppcsRequestJoinMember = NULL;

//	���Դ������ ����ڿ��Ե� ����̸��� ����Ǿ������� �����ش�.
	for(ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsRequestJoinMember;
		ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(!ppcsRequestJoinMember || !*ppcsRequestJoinMember)
			break;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsRequestJoinMember)->m_szMemberID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;

			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
			if(pcsAttachedGuild)
			{
				memset(pcsAttachedGuild->m_szRequestJoinGuildID, 0 , AGPMGUILD_MAX_GUILD_ID_LENGTH + 1);
				strncpy(pcsAttachedGuild->m_szRequestJoinGuildID, pcsGuild->m_szID,AGPMGUILD_MAX_GUILD_ID_LENGTH); 

				memset((*ppcsRequestJoinMember)->m_szGuildID, 0 , AGPMGUILD_MAX_GUILD_ID_LENGTH + 1);
				strncpy((*ppcsRequestJoinMember)->m_szGuildID, pcsGuild->m_szID,AGPMGUILD_MAX_GUILD_ID_LENGTH);

				SendPacket(pvRenamePacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);		//	��� �̸� �ٲ��ٴ� ��Ŷ ����
			}
		}
	}
	return TRUE;
}

/*
	2005.08.24. By SungHoon
	��� ���̵� ������ ��û�Ѵ�.
*/
BOOL AgsmGuild::CBGuildRenameGuildID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild *pThis = (AgsmGuild *)pClass;
	INT32 *pCID = ( INT32 *)pData;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	CHAR *szGuildID = ( CHAR *)ppvBuffer[0];
	CHAR *szMemberID = ( CHAR *)ppvBuffer[1];

	return (pThis->RenameGuild(*pCID, szGuildID, szMemberID));
}

/*
	2005.08.25. By SungHoon
	������ ����̸��� �ٲٴ� ������ �����Ѵ�.
*/
BOOL AgsmGuild::RenameGuild(INT32 lCID, CHAR *szGuildID, CHAR *szMemberID)
{
	// Guild Name Ư������ üũ
	INT8 cErrorCode = -1;
	CHAR szOldGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	memset(szOldGuildID, 0 ,AGPMGUILD_MAX_GUILD_ID_LENGTH+1);
	UINT32 ulNID = 0;

	ApAutoLockCharacter Lock(m_pagpmCharacter, szMemberID);
	AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
	if (!pcsCharacter) return FALSE;
	AgpdGuildADChar* pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
	if (!pcsAttachedGuild) return FALSE;
	strncpy(szOldGuildID, pcsAttachedGuild->m_szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
	ulNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	if(cErrorCode == -1)
	{
		if(!CheckGuildID(szGuildID))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_USE_SPECIAL_CHAR;
	}

	// Guild Name �ߺ� üũ
	if(cErrorCode == -1)
	{
		if(m_pagpmGuild->GetGuild(szGuildID))
			cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_EXIST_GUILD_ID;
	}
	//	�渶���� Ȯ�� �ϱ�
	if(cErrorCode == -1)
	{
		if (m_pagpmGuild->IsMaster(szOldGuildID, szMemberID) == FALSE) 
			cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_NOT_MASTER;
	}
	//	���� ����̸��� @�� ����ִ��� Ȯ���ؾ� �Ѵ�.
	if(cErrorCode == -1)
	{
		if (m_pagpmGuild->IsDuplicatedCharacterOfMigration(szOldGuildID)==FALSE)
			{
				cErrorCode = AGPMGUILD_SYSTEM_CODE_RENAME_IMPOSIBLE_GUILD_ID;
			}
	}

	// Error �� �߻�������, �޽����� ������.
	if(cErrorCode != -1)
	{
		SendSystemMessage(cErrorCode, ulNID);
		return FALSE;
	}

	AgpdGuild *pcsGuild = m_pagpmGuild->GetGuildLock(szOldGuildID);
	if (!pcsGuild) return FALSE;

	if (m_pagpmGuild->RenameGuildID(pcsGuild, szGuildID) == FALSE)		//	�̸��ٲٱ⿡ �����ϸ� �̹� �ִٰ� ����.
	{
		m_pagpmGuild->RenameGuildID(pcsGuild, szOldGuildID);
		pcsGuild->m_Mutex.Release();
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_RENAME_IMPOSIBLE_GUILD_ID, ulNID);
		return FALSE;
	}

//	�� �뿡�� DB�� Update�Ѵ�.
	EnumCallback(AGSMGUILD_CB_DB_GUILD_RENAME, szOldGuildID, szGuildID);

	// Log
	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	m_pagpmLog->WriteLog_GuildRename(0,
									 pAgsdCharacter ? &pAgsdCharacter->m_strIPAddress[0] : "",
									 pAgsdCharacter ? pAgsdCharacter->m_szAccountID : "",
									 pAgsdCharacter ? pAgsdCharacter->m_szServerName : "",
									 pcsCharacter->m_szID,
									 pcsCharacter->m_pcsCharacterTemplate->m_lID,
									 m_pagpmCharacter->GetLevel(pcsCharacter),
									 m_pagpmCharacter->GetExp(pcsCharacter),
									 pcsCharacter->m_llMoney,
									 pcsCharacter->m_llBankMoney,
									 szGuildID,
									 szOldGuildID
									 );

//	������� ��ó�� �������� �뺸�ϱ�
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildRenameGuildIDPacket(&lCID, szGuildID, szMemberID, &nPacketLength );

	if(!pvPacket || nPacketLength < 1)
		return FALSE;
	ProcessAllMemberRenameGuildID( lCID, szOldGuildID, szMemberID, pcsGuild, pvPacket , nPacketLength);
	ProcessAllRequestJoinMemberRenameGuildID( lCID, szOldGuildID, szMemberID, pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
//	��Ǯ��
	pcsGuild->m_Mutex.Release();
	return TRUE;
}


/*
	2005.09.05. By SungHoon
	ĳ���� �̸��� �ٲ��� AgsmLogin���� ���� �ݹ� �ȴ�.
*/
BOOL AgsmGuild::CBGuildRenameCharacterID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData) return FALSE;
	AgsmGuild *pThis = (AgsmGuild *)pClass;
	CHAR *szOldID = (CHAR *)pData;
	CHAR *szNewID = (CHAR *)pCustData;

	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->FindGuildLockFromCharID(szOldID);
	if (!pcsGuild) return FALSE;

	BOOL bResult = pThis->GuildRenameCharacterID(pcsGuild, szOldID, szNewID);

	pcsGuild->m_Mutex.Release();

	return bResult;
}

/*
	2005.09.05. By SungHoon
	ĳ���� �̸��� �ٲ��� ���� ��ο��� ������ �̸��� ����Ǿ����� �˷��ش�.
*/
BOOL AgsmGuild::GuildRenameCharacterID(AgpdGuild *pcsGuild, CHAR *szOldID, CHAR *szNewID)
{
	if (!pcsGuild || !szOldID || !szNewID) return FALSE;

	if (!m_pagpmGuild->RenameCharID(pcsGuild, szOldID, szNewID)) return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildRenameCharacterIDPacket(pcsGuild->m_szID, szOldID, szNewID, &nPacketLength );
	if (!pvPacket) return FALSE;
	
	SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);
	SendPacketToAllJoinRequestMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

/*
	2005.10.19. By SungHoon
	��� ��ũ���� ��û�� �Ѵ�. 
*/
BOOL AgsmGuild::CBGuildBuyGuildMark(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild *pThis = (AgsmGuild *)pClass;
	PVOID *ppBuffer = (PVOID *)pData;
	CHAR *szGuild = (CHAR *)pCustData;

	INT32 *plCID = ( INT32 *)ppBuffer[0];
	INT32 *pGuildMarkTID = ( INT32 *)ppBuffer[1];
	INT32 *pGuildMarkColor = ( INT32 *)ppBuffer[2];

	return ( pThis->GuildBuyGuildMark(*plCID, szGuild, *pGuildMarkTID, *pGuildMarkColor , FALSE) );
}

/*
	2005.10.19. By SungHoon
	��� ��ũ���� ��û�� �Ѵ�. �ߺ��� �����Ѵ�.
*/
BOOL AgsmGuild::CBGuildBuyGuildMarkForce(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgsmGuild *pThis = (AgsmGuild *)pClass;
	PVOID *ppBuffer = (PVOID *)pData;
	CHAR *szGuild = (CHAR *)pCustData;

	INT32 *plCID = ( INT32 *)ppBuffer[0];
	INT32 *pGuildMarkTID = ( INT32 *)ppBuffer[1];
	INT32 *pGuildMarkColor = ( INT32 *)ppBuffer[2];

	return ( pThis->GuildBuyGuildMark(*plCID, szGuild, *pGuildMarkTID, *pGuildMarkColor, TRUE ) );
}

/*
	2005.10.19. By SungHoon
	��� ��ũ���� ��û�� �Ѵ�. pcsGuild�� Lock�Ǿ� ���´�.
*/
BOOL AgsmGuild::GuildBuyGuildMark(INT32 lCID, CHAR *szGuildID, INT32 lGuildMarkTID, INT32 lGuildMarkColor, BOOL bForce)
{
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(szGuildID);
	if (!pcsGuild) return FALSE;

	{
		ApAutoLockCharacter Lock(m_pagpmCharacter, lCID);
		AgpdCharacter *pcsCharacter = Lock.GetCharacterLock();
		if (!pcsCharacter)
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}

		if (!m_pagpmGuild->IsMaster(pcsGuild,pcsCharacter->m_szID))
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}

//	2005.10.25. By SungHoon �ӽ÷� �׼����� �ּ�ó��
		INT32 lMemberCount = m_pagpmGuild->GetMemberCount(pcsGuild);
		if ( lMemberCount < AGPMGUILD_GUILDMARK_ENABLE_MEMBER_COUNT)
		{
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}

		INT32 lResultCode = -1;
		if (bForce == FALSE && m_pagpmGuild->IsDuplicateGuildMark(lGuildMarkTID) == TRUE)
			lResultCode = AGPMGUILD_SYSTEM_CODE_BUY_GUILDMARK_DUPLICATE;
		if (lResultCode == -1)
			lResultCode = m_pagpmGuild->IsEnableBuyGuildMark(lGuildMarkTID, pcsCharacter);
		if (lResultCode != -1) // ����
		{
			SendSystemMessage(lResultCode, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
			pcsGuild->m_Mutex.Release();
			return FALSE;
		}
		
		INT32 lGheld = SubItemBuyGuildMark(lGuildMarkTID, pcsCharacter);			//	�������� ���

		pcsGuild->m_lGuildMarkTID = lGuildMarkTID;					//	��忡 ��� ��ũ �����ϰ�
		pcsGuild->m_lGuildMarkColor = lGuildMarkColor;

		m_pagpmGuild->SetCharAD(pcsCharacter, szGuildID, lGuildMarkTID, lGuildMarkColor, pcsGuild->m_lBRRanking, NULL);

		EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);		// DB�� �����ϰ�

		//	�α� ������.
		AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);

		CHAR szLogBuffer[65];
		wsprintf( szLogBuffer, "%d", lGuildMarkTID);
		m_pagpmLog->WriteLog_GuildBuyMark(0,
												pAgsdCharacter ? &pAgsdCharacter->m_strIPAddress[0] : "",
												pAgsdCharacter ? pAgsdCharacter->m_szAccountID : "",
												pAgsdCharacter ? pAgsdCharacter->m_szServerName : "",
												pcsCharacter->m_szID,
												pcsCharacter->m_pcsCharacterTemplate->m_lID,
												m_pagpmCharacter->GetLevel(pcsCharacter),
												m_pagpmCharacter->GetExp(pcsCharacter),
												pcsCharacter->m_llMoney,
												pcsCharacter->m_llBankMoney,
												pcsGuild->m_szID,
												szLogBuffer,
												lGheld
												);

	}

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildBuyGuildMarkPacket(&nPacketLength ,lCID, szGuildID, &lGuildMarkTID, 
		&lGuildMarkColor);

	if(!pvPacket || nPacketLength < 1)
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	ProcessAllMemberBuyGuildMark(pcsGuild, pvPacket, nPacketLength);
//	ProcessAllRequestJoinMemberBuyGuildMark(pcsGuild, pvPacket, nPacketLength);		//	���Դ������ ������ ��帶ũ �������� �ʴ´�.

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	pcsGuild->m_Mutex.Release();

	return  TRUE;
}

/*
	2005.10.19. By SungHoon
	��� ��ũ������ Ŭ���̾�Ʈ�� �����ش�.
*/
BOOL AgsmGuild::ProcessAllMemberBuyGuildMark(AgpdGuild *pcsGuild, PVOID pvBuyPacket, INT16 nPacketLength )
{
	//STOPWATCH2(GetModuleName(), _T("ProcessAllMemberBuyGuildMark"));

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsMember)->m_szID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			SendPacket(pvBuyPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);		

			SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
								pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;
}

/*
	2005.10.19. By SungHoon
	��� ��ũ������ Ŭ���̾�Ʈ�� �����ش�.
*/
BOOL AgsmGuild::ProcessAllRequestJoinMemberBuyGuildMark(AgpdGuild *pcsGuild, PVOID pvBuyPacket, INT16 nPacketLength )
{
	INT32 lIndex = 0;
	AgpdGuildRequestMember **ppcsRequestJoinMember = NULL;

//	���Դ������ ����ڿ��Ե� ����̸��� ����Ǿ������� �����ش�.
	for(ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex); ppcsRequestJoinMember;
		ppcsRequestJoinMember = (AgpdGuildRequestMember**)pcsGuild->m_pGuildJoinList->GetObjectSequence(&lIndex))
	{
		if(!ppcsRequestJoinMember || !*ppcsRequestJoinMember)
			break;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsRequestJoinMember)->m_szMemberID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;

			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			SendPacket(pvBuyPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;
}

/*
	2005.10.21. By SungHoon
	��� ��ũ ���Կ� �ʿ��� �����۰� �ֵ带 �Ҹ��Ų��.
	***���� �и��� pcsCharacter�� Lock �� �ɸ� ��Ȳ���� ���԰����� �����۰� �ֵ尡 �ִ��� �̸� Ȯ���صξ�� �Ѵ�.
	AgpmGuild::IsEnableBuyGuildMark �� Ȯ�ΰ���
*/
INT32 AgsmGuild::SubItemBuyGuildMark(INT32 lGuildMarkTID, AgpdCharacter *pcsCharacter)
{
	AgpdGuildMarkTemplate *pGuildMarkTemplate[3];
	pGuildMarkTemplate[0] = m_pagpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_BOTTOM, lGuildMarkTID);
	pGuildMarkTemplate[1] = m_pagpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_PATTERN, lGuildMarkTID);
	pGuildMarkTemplate[2] = m_pagpmGuild->GetGuildMarkTemplate(AGPMGUILD_ID_MARK_SYMBOL, lGuildMarkTID);

	INT32 lGheld = 0;
	for ( int i = 0 ; i < 3; i++)
	{
		if (pGuildMarkTemplate[i])
		{
			lGheld += pGuildMarkTemplate[i]->m_lGheld;

			// ��� ��å ���� - arycoat 2008.02
			// �ʿ��ذ� ����
			/*if (pGuildMarkTemplate[i]->m_lSkullCount > 0)
			{
				AgpdItem *pcsItem = m_pagpmItem->GetInventoryItemByTID(pcsCharacter,pGuildMarkTemplate[i]->m_lSkullTID);
				if (pcsItem)
				{
					if (m_pagsmItem)	// log
						m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_USE, pcsCharacter->m_lID, pcsItem, pGuildMarkTemplate[i]->m_lSkullCount);
					m_pagpmItem->SubItemStackCount(pcsItem, pGuildMarkTemplate[i]->m_lSkullCount, TRUE);
				}
			}*/
		}
	}

	// apply tax	
	INT32 lTaxRatio = m_pagpmCharacter->GetTaxRatio(pcsCharacter);
	INT32 lTax = 0;
	if (lTaxRatio > 0)
	{
		lTax = (lGheld * lTaxRatio) / 100;
	}
	lGheld = lGheld + lTax;	
	
	m_pagpmCharacter->SubMoney(pcsCharacter,lGheld);
	m_pagpmCharacter->PayTax(pcsCharacter, lTax);

	return lGheld;
}

BOOL AgsmGuild::SendCharGuildID(CHAR* szGuildID, CHAR* szCharID, AuPOS stPos, INT32 lGuildMarkTID, INT32 lGuildMarkColor, INT32 lBRRanking, UINT32 ulNID)
{
	if(!szCharID || !ulNID)
		return FALSE;

	INT32 lCID = 1;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildCharDataPacket(&nPacketLength,
									&lCID,
									szGuildID,
									szCharID, NULL, NULL, NULL, NULL, NULL,
									&lGuildMarkTID, &lGuildMarkColor, &lBRRanking);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmGuild::SetWinnerGuild(CHAR* szGuildName, INT32 lBRRank)
{
	AuAutoLock lock(m_Mutex);
	if (!lock.Result()) return FALSE;

	// �����ϰ��� �ϴ� ��尡 �����ϴ��� Ȯ��
	AgpdGuild *pcsGuild = m_pagpmGuild->GetGuild(szGuildName);

	if (NULL == pcsGuild)
	{
		ASSERT(pcsGuild);
		return FALSE;
	}

	if(lBRRank < WINNER_GUILD_NOTRANKED || lBRRank >= WINNER_GUILD_MAX)
		return FALSE;

	INT32 index = 0;
	AgpdCharacter *pcsCharacter = NULL;
	AgpdGuildADChar* pcsAttachedGuild = NULL;
	AgpdGuildMember** ppcsMember = NULL;

	// ������ �¸��� ��尡 �ִ��� Ȯ��
	if (false == szWinnerGuildName[lBRRank].IsEmpty())
	{
		AgpdGuild *pcsWinnerGuild = m_pagpmGuild->GetGuild(szWinnerGuildName[lBRRank].GetBuffer());
		if (NULL != pcsWinnerGuild)
		{
			// ���� ���� �¸���� ǥ�ø� �����Ѵ�.
			pcsWinnerGuild->m_lBRRanking = WINNER_GUILD_NOTRANKED;

			for(ppcsMember = (AgpdGuildMember**)pcsWinnerGuild->m_pMemberList->GetObjectSequence(&index); ppcsMember;
					ppcsMember = (AgpdGuildMember**)pcsWinnerGuild->m_pMemberList->GetObjectSequence(&index))
			{
				if(!ppcsMember || !*ppcsMember)
					continue;

				pcsCharacter = m_pagpmCharacter->GetCharacter((*ppcsMember)->m_szID);
				if (NULL == pcsCharacter)
					continue;

				pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
				if(!pcsAttachedGuild)
					continue;

				AuAutoLock lock(pcsCharacter->m_Mutex);
				if (lock.Result())
					pcsAttachedGuild->m_lBRRanking = WINNER_GUILD_NOTRANKED;
			}

			SendPacketWinnerInfo(pcsWinnerGuild);
		}
	}

	index = 0;
	pcsCharacter = NULL;
	pcsAttachedGuild = NULL;
	ppcsMember = NULL;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&index); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&index))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		pcsCharacter = m_pagpmCharacter->GetCharacter((*ppcsMember)->m_szID);
		if (NULL == pcsCharacter)
			continue;

		pcsAttachedGuild = m_pagpmGuild->GetADCharacter(pcsCharacter);
		if(!pcsAttachedGuild)
			continue;

		AuAutoLock lock(pcsCharacter->m_Mutex);
		if (lock.Result())
			pcsAttachedGuild->m_lBRRanking = lBRRank; // BattleRoyal Ranking
	}

	szWinnerGuildName[lBRRank].SetText(szGuildName);
	pcsGuild->m_lBRRanking = lBRRank; // BattleRoyal Ranking

	SendPacketWinnerInfo(pcsGuild);

	return TRUE;	
}

BOOL AgsmGuild::SendPacketWinnerInfo(AgpdGuild* pcsGuild)
{
	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;

	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
			ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		if(!ppcsMember || !*ppcsMember)
			continue;

		{
			ApAutoLockCharacter Lock(m_pagpmCharacter, (*ppcsMember)->m_szID);
			AgpdCharacter* pcsCharacter = Lock.GetCharacterLock();
			if (!pcsCharacter) continue;
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter) continue;

			SendCharGuildID(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
				pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsAgsdCharacter->m_dpnidCharacter);

			SendCharGuildIDToNear(pcsGuild->m_szID, pcsCharacter->m_szID, pcsCharacter->m_stPos, pcsGuild->m_lGuildMarkTID,
				pcsGuild->m_lGuildMarkColor, pcsGuild->m_lBRRanking, pcsAgsdCharacter->m_dpnidCharacter);
		}
	}
	return TRUE;	
}

// Joint, Hostile ����.
// 2006.07.13. steeple
BOOL AgsmGuild::CBJointRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szJointGuildID	= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID		= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->JointRequest(szGuildID, szJointGuildID, szMasterID);
}

BOOL AgsmGuild::CBJointReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	CHAR* szJointGuildID	= static_cast<CHAR*>(pCustData);

	return pThis->JointReject(szGuildID, szJointGuildID);
}

BOOL AgsmGuild::CBJoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis				= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID					= static_cast<CHAR*>(pData);
	AgpdGuildRelationUnit* pUnit	= static_cast<AgpdGuildRelationUnit*>(pCustData);

	return pThis->Joint(szGuildID, pUnit);
}

BOOL AgsmGuild::CBJointLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	INT32 lCID				= *static_cast<INT32*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szGuildID			= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szJointGuildID	= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->JointLeave(lCID, szGuildID);
}

BOOL AgsmGuild::CBHostileRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID		= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->HostileRequest(szGuildID, szHostileGuildID, szMasterID);
}

BOOL AgsmGuild::CBHostileReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(pCustData);

	return pThis->HostileReject(szGuildID, szHostileGuildID);
}

BOOL AgsmGuild::CBHostile(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis				= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID					= static_cast<CHAR*>(pData);
	AgpdGuildRelationUnit* pUnit	= static_cast<AgpdGuildRelationUnit*>(pCustData);

	return pThis->Hostile(szGuildID, pUnit);
}

BOOL AgsmGuild::CBHostileLeaveRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szMasterID		= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->HostileLeaveRequest(szGuildID, szHostileGuildID, szMasterID);
}

BOOL AgsmGuild::CBHostileLeaveReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	CHAR* szGuildID			= static_cast<CHAR*>(pData);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(pCustData);

	return pThis->HostileLeaveReject(szGuildID, szHostileGuildID);
}

BOOL AgsmGuild::CBHostileLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);
	INT32 lCID				= *static_cast<INT32*>(pData);
	PVOID* ppvBuffer		= static_cast<PVOID*>(pCustData);
	CHAR* szGuildID			= static_cast<CHAR*>(ppvBuffer[0]);
	CHAR* szHostileGuildID	= static_cast<CHAR*>(ppvBuffer[1]);

	return pThis->HostileLeave(szGuildID, szHostileGuildID);
}

BOOL AgsmGuild::JointRequest(CHAR* szGuildID, CHAR* szJointGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	// szGuildID �� szJointGuildID ���� ��û�� ����.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szJointGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild1, szMasterID))
		return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));

	INT32 lCheck = CheckJointEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		SendSystemMessage(lCheck, ulMasterNID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID);

	// ��Ŷ ����.
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendJointRequest(pcsGuild2, pcsGuild1->m_szID, pcsGuild1->m_szMasterID, ulMaster2NID);

	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_WAIT, ulMasterNID);

	return TRUE;
}

BOOL AgsmGuild::JointReject(CHAR* szGuildID, CHAR* szJointGuildID)
{
	if(!szGuildID || !szJointGuildID)
		return FALSE;

	// ��û�� szJointGuildID �� �� ����.
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szJointGuildID);
	if(!pcsGuild2)
		return FALSE;

	AuAutoLock csLock2(pcsGuild2->m_Mutex);
	if (!csLock2.Result()) return FALSE;

	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_REJECT, ulMaster2NID);

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild2);	// ������ ó�� ��� �ʱ�ȭ
	return TRUE;
}

BOOL AgsmGuild::Joint(CHAR* szGuildID, AgpdGuildRelationUnit* pUnit)
{
	if(!szGuildID || !pUnit)
		return FALSE;

	// szGuildID �� ��û���� ���̴�. ��� �ٲ۴�.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(pUnit->m_szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));

	// �ٽ��ѹ� �Ǵ� �� �ȵǴ� �� üũ
	INT32 lCheck = CheckJointEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		// ���⼭�� ���� ������ ������ �ʰ�, �� ���з� ������.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);
		return FALSE;
	}

	// ����� �°��� Ȯ�����ְ�
	if(!m_pagpmGuild->IsLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID))
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1);	// ������ ó�� ��� �ʱ�ȭ

	// �� ���� ��¥ �־��ش�.
	BOOL bResult = TRUE;
	UINT32 ulDate = m_pagpmGuild->GetCurrentTimeStamp();

	// �Ѵ� ������ ���� ���� ó�� �����ϴ� ���̴�.
	INT32 lGuild1JointCount = m_pagpmGuild->GetJointGuildCount(pcsGuild1);
	INT32 lGuild2JointCount = m_pagpmGuild->GetJointGuildCount(pcsGuild2);
	if(lGuild1JointCount == 0 && lGuild2JointCount == 0)
	{
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		bResult &= m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);	// �ڱ� �ڽŵ� �ִ´�.

		if(!bResult)
		{
			// �ѹ�
			m_pagpmGuild->RemoveJointGuild(pcsGuild1, pcsGuild1->m_szID);
			m_pagpmGuild->RemoveJointGuild(pcsGuild1, pcsGuild2->m_szID);
			m_pagpmGuild->RemoveJointGuild(pcsGuild2, pcsGuild1->m_szID);
			m_pagpmGuild->RemoveJointGuild(pcsGuild2, pcsGuild2->m_szID);

			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);
			return FALSE;
		}

		SendJoint(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		SendJoint(pcsGuild1, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJoint(pcsGuild2, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT_LEADER);
		SendJoint(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);

		SendJointDetail(pcsGuild1, pcsGuild1);
		SendJointDetail(pcsGuild1, pcsGuild2);
		SendJointDetail(pcsGuild2, pcsGuild1);
		SendJointDetail(pcsGuild2, pcsGuild2);

		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild1);
		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild2);

		// DB �� ���ش�.
		EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild1, pcsGuild2->m_szID);
		EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild2, pcsGuild1->m_szID);
	}

	// ��û�� ��尡 �̹� ���뿡 �������̴�. ��� �߰� �۾��� �ִ�.
	else if(lGuild1JointCount > 0 && lGuild2JointCount == 0)
	{
		// �߰��� ���Ͱ� ����Ǳ� ������, �̷��� �ӽ÷� ���� �ӽø� ����.
		JointVector vcTmp(pcsGuild1->m_csRelation.m_pJointVector->begin(), pcsGuild1->m_csRelation.m_pJointVector->end());
		JointIter iter = vcTmp.begin();
		while(iter != vcTmp.end())
		{
			AgpdGuild* pcsAlreadyJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(pcsAlreadyJoint)
			{
				AuAutoLock csLock(pcsAlreadyJoint->m_Mutex);
				if (csLock.Result())
				{
					bResult &= m_pagpmGuild->AddJointGuild(pcsAlreadyJoint, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					bResult &= m_pagpmGuild->AddJointGuild(pcsGuild2, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					if(!bResult)
					{
						// �ѹ�
						m_pagpmGuild->RemoveJointGuild(pcsAlreadyJoint, pcsGuild2->m_szID);
						m_pagpmGuild->RemoveJointGuild(pcsGuild2, iter->m_szGuildID);

						break;
					}

					SendJoint(pcsAlreadyJoint, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					SendJoint(pcsGuild2, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					SendJointDetail(pcsAlreadyJoint, pcsGuild2);
					SendJointDetail(pcsGuild2, pcsAlreadyJoint);

					SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS2, pcsAlreadyJoint, pcsGuild2->m_szID);

					// DB �� ���ش�.
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsAlreadyJoint, pcsGuild2->m_szID);
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild2, pcsAlreadyJoint->m_szID);
				}
			}
			++iter;
		}

		if(!bResult)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);

			return FALSE;
		}

		// �ڽŵ� �߰����ش�.
		m_pagpmGuild->AddJointGuild(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJoint(pcsGuild2, pcsGuild2->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJointDetail(pcsGuild2, pcsGuild2);
		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild2);
	}

	// Guild2�� �̹� ���뿡 �������̴�. ��� �߰� �۾��� �ִ�.
	else if(lGuild1JointCount == 0 && lGuild2JointCount > 0)
	{
		// �߰��� ���Ͱ� ����Ǳ� ������, �̷��� �ӽ÷� ���� �ӽø� ����.
		JointVector vcTmp(pcsGuild2->m_csRelation.m_pJointVector->begin(), pcsGuild2->m_csRelation.m_pJointVector->end());
		JointIter iter = vcTmp.begin();
		while(iter != vcTmp.end())
		{
			AgpdGuild* pcsAlreadyJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(pcsAlreadyJoint)
			{
				AuAutoLock csLock(pcsAlreadyJoint->m_Mutex);
				if (csLock.Result())
				{
					bResult &= m_pagpmGuild->AddJointGuild(pcsAlreadyJoint, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					bResult &= m_pagpmGuild->AddJointGuild(pcsGuild1, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					if(!bResult)
					{
						// �ѹ�
						m_pagpmGuild->RemoveJointGuild(pcsAlreadyJoint, pcsGuild1->m_szID);
						m_pagpmGuild->RemoveJointGuild(pcsGuild1, iter->m_szGuildID);

						break;
					}

					SendJoint(pcsAlreadyJoint, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
					SendJoint(pcsGuild1, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation);

					SendJointDetail(pcsAlreadyJoint, pcsGuild1);
					SendJointDetail(pcsGuild1, pcsAlreadyJoint);

					SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS2, pcsAlreadyJoint, pcsGuild1->m_szID);

					// DB �� ���ش�.
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsAlreadyJoint, pcsGuild1->m_szID);
					EnumCallback(AGSMGUILD_CB_DB_JOINT_INSERT, pcsGuild1, pcsAlreadyJoint->m_szID);
				}
			}
			++iter;
		}

		if(!bResult)
		{
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMasterNID);
			SendSystemMessage(AGPMGUILD_SYSTEM_CODE_JOINT_FAILURE, ulMaster2NID);

			return FALSE;
		}

		// �ڽŵ� �߰����ش�.
		m_pagpmGuild->AddJointGuild(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJoint(pcsGuild1, pcsGuild1->m_szID, ulDate, AGPMGUILD_RELATION_JOINT);
		SendJointDetail(pcsGuild1, pcsGuild1);
		SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_SUCCESS, pcsGuild1);
	}

	return TRUE;
}

BOOL AgsmGuild::JointLeave(INT32 lCID, CHAR* szGuildID, BOOL bForce)
{
	if(!szGuildID)
		return FALSE;

	AgpdGuild* pcsGuild = NULL;
	if(!bForce)
	{
		// szGuildID �� ���� Ż�� ����3.
		// ���������� ���� Ȯ��.
		AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
		if(!pcsCharacter)
			return FALSE;

		pcsGuild = m_pagpmGuild->GetGuild(szGuildID);
		if(!pcsGuild)
		{
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}

		BOOL bMaster = m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID);
		pcsCharacter->m_Mutex.Release();

		if(!bMaster)
			return FALSE;
	}
	else
	{
		pcsGuild = m_pagpmGuild->GetGuild(szGuildID);
		if(!pcsGuild)
			return FALSE;
	}

	AuAutoLock csLock(pcsGuild->m_Mutex);
	if (!csLock.Result()) return FALSE;

	BOOL bIsLeaderLeave = FALSE;

	// ���࿡ Ż���ϴ� ��尡 �������̶��, ���������� ������ �����Ѵ�.
	if(m_pagpmGuild->IsJointLeader(pcsGuild))
	{
		bIsLeaderLeave = TRUE;
		std::sort(pcsGuild->m_csRelation.m_pJointVector->begin(), pcsGuild->m_csRelation.m_pJointVector->end(), SortRelation());
	}

	AgpdGuildRelationUnit stNewLeaderUnit;
	memset(&stNewLeaderUnit, 0, sizeof(stNewLeaderUnit));

	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		if(_tcscmp(pcsGuild->m_szID, iter->m_szGuildID) != 0)
		{
			if(bIsLeaderLeave && _tcslen(stNewLeaderUnit.m_szGuildID) == 0)
				stNewLeaderUnit = *iter;

			// �� ��带 ���鼭 szGuildID �� ����� �Ѵ�.
			AgpdGuild* pcsOtherGuild = m_pagpmGuild->GetGuild(iter->m_szGuildID);
			if(pcsOtherGuild)
			{
				AuAutoLock csLock2(pcsOtherGuild->m_Mutex);
				if (csLock2.Result())
				{
					JointLeave(pcsOtherGuild, szGuildID, stNewLeaderUnit);

					//m_pagpmGuild->RemoveJointGuild(pcsOtherGuild, pcsGuild->m_szID);
					SendJointLeave(pcsOtherGuild, pcsGuild->m_szID, stNewLeaderUnit.m_szGuildID);

					SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_OTHER_GUILD, pcsOtherGuild, pcsGuild->m_szID);

					// ���࿡ size �� 1�� �ȴٸ� Clear ���ش�. ������ü�ΰŴ�.
					if(pcsOtherGuild->m_csRelation.m_pJointVector->size() == 1)
					{
						m_pagpmGuild->ClearJointGuild(pcsOtherGuild);
						SendJointLeave(pcsOtherGuild, pcsOtherGuild->m_szID, stNewLeaderUnit.m_szGuildID);
						EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pcsOtherGuild, pcsGuild->m_szID);
						SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_DESTROY, pcsOtherGuild);
					}
				}
			}

			// �̰� pcsGuild �ʿ��� ���°�.
			EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pcsGuild, iter->m_szGuildID);
		}

		++iter;
	}

	// Ŭ����
	m_pagpmGuild->ClearJointGuild(pcsGuild);
	
	// Joint Leave �� ��������� ������.
	SendJointLeave(pcsGuild, pcsGuild->m_szID, stNewLeaderUnit.m_szGuildID);

	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_JOINT_LEAVE_SUCCESS, pcsGuild);

	return TRUE;
}

// szGuildID �� ���¸� Ż���ߴ�.
// stNewLeaderUnit ���� ���Ӱ� �������� �� ��������� �´�.
BOOL AgsmGuild::JointLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, AgpdGuildRelationUnit& stNewLeaderUnit)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	// �ڱⰡ Ż���ϴ� ���� ���� ������ ������.
	if(_tcscmp(pcsGuild->m_szID, szGuildID) == 0)
		return FALSE;

	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		if(_tcscmp(iter->m_szGuildID, szGuildID) == 0)
		{
			// ������ ���� ����
			EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, pcsGuild->m_szID, szGuildID);
			iter = pcsGuild->m_csRelation.m_pJointVector->erase(iter);
			continue;
		}

		if(_tcscmp(iter->m_szGuildID, stNewLeaderUnit.m_szGuildID) == 0)
		{
			// ���ο� ������ ��� ����
			iter->m_cRelation= (INT8)AGPMGUILD_RELATION_JOINT_LEADER;
			EnumCallback(AGSMGUILD_CB_DB_JOINT_UPDATE, pcsGuild, stNewLeaderUnit.m_szGuildID);
		}

		++iter;
	}

	return TRUE;
}

BOOL AgsmGuild::HostileRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// szGuildID �� szJointGuildID ���� ��û�� ����.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild1, szMasterID))
		return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));

	INT32 lCheck = CheckHostileEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		SendSystemMessage(lCheck, ulMasterNID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID);

	// ��Ŷ ����.
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendHostileRequest(pcsGuild2, pcsGuild1->m_szID, pcsGuild1->m_szMasterID, ulMaster2NID);

	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_WAIT, ulMasterNID);

	return TRUE;
}

BOOL AgsmGuild::HostileReject(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// ��û�� szHostileGuildID �� �� ����.
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);
	if(!pcsGuild2)
		return FALSE;

	AuAutoLock csLock2(pcsGuild2->m_Mutex);
	if (!csLock2.Result()) return FALSE;

	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_REJECT, ulMaster2NID);

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild2);	// ������ ó�� ��� �ʱ�ȭ
	return TRUE;
}

BOOL AgsmGuild::Hostile(CHAR* szGuildID, AgpdGuildRelationUnit* pUnit)
{
	if(!szGuildID || !pUnit)
		return FALSE;

	// szGuildID �� ��û���� ���̴�. ��� �ٲ۴�.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(pUnit->m_szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));

	// �ٽ��ѹ� �Ǵ� �� �ȵǴ� �� üũ
	INT32 lCheck = CheckHostileEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		// ���⼭�� ���� ������ ������ �ʰ�, �� ���з� ������.
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	// ����� �°��� Ȯ�����ְ�
	if(!m_pagpmGuild->IsLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID))
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1);	// ������ ó�� ��� �ʱ�ȭ

	// �� ���� ��¥ �־��ش�.
	BOOL bResult = TRUE;
	UINT32 ulDate = m_pagpmGuild->GetCurrentTimeStamp();
	
	bResult &= m_pagpmGuild->AddHostileGuild(pcsGuild1, pcsGuild2->m_szID, ulDate);
	bResult &= m_pagpmGuild->AddHostileGuild(pcsGuild2, pcsGuild1->m_szID, ulDate);

	if(!bResult)
	{
		m_pagpmGuild->RemoveHostileGuild(pcsGuild1, pcsGuild2->m_szID);
		m_pagpmGuild->RemoveHostileGuild(pcsGuild2, pcsGuild1->m_szID);

		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	// �����ߴ�.
	SendHostile(pcsGuild1, pcsGuild2->m_szID, ulDate);
	SendHostile(pcsGuild2, pcsGuild1->m_szID, ulDate);

	SendHostileDetail(pcsGuild1, pcsGuild2);
	SendHostileDetail(pcsGuild2, pcsGuild1);

	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_SUCCESS, pcsGuild1);
	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_SUCCESS, pcsGuild2);

	// DB �� ���ش�.
	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_INSERT, pcsGuild1, pcsGuild2->m_szID);
	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_INSERT, pcsGuild2, pcsGuild1->m_szID);

	return TRUE;
}

BOOL AgsmGuild::HostileLeaveRequest(CHAR* szGuildID, CHAR* szHostileGuildID, CHAR* szMasterID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// szGuildID �� szJointGuildID ���� ��û�� ����.

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild1, szMasterID))
		return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));

	INT32 lCheck = CheckHostileLeaveEnable(pcsGuild1, pcsGuild2);
	if(lCheck != -1)
	{
		SendSystemMessage(lCheck, ulMasterNID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID);

	// ��Ŷ ����.
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendHostileLeaveRequest(pcsGuild2, pcsGuild1->m_szID, pcsGuild1->m_szMasterID, ulMaster2NID);

	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_WAIT, ulMasterNID);

	return TRUE;
}

BOOL AgsmGuild::HostileLeaveReject(CHAR* szGuildID, CHAR* szHostileGuildID)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// ��û�� szHostileGuildID �� �� ����.
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szHostileGuildID);
	if(!pcsGuild2)
		return FALSE;

	AuAutoLock csLock2(pcsGuild2->m_Mutex);
	if (!csLock2.Result()) return FALSE;

	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));
	SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_REJECT, ulMaster2NID);

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild2);	// ������ ó�� ��� �ʱ�ȭ
	return TRUE;
}

BOOL AgsmGuild::HostileLeave(CHAR* szGuildID, CHAR* szHostileGuildID, BOOL bForce)
{
	if(!szGuildID || !szHostileGuildID)
		return FALSE;

	// szHostileGuildID �� ��û�� �����.
	
	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(szHostileGuildID);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(szGuildID);

	if(!pcsGuild1 || !pcsGuild2)
		return FALSE;

	AuAutoLock csLock1(pcsGuild1->m_Mutex);
	AuAutoLock csLock2(pcsGuild2->m_Mutex);

	if (!csLock1.Result() || !csLock2.Result()) return FALSE;

	UINT32 ulMasterNID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild1));
	UINT32 ulMaster2NID = GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2));

	// ����� �°��� Ȯ�����ְ�
	if(!bForce && !m_pagpmGuild->IsLastRelationWaitGuildID(pcsGuild1, pcsGuild2->m_szID))
	{
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_FAILURE, ulMasterNID);
		SendSystemMessage(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_FAILURE, ulMaster2NID);
		return FALSE;
	}

	m_pagpmGuild->SetLastRelationWaitGuildID(pcsGuild1);	// ������ ó�� ��� �ʱ�ȭ

	// ���ָ� �ȴ�.
	m_pagpmGuild->RemoveHostileGuild(pcsGuild1, pcsGuild2->m_szID);
	m_pagpmGuild->RemoveHostileGuild(pcsGuild2, pcsGuild1->m_szID);

	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, pcsGuild1, pcsGuild2->m_szID);
	EnumCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, pcsGuild2, pcsGuild1->m_szID);

	SendHostileLeave(pcsGuild1, pcsGuild2->m_szID);
	SendHostileLeave(pcsGuild2, pcsGuild1->m_szID);

	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_SUCCESS, pcsGuild1);
	SendSystemMessageToAllMembers(AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_SUCCESS, pcsGuild2);

	return TRUE;
}

// pcsGuild1 �� ��û�� ���, pcsGuild2 �� ��û �޴� ����̴�.
INT32 AgsmGuild::CheckJointEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if(!pcsGuild1 || !pcsGuild2)
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;

	if(m_pagpmGuild->IsJointGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_ALREADY_JOINT;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;
	if(m_pagpmGuild->GetJointGuildCount(pcsGuild1) >= AGPMGUILD_MAX_JOINT_GUILD)
		return AGPMGUILD_SYSTEM_CODE_JOINT_MAX;
	if(m_pagpmGuild->GetJointGuildCount(pcsGuild1) > 0 &&	// �̹� ���뿡 ������ �ִµ�, �������� �ƴϴ�.
		!m_pagpmGuild->IsJointLeader(pcsGuild1))
		return AGPMGUILD_SYSTEM_CODE_JOINT_NOT_LEADER;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild1) < AGPMGUILD_MIN_JOINT_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_JOINT_NOT_ENOUGH_MEMBER;
#endif

	if(m_pagpmGuild->IsJointGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;
	if(m_pagpmGuild->GetJointGuildCount(pcsGuild2) >= AGPMGUILD_MAX_JOINT_GUILD)		// �̹� ���뿡 ������ �ִٸ� GG
		return AGPMGUILD_SYSTEM_CODE_JOINT_DISABLE;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild2) < AGPMGUILD_MIN_JOINT_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_JOINT_OHTER_NOT_ENOUGH_MEMBER;
#endif

	//if(GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2)) == 0)	// ��� �渶�� ��������.
	//	return AGPMGUILD_SYSTEM_CODE_JOINT_MASTER_OFFLINE;

	AgpdCharacter* pcsMaster2 = m_pagpmCharacter->GetCharacter(pcsGuild2->m_szMasterID);
	if(!pcsMaster2 || pcsMaster2->m_bIsReadyRemove)
		return AGPMGUILD_SYSTEM_CODE_JOINT_MASTER_OFFLINE;

	if(pcsMaster2->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION;

	return -1;
}

// pcsGuild1 �� ��û�� ���, pcsGuild2 �� ��û �޴� ����̴�.
INT32 AgsmGuild::CheckHostileEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if(!pcsGuild1 || !pcsGuild2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;

	if(m_pagpmGuild->IsJointGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild1, pcsGuild2->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_ALREADY_HOSTILE;
	if(m_pagpmGuild->GetHostileGuildCount(pcsGuild1) >= AGPMGUILD_MAX_HOSTILE_GUILD)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_MAX;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild1) < AGPMGUILD_MIN_HOSTILE_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_NOT_ENOUGH_MEMBER;
#endif

	if(m_pagpmGuild->IsJointGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;
	if(m_pagpmGuild->IsHostileGuild(pcsGuild2, pcsGuild1->m_szID))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;
	if(m_pagpmGuild->GetHostileGuildCount(pcsGuild2) >= AGPMGUILD_MAX_HOSTILE_GUILD)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_DISABLE;

#ifndef _DEBUG
	if(m_pagpmGuild->GetMemberCount(pcsGuild2) < AGPMGUILD_MIN_HOSTILE_MEMBER_COUNT)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_OTHER_NOT_ENOUGH_MEMBER;
#endif

	//if(GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2)) == 0)	// ��� �渶�� ��������.
	//	return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	AgpdCharacter* pcsMaster2 = m_pagpmCharacter->GetCharacter(pcsGuild2->m_szMasterID);
	if(!pcsMaster2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	if(pcsMaster2->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION;

	return -1;
}

INT32 AgsmGuild::CheckHostileLeaveEnable(AgpdGuild* pcsGuild1, AgpdGuild* pcsGuild2)
{
	if(!pcsGuild1 || !pcsGuild2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_DISABLE;

	// �Ѵ� ���� ������迩���� �� ���� ��û�� �ϴ� ���� �Ѵ�.
	if(!(m_pagpmGuild->IsHostileGuild(pcsGuild1, pcsGuild2->m_szID) && m_pagpmGuild->IsHostileGuild(pcsGuild2, pcsGuild1->m_szID)))
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_LEAVE_DISABLE;

	//if(GetMemberNID(m_pagpmGuild->GetMaster(pcsGuild2)) == 0)	// ��� �渶�� ��������.
	//	return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	AgpdCharacter* pcsMaster2 = m_pagpmCharacter->GetCharacter(pcsGuild2->m_szMasterID);
	if(!pcsMaster2)
		return AGPMGUILD_SYSTEM_CODE_HOSTILE_MASTER_OFFLINE;

	if(pcsMaster2->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_GUILD_RELATION)
		return AGPMGUILD_SYSTEM_CODE_REFUSE_RELATION;

	return -1;
}

// 2006.08.31. steeple
// ����-���� ���� ��ȿ�� üũ. ���� �� �� �ѹ��� ���ش�.
BOOL AgsmGuild::CheckValidationForRelation()
{
	// ��� ��ü�� ���鼭 ��ȿ�� üũ���ش�.
	AgpdGuild** ppcsGuild = NULL;
	INT32 lIndex = 0;
	for(ppcsGuild = (AgpdGuild**)m_pagpmGuild->m_csGuildAdmin.GetObjectSequence(&lIndex); ppcsGuild;
		ppcsGuild = (AgpdGuild**)m_pagpmGuild->m_csGuildAdmin.GetObjectSequence(&lIndex))
	{
		if(!*ppcsGuild)
			continue;

		AuAutoLock Lock1((*ppcsGuild)->m_Mutex);
		if (!Lock1.Result()) continue;

		JointIter iter = (*ppcsGuild)->m_csRelation.m_pJointVector->begin();
		while(iter != (*ppcsGuild)->m_csRelation.m_pJointVector->end())
		{
			if(_tcscmp((*ppcsGuild)->m_szID, iter->m_szGuildID) != 0 &&
				!m_pagpmGuild->IsJointGuild(iter->m_szGuildID, (*ppcsGuild)->m_szID))
			{
				EnumCallback(AGSMGUILD_CB_DB_JOINT_DELETE, *ppcsGuild, iter->m_szGuildID);
				iter = (*ppcsGuild)->m_csRelation.m_pJointVector->erase(iter);
				continue;
			}
			
			++iter;
		}

		HostileIter iter2 = (*ppcsGuild)->m_csRelation.m_pHostileVector->begin();
		while(iter2 != (*ppcsGuild)->m_csRelation.m_pHostileVector->end())
		{
			if(_tcscmp((*ppcsGuild)->m_szID, iter2->m_szGuildID) != 0 &&
				!m_pagpmGuild->IsHostileGuild(iter2->m_szGuildID, (*ppcsGuild)->m_szID))
			{
				EnumCallback(AGSMGUILD_CB_DB_HOSTILE_DELETE, *ppcsGuild, iter2->m_szGuildID);
				iter2 = (*ppcsGuild)->m_csRelation.m_pHostileVector->erase(iter2);
				continue;
			}

			++iter2;
		}
	}

	return TRUE;
}

BOOL AgsmGuild::SendJointRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJointRequestPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, szMasterID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return bResult;
}

BOOL AgsmGuild::SendJoint(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, INT8 cRelation, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJointPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, ulDate, cRelation);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendJointLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szNewLeader, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildJointLeavePacket(&nPacketLength, 1, pcsGuild->m_szID, szGuildID, szNewLeader);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostileRequestPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, szMasterID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return bResult;
}

BOOL AgsmGuild::SendHostile(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulDate, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostilePacket(&nPacketLength, pcsGuild->m_szID, szGuildID, ulDate);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileLeaveRequest(AgpdGuild* pcsGuild, CHAR* szGuildID, CHAR* szMasterID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostileLeaveRequestPacket(&nPacketLength, pcsGuild->m_szID, szGuildID, szMasterID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return bResult;
}

BOOL AgsmGuild::SendHostileLeave(AgpdGuild* pcsGuild, CHAR* szGuildID, UINT32 ulNID)
{
	if(!pcsGuild || !szGuildID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmGuild->MakeGuildHostileLeavePacket(&nPacketLength, 1, pcsGuild->m_szID, szGuildID);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendJointAll(AgpdGuild* pcsGuild, UINT32 ulNID)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket[AGPMGUILD_MAX_JOINT_GUILD] = {0,};

	INT32 lCount = 0;
	JointIter iter = pcsGuild->m_csRelation.m_pJointVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pJointVector->end())
	{
		// Joint ������
		SendJoint(pcsGuild, iter->m_szGuildID, iter->m_ulDate, iter->m_cRelation, ulNID);

		AgpdGuild* pcsJoint = m_pagpmGuild->GetGuild(iter->m_szGuildID);
		if(!pcsJoint)
		{
			++iter;
			continue;
		}

		AuAutoLock csLock(pcsJoint->m_Mutex);

		if (csLock.Result())
		{
			pvGuildListItemPacket[lCount] = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsJoint);
			if(++lCount >= AGPMGUILD_MAX_JOINT_GUILD)
				break;
		}

		++iter;
	}

	if(lCount == 0)
		return TRUE;

	nPacketLength = 0;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket[0],
											pvGuildListItemPacket[1],
											pvGuildListItemPacket[2],
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL
											);

	for(int i = 0; i < AGPMGUILD_MAX_JOINT_GUILD; ++i)
	{
		if( pvGuildListItemPacket[i] )
			m_pagpmGuild->m_csGuildListItemPacket.FreePacket( pvGuildListItemPacket[i] );
	}

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_JOINT_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket, 
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileAll(AgpdGuild* pcsGuild, UINT32 ulNID)
{
	if(!pcsGuild)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket[AGPMGUILD_MAX_HOSTILE_GUILD] = {0,};

	INT32 lCount = 0;
	HostileIter iter = pcsGuild->m_csRelation.m_pHostileVector->begin();
	while(iter != pcsGuild->m_csRelation.m_pHostileVector->end())
	{
		SendHostile(pcsGuild, iter->m_szGuildID, iter->m_ulDate, ulNID);

		AgpdGuild* pcsHostile = m_pagpmGuild->GetGuild(iter->m_szGuildID);
		if(!pcsHostile)
		{
			++iter;
			continue;
		}

		AuAutoLock csLock(pcsHostile->m_Mutex);
		if (csLock.Result())
		{
			pvGuildListItemPacket[lCount] = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsHostile);
			if(++lCount >= AGPMGUILD_MAX_HOSTILE_GUILD)
				break;
		}

		++iter;
	}

	if(lCount == 0)
		return TRUE;

	nPacketLength = 0;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket[0],
											pvGuildListItemPacket[1],
											pvGuildListItemPacket[2],
											pvGuildListItemPacket[3],
											NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL,
											NULL,
											//pvGuildListItemPacket[4],
											//pvGuildListItemPacket[5],
											//pvGuildListItemPacket[6],
											//pvGuildListItemPacket[7],
											//pvGuildListItemPacket[8],
											//pvGuildListItemPacket[9],
											//pvGuildListItemPacket[10],
											//pvGuildListItemPacket[11],
											//pvGuildListItemPacket[12],
											//pvGuildListItemPacket[13],
											//pvGuildListItemPacket[14],
											NULL,
											NULL,
											NULL,
											NULL
											);

	for(int i = 0; i < AGPMGUILD_MAX_HOSTILE_GUILD; ++i)
	{
		if(pvGuildListItemPacket[i])
			m_pagpmGuild->m_csGuildListItemPacket.FreePacket(pvGuildListItemPacket[i]);
	}

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket, 
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendJointDetail(AgpdGuild* pcsGuild, AgpdGuild* pcsJoint, UINT32 ulNID)
{
	if(!pcsGuild || !pcsJoint)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsJoint);

	if(!pvGuildListItemPacket || nPacketLength < 1)
		return FALSE;

	nPacketLength = 0;
	INT32 lCount = 1;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL
											);
	m_pagpmGuild->m_csGuildListItemPacket.FreePacket( pvGuildListItemPacket );
	if(!pvGuildListPacket || nPacketLength < 1)
		return FALSE;

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_JOINT_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket,
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::SendHostileDetail(AgpdGuild* pcsGuild, AgpdGuild* pcsHostile, UINT32 ulNID)
{
	if(!pcsGuild || !pcsHostile)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvGuildListItemPacket = m_pagpmGuild->MakeGuildListPacket(&nPacketLength, pcsHostile);
	if(!pvGuildListItemPacket || nPacketLength < 1)
		return FALSE;

	nPacketLength = 0;
	INT32 lCount = 1;
	INT32 lPage = 1;
	INT16 nCount = (INT16)lCount;

	PVOID pvGuildListPacket = m_pagpmGuild->m_csGuildListPacket.MakePacket(FALSE, &nPacketLength, 0,
											&lCount,
											&lPage,
											&nCount,
											pvGuildListItemPacket,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL,
											NULL, NULL, NULL, NULL, NULL, NULL
											);
	m_pagpmGuild->m_csGuildListItemPacket.FreePacket( pvGuildListItemPacket );
	if(!pvGuildListPacket || nPacketLength < 1)
		return FALSE;

	INT32 lCID = 1;
	INT8 cOperation = AGPMGUILD_PACKET_HOSTILE_DETAIL;
	PVOID pvPacket = m_pagpmGuild->MakeGuildPacket(TRUE, &nPacketLength, AGPMGUILD_PACKET_TYPE, &cOperation, &lCID,
									pcsGuild->m_szID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL,
									NULL, NULL, NULL, NULL, NULL, pvGuildListPacket, 
									NULL, NULL, NULL);
	m_pagpmGuild->m_csGuildListPacket.FreePacket(pvGuildListPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	if(ulNID)
		SendPacket(pvPacket, nPacketLength, ulNID);
	else
		SendPacketToAllMembers(pcsGuild, pvPacket, nPacketLength);

	m_pagpmGuild->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}

BOOL AgsmGuild::OnOperationWorldChampionshipRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST* pPacket = (PACKET_GUILD_WORLDCHAMPIONSHIP_REQUEST*)pData;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pPacket->lCID);
	if(!pcsCharacter)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pagpmGuild->GetGuild(pcsCharacter);
	if(!pcsGuild || !pThis->m_pagpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
	{
		// ��帶���Ͱ� �ƴϹǷ� ����è�Ǿ� ���� ��û�� �Ҽ� ����
		AgsdCharacter* pagsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(!pagsdCharacter)
			return FALSE;

		AuXmlElement* pElem = pThis->GetNoticeNodeWorldChampionship()->FirstChildElement("Notice_03");

		if(pElem)
		{
			PACKET_GUILD_WORLDCHAMPIONSHIP_RESULT pPacket2;
			strcpy(pPacket2.strMessage, pElem->GetText());
			
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdCharacter->m_dpnidCharacter);
		}

		return FALSE;
	}

	PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_REQUEST pRelayPacket(pcsCharacter->m_szID, pcsGuild->m_szID);

	AgsdServer* pRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

BOOL AgsmGuild::OnOperationWorldChampionshipEnter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER* pPacket = (PACKET_GUILD_WORLDCHAMPIONSHIP_ENTER*)pData;

	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(pPacket->lCID);
	if(!pcsCharacter)
		return FALSE;

	AgsdServer *pAgsdServer = pThis->m_pagsmServerManager->GetAimEventServer();
	if (NULL == pAgsdServer)
		return FALSE;

	PACKET_AGSMGUILD_RELAY_WORLD_CHAMPIONSHIP_ENTER pRelayPacket(pcsCharacter->m_szID, pAgsdServer->m_lServerID, pAgsdServer->m_szWorld);

	AgsdServer* pRelayServer = pThis->m_pagsmServerManager->GetRelayServer();
	if (!pRelayServer || !pRelayServer->m_bIsConnected)
		return FALSE;

	AgsEngine::GetInstance()->SendPacket(pRelayPacket, pRelayServer->m_dpnidServer);

	return TRUE;
}

AuXmlNode* AgsmGuild::GetNoticeNodeWorldChampionship()
{
	static AuXmlNode* m_pNoticeNode = NULL;

	if(!m_pNoticeNode)
		m_pNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("WorldChampionship");

	return m_pNoticeNode;
}

BOOL AgsmGuild::CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmGuild		*pThis			= (AgsmGuild *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	//return pThis->SendAllMail(pAgpdCharacter, ulNID);

	// PC �� �ƴϸ� ������.
	if(pThis->m_pagpmCharacter->IsPC(pcsCharacter) == FALSE)
		return TRUE;

	AgpdGuildADChar* pcsAttachedGuild = pThis->m_pagpmGuild->GetADCharacter(pcsCharacter);
	if(!pcsAttachedGuild)
		return FALSE;

	// ������ �ƴϸ� ������.
	if(strlen(pcsAttachedGuild->m_szGuildID) == 0)
		return TRUE;

	pThis->SendCharGuildData(pcsAttachedGuild->m_szGuildID, pThis->m_pagsmCharacter->GetRealCharName(pcsCharacter->m_szID), NULL, 1, ulNID);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
AuXmlNode* AgsmGuild::GetNoticeNodeCSAppointment()
{
	static AuXmlNode* m_pNoticeNode = NULL;

	if(!m_pNoticeNode)
		m_pNoticeNode = m_pagsmSystemMessage->GetRootNode()->FirstChildElement("ClassSociety");

	return m_pNoticeNode;
}

BOOL AgsmGuild::OnOperationCSAppointmentRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST* pPacket = (PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_REQUEST*)pData;

	switch(pPacket->Rank)
	{
		case AGPMGUILD_MEMBER_RANK_NORMAL:
			{
				pThis->GuildMemberDismissal(pPacket->strSChar, pPacket->strTChar, pPacket->Rank);
			} break;
		case AGPMGUILD_MEMBER_RANK_SUBMASTER:
			{
				pThis->GuildMemberAppointmentAsk(pPacket->strSChar, pPacket->strTChar, pPacket->Rank);
			} break;
		default:
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmGuild::OnOperationCSAppointmentAnswer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass /*|| !pCustData*/)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER* pPacket = (PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ANSWER*)pData;	
	
	// ������ üũ
	AgpdCharacter* pcsMaster = pThis->m_pagpmCharacter->GetCharacter(pPacket->strMasterID);
	AgpdCharacter* pcsMember = pThis->m_pagpmCharacter->GetCharacter(pPacket->strChar);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	AgpdGuild* pcsGuild1 = pThis->m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = pThis->m_pagpmGuild->GetGuild(pcsMember);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	if(!pThis->m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	AgsdCharacter* pagsdMaster = pThis->m_pagsmCharacter->GetADCharacter(pcsMaster);
	AgsdCharacter* pagsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
	if(!pagsdMaster || !pagsdMember)
		return FALSE;

	if(pPacket->Answer == FALSE)
	{
		// �Ӹ��� �ź�������
		AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_01");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	INT32 nRank = pPacket->Rank;
	switch(nRank)
	{
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			if(strlen(pcsGuild1->m_szSubMasterID) > 0)
			{
				// �α渶�� �̹� ������
				AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_02");
				if(pElem)
				{
					PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
					AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
				}

				return TRUE;
			}
		} break;
	default:
		{
			return FALSE;
		} break;
	}

	if(! pThis->GuildMemberRankUpdate(pcsMember, nRank))
		return FALSE;

	AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_03");
	if(pElem)
	{
		PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
		AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
	}

	return TRUE;
}

BOOL AgsmGuild::GuildMemberRankUpdate(AgpdCharacter* pcsCharacter, INT32 nRank)
{
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pcsCharacter);
	if(!pcsGuild)
		return FALSE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, pcsCharacter->m_szID);
	if(!pcsGuildMember)
		return FALSE;

	INT32 nBeforeRank = pcsGuildMember->m_lRank;

	// ��ũ����
	pcsGuildMember->m_lRank = nRank;
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuild->m_szID, pcsGuildMember);

	switch(nRank)
	{
		case AGPMGUILD_MEMBER_RANK_SUBMASTER:
			{
				memcpy(pcsGuild->m_szSubMasterID, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
			} break;
		case AGPMGUILD_MEMBER_RANK_MASTER:
			{
				if(nBeforeRank != AGPMGUILD_MEMBER_RANK_SUBMASTER)
				{
					pcsGuildMember->m_lRank = nBeforeRank;
					EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuildMember->m_szID, pcsGuildMember);
					return FALSE;
				}

				memcpy(pcsGuild->m_szMasterID, pcsGuildMember->m_szID, AGPACHARACTER_MAX_ID_STRING);	
				ZeroMemory(pcsGuild->m_szSubMasterID, AGPACHARACTER_MAX_ID_STRING);	
			} break;
	}

	// ��� ������ ������Ʈ
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);

	PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK pUpdatePacket(pcsGuild->m_szID, pcsCharacter->m_szID, nRank);
	SendPacketToAllMembers(pcsGuild, pUpdatePacket);

	return TRUE;
}

BOOL AgsmGuild::GuildMemberRankUpdate(AgpdGuild* pcsGuild, AgpdGuildMember* pcsGuildMember, INT32 nRank)
{
	if(!pcsGuildMember || !pcsGuild)
		return FALSE;

	INT32 nBeforeRank = pcsGuildMember->m_lRank;

	// ��ũ����
	pcsGuildMember->m_lRank = nRank;
	EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuildMember->m_szID, pcsGuildMember);

	switch(nRank)
	{
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			memcpy(pcsGuild->m_szSubMasterID, pcsGuildMember->m_szID, AGPACHARACTER_MAX_ID_STRING);
		} break;
	case AGPMGUILD_MEMBER_RANK_MASTER:
		{
			if(nBeforeRank != AGPMGUILD_MEMBER_RANK_SUBMASTER)
			{
				pcsGuildMember->m_lRank = nBeforeRank;
				EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuildMember->m_szID, pcsGuildMember);
				return FALSE;
			}

			memcpy(pcsGuild->m_szMasterID, pcsGuildMember->m_szID, AGPACHARACTER_MAX_ID_STRING);	
			ZeroMemory(pcsGuild->m_szSubMasterID, AGPACHARACTER_MAX_ID_STRING);		
		} break;
	}

	// ��� ������ ������Ʈ
	EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);

	PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK pUpdatePacket(pcsGuild->m_szID, pcsGuildMember->m_szID, nRank);
	SendPacketToAllMembers(pcsGuild, pUpdatePacket);

	return TRUE;
}

BOOL AgsmGuild::OnOperationCSSuccessionRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmGuild* pThis		= static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST *pvPacket = (PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_REQUEST*)(pData);
	
	AgpdCharacter *pcsMaster = pThis->m_pagpmCharacter->GetCharacter(pvPacket->strSChar);
	AgpdCharacter *pcsMember = pThis->m_pagpmCharacter->GetCharacter(pvPacket->strTChar);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	if(pThis->m_pagpmSiegeWar->GetSiegeWarByOwner(pcsMaster->m_szID))
		return FALSE;

	AgpdGuild* pcsGuild1 = pThis->m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = pThis->m_pagpmGuild->GetGuild(pcsMember);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	AgsdCharacter* pagsdMaster  = pThis->m_pagsmCharacter->GetADCharacter(pcsMaster);
	AgsdCharacter* pagsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
	if(!pagsdMember || !pagsdMaster)
		return FALSE;

	if(pThis->m_pagpmGuild->IsExceptionCharacterOfGuildRankSystem(pcsGuild2, pcsMember))
	{
		AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_05");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	// Check Master
	if(!pThis->m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	// Check SubMaster
	if(!pThis->m_pagpmGuild->IsSubMaster(pcsGuild1, pcsMember->m_szID))
		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ASK pPacket2(pcsMaster->m_szID, pcsGuild1->m_szID, pcsMember->m_szID);

	AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMember->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmGuild::OnOperationCSSuccessionAnswer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmGuild *pThis		    = static_cast<AgsmGuild*>(pClass);

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER *pPacket = (PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ANSWER*)(pData);

	AgpdCharacter *pcsMaster    = pThis->m_pagpmCharacter->GetCharacter(pPacket->strMasterID);
	AgpdCharacter *pcsSuccessor = pThis->m_pagpmCharacter->GetCharacter(pPacket->strChar); 

	// Check Being Character
	if(!pcsMaster || !pcsSuccessor)
		return FALSE;

	// Check Being Guild and Same Guild
	AgpdGuild* pcsGuild1 = pThis->m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = pThis->m_pagpmGuild->GetGuild(pcsSuccessor);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	// Check Master
	if(!pThis->m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	// Check SubMaster
	if(!pThis->m_pagpmGuild->IsSubMaster(pcsGuild1, pcsSuccessor->m_szID))
		return FALSE;

	AgsdCharacter* pagsdMaster = pThis->m_pagsmCharacter->GetADCharacter(pcsMaster);
	if(!pagsdMaster)

		return FALSE;
	
	// Refuse a succession 
	if(!pPacket->Answer)
	{
		AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_01");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}
	
	if(! pThis->GuildMemberRankUpdate(pcsMaster, AGPMGUILD_MEMBER_RANK_NORMAL))
		return FALSE;
	
	if(! pThis->GuildMemberRankUpdate(pcsSuccessor, AGPMGUILD_MEMBER_RANK_MASTER))
		return FALSE;

	AuXmlElement* pElem = pThis->GetNoticeNodeCSAppointment()->FirstChildElement("Notice_04");
	if(pElem)
	{
		PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
		AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
	}
	
	return TRUE;
}

// ��� ����� pPacket�� �ִ� Rank�� �Ӹ��ϱ�.
BOOL AgsmGuild::GuildMemberAppointmentAsk(CHAR *strMaster, CHAR *strMember, INT32 nRank)
{
	// ������ üũ
	AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacter(strMaster);
	AgpdCharacter* pcsMember = m_pagpmCharacter->GetCharacter(strMember);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	AgpdGuild* pcsGuild1 = m_pagpmGuild->GetGuild(pcsMaster);
	AgpdGuild* pcsGuild2 = m_pagpmGuild->GetGuild(pcsMember);
	if(!pcsGuild1 || !pcsGuild2 || pcsGuild1->m_lID != pcsGuild2->m_lID)
		return FALSE;

	AgsdCharacter* pagsdMaster = m_pagsmCharacter->GetADCharacter(pcsMaster);
	AgsdCharacter* pagsdMember = m_pagsmCharacter->GetADCharacter(pcsMember);
	if(!pagsdMaster || !pagsdMember)
		return FALSE;

	if(m_pagpmGuild->IsExceptionCharacterOfGuildRankSystem(pcsGuild2, pcsMember))
	{
		AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_05");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	if(!m_pagpmGuild->IsMaster(pcsGuild1, pcsMaster->m_szID))
		return FALSE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild2, pcsMember->m_szID);
	if(!pcsGuildMember)
		return FALSE;

	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ASK pPacket2(pcsMaster->m_szID, pcsGuild1->m_szID, pcsMember->m_szID, nRank);

	AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMember->m_dpnidCharacter);

	return TRUE;	
}

// ��� ����� AGPMGUILD_MEMBER_RANK_NORMAL�� ���ӽ��Ѷ�
BOOL AgsmGuild::GuildMemberDismissal(CHAR *strMaster, CHAR *strMember, INT32 nRank)
{
	// ������ üũ
	AgpdCharacter* pcsMaster = m_pagpmCharacter->GetCharacter(strMaster);
	AgpdCharacter* pcsMember = m_pagpmCharacter->GetCharacter(strMember);
	if(!pcsMaster || !pcsMember)
		return FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pcsMaster);
	if(!pcsGuild)
		return FALSE;

	if(!m_pagpmGuild->IsMaster(pcsGuild, pcsMaster->m_szID))
		return FALSE;

	AgsdCharacter* pagsdMaster = m_pagsmCharacter->GetADCharacter(pcsMaster);
	if(!pagsdMaster)
		return FALSE;

	AgpdGuildMember* pcsGuildMember = m_pagpmGuild->GetMember(pcsGuild, strMember);
	if(!pcsGuildMember)
		return FALSE;

	INT32 Rank = nRank;
	if( pcsGuildMember->m_lRank == Rank)
	{
		AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_02");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	if(m_pagpmGuild->IsExceptionCharacterOfGuildRankSystem(pcsGuild, pcsMember))
	{
		AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_05");
		if(pElem)
		{
			PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
			AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
		}

		return TRUE;
	}

	INT32 nBeforRank = pcsGuildMember->m_lRank;

	GuildMemberRankUpdate(pcsGuild, pcsGuildMember, Rank);

	switch(nBeforRank)
	{
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:
		{
			memset(pcsGuild->m_szSubMasterID, 0, sizeof(pcsGuild->m_szSubMasterID));
		} break;
	}

	AuXmlElement* pElem = GetNoticeNodeCSAppointment()->FirstChildElement("Notice_03");
	if(pElem)
	{
		PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE pPacket2((CHAR*)pElem->GetText());
		AgsEngine::GetInstance()->SendPacket(pPacket2, pagsdMaster->m_dpnidCharacter);
	}

	return TRUE;
}

void AgsmGuild::LoadWinnerGuild()
{
	AuXmlDocument pDoc;

	if(!pDoc.LoadFile("WinnerGuild.xml"))
		return;

	AuXmlNode* pFileNode = pDoc.FirstChild("WinnerGuild");
	if(!pFileNode)
		return;

	for(AuXmlElement* pWinner = pFileNode->FirstChildElement("WINNER"); pWinner; pWinner = pWinner->NextSiblingElement())
	{
		CHAR* pszGuild = (CHAR*)pWinner->GetText();
		INT32 lRanking = 0;
		pWinner->Attribute("RANK", &lRanking);

		if(pszGuild && lRanking != WINNER_GUILD_NOTRANKED)
		{
			szWinnerGuildName[lRanking].SetText(pszGuild);
			SetWinnerGuild(pszGuild, lRanking);
		}
	}
}

void AgsmGuild::SaveWinnerGuild()
{
	AuXmlDocument pDoc;

	string encoding = "euc-kr";

	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", encoding.c_str(), "no" ); 
	pDoc.LinkEndChild( decl ); 

	AuXmlElement * root = new AuXmlElement( "WinnerGuild" ); 
	pDoc.LinkEndChild( root );

	for(INT32 it = WINNER_GUILD_NOTRANKED; it < WINNER_GUILD_MAX; ++it)
	{
		if(szWinnerGuildName[it].IsEmpty())
			continue;

		AuXmlElement * pWinnerGuild = new AuXmlElement( "WINNER" ); 
		if( pWinnerGuild )
		{
			pWinnerGuild->LinkEndChild(new TiXmlText(szWinnerGuildName[it].GetBuffer()));
			pWinnerGuild->SetAttribute("RANK", it);

			root->LinkEndChild( pWinnerGuild );
		}
	}

	pDoc.SaveFile("WinnerGuild.xml");
}
