#ifndef __AGCMGLYPH_H__
#define __AGCMGLYPH_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmGlyphD" )
#else
#pragma comment ( lib , "AgcmGlyph" )
#endif
#endif

#include <AgcModule/AgcModule.h>
#include <rwcore.h>
#include <rpworld.h>
#include <AgcmEff2/AgcuEffD3dVtx.h>


//@{ 2006/10/27 burumal
enum EnumGlyphFlag
{
	eGLYPH_FLAG_NONE			= 0x00,
	eGLYPH_FLAG_FORCED_VISIBLE	= 0x01,
};
//@}

struct GlyphData
{
	//@{ 2006/10/27 burumal
	unsigned char	flag;
	//@}

	RwTexture*		tex;
	INT32			life_count;		// -99999�̸� ������ �׷���
	
	int				type;			// 0 - static,1 - movable
	RwFrame*		movepos;		// �����̴� ��ü�� ���� ��ġ
	RwV3d			staticpos;
	float			width;
	float			height;

	RwRGBA			color;
	float			rot_angle;

	RwSphere		sphere;			// frustum check��
	bool			bInFrustum;		// Frustum�ȿ� ���Դ��� ����

	D3DVTX_PCT*		vertex;			// heap
	WORD*			indies;			// 2005. 10. 28. Nonstopdj Indies�߰�
	int				vert_count;
	int				index_count;
	int				primitive_count;

	GlyphData*	next;
};


typedef struct tag_TexList
{
	RwTexture*		tex;
	tag_TexList*	next;
}TexList;

enum	enumGlyphType
{
	AGCM_GLYPH_STATIC_POS = 0,
	AGCM_GLYPH_STATIC_POS_DIRTY,							// Static Pos Glyph�� ���°� ��ȭ�� ���
	AGCM_GLYPH_STATIC_POS_SIZE_UPDATE,						// Size�� ��� �������� ��� (��¼�� ���� ��쿣 Resize�Լ��� ����)
	AGCM_GLYPH_MOVABLE_POS,
	AGCM_GLYPH_MOVABLE_POS_SIZE_UPDATE,						// Size�� ��� �������� ��� (��¼�� ���� ��쿣 Resize�Լ��� ����)
};

typedef struct tag_glyphdataA		// Animation
{
	TexList*		texlist;
	TexList*		cur_tex;
	
	INT32			cur_tick;
	INT32			change_tick;	// frame ��ȭ(cur_tex->next�� null�̸� texlist�� ����) 

	INT32			life_count;		// -99999�̸� ������ �׷���
	
	int				type;			// 0 - static,1 - movable
	RwFrame*		movepos;		// �����̴� ��ü�� ���� ��ġ
	RwV3d			staticpos;
	float			width;
	float			height;

	RwRGBA			color;
	float			rot_angle;

	RwSphere		sphere;			// frustum check��
	bool			bInFrustum;		// Frustum�ȿ� ���Դ��� ����

	D3DVTX_PCT*		vertex;			// heap
	WORD*			indies;			// 2005. 10. 28. Nonstopdj Indies�߰�
	int				vert_count;
	int				index_count;
	int				primitive_count;

	tag_glyphdataA*	next;
}GlyphDataA;

typedef	struct
{
	UINT8		blue;
	UINT8		green;
	UINT8		red;
	UINT8		alpha;
}D3DParticleColor;					// Dx color�� ������� ������ ����ü �ٷ� memcpy�� ����..^^v

typedef	struct	tag_DestMoveParticle
{
	RwV3d			pos;
	FLOAT			size;
	FLOAT			acceleration;
	FLOAT			deg;
	D3DParticleColor	color;
}DestMoveParticle;

enum	DestMoveParticleDrawType
{
	DEST_MOVE_PARTICLE_STOP = 0,
	DEST_MOVE_PARTICLE_START,
	DEST_MOVE_PARTICLE_CIRCLE
};

#define			DEST_MOVE_PARTICLE_NUM		12
typedef	struct	tag_DestMoveTarget
{
	DestMoveParticleDrawType		eDrawParticle;
	INT32				AnimCount;
	RwSphere			sphere;
	GlyphData*			pGlyph;

	INT32				DrawParticleTailNum;
	DestMoveParticle	Particle[5][DEST_MOVE_PARTICLE_NUM];

	FLOAT				fGroundHeight[72];			// 5' ������ ���̰� ����
	FLOAT				fCircleRotAngle;
}DestMoveTarget;

enum	DestEnemyTargetSoulStoneType
{
	DEST_TARGET_NORMAL = 0,
	DEST_TARGET_MAGIC,
	DEST_TARGET_WATER,
	DEST_TARGET_FIRE,
	DEST_TARGET_EARTH,
	DEST_TARGET_AIR,
	DEST_TARGET_SOUL_STONE_NUM
};

#define		SOUL_STONE_SLOT_MAX_NUM			8

enum	DestEnemyTargetSkillType
{
	DEST_TARGET_NONE = 0,
	DEST_TARGET_ATTACK,
	DEST_TARGET_BUFF
};

//. 2005. 11. 1. Nonstopdj
//. ridable Object�� Block Object�� RpClump*�� stl list�� �����Ѵ�.
//. SetVertsCompact()���� Update�ȴ�.
#include <list>
typedef std::list<RpClump*>				ClumpList;
typedef std::list<RpClump*>::iterator	ClumpListIter;

// �� Ŭ���� ����� Ÿ�� �̹����� ���� ��������.. ���� ���̾ �����Ƿ�.. vertex������ ����
typedef	struct	tag_DestEnemyTargetGlyph
{
	BOOL			bAdded;

	RwFrame*		movepos;		// �����̴� ��ü�� ���� ��ġ
	float			width;
	float			height;
	INT32			mem_alloc_size;
	
	INT32			color_anim_type;
	float			color_anim_val;
	BOOL			bEnableDefault;											// ���� ���ɼ� á���ÿ��� �⺻ �̹��� �ȱ׸���.. ���Ƽ�..
	
	INT32			soul_stone_type_num;									// ���ɼ��� ���� 1�̴�..(�⺻ ���⵵ ���� ���� ^^)
	INT32			soul_stone_type[SOUL_STONE_SLOT_MAX_NUM];				// ���� ���⿡ �߷��ִ� ���ɼ� type
	INT32			soul_stone_level[SOUL_STONE_SLOT_MAX_NUM];				// ���� ���⿡ �߷��ִ� �ִ� ���ɼ� level

	RwRGBA			color[SOUL_STONE_SLOT_MAX_NUM];
	float			rot_angle[SOUL_STONE_SLOT_MAX_NUM];

	DestEnemyTargetSkillType	eSkillType;									// skill�ߵ��� ���� �׸����
	RwIm3DVertex*	vertex_skill_deco;
	RwRGBA			color_skill_deco;
	INT32			skill_duration;
	float			rot_angle_skill_deco;

	RwSphere		sphere;			// frustum check��
	BOOL			bInFrustum;		// Frustum�ȿ� ���Դ��� ����

	RwIm3DVertex*	vertex[SOUL_STONE_SLOT_MAX_NUM];						// heap(AcuFrameMemory ���).. uv�� Į�� Ʋ���� ����
	int				vert_count;

	//. 2006. 5. 16. nonstopdj
	RpClump*		pTargetClump;
	BOOL			bSelected;

}DestEnemyTargetGlyph;

class AgcmGlyph : public AgcModule
{
public:
	AgcmGlyph();
	~AgcmGlyph();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);		

	// Texture 1�� ¥��.. ------------------------------------------------------------------------------------------------
	GlyphData*		AddStaticGlyph(RwTexture* tex,RwV3d*	pos,float width,float height
		,DWORD	color=0xFFFFFFFF,INT32 life_count = -99999,BOOL	bSizeVariable = FALSE, unsigned char uFlag = eGLYPH_FLAG_NONE);

	GlyphData*		AddMovableGlyph(RwTexture* tex,RwFrame*	frame,float width,float height
		,DWORD color=0xFFFFFFFF,INT32 life_count = -99999,BOOL	bSizeVariable = FALSE, unsigned char uFlag = eGLYPH_FLAG_NONE);
	void	DeleteGlyph(GlyphData* del_node);
	void	UpdatePosStaticGlyph(GlyphData*	pData,float x,float y,float z);
	void	ResizeGlyph(GlyphData*	pData,float	width,float	height);

	inline	void	SetWorldCamera(RwCamera* cam) { m_pCamera = cam; }
	static	BOOL	CB_POST_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CB_CUSTOM_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL	CB_SELF_CHARACTER_CAST_SKILL ( PVOID pData, PVOID pClass, PVOID pCustData );

	void	StartDestMoveTarget(RwV3d*	pos,float	radius,GlyphData*	pGlyph);
	void	StopDestMoveTarget();

	//. 2006. 5. 16. nonstopdj.
	void	StartEnemyTarget(RwFrame*	pFrame, RpClump* pClump, float	radius);
	void	SetMouseOnTargetClump(RpClump* pClump);
	void	StopEnemyTarget();
	void	CloseEnemyTargetSelectWindow();

	void	UpdateGlyph(INT32 tickdiff,RwV3d*	at);
	void	UpdateDestMoveTarget(INT32	tickdiff);
	void	UpdateDestEnemyTarget(INT32	tickdiff);

	// Vertex count�� �����Ѵ�..
	INT32	SetVerts(RwIm3DVertex*	pVerts,float x,float z,float width,float height,float rot_angle,RwRGBA	color);
	
	//. 2005. 10. 28. Nonstopdj
	//. Radius�� �߰�.
	void	SetVertsCompact(D3DVTX_PCT** ppVertex, WORD** ppIndies, int* vertex_count, int* index_count, int* primitive_count,
							float posx, float posz,float fwidth, float fheight, RwReal radius, float rot_angle,RwRGBA	color);

	//. 2005. 11. 3. Nonstopdj
	//. Glyph�� �׷��� Clump list�� clear�ϴ� callback.
	static	BOOL	CB_CLEAR_CLUMPLIST( PVOID pData, PVOID pClass, PVOID pCustData );

	//@{ 2006/04/04 burumal
public : 
	void LockDestEnemyTarget() { m_csDestTarget.Lock(); }
	void UnlockDestEnemyTarget() { m_csDestTarget.Unlock(); }

	BOOL IsGlyphedFrame(const RwFrame* pSrcFrame);
	//@}

	//@{ 2006/09/01 burumal
	VOID ShowDestMoveTarget();
	VOID HideDestMoveTarget();
	//@}
	
	// data
	UINT32				m_uiLastTick;
	GlyphData*			m_listGlyph;

	//. 2005. 11. 1. Nonstopdj
	ClumpList			m_RBObjClumplist;		//. Ridable, Block Object Clump list
	D3DXMATRIX			m_TextureMatrix;		//. Projectively Texture Matrix

	RwCamera*					m_pCamera;
	class AgcmMap*				m_pcmMap;
	class AgcmRender*			m_pcmRender;
	class AgcmResourceLoader*	m_pcmResourceLoader;
	class AgcmCharacter*		m_pcsAgcmCharacter;
	class AgpmItem*				m_pcsAgpmItem;
	class AgpmItemConvert*		m_pcsAgpmItemConvert;
	class AgpmFactors*			m_pcsAgpmFactors;
	class AgcmSkill*			m_pcsAgcmSkill;
	class ApmObject*			m_pApmObject;
	class AgcmObject*			m_pcmObject;
	class AgcmCamera2*			m_pcsAgcmCamera2;

	float				m_fHeightSave[15][15];

// 3D Dest Move Cursor Datas
	DestMoveTarget		m_stDestMove;
	RwTexture*			m_pDestMoveTextureParticle;

// 3D Target Glyph Datas
	BOOL					m_bDraw3DDestTarget;					// texture NULL�� ��� �ȱ׸��� ����..
	DestEnemyTargetGlyph	m_stDestEnemyTarget;
	RwTexture*				m_pDestEnemyTargetTexture[7];

	RpClump*				m_pMouseOnClump;

	//@{ 2006/04/04 burumal
protected : 
	ApCriticalSection	m_csDestTarget;
	//@}

	//@{ 2006/09/01 burumal
	BOOL				m_bShowDestMoveTarget;
	//@}

	
private:
	void	AddRenderClumplist(const INT32 nIndex);
	void	UpdateProjectivelyTextureMatrix(const float fposx, const float fposz, 
											const float fwidth, const float fheight,
											const float fRadius, const float fRotationAng = 0.0f);
	
	static RwInt32		m_MateriralIndex;
	static RwTexture*	m_pGlyphTexture;
	static RpAtomic*	AtomicsRenderProcess(RpAtomic* pAtomic, void* pData);
	static RpMaterial*	AllMaterialRwTexturePointerSwap(RpMaterial *material, void *data);
	static RpMaterial*	AllMaterialRwTexturePointerRestore(RpMaterial *material, void *data);
};

#endif