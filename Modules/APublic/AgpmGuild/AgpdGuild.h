// AgpdGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 06. 15.

#ifndef _AGPDGUILD_H_
#define _AGPDGUILD_H_

#include <ApBase/ApBase.h>
#include <ApAdmin/ApAdmin.h>
#include <AgpmItem/AgpdItemTemplate.h>

const INT32 AGPMGUILD_MAX_MAKE_GUILD_ID_LENGTH		= 16;		//	2005.09.06. By SungHoon ����� �ִ� �ִ� ũ��
const INT32 AGPMGUILD_MAX_GUILD_ID_LENGTH			= 32;		//	2005.08.03. By SungHoon ���� ������ ������ 16 -> 32 ����
const INT32 AGPMGUILD_MAX_DATE_LENGTH				= 32;
const INT32 AGPMGUILD_MAX_MEMBER_COUNT				= 500;		// 2005.06.09. �ø�!!!
const INT32 AGPMGUILD_MAX_PASSWORD_LENGTH			= 12;
const INT32 AGPMGUILD_MAX_NOTICE_LENGTH				= 89;
const INT32 AGPMGUILD_MAX_GUILDMARK_TEMPLATE_COUNT	= 200;		// 2005.10.10. By SungHoon

// ����� ���� - Default ���̴�. Server ���� �ǽð����� �ٲ��� �� ����.
const UINT32 AGPMGUILD_BATTLE_READY_TIME			= 30;		// �ʴ���. 5��
const UINT32 AGPMGUILD_BATTLE_DURATION_MIN			= 300;		// �ʴ���. 5��.
const UINT32 AGPMGUILD_BATTLE_DURATION_MAX			= 10800; //259200;	// �ʴ���. 3��. -> 180��
const UINT32 AGPMGUILD_BATTLE_CANCEL_ENABLE_TIME	= 60;		// �ʴ���. 1��
const INT32 AGPMGUILD_BATTLE_NEED_MEMBER_COUNT		= 10;		// ������ 10�� �̻��̾�� ��.
const INT32 AGPMGUILD_BATTLE_NEED_LEVEL_SUM			= 100;		// ������ �� ���� 100�� �Ѿ�� ��.
const INT32 AGPMGUILD_MAX_VISIBLE_GUILD_LIST		= 19;		// ���������� ������ Guild List ��

const UINT32 AGPMGUILD_LEAVE_NEED_TIME				= 60 * 60 * 24;		//	Ż�� ��� �ð� 24�ð�

typedef ApString<AGPMGUILD_MAX_GUILD_ID_LENGTH>		ApStrGuildName;

enum AgpmGuildMemberRank
{
	AGPMGUILD_MEMBER_RANK_NORMAL		= 1,			//	�Ϲ� ����
	AGPMGUILD_MEMBER_RANK_JOIN_REQUEST	= 2,			//	���� ��û��
	AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST	= 3,			//	Ż�� ��û��
	AGPMGUILD_MEMBER_RANK_MASTER		= 10,			//	��� ������
	AGPMGUILD_MEMBER_RANK_SUBMASTER		= 11,			//	��� �θ�����
};

enum AgpmGuildMemberStatus
{
	AGPMGUILD_MEMBER_STATUS_OFFLINE = 0,
	AGPMGUILD_MEMBER_STATUS_ONLINE,
};

enum AgpmGuildStatus
{
	AGPMGUILD_STATUS_NONE = 0,
	AGPMGUILD_STATUS_BATTLE_DECLARE,
	AGPMGUILD_STATUS_BATTLE_READY,
	AGPMGUILD_STATUS_BATTLE,
};

enum AgpmGuildBattleResult
{
	AGPMGUILD_BATTLE_RESULT_NONE = 0,
	AGPMGUILD_BATTLE_RESULT_WIN,
	AGPMGUILD_BATTLE_RESULT_DRAW,
	AGPMGUILD_BATTLE_RESULT_LOSE,
	AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW,
	AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW,
};

enum eGuildBattleType
{
	eGuildBattlePointMatch,		//���� ����Ȯ��
	eGuildBattlePKMatch,		//���� ���� PK
	eGuildBattleDeadMatch,		//�渶 ���� ���̱�
	eGuildBattleTotalSurvive,	//�������
	eGuildBattlePrivateSurvive,	//���̴���
	eGuildBattleMax,
};

enum eGuildBattleMode
{
	eGuildBattleTimeModeMax		= 6,
	eGuildBattleTotalModeMax	= 6,
	eGuildBattlePrivateModeMax	= 4,
};

extern INT32 g_nGuildBattleTime[eGuildBattleTimeModeMax];
extern INT32 g_nBattleTotalMan[eGuildBattleTotalModeMax];
extern INT32 g_nBattlePrivateMan[eGuildBattlePrivateModeMax];

enum AgpmGuildRank
{
	AGPMGUILD_RANK_BEGINNER = 1,
};

enum AgpmGuildRelation
{
	AGPMGUILD_RELATION_NONE = 0,
	AGPMGUILD_RELATION_JOINT,
	AGPMGUILD_RELATION_JOINT_LEADER,
	AGPMGUILD_RELATION_HOSTILE = 10,
};

enum _eBattleLoyalRanking
{
	WINNER_GUILD_NOTRANKED = 0,
	WINNER_GUILD_1STPLACE = 1,
	WINNER_GUILD_2NDPLACE,
	WINNER_GUILD_MAX
};

//////////////////////////////////////////////////////////////////////////
// GuildMember Class

//Battle�� ���������� �Ҽ��ִ��� ���������� �����߰�..
class AgpdGuildMemberBattleInfo
{
public:
	CHAR		m_szID[AGPACHARACTER_MAX_ID_STRING+1];

	BOOL		m_bBattle;		//Is Battle?
	UINT32		m_ulScore;
	UINT32		m_ulKill;
	UINT32		m_ulDeath;
	UINT32		m_ulSequence;	// ó������ ����������
};

class AgpdGuildMember : public AgpdGuildMemberBattleInfo
{
public:
	INT32		m_lRank;
	INT32		m_lJoinDate;	// TimeStamp

	INT32		m_lLevel;
	INT32		m_lTID;
	INT8		m_cStatus;		// Online, Offline.. ��Ÿ ���
};

//�����̹��� �ο������� ���µ�..
//eGuildBattleTotalSurvive -> 10, 20, 30, 40, 50, 100��
//eGuildBattlePrivateSurvive -> 3, 5, 7, 9

//////////////////////////////////////////////////////////////////////////
// Battle Info
class AgpdGuildBattle
{
public:
	eGuildBattleType	m_eType;
	UINT32				m_ulPerson;		//�����̹����� �����ο�����
	
	CHAR	m_szEnemyGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR	m_szEnemyGuildMasterID[AGPACHARACTER_MAX_ID_STRING+1];

	UINT32	m_ulAcceptTime;			// ���Ӽ������� ����Ѵ�.
	UINT32	m_ulReadyTime;			// ���Ӽ������� ����Ѵ�.
	UINT32	m_ulStartTime;			// ���Ӽ������� ����Ѵ�.
	UINT32	m_ulDuration;			// ���Ӽ������� ����Ѵ�.

	UINT32	m_ulCurrentTime;		// ���Ӽ������� ����Ѵ�.

	INT32	m_lMyScore;
	INT32	m_lEnemyScore;

	INT32	m_lMyUpScore;
	INT32	m_lEnemyUpScore;
	
	INT32	m_lMyUpPoint;
	INT32	m_lEnemyUpPoint;

	INT8	m_cResult;				// 0:None, 1:Win, 2:Draw, 3:Lose

	UINT32	m_ulRound;

	void	Init();
	void	Start( UINT32 lTime );
	void	End( UINT32 lTime );
	BOOL	Update( UINT32 lTime );

	INT32	GetGuildPoint();
};

//////////////////////////////////////////////////////////////////////////
// Related Guild
struct AgpdGuildRelationUnit
{
	CHAR	m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
	UINT32	m_ulDate;
	INT8	m_cRelation;
};

const INT32 AGPMGUILD_MAX_JOINT_GUILD				= 3;
const INT32 AGPMGUILD_MAX_HOSTILE_GUILD				= 5;
const INT32 AGPMGUILD_MIN_JOINT_MEMBER_COUNT		= 20;
const INT32 AGPMGUILD_MIN_HOSTILE_MEMBER_COUNT		= 10;

typedef vector<AgpdGuildRelationUnit>				JointVector;
typedef vector<AgpdGuildRelationUnit>::iterator		JointIter;
typedef vector<AgpdGuildRelationUnit>				HostileVector;
typedef vector<AgpdGuildRelationUnit>::iterator		HostileIter;

struct AgpdGuildRelation
{
	JointVector*	m_pJointVector;
	HostileVector*	m_pHostileVector;
	
	UINT32	m_ulLastJointLeaveDate;	// TimeStamp
	CHAR	m_szLastWaitGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
};

struct FindRelation
{
private:
	FindRelation()		{	}

public:
	FindRelation(const CHAR* szGuildID) : m_szGuildID(szGuildID) {;}

	bool operator() (const AgpdGuildRelationUnit& unit)
	{
		return m_szGuildID ? !_tcscmp(unit.m_szGuildID, m_szGuildID) : FALSE;
	}

private:
	const CHAR* m_szGuildID;
};

struct SortRelation
{
public:
	bool operator() (const AgpdGuildRelationUnit& lhs, const AgpdGuildRelationUnit& rhs)
	{
		if(lhs.m_cRelation > rhs.m_cRelation)
			return true;
		else if(lhs.m_cRelation == rhs.m_cRelation)
			return lhs.m_ulDate < rhs.m_ulDate;
		else
			return false;
	}
};

//////////////////////////////////////////////////////////////////////////
// Guild Class
class AgpdGuild : public ApBase
{
public:
	CHAR		m_szID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR		m_szMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR		m_szSubMasterID[AGPACHARACTER_MAX_ID_STRING+1];
	CHAR		m_szPassword[AGPMGUILD_MAX_PASSWORD_LENGTH+1];
	CHAR*		m_szNotice;

	INT32		m_lTID;
	INT32		m_lRank;
	INT32		m_lCreationDate;		// TimeStamp
	INT32		m_lMaxMemberCount;
	INT32		m_lUnionID;				// Union ID. Race �� ���� �ȴ�.
	INT32		m_lPage;
	INT32		m_lGuildMarkTID;
	INT32		m_lGuildMarkColor;

	UINT32		m_ulLastRefreshClock;	// 2005.06.16. steeple

	// �������� ����Ÿ
	UINT32		m_ulTotalGuildBattlePoint;
	UINT32		m_ulArchonScrollCount;

	BOOL		m_bRemove;
	INT32		m_lBRRanking;			// ��������� ����� �� ������� Ȯ�� (��Ʋ�ξⷩŷ)

	ApAdmin*	m_pMemberList;			// Member List
	ApAdmin*	m_pGuildJoinList;		// ���Խ�û�� ����

	AgpdGuildRelation m_csRelation;

	//����� ����
	INT8		m_cStatus;
	INT32		m_lWin, m_lDraw, m_lLose;
	INT32		m_lGuildPoint;

	AgpdGuildBattle	m_csCurrentBattleInfo;

public:
	AgpdGuildMember* GetMember( CHAR* szID );
	AgpdGuildMember* GetMaster();
	AgpdGuildMember* GetRoundMember( UINT32 ulRound );
	AgpdGuildMember* GetPreRoundMember()		{	return GetRoundMember( m_csCurrentBattleInfo.m_ulRound - 1 );	}
	AgpdGuildMember* GetCurRoundMember()		{	return GetRoundMember( m_csCurrentBattleInfo.m_ulRound );		}
	AgpdGuildMember* GetNextRoundMember()		{	return GetRoundMember( m_csCurrentBattleInfo.m_ulRound + 1 );	}
	UINT32	GetBattleMemberCount();
	AgpdGuildMemberBattleInfo* GetBattleMemberPack();

	UINT32	GetStateMemberCount( AgpmGuildMemberStatus eState = AGPMGUILD_MEMBER_STATUS_ONLINE );

	void	BattleStart( UINT32 lTime );
	void	BattleEnd( UINT32 lTime );
	void	BattleMemberInit();
	BOOL	BattleUpdate( UINT32 lTime );
	BOOL	BattleResult( AgpdGuild* pEnemy );

	void	SetBattle( CHAR* szID, BOOL bBattle );
	BOOL	IsBattle( CHAR* szID );
	BOOL	IsBattle( AgpdGuildMember* pMember );
	BOOL	IsOnBattle( CHAR* szID );
	BOOL	IsOnBattle( AgpdGuildMember* pMember );

	void	UpdateScore( CHAR* szID, UINT32 lScore );
	void	UpdateKill( CHAR* szID );
	void	UpdateDeath( CHAR* szID );

	UINT32	GetScoreAll();
	UINT32	GetKillAll();
	UINT32	GetDeathAll();

	BOOL	IsPrivateSurvivePlayer( AgpdGuild* pEnemyGuild, CHAR* szPlay1, CHAR* szPlay2 );


	// 2008. 9. 18. ��ũ ���� ���� �Լ� MK
	VOID	SetMasterID			( CHAR*	szMasterID		) { sprintf_s( m_szMasterID , AGPACHARACTER_MAX_ID_STRING , "%s" , szMasterID );		}
	VOID	SetSubMasterID		( CHAR* szSubMasterID	) { sprintf_s( m_szSubMasterID , AGPACHARACTER_MAX_ID_STRING , "%s" , szSubMasterID );	}
	VOID	SetMemberRankChange	( CHAR* szMemberID , INT32 nRank );

private:
	void	BattleResultSetting( AgpdGuild* pWinner, AgpdGuild* pLoser, BOOL bDraw = FALSE );
};

//////////////////////////////////////////////////////////////////////////
// Attach Character Data
struct AgpdGuildADChar
{
	CHAR				m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR				m_szRequestJoinGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	INT32				m_lGuildMarkTID;
	INT32				m_lGuildMarkColor;
	INT32				m_lBRRanking;
	AgpdGuildMember*	m_pMemberData;
};

//////////////////////////////////////////////////////////////////////////
// System Message
struct AgpdGuildSystemMessage
{
	INT32	m_lCode;
	CHAR*	m_aszData[2];
	INT32	m_alData[2];
} ;

//////////////////////////////////////////////////////////////////////////
// Guild Battle Point
struct AgpdGuildBattlePoint
{
	INT32	m_lLevelGap;
	INT32	m_lPoint;
};

class AgpdRequireItemIncreaseMaxMember// : public ApBase
{
public:
	INT32	m_lMaxMember;
	INT32	m_lGheld;
	CHAR	m_szSkullName[AGPMITEM_MAX_ITEM_NAME + 1];			// item name
	INT32	m_lSkullTID;
	INT32	m_lSkullCount;
};

class AgpdGuildRequestMember// : public ApAdmin
{
public:
	AgpdGuildRequestMember() : m_lLeaveRequestTime( 0 ), m_lLevel( 0 ), m_lTID( 0 ), m_cStatus( 0 )
	{
		memset( m_szGuildID, 0x00, AGPMGUILD_MAX_GUILD_ID_LENGTH+1 );
		memset( m_szMemberID, 0x00, AGPACHARACTER_MAX_ID_STRING+1 );
	};

	INT32	m_lLeaveRequestTime;
	INT32	m_lLevel;
	INT32	m_lTID;
	CHAR	m_szGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH+1];
	CHAR	m_szMemberID[AGPACHARACTER_MAX_ID_STRING+1];
	INT8	m_cStatus;
};

class AgpdGuildMarkTemplate// : public ApBase
{
public:
	AgpdGuildMarkTemplate() : m_szName(NULL), m_lTypeCode(0), m_lTID(0), m_lIndex(0), m_lGheld(0), 
								m_lSkullTID(0), m_lSkullCount(0), m_pcsGridItem(NULL)
	{
	}
	~AgpdGuildMarkTemplate()
	{
	}

public:
	CHAR*			m_szName;
	INT32			m_lTypeCode;
	INT32			m_lTID;
	INT32			m_lIndex;
	INT32			m_lGheld;
	INT32			m_lSkullTID;
	INT32			m_lSkullCount;
	AgpdGridItem*	m_pcsGridItem;
};

#endif //_AGPDGUILD_H_
