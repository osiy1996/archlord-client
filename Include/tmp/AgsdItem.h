/******************************************************************************
Module:  AgsdItem.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 07. 29
******************************************************************************/

#if !defined(__AGSDITEM_H__)
#define __AGSDITEM_H__

#include "AgpmItem.h"
#include "AgsdAccount.h"

typedef enum _eAgsdItemPauseReason
{
	AGSDITEM_PAUSE_REASON_NONE = 0,
	AGSDITEM_PAUSE_REASON_INVISIBLE,
	AGSDITEM_PAUSE_REASON_RIDE,
	AGSDITEM_PAUSE_REASON_TRANSFORM,
	AGSDITEM_PAUSE_REASON_JAIL,
	AGSDITEM_PAUSE_REASON_EVOLUTION,
} eAgsdItemPauseReason;

class AgsdItem {
public:
	UINT64		m_ullDBIID;				// DB�� ���ִ� ���� �Ƶ�

	UINT32		m_ulOwnTime;			// �� ������ ȹ���� �ð�

	INT32		m_lUseItemRemain;		// ������ use �����ΰ��, �׸��� ����� ���� ���Ǵ� ��� �����ִ� ���Ƚ��
	UINT32		m_ulNextUseItemTime;	// ������ ���Ǵ� Ƚ���� �����ִ°�� ���� ���ð�

	// item drop & loot
	UINT32		m_ulDropTime;			// ������ ���� �ð�
	ApBaseExLock	m_csFirstLooterBase;	// ���ÿ� �켱���� ���� �ִ� ĳ����

	BOOL		m_bIsNeedInsertDB;		// DB�� ������ �ؾ� �ϴ� ������ ����
										// DB�� �������� �̰��� FALSE�� �����ؾ� �Ѵ�.

	INT32		m_lPrevSaveStackCount;	// 

	eAgsdItemPauseReason	m_ePauseReason;	// � �������� ���ؼ� �������� ���� �Ǿ�����. �ַ� NONE �̴�.
	UINT32		m_ulPausedClock;

	stOptionSkillData		m_stOptionSkillData;	// 2007.05.04. steeple 

	//BOOL		m_bUseOnEnterGameWorld;	// EnterGameWorld �ÿ� �ڵ����� ���Ǵ� ���������� (��:�̹� ������� ĳ��������) 2005.12.13. steeple
};

class AgsdItemADChar 
{
public:
	ApSafeArray<INT32, AGPMITEM_MAX_USE_ITEM>		m_lUseItemID;
	INT32		m_lNumUseItem;
	
	BOOL		m_bUseTimeLimitItem;
	UINT32		m_ulTimeLimitItemCheckClock;

	ApSafeArray<PVOID, AGPMITEM_PART_NUM>		m_pvPacketEquipItem;
	ApSafeArray<INT16, AGPMITEM_PART_NUM>		m_lPacketEquipItemLength;

	// ������ ��� �����Ѵ�.
	INT32		m_lAddMaxHP;
	INT32		m_lAddMaxMP;
};

typedef enum _eAgsdItemCashItemBuyListStatus {
	AGSDITEM_CASHITEMBUYLIST_STATUS_REQUEST,
	AGSDITEM_CASHITEMBUYLIST_STATUS_COMPLETE,
	AGSDITEM_CASHITEMBUYLIST_STATUS_DRAWBACK,
	AGSDITEM_CASHITEMBUYLIST_STATUS_GMGIVE,
	//AGSDITEM_CASHITEMBUYLIST_STATUS_EXHAUST,
	} eAgsdItemCashItemBuyListStatus;

typedef struct _stCashItemBuyList {
	UINT64		m_ullBuyID;
	INT32		m_lItemTID;
	INT32		m_lItemQty;
	UINT64		m_ullItemSeq;
	INT64		m_llMoney;
	UINT64		m_ullOrderNo;
	INT8		m_cStatus;
	
	CHAR			m_szCharName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR			m_szAccName[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];
	CHAR			m_szIP[15];
	INT32			m_lCharTID;
	INT32			m_lCharLevel;
	
	CHAR			m_szOrderID[32 + 1];
} stCashItemBuyList;

struct AgsdSealData
{
	UINT64 DBID;
	AgpdSealData SealData;
};

#endif //__AGSDITEM_H__
