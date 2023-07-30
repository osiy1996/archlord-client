#include "AgsmBillInfo.h"
#include "AgppBillInfo.h"

AgsmBillInfo::AgsmBillInfo()
{
	SetModuleName("AgsmBillInfo");
}

AgsmBillInfo::~AgsmBillInfo()
{
}

BOOL AgsmBillInfo::OnAddModule()
{
	m_pcsAgpmBillInfo	= (AgpmBillInfo *)	GetModule("AgpmBillInfo");
	m_pcsAgsmCharacter	= (AgsmCharacter *)	GetModule("AgsmCharacter");

	if (!m_pcsAgpmBillInfo ||
		!m_pcsAgsmCharacter)
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackSendCharacterAllInfo(CBSendCharacterAllInfo, this))
		return FALSE;
	if (!m_pcsAgsmCharacter->SetCallbackSendCharacterNewID(CBSendCharacterAllInfo, this))
		return FALSE;

	return  TRUE;
}

BOOL AgsmBillInfo::OnInit()
{
	return TRUE;
}

BOOL AgsmBillInfo::OnDestroy()
{
	return TRUE;
}

BOOL AgsmBillInfo::CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmBillInfo	*pThis	= (AgsmBillInfo *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID	= *(UINT32 *)	pCustData;

#ifdef _AREA_GLOBAL_
	DWORD AccountGUID = pThis->m_pcsAgpmBillInfo->GetGUIDGlobal(pcsCharacter);
	if( AccountGUID != -1 )
	{
		PACKET_BILLINGINFO_GUID pPacket(pcsCharacter->m_lID, AccountGUID);
		pThis->SendPacketUser(pPacket, ulNID);
	}
#endif

	return TRUE;
}

BOOL AgsmBillInfo::SendCharacterBillingInfo(AgpdCharacter* pcsCharacter)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmBillInfo->MakePacketBillInfo(pcsCharacter, &nPacketLength);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	m_pcsAgpmBillInfo->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}