#include "AgcmArena.h"
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmUIControl/AcUIEdit.h>

enum ArenaPacketType {
	ARENA_PT_SET				= 0x00,
	ARENA_PT_LEADERBOARD		= 0x01,
	ARENA_PT_REGISTER_NORMAL	= 0x02,
	ARENA_PT_DEREGISTER			= 0x03,
	ARENA_PT_IN_QUEUE_COUNT		= 0x04,
	ARENA_PT_REGISTER_RANKED	= 0x05,
};

AgcmArena::AgcmArena()
{
	SetModuleName("AgcmArena");
	SetModuleType(APMODULE_TYPE_CLIENT);
	SetPacketType(AGPM_ARENA_PACKET_TYPE);
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		AUTYPE_INT8,	1,	// Operation
		AUTYPE_INT32,	1,	// Arena points
		AUTYPE_INT32,	1,	// Arena coins
		AUTYPE_PACKET,	1,	// Leaderboard
		AUTYPE_INT32,	1,	// In queue count
		AUTYPE_END,		0);
	m_csLeaderboardPacket.SetFlagLength(sizeof(INT8));
	m_csLeaderboardPacket.SetFieldType(
		AUTYPE_INT8,	1,	// 
		AUTYPE_INT32,	1,	// Index
		AUTYPE_INT32,	1,	// Entry count
		AUTYPE_PACKET,	16,	// Entries
		AUTYPE_END,		0);
	m_csEntryPacket.SetFlagLength(sizeof(INT8));
	m_csEntryPacket.SetFieldType(
		AUTYPE_CHAR,	32,	// Player name
		AUTYPE_INT32,	1,	// Arena points
		AUTYPE_INT32,	1,	// Win count
		AUTYPE_INT32,	1,	// Lose count
		AUTYPE_END,		0);
	EnableIdle( FALSE );
	/* Initialize member variables. */
	m_pcsAgcmUIManager2 = NULL;
	m_ulArenaPoints = 0;
	m_ulArenaCoins = 0;
	m_pstArenaUD = NULL;
	m_pstLeaderboardUD = NULL;
	memset(m_stLeaderboard, 0, sizeof(m_stLeaderboard));
	m_ulLeaderboardIndex = 0;
	m_ulLeaderboardCount = 0;
	m_bArenaRegisteredNormal = FALSE;
	m_bArenaRegisteredRanked = FALSE;
}

AgcmArena::~AgcmArena()
{
}

BOOL
AgcmArena::OnAddModule()
{
	m_pcsAgcmUIManager2 = (AgcmUIManager2 *)GetModule("AgcmUIManager2");
	if(!m_pcsAgcmUIManager2)
		return FALSE;
	m_pstArenaUD = m_pcsAgcmUIManager2->AddUserData("Arena_UD",
		this, sizeof(*this), 1, AGCDUI_USERDATA_TYPE_ETC);
	if (!m_pstArenaUD)
		return FALSE;
	m_pstLeaderboardUD = m_pcsAgcmUIManager2->AddUserData("ArenaLeaderboard_UD",
		m_stLeaderboard, sizeof(AgcdArenaLeaderboardEntry), 16, 
		AGCDUI_USERDATA_TYPE_ETC);
	if (!m_pstLeaderboardUD)
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_LB_DisplayRank",
		0, CBDisplayLBRank, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_LB_DisplayName",
		0, CBDisplayLBName, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_LB_DisplayWinLose",
		0, CBDisplayLBWinLose, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_LB_DisplayPoints",
		0, CBDisplayLBPoints, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_LB_DisplayPage",
		0, CBDisplayLBPage, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_LB_DisplayPageMax",
		0, CBDisplayLBPageMax, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_DisplayInQueueCount",
		0, CBDisplayInQueueCount, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_DisplayPoints",
		0, CBDisplayPoints, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_DisplayCoins",
		0, CBDisplayCoins, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_DisplayRegisterNormal",
		0, CBDisplayRegisterNormal, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ARENA_DisplayRegisterRanked",
		0, CBDisplayRegisterRanked, AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"ARENA_LB_HomePage", CBHomePage, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"ARENA_LB_PreviousPage", CBPreviousPage, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"ARENA_LB_NextPage", CBNextPage, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"ARENA_MyRank", CBMyRank, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"ARENA_RegisterNormal", CBRegisterNormal, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, 
		"ARENA_RegisterRanked", CBRegisterRanked, 0))
		return FALSE;
	//public module callback setting
	//m_pcsAgpmBattleGround->SetCallbackSystemMessage( CBSystemMessage, this );
	return TRUE;
}

BOOL
AgcmArena::OnInit()
{
	return TRUE;
}

BOOL
AgcmArena::OnDestroy()
{
	return TRUE;
}

BOOL
AgcmArena::OnIdle(UINT32 ulClockCount)
{
	//m_pcsAgcmUISiegeWar->SetRemainTime( startClock, ulClockCount );
	return TRUE;
}

BOOL
AgcmArena::OnReceive(
	UINT32			ulType, 
	PVOID			pvPacket, 
	INT16			nSize, 
	UINT32			ulNID, 
	DispatchArg *	pstCheckArg)
{
	INT8	cOperation = -1;
	UINT32	ulPoints = -1;
	UINT32	ulCoins = -1;
	PVOID	pLeaderboard = NULL;
	UINT32	ulInQueueCount = -1;

	if(!pvPacket || nSize < 1)
		return FALSE;
	m_csPacket.GetField(TRUE, pvPacket, nSize,
		&cOperation,
		&ulPoints,
		&ulCoins,
		&pLeaderboard,
		&ulInQueueCount);
	switch (cOperation) {
	case ARENA_PT_SET:
		m_ulArenaPoints = ulPoints;
		m_ulArenaCoins = ulCoins;
		RefreshUD();
		break;
	case ARENA_PT_LEADERBOARD: {
		UINT32	ulIndex = 0;
		UINT32	ulEntryCount = 0;
		PVOID	pEntries[16];

		if (!pLeaderboard)
			return FALSE;
		memset(pEntries, 0, sizeof(pEntries));
		m_csLeaderboardPacket.GetField(FALSE, pLeaderboard, 0,
			NULL,
			&ulIndex,
			&ulEntryCount,
			pEntries);
		memset(m_stLeaderboard, 0, sizeof(m_stLeaderboard));
		m_ulLeaderboardIndex = (ulIndex / 16) + 1;
		m_ulLeaderboardCount = ulEntryCount / 16;
		if ((ulEntryCount % 16) != 0)
			m_ulLeaderboardCount++;
		for (UINT32 i = 0; i < 16; i++) {
			char * szName = NULL;
			UINT32 ulRankPoints = 0;
			UINT32 ulWin = 0;
			UINT32 ulLose = 0;

			if (!pEntries[i])
				continue;
			m_csEntryPacket.GetField(FALSE, pEntries[i], 0,
				&szName,
				&ulRankPoints,
				&ulWin,
				&ulLose);
			if (!szName)
				continue;
			m_stLeaderboard[i].ulRank = ulIndex + i + 1;
			strcpy_s(m_stLeaderboard[i].szName, 
				sizeof(m_stLeaderboard[i].szName), szName);
			m_stLeaderboard[i].ulPoints = ulRankPoints;
			m_stLeaderboard[i].ulWin = ulWin;
			m_stLeaderboard[i].ulLose = ulLose;
		}
		RefreshUD();
	} break;
	case ARENA_PT_REGISTER_NORMAL:
		m_bArenaRegisteredNormal = TRUE;
		m_pcsAgcmUIManager2->RefreshUserData(m_pstArenaUD, TRUE);
		break;
	case ARENA_PT_DEREGISTER:
		m_bArenaRegisteredNormal = FALSE;
		m_bArenaRegisteredRanked = FALSE;
		m_pcsAgcmUIManager2->RefreshUserData(m_pstArenaUD, TRUE);
		break;
	case ARENA_PT_IN_QUEUE_COUNT:
		m_ulInQueueCount = ulInQueueCount;
		m_pcsAgcmUIManager2->RefreshUserData(m_pstArenaUD, TRUE);
		break;
	case ARENA_PT_REGISTER_RANKED:
		m_bArenaRegisteredRanked = TRUE;
		m_pcsAgcmUIManager2->RefreshUserData(m_pstArenaUD, TRUE);
		break;
	}
	return TRUE;
}

void
AgcmArena::RefreshUD()
{
	m_pcsAgcmUIManager2->RefreshUserData(m_pstArenaUD, TRUE);
	m_pcsAgcmUIManager2->RefreshUserData(m_pstLeaderboardUD, TRUE);
}

void
AgcmArena::QueryPage(UINT32 ulPageIndex, BOOL bSearchSelf)
{
	INT8	cOperation = ARENA_PT_LEADERBOARD;
	INT16	nSize;
	PVOID	pLeaderboard;
	PVOID	pPacket;

	pLeaderboard = m_csLeaderboardPacket.MakePacket(
		FALSE, NULL, 0,
		&bSearchSelf,
		&ulPageIndex,
		NULL,
		NULL);
	if (!pLeaderboard)
		return;
	pPacket = m_csPacket.MakePacket(TRUE, &nSize, AGPM_ARENA_PACKET_TYPE,
		&cOperation,
		NULL,
		NULL,
		pLeaderboard);
	if (!pPacket || nSize < 1) {
		m_csLeaderboardPacket.FreePacket(pLeaderboard);
		return;
	}
	SendPacket(pPacket, nSize);
	m_csPacket.FreePacket(pPacket);
	m_csLeaderboardPacket.FreePacket(pLeaderboard);
}

static void
setDisplay(
	AgcdUIControl *						pcsControl, 
	char *								szDisplay,
	const AgcdArenaLeaderboardEntry *	pstEntry,
	const char *						fmt, ...)
{
	UINT32 ulColor = 0xFFA5A5A5;
	va_list ap;

	if (pstEntry->ulRank) {
		switch (pstEntry->ulRank) {
		case 1:
			ulColor = 0xFFFFD700;
			break;
		case 2:
			ulColor = 0xFFE7E7E7;
			break;
		case 3:
			ulColor = 0xFFA17419;
			break;
		}
		va_start(ap, fmt);
		vsprintf(szDisplay, fmt, ap);
		va_end(ap);
	}
	else {
		strcpy(szDisplay, "-");
	}
	pcsControl->m_stDisplayMap.m_stFont.m_ulColor = ulColor;
}

BOOL
AgcmArena::CBDisplayLBRank(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	const AgcdArenaLeaderboardEntry * pstEntry =
		(AgcdArenaLeaderboardEntry *)pData;

	setDisplay(pcsSourceControl, szDisplay, pstEntry, "%u", pstEntry->ulRank);
	return TRUE;
}

BOOL
AgcmArena::CBDisplayLBName(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	const AgcdArenaLeaderboardEntry * pstEntry =
		(AgcdArenaLeaderboardEntry *)pData;

	setDisplay(pcsSourceControl, szDisplay, pstEntry, "%s", pstEntry->szName);
	return TRUE;
}

BOOL
AgcmArena::CBDisplayLBWinLose(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	const AgcdArenaLeaderboardEntry * pstEntry =
		(AgcdArenaLeaderboardEntry *)pData;

	setDisplay(pcsSourceControl, szDisplay, pstEntry, "%u/%u", pstEntry->ulWin, pstEntry->ulLose);
	return TRUE;
}

BOOL
AgcmArena::CBDisplayLBPoints(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	const AgcdArenaLeaderboardEntry * pstEntry =
		(AgcdArenaLeaderboardEntry *)pData;

	setDisplay(pcsSourceControl, szDisplay, pstEntry, "%u", pstEntry->ulPoints);
	return TRUE;
}

BOOL
AgcmArena::CBDisplayLBPage(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	AcUIEdit * pcsEdit = (AcUIEdit *)pcsSourceControl->m_pcsBase;

	if (pThis->m_ulLeaderboardCount) {
		pThis->m_szLBPageBuffer[0] = '\0';
		for (UINT32 i = 0; i < pThis->m_ulLeaderboardCount; i++) {
			char szPage[32];

			if (i + 1 == pThis->m_ulLeaderboardIndex)
				sprintf(szPage, "<C%d>%u", 0xFFFFFF, i + 1);
			else
				sprintf(szPage, "<C%d>%u", 0xDDDDDD, i + 1);
			strcat_s(pThis->m_szLBPageBuffer, 
				sizeof(pThis->m_szLBPageBuffer), szPage);
		}
		pcsEdit->SetText(pThis->m_szLBPageBuffer);
	}
	else {
		pcsEdit->SetText("None");
	}
	return TRUE;
}

BOOL
AgcmArena::CBDisplayLBPageMax(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	AcUIEdit * pcsEdit = (AcUIEdit *)pcsSourceControl->m_pcsBase;

	if (pThis->m_ulLeaderboardCount) {
		char szEdit[2048] = "";

		sprintf_s(szEdit, sizeof(szEdit), "%u/%u", 
			pThis->m_ulLeaderboardIndex, pThis->m_ulLeaderboardCount);
		pcsEdit->SetText(szEdit);
	}
	else {
		pcsEdit->SetText("-/-");
	}
	return TRUE;
}

BOOL
AgcmArena::CBDisplayInQueueCount(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	AcUIEdit * pcsEdit = (AcUIEdit *)pcsSourceControl->m_pcsBase;
	
	if (pThis->m_bArenaRegisteredNormal || pThis->m_bArenaRegisteredRanked) {
		char szEdit[2048] = "";

		sprintf_s(szEdit, sizeof(szEdit), "In Queue: %u", 
			pThis->m_ulInQueueCount);
		pcsEdit->SetText(szEdit);
	}
	else {
		pcsEdit->SetText("");
	}
	return TRUE;
}

BOOL
AgcmArena::CBDisplayPoints(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	AcUIEdit * pcsEdit = (AcUIEdit *)pcsSourceControl->m_pcsBase;
	char szEdit[128];

	sprintf_s(szEdit, sizeof(szEdit), "Points: %u", pThis->m_ulArenaPoints);
	pcsEdit->SetText(szEdit);
	return TRUE;
}

BOOL
AgcmArena::CBDisplayCoins(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	AcUIEdit * pcsEdit = (AcUIEdit *)pcsSourceControl->m_pcsBase;
	char szEdit[128];

	sprintf_s(szEdit, sizeof(szEdit), "<C%d>Coins: %u", 0x00A5D5, 
		pThis->m_ulArenaCoins);
	pcsEdit->SetText(szEdit);
	return TRUE;
}

BOOL
AgcmArena::CBDisplayRegisterNormal(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;

	strcpy(szDisplay, pThis->m_bArenaRegisteredNormal ? "Leave Queue" : "Play Normal");
	return TRUE;
}

BOOL
AgcmArena::CBDisplayRegisterRanked(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;

	strcpy(szDisplay, pThis->m_bArenaRegisteredRanked ? "Leave Queue" : "Play Ranked");
	return TRUE;
}

BOOL
AgcmArena::CBHomePage(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	((AgcmArena *)pClass)->QueryPage(0, FALSE);
	return TRUE;
}

BOOL
AgcmArena::CBPreviousPage(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	
	if (pThis->m_ulLeaderboardIndex > 1)
		pThis->QueryPage(pThis->m_ulLeaderboardIndex - 2, FALSE);
	return TRUE;
}

BOOL
AgcmArena::CBNextPage(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena * pThis = (AgcmArena *)pClass;
	
	if (pThis->m_ulLeaderboardIndex < pThis->m_ulLeaderboardCount)
		pThis->QueryPage(pThis->m_ulLeaderboardIndex, FALSE);
	return TRUE;
}

BOOL
AgcmArena::CBMyRank(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	((AgcmArena *)pClass)->QueryPage(0, TRUE);
	return TRUE;
}

BOOL
AgcmArena::CBRegisterNormal(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena *	pThis = (AgcmArena *)pClass;
	INT8		cOperation = pThis->m_bArenaRegisteredNormal ? ARENA_PT_DEREGISTER:
		ARENA_PT_REGISTER_NORMAL;
	INT16		nSize;
	PVOID		pPacket;

	pPacket = pThis->m_csPacket.MakePacket(TRUE, &nSize, AGPM_ARENA_PACKET_TYPE,
		&cOperation,
		NULL,
		NULL,
		NULL);
	if (!pPacket || nSize < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nSize);
	pThis->m_csPacket.FreePacket(pPacket);
	return TRUE;
}

BOOL
AgcmArena::CBRegisterRanked(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmArena *	pThis = (AgcmArena *)pClass;
	INT8		cOperation = pThis->m_bArenaRegisteredRanked ? ARENA_PT_DEREGISTER :
		ARENA_PT_REGISTER_RANKED;
	INT16		nSize;
	PVOID		pPacket;

	pPacket = pThis->m_csPacket.MakePacket(TRUE, &nSize, AGPM_ARENA_PACKET_TYPE,
		&cOperation,
		NULL,
		NULL,
		NULL);
	if (!pPacket || nSize < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nSize);
	pThis->m_csPacket.FreePacket(pPacket);
	return TRUE;
}
