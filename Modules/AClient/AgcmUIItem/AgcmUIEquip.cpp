#include <AgcmUIItem/AgcmUIItem.h>
#include <AgcmUIControl/AcUIGrid.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmItem/AgcmItem.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcModule/AgcEngine.h>
#include <AgpmGrid/AgpmGrid.h>


BOOL AgcmUIItem::CBEquipMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pcsSourceControl;
	AgpdGridSelectInfo *	pstGridSelectInfo;
	AgpdGrid *				pstGrid;
	AcUIGrid *				pcsUIGrid;

	BOOL					bResult;

	bResult = FALSE;

	// ���� �������̶�� ������ �ϸ� �ȵȴ�.
	if (pThis->m_bIsProcessConvert)
		return TRUE;

	//���� ��ġ���� �̵��� Layer, Row, Column�� �������� �ű�ٴ� ��Ŷ�� ������.
	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return bResult;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pstGridSelectInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo)
		return bResult;

	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);

	if (pstGrid)
	{
		INT32			lItemID;
		AgpdItem		*pcsAgpdItem;

		lItemID = pstGridSelectInfo->pGridItem->m_lItemID;

		pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( lItemID );

		if( pcsAgpdItem != NULL )
		{
			// ���� �� �������� ���밡������ �˻�
			if( !pThis->OnCheckEquipEnable( pcsAgpdItem ) )
				return FALSE;

			//�κ����� Equipâ���� �Ű��� ���
			if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_INVENTORY )
			{
				bResult = pThis->m_pcsAgcmItem->SendEquipInfo( lItemID, pThis->m_pcsAgcmCharacter->m_lSelfCID );
			}
		}
	}

	return bResult;

}

BOOL AgcmUIItem::CBEquipDragDropBase(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIItem	*pThis	= (AgcmUIItem *)	pClass;

	if (!pcsSourceControl->m_pcsBase)
		return FALSE;
	
	AcUIDragInfo	*pcsUIDragInfo	= pcsSourceControl->m_pcsBase->GetDragDropMessage();
	if (!pcsUIDragInfo || !pcsUIDragInfo->pSourceWindow)
		return FALSE;

	if (pcsUIDragInfo->pSourceWindow->m_nType != pcsUIDragInfo->pSourceWindow->TYPE_GRID_ITEM)
		return FALSE;

	AcUIGridItem	*pcsUIGridItem	= (AcUIGridItem *) pcsUIDragInfo->pSourceWindow;

	if (!pcsUIGridItem->m_ppdGridItem)
		return FALSE;

	if (pcsUIGridItem->m_ppdGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;

	AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pcsUIGridItem->m_ppdGridItem);

	// ������ ���밡�� �˻�
	if( !pThis->OnCheckEquipEnable( pcsItem ) ) return FALSE;

	if (pcsItem && pcsItem->m_eStatus == AGPDITEM_STATUS_INVENTORY)
	{
		pThis->m_pcsAgcmItem->SendEquipInfo(pcsItem->m_lID, pThis->m_pcsAgcmCharacter->GetSelfCID());
	}

	return TRUE;
}

BOOL AgcmUIItem::CBEquipItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis		= (AgcmUIItem *)	pClass;

	if (!pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return TRUE;

	AgpdItem		*pcsItem	= (AgpdItem *)		pData;

	/*
	// ���� �Ӽ��� �ִ� �������̶�� ĳ���� ����â�� ��ȭ�� �ش�.
	if (pThis->m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) > 0)
	{
		AgcmUIUpdateCharStatus	stUpdateCharStatus;
		ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

		pThis->SetUpdateCharStatus(pcsItem, &stUpdateCharStatus);

		pThis->m_pcsAgcmUICharacter->SetUpdateCharStatus(stUpdateCharStatus);
	}
	*/

	return pThis->CheckItemDurability(pcsItem);
}

BOOL AgcmUIItem::CBUnEquipItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis		= (AgcmUIItem *)	pClass;
	AgpdItem		*pcsItem	= (AgpdItem *)		pData;

	pThis->CheckItemDurability(pcsItem);

	//pThis->RemoveItemDurabilityZeroList(pcsItem);
	//pThis->RemoveItemDurabilityUnder5PercentList(pcsItem);

	/*
	// ���� �Ӽ��� �ִ� �������̶�� ĳ���� ����â ��ȭ�� �ʱ�ȭ���ش�.
	if (pThis->m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) > 0)
	{
		AgcmUIUpdateCharStatus	stUpdateCharStatus;
		ZeroMemory(&stUpdateCharStatus, sizeof(AgcmUIUpdateCharStatus));

		pThis->SetUpdateCharStatus(pcsItem, &stUpdateCharStatus);

		pThis->m_pcsAgcmUICharacter->ResetUpdateCharStatus(stUpdateCharStatus);
	}
	*/

	return TRUE;
}

/*
BOOL AgcmUIItem::SetUpdateCharStatus(AgpdItem *pcsItem, AgcmUIUpdateCharStatus *pstUpdateCharStatus)
{
	if (!pcsItem || !pstUpdateCharStatus)
		return FALSE;

	if (m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem) > 0)
	{
		switch (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind) {
		case AGPMITEM_EQUIP_KIND_WEAPON:
			{
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC) > 0)
				{
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_MAGIC_MIN]	= 1;
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_MAGIC_MAX]	= 1;
				}
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE) > 0)
				{
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_FIRE_MIN]	= 1;
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_FIRE_MAX]	= 1;
				}
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER) > 0)
				{
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_WATER_MIN]	= 1;
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_WATER_MAX]	= 1;
				}
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR) > 0)
				{
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_AIR_MIN]	= 1;
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_AIR_MAX]	= 1;
				}
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH) > 0)
				{
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_EARTH_MIN]	= 1;
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_EARTH_MAX]	= 1;
				}
			}
			break;

		default:
			{
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC) > 0)
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DEFENSE_MAGIC]	= 1;
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE) > 0)
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DEFENSE_FIRE]	= 1;
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER) > 0)
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DEFENSE_WATER]	= 1;
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR) > 0)
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DEFENSE_AIR]	= 1;
				if (m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH) > 0)
					pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DEFENSE_EARTH]	= 1;
			}
			break;
		}

		AgpdItemConvertADItem	*pcsAttachItem	= m_pcsAgpmItemConvert->GetADItem(pcsItem);

		for (int i = 0; i < m_pcsAgpmItemConvert->GetNumConvertedSocket(pcsItem); ++i)
		{
			if (!pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate ||
				pcsAttachItem->m_stSocketAttr[i].bIsSpiritStone)
				continue;

			if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_DAMAGE)
			{
				pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_PHYSICAL_MIN]	= 1;
				pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DAMAGE_PHYSICAL_MAX]	= 1;
			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_AR)
			{
			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_BLOCK_RATE)
			{
			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_AC)
			{
				pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_DEFENSE_PHYSICAL] = 1;
			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_MAX_HP_POINT ||
					 ((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_MAX_HP_PERCENT)
			{
				pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_HP_MAX] = 1;
			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_MAX_MP_POINT ||
					 ((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_MAX_MP_PERCENT)
			{
				pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_MP_MAX] = 1;
			}

//			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_MAX_SP)
//			{
//				pstUpdateCharStatus->lCharStatus[AGCMUICHAR_STATUS_TYPE_SP_MAX] = 1;
//			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_HP_REGEN)
			{
			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_MP_REGEN)
			{
			}

//			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_RECOVERY_SP)
//			{
//			}

			else if (((AgpdItemTemplateUsableRune *) pcsAttachItem->m_stSocketAttr[i].pcsItemTemplate)->m_eRuneAttributeType == AGPMITEM_RUNE_ATTR_ADD_ATTACK_SPEED)
			{
			}
		}
	}

	return TRUE;
}
*/

BOOL AgcmUIItem::UpdateEquipSlotGrid()
{
	AgpdCharacter* ppdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_BODY,					m_pcsEquipSlotBody );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_HEAD,					m_pcsEquipSlotHead );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_HANDS, 				m_pcsEquipSlotHands );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_LEGS,					m_pcsEquipSlotLegs );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_FOOT,					m_pcsEquipSlotFoot );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_HAND_LEFT,				m_pcsEquipSlotHandLeft );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_HAND_RIGHT,			m_pcsEquipSlotHandRight );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_ACCESSORY_RING1,		m_pcsEquipSlotRing1 );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_ACCESSORY_RING2,		m_pcsEquipSlotRing2 );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_ACCESSORY_NECKLACE,	m_pcsEquipSlotNecklace );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_RIDE,					m_pcsEquipSlotRide );
	UpdateEquipSlot( ppdCharacter, AGPMITEM_PART_LANCER,				m_pcsEquipSlotLancer );
	return TRUE;
}

BOOL AgcmUIItem::UpdateEquipSlot( void* pCharacter, INT32 nPartID, void* pSlotUserData )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgcdUIUserData* pcdSlotUserData = ( AgcdUIUserData* )pSlotUserData;
	if( !ppdCharacter || !m_pcsAgpmGrid || !m_pcsAgcmUIManager2 || !pcdSlotUserData ) return FALSE;

	AgpmItemPart ePartType = ( AgpmItemPart )nPartID;

	// �ش� ���� �ʱ�ȭ
	m_pcsAgpmGrid->Reset( &m_csEquipSlotGrid[ ePartType ] );

	// �ش� ���Կ� �� �������� �����ͼ�..
	AgpdGridItem* ppdGridItem = m_pcsAgpmItem->GetEquipItem( ppdCharacter, ePartType );
	if( ppdGridItem )
	{
		AgpdItem* ppdItem = m_pcsAgpmItem->GetItem( ppdGridItem->m_lItemID );
		if( ppdItem )
		{
			// �ش� ���Կ� �ٽ� ����ְ�..
			m_pcsAgpmGrid->Add( &m_csEquipSlotGrid[ ePartType ], ppdGridItem, 1, 1 );
		}
		else
		{
			// ���Կ� �������� ������ ������ �ʱ�ȭ �Ѵ�... �����ʹ� �̹� �ʱ�ȭ �Ǿ����� ���� �׸��忡�� �ؽ�ó�� ��ư�� �����Ѵ�.
			m_csEquipSlotGrid[ ePartType ].Clear();
		}
	}

	// ��밡�ɿ��ε� �� üũ���ְ�..
	AuCharClassType eClassType = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdCharacter->m_csFactor );
	UpdateEquipSlotEnable( ppdCharacter, nPartID, eClassType );

	// ��������Ÿ ����
	m_pcsAgcmUIManager2->SetUserDataRefresh( pcdSlotUserData );
	return TRUE;
}

BOOL AgcmUIItem::UpdateEquipSlotEnable( void* pCharacter, INT32 nPartID, AuCharClassType eClassType )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	if( !ppdCharacter || !m_pcsAgpmGrid ) return FALSE;

	// �ش� ���Կ� �� �������� �����ͼ�..
	AgpmItemPart ePartType = ( AgpmItemPart )nPartID;
	AgpdGridItem* ppdGridItem = m_pcsAgpmItem->GetEquipItem( ppdCharacter, ePartType );
	if( !ppdGridItem ) return FALSE;

	// �� ������ �������� ���Ұ����� ���¶�� ������ ������� UI ���� �������ش�.
	BOOL bIsUsable = UpdateEquipSlotEnable( ppdCharacter, ppdGridItem, eClassType );
	INT32 nEvent = GetEquipSlotMaskEvent( nPartID, !bIsUsable );
	if( nEvent >= 0 )
	{
		m_pcsAgcmUIManager2->ThrowEvent( nEvent );
	}

	return TRUE;
}

BOOL AgcmUIItem::UpdateEquipSlotEnable( void* pCharacter, void* pGridItem, AuCharClassType eClassType )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdGridItem* ppdGridItem = ( AgpdGridItem* )pGridItem;
	if( !ppdCharacter || !ppdGridItem ) return FALSE;

	// �ش罽�Կ� �������� ������ �� �Ѿ��..
	AgpdItem* ppdItem = m_pcsAgpmItem->GetItem( ppdGridItem->m_lItemID );
	if( !ppdItem ) return FALSE;

	// ��Ȱ��ȭ �Ͽ��� �Ѵٸ� ��Ȱ��ȭ ó��.. �������� 0 �� ��ó�� ó���Ѵ�.
	BOOL bIsUsable = IsUsableItem( ppdCharacter, ppdItem, eClassType );
	BOOL bIsDurationUnder20 = IsAlertItemDurationUnder20( ppdItem );
	BOOL bIsDisable = !bIsUsable || bIsDurationUnder20;
	ppdGridItem->SetDurabilityZero( bIsDisable );
	return !bIsDisable;
}

BOOL AgcmUIItem::IsUsableItem( void* pCharacter, void* pItem, AuCharClassType eClassType )
{
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCharacter;
	AgpdItem* ppdItem = ( AgpdItem* )pItem;
	if( !ppdCharacter || !ppdItem || !m_pcsAgpmFactors || !m_pcsAgpmCharacter || !m_pcsAgcmItem ) return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return FALSE;

	// NPC �� ������ ��.. �˻���Ѵ�.. �� ���� ����..
	if( !ppmCharacter->IsPC( ppdCharacter ) ) return TRUE;

	// ������ ���� ������ ���Ұ�
	INT32 nRequireLevel = m_pcsAgpmFactors->GetLevel( &ppdItem->m_csRestrictFactor );
	INT32 nCharacterLevel = m_pcsAgpmCharacter->GetLevel( ppdCharacter );
	if( nCharacterLevel < nRequireLevel ) return FALSE;

	// ��밡���� ������ ���� ������ ���Ұ�
	INT32 nRequireRace = m_pcsAgpmFactors->GetRace( &ppdItem->m_csRestrictFactor );
	AuRaceType eCharacterRace = ( AuRaceType )m_pcsAgpmFactors->GetRace( &ppdCharacter->m_csFactor );
	if( !_IsEnableRace( nRequireRace, eCharacterRace ) ) return FALSE;

	// ��밡���� Ŭ������ ���� ������ ���Ұ�
	INT32 nRequireClass = m_pcsAgpmFactors->GetClass( &ppdItem->m_csRestrictFactor );
	AuCharClassType eCharacterClass = AUCHARCLASS_TYPE_NONE;

	// �Ű������� Ŭ����Ÿ�Ը��� ������ ĳ������ Ŭ����Ÿ�Ժ��� �Ű������� ���� üũ�Ѵ�.
	if( eClassType != AUCHARCLASS_TYPE_NONE )
	{
		eCharacterClass = eClassType;
	}
	else
	{
		eCharacterClass = ( AuCharClassType )m_pcsAgpmFactors->GetClass( &ppdCharacter->m_csFactor );
	}
	
	if( !_IsEnableClass( nRequireClass, eCharacterClass ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::IsAlertItemDurationUnder20( void* pItem )
{
	// ������ 20% �̸��̸� ���Ұ�
	AgpdItem* ppdItem = ( AgpdItem* )pItem;
	INT32 nDurablePercent = m_pcsAgpmItem->GetItemDurabilityPercent( ppdItem );
	if( nDurablePercent < 0 ) return FALSE;
	return nDurablePercent <= 20 ? TRUE : FALSE;
}

INT32 AgcmUIItem::GetEquipSlotMaskEvent( INT32 nPartID, BOOL bIsOnEvent )
{
	AgpmItemPart ePartType = ( AgpmItemPart )nPartID;
	switch( ePartType )
	{
	case AGPMITEM_PART_BODY :				return bIsOnEvent ? m_lEventEquipStatusBodyOn 	: m_lEventEquipStatusBodyOff;		break;
	case AGPMITEM_PART_HEAD :				return bIsOnEvent ? m_lEventEquipStatusHeadOn 	: m_lEventEquipStatusHeadOff;		break;
	case AGPMITEM_PART_HANDS :				return bIsOnEvent ? m_lEventEquipStatusHandOn 	: m_lEventEquipStatusHandOff;		break;
	case AGPMITEM_PART_LEGS :				return bIsOnEvent ? m_lEventEquipStatusLegsOn 	: m_lEventEquipStatusLegsOff;		break;
	case AGPMITEM_PART_FOOT :				return bIsOnEvent ? m_lEventEquipStatusFootOn 	: m_lEventEquipStatusFootOff;		break;
	case AGPMITEM_PART_HAND_LEFT :			return bIsOnEvent ? m_lEventEquipStatusShieldOn : m_lEventEquipStatusShieldOff;		break;
	case AGPMITEM_PART_HAND_RIGHT :			return bIsOnEvent ? m_lEventEquipStatusWeaponOn : m_lEventEquipStatusWeaponOff;		break;
	}

	return -1;
}

BOOL AgcmUIItem::IsEnableUnEquipItem( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter )
{
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return FALSE;

	AgpmFactors* ppmFactor = ( AgpmFactors* )g_pEngine->GetModule( "AgpmFactors" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmFactor || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdTargetCharacter = ppdCharacter;
	if( !ppdTargetCharacter )
	{
		// �Ű������� NULL �� ���� �ڱ� �ڽ����� �����Ѵ�.
		ppdTargetCharacter = pcmCharacter->GetSelfCharacter();
		if( !ppdTargetCharacter ) return FALSE;
	}

	AuRaceType eRaceType = ( AuRaceType )ppmFactor->GetRace( &ppdTargetCharacter->m_csFactor );
	AuCharClassType eClassType = pcmCharacter->GetClassTypeByTID( ppdTargetCharacter->m_lTID1 );

	// �巡��ÿ¿� ���� �Ʒ��� ���ѻ����� �߰�
	if( eRaceType == AURACE_TYPE_DRAGONSCION )
	{
		AgpdItemTemplate* ppdItemTemplate = ppdItem->m_pcsItemTemplate;
		if( !ppdItemTemplate || ppdItemTemplate->m_nType != AGPMITEM_TYPE_EQUIP ) return FALSE;

		AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItemTemplate;
		if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON )
		{
			// ������ ���...
			AgpdItemTemplateEquipWeapon* ppdItemTemplateEquipWeapon = ( AgpdItemTemplateEquipWeapon* )ppdItemTemplateEquip;
			switch( eClassType )
			{
			case AUCHARCLASS_TYPE_KNIGHT :
			case AUCHARCLASS_TYPE_MAGE :
				{
					if( ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON || 
						ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_ZENON )
					{
						// �����̾�� ���ӳʴ� ������ ���� �� ����.
						return FALSE;
					}
				}
				break;

			case AUCHARCLASS_TYPE_RANGER :
				{
					if( ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON || 
						ppdItemTemplateEquipWeapon->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CHARON )
					{
						// �����ʹ� ī���� ���� �� ����.
						return FALSE;
					}
				}
				break;
			}			
		}
	}

	return TRUE;
}
