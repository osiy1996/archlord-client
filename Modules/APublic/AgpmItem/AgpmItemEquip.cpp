/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 16
******************************************************************************/

#include <AgpmItem/AgpmItem.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmSystemMessage/AgpmSystemMessage.h>
#include <AgpmGrid/AgpmGrid.h>


BOOL AgpmItem::SetCallbackEquip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_EQUIP, pfCallback, pClass);
}

// 2004.04.01. steeple
BOOL AgpmItem::SetCallbackEquipForAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_EQUIP_FOR_ADMIN, pfCallback, pClass);
}

/******************************************************************************
* Purpose : Set call-back.
*
* 091202. Bob Jung
******************************************************************************/
BOOL AgpmItem::SetCallbackUnEquip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_UNEQUIP, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveForNearCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_REMOVE_FOR_NEAR_CHARACTER, pfCallback, pClass);
}

BOOL AgpmItem::CBGetItemLancer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmItem *pThis = (AgpmItem *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *)pData;

	AgpdGridItem *pcsAgpdGridItem = pThis->GetEquipItem(pcsCharacter, AGPMITEM_PART_LANCER);
	if (!pcsAgpdGridItem)
		return FALSE;

	AgpdItem *pcsItem = pThis->GetItem(pcsAgpdGridItem);
	if (NULL == pcsItem)
		return FALSE;

	return TRUE;
}

AgpdGridItem *AgpmItem::GetEquipItem( INT32 lCID, INT32 lPart )
{
	if (lPart <= AGPMITEM_PART_NONE || lPart >= AGPMITEM_PART_NUM)
		return NULL;

	AgpdGridItem		*pcsAgpdGridItem;
	AgpdCharacter		*pcsAgpdCharacter;

	pcsAgpdGridItem = NULL;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if( pcsAgpdCharacter )
		pcsAgpdGridItem = GetEquipItem( pcsAgpdCharacter, lPart );

	return pcsAgpdGridItem;
}

AgpdGridItem *AgpmItem::GetEquipItem( AgpdCharacter *pcsAgpdCharacter, INT32 lPart )
{
	if (!pcsAgpdCharacter)
		return NULL;

	if (lPart <= AGPMITEM_PART_NONE || lPart >= AGPMITEM_PART_NUM)
		return NULL;

	AgpdItemADChar		*pcsAgpdItemADChar;
	AgpdGridItem		*pcsAgpdGridItem;

	pcsAgpdGridItem = NULL;
	
	pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);

	if(pcsAgpdItemADChar)
	{
		INT32	lEquipIndex	= GetEquipIndexFromTable(lPart);
		if (lEquipIndex < 0)
			return NULL;

		pcsAgpdGridItem = m_pagpmGrid->GetItem( &pcsAgpdItemADChar->m_csEquipGrid, lEquipIndex );
	}

	return pcsAgpdGridItem;
}

BOOL AgpmItem::EquipItem(INT32 lCID, INT32 lIID, BOOL bCheckUseItem, BOOL bLogin)
{
	if (m_pagpmCharacter)
		return EquipItem(m_pagpmCharacter->GetCharacter(lCID), GetItem(lIID), bCheckUseItem, bLogin);

	return FALSE;
}

BOOL AgpmItem::EquipItem(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bCheckUseItem, BOOL bLogin)
{
	if (m_pagpmCharacter)
		return EquipItem(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, bCheckUseItem, bLogin);

	return FALSE;
}

BOOL AgpmItem::EquipItem(AgpdCharacter *pcsAgpdCharacter, INT32 lIID, BOOL bCheckUseItem, BOOL bLogin)
{
	return EquipItem(pcsAgpdCharacter, GetItem(lIID), bCheckUseItem, bLogin);
}

BOOL AgpmItem::EquipItem(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckUseItem, BOOL bLogin)
{
	if (!pcsAgpdItem || !pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::EquipItem() Error (1) !!!\n");
		return FALSE;
	}

	AgpdItemADChar			*pcsAgpdItemADChar;

	pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);

	if (!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::EquipItem() Error (2) !!!\n");
		return FALSE;
	}

	return EquipItem(pcsAgpdCharacter, pcsAgpdItemADChar, pcsAgpdItem, bCheckUseItem, bLogin);
}

BOOL AgpmItem::EquipItem(AgpdCharacter *pcsCharacter, AgpdItemADChar *pcsItemADChar, AgpdItem *pcsItem, BOOL bCheckUseItem, BOOL bLogin)
{
	if( !pcsCharacter || !pcsItemADChar || !pcsItem || !m_pagpmCharacter )	return FALSE;
	if( bCheckUseItem && !CheckUseItem( pcsCharacter, pcsItem ) )			return FALSE;

	// ���� �������� ����� ���� �� �� ����.
	if ( (pcsCharacter->m_bIsTrasform || pcsCharacter->m_bIsEvolution) && 
		((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_RIDE && 	// �����߿��� Ż���� Ż �� �־�� �Ѵ�
		!CheckUseItem(pcsCharacter, pcsItem))	// �����߿��� ���氡���� ����� ������ �� �־�� �Ѵ�
		return FALSE;		

	AgpdItemTemplateEquip* pcsAgpdItemTemplate = (AgpdItemTemplateEquip*)GetItemTemplate(pcsItem->m_lTID);
	if( !pcsAgpdItemTemplate || pcsAgpdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP )	return FALSE;

	if ( bLogin == FALSE ) // ���� ���ӽø� �����ϰ� üũ
	{
		// �������� �������� 0���ϸ� ����Ұ���
		INT32 lCurrentDurability = 0;
		m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lCurrentDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
		if ( lCurrentDurability <= 0 )
		{
			INT32 lEquipPart	= pcsAgpdItemTemplate->m_nPart;
			if ( AGPMITEM_PART_BODY <= lEquipPart  && lEquipPart <= AGPMITEM_PART_HAND_RIGHT ) 
			{
				return FALSE;
			}
		}
	}

	// �ش� Region���� �����Ҽ� ���� �������̸� ���� �Ұ����ϴ�.
	if(m_pagpmCharacter->IsPC(pcsCharacter) == TRUE)
	{
		if(!bLogin && IsEnableEquipItemInMyRegion(pcsCharacter, pcsItem) == FALSE)			
		{
			if(m_pagpmSystemMessage)
				m_pagpmSystemMessage->ProcessSystemMessage(0, AGPMSYSTEMMESSAGE_CODE_DISABLE_EQUIP_ITEM_THIS_REGION, -1, -1, pcsItem->m_pcsItemTemplate->m_szName, NULL, pcsCharacter);
			return FALSE;
		}
	}

	// �����߿��� Ż�� Ż �� ����. 2007.01.23. steeple
	if(((AgpdItemTemplateEquip*)pcsAgpdItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RIDE && m_pagpmCharacter->IsCombatMode(pcsCharacter)
		&& pcsAgpdItemTemplate->m_nPart != AGPMITEM_PART_V_RIDE /* �����߿��� �ƹ�Ÿ�� Ż���� Ż �� �ִ�. */)
	{
		if( m_pagpmSystemMessage )
			m_pagpmSystemMessage->ProcessSystemMessage( 0 ,AGPMSYSTEMMESSAGE_CODE_CANNOT_RIDE_WHILE_COMBAT, -1, -1, NULL, NULL, pcsCharacter);
		return FALSE;
	}

	if (AGPDCHAR_SPECIAL_STATUS_DISARMAMENT & pcsCharacter->m_ulSpecialStatus)
	{
		if( m_pagpmSystemMessage )
			m_pagpmSystemMessage->ProcessSystemMessage( 0 ,AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS, -1, -1, NULL, NULL, pcsCharacter);
		return FALSE;
	}

	AgpdGridItem* pcsAgpdGridItem = NULL;
	BOOL bIsAvatarItem = pcsAgpdItemTemplate->IsAvatarEquip();

	INT32 lEquipPart	= pcsAgpdItemTemplate->m_nPart;
	switch( lEquipPart )
	{
	case AGPMITEM_PART_ACCESSORY_RING1:
	case AGPMITEM_PART_ACCESSORY_RING2:
		{
			lEquipPart = AGPMITEM_PART_ACCESSORY_RING1;
			pcsAgpdGridItem = GetEquipItem( pcsCharacter, AGPMITEM_PART_ACCESSORY_RING1 );
			if ( pcsAgpdGridItem )
			{
				lEquipPart = AGPMITEM_PART_ACCESSORY_RING2;
				pcsAgpdGridItem = GetEquipItem( pcsCharacter, AGPMITEM_PART_ACCESSORY_RING2 );
			}
		}
		break;
	case AGPMITEM_PART_HAND_LEFT:
		{
			if( !bLogin && GetWeaponType(pcsItem->m_pcsItemTemplate) == (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
			{
				AgpdItem* pcsRightItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
				if( !pcsRightItem || !pcsRightItem->m_pcsItemTemplate ||
					GetWeaponType(pcsRightItem->m_pcsItemTemplate) != (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER )
				{
					// �̷��� ���� ����
					return FALSE;
				}
			}

			pcsAgpdGridItem = GetEquipItem( pcsCharacter, lEquipPart );
			if( !pcsAgpdGridItem )
			{
				AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
				if (pcsEquipItem && pcsEquipItem->m_pcsItemTemplate)
				{
					INT32	lRightHand	= 0;
					m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsEquipItem->m_pcsItemTemplate)->m_csFactor, &lRightHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

					INT32	lLeftHand	= 0;
					m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lLeftHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

					if( lLeftHand == 2 || lRightHand == 2 )
						pcsAgpdGridItem = pcsEquipItem->m_pcsGridItem;
				}
			}
			else
			{
				// ��չ������� ����.
				INT32	lHand	= 0;
				m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

				if (lHand == 2)
				{
					AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
					if (pcsEquipItem)
					{
						if( !bIsAvatarItem && !AddItemToInventory( pcsCharacter, pcsEquipItem ) )
						{
							OutputDebugString("AgpmItem::EquipItem() Error (88) !!!\n");
							return FALSE;
						}
					}
				}
			}
		}
		break;
	case AGPMITEM_PART_HAND_RIGHT:
		{
			// 2007.11.02. steeple
			// �޼տ� Dagger �� ��� �ִٸ�
			//AgpdItem* pcsLeftItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
			//if(pcsLeftItem && GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
			//{
			//	// ���� ��� ���Ⱑ Rapier �� �ƴ϶�� ������� �Ѵ�.
			//	if(GetWeaponType(pcsItem->m_pcsItemTemplate) != (INT32)AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
			//	{
			//		if(!AddItemToInventory(pcsCharacter, pcsLeftItem))
			//			return FALSE;
			//	}
			//}

			pcsAgpdGridItem = GetEquipItem(pcsCharacter, lEquipPart);

			// ��չ������� ����.
			INT32	lHand	= 0;
			m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

			// ����̶�� ������ �޼� ���� ������ �κ��� �ִ´�.
			if (lHand == 2)
			{
				AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
				if (pcsEquipItem)
				{
					if(!bIsAvatarItem && !AddItemToInventory(pcsCharacter, pcsEquipItem))
					{
						OutputDebugString("AgpmItem::EquipItem() Error (88) !!!\n");
						return FALSE;
					}
				}
			}
			else
			{
				AgpdItem	*pcsEquipItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
				if (pcsEquipItem)
				{
					INT32	lLeftHand	= 0;
					m_pagpmFactors->GetValue(&((AgpdItemTemplate *) pcsEquipItem->m_pcsItemTemplate)->m_csFactor, &lLeftHand, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_HAND);

					if (lLeftHand == 2)
					{
						if(!bIsAvatarItem && !AddItemToInventory(pcsCharacter, pcsEquipItem))
						{
							OutputDebugString("AgpmItem::EquipItem() Error (88) !!!\n");
							return FALSE;
						}
					}
				}
			}
		}
		break;
	default:
		pcsAgpdGridItem = GetEquipItem( pcsCharacter, lEquipPart );
		break;
	}
	
	if( pcsAgpdGridItem )
	{
		if( bIsAvatarItem )		return FALSE;
			
		AgpdItem* pcsAgpdItemOld = GetItem( pcsAgpdGridItem );
		if( !pcsAgpdItemOld )
		{
			OutputDebugString("AgpmItem::EquipItem() Error (6) !!!\n");
			return FALSE;
		}

		// �ִ� ���̶� ���� �������� ���̶� ���� ������ �˻��Ѵ�.
		// ���� ���̶�� ���� ���� �ʴ´�.
		if (pcsAgpdItemOld->m_lID != pcsItem->m_lID)
		{
			if(!AddItemToInventory(pcsCharacter, pcsAgpdItemOld))
			{
				OutputDebugString("AgpmItem::EquipItem() Error (8) !!!\n");
				return FALSE;
			}
		}
	}

	INT32	lEquipIndex	= GetEquipIndexFromTable(lEquipPart);
	if( lEquipIndex < 0 )		return FALSE;

	INT16 nLayer = m_pagpmGrid->GetLayerByIndex( &pcsItemADChar->m_csEquipGrid, lEquipIndex );
	INT16 nRow = m_pagpmGrid->GetRowByIndex( &pcsItemADChar->m_csEquipGrid, lEquipIndex );
	INT16 nColumn = m_pagpmGrid->GetColumnByIndex( &pcsItemADChar->m_csEquipGrid, lEquipIndex );

	// ������ ���¸� �����Ѵ�.
	//ChangeItemOwner(pcsItem, pcsCharacter);

//	pcsItem->m_eStatus											= AGPDITEM_STATUS_EQUIP;
//	pcsItemADChar->m_lEquipSlot[pcsAgpdItemTemplate->m_nPart]	= AGPDITEM_MAKE_ITEM_ID(pcsItem->m_lID, pcsItem->m_lTID);

//	m_pagpmGrid->Add( &pcsItemADChar->m_csEquipGrid, nLayer, nRow, nColumn, pcsAgpdGridItem, 1, 1 );
//	 Callback�� �ҷ��ش�.
//	EnumCallback(ITEM_CB_ID_CHAR_EQUIP, pcsItem, &bEmptyEquipSlot);

	BOOL	bResult	= FALSE;
	AgpdItemGridResult	eResult	= Insert(pcsItem, &pcsItemADChar->m_csEquipGrid, nLayer, nRow, nColumn);
	if( eResult == AGPDITEM_INSERT_SUCCESS )
	{
		RemoveStatus(pcsItem, AGPDITEM_STATUS_EQUIP);

		pcsItem->m_eStatus	= AGPDITEM_STATUS_EQUIP;

		EnumCallback(ITEM_CB_ID_CHAR_EQUIP, pcsItem, pcsCharacter);

		bResult	= TRUE;

		pcsItem->m_anPrevGridPos	= pcsItem->m_anGridPos;
	}

	// �������ο� ������� ������ �θ���. For Admin Client - 2004.04.01. steeple
	EnumCallback(ITEM_CB_ID_CHAR_EQUIP_FOR_ADMIN, pcsItem, NULL);

	return bResult;
}

/******************************************************************************
* Purpose : Unequip item
*
* 091202. Bob Jung
******************************************************************************/
BOOL AgpmItem::UnEquipItem(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem)
{
	if (!pcsAgpdItem)
		return FALSE;

	AgpdCharacter			*pcsAgpdCharacter = NULL;

	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::UnEquipItem() Error (1) !!!\n");
		return FALSE;
	}

	return UnEquipItem(pcsAgpdCharacter, pcsAgpdItem, bEquipDefaultItem);
}

/*AgpdItem *AgpmItem::FindCharacterDefaultItem(INT32 lCID, INT32 lTID)
{
	INT32 lIndex = 0;
	for(AgpdItem *pcsAgpdItem = GetItemSequence(&lIndex); pcsAgpdItem; pcsAgpdItem = GetItemSequence(&lIndex))
	{
		if((pcsAgpdItem->m_ulCID == lCID) && (pcsAgpdItem->m_lTID == lTID))
			return pcsAgpdItem;
	}

	return NULL;
}*/

BOOL AgpmItem::UnEquipItem(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem)
{
	if(!pcsCharacter || !pcsAgpdItem)
	{
		OutputDebugString("AgpmItem::UnEquipItem() Error (1) !!!\n");
		return FALSE;
	}

	// ���� �������� ����� �԰�, ���� �� �� ����.
	if ( (pcsCharacter->m_bIsTrasform || pcsCharacter->m_bIsEvolution) && 
		((AgpdItemTemplateEquip*)pcsAgpdItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_RIDE &&	// �����߿��� Ż���� Ÿ�ų� ������ �־�� �Ѵ�.
		!CheckUseItem(pcsCharacter, pcsAgpdItem))	// �����߿��� ���氡���� ����� ������ �� �־�� �Ѵ�
		return FALSE;

	AgpdItemTemplateEquip		*pcsItemTemplateEquip	= (AgpdItemTemplateEquip *)(GetItemTemplate(pcsAgpdItem->m_lTID));
	AgpdItemADChar				*pcsADChar				= GetADCharacter(pcsCharacter);

	INT16				nLayer, nRow, nColumn;

	if((!pcsItemTemplateEquip) || (!pcsADChar) || (pcsItemTemplateEquip->m_nType != AGPMITEM_TYPE_EQUIP))
	{
		OutputDebugString("AgpmItem::UnEquipItem() Error (3) !!!\n");
		return FALSE;
	}

	// ������ �������� ������ Rapier �̰�, �޼տ� ��� �ִ� �������� ���ؼ� Dagger ��� �����ش�. 2007.11.02. steeple
	//if(GetWeaponType(pcsAgpdItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
	//{
	//	AgpdItem* pcsLeftItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	//	if(pcsLeftItem && pcsLeftItem->m_pcsItemTemplate && pcsLeftItem->m_lID != pcsAgpdItem->m_lID &&
	//		GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
	//	{
	//		// �����Ǿ�� �̹� ��� �κ��丮�� �־����Ƿ� �ܰ��� ���� ���� 1ĭ�� �����ִ��� �˻��Ѵ�.
	//		if(m_pagpmGrid->GetEmpyGridCount(GetInventory(pcsCharacter)) < 1)
	//			return FALSE;

	//		if(!AddItemToInventory(pcsCharacter, pcsLeftItem))
	//				return FALSE;
	//	}
	//}

	nLayer = m_pagpmGrid->GetLayerByIndex( &pcsADChar->m_csEquipGrid, GetEquipIndexFromTable(pcsItemTemplateEquip->m_nPart) );
	nRow = m_pagpmGrid->GetRowByIndex( &pcsADChar->m_csEquipGrid, GetEquipIndexFromTable(pcsItemTemplateEquip->m_nPart) );
	nColumn = m_pagpmGrid->GetColumnByIndex( &pcsADChar->m_csEquipGrid, GetEquipIndexFromTable(pcsItemTemplateEquip->m_nPart) );

	if (!RemoveItemFromGrid(&pcsADChar->m_csEquipGrid, -1, pcsCharacter, pcsAgpdItem))
		return FALSE;

/*	m_pagpmGrid->Clear( &pcsADChar->m_csEquipGrid, nLayer, nRow, nColumn, 1, 1 );
	pcsAgpdItem->m_eStatus									= AGPDITEM_STATUS_NONE;
	*/

	return EnumCallback(ITEM_CB_ID_CHAR_UNEQUIP, pcsAgpdItem, &bEquipDefaultItem);
}

AgpdItem* AgpmItem::GetEquipWeapon(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdItem *pcsWeaponItem = NULL;

	if (pcsCharacter->m_bRidable)
	{
		pcsWeaponItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_LANCER);
		if (pcsWeaponItem)
			return pcsWeaponItem;
		else
			return NULL;
	}

	pcsWeaponItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if (pcsWeaponItem)
		return pcsWeaponItem;

	pcsWeaponItem	= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if (pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if (((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW ||
			((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON)
			return pcsWeaponItem;
	}

	return NULL;
}

BOOL AgpmItem::IsEquipWeapon(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdGridItem		*pcsWeaponGridItem	= GetEquipItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if (pcsWeaponGridItem)
		return TRUE;

	AgpdItem			*pcsWeaponItem		= GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if (pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if (((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW ||
			((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON
			|| ((AgpdItemTemplateEquipWeapon *) pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipOneHandSword(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(!pcsWeaponItem)
		return FALSE;

	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipOneHandAxe(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(!pcsWeaponItem)
		return FALSE;

	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_AXE)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipBlunt(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// �Ѽ����� ������� �𸣴� �Ѵ� �˻��ؾ� �Ѵ�.

	// ���� ������ ���� �˻�
	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			(((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER)
			)
			return TRUE;
	}

	// �޼� �˻�
	pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			(((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER)
			)
			return TRUE;
	}

	return FALSE;
}

BOOL	AgpmItem::IsEquipTwoHandBlunt(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// ��մ� ���ÿ� �˻�.
	// �������� �̵����� üũ��.

	AgpdItem* pcsWeaponItemRight = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	AgpdItem* pcsWeaponItemLeft = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);

	if(	pcsWeaponItemRight	&& pcsWeaponItemRight->m_pcsItemTemplate	&&
		pcsWeaponItemLeft	&& pcsWeaponItemLeft->m_pcsItemTemplate		)
	{
		AgpmItemEquipKind		eKindRight	= ((AgpdItemTemplateEquip*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeRight	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nWeaponType;
		AgpmItemEquipKind		eKindLeft	= ((AgpdItemTemplateEquip*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeLeft	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nWeaponType;

		if( eKindLeft	== AGPMITEM_EQUIP_KIND_WEAPON					&&
			eKindRight	== AGPMITEM_EQUIP_KIND_WEAPON					&&
			eTypeRight	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER	&&
			eTypeLeft	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER	)
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipTwoHandSlash(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			(((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SWORD ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_AXE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_POLEARM ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SCYTHE ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CLAW ||
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WING)
			)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipStaff(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STAFF)
			return TRUE;
	}

	return FALSE;
}

// 2004.12.14. steeple
BOOL AgpmItem::IsEquipWand(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WAND)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipBow(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdItem		*pcsWeapon	= GetEquipWeapon(pcsCharacter);
	if (pcsWeapon &&
		pcsWeapon->m_pcsItemTemplate &&
		((AgpdItemTemplateEquipWeapon *) pcsWeapon->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW)
		return TRUE;

	return FALSE;
}

BOOL AgpmItem::IsEquipCrossBow(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdItem		*pcsWeapon	= GetEquipWeapon(pcsCharacter);
	if (pcsWeapon &&
		pcsWeapon->m_pcsItemTemplate &&
		((AgpdItemTemplateEquipWeapon *) pcsWeapon->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW)
		return TRUE;

	return FALSE;
}

BOOL AgpmItem::IsEquipShield(AgpdCharacter* pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	// �޼� �˻�
	AgpdItem* pcsShield = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	if(pcsShield && pcsShield->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsShield->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipKatariya(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_KATARIYA)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipChakram(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHAKRAM)
			return TRUE;
	}

	return FALSE;
}

// 2005.11.02. steeple.
// ������ ��� ��� �ִ� �� �n��.
BOOL AgpmItem::IsEquipStandard(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_LANCER);
	if(pcsWeaponItem && pcsWeaponItem->m_pcsItemTemplate)
	{
		if(((AgpdItemTemplateEquip*)pcsWeaponItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON &&
			((AgpdItemTemplateEquipWeapon*)pcsWeaponItem->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD)
			return TRUE;
	}

	return FALSE;
}

// 2007.10.29. steeple
// ������ ���⸸ ��� �ִ��� üũ
BOOL AgpmItem::IsEquipRightHandOnly(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// �����տ� ���Ⱑ �ְ� �޼��� ����־�� �Ѵ�.
	AgpdItem* pcsWeaponItemRight = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);
	AgpdItem* pcsWeaponItemLeft = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);

	if(pcsWeaponItemRight && pcsWeaponItemRight->m_pcsItemTemplate && !pcsWeaponItemLeft)
	{
		AgpmItemEquipKind		eKindRight	= ((AgpdItemTemplateEquip*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeRight	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nWeaponType;
		if(	eKindRight	== AGPMITEM_EQUIP_KIND_WEAPON)//					&&
			//eTypeRight	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipCharon(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItemRight = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);

	if(pcsWeaponItemRight && pcsWeaponItemRight->m_pcsItemTemplate)
	{
		AgpmItemEquipKind		eKindRight	= ((AgpdItemTemplateEquip*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeRight	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemRight->m_pcsItemTemplate)->m_nWeaponType;
		if(	eKindRight	== AGPMITEM_EQUIP_KIND_WEAPON && eTypeRight	== AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmItem::IsEquipZenon(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItem* pcsWeaponItemLeft = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);

	if(pcsWeaponItemLeft && pcsWeaponItemLeft->m_pcsItemTemplate)
	{
		AgpmItemEquipKind		eKindLeft	= ((AgpdItemTemplateEquip*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nKind;
		AgpmItemEquipWeaponType eTypeLeft	= ((AgpdItemTemplateEquipWeapon*)pcsWeaponItemLeft->m_pcsItemTemplate)->m_nWeaponType;
		if(	eKindLeft	== AGPMITEM_EQUIP_KIND_WEAPON && eTypeLeft == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/******************************************************************************
* Purpose : Equip ��Ŷ �Ľ�.
*
* 091602. Bob Jung
******************************************************************************/
BOOL AgpmItem::ParseEquipPacket(PVOID pEquip, INT32 *plCID)
{
	if (!pEquip || !plCID)
		return FALSE;

	m_csPacketEquip.GetField(FALSE, pEquip, *((UINT16 *)(pEquip)),
		                     plCID);

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 060803. BOB
******************************************************************************/
AgpdItem *AgpmItem::GetEquipSlotItem(AgpdCharacter *pstAgpdCharacter, AgpmItemPart ePart)
{
	if (!pstAgpdCharacter)
		return NULL;

	AgpdGridItem	*pcsAgpdGridItem = GetEquipItem( pstAgpdCharacter, ePart );

	if (pcsAgpdGridItem)
	{
		if (pcsAgpdGridItem->GetParentBase())
			return (AgpdItem *) pcsAgpdGridItem->GetParentBase();
		else
			return GetItem( pcsAgpdGridItem->m_lItemID );
	}

	return NULL;
}

/******************************************************************************
* �����Ǿ� �ִ� �������� Durability�� ����� ��� �������� �Լ�
* Out	 :   INT32 (���� �����ϰ� �ִ� �������� ����)
			 pArrEquipItem (AgpdItem* ���� �迭)
* In	 :   pcsCharacter (�ش��ϴ� ĳ��������)
			 lMax (�迭�� �ִ밪)
******************************************************************************/

INT32 AgpmItem::GetEquipItems(AgpdCharacter *pcsCharacter, INT32* pArrEquipItem, INT32 lMax)
{
	if(!pcsCharacter || !pArrEquipItem)
		return NULL;

	INT32	  nEquipCount = 0;

	for(INT32 lEquipPart=AGPM_DURABILITY_EQUIP_PART_HEAD; lEquipPart < lMax; ++lEquipPart)
	{
		AgpdItem *EquipItem = GetEquipSlotItem(pcsCharacter, (AgpmItemPart)s_lBeDurabilityEquipItem[lEquipPart]);
		if(EquipItem)
		{
			pArrEquipItem[nEquipCount] = lEquipPart;
			nEquipCount++;
		}
	}

	return nEquipCount;
}

/******************************************************************************
******************************************************************************/

// 2007.08.07. steeple
BOOL AgpmItem::EquipAvatarItem(INT32 lCID, INT32 lID, BOOL bCheckUseItem)
{
	if(!lCID || !lID)
		return FALSE;

	return EquipAvatarItem(m_pagpmCharacter->GetCharacter(lCID), GetItem(lID), bCheckUseItem);
}

// 2007.08.07. steeple
BOOL AgpmItem::EquipAvatarItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsAgpdItem, BOOL bCheckUseItem)
{
	if(!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	return TRUE;
}

// 2007.08.07. steeple
BOOL AgpmItem::UnEquipAvatarItem(INT32 lCID, INT32 lID, BOOL bEquipDefaultItem)
{
	if(!lCID || !lID)
		return FALSE;

	return UnEquipAvatarItem(m_pagpmCharacter->GetCharacter(lCID), GetItem(lID), bEquipDefaultItem);
}

// 2007.08.07. steeple
BOOL AgpmItem::UnEquipAvatarItem(AgpdCharacter* pcsCharacter, AgpdItem* pcsAgpdItem, BOOL bEquipDefaultItem)
{
	if(!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	return TRUE;
}
