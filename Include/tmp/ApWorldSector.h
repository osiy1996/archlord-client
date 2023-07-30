// ApWorldSector.h: interface for the ApWorldSector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APWORLDSECTOR_H__FA496200_24F6_4F97_8D80_4AAA2462BEA1__INCLUDED_)
#define AFX_APWORLDSECTOR_H__FA496200_24F6_4F97_8D80_4AAA2462BEA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "MagList.h"
#include "ApmMap.h"
#include "ApDefine.h"
#include "ApMapBlocking.h"
#include "AuOTree.h"
#include "AuList.h"
#include "ApRWLock.h"
#include <vector>

//#ifdef DEBUG_NEW
//#undef DEBUG_NEW

// ��� ����.
#define	ALEF_MAPFILE				0x0001
#define	MAPSTRUCTURE_FILE_VERSION	0x0002

#define	ALEF_GEOMETRYFILE			0x0002

//#define GEOMETRY_FILE_VERSION		0x0003
#define GEOMETRY_FILE_VERSION		0x0004

#define	ALEF_MOONIEFILE				0x0003
//#define MOONEE_FILE_VERSION		0x0003
#define MOONEE_FILE_VERSION			0x0004
// ������ (2005-03-15 ���� 3:27:56) : ���� ��.. ������Ʈ�� ����Ʈ ���� �߰�

#define ALEF_COMPACT_FILE_VERSION	0x1000
#define ALEF_COMPACT_FILE_VERSION2	0x1001
	// ������ (2005-07-19 ���� 11:26:02) : ���۽� ���� �ɼ� �߰�..
#define ALEF_COMPACT_FILE_VERSION3	0x1002
	// ������ (2005-12-13 ���� 4:45:27) : ���̰� FLOAT�� ���� , ������ UINT8 -> UINT16 , ������ �ʵ� ����.
#define ALEF_COMPACT_FILE_VERSION4	0x1003
	// ������ (2007-07-03) : Ÿ�Ͽ� ������Ʈ ��ŷ 4��Ʈ�� NoLayer �ɼǰ� Reserved�� ����.
#define ALEF_SERVER_DATA_VERSION	0x2001

#define	MAPSTRUCTURE_FILE_HEADER	( ( ALEF_MAPFILE		<< 16 ) | MAPSTRUCTURE_FILE_VERSION	)
#define	ALEF_GEOMETRYFILE_HEADER	( ( ALEF_GEOMETRYFILE	<< 16 ) | GEOMETRY_FILE_VERSION		)
#define	ALEF_MOONIEFILE_HEADER		( ( ALEF_MOONIEFILE		<< 16 ) | MOONEE_FILE_VERSION		)

#define SERVER_DATA_VERSION			0x10011001

// ������ (2005-03-15 ���� 3:29:18) : 
// ���� ������ ȣȯ��..
#define	ALEF_MOONIEFILE_HEADER_COMPATIBLE	( ( ALEF_MOONIEFILE		<< 16 ) | 0x003	)


#define	ALEF_GetFileType( x )		( x >> 16 )
#define	ALEF_GetFileVersion( x )	( x & 0x0000ffff )
#define	ALEF_SECTOR_DEFAULT_TILE_INDEX		( 0x00010380 )		// ����Ʈ�� ���� �Ŵ� Ÿ�� ��ȣ
#define	ALEF_SECTOR_DEFAULT_ROUGHTILE_INDEX	( 0x00010200 )		// ����Ʈ�� ���� �Ŵ� Ÿ�� ��ȣ
#define	ALEF_DEFAULT_ALPHA					( 0x000001c0 )		// ����Ʈ �ؽ����� ���� �ε���.
#define	ALEF_TEXTURE_NO_TEXTURE				( 0 )
#define	ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT	1.0f	// ���׸�Ʈ�� ��������� ����Ʈ ����

#define	ALEF_MAX_HEIGHT					10000.0f
#define	ALEF_MIN_HEIGHT					0.0f

#define	ALEF_COMPACT_DATA_DEFAULT_DEPTH	16

#define	MAX_BLOCKCOUNT				20
#define	MAX_USER_COUNT_IN_SECTOR	200
#define	MAX_ITEM_COUNT_IN_SECTOR	200
#define	MAX_OBJECT_COUNT_IN_SECTOR	200
#define	MAP_INITIAL_HEIGHT			(5.0f)

enum TextureDepth
{
	TD_FIRST	,	// ���̽� �ؽ���
	TD_SECOND	,	// ���� �ؽ���
	TD_THIRD	,	// ���İ� ����� ���̽� �ؽ���/
	TD_FOURTH	,	// 3���� ���Ŀ�..
	TD_FIFTH	,	// 3���� ���Ŀ�..
	TD_SIXTH	,	// ���� �׷��� �ؽ���.

	// Reserved
	TD_SEVENTH	,
	TD_EIGHTTH	,
	TD_DEPTH
};

#define	SECTOR_COMPACT_BLOCK_FILE		"c%04d.mpf"
#define	SECTOR_GEOMETRY_BLOCK_FILE		"g%04d.mpf"
#define	SECTOR_MOONIE_BLOCK_FILE		"m%04d.mpf"
#define	SECTOR_SERVERDATA_FILE			"sd%04d.bin"

#define	SECTOR_MAX_COLLISION_OBJECT_COUNT	10

// �ؽ��� �ε��� ����.
// 11111111 | 11111111 | 11111111 | 11111111
//    �Ⱦ�    Category    Index    �տ� �κ�Ʈ�� Type , �ڿ� 6���� Offset.
//

// Tile Blocking Flag..
// INT32 ���� ��ŷ..
// ���� char���� ����Ÿ�� ��Ʈ�� �����Ѵ�.
// 33333333222222221111111100000000 ���� ó���Ѵ�.
#define	TBF_NONE		0
#define	TBF_GEOMETRY	1
#define	TBF_OBJECT		2

enum	TILE_POSITION
{
	TP_LEFTTOP		,
	TP_RIGHTTOP		,
	TP_LEFTBOTTOM	,
	TP_RIGHTBOTTOM	,
	TP_MAX
};

// ��ŷ ������ ���� ������.
// ���Ϳ��� ��ŷ ������ ����Ҷ� ����Ѵ�.
// ��ŷ ������ �� �׸��带 2x2�� �ɰ��� �����Ѵ�.
// nPosition�� TILE_POSITION ��..
// nFlag ���� ������ �����ϹǷ�, ���� ���� ���� �ϸ鼭 �÷��� �߰��� ���ؼ���
// _GetBlocking_Unit �Լ��� ���� ����Ͽ��� �Ѵ�.
inline	void	_SetBlocking_Unit( INT32 * pBlocking , INT32 nPosition , INT32 nFlag	)
{
	INT32 mask = ~( 0xff << ( nPosition * 8 ) );
	*pBlocking &= mask;
	*pBlocking |= nFlag << ( nPosition * 8 );
}

// �ش� ��ġ�� ���� ��..
// nPosition�� TILE_POSITION ��..
inline	INT32	_GetBlocking_Unit( INT32 * pBlocking , INT32 nPosition					)
{
	return ( *pBlocking & ( 0x000000ff << ( nPosition * 8 ) ) ) >> ( nPosition * 8 );
}



// ������ ���̸� �����ϱ� ���� ��Ʈ����.

class	ApmMap						;

// Vertex Color�� �����ϱ� ���� ��Ʈ����
struct	ApRGBA
{
	UINT8	red		;
	UINT8	green	;
	UINT8	blue	;
	UINT8	alpha	;	
};

// ���� ������ ����.
enum	SECTOR_DETAIL
{
	SECTOR_EMPTY		,
	SECTOR_LOWDETAIL	,
	SECTOR_HIGHDETAIL	,
	SECTOR_DETAILDEPTH	,
	SECTOR_ERROR		// ������ ���!
};

enum	TILE_DIRECTION
{
	TD_NORTH	,
	TD_EAST		,
	TD_SOUTH	,
	TD_WEST		,
	TD_DIRECTIONCOUNT
};

inline	void	_SetFlag( INT32 * pBlocking , int offset )
{
	int data = 0x01 << offset	;
	int mask = ~data			;

}

inline	void	_UnSetFlag( INT32 * pBlocking , int offset )
{
	int data = 0x01 << offset	;
	int mask = ~data			;

	*pBlocking &= mask			;
}

struct	ApTileInfo
{
	enum
	{
		BLOCKNONE		=	0x00	,
		BLOCKGEOMETRY	=	0x01	,
		BLOCKOBJECT		=	0x02	,
		BLOCKSKY		=	0x04
	};

	enum GEOMETRYBLOCK
	{
		GB_NONE		= 0x00,
		GB_GROUND	= 0x01,
		GB_SKY		= 0x02
	};

	UINT8	bEdgeTurn	  :1;	// ���� �� ����.
	UINT8	tiletype	  :7;	// Ÿ���� ������ Ÿ���� �����Ѵ�.
	UINT8	geometryblock :4;	// ���� ��ŷ.. 0x01 �� �� 0x02�� �ϴ� , 0x04,0x08�� �����ʵ�.

	// UINT8	objectblock	  :4;	// ������Ʈ ��ŷ . 4���� ��ġ�� ��Ʈ �÷��׷� ����.
	// ������� �ʱ⶧���� ������.

	UINT8	bNoLayer	  :1;	// ������ ������� ������ǥ�� ,�� ����� Ridable Object�� ���� �÷�������.
	UINT8	reserved	  :3;	// �ʳ��� ���� ����..

	ApTileInfo()
	{
		bEdgeTurn		= 0;
		tiletype		= 0;
		geometryblock	= 0;
		//objectblock		= 0;

		bNoLayer		= 0;
		reserved		= 0;
	}

	/*
	// Zero Base
	void	SetObjectBlocking( INT32 nPosition , BOOL bBlock )
	{
		UINT8 data = 0x01 << nPosition	;
		UINT8 mask = ~data				;

		objectblock	 &= mask	;

		if( bBlock )
			objectblock |= data	;
	}

	BOOL	GetObjectBlocking( INT32 nPosition )
	{
		if( objectblock & ( 0x01 << nPosition ) )	return TRUE		;
		else										return FALSE	;
	}
	*/

	void	SetNoLayer( BOOL bNoLayer )
	{
		this->bNoLayer = bNoLayer ? TRUE : FALSE;
	}

	BOOL	IsNoLayer() { return ( BOOL ) bNoLayer; }

	void	SetGeometryBlocking( BOOL bBlock )
	{
		UINT8 data = GB_GROUND;
		UINT8 mask = ~data			;

		geometryblock	 &= mask	;
		if( bBlock )
			geometryblock |= data	;
	}

	void	SetSkyBlocking( BOOL bBlock )
	{
		UINT8 data = GB_SKY;
		UINT8 mask = ~data			;

		geometryblock	 &= mask	;
		if( bBlock )
			geometryblock |= data	;
	}

	BOOL	GetGeometryBlocking()
	{
		return ( geometryblock & GB_GROUND ) ? TRUE : FALSE;
	}

	BOOL	GetSkyBlocking()
	{
		return ( geometryblock & GB_SKY ) ? TRUE : FALSE;
	}

	UINT8	GetBlocking( INT32 nPosition )
	{
		// ������ (2004-05-03 ���� 1:08:01) : ���� ��� ����..
		// � ��ŷ�� �ɷȴ��� �÷��׷� ����..
		// üũ�� ApTileInfo::BLOCKGEOMETRY �̷� �༮���� �̿��ϸ� ok
		UINT8	nRet = BLOCKNONE;
		if( GetGeometryBlocking()			) nRet |= BLOCKGEOMETRY	;
		//if( GetObjectBlocking( nPosition )	) nRet |= BLOCKOBJECT	;
		return nRet;
	}

	// ������ (2004-06-15 ���� 10:42:13) : ������ ����..
	BOOL	SetEdgeTurn( BOOL bEnable )
	{
		if( bEnable )	bEdgeTurn = TRUE	;
		else			bEdgeTurn = FALSE	;
		return bEdgeTurn;
	}
	BOOL	GetEdgeTurn() { return ( BOOL ) bEdgeTurn; }
};

#define APCOMPACTSEGMENTSIZE			4
#define APCOMPACTSEGMENTSIZE_20040917	6
#define APCOMPACTSEGMENTSIZE_20051213	8

// Ŭ���̾�Ʈ���� ���Ǵ� ����� ���������� �����ϱ� ���� ���׸�Ʈ Ŭ����..
struct	ApCompactSegment
{
	// ������ (2005-12-19 ���� 10:45:57) : ���� ������ ������.���� ��������Ÿ�� ����.
	// FLOAT		fHeight	;	// �������� , ������ ���� ���ؾ���..
	ApTileInfo	stTileInfo	;
	
	UINT16		uRegioninfo		;	// ���� ����..

	ApCompactSegment():uRegioninfo(0)
	{
	}

	// ������ (2004-09-17 ���� 11:46:38) : 
	// ���� ����..
	UINT16	GetRegion()					{ return uRegioninfo; }
	UINT16	SetRegion( UINT16 uRegion)	{ return uRegioninfo = uRegion; }
};

struct	ApCompactSegment_20040917
{
	UINT16		fHeight			;	// �������� , ������ ���� ���ؾ���..
	ApTileInfo	stTileInfo		;
	UINT8		uRegioninfo		;	// ���� ����..
	UINT8		uReserved		;
};


// �������� ����ϴ� �������� ������ ��� �ִ� 
struct	ApDetailSegment
{
	enum	FLAG
	{
		USEFLATSHADING	,
		FLAGCOUNT		,
	};
	
	FLOAT		height				;
	UINT32		pIndex[ TD_DEPTH ]	; // Ÿ�Ϲ�ȣ. ALEF_TILE_DO_NOT_USE �̸� ������� ����.
	ApRGBA		vertexcolor			;
	ApTileInfo	stTileInfo			; // ��ŷ ���� �����.. �̳༮ ���� ���� ����� �̿��Ѵ�.. 2x2�� ��ŷ ������ ����.
	UINT16		nPadding			;

	// ����Ÿ ���� �ʵ�..
	
	INT32	nFlags					;	// ��Ʈ �÷��� 32���� ������..
	UINT16	uCheckTime				;	// ������ ����ϴ� üũŸ��..

	void	SetDefault( FLOAT fDefaultHeight = 1.0f )
	{
		height					= fDefaultHeight			;

		// �ؽ��� �ʱ�ȭ.
		pIndex[ TD_FIRST ]	= ALEF_SECTOR_DEFAULT_TILE_INDEX;
		for( int t = 1 ; t < TD_DEPTH ; ++t )
			pIndex[ t ]		= ALEF_TEXTURE_NO_TEXTURE	;

		// ���ؽ� Į�� �ʱ�ȭ.
		vertexcolor	.alpha		= 255						;
		vertexcolor	.red		= 255						;
		vertexcolor	.green		= 255						;
		vertexcolor	.blue		= 255						;

		// Reserved
		nFlags					= 0							;
		nPadding				= 0							;
	}

	//void	SetBlocking( INT32 nPosition , INT32 nFlag	) { stTileInfo.SetGeometryBlocking( nFlag ); }
	//INT32	GetBlocking( INT32 nPosition				) { return stTileInfo.GetGeometryBlocking(); }

	void	SetFlag		( INT32 nFlagIndex , BOOL bData	)
	{
		ASSERT( nFlagIndex >= 0 && nFlagIndex < FLAGCOUNT );

		int	mask	= 0x00000001 << nFlagIndex				;
		int	bitflag	= ( ( bData ) ? 1 : 0 ) << nFlagIndex	;

		nFlags = ( nFlags & ~mask ) | bitflag				;
	}

	BOOL	GetFlag		( INT32 nFlagIndex				)
	{
		ASSERT( nFlagIndex >= 0 && nFlagIndex < FLAGCOUNT );
		return nFlags & ( 0x00000001 << nFlagIndex )		;
	}
};

struct	ApCompactSectorInfo
{
	ApCompactSegment *	m_pCompactSegmentArray		;
	INT32				m_nCompactSegmentDepth		;

	ApCompactSectorInfo()
	{
		m_nCompactSegmentDepth = 0		;
		m_pCompactSegmentArray = NULL	;
	}

	~ApCompactSectorInfo()
	{
		Free();
	}

	BOOL	Alloc( int nDepth )
	{
		Free();
		m_nCompactSegmentDepth = nDepth;
		m_pCompactSegmentArray = new ApCompactSegment[ nDepth * nDepth ];
		return TRUE;
	}

	BOOL	Free()
	{
		if( m_pCompactSegmentArray )
		{
			delete [] m_pCompactSegmentArray;
			m_pCompactSegmentArray = NULL;
		}
		m_nCompactSegmentDepth = 0;

		return TRUE;
	}

	ApCompactSegment * GetSegment( INT32 x , INT32 z )
	{
		if( m_pCompactSegmentArray )
			return &m_pCompactSegmentArray[ x + z * m_nCompactSegmentDepth ];
		else
			return NULL;
	}

	BOOL	IsLoaded() { if( m_pCompactSegmentArray ) return TRUE; else return FALSE; }
};

struct	ApDetailSectorInfo
{
	ApDetailSegment *						m_pSegment		[ SECTOR_DETAILDEPTH ];
	int										m_nDepth		[ SECTOR_DETAILDEPTH ]; // ������

	// Reserved parameter;
	int		m_nAlignment;	// �ؽ��� ��� Ÿ��.
	int		m_nReserved1;
	int		m_nReserved2;
	int		m_nReserved3;
	int		m_nReserved4;
};

// ������ (2005-12-14 ���� 3:10:20) : 
// ���� ������ ������ �����ϱ� ���� ����Ÿ.
struct ApServerSectorInfo
{
protected:
	AuPOS	* pcsVector		;

	INT32	nTotal			;
	INT32	nBlockingCount	;
	INT32	nRidableCount	;

public:
	AuPOS	* GetVector() { return pcsVector		; }

	INT32	GetTotal		() { return nTotal			; }
	INT32	GetBlockingCount() { return nBlockingCount	; }
	INT32	GetRidableCount	() { return nRidableCount	; }
	
	ApServerSectorInfo():pcsVector( NULL ),nTotal( 0 ),nBlockingCount( 0 ),nRidableCount( 0 ) {}
	~ApServerSectorInfo(){ FreeVector(); }

	BOOL	AllocVector( int nCount )
	{
		FreeVector();

		if( nCount > 0 )
		{
			pcsVector		= new AuPOS[ nCount ];
			nTotal			= nCount;
			nBlockingCount	= nCount;
			nRidableCount	= nCount;
			return TRUE;
		}
		else
			return FALSE;
	}

	void	SetBlockingVertexCount( int nCount )
	{
		nBlockingCount = nCount;
	}
	void	SetRidableVertexCount( int nCount )
	{
		nRidableCount = nCount;
	}

	void	FreeVector()
	{
		if( pcsVector ) delete [] pcsVector;
		pcsVector		= NULL;
		nTotal			= 0;
		nBlockingCount	= 0;
		nRidableCount	= 0;
	}
};
		
	//typedef struct IdPos IdPos;
// ���� ���� ����..
class ApWorldSector  
{
public:
	// ���ۺ� ��� �����弱��..
	friend class ApmMap;

	typedef enum
	{
		STATUS_INIT		= 0x01,	// ���Ͱ� �޸������� �ݹ��� �ҷ��� �ִ� ����..
		STATUS_LOADED	= 0x02,	// ����Ÿ �ε� ����� ����ž� ���Ϳ���Ÿ�� �����ϰ� �ִ°��.
		STATUS_DELETED	= 0x04, // 
	} Status;

	typedef struct	IdPos
	{
		AuPOS	pos		;
		INT32	range	;
		INT_PTR	id		;	// pCharacter 
		INT_PTR	id2		;	// CID
		IdPos *	pNext	;
		IdPos *	pPrev	;
	} IdPos;

	enum HEIGHTINFO
	{
		NOHEIGHTINFO			= 0x00,
		HEIGHTWITHRIDABLEOBJECT	= 0x01,
		HEIGHTWITHGEOMETRYONLY	= 0x02,
		HEIGHTINFO_MAX			= 0x03
	};
	struct	HeightPool
	{
		UINT8	uDataAvailable	;
		FLOAT	fHeight			;
		FLOAT	fGeometryHeight	;

		HeightPool():uDataAvailable( NOHEIGHTINFO ),fHeight(0.0f), fGeometryHeight( 0.0f ) {}
	};

	// �ε��� ������ ���� �༮..
	struct	IndexStruct
	{
		INT32	nCount	;
		INT32	pArray[ SECTOR_MAX_COLLISION_OBJECT_COUNT ];

		IndexStruct():nCount(0){}
	};

	struct	IndexArray
	{
		IndexStruct	*	pIndexStruct	;
		INT32			nDepth			;

		IndexArray(): pIndexStruct( NULL ) , nDepth( 0 ) {}
	};

	struct	AuLineBlock	: public AuLine
	{
	public:
		UINT32 uSerial; // ��ŷ ������ �ε���.. �ߺ��� ���� Ȯ�ο�.

		AuLineBlock() : uSerial( 0 ) {}

		void	FromAuLine( AuLine &stParam )
		{
			this->start	= stParam.start	;
			this->end	= stParam.end	;
		}
	};

	enum	INDEX_TYPE
	{
		AWS_COLLISIONOBJECT	= 0	,	// �ݸ��� ������Ʈ..
		AWS_SKYOBJECT			,	// ��ī�� ������ ���� ������Ʈ..
		AWS_GRASS				,	// Ǯ������ ���� ������Ʈ..
		AWS_RIDABLEOBJECT		,
		AWS_COUNT			
	};

	// ������ (2005-03-15 ���� 3:24:10) : 
	// ������Ʈ�� �ɼǵ�..
	enum	SECTOR_OPTIONS
	{
		OP_NONE					= 0x0000,
		OP_GEOMETRYEFFECTENABLE	= 0x0001,
		OP_DONOTLOADSECTOR		= 0x0002,	// ��������Ÿ�� Ŭ���̾�Ʈ���� �о������ �ʴ´�.
		OP_HEIGHTUPDATEAFTERLOAD= 0x0004,	// ���� �о���� �Ŀ� �ɸ��� ���̸� �����Ѵ�. ( Ŭ���̾�Ʈ���� )
	};

	enum	LOADING_FLAG
	{
		LF_NONE				= 0x0000,
		LF_OBJECT_LOADED	= 0x0001
	};
	
protected:
	INT32			m_lStatus		; // WorldSector�� ���� ���� �ٲܶ��� �ݵ�� Lock�ϰ� �ٲٽÿ�.

	BOOL			m_bInitialized	;

	// ���Ͱ� Ŀ���ϴ� ����.
	FLOAT			m_fxStart		; // ��ü���������� X��ǥ ��ŸƮ
	FLOAT			m_fxEnd			;
	FLOAT			m_fyStart		; // ��ü���������� Y��ǥ ��ŸƮ
	FLOAT			m_fyEnd			;
	FLOAT			m_fzStart		; // ��ü���������� Z��ǥ ��ŸƮ
	FLOAT			m_fzEnd			;

	//FLOAT			m_fStepSize		; // ������ ���� ������..

	// �ʿ����� ���� �ε��� .
	INT32			m_nIndexX		;
	INT32			m_nIndexZ		;

	INT32			m_nCurrentDetail;

	// ������ (2005-03-15 ���� 3:24:33) : 
	DWORD			m_uFlag			; // �ɼǵ�..
	DWORD			m_uLoadingFlag	;

public:
	// ���� �� ����Ŵ� ����..

	// �� ��Ʈ����..
	// �������� �� �������� ������ �´�..

	struct	Dimension
	{	
		INT32	nIndex		;	// ������ �� �ε��� . ApmMap���� �ο�����.		

		IdPos *	pUsers		;
		IdPos *	pNPCs		;
		IdPos *	pMonsters	;
		IdPos *	pItems		;
		IdPos *	pObjects	;

		// ���� ���� �߰�.. �����ؼ� �������.
		// ���� ������ ������ ������ �ٲٰ� ������
		// �����ϱⰡ �������Ƿ� �밡�ٷ� ����.

		ApRWLock	lockUsers	;
		ApRWLock	lockNPCs	;
		ApRWLock	lockMonsters;
		ApRWLock	lockItems	;
		ApRWLock	lockObjects	;

		Dimension() :
			nIndex		( 0	   ),
			pUsers		( NULL ), 
			pNPCs		( NULL ), 
			pMonsters	( NULL ), 
			pItems		( NULL ), 
			pObjects	( NULL )
		{
			// do nothing..
		}

		Dimension( const Dimension &stParam )
		{
			this->nIndex	= stParam.nIndex	;
			this->pUsers	= stParam.pUsers	;
			this->pNPCs		= stParam.pNPCs		;
			this->pMonsters	= stParam.pMonsters	;
			this->pItems	= stParam.pItems	;
			this->pObjects	= stParam.pObjects	;
		}
	};

	// ����� ó��..
	std::vector< Dimension >		m_arrayDimension;	// ��迭�� ����
	std::vector< AuLineBlock >		m_vecBlockLine;	// ���� ��ŷ ����..

	BOOL	AddLineBlock( AuLineBlock & stParam );
	BOOL	ClearLineBlock();
	INT32	GetLineBlockCount();

	Dimension *	GetDimensionArray() { return &m_arrayDimension[ 0 ]; }
	INT32		GetDimensionCount() { return (INT32)m_arrayDimension.size(); }
	Dimension *	GetDimension( INT32 nCreatedIndex );

	BOOL	CreateDimension( INT32 nCreatedIndex );
	BOOL	DeleteDimension( INT32 nCreatedIndex );

	//ApCriticalSection	m_Mutex						;
	ApRWLock			m_RWLock					;
	AuOTree				m_csOTree					;
	
	// AgcmMap ���� ����ϴ� ť �ɼ� �����.
	UINT32				m_uQueueOffset				;

	UINT32				m_ulRemoveTimeMSec			;

	// Compact Segment Data...
	ApCompactSectorInfo	*	m_pCompactSectorInfo	;
	ApDetailSectorInfo	*	m_pDetailSectorInfo		;
	ApServerSectorInfo		m_ServerSectorInfo		;	// �����ͻ����������..

	BOOL	AllocDetailSectorInfo	();
	BOOL	FreeDetailSectorInfo	();

	BOOL	AllocCompactSectorInfo	();
	BOOL	FreeCompactSectorInfo	();

	bool	LockSector		(); // ��ü�� ���� Write Lock
	void	ReleaseSector	(); // Write Lock �Ѱ� ������..

protected:

	// Detail Segment Data...

	// Height Pool
	HeightPool	*		m_pHeightPool			;

	IndexArray			m_aIndexArray[ AWS_COUNT ]	;

	void				CreateCollisionObjectArray(){ CreateIndexArray( AWS_COLLISIONOBJECT ); }
	void				DeleteCollisionObjectArray(){ DeleteIndexArray( AWS_COLLISIONOBJECT ); }

public:
	void				CreateIndexArray( INT32 nIndexType );
	void				DeleteIndexArray( INT32 nIndexType );

	void				DeleteAllIndexArray()
	{
		for( int i = 0 ; i < AWS_COUNT ; ++ i ) DeleteIndexArray( i );
	}
	BOOL				AddObjectCollisionID( int x , int z , INT32 oid )
	{
		return			AddIndex( AWS_COLLISIONOBJECT , x , z , oid );
	}
	INT32				GetObjectCollisionID( int x , int z , INT32 * pArrayID , INT32 nMax )	// Return is its count.
	{
		return			GetIndex( AWS_COLLISIONOBJECT , x , z , pArrayID , nMax );
	}

	BOOL				AddIndex		( INT32	nIndexType , int x , int z , INT32 oid			, BOOL bOverlappingTest = FALSE );
	INT32				GetIndex		( INT32	nIndexType , int x , int z , INT32 * pArrayID	, INT32 nMax );
	BOOL				DeleteIndex		( INT32	nIndexType , int x , int z );	// �ش缼�׸�Ʈ�� �ε����� �����Ѵ�.
	BOOL				DeleteIndex		( INT32	nIndexType , int x , int z , INT32 nIndex		);	// �ش缼�׸�Ʈ�� �ε����� �����Ѵ�.

	BOOL				AddSkyObjectTemplateID	( int x , int z , INT32 nTemplate , FLOAT fDistance );
	INT32				GetSkyObjectTemplateID	( int x , int z , INT32 * pArrayID , INT32 nMax );	// Return is its count.

	INT32				GetStatus()	{ return m_lStatus; }

public:
	BOOL	AllocHeightPool			();
	BOOL	FreeHeightPool			();

	BOOL	HP_SetHeight			( int x , int z , FLOAT fHeight );
	FLOAT *	HP_GetHeight			( int x , int z );
	BOOL	HP_SetHeightGeometryOnly( int x , int z , FLOAT fHeight );
	FLOAT *	HP_GetHeightGeometryOnly( int x , int z );

// Operations
		// Get Functions..
		INT32	GetIndexX	() const	{ return m_nIndexX	; }
		INT32	GetIndexZ	() const	{ return m_nIndexZ	; }

		INT32	GetArrayIndexX() const;
		INT32	GetArrayIndexZ() const;

		int	GetArrayIndexDWORD	() const;
		inline int	GetCurrentDetail	() { return m_nCurrentDetail; }

		FLOAT	GetXStart	() const	{ return m_fxStart	; }
		FLOAT	GetXEnd		() const	{ return m_fxEnd	; }
		FLOAT	GetYStart	() const	{ return m_fyStart	; }
		FLOAT	GetYEnd		() const	{ return m_fyEnd	; }
		FLOAT	GetZStart	() const	{ return m_fzStart	; }
		FLOAT	GetZEnd		() const	{ return m_fzEnd	; }

		FLOAT	GetStepSizeX() ;
		FLOAT	GetStepSizeZ() ;
		FLOAT	GetStepSizeX( int nDetail ) ;
		FLOAT	GetStepSizeZ( int nDetail ) ;

		UINT	GetDivisionOffset()
		{
			return ( UINT ) ( (GetArrayIndexX() % 16) + (GetArrayIndexZ() % 16) * 16 );
		}

		// ������ (2005-03-15 ���� 3:40:33) : �÷��� ����..
		DWORD	GetFlag		() const	{ return m_uFlag	; }
		void	SetFlag		( DWORD uFlag )	{ m_uFlag = uFlag;}

		void	SetLoadingFlag	( LOADING_FLAG eFlagIndex , BOOL bData	)
		{
			int	mask	= 0x00000001 << eFlagIndex				;
			int	bitflag	= ( ( bData ) ? 1 : 0 ) << eFlagIndex	;

			m_uLoadingFlag = ( m_uLoadingFlag & ~mask ) | bitflag;
		}

		BOOL	GetLoadingFlag		( LOADING_FLAG eFlagIndex				)
		{
			return m_uLoadingFlag & ( 0x00000001 << eFlagIndex )		;
		}

		ApCompactSectorInfo *	GetCompactSectorInfo()	{ return m_pCompactSectorInfo	; }
		ApDetailSectorInfo *	GetDetailSectorInfo()	{ return m_pDetailSectorInfo	; }

	// ���� ��ǵ�
protected:
		// ������ �� �ۺ� ��⿡���� ȣ���� �����ϴ�. ( �ݹ� ���������� );
		// ������ ������ ó������ ������..
		BOOL	SetupSector		( INT32 indexX , INT32 indexY , INT32 indexZ			);
		BOOL	LoadSector		( BOOL bLoadCompact = TRUE , BOOL bLoadDetail = TRUE	);
		BOOL	LoadSectorServer();
		//BOOL	_LoadSectorOld	( INT32 indexX , INT32 indexY , INT32 indexZ			);
		// ������ ��� ����Ÿ�� ����..
		BOOL	RemoveAllData	();

public:

		// OTree : Parn �۾�, OTree �����ϱ� ���ؼ� Init�����.
		BOOL	Init			( FLOAT width	, FLOAT unitsize , BOOL bBlock = TRUE	);
		// OTree : AddObject�� �� Blocking ������ ������ OTree�� Optimize�Ѵ�.
		void	OptimizeBlocking();


		inline ApWorldSector *	GetNearSector	( int direction );	// ���� ������ �����͸� ����.

		// ��ǥ�� �� ���� �ȿ� �����ϴ°�..
		// ���� ��ǥ���� �˻��Ѵ�.
		inline bool		IsInSector		( AuPOS pos	) { return IsInSector( pos.x , pos.y , pos.z ); }
		inline bool		IsInSector		( FLOAT x , FLOAT y , FLOAT z )
		{
			if( x <		m_fxStart	||	z <	m_fzStart	||
				x >=	m_fxEnd		||	z >= m_fzEnd	)	return false	;
			else											return true		;
		}
		BOOL	IsInSectorRadius( float x , float z , float radius );
		BOOL	IsInSectorRadiusBBox( float x1 , float z1 , float x2 , float z2 );

		// ��ŷ üũ �Լ�.
		FLOAT	GetHeight		( FLOAT x , FLOAT y , FLOAT z	);	// �ش� ��ǥ�� �̷��� ���̸� ����س���.
		UINT8	GetType			( FLOAT x , FLOAT y , FLOAT z	);	// �ٴ��� Ÿ���� ����.
		UINT8	GetBlocking		( FLOAT x , FLOAT y , FLOAT z	, INT32 eType );
			// �ε����� �Ѿ�´�..
			// ������ŷ�� ������Ʈ ��ŷ�� ���еž� �÷��׷� �Ѿ�ͼ� üũ�Ѵ�.

		// �̰� 50x50 ��ǥ..
		//UINT8	SetBlocking		( INT32 x , INT32 z , INT32 nBlocking	);
			// nBlocking �� Bit �÷��׷� �����Ͽ� ���� �Ѵ�.
			// �ٷ� �����ϴ°��̱� ������ ���Ǹ� ���Ѵ�.
		//UINT8	AddBlocking		( INT32 x , INT32 z , INT32 nBlockType	)
		//	{ return SetBlocking( x , z , GetBlocking( x , z , 0x00 ) | nBlockType ); }	// 0x00 �� GROUND BLOCK
		//UINT8	AddBlocking		( AuBLOCKING * pBlocking  , INT32 count	, INT32 nBlockType );
			// �̳༮�� Ÿ�Ը� �ִ´�. �ش� ��ŷ�� �߰��Ѵ�.
			// ���� ����Ÿ�� Or �������� ���� �߰��´�.
		// void	ClearObjectBlocking();
		UINT8	GetBlocking		( INT32 x , INT32 z	, INT32 eType );
	
		FLOAT	GetHeight		( AuPOS pos ) { return GetHeight	( pos.x , pos.y , pos.z ); }	// �ش� ��ǥ�� �̷��� ���̸� ����س���.
		UINT8	GetType			( AuPOS pos ) { return GetType		( pos.x , pos.y , pos.z ); }	// �ٴ��� Ÿ���� ����.
		UINT8	GetBlocking		( AuPOS pos , INT32 eType ) { return GetBlocking	( pos.x , pos.y , pos.z , eType ); }

		BOOL	IsPassThis		( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint  = NULL		);

		// ���;ȿ� ��ġ�ϴ� �ɸ��� ,������ ���� ������ ����,�����ϱ� ���� �Լ���..
		BOOL	AddChar			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range = 0		);
		BOOL	AddChar			( INT32	nDimensionIndex , IdPos *pUser											);
		BOOL	UpdateChar		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid							);
		IdPos *	GetChar			( INT32	nDimensionIndex , INT_PTR cid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteChar		( INT32	nDimensionIndex , INT_PTR cid												);
		VOID	RemoveChar		( INT32	nDimensionIndex , IdPos *pUser ,	BOOL bIsNeedLock = FALSE			);

		BOOL	AddNPC			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range = 0		);
		BOOL	AddNPC			( INT32	nDimensionIndex , IdPos *pNPC											);
		BOOL	UpdateNPC		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid							);
		IdPos *	GetNPC			( INT32	nDimensionIndex , INT_PTR cid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteNPC		( INT32	nDimensionIndex , INT_PTR cid												);
		VOID	RemoveNPC		( INT32	nDimensionIndex , IdPos *pNPC ,		BOOL bIsNeedLock = FALSE			);

		BOOL	AddMonster		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range = 0		);
		BOOL	AddMonster		( INT32	nDimensionIndex , IdPos *pMonster										);
		BOOL	UpdateMonster	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid							);
		IdPos *	GetMonster		( INT32	nDimensionIndex , INT_PTR cid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteMonster	( INT32	nDimensionIndex , INT_PTR cid												);
		VOID	RemoveMonster	( INT32	nDimensionIndex , IdPos *pMonster , BOOL bIsNeedLock = FALSE			);

		BOOL	AddItem			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid							);
		BOOL	AddItem			( INT32	nDimensionIndex , IdPos *pItem											);
		BOOL	UpdateItem		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid							);
		IdPos *	GetItem			( INT32	nDimensionIndex , INT_PTR iid	,	BOOL bIsNeedLock = TRUE				);
		BOOL	DeleteItem		( INT32	nDimensionIndex , INT_PTR iid												);
		VOID	RemoveItem		( INT32	nDimensionIndex , IdPos *pItem ,	BOOL bIsNeedLock = FALSE			);

		// Parn �۾� Blocking ���� �߰�
		BOOL	AddObject		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid ,	AuBLOCKING *pstBlocking = NULL	);
		BOOL	AddObject		( INT32	nDimensionIndex , IdPos *pObject			  ,		AuBLOCKING *pstBlocking = NULL	);
		BOOL	UpdateObject	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid									);
		IdPos *	GetObject		( INT32	nDimensionIndex , INT_PTR oid	,		BOOL bIsNeedLock = TRUE						);
		BOOL	DeleteObject	( INT32	nDimensionIndex , INT_PTR oid														);
		VOID	RemoveObject	( INT32	nDimensionIndex , IdPos *pObject ,	BOOL bIsNeedLock = FALSE					);

	// Compact Segment Funcitons...

		// �ε� ����..
		BOOL	IsLoadedCompactData				() { if( m_pCompactSectorInfo && m_pCompactSectorInfo->IsLoaded() ) return TRUE ; else return FALSE; }
		BOOL	LoadCompactData					();	//	Compact ����Ÿ�� �о����.
		//BOOL	_LoadCompactData_Old			();	//	Compact ����Ÿ�� �о����.
		BOOL	_LoadCompactData_FromBuffer		( BYTE * pBuffer , UINT uSize );	//	Compact ����Ÿ�� �о����.
		
		BOOL	LoadServerData					();
			// ������ (2005-12-14 ���� 3:31:47) : 
			// �������� �ε��Ҷ� ���� �뵵..
			// ���� ������������ �о����.
		
		BOOL	SaveCompactData					( BOOL bServer = FALSE );	//	���� �ε�� ����Ʈ ����Ÿ�� ������.

		BOOL	CreateCompactDataFromDetailInfo	( int nTargetDetail = SECTOR_HIGHDETAIL , int nDepth = ALEF_COMPACT_DATA_DEFAULT_DEPTH );	//	������ ����Ÿ���� ����Ʈ ����Ÿ�� ������.

		BOOL	RemoveCompactData				();	//	����Ʈ ����Ÿ �޸𸮿��� ����..
		BOOL	RemoveCompactDataFile			();	//	����Ʈ ����Ÿ ���� ����.

		FLOAT	C_GetHeight						( FLOAT x , FLOAT y , FLOAT z	);	// �ش� ��ǥ�� �̷��� ���̸� ����س���.
		FLOAT	C_GetHeight						( INT32 nX , INT32 nZ			);	// �ش� ��ǥ�� �̷��� ���̸� ����س���.
		UINT8	C_GetType						( FLOAT x , FLOAT y , FLOAT z	);	// �ٴ��� Ÿ���� ����.
		UINT8	C_GetBlocking					( FLOAT x , FLOAT y , FLOAT z , INT32 eType );	// �ٴ��� Ÿ���� ����.

		ApCompactSegment	* C_GetSegment		( FLOAT x , FLOAT y , FLOAT z );
		ApCompactSegment	* C_GetSegment		( INT32 x , INT32 z );

		UINT8	GetTileCompactType				( INT32 tileindex				);

	// Detail Segment Functions....

		// �ε� ����..
		BOOL	IsLoadedDetailData		() const { if( m_pDetailSectorInfo ) return TRUE ; else return FALSE; }
		BOOL	IsLoadedDetailData		( INT32 nTargetDetail );

		BOOL	LoadDetailData			();	// ������ ����Ÿ �ε�.
		BOOL	SaveDetailData			();	// ������ ����Ÿ ����.

		// ���� ����. Ÿ���� �Ŀ�..
		BOOL	LoadDetailDataGeometry	();	// ��������
		BOOL	LoadDetailDataTile		();	// Ÿ��+���ؽ� Į��
		BOOL	_LoadDetailDataGeometry	( char * pfilename = NULL );	// ��������
		BOOL	_LoadDetailDataTile		( char * pfilename = NULL );	// Ÿ��+���ؽ� Į��
		BOOL	SaveDetailDataGeometry	();
		BOOL	SaveDetailDataTile		();

		inline	void	SetCurrentDetail( BOOL bDetail ) { m_nCurrentDetail = bDetail; }
		
		BOOL	CreateDetailData		( INT32 nTargetDetail , INT32 nDepth , FLOAT fDefaultHeight = 1.0f );	// �ش� ������ ���� ����..
		BOOL	GenerateRoughMap		( INT32 nTargetDetail , INT32 sourcedetail , FLOAT offset ,
										BOOL bHeight = TRUE , BOOL bTile = FALSE , BOOL bVertexColor = FALSE );

		BOOL	RemoveAllDetailData		();	// ������ ����Ÿ �޸𸮿��� ����.
		BOOL	RemoveDetailData		( INT32 nTargetDetail );	// ������ ����Ÿ �޸𸮿��� ����. �ش� �����ϸ�.
		BOOL	RemoveDetailDataFile	();	// ����Ÿ ���ϱ��� ������..

		INT32	D_GetDepth				( INT32 nTargetDetail );
		FLOAT	D_GetStepSize			( INT32 nTargetDetail )
		{
			if( m_pDetailSectorInfo )
				return ( ( m_fxEnd - m_fxStart ) / ( ( float ) m_pDetailSectorInfo->m_nDepth[ nTargetDetail ] ) );
			else
			{
				switch( nTargetDetail )
				{
				case SECTOR_LOWDETAIL	:	return 1600.0f	;	// ������ (2003-11-03 ���� 12:39:59) : ���� �ʿ���.
				case SECTOR_HIGHDETAIL	:	return 400.0f	;
				default					:	return 1.0f		;
				}
			}
		}

		// ���� ���� ��ǵ�..
		ApDetailSegment * D_GetSegment	( INT32 nTargetDetail , INT32 x , INT32 z ,	FLOAT *pfPosX = NULL,	FLOAT *pfPosZ = NULL	);
		ApDetailSegment	* D_GetSegment	( INT32 nTargetDetail , FLOAT x , FLOAT z ,	INT32 *pPosX = NULL ,	INT32 *pPosZ = NULL		);

		FLOAT	D_GetHeight2			( INT32 x , INT32 z );
		FLOAT	D_GetHeight				( INT32 nTargetDetail , FLOAT x , FLOAT z );	// �ش� ��ǥ�� �̷��� ���̸� ����س���.
		ApRGBA	D_GetValue				( INT32 nTargetDetail , FLOAT x , FLOAT z );
		UINT32 *D_GetTile				( INT32 nTargetDetail , FLOAT x , FLOAT z );
		FLOAT	D_GetRoughHeight		( INT32 nTargetDetail , INT32 x , INT32 z );	// �ش� ��ǥ�� �̷��� ���̸� ����س���.

		FLOAT	D_SetHeight				( INT32 nTargetDetail , INT32 x , INT32 z , FLOAT height	);
		ApRGBA	D_SetValue				( INT32 nTargetDetail , INT32 x , INT32 z , ApRGBA value	);
		UINT32 *D_SetTile				( INT32 nTargetDetail , INT32 x , INT32 z , UINT32 firsttexture ,
										UINT32 secondtexture	= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 thirdtexture		= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 fourthtexture	= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 fifthtexture		= ALEF_TEXTURE_NO_TEXTURE	,
										UINT32 sixthtexture		= ALEF_TEXTURE_NO_TEXTURE	);
		
		INT32	D_SetAllTile			( INT32 nTargetDetail , INT32 tileindex					);

		FLOAT	D_GetMinHeight			();

		//////////////////////////////////////////////////////////
		// Inline Functions..
		//////////////////////////////////////////////////////////

		inline BOOL	CreateDetailData		( INT32 nDepth , FLOAT fDefaultHeight = 1.0f	)	{ return CreateDetailData( GetCurrentDetail() , nDepth , fDefaultHeight );	}
		inline BOOL	GenerateRoughMap		( INT32 sourcedetail , FLOAT offset				)	{ return GenerateRoughMap( GetCurrentDetail() , sourcedetail , offset ); }

		inline BOOL	RemoveDetailData		() { return RemoveDetailData( GetCurrentDetail() ); }

		inline INT32	D_GetDepth				() { return D_GetDepth( GetCurrentDetail() ); }
		inline FLOAT	D_GetStepSize			() { return D_GetStepSize( GetCurrentDetail() ); }

		// ���� ���� ��ǵ�..
		inline ApDetailSegment * D_GetSegment	( INT32 x , INT32 z ,	FLOAT *pfPosX = NULL,	FLOAT *pfPosZ = NULL	) { return D_GetSegment( GetCurrentDetail() , x , z , pfPosX , pfPosZ ); }
		inline ApDetailSegment * D_GetSegment	( FLOAT x , FLOAT z ,	INT32 *pPosX = NULL ,	INT32 *pPosZ = NULL		) { return D_GetSegment( GetCurrentDetail() , x , z , pPosX , pPosZ); }

		inline FLOAT	D_GetHeight				( FLOAT x , FLOAT z ) { return D_GetHeight		( GetCurrentDetail() , x , z ); }
		inline ApRGBA	D_GetValue				( FLOAT x , FLOAT z ) { return D_GetValue		( GetCurrentDetail() , x , z ); }
		inline FLOAT	D_GetRoughHeight		( INT32 x , INT32 z ) { return D_GetRoughHeight	( GetCurrentDetail() , x , z ); }

		inline FLOAT	D_SetHeight				( INT32 x , INT32 z , FLOAT height	) { return D_SetHeight( GetCurrentDetail() , x , z , height ); }
		inline ApRGBA	D_SetValue				( INT32 x , INT32 z , ApRGBA value	) { return D_SetValue( GetCurrentDetail() , x , z , value ); }
		inline INT32	D_SetAllTile			( INT32 tileindex					) { return D_SetAllTile( GetCurrentDetail() , tileindex ); }

	// ����Ʈ����/��Ʈ����. �λ��� ������ �ȵ�.
	ApWorldSector();
	virtual ~ApWorldSector();

// Inline Functions...

	static ApmMap * m_pModuleMap;
};

// ����Ʈ ������ ����� �� ���..
bool operator==( ApWorldSector::Dimension &lParam , INT32 nCreatedIndex );

inline int	GetArrayIndexXDWORD	( int index	) { return ( index >> 16		);	}
inline int	GetArrayIndexZDWORD	( int index	) { return ( index &0x0000ffff	);	}

inline FLOAT GET_REAL_HEIGHT_FROM_COMPACT_HEIGHT( UINT16 compactheight )
{
	return ( ALEF_MAX_HEIGHT - ALEF_MIN_HEIGHT ) * ( FLOAT ) compactheight / 65536.0f + ALEF_MIN_HEIGHT;
}

//#define DEBUG_NEW
//#endif

#define	ALEF_WORLD_COMPACT_SECTOR_FILE_NAME_FORMAT		"a%06dx.ma1"
#define	ALEF_WORLD_COMPACT_SECTOR_FILE_WILDCARD_FORMAT	"a*.ma1"
#define COMPACT_SERVER_FORMAT	"c%04d.dat"
#define COMPACT_SERVER_FORMAT_WILDCARD					"server\\c*.dat"

#endif // !defined(AFX_APWORLDSECTOR_H__FA496200_24F6_4F97_8D80_4AAA2462BEA1__INCLUDED_)
