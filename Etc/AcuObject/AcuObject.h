// AcuObject.h: interface for the AcuObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUOBJECT_H__1801AA47_8F43_40BC_9B0C_2C7F1FD02115__INCLUDED_)
#define AFX_ACUOBJECT_H__1801AA47_8F43_40BC_9B0C_2C7F1FD02115__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <rwcore.h>
#include <rpworld.h>
#include <rpusrdat.h>
#include <rplodatm.h>
#include <rphanim.h>
#include <ApBase/ApBase.h>
#include <vector>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuObjectD" )
#else
#pragma comment ( lib , "AcuObject" )
#endif
#endif

//#define			DINFONAME				"DI"
//#define			RENDERINFONAME			"REI"
#define			BACKFRAMEINFONAME		"BFI"
#define			SECTORMODEBACKUPNAME	"SEB"
#define			OCTREEID				"OT"

#define			TYPEINFOSLOT		0

#define			GEOM_TYPESLOT		0

enum ObjectType
{
	TYPE_ETYPE						= 0		,
	TYPE_LID						= 1		,
	TYPE_POBJECT					= 2		,
	TYPE_PTEMPLATE					= 3		,
	TYPE_PCUSTDATA					= 4		,
	TYPE_PICKATOMIC					= 5		,	// ��ŷ ���� �����..
	TYPE_OCTREEDATA					= 6		,	// octree renderdata �߰�^^
	TYPE_OCTREEIDLIST				= 7		,	// ApdObject�� OcTreeIDList
	TYPE_BSX						= 8		,
	TYPE_BSY						= 9		,
	TYPE_BSZ						= 10	,
	TYPE_BSRADIUS					= 11	,
	TYPE_UPDATELIST					= 12	,
	TYPE_UPDATETICK					= 13	,
	// Clump�� ���� ��� clump�� bounding sphere frustum check�� �����ϱ� ����(for performance)
	TYPE_RENDERFRUSTUMTESTTICK		= 14	,
	TYPE_RENDERFRUSTUMTESTRESULT	= 15	,
	// �ش� ������Ʈ�� �������� sector�Ÿ�
	TYPE_VIEWSECTORDISTANCE			= 16	,
	TYPE_COLLISIONATOMIC			= 17	,	// �ݸ��� ���� �����.
	TYPE_ALLOC_NUM					= 18	
};

// 0x00ff	Ÿ���� ��.
// 0xff00	�Ӽ��� �÷��׷� ���յǾ� ��.

/*	//atomic�� AgcdRenderInfo�� �ٽ��ϴ�. ���� AcuObject���� ����..(2004.11.3)
enum ObjectRenderIndex
{
	RI_RTYPE			= 0	,
	RI_CB				= 1	,
	RI_BMODE			= 2	,
	RI_ISBILL			= 3	,
	// 0�� �ƴ� shader�����
	// RI_SHADER - ���� atomic���� 0 - default path, 1 - shadow2 path�� ���δ�.(AcuRpMTexture���� ��)
	RI_SHADER			= 4	,	
	RI_BEFORELOD		= 5	,
	RI_SCOUNT			= 6	,
	RI_SCOUNTEND		= 7	,
	RI_LEV1				= 8	,		// FadeIn LOD Level
	RI_LEV2				= 9	,		// FadeOut LOD Level
	RI_CB2				= 10,

	RI_CURTICK			= 14,
	RI_INTERSECTTICK	= 15,
	// Billboard�� ���Խÿ� 1 , ���ýÿ� 0
	// ó�� �߰����� �� tick
//	RI_ADDTICK			= 16,
	RI_ISBILLBOARDLEV	= 17,

	RI_ALLOC_NUM		= 18
};*/

// �̰� ���� ���� ���� �ü� �ְ� ����.. �׷��� ���� ������� ..SECTORMODEBACKUPNAME 
//RI_IT_SECTORX: �߰����� sectorx(worldpos)
//RI_IT_SECTORZ: �߰����� sectorz(worldpos)
//RI_IT_RADIUS: �߰����� radius
	
// 1 - high(���ΰ�) 2 - medium , 3 - low
/*enum WhatIsDI
{
	DI_SHADOWLEVEL			= 0,
	DI_DISTANCE				= 1,
	DI_DISTANCETICK			= 2,
	DI_MAXLODLEVEL			= 3,
	// 0 - none , 1 - mouse on , 2 - lock ...
	DI_TARGETINGSTATUS		= 4,
	DI_CAMERA_Z_INDEX		= 5,

	DI_ALLOC_NUM			= 6
};*/

typedef enum AcuObjectType
{
	// Type
	ACUOBJECT_TYPE_NONE					=	0x0000	,
	ACUOBJECT_TYPE_OBJECT				=	0x0001	,
	ACUOBJECT_TYPE_CHARACTER			=	0x0002	,
	ACUOBJECT_TYPE_ITEM					=	0x0003	,
	ACUOBJECT_TYPE_WORLDSECTOR			=	0x0004	,

	ACUOBJECT_TYPE_TYPEFILEDMASK		=	0x000f	,

	// ������ (2005-04-11 ���� 4:50:40) : ������ M�ؽ��� ������������ ��Ÿ�� �ϴ� �÷���.
	ACUOBJECT_TYPE_NO_MTEXTURE			=	0x0010	,
	ACUOBJECT_TYPE_SECTOR_ROUGHMAP		=	0x0020	,	// �������� ���..

	ACUOBJECT_TYPE_RESERVED2			=	0x0040	,
	ACUOBJECT_TYPE_RESERVED3			=	0x0080	,
	ACUOBJECT_TYPE_OPTIONMASK			=	0x00f0	,

	// ���հ����� �÷���.. ( Property )
	ACUOBJECT_TYPE_BLOCKING				=	0x00000100	,
	ACUOBJECT_TYPE_RIDABLE				=	0x00000200	,
	ACUOBJECT_TYPE_NO_CAMERA_ALPHA		=	0x00000400	,
	ACUOBJECT_TYPE_USE_ALPHA			=	0x00000800	,
	ACUOBJECT_TYPE_USE_AMBIENT			=	0x00001000	,
	ACUOBJECT_TYPE_USE_LIGHT			=	0x00002000	,
	ACUOBJECT_TYPE_USE_PRE_LIGHT		=	0x00004000	,
	ACUOBJECT_TYPE_USE_FADE_IN_OUT		=	0x00008000	,
	ACUOBJECT_TYPE_IS_SYSTEM_OBJECT		=	0x00010000	,
	ACUOBJECT_TYPE_MOVE_INSECTOR		=	0x00020000	,
	ACUOBJECT_TYPE_NO_INTERSECTION		=	0x00040000	,
	ACUOBJECT_TYPE_WORLDADD				=	0x00080000	,
	ACUOBJECT_TYPE_OBJECTSHADOW			=	0x00100000	,
	ACUOBJECT_TYPE_RENDER_UDA			=	0x00200000	,
	ACUOBJECT_TYPE_USE_ALPHAFUNC		=	0x00400000	,
	ACUOBJECT_TYPE_OCCLUDER				=	0x00800000	,
	ACUOBJECT_TYPE_DUNGEON_STRUCTURE	=	0x01000000	,
	ACUOBJECT_TYPE_DUNGEON_DOME			=	0x02000000	,	// ���� õ��
	ACUOBJECT_TYPE_CAM_ZOOM				=	0x04000000	,	// ī�޶��浹 ī�޶���.
	ACUOBJECT_TYPE_CAM_ALPHA			=	0x08000000	,	// ī�޶��浹 ������Ʈ ����ó��.
	//@{ Jaewon 20050912
	// For the 'invisible' skill
	ACUOBJECT_TYPE_INVISIBLE			=   0x10000000	,
	//@} Jaewon
	//@{ 2006/09/08 burumal
	ACUOBJECT_TYPE_FORCED_RENDER_EFFECT	=   0x20000000	,
	//@}
	ACUOBJECT_TYPE_DONOT_CULL			=	0x40000000	,	// ������ �����Ѵ�.. ������� �༮ ����..
	ACUOBJECT_TYPE_PROPERTY_FILTER		=	0xffffff00
} AcuObjectType;

// ACUOBJECT_TYPE_MOVEINSECTOR : sector�������� �̵��� �Ͼ�� ��(ex. �ĵ�) �ٽ� ���� RwFrame�� position�� ��� ����Ǵ°�
// ACUOBJECT_TYPE_NO_INTERSECTION : Ray intersection���� �ɸ� �ʿ䰡 ���� ��ü��(ex.Ǯ,�ĵ�)
// ACUOBJECT_TYPE_WORLDADD : RpWorld�� �߰��Ǿ�� �ϴ� �̵� ��ü��

typedef struct
{
	INT32			eType			;
	INT32			lID				;
	PVOID			pvObject		;
	PVOID			pvTemplate		;
	PVOID			pvCustData		;
	PVOID			pvPickAtomic	;
} AcuObjectTypeData;

#define				BILLARRAYNUM	10

typedef struct tag_IsBill
{
	int				num;
	int				isbill[BILLARRAYNUM];
}IsBillData;

#define				DATABACKUPSIZE	1572
typedef struct tag_UserDataBackup
{
	PVOID				data[64];
	char				strdata[25][50];
	INT32				numelements;
	RpUserDataFormat	format;
	char				name[52];

	tag_UserDataBackup*		next;
}UserDataBackup;

#define				ALLOC_FRAME_MEM_ACUOBJECT		6000000

#define				ALLOC_OCTREE_ID_TEST_UDA_NUM	1501

class AcuObject  
{
private:
	// 2005.4.12 gemani ���� �޸� alloc���� ����.. 
	//static void*			AllocFrameMemory(INT32 nBytes);
	//static void				DeallocFrameMemory(INT32	nBytes);	// �����Ⱓ�� block�� �ΰ�� block������ �������� ����������

	//static INT32			m_iByteAllocated;			// debug ���������� check
	//static INT32			m_iCurAllocated;
	//static UINT8*			m_pMemoryBlock;
	//static UINT8*			m_pFrame;

public:
	//static ApCriticalSection	m_csMutexFrameMemory;

	AcuObject();
	virtual ~AcuObject();

	static CHAR *			m_szName;
	static RwCamera *		m_pstCamera;

	// �ε��� �м��ϴ� ���..
	static INT32			GetType			( int index );
	static INT32			GetProperty		( int index );

	// Atomic�� Type�� ID�� Get/Set �Ѵ�.
	static VOID				SetAtomicType	(
		RpAtomic *pstAtomic			,
		INT32 eType					,
		INT32 lID					,
		PVOID pvObject = NULL		,
		PVOID pvTemplate = NULL		,
		PVOID pvCustData = NULL		,
		PVOID pvPickAtomic = NULL	,
		RwSphere*	sphere = NULL	,
		PVOID pvOcTreeData = NULL	,
		PVOID pvOcTreeIDList = NULL
		);
	
	static INT32			GetAtomicType	( RpAtomic *pstAtomic	, INT32 *plID			, PVOID *ppvObject = NULL	, PVOID *ppvTemplate = NULL		, PVOID *ppvCustData	= NULL	, PVOID *ppvPickAtomic	= NULL);

	static RpAtomic *		GetAtomicGetCollisionAtomic	( RpAtomic * pstAtomic );
	static RpAtomic *		GetAtomicGetPickingAtomic	( RpAtomic * pstAtomic );

	// Clump�� Type�� ID�� Get/Set �Ѵ�. (CustData�� LODDistance�� ������ �� ���δ�.)
	static INT32			GetClumpType	(
		RpClump* pstClump,
		INT32* plID,
		PVOID* ppvObject = NULL,
		PVOID* ppvTemplate = NULL,
		PVOID* ppvCustData = NULL,
		PVOID* ppvPickAtomic = NULL);

	// ���� ������ �����ϴ� ���..
	//static RpAtomic *		GetClumpGetCollisionAtomic	( RpClump * pstClump );		//clump�� ���� AgcdType���� ����..
	//static RpAtomic *		GetClumpGetPickingAtomic	( RpClump * pstClump );		//clump�� ���� AgcdType���� ����..

	static VOID				SetClumpType	(
		RpClump *pstClump							,
		INT32 eType									, 
		INT32 lID									,
		PVOID			pvObject			= NULL	,
		PVOID			pvTemplate			= NULL	,
		PVOID			pvCustData			= NULL	,
		RpAtomic *		pvPickAtomic		= NULL	,
		RwSphere *		pvSphere			= NULL	,
		RpAtomic *		pvCollisionAtomic	= NULL	,
		PVOID			pvOctreeData		= NULL	,
		PVOID			pvOcTreeIDList		= NULL );

	static VOID				SetClumpType	( RpClump *pstClump		, INT32 eType );
	
	//static INT32*			GetClumpTypeArray ( RpClump *pClump );		//clump�� ���� AgcdType���� ����..
	static AgcdType*		GetAtomicTypeStruct ( RpAtomic *pAtomic );

	//static VOID			GetClumpBSphere ( RpClump *pClump , RwSphere* sphere);	//clump�� ���� AgcdType���� ����..
	static VOID				GetAtomicBSphere ( RpAtomic *pAtomic , RwSphere* sphere);

	// Add By gemani(03.06.26)
	static VOID				SetClumpDInfo_Distance	( RpClump *pstClump, float Distance, UINT32 UpdateTick);
	static VOID				SetClumpDInfo_Shadow	( RpClump *pstClump, int	ShadowLevel);
	static VOID				SetClumpDInfo_LODLevel	( RpClump *pstClump, int	LODLevel);
	//static INT32*			GetClumpDInfo	( RpClump *pstClump	);
	
	// Atomic�� Clump�� User Data�� �����Ѵ�.
	static RpAtomic			*DestroyAtomicDataCB(RpAtomic *atomic, void *data);
	static VOID				DestroyAtomicData	( RpAtomic *pstAtomic	);
	static VOID				DestroyClumpData	( RpClump *pstClump		);

	// Geometry�� Prelight�� setting�Ѵ�.
	static RpGeometry		*SetGeometryPreLitLum(RpGeometry *pstGeometry, RwRGBA *pstPreLitLum); // 030303 Bob�� �߰�
	static RpGeometry *		CBSetGeometryPreLitLum	( RpGeometry *pstGeometry, PVOID pvData );
	static VOID				SetAtomicPreLitLim		( RpAtomic *pstAtomic	, RwRGBA *pstPreLitLum	);
	static RpAtomic *		CBSetAtomicPreLitLim	( RpAtomic *pstAtomic	, PVOID pvPreLitLum	);
	static VOID				SetClumpPreLitLim		( RpClump *pstClump		, RwRGBA *pstPreLitLum	);

	// Geometry�� Instancing�Ѵ�.
	static BOOL				InstanceAtomic	( RpAtomic *pstAtomic	);
	static RpAtomic *		CBInstanceAtomic( RpAtomic *pstAtomic, PVOID pvData );
	static BOOL				InstanceClump	( RpClump *pstClump		);

	static VOID				SetCamera		(RwCamera *pstCamera	);

//	static VOID				SetAtomicInFrustumTime	(RpAtomic *pstAtomic, UINT32 ulInFrustumTime);
//	static UINT32			GetAtomicInFrustumTime	(RpAtomic *pstAtomic);

	// Add by TaiHyung 03.4.17
	static VOID				SetAtomicRenderUDA(RpAtomic *pstAtomic,UINT32 rendertype,
			RpAtomicCallBackRender cbRender,INT32	BMode,BOOL bBill,INT32 shadertype);

	/*static INT32*			GetAtomicRenderArray (RpAtomic *pstAtomic);		// �̰ɷ� �ַ� ó������
	static INT32*			GetClumpRenderArray(RpClump *pstClump);		// Clump�� �پ��ִ� renderarray�� �����´�(fade in/out�� refcount���� ����)

	static RpAtomicCallBackRender	GetAtomicRenderCallBack(RpAtomic *pstAtomic);
*/
	static float*			MakeAtomicBackFrameInfo(RpAtomic *pstAtomic);		// billboardó���� ���ư��� �� frame
	static float*			GetAtomicBackFrameInfo(RpAtomic *pstAtomic);

	static float*			MakeAtomicSectorModeBackup(RpAtomic *pstAtomic);	//  render sectormode�� save��
	static float*			GetAtomicSectorModeBackup(RpAtomic *pstAtomic);

	static VOID				SetClumpRenderInFrustum(RpClump *pstClump, PVOID pvData);
	static RpAtomic*		CBSetRenderInFrustum(RpAtomic *pstAtomic	, PVOID pvData);
	static VOID				SetAtomicRenderInFrustum(RpAtomic *pstAtomic, PVOID pvData);

	// LODAtomic���� Geometry�� Billboard ���� Set 
	static VOID				SetClumpIsBillboard(RpClump *pstClump, PVOID pvData);
	static RpAtomic*		CBSetIsBillboard(RpAtomic* pstAtomic, PVOID pvData);

	static VOID				SetGeometryIsBillboard(RpGeometry* pstGeom,int isBill); // 0 - false, 1 - true
	static int 				GetAtomicIsBillboard(RpAtomic* pstAtomic);

	static VOID				SetClumpMaterialAlphaGeometryFlagOff(RpClump *clump);
	static RpAtomic*		CBSetAtomicMaterialAlphaGeometryFlagOff( RpAtomic* atomic, void *data );		//2003_03_13 Seong Yon-jun	// Modify 2003_06_20 Bob.
	static RpGeometry*		CBSetGeometryMaterialAlphaFlagOff(RpGeometry *geom, void *data);
	static RpGeometry*		SetGeometryMaterialAlphaFlagOff(RpGeometry *geom);

	static void				DestroyAtomicUD(RpAtomic*	pAtomic);

	static	RwFrame*			CB_GetHeirarchy( RwFrame* lpFrm, void* lplpHierarch );
	static	RpHAnimHierarchy*	GetHierarchyFromFrame( RwFrame* pFrm );
	static	RpAtomic*			SetHierarchyForSkinAtomic( RpAtomic* atomic, void* data );

	// ���� �Ⱦ���..
	//	static RpGeometry *		CBSetGeometryData( RpGeometry *pstGeometry	, PVOID pvData		);
	//	static RpAtomic *		CBSetAtomicForGT	( RpAtomic *pstAtomic	, PVOID pvData			);

	//static void				SetDefaultClumpFrameUD(RwFrame*	frame);			// clump�� frame 
	//static void				SetDefaultAtomicGeometryUD(RpGeometry* pGeom,INT32	bClump);		// Clump�� �ִ�������
	//static void				SetDefaultAtomicFrameUD(RwFrame* pFrame);
	//static void				RearrangeAtomicUserdata(RpAtomic*	atomic);
	//static void				RearrangeClumpUserdata(RpClump*		clump);
	//static	RpAtomic*		CBRearrangeUD(RpAtomic*	pAtomic,PVOID pvData);

	// ������ ���� ������ ���� ����,, ���� ��Ʈ�� id������ ���� �ӽû��� �Ǵ� userdata�̴�! (gemani)
	// �� 1201 ũ���� ���� �迭 .. 400���� octreeid�� �����Ѵ�.
	// totalID �� 0�̸� ��� �ȵȰ���..(�ּ� 1)
	// 0��° index�� totalID , 
	//<1> 1: sectoriX , 2: sectoriZ, 3: OctreeID	//<2> 4: sectoriX , 5: sectoriZ, 6: OctreeID 
	//<3> 7: sectoriX , 8: sectoriZ, 9: OctreeID	//<4> 10: sectoriX , 11: sectoriZ, 12: OctreeID
	//<5> 13: sectoriX , 14: sectoriZ, 15: OctreeID	//<6> 16: sectoriX , 17: sectoriZ, 18: OctreeID
	//<7> 19: sectoriX , 20: sectoriZ, 21: OctreeID	//<8> 22: sectoriX , 23: sectoriZ, 24: OctreeID
	//<9> 25: sectoriX , 26: sectoriZ, 27: OctreeID	//<10> 28: sectoriX , 29: sectoriZ, 30: OctreeID
	// ... ���� ����
	static	INT32*			GetOcTreeID(RpClump*	clump);

	//@{ kday 20051013
	// ;)
	// �� Ÿ�Ͽ� ���ԵǴ� �ø���&���̴��� ������Ʈ�� �ﰢ���ε����� �̾Ƴ����� ����.
	typedef std::vector<INT32>	CONTAINER_TRIINDEX;
	static INT32			GetAllTriIndexThatIsInTile( CONTAINER_TRIINDEX& container, RpAtomic* atom
													  , const RwV3d& leftTop, const RwV3d& rightBottom);
	static INT32			IntsctLineTriInAtomic( RpAtomic* atom
												, const CONTAINER_TRIINDEX& triindices
												, const RwLine& line
												, BOOL	nearest = FALSE
												, RwReal* dist = NULL
												);
	//@} kday
};

#endif // !defined(AFX_ACUOBJECT_H__1801AA47_8F43_40BC_9B0C_2C7F1FD02115__INCLUDED_)
