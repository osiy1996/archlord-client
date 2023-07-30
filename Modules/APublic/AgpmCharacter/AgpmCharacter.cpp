
#include <AgpmCharacter/AgpmCharacter.h>
#include <AuTickCounter/AuTickCounter.h>
#include <AuMath/AuMath.h>
#include <ApBase/ApLockManager.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AgpmCharacter/ApAutoLockCharacter.h>
#include <AgpmSiegeWar/AgpmSiegeWar.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmCharacter/AgppCharacter.h>
#include <time.h>
#include <AgpmTitle/AgpdTitle.h>
#include <AgpmTitle/AgpdTitleQuest.h>

//INT32					g_llLevelUpExp[AGPMCHAR_MAX_LEVEL + 1];		// ĳ���Ͱ� level up �ϴµ� �ʿ��� exp
ApSafeArray<INT64, AGPMCHAR_MAX_LEVEL + 1>	g_llLevelUpExp;

UINT32		g_ulReserveCharacterData		= 8000;				// msec

LONG		g_lNumCharacter					= 0;
BOOL		g_bPrintRemovePoolTID			= 0;
LONG		g_lMaxRemovePoolCount			= 0;

const int	AGPMCHARACTER_MAX_FILTER_TEXT	= 32;

/* -------------------------------------------------------- */
/*
 * ApAutoLockCharacter implementation.
 * Interface is defined in ApAutoLockCharacter.h header file.
 */

ApAutoLockCharacter::ApAutoLockCharacter(AgpmCharacter *pcsAgpmCharacter, INT32 lCID) 
	: m_pcsAgpdCharacter( NULL )
{
	m_pcsAgpdCharacter = pcsAgpmCharacter->GetCharacterLock( lCID );
}

ApAutoLockCharacter::ApAutoLockCharacter(AgpmCharacter *pcsAgpmCharacter, CHAR *szGameID) 
	: m_pcsAgpdCharacter( NULL )
{
	m_pcsAgpdCharacter = pcsAgpmCharacter->GetCharacterLock( szGameID );
}

ApAutoLockCharacter::~ApAutoLockCharacter()
{
	if (m_pcsAgpdCharacter) m_pcsAgpdCharacter->m_Mutex.Release();
}

/* -------------------------------------------------------- */

BOOL AuBoxCollision::CollBox( AuPOS *pStart , AuPOS *pVelocity , float fRadius , AuPOS * pvCollPoint )
{
	BOOL bCollision = FALSE;
	AuPOS vColPoint;

	float fMaxDistance = pVelocity->LengthXZ();
	float fNearestDistance = fMaxDistance;

	for( int i = 0 ; i < 4 ; i ++ )
	{
		float fDotN1V = ( v[i].x * pVelocity->x + v[i].z * pVelocity->z );

		if( fDotN1V <= 0 )
		{
			// 0 �̸� ������.
			float fD = - ( v[i].x * p[i].x + v[i].z * p[i].z ) - fRadius;
			float t;

			t = - ( v[i].x * pStart->x + v[i].z * pStart->z + fD * 1 ) / fDotN1V ;

			if( t <= 0 )
			{
				AuPOS ptQ;
				ptQ.x = pStart->x;// + vVelocity.x * t;
				ptQ.z = pStart->z;// + vVelocity.z * t;

				AuPOS vP0 , vV0;

				vP0 = p[i];
				vV0 = p[(i+1)%4] - p[i]; 

				AuPOS	vPQ;
				vPQ = ptQ - vP0;

				// �� ���ΰ��� �Ÿ� �� fRadius ���� ������..
				FLOAT fDistance = 
					sqrt( 
					( vPQ.x * vPQ.x + vPQ.z * vPQ.z ) -
					( vPQ.x * vV0.x + vPQ.z * vV0.z ) *
					( vPQ.x * vV0.x + vPQ.z * vV0.z ) /
					( vV0.x * vV0.x + vV0.z * vV0.z )
					);

				// ������
				// �÷�Ʈ ���������� 10%�� �������� �߰��ؼ� �����.
				fRadius *= 1.1f;

				if( fDistance <= fRadius )
				{
					// ���� �������� Ȯ��
					AuPOS vA , vB;
					vA = p[(i+1)%4] - p[i];
					vB = *pStart - p[i];

					FLOAT fDotA = ( vA.x * vB.x + vA.z * vB.z );

					vA = p[i] - p[(i+1)%4];
					vB = *pStart - p[(i+1)%4];

					FLOAT fDotB = ( vA.x * vB.x + vA.z * vB.z );

					if( fDotA >= 0 && fDotB >= 0 )
					{
						// �ݸ���..
						bCollision = TRUE;
						fNearestDistance = 0.0f;
						vColPoint = *pStart;
						break;
					}
				}
			}

			if( 0 < t && t <= 1 )
			{
				AuPOS ptTmp;
				ptTmp.x = pStart->x + pVelocity->x * t;
				ptTmp.z = pStart->z + pVelocity->z * t;

				float fDistance = AUPOS_DISTANCE_XZ( *pStart , ptTmp );

				if( fDistance < fNearestDistance )
				{
					// ���� �������� Ȯ��
					AuPOS vA , vB;
					vA = p[(i+1)%4] - p[i];
					vB = ptTmp - p[i];

					//vA.Normalize();
					//vB.Normalize();

					FLOAT fDotA = ( vA.x * vB.x + vA.z * vB.z );

					vA = p[i] - p[(i+1)%4];
					vB = ptTmp - p[(i+1)%4];

					//vA.Normalize();
					//vB.Normalize();

					FLOAT fDotB = ( vA.x * vB.x + vA.z * vB.z );

					if( fDotA >= 0 && fDotB >= 0 )
					{
						// �ݸ���..
						bCollision = TRUE;
						fNearestDistance = fDistance;
						vColPoint = ptTmp;
					}
				}
			}
		}
	}

	for( int j = 0 ; j < 4 ; j ++ )
	{
		// ������⿡ �ִ��� ����.
		{
			AuPOS	vNewDir;
			vNewDir = p[j] - *pStart;
			float fDotProduct;
			fDotProduct = ( vNewDir.x * pVelocity->x + vNewDir.z * pVelocity->z );
			if( fDotProduct <= 0 ) continue;
		}

		// p[j] �� vStart , vColPoint �� ���� �ݸ���.
		float ftime;
		float ftime2;
		{
			AuPOS vVectorA;
			AuPOS vVectorB;

			vVectorA = *pStart - p[j];
			vVectorB = *pVelocity;
			float fdistance = fRadius;

			float fA2 = ( vVectorA.x * vVectorA.x + vVectorA.z * vVectorA.z );
			float fB2 = ( vVectorB.x * vVectorB.x + vVectorB.z * vVectorB.z );
			float fAB = ( vVectorA.x * vVectorB.x + vVectorA.z * vVectorB.z );

			float	fValueInTheSqrt;
			fValueInTheSqrt =	fAB * fAB - fB2 *( fA2 - fdistance * fdistance );

			if( fValueInTheSqrt < 0 ) continue;

			ftime = ( - fAB - sqrt( fValueInTheSqrt ) ) / fB2;
			ftime2 = ( - fAB + sqrt( fValueInTheSqrt ) ) / fB2;
			// if( ftime < 0 ) continue;
			if( ftime > 1 ) continue;
		}

		if( ftime < 0 )
		{
			// �̹� ���Ǿ��ִ� ���´�.
			// ����ġ�� ����.
			bCollision = TRUE;
			fNearestDistance = 0.0f;
			vColPoint = *pStart;
			break;
		}

		AuPOS vJoint;
		vJoint.x = pStart->x + ftime * pVelocity->x;
		vJoint.z = pStart->z + ftime * pVelocity->z;
		vJoint.y = 0.0f;

		float fDistanceFromStart;
		fDistanceFromStart = AUPOS_DISTANCE_XZ( *pStart , vJoint );

		if( fDistanceFromStart < fNearestDistance )
		{
			bCollision = TRUE;
			fNearestDistance = fDistanceFromStart;
			vColPoint = vJoint;
		}
	}

	if( bCollision )
	{
		*pvCollPoint = vColPoint;
	}

	return bCollision;
}


/*****************************************************************
*   Function : AgpmCharacter()
*   Comment  : ������ 
*   Date&Time : 2002-04-16, ���� 12:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpmCharacter::AgpmCharacter()
{
	SetModuleName("AgpmCharacter");

	EnableIdle(TRUE);
	EnableIdle2(TRUE);

	// Data�� ũ�⸦ �����Ѵ�.
	SetModuleData( sizeof( AgpdCharacter ), AGPMCHAR_DATA_TYPE_CHAR );
	SetModuleData( sizeof( AgpdCharacterTemplate ) , AGPMCHAR_DATA_TYPE_TEMPLATE );

	SetPacketType(AGPMCHARACTER_PACKET_TYPE);

	// flag length = 16bits
	m_csPacket.SetFlagLength(sizeof(INT32));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,											// Operation
							AUTYPE_INT32,			1,											// Character ID
							AUTYPE_INT32,			1,											// Character Template ID
							AUTYPE_MEMORY_BLOCK,	1,											// Game ID
							AUTYPE_INT8,			1,											// Character Status
							AUTYPE_PACKET,			1,											// Move Packet
							AUTYPE_PACKET,			1,											// Action Packet
							AUTYPE_PACKET,			1,											// Factor Packet
							AUTYPE_INT64,			1,											// llMoney
							AUTYPE_INT64,			1, 											// bank money
							AUTYPE_INT64,			1,											// cash
							AUTYPE_INT8,			1,											// character action status
							AUTYPE_INT8,			1,											// character criminal status
							AUTYPE_INT32,			1,											// attacker id (������� ������ �ʿ�)
							AUTYPE_INT8,			1,											// ���� �����Ǽ� �ʿ� �������� ����
							AUTYPE_UINT8,			1,											// region index
							AUTYPE_UINT8,			1,											// social action index
							AUTYPE_UINT64,			1,											// special status
							AUTYPE_INT8,			1,											// is transform status
							AUTYPE_CHAR,			AGPACHARACTER_MAX_CHARACTER_SKILLINIT,		// skill initialization text
							AUTYPE_INT8,			1,											// face index
							AUTYPE_INT8,			1,											// hair index
							AUTYPE_INT32,			1,											// Option Flag
							AUTYPE_INT8,			1,											// bank size
							AUTYPE_UINT16,			1,											// event status flag
							AUTYPE_INT32,			1,											// remained criminal status time
							AUTYPE_INT32,			1,											// remained murderer point time
							AUTYPE_CHAR,			AGPACHARACTER_MAX_CHARACTER_NICKNAME,		// nick name
							AUTYPE_MEMORY_BLOCK,	1,											// gameguard auth data
							AUTYPE_UINT32,			1,											// last killed time in battlesquare
							AUTYPE_END,				0
							);

	m_csPacketMove.SetFlagLength(sizeof(INT8));
	m_csPacketMove.SetFieldType(
							AUTYPE_POS,		1,		// Character Current Position
							AUTYPE_POS,		1,		// Character Destination Position
							AUTYPE_INT32,	1,		// Character Follow Target ID
							AUTYPE_INT32,	1,		// Follow Distance
							AUTYPE_FLOAT,	1,		// Turn Degree X
							AUTYPE_FLOAT,	1,		// Turn Degree Y
							AUTYPE_INT8,	1,		// Move Flag
							AUTYPE_INT8,	1,		// Move Direction
							AUTYPE_END, 0
							);

	m_csPacketAction.SetFlagLength(sizeof(INT16));
	m_csPacketAction.SetFieldType(
							AUTYPE_INT8, 1,			// action type
							AUTYPE_INT32, 1,		// �׼��� ����� �Ǵ³� (Target CID)
							AUTYPE_INT32, 1,		// skill tid
							AUTYPE_INT8, 1,			// action result type (miss, evade, critical hit, etc)
							AUTYPE_PACKET, 1,		// target damage factor packet
							AUTYPE_POS, 1,			// character attack position
							AUTYPE_UINT8, 1,		// Combo Information
							AUTYPE_INT8, 1,			// is force attack
							AUTYPE_UINT32, 1,		// Additional Effect. (2005.12.14. steeple)							
							AUTYPE_UINT8, 1,		// Hit Index. (2006.03.21. steeple)
							AUTYPE_END, 0
							);

	m_ulPrevClockCount	= 0;
	m_ulIntervalClock	= 0;
	m_ulPrevRemoveClockCount	= 0;

	m_ulPrevProcessMoveFollow		= 0;
	m_ulProcessMoveFollowInterval	= 0;

	g_llLevelUpExp.MemSet(0, AGPMCHAR_MAX_LEVEL + 1);

	m_pcsAgpmFactors	= NULL;
	m_pcsApmMap			= NULL;

	m_lStaticCharacterIndex		= 2;

//	m_bIsAcceptPvP		= FALSE;
	//���ؿ˲��� �̰� TRUE�� �϶�� �ؼ� TRUE�� �ٲ�.
	m_bIsAcceptPvP		= TRUE;

	m_ulRegionRefreshInterval	= 0;

	m_aszFilterText = NULL;
	m_lNumFilterText = 0;

	m_lMaxTID	= 0;
	
	m_lEventEffectID = 0;
	
	m_pAgpdCharisma = NULL;
	m_lTotalCharisma = 0;

	m_ulMoveLockFrameTick = 0;

	InitFuncPtr();	// �� �Լ� ������ �ʱ�ȭ
}

/*****************************************************************
*   Function : ~AgpmCharacter()
*   Comment  : �Ҹ��� 
*   Date&Time : 2002-04-16, ���� 12:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AgpmCharacter::~AgpmCharacter()
{
}

/*****************************************************************
*   Function : AddModule()
*   Comment  : Add Module 
*   Date&Time : 2002-04-16, ���� 4:16
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpmCharacter::OnAddModule()
{
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsApmMap = (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmConfig = (AgpmConfig *) GetModule("AgpmConfig");

	/*
	if (!m_pcsAgpmFactors || !m_pcsApmMap)
		return FALSE;
	*/

	if(!AddStreamCallback(AGPMCHAR_DATA_TYPE_TEMPLATE, TemplateReadCB, TemplateWriteCB, this))
		return FALSE;

	if (!AddStreamCallback(AGPMCHAR_DATA_TYPE_CHAR, CharacterReadCB, CharacterWriteCB, this))
		return FALSE;

	if (!AddStreamCallback(AGPMCHAR_DATA_TYPE_STATIC, StaticCharacterReadCB, StaticCharacterWriteCB, this))
		return FALSE;

	if (!AddStreamCallback(AGPMCHAR_DATA_TYPE_STATIC_ONE, StaticOneCharacterReadCB, StaticOneCharacterWriteCB, this))
		return FALSE;

	return TRUE;
}	

/*****************************************************************
*   Function : OnInit()
*   Comment  : Init
*   Date&Time : 2002-04-16, ���� 5:45
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpmCharacter::OnInit()
{
	//�޸� �Ҵ� 

	if (!m_csACharacter.InitializeObject(sizeof(AgpdCharacter *), m_csACharacter.GetCount()))
		return FALSE;

	// ������ (2005-12-28 ���� 11:59:13) : 
	// �ɸ��� ���ø��� �ϴ� �������� �ǽ���..
	// if (!m_csACharacterTemplate.InitializeObject( sizeof(AgpdCharacterTemplate *), m_csACharacterTemplate.GetCount()))
	// 	return FALSE;

	if (!m_csAdminCharacterRemove.InitializeObject( sizeof(AgpdCharacter *), m_csAdminCharacterRemove.GetCount()))
		return FALSE;
	
	return TRUE;
}

BOOL	AgpmCharacter::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgpmCharacter::OnIdle");

	//ProcessRemove(ulClockCount);

	if (m_ulIntervalClock + m_ulPrevClockCount > ulClockCount)
		return TRUE;

	INT32			lIndex	= 0;
	AgpdCharacter	*pcsCharacter = GetCharSequence(&lIndex);

	while (pcsCharacter)
	{
		if (pcsCharacter->m_Mutex.WLock())
		{
			pcsCharacter->m_bIsPostedIdleProcess	= TRUE;

			CharacterIdle(pcsCharacter, ulClockCount);

			pcsCharacter->m_bIsPostedIdleProcess	= FALSE;

			pcsCharacter->m_Mutex.Release();
		}

		pcsCharacter = GetCharSequence(&lIndex);
	}

	if (ulClockCount > m_ulPrevClockCount)
		m_ulPrevClockCount = ulClockCount;

	return TRUE;
}

BOOL	AgpmCharacter::OnIdle2(UINT32 ulClockCount)
{
	PROFILE("AgpmCharacter::OnIdle2");

	//STOPWATCH2(GetModuleName(), _T("OnIdle2"));

	ProcessRemove(ulClockCount);

	return TRUE;
}

BOOL	AgpmCharacter::CharacterIdle(AgpdCharacter *pcsCharacter, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_ulPrevProcessTime >= ulClockCount ||
		!pcsCharacter->m_bIsAddMap)
		return FALSE;

	if (IsDisableCharacter(pcsCharacter))
		return TRUE;

	/*
	if (!EnumCallback(AGPMCHAR_CB_ID_CHECK_PROCESS_IDLE, pcsCharacter, &ulClockCount))
		return TRUE;

	{
		PROFILE("AgpmCharacter::CharacterIdle() - Lock/Unlock");

		if (!pcsCharacter->m_Mutex.WLock())
			return FALSE;
	}
	*/

	if (IsAllBlockStatus(pcsCharacter))
		return TRUE;


	// 2004.08.25. steeple
	// stun ����
	//if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
	{
		if (pcsCharacter->m_bMove)
			StopCharacter(pcsCharacter, NULL);
	}

	// 2007.06.27. steeple
	if(IsStatusSleep(pcsCharacter))
	{
		if (pcsCharacter->m_bMove)
			StopCharacter(pcsCharacter, NULL);
	}

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		if (pcsCharacter->m_bMove)
			StopCharacter(pcsCharacter, NULL);
	}

	if (IsCombatMode(pcsCharacter))
	{
		if (pcsCharacter->m_ulSetCombatModeTime + AGPMCHAR_MAX_COMBAT_MODE_TIME < ulClockCount)
			ResetCombatMode(pcsCharacter);
	}

	{
		PROFILE("AgpmCharacter::CharacterIdle() - Enum Callback");

		if (!EnumCallback(AGPMCHAR_CB_ID_UPDATE_CHAR, pcsCharacter, UintToPtr(ulClockCount)))
		{
			OutputDebugString("AgpmCharacter::CharacterIdle() Error (1) !!!\n");
			return FALSE;
		}
	}

	ProcessIdleMove(pcsCharacter, ulClockCount);

	/*
	{
		PROFILE("AgpmCharacter::CharacterIdle() - Lock/Unlock");
		pcsCharacter->m_Mutex.Release();
	}
	*/

	return TRUE;
}

BOOL	AgpmCharacter::ProcessIdleMove(AgpdCharacter *pcsCharacter, UINT32 ulClockCount)
{
	if (!pcsCharacter || !pcsCharacter->m_bIsAddMap)
		return FALSE;

	if (!pcsCharacter->m_pcsCharacterTemplate)
		return FALSE;

	if (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsCharacter->m_ulSpecialStatus)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("ProcessIdleMove"));

	ApmMap::BLOCKINGTYPE eType = GetBlockingType( pcsCharacter );

	if (pcsCharacter->m_bMove)
	{
		FLOAT			ulDeltaMove;
		AuPOS			stDestPos;
		AuPOS			stAdjustDestPos;

		BOOL			bIgnoreBlocking	= FALSE;

		PROFILE("AgpmCharacter::CharacterIdle() - Move");

		INT32	lMoveSpeed = 0;

		if (pcsCharacter->m_bMoveFast || pcsCharacter->m_bHorizontal)
			lMoveSpeed = GetFastMoveSpeed(pcsCharacter);
		else
			lMoveSpeed = GetMoveSpeed(pcsCharacter);

		if( pcsCharacter->m_eMoveDirection == MD_BACKWARD		||
			pcsCharacter->m_eMoveDirection == MD_BACKWARDLEFT	||
			pcsCharacter->m_eMoveDirection == MD_BACKWARDRIGHT	)
		{
			// ������ .. �Դٸ� �ڷζٱ� �ӵ� ����.
			FLOAT	fBackwardAdjustConstant = 0.60f; // 60% �� �ӵ��� ��..
			{
				#ifdef _DEBUG
				static FLOAT _sfBACDebugValue = fBackwardAdjustConstant;
				fBackwardAdjustConstant = _sfBACDebugValue;
				#endif
			}

			lMoveSpeed = (INT32) ( lMoveSpeed * fBackwardAdjustConstant );
		}

		if (lMoveSpeed == 0)
			lMoveSpeed = AGPMCHAR_DEFAULT_MOVE_SPEED;

		ulDeltaMove = (FLOAT) ((ulClockCount - pcsCharacter->m_ulPrevProcessTime) * /*0.4*/ (lMoveSpeed / 10000.0) );

		/*
		if (pcsCharacter->m_bMoveFast)
			ulDeltaMove *= 2;
		else if (pcsCharacter->m_bHorizontal)
			ulDeltaMove *= 4;
		*/

		if (pcsCharacter->m_bHorizontal)
			ulDeltaMove *= 4;

		pcsCharacter->m_ulPrevProcessTime = ulClockCount;

		ZeroMemory(&stDestPos, sizeof(stDestPos));
		ZeroMemory(&stAdjustDestPos, sizeof(stAdjustDestPos));

		if (pcsCharacter->m_eMoveDirection)
		{
			stDestPos.x = pcsCharacter->m_stPos.x + pcsCharacter->m_stDirection.x * ulDeltaMove;
			stDestPos.y = pcsCharacter->m_stPos.y + pcsCharacter->m_stDirection.y * ulDeltaMove;
			stDestPos.z = pcsCharacter->m_stPos.z + pcsCharacter->m_stDirection.z * ulDeltaMove;

			BOOL	bDestUpdate = FALSE;
			if( IsPC( pcsCharacter ) )
			{
				bDestUpdate |= m_pcsApmMap->GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , pcsCharacter->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , eType );
				bDestUpdate |= GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , eType , pcsCharacter );
			}
			else
				bDestUpdate	= TRUE;

			if (!pcsCharacter->m_ulCheckBlocking || bDestUpdate )
			{
				UpdatePosition(pcsCharacter, &stDestPos);
			}
			else
			{
				UpdatePosition(pcsCharacter, &stAdjustDestPos);
				pcsCharacter->m_bSync = TRUE;
				pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
				StopCharacter(pcsCharacter, NULL);
			}
		}
		else if (pcsCharacter->m_bMoveFollow)
		{
			if (pcsCharacter->m_lFollowTargetID == AP_INVALID_CID)
				StopCharacter(pcsCharacter, NULL);
			else
			{
				AgpdCharacter	*pcsTargetCharacter	= GetCharacter(pcsCharacter->m_lFollowTargetID);
				if (pcsTargetCharacter)
				{
					if (m_ulProcessMoveFollowInterval == 0 ||
						m_ulPrevProcessMoveFollow + m_ulProcessMoveFollowInterval < ulClockCount)
					{
						MoveCharacterFollow(pcsCharacter, pcsTargetCharacter, pcsCharacter->m_lFollowDistance);
					}

					stDestPos.x = pcsCharacter->m_stPos.x + pcsCharacter->m_stDirection.x * ulDeltaMove;
					stDestPos.y = pcsCharacter->m_stPos.y + pcsCharacter->m_stDirection.y * ulDeltaMove;
					stDestPos.z = pcsCharacter->m_stPos.z + pcsCharacter->m_stDirection.z * ulDeltaMove;

					/*
					{
						AuPOS	stDelta;
						// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
						AuMath::V3DSubtract(&stDelta, &pcsTargetCharacter->m_stPos, &pcsCharacter->m_stPos);
						stDelta.y = 0;
						// fDiv�� �� �Ÿ�
						FLOAT	fDivTarget = AuMath::V3DLength(&stDelta);

						// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
						AuMath::V3DSubtract(&stDelta, &stDestPos, &pcsCharacter->m_stPos);
						stDelta.y = 0;
						// fDiv�� �� �Ÿ�
						FLOAT	fDivDest = AuMath::V3DLength(&stDelta);

						if (fDivTarget - (FLOAT) pcsCharacter->m_lFollowDistance < fDivDest)
							stDestPos	= pcsTargetCharacter->m_stPos;
					}
					*/

					BOOL	bDestUpdate = FALSE;
					if( IsPC( pcsCharacter ) )
					{
						bDestUpdate |= m_pcsApmMap->GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , pcsCharacter->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , eType );
						bDestUpdate |= GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , eType , pcsCharacter );
					}
					else
						bDestUpdate	= TRUE;

					if (!pcsCharacter->m_ulCheckBlocking || bDestUpdate )
					{
						UpdatePosition(pcsCharacter, &stDestPos);

						if (CheckStopFollow(pcsCharacter, pcsTargetCharacter))
							StopCharacter(pcsCharacter, NULL);
					}
					else
					{
						UpdatePosition(pcsCharacter, &stAdjustDestPos);
						pcsCharacter->m_bSync = TRUE;
						pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
						StopCharacter(pcsCharacter, NULL);
					}
				}
				else
				{
					StopCharacter(pcsCharacter, NULL);
				}
			}
		}
		else
		{
			if (pcsCharacter->m_ulEndMoveTime <= ulClockCount)
			{
				// �̵� ������ �ð��� �Ǿ��µ� ���� ��ǥ�������� ���� ���ߴ�.
				// �� ������ �ð��� �ٽ� ����Ѵ�.

				FLOAT	fDivTarget = 0;
				FLOAT	fDivDest = 0;
				AuPOS	stDelta;

				// �ӽ÷� Y �� �˻縦 ���� �ʴ´�. ������ DestinationPos�� Y�� ���� �� �ؿ� �ִ°�� �ʿ��� �׸��� �������� ���ϹǷ� ������ ��ǥ���� �������� ���ϰ� �̵��Ѵ�.
				////////////////////////////////////////////////////////////////////////
				//if (pcsCharacter->m_stDestinationPos.x != pcsCharacter->m_stPos.x ||
				//	pcsCharacter->m_stDestinationPos.z != pcsCharacter->m_stPos.z)
				{
					// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
					AuMath::V3DSubtract(&stDelta, &pcsCharacter->m_stDestinationPos, &pcsCharacter->m_stPos);

					stDelta.y = 0;
					// fDiv�� �� �Ÿ�
					fDivTarget = AuMath::V3DLength(&stDelta);

					if (ulDeltaMove > fDivTarget)
						ulDeltaMove = fDivTarget;

					stDestPos.x = pcsCharacter->m_stPos.x + pcsCharacter->m_stDirection.x * ulDeltaMove;
					stDestPos.y = pcsCharacter->m_stPos.y + pcsCharacter->m_stDirection.y * ulDeltaMove;
					stDestPos.z = pcsCharacter->m_stPos.z + pcsCharacter->m_stDirection.z * ulDeltaMove;

					// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
					AuMath::V3DSubtract(&stDelta, &stDestPos, &pcsCharacter->m_stPos);
					stDelta.y = 0;
					// fDiv�� �� �Ÿ�
					fDivDest = AuMath::V3DLength(&stDelta);
				}

				if (fDivDest < fDivTarget &&
					fDivTarget > 10.0)
//					&&
//					(abs(pcsCharacter->m_stMatrix.at.x) + abs(stDelta.x) == abs(pcsCharacter->m_stMatrix.at.x + stDelta.x)) &&
//					/*(abs(pcsCharacter->m_stMatrix.at.y) + abs(stDelta.y) == abs(pcsCharacter->m_stMatrix.at.y + stDelta.y)) &&*/
//					(abs(pcsCharacter->m_stMatrix.at.z) + abs(stDelta.z) == abs(pcsCharacter->m_stMatrix.at.z + stDelta.z)))
				{
					pcsCharacter->m_ulEndMoveTime = (UINT32) (ulClockCount + fDivTarget / (lMoveSpeed / 10000.0) );

					BOOL	bDestUpdate = FALSE;
					if( IsPC( pcsCharacter ) )
					{
						bDestUpdate |= m_pcsApmMap->GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , pcsCharacter->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , eType );
						bDestUpdate |= GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , eType , pcsCharacter );
					}
					else
						bDestUpdate	= TRUE;

					if (!pcsCharacter->m_ulCheckBlocking || bDestUpdate )
					{
						// UpdatePosition�� ���ؼ� ���ߴ� ��쿡�� Sync�� �����.
						if( !UpdatePosition(pcsCharacter, &stDestPos) )
						{
							pcsCharacter->m_bSync = TRUE;
							pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
							StopCharacter(pcsCharacter, NULL);

							if (!IsPC(pcsCharacter))
							{
								bIgnoreBlocking	= TRUE;
								EnumCallback( AGPMCHAR_CB_ID_MONSTER_AI_PATHFIND, pcsCharacter, &bIgnoreBlocking );
							}
						}
					}
					else
					{
						UpdatePosition(pcsCharacter, &stAdjustDestPos);
						pcsCharacter->m_bSync = TRUE;
						pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
						StopCharacter(pcsCharacter, NULL);
					}
				}
				//�������� ������ ���.
				else
				{
					//���Ͷ��?
					if( !IsPC(pcsCharacter) )
					{
						UpdatePosition(pcsCharacter, &pcsCharacter->m_stDestinationPos);

						StopCharacter(pcsCharacter, NULL);

/*						//���� ��ġ�� ��� MoveCharacter�� �Ѵ�.
						if( EnumCallback( AGPMCHAR_CB_ID_MONSTER_AI_PATHFIND, pcsCharacter, NULL ) )
						{
							//���� ��尡 ���ٸ�~ ��Ȯ�� �������� ������ ����̴�.
							StopCharacter(pcsCharacter, NULL);
						}*/
					}
					//PC���?
					else
					{
						UpdatePosition(pcsCharacter, &pcsCharacter->m_stDestinationPos);
						StopCharacter(pcsCharacter, NULL);
					}
				}
			}
			else
			{
				stDestPos.x = pcsCharacter->m_stPos.x + pcsCharacter->m_stDirection.x * ulDeltaMove;
				stDestPos.y = pcsCharacter->m_stPos.y + pcsCharacter->m_stDirection.y * ulDeltaMove;
				stDestPos.z = pcsCharacter->m_stPos.z + pcsCharacter->m_stDirection.z * ulDeltaMove;

				BOOL	bNeedStopCharacter	= FALSE;

				{
					AuPOS	stDelta;
					// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
					AuMath::V3DSubtract(&stDelta, &pcsCharacter->m_stDestinationPos, &pcsCharacter->m_stPos);
					stDelta.y = 0;
					// fDiv�� �� �Ÿ�
					FLOAT	fDivTarget = AuMath::V3DLength(&stDelta);

					// stDelta�� ���� ��ġ���� ��ǥ ��ġ������ Vector
					AuMath::V3DSubtract(&stDelta, &stDestPos, &pcsCharacter->m_stPos);
					stDelta.y = 0;
					// fDiv�� �� �Ÿ�
					FLOAT	fDivDest = AuMath::V3DLength(&stDelta);

					if (fDivTarget < fDivDest)
					{
						stDestPos	= pcsCharacter->m_stDestinationPos;
						bNeedStopCharacter	= TRUE;
					}
				}

				BOOL	bDestUpdate = FALSE;
				if( IsPC( pcsCharacter ) )
				{
					bDestUpdate |= m_pcsApmMap->GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , pcsCharacter->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , eType );
					bDestUpdate |= GetValidDestination(&pcsCharacter->m_stPos, &stDestPos, &stAdjustDestPos , eType , pcsCharacter  );
				}
				else
					bDestUpdate	= TRUE;

				if (!pcsCharacter->m_ulCheckBlocking || bDestUpdate )
				{
					//���Ͷ��?
					if( !UpdatePosition(pcsCharacter, &stDestPos) || bNeedStopCharacter)
					{
						pcsCharacter->m_bSync = TRUE;
						pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
						StopCharacter(pcsCharacter, NULL);

						if (!IsPC(pcsCharacter))
						{
							bIgnoreBlocking	= TRUE;
							EnumCallback( AGPMCHAR_CB_ID_MONSTER_AI_PATHFIND, pcsCharacter, &bIgnoreBlocking );
						}
					}
				}
				else
				{
					UpdatePosition(pcsCharacter, &stAdjustDestPos);
					pcsCharacter->m_bSync = TRUE;
					pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;
					StopCharacter(pcsCharacter, NULL);

					if (!IsPC(pcsCharacter))
					{
						bIgnoreBlocking	= TRUE;
						EnumCallback( AGPMCHAR_CB_ID_MONSTER_AI_PATHFIND, pcsCharacter, &bIgnoreBlocking );
					}
				}
			}
		}
	}

	return TRUE;
}

/*****************************************************************
*   Function : 
*   Comment  : 
*   Date&Time : 2002-04-17, ���� 3:43
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpmCharacter::OnDestroy()
{
	//@{ Jaewon 20050906
	// GetCharSequence, GetObjectSequence, GetSequence are not good because they have two responsibilities.
	// They get the element at index and advance the index to the next.
	// In the following loops, a loop body can invalidate the next element.
	// In that case, it returns early so that we cannot iterate all the elements.
	INT32					lIndex, prevIndex;
	AgpdCharacter			*pcsCharacter;
	AgpdCharacterTemplate	*pcsTemplate;

	prevIndex = lIndex = 0;
	for (pcsCharacter = GetCharSequence(&lIndex); pcsCharacter; pcsCharacter = GetCharSequence(&lIndex))
	{
		//@{ Jaewon 20050906
		//TRACEFILE("pickingAtomic.log", "OnDestroy %d %s %d\n", pcsCharacter->m_lID, pcsCharacter->m_szID, m_csACharacter.GetObjectCount());
		//@} Jaewon
		AuAutoLock pLock(pcsCharacter->m_Mutex);
		
		RemoveCharacterFromMap(pcsCharacter);
		DestroyCharacter(pcsCharacter);
		//RemoveCharacter(pcsCharacter, FALSE, TRUE);

		GetCharSequence(&prevIndex);
		lIndex = prevIndex; 
	}

	prevIndex = lIndex = 0;
	AgpdCharacter			**ppcsCharacter	= (AgpdCharacter **) m_csAdminCharacterRemove.GetObjectSequence(&lIndex);
	while (ppcsCharacter && *ppcsCharacter)
	{
		//@{ Jaewon 20050906
		//TRACEFILE("pickingAtomic.log", "OnDestroy2 %d %s %d\n", (*ppcsCharacter)->m_lID, (*ppcsCharacter)->m_szID, m_csAdminCharacterRemove.GetObjectCount());
		//@} Jaewon
		DestroyCharacter(*ppcsCharacter);

		m_csAdminCharacterRemove.GetObjectSequence(&prevIndex);
		lIndex = prevIndex;

		ppcsCharacter	= (AgpdCharacter **) m_csAdminCharacterRemove.GetObjectSequence(&lIndex);
	}
	//@} Jaewon

	lIndex = 0;
	for (pcsTemplate = GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = GetTemplateSequence(&lIndex))
		DestroyTemplate(pcsTemplate);

	m_csACharacter.RemoveObjectAll();
	m_csACharacterTemplate.RemoveObjectAll();
	m_csAdminCharacterRemove.RemoveObjectAll();

	// 2005.07.01. steeple
	if(m_aszFilterText)
	{
		delete [] m_aszFilterText;
		m_aszFilterText	= NULL;
	}

	// 2007.02.15. laki
	if (m_pAgpdCharisma)
		{
		delete [] m_pAgpdCharisma;
		m_pAgpdCharisma = NULL;
		}
	m_lTotalCharisma = 0;

	return TRUE;
}

BOOL	AgpmCharacter::OnValid(CHAR* szData, INT16 nSize)
{
	return TRUE;
}


//		CreateCharacter / DestroyCharacter
//	Functions
//		- ĳ���͸� ����/�ı��Ѵ�.
//			* ApModule�� CreateModuleData()�� �����ϸ�, Memory�� �Ҵ��ϰ� Constructor Enum�Ѵ�.
//			* ApModule�� DestroyModuleData()�� �����ϸ�, Memory�� �����ϰ� Constructor Enum�Ѵ�.
//	Arguments
//	Return value
//		- AgpdCharacter *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacter *AgpmCharacter::CreateCharacter()
{
	PROFILE("AgpmCharacter::CreateCharacter");

	AgpdCharacter *pcsCharacter = (AgpdCharacter *) CreateModuleData(AGPMCHAR_DATA_TYPE_CHAR);

	if (pcsCharacter)
	{
		pcsCharacter->m_Mutex.Init((PVOID) pcsCharacter);
		pcsCharacter->m_eType = APBASE_TYPE_CHARACTER;
		AuMath::MatrixIdentity(&pcsCharacter->m_stMatrix);

		pcsCharacter->m_bIsAddMap = FALSE;

		pcsCharacter->m_bHorizontal	= FALSE;

		ZeroMemory(&pcsCharacter->m_csFactor, sizeof(AgpdFactor));
		ZeroMemory(&pcsCharacter->m_csFactorPoint, sizeof(AgpdFactor));
		ZeroMemory(&pcsCharacter->m_csFactorPercent, sizeof(AgpdFactor));

		pcsCharacter->m_unActionStatus		= AGPDCHAR_STATUS_NOT_ACTION;
		pcsCharacter->m_unCriminalStatus	= AGPDCHAR_CRIMINAL_STATUS_INNOCENT;

		//pcsCharacter->m_lAttackerList.MemSet(0, AGPMCHAR_MAX_ATTACKER_LIST);

		pcsCharacter->m_lOriginalTID		= AP_INVALID_CID;
		pcsCharacter->m_lTransformedByCID	= AP_INVALID_CID;

		if (m_pcsAgpmFactors)
		{
			m_pcsAgpmFactors->InitFactor(&pcsCharacter->m_csFactor);
			m_pcsAgpmFactors->InitFactor(&pcsCharacter->m_csFactorPoint);
			m_pcsAgpmFactors->InitFactor(&pcsCharacter->m_csFactorPercent);
			m_pcsAgpmFactors->InitFactor(&pcsCharacter->m_csFactorOriginal);

			pcsCharacter->m_csFactor.m_bPoint			= TRUE;
			pcsCharacter->m_csFactorPoint.m_bPoint		= TRUE;
			pcsCharacter->m_csFactorPercent.m_bPoint	= FALSE;
			pcsCharacter->m_csFactorOriginal.m_bPoint	= TRUE;
		}

		pcsCharacter->m_bIsTrasform			= FALSE;
		pcsCharacter->m_bIsEvolution		= FALSE;
		pcsCharacter->m_eTransformType		= -1;
		pcsCharacter->m_lTransformItemTID	= 0;
		pcsCharacter->m_bCopyTransformFactor = 0;

		pcsCharacter->m_llBankMoney			= 0;
		pcsCharacter->m_llMoney				= 0;

		pcsCharacter->m_ulRemoveTimeMSec	= 0;

		pcsCharacter->m_ulSyncMoveTime		= 0;
		pcsCharacter->m_ulPrevProcessTime	= 0;
		pcsCharacter->m_ulNextRegionRefreshTime	= 0;
		pcsCharacter->m_ulSetCombatModeTime	= 0;

		pcsCharacter->m_lAdjustMoveSpeed	= 0;

//		ZeroMemory(pcsCharacter->m_szBindingAreaName, AGPMCHAR_MAX_TOWN_NAME + 1);

		// ������ (2004-09-17 ���� 5:18:27) : 
		pcsCharacter->m_nBindingRegionIndex	= -1; // ����Ʈ
		pcsCharacter->m_nLastExistBindingIndex	= -1;

		pcsCharacter->m_ulCheckBlocking		= ApTileInfo::BLOCKGEOMETRY;

		pcsCharacter->m_bIsActionBlock		= FALSE;
		pcsCharacter->m_ulActionBlockTime	= 0;
		pcsCharacter->m_eActionBlockType	= AGPDCHAR_ACTION_BLOCK_TYPE_ALL;

		pcsCharacter->m_ulCharType			= 0;

		pcsCharacter->m_bIsPostedIdleProcess	= FALSE;

		pcsCharacter->m_nDimension		= 0;			// �� �ε��� �ʱ�ȭ

		pcsCharacter->m_lOptionFlag		= 0;			// Option����Ʈ �ʱ�ȭ( 2005.05.31 By SungHoon )
		pcsCharacter->m_bRidable		= FALSE;
		//pcsCharacter->m_pOwner			= NULL;
		//pcsCharacter->m_pSummon			= NULL;
		
		pcsCharacter->m_lLastUpdateCriminalFlagTime = 0;
		pcsCharacter->m_lLastUpdateMurdererPointTime = 0;
		pcsCharacter->m_ulBattleSquareKilledTime = 0;

		pcsCharacter->m_lCountMobMoli	= 0;

		pcsCharacter->m_stGameBonusByCash.init();

		pcsCharacter->m_bIsActiveData	= TRUE;
		pcsCharacter->m_bIsReadyRemove	= FALSE;		// 2007.03.05. steeple

		pcsCharacter->m_bIsProtectedNPC	= FALSE;
		pcsCharacter->m_nNPCID			= -1;

		pcsCharacter->m_ulOnlineTime	= 0;
		pcsCharacter->m_lAddictStatus	= 0;

		pcsCharacter->m_csTitle			= NULL;
		pcsCharacter->m_csTitleQuest	= NULL;

		pcsCharacter->m_bNPCDisplayForMap		=	TRUE;
		pcsCharacter->m_bNPCDisplayForNameBoard	=	TRUE;
	}

	return pcsCharacter;
}

BOOL AgpmCharacter::DestroyCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !pcsCharacter->m_bIsActiveData)
		return FALSE;

	if (m_pcsAgpmFactors)
	{
		m_pcsAgpmFactors->DestroyFactor(&pcsCharacter->m_csFactor);
		m_pcsAgpmFactors->DestroyFactor(&pcsCharacter->m_csFactorPoint);
		m_pcsAgpmFactors->DestroyFactor(&pcsCharacter->m_csFactorPercent);
		m_pcsAgpmFactors->DestroyFactor(&pcsCharacter->m_csFactorOriginal);
	}

	pcsCharacter->m_bIsActiveData	= FALSE;

	if(pcsCharacter->m_csTitle)
	{
		delete pcsCharacter->m_csTitle;
		pcsCharacter->m_csTitle = NULL;
	}

	if(pcsCharacter->m_csTitleQuest)
	{
		delete pcsCharacter->m_csTitleQuest;
		pcsCharacter->m_csTitleQuest = NULL;
	}

	pcsCharacter->m_Mutex.Destroy();

	return DestroyModuleData((PVOID) pcsCharacter, AGPMCHAR_DATA_TYPE_CHAR);
}

//		CreateTemplate / DestroyTemplate
//	Functions
//		- ĳ���� Template�� ����/�ı��Ѵ�.
//			* ApModule�� CreateModuleData()�� �����ϸ�, Memory�� �Ҵ��ϰ� Constructor Enum�Ѵ�.
//			* ApModule�� DestroyModuleData()�� �����ϸ�, Memory�� �����ϰ� Constructor Enum�Ѵ�.
//	Arguments
//	Return value
//		- AgpdCharacter *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacterTemplate *AgpmCharacter::CreateTemplate()
{
	AgpdCharacterTemplate *pcsTemplate = (AgpdCharacterTemplate *) CreateModuleData(AGPMCHAR_DATA_TYPE_TEMPLATE);

	if (pcsTemplate)
	{
		pcsTemplate->m_Mutex.Init((PVOID) pcsTemplate);
		pcsTemplate->m_eType = APBASE_TYPE_CHARACTER_TEMPLATE;

		ZeroMemory(&pcsTemplate->m_csFactor, sizeof(AgpdFactor));
		if (m_pcsAgpmFactors)
		{
			m_pcsAgpmFactors->InitFactor(&pcsTemplate->m_csFactor);
			pcsTemplate->m_csFactor.m_bPoint = TRUE;

			for (int i = 0; i < AGPMCHAR_MAX_LEVEL + 1; ++i)
				m_pcsAgpmFactors->InitFactor(&pcsTemplate->m_csLevelFactor[i]);
		}

		pcsTemplate->m_ulCharType	= AGPMCHAR_TYPE_ATTACKABLE | AGPMCHAR_TYPE_TARGETABLE;
		pcsTemplate->m_lGhelldMin = 0;
		pcsTemplate->m_lGhelldMax = 0;
		pcsTemplate->m_lDropTID = 0;
		pcsTemplate->m_ulIDColor	= 0xffffff;

		pcsTemplate->m_lFaceNum = 0;
		pcsTemplate->m_lHairNum = 0;

		pcsTemplate->m_eTamableType = AGPDCHAR_TAMABLE_TYPE_NONE;

		pcsTemplate->m_eRangeType	= AGPDCHAR_RANGE_TYPE_MELEE;

		pcsTemplate->m_lStaminaPoint = 0;
		pcsTemplate->m_lPetType = 0;
		pcsTemplate->m_lStartStaminaPoint = 0;
	}

	return pcsTemplate;
}

BOOL AgpmCharacter::DestroyTemplate(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	if (m_pcsAgpmFactors)
	{
		m_pcsAgpmFactors->DestroyFactor(&pcsTemplate->m_csFactor);

		for (int i = 0; i < AGPMCHAR_MAX_LEVEL + 1; ++i)
			m_pcsAgpmFactors->DestroyFactor(&pcsTemplate->m_csLevelFactor[i]);
	}

	pcsTemplate->m_Mutex.Destroy();

	return DestroyModuleData((PVOID) pcsTemplate, AGPMCHAR_DATA_TYPE_TEMPLATE);
}

/*
BOOL AgpmCharacter::SetMonster( AgpdCharacter *pcsAgpdCharacter, BOOL bMonster )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdCharacter != NULL )
	{
		if( pcsAgpdCharacter->m_pcsCharacterTemplate != NULL )
		{
			pcsAgpdCharacter->m_pcsCharacterTemplate->m_bIsMonster = bMonster;

			bResult = TRUE;
		}
	}

	return bResult;
}
*/

UINT32 AgpmCharacter::GetCharacterType(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !pcsCharacter->m_pcsCharacterTemplate)
		return AGPMCHAR_TYPE_NONE;

	return pcsCharacter->m_ulCharType;
}

BOOL AgpmCharacter::SetTypeMonster(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_MONSTER;

	return TRUE;
}

BOOL AgpmCharacter::SetTypePC(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_PC;

	return TRUE;
}

BOOL AgpmCharacter::SetTypeNPC(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_NPC;

	return TRUE;
}

BOOL	AgpmCharacter::SetTypeTrap(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_TRAP;

	return TRUE;
}
BOOL AgpmCharacter::SetTypeGuard(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_GUARD;

	return TRUE;
}

BOOL AgpmCharacter::SetStatusTame(AgpdCharacter *pcsCharacter, BOOL bSetStatus)
{
	ASSERT(NULL != pcsCharacter);

	if (NULL == pcsCharacter)
		return FALSE;

	if (bSetStatus)
	{
		pcsCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_TAME;
		pcsCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_WAS_TAMED;
	}
	else
		pcsCharacter->m_ulSpecialStatus &= !AGPDCHAR_SPECIAL_STATUS_TAME;

	return TRUE;
}

BOOL AgpmCharacter::SetStatusSummoner(AgpdCharacter *pcsCharacter, BOOL bSetStatus)
{
	ASSERT(NULL != pcsCharacter);

	if (NULL == pcsCharacter)
		return FALSE;

	if (bSetStatus)
		pcsCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_SUMMONER;
	else
		pcsCharacter->m_ulSpecialStatus &= !AGPDCHAR_SPECIAL_STATUS_SUMMONER;

	return TRUE;
}

// 2005.10.05. steeple
BOOL AgpmCharacter::SetStatusFixed(AgpdCharacter* pcsCharacter, BOOL bSetStatus)
{
	ASSERT(NULL != pcsCharacter);

	if(NULL == pcsCharacter)
		return FALSE;

	if(bSetStatus)
	{
		pcsCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_SUMMONER;
		pcsCharacter->m_ulSpecialStatus |= AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;
	}
	else
	{
		pcsCharacter->m_ulSpecialStatus &= ~AGPDCHAR_SPECIAL_STATUS_SUMMONER;
		pcsCharacter->m_ulSpecialStatus &= ~AGPDCHAR_SPECIAL_STATUS_DONT_MOVE;
	}

	return TRUE;
}

BOOL AgpmCharacter::SetTypeAttackable(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_ATTACKABLE;

	return TRUE;
}

BOOL AgpmCharacter::SetTypeTargetable(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_TARGETABLE;

	return TRUE;
}

BOOL AgpmCharacter::SetTypePolyMorph(AgpdCharacterTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	pcsTemplate->m_ulCharType |= AGPMCHAR_TYPE_POLYMORPH;

	return TRUE;
}

BOOL AgpmCharacter::IsPolyMorph(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	//if (ulCharType != pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType)
	//{
	//	if (pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_POLYMORPH)
	//		return TRUE;
	//}
	//else
	//{
		if (ulCharType & AGPMCHAR_TYPE_POLYMORPH)
			return TRUE;
	//}

	return FALSE;
}


BOOL AgpmCharacter::IsMonster(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType & AGPMCHAR_TYPE_MONSTER)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsPC(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType & AGPMCHAR_TYPE_PC)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsNPC(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType & AGPMCHAR_TYPE_NPC)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsStatusSummoner(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SUMMONER)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsStatusWasTamed(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	
	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_WAS_TAMED)
		return TRUE;
	
	return FALSE;
}

// 2005.10.05. steeple
// Fixed ������ ���� IsStatusSummoner ������ TRUE �� ���ϵȴ�.
BOOL AgpmCharacter::IsStatusFixed(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if((pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SUMMONER) &&
		(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DONT_MOVE))
		return TRUE;

	return FALSE;
}

// ���� �������� �����Ѵ�. 2005.10.04. steeple
BOOL AgpmCharacter::IsStatusTransparent(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_TRANSPARENT)
		return TRUE;

	return FALSE;
}

// ���� �������� �����Ѵ�. 2006.01.09. steeple
BOOL AgpmCharacter::IsStatusInvincible(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_INVINCIBLE)
		return TRUE;

	return FALSE;
}

// �Ӽ� �������� Ȯ��. 2007.07.10. steeple
BOOL AgpmCharacter::IsStatusAttrInvincible(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE)
		return TRUE;

	return FALSE;
}

// ������ �������� �����Ѵ�. 2006.09.21. steeple
BOOL AgpmCharacter::IsStatusHalfTransparent(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT)
		return TRUE;

	return FALSE;
}

// ��������̰� �������� �ƴ��� ����. 2006.09.21. steeple
BOOL AgpmCharacter::IsStatusFullTransparent(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	return IsStatusTransparent(pcsCharacter) && !IsStatusHalfTransparent(pcsCharacter);
}

// �Ϲݰ��ݿ� ���ؼ� ������������ ����. 2007.06.27. steeple
BOOL AgpmCharacter::IsStatusNormalATKInvincible(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE)
		return TRUE;

	return FALSE;
}

// ��ų���ݿ� ���ؼ� ������������ ����. 2007.06.27. steeple
BOOL AgpmCharacter::IsStatusSkillATKInvincible(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE)
		return TRUE;

	return FALSE;
}

// ��ų�� ����� �� ���� �������� ����. 2007.06.27. steeple
BOOL AgpmCharacter::IsStatusDisableSkill(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL)
		return TRUE;

	return FALSE;
}

// �Ϲݰ����� ����� �� ���� �������� ����. 2007.06.27. steeple
BOOL AgpmCharacter::IsStatusDisableNormalATK(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK)
		return TRUE;

	return FALSE;
}

// �ڰ��ִ� �� ����. 2007.06.27. steeple
BOOL AgpmCharacter::IsStatusSleep(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SLEEP)
		return TRUE;

	return FALSE;
}

// Go (Game Operator) ���� ����. 2007.07.29. steeple
BOOL AgpmCharacter::IsStatusGo(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_GO)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsStatusTame(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_TAME)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsCreature(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType & AGPMCHAR_TYPE_CREATURE)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsCreature(AgpdCharacterTemplate* pcsCharacterTemplate)
{
	if(!pcsCharacterTemplate)
		return FALSE;

	if(pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_CREATURE)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsGM(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType & AGPMCHAR_TYPE_GM)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsGuard(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType & AGPMCHAR_TYPE_GUARD)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsPet(AgpdCharacter *pcsAgpdCharacter)
{
	if(pcsAgpdCharacter == NULL)
		return FALSE;

	if(pcsAgpdCharacter->m_pcsCharacterTemplate->m_lStartStaminaPoint > 0)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsAttackable(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType != pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType)
	{
		if (pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_ATTACKABLE)
			return TRUE;
	}
	else
	{
		if (ulCharType & AGPMCHAR_TYPE_ATTACKABLE)
			return TRUE;
	}

	if (IsAllBlockStatus(pcsAgpdCharacter))
		return FALSE;

	return FALSE;
}

BOOL AgpmCharacter::IsTargetable(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType != pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType)
	{
		if (pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_TARGETABLE)
			return TRUE;
	}
	else
	{
		if (ulCharType & AGPMCHAR_TYPE_TARGETABLE)
			return TRUE;
	}

	if (IsAllBlockStatus(pcsAgpdCharacter))
		return FALSE;

	return FALSE;
}

BOOL	AgpmCharacter::IsMovable	(AgpdCharacter *pcsAgpdCharacter)	// �̵��� ������ NPC���� üũ.
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType != pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType)
	{
		if (pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_MOVABLENPC )
			return TRUE;
	}
	else
	{
		if (ulCharType & AGPMCHAR_TYPE_MOVABLENPC )
			return TRUE;
	}

	return FALSE;
}

BOOL	AgpmCharacter::IsTrap	(AgpdCharacter *pcsAgpdCharacter)
{
	UINT32	ulCharType	= GetCharacterType(pcsAgpdCharacter);

	if (ulCharType != pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType)
	{
		if (pcsAgpdCharacter->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_TRAP )
			return TRUE;
	}
	else
	{
		if (ulCharType & AGPMCHAR_TYPE_TRAP )
			return TRUE;
	}

	return FALSE;
}

BOOL	AgpmCharacter::IsInDungeon( AgpdCharacter* pcsCharacter )
{
	ApmMap::RegionTemplate*	pTemplate = m_pcsApmMap->GetTemplate( pcsCharacter->m_nBindingRegionIndex );
	return ( pTemplate && pTemplate->ti.stType.uFieldType == ApmMap::FT_PVP ) ? TRUE : FALSE;
}

BOOL	AgpmCharacter::IsInSiege( AgpdCharacter* pcsCharacter )
{
	AgpmSiegeWar* pSiegeWar = (AgpmSiegeWar*)GetModule( "AgpmSiegeWar" );
	if( !pSiegeWar )	return FALSE;

	return pSiegeWar->GetSiegeWar( pcsCharacter ) ? TRUE : FALSE;

	//���������̸鼭 ������ �ߴ������� üũ
	//AgpmPvP* pPvP = (AgpmPvP*)GetModule( "AgpmPvP" );
	//if( !pPvP )		return FALSE;

	//BOOL bIsInSiegeWarIngArea = FALSE;
	//PVOID pvBuffer[2];
	//pvBuffer[0] = &bIsInSiegeWarIngArea;
	//pvBuffer[1] = NULL;
	//pPvP->EnumCallback( AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, (PVOID)pcsCharacter, pvBuffer );
	//return bIsInSiegeWarIngArea;
}


//		AddCharacter
//	Functions
//		- ĳ������ �޸𸮸� Ȯ���� ���ο� �־��ְ� �� �����͸� �����Ѵ�.
//			* Ư���� �����Ұ� : ���⼭ �߰��Ǵ� ����Ÿ�� ĳ������ status�� ��� ���̱� ����
//								EnumCallback(AGPMCHAR_CB_ID_ADD, )�� ȣ���� ���� ����.
//								���� ���߿� �� ĳ������ �Ӽ����� �� ������ �Ŀ� �̳ѿ� ����
//								AGPMCHAR_CB_ID_ADD �ݹ���� ȣ��������Ѵ�.
//								���� �ʱ⿡ Factor �鵵 ����� �־�� �Ѵ�.
//								ĳ���� status ������ ��� �Ϸ�Ǹ� �ݵ�� UpdateInit()�� ȣ���ؾ� �Ѵ�. (���ϸ� å�� ����)
//	Arguments
//		- lCID		: Character ID
//		- lTID		: Template ID
//		- szGameID	: Game ID (String)
//	Return value
//		- AgpdCharacter *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacter* AgpmCharacter::AddCharacter(INT32 lCID, INT32 lTID, CHAR* szGameID)
{
	PROFILE("AgpmCharacter::AddCharacter");

	AgpdCharacterTemplate *pcsCharacterTemplate;
	AgpdCharacter *pcsCharacter;

	pcsCharacterTemplate = GetCharacterTemplate(lTID);
	if (!pcsCharacterTemplate)
		return NULL;

	pcsCharacter = CreateCharacter();
	if (!pcsCharacter)
		return NULL;

	pcsCharacter->m_bIsActiveData	= TRUE;

	pcsCharacter->m_lID = lCID;
	pcsCharacter->m_lTID1 = lTID;
	pcsCharacter->m_lFixTID = lTID;

	pcsCharacter->m_pcsCharacterTemplate = pcsCharacterTemplate;
	pcsCharacter->m_ulCharType = pcsCharacterTemplate->m_ulCharType;
	pcsCharacter->m_unActionStatus = AGPDCHAR_STATUS_NOT_ACTION;
	if (szGameID)
		strncpy(pcsCharacter->m_szID, szGameID, AGPDCHARACTER_MAX_ID_LENGTH);

	if (m_pcsAgpmFactors)
	{
		PROFILE("AddCharacter - SetOwnerFactor");

		if (!m_pcsAgpmFactors->SetOwnerFactor(&pcsCharacter->m_csFactor, lCID, pcsCharacter))
		{
			DestroyCharacter(pcsCharacter);
			return NULL;
		}
	}

	if (!AddCharacter(pcsCharacter))
	{
		PROFILE("AddCharacter - DestroyCharacter");

		if (szGameID && szGameID[0])
		{
			AgpdCharacter	*pcsTempCharacter1	= GetCharacter(lCID);
			AgpdCharacter	*pcsTempCharacter2	= GetCharacter(szGameID);

			if (!pcsTempCharacter1 && pcsTempCharacter2)
			{
				//if (IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "CharacterID = %s", szGameID);
				AuLogFile_s("LOG\\CharacterTemplateAsync.txt", strCharBuff);

				RemoveCharacter(szGameID);

				return NULL;

			//	if (AddCharacter(pcsCharacter))
			//		return pcsCharacter;
			}
		}

		DestroyCharacter(pcsCharacter);
		return NULL;
	}

	::InterlockedIncrement(&g_lNumCharacter);

	return pcsCharacter;
}

AgpdCharacter* AgpmCharacter::AddCharacter(AgpdCharacter *pcsCharacter)
{
	//@{ Jaewon 20050906
	//TRACEFILE("pickingAtomic.log", "AddCharacter %d %s %d\n", pcsCharacter->m_lID, pcsCharacter->m_szID, m_csACharacter.GetObjectCount());
	//@} Jaewon
	if (!m_csACharacter.AddCharacter(pcsCharacter, pcsCharacter->m_lID, pcsCharacter->m_szID[0] ? pcsCharacter->m_szID : NULL))
	{
		return NULL;
	}

	return pcsCharacter;
}

//		GetCharacter
//	Functions
//		- Character Data�� �����´�.
//	Arguments
//		- lCID		: Character ID
//			or
//		- szGameID	: Game ID
//	Return value
//		- AgpdCharacter *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacter* AgpmCharacter::GetCharacter(INT32 lCID)
{
	PROFILE("AgpmCharacter::GetCharacter()");

	return m_csACharacter.GetCharacter(lCID);
}

AgpdCharacter* AgpmCharacter::GetCharacter(CHAR*	szGameID)
{
	return m_csACharacter.GetCharacter(szGameID);
}

AgpdCharacter* AgpmCharacter::GetCharacterLock(INT32 lCID)
{
	AgpdCharacter *pcsCharacter = GetCharacter(lCID);
	if (!pcsCharacter)
	{
		return NULL;
	}

	if (!pcsCharacter->m_Mutex.WLock())
	{
		return NULL;
	}

	return pcsCharacter;
}

AgpdCharacter* AgpmCharacter::GetCharacterLock(CHAR*	szGameID)
{
	AgpdCharacter *pcsCharacter = GetCharacter(szGameID);
	if (!pcsCharacter)
	{
		return NULL;
	}

	if (!pcsCharacter->m_Mutex.WLock())
	{
		return NULL;
	}

	return pcsCharacter;
}

INT32 __cdecl	QSortCIDCompare(const void *pvData1, const void *pvData2)
{
	return (*((INT32 *) pvData2) - *((INT32 *) pvData1));
}

BOOL AgpmCharacter::GetCharacterLock(INT32 *palCID, INT32 lNumCharacter, AgpdCharacter **ppacsCharacter)
{
	PROFILE("AgpmCharacter::GetCharacterLock()");

	if (!palCID || !ppacsCharacter || lNumCharacter < 1)
		return FALSE;

	qsort(palCID, lNumCharacter, sizeof(INT32), QSortCIDCompare);

	BOOL	bResult	= TRUE;

	int i = 0;
	for (i = 0; i < lNumCharacter; ++i)
	{
		ppacsCharacter[i] = GetCharacterLock(palCID[i]);
		if (!ppacsCharacter[i])
		{
			bResult	= FALSE;

		}
		else
		{
			bResult = TRUE;
		}

		if (!bResult)
			break;
	}

	if (!bResult)
	{
		for (int j = 0; j < i; ++j)
			ppacsCharacter[j]->m_Mutex.Release();
	}

	return bResult;
}

BOOL AgpmCharacter::GetCharacterLockByPointer(INT_PTR *paCharPointer, INT32 lNumCharacter)
{
	PROFILE("AgpmCharacter::GetCharacterLockByPointer");

	if (!paCharPointer || lNumCharacter < 1)
		return FALSE;

	INT32			*palCID				= new INT32[lNumCharacter];
	AgpdCharacter	**ppacsCharacter	= new AgpdCharacter *[lNumCharacter];

	AgpdCharacter	*pcsCharacter	= NULL;

	int i = 0, j = 0;
	for (i = 0; i < lNumCharacter; ++i)
	{
		if (paCharPointer[i] == 0)
		{
			// 2008.03.06. steeple
			// �� �Լ��� ���� ������, �޸� ���� ���� �� �����Ƿ� ó���Ѵ�.
			delete [] palCID;
			delete [] ppacsCharacter;

			return FALSE;
		}

		palCID[i]	= ((AgpdCharacter *) paCharPointer[i])->m_lID;
	}

	qsort(palCID, lNumCharacter, sizeof(INT32), QSortCIDCompare);

	BOOL	bResult	= TRUE;

	for (i = 0; i < lNumCharacter; ++i)
	{
		for (j = 0; j < lNumCharacter; ++j)
		{
			pcsCharacter	= (AgpdCharacter *) paCharPointer[j];

			if (pcsCharacter && palCID[i] == pcsCharacter->m_lID)
			{
				if (!pcsCharacter->m_Mutex.WLock())
				{
					bResult	= FALSE;
					break;
				}
				
				ppacsCharacter[i] = pcsCharacter;
				break;
			}
		}

		if (j == lNumCharacter)
			bResult = FALSE;

		if (!bResult)
			break;

		bResult	= TRUE;
	}

	if (!bResult)
	{
		for (int j = 0; j < i; ++j)
			ppacsCharacter[j]->m_Mutex.Release();
	}

	delete [] palCID;
	delete [] ppacsCharacter;

	return bResult;
}

//		RemoveCharacter
//	Functions
//		- Character Data�� �����.
//			* ������ �� : �ݵ�� REMOVE�� ���� EnumCallback�� ����� �Ѵ�.
//						  ApAdmin���� RemoveObject�� �ϸ� Memory�� �� ���󰣴�.
//						  �׷��Ƿ�, �̸� �ش� Pointer�� ������ �ִٰ� Destroy�� ��
//	Arguments
//		- lCID			: Character ID
//			or
//		- szGameID		: Game ID
//			or
//		- pcsCharacter	: Character Data Pointer
//	Return value
//		- AgpdCharacter *
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::RemoveCharacter(INT32	lCID, BOOL bDelete, BOOL bDestroyModuleData)
{
	PROFILE("AgpmCharacter::RemoveCharacter");

	if (lCID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= GetCharacter(lCID);

	if (!pcsCharacter)
	{
		return FALSE;
	}

	pcsCharacter->m_bIsReadyRemove = TRUE;

	if (::GetCurrentThreadId() == pcsCharacter->m_Mutex.GetOwningThreadID())
	{
		//if (g_eServiceArea == AP_SERVICE_AREA_KOREA &&
		//	m_pcsAgpmConfig &&
		//	m_pcsAgpmConfig->IsTestServer())
		//{
		//	CHAR	*pszCrash	= NULL;
		//	*pszCrash	= 0;
		//}

		pcsCharacter->m_Mutex.Release();
	}

	// ���� !!!!!
	//		Mutex Release�� DestroyCharacter() ������ �Ѵ�. ��� ���⼱ �� �ʿ� ����.
	if (!pcsCharacter->m_Mutex.RemoveLock())
	{
		return FALSE;
	}

	return RemoveCharacter(pcsCharacter, bDelete, bDestroyModuleData);
}

BOOL AgpmCharacter::RemoveCharacter(CHAR *szGameID, BOOL bDelete, BOOL bDestroyModuleData)
{
	if (!szGameID || !strlen(szGameID))
		return FALSE;

	AgpdCharacter	*pcsCharacter	= GetCharacter(szGameID);
	if (!pcsCharacter)
	{
		return FALSE;
	}

	pcsCharacter->m_bIsReadyRemove = TRUE;

	if (::GetCurrentThreadId() == pcsCharacter->m_Mutex.GetOwningThreadID())
	{
		/*
		if (g_eServiceArea == AP_SERVICE_AREA_KOREA &&
			m_pcsAgpmConfig &&
			m_pcsAgpmConfig->IsTestServer())
		{
			CHAR	*pszCrash	= NULL;
			*pszCrash	= 0;
		}
		*/

		pcsCharacter->m_Mutex.Release();
	}

	// ���� !!!!!
	//		Mutex Release�� DestroyCharacter() ������ �Ѵ�. ��� ���⼱ �� �ʿ� ����.
	if (!pcsCharacter->m_Mutex.RemoveLock())
	{
		return FALSE;
	}

	return RemoveCharacter(pcsCharacter, bDelete, bDestroyModuleData);
}

BOOL AgpmCharacter::RemoveCharacter(AgpdCharacter *pcsCharacter, BOOL bDelete, BOOL bDestroyModuleData)
{
	if (!pcsCharacter)
		return FALSE;

	if (!pcsCharacter->m_bIsActiveData)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "already destroyed character data (character_id : %s)\n", pcsCharacter->m_szID);
		AuLogFile_s("LOG\\RCErr.txt", strCharBuff);
		return FALSE;
	}

	pcsCharacter->m_bIsReadyRemove	= TRUE;

	//STOPWATCH2(GetModuleName(), _T("RemoveCharacter"));

	//if (IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
	//{
	//	AuLogFile("RemoveNPC.log", "ProtectedNPC cannot remove (character_id : %s)\n", pcsCharacter->m_szID);
	//	//return FALSE;
	//}

	::InterlockedDecrement(&g_lNumCharacter);

	EnumCallback(AGPMCHAR_CB_ID_REMOVE_RIDE, pcsCharacter, NULL );

	// Transform�� �������̾��ٸ� ������� �ǵ�����.
	RestoreTransformCharacter(pcsCharacter);

	if (bDelete)
		EnumCallback(AGPMCHAR_CB_ID_DELETE, pcsCharacter, NULL);

	// 012903 Bob�� �۾�(Modify, �� ����� ���� ���� ���Ѵ粲~)
	if(m_pcsApmMap && (pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD || pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_RETURN_TO_LOGIN_SERVER))
	{
		RemoveCharacterFromMap(pcsCharacter);
	}

	EnumCallback(AGPMCHAR_CB_ID_REMOVE, pcsCharacter, NULL);

	//@{ Jaewon 20050906
	//TRACEFILE("pickingAtomic.log", "RemoveCharacter %d %s %d\n",et pcsCharacter->m_lID, pcsCharacter->m_szID, m_csACharacter.GetObjectCount());
	//@} Jaewon
	m_csACharacter.RemoveCharacter(pcsCharacter->m_lID, pcsCharacter->m_szID[0] ? pcsCharacter->m_szID:NULL);

	EnumCallback(AGPMCHAR_CB_ID_REMOVE_ID, pcsCharacter, NULL);

	//pcsCharacter->m_Mutex.Destroy();
	pcsCharacter->m_Mutex.SafeRelease();

	if (m_csAdminCharacterRemove.GetCount() > 0 && !bDestroyModuleData)
		return AddRemoveCharacter(pcsCharacter);
	else
	{
		//TRACEFILE(ALEF_ERROR_FILENAME,"AgpmCharacter::RemoveCharacter() Remove Pool Full !!!\n"); 
		return DestroyCharacter(pcsCharacter);
	}
}

BOOL AgpmCharacter::RemoveAllCharacters()
{
	INT32	lIndex	= 0;

	AgpdCharacter	*pcsCharacter	= GetCharSequence(&lIndex);
	while (pcsCharacter)
	{
		RemoveCharacter(pcsCharacter->m_lID, FALSE, TRUE);

		pcsCharacter	= GetCharSequence(&lIndex);
	}

	return TRUE;
}

AgpdCharacter* AgpmCharacter::RemoveAllCharactersExceptOne(INT32 lCID)
{
	AgpdCharacter	*pcsExceptChar	= NULL;

	INT32	lIndex	= 0;

	AgpdCharacter	*pcsCharacter	= GetCharSequence(&lIndex);
	while (pcsCharacter)
	{
		if (pcsCharacter->m_lID == lCID)
		{
			pcsExceptChar	= pcsCharacter;

			// 012903 Bob�� �۾�(Modify, �� ����� ���� ���� ���Ѵ粲~)
			if(m_pcsApmMap && pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
			{
				RemoveCharacterFromMap(pcsCharacter);
			}

			EnumCallback(AGPMCHAR_CB_ID_REMOVE, pcsCharacter, NULL);

			// ��ⵥ��Ÿ�� �����.
			m_csACharacter.RemoveCharacter(lCID, pcsCharacter->m_szID);
		}
		else
		{
			RemoveCharacter(pcsCharacter->m_lID, FALSE, TRUE);
		}

		pcsCharacter	= GetCharSequence(&lIndex);
	}

	return pcsExceptChar;
}

BOOL AgpmCharacter::ResetMonsterData(AgpdCharacter *pcsCharacter, INT32 lNewTID)
{
	if (!pcsCharacter)
		return FALSE;

	pcsCharacter->m_bIsActiveData	= TRUE;
	pcsCharacter->m_bIsReadyRemove	= FALSE;

	AgpdCharacterTemplate	*pcsNewTemplate	= GetCharacterTemplate(lNewTID);
	if (!pcsNewTemplate)
		return FALSE;

	{
		AuMath::MatrixIdentity(&pcsCharacter->m_stMatrix);

		pcsCharacter->m_ulRemoveTimeMSec	= 0;

		pcsCharacter->m_ulSyncMoveTime		= 0;

		pcsCharacter->m_lAdjustMoveSpeed	= 0;

		// ������ (2004-09-17 ���� 5:18:27) : 
		pcsCharacter->m_ulCheckBlocking		= ApTileInfo::BLOCKGEOMETRY;

		pcsCharacter->m_bIsActionBlock		= FALSE;
		pcsCharacter->m_ulActionBlockTime	= 0;
		pcsCharacter->m_eActionBlockType	= AGPDCHAR_ACTION_BLOCK_TYPE_ALL;

		pcsCharacter->m_bIsPostedIdleProcess	= FALSE;

		pcsCharacter->m_nDimension		= 0;			// �� �ε��� �ʱ�ȭ

		pcsCharacter->m_lOptionFlag		= 0;			// Option����Ʈ �ʱ�ȭ( 2005.05.31 By SungHoon )

		pcsCharacter->m_ulSpecialStatus	= 0;
	}

	pcsCharacter->m_lTID1 = lNewTID;
	pcsCharacter->m_pcsCharacterTemplate = pcsNewTemplate;
	pcsCharacter->m_ulCharType = pcsNewTemplate->m_ulCharType;
	pcsCharacter->m_unActionStatus = AGPDCHAR_STATUS_NOT_ACTION;

	m_pcsAgpmFactors->InitFactor(&pcsCharacter->m_csFactor);
	m_pcsAgpmFactors->CopyFactor(&pcsCharacter->m_csFactor, &pcsNewTemplate->m_csFactor, TRUE);
	m_pcsAgpmFactors->SetOwnerFactor(&pcsCharacter->m_csFactor, pcsCharacter->m_lID, pcsCharacter);

	return EnumCallback(AGPMCHAR_CB_ID_RESET_MONSTER, pcsCharacter, pcsNewTemplate);
}

//		AddCharacterTemplate
//	Functions
//		- Character Template Data�� Add �Ѵ�.
//	Arguments
//		- lTID		: Character Template ID
//	Return value
//		- AgpdCharacterTemplate *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacterTemplate * AgpmCharacter::AddCharacterTemplate(INT32 lTID)
{
	AgpdCharacterTemplate *pcsTemplate;

	pcsTemplate = CreateTemplate();
	if (!pcsTemplate)
		return NULL;

	pcsTemplate->m_lID = lTID;
	if (!m_csACharacterTemplate.AddCharacterTemplate(pcsTemplate, lTID))
	{
		DestroyTemplate(pcsTemplate);
		return NULL;
	}

	return pcsTemplate;
}

//		GetCharacterTemplate
//	Functions
//		- Character Template Data�� �����´�.
//	Arguments
//		- lTID		: Character Template ID
//	Return value
//		- AgpdCharacterTemplate *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacterTemplate* AgpmCharacter::GetCharacterTemplate(INT32 ulTID)
{
	PROFILE("GetCharacterTemplate");

	return m_csACharacterTemplate.GetCharacterTemplate(ulTID);
}

//		GetCharacterTemplate
//	Functions
//		- Character Template Data�� �����´�.
//	Arguments
//		- szTName		: Character Template name
//	Return value
//		- AgpdCharacterTemplate *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacterTemplate* AgpmCharacter::GetCharacterTemplate(CHAR *szTName)
{
	return m_csACharacterTemplate.GetCharacterTemplate(szTName);
}

//		GetCharacterTemplateUseCID
//	Functions
//		- Character Data�� �̿��ؼ� Template Data�� �����´�.
//	Arguments
//		- lCID		: Character ID
//	Return value
//		- AgpdCharacterTemplate *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacterTemplate* AgpmCharacter::GetCharacterTemplateUseCID(INT32 lCID)
{
	AgpdCharacter *pCharacter = m_csACharacter.GetCharacter(lCID);

	if (pCharacter == NULL)
		return NULL;

	return (AgpdCharacterTemplate *) pCharacter->m_pcsCharacterTemplate;
}

// Add 090802 Bob Jung.
//		RemoveCharacterTemplate
//	Functions
//		- Character Template Data�� Remove�Ѵ�.
//	Arguments
//		- lTID			: Character Template ID
//			or
//		- pcsTemplate	: Template Data Pointer
//	Return value
//		- AgpdCharacterTemplate *
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::RemoveCharacterTemplate(INT32 lTID)
{
	return RemoveCharacterTemplate(m_csACharacterTemplate.GetCharacterTemplate(lTID));
}

BOOL AgpmCharacter::RemoveAllCharacterTemplate()
{
	INT32 lIndex = 0;
	for(AgpdCharacterTemplate *pcsTemplate = GetTemplateSequence(&lIndex);
		pcsTemplate;
		pcsTemplate = GetTemplateSequence(&lIndex)							)
	{
		if(!RemoveCharacterTemplate(pcsTemplate))
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmCharacter::RemoveCharacterTemplate(AgpdCharacterTemplate* pcsTemplate)
{
	if(!pcsTemplate)
		return FALSE;

	if(!m_csACharacterTemplate.RemoveCharacterTemplate(pcsTemplate->m_lID, pcsTemplate->m_szTName))
	{
		OutputDebugString("Character Template�� �ִµ� ����������.");
//		return FALSE;
	}

	DestroyTemplate(pcsTemplate);

	return TRUE;
}

BOOL AgpmCharacter::GetRaceCharacterTemplate(INT32 lRace, INT32 *alTID, INT32 lBufferSize)
{
	if (lRace <= AURACE_TYPE_NONE || lRace >= AURACE_TYPE_MAX || !alTID || lBufferSize <= 0)
		return FALSE;

	INT32					lBufferIndex	= 0;
	INT32					lIndex			= 0;

	AgpdCharacterTemplate	*pcsTemplate	= GetTemplateSequence(&lIndex);

	while (pcsTemplate && lBufferIndex < lBufferSize)
	{
		INT32	lTemplateRace = 0;
		
		if (m_pcsAgpmFactors)
		{
			lTemplateRace = m_pcsAgpmFactors->GetRace(&pcsTemplate->m_csFactor);
		}

		if (lTemplateRace == lRace)
		{
			if (lBufferIndex > lBufferSize)
				return FALSE;

			alTID[lBufferIndex++] = pcsTemplate->m_lID;
		}

		pcsTemplate	= GetTemplateSequence(&lIndex);
	}

	return TRUE;
}

INT32 AgpmCharacter::GetRaceFromTemplate(AgpdCharacterTemplate* pcsTemplate)
{
	if(!pcsTemplate)
		return FALSE;

	return m_pcsAgpmFactors->GetRace(&pcsTemplate->m_csFactor);
}

//		IsCharacter
//	Functions
//		- Character Data�� Valid �Ѱ�.
//	Arguments
//		- lCID		: Character ID
//			or
//		- szGameID	: Game ID
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::IsCharacter(INT32 lCID)
{
	return m_csACharacter.IsCharacter(lCID);
}

BOOL AgpmCharacter::IsCharacter(CHAR* szGameID)
{
	return m_csACharacter.IsCharacter(szGameID);
}

//		GetCharSequence
//	Functions
//		- Character Data Sequence ��������.
//	Arguments
//		- plIndex	: Index (internal use, ó���� 0�̾�� �ȴ�.)
//	Return value
//		- AgpdCharacter *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacter* AgpmCharacter::GetCharSequence(INT32 *plIndex)
{
	AgpdCharacter **pcsCharacter = (AgpdCharacter **) m_csACharacter.GetObjectSequence(plIndex);

	if (!pcsCharacter)
		return NULL;

	return  *pcsCharacter;
}

//		GetTemplateSequence
//	Functions
//		- Character Template Data Sequence ��������.
//	Arguments
//		- plIndex	: Index (internal use, ó���� 0�̾�� �ȴ�.)
//	Return value
//		- AgpdCharacterTemplate *
///////////////////////////////////////////////////////////////////////////////
AgpdCharacterTemplate* AgpmCharacter::GetTemplateSequence(INT32 *plIndex)
{
	AgpdCharacterTemplate **pcsTemplate = (AgpdCharacterTemplate **) m_csACharacterTemplate.GetObjectSequence(plIndex);

	if (!pcsTemplate)
		return NULL;

	return  *pcsTemplate;
}

//		SetMaxCharacter / SetMaxCharacterTemplate
//	Functions
//		- ȯ�� ������ Set �Ѵ�.
//			* �� Application���� ���� ������ Character�� Template�� ������ ���Ѵ�.
//			  
//	Arguments
//		- nCount		: Max Count
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::SetMaxCharacter(INT32 nCount)
{
	return m_csACharacter.SetCount(nCount);
}

BOOL AgpmCharacter::SetMaxCharacterTemplate(INT32 nCount)
{
	return m_csACharacterTemplate.SetCount(nCount);
}

INT32 AgpmCharacter::GetMaxCharacterTemplate( void )
{
	return m_csACharacterTemplate.GetCount();
}

BOOL AgpmCharacter::SetMaxCharacterRemove(INT32 nCount)
{
	return m_csAdminCharacterRemove.SetCount(nCount);
}

BOOL AgpmCharacter::SetIdleIntervalMSec(UINT32 lIntervalClock)
{
	m_ulIntervalClock	= lIntervalClock;

	return TRUE;
}

BOOL AgpmCharacter::SetProcessMoveFollowInterval(UINT32 ulIntervalClock)
{
	m_ulProcessMoveFollowInterval	= ulIntervalClock;

	return TRUE;
}

//		Attach~Data
//	Functions
//		- Module Data�� Attach �Ѵ�.
//			* ���� Module ���� �� �Լ��� �̿��ؼ� Attach �Ѵ�.
//			  
//	Arguments
//		- pClass		: Callback Class
//		- nDataSize		: Attach �� Data�� Size
//		- pfConstructor	: Data Construction Function
//		- pfDestructor	: Data Destruction Function
//	Return value
//		- INT16			: Attach �� Index (�����ؾ� �ȴ�. ���߿� Attached Data �����ö�)
///////////////////////////////////////////////////////////////////////////////
INT16 AgpmCharacter::AttachCharacterData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMCHAR_DATA_TYPE_CHAR, nDataSize, pfConstructor, pfDestructor);
}

INT16 AgpmCharacter::AttachCharacterTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, AGPMCHAR_DATA_TYPE_TEMPLATE, nDataSize, pfConstructor, pfDestructor);
}

//		InitFactor
//	Functions
//		- Character�� Factor�� Init �Ѵ�.
//			* Factor ��ü�� Init�� �ص�. ���� Template Data�κ��� �ʱ�ȭ �ϴ� �Ŵ� ���ص�.
//			  
//	Arguments
//		- pCharacter	: Character Data Pointer
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::InitFactor(AgpdCharacter *pCharacter)
{
	return TRUE;
}

BOOL AgpmCharacter::CheckActionStatus(INT32 lCID, INT16 nActionType)
{
	return CheckActionStatus(GetCharacter(lCID), nActionType);
}

//		CheckActionStatus
//	Functions
//		- character�� Ư�� �ൿ�� ���� �� �ִ���, �ִ� �������� �˻��� ����� �����Ѵ�.
//	Arguments
//		- pcsCharacter	: Character data pointer
//		- nActionType	: pcsCharacter�� ���ϰ� �;��ϴ� action type
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::CheckActionStatus(AgpdCharacter *pcsCharacter, INT16 nActionType)
{
	if (!pcsCharacter)
		return FALSE;

	switch (nActionType) {
	case AGPDCHAR_STATUS_READY_SKILL:
		{
			if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK)
				return TRUE;
		}
	case AGPDCHAR_STATUS_MOVE:
	case AGPDCHAR_STATUS_ATTACK:
	case AGPDCHAR_STATUS_TRADE:
	case AGPDCHAR_STATUS_PARTY:
		{
			if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_NOT_ACTION ||
				pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE ||
				pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_NORMAL)
				return TRUE;
		}
		break;

	default:
		return FALSE;
		break;
	}

	if (IsAllBlockStatus(pcsCharacter))
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::CheckNormalAttack(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, BOOL bForceAttack)
{
	if (!pcsCharacter || !pcsTarget)
		return FALSE;

	// ������ ��������.. ���� ���ϴ°� �������� �˻��Ѵ�.
	if (!IsAttackable(pcsCharacter) ||
		!IsTargetable(pcsTarget))
		return FALSE;

	// 2005.01.14. steeple �ּ�ó��.
	// PvP �������� ���ؼ� ������ Union üũ�� �ǹ̰� ����.
	//
	//
	//INT32	lAttackUnion	= GetUnion(pcsCharacter);
	//INT32	lTargetUnion	= GetUnion(pcsTarget);

	//if (lAttackUnion == (-1))
	//	return FALSE;

	//// �Ѵ� PC�̰� ���Ͽ��� ���� ��������.
	//if (IsPC(pcsCharacter) &&
	//	IsPC(pcsTarget))
	//{
	//	if (!m_bIsAcceptPvP)
	//		return FALSE;

	//	if (lAttackUnion == lTargetUnion && !bForceAttack)
	//		return FALSE;
	//}

	return TRUE;
}

BOOL AgpmCharacter::CheckVaildNormalAttackTarget(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTarget, BOOL bForceAttack)
{
	if (!pcsCharacter || !pcsTarget)
		return FALSE;

	if (IsAllBlockStatus(pcsCharacter) || IsAllBlockStatus(pcsTarget))
		return FALSE;

	PVOID	pvBuffer[2];
	pvBuffer[0]	= (PVOID) pcsTarget;
	pvBuffer[1]	= IntToPtr((INT32)bForceAttack);

	return EnumCallback(AGPMCHAR_CB_ID_CHECK_VALID_NORMAL_ATTACK, pcsCharacter, pvBuffer);
}

BOOL AgpmCharacter::CheckActionRequirement(AgpdCharacter *pcsCharacter, AgpdCharacterActionResultType *peActionResult, AgpdCharacterActionType eType)
{
	if (!pcsCharacter || !peActionResult)
		return FALSE;

	// �ü��ΰ�� ȭ���� �־�� ������ �����ϴ�. ȭ���� �κ��丮�� �ִ��� �˻縦 �ؾ��Ѵ�.
	// ���� ����� ó���ϰ� �ݹ��� �ҷ��ش�.

	// ������ ���(���⸦ ����ִٸ�)�� MP�� �־�� �Ѵ�. ó���� �ش�.
//	if (!IsSufficientCost(pcsCharacter, eType))
//	{
//		*peActionResult	= AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP;
//		return FALSE;
//	}

	return EnumCallback(AGPMCHAR_CB_ID_CHECK_ACTION_REQUIREMENT, pcsCharacter, peActionResult);
}

BOOL AgpmCharacter::PayActionCost(AgpdCharacter *pcsCharacter, INT16 nActionType)
{
	if (!pcsCharacter)
		return FALSE;

	return EnumCallback(AGPMCHAR_CB_ID_PAY_ACTION_COST, pcsCharacter, &nActionType);
}

//		SetCallback~
//	Functions
//		- Callback �Լ����� ����Ѵ�.
//			  
//	Arguments
//		- pfCallback	: Callback Function
//		- pClass		: Callback Class
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::SetCallbackInitChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_INIT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackNewCreatedCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_NEW_CREATED_CHAR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackAddCharPost(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_POST_ADD, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRemoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_REMOVE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRemoveID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_REMOVE_ID, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackResetMonster(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RESET_MONSTER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackDeleteChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_DELETE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackMoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_MOVE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackStopChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_STOP, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_STATUS, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateActionStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_ACTION_STATUS, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateActionStatusCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_ACTION_STATUS_CHECK, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateSpecialStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_SPECIAL_STATUS, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateCriminalStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_CRIMINAL_STATUS, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_CHAR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdatePositionCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_POSITION_CHECK, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdatePosition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_POSITION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackPreUpdatePosition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_PRE_UPDATE_POSITION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckIsActionMove(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_IS_ACTION_MOVE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_ATTACK, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionAttackMiss(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_ATTACK_MISS, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_SKILL, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionPickupItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_PICKUP_ITEM, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_TELEPORT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventNPCTrade(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_NPC_TRADE, pfCallback, pClass);
}

//BOOL AgpmCharacter::SetCallbackActionEventMasterySpecialize(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_MASTERY_SPECIALIZE, pfCallback, pClass);
//}

BOOL AgpmCharacter::SetCallbackActionEventBank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_BANK, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventItemConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_ITEMCONVERT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMHCAR_CB_ID_ACTION_EVENT_GUILD, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventGuildWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMHCAR_CB_ID_ACTION_EVENT_GUILD_WAREHOUSE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventProduct(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_PRODUCT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventNPCDialog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_NPC_DIALOG, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventSkillMaster(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_SKILL_MASTER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventRefinery(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_REFINERY, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_QUEST, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionProductSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_PRODUCT_SKILL, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventCharCustomize(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_CHARCUSTOMIZE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventItemRepair(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_ITEMREPAIR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventRemission(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_REMISSION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventWantedCriminal(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_WANTEDCRIMINAL, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_ARCHLORD, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_SIEGE_WAR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventTax(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_TAX, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventGamble(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_GAMBLE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionEventGacha(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_EVENT_GACHA, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionUseSiegeWarAttackObject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_USE_SIEGEWAR_ATTACK_OBJECT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackActionCarveASeal(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ACTION_CARVE_A_SEAL, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackTransformAppear(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_TRANSFORM_APPEAR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackTransformStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_TRANSFORM_STATUS, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRestoreTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RESTORE_TRANSFORM, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCancelTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CANCEL_TRANSFORM, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackEvolution(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_EVOLUTION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRestoreEvolution(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RESTORE_EVOLUTION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateMurdererPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_MURDERER_POINT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateMukzaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_MUKZA_POINT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateCharismaPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_CHARISMA_POINT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateSkillPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_SKILL_POINT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRemoveRide(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_REMOVE_RIDE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackAlreadyExistChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ALREADY_EXIST_CHAR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateEventStatusFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_EVENTSTATUSFLAG, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackInitTemplateDefaultValue(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_INIT_TEMPLATE_DEFAULT_VALUE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackReadAllCharacterTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_READ_ALL_CHARACTER_TEMPLATE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckAllBlockStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_ALL_BLOCK_STATUS, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackAddStaticCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ADD_STATIC_CHARACTER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRemoveProtectedNPC(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_REMOVE_PROTECTED_NPC, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackIsStaticCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_IS_STATIC_CHARACTER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackIsArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_IS_ARHCLORD, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGetBonusDropRate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_GET_BONUS_DROP_RATE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGetBonusDropRate2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_GET_BONUS_DROP_RATE2, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGetBonusMoneyRate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_GET_BONUS_MONEY_RATE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGetBonusCharismaRate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_GET_BONUS_CHARISMA_RATE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateNickName(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_NICKNAME, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackOnlineTime(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ONLINE_TIME, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGameguardAuth( ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_GAMEGUARD_AUTH, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackResurrectionByOther(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RESURRECTION_BY_OTHER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackEventEffectID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_EVENT_EFFECT_ID, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackPinchWantedAnswer( ApModuleDefaultCallBack pfCallback , PVOID pClass )
{
	return SetCallback(AGPMCHAR_CB_ID_PINCHWANTED_ANSWER , pfCallback , pClass );
}

BOOL AgpmCharacter::SetCallbackPinchWantedRequest( ApModuleDefaultCallBack pfCallback , PVOID pClass )
{
	return SetCallback(AGPMCHAR_CB_ID_PINCHWANTED_REQUEST, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckPCRoomType( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRequestCoupon( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(AGPMCHAR_CB_ID_REQUEST_COUPON, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallBackRecieveCouponInfo( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(AGPMCHAR_CB_ID_RECIEVE_COUPON_INFO, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallBackRecieveCouponUse( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(AGPMCHAR_CB_ID_RECIEVE_COUPON_USE, pfCallback, pClass );
}

BOOL
AgpmCharacter::SetCallBackCheckMovementLock(
	ApModuleDefaultCallBack pfCallback, 
	PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_MOVEMENT_LOCK, 
		pfCallback, pClass );
}

/*
BOOL AgpmCharacter::SetCallbackReCalcFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RECALC_FACTOR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateHP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_HP, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateMaxHP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_MAX_HP, pfCallback, pClass);
}
*/

BOOL AgpmCharacter::SetCallbackUpdateLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_LEVEL, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckRemoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_REMOVE_CHAR, pfCallback, pClass);
}

//@{ Jaewon 20050902
// For fade-in/out
BOOL AgpmCharacter::SetCallbackCheckRemoveChar2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_REMOVE_CHAR2, pfCallback, pClass);
}
//@} Jaewon

BOOL AgpmCharacter::SetCallbackAddAttackerToList(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ADD_ATTACKER_TO_LIST, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackReceiveAction(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RECEIVE_ACTION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckNowUpdateActionFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_NOW_UPDATE_ACTION_FACTOR, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckValidNormalAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_VALID_NORMAL_ATTACK, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckActionRequirement(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_ACTION_REQUIREMENT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckActionAttackTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_ACTION_ATTACK_TARGET, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackPayActionCost(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_PAY_ACTION_COST, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackReceiveCharacterData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RECEIVE_CHARACTER_DATA, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackDisconnectByAnotherUser( ApModuleDefaultCallBack pfCallback, PVOID pClass	)
{
	return SetCallback( AGPMCHAR_CB_ID_DISCONNECT_BY_ANOTHER_USER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackMonsterAIPathFind( ApModuleDefaultCallBack pfCallback, PVOID pClass	)
{
	return SetCallback( AGPMCHAR_CB_ID_MONSTER_AI_PATHFIND, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackStreamReadImportData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_STREAM_READ_IMPORT_DATA, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackStreamReadImportDataErrorReport(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_STREAM_READ_IMPORT_DATA_ERROR_REPORT, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRequestResurrection(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_REQUEST_RESURRECTION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGetNewCID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_GET_NEW_CID, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackIsPlayerCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_IS_PLAYER_CHARACTER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackAddCharacterToMap(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_ADD_CHARACTER_TO_MAP, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackRemoveCharacterFromMap(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_REMOVE_CHARACTER_FROM_MAP, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackSetCombatMode(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_SET_COMBAT_MODE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackResetCombatMode(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RESET_COMBAT_MODE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackCheckProcessIdle(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_CHECK_PROCESS_IDLE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackBindingRegionChange(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackSocialAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_SOCIAL_ANIMATION, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackReleaseActionMoveLock(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_RELEASE_ACTION_MOVE_LOCK, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateCustomize(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_CUSTOMIZE, pfCallback, pClass);
}

/*
	2005.05.31. By SungHoon
	�ź� ����� ���� ��� �Ҹ����� �ݹ��Լ� ���
*/
BOOL AgpmCharacter::SetCallbackUpdateOptionFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_OPTION_FLAG, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateBankSize(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_BANK_SIZE, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGetItemLancer(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_GET_ITEM_LANCER, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUseEffect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_USE_EFFECT, pfCallback, pClass);
}

//		OnReceive
//	Functions
//		- ���� ���� Character Packet�� ó���Ѵ�.
//			* AGPMCHARACTER_PACKET_TYPE ������ Packet�� ������ ModuleManager���� ����� �����ش�.
//			* AuPacket�� �̿��ؼ� Parsing �� �� ������ ó���� ���ش�.
//			* ����� ���� Test�� ���� ó���� �Ǿ��ִ�.
//			  
//	Arguments
//		- ulType	: Packet Type
//		- pvPacket	: ���� Packet Data
//		- nSize		: Packet�� ����
//		- ulNID		: Packet�� ���� Network ID (Server������ ����.)
//	Return value
//		- BOOL
//  Modify
//      - 093002 Bob Jung.-���ؽ� �� �κ� ����.
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmCharacter::OnReceive");

	// m_csPacket
	INT8			cOperation = -1;
	INT32			lCID = 0;
	INT32			lTID = 0;
	//CHAR			*szName = NULL;
	CHAR			*szMessage	= NULL;
	UINT16			unMessageLength	= 0;

	INT8			cStatus = -1;
	INT8			cActionStatus = -1;
	INT8			cCriminalStatus = -1;
	PVOID			pvMovePacket	= NULL;
	PVOID			pvActionPacket	= NULL;
	PVOID			pvFactorPacket	= NULL;
	INT64			llMoneyCount = -1;
	INT64			llBankMoney = -1;
	INT64			llCash = -1;
	INT32			lAttackerID = AP_INVALID_CID;
	INT8			cIsNewChar	= -1;
	UINT8			ucRegionIndex	= 0xff;
	UINT8			ucSocialAnimationIndex	= 0xff;
	UINT64			ulSpecialStatus	= AGPDCHAR_SPECIAL_STATUS_NONE;
	INT8			cIsTransform	= -1;
	CHAR			*szSkillInit	= NULL;
	CHAR			*szNickName		= NULL;
	PVOID			pggAuthData	= 0;
	UINT16			unGGLength = 0;
	

	INT8			cFaceIndex	= -1;
	INT8			cHairIndex	= -1;

	INT32			lOptionFlag	= 0;

	INT8			cBankSize	= -1;
	UINT16			unEventStatusFlag	= 0;

	INT32			lRemainedCriminalTime = -1;
	INT32			lRemainedMurdererTime = -1;
	INT32			lWantedCriminal = -1;

	UINT32			lLastKilledTimeInBattleSquare = 0;

	{
		PROFILE("AgpmCharacter::OnReceive GetField");

		m_csPacket.GetField(TRUE, pvPacket, nSize,														
							&cOperation,										// Operation						
							&lCID,												// Character ID
							&lTID,												// Character Template ID
							&szMessage, &unMessageLength,						// Game ID
							&cStatus,											// Character Status
							&pvMovePacket,										// Move Packet
							&pvActionPacket,									// Action Packet
							&pvFactorPacket,									// Factor Packet
							&llMoneyCount,										// llMoney
							&llBankMoney,										// bank money
							&llCash,											// cash
							&cActionStatus,										// character action status
							&cCriminalStatus,									// character criminal status
							&lAttackerID,										// attacker id (������� ������ �ʿ�)
							&cIsNewChar,										// ���� �����Ǽ� �ʿ� �������� ����
							&ucRegionIndex,										// region index
							&ucSocialAnimationIndex,							// social action index
							&ulSpecialStatus,									// special status
							&cIsTransform,										// is transform status
							&szSkillInit,										// skill initialization text
							&cFaceIndex,										// face index
							&cHairIndex,										// hair index
							&lOptionFlag,										// Option Flag
							&cBankSize,											// bank size
							&unEventStatusFlag,									// event status flag
							&lRemainedCriminalTime,								// remained criminal status time
							&lRemainedMurdererTime,								// remained murderer point time
							&szNickName,										// nick name
							&pggAuthData, &unGGLength,							// gameguard auth data
							&lLastKilledTimeInBattleSquare						// last killed time in battlesquare
							);
	}

	// ����ü�� Packet�� CID�� �ǹ������� �־ ���� �ʴ´�.
	if(cOperation != AGPMCHAR_PACKET_OPERATION_REQUEST_PINCHWANTED_CHARACTER &&
		cOperation != AGPMCHAR_PACKET_OPERATUIN_ANSWER_PINCHWANTED_CHARACTER &&
		cOperation != AGPMCHAR_PACKET_OPERATION_REQUEST_COUPON &&
		cOperation != AGPMCHAR_PACKET_OPERATION_COUPON_INFO &&
		cOperation != AGPMCHAR_PACKET_OPERATION_REQUEST_COUPON_USE)
	{
		if (!pstCheckArg->bReceivedFromServer &&
			pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
			lCID != pstCheckArg->lSocketOwnerID)
		{
			printf("\n  Received Invalid CID Packet (AgpmCharacter)\n");
			printf("      CID : %d, SocketOwnerID : %d\n", lCID, pstCheckArg->lSocketOwnerID);
			return FALSE;
		}
	}

	//TRACE("Character Packet : %d,%d,%d\n", lCID, lTID, cOperation);

	CHAR	*szName	= NULL;

	CHAR	szNameBuffer[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	ZeroMemory(szNameBuffer, sizeof(CHAR) * (AGPDCHARACTER_MAX_ID_LENGTH + 1));

	if (szMessage && unMessageLength > 0)
	{
		if (unMessageLength > AGPDCHARACTER_MAX_ID_LENGTH)
			unMessageLength	= AGPDCHARACTER_MAX_ID_LENGTH;

		CopyMemory(szNameBuffer, szMessage, sizeof(CHAR) * unMessageLength);
		szName	= szNameBuffer;
	}

	CHAR szTmp[32];
	sprintf(szTmp, "OnReceive; operation : %d", (INT32)cOperation);
	//STOPWATCH2(GetModuleName(), szTmp);

	switch (cOperation)
	{
	case AGPMCHAR_PACKET_OPERATION_ADD:
		{
			if (pstCheckArg->bReceivedFromServer)
			{
				if (OnOperationAdd(pstCheckArg, lCID, lTID, szName, cStatus, cActionStatus, cCriminalStatus, pvMovePacket, pvActionPacket, pvFactorPacket, llMoneyCount, llBankMoney, llCash, cIsNewChar, ucRegionIndex, ulSpecialStatus, cIsTransform, szSkillInit, cFaceIndex, cHairIndex, cBankSize, unEventStatusFlag, lRemainedCriminalTime, lRemainedMurdererTime, lWantedCriminal, szNickName , 1 , 1, lLastKilledTimeInBattleSquare))
				{
					printf("\n  Add Character Success (%d, %s) \n", lCID, szName);
				}
				else
				{
					printf("\n  Add Character Fail (%d, %s) \n", lCID, szName);
				}
			}
			else
			{
				printf("\n  Add Character Fail : bReceivedFromServer is FALSE (%d, %s) \n", lCID, szName);
			}
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_UPDATE:
		{
			PROFILE("AgpmCharacter::OnReceive Operation Update");

			OnOperationUpdate(pstCheckArg, lCID, lTID, szName, cStatus, cActionStatus, cCriminalStatus, pvMovePacket, pvActionPacket, pvFactorPacket, llMoneyCount, llBankMoney, llCash, cIsNewChar, ucRegionIndex, ulSpecialStatus, cIsTransform, cFaceIndex, cHairIndex, cBankSize, unEventStatusFlag, lRemainedCriminalTime, lRemainedMurdererTime, lWantedCriminal, szNickName , lOptionFlag);

			EnumCallback(AGPMCHAR_CB_ID_RECEIVE_CHARACTER_DATA, &lCID, NULL);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_REMOVE:
		{
			if (pstCheckArg->bReceivedFromServer)
			{
				OnOperationRemove(lCID, lTID, szName, cStatus, pvMovePacket, pvActionPacket, pvFactorPacket, ulNID);

				printf("\n  Remove Character (%d, %s) \n", lCID, szName);
			}
			else
			{
				printf("\n  Remove Character Error (%d, %s) \n", lCID, szName);
			}
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_REMOVE_FOR_VIEW:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationRemoveForView(lCID);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_LEVEL_UP:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationLevelUp(lCID, pvFactorPacket);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_ADD_ATTACKER:
		{
			if (pstCheckArg->bReceivedFromServer)
				OnOperationAddAttacker(lCID, lAttackerID);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_MOVE_BANKMONEY:
		{
			OnOperationMoveBankMoney(pstCheckArg, lCID, llBankMoney);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_DISCONNECT_BY_ANOTHER_USER:
		{
			EnumCallback( AGPMCHAR_CB_ID_DISCONNECT_BY_ANOTHER_USER, NULL, NULL );
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_TOWN:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			AgpdCharacter	*pcsCharacter		= GetCharacterLock(lCID);

			INT32			lResurrection		= AGPMCHAR_RESURRECT_TOWN;

			if (pcsCharacter)
			{
				EnumCallback( AGPMCHAR_CB_ID_REQUEST_RESURRECTION, pcsCharacter, &lResurrection );
				pcsCharacter->m_Mutex.Release();
			}
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_NOW:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			return TRUE;

			AgpdCharacter	*pcsCharacter		= GetCharacterLock(lCID);

			INT32			lResurrection		= AGPMCHAR_RESURRECT_NOW;

			if (pcsCharacter)
			{
				EnumCallback( AGPMCHAR_CB_ID_REQUEST_RESURRECTION, pcsCharacter, &lResurrection );
				pcsCharacter->m_Mutex.Release();
			}
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_SIEGE_INNER:
	case AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_SIEGE_OUTER:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			AgpdCharacter	*pcsCharacter		= GetCharacterLock(lCID);

			INT32			lResurrection	= AGPMCHAR_RESURRECT_SIEGE_INNER;
			if (cOperation == AGPMCHAR_PACKET_OPERATION_REQUEST_RESURRECTION_SIEGE_OUTER)
				lResurrection	= AGPMCHAR_RESURRECT_SIEGE_OUTER;

			if (pcsCharacter)
			{
				EnumCallback( AGPMCHAR_CB_ID_REQUEST_RESURRECTION, pcsCharacter, &lResurrection );
				pcsCharacter->m_Mutex.Release();
			}
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_TRANSFORM:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			OnOperationTransform(lCID, lTID);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_RESTORE_TRANSFORM:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			OnOperationRestoreTransform(lCID);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_CANCEL_TRANSFORM:
		{
			OnOperationCancelTransform(lCID);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_EVOLUTION:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			OnOperationEvolution(lCID, lTID);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_RESTORE_EVOLUTION:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			OnOperationRestoreEvolution(lCID, lTID);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_RIDABLE:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_RESTORE_RIDABLE:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_SOCIAL_ANIMATION:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			OnOperationSocialAnimation(lCID, ucSocialAnimationIndex);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_OPTION_UPDATE :
		{
			ApAutoLockCharacter Lock(this, lCID);
			AgpdCharacter	*pcsCharacter		= Lock.GetCharacterLock();

			if (pcsCharacter)
			{
				if (pcsCharacter->m_lOptionFlag != lOptionFlag)
				{
					EnumCallback(AGPMCHAR_CB_ID_UPDATE_OPTION_FLAG, pcsCharacter, &lOptionFlag);
				}
			}
		}
		break;
	
	case AGPMCHAR_PACKET_OPERATION_BLOCK_BY_PENALTY :
		{
			ApAutoLockCharacter Lock(this, lCID);
			AgpdCharacter	*pcsCharacter		= Lock.GetCharacterLock();

			if (pcsCharacter)
			{
				eAgpmCharacterPenalty ePenalty = (eAgpmCharacterPenalty) lOptionFlag;
				EnumCallback(AGPMCHAR_CB_ID_BLOCK_BY_PENALTY, pcsCharacter, &ePenalty);
			}
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_ONLINE_TIME:
		{
			OnOperationOnlineTime(lCID, (UINT32)lRemainedCriminalTime, lRemainedMurdererTime);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_NPROTECT_AUTH:
		{
			OnOperationGameguardAuth( lCID, pggAuthData );
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_UPDATE_SKILLINIT_STRING:
		{
			AgpdCharacter* pcsCharacter = GetCharacter(lCID);
			if(pcsCharacter)
			{
				AuAutoLock csLock(pcsCharacter->m_Mutex);

				strncpy(pcsCharacter->m_szSkillInit, szSkillInit, AGPACHARACTER_MAX_CHARACTER_SKILLINIT);
			}
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_RESURRECTION_BY_OTHER:
		{
			OnOperationResurrectionByOther(lCID, szNickName, lOptionFlag);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_EVENT_EFFECT_ID:
		{
			OnOperationEventEffectID(lCID, lOptionFlag);
		}
		break;

	case AGPMCHAR_PACKET_OPERATUIN_ANSWER_PINCHWANTED_CHARACTER:
		{
			EnumCallback(AGPMCHAR_CB_ID_PINCHWANTED_ANSWER , pvPacket , NULL );
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_REQUEST_PINCHWANTED_CHARACTER:
		{
			OnOperationRequestPinchWantedCharacter(pvPacket);
		}
		break;

	case AGPMCHAR_PACKET_OPERATION_REQUEST_COUPON:
		{
			OnOperationRequestCouponInfo(pvPacket);
		} break;

	case AGPMCHAR_PACKET_OPERATION_COUPON_INFO:
		{
			// Client
			OnOperationRecieveCouponInfo(pvPacket);

		} break;

	case AGPMCHAR_PACKET_OPERATION_REQUEST_COUPON_USE:
		{
			OnOperationRequestCouponUse(pvPacket);
		} break;

	case AGPMCHAR_PACKET_OPERATION_ANSWER_COUPON_USE:
		{
			OnOperationAnswerCouponUse(pvPacket);
		}
		break;
	case AGPMCHAR_PACKET_OPERATION_UPDATE_GAMEPLAY_OPTION: {
		int * options = (int *)szSkillInit;

		if (!options)
			return FALSE;
		EnumCallback(AGPMCHAR_CB_ID_UPDATE_GAMEPLAY_OPTION, NULL, options);
	} break;
	default:
		{
		 	OutputDebugString("AgpmCharacter::OnReceive() Error (7) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmCharacter::OnOperationAdd(DispatchArg *pstCheckArg, INT32 lCID, INT32 lTID, CHAR *szName, INT8 cStatus, INT8 cActionStatus, INT8 cCriminalStatus, PVOID pvMovePacket, PVOID pvActionPacket, PVOID pvFactorPacket, INT64 llMoney, INT64 llBankMoney, INT64 llCash, INT8 cIsNewChar, UINT8 ucRegionIndex, UINT64 ulSpecialStatus, INT8 cIsTransform, CHAR *szSkillInit, INT8 cFaceIndex, INT8 cHairIndex, INT8 cBankSize, UINT16 unEventStatusFlag, INT32 lRemainedCriminalTime, INT32 lRemainedMurdererTime, INT32 lWantedCriminal, CHAR *szNickName ,BOOL bNPCDisplayForMap , BOOL bNPCDisplayForNameBoard, UINT32 ulLastKilledTimeInBattleSquare )
{
	PROFILE("AgpmCharacter::OnOperationAdd");

	// m_csPacketMove
	AuPOS			stSrcPos;
	AuPOS			stDstPos;
	INT32			lMoveTargetID = AP_INVALID_CID;
	INT32			lMoveFollowDistance = 0;
	FLOAT			fDegreeX;
	FLOAT			fDegreeY;
	INT8			cMoveFlag = -1;
	INT8			cMoveDirection = -1;

	// m_csPacketAction
	INT8			cAction = -1;

	AgpdCharacter	*pcsCharacter;

	if (pvMovePacket)
	{
		m_csPacketMove.GetField(FALSE, pvMovePacket, 0, 
								&stSrcPos,
								&stDstPos,
								&lMoveTargetID,
								&lMoveFollowDistance,
								&fDegreeX,
								&fDegreeY,
								&cMoveFlag,
								&cMoveDirection);
	}

	if (!lCID || !lTID)
	{
		OutputDebugString("AgpmCharacter::OnReceive() Error (1) !!!\n");
		return FALSE; 
	}

	if(lTID == 132)
		BOOL bBreakable = TRUE;

	pcsCharacter = AddCharacter(lCID, lTID, szName);
	if (!pcsCharacter)
	{
		PROFILE("AgpmCharacter::OnOperationAdd - 0");

		pcsCharacter = GetCharacterLock(lCID);
		if (!pcsCharacter)
		{
			return FALSE;
		}

		BOOL	bIsNeedRemove	= FALSE;
		EnumCallback(AGPMCHAR_CB_ID_ALREADY_EXIST_CHAR, pcsCharacter, &bIsNeedRemove);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "CharacterID = %s", pcsCharacter->m_szID);
		AuLogFile_s("LOG\\AddFailButBeCharacter.txt", strCharBuff);

		if (bIsNeedRemove)
		{
			//if (IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
			//	AuLogFile("RemoveNPC.log", "Removed by OnOperationAdd()\n");

			// ����� �ٽ� �߰��ؾ� �ϴ� ���̴�.
			pcsCharacter->m_Mutex.Release();

			RemoveCharacter(lCID);

			pcsCharacter	= AddCharacter(lCID, lTID, szName);
			if (!pcsCharacter)
				return FALSE;
		}
		else
		{
			if (pcsCharacter && pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD && !pcsCharacter->m_bIsAddMap)
			{
				AddCharacterToMap(pcsCharacter);
			}

			pcsCharacter->m_Mutex.Release();

			if (!OnOperationUpdate(pstCheckArg, lCID, lTID, szName, cStatus, cActionStatus, cCriminalStatus, pvMovePacket, pvActionPacket, pvFactorPacket, llMoney, llBankMoney, llCash, cIsNewChar, ucRegionIndex, ulSpecialStatus, cIsTransform, cFaceIndex, cHairIndex, cBankSize, unEventStatusFlag, lRemainedCriminalTime, lRemainedMurdererTime, lWantedCriminal, szNickName))
			{
				OutputDebugString("AgpmCharacter::OnReceive() Error (2) !!!\n");
				return FALSE;
			}
			else
			{
				// �����ϴ� ĳ���Ͷ� ������Ʈ�� �ߴ�
				return TRUE;
			}
		}
	}

	{
		PROFILE("AgpmCharacter::OnOperationAdd - 1");

		if (!pcsCharacter->m_Mutex.WLock())
		{
			return FALSE;
		}

		if (szSkillInit && szSkillInit[0])
			strncpy(pcsCharacter->m_szSkillInit, szSkillInit, AGPACHARACTER_MAX_CHARACTER_SKILLINIT);

		if (szNickName && szNickName[0])
			strncpy(pcsCharacter->m_szNickName, szNickName, AGPACHARACTER_MAX_CHARACTER_NICKNAME);

		// ����ٰ� ���� Data�� Setting �ϴ� �Լ��� �ʿ��ϴ�. (���߿� UpdateInit() �Լ��� �� �ִ´�.)
		pcsCharacter->m_stPos = stSrcPos;

		// setting status
		pcsCharacter->m_unCurrentStatus		= cStatus;
		pcsCharacter->m_unActionStatus		= cActionStatus;
		pcsCharacter->m_unCriminalStatus	= cCriminalStatus;
		pcsCharacter->m_ulSpecialStatus		= ulSpecialStatus;

		pcsCharacter->m_unEventStatusFlag	= unEventStatusFlag;

		if (pvFactorPacket && m_pcsAgpmFactors)
		{
			ReflectFactorPacket(pcsCharacter, pvFactorPacket);
		}

		if (ucRegionIndex != 0xff)
		{
			pcsCharacter->m_nBindingRegionIndex		= (INT16) ucRegionIndex;
			pcsCharacter->m_nLastExistBindingIndex	= (INT16) ucRegionIndex;
		}

		if (cIsTransform & AGPMCHAR_FLAG_TRANSFORM)
			pcsCharacter->m_bIsTrasform	= TRUE;
		else
			pcsCharacter->m_bIsTrasform	= FALSE;

		if (cIsTransform & AGPMCHAR_FLAG_RIDABLE)
			pcsCharacter->m_bRidable	= TRUE;
		else
			pcsCharacter->m_bRidable	= FALSE;

		pcsCharacter->m_bIsEvolution = (cIsTransform & AGPMCHAR_FLAG_EVOLUTION) ? TRUE : FALSE;

		if (cFaceIndex >= 0)
			pcsCharacter->m_lFaceIndex	= (INT32) cFaceIndex;
		if (cHairIndex >= 0)
			pcsCharacter->m_lHairIndex	= (INT32) cHairIndex;

		if (cIsNewChar == (INT8) TRUE)
			EnumCallback(AGPMCHAR_CB_ID_NEW_CREATED_CHAR, pcsCharacter, NULL);

		pcsCharacter->m_cBankSize	= cBankSize;

		pcsCharacter->m_fTurnX	= fDegreeX;
		pcsCharacter->m_fTurnY	= fDegreeY;
		pcsCharacter->m_bNPCDisplayForMap	=	bNPCDisplayForMap;
		pcsCharacter->m_bNPCDisplayForNameBoard	=	bNPCDisplayForNameBoard;
		
		
		SetLastCriminalTime(pcsCharacter, lRemainedCriminalTime);
		SetLastMurdererTime(pcsCharacter, lRemainedMurdererTime);
		SetLastKilledTimeInBattleSquare(pcsCharacter, ulLastKilledTimeInBattleSquare);

		if (lWantedCriminal > 0)
			pcsCharacter->m_bIsWantedCriminal = (BOOL) lWantedCriminal;

		UpdateInit(pcsCharacter);

	}

	{
		PROFILE("AgpmCharacter::OnOperationAdd - 2");

		//AgpdCharacter	*pcsFollowTarget	= GetCharacterLock(lMoveTargetID);
		AgpdCharacter	*pcsFollowTarget	= GetCharacter(lMoveTargetID);

		if (cMoveFlag >= 0)
		{
			if (!(cMoveFlag & AGPMCHARACTER_MOVE_FLAG_HORIZONTAL))
				TurnCharacter(pcsCharacter, fDegreeX, fDegreeY);

			if (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_STOP ||
				cMoveFlag & AGPMCHARACTER_MOVE_FLAG_SYNC)
				StopCharacter(pcsCharacter, &stDstPos);
			else if (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FOLLOW && pcsFollowTarget)
				MoveCharacterFollow(pcsCharacter, pcsFollowTarget, lMoveFollowDistance);
			else
				MoveCharacter(pcsCharacter, &stDstPos, 
								(cMoveDirection != -1) ? (MOVE_DIRECTION)cMoveDirection : MD_NODIRECTION, 
								cMoveFlag & AGPMCHARACTER_MOVE_FLAG_PATHFINDING, 
								cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FAST,
								cMoveFlag & AGPMCHARACTER_MOVE_FLAG_HORIZONTAL);
		}

	}

	{
		PROFILE("AgpmCharacter::OnOperationAdd - 3");

		// �������...

		//if (pcsFollowTarget)
		//	pcsFollowTarget->m_Mutex.Release();

		//���� ���õǾ �Ѿ������ ���� �ٲ��ش�.
		if( (0 <= llMoney ) && ( llMoney <= AGPDCHARACTER_MAX_INVEN_MONEY ) )
		{
			SetMoney( pcsCharacter, llMoney );
		}

		if (0 <= llBankMoney && llBankMoney <= AGPDCHARACTER_MAX_BANK_MONEY)
			SetBankMoney(pcsCharacter, llBankMoney);

		if (0 <= llCash && llCash <= AGPDCHARACTER_MAX_CASH)
			SetCash(pcsCharacter, llCash);

		EnumCallback( AGPMCHAR_CB_ID_POST_ADD, pcsCharacter, NULL);

	}

	{
		PROFILE("AgpmCharacter::OnOperationAdd - 4");

		pcsCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgpmCharacter::OnOperationUpdate(DispatchArg *pstCheckArg, INT32 lCID, INT32 lTID, CHAR *szName, INT8 cStatus, INT8 cActionStatus, INT8 cCriminalStatus, PVOID pvMovePacket, PVOID pvActionPacket, PVOID pvFactorPacket, INT64 llMoney, INT64 llBankMoney, INT64 llCash, INT8 cIsNewChar, UINT8 ucRegionIndex, UINT64 ulSpecialStatus, INT8 cIsTransform, INT8 cFaceIndex, INT8 cHairIndex, INT8 cBankSize, UINT16 unEventStatusFlag, INT32 lRemainedCriminalTime, INT32 lRemainedMurdererTime, INT32 lWantedCriminal, CHAR *szNickName , INT32 lSpecialPart )
{
	PROFILE("AgpmCharacter::OnOperationUpdate");

	// m_csPacketMove
	AuPOS			stSrcPos = {0,0,0};
	AuPOS			stDstPos = {0,0,0};
	INT32			lMoveTargetID	= AP_INVALID_CID;
	INT32			lMoveFollowDistance = 0;
	FLOAT			fDegreeX = -123;
	FLOAT			fDegreeY;
	INT8			cMoveFlag = -1;
	INT8			cMoveDirection = -1;

	// m_csPacketAction
	INT8			cAction		= -1;
	INT32			lTargetCID	= AP_INVALID_CID;
	INT32			lSkillTID	= AP_INVALID_CID;
	INT8			cActionResultType	= -1;
	PVOID			pvPacketTargetDamageFactor	= NULL;
	AuPOS			stAttackPos = {0,0,0};
	UINT8			cComboInfo	= 0;
	INT8			cForceAttack	= (INT8) FALSE;
	UINT32			ulAdditionalEffect = 0;
	UINT8			cHitIndex = 0;
	
	BOOL			bRet;

	//TRACE("UpdatePacket (%d), ", lCID);
	if (pvMovePacket)
	{
		//TRACE("< Move >");
		m_csPacketMove.GetField(FALSE, pvMovePacket, 0, 
								&stSrcPos,
								&stDstPos,
								&lMoveTargetID,
								&lMoveFollowDistance,
								&fDegreeX,
								&fDegreeY,
								&cMoveFlag,
								&cMoveDirection);
	}
	
	if (pvActionPacket)
	{
		//TRACE("< Attack >");
		m_csPacketAction.GetField(FALSE, pvActionPacket, 0, 
								  &cAction,
								  &lTargetCID,
								  &lSkillTID,
								  &cActionResultType,
								  &pvPacketTargetDamageFactor,
								  &stAttackPos,
								  &cComboInfo,
								  &cForceAttack,
								  &ulAdditionalEffect,
								  &cHitIndex );
	}
	//TRACE(" End\n");

	if (lCID == AP_INVALID_CID ||
		(!pstCheckArg->bReceivedFromServer && 
		 pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		 lCID != pstCheckArg->lSocketOwnerID))
	{
		OutputDebugString("AgpmCharacter::OnReceive() Error (3) !!!\n");
		return FALSE;
	}

	INT32	alCID[4];
	ZeroMemory(alCID, sizeof(INT32) * 4);

	alCID[0]	= lCID;

	AgpdCharacter	*pacsCharacter[4];
	ZeroMemory(pacsCharacter, sizeof(AgpdCharacter *) * 4);

	INT32	lNumCharacter	= 1;
	if (lTargetCID != AP_INVALID_CID)
	{
		alCID[lNumCharacter] = lTargetCID;
		++lNumCharacter;
	}
	if (lMoveTargetID != AP_INVALID_CID)
	{
		alCID[lNumCharacter] = lMoveTargetID;
		++lNumCharacter;
	}

	BOOL	bLockResult = GetCharacterLock(alCID, lNumCharacter, pacsCharacter);

	AgpdCharacter	*pcsCharacter		= NULL;
	AgpdCharacter	*pcsTargetChar		= NULL;
	AgpdCharacter	*pcsMoveTargetChar	= NULL;
	
	if (lNumCharacter == 1 && !bLockResult)
	{
		if (pstCheckArg ->bReceivedFromServer)
			return OnOperationAdd(pstCheckArg, lCID, lTID, szName, cStatus, cActionStatus, cCriminalStatus, pvMovePacket, pvActionPacket, pvFactorPacket, llMoney, llBankMoney, llCash, cIsNewChar, ucRegionIndex, ulSpecialStatus, cIsTransform, NULL, cFaceIndex, cHairIndex, cBankSize, unEventStatusFlag, lRemainedCriminalTime, lRemainedMurdererTime, lWantedCriminal, NULL , 1 , 1, 0);
		else
			return FALSE;
	}
	else if (!bLockResult)
		return FALSE;

	for (int i = 0; i < lNumCharacter; ++i)
	{
		if (pacsCharacter[i]->m_lID == lCID)
			pcsCharacter		= pacsCharacter[i];
		else if (pacsCharacter[i]->m_lID == lTargetCID)
			pcsTargetChar		= pacsCharacter[i];
		else if (pacsCharacter[i]->m_lID == lMoveTargetID)
			pcsMoveTargetChar	= pacsCharacter[i];
	}

	//if (pcsCharacter->m_bIsProtectedNPC)
	//{
	//	AuLogFile("RemoveNPC.log", "ProtectedNPC cannot data update (received from server : %d)", pstCheckArg->bReceivedFromServer);
	//}

	if (!pstCheckArg->bReceivedFromServer && IsAllBlockStatus(pcsCharacter) ||
		!pstCheckArg->bReceivedFromServer && IsAllBlockStatus(pcsTargetChar) ||
		!pstCheckArg->bReceivedFromServer && IsAllBlockStatus(pcsMoveTargetChar))
	{
		if (pcsTargetChar)
			pcsTargetChar->m_Mutex.Release();

		if (pcsMoveTargetChar)
			pcsMoveTargetChar->m_Mutex.Release();

		if (pcsCharacter)
			pcsCharacter->m_Mutex.Release();

		return TRUE;
	}

	if (pstCheckArg->bReceivedFromServer && pvFactorPacket && m_pcsAgpmFactors)
	{
		ReflectFactorPacket(pcsCharacter, pvFactorPacket);

		EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pcsCharacter, NULL);
		//ReCalcCharacterFactors(pcsCharacter);
	}

	if (pstCheckArg->bReceivedFromServer && cStatus >= 0)
	{
		UpdateStatus(pcsCharacter, cStatus);

		//fprintf(stdout, "Update Status !!!\n");
		//fflush(stdout);
	}

	if (pstCheckArg->bReceivedFromServer && cActionStatus >= 0)
		UpdateActionStatus(pcsCharacter, cActionStatus);

	if (pstCheckArg->bReceivedFromServer && cCriminalStatus >= 0)
		UpdateCriminalStatus(pcsCharacter, (AgpdCharacterCriminalStatus) cCriminalStatus);

	if (pstCheckArg->bReceivedFromServer && ulSpecialStatus != AGPDCHAR_SPECIAL_STATUS_NONE)
		UpdateSpecialStatus(pcsCharacter, ulSpecialStatus, lSpecialPart);

	if (pstCheckArg->bReceivedFromServer && cBankSize != (-1) && cBankSize != pcsCharacter->m_cBankSize)
		UpdateBankSize(pcsCharacter, cBankSize);

	if (pvMovePacket)
	{
	
		bRet	= ProcessMovePacket(pcsCharacter, pcsMoveTargetChar, lMoveFollowDistance,
									&stSrcPos, &stDstPos, fDegreeX, fDegreeY, cMoveFlag, cMoveDirection, pstCheckArg->bReceivedFromServer);

		/*
		if (bReceivedFromServer || !IsActionBlockCondition(pcsCharacter))
		{
			PROFILE("AgpmCharacter::OnOperationUpdate MovePacket");

			if (bReceivedFromServer && !pcsCharacter->m_bIsAddMap)
			{
				UpdatePosition(pcsCharacter, &stSrcPos, FALSE, TRUE);
			}
			else if (bReceivedFromServer && (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_SYNC))
			{
				StopCharacter(pcsCharacter, NULL);

				if (fDegreeX != -123)
					TurnCharacter(pcsCharacter, fDegreeX, fDegreeY);

				UpdatePosition(pcsCharacter, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_STOP ? &stSrcPos : &stDstPos, FALSE, TRUE);
			}
			else if (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_STOP)
			{
				//fprintf(stdout, "Stop Packet (%3.3f,%3.3f,%3.3f) !!!\n", stSrcPos.x, stSrcPos.y, stSrcPos.z);
				//fflush(stdout);

				//bRet = MoveTurnCharacter(pcsCharacter, &stSrcPos, fDegreeX, fDegreeY, FALSE, FALSE);
				bRet = MoveCharacter(pcsCharacter, &stSrcPos, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE);

				//fprintf(stdout, "Stop Packet Processed %d !!!\n", bRet);
				//fflush(stdout);
			}
			else if (bReceivedFromServer && cMoveFlag & AGPMCHARACTER_MOVE_FLAG_HORIZONTAL)
			{
				bRet = MoveCharacter(pcsCharacter, &stDstPos, FALSE, FALSE, FALSE, TRUE);
			}
			else if (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FOLLOW)
			{
				BOOL	bSyncPosition	= TRUE;
				if (stSrcPos.x != 0 &&
					stSrcPos.y != 0 &&
					stSrcPos.z != 0)
					bSyncPosition = SyncMoveStartPosition(pcsCharacter, &stSrcPos);

				bRet = MoveCharacterFollow(pcsCharacter, pcsMoveTargetChar, lMoveFollowDistance, TRUE);

				if (!bSyncPosition)
				{
					// ���� ĳ���� ��ġ�� stSrcPos�� ��ġ�� ���ؼ� m_lAdjustMoveSpeed�� �����Ѵ�.
					AdjustMoveSpeed(pcsCharacter, &stSrcPos);
				}
			}
			else
			{
				//fprintf(stdout, "Move Packet (%3.3f,%3.3f,%3.3f) !!!\n", stDstPos.x, stDstPos.y, stDstPos.z);
				//fflush(stdout);

				// �����̴� ���� ��ġ�� ���ؼ� ���̰� ���� ���� ��� ó���Ѵ�.
				//
				//		�ϴ� �׽�Ʈ�� �����غ���.
				//

				BOOL	bSyncPosition	= TRUE;
				if (stSrcPos.x != 0 &&
					stSrcPos.y != 0 &&
					stSrcPos.z != 0)
					bSyncPosition = SyncMoveStartPosition(pcsCharacter, &stSrcPos);

				//bRet = MoveTurnCharacter(pcsCharacter, &stDstPos, fDegreeX, fDegreeY, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_DIRECTION, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_PATHFINDING, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FAST);
				bRet = MoveCharacter(pcsCharacter, &stDstPos, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_DIRECTION, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_PATHFINDING, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FAST);

				if (!bSyncPosition)
				{
					// ���� ĳ���� ��ġ�� stSrcPos�� ��ġ�� ���ؼ� m_lAdjustMoveSpeed�� �����Ѵ�.
					AdjustMoveSpeed(pcsCharacter, &stSrcPos);
				}
			}
		}
		*/
	}

	if (pcsMoveTargetChar)
		pcsMoveTargetChar->m_Mutex.Release();

	if (pvActionPacket)					// action packet ó��
	{
		PROFILE("AgpmCharacter::OnOperationUpdate ActionPacket");

		ProcessActionPacket(pcsCharacter, pcsTargetChar, pvPacketTargetDamageFactor,
							cActionResultType, cForceAttack, cComboInfo, lSkillTID,
							ulAdditionalEffect, cHitIndex, pstCheckArg->bReceivedFromServer, cAction);

		/*
		AgpdFactor	*pcsFactorDamage	= NULL;

		BOOL		bIsNowUpdate		= TRUE;
		EnumCallback(AGPMCHAR_CB_ID_CHECK_NOW_UPDATE_ACTION_FACTOR, pcsTargetChar, &bIsNowUpdate);

		if (bIsNowUpdate && bReceivedFromServer && pvPacketTargetDamageFactor && m_pcsAgpmFactors)
		{
			pcsFactorDamage	= new (AgpdFactor);

			m_pcsAgpmFactors->InitFactor(pcsFactorDamage);

			ReflectFactorPacket(pcsTargetChar, pvPacketTargetDamageFactor, pcsFactorDamage);

			EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pcsTargetChar, NULL);
		}

		PVOID	pvBuffer[5];
		pvBuffer[0]	= pcsTargetChar;
		pvBuffer[1] = (PVOID) cActionResultType;
		pvBuffer[2] = (PVOID) pcsFactorDamage;
		pvBuffer[3] = (PVOID) cAction;
		pvBuffer[4]	= (PVOID) pvPacketTargetDamageFactor;

		EnumCallback(AGPMCHAR_CB_ID_RECEIVE_ACTION, pcsCharacter, pvBuffer);

		ActionCharacter((AgpdCharacterActionType) cAction, pcsCharacter, pcsTargetChar, NULL, (BOOL) cForceAttack, cActionResultType, pcsFactorDamage, cComboInfo);
		*/
	}

	if (pcsTargetChar)
		pcsTargetChar->m_Mutex.Release();

	//���� ���õǾ �Ѿ������ ���� �ٲ��ش�.
	if( pstCheckArg->bReceivedFromServer && (0 <= llMoney ) && ( llMoney <= AGPDCHARACTER_MAX_INVEN_MONEY ) )
	{
		SetMoney( pcsCharacter, llMoney );
	}

	if (pstCheckArg->bReceivedFromServer && 0 <= llBankMoney && llBankMoney <= AGPDCHARACTER_MAX_BANK_MONEY)
		SetBankMoney(pcsCharacter, llBankMoney);

	if (pstCheckArg->bReceivedFromServer && 0 <= llCash && llCash <= AGPDCHARACTER_MAX_CASH)
		SetCash(pcsCharacter, llCash);

	/*
	if (ucRegionIndex != 0xff &&
		pcsCharacter->m_nBindingRegionIndex != (INT16) ucRegionIndex)
	{
		INT16 nPrevRegion = pcsCharacter->m_nBindingRegionIndex;
		pcsCharacter->m_nBindingRegionIndex		= (INT16) ucRegionIndex;
		pcsCharacter->m_nLastExistBindingIndex	= (INT16) ucRegionIndex;
		EnumCallback(AGPMCHAR_CB_ID_BINDING_REGION_CHANGE, pcsCharacter, &nPrevRegion);
	}
	*/

	if (pstCheckArg->bReceivedFromServer &&
		(cFaceIndex >= 0 || cHairIndex >= 0))
		UpdateCustomizeIndex(pcsCharacter, (INT32) cFaceIndex, (INT32) cHairIndex);

	if (pstCheckArg->bReceivedFromServer && unEventStatusFlag > 0)
		UpdateEventStatusFlag(pcsCharacter, unEventStatusFlag);

	if (pstCheckArg->bReceivedFromServer)
	{
		SetLastCriminalTime(pcsCharacter, lRemainedCriminalTime);
		SetLastMurdererTime(pcsCharacter, lRemainedMurdererTime);
		
		if (lWantedCriminal > 0)
			pcsCharacter->m_bIsWantedCriminal = (BOOL) lWantedCriminal;
	}

	if (pstCheckArg->bReceivedFromServer && szNickName && szNickName[0]
		 && (0 != strcmp(szNickName, pcsCharacter->m_szNickName)))
	{
		ZeroMemory(pcsCharacter->m_szNickName, sizeof(pcsCharacter->m_szNickName));
		strncpy(pcsCharacter->m_szNickName, szNickName, AGPACHARACTER_MAX_CHARACTER_NICKNAME);
		EnumCallback(AGPMCHAR_CB_ID_UPDATE_NICKNAME, pcsCharacter, NULL);
	}
	
	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRemove(INT32 lCID, INT32 lTID, CHAR *szName, INT8 cStatus, PVOID pvMovePacket, PVOID pvActionPacket, PVOID pvFactorPacket, UINT32 ulNID)
{
	PROFILE("AgpmCharacter::OnOperationRemove");

	if (!lCID)
	{
		OutputDebugString("AgpmCharacter::OnReceive() Error (5) !!!\n");
		return FALSE;
	}

	/*
	BOOL	bRemove = TRUE;
	EnumCallback(AGPMCHAR_CB_ID_CHECK_REMOVE_CHAR, &lCID, &bRemove);

	if (!bRemove)
	{
		AgpdCharacter	*pcsCharacter = GetCharacterLock(lCID);

		if (!pcsCharacter)
		{
			return FALSE;
		}

		BOOL bRetval = RemoveCharacterFromMap(pcsCharacter);

		pcsCharacter->m_Mutex.Release();

		return bRetval;
	}
	*/

	AgpdCharacter	*pcsCharacter	= GetCharacterLock(lCID);

	if (pcsCharacter && (IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC))
	{
		//AuLogFile("RemoveNPC.log", "Removed by OnOperationRemove()\n");
		EnumCallback(AGPMCHAR_CB_ID_REMOVE_PROTECTED_NPC, pcsCharacter, &ulNID);
	}

	if (pcsCharacter)
		pcsCharacter->m_Mutex.Release();

	if (!RemoveCharacter(lCID))
	{
		OutputDebugString("AgpmCharacter::OnReceive() Error (6) !!!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRemoveForView(INT32 lCID)
{
	PROFILE("AgpmCharacter::OnOperationRemoveForView");

	if (lCID ==	AP_INVALID_CID)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= GetCharacter(lCID);
	BOOL	bRemove = TRUE;
	EnumCallback(AGPMCHAR_CB_ID_CHECK_REMOVE_CHAR,	&lCID, &bRemove);

	BOOL bResult = TRUE;
	if (bRemove)
	{
		//@{ Jaewon 20050902
		// For fade-in/out
		BOOL bRemove2 = TRUE;
		EnumCallback(AGPMCHAR_CB_ID_CHECK_REMOVE_CHAR2,	&lCID, &bRemove2);
		if(bRemove2)
		{
			bResult = RemoveCharacter(lCID);
		}
		//@} Jaewon
	}
	return bResult;
}

BOOL AgpmCharacter::OnOperationLevelUp(INT32 lCID, PVOID pvFactorPacket)
{
	PROFILE("AgpmCharacter::OnOperationLevelUp");

	if (!lCID/* || !pvFactorPacket*/)
	{
		OutputDebugString("AgpmCharacter::OnReceive() Error (33) !!!\n");
		return FALSE;
	}

	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
	{
		return FALSE;
	}

	if (m_pcsAgpmFactors)
	{
		ReflectFactorPacket(pcsCharacter, pvFactorPacket);

		EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pcsCharacter, NULL);
	}

	ChangedLevel(pcsCharacter);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmCharacter::OnOperationAddAttacker(INT32 lCID, INT32 lAttackerID)
{
	PROFILE("AgpmCharacter::OnOperationLevelUp");

	if (lCID == AP_INVALID_CID || lAttackerID == AP_INVALID_CID)
		return FALSE;

	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
	{
		return FALSE;
	}

	// attacker�� list�� �߰��Ѵ�.
	AddAttackerToList(pcsCharacter, lAttackerID);

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}
BOOL AgpmCharacter::OnOperationTransform(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter)
		return FALSE;
	
	if (!pcsCharacter->m_Mutex.WLock())
	{
		return NULL;
	}

	if (lTID != AP_INVALID_CID)
		TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY, GetCharacterTemplate(lTID));

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}

BOOL AgpmCharacter::OnOperationTransform(INT32 lCID, INT32 lTID)
{
	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (lTID != AP_INVALID_CID)
		TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY, GetCharacterTemplate(lTID));

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRestoreTransform(INT32 lCID)
{
	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	RestoreTransformCharacter(pcsCharacter);

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}

BOOL AgpmCharacter::OnOperationCancelTransform(INT32 lCID)
{
	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;
	
	if (IsAllBlockStatus(pcsCharacter))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	// ���� ����Ѵܴ�,,, ��ҽ����ش�.
	BOOL	bResult	= EnumCallback(AGPMCHAR_CB_ID_CANCEL_TRANSFORM, pcsCharacter, NULL);

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return bResult;
}

BOOL AgpmCharacter::OnOperationEvolution(INT32 lCID, INT32 lTID)
{
	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (lTID != AP_INVALID_CID)
		EvolutionCharacter(pcsCharacter, GetCharacterTemplate(lTID));

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRestoreEvolution(INT32 lCID, INT32 lTID)
{
	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	if (lTID != AP_INVALID_CID)
		RestoreEvolutionCharacter(pcsCharacter, GetCharacterTemplate(lTID));

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}

BOOL AgpmCharacter::OnOperationSocialAnimation(INT32 lCID, UINT8 ucSocialActionIndex)
{
	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	EnumCallback(AGPMCHAR_CB_ID_SOCIAL_ANIMATION, pcsCharacter, &ucSocialActionIndex);

	if (!pcsCharacter->m_Mutex.Release())
		return FALSE;

	return TRUE;
}

// 2007.07.06. steeple
BOOL AgpmCharacter::OnOperationOnlineTime(INT32 lCID, UINT32 ulOnlineTime, INT32 lAddictStatus)
{
	AgpdCharacter* pcsCharacter = GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	// �׳� �����Ѵ�.
	pcsCharacter->m_ulOnlineTime = ulOnlineTime;
	pcsCharacter->m_lAddictStatus = lAddictStatus;

	EnumCallback(AGPMCHAR_CB_ID_ONLINE_TIME, pcsCharacter, NULL);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgpmCharacter::OnOperationGameguardAuth( INT32 lCID, PVOID pggAuth )
{
	EnumCallback(AGPMCHAR_CB_ID_GAMEGUARD_AUTH, &lCID, pggAuth);

	return TRUE;
}

// 2007.08.24. steeple
BOOL AgpmCharacter::OnOperationResurrectionByOther(INT32 lCID, CHAR* szName, INT32 lFlag)
{
	if(!lCID || !szName)
		return FALSE;

	AgpdCharacter* pcsCharacter = GetCharacterLock(lCID);
	if(!pcsCharacter)
		return FALSE;

	PVOID pvBuffer[2];
	pvBuffer[0] = (PVOID)szName;
	pvBuffer[1] = (PVOID)&lFlag;

	EnumCallback(AGPMCHAR_CB_ID_RESURRECTION_BY_OTHER, pcsCharacter, pvBuffer);

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

// 2007.09.17. steeple
BOOL AgpmCharacter::OnOperationEventEffectID(INT32 lCID, INT32 lEventEffectID)
{
	if(!lEventEffectID)
		return FALSE;

	// ĳ���� ������ ���� �ʿ� ����.


	SetEventEffectID(lEventEffectID);
	
	EnumCallback(AGPMCHAR_CB_ID_EVENT_EFFECT_ID, NULL, NULL);

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRequestPinchWantedCharacter(PVOID pvPacket)
{
	if(!pvPacket)
		return FALSE;

	EnumCallback(AGPMCHAR_CB_ID_PINCHWANTED_REQUEST, pvPacket, NULL);

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRequestCouponInfo(PVOID pvPacket)
{
	if(!pvPacket)
		return FALSE;

	EnumCallback(AGPMCHAR_CB_ID_REQUEST_COUPON, pvPacket, NULL);

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRequestCouponUse(PVOID pvPacket)
{
	if(NULL == pvPacket)
		return FALSE;

	EnumCallback(AGPMCHAR_CB_ID_REQUEST_COUPON_USE, pvPacket, NULL);

	return TRUE;
}

BOOL AgpmCharacter::OnOperationAnswerCouponUse(PVOID pvPacket)
{
	if( !pvPacket )
		return FALSE;

	EnumCallback( AGPMCHAR_CB_ID_RECIEVE_COUPON_USE, pvPacket, NULL );

	return TRUE;
}

BOOL AgpmCharacter::OnOperationRecieveCouponInfo(PVOID pvPacket)
{
	if(!pvPacket)
		return FALSE;

	// Callback ȣ��
	EnumCallback(AGPMCHAR_CB_ID_RECIEVE_COUPON_INFO, pvPacket, NULL);

	return TRUE;
}

BOOL
AgpmCharacter::ProcessMovePacket(
	AgpdCharacter *	pcsCharacter, 
	AgpdCharacter *	pcsMoveTargetChar, 
	INT32			lMoveFollowDistance,
	AuPOS *			pstSrcPos, 
	AuPOS *			pstDstPos, 
	FLOAT			fDegreeX, 
	FLOAT			fDegreeY,
	INT8			cMoveFlag, 
	INT8			cMoveDirection, 
	BOOL			bReceivedFromServer)
{
	if (!pcsCharacter || !pstSrcPos || !pstDstPos)
		return FALSE;

	PROFILE("AgpmCharacter::ProcessMovePacket");

	BOOL	bRet	= TRUE;

	if (bReceivedFromServer || !IsActionBlockCondition(pcsCharacter))
	{
		PROFILE("AgpmCharacter::OnOperationUpdate MovePacket");

		if (bReceivedFromServer && !pcsCharacter->m_bIsAddMap)
		{
			UpdatePosition(pcsCharacter, pstSrcPos, FALSE, TRUE);
		}
		else if (bReceivedFromServer && (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_SYNC))
		{
			StopCharacter(pcsCharacter, NULL);

			if (fDegreeX != -123)
				TurnCharacter(pcsCharacter, fDegreeX, fDegreeY);

			UpdatePosition(pcsCharacter, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_STOP ? pstSrcPos : pstDstPos, FALSE, TRUE);
		}
		else if (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_STOP)
		{
			//fprintf(stdout, "Stop Packet (%3.3f,%3.3f,%3.3f) !!!\n", stSrcPos.x, stSrcPos.y, stSrcPos.z);
			//fflush(stdout);

			//bRet = MoveTurnCharacter(pcsCharacter, &stSrcPos, fDegreeX, fDegreeY, FALSE, FALSE);

			//OutputDebugString("Receive Stop Packet \n");

			bRet = MoveCharacter(pcsCharacter, pstSrcPos, MD_NODIRECTION, FALSE, FALSE, FALSE, FALSE, TRUE);

			//fprintf(stdout, "Stop Packet Processed %d !!!\n", bRet);
			//fflush(stdout);
		}
		else if (bReceivedFromServer && cMoveFlag & AGPMCHARACTER_MOVE_FLAG_HORIZONTAL)
		{
			bRet = MoveCharacter(pcsCharacter, pstDstPos, MD_NODIRECTION, FALSE, FALSE, TRUE);
		}
		else if (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FOLLOW)
		{
			BOOL	bSyncPosition	= TRUE;
			if (pstSrcPos->x != 0 &&
				pstSrcPos->y != 0 &&
				pstSrcPos->z != 0 &&
				bReceivedFromServer)
				bSyncPosition = SyncMoveStartPosition(pcsCharacter, pstSrcPos);

			pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

			bRet = MoveCharacterFollow(pcsCharacter, pcsMoveTargetChar, lMoveFollowDistance, TRUE);

			if (!bSyncPosition)
			{
				// ���� ĳ���� ��ġ�� stSrcPos�� ��ġ�� ���ؼ� m_lAdjustMoveSpeed�� �����Ѵ�.
				AdjustMoveSpeed(pcsCharacter, pstSrcPos);
			}
		}
		else
		{
			//fprintf(stdout, "Move Packet (%3.3f,%3.3f,%3.3f) !!!\n", stDstPos.x, stDstPos.y, stDstPos.z);
			//fflush(stdout);

			// �����̴� ���� ��ġ�� ���ؼ� ���̰� ���� ���� ��� ó���Ѵ�.
			//
			//		�ϴ� �׽�Ʈ�� �����غ���.
			//

			// 2007.01.17. steeple
			// Direction ����.
			//if (cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FAST) {
			//	BOOL bLocked = FALSE;
			//	PVOID pData[] = { pcsCharacter, &bLocked };

			//	EnumCallback(AGPMCHAR_CB_ID_CHECK_MOVEMENT_LOCK, pData, NULL);
			//	if (bLocked)
			//		return TRUE;
			//}
			if(cMoveDirection != -1)
				pcsCharacter->m_eMoveDirection = (MOVE_DIRECTION)cMoveDirection;
			else
				pcsCharacter->m_eMoveDirection = MD_NODIRECTION;

			BOOL	bSyncPosition	= TRUE;
			if (pstSrcPos->x != 0 &&
				pstSrcPos->y != 0 &&
				pstSrcPos->z != 0 &&
				bReceivedFromServer)
				bSyncPosition = SyncMoveStartPosition(pcsCharacter, pstSrcPos);

			//bRet = MoveTurnCharacter(pcsCharacter, &stDstPos, fDegreeX, fDegreeY, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_DIRECTION, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_PATHFINDING, cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FAST);
			bRet = MoveCharacter(pcsCharacter,
								pstDstPos,
								pcsCharacter->m_eMoveDirection,
								cMoveFlag & AGPMCHARACTER_MOVE_FLAG_PATHFINDING,
								cMoveFlag & AGPMCHARACTER_MOVE_FLAG_FAST);
			// ��κ� cMoveFlag & AGPMCHARACTER_MOVE_FLAG_DIRECTION -> �̰� Direction���� �޾ƿ;���.
			// �׷��� ���� MOVE_DIRECTION ������ ĳ�����ؼ� ������ (�ȱ׷��� �����ε� ������ ..)

			if (!bSyncPosition)
			{
				// ���� ĳ���� ��ġ�� stSrcPos�� ��ġ�� ���ؼ� m_lAdjustMoveSpeed�� �����Ѵ�.
				AdjustMoveSpeed(pcsCharacter, pstSrcPos);
			}
		}
	}

	return TRUE;
}

BOOL	AgpmCharacter::ProcessActionPacket(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar, PVOID pvPacketTargetDamageFactor,
										   INT8 cActionResultType, INT8 cForceAttack, INT8 cComboInfo, INT32 lSkillTID,
										   UINT32 ulAdditionalEffect, UINT8 cHitIndex, BOOL bReceivedFromServer, INT8 cAction )
{
	if (!pcsCharacter)
		return FALSE;

	PROFILE("AgpmCharacter::ProcessActionPacket");

	AgpdFactor	*pcsFactorDamage	= NULL;

	BOOL		bIsNowUpdate		= TRUE;
	PVOID		pvBuffer[9];

	pvBuffer[0]		=	pcsTargetChar;
	EnumCallback(AGPMCHAR_CB_ID_CHECK_NOW_UPDATE_ACTION_FACTOR, pvBuffer, &bIsNowUpdate);

	if (bIsNowUpdate && bReceivedFromServer && pvPacketTargetDamageFactor && m_pcsAgpmFactors)
	{
		pcsFactorDamage	= new (AgpdFactor);

		m_pcsAgpmFactors->InitFactor(pcsFactorDamage);

		ReflectFactorPacket(pcsTargetChar, pvPacketTargetDamageFactor, pcsFactorDamage);
		EnumCallback(AGPMCHAR_CB_ID_UPDATE_FACTOR, pcsTargetChar, NULL);
		
	}

	pvBuffer[0]	= pcsTargetChar;
	pvBuffer[1] = (PVOID) cActionResultType;
	pvBuffer[2] = (PVOID) pcsFactorDamage;
	pvBuffer[3] = (PVOID) cAction;
	pvBuffer[4]	= (PVOID) pvPacketTargetDamageFactor;
	pvBuffer[5]	= IntToPtr(lSkillTID);
	pvBuffer[6] = UintToPtr(ulAdditionalEffect);
	pvBuffer[7] = (PVOID) cHitIndex;

	EnumCallback(AGPMCHAR_CB_ID_RECEIVE_ACTION, pcsCharacter, pvBuffer);

	ActionCharacter((AgpdCharacterActionType) cAction, pcsCharacter, pcsTargetChar, NULL, (BOOL) cForceAttack, cActionResultType, pcsFactorDamage, cComboInfo, ulAdditionalEffect, cHitIndex, lSkillTID);

	return TRUE;
}

/*****************************************************************
*   Function : ReceiveAddCharacter
*   Comment  : Character�� Add�϶�� ��Ŷ�� �޴� 
*   Date&Time : 2002-04-28, ���� 5:22
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgpmCharacter::ReceiveAddCharacter()
{
	// ������� Add�� �ɸ����� ������ ���� ������ ����ؾ� �Ѵ�. 
	// m_csACharacter.AddCharacter�� ���� ������ �޸𸮿� �����ϰ� 
	// EnumCallback( AGPMCHAR_CB_ID_ADD_CHARACTER, (PVOID) ) ���ش�. 

	return TRUE;
}

BOOL AgpmCharacter::ReflectFactorPacket(AgpdCharacter *pcsCharacter, PVOID pvPacketFactor, AgpdFactor *pcsFactorDamageBuffer)
{
	PROFILE("AgpmCharacter::ReflectFactorPacket");

	if (!pcsCharacter || !pvPacketFactor || !m_pcsAgpmFactors)
		return FALSE;

	m_pcsAgpmFactors->ReflectPacket(&pcsCharacter->m_csFactor, pvPacketFactor, 0, pcsFactorDamageBuffer);

	return TRUE;
}

BOOL AgpmCharacter::RemoveCharacterFromMap(AgpdCharacter* pcsCharacter, BOOL bIsEnumCallback)
{
	PROFILE("AgpmCharacter::RemoveCharacterFromMap");

	if (!pcsCharacter || !m_pcsApmMap)
		return FALSE;

	if (pcsCharacter->m_bIsAddMap)
	{
		if (bIsEnumCallback)
			EnumCallback(AGPMCHAR_CB_ID_REMOVE_CHARACTER_FROM_MAP, pcsCharacter, NULL);

		if (IsMonster(pcsCharacter))
		{
			if (m_pcsApmMap->DeleteChar( pcsCharacter->m_nDimension , APMMAP_CHAR_TYPE_MONSTER, pcsCharacter->m_stPos, (INT_PTR)pcsCharacter))
			//if (m_pcsApmMap->DeleteMonster(pcsCharacter->m_stPos, (INT32) pcsCharacter))
			{
				pcsCharacter->m_bMove = FALSE;
				pcsCharacter->m_bIsAddMap = FALSE;

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else if (IsPC(pcsCharacter) || IsCreature(pcsCharacter))
		{
			if (m_pcsApmMap->DeleteChar( pcsCharacter->m_nDimension , APMMAP_CHAR_TYPE_PC, pcsCharacter->m_stPos, (INT_PTR) pcsCharacter))
			//if (m_pcsApmMap->DeleteChar(pcsCharacter->m_stPos, (INT32) pcsCharacter))
			{
				pcsCharacter->m_bMove = FALSE;
				pcsCharacter->m_bIsAddMap = FALSE;

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			if (m_pcsApmMap->DeleteChar( pcsCharacter->m_nDimension , APMMAP_CHAR_TYPE_NPC, pcsCharacter->m_stPos, (INT_PTR) pcsCharacter))
			//if (m_pcsApmMap->DeleteNPC(pcsCharacter->m_stPos, (INT32) pcsCharacter))
			{
				pcsCharacter->m_bMove = FALSE;
				pcsCharacter->m_bIsAddMap = FALSE;

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL AgpmCharacter::AddCharacterToMap(AgpdCharacter *pcsCharacter, BOOL bIsEnumCallback)
{
	PROFILE("AgpmCharacter::AddCharacterToMap");

	//@{ Jaewon 20051020
	//STOPWATCH("[AddCharacterToMap]\n");
	//@} Jaewon

	if (!pcsCharacter || !m_pcsApmMap)
		return FALSE;

	if (pcsCharacter->m_bIsAddMap)
		return TRUE;

	INT32	lHitRange	= 0;

	if (m_pcsAgpmFactors)
	{
		m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lHitRange, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HITRANGE);
	}

	//@{ Jaewon 20051020
	//STOPWATCH_START;
	//@} Jaewon

	if (IsMonster(pcsCharacter))
	{
		if (!m_pcsApmMap->AddChar( pcsCharacter->m_nDimension , APMMAP_CHAR_TYPE_MONSTER, pcsCharacter->m_stPos, (INT_PTR) pcsCharacter, (INT_PTR) pcsCharacter->m_lID, lHitRange))
			return FALSE;

		/*
		if (!m_pcsApmMap->AddMonster(pcsCharacter->m_stPos, (INT32) pcsCharacter, lHitRange))
			return FALSE;
		*/
	}
	else if (IsPC(pcsCharacter) || IsCreature(pcsCharacter))
	{
		if (!m_pcsApmMap->AddChar( pcsCharacter->m_nDimension , APMMAP_CHAR_TYPE_PC, pcsCharacter->m_stPos, (INT_PTR) pcsCharacter, (INT_PTR) pcsCharacter->m_lID, lHitRange))
			return FALSE;

		/*
		if (!m_pcsApmMap->AddChar(pcsCharacter->m_stPos, (INT32) pcsCharacter, lHitRange))
			return FALSE;
		*/
	}
	else
	{
		if (!m_pcsApmMap->AddChar( pcsCharacter->m_nDimension , APMMAP_CHAR_TYPE_NPC, pcsCharacter->m_stPos, (INT_PTR) pcsCharacter, (INT_PTR) pcsCharacter->m_lID, lHitRange))
			return FALSE;

		/*
		if (!m_pcsApmMap->AddNPC(pcsCharacter->m_stPos, (INT32) pcsCharacter, lHitRange))
			return FALSE;
		*/
	}

	//@{ Jaewon 20051020
	//STOPWATCH_STOP("(1)");
	//STOPWATCH_START;
	//@} Jaewon

	//@{ Jaewon 20051020
	//STOPWATCH_STOP("(2)");
	//STOPWATCH_START;
	//@} Jaewon

	pcsCharacter->m_bIsAddMap = TRUE;

	if (bIsEnumCallback)
		EnumCallback(AGPMCHAR_CB_ID_ADD_CHARACTER_TO_MAP, pcsCharacter, NULL);

	UpdateRegion(pcsCharacter);

	//@{ Jaewon 20051020
	//STOPWATCH_STOP("(3)");
	//@} Jaewon

	return TRUE;
}

INT32 AgpmCharacter::GetUnion(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return (-1);

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
		return GetUnion(m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor));

	return GetUnion(m_pcsAgpmFactors->GetRace(pcsResultFactor));
}

INT32 AgpmCharacter::GetUnion(INT32 lRace)
{
	return lRace;
}

INT32 AgpmCharacter::GetMurdererPoint(INT32 lCID)
{
	return GetMurdererPoint(GetCharacter(lCID));
}

INT32 AgpmCharacter::GetMurdererPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return 0;

	INT32	lMurdererPoint = 0;

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
		m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
	else
		m_pcsAgpmFactors->GetValue(pcsResultFactor, &lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);

	return lMurdererPoint;
}

INT32 AgpmCharacter::GetCharismaPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return 0;

	INT32	lPoint = 0;

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
		m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	else
		m_pcsAgpmFactors->GetValue(pcsResultFactor, &lPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

	return lPoint;
}

INT32 AgpmCharacter::GetRemainedCriminalTime(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	// 2006.02.07. steeple
	if (pcsCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_INNOCENT)
		return 0;

	__time32_t timeval = 0;
	_time32(&timeval);
	INT32 lCurrentClock = (INT32) timeval;
	
	INT32 lElapsed = lCurrentClock - pcsCharacter->m_lLastUpdateCriminalFlagTime;
	if (lElapsed >= AGPMCHARACTER_CRIMINAL_PRESERVE_TIME)
		return 0;
	else if (lElapsed < 0)
		return AGPMCHARACTER_CRIMINAL_PRESERVE_TIME;
	else
		return (AGPMCHARACTER_CRIMINAL_PRESERVE_TIME - lElapsed);
}

INT32 AgpmCharacter::GetRemainedMurdererTime(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	// 2006.02.07. steeple
	if (GetMurdererPoint(pcsCharacter) == 0)
		return 0;

	__time32_t timeval = 0;
	_time32(&timeval);
	INT32 lCurrentClock = (INT32) timeval;

	INT32 lElapsed = lCurrentClock - pcsCharacter->m_lLastUpdateMurdererPointTime;

	if( lElapsed >= AGPMCHARACTER_MURDERER_PRESERVE_TIME )
		return 0;
	else if( lElapsed < 0 )
		return AGPMCHARACTER_MURDERER_PRESERVE_TIME;
	else
		return AGPMCHARACTER_MURDERER_PRESERVE_TIME - lElapsed;
}

UINT32 AgpmCharacter::SetLastCriminalTime(AgpdCharacter *pcsCharacter, INT32 lRemain)
{
	if (!pcsCharacter)
		return 0;

	if (lRemain > 0)
	{
		__time32_t timeval = 0;
		_time32(&timeval);
		INT32 lCurrentClock = (INT32) timeval;
	
		INT32 l = min(AGPMCHARACTER_CRIMINAL_PRESERVE_TIME, lRemain);
		pcsCharacter->m_lLastUpdateCriminalFlagTime = lCurrentClock -
							(AGPMCHARACTER_CRIMINAL_PRESERVE_TIME - l);
	}
	
	return pcsCharacter->m_lLastUpdateCriminalFlagTime;
}

UINT32 AgpmCharacter::SetLastMurdererTime(AgpdCharacter *pcsCharacter, INT32 lRemain)
{
	if (!pcsCharacter)
		return 0;

	if (lRemain > 0)
	{
		__time32_t timeval = 0;
		_time32(&timeval);
		INT32 lCurrentClock = (INT32) timeval;
	
		INT32 l = min(AGPMCHARACTER_MURDERER_PRESERVE_TIME, lRemain);
		pcsCharacter->m_lLastUpdateMurdererPointTime = lCurrentClock -
						(AGPMCHARACTER_MURDERER_PRESERVE_TIME - l);
	}
	
	return pcsCharacter->m_lLastUpdateMurdererPointTime;
}

UINT32 AgpmCharacter::GetLastKilledTimeInBattleSquare(AgpdCharacter *pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	return pcsCharacter->m_ulBattleSquareKilledTime;
}

UINT32 AgpmCharacter::SetLastKilledTimeInBattleSquare(AgpdCharacter *pcsCharacter, UINT32 ulKilledTime)
{
	if(!pcsCharacter)
		return 0;

	if(ulKilledTime > 0)
		pcsCharacter->m_ulBattleSquareKilledTime = ulKilledTime;

	return pcsCharacter->m_ulBattleSquareKilledTime;
}

INT32 AgpmCharacter::GetMurdererLevel(INT32 lCID)
{
	return GetMurdererLevel(GetCharacter(lCID));
}

INT32 AgpmCharacter::GetMurdererLevel(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return AGPMCHAR_MURDERER_LEVEL0_POINT;

	INT32 lPoint = GetMurdererPoint(pcsCharacter);
	
	return GetMurdererLevelOfPoint(lPoint);
}

INT32 AgpmCharacter::GetMurdererLevelOfPoint(INT32 lPoint)
{
	if (lPoint >= AGPMCHAR_MURDERER_LEVEL3_POINT)				// 100
		return AGPMCHAR_MURDERER_LEVEL3_POINT;
	else if (lPoint >= AGPMCHAR_MURDERER_LEVEL2_POINT)			// 60
		return AGPMCHAR_MURDERER_LEVEL2_POINT;
	else if (lPoint >= AGPMCHAR_MURDERER_LEVEL1_POINT)			// 40
		return AGPMCHAR_MURDERER_LEVEL1_POINT;

	return AGPMCHAR_MURDERER_LEVEL0_POINT;
}

INT32 AgpmCharacter::GetMukzaPoint(INT32 lCID)
{
	return GetMukzaPoint(GetCharacter(lCID));
}

INT32 AgpmCharacter::GetMukzaPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return 0;

	INT32	lMukzaPoint = 0;

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
	{
		m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMukzaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);
	}

	m_pcsAgpmFactors->GetValue(pcsResultFactor, &lMukzaPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MUKZA);

	return lMukzaPoint;
}

BOOL AgpmCharacter::IsMurderer(INT32 lCID, INT32 *plPoint)
{
	return IsMurderer(GetCharacter(lCID), plPoint);
}

BOOL AgpmCharacter::IsMurderer(AgpdCharacter *pcsCharacter, INT32 *plPoint)
{
	if (!pcsCharacter)
		return FALSE;

	INT32 lPoint = GetMurdererPoint(pcsCharacter);

	if (plPoint)
		*plPoint = lPoint;
	
	if (lPoint >= AGPMCHAR_MURDERER_LEVEL1_POINT)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsMukza(INT32 lCID)
{
	return IsMukza(GetCharacter(lCID));
}

BOOL AgpmCharacter::IsMukza(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (GetMukzaPoint(pcsCharacter) > 0)
		return TRUE;

	return FALSE;
}

INT64 AgpmCharacter::GetLevelUpExp(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return 0;

	return GetLevelUpExp(GetLevel(pcsCharacter));
}

INT64 AgpmCharacter::GetLevelUpExp(INT32 lLevel)
{
	if (lLevel > AGPMCHAR_MAX_LEVEL || lLevel < 1)
		return 0;

	return g_llLevelUpExp[lLevel];
}

INT32 AgpmCharacter::GetCharGrowUpTemplate(CHAR *szData, AgpdCharacterTemplate **ppcsTemplate, INT32 lCount)
{
	if (!szData || !ppcsTemplate || lCount < 1)
		return 0;

	INT32	lStringLength	= (INT32)strlen(szData);

	INT32	lNameFirst		= 0;

	INT32	lNumTemplate	= 0;

	CHAR	szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	int i = 0;
	for (i = 0; i < lStringLength; i++)
	{
		if (szData[i] == '/')
		{
			if (lNumTemplate > lCount - 1)
				return 0;

			ZeroMemory(szBuffer, sizeof(CHAR) * 128);

			strncpy(szBuffer, szData + lNameFirst, i - lNameFirst);

			ppcsTemplate[lNumTemplate++] = GetCharacterTemplate(atoi(szBuffer));

			lNameFirst = i + 1;
		}
	}

	strncpy(szBuffer, szData + lNameFirst, i - lNameFirst);

	ppcsTemplate[lNumTemplate++] = GetCharacterTemplate(atoi(szBuffer));

	return lNumTemplate;
}

BOOL AgpmCharacter::SetCharGrowUpFactor(AgpdFactor *pcsFactor, CHAR *szColumnName, CHAR *szValue)
{
	if (!m_pcsAgpmFactors || !pcsFactor || !szColumnName || !szValue || !strlen(szColumnName) || !strlen(szValue))
		return FALSE;

	INT32	lValue	= atoi(szValue);
	FLOAT	fValue	= (FLOAT)atof(szValue);

	if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_STR, strlen(AGPMCHAR_EXCEL_COLUMN_STR)))
	{
		AgpdFactorCharStatus *pcsCharStatus = 
			(AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if (!pcsCharStatus)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_DEX, strlen(AGPMCHAR_EXCEL_COLUMN_DEX)))
	{
		AgpdFactorCharStatus *pcsCharStatus = 
			(AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if (!pcsCharStatus)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_CON, strlen(AGPMCHAR_EXCEL_COLUMN_CON)))
	{
		AgpdFactorCharStatus *pcsCharStatus = 
			(AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if (!pcsCharStatus)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_WIS, strlen(AGPMCHAR_EXCEL_COLUMN_WIS)))
	{
		AgpdFactorCharStatus *pcsCharStatus = 
			(AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if (!pcsCharStatus)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_INT, strlen(AGPMCHAR_EXCEL_COLUMN_INT)))
	{
		AgpdFactorCharStatus *pcsCharStatus = 
			(AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if (!pcsCharStatus)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_CHA, strlen(AGPMCHAR_EXCEL_COLUMN_CHA)))
	{
		AgpdFactorCharStatus *pcsCharStatus = 
			(AgpdFactorCharStatus *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_STATUS);
		if (!pcsCharStatus)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MAX_HP, strlen(AGPMCHAR_EXCEL_COLUMN_MAX_HP)))
	{
		AgpdFactorCharPointMax *pcsCharPointMax = 
			(AgpdFactorCharPointMax *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (!pcsCharPointMax)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MAX_MP, strlen(AGPMCHAR_EXCEL_COLUMN_MAX_MP)))
	{
		AgpdFactorCharPointMax *pcsCharPointMax = 
			(AgpdFactorCharPointMax *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (!pcsCharPointMax)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MAX_SP, strlen(AGPMCHAR_EXCEL_COLUMN_MAX_SP)))
	{
		AgpdFactorCharPointMax *pcsCharPointMax = 
			(AgpdFactorCharPointMax *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (!pcsCharPointMax)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MAX_AR, strlen(AGPMCHAR_EXCEL_COLUMN_MAX_AR)))
	{
		AgpdFactorCharPointMax *pcsCharPointMax = 
			(AgpdFactorCharPointMax *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (!pcsCharPointMax)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MAX_DR, strlen(AGPMCHAR_EXCEL_COLUMN_MAX_DR)))
	{
		AgpdFactorCharPointMax *pcsCharPointMax = 
			(AgpdFactorCharPointMax *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (!pcsCharPointMax)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MAX_MAP, strlen(AGPMCHAR_EXCEL_COLUMN_MAX_MAP)))
	{
		AgpdFactorCharPointMax *pcsCharPointMax = 
			(AgpdFactorCharPointMax *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (!pcsCharPointMax)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, lValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAP);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_DEF_PHY, strlen(AGPMCHAR_EXCEL_COLUMN_DEF_PHY)))
	{
		AgpdFactorDefense *pcsDefense = 
			(AgpdFactorDefense *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsDefense)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_DEF_FIRE, strlen(AGPMCHAR_EXCEL_COLUMN_DEF_FIRE)))
	{
		AgpdFactorDefense *pcsDefense = 
			(AgpdFactorDefense *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsDefense)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_DEF_WATER, strlen(AGPMCHAR_EXCEL_COLUMN_DEF_WATER)))
	{
		AgpdFactorDefense *pcsDefense = 
			(AgpdFactorDefense *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsDefense)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_DEF_EARTH, strlen(AGPMCHAR_EXCEL_COLUMN_DEF_EARTH)))
	{
		AgpdFactorDefense *pcsDefense = 
			(AgpdFactorDefense *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsDefense)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_DEF_AIR, strlen(AGPMCHAR_EXCEL_COLUMN_DEF_AIR)))
	{
		AgpdFactorDefense *pcsDefense = 
			(AgpdFactorDefense *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsDefense)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_DEF_MAGIC, strlen(AGPMCHAR_EXCEL_COLUMN_DEF_MAGIC)))
	{
		AgpdFactorDefense *pcsDefense = 
			(AgpdFactorDefense *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DEFENSE);
		if (!pcsDefense)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MIN_DMG_PHY, strlen(AGPMCHAR_EXCEL_COLUMN_MIN_DMG_PHY)))
	{
		AgpdFactorDamage *pcsDamage = 
			(AgpdFactorDamage *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DAMAGE);
		if (!pcsDamage)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

		return TRUE;
	}
	else if (!strncmp(szColumnName, AGPMCHAR_EXCEL_COLUMN_MAX_DMG_PHY, strlen(AGPMCHAR_EXCEL_COLUMN_MAX_DMG_PHY)))
	{
		AgpdFactorDamage *pcsDamage = 
			(AgpdFactorDamage *) m_pcsAgpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_DAMAGE);
		if (!pcsDamage)
			return FALSE;

		m_pcsAgpmFactors->SetValue(pcsFactor, fValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

		return TRUE;
	}

	return FALSE;
}

INT64 AgpmCharacter::GetExp(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return (0);

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
		return m_pcsAgpmFactors->GetExp(&pcsCharacter->m_csFactor);

	return m_pcsAgpmFactors->GetExp(pcsResultFactor);
}

INT32 AgpmCharacter::GetSkillPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return 0;

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
		return m_pcsAgpmFactors->GetSkillPoint(&pcsCharacter->m_csFactor);

	return m_pcsAgpmFactors->GetSkillPoint(pcsResultFactor);
}

INT32 AgpmCharacter::GetHeroicPoint(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter || NULL == m_pcsAgpmFactors)
		return 0;

	AgpdFactor *pcsResultFacter = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsResultFacter)
		return m_pcsAgpmFactors->GetHeroicPoint(&pcsCharacter->m_csFactor);

	return m_pcsAgpmFactors->GetHeroicPoint(pcsResultFacter);
}

INT32 AgpmCharacter::GetLevel(AgpdCharacter *pcsCharacter)
{
	return m_pcsAgpmFactors->GetLevel(&pcsCharacter->m_csFactor);
}

INT32	AgpmCharacter::GetLevelBefore(AgpdCharacter *pcsCharacter)
{
	return m_pcsAgpmFactors->GetLevelBefore(&pcsCharacter->m_csFactor);
}

INT32	AgpmCharacter::GetLevelLimited(AgpdCharacter *pcsCharacter)
{
	return m_pcsAgpmFactors->GetLevelLimited(&pcsCharacter->m_csFactor);
}

// 2006.10.19. steeple
// ���Ͱ��� ���õ� ������ ��´�.
INT32 AgpmCharacter::GetLevelFromTemplate(AgpdCharacterTemplate* pcsTemplate)
{
	if(!pcsTemplate)
		return FALSE;

	return m_pcsAgpmFactors->GetLevel(&pcsTemplate->m_csFactor);
}

// 2006.11.15. steeple
// AgpdCharacter* �� HP ���� �� �ְԲ� �Ϸ���. (�Ź� Factor ���⵵ �ȴ�)
INT32 AgpmCharacter::GetHP(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	return m_pcsAgpmFactors->GetHP((AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
}

INT32 AgpmCharacter::GetMaxHP(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	return m_pcsAgpmFactors->GetMaxHP((AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
}

BOOL AgpmCharacter::AddAttackerToList(AgpdCharacter *pcsCharacter, INT32 lAttackerID)
{
	//if (!pcsCharacter || lAttackerID == AP_INVALID_CID)
	//	return FALSE;

	//for (int i = 0; i < AGPMCHAR_MAX_ATTACKER_LIST; ++i)
	//{
	//	// �̹� ����ִ� ������ ����.
	//	if (pcsCharacter->m_lAttackerList[i] == lAttackerID)
	//		return FALSE;

	//	if (pcsCharacter->m_lAttackerList[i] == AP_INVALID_CID)
	//	{
	//		pcsCharacter->m_lAttackerList[i] = lAttackerID;
	//		break;
	//	}
	//}

	//if (i == AGPMCHAR_MAX_ATTACKER_LIST)
	//	pcsCharacter->m_lAttackerList[0] = lAttackerID;

	//EnumCallback(AGPMCHAR_CB_ID_ADD_ATTACKER_TO_LIST, pcsCharacter, &lAttackerID);

	return TRUE;
}

BOOL AgpmCharacter::IsInAttackerList(AgpdCharacter *pcsCharacter, INT32 lAttackerID)
{
	//if (!pcsCharacter || lAttackerID == AP_INVALID_CID)
	//	return FALSE;

	//for (int i = 0; i < AGPMCHAR_MAX_ATTACKER_LIST; ++i)
	//{
	//	if (pcsCharacter->m_lAttackerList[i] == AP_INVALID_CID)
	//		return FALSE;

	//	if (pcsCharacter->m_lAttackerList[i] == lAttackerID)
	//		return TRUE;
	//}

	return FALSE;
}

BOOL AgpmCharacter::ClearAttackerToList(AgpdCharacter *pcsCharacter)
{
	//if (!pcsCharacter)
	//	return FALSE;

	//pcsCharacter->m_lAttackerList.MemSet(0, AGPMCHAR_MAX_ATTACKER_LIST);
	//ZeroMemory(pcsCharacter->m_lAttackerList, sizeof(INT32) * AGPMCHAR_MAX_ATTACKER_LIST);

	return TRUE;
}

INT32 AgpmCharacter::GetMoveSpeed(INT32 lCID)
{
	return GetMoveSpeed(GetCharacter(lCID));
}

INT32 AgpmCharacter::GetMoveSpeed(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return 0;

	INT32	lMoveSpeed	= 0;

	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMoveSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);

	// 2005.10.19. steeple
	// MoveSpeed �� ��ų�̶���� �ܺ� ���ο� ���ؼ� 0 �� �� ���� �ִ�.
	// �̰� �׽�Ʈ�� ���� �غ��� �ϹǷ�, ���縮 ���󼭹��� �ø��� ����.
	//
	//if (lMoveSpeed == 0)
	//{
	//	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMoveSpeed, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
	//}

	if(lMoveSpeed + pcsCharacter->m_lAdjustMoveSpeed != 0)
		return lMoveSpeed + pcsCharacter->m_lAdjustMoveSpeed;
	else
	{
		if(IsPC(pcsCharacter) || IsMonster(pcsCharacter))
			return 1;
		else
			return lMoveSpeed + pcsCharacter->m_lAdjustMoveSpeed;
	}
}

INT32 AgpmCharacter::GetFastMoveSpeed(INT32 lCID)
{
	return GetFastMoveSpeed(GetCharacter(lCID));
}

INT32 AgpmCharacter::GetFastMoveSpeed(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pcsAgpmFactors)
		return 0;

	INT32	lMoveSpeed	= 0;

	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMoveSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

	// 2005.10.19. steeple
	// MoveSpeed �� ��ų�̶���� �ܺ� ���ο� ���ؼ� 0 �� �� ���� �ִ�.
	// �̰� �׽�Ʈ�� ���� �غ��� �ϹǷ�, ���縮 ���󼭹��� �ø��� ����.
	//
	//if (lMoveSpeed == 0)
	//{
	//	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMoveSpeed, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
	//}

	if(lMoveSpeed + pcsCharacter->m_lAdjustMoveSpeed != 0)
		return lMoveSpeed + pcsCharacter->m_lAdjustMoveSpeed;
	else
	{
		if(IsPC(pcsCharacter) || IsMonster(pcsCharacter))
			return 1;
		else
			return lMoveSpeed + pcsCharacter->m_lAdjustMoveSpeed;
	}
}

BOOL AgpmCharacter::ProcessRemove(UINT32 ulClockCount)
{
	PROFILE("AgpmCharacter::ProcessRemove");

	if (m_ulPrevRemoveClockCount + AGPMCHARACTER_PROCESS_REMOVE_INTERVAL > ulClockCount)
		return TRUE;

	INT32	lIndex	= 0;
	AgpdCharacter	*pcsCharacter	= NULL;

	AgpdCharacter	**ppcsCharacter = (AgpdCharacter **) m_csAdminCharacterRemove.GetObjectSequence(&lIndex);


	// ������ , �ɸ��� ���ø� ���� �ʰ��� ���� �Ҵ����� ����.
	// INT32	lCharacterNumBuffer[1000];
	//static INT32 * lCharacterNumBuffer = NULL;
	//if( NULL == lCharacterNumBuffer )
	//{
	//	lCharacterNumBuffer = new INT32[ GetMaxTID() ];
	//}

	//if (g_bPrintRemovePoolTID)
	//{
	//	ZeroMemory(lCharacterNumBuffer, sizeof(lCharacterNumBuffer));
	//}

	//INT32	lLoopCount		= 0;

	//INT32	lIsNotReady		= 0;
	//INT32	lIsPostedCount	= 0;
	INT32	lIsTimeoutCount	= 0;

	while (ppcsCharacter && *ppcsCharacter)
	{
		INT32	lRemovedCID	= AP_INVALID_CID;
		pcsCharacter		= *ppcsCharacter;

		if (!pcsCharacter->m_bIsPostedIdleProcess &&
			pcsCharacter->m_ulRemoveTimeMSec + g_ulReserveCharacterData < ulClockCount)
		{
			// Idle Thread�� ����Ǵ� ���� ���⼭ DestroyCharacter() �ϴ� ���� �ִ�.
			pcsCharacter->m_Mutex.WLock();
			pcsCharacter->m_Mutex.Release();

			// ���� ��� ����Ÿ�� ������ ���� �Ǿ���.
			m_csAdminCharacterRemove.RemoveObject((INT_PTR) pcsCharacter);
			lIndex = 0;
			DestroyCharacter(pcsCharacter);
		}
		//else
		//{
		//	// �α� ����Ÿ�� ����� ����.
		//	if (!pcsCharacter->m_bIsPostedIdleProcess &&
		//		pcsCharacter->m_ulRemoveTimeMSec + g_ulReserveCharacterData >= ulClockCount)
		//		++lIsNotReady;
		//	else if (pcsCharacter->m_bIsPostedIdleProcess &&
		//		pcsCharacter->m_ulRemoveTimeMSec + g_ulReserveCharacterData < ulClockCount)
		//		++lIsPostedCount;
		//	else
		//		++lIsTimeoutCount;
		//
		//	//if (g_bPrintRemovePoolTID)
		//	//{
		//	//	++lCharacterNumBuffer[pcsCharacter->m_pcsCharacterTemplate->m_lID];
		//	//}
		//}

		//++lLoopCount;

		ppcsCharacter = (AgpdCharacter **) m_csAdminCharacterRemove.GetObjectSequence(&lIndex);
	}

	//if (g_bPrintRemovePoolTID)
	//{
		//for (int nIndex	= 0; nIndex <1000; ++nIndex)
		//{
		//	AuLogFile("PrintRemovePoolTID.txt", "%d : %d\n", nIndex, lCharacterNumBuffer[nIndex]);
		//}

	//	AuLogFile("PrintRemovePoolTID.txt", "ProcessRemove() loop count is %d (max is %d)\n", lLoopCount, g_lMaxRemovePoolCount);
	//	AuLogFile("PrintRemovePoolTID.txt", "lIsNotReady : %d\n", lIsNotReady);
	//	AuLogFile("PrintRemovePoolTID.txt", "lIsPostedCount : %d\n", lIsPostedCount);
	//	AuLogFile("PrintRemovePoolTID.txt", "lIsTimeoutCount : %d\n", lIsTimeoutCount);

	//	g_bPrintRemovePoolTID	= FALSE;
	//}

	m_ulPrevRemoveClockCount = ulClockCount;

	return TRUE;
}

BOOL AgpmCharacter::AddRemoveCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	pcsCharacter->m_ulRemoveTimeMSec	= GetClockCount();

	if (g_lMaxRemovePoolCount < GetRemovePoolCount())
		g_lMaxRemovePoolCount	= GetRemovePoolCount();

	if (!m_csAdminCharacterRemove.AddObject(&pcsCharacter, (INT_PTR) pcsCharacter))
	{
		AgpdCharacter	**ppcsCharacter = (AgpdCharacter **) m_csAdminCharacterRemove.GetObject((INT_PTR) pcsCharacter);
		if (ppcsCharacter && *ppcsCharacter)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgpmCharacter::AddRemoveCharacter() Already exist in Pool !!!\n");
			AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
//			TRACEFILE(ALEF_ERROR_FILENAME,"AgpmCharacter::AddRemoveCharacter() Already exist in Pool !!!\n"); 

			/* �׳� ����� ������ �� �� �ִ�.
			m_csAdminCharacterRemove.RemoveObject((INT32) (*ppcsCharacter));

			DestroyCharacter(*ppcsCharacter);

//			if (m_csAdminCharacterRemove.AddObject(&pcsCharacter, (INT32) pcsCharacter))
//				return TRUE;
			*/
		}
		else
		{
//			TRACEFILE(ALEF_ERROR_FILENAME,"AgpmCharacter::AddRemoveCharacter() Remove Pool Full !!!\n");
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgpmCharacter::AddRemoveCharacter() Remove Pool Full !!!\n");
			AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);

			if (pcsCharacter)
				DestroyCharacter(pcsCharacter);
		}

		return FALSE;
	}

	return TRUE;
}

UINT32 AgpmCharacter::GetAttackIntervalMSec(AgpdCharacter *pcsCharacter)
{
	// ���� �ӵ��� �����´�.
	INT32	lAttackSpeed	= 0;

	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lAttackSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
	if (lAttackSpeed < 1)
		return 0;

	return (UINT32) ((60.0 / (FLOAT) lAttackSpeed) * 1000);		// �Ϻп� lAttackSpeed ��ŭ �����Ѵ�. (����� milisecond ������ ȯ���Ѵ�)
}

BOOL AgpmCharacter::IsCombatMode(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	return pcsCharacter->m_bIsCombatMode;
}

BOOL AgpmCharacter::SetCombatMode(AgpdCharacter *pcsCharacter, UINT32 ulClockCount)
{
	if (!pcsCharacter)
		return FALSE;

	BOOL	bChangeMode	= FALSE;

	if (!pcsCharacter->m_bIsCombatMode)
		bChangeMode	= TRUE;

	pcsCharacter->m_bIsCombatMode		= TRUE;
	pcsCharacter->m_ulSetCombatModeTime	= ulClockCount;

	if (bChangeMode)
		EnumCallback(AGPMCHAR_CB_ID_SET_COMBAT_MODE, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgpmCharacter::ResetCombatMode(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	BOOL	bChangeMode	= FALSE;

	if (pcsCharacter->m_bIsCombatMode)
		bChangeMode	= TRUE;

	pcsCharacter->m_bIsCombatMode		= FALSE;

	if (bChangeMode)
		EnumCallback(AGPMCHAR_CB_ID_RESET_COMBAT_MODE, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgpmCharacter::SetActionBlockTime(AgpdCharacter *pcsCharacter, UINT32 ulBlockTime, AgpdCharacterActionBlockType eBlockType)
{
	if (!pcsCharacter)
		return FALSE;

	pcsCharacter->m_bIsActionBlock		= TRUE;
	pcsCharacter->m_eActionBlockType	= eBlockType;

	UINT32	ulNewBlockTime	= GetClockCount() + ulBlockTime;

	if (pcsCharacter->m_ulActionBlockTime < ulNewBlockTime)
		pcsCharacter->m_ulActionBlockTime	= ulNewBlockTime;

	return TRUE;
}

BOOL AgpmCharacter::ClearActionBlockTime(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	pcsCharacter->m_bIsActionBlock		= FALSE;
	pcsCharacter->m_eActionBlockType	= AGPDCHAR_ACTION_BLOCK_TYPE_ALL;
	pcsCharacter->m_ulActionBlockTime	= 0;

	return TRUE;
}

BOOL AgpmCharacter::IsActionBlockCondition(AgpdCharacter *pcsCharacter, AgpdCharacterActionBlockType eBlockType)
{
	if (!pcsCharacter)
		return FALSE;

	if (!pcsCharacter->m_bIsActionBlock)
		return FALSE;

	BOOL	bIsCheckBlockTime	= FALSE;

	switch (eBlockType) {
	case AGPDCHAR_ACTION_BLOCK_TYPE_ALL:
		{
			bIsCheckBlockTime	= TRUE;
		}
		break;

	case AGPDCHAR_ACTION_BLOCK_TYPE_ATTACK:
		{
			if (pcsCharacter->m_eActionBlockType != AGPDCHAR_ACTION_BLOCK_TYPE_ATTACK)
				bIsCheckBlockTime	= TRUE;
		}
		break;

	case AGPDCHAR_ACTION_BLOCK_TYPE_SKILL:
		{
			if (pcsCharacter->m_eActionBlockType == AGPDCHAR_ACTION_BLOCK_TYPE_ATTACK)
				return FALSE;

			bIsCheckBlockTime	= TRUE;
		}
		break;
	}

	if (bIsCheckBlockTime)
	{
		if (pcsCharacter->m_ulActionBlockTime > GetClockCount())
			return TRUE;

		pcsCharacter->m_bIsActionBlock		= FALSE;
	}

	return FALSE;
}

BOOL AgpmCharacter::PvPModeOn()
{
	m_bIsAcceptPvP	= TRUE;

	return TRUE;
}

BOOL AgpmCharacter::PvPModeOff()
{
	m_bIsAcceptPvP	= FALSE;

	return TRUE;
}

BOOL AgpmCharacter::SetRegionRefreshInterval(UINT32 ulIntervalMSec)
{
	m_ulRegionRefreshInterval	= ulIntervalMSec;

	return TRUE;
}

UINT32 AgpmCharacter::GetRegionRefreshInterval()
{
	return m_ulRegionRefreshInterval;
}

PVOID AgpmCharacter::MakePacketOptionFlag(AgpdCharacter *pcsCharacter, INT32 lOptionFlag, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_OPTION_UPDATE;
	
	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
											&cOperation,										// Operation
											&pcsCharacter->m_lID,								// Character ID
											NULL,												// Character Template ID
											NULL,												// Game ID
											NULL,												// Character Status
											NULL,												// Move Packet
											NULL,												// Action Packet
											NULL,												// Factor Packet
											NULL,												// llMoney
											NULL,												// bank money
											NULL,												// cash
											NULL,												// character action status
											NULL,												// character criminal status
											NULL,												// attacker id (������� ������ �ʿ�)
											NULL,												// ���� �����Ǽ� �ʿ� �������� ����
											NULL,												// region index
											NULL,												// social action index
											NULL,												// special status
											NULL,												// is transform status
											NULL,												// skill initialization text
											NULL,												// face index
											NULL,												// hair index
											&lOptionFlag,										// Option Flag
											NULL,												// bank size
											NULL,												// event status flag
											NULL,												// remained criminal status time
											NULL,												// remained murderer point time
											NULL,												// nick name
											NULL,												// gameguard
											NULL												// last killed time in battlesquare
											);

	return	pvPacket;
}


PVOID AgpmCharacter::MakePacketCharView(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bIsNewChar)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_ADD;
	
	INT8 cMoveFlag = (pcsCharacter->m_eMoveDirection != MD_NODIRECTION ? AGPMCHARACTER_MOVE_FLAG_DIRECTION : 0) |
					 (pcsCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
			 		 (pcsCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_STOP) |
					 (pcsCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0) |
					 (pcsCharacter->m_bMoveFast ? AGPMCHARACTER_MOVE_FLAG_FAST : 0) |
					 (pcsCharacter->m_bMoveFollow ? AGPMCHARACTER_MOVE_FLAG_FOLLOW : 0) |
					 (pcsCharacter->m_bHorizontal ? AGPMCHARACTER_MOVE_FLAG_HORIZONTAL : 0);

	INT8 cMoveDirection = (INT8)pcsCharacter->m_eMoveDirection;

	PVOID pvPacketMove = m_csPacketMove.MakePacket(FALSE, pnPacketLength, 0, 
													&pcsCharacter->m_stPos,
													(pcsCharacter->m_bMove) ? &pcsCharacter->m_stDestinationPos : &pcsCharacter->m_stPos,
													&pcsCharacter->m_lFollowTargetID,
													&pcsCharacter->m_lFollowDistance,
													&pcsCharacter->m_fTurnX,
													&pcsCharacter->m_fTurnY,
													&cMoveFlag,
													&cMoveDirection);
	if (!pvPacketMove)
		return NULL;

	PVOID pvPacketFactor = m_pcsAgpmFactors->MakePacketFactorsCharView(&pcsCharacter->m_csFactor);
	if (!pvPacketFactor)
	{
		m_csPacketMove.FreePacket(pvPacketMove);
		return NULL;
	}

	INT8	cIsNewChar	= (INT8) bIsNewChar;

	UINT16	unIDLength	= (UINT16)strlen(pcsCharacter->m_szID);
	if (unIDLength > AGPDCHARACTER_MAX_ID_LENGTH)
		unIDLength	= AGPDCHARACTER_MAX_ID_LENGTH;

	INT8	cIsTransform		= 0;
	if (pcsCharacter->m_bIsTrasform)
		cIsTransform |= AGPMCHAR_FLAG_TRANSFORM;
	if (pcsCharacter->m_bRidable)
		cIsTransform |= AGPMCHAR_FLAG_RIDABLE;
	if (pcsCharacter->m_bIsEvolution)
		cIsTransform |= AGPMCHAR_FLAG_EVOLUTION;

	INT8	cFaceIndex	= (INT8)	pcsCharacter->m_lFaceIndex;
	INT8	cHairIndex	= (INT8)	pcsCharacter->m_lHairIndex;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
											&cOperation,												// Operation	
											&pcsCharacter->m_lID,										// Character ID
											&pcsCharacter->m_lTID1,										// Character Template ID
											pcsCharacter->m_szID, &unIDLength,							// Game ID
											&pcsCharacter->m_unCurrentStatus,							// Character Status
											pvPacketMove,												// Move Packet
											NULL,														// Action Packet
											pvPacketFactor,												// Factor Packet
											NULL,														// llMoney
											NULL,														// bank money
											NULL,														// cash
											&pcsCharacter->m_unActionStatus,							// character action status
											&pcsCharacter->m_unCriminalStatus,							// character criminal status
											NULL,														// attacker id (������� ������ �ʿ�)
											(bIsNewChar) ? &cIsNewChar : NULL,							// ���� �����Ǽ� �ʿ� �������� ����
											NULL,														// region index
											NULL,														// social action index
											&pcsCharacter->m_ulSpecialStatus,							// special status
											&cIsTransform,												// is transform status
											NULL,														// skill initialization text
											(cFaceIndex >= 0) ? &cFaceIndex : NULL,						// face index
											(cHairIndex >= 0) ? &cHairIndex : NULL,						// hair index
											&pcsCharacter->m_lOptionFlag,								// Option Flag
											NULL,														// bank size
											&pcsCharacter->m_unEventStatusFlag,							// event status flag
											NULL,														// remained criminal status time
											NULL,														// remained murderer point time
											pcsCharacter->m_szNickName,									// nick name
											NULL,														// gameguard
											NULL														// last killed time in battlesquare
											);

	m_csPacketMove.FreePacket(pvPacketMove);
	m_csPacketMove.FreePacket(pvPacketFactor);

	return	pvPacket;
}

BOOL AgpmCharacter::IsInPvPDanger(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	ApmMap::RegionTemplate	*pcsRegionTemplate	= m_pcsApmMap->GetTemplate(pcsCharacter->m_nBindingRegionIndex);
	if (pcsRegionTemplate)
	{
		if (pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_DANGER)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*
	By SungHoon
	���̵� ù���ڴ� �빮�� �������� �ҹ��ڷ� ������ش�.
*/
VOID AgpmCharacter::MakeGameID( CHAR *szSrcGameID, CHAR *szTargetGameID )
{
	szTargetGameID[0] = toupper(szSrcGameID[0]);
	size_t i = 1;
	for (i = 1 ; i < strlen(szSrcGameID);i++)
		szTargetGameID[i] = tolower(szSrcGameID[i]);
	szTargetGameID[i] = '\0';
}

BOOL AgpmCharacter::CheckNameKr(CHAR* pszCharName, int lCharNameLen)
{
	unsigned char	cData;
	unsigned char	cData2;

	for( INT32 lCounter=0; lCounter<lCharNameLen; lCounter++ )
	{
		cData = pszCharName[lCounter];

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
			if( lCounter < lCharNameLen )
			{
				if( (0xB0 <= cData) && (cData <= 0xC8) )
				{
					cData2 = pszCharName[lCounter+1];

					if( (0xA1 <= cData2) && (cData2 <= 0xFE) )
					{
						//�ѱ��̴ϱ� lCounter++ �Ѵ�.
						lCounter++;
						continue;
					}
				}
			}
		}
		return FALSE;
	}
	return TRUE;
}

// �߱��� ĳ���͸��� �ùٸ��� Ȯ���Ѵ�.
// ��밡���� ���ڴ� ����, ������/��, �߱���� �����Ѵ�
BOOL AgpmCharacter::CheckNameCn(CHAR* pszCharName, int charNameLen)
{
	unsigned char c1;

	for ( int i = 0; i < charNameLen; ++i )
	{
		c1 = pszCharName[i];

		if ( c1 < '0'
			|| ( c1 > '9' && c1 < 'A' )
			|| ( c1 > 'Z' && c1 < 'a' )
			|| ( c1 > 'z' && c1 <= 127 ) )
			return FALSE;
	}

	return TRUE;
}


// �븸�� ĳ���͸� üũ
BOOL AgpmCharacter::CheckNameTw(CHAR* pszCharName, int charNameLen)
{
	typedef unsigned short towbyte_t;
	typedef std::map< towbyte_t, towbyte_t, std::greater< towbyte_t > > AllowRanges;

	static AllowRanges allowRanges;

	// init allow char table
	if( allowRanges.empty() )
	{
		//insertRange( 0xa2cf, 0xa2fe ); // A~v
		//insertRange( 0xa340, 0xa343 ); // w~z

		// page 1
		allowRanges[ 0xa440 ] = 0xa47e;

		bool odd = true;

		for( towbyte_t i=0xa4a1; i<0xc641; )
		{
			if( odd ) {
				allowRanges[i] = i+93;
				i += 159;
			}
			else {
				allowRanges[i] = i+62;
				i += 97;
			}

			odd = !odd;
		}

		// page 2
		allowRanges[0xc940] = 0xc97e;

		odd = true;

		for( towbyte_t i=0xc9a1; i<0xf97e; )
		{
			if( odd ) {
				allowRanges[i] = i+93;
				i += 159;
			}
			else {
				allowRanges[i] = i+62;
				i += 97;
			}

			odd = !odd;
		}

		allowRanges[ 0xf9a1 ] = 0xf9d5;
	}

	BOOL result = TRUE;

	// check
	for ( int i = 0; i < charNameLen; )
	{
		unsigned char c = (unsigned char)pszCharName[i];

		// is ascii
		if(c <= 127)
		{
			++i;

			if(
				(48 <= c && c <= 57) //0~9
				|| ( 65 <= c && c <= 90 ) // A~Z
				|| ( 97 <= c && c <= 122 ) // a~z
				)
			{
				continue;
			}
		}

		// �ƽ�Ű ���ڰ� �ƴϸ鼭 2����Ʈ�� �ƴϸ� ��ȿ
		if( i+2 > charNameLen )
		{
			result = FALSE;
			break;
		}

		// is big-5

		towbyte_t bc = *(towbyte_t*)(&pszCharName[i]);

		// �յ� ����Ʈ�� ������ �ٲ�
		bc = ((((towbyte_t)0x00ff)&bc) << 8) | (bc >> 8);


		i+= 2;

		AllowRanges::iterator iter = allowRanges.lower_bound( bc );

		// ��� ������ ���
		if( iter == allowRanges.end() )
		{
			result = FALSE;
			break;
		}

		towbyte_t start = iter->first;
		towbyte_t end = iter->second;

		// ��� ������ ���
		if( bc < start || end < bc ) 
		{
			result = FALSE;
			break;
		}
	}

	return result;
}

BOOL AgpmCharacter::CheckNameJp( CHAR* pszCharName, int charNameLen )
{
	unsigned char c1;

	for ( int i = 0; i < charNameLen; ++i )
	{
		c1 = pszCharName[i];

		// number
		if ( c1 >= '0' && c1 <= '9' ) continue;

		// large character
		if ( c1 >= 'A' && c1 <= 'Z' ) continue;

		// small character
		if ( c1 >= 'a' && c1 <= 'z' ) continue;

		// single byte japanese
		if ( c1 >= 0xA2 && c1 <= 0xDD ) continue;

		// two byte characters
		if ( ( 0x80 < c1 && c1 <= 0x9F ) || ( c1 >= 0xE0 ) )
		{
			union
			{
				unsigned short mbc;
				char           sbc[2];
			} dbc;

			dbc.sbc[0] = pszCharName[i+1];
			dbc.sbc[1] = pszCharName[i];

			// ���� ��ȣ�� ����� ���� ��ȣ

			if ( dbc.mbc >= 0x8141 && dbc.mbc <= 0x81AD )
			{
				++i;
				continue;
			}

			// ���� ����
			if ( dbc.mbc >= 0x824F && dbc.mbc <= 0x8258 ) {
				++i;
				continue;
			}

			// ���� �빮�� ���ĺ�
			if ( dbc.mbc >= 0x8260 && dbc.mbc <= 0x8279 ) {
				++i;
				continue;
			}

			// ���� �ҹ��� ���ĺ�
			if ( dbc.mbc >= 0x8281 && dbc.mbc <= 0x829A ) {
				++i;
				continue;
			}

			// ���󰡳�, īŸ����
			if ( dbc.mbc >= 0x829F && dbc.mbc <= 0x8396 ) {
				++i;
				continue;
			}

			// �ѹ�
			if ( dbc.mbc >= 0x889F && dbc.mbc <= 0xEAA4 ) {
				++i;
				continue;
			}
		}

		return FALSE;
	}


	return TRUE;
}

BOOL AgpmCharacter::CheckNameEn( CHAR* pszCharName, int charNameLen )
{
	unsigned char cData;

	for( INT32 lCounter=0; lCounter < charNameLen; lCounter++ )
	{
		cData = pszCharName[lCounter];

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

		// Ȯ�� �������� �˻��Ѵ�. 0xD7�� ���ϱ� ��ȣ, 0xF7�� ������ ��ȣ
		if ( 0xC0 <= cData && cData != 0xD7 && cData != 0xF7 )
		{
			continue;
		}

		return FALSE;
	}

	return TRUE;
}


/*
	Movedd SungHoon
	2005.05.27
	���̵� ������ ���̵����� üũ���ش�.(Ư�����ڸ� �˻��Ѵ�.)
*/
BOOL AgpmCharacter::CheckCharName(CHAR *pstrCharName, BOOL bCheckMinLength)
{
	int lCharNameLen = (int)strlen( pstrCharName );

	if ( (bCheckMinLength && lCharNameLen < 4) || lCharNameLen > 32)
		return FALSE;

	return CheckName(pstrCharName, lCharNameLen);
}

// 2005.07.01. steeple
// AgcmChatting �� �ִ� �� �ణ ����.
BOOL AgpmCharacter::CheckFilterText(CHAR* pszText)
{
	if(!m_aszFilterText || !pszText || strlen(pszText) < 1)
		return TRUE;

	std::string strTempText = pszText;

	if(!IsDBCSLeadByte(pszText[0]))
	{
		// lower original chatting mesasge
		std::string strTempBuffer = pszText;
		_strlwr( &strTempBuffer[ 0 ] );

		for (int i = 0; i < m_lNumFilterText; ++i)
		{
			if (strstr( &strTempBuffer[ 0 ], m_aszFilterText + AGPMCHARACTER_MAX_FILTER_TEXT * i ) )
				return FALSE;
		}
	}
	else
	{
		for(int i = 0; i < m_lNumFilterText; i++)
		{
			if(_mbsstr((const unsigned char*)strTempText.c_str(), (const unsigned char*)(m_aszFilterText + AGPMCHARACTER_MAX_FILTER_TEXT * i)))
				return FALSE;
		}
	}

	return TRUE;
}

// 2005.07.01. steeple
// AgcmChatting �� �ִ� �� ���� �״�� ����
BOOL AgpmCharacter::StreamReadChatFilterText(CHAR* szFile, BOOL bEncryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bEncryption))
	{
		csExcelTxtLib.CloseFile();
		OutputDebugString("AgcmChatting2::StreamReadChatFilterText() Error (1) !!!\n");
		return FALSE;
	}

	INT32			lRow			= csExcelTxtLib.GetRow();

	m_aszFilterText	= new CHAR [sizeof(CHAR) * AGPMCHARACTER_MAX_FILTER_TEXT * lRow];

	ZeroMemory(m_aszFilterText, sizeof(CHAR) * AGPMCHARACTER_MAX_FILTER_TEXT * lRow);

	INT32			lCurRow			= 0;

	char szFilter[AGPMCHARACTER_MAX_FILTER_TEXT];

	for ( ; ; )
	{
		if (lCurRow >= lRow)
			break;

		CHAR		*szFilterText	= csExcelTxtLib.GetData(0, lCurRow);

		++lCurRow;

		if (!szFilterText || !szFilterText[0])
			continue;

		CHAR	*szTextIndex	= m_aszFilterText + AGPMCHARACTER_MAX_FILTER_TEXT * m_lNumFilterText;

		strncpy(szFilter, szFilterText, AGPMCHARACTER_MAX_FILTER_TEXT - 1);
		szFilter[AGPMCHARACTER_MAX_FILTER_TEXT - 1] = 0;

		if( !IsDBCSLeadByte( szFilter[ 0 ] ) )
		{
			_strlwr(szFilter);
		}

		strcpy(szTextIndex, szFilter);
		++m_lNumFilterText;
	}

	csExcelTxtLib.CloseFile();

	return TRUE;
}

/*
	2005.05.31	By SungHoon
	�ź� ����� üũ�Ѵ�.
*/
BOOL AgpmCharacter::IsOptionFlag( AgpdCharacter *pcsCharacter, AgpdCharacterOptionFlag eOptionFlag )
{
	return IsOptionFlag(pcsCharacter->m_lOptionFlag ,eOptionFlag);
}

BOOL AgpmCharacter::IsOptionFlag( INT32 lOptionFlag , AgpdCharacterOptionFlag eOptionFlag )
{
	return ((lOptionFlag & eOptionFlag) == eOptionFlag);
}
/*
	2005.05.31	By SungHoon
	�ź� ����� �����Ѵ�.
*/
INT32 AgpmCharacter::SetOptionFlag( AgpdCharacter *pcsCharacter, AgpdCharacterOptionFlag eOptionFlag )
{
	return (pcsCharacter->m_lOptionFlag | eOptionFlag);
}

/*
	2005.05.31	By SungHoon
	�ź� ����� �����Ѵ�.
*/
INT32 AgpmCharacter::UnsetOptionFlag( AgpdCharacter *pcsCharacter, AgpdCharacterOptionFlag eOptionFlag )
{
	return (pcsCharacter->m_lOptionFlag & ~eOptionFlag);
}

/*
	2005.05.31	By SungHoon
	�ź� ����� �����Ѵ�.
*/
INT32 AgpmCharacter::SetOptionFlag( INT32 lOptionFlag, AgpdCharacterOptionFlag eOptionFlag )
{
	return (lOptionFlag | eOptionFlag);
}

/*
	2005.05.31	By SungHoon
	�ź� ����� �����Ѵ�.
*/
INT32 AgpmCharacter::UnsetOptionFlag( INT32 lOptionFlag, AgpdCharacterOptionFlag eOptionFlag )
{
	return (lOptionFlag & ~eOptionFlag);
}

/*
	2005.06.14 By SungHoon
	������ �۾��� �� ������ �Ҹ� CallBack
*/
BOOL AgpmCharacter::SetCallbackUpdateLevelPost(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_LEVEL_POST, pfCallback, pClass);
}

/*
	2005.06.15 By SungHoon
	���� ĳ������ ���� ��Ŷ�� �����.
*/
PVOID AgpmCharacter::MakePacketCharLevel(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_LEVEL_UP;
	
	PVOID pvPacketFactor = m_pcsAgpmFactors->MakePacketLevel(&pcsCharacter->m_csFactor);
	if (!pvPacketFactor)
		return NULL;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
											&cOperation,										// Operation
											&pcsCharacter->m_lID,								// Character ID
											NULL,												// Character Template ID
											NULL,												// Game ID
											NULL,												// Character Status
											NULL,												// Move Packet
											NULL,												// Action Packet
											pvPacketFactor,										// Factor Packet
											NULL,												// llMoney
											NULL,												// bank money
											NULL,												// cash
											NULL,												// character action status
											NULL,												// character criminal status
											NULL,												// attacker id (������� ������ �ʿ�)
											NULL,												// ���� �����Ǽ� �ʿ� �������� ����
											NULL,												// region index
											NULL,												// social action index
											NULL,												// special status
											NULL,												// is transform status
											NULL,												// skill initialization text
											NULL,												// face index
											NULL,												// hair index
											NULL,												// Option Flag
											NULL,												// bank size
											NULL,												// event status flag
											NULL,												// remained criminal status time
											NULL,												// remained murderer point time
											NULL,												// nick name
											NULL,												// gameguard
											NULL												// last killed time in battlesquare
											);

	return	pvPacket;

}

PVOID AgpmCharacter::MakePacketCharAction(INT16 *pnPacketLength, AgpdCharacterActionType eAction, INT32 lTargetCID, INT32 lSkillTID, AgpdCharacterActionResultType eActionResult, PVOID pvFactor, AuPOS *pstPos, UINT8 ucCombo, BOOL bForce, AgpdCharacterAdditionalEffect eEffect, UINT8 cHitIndex)
{
	return m_csPacketAction.MakePacket(FALSE, pnPacketLength, 0,
							eAction == AGPDCHAR_ACTION_TYPE_NONE ? NULL : &eAction,			// action type
							lTargetCID == -1 ? NULL : &lTargetCID,							// �׼��� ����� �Ǵ³� (Target CID)
							lSkillTID == -1 ? NULL : &lSkillTID,							// skill tid
							eActionResult == AGPDCHAR_ACTION_RESULT_TYPE_NONE ? NULL : &eActionResult,
																							// action result type (miss, evade, critical hit, etc)
							pvFactor,														// target damage factor packet
							pstPos,															// character attack position
							ucCombo == -1 ? NULL : &ucCombo,								// Combo Information
							&bForce,														// is force attack
							eEffect == AGPDCHAR_ADDITIONAL_EFFECT_NONE ? NULL : &eEffect,	// Additional Effect. (2005.12.14. steeple)							
							cHitIndex == -1 ? NULL : &cHitIndex								// Hit Index. (2006.03.21. steeple)
							);
}

BOOL AgpmCharacter::HasPenalty(AgpdCharacter *pcsCharacter, eAgpmCharacterPenalty ePenalty)
{
	if (!pcsCharacter)
		return FALSE;
	
	if (AGPMCHAR_PENALTY_NONE >= ePenalty || ePenalty >= AGPMCHAR_PENALTY_MAX)
		return FALSE;
	
	INT32 lMurdererLevel = GetMurdererLevel(pcsCharacter);
	eAgpmCharacterPenalty eMaxPenalty = AGPMCHAR_PENALTY_NONE;
	
	switch (lMurdererLevel)
	{
		case AGPMCHAR_MURDERER_LEVEL3_POINT :
			eMaxPenalty = AGPMCHAR_PENALTY_FIRST_ATTACK;
			break;
		
		case AGPMCHAR_MURDERER_LEVEL2_POINT :
			eMaxPenalty = AGPMCHAR_PENALTY_EXP_LOSE;
			break;
		
		case AGPMCHAR_MURDERER_LEVEL1_POINT :
			eMaxPenalty = AGPMCHAR_PENALTY_CONVERT;
			break;
	}

	if (eMaxPenalty >= ePenalty)
	{
		EnumCallback(AGPMCHAR_CB_ID_BLOCK_BY_PENALTY, pcsCharacter, &ePenalty);
		return TRUE;
	}
	
	return FALSE;
}

BOOL AgpmCharacter::SetCallbackBlockByPenalty(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_BLOCK_BY_PENALTY, pfCallback, pClass);
}

// ������ (2005-08-16 ���� 11:56:29) : 
BOOL	AgpmCharacter::IsRideOn	(AgpdCharacter *pcsAgpdCharacter)	// Ż�Ϳ� Ÿ�� �ִ��� Ȯ��.
{
	if( NULL == pcsAgpdCharacter ) return FALSE;
	else return pcsAgpdCharacter->m_bRidable;
}

INT32	AgpmCharacter::GetVehicleTID(AgpdCharacter *pcsAgpdCharacter)
{
	ASSERT( NULL != pcsAgpdCharacter );
	// Ż����  Template ID �� ����.

	if( NULL == pcsAgpdCharacter)
		return 0;

	if( pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_RIDE_ARCADIA)
		return 1295;	//  ��ī��� �ϵ��ڵ� (�����ڰ� �����ٲ���)
	else if (0 == pcsAgpdCharacter->m_nRideCID) return 0;
	else
	{
		AgpdCharacter *pcsVehicle = this->GetCharacter( pcsAgpdCharacter->m_nRideCID );
		//ASSERT( NULL != pcsVehicle );

		if( pcsVehicle && pcsVehicle->m_pcsCharacterTemplate )
		{
			return pcsVehicle->m_pcsCharacterTemplate->m_lID;
		}
		else
		{
			// ������ �ȵŴµ�.. �ϴ� 0..
			return 0;
		}
	}
}

INT32	AgpmCharacter::GetRemovePoolCount()
{
	return m_csAdminCharacterRemove.m_csObject.m_lDataCount;
}

// 2005.12.02. steeple
BOOL	AgpmCharacter::AddGameBonusExp(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusExpRate += lValue;
	return TRUE;
}

// 2005.12.02. steeple
BOOL	AgpmCharacter::SubGameBonusExp(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusExpRate -= lValue;
	return TRUE;
}

// 2005.12.02. steeple
INT32	AgpmCharacter::GetGameBonusExp(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	return pcsCharacter->m_stGameBonusByCash.m_lBonusExpRate;
}

// 2005.12.02. steeple
BOOL	AgpmCharacter::AddGameBonusMoney(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusMoneyRate += lValue;
	return TRUE;
}

// 2005.12.02. steeple
BOOL	AgpmCharacter::SubGameBonusMoney(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusMoneyRate -= lValue;
	return TRUE;
}

// 2005.12.02. steeple
INT32	AgpmCharacter::GetGameBonusMoney(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	INT32 lBonusMoneyRate = 0;
	EnumCallback(AGPMCHAR_CB_ID_GET_BONUS_MONEY_RATE, pcsCharacter, &lBonusMoneyRate);

	return pcsCharacter->m_stGameBonusByCash.m_lBonusMoneyRate + lBonusMoneyRate;
}

// 2006.06.01. steeple
BOOL	AgpmCharacter::AddGameBonusDropRate(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusDropRate += lValue;
	return TRUE;
}

// 2008.07.11. iluvs
BOOL	AgpmCharacter::AddGameBonusDropRate2(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusDropRate2 += lValue;
	return TRUE;
}


// 2006.06.01. steeple
BOOL	AgpmCharacter::SubGameBonusDropRate(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusDropRate -= lValue;
	return TRUE;
}

// 2008.07.11. iluvs
BOOL	AgpmCharacter::SubGameBonusDropRate2(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusDropRate2 -= lValue;
	return TRUE;
}

// 2006.06.01. steeple
INT32	AgpmCharacter::GetGameBonusDropRate(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 2006.12.19. steeple
	INT32 lBonusDropRate = 0;
	EnumCallback(AGPMCHAR_CB_ID_GET_BONUS_DROP_RATE, pcsCharacter, &lBonusDropRate);

	return pcsCharacter->m_stGameBonusByCash.m_lBonusDropRate + lBonusDropRate;
}

// 2008.07.10. iluvs
INT32	AgpmCharacter::GetGameBonusDropRate2(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// 2006.12.19. steeple
	INT32 lBonusDropRate2 = 0;
	EnumCallback(AGPMCHAR_CB_ID_GET_BONUS_DROP_RATE2, pcsCharacter, &lBonusDropRate2);

	return pcsCharacter->m_stGameBonusByCash.m_lBonusDropRate2 + lBonusDropRate2;
}

BOOL AgpmCharacter::AddGameBonusCharismaRate(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusCharismaRate += lValue;
	return TRUE;
}

BOOL AgpmCharacter::SubGameBonusCharismaRate(AgpdCharacter* pcsCharacter, INT32 lValue)
{
	if(!pcsCharacter)
		return FALSE;

	pcsCharacter->m_stGameBonusByCash.m_lBonusCharismaRate -= lValue;
	return TRUE;
}

INT32 AgpmCharacter::GetGameBonusCharismaRate(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	INT32 lBonusCharismaRate = 0;
	EnumCallback(AGPMCHAR_CB_ID_GET_BONUS_CHARISMA_RATE, pcsCharacter, &lBonusCharismaRate);

	return pcsCharacter->m_stGameBonusByCash.m_lBonusCharismaRate + lBonusCharismaRate;
}

BOOL	AgpmCharacter::IsAllBlockStatus(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (IsReturnToLoginServerStatus(pcsCharacter))
		return TRUE;

	if (IsDisableCharacter(pcsCharacter))
		return TRUE;

	BOOL	bIsAllBlock	= FALSE;

	EnumCallback(AGPMCHAR_CB_ID_CHECK_ALL_BLOCK_STATUS, pcsCharacter, &bIsAllBlock);

	return	bIsAllBlock;
}

BOOL	AgpmCharacter::GetValidDestination( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , ApmMap::BLOCKINGTYPE eType , AgpdCharacter * pSkipCharacter )
{
	ASSERT( pStart	);
	ASSERT( pDest	);
	ASSERT( pValid	);

	#ifdef _DEBUG
	{
		static bool	_sbDontUseBlock	= false;
		if( _sbDontUseBlock )
		{
			eType = ApmMap::GHOST;
		}
	}
	#endif

	switch( eType )
	{
	case ApmMap::GROUND		:
		// ����ó��
		break;
	case ApmMap::SKY		:
	case ApmMap::UNDERGROUND:
	case ApmMap::GHOST		:
		// �ɸ��� ��ŷ ����
		if( pValid )
		{
			* pValid = * pDest;
		}
		return FALSE;
	}

	if( pSkipCharacter )
	{
		// ������ �ɸ��� ��ŷ ������ Ȯ��
		ApmMap::RegionTemplate	*pcsRegionTemplate	= m_pcsApmMap->GetTemplate(pSkipCharacter->m_nBindingRegionIndex);

		if( pcsRegionTemplate )
		{
			if( !pcsRegionTemplate->ti.stType.bCharacterBlock ) return FALSE;
		}
		else
		{
			return FALSE;
		}
	}

	AuPOS	posDest = * pDest;

	//  ������  .. �������ʿ䰡 ������,.
	// m_pcsApmMap->GetValidDestination( pStart , pDest , & posDest );

	if( pValid )
	{
		* pValid = * pDest;
	}

	INT32	nStartX , nEndX , nStartZ , nEndZ;
	{
		INT32	nAX , nAZ , nBX , nBZ;

		//ApWorldSector	* pSectorA	=	m_pcsApmMap->GetSector( pStart->x , pStart->z );
		//ApWorldSector	* pSectorB	=	m_pcsApmMap->GetSector( posDest.x , posDest.z );

		nAX = PosToSectorIndexX( pStart->x );
		nAZ = PosToSectorIndexZ( pStart->z );
		nBX = PosToSectorIndexX( posDest.x );
		nBZ = PosToSectorIndexZ( posDest.z );

		if( nAX == nBX )
		{
			// ���� ��迡 ���� ������ �ذ��ϱ� ���ؼ�
			// �� ���Ϳ����� üũ�ϴ� ��� �� ���͸� �� ���Խ�Ų��.
			nStartX	= nAX;
			nEndX	= nBX + 1;
		}
		else if ( nAX < nBX )
		{
			nStartX	= nAX;
			nEndX	= nBX;
		}
		else
		{
			nStartX	= nBX;
			nEndX	= nAX;
		}

		if( nAZ == nBZ )
		{
			// ���� ��迡 ���� ������ �ذ��ϱ� ���ؼ�
			// �� ���Ϳ����� üũ�ϴ� ��� �� ���͸� �� ���Խ�Ų��.
			nStartZ	= nAZ;
			nEndZ	= nBZ + 1;
		}
		else if( nAZ < nBZ )
		{
			nStartZ	= nAZ;
			nEndZ	= nBZ;
		}
		else
		{
			nStartZ	= nBZ;
			nEndZ	= nAZ;
		}
	}

	INT32	nSectorX , nSectorZ;

	ApWorldSector * pSector;

	#define SECTOR_TOTAL_COL_COUNT	( SECTOR_MAX_COLLISION_OBJECT_COUNT * 10 )
	vector< INT32 > vectorCharacter;

	for( nSectorZ = nStartZ ; nSectorZ <= nEndZ /* ���ԵǾ����*/ ; nSectorZ ++ )
	{
		for( nSectorX = nStartX ; nSectorX <= nEndX /* ���ԵǾ����*/ ; nSectorX ++ )
		{
			pSector	= m_pcsApmMap->GetSector( nSectorX , nSectorZ );

			if( pSector )
			{
				// 2007.01.20. steeple
				// ������� ���~
				//AuAutoLock csLock(pSector->m_Mutex);
				ApAutoReaderLock	csLock( pSector->m_RWLock );

				//  ���Ϳ� �ִ� ����� �� �˻���.
				ApWorldSector::Dimension * pDimension = pSector->GetDimension( 0 );
				if( pDimension )
				{
					ApWorldSector::IdPos *	pUser;

					pUser = pDimension->pUsers;
					while (pUser)
					{
						vectorCharacter.push_back( ( INT32 ) pUser->id2 );
						// ��� ���ϱ�...
						pUser = pUser->pNext;
					}

					pUser = pDimension->pNPCs;
					while (pUser)
					{
						vectorCharacter.push_back( ( INT32 ) pUser->id2 );
						// ��� ���ϱ�...
						pUser = pUser->pNext;
					}

					pUser = pDimension->pMonsters;
					while (pUser)
					{
						vectorCharacter.push_back( ( INT32 ) pUser->id2 );
						// ��� ���ϱ�...
						pUser = pUser->pNext;
					}
				}
			}
			///////////////
		}
	}

	if( !vectorCharacter.empty() )
	{
		/*
		FLOAT	fMaxDistance = AUPOS_DISTANCE_XZ( * pStart , posDest );

		FLOAT	fRadius = 50.0f; // ���� 1���� Ŀ��..

		AuPOS	vDirection;

		vDirection.x = posDest.x - pStart->x;
		vDirection.z = posDest.z - pStart->z;
		vDirection.y = 0.0f;

		FLOAT	fAngle = vDirection.z / vDirection.x;

		AuMath::V3DNormalize( &vDirection , &vDirection );

		FLOAT	fNearestDistance = fMaxDistance;
		AuPOS	vNearestJoint;

		vNearestJoint = posDest;

		for( vector< AgpdCharacter * >::iterator iTer = vectorCharacter.begin() ; 
			iTer != vectorCharacter.end() ; iTer ++ )
		{
			AuPOS	vJoint;
			AgpdCharacter * pCharacter = *iTer;

			vJoint.x = ( fAngle * pStart->x - pStart->z + pCharacter->m_stPos.x / fAngle + pCharacter->m_stPos.z ) /
						( fAngle + 1 / fAngle );
			vJoint.z = fAngle * ( vJoint.x - pStart->x ) + pStart->z;

			FLOAT fDistance = AUPOS_DISTANCE_XZ( * pStart , vJoint );

			if( fDistance > 0.0f && fDistance < fNearestDistance )
			{
				FLOAT fDistance2 = AUPOS_DISTANCE_XZ( vJoint , pCharacter->m_stPos );

				if( fDistance2 < fRadius )
				{
					// ������..
					fNearestDistance = fDistance;

					// ������ ������� �ʰ� ������ ����Ѵ� ( �ϴ� )
					fDistance -= fRadius;
					if( fDistance < 0.0f ) fDistance = 0.0f;

					vNearestJoint.x = pStart->x + vDirection.x * fDistance;
					vNearestJoint.z = pStart->z + vDirection.z * fDistance;
				}
			}
		}

		if( fNearestDistance < fMaxDistance )
		{
			// ����� ���� �ֵ�.
			pValid->x = vNearestJoint.x;
			pValid->z = vNearestJoint.z;
			//pValid->y = 0.0f;

			return TRUE;
		}
		else
		{
			return FALSE;
		}
		*/


		FLOAT	fRadius = 50.0f; // ���� 1���� Ŀ��..

		if( pSkipCharacter )
		{
			//fRadius = pSkipCharacter->m_fSiegeWarCollSphereRadius;
		}
		
		AuPOS vDirection;
		vDirection = *pDest - *pStart;

		float fMaxDistance = AUPOS_DISTANCE_XZ( *pStart  , *pDest  );
		float fNearestDistance = fMaxDistance;

		for( vector< INT32 >::iterator iTer = vectorCharacter.begin() ; 
			iTer != vectorCharacter.end() ; iTer ++ )
		{
			AgpdCharacter * pCharacter = GetCharacter(*iTer);

			// 2007.01.20. steeple
			// ������� ���~
			if(!pCharacter || !pCharacter->m_pcsCharacterTemplate)
				continue;

			if (pCharacter->IsPC())
				continue;
			if( pCharacter == pSkipCharacter )	continue;	// �ڱ�� ��ŵ
			if( pCharacter->IsDead() )			continue;	// �׾����� ��ŵ.
			if( !pCharacter->m_pcsCharacterTemplate->IsBlocking() ) continue; // ��ŷ ��Ұ� ������ ��ŵ

			// pSkipCharacter
			// pCharacter
			// * pStart
			// * pDest

			// pValid

			if( pCharacter->m_pcsCharacterTemplate->IsBoxCollision() )
			{
				// �ڽ� �ݸ���
				if( CollisionBox( pSkipCharacter , pCharacter , pStart , &vDirection , pValid , &fNearestDistance ) )
					break;
			}
			else
			{
				// �Ǹ��� �ݸ���
				if( CollisionCylinder( pSkipCharacter , pCharacter , pStart , &vDirection , pValid , &fNearestDistance ) )
					break;
			}
		}

		if( fNearestDistance < fMaxDistance )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	}
	else
	{
		// �ɸ��� ������Ʈ�� ������ üũ���� �ʴ´�..
		return FALSE;
	}
}

BOOL	AgpmCharacter::CollisionCylinder	( AgpdCharacter * pSelf , AgpdCharacter * pBlock , AuPOS * pStart , AuPOS * pDirection , AuPOS * pValid , FLOAT * pfDistance )
{
	AuPOS vJoint;
	AuPOS vPoint = pBlock->m_stPos;

	// ������⿡ �ִ��� ����.
	{
		AuPOS	vNewDir;
		vNewDir = vPoint - *pStart ;
		float fDotProduct;
		fDotProduct = ( vNewDir.x * pDirection->x + vNewDir.z * pDirection->z );
		if( fDotProduct <= 0 ) return FALSE;
	}

	float ftime;
	//float ftime2;
	{
		AuPOS vVectorA;
		AuPOS vVectorB;

		vVectorA = *pStart  - vPoint;
		vVectorB = *pDirection;

		float fdistance = 35.0f * 2;
		// float fdistance = pSelf->m_fSiegeWarCollSphereRadius + pCharacter->m_fSiegeWarCollSphereRadius;

		float fA2 = ( vVectorA.x * vVectorA.x + vVectorA.z * vVectorA.z );
		float fB2 = ( vVectorB.x * vVectorB.x + vVectorB.z * vVectorB.z );
		float fAB = ( vVectorA.x * vVectorB.x + vVectorA.z * vVectorB.z );

		float	fValueInTheSqrt;
		fValueInTheSqrt =	fAB * fAB - fB2 *( fA2 - fdistance * fdistance );

		if( fValueInTheSqrt < 0 )
			return FALSE;

		ftime = ( - fAB - sqrt( fValueInTheSqrt ) ) / fB2;
		//ftime2 = ( - fAB + sqrt( fValueInTheSqrt ) ) / fB2;
		// if( ftime < 0 ) continue;
		if( ftime > 1 )
			return FALSE;
	}

	if( ftime < 0 )
	{
		// �̹� ���Ǿ��ִ� ���´�.
		// ����ġ�� ����.
		*pfDistance = 0.0f;
		pValid->x = pStart->x;
		pValid->z = pStart->z;
		return TRUE;
	}

	vJoint.x = pStart->x + ftime * pDirection->x;
	vJoint.z = pStart->z + ftime * pDirection->z;

	float fDistanceFromStart;
	fDistanceFromStart = AUPOS_DISTANCE_XZ( *pStart  , vJoint );

	if( fDistanceFromStart < *pfDistance )
	{
		*pfDistance = fDistanceFromStart;
		pValid->x = vJoint.x;
		pValid->z = vJoint.z;
		return FALSE; // ������ TRUE�� Ȯ���϶��� �Ѵ�.
	}
	else
	{
		return FALSE;
	}
}

BOOL	AgpmCharacter::CollisionBox			( AgpdCharacter * pSelf , AgpdCharacter * pBlock , AuPOS * pStart , AuPOS * pDirection , AuPOS * pValid , FLOAT * pfDistance )
{
	if(!pSelf || !pBlock || !pStart || !pDirection || !pValid || !pfDistance)
		return FALSE;

	AuBoxCollision bcol;
	bcol.SetSize( pBlock->m_pcsCharacterTemplate->m_fSiegeWarCollBoxWidth , pBlock->m_pcsCharacterTemplate->m_fSiegeWarCollBoxHeight );
	bcol.SetOffset( pBlock->m_pcsCharacterTemplate->m_fSiegeWarCollObjOffsetX , pBlock->m_pcsCharacterTemplate->m_fSiegeWarCollObjOffsetZ );
	bcol.Transform( pBlock->m_stPos , pBlock->m_fTurnY );

	AuPOS ptTmp;
	BOOL bCol = bcol.CollBox( pStart , pDirection , pSelf->m_pcsCharacterTemplate->m_fSiegeWarCollSphereRadius , & ptTmp );
	if( bCol )
	{
		float fDistance = AUPOS_DISTANCE_XZ( *pStart , ptTmp );
		if( fDistance < *pfDistance )
		{
			*pfDistance = fDistance;
			*pValid = ptTmp;
		}
	}

	// Ȯ���϶��� TRUE ����
	return FALSE;
}


BOOL AgpmCharacter::IsDisableCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsUseCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_USE)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsRepairCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if (pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_REPAIR)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsArchlord(CHAR* szCharName)
{
	if(!szCharName || szCharName[0] == '\0')
		return FALSE;

	return EnumCallback(AGPMCHAR_CB_ID_IS_ARHCLORD, szCharName, NULL);
}

BOOL AgpmCharacter::IsArchlord(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	return IsArchlord(pcsCharacter->m_szID);
}

ApmMap::BLOCKINGTYPE	AgpmCharacter::GetBlockingType( AgpdCharacter* pcsCharacter )
{
	// ��ڳ� �����ΰ���
	// Ż�� ü������ �Ѱܹ���.

	if( IsRideOn( pcsCharacter ) )	// Ż�Ϳ� Ÿ�� �ִ��� Ȯ��.
	{
		INT32	nVehicleTID = GetVehicleTID(pcsCharacter);

		// ������
		// ������ ���ø� ������ ���ƴٴϴ°� ������ �־���ϴµ�..
		// �����ϱ� �ϴ��� ��ī��Ƹ� �ϵ��ڵ�.

		switch( nVehicleTID )
		{
		case 1295: // ��ī����� ���
			return ApmMap::SKY;
		default:
			return ApmMap::GROUND;
		}
	}
	else
	{
		return ApmMap::GROUND;
	}
}

BOOL AgpmCharacter::IsSameRace(AgpdCharacter *pcsPC1, AgpdCharacter *pcsPC2)
{
	if( !pcsPC1 || !pcsPC1 ) return FALSE;

	AgpdCharacterTemplate *	pcsTemplate1 = pcsPC1->m_pcsCharacterTemplate;
	AgpdCharacterTemplate *	pcsTemplate2 = pcsPC2->m_pcsCharacterTemplate;

	if (pcsPC1->m_bIsTrasform)
		pcsTemplate1 = GetCharacterTemplate(pcsPC1->m_lOriginalTID);

	if (pcsPC2->m_bIsTrasform)
		pcsTemplate2 = GetCharacterTemplate(pcsPC2->m_lOriginalTID);

	if( !pcsTemplate1 || !pcsTemplate2 ) return FALSE;

	if (m_pcsAgpmFactors->GetRace(&pcsTemplate1->m_csFactor) == m_pcsAgpmFactors->GetRace(&pcsTemplate2->m_csFactor))
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::IsSameRace(AgpdCharacter *pcsPC, INT32 lRace)
{
	if( !pcsPC ) return FALSE;
	AgpdCharacterTemplate *	pcsTemplate = pcsPC->m_pcsCharacterTemplate;

	if (pcsPC->m_bIsTrasform)
		pcsTemplate = GetCharacterTemplate(pcsPC->m_lOriginalTID);

	if( !pcsTemplate ) return FALSE;
	if (m_pcsAgpmFactors->GetRace(&pcsTemplate->m_csFactor) == lRace)
		return TRUE;

	return FALSE;
}


CHAR* AgpmCharacter::GetCharismaTitle(AgpdCharacter *pAgpdCharacter)
{
	if ( !m_pAgpdCharisma )
		return NULL;
	
	INT32 lPoint = GetCharismaPoint(pAgpdCharacter);
	for( INT32  i = 0; i < m_lTotalCharisma; ++i )
		if ( m_pAgpdCharisma[i].m_lPoint > lPoint )
			return m_pAgpdCharisma[i].m_szTitle;
	
	return NULL;
}

INT32	AgpmCharacter::GetCharismaStep(AgpdCharacter *pAgpdCharacter)
{
	if( !m_pAgpdCharisma )	return -1;

	INT32 lPoint = GetCharismaPoint( pAgpdCharacter );
	for( INT32 i = 0; i < m_lTotalCharisma; ++i )
		if ( m_pAgpdCharisma[i].m_lPoint > lPoint )
			return i;
	
	return -1;
		
}

// 2007.05.04. steeple
BOOL AgpmCharacter::IsInJail(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	ApmMap::RegionTemplate* pRegionTemplate = m_pcsApmMap->GetTemplate(
										m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z));
	if(pRegionTemplate && pRegionTemplate->ti.stType.bJail)
		return TRUE;

	return FALSE;
}

// 2009.03.04. iluvs
BOOL AgpmCharacter::IsInConsulationOffice(AgpdCharacter* pcsCharacter)
{
	if(pcsCharacter == NULL)
		return FALSE;

	// ĳ������ ���ε� ���� �ε����� ���ҿ� ���ٸ�
	if(pcsCharacter->m_nBindingRegionIndex == AGPMMAP_RESTRICT_CONSULT_OFFICE)
		return TRUE;

	return FALSE;
}

PVOID AgpmCharacter::MakePacketGameguardAuth( AgpdCharacter* pcsCharacter, PVOID pggData, INT16 ggDataLength, INT16* pPacketLength )
{
	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_NPROTECT_AUTH;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pPacketLength, AGPMCHARACTER_PACKET_TYPE,
									&cOperation,
									&pcsCharacter->m_lID,
									NULL,						// Character Template ID
									NULL,						// Game ID
									NULL,						// Character Status
									NULL,						// Move Packet
									NULL,						// Action Packet
									NULL,						// Factor Packet
									NULL,						// llMoney
									NULL,						// bank money
									NULL,						// cash
									NULL,						// character action status
									NULL,						// character criminal status
									NULL,						// attacker id (������� ������ �ʿ�)
									NULL,						// ���� �����Ǽ� �ʿ� �������� ����
									NULL,						// region index
									NULL,						// social action index
									NULL,						// special status
									NULL,						// is transform status
									NULL,						// skill initialization text
									NULL,						// face index
									NULL,						// hair index
									NULL,						// Option Flag
									NULL,						// bank size
									NULL,						// event status flag
									NULL,						// remained criminal status time
									NULL,						// remained murderer point time
									NULL,						// nick name
									pggData, &ggDataLength,		// gameguard auth data
									NULL
									);

	return pvPacket;
}

PVOID AgpmCharacter::MakePacketOnlineTime(AgpdCharacter* pcsCharacter, INT16* pnPacketLength)
{
	if(!pcsCharacter || !pnPacketLength)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_ONLINE_TIME;
	
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
											&cOperation,										// Operation
											&pcsCharacter->m_lID,								// Character ID
											NULL,												// Character Template ID
											NULL,												// Game ID
											NULL,												// Character Status
											NULL,												// Move Packet
											NULL,												// Action Packet
											NULL,												// Factor Packet
											NULL,												// llMoney
											NULL,												// bank money
											NULL,												// cash
											NULL,												// character action status
											NULL,												// character criminal status
											NULL,												// attacker id (������� ������ �ʿ�)
											NULL,												// ���� �����Ǽ� �ʿ� �������� ����
											NULL,												// region index
											NULL,												// social action index
											NULL,												// special status
											NULL,												// is transform status
											NULL,												// skill initialization text
											NULL,												// face index
											NULL,												// hair index
											NULL,												// Option Flag
											NULL,												// bank size
											NULL,												// event status flag
											&pcsCharacter->m_ulOnlineTime,						// remained criminal status time
											&pcsCharacter->m_lAddictStatus,						// remained murderer point time
											NULL,												// nick name
											NULL,												// gameguard
											NULL												// last killed time in battlesquare
											);
	return	pvPacket;
}

PVOID AgpmCharacter::MakePacketResurrectionByOther(INT16* pnPacketLength, INT32 lCID, CHAR* szName, INT32 lFlag)
{
	if(!pnPacketLength)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_RESURRECTION_BY_OTHER;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
											&cOperation,										// Operation
											&lCID,												// Character ID
											NULL,												// Character Template ID
											NULL,												// Game ID
											NULL,												// Character Status
											NULL,												// Move Packet
											NULL,												// Action Packet
											NULL,												// Factor Packet
											NULL,												// llMoney
											NULL,												// bank money
											NULL,												// cash
											NULL,												// character action status
											NULL,												// character criminal status
											NULL,												// attacker id (������� ������ �ʿ�)
											NULL,												// ���� �����Ǽ� �ʿ� �������� ����
											NULL,												// region index
											NULL,												// social action index
											NULL,												// special status
											NULL,												// is transform status
											NULL,												// skill initialization text
											NULL,												// face index
											NULL,												// hair index
											&lFlag,												// Option Flag
											NULL,												// bank size
											NULL,												// event status flag
											NULL,												// remained criminal status time
											NULL,												// remained murderer point time
											szName,												// nick name
											NULL,												// gameguard
											NULL												// last killed time in battlesquare
											);
	return pvPacket;
}

PVOID AgpmCharacter::MakePacketEventEffectID(INT16* pnPacketLength, INT32 lCID, INT32 lEventEffectID)
{
	if(!pnPacketLength)
		return FALSE;

	INT8 cOperation = AGPMCHAR_PACKET_OPERATION_EVENT_EFFECT_ID;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE, 
											&cOperation,										// Operation
											&lCID,												// Character ID
											NULL,												// Character Template ID
											NULL,												// Game ID
											NULL,												// Character Status
											NULL,												// Move Packet
											NULL,												// Action Packet
											NULL,												// Factor Packet
											NULL,												// llMoney
											NULL,												// bank money
											NULL,												// cash
											NULL,												// character action status
											NULL,												// character criminal status
											NULL,												// attacker id (������� ������ �ʿ�)
											NULL,												// ���� �����Ǽ� �ʿ� �������� ����
											NULL,												// region index
											NULL,												// social action index
											NULL,												// special status
											NULL,												// is transform status
											NULL,												// skill initialization text
											NULL,												// face index
											NULL,												// hair index
											&lEventEffectID,									// Option Flag
											NULL,												// bank size
											NULL,												// event status flag
											NULL,												// remained criminal status time
											NULL,												// remained murderer point time
											NULL,												// nick name
											NULL,												// gameguard
											NULL												// last killed time in battlesquare
											);
	return pvPacket;
}

void AgpmCharacter::InitFuncPtr( void )
{
	switch( g_eServiceArea )
	{
	case AP_SERVICE_AREA_KOREA :
		CheckNamePtr = &AgpmCharacter::CheckNameKr;
		break;

	case AP_SERVICE_AREA_CHINA :
		CheckNamePtr = &AgpmCharacter::CheckNameCn;
		break;

	case AP_SERVICE_AREA_TAIWAN :
		CheckNamePtr = &AgpmCharacter::CheckNameTw;
		break;

	case AP_SERVICE_AREA_GLOBAL :
		CheckNamePtr = &AgpmCharacter::CheckNameEn;
		break;

	case AP_SERVICE_AREA_JAPAN :
		CheckNamePtr = &AgpmCharacter::CheckNameJp;
		break;

	default:
		CheckNamePtr = &AgpmCharacter::CheckNameKr;
		break;
	}
}

AuRace	AgpmCharacter::GetCharacterRace( AgpdCharacter * pcsCharacter )
{
	INT32	lRace = m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	INT32	lClass = m_pcsAgpmFactors->GetClass(&pcsCharacter->m_csFactor);

	return AuRace( lRace , lClass );
}

CHAR* AgpmCharacter::GetCharacterRaceString( AgpdCharacter * pcsCharacter )
{
	CHAR *pszRace = NULL;
	AuRace lRace = GetCharacterRace(pcsCharacter);

	switch (lRace.detail.nRace)
	{	
	case AURACE_TYPE_HUMAN:
		pszRace = ServerStr().GetStr(STI_RACE_BATTLE_HUMAN);
		break;

	case AURACE_TYPE_ORC:
		pszRace = ServerStr().GetStr(STI_RACE_BATTLE_ORC);
		break;

	case AURACE_TYPE_MOONELF:
		pszRace = ServerStr().GetStr(STI_RACE_BATTLE_MOONELF);
		break;

	case AURACE_TYPE_DRAGONSCION:
		pszRace = ServerStr().GetStr(STI_DRAGONSCION);
		break;

	default:
		break;
	}

	return pszRace;
}

BOOL AgpmCharacter::IsDisableChattingCharacter(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISABLE_CHATTING)
		return TRUE;

	return FALSE;
}

BOOL AgpmCharacter::SetNationFlag(AgpdCharacter* pcsCharacter, INT32 nFlagType)
{
	if(!pcsCharacter)
		return FALSE;

	EventStatusFlag	unBitFlag;
	memset(&unBitFlag, 0, sizeof(unBitFlag));
	unBitFlag.BitFlag = pcsCharacter->m_unEventStatusFlag;

	unBitFlag.NationFlag = nFlagType;

	pcsCharacter->m_unEventStatusFlag = unBitFlag.BitFlag;

	return TRUE;
}

BOOL AgpmCharacter::CheckEnableActionCharacter(AgpdCharacter *pcsCharacter, eDisturbAction eDisturb)
{
	if(NULL == pcsCharacter)
		return FALSE;

	// Check Special_Status that can't control user's moving
	///////////////////////////////////////////////////////////////////////////////

	if(pcsCharacter->m_unDisturbAction & eDisturb)
		return FALSE;

	/////////////////////////////////////////////////////////////////////////////////

	return TRUE;
}

void
AgpmCharacter::SetMoveLockFrameTick(UINT32 ulFrameTick)
{
	m_ulMoveLockFrameTick = ulFrameTick;
}

UINT32
AgpmCharacter::GetMoveLockFrameTick()
{
	return m_ulMoveLockFrameTick;
}