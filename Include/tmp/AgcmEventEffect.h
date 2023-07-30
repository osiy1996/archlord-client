#ifndef __AGCMEVENTEFFECT_H__
#define __AGCMEVENTEFFECT_H__

#include "AgcModule.h"
#include "AcDefine.h"
#include "ApmEventManager.h"
#include "ApmMap.h"
#include "AgpmTimer.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgcmItem.h"
#include "AgpmSkill.h"
#include "AgcmSkill.h"
#include "AgcmEff2.h"

#include "AgcdEventEffect.h"
#include "AgcmEventEffectList.h"
#include "AgcuEventEffect.h"
#include "AgcaEffectData.h"
#include "AgcmEventEffectData.h"
#include "AgcmSound.h"

#include <vector>

class AgpmArchlord;
class AgcmCharacter;
class AgcmEventNature;

#define AGCMEVENTEFFECT_MONO_NAME_HEADER									"M"
#define AGCMEVENTEFFECT_EFFECT_SOUND_PATH									"SOUND\\EFFECT\\"
#define AGCMEVENTEFFECT_EFFECT_MONO_SOUND_PATH								"SOUND\\EFFECT\\"
#define AGCMEVENTEFFECT_CUSTOM_FLAGS_INI_NAME								"AGCMEVENTEFFECT_CUSTOM_FLAGS"
#define AGCMEVENTEFFECT_CONDITION_FLAGS_INI_NAME							"AGCMEVENTEFFECT_CONDITION_FLAGS"
#define AGCMEVENTEFFECT_SS_CONDITION_FLAGS_INI_NAME							"AGCMEVENTEFFECT_SS_CONDITION_FLAGS"
#define AGCMEVENTEFFECT_STATUS_FLAGS_INI_NAME								"AGCMEVENTEFFECT_STATUS_FLAGS"
#define AGCMEVENTEFFECT_EFFECT_DATA_INI_NAME								"AGCMEVENTEFFECT_EFFECT_DATA"
#define AGCMEVENTEFFECT_EFFECT_ROTATION_INI_NAME							"AGCMEVENTEFFECT_ROTATION_DATA"
#define AGCMEVENTEFFECT_SOUND_DATA_INI_NAME									"AGCMEVENTEFFECT_SOUND_DATA"
#define AGCMEVENTEFFECT_EFFECT_CUST_DATA_INI_NAME							"AGCMEVENTEFFECT_CUST_DATA"
#define AGCMEVENTEFFECT_ANIMATION_DATA_INI_NAME								"AGCMEVENTEFFECT_ANIMATION_DATA"
#define AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_INI_NAME				"AGCMEVENTEFFECT_CAAD_SOUND_DATA"
#define AGCMEVENTEFFECT_CHAR_ANIM_ATTACHED_DATA_SOUND_CONDITION_INI_NAME	"AGCMEVENTEFFECT_CAAD_SOUND_CONDITION_FLAGS"
#define AGCMEVENTEFFECT_SOUND_LOOP_COUNT			0
#define AGCMEVENTEFFECT_CONVERTED_ITEM_G_OFFSET		333
#define AGCMEVENTEFFECT_DIST_TO_GAP_OFFSET			1000.0f
#define AGCMEVENTEFFECT_BOUNDARY_3D_SOUND			5000.0f
#define AGCMEVENTEFFECT_NUM_CHECK_TIME				24

#define AGCMEVENTEFFECT_SOCIALSOUND_SETTING_FILE	"ini\\socialsound.ini"
#define	AGCMEVENTEFFECT_SOCIALSOUNDFILEFORMAT		"A%c%04d.wav"
	// ex> AA0000.wav .. �̷�����.
	// %c ���� Ŭ���� �ε���.

enum eAgcmEventEffectStatus
{
	E_AEE_STATUS_NONE				= 0x0000,
	E_AEE_STATUS_OFF_				= 0x0001,
};

enum eAgcmEventEffectIntervalAnimParams
{
	AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_PARENT_MODULE	= 0,
	AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_SOUND,
	AGCM_EVENT_EFFECT_INTERVAL_ANIM_PARAMS_CLUMP,
};

typedef BOOL (*AgcmEventEffectParamFunction)(PVOID pvClass, PVOID pvData1, PVOID pvData2);

enum eAgcmEventEffectDataType
{
	AGCMEVENT_EFFECT_DATA_TYPE_CHAR_TEMPLATE = 0,
};

class AgcmSEEPStandardObject
{
public:
	AgcmSEEPStandardObject()
	{
		m_pcsBase		= NULL;
		m_pcsHierarchy	= NULL;
		m_pcsFrame		= NULL;
		m_pcsClump		= NULL;
		m_pcsAtomic		= NULL;
		ZeroMemory( &m_stPosition, sizeof(m_stPosition) );
	}

	ApBase*				m_pcsBase;
	RpHAnimHierarchy*	m_pcsHierarchy;
	RwFrame*			m_pcsFrame;
	RpClump*			m_pcsClump;
	RpAtomic*			m_pcsAtomic;
	AuPOS				m_stPosition;
};

class AgcmStartEventEffectParams
{
public:
	enum
	{
		E_ASEEP_NONE			= 0x0000,
		E_ASEEP_SKIP_PLAY_SOUND	= 0x0001
	};

	AgcmStartEventEffectParams()
	{
		m_ulConditionFlags			= 0;
		m_pstAgcdUseEffectSet		= NULL;
		
		m_bSelfCharacter			= FALSE;
		m_ulFlags					= E_ASEEP_NONE;
		m_lExceptionType			= 0;
		m_lExceptionTypeCustData	= 0;
		m_lLife						= -1;
		m_fRange					= 0.0f;
		m_fScale					= 1.0f;
		m_fTimeScale				= 1.0f;

		m_pfDestroyMatchCB			= NULL;

		m_lCID						= 0;
		m_lTCID						= 0;
		m_lCustID					= 0;
		m_pfCallback				= NULL;
		m_pvClass					= NULL;

		m_pcsNodeInfo				= NULL;

		m_eType						= APBASE_MAX_TYPE;
	}

	virtual	~AgcmStartEventEffectParams()
	{
		AES_EFFCTRL_SET_LIST	*pcsNext	= NULL;
		AES_EFFCTRL_SET_LIST	*pcsCurrent	= m_pcsNodeInfo;
		while (pcsCurrent)
		{
			pcsNext							= pcsCurrent->m_pcsNext;
			delete pcsCurrent;
			pcsCurrent						= pcsNext;
		}
	}

	UINT32								m_ulConditionFlags;
	AgcdUseEffectSet*					m_pstAgcdUseEffectSet;

	AgcmSEEPStandardObject				m_csOrigin;
	AgcmSEEPStandardObject				m_csTarget;
	AgcmSEEPStandardObject				m_csPreTarget;

	BOOL								m_bSelfCharacter;
	UINT32								m_ulFlags;
	INT32								m_lExceptionType;
	INT32								m_lExceptionTypeCustData;
	INT32								m_lLife;
	FLOAT								m_fRange;
	FLOAT								m_fScale;
	FLOAT								m_fTimeScale;

	AgcmEffectNoticeEffectProcessCB		m_pfDestroyMatchCB;

	INT32								m_lCID;
	INT32								m_lTCID;
	INT32								m_lCustID;
	AgcmEffectNoticeEffectProcessCB		m_pfCallback;
	PVOID								m_pvClass;

	AES_EFFCTRL_SET_LIST				*m_pcsNodeInfo;

	ApBaseType							m_eType;

};

//. 2005. 10. 06. Nonstopdj
//. ���ȭ flag���� ����ü
//. agpdcharacter�� ������ �߰����� �ʰ� ��Ÿ�ӿ� attach�ؼ� ���..���������� ��õ~! 
//. AgcmEventEffect�� ������ Effect ���� �Լ����� ���ǰ˻��� ���� ����Ѵ�.
//. �������� �ִϸ��̼�(walk, run..)�� ���� �ٸ� �Ӽ����� ������ ���� �Բ� ����ϱ� ���� �������
//. �ᱹ �ܼ��� skyset�� ���¸� ĳ���ʹ� �����ϰ� �ֱ� ���� ����.

#define AGPMCHAR_CHARACTER_SKYSET_STATUS_MAX_NUM	3	//�������� �����..������´� NOMAL, COLD, UNKNOWN�� ����

struct ACharacterSkyFlags
{
	enum	ASkyStatus 
	{
		AGPMCHAR_CHARACTER_SKYSET_NORMAL = 0,
		AGPMCHAR_CHARACTER_SKYSET_COLD,
		AGPMCHAR_CHARACTER_SKYSET_UNKNOWN
		//. ������ Effec dialog���� ���Ǵ� NatureCondition = 0~ 
		//. ������ ���� ������ enum���� ���⿡ Sky Status �߰�
	};

	ACharacterSkyFlags() : m_Flags(AGPMCHAR_CHARACTER_SKYSET_NORMAL)
	{
	}

	UINT32		m_Flags;
};

class AgcmEventEffect;
class AgcmEventEffectExclusiveEffectMgr
{
	friend class AgcmEventEffect;	

	class CustomIntVector : public ApMemory<CustomIntVector, 1000>
	{
	public :
		CustomIntVector() {};

	public :
		std::vector<RwInt32>		m_vecArray;
	};

public :
	AgcmEventEffectExclusiveEffectMgr()		{		};

	void	Clear();
	void	Relase();

	RwBool	FindDominance( RwInt32 dwNewDominance );
	RwBool	FindDominance( RwInt32 dwNewDominance, CustomIntVector* pDominanceTable );

protected :
	std::vector<RwInt32>					m_vecDominanceTable;
	std::map<RwInt32, CustomIntVector*>		m_mapRecessivenessToDominanceTable;
	std::map<RpClump*, CustomIntVector*>	m_mapPerClumpDominances;
};

class AgcmEventEffect : public AgcModule
{
public:
	static AgcmEventEffect*	m_pcsThisAgcmEventEffect;

	ApmEventManager*		m_pcsApmEventManager;
	ApmMap*					m_pcsApmMap;
	AgpmTimer*				m_pcsAgpmTimer;
	AgpmFactors*			m_pcsAgpmFactors;
	ApmObject*				m_pcsApmObject;
	AgcmObject*				m_pcsAgcmObject;
	AgpmCharacter*			m_pcsAgpmCharacter;
	AgcmCharacter*			m_pcsAgcmCharacter;
	AgpmItem*				m_pcsAgpmItem;
	AgpmItemConvert*		m_pcsAgpmItemConvert;
	AgcmItem*				m_pcsAgcmItem;
	AgpmSkill*				m_pcsAgpmSkill;
	AgcmSkill*				m_pcsAgcmSkill;
	AgcmEff2*				m_pcsAgcmEff2;
	AgcmRender*				m_pcsAgcmRender;
	AgcmSound*				m_pcsAgcmSound;
	AgpmArchlord*			m_pcsAgpmArchlord;
	AgcmEventNature*		m_pcsEventNature;
	AgcmResourceLoader*		m_pcsAgcmResourceLoader;

	INT32					m_lLoaderLoadID;
	AgcmEventEffectData		m_csAgcmEventEffectData;

protected:
	INT16								m_nAttachedIndexObjEffectList;
	INT16								m_nAttachedIndexCharEffectList;
	INT16								m_nAttachedIndexItemEffectList;
	INT16								m_nAttachedIndexObjUseEffectSet;
	INT16								m_nAttachedIndexCharUseEffectSet;
	INT16								m_nAttachedIndexItemUseEffectSet;

	INT16								m_nAttachedIndexCharEventEffect;
	INT16								m_nAttachedIndexItemEventEffect;

	INT32								m_lSelfCID;
	UINT32								m_ulNowClockCount;

	UINT32								m_ulStatus;

	BOOL								m_bForceStereo;

	AgcdEventEffectCheckTimeDataList	*m_pacsCheckTimeDataList[E_AGCD_EVENT_EFFECT_MAX_CHECK_TIME_ON_OFF][AGCMEVENTEFFECT_NUM_CHECK_TIME];

	AgcaEffectData2						m_csEffectDataAdmin2;

	std::vector<RwInt32>				m_container_additionalHitEff;

	INT16								m_nSkyFlagOffset;
	AgcmEventEffectExclusiveEffectMgr	m_ExclusiveEffectsMgr;

	BOOL								m_bCastingEffectDisabled;
	BOOL								m_bSkillEffectDisabled;
	
public:
	AgcmEventEffect();
	virtual ~AgcmEventEffect();	

	// �ʼ� �Լ���...
	BOOL	OnInit();
	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

	// �ʱ�ȭ�� ȣ���ؾ� ��!
	VOID	SetSoundDataNum(INT32 lNum);

	// AgcmEventEffect ���� ����.
	VOID	SetStatus(UINT32 ulStatus);

	// ���� ��⿡ attach�� ������ ��������...
	AgcmEventEffectList*	GetEventEffectList(ApBase *pcsBase);
	AgcdUseEffectSet*		GetUseEffectSet(ApBase *pcsBase);
	AgcdUseEffectSet*		GetUseEffectSet(ApdObjectTemplate *pcsTemplate);
	AgcdUseEffectSet*		GetUseEffectSet(AgpdCharacterTemplate *pcsTemplate);
	AgcdUseEffectSet*		GetUseEffectSet(AgpdItemTemplate *pcsTemplate);
	void*					GetEventEffectData(ApBase *pcsBase);

	BOOL	UpdateNatureStatus(const INT16 nSkyOption);

	// Ŭ���̾�Ʈ ���ΰ��� ĳ���� �Ƶ�...
	INT32	GetSelfCID()					{	return m_lSelfCID;	}
	VOID	SetSelfCID(INT32 lCID)			{	m_lSelfCID = lCID;	}
	BOOL	CastingEffectDisabled()			{	return m_bCastingEffectDisabled;	}
	VOID	DisableCastingEffect(BOOL bVal)	{	m_bCastingEffectDisabled = bVal;	}
	BOOL	SkillEffectDisabled()			{	return m_bSkillEffectDisabled;		}
	VOID	DisableSkillEffect(BOOL bVal)	{	m_bSkillEffectDisabled = bVal;		}

	VOID	SetForceStereoSound()			{	m_bForceStereo = TRUE;	}
	
	// �ܺο��� ����Ʈ ����
	BOOL	SetCommonCharacterEffect(ApBase *pcsBase, eAgcmEventEffectDataCommonCharType eCommonCharType, INT32 lTargetCID = 0);
	BOOL	RemoveCommonCharacterEffect(ApBase *pcsBase, eAgcmEventEffectDataCommonCharType eCommonCharType);

	BOOL	StartSocialAnimation(AgpdCharacter *pstAgpdCharacter, AgpdCharacterSocialType eSocialType, AgcdCharacter *pstAgcdCharacter = NULL, AcCallbackData3 pfCustCallback = NULL);
	BOOL	StartSelectionAnimation(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter = NULL, AcCallbackData3 pfCustCallback = NULL, BOOL bGoBack = FALSE);

	VOID	ForXport_GetAllSocialAnim(std::vector<AgcdAnimData2*>& container)
	{
		m_csAgcmEventEffectData.ForXport_GetAllSocialAnim(container);
	}

	VOID	RemoveAllSocialAnimation()
	{
		m_csAgcmEventEffectData.RemoveAllSocialAnimation();
	}

	// Stream���� �������� ���̴� �Լ���...
	BOOL	UseEffectSetStreamRead(ApModuleStream *pStream, AgcdUseEffectSet *pstAgcdUseEffectSet, AgcmEventEffectParamFunction pfParam = NULL, ApBase *pcsBase = NULL);
	BOOL	UseEffectSetStreamWrite(ApModuleStream *pStream, AgcdUseEffectSet *pstAgcdUseEffectSet);

	// table�� �д� �༮��~
	BOOL	ReadMovingSoundData(CHAR *szName, BOOL bDecryption);
	BOOL	ReadConvertedItemGradeEffectData(CHAR *szName, BOOL bDecryption);
	BOOL	ReadSpiritStoneHitEffectData(CHAR *szName, BOOL bDecryption);
	BOOL	ReadSpiritStoneAttrEffectData(CHAR *szName, BOOL bDecryption);
	BOOL	ReadCommonCharEffectData(CHAR *szName, BOOL bDecryption);
	BOOL	ReadSocialAnimationData(CHAR *szSocialPC, CHAR *szSocialGM, CHAR *szAnimationPath, BOOL bDecryption);
	BOOL	ReadConvertedArmourAttrEffectData(CHAR * szName, BOOL bDecryption);

	// UseEffectSet ����
	BOOL	AddCustData(CHAR *pszDestCustData, CHAR *pszAddCustData);
	INT32	GetCustDataIndex(CHAR szCustIndex);
	INT32	GetCustDataIndex(CHAR *szCustData, INT32 *palIndex);
	INT32	GetCustDataIndex(CHAR *szCustData, CHAR szCustIndex);

	BOOL	CallEventInitCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// �����ݹ� ȣ��(������ ���δ�.)
	BOOL	StreamReadExclusiveEffectMappingTable( RwChar* szFile, BOOL bDecryption );	
	VOID	CheckExclusiveEffect(RpClump* pClump, UINT32 dwEffID);
	BOOL	IsExclusiveEffect(RpClump* pClump, UINT32 dwEffID);
	VOID	ClearExclusiveEffectInfoPerFrame();

	AgcaEffectData2*		GetAgcaEffectData2()			{	return &m_csEffectDataAdmin2;	}
	BOOL	CopyCharAnimAttachedData(AgcdAnimation2 *pcsSrc, AgcdAnimation2 *pcsDest);

	BOOL	PreStop3DSound(ApBase *pcsBase);

	BOOL	StreamReadTemplate( CHAR *szFile, CHAR *pszErrorMessage = NULL, BOOL bDecryption = FALSE );
	BOOL	StreamWriteTemplate( CHAR *szFile, BOOL bEncryption );

	void	CharaterEquipEffectRemove( AgpdCharacter * pcsCharacter );
	void	CharaterEquipEffectUpdate( AgpdCharacter * pcsCharacter );

protected:
	// EventEffect �ߵ�~
	eAcReturnType	StartEffect( AgcdUseEffectSetData* pcsData, AgcmStartEventEffectParams* pcsParams, EFFCTRL_SET** ppstNode, BOOL bAddList = TRUE, AES_EFFCTRL_SET_LIST **ppCurrentNodeInfo = NULL, AgcmEventEffectParamFunction pfCheckSkipping = NULL, BOOL bSkillDestroyMatch = FALSE );
	BOOL			StartEventEffect( AgcmStartEventEffectParams *pcsParams, INT32 lForceIndex = -1, AgcmEventEffectParamFunction pfCheckSkipping = NULL, BOOL bSkillDestroyMatch = FALSE );

	BOOL	SetBaseEffectUseInfo(stEffUseInfo *pstInfo, AgcdUseEffectSetData *pstData, AgcmStartEventEffectParams *pcsParams);
	BOOL	SetStandardEffectUseInfo(stEffUseInfo *pstInfo, AgcmStartEventEffectParams *pcsParams, INT32 lNodeID, INT32 lTargetOption);
	eAcReturnType CheckCustomData(INT32 *alCustData, stEffUseInfo *pstInfo, AgcmStartEventEffectParams *pcsParams, BOOL bRepetition = FALSE);

	VOID	SetEffect( stEffUseInfo* pstInfo, AgcmEventEffectList* pstList, EFFCTRL_SET** ppstNode = NULL, BOOL bForceImmediate = TRUE);	

	BOOL	SetBaseEffectParams(ApdObject *pstApdObject, AgcmStartEventEffectParams *pcsParams);
	BOOL	SetBaseEffectParams(AgpdCharacter *pstAgpdCharacter, AgcmStartEventEffectParams *pcsParams);
	BOOL	SetBaseEffectParams(AgpdItem *pstAgpdItem, AgcmStartEventEffectParams *pcsParams);

	// ����� ���.
	BOOL	SetBaseEffect(ApBase *pcsBase, AgcdUseEffectSetData *pcsData, UINT32 ulLife = -1, BOOL bAddList = TRUE, EFFCTRL_SET **ppstNode = NULL);
	BOOL	SetBaseObjectEventEffect(ApdObject *pcsApdObject, UINT32 ulConditionFlags);
	BOOL	SetBaseCharacterEventEffect(AgpdCharacter *pcsAgpdCharacter, UINT32 ulConditionFlags);
	BOOL	SetBaseCharacterEventEffect(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, UINT32 ulConditionFlags);
	BOOL	SetBaseEquipItemEventEffect(AgpdItem *pcsAgpdItem, UINT32 ulConditionFlags, BOOL bSelfCharacter = FALSE, INT32 lLife = -1, ApBase *pcsTarget= NULL, RpClump *pstTargetClump = NULL, RwFrame *pstTargetFrame = NULL, RpHAnimHierarchy *pstTargetHierarchy = NULL, FLOAT fTimeScale = 1.0f);
	BOOL	SetBaseEquipItemEventEffect(AgpdItem *pcsAgpdItem, AgcdUseEffectSet *pstAgcdUseEffectSet, UINT32 ulConditionFlags, BOOL bSelfCharacter = FALSE, INT32 lLife = -1, ApBase *pcsTarget= NULL, RpClump *pstTargetClump = NULL, RwFrame *pstTargetFrame = NULL, RpHAnimHierarchy *pstTargetHierarchy = NULL, FLOAT fTimeScale = 1.0f);
	BOOL	SetBaseUsableItemEventEffect(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, UINT32 ulConditionFlags);
	BOOL	SetBaseSkillEventEffect(AgpdCharacter *pcsAgpdCharacter);
	BOOL	SetBaseCustomEventEffect(AgpdCharacter *pcsAgpdCharacter, UINT UIEffectID, float fWaveHeightOffset);		//. ������ Effect ID�� ĳ������ġ�� �����ǰ��Ѵ�.
	BOOL	SetBaseNatureEventEffect(AgpdCharacter *pcsAgpdCharacter, AgpdCharacterTemplate *pcsTemplate, BOOL bIsStart);	//. �������� N0~ (NatureCondition)���� ������ EffetSet�� �����Ѵ�.
	
	void	RemoveEventEffectFromPlayList(AgpdCharacter *pcsAgpdCharacter, AgcdUseEffectSetList* pEffectSetList);		//. �ش� Effect�� use list���� �����Ѵ�.

	// Ư���� ���.
	BOOL	SetCharTargetStruck(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, AgcdUseEffectSet *pstAgcdUseEffectSet, BOOL bSetRange = TRUE);
	// ��������
	BOOL	UpdateConvertedItemEventEffect(AgpdItem *pstAgpdConvertedItem);
	BOOL	SetConvertedItemEffect(AgpdItem *pstAgpdItem, AgcdItem *pstAgcdItem);
	BOOL	ReleaseConvertedItemEffect(AgpdItem *pstAgpdItem, AgcdItem *pstAgcdItem);
	BOOL	ResetConvertedItemEffect(AgpdItem *pstAgpdItem, AgcdItem *pstAgcdItem);
	BOOL	StartConvertedWeaponGradeEventEffect(AgpdItem *pstAgpdConvertedItem);	
	BOOL	StopConvertedWeaponGradeEventEffect(AgpdItem *pstAgpdConvertedItem);
	BOOL	StartConvertedWeaponAttrEventEffect(AgpdItem *pstAgpdConvertedItem, AgcdItem *pstAgcdConvertedItem);
	BOOL	StopConvertedWeaponAttrEventEffect(AgpdItem *pstAgpdConvertedItem, AgcdItem *pstAgcdConvertedItem);
	BOOL	SetConvertedArmourAttrEventEffect(AgpdItem *pstAgpdConvertedArmour);
	BOOL	StartConvertedArmourAttrEventEffect(AgpdItem *pstAgpdConvertedArmour);
	BOOL	StopConvertedArmourAttrEventEffect(AgpdItem *pstAgpdConvertedArmour);
	VOID	GetSpiritStoneInfo( AgpdItem* pcsAgpdItem, INT32* plNumSpiritStone, INT32* plRank, INT32* plLevel, INT32* plNumConvertedWaterAttr, INT32* lNumConvertedFireAttr, INT32* lNumConvertedEarthAttr, INT32* lNumConvertedAirAttr, INT32* lNumConvertedMagicAttr );
		
	AgcdUseEffectSetData*	GetSpiritStoneHitEffect(AgpdItem *pcsAgpdItem, INT32 lIndex);
	eAgcmEventEffectDataSpiritStoneType GetMaxSpiritStoneType(AgpdItem *pstAgpdConvertedItem, INT32 *plMaxPoint);
	RpMaterial**	GetConvertedArmourEffectMaterial(AgpdItem *pstAgpdConvertedItem);
	VOID	SetConvertedArmourEffectData(RpAtomic *pstAtomic, RpMaterial **ppstMaterial);
	// ActionResult����.
	BOOL	SetActionResultSkill(AgpdCharacter *pcsAgpdCharacter, AgpdCharacterActionResultType eActionResult);
	// �ܰ躰 ���� ����Ʈ
	BOOL	SetConvertedItemGradeEffect(AgpdItem *pcsAgpdConvertedItem, AgcdUseEffectSetData *pstEffectSetData);
	// ���¼� Ÿ�� ����Ʈ
	eAcReturnType	SetSpiritStoneHitEffect(AgpdCharacter *pcsAgpdAttacker);
	// �ð� üũ ����Ʈ
	BOOL	SetCheckTimeDataList(AgcdUseEffectSet *pcsEffectSet, ApBase *pcsBase);
	BOOL	AddEffectSet(AgcdEventEffectCheckTimeDataList *pcsList);
	BOOL	AddCheckTimeData(AgcdEventEffectCheckTimeDataList* pcsList, AgcdUseEffectSetData *pcsData);
	AgcdEventEffectCheckTimeDataList*	GetCheckTimeDataList(AgcdEventEffectCheckTimeDataList *pcsHead, ApBase *pcsKey);
	BOOL	ReleaseCheckTimeDataList(ApBase *pcsBase);
	BOOL	RemoveCheckTimeDataList(ApBase *pcsKey);	
	BOOL	RemoveAllCheckTimeData(AgcdEventEffectCheckTimeDataList *pcsParant);
	BOOL	RemoveEffectSet(AgcdEventEffectCheckTimeDataList *pcsList);	
	// ĳ���� �⺻ ����Ʈ
	BOOL	SetCommonCharEffect(ApBase *pcsBase, eAgcmEventEffectDataCommonCharType eType, INT32 lTargetCID = 0);		

	// Buffed skill ����
	BOOL	SetBuffedAttackEffect( AgpdCharacter *pcsAgpdAttacker, AgpdCharacter *pcsAgpdTarget, AgcdCharacter *pcsAgcdAttacker = NULL, AgcdCharacter *pcsAgcdTarget = NULL );
	BOOL	SetOnlySoundSkill( AgcmStartEventEffectParams *pcsParams );

public:
	BOOL	PlaySocialSound( AgpdCharacter *pcsAgpdCharacter, AgpdCharacterSocialType eType );
	BOOL	RemoveAllEffect(ApBase *pcsBase);

	BOOL	SetPassiveSkill(AgpdCharacter *pcsAgpdCharacter, AgcdUseEffectSet *pstAgcdUseEffectSet, AgcmStartEventEffectParams *pcsParams, INT32 lExceptionType = 0, INT32 lLife = -1 , BOOL bForce = FALSE );

	ACharacterSkyFlags*	GetSkySetFlagData( AgpdCharacter* psdCharacter );

	BOOL	EventSkillAddPreengagedPassiveSkillEffect(INT32 lBuffedTID, INT32 lCasterTID, AgpdCharacter* pcsAgpdTarget);

protected:
	BOOL	RemoveEquipmentsEffect(AgpdCharacter *pcsAgpdCharacter);
	BOOL	RemoveContinuativetSkillEventEffect(AgcdEventEffectCharacter *pcsData, INT32 lSTID);
	BOOL	RemoveAllContinuativeSkillEventEffect(AgcdEventEffectCharacter *pcsData);
	BOOL	RemoveAllConvertedItemGradeEventEffect(AgcdEventEffectItem *pcsData);

	eAcReturnType		SetAttrDamageEffect(AgpdCharacter *pcsAgpdAttacker, AgpdCharacter *pcsAgpdTarget);
	eAcReturnType		SetAttrDamageEffect(AgpdCharacter *pcsAgpdAttacker, AgpdFactor* pcsDamageFactor);
	AgcdUseEffectSet*	GetSkillUseEffectSet(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, INT32 lSkillTID, INT32 lConditionFlags);

	// stream����.
	BOOL	StreamWriteCharacterAnimationAttachedData(AgpdCharacterTemplate *pstAgpdCharacterTemplate, ApModuleStream *pStream);
	BOOL	StreamReadCharacterAnimationAttachedData(AgpdCharacterTemplate *pstAgpdCharacterTemplate, ApModuleStream *pStream);

	// �⺻ Sound ����
	INT32	GetWeaponType(AgpdItem *pcsAgpdItem);
	UINT32	PlaySound(const CHAR *szSoundPath, const CHAR *szMonoPath, const CHAR *szSoundName, BOOL bSelfCharacter, RpClump *pstParentClump, BOOL bLoop = FALSE, AEE_CharAnimAttachedData *pcsAttachedData = NULL, FLOAT fPlaySpeed = 1.0f );
	BOOL	PlayCharacterMovingSound(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter);
	BOOL	PlayAttackSound(AgpdCharacter *pcsAgpdCharacter, INT32 lWeaponType, BOOL bSelfCharacter, RpClump *pstParentClump);
	BOOL	PlayHitSound(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter = NULL, AgpdCharacter *pstAgpdTarget = NULL);
	BOOL	PlayAnimLoopSound(AgpdCharacter *pcsAgpdCharacter, AgcdCharacter *pcsAgcdCharacter, CHAR *szSoundName);

	BOOL	HideShieldAndWeaponsWhileCurrentAnimationDuration(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter = NULL);

	// E_AGCD_EVENT_EFFECT_CUST_DATA_INDEX_ACTION_OBJECT ����	
	BOOL	AttachShieldAndWeapons(INT32 lCID, BOOL bSetConvertedItemEffect = TRUE);
	BOOL	AttachShieldAndWeapons(AgpdCharacter *pstAgpdCharacter, BOOL bSetConvertedItemEffect = TRUE);
	BOOL	AttachShieldAndWeapons(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, BOOL bSetConvertedItemEffect = TRUE);
	static	PVOID CBAttachShieldAndWeapons(PVOID pvData);

	// Ÿ�� ����Ʈ
	BOOL	SetTargetStruckEffect(AgpdCharacter *pstAgpdAttacker, AgpdCharacter *pstAgpdTarget);

	//////////////////////////////////////////////////////////////////////////////
	// Callback
	// ���ΰ� �Ƶ� ����~
	static BOOL CharacterSetSelfCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// object remove
	static BOOL PreRemoveCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// AgcmEventEffectList remove����
	static BOOL RemoveCommonObjectCB(PVOID pData, PVOID pClass, PVOID pCustData); // character, item, object ����
	static BOOL RemoveEffectCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Event(Init)
	static BOOL EventInitCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Event(Object)
	static BOOL EventObjectWorkCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventObjectCheckHourCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Event(Character)
	static BOOL EventCharAnimCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAnimWaitCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAnimWalkCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAnimRunCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAnimAttackCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAttackSuccessCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAnimSocialCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAnimDeadCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharDamageCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharLevelUPCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	EventCharCreate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharActionResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharTransform(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharUpdateMovemenntCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharEndSocialAnimCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharSpecialStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventCharAdditionalEffect(PVOID pData, PVOID pClass, PVOID pCustData);

	// Event(Item)
	static BOOL EventItemEquipCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventItemUnequipCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventItemDetachedCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventItemUpdateConvertHistoryCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventItemSpiritStoneAttrCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventItemUseCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Event(Skill)
	static BOOL EventSkillCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillAddBuffedListCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillRemoveBuffedListCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillChargeThirdAttackCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillConsumeThirdAttackCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillEvationCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillDamageReflectionCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillChangeVelocityCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL EventSkillAbsorbDamageCB(PVOID pData, PVOID pClass, PVOID pCustData);	
	static BOOL EventSkillAdditionalEffectCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Event(AgcmEffectNoticeEffectProcessCB)
	static BOOL NoticeEffectHitEffectCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass);
	static BOOL NoticeEffectDestroyMatchCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass);
	static BOOL NoticeEffectSkillDestroyMatchCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass);
	static BOOL NoticeEffectSetTargetStruckAnimCB(stAgcmEffectNoticeEffectProcessData stProcessData, PVOID pClass);

	// construct / destruct(������ ������ ���̴� ������, �ı���)
	static BOOL AttachedDataEffectListConstructCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AttachedDataEffectListDestructCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AttachedDataObjUseEffectSetConstructCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AttachedDataCharUseEffectSetConstructCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AttachedDataItemUseEffectSetConstructCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AttachedDataUseEffectSetDestructCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AttachedDataEventEffectDataConstructCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AttachedDataEventEffectDataDestructCB(PVOID pData, PVOID pClass, PVOID pCustData);

	// Streaming �Լ���...
	static BOOL	CharTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL CharTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL	ItemTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL ItemTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL	ObjTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL ObjTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);

	// AgcmCharacter::m_csAnimation�� Attached�� Data����.
	static PVOID CBRemoveAnimation(PVOID pvData1, PVOID pvData2);
	static BOOL CBStreamReadCharacterAnimationAttachedData(PVOID pvClass, PVOID pvData1, PVOID pvData2);
	
	// StartEventEffect ����
	static BOOL CBCheckSkipSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2);

	// RpClumpForAllAtomics ����
	static RpAtomic* SetConvertedArmourAttrEffectCB(RpAtomic *pstAtomic, void *pvData);
	static RpAtomic* ReleaseConvertedArmourAttrEffectCB(RpAtomic *pstAtomic, void *pvData);

	//ETC
	static PVOID	AnimSoundCB(PVOID pvData);
	static BOOL		EventWaveFxTrigger(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSkyChange ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL		CBCharUseEffect(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // __AGCMEVENTEFFECT_H__
