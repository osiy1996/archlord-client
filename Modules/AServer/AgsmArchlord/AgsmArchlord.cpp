#include "AgsmArchlord.h"

#include "AgsmGuild.h"
#include "AgpmItem.h"
#include "AgsmItemManager.h"
#include "AgpmFactors.h"
#include "AgsmAdmin.h"
#include "AgsmCharacter.h"
#include "AgpmEventSpawn.h"
#include "ApmEventManager.h"
#include "AgsmEventSpawn.h"
#include "AgpmEventTeleport.h"
#include "AgpmEventBinding.h"

const INT32 ARCHLORD_TID = 1276;

AgsmArchlord::AgsmArchlord()
{
	m_pcsAgpmArchlord = NULL;
	m_pcsAgsmGuild = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgpmGrid = NULL;
	m_pcsAgsmItemManager = NULL;
	m_pcsAgpmFactors = NULL;
	m_pcsAgsmAdmin = NULL;
	m_pcsAgpmCharacter = NULL;
	m_pcsAgsmCharacter = NULL;
	m_pcsAgpmEventSpawn = NULL;
	m_pcsApmEventManager = NULL;
	m_pcsAgsmEventSpawn = NULL;
	m_pcsAgpmEventTeleport = NULL;
	m_pcsAgpmSiegeWar = NULL;
	m_pcsAgsmRide = NULL;
	m_pcsAgsmDeath = NULL;
	m_pcsAgpmEventBinding = NULL;

	m_lLastTick = 0;
	m_lStepTime = 0;

	EnableIdle(TRUE);
	SetModuleName("AgsmArchlord");

	InitGuardItem();
}

AgsmArchlord::~AgsmArchlord()
{
}

void AgsmArchlord::InitGuardItem()
{
	::ZeroMemory(&m_GuardItem, sizeof(m_GuardItem));

	/*
		�޸ձü�
		[3941]H_Arc_M_Set_9thBody
		[3945]H_Arc_M_Set_9thFoot
		[3943]H_Arc_M_Set_9thHands
		[3946]H_Arc_M_Set_head_9thHead
		[3944]H_Arc_M_Set_9thLegs 
	*/
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 3941;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 3945;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 3943;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 3946;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 3944;

	/*
		�޸�����
		[3975]H_Kni_M_Set_9thBody
		[3979]H_Kni_M_Set_9thFoot
		[3977]H_Kni_M_Set_9thHands
		[3980]H_Kni_M_Set_head_9thHead
		[3978]H_Kni_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 3975;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 3979;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 3977;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 3980;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 3978;

	/*
		�޸չ���
		[4093]H_wiz_W_Set_9thBody
		[4097]H_wiz_W_Set_9thFoot
		[4095]H_wiz_W_Set_9thHands
		[4098]H_wiz_W_Set_head_9thHead
		[4096]H_wiz_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 4093;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 4097;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 4095;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 4098;
	m_GuardItem[AURACE_TYPE_HUMAN][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 4096;

	/*
		����������
		[5751]H_Kni_M_Set_9thBody
		[5754]H_Kni_M_Set_9thFoot
		[5752]H_Kni_M_Set_9thHands
		[5758]H_Kni_M_Set_head_9thHead
		[5753]H_Kni_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 5751;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 5754;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 5752;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 5758;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 5753;

	/*
		�������ü�
		[4423]M_Hun_W_Set_9thBody
		[4427]M_Hun_W_Set_9thFoot 
		[4425]M_Hun_W_Set_9thHands
		[4432]M_Hun_W_Set_head_9thHead
		[4426]M_Hun_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 4423;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 4427;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 4425;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 4432;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 4426;

	/*
		����������
		[4453]M_Sha_W_Set_9thBody
		[4457]M_Sha_W_Set_9thFoot
		[4455]M_Sha_W_Set_9thHands
		[4552]M_Sha_W_Set_head_9thHead
		[4456]M_Sha_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 4453;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 4457;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 4455;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 4552;
	m_GuardItem[AURACE_TYPE_MOONELF][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 4456;

	/*
		��ũ�ü�
		[4482]O_Hun_W_Set_9thBody
		[4486]O_Hun_W_Set_9thFoot 
		[4484]O_Hun_W_Set_9thHands
		[4491]O_Hun_W_Set_head_9thHead
		[4485]O_Hun_W_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 4482;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 4486;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 4484;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 4491;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 4485;

	/*
		��ũ����
		[4512]O_Sha_M_Set_9thBody
		[4516]O_Sha_M_Set_9thFoot 
		[4514]O_Sha_M_Set_9thHands
		[4521]O_Sha_M_Set_Head_9thHead
		[4515]O_Sha_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 4512;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 4516;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 4514;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 4521;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 4515;

	/*
		��ũ����
		[4542]O_War_M_Set_9thBody
		[4546]O_War_M_Set_9thFoot
		[4544]O_War_M_Set_9thHands
		[4551]O_War_M_Set_Head_9thHead
		[4545]O_War_M_Set_9thLegs
	*/
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 4542;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 4546;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 4544;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 4551;
	m_GuardItem[AURACE_TYPE_ORC][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 4545;


	// DragonScion
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_KNIGHT].ItemTID[EGIP_LEGS] = 1;

	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_RANGER].ItemTID[EGIP_LEGS] = 1;

	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_SCION].ItemTID[EGIP_LEGS] = 1;

	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_BODY] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_FOOT] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HAND] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_HEAD] = 1;
	m_GuardItem[AURACE_TYPE_DRAGONSCION][AUCHARCLASS_TYPE_MAGE].ItemTID[EGIP_LEGS] = 1;
}

BOOL AgsmArchlord::OnAddModule()
{
	m_pcsAgpmArchlord = (AgpmArchlord*)GetModule("AgpmArchlord");
	m_pcsAgsmGuild = (AgsmGuild*)GetModule("AgsmGuild");
	m_pcsAgpmItem = (AgpmItem*)GetModule("AgpmItem");
	m_pcsAgpmGrid = (AgpmGrid*)GetModule("AgpmGrid");
	m_pcsAgsmItemManager = (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pcsAgpmFactors = (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgsmAdmin = (AgsmAdmin*)GetModule("AgsmAdmin");
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pcsAgpmEventSpawn = (AgpmEventSpawn*)GetModule("AgpmEventSpawn");
	m_pcsApmEventManager = (ApmEventManager*)GetModule("ApmEventManager");
	m_pcsAgsmEventSpawn = (AgsmEventSpawn*)GetModule("AgsmEventSpawn");
	m_pcsAgpmEventTeleport = (AgpmEventTeleport*)GetModule("AgpmEventTeleport");
	m_pcsAgpmSiegeWar = (AgpmSiegeWar*)GetModule("AgpmSiegeWar");
	m_pcsAgsmRide = (AgsmRide*)GetModule("AgsmRide");
	m_pcsAgsmDeath = (AgsmDeath*)GetModule("AgsmDeath");
	m_pcsAgpmEventBinding = (AgpmEventBinding*)GetModule("AgpmEventBinding");
	
	if (!m_pcsAgpmArchlord || !m_pcsAgsmGuild || !m_pcsAgpmItem || !m_pcsAgpmGrid || 
		!m_pcsAgsmItemManager || !m_pcsAgpmFactors || !m_pcsAgsmAdmin || !m_pcsAgpmCharacter ||
		!m_pcsAgsmCharacter || !m_pcsAgsmCharManager || !m_pcsAgpmEventSpawn || !m_pcsApmEventManager ||
		!m_pcsAgsmEventSpawn || !m_pcsAgpmEventTeleport || !m_pcsAgpmSiegeWar || !m_pcsAgsmRide ||
		!m_pcsAgsmDeath || !m_pcsAgpmEventBinding)
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordSetArchlord(CBSetArchlord, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordSetGuard(CBSetGuard, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordCancelGuard(CBCancelGuard, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordGuardInfo(CBGuardInfo, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordCancelArchlord(CBCancelArchlord, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordStart(CBArchlordStart, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordNextStep(CBArchlordNextStep, this))
		return FALSE;

	if (!m_pcsAgsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;

	if (!m_pcsAgsmAdmin->SetCallbackArchlordEnd(CBArchlordEnd, this))
		return FALSE;

	if (!m_pcsAgpmEventTeleport->SetCallbackArchlordMessageId(CBTeleportMessageId, this))
		return FALSE;

	if (!m_pcsAgpmArchlord->SetCallbackEventRequest(CBEventRequest, this))
		return FALSE;

	if (!m_pcsAgsmDeath->SetCallbackCharacterDead(CBCharacterDead, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmArchlord::OnInit()
{
	return TRUE;
}

BOOL AgsmArchlord::OnDestroy()
{
	return TRUE;
}

BOOL AgsmArchlord::OnIdle(UINT32 ulClockCount)
{
	// 30�� ���� Ȯ��
	if (ulClockCount < (m_lLastTick + (30* 1000)))
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("OnIdle"));

	m_lLastTick = ulClockCount;
	

	switch (m_pcsAgpmArchlord->GetCurrentStep())
	{
	case AGPMARCHLORD_STEP_NONE:		IdleStepNone(ulClockCount);		break;
	case AGPMARCHLORD_STEP_DUNGEON:		IdleStepDungeon(ulClockCount);	break;
	case AGPMARCHLORD_STEP_SIEGEWAR:	IdleStepSiegewar(ulClockCount);	break;
	case AGPMARCHLORD_STEP_ARCHLORD:	IdleStepArchlord(ulClockCount);	break;
	default:	ASSERT(!"AgsmArchlord::OnIdle() error");
	};

	return TRUE;
}

void AgsmArchlord::StartArchlord(UINT32 ulClockCorrection)
{
	// NONE ���°� �ƴϸ� ��ũ�ε� ������ ���� �Ұ� // ��ڸ�ɾ� �߸��� ������� ���� �������� [ 2011-07-27 silvermoo ]
	if ( AGPMARCHLORD_STEP_NONE != m_pcsAgpmArchlord->GetCurrentStep() )
		return ;

	// ��ũ�ε� �ý��� ����
	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_DUNGEON);
	SetStepTime(GetClockCount() - ulClockCorrection);

	SendPacketCurrentStep();

	// ����� ������ ���� ���� ������ �Ǹ鼭 ��ũ�ε� ���������� ����. 2009. 09. 29 �ڰ浵
	//SpawnDungeonMonster();
	//m_pcsAgpmEventTeleport->ActiveTeleportCastleToDungeon();
	//��ũ�ε� �������� ���۵Ǹ� ����� �������� ��� ������ ���Ѵ´�.
	ExpelAllUsersFromSecretDungeon();
}

void AgsmArchlord::NextStep()
{	
	// ��ũ�ε��峪 ��ī�ΰ��� �������϶��� ���� �ܰ� ���� ��ɾ ����ϸ� �ȵȴ� [ 2011-07-27 silvermoo ]
	if ( AGPMARCHLORD_STEP_ARCHLORD == m_pcsAgpmArchlord->GetCurrentStep() )
		return;

	// (�ӽ�) ���������¿����� ���� �ܰ� ���� ��ɾ ����ϸ� �ȵȴ� [ 2011-07-27 silvermoo ]
	if( AGPMARCHLORD_STEP_SIEGEWAR == m_pcsAgpmArchlord->GetCurrentStep() )
		return;

	SetStepTime(m_lStepTime - (60 * 60 * 1000));
}

void AgsmArchlord::ArchlordEnd()
{
	SetStepTime(0);

	m_pcsAgpmSiegeWar->SetCurrentStatus(AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX, AGPD_SIEGE_WAR_STATUS_TIME_OVER_ARCHLORD_BATTLE);

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_END);
	SendPacketCurrentStep();
	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_NONE);
}

void AgsmArchlord::IdleStepNone(UINT32 ulClockCount)
{
	// ��¥ Ȯ��
	if (FALSE == IsArchlordStartDay())
		return;

	// �ð� Ȯ��
	SYSTEMTIME localTime;
	::GetLocalTime(&localTime);

	if ( AP_SERVICE_AREA_GLOBAL == g_eServiceArea )
	{
		const INT32 lSiegeWarTime = m_pcsAgpmSiegeWar->m_csSiegeWarInfo[0].m_csTemplate.m_ulSiegeWarBeginTime;

		if ( lSiegeWarTime == localTime.wHour && 0 <= localTime.wMinute )
		{
			UINT32 ulCorrection = 0;

			if ( localTime.wMinute >= 5 )
				ulCorrection = 180000; // 3 mins

			StartArchlord(ulCorrection);
		}
	}
	else if( AP_SERVICE_AREA_CHINA == g_eServiceArea )
	{
		// ���� 9�ÿ� ����
		if (21 == localTime.wHour)
		{
			UINT32 ulCorrection = 0;
			// 5�� �̻� ������� ���� �ð� ����.
			if (localTime.wMinute >= 35)
				ulCorrection = (UINT32) (60 * 1000 * (localTime.wMinute - (30 + 3))); 		// �� 3���� �߰��ð�
		
			StartArchlord(ulCorrection);
		}
	}
	else
	{
		// ���� 8�� 30�п� ����
		if (20 == localTime.wHour && 30 <= localTime.wMinute)
		{
			UINT32 ulCorrection = 0;
			// 5�� �̻� ������� ���� �ð� ����.
			if (localTime.wMinute >= 35)
				ulCorrection = (UINT32) (60 * 1000 * (localTime.wMinute - (30 + 3))); 		// �� 3���� �߰��ð�
		
			StartArchlord(ulCorrection);
		}
	}
}

void AgsmArchlord::IdleStepDungeon(UINT32 ulClockCount)
{
	// 30���� ������ ���� �ܰ� --> 10������ ���� 2009. 09. 29 �ڰ浵
	if (ulClockCount < m_lStepTime + AGSMARCHLORD_STEP_DUNGEON_TIME)
		return;

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_SIEGEWAR);
	SetStepTime(ulClockCount);
	SendPacketCurrentStep();

	// ����� ������ ���� ���� ������ �Ǹ鼭 ��ũ�ε� ���������� ����. 2009. 09. 29 �ڰ浵
	//// ���� ���� ����
	//RemoveDungeonMonster();
	//// ���� �ڷ���Ʈ�� ������Ų��.
	//m_pcsAgpmEventTeleport->DungeonTeleportDisable();

	//// ������ ���� �ִ� ��� �������� �Ѿ� ����.
	//ExpelAllUsersFromSecretDungeon();

	if (m_pcsAgpmSiegeWar->CheckStartCondition())
		EnumCallback(AGSMARCHLORD_CB_START_ARCHLORD_SIEGEWAR, NULL, NULL);
	else
	{	
		EnumCallback(AGSMARCHLORD_CB_END_ARCHLORD_SIEGEWAR, NULL, NULL);
		//��ũ�ε� ������ ������ �� ����.
		ArchlordEnd();
	}
}

void AgsmArchlord::IdleStepSiegewar(UINT32 ulClockCount)
{
	// 1�ð��� ������ ���� �ܰ�
	if (ulClockCount < m_lStepTime + AGSMARCHLORD_STEP_SIEGEWAR_TIME)
		return;

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_ARCHLORD);
	SetStepTime(ulClockCount);
	SendPacketCurrentStep();
}

void AgsmArchlord::IdleStepArchlord(UINT32 ulClockCount)
{
	// 1�ð��� ������ ���� �ܰ�
	if (ulClockCount < m_lStepTime + AGSMARCHLORD_STEP_ARCHLORD_TIME)
		return;

	m_pcsAgpmArchlord->SetCurrentStep(AGPMARCHLORD_STEP_NONE);
	SetStepTime(ulClockCount);
	SendPacketCurrentStep();
}

BOOL AgsmArchlord::IsArchlordStartDay()
{
	// ���� �ð� ���ϱ�
	SYSTEMTIME localTime;
	::GetLocalTime(&localTime);

	// �����(6)���� ��ũ�ε� �ý����� ����ȴ�.
	if (6 != localTime.wDayOfWeek)
		return FALSE;

	// ��ũ�ε�������� �������ְ� �ƴ� 4��° �ֿ� ������ �ȴ�.
	// 4��° �� ������� ����̰� 22�Ϻ��� 28�� ���̿� ��ġ�ϰ� �ȴ�.
	if (localTime.wDay >= 22 && localTime.wDay <= 28)
		return TRUE;

	return FALSE;
}

BOOL AgsmArchlord::CBSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pData;

	return pThis->SetArachlord(pcsTarget);
}

BOOL AgsmArchlord::CBCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pData;

	return pThis->CancelArchlord(pcsTarget->m_szID);
}

BOOL AgsmArchlord::CBSetGuard(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsArchlord = (AgpdCharacter*)pData;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pCustData;

	return pThis->SetGuard(pcsArchlord, pcsTarget);
}

BOOL AgsmArchlord::CBCancelGuard(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsArchlord = (AgpdCharacter*)pData;
	AgpdCharacter *pcsTarget = (AgpdCharacter*)pCustData;

	return pThis->CancelGuard(pcsArchlord, pcsTarget);
}

BOOL AgsmArchlord::CBGuardInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsArchlord = (AgpdCharacter*)pData;

	return pThis->GuardInfo(pcsArchlord);
}

BOOL AgsmArchlord::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmArchlord* pThis = (AgsmArchlord*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBEnterGameWorld"));

	if (FALSE == pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	if (pThis->m_pcsAgpmCharacter->IsNPC(pcsCharacter))
		return TRUE;

	if (pThis->m_pcsAgpmCharacter->IsGuard(pcsCharacter))
		return TRUE;

	// ��� �����ڿ��� ��ũ�ε��� ID�� �������ش�.
	pThis->SendPacketArchlordID(pcsCharacter);

	// ��ũ�ε����� Ȯ���� �۾� ����
	if (FALSE == pThis->TransformArchlord(pcsCharacter))
		// ��ũ�ε尡 �ƴ϶�� ȣ���� ���� Ȯ�� �� �۾� ����
		pThis->CheckGuard(pcsCharacter);

	return TRUE;
}

BOOL AgsmArchlord::CBArchlordStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBArchlordStart"));

	pThis->StartArchlord();
	return TRUE;
}

BOOL AgsmArchlord::CBArchlordNextStep(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBArchlordNextStep"));

	pThis->NextStep();

	return TRUE;
}

BOOL AgsmArchlord::CBArchlordEnd(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBArchlordEnd"));

	pThis->ArchlordEnd();

	return TRUE;
}

BOOL AgsmArchlord::CBTeleportMessageId(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;

	pThis->SendPacketMessageId(pcsCharacter, AGPMARCHLORD_MESSAGE_NEED_ARCHON_ITEM);

	return TRUE;
}

BOOL AgsmArchlord::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmArchlord *pThis = (AgsmArchlord *)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter *) pData;
	ApdEvent *pApdEvent = (ApdEvent *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pcsAgpmArchlord->MakePacketEventGrant(&nPacketLength, pApdEvent, pcsCharacter->m_lID);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	pThis->m_pcsAgpmArchlord->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmArchlord::TransformArchlord(AgpdCharacter *pcsCharacter)
{
	// ��ũ�ε��̸� ���� �����ش�.
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("TransformArchlord"));

	AuAutoLock Guard(pcsCharacter->m_Mutex);
	if (!Guard.Result()) return FALSE;

	// ��ũ�ε�� ����
	AgpdCharacterTemplate *pcsTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(ARCHLORD_TID);
	ASSERT(pcsTemplate);
	if (!pcsTemplate) 
		return FALSE;

	// 2006.11.23. steeple
	// Ż���� Ÿ ������ �����ش�.
	if(pcsCharacter->m_bRidable)
	{
		m_pcsAgsmRide->DismountReq(pcsCharacter, 1);
		//AgpdGridItem* pcsAgpdGridItem = m_pcsAgpmItem->GetEquipItem(pcsCharacter, AGPMITEM_PART_RIDE);
		//if(pcsAgpdGridItem)
		//{
		//	AgpdItem* pcsItem = m_pcsAgpmItem->GetItem(pcsAgpdGridItem);
		//	if(pcsItem)
		//	{
		//		m_pcsAgpmItem->UnEquipItem(pcsCharacter, pcsItem);
		//		//m_pcsAgsmRide->DismountReq(pcsCharacter, pcsItem->m_lID);
		//	}
		//}
	}

	INT64	lMaxExp		= m_pcsAgpmFactors->GetMaxExp(&pcsCharacter->m_csFactor);
	INT64	lCurrentExp	= m_pcsAgpmCharacter->GetExp(pcsCharacter);

	// 2006.12.14. steeple
	// �Ѹ�� �� �ٲ�Բ� �����Ѵ�.
	// ������ �߰������� ������� �ϴ� ���Ͱ� �ִ� �� üũ�ؾ� �Ѵ�.
	//m_pcsAgpmCharacter->TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL, pcsTemplate, &pcsTemplate->m_csEffectFactor);
	m_pcsAgpmCharacter->TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY, pcsTemplate);

	//// 2006.11.07. steeple
	//// ���͸� �����Ѵ�.
	//m_pcsAgpmCharacter->TransformCharacter(pcsCharacter, AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL, pcsTemplate, &pcsTemplate->m_csFactor, TRUE);

	m_pcsAgpmFactors->SetMaxExp(&pcsCharacter->m_csFactor, lMaxExp);
	//m_pcsAgpmFactors->SetMaxExp((AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), lMaxExp);

	m_pcsAgpmFactors->SetExp(&pcsCharacter->m_csFactor, lCurrentExp);
	//m_pcsAgpmFactors->SetExp((AgpdFactor *) m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), lCurrentExp);

	// �������� Ǯ�� ä���ش�.
	m_pcsAgsmCharacter->SetHPFull(pcsCharacter);

	m_pcsAgsmCharacter->ReCalcCharacterResultFactors(pcsCharacter);

	return TRUE;
}

BOOL AgsmArchlord::CheckGuard(AgpdCharacter *pcsCharacter)
{
	// ������ �� ȣ���밡 �ƴѵ� ȣ���� �������� ������ ������ ������ �����Ѵ�.

	ASSERT(pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	AuAutoLock Guard2(pcsCharacter->m_Mutex);
	if (!Guard2.Result()) return FALSE;

	vtIterator iter = ::find(m_vtGuard.begin(), m_vtGuard.end(), string(pcsCharacter->m_szID));
	if (m_vtGuard.end() == iter)
	{
		DeleteGuardItem(pcsCharacter);
	}

	return TRUE;
}

BOOL AgsmArchlord::SetGuard(AgpdCharacter *pcsArchlord, AgpdCharacter *pcsTarget)
{
	//STOPWATCH2(GetModuleName(), _T("SetGuard"));

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// ����� ���� ����� ��ũ�ε� ���� Ȯ��
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsArchlord->m_szID))
		return FALSE;

	// ���� ������� Ȯ��
	if (FALSE == m_pcsAgsmGuild->IsSameGuild(pcsArchlord, pcsTarget))
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// ������ ȣ������ �ִ� ���ڸ� �ʰ� �ϴ��� Ȯ��
	if (GUARD_MAX < m_vtGuard.size())
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// �̹� ȣ����� ���� �Ǿ� �ִ� ������� Ȯ��
	vtIterator iter = ::find(m_vtGuard.begin(), m_vtGuard.end(), string(pcsTarget->m_szID));
	if (m_vtGuard.end() != iter)
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// ȣ���� ������ ����
	if (FALSE == CreateGuardItem(pcsTarget))
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_SET_GUARD);
		return FALSE;
	}

	// ȣ���� ����
	m_vtGuard.push_back(string(pcsTarget->m_szID));
	
	// DB
	EnumCallback(AGSMARCHLORD_CB_DBINSERT_GUARD, pcsTarget, NULL);

	return TRUE;
}

BOOL AgsmArchlord::CancelGuard(AgpdCharacter *pcsArchlord, AgpdCharacter *pcsTarget)
{
	//STOPWATCH2(GetModuleName(), _T("CancelGuard"));

	ASSERT(pcsArchlord);
	ASSERT(pcsTarget);

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// ����� ���� ����� ��ũ�ε� ���� Ȯ��
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsArchlord->m_szID))
		return FALSE;

	// ���� ������� Ȯ��
	if (FALSE == m_pcsAgsmGuild->IsSameGuild(pcsArchlord, pcsTarget))
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_CANCEL_GUARD);
		return FALSE;
	}

	// ȣ���밡 �´��� Ȯ��
	vtIterator iter = ::find(m_vtGuard.begin(), m_vtGuard.end(), string(pcsTarget->m_szID));

	if (iter == m_vtGuard.end())
	{
		SendPacketMessageId(pcsArchlord, AGPMARCHLORD_MESSAGE_DONT_CANCEL_GUARD);
		return FALSE;
	}

	// ȣ���� ������ ����
	DeleteGuardItem(pcsTarget);

	// ȣ���� ����
	m_vtGuard.erase(iter);

	// DB
	EnumCallback(AGSMARCHLORD_CB_DBDELETE_GUARD, pcsTarget, NULL);

	return TRUE;
}

// ��ũ�ε� ����
BOOL AgsmArchlord::SetArachlord(AgpdCharacter *pcsCharacter)
{
	//STOPWATCH2(GetModuleName(), _T("SetArchlord"));

	ASSERT(pcsCharacter);
	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// ��ũ�ε尡 �״���̸� ȣ������ �������� �״�� �����Ѵ�.
	if (COMPARE_EQUAL != m_pcsAgpmArchlord->m_szArchlord.Compare(pcsCharacter->m_szID))
	{
		// �ٸ��� ������ ��ũ�ε尡 ������ �ִ� ���õ��� �����Ѵ�.
		AgpdCharacter *pcsGuard = NULL;
		if (m_vtGuard.size())
		{
			for (vtIterator iter = m_vtGuard.begin(); iter == m_vtGuard.end(); ++iter)
			{
				pcsGuard = m_pcsAgpmCharacter->GetCharacter(const_cast<char*>(iter->c_str()));
				if (NULL == pcsGuard)
					continue;

				DeleteGuardItem(pcsGuard);
			}

			m_vtGuard.clear();
		}
	}

	// 
	if (FALSE == m_pcsAgpmArchlord->m_szArchlord.IsEmpty())
	{
		CancelArchlord(m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	}

	// ���ο� ��ũ�ε带 �����Ѵ�.
	m_pcsAgpmArchlord->SetArchlord(pcsCharacter->m_szID);
	
	// DB
	UINT32 ulSiegeID = 0;
	EnumCallback(AGSMARCHLORD_CB_DBINSERT_ARCHLORD, pcsCharacter, &ulSiegeID);

	// ��ũ�ε尡 ������ ����� ��� �÷��̾�� ��Ŷ ����
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketSetArchlord(&nPacketLength, pcsCharacter->m_szID);
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacketAllUser(pvPacket, nPacketLength);

	TransformArchlord(pcsCharacter);

	return TRUE;
}

BOOL AgsmArchlord::CancelArchlord(CHAR* szArchlord)
{
	//STOPWATCH2(GetModuleName(), _T("CancelArchlord"));

	AuAutoLock Guard(m_lock);
	if (!Guard.Result()) return FALSE;

	// �ٸ��� ������ ��ũ�ε尡 ������ �ִ� ���õ��� �����Ѵ�.
	AgpdCharacter *pcsGuard = NULL;
	if (m_vtGuard.size())
	{
		for (vtIterator iter = m_vtGuard.begin(); iter == m_vtGuard.end(); ++iter)
		{
			pcsGuard = m_pcsAgpmCharacter->GetCharacter(const_cast<char*>(iter->c_str()));
			if (NULL == pcsGuard)
				continue;

			DeleteGuardItem(pcsGuard);
		}

		m_vtGuard.clear();
	}

	// ��ũ�ε尡 ������ ����� ��� �÷��̾�� ��Ŷ ����
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketCancelArchlord(&nPacketLength, m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacketAllUser(pvPacket, nPacketLength);

	// DB
	// �����ϱ��� �˷��ַ��� �ߴµ� �ϴ� ����...
	//UINT32 ulSiegeID = 0;
	//EnumCallback(AGSMARCHLORD_CB_DBUPDATE_ARCHLORD, pcsCharacter, &ulSiegeID);

	// �¶��� ���̸� ��ũ�ε�� ���� ���
	AgpdCharacter *pcsArchlord = m_pcsAgpmCharacter->GetCharacter(m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	if (NULL != pcsArchlord)
	{
		// Ż �� Ÿ�� ������ �����ְ�
		if(pcsArchlord->m_bRidable)
		{
			INT32 lItemID = 1295;		// Arcadia
			m_pcsAgsmRide->DismountReq(pcsArchlord, lItemID);
		}

		m_pcsAgpmCharacter->RestoreTransformCharacter(pcsArchlord);

		// 2006.11.23. steeple
		// Ȥ�� Ż�� Ÿ�� �־��ٸ� �ٽ� üũ���ֱ�.
		m_pcsAgsmRide->RegionChange(pcsArchlord, pcsArchlord->m_nBindingRegionIndex);
	}

	m_pcsAgpmArchlord->CancelArchlord(szArchlord);

	return TRUE;	
}

UINT32 AgsmArchlord::SetStepTime(UINT32 ulTime)
{
	m_lStepTime = ulTime;

	return m_lStepTime;
}

BOOL AgsmArchlord::GuardInfo(AgpdCharacter *pcsCharacter)
{
	if (FALSE == m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketGuardInfo(&nPacketLength, (INT32) m_vtGuard.size(), GUARD_MAX);
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));

	return TRUE;
}

BOOL AgsmArchlord::CreateGuardItem(AgpdCharacter *pcsCharacter)
{
	//STOPWATCH2(GetModuleName(), _T("CreateGuardItem"));

	// �κ��丮�� ������ �߰�
	AuAutoLock Guard2(pcsCharacter->m_Mutex);
	if (!Guard2.Result()) return FALSE;

	if (FALSE == IsValidInventory(pcsCharacter))
		return FALSE;

	INT32 Race = m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	INT32 Class = m_pcsAgpmFactors->GetClass(&pcsCharacter->m_csFactor);

	AgpdItem* pNewItem = NULL;

	for (INT32 i = 0; i < EGIP_MAX; ++i)
	{
		// ������ ����
		ASSERT(m_GuardItem[Race][Class].ItemTID[i]);
		pNewItem = m_pcsAgsmItemManager->CreateItem(m_GuardItem[Race][Class].ItemTID[i], pcsCharacter);
		if (!pNewItem) 
			break;

		if (FALSE == m_pcsAgpmItem->AddItemToInventory(pcsCharacter, pNewItem))
		{
			m_pcsAgpmItem->RemoveItem(pNewItem, TRUE);
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL AgsmArchlord::DeleteGuardItem(AgpdCharacter *pcsCharacter)
{
	//STOPWATCH2(GetModuleName(), _T("DeleteGuardItem"));

	INT32 Race = m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	INT32 Class = m_pcsAgpmFactors->GetClass(&pcsCharacter->m_csFactor);

	AgpdGrid *pcsInventory = m_pcsAgpmItem->GetInventory(pcsCharacter);
	if (NULL == pcsInventory)
		return FALSE;

	AgpdGrid *pcsEquip = m_pcsAgpmItem->GetEquipGrid(pcsCharacter);
	if (NULL == pcsEquip)
		return FALSE;

	AgpdGridItem *pcsGridItem = NULL;

	for (INT32 i = 0; i < EGIP_MAX; ++i)
	{
		// �κ��丮���� ã��
		ASSERT(m_GuardItem[Race][Class].ItemTID[i]);
		pcsGridItem = m_pcsAgpmGrid->GetItemByTemplate(pcsInventory, AGPDGRID_ITEM_TYPE_ITEM, 
						m_GuardItem[Race][Class].ItemTID[i]);

		// �κ��丮�� ������ ���������� Ȯ��
		if (NULL == pcsGridItem)
		{
			pcsGridItem = m_pcsAgpmGrid->GetItemByTemplate(pcsEquip, AGPDGRID_ITEM_TYPE_ITEM, 
							m_GuardItem[Race][Class].ItemTID[i]);
		}

		if (NULL == pcsGridItem)
			continue;

		AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if (pcsItem)
		{
			// Equip �̸� UnEquip ���� ���ش�. 2007.05.15. steeple
			if (pcsItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
			{
				m_pcsAgpmItem->UnEquipItem(pcsCharacter, pcsItem);
			}

			m_pcsAgpmItem->RemoveItem(pcsItem, TRUE);
		}
	}

	return TRUE;
}

BOOL AgsmArchlord::IsValidInventory(AgpdCharacter *pcsCharacter)
{
	// ȣ������ ���� �������� �־� �� �� �ִ� �κ��丮 ������ �ʿ����� Ȯ��
	AgpdGrid *pcsInventory = m_pcsAgpmItem->GetInventory(pcsCharacter);
	if (NULL == pcsInventory)
		return FALSE;

	INT32 ItemCount = m_pcsAgpmGrid->GetItemCount(pcsInventory);

	if (EGIP_MAX > pcsInventory->m_lGridCount - ItemCount )
	{
		SendPacketMessageId(pcsCharacter, AGPMARCHLORD_MESSAGE_NOT_ENOUGH_INVENTORY);
		return FALSE;
	}

	return TRUE;
}

// ����� �������� ���� ����
void AgsmArchlord::SpawnDungeonMonster()
{
	//STOPWATCH2(GetModuleName(), _T("SpawnDungeonMonster"));

	// 2007.03.23. steeple
	// EventSpawn Thread �� ������.
	m_pcsAgsmEventSpawn->InsertAdditionalSpawn(AGSMSPAWN_ADDITIONAL_TYPE_SECRET_DUNGEON,
												this,
												CBSpawnDungeonMonster,
												0,
												(INT32)AGPDSPAWN_ARCHLORD_TYPE_SECRET_DUNGEON);
}

BOOL AgsmArchlord::CBSpawnDungeonMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmArchlord* pThis = static_cast<AgsmArchlord*>(pClass);
	AdditionalSpawnData* pstSpawnData = static_cast<AdditionalSpawnData*>(pData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSpawnDungeonMonster"));

	AgpdSpawnGroup *	pstGroup;
	INT32				lIndex = 0;

	UINT32				ulClockCount	= pThis->GetClockCount();

	for (pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for (ApdEvent	*pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			if (!pstEvent->m_pvData)
				break;

			if (pstSpawnData->m_lData2 == ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType)
			{
				AuMATRIX	*pstDirection	= NULL;;
				AuPOS		stSpawnPos	= *pThis->m_pcsApmEventManager->GetBasePos(pstEvent->m_pcsSource, &pstDirection);

				pThis->m_pcsAgsmEventSpawn->ProcessSpawn(pstEvent, ulClockCount);
			}
		}
	}

	return TRUE;
}

void AgsmArchlord::RemoveDungeonMonster()
{
	//STOPWATCH2(GetModuleName(), _T("RemoveDungeonMonster"));

	// 2007.03.23. steeple
	// EventSpawn Thread �� ������.
	m_pcsAgsmEventSpawn->InsertAdditionalSpawn(AGSMSPAWN_ADDITIONAL_TYPE_SECRET_DUNGEON,
												this,
												CBRemoveDungeonMonster,
												0,
												(INT32)AGPDSPAWN_ARCHLORD_TYPE_SECRET_DUNGEON);
}

BOOL AgsmArchlord::CBRemoveDungeonMonster(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmArchlord* pThis = static_cast<AgsmArchlord*>(pClass);
	AdditionalSpawnData* pstSpawnData = static_cast<AdditionalSpawnData*>(pData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveDungeonMonster"));

	AgpdSpawnGroup *	pstGroup;
	INT32				lIndex = 0;

	for (pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex); pstGroup; pstGroup = pThis->m_pcsAgpmEventSpawn->GetGroupSequence(&lIndex))
	{
		for (ApdEvent	*pstEvent = pstGroup->m_pstChild; pstEvent; pstEvent = ((AgpdSpawn *) pstEvent->m_pvData)->m_pstNext)
		{
			if (!pstEvent->m_pvData)
				break;

			if (pstSpawnData->m_lData2 == ((AgpdSpawn *) pstEvent->m_pvData)->m_lSiegeWarType)
			{
				pThis->m_pcsAgsmEventSpawn->RemoveSpawnCharacter(pstEvent);
			}
		}
	}

	return TRUE;
}

BOOL AgsmArchlord::CBCharacterDead(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgsmArchlord* pThis = static_cast<AgsmArchlord*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);

	if(pThis->m_pcsAgpmArchlord->IsArchlord(pcsCharacter->m_szID))
	{
		// ��ũ�ε� ���� ��Ų��.
		pThis->CancelArchlord(pcsCharacter->m_szID);
	}

	return TRUE;
}

void AgsmArchlord::SendPacketCurrentStep()
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketCurrentStep(&nPacketLength, m_pcsAgpmArchlord->GetCurrentStep());

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacketAllUser(pvPacket, nPacketLength);
}

void AgsmArchlord::SendPacketMessageId(AgpdCharacter *pcsCharacter, AgpmArchlordMessageId eMessageID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakepacketMessageID(&nPacketLength, eMessageID);

	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
}

void AgsmArchlord::SendPacketArchlordID(AgpdCharacter *pcsCharacter)
{
	// ��ũ�ε� ���̵� ����
	if (m_pcsAgpmArchlord->m_szArchlord.IsEmpty())
		return;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmArchlord->MakePacketSetArchlord(&nPacketLength, m_pcsAgpmArchlord->m_szArchlord.GetBuffer());
	if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
}

// ���� ��ũ�ε� ����Ʈ
BOOL AgsmArchlord::OnSelectResultArchlord(AgpdCharacter *pcsCharacter, stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	if (bEnd)
		return TRUE;
		
	return TRUE;
}

// ȣ���� DB���� �о��
BOOL AgsmArchlord::LoadGuardFromDB()
{
	EnumCallback(AGSMARCHLORD_CB_DBSELECT_GUARD, NULL, NULL);
	
	return TRUE;
}

BOOL AgsmArchlord::OnSelectResultLordGuard(stBuddyRowset *pRowset, BOOL bEnd)
{
	if (NULL == pRowset && FALSE == bEnd)
		return FALSE;

	if (bEnd)
	{
		// ���⼭ ���� �ε��� �� ��������.
		return TRUE;
	}
	
	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		CHAR	*pszCharID = pRowset->Get(ul, 0);
		// �о������ �˾Ƽ�...
		// ���⼭ �� �������� �ƴ�. ���� ����.
		m_vtGuard.push_back(string(pszCharID));
	}

	return TRUE;
}

BOOL AgsmArchlord::SetCallbackDBSelectArclord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBSELECT_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBUpdateArclord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBUPDATE_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBInsertArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBINSERT_ARCHLORD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBSelectLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBSELECT_GUARD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBInsertLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBINSERT_GUARD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackDBDeleteLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_DBDELETE_GUARD, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackStartArchlordSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_START_ARCHLORD_SIEGEWAR, pfCallback, pClass);
}

BOOL AgsmArchlord::SetCallbackEndArchlordSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMARCHLORD_CB_END_ARCHLORD_SIEGEWAR, pfCallback, pClass);
}
// ��� ���� �ð��� ���������
// �����ȿ� ��ġ�� ��� ������ ���� ������ ������.
void AgsmArchlord::ExpelAllUsersFromSecretDungeon()
{
	INT32 lIndex = 0;
	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	while (pcsCharacter)
	{
		if (m_pcsAgpmCharacter->IsPC(pcsCharacter) && m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) != 0)
		{
			if (pcsCharacter->m_Mutex.WLock())
			{
				if (m_pcsAgpmSiegeWar->IsInSecretDungeon(pcsCharacter))
				{
					AuPOS stNewPosition;
					if (!m_pcsAgpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stNewPosition))
						stNewPosition = pcsCharacter->m_stPos;
					
					m_pcsAgpmCharacter->StopCharacter(pcsCharacter, NULL);
					m_pcsAgpmCharacter->UpdatePosition(pcsCharacter, &stNewPosition, 0, TRUE);
				}

				pcsCharacter->m_Mutex.Release();
			}
		}

		pcsCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	}
}
