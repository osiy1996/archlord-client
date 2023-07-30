#ifndef		_AGCDCLIENTCHARACTER_H_
#define		_AGCDCLIENTCHARACTER_H_

#include <AgpmCharacter/AgpdCharacter.h>
#include <AgcSkeleton/AcDefine.h>
#include <AgcmCharacter/AgcCharacterDefine.h>
#include <AgcmAnimation/AgcaAnimation.h>
#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rphanim.h>
#include <rpskin.h>
#include <rtcharse.h>
#include <rtanim.h>
#include <AgpmSkill/AgpdSkill.h>
#include <AcuCreateClump/AcuCreateClump.h>
#include <ApBase/ApRWLock.h>
#include <AgpmItem/AgpdItemTemplate.h>
#include <AgcmRender/AgcdRenderType.h>
#include <AgcmOcTree/AgcmOcTree.h>

class AgcmStartEventEffectParams;

#define	AGCDCHAR_DFF_NAME_LENGTH						64
#define AGCDCHAR_MAX_STR								256
#define AGCD_CHARACTER_LINK_ANIM_SPEED_OFFSET			0.01f
#define AGCD_CHARACTER_LINK_ANIM_SPEED_OFFSET_MAX		0.25f
#define AGCD_CHARACTER_MAX_ALLOC_ANIM_DATA				20
#define AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY		"AGCD_CHARACTER"

#define	AGCD_CHARACTER_MAX_ACTION_QUEUE_SIZE			20
#define	AGCD_CHARACTER_MAX_ACTION_ATTACK_PRESERVE_TIME	1000			// ���� �׼� ����Ÿ�� �ִ� �󸶱��� �����ϰ� ������...	1700 -> 1000 ���� ����
#define	AGCD_CHARACTER_ACTION_SELF_CHARACTER_INTERVAL	100
#define	AGCD_CHARACTER_MAX_WAIT_TIME_FOR_COMBO_ACTION	300
#define AGCD_CHARACTER_MAX_PREENGAGED_EVENT_EFFECT		20

using namespace std;

enum AgcmCharacterAnimType
{
	AGCMCHAR_ANIM_TYPE_SOCIAL_REPEAT	 = -13,
	AGCMCHAR_ANIM_TYPE_SOCIAL			 = -12,
	AGCMCHAR_ANIM_TYPE_SKILL			 = -11,
	AGCMCHAR_ANIM_TYPE_CUST_START		 = -10,
	AGCMCHAR_ANIM_TYPE_NONE				 = -1,
	AGCMCHAR_ANIM_TYPE_WAIT				 =  0,
	AGCMCHAR_ANIM_TYPE_WALK,
	AGCMCHAR_ANIM_TYPE_RUN,
	AGCMCHAR_ANIM_TYPE_ATTACK,
	AGCMCHAR_ANIM_TYPE_STRUCK,
	AGCMCHAR_ANIM_TYPE_DEAD,
	AGCMCHAR_ANIM_TYPE_CUSTOMIZE_PREVIEW,

	AGCMCHAR_ANIM_TYPE_LEFTSIDE_STEP,				// ���� Ⱦ�̵�
	AGCMCHAR_ANIM_TYPE_RIGHTSIDE_STEP,				// ������ Ⱦ�̵�
	AGCMCHAR_ANIM_TYPE_BACKWARD_STEP,				// �ް���	
	AGCMCHAR_ANIM_TYPE_PICKUP,						// �ݴ�
	AGCMCHAR_ANIM_TYPE_EAT,							// �Դ�
	AGCMCHAR_ANIM_TYPE_SUMMON_CONVEYANCE,			// Ż�ͼ�ȯ�ϴ�
	AGCMCHAR_ANIM_TYPE_EMBARK,						// �¿��
	AGCMCHAR_ANIM_TYPE_RIDE,						// ž���ϴ�
	AGCMCHAR_ANIM_TYPE_ABNORMAL,					// �̻����

	AGCMCHAR_MAX_ANIM_TYPE
};

enum AgcmCharacterAnimType2
{
	AGCMCHAR_AT2_BASE	= 0,
	AGCMCHAR_AT2_WARRIOR,		//����
	AGCMCHAR_AT2_ARCHER,		//�ü�
	AGCMCHAR_AT2_WIZARD,		//������
	AGCMCHAR_AT2_BOSS_MONSTER,
	AGCMCHAR_AT2_ARCHLORD,
	AGCMCHAR_AT2_NUM
};

#define AGCMCHAR_AT2_COMMON_DEFAULT	0

enum AgcmCharacterBaseAnimType2
{
	AGCMCHAR_AT2_BASE_DEFAULT = AGCMCHAR_AT2_COMMON_DEFAULT,
	AGCMCHAR_AT2_BASE_NUM
};

enum AgcmCharacterWarriorAnimType2
{
	AGCMCHAR_AT2_WARRIR_DEFAULT	= AGCMCHAR_AT2_COMMON_DEFAULT,
	AGCMCHAR_AT2_WARRIR_ONE_HAND_SLASH,
	AGCMCHAR_AT2_WARRIR_ONE_HAND_BLUNT,
	AGCMCHAR_AT2_WARRIR_TWO_HAND_SLASH,
	AGCMCHAR_AT2_WARRIR_TWO_HAND_BLUNT,
	AGCMCHAR_AT2_WARRIR_ONE_HAND_POLEARM,
	AGCMCHAR_AT2_WARRIR_ONE_HAND_SCYTHE,
	AGCMCHAR_AT2_WARRIR_DEFAULT_RIDE,
	AGCMCHAR_AT2_WARRIR_WEAPON_RIDE,
	AGCMCHAR_AT2_WARRIR_STANDARD_RIDE,
	AGCMCHAR_AT2_WARRIR_ONE_HAND_CHARON,
	AGCMCHAR_AT2_WARRIR_TWO_HAND_CHARON,
	AGCMCHAR_AT2_WARRIR_NUM
};

enum AgcmCharacterArcherAnimType2
{
	AGCMCHAR_AT2_ARCHER_DEFAULT	= AGCMCHAR_AT2_COMMON_DEFAULT,
	AGCMCHAR_AT2_ARCHER_TWO_HAND_BOW,
	AGCMCHAR_AT2_ARCHER_TWO_HAND_CROSSBOW,
	AGCMCHAR_AT2_ARCHER_TWO_HAND_GLAVE,
	AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE,
	AGCMCHAR_AT2_ARCHER_WEAPON_RIDE,
	AGCMCHAR_AT2_ARCHER_TWO_HAND_KATARIYA,
	AGCMCHAR_AT2_ARCHER_STANDARD_RIDE,
	AGCMCHAR_AT2_ARCHER_ONE_HAND_ZENNON,
	AGCMCHAR_AT2_ARCHER_TWO_HAND_ZENNON,
	AGCMCHAR_AT2_ARCHER_ONE_HAND_CHARON,
	AGCMCHAR_AT2_ARCHER_TWO_HAND_CHARON,
	AGCMCHAR_AT2_ARCHER_NUM
};

enum AgcmCharacterWizardAnimType2
{
	AGCMCHAR_AT2_WIZARD_DEFAULT	= AGCMCHAR_AT2_COMMON_DEFAULT,
	AGCMCHAR_AT2_WIZARD_STAFF,
	AGCMCHAR_AT2_WIZARD_TROPHY,
	AGCMCHAR_AT2_WIZARD_HOOP,
	AGCMCHAR_AT2_WIZARD_DEFAULT_RIDE,
	AGCMCHAR_AT2_WIZARD_WEAPON_RIDE,
	AGCMCHAR_AT2_WIZARD_TWO_HAND_CHAKRAM,
	AGCMCHAR_AT2_WIZARD_STANDARD_RIDE,
	AGCMCHAR_AT2_WIZARD_ONE_HAND_CHARON,
	AGCMCHAR_AT2_WIZARD_TWO_HAND_CHARON,
	AGCMCHAR_AT2_WIZARD_NUM
};

enum AgcmCharacterArchlordAnimType2
{
	AGCMCHAR_AT2_ARCHLORD_DEFAULT = AGCMCHAR_AT2_COMMON_DEFAULT,
	AGCMCHAR_AT2_ARCHLORD_DEFAULT_RIDE,
	AGCMCHAR_AT2_ARCHLORD_STANDARD_RIDE,
	AGCMCHAR_AT2_ARCHLORD_NUM
};

#define AGCM_CHARACTER_MAX_ANIM_TYPE2	AGCMCHAR_AT2_WARRIR_NUM

enum AgcmCharacterWaitAnimType
{
	AGCMCHAR_WAIT_ANIM_TYPE_NORMAL	= 0,
	AGCMCHAR_WAIT_ANIM_TYPE_ATTACK,
	AGCMCHAR_MAX_WAIT_ANIM_TYPE
};

enum AgcmCharacterSubAnimType
{
	AGCMCHAR_SUBANIM_TYPE_NONE = -1,
	AGCMCHAR_SUBANIM_TYPE_SPINE = 0,
	AGCMCHAR_MAX_SUBANIM_TYPE
};

enum AgcmCharacterSubAnimNodeIndex
{
	AGCMCHAR_SUBANIM_NODEINDEX_ERROR	= -1,
	AGCMCHAR_SUBANIM_NODEINDEX_SPINE	= 12
};

enum AgcmCharacterStartAnimCallbackPoint
{
	AGCMCHAR_START_ANIM_CB_POINT_DEFAULT = 0,
	AGCMCHAR_START_ANIM_CB_POINT_BLEND,
	AGCMCHAR_START_ANIM_CB_POINT_ANIMEND,
	AGCMCHAR_START_ANIM_CB_POINT_ANIMENDTHENCHANGE
};

enum AgcmCharacterStatus
{
	AGCMCHAR_STATUS_FLAG_NONE					= 0x0000,
	AGCMCHAR_STATUS_FLAG_INIT_HIERARCHY			= 0x0001,
	AGCMCHAR_STATUS_FLAG_LOAD_CLUMP				= 0x0002,
	AGCMCHAR_STATUS_FLAG_EQUIP_DEFAULT_ARMOUR	= 0x0004,
	AGCMCHAR_STATUS_FLAG_ADDED_WORLD			= 0x0010,
	AGCMCHAR_STATUS_FLAG_INIT_COMPLETE			= 0x0008,
	AGCMCHAR_STATUS_FLAG_REMOVED				= 0x0020,
	AGCMCHAR_STATUS_FLAG_ATTACHED_HIERARCHY		= 0x0040,
	AGCMCHAR_STATUS_FLAG_NEW_CREATE				= 0x0080,
	AGCMCHAR_STATUS_FLAG_START_ANIM				= 0x0100
};

enum AgcmCharacterAnimCBDataArray
{
	AGCMCHAR_ACDA_CHARACTER_ID	= 0,
	AGCMCHAR_ACDA_ANIM_TYPE,
	AGCMCHAR_ACDA_START_ANIM_CB_POINT,
	AGCMCHAR_AGCD_CUST_CALLBACK,
	AGCMCHAR_ACDA_CUST_ANIM_TYPE,
	AGCMCHAR_AGCD_INIT_ANIM,
	AGCMCHAR_MAX_ACDA
};

enum AgcmCharacterAnimFlags
{
	AGCMCHAR_AF_NONE		= 0x0000,
	AGCMCHAR_AF_BLEND		= 0x0001,
	AGCMCHAR_AF_BLEND_EX	= 0X0002,
	AGCMCHAR_AF_NO_UNEQUIP	= 0X0004,
};

enum AgcmCharacterAnimCustFlags
{
	AGCMCHAR_ACF_NONE					= 0x0000,
	AGCMCHAR_ACF_LOCK_LINK_ANIM			= 0x0001,
	AGCMCHAR_ACF_CLUMP_SHOW_START_TIME	= 0x0002,
	AGCMCHAR_ACF_CLUMP_HIDE_END_TIME	= 0x0004
};

enum AgcmCharacterTransparentType
{
	AGCMCHAR_TRANSPARENT_NONE = 0,
	AGCMCHAR_TRANSPARENT_HALF,
	AGCMCHAR_TRANSPARENT_FULL,
};

typedef struct _AgcmCharacterActionQueueData {
	AgpdCharacterActionType			eActionType;				// action type
	AgpdCharacterActionResultType	eActionResultType;			// action �� ���� ���
	INT32							lActorID;					// actor �Ƶ�
	INT32							lSkillTID;					// eActionResultType�� ��ų�� ���� ���� ��� �ش� ��ų TID
	AgpdFactor						csFactorDamage;				// AgpdFactorCharPoint. �������� ����ִ�.
	BOOL							bIsSetPacketFactor;
	CHAR*							pszPacketFactor;			// FactorPacket �� ���� �Ҵ�. 2004.11.30. steeple
	UINT32							ulProcessTime;				// �� �׼��� ó���ؾ� �ϴ� �ð�
	BOOL							bDeath;						// �׾��ٴ� ��������.. ����..
	INT32							lNumDivideForShow;			// �� Factor�� ����� ���� �������� ����
	UINT32							ulDivideIntervalMSec;		// �� ���㸦 ������� �ϴ� ���͹�
	BOOL							bIsNowUpdate;				// ������ ������ �ٷ� �ؾ��ϴ��� üũ. 2005.03.14. steeple
	UINT32							ulAdditionalEffect;			// �߰� ȿ��. 2005.12.14. steeple
} AgcmCharacterActionQueueData,		*pAgcmCharacterActionQueueData;


struct AgcdPreengagedEventEffect
{
	AgpdCharacter*	pcsAgpdTarget;

	INT32			lBuffedTID;
	INT32			lCasterTID;

	INT32			lCommonEffectID;	
};

class ACA_AttachedData
{
public:	
	INT8				m_nCustType;
	UINT8				m_unActiveRate;
	UINT16				m_unCustFlags;
	CHAR				*m_pszPoint;
	AgcdAnimData2		*m_pcsBlendingData;
	AgcdAnimData2		*m_pcsSubData;

	INT32				m_lClumpShowOffsetTime;
	UINT32				m_uClumpFadeInTime;

	INT32				m_lClumpHideOffsetTime;
	UINT32				m_uClumpFadeOutTime;

	ACA_AttachedData()
	{
		m_nCustType			= -1;
		m_unActiveRate		= 0;
		m_unCustFlags		= 0;
		m_pszPoint			= NULL;		
		m_pcsBlendingData	= NULL;
		m_pcsSubData		= NULL;

		m_lClumpShowOffsetTime	= 0;
		m_uClumpFadeInTime		= 0;

		m_lClumpHideOffsetTime	= 0;
		m_uClumpFadeOutTime		= 0;
	}
};

class AgcdCharacterAnimation
{
public:
	AgcdAnimation2			*m_pcsAnimation;
	AgcdAnimationFlag		*m_pstAnimFlags;

	AgcdCharacterAnimation()
	{
		m_pcsAnimation	= NULL;
		m_pstAnimFlags	= NULL;
	}
};

class AgcdCharacterDefaultHeadTemplate
{
public:
	AgcdCharacterDefaultHeadTemplate()
	{
		m_vtxNearCamera.x = m_vtxNearCamera.y = m_vtxNearCamera.z = 0.f;
		m_vtxFarCamera.x = m_vtxFarCamera.y = m_vtxFarCamera.z = 0.f;
	}
	~AgcdCharacterDefaultHeadTemplate()
	{
	}

	vector< string > vecFaceInfo;
	vector< string > vecHairInfo;

	AgcdClumpRenderTypeData		m_HairRenderType;
	AgcdClumpRenderTypeData		m_FaceRenderType;

	RwV3d			m_vtxNearCamera, m_vtxFarCamera;

	int	GetFaceNum() { return ( int ) vecFaceInfo.size(); }
	int	GetHairNum() { return ( int ) vecHairInfo.size(); }
};

class AgcdPreCharacterTemplate
{
public:
	CHAR		*m_pszLabel;

	CHAR		*m_pszDFFName;
	CHAR		*m_pszDADFFName;
	CHAR		*m_pszPickDFFName;

	RwRGBA		m_stPreLight;

	AgcdPreCharacterTemplate()
	{
		m_pszLabel			= NULL;
		m_pszDFFName		= NULL;
		m_pszDADFFName		= NULL;
		m_pszPickDFFName	= NULL;

		m_stPreLight.alpha	= 255;
		m_stPreLight.red	= 127;
		m_stPreLight.green	= 127;
		m_stPreLight.blue	= 127;
	}
};

class AgcdCharacterTemplate
{
public:
	AgcdPreCharacterTemplate*			m_pcsPreData;
	AgcdCharacterDefaultHeadTemplate*	m_pcsDefaultHeadData;

	RpClump*							m_pClump;
	RpClump*							m_pDefaultArmourClump;
	RpAtomic*							m_pPickingAtomic;

	//customize
	INT32								m_nHairNum;
	INT32								m_nFaceNum;
	std::vector< RpAtomic* >			m_vpHair; //��� ����Ʈ ����.
	std::vector< RpAtomic* >			m_vpFace; //�� ����Ʈ ����.

	RwSphere							m_stBSphere;

	INT32								m_lAnimType2;
	AgcdCharacterAnimation***			m_pacsAnimationData;

	RwV3d								m_stScale;
	FLOAT								m_fScale;
	INT32								m_lHeight;
	INT32								m_lRiderHeight;
	INT32								m_lObjectType;

	//. 2005. 10. 07
	//. ID��� TextBoard���� ����ϴ� �߰� depth��.
	FLOAT								m_fDepth;

	AgcdLOD								m_stLOD;

	AgcdClumpRenderType					m_csClumpRenderType;

	INT32								m_lRefCount;
	BOOL								m_bLoaded;

	// item,character�� (occluder���� �����Ƿ� �޸� �Ƴ���)
	OcTreeRenderData2					m_stOcTreeData;

	//@{ 2006/02/20 burumal 	
	//INT32								m_lPickingNodeIndex;	// Picking Atomic ���� Hierarchy Node Index

	// Picking Atomic ���� Hierarchy Node Index
	// D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP �� ���ǵ� ���� ���� ��ġ��ų��
	INT32								m_lPickingNodeIndex[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
	//@}

	//@{ 2006/03/24 burumal
	BOOL								m_bTagging;
	//@}

	//@{ 2006/05/03 burumal
	INT32								m_nDNF;
	//@}

	//@{ 2006/08/22 burumal
	UINT								m_lLookAtNode;
	//@}

	//@{ 2006/08/28 burumal
	BOOL								m_bUseBending;
	float								m_fBendingFactor;
	float								m_fBendingDegree;
	//@}	

	//@{ 2006/12/15 burumal
	BOOL								m_bNonPickingType;
	//@}
	
	INT32								m_lFaceAtomicIndex;		// �� Atomic Index

	FLOAT								m_afDefaultAnimSpeedRate[AGCMCHAR_MAX_ANIM_TYPE];

protected:
	UINT64								m_ulExportedID;	// 20��Ʈ�� �� ���̵� ����������. (�⺻ Ŭ���� ���̵� | �⺻ ���� Ŭ���� ���̵� | ��ŷ�� ����� ���̵�)

public:
	inline VOID							SetExportedID(UINT64 ulExportedID)	{m_ulExportedID	= ulExportedID;}
	inline VOID							SetClumpID(UINT32 ulExportedID)		{m_ulExportedID	|= ((UINT64)(ulExportedID) << 40);}
	inline VOID							SetDefArmourID(UINT32 ulExportedID)	{m_ulExportedID	|= ((UINT64)(ulExportedID) << 20);}
	inline VOID							SetPickClumpID(UINT32 ulExportedID)	{m_ulExportedID	|= ulExportedID;}
	inline UINT32						GetClumpID()						{return (UINT32)(m_ulExportedID >> 40);}
	inline UINT32						GetDefArmourID()					{return (UINT32)((m_ulExportedID & 0xfffff00000) >> 20);}
	inline UINT32						GetPickClumpID()					{return (UINT32)(m_ulExportedID & 0xfffff);}
	inline VOID							GetClumpName(CHAR *pszName)
	{
		sprintf(
			pszName,
			"%s%.*x.%s",
			AC_EXPORT_HD_CHAR_CHARACTER,
			AC_EXPORT_ID_LENGTH,
			GetClumpID(),
			AC_EXPORT_EXT_CLUMP				);
	}
	inline VOID		GetDefArmourName(CHAR *pszName)
	{
		sprintf(
			pszName,
			"%s%.*x.%s",
			AC_EXPORT_HD_CHAR_DEF_ARMOUR,
			AC_EXPORT_ID_LENGTH,
			GetDefArmourID(),
			AC_EXPORT_EXT_CLUMP				);
	}
	inline VOID		GetPickClumpName(CHAR *pszName)
	{
		sprintf(
			pszName,
			"%s%.*x.%s",
			AC_EXPORT_HD_CHAR_CHAR_PICKING_DATA,
			AC_EXPORT_ID_LENGTH,
			GetPickClumpID(),
			AC_EXPORT_EXT_CLUMP				);
	}
	inline VOID		GetCustomizeHairName(CHAR *pszName, int nID, int nIdx )
	{
		sprintf(
			pszName,
			"%s%.*x.%s",
			AC_EXPORT_HD_CHAR_CUSTOMIZE,
			AC_EXPORT_ID_LENGTH,
			(nID*0x100)+nIdx,
			AC_EXPORT_EXT_CLUMP				);
	}
	inline VOID		GetCustomizeFaceName(CHAR *pszName, int nID, int nIdx )
	{
		sprintf(
			pszName,
			"%s%.*x.%s",
			AC_EXPORT_HD_CHAR_CUSTOMIZE,
			AC_EXPORT_ID_LENGTH,
			(nID*0x200)+nIdx,
			AC_EXPORT_EXT_CLUMP				);
	}

	BOOL	IsSkipExport( ApServiceArea eArea )
	{
		return m_nDNF & GETSERVICEAREAFLAG( eArea );
	}

	AgcdCharacterAnimation* GetAnimation( AgcmCharacterAnimType eType , INT32 lCustAnimType2 );
};

class AgcdCharSubAnimInfo
{
public:
	RpHAnimHierarchy	*m_pInHierarchy;
	RpHAnimHierarchy	*m_pInHierarchy2;
	RpHAnimHierarchy	*m_pOutHierarchy;

	BOOL				m_bStopMainAnimation;

	UINT32				m_ulCurrentTime;
	UINT32				m_ulDuration;

	FLOAT				m_fForceTime;

	AgcdCharSubAnimInfo()
	{
		m_pInHierarchy	= NULL;
		m_pInHierarchy2	= NULL;
		m_pOutHierarchy	= NULL;
	}

	inline VOID InitComponent()
	{
		m_bStopMainAnimation	= FALSE;

		m_ulCurrentTime			= 0;
		m_ulDuration			= 0;

		m_fForceTime			= -0.1f;
	}

	inline VOID SetComponent(BOOL bStopMainAnimation, UINT32 ulDuration, FLOAT fForceTime = -0.1f)
	{
		m_bStopMainAnimation	= bStopMainAnimation;

		m_ulCurrentTime			= 0;
		m_ulDuration			= ulDuration;

		m_fForceTime			= fForceTime;
	}
};

struct AgcdCharacter
{
	///////////////////////////////////
	// Default.
	PVOID								m_pvClass;
	PVOID								m_pvTextboard;
	AgcdCharacterTemplate*				m_pstAgcdCharacterTemplate;
	RpClump*							m_pClump;

// customize
	RpAtomic*							m_pHair;
	RpAtomic*							m_pFace;
	INT32								m_nAttachHairID;
	INT32								m_nAttachFaceID;
	BOOL								m_bAttachableHair;	//WithHair�� false�� head�� �����Ǿ��ִ�.
	BOOL								m_bAttachableFace;	//WithFace�� false�� head�� �����Ǿ��ִ�.

// view helmet
	BOOL								m_bViewHelmet;		//�� flag �� false�̸� �׻� helmet�� ���õȴ�.
	BOOL								m_bEquipHelmet;		//Helmet�� �����Ǿ��°�.

// ride
	INT32								m_nRideCID;
	AgcdCharacter*						m_pRide;
	RwFrame*							m_pRideRevisionFrame;
	AgcdCharacter*						m_pOwner;
	///////////////////////////////////

	///////////////////////////////////
	// Picking-Info
	RpAtomic							*m_pPickAtomic;

	//@{ 2006/03/08 burumal
	// m_pPickAtomic ���� lock�ɶ� �ٲ�� �ִ�. multi picking atomic���� ����list�� �����ǹǷ� ��ҿ��� 
	// ��������� ���� �����ϴ��� �ƹ� ������ ������ first pick atomic �� bounding sphere�� offset ó����
	// �ϴºκ��� �����Ƿ� �ݵ�� ù��° pick atomic�� �˰� �־�� �ϹǷ� �Ʒ� ������ �����صε��� ����
	// 
	RpAtomic*							m_pOrgPickAtomic;
	//@}
	///////////////////////////////////

	///////////////////////////////////
	// Hierarchy
	RpHAnimHierarchy					*m_pInHierarchy;
	RpHAnimHierarchy					*m_pInHierarchy2;
	RpHAnimHierarchy					*m_pOutHierarchy;
	///////////////////////////////////

	///////////////////////////////////
	// Sub-Animation�� �ʿ��� ����
	AgcdCharSubAnimInfo					m_csSubAnim;
	///////////////////////////////////

	///////////////////////////////////
	// �⺻ animation�� �ʿ��� ����
	BOOL								m_bStop;
	UINT32								m_ulAnimationFlags;
	BOOL								m_bForceAnimation;		// added by netong
//	UINT32								m_ulBlendingTime;
	UINT32								m_ulPrevTime;			// add by gemani

	
	// 2005. 8. 18 Nonstopdj
	// Wave Fx ���۽ð��� ����
	UINT32								m_ulStartWaveFxTime;

	AgcdAnimData2						*m_pcsCurAnimData;
	AgcmCharacterAnimType				m_eCurAnimType;
	INT32								m_lCurAnimType2;

	AgcdAnimData2						*m_pcsNextAnimData;

	AgcdAnimationTime					m_csAnimation;

	BOOL								m_bSetBlendingHierarchy;
	///////////////////////////////////

	///////////////////////////////////
	// Struck-Animation�� �ʿ��� ����
	INT32								m_lAttackerID;
	///////////////////////////////////

	FLOAT								m_afAnimSpeedRate[AGCMCHAR_MAX_ANIM_TYPE];
	FLOAT								m_fSkillSpeedRate;

	///////////////////////////////////
	// Animation-Speed
	FLOAT								m_afDefaultAnimSpeedRate[AGCMCHAR_MAX_ANIM_TYPE];
//	FLOAT								m_fLinkAnimSpeed;
	///////////////////////////////////

	///////////////////////////////////
	//Update�� �ʿ䰡 �ִ��� �˾ƺ���. ������ False. Update�� �ʿ��� ������ ����� True. Update�� �ϸ� �ٽ� False
   	BOOL								m_bUpadate;
	///////////////////////////////////

	///////////////////////////////////
	// DP ID for Stress Test
	INT16								m_ulNID;
	///////////////////////////////////

	///////////////////////////////////
	// Status
	INT32								m_lStatus;
	///////////////////////////////////

	///////////////////////////////////
	// ���ݻ���üũ(���ݴ���...)
	INT32								m_lLastAttackTime;
	///////////////////////////////////

	UINT32								m_ulNextAttackTime;
	UINT32								m_ulLastSendAttackTime;
	UINT32								m_ulNextActionSelfCharacterTime;

	BOOL								m_bIsLockTarget;
	INT32								m_lLockTargetID;
	BOOL								m_bIsForceAttack;
	BOOL								m_bIsCastSkill;
	INT32								m_lLastCastSkillTID;	// 2005.08.24. steeple. TID �� ����.	// 2005.06.01. steeple

	INT32								m_lNumActionData;
	BOOL								m_bQueuedDeathEvent;
	AgcmCharacterActionQueueData		m_astActionQueue[AGCD_CHARACTER_MAX_ACTION_QUEUE_SIZE];

	BOOL								m_bIsCombo;

	// Shadow ���� �ȿ� �ִ��� ���� (add by gemani.. 04.06.21)
	BOOL								m_bInShadow;		

	AgpdCharacterAction					m_stNextAction;

	INT32								m_lSelectTargetID;

	DWORD								m_dwCharIDColor;			// PvP ��. 2005.01.19. steeple

	FLOAT								m_fLastWinkTime;
	FLOAT								m_fNextWinkTime;

	RwV3d								m_vOffsetBSphere;

	// D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP �� ���ǵ� ���� ���� ��ġ��ų��
	RwFrame *							m_pstNodeFrame[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
	//@}
	
	BOOL								m_bTransforming;			// ������?

	INT8								m_cTransparentType;			// 2005.09.09. steeple

	stAgpmSkillActionData *				m_pstSkillActionData;

	FLOAT								m_fLastBendingDegree;

	INT8								m_nPreengagedEventEffectCount;
	AgcdPreengagedEventEffect*			m_pPreengagedEventEffect[AGCD_CHARACTER_MAX_PREENGAGED_EVENT_EFFECT];


	AuCharacterLightInfo *				GetLightInfo( INT32 nPart )
	{
		if( nPart < AGPMITEM_PART_V_BODY )	return &m_aLightInfo[ nPart ];
		else								return NULL;
	}
	AuCharacterLightInfo				m_aLightInfo[ AGPMITEM_PART_V_BODY ];
										// AGPMITEM_PART_V_BODY�� �ƹ�Ÿ �ε����� ���� ó���̹Ƿ�..
										// ����� �迭�� ��´�.

	inline INT32	GetHeight()
	{
		INT		nReturnValue	=	0;
		if( m_nRideCID && m_pRide )
		{
			if( m_pRide->m_pstAgcdCharacterTemplate )
				nReturnValue	+=	m_pRide->m_pstAgcdCharacterTemplate->m_lRiderHeight;
		}

		if( m_pstAgcdCharacterTemplate )
			nReturnValue	+=	m_pstAgcdCharacterTemplate->m_lHeight;

		return nReturnValue;
	}

	inline FLOAT	GetDepth()
	{
		FLOAT	nReturnValue	=	0;
		if( m_nRideCID && m_pRide )
		{
			if( m_pRide->m_pstAgcdCharacterTemplate )
				return m_pRide->m_pstAgcdCharacterTemplate->m_fDepth;
		}

		if( m_pstAgcdCharacterTemplate )
			return m_pstAgcdCharacterTemplate->m_fDepth;

		return nReturnValue;
	}

	AgcdCharacter( void )
	{
		m_pstAgcdCharacterTemplate = NULL;

		m_pvClass			=	NULL;
		m_pvTextboard		=	NULL;
		m_pClump			=	NULL;

		m_pHair				=	NULL;
		m_pFace				=	NULL;
		m_nAttachHairID		=	-1;
		m_nAttachFaceID		=	-1;
		m_bAttachableHair	=	TRUE;
		m_bAttachableFace	=	TRUE;

		m_bViewHelmet		=	TRUE;
		m_bEquipHelmet		=	TRUE;
		m_ulPrevTime		=	0;

		m_pRide				=	NULL;
	}
};

inline UINT8 CalcPickAtomicIndex(RpAtomic* pCur, RpAtomic* pOrg)
{	
	if ( !pCur || (pCur == pOrg) || (pOrg->next == pOrg) )		return 0;

	UINT8 uIdx = 0;

	do {
		pCur = pCur->next;
		uIdx++;
	} while ( pCur != pOrg );

	return uIdx;
}

inline UINT8 CalcPickAtomicIndex(AgcdCharacter* pChar)
{
	return pChar ? CalcPickAtomicIndex(pChar->m_pPickAtomic, pChar->m_pOrgPickAtomic) : 0;
}

//@}

#endif     //_AGCDCLIENTCHARACTER_H_