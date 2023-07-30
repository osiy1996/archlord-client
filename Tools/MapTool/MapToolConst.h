#pragma once

// �������� �����ϴ� �ܽ���Ʈ ������. 
#include "ConstManager.h"
#include "ApDefine.h"
#include <ApBase.h>

#define	ALEF_LOADING_MODE					( g_Const.m_nGlobalMode					)
#define	ALEF_PREVIEW_MAP_SELECT_SIZE		( g_Const.m_nPreviewMapSelectSize		)
#define	ALEF_PREVIEW_MAP_STEP_SIZE			( 16									)

#define	ALEF_SECTOR_WIDTH					( g_Const.m_fSectorWidth				)	// �Ѽ����� ���μ��� ����.100.0f
#define	ALEF_MAX_HORI_SECTOR				( g_Const.m_nHoriSectorCount			)
#define	ALEF_MAX_VERT_SECTOR				( g_Const.m_nVertSectorCount			)		// ���μ��� ������ �ִ� ��ǥ.
#define	ALEF_SECTOR_DEFAULT_DEPTH			( g_Const.m_nSectorDepth				)		// 

#define	ALEF_CURRENT_DIRECTORY				( g_Const.m_strCurrentDirectory			)

// �ε� ����. Array Coordinate! -_-+
#define	ALEF_LOAD_RANGE_X1					( g_Const.m_nLoading_range_x1			)
#define	ALEF_LOAD_RANGE_Y1					( g_Const.m_nLoading_range_y1			)
#define	ALEF_LOAD_RANGE_X2					( g_Const.m_nLoading_range_x2			)
#define	ALEF_LOAD_RANGE_Y2					( g_Const.m_nLoading_range_y2			)

#define	ALEF_USE_EFFECT						( g_Const.m_bUseEffect					)

// Lock Mode
#define	ALEF_GEOMETRY_LOCK_MODE				( ( RwInt32 ) g_Const.m_nGeometryLockMode		)
#define	ALEF_VERTEXCOLOR_LOCK_MODE			( ( RwInt32 ) g_Const.m_nVertexColorLockMode	)

#define	ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT	1.0f	// ���׸�Ʈ�� ��������� ����Ʈ ����
#define	ALEF_SECTOR_DEFAULT_TILE_INDEX		( 0x00010380 )		// ����Ʈ�� ���� �Ŵ� Ÿ�� ��ȣ
#define	ALEF_SECTOR_DEFAULT_ROUGHTILE_INDEX	( 0x00010200 )		// ����Ʈ�� ���� �Ŵ� Ÿ�� ��ȣ
#define	ALEF_DEFAULT_ALPHA					( 0x000001c0 )		// ����Ʈ �ؽ����� ���� �ε���.
// �����Ͽ� 1/5 �� ó���Ѵ�..
#define	ALEF_SECTOR_ROUGH_DEPTH				( 4 )

#define	ALEF_SECTOR_DUMMY_DEPTH				1

// Ÿ�ϰ���
#define	ALEF_TILE_DO_NOT_USE				( 0x00000000 )

// ���� ���� ���
#define	ALEF_GetFileType( x )				( x >> 16 )
#define	ALEF_GetFileVersion( x )			( x & 0x0000ffff )

#define	ALEF_MAPFILE						0x0001

#define	ALEF_MATERIAL_DEFAULT_COUNT			(5)


#define	KEYFORWARD		'w'
#define	KEYBACKWARD		's'
#define	KEYSTEPLEFT		'a'
#define	KEYSTEPRIGHT	'd'
#define	KEYLIFTUP		VK_SPACE
#define	KEYLIFTDOWN		'z'
#define	KEYROTATECCW	'e'
#define KEYROTATECW		'q'

// ���α׷� �۾� ���� ������.
enum	TILEWINDOWTYPE
{
	EDITMODE_GEOMETRY	,
	EDITMODE_TILE		,
	EDITMODE_OBJECT		,
	EDITMODE_OTHERS		,	//���� ����� �߰�.
	EDITMODE_TYPECOUNT
};

// �۷ι� �����޽��� Į��
// Alef Error Message colors...
#define	AEM_WARNNING	RGB( 180 , 255 , 180	)
#define	AEM_ERROR		RGB( 255 , 0 , 0		)
#define	AEM_NOTIFY		RGB( 38 , 38 , 255		)
#define	AEM_NORMAL		RGB( 38 , 38 , 38		)

// ������ ��Ʈ��
#define	SECTORDETAIL_STRING_EMPTY		"Dummy Detail"
#define	SECTORDETAIL_STRING_LOWDETAIL	"Rough Detail"
#define	SECTORDETAIL_STRING_HIGHDETAIL	"High Detail"

// ������ ǥ�� Į��
#define	SECTORDETAIL_BACK_COLOR			RGB( 40 , 40 , 40 )
#define	SECTORDETAIL_TEXT_COLOR_EMPTY	RGB( 40 , 40 , 255 )
#define	SECTORDETAIL_TEXT_COLOR_LOW		RGB( 40 , 255 , 40 )
#define	SECTORDETAIL_TEXT_COLOR_HIGH	RGB( 255 , 40 , 40 )

#define	DEFAULT_OBJECT_BRUSH_WIDTH		5000


// Ÿ����...
	#define	TILECATEGORY_FILE			"map\\tile\\categorylist.txt"
	#define	TILECATEGORY_ERROR_BITMAP	"errorbmp.bmp"
	#define	TILECATEGORY_WIDTH			75
	#define	TILECATEGORY_HEIGHT			20
	// ���δ� Ÿ�ϼ�.. �̰����� �����.
	#define	TILECATEGORY_COUNTPERLINE	3
	#define	TILECATEGORY_TILEWMARGIN	12
	#define	TILECATEGORY_TILEHMARGIN	2
	#define	TILECATEGORY_TILEWIDTH		50
	#define	TILECATEGORY_TILEHEIGHT		50

	// ���̾� ǥ�� ũ��..
	// ���̾� ����..
	// #define	TILELAYER_HEIGHT			20
	#define	TILELAYER_HEIGHT			0

	#define	TILELAYERSTRING_BACKGROUND	"������"
	#define	TILELAYERSTRING_ALPHATILE	"Alpha Tiling �۾�"
	#define	TILELAYERSTRING_UPPER		"���� �����̹��� �÷� ����"
	#define	TILELAYERCOLOR_BACKTEXT		RGB( 224 , 244 , 50 )
	#define	TILELAYERCOLOR_BACKBACK		RGB( 35 , 23 , 68 )
	#define	TILELAYERCOLOR_ALPHATEXT	RGB( 224 , 100 , 100 )
	#define	TILELAYERCOLOR_ALPHABACK	RGB( 224 , 244 , 50 )
	#define	TILELAYERCOLOR_UPPERTEXT	RGB( 100 , 123 , 200 )
	#define	TILELAYERCOLOR_UPPERBACK	RGB( 154 , 54 , 20 )

	// Color Table
	#define	TILECATEGORY_TILEINFOTEXTCOLOR_USED		RGB( 255	, 255	, 255	)
	#define	TILECATEGORY_TILEINFOTEXTCOLOR_UNUSED	RGB( 255	, 154	, 145	)
	#define	TILECATEGORY_TILEINFOTEXTBKCOLOR		RGB( 0		, 0		, 0		)
	#define TILECATEGORY_BACKGROUNDCOLOR			RGB( 64		, 64	, 64	)
	#define	TILECATEGORY_COLOR_SELECTEDBOX			RGB( 0		, 0		, 0		)
	#define	TILECATEGORY_COLOR_SELECTEDTEXT			RGB( 255	, 128	, 128	)
	#define	TILECATEGORY_COLOR_DEFAULTBOX			RGB( 64		, 64	, 64	)
	#define	TILECATEGORY_COLOR_DEFAULTTEXT			RGB( 200	, 200	, 200	)

	enum	TILELAYER
	{
		TILELAYER_BACKGROUND	,
		TILELAYER_TONGMAP		,
		TILELAYER_ALPHATILEING	,
		TILELAYER_UPPER			,
		TILELAYER_COUNT
	};

// Geometry ����Ʈ �� â.
// UI Geomtry
// �޴��Ʒ��� �귯�� �׸���.
enum UIGM_BRUSH_TYPE
{
	BRUSHTYPE_GEOMETRY		,		// �ö��
	BRUSHTYPE_SMOOTHER		,		// �ݵ�ݵ�
	BRUSHTYPE_VERTEXCOLOR	,		// ���ý�Į��..
	BRUSHTYPE_CHANGENORMAL	,
	BRUSHTYPE_EDGETURN		,
	BRUSHTYPE_COUNT
};

#define	UIGM_COLOR_BACKGROUND	RGB( 35 , 23 , 68 )
#define	UIGM_COLOR_CIRCLE		RGB( 255 , 255 , 255 )
#define	UIGM_COLOR_TEXT			RGB( 231 , 128 , 52 )

#define	UIGM_MAX_CIRCLE_SIZE	( ALEF_SECTOR_WIDTH / 2.0f	)
#define	UIGM_MIN_CIRCLE_SIZE	( ALEF_SECTOR_WIDTH / 40.0f	)
#define	UIGM_DEFAULT_RADIUS		( ALEF_SECTOR_WIDTH / 20.0f	)
#define	UIGM_MENU_HEIGHT		50
#define	UIGM_DEFAULT_BRUSHTYPE	BRUSHTYPE_GEOMETRY

struct	stMenuItem
{
	// �޴� ������ ��Ʈ�� Ÿ��..
	enum MIB_TYPE
	{
		MIB_SELECTED		,
		MIB_UNSELECT		,
		MIB_SELECTEDDOWN	,
		MIB_COUNT
	};

	INT32	nIndex	;
	CString	name	;	// �޴�����
	CRect	rect	;	// ���÷�Ʈ.
	CWnd *	pWnd	;	// �ش� �������� �ڵ�.
	CBitmap	abitmap[ MIB_COUNT ];

	BOOL	bEnable	;

	stMenuItem()
	{
		bEnable	 = TRUE;
	}
};

// ���������쿡�� ����ϴ� ��� ������Ʈ.;
	struct ObjectUndoInfo
	{
		INT32	oid			;	// Object ID
		AuPOS	stScale		;	// Scale Vector
		AuPOS	stPosition	;	// Base Position
		FLOAT	fDegreeX	;	// Rotation Degree
		FLOAT	fDegreeY	;
	};

// ������Ʈ �˾� �Ŵ� �ε��� ��ŸƮ..
	#define	IDM_OBJECT_POPUP_OFFSET	19324

	enum DWSectorSetting
	{
		DWSECTOR_EACHEACH			,
		DWSECTOR_ALLROUGH			,
		DWSECTOR_ALLDETAIL			,
		DWSECTOR_ACCORDINGTOCURSOR	,
		DWSECTOR_SETTING_COUNT
	};



// �ε����
enum LOADINGMODE
{
	LOAD_NORMAL				,
	LOAD_EXPORT_DIVISION	,
	LOAD_EXPORT_LIST		
};

//inline int	GetSectorBlockIndex( int arrayx , int arrayy )
//{
//	// 14�� �Ѷ��� ����..
//	return 1 + arrayx/10 + (arrayy/10) * 14;
//}

inline void	RemoveFullPath( char * fullpath , char * filename)
{
	char			drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath	( fullpath	, drive, dir, fname, ext );

	wsprintf( filename , "%s%s" , fname , ext );
}

enum GRID_MODE
{
	GRIDMODE_NONE			,	// No Grid
	GRIDMODE_D_GETHEIGHT	,	// 
	GRIDMODE_SECTOR_GRID	,
	GRIDMODE_HEIGHT_SECTOR	,	// ������ �Ѵ�.
	GRIDMODE_COUNT
};

inline	FLOAT	frand()
{
	return ( FLOAT ) ( rand() % RAND_MAX ) / ( FLOAT ) RAND_MAX;
}

#define ALEF_BRUSH_LOAD_NORMAL	( g_Const.m_brushLoadNormal )
#define ALEF_BRUSH_LOAD_EXPORT	( g_Const.m_brushLoadExport )

#define ALEF_BRUSH_BACKGROUND	( g_Const.m_nGlobalMode == LOAD_NORMAL ? g_Const.m_brushLoadNormal : g_Const.m_brushLoadExport )

// ������ (2005-04-06 ���� 10:40:33) : 
// ���̺� �÷��� ����..
void	SaveSetChangeGeometry	();
void	SaveSetChangeMoonee		();
void	SaveSetChangeTileList	();
void	SaveSetChangeObjectList	();

void	GetSubDataDirectory		( char * pStr );

void	ClearAllSaveFlags		();

// ������ (2005-06-16 ���� 11:57:54) : 
// NPC�� ������ �����ϱ� ���� �÷���..

#define	BOSSMOB_OFFSET	100000
#define	ISNPCID( id )	( id < BOSSMOB_OFFSET )
#define	ISBOSSID( id )	( id >= BOSSMOB_OFFSET )

#define ISBUTTONDOWN( key )	( GetAsyncKeyState( key ) < 0 )

#define USE_NEW_GRASS_FORMAT
