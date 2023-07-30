// AgsmAI.cpp: implementation of the AgsmAI class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmAI.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmAI::AgsmAI()
{
	SetModuleName("AgsmAI");
}

AgsmAI::~AgsmAI()
{

}

BOOL			AgsmAI::OnAddModule()
{
	// Parent Module�� ��������
	m_pcsApmMap			= (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmFactors	= (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsApmObject		= (ApmObject *) GetModule("ApmObject");
	m_pcsAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem		= (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmAI			= (AgpmAI *) GetModule("AgpmAI");
	m_pcsApmEventManager= (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmEventSpawn	= (AgpmEventSpawn *) GetModule("AgpmEventSpawn");
	m_pcsAgsmEventSpawn	= (AgsmEventSpawn *) GetModule("AgsmEventSpawn");
	m_pcsAgsmCharacter	= (AgsmCharacter *) GetModule("AgsmCharacter");

	// Character���� Public AI ������ �ϴ� OK
	if (!m_pcsAgpmCharacter && !m_pcsAgpmAI && !m_pcsAgsmCharacter)
		return FALSE;

	// Update Character Callback ����ϰ�
	if (!m_pcsAgpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;

	if (m_pcsAgsmEventSpawn && !m_pcsAgsmEventSpawn->SetCallbackSpawn(CBSpawnCharacter, this))
		return FALSE;

	if (m_pcsAgsmCharacter && !m_pcsAgsmCharacter->SetCallbackSendCharacterAllServerInfo(CBSendCharacterAllInfo, this))
		return FALSE;

	return TRUE;
}	

BOOL			AgsmAI::CBUpdateCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	PROFILE("AgsmAI::CBUpdateCharacter");

	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pvData;
	AgsmAI *			pThis = (AgsmAI *) pvClass;
	AgpdAIADChar *		pstAIADChar = pThis->m_pcsAgpmAI->GetCharacterData(pcsCharacter);
	UINT32				ulClockCount = (UINT32) pvCustData;

	// AI�� ����ϴ� ���� �ƴϸ�, return
	if (!pstAIADChar->m_bUseAI)
		return TRUE;

	// Interval��ŭ ����������, return
	if (pstAIADChar->m_ulPrevProcessTime + pstAIADChar->m_stAI.m_ulProcessInterval > ulClockCount)
		return TRUE;

	//Action ������ ������ üũ�ؾ��ϴ� AI(���ݵ� ��Ÿ ó���� ����)
	pThis->PreProcessAI( pcsCharacter, pstAIADChar, ulClockCount );

	pThis->ProcessAI(pcsCharacter, pstAIADChar, ulClockCount);

	pstAIADChar->m_ulPrevProcessTime = ulClockCount;

	return TRUE;
}

BOOL			AgsmAI::CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pvCustData;
	AgsmAI *			pThis = (AgsmAI *) pvClass;

	ASSERT(pcsCharacter && pThis);

	pThis->m_pcsAgpmAI->GetCharacterData(pcsCharacter)->m_bUseAI = TRUE;

	return TRUE;
}

BOOL			AgsmAI::CBSendCharacterAllInfo(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	if (!pvClass || !pvData || !pvCustData)
		return FALSE;

	AgsmAI *			pThis			= (AgsmAI *)		pvClass;
	AgpdCharacter *		pcsCharacter	= (AgpdCharacter *)	pvData;
	UINT32 *			pulBuffer		= (UINT32 *)		pvCustData;

	if (!pulBuffer[1])	// ���� ������ ����� ��츸 ��Ŷ�� ������.
	{
		// AI�� ����ϴ� ������ üũ�ϰ� ����ϴ� ���̶�� ����Ÿ�� ������.
		AgpdAIADChar *		pcsAgpdAIADChar		= pThis->m_pcsAgpmAI->GetCharacterData(pcsCharacter);
		if (pcsAgpdAIADChar)
		{
			if (pcsAgpdAIADChar->m_bUseAI)
			{
				return pThis->SendPacketAIADChar(pcsCharacter, pulBuffer[0]);
			}
		}
	}

	return TRUE;
}

VOID			AgsmAI::PreProcessAI(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount)
{
	BOOL			bAgressiveMob;

	//����� �ð���ŭ Slow,Fast Agro�� �ٿ��ְ� �ʿ信 ���� History���� �����ش�.

	bAgressiveMob = TRUE;
	//��׷��ú� ������ ����.
	if( bAgressiveMob )
	{
		//�ֺ��� PC���� ����.

		//�þ߿� �ִ³�鸸 �߷�����.

		//����ġ�� �ش�.
	}

	return;
}

VOID			AgsmAI::ProcessAI(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount)
{
	if (!pcsCharacter || !pstAIADChar)
		return;

	if (!SortActions(pcsCharacter, pstAIADChar, ulClockCount))
		return;

	if (!ProcessActions(pcsCharacter, pstAIADChar, ulClockCount))
		return;
}

INT32 __cdecl	QSortActionCompare(const void *pvData1, const void *pvData2)
{
	AgpdAIActionResult *		pstResult1 = (AgpdAIActionResult *) pvData1;
	AgpdAIActionResult *		pstResult2 = (AgpdAIActionResult *) pvData2;

	return (pstResult2->m_lActionRate - pstResult1->m_lActionRate);
}

int				UseItemTIDCompare( const void *arg1, const void *arg2 )
{
	int				iResult;

	if( *((int *)arg1) > *((int *)arg2) )
	{
		iResult = 1;
	}
	else if( *((int *)arg2) > *((int *)arg1) )
	{
		iResult = -1;
	}
	else
	{
		iResult = 0;
	}

	return iResult;
}

BOOL			AgsmAI::SortActions(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount)
{
	PROFILE("AgsmAI::SortActions");

	AgpdAIFactor *				pstAIFactor;
	AuNode< AgpdAIFactor > *	pNode;
	INT32						lIndex;

	for (lIndex = 0; lIndex < AGPDAI_ACTION_MAX_TYPE; ++lIndex)
	{
		pstAIADChar->m_astResult[lIndex].m_eActionType = (AgpdAIActionType) lIndex;
		pstAIADChar->m_astResult[lIndex].m_lActionRate = 0;
		pstAIADChar->m_astResult[lIndex].m_pstActions = NULL;
	}

	pNode = pstAIADChar->m_stAI.m_listAIFactors.GetHeadNode();
	while (pNode)
	{
		pstAIFactor	= &pNode->GetData();

		if (IsFactorApplicable(pcsCharacter, pstAIFactor, ulClockCount))
			AddActionToResult(pstAIADChar, pstAIFactor);

		pstAIADChar->m_stAI.m_listAIFactors.GetNext(pNode);
	}

	QSort(&pstAIADChar->m_astResult[0], AGPDAI_ACTION_MAX_TYPE, sizeof(AgpdAIActionResult), QSortActionCompare);

	return TRUE;
}

BOOL			AgsmAI::ProcessActions(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount)
{
	PROFILE("AgsmAI::ProcessActions");

	AgpdAIAction *				pstAction;

	pstAction = pstAIADChar->m_astResult[0].m_pstActions;
	if (!pstAction)
		return FALSE;

	if (ProcessAction(pcsCharacter, pstAIADChar->m_astResult[0].m_eActionType, pstAction, ulClockCount))
		return TRUE;

	pstAIADChar->m_astResult[0].m_pstActions = pstAction->m_pstNext;
	pstAIADChar->m_astResult[0].m_lActionRate -= pstAction->m_lActionRate;

	QSort(&pstAIADChar->m_astResult[0], AGPDAI_ACTION_MAX_TYPE, sizeof(AgpdAIActionResult), QSortActionCompare);

	if (ProcessActions(pcsCharacter, pstAIADChar, ulClockCount))
		return TRUE;

	return FALSE;
}

BOOL			AgsmAI::ProcessAction(AgpdCharacter *pcsCharacter, AgpdAIActionType eActionType, AgpdAIAction *pstAction, UINT32 ulClockCount)
{
	AgpdAIADChar *		pstAIADChar = m_pcsAgpmAI->GetCharacterData(pcsCharacter);
	AgpdAI *			pstAI = &pstAIADChar->m_stAI;
	AgpdCharacter *		apcsTargetCharacter[AGSMAI_MAX_PROCESS_TARGET];
	AuPOS				stTargetPos;
	INT32				lNumTarget;
	INT32				lIndex;

	switch (eActionType)
	{
	case AGPDAI_ACTION_TYPE_ATTACK:
		{
			lNumTarget = GetTargetBase(pcsCharacter, pstAI, pstAction, ulClockCount, (ApBase **) apcsTargetCharacter);
			for (lIndex = 0; lIndex < lNumTarget; ++lIndex)
			{
				if (apcsTargetCharacter[lIndex]->m_eType == APBASE_TYPE_CHARACTER)
				{
					if (m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsCharacter, apcsTargetCharacter[lIndex], NULL))
						return TRUE;
				}
			}
		}

		break;

	case AGPDAI_ACTION_TYPE_WANDER:
		{
			if (pcsCharacter->m_bMove)
				return TRUE;

			ApdEvent			*pcsEvent;

			pcsEvent = m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter)->m_pstEvent;

			if( pcsEvent == NULL )
			{
				pstAction->m_stTarget.m_stTargetPos = pcsCharacter->m_stPos;
			}
			else
			{
				m_pcsApmEventManager->GetRandomPos(pcsEvent, &pstAction->m_stTarget.m_stTargetPos);
			}

			pstAction->m_stTarget.m_fRadius = pstAI->m_fVisibility;

			if (m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &pstAction->m_stTarget.m_stTargetPos, FALSE))
				return TRUE;
		}

		break;

	case AGPDAI_ACTION_TYPE_HOLD:
		{
			return TRUE;
		}

		break;

	case AGPDAI_ACTION_TYPE_FOLLOW:
		{
			lNumTarget = GetTargetBase(pcsCharacter, pstAI, pstAction, ulClockCount, (ApBase **) apcsTargetCharacter);
			for (lIndex = 0; lIndex < lNumTarget; ++lIndex)
			{
				if (apcsTargetCharacter[lIndex]->m_eType == APBASE_TYPE_CHARACTER)
				{
					if (!m_pcsAgpmFactors->IsInRange(&pcsCharacter->m_stPos, &apcsTargetCharacter[lIndex]->m_stPos, &pcsCharacter->m_csFactor, &pcsCharacter->m_csFactor, &stTargetPos))
					{
						if (m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, FALSE))
							return TRUE;
					}
					else
						return TRUE;
				}
			}
		}

		break;

	case AGPDAI_ACTION_TYPE_MOVE_AWAY:
		{
			FLOAT	fDeltaDegree;
			FLOAT	fDegreeY;

			lNumTarget = GetTargetBase(pcsCharacter, pstAI, pstAction, ulClockCount, (ApBase **) apcsTargetCharacter);
			for (lIndex = 0; lIndex < lNumTarget; ++lIndex)
			{
				if (apcsTargetCharacter[lIndex]->m_eType == APBASE_TYPE_CHARACTER)
				{
					AuMath::V3DScale(&stTargetPos, &pcsCharacter->m_stMatrix.at, pstAI->m_fVisibility);
					if (m_pcsApmMap && m_pcsApmMap->CheckBlockingInfo(stTargetPos))
					{
						for (fDeltaDegree = (FLOAT) (AUMATH_PI / 10.0f); fDeltaDegree < AUMATH_PI; fDeltaDegree += (FLOAT) (AUMATH_PI / 10.0f))
						{
							fDegreeY = pcsCharacter->m_fTurnY + fDeltaDegree;
							stTargetPos.x = pstAI->m_fVisibility * (FLOAT) sin(fDegreeY);
							stTargetPos.y = pcsCharacter->m_stPos.y;
							stTargetPos.z = pstAI->m_fVisibility * (FLOAT) cos(fDegreeY);
							if (m_pcsApmMap->CheckBlockingInfo(stTargetPos))
								break;

							fDegreeY = pcsCharacter->m_fTurnY - fDeltaDegree;
							stTargetPos.x = pstAI->m_fVisibility * (FLOAT) sin(fDegreeY);
							stTargetPos.y = pcsCharacter->m_stPos.y;
							stTargetPos.z = pstAI->m_fVisibility * (FLOAT) cos(fDegreeY);
							if (m_pcsApmMap->CheckBlockingInfo(stTargetPos))
								break;
						}

						if (fDeltaDegree >= AUMATH_PI)
							return FALSE;
					}

					if (m_pcsAgpmCharacter->MoveCharacter(pcsCharacter, &stTargetPos, FALSE))
						return TRUE;
				}
			}
		}

		break;

	case AGPDAI_ACTION_TYPE_GUARD:
		{
		}

		break;

	case AGPDAI_ACTION_TYPE_ASSIST:
		{
		}

		break;

	case AGPDAI_ACTION_TYPE_PICKUP:
		{
		}

		break;

	case AGPDAI_ACTION_TYPE_USE_ITEM:
		{
			if( pcsCharacter != NULL )
			{
				AgpdAIADChar		*pstAIADChar;
				AgpdGrid			*pcsGrid;
				AgpdGridItem		*pcsAgpdGridItem;

				pcsGrid = m_pcsAgpmItem->GetInventory( pcsCharacter );
				pstAIADChar = m_pcsAgpmAI->GetCharacterData( pcsCharacter );

				if( (pcsGrid != NULL) && (pstAIADChar != NULL) )
				{
					//HP������ ���Ѵ�.
					if( pstAIADChar->m_stAI.m_csAIUseItem.m_lHP )
					{
						INT32			lTotalRate;
						INT32			lCurrentItemCount;
						INT32			lItemTID;
						INT32			iaItemTID[AGPDAI_MAX_USABLE_ITEM_COUNT];
						INT32			iaItemRate[AGPDAI_MAX_USABLE_ITEM_COUNT];
						int				iCount;

						lTotalRate = 0;
						lCurrentItemCount = 0;
						memset( iaItemTID, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_ITEM_COUNT );
						memset( iaItemRate, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_ITEM_COUNT );

						//��밡���� �����۸� ��󳽴�.
						for( iCount=0; iCount<AGPDAI_MAX_USABLE_ITEM_COUNT; iCount++ )
						{
							 lItemTID = pstAIADChar->m_stAI.m_csAIUseItem.m_alTID[iCount];

							 if( lItemTID != 0 )
							 {
								pcsAgpdGridItem = m_pcsAgpmGrid->GetItemByTemplate( pcsGrid, AGPDGRID_ITEM_TYPE_ITEM, iaItemTID[iCount] );

								if( pcsAgpdGridItem != NULL )
								{
									iaItemTID[lCurrentItemCount] = lItemTID;
									iaItemRate[lCurrentItemCount] = pstAIADChar->m_stAI.m_csAIUseItem.m_alRate[iCount];
									lTotalRate += pstAIADChar->m_stAI.m_csAIUseItem.m_alRate[iCount];
								}
							 }
						}

						//��밡���� �������� ��������� � �������� ������� ����.
						if( lTotalRate != 0 )
						{
							INT32			lCurrentRate;
							INT32			lTempRate;

							lCurrentRate = rand()%lTotalRate;
							lTempRate = 0;

							for( iCount=0; iCount<AGPDAI_MAX_USABLE_ITEM_COUNT; iCount++ )
							{
								if( iaItemTID[iCount] == 0 )
									break;

								lTempRate += iaItemRate[iCount];

								if( lCurrentRate < lTempRate )
								{
									break;
								}
							}

							//�������� �������!!
							pcsAgpdGridItem = m_pcsAgpmGrid->GetItemByTemplate( pcsGrid, AGPDGRID_ITEM_TYPE_ITEM, iaItemTID[iCount] );

							if( pcsAgpdGridItem != NULL )
							{
								if( m_pcsAgpmItem->UseItem( pcsAgpdGridItem->m_lItemID ) )
								{
									return TRUE;
								}
							}
						}
					}
				}
			}
		}

		break;

	case AGPDAI_ACTION_TYPE_USE_SKILL:
		{
			if( pcsCharacter != NULL )
			{
				AgpdAIADChar		*pstAIADChar;

				pstAIADChar = m_pcsAgpmAI->GetCharacterData( pcsCharacter );
			}
		}

		break;

	case AGPDAI_ACTION_TYPE_SCREAM:
		{
			if( pcsCharacter != NULL )
			{
				bool				bResult;
				AgpdAIADChar		*pstAIADChar;
				pstAIADChar = m_pcsAgpmAI->GetCharacterData( pcsCharacter );

				bResult = false;

				//Fear Scream L1 �ۿ��� �������� ����.
				if( pstAIADChar->m_stAI.m_csAIScream.m_lFearLV1Agro )
				{
					//�ֺ� ���͸� ����. ��ũ���� ���ĺ��ƿ�~
					if( ProcessHelpScreamL1( pcsCharacter ) )
						bResult = true;
				}

				//Fear Scream L2 �ۿ��� �������� ����.
				if( pstAIADChar->m_stAI.m_csAIScream.m_lFearLV2HP )
				{
					if( ProcessHelpScreamL2( pcsCharacter ) )
						bResult = true;
				}

				if( bResult )
					return bResult;
			}
		}
		break;
	}

	return FALSE;
}

INT32 __cdecl	QSortTargetCompare(const void *pvData1, const void *pvData2)
{
	AgsdAITargetBaseData *		pstResult1 = (AgsdAITargetBaseData *) pvData1;
	AgsdAITargetBaseData *		pstResult2 = (AgsdAITargetBaseData *) pvData2;

	return (pstResult2->m_lCalcWeight - pstResult1->m_lCalcWeight);
}

INT32			AgsmAI::GetTargetBase(AgpdCharacter *pcsCharacter, AgpdAI *pstAI, AgpdAIAction *pstAction, UINT32 ulClockCount, ApBase *apcsTarget[])
{
	PROFILE("AgsmAI::GetTargetBase()");

	AgsdCharacter *		pstAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	ApBaseType			eBaseType = pstAction->m_stTarget.m_csTargetBase.m_eType;
	INT32				lIndex;
	INT32				alCID[AGSMAI_MAX_PROCESS_TARGET];
	INT32				lNumCID;
	INT32				lNumTarget;
	AgsdAITargetBaseData		astCandidates[AGSMAI_MAX_PROCESS_TARGET];

	switch (eBaseType)
	{
	case APBASE_TYPE_CHARACTER:
		{
			if (pstAction->m_stTarget.m_csTargetBase.m_lID)
			{
				// CID�� ������, Character�� �����ͼ� �ѱ��.
				apcsTarget[0] = m_pcsAgpmCharacter->GetCharacter(pstAction->m_stTarget.m_csTargetBase.m_lID);
				if (!apcsTarget[0])
					return 0;

				return 1;
			}

			// Map���� ���� �þ� �Ÿ��� ��� Character�� �����´�.
			lNumCID = m_pcsApmMap->GetCharList(APMMAP_CHAR_TYPE_PC, pcsCharacter->m_stPos, pstAI->m_fVisibility, alCID, AGSMAI_MAX_PROCESS_TARGET);

			memset(astCandidates, 0, sizeof(AgsdAITargetBaseData) * AGSMAI_MAX_PROCESS_TARGET);

			for (lIndex = 0; lIndex < lNumCID; ++lIndex)
			{
				// �ش� Character�� �����´�.
				astCandidates[lIndex].m_pcsTargetBase = (AgpdCharacter *) alCID[lIndex];
				if (!astCandidates[lIndex].m_pcsTargetBase)
					continue;

				if (!CalcTargetWeight(pcsCharacter, astCandidates + lIndex, pstAI, pstAction, ulClockCount))
					continue;
			}

			QSort(astCandidates, lNumCID, sizeof(AgsdAITargetBaseData), QSortTargetCompare);

			for (lIndex = 0, lNumTarget = 0; lIndex < lNumCID; ++lIndex)
			{
				if (astCandidates[lIndex].m_pcsTargetBase && astCandidates[lIndex].m_lCalcWeight)
				{
					apcsTarget[lNumTarget] = astCandidates[lIndex].m_pcsTargetBase;
					++lNumTarget;
				}
			}

			return lNumTarget;
		}

		break;

	case APBASE_TYPE_CHARACTER_TEMPLATE:
		{
		}

		break;

	case APBASE_TYPE_ITEM:
		{
			// Item�� �������� Code
		}

		break;

	case APBASE_TYPE_OBJECT:
		{
			if (!m_pcsApmObject)
				return FALSE;

			if (pstAction->m_stTarget.m_csTargetBase.m_lID)
			{
				apcsTarget[0] = m_pcsApmObject->GetObject(pstAction->m_stTarget.m_csTargetBase.m_lID);
				if (!apcsTarget[0])
					return FALSE;

				return 1;
			}
		}

		break;
	}

	return 0;
}

BOOL			AgsmAI::CalcTargetWeight(AgpdCharacter *pcsCharacter, AgsdAITargetBaseData *pstTargetData, AgpdAI *pstAI, AgpdAIAction *pstAction, UINT32 ulClockCount)
{
	PROFILE("AgsmAI::CalcTargetWeight()");

	INT32					lValue;
	FLOAT					fDistance;
	INT32					lAgro = 0;
	INT32					lHP = 0;
	INT32					lHistoryHP = 0;
	INT32					lLevel = 0;
	INT32					lTargetLevel = 0;
	INT32					lDiffLevel;
	AgpdCharacter *			pcsTargetCharacter;
	AgpdAITargetWeight *	pstTargetWeight;
	INT32					lTargetFlags;
	AgsdCharacterHistoryEntry *	pstHistoryEntry;

	lValue = 0;

	if (pstTargetData->m_pcsTargetBase->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	pcsTargetCharacter = (AgpdCharacter *) pstTargetData->m_pcsTargetBase;
	pstTargetWeight = &pstAction->m_stTarget.m_stTargetWeight;
	lTargetFlags = pstAction->m_stTarget.m_lTargetFlags;

	//�����̰� Agressive Mob�̸� Value��(��׷ΰ�)�� �־��ش�.
	if( pstTargetWeight->m_lRelationAggressive != 0 )
	{
		lValue += pstTargetWeight->m_lRelationAggressive;
	}

	if (pcsTargetCharacter == pcsCharacter)
	{
		// Self Character�ε�, Other Flag�� On�̸� return FALSE
		if (lTargetFlags & AGPDAI_TARGET_FLAG_OTHER)
			return FALSE;

		lValue += pstTargetWeight->m_lSelf;
	}
	else
	{
		// Other �ε�, Self Flag�� On�̸� return FALSE
		if (lTargetFlags & AGPDAI_TARGET_FLAG_SELF)
			return FALSE;

		lValue += pstTargetWeight->m_lOther;
	}

	// History���� ������.
	pstHistoryEntry = m_pcsAgsmCharacter->GetHistoryEntry(pcsCharacter, pcsTargetCharacter);
	if (pstHistoryEntry)
	{
		// Character���� Agro�� �����ͼ�
		if (m_pcsAgpmFactors->GetValue(&pstHistoryEntry->m_csUpdateFactor, &lAgro, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_AGRO))
		{
			// ���� Agro Flag�� On�ε�, Agro�� 0�̸� return FALSE
			if (!lAgro && (lTargetFlags & AGPDAI_TARGET_FLAG_AGRO))
				return FALSE;

			// Value�� ����Ѵ�.
			lValue += pstTargetWeight->m_lAgro * lAgro / 1000;
		}
		else if (lTargetFlags & AGPDAI_TARGET_FLAG_AGRO)
			return FALSE;

		AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
		if (!pcsResultFactor)
			return FALSE;

		// Character�� HP�� History���� ������ �� HP�� �����ͼ�
		if (m_pcsAgpmFactors->GetValue(&pstHistoryEntry->m_csUpdateFactor, &lHistoryHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP) &&
			m_pcsAgpmFactors->GetValue(pcsResultFactor, &lHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_HP))
		{
			// Value�� ����Ѵ�.
			if (!lHistoryHP)
			{
				// ���� ������ ����µ�, Aggressive�� Friendly Flag�� On�̸� return FALSE
				if (lTargetFlags & (AGPDAI_TARGET_FLAG_HISTORY_DAMAGE | AGPDAI_TARGET_FLAG_HISTORY_BUFFER))
					return FALSE;
			}
			else if (lHistoryHP > 0)
			{
				// ���� Aggressive Flag�� On�ε�, HP�� �÷���ٸ� return FALSE
				if (lTargetFlags & AGPDAI_TARGET_FLAG_HISTORY_DAMAGE)
					return FALSE;

				lValue += pstTargetWeight->m_lHistoryBuffer * (INT32) (lHistoryHP / (FLOAT) lHP * 10);
			}
			else
			{
				// ���� Friendly Flag�� On�ε�, HP�� ���� Aggressive��� return FALSE
				if (lTargetFlags & AGPDAI_TARGET_FLAG_HISTORY_BUFFER)
					return FALSE;

				lValue = (INT32)(pstTargetWeight->m_lHistoryDamage * (-lHistoryHP / (FLOAT) lHP * 10));

				if( lValue <= 0 )
					lValue = 1;
			}
		}
		else if (lTargetFlags & (AGPDAI_TARGET_FLAG_HISTORY_DAMAGE | AGPDAI_TARGET_FLAG_HISTORY_BUFFER))
			return FALSE;
	}
	else if (lTargetFlags & (AGPDAI_TARGET_FLAG_HISTORY_DAMAGE | AGPDAI_TARGET_FLAG_HISTORY_BUFFER | AGPDAI_TARGET_FLAG_AGRO))
		return FALSE;

	// Near �� ��������, Far�� �ּ��� ���� �켱������ ������.
	fDistance = AUPOS_DISTANCE(pcsCharacter->m_stPos, pcsTargetCharacter->m_stPos);
	lValue += pstTargetWeight->m_lNear * (INT32) ((pstAI->m_fVisibility - fDistance) * 10 / pstAI->m_fVisibility);
	lValue += pstTargetWeight->m_lNear * (INT32) (fDistance * 10 / pstAI->m_fVisibility);

	// Character�� Target�� Level�� �����ͼ�
	if (m_pcsAgpmFactors->GetValue(&pcsTargetCharacter->m_csFactor, &lTargetLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL) &&
		m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL))
	{
		// Level���̸� ����Ѵ�. (10�̻� ���� ���� 10���� ����)
		lDiffLevel = lLevel - lTargetLevel;
		if (lDiffLevel >= 10)
			lDiffLevel = 10;
		else if (lDiffLevel <= -10)
			lDiffLevel = -10;

		if (!lDiffLevel)
		{
			if (lTargetFlags & (AGPDAI_TARGET_FLAG_STRONG | AGPDAI_TARGET_FLAG_WEAK))
				return FALSE;
		}
		else if (lDiffLevel > 0)
		{
			// Target�� Level�� �� ������, Strong Flag�� On�̸� return fALSE
			if (lTargetFlags & AGPDAI_TARGET_FLAG_STRONG)
				return FALSE;

			lValue += lDiffLevel * pstAction->m_stTarget.m_stTargetWeight.m_lWeak;
		}
		else
		{
			// Target Level�� �� ������, Weak Flag�� On�̸� return FALSE
			if (lTargetFlags & AGPDAI_TARGET_FLAG_WEAK)
				return FALSE;

			lValue += -lDiffLevel * pstAction->m_stTarget.m_stTargetWeight.m_lStrong;
		}
	}

	// Boss �κ� Coding �ؾߵȴ�.

	pstTargetData->m_lCalcWeight = lValue;

	return TRUE;
}


BOOL			AgsmAI::GetTargetPosition(AgpdCharacter *pcsCharacter, AgpdAI *pstAI, AgpdAIAction *pstAction, UINT32 ulClockCount, AuPOS *pstTarget)
{
	AgsdCharacter *		pstAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	AuPOS *				pstPosTemp;
	FLOAT				fAngle;

	if (m_pcsApmEventManager)
	{
		pstPosTemp = m_pcsApmEventManager->GetBasePos(&pstAction->m_stTarget.m_csTargetBase, NULL);
		if (pstPosTemp)
		{
			*pstTarget = *pstPosTemp;
		}
		else
		{
			*pstTarget = pstAction->m_stTarget.m_stTargetPos;
		}
	}
	else
	{
		*pstTarget = pstAction->m_stTarget.m_stTargetPos;
	}

	if (!pstAction->m_stTarget.m_fRadius)
		return TRUE;

	fAngle = m_csRand.rand(2 * AUMATH_PI);

	pstTarget->x += cos(fAngle) * pstAction->m_stTarget.m_fRadius;
	pstTarget->z += sin(fAngle) * pstAction->m_stTarget.m_fRadius;

	return TRUE;
}

VOID			AgsmAI::AddActionToResult(AgpdAIADChar *pstAIADChar, AgpdAIFactor *pstAIFactor)
{
	INT32				lIndex;
	AgpdAIAction *		pstAction;
	AgpdAIAction *		pstActionPrev;
	AgpdAIAction *		pstActionTemp;

	for (lIndex = 0; lIndex < AGPDAI_ACTION_MAX_TYPE; ++lIndex)
	{
		pstAction = &pstAIFactor->m_astActions[lIndex];

		pstActionPrev = NULL;
		pstActionTemp = pstAIADChar->m_astResult[lIndex].m_pstActions;
		while (pstActionTemp)
		{
			if (pstAction->m_lActionRate > pstActionTemp->m_lActionRate)
			{
				pstAction->m_pstNext = pstActionTemp;
				if (pstActionPrev)
					pstActionPrev->m_pstNext = pstAction;
				else
					pstAIADChar->m_astResult[lIndex].m_pstActions = pstAction;

				break;
			}

			pstActionPrev = pstActionTemp;
			pstActionTemp = pstActionTemp->m_pstNext;
		}

		if (!pstActionTemp)
		{
			pstAction->m_pstNext = NULL;
			if (pstActionPrev)
				pstActionPrev->m_pstNext = pstAction;
			else
				pstAIADChar->m_astResult[lIndex].m_pstActions = pstAction;
		}

		pstAIADChar->m_astResult[lIndex].m_lActionRate += pstAction->m_lActionRate;
	}
}

BOOL			AgsmAI::IsFactorApplicable(AgpdCharacter *pcsCharacter, AgpdAIFactor *pstAIFactor, UINT32 ulClockCount)
{
	switch (pstAIFactor->m_eType)
	{
	case AGPDAI_FACTOR_TYPE_DEFAULT:
		{
			return TRUE;
		}

		break;

	case AGPDAI_FACTOR_TYPE_CHAR_FACTOR:
		{
			if (pstAIFactor->m_uoData.m_pstCharFactor)
			{
				if (m_pcsAgpmFactors->CompareFactor(&pstAIFactor->m_uoData.m_pstCharFactor->m_stFactor, &pcsCharacter->m_csFactor, pstAIFactor->m_uoData.m_pstCharFactor->m_lCompareValue))
					return TRUE;
			}
		}

		break;

	case AGPDAI_FACTOR_TYPE_CHAR_STATUS:
		{
			if (pstAIFactor->m_uoData.m_pstCharStatus)
			{
				if (pstAIFactor->m_uoData.m_pstCharStatus->m_eStatus == pcsCharacter->m_unCurrentStatus)
					return TRUE;
			}
		}

		break;

	case AGPDAI_FACTOR_TYPE_CHAR_ITEM:
		{
			if (pstAIFactor->m_uoData.m_pstCharItem)
			{
				if (m_pcsAgpmItem->HasItemTemplate(pcsCharacter, pstAIFactor->m_uoData.m_pstCharItem->m_lItemTID))
					return TRUE;
			}
		}

		break;

	case AGPDAI_FACTOR_TYPE_ACTION_HISTORY:
		{
			if (pstAIFactor->m_uoData.m_pstActionHistory)
			{
			}
		}

		break;

	case AGPDAI_FACTOR_TYPE_ENV_POSITION:
		{
			if (pstAIFactor->m_uoData.m_pstEnvPosition)
			{
			}
		}

		break;

	case AGPDAI_FACTOR_TYPE_EMOTION:
		{
			if (pstAIFactor->m_uoData.m_pstEmotion)
			{
			}
		}

		break;
	}

	return FALSE;
}

BOOL		AgsmAI::SendPacketAIADChar(AgpdCharacter *pcsCharacter, UINT32 ulDPNID)
{
	if (!pcsCharacter || ulDPNID ==  0)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmAI->MakePacketAIADChar(pcsCharacter, &nPacketLength);

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulDPNID);

	m_pcsAgpmAI->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

INT32		AgsmAI::GetHelpMobList( AgpdCharacter *pcsAgpdCharacter, INT32 *plaCharList, int lCount )
{
	AgpdCharacter		*pcsAgpdHelpCharacter;
	AgpdAIADChar		*pstAIADChar;
	INT32				laCharList[20];
	INT32				lNearCharCount;
	INT32				lHelpCharTID;
	INT32				lHelpCharCount;

	lHelpCharCount = 0;

	lNearCharCount = m_pcsApmMap->GetCharList( APMMAP_CHAR_TYPE_MONSTER, pcsAgpdCharacter->m_stPos, 10.0f, laCharList, 20 );
	 
	for( int i=0; i<lNearCharCount; i++ )
	{
		pcsAgpdHelpCharacter = (AgpdCharacter *) laCharList[i];

		if( pcsAgpdHelpCharacter != NULL )
		{
			pstAIADChar = m_pcsAgpmAI->GetCharacterData(pcsAgpdHelpCharacter);

			if( pstAIADChar != NULL )
			{
				for( int lTempTIDCounter=0; lTempTIDCounter<AGPDAI_MAX_HELP_CHARTYPE_COUNT; lTempTIDCounter++ )
				{
					lHelpCharTID = pstAIADChar->m_stAI.m_csAIScream.m_alHelpCharTID[lTempTIDCounter];

					if( lHelpCharTID == pcsAgpdCharacter->m_lTID1 )
					{
						for( int j=0; j<lCount; j++ )
						{
							if( plaCharList[j] == 0 )
							{
								plaCharList[j] = lHelpCharTID;
								break;
							}
						}

						lHelpCharCount++;
						break;
					}
				}
			}
		}
	}

	return lHelpCharCount;
}

BOOL		AgsmAI::ProcessHelpScreamL1(AgpdCharacter *pcsCharacter)
{
	//���� ��ġ�� �������� �ֺ��� �ִ� ���͸� ����.
	AgpdCharacter		*pcsAgpdHelpCharacter;
	INT32				laCharList[20];
	INT32				lCharCount;
	INT32				lCounter;
	INT32				lCharID;

	memset( laCharList, 0, sizeof(INT32) * 20 );
	lCharCount = GetHelpMobList( pcsCharacter, laCharList, 20 );

	for( lCounter=0; lCounter<lCharCount; lCounter++ )
	{
		lCharID = laCharList[lCounter];

		pcsAgpdHelpCharacter = m_pcsAgpmCharacter->GetCharacter( lCharID );

		if( pcsAgpdHelpCharacter != NULL )
		{
			//Fast Agro History �߰� 
		}
	}

	return TRUE;
}

BOOL		AgsmAI::ProcessHelpScreamL2(AgpdCharacter *pcsCharacter)
{
	AgpdCharacter		*pcsAgpdHelpCharacter;
	INT32				laCharList[20];
	INT32				lCharCount;
	INT32				lCounter;
	INT32				lCharID;

	memset( laCharList, 0, sizeof(INT32) * 20 );
	lCharCount = GetHelpMobList( pcsCharacter, laCharList, 20 );

	for( lCounter=0; lCounter<lCharCount; lCounter++ )
	{
		lCharID = laCharList[lCounter];

		pcsAgpdHelpCharacter = m_pcsAgpmCharacter->GetCharacter( lCharID );

		if( pcsAgpdHelpCharacter != NULL )
		{
			//Heal ��ų�� ������ pcsCharacter���� Heal�� �ɾ��ش�.
		}
	}

	return TRUE;
}

BOOL		AgsmAI::ProcessHelpScreamL3(AgpdCharacter *pcsCharacter)
{
	AgpdCharacter		*pcsAgpdHelpCharacter;
	INT32				laCharList[20];
	INT32				lCharCount;
	INT32				lCounter;
	INT32				lCharID;

	memset( laCharList, 0, sizeof(INT32) * 20 );
	lCharCount = GetHelpMobList( pcsCharacter, laCharList, 20 );

	for( lCounter=0; lCounter<lCharCount; lCounter++ )
	{
		lCharID = laCharList[lCounter];

		pcsAgpdHelpCharacter = m_pcsAgpmCharacter->GetCharacter( lCharID );

		if( pcsAgpdHelpCharacter != NULL )
		{
			//Slow Agro History �߰� 
		}
	}

	return TRUE;
}

void __cdecl AgsmAI::QSort(void *pData, unsigned int nMaxData, unsigned int nDataSize, int (__cdecl *pfCompare)(const void *, const void *))
{
	PROFILE("AgsmAI::QSort()");

	qsort(pData, nMaxData, nDataSize, pfCompare);
}