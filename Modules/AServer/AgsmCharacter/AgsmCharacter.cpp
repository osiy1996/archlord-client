/******************************************************************************
Module:  AgsmCharacter.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 03
******************************************************************************/

#include "AgsmCharacter.h"
#include "AgsdCharacter.h"

#include "AuMath.h"

#include "ApLockManager.h"

#include <stdio.h>
#include "ApMemoryTracker.h"
#include "AuTimeStamp.h"
#include "AgpmArchlord.h"
#include "AuGameEnv.h"
#include "AgsmLogin.h"
#include "AgpmEpicZone.h"
#include "AgsmTitle.h"
#include "AgpmAI2.h"
#include "AgsmLoginClient.h"

#include <AuLua.h>

#ifdef	__PROFILE__
#include "API_AuFrameProfile.h"
#endif

extern	INT32	g_alReceivedSkillPoint[2][3];

INT32				g_lNumberOfProcessors	= 1;

INT32*				g_plNumOfDispatchedIdle;
CRITICAL_SECTION*	g_pcsDispatchedIdle;
LONG				g_ulIndexDispatchThread	= 0;

BOOL				g_bPrintNumCharacter	= 0;

UINT WINAPI AgsmCharacter::AgsmCharacterDispatcher(PVOID pvParam)
{
	AgsmCharacter* pThis = (AgsmCharacter*)pvParam;

	DWORD			dwTransferred = 0;
	LPOVERLAPPED	pOverlapped = NULL;
	AsObject		*pObject = NULL;

	INT32			lThreadIndex	= InterlockedIncrement(&g_ulIndexDispatchThread) - 1;

	INT32			lLoopCount		= 0;

	while (TRUE)
	{
		pThis->m_AuIOCPDispatcher.GetStatus((PULONG_PTR)&pObject, &dwTransferred, &pOverlapped, INFINITE);

		AgpdCharacter	*pcsCharacter = (AgpdCharacter*)pObject;

		if (pcsCharacter->m_Mutex.WLock())
		{
			pThis->m_pcsAgpmCharacter->CharacterIdle(pcsCharacter, pThis->GetClockCount());

			pcsCharacter->m_Mutex.Release();
		}

		pcsCharacter->m_bIsPostedIdleProcess	= FALSE;

		++lLoopCount;

		if (lLoopCount == 10)
		{
			EnterCriticalSection(&g_pcsDispatchedIdle[lThreadIndex]);
			g_plNumOfDispatchedIdle[lThreadIndex] -= lLoopCount;
			LeaveCriticalSection(&g_pcsDispatchedIdle[lThreadIndex]);

			lLoopCount	= 0;
		}

		if (ApLockManager::Instance()->IsExistData())
			ApLockManager::Instance()->AllUnlock();
	}

	return 0;
}

AgsmCharacter::AgsmCharacter()
{
	SetModuleName("AgsmCharacter");

	EnableIdle(TRUE);
	EnableIdle2(TRUE);

	SetModuleType(APMODULE_TYPE_SERVER);

	SetPacketType(AGSMCHARACTER_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,		// operation
							AUTYPE_INT32,			1,		// character id
							AUTYPE_INT8,			1,		// operation�� ���� status
							AUTYPE_CHAR,			23,		// zone server address (xxx.xxx.xxx.xxx:port)
							AUTYPE_INT32,			1,		// zone server id
							AUTYPE_PACKET,			1,		// ������ ĳ���� ����Ÿ.
							AUTYPE_PACKET,			1,		// Ban Data - 2004.03.30. steeple
							AUTYPE_INT32,			1,		// New Character ID
							AUTYPE_END,				0);

	m_csPacketServerData.SetFlagLength(sizeof(INT32));
	m_csPacketServerData.SetFieldType(
							AUTYPE_INT32,			1,		// ó���ϰ� �ִ� server id
							AUTYPE_CHAR,			AGSMACCOUNT_MAX_ACCOUNT_NAME + 1,	// �� ĳ���� ���� �̸�
							AUTYPE_UINT32,			1,		// m_ulPrevSendMoveTime
							AUTYPE_UINT32,			1,		// m_ulNextAttackTime
							AUTYPE_UINT32,			1,		// m_ulNextSkillTime
							AUTYPE_INT8,			1,		// m_bDestroyWhenDie
							AUTYPE_PACKET,			1,		// m_stHistory
							AUTYPE_UINT64,			1,		// m_ullDBID
							AUTYPE_CHAR,			AGSM_CHARACTER_MAX_SHOUT_WORD + 1,	// m_szLastShoutWord
							AUTYPE_UINT32,			1,		// m_ulLastUpdateMukzaPointTime
							AUTYPE_UINT32,			1,		// m_ulRemainUpdateMukzaPointTime
							AUTYPE_INT32,			1,		// m_lReceivedSkillPoint
							AUTYPE_UINT32,			1,		// m_ulLastUpdateActionStatusTime
							AUTYPE_UINT32,			1,		// current clock count
							AUTYPE_CHAR,			AGPD_TELEPORT_MAX_POINT_NAME + 1,		// LastUseTeleportPointName
							AUTYPE_CHAR,			AGSM_MAX_SERVER_NAME + 1,		// (��������)���� ������ �̸�.
							AUTYPE_INT32,			1,		// �Ϻ�: ���� ��ȣ
							AUTYPE_END,				0);

	m_csPacketHistory.SetFlagLength(sizeof(INT8));
	m_csPacketHistory.SetFieldType(
							AUTYPE_INT16,			1,		// m_lEntryNum
							AUTYPE_PACKET,			AGSD_CHARACTER_HISTORY_NUMBER,		// m_astEntry
							AUTYPE_END,				0);

	m_csPacketHistoryEntry.SetFlagLength(sizeof(INT8));
	m_csPacketHistoryEntry.SetFieldType(
							AUTYPE_PACKET,			1,		// m_csSource (Base packet)
							AUTYPE_PACKET,			1,		// m_csUpdateFactor
							AUTYPE_UINT32,			1,		// m_ulFirstHitTime
							AUTYPE_UINT32,			1,		// m_ulLastHitTime
							AUTYPE_INT8,			1,		// m_bFirstHit
							AUTYPE_FLOAT,			1,		// m_fAgro
							AUTYPE_END,				0);

	m_csPacketBase.SetFlagLength(sizeof(INT8));
	m_csPacketBase.SetFieldType(
							AUTYPE_INT8,			1,		// base type
							AUTYPE_INT32,			1,		// base id
							AUTYPE_END,				0
							);
	
	// 2004.03.30. steeple
	m_csPacketBanData.SetFlagLength(sizeof(INT8));
	m_csPacketBanData.SetFieldType(
							AUTYPE_INT32,			1,		// ChatBan StartTime
							AUTYPE_INT8,			1,		// ChatBan KeepTime
							AUTYPE_INT32,			1,		// CharBan StartTime
							AUTYPE_INT8,			1,		// CharBan KeepTime
							AUTYPE_INT32,			1,		// AccBan StartTime
							AUTYPE_INT8,			1,		// AccBan KeepTime
							AUTYPE_END,				0
							);

	m_csPacketCoupon.SetFlagLength(sizeof(INT8));
	m_csPacketCoupon.SetFieldType(AUTYPE_CHAR,			AGSMACCOUNT_MAX_ACCOUNT_NAME + 1,	// �� ĳ���� ���� �̸�
								  AUTYPE_MEMORY_BLOCK,		1,		// Coupon Info
								  AUTYPE_END,				0
								 );

	m_pagpmLog = NULL;
	m_pagsmLogin = NULL;

	m_ulSendMoveDelay			= AGSMCHARACTER_DEFAULT_SEND_MOVE_DELAY;
	m_ulHistoryRefreshDelay		= AGSMCHARACTER_DEFAULT_HISTORY_REFRESH_DELAY;
	m_ulPrevHistoryRefreshTime	= 0;
	m_ulNextSaveDBTime			= 0;
	m_ulNextProcessIdleTime		= 0;
	
	m_ulNextProcessIdleTime3	= 0;

#ifdef _USE_NPROTECT_GAMEGUARD_
	m_ulNextProcessIdleTimeGameGuard = (GetGameEnv().IsDebugTest()) ? CTime(2100, 1, 1, 0, 0, 0) : 0;
#endif

	m_bReloadLua				= TRUE;
	memset(&m_CheckCharacterValue, 0, sizeof(m_CheckCharacterValue));
	
	m_bUseIOCPDispatcher		= FALSE;

	m_ulPreviousClock			= 0;

	SYSTEM_INFO		stSystemInfo;

	GetSystemInfo(&stSystemInfo);

	g_lNumberOfProcessors		= stSystemInfo.dwNumberOfProcessors;

//	g_plNumOfDispatchedIdle		= (INT32 *) malloc(sizeof(INT32) * g_lNumberOfProcessors);
	g_plNumOfDispatchedIdle		= (INT32 *) new BYTE[(sizeof(INT32) * g_lNumberOfProcessors)];
	g_pcsDispatchedIdle			= (CRITICAL_SECTION *) new BYTE[(sizeof(CRITICAL_SECTION) * g_lNumberOfProcessors)];

	for (int i = 0; i < g_lNumberOfProcessors; ++i)
	{
		g_plNumOfDispatchedIdle[i]	= 0;
		InitializeCriticalSection(&g_pcsDispatchedIdle[i]);
	}

	ZeroMemory(m_alNumPlayerByClass, sizeof(LONG) * AURACE_TYPE_MAX * AUCHARCLASS_TYPE_MAX);

	m_csMutexNumOfPlayers.Init();
	m_lNumOfPlayers	= 0;
}

AgsmCharacter::~AgsmCharacter()
{
	for (int i = 0; i < g_lNumberOfProcessors; ++i)
		DeleteCriticalSection(&g_pcsDispatchedIdle[i]);

//	free(g_pcsDispatchedIdle);
//	free(g_plNumOfDispatchedIdle);
	delete [] (BYTE*)(g_pcsDispatchedIdle);
	delete [] (BYTE*)(g_plNumOfDispatchedIdle);

	m_csMutexNumOfPlayers.Destroy();
}

BOOL AgsmCharacter::OnAddModule()
{
	m_papmMap = (ApmMap *) GetModule("ApmMap");
	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmSkill = (AgpmSkill *) GetModule("AgpmSkill");
	m_pcsAgpmParty = (AgpmParty *) GetModule("AgpmParty");
	m_pagpmAdmin = (AgpmAdmin*)GetModule("AgpmAdmin");	// 2004.03.05. steeple
	m_pagpmUIStatus = (AgpmUIStatus *) GetModule("AgpmUIStatus");
	m_pagpmLog = (AgpmLog*)GetModule("AgpmLog");	// 2004.05.18. steeple
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	//m_pagpmOptimizedPacket = (AgpmOptimizedPacket *) GetModule("AgpmOptimizedPacket");
	m_pagpmOptimizedPacket2 = (AgpmOptimizedPacket2 *) GetModule("AgpmOptimizedPacket2");
	m_pcsAgpmEventBinding = (AgpmEventBinding *) GetModule("AgpmEventBinding");
	m_pcsAgpmEventTeleport = (AgpmEventTeleport *) GetModule("AgpmEventTeleport");
	m_pcsAgpmPvP = (AgpmPvP *) GetModule("AgpmPvP");
	m_pcsAgpmScript = (AgpmScript *) GetModule("AgpmScript");
	m_pcsAgpmSummons = (AgpmSummons *) GetModule("AgpmSummons");
	m_pcsAgpmGuild = (AgpmGuild *) GetModule("AgpmGuild");
	m_pcsAgpmItemConvert = (AgpmItemConvert*)GetModule("AgpmItemConvert");
	m_pagpmConfig = (AgpmConfig*)GetModule("AgpmConfig");

	m_pagsmSystemMessage = (AgsmSystemMessage *) GetModule("AgsmSystemMessage");

	//m_pagsmDBStream = (AgsmDBStream *) GetModule("AgsmDBStream");
	m_pagsmFactors = (AgsmFactors *) GetModule("AgsmFactors");
	m_pagsmAOIFilter = (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pAgsmServerManager2 = (AgsmServerManager2 *) GetModule("AgsmServerManager2");
	m_pagsmAccountManager = (AgsmAccountManager *) GetModule("AgsmAccountManager");
	//m_pagsmCharManager = (AgsmCharManager *) GetModule("AgsmCharManager");
	m_pagsmMap = (AgsmMap *) GetModule("AgsmMap");
	m_pagsmInterServerLink = (AgsmInterServerLink *) GetModule("AgsmInterServerLink");
	m_pcsAgpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");
	
	// JNY TODO : Relay ���� ������ ���� m_pagsmDBStream�� üũ�ϴ� �κ��� 
	// ��� �����մϴ�. 
	// 2004.2.16
	// �ٽ� ���� �ϰų� DB��� ���濡 ���� �����۾��� �ؾ��մϴ�.

	if (!m_papmMap || !m_pagpmFactors || !m_pcsAgpmCharacter || !m_pcsAgpmSkill || !m_pcsAgpmParty || !m_pagsmFactors || !m_pagsmAOIFilter || 
		/*!m_pagsmDBStream ||*/ !m_pAgsmServerManager2 || !m_pagsmMap || !m_pagsmAccountManager /*|| !m_pagsmCharManager*/ ||
		!m_pagpmAdmin || !m_pagpmUIStatus || !m_pagpmItem /*|| !m_pagpmOptimizedPacket*/ || !m_pcsAgpmEventTeleport ||
		!m_pcsAgpmPvP || /*!m_pcsAgpmScript ||*/ !m_pcsAgpmSummons || !m_pcsAgpmGuild || !m_pagsmInterServerLink || !m_pcsAgpmArchlord)
		return FALSE;

	// setting attached data(m_nKeeping) in character module
	m_nIndexADCharacter = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgsdCharacter), ConAgsdCharacter, DesAgsdCharacter);
	if (m_nIndexADCharacter < 0)
		return FALSE;

	/*
	if (m_pagsmDBStream)
	{
		if (!m_pagsmDBStream->SetCallbackCharacterResult(CBStreamDB, this))
			return FALSE;
	}
	*/

	/*
	if (!m_pagsmAOIFilter->SetCallbackAddChar(CBAddSector, this))
		return FALSE;

	if (!m_pagsmAOIFilter->SetCallbackRemoveChar(CBRemoveSector, this))
		return FALSE;

	if (!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveSector, this))
		return FALSE;
	*/

	if (!m_pagsmAOIFilter->SetCallbackAddChar(CBAddCell, this))
		return FALSE;
	if (!m_pagsmAOIFilter->SetCallbackRemoveChar(CBRemoveCell, this))
		return FALSE;
	if (!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveCell, this))
		return FALSE;

	if (!m_pagsmAOIFilter->SetCallbackGetDPNID(CBGetDPNID, this))
		return FALSE;

	if (!m_pagsmSystemMessage->SetCallbackGetDPNID(CBGetDPNID, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackMoveChar(CBMoveCharacter, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackStopChar(CBStopCharacter, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdatePosition(CBUpdatePosition, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateMoney(CBUpdateMoney, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateBankMoney(CBUpdateBankMoney, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateCash(CBUpdateCash, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackMoveBankMoney(CBMoveBankMoney, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateStatus(CBUpdateStatus, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateLevel(CBUpdateLevel, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateActionStatus, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateCriminalStatus(CBUpdateCriminalStatus, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackInitChar(CBInitCharacter, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackResetMonster(CBInitCharacter, this))
		return FALSE;

	/*
	if (!m_pcsAgpmCharacter->AddPreReceiveModule(this))
		return FALSE;
	*/

	if (!m_pcsAgpmCharacter->SetCallbackUpdateFactor(CBUpdateFactor, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackTransformAppear(CBTransformAppear, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackTransformStatus(CBTransformStatus, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRestoreTransform(CBRestoreTransform, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackCancelTransform(CBCancelTransform, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackEvolution(CBEvolution, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRestoreEvolution(CBRestoreEvolution, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateMurdererPoint(CBUpdateMurdererPoint, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateMukzaPoint(CBUpdateMukzaPoint, this))
		return FALSE;
		
	if (!m_pcsAgpmCharacter->SetCallbackUpdateCharismaPoint(CBUpdateCharismaPoint, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateSkillPoint(CBUpdateSkillPoint, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackAddAttackerToList(CBAddAttackerToList, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveChar, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackPayActionCost(CBPayActionCost, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackCheckActionAttackTarget(CBCheckActionTargetAttack, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackIsPlayerCharacter(CBIsPlayerCharacter, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackBindingRegionChange(CBUpdateRegionIndex, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackSocialAnimation(CBSocialAnimation, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateSpecialStatus(CBUpdateSpecialStatus, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackCheckAllBlockStatus(CBCheckAllBlock, this))
		return FALSE;

	if (!m_pagpmFactors->SetCallbackUpdateFactorView(CBUpdateFactorView, this))
		return FALSE;
	if (!m_pagpmFactors->SetCallbackUpdateFactorMovement(CBUpdateFactorMovement, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackCheckProcessIdle(CBCheckProcessIdle, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackReleaseActionMoveLock(CBReleaseActionMove, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateCustomize(CBUpdateCustomizeIndex, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateOptionFlag(CBUpdateOptionFlag, this))		//2005.05.31 By SungHoon
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateBankSize(CBUpdateBankSize, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackBlockByPenalty(CBBlockByPenalty, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackAddStaticCharacter(CBAddStaticCharacter, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRemoveProtectedNPC(CBRemoveProtectedNPC, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackGetBonusDropRate(CBGetBonusDropRate, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackGetBonusDropRate2(CBGetBonusDropRate2, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackGetBonusMoneyRate(CBGetBonusMoneyRate, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackGetBonusCharismaRate(CBGetBonusCharismaRate, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackRequestCoupon(CBRequestCoupon, this))
		return FALSE;

	if (!m_pagpmItem->SetCallbackChangeAutoPickUp(CBChangeAutoPickItem, this))
		return FALSE;

	// register data backup callback functions
	//////////////////////////////////////////////////////////////////
	//if (!m_pcsAgpmCharacter->SetCallbackUpdateLevel(CBBackupCharacterData, this))
	//	return FALSE;
	//if (!m_pcsAgpmCharacter->SetCallbackUpdateCriminalStatus(CBBackupCharacterData, this))
	//	return FALSE;
	//if (!m_pcsAgpmCharacter->SetCallbackUpdateMurdererPoint(CBBackupCharacterData, this))
	//	return FALSE;

	if (!m_pagsmAccountManager->SetCallbackResetNID(CBResetNID, this))
		return FALSE;
	if (!m_pagsmAccountManager->SetCallbackRemoveCharacter(CBAccountRemoveCharacter, this))
		return FALSE;

	if (!m_pagsmInterServerLink->SetCallbackGetCurrentUser(CBGetCurrentUser, this))
		return FALSE;

	//9999���� ĳ�������� �������ִ�.
	m_csCertificatedAccountID.InitializeObject( sizeof(INT32), 9999 );

	m_lFactorTypeIndex	= m_pagpmFactors->SetFactorDataName(_T("HistoryEntry"));

	return TRUE;
}

BOOL AgsmCharacter::OnInit()
{
	m_pagsmLogin =		(AgsmLogin*)GetModule("AgsmLogin");
	m_pagsmTitle =		(AgsmTitle*)GetModule("AgsmTitle");
	m_pagpmAI2	 =		(AgpmAI2*)GetModule("AgpmAI2");

	if (m_pagsmLogin)
	{
		if(!m_pagsmLogin->SetCallbackCharacterRenamed(CBRenameCharacterID, this))
			return FALSE;
	}

	m_csAdminCheckRecvChar.InitializeObject(sizeof(INT32) * 3, m_csAdminCheckRecvChar.GetCount());
	m_csAdminWaitForRemoveChar.InitializeObject(sizeof(INT32), m_csAdminWaitForRemoveChar.GetCount());

	if (m_bUseIOCPDispatcher)
	{
		m_pcsAgpmCharacter->EnableIdle(FALSE);

		InitIOCPDispatcher(g_lNumberOfProcessors);
	}

	m_pcsAgpmCharacter->SetCallbackGameguardAuth(CBGameGuardAuth, this);

	AddTimer(1 * 60 * 1000, 0, this, CheckStaticCharacter, NULL);

	EnableIdle3(TRUE);
	
	return TRUE;
}

BOOL AgsmCharacter::OnDestroy()
{
	if (m_bUseIOCPDispatcher)
	{
		m_AuIOCPDispatcher.Destroy();
	}

	return TRUE;
}

BOOL AgsmCharacter::OnIdle(UINT32 lClock)
{
	PROFILE("AgsmCharacter::OnIdle");

	if (m_bUseIOCPDispatcher && (lClock - m_ulPreviousClock > 300))
	{
		INT32	lMinIndex	= 0;
		INT32	lMinValue	= 0;

		INT32	lTotalQueuedEvent	= 0;
		INT32	lQueuedEvent		= 0;

		for (int i = 0; i < g_lNumberOfProcessors; ++i)
		{
			EnterCriticalSection(&g_pcsDispatchedIdle[i]);
			lQueuedEvent	= g_plNumOfDispatchedIdle[i];
			LeaveCriticalSection(&g_pcsDispatchedIdle[i]);

			if (i == 0)
				lMinValue	= lQueuedEvent;

			if (lMinValue > lQueuedEvent)
			{
				lMinValue	= lQueuedEvent;
				lMinIndex	= i;
			}

			lTotalQueuedEvent	+= lQueuedEvent;
		}

		if (lTotalQueuedEvent > 1000)
		{
			OutputDebugString("=================== Skip Idle Process ======================\n");
			for (int i = 0; i < g_lNumberOfProcessors; ++i)
			{
				EnterCriticalSection(&g_pcsDispatchedIdle[i]);
				TRACE("g_plNumOfDispatchedIdle[%d] : %d\n", i, g_plNumOfDispatchedIdle[i]);
				LeaveCriticalSection(&g_pcsDispatchedIdle[i]);
			}
			
			return TRUE;
		}

		INT32 lCharacterNumBuffer = 0;

		INT32			lIndex	= 0;
		AgpdCharacter	*pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);

		INT32			lNumDispatchedIdle	= 0;

		INT32			lNumOfPlayers	= 0;

		while (pcsCharacter)
		{
			if (!pcsCharacter->m_Mutex.WLock())
			{
				pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
				continue;
			}

#ifdef _M_X64
			AgsmCellUnit	*pstCellUnit	= m_pagsmAOIFilter->GetADCharacter(pcsCharacter);
			if (pstCellUnit && pstCellUnit->pcsCurrentCell && pstCellUnit->pcsCurrentCell->IsAnyActiveCell())
#endif
			{
				++lCharacterNumBuffer;

				if (m_pcsAgpmCharacter->IsPC(pcsCharacter) || (!pcsCharacter->m_bIsPostedIdleProcess && CheckProcessIdle(pcsCharacter, lClock)))
				{
					pcsCharacter->m_bIsPostedIdleProcess	= TRUE;

					m_AuIOCPDispatcher.Dispatch(pcsCharacter);

					++lNumDispatchedIdle;
				}
			}
#ifdef _M_X64
			else
			{
				if (pcsCharacter->m_bMove)
				{
					m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
				}
			}
#endif

			if (GetCharDPNID(pcsCharacter) > 0)
				++lNumOfPlayers;

			pcsCharacter->m_Mutex.Release();

			pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		}

		if (m_csMutexNumOfPlayers.WLock())
		{
			m_lNumOfPlayers	= lNumOfPlayers;
			m_csMutexNumOfPlayers.Release();
		}

		EnterCriticalSection(&g_pcsDispatchedIdle[lMinIndex]);
		g_plNumOfDispatchedIdle[lMinIndex] += lNumDispatchedIdle;
		LeaveCriticalSection(&g_pcsDispatchedIdle[lMinIndex]);

		m_ulPreviousClock	= lClock;

		if (lClock > m_pcsAgpmCharacter->m_ulPrevClockCount)
			m_pcsAgpmCharacter->m_ulPrevClockCount = lClock;

		if (g_bPrintNumCharacter)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "TotalCount : %d\n   TotalRemovePool : %d\n", lCharacterNumBuffer, m_pcsAgpmCharacter->GetRemovePoolCount());
			AuLogFile_s("LOG\\CharacterCount.txt", strCharBuff);

			g_bPrintNumCharacter	= FALSE;
		}
	}

	return TRUE;
}

BOOL AgsmCharacter::OnIdle2(UINT32 lClock)
{
	PROFILE("AgsmCharacter::OnIdle2");

	if (m_ulNextProcessIdleTime > lClock)
		return TRUE;

//	AgsdServer	*pcsThisServer	= m_pAgsmServerManager2->GetThisServer();
//	if (pcsThisServer)
//	{
//		switch ((eAGSMSERVER_TYPE) pcsThisServer->m_cType) {
//		case AGSMSERVER_TYPE_LOGIN_SERVER:
			{
				INT32			lIndex	= 0;
				INT32			*plCID	= (INT32 *) m_csAdminWaitForRemoveChar.GetObjectSequence(&lIndex);

				while (plCID)
				{
					AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(*plCID);
					if (pcsCharacter)
					{
//						BOOL	bRemove	= FALSE;

						AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

						AgsdCharacterCBWaitOperation	stWaitOperationArg;
						ZeroMemory(&stWaitOperationArg, sizeof(AgsdCharacterCBWaitOperation));

						stWaitOperationArg.lCID			= *plCID;
						stWaitOperationArg.lTID			= pcsCharacter->m_lTID1;
						stWaitOperationArg.lServerID	= pcsAgsdCharacter->m_ulServerID;
						stWaitOperationArg.dpnidClient	= pcsAgsdCharacter->m_dpnidCharacter;

						strncpy(stWaitOperationArg.szAccountName, pcsAgsdCharacter->m_szAccountID, AGSMACCOUNT_MAX_ACCOUNT_NAME);
						strncpy(stWaitOperationArg.szCharName, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

						BOOL	bIsTimeout	= IsWaitTimeout(pcsCharacter, lClock);
						BOOL	bIsContWait	= IsWaitOperation(pcsCharacter);

//						if (!IsWaitOperation(pcsCharacter) || IsWaitTimeout(pcsCharacter, lClock))
//						{
//							bRemove	= TRUE;
//						}

						if (!bIsContWait || bIsTimeout)
						{
							m_pagsmAccountManager->RemoveAccount(pcsAgsdCharacter->m_szAccountID);

							pcsAgsdCharacter->m_dpnidCharacter	= 0;

							if (pcsAgsdCharacter->m_lOldCID != AP_INVALID_CID)
								pcsCharacter->m_lID	= pcsAgsdCharacter->m_lOldCID;
						}

						pcsCharacter->m_Mutex.Release();

						if (!bIsContWait || bIsTimeout)
						{
							m_csAdminWaitForRemoveChar.RemoveObject(stWaitOperationArg.lCID);
							lIndex = 0;

							if (m_pcsAgpmCharacter->RemoveCharacter(stWaitOperationArg.lCID))
							{
								printf(" AgsmCharacter::OnIdle() remove character (%s)\n", stWaitOperationArg.szCharName);
							}

							//if (bIsTimeout)
								EnumCallback(AGSMCHARACTER_CB_REMOVE_WAIT_OPERATION, &stWaitOperationArg, &bIsTimeout);

							DestroyClient(stWaitOperationArg.dpnidClient, DISCONNNECT_REASON_CLIENT_NORMAL);
							//DestroyClient(stWaitOperationArg.dpnidClient);
						}
					}
					else
					{
						m_csAdminWaitForRemoveChar.RemoveObject(*plCID);
						lIndex = 0;
					}

					plCID = (INT32 *) m_csAdminWaitForRemoveChar.GetObjectSequence(&lIndex);
				}
			}
//			break;
//
//		case AGSMSERVER_TYPE_GAME_SERVER:
//			{
//			}
//			break;
//		}
//	}

	m_ulNextProcessIdleTime	+= AGSMCHARACTER_PROCESS_IDLE_TIME_INTERVAL;

	return TRUE;
}

BOOL AgsmCharacter::OnIdle3(UINT32 lClock)
{
	CTime time = CTime::GetCurrentTime();

#ifdef _USE_NPROTECT_GAMEGUARD_
	if( m_ulNextProcessIdleTimeGameGuard < time)
	{
		INT32			lIndex	= 0;
		
		for(AgpdCharacter*pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
			pcsCharacter;
			pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex))
		{
			if(!m_pcsAgpmCharacter->IsPC(pcsCharacter) || pcsCharacter->m_szID[0] == '\0')
				continue;

			AgsdCharacter *pagsdCharacter = GetADCharacter(pcsCharacter);
			if(!pagsdCharacter || !pagsdCharacter->csa)
				continue;

			if(pagsdCharacter->m_GGLastQueryTime + CTimeSpan(0, 0, 5, 0) > time)
				continue;

			DWORD dwRet = pagsdCharacter->csa->GetAuthQuery();
			if(dwRet == ERROR_SUCCESS)
			{
				GG_AUTH_DATA ggData;
				memcpy(&ggData, &pagsdCharacter->csa->m_AuthQuery, sizeof(GG_AUTH_DATA));

				INT16 packetLength;
				PVOID pvPacket = m_pcsAgpmCharacter->MakePacketGameguardAuth( pcsCharacter, &ggData, sizeof(GG_AUTH_DATA), &packetLength );
				if( pvPacket )
				{
					SendPacket(pvPacket, packetLength, GetCharDPNID(pcsCharacter));
					m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
				}

				pagsdCharacter->m_GGLastQueryTime = time;
			}
			else
			{
				{
					CHAR strLog[256] = { 0, };
					sprintf_s(strLog, sizeof(strLog), "GameGuard GetAuthQuery Error - [%s][%d]\n", pcsCharacter->m_szID, dwRet);
					AuLogFile_s("LOG\\GameGuard.txt", strLog);
				}
				
				switch(dwRet)
				{
				case ERROR_GGAUTH_INVALID_PROTOCOL_VERSION:
				case ERROR_GGAUTH_INVALID_GAMEGUARD_VER:
					{
						DestroyClient(pagsdCharacter->m_dpnidCharacter);
					} break;
				}
			}
		}

		m_ulNextProcessIdleTimeGameGuard = time + CTimeSpan(0, 0, 1, 0);
	}
#endif

	if(m_ulNextProcessIdleTime3 > lClock)
		return TRUE;

	LoadCheckCharacterLua();

	if(m_CheckCharacterValue.bUse)
	{
		INT32			lIndex	= 0;
		AgpdCharacter	*pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);

		while (pcsCharacter)
		{
			if(m_pcsAgpmCharacter->IsPC(pcsCharacter) && pcsCharacter->m_szID[0])
			{
				INT32 lMaxHP = 0;
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				if(lMaxHP >= m_CheckCharacterValue.MaxHP)
				{
					CHAR strLogBuff[256] = { 0, };
					sprintf_s(strLogBuff, sizeof(strLogBuff), "Character : %s, MaxHP : %d", pcsCharacter->m_szID, lMaxHP);
					AuLogFile_s("LOG\\CharacterCheckLog.txt", strLogBuff);
				}

				INT32 lMaxMP = 0;
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				if(lMaxMP >= m_CheckCharacterValue.MaxMP)
				{
					CHAR strLogBuff[256] = { 0, };
					sprintf_s(strLogBuff, sizeof(strLogBuff), "Character : %s, MaxMP : %d", pcsCharacter->m_szID, lMaxMP);
					AuLogFile_s("LOG\\CharacterCheckLog.txt", strLogBuff);
				}

				INT32 lAttackSpeed = 0;
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lAttackSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
				if(lAttackSpeed >= m_CheckCharacterValue.AttackSpeed)
				{
					CHAR strLogBuff[256] = { 0, };
					sprintf_s(strLogBuff, sizeof(strLogBuff), "Character : %s, lAttackSpeed : %d", pcsCharacter->m_szID, lAttackSpeed);
					AuLogFile_s("LOG\\CharacterCheckLog.txt", strLogBuff);
				}

				/*INT32 fPhysicalDefense = 0;
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &fPhysicalDefense, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
				if(fPhysicalDefense >= m_CheckCharacterValue.fPhysicalDefense)
				{
					CHAR strLogBuff[256] = { 0, };
					sprintf_s(strLogBuff, sizeof(strLogBuff), "Character : %s, fPhysicalDefense : %d", pcsCharacter->m_szID, fPhysicalDefense);
					AuLogFile_s("LOG\\CharacterCheckLog.txt", strLogBuff);
				}*/

				INT32 fPhysicalResistance = 0;
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &fPhysicalResistance, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
				if(fPhysicalResistance >= m_CheckCharacterValue.PhysicalResistance)
				{
					CHAR strLogBuff[256] = { 0, };
					sprintf_s(strLogBuff, sizeof(strLogBuff), "Character : %s, fPhysicalResistance : %d", pcsCharacter->m_szID, fPhysicalResistance);
					AuLogFile_s("LOG\\CharacterCheckLog.txt", strLogBuff);
				}

				INT32	fAttackMaxDmg	= 0;
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &fAttackMaxDmg, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
				if(fAttackMaxDmg >= m_CheckCharacterValue.fAttackMaxDmg)
				{
					CHAR strLogBuff[256] = { 0, };
					sprintf_s(strLogBuff, sizeof(strLogBuff), "Character : %s, fAttackMaxDmg : %d", pcsCharacter->m_szID, fAttackMaxDmg);
					AuLogFile_s("LOG\\CharacterCheckLog.txt", strLogBuff);
				}

				INT32 lMoveSpeed = 0;
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMoveSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
				if(lMoveSpeed >= m_CheckCharacterValue.lMoveSpeed)
				{
					CHAR strLogBuff[256] = { 0, };
					sprintf_s(strLogBuff, sizeof(strLogBuff), "Character : %s, lMoveSpeed : %d", pcsCharacter->m_szID, lMoveSpeed);
					AuLogFile_s("LOG\\CharacterCheckLog.txt", strLogBuff);
				}
			}
			
			pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
		}
	}
	
	m_ulNextProcessIdleTime3 = lClock + (5 * 60 * 1000);

	return TRUE;
}

BOOL AgsmCharacter::LoadCheckCharacterLua()
{
	if(m_bReloadLua)
	{
		m_bReloadLua = FALSE;

		lua_State* pLuaState =lua_open();
		luaL_openlibs(pLuaState);

		int status = luaL_loadfile(pLuaState, "CONFIG\\CheckCharacter.Lua") || lua_pcall(pLuaState, 0, 0, 0);
		if(status != 0)
			return FALSE;

		lua_getglobal(pLuaState, "CheckCharacterValue");
		if(!lua_istable(pLuaState, -1))
			return FALSE;

		lua_getfield(pLuaState, -1, "use");
		m_CheckCharacterValue.bUse = (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);
		
		lua_getfield(pLuaState, -1, "MaxHP");
		m_CheckCharacterValue.MaxHP = (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);

		lua_getfield(pLuaState, -1, "MaxMP");
		m_CheckCharacterValue.MaxMP = (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);

		lua_getfield(pLuaState, -1, "AttackMaxDmg");
		m_CheckCharacterValue.fAttackMaxDmg  = (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);

		lua_getfield(pLuaState, -1, "AttackSpeed");
		m_CheckCharacterValue.AttackSpeed  = (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);
		
		/*lua_getfield(pLuaState, -1, "PhysicalDefense");
		m_CheckCharacterValue.fPhysicalDefense  = (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);*/

		lua_getfield(pLuaState, -1, "PhysicalResistance");
		m_CheckCharacterValue.PhysicalResistance  = (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);
		
		lua_getfield(pLuaState, -1, "MoveSpeed");
		m_CheckCharacterValue.lMoveSpeed	= (INT32)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);

		lua_close(pLuaState);
	}
	
	return TRUE;
}

BOOL AgsmCharacter::InitIOCPDispatcher(INT32 lThreadCount)
{
	return m_AuIOCPDispatcher.CreateWalkerThread(lThreadCount, AgsmCharacterDispatcher, (PVOID)this, "CharacterDispatch");
}

void AgsmCharacter::SetIOCPDispatcherUse(BOOL bUse)
{
	m_bUseIOCPDispatcher = bUse;
}
/*

BOOL AgsmCharacter::OnIdle2(UINT32 lClock)
{
	PROFILE("AgsmCharacter::OnIdle2");

	if (m_ulNextProcessIdleTime > lClock)
		return TRUE;

	AgsdServer	*pcsThisServer	= m_pagsmServerManager->GetThisServer();
	if (pcsThisServer && pcsThisServer->m_pcsTemplate)
	{
		switch ((eAGSMSERVER_TYPE) ((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_cType) {
		case AGSMSERVER_TYPE_LOGIN_SERVER:
			{
				INT32			lIndex	= 0;
				INT32			*plCID	= (INT32 *) m_csAdminWaitForRemoveChar.GetObjectSequence(&lIndex);

				while (plCID)
				{
					AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(*plCID);
					if (pcsCharacter)
					{
						BOOL	bRemove	= FALSE;

						AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

						AgsdCharacterCBWaitOperation	stWaitOperationArg;
						ZeroMemory(&stWaitOperationArg, sizeof(AgsdCharacterCBWaitOperation));

						stWaitOperationArg.lCID			= *plCID;
						stWaitOperationArg.lTID			= pcsCharacter->m_lTID1;
						stWaitOperationArg.lServerID	= pcsAgsdCharacter->m_ulServerID;
						stWaitOperationArg.dpnidClient	= pcsAgsdCharacter->m_dpnidCharacter;

						strncpy(stWaitOperationArg.szAccountName, pcsAgsdCharacter->m_szAccountID, AGSMACCOUNT_MAX_ACCOUNT_NAME);
						strncpy(stWaitOperationArg.szCharName, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

						if (!IsWaitOperation(pcsCharacter) || IsWaitTimeout(pcsCharacter, lClock))
						{
							bRemove	= TRUE;
						}

						if (bRemove)
						{
							m_pagsmAccountManager->RemoveAccount(pcsAgsdCharacter->m_szAccountID);

							pcsAgsdCharacter->m_dpnidCharacter	= 0;

							if (pcsAgsdCharacter->m_lOldCID != AP_INVALID_CID)
								pcsCharacter->m_lID	= pcsAgsdCharacter->m_lOldCID;
						}

						pcsCharacter->m_Mutex.Release();

						if (bRemove)
						{
							m_csAdminWaitForRemoveChar.RemoveObject(stWaitOperationArg.lCID);

							if (m_pcsAgpmCharacter->RemoveCharacter(stWaitOperationArg.lCID))
							{
								printf(" AgsmCharacter::OnIdle() remove character (%s)\n", stWaitOperationArg.szCharName);
							}

							EnumCallback(AGSMCHARACTER_CB_REMOVE_WAIT_OPERATION, &stWaitOperationArg, NULL);

							DestroyClient(stWaitOperationArg.dpnidClient, DISCONNNECT_REASON_CLIENT_NORMAL);
							//DestroyClient(stWaitOperationArg.dpnidClient);
						}
					}

					plCID	= (INT32 *) m_csAdminWaitForRemoveChar.GetObjectSequence(&lIndex);
				}
			}
			break;

		case AGSMSERVER_TYPE_GAME_SERVER:
			{
			}
			break;
		}
	}

	m_ulNextProcessIdleTime	+= AGSMCHARACTER_PROCESS_IDLE_TIME_INTERVAL;

	return TRUE;
}

*/

BOOL AgsmCharacter::OnValid(CHAR* szData, INT16 nSize)
{
	return TRUE;
}

void AgsmCharacter::SetSendMoveDelay(UINT32 ulDelay)
{
	m_ulSendMoveDelay = ulDelay;
}

void AgsmCharacter::SetHistoryRefreshDelay(UINT32 ulDelay)
{
	m_ulHistoryRefreshDelay = ulDelay;
}

AgsdCharacter* AgsmCharacter::GetADCharacter(AgpdCharacter *pData)
{
	return (AgsdCharacter *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, pData);
}

//		SetServerID
//	Functions
//		- ulCID ĳ���Ͱ� ��� �������� �����ް� �ִ����� �����Ѵ�.
//	Arguments
//		- ulCID : character id
//		- ulServerID : server id
//	Return value
//		- BOOL : ���� ����
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::SetServerID(INT32 ulCID, INT32 ulServerID)
{
	AgpdCharacter *pCharacter = m_pcsAgpmCharacter->GetCharacter(ulCID);
	if (pCharacter == NULL)
		return FALSE;

	return SetServerID(pCharacter, ulServerID);
}

//		SetServerID
//	Functions
//		- pCharacter�� ��� �������� �����ް� �ִ����� �����Ѵ�.
//	Arguments
//		- pCharacter : character data pointer
//		- ulServerID : server id
//	Return value
//		- BOOL : ���� ����
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::SetServerID(AgpdCharacter *pCharacter, INT32 ulServerID)
{
	if (pCharacter == NULL || ulServerID == AP_INVALID_SERVERID)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = GetADCharacter(pCharacter);
	if (pAgsdCharacter == NULL)
		return FALSE;

	pAgsdCharacter->m_ulServerID = ulServerID;

	return TRUE;
}

//		GetCharDPNID
//	Functions
//		- get and return character dpnid
//	Arguments
//		- ulCID : character id
//	Return value
//		- DPNID : character dpnid
///////////////////////////////////////////////////////////////////////////////
DPNID AgsmCharacter::GetCharDPNID(INT32 ulCID)
{
	AgpdCharacter *pCharacter = m_pcsAgpmCharacter->GetCharacter(ulCID);
	if (pCharacter == NULL)
		return 0;

	return GetCharDPNID(pCharacter);
}

//		GetCharDPNID
//	Functions
//		- get and return character dpnid
//	Arguments
//		- pCharacter : character data pointer
//	Return value
//		- DPNID : character dpnid
///////////////////////////////////////////////////////////////////////////////
DPNID AgsmCharacter::GetCharDPNID(AgpdCharacter *pCharacter)
{
	if (pCharacter == NULL)
		return 0;

	AgsdCharacter *pcsCharacter = GetADCharacter(pCharacter);
	if (pcsCharacter == NULL)
		return 0;

	return (pcsCharacter->m_dpnidCharacter);
}

//		ConAgsdCharacter
//	Functions
//		- AgsdCharacter constructor
//	Arguments
//		- pData : character data pointer
//		- pClass : this class pointer
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::ConAgsdCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
	{
		// invalid constructor parameter
		return FALSE;
	}

	AgsmCharacter *pThis			= (AgsmCharacter *) pClass;
	AgpdCharacter *pCharacter		= (AgpdCharacter *) pData;
	AgsdCharacter *pAgsdCharacter	= pThis->GetADCharacter(pCharacter);

	ZeroMemory(pAgsdCharacter->m_szAccountID, sizeof(CHAR) * AGSMACCOUNT_MAX_ACCOUNT_NAME);
	
	ZeroMemory(pAgsdCharacter->m_szLastShoutWord, sizeof(CHAR) * AGSM_CHARACTER_MAX_SHOUT_WORD);

	pAgsdCharacter->m_ulServerID	= AP_INVALID_SERVERID;

	//pAgsdCharacter->m_bMoveSector = FALSE;

	pAgsdCharacter->m_bMoveCell			= FALSE;
	pAgsdCharacter->m_pcsPrevCell		= NULL;
	pAgsdCharacter->m_pcsCurrentCell	= NULL;

	pAgsdCharacter->m_ulNextAttackTime	= 0;
	pAgsdCharacter->m_ulNextSkillTime	= 0;

	pAgsdCharacter->m_ulLastUpdateActionStatusTime	= 0;

	ZeroMemory(&pAgsdCharacter->m_stHistory, sizeof(AgsdCharacterHistory));
	ZeroMemory(&pAgsdCharacter->m_stHistoryParty, sizeof(AgsdCharacterHistory));

	pAgsdCharacter->m_ulHistoryDuration = AGSD_CHARACTER_HISTORY_DEFAULT_DURATION;
	pAgsdCharacter->m_fHistoryRange = AGSD_CHARACTER_HISTORY_DEFAULT_RANGE;

	pAgsdCharacter->m_lReceivedSkillPoint				= 0;

	pAgsdCharacter->m_bWaitOperationBeforeRemove		= FALSE;
	pAgsdCharacter->m_ulMaxWaitTime						= 0;
	//ZeroMemory(pAgsdCharacter->m_bWaitOperation, sizeof(BOOL) * AGSMCHARACTER_WAIT_OPERATION_MAX);
	pAgsdCharacter->m_bWaitOperation.MemSetAll();

	pAgsdCharacter->m_ulReleaseStunStatusTime			= 0;
	pAgsdCharacter->m_ulReleaseFreezeStatusTime			= 0;
	pAgsdCharacter->m_ulReleaseSlowStatusTime			= 0;
	pAgsdCharacter->m_ulReleaseInvincibleStatusTime		= 0;
	pAgsdCharacter->m_ulReleaseAttributeInvincibleStatusTime = 0;
	pAgsdCharacter->m_ulReleaseNotAddAgroStatusTime		= 0;
	pAgsdCharacter->m_ulReleaseHideAgroStatusTime		= 0;
	pAgsdCharacter->m_ulReleaseStunProtectStatusTime	= 0;
	pAgsdCharacter->m_ulReleaseTransparentTime			= 0;
	pAgsdCharacter->m_ulReleasePvPInvincibleTime		= 0;
	pAgsdCharacter->m_ulReleaseHaltStatusTime			= 0;
	pAgsdCharacter->m_ulReleaseHalfTransparentTime		= 0;
	pAgsdCharacter->m_ulReleaseNormalATKInvincibleTime	= 0;
	pAgsdCharacter->m_ulReleaseSkillATKInvincibleTime	= 0;
	pAgsdCharacter->m_ulReleaseDisableSkillTime			= 0;
	pAgsdCharacter->m_ulReleaseDisableNormalATKTime		= 0;
	pAgsdCharacter->m_ulReleaseSleepTime				= 0;
	pAgsdCharacter->m_ulReleaseDisArmamentTime			= 0;
	pAgsdCharacter->m_ulReleaseDisableChatting			= 0;
	pAgsdCharacter->m_ulReleaseHoldTime					= 0;
	pAgsdCharacter->m_ulReleaseConfusionTime			= 0;

	pAgsdCharacter->m_lSlowStatusValue					= 0;

	pAgsdCharacter->m_ulNextBackupCharacterDataTime		= 0;

	pAgsdCharacter->m_bIsRecvAllDataFromLoginSvr		= FALSE;
	pAgsdCharacter->m_nRecvLoginServer					= -1;

	pAgsdCharacter->m_ulStartClockCount			= 0;
	pAgsdCharacter->m_ulPartyPlayTime			= 0;
	pAgsdCharacter->m_ulSoloPlayTime			= 0;
	pAgsdCharacter->m_ulKillMonCount			= 0;
	pAgsdCharacter->m_ulKillPCCount				= 0;
	pAgsdCharacter->m_ulDeadByMonCount			= 0;
	pAgsdCharacter->m_ulDeadByPCCount			= 0;

	pAgsdCharacter->m_bIsNewCID					= FALSE;

	pAgsdCharacter->m_lOldCID					= AP_INVALID_CID;

	pAgsdCharacter->m_ulTimeOutTransform		= 0;
	//pAgsdCharacter->m_ulTimeOutPaid				= 0;	// ���� ����

	pAgsdCharacter->m_eIdleProcessInterval[AGSDCHAR_IDLE_TYPE_TOTAL]	= AGSDCHAR_IDLE_INTERVAL_NEXT_TIME;

	pAgsdCharacter->m_ulReserveTimeForDestroy	= 0;

	pAgsdCharacter->m_ulPlayLogCount = 0;

	pAgsdCharacter->m_bRemoveByPeriod = FALSE;
	pAgsdCharacter->m_DetailInfo.clear();

	pAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.clear();

	pAgsdCharacter->m_bIsAuctionBlock	= FALSE;
	pAgsdCharacter->m_bIsGachaBlock		= FALSE;
	pAgsdCharacter->m_bIsTeleportBlock	= FALSE;

	pAgsdCharacter->m_ulConnectedTimeStamp = 0;

	pAgsdCharacter->m_bIsAutoPickupItem	= FALSE;
	
	pAgsdCharacter->m_stOptionSkillData.init();
	pAgsdCharacter->m_bLevelUpForced = FALSE;
	pAgsdCharacter->m_lLevelBeforeForced = 0;

	pAgsdCharacter->m_stInvincibleInfo.clear();

	ZeroMemory(pAgsdCharacter->m_szResurrectionCaster, sizeof(pAgsdCharacter->m_szResurrectionCaster));

	ZeroMemory(pAgsdCharacter->m_szCRMCode, sizeof(pAgsdCharacter->m_szCRMCode));
	ZeroMemory(pAgsdCharacter->m_szGrade, sizeof(pAgsdCharacter->m_szGrade));

#ifdef _USE_NPROTECT_GAMEGUARD_
	pAgsdCharacter->csa = NULL;
	pAgsdCharacter->m_GGLastQueryTime = 0;
#endif

	return TRUE;
}

//		DesAgsdCharacter
//	Functions
//		- AgsdCharacter destructor
//	Arguments
//		- pData : character data pointer
//		- pClass : this class pointer
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::DesAgsdCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
	{
		// invalid constructor parameter
		return FALSE;
	}

	AgsmCharacter *pThis			= (AgsmCharacter *) pClass;
	AgpdCharacter *pCharacter		= (AgpdCharacter *) pData;
	AgsdCharacter *pAgsdCharacter	= pThis->GetADCharacter(pCharacter);

	for (int i = 0; i < AGSD_CHARACTER_HISTORY_NUMBER; ++i)
	{
		if (pAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor)
		{
			pThis->m_pagpmFactors->DestroyFactor(pAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor);
			pThis->m_pagpmFactors->FreeAgpdFactor(pAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor, pThis->m_lFactorTypeIndex);

			pAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor	= NULL;
		}

		if (pAgsdCharacter->m_stHistoryParty.m_astEntry[i].m_pcsUpdateFactor)
		{
			pThis->m_pagpmFactors->DestroyFactor(pAgsdCharacter->m_stHistoryParty.m_astEntry[i].m_pcsUpdateFactor);
			pThis->m_pagpmFactors->FreeAgpdFactor(pAgsdCharacter->m_stHistoryParty.m_astEntry[i].m_pcsUpdateFactor, pThis->m_lFactorTypeIndex);

			pAgsdCharacter->m_stHistoryParty.m_astEntry[i].m_pcsUpdateFactor	= NULL;
		}
	}

#ifdef _USE_NPROTECT_GAMEGUARD_
	if( pAgsdCharacter->csa )
		delete pAgsdCharacter->csa;

	pAgsdCharacter->m_GGLastQueryTime = 0;
#endif

	return TRUE;
}

CHAR* AgsmCharacter::GetAccountID(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return NULL;

	AgsdCharacter *pagsdCharacter = GetADCharacter(pCharacter);
	if (!pagsdCharacter)
		return NULL;

	return pagsdCharacter->m_szAccountID;
}

CHAR* AgsmCharacter::GetServerName(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return NULL;

	AgsdCharacter *pagsdCharacter = GetADCharacter(pCharacter);
	if (!pagsdCharacter)
		return NULL;

	return pagsdCharacter->m_szServerName;
}

BOOL AgsmCharacter::OnDisconnect(INT32 lCID)
{
	/*
	INT32			lIndex = 0;
	INT8			cOperation = AGPMCHAR_PACKET_OPERATION_REMOVE;
	PVOID			pvPacket = NULL;
	INT16			nSize;
	AuPOS			posCharacter;
	AgpdCharacter	*pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);

	if (!pcsCharacter)
	{
		OutputDebugString("AgsmCharacter::OnDisconnect() Error (0) !!!\n");
		return FALSE;
	}

	m_papmMap->DeleteChar(pcsCharacter->m_stPos, pcsCharacter->m_lID);

	if (!m_pcsAgpmCharacter->RemoveCharacter(lCID))
	{
		OutputDebugString("AgsmCharacter::OnDisconnect() Error (1) !!!\n");
		return FALSE;
	}

	posCharacter = pcsCharacter->m_stPos;

	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE, sizeof(INT16),
														&cOperation,
														&lCID,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);

	if (!pvPacket)
	{
		OutputDebugString("AgsmCharacter::OnDisconnect() Error (2) !!!\n");
		return FALSE;
	}
 
	if (!m_pagsmAOIFilter->SendPacketNear(pvPacket, nSize, posCharacter))
	{
		OutputDebugString("AgsmCharacter::OnDisconnect() Error (3) !!!\n");
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
	*/

	return TRUE;
}

//		OnPreReceive
//	Functions
//		- AgpmCharacter ���� ��Ŷ�� ���ư��� ���� ��ȿ�� ��Ŷ���� �˻��Ѵ�.
//	Arguments
//		- ulType : packet type
//		- pvPacket : packet data
//		- nSize : packet size
//		- ulNID : client dpnid
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::OnPreReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || !nSize)
		return FALSE;

	if (!m_pcsAgpmCharacter)
		return FALSE;

	// m_csPacket
	INT8			cOperation = -1;
	INT32			lCID = 0;
	INT32			lTID = 0;
//	CHAR			*szName = NULL;
	CHAR			*szMessage	= NULL;
	UINT16			unMessageLength	= 0;
	INT8			cStatus = -1;
	INT8			cActionStatus = -1;
	INT8			cCriminalStatus = -1;
	PVOID			pvMovePacket;
	PVOID			pvActionPacket;
	PVOID			pvFactorPacket;
	INT64			llMoneyCount = 0;
	INT64			llBankMoney	= 0;
	INT64			llCash = 0;
	INT32			lAttackerID = AP_INVALID_CID;
	INT8			cIsNewChar = -1;
	UINT8			ucRegionIndex	= 0xff;
	UINT8			ucSocialAnimation	= 0xff;
	UINT64			ulSpecialStatus		= AGPDCHAR_SPECIAL_STATUS_NONE;

	m_pcsAgpmCharacter->m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,									// Operation
						&lCID,											// Character ID
						&lTID,											// Character Template ID
						&szMessage, &unMessageLength,					// Game ID
						&cStatus,										// Character Status
						&pvMovePacket,									// Move Packet
						&pvActionPacket,								// Action Packet
						&pvFactorPacket,								// Factor Packet
						&llMoneyCount,									// llMoney
						&llBankMoney,									// bank money
						&llCash,										// cash
						&cActionStatus,									// character action status
						&cCriminalStatus,								// character criminal status
						&lAttackerID,									// attacker id (������� ������ �ʿ�)
						&cIsNewChar,									// ���� �����Ǽ� �ʿ� �������� ����
						&ucRegionIndex,									// region index
						&ucSocialAnimation,								// social action index
						&ulSpecialStatus,								// special status
						NULL,											// is transform status
						NULL,											// skill initialization text
						NULL,											// face index
						NULL,											// hair index
						NULL,											// Option Flag
						NULL,											// bank size
						NULL,											// event status flag
						NULL,											// remained criminal status time
						NULL,											// remained murderer point time
						NULL,											// nick name
						NULL											// gameguard
						);

	if (pvActionPacket)
	{
		AgpdCharacter	*pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);
		if (!pcsCharacter)
			return FALSE;

		INT8			cActionType = (-1);
		INT32			lTargetCID = 0;
		INT32			lSkillTID = 0;
		INT8			cActionResultType	= -1;
		PVOID			pvPacketTargetDamageFactor	= NULL;
		AuPOS			stAttackPos = {0,0,0};
		UINT8			cComboInfo	= 0;
		INT8			cForceAttack	= 0;

		m_pcsAgpmCharacter->m_csPacketAction.GetField(FALSE, pvActionPacket, 0,
						&cActionType,
						&lTargetCID,
						&lSkillTID,
					    &cActionResultType,
					    &pvPacketTargetDamageFactor,
					    &stAttackPos,
					    &cComboInfo,
						&cForceAttack);

		AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);

		if (pcsAgsdCharacter->m_ulNextAttackTime > GetClockCount())
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgsmCharacter::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgsmCharacter::OnReceive");

	if (!pvPacket || !nSize)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	INT32	lServerID			= 0;
	INT8	cOperation			= (-1);
	INT32	lCharacterID		= 0;
	INT8	cStatus				= (-1);
	CHAR*	szAddress			= NULL;
	PVOID	pvCharServerData	= NULL;
	PVOID	pvBanData = NULL;

	INT32	lControlServerID	= AP_INVALID_SERVERID;
	CHAR*	szAccountName		= NULL;
	UINT32	ulPrevSendMoveTime	= 0;

	INT32	lNewCID				= AP_INVALID_CID;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCharacterID,
						&cStatus,
						&szAddress,
						&lServerID,
						&pvCharServerData,
						&pvBanData,
						&lNewCID);
#ifdef _DEBUG
	AuLogConsole_s("[%d] %s ( Operation %d )", ulNID, __FUNCTION__, (int)cOperation);
#endif
	switch (cOperation) {
	case AGSMCHAR_PACKET_OPERATION_UPDATE:
		{
			OnOperationUpdate(lCharacterID, pvCharServerData);

			EnumCallback(AGSMCHARACTER_CB_RECEIVE_CHARACTER_DATA, &lCharacterID, NULL);
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR:
		{
			CheckRecvChar(lNewCID, lServerID, lCharacterID);
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR_SUCCESS:
		{
			// ��Ű.. ĳ���� ���� ������ ��� ����Ÿ�� ������.
			AgsdServer	*pcsThisServer	= m_pAgsmServerManager2->GetThisServer();
			if (!pcsThisServer)
			{
				return FALSE;
			}

			AgsdServer	*pcsServer		= m_pAgsmServerManager2->GetServer(lServerID);
			if (!pcsServer)
			{
				return FALSE;
			}

			BOOL		bLogin	= FALSE;

			if (pcsThisServer->m_cType == AGSMSERVER_TYPE_LOGIN_SERVER)
				bLogin = TRUE;

			AgpdCharacter	*pcsCharacter			= m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				return FALSE;
			}

			INT32	lCID	= pcsCharacter->m_lID;

			AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

			/*
			INT32	lOldCID	= pcsAgsdCharacter->m_lOldCID;

			if (lOldCID == AP_INVALID_CID)
				lOldCID = pcsCharacter->m_lID;
			*/

			BOOL	bRetval	= FALSE;

			if (!pcsAgsdCharacter->m_bIsNewCID)
				bRetval = EnumCallback(AGSMCHARACTER_CB_SEND_CHARACTER_ALL_INFO, pcsCharacter, &pcsServer->m_dpnidServer);
			else
				bRetval = EnumCallback(AGSMCHARACTER_CB_SEND_CHARACTER_NEW_ID, pcsCharacter, &pcsServer->m_dpnidServer);

			// Send AuthKey to Client
			EnumCallback(AGSMCHARACTER_CB_SEND_AUTH_KEY, pcsCharacter, &pcsAgsdCharacter->m_dpnidCharacter);

			bRetval |= SendCharacterAllServerInfo(pcsCharacter, pcsServer->m_dpnidServer, bLogin);

			bRetval |= SendCompleteSendCharInfo(lCID, pcsServer->m_dpnidServer);

			EnumCallback(AGSMCHARACTER_CB_COMPLETE_SEND_CHARACTER_INFO, pcsCharacter, NULL);

			ResetWaitOperation(pcsCharacter, AGSMCHARACTER_WAIT_OPERATION_SEND_CHAR_INFO);

			pcsCharacter->m_Mutex.Release();

			return bRetval;
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_CHECK_RECV_CHAR_FAIL:
		{
			// ���д�. ����.. ������� ó�� �Ѵ�. ���� �� �������� �𸥴�. �� ��������.
			AgpdCharacter	*pcsCharacter			= m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
				return FALSE;

			ResetWaitOperation(pcsCharacter, AGSMCHARACTER_WAIT_OPERATION_SEND_CHAR_INFO);

			pcsCharacter->m_Mutex.Release();

			return TRUE;
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_COMPLETE_SEND_CHAR_INFO:
		{
			AgpdCharacter	*pcsCharacter			= m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
				return FALSE;

			AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

			pcsAgsdCharacter->m_bIsRecvAllDataFromLoginSvr	= TRUE;
			pcsAgsdCharacter->m_nRecvLoginServer			= lServerID;

			// ���� �̰� �Ⱦ���.... AgsmAccountManager���� ó�����ش�.
			//m_csCertificatedAccountID.AddObject( (void *)&pcsCharacter->m_lID, pcsAgsdCharacter->m_szAccountID );

			BOOL	bRetval = EnumCallback(AGSMCHARACTER_CB_COMPLETE_RECV_CHARDATA_FROM_LOGINSERVER, pcsCharacter, NULL);

			pcsCharacter->m_Mutex.Release();

			return bRetval;
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_BAN_DATA:
		{
			OnOperationBanData(lCharacterID, pvBanData);
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_REQUEST_NEW_CID:
		{
			OnOperationRequestNewCID(lCharacterID, ulNID);
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_RESPONSE_NEW_CID:
		{
			AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter) return FALSE;

			OnOperationResponseNewCID(pcsCharacter, lNewCID, ulNID);

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMCHAR_PACKET_OPERATION_COUPON_INFO:
		{
			if(FALSE == OnOperationCouponList(pvCharServerData, ulNID))
				return FALSE;
		}break;

	default:
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL AgsmCharacter::OnOperationUpdate(INT32 lCID, PVOID pvCharServerData)
{
	PROFILE("AgsmCharacter::OnOperationUpdate");

	if (!lCID || !pvCharServerData)
		return FALSE;

	// m_csPacketServerData
	INT32		lControlServerID					= AP_INVALID_SERVERID;
	CHAR*		szAccount							= NULL;
	UINT32		ulPrevSendMoveTime					= 0;
	UINT32		ulNextAttackTime					= 0;
	UINT32		ulNextSkillTime						= 0;
	INT8		cDestroyWhenDie						= 0;
	PVOID		pvPacketHistory						= NULL;
	UINT64		ullDBID								= 0;
	CHAR*		szLastShoutWord						= NULL;
	UINT32		ulLastUpdateMukzaPointTime			= 0;
	UINT32		ulRemainUpdateMukzaPointTime		= 0;
	INT32		lReceivedSkillPoint					= 0;
	UINT32		ulLastUpdateActionStatusTime		= 0;
	UINT32		ulReceivedClockCount				= 0;

	UINT32		ulCurrentClockCount					= GetClockCount();

	CHAR*		szLastUseTeleportPointName			= NULL;
	CHAR*		szServerName						= NULL;

	INT32		lMemberBillingNum					= 0;	// �Ϻ�: ���� ��ȣ

	m_csPacketServerData.GetField(FALSE, pvCharServerData, 0,
									&lControlServerID,
									&szAccount,
									&ulPrevSendMoveTime,
									&ulNextAttackTime,
									&ulNextSkillTime,
									&cDestroyWhenDie,
									&pvPacketHistory,
									&ullDBID,
									&szLastShoutWord,
									&ulLastUpdateMukzaPointTime,
									&ulRemainUpdateMukzaPointTime,
									&lReceivedSkillPoint,
									&ulLastUpdateActionStatusTime,
									&ulReceivedClockCount,
									&szLastUseTeleportPointName,
									&szServerName,
									&lMemberBillingNum);

	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
	{
		return FALSE;
	}

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (lControlServerID != AP_INVALID_SERVERID)
	{
		INT32	lPrevServerID = pcsAgsdCharacter->m_ulServerID;

		pcsAgsdCharacter->m_ulServerID = lControlServerID;

		if (lPrevServerID != lControlServerID)
			EnumCallback(AGSMCHARACTER_CB_UPDATE_CONTROLSERVER, pcsCharacter, &lPrevServerID);
		
		// 2006.06.04. steeple
		// ���� üũ EnterGameWorld ���� �ݹ�Ÿ�Բ� ����~
		//// ������ ������ ���� �ڵ� �߰� - 2004.03.26.
		//AgpdAdmin* pcsAttachedAdmin = m_pagpmAdmin->GetADCharacter(pcsCharacter);
		//if(pcsAttachedAdmin && pcsAttachedAdmin->m_lLoginCheck == 0)	// ó�� ������ ĳ����
		//{
		//	// Add Character Callbck �� �ҷ��ش�. - AgsmAdmin ���� ���� ��. 2004.03.25.
		//	m_pagpmAdmin->EnumCallback(AGPMADMIN_CB_ADD_CHARACTER, pcsCharacter, NULL);
		//	pcsAttachedAdmin->m_lLoginCheck = 1;
		//}
	}

	if (szAccount != NULL)
	{
		ZeroMemory(pcsAgsdCharacter->m_szAccountID, sizeof(CHAR) * (AGSMACCOUNT_MAX_ACCOUNT_NAME + 1));
		strncpy(pcsAgsdCharacter->m_szAccountID, szAccount, AGSMACCOUNT_MAX_ACCOUNT_NAME);
	}

	if (ulPrevSendMoveTime != 0)
		pcsAgsdCharacter->m_ulPrevSendMoveTime = ulCurrentClockCount + (ulPrevSendMoveTime - ulReceivedClockCount);

	if (ulNextAttackTime != 0)
		pcsAgsdCharacter->m_ulNextAttackTime = ulCurrentClockCount + (ulNextAttackTime - ulReceivedClockCount);

	if (ulNextSkillTime != 0)
		pcsAgsdCharacter->m_ulNextSkillTime = ulCurrentClockCount + (ulNextSkillTime - ulReceivedClockCount);

	if (cDestroyWhenDie != 0)
	{
		if (cDestroyWhenDie == 0)
			pcsAgsdCharacter->m_bDestroyWhenDie = FALSE;
		else
			pcsAgsdCharacter->m_bDestroyWhenDie = TRUE;
	}

	if (ullDBID != 0)
	{
		pcsAgsdCharacter->m_ullDBID = ullDBID;
	}

	if (szLastShoutWord != NULL)
	{
		ZeroMemory(pcsAgsdCharacter->m_szLastShoutWord, sizeof(CHAR) * (AGSM_CHARACTER_MAX_SHOUT_WORD + 1));
		strncpy(pcsAgsdCharacter->m_szLastShoutWord, szLastShoutWord, AGSM_CHARACTER_MAX_SHOUT_WORD);
	}

	if (ulLastUpdateMukzaPointTime != 0)
		pcsAgsdCharacter->m_ulLastUpdateMukzaPointTime = ulCurrentClockCount + (ulLastUpdateMukzaPointTime - ulReceivedClockCount);
	
	if (ulRemainUpdateMukzaPointTime != 0)
		pcsAgsdCharacter->m_ulRemainUpdateMukzaPointTime = ulCurrentClockCount + (ulRemainUpdateMukzaPointTime - ulReceivedClockCount);

	if (lReceivedSkillPoint != 0)
		pcsAgsdCharacter->m_lReceivedSkillPoint = lReceivedSkillPoint;

	if (ulLastUpdateActionStatusTime != 0)
		pcsAgsdCharacter->m_ulLastUpdateActionStatusTime = ulCurrentClockCount + (ulLastUpdateActionStatusTime - ulReceivedClockCount);

	if (pvPacketHistory)
	{
		if (!ParsingHistoryPacket(pcsCharacter, pvPacketHistory, ulCurrentClockCount, ulReceivedClockCount))
		{
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}
	}

	if (szLastUseTeleportPointName)
	{
		AgpdTeleportAttachToChar	*pcsTeleportAttachData	= m_pcsAgpmEventTeleport->GetADCharacter(pcsCharacter);
		pcsTeleportAttachData->m_pcsLastUseTeleportPoint	= m_pcsAgpmEventTeleport->GetTeleportPoint(szLastUseTeleportPointName);
	}
	
	if (szServerName)
	{
		ZeroMemory(pcsAgsdCharacter->m_szServerName, sizeof(pcsAgsdCharacter->m_szServerName));
		strncpy(pcsAgsdCharacter->m_szServerName, szServerName, AGSM_MAX_SERVER_NAME);
		
		// ������� �ٸ� ������ �α��� �ߴ��� üũ.
		AgsdServer *pcsServer = m_pAgsmServerManager2->GetThisServer();
		if (!pcsServer)
		{
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}

		BOOL bFoundWorld = FALSE;
		if (0 == strcmp(pcsServer->m_szWorld, pcsAgsdCharacter->m_szServerName))
			bFoundWorld = TRUE;

		if (!bFoundWorld)
		{
			for (INT32 i = 0; i < pcsServer->m_nSubWorld; i++)
			{
				if (0 == strcmp(pcsServer->m_SubWorld[i].m_szName, pcsAgsdCharacter->m_szServerName))
				{
					bFoundWorld = TRUE;
					break;
				}
			}
		}

		if (!bFoundWorld)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "Acc[%s]Char[%s]World[%s] entered another world[%s]\n",
																pcsAgsdCharacter->m_szAccountID,
																pcsCharacter->m_szID,
																pcsAgsdCharacter->m_szServerName,
																pcsServer->m_szWorld);
			AuLogFile_s("LOG\\DIFF_WORLD.log", strCharBuff);
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}
	
	}

	pcsAgsdCharacter->m_lMemberBillingNum = lMemberBillingNum;	// �Ϻ�: ���� ��ȣ
	_itoa_s( lMemberBillingNum, pcsAgsdCharacter->m_szMemberBillingNum, 32, 10 );

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmCharacter::OnOperationBanData(INT32 lCID, PVOID pvBanData)
{
	if(!lCID || !pvBanData)
		return FALSE;

	AgsdCharacterBan agsdCharacterBan;
	memset(&agsdCharacterBan, 0, sizeof(agsdCharacterBan));

	m_csPacketBanData.GetField(FALSE, pvBanData, 0,
							&agsdCharacterBan.m_lChatBanStartTime,
							&agsdCharacterBan.m_lChatBanKeepTime,
							&agsdCharacterBan.m_lCharBanStartTime,
							&agsdCharacterBan.m_lCharBanKeepTime,
							&agsdCharacterBan.m_lAccBanStartTime,
							&agsdCharacterBan.m_lAccBanKeepTime
							);

	AgpdCharacter* pcsAgpdCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if(!pcsAgpdCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	memcpy(&pcsAgsdCharacter->m_stBan, &agsdCharacterBan, sizeof(agsdCharacterBan));

	pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmCharacter::OnOperationRequestNewCID(INT32 lOldCID, UINT32 ulNID)
{
	if (lOldCID == AP_INVALID_CID || ulNID == 0)
		return FALSE;

	INT32	lNewCID	= AP_INVALID_CID;

	EnumCallback(AGSMCHARACTER_CB_GET_NEW_CID, &lNewCID, NULL);

	/*
	if (lNewCID == AP_INVALID_CID)
		return FALSE;
	*/

	// �����ߴٴ� �͵� �˾ƾ� �ϴ� AP_INVALID_CID �� �� �����ش�.

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketResponseNewCID(lOldCID, lNewCID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacketSvr(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmCharacter::OnOperationResponseNewCID(AgpdCharacter *pcsCharacter, INT32 lNewCID, UINT32 ulNID)
{
	if (!pcsCharacter || lNewCID == AP_INVALID_CID || ulNID == 0)
		return FALSE;

	AgsdCharacter			*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_bIsNewCID	= TRUE;

	PVOID	pvBuffer[2];
	pvBuffer[0]				= IntToPtr(lNewCID);
	pvBuffer[1]				= UintToPtr(ulNID);

	EnumCallback(AGSMCHARACTER_CB_RECEIVE_NEW_CID, pcsCharacter, pvBuffer);

	return TRUE;
}

BOOL AgsmCharacter::OnOperationCouponList(PVOID pvCharDetailInfo, UINT32 ulNID)
{
	if(NULL == pvCharDetailInfo)
		return FALSE;

	CouponInfo *pCouponInfo = NULL;

	INT32 lLength = 0;
	INT32 lCouponCount = 0;

	CHAR *pszAccountID = NULL;

	m_csPacketCoupon.GetField(FALSE, pvCharDetailInfo, 0,
							  &pszAccountID,
							  &pCouponInfo,		
							  &lLength
							  );	

	lCouponCount = lLength / sizeof(CouponInfo);

	AgsdAccount *pcsAccount = m_pagsmAccountManager->GetAccount(pszAccountID);
	if(pcsAccount)
	{
		m_pagsmAccountManager->RemoveAccount(pszAccountID, TRUE);
	}
	
	if (!m_pagsmAccountManager->AddAccount(pszAccountID, ulNID))
	{
		DestroyClient(ulNID);
		return FALSE;
	}

	pcsAccount = m_pagsmAccountManager->GetAccount(pszAccountID);
	if(NULL == pcsAccount)
	{
		DestroyClient(ulNID);
		return FALSE;
	}

	m_pagsmAccountManager->SetCouponNum(pcsAccount, lCouponCount);

	for(int i=0; i < lCouponCount; ++i)
	{
		m_pagsmAccountManager->AddCouponInfo(pcsAccount, &pCouponInfo[i]);
	}

	return TRUE;
}


BOOL AgsmCharacter::ParsingHistoryPacket(AgpdCharacter *pcsCharacter, PVOID pvPacketHistory, UINT32 ulCurrentClockCount, UINT32 ulReceivedClockCount)
{
	if (!pcsCharacter ||
		!pvPacketHistory)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return FALSE;

	INT16		nEntryNum = 0;
	ApSafeArray<PVOID, AGSD_CHARACTER_HISTORY_NUMBER>	pvPacketHistoryEntries;
	pvPacketHistoryEntries.MemSetAll();

	m_csPacketHistory.GetField(FALSE, pvPacketHistory, 0,
							&nEntryNum,
							&pvPacketHistoryEntries[0]);

	pcsAgsdCharacter->m_stHistory.m_lEntryNum = nEntryNum;

	for (int i = 0; i < pcsAgsdCharacter->m_stHistory.m_lEntryNum; ++i)
	{
		if (pvPacketHistoryEntries[i])
		{
			PVOID	pvPacketBase		= NULL;
			PVOID	pvPacketFactor		= NULL;
			UINT32	ulFirstHitTime		= 0;
			UINT32	ulLastHitTime		= 0;
			INT8	cFirstHit			= 0;
			FLOAT	fAgro				= 0;

			m_csPacketHistoryEntry.GetField(FALSE, pvPacketHistoryEntries[i], 0,
							&pvPacketBase,
							&pvPacketFactor,
							&ulFirstHitTime,
							&ulLastHitTime,
							&cFirstHit,
							&fAgro);

			if (!pvPacketBase || !pvPacketFactor)
			{
				--pcsAgsdCharacter->m_stHistory.m_lEntryNum;
				--i;
				continue;
			}

			INT8	cBaseType			= (-1);
			INT32	lBaseID				= 0;

			m_csPacketBase.GetField(FALSE, pvPacketBase, 0,
							&cBaseType,
							&lBaseID);

			pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_csSource.m_eType	= (ApBaseType) cBaseType;
			pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_csSource.m_lID	= lBaseID;

			if (pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor)
				m_pagpmFactors->ReflectPacket(pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor, pvPacketFactor, 0);
			else
			{
				pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor	= m_pagpmFactors->AllocAgpdFactor(m_lFactorTypeIndex);
				if (pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor)
				{
					m_pagpmFactors->InitFactor(pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor);
					m_pagpmFactors->ReflectPacket(pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_pcsUpdateFactor, pvPacketFactor, 0);
				}
			}

			pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_ulFirstHitTime = ulCurrentClockCount + (ulFirstHitTime - ulReceivedClockCount);
			pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_ulLastHitTime = ulCurrentClockCount + (ulLastHitTime - ulReceivedClockCount);
			pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_fAgro = fAgro;
			pcsAgsdCharacter->m_stHistory.m_astEntry[i].m_bFirstHit = (BOOL) cFirstHit;
		}
	}

	return TRUE;
}

BOOL AgsmCharacter::SetCallbackSendAddCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SENDADDCHAR, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSendEquipItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SENDEQUIPITEM, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackRemoveItemOnly(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_REMOVE_ITEM_ONLY, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackReCalcFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_RECALC_FACTOR, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackReCalcResultFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_RECALC_RESULT_FACTOR, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackUpdateLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_UPDATE_LEVEL, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackUpdateHP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_UPDATE_HP, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackUpdateMaxHP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_UPDATE_MAX_HP, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSendCharacterAllInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_CHARACTER_ALL_INFO, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSendCharacterNewID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_CHARACTER_NEW_ID, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSendCharacterAllServerInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_CHARACTER_ALL_SERVER_INFO, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSendCellInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_CELL_INFO, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSendCellRemoveInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_CELL_REMOVE_INFO, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackEnterGameworld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_ENTER_GAMEWORLD, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackRemoveWaitOperation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_REMOVE_WAIT_OPERATION, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackCompleteSendCharacterInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_COMPLETE_SEND_CHARACTER_INFO, pfCallback, pClass);
}

/*
BOOL AgsmCharacter::SetCallbackSendAllDBData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_ALL_DB_DATA, pfCallback, pClass);
}
*/

BOOL AgsmCharacter::SetCallbackReceiveCharacterData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_RECEIVE_CHARACTER_DATA, pfCallback, pClass);
}

/*
BOOL AgsmCharacter::SetCallbackDBStreamInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_DB_STREAM_INSERT, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackDBStreamDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_DB_STREAM_DELETE, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackDBStreamSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_DB_STREAM_SELECT, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackDBStreamUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_DB_STREAM_UPDATE, pfCallback, pClass);
}
*/

BOOL AgsmCharacter::SetCallbackGetNewCID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_GET_NEW_CID, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackDisconnectFromGameServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_DISCONNECT_FROM_GAMESERVER, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSyncCharacterActions(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SYNC_CHARACTER_ACTIONS, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackReceiveNewCID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_RECEIVE_NEW_CID, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackCompleteRecvCharDataFromLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_COMPLETE_RECV_CHARDATA_FROM_LOGINSERVER, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackAdjustTransformFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_ADJUST_TRANSFORM_FACTOR, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackApplyBonusFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_APPLY_BONUS_FACTOR, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackSendAuthKey(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_AUTH_KEY, pfCallback, pClass);
}

/*
	��� Module �� ���� ����̹Ƿ� ��帶�������� Ȯ���ϱ� ���� �ݹ� ���
*/
BOOL AgsmCharacter::SetCallbackIsGuildMaster(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_IS_GUILD_MASTER, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackExitGameWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_EXIT_GAME_WORLD, pfCallback, pClass);
}

/*
//		GetForwardSector
//	Functions
//		- pOldSector, pNewSector�� ���� �̵������� ���Ѵ�.
//		- ������ ������ �̵������� ���� �þ߳� ���� �߰��� ���Ϳ� �þ߿��� ��� ���͸� ���Ѵ�.
//	Arguments
//		- pOldSector : �̵��� ����
//		- pNewSector : �̵��� ����
//		- ppForwardBuffer : �þ߳� ���� �߰��� ���͵� �����͸� ������ ����
//		- ppBackwardBuffer : �þ߿��� ��� ���͵� �����͸� ������ ����
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::GetForwardSector(ApWorldSector *pOldSector, ApWorldSector *pNewSector, 
									 ApWorldSector **ppForwardBuffer, ApWorldSector **ppBackBuffer)
{
	PROFILE("AgsmCharacter::GetForwardSector");

	INT16 moveX = pNewSector->GetIndexX() - pOldSector->GetIndexX();
	INT16 moveY = pNewSector->GetIndexZ() - pOldSector->GetIndexZ();

	if (moveX == 0 && moveY == 0)
		return TRUE;

	int i, j;

	// �̵� ��, �� ���͸� �߽����� �� �ֺ� ���� �����͸� �ϴ� �� �����Ѵ�.
	// ���߿� ��ġ�� �κ��� NULL�� �����Ѵ�.
	for (i = 0; i < 3; ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			ppForwardBuffer[j * 3 + i] = GetSectorBySectorIndex(pNewSector->GetIndexX() - (1 - i), 0, pNewSector->GetIndexZ() - (1 - j));
			ppBackBuffer   [j * 3 + i] = GetSectorBySectorIndex(pOldSector->GetIndexX() - (1 - i), 0, pOldSector->GetIndexZ() - (1 - j));
		}
	}

	if (abs(moveX) > 2 || abs(moveY) > 2)
		// ��ġ�� ���� �ϳ��� ����.
		return TRUE;

	INT16	nIndexX, nIndexZ;
	INT16	nOriginX, nOriginZ;

	nOriginX = pOldSector->GetIndexX() - 1;
	nOriginZ = pOldSector->GetIndexZ() - 1;
	
	for (i = 0; i < 9; ++i)
	{
		if (!ppForwardBuffer[i])
			continue;

		nIndexX = ppForwardBuffer[i]->GetIndexX() - nOriginX;
		nIndexZ = ppForwardBuffer[i]->GetIndexZ() - nOriginZ;

		if (nIndexX >= 0 && nIndexX <= 2 && nIndexZ >= 0 && nIndexZ <= 2)
		{
			ppBackBuffer[nIndexZ * 3 + nIndexX] = NULL;
			ppForwardBuffer[i] = NULL;
		}
	}

	return TRUE;
}
*/

BOOL AgsmCharacter::GetForwardCell(AgsmAOICell *pcsOldCell, AgsmAOICell *pcsNewCell,
									 AgsmAOICell **ppForwardBuffer, AgsmAOICell **ppBackBuffer, INT32 lForwardRange, INT32 lBackwardRange)
{
	if (!pcsOldCell || !pcsNewCell || !ppForwardBuffer || !ppBackBuffer || lForwardRange < 1 || lBackwardRange <1)
		return FALSE;

	PROFILE("AgsmCharacter::GetForwardCell");

	INT16 moveX = pcsNewCell->GetIndexX() - pcsOldCell->GetIndexX();
	INT16 moveY = pcsNewCell->GetIndexZ() - pcsOldCell->GetIndexZ();

	if (moveX == 0 && moveY == 0)
		return TRUE;

	int i, j;

	// �̵� ��, �� ���͸� �߽����� �� �ֺ� ���� �����͸� �ϴ� �� �����Ѵ�.
	// ���߿� ��ġ�� �κ��� NULL�� �����Ѵ�.
	for (i = 0; i < lForwardRange; ++i)
	{
		for (j = 0; j < lForwardRange; ++j)
		{
			ppForwardBuffer[j * lForwardRange + i] = m_pagsmAOIFilter->m_csAOIMap.GetCell(pcsNewCell->GetIndexX() - (lForwardRange / 2 - i), pcsNewCell->GetIndexZ() - (lForwardRange / 2 - j));
		}
	}

	for (i = 0; i < lBackwardRange; ++i)
	{
		for (j = 0; j < lBackwardRange; ++j)
		{
			ppBackBuffer   [j * lBackwardRange + i] = m_pagsmAOIFilter->m_csAOIMap.GetCell(pcsOldCell->GetIndexX() - (lBackwardRange / 2 - i), pcsOldCell->GetIndexZ() - (lBackwardRange / 2 - j));
		}
	}

	INT32	lMaxRange	= (lForwardRange > lBackwardRange) ? lForwardRange : lBackwardRange;

	if (abs(moveX) >= lMaxRange || abs(moveY) >= lMaxRange)
		// ��ġ�� ���� �ϳ��� ����.
		return TRUE;

	INT16	nIndexX, nIndexZ;
	INT16	nOriginX, nOriginZ;

	nOriginX = pcsOldCell->GetIndexX() - lBackwardRange / 2;
	nOriginZ = pcsOldCell->GetIndexZ() - lBackwardRange / 2;
	
	INT32	lPowRange	= lForwardRange * lForwardRange;

	for (i = 0; i < lPowRange; ++i)
	{
		if (!ppForwardBuffer[i])
			continue;

		nIndexX = ppForwardBuffer[i]->GetIndexX() - nOriginX;
		nIndexZ = ppForwardBuffer[i]->GetIndexZ() - nOriginZ;

		if (nIndexX >= 0 && nIndexX < lForwardRange && nIndexZ >= 0 && nIndexZ < lForwardRange)
		{
			ppBackBuffer[nIndexZ * lBackwardRange + nIndexX] = NULL;
			ppForwardBuffer[i] = NULL;
		}
	}

	return TRUE;
}

ApWorldSector*	AgsmCharacter::GetSectorBySectorIndex(int arrayindexX , int arrayindexY , int arrayindexZ)
{
	return m_papmMap->GetSectorByArray(SectorIndexToArrayIndexX(arrayindexX), arrayindexY, SectorIndexToArrayIndexZ(arrayindexZ));
}

BOOL AgsmCharacter::SetCallbackUpdateControlServer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_UPDATE_CONTROLSERVER, pfCallback, pClass);
}

//		EnterGameWorld
//	Functions
//		- pcsCharacter�� �ٸ� ��� ���� ������ �����Ѵ�.
//	Arguments
//		- pCharacter : �߰��� ĳ���� 
//	Return value
//		- AgpdCharacter * : �߰��� ĳ���� ����Ÿ 
///////////////////////////////////////////////////////////////////////////////
AgpdCharacter *AgsmCharacter::EnterGameWorld(AgpdCharacter *pCharacter, BOOL bZoning)
{
	PROFILE("AgsmCharacter::EnterGameWorld");

	if (!pCharacter)
		return NULL;

	AgsdCharacter *pcsAgsdCharacter = GetADCharacter(pCharacter);

	//ReCalcCharacterFactors(pCharacter, TRUE);
	
	pcsAgsdCharacter->m_ulServerID = m_pagsmMap->GetThisServer();

	if (!bZoning /*&& pcsAgsdCharacter->m_dpnidCharacter != 0*/)
	{
		if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		{
			m_pagsmFactors->SetCharPoint(&pCharacter->m_csFactor, TRUE, FALSE);
			/*
			PVOID pvPacketFactor = m_pagsmFactors->SetCharPoint(&pCharacter->m_csFactor, TRUE, TRUE);
			if (pvPacketFactor)
			{
				SendPacketFactor(pvPacketFactor, pCharacter);

				m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);
			}
			*/
		}
		
		if (m_pcsAgpmCharacter->IsPC(pCharacter))
		{
			INT32	lClass	= m_pagpmFactors->GetClass(&pCharacter->m_pcsCharacterTemplate->m_csFactor);
			INT32	lRace	= m_pagpmFactors->GetRace(&pCharacter->m_pcsCharacterTemplate->m_csFactor);

			if (lRace > AURACE_TYPE_NONE && lRace < AURACE_TYPE_MAX &&
				lClass > AUCHARCLASS_TYPE_NONE && lClass < AUCHARCLASS_TYPE_MAX)
			{
				InterlockedIncrement(&m_alNumPlayerByClass[lRace][lClass]);
			}
		}

		m_pcsAgpmCharacter->UpdateStatus(pCharacter, AGPDCHAR_STATUS_IN_GAME_WORLD);

		if (!EnumCallback(AGSMCHARACTER_CB_ENTER_GAMEWORLD, pCharacter, NULL))
		{
			if (pcsAgsdCharacter->m_dpnidCharacter != 0)
				DestroyClient(pcsAgsdCharacter->m_dpnidCharacter);
			return NULL;
		}
	}

	//m_pcsAgpmCharacter->UpdateStatus(pCharacter, AGPDCHAR_STATUS_IN_GAME_WORLD);

	if (!m_pcsAgpmCharacter->IsPC(pCharacter))
		m_pcsAgpmCharacter->UpdateActionStatus(pCharacter, AGPDCHAR_STATUS_NORMAL);

	StartPlayTime(pCharacter);

	if (m_pcsAgpmCharacter->IsPC(pCharacter))
	{
		// Saved SpecialStatus�� ó�����ش�.
		LoadSavedSpecialStatus(pCharacter);

		// PC ��� �⺻ ���� �ɾ��ش�.
		SetDefaultInvincible(pCharacter);

		// ������ �ð��� �����س��´�. 2006.03.09. steeple
		pcsAgsdCharacter->m_ulConnectedTimeStamp = AuTimeStamp::GetCurrentTimeStamp();

		// �α��� ���� ���� ������ ������ ���´�. 2007.11.06. steeple
		pcsAgsdCharacter->m_lLoginLevel = m_pcsAgpmCharacter->GetLevel(pCharacter);

		if (m_pcsAgpmEventBinding)
		{
			INT16	nBindingRegion = m_papmMap->GetRegion(pCharacter->m_stPos.x ,pCharacter->m_stPos.z);
			ApmMap::RegionTemplate	*pcsRegionTemplate	= m_papmMap->GetTemplate(nBindingRegion);
			if (pcsRegionTemplate)
			{
				// binding event�� �پ� �ִ� region ���� ����.
				if (m_pcsAgpmEventBinding->GetBindingByTown(pcsRegionTemplate->pStrName, AGPDBINDING_TYPE_RESURRECTION))
				{
					pCharacter->m_nBindingRegionIndex		= nBindingRegion;
					pCharacter->m_nLastExistBindingIndex	= nBindingRegion;
				}
			}
		}

#ifdef _USE_NPROTECT_GAMEGUARD_
		if(pcsAgsdCharacter->m_dpnidCharacter > 0)
		{
			pcsAgsdCharacter->csa = new CCSAuth2();
			pcsAgsdCharacter->csa->Init();
		}
#endif
	}
	else
	{
		if( pCharacter->m_nBindingRegionIndex < 0 )
			pCharacter->m_nBindingRegionIndex = m_papmMap->GetRegion(pCharacter->m_stPos.x ,pCharacter->m_stPos.z);
	}

	return pCharacter;
}

//		RemoveItemOnly
//	Functions
//		- ĳ���ʹ� �ΰ� �����۸� ���ش�.
//	Arguments
//		- pCharacter : ó���� ĳ����
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::RemoveItemOnly(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	EnumCallback(AGSMCHARACTER_CB_REMOVE_ITEM_ONLY, pCharacter, NULL);

	return TRUE;
}

BOOL AgsmCharacter::SetCharacterLevel(AgpdCharacter *pcsCharacter, INT32 lLevel, BOOL bCopyOnlyAttack)
{
	if (!pcsCharacter || !pcsCharacter->m_pcsCharacterTemplate/* || lLevel > AGPMCHAR_MAX_LEVEL*/)
		return FALSE;

	if (lLevel < 1)
		lLevel	= 1;

	// 2006.12.26. steeple
	// �� �Լ��� �θ��� �Ǹ� ������ ���� ������ 1�� ������ �Ѵ�.
	// �׷��� AgpmCharacter::UpdateLevel() �� �θ� �� 1 �� ���ִ� ���̴�.
	//
	//

	// ĳ������ ������ lLevel�� �����Ѵ�. ������ ���ø��� �ִ� ���͸� �����ؼ� ������.
	// ���ø��� ���Ͱ� ���õǾ� ���� �ʴٸ� FALSE�� �����Ѵ�.

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	pcsAgsdCharacter->m_bLevelUpForced = TRUE;
	pcsAgsdCharacter->m_lLevelBeforeForced = m_pcsAgpmCharacter->GetLevel(pcsCharacter);

	INT32 lCharismaPoint = m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter);
	INT32 lMurdererPoint = m_pcsAgpmCharacter->GetMurdererPoint(pcsCharacter);

	AgpdCharacterTemplate	*pcsTemplate	= pcsCharacter->m_pcsCharacterTemplate;

	if (bCopyOnlyAttack)
	{
		if (!m_pagpmFactors->CopyFactor(&pcsCharacter->m_csFactor, &pcsTemplate->m_csFactor, FALSE, FALSE, AGPD_FACTORS_TYPE_ATTACK))
			return FALSE;
	}
	else
	{
		if (!m_pagpmFactors->CopyFactor(&pcsCharacter->m_csFactor, &pcsTemplate->m_csFactor, FALSE, FALSE))
			return FALSE;
	}

	m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);

	if (lLevel <= AGPMCHAR_MAX_LEVEL && !m_pagpmFactors->IsUseFactor(&pcsTemplate->m_csLevelFactor[lLevel]))
		return FALSE;

	BOOL bResult = m_pcsAgpmCharacter->UpdateLevel(pcsCharacter, lLevel - 1);

	pcsAgsdCharacter->m_bLevelUpForced = FALSE;
	pcsAgsdCharacter->m_lLevelBeforeForced = 0;

	return bResult;
}

BOOL AgsmCharacter::ReCalcCharacterFactors(INT32 lCID, BOOL bSendPacket)
{
	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);

	return ReCalcCharacterFactors(pcsCharacter, bSendPacket);
}

BOOL AgsmCharacter::ReCalcCharacterFactors(AgpdCharacter *pcsCharacter, BOOL bSendPacket)
{
//	PROFILE("AgsmCharacter::ReCalcCharacterFactors");

	if (!pcsCharacter)
		return FALSE;

	//AgpdFactor	csUpdateFactor;
	//ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));
	//m_pagpmFactors->InitFactor(&csUpdateFactor);

	m_pagpmFactors->InitFactor(&pcsCharacter->m_csFactorPoint);
	m_pagpmFactors->InitFactor(&pcsCharacter->m_csFactorPercent);

	// Item, Party ������ ����϶�� �Ѵ�.
	EnumCallback(AGSMCHARACTER_CB_RECALC_FACTOR, pcsCharacter, NULL);

	// arycoat - 2008.11
	ReCalcCharacterResultFactors(pcsCharacter, bSendPacket);

	return TRUE;
}

BOOL AgsmCharacter::ReCalcCharacterResultFactors(AgpdCharacter *pcsCharacter, BOOL bSendPacket)
{
	PROFILE("AgsmCharacter::ReCalcCharacterResultFactors");

	if (!pcsCharacter)
		return FALSE;

	// shrine ����� ��⿡�� ��ü������ ������ �ִ� ���͵��� ó���Ѵ�.
	EnumCallback(AGSMCHARACTER_CB_RECALC_RESULT_FACTOR, pcsCharacter, NULL);

	// 2005.07.08. steeple
	// Special Status �� ���ؼ� ����Ǵ� Factor �� ����Ѵ�.
	//ApplySpecialStatusFactor(pcsCharacter);

	AgpdFactor	csUpdateFactor;
	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));
	m_pagpmFactors->InitFactor(&csUpdateFactor);

	if (!m_pagpmFactors->CalcResultFactor(&csUpdateFactor, &pcsCharacter->m_csFactor, &pcsCharacter->m_csFactorPoint, &pcsCharacter->m_csFactorPercent))
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return FALSE;
	}

	AgpdFactor	*pcsUpdateResultFactor	= (AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT);

	// ������ ���� ���� �ʴ´�.
	if (m_pcsAgpmCharacter->IsPC(pcsCharacter))
	{
		m_pagsmFactors->ResetCharMaxPoint(pcsUpdateResultFactor);

		ApplyBonusFactor(pcsCharacter, &csUpdateFactor);

		if (pcsUpdateResultFactor &&
			!m_pagpmFactors->GetFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT))
		{
			m_pagpmFactors->CopyFactor(pcsUpdateResultFactor, (AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), TRUE, FALSE, AGPD_FACTORS_TYPE_CHAR_POINT);
		}

		//m_pagsmFactors->AdjustWeaponFactor(pcsCharacter, pcsUpdateResultFactor);
	}

	// 2005.06.30. steeple
	// �ٽ� ���� �ڵ�� Rollback.
	//
	// 2005.06.29. steeple
	// HP, MP, SP �� Max ���� ���簪 ������ �ϴ� �κ� �ణ ����
	if (!m_pagsmFactors->AdjustCharMaxPoint(&csUpdateFactor))
	//if(!m_pagsmFactors->AdjustCharMaxPointFromBaseFactor(&csUpdateFactor, &pcsCharacter->m_csFactor))
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return FALSE;
	}

	// Ż�Ϳ� ���� �̵��ӵ��� ����
	if (pcsCharacter->m_bRidable)
	{
		AgpdGridItem *pcsAgpdGridItem = m_pagpmItem->GetEquipItem( pcsCharacter, AGPMITEM_PART_RIDE );
		if (NULL != pcsAgpdGridItem)
		{
			AgpdItem *pcsItem = m_pagpmItem->GetItem(pcsAgpdGridItem);
			if (NULL != pcsItem)
			{
				if (NULL != m_pagpmFactors->GetFactor((AgpdFactor*)m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT), AGPD_FACTORS_TYPE_CHAR_STATUS))
					m_pagpmFactors->SetValue(&csUpdateFactor, pcsItem->m_pcsItemTemplate->m_lRunBuff, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
			}
		}
	}

	PVOID pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, &csUpdateFactor, bSendPacket);

	FLOAT fHeroicRangedResistance = 0.0f;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &fHeroicRangedResistance, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC_RANGE);

	if (bSendPacket && pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_LOGOUT)
	{
		// ����� ��Ŷ�� Ŭ���̾�Ʈ�� ������.
		INT16 nPacketLength = 0;
		PVOID pvPacket = MakePacketFactor(pcsCharacter, pvPacketFactor, &nPacketLength);
		if (NULL != pvPacket)
		{
			if (pcsCharacter->m_bIsAddMap)
				m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_3);
			else
				SendPacket(pvPacket, nPacketLength, GetCharDPNID(pcsCharacter));
		}
	}

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	return TRUE;
}

BOOL AgsmCharacter::AddAgroPoint( AgpdCharacter *pcsCharacter, ApBase *pcsSource, eAgpdFactorAgroType eAgroType, INT32 lAgroPoint )
{
	// 2007.01.24. steeple
	// �ڱ� �ڽſ��� ��׷θ� ���� �� ���� �� �ƴѰ�
	if(pcsCharacter && pcsSource && pcsCharacter->m_lID == pcsSource->m_lID)
		return FALSE;

	AgpdFactor			csAttackTryFactor;
	AgpdFactor			*pcsUpdateFactorResult;

	//��׷� ����Ʈ�� ���̳ʽ��� ���� ����.
	if( lAgroPoint > 0 )
	{
		if( eAgroType == AGPD_FACTORS_AGRO_TYPE_STATIC )
		{
			pcsUpdateFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(&csAttackTryFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
			m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lAgroPoint), AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_STATIC );
		}
		else if( eAgroType == AGPD_FACTORS_AGRO_TYPE_XP_STATIC )
		{
			pcsUpdateFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(&csAttackTryFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
			m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lAgroPoint), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP );
		}
		else if( eAgroType == AGPD_FACTORS_AGRO_TYPE_SLOW )
		{
			pcsUpdateFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(&csAttackTryFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
			m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lAgroPoint), AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_SLOW );
		}
		else if( eAgroType == AGPD_FACTORS_AGRO_TYPE_FAST )
		{
			pcsUpdateFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(&csAttackTryFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
			m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lAgroPoint), AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_FAST );
		}

		AddHistory(pcsCharacter, pcsSource, pcsUpdateFactorResult, GetClockCount());
	}

	m_pagpmFactors->DestroyFactor(&csAttackTryFactor);

	return TRUE;
}

//EXP�� ���� ���õ� ��׷� �����Լ�.
BOOL AgsmCharacter::AddAgroPoint( AgpdCharacter *pcsCharacter, ApBase *pcsSource, AgpdFactor *pcsAgpdFactor )
{
	ASSERT(NULL != pcsCharacter);
//	ASSERT(NULL != pcsSource);
	ASSERT(NULL != pcsAgpdFactor);

	if( NULL == pcsAgpdFactor)
		return FALSE;

	// 2005.10.13. steeple
	// �̰� �� NULL üũ ���ϳ�~~~~
	if(NULL == pcsCharacter || NULL == pcsSource)
		return FALSE;

	// 2007.01.24. steeple
	// �ڱ� �ڽſ��� ��׷θ� ���� �� ���� �� �ƴѰ�
	if(pcsCharacter->m_lID == pcsSource->m_lID)
		return FALSE;

	BOOL bSlaveCharacter = FALSE;
	if (APBASE_TYPE_CHARACTER == pcsSource->m_eType)
	{
		AgpdCharacter *pcsSrcCharacter = (AgpdCharacter*)pcsSource;
		if (m_pcsAgpmCharacter->IsStatusTame(pcsSrcCharacter) || m_pcsAgpmCharacter->IsStatusSummoner(pcsSrcCharacter))
			bSlaveCharacter = TRUE;
	}

	//Damage�� �޾Ƽ� Static Agro�� �߰��Ѵ�. �Ϲ������� Static_Agro = XP_Static_Agro�̴�.
	INT32 lDamage = 0;
	m_pagpmFactors->GetValue(pcsAgpdFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP );
	m_pagpmFactors->SetValue(pcsAgpdFactor, lDamage, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_SLOW );

	AddHistory(pcsCharacter, pcsSource, pcsAgpdFactor, GetClockCount());

	return TRUE;
}

BOOL AgsmCharacter::DecreaseHistory( AgpdCharacter *pcsCharacter, eAgpdFactorAgroType eAgroType, INT32 lDecreasePoint )
{
	PROFILE("AgsmCharacter::DecreaseHistory");

	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);
	AgsdCharacterHistoryEntry *	pstEntry;
	INT32						lIndex;
	INT32						lFast, lSlow, lXP, lStatic;

	lFast = 0;
	lSlow = 0;
	lXP = 0;
	lStatic =0;

	for(lIndex = 0; lIndex < pstAgsdCharacter->m_stHistory.m_lEntryNum; ++lIndex)
	{
		if (pstAgsdCharacter->m_stHistory.m_astEntry[lIndex].m_pcsUpdateFactor == NULL)
			continue;

		pstEntry = &pstAgsdCharacter->m_stHistory.m_astEntry[lIndex];

		m_pagpmFactors->GetValue(pstEntry->m_pcsUpdateFactor, &lStatic, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_STATIC );
		m_pagpmFactors->GetValue(pstEntry->m_pcsUpdateFactor, &lSlow, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_SLOW );
		m_pagpmFactors->GetValue(pstEntry->m_pcsUpdateFactor, &lFast, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_FAST );

		//Ư���� ����ŭ �÷��ش�. ����~
		if( eAgroType == AGPD_FACTORS_AGRO_TYPE_STATIC )
		{
			lStatic += lDecreasePoint;
		}
		else if( eAgroType == AGPD_FACTORS_AGRO_TYPE_SLOW )
		{
			lSlow += lDecreasePoint;
		}
		else if( eAgroType == AGPD_FACTORS_AGRO_TYPE_FAST )
		{
			lFast += lDecreasePoint;
		}

		if( lStatic >= 0 )
			lStatic = 0;

		if( lSlow >= 0 )
			lSlow = 0;

		if( lFast >= 0 )
			lFast = 0;

		//���� ��׷ΰ��� �ٿ��µ� ��� ��׷ΰ��� 0�̵Ǹ� ����ġ�����丮���� ������Ų��.
		if( (lStatic == 0) && (lSlow == 0) && (lFast == 0) )
		{
			m_pagpmFactors->SetValue(pstEntry->m_pcsUpdateFactor, lStatic, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_STATIC );
			m_pagpmFactors->SetValue(pstEntry->m_pcsUpdateFactor, lSlow, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_SLOW );
			m_pagpmFactors->SetValue(pstEntry->m_pcsUpdateFactor, lFast, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_FAST );

			/*
			m_pagpmFactors->DestroyFactor(&pstEntry->m_csUpdateFactor, AGPD_FACTORS_TYPE_AGRO);

			memcpy(pstEntry, pstEntry + 1, sizeof(AgsdCharacterHistoryEntry) * (pstAgsdCharacter->m_stHistory.m_lEntryNum - lIndex - 1));

			memset(&pstAgsdCharacter->m_stHistory.m_astEntry[pstAgsdCharacter->m_stHistory.m_lEntryNum - 1], 0, sizeof(AgsdCharacterHistoryEntry));

			--(pstAgsdCharacter->m_stHistory.m_lEntryNum);
			*/
		}
		//�ٿ��������� �����Ѵ�.
		else
		{
			if( eAgroType == AGPD_FACTORS_AGRO_TYPE_STATIC )
				m_pagpmFactors->SetValue(pstEntry->m_pcsUpdateFactor, lStatic, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_STATIC );

			if( eAgroType == AGPD_FACTORS_AGRO_TYPE_SLOW )
				m_pagpmFactors->SetValue(pstEntry->m_pcsUpdateFactor, lSlow, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_SLOW );

			if( eAgroType == AGPD_FACTORS_AGRO_TYPE_FAST )
				m_pagpmFactors->SetValue(pstEntry->m_pcsUpdateFactor, lFast, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_FAST );
		}
	}

	return TRUE;
}

BOOL AgsmCharacter::AddHistory(AgpdCharacter *pcsCharacter, ApBase *pcsSource, AgpdFactor *pcsFactor, UINT32 ulTime)
{
	PROFILE("AgsmCharacter::AddHistory");

	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsSource);
	ASSERT(NULL != pcsFactor);

	if (!pcsCharacter || !pcsSource || !pcsFactor)
		return FALSE;

	// 2007.01.24. steeple
	// �ڱ� �ڽſ��� ��׷θ� ���� �� ���� �� �ƴѰ�
	if (pcsCharacter->m_lID == pcsSource->m_lID)
		return FALSE;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);

	ApBaseExLock				csSource;
	csSource.m_eType			= pcsSource->m_eType;
	csSource.m_lID				= pcsSource->m_lID;

	ApBaseExLock				csSourceParty;
	csSourceParty.m_eType		= APBASE_TYPE_PARTY;
	csSourceParty.m_lID			= AP_INVALID_PARTYID;

	// pcsSource�� character�̰� party�� ���Ե� ����� history�� �ִ� base�� party�� �Ѵ�.
	if (pcsSource->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgpdCharacter	*pcsSourceBase	= m_pcsAgpmCharacter->GetCharacter(pcsSource->m_lID);

		if (pcsSourceBase)
		{
			AgpdParty *pcsParty = m_pcsAgpmParty->GetParty(pcsSourceBase);
			if (pcsParty)
			{
				csSourceParty.m_lID		= pcsParty->m_lID;
			}
		}
	}

	// ���ο� ���� �����丮�� �߰��Ѵ�.
	BOOL	bResult = AddHistoryEntry(&pstAgsdCharacter->m_stHistory, (ApBase *) &csSource, pcsFactor, ulTime, csSourceParty.m_lID);

	// 2006.06.01. steeple
	// �ϴ��� �� TID �� �����Ѵ�.
	if(pcsSource->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgpdCharacter* pcsSrcCharacter = m_pcsAgpmCharacter->GetCharacter(pcsSource->m_lID);
		if(pcsSrcCharacter && pcsSrcCharacter->m_pcsCharacterTemplate)
			pstAgsdCharacter->m_stHistory.m_lLastEnemyTID = pcsSrcCharacter->m_pcsCharacterTemplate->m_lID;
	}

	//// 2006.06.01. steeple
	//// LastEntry �� ������ ������Ʈ
	//// �ʿ��� �� eType �� ID ��.
	//pstEntry->m_stLastEntry.m_csSource.m_eType = pcsSource->m_eType;
	//pstEntry->m_stLastEntry.m_csSource.m_lID = pcsSource->m_lID;

	//// ������ PC ���� MOB ���� �����Ѵ�.
	//if(pcsSource->m_eType == APBASE_TYPE_CHARACTER)
	//{
	//	AgpdCharacter* pcsSourceCharacter = m_pcsAgpmCharacter->GetCharacter(pcsSource->m_lID);
	//	if(pcsSourceCharacter && m_pcsAgpmCharacter->IsPC(pcsSourceCharacter))
	//	{
	//		pstEntry->m_eLastTargetType = AGPMPVP_TARGET_TYPE_NORMAL_PC;

	//		// �ڽ��� PC �϶��� ����������� Ȯ���Ѵ�.
	//		if(m_pcsAgpmCharacter->IsPC(pcsCharacter) &&
	//			m_pcsAgpmGuild->IsEnemyGuild(m_pcsAgpmGuild->GetJoinedGuildID(pcsCharacter),
	//										m_pcsAgpmGuild->GetJoinedGuildID(pcsSourceBase),
	//										TRUE))
	//		{
	//			pstEntry->m_eLastTargetType = AGPMPVP_TARGET_TYPE_ENEMY_GUILD;
	//		}
	//	}
	//	else if(m_pagpmCharacter->IsStatusSummoner(pcsCharacter) || m_pagpmCharacter->IsStatusTame(pcsCharacter) ||
	//			m_pagpmCharacter->IsStatusFixed(pcsCharacter))
	//	{
	//		pstEntry->m_eLastTargetType = AGPMPVP_TARGET_TYPE_MOB;
	//	}
	//}

	if (pcsFactor->m_pvFactor[3] &&
		(*((AgpdFactorCharPoint *) (*pcsFactor).m_pvFactor[3])).lValue[0] > 0)
		bResult	= bResult;

	// ��Ƽ���̸� ��Ƽ�� ���� �����丮�� �߰��Ѵ�.
	if (csSourceParty.m_lID	!= AP_INVALID_PARTYID)
		bResult &= AddHistoryEntry(&pstAgsdCharacter->m_stHistoryParty, (ApBase *) &csSourceParty, pcsFactor, ulTime);

	if (bResult)
		SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);

	return bResult;
}

BOOL AgsmCharacter::AddHistoryEntry(AgsdCharacterHistory *pstHistory, ApBase *pcsSource, AgpdFactor *pcsFactor, UINT32 ulTime, INT32 lPartyID)
{
	PROFILE("AgsmCharacter::AddHistoryEntry");

	if (!pstHistory ||
		!pcsSource ||
		!pcsFactor)
		return FALSE;

	AgsdCharacterHistoryEntry *	pstEntry	= NULL;
	INT32						lIndex;

	for (lIndex = 0; lIndex < pstHistory->m_lEntryNum; ++lIndex)
	{
		pstEntry = &pstHistory->m_astEntry[lIndex];

		if(pstEntry)
		{
			if (pstEntry->m_csSource.m_eType == pcsSource->m_eType && pstEntry->m_csSource.m_lID == pcsSource->m_lID)
			{
				break;
			}
		}
	}

	if (lIndex >= AGSD_CHARACTER_HISTORY_NUMBER)
		return FALSE;

	
	if (lIndex == pstHistory->m_lEntryNum)
	{
		++(pstHistory->m_lEntryNum);
		pstEntry = &pstHistory->m_astEntry[lIndex];
	}
	
	if(pstEntry)
	{
		pstEntry->m_csSource.m_eType = pcsSource->m_eType;
		pstEntry->m_csSource.m_lID = pcsSource->m_lID;
		pstEntry->m_bFirstHit = (lIndex == 0);
		pstEntry->m_ulLastHitTime = ulTime;

		if (pstEntry->m_pcsUpdateFactor == NULL)
		{
			pstEntry->m_pcsUpdateFactor	= m_pagpmFactors->AllocAgpdFactor(m_lFactorTypeIndex);
			m_pagpmFactors->InitFactor(pstEntry->m_pcsUpdateFactor);
		}

		if (!pstEntry->m_ulFirstHitTime || pstEntry->m_lPartyID != lPartyID)
		{
			pstEntry->m_ulFirstHitTime = ulTime;

			m_pagpmFactors->CopyFactor(pstEntry->m_pcsUpdateFactor, pcsFactor, TRUE);
		}
		else
		{
			m_pagpmFactors->CalcFactor(pstEntry->m_pcsUpdateFactor, pcsFactor, FALSE, FALSE, TRUE, FALSE);
		}
		
		pstEntry->m_lPartyID	= lPartyID;
	}

	return TRUE;
}

BOOL				AgsmCharacter::RemoveHistory(AgpdCharacter *pcsCharacter, ApBase *pcsSource)
{
	PROFILE("AgsmCharacter::RemoveHistory");

	if (!pcsCharacter || !pcsSource)
		return FALSE;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);
	AgsdCharacterHistoryEntry *	pstEntry;
	INT32						lIndex;

	for (lIndex = 0; lIndex < pstAgsdCharacter->m_stHistory.m_lEntryNum; ++lIndex)
	{
		pstEntry = &pstAgsdCharacter->m_stHistory.m_astEntry[lIndex];

		if(pstEntry)
		{
			if (pstEntry->m_csSource.m_eType == pcsSource->m_eType && pstEntry->m_csSource.m_lID == pcsSource->m_lID)
			{
				if (pstEntry->m_pcsUpdateFactor)
				{
					m_pagpmFactors->DestroyFactor(pstEntry->m_pcsUpdateFactor);
					m_pagpmFactors->FreeAgpdFactor(pstEntry->m_pcsUpdateFactor, m_lFactorTypeIndex);

					pstEntry->m_pcsUpdateFactor	= NULL;
				}

				pstAgsdCharacter->m_stHistory.m_astEntry.MemCopy(lIndex, pstEntry + 1, pstAgsdCharacter->m_stHistory.m_lEntryNum - lIndex - 1);
				pstAgsdCharacter->m_stHistory.m_astEntry.MemSet(pstAgsdCharacter->m_stHistory.m_lEntryNum - 1, 1);

				--(pstAgsdCharacter->m_stHistory.m_lEntryNum);

				return TRUE;
			}
		}
	}

	return FALSE;
}

// 2005.04.27. steeple
BOOL				AgsmCharacter::RemovePartyHistory(AgpdCharacter *pcsCharacter, ApBase *pcsSource)
{
	PROFILE("AgsmCharacter::RemovePartyHistory");

	if (!pcsCharacter || !pcsSource)
		return FALSE;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);
	AgsdCharacterHistoryEntry *	pstEntry;
	INT32						lIndex;

	for (lIndex = 0; lIndex < pstAgsdCharacter->m_stHistoryParty.m_lEntryNum; ++lIndex)
	{
		pstEntry = &pstAgsdCharacter->m_stHistoryParty.m_astEntry[lIndex];

		if (pstEntry->m_csSource.m_eType == pcsSource->m_eType && pstEntry->m_csSource.m_lID == pcsSource->m_lID)
		{
			if (pstEntry->m_pcsUpdateFactor)
			{
				m_pagpmFactors->DestroyFactor(pstEntry->m_pcsUpdateFactor);
				m_pagpmFactors->FreeAgpdFactor(pstEntry->m_pcsUpdateFactor, m_lFactorTypeIndex);

				pstEntry->m_pcsUpdateFactor	= NULL;
			}

			pstAgsdCharacter->m_stHistoryParty.m_astEntry.MemCopy(lIndex, pstEntry + 1, pstAgsdCharacter->m_stHistoryParty.m_lEntryNum - lIndex - 1);
			pstAgsdCharacter->m_stHistoryParty.m_astEntry.MemSet(pstAgsdCharacter->m_stHistoryParty.m_lEntryNum - 1, 1);

			--(pstAgsdCharacter->m_stHistoryParty.m_lEntryNum);

			return TRUE;
		}
	}

	return FALSE;
}

AgsdCharacterHistoryEntry *	AgsmCharacter::GetHistoryEntry(AgpdCharacter *pcsCharacter, ApBase *pcsSource)
{
	PROFILE("gsmCharacter::GetHistoryEntry");

	if (!pcsCharacter || !pcsSource)
		return NULL;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);
	AgsdCharacterHistoryEntry *	pstEntry;
	INT32						lIndex;

	for (lIndex = 0; lIndex < pstAgsdCharacter->m_stHistory.m_lEntryNum; ++lIndex)
	{
		pstEntry = &pstAgsdCharacter->m_stHistory.m_astEntry[lIndex];

		if(pstEntry)
		{
			if (pstEntry->m_csSource.m_eType == pcsSource->m_eType && pstEntry->m_csSource.m_lID == pcsSource->m_lID)
			{
				return pstEntry;
			}
		}
	}

	return NULL;
}

AgsdCharacterHistory* AgsmCharacter::GetHistory(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);

	return &pstAgsdCharacter->m_stHistory;
}

AgsdCharacterHistory* AgsmCharacter::GetPartyHistory(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);

	return &pstAgsdCharacter->m_stHistoryParty;
}

BOOL				AgsmCharacter::InitHistory(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);
	AgsdCharacterHistoryEntry *	pstEntry;
	INT32						lIndex;

	for (lIndex = 0; lIndex < pstAgsdCharacter->m_stHistory.m_lEntryNum; ++lIndex)
	{
		pstEntry = &pstAgsdCharacter->m_stHistory.m_astEntry[lIndex];

		if (pstEntry->m_pcsUpdateFactor)
		{
			m_pagpmFactors->DestroyFactor(pstEntry->m_pcsUpdateFactor);
			m_pagpmFactors->FreeAgpdFactor(pstEntry->m_pcsUpdateFactor, m_lFactorTypeIndex);

			pstEntry->m_pcsUpdateFactor	= NULL;
		}
	}

	memset(&pstAgsdCharacter->m_stHistory, 0, sizeof(AgsdCharacterHistory));

	return TRUE;
}

// 2005.04.26. steeple
BOOL AgsmCharacter::InitPartyHistory(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *				pstAgsdCharacter = GetADCharacter(pcsCharacter);
	AgsdCharacterHistoryEntry *	pstEntry;
	INT32						lIndex;

	for (lIndex = 0; lIndex < pstAgsdCharacter->m_stHistoryParty.m_lEntryNum; ++lIndex)
	{
		pstEntry = &pstAgsdCharacter->m_stHistoryParty.m_astEntry[lIndex];

		if (pstEntry->m_pcsUpdateFactor)
		{
			m_pagpmFactors->DestroyFactor(pstEntry->m_pcsUpdateFactor);
			m_pagpmFactors->FreeAgpdFactor(pstEntry->m_pcsUpdateFactor, m_lFactorTypeIndex);

			pstEntry->m_pcsUpdateFactor	= NULL;
		}
	}

	memset(&pstAgsdCharacter->m_stHistoryParty, 0, sizeof(AgsdCharacterHistory));

	return TRUE;
}

AgsdCharacterHistoryEntry* AgsmCharacter::GetMostDamager(AgpdCharacter *pcsCharacter)
{
	PROFILE("AgsmCharacter::GetMostDamager");

	if (!pcsCharacter)
		return NULL;

	// history�� ������ ���� ū �������� �س��� �������� ã�´�.

	INT32						lMostDamage	= 0;
	AgsdCharacterHistoryEntry	*pcsEntry	= NULL;

	AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
	{
		AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

		if (pcsEntryTemp->m_lPartyID != AP_INVALID_PARTYID ||
			pcsEntryTemp->m_pcsUpdateFactor == NULL)
			continue;

		// damage�� ������ �����´�.
		INT32	lDamage = 0;
		m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

		if (lDamage < lMostDamage)
		{
			lMostDamage = lDamage;
			pcsEntry = pcsEntryTemp;
		}
	}

	pcsHistory	= GetPartyHistory(pcsCharacter);
	if (pcsHistory)
	{
		for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
		{
			AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

			if (pcsEntryTemp->m_pcsUpdateFactor == NULL)
				continue;

			// damage�� ������ �����´�.
			INT32	lDamage = 0;
			m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

			if (lDamage < lMostDamage)
			{
				lMostDamage = lDamage;
				pcsEntry = pcsEntryTemp;
			}
		}
	}

	return pcsEntry;
}

AgsdCharacterHistoryEntry* AgsmCharacter::GetMostAgroChar(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	// history�� ������ ���� ū ��׷θ� �س��� �������� ã�´�.
	AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	AgsdCharacterHistoryEntry	*pcsMostAgroEntry	= NULL;

	INT32 lMostAgro = 0;
	INT32 lAgro = 0;
	INT32 lStatic = 0;
	INT32 lSlow = 0;
	INT32 lFast = 0;

	for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
	{
		AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

		if( pcsEntryTemp->m_pcsUpdateFactor)
		{
			m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lStatic, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_STATIC );
			m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lSlow, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_SLOW );
			m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lFast, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_FAST );

			//pcsMostAgroEntry = pcsEntryTemp;

			lAgro = lStatic + lSlow + lFast;

			if (lAgro < lMostAgro)
			{
				lMostAgro = lAgro;
				pcsMostAgroEntry = pcsEntryTemp;
			}
		}
	}

	return pcsMostAgroEntry;
}

// 2005.04.22. steeple
AgsdCharacterHistoryEntry* AgsmCharacter::GetFinalAttacker(AgpdCharacter* pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	INT32						lMostDamage	= 0;
	AgsdCharacterHistoryEntry	*pcsEntry	= NULL;

	AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	//// 2006.06.01. steeple
	//// �׳� LastEntry �����Ѵ�
	//return &pcsHistory->m_stLastEntry;

	if(pcsHistory->m_lEntryNum < 1)
		return NULL;

	INT32	lLastHitIndex	= 0;

	for (int i = 1; i < pcsHistory->m_lEntryNum; ++i)
	{
		if (pcsHistory->m_astEntry[lLastHitIndex].m_ulLastHitTime < pcsHistory->m_astEntry[i].m_ulLastHitTime)
			lLastHitIndex	= i;
	}

	return &pcsHistory->m_astEntry[lLastHitIndex];
}

// 2005.04.22. steeple
AgsdCharacterHistoryEntry * AgsmCharacter::GetFinalAttackerInParty(AgpdCharacter* pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	INT32						lMostDamage	= 0;
	AgsdCharacterHistoryEntry	*pcsEntry	= NULL;

	AgsdCharacterHistory	*pcsHistory = GetPartyHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	//// 2006.06.01. steeple
	//// �׳� LastEntry �����Ѵ�
	//return &pcsHistory->m_stLastEntry;

	if(pcsHistory->m_lEntryNum < 1)
		return NULL;

	INT32	lLastHitIndex	= 0;

	for (int i = 1; i < pcsHistory->m_lEntryNum; ++i)
	{
		if (pcsHistory->m_astEntry[lLastHitIndex].m_ulLastHitTime < pcsHistory->m_astEntry[i].m_ulLastHitTime)
			lLastHitIndex	= i;
	}

	return &pcsHistory->m_astEntry[lLastHitIndex];
}

AgsdCharacterHistoryEntry* AgsmCharacter::GetMostDamagerInParty(AgpdCharacter *pcsCharacter, INT32 lPartyID)
{
	PROFILE("AgsmCharacter::GetMostDamagerInParty");

	if (!pcsCharacter || lPartyID == AP_INVALID_PARTYID)
		return NULL;

	// history�� ������ ���� ū �������� �س��� �������� ã�´�.

	INT32						lMostDamage	= 0;
	AgsdCharacterHistoryEntry	*pcsEntry	= NULL;

	AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
	{
		AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

		if (pcsEntryTemp->m_lPartyID != lPartyID ||
			pcsEntryTemp->m_pcsUpdateFactor == NULL)
			continue;

		// damage�� ������ �����´�.
		INT32	lDamage = 0;
		m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

		if (lDamage < lMostDamage)
		{
			lMostDamage = lDamage;
			pcsEntry = pcsEntryTemp;
		}
	}

	return pcsEntry;
}

BOOL  AgsmCharacter::IsDamager( AgpdCharacter *pcsCharacter, INT32 lTargetCID )
{
	if (!pcsCharacter)
		return NULL;

	AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	AgsdCharacterHistoryEntry	*pcsEntry	= NULL;

	BOOL				bResult;

	bResult = FALSE;

	for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
	{
		AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

		// damage�� �س������� ����.
		if( pcsEntryTemp->m_csSource.m_lID == lTargetCID )
		{
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgsmCharacter::IsNoAgro( AgpdCharacter *pcsCharacter )
{
	if (!pcsCharacter)
		return NULL;

	AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	AgsdCharacterHistoryEntry	*pcsEntry	= NULL;

	BOOL				bResult;

	bResult = TRUE;

	for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
	{
		AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

		// damage�� �س��� �ִ�! �̿���~
		if( pcsEntryTemp->m_csSource.m_lID )
		{
			bResult = FALSE;
			break;
		}
	}

	return bResult;
}

INT32 AgsmCharacter::GetAgroFromEntry( AgpdCharacter *pcsCharacter, INT32 lTargetCID )
{
	PROFILE("AgsmCharacter::GetDamageFromEntry");

	if (!pcsCharacter)
		return NULL;

	AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
	if (!pcsHistory)
		return NULL;

	AgsdCharacterHistoryEntry	*pcsEntry	= NULL;

	BOOL				bResult;
	INT32				lAgro;

	INT32				lStatic;
	INT32				lSlow;
	INT32				lFast;

	bResult = FALSE;
	lAgro = 0;
	lStatic = 0;
	lSlow = 0;
	lFast = 0;

	for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
	{
		AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

		if( pcsEntryTemp->m_csSource.m_lID == lTargetCID &&
			pcsEntryTemp->m_pcsUpdateFactor)
		{
			m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lStatic, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_STATIC );
			m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lSlow, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_SLOW );
			m_pagpmFactors->GetValue(pcsEntryTemp->m_pcsUpdateFactor, &lFast, AGPD_FACTORS_TYPE_AGRO, AGPD_FACTORS_AGRO_TYPE_FAST );

			lAgro = lStatic + lSlow + lFast;

			break;
		}
	}

	return lAgro;
}

INT32 AgsmCharacter::GetMobListFromHistoryEntry( AgpdCharacter *pcsCharacter, INT32 *alMobList, INT32 lArraryCount )
{
	INT32			lMobListIndex;

	lMobListIndex = 0;

	if( pcsCharacter )
	{
		AgsdCharacterHistory	*pcsHistory = GetHistory(pcsCharacter);
		if (!pcsHistory)
			return lMobListIndex;

		memset( alMobList, 0, sizeof( INT32 ) * lArraryCount );

		for (int lIndex = 0; lIndex < pcsHistory->m_lEntryNum; ++lIndex)
		{
			AgsdCharacterHistoryEntry	*pcsEntryTemp = &pcsHistory->m_astEntry[lIndex];

			if( pcsEntryTemp->m_csSource.m_lID != 0 )
			{
				alMobList[lMobListIndex] = pcsEntryTemp->m_csSource.m_lID;

				++lMobListIndex;

				if( lMobListIndex >= lArraryCount )
				{
					break;
				}
			}
		}
	}

	return lMobListIndex;
}

BOOL AgsmCharacter::SetParamMasterInsertQuery5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter, INT8 cSlotIndex, CHAR *pszServerName)
	{
	if (!pDatabase || !pcsCharacter)
		return FALSE;

	AgsdCharacter		*pcsAgsdCharacter = GetADCharacter( pcsCharacter );

	if( !pcsAgsdCharacter )
		return FALSE;
	
	if (0 > _tcslen(GetRealCharName(pcsCharacter->m_szID))
		|| 0 > _tcslen(pcsAgsdCharacter->m_szAccountID)
		|| 0 > _tcslen(pszServerName)
		)
		return FALSE;
	
	pDatabase->SetParam(0, GetRealCharName(pcsCharacter->m_szID), sizeof(pcsCharacter->m_szID));
	pDatabase->SetParam(1, pcsAgsdCharacter->m_szAccountID, sizeof(pcsAgsdCharacter->m_szAccountID));
	pDatabase->SetParam(2, &cSlotIndex);
	pDatabase->SetParam(3, &pcsCharacter->m_pcsCharacterTemplate->m_lID);
	pDatabase->SetParam(4, pszServerName, (INT32)(sizeof(CHAR) * (_tcslen(pszServerName)+1)));

	if (AUDATABASE2_QR_SUCCESS == pDatabase->ExecuteQuery())
		return TRUE;
	
	return FALSE;
}

BOOL AgsmCharacter::SetParamDetailInsertQuery5(AuDatabase2 *pDatabase, AgpdCharacter *pcsCharacter, CHAR *pszQuickBelt)
{
	BOOL				bResult = FALSE;

	if( pcsCharacter != NULL )
	{
		AgsdCharacter		*pcsAgsdCharacter;

		pcsAgsdCharacter = GetADCharacter( pcsCharacter );

		if( pcsAgsdCharacter != NULL )
		{
			INT32				lHP;
			INT32				lMP;
			INT32				lSP;
			INT64				llEXP;
			CHAR				szEXP[21];
			ZeroMemory(szEXP, sizeof(szEXP));
			INT32				lLV;
			INT32				lSkillPoint;
			INT32				lMurderPoint;
			
			INT32				lRCF = 0;
			INT32				lRMF = 0;
			CHAR				szMoney[21];
			ZeroMemory(szMoney, sizeof(szMoney));
			_i64toa(pcsCharacter->m_llMoney, szMoney, 10);

			lHP = m_pagpmFactors->GetHP( &pcsCharacter->m_csFactor );
			lMP = m_pagpmFactors->GetMP( &pcsCharacter->m_csFactor );
			lSP = m_pagpmFactors->GetSP( &pcsCharacter->m_csFactor );
			llEXP = m_pagpmFactors->GetExp( &pcsCharacter->m_csFactor );
			_i64toa(llEXP, szEXP, 10);
			lLV = m_pcsAgpmCharacter->GetLevel( pcsCharacter );
			lSkillPoint = m_pcsAgpmCharacter->GetSkillPoint(pcsCharacter);
			lMurderPoint = m_pagpmFactors->GetMurderPoint( &pcsCharacter->m_csFactor );

			CHAR	szPosition[33];
			ZeroMemory(szPosition, sizeof(CHAR) * 33);

			EncodingPosition(&pcsCharacter->m_stPos, szPosition, 32);

			INT16 nIndex = 0;
			pDatabase->SetParam(nIndex++, GetRealCharName(pcsCharacter->m_szID), sizeof(pcsCharacter->m_szID));
			pDatabase->SetParam(nIndex++, pcsCharacter->m_szSkillInit, sizeof(pcsCharacter->m_szSkillInit));
			pDatabase->SetParam(nIndex++, szPosition, sizeof(szPosition));
			pDatabase->SetParam(nIndex++, &pcsCharacter->m_unCriminalStatus);
			pDatabase->SetParam(nIndex++, (INT16 *)&lMurderPoint);
			pDatabase->SetParam(nIndex++, &lRCF);
			pDatabase->SetParam(nIndex++, &lRMF);
			pDatabase->SetParam(nIndex++, szMoney, sizeof(szMoney));
			pDatabase->SetParam(nIndex++, &lHP);
			pDatabase->SetParam(nIndex++, &lMP);
			pDatabase->SetParam(nIndex++, &lSP);
			pDatabase->SetParam(nIndex++, szEXP, sizeof(szEXP));
			pDatabase->SetParam(nIndex++, (INT8 *) &lLV);
			pDatabase->SetParam(nIndex++, &lSkillPoint);
			pDatabase->SetParam(nIndex++, (pszQuickBelt && pszQuickBelt[0]) ? pszQuickBelt : "",
								        (pszQuickBelt && pszQuickBelt[0]) ? (INT32)(sizeof(CHAR) * (strlen(pszQuickBelt)+1)) : (INT32)sizeof(CHAR));
			pDatabase->SetParam(nIndex++, (INT8 *) &pcsCharacter->m_nLastExistBindingIndex);
			pDatabase->SetParam(nIndex++, &pcsCharacter->m_unActionStatus);
			pDatabase->SetParam(nIndex++, &pcsCharacter->m_lHairIndex);
			pDatabase->SetParam(nIndex++, &pcsCharacter->m_lFaceIndex);

			if (AUDATABASE2_QR_SUCCESS == pDatabase->ExecuteQuery())
				bResult = TRUE;
		}
	}

	return bResult;
}

INT32 AgsmCharacter::GetMasterSelectResult5(AuDatabase2 *pDatabase, INT8 &cSlot)
{
	if (!pDatabase)
		return AP_INVALID_CID;

	AgpdCharacter		*pcsCharacter	= NULL;

	// DB���� �о�� ����Ÿ�� ������.
	CHAR	szName[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	INT32	lTemplateID	= AP_INVALID_CID;

	CHAR	szAccountName[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];

	CHAR	szServerName[AGSM_MAX_SERVER_NAME + 1];

	INT32	lSlotIndex = 0;

	INT32	lCID = 0;

	CHAR	*szBuffer = NULL;

	ZeroMemory(szName, sizeof(szName));
	ZeroMemory(szAccountName, sizeof(szAccountName));
	ZeroMemory(szServerName, sizeof(szServerName));

	// ������ �����´�.
	INT16	nIndex = 0;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szName, szBuffer, AGPDCHARACTER_MAX_ID_LENGTH);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szAccountName, szBuffer, AGSMACCOUNT_MAX_ACCOUNT_NAME);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szServerName, szBuffer, AGSM_MAX_SERVER_NAME);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lSlotIndex	= (INT32) atof(szBuffer);

	cSlot = (INT8) lSlotIndex;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lTemplateID	= (INT32) atoi(szBuffer);

	if (strlen(szServerName) > AGSM_MAX_SERVER_NAME)
		return 0;

	EnumCallback(AGSMCHARACTER_CB_GET_NEW_CID, &lCID, NULL);

	if (0 == lCID || _T('\0') == szAccountName[0])
		return 0;

	// ������ �������� ������ character ����Ÿ�� public ��⿡ �߰��Ѵ�.
	AgpdCharacterTemplate	*pcsTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(lTemplateID);
	if (!pcsTemplate )
		return 0;

	AgpdCharacter	*pcsExistCharacter	= m_pcsAgpmCharacter->GetCharacter(lCID);
	if (pcsExistCharacter)
	{
		AgsdCharacter	*pcsAgsdExistCharacter	= GetADCharacter(pcsExistCharacter);
		pcsAgsdExistCharacter->m_dpnidCharacter	= 0;

		m_pcsAgpmCharacter->RemoveCharacter(lCID);
	}

	CHAR	szNewName[AGPDCHARACTER_MAX_ID_LENGTH * 2];
	ZeroMemory(szNewName, sizeof(szNewName));

	AddServerNameToCharName(szName, szServerName, szNewName);

	pcsExistCharacter	= m_pcsAgpmCharacter->GetCharacter(szNewName);
	if (pcsExistCharacter)
	{
		AgsdCharacter	*pcsAgsdExistCharacter	= GetADCharacter(pcsExistCharacter);
		pcsAgsdExistCharacter->m_dpnidCharacter	= 0;

		m_pcsAgpmCharacter->RemoveCharacter(szNewName);
	}

	m_pcsAgpmCharacter->AddCharacter(lCID, pcsTemplate->m_lID, szNewName);

	pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock( lCID );

	if( !pcsCharacter )
		return 0;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);

	ZeroMemory(pcsAgsdCharacter->m_szAccountID, sizeof(pcsAgsdCharacter->m_szAccountID));
	strncpy(pcsAgsdCharacter->m_szAccountID, szAccountName, AGSMACCOUNT_MAX_ACCOUNT_NAME);

	ZeroMemory(pcsAgsdCharacter->m_szServerName, sizeof(pcsAgsdCharacter->m_szServerName));
	strncpy(pcsAgsdCharacter->m_szServerName, szServerName, AGSM_MAX_SERVER_NAME);

	pcsCharacter->m_Mutex.Release();
	
	return lCID;
}

BOOL AgsmCharacter::GetDetailSelectResult5( AgpdCharacter *pcsCharacter, AuDatabase2 *pDatabase )
{
	if (!pcsCharacter || !pDatabase)
		return FALSE;

	// DB���� �о�� ����Ÿ�� ������.
	CHAR	szName[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	CHAR	szSkillInit[AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1];
	CHAR	szNickName[AGPACHARACTER_MAX_CHARACTER_NICKNAME + 1];

	CHAR	szPosition[65];

	INT32	lCriminalStatus	= (-0);
	INT32	lMurdererPoint	= (-0);

	INT64	llInventoryMoney		= (-0);
	INT32	lRemainCriminalFlagTimeMsec	= (-1);
	INT32	lRemainMurdererPointTimeMsec = (-1);

	INT32	lHP		= (-0);
	INT32	lMP		= (-0);
	INT32	lSP		= (-0);
	INT64	llEXP	= (-0);
	INT32	lLevel	= (-0);
	INT32	lSkillPoint	= (-0);
	INT32	lCharismaPoint = (-0);

	INT32	lRegionIndex	= (-0);
	INT32	lActionStatus	= (-0);

	CHAR	szQBeltString[AGPMUISTATUS_MAX_QUICKBELT_STRING + 1];
	CHAR	szCooldown[AGPMUISTATUS_MAX_COOLDOWN_STRING + 1];

	CHAR	szResurrectDate[33];
	CHAR	szTransformDate[33];

	UINT32	ulBattleSquareLastKilledDate;

	INT32	lHair	= 0;
	INT32	lFace	= 0;

	CHAR	*szBuffer = NULL;

	eAgpmPvPTargetType	eDeadType	= AGPMPVP_TARGET_TYPE_MOB;

	ZeroMemory(szName, sizeof(szName));
	ZeroMemory(szSkillInit, sizeof(szSkillInit));
	ZeroMemory(szNickName, sizeof(szNickName));
	ZeroMemory(szPosition, sizeof(szPosition));
	ZeroMemory(szQBeltString, sizeof(szQBeltString));
	ZeroMemory(szResurrectDate, sizeof(szResurrectDate));
	ZeroMemory(szTransformDate, sizeof(szTransformDate));
	ZeroMemory(szCooldown, sizeof(szCooldown));

	// ������ �����´�.
	INT16	nIndex = 0;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szName, szBuffer, AGPDCHARACTER_MAX_ID_LENGTH);

	// �����ߴ�.. �پ����.
	nIndex = 5;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szSkillInit, szBuffer, AGPACHARACTER_MAX_CHARACTER_SKILLINIT);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szNickName, szBuffer, AGPACHARACTER_MAX_CHARACTER_NICKNAME);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL && szBuffer[0])
	{
		strncpy(szPosition, szBuffer, 64);
		DecodingPosition(szPosition, &pcsCharacter->m_stPos);
	}
	else
	{
		m_pcsAgpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);
	}

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lCriminalStatus	= (INT32) atoi(szBuffer);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lMurdererPoint	= (INT32) atof(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lRemainCriminalFlagTimeMsec	= (INT32) atof(szBuffer);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lRemainMurdererPointTimeMsec	= (INT32) atof(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		llInventoryMoney	= (INT64) _atoi64(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL && szBuffer[0])
		lHP	= (INT32) atof(szBuffer);
	else
		lHP = INT_MAX;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL && szBuffer[0])
		lMP	= (INT32) atof(szBuffer);
	else
		lMP = INT_MAX;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL && szBuffer[0])
		lSP	= (INT32) atof(szBuffer);
	else
		lSP = INT_MAX;

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		llEXP	= (INT64) _atoi64(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lLevel	= (INT32) atof(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lSkillPoint	= (INT32) atof(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szQBeltString, szBuffer, AGPMUISTATUS_MAX_QUICKBELT_STRING);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lRegionIndex = (INT32) atoi(szBuffer);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lActionStatus = (INT32) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lHair	= (INT32) atoi(szBuffer);
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lFace	= (INT32) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szResurrectDate, szBuffer, 32);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szTransformDate, szBuffer, 32);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		eDeadType	= (eAgpmPvPTargetType) atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		lCharismaPoint = atoi(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		strncpy(szCooldown, szBuffer, AGPMUISTATUS_MAX_COOLDOWN_STRING);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		ulBattleSquareLastKilledDate = (UINT32) atoi(szBuffer);

	
	// ������ �Ϸ�

	// ������ ���鿡 ���� ������ �Ѵ�.

	if (llInventoryMoney != (-0))
		pcsCharacter->m_llMoney	= llInventoryMoney;

	ZeroMemory(pcsCharacter->m_szSkillInit, sizeof(pcsCharacter->m_szSkillInit));
	strncpy(pcsCharacter->m_szSkillInit, szSkillInit, AGPACHARACTER_MAX_CHARACTER_SKILLINIT);

	ZeroMemory(pcsCharacter->m_szNickName, sizeof(pcsCharacter->m_szNickName));
	strncpy(pcsCharacter->m_szNickName, szNickName, AGPACHARACTER_MAX_CHARACTER_NICKNAME);

	// criminal, murderer
	m_pcsAgpmCharacter->SetLastCriminalTime(pcsCharacter, lRemainCriminalFlagTimeMsec);
	m_pcsAgpmCharacter->SetLastMurdererTime(pcsCharacter, lRemainMurdererPointTimeMsec);
	m_pcsAgpmCharacter->SetLastKilledTimeInBattleSquare(pcsCharacter, ulBattleSquareLastKilledDate);

	if (lCriminalStatus != (-0))
		pcsCharacter->m_unCriminalStatus	= (INT8) lCriminalStatus;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);

	if (lLevel == (-0))
	{
		pcsAgsdCharacter->m_bNeedReinitialize = TRUE;
		lLevel = 1;
	}
	SetCharacterLevel(pcsCharacter, lLevel, FALSE);

	if (lMurdererPoint != (-0))
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);

	if (lHP != (-0))
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	if (lMP != (-0))
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);

	if (lSP != (-0))
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	if (llEXP != (-0))
	{
		// 2006.05.05. steeple
		// �׾����� �� ������, HP, MP, SP �� 0 �̾ CHAR_POINT Factor �� �ʱ�ȭ�� �ȵǾ� �ִ�.
		// �׷��� �׳� ������ SetFactor �ѹ� ���ش�.
		if(lHP == (-0) && lMP == (-0) && lSP == (-0))
			m_pagpmFactors->SetFactor(&pcsCharacter->m_csFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);

		m_pagpmFactors->SetExp(&pcsCharacter->m_csFactor, llEXP);
	}

	if (lSkillPoint != (-0))
	{
		INT32	lCurrentSkillPoint	= m_pcsAgpmCharacter->GetSkillPoint(pcsCharacter);

		m_pcsAgpmCharacter->UpdateSkillPoint(pcsCharacter, lSkillPoint - lCurrentSkillPoint);
	}

	if (lRegionIndex != (-0))
	{
		pcsCharacter->m_nBindingRegionIndex		= (UINT8)	lRegionIndex;
		pcsCharacter->m_nLastExistBindingIndex	= (UINT8)	lRegionIndex;
	}

	if (lActionStatus != (-0))
	{
		if ((INT8) lActionStatus == AGPDCHAR_STATUS_DEAD)
			pcsCharacter->m_unActionStatus	= (INT8) lActionStatus;
		else
			pcsCharacter->m_unActionStatus	= AGPDCHAR_STATUS_NORMAL;
	}

	if (lCharismaPoint != (-0))
	{
		m_pcsAgpmCharacter->UpdateCharismaPoint(pcsCharacter, lCharismaPoint);
	}

	if (lFace >= 0)
		pcsCharacter->m_lFaceIndex	= lFace;
	if (lHair >= 0)
		pcsCharacter->m_lHairIndex	= lHair;

	if (strlen(szQBeltString) > 0)
		m_pagpmUIStatus->SetQBeltEncodedString(pcsCharacter, szQBeltString, (INT32)strlen(szQBeltString));

	if (strlen(szCooldown) > 0)
		m_pagpmUIStatus->SetCooldownEncodedString(pcsCharacter, szCooldown, (INT32)strlen(szCooldown));

	if (strlen(szResurrectDate) > 0) 
	{
		SYSTEMTIME	stResurrect;

		stResurrect.wDayOfWeek	= 0;

		sscanf(szResurrectDate, "%d-%d-%d %d:%d:%d", &stResurrect.wYear,
													&stResurrect.wMonth,
													&stResurrect.wDay,
													&stResurrect.wHour,
													&stResurrect.wMinute,
													&stResurrect.wSecond);

		SYSTEMTIME	stCurrent;
		::GetLocalTime(&stCurrent);

		FILETIME	ftResurrect;
		FILETIME	ftCurrent;

		::SystemTimeToFileTime(&stResurrect, &ftResurrect);
		::SystemTimeToFileTime(&stCurrent, &ftCurrent);

		UINT64	ulCurrentTime;
		UINT64	ulResurrectTime;

		::CopyMemory(&ulCurrentTime, &ftCurrent, sizeof(ULARGE_INTEGER));
		::CopyMemory(&ulResurrectTime, &ftResurrect, sizeof(ULARGE_INTEGER));

		UINT64	ulDiffTime	= (ulCurrentTime - ulResurrectTime) / 10000;

// 		AgpdItemTemplate	*pcsItemTemplate	= m_pagpmItem->GetItemTemplate(1054);
// 		if (pcsItemTemplate)
// 		{
// 			INT64	llReuseTime	= (INT64) ((AgpdItemTemplateUsable *) pcsItemTemplate)->m_ulUseInterval - ulDiffTime;
// 
// 			if (llReuseTime < 0)
// 				llReuseTime	= 0;
// 
// 			m_pagpmItem->UpdateReverseOrbReuseTime(pcsCharacter, (UINT32) llReuseTime);
// 		}

		INT64	llReuseTime	= (INT64) AGPMITEM_REVERSE_ORB_REUSE_INTERVAL - ulDiffTime;

		if (llReuseTime < 0)
			llReuseTime	= 0;

		m_pagpmItem->UpdateReverseOrbReuseTime(pcsCharacter, (UINT32) llReuseTime);

	}

	//##################################################
	//���� ����� ó�� ���⼭... szTransformDate
	if (strlen(szTransformDate) > 0) 
	{
		SYSTEMTIME	stTransformTIme;

		stTransformTIme.wDayOfWeek	= 0;

		sscanf(szTransformDate, "%d-%d-%d %d:%d:%d", &stTransformTIme.wYear,
													&stTransformTIme.wMonth,
													&stTransformTIme.wDay,
													&stTransformTIme.wHour,
													&stTransformTIme.wMinute,
													&stTransformTIme.wSecond);

		SYSTEMTIME	stCurrent;
		::GetLocalTime(&stCurrent);

		FILETIME	ftTransform;
		FILETIME	ftCurrent;

		::SystemTimeToFileTime(&stTransformTIme, &ftTransform);
		::SystemTimeToFileTime(&stCurrent, &ftCurrent);

		UINT64	ulCurrentTime;
		UINT64	ulTransformTime;

		::CopyMemory(&ulCurrentTime, &ftCurrent, sizeof(ULARGE_INTEGER));
		::CopyMemory(&ulTransformTime, &ftTransform, sizeof(ULARGE_INTEGER));

		UINT64	ulDiffTime	= (ulCurrentTime - ulTransformTime) / 10000;

		INT64	llReuseTime	= (INT64) AGPMITEM_TRANSFORM_RECAST_TIME - ulDiffTime;

		if (llReuseTime < 0)
			llReuseTime	= 0;

		m_pagpmItem->UpdateTransformReuseTime(pcsCharacter, (UINT32) llReuseTime);
	}

	AgpdPvPADChar* pcsAttachedPvP = m_pcsAgpmPvP->GetADCharacter(pcsCharacter);
	if (pcsAttachedPvP)
        pcsAttachedPvP->m_cDeadType	= (INT8) eDeadType;

	return TRUE;
}

BOOL AgsmCharacter::GetAccountWorldSelectResult5(AgpdCharacter *pcsCharacter, AuDatabase2 *pDatabase)
{
	if (!pcsCharacter || !pDatabase)
		return FALSE;

	CHAR	*szBuffer	= NULL;
	INT16	nIndex		= 0;

	// accountid
	szBuffer = pDatabase->GetQueryResult(nIndex++);

	// bankmoney
	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		pcsCharacter->m_llBankMoney	= (INT64) _atoi64(szBuffer);

	if ((szBuffer = pDatabase->GetQueryResult(nIndex++)) != NULL)
		pcsCharacter->m_cBankSize	= (INT8) atoi(szBuffer);

	return TRUE;
}



BOOL AgsmCharacter::BackupCharacterData(AgpdCharacter *pcsCharacter)
{
	PROFILE("AgsmCharacter::BackupCharacterData");

	//STOPWATCH2(GetModuleName(), _T("BackupCharacterData"));

	if (!pcsCharacter || m_pcsAgpmCharacter->IsReturnToLoginServerStatus(pcsCharacter))
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return FALSE;

	if (pcsAgsdCharacter->m_ulNextBackupCharacterDataTime <= 0)
	{
		pcsAgsdCharacter->m_ulNextBackupCharacterDataTime = GetClockCount() + AGSMCHARACTER_SAVE_CHARACTER_DB_INTERVAL;
		return TRUE;
	}

	pcsAgsdCharacter->m_llPrevUpdateMoney	= pcsCharacter->m_llMoney;

	// Game Server�� ��찡 �ƴϸ� ���⼱ ������ �ϸ� �ȵȴ�.
	/////////////////////////////////////////////////////////////////////////////
	
	AgsdServer		*pcsThisServer		= m_pAgsmServerManager2->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	if ((eAGSMSERVER_TYPE) pcsThisServer->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
		return TRUE;

	// EnterGameWorld() ȣ��Ǳ� �����̸� ���� ������ ���Ѵ�.
	/////////////////////////////////////////////////////////////////////////////
	if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD)
		return TRUE;

	// Relay Server�� DB�� �����ؾ��� ����Ÿ�� ������.
	/////////////////////////////////////////////////////////////////////////////

	AgsdServer		*pcsRelayServer		= m_pAgsmServerManager2->GetRelayServer();
	if (pcsRelayServer && pcsRelayServer->m_dpnidServer != 0)
	{
		/*
		if (!SendPacketDBData(pcsCharacter, pcsRelayServer->m_dpnidServer))
		{
			return FALSE;
		}
		*/

		EnumCallback(AGSMCHARACTER_CB_UPDATE_CHARACTER_TO_DB, pcsCharacter, NULL);
	}

	pcsAgsdCharacter->m_ulNextBackupCharacterDataTime	= GetClockCount() + AGSMCHARACTER_SAVE_CHARACTER_DB_INTERVAL;

	pcsAgsdCharacter->m_ulPlayLogCount = (pcsAgsdCharacter->m_ulPlayLogCount + 1) % AGSM_CHARACTER_PLAY_LOG_TICK_TERM;
	if (0 == pcsAgsdCharacter->m_ulPlayLogCount)
		WritePlayLog(pcsCharacter, 0);
	
	return TRUE;
}

BOOL AgsmCharacter::UpdateBankToDB(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;
	
	return EnumCallback(AGSMCHARACTER_CB_UPDATE_BANK_TO_DB, pcsCharacter, NULL);
}

BOOL AgsmCharacter::SetCallbackCharacterUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_UPDATE_CHARACTER_TO_DB, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackBankUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_UPDATE_BANK_TO_DB, pfCallback, pClass);
}

BOOL AgsmCharacter::SetCallbackAllUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pfCallback, pClass);
}


//		GetGiveSkillPoint
//	Functions
//		- pcsCharacter���� skill point�� ����ϴ��� �˻��Ѵ�.
//			(exp�� ���� AGPMCHAR_LEVELUP_SKILL_POINT��ŭ�� ������ �Ѿ�����.. �˻��Ѵ�)
//	Arguments
//		- pcsCharacter : ĳ���� 
//	Return value
//		- INT32 : ����ϴ��� ����
///////////////////////////////////////////////////////////////////////////////
INT32 AgsmCharacter::GetGiveSkillPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	INT64	llCurrentExp = m_pcsAgpmCharacter->GetExp(pcsCharacter);
	INT64	llLevelUpExp = m_pcsAgpmCharacter->GetLevelUpExp(pcsCharacter);
	INT32	lCharLevel	= m_pcsAgpmCharacter->GetLevel(pcsCharacter);

	if (llLevelUpExp < 1)
		return 0;

	INT32	lIndexTable	= 0;
	if (lCharLevel <= 50)
		lIndexTable	= 0;
	else
		lIndexTable	= 1;

	INT64	llMaxExp	= m_pagpmFactors->GetMaxExp(&pcsCharacter->m_csFactor);
	if (llMaxExp <= 0)
		return 0;

	INT32	lCurrentExpPercent	= (INT32) (llCurrentExp / (llMaxExp + 0.0) * 100);

	if (g_alReceivedSkillPoint[lIndexTable][0] > lCurrentExpPercent)
		return 0;

	int i = 0;
	for (i = 1; i < AGSMCHARACTER_MAX_RECEIVED_SKILL_POINT; ++i)
	{
		if (g_alReceivedSkillPoint[lIndexTable][i] == 0)
		{
			i = AGSMCHARACTER_MAX_RECEIVED_SKILL_POINT;
			break;
		}

		if (lCurrentExpPercent < g_alReceivedSkillPoint[lIndexTable][i])
			break;
	}

	if (i == AGSMCHARACTER_MAX_RECEIVED_SKILL_POINT)
		return 0;

	AgsdCharacter	*pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter) return FALSE;

	INT32	lGivePoint	= i - pcsAgsdCharacter->m_lReceivedSkillPoint;
	if (lGivePoint < 1)
		return 0;
	
	return lGivePoint;
}

//		GiveSkillPoint
//	Functions
//		- pcsCharacter���� lPoint��ŭ�� skill point�� �ش�.
//	Arguments
//		- pcsCharacter : ĳ���� 
//	Return value
//		- BOOL : ���� ����
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharacter::GiveSkillPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	return GiveSkillPoint(pcsCharacter, GetGiveSkillPoint(pcsCharacter));
}

BOOL AgsmCharacter::GiveSkillPoint(AgpdCharacter *pcsCharacter, INT32 lSkillPoint)
{
	if (!pcsCharacter || lSkillPoint < 1 || lSkillPoint > AGPMCHAR_LEVELUP_SKILL_POINT)
		return FALSE;

	// lGivePoint ��ŭ pcsCharacter���� skill point�� �ش�.
	return m_pcsAgpmCharacter->UpdateSkillPoint(pcsCharacter, lSkillPoint);
}

BOOL AgsmCharacter::EncodingPosition(AuPOS *pstPos, CHAR *szPosition, INT32 lBufferSize)
{
	if (!pstPos || !szPosition || lBufferSize < 1)
		return FALSE;

	CHAR	szBuffer[32];
	ZeroMemory(szBuffer, sizeof(CHAR) * 32);

	sprintf(szBuffer, "%ld,%ld,%ld",
						(INT32) pstPos->x,
						(INT32) pstPos->y,
						(INT32) pstPos->z);

	if (strlen(szBuffer) > lBufferSize)
	{
		ASSERT(strlen(szBuffer) <= lBufferSize);
		return FALSE;
	}

	CopyMemory(szPosition, szBuffer, strlen(szBuffer));

	return TRUE;
}

BOOL AgsmCharacter::DecodingPosition(CHAR *szPosition, AuPOS *pstPos)
{
	if (!szPosition || !pstPos)
		return FALSE;

	INT32	lX	= 0;
	INT32	lY	= 0;
	INT32	lZ	= 0;

	sscanf(szPosition, "%ld,%ld,%ld", &lX, &lY, &lZ);

	pstPos->x	= (FLOAT) lX;
	pstPos->y	= (FLOAT) lY;
	pstPos->z	= (FLOAT) lZ;

	return TRUE;
}

BOOL AgsmCharacter::StartPlayTime(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	UINT32			ulClockCount			= GetClockCount();

	pcsAgsdCharacter->m_ulStartClockCount	= ulClockCount;

	pcsAgsdCharacter->m_bIsPartyPlay		= FALSE;

	return TRUE;
}

BOOL AgsmCharacter::StartPartyPlay(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	UINT32			ulClockCount			= GetClockCount();

	if (pcsAgsdCharacter->m_ulStartClockCount > 0 &&
		!pcsAgsdCharacter->m_bIsPartyPlay)
	{
		pcsAgsdCharacter->m_ulSoloPlayTime += ulClockCount - pcsAgsdCharacter->m_ulStartClockCount;
	}

	pcsAgsdCharacter->m_ulStartClockCount	= ulClockCount;

	pcsAgsdCharacter->m_bIsPartyPlay		= TRUE;

	return TRUE;
}

BOOL AgsmCharacter::StopPartyPlay(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	UINT32			ulClockCount			= GetClockCount();

	if (pcsAgsdCharacter->m_ulStartClockCount > 0 &&
		pcsAgsdCharacter->m_bIsPartyPlay)
	{
		pcsAgsdCharacter->m_ulPartyPlayTime += ulClockCount - pcsAgsdCharacter->m_ulStartClockCount;
	}

	pcsAgsdCharacter->m_ulStartClockCount	= ulClockCount;

	pcsAgsdCharacter->m_bIsPartyPlay		= FALSE;

	return TRUE;
}

BOOL AgsmCharacter::AddKillMonCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	++pcsAgsdCharacter->m_ulKillMonCount;

	return TRUE;
}

BOOL AgsmCharacter::AddKillPCCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	++pcsAgsdCharacter->m_ulKillPCCount;

	return TRUE;
}

BOOL AgsmCharacter::AddDeadByMonCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	++pcsAgsdCharacter->m_ulDeadByMonCount;

	return TRUE;
}

BOOL AgsmCharacter::AddDeadByPCCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	++pcsAgsdCharacter->m_ulDeadByPCCount;

	return TRUE;
}


UINT32 AgsmCharacter::GetPartyPlayTimeMSec(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	UINT32			ulPlayTimeBuffer		= 0;

	if (pcsAgsdCharacter->m_bIsPartyPlay)
		ulPlayTimeBuffer	= GetClockCount() - pcsAgsdCharacter->m_ulStartClockCount;

	return pcsAgsdCharacter->m_ulPartyPlayTime + ulPlayTimeBuffer;
}

UINT32 AgsmCharacter::GetSoloPlayTimeMSec(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	UINT32			ulPlayTimeBuffer		= 0;

	if (!pcsAgsdCharacter->m_bIsPartyPlay)
		ulPlayTimeBuffer	= GetClockCount() - pcsAgsdCharacter->m_ulStartClockCount;

	return pcsAgsdCharacter->m_ulSoloPlayTime + ulPlayTimeBuffer;
}

UINT32 AgsmCharacter::GetKillMonCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	return pcsAgsdCharacter->m_ulKillMonCount;
}

UINT32 AgsmCharacter::GetKillPCCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	return pcsAgsdCharacter->m_ulKillPCCount;
}

UINT32 AgsmCharacter::GetDeadByMonCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	return pcsAgsdCharacter->m_ulDeadByMonCount;
}

UINT32 AgsmCharacter::GetDeadByPCCount(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	return pcsAgsdCharacter->m_ulDeadByPCCount;
}

BOOL AgsmCharacter::ResetPlayLogData(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_ulStartClockCount	= GetClockCount();

	pcsAgsdCharacter->m_ulPartyPlayTime		= 0;
	pcsAgsdCharacter->m_ulSoloPlayTime		= 0;
	pcsAgsdCharacter->m_ulKillMonCount		= 0;
	pcsAgsdCharacter->m_ulKillPCCount		= 0;
	pcsAgsdCharacter->m_ulDeadByMonCount	= 0;
	pcsAgsdCharacter->m_ulDeadByPCCount		= 0;

	return TRUE;
}

// 2004.05.18. steeple
BOOL AgsmCharacter::WritePlayLog(AgpdCharacter* pcsCharacter, INT32 lLevelUp)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = GetADCharacter(pcsCharacter);

	INT32 lCharTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lCurrentLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	INT32 lPrevLevel = lCurrentLevel - lLevelUp;
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pcsCharacter);
	
	INT32 lPartyTime = (INT32) (GetPartyPlayTimeMSec(pcsCharacter) / 1000 / 60 + 0.5);		// minute, round
	INT32 lSoloTime = (INT32) (GetSoloPlayTimeMSec(pcsCharacter) / 1000 / 60 + 0.5);		// minute, round
	INT32 lKillPC = GetKillMonCount(pcsCharacter);
	INT32 lKillMon = GetKillPCCount(pcsCharacter);
	INT32 lDeadByPC = GetDeadByMonCount(pcsCharacter);
	INT32 lDeadByMon = GetDeadByPCCount(pcsCharacter);

	if (lLevelUp > 0)
	{
		m_pagpmLog->WriteLog_LevelUp(0, &pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									lCharTID,
									lCurrentLevel,
									llExp,
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									lPrevLevel,
									lPartyTime,
									lSoloTime,
									lKillPC,
									lKillMon,
									lDeadByPC,
									lDeadByMon
									);
	}
	else
	{
		m_pagpmLog->WriteLog_Tick(0, &pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									lCharTID,
									lCurrentLevel,
									llExp,
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									lPartyTime,
									lSoloTime,
									lKillPC,
									lKillMon,
									lDeadByPC,
									lDeadByMon
									);	
	}
	
	ResetPlayLogData(pcsCharacter);
	
	return TRUE;
}

BOOL AgsmCharacter::WriteGheldLog(eAGPDLOGTYPE_GHELD eType, AgpdCharacter *pcsCharacter, INT32 lGheld)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = GetADCharacter(pcsCharacter);

	INT32 lCharTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pcsCharacter);
	
	m_pagpmLog->WriteLog_Gheld(eType,
								0,
								&pAgsdCharacter->m_strIPAddress[0],
								pAgsdCharacter->m_szAccountID,
								pAgsdCharacter->m_szServerName,
								pcsCharacter->m_szID,
								lCharTID,
								lLevel,
								llExp,
								pcsCharacter->m_llMoney,
								pcsCharacter->m_llBankMoney,
								lGheld,
								NULL
								);

	return TRUE;
}

BOOL AgsmCharacter::WriteBankExpLog(AgpdCharacter *pcsCharacter)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = GetADCharacter(pcsCharacter);

	INT32 lCharTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pcsCharacter);
	INT32 lBankSize = pcsCharacter->m_cBankSize;
	INT64 llGheld = m_pagpmItem->GetBuyBankSlotCost(pcsCharacter);
	
	return m_pagpmLog->WriteLog_BankExpansion(0,
									&pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pcsCharacter->m_szID,
									lCharTID,
									lLevel,
									llExp,
									pcsCharacter->m_llMoney,
									pcsCharacter->m_llBankMoney,
									lBankSize,
									(INT32) llGheld
									);
}

BOOL AgsmCharacter::WriteExpLog(AgpdCharacter* pcsCharacter, INT32 lExp, BOOL bIsQeuest)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = GetADCharacter(pcsCharacter);

	INT32 lCharTID = ((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lCurrentLevel = m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	INT64 llExp = m_pcsAgpmCharacter->GetExp(pcsCharacter);

	return m_pagpmLog->WriteLog_Play(AGPDLOGTYPE_PLAY_GETEXP, 0, &pAgsdCharacter->m_strIPAddress[0],
		pAgsdCharacter->m_szAccountID,
		pAgsdCharacter->m_szServerName,
		pcsCharacter->m_szID,
		lCharTID,
		lCurrentLevel,
		llExp,
		pcsCharacter->m_llMoney,
		pcsCharacter->m_llBankMoney,
		lExp,
		bIsQeuest, 0, 0, 0, 0, 0
		);

	return TRUE;
}

BOOL AgsmCharacter::SaveAllCharacterData()
{
	INT32	lIndex	= 0;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	while (pcsCharacter)
	{
		if (GetCharDPNID(pcsCharacter) != 0 && !m_pcsAgpmCharacter->IsReturnToLoginServerStatus(pcsCharacter))
		{
			BackupCharacterData(pcsCharacter);

			EnumCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pcsCharacter, NULL);
		}

		pcsCharacter	= m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgsmCharacter::SetTransformTimeout(AgpdCharacter *pcsCharacter, UINT32 ulDurationTMsec)
{
	if (!pcsCharacter || ulDurationTMsec == 0)
		return FALSE;

	AgsdCharacter		*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_ulTimeOutTransform		= GetClockCount() + ulDurationTMsec;

	return TRUE;
}

BOOL AgsmCharacter::ResetTransformTimeout(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter		*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_ulTimeOutTransform		= 0;

	return TRUE;
}

BOOL AgsmCharacter::AdjustTransformFactor(AgpdCharacter *pcsCharacter, AgpdFactor *pcsUpdateFactor)
{
	if (!pcsCharacter || !pcsUpdateFactor)
		return FALSE;

	if (!pcsCharacter->m_bIsTrasform)
		return TRUE;

	// ���� ������ ��� �̵��ӵ�, ���ݰŸ�, ���ݼӵ��� ���Ͳ��� �Ѵ�.

    /*
	FLOAT	fMoveSpeed		= 0.0f;
	FLOAT	fAttackRange	= 0.0f;
	FLOAT	fAttackSpeed	= 0.0f;

	m_pagpmFactors->GetValue(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor, &fMoveSpeed, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
	m_pagpmFactors->GetValue(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor, &fAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
	m_pagpmFactors->GetValue(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor, &fAttackSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);

	m_pagpmFactors->SetValue(pcsUpdateFactor, fMoveSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
	m_pagpmFactors->SetValue(pcsUpdateFactor, fAttackRange, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
	m_pagpmFactors->SetValue(pcsUpdateFactor, fAttackSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
	*/

	FLOAT	fAttackRange	= 0.0f;

	m_pagpmFactors->GetValue(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor, &fAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);

	m_pagpmFactors->SetValue(pcsUpdateFactor, fAttackRange, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);

	EnumCallback(AGSMCHARACTER_CB_ADJUST_TRANSFORM_FACTOR, pcsCharacter, pcsUpdateFactor);

	return TRUE;
}

BOOL AgsmCharacter::ApplyBonusFactor(AgpdCharacter *pcsCharacter, AgpdFactor *pcsUpdateFactor)
{
	if (!pcsCharacter || !pcsUpdateFactor)
		return FALSE;

	AdjustTransformFactor(pcsCharacter, pcsUpdateFactor);

	EnumCallback(AGSMCHARACTER_CB_APPLY_BONUS_FACTOR, pcsCharacter, pcsUpdateFactor);

	// char point max ġ�� ����Ȱ� ������ ó�����ش�.

	INT32	lMaxHPPoint		= 0;
	INT32	lMaxMPPoint		= 0;

	INT32	lMaxHPPercent	= 0;
	INT32	lMaxMPPercent	= 0;

	INT32	lMaxHP			= 0;
	INT32	lMaxMP			= 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactorPoint, &lMaxHPPoint, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactorPoint, &lMaxMPPoint, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactorPercent, &lMaxHPPercent, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactorPercent, &lMaxMPPercent, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

	m_pagpmFactors->GetValue(pcsUpdateFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsUpdateFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

	INT32	lNewMaxHP		= (INT32)((FLOAT)lMaxHP * (1.0f + (FLOAT)lMaxHPPercent / 100.0f) + (FLOAT)lMaxHPPoint);
	INT32	lNewMaxMP		= (INT32)((FLOAT)lMaxMP * (1.0f + (FLOAT)lMaxMPPercent / 100.0f) + (FLOAT)lMaxMPPoint);

	m_pagpmFactors->SetValue(pcsUpdateFactor, lNewMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->SetValue(pcsUpdateFactor, lNewMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

	return TRUE;
}

BOOL AgsmCharacter::SetIdleInterval(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, AgsdCharacterIdleInterval eInterval)
{
	if (!pcsCharacter)
		return FALSE;

	if (eIdleType < 0 || eIdleType >= AGSDCHAR_IDLE_TYPE_MAX ||
		eInterval < AGSDCHAR_IDLE_INTERVAL_NEXT_TIME || eInterval > AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_eIdleProcessInterval[eIdleType] > 0 &&
		pcsAgsdCharacter->m_eIdleProcessInterval[eIdleType] <= eInterval)
		return TRUE;

	pcsAgsdCharacter->m_eIdleProcessInterval[eIdleType] = eInterval;

	if (pcsAgsdCharacter->m_eIdleProcessInterval[AGSDCHAR_IDLE_TYPE_TOTAL] == 0 ||
		pcsAgsdCharacter->m_eIdleProcessInterval[AGSDCHAR_IDLE_TYPE_TOTAL] > eInterval)
		pcsAgsdCharacter->m_eIdleProcessInterval[AGSDCHAR_IDLE_TYPE_TOTAL]	= eInterval;

	return TRUE;
}

BOOL AgsmCharacter::IsIdleProcessTime(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	if (eIdleType < 0 || eIdleType >= AGSDCHAR_IDLE_TYPE_MAX)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_eIdleProcessInterval[eIdleType] == AGSDCHAR_IDLE_INTERVAL_MAX)
		return FALSE;

	if (pcsAgsdCharacter->m_ulLastIdleProcessTime[eIdleType] > 0 &&
		pcsAgsdCharacter->m_ulLastIdleProcessTime[eIdleType]
			+ pcsAgsdCharacter->m_eIdleProcessInterval[eIdleType] > ulClockCount)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmCharacter::ResetIdleInterval(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType)
{
	if (!pcsCharacter)
		return FALSE;

	if (eIdleType < 0 || eIdleType >= AGSDCHAR_IDLE_TYPE_MAX)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_eIdleProcessInterval[eIdleType]	= AGSDCHAR_IDLE_INTERVAL_MAX;

	return TRUE;
}

BOOL AgsmCharacter::SetProcessTime(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	if (eIdleType < 0 || eIdleType >= AGSDCHAR_IDLE_TYPE_MAX)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_ulLastIdleProcessTime[eIdleType]	= ulClockCount;

	return TRUE;
}

UINT32 AgsmCharacter::GetLastIdleProcessTime(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType)
{
	if (!pcsCharacter)
		return 0;

	if (eIdleType < 0 || eIdleType >= AGSDCHAR_IDLE_TYPE_MAX)
		return 0;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	return pcsAgsdCharacter->m_ulLastIdleProcessTime[eIdleType];
}

UINT32 AgsmCharacter::GetElapsedTimeFromLastIdleProcess(AgpdCharacter *pcsCharacter, AgsdCharacterIdleType eIdleType, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return 0;

	if (eIdleType < 0 || eIdleType >= AGSDCHAR_IDLE_TYPE_MAX)
		return 0;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_ulLastIdleProcessTime[eIdleType] == 0)
	{
		return 0;
	}

	return ulClockCount - pcsAgsdCharacter->m_ulLastIdleProcessTime[eIdleType];
}

BOOL AgsmCharacter::IsProcessIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_bMove)
		return TRUE;

	if (m_pcsAgpmCharacter->IsStatusSummoner(pcsCharacter))
		return TRUE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_ulLastIdleProcessTime[AGSDCHAR_IDLE_TYPE_TOTAL]
		+ pcsAgsdCharacter->m_eIdleProcessInterval[AGSDCHAR_IDLE_TYPE_TOTAL]
		<= ulClockCount)
		return TRUE;

	return FALSE;
}

BOOL AgsmCharacter::ProcessIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	if (!IsProcessIdle(pcsCharacter, ulClockCount))
		return TRUE;

	ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_TOTAL);
	SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_TOTAL, ulClockCount);

	m_AuIOCPDispatcher.Dispatch(pcsCharacter);

	return TRUE;
}

// 2005.07.07. steeple
BOOL AgsmCharacter::SetSpecialStatusTime(AgpdCharacter* pcsCharacter, UINT64 ulSpecialStatus, UINT32 ulDuration, INT32 lValue)
{
	if(!pcsCharacter || ulSpecialStatus < 1 || ulDuration == 0)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	UINT32 ulReleaseTime = GetClockCount() + ulDuration;

	switch(ulSpecialStatus)
	{
		case AGPDCHAR_SPECIAL_STATUS_STUN:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_STUN);
			if(pcsAgsdCharacter->m_ulReleaseStunStatusTime < ulReleaseTime)
			{
				pcsAgsdCharacter->m_ulReleaseStunStatusTime	= ulReleaseTime;

				// 2005.10.14. steeple
				// ���߰� ���ش�.
				m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
			}

			break;

		case AGPDCHAR_SPECIAL_STATUS_FREEZE:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_FREEZE);
			if(pcsAgsdCharacter->m_ulReleaseFreezeStatusTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseFreezeStatusTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_SLOW:
			// ���� Slow ������ lValue �� Ŭ ���� ��������.
			//if(pcsAgsdCharacter->m_lSlowStatusValue <= lValue)
			//{
			//	pcsAgsdCharacter->m_lSlowStatusValue = lValue;

				m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SLOW);
				if(pcsAgsdCharacter->m_ulReleaseSlowStatusTime < ulReleaseTime)
					pcsAgsdCharacter->m_ulReleaseSlowStatusTime	= ulReleaseTime;

			//	// Factor �� �պ���� �Ѵ�.
			//	ReCalcCharacterFactors(pcsCharacter);
			//}
			break;

		case AGPDCHAR_SPECIAL_STATUS_INVINCIBLE:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE);
			if(pcsAgsdCharacter->m_ulReleaseInvincibleStatusTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseInvincibleStatusTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE);
			if(pcsAgsdCharacter->m_ulReleaseAttributeInvincibleStatusTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseAttributeInvincibleStatusTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO);
			if(pcsAgsdCharacter->m_ulReleaseNotAddAgroStatusTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseNotAddAgroStatusTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO);
			if(pcsAgsdCharacter->m_ulReleaseHideAgroStatusTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseHideAgroStatusTime	= ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT);
			if(pcsAgsdCharacter->m_ulReleaseStunProtectStatusTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseStunProtectStatusTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_TRANSPARENT:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);
			if(pcsAgsdCharacter->m_ulReleaseTransparentTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseTransparentTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_HALT:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HALT);
			if(pcsAgsdCharacter->m_ulReleaseHaltStatusTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseHaltStatusTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT);
			if(pcsAgsdCharacter->m_ulReleaseHalfTransparentTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseHalfTransparentTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE);
			if(pcsAgsdCharacter->m_ulReleaseNormalATKInvincibleTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseNormalATKInvincibleTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE);
			if(pcsAgsdCharacter->m_ulReleaseSkillATKInvincibleTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseSkillATKInvincibleTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL);
			if(pcsAgsdCharacter->m_ulReleaseDisableSkillTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseDisableSkillTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK);
			if(pcsAgsdCharacter->m_ulReleaseDisableNormalATKTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseDisableNormalATKTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_SLEEP:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SLEEP);
			if(pcsAgsdCharacter->m_ulReleaseSleepTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseSleepTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_DISARMAMENT:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISARMAMENT, lValue);
			if(pcsAgsdCharacter->m_ulReleaseDisArmamentTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseDisArmamentTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_HOLD:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HOLD);
			if(pcsAgsdCharacter->m_ulReleaseHoldTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseHoldTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_CONFUSION:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_CONFUSION);
			if(pcsAgsdCharacter->m_ulReleaseConfusionTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseConfusionTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_FEAR:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_FEAR);
			if(pcsAgsdCharacter->m_ulReleaseFearTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseFearTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_DISEASE:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISEASE);
			if(pcsAgsdCharacter->m_ulReleaseDiseaseTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseDiseaseTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_BERSERK:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_BERSERK);
			if(pcsAgsdCharacter->m_ulReleaseBerserkTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseBerserkTime = ulReleaseTime;
			break;

		case AGPDCHAR_SPECIAL_STATUS_SHRINK:
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SHRINK);
			if(pcsAgsdCharacter->m_ulReleaseShrinkTime < ulReleaseTime)
				pcsAgsdCharacter->m_ulReleaseShrinkTime = ulReleaseTime;
			break;
	}

	return TRUE;
}

// 2004.08.26. steeple
BOOL AgsmCharacter::CheckSpecialStatusIdleTime(AgpdCharacter* pcsCharacter, UINT32 ulClockCount)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// stun
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
	{
		if(pcsAgsdCharacter->m_ulReleaseStunStatusTime <= ulClockCount)
		{
			//pThis->m_pcsAgpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NORMAL);
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_STUN);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}
	
	// Freeze
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_FREEZE)
	{
		if(pcsAgsdCharacter->m_ulReleaseFreezeStatusTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_FREEZE);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Slow
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SLOW)
	{
		if(pcsAgsdCharacter->m_ulReleaseSlowStatusTime <= ulClockCount)
		{
			// 2005.07.07. steeple
			// �̳��� Ǯ�� �� Slow ����� Factor �� �����·� ��������� �Ѵ�.
			if(pcsAgsdCharacter->m_lSlowStatusValue > 0)
			{
			}

			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SLOW);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Invincible
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_INVINCIBLE)
	{
		if(pcsAgsdCharacter->m_ulReleaseInvincibleStatusTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Attribute Invincible
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE)
	{
		if(pcsAgsdCharacter->m_ulReleaseAttributeInvincibleStatusTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Not Add Agro
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO)
	{
		if(pcsAgsdCharacter->m_ulReleaseNotAddAgroStatusTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Hide Agro
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO)
	{
		if(pcsAgsdCharacter->m_ulReleaseHideAgroStatusTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Stun Protect
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT)
	{
		if(pcsAgsdCharacter->m_ulReleaseStunProtectStatusTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Transparent
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_TRANSPARENT)
	{
		if(pcsAgsdCharacter->m_ulReleaseTransparentTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Halt - 2005.10.17. steeple
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HALT)
	{
		if(pcsAgsdCharacter->m_ulReleaseHaltStatusTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HALT);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Half Transparent
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT)
	{
		if(pcsAgsdCharacter->m_ulReleaseHalfTransparentTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Normal Attack Invincible
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE)
	{
		if(pcsAgsdCharacter->m_ulReleaseNormalATKInvincibleTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Skill Attack Invincible
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE)
	{
		if(pcsAgsdCharacter->m_ulReleaseSkillATKInvincibleTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Disable Skill
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL)
	{
		if(pcsAgsdCharacter->m_ulReleaseDisableSkillTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Disable Normal Attack
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK)
	{
		if(pcsAgsdCharacter->m_ulReleaseDisableNormalATKTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Sleep
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SLEEP)
	{
		if(pcsAgsdCharacter->m_ulReleaseSleepTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SLEEP);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// DisArmament
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISARMAMENT)
	{
		if(pcsAgsdCharacter->m_ulReleaseDisArmamentTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISARMAMENT);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Hold
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
	{
		if(pcsAgsdCharacter->m_ulReleaseHoldTime <= ulClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_HOLD);
			
			ProcessReleaseSpecialStatus(pcsCharacter);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Confusion
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_CONFUSION)
	{
		if(pcsAgsdCharacter->m_ulReleaseConfusionTime <= ulClockCount)
		{
			ProcessReleaseSpecialStatus(pcsCharacter);

			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_CONFUSION);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Fear
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_FEAR)
	{
		if(pcsAgsdCharacter->m_ulReleaseFearTime <= ulClockCount)
		{
			ProcessReleaseSpecialStatus(pcsCharacter);

			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_FEAR);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Disease
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISEASE)
	{
		if(pcsAgsdCharacter->m_ulReleaseDiseaseTime <= ulClockCount)
		{
			ProcessReleaseSpecialStatus(pcsCharacter);

			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_DISEASE);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Berserk
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_BERSERK)
	{
		if(pcsAgsdCharacter->m_ulReleaseBerserkTime <= ulClockCount)
		{
			ProcessReleaseSpecialStatus(pcsCharacter);

			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_BERSERK);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	// Shrink
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SHRINK)
	{
		if(pcsAgsdCharacter->m_ulReleaseShrinkTime <= ulClockCount)
		{
			ProcessReleaseSpecialStatus(pcsCharacter);

			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_SHRINK);
		}
		else
		{
			SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_CHARACTER, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
		}
	}

	return TRUE;
}

BOOL AgsmCharacter::ProcessReleaseSpecialStatus(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	//Init PC AI Info
	if(m_pagpmAI2)
		m_pagpmAI2->InitPCAIInfo(pcsCharacter);

	if((pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_CONFUSION) &&
	   (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_FEAR) &&
	   (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISEASE))
	{
		
	}

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_BERSERK)
	{
		// Restore changed Factor (Attack Up -> Down / Defense Down -> Up)
	}

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SHRINK)
	{
		// Restore changed Factor (Attack Down -> Up / Defense Up -> Down)
	}

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
	{
		// Restore changed Factor (Defense Down -> Up)
	}

	return TRUE;
}

// 2008.02.27. steeple
BOOL AgsmCharacter::RestoreSpecialStatusTime(AgpdCharacter* pcsCharacter, UINT64 ulOldSpecialStatus)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// �ʿ��� �༮�鸸 ó���Ѵ�.

	// Transparent
	if((ulOldSpecialStatus & AGPDCHAR_SPECIAL_STATUS_TRANSPARENT) && !(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_TRANSPARENT))
		pcsAgsdCharacter->m_ulReleaseTransparentTime = 0;

	// Half Transparent
	if((ulOldSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT) && !(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT))
		pcsAgsdCharacter->m_ulReleaseHaltStatusTime = 0;

	return TRUE;
}

BOOL AgsmCharacter::AddReservedTimeForDestroy(AgpdCharacter *pcsCharacter, UINT32 ulAddReservedTimeMSec)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_ulReserveTimeForDestroy	+= ulAddReservedTimeMSec;

	return TRUE;
}

UINT32 AgsmCharacter::GetReservedTimeForDestory(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	return pcsAgsdCharacter->m_ulReserveTimeForDestroy;
}

BOOL AgsmCharacter::GenerateAuthKey(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_lAuthKey		= m_csRandom.randInt(2147483640);
	pcsAgsdCharacter->m_bIsValidAuthKey	= TRUE;

	return TRUE;
}

BOOL AgsmCharacter::SetCallbackSendCharView(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARACTER_CB_SEND_CHAR_VIEW, pfCallback, pClass);
}

BOOL AgsmCharacter::CheckProcessIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	if (!IsProcessIdle(pcsCharacter, ulClockCount))
		return FALSE;

	ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_TOTAL);
	SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_TOTAL, ulClockCount);

	return TRUE;
}

// 2005.07.08. steeple
// RecalcFactor �ÿ� SpecialStatus �� ���õ� ���͵��� ������ش�.
BOOL AgsmCharacter::ApplySpecialStatusFactor(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgpdFactor csFactorPoint, csFactorPercent;
	m_pagpmFactors->InitFactor(&csFactorPoint);
	m_pagpmFactors->InitFactor(&csFactorPercent);
	
	// Slow
	if(pcsAgsdCharacter->m_lSlowStatusValue != 0)
	{
		// Slow �� Percent Factor �� ����
		AgpdFactorCharStatus* pcsCharStatusFactor = (AgpdFactorCharStatus*)m_pagpmFactors->SetFactor(&csFactorPercent, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if(!pcsCharStatusFactor)
			return FALSE;

		m_pagpmFactors->SetValue(&csFactorPercent, pcsAgsdCharacter->m_lSlowStatusValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		m_pagpmFactors->SetValue(&csFactorPercent, pcsAgsdCharacter->m_lSlowStatusValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
	}

	m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, &csFactorPoint, FALSE, FALSE, TRUE, FALSE);
	m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, &csFactorPercent, FALSE, FALSE, TRUE, FALSE);

	m_pagpmFactors->DestroyFactor(&csFactorPoint);
	m_pagpmFactors->DestroyFactor(&csFactorPercent);

	return TRUE;
}

// 2005.11.29. steeple
// �� ���� �� �� �������� �ֱ� ���ؼ� ���� �ڱⰡ �����ϰ� �ִ� Ÿ�� ����Ʈ�� �����Ѵ�.
BOOL AgsmCharacter::AddTargetInfo(AgpdCharacter* pcsAttacker, INT32 lTargetCID, UINT32 ulClockCount)
{
	// 2005.11.30. steeple
	// �ϴ� �ּ��ؼ� Ŀ��
	return TRUE;

	//if(!pcsAttacker || lTargetCID == AP_INVALID_CID)
	//	return FALSE;

	//if(ulClockCount == 0)
	//	ulClockCount = GetClockCount();

	//AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsAttacker);
	//if(!pcsAgsdCharacter)
	//	return FALSE;

	//AgsdCharacterTargetInfoArray::iterator iter = std::find(pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.begin(),
	//														pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.end(),
	//														lTargetCID);

	//if(pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.IsValidIterator(iter))
	//{
	//	// �̹� �迭�� �ִ�. �ð��� ������Ʈ
	//	(*iter).m_ulLastAttackTime = ulClockCount;
	//	return TRUE;
	//}

	//// �̹� �� á3
	//if(pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.size() >= pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.GetLength() - 1)
	//	return FALSE;

	//// �� �ڿ� �־��ش�.
	//AgsdCharacterTargetInfo stTargetInfo;
	//stTargetInfo.m_lTargetCID = lTargetCID;
	//stTargetInfo.m_ulLastAttackTime = ulClockCount;

	//pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.push_back(stTargetInfo);
	//pcsAttacker->m_lCountMobMoli = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.size();

	//return TRUE;
}

// 2005.11.29. steeple
BOOL AgsmCharacter::RemoveTargetInfo(AgpdCharacter* pcsAttacker, INT32 lTargetCID)
{
	if(!pcsAttacker || lTargetCID == AP_INVALID_CID)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsAttacker);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgsdCharacterTargetInfoArray::iterator iter = std::find(pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.begin(),
															pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.end(),
															lTargetCID);

	if(pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.IsValidIterator(iter) == false)
		return FALSE;

	pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.erase(iter);
	pcsAttacker->m_lCountMobMoli = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.size();

	return TRUE;
}

// 2005.11.29. steeple
BOOL AgsmCharacter::InitTargetInfoArray(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.clear();
	pcsCharacter->m_lCountMobMoli = 0;

	return TRUE;
}

// 2005.11.30. steeple
INT32 AgsmCharacter::CheckTargetInfoIdle(AgpdCharacter* pcsCharacter, UINT32 ulClockCount)
{
	if(!pcsCharacter)
		return 0;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgsdCharacterTargetInfoArray::iterator iter = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.begin();
	while(iter != pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.end())
	{
		if((*iter).m_lTargetCID == AP_INVALID_CID)
		{
			break;
		}

		// �ð��� �������� ����
		//if((*iter).m_ulLastAttackTime + AGSDCHARACTER_TARGET_INFO_PERIOD < ulClockCount)
		//{
		//	iter = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.erase(iter);
		//	continue;
		//}

		// Lock �� ���� �ʴ´�.
		AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter((*iter).m_lTargetCID);
		if(!pcsTarget)
		{
			iter = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.erase(iter);
			continue;
		}

		if(pcsTarget->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD ||
			pcsTarget->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		{
			iter = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.erase(iter);
			continue;
		}

		// �Ÿ�üũ�� �߰�
		if(AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pcsTarget->m_stPos) > pcsAgsdCharacter->m_fHistoryRange)
		{
			iter = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.erase(iter);
			continue;
		}

		iter++;
	}

	pcsCharacter->m_lCountMobMoli = pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.size();
	return pcsAgsdCharacter->m_TargetInfoArray.m_arrTargetInfo.size();
}

// �⺻ ���� �ɾ��ֱ�
BOOL AgsmCharacter::SetDefaultInvincible(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	SetSpecialStatusTime(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE, AGSMCHAR_DEFAULT_INVINCIBLE_TIME);
	return TRUE;
}

// �������� ���긦 ����� ���ڸ� ��Ȱ ���� �ɾ��ֱ�
BOOL AgsmCharacter::SetResurrectionOrbInvincible(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	SetSpecialStatusTime(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE, AGSMCHAR_RESSURECTION_ORB_INVINCIBLE_TIME);
	return TRUE;
}

BOOL AgsmCharacter::IsAuctionBlocked(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pAgsdCharacter)
		return FALSE;
	
	return pAgsdCharacter->m_bIsAuctionBlock;
}

BOOL AgsmCharacter::AddExp(AgpdCharacter *pcsCharacter, INT64 llAddExp)
{
	AgpdFactor		csUpdateFactor;
	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

	AgpdFactor	*pcsUpdateResultFactor = (AgpdFactor *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsUpdateResultFactor)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	AgpdFactorCharPoint	*pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	INT64	llOriginalExp	= m_pcsAgpmCharacter->GetExp(pcsCharacter);

	m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT), llOriginalExp + llAddExp);

	PVOID	pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, &csUpdateFactor, TRUE, TRUE, TRUE, FALSE);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	if (!pvPacketFactor)
		return FALSE;

	BOOL bSendResult = SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_4);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	return TRUE;
}

BOOL AgsmCharacter::SubExp(AgpdCharacter *pcsCharacter, INT64 llSubExp)
{
	AgpdFactor		csUpdateFactor;
	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

	AgpdFactor	*pcsUpdateResultFactor = (AgpdFactor *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsUpdateResultFactor)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	AgpdFactorCharPoint	*pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	INT64	llOriginalExp	= m_pcsAgpmCharacter->GetExp(pcsCharacter);

	m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT), (-llSubExp));

	PVOID	pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, &csUpdateFactor, FALSE, TRUE, TRUE, FALSE);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	if (!pvPacketFactor)
		return FALSE;

	BOOL bSendResult = SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_4);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	return TRUE;
}

BOOL AgsmCharacter::SetHPFull(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// HP�� Full�� ä���.
	PVOID pvPacketFactor = NULL;

	if(!m_pagsmFactors->Resurrection(&pcsCharacter->m_csFactor, &pvPacketFactor))
		return FALSE;

	// factor ��Ŷ�� ������.
	BOOL bResult = SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_2);

	if(pvPacketFactor)
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	return bResult;
}

// 2007.07.08. steeple
// �ش� ����ŭ HP �� ���ҽ�Ų��.
BOOL AgsmCharacter::DecreaseHP(AgpdCharacter* pcsCharacter, INT32 lDecreaseHP)
{
	if(!pcsCharacter || lDecreaseHP < 1)
		return FALSE;

	AgpdFactor csUpdateFactor;
	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

	AgpdFactor* pcsUpdateResultFactor = (AgpdFactor*)m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsUpdateResultFactor)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	INT32 lCurrentHP = 0;
	INT32 lCurrentMP = 0;
	INT32 lCurrentSP = 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	if(lCurrentHP - lDecreaseHP > 0)
	{
		m_pagpmFactors->SetValue(pcsUpdateResultFactor, -(lDecreaseHP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	}
	else
	{
		// ������ �˴� 0 ����
		m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

		m_pagpmFactors->SetValue(pcsUpdateResultFactor, (-lCurrentHP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->SetValue(pcsUpdateResultFactor, (-lCurrentMP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->SetValue(pcsUpdateResultFactor, (-lCurrentSP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}

	PVOID pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, &csUpdateFactor, FALSE, TRUE, TRUE, FALSE);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	if(!pvPacketFactor)
		return FALSE;

	BOOL bSendResult = SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_4);

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	return bSendResult;
}

// 2007.07.10. steeple
// Max ġ �ʰ��� ������ Max �� �������ش�.
BOOL AgsmCharacter::AdjustExceedPoint(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	INT32 lCurrentHP, lCurrentMP, lCurrentSP;
	INT32 lMaxHP, lMaxMP, lMaxSP;
	lCurrentHP = lCurrentMP = lCurrentSP = lMaxHP = lMaxMP = lMaxSP = 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	// �ٲ� �� ����.
	if(lCurrentHP <= lMaxHP && lCurrentMP <= lMaxMP && lCurrentSP <= lMaxSP)
		return TRUE;

	if(lCurrentHP > lMaxHP)
		lCurrentHP = lMaxHP;
	if(lCurrentMP > lMaxMP)
		lCurrentMP = lMaxMP;
	if(lCurrentSP > lMaxSP)
		lCurrentSP = lMaxSP;

	PVOID pvPacket = NULL;
	m_pagsmFactors->UpdateCharPoint(&pcsCharacter->m_csFactor, &pvPacket, lCurrentHP, lCurrentMP, lCurrentSP);

	if(pvPacket)
	{
		SendPacketFactor(pvPacket, pcsCharacter, PACKET_PRIORITY_4);
		m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

// 2008.03.10. steeple
// ĳ������ ���� HP �� ĳ���� �� ������ ��ũ�Ѵ�.
BOOL AgsmCharacter::SyncHP(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	PVOID pvPacketHP = m_pagpmFactors->MakePacketFactorsCharHP(&pcsCharacter->m_csFactor);
	if(!pvPacketHP)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = MakePacketFactor(pcsCharacter, pvPacketHP, &nPacketLength);
	
	m_pagpmFactors->m_csPacket.FreePacket(pvPacketHP);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_3);
	m_pagpmFactors->m_csPacket.FreePacket(pvPacket);
	return bResult;
}

INT32 AgsmCharacter::GetNumOfPlayers()
{
	if (m_csMutexNumOfPlayers.WLock())
	{
		INT32	lNumOfPlayers	= m_lNumOfPlayers;
		m_csMutexNumOfPlayers.Release();
		return lNumOfPlayers;
	}

	return 0;
}

void AgsmCharacter::AddServerNameToCharName(CHAR *pszOriginalName, CHAR *pszServerName, CHAR *pszNewNameBuffer)
{
	if (!pszOriginalName || !pszOriginalName[0] || !pszServerName || !pszServerName[0] || !pszNewNameBuffer)
		return;

	sprintf(pszNewNameBuffer, "%s#%s", pszServerName, pszOriginalName);
}

CHAR* AgsmCharacter::GetRealCharName(CHAR *pszCharName)
{
	if (!pszCharName || !pszCharName[0])
		return NULL;

	for (int i = 0; i < AGPDCHARACTER_MAX_ID_LENGTH; ++i)
		if (pszCharName[i] == '#')
			return (pszCharName + i + 1);

	return pszCharName;
}

// �Ϲ̵� ������ ����ȭ�� �����ϸ鼭 �ּ� ó�� by ����
//BOOL AgsmCharacter::SetPaidTimeout(AgpdCharacter *pcsCharacter, UINT32 ulDurationTMsec)
//{
//	if (!pcsCharacter || ulDurationTMsec == 0)
//		return FALSE;
//
//	AgsdCharacter		*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);
//
//	pcsAgsdCharacter->m_ulTimeOutPaid		= GetClockCount() + ulDurationTMsec;
//
//	return TRUE;
//}
//
//BOOL AgsmCharacter::ResetPaidTimeout(AgpdCharacter *pcsCharacter)
//{
//	if (!pcsCharacter)
//		return FALSE;
//
//	AgsdCharacter		*pcsAgsdCharacter		= GetADCharacter(pcsCharacter);
//
//	pcsAgsdCharacter->m_ulTimeOutPaid		= 0;
//
//	return TRUE;
//}

BOOL AgsmCharacter::IsAutoPickup(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);

	if(pcsAgsdCharacter->m_bIsAutoPickupItem)
	{
		if(m_pagpmItem->IsEnableSubInventory(pcsCharacter))
			return TRUE;
	}	

	return FALSE;
}

VOID AgsmCharacter::SetAutoPickup(AgpdCharacter *pcsCharacter, BOOL bSetValue)
{
	if (!pcsCharacter) return;

	AgsdCharacter	*pcsAgsdCharacter	= GetADCharacter(pcsCharacter);
	pcsAgsdCharacter->m_bIsAutoPickupItem	= bSetValue;
}

//BOOL AgsmCharacter::GGSendInitialPacket(AgpdCharacter* pcsCharacter)
//{
//	g_nProtect.CreateAuthObject( &pcsCharacter->m_pGameguard );
//	pcsCharacter->m_ggLastSyncTime = GetClockCount();
//	
//	return GGSendServerAuth( pcsCharacter );
//}
//
//BOOL AgsmCharacter::GGSendServerAuth(AgpdCharacter* pcsCharacter)
//{
//	GG_AUTH_DATA ggData;
//	g_nProtect.ServerToClient( pcsCharacter->m_pGameguard, &ggData );
//	
//	INT16 packetLength;
//	PVOID pvPacket = m_pcsAgpmCharacter->MakePacketGameguardAuth( pcsCharacter, &ggData, &packetLength );
//	if( pvPacket && packetLength >= 1)
//	{
//		SendPacket(pvPacket, packetLength, GetCharDPNID(pcsCharacter));
//		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
//
//		return TRUE;
//	}
//
//	return FALSE;
//}

BOOL AgsmCharacter::SetCash(AgpdCharacter *pcsCharacter, INT64 llCash)
{
	return m_pcsAgpmCharacter->SetCash( pcsCharacter, llCash );
}

INT64 AgsmCharacter::SubCash(AgpdCharacter *pcsCharacter, INT64 llCash)
{
	return m_pcsAgpmCharacter->SubCash( pcsCharacter, llCash );
}

BOOL	AgsmCharacter::SetCharacterLevelLimit		( AgpdCharacter * pcsCharacter , INT32 nLevel )
{
	if( NULL == pcsCharacter ) return FALSE;

	if (nLevel < 1)
		nLevel	= 1;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	INT32	nLevelOriginal	= m_pagpmFactors->GetLevel( &pcsCharacter->m_csFactor );
	if( m_pcsAgpmCharacter->SetCharacterLevelLimit( pcsCharacter , nLevel ) )
	{
		if( nLevel < nLevelOriginal )
		{
			pcsAgsdCharacter->m_bLevelUpForced = TRUE;
			pcsAgsdCharacter->m_lLevelBeforeForced = m_pcsAgpmCharacter->GetLevel(pcsCharacter);

			INT32 lCharismaPoint	= m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter);
			INT64 lExp				= m_pcsAgpmCharacter->GetExp(pcsCharacter);
			INT32 lMurdererPoint	= m_pcsAgpmCharacter->GetMurdererPoint(pcsCharacter);
			
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, nLevelOriginal, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_BEFORELEVEL);
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, nLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LIMITEDLEVEL);

			BOOL bResult = m_pcsAgpmCharacter->UpdateLevel(pcsCharacter, nLevel - nLevelOriginal);

			AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
			m_pagpmFactors->SetExp( pcsResultFactor , lExp );
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);

			pcsAgsdCharacter->m_bLevelUpForced = FALSE;
			pcsAgsdCharacter->m_lLevelBeforeForced = 0;

			//m_pcsAgpmCharacter->ChangedLevel( pcsCharacter );
			//ReCalcCharacterFactors( pcsCharacter );
		}
		else
		{
			// Before Level ����.
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, nLevelOriginal, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_BEFORELEVEL);
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, nLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LIMITEDLEVEL);
		}
	}
	return TRUE;
}

BOOL	AgsmCharacter::ReleaseCharacterLevelLimit	( AgpdCharacter * pcsCharacter )
{
	if( NULL == pcsCharacter ) return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	INT32	nLevelPast	= m_pcsAgpmCharacter->GetLevel		( pcsCharacter );
	INT32	nLevel		= m_pcsAgpmCharacter->GetLevelBefore( pcsCharacter );
	if( m_pcsAgpmCharacter->ReleaseCharacterLevelLimit( pcsCharacter ) )
	{
		if( nLevelPast < nLevel )
		{
			pcsAgsdCharacter->m_bLevelUpForced = TRUE;
			pcsAgsdCharacter->m_lLevelBeforeForced = m_pcsAgpmCharacter->GetLevel(pcsCharacter);

			INT32 lCharismaPoint = m_pcsAgpmCharacter->GetCharismaPoint(pcsCharacter);
			INT64 lExp				= m_pcsAgpmCharacter->GetExp(pcsCharacter);
			INT32 lMurdererPoint	= m_pcsAgpmCharacter->GetMurdererPoint(pcsCharacter);

			BOOL bResult = m_pcsAgpmCharacter->UpdateLevel(pcsCharacter, nLevel - nLevelPast);

			AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
			m_pagpmFactors->SetExp( pcsResultFactor , lExp );
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurdererPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);

			pcsAgsdCharacter->m_bLevelUpForced = FALSE;
			pcsAgsdCharacter->m_lLevelBeforeForced = 0;

			//m_pcsAgpmCharacter->ChangedLevel( pcsCharacter );
			//ReCalcCharacterFactors( pcsCharacter );
		}
	}

	return TRUE;
}

// 2008.03.25. steeple
// EnterGameWorld �� �� Position Check �ؼ� �� �� ���� ���̶�� ���� ������ ��ǥ�� �������ش�.
BOOL AgsmCharacter::CheckLoginPosition(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// Resurrection Region�� �����Ǿ��ִٸ� �װ����� ������.
	ApmMap::RegionTemplate* pcsRegionTemplate = m_papmMap->GetTemplate(m_papmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z));
	if(pcsRegionTemplate)
	{
		if(pcsRegionTemplate->stResurrectionPos.x != 0 || pcsRegionTemplate->stResurrectionPos.y != 0)
		{
			pcsCharacter->m_stPos = pcsRegionTemplate->stResurrectionPos;
		}
	}

	if(pcsRegionTemplate && pcsRegionTemplate->nLevelLimit)
	{
		if(!m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &pcsCharacter->m_stPos))
		{
			// ���� ���� ��ǥ�� ���ϴ� �� �����ϸ�, ĳ�� ���� ������ ����������.
			m_pcsAgpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);
		}
	}

	// �����Ҷ� ������ �̺�Ʈ ��Ʋ�׶��� ���̸� ������ ����
	if(pcsRegionTemplate && (pcsRegionTemplate->nIndex == AGPMMAP_RESTRICT_EVENT_BATTLE_GROUND || pcsRegionTemplate->nIndex == AGPMMAP_RESTRICT_EVENT_BATTLE_GROUND_TOWN))
	{
		if(!m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &pcsCharacter->m_stPos))
		{
			// ���� ���� ��ǥ�� ���ϴ� �� �����ϸ�, ĳ�� ���� ������ ����������.
			m_pcsAgpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);
		}
	}

	//�α��ν� ��Ȱ�Ǹ� �ȵǴ� ������ ���� ��, ������ ������.
	if(pcsRegionTemplate && m_papmMap->CheckRegionPerculiarity(pcsRegionTemplate->nIndex, APMMAP_PECULIARITY_IS_ALLOW_LOGIN_PLACE) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
	{
		if(!m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &pcsCharacter->m_stPos))
		{
			// ���� ���� ��ǥ�� ���ϴ� �� �����ϸ�, ĳ�� ���� ������ ����������.
			m_pcsAgpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);
		}
	}

	return TRUE;
}

BOOL AgsmCharacter::IsNotLogoutStatus(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(pcsAgsdCharacter)
		return pcsAgsdCharacter->m_bIsNotLogout;

	return FALSE;
}

BOOL AgsmCharacter::IsResurrectingNow(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = GetADCharacter(pcsCharacter);
	if(pcsAgsdCharacter)
		return pcsAgsdCharacter->m_bIsResurrectingNow;

	return FALSE;
}

BOOL AgsmCharacter::CBGameGuardAuth(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

#ifdef _USE_NPROTECT_GAMEGUARD_

	AgsmCharacter* pThis	= (AgsmCharacter*)pClass;
	INT32 lCID				= *(INT32*)pData;

	GG_AUTH_DATA *pggData	= (GG_AUTH_DATA *)pCustData;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(lCID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = pThis->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	memcpy(&pagsdCharacter->csa->m_AuthAnswer, pggData, sizeof(GG_AUTH_DATA));
	DWORD dwRet = pagsdCharacter->csa->CheckAuthAnswer();
	if(dwRet != ERROR_SUCCESS)
	{
		{
			CHAR strLog[256] = { 0, };
			sprintf_s(strLog, sizeof(strLog), "GameGuard GetAuthQuery Error - [%s][%d]\n", pcsCharacter->m_szID, dwRet);
			AuLogFile_s("LOG\\GameGuardCB.txt", strLog);
		}

		pThis->DestroyClient(pagsdCharacter->m_dpnidCharacter);
	}
#endif

	return TRUE;
}

// ����Ǿ��ִ� SpecialStatus�� ���� ó��
// pcsCharacter->m_szSkillInit���⿡ �ִ� ���ڿ��� �Ľ��ؼ� ó���Ѵ�.
BOOL AgsmCharacter::ProcessSavedSpecialStatus(AgpdCharacter *pcsCharacter, UINT64 ulSpecialStatus, BOOL bAdd)
{
	if(NULL == pcsCharacter)
		return FALSE;

	CHAR* szStatus	= NULL;
	CHAR* szSeps    = ";";

	// ���� ulSpecialStatus�� �ش��ϴ� ���ڿ��� ����
	//////////////////////////////////////////////////////////////////////////

	if(ulSpecialStatus == AGPDCHAR_SPECIAL_STATUS_VOLUNTEER)
	{
		szStatus = AGPMCHAR_SAVED_SPECIAL_STATUS_VOLUNTEER;
	}

	//////////////////////////////////////////////////////////////////////////

	if(bAdd)
	{
		// �̹� �� �����̸� ����ó���Ѵ�.
		if(pcsCharacter->m_ulSpecialStatus & ulSpecialStatus)
			return FALSE;

		if(m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, ulSpecialStatus))
		{
			strcat_s(pcsCharacter->m_szSkillInit, AGPACHARACTER_MAX_CHARACTER_SKILLINIT, szStatus);
			strcat_s(pcsCharacter->m_szSkillInit, AGPACHARACTER_MAX_CHARACTER_SKILLINIT, szSeps);
		}
	}
	else
	{
		if(!(pcsCharacter->m_ulSpecialStatus & ulSpecialStatus))
			return FALSE;

		if(m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsCharacter, ulSpecialStatus))
		{
			// �켱 ���� ���ڿ��� m_szSkillInit�� ���� �����Ѵ�.
			CHAR szCharSavedStatus[AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1];
			ZeroMemory(szCharSavedStatus, AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1);

			CHAR* next_token = NULL;
			CHAR* token		 = NULL;
			token = strtok_s(pcsCharacter->m_szSkillInit, ";", &next_token);
			while (NULL != token)
			{
				// ���⼭ �ش� ���ڿ��� ������ Special_Status�� �����Ѵ�.
				if(strcmp(token, szStatus) == 0)
				{
					token = strtok_s(NULL, ";", &next_token);
					continue;
				}

				strcat_s(szCharSavedStatus, AGPACHARACTER_MAX_CHARACTER_SKILLINIT, token);
				strcat_s(szCharSavedStatus, AGPACHARACTER_MAX_CHARACTER_SKILLINIT, szSeps);

				token = strtok_s(NULL, ";", &next_token);
			}

			strncpy_s(pcsCharacter->m_szSkillInit, AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1, szCharSavedStatus, _TRUNCATE);
		}
	}

	if( BackupCharacterData(pcsCharacter) == FALSE )
		return FALSE;

	return TRUE;
}

BOOL AgsmCharacter::LoadSavedSpecialStatus(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	// �켱 ���� ���ڿ��� m_szSkillInit�� ���� �����Ѵ�.
	CHAR szCharSavedStatus[AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1];
	ZeroMemory(szCharSavedStatus, AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1);

	strncpy_s(szCharSavedStatus, AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1, pcsCharacter->m_szSkillInit, _TRUNCATE);

	CHAR* token			= NULL;
	CHAR* next_token	= NULL;
	token = strtok_s(szCharSavedStatus, ";", &next_token);
	while (token != NULL)
	{
		// ���⼭ �ش� ���ڿ��� ������ Special_Status�� �����Ѵ�.
		if(strcmp(token, AGPMCHAR_SAVED_SPECIAL_STATUS_VOLUNTEER) == 0)
		{
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsCharacter, AGPDCHAR_SPECIAL_STATUS_VOLUNTEER);
		}

		token = strtok_s(NULL, ";", &next_token);
	}

	return TRUE;
}

BOOL AgsmCharacter::MoveCharacterPosition(AgpdCharacter* pcsCharacter, FLOAT PosX, FLOAT PosZ)
{
	if(NULL == pcsCharacter)
		return FALSE;

	if( pcsCharacter->m_bIsTrasform )
	{
		m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
		return FALSE;
	}

	if( m_pcsAgpmCharacter->HasPenalty( pcsCharacter, AGPMCHAR_PENALTY_POTAL ) )
	{
		m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
		return FALSE;
	}

	AgsdCharacter *pagsdCharacter = GetADCharacter(pcsCharacter);
	if(NULL == pagsdCharacter)
		return FALSE;

	INT16 nRegionIndex = m_papmMap->GetRegion(PosX, PosZ);

	// ������ �ƴ� �� ���Ұ�
	if(!pcsCharacter->m_bIsAddMap)
		return FALSE;

	if(pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	if(m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	if( m_pcsAgpmCharacter->IsCombatMode(pcsCharacter)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK)
		|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		|| (m_pcsAgpmCharacter->IsActionBlockCondition(pcsCharacter)))
	{
		m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);
		return FALSE;
	}

	AgpmEpicZone *pcsEpicZone = (AgpmEpicZone*)GetModule("AgpmEpicZone");
	if(pcsEpicZone)
	{
		if(pcsEpicZone->IsEpicZone(nRegionIndex))
		{
			if(m_pagpmItem->IsEquipUnableItemInEpicZone(pcsCharacter))
			{
				// ������ ���� EpicZone�̰� �����ϸ� �ȵǴ� �������� �����ϰ� �ִٸ� �̵��Ҽ� ����.
				m_pagsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_DISABLE_EQUIP_ITEM_THIS_REGION);
				return FALSE;
			}
		}
	}

	AuPOS stPos;
	stPos.x = PosX;
	stPos.y = 0;
	stPos.z = PosZ;

	////////////////////////////////////////////////////////
	/// ���⼭���� �ɸ��� �̵� ó��.
	////////////////////////////////////////////////////////
	AuAutoLock pLock(pcsCharacter->m_Mutex);

	m_pcsAgpmCharacter->SetActionBlockTime(pcsCharacter, 3000);
	pagsdCharacter->m_bIsTeleportBlock = TRUE;

	m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
	m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, &stPos, FALSE, TRUE);

	pagsdCharacter->m_bIsTeleportBlock = FALSE;

	m_pcsAgpmSummons->UpdateAllSummonsPosToOwner(pcsCharacter);

	return TRUE;
}

BOOL AgsmCharacter::MoveCharacterIncludeParty(AgpdCharacter* pcsCharacter, FLOAT PosX, FLOAT PosZ)
{
	if(NULL == pcsCharacter)
		return FALSE;

	AgpdParty *pcsParty = m_pcsAgpmParty->GetParty(pcsCharacter);
	if(!pcsParty)
	{
		if(MoveCharacterPosition(pcsCharacter, PosX, PosZ))
		{
			SetDefaultInvincible(pcsCharacter);
		}
	}
	else
	{
		for(int lIndex=0; lIndex < pcsParty->m_nCurrentMember; lIndex++)
		{
			AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsParty->m_lMemberListID[lIndex]);
			if(m_pcsAgpmCharacter->IsPC(pcsTarget) || GetCharDPNID(pcsTarget))
			{
				if(MoveCharacterPosition(pcsTarget, PosX, PosZ))
				{
					SetDefaultInvincible(pcsTarget);
				}
			}
		}
	}

	return TRUE;
}
#ifndef _AREA_CHINA_
// base character info reading (szfile = BaseCharacterInfo.xml)
BOOL AgsmCharacter::StreamReadBaseCharacterInfo(CHAR *szFile)
{
	if(NULL == szFile)
		return FALSE;

	AuXmlDocument Doc;
	if(!Doc.LoadFile(szFile))
		return FALSE;

	AgpdCharacterTemplate		*pcsCharacterTemplate	= NULL;
	AgpdItemADCharTemplate		*pcsADItem				= NULL; 

	AuXmlNode *pNodeRoot = Doc.FirstChild("BaseCharacterSetting");
	if(pNodeRoot)
	{
		if ( atoi( pNodeRoot->ToElement()->Attribute("UseJumpingEventCharacter") ) )
		{
			m_pagpmConfig->SetServerStatusFlag(AGPM_CONFIG_FLAG_CHARACTER_JUMPING_EVENT);
		}

		AgsmLoginClient* pagsmLoginClient = (AgsmLoginClient*)GetModule("AgsmLoginClient");
		if(!pagsmLoginClient)
			return FALSE;

		//////////////////////////////////////////////////////////////////////////
		// Normal Character Information
		//////////////////////////////////////////////////////////////////////////
		AuXmlNode *pCharacterNode = pNodeRoot->FirstChild("Character");
		while(pCharacterNode)
		{
			AuXmlNode	 *pTIDNode = pCharacterNode->FirstChild("TID");
			AuXmlElement *pCharacterTIDElement = pCharacterNode->FirstChildElement("TID");

			if(pCharacterTIDElement && pTIDNode)
			{
				//////////////////////////////////////////////////////////////////////////
				// need AgsmLoginClient Module

				INT32 lTID				  = atoi(pCharacterTIDElement->GetText());
				INT32 lCharRace			  = atoi(pTIDNode->ToElement()->Attribute("Race"));
				INT32 lCharClass		  = atoi(pTIDNode->ToElement()->Attribute("Class"));
				CHAR *szCharName		  = (CHAR*)pTIDNode->ToElement()->Attribute("Name");

				pagsmLoginClient->SetBaseCharacterInfo(lCharRace, lCharClass, lTID, szCharName);
				//////////////////////////////////////////////////////////////////////////
				// need AgsmItemManager Module

				pcsCharacterTemplate	  = m_pcsAgpmCharacter->GetCharacterTemplate(lTID);
				if(NULL == pcsCharacterTemplate)
					return FALSE;

				// Equip Setting
				AuXmlNode *pEquipNode = pCharacterNode->FirstChild("Equip");
				if(pEquipNode)
				{
					AuXmlNode *pSetNode	  = pEquipNode->FirstChild("Set");

					while(pSetNode)
					{
						AgpdItemADCharTemplate::EquipItemSetInfo stEquipSetInfo;

						// setting index
						stEquipSetInfo.m_nSetNumber = atoi(pSetNode->ToElement()->Attribute("Index"));

						AuXmlNode *pItemNode  = pSetNode->FirstChild("Item");
						while(pItemNode)
						{
							INT32 lPartIndex  = atoi(pItemNode->ToElement()->Attribute("Part"));
							INT32 lItemTID	  = atoi(pItemNode->ToElement()->Attribute("TID"));

							if(lPartIndex == 0 || lItemTID == 0)
								break;

							// setting set item
							stEquipSetInfo.m_lDefaultEquipITID[lPartIndex] = lItemTID;

							pItemNode = pItemNode->NextSibling();
						}

						m_pagpmItem->SetDefaultEquipments(pcsCharacterTemplate, stEquipSetInfo);

						pSetNode = pSetNode->NextSibling();
					}
				}

				// Inventory Setting
				AuXmlNode *pInventoryNode = pCharacterNode->FirstChild("Inventory");
				if(pInventoryNode)
				{
					INT32 lIndex = 0;

					AgpdItemADCharTemplate::InvenItemSetInfo stInvenSetInfo;

					// setting index
					AuXmlNode *pDefaultNode = pInventoryNode->FirstChild("Default");
					while(pDefaultNode)
					{
						INT32 lItemTID	  = atoi(pDefaultNode->ToElement()->Attribute("TID"));
						INT32 lCount	  = atoi(pDefaultNode->ToElement()->Attribute("Count"));

						if(lItemTID == 0 || lCount == 0)
							break;

						stInvenSetInfo.m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID]			= lItemTID;
						stInvenSetInfo.m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT]	= lCount;
						lIndex++;

						pDefaultNode = pDefaultNode->NextSibling();
					}
					m_pagpmItem->SetDefaultInventoryItems(pcsCharacterTemplate, stInvenSetInfo);
				}

				// Gheld Setting
				AuXmlElement *pGheldElement = pCharacterNode->FirstChildElement("Gheld");
				if(pGheldElement)
				{
					CHAR* szGheld = (CHAR*)(pGheldElement->GetText());
					if(szGheld)
					{
						m_pagpmItem->SetDefaultMoney(pcsCharacterTemplate, atoi(szGheld));
					}
				}
			}

			pCharacterNode = pCharacterNode->NextSibling();
		}

		//////////////////////////////////////////////////////////////////////////
		// Jumping Event Character Information
		//////////////////////////////////////////////////////////////////////////
		AuXmlNode *pJumpingCharacterNode = pNodeRoot->FirstChild("JumpingCharacter");
		while(pJumpingCharacterNode)
		{
			AgsdServer *pcsThisServer = m_pAgsmServerManager2->GetThisServer();
			pcsThisServer->m_lJumpingStartLevel = atoi(pJumpingCharacterNode->ToElement()->Attribute("JumpingStartLevel"));
			m_pagpmConfig->SetJumpingStartLevel(atoi(pJumpingCharacterNode->ToElement()->Attribute("JumpingStartLevel")));

			AuXmlNode	 *pTIDNode = pJumpingCharacterNode->FirstChild("TID");
			AuXmlElement *pCharacterTIDElement = pJumpingCharacterNode->FirstChildElement("TID");

			if(pCharacterTIDElement && pTIDNode)
			{
				//////////////////////////////////////////////////////////////////////////
				// need AgsmLoginClient Module

				INT32 lTID				  = atoi(pCharacterTIDElement->GetText());
				INT32 lCharRace			  = atoi(pTIDNode->ToElement()->Attribute("Race"));
				INT32 lCharClass		  = atoi(pTIDNode->ToElement()->Attribute("Class"));
				CHAR *szCharName		  = (CHAR*)pTIDNode->ToElement()->Attribute("Name");

				pagsmLoginClient->SetBaseCharacterInfo(lCharRace, lCharClass, lTID, szCharName);
				//////////////////////////////////////////////////////////////////////////
				// need AgsmItemManager Module

				pcsCharacterTemplate	  = m_pcsAgpmCharacter->GetCharacterTemplate(lTID);
				if(NULL == pcsCharacterTemplate)
					return FALSE;

				// Equip Setting
				AuXmlNode *pEquipNode = pJumpingCharacterNode->FirstChild("Equip");
				if(pEquipNode)
				{
					AuXmlNode *pSetNode	  = pEquipNode->FirstChild("Set");

					while(pSetNode)
					{
						AgpdItemADCharTemplate::EquipItemSetInfo stEquipSetInfo;

						// setting index
						stEquipSetInfo.m_nSetNumber = atoi(pSetNode->ToElement()->Attribute("Index"));
						stEquipSetInfo.m_bIsJumpingEventEquip = TRUE;

						AuXmlNode *pItemNode  = pSetNode->FirstChild("Item");
						while(pItemNode)
						{
							INT32 lPartIndex  = atoi(pItemNode->ToElement()->Attribute("Part"));
							INT32 lItemTID	  = atoi(pItemNode->ToElement()->Attribute("TID"));

							if(lPartIndex == 0 || lItemTID == 0)
								break;

							// setting set item
							stEquipSetInfo.m_lDefaultEquipITID[lPartIndex] = lItemTID;

							pItemNode = pItemNode->NextSibling();
						}

						m_pagpmItem->SetDefaultEquipments(pcsCharacterTemplate, stEquipSetInfo);

						pSetNode = pSetNode->NextSibling();
					}
				}

				// Inventory Setting
				AuXmlNode *pInventoryNode = pJumpingCharacterNode->FirstChild("Inventory");
				if(pInventoryNode)
				{
					INT32 lIndex = 0;

					AgpdItemADCharTemplate::InvenItemSetInfo stInvenSetInfo;

					// setting index
					stInvenSetInfo.m_bIsJumpingEventItem = TRUE;

					AuXmlNode *pDefaultNode = pInventoryNode->FirstChild("Default");
					while(pDefaultNode)
					{
						INT32 lItemTID	  = atoi(pDefaultNode->ToElement()->Attribute("TID"));
						INT32 lCount	  = atoi(pDefaultNode->ToElement()->Attribute("Count"));

						if(lItemTID == 0 || lCount == 0)
							break;

						stInvenSetInfo.m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_TID]			= lItemTID;
						stInvenSetInfo.m_aalDefaultInvenInfo[lIndex][AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT]	= lCount;
						lIndex++;

						pDefaultNode = pDefaultNode->NextSibling();
					}
					m_pagpmItem->SetDefaultInventoryItems(pcsCharacterTemplate, stInvenSetInfo);
				}

				// Gheld Setting
				AuXmlElement *pGheldElement = pJumpingCharacterNode->FirstChildElement("Gheld");
				if(pGheldElement)
				{
					CHAR* szGheld = (CHAR*)(pGheldElement->GetText());
					if(szGheld)
					{
						m_pagpmItem->SetDefaultMoney(pcsCharacterTemplate, atoi(szGheld));
						AgsdServer *pThisServer = m_pAgsmServerManager2->GetThisServer();
						if (pThisServer)
						{
							pThisServer->m_llJumpingStartGheld = _atoi64(szGheld);
						}
					}
				}
			}

			pJumpingCharacterNode = pJumpingCharacterNode->NextSibling();
		}
	}

	return TRUE;
}
#endif