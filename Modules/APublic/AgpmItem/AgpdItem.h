/******************************************************************************
Module:  AgpdItem.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 15
******************************************************************************/

#if !defined(__AGPDITEM_H__)
#define __AGPDITEM_H__

#include <ApModule/ApModule.h>
#include <ApBase/ApBase.h>
#include <AgpmItem/ApdItem.h>
#include <AgpmItem/AgpdItemTemplate.h> 
#include <AgpmItem/AgpdItemOptionTemplate.h>
#include <AgpmGrid/AgpdGrid.h>
#include <AgpmFactors/AgpdFactors.h>
#include <AgpmCharacter/AgpdCharacter.h>

#define AGPDITEM_MAKE_ITEM_ID(lIID,lITID)	((((INT64) (lITID)) << 32) | (lIID))
#define AGPDITEM_GET_IID(llID)				((INT32) ((llID) & 0xffffffff))
#define AGPDITEM_GET_ITID(llID)				((INT32) (((llID) >> 32) & 0xffffffff))

// AGPDITEM_CASH_ITEM_XXX
#define	AGPDITEM_CASH_ITEM_UNUSE				0
#define	AGPDITEM_CASH_ITEM_INUSE				1
#define AGPDITEM_CASH_ITEM_PAUSE				2

#define AGPDITEM_PRIVATE_TRADE_OPTION_MAX		3

#define	AGPDITEM_CASH_ITEM_MAX_USABLE_COUNT		0x0FFFFFFF
// Constant Values
//const INT32 AGPMITEM_MAX_ITEM_NAME						= 40;
const INT32 AGPMITEM_MAX_ITEM_MAGIC_ATTR				= 22;
const INT32 AGPMITEM_MAX_ITEM_SPIRITED					= 10;
const INT32 AGPMITEM_MAX_ITEM_DATA_COUNT				= 1000000;
const INT32 AGPMITEM_MAX_ITEM_OWN						= 100;		// �� ĳ���Ͱ� �ִ�� ���� �� �ִ� ���� ��

const INT32 AGPMITEM_MAX_PICKUP_ITEM_DISTANCE			= 400;

//Equip
const INT32 AGPMITEM_EQUIP_LAYER						= 3;
const INT32 AGPMITEM_EQUIP_ROW							= 3;
const INT32 AGPMITEM_EQUIP_COLUMN						= 4;

//Inventory
const INT32 AGPMITEM_MAX_INVENTORY						= 5;
const INT32 AGPMITEM_INVENTORY_ROW						= 5;
const INT32 AGPMITEM_INVENTORY_COLUMN					= 5;

const INT32 AGPAITEM_MAX_ITEM_TEMPLATE					= 256;

const INT32 AGPMITEM_BTREE_ORDER						= 30;

const INT32 AGPMITEM_INVALID_IID						= 0;

const INT32 AGPMITEM_SALES_BOX_SIZE						= 10;

//Bank
const INT32	AGPMITEM_BANK_MAX_LAYER						= 4;
const INT32 AGPMITEM_BANK_ROW							= 7;
const INT32 AGPMITEM_BANK_COLUMN						= 4;

//Trade Box(1:1��ȯ)
const INT32 AGPMITEM_TRADEBOX_LAYER						= 1; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����
const INT32 AGPMITEM_TRADEBOX_ROW						= 3; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����
const INT32 AGPMITEM_TRADEBOX_COLUMN					= 4; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����

//SalesBox
const INT32 AGPMITEM_SALES_LAYER						= 1; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����
const INT32 AGPMITEM_SALES_ROW							= 1; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����
const INT32 AGPMITEM_SALES_COLUMN						= 10; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����

//NPCTradeBox
const INT32 AGPMITEM_NPCTRADEBOX_LAYER					= 1; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����
const INT32 AGPMITEM_NPCTRADEBOX_ROW					= 20; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����
const INT32 AGPMITEM_NPCTRADEBOX_COLUMN					= 4; //��ȹ�� ��û�� ���� �󸶵��� ��������~ ����

//Quest Grid
const INT32 AGPMITEM_QUEST_LAYER						= 1;
const INT32 AGPMITEM_QUEST_ROW							= 4;
const INT32	AGPMITEM_QUEST_COLUMN						= 4;

//2005.11.16. By SungHoon for cash inventory
const INT32 AGPMITEM_CASH_INVENTORY_LAYER				= 1;
const INT32 AGPMITEM_CASH_INVENTORY_ROW					= 200;
const INT32	AGPMITEM_CASH_INVENTORY_COLUMN				= 1;

const INT32 AGPMITEM_GUILD_WAREHOUSE_LAYER				= 2;
const INT32 AGPMITEM_GUILD_WAREHOUSE_ROW				= 7;
const INT32	AGPMITEM_GUILD_WAREHOUSE_COLUMN				= 4;

// 2008.05.08. steeple
const INT32 AGPMITEM_SUB_LAYER							= 4;
const INT32 AGPMITEM_SUB_ROW							= 2;
const INT32 AGPMITEM_SUB_COLUMN							= 4;

/*
// 2008.11.07. iluvs for Unseen Inventory
const INT32 AGPMITEM_UNSEEN_LAYER						= 1;
const INT32 AGPMITEM_UNSEEN_ROW							= 2;
const INT32 AGPMITEM_UNSEEN_COLUMN						= 4;
*/

const INT32	AGPMITEM_MAX_USE_ITEM						= 20;	// ĳ���Ͱ� ���ÿ� ����� �� �ִ� �ִ� ���ۼ�

const INT32	AGPMITEM_MAX_CONVERT						= 10;
const INT32	AGPMITEM_MAX_CONVERT_WEAPON					= 10;	// ���� �����ΰ�� ���ɼ����� ������ �� �ִ� �ִ�Ƚ��
const INT32	AGPMITEM_MAX_CONVERT_ARMOUR					= 5;	// ���ΰ�� ���ɼ����� ������ �� �ִ� �ִ�Ƚ��

const INT32 AGPMITEM_MAX_DURABILITY						= 100;	// �ִ� ������

const INT32 AGPMITEM_DEFAULT_INVEN_NUM					= 30;	// ĳ���Ͱ� �⺻���� ������ �ִ� �κ��� ������ �迭 ����

const INT32	AGPMITEM_REQUIRE_LEVEL_UP_STEP				= 2;

const INT32 AGPMITEM_MAX_DELETE_REASON					= 32;

const INT32 AGPMITEM_TRANSFORM_RECAST_TIME				= 600000;

const INT32 AGPMITEM_REVERSE_ORB_REUSE_INTERVAL			= 1800000;	// 30��, ���������� ���� ��Ÿ��(ItemDataTable.txt���� ���������Ͽ� �������� �Űܿ�)

typedef enum _eAgpdItemDefaultInvenIndex
{
	AGPDITEM_DEFAULT_INVEN_INDEX_TID = 0,
	AGPDITEM_DEFAULT_INVEN_INDEX_STACK_COUNT,
	AGPDITEM_DEFAULT_INVEN_INDEX_NUM
} eAgpdItemDefaultInvenIndex;

typedef enum _eAgpdItemMaterial
{
	AGPDITEM_MATERIAL_NUM
} eAgpdItemMaterial;

typedef enum AgpdItemGridPos
{
	AGPDITEM_GRID_POS_TAB = 0,
	AGPDITEM_GRID_POS_ROW,
	AGPDITEM_GRID_POS_COLUMN,
	AGPDITEM_GRID_POS_NUM
} AgpdItemGridPos;

typedef enum AgpdItemTradeStatus
{
	AGPDITEM_TRADE_STATUS_NONE = 0,            //�ŷ����°� �ƴ�.
	AGPDITEM_TRADE_STATUS_WAIT_CONFIRM,        //�ŷ����ڰ� ��û���ϰ� ������ ��ٸ��ų� ���ؾ��ϴ� ��Ȳ
	AGPDITEM_TRADE_STATUS_TRADING,             //�ŷ���(�������� �ø��� ������....), Unlock����
	AGPDITEM_TRADE_STATUS_LOCK,				   //Lock�Ȱ��.
	AGPDITEM_TRADE_STATUS_READY_TO_EXCHANGE    //�ŷ��غ� �Ϸ�( �������� �� �ø��� ok��ư�� ���� ����)
} AgpdItemTradeStatus;

//typedef enum _AgpmItemConvertResult
//{
//	AGPMITEM_CONVERT_FAIL					= 0,
//	AGPMITEM_CONVERT_DIFFER_RANK,
//	AGPMITEM_CONVERT_DIFFER_TYPE,
//	AGPMITEM_CONVERT_FULL,
//	AGPMITEM_CONVERT_NOT_EQUIP_ITEM,
//	AGPMITEM_CONVERT_EGO_ITEM,
//	AGPMITEM_CONVERT_DESTROY_ATTRIBUTE,
//	AGPMITEM_CONVERT_DESTROY_ITEM,
//	AGPMITEM_CONVERT_SUCCESS,
//} AgpmItemConvertResult;

//typedef enum _AgpmItemEgoResult
//{
//	AGPMITEM_EGO_PUT_SOUL_SUCCESS	= 0,
//	AGPMITEM_EGO_PUT_SOUL_FAIL,
//	AGPMITEM_EGO_USE_SOULCUBE_SUCCESS,
//	AGPMITEM_EGO_USE_SOULCUBE_FAIL,
//} AgpmItemEgoResult;


typedef enum	_AgpmItemBoundTypes {
	E_AGPMITEM_NOT_BOUND				= 0,
	E_AGPMITEM_BIND_ON_ACQUIRE,
	E_AGPMITEM_BIND_ON_EQUIP,
	E_AGPMITEM_BIND_ON_USE,
	E_AGPMITEM_BIND_ON_GUILDMASTER		// ��� �����͸� ȹ�� ����
} AgpmItemBoundTypes;

//	Define item status flag
//////////////////////////////////////////////////////////////////

//	BoundTypes
//////////////////////////////////////////////////////////////////
#define	AGPMITEM_BIND_ON_ACQUIRE						0x00000001
#define	AGPMITEM_BIND_ON_EQUIP							0x00000002
#define	AGPMITEM_BIND_ON_USE							0x00000004
#define	AGPMITEM_BOUND_ON_OWNER							0x00000008

//	QuestType
//////////////////////////////////////////////////////////////////
#define	AGPMITEM_STATUS_QUEST							0x00000010

// CashType
#define AGPMITEM_STATUS_CASH_PPCARD						0x00000100

// ExtraType (Status)
#define AGPMITEM_STATUS_DISARMAMENT						0x00001000

#define AGPMITEM_STATUS_CANNOT_BE_SOLD					0x00002000

//	Define item status flag
//////////////////////////////////////////////////////////////////



//typedef	struct _stAgpmItemConvertHistory
//{
//	INT32					lConvertLevel;
//
//	FLOAT					fConvertConstant;
//	
//	BOOL					bUseSpiritStone[AGPMITEM_MAX_CONVERT];
//	
//	INT32					lTID[AGPMITEM_MAX_CONVERT];
//	ApBase					*pcsTemplate[AGPMITEM_MAX_CONVERT];
//
//	AgpmItemRuneAttribute	eRuneAttribute[AGPMITEM_MAX_CONVERT];
//	INT32					lRuneAttributeValue[AGPMITEM_MAX_CONVERT];
//
//	AgpdFactor				csFactorHistory[AGPMITEM_MAX_CONVERT];
//	AgpdFactor				csFactorPercentHistory[AGPMITEM_MAX_CONVERT];
//} stAgpmItemConvertHistory;

class AgpdItem;

typedef struct _stAgpmItemCheckPickupItem
{
	AgpdItem	*pcsItem;
	AgpdCharacter	*pcsCharacter;
	INT32		lStackCount;
} stAgpmItemCheckPickupItem, *pstAgpmItemCheckPickupItem;

typedef struct _stAgpmItemSkullInfo
{
	CHAR		szSkullOwner[AGPACHARACTER_MAX_ID_STRING + 1];
	CHAR		szKiller[AGPACHARACTER_MAX_ID_STRING + 1];
	//INT32		lPartyMemberID[AGPMPARTY_MAX_PARTY_MEMBER];
	ApSafeArray<INT32, AGPMPARTY_MAX_PARTY_MEMBER>	lPartyMemberID;

	INT32		lSkullUnionRank;
	INT32		lKillerUnionRank;
} AgpmItemSkullInfo;


//�������� �׸��忡 �ֱ����� ���� �ִ� �׸����� ��ġ ����
//Balanced BST�� �ϼ��Ǹ� ���߿� ��ü����~ �켱�� �迭��..... ��.��
class AgpmGridItemOriginInfo
{
public:
	INT64			m_llItemID;
	INT16			m_ePrevStatus;// �� ������ ����... (����, �κ��丮, ���ٴ� ���.)
	//INT16			m_anGridPos[AGPDITEM_GRID_POS_NUM];
	ApSafeArray<INT16, AGPDITEM_GRID_POS_NUM>		m_anGridPos;

	void Set( INT64 llItemID, AgpdItemStatus eStatus, INT16 nLayer, INT16 nRow, INT16 nColumn );
	void Reset();
};

class AgpdItemADCharTemplate : public ApBase {
public:
	//INT32			m_lDefaultEquipITID[AGPMITEM_PART_NUM];
	//ApSafeArray<INT32, AGPMITEM_PART_NUM>			m_lDefaultEquipITID;

	struct EquipItemSetInfo
	{
		INT32										m_nSetNumber;
		BOOL										m_bIsJumpingEventEquip;
		ApSafeArray<INT32, AGPMITEM_PART_NUM>		m_lDefaultEquipITID;

		EquipItemSetInfo()
			: m_nSetNumber(0), m_bIsJumpingEventEquip(FALSE)
		{
			m_lDefaultEquipITID.MemSetAll();
		}

		EquipItemSetInfo(const EquipItemSetInfo& stEquipInfo)
		{
			m_nSetNumber			= stEquipInfo.m_nSetNumber;
			m_bIsJumpingEventEquip	= stEquipInfo.m_bIsJumpingEventEquip;
			m_lDefaultEquipITID		= stEquipInfo.m_lDefaultEquipITID;
		}

		~EquipItemSetInfo()
		{
			m_lDefaultEquipITID.MemSetAll();
		}
	};

	struct InvenItemSetInfo
	{
		BOOL					m_bIsJumpingEventItem;
		INT32					m_aalDefaultInvenInfo[AGPMITEM_DEFAULT_INVEN_NUM][AGPDITEM_DEFAULT_INVEN_INDEX_NUM];

		InvenItemSetInfo()
			: m_bIsJumpingEventItem(FALSE)
		{
			ZeroMemory(m_aalDefaultInvenInfo, sizeof(m_aalDefaultInvenInfo));
		}
		InvenItemSetInfo(const InvenItemSetInfo& stInvenInfo)
		{
			m_bIsJumpingEventItem	= stInvenInfo.m_bIsJumpingEventItem;
			memcpy(m_aalDefaultInvenInfo, stInvenInfo.m_aalDefaultInvenInfo, sizeof(m_aalDefaultInvenInfo));
		}
	};

	//INT32							m_aalDefaultInvenInfo[AGPMITEM_DEFAULT_INVEN_NUM][AGPDITEM_DEFAULT_INVEN_INDEX_NUM];
	INT32							m_lDefaultMoney;

	std::vector<EquipItemSetInfo>	m_vtEquipItemSetInfo;
	std::vector<InvenItemSetInfo>	m_vtInvenItemSetInfo;
};

// 2008.02.14. steeple
struct AgpdItemCooldownBase
{
	INT32			m_lTID;
	UINT32			m_ulRemainTime;
	BOOL			m_bPause;
	BOOL			m_bPrevPause;

	AgpdItemCooldownBase() :
		m_lTID(0), m_ulRemainTime(0), m_bPause(0), m_bPrevPause(0)
	{;}

	bool operator == (INT32 lTID)
	{
		if(m_lTID == lTID)
			return true;

		return false;
	}
};

struct AgpdItemCooldownArray
{
	typedef vector<AgpdItemCooldownBase>			CooldownBaseVector;
	typedef vector<AgpdItemCooldownBase>::iterator	CooldownBaseIter;

	CooldownBaseVector* m_pBases;

	AgpdItemCooldownArray()
	{
		m_pBases = NULL;
	}
};

struct AgpdPrivateOptionItem
{
	INT32		m_lItemTID;
	INT32		m_lItemIID;
	BOOL		m_bUsed;
	BOOL		m_bOnGrid;
	
	AgpdPrivateOptionItem()
	{
		m_lItemTID = 0;
		m_lItemIID = 0;
		m_bUsed = FALSE;
		m_bOnGrid = FALSE;
	}
};

struct AgpdSealData
{
	CHAR strCharName[AGPDCHARACTER_NAME_LENGTH+1];
	INT32 TID;
	INT32 Level;
	INT64 Exp;
}; 

typedef struct
{
//public:
	INT16		m_nNumItem;							// ���� �ִ� ������ �� ����
//	INT16		m_nNewPart;							// ���� �ٲ� Item Part (Equip Callback�� ���) Remove 081002 Bob Jung.-�ʿ����.
//	PVOID		m_pvItemData[AGPMITEM_MAX_ITEM_OWN];	// ������ ����Ÿ ������

	AgpdGrid		m_csInventoryGrid;			// �κ��丮
	AgpdGrid		m_csEquipGrid;				// Equip
	AgpdGrid		m_csSalesBoxGrid;			// SalesBox
	AgpdGrid		m_csBankGrid;				// Bank
	AgpdGrid		m_csTradeGrid;				// �ڽ��� Trade Grid.
	AgpdGrid		m_csClientTradeGrid;		// �ŷ����� Trade Grid.
	AgpdGrid		m_csQuestGrid;				// Quest Item ������ Grid
	AgpdGrid		m_csCashInventoryGrid;		// CashItem �κ��丮
	AgpdGrid		m_csSubInventoryGrid;		// PetItem �κ��丮
//	AgpdGrid		m_csUnseenInventoryGrid;	// Unseen �κ��丮

	INT32			m_lTradeTargetID;			//������ �ŷ����ΰ�?
	INT32			m_lTradeStatus;				//�ŷ�����.
	INT32			m_lMoneyCount;				//���� ���濡�� �� �ݾ�
	INT32			m_lClientMoneyCount;		//������ ���� �� �ݾ�.
	bool			m_bConfirmButtonDown;		//�� ���߹�ư�� ���ȴ����� �˾Ƴ��� ���
	bool			m_bTargetConfirmButtonDown; //����� ���߹�ư�� ���ȴ����� �˾Ƴ��� ���

	INT32			m_lArrowIDInInventory;		// �κ��丮�ȿ� �ִ� ȭ�� ���� �Ƶ� (ȭ���� ã������ �Ź� �κ��丮�� �˻��� ���� ����.)
	INT32			m_lBoltIDInInventory;		// �κ��丮�ȿ� �ִ� ȭ�� ���� �Ƶ� (ȭ���� ã������ �Ź� �κ��丮�� �˻��� ���� ����.)

	INT32			m_lNumArrowCount;			// �κ��丮�ȿ� �ִ� �� ȭ�� ī��Ʈ
	INT32			m_lNumBoltCount;			// �κ��丮�ȿ� �ִ� �� ��Ʈ ī��Ʈ

	INT32			m_lHPPotionIDInInventory;	// �κ��丮�ȿ� �ִ� HP ���� �Ƶ�
	INT32			m_lMPPotionIDInInventory;	// �κ��丮�ȿ� �ִ� MP ���� �Ƶ�
	INT32			m_lSPPotionIDInInventory;	// �κ��丮�ȿ� �ִ� SP ���� �Ƶ�

	BOOL			m_bUseReturnTeleportScroll;	// �պ� ������ ����ߴ��� ����
	AuPOS			m_stReturnPosition;			// �պ� ������ ����� ����. �ǵ��ƿö� ���ڸ��� ���ƿ´�.

	UINT32			m_ulUseHPPotionTime;		// positon�� ���������� ����� �ð�
	UINT32			m_ulUseMPPotionTime;		// positon�� ���������� ����� �ð�
	UINT32			m_ulUseSPPotionTime;		// positon�� ���������� ����� �ð�

	UINT32			m_ulUseReverseOrbTime;		// reverse orb �� ���������� ����� �ð�
	UINT32			m_ulUseTransformTime;		// transform item �� ���������� ����� �ð�

	UINT32			m_ulUseJumpTime;			// ���������� jump ������ ����� �ð�

	UINT32			m_ulUseChattingEmphasis;	// ���������� ����� ������ ����� �ð�

	AgpdItemCooldownArray	m_CooldownInfo;		// ItemTID �� cooldown ����.

	// ���ΰŷ��� �ɼǾ����� 
	AgpdPrivateOptionItem	m_stTradeOptionItem[AGPDITEM_PRIVATE_TRADE_OPTION_MAX];
	AgpdPrivateOptionItem	m_stClientTradeOptionItem[AGPDITEM_PRIVATE_TRADE_OPTION_MAX];

} AgpdItemADChar;

class AgpdItem : public ApdItem {
public:
	AgpdItemTemplate*			m_pcsItemTemplate;

	INT32						m_nDimension	;		// �� �ε���.

	INT32						m_ulCID;
	AgpdCharacter *				m_pcsCharacter;
	INT32						m_lPrevOwner;
	//INT8						m_nColor[2];
	ApSafeArray<INT8, 2>		m_nColor;
	//BOOL						m_bStackable;
	INT32						m_nCount;
	//INT16						m_nKeeping;
	AuPOS						m_posItem;
	//FLOAT						m_fMagicAttr[AGPMITEM_MAX_ITEM_MAGIC_ATTR];
	//INT16						m_nSpirited[AGPMITEM_MAX_ITEM_SPIRITED];
	//INT16						m_nEgoExp;
	//INT16						m_nEgoLevel;
	//INT16						m_nDurability;

	AgpdFactor					m_csFactor;
	AgpdFactor					m_csFactorPercent;
	AgpdFactor					m_csRestrictFactor;

	INT16						m_eNewStatus;						// �� ������ ���� ���� (init ���¿���) (2006/2/23)... (����, �κ��丮, ���ٴ� ���.)
	INT16						m_eStatus;							// �� ������ ����... (����, �κ��丮, ���ٴ� ���.)

	INT32						m_lStatusFlag;

	AgpdCharacter				*m_pcsBoundOwner;

	ApBase						*m_pcsGuildWarehouseBase;

	//INT16						m_anGridPos[AGPDITEM_GRID_POS_NUM];
	ApSafeArray<INT16, AGPDITEM_GRID_POS_NUM>	m_anGridPos;
	ApSafeArray<INT16, AGPDITEM_GRID_POS_NUM>	m_anPrevGridPos;

//	stAgpmItemConvertHistory	m_stConvertHistory;

	// ego item
	CHAR						m_szSoulMasterID[AGPACHARACTER_MAX_ID_STRING + 1];		// ���� ���� �Ƶ�

	AgpmItemSkullInfo*			m_pstSkullInfo;						// �� ������ ������ ��츸 �޸� �Ҵ��ؼ� ����Ѵ�.

	AgpdGridItem*				m_pcsGridItem;

	UINT32						m_ulRemoveTimeMSec;		// �� ������ ������ Ŭ��ī��Ʈ

	//INT32						m_lDeleteReason;		// �� ������ DB���� �����ϴ� ��� �����϶�� �Ѱ����� ������ ����
	CHAR						m_szDeleteReason[AGPMITEM_MAX_DELETE_REASON + 1];

	INT32						m_lSkillTID;			// skill book(skill scroll) �� ��� �� ������ ����ؼ� ��� �� �ִ� ��ų TID ����
	INT32						m_lSkillLevel;			// skill scroll �� ��� �� ��ų ����

	ApSafeArray<UINT16, AGPDITEM_OPTION_MAX_NUM>		m_aunOptionTID;
	ApSafeArray<UINT16, AGPDITEM_OPTION_RUNE_MAX_NUM>	m_aunOptionTIDRune;
	ApSafeArray<AgpdItemOptionTemplate *, AGPDITEM_OPTION_MAX_NUM>		m_apcsOptionTemplate;
	ApSafeArray<AgpdItemOptionTemplate *, AGPDITEM_OPTION_RUNE_MAX_NUM>	m_apcsOptionTemplateRune;

	ApSafeArray<UINT16, AGPMITEM_MAX_SKILL_PLUS_EFFECT>	m_aunSkillPlusTID;		// 2007.02.05. steeple

//	2005.11.30. By SungHoon	For CashItem
	INT8						m_nInUseItem;			//	���������
	INT64						m_lRemainTime;			//	���� ���Ⱓ, milli-seconds
	UINT32						m_lExpireTime;			//	�Ϸ�ð�, seconds, time_t

	INT32						m_lCashItemUseCount;	//	���Ƚ��

	INT64						m_llStaminaRemainTime;	// A remaining time of staminas		2008.06.09. steeple
};


#endif //__AGPDITEM_H__