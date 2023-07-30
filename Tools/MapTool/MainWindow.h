#pragma once

#include "rtimport.h"
#include "ApmMap.h"
#include "AgcmMap.h"
#include "AcuObject.h"
#include "UndoManager.h"
#include "rpspline.h"

#include "AuProfileManager.h"

#define	GDEPTH			50
#define	GSECTORWIDTH	5.f
#define	HEIGHTDEFAULT	5.f

#define	WM_WORKINGSECTORCHANGE	( WM_USER + 1009 )

enum DIRECTION
{
	LEFTTOP,
	TOP,
	RIGHTTOP,
	LEFT,
	CENTER,
	RIGHT,
	LEFTBOTTOM,
	BOTTOM,
	RIGHTBOTTOM
};

struct	_TileSelectStruct
{
	int				x;
	int				z;
	ApWorldSector*	pSector;
};

class MainWindow : public AgcWindow  
{
public:
	struct	ObjectSelectStruct
	{
		RpClump	*		pClump	;
		ObjectUndoInfo	undoInfo;
	};

	RwV3d				m_PixelRayEnd	;

	// Map ����Ÿ
	CUndoManager		m_UndoManager	;

	ApWorldSector *		m_pCurrentGeometry		;
	INT32				m_nCurrentTileIndexX	;
	INT32				m_nCurrentTileIndexZ	;
	
	// Camera Walking ����
	BOOL		m_bForwardKeyDown	;
	BOOL		m_bBackwardKeyDown	;
	BOOL		m_bStepLeftKeyDown	;
	BOOL		m_bStepRightKeyDown	;
	BOOL		m_bLiftUpKeyDown	;
	BOOL		m_bLiftDownKeyDown	;
	BOOL		m_bRotateCCWDown	;
	BOOL		m_bRotateCWDown		;
	BOOL		m_bUp				;
	BOOL		m_bDown				;

	UINT		m_uLastKeyPressTime	;
	
	// Subdivision ���� ���� â..
	BOOL				m_bGeometrySelection	;
	int					m_SelectedPosX1			;
	int					m_SelectedPosY1			;
	ApWorldSector *		m_pSelectedGeometry1	;
	
	int					m_SelectedPosX2			;
	int					m_SelectedPosY2			;
	ApWorldSector *		m_pSelectedGeometry2	;

	RwV3d		m_Position				;

	BOOL			m_bLButtonDown		;
	BOOL			m_bRButtonDown		;
	RsMouseStatus	m_PrevMouseStatus	;
	RsMouseStatus	m_PrevRButtonPos	;
	RwBBox			PickBox				;

	UINT32			m_ulPrevTime		;

	FLOAT		m_fCameraPan			;
	FLOAT		m_fCameraPanInitial		;
	FLOAT		m_fCameraPanUnit		;

	BOOL		m_bLCtrl;
	BOOL		m_bRCtrl;

	INT32			m_nFoundPolygonIndex;
	float			m_fApplyheight		;
	ApWorldSector *	m_pCameraSector		;

	BOOL		IsLoaded()	{ return m_bLoaded; }
	BOOL		m_bLoaded	;	//�ε�ž������� ���� üũ.

	// Operations
	void		LoadPolygonData				();	// ������ ���� ������.
	void		CreateSector				( BOOL bDetail = FALSE );

	void		ApplyAlphaOnTheSelection	();
	BOOL		ChangeSegmentMaterial		(stFindMapInfo *pMapInfo, int posx, int posy, int maskedindex , int alphaindex , int back = 0 );
	BOOL		ChangeSegmentMaterial2		(stFindMapInfo *pMapInfo, int posx, int posy, int maskedindex , int back = 0 );

	BOOL		ApplyAlpha					(stFindMapInfo *pMapInfo, int basetile , int maskedindex , int alphaindex , BOOL bReculsive = FALSE , BOOL bFirst = TRUE);
	BOOL		ApplyFill					(stFindMapInfo *pMapInfo, int basetile , int replaceindex , BOOL bReculsive = FALSE , BOOL bFirst = TRUE);
	
	BOOL		OnApplyFill					();
	BOOL		OnApplyTongMap				();

	BOOL		OnAppyWater					();

	// Camera Walking
	void		MoveToSectorCenter			( ApWorldSector * pSector	);
	void		MoveTo						( FLOAT fX , FLOAT fZ , FLOAT fDY = 7000.0f	);
	void		CameraIdleMovement			();
	void		SectorCollisionCheck		( RsMouseStatus * pPos		);

	// Get Functions...
	int			GetCurrentMode				();	// ���� �۾����� ��带 ����.
	int			GetCurrentLayer				();	// ���� ���̾� ��.
	ApDetailSegment * GetCurrentPolygonsTileIndex( ApWorldSector *pWorldSector , int polygonindex ,
											  int mousex , int mousey ,	 // ���콺 ��ǥ.
											  int * pPosX , int *pPosY , // ��ǥ..
											  int *pPosDetailedX = NULL , int *pPosDetailedY = NULL );// ������ǥ , ���߿� �����.
	BOOL		GetWorldSectorInfo			( stFindMapInfo * pMapInfo , ApWorldSector * pSector , int x , int z , int detailedX = -1 , int detailedZ = -1 );
	
	int			GetTileIndexOnMouseCursor	( int type ); // 0 back 1 upper 2 all
	BOOL		GetVertexColorOnMouseCursor	( RwRGBA * pRGB );

	float		SetSectorHeight				( stFindMapInfo * pFindMapInfo , float height ); // 9���� �迭�� �Է�����..

	// ���̾� ������ �׷���
	RwIm3DVertex	*	m_pVertexBuffer		;
	RwIm3DVertex	*	m_pVertexBuffer2	;	// Edge Turn ���̾� ǥ�ǿ�..
	RwImVertexIndex	*	m_pVertexIndex		;
	RwImVertexIndex	*	m_pVertexIndex2		;	// Edge Turn ���̾� ǥ�ǿ�..

	void		DrawWireFrame				( INT32 nType );
	void		DrawGeometryBlocking		();
	// ������ �׸�.
	void		DrawGeometrySelection		();

	// �׸��� ǥ��.
	void		DrawHeightGrid				();
	void		DrawSectorGrid				();

	// �귯�� �̹���.
	void		OnBrushUpdate				();

	// ������Ʈ�� ó��..
	UINT		m_uLastGeometryPressTime	;
	BOOL		m_bGeometryAdjust			;
	void		GeometryIdleAdjustment()	;

	// Ÿ�� ó��..
	BOOL		m_bTileAdjust				;
	BOOL		m_bTileSelecting			;
	void		TileIdleAdjustment()		;
	
	void		Reset();
	void		RenderStateClear();				// Ǯ �׸��� �� renderstate �ʱ�ȭcallback

	enum
	{
		OBJECT_NONE,
		OBJECT_BRUSHING,
		OBJECT_SELECT,
		OBJECT_SELECTADD,
		OBJECT_SELECTREMOVE,
		OBJECT_MODE
	};

	UINT		m_bObjectBrushing			;
	void		ObjectIdleBrushing()		;
	void		SelectedObjectTurning( FLOAT	fDegree );
	POINT		m_pointObjectSelectFirst	;
	BOOL		m_bCopyObject				;

	void		SetObjectSelection( POINT p1, POINT p2 , BOOL bGeometryCheck = TRUE );
	void		SetObjectSelection( RpClump* pClump );
	void		AddObjectSelection( RpClump* pClump, BOOL bSelecting = FALSE );
	void		RemoveObjectSelection( RpClump * pClump );
	BOOL		IsObjectSelected()			{ return m_listObjectSelected.GetCount() > 0 ? TRUE : FALSE;	}
	RpClump *	GetSelectedObject()			{ return m_listObjectSelected.GetCount() > 0 ? m_listObjectSelected.GetHead().pClump : NULL; }
	void		GetObjectCenter( RwV3d *v );
	void		UpdateObjectUndoInfo();

	void		DrawGridInfo				();
	void		DrawObjectSelectionWire		();

	AuList< ObjectSelectStruct >	m_listObjectSelected	;	// ���É� ������Ʈ ����Ʈ..
	AuList< ObjectSelectStruct >	m_listObjectSelecting	;	// ���É� ������Ʈ ����Ʈ..
	
	AuList<_TileSelectStruct>		m_listTileSelect	;


	// ������Ʈ ���� ������ ǥ�ÿ� ��
	RpAtomic *	m_pAreaSphere				;
	void		ChangeSphereAlpha	( INT32 nAlpha );
private:
	void		AddTileSelect		( ApWorldSector * pSector , int x , int z );
	void		CancelTileSelect	(				);

public:
	void		PopupMenuSelect		( int index		);
	void		PopupMenuObject		( int index		);
	void		DetailChange		( int detail	);
	RpWorld*	LoadWorld			(				);

	MainWindow();
	virtual ~MainWindow();

	void OnEditModeChange	(	int changedmode		, int prevmode	);
	void OnTileLayerChange	(	int changedlayer	, int prevlayer	);

	BOOL OnInit();
	BOOL OnMouseMove( RsMouseStatus *ms	);
	BOOL OnLButtonDown( RsMouseStatus *ms	);
	BOOL OnLButtonDblClk( RsMouseStatus *ms	);
	BOOL OnLButtonUp( RsMouseStatus *ms	);
	BOOL OnRButtonDown( RsMouseStatus *ms	);
	BOOL OnRButtonUp( RsMouseStatus *ms	);
	BOOL OnRButtonDblClk( RsMouseStatus *ms	);
	
	void OnPreRender( RwRaster *raster	)	{		}
	void OnRender( RwRaster *raster	)		{		}
	void OnPostRender( RwRaster *raster	)	{		}
	void OnWindowRender();	 // Im2D�� Render�ϱ�
	void OnClose();	// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.
	BOOL OnKeyDown( RsKeyStatus* ks );
	BOOL OnKeyUp( RsKeyStatus* ks );
	BOOL OnIdle( UINT32 ulClockCount );
	BOOL OnMouseWheel( BOOL bForward );

	void HighlightRender();
	void DrawAreaSphere( FLOAT x, FLOAT z, FLOAT fScale	);
	void DrawAreaSphere( FLOAT x, FLOAT y, FLOAT z, FLOAT fScale );
	void DrawPillar( FLOAT x, FLOAT z	);
	
	void OnChangeCurrentSector( ApWorldSector * pCurrent , ApWorldSector * pFuture );

	// ������� ����.
	RtWorldImport*	CreateWorldImport	( ApWorldSector *pGeo	);
	RpWorld*		CreateWorld			( void				);


	// �� �����ֱ�..
	RwIm3DVertex	m_pLineList [ 6 ];
	void AxisCreate();
	void AxisRender();

	// Lock Master
	BOOL	LockSector( ApWorldSector * pSector	, RwInt32 nLockMode	);	// ���� ���͸� ����.
	BOOL	UnlockSectors( BOOL bUpdateCollision = FALSE  , BOOL bUpdateRough = FALSE );	// ���� ��� ���͸� �����.
	void	DetailIdleCheck	();
	
	void	UpdateCollision	( ApWorldSector * pSector	);

	BOOL	LoadLocalMapFile();

#ifdef	_PROFILE_
	// Profile ��
	BOOL						m_bShowProfile		;
	int							m_iProfileMode		;				// 0 - 1�ʸ��� reset, 1 - reset ���� �ʰ� ����
	char						profile_str[10][200];
	CProfileNode*				m_pCurNode			;
	int							m_iCommand			;// 0 - 9 : Down to Num Child, 98 - up to parent,99 - reset
	int							m_iMyCurPage		;// 0- 0~9, 1- 10~19
	int							m_iTotalItem		;

	char						profile_check_str	[10][200];
	char						profile_max_str		[200];
	CProfileNode*				m_pCheckedNode		[10];						// �ֽ��ϱ� ���� �����ϴ� ���
	int							m_iCheckIndex		;

	void						FindMaxItem			(CProfileNode* node,CProfileNode** setnode);
	void						SetProfileCommand	( int nCommand ) { m_iCommand = nCommand; }
	BOOL						IsProfile			() { return m_bShowProfile; }
	void						SetProfile			( BOOL bSet ) { m_bShowProfile = bSet; }
#endif
};

BOOL	GetWorldPosToScreenPos( RwV3d * pWorldPos, POINT * pPoint );
BOOL	IsInLoadedRange( FLOAT x , FLOAT z );
BOOL	ClipLoadedRange( FLOAT* pfX , FLOAT* pfZ );
BOOL	CBPreRenderAtomicCallback( RpAtomic * , void* pData );
BOOL	__AgcmRender_Maptool_PreRenderCallback( PVOID pData, PVOID pClass, PVOID pCustData );	// SetCallbackPostRender

#ifdef	_PROFILE_
enum PROFILING_COMMAND
{
	PFC_NOCOMMAND	= -1	,
	PFC_STEPINTO0	= 0		,	// into 
	PFC_REGISTER0	= 50	,	// add

	PFC_PAGEUP		= 90	,
	PFC_PAGEDN		= 91	,

	PFC_RESET		= 97	,	// reset
	PFC_TOROOT		= 98	,	// root
	PFC_TOLEAF		= 99		// leaf
};
#endif
