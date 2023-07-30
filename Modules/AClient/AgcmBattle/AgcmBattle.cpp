#include "AgcmBattle.h"
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcmUISystemMessage/AgcmUISystemMessage.h>

enum BattlePacketType {
	BATTLE_PT_CONFIRMATION,
	BATTLE_PT_ACCEPT,
	BATTLE_PT_DECLINE,
};

AgcmBattle::AgcmBattle()
{
	SetModuleName("AgcmBattle");
	SetModuleType(APMODULE_TYPE_CLIENT);
	SetPacketType(AGPM_BATTLE_PACKET_TYPE);
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(AUTYPE_INT8,	1,		// Operation
							AUTYPE_CHAR,	128,	// String
							AUTYPE_END,		0);
	EnableIdle(TRUE);
	m_pcsAgcmUIManager2 = NULL;
	m_pcsAgcmUISystemMessage = NULL;
	m_tBegin = 0;
	m_tUpdate = 0;
	m_pstTimerUD = NULL;
	m_lEventShowConfirm = 0;
	m_lEventHideConfirm = 0;
}

AgcmBattle::~AgcmBattle()
{
}

BOOL
AgcmBattle::OnAddModule()
{
	m_pcsAgcmUIManager2 = (AgcmUIManager2 *)GetModule("AgcmUIManager2");
	if(!m_pcsAgcmUIManager2)
		return FALSE;
	m_pcsAgcmUISystemMessage = (AgcmUISystemMessage *)GetModule(
		"AgcmUISystemMessage");
	if(!m_pcsAgcmUISystemMessage)
		return FALSE;
	m_pstTimerUD = m_pcsAgcmUIManager2->AddUserData("BattleTimer_UD",
		&m_tBegin, sizeof(time_t), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstTimerUD)
		return FALSE;
	m_lEventShowConfirm = m_pcsAgcmUIManager2->AddEvent(
		"EVENT_BATTLE_ShowConfirm");
	if (m_lEventShowConfirm < 0)
		return FALSE;
	m_lEventHideConfirm = m_pcsAgcmUIManager2->AddEvent(
		"EVENT_BATTLE_HideConfirm");
	if (m_lEventHideConfirm < 0)
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "BATTLE_DisplayTimer", 0,
		CBDisplayTimer, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"BATTLE_Accept", CBAccept, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"BATTLE_Decline", CBDecline, 0))
		return FALSE;
	//public module callback setting
	//m_pcsAgpmBattleGround->SetCallbackSystemMessage( CBSystemMessage, this );
	return TRUE;
}

BOOL
AgcmBattle::OnInit()
{
	return TRUE;
}

BOOL
AgcmBattle::OnDestroy()
{
	return TRUE;
}

BOOL
AgcmBattle::OnIdle(UINT32 ulClockCount)
{
	time_t t = time(NULL);
	
	if (t < m_tBegin + 45 && t != m_tUpdate) {
		m_tUpdate = t;
		m_pcsAgcmUIManager2->RefreshUserData(m_pstTimerUD, TRUE);
	}
	return TRUE;
}

BOOL
AgcmBattle::OnReceive(
	UINT32			ulType, 
	PVOID			pvPacket, 
	INT16			nSize, 
	UINT32			ulNID, 
	DispatchArg *	pstCheckArg)
{
	INT8 cOperation = -1;
	char * szStr = NULL;

	if(!pvPacket || nSize < 1)
		return FALSE;
	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&szStr);
	switch (cOperation) {
	case BATTLE_PT_CONFIRMATION: {
		FLASHWINFO flash;
		HWND hHwnd = RsGetWindowHandle();

		m_tBegin = time(NULL);
		m_tUpdate = m_tBegin;
		m_pcsAgcmUIManager2->RefreshUserData(m_pstTimerUD, TRUE);
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventShowConfirm);
		memset(&flash, 0, sizeof(FLASHWINFO));
		flash.cbSize = sizeof(FLASHWINFO);
		flash.hwnd = hHwnd;
		flash.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		flash.uCount = ~0U;
		flash.dwTimeout = 1000;
		FlashWindowEx(&flash);
		SetForegroundWindow(hHwnd);
	} break;
	case BATTLE_PT_DECLINE:
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventHideConfirm);
		if (szStr)
			m_pcsAgcmUISystemMessage->AddSystemMessage(szStr);
		break;
	}
	return TRUE;
}

BOOL
AgcmBattle::CBDisplayTimer(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	time_t t = time(NULL);
	time_t tBegin = *(time_t *)pData;
	UINT32 rt = (t < tBegin + 45) ? (UINT32)((tBegin + 45) - t) : 0u;

	sprintf(szDisplay, "%02u : %02u", rt / 60, rt % 60);
	return TRUE;
}

BOOL
AgcmBattle::CBAccept(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmBattle *	pThis = (AgcmBattle *)pClass;
	INT8			cOperation = BATTLE_PT_ACCEPT;
	INT16			nSize;
	PVOID			pPacket;

	pPacket = pThis->m_csPacket.MakePacket(TRUE, &nSize, AGPM_BATTLE_PACKET_TYPE,
		&cOperation,
		NULL);
	if (!pPacket || nSize < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nSize);
	pThis->m_csPacket.FreePacket(pPacket);
	return TRUE;
}

BOOL
AgcmBattle::CBDecline(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmBattle *	pThis = (AgcmBattle *)pClass;
	INT8			cOperation = BATTLE_PT_DECLINE;
	INT16			nSize;
	PVOID			pPacket;

	pPacket = pThis->m_csPacket.MakePacket(TRUE, &nSize, AGPM_BATTLE_PACKET_TYPE,
		&cOperation,
		NULL);
	if (!pPacket || nSize < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nSize);
	pThis->m_csPacket.FreePacket(pPacket);
	return TRUE;
}
