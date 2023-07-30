/******************************************************************************
Module:  AgpdShrine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 22
******************************************************************************/

#if !defined(__AGPDSHRINE_H__)
#define __AGPDSHRINE_H__

#include <ApBase/ApBase.h>
#include <AgpmFactors/AgpdFactors.h>
#include <AgpmEventSpawn/AgpdEventSpawn.h>

const int	AGPMSHRINE_MAX_POSITION				= 20;
const int	AGPMSHRINE_MAX_NAME					= 32;

const int	AGPMSHRINE_MAX_LEVEL				= 5;
const int	AGPMSHRINE_MAX_SHRINE				= 40;
const int	AGPMSHRINE_MAX_KILL_GUARDIAN		= 10;

typedef enum _eAgpmShrineUseCondition {
	AGPMSHRINE_USE_ONCE							= 1,
	AGPMSHRINE_USE_WHEN_ACTIVE,
	AGPMSHRINE_USE_TIME_INTERVAL
} eAgpmShrineUseCondition;

typedef enum _eAgpmShrineActiveCondition {
	AGPMSHRINE_ACTIVE_ALWAYS					= 1,
	AGPMSHRINE_ACTIVE_DAY_ONLY,
	AGPMSHRINE_ACTIVE_SPAWN
} eAgpmShrineActiveCondition;

typedef enum _eAgpmShrineReActiveCondition {
	AGPMSHRINE_REACTIVE_NONE					= 1,
	AGPMSHRINE_REACTIVE_DAY_ONLY,
	AGPMSHRINE_REACTIVE_TWICE_LAST_ACTIVE_TIME,
} eAgpmShrineReActiveCondition;

class AgpdShrineTemplate : public ApBase {
public:
	CHAR						m_szShrineName[AGPMSHRINE_MAX_NAME];

	// �� ����� �ɷ� (��ų)
	CHAR						m_szSkillName[AGPMSHRINE_MAX_LEVEL][AGPMSKILL_MAX_SKILL_NAME + 1];

	// �����ϰ� �����Ǵ� ������� �ƴ���
	BOOL						m_bIsRandomPosition;
	// shrine�� ��ġ��... �������ִ°�� m_posShrine[0], �׷��� ��������  m_nNumShrinePos ��ŭ..
	AuPOS						m_posShrine[AGPMSHRINE_MAX_POSITION];
	INT32						m_nNumShrinePos;

	// �� ����� ����ϴ� ���.. (�ѹ�����, Ȱ��ȭ �ɶ���������, �־��� �ð� ��������)
	eAgpmShrineUseCondition		m_eUseCondition;

	UINT32						m_ulUseIntervalTimeHR;

	// Ȱ��ȭ ����.. (�׻�, ������, ����)
	eAgpmShrineActiveCondition	m_eActiveCondition;

	UINT32						m_ulMinActiveTimeHR;
	UINT32						m_ulMaxActiveTimeHR;

	// �ٽ� Ȱ��Ȱ�� ����
	eAgpmShrineReActiveCondition	m_eReActiveCondition;

	UINT32						m_ulReActiveTimeHR;

	// �� ����� ����� ���� �̸�
	CHAR						m_szGuardianSpawnName[AGPMSHRINE_MAX_LEVEL][AGPDSPAWN_SPAWN_NAME_LENGTH];

	// sigil data��...
	//		m_csSigilAttrFactor	: status, magic defense �� �� sigil�� ���� ���� �����Ѵ�.
	//			- 0�� �ƴѰ��ΰ�� %�� Ȯ���� �ش� �Ӽ����� ���´�.
	//		m_csMinValueFactor : �� Ȯ���� ���� �Ӽ��� ���´ٰ� ����� �Ǿ����� �Ӽ����� �ּ�ġ
	//		m_csMaxValueFactor : �� Ȯ���� ���� �Ӽ��� ���´ٰ� ����� �Ǿ����� �Ӽ����� �ִ�ġ
	BOOL						m_bIsSigil;
	AgpdFactor					m_csSigilAttrFactor;
	AgpdFactor					m_csMinValueFactor;
	AgpdFactor					m_csMaxValueFactor;
};

class AgpdShrine : public ApBase {
public:
	AuPOS						m_posShrine;

	BOOL						m_bIsActive;

	UINT32						m_ulLastActiveTimeMSec;
	UINT32						m_ulLastInactiveTimeMSec;

	UINT32						m_ulLastActiveDurationMSec;

	AgpdShrineTemplate*			m_pcsTemplate;

	ApBase*						m_pcsBase;

	INT32						m_lSkillID[AGPMSHRINE_MAX_LEVEL];

	AgpdFactor					m_csSigilFactor;
	INT32						m_lSigilOwnerUnion;
	ApBase						m_csSigilOwnerBase;
};

class AgpdShrineADChar {
public:
	BOOL						m_bIsGuardian;

	INT32						m_lShrineID;
	INT32						m_lGuardianLevel;

	// ����� ���� ����
	INT32						m_lKillGuardianNum[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL];
	UINT32						m_ulKillGuardianTime[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL][AGPMSHRINE_MAX_KILL_GUARDIAN];

	// shrine ��� ����
	BOOL						m_bUseShrineNum[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL];
	UINT32						m_ulUseShrineTimeMSec[AGPMSHRINE_MAX_SHRINE][AGPMSHRINE_MAX_LEVEL];
};

class AgpdShrineADObject {
public:
	CHAR						m_szShrineName[AGPMSHRINE_MAX_NAME];
	INT32						m_lShrineID;
};

class AgpdShrineADSpawn {
public:
	// spawn �Ǵ� ��� ����
	CHAR						m_szShrineName[AGPMSHRINE_MAX_NAME];
	INT32						m_lShrineID;

	// �� ������� ������ �Ǵµ� ��� ������ ������ �Ǵ���...
	INT32						m_lShrineLevel;
};

#endif //__AGPDSHRINE_H__
