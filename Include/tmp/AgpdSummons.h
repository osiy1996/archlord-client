// AgpdSummons.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2005. 07. 26.

#ifndef _AGPDSUMMONS_H_
#define _AGPDSUMMONS_H_

#include "ApBase.h"
#include "AgpdCharacter.h"

const INT32 AGPMSUMMONS_MAX_SUMMONS			= 5;		// ��ȯ�� �ִ� ����
const INT32 AGPMSUMMONS_MAX_TAME			= 10;		// ���̹� �ִ� ����
const INT32 AGPMSUMMONS_MAX_FIXED			= 5;		// ������ ��ȯ�� �ִ� ����
const INT32 AGPMSUMMONS_MAX_SUMMONS_START	= 1;		// �⺻���� �����ϴ� ����. ��ų�� �����ȴ�.

enum EnumAgpdSummonsType
{
	AGPMSUMMONS_TYPE_NONE		= 0,
	AGPMSUMMONS_TYPE_ELEMENTAL	= (1 << 0),
	AGPMSUMMONS_TYPE_TAME		= (1 << 1),
	AGPMSUMMONS_TYPE_FIXED		= (1 << 2),
	AGPMSUMMONS_TYPE_PET		= (1 << 3),				// 2007.03.20. steeple
	AGPMSUMMONS_TYPE_PET2		= (1 << 4),				// 2008.05.08. steeple
	AGPMSUMMONS_TYPE_MONSTER	= (1 << 5),				
};

enum EnumAgpdSummonsPropensity
{
	AGPMSUMMONS_PROPENSITY_ATTACK	= 0,
	AGPMSUMMONS_PROPENSITY_DEFENSE	= (1 << 0),
	AGPMSUMMONS_PROPENSITY_SHADOW	= (1 << 1),
};

enum EnumAgpdSummonsStatus
{
	AGPMSUMMONS_STATUS_NORMAL		= 0,
	AGPMSUMMONS_STATUS_HUNGRY		= (1 << 0),
};

struct AgpdSummons
{
	INT32						m_lCID;
	INT32						m_lTID;
	EnumAgpdSummonsType			m_eType;
	EnumAgpdSummonsPropensity	m_ePropensity;
	EnumAgpdSummonsStatus		m_eStatus;
	UINT32						m_ulStartTime;
	UINT32						m_ulEndTime;

	bool operator == (INT32 lCID)
	{
		if (m_lCID == lCID)
			return true;

		return false;
	}
};

struct AgpdSummonsArray
{
	// 2006.10.26. steeple
	// ������ �迭�� vector �� ���� ����.
	// ���� ���� Summons �� ��ȯ�ϴ� ��ų�� ���ܼ�...

	typedef vector<AgpdSummons>					SummonsVector;
	typedef vector<AgpdSummons>::iterator		iterator;

	INT8 m_cMaxNum;
	SummonsVector* m_pSummons;

	//typedef ApSafeArray<AgpdSummons, AGPMSUMMONS_MAX_SUMMONS>::iterator iterator;
	//ApVector<AgpdSummons, AGPMSUMMONS_MAX_SUMMONS> m_arrSummons;
};

struct AgpdTameArray
{
	typedef ApSafeArray<AgpdSummons, AGPMSUMMONS_MAX_TAME>::iterator iterator;

	INT8 m_cMaxNum;
	ApVector<AgpdSummons, AGPMSUMMONS_MAX_TAME> m_arrTame;
};

struct AgpdFixedArray
{
	typedef ApSafeArray<AgpdSummons, AGPMSUMMONS_MAX_FIXED>::iterator iterator;

	INT8 m_cMaxNum;
	ApVector<AgpdSummons, AGPMSUMMONS_MAX_FIXED> m_arrFixed;
};

// �޸𸮸� ���̱� ���ؼ� �Ʒ��� ���� ��.
struct AgpdSummonsADChar
{
	AgpdSummonsArray m_SummonsArray;			// For Owner - ��ȯ���� ����
	AgpdTameArray m_TameArray;					// For Owner - ���̹� ���͸� ����
	AgpdFixedArray m_FixedArray;				// For Owner - ������ ��ȯ���� ����

	INT32 m_lOwnerCID;							// For Summons - ���� ��ȯ�� ������ CID
	INT32 m_lOwnerTID;							// For Summons - ���� ��ȯ�� ������ TID
	AgpdSummons m_stSummonsData;				// For Summons - ���� ��ȯ�Ǿ����� �ʿ��� ������(��ȯ �ð� ��)
};

#endif //_AGPDSUMMONS_H_
