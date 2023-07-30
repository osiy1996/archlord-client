/******************************************************************************
Module:  AgsmParty.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 07
******************************************************************************/

#include "AgsmParty.h"

#include "ApLockManager.h"
#include "ApAutoLockCharacter.h"
#include "AuMath.h"
#include "AgpmBattleGround.h"
#include "AgsmSkill.h"
#include "AgpmEpicZone.h"

AgsmParty::AgsmParty()
{
	SetModuleName("AgsmParty");

	SetPacketType(AGSMPARTY_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,		// operation
							AUTYPE_INT32,		1,		// operator id
							AUTYPE_INT32,		1,		// target id
							AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1,
							AUTYPE_END,			0
							);

	EnableIdle2(TRUE);

	m_pagpmSiegeWar = NULL;
	m_pagpmBattleGround		= NULL;
	m_pagsmSystemMessage	= NULL;

	m_ulNextProcessTimeMSec	= 0;
}

AgsmParty::~AgsmParty()
{
}

BOOL AgsmParty::OnAddModule()
{
	m_papmMap				= (ApmMap *) GetModule("ApmMap");
	m_pagpmFactors			= (AgpmFactors *) GetModule("AgpmFactors");
	m_pagpmParty			= (AgpmParty *) GetModule("AgpmParty");
	m_pagpmCharacter		= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmSiegeWar			= (AgpmSiegeWar *) GetModule("AgpmSiegeWar");
	m_pagpmDropItem2		= (AgpmDropItem2 *) GetModule("AgpmDropItem2");
	m_pagpmItemConvert		= (AgpmItemConvert *) GetModule("AgpmItemConvert");
	m_papmEventManager		= (ApmEventManager *) GetModule("ApmEventManager");

	m_pagsmAOIFilter		= (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pAgsmServerManager	= (AgsmServerManager *) GetModule("AgsmServerManager2");
	m_pagsmFactors			= (AgsmFactors *) GetModule("AgsmFactors");
	m_pagsmCharacter		= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pagsmZoning			= (AgsmZoning *) GetModule("AgsmZoning");

	if (!m_papmMap ||
		!m_pagpmFactors || 
		!m_pagpmParty || 
		!m_pagpmCharacter || 
		!m_pagpmSiegeWar ||
		!m_pagpmDropItem2 ||
		!m_pagpmItemConvert ||
		!m_papmEventManager ||
		!m_pagsmAOIFilter ||
		!m_pAgsmServerManager || 
		!m_pagsmFactors || 
		!m_pagsmCharacter ||
		!m_pagsmZoning)
		return FALSE;

	/*
	m_nIndexADParty = m_pagpmParty->AttachPartyData(this, sizeof(AgsdParty), ConAgsdParty, DesAgsdParty);
	if (m_nIndexADParty < 0)
		return FALSE;
	*/

	if (!m_pagpmFactors->SetCallbackUpdateFactorParty(CBUpdateFactorParty, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackUpdateStatus(CBUpdateActionStatus, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackTransformStatus(CBTransform, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackRestoreTransform(CBTransform, this))
		return FALSE;

	/*
	if (!m_papmMap->SetCallbackMoveChar(CBMoveSector, this))
		return FALSE;
	*/

	if (!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveCell, this))
		return FALSE;

	if (!m_pagpmParty->SetCBCreateParty(CBCreateParty, this))
		return FALSE;

	if (!m_pagpmParty->SetCBRemoveParty(CBRemoveParty, this))
		return FALSE;
	if (!m_pagpmParty->SetCBRemoveID(CBRemovePartyID, this))
		return FALSE;

	if (!m_pagpmParty->SetCBAddMember(CBAddMember, this))
		return FALSE;

	if (!m_pagpmParty->SetCBRemoveMember(CBRemoveMember, this))
		return FALSE;

	if (!m_pagpmParty->SetCBChangeLeader(CBChangeLeader, this))
		return FALSE;

	if (!m_pagpmParty->SetCBAddParty(CBAddParty, this))
		return FALSE;

	if (!m_pagpmParty->SetCBUpdateFactor(CBUpdateFactor, this))
		return FALSE;

	if (!m_pagpmParty->SetCBSyncRemoveCharacter(CBSyncCharacterRemove, this))
		return FALSE;

	if (!m_pagpmParty->SetCBCharLevelUp(CBCharLevelUp, this))
		return FALSE;

	if (!m_pagpmParty->SetCBResetEffectArea(CBResetEffectFactor, this))
		return FALSE;

	if (!m_pagpmParty->SetCBUpdateExpType(CBUpdateExpType, this))
		return FALSE;

	if (!m_pagpmParty->SetCBUpdatePartyFactor(CBUpdatePartyFactor, this))
		return FALSE;

	if (!m_pagpmParty->SetCBUpdateItemDivision(CBUpdateItemDivision, this))		// 2005.04.19 By SungHoon
		return FALSE;

	if (!m_pagpmDropItem2->SetCallbackGetBonusOfPartyMember(CBGetBonusOfPartyMember, this))
		return FALSE;

	if (!m_pagsmZoning->SetCallbackZoningPassControl(CBZoningPassControl, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackUpdateControlServer(CBUpdateControlServer, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackSyncCharacterActions(CBSyncCharacterActions, this))
		return FALSE;


#ifdef	AGSMPARTY_UPDATE_PARTY_FACTOR
	if (!m_pagsmCharacter->SetCallbackReCalcFactor(CBReCalcFactor, this))
		return FALSE;
#endif

//	2005.10.24. By SungHoon
	if (!m_pagpmCharacter->SetCallbackUpdateMurdererPoint(CBUpdateMurdererPoint, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmParty::OnInit()
{
	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagsmSkill			= (AgsmSkill*)GetModule("AgsmSkill");
	m_pagpmSummons			= (AgpmSummons*)GetModule("AgpmSummons");
	m_pagsmSystemMessage	= (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	m_pagpmEpicZone			= (AgpmEpicZone*)GetModule("AgpmEpicZone");
	
	if(!m_pagpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
		return FALSE;
	
	if(!m_pagpmBattleGround || !m_pagsmSkill
		|| !m_pagsmSystemMessage
		)
		return FALSE;
		
	return TRUE;
}

BOOL AgsmParty::OnDestroy()
{
	return TRUE;
}

BOOL AgsmParty::OnIdle2(UINT32 ulClockCount)
{
	//STOPWATCH2(GetModuleName(), _T("OnIdle2"));

	if (m_ulNextProcessTimeMSec <= ulClockCount)
	{
		INT32	lIndex	= 0;

		AgpdParty	*pcsParty	= m_pagpmParty->GetPartySequence(&lIndex);
		while (pcsParty)
		{
			if (pcsParty->m_Mutex.WLock())
			{
				m_pagpmParty->SetEffectArea(pcsParty);

				pcsParty->m_Mutex.Release();
			}

			pcsParty	= m_pagpmParty->GetPartySequence(&lIndex);
		}

		m_ulNextProcessTimeMSec	= ulClockCount + AGSMPARTY_PROCESS_IDLE_INTERVAL;
	}

	return TRUE;
}

/*
	2005.04.19	By SungHoon
	��Ƽ ����� �ʴ��ҋ�
*/
BOOL AgsmParty::InvitePartyMember(  UINT32 ulNID, INT32 lOperatorID, INT32 lTargetID, CHAR *szTargetID )
{
	if (lOperatorID == AP_INVALID_CID || lOperatorID == lTargetID || (lTargetID == AP_INVALID_CID && szTargetID == NULL))
	{
		SendPacketFailed(lOperatorID, ulNID);
		return FALSE;
	}

	AgpdCharacter	*pcsOperator = m_pagpmCharacter->GetCharacterLock(lOperatorID);
	if (!pcsOperator || m_pagsmCharacter->GetCharDPNID(pcsOperator) == 0)
	{
		SendPacketFailed(lOperatorID, ulNID);
		return FALSE;
	}

	// RegionPeculiarity Check
	if(m_papmMap->CheckRegionPerculiarity(pcsOperator->m_nBindingRegionIndex, APMMAP_PECULIARITY_PARTY) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
	{
		SendPacketFailed(lOperatorID, ulNID);
		return FALSE;
	}

	INT32 lOperatorMurdererLevel = m_pagpmCharacter->GetMurdererLevel(pcsOperator);
	if (lOperatorMurdererLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT)		//	MurdererPoint�� ������ ��Ƽ ������
	{
		SendPacketInviteFailed(lOperatorID,
								lTargetID,
								AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_OPERATOR,
								ulNID);
		pcsOperator->m_Mutex.Release();
		return FALSE;
	}


	
	AgpdPartyADChar	*pcsOperatorPartyADChar = m_pagpmParty->GetADCharacter(pcsOperator);

	INT32	lPartyID	= pcsOperatorPartyADChar->lPID;

	pcsOperator->m_Mutex.Release();

	AgpdCharacter	*pcsTargetChar = NULL;

	if (lTargetID == AP_INVALID_CID) 
		pcsTargetChar = m_pagpmCharacter->GetCharacterLock(szTargetID);
	else 
		pcsTargetChar = m_pagpmCharacter->GetCharacterLock(lTargetID);
	
	if (!pcsTargetChar)		// 2005.04.21. By SungHoon
	{
		// �����ߴٰ� �˷��ش�.
		if (szTargetID == NULL) SendPacketFailed(lOperatorID, ulNID);
		else SendPacketInviteFailed(lOperatorID, szTargetID, AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_NO_LOGIN_MEMBER, ulNID);

		return FALSE;
	}

	// ��Ʋ ����� üũ�ϴ����� �������� �����ΰ��� ��� ��Ƽ �ȵǰ� ������ �ٲ�. supertj@20100413
	// ��Ƽ��û�ڿ� ��Ƽ����ڰ� �ٸ� ������ �ְ� ���߿� ��� �Ѹ��� �������� ������ �������� ��Ƽ�� ���ϰ� �Ѵ�.
	ApmMap::RegionTemplate	*pcsRegionTemplateMe = m_papmMap->GetTemplate(pcsOperator->m_nBindingRegionIndex);	//��Ƽ��û�� ��������.
	ApmMap::RegionTemplate	*pcsRegionTemplateTarget = m_papmMap->GetTemplate(pcsTargetChar->m_nBindingRegionIndex);	//��Ƽ����� ��������.

	if (pcsRegionTemplateMe && pcsRegionTemplateTarget)
	{
		if(pcsRegionTemplateMe->ti.stType.uSafetyType != pcsRegionTemplateTarget->ti.stType.uSafetyType) //�ٸ��������� üũ
		{
			if( pcsRegionTemplateMe->ti.stType.uSafetyType == ApmMap::ST_DANGER ||	//�������������� �ִ��� üũ
				pcsRegionTemplateTarget->ti.stType.uSafetyType == ApmMap::ST_DANGER )
			{
				SendPacketFailed(lOperatorID, ulNID);
				pcsTargetChar->m_Mutex.Release();
				return FALSE;
			}
		}
	}

	// RegionPeculiarity Check
	if(m_papmMap->CheckRegionPerculiarity(pcsTargetChar->m_nBindingRegionIndex, APMMAP_PECULIARITY_PARTY) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
	{
		SendPacketFailed(lOperatorID, ulNID);
		pcsTargetChar->m_Mutex.Release();
		return FALSE;
	}
	
	INT32 lTargetMurdererLevel = m_pagpmCharacter->GetMurdererLevel(pcsTargetChar);
	if (lTargetMurdererLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT)
	{
		SendPacketInviteFailed(lOperatorID,
								pcsTargetChar->m_szID,
								AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_MURDERER_TARGET,
								ulNID);
		pcsTargetChar->m_Mutex.Release();
		return FALSE;
	}

	if (m_pagpmCharacter->IsOptionFlag(pcsTargetChar,AGPDCHAR_OPTION_REFUSE_PARTY_IN) == TRUE)	//	2005.05.31. �ź����� Ȯ���Ѵ�.
	{
		SendPacketInviteFailed(lOperatorID, pcsTargetChar->m_szID, AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_REFUSE, ulNID);
		pcsTargetChar->m_Mutex.Release();
		return FALSE;
	}

	// Ban list���� �ʴ� �������� Ȯ���Ѵ�.
	if (FALSE == EnumCallback(AGSMPARTY_CB_BUDDY_CHECK, pcsOperator, pcsTargetChar))
	{
		SendPacketInviteFailed(lOperatorID, pcsTargetChar->m_szID, AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_REFUSE, ulNID);
		pcsTargetChar->m_Mutex.Release();
		return FALSE;
	}

	if (m_pagsmCharacter->GetCharDPNID(pcsTargetChar) == 0)
	{
		// �����ߴٰ� �˷��ش�.
		SendPacketFailed(lOperatorID, ulNID);
		pcsTargetChar->m_Mutex.Release();
		return FALSE;
	}
	
	BOOL bOperator = m_pagpmBattleGround->IsInBattleGround(pcsOperator);
	BOOL bTarget = m_pagpmBattleGround->IsInBattleGround(pcsTargetChar);
	if(	(bOperator && bTarget && m_pagpmCharacter->IsSameRace(pcsOperator, pcsTargetChar) == FALSE)
		|| (bOperator && !bTarget) || (!bOperator && bTarget) )
	{
		SendPacketFailed(lOperatorID, ulNID);
		pcsOperator->m_Mutex.Release();
		return FALSE;
	}

	bOperator = m_pagpmEpicZone->IsInEpicZone(pcsOperator);
	bTarget = m_pagpmEpicZone->IsInEpicZone(pcsTargetChar);
	if((bOperator && !bTarget) || (!bOperator && bTarget))
	{
		SendPacketFailed(lOperatorID, ulNID);
		pcsOperator->m_Mutex.Release();
		return FALSE;
	}

	AgpdPartyADChar *pcsTargetPartyADChar = m_pagpmParty->GetADCharacter(pcsTargetChar);

	if (pcsTargetPartyADChar->lPID != AP_INVALID_PARTYID)
	{
		SendPacketInviteFailed(lOperatorID,
								lTargetID,
								AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER,
								ulNID);
		pcsTargetChar->m_Mutex.Release();
		return FALSE;
	}

	AgpdParty	*pcsParty = NULL;

	if (lPartyID != AP_INVALID_PARTYID)
	{
		pcsParty = m_pagpmParty->GetPartyLock(pcsOperatorPartyADChar->lPID);
		if (!pcsParty)
		{
			pcsTargetChar->m_Mutex.Release();
			SendPacketFailed(lOperatorID, ulNID);
			return FALSE;
		}
	}
	
	if (pcsParty)
	{
		// ��Ƽ�� �߰��� �� �ִ��� �˻��Ѵ�.
		// �ʴ��� ĳ���Ͱ� ��Ƽ �������� �˻��Ѵ�.
		if (pcsParty->m_lMemberListID[0] != lOperatorID)
		{
			SendPacketInviteFailed(lOperatorID,
									lTargetID,
									AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER,
									ulNID);
			pcsTargetChar->m_Mutex.Release();
			pcsParty->m_Mutex.Release();
			return FALSE;
		}
		else if (!IsAddMember(pcsParty, pcsTargetChar))
		{
			SendPacketInviteFailed(lOperatorID,
									lTargetID,
									AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_PARTY_MEMBER_IS_FULL,
									ulNID);
			pcsTargetChar->m_Mutex.Release();
			pcsParty->m_Mutex.Release();
			return FALSE;
		}
		else if (m_pagpmParty->IsMember(pcsParty, pcsTargetChar->m_lID))
		{
			// �̹� ���ԵǾ��� �ִ� �����.
			SendPacketInviteFailed(lOperatorID,
									lTargetID,
									AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_PARTY_MEMBER,
									ulNID);
			pcsTargetChar->m_Mutex.Release();
			pcsParty->m_Mutex.Release();
			return FALSE;
		}
/*		else			// ���� üũ�� ���ش�. By SungHoon 2005.06.08
		{
//	���� üũ �ؾ� �Ѵ�.

			INT32	lMemberLevel = m_pagpmCharacter->GetLevel(pcsTargetChar);
			INT32	lLevelOffset = AGPMPARTY_AVAILABLE_LEVEL_DIFF - (pcsParty->m_lHighestMemberLevel - pcsParty->m_lLowerMemberLevel);
			if (lLevelOffset < 0 || lLevelOffset > AGPMPARTY_AVAILABLE_LEVEL_DIFF) lLevelOffset = AGPMPARTY_AVAILABLE_LEVEL_DIFF;
			if ((lMemberLevel + lLevelOffset < pcsParty->m_lLowerMemberLevel) ||
				(lMemberLevel > pcsParty->m_lHighestMemberLevel + lLevelOffset))
			{
				SendPacketInviteFailed(lOperatorID,
										lTargetID,
										AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_LEVEL_DIFF,
										ulNID);
				pcsTargetChar->m_Mutex.Release();
				pcsParty->m_Mutex.Release();
				return FALSE;
			}
		}
*/
	}
/*	else			// ���� üũ�� ���ش�. By SungHoon 2005.06.08
	{
//	���� üũ �ؾ� �Ѵ�.
		INT32	lLevelOffset = abs( m_pagpmCharacter->GetLevel(pcsOperator) - m_pagpmCharacter->GetLevel(pcsTargetChar) );
		if ( AGPMPARTY_AVAILABLE_LEVEL_DIFF <= lLevelOffset)
		{
			SendPacketInviteFailed(lOperatorID,
									lTargetID,
									AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_LEVEL_DIFF,
									ulNID);
			pcsTargetChar->m_Mutex.Release();
			return FALSE;
		}
	}
*/
	AgsdCharacter *pcsAgsdTargetChar = m_pagsmCharacter->GetADCharacter(pcsTargetChar);

	// target character���� �ʴ��Ѵٴ� ������ �����ش�.
	SendPacketInvite(lOperatorID, pcsTargetChar->m_lID, pcsOperator->m_szID, pcsAgsdTargetChar->m_dpnidCharacter);


	if (pcsParty)
		pcsParty->m_Mutex.Release();

	pcsTargetChar->m_Mutex.Release();

	return TRUE;
}


BOOL AgsmParty::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT8		cOperation	= (-1);
	INT32		lOperatorID	= AP_INVALID_CID;
	INT32		lTargetID	= AP_INVALID_CID;
	CHAR		*szTargetID = NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,
						&lOperatorID,
						&lTargetID,
						&szTargetID );

	switch (cOperation) {
	case AGSMPARTY_PACKET_OPERATION_INVITE:
		{
			return ( InvitePartyMember( ulNID, lOperatorID, lTargetID, szTargetID ) );
		}
		break;

	case AGSMPARTY_PACKET_OPERATION_REJECT:
		{
			if (lOperatorID == AP_INVALID_CID || lTargetID == AP_INVALID_CID)
			{
				SendPacketFailed(lOperatorID, ulNID);
				return FALSE;
			}

			AgpdCharacter *pcsTargetChar = m_pagpmCharacter->GetCharacterLock(lTargetID);
			if (!pcsTargetChar)
			{
				SendPacketFailed(lOperatorID, ulNID);
				return FALSE;
			}

			AgsdCharacter *pcsAgsdTargetChar = m_pagsmCharacter->GetADCharacter(pcsTargetChar);

			// ��Ŷ�� �����ش�. (target ���Է�...)
			SendPacketReject(lOperatorID, lTargetID, pcsAgsdTargetChar->m_dpnidCharacter);

			pcsTargetChar->m_Mutex.Release();
		}
		break;

	case AGSMPARTY_PACKET_OPERATION_INVITE_ACCEPT:
		{
			if (lOperatorID == AP_INVALID_CID || lTargetID == AP_INVALID_CID || lOperatorID == lTargetID)
			{
				SendPacketFailed(lTargetID, ulNID);
				return FALSE;
			}

			// lTargetID�� �ʴ��� ���̰� lOperatorID�� �ʴ븦 ���� ���̴�.
			///////////////////////////////////////////////////////////////////////

			AgpdCharacter *pcsTargetChar = m_pagpmCharacter->GetCharacterLock(lTargetID);
			if (!pcsTargetChar)
			{
				return FALSE;
			}
			
			if (m_pagsmCharacter->GetCharDPNID(pcsTargetChar) == 0)
			{
				pcsTargetChar->m_Mutex.Release();
				return FALSE;
			}

			AgpdPartyADChar	*pcsTargetPartyADChar = m_pagpmParty->GetADCharacter(pcsTargetChar);

			INT32	lPartyID		= pcsTargetPartyADChar->lPID;
			UINT32	ulTargetDPNID	= m_pagsmCharacter->GetCharDPNID(pcsTargetChar);

			/* �Ʒ��� ����Ѵ�. Parn
			if (lPartyID != AP_INVALID_PARTYID)
				pcsTargetChar->m_Mutex.Release();
			*/

			AgpdCharacter *pcsOperator = m_pagpmCharacter->GetCharacterLock(lOperatorID);
			if (!pcsOperator || m_pagsmCharacter->GetCharDPNID(pcsOperator) == 0)
			{
				pcsTargetChar->m_Mutex.Release();

				if (pcsOperator)
					pcsOperator->m_Mutex.Release();

				return FALSE;
			}

			// Ÿ �������� ��Ʋ �׶��忡 ������� ��Ƽ �Ұ�. , 20090203 absenty
			BOOL bOperator = m_pagpmBattleGround->IsInBattleGround(pcsOperator);
			BOOL bTarget = m_pagpmBattleGround->IsInBattleGround(pcsTargetChar);
			if(	(bOperator && bTarget && m_pagpmCharacter->IsSameRace(pcsOperator, pcsTargetChar) == FALSE)
				|| (bOperator && !bTarget) || (!bOperator && bTarget) )
			{
				SendPacketFailed(lOperatorID, ulNID);
				pcsOperator->m_Mutex.Release();
				return FALSE;
			}

			bOperator = m_pagpmEpicZone->IsInEpicZone(pcsOperator);
			bTarget = m_pagpmEpicZone->IsInEpicZone(pcsTargetChar);
			if((bOperator && !bTarget) || (!bOperator && bTarget))
			{
				SendPacketFailed(lOperatorID, ulNID);
				pcsOperator->m_Mutex.Release();
				return FALSE;
			}

			//AgsdCharacter *pcsAgsdTargetChar = m_pagsmCharacter->GetADCharacter(pcsTargetChar);
			
			AgpdPartyADChar	*pcsOperatorPartyADChar = m_pagpmParty->GetADCharacter(pcsOperator);

			AgpdParty	*pcsParty = NULL;
			if (lPartyID != AP_INVALID_PARTYID)
			{
				pcsParty = m_pagpmParty->GetPartyLock(lPartyID);
				if (!pcsParty)
				{
					pcsTargetChar->m_Mutex.Release();
					pcsOperator->m_Mutex.Release();
					SendPacketFailed(lTargetID, ulTargetDPNID);

					return FALSE;
				}
			}

			if (pcsOperatorPartyADChar->lPID != AP_INVALID_PARTYID)
			{
				SendPacketFailed(lOperatorID, ulNID);
				//SendPacketFailed(lTargetID, ulTargetDPNID);

				SendPacketInviteFailed(lOperatorID,
									   lTargetID,
									   AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER,
									   ulNID);

				if (pcsParty)
					pcsParty->m_Mutex.Release();

				pcsOperator->m_Mutex.Release();

				pcsTargetChar->m_Mutex.Release();

				return FALSE;
			}

			if (pcsParty)
			{
				// ��Ƽ�� �߰��� �� �ִ��� �˻��Ѵ�.
				// �ʴ��� ĳ���Ͱ� ��Ƽ �������� �˻��Ѵ�.
				if (pcsParty->m_lMemberListID[0] != lTargetID)
				{
					SendPacketInviteFailed(lTargetID,
										   lOperatorID,
										   AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_OTHER_PARTY_MEMBER,
										   ulTargetDPNID);
					SendPacketFailed(lOperatorID, ulNID);
					pcsParty->m_Mutex.Release();
					pcsOperator->m_Mutex.Release();

					pcsTargetChar->m_Mutex.Release();

					return FALSE;
				}
				else if (!IsAddMember(pcsParty, pcsOperator))
				{
					SendPacketInviteFailed(lTargetID,
										   lOperatorID,
										   AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_PARTY_MEMBER_IS_FULL,
										   ulTargetDPNID);
					SendPacketFailed(lOperatorID, ulNID);
					pcsParty->m_Mutex.Release();
					pcsOperator->m_Mutex.Release();

					pcsTargetChar->m_Mutex.Release();

					return FALSE;
				}
				else if (m_pagpmParty->IsMember(pcsParty, pcsOperator->m_lID))
				{
					// �̹� ���ԵǾ��� �ִ� �����.
					SendPacketInviteFailed(lTargetID,
										   lOperatorID,
										   AGSMPARTY_PACKET_OPERATION_INVITE_FAILED_ALREADY_PARTY_MEMBER,
										   ulTargetDPNID);
					SendPacketFailed(lOperatorID, ulNID);

					pcsParty->m_Mutex.Release();
					pcsOperator->m_Mutex.Release();

					pcsTargetChar->m_Mutex.Release();

					return FALSE;
				}

				/*
				// ��Ƽ�� �߰��� �� �ִ��� �˻��Ѵ�.
				// �ʴ��� ĳ���Ͱ� ��Ƽ �������� �˻��Ѵ�.
				if (pcsParty->m_lMemberListID[0] != lTargetID || !IsAddMember(pcsParty, pcsOperator) || m_pagpmParty->IsMember(pcsParty, pcsOperator->m_lID))
				{
					SendPacketFailed(lOperatorID, ulNID);
					SendPacketFailed(lTargetID, ulTargetDPNID);

					pcsParty->m_Mutex.Release();
					pcsOperator->m_Mutex.Release();
					if (lPartyID == AP_INVALID_PARTYID)
						pcsTargetChar->m_Mutex.Release();
					return FALSE;
				}
				*/
			}

			// ��Ƽ�� �ͱٴ�. Ȥ�� ��Ƽ�� �߰��Ѵ�.

			if (!pcsParty)		// ��Ƽ�� ����. ���� �����Ѵ�.
			{
				pcsParty = m_pagpmParty->CreateParty(m_csGenerateID.GetID(), pcsTargetChar, pcsOperator);
				if (!pcsParty)
				{
					SendPacketFailed(lOperatorID, ulNID);
					SendPacketFailed(lTargetID, ulTargetDPNID);
					pcsOperator->m_Mutex.Release();

					pcsTargetChar->m_Mutex.Release();

					return FALSE;
				}
				else
					pcsParty = NULL;
			}
			else														// �̹� �����ϴ� ��Ƽ��. �� target �� ��Ƽ�� �߰��Ѵ�.
			{
				if (!m_pagpmParty->AddMember(pcsParty, pcsOperator))
				{
					SendPacketFailed(lOperatorID, ulNID);
					SendPacketFailed(lTargetID, ulTargetDPNID);
					pcsParty->m_Mutex.Release();
					pcsOperator->m_Mutex.Release();

					pcsTargetChar->m_Mutex.Release();

					return FALSE;
				}
			}

			// ��Ŷ�� �����ش�. (target ���Է�...)

			SendPacketAccept(lOperatorID, lTargetID, ulTargetDPNID);

			if (pcsParty)
				pcsParty->m_Mutex.Release();

			pcsOperator->m_Mutex.Release();

			pcsTargetChar->m_Mutex.Release();
		}
		break;

	case AGSMPARTY_PACKET_OPERATION_LEAVE:
		{
			return ProcessRemoveMember(lOperatorID);
		}
		break;

	case AGSMPARTY_PACKET_OPERATION_BANISH:
		{
			if (lOperatorID == AP_INVALID_CID || lTargetID == AP_INVALID_CID || lOperatorID == lTargetID)
			{
				SendPacketFailed(lOperatorID, ulNID);
				return FALSE;
			}

			AgpdCharacter *pcsOperator = m_pagpmCharacter->GetCharacterLock(lOperatorID);
			if (!pcsOperator)
			{
				SendPacketFailed(lOperatorID, ulNID);
				return FALSE;
			}

			AgpdPartyADChar	*pcsOperatorPartyADChar = m_pagpmParty->GetADCharacter(pcsOperator);

			INT32	lPartyID	= pcsOperatorPartyADChar->lPID;

			pcsOperator->m_Mutex.Release();

			AgpdParty	*pcsParty = m_pagpmParty->GetPartyLock(lPartyID);
			if (!pcsParty || !m_pagpmParty->IsMember(pcsParty, lTargetID))
			{
				SendPacketFailed(lOperatorID, ulNID);
				return FALSE;
			}

			// lOperator�� ��Ƽ �������� �˻��Ѵ�. (������ �߹��� �� �ִ�.)
			if (pcsParty->m_lMemberListID[0] != lOperatorID)
			{
				pcsParty->m_Mutex.Release();
				SendPacketFailed(lOperatorID, ulNID);
				return FALSE;
			}

			AgpdCharacter *pcsTargetChar = m_pagpmCharacter->GetCharacterLock(lTargetID);
			if (!pcsTargetChar)
			{
				pcsParty->m_Mutex.Release();
				SendPacketFailed(lOperatorID, ulNID);
				return FALSE;
			}

			AgsdCharacter *pcsAgsdTargetChar = m_pagsmCharacter->GetADCharacter(pcsTargetChar);

			// �߹��Ѵٴ� ��Ŷ�� target���� ������.
			SendPacketBanish(lOperatorID, lTargetID, pcsAgsdTargetChar->m_dpnidCharacter);

			// ��Ƽ���� ����.
			BOOL	bDestroyParty = FALSE;

			m_pagpmParty->RemoveMember(pcsParty, lTargetID, &bDestroyParty);

			pcsTargetChar->m_Mutex.Release();

			if (!bDestroyParty)
				pcsParty->m_Mutex.Release();
		}
		break;
	case AGSMPARTY_PACKET_OPERATION_DELEGATION_LEADER :
		{
			DelegationLeader(ulNID, lOperatorID,lTargetID);
		}
		break;

	case AGSMPARTY_PACKET_OPERATION_ACCEPT_RECALL:
		{
			OnReceiveAcceptRecall(lOperatorID, lTargetID);
		}
		break;

	case AGSMPARTY_PACKET_OPERATION_REJECT_RECALL:
		{
			OnReceiveRejectRecall(lOperatorID, lTargetID);
		}
		break;
	}

	return TRUE;
}

//		IsAddMember
//	Functions
//		- ��Ƽ�� ����� �߰��� �� �ִ��� �˻��Ѵ�.
//			1. ��Ƽ�� ��á���� �˻��Ѵ�.
//			2. �߰��Ϸ��� ����� �̹� �ٸ� ��Ƽ�� ��� �ִ��� �˻��Ѵ�.
//	Arguments
//		- pcsPartyLeader : ��Ƽ��
//		- pcsMember : �߰��Ϸ��� ��Ƽ��
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmParty::IsAddMember(AgpdCharacter *pcsPartyLeader, AgpdCharacter *pcsMember)
{
	if (!pcsPartyLeader || !pcsMember)
		return FALSE;

	AgpdPartyADChar *pcsLeaderPartyADChar = m_pagpmParty->GetADCharacter(pcsPartyLeader);

	if (pcsLeaderPartyADChar->pcsParty)
		return IsAddMember(pcsLeaderPartyADChar->pcsParty, pcsMember);
	else
		return IsAddMember(m_pagpmParty->GetParty(pcsLeaderPartyADChar->lPID), pcsMember);

	return TRUE;
}

BOOL AgsmParty::IsAddMember(AgpdParty *pcsParty, AgpdCharacter *pcsMember)
{
	if (!pcsParty || !pcsMember)
		return FALSE;

	if (m_pagpmParty->IsFull(pcsParty))
		return FALSE;									// ��Ƽ�� �̹� ��ȭ���´�.

	AgpdPartyADChar	*pcsMemberPartyADChar = m_pagpmParty->GetADCharacter(pcsMember);

	// ����� �߰��Ϸ��� ���� �̹� ��Ƽ�� ����ִ�.
	if (pcsMemberPartyADChar->lPID != AP_INVALID_PARTYID)
	{
		return FALSE;
	}
	
	AgpdCharacter* pcsLeader = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[0]);
	if(m_pagpmBattleGround->IsAttackable(pcsLeader, pcsMember)) return FALSE;

	return TRUE;
}

BOOL AgsmParty::ProcessRemoveMember(INT32 lOperatorID)
{
	if (lOperatorID == AP_INVALID_CID)
		return FALSE;

	// ��Ƽ���� ����.

	AgpdCharacter *pcsOperator = m_pagpmCharacter->GetCharacter(lOperatorID);
	if (!pcsOperator) return FALSE;
	AuAutoLock pLock2(pcsOperator->m_Mutex);

	AgpdPartyADChar	*pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsOperator);
	if(!pcsPartyADChar) return FALSE;

	AgpdParty *pcsParty = m_pagpmParty->GetPartyLock(pcsPartyADChar->lPID);
	if (!pcsParty) return FALSE;
	AuAutoLock pLock(pcsParty->m_Mutex);

	BOOL	bDestroyParty = FALSE;
	m_pagpmParty->RemoveMember(pcsParty, pcsOperator->m_lID, &bDestroyParty);

	return TRUE;
}

BOOL AgsmParty::SendPacketInvite(INT32 lOperator, INT32 lTarget, CHAR *szTargetID, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketInvite(lOperator, lTarget, szTargetID, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketInvite() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketReject(INT32 lOperator, INT32 lTarget, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketReject(lOperator, lTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketReject() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketAccept(INT32 lOperator, INT32 lTarget, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketAccept(lOperator, lTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketAccept() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketLeave(INT32 lOperator, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketLeave(lOperator, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketLeave() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketBanish(INT32 lOperator, INT32 lTarget, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketBanish(lOperator, lTarget, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketBanish() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketInviteFailed(INT32 lOperator, INT32 lTarget, eAgsmPartyPacketOperation eFailReason, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketInviteFailed(lOperator, lTarget, eFailReason, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketInviteFailed() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketInviteFailed(INT32 lOperator, CHAR *szTargetID, eAgsmPartyPacketOperation eFailReason, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketInviteFailed(lOperator, szTargetID, eFailReason, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketInviteFailed() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketFailed(INT32 lOperator, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketFailed(lOperator, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
//		TRACEFILE("AgsmParty::SendPacketFailed() failed");
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

/*
	2005.05.02	By SungHoon
	��Ƽ�� ���ӿ� �����ߴٴ� ��Ŷ�� ������ ��û�� �÷��̾�� ������.
*/
BOOL AgsmParty::SendDelegationLeaderFailed(INT32 lOperator, DPNID dpnid)
{
	INT16	nPacketLength;

	PVOID	pvPacket = MakePacketDelegationLeaderFailed(lOperator, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid))
	{
		m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmParty::SendPacketRequestRecall(INT32 lOperator, UINT32 ulNID)
{
	if (ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketRequestRecall(lOperator, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmParty::SendPacketAcceptRecall(INT32 lOperator, INT32 lTargetID, UINT32 ulNID)
{
	if (ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketAcceptRecall(lOperator, lTargetID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmParty::SendPacketRejectRecall(INT32 lOperator, INT32 lTargetID, UINT32 ulNID)
{
	if (ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= MakePacketRejectRecall(lOperator, lTargetID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

PVOID AgsmParty::MakePacketInvite(INT32 lOperator, INT32 lTarget, CHAR *szTargetID, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_INVITE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												szTargetID);

	return pvPacket;
}

PVOID AgsmParty::MakePacketReject(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_REJECT;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgsmParty::MakePacketAccept(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_INVITE_ACCEPT;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgsmParty::MakePacketLeave(INT32 lOperator, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_LEAVE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												NULL);

	return pvPacket;
}

PVOID AgsmParty::MakePacketBanish(INT32 lOperator, INT32 lTarget, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_BANISH;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgsmParty::MakePacketInviteFailed(INT32 lOperator, CHAR *szTargetID, eAgsmPartyPacketOperation eFailReason, INT16 *pnPacketLength)
{
	INT8	cOperation = eFailReason;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												szTargetID);

	return pvPacket;
}

PVOID AgsmParty::MakePacketInviteFailed(INT32 lOperator, INT32 lTarget, eAgsmPartyPacketOperation eFailReason, INT16 *pnPacketLength)
{
	INT8	cOperation = eFailReason;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTarget,
												NULL);

	return pvPacket;
}

PVOID AgsmParty::MakePacketFailed(INT32 lOperator, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_FAILED;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												NULL);

	return pvPacket;
}

/*
	2005.05.02	By SungHoon
	��Ƽ�� ���Ӽ��� ��Ŷ�� �����.
*/
PVOID AgsmParty::MakePacketDelegationLeader(INT32 lOperator, INT32 lTargetID, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_DELEGATION_LEADER;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTargetID,
												NULL);

	return pvPacket;
}

/*
	2005.05.02	By SungHoon
	��Ƽ�� ���ӽ��� ��Ŷ�� �����.
*/
PVOID AgsmParty::MakePacketDelegationLeaderFailed(INT32 lOperator, INT16 *pnPacketLength)
{
	INT8	cOperation = AGSMPARTY_PACKET_OPERATION_DELEGATION_LEADER_FAILED;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												NULL);

	return pvPacket;
}

PVOID AgsmParty::MakePacketRequestRecall(INT32 lOperator, INT16 *pnPacketLength)
{
	if (lOperator == AP_INVALID_CID || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGSMPARTY_PACKET_OPERATION_REQUEST_RECALL;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												NULL,
												NULL);
}

PVOID AgsmParty::MakePacketAcceptRecall(INT32 lOperator, INT32 lTargetID, INT16 *pnPacketLength)
{
	if (lOperator == AP_INVALID_CID || lTargetID == AP_INVALID_CID || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGSMPARTY_PACKET_OPERATION_ACCEPT_RECALL;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTargetID,
												NULL);
}

PVOID AgsmParty::MakePacketRejectRecall(INT32 lOperator, INT32 lTargetID, INT16 *pnPacketLength)
{
	if (lOperator == AP_INVALID_CID || lTargetID == AP_INVALID_CID || !pnPacketLength)
		return NULL;

	INT8	cOperation	= AGSMPARTY_PACKET_OPERATION_REJECT_RECALL;

	return	m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMPARTY_PACKET_TYPE,
												&cOperation,
												&lOperator,
												&lTargetID,
												NULL);
}

BOOL AgsmParty::InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize)
{
	return m_csGenerateID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag, lRemoveIDQueueSize, TRUE);
}

/*
AgsdParty* AgsmParty::GetADParty(AgpdParty *pcsParty)
{
	if (pcsParty)
		return (AgsdParty *) m_pagpmParty->GetAttachedModuleData(m_nIndexADParty, (PVOID) pcsParty);

	return NULL;
}
*/

BOOL AgsmParty::CBCreateParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmParty *pThis	= (AgsmParty *) pClass;
	AgpdParty *pcsParty	= (AgpdParty *) pData;

	pThis->m_pagpmParty->EnumCallback(AGPMPARTY_CB_ADD_MEMBER, pcsParty, pThis->m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[0]));

	/*
	AgsdParty *pcsAgsdParty = pThis->GetADParty(pcsParty);

	pcsAgsdParty->m_dpnidPartyServer = pThis->m_pagsmAOIFilter->CreateGroup(AGPMPARTY_MAX_PARTY_MEMBER);

	// ��Ƽ �׷��� ����� �׷�ȿ� ���� ��Ƽ������ ��Ͻ�Ų��.
	pcsAgsdParty->m_dpnidParty = pThis->m_pagsmAOIFilter->CreateGroup(AGPMPARTY_MAX_PARTY_MEMBER);
	*/
/*
	pThis->SendPartyInfo(pcsParty);

	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if (!pcsParty->m_pcsMemberList[i])
			return FALSE;//continue;

		AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsParty->m_pcsMemberList[i]);
		if (!pcsAgsdCharacter)
			return FALSE;//continue;

		//pThis->m_pagsmAOIFilter->AddPlayerToGroup(pcsAgsdParty->m_dpnidParty, pcsAgsdCharacter->m_dpnidCharacter);

		AgpdPartyADChar	*pcsAttachData	= pThis->m_pagpmParty->GetADCharacter(pcsParty->m_pcsMemberList[i]);

		pThis->m_pagpmParty->CalcPartyFactor(pcsParty->m_pcsMemberList[0], pcsParty->m_pcsMemberList[i], &pcsAttachData->m_csFactorPoint);

#ifdef	AGSMPARTY_UPDATE_PARTY_FACTOR
		if (pcsParty->m_bIsInEffectArea[i])
			pThis->m_pagsmCharacter->ReCalcCharacterFactors(pcsParty->m_pcsMemberList[i], TRUE);
#endif	//AGSMPARTY_UPDATE_PARTY_FACTOR
	}

	for (i = 0; i < pcsParty->m_nCurrentMember - 1; ++i)
	{
		pThis->SyncMemberHPMax(pcsParty, pcsParty->m_pcsMemberList[i], TRUE);
	}

*/
	return TRUE;
}

BOOL AgsmParty::CBRemoveParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmParty *pThis	= (AgsmParty *) pClass;
	AgpdParty *pcsParty	= (AgpdParty *) pData;

#ifdef	AGSMPARTY_UPDATE_PARTY_FACTOR
	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsCharacter	= pThis->m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (pcsCharacter)
		{
//	��Ƽ �߰��ɷ�ġ ���� By SungHoon 2005.06.28
			pThis->RecalcPartyMemberHP(pcsParty, pcsCharacter, FALSE);
			pThis->m_pagsmSkill->RemoveSaveSkill(pcsCharacter, AGPMSKILL_RANGE2_TARGET_PARTY);
			pcsCharacter->m_Mutex.Release();
//			pThis->m_pagpmFactors->DestroyFactor(&pcsAttachData->m_csFactorPoint);

//			pThis->m_pagsmCharacter->ReCalcCharacterFactors(pcsParty->m_pcsMemberList[i], TRUE);
		}
	}
#endif	//AGSMPARTY_UPDATE_PARTY_FACTOR

	//AgsdParty *pcsAgsdParty = pThis->GetADParty(pcsParty);

	// ��Ƽ�� ���� �Ǿ��ٴ� ������ ���� ��Ƽ���鿡�� ������.
	if (pThis->SendPartyRemove(pcsParty))
		return FALSE;

	/*
	// ��Ƽ DPNID Group�� �����Ѵ�.
	pThis->m_pagsmAOIFilter->DestroyGroup(pcsAgsdParty->m_dpnidParty);

	pThis->m_pagsmAOIFilter->DestroyGroup(pcsAgsdParty->m_dpnidPartyServer);
	*/

	pThis->RemoveFarMemberView(pcsParty);		//	��ü ��Ƽ�� �˻��ؼ� ���� �ָ� ���� ��� �þ߿��� ����.  By SungHoon

	// Log ����. Remove Party �� �� ��� ���鼭 üũ���ش�.
	for(INT32 j = 0; j < pcsParty->m_nCurrentMember; j++)
	{
		AgpdCharacter	*pcsCharacter	= pThis->m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[j]);
		if (pcsCharacter)
		{
			pThis->m_pagsmCharacter->StopPartyPlay(pcsCharacter);
			pThis->m_pagsmSkill->RemoveSaveSkill(pcsCharacter, AGPMSKILL_RANGE2_TARGET_PARTY);
			pcsCharacter->m_Mutex.Release();
		}
	}

	return TRUE;
}

BOOL AgsmParty::CBRemovePartyID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdParty		*pcsParty		= (AgpdParty *)		pData;

	pThis->m_csGenerateID.AddRemoveID(pcsParty->m_lID);

	return TRUE;
}

BOOL AgsmParty::CBAddMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmParty *pThis				= (AgsmParty *) pClass;
	AgpdParty *pcsParty				= (AgpdParty *) pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pCustData;
	AgsdCharacter *pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsParty->m_nCurrentMember < 2) return FALSE;

	// �߰��� ������� �� ��Ƽ ������ ������.
	pThis->SendPartyMemberInfo(pcsParty, pcsAgsdCharacter->m_dpnidCharacter, pcsCharacter->m_lID);
	if (!pThis->SendPartyInfo(pcsParty, pcsAgsdCharacter->m_dpnidCharacter))
		return FALSE;

	// �߰��� ����� ���� ������ ������.
	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		if (pcsCharacter != pcsMember)
			pThis->m_pagsmCharacter->SendPacketCharView(pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsMember), FALSE);
		pThis->SendMemberAdd(pcsParty, pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsMember));
	}

	pThis->RecalcPartyMemberHP(pcsParty, pcsCharacter, TRUE);

	pThis->SyncMemberHPMax(pcsParty, pcsCharacter, TRUE);

	// ���� �߰��� ��� DPNID�� ��Ƽ �׷쿡 �߰���Ų��.
	//pThis->m_pagsmAOIFilter->AddPlayerToGroup(pcsAgsdParty->m_dpnidParty, pcsAgsdCharacter->m_dpnidCharacter);

	AgsdServer *pcsThisServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	// �� ĳ���Ϳ��� ��Ʈ���ϴ� ĳ���Ͷ�� �ٸ� �������� ����� �߰��� ����� �˷��ش�.
	/*
	if (pcsAgsdCharacter->m_ulServerID == pcsThisServer->m_lServerID)
		if (!pThis->SendMemberAdd(pcsParty, pcsCharacter, pcsAgsdParty->m_dpnidPartyServer, TRUE))
			return FALSE;
	*/

	pThis->m_pagsmCharacter->StartPartyPlay(pcsCharacter);

	return TRUE;
}

BOOL AgsmParty::CBRemoveMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmParty *pThis				= (AgsmParty *) pClass;
	AgpdParty *pcsParty				= (AgpdParty *) pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pCustData;

//	��Ƽ �߰��ɷ�ġ ���� By SungHoon 2005.06.28
	pThis->RecalcPartyMemberHP(pcsParty, pcsCharacter, FALSE);
	pThis->m_pagsmSkill->RemoveSaveSkill(pcsCharacter, AGPMSKILL_RANGE2_TARGET_PARTY);

//	AgsdParty *pcsAgsdParty			= pThis->GetADParty(pcsParty);
	//AgsdCharacter *pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	// ��Ƽ���� ���� ����� ���� ������ ������.
	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		pThis->SendMemberRemove(pcsParty, pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]));
	}

	pThis->SendMemberRemove(pcsParty, pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// ���� ��� DPNID�� ��Ƽ �׷쿡�� ����.
	//pThis->m_pagsmAOIFilter->RemovePlayerFromGroup(pcsAgsdParty->m_dpnidParty, pcsAgsdCharacter->m_dpnidCharacter);

	/*
	AgsdServer *pcsThisServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	// �� ĳ���Ϳ��� ��Ʈ���ϴ� ĳ���Ͷ�� �ٸ� �������� ��Ƽ���� ���� ����� �˷��ش�.
	if (pcsAgsdCharacter->m_ulServerID == pcsThisServer->m_lServerID)
		if (!pThis->SendMemberRemove(pcsParty, pcsCharacter, pcsAgsdParty->m_dpnidPartyServer, TRUE))
			return FALSE;
	*/

	pThis->m_pagsmCharacter->StopPartyPlay(pcsCharacter);

	pThis->RemoveFarMemberView(pcsParty, pcsCharacter->m_lID);		//	��Ƽ���� ���� ����� �ָ� ���� ��� ó�� By SungHoon
	return TRUE;
}

BOOL AgsmParty::CBChangeLeader(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmParty *pThis	= (AgsmParty *) pClass;
	AgpdParty *pcsParty	= (AgpdParty *) pData;

//	AgsdParty *pcsAgsdParty = pThis->GetADParty(pcsParty);

	AgsdServer *pcsThisServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	// �ϴ� ��Ƽ������ factor�� ������Ʈ�Ѵ�. (�� �������� �����ϴ� �Ѹ�..)
	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		if (pcsParty->m_lMemberListID[i] == AP_INVALID_CID)
			continue;

		AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		AgsdCharacter	*pcsAgsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
		if (!pcsAgsdMember || pcsAgsdMember->m_ulServerID != pcsThisServer->m_lServerID)
		{
			pcsMember->m_Mutex.Release();
			continue;
		}

//	2005.06.28 By SungHoon ��Ƽ ���� �ٽ� ���
		pThis->RecalcPartyMemberHP(pcsParty,pcsMember, TRUE);

		pcsMember->m_Mutex.Release();
	}

	// ���ο� ������ �̼������� �����ϴ� ���̸� �ٸ� ������� ����� ��Ƽ ���͸� �����ش�.
//	if (pcsParty->m_pcsMemberList[0])
//	{
//		AgsdCharacter *pcsAgsdLeader = pThis->m_pagsmCharacter->GetADCharacter(pcsParty->m_pcsMemberList[0]);
//		if (pcsAgsdLeader)
//		{
//			if (pcsAgsdLeader->m_ulServerID == pcsThisServer->m_lServerID)
//			{
//				PVOID pvPacketFactor = pThis->m_pagsmFactors->MakePacketSyncParty(&pcsParty->m_csFactorPoint);
//				if (pvPacketFactor)
//				{
//					pThis->m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsParty->m_pcsMemberList[0], pcsAgsdParty->m_dpnidPartyServer, TRUE);
//
//					pThis->m_csPacket.FreePacket(pvPacketFactor);
//				}
//			}
//		}
//	}

	return TRUE;
}

BOOL AgsmParty::CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdPartyADChar	*pcsPartyADChar = pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	// ��Ƽ�� ����ִ� ������ �˻��Ѵ�.
	if (pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return FALSE;

	// ��Ƽ ����Ÿ�� ���� �ȵǾ� �ִٸ� �����ϰ�...
	if (!pcsPartyADChar->pcsParty)
		pcsPartyADChar->pcsParty = pThis->m_pagpmParty->GetParty(pcsPartyADChar->lPID);

	if (!pcsPartyADChar->pcsParty)
		return FALSE;

//	AgsdParty *pcsAgsdParty = pThis->GetADParty(pcsPartyADChar->pcsParty);

	/*
	// ���� �������� �ִ� ��Ʈ���鿡�� �����ش�.
	pThis->m_pagsmCharacter->SendPacketCharStatus(pcsCharacter, pcsAgsdParty->m_dpnidParty);
	*/

	/*
	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	AgsdServer *pcsThisServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (pcsThisServer)
	{
		// �̼����� ��Ʈ���ϴ� ������ ���캻��.
		// �̼����� ��Ʈ���ϴ� ���̸� �ٸ� �����鿡 �ִ� ��Ƽ���鿡�� ��ũ�� ���������� �����ϰ�
		// ��Ʈ�� �ϴ� ���� �ƴ϶�� �� �Ѿ��.
		if (pcsAgsdCharacter->m_ulServerID == pcsThisServer->m_lServerID)
		{
			// ��ũ �����ش�. ��, ��Ƽ���� �ִ� �ٸ� �����鿡�� �˷��ش�.
			pThis->m_pagsmCharacter->SendPacketCharStatus(pcsCharacter, pcsAgsdParty->m_dpnidPartyServer, TRUE);
		}
	}
	*/

	return TRUE;
}

/*
BOOL AgsmParty::CBMoveSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	ApWorldSector	*pcsSector		= (ApWorldSector *)	pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

	AgpdPartyADChar	*pcsPartyADChar = pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	// ��Ƽ�� ����ִ� ������ �˻��Ѵ�.
	if (pcsPartyADChar && pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return TRUE;

	if (pcsPartyADChar && !pcsPartyADChar->pcsParty)
		pcsPartyADChar->pcsParty = pThis->m_pagpmParty->GetParty(pcsPartyADChar->lPID);

	if (pcsPartyADChar && pcsPartyADChar->pcsParty == NULL)
		return FALSE;

	AgsdParty	*pcsAgsdParty	= pThis->GetADParty(pcsPartyADChar->pcsParty);
	if (!pcsAgsdParty)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	AgsdServer *pcsThisServer = pThis->m_pagsmServerManager->GetThisServer();
	if (pcsThisServer)
	{
		// �̼����� ��Ʈ���ϴ� ������ ���캻��.
		// �̼����� ��Ʈ���ϴ� ���̸� �ٸ� �����鿡 �ִ� ��Ƽ���鿡�� ��ũ�� ���������� �����ϰ�
		// ��Ʈ�� �ϴ� ���� �ƴ϶�� �� �Ѿ��.
		if (pcsAgsdCharacter->m_ulServerID == pcsThisServer->m_lServerID)
		{
			// ��ũ �����ش�. ��, ��Ƽ���� �ִ� �ٸ� �����鿡�� �˷��ش�.
			pThis->m_pagsmCharacter->SendPacketCharPosition(pcsCharacter, pcsAgsdParty->m_dpnidPartyServer, TRUE);
		}
	}

	return TRUE;
}
*/

BOOL AgsmParty::CBMoveCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	/*
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	ApWorldSector	*pcsSector		= (ApWorldSector *)	pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

	AgpdPartyADChar	*pcsPartyADChar = pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	// ��Ƽ�� ����ִ� ������ �˻��Ѵ�.
	if (pcsPartyADChar && pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return TRUE;

	if (pcsPartyADChar && !pcsPartyADChar->pcsParty)
		pcsPartyADChar->pcsParty = pThis->m_pagpmParty->GetParty(pcsPartyADChar->lPID);

	if (pcsPartyADChar && pcsPartyADChar->pcsParty == NULL)
		return FALSE;

	AgsdParty	*pcsAgsdParty	= pThis->GetADParty(pcsPartyADChar->pcsParty);
	if (!pcsAgsdParty)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	AgsdServer *pcsThisServer = pThis->m_pagsmServerManager->GetThisServer();
	if (pcsThisServer)
	{
		// �̼����� ��Ʈ���ϴ� ������ ���캻��.
		// �̼����� ��Ʈ���ϴ� ���̸� �ٸ� �����鿡 �ִ� ��Ƽ���鿡�� ��ũ�� ���������� �����ϰ�
		// ��Ʈ�� �ϴ� ���� �ƴ϶�� �� �Ѿ��.
		if (pcsAgsdCharacter->m_ulServerID == pcsThisServer->m_lServerID)
		{
			// ��ũ �����ش�. ��, ��Ƽ���� �ִ� �ٸ� �����鿡�� �˷��ش�.
			pThis->m_pagsmCharacter->SendPacketCharPosition(pcsCharacter, pcsAgsdParty->m_dpnidPartyServer, TRUE);
		}
	}
	*/

	return TRUE;
}

BOOL AgsmParty::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty*		pThis			= (AgsmParty*)pClass;
	AgpdCharacter*	pcsCharacter	= (AgpdCharacter*)pData;
	INT16 nPrevRegionIndex			= *(INT16*)pCustData;	// NULL �� �ö��� ADD_CHARACTER_TO_MAP ���� �Ҹ�����.
	
	AgpdParty	*pcsParty	= pThis->m_pagpmParty->GetParty(pcsCharacter);
	if (!pcsParty)
		return FALSE;
	
	BOOL bPrevRegion = pThis->m_pagpmBattleGround->IsBattleGround(nPrevRegionIndex);
	BOOL bCurrRegion = pThis->m_pagpmBattleGround->IsBattleGround(pcsCharacter->m_nBindingRegionIndex);

	if( (bPrevRegion && !bCurrRegion) || (!bPrevRegion && bCurrRegion))
		return pThis->ProcessRemoveMember(pcsCharacter->m_lID);

	bPrevRegion = pThis->m_pagpmEpicZone->IsEpicZone(nPrevRegionIndex);
	bCurrRegion = pThis->m_pagpmEpicZone->IsEpicZone(pcsCharacter->m_nBindingRegionIndex);

	if((bPrevRegion && !bCurrRegion) || (!bPrevRegion && bCurrRegion))
		return pThis->ProcessRemoveMember(pcsCharacter->m_lID);

	// Check RegionPeculiarity
	if(pThis->m_papmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_PARTY) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
	{
		return pThis->ProcessRemoveMember(pcsCharacter->m_lID);
	}

	return FALSE;
}

BOOL AgsmParty::CBSyncCharacterActions(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*pvBuffer		= (PVOID *)			pCustData;

	if (pcsCharacter->m_bSync)
	{
		AgpdParty	*pcsParty	= pThis->m_pagpmParty->GetParty(pcsCharacter);
		if (!pcsParty)
			return FALSE;

		return pThis->SendPacketToPartyExceptOne(pcsParty, pvBuffer[0], (INT16) pvBuffer[1], pcsCharacter->m_lID, PACKET_PRIORITY_4);
	}
	else
	{
		return pThis->SendPacketToFarPartyMember(pcsCharacter, pvBuffer[0], (INT16) pvBuffer[1], PACKET_PRIORITY_4);
	}

	return TRUE;
}

BOOL AgsmParty::CBSyncCharacterRemove(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdParty		*pcsParty		= (AgpdParty *)		pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

//	AgsdParty		*pcsAgsdParty	= pThis->GetADParty(pcsParty);

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
		pThis->m_pagsmCharacter->SendPacketCharRemove(pcsCharacter->m_lID, pThis->m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), FALSE);

	return TRUE;

	/*
	INT16			nPacketLength	= 0;
	PVOID			pvPacket		= pThis->m_pagsmCharacter->MakePacketCharRemove(pcsCharacter->m_lID, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL			bSendResult		= pThis->SendPacketToFarPartyMember(pcsCharacter, pvPacket, nPacketLength);

	pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return			bSendResult;
	*/
}

BOOL AgsmParty::CBUpdateFactorParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdFactor		*pcsFactor		= (AgpdFactor *)	pData;

	AgpdFactorOwner	*pcsFactorOwner	= (AgpdFactorOwner *) pThis->m_pagpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_OWNER);
	if (!pcsFactorOwner || pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_ID] == AP_INVALID_CID || !pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER])
		return TRUE;

	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pcsFactorOwner->lValue[AGPD_FACTORS_OWNER_TYPE_OWNER];

	if(!pThis->m_pagpmCharacter->GetCharacter(pcsCharacter->m_szID))
		return FALSE;

	AgpdPartyADChar	*pcsPartyADChar = pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	if(!pcsPartyADChar)
		return FALSE;

	// ��Ƽ�� ����ִ� ������ �˻��Ѵ�.
	if (pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return FALSE;

	// ��Ƽ ����Ÿ�� ���� �ȵǾ� �ִٸ� �����ϰ�...
	if (!pcsPartyADChar->pcsParty)
		pcsPartyADChar->pcsParty = pThis->m_pagpmParty->GetParty(pcsPartyADChar->lPID);

	if (!pcsPartyADChar->pcsParty)
		return FALSE;

	pThis->SyncMemberHP(pcsPartyADChar->pcsParty, pcsCharacter, FALSE);

	/*
	AgsdParty *pcsAgsdParty = pThis->GetADParty(pcsPartyADChar->pcsParty);

	// ��Ƽ���� ��ũ�� ���� ��Ŷ ����Ÿ�� ���´�.
	PVOID pvPacketFactor = pThis->m_pagsmFactors->MakePacketSyncParty(&pcsCharacter->m_csFactor);

	if (pvPacketFactor)
	{
		// ��Ƽ���鿡�� ������.
		pThis->m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, pcsAgsdParty->m_dpnidParty);

		AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

		AgsdServer *pcsThisServer = pThis->m_pagsmServerManager->GetThisServer();
		if (pcsThisServer)
		{
			// �̼����� ��Ʈ���ϴ� ������ ���캻��.
			// �̼����� ��Ʈ���ϴ� ���̸� �ٸ� �����鿡 �ִ� ��Ƽ���鿡�� ��ũ�� ���������� �����ϰ�
			// ��Ʈ�� �ϴ� ���� �ƴ϶�� �� �Ѿ��.
			if (pcsAgsdCharacter->m_ulServerID == pcsThisServer->m_lServerID)
			{
				// ��ũ �����ش�. ��, ��Ƽ���� �ִ� �ٸ� �����鿡�� �˷��ش�.
				pThis->m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, pcsAgsdParty->m_dpnidPartyServer, TRUE);
			}
		}

		pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);
	}
	*/

	return TRUE;
}

BOOL AgsmParty::CBZoningPassControl(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	// �����ϴ°�� ��Ƽ ������ �����ϴ� ������ �����ʿ䰡 �ִ°� �˻�
	// �ʿ信 ���� ��Ƽ ������ ������. (��Ƽ ���� �׷쿡 �߰��Ѵ�.)

	AgsmParty			*pThis			= (AgsmParty *)				pClass;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)			pData;
	AgsdServer			*pcsServer		= (AgsdServer *)			pCustData;

	AgsdCharacter		*pcsAgsdServer	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	AgpdPartyADChar	*pcsPartyADChar = pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	if (pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return FALSE;			// ��Ƽ���� �ƴϴ�. ���ԵǾ� �ִ� ��Ƽ�� ����.

	if (!pcsPartyADChar->pcsParty)
		pcsPartyADChar->pcsParty = pThis->m_pagpmParty->GetParty(pcsPartyADChar->lPID);

	if (!pcsPartyADChar->pcsParty)
		return FALSE;

	/*
	AgpdParty	*pcsParty = pcsPartyADChar->pcsParty;

	// pcsServer�� ��Ƽ ����� ����̳� �ִ��� �����´�.
	INT16	nZoneServerMember = pThis->GetPartyMemberInServer(pcsParty, pcsServer->m_lServerID);
	if (nZoneServerMember < 0)
		return FALSE;

	if (nZoneServerMember == 0)		// �� ����� ó������ pcsServer�� �����ߴ�. ���... ��Ƽ ����Ÿ�� �����ش�.
	{
//		AgsdParty	*pcsAgsdParty = pThis->GetADParty(pcsParty);

//		pThis->m_pagsmAOIFilter->AddPlayerToGroup(pcsAgsdParty->m_dpnidPartyServer, pcsServer->m_dpnidServer);

		if (!pThis->SendPartyInfo(pcsParty, pcsServer->m_dpnidServer, FALSE))
			return FALSE;
	}
	*/

	return TRUE;
}

BOOL AgsmParty::CBUpdateControlServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty		*pThis					= (AgsmParty *)		pClass;
	AgpdCharacter	*pcsCharacter			= (AgpdCharacter *)	pData;
	INT32			*plPrevControlServer	= (INT32 *)			pCustData;

	AgpdPartyADChar	*pcsAgpdPartyADChar		= pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	// ��Ƽ�� ������ ������� �˻��Ѵ�.
	if (pcsAgpdPartyADChar->lPID == AP_INVALID_PARTYID)
		return TRUE;

	if (!pcsAgpdPartyADChar->pcsParty)
		pcsAgpdPartyADChar->pcsParty = pThis->m_pagpmParty->GetParty(pcsAgpdPartyADChar->lPID);

	if (!pcsAgpdPartyADChar->pcsParty)
		return FALSE;

	AgpdParty	*pcsParty = pcsAgpdPartyADChar->pcsParty;

	AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	AgsdServer	*pcsServer = pThis->m_pAgsmServerManager->GetServer(pcsAgsdCharacter->m_ulServerID);
	if (!pcsServer)
		return FALSE;

	/*
	// ���� ������ �� ������ ���� �׷��� ������ �Ѵ�.
	INT16	nNumMember = pThis->GetPartyMemberInServer(pcsParty, pcsAgsdCharacter->m_ulServerID);
	if (nNumMember < 0)
		return FALSE;

	if (nNumMember == 1)	// �� ����� ó������ pcsServer�� �����ߴ�. ���... ��Ƽ ���� �׷����� �Ѵ�.
	{
		AgsdParty	*pcsAgsdParty = pThis->GetADParty(pcsParty);

		pThis->m_pagsmAOIFilter->AddPlayerToGroup(pcsAgsdParty->m_dpnidPartyServer, pcsServer->m_dpnidServer);
	}

	// ���� ��Ʈ�� ������ ���� �׷��� ������ �Ѵ�.
	nNumMember = pThis->GetPartyMemberInServer(pcsParty, *plPrevControlServer);
	if (nNumMember < 0)
		return FALSE;

	if (nNumMember == 0)	// �� ��� ����� ���̻� �����ִ� ����� ����. ���.. �׷쿡�� ����.
	{
		AgsdParty	*pcsAgsdParty = pThis->GetADParty(pcsParty);

		pThis->m_pagsmAOIFilter->RemovePlayerFromGroup(pcsAgsdParty->m_dpnidPartyServer, pcsServer->m_dpnidServer);
	}
	*/

	return TRUE;
}

BOOL AgsmParty::CBReCalcFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdParty		*pcsParty		= pThis->m_pagpmParty->GetParty(pcsCharacter);
	if (pcsParty)
	{
		int i = 0;
		for (i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER; ++i)
		{
			if (pcsParty->m_lMemberListID[i] == pcsCharacter->m_lID)
				break;
		}

		if (i == AGPMPARTY_MAX_PARTY_MEMBER)
			return TRUE;

		if (pcsParty->m_bIsInEffectArea[i])
		{
			AgpdPartyADChar	*pcsAttachData	= pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

			pThis->m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, &pcsAttachData->m_csFactorPoint, TRUE, FALSE, TRUE, FALSE);
		}
	}

	return TRUE;
}

BOOL AgsmParty::CBResetEffectFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdParty		*pcsParty		= (AgpdParty *)		pData;
	BOOL			*pbOldList		= (BOOL *)			pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pagpmParty->MakePacketEffectArea(pcsParty, pbOldList, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	//AgsdParty		*pcsAgsdParty	= pThis->GetADParty(pcsParty);

	BOOL	bSendResult	= TRUE;
	
	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
		bSendResult &= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), PACKET_PRIORITY_4);

	pThis->m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmParty::CBUpdateExpType(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdParty		*pcsParty		= (AgpdParty *)		pData;
	INT32			*pMemberID		= (INT32	 *)		pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pagpmParty->MakePacketExpDivision(pcsParty, *pMemberID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	//AgsdParty		*pcsAgsdParty	= pThis->GetADParty(pcsParty);

	BOOL	bSendResult	= TRUE;
	
	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
		bSendResult &= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), PACKET_PRIORITY_4);

	pThis->m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmParty::CBUpdatePartyFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	/*
	if (!ppvBuffer[0] ||
		!ppvBuffer[1] ||
		!ppvBuffer[2] ||
		!ppvBuffer[3] ||
		!ppvBuffer[4] ||
		!ppvBuffer[5] ||
		!ppvBuffer[6] ||
		!ppvBuffer[7])
		return FALSE;
	*/

	INT32	lOriginalDamage		= PtrToInt(ppvBuffer[0]);
	INT32	lOriginalDefense	= PtrToInt(ppvBuffer[1]);
	INT32	lOriginalMaxHP		= PtrToInt(ppvBuffer[2]);
	INT32	lOriginalMaxMP		= PtrToInt(ppvBuffer[3]);

	INT32	lNewDamage			= PtrToInt(ppvBuffer[4]);
	INT32	lNewDefense			= PtrToInt(ppvBuffer[5]);
	INT32	lNewMaxHP			= PtrToInt(ppvBuffer[6]);
	INT32	lNewMaxMP			= PtrToInt(ppvBuffer[7]);

/*	if (lNewDamage > 0 && lNewDamage != lOriginalDamage ||
		lNewDefense > 0 && lNewDefense != lOriginalDefense ||
		lNewMaxHP > 0 && lNewMaxHP != lOriginalMaxHP ||
		lNewMaxMP > 0 && lNewMaxMP != lOriginalMaxMP)
	{
		// ��Ŷ�� ������ �Ѵ�.
		return pThis->SendPartyBonusStats(pcsCharacter);
	}

	return TRUE;
*/
	return pThis->SendPartyBonusStats(pcsCharacter);
}

BOOL AgsmParty::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdPartyADChar	*pcsPartyADChar = pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	if (pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	if (!pcsPartyADChar->pcsParty)
		pcsPartyADChar->pcsParty = pThis->m_pagpmParty->GetParty(pcsPartyADChar->lPID);

	if (!pcsPartyADChar->pcsParty)
		return FALSE;

	//AgsdParty		*pcsAgsdParty	= pThis->GetADParty(pcsPartyADChar->pcsParty);

	for (int i = 0; i < pcsPartyADChar->pcsParty->m_nCurrentMember; ++i)
		pThis->m_pagsmCharacter->SendPacketCharRemove(pcsCharacter, pThis->m_pagsmCharacter->GetCharDPNID(pcsPartyADChar->pcsParty->m_lMemberListID[i]), FALSE);

	return TRUE;
}

/*
	2005.04.19	By SungHoon
	������ �й� ����� ���� ��û�� ��Ƽ���� �䱸���� ��� ó���Ѵ�.
*/
BOOL AgsmParty::CBUpdateItemDivision(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdParty		*pcsParty		= (AgpdParty *)		pData;
	INT32			*pMemberID		= (INT32	 *)		pCustData;

	if (pcsParty == NULL) return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pagpmParty->MakePacketItemDivision(pcsParty, *pMemberID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	//AgsdParty		*pcsAgsdParty	= pThis->GetADParty(pcsParty);

	BOOL	bSendResult	= TRUE;

	pThis->SendPacketToParty(pcsParty, pvPacket, nPacketLength, PACKET_PRIORITY_4);

	pThis->m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

/*
	2005.05.02	By SungHoon
	��Ƽ�� ������ ��û���� ��� �Ҹ�������.
*/
BOOL AgsmParty::DelegationLeader(UINT32 ulNID, INT32 lOperator, INT32 lTarget )
{
	AgpdCharacter *pcsOperator = m_pagpmCharacter->GetCharacterLock(lOperator);
	if(!pcsOperator)
		return FALSE;

	AgpdParty *pcsParty = m_pagpmParty->GetPartyLock(pcsOperator);
	if (!pcsParty)
	{
		pcsOperator->m_Mutex.Release();
		return FALSE;
	}

	if (lOperator != m_pagpmParty->GetLeaderCID(pcsParty))
	{
		SendDelegationLeaderFailed(lOperator, ulNID);
		pcsOperator->m_Mutex.Release();
		pcsParty->m_Mutex.Release();
		return FALSE;
	}
	pcsOperator->m_Mutex.Release();
	if (m_pagpmParty->DelegationLeader(pcsParty, lOperator, lTarget) == TRUE)
	{
		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= MakePacketDelegationLeader(lOperator, lTarget, &nPacketLength);
		if (!pvPacket || nPacketLength < 1)
		{
			pcsParty->m_Mutex.Release();
			return FALSE;
		}
		SendPacketToParty( pcsParty, pvPacket, nPacketLength, PACKET_PRIORITY_4);
		m_pagpmParty->m_csPacket.FreePacket(pvPacket);
	}
	pcsParty->m_Mutex.Release();

	return TRUE;
}
//	Finish 2005.05.02	By Sunghoon

BOOL AgsmParty::SendPacketToParty(INT32 lPartyID, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority)
{
	return SendPacketToParty(m_pagpmParty->GetParty(lPartyID), pvPacket, nPacketLength, ePriority);
}

BOOL AgsmParty::SendPacketToParty(AgpdParty *pcsParty, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority)
{
	if (!pcsParty || !pvPacket || !nPacketLength)
		return FALSE;

	//AgsdParty	*pcsAgsdParty	= GetADParty(pcsParty);

	//if (pcsAgsdParty)
	{
		for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
			SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), ePriority);
		//m_pagsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, pcsAgsdParty->m_dpnidPartyServer, ePriority);
	}

	return TRUE;
}

BOOL AgsmParty::SendPacketToParty(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority)
{
	if (!pcsCharacter || !pvPacket || !nPacketLength)
		return FALSE;

	AgpdPartyADChar *pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsCharacter);

	if (pcsPartyADChar)
	{
		if (!pcsPartyADChar->pcsParty)
			pcsPartyADChar->pcsParty = m_pagpmParty->GetParty(pcsPartyADChar->lPID);

		return SendPacketToParty(pcsPartyADChar->pcsParty, pvPacket, nPacketLength, ePriority);
	}

	return FALSE;
}

BOOL AgsmParty::SendPacketToPartyExceptOne(AgpdParty *pcsParty, PVOID pvPacket, INT16 nPacketLength, INT32 lExceptMemberID, PACKET_PRIORITY ePriority)
{
	if (!pcsParty || !pvPacket || nPacketLength < 1 || lExceptMemberID == AP_INVALID_CID)
		return FALSE;

	BOOL	bSendResult	= TRUE;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		if (pcsParty->m_lMemberListID[i] == lExceptMemberID)
			continue;

		bSendResult	&= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), ePriority);
	}

	return bSendResult;
}

BOOL AgsmParty::SendPacketToNearPartyMember(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority)
{
	if (!pcsCharacter || !pvPacket || nPacketLength < 1)
		return FALSE;

	// ����� ���� ���� ���� �ִ� �ѵ����� ������ ����ۿ� ����. ��.�� �̰� �׷��� �ٽ� �� �����ؾ� �ϳ�... ��.��

	// ��Ƽ ���� ���θ� ������ ��Ƽ���� �������� �� �������� �������� �Ѵ�.

	AgpdParty	*pcsParty	= m_pagpmParty->GetParty(pcsCharacter);

	if (pcsParty)
	{
		//AgsdParty	*pcsAgsdParty	= GetADParty(pcsParty);

		for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
			SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), ePriority);

		return TRUE;
	}
	else
	{
		return SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter), ePriority);
	}

	return TRUE;
}

BOOL AgsmParty::SendPacketToFarPartyMember(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, PACKET_PRIORITY ePriority)
{
	if (!pcsCharacter || !pvPacket || nPacketLength < 1)
		return FALSE;

	ApSafeArray<INT32, AGPMPARTY_MAX_PARTY_MEMBER>		alMemberListBuffer;
	alMemberListBuffer.MemSetAll();

	AgpdParty	*pcsParty	= m_pagpmParty->GetPartyLock(pcsCharacter);
	if (!pcsParty)
		return FALSE;

	INT32	nCurrentMember	= pcsParty->m_nCurrentMember;
	alMemberListBuffer.MemCopy(0, &pcsParty->m_lMemberListID[0], pcsParty->m_nCurrentMember);

	pcsParty->m_Mutex.Release();

	for (int i = 0; i < nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pagpmCharacter->GetCharacterLock(alMemberListBuffer[i]);
		if (!pcsMember)
			continue;

		INT32	lDistance = CheckValidDistance(pcsMember, pcsCharacter);

		if (lDistance > 1)		// �Ÿ��� 2 �̻��̸� �þ߳��� ���°Ŵ�.
			SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsMember), ePriority);

		pcsMember->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmParty::SendPartyMemberInfo(AgpdParty *pcsParty, DPNID dpnid, INT32 lExceptMemberID)
{
	if (!pcsParty || dpnid == 0)
		return FALSE;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		if (pcsParty->m_lMemberListID[i] == lExceptMemberID)
			continue;

		m_pagsmCharacter->SendPacketCharView(m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]), dpnid, FALSE);
	}

	return TRUE;
}

BOOL AgsmParty::SendPartyInfo(AgpdParty *pcsParty, DPNID dpnid)
{
	if (!pcsParty)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmParty->MakePacketParty(pcsParty, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bRetval = TRUE;

	if (!SendPacket(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_2))
		bRetval = FALSE;

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return bRetval;
}

BOOL AgsmParty::SendPartyInfo(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmParty->MakePacketParty(pcsParty, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bRetval = TRUE;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		if (!SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), PACKET_PRIORITY_2))
			bRetval = FALSE;
	}

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return bRetval;
}

BOOL AgsmParty::SendPartyRemove(AgpdParty *pcsParty)
{
	if (!pcsParty)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmParty->MakePacketRemoveParty(pcsParty->m_lID, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bRetval = TRUE;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		if (!SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsParty->m_lMemberListID[i]), PACKET_PRIORITY_2))
			bRetval = FALSE;
	}

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return bRetval;
}

BOOL AgsmParty::SendMemberAdd(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, DPNID dpnid)
{
	if (!pcsParty)
		return FALSE;

	BOOL	bIsInEffectArea	= FALSE;

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		if (pcsParty->m_lMemberListID[i] == pcsCharacter->m_lID)
		{
			bIsInEffectArea	= pcsParty->m_bIsInEffectArea[i];
			break;
		}
	}

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmParty->MakePacketAddMember(pcsParty, pcsCharacter->m_lID, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bRetval = SendPacket(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_2);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return bRetval;
}

BOOL AgsmParty::SendMemberRemove(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, DPNID dpnid)
{
	if (!pcsParty)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pagpmParty->MakePacketRemoveMember(pcsParty->m_lID, pcsCharacter->m_lID, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	BOOL	bRetval = SendPacket(pvPacket, nPacketLength, dpnid, PACKET_PRIORITY_2);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return bRetval;
}

BOOL AgsmParty::SyncMemberHP(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, BOOL bIsMemberAdd)
{
	if (!pcsParty || !pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmParty->MakePacketSyncMemberHP(pcsCharacter, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacketToPartyExceptOne(pcsParty, pvPacket, nPacketLength, pcsCharacter->m_lID, PACKET_PRIORITY_4);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	if (bIsMemberAdd)
	{
		for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
		{
			if (pcsParty->m_lMemberListID[i] == pcsCharacter->m_lID)
				continue;

			pvPacket	= m_pagpmParty->MakePacketSyncMemberHP(m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]), &nPacketLength);
			if (!pvPacket || nPacketLength < 1)
				continue;

			SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

			m_pagpmParty->m_csPacket.FreePacket(pvPacket);
		}
	}

	return bSendResult;
}

BOOL AgsmParty::SyncMemberHPMax(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, BOOL bIsMemberAdd)
{
	if (!pcsParty || !pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmParty->MakePacketSyncMemberHPMax(pcsCharacter, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacketToPartyExceptOne(pcsParty, pvPacket, nPacketLength, pcsCharacter->m_lID, PACKET_PRIORITY_4);

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	if (bIsMemberAdd)
	{
		for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
		{
			if (pcsParty->m_lMemberListID[i] == pcsCharacter->m_lID)
				continue;

			pvPacket	= m_pagpmParty->MakePacketSyncMemberHPMax(m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]), &nPacketLength);
			if (!pvPacket || nPacketLength < 1)
				continue;

			SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

			m_pagpmParty->m_csPacket.FreePacket(pvPacket);
		}
	}

	return bSendResult;
}

BOOL AgsmParty::SendPartyBonusStats(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pagpmParty->MakePacketBonusStatus(pcsCharacter, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	m_pagpmParty->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

/*
BOOL AgsmParty::ConAgsdParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmParty *pThis = (AgsmParty *) pClass;

	AgsdParty *pcsAgsdParty = pThis->GetADParty((AgpdParty *) pData);

	pcsAgsdParty->m_dpnidParty			= ASDP_INVALID_DPNID;
	pcsAgsdParty->m_dpnidPartyServer	= ASDP_INVALID_DPNID;

	return TRUE;
}

BOOL AgsmParty::DesAgsdParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}
*/

INT16 AgsmParty::GetPartyMemberInServer(AgpdParty *pcsParty, INT32 lServerID)
{
	if (!pcsParty || lServerID == AP_INVALID_SERVERID)
		return (-1);

	INT16	nMember = 0;
	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		AgpdCharacter	*pcsMember = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		AgsdCharacter	*pcsAgsdMember = m_pagsmCharacter->GetADCharacter(pcsMember);
		if (!pcsAgsdMember)
			continue;

		if (pcsAgsdMember->m_ulServerID == lServerID)
			nMember++;
	}

	return nMember;
}

BOOL AgsmParty::CBAddParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmParty	*pThis		= (AgsmParty *) pClass;
	AgpdParty	*pcsParty	= (AgpdParty *) pData;

	// ��Ƽ�� ���� �߰��Ǿ���. (��Ŷ���� �޾Ҵ�.)
	// �� ��쿣 ���� ��Ƽ ������� �ִ� ������ ���� �׷����� �Ѵ�.

//	AgsdParty	*pcsAgsdParty	= pThis->GetADParty(pcsParty);
//	if (!pcsAgsdParty)
//		return FALSE;

	AgsdServer	*pcsThisServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		AgsdCharacter	*pcsAgsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
		if (!pcsAgsdMember)
			continue;

		if (pcsThisServer->m_lServerID == pcsAgsdMember->m_ulServerID)
			continue;

		AgsdServer		*pcsServer = pThis->m_pAgsmServerManager->GetServer(pcsAgsdMember->m_ulServerID);
		if (!pcsServer)
			continue;

		//pThis->m_pagsmAOIFilter->AddPlayerToGroup(pcsAgsdParty->m_dpnidPartyServer, pcsServer->m_dpnidServer);
	}

	return TRUE;
}

BOOL AgsmParty::CBUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmParty	*pThis		= (AgsmParty *) pClass;
	AgpdParty	*pcsParty	= (AgpdParty *)	pData;

	AgsdServer	*pcsThisServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	// ������Ʈ ���� ��Ŷ�� �޾Ҵ�. �̼������� �����ϴ� ����� ���͸� �ٽ� ������ش�.
	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		AgsdCharacter	*pcsAgsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
		if (!pcsAgsdMember)
		{
			pcsMember->m_Mutex.Release();
			continue;
		}

#ifdef	AGSMPARTY_UPDATE_PARTY_FACTOR
		if (pcsAgsdMember->m_ulServerID == pcsThisServer->m_lServerID)
			pThis->m_pagsmCharacter->ReCalcCharacterFactors(pcsMember);
#endif	//AGSMPARTY_UPDATE_PARTY_FACTOR

		pcsMember->m_Mutex.Release();
	}

	return TRUE;
}

// 2004.07,22, steeple
BOOL AgsmParty::CBCharLevelUp(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdParty* pcsParty = (AgpdParty*)pData;
	AgsmParty* pThis = (AgsmParty*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;
	
//	AgsdParty *pcsAgsdParty = pThis->GetADParty(pcsParty);

	AgsdServer *pcsThisServer = pThis->m_pAgsmServerManager->GetThisServer();
	if (!pcsThisServer)
		return FALSE;

	INT16 nPacketLength	=	0;
	PVOID pvPacket		=	pThis->m_pagpmCharacter->MakePacketCharLevel(pcsCharacter, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	pThis->SendPacketToPartyExceptOne(pcsParty, pvPacket, nPacketLength, pcsCharacter->m_lID, PACKET_PRIORITY_4);
	pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);


	// Level Up �� ����� �����Ͱ� �ƴϸ� factor ������Ʈ ����		2005.06.15 By SungHoon
	if(pcsParty->m_lMemberListID[0] != pcsCharacter->m_lID) return FALSE;	

	// �ϴ� ��Ƽ������ factor�� ������Ʈ�Ѵ�. (�� �������� �����ϴ� �Ѹ�..)
	for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
	{
		AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		AgsdCharacter	*pcsAgsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
		if (!pcsAgsdMember || pcsAgsdMember->m_ulServerID != pcsThisServer->m_lServerID)
		{
			pcsMember->m_Mutex.Release();
			continue;
		}

//	2005.06.28 By SungHoon ��Ƽ ���� �ٽ� ���
		pThis->RecalcPartyMemberHP(pcsParty, pcsMember, TRUE);

		pcsMember->m_Mutex.Release();
	}

//	// ���ο� ������ �̼������� �����ϴ� ���̸� �ٸ� ������� ����� ��Ƽ ���͸� �����ش�.
//	if (pcsParty->m_pcsMemberList[0])
//	{
//		AgsdCharacter *pcsAgsdLeader = pThis->m_pagsmCharacter->GetADCharacter(pcsParty->m_pcsMemberList[0]);
//		if (pcsAgsdLeader)
//		{
//			if (pcsAgsdLeader->m_ulServerID == pcsThisServer->m_lServerID)
//			{
//				PVOID pvPacketFactor = pThis->m_pagsmFactors->MakePacketSyncParty(&pcsParty->m_csFactorPoint);
//				if (pvPacketFactor)
//				{
//					pThis->m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsParty->m_pcsMemberList[0], pcsAgsdParty->m_dpnidPartyServer, TRUE);
//
//					pThis->m_csPacket.FreePacket(pvPacketFactor);
//				}
//			}
//		}
//	}

	return TRUE;
}

INT32 AgsmParty::CheckValidDistance(AgpdCharacter *pcsPartyMember, AgpdCharacter *pcsBaseCharacter)
{
	if (!pcsPartyMember || !pcsBaseCharacter)
		return 100;

	AgsdCharacter	*pcsAgsdPartyMember		= m_pagsmCharacter->GetADCharacter(pcsPartyMember);
	AgsdCharacter	*pcsAgsdBaseCharacter	= m_pagsmCharacter->GetADCharacter(pcsBaseCharacter);

	/*
	INT32	lSubX	= abs(pcsAgsdPartyMember->m_pCurrentSector->GetIndexX() - pcsAgsdBaseCharacter->m_pCurrentSector->GetIndexX());
	INT32	lSubZ	= abs(pcsAgsdPartyMember->m_pCurrentSector->GetIndexZ() - pcsAgsdBaseCharacter->m_pCurrentSector->GetIndexZ());
	*/

	if (!pcsAgsdPartyMember->m_pcsCurrentCell ||
		!pcsAgsdBaseCharacter->m_pcsCurrentCell)
		return 100;

	INT32	lSubX	= abs(pcsAgsdPartyMember->m_pcsCurrentCell->GetIndexX() - pcsAgsdBaseCharacter->m_pcsCurrentCell->GetIndexX());
	INT32	lSubZ	= abs(pcsAgsdPartyMember->m_pcsCurrentCell->GetIndexZ() - pcsAgsdBaseCharacter->m_pcsCurrentCell->GetIndexZ());

	if (lSubX > lSubZ)
		return lSubX;

	return lSubZ;
}

//INT32 AgsmParty::GetNearMember(AgpdParty *pcsParty, AgpdCharacter **ppcsMemberBuffer, INT32 *plMemberTotalLevel)
//{
//	if (!pcsParty)
//		return 0;
//
//	*plMemberTotalLevel		= 0;
//
//	INT32	lNumNearMember	= 0;
//
//	// �þ߳��� �ִ� ��Ƽ���� ��󳽴�.
//	// ������ġ�� pcsBase��.
//
//	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
//	{
//		if (!pcsParty->m_bIsInEffectArea[i])
//			continue;
//
//		// �þ߳��� �����ִ� ���̴�. ��.....
//
//		// Ȥ�� �������� �ƴ��� ����.
//		// �������� �ϰ͵� ���Ѵ�.
//
//		if (pcsParty->m_pcsMemberList[i]->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
//			continue;
//
//		if (ppcsMemberBuffer)
//		{
//			ppcsMemberBuffer[lNumNearMember] = pcsParty->m_pcsMemberList[i];
//		}
//
//		if (plMemberTotalLevel)
//		{
//			*plMemberTotalLevel += m_pagpmFactors->GetLevel(&pcsParty->m_pcsMemberList[i]->m_csFactor);
//		}
//
//		++lNumNearMember;
//	}
//
//	return lNumNearMember;
//}

INT32 AgsmParty::GetNearMember(AgpdParty *pcsParty, AgpdCharacter *pcsBase, AgpdCharacter **ppcsMemberBuffer, INT32 *plMemberTotalLevel)
{
	if (!pcsParty ||
		!pcsBase)
		return 0;

	*plMemberTotalLevel		= 0;

	INT32	lNumNearMember	= 0;

	// �þ߳��� �ִ� ��Ƽ���� ��󳽴�.
	// ������ġ�� pcsBase��.

	for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
		if (!pcsMember)
			continue;

		if (IsNearMember(pcsBase, pcsMember) == FALSE) continue;

		if (ppcsMemberBuffer)
		{
			ppcsMemberBuffer[lNumNearMember] = pcsMember;
		}

		if (plMemberTotalLevel)
		{
			*plMemberTotalLevel += m_pagpmCharacter->GetLevel( pcsMember );
		}

		++lNumNearMember;
	}

	return lNumNearMember;
}

INT32 AgsmParty::GetPartyBonusDamage(ApBase *pcsBase, AgpdSkill *pcsSkill)
{
	return 0;

//	if (!pcsBase || pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return 0;
//
//	pcsBase->m_Mutex.Release();
//
//	AgpdParty	*pcsParty	= m_pagpmParty->GetPartyLock((AgpdCharacter *) pcsBase);
//	if (!pcsParty)
//	{
//		pcsBase->m_Mutex.WLock();
//		return 0;
//	}
//
//	FLOAT	fLeaderCharisma	= 0.0f;
//	INT32	lLeaderClass	= 0;
//
//	AgpdCharacter	*pcsCharacter = pcsParty->m_pcsMemberList[0];
//	if (!pcsCharacter)
//	{
//		pcsParty->m_Mutex.Release();
//		pcsBase->m_Mutex.WLock();
//		return 0;
//	}
//
//	if (!m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLeaderClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS))
//	{
//		pcsParty->m_Mutex.Release();
//		pcsBase->m_Mutex.WLock();
//		return 0;
//	}
//
//	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
//	if (!pcsResultFactor)
//	{
//		pcsParty->m_Mutex.Release();
//		pcsBase->m_Mutex.WLock();
//		return 0;
//	}
//
//	if (!m_pagpmFactors->GetValue(pcsResultFactor, &fLeaderCharisma, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA))
//	{
//		pcsParty->m_Mutex.Release();
//		pcsBase->m_Mutex.WLock();
//		return 0;
//	}
//
//	pcsParty->m_Mutex.Release();
//
//	pcsBase->m_Mutex.WLock();
//
//
//	switch (lLeaderClass) {
//	case AUCHARCLASS_TYPE_KNIGHT:
//		{
//			if (pcsSkill)
//				return 0;
//
//			return (INT32) (fLeaderCharisma / 5.0f);
//		}
//		break;
//
//	case AUCHARCLASS_TYPE_MAGE/* ������ */:
//		{
//			if (!pcsSkill)
//				return 0;
//
//			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lCostType & AGPMSKILL_COST_MP)
//				return (INT32) (fLeaderCharisma / 10.0f);
//
//			return 0;
//		}
//		break;
//
//	case AUCHARCLASS_TYPE_RANGER/* �ü� */:
//		{
//			if (!pcsSkill)
//				return 0;
//
//			return  (INT32) (fLeaderCharisma / 10.0f);
//		}
//		break;
//
//		/*
//	case AUCHARCLASS_TYPE_MONK:
//		{
//			return lCharisma;
//		}
//		break;
//		*/
//
//	default:
//		return 0;
//		break;
//	}
//
//	return 0;
}

/*
	2005.04.21	By SungHoon
	���� ��Ƽ ���̰� ��Ƽ�� ������ ȹ���̶�� ������ �������� �̹����� ������� �ش�.
*/
INT32 AgsmParty::GetCurrentGetItemPartyMember( ApBase *pcsCharacter , ApBase *pcsDropbase, BOOL bTemp )
{
	if (!pcsCharacter) return AP_INVALID_CID;

	AgpdParty		*pcsAgpdParty	= NULL;
	AgpdCharacter	*pcsDropCharacter = NULL;

	if (pcsCharacter->m_eType == APBASE_TYPE_CHARACTER)		pcsAgpdParty = m_pagpmParty->GetParty(( AgpdCharacter *)pcsCharacter);
	if (pcsCharacter->m_eType == APBASE_TYPE_PARTY)			pcsAgpdParty = (AgpdParty *)pcsCharacter;
	if (pcsDropbase->m_eType == APBASE_TYPE_CHARACTER)		pcsDropCharacter = ( AgpdCharacter *)pcsDropbase;

	if (pcsAgpdParty == NULL || pcsDropCharacter == NULL) return AP_INVALID_CID;
	
	switch (pcsAgpdParty->m_eDivisionItem)
	{
	case AGPMPARTY_DIVISION_ITEM_SEQUENCE :
			{
				ApSafeArray<AgpdCharacter*, AGPMPARTY_MAX_PARTY_MEMBER>	pcsNearMember;
				pcsNearMember.MemSetAll();
				INT32 lMemberTotalLevel = 0;

				INT32 lNumNearMember = GetNearMember(pcsAgpdParty, pcsDropCharacter, &pcsNearMember[0], &lMemberTotalLevel);
				INT32	lGetItemCID	=	m_pagpmParty->GetNextGetItemMember( pcsAgpdParty, bTemp );

				for (int i = 0;i < lNumNearMember; i++)
				{
					if (lGetItemCID == pcsNearMember[i]->m_lID)
						return lGetItemCID;
				}
			}
			break;
	case AGPMPARTY_DIVISION_ITEM_DAMAGE :
			{
				AgsdCharacterHistoryEntry* pcsEntry = m_pagsmCharacter->GetMostDamagerInParty(pcsDropCharacter, pcsAgpdParty->m_lID);
				if(pcsEntry && pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
					return pcsEntry->m_csSource.m_lID;
			}
			break;
	}

	return AP_INVALID_CID;
}

BOOL AgsmParty::CBTransform(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmParty		*pThis			= (AgsmParty *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdPartyADChar	*pcsAttachData	= pThis->m_pagpmParty->GetADCharacter(pcsCharacter);

	if (pcsAttachData->pcsParty)
	{
		if (pcsAttachData->pcsParty->m_lMemberListID[0] == pcsCharacter->m_lID)
		{
			// if pcsCharacter is party leader, must be recalculate bonus factor of party leader
			//////////////////////////////////////////////////////////////////////////

			AgpdParty	*pcsParty	= pcsAttachData->pcsParty;

			for (int i = 0; i < pcsParty->m_nCurrentMember; i++)
			{
				AgpdCharacter	*pcsMember	= pThis->m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[i]);
				if (!pcsMember)
					continue;

				AgsdCharacter	*pcsAgsdMember = pThis->m_pagsmCharacter->GetADCharacter(pcsMember);
				if (!pcsAgsdMember)
				{
					pcsMember->m_Mutex.Release();
					continue;
				}

//	2005.06.28 By SungHoon ��Ƽ ���� �ٽ� ���
				pThis->RecalcPartyMemberHP(pcsParty, pcsMember, TRUE);

				pcsMember->m_Mutex.Release();
			}
		}
		else
		{
//	2005.06.28 By SungHoon ��Ƽ ���� �ٽ� ���
			AgpdParty	*pcsParty	= pcsAttachData->pcsParty;
			pThis->RecalcPartyMemberHP(pcsParty,pcsCharacter, TRUE);
		}
	}

	return TRUE;
}

BOOL AgsmParty::CBGetBonusOfPartyMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty* pThis = static_cast<AgsmParty*>(pClass);
	INT32* plRate = static_cast<INT32*>(pData);
	AgpdDropItemInfo2* pcsDropInfo = static_cast<AgpdDropItemInfo2*>(pCustData);

	INT32 lCID = pThis->GetCurrentGetItemPartyMember(pcsDropInfo->m_pcsFirstLooter, pcsDropInfo->m_pcsDropCharacter, TRUE);
	AgpdCharacter* pcsCharacter = pThis->m_pagpmCharacter->GetCharacter(lCID);
	if(pcsCharacter)
	{
		*plRate += pThis->m_pagpmCharacter->GetGameBonusDropRate(pcsCharacter);
	}

	return TRUE;
}

/*
	2005.05.25.	Bt SungHoon
	��Ƽ�� ������ ��� ��Ƽ�� �Ҽӵ� ��� ����� �˻��� ���� �����Ѵ�.
	1...2
	1...3  2...3 
	1...4  2...4  3...4
	.....
	���ΰ� �˻��� �� �ȴ�.
*/
BOOL AgsmParty::RemoveFarMemberView(AgpdParty *pcsParty)
{
	if(pcsParty->m_nCurrentMember < 2) return FALSE;
	for (int i = 0; i < pcsParty->m_nCurrentMember - 1; ++i)
	{
		if (pcsParty->m_lMemberListID[i] != AP_INVALID_CID)
			RemoveFarMemberView(pcsParty, pcsParty->m_lMemberListID[i], i + 1);
	}
	return TRUE;

}

/*
	2005.05.25.	Bt SungHoon
	�� ����� ���� ��� �ٸ� ��Ƽ ����� ���� �ָ� ���� ��� �����Ѵ�.
*/
BOOL AgsmParty::RemoveFarMemberView(AgpdParty *pcsParty, INT32 lCID, int nStartPos )
{
	ApAutoLockCharacter LockChecker(m_pagpmCharacter, lCID);
	AgpdCharacter *pcsCharacter = LockChecker.GetCharacterLock();
	if (!pcsCharacter) return FALSE;

	DPNID CharacterDPNID = m_pagsmCharacter->GetCharDPNID(pcsCharacter);
	for (int i = nStartPos; i < pcsParty->m_nCurrentMember; ++i)
	{
		if(lCID == pcsParty->m_lMemberListID[i] || pcsParty->m_lMemberListID[i] == AP_INVALID_CID) continue;	//	���� ������ ó������ �ʴ´�.

		ApAutoLockCharacter Lock(m_pagpmCharacter, pcsParty->m_lMemberListID[i]);
		AgpdCharacter	*pcsMember	= Lock.GetCharacterLock();
		if (!pcsMember || pcsMember == pcsCharacter) continue;

		INT32	lDistance = CheckValidDistance(pcsMember, pcsCharacter);
		if (lDistance > 2)		// �Ÿ��� 2 �̻��̸� �þ߳��� ���°Ŵ�.
		{
			m_pagsmCharacter->SendPacketCharRemoveForView(pcsMember, CharacterDPNID, FALSE);
			m_pagsmCharacter->SendPacketCharRemoveForView(pcsCharacter, m_pagsmCharacter->GetCharDPNID(pcsMember), FALSE);
		}
	}
	return TRUE;
}

/*
	2005.06.07. By SungHoon
	pcsTarget�� pcsOperator�κ��� �ش� �Ÿ��ȿ� ��ȿ��(��������) ���·� �ִ��� Ȯ���Ѵ�.
*/
BOOL AgsmParty::IsNearMember(AgpdCharacter *pcsOperator, AgpdCharacter *pcsTarget, FLOAT lDistance /*= 8000 */)
{
	AuPOS	stDelta;
	// �ӽ÷� Y �� �˻縦 ���� �ʴ´�. ������ DestinationPos�� Y�� ���� �� �ؿ� �ִ°�� �ʿ��� �׸��� �������� ���ϹǷ� ������ ��ǥ���� �������� ���ϰ� �̵��Ѵ�.
	////////////////////////////////////////////////////////////////////////
	// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
	AuMath::V3DSubtract(&stDelta, &pcsTarget->m_stPos, &pcsOperator->m_stPos);
	stDelta.y = 0;

	// fDiv�� �� �Ÿ�
	FLOAT fDiv = AuMath::V3DLength(&stDelta);

	// 50M �ȿ� �ִ��� üũ�Ѵ�.
	if (fDiv > lDistance) return FALSE;

/*
	INT32	lDistance = CheckValidDistance(pcsParty->m_pcsMemberList[i], pcsBase);

	if (lDistance > 1)		// �Ÿ��� 2 �̻��̸� �þ߳��� ���°Ŵ�.
		continue;
*/

	// �þ߳��� �����ִ� ���̴�. ��.....

	// Ȥ�� �������� �ƴ��� ����.
	// �������� �ϰ͵� ���Ѵ�.

	if (pcsTarget->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	/*
	// ������ �ִ� �� ����.

	INT16	nBindingRegion	= m_papmMap->GetRegion(pcsTarget->m_stPos.x , pcsTarget->m_stPos.z);

	ApmMap::RegionTemplate	*pcsRegionTemplate	= m_papmMap->GetTemplate(nBindingRegion);
	if (pcsRegionTemplate &&
		pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_SAFE)
		return FALSE;
	*/

	return TRUE;
}


/*
	2005.06.29 By SungHoon
	��Ƽ���� ������ ���� �ʿ��ϸ� ������Ʈ �� �ٽ� ����϶�� ��.(�Ѥ�)
*/
BOOL AgsmParty::RecalcPartyMemberHP(AgpdParty *pcsParty, AgpdCharacter *pcsCharacter, BOOL bRecalc)
{
	if (!pcsParty || !pcsCharacter)
		return FALSE;

	AgpdPartyADChar	*pcsAttachData	= m_pagpmParty->GetADCharacter(pcsCharacter);
	m_pagpmFactors->DestroyFactor(&pcsAttachData->m_csFactorPoint);

	AgpdCharacter	*pcsLeader	= m_pagpmCharacter->GetCharacterLock(pcsParty->m_lMemberListID[0]);
	if (pcsLeader)
	{
		if (bRecalc)
			m_pagpmParty->CalcPartyFactor(pcsLeader, pcsCharacter, &pcsAttachData->m_csFactorPoint);

		pcsLeader->m_Mutex.Release();
	}

#ifdef	AGSMPARTY_UPDATE_PARTY_FACTOR
	m_pagsmCharacter->ReCalcCharacterFactors(pcsCharacter);
#endif	//AGSMPARTY_UPDATE_PARTY_FACTOR

	// 2005.06.30. steeple
	// �࿩�� ���� HP �� Max �� �Ѿ ���� �������� �𸣴� �ٽ� üũ�غ���.
	// �ᱹ�� �̷��� �� �� ������ ���� ������3
	INT32 lMaxHP = 0, lMaxMP = 0, lMaxSP = 0;
	INT32 lCurrentHP = 0, lCurrentMP = 0, lCurrentSP = 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

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
		m_pagsmCharacter->SendPacketFactor(pvPacket, pcsCharacter, PACKET_PRIORITY_4);
		m_pagsmCharacter->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmParty::RecallPartyMember(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdPartyADChar	*pcsAttachData	= m_pagpmParty->GetADCharacter(pcsCharacter);

	if (!pcsAttachData->pcsParty)
		return FALSE;

	ApmMap::RegionTemplate *pcsRegionTemplate = m_papmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
	if(NULL == pcsRegionTemplate)
		return FALSE;

	// ����ڰ� ������ �ְų� ���ҿ� ������ ����� �� ����.
	if(m_pagpmCharacter->IsInJail(pcsCharacter) || m_pagpmCharacter->IsInConsulationOffice(pcsCharacter) ||
	   pcsRegionTemplate->ti.stType.bRecallItem)
		return FALSE;

	// ����� ���� ��, �����Ǿ� ���������� ��� ���Ѵ�.
	if(m_pagpmSiegeWar->IsInSecretDungeon(pcsCharacter))
		return FALSE;

	BOOL bResult = FALSE;

	for (int i = 0; i < pcsAttachData->pcsParty->m_nCurrentMember; ++i)
	{
		if (AgpdCharacter *pcsMember = m_pagpmCharacter->GetCharacterLock(pcsAttachData->pcsParty->m_lMemberListID[i]))
		{
			if (pcsMember != pcsCharacter) // ������ ó�� ���Ѵ�
			{
				// 2008.04.08. steeple
				// �������� ���� �������� �������� �θ��� ���Ѵ�.
				// Ż���� ź ���·� �������� �������� �θ��� ���Ѵ�.
				if(pcsMember->m_bIsTrasform && m_pagpmCharacter->GetCurrentRegionLevelLimit(pcsCharacter) != 0 ||
				   m_pagpmCharacter->IsRideOn(pcsMember) && m_pagpmCharacter->GetCurrentRegionLevelLimit(pcsCharacter) != 0)
				{
					pcsMember->m_Mutex.Release();
					continue;
				}

				//////////////////////////////////////////////////////////////////////////
				//
				AuPOS stTeleportPos = pcsMember->m_stPos;
				AuPOS stCharacterPos = pcsCharacter->m_stPos;

				if( m_papmMap->GetTargetPositionLevelLimit( stTeleportPos ) 
					&& m_papmMap->GetTargetPositionLevelLimit( stCharacterPos ) )
				{
					/*if(m_pagsmSystemMessage)
						m_pagsmSystemMessage->SendSystemMessage( pcsMember , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );*/
					
					pcsMember->m_Mutex.Release();
					continue;
				}

				// �ش� ĳ���Ͱ� ����� ������ ������ �ȵȴ�.
				if (pcsMember->m_unActionStatus != AGPDCHAR_STATUS_DEAD &&
					!m_pagpmSiegeWar->IsInSecretDungeon(pcsMember)
					&& pcsMember->m_nBindingRegionIndex != 81	// �����Ǿ�
					&& pcsMember->m_nBindingRegionIndex != 88	// �����Ǿ�
					&& !m_pagpmCharacter->IsInJail(pcsMember)	// ����
					&& !m_pagpmCharacter->IsInConsulationOffice(pcsMember))	//����
				{
					// �ش� ĳ���Ͱ� ���������ȿ� �־ �ȵȴ�.
					AgpdSiegeWar* pcsSiegeWar = m_pagpmSiegeWar->GetSiegeWarInfo(pcsMember);
					if( !(pcsSiegeWar && m_pagpmSiegeWar->IsStarted(pcsSiegeWar)) )
					{
						SendPacketRequestRecall(pcsCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsMember));
						bResult = TRUE;
					}
				}
			}

			pcsMember->m_Mutex.Release();
		}
	}

	return bResult;
}

BOOL AgsmParty::OnReceiveAcceptRecall(INT32 lOperator, INT32 lTarget)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(lOperator);
	if (!pcsCharacter)
		return FALSE;

	AuAutoLock pLock(pcsCharacter->m_Mutex);
	if(!pLock.Result())
		return FALSE;
	
	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	if (m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return FALSE;

	AgsdCharacter *pagsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (NULL == pagsdCharacter)
		return FALSE;

	AgpdCharacter* pcsTarget = m_pagpmCharacter->GetCharacter(lTarget);
	if (!pcsTarget)
		return FALSE;

	AuAutoLock pLock2(pcsTarget->m_Mutex);
	if(!pLock2.Result())
		return FALSE;
	
	AgpdPartyADChar	*pcsOperatorAttachData	= m_pagpmParty->GetADCharacter(pcsCharacter);
	AgpdPartyADChar	*pcsTargetAttachData	= m_pagpmParty->GetADCharacter(pcsTarget);

	if(!pcsOperatorAttachData || !pcsTargetAttachData)
		return FALSE;

	if (pcsOperatorAttachData->lPID == pcsTargetAttachData->lPID)
	{
		AuPOS stTeleportPos = pcsTarget->m_stPos;
		AuPOS stCharacterPos = pcsCharacter->m_stPos;

		ApmMap::RegionTemplate	* pTemplateTarget		= m_papmMap->GetTemplate( m_papmMap->GetRegion( stTeleportPos.x , stTeleportPos.z ) );
		ApmMap::RegionTemplate	* pTemplateCharacter	= m_papmMap->GetTemplate( m_papmMap->GetRegion( stCharacterPos.x , stCharacterPos.z ) );

		if (pTemplateTarget && pTemplateCharacter)
		{
			// ����� �� ���� �����̶�� ��� ���Ѵ�.
			if (pTemplateTarget->ti.stType.bRecallItem || pTemplateCharacter->ti.stType.bRecallItem)
			{
				if(m_pagsmSystemMessage)
					m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );

				return FALSE;
			}
		}
		//JK_��ȯ ��û�� ī��ƽ���� �̵��� ��츦 ���� �����Ҷ� ���Ű� ī��ƽ ���� Ȯ�� 
		if(pcsCharacter->m_bIsTrasform &&  m_papmMap->GetTargetPositionLevelLimit(stTeleportPos))
		{
			if(m_pagsmSystemMessage)
				m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );
			return FALSE;
		}

		if( m_papmMap->GetTargetPositionLevelLimit( stTeleportPos ) 
			&& m_papmMap->GetTargetPositionLevelLimit( stCharacterPos ) )
		{
			if(m_pagsmSystemMessage)
				m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );
			
			return FALSE;
		}

		if( m_pagpmCharacter->IsCombatMode(pcsCharacter) 
			|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE)
			|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK)
			|| (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
			|| (m_pagpmCharacter->IsActionBlockCondition(pcsCharacter))
			)
		{
			if(m_pagsmSystemMessage)
				m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );

			return FALSE;
		}

		SendPacketAcceptRecall(lOperator, lTarget, m_pagsmCharacter->GetCharDPNID(pcsTarget));

		m_pagpmCharacter->SetActionBlockTime(pcsCharacter, 3000);
		pagsdCharacter->m_bIsTeleportBlock = TRUE;

		m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);
		m_pagpmCharacter->UpdatePosition(pcsCharacter, &pcsTarget->m_stPos, FALSE, TRUE);
		m_pagpmSummons->UpdateAllSummonsPosToOwner(pcsCharacter);

		pagsdCharacter->m_bIsTeleportBlock = FALSE;
	}

	return TRUE;
}

BOOL AgsmParty::OnReceiveRejectRecall(INT32 lOperator, INT32 lTarget)
{
	AgpdCharacter	*pcsOperator	= m_pagpmCharacter->GetCharacterLock(lOperator);
	if (!pcsOperator)
		return FALSE;

	AgpdCharacter	*pcsTarget		= m_pagpmCharacter->GetCharacterLock(lTarget);
	if (!pcsTarget)
		return FALSE;

	AgpdPartyADChar	*pcsOperatorAttachData	= m_pagpmParty->GetADCharacter(pcsOperator);
	AgpdPartyADChar	*pcsTargetAttachData	= m_pagpmParty->GetADCharacter(pcsTarget);

	if (pcsOperatorAttachData->lPID == pcsTargetAttachData->lPID)
	{
		SendPacketRejectRecall(lOperator, lTarget, m_pagsmCharacter->GetCharDPNID(pcsTarget));
	}

	pcsOperator->m_Mutex.Release();
	pcsTarget->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmParty::SetCallbackUseRecall(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMPARTY_CB_USE_RECALL, pfCallback, pClass);
}

BOOL AgsmParty::SetCallbackBuddyCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMPARTY_CB_BUDDY_CHECK, pfCallback, pClass);
}

/*
	2005.10.24. By SungHoon
	Muderer Point �� ���� ��� �Ҹ�� ��Ȳ�� ���� ��Ƽ�� ��ü�Ѵ�.
*/
BOOL AgsmParty::CBUpdateMurdererPoint(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmParty	*pThis				= (AgsmParty *) pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	INT32			lDiff			= *((INT32 *) pCustData);

	INT32 lMurdererLevel = pThis->m_pagpmCharacter->GetMurdererLevel(pcsCharacter);
	if (lMurdererLevel >= AGPMCHAR_MURDERER_LEVEL1_POINT)		//	��Ƽ Ż��
	{
		AgpdParty *pcsParty = pThis->m_pagpmParty->GetPartyLock(pcsCharacter);
		if (!pcsParty) return FALSE;

		AgsdCharacter *pcsAgsdTargetChar = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		pThis->SendPacketInviteFailed(pcsCharacter->m_lID,
								NULL,
								AGSMPARTY_PACKET_OPERATION_LEAVE_BY_MURDERER,
								pcsAgsdTargetChar->m_dpnidCharacter);

		BOOL	bDestroyParty = FALSE;

		pThis->m_pagpmParty->RemoveMember(pcsParty, pcsCharacter->m_lID, &bDestroyParty);

		if (!bDestroyParty)
			pcsParty->m_Mutex.Release();
	}
	return TRUE;
}

