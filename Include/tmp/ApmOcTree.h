#ifndef _APMOCTREE_H_
#define _APMOCTREE_H_

#include "ApModule.h"
#include "ApmMap.h"
#include "ApMemory.h"

#include "AuPackingManager.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApmOcTreeD" )
#else
#pragma comment ( lib , "ApmOcTree" )
#endif
#endif
//@} Jaewon

//����: RootNode�� BBOX size�� �� ���� width,height�� ���� . �׷��Ƿ� y�� -size/2 ���� size/2 �� ������ ���´�.
// ������ RootNode���� �����ϰ� RootNode�� ������ �����Ͽ� ���� ���� BBOX�� QuadTree�� ���´�.
// leaf�� �ƴϴ��� clump�� list�� ���´�.
// ��,���� fit�� node�� ���� �ȴ�.
// ���� ��� 8���� �ڽĿ� ��� ���� ��� �ڽ� ��忣 ���� �θ� ��� �ѱ����� ���� �ȴ�.. �ּ� ��ȸ�� ����

// �밢�� �Ÿ��� ���ϱ� ���� 1.732 (root3)������.. �ణ ū 1.75f ��������.
//#define		OCTREE_SPHEREVAL		1.732f
#define		OCTREE_SPHEREVAL		1.75f

// Octree���� �ִ� depth level
#define		OCTREE_MAXDEPTH			2
// octree���� ������ ApmOcTree���� �����ϰ� �д´�.
#define		OCTREE_MAX_ROOT_COUNT	4

typedef struct tag_OcTreeNode
{
	// ������ ������
	// 0 - 3200 , 1 - 1600 , 2 - 800 , 3 - 400 , 4 - 200 , 5 - 100 , 6 - 50 
	// ��: ID�� ����(bit����)	- level7 ���� ���´�.(0 base)(6400 size�������� 50 size) .. root���� childs�� �ʱ�ȭ�ÿ� �ڵ� ����
	// Ln = Level n�� Index, Depth = ���� node�� depth
	// IsLeaf = ���� node�� leaf���� �ƴ���..(Tree���� �ٴ´�,object�� �Ű� �Ⱦ�) ,Root Index = root list�� index
	// dummy(5):L7(3):L6(3):L5(3):L4(3):L3(3):L2(3):L1(3):Depth(3):IsLeaf(1):RootIndex(2)
	UINT32				ID;				
	tag_OcTreeNode*		parent;
	
	UINT16				objectnum;		// ���� ����� object ����
	UINT8				bHasChild;		// FALSE�̸� leaf
	UINT8				level;			// tree ���� ����

	UINT32				hsize;			// half-size

	tag_OcTreeNode*		child[8];

	AuSPHERE			BS;				// �ش� node�� BS(Frustum check& �浹 üũ��)
}OcTreeNode;

//	���η� �������� root�� �ü� �ִ�.(�ִ� 4��) ���� �� ���� ���
typedef struct tag_OcTreeRootList
{
	OcTreeNode*				node;
	INT32					rootindex;
	tag_OcTreeRootList*		next;
}OcTreeRootList;

typedef struct tag_OcTreeRoot
{
	INT8				sectorX;			// index( 16���� ���Ͱ� �ü� �ִ�,, �׷��Ƿ� INT8�� ���� )
	INT8				sectorZ;
	INT8				rootnum;
	INT8				pad;

	FLOAT				centerX;
	FLOAT				centerZ;
	
	OcTreeRootList*		roots;				// �갰�� ��쿣 ���η� �������� ������ü(6400size)�� �ü� �ִ�. �׷��Ƿ� list

	ApCriticalSection	m_csCSection;
}OcTreeRoot;

// ������Ʈ�� �ٴ� octree id list ..  ApmObject���� Streamming �Ѵ�.
struct OcTreeIDList: public ApMemory<OcTreeIDList, 12000>
{
	INT16			six;					// ���� 800���� ��� �ִ�.. �׷��Ƿ� 16bit�Ҵ�
	INT16			siz;
	UINT32			ID;

	OcTreeNode*		pAddedNode;
	PVOID			pAddedClumpListNode;	// clump����Ʈ���� �˻� ������ �ϱ� ���� �̸� ��� ����.. customdata listnode�� �̰� ����..
	OcTreeRoot*		pAddedRoot;

	OcTreeIDList*	next;

	//@{ kday 20050628
	// ;)
	OcTreeIDList() : six(0),siz(0),ID(0LU),pAddedNode(NULL),pAddedClumpListNode(NULL),pAddedRoot(NULL),next(NULL){};
	//@} kday
};

// custom data������ �̸� ID�� �����(����� Ǯ��) ���Ǵ� ����ü,, ���� �ش� ��⿡�� Streamming �ȴ�..
struct	OcTreeCustomID
{
	INT16			six;					// ���� 800���� ��� �ִ�.. �׷��Ƿ� 16bit�Ҵ�
	INT16			siz;
	UINT32			ID;

	OcTreeNode*		pAddedNode;
	PVOID			pAddedClumpListNode;	// clump����Ʈ���� �˻� ������ �ϱ� ���� �̸� ��� ����.. customdata listnode�� �̰� ����..
	OcTreeRoot*		pAddedRoot;
};

// ��� ����Ÿ �ε���
enum APMOCTREE_DATA_INDEX
{
	OCTREE_NODE_DATA,
	OCTREE_ROOT_DATA
};

enum APMOCTREE_NODES
{
	APMOCTREE_TOP_LEFT_BACK,
	APMOCTREE_TOP_LEFT_FRONT,
	APMOCTREE_TOP_RIGHT_BACK,
	APMOCTREE_TOP_RIGHT_FRONT,
	APMOCTREE_BOTTOM_LEFT_BACK,
	APMOCTREE_BOTTOM_LEFT_FRONT,
	APMOCTREE_BOTTOM_RIGHT_BACK,
	APMOCTREE_BOTTOM_RIGHT_FRONT
};

// �޸� �Ƴ��� ���ؼ� char������ ����.. �׷��� ���ؼ� enum�� ����.. 2005.3.29 gemani
#define		APMOCTREE_ROOT_STATUS_EMPTY		0
#define		APMOCTREE_ROOT_STATUS_LOADING	1
#define		APMOCTREE_ROOT_STATUS_LOADED	2
#define		APMOCTREE_ROOT_STATUS_REMOVED	3

typedef enum ApmOcTreeCallbackPoint	
{
	APMOCTREE_CB_ID_INIT_ROOT		= 0	
} ApmOcTreeCallbackPoint;

typedef void	(*ApmOcTreeSaveCallBackFunc) (HANDLE fd,DWORD* fp,INT32* foffset,OcTreeRoot*	root);	// agcmoctree���� �߰� ������ �����ϱ� ����
typedef void	(*ApmOcTreeLoadCallBackFunc) (DWORD* pLoadBuffer,INT32* pLoadIndex,OcTreeRoot*	root); // agcmoctree���� �߰� ������ �ε��ϱ� ����

/*#define			OCTREE_FO_NUM		8
// �ִ� �������� LRU �˰��� ���� �����Ѵ�. fileopen�� ���̱� ���ؼ� handle���� .. FOList == FileOpenedList ^^..
typedef struct tag_OcTreeFOList
{
	INT32					lx;
	INT32					lz;

	HANDLE					fd;
	tag_OcTreeFOList*		next;
}OcTreeFOList;*/

class ApmOcTree : public ApModule  
{
	OcTreeNode*	MakeChild(OcTreeNode*	node,APMOCTREE_NODES	pos);

public:
	ApmOcTree();
	~ApmOcTree();

	BOOL	OnAddModule	();
	BOOL	OnInit		();
	BOOL	OnDestroy	();

	INT32		AttachOcTreeNodeData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT32		AttachOcTreeRootData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	BOOL		SetCallBackSave(ApmOcTreeSaveCallBackFunc	pFunc);
	BOOL		SetCallBackLoad(ApmOcTreeLoadCallBackFunc	pFunc);

	// octree���� ��带 ���� 
	OcTreeNode*		CreateRoot(INT32	sectorix,INT32	sectoriz,FLOAT cy, OcTreeRoot**	root);	// world y�� �ѱ��. root�� �����ϸ� ����Ʈ �߰�
	void			DivideNode(OcTreeNode*	node);				// ���� ��带 8���� ����(���� ���� leaf �̾�� ��)
	
	void			CombineNodeChilds(OcTreeNode*	node);		// ���� ����� �ڽĵ��� ����
	
	OcTreeRoot*		GetRoot(INT32	sectorix,	INT32	sectoriz);		// sector index�� �Ѱܼ� root�� �����´�.
	OcTreeRoot*		GetRootByWorldPos(FLOAT	world_x, FLOAT world_z, BOOL bLoad = FALSE);			// world x,z�� �Ѱܼ� root�� �����´�.

	BOOL			SetRoot(INT32	sectorix,	INT32	sectoriz,	OcTreeRoot*	root);
		
	OcTreeNode*		GetNode(OcTreeRoot*		root,UINT32 ID);
	OcTreeNode*		GetNode(OcTreeNode*		start,UINT32 ID);

	OcTreeNode*		GetNode(AuPOS*	pos);								// pos�� �Ѱܼ� leaf�� �����´�.
	OcTreeNode*		GetNode(OcTreeNode*		start,AuPOS*	pos);
	
	OcTreeNode*		GetStartNode(OcTreeRoot*	root , INT32	rootindex);
	OcTreeNode*		GetStartNode(FLOAT x,FLOAT y,FLOAT z);		// world x,y,z �� �Ѱܼ� ���� node�� �����´�.
	OcTreeNode*		GetStartNode(OcTreeNode*		node);		// ���� node�� �����´�. ���� node��..
	UINT32			GetNodeID(OcTreeNode*			node);

	// ���� octree�� �ش�ID��ŭ ���ҵ��� �ʾҰ� bDivide�� True�̸� �ش� IDlevel���� �����Ͽ� �����Ѵ�. 
	OcTreeNode*		GetNodeForInsert(INT32 six,INT32 siz,UINT32	octreeID,BOOL	bDivide = TRUE);

	BOOL	TestBoxPos(AuPOS*	worldpos,AuPOS*	boxcenter,FLOAT	boxsize);

	void	DestroyTree(OcTreeRoot*		root);
	void	DestroyChilds(OcTreeNode*	node);

	// ������ (2005-11-24 ���� 5:03:57) : ��â ������.
	void	DestroyAll();

	// (������)
	// Octree test�� (���� ��� �������� �������� node�� �߰��ϰ� ���������� optimizing�Ѵ�.)
	void	DivideAllTree(OcTreeRoot*	root);
	void	DivideAllChilds(OcTreeNode*	node);

	// �ڽ��� ��� null�̸� �������ִ� �Լ�
	void	OptimizeTree(INT32 six,INT32 siz);
	int 	OptimizeChild(OcTreeNode*	node);

	// ���� pos���� leaf�� ��ġ ID�� ����ؼ� return(object�� - root�� ������ ���� ó�� X)
	// 0xffffffff�� �����ϸ� error
	UINT32	GetLeafID(AuPOS*	pos);

	// File �����
	BOOL	SaveToFiles(char*	szDir,INT32	Loadx1,INT32	Loadx2,INT32	Loadz1,INT32	Loadz2);
	void	SaveNode(OcTreeNode*	node,INT32*		Foffset,HANDLE	fd,DWORD*	FP);

	OcTreeRoot*		LoadFromFiles(INT32	six,INT32 siz);
	void	LoadNode(OcTreeNode*	node,DWORD* pLoadBuffer,INT32*	pLoadIndex);

	BOOL	SetCallbackInitRoot	( ApModuleDefaultCallBack pfCallback, PVOID pClass	);

	static BOOL	CB_LoadSector			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_ClearSector			( PVOID pData, PVOID pClass, PVOID pCustData );

	inline	UINT32	CalcRootIndex(UINT32	ID){return	(ID & 0x3); }
	inline	UINT32	SetRootIndex(UINT32	ID,UINT32 rootindex){return	((ID & 0xfffffffc) | (rootindex & 0x3)); }
	inline	UINT32	CalcDepth(UINT32	ID){return	((ID & 0x38) >> 3); }
	inline	UINT32	SetDepth(UINT32	ID,UINT32	depth){return ((ID & 0xffffffc7) | ((depth & 0x7) << 3)); }

	inline	UINT32	SetIsLeaf(UINT32	ID,BOOL	bLeaf) {return (ID | ((bLeaf & 0x1) << 2)); }

	// 1~7
	inline	UINT32	CalcIndex(UINT32	ID,INT32 lev){return	((ID & m_uiIndexMask[lev]) >> m_uiIndexShift[lev]); }

	inline	void	SetOcTreeEnable( BOOL	bEnable ) { m_bOcTreeEnable = bEnable; }
		
// Datas
	BOOL			m_bOcTreeEnable;
	ApmMap*			m_pApmMap;
	
	ApmOcTreeSaveCallBackFunc		m_pfSaveCB;
	ApmOcTreeLoadCallBackFunc		m_pfLoadCB;

	OcTreeRoot*		m_pOcTreeRoots[ MAP_WORLD_INDEX_WIDTH ][ MAP_WORLD_INDEX_HEIGHT ];
	char			m_aeOcTreeRootStatus[ MAP_WORLD_INDEX_WIDTH ][ MAP_WORLD_INDEX_HEIGHT ];

	UINT32			m_uiIndexMask[8];
	UINT32			m_uiIndexShift[8];

	ApCriticalSection	m_csCSection;

	ApWorldSector*		m_pCenterSector;			// ���ΰ��� ���ִ� ����(release���ؼ�..)
	INT16				m_iLoadRange;				// Release����

	INT16				m_iCurLoadVersion;

	//OcTreeFOList*		m_listFO;
	//INT32				m_iFOCount;					

	//HANDLE	GetHandle(INT32 lx,INT32 lz);
};

#endif