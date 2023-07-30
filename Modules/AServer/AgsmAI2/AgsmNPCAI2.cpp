#include "AgsmAI2.h"
#include <math.h>

#define AGSMNPCAI2_FIXED_NPC_RANDE		100.0f

AgpdCharacter *AgsmAI2::GetTargetForGuardNPC(AgpdCharacter *pcsAgpdNPCCharacter, AuPOS csNPCPos, AgpdAI2ADChar *pcsAgpdAI2ADChar )
{
	PROFILE("AgsmAI2::GetTargetPC");

	AgpdAI2ADChar			*pcsTempAI2ADChar;

	INT_PTR				alCID[AGSMAI2_MAX_PROCESS_TARGET*2];
	INT32				lCharListCount;
	INT32				lNumCID;
	//INT32				lCounter;
	INT32				lMaxDamage;
	float				fVisibility;

	lMaxDamage = 0;
	fVisibility = 2000.0f;

	AgpdCharacter	*pcsTargetCharacter	= NULL;

	//�ֺ��� MOB,PC�� �ֳ�����.
	lCharListCount = m_pcsApmMap->GetCharList(pcsAgpdNPCCharacter->m_nDimension, APMMAP_CHAR_TYPE_PC, csNPCPos, fVisibility, alCID, AGSMAI2_MAX_PROCESS_TARGET);
	lNumCID = m_pcsApmMap->GetCharList( pcsAgpdNPCCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, csNPCPos, fVisibility, &alCID[lCharListCount], AGSMAI2_MAX_PROCESS_TARGET);

	lNumCID += lCharListCount;

	//��(����) ���� ������ �ִ°��.
	if( lNumCID > 0 )
	{
		AgpdCharacter	*pcsAgroTargetCharacter = NULL;

		float			fAgroDistance = 0.0f;
		INT32			lDamage;

		for (int i = 0; i < lNumCID; ++i)
		{
			AgpdCharacter	*pcsListCharacter	= (AgpdCharacter *) (alCID[i]);

			if(!pcsListCharacter)
				continue;

			//NPC���� �ڽ��� ����.
			if( pcsListCharacter->m_lID == pcsAgpdNPCCharacter->m_lID )
				continue;

			//��~ ���� ���Ͽ��� PC�� �����Ѵ�.
			if( m_pcsAgpmCharacter->IsPC( pcsListCharacter ) )
			{
				if (AGPMCHAR_MURDERER_LEVEL1_POINT > m_pcsAgpmCharacter->GetMurdererLevel(pcsListCharacter))
					continue;
			}

			//GM�� ���� �����Ѵ�.
			if( m_pcsAgpmCharacter->IsGM( pcsListCharacter ) )
				continue;

			//���� ��� �����Ѵ�.
			if( pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
				continue;

			//�ڸ��� ��Ű�� ���� NPC�� ���.
			if(	pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
			{
				float				fTempX, fTempZ, fTempDistance;

				fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
				//fTempX = pcsListCharacter->m_stPos.x - pcsAgpdNPCCharacter->m_stPos.x;
				//fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdNPCCharacter->m_stPos.z;
				fTempX = pcsListCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
				fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

				//���� �������� �ۿ� �ִ� ���̶�� �����Ѵ�.
				if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
				{
					continue;
				}
			}

			pcsTempAI2ADChar = m_pcsAgpmAI2->GetCharacterData( pcsListCharacter );

			if (pcsListCharacter->m_Mutex.WLock())
			{
				//���� �������� �ִ°�?
				lDamage = m_pcsAgsmCharacter->GetAgroFromEntry( pcsAgpdNPCCharacter, pcsListCharacter->m_lID );

				pcsListCharacter->m_Mutex.Release();

				if(	lDamage != 0 )
				{
					//����Ʈ�� �ִ´�.
					if( lDamage < lMaxDamage )
					{
						pcsTargetCharacter = pcsListCharacter;
						lMaxDamage = lDamage;
					}
				}
				//������� ������ ���븮��Ʈ�� �߰��Ѵ�.
				else
				{
					//NPC���� ���� ������ ������ �����Ѵ�.(������ NPC������ �����Ŷ� ���������?^^)
	//				if( m_pcsAgpmFactors->GetLevel( &pcsAgpdNPCCharacter->m_csFactor ) >= m_pcsAgpmFactors->GetLevel( &pcsListCharacter->m_csFactor ) )
					//������ �����Ѵ�.
					{
						float			fTempX, fTempZ;

						fTempX = pcsListCharacter->m_stPos.x - pcsAgpdNPCCharacter->m_stPos.x;
						fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdNPCCharacter->m_stPos.z;

						//���� �ƹ��� Ÿ������ �������� ���� ���.
						if( pcsAgroTargetCharacter == NULL )
						{
							pcsAgroTargetCharacter = pcsListCharacter;
							fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
						}
						//������ �̹� Ÿ������ ������ ���.
						else
						{
							if( fAgroDistance > fTempX*fTempX + fTempZ*fTempZ )
							{
								pcsAgroTargetCharacter = pcsListCharacter;
								fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
							}
						}
					}
				}
			}
		}

		if( (pcsTargetCharacter == NULL) && (pcsAgroTargetCharacter != NULL) )
		{
			pcsTargetCharacter = pcsAgroTargetCharacter;
		}
	}

	//���� ����ġ�� ����PC�� ID�� �����Ѵ�!
	return pcsTargetCharacter;
}

BOOL AgsmAI2::ProcessGuardNPCAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	PROFILE("AgsmAI2::ProcessAI");

	AgpdCharacter	*pcsAgpdNPCCharacter;
	AgpdCharacter	*pcsAgpdTargetCharacter;
	AuPOS			csNPCPos;

	BOOL			bResult;
	INT32			lNPCID, lAggressivePoint;
	float			fTempX, fTempZ, fTempDistance;

	pcsAgpdNPCCharacter = NULL;
	pcsAgpdTargetCharacter = NULL;
	bResult = FALSE;

	if( !pcsAgpdAI2ADChar || !pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
		return FALSE;

	pcsAgpdNPCCharacter	= pcsAgpdCharacter;

	sprintf_s( pcsAgpdCharacter->m_szSkillInit, "NPCAI" );

	// ���� �����ΰ��� ������ �ϸ� �ȵȴ�. ����...
	// 2004.08.25. steeple
	//if (pcsAgpdNPCCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsAgpdNPCCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return TRUE;

	csNPCPos = pcsAgpdCharacter->m_stPos;
	lNPCID = pcsAgpdCharacter->m_lID;
	lAggressivePoint = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_lAgressivePoint;

	pcsAgpdTargetCharacter = GetTargetForGuardNPC(pcsAgpdCharacter, csNPCPos, pcsAgpdAI2ADChar );

	//Ÿ���� ������.
	if( pcsAgpdTargetCharacter )
	{
		// ��ȯ���� �������� �ʴ´�.
		if (m_pcsAgpmCharacter->IsStatusSummoner(pcsAgpdTargetCharacter))
			return TRUE;

		// ũ���Ĵ� �������� �ʴ´�.
		if (m_pcsAgpmCharacter->IsCreature(pcsAgpdTargetCharacter))
			return TRUE;

		if (m_pcsAgpmCharacter->IsMonster(pcsAgpdTargetCharacter))
			return TRUE;

		//�þ߿� Ÿ���� ������ �翬�� �����Ѵ�. ^^;
		PROFILE("AgsmAI2::AttackTarget");

		BOOL				bInRange;
		BOOL				bPathFind;

		bInRange =  FALSE;
		bPathFind = FALSE;

		bInRange = m_pcsAgpmFactors->IsInRange(&pcsAgpdNPCCharacter->m_stPos, &pcsAgpdTargetCharacter->m_stPos, &pcsAgpdNPCCharacter->m_csFactor, &pcsAgpdTargetCharacter->m_csFactor);

		//���� ���� ���ΰ�?
		if( bInRange )
		{
			//�����ص� �Ǹ� �����Ѵ�.
			if( lClockCount > pcsAgpdAI2ADChar->m_ulLastAttackTime + pcsAgpdAI2ADChar->m_lAttackInterval )
			{
				if (pcsAgpdTargetCharacter->m_Mutex.WLock())
				{
					m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsAgpdNPCCharacter, (ApBase *)pcsAgpdTargetCharacter, NULL);
					pcsAgpdTargetCharacter->m_Mutex.Release();
				}

				pcsAgpdAI2ADChar->m_ulLastAttackTime = lClockCount;
			}
			else
			{
				//���� �̵����̸� �����.
				if (pcsAgpdNPCCharacter->m_bMove)
					m_pcsAgpmCharacter->StopCharacter(pcsAgpdNPCCharacter, NULL);
			}
		}
		//���ݹ��� ���ΰ�?
		else
		{
			bPathFind = TRUE;
		}

		if( bPathFind )
		{
			float				fTempX, fTempZ;

			fTempX = pcsAgpdTargetCharacter->m_stPos.x - pcsAgpdNPCCharacter->m_stPos.x;
			fTempZ = pcsAgpdTargetCharacter->m_stPos.z - pcsAgpdNPCCharacter->m_stPos.z;

			//fDistance = 500.0f;

			//if( fTempX*fTempX + fTempZ*fTempZ <= fDistance*fDistance )
			{
				pcsAgpdAI2ADChar->m_csTargetPos.x = pcsAgpdTargetCharacter->m_stPos.x;
				pcsAgpdAI2ADChar->m_csTargetPos.z = pcsAgpdTargetCharacter->m_stPos.z;

				//�������� ���� ã�Ƶд�.
				bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdCharacter->m_stPos.x,
														pcsAgpdCharacter->m_stPos.z,
														pcsAgpdAI2ADChar->m_csTargetPos.x,
														pcsAgpdAI2ADChar->m_csTargetPos.z,
														pcsAgpdAI2ADChar->m_pclPath );

				//
				CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

				pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

				if( pcsNode )
				{
					AuPOS			csPos;

					csPos.x = pcsNode->data->m_fX;
					csPos.y = 0.0f;
					csPos.z = pcsNode->data->m_fY;

					m_pcsAgpmCharacter->MoveCharacter( pcsAgpdCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
				}
			}
		}
	}
	//Ÿ���� ������?
	else
	{
		if( lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime )
		{
			//�ƹ��Ͼ���.
		}
		else
		{
			//�̵����� �ƴѵ� ���� �н� ����Ʈ�� ������ ���� ã�� 
			if( !pcsAgpdCharacter->m_bMove )
			{
				CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

				pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

				if( pcsNode )
				{
					AuPOS			csPos;

					csPos.x = pcsNode->data->m_fX;
					csPos.y = 0.0f;
					csPos.z = pcsNode->data->m_fY;

					//NPC������ ���� ������ �ڴ�. ���� �ȴ¾ִϰ� ����. -_-;
					m_pcsAgpmCharacter->MoveCharacter( pcsAgpdNPCCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
				}
				//���� ã�� NextWanderingTime�� �����Ѵ�.
				else
				{
					//�켱 ������ ���� NPC����, ��Ʈ�������� ����.
					if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
					{
						fTempX = pcsAgpdAI2ADChar->m_csSpawnBasePos.x - pcsAgpdNPCCharacter->m_stPos.x;
						fTempZ = pcsAgpdAI2ADChar->m_csSpawnBasePos.z - pcsAgpdNPCCharacter->m_stPos.z;
						fTempDistance = AGSMNPCAI2_FIXED_NPC_RANDE;

						//���� �ڸ����� 1�����̻� ����ִٸ� ���� �ڸ��� ���ư���.
						if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
						{
							//Ÿ���� ���� ��������Ʈ�� �������ش�.
							pcsAgpdAI2ADChar->m_csTargetPos = pcsAgpdAI2ADChar->m_csSpawnBasePos;

							//�ٷ� �����ڸ��� ���ư���.
							pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount;

							bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdNPCCharacter->m_stPos.x,
																	pcsAgpdNPCCharacter->m_stPos.z,
																	pcsAgpdAI2ADChar->m_csTargetPos.x,
																	pcsAgpdAI2ADChar->m_csTargetPos.z,
																	pcsAgpdAI2ADChar->m_pclPath );
						}
						//�ƴ϶�� Ư���� ������ ���� ����.
						else
						{
							;//�� �� ����.
						}
					}
					else //if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_PATROL_NPC )
					{
						PROFILE("AgsmAI2::GetNewTargetPosition");

						fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
						fTempX = pcsAgpdNPCCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
						fTempZ = pcsAgpdNPCCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

						//���� ���������ȿ� �������� �ʴ´ٸ� ���� ��ġ�� ���ο� ��������Ʈ�� ��´�.
						if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
						{
							pcsAgpdAI2ADChar->m_csSpawnBasePos = pcsAgpdCharacter->m_stPos;
						}

						if( GetTargetPosFromSpawn(pcsAgpdNPCCharacter, &pcsAgpdNPCCharacter->m_stPos, &pcsAgpdAI2ADChar->m_csTargetPos, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius ) )
						{
							bool			bResult;
							INT32			lRandTime;

							bResult = false;
							lRandTime = m_csRand.randInt()%15000;

							//�⺻ 15�ʿ� 0~15�ʻ����� ������ �ð��� ������ �ٿ� �����δ�.
							pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount + 15000 + lRandTime;

							//���ο� �������� ���õǾ����� ���� ã�ƺ�����~
							bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdNPCCharacter->m_stPos.x,
																	pcsAgpdNPCCharacter->m_stPos.z,
																	pcsAgpdAI2ADChar->m_csTargetPos.x,
																	pcsAgpdAI2ADChar->m_csTargetPos.z,
																	pcsAgpdAI2ADChar->m_pclPath );

							if( bResult )
							{
								//AgpmCharacter::OnIdle()���� CallBackȣ��! GetNextNode�� ȣ���ϰ� �̵��Ѵ�.
							}
							else
							{
								//�̰� ���� ���ٴ� �̾߱��ε�.... �׳� �����ؾ��� ��.... ��.
							}
						}
					}
				}
			}
		}
	}

	return bResult;
}