// AgpmConfig.h
// (C) NHN Games - ArchLord Development Team
// kelovon, 20051006

#ifndef _AGPM_CONFIG_H_
#define _AGPM_CONFIG_H_

#include <AuPacket/AuPacket.h>
#include <ApModule/ApModule.h>

typedef	ApString<64>	ApDirString;
typedef ApString<32>	ApEncString;

// ����: AgsdServer2.h�� AGSMSERVER_EXTRA_KEY_XXX�� ��ġ�ؾ� �Ѵ�.
#define AGPM_CONFIG_DEFAULT_VALUE_EXP_ADJUSTMENT_RATIO		1.0f
#define AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO	1.0f
#define AGPM_CONFIG_DEFAULT_VALUE_CHARISMA_ADJUST_RATIO		1.0f
#define AGPM_CONFIG_DEFAULT_VALUE_IS_ADULT_SERVER			FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_ADM_CMD_TO_ALL			FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_IS_NEW_SERVER				FALSE			// �ż��� ����
#define AGPM_CONFIG_DEFAULT_VALUE_PC_DROP_ITEM				FALSE			// PC�� �׾��� �� Item Drop ����
#define AGPM_CONFIG_DEFAULT_VALUE_INI_DIR					"Ini"			// INI ���丮 ��
#define	AGPM_CONFIG_VALUE_NO_ACCOUNT_AUTH					_T('n')
#define	AGPM_CONFIG_VALUE_WEB_ACCOUNT_AUTH					_T('w')
#define	AGPM_CONFIG_VALUE_DB_ACCOUNT_AUTH					_T('d')
#define	AGPM_CONFIG_VALUE_IGNORE_ACCOUNT_AUTH				_T('i')
#define	AGPM_CONFIG_DEFAULT_VALUE_ACCOUNT_AUTH				AGPM_CONFIG_VALUE_WEB_ACCOUNT_AUTH
#define	AGPM_CONFIG_DEFAULT_VALUE_IGNORE_LOG_FAIL			FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_FILE_LOG					FALSE
#define AGPM_CONFIG_DEFAULT_VALUE_EXP_PENALTY				FALSE//TRUE		//JK_������� ���� �⺻�� ����	// PC�� �׾��� �� EXP Down ����
#define AGPM_CONFIG_DEFAULT_VALUE_START_LEVEL				1				// �ɸ��� ������ ����
#define AGPM_CONFIG_DEFAULT_VALUE_START_GHELD				0				// �ɸ��� ������ ���� �ֵ�
#define AGPM_CONFIG_DEFAULT_VALUE_START_CHARISMA			0			
#define AGPM_CONFIG_DEFAULT_VALUE_ENABLE_AUCTION			TRUE			// Aution ���ɿ���
#define AGPM_CONFIG_DEFAULT_VALUE_ENABLE_EVENT_ITEM_EFFECT	TRUE			// EventItemEffect ȿ�� ���� 
#define AGPM_CONFIG_DEFAULT_VALUE_ENABLE_PVP				TRUE			// PVP ���ɿ���
#define AGPM_CONFIG_DEFAULT_VALUE_ENABLE_QUEST				TRUE			// QUEST ���ɿ���


// ���� �̺�Ʈ ���� ���� Flag��
#define	AGPM_CONFIG_DEFAULT_VALUE_EVENT_CHATTING			FALSE			// 330���� ä��â �̺�Ʈ
#define	AGPM_CONFIG_DEFAULT_VALUE_EVENT_RESURRECT			FALSE			// ��� �� ��Ȱ �г�Ƽ ���� ���ϴ� �̺�Ʈ
#define	AGPM_CONFIG_DEFAULT_VALUE_EVENT_ITEMDROP			FALSE

typedef enum _AgpmConfigServerStatusFlag {
	AGPM_CONFIG_FLAG_NORMAL						= 0x00000000,
	AGPM_CONFIG_FLAG_SIEGE_WAR					= 0x00000001,
	AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM			= 0x00000002,
	AGPM_CONFIG_FLAG_CHARACTER_JUMPING_EVENT	= 0x00000004,
} AgpmConfigServerStatusFlag;

struct AgpdTPack
{
	FLOAT		fTPackExpRatio;
	FLOAT		fTPackDropRatio;
	FLOAT		fTPackGheldRatio;
	FLOAT		fTPackConvertRatio;
	FLOAT		fTPackCharismaRatio;

public:
	AgpdTPack()
		: fTPackExpRatio(0.0f), fTPackDropRatio(0.0f), fTPackGheldRatio(0.0f), fTPackConvertRatio(0.0f), fTPackCharismaRatio(0.0f)
	{

	}

	~AgpdTPack()
	{
		Init();
	}

	void		Init()
	{
		fTPackExpRatio		= 0.0f;
		fTPackDropRatio		= 0.0f;
		fTPackGheldRatio	= 0.0f;
		fTPackConvertRatio	= 0.0f;
		fTPackCharismaRatio	= 0.0f;
	}
};

struct AgpdConfig
{
	INT32		lEventNumber;					//�����̷��� �̺�Ʈ ����� �̺�Ʈ ���� �Ƶ�.
	float		fExpAdjustmentRatio;
	float		fDropAdjustmentRatio;
	float		fDrop2AdjustmentRatio;
	float		fGheldDropAdjustmentRatio;
	float		fCharismaDropAdjustmentRatio;
	BOOL		bIsAdultServer;
	BOOL		bIsAllAdmin;					// ��� ����ڰ� admin command ��� ����, ���߿����θ� ���
	BOOL		bIsNewServer;
	BOOL		bIsEventServer;
	BOOL		bIsAimEventServer;
	BOOL		bPCDropItemOnDeath;
	ApDirString	szIniDir;
	TCHAR		cAccountAuth;
	BOOL		bIgnoreLogFail;
	BOOL		bFileLog;
	INT32		lMaxUserCount;
	BOOL		bExpPenaltyOnDeath;

	BOOL		bIsEventChatting;
	BOOL		bIsEventResurrect;
	BOOL		bIsEventItemDrop;

	BOOL		bIsTestServer;

	BOOL		bIsEnableAuction;
	BOOL		bIsEnableQuest;

	BOOL		bIsEventItemEffect;
	BOOL		bIsAblePvP;

	INT32		lStartLevel;
	INT64		llStartGheld;
	INT32		lStartCharisma;

	INT32		lJumpingStartLevel;
	INT64		llJumpingStartGheld;
	INT32		lJumpingStartCharisma;

	// ���� ���� ���� �÷��� (�������� �̺�Ʈ�����,,,)
	UINT32		ulServerStatus;

	ApEncString szEncPublic;					// 2006.06.14. steeple. ����ϴ� �˰���.
	ApEncString szEncPrivate;					// 2006.06.14. steeple. ����ϴ� �˰���.

	AgpdConfig()
	{
		Reset();
	}

	void Reset()
	{
		lEventNumber				= 0;
		fExpAdjustmentRatio			= AGPM_CONFIG_DEFAULT_VALUE_EXP_ADJUSTMENT_RATIO;
		fDropAdjustmentRatio		= AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO;
		fDrop2AdjustmentRatio		= AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO;
		fGheldDropAdjustmentRatio	= AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO;
		fCharismaDropAdjustmentRatio= AGPM_CONFIG_DEFAULT_VALUE_CHARISMA_ADJUST_RATIO;
		bIsAdultServer				= AGPM_CONFIG_DEFAULT_VALUE_IS_ADULT_SERVER;
		bIsAllAdmin					= AGPM_CONFIG_DEFAULT_VALUE_ADM_CMD_TO_ALL;
		bIsNewServer				= AGPM_CONFIG_DEFAULT_VALUE_IS_NEW_SERVER;
		bPCDropItemOnDeath			= AGPM_CONFIG_DEFAULT_VALUE_PC_DROP_ITEM;
		szIniDir					= AGPM_CONFIG_DEFAULT_VALUE_INI_DIR;
		cAccountAuth				= AGPM_CONFIG_DEFAULT_VALUE_ACCOUNT_AUTH;
		bIgnoreLogFail				= AGPM_CONFIG_DEFAULT_VALUE_IGNORE_LOG_FAIL;
		bFileLog					= AGPM_CONFIG_DEFAULT_VALUE_FILE_LOG;
		lMaxUserCount				= 0;
		bExpPenaltyOnDeath			= AGPM_CONFIG_DEFAULT_VALUE_EXP_PENALTY;

		bIsEventChatting			= AGPM_CONFIG_DEFAULT_VALUE_EVENT_CHATTING;
		bIsEventResurrect			= AGPM_CONFIG_DEFAULT_VALUE_EVENT_RESURRECT;
		bIsEventItemDrop			= AGPM_CONFIG_DEFAULT_VALUE_EVENT_ITEMDROP;

		ulServerStatus				= AGPM_CONFIG_FLAG_NORMAL;

		bIsTestServer				= FALSE;
		bIsEventServer				= FALSE;
		bIsAimEventServer			= FALSE;
		bIsEnableAuction			= TRUE;
		bIsEventItemEffect			= TRUE;
		bIsAblePvP					= TRUE;

		lStartLevel					= AGPM_CONFIG_DEFAULT_VALUE_START_LEVEL;
		llStartGheld				= AGPM_CONFIG_DEFAULT_VALUE_START_GHELD;
		lStartCharisma				= AGPM_CONFIG_DEFAULT_VALUE_START_CHARISMA;

		lJumpingStartLevel			= AGPM_CONFIG_DEFAULT_VALUE_START_LEVEL;
		llJumpingStartGheld			= AGPM_CONFIG_DEFAULT_VALUE_START_GHELD;
		lJumpingStartCharisma		= AGPM_CONFIG_DEFAULT_VALUE_START_CHARISMA;

		bIsEnableQuest				= AGPM_CONFIG_DEFAULT_VALUE_ENABLE_QUEST;


		szEncPublic.Clear();
		szEncPrivate.Clear();
	};
};

class AgpmConfig : public ApModule
{
public:
	AgpmConfig();
	virtual ~AgpmConfig();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnIdle2(UINT32 ulClockCount);
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL OnDestroy();

	void ResetConfig() { m_stAgpdConfig.Reset(); }

	// �̺�Ʈ ��ȣ
	void SetEventNumber(INT32 lEventNumber = 0);
	INT32 GetEventNumber();

	// ����ġ ������
	void SetExpAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_EXP_ADJUSTMENT_RATIO);
	float GetExpAdjustmentRatio(BOOL bTPackUser = FALSE);

	// ����� ������
	void SetDropAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO);
	float GetDropAdjustmentRatio(BOOL bTPackUser = FALSE);

	// �����2 ������
	void SetDrop2AdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO);
	float GetDrop2AdjustmentRatio(BOOL bTPackUser = FALSE);

	// �ֵ� ��� ����
	void SetGheldDropAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_DROP_RATE_ADJUST_RATIO);
	float GetGheldDropAdjustmentRatio(BOOL bTPackUser = FALSE);

	// ī������ ��� ����
	void SetCharismaDropAdjustmentRatio(float fRatio = AGPM_CONFIG_DEFAULT_VALUE_CHARISMA_ADJUST_RATIO);
	float GetCharismaDropAdjustmentRatio(BOOL bTPackUser = FALSE);

	// ���� ���� ����
	void SetAdultServer(BOOL bIsAdultServer = AGPM_CONFIG_DEFAULT_VALUE_IS_ADULT_SERVER);
	BOOL IsAdultServer();

	// ��� ����ڰ� admin command ��� �������� ����, ���߿����θ� ���
	void SetAllAdmin(BOOL bIsAllAdmin = AGPM_CONFIG_DEFAULT_VALUE_ADM_CMD_TO_ALL);
	BOOL IsAllAdmin();

	// �ż��� ����
	void SetNewServer(BOOL bIsNewServer = AGPM_CONFIG_DEFAULT_VALUE_IS_NEW_SERVER);
	BOOL IsNewServer();

	// PC�� �׾��� �� Item Drop ����
	void SetPCDropItemOnDeath(BOOL bPCDropItemOnDeath = AGPM_CONFIG_DEFAULT_VALUE_PC_DROP_ITEM);
	BOOL DoesPCDropItemOnDeath();

	// PC�� �׾��� �� ����ġ Penalty ����
	void SetExpPenaltyOnDeath(BOOL bExpPenaltyOnDeath = AGPM_CONFIG_DEFAULT_VALUE_EXP_PENALTY);
	BOOL DoesExpPenaltyOnDeath();

	// INI ���丮 ��
	void SetIniDir(LPCTSTR szIniDir = AGPM_CONFIG_DEFAULT_VALUE_INI_DIR);
	LPCTSTR GetIniDir();

	// Auction ���ɿ���
	void SetEnableAuction(BOOL bEnableAuction = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_AUCTION);
	BOOL IsEnableAuction();

	// Quest ���ɿ���
	void SetEnableQuest(BOOL bEnableQuest = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_QUEST);
	BOOL IsEnableQuest();

	// EventItemEffect ȿ�� ����
	void SetEnableEventItemEffect(BOOL bEnableEventItemEffect = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_EVENT_ITEM_EFFECT);
	BOOL IsEnableEventItemEffect();

	// PVP ���ɿ���
	void SetEnablePvP(BOOL bEnalbePvP = AGPM_CONFIG_DEFAULT_VALUE_ENABLE_PVP);
	BOOL IsEnablePvP();

	void SetAccountAuth(CHAR cAccountAuth = AGPM_CONFIG_DEFAULT_VALUE_ACCOUNT_AUTH);
	BOOL IsNoAccountAuth();
	BOOL IsWebAccountAuth();
	BOOL IsDBAccountAuth();
	BOOL IsIgnoreAccountAuth();
	
	void SetIgnoreLogFail(BOOL bIgnore = AGPM_CONFIG_DEFAULT_VALUE_IGNORE_LOG_FAIL);
	BOOL IsIgnoreLogFail();

	void SetFileLog(BOOL bFile = AGPM_CONFIG_DEFAULT_VALUE_FILE_LOG);
	BOOL IsFileLog();	

	void SetEventChatting(BOOL bIsEventChatting = AGPM_CONFIG_DEFAULT_VALUE_EVENT_CHATTING);
	BOOL IsEventChatting();

	void SetEventResurrect(BOOL bIsEventResurrect = AGPM_CONFIG_DEFAULT_VALUE_EVENT_RESURRECT);
	BOOL IsEventResurrect();

	void SetEventItemDrop(BOOL bIsEventItemDrop = AGPM_CONFIG_DEFAULT_VALUE_EVENT_ITEMDROP);
	BOOL IsEventItemDrop();

	void SetServerStatusFlag(AgpmConfigServerStatusFlag eFlag);
	void ResetServerStatusFlag(AgpmConfigServerStatusFlag eFlag);
	BOOL IsSetServerStatusFlag(AgpmConfigServerStatusFlag eFlag);

	void SetMaxUserCount(INT32 lMaxUserCount = 0);
	INT32 GetMaxUserCount();

	void SendConfigPacket(UINT32 ulNID);

	void SetEncPublic(LPCSTR szEncAlgorithm) { if(szEncAlgorithm) m_stAgpdConfig.szEncPublic.SetText(szEncAlgorithm); }
	LPCTSTR GetEncPublic() { return (LPCTSTR)m_stAgpdConfig.szEncPublic; }
	void SetEncPrivate(LPCSTR szEncAlgorithm) { if(szEncAlgorithm) m_stAgpdConfig.szEncPrivate.SetText(szEncAlgorithm); }
	LPCTSTR GetEncPrivate() { return (LPCTSTR)m_stAgpdConfig.szEncPrivate; }

	void SetTestServer(BOOL bIsTestServer) { m_stAgpdConfig.bIsTestServer = bIsTestServer; }
	BOOL IsTestServer() { return m_stAgpdConfig.bIsTestServer; }

	void SetStartLevel(INT32 lStartLevel) { m_stAgpdConfig.lStartLevel = lStartLevel; }
	INT32 GetStartLevel() { return m_stAgpdConfig.lStartLevel; }

	void SetStartGheld(INT64 llStartGheld) { m_stAgpdConfig.llStartGheld = llStartGheld; }
	INT64 GetStartGheld() { return m_stAgpdConfig.llStartGheld; }

	void SetStartCharisma(INT32 lStartCharisma) { m_stAgpdConfig.lStartCharisma = lStartCharisma; }
	INT32 GetStartCharisma() { return m_stAgpdConfig.lStartCharisma; }

	void SetJumpingStartLevel(INT32 lJumpingStartLevel) { m_stAgpdConfig.lJumpingStartLevel = lJumpingStartLevel; }
	INT32 GetJumpingStartLevel() { return m_stAgpdConfig.lJumpingStartLevel; }

	void SetJumpingStartGheld(INT64 llJumpingStartGheld) { m_stAgpdConfig.llJumpingStartGheld = llJumpingStartGheld; }
	INT64 GetJumpingStartGheld() { return m_stAgpdConfig.llJumpingStartGheld; }

	void SetJumpingStartCharisma(INT32 lJumpingStartCharisma) { m_stAgpdConfig.lJumpingStartCharisma = lJumpingStartCharisma; }
	INT32 GetJumpingStartCharisma() { return m_stAgpdConfig.lJumpingStartCharisma; }

	BOOL IsEventServer();
	void SetEventServer(BOOL bIsEventServer);

	BOOL IsAimEventServer();
	void SetAimEventServer(BOOL bIsAimEventServer);

	void SetIsAdminEventItem(BOOL bSet)
	{
		if(bSet)
			m_stAgpdConfig.ulServerStatus |= AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM;
		else
			m_stAgpdConfig.ulServerStatus &= ~AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM;
	}
	BOOL GetIsAdminEventItem()
	{
		if(m_stAgpdConfig.ulServerStatus & AGPM_CONFIG_FLAG_ADMIN_EVENT_ITEM)
			return TRUE;
		else
			return FALSE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TPack ����

	VOID	InitTPack() { m_stAgpdTPack.Init(); }

	VOID	SetTPackExpRatio(FLOAT fExpRatio)			{ m_stAgpdTPack.fTPackExpRatio = fExpRatio; }
	VOID	SetTPackDropRatio(FLOAT fDropRatio)			{ m_stAgpdTPack.fTPackDropRatio = fDropRatio; }
	VOID	SetTPackGheldRatio(FLOAT fGheldRatio)		{ m_stAgpdTPack.fTPackGheldRatio = fGheldRatio; }
	VOID	SetTPackConvertRatio(FLOAT fConvertRatio)	{ m_stAgpdTPack.fTPackConvertRatio = fConvertRatio; }
	VOID	SetTPackCharismaRatio(FLOAT fCharismaRatio)	{ m_stAgpdTPack.fTPackCharismaRatio = fCharismaRatio; }

	FLOAT	GetTPackExpRatio()							{ return m_stAgpdTPack.fTPackExpRatio; }
	FLOAT	GetTPackDropRatio()							{ return m_stAgpdTPack.fTPackDropRatio; }
	FLOAT	GetTPackGheldRatio()						{ return m_stAgpdTPack.fTPackGheldRatio; }
	FLOAT	GetTPackConvertRatio()						{ return m_stAgpdTPack.fTPackConvertRatio; }
	FLOAT	GetTPackCharismaRatio()						{ return m_stAgpdTPack.fTPackCharismaRatio; }

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
	AgpdConfig	m_stAgpdConfig;
	AgpdTPack	m_stAgpdTPack;
	AuPacket	m_csPacket;
};

#endif // _AGPM_CONFIG_H_
