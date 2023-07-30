#include "AgsmDropItem.h"

AgsmDropItem::AgsmDropItem()
{
	SetModuleName("AgsmDropItem");
}

AgsmDropItem::~AgsmDropItem()
{
}

BOOL AgsmDropItem::OnAddModule()
{
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmParty = (AgpmParty *) GetModule( "AgpmParty" );
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmItemConvert = (AgpmItemConvert *) GetModule( "AgpmItemConvert" );
	m_pcsAgpmDropItem = (AgpmDropItem *) GetModule("AgpmDropItem");
	m_pcsAgsmItemManager = (AgsmItemManager *) GetModule("AgsmItemManager");
	m_pcsAgsmItem = (AgsmItem *) GetModule("AgsmItem");

	if( !m_pcsAgpmFactors || !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgpmItemConvert || !m_pcsAgpmDropItem || !m_pcsAgsmItemManager || !m_pcsAgsmItem )
	{
		return FALSE;
	}

	m_nIndexDropTemplateInfo = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdDropItemTemplate *)*10, NULL, NULL);

	m_pcsAgpmItem->SetCallbackDropMoneyToField( CBDropMoneyToField, this );

	return TRUE;
}

AgpdDropItemTemplate* AgsmDropItem::GetDropItemTemplate(AgpdCharacter *pData, INT32 lIndex)
{
	AgpdDropItemTemplate	*pcsTemplate;

	ASSERT(lIndex >= 0);

	if (m_pcsAgpmCharacter)
	{
		pcsTemplate = ((AgpdDropItemTemplate **)m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexDropTemplateInfo, (PVOID) pData))[lIndex];

		return pcsTemplate;
	}
	else
	{
		return NULL;
	}
}

BOOL AgsmDropItem::SetDropItemTemplate(AgpdDropItemTemplate *pcsTemplate, AgpdCharacter *pData, INT32 lIndex )
{
	BOOL				bResult;

	bResult = FALSE;

	ASSERT(lIndex >= 0);

	if (m_pcsAgpmCharacter)
	{
		((AgpdDropItemTemplate **)m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexDropTemplateInfo, (PVOID) pData))[lIndex] = pcsTemplate;

		bResult = TRUE;
	}

	return bResult;
}

INT32 AgsmDropItem::GetProbableByLvlGap( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter )
{
	INT32			lMobLevel, lPCLevel;
	INT32			lProbable;

	lProbable = -1;
	lPCLevel = 0;
	lMobLevel = m_pcsAgpmCharacter->GetLevel( pcsAgpdCharacter );

	if (pcsFirstLooter)
	{
		AgpdParty		*pcsAgpdParty;

		if (pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER)
		{
			pcsAgpdParty = m_pcsAgpmParty->GetParty( ((AgpdCharacter *)pcsFirstLooter) );
		}
		else
		{
			pcsAgpdParty = (AgpdParty *) pcsFirstLooter;
		}

		//��Ƽ�� �����ִٸ�?
		if( pcsAgpdParty )
		{
			lPCLevel = m_pcsAgpmParty->GetPartyAvrMemberLevel( pcsAgpdParty );
		}
		else
		{
			lPCLevel = m_pcsAgpmCharacter->GetLevel( (AgpdCharacter *)pcsFirstLooter );
		}
	}

	if( lPCLevel && lMobLevel )
	{
		lProbable = 0;

		if( lPCLevel > lMobLevel+2 )
		{
			lProbable += (lPCLevel-(lMobLevel+2))*20;

			if( lProbable >= 100 )
				lProbable = -1;
		}
	}

	return lProbable;
}

BOOL AgsmDropItem::DropCommonItem( AgpdCharacter *pcsCharacter, ApBase *pcsFirstLooter, AgpdDropItemTemplate *pcsAgpdDropItemTemplate )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pcsAgpdDropItemTemplate )
	{
		for( int lCategory=0; lCategory<AGPMDROPITEM_MAX_CATEGORY; lCategory++ )
		{
			//���ʿ� ����� �������� ���ٸ� �׳� �Ѿ��. 
			if( pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_lItemCount == 0 )
				continue;

			//Equip, Ghelld�� ���� ����.
			if( (lCategory == AGPMDROPITEM_EQUIP) || (lCategory == AGPMDROPITEM_GHELLD) )
			{
				//Ư���� ���⼭ ������ ������ �ʴ´�.
			}
			else
			{
				INT32			lProbable;
				float			fRealProbable;

				lProbable = GetProbableByLvlGap( pcsCharacter, pcsFirstLooter );

				//�������̰� Ŀ�� ����� �ƿ� �ȵǴ� ���.
				if( lProbable < 0 )
					continue;

				fRealProbable = pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_fDropProbable - lProbable;

				//���Ȯ���� �־����� �������� �������� ���ؼ� �ƿ� ����� �ȵǴ� ���.
				if( fRealProbable < 0.0f )
					continue;

				//�켱 ����� �������� ����~
				if( m_csRand.randInt(100000000) <= (INT32)(fRealProbable*1000000.0f) )
				{
					INT32				lCounter;
					INT32				lItemNum;
					INT32				lItemDropIndex;
					INT32				lTotalProbable;
					INT32				lDropProbable, lMinDropProbable, lMaxDropProbable;

					lTotalProbable = 0;
					lItemNum = pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_lItemCount;
					lDropProbable = 0;
					lItemDropIndex = -1;
					lMinDropProbable = 0;
					lMaxDropProbable = 0;

					for( lCounter=0; lCounter<lItemNum; lCounter++ )
					{
						//�ǹ̾��� �ڵ����� ��������� �Ϸ���.... ��.
						if( pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_alScalar[lCounter] == 0 )
							continue;

						lTotalProbable+=pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_alScalar[lCounter];
					}

					if( lTotalProbable )
					{
						lDropProbable = m_csRand.randInt(lTotalProbable);

						for( lCounter=0; lCounter<lItemNum; lCounter++ )
						{
							if( pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_alScalar[lCounter] == 0 )
								continue;

							lMinDropProbable = lMaxDropProbable;
							lMaxDropProbable = ( lMinDropProbable + pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_alScalar[lCounter] );

							if( (lMinDropProbable <= lDropProbable) && ( lDropProbable < lMaxDropProbable ) )
							{
								lItemDropIndex = lCounter;
								break;
							}
						}
					}

					if( lItemDropIndex != -1 )
					{
						//�������� �����Ѵ�.
						AddItemToField( pcsCharacter, pcsFirstLooter, pcsAgpdDropItemTemplate->m_cDropCategory[lCategory].m_alTID[lItemDropIndex] );
					}
				}
			}
		}

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgsmDropItem::DropEquipItem( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter, AgpdDropItemEquipTemplate *pcsAgpdDropEquipItemTemplate, float fProbable )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pcsAgpdDropEquipItemTemplate )
	{
		INT32			lTempProbable;
		float			fRealProbable;

		lTempProbable = GetProbableByLvlGap( pcsAgpdCharacter, pcsFirstLooter );

		//�������̷� �ƿ� ����� �ȵǴ°��.
		if( lTempProbable < 0 )
			return FALSE;

		fRealProbable = fProbable - lTempProbable;

		//���Ȯ���� �־����� �������� �������� ���ؼ� �ƿ� ����� �ȵǴ� ���.
		if( fRealProbable < 0.0f )
			return FALSE;

		//�켱 ����� �������� ����~
		if( m_csRand.randInt(100000000) <= (INT32)(fRealProbable*1000000.0f) )
		{
			AgpdDropEquipData	*pcsAgpdDropEquipData;

			INT32				lCounter;
			INT32				lItemNum;
			INT32				lItemDropIndex;
			INT32				lTotalProbable;
			INT32				lDropProbable, lMinDropProbable, lMaxDropProbable;
			INT32				lRace, lClass;

			pcsAgpdDropEquipData = NULL;

			lTotalProbable = 0;

			if(	pcsFirstLooter && pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER )
			{
				lRace = m_pcsAgpmFactors->GetRace( &(((AgpdCharacter *)pcsFirstLooter)->m_csFactor) );
				lClass = m_pcsAgpmFactors->GetClass( &(((AgpdCharacter *)pcsFirstLooter)->m_csFactor) );

				for( lCounter=0;lCounter<AGPMDROPITEM_EQUIP_MAX_RACE;lCounter++ )
				{
					if( (pcsAgpdDropEquipItemTemplate->m_csAgpdDropEquipData[lCounter].m_lRace == lRace ) &&
						(pcsAgpdDropEquipItemTemplate->m_csAgpdDropEquipData[lCounter].m_lClass == lClass ) )
					{
						pcsAgpdDropEquipData = &pcsAgpdDropEquipItemTemplate->m_csAgpdDropEquipData[lCounter];
						break;
					}
				}
			}

			if( pcsAgpdDropEquipData )
			{
				lItemNum = pcsAgpdDropEquipData->m_lItemCount;
				lDropProbable = 0;
				lItemDropIndex = -1;
				lMinDropProbable = 0;
				lMaxDropProbable = 0;

				for( lCounter=0; lCounter<lItemNum; lCounter++ )
				{
					//�ǹ̾��� �ڵ����� ��������� �Ϸ���~
					if( pcsAgpdDropEquipData->m_alScalar[lCounter] == 0 )
						continue;

					lTotalProbable+=pcsAgpdDropEquipData->m_alScalar[lCounter];
				}

				if( lTotalProbable )
				{
					lDropProbable = m_csRand.randInt(lTotalProbable);

					for( lCounter=0; lCounter<lItemNum; lCounter++ )
					{
						if( pcsAgpdDropEquipData->m_alScalar[lCounter] == 0 )
							continue;

						lMinDropProbable = lMaxDropProbable;
						lMaxDropProbable = ( lMinDropProbable + pcsAgpdDropEquipData->m_alScalar[lCounter] );

						if( (lMinDropProbable <= lDropProbable) && ( lDropProbable < lMaxDropProbable ) )
						{
							lItemDropIndex = lCounter;
							break;
						}
					}
				}

				if( lItemDropIndex != -1 )
				{
					//�������� �����Ѵ�.
					AddItemToField( pcsAgpdCharacter, pcsFirstLooter, pcsAgpdDropEquipData->m_alTID[lItemDropIndex] );
				}
			}
		}
	}

	return bResult;
}

BOOL AgsmDropItem::DropItem( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter )
{
	AgpdDropItemTemplate			*pcsAgpdDropItemTemplate;

	//���ñ����� �������� ĳ���ε�....
	if( pcsFirstLooter && pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER && m_pcsAgpmCharacter->IsPC((AgpdCharacter *)pcsFirstLooter) )
	{
		//���ñ����� ���忡�� �ִٸ�... TID 130~133 �׳� ������. 
		if( 130 <= ((AgpdCharacter *)pcsFirstLooter)->m_pcsCharacterTemplate->m_lID && 
			((AgpdCharacter *)pcsFirstLooter)->m_pcsCharacterTemplate->m_lID <= 133 )
		{
			return TRUE;
		}
	}

	//������ ��츸 ����Ѵ�.
	if( m_pcsAgpmCharacter->IsMonster(pcsAgpdCharacter) )
	{
		INT32				lDropTID;

		lDropTID = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lDropTID;

		pcsAgpdDropItemTemplate = m_pcsAgpmDropItem->m_aDropItemTemplate.GetDropItemTemplate( lDropTID );

		if( pcsAgpdDropItemTemplate )
		{
			DropCommonItem( pcsAgpdCharacter, pcsFirstLooter, pcsAgpdDropItemTemplate );

			if( pcsAgpdDropItemTemplate->m_lEquip > 0 )
			{
				AgpdDropItemEquipTemplate			*pcsAgpdDropItemEquipTemplate;

				pcsAgpdDropItemEquipTemplate = m_pcsAgpmDropItem->m_aEquipItemTemplate.GetDropItemEquipTemplate( pcsAgpdDropItemTemplate->m_lEquip );

				if( pcsAgpdDropItemEquipTemplate )
				{
					DropEquipItem( pcsAgpdCharacter, pcsFirstLooter, pcsAgpdDropItemEquipTemplate, pcsAgpdDropItemTemplate->m_cDropCategory[AGPMDROPITEM_EQUIP].m_fDropProbable );
				}
			}
		}

		//���������� ���� 100% ����Ѵ�. ����~
		INT32			lMinGhelld, lMaxGhelld;

		lMinGhelld = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lGhelldMin;
		lMaxGhelld = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lGhelldMax;

		if( lMinGhelld == lMaxGhelld )
		{
		}
		else
		{
			if( lMinGhelld > lMaxGhelld )
			{
				INT32			lTemp;
				lTemp = lMinGhelld;
				lMinGhelld = lMaxGhelld;
				lMaxGhelld = lTemp;
			}

			lMinGhelld += m_csRand.randInt( lMaxGhelld - lMinGhelld );

			if (pcsFirstLooter && pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER)
			{
				INT32	lMoreGhelldPercent	= 0;
				
				/*lMoreGhelldPercent += m_pcsAgpmItemConvert->GetBonusMoreGhelldPercent((AgpdCharacter *) pcsFirstLooter);*/

				if (lMoreGhelldPercent > 0)
					lMinGhelld	= (INT32) ((FLOAT)lMinGhelld * (1.0f + (FLOAT)lMoreGhelldPercent / 100.0f));
			}

			//���� TID�� ��� lMinGhelld��ŭ ����ϸ�ȴ�.
			AddItemToField( pcsAgpdCharacter, pcsFirstLooter, m_pcsAgpmItem->GetMoneyTID(), lMinGhelld );
		}
	}

	return TRUE;
}

BOOL AgsmDropItem::DropItem( AgpdCharacter *pcsAgpdCharacter, AgpdDropItemBag *pcsAgpdDropItemBag, ApBase *pcsFirstLooter )
{
	AgpdParty		*pcsAgpdParty;
	INT32			lTotalDropItemCount;
	INT32			lCounter;
	INT32			lPCLevel;
	INT32			lMobLevel;

//	INT32			alTempDropRate[AGPDDROPITEM_COUNT+1];
	ApSafeArray<INT32, AGPDDROPITEM_COUNT+1>	alTempDropRate;
	alTempDropRate.MemSetAll();

	pcsAgpdParty	= NULL;
	lTotalDropItemCount = 0;

	//PC�Ƿ��� Mob������ ������.
	lPCLevel = 0;
	lMobLevel = m_pcsAgpmCharacter->GetLevel( pcsAgpdCharacter );

	if (pcsFirstLooter)
	{
		if (pcsFirstLooter->m_eType == APBASE_TYPE_CHARACTER)
		{
			pcsAgpdParty = m_pcsAgpmParty->GetParty( ((AgpdCharacter *)pcsFirstLooter) );
		}
		else
		{
			pcsAgpdParty = (AgpdParty *) pcsFirstLooter;
		}

		//��Ƽ�� �����ִٸ�?
		if( pcsAgpdParty )
		{
			lPCLevel = m_pcsAgpmParty->GetPartyAvrMemberLevel( pcsAgpdParty );
		}
		else
		{
			lPCLevel = m_pcsAgpmCharacter->GetLevel( (AgpdCharacter *)pcsFirstLooter );
		}
	}

	if( lPCLevel && lMobLevel )
	{
		//���� ���̿� ���� Drop Rate�� �����Ѵ�.
		for( lCounter=0; lCounter<AGPDDROPITEM_COUNT+1; lCounter++ )
		{
			alTempDropRate[lCounter] = pcsAgpdDropItemBag->m_alDropItemCountRate[lCounter];
		}

		//PC�� ������ ������ ���ٸ� DropRate�� �����Ѵ�.
		if( lPCLevel > lMobLevel )
		{
			INT32				lTempDropRate;
			INT32				lLevelGap;

			lLevelGap = lPCLevel - lMobLevel;

			for( lCounter=0; lCounter<AGPDDROPITEM_COUNT+1; lCounter++ )
			{
				if( lCounter == 0 )
				{
					lTempDropRate = alTempDropRate[lCounter];
					lTempDropRate += (alTempDropRate[lCounter]*(lLevelGap*lLevelGap)/10);
				}
				else
				{
					lTempDropRate = alTempDropRate[lCounter];
					lTempDropRate -= (alTempDropRate[lCounter]*(lLevelGap*lLevelGap)/10);
				}

				if( lTempDropRate < 0 )
				{
					lTempDropRate = 0;
				}

				alTempDropRate[lCounter] = lTempDropRate;
			}
		}

		//
		for( lCounter=0; lCounter<AGPDDROPITEM_COUNT+1; lCounter++ )
		{
			lTotalDropItemCount += alTempDropRate[lCounter];
		}

		//Ư���� ������ �������� ����ؾ��ϸ�?
		if( lTotalDropItemCount )
		{
			INT32			lItemDropCount;
			INT32			lItemDropCountRate;
			INT32			lItemDropCountMin;
			INT32			lItemDropCountMax;

			lItemDropCount = 0;
			lItemDropCountMin = 0;
			lItemDropCountMax = 0;

			lItemDropCountRate = m_csRand.randInt(lTotalDropItemCount);

			for( lCounter=0; lCounter<AGPDDROPITEM_COUNT+1; lCounter++ )
			{
				if( alTempDropRate[lCounter] == 0 )
					continue;

				lItemDropCountMin = lItemDropCountMax;
				lItemDropCountMax = ( lItemDropCountMin + alTempDropRate[lCounter] );

				if( (lItemDropCountMin <= lItemDropCountRate) && ( lItemDropCountRate < lItemDropCountMax ) )
				{
					lItemDropCount = lCounter;
					break;
				}
			}

			//��� ������� ����������~
			if( lItemDropCount )
			{
//				INT32			lTempRate[AGPDDROPITEM_ITEM_COUNT];
//
//				memset(lTempRate, 0, sizeof(INT32)*(AGPDDROPITEM_ITEM_COUNT) );

				ApSafeArray<INT32, AGPDDROPITEM_ITEM_COUNT>		lTempRate;
				lTempRate.MemSetAll();

				for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
				{
					lTempRate[lCounter] = pcsAgpdDropItemBag->m_csAgpdDropItemInfo[lCounter].m_lDropRate;
				}

				for( lCounter=0; lCounter<lItemDropCount; lCounter++ )
				{
					INT32			lProbable;
					INT32			lMinProbable;
					INT32			lMaxProbable;
					INT32			lProbableCount;
					INT32			lTotalProbable;

					lMinProbable = 0;
					lMaxProbable = 0;

					lTotalProbable = 0;

					for( lProbableCount=0; lProbableCount<AGPDDROPITEM_ITEM_COUNT; lProbableCount++ )
						lTotalProbable += lTempRate[lProbableCount];

					if (lTotalProbable == 0)
						break;

					lProbable = m_csRand.randInt(lTotalProbable);

					for( lProbableCount=0; lProbableCount<AGPDDROPITEM_ITEM_COUNT; lProbableCount++ )
					{
						lMinProbable = lMaxProbable;
						lMaxProbable = (lMinProbable + lTempRate[lProbableCount]);

						if( (lMinProbable <= lProbable) && (lProbable < lMaxProbable ) )
						{
							lTempRate[lProbableCount] = 0;

							DropItemToField( pcsAgpdCharacter, pcsAgpdDropItemBag, lProbableCount, lPCLevel, lMobLevel, pcsFirstLooter, FALSE );
							break;
						}
					}
				}
			}
		}
		//Free For All�̶��?
		else
		{
			//������ ��~ ����.
			for( lCounter=0; lCounter<AGPDDROPITEM_ITEM_COUNT; lCounter++ )
			{
				if( pcsAgpdDropItemBag->m_csAgpdDropItemInfo[lCounter].m_lItemTID )
				{
					DropItemToField( pcsAgpdCharacter, pcsAgpdDropItemBag, lCounter, lPCLevel, lMobLevel, pcsFirstLooter );
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmDropItem::DropItemToField( AgpdCharacter *pcsCharacter, AgpdDropItemBag *pcsAgpdDropItemBag, INT32 lIndex, INT32 lPCLevel, INT32 lMobLevel, ApBase *pcsFirstLooter, BOOL bCheckDropRate )
{
	AgpdDropItemInfo		*pcsAgpdDropItemInfo;
	AgpdItemTemplate		*pcsItemTemplate;


	BOOL		bDrop;
	INT32		lDropItemCount;
	INT32		lDropProbable;
	INT32		lMaxQuantity, lMinQuantity;

	pcsAgpdDropItemInfo = &pcsAgpdDropItemBag->m_csAgpdDropItemInfo[lIndex];
	pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcsAgpdDropItemInfo->m_lItemTID );

	bDrop = FALSE;
	lDropItemCount = 0;
	lMaxQuantity = pcsAgpdDropItemInfo->m_lMaxQuantity;
	lMinQuantity = pcsAgpdDropItemInfo->m_lMinQuantity;

	//�����ϴ� ������ Ÿ������ Ȯ���Ѵ�.
	if( pcsItemTemplate == NULL )
		return FALSE;

	if( bCheckDropRate == FALSE )
	{
		bDrop = TRUE;
	}
	else
	{
		//PC�� Mob���� ������ ���ų� ���ٸ�.
		if( lPCLevel <= lMobLevel )
		{
			lDropProbable = m_csRand.randInt(AGPDDROPITEM_PROBABLE);

			if( lDropProbable < pcsAgpdDropItemInfo->m_lDropRate )
			{
				bDrop = TRUE;
			}
		}
		//PC�� Mob���� ������ ���ٸ�? ������� �����Ѵ�.
		else
		{
			float				fRevisionC;
			INT32				lRevisionUnitC;
			INT32				lLevelGap;

			lLevelGap = lPCLevel - lMobLevel;

			fRevisionC = (lLevelGap)*(lLevelGap)/100.0f;

			lRevisionUnitC = (INT32)(AGPDDROPITEM_PROBABLE + AGPDDROPITEM_PROBABLE*fRevisionC);

			lDropProbable = m_csRand.randInt(lRevisionUnitC);

			if( lDropProbable < pcsAgpdDropItemInfo->m_lDropRate )
			{
				bDrop = TRUE;

				//���� ����Ŀ�� �������̸� ��������۰����� �����Ѵ�.
				if( pcsItemTemplate->m_bStackable )
				{
					//�������� 5�̻��̸� �ȶ�����.
					if( lLevelGap >= 5 )
					{
						bDrop = FALSE;
					}
					else
					{
						//Quantity ����.
						//Random (Min*(1-Level Gap/5)+1, Max*(1-Level Gap/5)+1);
						lMaxQuantity = (INT32)(lMaxQuantity*(1.0f-lLevelGap/5.0f))+1;
						lMinQuantity = (INT32)(lMinQuantity*(1.0f-lLevelGap/5.0f))+1;

						//����Ŀ�� �������� �ƽ����� 0�� ���ų� �۴ٸ� ����� �� ����.
						if( lMaxQuantity <= 0 )
						{
							bDrop = FALSE;
						}

						//�ּҰ��� 0���� �۴ٸ� 0���� �����Ѵ�. �ִ밪�� 0���� ũ�� ������ Ȯ���� �ִ�.
						if( lMinQuantity <= 0 )
						{
							lMinQuantity = 0;
						}
					}
				}
			}
		}
	}

	if( bDrop )
	{
		bool				bJackpot;

		bJackpot = false;

		//���̰��ɼ��� �ִ��� Ȯ���Ѵ�. ������ ������ �Ϲ� ����� �ȵȴ�.
		if( pcsAgpdDropItemInfo->m_lJackpotRate )
		{
			INT32			lJackpotProbalbe;

			lJackpotProbalbe = m_csRand.randInt(AGPDDROPITEM_PROBABLE);

			if( lJackpotProbalbe <  pcsAgpdDropItemInfo->m_lJackpotRate )
			{
				for( INT32 lCounter=0; lCounter< pcsAgpdDropItemInfo->m_lJackpotPiece; lCounter++ )
				{
					INT32			lDropQuantity;

					lDropQuantity = 0;

					if( lMaxQuantity > lMinQuantity )
					{
						lDropQuantity = lMinQuantity + (m_csRand.randInt((lMaxQuantity-lMinQuantity+1)));
					}
					else if( lMaxQuantity == lMinQuantity )
					{
						lDropQuantity = lMaxQuantity;
					}
					//�̰� ����. Min�� Max���� ũ��.
					else
					{
						//���� �α׸� ���ܾ��Ϸ���?
					}

					if( lDropQuantity > 0 )
					{
						AddItemToField( pcsCharacter, pcsAgpdDropItemInfo, lDropQuantity, pcsFirstLooter );
					}
				}

				bJackpot = true;
			}
		}

		//Jackpot�� �������ʾ����� �׳� ����Ѵ�.
		if( bJackpot == false )
		{
			INT32			lDropQuantity;

			lDropQuantity = 0;

			if( lMaxQuantity > lMinQuantity )
			{
				lDropQuantity = lMinQuantity + (m_csRand.randInt((lMaxQuantity-lMinQuantity+1)));
			}
			else if( lMaxQuantity == lMinQuantity )
			{
				lDropQuantity = lMaxQuantity;
			}
			//�̰� ����. Min�� Max���� ũ��.
			else
			{
				//���� �α׸� ���ܾ��Ϸ���?
			}

			//�������.
			AddItemToField( pcsCharacter, pcsAgpdDropItemInfo, lDropQuantity, pcsFirstLooter );
		}
	}

	return TRUE;
}

INT32 AgsmDropItem::GetRandomRuneOptValue( INT32 lMinValue, INT32 lMaxValue )
{
	INT32			lResult;

	lResult = 0;

	if( lMinValue == lMaxValue )
	{
		lResult = lMinValue;
	}
	else
	{
		if( lMaxValue > lMinValue )
		{
			INT32			lTempValue;

			lTempValue = m_csRand.randInt((lMaxValue-lMinValue+1));

			lResult = lMinValue + lTempValue;
		}
	}

	return lResult;
}

BOOL AgsmDropItem::ProcessRuneOption( AgpdItem *pcsAgpdItem, AgpdDropItemInfo *pcsAgpdDropItemInfo )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pcsAgpdItem != NULL && pcsAgpdDropItemInfo != NULL )
	{
		INT32				lTotalRuneOptCount;
		INT32				lCounter;

		lTotalRuneOptCount = 0;

		for( lCounter=0; lCounter<AGPDDROPITEM_RUNECOUNT+1; lCounter++ )
		{
			lTotalRuneOptCount += pcsAgpdDropItemInfo->m_alDropRuneCountRate[lCounter];
		}

		if( lTotalRuneOptCount )
		{
			INT32			lRuneOptCount;
			INT32			lRuneOptCountRate;
			INT32			lRuneOptCountMin;
			INT32			lRuneOptCountMax;

			lRuneOptCount = 0;
			lRuneOptCountMin = 0;
			lRuneOptCountMax = 0;

			lRuneOptCountRate = m_csRand.randInt(lTotalRuneOptCount);

			for( lCounter=0; lCounter<AGPDDROPITEM_RUNECOUNT+1; lCounter++ )
			{
				if( pcsAgpdDropItemInfo->m_alDropRuneCountRate[lCounter] == 0 )
					continue;

				lRuneOptCountMin = lRuneOptCountMax;
				lRuneOptCountMax = ( lRuneOptCountMin + pcsAgpdDropItemInfo->m_alDropRuneCountRate[lCounter] );

				if( (lRuneOptCountMin <= lRuneOptCountRate) && ( lRuneOptCountRate < lRuneOptCountMax ) )
				{
					lRuneOptCount = lCounter;
					break;
				}
			}
		}
	}

	return bResult;
}

BOOL AgsmDropItem::AddItemToField( AgpdCharacter *pcsAgpdCharacter, ApBase *pcsFirstLooter, INT32 lItemTID, INT32 lDropCount )
{
	AgpdItemTemplate			*pcsAgpdItemTemplate;

	pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate( lItemTID );

	if( pcsAgpdItemTemplate != NULL )
	{
		AgpdItem			*pcsAgpdItem;

		//���ð����� �������̸�?
		if( pcsAgpdItemTemplate->m_bStackable )
		{
			if( lDropCount == 0 )
			{
				pcsAgpdItem = m_pcsAgsmItemManager->CreateItem( pcsAgpdItemTemplate->m_lID, NULL, 1, FALSE );
			}
			else if( lDropCount > 0 )
			{
				pcsAgpdItem = m_pcsAgsmItemManager->CreateItem( pcsAgpdItemTemplate->m_lID, NULL, lDropCount, FALSE );
			}
		}
		//���úҰ����� �������̸�?
		else
		{
			pcsAgpdItem = m_pcsAgsmItemManager->CreateItem( pcsAgpdItemTemplate->m_lID, NULL, 0, FALSE );
		}

		if (!pcsAgpdItem)
			return FALSE;

		AgsdItem *pcsAgsdItem = m_pcsAgsmItem->GetADItem(pcsAgpdItem);
		if (!pcsAgsdItem)
			return FALSE;

		//���� ������̸�~ 
		if ( ((AgpdItemTemplate *)pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP )
		{
			INT32				lProbable;

			//90%�� Ȯ���� �ƹ��ɼǾ���, 9%�� Ȯ���� +1����, 1%�� Ȯ���� +2������ ���~
			lProbable = m_csRand.randInt(100);

			if( lProbable == 0 )
			{
				m_pcsAgpmItemConvert->SetPhysicalConvert( pcsAgpdItem, 2 );
			}
			else if( lProbable <= 10 )
			{
				m_pcsAgpmItemConvert->SetPhysicalConvert( pcsAgpdItem, 1 );
			}
			else
			{
				//�׿��� ���� �ƹ��ϵ� ����. �ȳ�~
			}

			//90%�� Ȯ���� �ƹ��ɼǾ���, 9%�� Ȯ���� +2����, 1%�� Ȯ���� +3�������� ���~
			lProbable = m_csRand.randInt(100);

			//������ ������ �Ѱ��� �շ������Ƿ� 2,3�� �Է�������Ѵ�.
			if( lProbable == 0 )
			{
				m_pcsAgpmItemConvert->SetSocketConvert( pcsAgpdItem, 3 );
			}
			else if( lProbable <= 10 )
			{
				m_pcsAgpmItemConvert->SetSocketConvert( pcsAgpdItem, 2 );
			}
			else
			{
				//�׿��� ���� �ƹ��ϵ� ����. �ȳ�~
			}
		}

		//���� ������ ��ġ�� ������.
		pcsAgpdItem->m_posItem = pcsAgpdCharacter->m_stPos;

		INT32	lRandomX	= m_csRand.randInt(100);
		INT32	lRandomZ	= m_csRand.randInt(100);

		if (lRandomX > 50)
			pcsAgpdItem->m_posItem.x	+= lRandomX;
		else
			pcsAgpdItem->m_posItem.x	-= (lRandomX + 50);

		if (lRandomZ > 50)
			pcsAgpdItem->m_posItem.z	+= lRandomZ;
		else
			pcsAgpdItem->m_posItem.z	-= (lRandomZ + 50);

		pcsAgsdItem	= m_pcsAgsmItem->GetADItem(pcsAgpdItem);
		if (pcsAgsdItem && pcsFirstLooter)
		{
			pcsAgsdItem->m_csFirstLooterBase.m_eType	= pcsFirstLooter->m_eType;
			pcsAgsdItem->m_csFirstLooterBase.m_lID		= pcsFirstLooter->m_lID;

			pcsAgsdItem->m_ulDropTime					= GetClockCount();
		}

		m_pcsAgpmItem->AddItemToField( pcsAgpdItem );
	}

	return TRUE;
}

BOOL AgsmDropItem::AddItemToField( AgpdCharacter *pcsAgpdCharacter, AgpdDropItemInfo *pcsAgpdDropItemInfo, INT32 lQuantity, ApBase *pcsFirstLooter )
{
	AgpdItemTemplate			*pcsAgpdItemTemplate;

	pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcsAgpdDropItemInfo->m_lItemTID );

	if( pcsAgpdItemTemplate != NULL )
	{
		//���� ����Ŀ�� �������ε� Quantity�� 0�̸� �׳� �Ѿ��.
		if( pcsAgpdItemTemplate->m_bStackable && (lQuantity <= 0) )
		{
		}
		//�׷��� �ʴٸ� �������� �������~
		else
		{
			AgpdItem			*pcsAgpdItem;

			pcsAgpdItem = m_pcsAgsmItemManager->CreateItem( pcsAgpdItemTemplate->m_lID, NULL, lQuantity, FALSE );
			if (!pcsAgpdItem)
				return FALSE;

//			//�������� ��������� Rune Option�� �پ��ִٸ� Rune Option�� �߰��Ѵ�.
//			ProcessRuneOption( pcsAgpdItem, pcsAgpdDropItemInfo );

			//���� ������ ��ġ�� ������.
			pcsAgpdItem->m_posItem = pcsAgpdCharacter->m_stPos;

			INT32	lRandomX	= m_csRand.randInt(100);
			INT32	lRandomZ	= m_csRand.randInt(100);

			if (lRandomX > 50)
				pcsAgpdItem->m_posItem.x	+= lRandomX;
			else
				pcsAgpdItem->m_posItem.x	-= (lRandomX + 50);

			if (lRandomZ > 50)
				pcsAgpdItem->m_posItem.z	+= lRandomZ;
			else
				pcsAgpdItem->m_posItem.z	-= (lRandomZ + 50);

			AgsdItem			*pcsAgsdItem	= m_pcsAgsmItem->GetADItem(pcsAgpdItem);
			if (pcsAgsdItem && pcsFirstLooter)
			{
				pcsAgsdItem->m_csFirstLooterBase.m_eType	= pcsFirstLooter->m_eType;
				pcsAgsdItem->m_csFirstLooterBase.m_lID		= pcsFirstLooter->m_lID;

				pcsAgsdItem->m_ulDropTime					= GetClockCount();
			}

			m_pcsAgpmItem->AddItemToField( pcsAgpdItem );
		}
	}

	return TRUE;
}

BOOL AgsmDropItem::CBDropMoneyToField( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pClass || !pData)
		return FALSE;

	AgsmDropItem		*pThis;
	AgpdCharacter		*pcsAgpdCharacter;
	INT32				lDropMoneyCount;
	BOOL				bResult;

	pThis	= (AgsmDropItem *) pClass;
	pcsAgpdCharacter = (AgpdCharacter *)pData;
	lDropMoneyCount = *((INT32 *)pCustData);
	bResult = FALSE;

	if( pcsAgpdCharacter && (lDropMoneyCount>0) )
	{
		//���� ������~
		if( pThis->m_pcsAgpmCharacter->SubMoney( pcsAgpdCharacter, lDropMoneyCount ) )
		{
			AgpdItem			*pcsAgpdItem;
			INT32				lMoneyTID;

			lMoneyTID = pThis->m_pcsAgpmItem->GetMoneyTID();

			//���� ������~ ���� ���� ������.
			pcsAgpdItem = pThis->m_pcsAgsmItemManager->CreateItem( lMoneyTID, NULL, lDropMoneyCount, FALSE );

			if( pcsAgpdItem )
			{
				pcsAgpdItem->m_posItem = pcsAgpdCharacter->m_stPos;

				bResult = pThis->m_pcsAgpmItem->AddItemToField( pcsAgpdItem );
			}
		}
	}

	return bResult;
}