// AgpdPvP.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2004. 12. 16.

#ifndef _AGPDPVP_H_
#define _AGPDPVP_H_

#include "ApBase.h"
#include "AgpdCharacter.h"
#include "AgpdGuild.h"

#define AGPMPVP_MAX_FRIEND_COUNT			50				// �׳� �� ����� ����
#define AGPMPVP_MAX_ENEMY_COUNT				50				// �׳� �� ����� ����

typedef enum _eAgpdPvPMode
{
	AGPDPVP_MODE_NONE = 0,
	AGPDPVP_MODE_SAFE,
	AGPDPVP_MODE_FREE,
	AGPDPVP_MODE_COMBAT,
	AGPDPVP_MODE_BATTLE,			// ��� Character Attached Data �� ���� �ʰ�, �� PvPInfo ���� ����. 2005.04.17. steeple
} eAgpdPvPMode;

typedef enum _eAgpdPvPStatus
{
	AGPDPVP_STATUS_NONE = 0x00,
	AGPDPVP_STATUS_NOWPVP = 0x01,
	AGPDPVP_STATUS_INVINCIBLE =0x02,
} eAgpdPvPStatus;

class AgpmPvPArray {
private:
	ApSafeArray<INT32, AGPMPVP_MAX_FRIEND_COUNT>	m_csArrayID;
	CHAR	m_csArrayName[AGPMPVP_MAX_FRIEND_COUNT][AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];

	ApSafeArray<PVOID, AGPMPVP_MAX_FRIEND_COUNT>	m_csArrayData;

	INT32	lObjectCount;

public:
	AgpmPvPArray();
	virtual ~AgpmPvPArray();

	BOOL	Initialize();

	PVOID	AddObject(PVOID pObject, INT32 nKey);
	PVOID	AddObject(PVOID pObject, CHAR *szName);

	BOOL	RemoveObject(INT32 nKey);
	BOOL	RemoveObject(CHAR *szName);

	PVOID	GetObject(INT32 nKey);
	PVOID	GetObject(CHAR *szName);

	PVOID	GetObjectSequence(INT32* plIndex);

	INT32	GetObjectCount();
};

//////////////////////////////////////////////////////////////////////////
// Enemy Info
struct AgpdPvPCharInfo
{
	INT32 m_lCID;				// Target CID
	eAgpdPvPMode m_ePvPMode;	// Target ���� PvPMode
	UINT32 m_ulLastCombatClock;	// ���������� �ƹ��� ��븦 ���� �ð�

	bool operator == (INT32 lCID)
	{
		if(m_lCID == lCID)
			return true;

		return false;
	}

	AgpdPvPCharInfo() : m_lCID(0), m_ePvPMode(AGPDPVP_MODE_NONE), m_ulLastCombatClock(0) {;}
};

struct AgpdPvPGuildInfo
{
	CHAR m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	eAgpdPvPMode m_ePvPMode;
	UINT32 m_ulLastCombatClock;

	bool operator == (const CHAR* szGuildID)
	{
		if(!szGuildID)
			return false;

		return !_tcscmp(m_szGuildID, szGuildID);
	}

	AgpdPvPGuildInfo() : m_ePvPMode(AGPDPVP_MODE_NONE), m_ulLastCombatClock(0)
	{
		memset(m_szGuildID, 0, sizeof(m_szGuildID));
	}
};

typedef std::vector<AgpdPvPCharInfo>				PvPCharVector;
typedef std::vector<AgpdPvPCharInfo>::iterator		PvPCharIter;
typedef std::vector<AgpdPvPGuildInfo>				PvPGuildVector;
typedef std::vector<AgpdPvPGuildInfo>::iterator		PvPGuildIter;

//////////////////////////////////////////////////////////////////////////
// Attach Character Data
typedef struct _stAgpdPvPADChar
{
	BOOL m_bInit;

	INT32 m_lWin;
	INT32 m_lLose;

	eAgpdPvPMode m_ePvPMode;		// ���� � PvP ������� �����Ѵ�. �ַ� ������ ���� �¿�ȴ�.
	INT8 m_cPvPStatus;				// ���� PvP ���¸� �����Ѵ�. Bit ���� �ؾ��Ѵ�.

	BOOL m_bDeadInCombatArea;		// Combat Area �ȿ��� �׾���. ��� ��Ȱ�ϰ� ��. ��� �����ؾ��ϴ� ���

	/*
	ApAdmin* m_pFriendList;
	ApAdmin* m_pEnemyList;

	ApAdmin* m_pFriendGuildList;	// ���� ���� �����Ѵ�.
	ApAdmin* m_pEnemyGuildList;		// ���� ���� �����Ѵ�.
	*/

	//AgpmPvPArray*	m_pFriendList;
	//AgpmPvPArray*	m_pEnemyList;

	//AgpmPvPArray*	m_pFriendGuildList;
	//AgpmPvPArray*	m_pEnemyGuildList;

	PvPCharVector*	m_pFriendVector;
	PvPCharVector*	m_pEnemyVector;

	PvPGuildVector* m_pFriendGuildVector;
	PvPGuildVector* m_pEnemyGuildVector;

	INT8 m_cDeadType;
} AgpdPvPADChar;

//////////////////////////////////////////////////////////////////////////
// Item Drop Probabiliy
#define AGPDPVP_DROP_KIND_LENGTH		32

typedef struct _stAgpdPvPAreaDrop
{
	CHAR m_szKind[AGPDPVP_DROP_KIND_LENGTH+1];
	eAgpdPvPMode m_ePvPMode;
	INT16 m_nAreaProbability;
	INT16 m_nEquipProbability;
	INT16 m_nInvenProbability;
} AgpdPvPAreaDrop;

typedef struct _stAgpdPvPItemDrop
{
	CHAR m_szKind[AGPDPVP_DROP_KIND_LENGTH+1];
	INT32 m_lIndex;
	INT16 m_nEquipProbability;
	INT16 m_nInvenProbability;
} AgpdPvPItemDrop;

typedef struct _stAgpdPvPSkullDrop
{
	INT32 m_lLevelGap;
	INT32 m_lSkullLevel;
	INT32 m_lSkullProbability;
} AgpdPvPSkullDrop;

//////////////////////////////////////////////////////////////////////////
// System Message
typedef struct _stAgpdPvPSystemMessage
{
	INT32 m_lCode;
	CHAR* m_aszData[2];
	INT32 m_alData[2];
} AgpdPvPSystemMessage;

#endif //_AGPDPVP_H_
