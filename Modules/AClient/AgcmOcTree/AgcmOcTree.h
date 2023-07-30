#ifndef _AGCM_OCTREE_H_
#define _AGCM_OCTREE_H_

#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rtintsec.h>
#include <rtpick.h>
#include <rtbary.h>
#include <AgcModule/AgcModule.h>
#include <ApBase/ApBase.h>
#include <AcuObject/AcuObject.h>
#include <ApmOcTree/ApmOcTree.h>
#include <ApmMap/ApmMap.h>
#include <AcuFrameMemory/AcuFrameMemory.h>
#include <AcuIMDraw/AcuIMDraw.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmOcTreeD" )
#else
#pragma comment ( lib , "AgcmOcTree" )
#endif
#endif

// ������ (2004-04-07 ���� 11:22:17) : DLL �������� �ȵż� ���� ����.
#pragma comment ( lib , "rtintsec" )
#pragma comment ( lib , "rpcollis" )
#pragma comment ( lib , "rpworld" )
#pragma comment ( lib , "RtRay" )

// note:
// bbox��ſ� 4���� �� list�� ����. �Ʒ����� ��� üũ �� �ʿ䰡 ����. ���� �˰��� �̹Ƿ�
// �������� ����� ������ ����˻�� draw�� ���Ѵ�. ###
// ���� �˻縦 �Ҷ� octree ��尡 �������� �˻��Ѵ�. �Ϻ� ��带 ���� ���ܽ�ų�� �մ�.

// ��� ���� ���� .. ī�޶󿡼� far plane ����
// 4���� �ּ� y �׸��� �ִ�,�ּ� x�� ���� ������ �����Ѵ�.

// render���� octree�����ϴ� �Լ��� �д�.
// ������ apmoctree���� �ϱ� agcmoctree���� ������ ���� bbox�� streamcallback �޾Ƽ� �Ѵ�.

// ���� minbox�� quadtree �ڷ����¸� �����ϴ�. �׸� ���� enum��
enum AGCMQTREE_NODES
{
	AGCMQTREE_LEFT_BACK,
	AGCMQTREE_LEFT_FRONT,
	AGCMQTREE_RIGHT_BACK,
	AGCMQTREE_RIGHT_FRONT
};

#define		OC_CLUMP_ATOMIC_LIST_SIZE		12
struct OcClumpAtomicList:public ApMemory<OcClumpAtomicList, 40000>
{
	RpAtomic*					atomic;	
	//@{ Jaewon 20050602
	// Just use atomic->renderCallBack.
	//RpAtomicCallBackRender		renderCB;
	//@} Jaewon
	OcClumpAtomicList*			next;
};

struct OcClumpList:public ApMemory<OcClumpList, 20000>
{
	RpClump*					clump;
	RwSphere					BS;						// �̵��� ���ٰ� ����.. �̵� �ϴ� ���� ���忡 ��!^^

	//@{ Jaewon 20050608
	// Just use clump->atomicList.
	//OcClumpAtomicList*			atomiclist;				// �ش� clump�� atomic�� list�� ���´�.
	//@} Jaewon

	OcClumpList*				next;
	OcClumpList*				prev;
};

struct OcCustomDataList
{
	RwSphere					BS;						// �̵��� ���ٰ� ����.. �̵� �ϴ� ���� ���忡 ��!^^
	RwV3d						TopVerts[4];
	
	ApModuleDefaultCallBack		pRenderCB;
	ApModuleDefaultCallBack		pUpdateCB;

	PVOID						pClass;
	PVOID						pData1;
	PVOID						pData2;

	INT32						iAppearanceDistance;
	//@{ 2006/11/17 burumal
	ApModuleDefaultCallBack		pDistCorrectCB;
	//@}

	INT32*						piCameraZIndex;		// grass����ü�� effect����ü�� int������ �ΰ� �ּҸ� �ѱ��.
													// �׷� render�ÿ� camera z index�� �� �����Ϳ� set���ش�.(0 - 10, 0 - near , 9 - far)
													// ������ �ִ� �͸� update�Ϸ� �Ҷ� �����ϴ�.^^
	
// �� Data set�ؼ� AddCustomRenderDataToOcTree �Լ��� �ѱ��.
	INT32*						iRenderTick;			// �ߺ� rendering ����

	OcCustomDataList*		next;
	OcCustomDataList*		prev;
};

// Node�� �ٴ� attachdata
typedef struct tag_OcNodeData
{
	OcClumpList*		clump_list;
	OcCustomDataList*	custom_data_list;				// Ǯ�̳� effect���� �͵�
}OcNodeData;

// level2 (800 size)���� ���ҵȴ� .. �׷� root�� leaf ��� �Ѱ����� 64���̴�. leaf�� occluder�� �߰�
typedef struct OcRootQuadTreeNode
{
	INT8						level;
	INT8						isleaf;
	INT16						hsize;

	OcRootQuadTreeNode*		child[4];
	RwV3d						topVerts[4];
	RwV3d						worldcenter;
}OcRootQuadTreeNode;

// Root�� �ٴ� attachdata
typedef struct tag_OcRootData
{
	RpAtomic*					terrain;
	OcRootQuadTreeNode*			child[4];
}OcRootData;

// OcTree ������ �ʿ��� ����ü��(���� �߽ɿ� ���� 4���� ��尡 ���ϵɼ� �ִ�).. ����� Ž����.
typedef struct tag_WhereQuadNodes
{
	int							num;
	OcRootQuadTreeNode*			quads[4];
}WhereQuadNodes;

#define			OCID_DATA_SIZE	24
// clump�� ��� octree node�� ���ϴ��� �˾Ƴ��� ���� ����ü
typedef struct tag_WhereOcIDs
{
	INT32						six;
	INT32						siz;
	INT32						ID;

	BOOL						bSearch;
	INT32						cur_level;

	tag_WhereOcIDs*				next;
}WhereOcIDs;

// Render���� ����.. AgcdObject���� ����� ���� ����
typedef struct tag_OcTreeRenderData
{
	RwV3d		topVerts_MAX[4];		// �ִ� BBOX

	INT8		bMove;					// �̵� ���� ��ü�� ��쿣 local�� verts���� ���� �Ź� �����ش�.. �ƴϸ� world space ��ȯ�� ����
	INT8		isOccluder;
	INT8		nOccluderBox;
	INT8		pad;

	RwV3d*		pTopVerts;				// �ּ� BBOX(Occluder�� ��츸)
	
	//OcTreeIDList*	ID;					// ApdObject�� �ִ� IDlist�� ���������� ����Ų��.
}OcTreeRenderData;

// Item,Character���� occluder�� �ۿ��� ���� ���� ���� ���� ����ü(�޸� ���� ����)..AgcdItemTemplate,AgcdCharacterTemplate�� ����� ����
typedef struct tag_OcTreeRenderData2
{
	RwV3d		topVerts_MAX[4];		// �ִ� BBOX
}OcTreeRenderData2;

// IntersectionLine�� �ɸ� sector���� ���� ����Ʈ
#define			INTERSECTION_SECTORS_SIZE	8
typedef struct tag_IntersectionSectors
{
	ApWorldSector*				pSector;
	tag_IntersectionSectors*	next;
}IntersectionSectors;

typedef void	(*AgcmOcTreeIntersectionCBFunc) (RpIntersection*	intersection,
							 	RpIntersectionCallBackAtomic    callBack,    void *    data);

#define QUAD_MAX_DEPTH	2

class AgcmOcTree : public AgcModule
{
public:
	static AgcmOcTree *			m_pThis;
	
public:
	ApmOcTree*		m_pApmOcTree;
	ApmMap*			m_pApmMap;

	RpWorld*		m_pWorld;

	// ApmOcTree attach data index
	int				m_iRootDataIndex;
	int				m_iNodeDataIndex;

	// octree �߰� �˻��(����ÿ�)
	WhereOcIDs*		m_listFindID;

	// octree draw��(�������� ����� ȭ�� ǥ�ÿ�)
	BOOL			m_bDrawOcTree;
	RpClump*		m_pSelectedClump;

	//ApCriticalSection	m_csCSection;
	
public:
	AgcmOcTree();
	virtual ~AgcmOcTree();

	#ifdef USE_MFC
	protected:
		ApWorldSector * m_pSelectedSector;
	public:
		void SetOctreeDebugSector( ApWorldSector * pSector = NULL );
		void DrawOctreeDebugSectorBox();
	#endif // USE_MFC

	BOOL                OnInit();
	BOOL                OnAddModule();
	BOOL                OnIdle(UINT32 ulClockCount);	
	BOOL                OnDestroy();

	OcRootData *	GetRootData(OcTreeRoot *pRoot)	{ return (OcRootData*) m_pApmOcTree->GetAttachedModuleData(m_iRootDataIndex,(PVOID)pRoot);}
	OcNodeData *	GetNodeData(OcTreeNode *pNode)	{ return (OcNodeData*) m_pApmOcTree->GetAttachedModuleData(m_iNodeDataIndex,(PVOID)pNode);}

	inline	void	SetRpWorld(RpWorld*		pWorld) { m_pWorld = pWorld; }

	void	DivideRootQuadTree( OcRootQuadTreeNode*		node, int level);
	void	RemoveRootQuadTree( OcRootQuadTreeNode*		node);

	BOOL	AddClumpToOcTree( RpClump*	pClump );
	BOOL	RemoveClumpFromOcTree( RpClump*	pClump );

	//OcCustomDataList ����ü�� data�� set���� �ѱ��.		
	BOOL	AddCustomRenderDataToOcTree( FLOAT x, FLOAT y,FLOAT z, OcCustomDataList* pStruct);
	BOOL	RemoveCustomRenderDataFromOcTree( FLOAT x,FLOAT y,FLOAT z,PVOID pClass, PVOID pData1, PVOID pData2);

	BOOL	AddCustomRenderDataToOcTree( OcTreeCustomID* IDList, OcCustomDataList* pStruct);		// octree ID�̸� setting�Ȱ��
	BOOL	RemoveCustomRenderDataFromOcTree( OcTreeCustomID*	IDList, PVOID pClass, PVOID pData1, PVOID pData2);
	
	//@{ Jaewon 20050608
	// Just use clump->atomicList.
	//BOOL	AddAtomicToOcTree( RpAtomic*	pAtomic);		// UDA�� ���� add�Ǵ� ���� ���� ó���ϱ� ����.. ������ �Ⱦ��δ�..2005.4.18 gemani
	//@} Jaewon

	//@{ Jaewon 20050608
	// Just use clump->atomicList.
	//void	SetAtomicList(RpClump*	pClump,OcClumpList*	clump_list);
	//static	RpAtomic*	SetAtomicListCB( RpAtomic *atomic, void *data );
	//@} Jaewon
	
	// octree������ 
	void	CreateRootByTerrain( RpAtomic*	atomic, ApWorldSector * pSector );
	void	DivideToLeafTree(INT32 six, INT32 siz);

	// clump�� �ش� apmoctree�� node�� �����ϰ� OcClumpList�� set�Ѵ�.
	BOOL	TestClumpOctrees(RpClump*	pClump);
	static	RpAtomic*	CallBackSetOcTreeIDs(RpAtomic*		atomic, void* data);
	BOOL	TestAtomicOctreesByClump(RpAtomic*		pAtomic,WhereOcIDs**	pIDs);

	void	TestAtomicOctrees(RpAtomic*	pAtomic);
	
	void 	GetQuadNodes(FLOAT x,FLOAT z,WhereQuadNodes*	pData,OcRootQuadTreeNode*	pQuadNode);
	
	void	SetAllQuadY(OcRootData*		pRoot,FLOAT y);
	void	SetAllQuadY(OcRootQuadTreeNode*		pNode,FLOAT y);

	// �������� ȭ�� ������
	void	SetClumpAndDrawStart(RpClump*	pClump);
	void	SetDrawEnd();
	void	DrawDebugBoxes();							// �ش� clump�� �����ִ� sector���� octree node��� 
														// clump�� ���� node�� �ٸ� ������ ȭ�鿡 ǥ��..
	void	DrawOcTreeNodes(OcTreeNode*		node,INT32 six,INT32 siz,OcTreeIDList**		IdList);
	void	DrawQuadNodes(OcRootQuadTreeNode*	qnode);

	void	SaveQuadTreeBoxes(OcRootQuadTreeNode*	qnode,HANDLE fd,DWORD* fp,INT32* foffset);
	void	LoadQuadTreeBoxes(OcRootQuadTreeNode*	qnode,DWORD* pLoadBuffer,INT32* pLoadIndex);

	static BOOL	CBInitRootData ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBRemoveRootData ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBInitNodeData ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBRemoveNodeData ( PVOID pData, PVOID pClass, PVOID pCustData );

	static void	CBSaveRootData (HANDLE fd,DWORD* fp,INT32* foffset,OcTreeRoot*	root);	
	static void	CBLoadRootData (DWORD* pLoadBuffer,INT32* pLoadIndex,OcTreeRoot*	root); 

	//RpWorldForAllClumps�� ����, �ش� root�� ��� Ŭ������ callback�� ȣ��
	BOOL	OcTreeForAllClumps(INT32 six,INT32 siz,RpClumpCallBack    fpCallBack, void *    pData, INT32* pCorruptCheckArray);

	void	NodeForAllClumps(OcTreeNode*	node,RpClumpCallBack    fpCallBack, void *    pData,void*	pCheckArray);

	//�ݡݡݡݡݡݡݡݡ� Intersection �� �ݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡ� add by gemani(RpWorld �Ⱦ���)
	AgcmOcTreeIntersectionCBFunc			m_pfWorldIntersection;

	BOOL				SetWorldIntersectionCallBack(AgcmOcTreeIntersectionCBFunc	pFunc);
	
	UINT32				m_ulCurTick;

	void				OcTreeChildCheck(OcTreeNode*	node,RpIntersection *    intersection,  
										RpIntersectionCallBackAtomic    callBack,    void *    data);

	// �ش� ���� ��Ʈ���� ���� ���ͼ����� ���Ѵ�..
	void				OneOcTreeForAllAtomicsIntersection( INT32	six, INT32 siz , RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data );

	// �ش� ���ο� �ش��ϴ� ��Ʈ ����Ʈ�� �����´�
	IntersectionSectors*	LineGetIntersectionSectors(RwLine*	line);

	// ���� �ε��� ��� octree node�� üũ�Ѵ�
	// ID�� �ߺ� Intersectionüũ�� �����ϱ� ���� �ο��� ��ȣ
	// AgcmTargeting - 0 , AgcmLensFlare - 1, AgcmCamera - 2,
	// AgcmMap::GetMapPositionFromMousePosition - 3,
	// AgcmMap::GetCursorClump - 4

	// ���� ,object,character �� üũ�Ѵ�!!
	void				OcTreesForAllAtomicsIntersection( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// ������ �����ϰ� world�� object üũ�Ѵ�!!
	void				OcTreesForAllAtomicsIntersection2( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// object�� üũ�Ѵ�!!(ī�޶󿡼� ����!)
	void				OcTreesForAllAtomicsIntersection3( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// character�� üũ�Ѵ�!!(debug��)
	void				OcTreesForAllAtomicsIntersection4( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// �����ϰ� üũ�Ѵ�!!(ī�޶󿡼� ����!)
	BOOL				OcTreesForAllAtomicsIntersection5(RpIntersection *    intersection, BOOL bLock = TRUE );

	// ��Ʈ���� ��� Ŭ������ ���� callback�� �����Ѵ�..
	// ������ (2003-11-20 ���� 3:17:52) : Ŭ���� üũ�Լ� �߰�.
	void				OcTreesForAllClumps( INT32				ID			,	// ���� ��� ���ϳ���.. ���� 0 �ְ� ������..
											RpClumpCallBack		pCallBack	,
											void *				pData		,
											BOOL				bLock = TRUE );
};

#endif