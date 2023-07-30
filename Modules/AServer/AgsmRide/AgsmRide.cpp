#include ".\agsmride.h"

AgsmRide::AgsmRide(void)
{
	SetModuleName("AgsmRide");
	EnableIdle(FALSE);

	m_pcsAgpmRide = NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgpmCharacter = NULL;
	m_pcsAgsmSkill = NULL;
	m_pcsAgsmCharacter = NULL;
	m_pcsAgsmSummons = NULL;
}

AgsmRide::~AgsmRide(void)
{
}

BOOL AgsmRide::OnAddModule()
{
	m_pcsApmMap = (ApmMap*)GetModule("ApmMap");
	m_pcsAgpmRide = (AgpmRide*)GetModule("AgpmRide");
	m_pcsAgpmItem = (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgsmSkill = (AgsmSkill*)GetModule("AgsmSkill");
	m_pcsAgsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgsmItem = (AgsmItem*)GetModule("AgsmItem");
	m_pcsAgsmSummons = (AgsmSummons*)GetModule("AgsmSummons");
	m_pcsAgsmAOIFilter = (AgsmAOIFilter*)GetModule("AgsmAOIFilter");

	if (!m_pcsApmMap || !m_pcsAgpmRide || !m_pcsAgsmSkill || !m_pcsAgsmCharacter || !m_pcsAgpmCharacter || !m_pcsAgpmFactors || 
		!m_pcsAgpmItem || !m_pcsAgsmItem || !m_pcsAgsmAOIFilter)
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackRideReq(CBRideReq, this))
		return FALSE;

	if (!m_pcsAgpmRide->SetCallbackDismountReq(CBDismountReq, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackReCalcFactor(CBReCalcFactors, this))
		return FALSE;

// JNY TODO : Factor Calc�� �ٲٸ鼭 �ʿ� ������
//	if (!m_pcsAgpmItem->SetCallbackEquip(CBCharItemEquip, this))
//		return FALSE;

	if (!m_pcsAgsmItem->SetCallbackRideMount(CBRideReq, this))
		return FALSE;

	if (!m_pcsAgsmItem->SetCallbackRideDisMount(CBDismountReq, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackEnterGameworld(CBEnterGameWorld, this))
		return FALSE;

	if (!m_pcsAgsmSkill->SetCallbackRide(CBRideReqBySkill, this))
		return FALSE;

	m_lIndexAttachData = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgsdADRideData), CBConstructor, CBDestructor);
	if (m_lIndexAttachData < 0)
		return FALSE;

	return TRUE;
}

BOOL AgsmRide::CBConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if((!pClass) || (!pData))
		return FALSE;

	AgsmRide *pThis = (AgsmRide *) pClass;
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter *) pData;
	AgsdADRideData *pcsADRideData = (AgsdADRideData *)pThis->GetAttachRideData(pstAgpdCharacter);
	pcsADRideData->m_lStartTime = 0;
	pcsADRideData->m_lStep = 0;

	return TRUE;
}

BOOL AgsmRide::CBDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

AgsdADRideData* AgsmRide::GetAttachRideData(AgpdCharacter *pcsCharacter)
{
	if(m_pcsAgpmCharacter)
		return (AgsdADRideData*)m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexAttachData, (PVOID)pcsCharacter);

	return NULL;
}

BOOL AgsmRide::OnInit()
{
	return TRUE;
}

BOOL AgsmRide::OnDestroy()
{
	return TRUE;
}

BOOL AgsmRide::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmRide::CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRide		*pThis			= (AgsmRide *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulClockCount	= PtrToUint(pCustData);

	if (FALSE == pcsCharacter->m_bRidable)
		return TRUE;

	AgsdADRideData	*pADRideData	= pThis->GetAttachRideData(pcsCharacter);
	if (NULL == pADRideData)
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBUpdateCharacter"));

	// �������� ���������� ���� �Ǿ��ִٸ� ���⼭ return TRUE�Ѵ�.
	if (TRUE == pThis->IsFreeDurationItem(pcsCharacter))
		return TRUE;

	// Ż���� 30�ʿ� �ѹ��� �������� ������.
	if ((pADRideData->m_lPrevTime + AGSDCHAR_IDLE_INTERVAL_THIRTY_SECONDS) > ulClockCount)
		return TRUE;

	// ���� PrevTime�� 0�̸� ó�� ������ ���̹Ƿ� �ð��� �����ϰ� �������� ���� �������� ������.
	if (0 == pADRideData->m_lPrevTime)
	{
		pADRideData->m_lPrevTime = ulClockCount;
		return TRUE;
	}

	pADRideData->m_lPrevTime = ulClockCount;
	return pThis->UpdateRideDurability(pcsCharacter, (AGSDCHAR_IDLE_INTERVAL_THIRTY_SECONDS / AGSDCHAR_IDLE_INTERVAL_ONE_SECOND), ulClockCount);
}

BOOL AgsmRide::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmRide		*pThis = (AgsmRide*)pClass;
	AgpdCharacter	*pcsCharacter = (AgpdCharacter*)pData;

	pThis->AutoRide(pcsCharacter);

	return TRUE;
}

// 2006.10.10. steeple
BOOL AgsmRide::CBRideReqBySkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRide* pThis = static_cast<AgsmRide*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	INT32 lRideTID = *static_cast<INT32*>(pCustData);

	BOOL bResult = FALSE;
	if(pcsCharacter->m_bRidable == FALSE)
	{
		// ź��.
		//
		// ���� üũ�� �Ѵ�.
		if(!pThis->CheckRideRegion(pcsCharacter))
			return FALSE;

		bResult = pThis->Ride(pcsCharacter, 0, lRideTID);
	}
	else
	{
		// ������.
		// ������ ���̵� �׳� RideTID �������.
		bResult = pThis->DismountReq(pcsCharacter, lRideTID, TRUE);
	}

	return bResult;
}

BOOL AgsmRide::IsFreeDurationItem(AgpdCharacter *pcsCharacter)
{
	AgpdGridItem *pcsAgpdGridItem = m_pcsAgpmItem->GetEquipItem( pcsCharacter, AGPMITEM_PART_RIDE );
	if (NULL == pcsAgpdGridItem)
		return FALSE;

	AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsAgpdGridItem);
	if (NULL == pcsItem)
		return FALSE;

	return (pcsItem->m_pcsItemTemplate->m_bFreeDuration);
}

BOOL AgsmRide::UpdateRideDurability(AgpdCharacter *pcsCharacter, INT32 lDurability, UINT32 ulClockCount, INT32 lItemID)
{
	AgpdItem *pcsItem = NULL;

	if (0 == lItemID)
	{
		AgpdGridItem *pcsAgpdGridItem = m_pcsAgpmItem->GetEquipItem( pcsCharacter, AGPMITEM_PART_RIDE );
		if (NULL == pcsAgpdGridItem)
			return FALSE;

		pcsItem = m_pcsAgpmItem->GetItem(pcsAgpdGridItem);
		if (NULL == pcsItem)
			return FALSE;
	}
	else
	{
		pcsItem = m_pcsAgpmItem->GetItem(lItemID);
		if (NULL == pcsItem)
			return FALSE;
	}

	if (AGPMITEM_EQUIP_KIND_RIDE != ((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind)
		return FALSE;

	INT32 lCurrentDurability = 0;
	INT32 lTemplateMaxDurability = 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_pcsItemTemplate->m_csFactor, &lTemplateMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lCurrentDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

	// lMaxDurability�� 0�̸� ������ ����Ҽ� �ֱ� ������ �������� ���ҽ�Ű�� �ʰ� ���⼭ �����Ѵ�.
	if (0 == lTemplateMaxDurability)
		return TRUE;

	INT32 lCalcDurability = lCurrentDurability - lDurability;
	if (0 >= lCalcDurability)
	{
		// �������� 0�̸��̸� ���� Dismount ��Ų��.
		m_pcsAgsmItem->UpdateItemDurability(pcsItem, -(lCurrentDurability));
		if (0 == lItemID)
			DismountReq(pcsCharacter, pcsItem->m_lID);
	}
	else
	{
		// 0�̻� �϶�
		m_pcsAgsmItem->UpdateItemDurability(pcsItem, -(lDurability));
	}

	return TRUE;
}

BOOL AgsmRide::CBReCalcFactors(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmRide		*pThis			= (AgsmRide *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (FALSE == pcsCharacter->m_bRidable)
		return FALSE;

	AgpdGridItem *pcsAgpdGridItem = pThis->m_pcsAgpmItem->GetEquipItem( pcsCharacter, AGPMITEM_PART_LANCER );
	if (NULL == pcsAgpdGridItem)
		return FALSE;

	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsAgpdGridItem);
	if (NULL == pcsItem)
		return FALSE;

	pThis->m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactorPercent, pcsItem->m_pcsItemTemplate->m_lRunBuff, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

	INT32	lDurability		= 0;
	INT32	lTemplateMaxDurability	= 0;
	pThis->m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);
	pThis->m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csFactor, &lTemplateMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);

	if ((lTemplateMaxDurability > 0 && lDurability < 1) || !pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	pThis->m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactorPercent, pcsItem->m_pcsItemTemplate->m_lHpBuff, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	pThis->m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactorPercent, pcsItem->m_pcsItemTemplate->m_lMpBuff, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	pThis->m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactorPercent, pcsItem->m_pcsItemTemplate->m_lAttackBuff, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_AP);

	return TRUE;
}

BOOL AgsmRide::CBCharItemEquip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmRide *pThis = (AgsmRide*)pClass;
	AgpdItem *pcsItem = (AgpdItem*)pData;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pCustData;

	return pThis->RideReq(pcsCharacter, pcsItem->m_lID);
}

BOOL AgsmRide::CBRideReq(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmRide		*pThis = (AgsmRide*)pClass;
	AgpdCharacter	*pcsAgpdCharacter = (AgpdCharacter*)pData;
	INT32			*pItemID = (INT32*)pCustData;

	return pThis->RideReq(pcsAgpdCharacter, *pItemID);
}

BOOL AgsmRide::CBDismountReq(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT(NULL != pData);
	ASSERT(NULL != pClass);
	ASSERT(NULL != pCustData);

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmRide		*pThis = (AgsmRide*)pClass;
	AgpdCharacter	*pcsAgpdCharacter = (AgpdCharacter*)pData;
	INT32			ItemID = *(INT32*)pCustData;

	return pThis->DismountReq(pcsAgpdCharacter, ItemID);
}

BOOL AgsmRide::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmRide* pThis = (AgsmRide*)pClass;
	AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	INT16 nPrevRegionIndex = pCustData ? *(INT16*)pCustData : -1;	// NULL �� �ö��� ADD_CHARACTER_TO_MAP ���� �Ҹ�����.

	return pThis->RegionChange(pcsAgpdCharacter, nPrevRegionIndex);
}

BOOL AgsmRide::AutoRide(AgpdCharacter* pcsAgpdCharacter)
{
	if (NULL == pcsAgpdCharacter)
		return TRUE;

	AgpdGridItem *pcsAgpdGridItem = m_pcsAgpmItem->GetEquipItem( pcsAgpdCharacter, AGPMITEM_PART_RIDE );
	if (NULL == pcsAgpdGridItem)
		return TRUE;

	AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsAgpdGridItem);
	if (NULL == pcsItem)
		return TRUE;

	RideReq(pcsAgpdCharacter, pcsItem->m_lID);

	return TRUE;
}

BOOL AgsmRide::RideReq(AgpdCharacter* pcsAgpdCharacter, INT32 lItemID)
{
	ASSERT(NULL != pcsAgpdCharacter);
	ASSERT(0 != lItemID);

	// �����ص� ������ �������� Ȯ��
	if(FALSE == CheckRideRegion(pcsAgpdCharacter))
		return FALSE;

	// ������ �˻�
	if(FALSE == CheckRideItem(pcsAgpdCharacter, lItemID))
		return FALSE;

	// 2007.08.04. steeple
	// ��ũ�ε�� Ż �� ����. ��ų�θ� Ż �� �����Ƿ� ��ų �ʿ��� üũ ���� �ʴ´�.
	if(m_pcsAgpmCharacter->IsArchlord(pcsAgpdCharacter))
		return FALSE;

	// �����ص� ������ ����
	return Ride(pcsAgpdCharacter, lItemID);
}

BOOL AgsmRide::DismountReq(AgpdCharacter* pcsAgpdCharacter, INT32 ItemID, BOOL bBySkill)
{
	pcsAgpdCharacter->m_bRidable = FALSE;

	AgsdADRideData* pRideData = GetAttachRideData(pcsAgpdCharacter);
	if (NULL == pRideData)
		return FALSE;

	// �������� �������� ���ð����� ����Ѵ�.
	if (0 != pRideData->m_lPrevTime && !bBySkill)
	{
		INT32 lDurability = (GetClockCount() - pRideData->m_lPrevTime) / AGSDCHAR_IDLE_INTERVAL_ONE_SECOND;
		UpdateRideDurability(pcsAgpdCharacter, lDurability, GetClockCount(), ItemID);
	}

	pRideData->m_lStartTime = 0;
	pRideData->m_lPrevTime = 0;
	pRideData->m_lStep = 0;

	// ĳ���͸� ������Ų��.
	pcsAgpdCharacter->m_bSync = TRUE;
	m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

	m_pcsAgsmCharacter->ReCalcCharacterFactors(pcsAgpdCharacter, TRUE);
	MakeAndSendPacket(AGPMRIDE_OPERATION_DISMOUNT_ACK, pcsAgpdCharacter, ItemID, 0);

	// 2006.01.06. steeple
	// �Ͻ� ������ ĳ�� ������ ��ų ��ũ���� ����Ѵ�.
	m_pcsAgsmItem->UsePausedCashItem(pcsAgpdCharacter, AGSDITEM_PAUSE_REASON_RIDE);
	m_pcsAgsmSkill->RecastSaveSkill(pcsAgpdCharacter);

	if (bBySkill && ItemID == 1295)
		m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_RIDE_ARCADIA);

	return TRUE;
}

BOOL AgsmRide::RegionChange(AgpdCharacter* pcsAgpdCharacter, INT16 nPrevRegionIndex)
{
	INT32 RegionIndex = m_pcsApmMap->GetRegion(pcsAgpdCharacter->m_stPos.x, pcsAgpdCharacter->m_stPos.z);
	ApmMap::RegionTemplate* pRegion = m_pcsApmMap->GetTemplate(RegionIndex);

	if (NULL == pRegion)
		return FALSE;

	if (FALSE == pRegion->ti.stType.bRidable)
	{
		// Ż���� �̿��Ҽ� ���� �����ε� Ż���� �����ϰ� ������ Ż���� Disable ��Ų��.
		if (TRUE == pcsAgpdCharacter->m_bRidable)
			DismountReq(pcsAgpdCharacter, 1);
	}
	else
	{
		if (FALSE == pcsAgpdCharacter->m_bRidable)
		{
			AgpdGridItem *pcsAgpdGridItem = m_pcsAgpmItem->GetEquipItem( pcsAgpdCharacter, AGPMITEM_PART_RIDE );
			if (NULL == pcsAgpdGridItem)
				return FALSE;

			AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsAgpdGridItem);
			if (NULL == pcsItem)
				return FALSE;

			RideReq(pcsAgpdCharacter, pcsItem->m_lID);
		}
	}

	return TRUE;
}

BOOL AgsmRide::CheckRideItem(AgpdCharacter* pcsAgpdCharacter, INT32 lItemID)
{
	ASSERT(pcsAgpdCharacter);
	ASSERT(lItemID);

	if(!pcsAgpdCharacter || !lItemID)
		return FALSE;

	// ������ �˻�
	AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(lItemID);
	if (NULL == pcsItem)
		return FALSE;

	if (AGPMITEM_EQUIP_KIND_RIDE != ((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind)
		return FALSE;

	INT32 lCurrentDurability = 0;
	INT32 lTemplateMaxDurability = 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_pcsItemTemplate->m_csFactor, &lTemplateMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY);
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lCurrentDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY);

	// lMaxDurability�� 0�̸� ������ ��밡��
	if ((0 != lTemplateMaxDurability) && (0 >= lCurrentDurability))
		return FALSE;

	return TRUE;
}

BOOL AgsmRide::CheckRideRegion(AgpdCharacter* pcsAgpdCharacter)
{
	ASSERT(pcsAgpdCharacter);
	if(!pcsAgpdCharacter)
		return FALSE;

	// �����ص� ������ �������� Ȯ��
	INT32 RegionIndex = m_pcsApmMap->GetRegion(pcsAgpdCharacter->m_stPos.x, pcsAgpdCharacter->m_stPos.z);
	ApmMap::RegionTemplate* pRegion = m_pcsApmMap->GetTemplate(RegionIndex);

	ASSERT(NULL != pRegion);
	if (NULL == pRegion)
		return FALSE;

	if (FALSE == pRegion->ti.stType.bRidable)
		return FALSE;

	return TRUE;
}

BOOL AgsmRide::Ride(AgpdCharacter* pcsAgpdCharacter, INT32 lItemID, INT32 lRideTID)
{
	ASSERT(pcsAgpdCharacter);
	if(!pcsAgpdCharacter)
		return FALSE;

	pcsAgpdCharacter->m_bRidable = TRUE;
	AgsdADRideData* pRideData = GetAttachRideData(pcsAgpdCharacter);
	if (NULL == pRideData)
		return FALSE;

	pRideData->m_lStartTime = GetClockCount();
	pRideData->m_lPrevTime = pRideData->m_lStartTime;
	pRideData->m_lStep = 0;

	// ĳ���͸� ������Ų��.
	pcsAgpdCharacter->m_bSync = TRUE;
	m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

	// ���� ��ų���� ��δ� ���ش�.
	// �ƹ�Ÿ�� Ż���� ������ �̼� ���� ������ ���������ش�.

	AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(lItemID);
	if (NULL == pcsItem)
		return FALSE;

	if(((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nPart == AGPMITEM_PART_V_RIDE)
		m_pcsAgsmSkill->EndMovementBuffedSkill(pcsAgpdCharacter);
	else
		m_pcsAgsmSkill->EndAllBuffedSkillExceptDebuff(pcsAgpdCharacter, TRUE);

	// 2005.12.27. steeple
	// Ż �� Ÿ�� ������ �ʾƾ� �ϴ� ĳ���������� ����� �����.
	m_pcsAgsmItem->UnUseAllCashItem(pcsAgpdCharacter, TRUE, AGSDITEM_PAUSE_REASON_RIDE);

	// ���� ���¿����� Ǯ����� ��~ 2005.10.07. steeple
	if (m_pcsAgpmCharacter->IsStatusTransparent(pcsAgpdCharacter) == TRUE)
		m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);


	// ��ȯ�� ���� �͵� ��� ���ش�. 2005.10.07. steeple
	if(m_pcsAgsmSummons)
	{
		m_pcsAgsmSummons->RemoveAllSummons(pcsAgpdCharacter,
						AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS | AGSMSUMMONS_CB_REMOVE_TYPE_TAME | AGSMSUMMONS_CB_REMOVE_TYPE_FIXED);
	}

	m_pcsAgsmCharacter->ReCalcCharacterFactors(pcsAgpdCharacter, TRUE);
	if(lItemID)
		MakeAndSendPacket(AGPMRIDE_OPERATION_RIDE_ACK, pcsAgpdCharacter, lItemID, 0);
	else if(lRideTID)
	{
		//if (lRideTID == 1295)
			m_pcsAgpmCharacter->UpdateSetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_RIDE_ARCADIA);
		MakeAndSendPacket(AGPMRIDE_OPERATION_RIDE_TID, pcsAgpdCharacter, lRideTID, 0);
	}

	// 2005.12.26. steeple
	// Ż �� �̿� �߿��� ������ ĳ�� ������(��ų) �� Use �Ѵ�.
	m_pcsAgsmItem->UseAllEnableCashItem(pcsAgpdCharacter, TRUE);

	return TRUE;
}

BOOL AgsmRide::MakeAndSendPacket(INT8 cOperation, AgpdCharacter *pcsCharacter, INT32 lRideItemID, INT32 lRemainTime)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmRide->MakePacketRide(&nPacketLength, cOperation, pcsCharacter->m_lID, lRideItemID, lRemainTime);

	ASSERT(NULL != pvPacket);
	if (NULL == pvPacket)
		return FALSE;

	m_pcsAgsmAOIFilter->SendPacketNearExceptSelf(pvPacket, nPacketLength, pcsCharacter->m_stPos,
												m_pcsAgpmCharacter->GetRealRegionIndex(pcsCharacter),
												m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_4);

	BOOL bPacketResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));

	m_pcsAgpmRide->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}