/*============================================================================

	AgsdServer2.h

============================================================================*/

#ifndef _AGSD_SERVER2_H_
	#define _AGSD_SERVER2_H_

#include "ApBase.h"
#include "ApAdmin.h"
#include "AsDefine.h"
#include "ApMutualEx.h"
#include <time.h>


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGSMSERVER_MAX_CONTROL_AREA			20
#define AGSMSERVER_INVALID_NID				0
#define AGSMSERVER_MAX_DBNAME				32
#define AGSMSERVER_MAX_EXTRA_VALUE			512
#define AGSMSERVER_MAX_SUBWORLD				50
#define AGSMSERVER_EXTRA_KEY_DELIM			_T('=')
#define AGSMSERVER_EXTRA_VALUE_DELIM		_T(',')

#define AGSMSERVER_EXTRA_KEY_SUB_WORLD					_T("SWORLD")
#define AGSMSERVER_EXTRA_KEY_EXP_ADJUSTMENT_RATIO		_T("ExpAdjRatio")
#define AGSMSERVER_EXTRA_KEY_DROP_ADJUST_RATIO			_T("DropAdjRatio")
#define AGSMSERVER_EXTRA_KEY_GHELD_DROP_ADJUST_RATIO	_T("GheldAdjRatio")
#define AGSMSERVER_EXTRA_KEY_IS_ADULT_SERVER			_T("AdultServer")
#define AGSMSERVER_EXTRA_KEY_ADM_CMD_TO_ALL				_T("AllAdmin")			// ��� ����ڰ� admin command ��� ����, ���߿����θ� ���
#define AGSMSERVER_EXTRA_KEY_IS_NEW_SERVER				_T("NewServer")			// �ż��� ����
#define AGSMSERVER_EXTRA_KEY_IS_EVENT_SERVER			_T("EventServer")		// �̺�Ʈ ���� ���� ����
#define AGSMSERVER_EXTRA_KEY_PC_DROP_ITEM				_T("PCDropItemOnDeath")	// PC�� �׾��� �� Item Drop ����
#define AGSMSERVER_EXTRA_KEY_EXP_PENALTY				_T("ExpPenaltyOnDeath")	// PC�� �׾��� �� Item Drop ����
#define AGSMSERVER_EXTRA_KEY_INI_DIR					_T("IniDir")			// INI ���丮 ��
#define AGSMSERVER_EXTRA_KEY_ACCOUNT_AUTH				_T("AccountAuth")		// ���� ���� ���
#define AGSMSERVER_EXTRA_KEY_IGNORE_LOG_FAIL			_T("IgnoreLog")			// �α� ���� ���� ����
#define AGSMSERVER_EXTRA_KEY_FILE_LOG					_T("FileLog")			// ���� �α�
#define	AGSMSERVER_EXTRA_KEY_MAX_USER_COUNT				_T("MaxUserCount")
#define AGSMSERVER_EXTRA_KEY_ENC_PUBLIC					_T("EncPublic")
#define AGSMSERVER_EXTRA_KEY_ENC_PRIVATE				_T("EncPrivate")
#define	AGSMSERVER_EXTRA_KEY_TEST_SERVER				_T("TestServer")
#define	AGSMSERVER_EXTRA_KEY_START_LEVEL				_T("StartLevel")
#define	AGSMSERVER_EXTRA_KEY_START_GHELD				_T("StartGheld")
#define AGSMSERVER_EXTRA_KEY_ORDER						_T("Order")
#define AGSMSERVER_EXTRA_KEY_ENABLE_AUCTION				_T("EnableAuction")		// ��� ���ɿ��� 
#define AGSMSERVER_EXTRA_KEY_AIM_EVENT_SERVER			_T("AimEventServer")	// �̺�Ʈ ������ ����
#define AGSMSERVER_EXTRA_KEY_EVENT_CHATTING				_T("EventChatting")		// �̺�Ʈ ä�� ���� 
#define AGSMSERVER_EXTRA_KEY_LIMIT_LEVEL				_T("LimitLevel")		// �ִ뷹������ JK_Ư��ȭ����
#define AGSMSERVER_EXTRA_KEY_DROP2_ADJUST_RATIO			_T("Drop2AdjRatio")		// ��;����� ����� //JK_Ư��ȭ����
#define AGSMSERVER_EXTRA_KEY_MIDNIGHTSHUTDOWN_AGE		_T("ShutDownUnderAge")		// �ɾ߼��ٿ���س���..�̸�..//JK_�ɾ߼��ٿ�
	
typedef enum _eAGSMSERVER_TYPE
	{
	AGSMSERVER_TYPE_NONE					= 0,
	AGSMSERVER_TYPE_MASTERDB_SERVER			= 1,		// master(account) DB server
	AGSMSERVER_TYPE_LOGIN_SERVER			= 2,
	AGSMSERVER_TYPE_GAME_SERVER				= 8,
	AGSMSERVER_TYPE_RELAY_SERVER			= 4,
	AGSMSERVER_TYPE_LK_SERVER				= 10,		// korea only
	AGSMSERVER_TYPE_RECRUIT_SERVER			= 11,
	AGSMSERVER_TYPE_BILLING_SERVER			= 12,
	AGSMSERVER_TYPE_COMPENDB_SERVER			= 15,		// compensation DB server
	} eAGSMSERVER_TYPE;

/************************************************/
/*		The Definition of ControlArea class		*/
/************************************************/
//
class AgsdServer2;

class AgsdControlArea
	{
	public:
		AuAREA		m_stControlArea;
		INT16		m_nNumAroundUnit;
		ApSafeArray<AgsdServer2 *, 7>	m_pcsAroundUnit;
	};


/********************************************/
/*		The Definition of World class		*/
/********************************************/
//
class AgsdServerWorld
	{
	public:
		CHAR	m_szName[AGSM_MAX_SERVER_NAME+1];
		BOOL	m_bShow;
	
	public:
		AgsdServerWorld()
			{
			ZeroMemory(m_szName, sizeof(m_szName));
			m_bShow = TRUE;
			}

		~AgsdServerWorld()
			{
			}
	};


/************************************************/
/*		The Definition of AgsdServer2 calss		*/
/************************************************/
//
class AgsdServer2
	{
	public:
		ApMutualEx		m_Mutex;
		
		//	basic
		INT32			m_lServerID;						// ID
		CHAR			m_szIP[23];							// IP address
		CHAR			m_szPrivateIP[23];					// Private IP address
		UINT8			m_cType;							// eAGSMSERVER_TYPE
		CHAR			m_szWorld[AGSM_MAX_SERVER_NAME+1];	// Main World
		CHAR			m_szGroupName[AGSM_MAX_SERVER_NAME+1];	// Group Name
		BOOL			m_bEnable;
		
		//	Sub-world
		ApSafeArray<AgsdServerWorld, AGSMSERVER_MAX_SUBWORLD>		m_SubWorld;
		INT32								m_nSubWorld;

		//	database
		CHAR			m_szDBUser[AGSMSERVER_MAX_DBNAME+1];
		CHAR			m_szDBPwd[AGSMSERVER_MAX_DBNAME+1];
		CHAR			m_szDBDSN[AGSMSERVER_MAX_DBNAME+1];
		CHAR			m_szLogDBUser[AGSMSERVER_MAX_DBNAME+1];
		CHAR			m_szLogDBPwd[AGSMSERVER_MAX_DBNAME+1];
		CHAR			m_szLogDBDSN[AGSMSERVER_MAX_DBNAME+1];
		CHAR			m_szDBVender[AGSMSERVER_MAX_DBNAME+1];

		//	network
		UINT32			m_dpnidServer;						// DPNID (as server)
		BOOL			m_bIsConnected;						// 

		BOOL			m_bIsReplyConnect;

		//	control area
		INT16			m_nControlArea;
		ApSafeArray<AgsdControlArea, AGSMSERVER_MAX_CONTROL_AREA>	m_csControlArea;

		//	status
		INT16			m_nStatus;				// status
		INT16			m_nNumPlayers;			// no. of players
		time_t			m_lTime;				// start time

		//	Extra Key-Value Map
		ApAdmin			*m_pExtraMap;

		INT32			m_lMaxUserCount;

		BOOL			m_bIsTestServer;
		BOOL			m_bIsAimEventServer;

		INT32			m_lStartLevel;
		INT64			m_llStartGheld;
		
		INT32			m_lOrder;

		//JK_�ɾ߼��ٿ�
		BOOL			m_bMidNightShutDown;

	public:
		AgsdServer2();
		virtual ~AgsdServer2();

		void	Initialize();
		void	Cleanup();
		BOOL	AddWorld(CHAR *pszWorld, BOOL bShow);
		BOOL	DecodeExtra(CHAR *pszExtra);
		CHAR*	GetExtraValue(CHAR *pszKey);
	};

typedef AgsdServer2 AgsdServer;

#endif //	_AGSDSERVER_H_


