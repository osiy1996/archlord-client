// AgpmPvP.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 16.

#ifndef _AGPMPVP_H_
#define _AGPMPVP_H_

#include <ApModule/ApModule.h>
#include <AuPacket/AuPacket.h>
#include <ApmMap/ApmMap.h>
#include <AgpmFactors/AgpdFactors.h>
#include <AgpmPvP/AgpdPvP.h>
#include <AuRandomNumberGenerator/AuRandomNumber.h>

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgpmPvPD.lib")
#else
	#pragma comment(lib, "AgpmPvP.lib")
#endif
#endif

#define AGPMPVP_RELEASE_PVP_TIME			300000			// �ٽ� 5�� (2007.02.02 ����)
#define AGPMPVP_INVINCIBLE_TIME				30000			// 20��
#define AGPMPVP_MAX_SKULL_LEVEL_BRANCH		5				// �׳� �� ����� ����
#define AGPMPVP_LEVEL_GAP_ZERO				123456			// Admin ���� Key �� 0 ���� �� �� �����Ƿ� ��ü
#define AGPMPVP_BATTLE_SQAURE_REQUIRE_LEVEL	10				// ��Ʋ ������ ���� ����
//#define AGPMPVP_BATTLE_SQAURE_REQUIRE_LEVEL	99				// ��Ʋ ������ ���� ����

#if defined(_AREA_JAPAN_)||defined(_AREA_CHINA_)
	#define AGPMPVP_MINIMUM_ABLE_LEVEL			30				// PVP ������ �ּҷ���
#else
	#define AGPMPVP_MINIMUM_ABLE_LEVEL			20				// PVP ������ �ּҷ���
#endif
typedef enum _eAgpmPvPPacketType
{
	AGPMPVP_PACKET_PVP_INFO = 0,
	AGPMPVP_PACKET_ADD_FRIEND,
	AGPMPVP_PACKET_ADD_ENEMY,
	AGPMPVP_PACKET_REMOVE_FRIEND,
	AGPMPVP_PACKET_REMOVE_ENEMY,
	AGPMPVP_PACKET_INIT_FRIEND,
	AGPMPVP_PACKET_INIT_ENEMY,
	AGPMPVP_PACKET_UPDATE_FRIEND,
	AGPMPVP_PACKET_UPDATE_ENEMY,
	AGPMPVP_PACKET_ADD_FRIEND_GUILD,
	AGPMPVP_PACKET_ADD_ENEMY_GUILD,
	AGPMPVP_PACKET_REMOVE_FRIEND_GUILD,
	AGPMPVP_PACKET_REMOVE_ENEMY_GUILD,
	AGPMPVP_PACKET_INIT_FRIEND_GUILD,
	AGPMPVP_PACKET_INIT_ENEMY_GUILD,
	AGPMPVP_PACKET_UPDATE_FRIEND_GUILD,
	AGPMPVP_PACKET_UPDATE_ENEMY_GUILD,
	
	AGPMPVP_PACKET_CANNOT_USE_TELEPORT,

	AGPMPVP_PACKET_SYSTEM_MESSAGE,

	AGPMPVP_PACKET_REQUEST_DEAD_TYPE,
	AGPMPVP_PACKET_RESPONSE_DEAD_TYPE,

	AGPMPVP_PACKET_RACE_BATTLE,

	AGPMPVP_PACKET_MAX,
} eAgpmPvPPacketType;

typedef enum _eAgpmPvPCallback
{
	AGPMPVP_CB_PVP_INFO = 0,
	AGPMPVP_CB_ADD_FRIEND,
	AGPMPVP_CB_ADD_ENEMY,
	AGPMPVP_CB_REMOVE_FRIEND,
	AGPMPVP_CB_REMOVE_ENEMY,
	AGPMPVP_CB_PVP_RESULT,
	AGPMPVP_CB_INIT_FRIEND,
	AGPMPVP_CB_INIT_ENEMY,
	AGPMPVP_CB_UPDATE_FRIEND,
	AGPMPVP_CB_UPDATE_ENEMY,
	AGPMPVP_CB_ADD_FRIEND_GUILD,
	AGPMPVP_CB_ADD_ENEMY_GUILD,
	AGPMPVP_CB_REMOVE_FRIEND_GUILD,
	AGPMPVP_CB_REMOVE_ENEMY_GUILD,
	AGPMPVP_CB_INIT_FRIEND_GUILD,
	AGPMPVP_CB_INIT_ENEMY_GUILD,
	AGPMPVP_CB_UPDATE_FRIEND_GUILD,
	AGPMPVP_CB_UPDATE_ENEMY_GUILD,

	AGPMPVP_CB_RECV_CANNOT_USE_TELEPORT,

	AGPMPVP_CB_ENTER_SAFE_PVP_AREA,
	AGPMPVP_CB_ENTER_FREE_PVP_AREA,
	AGPMPVP_CB_ENTER_COMBAT_PVP_AREA,
	AGPMPVP_CB_LEAVE_SAFE_PVP_AREA,
	AGPMPVP_CB_LEAVE_FREE_PVP_AREA,
	AGPMPVP_CB_LEAVE_COMBAT_PVP_AREA,

	AGPMPVP_CB_SYSTEM_MESSAGE,

	AGPMPVP_CB_REQUEST_DEAD_TYPE,
	AGPMPVP_CB_RESPONSE_DEAD_TYPE,

	AGPMPVP_CB_IS_FRIEND_SIEGEWAR,
	AGPMPVP_CB_IS_ENEMY_SIEGEWAR,		// ���������ؼ� ������ �� �ִ� �� üũ�Ѵ�. 2006.06.20. steeple

	AGPMPVP_CB_CHECK_NPC_ATTACKABLE_TARGET,
	AGPMPVP_CB_IS_ATTACKABLE_TIME,
	
	AGPMPVP_CB_RACE_BATTLE_STATUS,
	
	AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA,		// ���� �������� ������ �ִ��� üũ

	AGPMPVP_CB_MAX,
} eAgpmPvPCallback;

typedef enum _eAgpmPvPDataType
{
	AGPMPVP_DATA_TYPE_CHAR_INFO = 0,
	AGPMPVP_DATA_TYPE_GUILD_INFO,
	AGPMPVP_DATA_TYPE_AREA_DROP,
	AGPMPVP_DATA_TYPE_ITEM_DROP,
	AGPMPVP_DATA_TYPE_SKULL_DROP,
} eAgpmPvPDataType;

#define AGPMPVP_EXCEL_ITEMDROP_KIND_FREEAREA			"������������"
#define AGPMPVP_EXCEL_ITEMDROP_KIND_COMBATAREA			"������������"

typedef enum _eAgpmPvPItemDropKind				// Excel ������ ������ ���ƾ� �Ѵ�.
{
	AGPMPVP_ITEM_DROP_KIND_WEAPON = 1,
	AGPMPVP_ITEM_DROP_KIND_ARMOR,
	AGPMPVP_ITEM_DROP_KIND_SPIRIT_STONE,		// ���ɼ�
	AGPMPVP_ITEM_DROP_KIND_RUNE,				// �����
	AGPMPVP_ITEM_DROP_KIND_ACCESSORY,			// �Ǽ�����
	AGPMPVP_ITEM_DROP_KIND_STRENGTH,			// ��ȭ��
	AGPMPVP_ITEM_DROP_KIND_POTION,				// ����
	AGPMPVP_ITEM_DROP_KIND_SCROLL,				// ��ũ��
	AGPMPVP_ITEM_DROP_KIND_MATERIAL,			// ���
	AGPMPVP_ITEM_DROP_KIND_PRODUCT,				// ����ǰ
} eAgpmPvPItemDropKind;

typedef enum _eAgpmPvPItemDropPos
{
	AGPMPVP_ITEM_DROP_POS_EQUIP = 0x10000000,
	AGPMPVP_ITEM_DROP_POS_INVEN = 0x20000000,
} eAgpmPvPItemDropPos;

typedef enum _eAgpmPvPInitType
{
	AGPMPVP_INIT_TYPE_FRIEND = 0,
	AGPMPVP_INIT_TYPE_ENEMY,
	AGPMPVP_INIT_TYPE_FRIEND_GUILD,
	AGPMPVP_INIT_TYPE_ENEMY_GUILD,
} eAgpmPvPInitType;

typedef enum _eAgpmPvPSystemMessageCode
{
	AGPMPVP_SYSTEM_CODE_ENTER_SAFE_AREA = 0,		// ���� ���� ����
	AGPMPVP_SYSTEM_CODE_ENTER_FREE_AREA,			// ������������ ����
	AGPMPVP_SYSTEM_CODE_ENTER_COMBAT_AREA,			// ������������ ����
	AGPMPVP_SYSTEM_CODE_NOW_PVP_STATUS,				// PvP ���� ����
	AGPMPVP_SYSTEM_CODE_NONE_PVP_STATUS,			// PvP ��
	AGPMPVP_SYSTEM_CODE_DEAD,						// ���� ������� ���ư�.
	AGPMPVP_SYSTEM_CODE_DEAD_GUILD_MEMBER,			// ��� ����� �׾���.
	AGPMPVP_SYSTEM_CODE_DEAD_PARTY_MEMBER,			// ��Ƽ ����� �׾���.
	AGPMPVP_SYSTEM_CODE_ITEM_DROP,					// �������� �������. �����������.
	AGPMPVP_SYSTEM_CODE_START_INVINCIBLE,			// �������� (��Ʋ������ �����Ҷ�)
	AGPMPVP_SYSTEM_CODE_END_INVINCIBLE,				// �������� ��
	AGPMPVP_SYSTEM_CODE_CANNOT_USE_TELEPORT,		// �������� ���������� �̵����� ��� �Ұ�
	AGPMPVP_SYSTEM_CODE_KILL_PLAYER,				// �̱� �������.
	AGPMPVP_SYSTEM_CODE_MOVE_BATTLE_SQAURE,			// ��Ʋ������ ����
	AGPMPVP_SYSTEM_CODE_LEAVE_GUILD_OR_PARTY,		// ��Ʋ������ ���� ���ǿ��� �ɷ��� ��
	AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_GUILD,		// ��� ��� �ʴ� �Ұ�
	AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_PARTY,		// ��Ƽ ��� �ʴ� �Ұ�
	AGPMPVP_SYSTEM_CODE_TARGET_INVINCIBLE,			// Ÿ���� ��������
	AGPMPVP_SYSTEM_CODE_NOT_ENOUGH_LEVEL,			// ��Ʋ ������ ���� ���� ���ڶ�
	AGPMPVP_SYSTEM_CODE_CANNOT_INVITE_MEMBER,		// �ʴ��Ϸ��� ���� ��Ʋ������ �ȿ� ����
	AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_FRIEND,		// �������� ���ݺҰ�
	AGPMPVP_SYSTEM_CODE_CANNOT_ATTACK_OTHER,		// ������������ ���ݺҰ�

	AGPMPVP_SYSTEM_CODE_SKILL_CANNOT_APPLY_EFFECT,	// �̹� �� ���� ������ �ɷ� ����. �ӽ� ���� -0-;; 2005.01.31
	AGPMPVP_SYSTEM_CODE_ITEM_ANTI_DROP,				// ��ȣ�η� ������ ����� ������.
	AGPMPVP_SYSTEM_CODE_CHARISMA_UP,				// ī������ ����Ʈ�� �ö���.
	AGPMPVP_SYSTEM_CODE_CANNOT_PVP_BY_LEVEL,		// % ���� ���ϴ� PvP �Ұ�.

	AGPMPVP_SYSTEM_CODE_DISABLE_NORMAL_ATTACK,		// �Ϲ� ������ �Ұ����� ����.
	AGPMPVP_SYSTEM_CODE_DISABLE_SKILL_CAST,			// ��ų ĳ������ �Ұ����� ����.
} eAgpmPvPSystemMessageCode;

// 2005.07.28. steeple
typedef enum _eAgpmPvPResurrectionType
{
	AGPMPVP_RESURRECTION_TYPE_TOWN_REBIRTH = 0,
	AGPMPVP_RESURRECTION_TYPE_NOW_POSITION,
	AGPMPVP_RESURRECTION_TYPE_REBIRTH_ITEM_USE,
} eAgpmPvPResurrectionType;

typedef enum _eAgpmPvPTargetType
{
	AGPMPVP_TARGET_TYPE_MOB = 0,
	AGPMPVP_TARGET_TYPE_NORMAL_PC,
	AGPMPVP_TARGET_TYPE_ENEMY_GUILD,
} eAgpmPvPTargetType;

const enum eAGPMPVP_RACE_BATTLE_STATUS
	{
	AGPMPVP_RACE_BATTLE_STATUS_NONE = 0,
	AGPMPVP_RACE_BATTLE_STATUS_READY,
	AGPMPVP_RACE_BATTLE_STATUS_ING,
	AGPMPVP_RACE_BATTLE_STATUS_ENDED,		// Ŭ���̾�Ʈ�� �˷��ֱ� ���� �����̴�. ���� ���������� �̷� ���¸� ���� �ʴ´�.
	AGPMPVP_RACE_BATTLE_STATUS_REWARD,
	AGPMPVP_RACE_BATTLE_STATUS_REWARD_END,
	AGPMPVP_RACE_BATTLE_STATUS_MAX
	};

class AgpdCharacter;
class AgpdSkill;
class AgpdSkillTemplate;

class AgpmPvP : public ApModule
{
private:
	class ApmMap*				m_papmMap;
	class AgpmCharacter*		m_pagpmCharacter;
	class AgpmFactors*			m_pagpmFactors;
	class AgpmGuild*			m_pagpmGuild;
	class AgpmSkill*			m_pagpmSkill;
	class AgpmSummons*			m_pagpmSummons;
	class AgpmParty*			m_pagpmParty;
	class AgpmBattleGround*		m_pagpmBattleGround;
	class AgpmConfig*			m_pagpmConfig;

	MTRand m_csRandom;

	INT16 m_nIndexADCharacter;

	ApAdmin m_csAreaDropAdmin;
	ApAdmin m_csItemDropAdmin;
	ApAdmin m_csSkullDropAdmin;

	INT32 m_lSkullDropMinLevelGap;
	INT32 m_lSkullDropMaxLevelGap;

	INT32 m_alSkullDropLevelBranch[AGPMPVP_MAX_SKULL_LEVEL_BRANCH];
	INT32 m_alSkullDropLevel[AGPMPVP_MAX_SKULL_LEVEL_BRANCH];

	INT32 m_lMinPvPLevel;
	
	UINT32	m_ulLastClock;
	
	eAGPMPVP_RACE_BATTLE_STATUS	m_eRaceBattleStatus;
	INT32	m_lRaceBattleWinner;

public:
	ApSafeArray<INT32, AURACE_TYPE_MAX>		m_lRaceBattlePoint;
	
	AuPacket m_csPacket;

	//ApMemoryPool	m_csMemoryPoolAdmin;

	//ApMemoryPool	m_csMemoryPoolCharInfo;
	//ApMemoryPool	m_csMemoryPoolGuildInfo;

public:
	AgpmPvP();
	virtual ~AgpmPvP();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle(UINT32 ulClockCount);
	BOOL OnDestroy();

	// Setting Minimum PVP Level
	VOID SetMinPvPLevel(INT32 lMinPvPLevel) {m_lMinPvPLevel = lMinPvPLevel;}

	// Attached Data
	AgpdPvPADChar* GetADCharacter(AgpdCharacter* pData);
	static BOOL ConAgpdPvPADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgpdPvPADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// Attach Data
	INT16 AttachPvPData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	// Character
	BOOL InitCharacter(AgpdCharacter* pcsCharacter);
	BOOL RemoveCharacter(AgpdCharacter* pcsCharacter);
	
	// OnReceive
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnReceivePvPInfo(INT32 lCID, INT32 lTargetCID, INT32 lWin, INT32 lLose, INT8 cPvPMode, INT8 cPvPStatus, INT8 cWin);
	BOOL OnReceiveAddFriend(INT32 lCID, INT32 lTargetCID);
	BOOL OnReceiveAddEnemy(INT32 lCID, INT32 lTargetCID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL OnReceiveRemoveFriend(INT32 lCID, INT32 lTargetCID);
	BOOL OnReceiveRemoveEnemy(INT32 lCID, INT32 lTargetCID);
	BOOL OnReceiveInitFriend(INT32 lCID);
	BOOL OnReceiveInitEnemy(INT32 lCID);
	BOOL OnReceiveUpdateFriend(INT32 lCID, INT32 lTargetCID);
	BOOL OnReceiveUpdateEnemy(INT32 lCID, INT32 lTargetCID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL OnReceiveAddFriendGuild(INT32 lCID, CHAR* szGuildID);
	BOOL OnReceiveAddEnemyGuild(INT32 lCID, CHAR* szGuildID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL OnReceiveRemoveFriendGuild(INT32 lCID, CHAR* szGuildID);
	BOOL OnReceiveRemoveEnemyGuild(INT32 lCID, CHAR* szGuildID);
	BOOL OnReceiveInitFriendGuild(INT32 lCID);
	BOOL OnReceiveInitEnemyGuild(INT32 lCID);
	BOOL OnReceiveUpdateFriendGuild(INT32 lCID, CHAR* szGuildID);
	BOOL OnReceiveUpdateEnemyGuild(INT32 lCID, CHAR* szGuildID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL OnReceiveCannotUseTeleport(INT32 lCID);
	BOOL OnReceiveSystemMessage(INT32 lCode, CHAR* szData1, CHAR* szData2, INT32 lData1, INT32 lData2);
	BOOL OnReceiveRequestDeadType(INT32 lCID, INT32 lResurrectionType);
	BOOL OnReceiveResponseDeadType(INT32 lCID, INT32 lTargetCID, INT32 lDropExpRate, INT32 lTargetType);
	BOOL OnReceiveRaceBattle(INT8 cStatus, INT32 lRemainTime, INT32 lHumanPoint, INT32 lOrcPoint, INT32 lMoonelfPoint);
	
	// Friend List
	BOOL InitFriendList(AgpdCharacter* pcsCharacter);
	BOOL AddFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend);
	BOOL AddFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID);
	BOOL RemoveFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend);
	BOOL RemoveFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID);
	inline BOOL UpdateFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID);
	inline BOOL IsFriend(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend);
	inline BOOL IsFriend(AgpdCharacter* pcsCharacter, INT32 lFriendCID);
	inline AgpdPvPCharInfo* GetFriendInfo(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsFriend);
	inline AgpdPvPCharInfo* GetFriendInfo(AgpdCharacter* pcsCharacter, INT32 lFriendCID);

	// Enemy List
	BOOL InitEnemyList(AgpdCharacter* pcsCharacter);
	BOOL AddEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL AddEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL RemoveEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy);
	BOOL RemoveEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID);
	inline BOOL UpdateEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL IsEnemy(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy);
	BOOL IsEnemy(AgpdCharacter* pcsCharacter, INT32 lEnemyCID);
	inline AgpdPvPCharInfo* GetEnemyInfo(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsEnemy);
	inline AgpdPvPCharInfo* GetEnemyInfo(AgpdCharacter* pcsCharacter, INT32 lEnemyCID);

	// Friend Guild List
	BOOL InitFriendGuildList(AgpdCharacter* pcsCharacter);
	BOOL AddFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	BOOL RemoveFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	inline BOOL UpdateFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	inline BOOL IsFriendGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	inline AgpdPvPGuildInfo* GetFriendGuildInfo(AgpdCharacter* pcsCharacter, CHAR* szGuildID);

	// Enemy Guild List
	BOOL InitEnemyGuildList(AgpdCharacter* pcsCharacter);
	BOOL AddEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	BOOL RemoveEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	inline BOOL UpdateEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID, INT8 cPvPMode = -1, UINT32 ulLastCombatClock = 0);
	inline BOOL IsEnemyGuild(AgpdCharacter* pcsCharacter, CHAR* szGuildID);
	inline AgpdPvPGuildInfo* GetEnemyGuildInfo(AgpdCharacter* pcsCharacter, CHAR* szGuildID);

	// Drop Probability
	INT32 GetItemDropProbability(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);	// Ȯ�� ����ؼ� ������ Position ����.
	INT32 GetSkullDropLevel(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);			// Ȯ�� ����ؼ� Skull Level ����.
	AgpdPvPAreaDrop* GetAreaDrop(eAgpdPvPMode ePvPMode);
	AgpdPvPItemDrop* GetItemDrop(INT32 lIndex);
	AgpdPvPSkullDrop* GetSkullDrop(INT32 lLevelGap);

	// Mode (Area)
	BOOL RegionChange(AgpdCharacter* pcsCharacter, INT16 nPrevRegionIndex);
	inline BOOL IsSafePvPMode(AgpdCharacter* pcsCharacter);
	inline BOOL IsFreePvPMode(AgpdCharacter* pcsCharacter);
	inline BOOL IsCombatPvPMode(AgpdCharacter* pcsCharacter);
	inline eAgpdPvPMode GetPvPModeByPosition(FLOAT fX, FLOAT fZ);
	inline eAgpdPvPMode GetPvPModeByRegion(INT16 nRegionIndex);

	// Status
	BOOL IsNowPvP(AgpdCharacter* pcsCharacter);
	inline BOOL IsInvincible(AgpdCharacter* pcsCharacter);
	BOOL UpdatePvPStatus(AgpdCharacter* pcsCharacter);
	BOOL SetDeadInCombatArea(AgpdCharacter* pcsCharacter, BOOL bDead);
	
	// Race War Status
	eAGPMPVP_RACE_BATTLE_STATUS GetRaceBattleStatus(); 
	void	SetRaceBattleStatus(eAGPMPVP_RACE_BATTLE_STATUS eStatus);
	BOOL	IsInRaceBattle();
	BOOL	IsInRaceBattleReward();
	void	SetRaceBattleResult();
	INT32	GetRaceBattleWinner();

	// Common
	inline BOOL IsPC(AgpdCharacter* pcsCharacter);
	inline BOOL IsFriendCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget);
	inline BOOL IsEnemyCharacter(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bServer = TRUE);
	BOOL IsAttackable(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce);
	BOOL IsSkillEnable(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget, AgpdCharacter* pcsSummonsOwner = NULL);
	inline BOOL IsAttackSkill(AgpdSkill* pcsSkill);
	inline BOOL IsAttackSkill(AgpdSkillTemplate* pcsSkillTemplate);
	inline BOOL IsAreaSkill(AgpdSkill* pcsSkill);

	// Common For Summons
	BOOL IsAttackableSummons(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTarget, BOOL bForce);
	BOOL IsSkillEnableSummons(AgpdSkill* pcsSkill, AgpdCharacter* pcsTarget);

	// System Message
	BOOL ProcessSystemMessage(INT32 lCode, CHAR* szData1 = NULL, CHAR* szData2 = NULL, INT32 lData1 = 0, INT32 lData2 = 0, AgpdCharacter* pcsCharacter = NULL);

	// Packet Make
	PVOID MakePvPPacket(INT16* pnPacketLength, INT8 cOperation, INT32* plCID, INT32* plTargetCID, INT32* plWin, INT32* plLose,
						INT8* pcPvPMode, INT8* pcPvPStatus, INT8* pcWin, CHAR* szGuildID, CHAR* szMessage);
	PVOID MakePvPInfoPacket(INT16* pnPacketLength, INT32* plCID, INT32* plWin, INT32* plLose, INT8* pcPvPMode, INT8* pcPvPStatus);
	PVOID MakePvPResultPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID, INT32* plWin, INT32* plLose,
						INT8* pcPvPMode, INT8* pcPvPStatus, INT8* pcWin);
	PVOID MakeAddFriendPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID);
	PVOID MakeAddEnemyPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID, INT8* pcPvPMode);
	PVOID MakeRemoveFriendPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID);
	PVOID MakeRemoveEnemyPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID);
	PVOID MakeInitFriendPacket(INT16* pnPacketLength, INT32* plCID);
	PVOID MakeInitEnemyPacket(INT16* pnPacketLength, INT32* plCID);
	PVOID MakeUpdateFriendPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID);
	PVOID MakeUpdateEnemyPacket(INT16* pnPacketLength, INT32* plCID, INT32* plTargetCID, INT8* pcPvPMode);
	PVOID MakeAddFriendGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID);
	PVOID MakeAddEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT8* pcPvPMode);
	PVOID MakeRemoveFriendGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID);
	PVOID MakeRemoveEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID);
	PVOID MakeInitFriendGuildPacket(INT16* pnPacketLength, INT32* plCID);
	PVOID MakeInitEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID);
	PVOID MakeUpdateFriendGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID);
	PVOID MakeUpdateEnemyGuildPacket(INT16* pnPacketLength, INT32* plCID, CHAR* szGuildID, INT8* pcPvPMode);
	PVOID MakeCannotUseTeleportPacket(INT16 *pnPacketLength, INT32 lCID);
	PVOID MakeSystemMessagePacket(INT16* pnPacketLength, INT32* plCode, CHAR* szData1, CHAR* szData2, INT32* plData1, INT32* plData2);
	PVOID MakeRequestDeadTypePacket(INT16* pnPacketLength, INT32 lCID, INT32 lResurrectionType);
	PVOID MakeResponseDeadTypePacket(INT16* pnPacketLength, INT32 lCID, INT32 lTargetCID, INT32 lDropExpRate, INT32 lTargetType);
	PVOID MakeRaceBattlePacket(INT16* pnPacketLength, INT8 cStatus, INT32 lRemained, INT32 lHumanPoint, INT32 lOrcPoint, INT32 lMoonelfPoint);

	// Set Max Admin
	BOOL SetMaxAreaDropAdmin(INT32 lCount);
	BOOL SetMaxItemDropAdmin(INT32 lCount);
	BOOL SetMaxSkullDropAdmin(INT32 lCount);

	// Read Excel InI
	BOOL ReadItemDropTxt(CHAR* szFile, BOOL bDecryption);
	BOOL ReadSkullDropTxt(CHAR* szFile, BOOL bDecryption);

	// Callback
	// From AgpmCharacter
	static BOOL CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBIsAttackable(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgpmSkill
	static BOOL CBIsSkillEnable(PVOID pData, PVOID pClass, PVOID pCustData);

	// Callback Registration
	BOOL SetCallbackPvPInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPvPResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInitFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInitEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateFriend(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateEnemy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInitFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInitEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateFriendGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateEnemyGuild(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackRecvCannotUseTeleport(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackEnterSafePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEnterFreePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEnterCombatPvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackLeaveSafePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackLeaveFreePvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackLeaveCombatPvPArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackSystemMessage(ApModuleDefaultCallBack, PVOID pClass);

	BOOL SetCallbackRequestDeadType(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackResponseDeadType(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackIsFriendSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackIsEnemySiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackIsInSiegeWarIngArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackCheckNPCAttackableTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackIsAttackableTime(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRaceBattleStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif //_AGPMPVP_H_