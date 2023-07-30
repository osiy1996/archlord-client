#ifndef __AGCMGRASS_H__
#define __AGCMGRASS_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmGrassD" )
#else
#pragma comment ( lib , "AgcmGrass" )
#endif
#endif

#include "AgcModule.h"
#include "rwcore.h"
#include "rpworld.h"

#include "AgcmRender.h"
#include "ApmOcTree.h"
#include "AgcmOcTree.h"
#include "ApmMap.h"
#include "AgcmMap.h"
#include "AgcmEventNature.h"
#include "AgcmShadow2.h"

#include "AuPackingManager.h"

#include "ApMemory.h"
#include <vector>
#include <list>

#include "ContainerUtil.h"

// Sector Destroy CB�޾Ƽ� octree node�� sector data�� �ִ� Ǯ ������ ������!
// Sector Add CB �޾Ƽ� Ǯ �ε�����~ (wateró��)
#define AGCMGRASS_INI_NAME_TEXTURE_NAME				"TEXTURE_NAME"
#define AGCMGRASS_INI_NAME_TEXTURE_ID				"TEXTURE_ID"
#define AGCMGRASS_INI_NAME_GROUP_NAME				"GRASS_GROUP"
#define AGCMGRASS_INI_NAME_GRASS_INFO_ID			"GRASS_ID"
#define AGCMGRASS_INI_NAME_GRASS_NAME				"GRASS_NAME"
#define AGCMGRASS_INI_NAME_SHAPE_TYPE				"SHAPE_TYPE"
#define AGCMGRASS_INI_NAME_GRASS_WIDTH				"GRASS_WIDTH"
#define AGCMGRASS_INI_NAME_GRASS_HEIGHT				"GRASS_HEIGHT"
#define AGCMGRASS_INI_NAME_GRASS_WIDTH_RANDOM		"GRASS_WIDTH_RANDOM"
#define AGCMGRASS_INI_NAME_GRASS_HEIGHT_RANDOM		"GRASS_HEIGHT_RANDOM"
#define AGCMGRASS_INI_NAME_ANIMATION_TYPE			"ANIM_TYPE"
#define AGCMGRASS_INI_NAME_ANIMATION_AMOUNT			"ANIM_AMOUNT"
#define AGCMGRASS_INI_NAME_ANIMATION_SPEED			"ANIM_SPEED"
#define AGCMGRASS_INI_NAME_IMAGE_START_X			"IMAGE_START_X"
#define AGCMGRASS_INI_NAME_IMAGE_START_Y			"IMAGE_START_Y"
#define AGCMGRASS_INI_NAME_IMAGE_WIDTH				"IMAGE_WIDTH"
#define AGCMGRASS_INI_NAME_IMAGE_HEIGHT				"IMAGE_HEIGHT"

typedef enum
{
	GRASS_SHAPE_CROSS			= 0,
	GRASS_SHAPE_PERPENDICULAR	= 1
} enumGrassShapeType;

// octree�� leaf �ʺ� 400�̰� �밢 ���� �뷫 ���ϸ� 700
#define	AGCM_GRASS_SPHERE_RADIUS	700.0f
#define	AGCM_GRASS_HEIGHT_OFFSET	10.0f

#define AGCM_GRASS_SECTOR_APPEAR_DIST	2

#define		D3DFVF_GRASS_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)
struct GRASS_VERTEX {
	float x, y, z;
	DWORD color;
	DWORD spec;	
	float u,v;
};

#define		GRASSLIST			std::list< Grass* >
#define		GRASSLISTITER		std::list< Grass* >::iterator

struct Grass: public ApMemory<Grass, 20000>
{
	WORD			iGrassID;
	WORD			iShadow;

	FLOAT			fRotX;
	FLOAT			fRotY;
	FLOAT			fScale;
	
	FLOAT			fAnimParam1;						// Animation Parameter1
	FLOAT			fAnimParam2;

	RwV3d			vPos;
	D3DXMATRIX		matTransform;
	
	GRASS_VERTEX*	pVB;

	INT32			iVertexOffset;
	RwTexture*		pTexture;
				
	VOID			Init()	{ iGrassID=0; iShadow=0; fRotX=fRotY=0.0f; fScale=1.0f; fAnimParam1=fAnimParam2=0.f; pVB=NULL; iVertexOffset=0; pTexture=NULL; }

	Grass(){ Init(); }
};

#define				DRAW_ORDER_SORT_COUNT		4

struct GrassGroup				// �� ������ grass ����.���� ���� ���� ���´�.
{
	RwSphere		BS;
	
	// ��node���� ����.
	OcTreeCustomID	stOctreeID;				
	
	INT32			iGrassNum;
	GRASSLIST		ListGrass;

	FLOAT			MaxY;					// �׷쳻 Ǯ�� �ִ� Y��(���߿� Ŭ���̾�Ʈ���� �ִ� BBOX���Ҷ� ����)
	
	INT32			iCameraZIndex;			// client�� ����(octree mode��) .. maptool���� save flag�� ����

	RwV3d			VertDrawOrder[DRAW_ORDER_SORT_COUNT];

	GrassGroup*		next;

	GrassGroup():iGrassNum( 0 ) , MaxY( 0.0f ),next( NULL )
	{
	}
	~GrassGroup(){  }

	void			InitVertOrder();
};

struct GrassInfo
{
	char			Name[30];

	INT32			iTextureID;
	INT32			iShapeType;
		
	INT32			iVertexOffset;
	INT32			iVertexOffset_Dark;
	INT32			numTris;

	//. RwD3D9DrawPrimitiveUP ������� Ʃ��.
	GRASS_VERTEX*	pOriginalVB;
	GRASS_VERTEX*	pOriginalDarkVB;

	INT32			iAnimationType;
	FLOAT			fAnimAmount;
	FLOAT			fAnimSpeed;

	INT32			iWidth;
	INT32			iHeight;
	INT32			iWidth_random;
	INT32			iHeight_random;

	INT32			iImageX;
	INT32			iImageY;
	INT32			iImageW;
	INT32			iImageH;

	GrassInfo() : /*pvOriginalVB( NULL ), 
				pvOriginalVB_Dark( NULL ), */
				pOriginalVB(NULL), 
				pOriginalDarkVB(NULL),
				iVertexOffset(0){}
};

//Ǯ���� �ε��� ���� ������(�ߺ� �ε� ����..)
struct	SectorGrassRoot
{
	ApCriticalSection	pLockMutex;

	INT16			six;
	INT16			siz;

	GrassGroup*		listGrassGroup;
	INT32			iGrassGroupCount;

	UINT32			iTotalGrassCount;					// maptool �۾��� ����� .. ������ ������ �ȴ�..
	INT16			bLoad;
	INT16			bAddRender;

	SectorGrassRoot*	next;

	BOOL			DoContainThis( RwV3d * pV3d );
};

typedef enum
{
	GRASS_DETAIL_HIGH			= 0,
	GRASS_DETAIL_MEDIUM			= 1,
	GRASS_DETAIL_LOW			= 2,
	GRASS_DETAIL_OFF
} enumGrassDetail;

typedef enum
{
	GRASS_JOB_ADD_ROOT = 0 ,
	GRASS_JOB_ADD_RENDER,
	GRASS_JOB_REMOVE_ROOT,
	GRASS_JOB_CLEAR_ROOT,				// option���� off�� ���� root�� ����� clear
	
	GRASS_JOB_REMOVED
} enumGrassJobType;

struct GrassJobQueue
{
	enumGrassJobType		type;
	void*					data1;							// �ظ��ϸ� sector ���� (sector release �˻��)
	void*					data2;
};

struct stGrassTextureEntry
{
	RwTexture*				m_pTexture;
	int						m_nIndex;

	stGrassTextureEntry( void )
	{
		m_pTexture = NULL;
		m_nIndex = -1;
	}
};

// �������� Ǯ �ϳ��ϳ��� group���� ���ֵǰ� client���� octree node�ϳ��� Ǯ group�ϳ��̴�.
class AgcmGrass : public AgcModule
{
private:
	typedef	std::multimap< FLOAT ,  Grass* , greater< FLOAT > >				mmapDrawGrass;
	typedef	std::multimap< FLOAT ,	Grass* , greater< FLOAT > >::iterator	mmapDrawGrassIter;

public:
	AgcmGrass();
	~AgcmGrass();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);		

	void	DeleteAll(SectorGrassRoot*	root);

	void	SetGrassDetail(enumGrassDetail	detail);

	BOOL	LoadGrassInfoFromINI(char*	szFileName , BOOL bDecryption);
	BOOL	SaveGrassInfoToINI(char*	szFileName , BOOL bEncryption);

	INT32	MakeGrassVerts(GrassInfo*	pGrass);

	BOOL	InitGrass(Grass*	pGrass);
		// Ǯ �����ǰ� ���ϸ��̼� ������ �����Ѵ�.

	void	CheckAndUpdateInShadow();		// Ǯ���� list���� shadow�������� �ƴ��� üũ�ϰ� �ش�VB set(normal or shadow)

	void	JobQueueAdd(enumGrassJobType	type,PVOID data1,PVOID data2);		// ���� data�˻��ؼ� skipó��
	void	JobQueueRemove(INT32	six,INT32	siz);

	void	GrassMatrixSet(D3DXMATRIX*	pMat,Grass*	pGrass,INT32	iCamIndex);
	
	static	BOOL	CB_GRASS_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CB_GRASS_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData );

	//@{ 2006/11/17 burumal
	static	BOOL	CB_GRASS_DISTCORRECT( PVOID pDistFloat, PVOID pNull1, PVOID pNull2 );
	//@}
	
	//void	RenderGrass();

	void	LoadGrass(SectorGrassRoot*	pRoot);
	
	static	BOOL	CB_LOAD_MAP ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CB_UNLOAD_MAP ( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL	CB_LIGHT_VALUE_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL	CB_SHADOW_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData );

	// MapTool�� 
	static	BOOL	CB_OCTREE_ID_SET ( PVOID pData, PVOID pClass, PVOID pCustData );

	void	SetDirectoryPath(char*	szDir);

	SectorGrassRoot * GetGrassRoot( ApWorldSector * pSector ) { return GetGrassRoot( pSector->GetArrayIndexX() , pSector->GetArrayIndexZ() ); }
	SectorGrassRoot * GetGrassRoot( INT32 nIndexX , INT32 nIndexZ );

	void	AddGrass(INT32	iGrassID, FLOAT x, FLOAT z,FLOAT fRotX,FLOAT fRotY,FLOAT fScale);
	void	RemoveGrass(RwSphere*	sphere);			// �� ���� �ȿ� ��� Ǯ�� ����..
	void	SaveToFiles( char * szDir );
	void	RemoveAll();

	void	UnityForSave();								// ���� octree id���� root���� �����Ѵ�. ^^ 

	BOOL	CheckGrassList		();	// ����Ÿ�� �������� Ȯ��..
	void	OptimaizeGrassList	();	// �ҿ����� ����Ÿ�� ������.

	void	UpdateGrassHeight();							// ��� Ǯ list���̸� ����
	void	UpdateGrassHeight(ApWorldSector*	pSector);	// �ش� ���� Ǯlist���̸� ����

	FLOAT	GetGrassMinHeight( ApWorldSector*	pSector , FLOAT fTopHeight = SECTOR_MAX_HEIGHT );
	static	BOOL	CB_GET_MIN_HEIGHT ( PVOID pData, PVOID pClass, PVOID pCustData );
	
// data
	AgcmRender*			m_pcsAgcmRender;
	ApmMap*				m_pcsApmMap;
	AgcmMap*			m_pcsAgcmMap;
	AgcmOcTree*			m_pcsAgcmOcTree;
	ApmOcTree*			m_pcsApmOcTree;
	AgcmEventNature*	m_pcsAgcmEventNature;
	AgcmShadow2*		m_pcsAgcmShadow2;

	LPDIRECT3DDEVICE9	m_pCurDevice;

	BOOL				m_bDrawGrass;
	enumGrassDetail		m_eDetail;
					
	INT32				m_iDrawGrassNum;
	INT32				m_iDrawGrassCountNear;			// ���� �����ӿ��� �׷��� Ǯ����(������ �ִ°ų� maptool)
	INT32				m_iDrawGrassCount;				// ���� �����ӿ��� �׷��� Ǯ����(profile��)

	RwV3d*				m_pViewMatrixAt;

	UINT32				m_uiLastTick;
	UINT32				m_uiCurTickDiff;
	
	INT32				m_iGrassInfoNum;
	vector< GrassInfo >	m_astGrassInfo;
	
	INT32				m_iTextureNum;
	ContainerVector< stGrassTextureEntry >		m_vecGrassTexture;

	SectorGrassRoot*	m_listGrassRoot;
	//DrawGrass*		m_listDraw;
	
	ApCriticalSection	m_csCSection;
	GrassJobQueue*		m_pJobQueue;
	INT32				m_iJobQueueCount;
	INT32				m_iJobQueueCount2;
	INT32				m_iJobQueueMaxCount;

	char				m_szDirectory[48];

	INT16				m_iDrawCameraIndex;
	INT16				m_iSectorRange;
};

#endif