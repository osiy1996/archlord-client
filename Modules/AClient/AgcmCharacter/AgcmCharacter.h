#ifndef			_AGCMCHARACTER_H_
#define			_AGCMCHARACTER_H_

#include <map>
#include <rwcore.h>
#include <rpworld.h>
#include <rphanim.h>
#include <rpskin.h>
#include <AuPacket/AuPacket.h>
#include <AcuObject/AcuObject.h>
#include <AgcModule/AgcModule.h>
#include <ApBase/ApBase.h>
#include <ApModule/ApModule.h>
#include <AgcmCharacter/AgcdCharacter.h>
#include <AgcmCharacter/AgcCharacterDefine.h>
#include <AuRandomNumberGenerator/AuRandomNumber.h>
#include <AgpmCharacter/AgppCharacter.h>
#include <AgcmUILogin/ContainerUtil.h>
#include <AgpmPathFind/SpecializeListPathFind.h>
#include <set>


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmCharacterD" )
#else
#pragma comment ( lib , "AgcmCharacter" )
#endif
#endif

class AgcdUIControl;
class AgcmUIHotkey;
class AgpdPathFindPoint;
struct AgcdUIHotkey;

#define AGCMCHAR_MAX_PATH				128

#define AGCMCHAR_HIERARCHY_MAX_KEYFRAME		100

#define AGCMCHAR_DEFAULT_SEND_MOVE_DELAY	500

#define	AGCMCHAR_MAX_SELECTABLE_DISTANCE	4000

#define AGCMCHAR_DEFAULT_ANIM_BLEND_DURATION		200
#define AGCMCHAR_DEFAULT_SUB_ANIM_BLEND_DURATION	50
#define AGCMCHAR_ATTACK_ANIM_POINT_RATIO			0.4f
#define AGCMCHAR_COMBO_ANIM_SKIP_RATIO				0.225f
#define AGCMCHAR_MAX_ATTACK_TIME					10000

#define AGCMCHAR_RAND_PERCENTAGE					100
#define AGCMCHAR_PC_STRUCK_ANIM_RATE				20

#define AGCMCHAR_MAX_ANIM_CB					20

#define AGCMCHAR_RIDE_CID_START					1000000

#ifndef	__ATTACH_LOGINSERVER__

typedef enum  AgcmCharacterPacketOperation {
	AGCMCHAR_PACKET_OPERATION_LOGIN	= 0,
	AGCMCHAR_PACKET_OPERATION_LOGIN_RESULT,
	AGCMCHAR_PACKET_OPERATION_CREATECHAR,
	AGCMCHAR_PACKET_OPERATION_CREATECHAR_RESULT,
	AGCMCHAR_PACKET_OPERATION_DELETECHAR,
	AGCMCHAR_PACKET_OPERATION_DELETECHAR_RESULT,
	AGCMCHAR_PACKET_OPERATION_SELECTCHAR,
	AGCMCHAR_PACKET_OPERATION_SELECTCHAR_RESULT
} AgcmCharacterPacketOperation;

#else

typedef enum  AgcmCharacterPacketOperation {
	AGCMCHAR_PACKET_OPERATION_REQUEST_ADDCHAR			= 0,
	AGCMCHAR_PACKET_OPERATION_REQUEST_ADDCHAR_RESULT,
	AGCMCHAR_PACKET_OPERATION_REQUEST_CLIENT_CONNECT,
	AGCMCHAR_PACKET_OPERATION_REQUEST_CLIENT_CONNECT_RESULT,
	AGCMCHAR_PACKET_OPERATION_SEND_COMPLETE,
	AGCMCHAR_PACKET_OPERATION_SEND_INCOMPLETE,
	AGCMCHAR_PACKET_OPERATION_SEND_CANCEL,
	AGCMCHAR_PACKET_OPERATION_SETTING_CHARACTER_OK,
	AGCMCHAR_PACKET_OPERATION_CHARACTER_POSITION,
	AGCMCHAR_PACKET_OPERATION_LOADING_COMPLETE,
	AGCMCHAR_PACKET_OPERATION_DELETE_COMPLETE,
	AGCMCHAR_PACKET_OPERATION_AUTH_KEY,
} AgcmCharacterPacketOperation;

#endif	//__ATTACH_LOGINSERVER__

typedef enum AgcmCharacterDataType {
	AGCMCHAR_DATA_TYPE_TEMPLATE	 = 0,
	AGCMCHAR_DATA_TYPE_ANIMATION,
	AGCMCHAR_DATA_TYPE_CUSTOMIZE,
} AgcmCharacterDataType;

typedef enum AgcmCharacterCallbackPoint
{
	AGCMCHAR_CB_ID_INIT_CHARACTER = 0,
//	AGCMCHAR_CB_ID_INIT_CHAR_CLUMP,
	AGCMCHAR_CB_ID_CHANGE_ANIM,
	AGCMCHAR_CB_ID_CHANGE_ANIM_WAIT,
	AGCMCHAR_CB_ID_CHANGE_ANIM_WALK,
	AGCMCHAR_CB_ID_CHANGE_ANIM_RUN,
	AGCMCHAR_CB_ID_CHANGE_ANIM_ATTACK,
	AGCMCHAR_CB_ID_CHANGE_ANIM_STRUCK,
	AGCMCHAR_CB_ID_CHANGE_ANIM_DEAD,
	AGCMCHAR_CB_ID_CHANGE_ANIM_SKILL,
	AGCMCHAR_CB_ID_CHANGE_ANIM_SOCIAL,
	AGCMCHAR_CB_ID_END_SOCIAL_ANIM,
	AGCMCHAR_CB_ID_ATTACK_SUCESS,
	AGCMCHAR_CB_ID_STRUCK_CHARACTER,
	AGCMCHAR_CB_ID_SKILL_STRUCK_CHARACTER,
	AGCMCHAR_CB_ID_PROCESS_SKILL_STRUCK_CHARACTER,
	AGCMCHAR_CB_ID_SET_SELF_CHARACTER,
	AGCMCHAR_CB_ID_RELEASE_SELF_CHARACTER,
	AGCMCHAR_CB_ID_SELF_UPDATA_POSITION,
	AGCMCHAR_CB_ID_SELF_STOP_CHARACTER,
	AGCMCHAR_CB_ID_REMOVE_CHARACTER,
	AGCMCHAR_CB_ID_PRE_REMOVE_DATA,
	AGCMCHAR_CB_ID_SETTING_CHARACTER_OK,
	AGCMCHAR_CB_ID_LOAD_CLUMP,
	AGCMCHAR_CB_ID_LOAD_DEFAULT_ARMOUR_CLUMP,
	AGCMCHAR_CB_ID_DISPLAY_ACTION_RESULT,
	AGCMCHAR_CB_ID_LOCK_TARGET,
	AGCMCHAR_CB_ID_UNLOCK_TARGET,
	AGCMCHAR_CB_ID_SELECT_TARGET,
	AGCMCHAR_CB_ID_UNSELECT_TARGET,
	AGCMCHAR_CB_ID_NUM,
	AGCMCHAR_CB_ID_GAMESERVER_DISCONNECT,
	AGCMCHAR_CB_ID_GAMESERVER_ERROR,
	AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_ARROW,
	AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_BOLT,
	AGCMCHAR_CB_ID_ACTION_RESULT_NOT_ENOUGH_MP,
	AGCMCHAR_CB_ID_ACTION_SKILL,
	AGCMCHAR_CB_ID_UPDATE_CHAR_HP,
	AGCMCHAR_CB_ID_UPDATE_CHAR_MP,
	AGCMCHAR_CB_ID_UPDATE_MOVEMENT,
	AGCMCHAR_CB_ID_ON_ANIM_CHAGE,
	AGCMCHAR_CB_ID_KILL_CHARACTER,
	AGCMCHAR_CB_ID_LOAD_TEMPLATE,
	AGCMCHAR_CB_ID_RELEASE_TEMPLATE,
	AGCMCHAR_CB_ID_SELF_REGION_CHANGE,
	AGCMCHAR_CB_ID_LOADING_COMPLETE,
	AGCMCHAR_CB_ID_UPDATE_OPTION_FLAG,		//	2005.06.01. By SungHoon
	AGCMCHAR_CB_ID_WAVE_FX_TRIGGER,			//	2005.08.17. Nonstopdj
	AGCMCHAR_CB_ID_UPDATE_TRANSPARENT,		//	2005.09.09. steeple
	AGCMCHAR_CB_ID_NATURE_FX_TRIGGER,		//	2005.10.04. Nonstopdj
	AGCMCHAR_CB_ID_SOCKET_ON_CONNECT,		//	2006.04.14. steeple
	AGCMCHAR_CB_ID_IS_CAMERA_MOVING,
	AGCMCHAR_CB_ID_TURN_CAMERA,
	AGCMCHAR_CB_ID_PINCHWANTED_ANSWER,
	AGCMCHAR_CB_ID_DESTROY_TEXTBOARD,
} AgcmCharacterCallbackPoint;

enum eAgcmCharacterFlags
{
	E_AGCM_CHAR_FLAGS_NONE							= 0x0000,
	E_AGCM_CHAR_FLAGS_SKIP_SEARCH_SITUATION			= 0x0001,
	E_AGCM_CHAR_FLAGS_EXPORT						= 0x0002,
	E_AGCM_CHAR_FLAGS_CONVERT_ANIMTYPE2				= 0x0004,
};

typedef enum
{
	AGCMCHAR_READ_TYPE_ALL				= 0,
	AGCMCHAR_READ_TYPE_WAIT_ANIM_ONLY	= 1,
} AgcmCharacterReadType;


//. 2006. 6. 21. nonstopdj
//. ���� ���̺� HP ����.
typedef enum AgcmCharacterPolymorphCondition{
	AGCMCHAR_POLYMORPH_CONDITION_TYPE1 = 1,				//. HP 0.
	AGCMCHAR_POLYMORPH_CONDITION_TYPE2,					//. HP 50, 0.
	AGCMCHAR_POLYMORPH_CONDITION_TYPE3					//. HP 70, 30, 0.
}AgcmCharacterPolymorphCondition;

struct stCharacterPolyMorphEntry
{
	ContainerVector< int >				m_vecTID;
	AgcmCharacterPolymorphCondition		m_eConditionType;

	stCharacterPolyMorphEntry( void )
	{
		m_eConditionType = AGCMCHAR_POLYMORPH_CONDITION_TYPE1;
	}
};

#define AGPMITEM_IMPORT_CHAR_RUN_CORRECT			"Run Correct"
#define AGPMITEM_IMPORT_CHAR_ATTACK_CORRECT			"Attack Correct"


const RwV3d		m_stXAxis = {1,0,0};
const RwV3d		m_stYAxis = {0,1,0};

class AgcmGuild;
class AgpdItem;

class AgcmCharacter : public AgcModule
{
public:
	static AgcmCharacter *	m_pThisAgcmCharacter;

//private:
protected:
	AgcaAnimation2					m_csAnimation2			;
	class AgpmFactors			*	m_pcsAgpmFactors		;
	class AgpmCharacter			*	m_pcsAgpmCharacter		;
	class AgpmParty				*	m_pcsAgpmParty;
	class AgpmSkill				*	m_pcsAgpmSkill			;
	class AgpmPvP				*	m_pcsAgpmPvP			;
	class AgcmRender			*	m_pcsAgcmRender			;
	class AgcmShadow			*	m_pcsAgcmShadow			;
	class AgcmShadowmap			*	m_pcsAgcmShadowmap		;
	class AgcmConnectManager	*	m_pcsAgcmConnectManager	;
	class AgpmSummons			*	m_pcsAgpmSummons		;
	class AgcmUIHotkey			*	m_pcsAgcmUIHotkey		;
	class AgpmArchlord			*	m_pcsAgpmArchlord		;
	class AgcmGuild				*	m_pcsAgcmGuild;
	class ApmMap				*	m_pcsApmMap				;
	class AgcmMap				*	m_pcsAgcmMap			;
	class ApmObject				*	m_pcsApmObject			;
	class AgcmObject			*	m_pcsAgcmObject			;
	class AgcmLODManager		*	m_pcsAgcmLODManager		;
	class AgcmShadow2			*	m_pcsAgcmShadow2		;
	class AgcmFont				*	m_pcsAgcmFont			;
	class AgcmResourceLoader	*	m_pcsAgcmResourceLoader	;
	class AgpmOptimizedPacket2	*	m_pcsAgpmOptimizedPacket2	;
	class AgcmSound				*	m_pcsAgcmSound			;
	class AgpmPathFind			*	m_pcsAgpmPathFind		;

	// ��⿡ �ʿ��� Flags�� �����Ѵ�. (BOB, 081104)
	UINT32					m_ulModuleFlags;

	INT16					m_nCharacterAttachIndex	;
	INT16					m_nTemplateAttachIndex	;
	RpWorld *				m_pWorld				;
	RwCamera *				m_pCamera				;
	INT32					m_lMaxAnimation			;
	CHAR					m_szClumpPath		[AGCMCHAR_MAX_PATH];
	CHAR					m_szAnimationPath	[AGCMCHAR_MAX_PATH];

	FLOAT					m_fCameraPan			;
	FLOAT					m_fCameraPanInitial		;
	FLOAT					m_fCameraPanUnit		;

	UINT32					m_ulPrevSendMoveTime	;
	UINT32					m_ulSendMoveDelay		;
	INT32					m_lLastSendActionTarget	;

	//UINT32					m_ulActionBlockTime		;

	BOOL					m_bUseTexDict			;
	CHAR					m_szTexDict			[AGCMCHAR_MAX_PATH];
	RwTexDictionary *		m_pstTexDict			;

	CHAR					m_szTexturePath		[AGCMCHAR_MAX_PATH];

	// �׽�Ʈ��
	INT32					m_ulSelectedTID			;

	static RwTextureCallBackRead	m_fnDefaultTextureReadCB	;

	BOOL					m_bFirstSendAttack		;
	BOOL					m_bStartAttack			;

	INT32					m_lLoaderIDInit			;
	INT32					m_lLoaderCustomize		;
	INT32					m_lLoaderIDRemove		;

	MTRand					m_csRandom				;

	INT32					m_lReceivedAuthKey		;

	AgpdCharacterAction		m_stNextAction			;

	BOOL					m_bShowPickingInfo		;
	BOOL					m_bShowActionQueue		;

	BOOL					m_bShowBoundingSphere	;

	BOOL					m_bStrafeMove			;
	BOOL					IsStrafeMove()			{ return m_bStrafeMove; }
	VOID					ToggleStrafeMove()		;


//ride
	RpClump*				m_pSelfRideClump		;

	AgcmCharacterReadType	m_eReadType				;

public:
	CHAR					m_szGameServerAddress[128]	;

	AuPacket				m_csPacket				;
	UINT32					m_ulCurTick				;

	//Current Charcter���� ��Ȳ ���� 
	UINT32					m_lCurCharacterNum		;								//Add�� Character�� ��
	UINT32					m_lCurCID			[CLIENT_CHARACTER_MAX_NUM];			//���� ���� �ִ� Character�� CID
	AgcdCharacter *			m_pCurCharacter		[CLIENT_CHARACTER_MAX_NUM];		//���� ���� �ִ� Character Data�� ���� ������

	INT32					m_lSelfCID				;
	RwFrame *				m_pPlayerFrame			;
	AgpdCharacter *			m_pcsSelfCharacter		;

	BOOL					m_bIsDeletePrevSelfCharacterWhenNewSelfCharacterisSet;
	AgpdCharacter *			m_pcsPrevSelfCharacter	;

	CHAR					m_szCharName[AGPACHARACTER_MAX_ID_STRING + 1];

	RpClump*				m_pMainCharacterClump;

	INT32					m_lFollowTargetCID		;
	INT32					m_lFollowDistance		;

	BOOL					m_bReservedMove;
	UINT32					m_ulLastSendMoveTime;

	AuPOS					m_stReservedMoveDestPos;
	MOVE_DIRECTION			m_eReservedMoveDirection;
	BOOL					m_bReservedMovePathFinding;
	BOOL					m_bReservedMoveMoveFast;
	RwV3d					m_stReservedMoveAt;

	BOOL					m_bFollowAttackLock;
	INT32					m_lFollowAttackTargetID;

	std::string				m_szNextAction;
	std::string				m_szActionQueue;
	std::string				m_szActionQueueAcc;
	std::string				m_szAttackSentAcc;

	// Current Character�� Status â
	//AcUICharacterStatus		m_clCharacterStatus		;

	// Current Character�� Personal Info â 
	//AcUIPersonalInfo		m_clPersonalInfo		;

	// ������ (2005-03-11 ���� 11:38:20) : 
	// PathFinding ����
	CSpecializeListPathFind<int, AgpdPathFindPoint *>	m_cPathNodeList;

	AuPOS					m_posMoveStart		;	// �̵� ������
	AuPOS					m_posMoveEnd		;	// �̵� ������
	BOOL					m_bAdjustMovement	;	// �̵� ������.
	INT32					m_nRetryCount		;	// ��õ� Ƚ��..

	//@{ 2006/07/05 burumal
	AgcdUIHotkey*			m_pMovementHotkey[4];
	//@}

	BOOL					m_bTurnLeft;
	BOOL					m_bTurnRight;

#ifdef USE_MFC
	//. 2006. 3. 16. nonstopdj
	//. only using Modeltool debug.
	BOOL					m_bisSaveHairClump		;	// �븻�� ������ Hair Clump�� ������ ���ΰ�?
	void					SaveEditHairAtomic(AgcdCharacter* pstCharacter);
#endif

	//. 2006. 3. 16. Nonstopdj.
	void					GeneralizeAttachedAtomicsNormals(AgcdCharacter* pstCharacter);

	//@{ Jaewon 20050826
	// A list of characters who will be removed from the world when its fading-out completes.
	std::deque<std::pair<AgpdCharacter*, RwUInt32> > m_fadeOutReomvalList;
	//@} Jaewon

	//. 2006. 6. 21. nonstopdj
	//. PolyMorph Table.
	ContainerMap< int, stCharacterPolyMorphEntry >	m_PolyMorphTableMap;

	//@{ 2006/12/15 burumal
	std::set<INT32> m_NonPickingTIDSet;
	//@}

	// ������ (2005-03-29 ���� 3:46:51) : �н� ���� ������ �̵�..
	// ret : �佺 ����.
	BOOL	MoveReservedPath();

	// test
	INT32 ConvertAnimType2(INT32 lAnimType2, INT32 lAnimEquip);

public:
	AgcmCharacter()		;
	~AgcmCharacter()	;

	// �ʼ� �Լ� (virtual �Լ���)
	BOOL			OnAddModule		(						);
	BOOL			OnInit			(						);
	BOOL			OnDestroy		(						);
	BOOL			OnIdle			( UINT32 ulClockCount	);
	void			OnLuaInitialize	( AuLua * pLua			);

	void			KeyboardIdleProcess		();
	void			CharacterPosIdleProcess	();

	struct			PolymorphQueueInfo
	{
		AgpdCharacter *	pCharacter	;
		INT32			lNewTID		;
		UINT32			uRequestTime;
		BOOL			bSelf		;
		
		enum	PROGRESS
		{
			REQUEST		,
			PROCESSING	,
			PROCESSED	
		};

		PROGRESS		eProgress	;

		
		PolymorphQueueInfo(): pCharacter( NULL ) , lNewTID( 0 ) , uRequestTime( 0 ) , eProgress( REQUEST ) , bSelf( FALSE ) {}
	};
	
	// �������� ���̺��� ����ؼ� �׻� Idle���� ������ �ŵ��� �Ѵ�.
	ApRWLock						m_RWLockPolymorphTable;
	vector< PolymorphQueueInfo >	m_vecPolymorphQueueTable;
	void			PolymorphIdleProcess	();
	bool			CheckCharacterPolyMorph	( AgpdCharacter * pcsCharacter );

	// ��� ������ �Լ���...
	AgpmCharacter	*GetAgpmCharacter()							{return m_pcsAgpmCharacter;}

	// ȯ�� ���� ���� �Լ���
	BOOL			SetWorld		( RpWorld *pWorld, RwCamera* pCamera		);			//�� ����� ������ �ݵ�� ���Ǵ� World�� �����͸� ��� �Ѵ�.
	BOOL			SetClumpPath	( CHAR *szPath								);
	CHAR			*GetClumpPath	(											)	{return m_szClumpPath;}
	BOOL			SetAnimationPath( CHAR *szPath								);
	void			SetMaxAnimations( INT32 lAnimationNum						);
	void			SetSendMoveDelay( UINT32 ulDelay							);
	void			SetTexDictFile	( CHAR *szTexDict							);
	void			SetTexturePath	( CHAR *szTexturePath						);

	// Character Initialization �Լ��� (Attached Module Data�� �ʱ�ȭ�Ѵ�.)
	BOOL				InitCharacter				( AgpdCharacter *pstAgpdCharacter, BOOL bUseThread = TRUE	);
	BOOL				LoadCharacterData			( AgpdCharacter *pstAgpdCharacter, BOOL bUseThread = TRUE	);

	BOOL				InitCharacterMainThread		( AgpdCharacter *pstAgpdCharacter	);
	BOOL				InitCharacterBackThread		( AgpdCharacter *pstAgpdCharacter	);
	static RwFrame *	GetChildFrameHierarchy		( RwFrame *frame, void *data		);
	static RpAtomic *	UnHookLODAtomic				( RpAtomic *atomic, void *data		);
	static RpAtomic *	SetHierarchyForSkinAtomic	( RpAtomic *atomic, void *data		);			//Skin Atomic�� Hierarchy�� Set�ϴ� Callback �Լ�
	RpHAnimHierarchy	*GetHierarchy				( RpClump *clump					);
	RpClump				*LoadClump					( CHAR *szDFF						);
	RpAtomic			*LoadAtomic					( const CHAR *szDFF, const CHAR *szSubPath = NULL, RpHAnimHierarchy* pstHierarchy = NULL );

	BOOL				LoadAllTemplateClump		(									);
	BOOL				SetTemplateClumpRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsRenderType);
	BOOL				LoadTemplateDefaultFace( AgcdCharacterTemplate *pstAgcdCharacterTemplate );
	BOOL				LoadTemplateDefaultHair( AgcdCharacterTemplate *pstAgcdCharacterTemplate );
	BOOL				LoadTemplateClump(AgpdCharacterTemplate *pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate);
	BOOL				LoadTemplateAnimation(AgpdCharacterTemplate *pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate);
	BOOL				LoadTemplateData(AgpdCharacterTemplate *pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate);
	BOOL				ReleaseTemplateDefaultFace( AgcdCharacterTemplate *pstTemplate );
	BOOL				ReleaseTemplateDefaultHair( AgcdCharacterTemplate *pstTemplate );
	BOOL				ReleaseTemplateClump(AgcdCharacterTemplate *pstTemplate);
	BOOL				ReleaseTemplateAnimation(AgcdCharacterTemplate *pstTemplate);
	BOOL				ReleaseTemplateData(AgcdCharacterTemplate *pstTemplate);
	BOOL				DeleteTemplateAnimation(AgcdCharacterTemplate *pstTemplate);

	BOOL				LoadTemplatePickingClump(AgpdCharacterTemplate* pstAgpdCharacterTemplate, AgcdCharacterTemplate *pstAgcdCharacterTemplate);
	BOOL				ReleaseTemplatePickingClump(AgcdCharacterTemplate *pstAgcdCharacterTemplate);

	BOOL				IsCharacterTransforming(AgpdCharacter *pcsCharacter);

	BOOL				RemoveCharacter(AgpdCharacter *pcsAgpdCharacter);
	BOOL				DestroyCharacter(AgpdCharacter *pcsAgpdCharacter);
	VOID				DestoryCustomize( AgcdCharacter* pstAgcdCharacter );

	// create sub-hierarchy(Add, 112602, Bob)
	RpHAnimHierarchy	*CreateSubHierarchy(RpHAnimHierarchy *pParentHierarchy, RwInt32 lNodeID);
	AgcmCharacterSubAnimNodeIndex FindNodeIndexFromSubAnimType(AgcmCharacterSubAnimType eSubAnimType);

	BOOL				SetSelfCharacter			(AgpdCharacter *pcsCharacter, BOOL bSetMainFrame = TRUE);
	AgpdCharacter *		GetSelfCharacter			() { return m_pcsSelfCharacter; }
	INT32				GetSelfCID					() { return m_lSelfCID; }
	void				ReleaseSelfCharacter		()							;	// Self Character ���� m_lSelfCID�� m_pcsSelfCharacter�� �ʱ�ȭ�Ѵ� 

	// Attached Module Data
	AgpdCharacter*				GetCharacter		( AgcdCharacter *pstCharacter			);
	AgcdCharacter*				GetCharacterData	( AgpdCharacter *pstCharacter			);
	AgpdCharacterTemplate*		GetTemplate			( AgcdCharacterTemplate *pstTemplate	);
	AgcdCharacterTemplate*		GetTemplateData		( AgpdCharacterTemplate *pstTemplate	);
//	AgcdEventCharacterEffect*	GetEFfectData(AgpdCharacterTemplate *pstTemplate);


	inline VOID					AddFlags(UINT32 ulAdd)	{m_ulModuleFlags	|= ulAdd;}
	///////////////////////////////////////////////////////////////////////////
	// Animation
	BOOL				SetAttackStatus(AgcdCharacter *pstAgcdCharacter);
	BOOL				AdminChangeAnim(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter = NULL);
	BOOL				SetSubAnimation(AgcdCharacter *pstAgcdCharacter, RpHAnimHierarchy *pstHierarchy, AgcmCharacterAnimType eType);
	BOOL				SetSubStruckAnimation(AgcdCharacter *pstAgcdCharacter);
	BOOL				SetStruckAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter = NULL, FLOAT fForceDelayTime = -1.0F, BOOL bMakeAnimPoint = TRUE);
	BOOL				SetSkillStruckAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter);
	BOOL				SetDeadAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter);

	BOOL				StopAnimation(INT32 lCID);
	BOOL				StopAnimation(AgpdCharacter *pstCharacter);
	BOOL				StopAnimation(AgcdCharacter *pstCharacter);

	VOID				SkipSearchSituation(BOOL bSkipSearchSituation = TRUE);	// Tool���� ���!(BOB, 081104)
	eAcReturnType		SearchSituation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eNewAnimType);
	eAcReturnType		CheckStartAnimCallback(
		AgpdCharacter						*pstAgpdCharacter,
		AgcdCharacter						*pstAgcdCharacter,
		AgcmCharacterAnimType				eNextAnimType,
		AgcdCharacterAnimation				*pstNextAnim,
		BOOL								*pbDestIdenticalAnim,
		BOOL								*pbDestEndThenChange,
		AgcmCharacterStartAnimCallbackPoint ePoint,
		INT32								lCustAnimType			);
	
	VOID				SetAnimOption(
		BOOL								*pbDestBlend,
		BOOL								*pbDestEndThenChange,
		AgcdCharacter						*pstAgcdCharacter,
		AgcdAnimationFlag					*pstFlags,
		BOOL								bIdenticalAnimation			);
	//@{ Jaewon 20051125
	// 'pfCustCallback' parameter added.
	//@{ Jaewon 20051129
	// 'fBlendingDuration' parameter added.
	BOOL				SetBlendingAnimation(
		AgpdCharacter			*pstAgpdCharacter,
		AgcdCharacter			*pstAgcdCharacter,
		AgcmCharacterAnimType	eNextAnimType,
		FLOAT					fBlendingDuration = 1.0f,
		AcCallbackData3			pfCustCallback = NULL	);
	//@} Jaewon
	//@} Jaewon

	BOOL				SetAnimSpeed(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter);
	BOOL				SetNextAnimation(
		AgpdCharacter						*pstAgpdCharacter,
		AgcdCharacter						*pstAgcdCharacter,
		AgcmCharacterAnimType				eNextAnimType,
		BOOL								bLoop,
		AcCallbackData3						pfCustCallback = NULL	);

	BOOL				StartAnimation(
		INT32 lCID,
		AgcmCharacterAnimType eType,
		BOOL bEndThenChange = FALSE,
		AgcmCharacterStartAnimCallbackPoint ePoint = AGCMCHAR_START_ANIM_CB_POINT_DEFAULT,
		INT32 lCustAnimType = 0																);
	BOOL				StartAnimation(
		AgpdCharacter *pstAgpdCharacter,
		AgcmCharacterAnimType eType,
		BOOL bEndThenChange = FALSE,
		AgcmCharacterStartAnimCallbackPoint ePoint = AGCMCHAR_START_ANIM_CB_POINT_DEFAULT,
		INT32 lCustAnimType = 0																);
	BOOL				StartAnimation(
		AgpdCharacter *pstAgpdCharacter,
		AgcdCharacter *pstAgcdCharacter,
		AgcmCharacterAnimType eType,
		BOOL bEndThenChange = FALSE,
		AgcmCharacterStartAnimCallbackPoint ePoint = AGCMCHAR_START_ANIM_CB_POINT_DEFAULT,
		INT32 lCustAnimType = 0																);

	BOOL				StartAnimation(
		AgpdCharacter *pstAgpdCharacter,
		AgcmCharacterAnimType eType,
		AgcdCharacterAnimation *pstNextAnim,
		BOOL bEndThenChange = FALSE,
		AgcmCharacterStartAnimCallbackPoint ePoint = AGCMCHAR_START_ANIM_CB_POINT_DEFAULT,
		INT32 lCustAnimType = 0																);
	BOOL				StartAnimation(
		AgpdCharacter *pstAgpdCharacter,
		AgcdCharacter *pstAgcdCharacter,
		AgcmCharacterAnimType eType,
		AgcdCharacterAnimation *pstNextAnim,
		BOOL bEndThenChange = FALSE,
		AgcmCharacterStartAnimCallbackPoint ePoint = AGCMCHAR_START_ANIM_CB_POINT_DEFAULT,
		INT32 lCustAnimType = 0																);

	BOOL				UpdateAnimationTimeForAllCharacter();

	BOOL				UpdateMainAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter* pstAgcdCharacter, UINT32 ulDeltaTime);
	BOOL				UpdateSubAnimation(AgcdCharacter* pstAgcdCharacter, UINT32 ulDeltaTime);

	BOOL				DropAnimation(AgcdCharacter *pstAgcdCharacter);

	BOOL				SetEndThenChangeAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eType);

	VOID				InitAnimData(AgcdCharacter *pstAgcdCharacter, AgcdCharacterAnimation *pcsAnimation);

	FLOAT				GetCurrentAnimDuration(AgcdCharacter *pstAgcdCharacter);
	FLOAT				GetAnimDuration(AgcdAnimation2 *pstAnim, INT32 lAnimIndex = 0);

	INT32				GetAnimType2Num(AgcdCharacterTemplate *pstAgcdCharacterTemplate);
	BOOL				AllocateAnimationData(AgcdCharacterTemplate *pstAgcdCharacterTemplate);

	VOID				SetAnimCBData(
		AgcdAnimationCallbackData *pcsData,
		__AnimationCallback pfCallback,
		FLOAT fTime,
		BOOL bLoop,
		INT32 lCID,
		AgcmCharacterAnimType eAnimType = AGCMCHAR_ANIM_TYPE_NONE,
		AgcmCharacterStartAnimCallbackPoint eStartAnimCBPoint = AGCMCHAR_START_ANIM_CB_POINT_DEFAULT,
		BOOL bInitAnim = FALSE,
		AcCallbackData3 pfCustCallbak = NULL,
		//INT32 lCustAnimType = -1
		INT32 lCustAnimType = 0
		);

	BOOL				AddAnimCallback(AgcdCharacter *pstAgcdCharacter, AgcdAnimationCallbackData *pcsData);

	static PVOID		RemoveAnimAttachedDataCB(PVOID pvData1, PVOID pvData2);

	static PVOID		OnAnimCB(PVOID pvData);

	static RtAnimInterpolator	*OnAnimEndCB(RtAnimInterpolator *animInstance, PVOID pvData);
	static RtAnimInterpolator	*OnAnimEndThenChangeCB(RtAnimInterpolator *animInstance, PVOID pvData);
	static RtAnimInterpolator	*OnSubAnimEndCB(RtAnimInterpolator *animInstance, PVOID pvData);
	static RtAnimInterpolator	*OnSubAnimEndThenChangeCB(RtAnimInterpolator *animInstance, PVOID pvData);
	static BOOL					IntervalMainAnimCB(PVOID pvData);
	static BOOL					IntervalSubAnimCB(PVOID pvData);

	BOOL						CopyAnimAttachedData(AgcdAnimation2 *pcsSrc, AgcdAnimation2 *pcsDest);
	///////////////////////////////////////////////////////////////////////////

	BOOL						RemovePreData(AgcdCharacterTemplate *pcsAgcdCharacterTemplate);

	// OnAnimEndCB���� Callback
	static PVOID				EnumCallbackStruckChar(PVOID pvClass, PVOID pvData1, PVOID pvData2);

	// Action �Լ���
	BOOL				MoveSelfCharacter		( AuPOS * pstPos, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding = FALSE, BOOL bMoveFast = FALSE, RwV3d *pstAt = NULL);
	BOOL				StopSelfCharacter		();
	BOOL				TurnSelfCharacter		( FLOAT fDegreeX, FLOAT fDegreeY, BOOL bSendPacket = FALSE		);
	BOOL				MoveTurnSelfCharacter	( AuPOS * pstPos, FLOAT fDegreeX, FLOAT fDegreeY, BOOL bMoveDirection, BOOL bPathFinding = FALSE	);
	BOOL				ActionSelfCharacter		( AgpdCharacterActionType nType, INT32 lTargetCID, INT32 lSkillID = 0, BOOL bForceAttack = FALSE	);

	BOOL				MoveForward();
	BOOL				MoveBackward();
	BOOL				MoveLeft();
	BOOL				MoveRight();

	BOOL				StopForward();
	BOOL				StopBackward();
	BOOL				StopLeft();
	BOOL				StopRight();

	BOOL				IsSendActionTime		( AgpdCharacterActionType eType, AgpdCharacter *pcsCharacter, INT32 lTargetID	);

	//@{ 2006/03/06 burumal	
	//BOOL				LockTarget				( AgpdCharacter *pcsCharacter, INT32 lTargetID, BOOL bForceAttack = FALSE	);
	BOOL				LockTarget				(AgpdCharacter* pcsCharacter, INT32 lTargetID, RpAtomic* pNewTargetFirstPickAtomic = NULL, BOOL bForceAttack = FALSE);
	//@}
	BOOL				UnLockTarget			( AgpdCharacter *pcsCharacter					);

	BOOL				SelectTarget			( AgpdCharacter *pcsCharacter, INT32 lTargetID	);
	BOOL				UnSelectTarget			( AgpdCharacter *pcsCharacter					);
	BOOL				IsSelectTarget			( AgpdCharacter *pcsCharacter					);
	INT32				GetSelectTarget			( AgpdCharacter *pcsCharacter					);

	BOOL				SetCastSkill			( AgpdCharacter *pcsCharacter					);
	BOOL				ResetCastSkill			( AgpdCharacter *pcsCharacter					);

	// Transparent ����. 2005.09.09. steeple
	//@{ Jaewon 20051101
	// 'fadeOutTiming' parameter added.
	BOOL				UpdateTransparent		( AgpdCharacter* pcsCharacter, BOOL bStart, RwUInt16 fadeOutTiming=0 );
	//@} Jaewon

	// Texture Dictionary ���� �Լ���
	BOOL				DumpTexDict				();

	// Streaming �Լ��� (Attached Module Data)

	BOOL				StreamReadTemplate			( CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE );
	BOOL				StreamReadTemplateAnimation	( CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE );
	BOOL				StreamReadTemplateCustomize	( CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE );

	//. 2006. 6. 21. Nonstopdj.
	BOOL				StreamReadPolyMorphTable	( CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = TRUE );

	BOOL				StreamWriteTemplate			(CHAR *szFile, BOOL bEncryption);
	BOOL				StreamWriteTemplateAnimation(CHAR *szFile, BOOL bEncryption);
	BOOL				StreamWriteTemplateCustomize(CHAR *szFile, BOOL bEncryption);

	static BOOL			AnimationStreamWriteCB		( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream	);
	static BOOL			AnimationStreamReadCB		( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream	);
	static BOOL			CustomizeStreamWriteCB		( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream	);
	static BOOL			CustomizeStreamReadCB		( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream	);
	static BOOL			TemplateStreamWriteCB		( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream	);
	static BOOL			TemplateStreamReadCB		( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream	);

	BOOL				TemplatePreDataStreamWrite(
		ApModuleStream *pcsStream,
		AgcdCharacterTemplate *pstAgcdCharacterTemplate	);

	BOOL				TemplateAnimationAttachedDataStreamWrite(
		ApModuleStream *pcsStream,
		AgcdAnimData2 *pcsCurAnimData,
		INT32 lAnimType,
		INT32 lAnimEquip,
		INT32 lAnimIndex											);

	BOOL				TemplateAnimationStreamWrite(
		ApModuleStream *pcsStream,
		AgcdCharacterTemplate *pstAgcdCharacterTemplate	);

	BOOL				TemplateFaceStreamWrite(
		ApModuleStream *pcsStream,
		AgcdCharacterTemplate *pstAgcdCharacterTemplate	);

	BOOL				TemplateHairStreamWrite(
		ApModuleStream *pcsStream,
		AgcdCharacterTemplate *pstAgcdCharacterTemplate	);

	//@{ 2006/08/21 burumal
	//BOOL				TemplateEtcDataStreamWrite( ApModuleStream *pcsStream, AgcdCharacterTemplate *pstAgcdCharacterTemplate	);
	BOOL				TemplateEtcDataStreamWrite( ApModuleStream *pcsStream, PVOID pData, AgcdCharacterTemplate *pstAgcdCharacterTemplate	);
	//@}

	BOOL				InclinePlaneQuadruped( RwMatrix* pstModelling, AgcdCharacter* pstAgcdCharacter, const RwV3d* pstPos );
	BOOL				InclinePlaneBiped( RwMatrix* pstModelling, AgcdCharacter* pstAgcdCharacter, const RwV3d* pstPos ); //�ΰ��� ���Ͱ� �ƴ϶�..�е� 2���� ��带 ����� ĳ��.

	static BOOL			CharacterImportDataReadCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Attached Module Data�� ������/�ı���
	static BOOL			CharacterConstructor	( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CharacterDestructor		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			TemplateConstructor		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			TemplateDestructor		( PVOID pData, PVOID pClass, PVOID pCustData				);
//	static BOOL			EffectConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL			EffectDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	// Streaming �Ҷ� ����.
//	BOOL				ParseValueAnimation		( INT32 lTID, AgcdCharacterTemplate *pTemplate, CHAR *szValue, BOOL bSub = FALSE	);
//	BOOL				ParseValueAnimationName	( CHAR *szValue, AgcdCharacterTemplate *pTemplate			);
//	BOOL				ParseValueAnimationName	( CHAR *szValue, CHAR **ppszAnimName);
//	BOOL				ParseValueSubAnimName	( CHAR *szValue, AgcdCharacterTemplate *pTemplate			);

	//Callback �Լ� 
	static BOOL			CBInitCharacter			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBRemoveCharacter		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBMoveCharacter			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBStopCharacter			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateCharacter		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateCharacter2		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdatePosition		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdatePositionCheck	( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBCheckISActionMove		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBActionAttack			( PVOID pData, PVOID pClass, PVOID pCustData				);
//	static BOOL			CBActionSkill			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateStatus			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateActionStatus	( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateActionStatusCheck	( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateLevel			( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBReceiveAction			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBCheckNowUpdateActionFactor	( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBNewCreateCharacter	( PVOID pData, PVOID pClass, PVOID pCustData				);
	
	static BOOL			CBAddChar				( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBRemoveChar			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateFactor			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateFactorMovement	( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateFactorHP		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateFactorMP		( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBUpdateMoney			( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBUpdateBankMoney		( PVOID pData, PVOID pClass, PVOID pCustData				);

	static RpAtomic*	CBRemoveCheckAtomic		( RpAtomic	*	pstAtomic	, PVOID pvData					);
	static BOOL			CBLoadMap				( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBTransformAppear		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBEvolutionAppear		( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBDisconnectByAnotherUser(PVOID pData, PVOID pClass, PVOID pCustData				);

//	static BOOL			CBRemoveAnimation		( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBSocialAnimation		( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBEndDeadAnimation		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBBindingRegionChange	( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBMoveActionAttack		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBMoveActionRelease		( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBRidableUpdate			( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBUpdateCustomize		( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL			CBUpdateOptionFlag		( PVOID pData, PVOID pClass, PVOID pCustData				);	//2005.05.31. By SungHoon

	static BOOL			CBRemoveRide			( PVOID pData, PVOID pClass, PVOID pCustData);

	//@{ Jaewon 20050826
	// Register a 'CheckRemoveChar' callback for the fade-out removal.
	static BOOL			CBCheckRemoveChar		( PVOID pData, PVOID pClass, PVOID pCustData				);
	//@} Jaewon

	static BOOL			CBAlreadyExistChar		( PVOID pData, PVOID pClass, PVOID pCustData				);
	static BOOL			CBGameguardAuth			( PVOID pData, PVOID pClass, PVOID pCustData				);

	static BOOL CBCheckMovementLock(PVOID pData, PVOID pClass, PVOID pCustData);

	// Bob �߰�
	BOOL SetCallbackInitCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL SetCallbackInitCharClump(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimWait(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimWalk(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimRun(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimStruck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimDead(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEndSocialAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChangeAnimSocial(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackStruckCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAttackSuccess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSkillStruckCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackProcessSkillStruckCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL SetCallbackChangeAction(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL SetCallbackActionAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL SetCallbackActionStruck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL SetCallbackActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSetSelfCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackReleaseSelfCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSelfUpdatePosition(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSelfStopCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPreRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSettingCharacterOK(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackLoadClump(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackLoadDefaultArmourClump(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackDisplayActionResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGameServerDisconnect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGameServerError(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackLockTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUnlockTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSelectTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUnSelectTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackActionResultNotEnoughArrow(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackActionResultNotEnoughBolt(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackActionResultNotEnoughMP(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackUpdateCharHP(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateCharMP(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateMovement(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackKillCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackLoadTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackReleaseTemplate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSelfRegionChange(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackLoadingComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackUpdateOptionFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.06.01. By SungHoon(�ʴ� �ź� �ݹ� ����)

	//. 2005. 8. 17 Nonstopdj
	//. �������� ������ �߻��ϴ� Effect Event Callback�� ����
	BOOL SetCallbackWaveFxTrigger(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackUpdateTransparent(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackSocketOnConnect(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackIsCameraMoving( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL SetCallbackTurnCamera( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL SetCallbackDestroyBoard( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	// ��� ���� �Լ�
	BOOL				OnPreReceive		( UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg		);
	BOOL				OnReceive			( UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg		);
	BOOL				SendLoginInfo		( INT32 lTID, CHAR *pszName										);
	BOOL				SendMoveCharacter	( AuPOS *posDest, MOVE_DIRECTION eMoveDirection, BOOL bPathFinding, BOOL bMoveFast);
	BOOL				SendMoveFollowCharacter	( INT32 lTargetCID, INT32 lFollowDistance );
	BOOL				SendStopCharacter	( );
	BOOL				SendActionInfo		( AgpdCharacterActionType eType, INT32 lTargetCID, INT32 lSkillID, UINT8 cComboInfo = 0, BOOL bForceAttack = FALSE, UINT8 cHitIndex = 0	);

	BOOL				SendSocialAnimation	( UINT8 ucSocialAnimation	);

	/*
	BOOL				SendCreateChar		( CHAR *szAccountName, INT32 lTID, CHAR *szName	);
	BOOL				SendSelectChar		( CHAR *szAccountName, INT32 lCID				);
	BOOL				SendRemoveChar		( CHAR *szAccountName, INT32 lCID				);
	*/
	BOOL				SendEnterGameWorld	( CHAR *szCharName,	CHAR *szServerAddress, INT32 lCID	);
	BOOL				SendLoadingComplete	( INT32 lCID									);

	BOOL				SendCancelEvolution	( INT32 lCID									);
	BOOL				SendCancelTransform	( INT32 lCID									);

	BOOL				SendRequestCouponInfo(INT32 lCID);
	BOOL				SendRequestCouponUse(INT32 lCID, stCouponInfo & couponInfo );
	BOOL				SendCancelAction(AgpdCharacter * pcsCharacter);

	static RpCollisionTriangle *CollisionWorldFirstCallback(RpIntersection *pstIntersection,
							RpWorldSector *pstSector, 
							RpCollisionTriangle *pstCollTriangle,
							RwReal fDistance, 
							void *pvData);

	VOID				CheckCustAnimType(INT32 *plDestAnimType, AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eChangeType);
	//INT32				GenerateAnimIndex(AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eChangeType, AgcdAnimation *pstAgcdAnimation, INT32 lCustAnimType);
	//INT32				GenerateAnimIndex(AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eChangeType, AgcdAnimation2 *pstAgcdAnimation, INT32 lCustAnimType);
	BOOL				SetNextAnimData(AgcdCharacter *pstAgcdCharacter, AgcmCharacterAnimType eChangeType, AgcdCharacterAnimation *pstAgcdAnimation, INT32 lCustAnimType);
	INT16				GetSelfNID();

//	CHAR				*GetAnimTypeName(INT16 nIndex);
//	CHAR				*GetAnimEquipName(INT16 nIndex);
//	CHAR				*GetSubAnimTypeName(INT16 nIndex);
	CHAR				*GetAnimationPath();

//	void				SetCharacterStatusInfo();			// Character Status�� ������ �����Ѵ�. 
//	void				SetPersonalInfo();					// Personal Info�� ������ �����Ѵ�.
	
	BOOL				SendMoveMoneyFromInventoryToBank(AgpdCharacter *pcsCharacter, INT64 llMoney);
	BOOL				SendMoveMoneyFromBankToInventory(AgpdCharacter *pcsCharacter, INT64 llMoney);

	BOOL				SendPacketOptionFlag(AgpdCharacter *pcsCharacter, INT32 lOptionFlag);	//	2005.05.31. By SungHoon

	BOOL				SendRequestClientConnection(INT32 lSocketIndex);	// 2006.04.14. steeple

//	BOOL				UpdateAllCharacter();
//	BOOL				UpdataSpecificCharacter(UINT32	CID);

	// �Ⱦ��� �Լ���
	/*
	BOOL				Test();			//�׽�Ʈ�� ���� �Լ�	

	void				CharacterUpdateAnimation(RwReal deltaTime);
	BOOL				ReceiveAddCharacter(void* pAgpdCharacter);				//Chararter Add�� ���� ���� ���� - AgcdCharacter �����ϰ� Current Character���� ���� 
	BOOL				ReceiveRemoveCharacter(UINT32 lCID);			//Character Remove�� ���� ���� ���� - AgcdCharacter �����ϰ� Current Character���� ����  
	*/

	static RwTexture *	CBTextureRead(const RwChar *name, const RwChar *maskName);

	//AgcaAnimation		*GetAnimationAdminModule()	{return &m_csAnimation;)
	AgcaAnimation2		*GetAgcaAnimation2() {return &m_csAnimation2;}

	// fOffset�� 0.0f���� 2.0f ������ ������.
	BOOL SetPreLightForAllCharacter(FLOAT fOffset);

	// ���� �׼ǵ鿡 ���� ������ ť�� �ְ� ���� �ϴ� �۾��� �Ѵ�.
	//////////////////////////////////////////////////////////////////
	BOOL							AddReceivedAction(AgpdCharacter *pcsCharacter, AgcmCharacterActionQueueData *pstActionData);
	AgcmCharacterActionQueueData*	GetHeadActionData(AgpdCharacter *pcsCharacter);
	BOOL							RemoveHeadActionData(AgpdCharacter *pcsCharacter);
	BOOL							GetReceivedAction(AgpdCharacter *pcsCharacter, AgpdCharacterActionType eType, INT32 lActorID, AgcmCharacterActionQueueData *pstReceiveBuffer, INT32 lSkillTID = 0);
	BOOL							ClearActionFactor(AgpdCharacter* pcsCharacter, AgcmCharacterActionQueueData *pstActionData, BOOL bShowDamage = FALSE);
	AgpdCharacterActionResultType	GetNextReceivedActionResult(AgpdCharacter *pcsActor, AgpdCharacter *pcsTarget, AgpdCharacterActionType eActionType);
	AgpdFactor*						GetNextReceivedActionDamageFactor(AgpdCharacter *pcsActor, AgpdCharacter *pcsTarget, AgpdCharacterActionType eActionType);

	AgcmCharacterActionQueueData*	GetSequenceActionData(AgpdCharacter *pcsCharacter, INT32 *plIndex);
	BOOL							RemoveActionData(AgpdCharacter *pcsCharacter, INT32 lIndex);

	BOOL							ApplyActionData(AgpdCharacter *pcsCharacter, AgcmCharacterActionQueueData *pcsActionData);
	BOOL							ProcessActionQueue(AgpdCharacter *pcsActor, AgpdCharacter *pcsTarget, AgpdCharacterActionType eActionType, INT32 lSkillTID = 0);
	
	BOOL							IsReceivedDeadAction(AgpdCharacter *pcsCharacter);

	static BOOL						ProcessStruckIdleEvent(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);
	static BOOL						ProcessSkillStruckIdleEvent(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);

	// Socket���� Callback
	static	BOOL					CBSocketOnConnect( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL					CBSocketOnDisConnect( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL					CBSocketOnError( PVOID pData, PVOID pClass, PVOID pCustData	);

	static	BOOL					CBInitCharacterMainThread( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL					CBInitCharacterBackThread( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL					CBRemoveCharacterLoader( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL					CBCustomizeLoader	( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL					m_sbStandAloneMode	;
	// ������ �������� �ʴ��� üũ

	/*
	BOOL							SetActionBlockTime(UINT32 ulClockCount);
	BOOL							IsActionBlockCondition(UINT32 ulClockCount);
	*/

	BOOL							ProcessReservedAction(AgpdCharacter *pcsCharacter);

	BOOL							SetFollowTarget(INT32 lTargetCID, INT32 lFollowDistance);
	BOOL							ReleaseFollowTarget();

	BOOL							SetNextAction(AgpdCharacterAction *pstNextAction);
	AgpdCharacterAction *			GetNextAction();
	BOOL							ProcessNextAction();

	VOID							ShowPickingInfo();
	VOID							HidePickingInfo();

	//@{ 2006/03/08	burumal
	VOID							ShowBoundingSphere();
	VOID							HideBoundingSphere();
	//@}

	VOID							ShowNextAction();
	VOID							ShowActionQueue();
	VOID							ClearActionQueue();
	VOID							ShowMyCID();
// #ifdef _AREA_CHINA_
// 	//@{ kday 20051027
// 	// ;)
// 	VOID							ShowNumOfCharacByType();
// 	//@} kday
// #endif
	void							ShowRegionInfo();

	VOID							BuildActionQueue();

	VOID							KillGameServer1();

#ifdef __PROFILE__
	VOID							EnableFrameCallStack();
	VOID							DisableFrameCallStack();
#endif

	BOOL							IsValidCharacter(AgpdCharacter *pcsCharacter);
	BOOL							IsValidCharacter(AgcdCharacter *pstCharacter);	

// customize
	BOOL							AttachFace( AgcdCharacter* pstCharacter );
	BOOL							DetachFace( AgcdCharacter* pstCharacter );
	VOID							SetFace( AgcdCharacter* pstCharacter, INT32 nFaceID );

	BOOL							AttachHair( AgcdCharacter* pstCharacter );
	BOOL							DetachHair( AgcdCharacter* pstCharacter );
	VOID							SetHair( AgcdCharacter* pstCharacter, INT32 nHairID );

	VOID							SetCustomizeRenderType( RpAtomic *pAtomic, int nRenderType );

// ride
	INT32							GenerateRideCID();
	BOOL							AttachRide( AgcdCharacter* pstCharacter, int nTID );
	BOOL							DetachRide( AgcdCharacter* pstCharacter, BOOL bRemove = FALSE );
	BOOL							AttachRideClump( AgcdCharacter* pstCharacter );

	VOID							SetSelfRideClump( RpClump* pSelfRideClump );
	RpClump*						GetSelfRideClump() { return m_pSelfRideClump; }

	AgcdCharacter*					CreateRideCharacter( int nCID, int nTID );
	void							DestroyRideCharacter( AgcdCharacter* pcsAgcdCharacter );

	VOID							DestroyTextBoard( AgcdCharacter* pstCharacter );

	//@{ 2006/11/17 burumal
	inline BOOL						IsDead(AgcdCharacter* pcsCharacter)
	{
		if ( !pcsCharacter ) return FALSE;		
		if ( pcsCharacter->m_eCurAnimType == AGCMCHAR_ANIM_TYPE_DEAD ) return TRUE;
		return FALSE;
	}
	//@}

	//@{ 2006/12/15 burumal
	VOID							SetNonPickingTID(INT32 lTID);
	BOOL							IsNonPickingTID(INT32 lTID);
	//@}

	// ������ (2005-03-29 ���� 4:59:34) : 
	// �������̿� ��ŷ ������Ʈ �ɸ��°� �ִ��� Ȯ��.
	enum BLOCKING_STATE
	{
		NOBLOCKING			= 0,
		GEOMETRY_BLOCKING	,	// ��� üũ
		OBJECT_BLOCKING		,	// ��� üũ
		SECTOR_NOTFOUND		,	// ��� ����.

		UNKNOWN_BLOCKING		// ��� ����.
	};

	INT32	BlockingPositionCheck( AuPOS * pSrc , AuPOS * pstPos , RwV3d * pNormal , BOOL * pbLeft );
	INT32	GetValidDestination	( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , RwV3d * pNormal= NULL , BOOL * pbLeft = NULL );

	//. 2005. 09. 22 Nonstopdj
	//. Restore Character test. ������ �׽�Ʈ�� AngelScript�� ��ϵ� �Լ�
	void	RefreshCharacter();

	//@{ 2006/05/11 burumal
	void SetPlayerAttackDirection(const RwV3d& vAttDir);
	//@}

	//@{ Jaewon 20051202
	// The player character looks at a specific target direction 
	// according to mouse-moving or object targetting.
	void SetPlayerLookAtTarget(const RwV3d &lookAtTarget);
	void SetPlayerLookAtFactor(RwReal factor);
	RwReal GetPlayerLookAtFactor() const;
	void UpdateLookAt(UINT32 dt);
	//@}

	//@{ 2006/08/24 burumal
	void UpdateBendOver(UINT32 uDeltaTime, AgpdCharacter* pAgpdCharacter, AgpdCharacter* pAgpdTargetCharacter);
	//@}

	//. 2006. 6. 21. Nonstopdj
	//. TID PolyMorph
	BOOL SetCharacterPolyMorph(AgpdCharacter* pdCharacter, const float fHPrate);
	BOOL QueueCharPolyMorphByTID( AgpdCharacter *	pcsAgpdCharacter, INT32	 lNewTID);
	BOOL StartCharPolyMorphByTID( AgpdCharacter *	pcsAgpdCharacter, INT32	 nPrevTID);
	BOOL EndCharPolyMorph( AgpdCharacter* pdCharacter );

	BOOL StartCharEvolutionByTID( AgpdCharacter *	pcsAgpdCharacter, INT32	 lNewTID);
	BOOL EndCharEvolution( AgpdCharacter* pdCharacter );

	//@{ 2006/07/05 burumal
	BOOL SetMovementKeys();
	//@}

	//@{ 2006/09/06 burumal
	VOID ShowClumpForced(INT32 lCID, BOOL bShow, UINT32 uFadeTime);
	//@}

private:

	RwV3d m_lookAtTarget;
	RwV3d m_lookAtCurrent;
	RwMatrix m_headMatrix;
	// A blending factor(0.0~1.0) between a keyframe animation &  the look-at motion.
	RwReal m_lookAtFactor;
	//@} Jaewon

	//@{ 2006/07/14 burumal
	bool m_bLookAtFirstUpdateFlag;
	//@}

	//@{ 2007/01/03 burumal
	bool m_bBendingDisabled;
	//@}

	//@{ 2006/07/26 burumal
	static BOOL		CBFunctionCharacterMoveFront(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static BOOL		CBFunctionCharacterMoveLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static BOOL		CBFunctionCharacterMoveRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static BOOL		CBFunctionCharacterMoveBack(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	//@}
	
public:
	bool IsValidLOD( AgpdCharacterTemplate* pAgpdCharacterTemplate, AgcdCharacterTemplate* pAgcdCharacterTemplate );
	bool GetLodClumpPolyNum( std::vector< int >& vResult, AgpdCharacter* pAgpdCharacter );

	VOID SetTemplateReadType( AgcmCharacterReadType eReadType = AGCMCHAR_READ_TYPE_ALL )
	{
		m_eReadType = eReadType;
	}
	AgcmCharacterReadType	GetTemplateReadType()
	{
		return m_eReadType;
	}

	//@{ 2007/01/03 burumal
	VOID					DisableBending(bool bVal) { m_bBendingDisabled = bVal; }
	BOOL					BendingDisabled() { return m_bBendingDisabled; }
	//@}

	BOOL					OnEquipItemByEvolution( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AuCharClassType eNextClassType );
	BOOL					OnChagneEquipItemByEvolution( AgpdCharacter* ppdCharacter, AuCharClassType eNextClassType );
	BOOL					OnEquipCharonForSummoner( AgpdCharacter* ppdCharacter );

	BOOL					OnPolyMorph( void* pCharacter, INT32 nTargetTID );
	AuCharClassType			GetClassTypeByTID( INT32 nTID );
	void					OnUpdateCharacterCustomize( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter );

private :
	void					_UpdateFactorCharacterType( AgpdCharacter* ppdCharacter, INT32 nNewTID );

private :
	//@{ 2006/07/12 burumal
	BOOL	m_bMoveKeyEnabled;
	//@}

	//@{ 2006/07/26 burumal
	UINT	m_nMoveVirtualKeys[4];
	//@}

	BOOL	m_bKeyDownW;
	BOOL	m_bKeyDownS;
	BOOL	m_bKeyDownA;
	BOOL	m_bKeyDownD;

	BOOL	SelfMoveDirection();

	BOOL	SetHPZero(AgpdCharacter *pcsCharacter);
	
	void RemoveDuplicateSiegeWarObject(AgpdCharacter *pcsCharacter);
	AgpdSkillTemplate*	_GetCurrentEvolutionSkillTemplate( void );
#if defined( _AREA_GLOBAL_ ) || defined( _AREA_KOREA_ ) || defined( _AREA_TAIWAN_ )
public:
	BOOL SendGameGuardAuth(PVOID pggData, INT16 ggDataLength);
#endif
};

#endif //_AGCMCHARACTER_H_
