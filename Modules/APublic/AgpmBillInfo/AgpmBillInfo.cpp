
#include <AgpmBillInfo/AgpmBillInfo.h>
#include <AgpmBillInfo/AgppBillInfo.h>
#include <AgpmCharacter/AgpmCharacter.h>

AgpmBillInfo::AgpmBillInfo()
{
	SetModuleName("AgpmBillInfo");

	SetPacketType(AGPMBILLINFO_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,	1,		// operation
							AUTYPE_INT32,	1,		// character id
							AUTYPE_INT8,	1,		// payment type
							AUTYPE_UINT32,	1,		// remain coupon play time
							AUTYPE_INT8,	1,		// is pc-room
							AUTYPE_UINT32,	1,		// PCRoomType
							AUTYPE_END,		0
							);
}

AgpmBillInfo::~AgpmBillInfo()
{
}

BOOL AgpmBillInfo::OnAddModule()
{
	m_pcsAgpmCharacter	= (AgpmCharacter *)	GetModule("AgpmCharacter");

	if (!m_pcsAgpmCharacter)
		return FALSE;

	m_lIndexADCharacter	= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdBillInfo), NULL, NULL);
	if (m_lIndexADCharacter < 0)
		return FALSE;

	return TRUE;
}

BOOL AgpmBillInfo::OnInit()
{
	return TRUE;
}

BOOL AgpmBillInfo::OnDestroy()
{
	return TRUE;
}

AgpdBillInfo* AgpmBillInfo::GetADCharacter(AgpdCharacter *pcsCharacter)
{
	if (m_pcsAgpmCharacter)
		return (AgpdBillInfo *) m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexADCharacter, (PVOID) pcsCharacter);

	return NULL;
}

BOOL AgpmBillInfo::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1 || !pstCheckArg)
		return FALSE;

	INT8	cOperation				= (-1);
	INT32	lCID					= AP_INVALID_CID;
	INT8	cPaymentType			= (-1);
	UINT32	ulRemainCouponPlayTime	= 0;
	INT8	cIsPCRoom				= (-1);
	UINT32	ulPCRoomType			= AGPDPCROOMTYPE_NONE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&cPaymentType,
						&ulRemainCouponPlayTime,
						&cIsPCRoom,
						&ulPCRoomType);

	switch (cOperation) 
	{
		case AGPMBILL_OPERATION_ADD:
			{
				AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
				if (!pcsCharacter)
					return FALSE;

				AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);

				pcsAttachData->m_ePaymentType	= (AgpdPaymentType)	cPaymentType;
				pcsAttachData->m_ulRemainCouponPlayTime	= ulRemainCouponPlayTime;
				pcsAttachData->m_bIsPCRoom		= (BOOL)	cIsPCRoom;
				pcsAttachData->m_ulPCRoomType	= (UINT32)ulPCRoomType;

				EnumCallback(AGPMBILL_UPDATE_BIIL_INFO, pcsCharacter, NULL);

				pcsCharacter->m_Mutex.Release();
			} break;
		case AGPMBILL_OPERATION_CASHINFO:
			{
				PACKET_BILLINGINFO_CASHINFO* pPacket2 = (PACKET_BILLINGINFO_CASHINFO*)pvPacket;

				AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(pPacket2->lCID);
				if (!pcsCharacter)
					return FALSE;

#ifdef _AREA_GLOBAL_
				SetCashGlobal(pcsCharacter, (double)pPacket2->m_pWCoin, (double)pPacket2->m_pPCoin);
				m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_UPDATE_CASH, pcsCharacter, NULL);
#else
				m_pcsAgpmCharacter->SetCash( pcsCharacter, pPacket2->m_pWCoin );
#endif

				pcsCharacter->m_Mutex.Release();
			} break;
		case AGPMBILL_OPERATION_GUID:
			{
				PACKET_BILLINGINFO_GUID* pPacket2 = (PACKET_BILLINGINFO_GUID*)pvPacket;

				AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(pPacket2->lCID);
				if (!pcsCharacter)
					return FALSE;

				DWORD AccountGUID = pPacket2->m_dwAccountGUID;

				SetGUIDGlobal(pcsCharacter, AccountGUID);

				pcsCharacter->m_Mutex.Release();
			} break;
	}

	return TRUE;
}

PVOID AgpmBillInfo::MakePacketBillInfo(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength)
{
	if (!pcsCharacter)
		return NULL;

	AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);

	INT8	cOperation		= AGPMBILL_OPERATION_ADD;
	INT8	cPaymentType	= (INT8)	pcsAttachData->m_ePaymentType;
	INT8	cIsPCRoom		= (INT8)	pcsAttachData->m_bIsPCRoom;
	UINT32	ulPCRoomType	= (UINT32)	pcsAttachData->m_ulPCRoomType;

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMBILLINFO_PACKET_TYPE,
								 &cOperation,
								 &pcsCharacter->m_lID,
								 &cPaymentType,
								 &pcsAttachData->m_ulRemainCouponPlayTime,
								 &cIsPCRoom,
								 &ulPCRoomType);
}

BOOL AgpmBillInfo::UpdateIsPCRoom(AgpdCharacter *pcsCharacter, BOOL bIsPCRoom)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);

	pcsAttachData->m_bIsPCRoom	= bIsPCRoom;

	return EnumCallback(AGPMBILL_UPDATE_BIIL_INFO, pcsCharacter, NULL);
}

BOOL AgpmBillInfo::SetCallbackUpdateBillInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBILL_UPDATE_BIIL_INFO, pfCallback, pClass);
}

BOOL AgpmBillInfo::IsPCBang(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);

	return pcsAttachData->m_bIsPCRoom;
}

BOOL AgpmBillInfo::SetCashGlobal(AgpdCharacter* pcsCharacter, double WCoin, double PCoin)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);
	if(!pcsAttachData)
		return FALSE;

	pcsAttachData->m_CashInfoGlobal.m_WCoin = WCoin;
	pcsAttachData->m_CashInfoGlobal.m_PCoin = PCoin;

	return TRUE;
}

BOOL AgpmBillInfo::GetCashGlobal(AgpdCharacter* pcsCharacter, double& WCoin, double& PCoin)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);
	if(!pcsAttachData)
		return FALSE;

	WCoin = pcsAttachData->m_CashInfoGlobal.m_WCoin;
	PCoin = pcsAttachData->m_CashInfoGlobal.m_PCoin;

	return TRUE;
}

BOOL AgpmBillInfo::SetGUIDGlobal( AgpdCharacter* pcsCharacter, DWORD AccountGUID )
{
	if (!pcsCharacter)
		return FALSE;

	AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);
	if(!pcsAttachData)
		return FALSE;

	pcsAttachData->m_CashInfoGlobal.m_dwAccountGUID = AccountGUID;

	return TRUE;
}

DWORD AgpmBillInfo::GetGUIDGlobal( AgpdCharacter* pcsCharacter )
{
	if (!pcsCharacter)
		return -1;

	AgpdBillInfo	*pcsAttachData	= GetADCharacter(pcsCharacter);
	if(!pcsAttachData)
		return -1;

	return pcsAttachData->m_CashInfoGlobal.m_dwAccountGUID;
}

