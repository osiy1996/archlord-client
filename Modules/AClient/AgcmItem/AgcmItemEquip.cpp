#include <AgcmItem/AgcmItem.h>
#include <AgcmCharacter/AgcCharacterUtil.h>
#include <rtpitexd.h>
#include <AgcmRide/AgcmRide.h>
#include <AgcmSkill/AgcmSkill.h>
#include <AgcmEventEffect/AgcmEventEffect.h>
#include <AgcmUIItem/AgcmUIItem.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmRender/AgcmRender.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <AgpmItemConvert/AgpmItemConvert.h>
#include <AgcmShadow2/AgcmShadow2.h>
#include <AgcmLODManager/AgcmLODManager.h>

#ifdef _DEBUG
extern DWORD	g_dwThreadID;
#endif

BOOL AgcmItem::ReleaseEquipItems( AgpdCharacter* pcsAgpdCharacter )
{
	AgpdItemADChar* pstItemADChar = m_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);
	AgpdGrid* pcsEquipGrid = &pstItemADChar->m_csEquipGrid;

	INT32 lIndex = 0;
	for( AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex) )
	{
		AgpdItem* pcsItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
		if ( pcsItem )
			ReleaseItemData( pcsItem );
	}

	return TRUE;
}

BOOL AgcmItem::RefreshEquipItems(AgpdCharacter *pcsAgpdCharacter)
{
	AgpdItemADChar* pstItemADChar = m_pcsAgpmItem->GetADCharacter(pcsAgpdCharacter);
	AgpdGrid* pcsEquipGrid = &pstItemADChar->m_csEquipGrid;

	INT32 lIndex = 0;
	for( AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence(pcsEquipGrid, &lIndex))
	{
		AgpdItem* pcsItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
		if( !pcsItem )		continue;

		ReleaseItemData(pcsItem);
		MakeItemClump(pcsItem);
		EquipItem(pcsAgpdCharacter, pcsItem);
	}

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgcdCharacter* pcdCharacter = pcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcdCharacter ) return FALSE;

	OnUpdateViewHelmet( pcdCharacter );
	return TRUE;
}

RpAtomic *AgcmItem::AttachNodeIndex( RpClump* pstBaseClump, RpHAnimHierarchy* pstBaseHierarchy, RpHAnimHierarchy* pstAttachHierarchy, RpAtomic* pstSrcAtomic, 
									RwInt32 lNodeID, RwInt32 lPartID, RwMatrix* pstTransform, RpHAnimHierarchy** ppstAttachedAtomicHierarchy, enumRenderType eForedRenderType )
{
	if (!pstBaseHierarchy)
		return NULL;
	LockFrame();

	RpAtomic* pstCloneAtomic = RpAtomicClone(pstSrcAtomic);
	if( !pstCloneAtomic )
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() Cloning Error!!!");

		UnlockFrame();
		return NULL;
	}

#ifdef _DEBUG
	pstCloneAtomic->pvApBase = RpAtomicGetClump(pstSrcAtomic) ? RpAtomicGetClump(pstSrcAtomic)->pvApBase : pstSrcAtomic->pvApBase;
#endif

	RwInt32 lNodeIndex = RpHAnimIDGetIndex( pstBaseHierarchy, lNodeID );
	if( lNodeIndex < 0 )
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() NodeID Error!!!");

		UnlockFrame();
		return NULL;
	}

	RpClumpAddAtomic( pstBaseClump, pstCloneAtomic );

	if( !pstBaseHierarchy->pNodeInfo[lNodeIndex].pFrame )
		RpHAnimHierarchyAttachFrameIndex( pstBaseHierarchy, lNodeIndex );

	RwFrame* pstCloneAtomicFrame	= NULL;

	if( pstAttachHierarchy )
	{
		RpClump *pstSrcClump				= RpAtomicGetClump(pstSrcAtomic);
		RwFrame *pstSrcClumpFrame			= RpClumpGetFrame(pstSrcClump);
		pstCloneAtomicFrame					= RwFrameCloneHierarchy(pstSrcClumpFrame);

		RpHAnimHierarchy *pstNewHierarchy	= RpHAnimHierarchyCreateFromHierarchy(pstAttachHierarchy, (RpHAnimHierarchyFlag)((pstAttachHierarchy)->flags), 0);

		RpHAnimFrameSetHierarchy(pstCloneAtomicFrame, pstNewHierarchy);
		RpHAnimHierarchyAttach(pstNewHierarchy);

		*(ppstAttachedAtomicHierarchy)		= pstNewHierarchy;

		TRACE("AgcmItem::AttachNodeIndex() Clump(Src %x, Base %x), Atomic(Src %x, Clone %x), Node(%d), Part(%d)\n", pstSrcClump, pstBaseClump, pstSrcAtomic, pstCloneAtomic, lNodeID, lPartID);
	}
	else
	{
		pstCloneAtomicFrame	= RwFrameCreate();
	}

	if (!pstCloneAtomicFrame)
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() CloneAtomic Frame Error!!!");

		UnlockFrame();
		return NULL;
	}

	RpAtomicSetFrame(pstCloneAtomic, pstCloneAtomicFrame);

	RwFrameAddChild(pstBaseHierarchy->pNodeInfo[lNodeIndex].pFrame, pstCloneAtomicFrame);
	if( pstTransform )
		RwFrameTransform(pstCloneAtomicFrame, pstTransform, rwCOMBINEREPLACE);

	UnlockFrame();

	if( lPartID )
		pstCloneAtomic->iPartID = lPartID;

	if(m_pWorld && (pstBaseClump->ulFlag & RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		if(m_pcsAgcmRender)
		{
			AgcdType* pTypeClone = AcuObject::GetAtomicTypeStruct(pstCloneAtomic);
			AgcdType* pTypeSrc = AcuObject::GetAtomicTypeStruct(pstSrcAtomic);
			ASSERT( pTypeClone && pTypeSrc );

			if( AcuObject::GetProperty(pTypeClone->eType) & ACUOBJECT_TYPE_RENDER_UDA && AcuObject::GetProperty(pTypeSrc->eType) & ACUOBJECT_TYPE_USE_ALPHAFUNC )
				if ( eForedRenderType != R_DEFAULT )
					pstCloneAtomic->stRenderInfo.renderType = eForedRenderType;

			m_pcsAgcmRender->AddAtomicToWorld(pstCloneAtomic, ONLY_ALPHA,AGCMRENDER_ADD_NONE, true);
		}
		else
			RpWorldAddAtomic( m_pWorld, pstCloneAtomic );
	}

	return pstCloneAtomic;
}

RpAtomic *AgcmItem::AttachItem(RpAtomic *atomic, AgcdItemTemplateEquipSet *pstAgcdItemTemplateEquipSet, BOOL bCheckPartID)
{
	if( !pstAgcdItemTemplateEquipSet->m_pstBaseClump )		return NULL;
	AgpdItemTemplate* ppdItemTemplate					= m_pcsAgpmItem->GetItemTemplate( pstAgcdItemTemplateEquipSet->m_nItemTID );

	PROFILE("AgcmItem::AttachItem");

	if ( bCheckPartID && atomic->iPartID != (RwInt32)(pstAgcdItemTemplateEquipSet->m_nPart ))
		return atomic;

	INT16 nKind = pstAgcdItemTemplateEquipSet->m_nKind;
	INT16 nPart	= pstAgcdItemTemplateEquipSet->m_nPart;

	RpAtomic* pstAtomic = NULL;
	RpAtomic* pstAtomic2 = NULL;

	switch( nKind )
	{
	case AGPMITEM_EQUIP_KIND_ARMOUR: pstAtomic = _AttachItemArmor( atomic, pstAgcdItemTemplateEquipSet, nPart );		break;
	case AGPMITEM_EQUIP_KIND_WEAPON:
		{
			AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon  = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplate;
			if( ppdItemTemplateEquipWeapon && 
				ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON &&
				nPart == AGPMITEM_PART_HAND_RIGHT )
			{
				if( atomic->iPartID != ( RwInt32 )( pstAgcdItemTemplateEquipSet->m_nPart ) )
				{
					// ����ó�� �߰� : �巡��ÿ¿� ����������� Max �󿡼� Export �ɶ� Arms �� �����Ǽ� Export �ȴ�.
					// ���� atomic->iPartID �� AGPMITEM_PART_ARMS �� ��쿡�� �������� �ʰ� ����ó���ϵ��� �Ѵ�.
					if( atomic->iPartID != AGPMITEM_PART_ARMS )
						return atomic;
				}

				pstAtomic = _AttachItemArmor( atomic, pstAgcdItemTemplateEquipSet, nPart );
			}
			else
			{
				pstAtomic = _AttachItemWeapon( atomic, pstAgcdItemTemplateEquipSet, nPart );
			}
		}
		break;

	case AGPMITEM_EQUIP_KIND_SHIELD:	pstAtomic = _AttachItemShield( atomic, pstAgcdItemTemplateEquipSet, nPart );	break;
	case AGPMITEM_EQUIP_KIND_RING:
	case AGPMITEM_EQUIP_KIND_NECKLACE:																					break;
	default:																											break;
	}

	++pstAgcdItemTemplateEquipSet->m_nCBCount;
	return atomic;
}

RpAtomic *AgcmItem::AttachCharonToSummoner( RpAtomic *atomic, AgcdItemTemplateEquipSet *pcdEquipSet )
{
	if( !pcdEquipSet || !pcdEquipSet->m_pstBaseClump ) return NULL;
	AgpdItemTemplate* ppdItemTemplate = m_pcsAgpmItem->GetItemTemplate( pcdEquipSet->m_nItemTID );

	// ī���� ������ �����ϱ� ������ ������ AGPMITEM_PART_BODY ���ٰ� �Ǵ�.
	RpAtomic* pAtomic = _AttachItemArmor( atomic, pcdEquipSet, AGPMITEM_PART_BODY );

	++pcdEquipSet->m_nCBCount;
	return atomic;
}

RpAtomic* AgcmItem::AttachItemCheckPartIDCB(RpAtomic* atomic, PVOID pData)
{
	AgcdItemTemplateEquipSet	*pstAgcdItemTemplateEquipSet	= (AgcdItemTemplateEquipSet*)(pData);
	AgcmItem					*pThis							= (AgcmItem *)(pstAgcdItemTemplateEquipSet->m_pvClass);

	return pThis->AttachItem(atomic, pstAgcdItemTemplateEquipSet, TRUE);
}

RpAtomic* AgcmItem::AttachItemCB(RpAtomic* atomic, PVOID pData)
{
	AgcdItemTemplateEquipSet	*pstAgcdItemTemplateEquipSet	= (AgcdItemTemplateEquipSet*)(pData);
	AgcmItem					*pThis							= (AgcmItem *)(pstAgcdItemTemplateEquipSet->m_pvClass);

	return pThis->AttachItem(atomic, pstAgcdItemTemplateEquipSet, FALSE);
}

RpAtomic* AgcmItem::AttachCharonToSummonerCB( RpAtomic* atomic, PVOID pData )
{
	AgcdItemTemplateEquipSet	*pcdEquipSet = ( AgcdItemTemplateEquipSet* )pData;
	AgcmItem					*pThis		 = ( AgcmItem* )pcdEquipSet->m_pvClass;

	return pThis->AttachCharonToSummoner( atomic, pcdEquipSet );
}

BOOL AgcmItem::DetachNodeIndex(RpClump *pstBaseClump, RpHAnimHierarchy *pstBaseHierarchy, RpAtomic *pstDetachAtomic, RwInt32 lNodeID, RwInt32 lNextNodeID)
{
	RwFrame *	pstFrame;

	if ( pstDetachAtomic->ulFlag & RWFLAG_RENDER_ADD )
	{
		if(m_pcsAgcmRender)
			m_pcsAgcmRender->RemoveAtomicFromWorld(pstDetachAtomic);
		else if (m_pWorld)
			RpWorldRemoveAtomic(m_pWorld, pstDetachAtomic);
	}

	LockFrame();

	RpClumpRemoveAtomic(pstBaseClump, pstDetachAtomic);
	pstFrame = RpAtomicGetFrame(pstDetachAtomic);
	RpAtomicSetFrame(pstDetachAtomic, NULL);

	if (pstFrame != RpClumpGetFrame(pstBaseClump))
		RwFrameDestroy(pstFrame);

	m_pcsAgcmResourceLoader->AddDestroyAtomic(pstDetachAtomic);

	UnlockFrame();

	return TRUE;
}

RpAtomic *AgcmItem::DetachItem(RpAtomic *atomic, AgcdItemTemplateEquipSet *pstAgcdItemTemplateEquipSet)
{
	PROFILE("AgcmItem::DetachItem");

	RwFrame						*pstFrame;
	RpClump						*pstCharacterClump				= pstAgcdItemTemplateEquipSet->m_pstBaseClump;
	RpHAnimHierarchy			*pstCharacterHierarchy			= pstAgcdItemTemplateEquipSet->m_pstBaseHierarchy;
	INT16						nKind							= pstAgcdItemTemplateEquipSet->m_nKind;
	INT16						nPart							= pstAgcdItemTemplateEquipSet->m_nPart;
	AgpdItemTemplate*			ppdItemTemplate					= m_pcsAgpmItem->GetItemTemplate( pstAgcdItemTemplateEquipSet->m_nItemTID );

	if(atomic->iPartID != (RwInt32)(pstAgcdItemTemplateEquipSet->m_nPart))
	{
		// ������ iPartID �� m_nPart �� ���� ������ ���������� ������..
		// �巡��ÿ¿� ������ ��� ��¿�� ���� ���� �ʰ� �ȴ�.. iPartID �� 4�� �ǰ� m_nPart �� 10�� �� ���̴�.
		// �׷��� �װ� ���ؼ� �Ʒ��� ����ó���� �߰�..

		// ���ø� ������ ����������.
		if( !ppdItemTemplate ) return atomic;

		// ���Ⱑ �ƴϸ� ����������.
		if( nKind != AGPMITEM_EQUIP_KIND_WEAPON ) return atomic;

		// ����Ÿ���� �Ѽ������� �ƴϸ� ����������.	
		AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplate;
		if( ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON &&
			ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON &&
			ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON &&
			ppdItemTemplateEquipWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON ) return atomic;

		// ���������� �������� �ƴϸ� ����������.
		if( nPart != AGPMITEM_PART_HAND_RIGHT ) return atomic;

		// ����Ϳ� ������ ���������� ARMS �� �ƴϸ� ����������. ������ �ƽ��󿡼� ARMS �� ���õǾ� Export �ȴ�.
		if( atomic->iPartID != AGPMITEM_PART_ARMS ) return atomic;
	}

	switch(nKind)
	{
	case AGPMITEM_EQUIP_KIND_ARMOUR:
		{
			if (atomic->ulFlag & RWFLAG_RENDER_ADD)
			{
#ifdef _DEBUG
				if (g_dwThreadID != GetCurrentThreadId())
					ASSERT(!"No proper thread process!!!");
#endif
				if(m_pcsAgcmRender)
					m_pcsAgcmRender->RemoveAtomicFromWorld(atomic);
				else if (m_pWorld)
					RpWorldRemoveAtomic(m_pWorld, atomic);
			}

			LockFrame();

			RpClumpRemoveAtomic(pstCharacterClump, atomic);

			pstFrame = RpAtomicGetFrame(atomic);
			RpAtomicSetFrame(atomic, NULL);

			if (pstFrame != RpClumpGetFrame(pstCharacterClump))
			{
				m_pcsAgcmResourceLoader->AddDestroyAtomic(atomic);
			}

			UnlockFrame();
		}
		break;

	case AGPMITEM_EQUIP_KIND_WEAPON:
		{
			AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(pstAgcdItemTemplateEquipSet->m_pvCustData);
			DetachNodeIndex(pstCharacterClump, pstCharacterHierarchy, atomic, AGCMITEM_NODE_ID_RIGHT_HAND, (pcsAgcdItemTemplate->m_bEquipTwoHands) ? AGCMITEM_NODE_ID_LEFT_HAND : 0);
		}
		break;

	case AGPMITEM_EQUIP_KIND_SHIELD:
		{
			DetachNodeIndex(pstCharacterClump, pstCharacterHierarchy, atomic, AGCMITEM_NODE_ID_LEFT_HAND);
		}
		break;

	case AGPMITEM_EQUIP_KIND_RING:
	case AGPMITEM_EQUIP_KIND_NECKLACE:
		{
		}
		break;

	default: // error
		break;
	}

	return atomic;
}

RpAtomic* AgcmItem::DetachItemCB(RpAtomic* atomic, PVOID pData)
{
	AgcdItemTemplateEquipSet	*pstAgcdItemTemplateEquipSet	= (AgcdItemTemplateEquipSet*)(pData);
	AgcmItem					*pThis							= (AgcmItem *)(pstAgcdItemTemplateEquipSet->m_pvClass);

	return pThis->DetachItem(atomic, pstAgcdItemTemplateEquipSet);
}

BOOL AgcmItem::AttachItem(INT32 lCharacterTemplateID, AgcdCharacter *pstAgcdCharacter, AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip, AgcdItem *pstAgcdItem, AgcdItemTemplate *pstAgcdItemTemplate, AuCharClassType eClassType)
{
	AgpdItemTemplateEquip* ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
	AgcdItemTemplate* pcdEquipItemTemplate = pstAgcdItemTemplate;
	AgcdItem* pcdEquipItem = pstAgcdItem;

	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetCharacter( pstAgcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eCharClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eCharClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	// �巡��ÿ��� ��� ���� ��ȭ���¿� ���� �����ؾ� �� �������� �޶�����.
	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// �ش��ϴ� ������ ���ų� �ش�Ǵ� ������ ���� �����̶� ���� �Ÿ� ���� ���ø��� ���
		ppdEquipItemTemplate = _GetDragonScionItemAgpdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
		if( !ppdEquipItemTemplate )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else if( ppdEquipItemTemplate->m_lID == pcsAgpdItemTemplateEquip->m_lID )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else
		{
			pcdEquipItemTemplate = _GetDragonScionItemAgcdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
			pcdEquipItem = _GetDragonScionItemAgcdItem( ppdEquipItemTemplate, ppdCharacter, eClassType );
		}

		// ��������� ����ó��
		if( pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon = ( AgpdItemTemplateEquipWeapon* )pcsAgpdItemTemplateEquip;

			//// ĳ���Ͱ� ���ӳ��̰� ������ ������ ī���� ��� ����ó�� ��ƾ�� ź��.
			//INT32 nWeaponType = ppdItemTemplateEquipWeapon->m_nWeaponType;
			//if( eClassType == AUCHARCLASS_TYPE_MAGE &&
			//	( nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON || nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON ) )
			//{
			//	return OnAttachCharonToSummoner( ppdCharacter, pstAgcdCharacter, ppdEquipItemTemplate, pcdEquipItemTemplate, pcdEquipItem );
			//}
		}
	}

	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
	if( pcmUIItem )
	{
		AgpdItem* ppdItem = GetItem( pcdEquipItem );
		if( !pcmUIItem->IsUsableItem( ppdCharacter, ppdItem, eClassType ) )
		{
			// ����Ұ����� �������̶�� ������ �𵨸��۾��� ó������ �ʴ´�.
			return FALSE;
		}
	}

	// �Ӹ�ī���� ��ó��..
	// ���� �������� �󱼰� �� �پ��ִ��� Ȯ���ؾ��Ѵ�.
	OnUpdateHairAndFace( ppdEquipItemTemplate, pstAgcdCharacter, TRUE );

	// EquipItem()�� ����ȣ�� �� ��� �ʱ�ȭ���� �ʴ´�.
	RemoveAllAttachedAtomics( &pcdEquipItem->m_csAttachedAtomics );

	AgcdItemTemplateEquipSet stAgcdItemTemplateEquipSet;
	memset(&stAgcdItemTemplateEquipSet, 0, sizeof(AgcdItemTemplateEquipSet));
	stAgcdItemTemplateEquipSet.m_pvClass				= this;
	stAgcdItemTemplateEquipSet.m_nCBCount				= 0;
	stAgcdItemTemplateEquipSet.m_nPart					= ppdEquipItemTemplate->GetPartIndex();
	stAgcdItemTemplateEquipSet.m_nKind					= ppdEquipItemTemplate->m_nKind;
	stAgcdItemTemplateEquipSet.m_pstBaseClump			= pstAgcdCharacter->m_pClump;
	stAgcdItemTemplateEquipSet.m_pstBaseHierarchy		= pstAgcdCharacter->m_pInHierarchy;
	stAgcdItemTemplateEquipSet.m_pstEquipmentHierarchy	= pcdEquipItem->m_pstHierarchy;
	stAgcdItemTemplateEquipSet.m_pcsAttachedAtomics		= &pcdEquipItem->m_csAttachedAtomics;
	stAgcdItemTemplateEquipSet.m_pvCustData				= (PVOID)(pcdEquipItemTemplate);
	stAgcdItemTemplateEquipSet.m_pstTransformInfo		= pcdEquipItemTemplate->m_pItemTransformInfo->GetInfo( lCharacterTemplateID );
	stAgcdItemTemplateEquipSet.m_nItemTID				= ppdEquipItemTemplate->m_lID;

	if( !pcdEquipItem->m_pstClump ) return FALSE;

	RpClumpForAllAtomics( pcdEquipItem->m_pstClump,
		(ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) ? (AttachItemCheckPartIDCB) : (AttachItemCB),
		(PVOID)(&stAgcdItemTemplateEquipSet) );

	if (	ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR && (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY)	)
	{
		AttachedAtomicList* pcsCurrent = pcdEquipItem->m_csAttachedAtomics.pcsList;
		while (pcsCurrent)
		{
			m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
			pcsCurrent	= pcsCurrent->m_pcsNext;
		}
	}

	if( ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
	{
		AgpdItemTemplateEquipWeapon* ppdITemTemplateWeapon = ( AgpdItemTemplateEquipWeapon* )ppdEquipItemTemplate;
		if( ppdITemTemplateWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON &&
			ppdITemTemplateWeapon->m_nPart == AGPMITEM_PART_HAND_RIGHT )
		{
			AttachedAtomicList* pcsCurrent = pcdEquipItem->m_csAttachedAtomics.pcsList;
			while (pcsCurrent)
			{
				m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
				pcsCurrent	= pcsCurrent->m_pcsNext;
			}
		}
	}

	// Light ����..
	if( ppdEquipItemTemplate->m_lightInfo.GetType() )
	{		
		AuCharacterLightInfo*	pLightInfo = pstAgcdCharacter->GetLightInfo( ppdEquipItemTemplate->GetPartIndex() );
		if( pLightInfo )
			*pLightInfo = ppdEquipItemTemplate->m_lightInfo;
	}

	return TRUE;
}

BOOL AgcmItem::DetachItem(AgcdCharacter *pstAgcdCharacter, AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip, AgcdItem *pstAgcdItem, AgcdItemTemplate *pstAgcdItemTemplate, AuCharClassType eClassType )
{
	if (!m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter))
		return TRUE;

	AgpdItemTemplateEquip* ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
	AgcdItemTemplate* pcdEquipItemTemplate = pstAgcdItemTemplate;
	AgcdItem* pcdEquipItem = pstAgcdItem;

	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetCharacter( pstAgcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// �巡��ÿ��� ��� ���� ��ȭ���¿� ���� �����ؾ� �� �������� �޶�����.
	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// �ش��ϴ� ������ ���ų� �ش�Ǵ� ������ ���� �����̶� ���� �Ÿ� ���� ���ø��� ���
		ppdEquipItemTemplate = _GetDragonScionItemAgpdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
		if( !ppdEquipItemTemplate )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else if( ppdEquipItemTemplate->m_lID == pcsAgpdItemTemplateEquip->m_lID )
		{
			ppdEquipItemTemplate = pcsAgpdItemTemplateEquip;
		}
		else
		{
			pcdEquipItemTemplate = _GetDragonScionItemAgcdTemplate( ppdEquipItemTemplate, ppdCharacter, eClassType );
			pcdEquipItem = _GetDragonScionItemAgcdItem( ppdEquipItemTemplate, ppdCharacter, eClassType );
		}
	}

	AgcdItemTemplateEquipSet stAgcdItemTemplateEquipSet;
	memset(&stAgcdItemTemplateEquipSet, 0, sizeof(AgcdItemTemplateEquipSet));

	stAgcdItemTemplateEquipSet.m_pvClass					= this;
	stAgcdItemTemplateEquipSet.m_nCBCount					= 0;
	stAgcdItemTemplateEquipSet.m_nKind						= ppdEquipItemTemplate->m_nKind;
	stAgcdItemTemplateEquipSet.m_nPart						= ppdEquipItemTemplate->GetPartIndex();
	stAgcdItemTemplateEquipSet.m_pstBaseClump				= pstAgcdCharacter->m_pClump;
	stAgcdItemTemplateEquipSet.m_pstBaseHierarchy			= pstAgcdCharacter->m_pInHierarchy;
	stAgcdItemTemplateEquipSet.m_nItemTID					= ppdEquipItemTemplate->m_lID;

	if (ppdEquipItemTemplate->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR)
	{
		stAgcdItemTemplateEquipSet.m_pvCustData = NULL;
	}
	else
	{
		if (!pcdEquipItemTemplate)
			pcdEquipItemTemplate = GetTemplateData((AgpdItemTemplate *)(ppdEquipItemTemplate));

		stAgcdItemTemplateEquipSet.m_pvCustData = (PVOID)(pcdEquipItemTemplate);
	}

	if( !pstAgcdCharacter->m_pClump )
	{
		ASSERT(!"AgcmItem::DetachItem() Error- pstAgcdCharacter->m_pClump!!!");
		return TRUE;
	}

	RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, DetachItemCB, (PVOID)(&stAgcdItemTemplateEquipSet));

	RemoveAllAttachedAtomics(& pstAgcdItem->m_csAttachedAtomics );

	AuCharacterLightInfo* pLightInfo = pstAgcdCharacter->GetLightInfo( ppdEquipItemTemplate->GetPartIndex() );
	if( pLightInfo )
		pLightInfo->Clear();

	return TRUE;
}

BOOL AgcmItem::UnEquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem, BOOL bViewHelmet )
{
	PROFILE("AgcmItem::UnEquipItem");

	if(!pstAgpdCharacter)
		return FALSE;

	// �������̸� �����Ѵ�.
	if (pstAgpdCharacter->m_bIsTrasform)
		return TRUE;

	// ���ڵ� ��ų�� ���ߴ��� �ִ� ��쿡�� ������ �������� �Ұ��� �ϴ�.
	if( pstAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) return TRUE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	return _UnEquipItem( pstAgpdCharacter, pcsAgpdItem, bEquipDefaultItem, bViewHelmet );
}

BOOL AgcmItem::ItemUnequipCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::ItemUnequipCB");
	if((!pData) || (!pClass) || (!pCustData)) return FALSE;

	AgpdItem				*pcsAgpdItem				= (AgpdItem *)(pData);
	AgcmItem				*pThis						= (AgcmItem *)(pClass);

	AgpdCharacter			*pstAgpdCharacter			= pcsAgpdItem->m_pcsCharacter;
	if(!pstAgpdCharacter) return FALSE;

	AgcdCharacter			*pstAgcdCharacter			= pThis->m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);

	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);

	// �ش� �ɸ��Ͱ� Remove�� ���̸� �ϰ͵� ���Ѵ�.
	if (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED)
		return TRUE;

	pThis->UnEquipItem(pstAgpdCharacter, pcsAgpdItem, TRUE);
	pThis->RefreshEquipItems( pstAgpdCharacter );
	return TRUE;
}

BOOL AgcmItem::EquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckDefaultArmour, BOOL bViewHelmet )
{
	PROFILE("AgcmItem::EquipItem");
	TRACE("AgcmItem::EquipItem() CHR : %s , Item : %s\n", pstAgpdCharacter->m_szID, pcsAgpdItem->m_pcsItemTemplate ? ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_szName : "NULL");

	if( pstAgpdCharacter->m_bIsTrasform )		return TRUE;

	// ���ڵ� ��ų�� ���ߴ��� �ִ� ��쿡�� ������ ����Ұ��� �ϴ�.
	if( pstAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISARMAMENT ) return TRUE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( pstAgpdCharacter );

		// �ִϸ��̼� Ÿ�Լ��ø� ���ְ� �Ѿ��
		SetEquipAnimType( pstAgpdCharacter, pcdCharacter );
		return TRUE;
	}

	return _EquipItem( pstAgpdCharacter, pcsAgpdItem, bCheckDefaultArmour, bViewHelmet );
}

BOOL AgcmItem::ItemEquipCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmItem::ItemEquipCB");
	if((!pData) || (!pClass)) return FALSE;

	AgpdItem				*pcsAgpdItem				= (AgpdItem *)(pData);
	AgcmItem				*pThis						= (AgcmItem *)(pClass);
	AgcdItem				*pstAgcdItem				= pThis->GetItemData(pcsAgpdItem);
	AgpdCharacter			*pstAgpdCharacter			= pcsAgpdItem->m_pcsCharacter;
	if(!pstAgpdCharacter) return FALSE;

	pThis->ReleaseItemData( pcsAgpdItem );		// �ϴ� ItemClump �� Release�Ѵ�.

	AgcdCharacter* pstAgcdCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData( pstAgpdCharacter);

	// Remove�� �ɸ��Ͱ� ���⸦ Ÿ�� �ȵȴ�.
	ASSERT(!(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED));
	if (pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED)
		return FALSE;

	pThis->m_pcsAgcmResourceLoader->RemoveLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);

	return pThis->m_pcsAgcmResourceLoader->AddLoadEntry(pThis->m_lLoaderSetID, (PVOID) pcsAgpdItem, (PVOID) NULL);
}

BOOL AgcmItem::SetEquipAnimType( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter )
{
	// ���� �ִϸ��̼� Ÿ�� �ӽ� ����
	INT32 nPrevAnimType = pcdCharacter->m_lCurAnimType2;

	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );

	AgpdItem* ppdWeaponLeft = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_LEFT );
	AgpdItem* ppdWeaponRight = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_RIGHT );

	if( ppdCharacter->m_bRidable )
	{		
		// �¸����̶��
		AgpdItem* ppdWeaponRide = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_LANCER );
		if( ppdWeaponRide )
		{
			// ������ �����ϰ� ������
			return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE );
		}
	}

	AgpdItemTemplateEquipWeapon* ppdWeaponTemplateLeft = ppdWeaponLeft ? ( AgpdItemTemplateEquipWeapon* )ppdWeaponLeft->m_pcsItemTemplate : NULL;
	AgpdItemTemplateEquipWeapon* ppdWeaponTemplateRight = ppdWeaponRight ? ( AgpdItemTemplateEquipWeapon* )ppdWeaponRight->m_pcsItemTemplate : NULL;
	if( !ppdWeaponTemplateLeft && !ppdWeaponTemplateRight )
	{
		// ��ո�� ���⸦ �������� ���� ��� �������� ���� ������ �����Ѵ�.
		return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
	}

	// �����տ� ������ ���⸦ �������� ó���Ѵ�.
	if( ppdWeaponTemplateRight )
	{
		// �޼տ��� ���⸦ ������ ���.. �� �̵����� ���.. ���밡���Ѱ� �Ѽմܰ˻��̴�.
		if( ppdWeaponTemplateLeft && ppdWeaponTemplateLeft->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER )
		{
			// �����Ǿ� + �ܰ� �̵����� ��� �����ù�Ŭ�����Ը� ���ȴ�.
			if( ppdWeaponTemplateRight->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER )
			{
				if( eRaceType == AURACE_TYPE_MOONELF && eClassType == AUCHARCLASS_TYPE_KNIGHT )
				{
					return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_RAPIERDAGGER );
				}
			}
		}

		// �巡��ÿ� �����̾��� ���
		if( eRaceType == AURACE_TYPE_DRAGONSCION && ( eClassType == AUCHARCLASS_TYPE_KNIGHT || eClassType == AUCHARCLASS_TYPE_MAGE ) )
		{
			// �޼տ� ���� ���� �ְ�.. ī���� ��� ī������ ����
			if( ppdWeaponTemplateLeft && ppdWeaponTemplateLeft->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON )
			{
				return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON );
			}

			// �޼տ� �ϰ͵� �ȳ��� ������ �Ǽ��̴�.
			if( !ppdWeaponTemplateLeft )
			{
				// �����̾ �޼տ��ٰ� ī�и��� �ٸ��� ������ �ִ��� ������ �ϴ� �������� ���Ѱɷ� ó��
				return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
			}
		}

		// �޼չ��⸦ �Ű澲�� �ʴ� ��� �����տ� ������ ���� Ÿ�Կ� ���� �ִϸ��̼��� �����Ѵ�.
		return _SetEquipAnimType( pcdCharacter, ppdWeaponTemplateRight->m_nWeaponType );
	}
	else
	{
		// �����տ� �����Ѱ� ���µ� �޼տ� ������ ���� �������� ó���ؾ� �ϴ� ����..
		// �巡��ÿ� �����̾��� ��� ī���� �޼������̹Ƿ� �޼տ� ī���� ���� �ִ��� �˻��ؾ� �Ѵ�.
		if( ppdWeaponTemplateLeft )
		{
			if( eRaceType == AURACE_TYPE_DRAGONSCION && ( eClassType == AUCHARCLASS_TYPE_KNIGHT || eClassType == AUCHARCLASS_TYPE_MAGE ) )
			{
				// �巡��ÿ¿��ٰ� �����̾��� ���
				if( ppdWeaponTemplateLeft->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON )
				{
					// ī���̶�� �����̴�.
					return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON );
				}
				else
				{
					// �����̾ �޼տ��ٰ� ī�и��� �ٸ��� ������ �ִ��� ������ �ϴ� �������� ���Ѱɷ� ó��
					return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
				}
			}
			else
			{
				//// �����̾� �ƴ� �ѵ��� �޼տ� �� ���簣�� �Ǽ��̴�.
				//return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
				// �޼տ� �� ���⿡ ���� �ִϸ��̼��� �����Ѵ�.
				return _SetEquipAnimType( pcdCharacter, ppdWeaponTemplateLeft->m_nWeaponType );
			}
		}
		else
		{
			// �޼տ��� ���� ������ ����.. ���������� ���Ѱ��̴�.
			return _SetEquipAnimType( pcdCharacter, AGPMITEM_EQUIP_WEAPON_TYPE_NONE );
		}
	}

	// �ִϸ��̼� Ÿ���� ����Ǿ����� ���� �ִϸ��̼��� Ÿ���� �ٲ㼭 �ٽ� Ʋ���ش�.
	if( nPrevAnimType != pcdCharacter->m_lCurAnimType2 )
	{
		if( AGCMCHAR_ANIM_TYPE_WAIT <= pcdCharacter->m_eCurAnimType && pcdCharacter->m_eCurAnimType < AGCMCHAR_MAX_ANIM_TYPE )
		{
			return m_pcsAgcmCharacter->StartAnimation( ppdCharacter, pcdCharacter, pcdCharacter->m_eCurAnimType );
		}
	}

	return TRUE;
}

VOID AgcmItem::ViewHelmet( AgcdCharacter* pstAgcdCharacter, BOOL bViewHelmet )
{
	//AgpdCharacter*		pcsAgpdCharacter= m_pcsAgcmCharacter->GetCharacter( pstAgcdCharacter );
	//AgpdItemADChar*		pstItemADChar	= m_pcsAgpmItem->GetADCharacter( pcsAgpdCharacter );
	//AgpdGrid*			pcsEquipGrid	= &pstItemADChar->m_csEquipGrid;
	//AgpdGridItem*		pcsGridItem;
	//INT32				nIndex;
	//AgpdItem*			pcsAgpdItem;

	//if( pstAgcdCharacter->m_bEquipHelmet && !pstAgcdCharacter->m_bViewHelmet && bViewHelmet ) {
	//	pstAgcdCharacter->m_bViewHelmet = bViewHelmet;

	//	nIndex = 0;
	//	for( pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ) )
	//	{
	//		pcsAgpdItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
	//		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)( pcsAgpdItem->m_pcsItemTemplate );

	//		if( pcsAgpdItem && pcsAgpdItemTemplateEquip &&
	//			pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD &&
	//			!pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	//		{
	//			ReleaseItemData( pcsAgpdItem );
	//			MakeItemClump( pcsAgpdItem );
	//			EquipItem( pcsAgpdCharacter, pcsAgpdItem, TRUE, TRUE );
	//		}
	//	}
	//	return;
	//}

	//if( pstAgcdCharacter->m_bEquipHelmet && pstAgcdCharacter->m_bViewHelmet && !bViewHelmet ) {
	//	pstAgcdCharacter->m_bViewHelmet = bViewHelmet;

	//	nIndex = 0;
	//	for( pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ); pcsGridItem; pcsGridItem = m_pcsAgpmGrid->GetItemSequence( pcsEquipGrid, &nIndex ) )
	//	{
	//		pcsAgpdItem = m_pcsAgpmItem->GetItem( pcsGridItem->m_lItemID );
	//		AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)( pcsAgpdItem->m_pcsItemTemplate );

	//		if( pcsAgpdItem && pcsAgpdItemTemplateEquip &&
	//			pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD &&
	//			!pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	//		{
	//			UnEquipItem( pcsAgpdCharacter, pcsAgpdItem, FALSE, TRUE );
	//		}
	//	}
	//	return;
	//}

	pstAgcdCharacter->m_bViewHelmet = bViewHelmet;
	OnUpdateViewHelmet( pstAgcdCharacter );
}

int AgcmItem::GetAnimDefaultRide( int lAnimType2 ) const
{
	switch( lAnimType2 )
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_DEFAULT;
	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_DEFAULT_RIDE;
	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE;
	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_DEFAULT_RIDE;
	case AGCMCHAR_AT2_ARCHLORD:
		return AGCMCHAR_AT2_ARCHLORD_DEFAULT_RIDE;
	}

	return -1;
}

int AgcmItem::GetAnimWeaponRide( int lAnimType2 ) const
{
	switch( lAnimType2 )
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_DEFAULT;
	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_WEAPON_RIDE;
	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_WEAPON_RIDE;
	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_WEAPON_RIDE;
	case AGCMCHAR_AT2_ARCHLORD:
		return AGCMCHAR_AT2_ARCHLORD_DEFAULT_RIDE;
	}

	return -1;
}

int AgcmItem::GetAnimStandardRide( int lAnimType2 ) const
{
	switch( lAnimType2 )
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return AGCMCHAR_AT2_BASE_DEFAULT;
	case AGCMCHAR_AT2_WARRIOR:
		return AGCMCHAR_AT2_WARRIR_STANDARD_RIDE;
	case AGCMCHAR_AT2_ARCHER:
		return AGCMCHAR_AT2_ARCHER_STANDARD_RIDE;
	case AGCMCHAR_AT2_WIZARD:
		return AGCMCHAR_AT2_WIZARD_STANDARD_RIDE;
	case AGCMCHAR_AT2_ARCHLORD:
		return AGCMCHAR_AT2_ARCHLORD_STANDARD_RIDE;
	}

	return -1;
}

BOOL AgcmItem::CBUpdateFactorDurability(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmItem	*pThis		= (AgcmItem *)		pClass;
	AgpdFactor	*pcsFactor	= (AgpdFactor *)	pData;

	INT32	lValue	= 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lValue, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_OWNER);
	if( ((ApBase *)lValue)->m_eType != APBASE_TYPE_ITEM )
		return TRUE;

	AgpdItem* pcsItem = (AgpdItem *)	lValue;
	if( !pThis->m_pcsAgpmCharacter->IsPC(pcsItem->m_pcsCharacter) )		return TRUE;
	if( pcsItem->m_eStatus != AGPDITEM_STATUS_EQUIP )					return TRUE;

	INT32	lDurability	= 0;
	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

	if (lDurability <= 0)
		pThis->UnEquipItem(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsItem, TRUE);

	return TRUE;
}

BOOL AgcmItem::CBUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmItem* pThis	= (AgcmItem *)		pClass;
	AgpdItem* pcsItem = (AgpdItem *)		pData;

	if( !pThis->m_pcsAgpmCharacter->IsPC( pcsItem->m_pcsCharacter ) )
		return TRUE;
	if( pcsItem->m_eStatus != AGPDITEM_STATUS_EQUIP &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsItem->m_eStatus != AGPDITEM_STATUS_NOTSETTING)
		return TRUE;

	// �������� ���� ����, ����̴� ������ �˻翡�� ���ܵȴ�.
	if( pThis->IsNoneDurabilityItem( pcsItem ) ) return TRUE;

	INT32	lDurability	= 0;
	pThis->m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	if ( lDurability <= 0 )
	{
		pThis->UnEquipItem(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsItem, TRUE);

		// Equip Item (Weapon & Armour & Shield) �� ��� Durability�� 0 ���Ϸ� ���������� �˻��Ѵ�.
		////////////////////////////////////////////////////////////////////////////////////////////
		if (!(pcsItem->m_pcsItemTemplate &&
			((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP &&
			(((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR ||
			((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON ||
			((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD)))
			pcsItem->m_pcsGridItem->SetDurabilityZero( FALSE );
		else
			pcsItem->m_pcsGridItem->SetDurabilityZero( TRUE );
	}
	else
		pcsItem->m_pcsGridItem->SetDurabilityZero( FALSE );

	return TRUE;
}

BOOL AgcmItem::OnEquipItemNecklace( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AgcdItem* pcdItem, AuCharClassType eClassType )
{
	AgcmEventEffect* pcmEventEffect = ( AgcmEventEffect* )GetModule( "AgcmEventEffect" );
	if( !pcmEventEffect ) return FALSE;

	BOOL bIsCloak = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );
	BOOL bIsWing = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );
	BOOL bIsWing2 = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );

	AuRaceType eRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = AUCHARCLASS_TYPE_NONE;

	if( eClassType == AUCHARCLASS_TYPE_NONE )
	{
		eClass = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	}
	else
	{
		eClass = eClassType;
	}

	// ���� �������ΰ�?
	if( bIsCloak )
	{
		// �ߵ��ؾ��� ����Ʈ Ÿ���� �����ΰ�?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );

		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		if( !pcdEffectData ) return FALSE;

		INT32 nEffectID = m_ItemEffectFinder.GetCharEffectID(nEffectIndex, eRace, eClass);
		if( nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
		pcmEventEffect->SetCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}

	// �����ΰ�
	if( bIsWing || bIsWing2 )
	{
		// �ߵ��ؾ��� ����Ʈ Ÿ���� �����ΰ�?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );

		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		if( !pcdEffectData )
		{
			nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );
			pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
			if( !pcdEffectData ) return FALSE;
		}

		INT32 nEffectID = m_ItemEffectFinder.GetCharEffectID(nEffectIndex, eRace, eClass);
		if( nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
		pcmEventEffect->SetCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}

	return TRUE;
}

BOOL AgcmItem::OnUnEquipItemNecklace( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AgcdItem* pcdItem, AuCharClassType eClassType )
{
	if( !ppdCharacter || !ppdItem || !pcdItem || !m_pcsAgpmItemConvert ) return FALSE;

	AgcmEventEffect* pcmEventEffect = ( AgcmEventEffect* )GetModule( "AgcmEventEffect" );
	if( !pcmEventEffect ) return FALSE;

	BOOL bIsCloak = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );
	BOOL bIsWing = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );
	BOOL bIsWing2 = m_pcsAgpmItemConvert->IsConvertedAttribute( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );

	AuRaceType eRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = AUCHARCLASS_TYPE_NONE;

	if( eClassType == AUCHARCLASS_TYPE_NONE )
	{
		eClass = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	}
	else
	{
		eClass = eClassType;
	}

	// ���� �������ΰ�?
	if( bIsCloak )
	{
		// �ߵ��ؾ��� ����Ʈ Ÿ���� �����ΰ�?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_ALL_REGISTANCE_WITH_CLOAK );

		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		INT32 nEffectID = m_ItemEffectFinder.GetCharEffectID(nEffectIndex, eRace, eClass);
		if( !pcdEffectData || nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}

	// �����ΰ�
	if( bIsWing || bIsWing2 )
	{
		// �ߵ��ؾ��� ����Ʈ Ÿ���� �����ΰ�?
		INT32 nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING );

		AgcdUseEffectSetData* pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
		if( !pcdEffectData )
		{
			nEffectIndex = m_pcsAgpmItemConvert->GetConvertedExtraType( ppdItem, AGPMITEM_RUNE_ATTR_ADD_MOVE_SPEED_WITH_WING2 );
			pcdEffectData = pcmEventEffect->m_csAgcmEventEffectData.SearchCommonCharEffectDataByID( nEffectIndex );
			if( !pcdEffectData ) return FALSE;
		}

		INT32 nEffectID = m_ItemEffectFinder.GetCharEffectID(nEffectIndex, eRace, eClass);
		if( nEffectID < 0 ) return FALSE;

		pcmEventEffect->RemoveCommonCharacterEffect( ppdCharacter, ( eAgcmEventEffectDataCommonCharType )nEffectID );
	}

	return TRUE;
}

BOOL AgcmItem::OnAttachCharonToSummoner( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter, AgpdItemTemplateEquip* ppdItemTemplateEquip, AgcdItemTemplate* pcdItemTemplate, AgcdItem* pcdItem )
{
	// ���Ⱑ �ƴϸ� �н�..
	if( ppdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON ) return FALSE;

	// ī���� �ƴϸ� �н�..
	AgpdItemTemplateEquipWeapon* ppdITemTemplateWeapon = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplateEquip;
	if( ppdITemTemplateWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON &&
		ppdITemTemplateWeapon->m_nWeaponType != AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON ) return FALSE;

	// EquipItem()�� ����ȣ�� �� ��� �ʱ�ȭ���� �ʴ´�.
	RemoveAllAttachedAtomics( &pcdItem->m_csAttachedAtomics );

	AgcdItemTemplateEquipSet ItemTemplateEquipSet;
	memset( &ItemTemplateEquipSet, 0, sizeof( AgcdItemTemplateEquipSet ) );

	ItemTemplateEquipSet.m_pvClass					= this;
	ItemTemplateEquipSet.m_nCBCount					= 0;
	ItemTemplateEquipSet.m_nPart					= AGPMITEM_PART_BODY;				// ī���� ������ �����ϹǷ� ������ BODY ��
	ItemTemplateEquipSet.m_nKind					= ppdItemTemplateEquip->m_nKind;
	ItemTemplateEquipSet.m_pstBaseClump				= pcdCharacter->m_pClump;
	ItemTemplateEquipSet.m_pstBaseHierarchy			= pcdCharacter->m_pInHierarchy;
	ItemTemplateEquipSet.m_pstEquipmentHierarchy	= pcdItem->m_pstHierarchy;
	ItemTemplateEquipSet.m_pcsAttachedAtomics		= &pcdItem->m_csAttachedAtomics;
	ItemTemplateEquipSet.m_pvCustData				= (PVOID)(pcdItemTemplate);
	ItemTemplateEquipSet.m_pstTransformInfo			= pcdItemTemplate->m_pItemTransformInfo->GetInfo( ppdCharacter->m_lTID1 );
	ItemTemplateEquipSet.m_nItemTID					= ppdItemTemplateEquip->m_lID;

	if( !pcdItem->m_pstClump ) return FALSE;

	RpClumpForAllAtomics( pcdItem->m_pstClump, AttachCharonToSummonerCB, ( PVOID )( &ItemTemplateEquipSet ) );

	// Light ����..
	if( ppdItemTemplateEquip->m_lightInfo.GetType() )
	{		
		AuCharacterLightInfo* pLightInfo = pcdCharacter->GetLightInfo( ppdItemTemplateEquip->GetPartIndex() );
		if( pLightInfo )
		{
			*pLightInfo = ppdItemTemplateEquip->m_lightInfo;
		}
	}

	return TRUE;
}

AgpdItem* AgcmItem::GetCurrentEquipWeapon( void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return NULL;

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	if( !ppmItem ) return NULL;

	// ������ ���� �˻�
	AgpdItem* ppdWeapon = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_RIGHT );
	if( !ppdWeapon )
	{
		// �����տ� �������� ���Ⱑ ������ �޼� �˻�
		ppdWeapon = GetCurrentEquipWeapon( ppdCharacter, AGPMITEM_PART_HAND_LEFT );

		// �޼տ��� ���� �ִ°� �̰� ���и� NULL �� ����
		if( ppdWeapon )
		{
			AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdWeapon->m_pcsItemTemplate;
			if( !ppdItemTemplateEquip ) return NULL;
			if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD ) return NULL;
		}
	}

	return ppdWeapon;
}

AgpdItem* AgcmItem::GetCurrentEquipWeapon( void* pCharacter, INT32 nHand )
{
	return m_pcsAgpmItem->GetEquipSlotItem(
		(AgpdCharacter *)pCharacter, (AgpmItemPart)nHand);
}

BOOL AgcmItem::OnAttachWeapon( void* pCharacter)
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	// �������� �����۵��� ���鼭 �������� ���⸦ ã�´�.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	if( !ppmItem || !ppmGrid ) return FALSE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );

		// �ִϸ��̼� Ÿ�Լ��ø� ���ְ� �Ѿ��
		SetEquipAnimType( ppdCharacter, pcdCharacter );
		return TRUE;
	}

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// �ش� �׸����� �������� ã�´�.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) continue;

		if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			//_UnEquipItem( ppdCharacter, ppdItemCurrent, FALSE );
			_EquipItem( ppdCharacter, ppdItemCurrent );
		}

		// ������..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}

BOOL AgcmItem::OnDetachWeapon( void* pCharacter )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter ) return FALSE;

	// �������� �����۵��� ���鼭 �������� ���⸦ ã�´�.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	if( !ppmItem || !ppmGrid ) return FALSE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// �ش� �׸����� �������� ã�´�.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) continue;

		if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			_UnEquipItem( ppdCharacter, ppdItemCurrent, FALSE );
		}

		// ������..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}

BOOL AgcmItem::OnAttachPart( VOID* pCharacter , AgpmItemPart ePartType )
{
	AgpdCharacter*	ppdCharacter	= static_cast< AgpdCharacter* >(pCharacter);
	AgpmItem*		ppmItem			= static_cast< AgpmItem* >(GetModule( "AgpmItem" ));
	AgpmGrid*		ppmGrid			= static_cast< AgpmGrid* >(GetModule( "AgpmGrid" ));	
	if( !ppmItem || !ppmGrid || !ppdCharacter ) 
		return FALSE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType		eRaceType	= static_cast< AuRaceType >(m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ));
	AuCharClassType eClassType	= m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );

		SetEquipAnimType( ppdCharacter, pcdCharacter );
		return TRUE;
	}

	AgpdItemADChar* ppdItemADChar	= ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid*		ppdEquipGrid	= &ppdItemADChar->m_csEquipGrid;

	INT32			nSequencialIndex	= 0;	
	AgpdGridItem*	ppdGridItem			= ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// �ش� �׸����� �������� ã�´�.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) 
			continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) 
			continue;

		if( ppdItemTemplateEquip->m_nPart == ePartType )
			_EquipItem( ppdCharacter, ppdItemCurrent );

		// ������..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}

BOOL AgcmItem::OnDetachPart( VOID* pCharacter , AgpmItemPart ePartType )
{
	AgpdCharacter*	ppdCharacter	= static_cast< AgpdCharacter* >(pCharacter);
	AgpmItem*		ppmItem			= static_cast< AgpmItem* >(GetModule( "AgpmItem" ));
	AgpmGrid*		ppmGrid			= static_cast< AgpmGrid* >(GetModule( "AgpmGrid" ));	
	if( !ppmItem || !ppmGrid || !ppdCharacter ) 
		return FALSE;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType		eRaceType	= static_cast< AuRaceType >(m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ));
	AuCharClassType eClassType	= m_pcsAgcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		AgcdCharacter* pcdCharacter = m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );

		SetEquipAnimType( ppdCharacter, pcdCharacter );
		return TRUE;
	}

	AgpdItemADChar* ppdItemADChar	= ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid*		ppdEquipGrid	= &ppdItemADChar->m_csEquipGrid;

	INT32			nSequencialIndex	= 0;	
	AgpdGridItem*	ppdGridItem			= ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// �ش� �׸����� �������� ã�´�.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) 
			continue;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemCurrent->m_pcsItemTemplate;
		if( !ppdItemTemplateEquip ) 
			continue;

		if( ppdItemTemplateEquip->m_nPart == ePartType )
			_UnEquipItem( ppdCharacter, ppdItemCurrent , TRUE );

		// ������..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return TRUE;
}


BOOL AgcmItem::OnUpdateHairAndFace( AgpdItemTemplate* ppdItemTemplate, AgcdCharacter* pcdCharacter, BOOL bIsEquip )
{
	if( !ppdItemTemplate || !pcdCharacter ) return FALSE;
	if( ppdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP ) return FALSE;

	AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemTemplate;
	if( ppdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_ARMOUR ) return FALSE;
	if( ppdItemTemplateEquip->GetPartIndex() != AGPMITEM_PART_HEAD ) return FALSE;

	AgcdItemTemplate* pcdItemTemplate = GetTemplateData( ppdItemTemplate );
	if( !pcdItemTemplate ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
	if( !pcmCharacter || !ppmFactor ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetCharacter( pcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// ���ӳʴ� �н�
	AuRaceType eRace = ( AuRaceType )ppmFactor->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_csFactor );
	if( eRace == AURACE_TYPE_DRAGONSCION && eClass == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	// Case 1-1. ĳ������ ���� �Ӹ��� �Բ� ���;� �ϴ� ���
	if( pcdItemTemplate->m_bWithHair )
	{
		// ĳ������ ���� �Ӹ��� ���δ�.
		pcmCharacter->SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
	}
	// Case 1-2. ĳ������ ���� �Ӹ��� �����ϰ� �����۸� ���;� �ϴ� ���
	else 
	{
		// Case 1-2-1. �� �������� �����Ϸ� �ϴ� ���
		if( bIsEquip )
		{
			// ĳ���Ϳ� �پ� �ִ� �Ӹ� ����
			m_pcsAgcmCharacter->DetachHair( pcdCharacter );

			// �Ӹ� ���ϼ� ���ٰ� ���
			pcdCharacter->m_bAttachableHair = FALSE;
		}
		// Case 1-2-2. �� �������� ���������ϴ� ���
		else
		{
			// �����ߴ� �Ӹ��� ������� �������ش�.
			pcdCharacter->m_bAttachableHair = TRUE;
			pcmCharacter->SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
		}
	}

	// Case 2-1. ĳ������ ���� �󱼰� �Բ� ���;� �ϴ� ���
	if( pcdItemTemplate->m_bWithFace )
	{
		// ĳ������ ���� ���� ���δ�.
		pcmCharacter->SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
	}
	// Case 2-2. ĳ������ ���� ���� �����ϰ� �����۸� ���;� �ϴ� ���
	else
	{
		// Case 2-2-1. �� �������� �����Ϸ� �ϴ� ���
		if( bIsEquip )
		{
			// ĳ���Ϳ� �پ� �ִ� �� ����
			m_pcsAgcmCharacter->DetachFace( pcdCharacter );

			// ĳ���Ϳ� ���� ���ϼ� ���ٰ� ���
			pcdCharacter->m_bAttachableFace = FALSE;
		}
		// Case 2-2-2. �� �������� ���������ϴ� ���
		else
		{
			// ������ ���� �ٽ� �ٿ��ش�.
			pcmCharacter->SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
		}
	}

	return TRUE;
}

BOOL AgcmItem::OnUpdateViewHelmet( AgcdCharacter* pcdCharacter )
{
	if( !pcdCharacter ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
	if( !ppmItem || !pcmCharacter || !ppmFactor ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetCharacter( pcdCharacter );
	if( !ppdCharacter ) return FALSE;

	// ���ӳʴ� �н�
	AuRaceType eRace = ( AuRaceType )ppmFactor->GetRace( &ppdCharacter->m_csFactor );
	AuCharClassType eClass = ( AuCharClassType )ppmFactor->GetClass( &ppdCharacter->m_csFactor );
	if( eRace == AURACE_TYPE_DRAGONSCION && eClass == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	// ���� �������� �����۵� �߿��� HEAD ������ �ش��ϴ� �������� ã�´�.
	AgpdGridItem* ppdGridItem = ppmItem->GetEquipItem( ppdCharacter, AGPMITEM_PART_HEAD );
	if( !ppdGridItem ) return FALSE;

	AgpdItem* ppdItem = ppmItem->GetItem( ppdGridItem->m_lItemID );
	if( !ppdItem ) return FALSE;

	AgpdItemTemplate* ppdItemTemplate = ppdItem->m_pcsItemTemplate;
	AgcdItemTemplate* pcdItemTemplate = GetTemplateData( ppdItemTemplate );
	
	if( !pcdItemTemplate ) return FALSE;
	if( ppmItem->IsAvatarItem( ppdItem->m_pcsItemTemplate ) ) return FALSE;
	if( _IsWearAvatarItem( ppdCharacter, TRUE, TRUE ) ) return FALSE;

	AgpdItemTemplate* ppdItemTemplateTemp = ppdItemTemplate;
	AgcdItemTemplate* pcdItemTemplateTemp = pcdItemTemplate;

	// �巡��ÿ��� ��� ���� ��ȭ���¿� ���� �����ؾ� �� �������� �޶�����.
	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) == AURACE_TYPE_DRAGONSCION )
	{
		// �ش��ϴ� ������ ���ų� �ش�Ǵ� ������ ���� �����̶� ���� �Ÿ� ���� ���ø��� ���
		ppdItemTemplate = _GetDragonScionItemAgpdTemplate( ppdItemTemplate, ppdCharacter, eClass );
		if( !ppdItemTemplate )
		{
			ppdItemTemplate = ppdItemTemplateTemp;
		}
		else if( ppdItemTemplate->m_lID == ppdItemTemplateTemp->m_lID )
		{
			ppdItemTemplate = ppdItemTemplateTemp;
		}
	}

	// Case 1. ����� �������� ���ƾ� �ϴ� ���
	if( !pcdCharacter->m_bViewHelmet )
	{
		// ���������� UnEquip ���ش�.
		UnEquipItem( ppdCharacter, ppdItem, FALSE, pcdCharacter->m_bViewHelmet );

		// HEAD ���� �������� �Ӹ��� ����������?
		if( !pcdItemTemplate->m_bWithHair )
		{
			// ���ư� �Ӹ��� �������ش�.
			pcmCharacter->SetHair( pcdCharacter, ppdCharacter->m_lHairIndex );
		}

		// HEAD ���� �������� ���� ����������?
		if( !pcdItemTemplate->m_bWithFace )
		{
			// ���ư� ���� ���� ���ش�.
			pcmCharacter->SetFace( pcdCharacter, ppdCharacter->m_lFaceIndex );
		}
	}
	// Case 2. ����� ������� �ϴ� ���
	else
	{
		EquipItem( ppdCharacter, ppdItem, FALSE, TRUE );
		OnUpdateHairAndFace( ppdItemTemplate, pcdCharacter, TRUE );
	}

	return TRUE;
}

AgpdItemTemplateEquip* AgcmItem::_GetDragonScionItemAgpdTemplate( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItem || !ppdCharacter ) return NULL;

	AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplateEquip ) return NULL;

	return _GetDragonScionItemAgpdTemplate( ppdItemTemplateEquip, ppdCharacter, eClassType );
}

AgpdItemTemplateEquip* AgcmItem::_GetDragonScionItemAgpdTemplate( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItemTemplate || !ppdCharacter ) return NULL;

	if( m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor ) != AURACE_TYPE_DRAGONSCION )
	{
		return ( AgpdItemTemplateEquip* )ppdItemTemplate;
	}

	INT32 nEvolutionTID = m_ItemEvolutionTable.GetEvolutionTID( ppdItemTemplate->m_lID, eClassType );
	if( nEvolutionTID < 0 )
	{
		return ( AgpdItemTemplateEquip* )ppdItemTemplate;
	}

	return ( AgpdItemTemplateEquip* )m_pcsAgpmItem->GetItemTemplate( nEvolutionTID );
}

AgcdItemTemplate* AgcmItem::_GetDragonScionItemAgcdTemplate( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	AgpdItemTemplateEquip* pItemTemplate = _GetDragonScionItemAgpdTemplate( ppdItem, ppdCharacter, eClassType );
	if( !pItemTemplate ) return NULL;
	return GetTemplateData( ( AgpdItemTemplate* )pItemTemplate );
}

AgcdItemTemplate* AgcmItem::_GetDragonScionItemAgcdTemplate( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItemTemplate || !ppdCharacter ) return NULL;
	return GetTemplateData( ( AgpdItemTemplate* )ppdItemTemplate );
}

AgcdItem* AgcmItem::_GetDragonScionItemAgcdItem( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType )
{
	if( !ppdItemTemplate ) return NULL;
	AgpdItem* ppdItem = m_pcsAgpmItem->GetInventoryItemByTID( ppdCharacter, ppdItemTemplate->m_lID );

	// �������� ���� ��������� �ʾ����� ����
	if( !ppdItem )
	{
		INT32 nItemID = 100000 + ppdItemTemplate->m_lID;
		ppdItem = m_pcsAgpmItem->AddItem( nItemID, ppdItemTemplate->m_lID, 1, FALSE );
		m_pcsAgpmItem->InitItem( ppdItem );

		ppdItem->m_pcsCharacter = ppdCharacter;
		ppdItem->m_eStatus = AGPDITEM_STATUS_EQUIP;
		MakeItemClump( ppdItem );
	}

	return GetItemData( ppdItem );
}

BOOL AgcmItem::_IsMatrixValid( RwMatrix* pTM )
{
	if( !pTM ) return FALSE;

	float fLengthAt = RwV3dLength( &pTM->at );
	float fLengthRight = RwV3dLength( &pTM->right );
	float fLengthUp = RwV3dLength( &pTM->up );

	if( fLengthAt == 0.0f || fLengthRight == 0.0f || fLengthUp == 0.0f ) return FALSE;
	return TRUE;
}

RpAtomic* AgcmItem::_AttachItemArmor( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart )
{
	if( !pAtomic || !pEquipSet ) return NULL;

	RpClump				*pstCharacterClump				= pEquipSet->m_pstBaseClump;
	RpHAnimHierarchy	*pstCharacterHierarchy			= pEquipSet->m_pstBaseHierarchy;
	RpHAnimHierarchy	*pstItemHierarchy				= pEquipSet->m_pstEquipmentHierarchy;
	INT16				nKind							= pEquipSet->m_nKind;
	RwInt16				nCBCount						= pEquipSet->m_nCBCount;

	LockFrame();

	RpAtomic* pstAtomic = RpAtomicClone(pAtomic);
	if (!pstAtomic)
	{
		ASSERT(!"AgcmItem::AttachNodeIndex() Cloning Error!!!");
		UnlockFrame();
		return NULL;
	}

#ifdef _DEBUG
	pstAtomic->pvApBase = RpAtomicGetClump(pAtomic) ? RpAtomicGetClump(pAtomic)->pvApBase : pAtomic->pvApBase;
#endif

	RpGeometry* pstGeom		= RpAtomicGetGeometry(pstAtomic);
	RwFrame* pstFrame		= RpClumpGetFrame(pstCharacterClump);
	RwFrame* pstAtomicFrame	= RwFrameCreate();

	RpAtomic* pstAtomic2 = NULL;
	AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(pEquipSet->m_pvCustData);

	RpAtomicSetFrame(pstAtomic, pstAtomicFrame);
	RwFrameAddChild(pstFrame, pstAtomicFrame);
	RpClumpAddAtomic(pstCharacterClump, pstAtomic);

	UnlockFrame();

	AcuObject::SetAtomicRenderUDA( pstAtomic, pstAtomic->stRenderInfo.renderType,
		(RpAtomicCallBackRender)(pstAtomic->stRenderInfo.backupCB),
		pstAtomic->stRenderInfo.blendMode, 0, 0 );

	if (pEquipSet->m_pstTransformInfo)
	{
		//ASSERT(!"Not Used Code...");
		RwMatrix* pstMatrix = pEquipSet->m_pstTransformInfo->m_astTransform + nCBCount;
		RpGeometryTransform( pstGeom, pstMatrix );
	}

	if( pcsAgcdItemTemplate && pcsAgcdItemTemplate->m_bEquipTwoHands )
	{			
		pstAtomic2 = RpAtomicClone(pAtomic);
		if( pstAtomic2 )
		{
			LockFrame();
#ifdef _DEBUG
			pstAtomic2->pvApBase = RpAtomicGetClump(pAtomic) ? RpAtomicGetClump(pAtomic)->pvApBase : pAtomic->pvApBase;
#endif
			RpGeometry* pstGeom		= RpAtomicGetGeometry(pstAtomic2);
			RwFrame* pstFrame		= RpClumpGetFrame(pstCharacterClump);
			RwFrame* pstAtomicFrame	= RwFrameCreate();
			RpAtomicSetFrame(pstAtomic2, pstAtomicFrame);
			RwFrameAddChild(pstFrame, pstAtomicFrame);
			RpClumpAddAtomic(pstCharacterClump, pstAtomic2);

			UnlockFrame();

			AcuObject::SetAtomicRenderUDA( pstAtomic2, pstAtomic2->stRenderInfo.renderType,
				(RpAtomicCallBackRender)(pstAtomic2->stRenderInfo.backupCB),
				pstAtomic2->stRenderInfo.blendMode, 0, 0 );

			if( pEquipSet->m_pstTransformInfo )
			{
				RwMatrix* pstMatrix = pEquipSet->m_pstTransformInfo->m_astTransform + 1;
				RpGeometryTransform( pstGeom, pstMatrix );
			}
		}			
	}

	if (m_pWorld && (pEquipSet->m_pstBaseClump->ulFlag & RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		if(m_pcsAgcmRender)
			m_pcsAgcmRender->AddAtomicToWorld(pstAtomic, ONLY_ALPHA,AGCMRENDER_ADD_NONE, true);
		else
			RpWorldAddAtomic(m_pWorld, pstAtomic);

		// shadow���� ����
		if( m_pcsAgcmShadow2 )
		{
			RwFrame*	pFrame = RpClumpGetFrame(pstCharacterClump);
			if( pFrame )
			{
				RwV3d* pPos = RwMatrixGetPos( RwFrameGetLTM(pFrame) );
				if( m_pcsAgcmShadow2->GetShadowBuffer( pPos ) )
					m_pcsAgcmRender->CustomizeLighting(pstCharacterClump, LIGHT_IN_SHADOW);
			}
		}
	}

	if (pEquipSet->m_pcsAttachedAtomics)
	{
		if (pstAtomic)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic);

		if (pstAtomic2)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic2);
	}

	return pstAtomic;
}

RpAtomic* AgcmItem::_AttachItemWeapon( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart )
{
	if( !pAtomic || !pEquipSet ) return NULL;

	RpClump				*pstCharacterClump				= pEquipSet->m_pstBaseClump;
	RpHAnimHierarchy	*pstCharacterHierarchy			= pEquipSet->m_pstBaseHierarchy;
	RpHAnimHierarchy	*pstItemHierarchy				= pEquipSet->m_pstEquipmentHierarchy;

	INT32 lNodeID	= AGCMITEM_NODE_ID_RIGHT_HAND;
	INT32 lNodeID2	= AGCMITEM_NODE_ID_LEFT_HAND;

	if (nPart == AGPMITEM_PART_HAND_LEFT)
	{
		lNodeID		= AGCMITEM_NODE_ID_LEFT_HAND;
		lNodeID2	= AGCMITEM_NODE_ID_RIGHT_HAND;
	}

	ItemTransformInfo* pTransformInfo = pEquipSet->m_pstTransformInfo;
	RwMatrix* pAttachTM = NULL;

	if( pTransformInfo )
	{
		pAttachTM = pTransformInfo->m_astTransform;
		if( !_IsMatrixValid( pAttachTM ) )
		{
			pAttachTM = NULL;
		}
	}

	RpAtomic* pstAtomic = AttachNodeIndex( pstCharacterClump, pstCharacterHierarchy, pstItemHierarchy, pAtomic,
		lNodeID, nPart,	pAttachTM, &pEquipSet->m_pcsAttachedAtomics->m_pstHierarchy, R_ALPHAFUNC );

	RpAtomic* pstAtomic2 = NULL;
	AgcdItemTemplate *pcsAgcdItemTemplate = (AgcdItemTemplate *)(pEquipSet->m_pvCustData);

	if (pcsAgcdItemTemplate->m_bEquipTwoHands)
	{
		ItemTransformInfo* pTransformInfo1 = pTransformInfo + 1;
		RwMatrix* pAttachTM1 = NULL;

		if( pTransformInfo && pTransformInfo1 )
		{
			pAttachTM1 = pTransformInfo->m_astTransform + 1;
			if( !_IsMatrixValid( pAttachTM1 ) )
			{
				pAttachTM1 = NULL;
			}
		}

		pstAtomic2 = AttachNodeIndex( pstCharacterClump, pstCharacterHierarchy, pstItemHierarchy, pAtomic,
			lNodeID2, nPart, pAttachTM1, &pEquipSet->m_pcsAttachedAtomics->m_pstHierarchy, R_ALPHAFUNC );
	}

	// shadow���� ����
	if(m_pcsAgcmShadow2 && (pstCharacterClump->ulFlag &RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		RwFrame* pFrame = RpClumpGetFrame(pstCharacterClump);
		if( pFrame )
		{
			RwV3d* pPos = RwMatrixGetPos( RwFrameGetLTM( pFrame ) );
			if(m_pcsAgcmShadow2->GetShadowBuffer(pPos))
				m_pcsAgcmRender->CustomizeLighting( pstCharacterClump, LIGHT_IN_SHADOW );
		}
	}

	if (pEquipSet->m_pcsAttachedAtomics)
	{
		if (pstAtomic)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic);

		if (pstAtomic2)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic2);
	}

	return pstAtomic;
}

RpAtomic* AgcmItem::_AttachItemShield( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart )
{
	if( !pAtomic || !pEquipSet ) return NULL;

	RpClump				*pstCharacterClump				= pEquipSet->m_pstBaseClump;
	RpHAnimHierarchy	*pstCharacterHierarchy			= pEquipSet->m_pstBaseHierarchy;
	RpHAnimHierarchy	*pstItemHierarchy				= pEquipSet->m_pstEquipmentHierarchy;

	INT32 lNodeID = nPart == AGPMITEM_PART_HAND_RIGHT ? AGCMITEM_NODE_ID_RIGHT_HAND : AGCMITEM_NODE_ID_LEFT_HAND;

	RpAtomic* pstAtomic = AttachNodeIndex( pstCharacterClump, pstCharacterHierarchy, pstItemHierarchy, pAtomic,
		lNodeID, nPart,	pEquipSet->m_pstTransformInfo ? pEquipSet->m_pstTransformInfo->m_astTransform : NULL,
		&pEquipSet->m_pcsAttachedAtomics->m_pstHierarchy );

	// shadow���� ����
	if(m_pcsAgcmShadow2 && (pstCharacterClump->ulFlag & RWFLAG_RENDER_ADD))
	{
#ifdef _DEBUG
		if (g_dwThreadID != GetCurrentThreadId())
			ASSERT(!"No proper thread process!!!");
#endif
		RwFrame* pFrame = RpClumpGetFrame( pstCharacterClump );
		if( pFrame )
		{
			RwV3d* pPos = RwMatrixGetPos( RwFrameGetLTM(pFrame) );
			if( m_pcsAgcmShadow2->GetShadowBuffer( pPos ) )
				m_pcsAgcmRender->CustomizeLighting( pstCharacterClump, LIGHT_IN_SHADOW );
		}
	}

	if (pEquipSet->m_pcsAttachedAtomics)
	{
		if (pstAtomic)
			((AgcmItem *)(pEquipSet->m_pvClass))->AddAttachedAtomic(pEquipSet->m_pcsAttachedAtomics, pstAtomic);
	}

	return pstAtomic;
}

BOOL AgcmItem::_EquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckDefaultArmour, BOOL bViewHelmet )
{
	AgcdItem				*pstAgcdItem				= GetItemData(pcsAgpdItem);

	AgcdCharacter			*pstAgcdCharacter			= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= pstAgcdCharacter->m_pstAgcdCharacterTemplate;
	AgcdItemTemplate		*pstAgcdItemTemplate		= GetTemplateData((AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate));
	AgpdItemTemplateEquip	*pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate);

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	//���� ź���¿��� �α��εǸ� �̰����� ���� �¿����Ѵ�.
	if( pstAgpdCharacter->m_bRidable && pstAgcdCharacter->m_pRide == NULL &&
		pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_RIDE )
	{
		AgcmRide::CBRideAck( pstAgpdCharacter, m_pcsAgcmRide, &pcsAgpdItem->m_lID );
		return TRUE;
	}

	//Ż���� Ÿ���ִ� ���¿��� ���󹫱���� ����� �������� �ʴ´�.
	if( pstAgpdCharacter->m_bRidable && 
		pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON && 
		pcsAgpdItemTemplateEquip->GetPartIndex() != AGPMITEM_PART_LANCER ) 
	{
		return TRUE;
	}

	//���е� �����Ҽ� ����.
	if( pstAgpdCharacter->m_bRidable &&
		pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD )
	{
		return TRUE;
	}

	//Ż���� ��ź���¿��� ���󹫱�� �������� �ʴ´�.
	if( pstAgpdCharacter->m_bRidable == FALSE && pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_LANCER )
	{
		return TRUE;
	}

	// �������� 0�����̸� ������ �� ����. (����, ����� ����)
	if (pcsAgpdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_NECKLACE && 
		pcsAgpdItemTemplateEquip->m_nKind != AGPMITEM_EQUIP_KIND_RING)
	{
		if (m_pcsAgpmCharacter->IsPC(pstAgpdCharacter))
		{
			INT32	lCurrentDurability	= 0;
			m_pcsAgpmFactors->GetValue(&pcsAgpdItem->m_csFactor, &lCurrentDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

#ifdef USE_MFC
			if( lCurrentDurability <= 0 )
				lCurrentDurability = 1;			
#else
			if( lCurrentDurability <= 0 && !pcsAgpdItemTemplateEquip->IsAvatarEquip() )
				return TRUE;		
#endif
		}
	}

	if( pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD )
	{
		if( !bViewHelmet )
			pstAgcdCharacter->m_bEquipHelmet = TRUE;

		if( !pcsAgpdItemTemplateEquip->IsAvatarEquip() && !pstAgcdCharacter->m_bViewHelmet )
			return TRUE;
	}

	if( !pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	{
		AgpdGridItem * pGrid = m_pcsAgpmItem->GetEquipItem( pstAgpdCharacter , pcsAgpdItemTemplateEquip->GetAvatarPartIndex() );
		if( pGrid )		// �ƹ�Ÿ �������� �̹� �����Ѵ�.
			return TRUE;
	}

	// ������ �۾��� �巡��ÿ� ���ӳ��� ��� ó������ �ʴ´�.
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE )
	{
		SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);
		EnumCallback(AGCMITEM_CB_ID_EQUIP_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

		// ������ ������ ���� �߰��� ó���ؾ��� �͵��� ó���Ѵ�.
		switch( pcsAgpdItemTemplateEquip->m_nKind )
		{
		case AGPMITEM_EQUIP_KIND_NECKLACE :
			{
				// ������� ��� ���������� ���� �ִ��� �˻��Ͽ� �̸� ó���Ѵ�.
				OnEquipItemNecklace( pstAgpdCharacter, pcsAgpdItem, pstAgcdItem );
			}
			break;
		};

		return TRUE;
	}

	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )GetModule( "AgcmUIItem" );
	if( pcmUIItem )
	{
		if( !pcmUIItem->IsUsableItem( pstAgpdCharacter, pcsAgpdItem, eClassType ) )
		{
			// ����Ұ����� �������̶�� ������ �𵨸��۾��� ó������ �ʴ´�.
			return FALSE;
		}
	}

	// �̹� Attach�� Item�� ���⿡ ������ �ȵȴ�.
	//	ASSERT(!(pstAgcdItem->m_lStatus & AGCDITEM_STATUS_ATTACH));

	// Valid ���� ���� �ɸ��ʹ� ������� ������ �ȵȴ�.
	// Valid ���� ���� �ɸ��ʹ� ������ 2���� ����̴�.
	// 1. ���� Init�� �� ������ �ʾҴ�. (�� ���� ResourceLoader�� ���ؼ� InitCharacter�� �� ���� ���;� �Ѵ�.)
	// 2. �̹� Remove�� �ɸ����̴�. (�� ���� ���� UnEquip�� �Ǳ� ������ ResourceLoader�� List���� ���ܵǾ� �Ѵ�.)
	ASSERT(m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter));
	ASSERT(pstAgcdCharacter->m_pClump && pstAgcdCharacter->m_pInHierarchy);		// �翬�� Valid �� Character��� Clump�� �־�� �ȴ�.
	if( !pstAgcdCharacter->m_pClump )
		return TRUE;

	// Record the 'countEnd' value of an atomic in order to use it later in case of the 'invisible' mode.
	RwInt16 countEnd = 0;
	if( pstAgcdCharacter->m_pClump && pstAgcdCharacter->m_pClump->atomicList )
		countEnd = pstAgcdCharacter->m_pClump->atomicList->stRenderInfo.countEnd;

	AgcdItemTemplateEquipSet stAgcdItemTemplateEquipSet;
	if( /*(bCheckDefaultArmour) && */(pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) && (pstAgcdCharacterTemplate->m_pDefaultArmourClump) )
	{
		ZeroMemory(&stAgcdItemTemplateEquipSet, sizeof(AgcdItemTemplateEquipSet));

		stAgcdItemTemplateEquipSet.m_pvClass					= this;
		stAgcdItemTemplateEquipSet.m_nCBCount					= 0;
		stAgcdItemTemplateEquipSet.m_nPart						= pcsAgpdItemTemplateEquip->GetPartIndex();	// ������� �ϴ� ������ part�� ����...
		stAgcdItemTemplateEquipSet.m_nKind						= pcsAgpdItemTemplateEquip->m_nKind;	// ������� �ϴ� ������ kind�� ����...
		stAgcdItemTemplateEquipSet.m_pstBaseClump				= pstAgcdCharacter->m_pClump;
		stAgcdItemTemplateEquipSet.m_pstBaseHierarchy			= pstAgcdCharacter->m_pInHierarchy;

		//. 2006. 1. 13. Nonstopdj
		//. pstAgcdCharacter->m_pClump�� NULL�� ��쿡�� ���� �� �����Ƿ� ���κп� NULLüũ �߰�. by Crash Report.
		RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, DetachItemCB, (PVOID)(&stAgcdItemTemplateEquipSet));
	}

	// �Ӹ�ī���� ��ó��..
	// ���� �������� �󱼰� �� �پ��ִ��� Ȯ���ؾ��Ѵ�.
	OnUpdateHairAndFace( pcsAgpdItem->m_pcsItemTemplate, pstAgcdCharacter, TRUE );

	if( !AttachItem( pstAgpdCharacter->m_lTID1, pstAgcdCharacter, pcsAgpdItemTemplateEquip, pstAgcdItem, pstAgcdItemTemplate, eClassType ) )
	{
		// � ���������� ������ ���.. ����Ʈ �������� ��ġ�Ѵ�.
		AttachedAtomics csAttachedAtomics;
		if( pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR	&& pstAgcdCharacterTemplate->m_pDefaultArmourClump )
		{
			if(!EquipDefaultArmour(
				pstAgcdCharacter->m_pClump,
				pstAgcdCharacterTemplate->m_pDefaultArmourClump,
				pstAgcdCharacter->m_pInHierarchy,
				pcsAgpdItemTemplateEquip->GetPartIndex(),
				&csAttachedAtomics ) ) return FALSE;

			AttachedAtomicList* pcsCurrent = csAttachedAtomics.pcsList;
			while (pcsCurrent)
			{
				m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
				pcsCurrent	= pcsCurrent->m_pcsNext;
			}

			RemoveAllAttachedAtomics(&csAttachedAtomics);
		}
	}

	// LOD
	if (m_pcsAgcmLODManager)
	{
		AgcdLODData	*pstCharacterLODData = m_pcsAgcmLODManager->GetLODData(&pstAgcdCharacterTemplate->m_stLOD, 0, FALSE);
		if( pstCharacterLODData && pstCharacterLODData->m_ulMaxLODLevel)
		{
			AgcdLODData *pstItemLODData	= m_pcsAgcmLODManager->GetLODData(&pstAgcdItemTemplate->m_stLOD, 0, FALSE);
			if( pstItemLODData && pstItemLODData->m_ulMaxLODLevel )
			{
				m_pcsAgcmLODManager->SetLODCallback(pstAgcdCharacter->m_pClump);
				AcuObject::SetClumpDInfo_LODLevel( pstAgcdCharacter->m_pClump, pstCharacterLODData->m_ulMaxLODLevel );
			}
		}
	}
	SetItemStatus(pstAgcdItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);

	EnumCallback(AGCMITEM_CB_ID_EQUIP_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

	if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_HALF)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 77, countEnd);
	else if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_FULL)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 0, countEnd);

	// ������ ������ ���� �߰��� ó���ؾ��� �͵��� ó���Ѵ�.
	switch( pcsAgpdItemTemplateEquip->m_nKind )
	{
	case AGPMITEM_EQUIP_KIND_NECKLACE :
		{
			// ������� ��� ���������� ���� �ִ��� �˻��Ͽ� �̸� ó���Ѵ�.
			OnEquipItemNecklace( pstAgpdCharacter, pcsAgpdItem, pstAgcdItem );
		}
		break;
	};

	// ������ ���� �������� �ִϸ��̼� ����
	SetEquipAnimType( pstAgpdCharacter, pstAgcdCharacter );
	return TRUE;
}

BOOL AgcmItem::_UnEquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem, BOOL bViewHelmet )
{
	AgcdCharacter			*pstAgcdCharacter			= m_pcsAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate	= pstAgcdCharacter->m_pstAgcdCharacterTemplate;
	AgpdItemTemplateEquip	*pcsAgpdItemTemplateEquip	= (AgpdItemTemplateEquip *)(pcsAgpdItem->m_pcsItemTemplate);
	AgcdItem				*pstAgcdItem				= GetItemData(pcsAgpdItem);
	AgcdItemTemplate		*pstAgcdItemTemplate		= pstAgcdItem->m_pstAgcdItemTemplate;

	// �巡��ÿ����� ���ӳ� Ŭ������ ���ؼ��� �������� �ʴ´�.
	AuRaceType eRaceType = ( AuRaceType )m_pcsAgpmFactors->GetRace( &pstAgpdCharacter->m_csFactor );
	AuCharClassType eClassType = m_pcsAgcmCharacter->GetClassTypeByTID( pstAgpdCharacter->m_lTID1 );
	if( eRaceType == AURACE_TYPE_DRAGONSCION && eClassType == AUCHARCLASS_TYPE_MAGE ) return TRUE;

	if( pcsAgpdItemTemplateEquip->GetPartIndex() == AGPMITEM_PART_HEAD && bViewHelmet == FALSE )
		pstAgcdCharacter->m_bEquipHelmet = FALSE;

	//if( !(pstAgcdItem->m_lStatus & AGCDITEM_STATUS_ATTACH ))
	//	return TRUE;

	// Valid ���� ���� Character�� ������� ������ �ȵȴ�.
	// Valid ���� �ʴٴ� ���� ������ 2���� ����̴�.
	// 1. ���� InitCharacter()�� �� �� �ɸ��� (�� ��쿡�� ���� AGCDITEM_STATUS_ATTACH Flag�� ���� ���̹Ƿ� ������ return�ȴ�.)
	// 2. Remove�� �ɸ��� (Module ������ Remove Flag�� ������ ���� UnEquip �� �ȴ�.)
	//ASSERT(m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter));
	if( !m_pcsAgcmCharacter->IsValidCharacter(pstAgcdCharacter))
	{
		MD_SetErrorMessage("AgcmItem::UnEquipItem() pstAgcdCharacter is Invalid.");
		return TRUE;
	}

	if (!pstAgcdCharacter->m_pClump ||
		!pstAgcdCharacter->m_pClump->atomicList) {
		MD_SetErrorMessage(
			"AgcmItem::UnEquipItem() "
			"pstAgcdCharacter->m_pClump was removed.");
		return TRUE;
	}

	if( !pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	{
		AgpdGridItem * pGrid = m_pcsAgpmItem->GetEquipItem( pstAgpdCharacter , pcsAgpdItemTemplateEquip->GetAvatarPartIndex() );
		if( pGrid )
			return TRUE;
	}

	RwInt16 countEnd = pstAgcdCharacter->m_pClump->atomicList->stRenderInfo.countEnd;

	EnumCallback(AGCMITEM_CB_ID_UNEQUIP_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgcdItem));

	OnUpdateHairAndFace( pcsAgpdItem->m_pcsItemTemplate, pstAgcdCharacter, FALSE );

	//. 2006. 3. 16. nonstopdj
	m_pcsAgcmCharacter->GeneralizeAttachedAtomicsNormals(pstAgcdCharacter);

	AuCharClassType eCurrentClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &pstAgpdCharacter->m_csFactor );
	DetachItem(pstAgcdCharacter, pcsAgpdItemTemplateEquip, pstAgcdItem, pstAgcdItemTemplate, eCurrentClassType );

	if( pcsAgpdItemTemplateEquip->IsAvatarEquip() )
	{
		// �ƹ�Ÿ ��� ���°��
		// �Ʒ��� ���� ��� �ٽ� ���� �ž���Ѵ�.
		AgpdGridItem * pGrid = m_pcsAgpmItem->GetEquipItem( pstAgpdCharacter , pcsAgpdItemTemplateEquip->GetPartIndex() );
		if( pGrid )
		{
			AgpdItem				*pcsUnderWearItem		= m_pcsAgpmItem->GetItem( pGrid );
			AgpdItemTemplateEquip	*pcsUnderWearTemplate	= (AgpdItemTemplateEquip *)(pcsUnderWearItem->m_pcsItemTemplate);

			AgcdItem				*pstAgcdUnderWearItem			= GetItemData(pcsUnderWearItem);
			AgcdItemTemplate		*pstAgcdUnderWearItemTemplate	= pstAgcdUnderWearItem->m_pstAgcdItemTemplate;

			if( pcsUnderWearTemplate->m_nPart == AGPMITEM_PART_HEAD && !pstAgcdCharacter->m_bViewHelmet )
			{
				bEquipDefaultItem = FALSE;
				SetItemStatus(pstAgcdUnderWearItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);
			}
			else
			{
				bEquipDefaultItem = FALSE;
				AttachItem( pstAgpdCharacter->m_lTID1, pstAgcdCharacter, pcsUnderWearTemplate, pstAgcdUnderWearItem, pstAgcdUnderWearItemTemplate, eClassType );
				SetItemStatus(pstAgcdUnderWearItem, AGCDITEM_STATUS_WORLD_ADDED | AGCDITEM_STATUS_ATTACH);
			}

			SetEquipAnimType( pstAgpdCharacter, pstAgcdCharacter );
			pstAgcdCharacter->m_bEquipHelmet = TRUE;
		}
	}

	if( bEquipDefaultItem )
	{
		AttachedAtomics csAttachedAtomics;
		if ((pcsAgpdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR) && (pstAgcdCharacterTemplate->m_pDefaultArmourClump))
		{
			if(!EquipDefaultArmour(
				pstAgcdCharacter->m_pClump,
				pstAgcdCharacterTemplate->m_pDefaultArmourClump,
				pstAgcdCharacter->m_pInHierarchy,
				pcsAgpdItemTemplateEquip->GetPartIndex(),
				&csAttachedAtomics									)	) return FALSE;

			AttachedAtomicList	*pcsCurrent	= csAttachedAtomics.pcsList;
			while (pcsCurrent)
			{
				m_pcsAgcmCharacter->SetHierarchyForSkinAtomic(pcsCurrent->m_pstAtomic, (void *)(pstAgcdCharacter->m_pInHierarchy));
				pcsCurrent	= pcsCurrent->m_pcsNext;
			}

			RemoveAllAttachedAtomics(&csAttachedAtomics);
		}
	}

	ResetItemStatus(pstAgcdItem, AGCDITEM_STATUS_ATTACH);

	EnumCallback(AGCMITEM_CB_ID_DETACHED_ITEM, (PVOID)(pcsAgpdItem), (PVOID)(pstAgpdCharacter));

	if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_HALF)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 77, countEnd);
	else if(pstAgcdCharacter->m_cTransparentType == (INT8)AGCMCHAR_TRANSPARENT_FULL)
		m_pcsAgcmRender->InvisibleFxBegin(pstAgcdCharacter->m_pClump, 0, countEnd);

	// ������ ������ ���� �߰��� ó���ؾ��� �͵��� ó���Ѵ�.
	switch( pcsAgpdItemTemplateEquip->m_nKind )
	{
	case AGPMITEM_EQUIP_KIND_NECKLACE :
		{
			// ������� ��� ���������� ���� �ִ��� �˻��Ͽ� �̸� ó���Ѵ�.
			OnUnEquipItemNecklace( pstAgpdCharacter, pcsAgpdItem, pstAgcdItem );
		}
		break;
	};

	// ������ ���� �������� �ִϸ��̼� Ÿ�� ����
	SetEquipAnimType( pstAgpdCharacter, pstAgcdCharacter );
	return TRUE;
}

BOOL AgcmItem::_SetEquipAnimType( AgcdCharacter* pcdCharacter, INT32 nWeaponType )
{
	INT32	lAnimType2	= pcdCharacter->m_pstAgcdCharacterTemplate->m_lAnimType2;
	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetCharacter( pcdCharacter );
	BOOL bRidable = ppdCharacter ? ppdCharacter->m_bRidable : FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	if( !ppmCharacter || !pcmCharacter ) return FALSE;

	BOOL bIsPCCharacter = ppmCharacter->IsPC( ppdCharacter );
	if( bIsPCCharacter )
	{
		AuRace race = ppmCharacter->GetCharacterRace( ppdCharacter );
		AuCharClassType eClassType = pcmCharacter->GetClassTypeByTID( ppdCharacter->m_lTID1 );

		// �巹��ÿ��� �ƴ� ��� Ż���� �������� ���󹫱⸦ �����ϰ� �ִ����� üũ�Ѵ�.
		if( bRidable && race.detail.nRace != AURACE_TYPE_DRAGONSCION )
		{
			switch( nWeaponType )
			{
				// ������ �����Ѱ��
			case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE :
				pcdCharacter->m_lCurAnimType2 = GetAnimWeaponRide( lAnimType2 );
				break;				
				// �ٸ������� ���󹫱Ⱑ �ִµ�
			case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD :
				pcdCharacter->m_lCurAnimType2 = GetAnimStandardRide( lAnimType2 );
				break;
				// ���󹫱Ⱑ ���°��
			default:
				pcdCharacter->m_lCurAnimType2 = GetAnimDefaultRide( lAnimType2 );
				break;
			}
		}
		else // �巹�� �ÿ��̰ų� Ż���� Ÿ�� ������� ��� ��츦 üũ�Ѵ�.
		{
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :
				{
					switch( nWeaponType )
					{
					case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WARRIR_DEFAULT_RIDE : AGCMCHAR_AT2_WARRIR_DEFAULT;			break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_SLASH;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_AXE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_BLUNT;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SWORD:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_SLASH;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_AXE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE:
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_BLUNT;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_POLEARM:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_POLEARM;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SCYTHE:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_SCYTHE;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE:			pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WARRIR_WEAPON_RIDE : pcdCharacter->m_lCurAnimType2;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD:		pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WARRIR_STANDARD_RIDE : pcdCharacter->m_lCurAnimType2;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_RAPIERDAGGER:	pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_BLUNT;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_ONE_HAND_CHARON;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WARRIR_TWO_HAND_CHARON;	break;
					default:												return FALSE;
					}		
				}
				break;

			case AUCHARCLASS_TYPE_RANGER :
				{
					switch( nWeaponType )
					{
					case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE : AGCMCHAR_AT2_ARCHER_DEFAULT;			break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_BOW;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_CROSSBOW;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE:			pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_WEAPON_RIDE : pcdCharacter->m_lCurAnimType2;		break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_KATARIYA:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_KATARIYA;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD:		pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_STANDARD_RIDE : pcdCharacter->m_lCurAnimType2;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_ONE_HAND_ZENNON;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_TWO_HAND_ZENNON;	break;
					default:												return FALSE;
					}
				}
				break;

			case AUCHARCLASS_TYPE_MAGE :
				{
					if( race.detail.nRace == AURACE_TYPE_DRAGONSCION )
					{
						// �巹�� �ÿ� ���ӳ��� ���� ������ AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON
						pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON;
					}
					else
					{
						switch( nWeaponType )
						{
						case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WIZARD_DEFAULT_RIDE : AGCMCHAR_AT2_WIZARD_DEFAULT;			break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STAFF:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_STAFF;				break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_TROPHY:
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WAND:			pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_TROPHY;				break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_LANCE:			pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WIZARD_WEAPON_RIDE : pcdCharacter->m_lCurAnimType2;		break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHAKRAM:		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_TWO_HAND_CHAKRAM;	break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STANDARD:		pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_WIZARD_STANDARD_RIDE : pcdCharacter->m_lCurAnimType2;	break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
						case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON;	break;
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :
						case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_TWO_HAND_CHARON;	break;
						default:												return FALSE;
						}
					}
				}
				break;

			case AUCHARCLASS_TYPE_SCION :
				{
					switch( nWeaponType )
					{
					case AGPMITEM_EQUIP_WEAPON_TYPE_NONE:					pcdCharacter->m_lCurAnimType2 = bRidable ? AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE : AGCMCHAR_AT2_ARCHER_DEFAULT;			break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON :
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHER_ONE_HAND_ZENNON;	break;
					case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON :		
					case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON :		pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON;	break;
					default:												return FALSE;
					}
				}
				break;

			case AUCHARCLASS_TYPE_MAX :
				{
					switch( lAnimType2 )
					{
					case AGCMCHAR_AT2_BASE:
					case AGCMCHAR_AT2_BOSS_MONSTER:									pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_BASE_DEFAULT;				break;
					case AGCMCHAR_AT2_ARCHLORD :									pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHLORD_DEFAULT;			break;
					}
				}
			}
		}
	}
	else
	{
		switch (lAnimType2)
		{
		case AGCMCHAR_AT2_BASE:
		case AGCMCHAR_AT2_BOSS_MONSTER:									pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_BASE_DEFAULT;				break;
		case AGCMCHAR_AT2_ARCHLORD :									pcdCharacter->m_lCurAnimType2 = AGCMCHAR_AT2_ARCHLORD_DEFAULT;			break;
		}
	}

	return TRUE;
}

BOOL AgcmItem::_IsWearAvatarItem( AgpdCharacter* ppdCharacter, BOOL bIsCheckFace, BOOL bIsCheckHair )
{
	// �� ĳ���Ͱ� ���� �ƹ�Ÿ �������� �������ΰ��� �˻��Ѵ�.
	if( !ppdCharacter ) return FALSE;

	// �� ĳ���Ͱ� �������� �������� �� ������.
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgcmItem* pcmItem = ( AgcmItem* )GetModule( "AgcmItem" );
	AgpmGrid* ppmGrid = ( AgpmGrid* )GetModule( "AgpmGrid" );	
	if( !ppmItem || !ppmGrid || !pcmItem ) return FALSE;

	AgpdItemADChar* ppdItemADChar = ppmItem->GetADCharacter( ppdCharacter );
	AgpdGrid* ppdEquipGrid = &ppdItemADChar->m_csEquipGrid;

	INT32 nSequencialIndex = 0;	
	AgpdGridItem* ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	while( ppdGridItem )
	{
		// �ش� �׸����� �������� ã�´�.
		AgpdItem* ppdItemCurrent = ppmItem->GetItem( ppdGridItem->m_lItemID );
		if( !ppdItemCurrent ) continue;

		// �ƹ�Ÿ �������̸�.. 
		if( ppmItem->IsAvatarItem( ppdItemCurrent->m_pcsItemTemplate ) )
		{
			AgcdItemTemplate* pcdItemTemplate = pcmItem->GetTemplateData( ppdItemCurrent->m_pcsItemTemplate );
			if( pcdItemTemplate )
			{
				// bIsCheckFace ���� üũ�Ͽ� �� �ƹ�Ÿ �������� ���� �����Ű���� �˾ƺ���.
				if( bIsCheckFace && !pcdItemTemplate->m_bWithFace )
				{
					return TRUE;
				}

				// bIsCheckHair ���� üũ�Ͽ� �� �ƹ�Ÿ �������� ���� �����Ű���� �˾ƺ���.
				if( bIsCheckHair && !pcdItemTemplate->m_bWithHair )
				{
					return TRUE;
				}
			}
		}

		// ������..
		ppdGridItem = ppmGrid->GetItemSequence( ppdEquipGrid, &nSequencialIndex );
	}

	return FALSE;
}

RpAtomic* CallBack_FindAtomicArms( RpAtomic* pAtomic, void* pData )
{
	if( pAtomic )
	{
		RpAtomic** ppTargetAtomic = ( RpAtomic** )pData;
		RpAtomic* pTargetAtomic = *ppTargetAtomic;
		if( pAtomic->iPartID == AGPMITEM_PART_ARMS )
		{
			pTargetAtomic = pAtomic;
			return NULL;
		}
	}

	return pAtomic;
}