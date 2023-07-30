#include "AgcdUIOption.h"
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcModule/AgcEngine.h>
#include <AgpmCharacter/AgpmCharacter.h>

static AgcmUIManager2 *		g_AgcmUIManager2 = NULL;
static int					g_IsAutoAttackEnabled = 1;
static AgcdUIUserData *		g_UDAutoAttack = NULL;
static int					g_ReceiveExp = 1;
static AgcdUIUserData *		g_UDReceiveExp = NULL;
static AuPacket *			g_Packet = NULL;

static void
send_ui_option_packet(int receive_exp, int auto_attack)
{
	PVOID	packet;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE_GAMEPLAY_OPTION;
	INT16	len = 0;
	char	raw[32];
	int *	list = (int *)raw;

	memset(raw, 0, 32);
	list[0] = receive_exp;
	list[1] = auto_attack;
	if (!g_Packet) {
		g_Packet = new AuPacket;
		g_Packet->SetFlagLength(sizeof(INT32));
		g_Packet->SetFieldType(
			AUTYPE_INT8,			1, // Operation
			AUTYPE_INT32,			1, // Character ID
			AUTYPE_INT32,			1, // Character Template ID
			AUTYPE_MEMORY_BLOCK,	1, // Game ID
			AUTYPE_INT8,			1, // Character Status
			AUTYPE_PACKET,			1, // Move Packet
			AUTYPE_PACKET,			1, // Action Packet
			AUTYPE_PACKET,			1, // Factor Packet
			AUTYPE_INT64,			1, // llMoney
			AUTYPE_INT64,			1, 	// bank money
			AUTYPE_INT64,			1, // cash
			AUTYPE_INT8,			1, // character action status
			AUTYPE_INT8,			1, // character criminal status
			AUTYPE_INT32,			1, // attacker id (정당방위 설정에 필요)
			AUTYPE_INT8,			1, // 새로 생성되서 맵에 들어간넘인지 여부
			AUTYPE_UINT8,			1, // region index
			AUTYPE_UINT8,			1, // social action index
			AUTYPE_UINT64,			1, // special status
			AUTYPE_INT8,			1, // is transform status
			AUTYPE_CHAR,			AGPACHARACTER_MAX_CHARACTER_SKILLINIT,
									   // skill initialization text
			AUTYPE_INT8,			1, // face index
			AUTYPE_INT8,			1, // hair index
			AUTYPE_INT32,			1, // Option Flag
			AUTYPE_INT8,			1, // bank size
			AUTYPE_UINT16,			1, // event status flag
			AUTYPE_INT32,			1, // remained criminal status time
			AUTYPE_INT32,			1, // remained murderer point time
			AUTYPE_CHAR,			AGPACHARACTER_MAX_CHARACTER_NICKNAME, 
									   // nick name
			AUTYPE_MEMORY_BLOCK,	1, // gameguard auth data
			AUTYPE_UINT32,			1, // last killed time in battlesquare
			AUTYPE_END,				0);
	}
	packet = g_Packet->MakePacket(TRUE, &len,
		AGPMCHARACTER_PACKET_TYPE,
		&cOperation, // Operation
		NULL, // Character ID
		NULL, // Character Template ID
		NULL, // Game ID
		NULL, // Character Status
		NULL, // Move Packet
		NULL, // Action Packet
		NULL, // Factor Packet
		NULL, // llMoney
		NULL, // bank money
		NULL, // cash
		NULL, // character action status
		NULL, // character criminal status
		NULL, // attacker id (정당방위 설정에 필요)
		NULL, // 새로 생성되서 맵에 들어간넘인지 여부
		NULL, // region index
		NULL, // social action index
		NULL, // special status
		NULL, // is transform status
		raw, // skill initialization text
		NULL, // face index
		NULL, // hair index
		NULL, // Option Flag
		NULL, // bank size
		NULL, // event status flag
		NULL, // remained criminal status time
		NULL, // remained murderer point time
		NULL, // nick name
		NULL, // gameguard
		NULL); // last killed time in battlesquare
	if (!packet)
		return;
	g_pEngine->SendPacket(packet, len);
	g_Packet->FreePacket(packet);
}

static BOOL
cb_op_receive_exp(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	send_ui_option_packet((g_ReceiveExp == 0) ^ 1, g_IsAutoAttackEnabled != 0);
	return TRUE;
}

static BOOL
cb_op_auto_atk(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	send_ui_option_packet(g_ReceiveExp == 0, (g_IsAutoAttackEnabled != 0) ^ 1);
	return TRUE;
}

static BOOL
cb_update_gameplay_options(PVOID pData, PVOID pClass, PVOID pCustData)
{
	int * options = (int *)pCustData;

	if (!options)
		return FALSE;
	AgcdUIOption::ForceReceiveExperienceOption(options[0] == 0);
	AgcdUIOption::ForceAutoAttackOption(options[1] != 0);
	return TRUE;
}
int
AgcdUIOption::Init()
{
	g_AgcmUIManager2 = (AgcmUIManager2 *)g_pEngine->GetModule("AgcmUIManager2");
	g_UDReceiveExp = g_AgcmUIManager2->AddUserData("OP_ReceiveExp", 
		&g_ReceiveExp, 4, 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!g_UDReceiveExp)
		return 0;
	g_UDAutoAttack = g_AgcmUIManager2->AddUserData("OP_AutoAttk", 
		&g_IsAutoAttackEnabled, 4, 1, AGCDUI_USERDATA_TYPE_BOOL);
	if (!g_UDAutoAttack)
		return 0;
	if (!g_AgcmUIManager2->AddFunction(NULL, 
		"OP_ReceiveExp", cb_op_receive_exp, 0))
		return 0;
	if (!g_AgcmUIManager2->AddFunction(NULL, 
		"OP_AutoAttk", cb_op_auto_atk, 0))
		return 0;
	((AgpmCharacter *)g_pEngine->GetModule("AgpmCharacter"))->SetCallback(
		AGPMCHAR_CB_ID_UPDATE_GAMEPLAY_OPTION, 
		cb_update_gameplay_options, NULL);
	return 1;
}

void
AgcdUIOption::SetAutoAttackOption(int enabled)
{
	send_ui_option_packet(g_ReceiveExp == 0, enabled != 0);
}

void
AgcdUIOption::ForceAutoAttackOption(int enabled)
{
	g_IsAutoAttackEnabled = (enabled != 0);
	g_AgcmUIManager2->RefreshUserData(g_UDAutoAttack, TRUE);
}

void
AgcdUIOption::SetReceiveExperienceOption(int enabled)
{
	send_ui_option_packet(enabled == 0, (g_IsAutoAttackEnabled != 0));
}

void
AgcdUIOption::ForceReceiveExperienceOption(int enabled)
{
	g_ReceiveExp = (enabled != 0);
	g_AgcmUIManager2->RefreshUserData(g_UDReceiveExp, TRUE);
}

int
AgcdUIOption::IsAutoAttackEnabled()
{
	return g_IsAutoAttackEnabled;
}
