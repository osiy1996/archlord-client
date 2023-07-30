// AgcmMap.h: interface for the AgcmMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMMAP_H__91149629_2429_47C8_9C28_5ECF4DCC6645__INCLUDED_)
#define AFX_AGCMMAP_H__91149629_2429_47C8_9C28_5ECF4DCC6645__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ApBase.h>
#include <rwcore.h>
#include <rpworld.h>

#include <ApmMap.h>
//#include <ApmObject.h>
//#include <AgcmObject.h>

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "AcTextureList.h"
#include "AcMaterialList.h"
#include "AcMapLoader.h"
#include "AgcmResourceLoader.h"

#include "AgcmRender.h"
#include "MagImAtomic.h"

#include "AgcmOcTree.h"
#include "ApmOcTree.h"

// �� Ŭ���̾�Ʈ ���μ�
// Ư���� �ϴ����� �ɸ��� �������� �Է� �ް�
// �ش� �������� �߽����� ���� �ε��ϰ�
// ������� �ʴ� ���� �޸𸮿��� �����Ѵ�.

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmMapD" )
#else
#pragma comment ( lib , "AgcmMap" )
#endif
#endif


#define	OBJECT_FILE_FORMAT				"%d,%d.aof"
#define	ALEF_SECTOR_DUMMY_DEPTH			1
#define	SECTOR_DFF_FILE_FORMAT_ROUGH	"R%d,%d.dff"
#define	SECTOR_ROUGH_TEXTURE			"R%d,%d"
#define	SECTOR_DFF_FILE_FORMAT_DETAIL	"D%d,%d.dff"
#define	SECTOR_TEMP_DIRECTORY			"$$TEMP$$"

typedef enum
{
	AGCM_CB_ID_ONLOAD_MAP		= 0	,
	AGCM_CB_ID_ONUNLOAD_MAP			,
	AGCM_CB_ID_MAP_LOAD_END			,	// Map Load End Callback : ���Ǻη� EnumCallback �� �Ҹ��� ( AgcmMapMapLoadEndCBMode ���� )
	AGCM_CB_ID_MAP_SECTOR_CHANGE_PRE,	// Sector Change�� �Ǳ� ����.
	AGCM_CB_ID_MAP_SECTOR_CHANGE	,	// Sector Change�� �� �Ŀ�. ObjectShadow�� sector��ȯ�ÿ� ������ ����....(gemani)
	AGCM_CB_ID_ONGETHEIGHT			,	// ���� ���� �ݹ�. ���ڴ�..stCollisionAtomicInfo * 
	AGCM_CB_ID_SETAUTOLOADCHANGE		// ����ε� �Ÿ� ����� ȣ��Ǵ� �ݹ�.
};

// Map Load End Callback �� ���� ���� Mode
enum	AgcmMapMapLoadEndCBMode
{
	AGCMMAP_MAPLOADEND_CB_MODE_OFF	=	0	,		// �̻��¿����� Map Load End Callback �� �۵����� �ʴ´� 
	AGCMMAP_MAPLOADEND_CB_MODE_ON			,		// On���� �ٲ�� MyCharacterPositionChange()�Լ��� �����⸦ ��ٸ���  
	AGCMMAP_MAPLOADEND_CB_MODE_LOAD_START			// MyCharacterPositionCharacter() �Լ��� ������ Load Start�� �ǰ� �� ���¿��� m_listSectorQueue == 0 �̸� EnumCallback!
};

// DWSector
	#define rwVENDORID_NHN			(0xfffff0L)
	#define rwID_DWSECTOR_NHN		(0x03)
	#define rwID_DWSECTOR_DATA_NHN	(0x04)

	#define rwID_DWSECTOR			MAKECHUNKID(rwVENDORID_NHN, rwID_DWSECTOR_NHN)
	#define rwID_DWSECTOR_DATA		MAKECHUNKID(rwVENDORID_NHN, rwID_DWSECTOR_DATA_NHN)

	typedef enum RpDWSectorFields
	{
		rpDWSECTOR_FIELD_NONE			= 0x0000,
		rpDWSECTOR_FIELD_VERTICES		= 0x0100,
		rpDWSECTOR_FIELD_POLYGONS		= 0x0200,
		rpDWSECTOR_FIELD_NORMALS		= 0x0400,
		rpDWSECTOR_FIELD_PRELITLUM		= 0x0800,
		rpDWSECTOR_FIELD_FLAG_TEXCOORDS	= 0x00ff
	} RpDWSectorFields;

	typedef struct RpDWSector RpDWSector;
	struct RpDWSector
	{
		RpGeometry			*	geometry	;			// terrain geometry
		RpAtomic			*	atomic		;			// terrain atomic
		RwTexDictionary		*	texdict		;			// texture dictionary
	};
	
enum RoughPolygonType
{
	R_LEFTTOP		,
	R_TOP			,
	R_RIGHTTOP		,
	R_LEFT			,
	R_CENTER		,
	R_RIGHT			,
	R_LEFTBOTTOM	,
	R_BOTTOM		,
	R_RIGHTBOTTOM	,

	R_TYPETOTAL
};	

#include "DWSector.h"
	
struct	rsDWSectorInfo
{
	RpDWSector				*pDWSector		;
	int						nDetail			;	// Current Detail
	RpMorphTarget			*pMorphTarget	;
	RpTriangle				*polygons		;
	RwV3d					*vertices		;
	RwV3d					*normals		;
	RwTexCoords				*texCoords[rwMAXTEXTURECOORDS]	;
	RwRGBA					*preLitLum		;

	int						numPolygons		;
	int						numVertices		;
};

struct	stFindMapInfo
{
	BOOL			bSuccess				;
	ApDetailSegment	*pSegment				;	// ���׸�Ʈ ������.
	ApWorldSector	*pSector				;	// ���弽��.
	int				nSegmentX				;
	int				nSegmentZ				;
	
	BOOL			bEdgeTurn				;
	int				nFirstVertexIndex		;
	int				nPolygonIndexFirst		;	// ������ ó�� �ε���..

	// ���� ���� �� ����..
	BOOL			bSelected				;

	int				nCurrentDetail			;

	void	Empty()
	{
		bSuccess				= FALSE	;
		nPolygonIndexFirst		= 0		;
		pSector					= NULL	;
		pSegment				= NULL	;
		nSegmentX				= 0		;
		nSegmentZ				= 0		;
		bEdgeTurn				= FALSE	;
		nFirstVertexIndex		= 0		;
		bSelected				= FALSE	;
		nCurrentDetail			= SECTOR_EMPTY;
	}
};

struct	stCollisionAtomicInfo
{
	FLOAT			fX						;
	FLOAT			fZ						;
	FLOAT			fHeight					;	// �ʸ�⿡�� �� ����
	FLOAT			fCharacterHeight		;	// �ɸ��� ����
	INT32			nObject					;	// ������Ʈ ī��Ʈ
	INT32 *			aObject					;
	UINT8			uRidableType			;	// �ö� ������Ʈ �ε���..

	stCollisionAtomicInfo():nObject( 0 ),aObject( NULL ),uRidableType( APMMAP_MATERIAL_SOIL ){}
};

class AgcmMap : public AgcModule  
{
public:
	DECLARE_SINGLETON( AgcmMap )

	// ���������θ� �����..
	// �ʸ�� ������..
	ApmMap *		m_pcsApmMap					;
	AgcmRender *	m_pcsAgcmRender				;
	AgcmResourceLoader *	m_pcsAgcmResourceLoader	;
	AgcmOcTree*		m_pcsAgcmOcTree				;
	ApmOcTree*		m_pcsApmOcTree				;

	INT32					m_lLoaderChangeID		;
	INT32					m_lLoaderDestroyID		;

	// ���ͷε�/��Ʈ���� �ݹ�..
	void		( *	m_pfSectorWaterLoadCallback		)( ApWorldSector * pSector , int nDetail	);
	void		( *	m_pfSectorWaterDestroyCallback	)( ApWorldSector * pSector , int nDetail	);
	RpAtomic *	( *	m_pfGetCollisionAtomicCallback	)( RpClump * pClump , PVOID pvData			);
	RpAtomic *	( *	m_pfGetCollisionAtomicCallbackFromAtomic	)( RpAtomic * pClump , PVOID pvData			);

	void		( * m_pfOnFirstMyCharacterPositionCallback	)();

	inline	void	SetCollisionAtomicCallback	( RpAtomic * ( * pCallback	)( RpClump * pClump , PVOID pvData ) )
	{	m_pfGetCollisionAtomicCallback	= pCallback;	}
	inline	void	SetCollisionAtomicCallbackFromAtomic	( RpAtomic * ( * pCallback	)( RpAtomic * pAtomic , PVOID pvData ) )
	{	m_pfGetCollisionAtomicCallbackFromAtomic	= pCallback;	}
	inline	void	SetSectorWaterCallback		( void	( *	pLoadCallback )( ApWorldSector * pSector , int nDetail ) , void ( * pDestroyCallback )( ApWorldSector * pSector , int nDetail ) )
	{ m_pfSectorWaterLoadCallback = pLoadCallback; m_pfSectorWaterDestroyCallback = pDestroyCallback; }
	inline	void	SetFirstMapLoadCallback		( void	( *	pCallback )() )
	{ m_pfOnFirstMyCharacterPositionCallback = pCallback; }

	// �������� ���� ������..
	struct	AgcmMapSectorData
	{
		DWSectorData	m_DWSector		;
		AcMaterialList	m_MaterialList[ SECTOR_DETAILDEPTH	];

		#ifdef USE_MFC
		BOOL			m_bValidGrid;
		RwIm3DVertex	m_pImVertex[ 70 ];	// �׵θ� ���ؽ� ����.

		void *			m_pLoadedData1; // Rough ����
		void *			m_pLoadedData2;	// Detail ����
		#endif // USE_MFC
	};

	AcTextureList	m_TextureList				;

	BOOL			m_bInited					;
	AuPOS			m_posCurrentAvatarPosition	;	// �������� ������ �ɸ��� ��ġ..
	BOOL			m_bAvatarPositionSet		;	// Avatar Position�� Setting�Ǿ��°� (�ܺο��� MyCharacterChangePosition �� �ҷ��°�?)

	char			m_strToolImagePath[ 256 ]	;	// ������ ����ϴ� �̹����� ����ִ� ���丮 ��ġ.

	BOOL			m_bFirstPosition			;
	void			DeleteMyCharacterCheck()	;

	AcMapLoader		m_csMapLoader				;	// Map Loading Thread

	RwCamera *		m_pstCamera					;
	UINT32			m_ulPrevLoadTime			;

	BOOL			m_bUseTexDict				;

	CHAR			m_szTexturePath[ 128 ]		;	// Texture Path�� �������ش�. ResourceLoader�� �߰��� ��� (Parn)

	/************************ 2003_11_14 98pastel : Map Load End CB ���� ******************************/
	INT32			m_lMapLoadEndCBMode			;	// Map Load End CB Mode : AgcmMapMapLoadEndCBMode�� ���� 
	void			SetMapLoadEndCBActive() { m_lMapLoadEndCBMode = AGCMMAP_MAPLOADEND_CB_MODE_ON; }
	BOOL			SetCallbackMapLoadEnd(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) 
						{ return SetCallback(AGCM_CB_ID_MAP_LOAD_END, pfnCallback, pvClass); }
	/************************ 2003_11_14 98pastel : Map Load End CB ���� ******************************/
	

	INT32			EnumLoadedDivision		( DivisionCallback pCallback , PVOID pData );
	// ������ ����� ī��Ʈ..

	static const int	EnumDivisionExpandValue = 2;
	INT32			EnumLoadedDivisionObject( DivisionCallback pCallback , PVOID pData );
	// ������ ������Ʈ�� �ε��ϱ� ���� �����¿� ������� �߰��� Enum�Ѵ�.

protected:
	RtWorldImport *	CreateWorldImport()			;

	BOOL			m_bInitialized				;
	void			SetInitialize(	BOOL bInit = TRUE ){ m_bInitialized = bInit; }
	BOOL			IsInitialized(){ if ( m_bInitialized ) return TRUE ; else return FALSE; }

	INT16				m_nSectorAttachedDataOffset	;

	// Load Range
	BOOL			m_bRangeApplied	;
	INT32			m_nLoadRangeX1	;
	INT32			m_nLoadRangeZ1	;
	INT32			m_nLoadRangeX2	;
	INT32			m_nLoadRangeZ2	;

	RwInt32			m_nGeometryLockMode		;
	RwInt32			m_nVertexColorLockMode	;

	RpWorld		*	m_pWorld				;

	// �ε� ������ ����..
	INT32			m_nMapDataLoadRadius	;
	INT32			m_nMapDataReleaseRadius	;
	INT32			m_nRoughLoadRadius		;	// �ʻ�Ÿ�..�������� �ε��Ŵ� ����.
	INT32			m_nDetailLoadRadius		;	// �ʻ�Ÿ�..������ ���� �ε��Ŵ� ����.
	INT32			m_nRoughReleaseRadius	;	// �ʻ�Ÿ�..�������� �޸𸮿��� ���ŵŴ� ����
	INT32			m_nDetailReleaseRadius	;	// �ʻ�Ÿ�..�����ϸ��� �޸𸮿��� �����Ŵ� ����.

	INT32			m_lSectorsToLoad		;	// Load�ؾ��� Sector�� ����
					
public:
	AgcmMapSectorData * GetAgcmAttachedData( ApWorldSector * pSector )
	{
		return ( AgcmMapSectorData * )m_pcsApmMap->GetAttachedModuleData( m_nSectorAttachedDataOffset , ( PVOID ) pSector );
	}

	inline INT32	GetMapDataLoadRadius	()	{ return m_nMapDataLoadRadius	; }
	inline INT32	GetRoughLoadRadius		()	{ return m_nRoughLoadRadius		; }
	inline INT32	GetDetailLoadRadius		()	{ return m_nDetailLoadRadius	; }
	inline INT32	GetMapDataReleaseRadius	()	{ return m_nMapDataReleaseRadius; }
	inline INT32	GetRoughReleaseRadius	()	{ return m_nRoughReleaseRadius	; }
	inline INT32	GetDetailReleaseRadius	()	{ return m_nDetailReleaseRadius	; }

	inline RwInt32	GetGeometryLockMode		(					) { return m_nGeometryLockMode		;				}
	inline RwInt32	SetGeometryLockMode		( RwInt32 nLockMode	) { return m_nGeometryLockMode		= nLockMode;	}
	inline RwInt32	GetVertexColorLockMode	(					) { return m_nVertexColorLockMode	;				}
	inline RwInt32	SetVertexColorLockMode	( RwInt32 nLockMode	) { return m_nVertexColorLockMode	= nLockMode;	}

	// �޽��� ��鷯.
	BOOL	OnAddModule	(						)	;
	BOOL	OnInit		(						)	;
	BOOL	OnDestroy	(						)	;
	BOOL	OnIdle		( UINT32 ulClockCount	)	;

	virtual	BOOL	OnLoadSector	( ApWorldSector * pSector );	// ���Ͱ� �ε��ű� ��!
	virtual	BOOL	OnDestorySector	( ApWorldSector * pSector );	// ���Ͱ� �������� ��!

	// �� Ŭ���̾�Ʈ ��� �̴�Ʈ..
	// �� �ȿ��� ���Ʈ�� �̴ϼȶ����� �ϹǷ� , ���� ��Ʈ ����� ȣ������ �ʵ��� �Ѵ�.
	// - ���ô� ���� init�� , ����( ������ ���������.. ) �� �ε��� ������ ȣ��ž����.
	// - ���� ���� ����Ÿ�� RpWorldSector * ����Ÿ�� �߰���.
	// - ���� ���带 �̴��ӷ������ؼ� , ������ ����.. <- ����� ������ , �Ƹ��� ���� ���� �����͸� ���� ������ �־���ҵ���.
	// ȣ��Ŵ½��� -> ���尡 ������ ����.

	virtual	BOOL	OnLoadRough		( ApWorldSector * pSector );	// ���� �ε��� ��û �ϱ���.
	virtual	BOOL	OnLoadDetail	( ApWorldSector * pSector );	// ������ �ε��� ��û�ϱ���. 
	virtual	BOOL	OnDestroyRough	( ApWorldSector * pSector );	// ���� �ε��� ��û �ϱ���.
	virtual	BOOL	OnDestroyDetail	( ApWorldSector * pSector );	// ������ �ε��� ��û�ϱ���. 

	BOOL	SetSectorIndexInsert( int indexType , ApWorldSector * pSector , INT32 nObjectIndex , RwSphere * pSphere );
	BOOL	SetSectorIndexInsertBBox( int indexType , ApWorldSector * pSector , INT32 nObjectIndex , RwBBox * pBBox );
	// �ֺ������� �ε����� ���ԵȰ�� , ���� ����

	VOID	SetTexturePath		( CHAR *szTexturePath				);

	// Texture Dictionary �̸� ����
	BOOL	SetTexDict			( char *pTexDictFileName			);
	BOOL	SetCamera			( RwCamera * pCamera				);
	BOOL	SetDataPath			( char * pInitToolImagePath			);
	BOOL	InitMapClientModule	( RpWorld * pWorld					);
	BOOL	InitMapToolData		( char *pTextureScriptFileName , char *pObjectScripFileName = NULL	);
	void	InitMapToolLoadRange( int x1 , int z1 , int x2 , int z2									);

	BOOL	SetAutoLoadRange		( INT32 nMapdata , INT32 nRough , INT32 nDetail );
	BOOL	__SetAutoLoadRange		( INT32 nMapdata , INT32 nRough , INT32 nDetail , INT32 nViewOption = -1 );
			// ������ �Ÿ��� �����ؼ� ������(���ʹ���..)�� �������ش�.
			// nViewOption�� �ɼ�â���� ������ �Ÿ� ����..

	// ��ü �ε�..

	// ������ (2004-04-30 ���� 11:45:56) : 
	// �Ʒ��Ͱ� ���� ���� �Լ��ε�..
	// �α��Ҷ� �̸� ������ �о�α� ���� �����.
	BOOL	LoadTargetPosition	( FLOAT fX , FLOAT fZ , ProgressCallback pfCallback = NULL , void * pData = NULL );

	// ������ (2004-04-29 ���� 5:23:41) : 
	// fX , fZ ���� �������� ���͸� ���� �� , �כ��Ϳ��� -nRange , + nRange ��ŭ �о� ���δ�.
	// �� , nRange �� 0 �̸� �ش� ���͸� �ε���.
	// �ε��� ������ ���� , �ʿ��Ұ�� Progress �ݺ��� ����ϸ� ���ٰ� ������.
	BOOL	LoadAll			( FLOAT	fX , FLOAT fZ , INT32 nRange , ProgressCallback pfCallback = NULL , void * pData = NULL );

	// ������ (2004-07-02 ���� 12:44:14) : bCreate�� �����Ұ��� �ε��Ұ��� üũ..
	BOOL	LoadAll			( BOOL bCreate , ProgressCallback pfCallback = NULL , void * pData = NULL );
	BOOL	SaveAll			( BOOL bGeometry = TRUE , BOOL bTile = TRUE , BOOL bCompact = TRUE	, ProgressCallback pfCallback = NULL , void * pData = NULL	, char * pBackupDirectory = NULL );
	BOOL	SaveAll			( char * pTarget , BOOL bGeometry = TRUE , BOOL bTile = TRUE , BOOL bCompact = TRUE	, ProgressCallback pfCallback = NULL , void * pData = NULL	);
							// pTarget �� "c:\\maptool\\subdata" �̷������� �ž����.

	BOOL	SaveGeometryBlocking( INT32 nDivisionIndex , char * pFileName );
	BOOL	LoadGeometryBlocking( INT32 nDivisionIndex , char * pFileName );
	BOOL	LoadSkyBlocking		( INT32 nDivisionIndex , char * pFileName );

	// ���� ���..
	BOOL	CreateAllCollisionAtomic	( ProgressCallback pfCallback = NULL , void * pData = NULL );
	BOOL	DestroyAllCollisionAtomic	( ProgressCallback pfCallback = NULL , void * pData = NULL );

	// �ܺο��� ���� �ڵ� �Լ���
	// �ڵ� �ε� �¿����� ó����.. ApWorldSector �� ������ ���̾���Ѵ�.
	//ApWorldSector *	LoadSector		( ApWorldSector * pSector											);
	ApWorldSector *	LoadSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);
	BOOL			ReleaseSector	( ApWorldSector * pSector											);
	BOOL			ReleaseSector	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);

	void			FlushLoadSector	( ApWorldSector * pNextSector );

	// �ͽ���Ʈ �Լ�

	BOOL			BspExport		( BOOL bDetail , BOOL bRough , BOOL bCompact , BOOL bServer ,
									char * pDestinationDirectory = NULL ,
									ProgressCallback pfCallback = NULL , void * pData = NULL );

	inline	BOOL	IsRangeApplied() { return m_bRangeApplied	;	}
	inline	INT32	GetLoadRangeX1() { return m_nLoadRangeX1	;	}
	inline	INT32	GetLoadRangeZ1() { return m_nLoadRangeZ1	;	}
	inline	INT32	GetLoadRangeX2() { return m_nLoadRangeX2	;	}
	inline	INT32	GetLoadRangeZ2() { return m_nLoadRangeZ2	;	}

	BOOL	IsInited			(){ return m_bInited; }

	void	MyCharacterPositionChange( AuPOS prevpos , AuPOS pos , BOOL bForceReload = FALSE );
		// ȣ����..
		// My Character �� �̵��Ҷ� �ɸ��� ��⿡�� �׻� ȣ�����־�� �Ѵ�.
		// �� �Լ� �ȿ��� ���� �ε��� ���� ��ƾ�� �����Ѵ�.

	static BOOL	IsInSectorRadius		( ApWorldSector * pSector , float x , float z , float radius );


	//////////////////////////////////////////////////////
	// ���� ������ ���� ���� ����..
	// ����� ����
	RpWorld	*		GetWorld		( void );

	FLOAT			GetHeight		( FLOAT x , FLOAT z , FLOAT y );//= SECTOR_MAX_HEIGHT );
	inline FLOAT	GetHeight		( AuPOS pos ) { return GetHeight( pos.x , pos.z , pos.y ); }
		// GetHeight�� , x,y���� �������� Ÿ�� ������ ������� ��ġ���� �����ش�.
		// ���� ��ü�� HP_GetHeight �� ���� , �� ����Ʈ�� ���⸦ ���Ͽ�
		// �̷����� ���̸� ���س���.
		// �Ϲ������� �� �Լ��� ���°��� ���� �Ǵ�.

	FLOAT			GetHeight_Lowlevel				( FLOAT x , FLOAT z , FLOAT y = SECTOR_MAX_HEIGHT  , BOOL * pValidCash = NULL );
	FLOAT			GetHeight_Lowlevel_HeightOnly	( FLOAT x , FLOAT z , BOOL * pValidCash = NULL );				// ���� ���̸��� ����.

	FLOAT			GetHeight_Lowlevel_Collision	( FLOAT x , FLOAT z , FLOAT y = SECTOR_MAX_HEIGHT  , BOOL * pbRiddable = NULL );

		// HeightPool�� ��ġ�� �ʰ� , ���� ���̰��� ���س���,
		// ���� �ݸ����� �̿�Ÿ� �ӵ��� �������̴� �������� ����.

	// Height Pool , �������� ����..
	FLOAT			HP_GetHeight	( ApWorldSector * pSector , int x , int z , FLOAT fY = SECTOR_MAX_HEIGHT );
	FLOAT			HP_GetHeight	( FLOAT fX , FLOAT fZ , FLOAT fY = SECTOR_MAX_HEIGHT );

	FLOAT			HP_GetHeightGeometryOnly	( ApWorldSector * pSector , int x , int z , FLOAT fY = SECTOR_MAX_HEIGHT);
	FLOAT			HP_GetHeightGeometryOnly	( FLOAT fX , FLOAT fZ , FLOAT fY = SECTOR_MAX_HEIGHT );

	void			RemoveHPInfo	();
		// HP_GetHeight �� , ���������� ĳ���ؼ� ó���ϴ� �Լ���.
		// �� , �ѹ��� �������� ���� ��ġ��� GetHeight_Lowlevel�� ȣ���ؼ� ������ �����
		// ������ �ε��� ���� ������ �� ���� �����Ѵ�.

	enum
	{
		PICKINGOBJECTCLUMP				,
		PICKINGOBJECTCOLLISIONATOMIC	,
		PICKINGCHARACTER				
	};
	RpClump *		GetCursorClump	( RwCamera * pCamera , RwV2d * pScreenPos , INT32 nOption = PICKINGOBJECTCOLLISIONATOMIC );

	BOOL	GetMapPositionFromMousePosition( INT32 x , INT32 y , FLOAT *px = NULL , FLOAT *py = NULL , FLOAT *pz = NULL , INT32 *pnPolygonIndex = NULL , FLOAT * pFoundDistancRatio = NULL , FLOAT * pFoundDistance = NULL );
		// pFoundDistancRatio = �ݸ��� �Ÿ��� ����
		// pFoundDistance = ���� ī�޶���� �Ÿ�.
	ApWorldSector *	GetMapPositionFromMousePosition_tool( INT32 x , INT32 y , FLOAT *px = NULL , FLOAT *py = NULL , FLOAT *pz = NULL , INT32 *pnPolygonIndex = NULL , FLOAT * pFoundDistance = NULL );
	
	// �Ⱦ��̴°� ���� �ּ�ó�� by gemani
	//BOOL			CheckCollision( RwV3d *pos1, RwV3d *pos2 );			//�� ���ͷ� �̷���� Line�� �������� Collision���θ� Return - AgcmCamera���� ���
		// ������ NULL �� ��� ����ó��.
		// �׿��� ���� ����. �׸��� �ش� ������ ������..�� ������.

	void				MakeDetailWorld			( ApWorldSector * pSector , int nDetail = SECTOR_HIGHDETAIL	);	// �ش缽�Ϳ� ������ �� ������ �����Ѵ�.
	RtWorldImport	*	__MakeDetailWorldImport	( ApWorldSector * pSector , int nDetail = SECTOR_HIGHDETAIL	);
	RpAtomic		*	MakeDetailWorldAtomic	( ApWorldSector * pSector , int nDetail = SECTOR_HIGHDETAIL	);
	// ���� ����Ÿ ����..
	void		Update				( ApWorldSector * pSector , int detail = -1					);

	void		RecalcNormal		( ApWorldSector * pSector , INT32 nDetail , BOOL bReculsive = TRUE );
	// ������ (2004-06-28 ���� 3:55:48) : ���ýú� �÷��� ����.. ���� ���͵� ���� ������Ʈ ��..
	// �����Ͽ����� �ǹ� ����..
	
	void		CopyDetailNormalToRoughNormal
									( ApWorldSector * pSector									);
	void		RecalcBoundingBox	( ApWorldSector * pSector									);
	// ������ ������ ��� �����ִ� �Լ�..

	//RpWorldSector	* GetWorldSector	( ApWorldSector * pSector );
	//RpWorldSector	* GetWorldSector	( INT32 arrayindexX	, INT32 arrayindexY	, INT32 arrayindexZ	);

	inline AcMaterialList	* GetMaterialList	( ApWorldSector * pSector )
	{
		AgcmMapSectorData	* pAgcmSectorData = GetAgcmAttachedData( pSector );
		if( pAgcmSectorData )	return pAgcmSectorData->m_MaterialList	;
		else					return NULL								;
	}

	//ApWorldSector *		GetSector		( RpWorldSector	* pWorldSector	);

	BOOL		GetWorldSectorInfo		( stFindMapInfo * pMapInfo , ApWorldSector * pSector , int x , int z , int detailedX = -1 , int detailedZ = -1 );
	BOOL		GetWorldSectorInfoOnly	( stFindMapInfo * pMapInfo , ApWorldSector * pSector , int x , int z , int detailedX = -1 , int detailedZ = -1 );
	void		SectorPreviwTextureSetup( stFindMapInfo * pMapInfo , int tileindex  );


	// ��ü ����..
	void		UpdateAll			(						);

	void		RecalcBoundingBox	(						);
	void		RecalcNormals		( BOOL bDetail = FALSE	, ProgressCallback pfCallback = NULL , void * pData = NULL );
	void		MakeDetailWorld		( ProgressCallback pfCallback = NULL , void * pData = NULL );	// �ش缽�Ϳ� ������ �� ������ �����Ѵ�.

	// ��Ÿ �Լ���..
	BOOL		GenerateRoughMap	( INT32 nTargetDetail , INT32 nSourceDetail , int alphatexture , FLOAT offset	,
									BOOL bHeight = TRUE , BOOL bTile = FALSE , BOOL bVertexColor = FALSE , BOOL bApplyAlpha = FALSE );

	void		UpdateNormal2		( ApWorldSector * pSector , float fx , float fz );
	
	BOOL		ApplyAlpha			( stFindMapInfo *pMapInfo, int basetile , int maskedindex , int alphaindex , BOOL bReculsive = FALSE , BOOL bFirst = TRUE);
	BOOL		ApplyAlphaToSector	( ApWorldSector * pSector , int alphatexture , int nDetail = -1 );

	// ���� ������ �Լ���..
	// ������ ������ ���� ������..

	BOOL		D_SetHeight		( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , FLOAT height , BOOL bReculsive = TRUE , BOOL bUpdateNormal = TRUE	);
	BOOL		D_SetValue		( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , ApRGBA value	);
	BOOL		D_SetTile		( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z ,
									UINT32 firsttexture ,
									UINT32 secondtexture	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 thirdtexture		= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 fourthtexture	= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 fifthtexture		= ALEF_TEXTURE_NO_TEXTURE	,
									UINT32 sixthtexture		= ALEF_TEXTURE_NO_TEXTURE	);
	BOOL		D_SetEdgeTurn	( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z , BOOL bEdge	);
	BOOL		D_GetEdgeTurn	( ApWorldSector * pSector , INT32 nTargetDetail , INT32 x , INT32 z );
	BOOL		D_GetEdgeTurn	( FLOAT fX , FLOAT fZ );

	// ������ (2005-04-25 ���� 5:39:44) : 
	// Ŭ���̾�Ʈ�� �� Ÿ�� �о..
	UINT8		GetType			( FLOAT fX , FLOAT fY , FLOAT fZ );
	BOOL		GetOnObject		( FLOAT fX , FLOAT fY , FLOAT fZ );

	// DWSector ����...
	BOOL					IsPolygonDataLoaded	( ApWorldSector * pSector );
	inline DWSectorData	*	_GetDWSectorData	( ApWorldSector * pSector )
	{
		AgcmMapSectorData	* pAgcmSectorData = GetAgcmAttachedData( pSector );
		if( pAgcmSectorData )	return &pAgcmSectorData->m_DWSector	;
		else					return NULL							;
	}

		// ����..
		RpDWSector *	CreateDWSector		( ApWorldSector * pSector ,	int detail , int nov , int not , int tex = 4 );
		void			DestroyDWSector		( ApWorldSector * pSector ,	int detail );

		// Ŭ���̾�Ʈ..
		BOOL			LoadDWSector		( ApWorldSector * pSector				);
		BOOL			LoadDWSector		( ApWorldSector * pSector , int nDetail	);
		BOOL			ReleaseDWSector		( ApWorldSector * pSector				);

		RwTexture *		LoadRoughTexture	( RwStream *stream						);

		RpAtomic *		LoadRoughAtomic		( INT32 wx , INT32 wz , FLOAT fSrcX , FLOAT fSrcZ , FLOAT fHeightOffset , FLOAT fDstX , FLOAT fDstZ );


	rsDWSectorInfo *	GetDWSector			( ApWorldSector * pSector ,	int detail ,  rsDWSectorInfo * pDWSectorInfo );
	rsDWSectorInfo *	GetDWSector			( ApWorldSector * pSector ,	rsDWSectorInfo * pDWSectorInfo ) { return GetDWSector( pSector , GetCurrentDetail( pSector ) , pDWSectorInfo ); }

	// ������ ����..
	void				SetCurrentDetail	( int detail , BOOL bForce = FALSE	);
	void				SetCurrentDetail	( ApWorldSector * pSector , int detail , BOOL bForce = FALSE );
	inline void			SetCurrentDetail	( INT32 arrayindexX	, INT32 arrayindexY , INT32 arrayindexZ , int detail	){ SetCurrentDetail( m_pcsApmMap->GetSectorByArray( arrayindexX , arrayindexY , arrayindexZ ) , detail ); }
	int					GetCurrentDetail	( ApWorldSector * pSector	);
	inline int			GetCurrentDetail	( INT32 arrayindexX	, INT32 arrayindexY , INT32 arrayindexZ ){ return GetCurrentDetail( m_pcsApmMap->GetSectorByArray( arrayindexX , arrayindexY , arrayindexZ ) ); }

	ApWorldSector *		GetSector			( RpAtomic		* pAtomic		);	// ��������� ����.. ���� ���� ����.
	BOOL				IsGeometry			( RpAtomic		* pAtomic	, INT32 * pIndex = NULL	);

	// �ε� �ݹ�..
	static	void		CallBackMapDataLoad		( ApWorldSector * pSector );
	static	void		CallBackMapDataDestroy	( ApWorldSector * pSector );

	// Texture Dictionary ���� �Լ���
	BOOL					DumpTexDict();

	INT32				GetSectorsToLoad	()				{ return m_lSectorsToLoad	;	}
	VOID				SetSectorsToLoad	(INT32 lCount)	{ m_lSectorsToLoad = lCount	;	}

	void	RenderSectorGrid	( ApWorldSector * pSector );
	BOOL	AllocSectorGridInfo	( ApWorldSector * pSector );

	virtual ~AgcmMap();

public:
	// DWSector ���� ��ǵ�..
	
	/*
	 * Initialize DWSector Object
	 * dwSector : DWSector must be fetched by RpDWSectorGetDetail()
	 * numVertices : Number of vertices
	 * numPolygons : Number of Polygons
	 * numTexCoords : Number of Texture Coordinates
	 */
	RpDWSector *		RpDWSectorInit				( RpDWSector *dwSector, RwUInt32 numVertices, RwUInt32 numPolygons, RwUInt8 numTexCoords);
	/* Get detail object (0 base)*/
	RpDWSector *		RpDWSectorGetDetail			( ApWorldSector * pWorldSector ,  RwUInt8 numDetail );
	INT32				RpDWSectorGetCurrentDetail	( ApWorldSector * pWorldSector );
	/* Set current detail index (0 base, one sector) */
	RpAtomic *			RpDWSectorSetCurrentDetail	( ApWorldSector * pWorldSector ,  RwUInt8 numDetail , BOOL bForce = FALSE );
	/* Build world sector collision data */
	ApWorldSector *		RpDWSectorBuildAtomicCollision( ApWorldSector *pWorldSector );
	ApWorldSector *		RpDWSectorDestroyDetail		( ApWorldSector * pWorldSector , RwUInt8 numDetail	);
	ApWorldSector *		RpDWSectorDestroyDetail		( ApWorldSector * pWorldSector);

	BOOL				RpDWSectorDestroy			( RpDWSector *dwSector	);

	BOOL				IsLoadedDetail				( ApWorldSector * pWorldSector , INT32 nDetail );

	BOOL				CreateImAtomic				( MagImAtomic * pAtomic , ApWorldSector * pWorldSector );
	RpAtomic *			CreateCollisionAtomic		( ApWorldSector * pWorldSector , INT32 nDetail = SECTOR_HIGHDETAIL );

	BOOL				SetPreLightForAllGeometry	( FLOAT fValue , ProgressCallback pfCallback = NULL , void * pData = NULL );
	// 0.0f ~ 2.0f , 1.0f�� ���� ��Ȳ ����..
	// �ʵ���Ÿ�� ���������� ��ä�� �ٲ۴�.

	//�ݡݡݡݡݡݡݡݡ� Intersection �� �ݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡݡ� add by gemani(RpWorld �Ⱦ���)
	ApWorldSector*		m_pSelfCharacterSector;			// ���ΰ��� ���ִ� ����
	bool				m_bUseCullMode;
	UINT32				m_ulCurTick;

	void				SectorForAllAtomicsIntersection( ApWorldSector * pWorldSector ,   RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data );

	IntersectionSectors*		LineGetIntersectionSectors(RwLine*	line); 

	// ���� �ε��� ��� sector�� üũ�Ѵ�
	// ID�� �ߺ� Intersectionüũ�� �����ϱ� ���� �ο��� ��ȣ
	// AgcmTargeting - 0 , AgcmLensFlare - 1, AgcmCamera - 2,
	// AgcmMap::GetMapPositionFromMousePosition - 3,
	// AgcmMap::GetCursorClump - 4

	// ���� ,object,character �� üũ�Ѵ�!!
	void				LoadingSectorsForAllAtomicsIntersection( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// ������ �����ϰ� world�� object üũ�Ѵ�!!
	void				LoadingSectorsForAllAtomicsIntersection2( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// object�� üũ�Ѵ�!!(ī�޶󿡼� ����!)
	void				LoadingSectorsForAllAtomicsIntersection3( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// character�� üũ�Ѵ�!!(debug��)
	void				LoadingSectorsForAllAtomicsIntersection4( INT32	ID, RpIntersection *    intersection,  
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// �����ϰ� üũ�Ѵ�!!(ī�޶󿡼� ����!)
	BOOL				LoadingSectorsForAllAtomicsIntersection5(RpIntersection *    intersection, 
														RpIntersectionCallBackAtomic    callBack,    void *    data,	BOOL bLock = TRUE );

	// ������ (2003-11-20 ���� 3:17:52) : Ŭ���� üũ�Լ� �߰�.
	void				LoadingSectorsForAllClumps(		INT32				ID			,	// ���� ��� ���ϳ���.. ���� 0 �ְ� ������..
												 		RpClumpCallBack		pCallBack	,
														void *				pData		,
														BOOL				bLock = TRUE );
	SectorRenderList *	GetSectorData(ApWorldSector *pSector)	
			{ return (SectorRenderList*) m_pcsApmMap->GetAttachedModuleData(m_pcsAgcmRender->m_iSectorDataIndex,(PVOID)pSector);}

	// gemani(2004-4-7) : Ŭ���� üũ�Լ�(��Ʈ����) �߰�.
	void				LoadingSectorsForAllClumps_OCTREE(INT32				ID			,	// ���� ��� ���ϳ���.. ���� 0 �ְ� ������..
												 		RpClumpCallBack		pCallBack	,
														void *				pData		,
														BOOL				bLock = TRUE );

protected:

	RwV3d	* m_pVertexList		;
	RwRGBA	* m_pPreLightList	;

	struct	_MakeDetailDataSet
	{
		ApWorldSector *	pSector			;

		INT32			nWidth			;
		INT32			nHeight			;
		RwV3d			*pVertexList	;
		RwRGBA			*pPreLightList	;


		INT32			nVertexIndex	;

		RpMorphTarget	*pMorphTarget	;
		RpTriangle		*pPolygons		;
		RwV3d			*pVertices		;
		RwV3d			*pNormals		;
		RwTexCoords		*pTexCoords		[rwMAXTEXTURECOORDS];
		RwRGBA			*pPreLitLum		;

		RwV3d	*		Map		( int x , int y ) { return &pVertexList	[ ( y ) * ( nWidth + 1 ) + ( x ) ]; }
		RwRGBA	*		Prelight( int x , int y ) { return &pPreLightList[ ( y ) * ( nWidth + 1 ) + ( x ) ]; }

		BOOL			bValidNormal	;
		BOOL			bValidTexCoords	;

		_MakeDetailDataSet() :
			bValidNormal	( FALSE ),
			bValidTexCoords	( FALSE )
		{
		}
	};

	BOOL	MakeRoughPolygon		( INT32 nDetail , INT32 nType , INT32 x , INT32 z , FLOAT fStepSize , _MakeDetailDataSet * pDataSet , ApDetailSegment * pSegment , INT32 nMatIndex );
public:
		// Lock Master
	struct stLockSectorInfo
	{
		ApWorldSector *	pSector		;
		INT32			nDetail		;
		RwInt32			nLockMode	;
	};
	
	AuList< stLockSectorInfo >		m_listLockedSectorList;
	BOOL IsLockedSector			( ApWorldSector * pSector	, RwInt32 nLockMode	, INT32 nDetail = -1 );
	BOOL LockSector				( ApWorldSector * pSector	, RwInt32 nLockMode	, INT32 nDetail = -1 );	// ���� ���͸� ����.
	BOOL UnlockSectors			( BOOL bUpdateCollision = FALSE , BOOL bUpdateRough = FALSE );	// ���� ��� ���͸� �����

public:
	static RpCollisionTriangle *		CollisionAtomicSectorCallback(
																		RpIntersection		*	pstIntersection	,
																		RpCollisionTriangle	*	pstCollTriangle	,
																		RwReal					fDistance		, 
																		void				*	pvData			);

	static RpAtomic *	_GetClumpCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpAtomic *	_GetClumpPickingOnlyCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpAtomic *	_GetHeightCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpAtomic *	_GetHeightClientCallback(
								 RpIntersection *	intersection	,
								 RpWorldSector *	sector			,
								 RpAtomic *			atomic			,
								 RwReal				distance		,
								 void *				data			);
	static RpCollisionTriangle * _IntersectionCallBackFindDistance
												(RpIntersection *intersection, RpCollisionTriangle *collTriangle,
												RwReal distance, void *data);

	static	BOOL	_AgcmMapSectorDataConstructor	(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	_AgcmMapSectorDataDestructor	(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetCallbackLoadMap			(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_ONLOAD_MAP			, pfnCallback, pvClass); }
	BOOL	SetCallbackUnLoadMap		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_ONUNLOAD_MAP		, pfnCallback, pvClass); }
	BOOL	SetCallbackSectorChangePre	(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_MAP_SECTOR_CHANGE_PRE	, pfnCallback, pvClass); }
	BOOL	SetCallbackSectorChange		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_MAP_SECTOR_CHANGE	, pfnCallback, pvClass); }
	BOOL	SetCallbackGetHeight		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_ONGETHEIGHT		, pfnCallback, pvClass); }
	BOOL	SetCallbackSetAutoLoadChange(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(AGCM_CB_ID_SETAUTOLOADCHANGE		, pfnCallback, pvClass); }
	
	static BOOL	CBMyCharacterPositionChange(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBMyCharacterPositionChange2(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBDestroySector_Start	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBDestroySector_End		(PVOID pData, PVOID pClass, PVOID pCustData);

	RtWorldImport *	__CreateWorldCollisionImport		( ApWorldSector * pWorldSector , INT32 nDetail );
	
	BOOL	CreateCompactDataFromDetailInfo	( ApWorldSector * pSector );	//	������ ����Ÿ���� ����Ʈ ����Ÿ�� ������.

	static BOOL	CBLoadGeometry	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSaveGeometry	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBLoadMoonee	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSaveMoonee	(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBMTextureRender(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	LoadZoneRoughBuffer( FLOAT fSrcX , FLOAT fSrcZ , FLOAT fHeight , FLOAT fDstX , FLOAT fDstZ , INT32 nRadius = 6 );
	void	FlushZoneRoughBuffer();
	vector<RpAtomic *>	m_vectorZoneRough;	// ���ε� ������ξ� ����� ����

	// ������ (2005-08-24 ���� 6:02:21) : ���ε� ó��..
	INT32	m_nZoneLoadDivision;
	BOOL	IsZoneLoading()
	{
		if( 0 == m_nZoneLoadDivision )	return FALSE;
		else							return TRUE	;
	}
	BOOL	CheckSelfCharacterRegionChange( AuPOS *pPrev , AuPOS *pNext );
	BOOL	ZoneLoadingDivision( INT32 nDivision , INT16 nRegion );
	void	ClearAllSectors();

	// ������ (2005-08-29 ���� 12:08:42) : 
	// ����׿� ���
	void	ShowAll();
	void	ToggleZoneRough();
	BOOL	m_bShowZoneRough;

	INT32	m_nLoadLockCount;

	// ���������� ���� ���� �����°� �ӽ÷� ���� ���� ��..
	INT32	LoadLock	() { return ++m_nLoadLockCount; }
	INT32	ReleaseLock	() { return --m_nLoadLockCount; }
	INT32	IsLocked	() { return m_nLoadLockCount; }

	#ifdef USE_MFC
	void	SetChangeTile( UINT32 uTiled , UINT32 uReplace )
	{
		m_uTiled	= uTiled	;
		m_uReplace	= uReplace	;
	}

	// Ÿ�� �ٲ�ġ��� ����Ÿ.
	// m_uTiled == 0 �̸� Disable
	UINT32	m_uTiled	;
	UINT32	m_uReplace	;
	#endif // USE_MFC


	// ���� ���� ��� ����..
	static RpAtomic * GetBBoxCallback(RpAtomic * pAtomic, void *data);
	INT32	GetSectorList		( RwBBox *pbbox , vector< ApWorldSector * > * pVector );
	inline	INT32 GetSectorList		( RpAtomic * pAtomic , vector< ApWorldSector * > * pVector )
	{
		RwBBox bbox;
		AgcmMap::GetBBoxCallback( pAtomic , ( void * ) &bbox );
		return GetSectorList( &bbox , pVector );
	}

	// ������ (2005-11-23 ���� 10:39:54) : 
	// ���� ���..

	struct stLoadedDivisionCheck
	{
		INT32	nDivision		;
		BOOL	bInLoadedRange	;

		stLoadedDivisionCheck():bInLoadedRange(FALSE){}
	};

	static	BOOL	CBLoadedDivisionCheck( DivisionInfo * pDivisionInfo , PVOID pData );

	BOOL	IsLoadedDivision( AuPOS * pPos );
	BOOL	IsLoadedDivision( INT32	nDivision );
};

// �ؽ��� Ÿ�� ����..
enum TEXTURETYPE
{
	TT_FLOORTILE	,	// �ٴ� Ÿ��.. �ؽ��� 4���� �ϳ��� �ž� ����Ÿ�Ϸ� ���۵ž� �ɼ����� ����.	2x2
	TT_UPPERTILE	,	// �ٴ� Ÿ�� ���� �ö󰡴� ���� ���̸� ���ϸ� , ���� ���ķ� ���۵��� �ʰ� �ɼ����� �з��´�.	4x4
	TT_ONE			,	// ��ä�� �ϳ��� �ؽ��ĸ� ����. ���� ������� �ʴ´�.
	TT_ALPHATILE	,	// �ٴ�Ÿ���� ���� Ÿ��.. 4x4
	TT_MAX
};

#define	TT_FLOOR_DEPTH	2
#define	TT_UPPER_DEPTH	4
#define TT_ONE_DEPTH	1
#define	TT_ALPHA_DEPTH	4

// �̰� Ŭ���̾�Ʈ ��� ����� ���پ�..
inline int GET_TEXTURE_DIMENSION( int index  )
{
	int dim = ( ( index & 0x0f000000 ) >> 24	);

	if( dim == TEXTURE_DIMENTION_DEFAULT )
	{
		switch( GET_TEXTURE_TYPE( index ) )
		{
		case	TT_FLOORTILE	:	return	TT_FLOOR_DEPTH	;
		case	TT_UPPERTILE	:	return	TT_UPPER_DEPTH	;
		case	TT_ONE			:	return	TT_ONE_DEPTH	;
		case	TT_ALPHATILE	:	return	TT_ALPHA_DEPTH	;
		default:
			ASSERT( !"-__-!" );
			return TEXTURE_DIMENTION_DEFAULT;
		}
	}
	else	return dim;
}

inline UINT32 SET_TEXTURE_DIMENSION( UINT32 nIndex , INT32 nDim  )
{
	// ������ (2004-05-28 ���� 12:28:42) : ����� ����.
	//int nPrevDim = ( ( nIndex & 0x0f000000 ) >> 24	);

	return ( nIndex & 0xf0ffffff ) | ( nDim << 24 );
}

inline float GET_TEXTURE_U_START	( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 0.0f;
	}
}
inline float GET_TEXTURE_V_START	( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 0.0f;
	}
}
inline float GET_TEXTURE_U_END		( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension + 1 ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension + 1 ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) % dimension + 1 ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 1.0f;
	}
}
inline float GET_TEXTURE_V_END		( int index  )
{
	int	dimension = GET_TEXTURE_DIMENSION( index );
	switch( GET_TEXTURE_TYPE( index ) )
	{
	case TT_FLOORTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension + 1 ) / ( float ) dimension;
	case TT_UPPERTILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension + 1 ) / ( float ) dimension;
	case TT_ALPHATILE	:	return ( float ) ( GET_TEXTURE_OFFSET( index ) / dimension + 1 ) / ( float ) dimension;
	case TT_ONE			:
	default:
		return 1.0f;
	}
}

inline void	_RemoveFullPath( char * fullpath , char * filename)
{
	char			drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath	( fullpath	, drive, dir, fname, ext );

	wsprintf( filename , "%s%s" , fname , ext );
}

void _PreviewTextureSetup( ApWorldSector * pSector , int indexX , int indexY , int tileindex , int depth , RwTexCoords * pCoord );
inline void _PreviewTextureSetup( ApWorldSector * pSector , int indexX , int indexY , int tileindex , RwTexCoords * pCoord )
{
	_PreviewTextureSetup( pSector , indexX , indexY , tileindex , pSector->GetDetailSectorInfo()->m_nDepth[ pSector->GetCurrentDetail() ] , pCoord );
}

inline	RwRGBA ApRGBAToRwRGBA( ApRGBA rgb )
{
	RwRGBA	rwrgba;
	rwrgba.alpha	= rgb.alpha	;
	rwrgba.red		= rgb.red	;
	rwrgba.green	= rgb.green	;
	rwrgba.blue		= rgb.blue	;
	return rwrgba;
}

inline	ApRGBA RwRGBAToApRGBA( RwRGBA rgb )
{
	ApRGBA	aprgba;
	aprgba.alpha	= rgb.alpha	;
	aprgba.red		= rgb.red	;
	aprgba.green	= rgb.green	;
	aprgba.blue		= rgb.blue	;
	return aprgba;
}

inline	INT32	GetSectorDistance( ApWorldSector * pSector1 , ApWorldSector * pSector2 )
{
	INT32 nDistanceX	= ( pSector1->GetIndexX() - pSector2->GetIndexX() );
	INT32 nDistanceZ	= ( pSector1->GetIndexZ() - pSector2->GetIndexZ() );

	nDistanceX	= ( nDistanceX < 0 ) ? -nDistanceX : nDistanceX;
	nDistanceZ	= ( nDistanceZ < 0 ) ? -nDistanceZ : nDistanceZ;
	// ������ (2004-03-24 ���� 11:55:01) : �Ÿ�üũ�� 

	return	nDistanceX < nDistanceZ ? nDistanceZ : nDistanceX;
}

// ���� ���� ������ �̾Ƴ��� ��ƿ��Ƽ �Լ�..
ApWorldSector * AcuGetWorldSectorFromAtomicPointer( RpAtomic * pAtomic );
BOOL RpDWSectorDestroy(RpDWSector *dwSector);

#define	ALEF_WORLD_DETAIL_SECTOR_FILE_NAME_FORMAT	"b%06dx.ma2"
#define	ALEF_WORLD_ROUGH_SECTOR_FILE_NAME_FORMAT	"a%06dx.ma2"

#define	ALEF_WORLD_OBJECT_FILE_NAME_FORMAT			"W%06d.off"

struct	_FindWorldSectorGeometryStruct
{
	_FindWorldSectorGeometryStruct()													{	}
	_FindWorldSectorGeometryStruct( float f, RpAtomic* p ) : distance(f), pAtomic(p)	{	}

	float		distance;
	RpAtomic*	pAtomic;
};

// ������ (2004-05-28 ���� 2:33:11) : ���� Ÿ������ .. ����� �˻�..
inline bool IsSameTile( int nIndex1 , int nIndex2 )
{
	int	category , index , dim;
	category	= GET_TEXTURE_CATEGORY	( nIndex1 );
	index		= GET_TEXTURE_INDEX		( nIndex1 );
	dim			= GET_TEXTURE_DIMENSION	( nIndex1 );
	

	if(	category	==	GET_TEXTURE_CATEGORY	( nIndex2 )	&&	
		index		==	GET_TEXTURE_INDEX		( nIndex2 )	&&
		dim			==	GET_TEXTURE_DIMENSION	( nIndex2 ) )
		return true;
	else
		return false;
}

// ������ (2004-05-28 ���� 2:33:19) : ���� �ؽ��ĸ� ����ϴ� �� �̶��..
inline bool IsSameTexture( int nIndex1 , int nIndex2 )
{
	int	category , index;
	category	= GET_TEXTURE_CATEGORY	( nIndex1 );
	index		= GET_TEXTURE_INDEX		( nIndex1 );

	if(	category	==	GET_TEXTURE_CATEGORY	( nIndex2 )	&&	
		index		==	GET_TEXTURE_INDEX		( nIndex2 )	)
		return true;
	else
		return false;
}

inline bool IsUsedThisTile( ApDetailSegment * pSegment , int tileindex )
{
	if( IsSameTile( tileindex , pSegment->pIndex[ TD_FIRST	] )	||
		IsSameTile( tileindex , pSegment->pIndex[ TD_THIRD	] )	)
		return true;
	else
		return false;
}

inline bool IsSingleBackTile( ApDetailSegment * pSegment )
{
	if( pSegment->pIndex[ TD_FIRST	] != ALEF_TEXTURE_NO_TEXTURE &&
		pSegment->pIndex[ TD_SECOND	] == ALEF_TEXTURE_NO_TEXTURE &&
		pSegment->pIndex[ TD_THIRD	] == ALEF_TEXTURE_NO_TEXTURE )
		// 4th �ؽ��Ĵ� �˻����� �ʴ´�.
	{
		return true;
	}
	else
		return false;
}

inline	int	_GetRandTileOffset( int tileindex )
{
	switch( GET_TEXTURE_TYPE( tileindex ) )
	{
	case TT_FLOORTILE	:
		{
			// �����ϰ� �����ؼ� ���.

			int	dimension	= GET_TEXTURE_DIMENSION( tileindex );
			int index;
			index = MAKE_TEXTURE_INDEX(
				GET_TEXTURE_DIMENSION	( tileindex ) ,
				GET_TEXTURE_CATEGORY	( tileindex ) ,
				GET_TEXTURE_INDEX		( tileindex ) ,
				GET_TEXTURE_TYPE		( tileindex ) ,
				rand() % (dimension * dimension )	);
			return index;
		}
		break;
	case TT_UPPERTILE	:
	case TT_ONE			:
	case TT_ALPHATILE	:
	default:
		return tileindex;
	}
}

extern INT32	g_nRidableCount		;
extern BOOL		g_bRidableChecked	;

#define AGCMMAP_THIS	( &AgcmMap::GetInstance() )

#endif // !defined(AFX_AGCMMAP_H__91149629_2429_47C8_9C28_5ECF4DCC6645__INCLUDED_)
