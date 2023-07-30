/******************************************************************************
Module:  AgsdDeath.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 11
******************************************************************************/

#if !defined(__AGSDDEATH_H__)
#define __AGSDDEATH_H__

#include "ApBase.h"
#include "AsDefine.h"

typedef struct
{
	UINT32				m_ulDeadTime;		// ���� �ð� (game clock count ����)
											// (�ױ� ���ð�, ������ �����ð� : m_fCharStatus�� ���� �����ϴ� ���� �޶�����.)

	BOOL				m_bIsNeedProcessDeath;	// ���� �㿡 Exp, ����� ó���� �ؾ� �ϴ���..

	INT64				m_llLastLoseExp;		// ���������� ���� Exp (������ ���� �������� Exp ���� ������ ���ȴ�.)

	ApBaseExLock		m_csLastHitBase;	// �̳����� ���������� �¾ �׾���. ��.��

	BOOL				m_bDropItem;		// �����㿡 ������ ���ɴ��� ����

	UINT32				m_ulRecoveryHPPointTime;	// HP Point ȸ�� Interval. �Ź� �ٲ� �� �־ �̷��� �и�. 2005.04.09. steeple
	UINT32				m_ulRecoveryMPPointTime;	// MP Point ȸ�� Interval. �Ź� �ٲ� �� �־ �̷��� �и�. 2005.04.09. steeple

	INT32				m_lDeadTargetCID;	// 2005.07.28. steeple. ���� ������ �׾��� �� �˱� ���ؼ�.
	INT8				m_cDeadType;		// 2005.07.28. steeple. ��Ȱ �г�Ƽ ������ �� �ٲ�.

	CTime				m_tCharismaGiveTime;	// ī������ ����Ʈ�� �� �ð�(���¡ ������) - arycoat 2008.7.
} AgsdDeath;

typedef struct
{
	INT32				m_lDropRate;		// �׾����� Item�� Drop�� Ȯ�� (0~100000)
} AgsdDeathADItem;

#endif //__AGSDDEATH_H__
