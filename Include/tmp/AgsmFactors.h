/******************************************************************************
Module:  AgsmFactors.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 10. 16
******************************************************************************/

#if !defined(__AGSMFACTORS_H__)
#define __AGSMFACTORS_H__

#include "ApBase.h"

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmTitle.h"
#include "AgsEngine.h"
#include "AuRandomNumber.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmFactorsD" )
#else
#pragma comment ( lib , "AgsmFactors" )
#endif
#endif


typedef enum _eAgsmFactorsCB {
	AGSM_FACTORS_CB_FACTOR_DB			= 0,

	/*
	AGSM_FACTORS_CB_DB_STREAM_INSERT,
	AGSM_FACTORS_CB_DB_STREAM_DELETE,
	AGSM_FACTORS_CB_DB_STREAM_SELECT,
	AGSM_FACTORS_CB_DB_STREAM_UPDATE
	*/
} eAgsmFactorsCB;

typedef enum _eAgsmFactorsAttackType {
	AGSM_FACTORS_MELEE_ATTACK			= 0,
	AGSM_FACTORS_SKILL_ATTACK
} eAgsmFactorsAttackType;


// �ֱ������� Max���� ���� HP, MP, SP�� ������ �ִ� ĳ���͵��� ���� �÷��ش�. �׶� ���Ǵ� �����̴�.
// �����϶� (�� �����̰� ������)
const int	AGSMFACTORS_RECOVERY_POINT_HP						= 7;
const int	AGSMFACTORS_RECOVERY_POINT_MP						= 4;
const int	AGSMFACTORS_RECOVERY_POINT_SP						= 4;

// �ƹ����� ���ϰ� ������...
const int	AGSMFACTORS_RECOVERY_POINT_NOT_ACTION_HP			= 11;
const int	AGSMFACTORS_RECOVERY_POINT_NOT_ACTION_MP			= 6;
const int	AGSMFACTORS_RECOVERY_POINT_NOT_ACTION_SP			= 6;

class AgsmFactors : public AgsModule {
private:
	AgpmFactors		*m_pagpmFactors;
	AgpmCharacter	*m_pagpmCharacter;
	AgpmItem		*m_pagpmItem;
	AgpmTitle		*m_pagpmTitle;

	MTRand			m_csRandom;

public:
	AgsmFactors();
	~AgsmFactors();

	// Virtual Function ��
	BOOL	OnInit();
	BOOL	OnDestroy();

	// �׾��� TRUE, ������� FALSE ����
	BOOL IsDead(AgpdFactor *pcsFactor);
	BOOL Resurrection(AgpdFactor *pcsFactor, PVOID *pvPacket);

	PVOID SetCharPoint(AgpdFactor *pcsFactor, BOOL bResultFactor = TRUE, BOOL bMakePacket = TRUE);
	PVOID SetCharPointFull(AgpdFactor *pcsFactor);

	BOOL ResetCharMaxPoint(AgpdFactor *pcsFactor);
	BOOL AdjustCharMaxPoint(AgpdFactor *pcsFactor);
	BOOL AdjustCharMaxPoint(AgpdFactor *pcsBaseFactor, AgpdFactor *pcsTargetFactor);
	BOOL AdjustCharMaxPointFromBaseFactor(AgpdFactor* pcsUpdateFactor, AgpdFactor* pcsBaseFactor);	// 2005.06.29. steeple
	BOOL AdjustWeaponFactor(AgpdCharacter* pcsCharacter, AgpdFactor* pcsUpdateResultFactor);

	PVOID AddCharPoint(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsFromResultFactor = FALSE, FLOAT fApplyPercent = 1.0f);
	PVOID AddCharPointByPercent(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsFromResultFactor = FALSE, FLOAT fApplyPercent = 1.0f);
	BOOL UpdateCharPoint(AgpdFactor *pcsFactor, PVOID *pvPacket, INT32 lHP, INT32 lMP, INT32 lSP);

	BOOL RecoveryPointInterval(AgpdFactor *pcsFactor, PVOID *pvPacket, INT32 lModifiedPointHP, INT32 lModifiedPointMP, BOOL bIsRecoveryHP = TRUE, BOOL bIsRecoveryMP = TRUE);
	BOOL SaveResultPoint(AgpdFactor *pcsFactor);
	// ���� �ʿ� ���͵��� ����ؼ� �������ش�.
	//	    �ٸ� ���鿡�� ���Ϳ� �ִ� ���� �ʿ�� �� ��� ���⿡ �������̽��� ����� (�ʿ��ϴٸ� ������ �� �ؼ�) �Ѱ��ش�.
	INT16	GetAttackSpeed(AgpdFactor *pcsFactor);

	PVOID	MakePacketSyncParty(AgpdFactor *pcsFactor);
	PVOID	MakePacketUpdateLevel(AgpdFactor *pcsFactor);
	PVOID	MakePacketUpdateUnionRank(AgpdFactor *pcsFactor);
	PVOID	MakePacketDBData(AgpdFactor *pcsFactor);

	FLOAT	Max(FLOAT lValue1, FLOAT lValue2);
	FLOAT	Min(FLOAT lValue1, FLOAT lValue2);

	BOOL	ResetResultFactorDamageInfo(AgpdFactor *pcsFactor);
};

#endif //__AGSMFACTORS_H__
