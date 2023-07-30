/******************************************************************************
Module:  AgcdSkill.h
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 31
******************************************************************************/

#if !defined(__AGCDSKILL_H__)
#define __AGCDSKILL_H__

#include <ApBase/ApBase.h>
#include <AgpmSkill/AgpdSkill.h>
#include <AgcmAnimation/AgcaAnimation.h>
#include <AgcmEffectDataAdmin/AgcdEffectData.h>

class AgcdCharacterAnimation;

#define		AGCDSKILL_TEXTURE_NAME_LENGTH				64
#define		AGCDSKILL_ANIMATION_NAME_LENGTH				128
#define		AGCDSKILL_INFO_NAME_LENGTH					128
#define		AGCDSKILL_MAX_ACTION_SKILL_PRESERVE_TIME	1400	// ���� �׼� ����Ÿ�� �ִ� �󸶱��� �����ϰ� ������...
#define		AGCDSKILL_BASE_PRESERVE_TIME_PER_METER		100
#define		AGCDSKILL_SHOW_DAMAGE_INTERVAL				500
#define		AGCDSKILL_START_SKILL_EFFECT_DELAY			450

#define		AGCDSKILL_MAX_VISUAL_INFO_PC				AGPMSKILL_MAX_SKILL_OWN
#define		AGCDSKILL_MAX_VISUAL_INFO_BOSS_MONSTER		5

#define		AGCDSKILL_MAX_ACTION_QUEUE_SIZE				5

// stAgcmSkillVisualInfo �� skill template id�� name �� ���� ������ AgpmSkill�� �ִ� m_aszUsableSkillTName ���� 1:1 ��Ī �Ǳ� �����̴�.
/*typedef struct _stAgcmSkillVisualInfo {
	// Tool info
	CHAR				*m_paszInfoName[AGCDITEM_EQUIP_ANIMATION_TYPE_NUM];
	// Animation
	CHAR				*m_paszAnimName[AGCDITEM_EQUIP_ANIMATION_TYPE_NUM];	
	AgcdAnimation		*m_pastAnim[AGCDITEM_EQUIP_ANIMATION_TYPE_NUM];
	// Effect
	AgcdUseEffectSet	m_astEffect[AGCDITEM_EQUIP_ANIMATION_TYPE_NUM];
} stAgcmSkillVisualInfo, *pstAgcmSkillVisualInfo;*/
class AgcmSkillVisualInfo
{
public:
	// ��ų �����ֱ� �ɼ�..
	enum	OPTION
	{
		DEFEND_ON_OPTION,
		ALWAYS_SHOW
	};

	CHAR					**m_paszInfo;
	//CHAR					**m_paszAnimName;
	AgcdCharacterAnimation	**m_pacsAnimation;
	AgcdUseEffectSet		**m_pastEffect;

	//. 2005. 09. 09 Nonstopdj
	//. INI���� �о�� Index�� ����.
	//. ModelTool���� CopyCharacter���� ���.
	INT						m_iVisualIndex;
	OPTION					m_eShowOption;
	
	AgcmSkillVisualInfo():m_eShowOption( DEFEND_ON_OPTION )
	{
		m_paszInfo		= NULL;
		//m_paszAnimName	= NULL;
		m_pacsAnimation	= NULL;
		m_pastEffect	= NULL;
		m_iVisualIndex	= 0;
	}

	BOOL AllocatePArray(INT32 lMax)
	{
		if (	(m_paszInfo) ||
				//(m_paszAnimName) ||
				(m_pacsAnimation) ||
				(m_pastEffect)			)
			return FALSE;

		m_paszInfo	= new CHAR * [lMax];
		memset(m_paszInfo, 0, sizeof (CHAR *) * lMax);

		//m_paszAnimName	= new CHAR * [lMax];
		//memset(m_paszAnimName, 0, sizeof (CHAR *) * lMax);

		m_pacsAnimation	= new AgcdCharacterAnimation * [lMax];
		memset(m_pacsAnimation, 0, sizeof (AgcdCharacterAnimation *) * lMax);

		m_pastEffect	= new AgcdUseEffectSet * [lMax];
		memset(m_pastEffect, 0, sizeof (AgcdUseEffectSet *) * lMax);

		return TRUE;
	}

	VOID FreePArray()
	{
		if (m_paszInfo)
			delete [] m_paszInfo;
		//if (m_paszAnimName)
		//	delete [] m_paszAnimName;
		if (m_pacsAnimation)
			delete [] m_pacsAnimation;
		if (m_pastEffect)
			delete [] m_pastEffect;
	}
};


class AgcdSkillAttachTemplateData {
public:
	// skill animation & effect
	//stAgcmSkillVisualInfo	m_astSkillVisualInfo[AGPMSKILL_MAX_SKILL_OWN];
	//AgcmSkillVisualInfo	m_acsSkillVisualInfo[AGPMSKILL_MAX_SKILL_OWN];
	AgcmSkillVisualInfo		**m_pacsSkillVisualInfo;
};

class AgcdSkillTemplate {
public:
	// 2D image
//	CHAR				m_szTextureName[AGCDSKILL_TEXTURE_NAME_LENGTH];
	CHAR				*m_pszTextureName;
	RwTexture			*m_pSkillTexture;

//	CHAR				m_szSmallTextureName[AGCDSKILL_TEXTURE_NAME_LENGTH];
	CHAR				*m_pszSmallTextureName;
	RwTexture			*m_pSmallSkillTexture;

//	CHAR				m_szUnableTextureName[AGCDSKILL_TEXTURE_NAME_LENGTH];
	CHAR				*m_pszUnableTextureName;
	RwTexture			*m_pUnableTexture;

	//@{ 2006/05/04 burumal
	INT32				m_nDNF;
	//@}

	BOOL	IsSkipExport( ApServiceArea eArea )
	{
		return m_nDNF & GETSERVICEAREAFLAG( eArea );
	}
};

class AgcdSkillSpecializeTemplate {
public:
//	CHAR				m_szTextureName[AGCDSKILL_TEXTURE_NAME_LENGTH];
	CHAR				*m_pszTextureName;
	RwTexture			*m_pTexture;
};


typedef	struct	_AgcmSkillCastQueue {
	INT32				lCasterID;
} AgcmSkillCastQueue;

// 2005.09.22. steeple
class AgcdSkill
{
public:
	// �� ��ų�� ����� TargetCID ��. 2005.09.21. steeple.
	ApSafeArray<INT32, AGPMSKILL_MAX_TARGET> m_alAffectedTargetCID;
};

// 2005.09.22. steeple
// Naming �� �� �� �Ǿ� �ֱ淡 ����.
class AgcdSkillADChar
{
public:
	INT32				m_lNumCastSkill;
	AgcmSkillCastQueue	m_stCastQueue[AGCDSKILL_MAX_ACTION_QUEUE_SIZE];

	// ���� ����� ��ų�� ���� ����(BOB, 081104)
	INT32				m_lSkillUsableIndex;
	INT32				m_lSkillID;
};

#endif	//__AGCDSKILL_H__