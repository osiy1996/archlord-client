
#include <AgpmCharacter/AgpmCharacter.h>
#include <AuMath/AuMath.h>
#include <AgpmSiegeWar/AgpmSiegeWar.h>
#include <AgpmEventSkillMaster/AgpmEventSkillMaster.h>

//#define AGPMCHARACTER_MOVE_ADJUST_DISTANCE		100

BOOL AgpmCharacter::NegligiblePosition(AuPOS *posSrc1, AuPOS *posSrc2)
{
	if (!posSrc1 || !posSrc2)
		return FALSE;

	FLOAT	fDifferX	= fabsf(posSrc1->x - posSrc2->x);
	//INT32	lDifferY	= abs(posSrc1->y - posSrc2->y);
	FLOAT	fDifferZ	= fabsf(posSrc1->z - posSrc2->z);

	if (fDifferX < 50.f &&
		//lDifferY < 20 &&
		fDifferZ < 50.f )
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::SyncMoveStartPosition(AgpdCharacter *pcsCharacter, AuPOS *posSrc)
{
	if (!pcsCharacter || !posSrc)
		return FALSE;

	// �����̴� ���� ��ġ�� ���ؼ� ���̰� ���� ���� ��� ó���Ѵ�.
	//
	//		�ϴ� �׽�Ʈ�� �����غ���.
	//

	FLOAT	fDifferX	= fabsf(pcsCharacter->m_stPos.x - posSrc->x);
	FLOAT	fDifferZ	= fabsf(pcsCharacter->m_stPos.z - posSrc->z);

	if (fDifferX > 600.f ||
		fDifferZ > 600.f )
	{
		return UpdatePosition(pcsCharacter, posSrc, FALSE, TRUE);
	}

	return FALSE;
}

BOOL AgpmCharacter::AdjustMoveSpeed(AgpdCharacter *pcsCharacter, AuPOS *pcsSrc)
{
	PROFILE("AgpmCharacter::AdjustMoveSpeed");

	if (!pcsCharacter || !pcsSrc || !pcsCharacter->m_bMove)
		return FALSE;

	AuPOS	stDeltaReceivedPos, stDeltaCharacterPos;

	// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
	AuMath::V3DSubtract(&stDeltaCharacterPos, &pcsCharacter->m_stDestinationPos, &pcsCharacter->m_stPos);
	AuMath::V3DSubtract(&stDeltaReceivedPos, &pcsCharacter->m_stDestinationPos, pcsSrc);

	stDeltaCharacterPos.y	= 0;
	stDeltaReceivedPos.y	= 0;

	// fDiv�� �� �Ÿ�
	FLOAT	fReceivedPosDiv = AuMath::V3DLength(&stDeltaReceivedPos);
	FLOAT	fCharacterPosDiv = AuMath::V3DLength(&stDeltaCharacterPos);

	if (abs(fReceivedPosDiv - fCharacterPosDiv) < 5)
		return TRUE;

	if (fReceivedPosDiv < 250 ||
		fCharacterPosDiv < 250)
		return TRUE;

	INT32	lMoveSpeed	= 0;
	if (pcsCharacter->m_bMoveFast || pcsCharacter->m_bHorizontal)
	{
		lMoveSpeed = GetFastMoveSpeed(pcsCharacter);

		if (lMoveSpeed == 0)
			lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED * 3;
	}
	else
	{
		lMoveSpeed = GetMoveSpeed(pcsCharacter);

		if (lMoveSpeed == 0)
			lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED;
	}

	UINT32 ulNewEndMoveTime = (UINT32) (pcsCharacter->m_ulStartMoveTime + fReceivedPosDiv / (lMoveSpeed / 10000.0) );

	if (ulNewEndMoveTime == pcsCharacter->m_ulStartMoveTime)
		return FALSE;

	INT32 lNewMoveSpeed	= (INT32) (fCharacterPosDiv / (ulNewEndMoveTime - pcsCharacter->m_ulStartMoveTime) * 10000.0);

	pcsCharacter->m_ulEndMoveTime		= ulNewEndMoveTime;
	pcsCharacter->m_lAdjustMoveSpeed	= lNewMoveSpeed - lMoveSpeed;

	// �ʹ� ���� ���°� ���� ���� �������ش�.
	if ((FLOAT) abs(pcsCharacter->m_lAdjustMoveSpeed) > (FLOAT) (lMoveSpeed * 1.15))
	{
		if (pcsCharacter->m_lAdjustMoveSpeed > 0)
			pcsCharacter->m_lAdjustMoveSpeed = (INT32) (lMoveSpeed * 0.15);
		else
			pcsCharacter->m_lAdjustMoveSpeed = -((INT32) (lMoveSpeed * 0.15));
	}

	return TRUE;
}

BOOL AgpmCharacter::MoveCharacter(INT32 lCID, AuPOS *pstPos, MOVE_DIRECTION eMoveDirection , BOOL bPathFinding, BOOL bMoveFast, BOOL bHorizontal, BOOL bExistNextAction, BOOL bStopMove)
{
	return MoveCharacter(GetCharacter(lCID), pstPos, eMoveDirection, bPathFinding, bMoveFast, bHorizontal, bExistNextAction, bStopMove);
}

BOOL AgpmCharacter::MoveCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstPos, MOVE_DIRECTION eMoveDirection , BOOL bPathFinding, BOOL bMoveFast, BOOL bHorizontal, BOOL bExistNextAction, BOOL bStopMove)
{
	//PROFILE("AgpmCharacter::MoveCharacter");
	
	FLOAT	fDiv;
	AuPOS	stDelta;

	// Validation Check
	if (!pcsCharacter || !pstPos || pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD)
		return FALSE;

	if (!CheckActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE))
		return FALSE;

	if (pcsCharacter->m_bHorizontal && !bHorizontal)
		return FALSE;

	/*
	// 2004.08.25. steeple
	// stun ����
	//if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return FALSE;

	// 2007.06.27. steeple
	if(IsStatusSleep(pcsCharacter))
		return FALSE;

	if(IsPC(pcsCharacter) && IsAllBlockStatus(pcsCharacter))
		return FALSE;
	*/

	if (!bExistNextAction)
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

	CheckLockAction(pcsCharacter);

	INT32	lMoveSpeed	= 0;
	if (bMoveFast || bHorizontal)
	{
		lMoveSpeed = GetFastMoveSpeed(pcsCharacter);

		if (lMoveSpeed == 0)
			lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED * 3;
	}
	else
	{
		lMoveSpeed = GetMoveSpeed(pcsCharacter);

		if (lMoveSpeed == 0)
			lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED;
	}

	//UINT32	ulClockCount	= GetClockCount();
	UINT32	ulClockCount	= m_ulPrevClockCount;

//	if (pcsCharacter->m_bMove)
//	{
//		// �̹� �����̴� ���̶�� ���� ó���ð��� ���� �󸶳� ������ ������.. ������ش�.
//		// ���̵��� �ȵ��Ƶ� �ּ��� ���ڸ� �ٱ�� �ϸ� �ȵȴ�.
//
//		FLOAT	ulDeltaMove = (FLOAT) ((ulClockCount - pcsCharacter->m_ulPrevProcessTime) * /*0.4*/ (lMoveSpeed / 10000.0) );
//
//		if (ulDeltaMove > AGPMCHARACTER_MOVE_ADJUST_DISTANCE)
//		{
//			AuPOS	stPos;
//			stPos.x		= pcsCharacter->m_stPos.x + pcsCharacter->m_stDirection.x * ulDeltaMove;
//			stPos.y		= pcsCharacter->m_stPos.y + pcsCharacter->m_stDirection.y * ulDeltaMove;
//			stPos.z		= pcsCharacter->m_stPos.z + pcsCharacter->m_stDirection.z * ulDeltaMove;
//
//			if (UpdatePosition(pcsCharacter, &stPos))
//			{
//				pcsCharacter->m_ulPrevProcessTime = ulClockCount;
//			}
//		}
//	}
	
	if (bStopMove)
	{
		// ����� (������) ��ġ�� ���� �������� �ʴ´�.
		if (NegligiblePosition(&pcsCharacter->m_stPos, pstPos))
		{
			StopCharacter(pcsCharacter, pstPos);
			//pcsCharacter->m_bMove = FALSE;
			return FALSE;
		}

		bMoveFast	= pcsCharacter->m_bMoveFast;
	}
	else
	{
		// ���� ��ġ�� �����δٸ�, FALSE�� return
		if (0.0f == AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos,*pstPos) && eMoveDirection == MD_NODIRECTION )
		{
			//OutputDebugString("AgpmCharacter::MoveCharacter() Equivalent Position !!!\n");
			StopCharacter(pcsCharacter, NULL);
			//pcsCharacter->m_bMove = FALSE;

			return FALSE;
		}
	}

	// ������ (2003-12-18 ���� 11:54:48) : ���������� ��ŷ�� ������ �ʿ䰡 ������?..;;
	// �ϴ� �����ڰ� �ּ� ó��.
	if (!IsPC(pcsCharacter) && !eMoveDirection && m_pcsApmMap && m_pcsApmMap->CheckBlockingInfo(*pstPos, ApmMap::GROUND))
	{
		return FALSE;
	}

	UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE);

	if (!bHorizontal && !bStopMove)
		//Move�ϱ� ������ ������ �������� Turn
	{
		if (eMoveDirection)
		{
			// ������ �԰��� ����.
			FLOAT	fTurnAngle = GetWorldDirection(pstPos->x, pstPos->z);

			switch( eMoveDirection )
			{
			default:
			case MD_FORWARD				:
			case MD_FORWARDRIGHT		:
			case MD_FORWARDLEFT			:
				break;

			case MD_RIGHT				:
			case MD_BACKWARD			:
			case MD_LEFT				:
				fTurnAngle += ( eMoveDirection - MD_FORWARD ) * 45.0f;
				break;

			case MD_BACKWARDRIGHT		:
			case MD_BACKWARDLEFT		:
				fTurnAngle += ( eMoveDirection - MD_FORWARD ) * 180.0f;
				break;
			}	 		

			TurnCharacter( pcsCharacter, 0, fTurnAngle );
		}
		else
		{
			TurnCharacter( pcsCharacter, 0, GetSelfCharacterTurnAngle( pcsCharacter, pstPos ) );
		}
	}

	if (eMoveDirection)
	{
		// �������� �������̶��, pstPos�� ���� Vector�̹Ƿ�, �̸� Normalize�ؼ� Direction�� �ִ´�.
		AuMath::V3DNormalize(&pcsCharacter->m_stDirection, pstPos);
	}
	else
	{
		// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
		AuMath::V3DSubtract(&stDelta, pstPos, &pcsCharacter->m_stPos);

		stDelta.y	= 0;

		// fDiv�� �� �Ÿ�
		fDiv = AuMath::V3DLength(&stDelta);

		// �̸� ������ Normalized Vector�� ���Ѵ�.
		pcsCharacter->m_stDirection.x = stDelta.x / fDiv;
		pcsCharacter->m_stDirection.y = stDelta.y / fDiv;
		pcsCharacter->m_stDirection.z = stDelta.z / fDiv;

		// ��ǥ ��ġ�� ���µ� �ɸ��� �ð��� ����� ���´�.
		pcsCharacter->m_stDestinationPos = *pstPos;
		pcsCharacter->m_ulStartMoveTime = ulClockCount;

		pcsCharacter->m_ulEndMoveTime = (UINT32) (ulClockCount + fDiv / /*0.4*/ (lMoveSpeed / 10000.0) );		// �ʴ� 4Ÿ���� ���ٰ� �����Ѵ�.
		//if (bMoveFast)
		//	pcsCharacter->m_ulEndMoveTime -= (UINT32) ((fDiv / (lMoveSpeed / 10.0)) * (2.0 / 3.0));

		if (bHorizontal)
			pcsCharacter->m_ulEndMoveTime -= (UINT32) ((fDiv / (lMoveSpeed / 10000.0)) * (3.0 / 4.0));
	}

	// �������� �ʰ� �־�����, ���ݺ��� �����̴� �ɷ� (������ ó�� �ð��� ����� �ٲ۴�.
	if (!pcsCharacter->m_bMove)
		pcsCharacter->m_ulPrevProcessTime = ulClockCount;

	pcsCharacter->m_eMoveDirection	= eMoveDirection	;
	pcsCharacter->m_bPathFinding	= bPathFinding		;
	pcsCharacter->m_bMove			= TRUE				;
	pcsCharacter->m_bMoveFast		= bMoveFast			;
	pcsCharacter->m_bHorizontal		= bHorizontal		;
	pcsCharacter->m_bMoveFollow		= FALSE				;
	pcsCharacter->m_lFollowTargetID	= AP_INVALID_CID	;
	pcsCharacter->m_ulSyncMoveTime	= 0					;

	pcsCharacter->m_lAdjustMoveSpeed= 0					;

	if (IsPC(pcsCharacter))
	{
		ApmMap::BLOCKINGTYPE eType = GetBlockingType( pcsCharacter );

		m_pcsApmMap->GetValidDestination(&pcsCharacter->m_stPos, &pcsCharacter->m_stDestinationPos, &pcsCharacter->m_stDestinationPos , pcsCharacter->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , eType );
		GetValidDestination(&pcsCharacter->m_stPos, &pcsCharacter->m_stDestinationPos, &pcsCharacter->m_stDestinationPos , eType , pcsCharacter );

		// ������ (2004-09-17 ���� 5:21:09) : ���ε� ���� ����..
		INT16	nBindingRegion = m_pcsApmMap->GetRegion( pcsCharacter->m_stPos.x , pcsCharacter->m_stPos.z );

		pcsCharacter->m_ulNextRegionRefreshTime	= ulClockCount + GetRegionRefreshInterval();

		if( nBindingRegion >= 0 && pcsCharacter->m_nBindingRegionIndex != nBindingRegion )
		{
			// ������ ���Ѵ�..
			// �ݹ�ȣ��..

			INT16 nPrevRegion = pcsCharacter->m_nBindingRegionIndex;
			pcsCharacter->m_nBindingRegionIndex = nBindingRegion;
			EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pcsCharacter, &nPrevRegion );
		}
	}

	return EnumCallback(AGPMCHAR_CB_ID_MOVE, pcsCharacter, NULL);
}

BOOL AgpmCharacter::CheckStopFollow(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetCharacter)
{
	PROFILE("AgpmCharacter::CheckStopFollow");

	if (!pcsCharacter || !pcsTargetCharacter)
		return TRUE;

	if (pcsCharacter->m_lFollowDistance == 0)
		return TRUE;

	if (m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &pcsTargetCharacter->m_stPos, pcsCharacter->m_lFollowDistance, 0, NULL))
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::MoveCharacterFollow(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetCharacter, INT32 lFollowDistance, BOOL bMoveFirst, FLOAT *pfDeltaMove)
{
	PROFILE("AgpmCharacter::MoveCharacterFollow");

	if (!pcsCharacter || !pcsTargetCharacter || lFollowDistance <= 0)
		return FALSE;

	FLOAT	fDiv;
	AuPOS	stDelta;
	AuPOS	*pstPos	= NULL;

	// Validation Check
	if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD)
		return FALSE;

	if (!CheckActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE))
		return FALSE;

	// 2004.08.25. steeple
	// �����̸� �ȵǴ� �������� üũ�Ѵ�
	//if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN || 
		pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
		return FALSE;

	// 2007.06.27. steeple
	if(IsStatusSleep(pcsCharacter))
		return FALSE;

	CheckLockAction(pcsCharacter);

	INT32	lMoveSpeed	= 0;
	lMoveSpeed = GetFastMoveSpeed(pcsCharacter);

	if (lMoveSpeed == 0)
		lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED * 3;

	//UINT32	ulClockCount	= GetClockCount();
	UINT32	ulClockCount	= m_ulPrevClockCount;

//	if (pcsCharacter->m_bMove)
//	{
//		// �̹� �����̴� ���̶�� ���� ó���ð��� ���� �󸶳� ������ ������.. ������ش�.
//		// ���̵��� �ȵ��Ƶ� �ּ��� ���ڸ� �ٱ�� �ϸ� �ȵȴ�.
//
//		FLOAT	ulDeltaMove = (FLOAT) ((ulClockCount - pcsCharacter->m_ulPrevProcessTime) * /*0.4*/ (lMoveSpeed / 10000.0) );
//
//		if (ulDeltaMove > AGPMCHARACTER_MOVE_ADJUST_DISTANCE)
//		{
//			AuPOS	stPos;
//			stPos.x		= pcsCharacter->m_stPos.x + pcsCharacter->m_stDirection.x * ulDeltaMove;
//			stPos.y		= pcsCharacter->m_stPos.y + pcsCharacter->m_stDirection.y * ulDeltaMove;
//			stPos.z		= pcsCharacter->m_stPos.z + pcsCharacter->m_stDirection.z * ulDeltaMove;
//
//			if (UpdatePosition(pcsCharacter, &stPos))
//			{
//				pcsCharacter->m_ulPrevProcessTime = ulClockCount;
//
//				if (pfDeltaMove)
//					*pfDeltaMove	= *pfDeltaMove - ulDeltaMove;
//			}
//		}
//	}

	pstPos	= &pcsTargetCharacter->m_stPos;

	// ���� ��ġ�� �����δٸ�, FALSE�� return
	if (0.0f == AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos,*pstPos))
	{
		//OutputDebugString("AgpmCharacter::MoveCharacter() Equivalent Position !!!\n");
		StopCharacter(pcsCharacter, NULL);
		//pcsCharacter->m_bMove = FALSE;

		return FALSE;
	}

	ApmMap::BLOCKINGTYPE eType = GetBlockingType( pcsCharacter );

	if (m_pcsApmMap && (m_pcsApmMap->CheckBlockingInfo(*pstPos , eType ) & pcsCharacter->m_ulCheckBlocking))
	{
		return FALSE;
	}

	UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE);

	//Move�ϱ� ������ ������ �������� Turn
	TurnCharacter( pcsCharacter, 0, GetSelfCharacterTurnAngle( pcsCharacter, pstPos ) );
	
	// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
	AuMath::V3DSubtract(&stDelta, pstPos, &pcsCharacter->m_stPos);

	stDelta.y	= 0;

	// fDiv�� �� �Ÿ�
	fDiv = AuMath::V3DLength(&stDelta);

	// �̸� ������ Normalized Vector�� ���Ѵ�.
	pcsCharacter->m_stDirection.x = stDelta.x / fDiv;
	pcsCharacter->m_stDirection.y = stDelta.y / fDiv;
	pcsCharacter->m_stDirection.z = stDelta.z / fDiv;

	// ��ǥ ��ġ�� ���µ� �ɸ��� �ð��� ����� ���´�.
	pcsCharacter->m_stDestinationPos = *pstPos;

	if( IsPC( pcsCharacter ) )
	{
		m_pcsApmMap->GetValidDestination(&pcsCharacter->m_stPos, &pcsCharacter->m_stDestinationPos, &pcsCharacter->m_stDestinationPos , pcsCharacter->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , eType );
		GetValidDestination(&pcsCharacter->m_stPos, &pcsCharacter->m_stDestinationPos, &pcsCharacter->m_stDestinationPos , eType , pcsCharacter );
	}

//	pcsCharacter->m_ulStartMoveTime = ulClockCount;

//	pcsCharacter->m_ulEndMoveTime = (UINT32) (ulClockCount + fDiv / /*0.4*/ (lMoveSpeed / 10000.0) );		// �ʴ� 4Ÿ���� ���ٰ� �����Ѵ�.
	//if (bMoveFast)
	//	pcsCharacter->m_ulEndMoveTime -= (UINT32) ((fDiv / (lMoveSpeed / 10.0)) * (2.0 / 3.0));

	// �������� �ʰ� �־�����, ���ݺ��� �����̴� �ɷ� (������ ó�� �ð��� ����� �ٲ۴�.
	if (!pcsCharacter->m_bMove)
		pcsCharacter->m_ulPrevProcessTime = ulClockCount;

	if (!pcsCharacter->m_bMoveFollow || bMoveFirst)
		pcsCharacter->m_ulSyncMoveTime = 0;

	pcsCharacter->m_bMoveFollow		= TRUE		;
	pcsCharacter->m_eMoveDirection	= MD_NODIRECTION;
	pcsCharacter->m_bPathFinding	= FALSE		;
	pcsCharacter->m_bMove			= TRUE		;
	pcsCharacter->m_bMoveFast		= TRUE		;
	pcsCharacter->m_bHorizontal		= FALSE		;

	pcsCharacter->m_lAdjustMoveSpeed= 0			;

	pcsCharacter->m_lFollowTargetID	= pcsTargetCharacter->m_lID;

	pcsCharacter->m_lFollowDistance	= lFollowDistance;

	if (IsPC(pcsCharacter))
	{
		// ������ (2004-09-17 ���� 5:21:09) : ���ε� ���� ����..
		INT16	nBindingRegion = m_pcsApmMap->GetRegion( pcsCharacter->m_stPos.x , pcsCharacter->m_stPos.z );

		pcsCharacter->m_ulNextRegionRefreshTime	= ulClockCount + GetRegionRefreshInterval();

		if( nBindingRegion >= 0 && pcsCharacter->m_nBindingRegionIndex != nBindingRegion )
		{
			// ������ ���Ѵ�..
			// �ݹ�ȣ��..

			INT16 nPrevRegion = pcsCharacter->m_nBindingRegionIndex;
			pcsCharacter->m_nBindingRegionIndex = nBindingRegion;
			EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pcsCharacter, &nPrevRegion );
		}
	}

	return EnumCallback(AGPMCHAR_CB_ID_MOVE, pcsCharacter, NULL);
}

// 2006.10.20. steeple
BOOL AgpmCharacter::MoveCharacterToPositionWithDistance(AgpdCharacter *pcsCharacter, AuPOS stTargetPos, INT32 lDistance, BOOL bMoveFirst, FLOAT *pfDeltaMove)
{
	if(!pcsCharacter || lDistance <= 0)
		return FALSE;

	FLOAT	fDiv;
	AuPOS	stDelta;
	AuPOS	*pstPos	= NULL;

	// Validation Check
	if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD)
		return FALSE;

	if (!CheckActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE))
		return FALSE;

	// 2004.08.25. steeple
	// stun ����
	//if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return FALSE;

	// 2007.06.27. steeple
	if(IsStatusSleep(pcsCharacter))
		return FALSE;

	CheckLockAction(pcsCharacter);

	INT32	lMoveSpeed	= 0;
	lMoveSpeed = GetFastMoveSpeed(pcsCharacter);

	if (lMoveSpeed == 0)
		lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED * 3;

	UINT32	ulClockCount	= m_ulPrevClockCount;

	pstPos	= &stTargetPos;

	// ���� ��ġ�� �����δٸ�, FALSE�� return
	if (0.0f == AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos,*pstPos))
	{
		//OutputDebugString("AgpmCharacter::MoveCharacter() Equivalent Position !!!\n");
		StopCharacter(pcsCharacter, NULL);
		//pcsCharacter->m_bMove = FALSE;

		return FALSE;
	}

	ApmMap::BLOCKINGTYPE eType = GetBlockingType( pcsCharacter );
	if (m_pcsApmMap && (m_pcsApmMap->CheckBlockingInfo( *pstPos , eType ) & pcsCharacter->m_ulCheckBlocking))
	{
		return FALSE;
	}

	UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE);

	//Move�ϱ� ������ ������ �������� Turn
	TurnCharacter( pcsCharacter, 0, GetSelfCharacterTurnAngle( pcsCharacter, pstPos ) );
	
	// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
	AuMath::V3DSubtract(&stDelta, pstPos, &pcsCharacter->m_stPos);

	stDelta.y	= 0;

	// fDiv�� �� �Ÿ�
	fDiv = AuMath::V3DLength(&stDelta);

	// 2006.10.25. steeple
	// lDistance �� ���ְ�, �׸�ŭ �������� �Ѵ�. �׷��� �� ��ǥ�� ���� ���Ѵ�.
	FLOAT fDiv2 = fDiv - (FLOAT)lDistance;
	AuPOS stDelta2;
	if(fDiv != 0.0f)
		AuMath::V3DScale(&stDelta2, &stDelta, fDiv2 / fDiv);
	else
		stDelta2 = stDelta;

	AuMath::V3DAdd(pstPos, &pcsCharacter->m_stPos, &stDelta2);
	AuMath::V3DSubtract(&stDelta, pstPos, &pcsCharacter->m_stPos);

	// �� ��ǥ�� ��������, �ٽ��ѹ� �Ÿ� �����ش�.
	stDelta.y = 0.0f;
	fDiv = AuMath::V3DLength(&stDelta);

	// �̸� ������ Normalized Vector�� ���Ѵ�.
	pcsCharacter->m_stDirection.x = stDelta.x / fDiv;
	pcsCharacter->m_stDirection.y = stDelta.y / fDiv;
	pcsCharacter->m_stDirection.z = stDelta.z / fDiv;

	// ��ǥ ��ġ�� ���µ� �ɸ��� �ð��� ����� ���´�.
	pcsCharacter->m_stDestinationPos = *pstPos;
	pcsCharacter->m_ulStartMoveTime = ulClockCount;
	pcsCharacter->m_ulEndMoveTime = (UINT32) (ulClockCount + fDiv / /*0.4*/ (lMoveSpeed / 10000.0) );		// �ʴ� 4Ÿ���� ���ٰ� �����Ѵ�.

	// �������� �ʰ� �־�����, ���ݺ��� �����̴� �ɷ� (������ ó�� �ð��� ����� �ٲ۴�.
	if (!pcsCharacter->m_bMove)
		pcsCharacter->m_ulPrevProcessTime = ulClockCount;

	//pcsCharacter->m_bMoveFollow	= TRUE;
	pcsCharacter->m_eMoveDirection	= MD_NODIRECTION;
	pcsCharacter->m_bPathFinding	= FALSE		;
	pcsCharacter->m_bMove			= TRUE		;
	pcsCharacter->m_bMoveFast		= TRUE		;
	pcsCharacter->m_bHorizontal		= FALSE		;

	pcsCharacter->m_lAdjustMoveSpeed	= 0;

	if (IsPC(pcsCharacter))
	{
		// ������ (2004-09-17 ���� 5:21:09) : ���ε� ���� ����..
		INT16	nBindingRegion = m_pcsApmMap->GetRegion( pcsCharacter->m_stPos.x , pcsCharacter->m_stPos.z );

		pcsCharacter->m_ulNextRegionRefreshTime	= ulClockCount + GetRegionRefreshInterval();

		if( nBindingRegion >= 0 && pcsCharacter->m_nBindingRegionIndex != nBindingRegion )
		{
			// ������ ���Ѵ�..
			// �ݹ�ȣ��..

			INT16 nPrevRegion = pcsCharacter->m_nBindingRegionIndex;
			pcsCharacter->m_nBindingRegionIndex = nBindingRegion;
			EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pcsCharacter, &nPrevRegion );
		}
	}

	return EnumCallback(AGPMCHAR_CB_ID_MOVE, pcsCharacter, NULL);
}

BOOL AgpmCharacter::MoveCharacterHorizontal(INT32 lCID, AuPOS *pstBasePos, INT32 lDistance, BOOL bExistNextAction)
{
	if (lCID == AP_INVALID_CID || !pstBasePos || lDistance <= 0)
		return FALSE;

	return MoveCharacterHorizontal(GetCharacter(lCID), pstBasePos, lDistance);
}

BOOL AgpmCharacter::MoveCharacterHorizontal(AgpdCharacter *pcsCharacter, AuPOS *pstBasePos, INT32 lDistance, BOOL bExistNextAction)
{
	if (!pcsCharacter || !pstBasePos || lDistance <= 0 || pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_NORMAL)
		return FALSE;

	AuPOS	stDelta;

	if (!bExistNextAction)
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

	UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE);

	// stDelta�� ���̽� ��ġ�� ĳ������ ��ġ���� Vector
	AuMath::V3DSubtract(&stDelta, &pcsCharacter->m_stPos, pstBasePos);

	stDelta.y	= 0;

	AuMath::V3DNormalize(&pcsCharacter->m_stDirection, &stDelta);

	//UINT32	ulClockCount	= GetClockCount();
	UINT32	ulClockCount	= m_ulPrevClockCount;

	AuPOS	posDest;
	posDest.x	= pcsCharacter->m_stDirection.x * lDistance;
	posDest.y	= pcsCharacter->m_stDirection.y * lDistance;
	posDest.z	= pcsCharacter->m_stDirection.z * lDistance;

	// �̸� ������ ��ǥ ��ġ�� ���Ѵ�.
	pcsCharacter->m_stDestinationPos.x += posDest.x;
	pcsCharacter->m_stDestinationPos.y += posDest.y;
	pcsCharacter->m_stDestinationPos.z += posDest.z;

	// ��ǥ ��ġ�� ���µ� �ɸ��� �ð��� ����� ���´�.
	pcsCharacter->m_ulStartMoveTime = ulClockCount;

	// �ʴ� 20Ÿ���� ���ٰ� �����Ѵ�.
	pcsCharacter->m_ulEndMoveTime = (UINT32) (ulClockCount + lDistance / 2.0 );

	// �������� �ʰ� �־�����, ���ݺ��� �����̴� �ɷ� (������ ó�� �ð��� ����� �ٲ۴�.
	if (!pcsCharacter->m_bMove)
		pcsCharacter->m_ulPrevProcessTime = ulClockCount;

	pcsCharacter->m_eMoveDirection	= MD_NODIRECTION	;
	pcsCharacter->m_bPathFinding	= FALSE			;
	pcsCharacter->m_bMove			= TRUE			;
	pcsCharacter->m_bMoveFast		= FALSE			;
	pcsCharacter->m_bHorizontal		= TRUE			;
	pcsCharacter->m_bMoveFollow		= FALSE			;
	pcsCharacter->m_lFollowTargetID = AP_INVALID_CID;
	pcsCharacter->m_ulSyncMoveTime = 0;

	pcsCharacter->m_lAdjustMoveSpeed	= 0;

	if (IsPC(pcsCharacter))
	{
		// ������ (2004-09-17 ���� 5:21:09) : ���ε� ���� ����..
		INT16	nBindingRegion = m_pcsApmMap->GetRegion( pcsCharacter->m_stPos.x , pcsCharacter->m_stPos.z );

		pcsCharacter->m_ulNextRegionRefreshTime	= ulClockCount + GetRegionRefreshInterval();

		if( nBindingRegion >= 0 && pcsCharacter->m_nBindingRegionIndex != nBindingRegion )
		{
			// ������ ���Ѵ�..
			// �ݹ�ȣ��..

			INT16 nPrevRegion = pcsCharacter->m_nBindingRegionIndex;
			pcsCharacter->m_nBindingRegionIndex = nBindingRegion;
			EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pcsCharacter, &nPrevRegion );
		}
	}

	return EnumCallback(AGPMCHAR_CB_ID_MOVE, pcsCharacter, NULL);
}

BOOL AgpmCharacter::TransformCharacter(INT32 lCID, eAgpmCharacterTransformType eType, AgpdCharacterTemplate *pcsTemplate, AgpdFactor *pcsTargetFactor, BOOL bIsCopyFactor)
{
	if (lCID == AP_INVALID_CID)
		return FALSE;

	return TransformCharacter(GetCharacter(lCID), eType, pcsTemplate, pcsTargetFactor, bIsCopyFactor);
}

BOOL AgpmCharacter::TransformCharacter(AgpdCharacter *pcsCharacter, eAgpmCharacterTransformType eType, AgpdCharacterTemplate *pcsTemplate, AgpdFactor *pcsTargetFactor, BOOL bIsCopyFactor)
{
	if (!pcsCharacter || !pcsTemplate)
		return FALSE;

	/*
	if (pcsCharacter->m_bIsTrasform)
		return FALSE;					// ���� ���� �ٸ� Ʈ�������� �������̴�.
	*/

	BOOL	bRetval	= FALSE;

	pcsCharacter->m_bIsTrasform = TRUE;
	pcsCharacter->m_eTransformType = eType;

	switch (eType) {
	case AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY:
		{
			bRetval = TransformCharacterAppear(pcsCharacter, pcsTemplate);
		}
		break;

	case AGPMCHAR_TRANSFORM_TYPE_STATUS_ONLY:
		{
			bRetval = TransformCharacterStatus(pcsCharacter, pcsTemplate, pcsTargetFactor, bIsCopyFactor);
		}
		break;

	case AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL:
		{
			bRetval = TRUE;
			bRetval |= TransformCharacterAppear(pcsCharacter, pcsTemplate);
			bRetval |= TransformCharacterStatus(pcsCharacter, pcsTemplate, pcsTargetFactor, bIsCopyFactor);
		}
		break;
	}

	if (!bRetval)
		pcsCharacter->m_bIsTrasform = FALSE;

	return bRetval;
}

BOOL AgpmCharacter::EvolutionCharacter(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsCharacter || !pcsTemplate)
		return FALSE;

	if( pcsCharacter->m_bIsEvolution == TRUE )
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//
	if (pcsCharacter->m_lTID1 == pcsTemplate->m_lID)
		return FALSE;

	pcsCharacter->m_lTID1					= pcsTemplate->m_lID;
	pcsCharacter->m_pcsCharacterTemplate	= pcsTemplate;
	pcsCharacter->m_bIsEvolution			= TRUE;

	EnumCallback(AGPMCHAR_CB_ID_EVOLUTION, pcsCharacter, pcsTemplate);

	return TRUE;
}

BOOL AgpmCharacter::RestoreTransformCharacter(INT32 lCID)
{
	return RestoreTransformCharacter(GetCharacter(lCID));
}

BOOL AgpmCharacter::RestoreTransformCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (!pcsCharacter->m_bIsTrasform)
		return FALSE;

	INT32	lPrevTID						= pcsCharacter->m_lTID1;

	if (pcsCharacter->m_lOriginalTID != AP_INVALID_CID &&
		pcsCharacter->m_lOriginalTID != pcsCharacter->m_lTID1)
	{
		TransformCharacterAppear(pcsCharacter, GetCharacterTemplate(pcsCharacter->m_lOriginalTID));

		pcsCharacter->m_lTID1			= pcsCharacter->m_lOriginalTID;
		pcsCharacter->m_lOriginalTID	= AP_INVALID_CID;
	}

	if (m_pcsAgpmFactors)
	{
		// ī������ ����Ʈ�� ������ �ƴϹǷ� ���� ���� ���������. arycoat 2008.08.21
		INT32 lCharismaPoint = GetCharismaPoint(pcsCharacter);

		//�Ǵ� ����Ʈ�� ������ �ƴϹǷ� ���� ���� ���������. shootingstart 2009.08.12
		INT32 lMurderPoint = GetMurdererPoint(pcsCharacter);

		m_pcsAgpmFactors->CopyFactorNotNull(&pcsCharacter->m_csFactor, &pcsCharacter->m_csFactorOriginal);
		m_pcsAgpmFactors->DestroyFactor(&pcsCharacter->m_csFactorOriginal);

		// ī������ ����Ʈ�� ������ �ƴϹǷ� ���� ���� ���������. arycoat 2008.08.21
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

		//�Ǵ� ����Ʈ�� ������ �ƴϹǷ� ���� ���� ���������. shootingstart 2009.08.12
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurderPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lMurderPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
	}

	pcsCharacter->m_bIsTrasform = FALSE;
	pcsCharacter->m_lTransformedByCID = AP_INVALID_CID;
	pcsCharacter->m_lTransformItemTID = 0;
	pcsCharacter->m_bCopyTransformFactor = FALSE;

	EnumCallback(AGPMCHAR_CB_ID_RESTORE_TRANSFORM, pcsCharacter, &lPrevTID);

	return TRUE;
}

BOOL AgpmCharacter::RestoreEvolutionCharacter(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsCharacter || !pcsTemplate)
		return FALSE;

	if( pcsCharacter->m_bIsEvolution == FALSE )
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//
	if (pcsCharacter->m_lTID1 == pcsTemplate->m_lID)
		return FALSE;

	pcsCharacter->m_lTID1					= pcsTemplate->m_lID;
	pcsCharacter->m_pcsCharacterTemplate	= pcsTemplate;
	pcsCharacter->m_bIsEvolution			= FALSE;

	EnumCallback(AGPMCHAR_CB_ID_RESTORE_EVOLUTION, pcsCharacter, pcsTemplate);

	return TRUE;
}

BOOL AgpmCharacter::TransformCharacterAppear(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsCharacter || !pcsTemplate)
		return FALSE;

	if (pcsCharacter->m_lTID1 == pcsTemplate->m_lID)
		return FALSE;

	if (pcsCharacter->m_lOriginalTID == AP_INVALID_CID)
		pcsCharacter->m_lOriginalTID			= pcsCharacter->m_lTID1;

	pcsCharacter->m_lTID1					= pcsTemplate->m_lID;
	pcsCharacter->m_pcsCharacterTemplate	= pcsTemplate;

	EnumCallback(AGPMCHAR_CB_ID_TRANSFORM_APPEAR, pcsCharacter, &pcsTemplate->m_lID);

	return TRUE;
}

BOOL AgpmCharacter::TransformCharacterStatus(AgpdCharacter *pcsCharacter, AgpdCharacterTemplate *pcsTemplate, AgpdFactor *pcsTargetFactor, BOOL bIsCopyFactor)
{
	if (!pcsCharacter || !pcsTemplate || !pcsTargetFactor)
		return FALSE;

	if (m_pcsAgpmFactors)
	{
		// first, backup original factor
		m_pcsAgpmFactors->InitFactor(&pcsCharacter->m_csFactorOriginal);

		m_pcsAgpmFactors->CopyFactor(&pcsCharacter->m_csFactorOriginal, &pcsCharacter->m_csFactor, FALSE, FALSE);

		// second, change pcsCharacter factor to pcsTemplate factor

		// copy char status, damage, defense, attack factor
		if (bIsCopyFactor)
		{
			INT32	lLevel	= GetLevel(pcsCharacter);
			// ī������ ����Ʈ�� ������ �ƴϹǷ� ���� ���� ���������. arycoat 2008.08.21
			INT32 lCharismaPoint = GetCharismaPoint(pcsCharacter);

			m_pcsAgpmFactors->CopyFactorNotNull(&pcsCharacter->m_csFactor, pcsTargetFactor);

			m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
			// ī������ ����Ʈ�� ������ �ƴϹǷ� ���� ���� ���������. arycoat 2008.08.21
			m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
			m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, lCharismaPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
		}
		else
			m_pcsAgpmFactors->CalcFactor(&pcsCharacter->m_csFactor, pcsTargetFactor, FALSE, FALSE, TRUE);
	}

	// call Callback Functions
	EnumCallback(AGPMCHAR_CB_ID_TRANSFORM_STATUS, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgpmCharacter::StopCharacter(INT32 lCID, AuPOS *pstPos)
{
	return StopCharacter(GetCharacter(lCID), pstPos);
}

BOOL AgpmCharacter::StopCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstPos)
{
	PROFILE("AgpmCharacter::StopCharacter");

	// Validation Check
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE)
		UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NOT_ACTION);

	if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD)
		return FALSE;

	if (pstPos)
	{
		// ��ġ�� ������, UpdatePosition()�� �ҷ��� ��ġ�� �����Ѵ�.
		if (!UpdatePosition(pcsCharacter, pstPos))
		{
			return FALSE;
		}
	}

	pcsCharacter->m_bWasMoveFollow	= pcsCharacter->m_bMoveFollow;

	pcsCharacter->m_bSync	= pcsCharacter->m_eMoveDirection || pcsCharacter->m_bSync;

	// ���������� ������ �ð� �ְ�, ������ Flag �� Setting
	pcsCharacter->m_bMove			= FALSE;
	pcsCharacter->m_bMoveFast		= FALSE;
	pcsCharacter->m_eMoveDirection	= MD_NODIRECTION;
	pcsCharacter->m_bPathFinding	= FALSE;
	pcsCharacter->m_bHorizontal		= FALSE;
	pcsCharacter->m_bMoveFollow		= FALSE;

	pcsCharacter->m_lFollowTargetID = AP_INVALID_CID;
	pcsCharacter->m_ulSyncMoveTime = 0;

	pcsCharacter->m_lAdjustMoveSpeed	= 0;

	if (IsPC(pcsCharacter))
	{
		// ������ (2004-09-17 ���� 5:21:09) : ���ε� ���� ����..
		INT16	nBindingRegion = m_pcsApmMap->GetRegion( pcsCharacter->m_stPos.x , pcsCharacter->m_stPos.z );

		pcsCharacter->m_ulNextRegionRefreshTime	= GetClockCount() + GetRegionRefreshInterval();

		if( nBindingRegion >= 0 && pcsCharacter->m_nBindingRegionIndex != nBindingRegion )
		{
			// ������ ���Ѵ�..
			// �ݹ�ȣ��..

			INT16 nPrevRegion = pcsCharacter->m_nBindingRegionIndex;
			pcsCharacter->m_nBindingRegionIndex = nBindingRegion;
			EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pcsCharacter, &nPrevRegion );
		}
	}

	// ���� ������ ����Ǿ� �ִٸ�...
	if (pcsCharacter->m_stNextAction.m_eActionType != AGPDCHAR_ACTION_TYPE_NONE)
	{
		AgpdCharacterAction		stAction = pcsCharacter->m_stNextAction;

		// ���� Action�� �Ѵ�.
		if (!ActionCharacter(stAction.m_eActionType, pcsCharacter, (ApBase *) &stAction.m_csTargetBase, &stAction.m_stTargetPos, stAction.m_bForceAction, (INT8) stAction.m_lUserData[0], (AgpdFactor *) stAction.m_lUserData[1], (INT8) stAction.m_lUserData[2]))
			EnumCallback(AGPMCHAR_CB_ID_STOP, pcsCharacter, pstPos);
	}
	else
	{
		EnumCallback(AGPMCHAR_CB_ID_STOP, pcsCharacter, pstPos);
	}

	// Sync�� �����.
	pcsCharacter->m_bSync = FALSE;

	return TRUE;
}

BOOL AgpmCharacter::TurnCharacter(INT32 lCID, FLOAT fDegreeX, FLOAT fDegreeY)
{
	return TurnCharacter(GetCharacter(lCID), fDegreeX, fDegreeY);
}

BOOL AgpmCharacter::TurnCharacter(AgpdCharacter *pcsCharacter, FLOAT fDegreeX, FLOAT fDegreeY)
{
	PROFILE("AgpmCharacter::TurnCharacter");

	// Validation Check
	if (!pcsCharacter)
		return FALSE;

	if (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsCharacter->m_ulSpecialStatus)
		return TRUE;

	/*
	if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD && pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_LOGIN_PROCESS )
		return FALSE;
	*/

	// 2004.08.25. steeple
	// stun ����
	//if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return FALSE;

	// 2007.06.27. steeple
	if(IsStatusSleep(pcsCharacter))
		return FALSE;

	//if (!CheckActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE))
	//	return FALSE;

	if (pcsCharacter->m_fTurnX == fDegreeX &&
		pcsCharacter->m_fTurnY == fDegreeY)
		return FALSE;

	//UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE);

	// ���� �ְ�
	pcsCharacter->m_fTurnX = fDegreeX;
	pcsCharacter->m_fTurnY = fDegreeY;

	// ������ ���� Matrix ����� ����
	AuMath::MatrixIdentity(&pcsCharacter->m_stMatrix);
	AuMath::MatrixRotateX(&pcsCharacter->m_stMatrix, fDegreeX);
	AuMath::MatrixRotateY(&pcsCharacter->m_stMatrix, fDegreeY);

	// UpdatePosition()���� ��ġ ���� (�����δ� ������ ����)
	if (!UpdatePosition(pcsCharacter, NULL, FALSE))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgpmCharacter::MoveTurnCharacter(INT32 lCID, AuPOS *pstPos, FLOAT fDegreeX, FLOAT fDegreeY, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding, BOOL bMoveFast, BOOL bExistNextAction)
{
	return MoveTurnCharacter(GetCharacter(lCID), pstPos, fDegreeX, fDegreeY, eMoveDirection, bPathFinding, bMoveFast);
}

BOOL AgpmCharacter::MoveTurnCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstPos, FLOAT fDegreeX, FLOAT fDegreeY, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding, BOOL bMoveFast, BOOL bExistNextAction)
{
	PROFILE("AgpmCharacter::MoveTurnCharacter");

	FLOAT	fDiv;
	AuPOS	stDelta;

	// Validation Check
	if (!pcsCharacter || !pstPos)
		return FALSE;

	if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD)
		return FALSE;

	if (!CheckActionStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE))
		return FALSE;

	// 2004.08.25. steeple
	// stun ����
	//if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return FALSE;

	// 2007.06.27. steeple
	if(IsStatusSleep(pcsCharacter))
		return FALSE;

	if (!bExistNextAction)
		pcsCharacter->m_stNextAction.m_eActionType = AGPDCHAR_ACTION_TYPE_NONE;

	CheckLockAction(pcsCharacter);

	UpdateStatus(pcsCharacter, AGPDCHAR_STATUS_MOVE);

	// ��ġ�� ������, Turn�� ����
	if (0.0f == AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos,*pstPos) && !eMoveDirection)
	{
		StopCharacter(pcsCharacter, NULL);
		//pcsCharacter->m_bMove = FALSE;
		return TurnCharacter(pcsCharacter, fDegreeX, fDegreeY);
	}

	//UINT32	ulClockCount	= GetClockCount();
	UINT32	ulClockCount	= m_ulPrevClockCount;

	if (eMoveDirection)
	{
		// �������� �������̶��, pstPos�� ���� Vector�̹Ƿ�, �̸� Normalize�ؼ� Direction�� �ִ´�.
		AuMath::V3DNormalize(&pcsCharacter->m_stDirection, pstPos);
	}
	else
	{
		// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
		AuMath::V3DSubtract(&stDelta, pstPos, &pcsCharacter->m_stPos);

		stDelta.y	= 0;

		// fDiv�� �� �Ÿ�
		fDiv = AuMath::V3DLength(&stDelta);

		// �̸� ������ Normalized Vector�� ���Ѵ�.
		pcsCharacter->m_stDirection.x = stDelta.x / fDiv;
		pcsCharacter->m_stDirection.y = stDelta.y / fDiv;
		pcsCharacter->m_stDirection.z = stDelta.z / fDiv;

		// ��ǥ ��ġ�� ���µ� �ɸ��� �ð��� ����� ���´�.
		pcsCharacter->m_stDestinationPos = *pstPos;
		pcsCharacter->m_ulStartMoveTime = ulClockCount;

		INT32	lMoveSpeed	= 0;
		if (bMoveFast)
			lMoveSpeed = GetFastMoveSpeed(pcsCharacter);
		else
			lMoveSpeed = GetMoveSpeed(pcsCharacter);

		if (lMoveSpeed == 0)
			lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED;

		pcsCharacter->m_ulEndMoveTime = (UINT32) (ulClockCount + fDiv / /*0.4*/ (lMoveSpeed / 10000.0) );		// �ʴ� 4Ÿ���� ���ٰ� �����Ѵ�.
		//if (bMoveFast)
		//	pcsCharacter->m_ulEndMoveTime -= (UINT32) ((fDiv / (lMoveSpeed / 10.0)) * (2.0 / 3.0));
	}

	// �������� �ʰ� �־�����, ���ݺ��� �����̴� �ɷ� (������ ó�� �ð��� ����� �ٲ۴�.
	if (!pcsCharacter->m_bMove)
		pcsCharacter->m_ulPrevProcessTime = ulClockCount;
	
	pcsCharacter->m_eMoveDirection = eMoveDirection;
	pcsCharacter->m_bPathFinding = bPathFinding;
	pcsCharacter->m_bMove = TRUE;
	pcsCharacter->m_bMoveFast = bMoveFast;
	pcsCharacter->m_bMoveFollow = FALSE;
	pcsCharacter->m_lFollowTargetID = AP_INVALID_CID;
	pcsCharacter->m_ulSyncMoveTime = 0;

	pcsCharacter->m_lAdjustMoveSpeed	= 0;

	// ���� �ְ�
	pcsCharacter->m_fTurnX = fDegreeX;
	pcsCharacter->m_fTurnY = fDegreeY;

	// ������ ���� Matrix ����� ����
	AuMath::MatrixIdentity(&pcsCharacter->m_stMatrix);
	AuMath::MatrixRotateX(&pcsCharacter->m_stMatrix, fDegreeX);
	AuMath::MatrixRotateY(&pcsCharacter->m_stMatrix, fDegreeY);

	if (IsPC(pcsCharacter))
	{
		// ������ (2004-09-17 ���� 5:21:09) : ���ε� ���� ����..
		INT16	nBindingRegion = m_pcsApmMap->GetRegion( pcsCharacter->m_stPos.x , pcsCharacter->m_stPos.z );

		pcsCharacter->m_ulNextRegionRefreshTime	= ulClockCount + GetRegionRefreshInterval();

		if( nBindingRegion >= 0 && pcsCharacter->m_nBindingRegionIndex != nBindingRegion )
		{
			// ������ ���Ѵ�..
			// �ݹ�ȣ��..

			INT16 nPrevRegion = pcsCharacter->m_nBindingRegionIndex;
			pcsCharacter->m_nBindingRegionIndex = nBindingRegion;
			EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pcsCharacter, &nPrevRegion );
		}
	}

	// Enumerate Callbacks
	return EnumCallback(AGPMCHAR_CB_ID_MOVE, pcsCharacter, NULL);
}

BOOL	AgpmCharacter::SyncActionPosition(AgpdCharacterActionType eAction, AgpdCharacter *pcsCharacter, ApBase *pcsTargetBase, AuPOS *pstActionPosition)
{
	if (!pcsCharacter || !pcsTargetBase || !pstActionPosition)
		return TRUE;

	if (pstActionPosition->x == 0 &&
		pstActionPosition->y == 0 &&
		pstActionPosition->z == 0)
		return TRUE;

	if (m_pcsAgpmFactors &&
		!m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &((AgpdCharacter *) pcsTargetBase)->m_stPos, &pcsCharacter->m_csFactor, &((AgpdCharacter *) pcsTargetBase)->m_csFactor, NULL, NULL))
		return FALSE;

	return TRUE;
}

BOOL	AgpmCharacter::ActionCharacter(AgpdCharacterActionType eAction, INT32 lCID, ApBase *pcsTargetBase, AuPOS *pstTargetPos, BOOL bForce, INT8 cActionResultType, AgpdFactor *pcsFactorDamage, UINT8 cComboInfo, UINT32 ulAdditionalEffect, UINT8 cHitIndex, INT32 lSkillTID)
{
	return ActionCharacter(eAction, GetCharacter(lCID), pcsTargetBase, pstTargetPos, bForce, cActionResultType, pcsFactorDamage, cComboInfo, ulAdditionalEffect, cHitIndex, lSkillTID);
}

BOOL	AgpmCharacter::ActionCharacter(AgpdCharacterActionType eAction, AgpdCharacter *pcsCharacter, ApBase *pcsTargetBase, AuPOS *pstTargetPos, BOOL bForce, INT8 cActionResultType, AgpdFactor *pcsFactorDamage, UINT8 cComboInfo, UINT32 ulAdditionalEffect, UINT8 cHitIndex, INT32 lSkillTID)
{
	PROFILE("AgpmCharacter::ActionCharacter");

	BOOL	bRet = TRUE;

	if (eAction == AGPDCHAR_ACTION_TYPE_NONE)
		return TRUE;

	if (!pcsCharacter)
		return FALSE;

	// 2004.08.25. steeple
	// stun ����
	//if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return FALSE;

	// 2007.06.27. steeple
	if(IsStatusSleep(pcsCharacter))
		return FALSE;

	if (IsAllBlockStatus(pcsCharacter))
		return FALSE;

	// eAction�� AGPDCHAR_ACTION_TYPE_SKILL�� ��� Target Base ������ ���
	AgpdCharacterAction	stActionBackup;
	//stActionBackup.m_csTargetBase.m_Mutex.Destroy();
	ZeroMemory(&stActionBackup, sizeof(stActionBackup));

	if (eAction == AGPDCHAR_ACTION_TYPE_SKILL				||
		eAction == AGPDCHAR_ACTION_TYPE_PICKUP_ITEM			||
		eAction == AGPDCHAR_ACTION_TYPE_EVENT_TELEPORT		||
		eAction == AGPDCHAR_ACTION_TYPE_EVENT_NPC_TRADE		||
		eAction == AGPDCHAR_ACTION_TYPE_EVENT_NPC_DIALOG	||
		eAction == AGPDCHAR_ACTION_TYPE_EVENT_QUEST			||
		eAction == AGPDCHAR_ACTION_TYPE_PRODUCT_SKILL		||
		eAction == AGPMCHAR_CB_ID_ACTION_EVENT_CHARCUSTOMIZE||
		eAction == AGPDCHAR_ACTION_TYPE_EVENT_ITEMREPAIR	||
		eAction == AGPDCHAR_ACTION_TYPE_EVENT_GACHA			)
		CopyMemory(&stActionBackup, &pcsCharacter->m_stNextAction, sizeof(AgpdCharacterAction));

	// ���� Action�� Reset
	memset(&pcsCharacter->m_stNextAction, 0, sizeof(AgpdCharacterAction));

	switch (eAction)
	{
	case AGPDCHAR_ACTION_TYPE_ATTACK:
		{
			PROFILE("AgpmCharacter::ActionCharacter(Attack)");

			// Before Process Attack Packet Check Attack disturbing
			if(CheckEnableActionCharacter(pcsCharacter, AGPDCHAR_DISTURB_ACTION_ATTACK) == FALSE)
			{
				return FALSE;
			}

			AuPOS			stPos;
			AgpdCharacter *	pcsTargetCharacter;

			// 2005.01.27. steeple
			// ���� �������Ƿ� bForce �� �ѱ��!!!
			// ���� PK�� ���� ���� ������ ���� �ʴ�. �ϴ� ���� ������ ���´�.
			//bForce	= FALSE;

			//printf(" ***** ActionCharacter() : called by AGPDCHAR_ACTION_TYPE_ATTACK type (CID : %d)\n", pcsCharacter->m_lID);

			if (!CheckActionStatus(pcsCharacter, AGPDCHAR_STATUS_ATTACK))
				return FALSE;

			static AgpmSiegeWar* s_pcsAgpmSiegeWar = (AgpmSiegeWar *)GetModule("AgpmSiegeWar");
			if( s_pcsAgpmSiegeWar )
			{
				AgpdSiegeWarMonsterType	eMonsterType = s_pcsAgpmSiegeWar->GetSiegeWarMonsterType( pcsCharacter );
				if( AGPD_SIEGE_MONSTER_CATAPULT == eMonsterType && !pcsTargetBase )
					pcsTargetBase = pcsCharacter;
			}

			if (!pcsTargetBase)
				return FALSE;

			// Target�� Character���� Ȯ��
			if (pcsTargetBase->m_eType != APBASE_TYPE_CHARACTER)
				return FALSE;

			// 2005.05.16. steeple
			// Result �� ���� ���õǼ� �°Ŷ�� Result üũ�غ���.
			//if(cActionResultType & AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK)
			//	return FALSE;

			if (pcsCharacter->m_bRidable)
			{
				if (FALSE == EnumCallback(AGPMCHAR_CB_ID_GET_ITEM_LANCER, pcsCharacter, NULL))
					return FALSE;
			}

			// Target Character�� �����´�.
			pcsTargetCharacter = GetCharacterLock(pcsTargetBase->m_lID);
			if (!pcsTargetCharacter)
				return FALSE;

			if (IsAllBlockStatus(pcsTargetCharacter))
			{
				pcsTargetCharacter->m_Mutex.Release();
				return FALSE;
			}

			stPos = pcsTargetCharacter->m_stPos;

			// TargetCID Setting
			memset(pcsCharacter->m_alTargetCID, 0, sizeof (INT32) * AGPDCHARACTER_MAX_TARGET);
			pcsCharacter->m_alTargetCID[0] = pcsTargetCharacter->m_lID;

			// ���°� Normal�̾�� �ȴ�.
			if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
				pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
			{
				pcsTargetCharacter->m_Mutex.Release();
				return FALSE;
			}

			BOOL	bIsValidAttack	= TRUE;

			PVOID	pvBuffer[4];
			pvBuffer[0]	= pcsTargetCharacter;
			pvBuffer[1]	= IntToPtr((INT32)bForce);
			pvBuffer[2]	= (PVOID) &bIsValidAttack;
			
			EnumCallback(AGPMCHAR_CB_ID_CHECK_ACTION_ATTACK_TARGET, pcsCharacter, pvBuffer);

			if (!bIsValidAttack)
			{
				pcsTargetCharacter->m_Mutex.Release();
				return FALSE;
			}

			/*
			if (!CheckVaildNormalAttackTarget(pcsCharacter, pcsTargetCharacter, bForce))
				return FALSE;
			*/

			// �ش� �������� Ʋ��
			BOOL bTurnChar = FALSE;
			if (FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsCharacter->m_ulSpecialStatus))
			{
				bTurnChar = TurnCharacter( pcsCharacter, 0, GetSelfCharacterTurnAngle ( pcsCharacter, &(pcsTargetCharacter->m_stPos) ) );
			}

			// Action Move�� �ؾ��ϴ� ������ �˻��Ѵ�.
			BOOL bIsActionMove	= TRUE;
			EnumCallback(AGPMCHAR_CB_ID_CHECK_IS_ACTION_MOVE, pcsCharacter, &bIsActionMove);

			INT32	lRange			= 0;
			INT32	lTargetRange	= 0;

			if (bIsActionMove &&
				m_pcsAgpmFactors &&
				!m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &pcsTargetCharacter->m_stPos, &pcsCharacter->m_csFactor, &pcsTargetCharacter->m_csFactor, &stPos, &lRange, &lTargetRange))
			{
				// �ʹ� �ִٸ� �׳� ������ ������. (���� 100����)
				if (lTargetRange > 10000)
				{
					pcsTargetCharacter->m_Mutex.Release();
					return FALSE;
				}

				//printf(" ***** ActionCharacter() : IsInRange() return FALSE (CID : %d)\n", pcsCharacter->m_lID);

				// Range�ۿ� �ִٸ�, ���� Action�� �����ϰ�, Boudary�� �̵��ϵ��� �Ѵ�.
				pcsCharacter->m_stNextAction.m_bForceAction = bForce;
				pcsCharacter->m_stNextAction.m_eActionType = eAction;
				pcsCharacter->m_stNextAction.m_csTargetBase.m_eType = pcsTargetCharacter->m_eType;
				pcsCharacter->m_stNextAction.m_csTargetBase.m_lID = pcsTargetCharacter->m_lID;
				pcsCharacter->m_stNextAction.m_lUserData[0] = cActionResultType;
				pcsCharacter->m_stNextAction.m_lUserData[1] = (INT_PTR) pcsFactorDamage;
				pcsCharacter->m_stNextAction.m_lUserData[2] = (INT_PTR) cComboInfo;
				pcsCharacter->m_stNextAction.m_lUserData[3] = (INT32) cHitIndex;
				pcsCharacter->m_stNextAction.m_lUserData[3] = (INT32) cHitIndex;
				pcsCharacter->m_stNextAction.m_ulAdditionalEffect = 0;	//ulAdditionalEffect; // ��� ������ �ʿ䰡 ����.

				BOOL	bIsPlayerCharacter	= TRUE;

				if (!IsPC(pcsCharacter))
				{
					EnumCallback(AGPMCHAR_CB_ID_IS_PLAYER_CHARACTER, pcsCharacter, &bIsPlayerCharacter);
				}

				//���Ͷ��?
				if (!bIsPlayerCharacter)
				{
					PROFILE("AgpmCharacter::ActionCharacter(AI Pathfind)");

					//���� ��ġ�� ��� MoveCharacter�� �Ѵ�.
					if( !EnumCallback( AGPMCHAR_CB_ID_MONSTER_AI_PATHFIND, pcsCharacter, pcsTargetCharacter ) )
					{
						//���� ��尡 ���ٸ�~ ��Ȯ�� �������� ������ ����̴�.
						//StopCharacter(pcsCharacter, NULL);
					}
				}
				//PC���?
				else
				{
					bRet = MoveCharacterFollow(pcsCharacter, pcsTargetCharacter, lRange, TRUE);
				}
			}
			else
			{
				//printf(" ***** ActionCharacter() : IsInRange() return TRUE (CID : %d)\n", pcsCharacter->m_lID);
				// Range�ȿ� ������, Stop�ϰ� Enumerate Callback
				//if (bTurnChar)
				if (pcsCharacter->m_bMove)
					StopCharacter(pcsCharacter, NULL);

				PVOID	pvBuffer[7];
				pvBuffer[0]	= pcsTargetCharacter;

				if (cActionResultType == (-1))
					pvBuffer[1]	= (PVOID) stActionBackup.m_lUserData[0];
				else
					pvBuffer[1] = (PVOID) cActionResultType;

				if (!pcsFactorDamage)
					pvBuffer[2] = (PVOID) stActionBackup.m_lUserData[1];
				else
					pvBuffer[2] = (PVOID) pcsFactorDamage;

				if (cComboInfo != 0)
					pvBuffer[3]	= (PVOID) cComboInfo;
				else
					pvBuffer[3] = (PVOID) stActionBackup.m_lUserData[2];

				pvBuffer[4] = IntToPtr((INT32)bForce);
				pvBuffer[5] = UintToPtr(ulAdditionalEffect);
				pvBuffer[6] = (PVOID) cHitIndex;
				pvBuffer[6] = (PVOID) cHitIndex;

				bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_ATTACK, pcsCharacter, pvBuffer);

				UINT32	ulClockCount	= GetClockCount();
				SetCombatMode(pcsCharacter, ulClockCount);
				SetCombatMode(pcsTargetCharacter, ulClockCount);
			}

			pcsTargetCharacter->m_Mutex.Release();
		}

		break;

		/*
	case AGPDCHAR_ACTION_TYPE_ATTACK_MISS:
		{
			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_ATTACK_MISS, NULL, pcsCharacter);
		}

		break;
		*/

	case AGPDCHAR_ACTION_TYPE_SKILL:
		{
			PROFILE("AgpmCharacter::ActionCharacter(Skill)");

			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_SKILL, pcsCharacter, &stActionBackup);
		}

		break;

	case AGPDCHAR_ACTION_TYPE_PICKUP_ITEM:
		{
			PROFILE("AgpmCharacter::ActionCharacter(PickupItem)");

			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_PICKUP_ITEM, pcsCharacter, &stActionBackup);
		}

		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_TELEPORT:
		{
			PROFILE("AgpmCharacter::ActionCharacter(Teleport)");

			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_TELEPORT, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_NPC_TRADE:
		{
			PROFILE("AgpmCharacter::ActionCharacter(NPC Trade)");

			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_NPC_TRADE, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_MASTERY_SPECIALIZE:
		{
			PROFILE("AgpmCharacter::ActionCharacter(MasterySpecialize)");

			StopCharacter(pcsCharacter, NULL);

			//bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_MASTERY_SPECIALIZE, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_BANK:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_BANK, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_ITEMCONVERT:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_ITEMCONVERT, pcsCharacter, &stActionBackup);
		}
		break;
		
	case AGPDCHAR_ACTION_TYPE_EVENT_GUILD:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMHCAR_CB_ID_ACTION_EVENT_GUILD, pcsCharacter, &stActionBackup);
		}
		break;
	
	case AGPDCHAR_ACTION_TYPE_EVENT_GUILD_WAREHOUSE:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMHCAR_CB_ID_ACTION_EVENT_GUILD_WAREHOUSE, pcsCharacter, &stActionBackup);
		}
		break;
		
	case AGPDCHAR_ACTION_TYPE_EVENT_PRODUCT:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_PRODUCT, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_NPC_DIALOG:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_NPC_DIALOG, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_SKILL_MASTER:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_SKILL_MASTER, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_REFINERY:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_REFINERY, pcsCharacter, &stActionBackup);
		}
		break;
		
	case AGPDCHAR_ACTION_TYPE_EVENT_QUEST:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_QUEST, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_PRODUCT_SKILL:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_PRODUCT_SKILL, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPMCHAR_CB_ID_ACTION_EVENT_CHARCUSTOMIZE:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_CHARCUSTOMIZE, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_ITEMREPAIR:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_ITEMREPAIR, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_REMISSION:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_REMISSION, pcsCharacter, &stActionBackup);
		}
		break;
		
	case AGPDCHAR_ACTION_TYPE_EVENT_WANTEDCRIMINAL:
		{
			StopCharacter(pcsCharacter, NULL);
			
			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_WANTEDCRIMINAL, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_ARCHLORD:
		{
			StopCharacter(pcsCharacter, NULL);
			
			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_ARCHLORD, pcsCharacter, &stActionBackup);
		}
		break;
		
	case AGPDCHAR_ACTION_TYPE_EVENT_SIEGE_WAR:
		{
			StopCharacter(pcsCharacter, NULL);
			
			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_SIEGE_WAR, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_TAX:
		{
			StopCharacter(pcsCharacter, NULL);
			
			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_TAX, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_GAMBLE:
		{
			StopCharacter(pcsCharacter, NULL);
			
			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_GAMBLE, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_EVENT_GACHA:
		{
			StopCharacter(pcsCharacter, NULL);
			
			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_EVENT_GACHA, pcsCharacter, &stActionBackup);
		}
		break;
	case AGPDCHAR_ACTION_TYPE_USE_SIEGEWAR_ATTACK_OBJECT:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_USE_SIEGEWAR_ATTACK_OBJECT, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_CARVE_A_SEAL:
		{
			StopCharacter(pcsCharacter, NULL);

			bRet = EnumCallback(AGPMCHAR_CB_ID_ACTION_CARVE_A_SEAL, pcsCharacter, &stActionBackup);
		}
		break;

	case AGPDCHAR_ACTION_TYPE_USE_EFFECT:
		{
			stActionBackup.m_ulAdditionalEffect = ulAdditionalEffect;	// 2007.07.08. steeple
			stActionBackup.m_lUserData[0] = lSkillTID;					// 2007.09.14. steeple	ItemDataTable �� �ִ� ExtraType �̴�.
			bRet = EnumCallback(AGPMCHAR_CB_ID_USE_EFFECT, pcsCharacter, &stActionBackup);
		}
		break;
	}

	return bRet;
}

/*****************************************************************
*   Function : GetSelfCharacterTurnAngle
*   Comment  : Get Self Character Turn Angle For Destination Pos
*   Date&Time : 2003-02-03, ���� 3:00
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
float AgpmCharacter::GetSelfCharacterTurnAngle( AgpdCharacter *pcsCharacter, AuPOS* pDestination )
{
	AuPOS	posSelfDirection;
	float	fTurnY = 0;

	posSelfDirection.x = pDestination->x - pcsCharacter->m_stPos.x;
	posSelfDirection.y = 0.0f;
	posSelfDirection.z = pDestination->z - pcsCharacter->m_stPos.z;

	// Get Angle
	if ( posSelfDirection.z == 0.0f )
	{
		if ( posSelfDirection.x > 0.0f )			fTurnY = 90.0f;
		else if( posSelfDirection.x < 0.0f )		fTurnY = -90.0f;
	}
	else if ( posSelfDirection.x == 0.0f )
	{
		if ( posSelfDirection.z > 0.0f )			fTurnY = 0.0f;
		else if( posSelfDirection.z < 0.0f )		fTurnY = 180.0f;
	}
	else if ( posSelfDirection.z > 0.0f )
	{
		fTurnY = (FLOAT) atan((double)( posSelfDirection.x / posSelfDirection.z )) * 180.0f / 3.141592f;
	}
	else if ( posSelfDirection.z < 0.0f )
	{
		fTurnY = (FLOAT) atan((double)( posSelfDirection.x / posSelfDirection.z )) * 180.0f / 3.141592f;

		if ( posSelfDirection.x > 0.0f ) 
			fTurnY = 180.0f + fTurnY; 
		else 
			fTurnY = fTurnY - 180.0f; 
	}

	return fTurnY;
}

FLOAT AgpmCharacter::GetWorldDirection(FLOAT fX, FLOAT fZ)
{
	//@{ 2006/05/11 burumal
	if ( fZ == 0.0f )
		return 0;
	//@}

	FLOAT	fArc	= (FLOAT) atan((double)(fX / fZ));

	while (fArc < 0) fArc += 3.141592f * 2;

	if (fX < 0 && fZ < 0)
		fArc	+= 3.141592f;
	if (fX > 0 && fZ < 0)
		fArc	-= 3.141592f;

	return	(360.0f / 3.141592f / 2.0f * fArc);
}

BOOL AgpmCharacter::IsSufficientCost(AgpdCharacter *pcsCharacter, AgpdCharacterActionType eAction)
{
	if (!pcsCharacter)
		return FALSE;

//	switch (eAction) {
//	case AGPDCHAR_ACTION_TYPE_ATTACK:
//		{
//			INT32		lClass	= (INT32) AUCHARCLASS_TYPE_NONE;
//			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);
//
//			// �������� ��� MP�� �Ҹ���� �ش�.
//			if (lClass == AUCHARCLASS_TYPE_MAGE)
//			{
//				INT32	lCost	= GetMeleeActionCostMP(pcsCharacter);
//				if (lCost < 0)
//					return FALSE;
//				else if (lCost == 0)
//					return TRUE;
//
//				INT32	lCurrentMP	= 0;
//
//				m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
//
//				if (lCurrentMP < lCost)
//					return FALSE;
//			}
//		}
//		break;
//	}

	return TRUE;
}

INT32 AgpmCharacter::GetMeleeActionCostMP(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return (-1);

	INT32		lClass	= (INT32) AUCHARCLASS_TYPE_NONE;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	// �������� ��� MP�� �Ҹ���� �ش�.
	if (lClass == AUCHARCLASS_TYPE_MAGE)
	{
		// 2005.11.30. steeple
		// �Ҹ� 2 �� ����!!!!
		return 2;

		//// �Ҹ� : (Max_MP  * 0.001) + 2

		//INT32	lMaxMP		= 0;

		//INT32	lLevel		= GetLevel(pcsCharacter);

		//m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

		//return (INT32) ((lMaxMP * 0.001) + 2);
	}
	else
		return 0;

	return (-1);
}

BOOL AgpmCharacter::CheckLockAction(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !pcsCharacter->m_bIsActionMoveLock)
		return FALSE;

	if (pcsCharacter->m_stLockAction.m_eActionType != pcsCharacter->m_stNextAction.m_eActionType ||
		pcsCharacter->m_stLockAction.m_csTargetBase.m_eType != pcsCharacter->m_stNextAction.m_csTargetBase.m_eType ||
		pcsCharacter->m_stLockAction.m_csTargetBase.m_lID != pcsCharacter->m_stNextAction.m_csTargetBase.m_lID ||
		(pcsCharacter->m_stLockAction.m_eActionType == AGPDCHAR_ACTION_TYPE_SKILL &&
		 pcsCharacter->m_stLockAction.m_lUserData[0] != pcsCharacter->m_stNextAction.m_lUserData[0]))
	{
		EnumCallback(AGPMCHAR_CB_ID_RELEASE_ACTION_MOVE_LOCK, pcsCharacter, NULL);

		pcsCharacter->m_bIsActionMoveLock	= FALSE;
		ZeroMemory(&pcsCharacter->m_stLockAction, sizeof(AgpdCharacterAction));
	}

	return TRUE;
}