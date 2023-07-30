// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MapTool.h"
#include "resource.h"
#include <afxadv.h>

#include "rwcore.h"
#include "rpworld.h"
#include <AgcEngine.h>
#include "MainWindow.h"
#include "MainFrm.h"

#include "MapSelectDlg.h"
#include "RgbSelectDlg.h"
#include "AdjustHeightDlg.h"
#include "ProgressDlg.h"
#include "KeyReferenceDlg.h"
#include "RoughMapGenerateDlg.h"
#include "SplashDlg.h"
#include "SubDivisionDlg.h"
#include "SectorCreateDlg.h"
#include "OptionDialog.h"
#include "PrelightAdjustDlg.h"

#include "MyEngine.h"

#include "DWSStream.h"

#include "ExportDlg.h"
#include "SaveDlg.h"
#include "ClipPlaneSetDlg.h"
//#include "SkySettingDlg.h"
#include "LoginDlg.h"
#include "SkyTemplatePreviewDlg.h"
#include "SkyValueChangeDlg.h"
#include "EditConsoleDlg.h"
#include "AtomicFilterDlg.h"
#include "EventNatureChangeFilterDlg.h"

#include "GetArg.h"
#include "AcuTexture.h"

#include "DDSConvertDlg.h"
#include "Acurpmtexture.h"
#include "FindObjectSettingDlg.h"
#include "RoughDisplayDlg.h"


#include <winbase.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	MINIMAP_DETAIL	512
#define	ROUGHMAP_DETAIL	128

#define MAINFRM_DEBUG_TIMER_ID	45123
#define MAINFRM_EXPORT_TIMER_ID	45124
#define OBJECT_GROUP_FILE_VERSION	( 0x0101 )

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

BOOL	_CreateDirectory( char *pFormat , ... )
{
	char	strtmp[8192];
	va_list vParams;

	va_start	( vParams,		pFormat				);
	vsprintf	( strtmp	,	pFormat,	vParams	);
	va_end		( vParams							);

	return CreateDirectory( strtmp , NULL );
}

void	GetSubDataDirectory		( char * pStr )
{
	CTime	time = CTime::GetCurrentTime();
	wsprintf( pStr , "%s\\Map\\SubData\\%02d%02d" , ALEF_CURRENT_DIRECTORY , 
		time.GetMonth() , time.GetDay() );
}

struct DivisionMinimapExportSetting
{
	FLOAT	fFogDistanceBackup , fFogFarClipBackup ;
	BOOL	bUseFogBackup;
	RwRaster *		pRaster	;
	RwRaster *		pZRaster;
	RwRect			rectMap	;
	UINT32	uAtomicFilterSave;

	bool	result_;

	DivisionMinimapExportSetting() : result_(true)
	{
		// ���� ������ ����..
		fFogFarClipBackup	= g_pcsAgcmEventNature->GetFogFarClip	()	;
		fFogDistanceBackup	= g_pcsAgcmEventNature->GetFogDistance	()	;
		bUseFogBackup		= g_pcsAgcmEventNature->IsFogOn			()	;

		g_pcsAgcmEventNature->SetFog(
			FALSE			,
			100				,
			204800			,
			g_pcsAgcmEventNature->GetFogColor		()	);


		// ������ (2005-03-28 ���� 4:22:18) : ���͸�� ����..
		// �Ѳ��� ����.
		uAtomicFilterSave = g_pMainFrame->m_Document.m_uAtomicFilter;
		g_pMainFrame->m_Document.m_uAtomicFilter = ~( CAlefMapDocument::AF_OBJECT_DOME | CAlefMapDocument::AF_SYSTEMOBJECT );

		rectMap.x	= 0;
		rectMap.y	= 0;
		rectMap.w	= MINIMAP_DETAIL;
		rectMap.h	= MINIMAP_DETAIL;

		pRaster		= RwRasterCreate(rectMap.w , rectMap.h , 0 , rwRASTERTYPECAMERA		);
		if( NULL == pRaster )
		{
			g_pMainFrame->MessageBox( "�̴ϸ� ī�޶� �޸� ���� ��������" );
			result_ = FALSE;
			return;
		}
		pZRaster	= RwRasterCreate(rectMap.w , rectMap.h , 0 , rwRASTERTYPEZBUFFER	);
		if( NULL == pZRaster )
		{
			RwRasterDestroy	( pRaster	);
			g_pMainFrame->MessageBox( "�̴ϸ� �޸� ���� ��������" );
			result_ = FALSE;
			return;
		}

		RwCameraSetRaster	( g_MyEngine.GetParallelCamera() , pRaster	);
		RwCameraSetZRaster	( g_MyEngine.GetParallelCamera() , pZRaster	);
	}

	~DivisionMinimapExportSetting()
	{
		g_pcsAgcmEventNature->SetFog(
			bUseFogBackup			,
			fFogDistanceBackup		,
			fFogFarClipBackup		,
			g_pcsAgcmEventNature->GetFogColor		()	);

		// ������ (2005-03-28 ���� 4:22:42) : �Ѳ� ����ġ.
		g_pMainFrame->m_Document.m_uAtomicFilter = uAtomicFilterSave;


		RwCameraSetRaster	( g_MyEngine.GetParallelCamera() , g_MyEngine.m_pSubCameraMainCameraSubRaster	);
		RwCameraSetZRaster	( g_MyEngine.GetParallelCamera() , g_MyEngine.m_pSubCameraMainCameraSubZRaster	);

		if( pRaster )
			RwRasterDestroy	( pRaster	);

		if( pZRaster )
			RwRasterDestroy	( pZRaster	);
	}
};

BOOL	__DivisionMinimapExportCallback ( DivisionInfo * pDivisionInfo , PVOID pData );

// ������ (2004-05-11 ���� 4:37:40) : 
// Ŀ�ǵ� ����Ʈ.
enum
{
	_NOCOMMAND			,
	_TOGGLETIMER		,
	_SHOWMINIMAP		,
	_SHOWROUGHMAP		,
	_MINIMAPOFF			,
	_SHOWMINIMAPOBJECT	,
	_SHOWCOLLISIONOBJECT,
	_SHOWRANGE			,
	_SPHEREALPHA		,
	_HELPCOMMAND		,
	_BLENDMODE			,
	_MTEXTUREPASS		,
	_RUNNINGSPEED		,
	_VERTEXSHADER		,
	_DLIGHTADJUST		,
	_ALIGHTADJUST		,
	_FARCLIPPLANE		,
	_SHOWBLOCKING_GEOMETRY	,
	_SHOWBLOCKING_OBJECT	,
	_CREATESTAR			,
	_SHOWSTARLEVEL		,
	_TESTCODE			,
	_TESTLIGHTMAP		,
	_CHANGESKYGAP		,
	_FADEOUTFACTOR		,
	_CREATE_COMPACTDATA	,
	//@{ Jaewon 20041229
	// to generate ambient occlusion maps
	_AMBOCCLMAP			,
	//@} Jaewon
	_CAMERAMOVINGSPEED	,
	//@{ Jaewon 20050531
	// ;)
	_TOGGLEGLOSSMAP		,
	//@} Jaewon
	//@{ Jaewon 20050713
	// ;)
	_TOGGLEMATD3DFX		,
	//@} Jaewon

	#ifdef	_PROFILE_
	_PROFILE_TOGGLE		,
	_PROFILE_STEPINTO	,
	_PROFILE_REGISTER	,
	_PROFILE_RESET		,
	_PROFILE_TOROOT		,
	_PROFILE_TOLEAF		,
	#endif

	_SAVE_OCTREE		,
	_DEBUG_EXPORT		,
	_GRASS_CHECK		,

	_PERSFECTIVE		,
	_WIND				,
	_SUNANGLE			,
	_SUNSIZE			,

	_MAX_COMMAND
};

// RenderWare�ʿ��� ���� �ڷ� ó���� ����.. �۷ι��� ������.
// MainWindow���� �����.
CMainFrame *		g_pMainFrame = NULL	;
extern	MainWindow	g_MainWindow		;

BOOL _ProgressCallback( char * strMessage , int nCurrent , int nTarget , void *data )
{
	if( NULL == data ) return FALSE;

	CProgressDlg	* pProgressDlg = ( CProgressDlg * ) data;

	pProgressDlg->SetMessage	( strMessage	);
	pProgressDlg->SetTarget		( nTarget		);
	pProgressDlg->SetProgress	( nCurrent		);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_NEWWORLD, OnNewWorld)
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_COMMAND(IDM_SAVE, OnSave)
	ON_COMMAND(IDM_REVERT, OnRevert)
	ON_COMMAND(IDM_UNDO, OnUndo)
	ON_COMMAND(IDM_REDO, OnRedo)
	ON_COMMAND(IDM_REVERT_CURRENT, OnRevertCurrent)
	ON_COMMAND(IDM_UPDATE_NORMAL, OnUpdateNormal)
	ON_WM_CLOSE()
	ON_COMMAND(IDM_ADJUST_GEOMETRY_HEIGHT, OnAdjustGeometryHeight)
	ON_COMMAND(IDM_MODECHANGE_GEOMETRY, OnModechangeGeometry)
	ON_COMMAND(IDM_MODECHANGE_OBJECT, OnModechangeObject)
	ON_COMMAND(IDM_MODECHANGE_SUBDIVISION, OnModechangeSubdivision)
	ON_COMMAND(IDM_MODECHANGE_TILE, OnModechangeTile)
	ON_COMMAND(IDM_GRID_TOGGLE , OnGridToggle)
	ON_COMMAND(IDM_CAMERA_FIRST_PERSON, OnCameraFirstPerson)
	ON_COMMAND(IDM_CAMERA_RESET, OnCameraReset)
	ON_COMMAND(IDM_TILE_SPOID_BACK, OnTileSpoidBack)
	ON_COMMAND(IDM_TILE_SPOID_ALPHA, OnTileSpoidAlpha)
	ON_COMMAND(IDM_TILE_SPOID_ALL, OnTileSpoidAll)
	ON_COMMAND(IDM_BSP_EXPORT, OnBspExport)
	ON_COMMAND(IDM_TONG_MAP_HAP_CHI_GI, OnTongMapHapChiGi)
	ON_COMMAND(IDM_HELP_KEYREFERENCE, OnHelpKeyreference)
	ON_COMMAND(IDM_GENERATE_ROUGHMAP, OnGenerateRoughmap)
	ON_COMMAND(IDM_OPTIMIZE_ADJUST_EDGE, OnOptimizeAdjustEdge)
	ON_COMMAND(IDM_CREATESECTOR_DETAIL, OnCreatesectorDetail)
	ON_COMMAND(IDM_CREATESECTOR, OnCreatesector)
	ON_COMMAND(IDM_TOGGLE_CAMERA_PROJECTIONTYPE, OnToggleCameraProjectiontype)
	ON_COMMAND(IDM_UPDATE_ALL, OnUpdateAll)
	ON_COMMAND(IDM_EDIT_CREATE_ALPHA, OnEditCreateAlpha)
	ON_COMMAND(IDM_EXPORT_TILE, OnExportTile)
	ON_COMMAND(IDM_SMOOTH_GEOMETRY, OnSmoothGeometry)
	ON_COMMAND(IDM_EDIT_CREATE_ALL_SECTOR, OnEditCreateAllSector)
	ON_COMMAND(IDM_BRUSH_SIZE_DOWN, OnBrushSizeDown)
	ON_COMMAND(IDM_BRUSH_SIZE_UP, OnBrushSizeUp)
	ON_COMMAND(IDM_FILL_TILE, OnFillTile)
	ON_COMMAND(IDM_OPEN_OPTION_DIALOG, OnOpenOptionDialog)
	ON_COMMAND(IDM_TOGGLE_FP_VIEW, OnToggleFpView)
	ON_COMMAND(IDM_REFREAH_OBJECT_BLOCKING, OnRefreahObjectBlocking)
	ON_COMMAND(IDM_VIEW_TOGGLE_OBJECT, OnViewToggleObject)
	ON_COMMAND(IDM_VIEW_CLIPPLANE, OnViewClipplane)
	ON_COMMAND(IDM_SKY_SETTING, OnSkySetting)
	ON_COMMAND(IDM_CHANGE_FPS_EDIT_MODE, OnChangeFpsEditMode)
	ON_COMMAND(IDM_OPEN_SHORTCUT2, OnOpenShortCut)
	ON_COMMAND(IDM_MENU_TOGGLE_TIMER, OnToggleTimer)
	ON_COMMAND(IDM_TONG_MAP_APPLY, OnTongMapApply)
	ON_COMMAND(IDM_OBJECT_REFRESH, OnObjectRefresh)
	ON_COMMAND(IDM_ADD_WATER, OnAddWater)
	ON_WM_TIMER()
	ON_COMMAND(IDM_FULL_SCREEN_MODE, OnFullScreenMode)
	ON_COMMAND(IDM_MODECHANGE_GEOMETRY_REVERSE, OnModechangeGeometryReverse)
	ON_COMMAND(IDM_MODECHANGE_TILE_REVERSE, OnModechangeTileReverse)
	ON_COMMAND(IDM_VIEW_SKY_TEMPLATE, OnViewSkyTemplate)
	ON_COMMAND(IDM_VIEW_WATERDLG, OnViewWaterdlg)
	ON_COMMAND(IDM_TIME_FASTER, OnTimeFaster)
	ON_COMMAND(IDM_TIME_SLOWER, OnTimeSlower)
	ON_COMMAND(IDM_PRELIGHT_ADJUST, OnPrelightAdjust)
	ON_COMMAND(IDM_EDIT_CREATE_COLLISION, OnEditCreateCollision)
	ON_COMMAND(IDM_CLIENT_FPS_VIEW, OnClientFpsView)
	ON_COMMAND(IDM_SKY_VALUE_CHANGE, OnSkyValueChange)
	ON_COMMAND(IDM_TEST_ADD, OnTestAdd)
	ON_COMMAND(IDM_TEST_SUB, OnTestSub)
	ON_COMMAND(IDM_VIEW_PROJECTION, OnViewProjection)
	ON_COMMAND(IDM_FILE_MINIMAPEXPORT, OnFileMinimapexport)
	ON_COMMAND(IDM_FILE_CLOSETEMPFILE, OnFileClosetempfile)
	ON_COMMAND(IDM_FILE_OBJECT_EXPORT_FOR_SERVER, OnFileObjectExportForServer)
	ON_COMMAND(IDM_FILE_DDS_CONVERT, OnFileDdsConvert)
	ON_COMMAND(IDM_CLEANUP_UNUSED_TEXTURE, OnCleanupUnusedTexture)
	ON_COMMAND(IDM_VIEW_ATOMICFILTER, OnViewAtomicfilter)
	ON_COMMAND(IDM_EVENTNATURE_FILTER, OnEventnatureFilter)
	ON_COMMAND(IDM_EDIT_UPDATECOLLISION, OnEditUpdatecollision)
	ON_COMMAND(IDM_MODE_DUNGEON_TOGGLE, OnModeDungeonToggle)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(IDM_OPEN_SHORTCUT, OnOpenShortCut)
	ON_COMMAND(IDM_ANOTHER_GRID_TOGGLE , OnGridToggle)
	ON_WM_SETCURSOR()
	ON_COMMAND_RANGE( IDM_VIEW_BLOCKING_GEOMETRY, IDM_VIEW_BLOCKING_OBJECT , OnShowBlocking )
	ON_COMMAND_RANGE( IDM_DWSECTOR_ALL_ROUGH , IDM_DWSECTOR_ACCORDINGTOCURSOR , OnDWSectorControl )	
	ON_COMMAND(IDM_CAMERA_SAVE, OnCameraPositionSave)
	ON_COMMAND(IDM_CAMERA_LOAD, OnCameraPositionLoad)
	ON_COMMAND(IDM_FINDOBJECT, OnFindobject)
	ON_COMMAND(IDM_FINDOBJECTNEXT, OnFindobjectnext)
	ON_COMMAND(IDM_SELECTALL, OnSelectAll)	
	ON_COMMAND(ID_VIEW_ROUGHDISPLAY, OnViewRoughdisplay)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pMiniMap		= NULL;
	m_pOutputWnd	= NULL;
	m_pTileList		= NULL;	
	m_pToolBar		= NULL;
	m_pHtmlWnd		= NULL;
	m_pDungeonWnd	= NULL;
	m_pDungeonToolbar= NULL;

	// ������ �۷ι��� ������.
	g_pMainFrame	= this;
	m_bFullScreen	= MT_VM_NORMAL;

	AddConsoleCommand( _HELPCOMMAND			, "?"		, "help"			, "h" , 1 , "���� ���" );
	AddConsoleCommand( _TOGGLETIMER			, "timer"	, ""				, "" , 0 , "�ð��� �帣�� ���߰� �Ѵ�." );
	AddConsoleCommand( _SHOWMINIMAP			, "mm"		, "minimap"			, "" , 0 , "�̴ϸ��� �Ѱ� ���ϴ�." );
	AddConsoleCommand( _SHOWROUGHMAP		, "rm"		, "roughmap"		, "" , 0 , "�������� �Ѱ� ���ϴ�." );
	AddConsoleCommand( _MINIMAPOFF			, "cm"		, "closemap"		, "" , 0 , "�̴ϸ�/�������� �ݽ��ϴ�" );
	AddConsoleCommand( _MINIMAPOFF			, "cm"		, "closemap"		, "" , 0 , "�̴ϸ�/�������� �ݽ��ϴ�" );
	AddConsoleCommand( _SHOWMINIMAPOBJECT	, "smmo"	, ""				, "" , 0 , "�̴ϸ�/�����ʿ��� ������Ʈ��ǥ���մϴ�." );
	AddConsoleCommand( _SHOWCOLLISIONOBJECT	, "sc"		, "showcoll"		, "" , 0 , "�ݸ��� ������Ʈ�� ǥ���մϴ�." );
	AddConsoleCommand( _SHOWRANGE			, "sr"		, "showrange"		, "" , 0 , "���� ǥ���� ������ �����մϴ�( ������/���� )" );
	AddConsoleCommand( _SPHEREALPHA			, "sa"		, "spherealpha"		, "" , 1 , "���� ǥ�� ���Ǿ ���İ��� �����մϴ�." );
	AddConsoleCommand( _BLENDMODE			, "bm"		, "blendmode"		, "" , 1 , "�ؽ��� ���� ��带 �����մϴ�." );
	AddConsoleCommand( _MTEXTUREPASS		, "mt"		, "multi"			, "" , 1 , "��Ƽ�ؽ��� ������ ���մϴ�." );
	AddConsoleCommand( _RUNNINGSPEED		, "rs"		, "runningspeed"	, "" , 1 , "�޸��� �ӵ� ������ �մϴ�." );
	AddConsoleCommand( _VERTEXSHADER		, "vs"		, "vertexshader"	, "" , 1 , "���ؽ� ���̴� ��뿩�θ� �����մϴ�." );
	AddConsoleCommand( _DLIGHTADJUST		, "da"		, "dlightadjust"	, "" , 1 , "������ �𷺼ų� ����Ʈ�� ���� ���� �����մϴ�." );
	AddConsoleCommand( _ALIGHTADJUST		, "aa"		, "alightadjust"	, "" , 1 , "������ �Ϻ��Ʈ ����Ʈ�� ���� ���� �����մϴ�." );
	AddConsoleCommand( _FARCLIPPLANE		, "fc"		, "farclipplane"	, "" , 1 , "��Ŭ�� �÷����� �����մϴ�." );
	AddConsoleCommand( _SHOWBLOCKING_GEOMETRY, "sg"		, ""				, "" , 1 , "������ ��ŷ ������Ʈ�� ����մϴ�.." );
	AddConsoleCommand( _SHOWBLOCKING_OBJECT	, "so"		, ""				, "" , 1 , "������Ʈ�� ��ŷ ������Ʈ�� ����մϴ�..." );
	//AddConsoleCommand( _SHOWSYSTEMOBJECT	, "sso"		, ""				, "" , 1 , "�ý��� ������Ʈ�� �Ѱų� ����." );

	AddConsoleCommand( _CREATESTAR			, "cs"		, ""				, "" , 1 , "�� �����(������->ū�� 4��)" );
	AddConsoleCommand( _SHOWSTARLEVEL		, "sl"		, ""				, "" , 1 , "�� ���̱� ���� ����.( 0 �� �ٺ��̱� 4�� �Ⱥ��̱� )" );
	AddConsoleCommand( _TESTCODE			, "t"		, ""				, "" , 1 , "�����ڵ��" );

	AddConsoleCommand( _TESTLIGHTMAP		, "tl"		, ""				, "" , 1 , "����Ʈ�� �׽�Ʈ �ڵ�" );
	AddConsoleCommand( _CHANGESKYGAP		, "csg"		, ""				, "" , 1 , "��ī�� ü���� �� ����" );
	AddConsoleCommand( _FADEOUTFACTOR		, "fof"		, ""				, "" , 1 , "���̵�ƿ� ����" );

	AddConsoleCommand( _CREATE_COMPACTDATA	, "cc"		, ""				, "" , 1 , "����Ʈ ����Ÿ ����" );
	
	//@{ Jaewon 20041229
	// to generate ambient occlusion maps
	AddConsoleCommand( _AMBOCCLMAP			, "aom"		, "ambocclmap"		, "" , 1 , "Ambient Occlusion �� ����" ); 
	//@} Jaewon

	AddConsoleCommand( _CAMERAMOVINGSPEED	, "cms"		, ""				, "" , 1 , "ī�޶� �̵� �ӵ� ����" ); 
	

	//@{ Jaewon 20050531
	// ;)
	AddConsoleCommand( _TOGGLEGLOSSMAP		, "gm"		, "glossmap"		, "" , 1 , "���� �۷ν��� ���" );
	//@} Jaewon

	//@{ Jaewon 20050719
	// ;)
	AddConsoleCommand( _TOGGLEMATD3DFX		, "fx"		, "matd3dfx"		, "" , 0 , "��� �ؽ�ó ȿ�� ���");
	//@} Jaewon

	#ifdef	_PROFILE_
	AddConsoleCommand( _PROFILE_TOGGLE		, "profile"		, ""				, "" , 0 , "�������ϸ� �ڵ�" , TRUE );
	AddConsoleCommand( _PROFILE_STEPINTO	, "in"			, ""				, "" , 1 , "�������ϸ� �ڵ�" , TRUE );
	AddConsoleCommand( _PROFILE_REGISTER	, "add"			, ""				, "" , 1 , "�������ϸ� �ڵ�" , TRUE );
	AddConsoleCommand( _PROFILE_RESET		, "reset"		, ""				, "" , 0 , "�������ϸ� �ڵ�" , TRUE );
	AddConsoleCommand( _PROFILE_TOROOT		, "root"		, ""				, "" , 0 , "�������ϸ� �ڵ�" , TRUE );
	AddConsoleCommand( _PROFILE_TOLEAF		, "leaf"		, ""				, "" , 0 , "�������ϸ� �ڵ�" , TRUE );
	#endif

	AddConsoleCommand( _SAVE_OCTREE			, "os"			,"octreesave"	 	, "" , 0 , "��Ʈ�� ����" , TRUE );
	AddConsoleCommand( _DEBUG_EXPORT		, "de"			,"debugexport"	 	, "" , 0 , "����׿� �ͽ���Ʈ" , TRUE );
	AddConsoleCommand( _GRASS_CHECK			, "cg"			,"checkgrass"	 	, "" , 0 , "ǮȮ��" , TRUE );
	AddConsoleCommand( _PERSFECTIVE			, "ps"			,"perspective"		, "" , 0 , "ȭ�� ����" , TRUE );
	AddConsoleCommand( _WIND				, "wd"			,"wind"				, "" , 0 , "�ٶ�����" , TRUE );
	AddConsoleCommand( _SUNANGLE			, "suna"		,"sunangle"			, "" , 0 , "�¾缳��" , TRUE );
	AddConsoleCommand( _SUNSIZE				, "suns"		,"sunsize"			, "" , 0 , "�¾�ũ�⼳��" , TRUE );
	
	m_nFindObjectEventIndex = APDEVENT_FUNCTION_QUEST;
	m_nLastSearchObject		= 0;
}

//---------------------------------------------------------------------------

CMainFrame::~CMainFrame()
{

}

//---------------------------------------------------------------------------

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

//--------------------------------------------------------------------------

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hIcon = theApp.LoadIcon( IDR_MAINFRAME );
	SetIcon( hIcon , FALSE );
	SetIcon( hIcon , TRUE );

	
	// �� ������ ����.
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	// ���� �� ������ ����.

	m_pMiniMap			= new CUIMiniMapWnd		;
	m_pOutputWnd		= new CUIOutputWnd		;
	m_pTileList			= new CUITileListWnd	;
	m_pToolBar			= new CAlefToolBar		;
	m_pDungeonWnd		= new CDungeonWnd		;
	m_pDungeonToolbar	= new CDungeonToolBar	;

	if (!m_pMiniMap->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create �̴ϸ� window\n");
		return -1;
	}

	if (!m_pOutputWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create �ƿ�ǲ window\n");
		return -1;
	}

	if (!m_pTileList->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create Ÿ�ϸ���Ʈ window\n");
		return -1;
	}

	if (!m_pToolBar->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create ���� window\n");
		return -1;
	}

	if (!m_pDungeonWnd->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create ���������� window\n");
		return -1;
	}

	if (!m_pDungeonToolbar->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create �������� window\n");
		return -1;
	}

#ifdef _USE_HTML_WINDOW
	m_pHtmlWnd		= new CHtmlWnd		;
	try
	{

		if (!m_pHtmlWnd->Create( NULL , NULL , AFX_WS_DEFAULT_VIEW , CRect(0, 0, 0, 0) , this , 1004 ))
		{
			TRACE0("Failed to create Html window\n");
			return -1;
		}
		else
		{
			CString	address;

			address.Format( "http://www.magoja.com/bbs/zboard.php?id=maptool" );
			
			m_pHtmlWnd->Navigate2( address );
		}
	}
	catch( ... )
	{
		// do no op..
		TRACE( "htmlWindow ��������!..");
		//delete m_pHtmlWnd;
		m_pHtmlWnd = NULL;
	}
#endif // _USE_HTML_WINDOW

//	//Create the main status
//	if (!m_wndStatusBar.Create(this) ||
//		!m_wndStatusBar.SetIndicators(indicators,
//		  sizeof(indicators)/sizeof(UINT)))
//	{
//		TRACE0("Failed to create status bar\n");
//		return -1;      // fail to create
//	}

	// Board Update..
	SetTimer( MAINFRM_DEBUG_TIMER_ID , 1000	, NULL );

	// �ε�ó��
	Load();

	if( ALEF_LOADING_MODE != LOAD_NORMAL )
		SetTimer( MAINFRM_EXPORT_TIMER_ID , 1000	, NULL );

//	OnFullScreenMode();
//	OnFullScreenMode();
	
	MD_SetMainWindowHandle( this->GetSafeHwnd() );
	return TRUE;
}

//--------------------------------------------------------------------------
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	if( m_wndView.m_hWnd ) m_wndView.SetFocus();
}

//--------------------------------------------------------------------------
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//--------------------------------------------------------------------------
//Repsond to requesting a new world
void CMainFrame::OnNewWorld() 
{
}


//--------------------------------------------------------------------------
//Check/uncheck passed menu item
bool CMainFrame::CheckMenuItem(CMenu* pMenu, UINT uiMenuID)
{
   //Initialise the menu item info structure and retrieve the state
   MENUITEMINFO miInfo;
   miInfo.cbSize = sizeof (MENUITEMINFO);
   miInfo.fMask = MIIM_STATE;           
   VERIFY( pMenu->GetMenuItemInfo(uiMenuID, &miInfo) );

   //If the state is currently checked...
   if (miInfo.fState & MF_CHECKED)
   {
      //Uncheck the itme and return true
      pMenu->CheckMenuItem(uiMenuID, MF_UNCHECKED | MF_BYCOMMAND);
      return true;
   }
   else 
   {
      //Otherwise check the item and return false
      pMenu->CheckMenuItem(uiMenuID, MF_CHECKED | MF_BYCOMMAND);
      return false;
   }
}


//--------------------------------------------------------------------------

void CMainFrame::OnKillFocus(CWnd* pNewWnd) 
{
	CFrameWnd::OnKillFocus(pNewWnd);
}

//--------------------------------------------------------------------------

BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
	return CFrameWnd::OnMouseWheel(nFlags, zDelta , pt);
}

//--------------------------------------------------------------------------

CChildView* CMainFrame::GetChildView()
{
   return &m_wndView;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

	SetWindowSize( cx , cy );
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// ������ ����.

	if( m_pMiniMap )
	{
		m_pMiniMap->DestroyWindow();
		delete m_pMiniMap;
	}
	if( m_pOutputWnd )
	{
		m_pOutputWnd->DestroyWindow();
		delete m_pOutputWnd;
	}
	if( m_pTileList )
	{
		m_pTileList->DestroyWindow();
		delete m_pTileList;
	}
	if( m_pToolBar )
	{
		m_pToolBar->DestroyWindow();
		delete m_pToolBar;
	}
	if( m_pHtmlWnd )
	{
		m_pHtmlWnd->DestroyWindow();
		delete m_pHtmlWnd;
	}
	if( m_pDungeonWnd )
	{
		m_pDungeonWnd->DestroyWindow();
		delete m_pDungeonWnd;
	}
	if( m_pDungeonToolbar )
	{
		m_pDungeonToolbar->DestroyWindow();
		delete m_pDungeonToolbar;
	}

	g_Const.StoreRegistry();
	
	m_wndView.DestroyWindow();
}

void CMainFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );

	//dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );
	dc.FillRect( rect , &ALEF_BRUSH_BACKGROUND );

//	static bool bFirst = true;
//	if( bFirst )
//	{
//		bFirst = false;
//		CSplashDlg pSplashDlg;
//		pSplashDlg.DoModal();
//	}
	
	// Do not call CFrameWnd::OnPaint() for painting messages
}

void	DisplayMessage( COLORREF color , char *pFormat , ...	)
{
	CMainFrame * pFrame = ( CMainFrame * ) theApp.m_pMainWnd;

	char	strDisplay[8192];
	va_list vParams;

	va_start	( vParams,		pFormat				);
	vsprintf	( strDisplay,	pFormat,	vParams	);
	va_end		( vParams							);

	if( pFrame->m_pOutputWnd )
		pFrame->m_pOutputWnd->AddMessage( strDisplay , color );

	printf( strDisplay );
}

void	DisplayMessage( char *pFormat , ... 					)
{
	CMainFrame * pFrame = ( CMainFrame * ) theApp.m_pMainWnd;
	char	strDisplay[8192];

	va_list vParams;

	va_start	( vParams,		pFormat				);
	vsprintf	( strDisplay,	pFormat,	vParams	);
	va_end		( vParams							);

	if( pFrame->m_pOutputWnd )
		pFrame->m_pOutputWnd->AddMessage( strDisplay , AEM_NORMAL );

	printf( strDisplay );
}

void	DisplayMessage( CString str								)
{
	DisplayMessage( (LPSTR) (LPCTSTR) str );
}

void	DisplayMessage( COLORREF color , CString str			)
{
	DisplayMessage( color , (LPSTR) (LPCTSTR) str );
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_TOOLBAR_SELECT:
		{
			int nSelected = ( int ) lParam;
			OnToolBarMenuSelected( nSelected );
		}
		break;
	case WM_TOOLBAR_DETAILCHANGE:
		{
			int	nDetail	= lParam;

			switch( wParam )
			{
			case	DETAIL_NOTIFY	:	// �˸�..
				g_MainWindow.DetailChange( nDetail );
				break;
			case	DETAIL_SET		:	// Ÿ ������� �ѱ�.
				m_pToolBar->SetDetail( nDetail );
				break;
			}
		}
		break;
	case WM_MAPBRUSHCHANGE:
		{
			g_MainWindow.OnBrushUpdate();
		}
		break;

	case WM_WORKINGSECTORCHANGE:
		{
			m_pMiniMap->Invalidate( FALSE );
		}
		break;
	}
	
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnToolBarMenuSelected( int index )
{
	// ���� �޴� �޽��� ó����.
	switch( index )
	{
	case	TOOLBAR_GOTO_SELECT_GEOMETRY:
		{
			g_pcsApmMap->FlushUnpackManagerDivision();

			switch( Save() )
			{
			case IDCANCEL:
				return;
			default:
				break;
			}
			
			CMapSelectDlg	msdlg;
		
			msdlg.m_nX_array	= ALEF_LOAD_RANGE_X1 / MAP_DEFAULT_DEPTH;
			msdlg.m_nY_array	= ALEF_LOAD_RANGE_Y1 / MAP_DEFAULT_DEPTH;
			msdlg.m_nX2_array	= ALEF_LOAD_RANGE_X2 / MAP_DEFAULT_DEPTH;
			msdlg.m_nY2_array	= ALEF_LOAD_RANGE_Y2 / MAP_DEFAULT_DEPTH;

			if( msdlg.DoModal( NORMAL_LOADING_SELECT ) == IDOK )
			{
				//char	filename[ 1024 ];
				CleanUp();
				
				ALEF_LOAD_RANGE_X1	= msdlg.m_nX_array		* MAP_DEFAULT_DEPTH;
				ALEF_LOAD_RANGE_Y1	= msdlg.m_nY_array		* MAP_DEFAULT_DEPTH;
				ALEF_LOAD_RANGE_X2	= msdlg.m_nX2_array		* MAP_DEFAULT_DEPTH;
				ALEF_LOAD_RANGE_Y2	= msdlg.m_nY2_array		* MAP_DEFAULT_DEPTH;

				// �ʷε�

				//g_MainWindow.LoadLocalMapFile();
				Load();
				g_MainWindow.MoveToSectorCenter	( NULL );
			}
		}
		break;
	case	TOOLBAR_LIGHTSETTING			:
		{
			// ����Ʈ Į�� ����..
			CRgbSelectDlg dlg;

			dlg.m_AmbiantColor		= *RpLightGetColor( g_pEngine->m_pLightAmbient	);
			dlg.m_DirectionalColor	= *RpLightGetColor( g_pEngine->m_pLightDirect	);

			
			switch( dlg.DoModal() )
			{
			case IDOK:
				// �Ŀ�;;
				RpLightSetColor( g_pEngine->m_pLightAmbient	, &dlg.m_AmbiantColor		);
				RpLightSetColor( g_pEngine->m_pLightDirect	, &dlg.m_DirectionalColor	);
				g_pcsAgcmRender->LightValueUpdate();

				/* Parn���� �۾��Ͻô�. Object�� Ambient ���� �ٲ�����.
				RwRGBA	rgba;

				rgba.red = 128 + dlg.m_DirectionalColor.red * 127;
				rgba.green = 128 + dlg.m_DirectionalColor.green * 127;
				rgba.blue = 128 + dlg.m_DirectionalColor.blue * 127;
				rgba.alpha = 255;

				g_pcsAgcmObject->ChangeAmbient( &rgba	);
				*/

				break;
			case IDC_BACKCOLORSET:
				g_pEngine->m_rgbBackgroundColor.blue	= ( RwUInt8 ) ( dlg.m_DirectionalColor.blue		* 255.0f );
				g_pEngine->m_rgbBackgroundColor.green	= ( RwUInt8 ) ( dlg.m_DirectionalColor.green	* 255.0f );
				g_pEngine->m_rgbBackgroundColor.red		= ( RwUInt8 ) ( dlg.m_DirectionalColor.red		* 255.0f );
				break;
			default:
				break;
			}
		}
		break;
	case	TOOLBAR_MOVE_LIGHT			:
		{
			// ����Ʈ ��ġ ����..

			RwFrame		*pFrame = RwCameraGetFrame	( g_pEngine->m_pCamera	);
			RwMatrix	*pMat	= RwFrameGetMatrix	( pFrame );
			RwV3d		*pvd;

			pvd	= RwMatrixGetAt( pMat );

			// ī�޶� ��ġ�� �̿��ؼ� ����Ʈ ��ġ�� ������.

			pFrame	= RpLightGetFrame( g_pEngine->m_pLightDirect );
			pMat	= RwFrameGetMatrix	( pFrame );
			RwV3d	* pCameraAt = RwMatrixGetAt( pMat );
			pCameraAt->x	= pvd->x;
			pCameraAt->y	= pvd->y;
			pCameraAt->z	= pvd->z;

			RwMatrixUpdate( pMat );
			RwFrameUpdateObjects( pFrame );
			RpLightSetFrame( g_pEngine->m_pLightDirect , pFrame );
		}
		break;
	case	TOOLBAR_TOGGLE_SHOW_GRID	:
		{
			// �׸��� �� ��!
			m_Document.ToggleGrid();
			if( this->m_pDungeonWnd ) this->m_pDungeonWnd->Invalidate( FALSE );
		}
		break;
	case	TOOLBAR_TOGGLE_RENDER_STATE	:
		{
			// ���� ������Ʈ ����..
			m_Document.m_bToggleShadingMode = true;
		}
		break;
	}
}
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN		||
		pMsg->message == WM_KEYUP		||
		pMsg->message == WM_MOUSEWHEEL	)
	{
		if( m_wndView.PreTranslateMessage( pMsg ) ) return TRUE;
	}
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnSave()
{
	Save();
}

UINT32 CMainFrame::Load()
{
	CProgressDlg dlg;
	dlg.StartProgress( "���õ���Ÿ �о����" , 100 , g_pMainFrame	);

	INT32	nCount = 0;
	INT32	nTarget = m_pTileList->m_pOthers->m_listPlugin.GetCount() + 1 ;

	_ProgressCallback( "������ �ε�" , nCount++ , nTarget , ( void * ) &dlg );

	// ������ (2004-06-28 ���� 8:00:02) : ��������..
	g_MainWindow.LoadLocalMapFile();

	// �÷����� �߰�..
	CUITileList_Others::Plugin				* pPlugin	;
	AuNode< CUITileList_Others::Plugin >	* pNode		= m_pTileList->m_pOthers->m_listPlugin.GetHeadNode();
	BOOL									bLoad		;
//	char									strQuery[ 1024 ];

	CString	str;
	while( pNode )
	{
		pPlugin = & pNode->GetData();

		try
		{
			if( ( ALEF_LOADING_MODE == LOAD_EXPORT_DIVISION || ALEF_LOADING_MODE == LOAD_EXPORT_LIST ) &&
				pPlugin->pWnd->m_stOption.bExcludeWhenExport )
			{
				// do nothing..
				// �ͽ���Ʈ �ÿ��� �о������ �ʰ���.
			}
			else
			{
				str.Format( "'%s' �÷����� ����Ÿ �ε�" , pPlugin->name );
				_ProgressCallback( ( LPSTR ) ( LPCTSTR ) str , nCount++ , nTarget , ( void * ) &dlg );

				bLoad = pPlugin->pWnd->OnLoadData();

				if( FALSE == bLoad )
				{
					TRACE( "�ε����~\n" );
				}

				pPlugin->pWnd->ClearSaveFlag();
			}
		}
		CRASH_SAFE_MESSAGE( pPlugin->name );

		m_pTileList->m_pOthers->m_listPlugin.GetNext( pNode );
	}

	dlg.EndProgress();

	return IDOK;
}

UINT32 CMainFrame::Save() 
{
	CSaveDlg dlg;	
	MyEngine * pMyEngine = ( MyEngine * ) g_pEngine;

	dlg.m_bMapInfo			= g_Const.m_bMapInfo		;
	dlg.m_bObjectList		= g_Const.m_bObjectList		;
	dlg.m_bObjectTemplate	= g_Const.m_bObjectTemplate	;
	dlg.m_bTileList			= g_Const.m_bTileList		;	
	dlg.m_bTileVertexColor	= g_Const.m_bTileVertexColor;
	dlg.m_bDungeonData		= g_Const.m_bDungeonData	;
	
	// �÷����� �߰�..
	CUITileList_Others::Plugin				* pPlugin	;
	AuNode< CUITileList_Others::Plugin >	* pNode		= m_pTileList->m_pOthers->m_listPlugin.GetHeadNode();
	BOOL									bSave		;
	char									strQuery[ 1024 ];

	while( pNode )
	{
		pPlugin = & pNode->GetData();

		bSave	= pPlugin->pWnd->OnQuerySaveData( strQuery );
		if( bSave )
		{
			dlg.AddPluginSaveData( pPlugin->pWnd->m_bSave , strQuery , pPlugin->pWnd );
		}

		m_pTileList->m_pOthers->m_listPlugin.GetNext( pNode );
	}

	UINT32 uRet = dlg.DoModal();
	if( uRet == IDOK )
	{
		CString	strResult	;
		BOOL	bFail		= FALSE;

		CProgressDlg	dlgProgress;
		dlgProgress.StartProgress( "Save" , 100 , this );

		char	strSub[ 1024 ];
		GetSubDataDirectory( strSub );

		#ifndef _DEBUG
		try
		#endif
		{
			if( dlg.m_bMapInfo || dlg.m_bTileVertexColor )
			{
				DisplayMessage( "��������....." );

				_CreateDirectory( strSub );
				_CreateDirectory( "%s\\map"					, strSub );
				_CreateDirectory( "%s\\map\\data"			, strSub );
				_CreateDirectory( "%s\\map\\data\\compact"	, strSub );
				_CreateDirectory( "%s\\map\\data\\geometry"	, strSub );
				_CreateDirectory( "%s\\map\\data\\moonee"	, strSub );
				
				if( AGCMMAP_THIS->SaveAll( dlg.m_bMapInfo , dlg.m_bTileVertexColor , FALSE ,
					 _ProgressCallback , ( void * ) &dlgProgress , strSub ) )
				{
					strResult	+=	"�������� - ����\r\n"	;
				}
				else
				{
					strResult	+=	"�������� - ����\r\n"	;
					bFail		=	TRUE					;
				}
			}

			if( dlg.m_bTileList )
			{
				DisplayMessage( "Ÿ�� ����Ʈ ����....." );

				// ī�װ� ����..
				this->m_pTileList->m_pTabWnd->SaveScript();
				
				// ��ũ��Ʈ ���̺�
				char filename [ 1024 ];
				wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , ALEF_TILE_LIST_FILE_NAME );

				if(AGCMMAP_THIS->m_TextureList.SaveScript( filename )  )
				{
					strResult	+=	"Ÿ�ϸ���Ʈ���� - ����\r\n"	;
				}
				else
				{
					strResult	+=	"Ÿ�ϸ���Ʈ���� - ����r\n"	;
					bFail		=	TRUE					;
				}
			}

			if( dlg.m_bObjectTemplate || dlg.m_bObjectList )
			{
				// ������ (2005-12-05 ���� 3:55:26) : 
				// ��Ʈ�� ������ �ͽ���Ʈ�ÿ��� ��.
				#ifndef USE_NEW_GRASS_FORMAT
					#ifndef _DEBUG
					try
					#endif
					{
						_CreateDirectory( "%s\\map\\data" , ALEF_CURRENT_DIRECTORY );
						_CreateDirectory( "%s\\map\\data\\octree" , ALEF_CURRENT_DIRECTORY );
						_CreateDirectory( "%s\\map\\data\\grass" , ALEF_CURRENT_DIRECTORY );

						// ��Ʈ ������ �����ؼ� �ѱ�..
						char strPath[ 1024 ];
						wsprintf( strPath , "%s\\map\\data" , ALEF_CURRENT_DIRECTORY );
						g_pcsAgcmRender->SaveAllOctrees( strPath );
						g_pcsAgcmGrass->SaveToFiles( strPath );


						// ������ (2005-04-14 ���� 5:50:36) : 
						// ����� ����
						_CreateDirectory( "%s\\map\\data" , strSub );
						_CreateDirectory( "%s\\map\\data\\octree" , strSub );
						_CreateDirectory( "%s\\map\\data\\grass" , strSub );

						// ��Ʈ ������ �����ؼ� �ѱ�..
						wsprintf( strPath , "%s\\map\\data" , strSub );
						g_pcsAgcmRender->SaveAllOctrees( strPath );
						g_pcsAgcmGrass->SaveToFiles( strPath );
					}
					#ifndef _DEBUG
					catch(...)
					{
						MessageBox( "��Ʈ�� ����� ���� �߻�~" , "���� �ͽ���Ʈ" , MB_ICONERROR | MB_OK );
					}
					#endif
				#endif

				DisplayMessage( "������Ʈ ���� ����......." );
				
				if( m_pTileList->m_pObjectWnd->SaveScript( dlg.m_bObjectTemplate , dlg.m_bObjectList ) )
				{
					strResult	+=	"������Ʈ ���� ���� - ����\r\n"	;
				}
				else
				{
					strResult	+=	"������Ʈ ���� ���� - ����r\n"	;
					bFail		=	TRUE							;
				}
			}

			/*
			if( dlg.m_bDungeonData )
			{
				// ���� ����Ÿ ����
				if( m_pDungeonWnd->SaveDungeon() )
				{
				
				}
				else
				{
					MD_SetErrorMessage( "���� ����Ÿ ���� ����" );
				}
			}
			*/
			
			// �÷����� ����..
			CSaveDlg::PluginSave				* pPluginSave	;
			AuNode< CSaveDlg::PluginSave >		* pNode			= dlg.m_listPlugin.GetHeadNode();
			while( pNode )
			{
				pPluginSave = & pNode->GetData();

				pPluginSave->pPlugin->m_bSave = pPluginSave->bSave;
				if( pPluginSave->bSave )
				{
					if( pPluginSave->pPlugin->OnSaveData() )
					{
						strResult	+=	pPluginSave->strName + "- ����\r\n"	;
						pPluginSave->pPlugin->ClearSaveFlag();
					}
					else
					{
						strResult	+=	pPluginSave->strName + "- ����r\n"	;
						bFail		=	TRUE							;
					}
				}

				dlg.m_listPlugin.GetNext( pNode );
			}

			DisplayMessage( "���̺긦 �Ϸ��Ͽ����ϴ�." );

			// ���� ����..
			g_Const.m_bMapInfo			= dlg.m_bMapInfo			;
			g_Const.m_bObjectList		= dlg.m_bObjectList			;
			g_Const.m_bObjectTemplate	= dlg.m_bObjectTemplate		;
			g_Const.m_bTileList			= dlg.m_bTileList			;	
			g_Const.m_bTileVertexColor	= dlg.m_bTileVertexColor	;
			g_Const.m_bDungeonData		= dlg.m_bDungeonData		;

			if( bFail )
			{
				CString	message;
				message = "���� ������ ������ �߻��Ͽ����ϴ�\r\n\r\n" + strResult;
				MessageBox( message , "-_-;;" , MB_ICONERROR );
			}

			ClearAllSaveFlags();
		}
		#ifndef _DEBUG
		catch( ... ) //CFileException* e)
		{
			if( IDYES == MessageBox( "���̺�������� ������ �߻��Ͽ����ϴ�. ����� ������ �ȵž��� ���ɼ��� ����!\n���α׷��� �׿��� �α׸� ���淡��? �ƴ� �׳� �ҷ���?" , "����" , MB_ICONERROR | MB_YESNOCANCEL ) )
			{
				throw;
			}
		}
		#endif

		dlgProgress.EndProgress();

	}

	return uRet;
}

void CMainFrame::OnRevert() 
{
	if( MessageBox( "���̺�ž��ִ� ������ ���������ϴ�. �����ڽ��ϱ�? -_-" , "����" , MB_YESNO | MB_ICONQUESTION )
		== IDYES )
	{
//		AGCMMAP_THIS->LoadAll		();
//		AGCMMAP_THIS->UpdateAll	();
//
//		DisplayMessage( AEM_NOTIFY , "Revert �Ϸ�." );
	}
}

void CMainFrame::OnUndo() 
{
	g_MainWindow.m_UndoManager.Undo();
}

void CMainFrame::OnRedo() 
{
	g_MainWindow.m_UndoManager.Redo();
}

void CMainFrame::OnRevertCurrent() 
{
}

void CMainFrame::OnUpdateNormal() 
{
	CProgressDlg	dlgProgress;
	dlgProgress.StartProgress( "Update Normal" , 100 , this );

	// �ε巴�� ����!
	MyEngine * pMyEngine = ( MyEngine * ) g_pEngine;
	ASSERT( NULL != pMyEngine );

	AGCMMAP_THIS->RecalcNormals( TRUE , _ProgressCallback , ( void * ) &dlgProgress	);

	dlgProgress.EndProgress();

}

void CMainFrame::OnClose() 
{
	if( ALEF_LOADING_MODE == LOAD_NORMAL )
	{
		switch( MessageBox( "�����Ҳ�����? -_-;;" , "�����ҷ���â" , MB_ICONWARNING | MB_YESNOCANCEL ) )
		{
		case IDYES		:
			OnSave();
			CFrameWnd::OnClose();
			break;
		case IDNO		:
			CFrameWnd::OnClose();
			break;
		case IDCANCEL	:
			break;
		}
	}
	else
	{
		CFrameWnd::OnClose();
	}
}

void CMainFrame::OnAdjustGeometryHeight() 
{
	CAdjustHeightDlg	dlg;

	if( dlg.DoModal() == IDOK )
	{
		// ��� �ʿ� ���̸� �����Ѵ�.

		OnDWSectorControl( IDM_DWSECTOR_ALL_DETAIL );
		
		int				i , j , x , z	;
		ApWorldSector *	pSector			;
		MyEngine *		pMyEngine		= ( MyEngine * ) g_pEngine;
		ApDetailSegment * pSegment		;


		UINT	currenttime = RsTimer();

		CProgressDlg	progressdlg;
		progressdlg.StartProgress( "���� ���� ����" , ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 ) * ( ALEF_LOAD_RANGE_Y2 - ALEF_LOAD_RANGE_Y1 ) , this );

		for( x = ALEF_LOAD_RANGE_X1 ; x <= ( INT32 ) ALEF_LOAD_RANGE_X2 ; x ++ )
		{
			for( z = ALEF_LOAD_RANGE_Y1 ; z <= ( INT32 ) ALEF_LOAD_RANGE_Y2 ; z ++ )
			{
				pSector = g_pcsApmMap->GetSectorByArray( x , 0 , z );

				if( NULL == pSector ) continue;

				if( NULL == pSector->GetNearSector( TD_NORTH	) ) continue;
				if( NULL == pSector->GetNearSector( TD_EAST		) ) continue;
				if( NULL == pSector->GetNearSector( TD_SOUTH	) ) continue;
				if( NULL == pSector->GetNearSector( TD_WEST		) ) continue;

				for( i = 0 ; i < MAP_DEFAULT_DEPTH ; i ++ )
				{
					for( j = 0 ; j < MAP_DEFAULT_DEPTH ; j ++ )
					{
						pSegment	= pSector->D_GetSegment( SECTOR_HIGHDETAIL , i , j );

						switch( dlg.m_nType )
						{
						case CAdjustHeightDlg::INSERT_HEIGHT	: pSegment->height	 =	dlg.m_fValue ; break;
						case CAdjustHeightDlg::ADD_HEIGHT		: pSegment->height	 +=	dlg.m_fValue ; break;
						case CAdjustHeightDlg::ADJUST_HEIGHT	: pSegment->height	 =	pSegment->height * dlg.m_fValue; break;
						}
					}
					
				}
				progressdlg.SetProgress	( z + x * ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 )	);
			}
		}
		progressdlg.EndProgress();

		AGCMMAP_THIS->UpdateAll();
	}	
}

// �۾� �� ü����..
void CMainFrame::OnModechangeGeometry() 
{
	m_pTileList->ChangeTab( EDITMODE_GEOMETRY		);
}

void CMainFrame::OnModechangeTile() 
{
	m_pTileList->ChangeTab( EDITMODE_TILE			);
}

void CMainFrame::OnModechangeObject() 
{
	m_pTileList->ChangeTab( EDITMODE_OBJECT			);
}

void CMainFrame::OnModechangeSubdivision() 
{
	m_pTileList->ChangeTab( EDITMODE_OTHERS	);
}

void CMainFrame::OnGridToggle() 
{
	OnToolBarMenuSelected( TOOLBAR_TOGGLE_SHOW_GRID	);
}

void CMainFrame::OnCameraFirstPerson() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnCameraReset() 
{
	OnToolBarMenuSelected( TOOLBAR_GOTO_SELECT_GEOMETRY	);
}

// Ÿ���� ����Ű.
void CMainFrame::OnTileSpoidBack() 
{
	switch( m_Document.m_nSelectedMode )
	{
	case EDITMODE_TILE			:
	case EDITMODE_OBJECT		:
		{
			int index = g_MainWindow.GetTileIndexOnMouseCursor( 0 );
			if( index != 0 )
			{

				m_pTileList->m_pTabWnd->SelectTile( index );
				m_Document.m_nSelectedMode						= EDITMODE_TILE			;
				m_pTileList->ChangeTab(  EDITMODE_TILE	)	;
				g_pMainFrame->m_Document.m_nCurrentTileLayer	= TILELAYER_BACKGROUND	;
				m_pTileList->m_pTabWnd->m_nSelectedLayer		= TILELAYER_BACKGROUND	;
				m_pTileList->Invalidate( FALSE );
			}
		}
		break;
	case EDITMODE_GEOMETRY		:
		{
			// ���ؽ� Į�� �����̵�..
			RwRGBA rgb;
			if( g_MainWindow.GetVertexColorOnMouseCursor( &rgb ) )
			{
				// Į�� ���� ����..
				m_pTileList->m_pMapWnd->SetVertexColor( rgb.red , rgb.green , rgb.blue);
			}
		}
		break;

	default:
		{
			// do nothing..
		}
		break;

	}
}

void CMainFrame::OnTileSpoidAlpha() 
{
//	int index = g_MainWindow.GetTileIndexOnMouseCursor( 1 );
//	if( index != 0 )
//	{
//
//		m_pTileList->m_pTabWnd->SelectTile( index );
//		m_Document.m_nSelectedMode						= EDITMODE_TILE		;
//		m_pTileList->ChangeTab(  EDITMODE_TILE	)	;
//		g_pMainFrame->m_Document.m_nCurrentTileLayer	= TILELAYER_UPPER	;
//		m_pTileList->m_pTabWnd->m_nSelectedLayer		= TILELAYER_UPPER	;
//		m_pTileList->Invalidate( FALSE );
//	}
}

void CMainFrame::OnTileSpoidAll() 
{
//	if( m_Document.m_nSelectedMode != EDITMODE_TILE ) return;
}

//RpMaterial * RestoreTextureName	( RpMaterial * material, void * data );
//RpMaterial * ChangeTextureName	( RpMaterial * material, void * data );
//
//RpWorldSector *ChangeWorldSectorDetailCallback(RpWorldSector *worldSector, void *data)
//{
//	RpDWSectorUpdateCurrent		( worldSector						);
//	RpDWSectorSetCurrentDetail	( worldSector , ( RwUInt8 ) data	);
//	return worldSector;
//}


BOOL	CMainFrame::ExportTile		()
{
	VERIFY(AGCMMAP_THIS->m_TextureList.ExportTile( NULL ) );

	return TRUE;
}

BOOL	CMainFrame::ExportDivisions	( int *pDivisions , int nDivisionCount
									, BOOL bMapDetail	
									, BOOL bMapRough		
									, BOOL bTile			
									, BOOL bCompactData	
									, BOOL bObject		
									, BOOL bMinimapExport
									, BOOL bServerDataExport)
{
	CExportDlg	dlg; // ���� �����ʰ� �ɼ� ��������θ� ��..

	dlg.m_bMapDetail		= bMapDetail		;
	dlg.m_bMapRough			= bMapRough			;
	dlg.m_bTile				= bTile				;
	dlg.m_bCompactData		= bCompactData		;
	dlg.m_bObject			= bObject			;
	dlg.m_bMinimapExport	= bMinimapExport	;
	dlg.m_bServerDataExport	= bServerDataExport	;

	CProgressDlg	dlgProgress;

	UINT32	uStartTime = GetTickCount();

	g_MainWindow.m_pCurrentGeometry	= NULL;
	g_MainWindow.m_UndoManager.ClearAllUndoData();

	DisplayMessage( AEM_NOTIFY , "������ BSP ������ �ͽ���Ʈ �ϰ��ֽ��ϴ�." );

	if( bTile )
	{
		ExportTile();
	}

	dlgProgress.StartProgress( "�ͽ���Ʈ�۾�!" , nDivisionCount , this );

	INT32	nDivisionLoaded;
	int nCount = 0;

	UINT32	uExportStart	;
	UINT32	uExportPastTime	;

	// �α� ���..
	time_t long_time;
	tm * pLocalTime;

	time( &long_time );                /* Get time as long integer. */
	pLocalTime = localtime(  &long_time );

	char strDestination[ 1024 ];
	wsprintf( strDestination , "export%02d%02d" , pLocalTime->tm_mon + 1 , pLocalTime->tm_mday );
	_CreateDirectory( strDestination );
	_CreateDirectory( "%s\\World" , strDestination );

	for( int i = 0 ; i < nDivisionCount ; i ++ )
	{
		dlgProgress.SetProgress( ++nCount );
		nDivisionLoaded = pDivisions[ i ];

		uExportStart = GetTickCount();
		LoadDivision( nDivisionLoaded );
		ExportDivision( nDivisionLoaded , &dlg , strDestination );
		uExportPastTime = GetTickCount() - uExportStart;

		DisplayMessage( AEM_ERROR , "(%02d/%02d) �ͼ���Ʈ ������. ���� �����ð�.. %d��" , nCount , nDivisionCount , ( uExportPastTime * ( nDivisionCount - nCount ) ) / 60000 );
	}

	UINT32	uPastTime = GetTickCount() - uStartTime;
	DisplayMessage( AEM_NOTIFY , "������ BSP ������ �ͽ���Ʈ �Ͽ����ϴ�.(%d�� �ɸ�)" ,  ( INT32 ) ( ( float ) uPastTime / 60000.0f ) );
	dlgProgress.EndProgress();
	return TRUE;
}

BOOL	__CBCollectLoadedDivision ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CMapSelectDlg * pMSDlg = ( CMapSelectDlg * ) pData;

	ASSERT( pMSDlg );

	pMSDlg->AddSelection( pDivisionInfo->nIndex );

	return TRUE;
}

void CMainFrame::OnBspExport() 
{
	CExportDlg	dlg;

	BOOL	bDestroyWindow = FALSE;

	if( dlg.DoModal() == IDOK )
	{
		CMapSelectDlg msdlg;

		msdlg.m_nX_array	= ALEF_LOAD_RANGE_X1 / MAP_DEFAULT_DEPTH;
		msdlg.m_nY_array	= ALEF_LOAD_RANGE_Y1 / MAP_DEFAULT_DEPTH;
		msdlg.m_nX2_array	= ALEF_LOAD_RANGE_X2 / MAP_DEFAULT_DEPTH;
		msdlg.m_nY2_array	= ALEF_LOAD_RANGE_Y2 / MAP_DEFAULT_DEPTH;

		// �ε�� ����� ����.
		AGCMMAP_THIS->EnumLoadedDivision( __CBCollectLoadedDivision , ( void * ) &msdlg );

		if( IDOK == msdlg.DoModal( EXPORT_RANGE_SELECT ) )
		{
			if( dlg.m_bMinimapExport )
			{
				// �̴ϸ� ĸ�� ������ ����.
				INT32 nCameraType;
				switch( MessageBox( "���ػ� �̴ϸ� ĸ�ĸ� ����ҷ���?" , "�̴ϸ��ͽ���Ʈ" , MB_YESNOCANCEL ) )
				{
				case IDYES	:	nCameraType = g_MyEngine.SC_MINIMAPX4	; break;

				default:
				case IDNO	:	nCameraType = g_MyEngine.SC_MINIMAP		; break;
				}

				dlg.m_nCameraType = nCameraType;
			}
			else
			{
				dlg.m_nCameraType = g_MyEngine.SC_MINIMAP;
			}

			ExportDivisions( &msdlg.m_MapSelectStatic.m_vectorDivision[ 0 ] , msdlg.m_MapSelectStatic.m_vectorDivision.size()
							,	dlg.m_bMapDetail		
							,	dlg.m_bMapRough			
							,	dlg.m_bTile				
							,	dlg.m_bCompactData		
							,	dlg.m_bObject			
							,	dlg.m_bMinimapExport	
							,	dlg.m_bServerDataExport);
		}
	}
}

BOOL	CMainFrame::LoadDivision	( INT32 nDivision )
{
	if( AGCMMAP_THIS->IsLoadedDivision( nDivision ) ) return TRUE;

	CleanUp();

	ALEF_LOAD_RANGE_X1	= ( GetDivisionXIndex( nDivision ) + 0 )	* MAP_DEFAULT_DEPTH;
	ALEF_LOAD_RANGE_Y1	= ( GetDivisionZIndex( nDivision ) + 0 )	* MAP_DEFAULT_DEPTH;
	ALEF_LOAD_RANGE_X2	= ( GetDivisionXIndex( nDivision ) + 1 )	* MAP_DEFAULT_DEPTH;
	ALEF_LOAD_RANGE_Y2	= ( GetDivisionZIndex( nDivision ) + 1 )	* MAP_DEFAULT_DEPTH;

	// �ʷε�

	//g_MainWindow.LoadLocalMapFile();
	//m_pTileList->m_pObjectWnd->LoadLocalObjectInfo();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	Load();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();

	return TRUE;

}
BOOL	CMainFrame::ExportDivision	( INT32 nDivision , CExportDlg * pDlg , char * pDestinationDirectory)
{
	if( !pDlg || !pDestinationDirectory )
		return FALSE;

	ASSERT( NULL != pDestinationDirectory );

	FLOAT	fFogDistanceBackup , fFogFarClipBackup ;
	BOOL	bUseFogBackup;

	// ���� ������ ����..
	fFogFarClipBackup	= g_pcsAgcmEventNature->GetFogFarClip	()	;
	fFogDistanceBackup	= g_pcsAgcmEventNature->GetFogDistance	()	;
	bUseFogBackup		= g_pcsAgcmEventNature->IsFogOn			()	;

	g_pcsAgcmEventNature->SetFog(
		FALSE			,
		100000			,
		200000			,
		g_pcsAgcmEventNature->GetFogColor		()	);

	ASSERT( NULL != pDestinationDirectory );

	DivisionInfo	stInfo;
	stInfo.GetDivisionInfo( nDivision );

	LoadDivision( nDivision );

	// �� ��⿡ �ͽ���Ʈ ����Ÿ ����..
	if( m_pTileList && m_pTileList->m_pOthers )
		m_pTileList->m_pOthers->Export( FALSE );

	// ������ ����� ����� ����..
	UINT32	uPrev	= m_Document.m_uAtomicFilter;

	UINT32	nNewFlag = 0;
	nNewFlag |= CAlefMapDocument::AF_GEOMTERY			;
	nNewFlag |= CAlefMapDocument::AF_OBJECTALL			;
	nNewFlag |= CAlefMapDocument::AF_OBJECT_RIDABLE		;
	nNewFlag |= CAlefMapDocument::AF_OBJECT_BLOCKING	;
	nNewFlag |= CAlefMapDocument::AF_OBJECT_EVENT		;
	nNewFlag |= CAlefMapDocument::AF_OBJECT_GRASS		;
	nNewFlag |= CAlefMapDocument::AF_OBJECT_OTHERS		;

	m_Document.m_uAtomicFilter = nNewFlag;

	g_MainWindow.MoveToSectorCenter	( NULL );

	PROCESSIDLE();

	/*
	if( pDlg->m_bServerDataExport )
	{
		// ��������Ÿ �ͽ���Ʈ..
		AGCMMAP_THIS->EnumLoadedDivision( CMainFrame::CBSeverDataExport , ALEF_CURRENT_DIRECTORY	);
		AGCMMAP_THIS->EnumLoadedDivision( CMainFrame::CBSeverDataExport , pDestinationDirectory		);
	}
	*/

	if( pDlg->m_bMapDetail || pDlg->m_bMapRough || pDlg->m_bCompactData )
	{
		// ������Ʈ ��ŷ �ٽ� ����..
		RefreshObjectBlocking();
		
		// ������ ���� ���� �ٽ� ����..
		if( pDlg->m_bMapRough )
		{
			// ������ (2004-09-16 ���� 9:30:55) : ������ �����ø� ..
			OnGenerateRoughmap();
		}

		// �����Ͽ� �ݸ��� ���� ������ ����..
		//OnEditCreateCollision();
		DisplayMessage( "�ݸ�������� ����" );
		AGCMMAP_THIS->CreateAllCollisionAtomic();

		CProgressDlg	dlgProgress;
		dlgProgress.StartProgress( "�ͽ���Ʈ" , 256 , this );
		VERIFY(AGCMMAP_THIS->BspExport(
			pDlg->m_bMapDetail			,
			pDlg->m_bMapRough			,
			pDlg->m_bCompactData		,
			FALSE						, //  ��������Ÿ.
			pDestinationDirectory		,
			_ProgressCallback			,
			( void * ) &dlgProgress	)	);

		if( pDlg->m_bCompactData )
		{
			// ��������Ÿ ����..
			if( m_pTileList && m_pTileList->m_pOthers )
				m_pTileList->m_pOthers->Export( TRUE );

			// ��������Ÿ�� �ͽ���Ʈ
			VERIFY(AGCMMAP_THIS->BspExport(
				FALSE						,
				FALSE						,
				FALSE						,
				pDlg->m_bCompactData		, //  ��������Ÿ.
				pDestinationDirectory		,
				_ProgressCallback			,
				( void * ) &dlgProgress	)	);
		}
		dlgProgress.EndProgress();
	}

	if( pDlg->m_bObject )
	{
		DisplayMessage( "OcTree ���� ����......." );

		// Octree����(octree�� ���� ����check.. ������Ʈ���� octree������ ���Ƿ� �̸� ����)
		#ifndef _DEBUG
		try
		#endif
		{
			char strPath[ 1024 ];
			// ���� ���� ����
			{			
				wsprintf( strPath , "%s\\world" , ALEF_CURRENT_DIRECTORY );
				CreateDirectory( strPath , NULL );
				wsprintf( strPath , "%s\\world\\octree" , ALEF_CURRENT_DIRECTORY );
				CreateDirectory( strPath , NULL );
				wsprintf( strPath , "%s\\world\\water" , ALEF_CURRENT_DIRECTORY );
				CreateDirectory( strPath , NULL );
				wsprintf( strPath , "%s\\world\\grass" , ALEF_CURRENT_DIRECTORY );
				CreateDirectory( strPath , NULL );

				// ��Ʈ ������ �����ؼ� �ѱ�..
				wsprintf( strPath , "%s\\world" , ALEF_CURRENT_DIRECTORY );
				g_pcsAgcmRender->SaveAllOctrees( strPath );

				// Ǯ ����.
				g_pcsAgcmGrass->SaveToFiles( strPath );
				// ���� ����
				g_pcsAgcmWater->SaveToFiles( strPath );
			}

			// �׳� ���� ����
			{
				wsprintf( strPath , "%s\\world" , pDestinationDirectory );
				CreateDirectory( strPath , NULL );
				wsprintf( strPath , "%s\\world\\octree" , pDestinationDirectory );
				CreateDirectory( strPath , NULL );
				wsprintf( strPath , "%s\\world\\water" , pDestinationDirectory );
				CreateDirectory( strPath , NULL );
				wsprintf( strPath , "%s\\world\\grass" , pDestinationDirectory );
				CreateDirectory( strPath , NULL );

				// ��Ʈ ������ �����ؼ� �ѱ�..
				wsprintf( strPath , "%s\\world" , pDestinationDirectory );
				if( !g_pcsAgcmRender->SaveAllOctrees( strPath ) )
				{
					DisplayMessage( RGB( 255 , 123 , 123 ) , "%04d ������� ��Ʈ�� �ͽ���Ʈ ����" , nDivision );
				}

				// Ǯ ����.
				g_pcsAgcmGrass->SaveToFiles( strPath );
				// ���� ����
				g_pcsAgcmWater->SaveToFiles( strPath );
			}
		}
		#ifndef _DEBUG
		catch(...)
		{
			MessageBox( "��Ʈ�� ����� ���� �߻�~" , "���� �ͽ���Ʈ" , MB_ICONERROR | MB_OK );
		}
		#endif

		// ���� ���� ����
		VERIFY( g_pcsAgcmObject->ObjectExport( 
			ALEF_LOAD_RANGE_X1,ALEF_LOAD_RANGE_Y1,
			ALEF_LOAD_RANGE_X2,ALEF_LOAD_RANGE_Y2 ,
			NULL) );
		// �ͽ���Ʈ ���� ����
		VERIFY( g_pcsAgcmObject->ObjectExport( 
			ALEF_LOAD_RANGE_X1,ALEF_LOAD_RANGE_Y1,
			ALEF_LOAD_RANGE_X2,ALEF_LOAD_RANGE_Y2 ,
			pDestinationDirectory) );

		g_pcsAgcmObject->ServerExport( nDivision , NULL );
		g_pcsAgcmObject->ServerExport( nDivision , pDestinationDirectory );
	}

	if( pDlg->m_bMinimapExport )
	{
		INT32	nPrevType	= g_MyEngine.GetSubCameraType();
		g_MyEngine.SetSubCameraType( pDlg->m_nCameraType );//g_MyEngine.SC_MINIMAP );

		{
			DivisionMinimapExportSetting setting;
			if( setting.result_ )
			{
				VERIFY( __DivisionMinimapExportCallback( &stInfo , NULL						) );
				VERIFY( __DivisionMinimapExportCallback( &stInfo , pDestinationDirectory	) );
			}
		}

		g_MyEngine.SetSubCameraType( nPrevType );
		g_MyEngine.MoveSubCameraToCenter();
	}

	// ����� ���� ��������..
	m_Document.m_uAtomicFilter = uPrev;

	g_pcsAgcmEventNature->SetFog(
		bUseFogBackup			,
		fFogDistanceBackup		,
		fFogFarClipBackup		,
		g_pcsAgcmEventNature->GetFogColor		()	);

	return TRUE;
}

void CMainFrame::OnTongMapHapChiGi() 
{
	// Ŀ�� ��۷� ����..

	SetCursor( NULL );
	DisplayMessage( "Ŀ��������");
}

void CMainFrame::OnHelpKeyreference() 
{
	CKeyReferenceDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnGenerateRoughmap() 
{
	FLOAT	fFogDistanceBackup , fFogFarClipBackup ;
	BOOL	bUseFogBackup;

	// ���� ������ ����..
	fFogFarClipBackup	= g_pcsAgcmEventNature->GetFogFarClip	()	;
	fFogDistanceBackup	= g_pcsAgcmEventNature->GetFogDistance	()	;
	bUseFogBackup		= g_pcsAgcmEventNature->IsFogOn			()	;

	// �ؽ��� ���ʷ�����..
	RoughTextureExport();

	// �ϴ� ��������.
	
	SetCurrentDetail( SECTOR_LOWDETAIL );
	OnEditCreateAllSector();

	// ������ ����.
	DisplayMessage( RGB( 255 , 0 , 0 ) , "������ ���� ���ʷ����� ����!" );

	// ���� �����Ϸ� ����..
	DisplayMessage( RGB( 90 , 52 , 223 ) , "������ ���̵����Ϸ� �ӽ� ����.!" );
	SetCurrentDetail( SECTOR_HIGHDETAIL );

	DisplayMessage( RGB( 90 , 52 , 223 ) , "������ ���̵����Ϸ� �ӽ� ����.!" );
	SetCurrentDetail( SECTOR_LOWDETAIL );

	DisplayMessage( RGB( 255 , 0 , 0 ) , "������ ���� ������ ����." );

	AGCMMAP_THIS->GenerateRoughMap( 
		SECTOR_LOWDETAIL					,
		SECTOR_HIGHDETAIL					,
		-1									,
		0.0f								,
		TRUE								,	// Height
		FALSE								,	// Tile .. ������ �ʴ´�.
		TRUE								);	// VertexColor �ܾ��..

	DisplayMessage( RGB( 255 , 0 , 0 ) , "�۾� ��~" );

	g_pcsAgcmEventNature->SetFog(
		bUseFogBackup			,
		fFogDistanceBackup		,
		fFogFarClipBackup		,
		g_pcsAgcmEventNature->GetFogColor		()	);

	SaveSetChangeGeometry();
	SaveSetChangeMoonee();
}

void CMainFrame::SetCurrentDetail( int nDetail )
{
	switch( nDetail )
	{
	case SECTOR_LOWDETAIL	:	OnDWSectorControl( IDM_DWSECTOR_ALL_ROUGH	);break;
	case SECTOR_HIGHDETAIL	:	OnDWSectorControl( IDM_DWSECTOR_ALL_DETAIL	);break;
	default:
		// do nothing;
		break;
	}
}

void CMainFrame::OnDWSectorControl(UINT nID)
{
	// �ͽ���Ʈ��忡�� �ƹ��͵� ���� �ʾƾ� �Ѵ�.
	CMenu	*pMenu = GetMenu();

	switch( nID )
	{
	case	IDM_DWSECTOR_EACHEACH           :
		DisplayMessage( AEM_NOTIFY , "��� �۵����� �ʽ��ϴ�." );
//		m_Document.m_nDWSectorSetting	= DWSECTOR_EACHEACH;
//		pMenu->CheckMenuItem( IDM_DWSECTOR_EACHEACH ,			MF_CHECKED		);
//		pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_ROUGH ,			MF_UNCHECKED	);
//		pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_DETAIL ,			MF_UNCHECKED	);
//		pMenu->CheckMenuItem( IDM_DWSECTOR_ACCORDINGTOCURSOR ,	MF_UNCHECKED	);
//		
//		AGCMMAP_THIS->SetCurrentDetail( SECTOR_DETAILDEPTH );
//
		break;
	case	IDM_DWSECTOR_ALL_ROUGH          :
		m_Document.m_nDWSectorSetting	= DWSECTOR_ALLROUGH;

		if( pMenu )
		{
			//		pMenu->CheckMenuItem( IDM_DWSECTOR_EACHEACH ,			MF_UNCHECKED	);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_ROUGH ,			MF_CHECKED		);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_DETAIL ,			MF_UNCHECKED	);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ACCORDINGTOCURSOR ,	MF_UNCHECKED	);
		}
		
		AGCMMAP_THIS->SetCurrentDetail( SECTOR_LOWDETAIL );

		m_Document.m_nSelectedDetail		= SECTOR_LOWDETAIL		;

		break;
	case	IDM_DWSECTOR_ALL_DETAIL         :
		m_Document.m_nDWSectorSetting	= DWSECTOR_ALLDETAIL;
//		pMenu->CheckMenuItem( IDM_DWSECTOR_EACHEACH ,			MF_UNCHECKED	);

		if( pMenu )
		{
			pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_ROUGH ,			MF_UNCHECKED	);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_DETAIL ,			MF_CHECKED		);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ACCORDINGTOCURSOR ,	MF_UNCHECKED	);
		}

		AGCMMAP_THIS->SetCurrentDetail( SECTOR_HIGHDETAIL );

		m_Document.m_nSelectedDetail		= SECTOR_HIGHDETAIL		;

		break;
	case	IDM_DWSECTOR_ACCORDINGTOCURSOR  :
		m_Document.m_nDWSectorSetting	= DWSECTOR_ACCORDINGTOCURSOR;

		if( pMenu )
		{
			//		pMenu->CheckMenuItem( IDM_DWSECTOR_EACHEACH ,			MF_UNCHECKED	);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_ROUGH ,			MF_UNCHECKED	);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ALL_DETAIL ,			MF_UNCHECKED	);
			pMenu->CheckMenuItem( IDM_DWSECTOR_ACCORDINGTOCURSOR ,	MF_CHECKED		);
		}

		AGCMMAP_THIS->SetCurrentDetail( SECTOR_LOWDETAIL );
		break;
	}
}

void CMainFrame::OnOptimizeAdjustEdge() 
{
	AGCMMAP_THIS->UpdateAll();
}

void CMainFrame::OnCreatesectorDetail() 
{
	g_MainWindow.CreateSector( TRUE );
}

void CMainFrame::OnCreatesector() 
{
	g_MainWindow.CreateSector();
}

void CMainFrame::OnToggleCameraProjectiontype() 
{
	switch( RwCameraGetProjection( g_pEngine->m_pCamera ) )
	{
	case rwPERSPECTIVE	:
		RwCameraSetProjection( g_pEngine->m_pCamera , rwPARALLEL );
		break;
	case rwPARALLEL		:
		RwCameraSetProjection( g_pEngine->m_pCamera , rwPERSPECTIVE );
		break;
	}
}

void CMainFrame::OnUpdateAll() 
{
	AGCMMAP_THIS->UpdateAll();
}

void CMainFrame::OnEditCreateAlpha() 
{
	g_MainWindow.ApplyAlphaOnTheSelection();
}

void CMainFrame::OnExportTile() 
{
	AGCMMAP_THIS->m_TextureList.ExportTile( NULL );
}

RwRGBA CMainFrame::GetVertexColor()
{
	return m_pTileList->m_pMapWnd->GetVertexColor();
}
void CMainFrame::OnSmoothGeometry() 
{
	OnUpdateNormal();
}

void CMainFrame::OnEditCreateAllSector() 
{
	CSectorCreateDlg dlg;
	int	detail;
	switch( m_Document.m_nDWSectorSetting )
	{
	case DWSECTOR_ALLROUGH			:
		{
			dlg.m_strDetail	= "������"					;
			dlg.m_nDepth	= ALEF_SECTOR_DEFAULT_DEPTH	;
			detail			= SECTOR_LOWDETAIL			;
		}
		break;
	case DWSECTOR_EACHEACH			:
	case DWSECTOR_ALLDETAIL			:
	case DWSECTOR_ACCORDINGTOCURSOR	:
		{
			dlg.m_strDetail	= "�����ϸ�"				;
			dlg.m_nDepth	= ALEF_SECTOR_DEFAULT_DEPTH	;
			detail			= SECTOR_HIGHDETAIL			;
		}
		break;
	}
	
//	if( dlg.DoModal() == IDOK )
//	{
//		
//	}
//	else
//	{
//		return;
//	}

	ApWorldSector	* pAlefSector;

	CProgressDlg	progressdlg;
	progressdlg.StartProgress( "���� ũ������Ʈ ��.." , ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 ) * ( ALEF_LOAD_RANGE_Y2 - ALEF_LOAD_RANGE_Y1 ) , this );

	int x , z;
	for( x = ALEF_LOAD_RANGE_X1 ; x <= ( INT32 ) ALEF_LOAD_RANGE_X2 ; x ++ )
	{
		for( z = ALEF_LOAD_RANGE_Y1 ; z <= ( INT32 ) ALEF_LOAD_RANGE_Y2 ; z ++ )
		{
			pAlefSector = g_pcsApmMap->GetSectorByArray( x , 0 , z );

			// ������ (2005-04-21 ���� 10:43:31) : 
			// ������ �ٽû���..
			switch( g_pMainFrame->m_Document.m_nDWSectorSetting )
			{
			case DWSECTOR_ALLROUGH			:
				{
					//pAlefSector->CreateDetailData	( SECTOR_LOWDETAIL , dlg.m_nDepth	);
					//AGCMMAP_THIS->Update		( pAlefSector , SECTOR_LOWDETAIL	);
				}
				break;
			case DWSECTOR_EACHEACH			:
			case DWSECTOR_ALLDETAIL			:
			case DWSECTOR_ACCORDINGTOCURSOR	:
				{
					pAlefSector->CreateDetailData	( SECTOR_HIGHDETAIL , dlg.m_nDepth	);
					//AGCMMAP_THIS->Update		( pAlefSector , SECTOR_HIGHDETAIL	);
					SaveSetChangeGeometry	();
					SaveSetChangeMoonee		();
				}
				break;
			}
			progressdlg.SetProgress	( z - ALEF_LOAD_RANGE_Y1 + ( x - ALEF_LOAD_RANGE_X1 ) * ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 )	);
		}
	}	
	
	for( x = ALEF_LOAD_RANGE_X1 ; x <= ( INT32 ) ALEF_LOAD_RANGE_X2 ; x ++ )
	{
		for( z = ALEF_LOAD_RANGE_Y1 ; z <= ( INT32 ) ALEF_LOAD_RANGE_Y2 ; z ++ )
		{
			pAlefSector = g_pcsApmMap->GetSectorByArray( x , 0 , z );

			// �ε��� �ž� ���� ������.. �����Ѵ�.
			switch( g_pMainFrame->m_Document.m_nDWSectorSetting )
			{
			case DWSECTOR_ALLROUGH			:
				{
					AGCMMAP_THIS->Update		( pAlefSector , SECTOR_LOWDETAIL	);
				}
				break;
			case DWSECTOR_EACHEACH			:
			case DWSECTOR_ALLDETAIL			:
			case DWSECTOR_ACCORDINGTOCURSOR	:
				{
					AGCMMAP_THIS->Update		( pAlefSector , SECTOR_HIGHDETAIL	);
				}
				break;
			}
			progressdlg.SetProgress	( z - ALEF_LOAD_RANGE_Y1 + ( x - ALEF_LOAD_RANGE_X1 ) * ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 )	);
		}
	}	

	progressdlg.EndProgress();
}

void CMainFrame::OnBrushSizeDown() 
{
	m_Document.m_fBrushRadius -= UIGM_MAX_CIRCLE_SIZE / 10.0f;
	if( m_Document.m_fBrushRadius < UIGM_MAX_CIRCLE_SIZE / 10.0f )
		m_Document.m_fBrushRadius = 100.0f;

	ASSERT( NULL != m_pTileList										);
	ASSERT( NULL != m_pTileList && NULL != m_pTileList->m_pMapWnd	);

	// �������� ������Ʈ..
	if( NULL != m_pTileList && NULL != m_pTileList->m_pMapWnd)
		m_pTileList->m_pMapWnd->Invalidate( FALSE );

	
}

void CMainFrame::OnBrushSizeUp() 
{
	m_Document.m_fBrushRadius += UIGM_MAX_CIRCLE_SIZE / 10.0f;
	if( m_Document.m_fBrushRadius > UIGM_MAX_CIRCLE_SIZE )
		m_Document.m_fBrushRadius = UIGM_MAX_CIRCLE_SIZE ;
	
	ASSERT( NULL != m_pTileList										);
	ASSERT( NULL != m_pTileList && NULL != m_pTileList->m_pMapWnd	);

	// �������� ������Ʈ..
	if( NULL != m_pTileList && NULL != m_pTileList->m_pMapWnd)
		m_pTileList->m_pMapWnd->Invalidate( FALSE );
	
}

void CMainFrame::OnFillTile() 
{
	if( m_Document.m_nSelectedMode != EDITMODE_TILE )
	{
		return;
	}

	g_MainWindow.OnApplyFill();	
}


void CMainFrame::OnTongMapApply() 
{
	if( m_Document.m_nSelectedMode != EDITMODE_TILE )
	{
		return;
	}

	g_MainWindow.OnApplyTongMap();		
}


void CMainFrame::OnOpenOptionDialog() 
{
	// TODO: Add your command handler code here
	COptionDialog	dlg;
	// ����Ÿ ����.

	dlg.m_bUsePolygonLock	= g_Const.m_bUsePolygonLock	;
	dlg.m_bUseBrushPolygon	= g_Const.m_bUseBrushPolygon;
	dlg.m_bCameraBlocking	= g_Const.m_bCameraBlocking	;

	dlg.m_bShowObjectBoundingSphere	= g_Const.m_bShowObjectBoundingSphere	;
	dlg.m_bShowObjectCollisionAtomic= g_Const.m_bShowObjectCollisionAtomic	;
	dlg.m_bShowObjectPickingAtomic	= g_Const.m_bShowObjectPickingAtomic	;
	dlg.m_bShowObjectRealPolygon	= g_Const.m_bShowObjectRealPolygon		;
	dlg.m_bShowOctreeInfo			= g_Const.m_bShowOctreeInfo				;
	
	if( g_pcsAgcmShadow2 )	dlg.m_bShadow2 = g_pcsAgcmShadow2->IsEnable()	;
	else					dlg.m_bShadow2 = FALSE							;
		
	if( IDOK == dlg.DoModal() )
	{
		g_Const.m_bUsePolygonLock	= dlg.m_bUsePolygonLock		;
		g_Const.m_bUseBrushPolygon	= dlg.m_bUseBrushPolygon	;
		g_Const.m_bCameraBlocking	= dlg.m_bCameraBlocking		;

		// ������ (2004-09-07 ���� 11:12:21) : ��� �������� ��..
		if( g_pcsAgcmShadow2 )
		{
			g_pcsAgcmShadow2->SetEnable( dlg.m_bShadow2 );
			g_pcsAgcmRender->m_bDrawShadow2 = dlg.m_bShadow2;
		}

		g_Const.m_bShowObjectBoundingSphere	= dlg.m_bShowObjectBoundingSphere	;
		g_Const.m_bShowObjectCollisionAtomic= dlg.m_bShowObjectCollisionAtomic	;
		g_Const.m_bShowObjectPickingAtomic	= dlg.m_bShowObjectPickingAtomic	;
		g_Const.m_bShowObjectRealPolygon	= dlg.m_bShowObjectRealPolygon		;
		g_Const.m_bShowOctreeInfo			= dlg.m_bShowOctreeInfo				;

		g_Const.Update();

		// ��Ʈ�� ���� ǥ�� �ʱ�ȭ.
		g_pcsAgcmOcTree->SetDrawEnd();
		g_pcsAgcmOcTree->SetOctreeDebugSector( NULL );

	}
}

void CMainFrame::OnToggleFpView() 
{
	// 1��Ī ���� ��ȯ..

	if( !m_Document.m_bUseFirstPersonView )
	{
		CLoginDlg	dlg;
		if( dlg.DoModal() == IDOK )
		{
			m_Document.ToggleFirstPersonViewMode( dlg.m_nID );
		}
	}
	else
	{
		m_Document.ToggleFirstPersonViewMode();
	}
}

static RpCollisionTriangle * __RsIntersectionCallBackObjectBlocking
											(RpIntersection *intersection, RpCollisionTriangle *collTriangle,
											RwReal distance, void *data)
{
	BOOL	* pRet = ( BOOL * ) data;
	* pRet = TRUE;

	// do nothing..
	// dummy..
	return NULL;
}

/*
BOOL	__DivisionObjectBlockingUpdateCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	AgcdObject	*	pstAgcmObject	= ( AgcdObject * ) pData				;
	RpAtomic	*	pAtomic			= pstAgcmObject->m_pstCollisionAtomic	;

	int					x , z	;
	ApWorldSector	*	pSector	;
	int					nSegmentX , nSegmentZ;
	RwSphere			stSphere	;

	RwBBox			bbox;
	AtomicGetBBox( pAtomic , & bbox );
	RwFrame		* pFrame	= RpAtomicGetFrame	( pAtomic	);
	RwMatrix	* pMatirx	= RwFrameGetLTM		( pFrame	);

	RwV3dTransformPoint( &bbox.inf , &bbox.inf , pMatirx );
	RwV3dTransformPoint( &bbox.sup , &bbox.sup , pMatirx );

	stSphere.center.x	= ( bbox.inf.x + bbox.sup.x ) / 2.0f;
	stSphere.center.y	= ( bbox.inf.y + bbox.sup.y ) / 2.0f;
	stSphere.center.z	= ( bbox.inf.z + bbox.sup.z ) / 2.0f;

	stSphere.radius		= AUPOS_DISTANCE_XZ( bbox.inf , stSphere.center );

	FLOAT				fX , fZ;

	RpIntersection		intersection	;
//	RwLine				pixelRay		;
	BOOL				bRet			;
	AuPOS				pos;
	FLOAT				fDistance		;

	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			pos.x	= ( pSector->GetXStart() + pSector->GetXEnd() ) / 2.0f;
			pos.y	= 0;
			pos.z	= ( pSector->GetZStart() + pSector->GetZEnd() ) / 2.0f;

			fDistance	= AUPOS_DISTANCE_XZ( pos , stSphere.center );

			if( fDistance > MAP_SECTOR_WIDTH * 1.414 + stSphere.radius ) continue;

			for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentZ )
			{
				for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; ++ nSegmentX )
				{
					// �� Ÿ�Ϻ��� ��ŷ üũ�ؼ� ����Ÿ ������.
					pSector->D_GetSegment( SECTOR_HIGHDETAIL , nSegmentX , nSegmentZ , & fX , & fZ );

					// �� �ݸ���..
					intersection.type			= rpINTERSECTLINE		;
					intersection.t.line.start.y	= SECTOR_MAX_HEIGHT		;
					intersection.t.line.end.y	= INVALID_HEIGHT		;

					////////////////////////////////////////////////////////////////////////////

					intersection.t.line.start.x	= fX + MAP_STEPSIZE * 0.25f ;
					intersection.t.line.start.z	= fZ + MAP_STEPSIZE * 0.25f ;
					intersection.t.line.end.x	= intersection.t.line.start.x	;
					intersection.t.line.end.z	= intersection.t.line.start.z	;

					bRet = FALSE;

					RpAtomicForAllIntersections (
						pAtomic							,
						&intersection					,
						__RsIntersectionCallBackObjectBlocking ,
						( void * ) & bRet );

					if( bRet )
					{
						// ������Ʈ ��ŷ..
						pSector->AddBlocking( nSegmentX * 2 + 0 , nSegmentZ * 2 + 0 , TBF_OBJECT );
					}

					intersection.t.line.start.x	= fX + MAP_STEPSIZE * 0.75f ;
					intersection.t.line.start.z	= fZ + MAP_STEPSIZE * 0.25f ;
					intersection.t.line.end.x	= intersection.t.line.start.x	;
					intersection.t.line.end.z	= intersection.t.line.start.z	;

					bRet = FALSE;

					RpAtomicForAllIntersections (
						pAtomic							,
						&intersection					,
						__RsIntersectionCallBackObjectBlocking ,
						( void * ) & bRet );

					if( bRet )
					{
						// ������Ʈ ��ŷ..
						pSector->AddBlocking( nSegmentX * 2 + 1 , nSegmentZ * 2 + 0 , TBF_OBJECT );
					}

					intersection.t.line.start.x	= fX + MAP_STEPSIZE * 0.25f ;
					intersection.t.line.start.z	= fZ + MAP_STEPSIZE * 0.75f ;
					intersection.t.line.end.x	= intersection.t.line.start.x	;
					intersection.t.line.end.z	= intersection.t.line.start.z	;

					bRet = FALSE;

					RpAtomicForAllIntersections (
						pAtomic							,
						&intersection					,
						__RsIntersectionCallBackObjectBlocking ,
						( void * ) & bRet );

					if( bRet )
					{
						// ������Ʈ ��ŷ..
						pSector->AddBlocking( nSegmentX * 2 + 0 , nSegmentZ * 2 + 1 , TBF_OBJECT );
					}

					intersection.t.line.start.x	= fX + MAP_STEPSIZE * 0.75f ;
					intersection.t.line.start.z	= fZ + MAP_STEPSIZE * 0.75f ;
					intersection.t.line.end.x	= intersection.t.line.start.x	;
					intersection.t.line.end.z	= intersection.t.line.start.z	;

					bRet = FALSE;

					RpAtomicForAllIntersections (
						pAtomic							,
						&intersection					,
						__RsIntersectionCallBackObjectBlocking ,
						( void * ) & bRet );

					if( bRet )
					{
						// ������Ʈ ��ŷ..
						pSector->AddBlocking( nSegmentX * 2 + 1 , nSegmentZ * 2 + 1 , TBF_OBJECT );
					}	
				}
			}

			////////////////////////////////////////////////////////////
		}
	}
	
	return TRUE;
}
*/

void	CMainFrame::RefreshObjectBlocking()
{
	// ������Ʈ ��ŷ�� ������� ����..

	/*
	// ��ŷ ������ �ʱ�ȭ�ϰ�..
	g_pcsApmMap->ClearAllObjectBlocking();	

	// �ٽ� �ϳ��ϳ��� �߰��Ѵ�.
	INT32					lIndex			=	0	;
	ApdObject			*	pstApdObject			;
	AgcdObject			*	pstAgcmObject			;
//	ApdObjectTemplate	*	pstApdObjectTemplate	;
//	AuBLOCKING				stBlockInfoCalcurated[APDOBJECT_MAX_BLOCK_INFO];		// Blocking Info

	// ��ϵ� ��� Object�� ���ؼ�...
	CProgressDlg dlg;
	dlg.StartProgress( "������Ʈ��ŷ ���÷���" , g_pcsApmObject->GetObjectAdmin()->m_csObject.m_lDataCount , g_pMainFrame );

	INT32	nCount			=	0	;
	INT32	nProperty				;
	for (	pstApdObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
			pstApdObject														;
			pstApdObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		dlg.SetProgress( lIndex );

		TRACE( "RefreshObjectBlocking , %d�� ������\n" , ++nCount );
		pstAgcmObject			= g_pcsAgcmObject->GetObjectData( pstApdObject );

		if( NULL == pstAgcmObject				) continue;
		// �ε���� ���� �༮�̴�.

		AgcdObjectGroupList *pstOGList = pstAgcmObject->m_stGroup.m_pstList;

		while( pstOGList )
		{
			if( NULL != pstOGList->m_csData.m_pstClump )
			{
				// ��ŷ�� �ƴѰ� ����.
				nProperty = AcuObject::GetProperty( pstAgcmObject->m_lObjectType );
				if( nProperty & ACUOBJECT_TYPE_BLOCKING	&&
					!( nProperty & ACUOBJECT_TYPE_RIDABLE )	)	
					// ������ (2004-03-17 ���� 12:39:14) : 
					// ���̴��� ��ŷ�� ���ִ� �ٸ��������ó���� �ȵż�..
					// ��ŷ�̰� , ���̴����� �ƴѰ͸�..ó����.
				{
					//pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate( pstApdObject->m_lTID );
					if( pstAgcmObject->m_pstCollisionAtomic )
					{
						AGCMMAP_THIS->EnumLoadedDivision( __DivisionObjectBlockingUpdateCallback , ( PVOID ) pstAgcmObject );
					}
				}
			}

			pstOGList = pstOGList->m_pstNext;
		}
	}
			
	dlg.EndProgress();

	TRACE( "���߽�\n" );

	*/
}

void CMainFrame::OnRefreahObjectBlocking() 
{
	// �׳� �޽����� ������..
	DisplayMessage( AEM_NOTIFY , "������Ʈ ��ŷ�� ���ۼ� �մϴ�." );

	RefreshObjectBlocking();

	DisplayMessage( AEM_NOTIFY , "������Ʈ ��ŷ ������Ʈ�� ���½��ϴ�." );
}

/*
void CMainFrame::OnShowGridMenuSelect(UINT nID)
{
	m_Document.m_nShowGrid = nID - IDM_VIEW_GRID_NOGRID;

	CMenu	*pMenu = GetMenu();

	pMenu->CheckMenuItem( IDM_VIEW_GRID_NOGRID	,	MF_UNCHECKED	);
	pMenu->CheckMenuItem( IDM_VIEW_GRID_FEW		,	MF_UNCHECKED	);
	pMenu->CheckMenuItem( IDM_VIEW_GRID_NORMAL	,	MF_UNCHECKED	);
	pMenu->CheckMenuItem( IDM_VIEW_GRID_LOT		,	MF_UNCHECKED	);

	pMenu->CheckMenuItem( IDM_VIEW_GRID_NOGRID + m_Document.m_nShowGrid	,	MF_CHECKED	);

	if( this->m_pDungeonWnd ) this->m_pDungeonWnd->Invalidate( FALSE );
}
*/

void CMainFrame::OnShowBlocking(UINT nID)
{
	switch( nID )
	{
	case IDM_VIEW_BLOCKING_GEOMETRY	:
		{
			m_Document.m_bShowGeometryBlocking	= ! m_Document.m_bShowGeometryBlocking	;
		}
		break;
	case IDM_VIEW_BLOCKING_OBJECT	:
		{
			m_Document.m_bShowObjectBlocking	= ! m_Document.m_bShowObjectBlocking	;
		}
		break;
	}
}

//
//RpClump *	_ChangeClumpVisibleCallBack(RpClump * clump, void *data)
//{
//	BOOL bVisible = ( BOOL ) data;
//
//	INT32	lID;
//
//	switch( AcuObject::GetClumpType( clump , & lID ) )
//	{
//	case	ACUOBJECT_TYPE_OBJECT		:
//		{
//
//		}
//		break;
//	case	ACUOBJECT_TYPE_CHARACTER	:
//	case	ACUOBJECT_TYPE_ITEM			:
//	case	ACUOBJECT_TYPE_WORLDSECTOR	:
//	default:
//		// Do no op
//		break;
//	}
//
//	return clump;
//}

void CMainFrame::OnViewToggleObject() 
{
//	m_Document.m_bShowObject = ! m_Document.m_bShowObject;
//
//	if( m_Document.m_bShowObject )
//	{
//		// ����� �˻��ϸ鼭 , ��� Visible check�� �ִ´�.
//		
//		RpWorldForAllClumps( g_MyEngine.m_pWorld , )
//	}
//	else
//	{
//		// ����� �˻��ϸ鼭 ��� Invisible check�� �ִ´�.
//
//	}

	ASSERT( !"���� ���� �ȵƾ��" );
	
}

void CMainFrame::OnViewClipplane() 
{
	CClipPlaneSetDlg	dlg;

	FLOAT	fFogDistanceBackup , fFogFarClipBackup ;
	BOOL	bUseFogBackup , bUseSkyBackup;

	// ��ī�� �������� �����..
	dlg.m_fFogFarClip	= g_pcsAgcmEventNature->GetFogFarClip	()	;
	dlg.m_fFogDistance	= g_pcsAgcmEventNature->GetFogDistance	()	;
	dlg.m_bUseFog		= g_pcsAgcmEventNature->IsFogOn		()	;
	dlg.m_bUseSky		= g_pcsAgcmEventNature->IsShowSky		()	;

	fFogFarClipBackup	= dlg.m_fFogFarClip		;
	fFogDistanceBackup	= dlg.m_fFogDistance	;
	bUseFogBackup		= dlg.m_bUseFog			;
	bUseSkyBackup		= dlg.m_bUseSky			;

	dlg.m_nLoadRange_Data	= g_Const.m_nLoadRange_Data		;
	dlg.m_nLoadRange_Rough	= g_Const.m_nLoadRange_Rough	;
	dlg.m_nLoadRange_Detail	= g_Const.m_nLoadRange_Detail	;
	
	if( dlg.DoModal() == IDOK && g_pEngine && g_pEngine->m_pCamera )
	{
		g_Const.m_nLoadRange_Data	= dlg.m_nLoadRange_Data		;
		g_Const.m_nLoadRange_Rough	= dlg.m_nLoadRange_Rough	;
		g_Const.m_nLoadRange_Detail	= dlg.m_nLoadRange_Detail	;
	}
	else
	{
		dlg.m_fFogFarClip	= 	fFogFarClipBackup	;
		dlg.m_fFogDistance	= 	fFogDistanceBackup	;
		dlg.m_bUseFog		= 	bUseFogBackup		;
		dlg.m_bUseSky		= 	bUseSkyBackup		;
	}

	g_pcsAgcmEventNature->SetSkyRadius( dlg.m_nLoadRange_Rough );
	
	g_pcsAgcmEventNature->SetFog(
		dlg.m_bUseFog										,
		dlg.m_fFogDistance									,
		dlg.m_fFogFarClip									,
		g_pcsAgcmEventNature->GetFogColor		()	);

	g_pcsAgcmEventNature->ShowSky( dlg.m_nLoadRange_Rough );

	AGCMMAP_THIS->SetAutoLoadRange	( g_Const.m_nLoadRange_Data , g_Const.m_nLoadRange_Rough , g_Const.m_nLoadRange_Detail );


	g_Const.m_fFogFarClip	= dlg.m_fFogFarClip		;
	g_Const.m_fFogDistance	= dlg.m_fFogDistance	;
	g_Const.m_bUseSky		= dlg.m_bUseSky			;
	g_Const.m_bUseFog		= dlg.m_bUseFog			;
}


static UINT32	__OpenEffectList()
{
	return g_pcsAgcmEffectDlg->OpenEffectList();
//	return 0;
}
static UINT32	__GetSoundLength( UINT32 lEid )
{
	return g_pcsAgcmEff2->GetSoundLength( lEid );
	// g_pcsAgcmEffect->GetEffectLength( lEid );
//	return 0;
}

void CMainFrame::OnSkySetting() 
{
	// ��ǵ� ����..
	g_pcsAgcmEventNatureDlg->pResampleTextureFunction
		= AgcmEventNature::ResampleTexture	;
	g_pcsAgcmEventNatureDlg->p__SetTimeFunction
		= AgcmEventNature::__SetTime			;
	g_pcsAgcmEventNatureDlg->p__SetFogFunction
		= AgcmEventNature::__SetFog			;
	g_pcsAgcmEventNatureDlg->p__MoveSky
		= AgcmEventNature::__MoveSky			;
	g_pcsAgcmEventNatureDlg->p__SetSkyTemplateID
		= AgcmEventNature::__SetSkyTemplateID	;

	g_pcsAgcmEventNatureDlg->p__OpenEffectList	= __OpenEffectList	;
	g_pcsAgcmEventNatureDlg->p__GetSoundLength	= __GetSoundLength	;

	g_pcsAgcmEventNatureDlg->OpenEditTemplateDialog( ALEF_CURRENT_DIRECTORY );
}

void CMainFrame::OnChangeFpsEditMode() 
{
	if( m_Document.m_bUseFirstPersonView )
	{
		if( m_pToolBar )
			m_pToolBar->ToggleFPSEditMode();
		else
		{
			MessageBox( "-_-?..." );
		}
	}
}

void CMainFrame::OnToggleTimer()
{
	g_pcsAgpmEventNature->ToggleTimer();
	DisplayMessage( "��� Ÿ�̸�~" );
}

extern RwBool illuminateProgressCB(RwInt32 message, RwReal value);
//@{ Jaewon 20050110
// technique change callback for the "aom", ambient occlusion map command
static void* effectTechniqueCB(SharedD3dXEffectPtr *pEffect, void *data)
{
	int index = *(int*)data;

	for(int i=0; i<3; ++i)
	{
		D3DXHANDLE hDesc = pEffect->d3dxEffect[i]->GetParameterByName(NULL, "description");
		const char *desc = NULL;
		if(hDesc)
			pEffect->d3dxEffect[i]->GetString(hDesc, &desc);
		if(desc && strstr(desc, "Ambient Occlusion"))
		{
			D3DXHANDLE handle = pEffect->d3dxEffect[i]->GetTechnique(index);
			if(handle)
			{
				D3DXTECHNIQUE_DESC technicDesc;
				pEffect->d3dxEffect[i]->SetTechnique(handle);
				pEffect->d3dxEffect[i]->GetTechniqueDesc(handle, &technicDesc);
				DisplayMessage("Technique changed to \"%s\".", technicDesc.Name);
			}
		}
	}

	return pEffect;
}
//@} Jaewon

void CMainFrame::OnOpenShortCut() 
{
//	g_pcsAgpmEventNature->ToggleTimer();
//	HWND hWnd = RsGetWindowHandle();
	CEditConsoleDlg dlg;

	CRect	rect;
	m_wndView.GetWindowRect( rect );
	dlg.m_Pos.x	= rect.left		;
	dlg.m_Pos.y = rect.bottom	;

	//@{ Jaewon 20040621
	// for testing a lightmap light
	static RpLight* pLightAttached = NULL;
	//@} Jaewon

	if( IDOK == dlg.DoModal() )
	{
		switch( dlg.m_nCommandType )
		{
		case	_TOGGLETIMER:
			{
				g_pcsAgpmEventNature->ToggleTimer();
				DisplayMessage( "��� Ÿ�̸�~" );
			}
			break;
		case	_SHOWMINIMAP:
			{
				if( g_MyEngine.GetSubCameraType() == MyEngine::SC_MINIMAP )
				{
					g_MyEngine.SetSubCameraType( MyEngine::SC_OFF );
					DisplayMessage( "�̴ϸ� ����" );
				}
				else
				{
					g_MyEngine.SetSubCameraType( MyEngine::SC_MINIMAP );
					DisplayMessage( "�� ����� �̴ϸ� ���÷���" );
				}
				g_MyEngine.MoveSubCameraToCenter();
			}
			break;
		case	_SHOWROUGHMAP:
			{
				if( g_MyEngine.GetSubCameraType() == MyEngine::SC_ROUGH )
				{
					g_MyEngine.SetSubCameraType( MyEngine::SC_OFF );
					DisplayMessage( "�̴ϸ� ����" );
				}
				else
				{
					g_MyEngine.SetSubCameraType( MyEngine::SC_ROUGH );
					DisplayMessage( "������ ���÷���." );
				}
				g_MyEngine.MoveSubCameraToCenter();
			}
			break;			
		case	_MINIMAPOFF:
			{
				g_MyEngine.SetSubCameraType( MyEngine::SC_OFF );
				DisplayMessage( "�̴ϸ� ����" );
			}
			break;
		case	_SHOWMINIMAPOBJECT:
			{
				g_MyEngine.SetRenderObject( !g_MyEngine.IsRenderObject() );
				if( g_MyEngine.IsRenderObject() )
					DisplayMessage( "�̴ϸ� ������Ʈ �����ֱ�" );
				else
					DisplayMessage( "�̴ϸ� ������Ʈ ���߱�" );
			}
			break;
		case	_SHOWCOLLISIONOBJECT:
			{
				g_Const.m_bShowCollisionAtomic = ! g_Const.m_bShowCollisionAtomic;

				if( g_Const.m_bShowCollisionAtomic )
					DisplayMessage( "���� �ݸ��� ������Ʈ ǥ��." );
				else
					DisplayMessage( "���� �ݸ��� ������Ʈ ����" );
			}
			break;
			
		case	_HELPCOMMAND	:
			{
				// ������!
				INT32	nIndex = _NOCOMMAND;
				char strOption[ 1024 ];

				CConsoleCommand * pCommand = NULL;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					dlg.m_Argument.GetParam( 1 , strOption );
					nIndex	= dlg.GetCommandIndex( strOption );

					AuNode< CConsoleCommand > * pNode = m_listCommand.GetHeadNode();
					while( pNode )
					{
						pCommand = &pNode->GetData();

						// �ߺ��ϴ°͸� �˻�..

						if( pCommand->nCommandIndex == nIndex )
						{
							TRACE( "�ߺ� Ŀ�ǵ� �߰�.\n");
							break;
						}

						m_listCommand.GetNext( pNode );
					}

					if( NULL == pNode ) nIndex = _NOCOMMAND;
				}

				if( nIndex == _NOCOMMAND || pCommand == NULL )
				{
					DisplayMessage( "-----���� �ֽܼý��� Ŀ�ǵ帮��Ʈ-----" );
					strcpy( strOption , "" );

					AuNode< CConsoleCommand > * pNode = m_listCommand.GetHeadNode();
					while( pNode )
					{
						pCommand = &pNode->GetData();

						// �ߺ��ϴ°͸� �˻�..
						strcat( strOption , pCommand->strAlias[ 0 ] );
						strcat( strOption , "," );

						m_listCommand.GetNext( pNode );
					}					

					strcat( strOption , "..." );

					DisplayMessage( strOption );

					DisplayMessage( "-----------------��------------------" );
				}
				else
				{
					DisplayMessage( "-----Ŀ�ǵ� ����-----" );
					DisplayMessage( "%s) %s" , pCommand->strAlias[ 0 ] , pCommand->strHelp );
					DisplayMessage( "-----��-----" );
				}
			}
			break;
		case	_SPHEREALPHA	:
			{
				INT32 nAlpha = 15;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nAlpha	= atoi( strOption );
				}
				// ������!
				DisplayMessage( "�������Ǿ� ���İ� ���� ( %d%% )" , nAlpha );

				g_MainWindow.ChangeSphereAlpha( nAlpha );
			}
			break;
		case	_SHOWRANGE		:
			{
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					m_Document.m_nRangeSphereType	= atoi( strOption );
				}
				else
				{
					// ���� ���� ���..
					m_Document.m_nRangeSphereType	= ( m_Document.m_nRangeSphereType + 1 ) % CAlefMapDocument::RST_MAX;
				}

				switch( m_Document.m_nRangeSphereType )
				{
				case	CAlefMapDocument::RST_SPAWN		:
					{
						DisplayMessage( "����ǥ�ø� ������ �������� ǥ����." );
						break;
					}
				case	CAlefMapDocument::RST_NATURE		:
					{
						DisplayMessage( "����ǥ�ø� �����ĸ� �������� ǥ����." );
						break;
					}
				default:
					{
						m_Document.m_nRangeSphereType = CAlefMapDocument::RST_NONE;
						DisplayMessage( "����ǥ�ø� ������." );
						break;
					}
				}

			}
			break;
		case	_BLENDMODE		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				INT32 nMode;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nMode	= atoi( strOption );

					RpMTextureSetBlendMode( nMode );
					DisplayMessage( "���� ��� ����~" );
				}
				else
				{
					DisplayMessage( "���� ��� ������ ���ڸ� �޾ƿ�~( ����Ʈ�� 4,��ⷹ��Ʈ2�� 5 )" );
				}
			}
			break;
		case	_MTEXTUREPASS		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				INT32 nMode;
				INT32 nPrev;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nMode	= atoi( strOption );

					nPrev = RpMTextureSetMaxTexturePerPass( nMode );

					wsprintf( strOption , "��Ƽ �ؽ��� �н����� --> %d , (���� %d����..)" , nMode , nPrev );
					DisplayMessage( strOption );
				}
				else
				{
					DisplayMessage( "��Ƽ�ؽ��� �н� ���� �̿���~ 1~8 ���� (���� 1-4����..) ���� ����" );
				}
			}
			break;
		case	_RUNNINGSPEED		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				INT32 nMode;
				INT32 nPrev;
				char strOption[ 1024 ];
				AgpdCharacter *	pCharacter = g_pcsAgcmCharacter->GetSelfCharacter();

				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					dlg.m_Argument.GetParam( 1 , strOption );
					nMode	= atoi( strOption );


					if( pCharacter )
					{
						nPrev = g_pcsAgpmCharacter->GetFastMoveSpeed( pCharacter );

						g_pcsAgpmFactors->SetValue(&pCharacter->m_csFactor, nMode, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

						wsprintf( strOption , "�ɸ��� �޸��� �ӵ��� %d->%d �� �����մϴ�( ����Ʈ 4000 )" , nPrev , nMode );
						DisplayMessage( strOption );
					}
					else
					{
						DisplayMessage( "�޸��� �ӵ� ������ FPS ��忡���� ��밡���մϴ�. " );
					}
				}
				else
				{
					nPrev = g_pcsAgpmCharacter->GetFastMoveSpeed( pCharacter );

					AgpdCharacter *	pCharacter = g_pcsAgcmCharacter->GetSelfCharacter();

					if( pCharacter )
					{
						wsprintf( strOption , "�ɸ��� �޸��� �ӵ����� Ŀ�ǵ� .. ���� �ӵ� %d" , nPrev );
						DisplayMessage( strOption );
					}
					else
					{
						DisplayMessage( "�޸��� �ӵ� ������ FPS ��忡���� ��밡���մϴ�. ( ����Ʈ 4000 )" );
					}
				}	
				
				// â������Ʈ ..
				m_Document.m_uRunSpeed	= nMode;
				if( this->m_pTileList ) this->m_pTileList->Invalidate( FALSE );
			}
			break;
		case	_VERTEXSHADER		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				INT32 nMode;
				INT32 nPrev;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nMode	= atoi( strOption );

					nPrev = RpMTextureEnableVertexShader( nMode );

					wsprintf( strOption , "���ý� ���̴� ��� --> %d , (���� %d����..)" , nMode , nPrev );
					DisplayMessage( strOption );
				}
				else
				{
					DisplayMessage( "���ؽ� ���̴� ��� ����.. 0-1" );
				}
			}
			break;
			
		case	_DLIGHTADJUST		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				FLOAT fMode;
				FLOAT fPrev;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fMode	= ( FLOAT ) atof( strOption );

					fPrev = g_pcsAgcmRender->SetDirectionalLightAdjust( fMode / 100.0f );
						//RpMTextureSetGeometryDirectionalLightAdjust( fMode / 100.0f );

					wsprintf( strOption , "Directional Light Adjustment --> %d%% , (���� %d%%����..)" , ( INT32 ) fMode , ( INT32 ) ( fPrev * 100.0f ) );
					DisplayMessage( strOption );
				}
				else
				{
					DisplayMessage( "Directional Light Adjustment , 0~100.0" );
				}
			}
			break;
		case	_ALIGHTADJUST		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				FLOAT fMode;
				FLOAT fPrev;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fMode	= ( FLOAT ) atof( strOption );

					fPrev = g_pcsAgcmRender->SetAmbientLightAdjust( fMode / 100.0f );
						//RpMTextureSetGeometryAmbientLightAdjust( fMode / 100.0f );

					wsprintf( strOption , "Ambient Light Adjustment --> %d%% , (���� %d%%����..)" , ( INT32 ) fMode , ( INT32 ) ( fPrev * 100.0f ) );
					DisplayMessage( strOption );
				}
				else
				{
					DisplayMessage( "Ambient Light Adjustment , 0~100.0" );
				}
			}
			break;

		case	_FARCLIPPLANE		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				FLOAT fMode;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fMode	= ( FLOAT ) atof( strOption );

					g_pEngine->SetWorldCameraFarClipPlane( fMode );

					wsprintf( strOption , "Farclipplane Adjustment --> %d%%" , ( INT32 ) fMode );
					DisplayMessage( strOption );
				}
				else
				{
					DisplayMessage( "Farclipplane Adjustment , Distance" );
				}
			}
			break;
		case	_SHOWBLOCKING_GEOMETRY		:
			{
				// ���� ��� ����~
				// ��۸� ����..

				if( !g_pcsAgcmRender->IsMaptoolBuild() )
				{
					DisplayMessage( "��ŷ �����ִ� �����尡 �ƴϳ׿�~ �����ڿ� ���Ǹ�~" );
					break;
				}

				INT32 nMode;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nMode	= atoi( strOption );

					g_pcsAgcmRender->SetDrawCollisionTerrain( nMode );

					if( nMode )
					{
						wsprintf( strOption , "������ŷ ������ ǥ�� --> �ѱ�" );
					}
					else
					{
						wsprintf( strOption , "������ŷ ������ ǥ�� --> ����" );
					}
					DisplayMessage( strOption );
				}
				else
				{
					DisplayMessage( "Farclipplane Adjustment , Distance" );
				}
			}
			break;
		case	_SHOWBLOCKING_OBJECT		:
			{
				// ���� ��� ����~
				// ��۸� ����..
				if( !g_pcsAgcmRender->IsMaptoolBuild() )
				{
					DisplayMessage( "��ŷ �����ִ� �����尡 �ƴϳ׿�~ �����ڿ� ���Ǹ�~" );
					break;
				}

				INT32 nMode;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nMode	= atoi( strOption );

					g_pcsAgcmRender->SetDrawCollisionObject( nMode );

					if( nMode )
					{
						wsprintf( strOption , "������Ʈ ��ŷ ������ ǥ�� --> �ѱ�" );
					}
					else
					{
						wsprintf( strOption , "������Ʈ ��ŷ ������ ǥ�� --> ����" );
					}
					DisplayMessage( strOption );
				}
				else
				{
					DisplayMessage( "Farclipplane Adjustment , Distance" );
				}
			}
			break;
			
		case _CREATESTAR:
			{
				int a= 5000 ,b = 1000 ,c = 500 ,d = 100;
				if( dlg.m_Argument.GetArgCount() >= 5 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					a	= atoi( strOption );
					dlg.m_Argument.GetParam( 2 , strOption );
					b	= atoi( strOption );
					dlg.m_Argument.GetParam( 3 , strOption );
					c	= atoi( strOption );
					dlg.m_Argument.GetParam( 4 , strOption );
					d	= atoi( strOption );
				}
				else
				{
					DisplayMessage( "���� ����~ ���ڰ� 4������~" );
				}

				g_pcsAgcmEventNature->GenerateStarVertex( a , b , c , d );
				DisplayMessage( "���峭~" );
			}
			break;
		case _SHOWSTARLEVEL:
			{

				INT32 nCommand = 0;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nCommand	= atoi( strOption );
				}

				g_pcsAgcmEventNature->SetStarShowLevel( nCommand );

				DisplayMessage( "���峭~" );
			}
			break;

		case _TESTCODE:
			{
				static BOOL bPlay = FALSE;
				char str[ 128  ] = "water2.wav";

				if( bPlay )
				{
					g_pcsAgcmSound->PlayBGM( str );
				}
				else
				{
					g_pcsAgcmSound->StopBGM( FALSE );
				}

				bPlay = ! bPlay;

				DisplayMessage( "�׽�Ʈ�ڵ� ����" );
			}
			break;

		//@{ Jaewon 20040621	
		case _TESTLIGHTMAP:
			{
				// remove any dynamic lightmap lights attached to the camera.
				if(pLightAttached)
				{
					((AgcmDynamicLightmap*)g_MyEngine.GetModule("AgcmDynamicLightmap"))->removeLight(pLightAttached);

					RwFrame* pFrame = RpLightGetFrame(pLightAttached);
					if(RwFrameGetParent(pFrame))
						RwFrameRemoveChild(pFrame);
					RpLightSetFrame(pLightAttached, NULL);
					RwFrameDestroy(pFrame);
					RpLightDestroy(pLightAttached);
					pLightAttached = NULL;

					DisplayMessage("removed the lightmap light which has been attached.");
				}

				if(dlg.m_Argument.GetArgCount() >= 2)
				{
					char strOption[1024];
					dlg.m_Argument.GetParam(1 , strOption);

					RpLight* pLight;

					if(strcmp(strOption, "point")==0)
					// add a point light.
					{
						pLight = RpLightCreate(rpLIGHTPOINT);

						DisplayMessage("a new point light created.");
					}
					else if(strcmp(strOption, "spot")==0)
					{
						pLight = RpLightCreate(rpLIGHTSPOT);
						RpLightSetConeAngle(pLight, 15.0f*3.141592f/180.0f);

						DisplayMessage("a new spot light created.");
					}
					else
					{
						DisplayMessage("Invalid argument!");
						return;
					}
						
					RwFrame* pFrame = RwFrameCreate();

					RwFrameSetIdentity(pFrame);

					RpLightSetFrame(pLight, pFrame);

					RpLightSetFlags(pLight, rpLIGHTLIGHTATOMICS|rpLIGHTLIGHTWORLD);

					RwRGBAReal col = { 1.0f, 0.0f, 0.0f, 0.0f };
					RpLightSetColor(pLight, &col);

					RpLightSetRadius(pLight, 5000.0f);

					pLightAttached = pLight;

					BOOL bMovable = TRUE;
					if(dlg.m_Argument.GetArgCount() >= 3)
					{
						bMovable = FALSE;
						RwFrameTransform(RpLightGetFrame(pLight), 
									RwFrameGetLTM(RwCameraGetFrame(g_MyEngine.m_pCamera)),
									rwCOMBINEREPLACE);
					}
					else
						RwFrameAddChild(RwCameraGetFrame(g_MyEngine.m_pCamera), pFrame);

					((AgcmDynamicLightmap*)g_MyEngine.GetModule("AgcmDynamicLightmap"))->addLight(pLightAttached, bMovable);

					RwV3d* camPos = RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(g_MyEngine.m_pCamera)));
					char buf[256];
					sprintf(buf, "a new light is attached to the camera at (%f, %f, %f).", 
							camPos->x, camPos->y, camPos->z);

					DisplayMessage(buf);
				}
			}
			break;
		//@} Jaewon

		//@{ Jaewon 20041229
		// to generate ambient occlusion maps
		case _AMBOCCLMAP:
			{
				if(dlg.m_Argument.GetArgCount() >= 2)
				{
					char strOption[1024];
					dlg.m_Argument.GetParam(1 , strOption);
					bool ok;
	
					if(strcmp(strOption, "cr")==0)
					// create
					{
						//@{ Jaewon 20050110
						if(g_MyEngine.m_computingAmbOcclMaps)
						{
							DisplayMessage("Invalid context!");
							return;
						}
						
						if(dlg.m_Argument.GetArgCount() >= 3)
						{
							dlg.m_Argument.GetParam(2, strOption);
							// A prefix string should have a length of less-than-4.
							strOption[4] = '\0';
							RtAmbOcclMapSetDefaultPrefixString(strOption);
						}
						else
							RtAmbOcclMapSetDefaultPrefixString("aomp");
						//@} Jaewon

						//@{ Jaewon 20050207
						// rayCount parameter added.
						ok = g_MyEngine.m_ambOcclMap.create(64);
						//@} Jaewon

						if(ok)
							DisplayMessage("Successfully created.");
						else
							DisplayMessage("Invalid context!");
					}
					else if(strcmp(strOption, "co")==0)
					// compute
					{
						//@{ Jaewon 20050110
						if(g_MyEngine.m_computingAmbOcclMaps)
						{
							DisplayMessage("Invalid context!");
							return;
						}
						//@} Jaewon

						//@{ Jaewon 20050112
						if(dlg.m_Argument.GetArgCount() >= 3)
						{
							// adjust the supersample value.
							dlg.m_Argument.GetParam(2, strOption);
							int superSample = atoi(strOption);
							g_MyEngine.m_ambOcclMap.setSuperSample((unsigned int)superSample);
							DisplayMessage("Set supersample to %d.", g_MyEngine.m_ambOcclMap.getSuperSample());
						}
						//@} Jaewon

						g_MyEngine.m_computingAmbOcclMaps = true;
						DisplayMessage("Start computing...");
					}
					else if(strcmp(strOption, "de")==0)
					// destroy
					{
						//@{ Jaewon 20050110
						if(g_MyEngine.m_computingAmbOcclMaps)
						{
							DisplayMessage("Invalid context!");
							return;
						}
						//@} Jaewon

						ok = g_MyEngine.m_ambOcclMap.destroy();

						if(ok)
							DisplayMessage("Successfully destroyed.");
						else
							DisplayMessage("Invalid context!");
					}
					else if(strcmp(strOption, "pa")==0)
					// pause
					{
						//@{ Jaewon 20050110
						if(g_MyEngine.m_computingAmbOcclMaps == false)
						{
							DisplayMessage("Invalid context!");
							return;
						}
						//@} Jaewon

						g_MyEngine.m_computingAmbOcclMaps = false;
						DisplayMessage("Computing paused.");
					}
					else if(strcmp(strOption, "cl")==0)
					// clear
					{
						//@{ Jaewon 20050110
						if(g_MyEngine.m_computingAmbOcclMaps)
						{
							DisplayMessage("Invalid context!");
							return;
						}
						//@} Jaewon

						ok = g_MyEngine.m_ambOcclMap.clear();

						if(ok)
							DisplayMessage("Successfully cleared.");
						else
							DisplayMessage("Invalid context!");
					}
					else if(strcmp(strOption, "sa")==0)
					// save
					{
						//@{ Jaewon 20050110
						if(g_MyEngine.m_computingAmbOcclMaps)
						{
							DisplayMessage("Invalid context!");
							return;
						}
						//@} Jaewon

						char buf[MAX_PATH];
						sprintf(buf, "%s\\Texture\\AmbOccl\\", ALEF_CURRENT_DIRECTORY);
						ok = g_MyEngine.m_ambOcclMap.save(buf);

						if(ok)
							DisplayMessage("Successfully saved.");
						else
							DisplayMessage("Invalid context or write failure!");
					}
					//@{ Jaewon 20050110
					else if(strcmp(strOption, "te")==0)
					// change the technique of "ambientOcclusion.fx".
					{
						int index = 0;
						if(dlg.m_Argument.GetArgCount() >= 3)
						{
							dlg.m_Argument.GetParam(2, strOption);
							index = atoi(strOption);
							if(0<=index && index<3)
							{
								RpMaterialD3DFxForAllEffects(effectTechniqueCB, (void*)&index);
							}
							else
								DisplayMessage("Invalid technique index!");
						}
						else
						{
							RpMaterialD3DFxForAllEffects(effectTechniqueCB, (void*)&index);
						}
					}
					//@} Jaewon
					else
					{
						DisplayMessage("Invalid argument!");
						return;
					}
				}
				else
				{
					DisplayMessage("ambocclmap [option]");
					DisplayMessage("following options are available.");
					DisplayMessage("cr - create ambient occlusion maps.");
					DisplayMessage("co - compute ambient occlusion maps.");
					DisplayMessage("de - destroy ambient occlusion maps of the current session.");
					DisplayMessage("pa - pause computing.");
					DisplayMessage("cl - clear ambient occlusion maps to checkerboard patterns.");
					DisplayMessage("sa - save ambient occlusion maps and object data.");
					return;
				}
			}
			break;
		//@} Jaewon

		case _CAMERAMOVINGSPEED		:
			{
				FLOAT fSpeed = m_Document.m_fCameraMovingSpeed;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fSpeed	= ( FLOAT ) atof( strOption );
				}
				
				m_Document.m_fCameraMovingSpeed = fSpeed;

				char	str[256 ];
				sprintf( str , "ī�޶� �̵��ӵ� �ʴ� %.0f ��ġ���� �̵�" , fSpeed );
                DisplayMessage( str );
			}
			break;

		//@{ Jaewon 20050531
		// ;)
		case _TOGGLEGLOSSMAP:
			{
				if(RpMTextureIsGlossMapEnabled())
				{
					RpMTextureEnableGlossMap(FALSE);
					DisplayMessage("Turn 'Gloss Map' on.");
				}
				else
				{
					RpMTextureEnableGlossMap(TRUE);
					DisplayMessage("Turn 'Gloss Map' off.");
				}
			}
			break;
		//@} Jaewon
		//@{ Jaewon 20050719
		// ;)
		case _TOGGLEMATD3DFX:
			{
				if(g_pcsAgcmRender)
				{
					if(g_pcsAgcmRender->IsMatD3DFxDisabled())
					{
						g_pcsAgcmRender->EnableMatD3DFx();
						DisplayMessage("Turn 'MatD3DFx' on.");
					}
					else
					{
						g_pcsAgcmRender->DisableMatD3DFx();
						DisplayMessage("Turn 'MatD3DFx' off.");
					}

					g_pcsAgcmRender->AllAtomicFxCheck();
				}
			}
			break;
		//@} Jaewon

		#ifdef	_PROFILE_
		case _PROFILE_TOGGLE		:
			{
				g_MainWindow.SetProfile( !g_MainWindow.IsProfile() );
				if( g_MainWindow.IsProfile() )
					DisplayMessage( "Profile Command : Show Info" );
				else
					DisplayMessage( "Profile Command : Hide Info" );
			}
			break;
			
		case _PROFILE_STEPINTO	:
			{
				INT32 nCommand = 0;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nCommand	= atoi( strOption );
				}

				if( nCommand >= 10	) nCommand = 0;
				if( nCommand < 0	) nCommand = 0;

				g_MainWindow.SetProfileCommand( PFC_STEPINTO0 + nCommand );
				DisplayMessage( "Profile Command : Step Into" );
			}
			break;
		case _PROFILE_REGISTER	:
			{
				INT32 nCommand = 0;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nCommand	= atoi( strOption );
				}

				if( nCommand >= 10	) nCommand = 0;
				if( nCommand < 0	) nCommand = 0;

				g_MainWindow.SetProfileCommand( PFC_REGISTER0 + nCommand );
				DisplayMessage( "Profile Command : Register Function" );
			}
			break;
		case _PROFILE_RESET		:
			{
				g_MainWindow.SetProfileCommand( PFC_RESET );
				DisplayMessage( "Profile Command : Reset" );
			}
			break;
		case _PROFILE_TOROOT	:
			{
				g_MainWindow.SetProfileCommand( PFC_TOROOT );
				DisplayMessage( "Profile Command : To Root" );
			}
			break;
		case _PROFILE_TOLEAF	:
			{
				g_MainWindow.SetProfileCommand( PFC_TOLEAF );
				DisplayMessage( "Profile Command : To Leaf" );
			}
			break;
		#endif

		case _CHANGESKYGAP:
			{

				INT32 nCommand = 0;
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					nCommand	= atoi( strOption );

					__uSkyChangeGap	= nCommand;
					
					char buf[256];
					sprintf(buf, "��ī�� ��ȭ �ð� ���� (%u)" , __uSkyChangeGap );
					DisplayMessage( buf );
				}
				else
				{
					char buf[256];
					sprintf(buf, "���ں��� : ��ī�� ��ȭ �ð� (%u)" , __uSkyChangeGap );
					DisplayMessage( buf );
				}
			}
			break;

		case _FADEOUTFACTOR:
			{
				FLOAT	fNear , fFar;
				if( dlg.m_Argument.GetArgCount() >= 3 )
				{
					// �ι�° ���� ���..
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fNear	= ( FLOAT ) atof( strOption );
					dlg.m_Argument.GetParam( 2 , strOption );
					fFar	= ( FLOAT ) atof( strOption );

					char buf[256];
					sprintf(buf, "���̵�ƿ� ����  (%f , %f)" , fNear , fFar );
					DisplayMessage( buf );

					//RpMTextureSetFadeFactor( fNear , fFar );
				}
				else
				{
					char buf[256];
					sprintf(buf, "���ں��� : ���̵�ƿ� ����  (%u)" , __uSkyChangeGap );
					DisplayMessage( buf );
				}
			}
			break;

		case _CREATE_COMPACTDATA:
			{
				int x1 = ALEF_LOAD_RANGE_X1 , x2 = ALEF_LOAD_RANGE_X2 , z1 = ALEF_LOAD_RANGE_Y1 , z2 = ALEF_LOAD_RANGE_Y2;

				ApWorldSector * pWorkingSector;
				int j , i;
				for( j = z1 ; j <= z2 ; j ++ )
				{
					for( i = x1 ; i <= x2 ; i ++ )
					{
						// ���� ã�ƺ���.
						pWorkingSector = g_pcsApmMap->GetSectorByArray( i , 0 , j );

						if( NULL == pWorkingSector ) continue;

						AGCMMAP_THIS->CreateCompactDataFromDetailInfo( pWorkingSector );
					}
				}// ���� �� ����..
				DisplayMessage( "����Ʈ ����Ÿ ���� �Ϸ�" );
			}
			break;

		case _SAVE_OCTREE:
			{
				// ��Ʈ�� ����.
				char strPath[ 1024 ];
				wsprintf( strPath , "%s\\map\\data" , ALEF_CURRENT_DIRECTORY );
				g_pcsAgcmRender->SaveAllOctrees( strPath );

				DisplayMessage( "��Ʈ�� ���� �Ϸ�" );

				// ���� ������ �Ѱ�� ���� Ȯ�� �ϵ��� �Ѵ�.
				g_Const.m_bShowOctreeInfo = TRUE;
			}
			break;
		case _DEBUG_EXPORT:
			{
				DisplayMessage( "���� �ε��� ������ ���͸� �ͽ���Ʈ�մϴ�." );

				CProgressDlg	dlgProgress;
				dlgProgress.StartProgress( "�ͽ���Ʈ" , 256 , this );
				VERIFY(AGCMMAP_THIS->BspExport(
					TRUE			,
					FALSE			,
					FALSE			,
					FALSE			,
					NULL			,
					_ProgressCallback			,
					( void * ) &dlgProgress	)	);
				dlgProgress.EndProgress();
			}
			break;

		case _GRASS_CHECK:
			{
				g_pcsAgcmGrass->UnityForSave();
 				if( g_pcsAgcmGrass->CheckGrassList() )
				{
					MessageBox( "����" );
				}
				else
				{
					MessageBox( "������!" );
				}
			}
			break;

		case _PERSFECTIVE:
			{
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					FLOAT	fPerspective;
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fPerspective	= ( FLOAT ) atof( strOption );

					char buf[256];
					sprintf(buf, "Perspective �� ���� (%.2f)" , fPerspective );
					g_pEngine->SetProjection( fPerspective );
					DisplayMessage( buf );
				}
				else
				{
					char buf[256];
					sprintf(buf, "���� Perspective �� = (%.2f)" , g_pEngine->GetProjection() );
					DisplayMessage( buf );
				}		
			}
			break;
		case _WIND:
			{
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					FLOAT	fXValue;
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fXValue	= ( FLOAT ) atof( strOption );

					if( 0.0f == fXValue )
					{
						DisplayMessage( "�ٶ� ���ϴ�" );
						g_pcsAgcmNatureEffect->SetWindEnable( FALSE );
					}
					else
					{
						char	str[ 256 ];
						sprintf( str , "%.2f ������ �ٶ��� �����մϴ� ( �������� �Ҿ�� )" , fXValue );

						RwV3d	vWind;
						vWind.x = fXValue;
						vWind.y = 0;
						vWind.z = 0;
						g_pcsAgcmNatureEffect->SetWindVector( vWind );
						g_pcsAgcmNatureEffect->SetWindEnable( TRUE );

						DisplayMessage( str );
					}
					
				}
				else
				{
					char buf[256];
					sprintf(buf, "���� Wind �� = (%.2f)" , g_pcsAgcmNatureEffect->GetWindVector()->x );
					DisplayMessage( buf );
				}		
			}
			break;
		case _SUNANGLE:
			{
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					FLOAT	fXValue;
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fXValue	= ( FLOAT ) atof( strOption );

					char	str[ 256 ];
					sprintf( str , "%.2f ������ �¾簢����" , fXValue );
					AgcmEventNature::__fsSunAngle = fXValue;
					g_pcsAgcmEventNature->MoveSky();
					DisplayMessage( str );
				}
				else
				{
					char buf[256];
					sprintf(buf, "���� �¾簢 �� = (%.2f)" , AgcmEventNature::__fsSunAngle );
					DisplayMessage( buf );
				}		
			}
			break;
		case _SUNSIZE:
			{
				if( dlg.m_Argument.GetArgCount() >= 2 )
				{
					// �ι�° ���� ���..
					FLOAT	fXValue;
					char strOption[ 1024 ];
					dlg.m_Argument.GetParam( 1 , strOption );
					fXValue	= ( FLOAT ) atof( strOption );

					char	str[ 256 ];
					sprintf( str , "%.2f ������ �¾�ũ�� ����" , fXValue );
	
					g_pcsAgcmEventNature->ChangeSunSize( fXValue );
					g_pcsAgcmEventNature->MoveSky();
					DisplayMessage( str );
				}
				else
				{
					char buf[256];
					sprintf(buf, "�°� ũ�� ����.. ����Ʈ�� 0.2"  );
					DisplayMessage( buf );
				}		
			}
			break;
		}
	}
}

BOOL	CMainFrame::CBSeverDataLoadTest( DivisionInfo * pDivisionInfo , PVOID pData )
{
	int nSectorX , nSectorZ;

	for( nSectorZ = pDivisionInfo->nZ ; nSectorZ < pDivisionInfo->nZ + pDivisionInfo->nDepth ; nSectorZ ++ )
	{
		for( nSectorX = pDivisionInfo->nX ; nSectorX < pDivisionInfo->nX + pDivisionInfo->nDepth ; nSectorX ++ )
		{
			ApWorldSector * pSector = g_pcsApmMap->GetSectorByArray( nSectorX , 0 , nSectorZ );

			if( pSector )
			{
				pSector->LoadServerData();
			}
		}
	}
	return TRUE;
}

BOOL	CMainFrame::CBSeverDataExport( DivisionInfo * pDivisionInfo , PVOID pData )
{
	g_pMainFrame->ExportServerData( pDivisionInfo->nIndex , ( char * ) pData );
	return TRUE;
}

void CMainFrame::OnObjectRefresh() 
{
	// ������Ʈ �������� �۾�!..

	char	filename[ FILENAME_MAX ];

	wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "ObjectTemplate.ini" );

	CProgressDlg dlg;
	dlg.StartProgress( "����" , 100 , g_pMainFrame );

	m_pTileList->m_pObjectWnd->m_wndTreeCtrl.DeleteAllItems();
	m_pTileList->m_pObjectWnd->LoadCategory();
	g_pcsAgcmObject->ReloadObjectClump( filename , _ProgressCallback , ( void * ) &dlg	);

	dlg.EndProgress();
}

void CMainFrame::OnAddWater() 
{
	g_MainWindow.OnAppyWater();
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch( nIDEvent )
	{
	case MAINFRM_DEBUG_TIMER_ID:
		{
			//if( ALEF_LOADING_MODE == LOAD_NORMAL )
			//	MD_ErrorCheck();
		}
		break;
	case MAINFRM_EXPORT_TIMER_ID:
		{
			KillTimer( MAINFRM_EXPORT_TIMER_ID );

			switch( ALEF_LOADING_MODE )
			{
			case	LOAD_EXPORT_DIVISION	:
				{
					/*
					CExportDlg	dlg;

					dlg.m_bMapDetail		= g_Const.m_stExportAutomateInfo.bMapDetail		;
					dlg.m_bMapRough			= g_Const.m_stExportAutomateInfo.bMapRough		;
					dlg.m_bTile				= g_Const.m_stExportAutomateInfo.bTile			;
					dlg.m_bCompactData		= g_Const.m_stExportAutomateInfo.bCompactData	;
					dlg.m_bObject			= g_Const.m_stExportAutomateInfo.bObject		;
					dlg.m_bMinimapExport	= g_Const.m_stExportAutomateInfo.bMinimap		;

					time_t long_time;
					tm * pLocalTime;

					time( &long_time );
					pLocalTime = localtime(  &long_time );

					char strDestination[ 1024 ];
					wsprintf( strDestination , "export%02d%02d" , pLocalTime->tm_mon + 1 , pLocalTime->tm_mday );
					_CreateDirectory( strDestination );
					_CreateDirectory( "%s\\World" , strDestination );

					ExportDivision( g_Const.m_stExportAutomateInfo.nDivision , &dlg , strDestination );
					*/

					ExportDivisions( &g_Const.m_stExportAutomateInfo.nDivision , 1
									,g_Const.m_stExportAutomateInfo.bMapDetail		
									,g_Const.m_stExportAutomateInfo.bMapRough		
									,g_Const.m_stExportAutomateInfo.bTile			
									,g_Const.m_stExportAutomateInfo.bCompactData	
									,g_Const.m_stExportAutomateInfo.bObject			
									,g_Const.m_stExportAutomateInfo.bMinimap	
									,g_Const.m_stExportAutomateInfo.bServer		);

					PostMessage( WM_CLOSE );
				}
				break;
			case	LOAD_EXPORT_LIST		:
				{
					CMapSelectDlg dlg;
					if( dlg.LoadSelection( g_Const.m_stExportAutomateInfo.strFilename ) )
					{
						ExportDivisions( &dlg.m_MapSelectStatic.m_vectorDivision[ 0 ] , dlg.m_MapSelectStatic.m_vectorDivision.size()
										,g_Const.m_stExportAutomateInfo.bMapDetail		
										,g_Const.m_stExportAutomateInfo.bMapRough		
										,g_Const.m_stExportAutomateInfo.bTile			
										,g_Const.m_stExportAutomateInfo.bCompactData	
										,g_Const.m_stExportAutomateInfo.bObject			
										,g_Const.m_stExportAutomateInfo.bMinimap	
										,g_Const.m_stExportAutomateInfo.bServer		);
					}
					else
					{
						MessageBox( "���������� ���� �� �����" );
					}
					PostMessage( WM_CLOSE );
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
	
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnFullScreenMode() 
{
	m_bFullScreen	= !m_bFullScreen;

	RECT DesktopRect;
    WINDOWPLACEMENT WPNew;

    if( MT_VM_FULLSCREEN == m_bFullScreen )
	{
	    // We'll need these to restore the original state.
	    GetWindowPlacement (&m_WPPrev);

	    m_WPPrev.length	= sizeof m_WPPrev;

        //Adjust RECT to new size of window
	    ::GetWindowRect(::GetDesktopWindow(), &DesktopRect);

		DesktopRect.left	-= 1;
		DesktopRect.top		-= 1;
		DesktopRect.bottom	+= 2;
		DesktopRect.right	+= 2;

	    ::AdjustWindowRectEx(&DesktopRect, GetStyle(), TRUE, GetExStyle());

	    // Remember this for OnGetMinMaxInfo()
	    m_FullScreenWindowRect	= DesktopRect	;
        
        WPNew					= m_WPPrev		;
        WPNew.showCmd			= SW_SHOWNORMAL	;
	    WPNew.rcNormalPosition	= DesktopRect	;
    }
    else
	{
        WPNew = m_WPPrev;
	}
    
    SetWindowPlacement(&WPNew);


	// ����߻�..
	CRect	rect;
	GetClientRect( rect );
	SetWindowSize( rect.Width() , rect.Height() );
}

void	CMainFrame::SetWindowSize( int cx , int cy )
{
	static CMenu * __spMenu = GetMenu();

	// �޴� ����
	switch( m_bFullScreen )
	{
	case	MT_VM_NORMAL		:	SetMenu( __spMenu	);	SetWindowText( theApp.m_strTitle	);break;
	case	MT_VM_FULLSCREEN	:	SetMenu( NULL		);	SetWindowText( "AlefClient"			);break;
	case	MT_VM_DUNGEON		:	SetMenu( __spMenu	);	SetWindowText( theApp.m_strTitle	);break;
	default:						SetMenu( __spMenu	);	SetWindowText( theApp.m_strTitle	);break;
	}

	// �޴� ��ȭ�� ���� ũ�� �ٽð��..
	CRect	rect;
	GetClientRect( rect );
	cx = rect.Width();
	cy = rect.Height();

	// ũ�⺸��
	CRect	rectStatus;
	CRect	rectToolbar;

	if( m_pToolBar )
		m_pToolBar->GetClientRect( rectToolbar	);
	else
		rectToolbar.SetRectEmpty();

	// �� ������ ũ�� ����.
	
	CRect	rectMainView;
	CRect	rectMiniMap	;
	CRect	rectTileList;
	CRect	rectOutput	;
	CRect	rectHtml	;

	// ��������! ���� ũ�� ���� �۾��̴�!
	rectToolbar.left	= 0					;
	rectToolbar.top		= 0					;
	rectToolbar.right	= cx				;
	rectToolbar.bottom	= TOOLBAR_HEIGHT	;

	rectMainView.left	= MAP_UI_MARGIN							;
	rectMainView.top	= MAP_UI_MARGIN + rectToolbar.Height()	;
	rectMainView.right	= cx - ( MAP_UI_MARGIN + MAP_UI_MARGIN + TILELIST_WIDTH	+ MAP_UI_MARGIN );
	rectMainView.bottom	= cy - ( MAP_UI_MARGIN + MAP_UI_MARGIN + MINIMAP_WIDTH	+ MAP_UI_MARGIN );

	rectMiniMap.left	= MAP_UI_MARGIN							;
	rectMiniMap.top		= rectMainView.bottom + MAP_UI_MARGIN	;
	rectMiniMap.right	= MAP_UI_MARGIN + MINIMAP_WIDTH			;
	rectMiniMap.bottom	= cy - MAP_UI_MARGIN					;

	rectTileList.left	= rectMainView.right + MAP_UI_MARGIN	;
	rectTileList.top	= MAP_UI_MARGIN	 + rectToolbar.Height()	;
	rectTileList.right	= cx - MAP_UI_MARGIN					;
	rectTileList.bottom	= cy - MAP_UI_MARGIN					;

	rectOutput.left		= rectMiniMap.right + MAP_UI_MARGIN		;
	rectOutput.top		= rectMainView.bottom + MAP_UI_MARGIN	;
	rectOutput.right	= rectMainView.right					;
	rectOutput.bottom	= cy - MAP_UI_MARGIN					;

#ifdef _USE_HTML_WINDOW
	rectHtml.left		= rectOutput.left + rectOutput.Width()/2 + 2;
	rectHtml.top		= rectOutput.top						;
	rectHtml.right		= rectOutput.right						;
	rectHtml.bottom		= rectOutput.bottom						;

	rectOutput.right	= rectHtml.left							- 2 ;
#endif // _USE_HTML_WINDOW

	switch( m_bFullScreen )
	{
	default:
	case	MT_VM_NORMAL		:
		{
			// �����Ѵ�� ������ �̵���Ŵ.
			SetMenu( __spMenu );
			m_wndView.MoveWindow( rectMainView );
			
			if( m_pMiniMap		)
			{
				m_pMiniMap	->ShowWindow( SW_SHOW		);
				m_pMiniMap	->MoveWindow( rectMiniMap	);
			}
			if( m_pOutputWnd	)
			{
				m_pOutputWnd->ShowWindow( SW_SHOW		);
				m_pOutputWnd->MoveWindow( rectOutput	);
			}
			if( m_pTileList		)
			{
				m_pTileList	->ShowWindow( SW_SHOW		);
				m_pTileList	->MoveWindow( rectTileList	);
			}
			if( m_pToolBar		)
			{
				m_pToolBar	->ShowWindow( SW_SHOW		);
				m_pToolBar	->MoveWindow( rectToolbar	); 
			}
			if( m_pHtmlWnd		)
			{
				m_pHtmlWnd	->ShowWindow( SW_SHOW		);
				m_pHtmlWnd	->MoveWindow( rectHtml		); 
			}

			if( m_pDungeonWnd	)
			{
				m_pDungeonWnd		->ShowWindow( SW_HIDE	);
				m_pDungeonToolbar	->ShowWindow( SW_HIDE	);
			}
		}
		break;
	case	MT_VM_FULLSCREEN	:
		{
			rectMainView.left	= 0		;
			rectMainView.top	= 0		;
			rectMainView.right	= cx	;
			rectMainView.bottom	= cy	;
			
			SetMenu( NULL );
			m_wndView.MoveWindow( rectMainView );
			
			if( m_pMiniMap		)
			{
				m_pMiniMap	->ShowWindow( SW_HIDE		);
			}
			if( m_pOutputWnd	)
			{
				m_pOutputWnd->ShowWindow( SW_HIDE		);
			}
			if( m_pTileList		)
			{
				m_pTileList	->ShowWindow( SW_HIDE		);
			}
			if( m_pToolBar		)
			{
				m_pToolBar	->ShowWindow( SW_HIDE		);
			}
			if( m_pHtmlWnd		)
			{
				m_pHtmlWnd	->ShowWindow( SW_HIDE		);
			}
			if( m_pDungeonWnd	)
			{
				m_pDungeonWnd		->ShowWindow( SW_HIDE	);
				m_pDungeonToolbar	->ShowWindow( SW_HIDE	);
			}
		}
		break;
	case	MT_VM_DUNGEON		:
		{
			if( m_pDungeonWnd	)
			{
				CRect	rectDungeonToobar = rectMainView;

				rectDungeonToobar.right	= rectDungeonToobar.left + DTM_TOOL_MENU_WIDTH	;
				rectMainView.left		+= DTM_TOOL_MENU_WIDTH	;

				m_pDungeonWnd		->ShowWindow( SW_SHOW			);
				m_pDungeonWnd		->MoveWindow( rectMainView		); 
				m_pDungeonToolbar	->ShowWindow( SW_SHOW			);
				m_pDungeonToolbar	->MoveWindow( rectDungeonToobar	);
			}
			
			if( m_pOutputWnd	)
			{
				m_pOutputWnd->ShowWindow( SW_SHOW		);
				m_pOutputWnd->MoveWindow( rectOutput	);
			}
			
			if( m_pToolBar		)
			{
				m_pToolBar	->ShowWindow( SW_SHOW		);
				m_pToolBar	->MoveWindow( rectToolbar	); 
			}

			if( m_pMiniMap		)
			{
				m_pMiniMap	->ShowWindow( SW_SHOW		);
				m_pMiniMap	->MoveWindow( rectMiniMap	);
			}

			if( m_pTileList		)
			{
				CRect	rectView = rectTileList;
				rectView.bottom = rectView.top + 200;
				m_wndView.MoveWindow( rectView );

				CRect	rectTile = rectTileList;
				rectTile.top = rectView.bottom + MAP_UI_MARGIN;
				m_pTileList	->ShowWindow( SW_SHOW		);
				m_pTileList	->MoveWindow( rectTile		);
			}
			if( m_pHtmlWnd		)
			{
				m_pHtmlWnd	->ShowWindow( SW_HIDE		);
			}
		}
		break;

	}
	
}

void CMainFrame::OnModechangeGeometryReverse() 
{
	m_pTileList->ChangeTab( EDITMODE_GEOMETRY		, TRUE );
}

void CMainFrame::OnModechangeTileReverse() 
{
	m_pTileList->ChangeTab( EDITMODE_TILE			, TRUE );	
}

void CMainFrame::OnViewSkyTemplate() 
{
	// �켱 ���� �ʱ�ȭ �ѹ� �����ֽð�...
	int	x,z;
	ApWorldSector	* pSector;
	for( z = ALEF_LOAD_RANGE_Y1 ; z <= ( INT32 ) ALEF_LOAD_RANGE_Y2 ; ++z )
	{
		for( x = ALEF_LOAD_RANGE_X1 ; x <= ( INT32 ) ALEF_LOAD_RANGE_X2 ; ++x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );
			if( pSector )
			{
				pSector->DeleteIndexArray( ApWorldSector::AWS_SKYOBJECT );
			}
		}
	}

	// �ٽ� ���� �߰� ����...

	INT32					lIndex			=	0	;
	ApdObject			*	pstApdObject			;

	// ��ϵ� ��� Object�� ���ؼ�...
	for (	pstApdObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
			pstApdObject														;
			pstApdObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		// ���� �ݹ� �ѹ澿 ������..
		AgcmEventNature::CBOnAddObject(
			( PVOID ) pstApdObject			,
			( PVOID ) g_pcsAgcmEventNature	,
			NULL							);
	}
			
	CSkyTemplatePreviewDlg dlg;
	if( IDOK == dlg.DoModal() )
	{
		//;;
	}
	
}

void CMainFrame::OnViewWaterdlg() 
{
	/*
	int		res = g_pcsAgcmWaterDlg->OpenWaterDlg(0);
	if(res)
	{
		g_pcsAgcmWater->ChangeStatus(0);
	}
	*/
}

void CMainFrame::OnTimeFaster() 
{
	// ���..
	UINT8	rate = g_pcsAgpmEventNature->GetSpeedRate();
	switch( rate )
	{
	case 0		:	rate	=	64	;	break;
	case 64		:	rate	=	128;	break;
	case 128	:	rate	=	255;	break;
	case 255	:	rate	=	255;	break;
	default:
		rate = 64;
		break;
	}

	g_pcsAgpmEventNature->SetSpeedRate( rate );	
}

void CMainFrame::OnTimeSlower() 
{
	// ���..
	UINT8	rate = g_pcsAgpmEventNature->GetSpeedRate();
	switch( rate )
	{
	case 0		:	rate	=	0;	break;
	case 64		:	rate	=	0;	break;
	case 128	:	rate	=	64;	break;
	case 255	:	rate	=	128;	break;
	default:
		rate = 0;
		break;
	}

	g_pcsAgpmEventNature->SetSpeedRate( rate );	
	
}

void CMainFrame::OnPrelightAdjust() 
{
	CPrelightAdjustDlg	dlg;

	if( m_pTileList )
	{
		m_pTileList->GetClientRect	( dlg.m_Rect );
		m_pTileList->ClientToScreen	( dlg.m_Rect );
	}

	if( IDOK == dlg.DoModal() )
	{
		if( dlg.m_nGeometry != 100 )
		{
			g_MainWindow.m_UndoManager.StartActionBlock( CUndoManager::VERTEXCOLOR		);

			//AGCMMAP_THIS->SetPreLightForAllGeometry( ( FLOAT ) ( dlg.m_nGeometry ) / 100.0f );
			FLOAT	fValue = ( FLOAT ) ( dlg.m_nGeometry ) / 100.0f;

			int x , z;
			int	x1 , x2 , z1 , z2;	// ���ȹ���..

			// MAP_DEFAULT_DEPTH ������ �߶�...
			x1 =AGCMMAP_THIS->GetLoadRangeX1() -AGCMMAP_THIS->GetLoadRangeX1() % MAP_DEFAULT_DEPTH				;
			z1 =AGCMMAP_THIS->GetLoadRangeZ1() -AGCMMAP_THIS->GetLoadRangeZ1() % MAP_DEFAULT_DEPTH				;
			x2 =AGCMMAP_THIS->GetLoadRangeX2() + ( MAP_DEFAULT_DEPTH -AGCMMAP_THIS->GetLoadRangeX2() % MAP_DEFAULT_DEPTH ) % MAP_DEFAULT_DEPTH	;
			z2 =AGCMMAP_THIS->GetLoadRangeZ2() + ( MAP_DEFAULT_DEPTH -AGCMMAP_THIS->GetLoadRangeZ2() % MAP_DEFAULT_DEPTH ) % MAP_DEFAULT_DEPTH	;

			ApWorldSector * pSector					;
			int				nSegmentZ , nSegmentX	;
			ApDetailSegment	* pSegment				;
			RwRGBA			rwRGBA					;
			UINT32			r,g,b					;

			static	char	strMessage[] = "SetPreLightForAllGeometry";
			int		nTarget		=	( x2 - x1 + 1 )		*
									( z2 - z1 + 1 )		;
			int		nCurrent	= 1						;

			for( x = x1 ; x < x2 ; x ++ )
			{
				for( z = z1 ; z < z2 ; z ++ )
				{
					pSector			= g_pcsApmMap->GetSectorByArray( x , 0 , z );

					//if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );
					
					if( pSector )
					{
					
						for( nSegmentZ = 0 ; nSegmentZ < pSector->D_GetDepth() ; nSegmentZ ++ )
						{
							for( nSegmentX = 0 ; nSegmentX < pSector->D_GetDepth() ; nSegmentX ++ )
							{
								pSegment = pSector->D_GetSegment( nSegmentX , nSegmentZ );

								if( pSegment )
								{
									// Segment Į����..
									r				= ( UINT32 ) ( ( ( FLOAT ) pSegment->vertexcolor.red	) * fValue );
									g				= ( UINT32 ) ( ( ( FLOAT ) pSegment->vertexcolor.green	) * fValue );
									b				= ( UINT32 ) ( ( ( FLOAT ) pSegment->vertexcolor.blue	) * fValue );

									if( r > 255 ) r = 255;
									if( g > 255 ) g = 255;
									if( b > 255 ) b = 255;

									rwRGBA.red		= ( UINT8 ) r;
									rwRGBA.green	= ( UINT8 ) g;
									rwRGBA.blue		= ( UINT8 ) b;
									rwRGBA.alpha	= pSegment->vertexcolor.alpha;

									g_MainWindow.m_UndoManager.AddVertexColorActionunit( pSector , nSegmentX , nSegmentZ , RwRGBAToApRGBA( rwRGBA ) );
			
									AGCMMAP_THIS->D_SetValue(
										pSector						,
										pSector->GetCurrentDetail()	,
										nSegmentX					,
										nSegmentZ					,
										RwRGBAToApRGBA( rwRGBA )	);

								}
								/////////////////////////////////////
							}
						}// ���׸�Ʈ ����


					}


				}
			}
			
			g_MainWindow.UnlockSectors();
			g_MainWindow.m_UndoManager.EndActionBlock();

		}

		if( dlg.m_nObject != 100 )
		{
			g_pcsAgcmObject->SetPreLightForAllObject( ( FLOAT ) ( dlg.m_nObject ) / 100.0f );
		}

		if( dlg.m_nCharacter != 100 )
		{
			g_pcsAgcmCharacter->SetPreLightForAllCharacter	( ( FLOAT ) ( dlg.m_nCharacter ) / 100.0f);
			g_pcsAgcmItem->		SetPreLightForAllItem		( ( FLOAT ) ( dlg.m_nCharacter ) / 100.0f);
		}

		DisplayMessage( "Prelightó����.." );
	}
}

void CMainFrame::OnEditCreateCollision() 
{
	// ������ (2003-12-04 ���� 6:04:53) : �׽�Ʈ~

	static BOOL	bFlag = TRUE;

	if( bFlag )
	{
		DisplayMessage( "�ݸ�������� ����" );
		AGCMMAP_THIS->CreateAllCollisionAtomic();
	}
	else
	{
		DisplayMessage( RGB( 255 , 124, 42 ) , "�ݸ�������� ����" );
		AGCMMAP_THIS->DestroyAllCollisionAtomic();
	}

	bFlag = !bFlag;
	
}

void CMainFrame::OnClientFpsView() 
{
	// Client FPS View
	// ������ (2003-12-10 ���� 5:44:08) : âũ�� 1024*768�� ����
	// Drag & drop ���� UI �̹��� �������..
}

void CMainFrame::OnSkyValueChange() 
{
// ������ (2004-01-09 ���� 5:13:45) : ��ī�� ���ð� ����..
	CSkyValueChangeDlg	dlg;

	// �����.
	FLOAT	fSkyHeight				=	AgcmEventNature::SKY_HEIGHT			;
	FLOAT	fSkyRange				=	AgcmEventNature::SKY_RADIUS			;
	FLOAT	fCircumstanceHeight		=	AgcmEventNature::CIRCUMSTANCE_HEIGHT;
	FLOAT	fCircumstanceRange		=	AgcmEventNature::CIRCUMSTANCE_RADIUS;

	dlg.m_fSkyHeight				=	fSkyHeight				;
	dlg.m_fSkyRadius				=	fSkyRange				;
	dlg.m_fCircumstanceHeight		=	fCircumstanceHeight		;
	dlg.m_fCircumstanceRadius		=	fCircumstanceRange		;

	if( IDOK == dlg.DoModal() )
	{
		AgcmEventNature::SKY_HEIGHT				=	dlg.m_fSkyHeight			;
		AgcmEventNature::SKY_RADIUS				=	dlg.m_fSkyRadius			;
		AgcmEventNature::CIRCUMSTANCE_HEIGHT	=	dlg.m_fCircumstanceHeight	;
		AgcmEventNature::CIRCUMSTANCE_RADIUS	=	dlg.m_fCircumstanceRadius	;
	}
	else
	{
		// ������� �������´�.
		AgcmEventNature::SKY_HEIGHT				=	fSkyHeight			;
		AgcmEventNature::SKY_RADIUS				=	fSkyRange			;
		AgcmEventNature::CIRCUMSTANCE_HEIGHT	=	fCircumstanceHeight	;
		AgcmEventNature::CIRCUMSTANCE_RADIUS	=	fCircumstanceRange	;
	}
	
	g_pcsAgcmEventNature->MoveSky();
}

static INT32 __nCloudDetail = 2 ;
void CMainFrame::OnTestAdd() 
{
	// TODO: Add your command handler code here
	TRACE( "+\n" );


	__nCloudDetail ++;
	g_pcsAgcmEventNature->SetCloudZoomRate( ( FLOAT ) __nCloudDetail );
	g_pcsAgcmEventNature->ApplySkySetting();
	g_pcsAgcmEventNature->MoveSky();

	DisplayMessage( "���� �ؽ��� Ÿ�ϸ� %dx%d�� ����" , __nCloudDetail , __nCloudDetail );
}

void CMainFrame::OnTestSub() 
{
	TRACE( "-\n" );

	if( __nCloudDetail > 1 )
	{
		__nCloudDetail --;
		g_pcsAgcmEventNature->SetCloudZoomRate( ( FLOAT ) __nCloudDetail );
		g_pcsAgcmEventNature->ApplySkySetting();
		g_pcsAgcmEventNature->MoveSky();

		DisplayMessage( "���� �ؽ��� Ÿ�ϸ� %dx%d�� ����" , __nCloudDetail );
	}
	else
	{
		TRACE( "��!\n" );
	}
}

void CMainFrame::OnViewProjection() 
{
	// TODO: Add your command handler code here

	CRect	rect;
	m_wndView.GetClientRect( rect );

	switch( RwCameraGetProjection( g_MyEngine.m_pCamera ) )
	{
	case rwPARALLEL		:
		{
			// �䷹�� ī�޶� ����.
			RwCameraSetProjection( g_MyEngine.m_pCamera , rwPERSPECTIVE );

			RwV2d	vWindow;
			vWindow.x	= DEFAULT_VIEWWINDOW;
			vWindow.y	= vWindow.x * ((RwReal)rect.Height()/(RwReal)rect.Width());
			RwCameraSetViewWindow( g_MyEngine.m_pCamera , & vWindow );
		}
		break;
	case rwPERSPECTIVE	:
		{
			// �䷹�� ī�޶� ����.
			RwCameraSetProjection( g_MyEngine.m_pCamera , rwPARALLEL );

			RwV2d	vWindow;
			vWindow.y	= MAP_SECTOR_WIDTH * 8;
			vWindow.x	= vWindow.y * ( (RwReal) rect.Width() / (RwReal)rect.Height() );
			RwCameraSetViewWindow( g_MyEngine.m_pCamera , & vWindow );
		
			// ��ġ����..
			if( g_pMainFrame->m_Document.IsInFirstPersonViewMode() ) return;
			{
				RwFrame		*pFrame = RwCameraGetFrame	( g_pEngine->m_pCamera	);

 				ASSERT( NULL != pFrame					);

				FLOAT	fHeight	= ALEF_SECTOR_WIDTH * 2.0f;

				ApWorldSector * pSector =
						g_pcsApmMap->GetSectorByArray( 
													ALEF_LOAD_RANGE_X1		+ ( ALEF_LOAD_RANGE_X2 - ALEF_LOAD_RANGE_X1 ) / 2	,
													0 ,
													( ALEF_LOAD_RANGE_Y2 - ALEF_LOAD_RANGE_Y1 ) / 2 + ALEF_LOAD_RANGE_Y1		);

				
				RwV3d		pos = { 
									pSector->GetXStart() ,
									fHeight,
									pSector->GetZStart() , };

				RwV3d		at = {
									pSector->GetXStart() ,
									0.0f ,
									pSector->GetZStart() };
				RwV3d		up = {
									0.0f ,
									0.0f ,
									-1.0f };
				RwV3d		right = {
									1.0f ,
									0.0f ,
									0.0f };

				// ������ (2003-11-25 ���� 1:01:12) : ī�޶� ��������������..
				fHeight = pSector->D_GetHeight( pos.x , pos.z );
				if( fHeight > pos.y ) pos.y = fHeight + ALEF_SECTOR_WIDTH / 10.0f;

				RwV3dSub		( &at , &pos	, &at	);
				RwV3dScale		( &at , &at		, -1.0f	);
				RwV3dNormalize	( &at , &at				);

				RwMatrix	*	pMatrix = RwFrameGetMatrix( pFrame );

				pMatrix->pos	= pos;
				pMatrix->at		= at;
				pMatrix->up		= up;
				pMatrix->right	= right;

				RwMatrixUpdate			( pMatrix						);
				RwFrameUpdateObjects	( pFrame						);
				//RwCameraSetFrame		( g_pEngine->m_pCamera , pFrame	);

				RwMatrix	*m;
				//RwV3d xaxis	= { 1.0f , 0.0f , 0.0f };
				RwV3d		yaxis	= { 0.0f , 1.0f , 0.0f };
				
				m = RwFrameGetMatrix( pFrame );
				ASSERT( NULL != m );

//				RwV3dCrossProduct	( &m->right	, &yaxis	, &m->at	);
//				m->right.y = 0.0f;
				RwV3dNormalize		( &m->right	, &m->right	);
				RwV3dCrossProduct	( &m->up	, &m->at	, &m->right	);
				RwV3dNormalize		( &m->up	, &m->up	);
				RwV3dCrossProduct	( &m->right	, &m->up	, &m->at	);

				RwMatrixOrthoNormalize( m , m );

				AuPOS	posCamera;
				RwV3d	*pCameraPos = RwMatrixGetPos( m );
				ASSERT( NULL != pCameraPos );

				posCamera.x	= pCameraPos->x;
				posCamera.y	= pCameraPos->y;
				posCamera.z	= pCameraPos->z;

				if( posCamera.y > 10000.0f / 1.2f )
					g_pcsAgcmEventNature->SetSkyHeight( posCamera.y * 1.2f );
				else
					g_pcsAgcmEventNature->SetSkyHeight( 10000.0f			);

				g_pcsAgcmEventNature->SetCharacterPosition( posCamera );
			}

		}
		break;
	}
}

BOOL	__DivisionMinimapExportCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	//FLOAT	fFogDistanceBackup;

	char * pDestinationDirectory = NULL;
	
	if( pData )
		pDestinationDirectory = ( char * ) pData;

	if( pDestinationDirectory )
	{
		char strFullPath[ 1024 ];

		wsprintf( strFullPath , "%s\\Minimap" , pDestinationDirectory );
		CreateDirectory( pDestinationDirectory	, NULL );
		CreateDirectory( strFullPath			, NULL );
	}
	else
	{
		_CreateDirectory( "%s\\Map\\Minimap" , ALEF_CURRENT_DIRECTORY );
	}

	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();

	// �̴ϸ� �ͽ���Ʈ.

	FLOAT	fMaxHeight	= 0.0f;
	{
		int x , z;
		ApWorldSector	* pSector	;
		int nSegmentX , nSegmentZ;
		FLOAT	fHeight;

		for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
		{
			for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
			{
				pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

				if( NULL == pSector ) continue;

				for( nSegmentZ = 0	; nSegmentZ < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) + 1 ; ++ nSegmentZ )
				{
					for( nSegmentX = 0	; nSegmentX < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) + 1 ; ++ nSegmentX )
					{
						fHeight =AGCMMAP_THIS->GetHeight( 
							pSector->GetXStart()	+ ( FLOAT ) nSegmentX * MAP_STEPSIZE ,
							pSector->GetZStart()	+ ( FLOAT ) nSegmentZ * MAP_STEPSIZE , SECTOR_MAX_HEIGHT );

						if( fMaxHeight < fHeight )
						{
							fMaxHeight = fHeight;
						}
					}
				}

				////////////////////////////////////////////////////////////
			}
		}
	}

	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	
	BOOL	bPrevObject	= g_MyEngine.IsRenderObject();

	TRACE( "%d�����\n" , pDivisionInfo->nIndex );
	RwImage	*	pImage = NULL;
	VERIFY( pImage = RwImageCreate( MINIMAP_DETAIL, MINIMAP_DETAIL, 32 ) );

	if( !pImage )
		return FALSE;
	RwImageAllocatePixels( pImage );
	char	filename[ 1024 ] = {0,};

	switch( g_MyEngine.GetSubCameraType() )
	{
	case MyEngine::SC_MINIMAP	:
		{
			// ������Ʈ �ִ°�.
			g_MyEngine.MoveSubCameraToCenter( g_pcsApmMap->GetSectorByArray( pDivisionInfo->nX , 0 , pDivisionInfo->nZ ) , TRUE , fMaxHeight );
			g_MyEngine.SetRenderObject( TRUE );
			g_MyEngine.OnRender();

			PROCESSIDLE();
			PROCESSIDLE();
			PROCESSIDLE();

			VERIFY( RwImageSetFromRaster( pImage , RwCameraGetRaster( g_MyEngine.GetParallelCamera() ) ) );

			if( pDestinationDirectory )
			{
				wsprintf( filename , "%s\\Minimap\\MP%04d.bmp" , pDestinationDirectory , pDivisionInfo->nIndex );
			}
			else
			{
				wsprintf( filename , "%s\\Map\\Minimap\\MP%04d.bmp" , ALEF_CURRENT_DIRECTORY , pDivisionInfo->nIndex );
			}

			if( RwImageWrite( pImage , filename ) )
			{
				DisplayMessage( "'%s' ���Ϸ� �̴ϸ� ����߾��" , filename );
			}
			else
			{
				RwError	error;
				RwErrorGet( & error );
			}
		}
		break;
	case MyEngine::SC_MINIMAPX4	:
		{
			for( int i = 0 ; i < 16 ; i ++ ) 
			{
				// ������Ʈ �ִ°�.
				g_MyEngine.MoveSubCameraToCenter( g_pcsApmMap->GetSectorByArray( pDivisionInfo->nX , 0 , pDivisionInfo->nZ ) , TRUE , fMaxHeight , i );
				g_MyEngine.SetRenderObject( TRUE );
				g_MyEngine.OnRender();

				PROCESSIDLE();
				PROCESSIDLE();
				PROCESSIDLE();

				VERIFY( RwImageSetFromRaster( pImage , RwCameraGetRaster( g_MyEngine.GetParallelCamera() ) ) );

				if( pDestinationDirectory )
				{
					wsprintf( filename , "%s\\Minimap\\MP%04d_%d%d.bmp" , pDestinationDirectory , pDivisionInfo->nIndex , i % 4, i / 4 );
				}
				else
				{
					wsprintf( filename , "%s\\Map\\Minimap\\MP%04d_%d%d.bmp" , ALEF_CURRENT_DIRECTORY , pDivisionInfo->nIndex , i % 4, i / 4 );
				}

				if( RwImageWrite( pImage , filename ) )
				{
					DisplayMessage( "'%s' ���Ϸ� �̴ϸ� ����߾��" , filename );
				}
				else
				{
					RwError	error;
					RwErrorGet( & error );
				}

				AcuFrameMemory::Clear();
			}
		}
		break;
	}

	/*
	// ������ (2005-05-20 ���� 6:44:21) : 
	// ������ �����ִ°� �Ⱦ��°� ���Ƽ� �ּ�..

	g_MyEngine.SetRenderObject( FALSE );
	g_MyEngine.OnRender();
	g_MyEngine.SetRenderObject( bPrevObject );

	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();

	VERIFY( RwImageSetFromRaster( pImage , RwCameraGetRaster( g_MyEngine.GetParallelCamera() ) ) );

	if( pDestinationDirectory )
	{
		wsprintf( filename , "%s\\Minimap\\MG%04d.bmp" , pDestinationDirectory , pDivisionInfo->nIndex );
	}
	else
	{
		wsprintf( filename , "%s\\Map\\Minimap\\MG%04d.bmp" , ALEF_CURRENT_DIRECTORY , pDivisionInfo->nIndex );
	}

	if( RwImageWrite( pImage , filename ) )
	{
		DisplayMessage( "'%s' ���Ϸ� �̴ϸ� ����߾��" , filename );
	}
	else
	{
		RwError	error;
		RwErrorGet( & error );
	}
	*/

	if( pImage )
	{
		RwImageDestroy	( pImage );
		pImage = 0;
	}

	return TRUE;
}

void	CMainFrame::OnFileMinimapexportSmallSize()
{
	// ���� ��� ���� ���׸İ� ĸ�� �ϱ� ���Ѱ�.
	INT32 nCameraType = g_MyEngine.SC_MINIMAP;
	INT32	nPrevType	= g_MyEngine.GetSubCameraType();
	g_MyEngine.SetSubCameraType( nCameraType );

	{
		DivisionMinimapExportSetting setting;
		if(setting.result_)
			AGCMMAP_THIS->EnumLoadedDivision( __DivisionMinimapExportCallback , NULL );
	}

	g_MyEngine.SetSubCameraType( nPrevType );
	g_MyEngine.MoveSubCameraToCenter();
}

void CMainFrame::OnFileMinimapexport() 
{
	INT32 nCameraType;
	switch( MessageBox( "���ػ� �̴ϸ� ĸ�ĸ� ����ҷ���?" , "�̴ϸ��ͽ���Ʈ" , MB_YESNOCANCEL ) )
	{
	case IDYES	:	nCameraType = g_MyEngine.SC_MINIMAPX4	; break;
	case IDNO	:	nCameraType = g_MyEngine.SC_MINIMAP		; break;
	default:
		return;
	}

	INT32	nPrevType	= g_MyEngine.GetSubCameraType();
	g_MyEngine.SetSubCameraType( nCameraType );

	{
		DivisionMinimapExportSetting setting;
		if( setting.result_ )
			AGCMMAP_THIS->EnumLoadedDivision( __DivisionMinimapExportCallback , NULL );
	}

	g_MyEngine.SetSubCameraType( nPrevType );
	g_MyEngine.MoveSubCameraToCenter();
}

BOOL	__DivisionRoughmapExportCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	if( !pDivisionInfo )
		return FALSE;

	AcuFrameMemory::Clear();

	RwRect			rectMap	;

	rectMap.x	= 0;
	rectMap.y	= 0;
	rectMap.w	= ROUGHMAP_DETAIL;
	rectMap.h	= ROUGHMAP_DETAIL;
	
	TRACE( "%d�����\n" , pDivisionInfo->nIndex );
	RwImage	*	pImage = NULL;
	VERIFY( pImage = RwImageCreate( rectMap.w , rectMap.h , 32 ) );
	RwImageAllocatePixels( pImage );

	int				x , z	;
	ApWorldSector *	pSector	;
	RwUInt32		uFlagsBackup	;

	char			strDir		[ 256 ];
	char			strFullPath [ 1024];
	char			filename	[ 256 ];

	strncpy( strDir ,AGCMMAP_THIS->m_strToolImagePath , 256 );

	for( z = pDivisionInfo->nZ ; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX ; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			pSector = g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( pSector )
			{
				g_MyEngine.MoveSubCameraToCenter( pSector );

				rsDWSectorInfo stInfo;
				AGCMMAP_THIS->GetDWSector( pSector , SECTOR_HIGHDETAIL , & stInfo );

				if( !stInfo.pDWSector || !stInfo.pDWSector->atomic || !stInfo.pDWSector->geometry )
					continue;
				uFlagsBackup = RpGeometryGetFlags( stInfo.pDWSector->geometry );

				// �������� ����..
				RpGeometrySetFlags( stInfo.pDWSector->geometry , uFlagsBackup & ~( rpGEOMETRYPRELIT | rpGEOMETRYNORMALS ) );

				// ���ý�Į�� Ǯ�� �����ؼ� , ����Ʈ ȿ���� ����..
				VERIFY( RpGeometryLock( stInfo.pDWSector->geometry , rpGEOMETRYLOCKALL ) );
				RwRGBA * pPreLitLum				= RpGeometryGetPreLightColors	( stInfo.pDWSector->geometry		);

				RwRGBA * rgbVertexColorBackup	= 0;

				if( !pPreLitLum ) 
				{
					RpGeometryUnlock( stInfo.pDWSector->geometry );
					continue;
				}
				
				rgbVertexColorBackup	= new RwRGBA[ stInfo.pDWSector->geometry->numVertices ];
				size_t backUpSize = sizeof( RwRGBA ) * stInfo.pDWSector->geometry->numVertices;
				// ����
				memcpy_s( ( void * ) rgbVertexColorBackup, backUpSize, ( void * ) pPreLitLum, backUpSize );

				for( int i = 0 ; i < stInfo.pDWSector->geometry->numVertices ; ++i )
				{
					(*( pPreLitLum + i )).red	= 255;
					(*( pPreLitLum + i )).green	= 255;
					(*( pPreLitLum + i )).blue	= 255;
					(*( pPreLitLum + i )).alpha	= 255;
				}
				RpGeometryUnlock( stInfo.pDWSector->geometry );

				// ��Ƽ���� Į�� Ǯ�μ���..
				{
					// ������ (2004-04-27 ���� 7:11:12) : 
					// ��Ƽ���� Į�� Ǯ�� �������� ������ ���� �ؽ��� �̹��� ���� ��Ӱ� ���´�..

					int						j			;
					RpMaterial	*			pMaterial	;
					RwSurfaceProperties		surfProp	;

					surfProp.ambient	= 1.0f;
					surfProp.diffuse	= 1.0f;
					surfProp.specular	= 1.0f;

					for( j = 0 ; j < RpGeometryGetNumMaterials( stInfo.pDWSector->geometry ) ; ++j )
					{
						pMaterial = RpGeometryGetMaterial( stInfo.pDWSector->geometry , j );
						if( pMaterial )
						{
							RpMaterialSetSurfaceProperties( pMaterial , &surfProp );
						}
						else
						{
							TRACE( "%s , �� ���ö� -_-;;\n" , __FILE__ );
						}
					}
				}

				// ����..
				VERIFY( g_MyEngine.RenderAtomicOnly( stInfo.pDWSector->atomic ) );

				// ������� ��������..
				RpGeometrySetFlags( stInfo.pDWSector->geometry , uFlagsBackup );
				VERIFY( RpGeometryLock( stInfo.pDWSector->geometry , rpGEOMETRYLOCKALL ) );
				memcpy( ( void * ) pPreLitLum , ( void * ) rgbVertexColorBackup , ( sizeof RwRGBA ) * stInfo.pDWSector->geometry->numVertices );
				RpGeometryUnlock( stInfo.pDWSector->geometry );

				// ��Ƽ���� Į�� ������� ��������..
				{
					// ������ (2004-04-27 ���� 7:11:12) : 
					// ��Ƽ���� Į�� Ǯ�� �������� ������ ���� �ؽ��� �̹��� ���� ��Ӱ� ���´�..

					int						j			;
					RpMaterial	*			pMaterial	;
					RwSurfaceProperties		surfProp	;

					surfProp.ambient	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_AMBIENT	;
					surfProp.diffuse	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_DIFFUSE	;
					surfProp.specular	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_SPECULAR;

					for( j = 0 ; j < RpGeometryGetNumMaterials( stInfo.pDWSector->geometry ) ; ++j )
					{
						pMaterial = RpGeometryGetMaterial( stInfo.pDWSector->geometry , j );
						if( pMaterial )
						{
							RpMaterialSetSurfaceProperties( pMaterial , &surfProp );
						}
						else
						{
							TRACE( "%s , �� ���ö� -_-;;\n" , __FILE__ );
						}
					}
				}

				delete [] rgbVertexColorBackup;

				//g_MyEngine.RenderSubCameraGeometryOnly();

//				RpAtomicCallBackRender	pFunc;
//				pFunc = AcuObject::GetAtomicRenderCallBack( stInfo.pDWSector->atomic );
//				pFunc( stInfo.pDWSector->atomic );


				/*
				VERIFY( RwImageSetFromRaster( pImage , RwCameraGetRaster( g_MyEngine.GetParallelCamera() ) ) );

				wsprintf( filename , SECTOR_ROUGH_TEXTURE , x , z );
				wsprintf( strFullPath , "%s\\World\\Rough\\%s" , strDir , filename );

				if( RwImageWrite( pImage , strFullPath ) )
				{
					DisplayMessage( RGB( 25 , 124 , 255 ) , "'%s' ���Ϸ� �̴ϸ� ����߾�� ( %3d/%03d )" ,
						filename , 1 + ( x - pDivisionInfo->nX ) + ( z - pDivisionInfo->nZ ) * pDivisionInfo->nDepth	,
						pDivisionInfo->nDepth * pDivisionInfo->nDepth );
				}
				else
				{
					RwError	error;
					RwErrorGet( & error );
				}
				*/

				RwTexture	* pTexture	;
				RwRaster	* pRaster	;
				RwRaster	* pCameraRaster = RwCameraGetRaster( g_MyEngine.GetParallelCamera() );

				ASSERT( NULL != pCameraRaster );

				pRaster = RwRasterCreate(	RwRasterGetWidth( pCameraRaster ) , RwRasterGetHeight( pCameraRaster ) ,
											RwRasterGetFormat( pCameraRaster ) , rwRASTERTYPECAMERATEXTURE  );

				ASSERT( NULL != pRaster );

				// �������..
				RwRasterPushContext( pRaster );
				RwRasterRender( pCameraRaster , 0 , 0 );
				RwRasterPopContext();

				VERIFY( pTexture = RwTextureCreate( pRaster ) );

				wsprintf( filename , SECTOR_ROUGH_TEXTURE , x , z );
				//wsprintf( strFullPath , "%s\\World\\Rough\\%s.dds" , strDir , filename );
				wsprintf( strFullPath , "%s\\Map\\Temp\\Rough\\%s.dds" , strDir , filename );

				if( AcuTexture::RwD3D9DDSTextureWrite( pTexture , strFullPath , D3DFMT_DXT1 ) )
				{
					DisplayMessage( RGB( 25 , 124 , 255 ) , "'%s' ���Ϸ� �̴ϸ� ����߾�� ( %3d/%03d )" ,
						filename , 1 + ( x - pDivisionInfo->nX ) + ( z - pDivisionInfo->nZ ) * pDivisionInfo->nDepth	,
						pDivisionInfo->nDepth * pDivisionInfo->nDepth );
				}
				else
				{
					RwError	error;
					RwErrorGet( & error );
				}

				if( pTexture	) RwTextureDestroy	( pTexture	);

				// �����ʹ� �ؽ��Ŀ� �Բ� ���ĵ�..���� ���� �ʿ����.
				// if( pRaster		) RwRasterDestroy	( pRaster	);
			}
			else
			{
				TRACE( "���� ���ö�!!!!\n" );
			}
		}
	}

	if( pImage ) 
		RwImageDestroy	( pImage	);
	return TRUE;
}

void CMainFrame::RoughTextureExport() 
{
	DisplayMessage( RGB( 255 , 0 , 0 ) , "���� �ؽ��� ���ʷ����� ����~" );

	INT32	nPrevType	= g_MyEngine.GetSubCameraType();

	g_MyEngine.SetSubCameraType( g_MyEngine.SC_ROUGH );

	// ������ �ͽ���Ʈ
	RwRaster *		pRaster	;
	RwRaster *		pZRaster;
	RwRect			rectMap	;

	rectMap.x	= 0;
	rectMap.y	= 0;
	rectMap.w	= ROUGHMAP_DETAIL;
	rectMap.h	= ROUGHMAP_DETAIL;

	VERIFY( pRaster		= RwRasterCreate(rectMap.w , rectMap.h , 0 , rwRASTERTYPECAMERA		) );
	VERIFY( pZRaster	= RwRasterCreate(rectMap.w , rectMap.h , 0 , rwRASTERTYPEZBUFFER	) );

	RwCameraSetRaster	( g_MyEngine.GetParallelCamera() , pRaster	);
	RwCameraSetZRaster	( g_MyEngine.GetParallelCamera() , pZRaster	);


	// ���� ����.
	BOOL	bPreFog;
	RwRenderStateGet		( rwRENDERSTATEFOGENABLE, (void *) &bPreFog	);
	RwRenderStateSet		( rwRENDERSTATEFOGENABLE, (void *) FALSE	);
	
	// ����Ʈ ������ ���⼭ ..
//	RpWorldRemoveLight		( g_MyEngine.m_pWorld	, g_MyEngine.m_pLightAmbient	);
//	RpWorldRemoveLight		( g_MyEngine.m_pWorld	, g_MyEngine.m_pLightDirect		);

	RwRGBAReal		rgbColorBackup , rgbColorSetup;
	rgbColorBackup = *RpLightGetColor( g_MyEngine.m_pLightAmbient );

	rgbColorSetup.alpha	= 1.0f;
	rgbColorSetup.red	= 1.0f;
	rgbColorSetup.green	= 1.0f;
	rgbColorSetup.blue	= 1.0f;

	RpLightSetColor( g_MyEngine.m_pLightAmbient , &rgbColorSetup );

	RwUInt32 prevShaderMode	= RpMTextureEnableVertexShader( 0 );
	RpMTextureSetBlendMode( D3DTOP_MODULATE );
	RpMTextureEnableLinearFog( FALSE );

	BOOL	bPrevObject	= g_MyEngine.IsRenderObject();
	g_MyEngine.SetRenderObject( FALSE );
	AGCMMAP_THIS->EnumLoadedDivision( __DivisionRoughmapExportCallback , ( PVOID ) this );
	g_MyEngine.SetRenderObject( bPrevObject );

	// ��������..
	RpMTextureEnableVertexShader( prevShaderMode );
	RpMTextureSetBlendMode		( D3DTOP_MODULATE4X );
	RpMTextureEnableLinearFog	( TRUE );

	RpLightSetColor( g_MyEngine.m_pLightAmbient , &rgbColorBackup );
	g_pcsAgcmRender->LightValueUpdate();

	// ���� ���� �������.
	RwRenderStateSet		( rwRENDERSTATEFOGENABLE, (void *) bPreFog	);

	RwCameraSetRaster	( g_MyEngine.GetParallelCamera() , g_MyEngine.m_pSubCameraMainCameraSubRaster	);
	RwCameraSetZRaster	( g_MyEngine.GetParallelCamera() , g_MyEngine.m_pSubCameraMainCameraSubZRaster	);

	RwRasterDestroy	( pRaster	);
	RwRasterDestroy	( pZRaster	);

	g_MyEngine.SetSubCameraType( nPrevType );
	g_MyEngine.MoveSubCameraToCenter();

	DisplayMessage( RGB( 255 , 0 , 0 ) , "���� �ؽ��� �ͽ���Ʈ ��" );
}

void CMainFrame::OnFileClosetempfile() 
{
	DisplayMessage( "������ �������� ����.." );
	CFileFind	ff;
	CString		str;

	str.Format( "%s\\World\\Rough\\*.*" );

	int			nCount = 0;
	if( ff.FindFile( str ) )
	{
		do
		{
			nCount++;
			DeleteFile( ff.GetFilePath() );
		}while( ff.FindNextFile() );
	}

	DisplayMessage( "%d�� ������ �����߽�." , nCount );
	
}

void CMainFrame::OnFileObjectExportForServer() 
{
	// ���� ������Ʈ ������ ��� �о�鿩��
	// �ϳ��� INI���Ϸ� �����Ѵ�.
	// �̰��� �������� ����Ѵ�.
	ASSERT(!"���� ���� �ȵž�!");

	// �ϴ� ������Ʈ ��� ����..
	

}

void CMainFrame::OnFileDdsConvert() 
{
	CDDSConvertDlg	dlg;
	if( IDOK == dlg.DoModal() )
	{
		// ������..
	}
}

BOOL	CMainFrame::AddConsoleCommand( INT32 nIndex , char *pAlias1 , char *pAlias2 , char *pAlias3 , INT32 nArgument , char * pHelp , BOOL bHide )
{
	// �ߺ��˻�..
	AuNode< CConsoleCommand > * pNode = m_listCommand.GetHeadNode();
	CConsoleCommand * pCommand;
	while( pNode )
	{
		pCommand = &pNode->GetData();

		// �ߺ��ϴ°͸� �˻�..

		if( pCommand->nCommandIndex == nIndex )
		{
			TRACE( "�ߺ� Ŀ�ǵ� �߰�.\n");
			return FALSE;
		}

		m_listCommand.GetNext( pNode );
	}

	CConsoleCommand	newCommand;

	newCommand.nCommandIndex	= nIndex	;
	newCommand.strAlias[ 0 ]	= pAlias1	;
	newCommand.strAlias[ 1 ]	= pAlias2	;
	newCommand.strAlias[ 2 ]	= pAlias3	;
	newCommand.nArgumentCount	= nArgument	;
	newCommand.strHelp			= pHelp		;
	newCommand.bHide			= bHide		;

	// ���� .
	m_listCommand.AddTail( newCommand );

	return TRUE;
}

void CMainFrame::OnCleanupUnusedTexture() 
{
	// TODO: Add your command handler code here
	int	nCount = AGCMMAP_THIS->m_TextureList.CleanUpUnusedTile();
	DisplayMessage( "������ �ʴ� %d���� ������ �����߾��~" , nCount );
}

void CMainFrame::OnViewAtomicfilter() 
{
	CAtomicFilterDlg	dlg;
	
	UINT32	uPrev	= m_Document.m_uAtomicFilter;

	// �� ����
	if( uPrev & CAlefMapDocument::AF_GEOMTERY			)	dlg.m_bGeometry			= TRUE;
	if( uPrev & CAlefMapDocument::AF_SYSTEMOBJECT		)	dlg.m_bSystemObject		= TRUE;
	if( uPrev & CAlefMapDocument::AF_OBJECTALL			)	dlg.m_bObjectAll		= TRUE;
	if( uPrev & CAlefMapDocument::AF_OBJECT_RIDABLE		)	dlg.m_bRidableObject	= TRUE;
	if( uPrev & CAlefMapDocument::AF_OBJECT_BLOCKING	)	dlg.m_bBlockingObject	= TRUE;
	if( uPrev & CAlefMapDocument::AF_OBJECT_EVENT		)	dlg.m_bEventFilter		= TRUE;
	if( uPrev & CAlefMapDocument::AF_OBJECT_GRASS		)	dlg.m_bGrass			= TRUE;
	if( uPrev & CAlefMapDocument::AF_OBJECT_OTHERS		)	dlg.m_bObjectOthers		= TRUE;

	if( dlg.DoModal() )
	{
		UINT32	nNewFlag = 0;
		if( dlg.m_bGeometry			)	nNewFlag |= CAlefMapDocument::AF_GEOMTERY			;
		if( dlg.m_bSystemObject		)	nNewFlag |= CAlefMapDocument::AF_SYSTEMOBJECT		;
		if( dlg.m_bObjectAll		)	nNewFlag |= CAlefMapDocument::AF_OBJECTALL			;
		if( dlg.m_bRidableObject	)	nNewFlag |= CAlefMapDocument::AF_OBJECT_RIDABLE		;
		if( dlg.m_bBlockingObject	)	nNewFlag |= CAlefMapDocument::AF_OBJECT_BLOCKING	;
		if( dlg.m_bEventFilter		)	nNewFlag |= CAlefMapDocument::AF_OBJECT_EVENT		;
		if( dlg.m_bGrass			)	nNewFlag |= CAlefMapDocument::AF_OBJECT_GRASS		;
		if( dlg.m_bObjectOthers		)	nNewFlag |= CAlefMapDocument::AF_OBJECT_OTHERS		;

		m_Document.m_uFilterEventID	= dlg.m_nSelectedEventID;
		m_Document.m_uAtomicFilter	= nNewFlag				;
/*
		// ���� ��ġ..
		if( nNewFlag == CAlefMapDocument::AF_SHOWALL )
		{
			// ������ (2004-06-01 ���� 2:09:49) : ����� �׸��� �� ���� �ݹ� ����.
			VERIFY( g_pcsAgcmRender->SetPreRenderAtomicCallback( NULL , NULL ) );
		}
		else
		{
			if( g_pcsAgcmRender->SetPreRenderAtomicCallback( CBPreRenderAtomicCallback , NULL ) )
			{
				// ���� ��ġ �Ϸ�..
			}
			else
			{
				MessageBox( "������ ���������� ������� �ʾҽ��ϴ�." );
			}
		}
*/
	}
}

void CMainFrame::OnEventnatureFilter() 
{
	if( !g_pcsAgcmEventNature->IsMaptoolBuild() )
	{
		MessageBox( "AgcmEventNature�� ���������� ���� ���� �ʾƼ� ����� �۵����� �ʽ��ϴ�." );
		return;
	}

	CEventNatureChangeFilterDlg	dlg;

	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGELIGHTCOLOR	) dlg.m_bLightColor		= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGELIGHTPOSITION	) dlg.m_bLightPosition	= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGESKYCOLOR		) dlg.m_bSkyColor		= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGEFOG			) dlg.m_bFog			= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGECLOUD			) dlg.m_bCloud			= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGECIRCUMSTANCE	) dlg.m_bCircumstance	= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGEEFFECT		) dlg.m_bAmbience		= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTCHANGEBGM			) dlg.m_bBgm			= TRUE;
	if( g_pcsAgcmEventNature->GetMaptoolDebugInfoFlag() & AgcmEventNature::SKY_DONOTTIMECHANGE			) dlg.m_bByTime			= TRUE;

	if( dlg.DoModal() )
	{
		UINT32	uFlag = 0 ; //AgcmEventNature::SKY_DONOTMOVE;

		if( dlg.m_bLightColor		) uFlag |= AgcmEventNature::SKY_DONOTCHANGELIGHTCOLOR	;
		if( dlg.m_bLightPosition	) uFlag |= AgcmEventNature::SKY_DONOTCHANGELIGHTPOSITION;
		if( dlg.m_bSkyColor			) uFlag |= AgcmEventNature::SKY_DONOTCHANGESKYCOLOR		;
		if( dlg.m_bFog				) uFlag |= AgcmEventNature::SKY_DONOTCHANGEFOG			;
		if( dlg.m_bCloud			) uFlag |= AgcmEventNature::SKY_DONOTCHANGECLOUD		;
		if( dlg.m_bCircumstance		) uFlag |= AgcmEventNature::SKY_DONOTCHANGECIRCUMSTANCE	;
		if( dlg.m_bAmbience			) uFlag |= AgcmEventNature::SKY_DONOTCHANGEEFFECT		;
		if( dlg.m_bBgm				) uFlag |= AgcmEventNature::SKY_DONOTCHANGEBGM			;
		if( dlg.m_bByTime			) uFlag |= AgcmEventNature::SKY_DONOTTIMECHANGE			;

		g_pcsAgcmEventNature->SetMaptoolDebugInfoFlag( uFlag );
		DisplayMessage( "������ �ý��ۿ� ����� ������ �����Ͽ����ϴ�. ");
	}
	
}

BOOL	__DivisionCollisionUpdateCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	int x , z;
	ApWorldSector	* pSector	;
	//int nSegmentX , nSegmentZ;
	//FLOAT	fHeight;

	CProgressDlg dlg;
	dlg				.StartProgress	( "����" , 256 , g_pMainFrame			);

	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			dlg.SetProgress( x - pDivisionInfo->nX + ( z - pDivisionInfo->nZ ) * pDivisionInfo->nDepth );
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			rsDWSectorInfo						dwSectorInfo	;
			AGCMMAP_THIS->GetDWSector( pSector ,  SECTOR_HIGHDETAIL , & dwSectorInfo	);

			if( NULL == dwSectorInfo.pDWSector				) continue;
			if( NULL == dwSectorInfo.pDWSector->geometry	) continue;

			RpCollisionGeometryBuildData( dwSectorInfo.pDWSector->geometry , NULL );
			
			RpMorphTarget * pDWMorph = RpGeometryGetMorphTarget( dwSectorInfo.pDWSector->geometry , 0 );
			RwSphere	sphere;
			RpMorphTargetCalcBoundingSphere	( pDWMorph , &sphere );
			RpMorphTargetSetBoundingSphere	( pDWMorph , &sphere );

			dwSectorInfo.pDWSector->atomic->boundingSphere		= sphere;
			dwSectorInfo.pDWSector->atomic->worldBoundingSphere	= sphere;
		}
	}

	dlg.EndProgress();
	
	return TRUE;
}
void CMainFrame::OnEditUpdatecollision() 
{
	// Collision Update

	DisplayMessage( "�ݸ��� ������Ʈ ��~" );
	AGCMMAP_THIS->EnumLoadedDivision( __DivisionCollisionUpdateCallback , ( PVOID ) this );
	DisplayMessage( "�ݸ��� ������Ʈ ��~" );
}

void CMainFrame::OnModeDungeonToggle() 
{
	// TODO: Add your command handler code here
	TRACE( "�������\n" );

	switch( m_bFullScreen )
	{
	case	MT_VM_NORMAL		:	
	case	MT_VM_FULLSCREEN	:	
		{
			m_bFullScreen	= MT_VM_DUNGEON;
			// �̴ϸ� �Կ�
			this->m_pDungeonWnd->UpdateDungeonPreviewBitmap();
			// ī�޶� ã��

			// ȭ���� �߾�..
			CRect	rect;
			this->m_wndView.GetClientRect( rect );

			RwV3d	posGeometry;
			if ( AGCMMAP_THIS->GetMapPositionFromMousePosition_tool( rect.Width() / 2 , rect.Height() / 2 ,  &posGeometry.x, &posGeometry.y, &posGeometry.z ) )
			{
				// do no op
			}
			else
			{
				// ȭ�� ��� ������ �����Ƿ� ī�޶� ��ġ�� �̿���.
				RwFrame * pFrame = RwCameraGetFrame( g_pEngine->m_pCamera );
				posGeometry = pFrame->ltm.pos;
			}

			// ���� ������ ��ġ �̵�.
			//
			this->m_pDungeonWnd->MoveToPosition( & posGeometry );
		}
		break;
	case	MT_VM_DUNGEON		:	
	default:
		m_bFullScreen	= MT_VM_NORMAL;
		break;
	}

	// ����߻�..
	CRect	rect;
	GetClientRect( rect );
	SetWindowSize( rect.Width() , rect.Height() );
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if( MT_VM_FULLSCREEN == m_bFullScreen )
	{
		lpMMI->ptMaxSize.y		= m_FullScreenWindowRect.Height()	;
		lpMMI->ptMaxTrackSize.y	= lpMMI->ptMaxSize.y				;
		lpMMI->ptMaxSize.x		= m_FullScreenWindowRect.Width()	;
		lpMMI->ptMaxTrackSize.x	= lpMMI->ptMaxSize.x				;
	}
		
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnCameraPositionSave()
{
	TRACE( "OnCameraPositionSave\n" );

	// ī�޶� ���� ����..
	// �ش� ����Ʈ�� �̵�..
	RwFrame		*	pFrame	= RwCameraGetFrame( g_pEngine->m_pCamera );
	RwMatrix	*	pMatrix = RwFrameGetMatrix( pFrame );

	CString szFileName;
	CFileDialog dlg(FALSE,_T("CPF"),_T("*.CPF"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("ī�޶� ������ ���� (*.CPF)|*.CPF|�˴� (*.*)|*.*|"));
	
	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	if(dlg.DoModal()==IDOK)
	{
		SetCurrentDirectory( strDir );

		szFileName=dlg.GetPathName();

		// ����..

		FILE	* pFile = fopen( ( LPCTSTR ) szFileName , "wt" );
		if( NULL == pFile )
		{
			MessageBox( "���� ���½���~ �����ؼ� üũ�Ͻð� �ٽ� �õ��ϼ�" );
			return;
		}
		else
		{
			fprintf( pFile ,
				"%f %f %f %u\n" , 
				pMatrix->right.x	,
				pMatrix->right.y	,
				pMatrix->right.z	,
				pMatrix->flags		);
			fprintf( pFile ,
				"%f %f %f %u\n" , 
				pMatrix->up.x	,
				pMatrix->up.y	,
				pMatrix->up.z	,
				pMatrix->pad1		);
			fprintf( pFile ,
				"%f %f %f %u\n" , 
				pMatrix->at.x	,
				pMatrix->at.y	,
				pMatrix->at.z	,
				pMatrix->pad2		);
			fprintf( pFile ,
				"%f %f %f %u\n" , 
				pMatrix->pos.x	,
				pMatrix->pos.y	,
				pMatrix->pos.z	,
				pMatrix->pad3		);

			fclose( pFile );
			return;
		}
	}
	else
	{
		SetCurrentDirectory( strDir );
		return;
	}


}

void CMainFrame::OnCameraPositionLoad()
{
	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	CFileDialog dlg(TRUE,_T("CPF"),_T("*.CPF"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("ī�޶� ������ ���� (*.CPF)|*.CPF|�˴� (*.*)|*.*|"));

	if (dlg.DoModal() == IDOK)
	{
		char filename[ 1024 ];
		strncpy( filename , (LPCTSTR) dlg.GetPathName() , 1024 );
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
		_splitpath( filename , drive, dir, fname, ext );

		wsprintf( dir , "%s%s" , drive, dir );

		SetCurrentDirectory( dir );

		wsprintf( filename , "%s%s" , fname , ext );

		// Load..

		FILE	* pFile = fopen( filename , "rt" );
		
		SetCurrentDirectory( strDir );
		if( NULL == pFile )
		{
			MessageBox( "���� ���½���~ �����ؼ� üũ�Ͻð� �ٽ� �õ��ϼ�" );
			return;
		}
		else
		{

			RwMatrix		matRead;
			RwFrame		*	pFrame	= RwCameraGetFrame( g_pEngine->m_pCamera );
			RwMatrix	*	pMatrix = RwFrameGetMatrix( pFrame );

			fscanf( pFile ,
				"%f %f %f %u\n" , 
				&matRead.right.x	,
				&matRead.right.y	,
				&matRead.right.z	,
				&matRead.flags		);
			fscanf( pFile ,
				"%f %f %f %u\n" , 
				&matRead.up.x	,
				&matRead.up.y	,
				&matRead.up.z	,
				&matRead.pad1		);
			fscanf( pFile ,
				"%f %f %f %u\n" , 
				&matRead.at.x	,
				&matRead.at.y	,
				&matRead.at.z	,
				&matRead.pad2		);
			fscanf( pFile ,
				"%f %f %f %u\n" , 
				&matRead.pos.x	,
				&matRead.pos.y	,
				&matRead.pos.z	,
				&matRead.pad3		);

			fclose( pFile );

			// ī�޶� ����Ʈ�� �̵���Ŵ..
			RwFrameTransform( pFrame, &matRead, rwCOMBINEREPLACE );
			return;
		}
	}
	else
	{
		SetCurrentDirectory( strDir );
		return;
	}
}


BOOL	__DivisionGetLefTTopDivisionIndexCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	INT32 * pDivisionIndex = ( INT32 * ) pData;
	*pDivisionIndex = pDivisionInfo->nIndex;
	return FALSE;
}

INT32	GetLeftTopDivisionIndex()
{
	if( NULL == AGCMMAP_THIS ) return 0;

	INT32	nDivisionIdnex = 0;
	AGCMMAP_THIS->EnumLoadedDivision( __DivisionGetLefTTopDivisionIndexCallback , ( PVOID ) &nDivisionIdnex );

	return nDivisionIdnex;
}

void CMainFrame::OnFindobject()
{
	CFindObjectSettingDlg	dlg;

	if( IDOK == dlg.DoModal() )
	{
		// ������ (2005-03-28 ���� 4:18:49) : �ʱ�ȭ..
		m_nLastSearchObject		= 0;
		m_nFindObjectEventIndex	= dlg.m_nEventID;
	}
}

void CMainFrame::OnFindobjectnext()
{
	// ������ (2005-03-28 ���� 3:33:50) : 
	// ����Ʈ ������Ʈ ã�� !
	{
		INT32			lIndex = 0		;
		ApdObject	*	pcsObject		;
		ApdEvent	*	pEvent			;

		BOOL	bFound = FALSE;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject												;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{	
			pEvent = g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) m_nFindObjectEventIndex );

			if( pEvent && pcsObject->m_lID > m_nLastSearchObject )
			{
				bFound = TRUE;
				m_nLastSearchObject = pcsObject->m_lID;
				// �̳��̴�!

				// �̵�!
				g_MainWindow.MoveTo( pcsObject->m_stPosition.x , pcsObject->m_stPosition.z );
				break;
			}
		}

		if( bFound )
		{
			// do nothing..
		}
		else
		{
			MessageBox( "�ѹ��� ���Ұų� �ƿ� �����!" );
			m_nLastSearchObject = 0;
		}
	}
}

void CMainFrame::OnObjectpopGroupsave()
{
	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	CString szFileName;
	CFileDialog dlg(FALSE,_T("OGF"),_T("*.OGF"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Object Group File (*.OGF)|*.OGF|�˴� (*.*)|*.*|"));
	if (dlg.DoModal() == IDOK)
	{
		SetCurrentDirectory( strDir );

		szFileName=dlg.GetPathName();

		// ����..

		FILE	* pFile = fopen( ( LPCTSTR ) szFileName , "wb" );
		if( NULL == pFile )
		{
			MessageBox( "���� ���½���~ �����ؼ� üũ�Ͻð� �ٽ� �õ��ϼ�" );
			return;
		}
		else
		{
			// ���� ���
			INT32	nVersion = OBJECT_GROUP_FILE_VERSION;
			fwrite( ( void * ) &nVersion , sizeof INT32 , 1 , pFile );

			AuNode< MainWindow::ObjectSelectStruct >	*	pNode	;
			ApdObject		*	pstApdObject	;
			int					oid		;
			RpClump *			pClump	;
			INT32				nType	;

			// ������ ����..
			RwV3d	center;
			RwV3d	vDelta;
			g_MainWindow.GetObjectCenter( & center );
			
			pNode	= g_MainWindow.m_listObjectSelected.GetHeadNode();

			while( pNode )
			{
				pClump	= pNode->GetData().pClump;

				{
					oid = 0;
					// Object ID�� ��.
					nType = AcuObject::GetClumpType( pClump, &oid );

					switch( AcuObject::GetType( nType ) )
					{
					case	ACUOBJECT_TYPE_OBJECT			:
						break;
					default:
						oid = 0;
						break;
					}
					
					// �ش� ������Ʈ�� �����͸� ��.
					pstApdObject	= g_pcsApmObject->	GetObject		( oid			);
					if( pstApdObject ) 
					{
						// �����Ұ�
						// Template ID
						// Delta Position
						// Scale
						// Rotation

						vDelta.x	= pstApdObject->m_stPosition.x - g_MainWindow.m_Position.x;
						vDelta.y	= pstApdObject->m_stPosition.y - g_MainWindow.m_Position.y;
						vDelta.z	= pstApdObject->m_stPosition.z - g_MainWindow.m_Position.z;

						fwrite( ( void * ) &pstApdObject->m_pcsTemplate->m_lID , sizeof INT32 , 1 , pFile );
						fwrite( ( void * ) &vDelta , sizeof RwV3d , 1 , pFile );
						fwrite( ( void * ) &pstApdObject->m_stScale , sizeof AuPOS , 1 , pFile );
						fwrite( ( void * ) &pstApdObject->m_fDegreeX , sizeof FLOAT , 1 , pFile );
						fwrite( ( void * ) &pstApdObject->m_fDegreeY , sizeof FLOAT , 1 , pFile );
					}
				}

				g_MainWindow.m_listObjectSelected.GetNext( pNode );
			}

			fclose( pFile );
			return;
		}
	}
	else
	{
		SetCurrentDirectory( strDir );
		return;
	}	

}

void CMainFrame::OnObjectpopGroupLoad( RwV3d * pPos )
{
	RwV3d	pos = * pPos;

	char	strDir[ 1024 ];
	GetCurrentDirectory( 1024 , strDir );

	CFileDialog dlg(TRUE,_T("OGF"),_T("*.OGF"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					_T("Object Group File (*.OGF)|*.OGF|�˴� (*.*)|*.*|"));
	if (dlg.DoModal() == IDOK)
	{
		char filename[ 1024 ];
		strncpy( filename , (LPCTSTR) dlg.GetPathName() , 1024 );
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
		_splitpath( filename , drive, dir, fname, ext );

		wsprintf( dir , "%s%s" , drive, dir );

		SetCurrentDirectory( dir );

		wsprintf( filename , "%s%s" , fname , ext );

		// Load..

		FILE	* pFile = fopen( filename , "rb" );
		
		SetCurrentDirectory( strDir );
		if( NULL == pFile )
		{
			MessageBox( "���� ���½���~ �����ؼ� üũ�Ͻð� �ٽ� �õ��ϼ�" );
			return;
		}
		else
		{
			// ������ (2005-04-11 ���� 11:38:00) : ����Ȯ��..
			INT32	nVersion;
			fread( ( void * ) &nVersion , sizeof INT32 , 1 , pFile );
			ASSERT( nVersion == OBJECT_GROUP_FILE_VERSION );

			INT32	nTID	;
			RwV3d	vDelta	;
			AuPOS	stScale	;
			FLOAT	fDegreeX;
			FLOAT	fDegreeY;

			INT32	nSizeRead = 0;

			do
			{
				nSizeRead = 0;
				nSizeRead += fread( ( void * ) &nTID , sizeof INT32 , 1 , pFile );
				nSizeRead += fread( ( void * ) &vDelta , sizeof RwV3d , 1 , pFile );
				nSizeRead += fread( ( void * ) &stScale , sizeof AuPOS , 1 , pFile );
				nSizeRead += fread( ( void * ) &fDegreeX , sizeof FLOAT , 1 , pFile );
				nSizeRead += fread( ( void * ) &fDegreeY , sizeof FLOAT , 1 , pFile );

				if( nSizeRead == 5 )
				{
					// ������Ʈ �߰�..
					ApdObject		*	pstNewApdObject	;
					AgcdObject		*	pstNewAgcdObject;

					vDelta.x	+= g_MainWindow.m_Position.x;
					vDelta.y	+= g_MainWindow.m_Position.y;
					vDelta.z	+= g_MainWindow.m_Position.z;

					pstNewApdObject = g_pcsApmObject->AddObject( 
						g_pcsApmObject->GetEmptyIndex( GetDivisionIndexF( vDelta.x , vDelta.z ) ) , nTID );
					ASSERT( NULL != pstNewApdObject );
					if( !pstNewApdObject )
					{
						MD_SetErrorMessage( "g_pcsApmObject->AddObject ������Ʈ �߰�����.." );
						continue;
					}

					// ������Ʈ ��ŷ �߰�..
					AuBLOCKING			stBlockInfoCalcurated[APDOBJECT_MAX_BLOCK_INFO];		// Blocking Info
					pstNewApdObject->CalcWorldBlockInfo( pstNewApdObject->m_pcsTemplate->m_nBlockInfo , pstNewApdObject->m_pcsTemplate->m_astBlockInfo , stBlockInfoCalcurated );
					g_pcsApmMap->AddObjectBlocking( stBlockInfoCalcurated , pstNewApdObject->m_nBlockInfo );

					pstNewAgcdObject = g_pcsAgcmObject->GetObjectData(pstNewApdObject);

					ASSERT( pstNewAgcdObject != NULL );
					if( !pstNewAgcdObject )
					{
						MD_SetErrorMessage( "g_pcsAgcmObject->GetObjectData ������Ʈ Ŭ���̾�Ʈ ���� ��� ����" );
						continue;
					}

					// �÷��� ����..
					pstNewAgcdObject->m_lObjectType = ACUOBJECT_TYPE_OBJECT;

					pstNewApdObject->m_stScale		= stScale	;
					pstNewApdObject->m_stPosition.x	= vDelta.x	;
					pstNewApdObject->m_stPosition.y	= vDelta.y	;
					pstNewApdObject->m_stPosition.z	= vDelta.z	;
					pstNewApdObject->m_fDegreeX		= fDegreeX	;
					pstNewApdObject->m_fDegreeY		= fDegreeY	;

					if( g_pcsApmObject->UpdateInit( pstNewApdObject ) )
					{

					}
					else
					{
						MD_SetErrorMessage( "g_pcsApmObject->UpdateInit Ŭ���� ���� ����" );
						continue;
					}
				}
				else
				{
					break;
				}
			}while(1);
		}
	}
	else
	{
		// do nothing
	}
}

void CMainFrame::OnSelectAll()
{
	switch( m_Document.m_nSelectedMode )
	{
	case EDITMODE_OBJECT		:
		{
			// ȭ�鿡 ���̴� ������Ʈ �� ����.
			CRect	rect;
			this->m_wndView.GetClientRect( rect );
			g_MainWindow.SetObjectSelection( rect.TopLeft() , rect.BottomRight() );

			g_MainWindow.m_listObjectSelected.RemoveAll();
			g_MainWindow.m_listObjectSelected = g_MainWindow.m_listObjectSelecting;
			g_MainWindow.m_listObjectSelecting.RemoveAll();
		}
		break;
	default:
		break;
	}
}

void	CMainFrame::CleanUp()
{
	// ������ (2005-04-27 ���� 12:09:34) : 
	// ����Ÿ �α׸� ����..
	g_MainWindow.m_pCurrentGeometry	= NULL;
	g_MainWindow.SetObjectSelection( NULL );
	g_MainWindow.m_UndoManager.ClearAllUndoData();

	g_pcsApmMap->SetAutoLoadData( TRUE );
	g_pcsApmMap->DeleteAllSector	(		);
	g_pcsApmMap->FlushDeleteSectors();

	g_pcsAgcmGrass->RemoveAll();
	g_pcsAgcmWater->RemoveAll();
	g_pcsApmOcTree->DestroyAll();

	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();
	PROCESSIDLE();

	g_pcsApmObject->DeleteAllObject	( TRUE	);
	g_pcsApmObject->FlushRemoveList();
	m_pTileList->m_pOthers->CleanUpData();
	PROCESSIDLE();
	g_pcsApmMap->SetAutoLoadData( FALSE );

	if( m_pDungeonWnd )
	{
		m_pDungeonWnd->RemoveAllElement();
	}
}

void CMainFrame::OnViewRoughdisplay()
{
	static	float	fSrcX			= 0.0f	;
	static	float	fSrcZ			= 0.0f	;
	static	float	fSrcHeight		= 0.0f	;
	static	float	fDstX			= 0.0f	;
	static	float	fDstZ			= 0.0f	;
	static	int		nRadius			= 6		;
	static	float	fCloudHeight	= 0.0f	;
	
	CRoughDisplayDlg	dlg;

	RwV3d			*	camPos			= RwMatrixGetPos( RwFrameGetLTM( RwCameraGetFrame( g_pEngine->m_pCamera ) ) );

	INT32 nDiv			= GetDivisionIndexF( camPos->x , camPos->z );

	dlg.m_fDstX			= GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDiv ) ) ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
	dlg.m_fDstZ			= GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDiv ) ) ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;

	dlg.m_fSrcX			= fSrcX			;
	dlg.m_fSrcZ			= fSrcZ			;
	dlg.m_fSrcHeight	= fSrcHeight	;
	dlg.m_nRadius		= nRadius		;
	dlg.m_fCloudHeight	= fCloudHeight	;

	dlg.DoModal();

	fSrcX			= dlg.m_fSrcX			;
	fSrcZ			= dlg.m_fSrcZ			;
	fSrcHeight		= dlg.m_fSrcHeight		;
	fDstX			= dlg.m_fDstX			;
	fDstZ			= dlg.m_fDstZ			;
	fCloudHeight	= dlg.m_fCloudHeight	;
}


BOOL	CMainFrame::ExportServerData( INT32 nDivision ,  char * pDestinationDirectory , ProgressCallback pfCallback , void * pData )
{
	ASSERT( pDestinationDirectory );

	char strPath[ 256 ];
	wsprintf( strPath , "%s\\world" , pDestinationDirectory );
	CreateDirectory( strPath , NULL );
	wsprintf( strPath , "%s\\world\\server" , pDestinationDirectory );
	CreateDirectory( strPath , NULL );

	INT32	nDivisionX1 , nDivisionZ1 , nDivisionX2 , nDivisionZ2;

	nDivisionX1	= GetFirstSectorXInDivision( nDivision );
	nDivisionZ1	= GetFirstSectorZInDivision( nDivision );
	nDivisionX2 = nDivisionX1 + MAP_DEFAULT_DEPTH;
	nDivisionZ2 = nDivisionZ1 + MAP_DEFAULT_DEPTH;

	char str[ 256 ];
	wsprintf( str , SECTOR_SERVERDATA_FILE , nDivision );
	wsprintf( strPath , "%s\\world\\server\\%s" , pDestinationDirectory , str );

	FILE * pFile = fopen( strPath , "wb" );

	// ������ (2005-12-14 ���� 4:38:15) : ���� ���..
	int nVersion = ALEF_SERVER_DATA_VERSION;
	fwrite( ( void * ) &nVersion , sizeof( int ), 1 , pFile );

	int nTotalCount = 0;

	for( int z = nDivisionZ1 ; z < nDivisionZ2 ; z ++ )
	{
		for( int x = nDivisionX1 ; x < nDivisionX2 ; x ++ )
		{
			ApWorldSector * pSector = g_pcsApmMap->GetSectorByArray( x , 0 , z );

			vector< RwV3d >	vectorVertex;
			int	nVectorBlocking			= 0;
			int	nVectorRidable			= 0;
			int	nVectorBlockingRidable	= 0;

			if( pSector )
			{
				// 
				// ���Ϳ� �ִ� ������ ������Ʈ�� �˻��Ͽ�
				// �� ���Ϳ� ���ԵŴ� ������ ������ �и��Ͽ�
				// �����Ѵ�.

				// ���� �����ǵ���Ÿ ( �̰� Height Map���� �ذ� �Ƿ���?.. )
				// �� ������Ʈ �� ���� �ݸ��� ����.
				// �� ���� �ȿ� ���ԵǴ� ��ŷ|���̴��� ������Ʈ�� ����������..
				// �ϴ� ���ܺ��ڰ�..

				// ������Ʈ �˻�..
				vector< INT32 >	vectorObjectListBlocking		;	// ��ŷ�� ����ִ°�
				vector< INT32 >	vectorObjectListRidable			;	// ���̴��� ����ִ°�..
				vector< INT32 >	vectorObjectListBlockingRidable	;	// ���� ���ÿ� ����Ŵ°�..

				// ���Ϳ� ���Ե� ������Ʈ ����.
				{
					int	aArray[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];
					int nCount;

					int nSectorX , nSectorZ;

					for( nSectorZ = 0 ; nSectorZ < MAP_DEFAULT_DEPTH ; nSectorZ++ )
					{
						for( nSectorX = 0 ; nSectorX < MAP_DEFAULT_DEPTH ; nSectorX++ )
						{
							nCount = pSector->GetIndex( ApWorldSector::AWS_COLLISIONOBJECT	, nSectorX , nSectorZ , aArray , SECTOR_MAX_COLLISION_OBJECT_COUNT );

							if( nCount )
							{
								// �ߺ�üũ..

								for( int nIndex = 0 ; nIndex < nCount ; nIndex++ )
								{
									ApdObject			*pcsApdObject = g_pcsApmObject->GetObject( aArray[ nIndex ] );
									ASSERT( pcsApdObject );

									AgcdObject		*pcsAgcdObject	= g_pcsAgcmObject->GetObjectData( pcsApdObject );
									ASSERT( pcsAgcdObject );

									INT32 nType = AcuObject::GetProperty( pcsAgcdObject->m_lObjectType );

									if( nType & ACUOBJECT_TYPE_RIDABLE )
									{
										vector <INT32>::iterator Iter;
										// Ridable�� ��ġ�� ���.
										for ( Iter = vectorObjectListBlockingRidable.begin( ) ; Iter != vectorObjectListBlockingRidable.end( ) ; Iter++ )
										{
											if( (*Iter) == aArray[ nIndex ] )
											{
												break;
											}
										}

										if( Iter != vectorObjectListBlockingRidable.end( ) )
										{
											// �ߺ�
										}
										else
										{
											vectorObjectListBlockingRidable.push_back( aArray[ nIndex ] );
										}
									}
									else
									{
										vector <INT32>::iterator Iter;
										// ��ŷ�� �ִ°��..
										for ( Iter = vectorObjectListBlocking.begin( ) ; Iter != vectorObjectListBlocking.end( ) ; Iter++ )
										{
											if( (*Iter) == aArray[ nIndex ] )
											{
												break;
											}
										}

										if( Iter != vectorObjectListBlocking.end( ) )
										{
											// �ߺ�
										}
										else
										{
											vectorObjectListBlocking.push_back( aArray[ nIndex ] );
										}
									}
								}
							}//if( nCount )

							nCount = pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT	, nSectorX , nSectorZ , aArray , SECTOR_MAX_COLLISION_OBJECT_COUNT );

							if( nCount )
							{
								// �ߺ�üũ..

								for( int nIndex = 0 ; nIndex < nCount ; nIndex++ )
								{
									ApdObject			*pcsApdObject = g_pcsApmObject->GetObject( aArray[ nIndex ] );
									ASSERT( pcsApdObject );

									AgcdObject		*pcsAgcdObject	= g_pcsAgcmObject->GetObjectData( pcsApdObject );
									ASSERT( pcsAgcdObject );

									INT32 nType = AcuObject::GetProperty( pcsAgcdObject->m_lObjectType );

									if( nType & ACUOBJECT_TYPE_BLOCKING )
									{
										vector <INT32>::iterator Iter;
										// Blocking�� ��ġ�� ���.
										for ( Iter = vectorObjectListBlockingRidable.begin( ) ; Iter != vectorObjectListBlockingRidable.end( ) ; Iter++ )
										{
											if( (*Iter) == aArray[ nIndex ] )
											{
												break;
											}
										}

										if( Iter != vectorObjectListBlockingRidable.end( ) )
										{
											// �ߺ�
										}
										else
										{
											vectorObjectListBlockingRidable.push_back( aArray[ nIndex ] );
										}
									}
									else
									{
										vector <INT32>::iterator Iter;
										// ���̴��� �ִ°��..
										for ( Iter = vectorObjectListRidable.begin( ) ; Iter != vectorObjectListRidable.end( ) ; Iter++ )
										{
											if( (*Iter) == aArray[ nIndex ] )
											{
												break;
											}
										}

										if( Iter != vectorObjectListRidable.end( ) )
										{
											// �ߺ�
										}
										else
										{
											vectorObjectListRidable.push_back( aArray[ nIndex ] );
										}
									}
								}
							}//if( nCount )
						}//for( nSectorX = 0 ; nSectorX < MAP_DEFAULT_DEPTH ; nSectorX++ )
					}//for( nSectorZ = 0 ; nSectorZ < MAP_DEFAULT_DEPTH ; nSectorZ++ )
				}

				{//
					// �� ������Ʈ�� ���Ͽ� �ݸ��� ������Ʈ ������..
					// ����Ÿ ����.
					nVectorBlocking			= CollectVertex( pSector , &vectorVertex , &vectorObjectListBlocking		 );
					nVectorBlockingRidable	= CollectVertex( pSector , &vectorVertex , &vectorObjectListBlockingRidable	 );
					nVectorRidable			= CollectVertex( pSector , &vectorVertex , &vectorObjectListRidable			 );					
				}//

			}

			// ���Ͽ� ���
			int nCount = vectorVertex.size();

			nTotalCount += nCount;

			fwrite( ( void * ) &nCount					, sizeof( int ), 1 , pFile );
			fwrite( ( void * ) &nVectorBlocking			, sizeof( int ), 1 , pFile );
			fwrite( ( void * ) &nVectorBlockingRidable	, sizeof( int ), 1 , pFile );
			fwrite( ( void * ) &nVectorRidable			, sizeof( int ), 1 , pFile );

			for( int i = 0; i < nCount ; i ++ )
			{
				fwrite( ( void * ) &vectorVertex[ i ] , sizeof RwV3d , 1 , pFile );
			}
		}// For
	}// For

	fclose( pFile );

	DisplayMessage( RGB( 255 , 0 , 0 ) , "��������Ÿ ���ý� ���� %d��" , nTotalCount );
	return TRUE;
}

int		CMainFrame::CollectVertex( ApWorldSector * pSector , vector< RwV3d > * pVertexVector , vector< INT32 > * pObjectList )
{
	int nCount = 0;

	ASSERT( pObjectList );

	if( !pObjectList )
		return nCount;

	
	vector <INT32>::iterator Iter;
	for ( Iter = pObjectList->begin( ) ; Iter != pObjectList->end( ) ; ++Iter )
	{
		ApdObject * pcsApdObject = g_pcsApmObject->GetObject( *Iter );

		if( pcsApdObject )
		{
			AgcdObject * pcsAgcdObject = g_pcsAgcmObject->GetObjectData( pcsApdObject );

			ASSERT( pcsAgcdObject );
			ASSERT( pcsAgcdObject->m_pstCollisionAtomic );

			if( pcsAgcdObject->m_pstCollisionAtomic )
			{
				RwV3d v3dTri[ 3 ];

				RpAtomic * pAtomic = pcsAgcdObject->m_pstCollisionAtomic;
				RpGeometry * pGeometry = RpAtomicGetGeometry( pAtomic );

				ASSERT( pGeometry );
				if(!pGeometry)
					continue;


				RpMorphTarget * pMorphTarget = RpGeometryGetMorphTarget( pGeometry , 0 );

				ASSERT( pMorphTarget );
				if( !pMorphTarget )
					continue;

				RwFrame	* pFrame = RpAtomicGetFrame( pAtomic );

				if( !pFrame )
					continue;


				RwMatrix * pMatrix = RwFrameGetLTM( pFrame );

				if( !pMatrix )
					continue;


				RpTriangle * pTriangle = RpGeometryGetTriangles( pGeometry );

				if( !pTriangle )
					continue;


				int triangleNum = RpGeometryGetNumTriangles( pGeometry );

				for( int i=0; i < triangleNum; ++i )
				{
					// 3���� �ϳ��� �� ���;ȿ� ���Եž� �ֳ�?...
					// �ִ� ��� �� ������ ��ä�� ���Ϳ� �߰���

					RwV3dTransformPoint( &v3dTri[ 0 ] , &pMorphTarget->verts[ ( pTriangle + i )->vertIndex[ 0 ] ] , pMatrix );
					RwV3dTransformPoint( &v3dTri[ 1 ] , &pMorphTarget->verts[ ( pTriangle + i )->vertIndex[ 1 ] ] , pMatrix );
					RwV3dTransformPoint( &v3dTri[ 2 ] , &pMorphTarget->verts[ ( pTriangle + i )->vertIndex[ 2 ] ] , pMatrix );

					int j;
					for( j = 0 ; j < 3 ; j ++ )
					{
						if( pSector->IsInSector(
							v3dTri[ j ].x ,
							v3dTri[ j ].y ,
							v3dTri[ j ].z ) )
						{
							break;
						}
					}

					if( j != 3 )
					{
						ASSERT( pVertexVector );

						if( pVertexVector )
						{
							// ���Ϳ� �߰���.
							pVertexVector->push_back( v3dTri[ 0 ] ); nCount++;
							pVertexVector->push_back( v3dTri[ 1 ] ); nCount++;
							pVertexVector->push_back( v3dTri[ 2 ] ); nCount++;
						}
					}
				}
			}// ������ ����
		}
	}// ������Ʈ ����

	return nCount;
}