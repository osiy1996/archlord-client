// ApmMap.cpp: implementation of the ApmMap class.
//
//////////////////////////////////////////////////////////////////////

#include <ApmMap/ApmMap.h>
#include <ApBase/MagDebug.h>
#include <math.h>
#include <AuMath/AuMath.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AuIniManager/AuIniManager.h>
#include <AuMD5EnCrypt/AuMD5EnCrypt.h>

#define APMMAP_DEFAULT_SECTOR_REMOVE_DELAY	5000
#define APMMAP_DEFAULT_SECTOR_REMOVE_POOL	1000
#define APMMAP_PROCESS_REMOVE_INTERVAL		1000

static const char g_strINIRegionTemplateSection		[]	= "%d"			;
static const char g_strINIRegionTemplateKeyName		[]	= "Name"		;
static const char g_strINIRegionTemplateKeyParentIndex[]= "ParentIndex"	;
static const char g_strINIRegionTemplateKeyItemSection[]= "ItemSection"	;
static const char g_strINIRegionTemplateKeyType		[]	= "Type"		;
static const char g_strINIRegionTemplateKeyComment	[]	= "Comment"		;
static const char g_strINIRegionTemplateKeyWorldMap	[]	= "WorldMap"	;
static const char g_strINIRegionTemplateKeySkySet	[]	= "SkySet"		;
static const char g_strINIRegionTemplateKeyVDistance[]	= "VisibleDistance";
static const char g_strINIRegionTemplateKeyTVHeight	[]	= "TopViewHeight";
static const char g_strINIRegionTemplateKeyLevelLimit[]	= "LevelLimit"	;
static const char g_strINIRegionTemplateKeyLevelMin[]	= "LevelMin"	;

static const char g_strINIRegionTemplateKeyZoneSrcX	[]	= "ZoneSrcX"	;
static const char g_strINIRegionTemplateKeyZoneSrcZ	[]	= "ZoneSrcZ"	;
static const char g_strINIRegionTemplateKeyZoneHeight[]	= "ZoneHeight"	;
static const char g_strINIRegionTemplateKeyZoneDstX	[]	= "ZoneDstX"	;
static const char g_strINIRegionTemplateKeyZoneDstZ	[]	= "ZoneDstZ"	;
static const char g_strINIRegionTemplateKeyZoneRadius[]	= "ZoneRadius"	;

static const char g_strINIRegionElementSection		[]	= "%d"			;
static const char g_strINIRegionElementIndex		[]	= "Index"		;
static const char g_strINIRegionElementKeyKind		[]	= "Kind"		;
static const char g_strINIRegionElementKeyStartX	[]	= "StartX"		;
static const char g_strINIRegionElementKeyStartZ	[]	= "StartZ"		;
static const char g_strINIRegionElementKeyEndX		[]	= "EndX"		;
static const char g_strINIRegionElementKeyEndZ		[]	= "EndZ"		;

static const char g_strINIRegionTemplateKeyResurrectionX	[]	=	"ResurrectionX"	;
static const char g_strINIRegionTemplateKeyResurrectionZ	[]	=	"ResurrectionZ"	;


#define __IS_ADDED_SECTOR( x )		( ( ( x->m_uQueueOffset & 0xffff0000 ) >> 16 ) == 0x0000	)
#define __GET__SECTOR_OFFSET( x )	( ( ( x->m_uQueueOffset & 0x0000ffff )       )				)
#define	__SET__SECTOR_OFFSET( x , index )	( x->m_uQueueOffset = index % 0xffff )

// �ʿ����� �ִ� ����üũ��.
// ���� ����� 1717 ���� 3232 ������ �����.
static	float	g_fPosInMapStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( 1717 ) ) );
static	float	g_fPosInMapStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( 1717 ) ) );
static	float	g_fPosInMapEndX		= GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( 3333 ) ) );
static	float	g_fPosInMapEndZ		= GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( 3333 ) ) );

inline	bool	IsValidPosition( AuPOS  * pPos )
{
	if( g_fPosInMapStartX < pPos->x && pPos->x < g_fPosInMapEndX &&
		g_fPosInMapStartZ < pPos->z && pPos->z < g_fPosInMapEndZ )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ApmMap::ApmMap()
{
	m_bInitialized		= FALSE	;

	m_bLoadCompactData	= TRUE	;
	m_bLoadDetailData	= FALSE	;


	ApWorldSector::m_pModuleMap = this;

	// ����Ÿ �ʱ�ȭ.
	for( INT32 y = 0 ; y < MAP_WORLD_INDEX_HEIGHT ; ++y  )
		for( INT32 x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x  )
			m_ppSector[ x ][ y ] = NULL;

	// ��� ����Ÿ ���..
		
	SetModuleName("ApmMap");

	EnableIdle(TRUE);

	SetModuleData( sizeof( ApWorldSector			),	SECTOR_DATA		);
//	SetModuleData( sizeof( ApWorldSector::Segment	),	SEGMENT_DATA	);

	// �ݹ� ��� �ʱ�ȭ.
	m_pfSectorLoadCallback		= NULL;
	m_pfSectorDestroyCallback	= NULL;

	m_strMapGeometryDirectory	= NULL;	// Detail Data , Geometry�� �ִ� ��ġ..
	m_strMapMooneeDirectory		= NULL;	// Detail Data , Moonee�� �ִ� ��ġ..
	m_strMapCompactDirectory	= NULL;	// Compact Data�� Ǯ���� ��ġ..
	m_strWorldDirectory			= NULL;	// ����Ʈ ����Ÿ�� ����ž��ִ���ġ..

	// Unpack Buffer �ʱ�ȭ.
	for( int i = 0 ; i < MAP_UNPACK_MANAGER_BUF ; ++ i )
	{
		m_ArrayUnpackBufferCompact	[ i ].nDivisionIndex	= 0		;
		m_ArrayUnpackBufferCompact	[ i ].uLastAccessTime	= 0		;
		m_ArrayUnpackBufferCompact	[ i ].pUnpack			= NULL	;

		m_ArrayUnpackBufferGeometry	[ i ].nDivisionIndex	= 0		;
		m_ArrayUnpackBufferGeometry	[ i ].uLastAccessTime	= 0		;
		m_ArrayUnpackBufferGeometry	[ i ].pUnpack			= NULL	;
		
		m_ArrayUnpackBufferMoonee	[ i ].nDivisionIndex	= 0		;
		m_ArrayUnpackBufferMoonee	[ i ].uLastAccessTime	= 0		;
		m_ArrayUnpackBufferMoonee	[ i ].pUnpack			= NULL	;
	}

	m_lSectorRemoveDelay	= APMMAP_DEFAULT_SECTOR_REMOVE_DELAY	;
	m_lSectorRemovePool		= APMMAP_DEFAULT_SECTOR_REMOVE_POOL		;
	
	m_bAutoLoadData				= FALSE	;
	m_ulPrevRemoveClockCount	= 0		;

	m_nCurrentLoadedSectorCount	= 0;

	//@{ kday 20050823
	// ;)
	m_fptrSkillDbg_Ready = NULL;
	m_fptrSkillDbg_PushBox = NULL;
	m_fptrSkillDbg_PushPos = NULL;
	//@} kday

	m_RegionPerculiarityMap.clear();
}

ApmMap::~ApmMap()
{
	if( m_strMapGeometryDirectory	) delete [] m_strMapGeometryDirectory	;
	if( m_strMapMooneeDirectory		) delete [] m_strMapMooneeDirectory		;
	if( m_strMapCompactDirectory	) delete [] m_strMapCompactDirectory	;
	if( m_strWorldDirectory			) delete [] m_strWorldDirectory			;

	// Ȯ�λ��.. ���� ����Ÿ�� �����ִ��� ������..
#ifdef _DEBUG
	for( INT32 z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
	{
		for( INT32 x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
		{
			// ������ (2004-03-29 ���� 3:10:55) : 
			// �̹� ����־���Ѵ�.
			// ������� ������ ������ ū��..
			//ASSERT( NULL == m_ppSector[ x ][ z ] );
		}
	}
#endif
}

//@{ kday 20050823
// ;)
VOID ApmMap::SetSkillDbgCB( FPTR_SkillDbg_Ready fptrSkillDbg_Ready
						   , FPTR_SkillDbg_PushBox	fptrSkillDbg_PushBox
						   , FPTR_SkillDbg_PushPos	fptrSkillDbg_PushPos)
{
	m_fptrSkillDbg_Ready = fptrSkillDbg_Ready;
	m_fptrSkillDbg_PushBox = fptrSkillDbg_PushBox;
	m_fptrSkillDbg_PushPos = fptrSkillDbg_PushPos;
};
//@} kday

BOOL	ApmMap::Init					(	char *strGeometryDir	,
											char *strMooneeDir		,
											char *strCompactDir		,
											char *strWorldDir		)
{
	//ASSERT( FALSE == m_bInitialized && "ApmMap�� �̹� �ʱ�ȭ �ž��ֽ��ϴ�."							);

	// �ʵ���Ÿ�� �ε��Ѵ�..
	// �����/Ŭ���̾�Ʈ/�������� ����..
	// ���� ��⿡�� ����..

	// �ʸ����ü������ �ƹ��͵� �ε����� �ʴ´�.
	// ��� �� ���·� ����..
	// �ʿ��� ���� ������ ���� ��⿡�� ó���ϵ��� �Ѵ�.

//	INT32 ax;
//	INT32 az;
//
//	INT32 x , z;
//	ApWorldSector * pSector;
//	for( z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; z ++ )
//	{
//		for( x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; x ++ )
//		{
//			ax = ArrayIndexToSectorIndexX( x );
//			az = ArrayIndexToSectorIndexZ( z );
//		}
//	}

	ASSERT( NULL == m_strMapGeometryDirectory	);
	ASSERT( NULL == m_strMapMooneeDirectory		);
	ASSERT( NULL == m_strMapCompactDirectory	);
	ASSERT( NULL == m_strWorldDirectory			);

	INT32	nLength;
	char	strFullPath[ 1024 ];
	char	strDelete[ 1024 ];

	char strCurDir[ 1024 ];
	GetCurrentDirectory( 1024 , strCurDir );
//	TRACE( "���� ���丮 %s\n" , strCurDir );
	wsprintf( strFullPath , "%s\\Map" , strCurDir );
	CreateDirectory( strFullPath , NULL );
	wsprintf( strFullPath , "%s\\Map\\Data" , strCurDir );
	CreateDirectory( strFullPath , NULL );


	// Geometry Directory..
	if( strGeometryDir )
	{
		m_strMapGeometryDirectory	=	new char [ nLength = ( (INT32)strlen( strGeometryDir ) + 1 ) ];
		strncpy( m_strMapGeometryDirectory , strGeometryDir , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	else
	{
		wsprintf( strFullPath , "%s\\Map\\Data\\Geometry" , strCurDir );
		m_strMapGeometryDirectory	=	new char [ nLength = ( (INT32)strlen( strFullPath ) + 1 ) ];
		strncpy( m_strMapGeometryDirectory , strFullPath , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	TRACE( "Geometry Directory - %s\n" , m_strMapGeometryDirectory );

	wsprintf( strDelete , "%s\\*.amf" , m_strMapGeometryDirectory );
	DeleteFile( strDelete );
	
	// Moonee Directory..
	if( strMooneeDir )
	{
		m_strMapMooneeDirectory	=	new char [ nLength = ( (INT32)strlen( strMooneeDir ) + 1 ) ];
		strncpy( m_strMapMooneeDirectory , strMooneeDir , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	else
	{
		wsprintf( strFullPath , "%s\\Map\\Data\\Moonee" , strCurDir );
		m_strMapMooneeDirectory	=	new char [ nLength = ( (INT32)strlen( strFullPath ) + 1 ) ];
		strncpy( m_strMapMooneeDirectory , strFullPath , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	TRACE( "Moonee Directory - %s\n" , m_strMapMooneeDirectory );
	wsprintf( strDelete , "%s\\*.amf" , m_strMapMooneeDirectory );
	DeleteFile( strDelete );
	
	// Compact Directory..
	if( strCompactDir )
	{
		m_strMapCompactDirectory	=	new char [ nLength = ( (INT32)strlen( strCompactDir ) + 1 ) ];
		strncpy( m_strMapCompactDirectory , strCompactDir , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	else
	{
		wsprintf( strFullPath , "%s\\Map\\Data\\Compact" , strCurDir );
		m_strMapCompactDirectory	=	new char [ nLength = ( (INT32)strlen( strFullPath ) + 1 ) ];
		strncpy( m_strMapCompactDirectory , strFullPath , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	TRACE( "Compact Directory - %s\n" , m_strMapCompactDirectory );
	wsprintf( strDelete , "%s\\*.amf" , m_strMapCompactDirectory );
	DeleteFile( strDelete );

	// World Directory..
	if( strWorldDir )
	{
		m_strWorldDirectory	=	new char [ nLength = ( (INT32)strlen( strWorldDir ) + 1 ) ];
		strncpy( m_strWorldDirectory , strWorldDir , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	else
	{
		wsprintf( strFullPath , "%s\\World" , strCurDir );
		m_strWorldDirectory	=	new char [ nLength = ( (INT32)strlen( strFullPath ) + 1 ) ];
		strncpy( m_strWorldDirectory , strFullPath , nLength );
		CreateDirectory( strFullPath , NULL );
	}
	TRACE( "World Directory - %s\n" , m_strWorldDirectory );

	return TRUE;
}

BOOL	ApmMap::CheckBlockingInfo( AuPOS pos , BLOCKINGTYPE eType )
{
	ASSERT( m_bInitialized );
	// �ش� ��ǥ�� �Ҽӵ� ���� �ε����� ���Ѵ�.
	// �ش缽�� �������� �ε����� ���Ͽ� �������� Ȯ���Ѵ�.
	// ����� �����Ѵ�.
	
	ApWorldSector * pSector;
	pSector = GetSector( pos );
	if( pSector == NULL ) return TRUE;
	else return pSector->GetBlocking( pos , eType );

	return FALSE;
}

BOOL	ApmMap::IsPassThis		( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint )
{
	ASSERT( m_bInitialized );
	// �浹üũ - -;

	return FALSE;
}


ApWorldSector *	ApmMap::GetSector		( AuPOS pos , INT32 *px , INT32 *py , INT32 *pz )
{
	ASSERT( m_bInitialized );

	int sectorx , sectorz;

	sectorx = PosToSectorIndexX( pos );
	sectorz = PosToSectorIndexZ( pos );

	// ���� �ε��� ī��
	if( px ) *px = sectorx;
	if( pz ) *pz = sectorz;

	return GetSector( sectorx , 0 , sectorz );

}

ApWorldSector *	ApmMap::GetSector		( INT32 wx , INT32 wy , INT32 wz )	// ���� �ε���.
{
	ASSERT( m_bInitialized );
	// ��� ��ǥ.
	INT32 x , z ;
	x = SectorIndexToArrayIndexX( wx );
	z = SectorIndexToArrayIndexZ( wz );

	// ���� �ʰ����� �˻�..
	if( x < 0 || z < 0 || x >= MAP_WORLD_INDEX_WIDTH || z >= MAP_WORLD_INDEX_HEIGHT ) return NULL;

	//TRACE( "���� ã�� ��û.. (S %d , %d) , (A %d , %d)\n" , wx , wz , x , z );

	// ������ (2004-03-29 ���� 3:01:57) : �ε尡 �������� ���� ���¶��..
	// �������� �ʴ´�.
	// ������ ���̵Ÿ� , �ʿ��ϰ� �ɰ�� , SetupSector�� ó���ɰ��̱� ������..
	// ���⼭ ���̻��� ���������� �ʴ´�..

	if( m_ppSector[ x ][ z ] && !( m_ppSector[ x ][ z ]->m_lStatus & ApWorldSector::STATUS_DELETED ) )
		return m_ppSector[ x ][ z ];
	else
		return NULL;
}

INT32	ApmMap::GetSectorList		( AuPOS pos , float range , ApWorldSector * apSectors[], INT32 size)
{
	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// ������ (2005-09-02 ���� 2:06:19) : 
	// ������ �Ϳ� �ణ�� ������ �ִ°� ���Ƽ�
	// ��Ÿ�� �Ÿ��� �߰���.
	range += MAP_STEPSIZE * 2.0f ;

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;

	ApWorldSector *	pSector;

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	//@{ Jaewon 20050419
	// Optimization!
	//pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );
	lStartX = PosToSectorIndexX( posTemp.x );
	lStartZ = PosToSectorIndexZ( posTemp.z );
	//@} Jaewon

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	//@{ Jaewon 20050419
	// Optimization!
	//pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );
	lEndX = PosToSectorIndexX( posTemp.x );
	lEndZ = PosToSectorIndexZ( posTemp.z );
	//@} Jaewon

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );
	

	// ������ (2003-11-25 ���� 12:28:23) : ������Ʈ ������ ���.. üũ.
	if(	lStartX	>= MAP_WORLD_INDEX_WIDTH || lStartZ >= MAP_WORLD_INDEX_HEIGHT	||
		lEndX	>= MAP_WORLD_INDEX_WIDTH || lEndZ	>= MAP_WORLD_INDEX_HEIGHT	
		//@{ Jaewon 20050427
		// Quick & dirty crash prevention --
		|| lStartX < 0 || lStartZ < 0 || lEndX < 0 || lEndZ < 0
		//@} Jaewon
		)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "GetSectorList ���� ���� �̻�\n");
		AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
		return 0 ;
	}

	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];

			if( !pSector )
				continue;

			if( !( pSector->m_lStatus & ApWorldSector::STATUS_LOADED ) )
				continue;

			apSectors[lCount++] = pSector;
			if (lCount >= size)
				return lCount;
		}
	}

	return lCount;
}

ApWorldSector * ApmMap::GetSectorByArray		( INT32 arrayindexX, INT32 arrayindexY, INT32 arrayindexZ )
{
	ASSERT( m_bInitialized );
	// ���� �ʰ����� �˻�..
	// Y������ ���� üũ���� ����.
	if( arrayindexX < 0 || arrayindexZ < 0 || arrayindexX >= MAP_WORLD_INDEX_WIDTH || arrayindexZ >= MAP_WORLD_INDEX_HEIGHT ) return NULL;


	if( m_ppSector[ arrayindexX ][ arrayindexZ ] &&
		m_ppSector[ arrayindexX ][ arrayindexZ ]->m_lStatus & ApWorldSector::STATUS_LOADED )
		return m_ppSector[ arrayindexX ][ arrayindexZ ];
	else
		return NULL;
}

INT32		ApmMap::GetItemList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size ) // return ����� ����;
{
	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pItem;

	FLOAT	fDistance;
	//@{ Jaewon 20041116
	// to be compatible with .net 2003 & to be faster
	// sqrt( 2 ) -> 1.414f
	FLOAT	fTemp = (FLOAT) ( range / 1.414f );	// ���� �����ϴ� �簢���� ���ϱ� ���� ��
	//@} Jaewon

	ApWorldSector *	pSector;

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;

	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];

			if( !pSector )
				continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			for( pItem = pDimension->pItems; pItem; pItem = pItem->pNext )
			{
				// �����ϴ� �簢�� �ȿ� �ִ��� OK
				if( pItem->pos.x >= pos.x - fTemp &&
					pItem->pos.z >= pos.z - fTemp &&
					pItem->pos.x <= pos.x + fTemp &&
					pItem->pos.z <= pos.z + fTemp )
				{
					if( array && lCount < size )
						array[ lCount ] = pItem->id;

					++lCount ;

					if (lCount >= size)
						return lCount;
				}
				// �����ϴ� �簢�� �ȿ� ������,
				else if( pItem->pos.x >= pos.x - range &&
						 pItem->pos.z >= pos.z - range &&
						 pItem->pos.x <= pos.x + range &&
						 pItem->pos.z <= pos.z + range )
				{
					// �Ÿ� ����ؼ�
					fDistance = AUPOS_DISTANCE_XZ(pItem->pos, pos);

					// �Ÿ��� range���� ������ OK
					if( fDistance <= range + pItem->range )
					{
						if( array && lCount < size )
							array[ lCount ] = pItem->id;

						++lCount ;

						if (lCount >= size)
							return lCount;
					}
				}

			}
		}
	}

	return lCount;
	/*
	INT32 x , z;


	ApWorldSector * pArraySector[ 9 ]	;
	ApWorldSector * pSector				;
	INT32				count = 0;
	bool			bUp = false,bDown = false,bRight = false,bLeft = false;


	pArraySector[ count ++ ] = GetSector( pos , &x , NULL , &z );

	if( pArraySector[ 0 ] == NULL ) return 0;

	AuPOS	posTemp;
	// ���� ��Ȳ ����..

	// ����..
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z - range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bUp = true;
	}

	// ������,,
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ������ �߰�..
		pArraySector[ count ++ ] = pSector;
		bRight = true;
	}

	// ������ ��..
	if( bUp && bRight )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// �Ʒ�
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bDown = true;
	}

	// ������ �Ʒ�.
	if( bDown && bRight )
	{
		pSector = GetSector( x + 1 , 0 , z );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ����..
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x - range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bLeft = true;
	}

	// ���� �Ʒ�
	if( bDown && bLeft )
	{
		pSector = GetSector( x , 0 , z + 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ���� ��.
	if( bUp && bLeft )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}
	
	// ���� �ݸ���..

//	INT32 j ;
	INT32 foundcount = 0;
	float	distance;
	ApWorldSector::IdPos *	pItem;

	for( INT32 i = 0 ; i < count ; ++i )
	{
		for( pItem = pArraySector[ i ]->m_pItems ; pItem ; pItem = pItem->pNext )
		{
			// �Ŀ�.
			// pArraySector[ i ]->m_aItemID[ j ] �� �ݸ��� üũ..

			distance = ( float ) sqrt( 
				( pItem->pos.x - pos.x ) * ( pItem->pos.x - pos.x ) +
				( pItem->pos.z - pos.z ) * ( pItem->pos.z - pos.z ) );

			if( distance <=  range )
			{
				// ����Ʈ�� �߰�.
				array[ foundcount ] = pItem->id;
				++foundcount ;

				// ��� ������ �ʰ�.
				if( foundcount >= size )
					return foundcount;
			}
		}
	}

	return foundcount;
	*/
}

INT32	ApmMap::GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 )
{
	if (!array || size < 1)
		return 0;

	INT32	lDataSize	= 0;

	if (APMMAP_CHAR_TYPE_PC & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetCharList( nDimensionIndex , pos, range, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetCharList( nDimensionIndex , pos, range, array + lDataSize, size - lDataSize, NULL, 0);
	}

	if (APMMAP_CHAR_TYPE_NPC & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetNPCList( nDimensionIndex , pos, range, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetNPCList( nDimensionIndex , pos, range, array + lDataSize, size - lDataSize, NULL, 0);
	}

	if (APMMAP_CHAR_TYPE_MONSTER & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetMonsterList( nDimensionIndex , pos, range, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetMonsterList( nDimensionIndex , pos, range, array + lDataSize, size - lDataSize, NULL, 0);
	}

	return lDataSize;
}

INT32	ApmMap::GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 )
{
	if (!array || size < 1)
		return 0;

	INT32	lDataSize	= 0;

	if (APMMAP_CHAR_TYPE_PC & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetCharList( nDimensionIndex , pos, range, matrix, sinhalfthetaby2, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetCharList( nDimensionIndex , pos, range, matrix, sinhalfthetaby2, array + lDataSize, size - lDataSize, NULL, 0);
	}

	if (APMMAP_CHAR_TYPE_NPC & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetNPCList( nDimensionIndex , pos, range, matrix, sinhalfthetaby2, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetNPCList( nDimensionIndex , pos, range, matrix, sinhalfthetaby2, array + lDataSize, size - lDataSize, NULL, 0);
	}

	if (APMMAP_CHAR_TYPE_MONSTER & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetMonsterList( nDimensionIndex , pos, range, matrix, sinhalfthetaby2, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetMonsterList( nDimensionIndex , pos, range, matrix, sinhalfthetaby2, array + lDataSize, size - lDataSize, NULL, 0);
	}

	return lDataSize;
}

INT32	ApmMap::GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2, BOOL bDbgSkill )
{
	if (!array || size < 1)
		return 0;

	INT32	lDataSize	= 0;

	if (APMMAP_CHAR_TYPE_PC & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetCharList( nDimensionIndex , pos, matrix, box, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetCharList( nDimensionIndex , pos, matrix, box, array + lDataSize, size - lDataSize, NULL, 0);
	}

	if (APMMAP_CHAR_TYPE_NPC & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetNPCList( nDimensionIndex , pos, matrix, box, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize);
		else
			lDataSize	+= GetNPCList( nDimensionIndex , pos, matrix, box, array + lDataSize, size - lDataSize, NULL, 0);
	}

	if (APMMAP_CHAR_TYPE_MONSTER & lCharType)
	{
		if (array2 && size > 0)
			lDataSize	+= GetMonsterList( nDimensionIndex , pos, matrix, box, array + lDataSize, size - lDataSize, array2 + lDataSize, size2 - lDataSize, bDbgSkill);
		else
			lDataSize	+= GetMonsterList( nDimensionIndex , pos, matrix, box, array + lDataSize, size - lDataSize, NULL, 0, bDbgSkill);
	}

	return lDataSize;
}

INT32		ApmMap::GetCharList			( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2) // �簢�� �ȿ���. return ����� ����;
{
	PROFILE("ApmMap::GetCharList() 1");

	ASSERT( m_bInitialized );

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	//INT32	lTempD;
	//INT32	lTemp;
	AuPOS	posTemp;
	AuPOS	posTemp2;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pUser;

	AuV3D *	pvAt = &matrix->at;
	AuMATRIX	matInverse;

	FLOAT	fRange;

	ApWorldSector *	pSector;

	// ���� inverse matrix�� ���Ѵ�.
	AuMath::MatrixInverse( &matInverse , matrix );

	// Box�� �밢�� ũ�⸦ ���Ѵ�.
	AuMath::V3DSubtract( &posTemp , &box->sup , &box->inf );
	fRange = AuMath::V3DLength( &posTemp );

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= fRange;
	posTemp.z -= fRange;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += fRange;
	posTemp.z += fRange;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;

	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockUsers );

			for( pUser = pDimension->pUsers; pUser; pUser = pUser->pNext )
			{
				// pos�� �߽����� ���ġ�Ѵ�.
				AuMath::V3DSubtract( &posTemp , &pUser->pos , &pos);
				AuMath::V3DTransform( &posTemp2 , &posTemp , &matInverse );

				posTemp2.y = 0;

				/*
				lTempD = 0;

				lTemp = posTemp2.x - box->inf.x;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.x - box->sup.x) * (posTemp2.x - box->sup.x);

				lTemp = posTemp2.y - box->inf.y;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.y - box->sup.y) * (posTemp2.y - box->sup.y);

				lTemp = posTemp2.z - box->inf.z;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.z - box->sup.z) * (posTemp2.z - box->sup.z);
				*/

				if( AUTEST_POS_IN_BOX( posTemp2 , *box ))
				//if (lTempD <= pUser->range * pUser->range)
				{
					if( array && lCount < size )
					{
						array[ lCount ] = pUser->id;

						if (array2 && lCount < size2)
							array2[ lCount ] = (INT32)pUser->id2;
					}

					++lCount ;

					if (lCount >= size)
						return lCount;
				}
			}
		}
	}

	return lCount;
}

INT32		ApmMap::GetCharList		( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ) // return ����� ����;
{
	PROFILE("ApmMap::GetCharList() 2");

	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;
	AuPOS	posTemp2;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pUser;

	AuV3D *	pvAt = &matrix->at;
	AuV3D	vNormalized;

	FLOAT	fDistance;
	FLOAT	fDistance2;
	//@{ Jaewon 20041116
	// to be compatible with .net 2003 & to be faster
	// sqrt( 2 ) -> 1.414f
	FLOAT	fTemp = (FLOAT) ( range / 1.414f );	// ���� �����ϴ� �簢���� ���ϱ� ���� ��
	//@} Jaewon

	ApWorldSector *	pSector;

	// ���� vector�� normalize �Ѵ�.
	AuMath::V3DNormalize( &vNormalized , pvAt );

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;

	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//ApAutoReaderLock	csLock( pSector->m_RWLock );
			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockUsers );

			for( pUser = pDimension->pUsers; pUser; pUser = pUser->pNext )
			{
				// �����ϴ� �簢�� �ȿ� ������,
				if( pUser->pos.x >= pos.x - range &&
						 pUser->pos.z >= pos.z - range &&
						 pUser->pos.x <= pos.x + range &&
						 pUser->pos.z <= pos.z + range )
				{
					AuMath::V3DSubtract( &posTemp , &pUser->pos , &pos );

					// �Ÿ� ����ؼ�
					fDistance = ( FLOAT ) AuMath::V3DLength( &posTemp );

					// �Ÿ��� range���� ������
					if( fDistance <= range + pUser->range )
					{
						AuMath::V3DScale( &posTemp2 , &vNormalized , fDistance );

						fDistance2 = AUPOS_DISTANCE_XZ(posTemp2, posTemp);
						if( fDistance2 <= sinhalfthetaby2 * fDistance )
						{
							if( array && lCount < size )
							{
								array[ lCount ] = pUser->id;

								if (array2 && lCount < size2)
									array2[ lCount ] = (INT32)pUser->id2;
							}

							++lCount ;

							if (lCount >= size)
								return lCount;
						}
					}
				}

			}
		}
	}

	return lCount;
}

INT32		ApmMap::GetCharList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ) // return ����� ����;
{
	PROFILE("ApmMap::GetCharList() 3");

	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pUser;

	FLOAT	fDistance;
	//@{ Jaewon 20041116
	// to be compatible with .net 2003 & to be faster
	// sqrt( 2 ) -> 1.414f
	FLOAT	fTemp = (FLOAT) ( range / 1.414f );	// ���� �����ϴ� �簢���� ���ϱ� ���� ��
	//@} Jaewon

	ApWorldSector *	pSector;

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;
	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//ApAutoReaderLock	csLock( pSector->m_RWLock );
			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockUsers );

			for( pUser = pDimension->pUsers; pUser; pUser = pUser->pNext )
			{
				// �����ϴ� �簢�� �ȿ� �ִ��� OK
				if( pUser->pos.x >= pos.x - fTemp &&
					pUser->pos.z >= pos.z - fTemp &&
					pUser->pos.x <= pos.x + fTemp &&
					pUser->pos.z <= pos.z + fTemp )
				{
					if( array && lCount < size )
					{
						array[ lCount ] = pUser->id;

						if (array2 && lCount < size2)
							array2[ lCount ] = (INT32)pUser->id2;
					}

					++lCount ;

					if (lCount >= size)
						return lCount;
				}
				// �����ϴ� �簢�� �ȿ� ������,
				else if( pUser->pos.x >= pos.x - range &&
						 pUser->pos.z >= pos.z - range &&
						 pUser->pos.x <= pos.x + range &&
						 pUser->pos.z <= pos.z + range )
				{
					// �Ÿ� ����ؼ�
					fDistance = AUPOS_DISTANCE_XZ(pUser->pos, pos);

					// �Ÿ��� range���� ������ OK
					if( fDistance <= range + pUser->range )
					{
						if( array && lCount < size )
						{
							array[ lCount ] = pUser->id;

							if (array2 && lCount < size2)
								array2[ lCount ] = (INT32)pUser->id2;
						}

						++lCount ;

						if (lCount >= size)
							return lCount;
					}
				}

			}
		}
	}

	return lCount;
	/*
	INT32 x , z ;
	ApWorldSector * pArraySector[ 9 ]	;
	ApWorldSector * pSector				;
	INT32				count = 0;
	bool			bUp = false,bDown = false,bRight = false,bLeft = false;


	pArraySector[ count ++ ] = GetSector( pos , &x , NULL , &z );

	if( pArraySector[ 0 ] == NULL ) return 0;

	AuPOS	posTemp;
	// ���� ��Ȳ ����..

	// ����..
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z - range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bUp = true;
	}

	// ������,,
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ������ �߰�..
		pArraySector[ count ++ ] = pSector;
		bRight = true;
	}

	// ������ ��..
	if( bUp && bRight )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// �Ʒ�
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bDown = true;
	}

	// ������ �Ʒ�.
	if( bDown && bRight )
	{
		pSector = GetSector( x + 1 , 0 , z );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ����..
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x - range	;

	pSector	= GetSector( pos , &x , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bLeft = true;
	}

	// ���� �Ʒ�
	if( bDown && bLeft )
	{
		pSector = GetSector( x , 0 , z + 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ���� ��.
	if( bUp && bLeft )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}
	
	// ���� �ݸ���..

	INT32 j ;
	INT32 foundcount = 0;
	float	distance;
	for( INT32 i = 0 ; i < count ; i ++ )
	{
		for( j = 0 ; j < pArraySector[ i ]->m_nUserID ; j++ )
		{
			// �Ŀ�.
			// pArraySector[ i ]->m_aItemID[ j ] �� �ݸ��� üũ..

			distance = ( float ) sqrt( 
				( pArraySector[ i ]->m_aUserID[ j ].pos.x - pos.x ) * ( pArraySector[ i ]->m_aUserID[ j ].pos.x - pos.x ) +
				( pArraySector[ i ]->m_aUserID[ j ].pos.z - pos.z ) * ( pArraySector[ i ]->m_aUserID[ j ].pos.z - pos.z ) );

			if( distance <=  range )
			{
				// ����Ʈ�� �߰�.
				array[ foundcount ] = pArraySector[ i ]->m_aUserID[ j ].id;
				foundcount ++;

				// ��� ������ �ʰ�.
				if( foundcount >= size )
					return foundcount;
			}
		}
	}
	return foundcount;
	*/
}

INT32		ApmMap::GetNPCList			( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ) // �簢�� �ȿ���. return ����� ����;
{
	PROFILE("ApmMap::GetNPCList() 1");

	ASSERT( m_bInitialized );

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	//INT32	lTempD;
	//INT32	lTemp;
	AuPOS	posTemp;
	AuPOS	posTemp2;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pNPC;

	AuV3D *	pvAt = &matrix->at;
	AuMATRIX	matInverse;

	FLOAT	fRange;

	ApWorldSector *	pSector;

	// ���� inverse matrix�� ���Ѵ�.
	AuMath::MatrixInverse( &matInverse , matrix );

	// Box�� �밢�� ũ�⸦ ���Ѵ�.
	AuMath::V3DSubtract( &posTemp , &box->sup , &box->inf );
	fRange = AuMath::V3DLength( &posTemp );

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= fRange;
	posTemp.z -= fRange;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += fRange;
	posTemp.z += fRange;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;

	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//ApAutoReaderLock	csLock( pSector->m_RWLock );
			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockNPCs );

			for( pNPC = pDimension->pNPCs; pNPC; pNPC = pNPC->pNext )
			{
				// pos�� �߽����� ���ġ�Ѵ�.
				AuMath::V3DSubtract( &posTemp , &pNPC->pos , &pos);
				AuMath::V3DTransform( &posTemp2 , &posTemp , &matInverse );

				posTemp2.y = 0;

				/*
				lTempD = 0;

				lTemp = posTemp2.x - box->inf.x;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.x - box->sup.x) * (posTemp2.x - box->sup.x);

				lTemp = posTemp2.y - box->inf.y;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.y - box->sup.y) * (posTemp2.y - box->sup.y);

				lTemp = posTemp2.z - box->inf.z;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.z - box->sup.z) * (posTemp2.z - box->sup.z);
				*/

				if( AUTEST_POS_IN_BOX( posTemp2 , *box ))
				//if (lTempD <= pUser->range * pUser->range)
				{
					if( array && lCount < size )
					{
						array[ lCount ] = pNPC->id;

						if (array2 && lCount < size2)
							array2[ lCount ] = (INT32)pNPC->id2;
					}

					++lCount ;

					if (lCount >= size)
						return lCount;
				}
			}
		}
	}

	return lCount;
}

INT32		ApmMap::GetNPCList		( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ) // return ����� ����;
{
	PROFILE("ApmMap::GetNPCList() 2");

	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;
	AuPOS	posTemp2;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pNPC;

	AuV3D *	pvAt = &matrix->at;
	AuV3D	vNormalized;

	FLOAT	fDistance;
	FLOAT	fDistance2;
	//@{ Jaewon 20041116
	// to be compatible with .net 2003 & to be faster
	// sqrt( 2 ) -> 1.414f
	FLOAT	fTemp = (FLOAT) ( range / 1.414f );	// ���� �����ϴ� �簢���� ���ϱ� ���� ��
	//@} Jaewon

	ApWorldSector *	pSector;

	// ���� vector�� normalize �Ѵ�.
	AuMath::V3DNormalize( &vNormalized , pvAt );

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;

	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//ApAutoReaderLock	csLock( pSector->m_RWLock );
			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockNPCs );

			for( pNPC = pDimension->pNPCs; pNPC; pNPC = pNPC->pNext )
			{
				// �����ϴ� �簢�� �ȿ� ������,
				if( pNPC->pos.x >= pos.x - range &&
						 pNPC->pos.z >= pos.z - range &&
						 pNPC->pos.x <= pos.x + range &&
						 pNPC->pos.z <= pos.z + range )
				{
					AuMath::V3DSubtract( &posTemp , &pNPC->pos , &pos );

					// �Ÿ� ����ؼ�
					fDistance = ( FLOAT ) AuMath::V3DLength( &posTemp );

					// �Ÿ��� range���� ������
					if( fDistance <= range + pNPC->range )
					{
						AuMath::V3DScale( &posTemp2 , &vNormalized , fDistance );

						fDistance2 = AUPOS_DISTANCE_XZ(posTemp2, posTemp);
						if( fDistance2 <= sinhalfthetaby2 * fDistance )
						{
							if( array && lCount < size )
							{
								array[ lCount ] = pNPC->id;

								if (array2 && lCount < size2)
									array2[ lCount ] = (INT32)pNPC->id2;
							}

							++lCount ;

							if (lCount >= size)
								return lCount;
						}
					}
				}

			}
		}
	}

	return lCount;
}

INT32		ApmMap::GetNPCList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ) // return ����� ����;
{
	PROFILE("ApmMap::GetNPCList() 3");

	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pNPC;

	FLOAT	fDistance;
	//@{ Jaewon 20041116
	// to be compatible with .net 2003 & to be faster
	// sqrt( 2 ) -> 1.414f
	FLOAT	fTemp = (FLOAT) ( range / 1.414f );	// ���� �����ϴ� �簢���� ���ϱ� ���� ��
	//@} Jaewon

	ApWorldSector *	pSector;

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;
	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//ApAutoReaderLock	csLock( pSector->m_RWLock );
			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockNPCs );

			for( pNPC = pDimension->pNPCs; pNPC; pNPC = pNPC->pNext )
			{
				// �����ϴ� �簢�� �ȿ� �ִ��� OK
				if( pNPC->pos.x >= pos.x - fTemp &&
					pNPC->pos.z >= pos.z - fTemp &&
					pNPC->pos.x <= pos.x + fTemp &&
					pNPC->pos.z <= pos.z + fTemp )
				{
					if( array && lCount < size )
					{
						array[ lCount ] = pNPC->id;

						if (array2 && lCount < size2)
							array2[ lCount ] = (INT32)pNPC->id2;
					}

					++lCount ;

					if (lCount >= size)
						return lCount;
				}
				// �����ϴ� �簢�� �ȿ� ������,
				else if( pNPC->pos.x >= pos.x - range &&
						 pNPC->pos.z >= pos.z - range &&
						 pNPC->pos.x <= pos.x + range &&
						 pNPC->pos.z <= pos.z + range )
				{
					// �Ÿ� ����ؼ�
					fDistance = AUPOS_DISTANCE_XZ(pNPC->pos, pos);

					// �Ÿ��� range���� ������ OK
					if( fDistance <= range + pNPC->range )
					{
						if( array && lCount < size )
						{
							array[ lCount ] = pNPC->id;

							if (array2 && lCount < size2)
								array2[ lCount ] = (INT32)pNPC->id2;
						}

						++lCount ;

						if (lCount >= size)
							return lCount;
					}
				}

			}
		}
	}

	return lCount;
	/*
	INT32 x , z ;
	ApWorldSector * pArraySector[ 9 ]	;
	ApWorldSector * pSector				;
	INT32				count = 0;
	bool			bUp = false,bDown = false,bRight = false,bLeft = false;


	pArraySector[ count ++ ] = GetSector( pos , &x , NULL , &z );

	if( pArraySector[ 0 ] == NULL ) return 0;

	AuPOS	posTemp;
	// ���� ��Ȳ ����..

	// ����..
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z - range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bUp = true;
	}

	// ������,,
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ������ �߰�..
		pArraySector[ count ++ ] = pSector;
		bRight = true;
	}

	// ������ ��..
	if( bUp && bRight )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// �Ʒ�
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bDown = true;
	}

	// ������ �Ʒ�.
	if( bDown && bRight )
	{
		pSector = GetSector( x + 1 , 0 , z );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ����..
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x - range	;

	pSector	= GetSector( pos , &x , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bLeft = true;
	}

	// ���� �Ʒ�
	if( bDown && bLeft )
	{
		pSector = GetSector( x , 0 , z + 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ���� ��.
	if( bUp && bLeft )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}
	
	// ���� �ݸ���..

	INT32 j ;
	INT32 foundcount = 0;
	float	distance;
	for( INT32 i = 0 ; i < count ; i ++ )
	{
		for( j = 0 ; j < pArraySector[ i ]->m_nUserID ; j++ )
		{
			// �Ŀ�.
			// pArraySector[ i ]->m_aItemID[ j ] �� �ݸ��� üũ..

			distance = ( float ) sqrt( 
				( pArraySector[ i ]->m_aUserID[ j ].pos.x - pos.x ) * ( pArraySector[ i ]->m_aUserID[ j ].pos.x - pos.x ) +
				( pArraySector[ i ]->m_aUserID[ j ].pos.z - pos.z ) * ( pArraySector[ i ]->m_aUserID[ j ].pos.z - pos.z ) );

			if( distance <=  range )
			{
				// ����Ʈ�� �߰�.
				array[ foundcount ] = pArraySector[ i ]->m_aUserID[ j ].id;
				foundcount ++;

				// ��� ������ �ʰ�.
				if( foundcount >= size )
					return foundcount;
			}
		}
	}
	return foundcount;
	*/
}

INT32		ApmMap::GetMonsterList			( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2, BOOL bDbgSkill ) // �簢�� �ȿ���. return ����� ����;
{
	PROFILE("ApmMap::GetMonsterList() 1");

	ASSERT( m_bInitialized );

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	//INT32	lTempD;
	//INT32	lTemp;
	AuPOS	posTemp;
	AuPOS	posTemp2;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pMonster;

	AuV3D *	pvAt = &matrix->at;
	AuMATRIX	matInverse;

	FLOAT	fRange;

	ApWorldSector *	pSector;

	// ���� inverse matrix�� ���Ѵ�.
	AuMath::MatrixInverse( &matInverse , matrix );

	// Box�� �밢�� ũ�⸦ ���Ѵ�.
	AuMath::V3DSubtract( &posTemp , &box->sup , &box->inf );
	fRange = AuMath::V3DLength( &posTemp );

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= fRange;
	posTemp.z -= fRange;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	//@{ kday 20050822
	// ;)
	if( bDbgSkill && m_fptrSkillDbg_Ready )
	{
		m_fptrSkillDbg_Ready( pSector, matrix, box, &pos );
	}
	//@} kday

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += fRange;
	posTemp.z += fRange;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;
	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//ApAutoReaderLock	csLock( pSector->m_RWLock );
			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			//@{ kday 20050822
			// ;)
			if( bDbgSkill && m_fptrSkillDbg_PushBox )
			{
				AuPOS suf = {pSector->m_fxEnd, 200.f, pSector->m_fzEnd};
				AuPOS inf = {pSector->m_fxStart, -200.f, pSector->m_fzStart};
				m_fptrSkillDbg_PushBox(suf, inf);
			}
			//@} kday

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockMonsters );

			for( pMonster = pDimension->pMonsters; pMonster; pMonster = pMonster->pNext )
			{
				
				//@{ kday 20050822
				// ;)
				if( bDbgSkill && m_fptrSkillDbg_PushPos )
				{
					m_fptrSkillDbg_PushPos(pMonster->pos);
				}
				//@} kday
				// pos�� �߽����� ���ġ�Ѵ�.
				AuMath::V3DSubtract( &posTemp , &pMonster->pos , &pos);
				AuMath::V3DTransform( &posTemp2 , &posTemp , &matInverse );

				posTemp2.y = 0;

				/*
				lTempD = 0;

				lTemp = posTemp2.x - box->inf.x;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.x - box->sup.x) * (posTemp2.x - box->sup.x);

				lTemp = posTemp2.y - box->inf.y;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.y - box->sup.y) * (posTemp2.y - box->sup.y);

				lTemp = posTemp2.z - box->inf.z;
				if (lTemp < 0)
					lTempD += lTemp * lTemp;
				else if (lTemp > 0)
					lTempD += (posTemp2.z - box->sup.z) * (posTemp2.z - box->sup.z);
				*/

				if( AUTEST_POS_IN_BOX( posTemp2 , *box ))
				//if (lTempD <= pUser->range * pUser->range)
				{
					if( array && lCount < size )
					{
						array[ lCount ] = pMonster->id;

						if (array2 && lCount < size2)
							array2[ lCount ] = (INT32)pMonster->id2;
					}

					++lCount ;

					if (lCount >= size)
						return lCount;
				}
			}
		}
	}

	return lCount;
}

INT32		ApmMap::GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ) // return ����� ����;
{
	PROFILE("ApmMap::GetMonsterList() 2");

	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;
	AuPOS	posTemp2;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pMonster;

	AuV3D *	pvAt = &matrix->at;
	AuV3D	vNormalized;

	FLOAT	fDistance;
	FLOAT	fDistance2;
	//@{ Jaewon 20041116
	// to be compatible with .net 2003 & to be faster
	// sqrt( 2 ) -> 1.414f
	FLOAT	fTemp = (FLOAT) ( range / 1.414f );	// ���� �����ϴ� �簢���� ���ϱ� ���� ��
	//@} Jaewon

	ApWorldSector *	pSector;

	// ���� vector�� normalize �Ѵ�.
	AuMath::V3DNormalize( &vNormalized , pvAt );

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;
	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockMonsters );

			for( pMonster = pDimension->pMonsters; pMonster; pMonster = pMonster->pNext )
			{
				// �����ϴ� �簢�� �ȿ� ������,
				if( pMonster->pos.x >= pos.x - range &&
						 pMonster->pos.z >= pos.z - range &&
						 pMonster->pos.x <= pos.x + range &&
						 pMonster->pos.z <= pos.z + range )
				{
					AuMath::V3DSubtract( &posTemp , &pMonster->pos , &pos );

					// �Ÿ� ����ؼ�
					fDistance = ( FLOAT ) AuMath::V3DLength( &posTemp );

					// �Ÿ��� range���� ������
					if( fDistance <= range + pMonster->range )
					{
						AuMath::V3DScale( &posTemp2 , &vNormalized , fDistance );

						fDistance2 = AUPOS_DISTANCE_XZ(posTemp2, posTemp);
						if( fDistance2 <= sinhalfthetaby2 * fDistance )
						{
							if( array && lCount < size )
							{
								array[ lCount ] = pMonster->id;

								if (array2 && lCount < size2)
									array2[ lCount ] = (INT32)pMonster->id2;
							}

							++lCount ;

							if (lCount >= size)
								return lCount;
						}
					}
				}

			}
		}
	}

	return lCount;
}

INT32		ApmMap::GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ) // return ����� ����;
{
	PROFILE("ApmMap::GetMonsterList() 3");

	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	if (!array || size < 1)
		return 0;

	INT32	lX , lZ;
	INT32	lStartX , lEndX;
	INT32	lStartZ , lEndZ;
	INT32	lCount = 0;
	AuPOS	posTemp;
//	INT32	lIndex;
	ApWorldSector::IdPos *	pMonster;

	FLOAT	fDistance;
	//@{ Jaewon 20041116
	// to be compatible with .net 2003 & to be faster
	// sqrt( 2 ) -> 1.414f
	FLOAT	fTemp = (FLOAT) ( range / 1.414f );	// ���� �����ϴ� �簢���� ���ϱ� ���� ��
	//@} Jaewon

	ApWorldSector *	pSector;

	// Start Index�� End Index�� ���� X,Z ������ ���Ѵ�.
	posTemp = pos;
	posTemp.x -= range;
	posTemp.z -= range;

	pSector = GetSector( posTemp , &lStartX , NULL , &lStartZ );

	lStartX = SectorIndexToArrayIndexX( lStartX );
	lStartZ = SectorIndexToArrayIndexZ( lStartZ );

	posTemp = pos;
	posTemp.x += range;
	posTemp.z += range;

	pSector = GetSector( posTemp , &lEndX , NULL , &lEndZ );

	lEndX = SectorIndexToArrayIndexX( lEndX );
	lEndZ = SectorIndexToArrayIndexZ( lEndZ );

	ApWorldSector::Dimension	* pDimension;
	// ���� ���ԵǴ� ��� Sector�� ���ؼ�
	for( lX = lStartX ; lX <= lEndX ; ++lX )
	{
		for( lZ = lStartZ ; lZ <= lEndZ ; ++lZ )
		{
			pSector = m_ppSector[ lX ][ lZ ];
			if( !pSector )
				continue;

			//AuAutoLock	csLock(pSector->m_Mutex);
			//if (!csLock.Result()) continue;

			pDimension = pSector->GetDimension( nDimensionIndex );

			if( !pDimension ) continue;

			ApAutoReaderLock	csLock( pDimension->lockMonsters );

			for( pMonster = pDimension->pMonsters; pMonster; pMonster = pMonster->pNext )
			{
				// �����ϴ� �簢�� �ȿ� �ִ��� OK
				if( pMonster->pos.x >= pos.x - fTemp &&
					pMonster->pos.z >= pos.z - fTemp &&
					pMonster->pos.x <= pos.x + fTemp &&
					pMonster->pos.z <= pos.z + fTemp )
				{
					if( array && lCount < size )
					{
						array[ lCount ] = pMonster->id;

						if (array2 && lCount < size2)
							array2[ lCount ] = (INT32)pMonster->id2;
					}

					++lCount ;

					if (lCount >= size)
						return lCount;
				}
				// �����ϴ� �簢�� �ȿ� ������,
				else if( pMonster->pos.x >= pos.x - range &&
						 pMonster->pos.z >= pos.z - range &&
						 pMonster->pos.x <= pos.x + range &&
						 pMonster->pos.z <= pos.z + range )
				{
					// �Ÿ� ����ؼ�
					fDistance = AUPOS_DISTANCE_XZ(pMonster->pos, pos);

					// �Ÿ��� range���� ������ OK
					if( fDistance <= range + pMonster->range )
					{
						if( array && lCount < size )
						{
							array[ lCount ] = pMonster->id;

							if (array2 && lCount < size2)
								array2[ lCount ] = (INT32)pMonster->id2;
						}

						++lCount ;

						if (lCount >= size)
							return lCount;
					}
				}

			}
		}
	}

	return lCount;
	/*
	INT32 x , z ;
	ApWorldSector * pArraySector[ 9 ]	;
	ApWorldSector * pSector				;
	INT32				count = 0;
	bool			bUp = false,bDown = false,bRight = false,bLeft = false;


	pArraySector[ count ++ ] = GetSector( pos , &x , NULL , &z );

	if( pArraySector[ 0 ] == NULL ) return 0;

	AuPOS	posTemp;
	// ���� ��Ȳ ����..

	// ����..
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z - range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bUp = true;
	}

	// ������,,
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ������ �߰�..
		pArraySector[ count ++ ] = pSector;
		bRight = true;
	}

	// ������ ��..
	if( bUp && bRight )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// �Ʒ�
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bDown = true;
	}

	// ������ �Ʒ�.
	if( bDown && bRight )
	{
		pSector = GetSector( x + 1 , 0 , z );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ����..
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x - range	;

	pSector	= GetSector( pos , &x , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bLeft = true;
	}

	// ���� �Ʒ�
	if( bDown && bLeft )
	{
		pSector = GetSector( x , 0 , z + 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ���� ��.
	if( bUp && bLeft )
	{
		pSector = GetSector( x , 0 , z - 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}
	
	// ���� �ݸ���..

	INT32 j ;
	INT32 foundcount = 0;
	float	distance;
	for( INT32 i = 0 ; i < count ; i ++ )
	{
		for( j = 0 ; j < pArraySector[ i ]->m_nUserID ; j++ )
		{
			// �Ŀ�.
			// pArraySector[ i ]->m_aItemID[ j ] �� �ݸ��� üũ..

			distance = ( float ) sqrt( 
				( pArraySector[ i ]->m_aUserID[ j ].pos.x - pos.x ) * ( pArraySector[ i ]->m_aUserID[ j ].pos.x - pos.x ) +
				( pArraySector[ i ]->m_aUserID[ j ].pos.z - pos.z ) * ( pArraySector[ i ]->m_aUserID[ j ].pos.z - pos.z ) );

			if( distance <=  range )
			{
				// ����Ʈ�� �߰�.
				array[ foundcount ] = pArraySector[ i ]->m_aUserID[ j ].id;
				foundcount ++;

				// ��� ������ �ʰ�.
				if( foundcount >= size )
					return foundcount;
			}
		}
	}
	return foundcount;
	*/
}


INT32		ApmMap::GetObjectList	( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size ) // return ����� ����;
{
	ASSERT( m_bInitialized );
	// range�� ������ ���̸� ����� �ȵȴ�.

	// pos �� ���� �߽����� �ؼ�.. �������Ϳ��� range�� ���ԵŴ��� üũ�Ѵ�.

	INT32 x , z;


	ApWorldSector * pArraySector[ 9 ]	;
	ApWorldSector * pSector				;
	INT32				count = 0;
	bool			bUp = false,bDown = false,bRight = false,bLeft = false;
	ApWorldSector::IdPos *	pObject;


	pArraySector[ count ++ ] = GetSector( pos , &x , NULL , &z );

	if( pArraySector[ 0 ] == NULL ) return 0;

	AuPOS	posTemp;
	// ���� ��Ȳ ����..

	// ����..
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z - range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bUp = true;
	}

	// ������,,
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ������ �߰�..
		pArraySector[ count ++ ] = pSector;
		bRight = true;
	}

	// ������ ��..
	if( bUp && bRight )
	{
		pSector = GetSector( x , 0 , z + 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// �Ʒ�
	posTemp		=	pos					;
	posTemp.z	=	posTemp.z + range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bDown = true;
	}

	// ������ �Ʒ�.
	if( bDown && bRight )
	{
		pSector = GetSector( x + 1 , 0 , z );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ����..
	posTemp		=	pos					;
	posTemp.x	=	posTemp.x - range	;

	pSector	= GetSector( pos , &x , NULL , &z );
	if( pSector && pSector != pArraySector[ 0 ] )
	{
		// ���� �߰�..
		pArraySector[ count ++ ] = pSector;
		bLeft = true;
	}

	// ���� �Ʒ�
	if( bDown && bLeft )
	{
		pSector = GetSector( x , 0 , z + 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}

	// ���� ��.
	if( bUp && bLeft )
	{
		pSector = GetSector( x , 0 , z + 1 );
		if( pSector ) pArraySector[ count ++ ]  = pSector;
	}
	
	// ���� �ݸ���..
	
//	INT32 j ;
	INT32 foundcount = 0;
	float	distance;
	ApWorldSector::Dimension	* pDimension;
	for( INT32 i = 0 ; i < count ; ++i  )
	{
		pDimension = pArraySector[ i ]->GetDimension( nDimensionIndex );

		if( !pDimension ) continue;

		ApAutoReaderLock	csLock( pDimension->lockObjects );

		for( pObject = pDimension->pObjects ; pObject ; pObject = pObject->pNext )
		{
			// �Ŀ�.
			// pArraySector[ i ]->m_aItemID[ j ] �� �ݸ��� üũ..

			distance = AUPOS_DISTANCE_XZ(pObject->pos, pos);

			if( distance <=  range )
			{
				// ����Ʈ�� �߰�.
				array[ foundcount ] = pObject->id;
				++foundcount ;

				// ��� ������ �ʰ�.
				if( foundcount >= size )
					return count;
			}
		}
	}
	return foundcount;
}

ApWorldSector *	ApmMap::SetupSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z )
{
	ASSERT( m_bInitialized );
	// Y ���ʹ� ���� �����Ѵ�.
	// ���� �ε����� ��� �ε����� �����Ѵ�.
	INT32			arrayIndexX = SectorIndexToArrayIndexX( sectorindex_x )	;
	INT32			arrayIndexZ = SectorIndexToArrayIndexZ( sectorindex_z )	;
	ApWorldSector *	pSector													;

	if (arrayIndexX < 0 || arrayIndexX >= MAP_WORLD_INDEX_WIDTH || arrayIndexZ < 0 || arrayIndexZ >= MAP_WORLD_INDEX_HEIGHT)
		return NULL;

	// ������ üŷ.
	ASSERT( arrayIndexX	>=	0						);
	ASSERT( arrayIndexZ	>=	0						);
	ASSERT( arrayIndexX	<	MAP_WORLD_INDEX_WIDTH	);
	ASSERT( arrayIndexZ	<	MAP_WORLD_INDEX_HEIGHT	);

	// �̹� �ִ°��.. �ΰ����� ��Ȳ���� ������.
	// 1, �׳� SEtup�� �ι� �Ҹ��°��..
	// 2, Delete List�� �ö� �ִ°��..
	//
	// 1���� ���� �׳� ������ ����.
	// 2���� ��쿡�� ����Ʈ �÷��׸� ���� �������� ������.
	pSector = m_ppSector[ arrayIndexX ][ arrayIndexZ ];
	if( pSector )
	{
		// �ε��Ȱ� ����.. ���� ����, �׳� return ����.
		//DeleteSector( m_ppSector[ arrayIndexX ][ arrayIndexZ ] );

		ApAutoWriterLock	lock(pSector->m_RWLock);
		//if (!lock.Result()) return NULL;

		// ����Ʈ �÷��װ� �ִ°��.. �÷��� ����..
		if (pSector->m_lStatus & ApWorldSector::STATUS_DELETED)
		{
			m_csAdminSectorRemove.RemoveObject((INT_PTR) pSector);
			pSector->m_lStatus &= ~ApWorldSector::STATUS_DELETED;
		}

		// �ε尡 �� �ž��־�� �����̴�..
		return pSector;
	}

	// �ٽ� �޸� �Ҵ�..
	pSector = 
		( ApWorldSector * ) ApModule::CreateModuleData( SECTOR_DATA );

	ASSERT( NULL != pSector );
	
	// ���͸� �¾���..
	VERIFY( pSector->SetupSector( sectorindex_x , sectorindex_y , sectorindex_z ) );

	// �ε� ����Ʈ�� �߰���..
	CLS_AddSector( pSector );
	// ���Ϸ� ���� �о���δ�.
;
	ApAutoWriterLock	csLock( pSector->m_RWLock );
	//if (!lock.Result()) return NULL;

	// �ε� �ݹ� ȣ��.
	if( NULL != m_pfSectorLoadCallback )
		m_pfSectorLoadCallback( pSector );

	m_ppSector[ arrayIndexX ][ arrayIndexZ ] = pSector;

	// �̴� �÷��� ����..
	pSector->m_lStatus = ApWorldSector::STATUS_INIT;

	return pSector;
}

//BOOL	ApmMap::_LoadSectorOld	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z )
//{
//	ASSERT( m_bInitialized );
//	// Y ���ʹ� ���� �����Ѵ�.
//	// ���� �ε����� ��� �ε����� �����Ѵ�.
//	INT32	arrayIndexX = SectorIndexToArrayIndexX( sectorindex_x );
//	INT32	arrayIndexZ = SectorIndexToArrayIndexZ( sectorindex_z );
//
//	// ������ üŷ.
//	ASSERT( arrayIndexX	>=	0						);
//	ASSERT( arrayIndexZ	>=	0						);
//	ASSERT( arrayIndexX	<	MAP_WORLD_INDEX_WIDTH	);
//	ASSERT( arrayIndexZ	<	MAP_WORLD_INDEX_HEIGHT	);
//
////	ASSERT( NULL != m_ppSector[ arrayIndexX ][ arrayIndexZ ] );
//
//	if( m_ppSector[ arrayIndexX ][ arrayIndexZ ] )
//	{
//		// �ε��Ȱ� ����..
//		if( m_ppSector[ arrayIndexX ][ arrayIndexZ ]->IsLoadedCompactData() )
//		{
//			m_ppSector[ arrayIndexX ][ arrayIndexZ ]->RemoveCompactData();
//		}
//		if( m_ppSector[ arrayIndexX ][ arrayIndexZ ]->IsLoadedDetailData() )
//		{
//			m_ppSector[ arrayIndexX ][ arrayIndexZ ]->RemoveAllDetailData();
//		}
//
//		ApModule::DestroyModuleData( m_ppSector[ arrayIndexX ][ arrayIndexZ ] , SECTOR_DATA );
//	}
//
//	// �ٽ� �޸� �Ҵ�..
//	m_ppSector[ arrayIndexX ][ arrayIndexZ ] = 
//		( ApWorldSector * ) ApModule::CreateModuleData( SECTOR_DATA );
//
//	ASSERT( NULL != m_ppSector[ arrayIndexX ][ arrayIndexZ ] );
//
//	// �ε�����.
//
//	// ���� ����� ��Ʈ�� ���̼��͸� �����Ѵ�.
//	m_ppSector[ arrayIndexX ][ arrayIndexZ ] ->_LoadSectorOld( sectorindex_x , sectorindex_y , sectorindex_z );
//
//	// ���Ϸ� ���� �о���δ�.
//
//	m_ppSector[ arrayIndexX ][ arrayIndexZ ] ->m_Mutex.Init();
//
//	return TRUE;
//}

BOOL	ApmMap::DeleteSector	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	)
{
	return DeleteSector	( GetSector( sectorindex_x , sectorindex_y , sectorindex_z ) );
}

BOOL	ApmMap::DeleteSector		( ApWorldSector	* pSector )//�޸� ���� ����
{
	PROFILE("ApmMap::DeleteSector");

	ASSERT( m_bInitialized );
	if( NULL == pSector ) return TRUE;

	if( pSector->m_lStatus & ApWorldSector::STATUS_DELETED )
	{
		// �̹� ����Ʈ ť�� �� �ִ� ���̹Ƿ� ���̻� �Ű��� ���� �ʴ´�..
		// �� ������ ���� �༮.
		return TRUE;
	}

	ApAutoWriterLock	csLock( pSector->m_RWLock );

	// ����Ʈ �÷��� �߰���.
	pSector->m_lStatus |= ApWorldSector::STATUS_DELETED;

	if (m_ppSector[ pSector->GetArrayIndexX() ][ pSector->GetArrayIndexZ() ] != pSector)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "ApmMap::DeleteSector() Error !!!!!!!!!!\n");
		AuLogFile_s("AlefMapError.txt", strCharBuff);
	}

	// ������ (2004-03-29 ���� 3:12:41) : ��Ʈ���� �ݹ��� DeleteSectorReal�� �Ѱ���..

	pSector->m_ulRemoveTimeMSec = m_ulPrevRemoveClockCount;

	// ����Ʈ�� �߰���Ŵ..
	if (!m_csAdminSectorRemove.AddObject( &pSector, (INT_PTR) pSector ))
	{
		ApWorldSector			**ppSector	= (ApWorldSector **) m_csAdminSectorRemove.GetObjectA((INT_PTR) pSector);

		if (!ppSector)
		{
			//ASSERT(!"aaa");
			pSector->m_lStatus = ApWorldSector::STATUS_DELETED;
			DeleteSectorReal(pSector);
		}
	}

	return TRUE;
}

BOOL	ApmMap::DeleteSectorReal		( ApWorldSector	* pSector )//�޸� ���� ����
{
	ASSERT( m_bInitialized );
	if( NULL == pSector ) 
		return TRUE;

	//@{ 2006/05/29 burumal
	if( ! __IS_ADDED_SECTOR( pSector ) )
	{
		// ����..
		return TRUE;
	}
	//@}

	if( !( pSector->m_lStatus & ApWorldSector::STATUS_DELETED ) )
	{
		// ������ (2004-03-29 ���� 3:09:06) : 
		// ����Ʈ �÷��װ� ���°��
		// ����Ʈ�� ���� �ʴ´�..
		// �̰��� ����Ʈ ���Ϳ� ����Ʈ�� �ö� �ڿ� ,
		// �ٽ� SetupSector�� ȣ��ɰ�� , �÷��׸� �����ϴµ�..
		// �׷���쿡�� ���Ͱ� ���ư��� ��츦 ���� ���ؼ��̴�.
		return TRUE;
	}

	CLS_RemoveSector( pSector );

	//pSector->m_Mutex.Lock();

	// ��Ʈ���� �ݹ� ȣ��.
	if( NULL != m_pfSectorDestroyCallback )
		m_pfSectorDestroyCallback( pSector );	// ����Ÿ�� ��� �����Ѵ�.

	//TRACE( "ApmMap::DeleteSectorReal() Remove Sector(%d,%d) %x\n", pSector->GetArrayIndexX(), pSector->GetArrayIndexZ(), pSector );

	// ���� �ε����� ��� �ε����� �����Ѵ�.
	INT32	arrayIndexX = pSector->GetArrayIndexX();
	INT32	arrayIndexZ = pSector->GetArrayIndexZ();

	// ������ üŷ.
	ASSERT( arrayIndexX	>=	0						);
	ASSERT( arrayIndexZ	>=	0						);
	ASSERT( arrayIndexX	<	MAP_WORLD_INDEX_WIDTH	);
	ASSERT( arrayIndexZ	<	MAP_WORLD_INDEX_HEIGHT	);

	ASSERT( m_ppSector[ arrayIndexX ][ arrayIndexZ ] == pSector );

	m_ppSector[ arrayIndexX ][ arrayIndexZ ] = NULL;

	// �ε��Ȱ� ����.
	ClearSector( pSector );

	//pSector->m_Mutex.Destroy()	;

	// �޸𸮿��� ������..
	ApModule::DestroyModuleData( pSector , SECTOR_DATA );

	return TRUE;
}

BOOL	ApmMap::AddChar			( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS pos,		INT_PTR cid,			INT_PTR cid2,			INT32 range)
{
	switch (eCharType) {
	case APMMAP_CHAR_TYPE_PC:
		{
			return AddChar( nDimensionIndex , pos, cid, cid2, range);
		}
		break;

	case APMMAP_CHAR_TYPE_NPC:
		{
			return AddNPC( nDimensionIndex , pos, cid, cid2, range);
		}
		break;

	case APMMAP_CHAR_TYPE_MONSTER:
		{
			return AddMonster( nDimensionIndex , pos, cid, cid2, range);
		}
		break;
	}

	return FALSE;
}

BOOL	ApmMap::UpdateChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid , INT_PTR cid2	)
{
	switch (eCharType) {
	case APMMAP_CHAR_TYPE_PC:
		{
			return UpdateChar( nDimensionIndex , posPrev, posCurrent, cid, cid2);
		}
		break;

	case APMMAP_CHAR_TYPE_NPC:
		{
			return UpdateNPC( nDimensionIndex , posPrev, posCurrent, cid, cid2);
		}
		break;

	case APMMAP_CHAR_TYPE_MONSTER:
		{
			return UpdateMonster( nDimensionIndex , posPrev, posCurrent, cid, cid2);
		}
		break;
	}

	return FALSE;
}

BOOL	ApmMap::DeleteChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS pos ,		INT_PTR cid						)
{
	switch (eCharType) {
	case APMMAP_CHAR_TYPE_PC:
		{
			return DeleteChar( nDimensionIndex , pos, cid);
		}
		break;

	case APMMAP_CHAR_TYPE_NPC:
		{
			return DeleteNPC( nDimensionIndex , pos, cid);
		}
		break;

	case APMMAP_CHAR_TYPE_MONSTER:
		{
			return DeleteMonster( nDimensionIndex , pos, cid);
		}
		break;
	}

	return FALSE;
}

BOOL	ApmMap::DeleteChars		( INT32	nDimensionIndex , ApmMapCharType eCharType,		ApWorldSector * pSector							)
{
	switch (eCharType) {
	case APMMAP_CHAR_TYPE_PC:
		{
			return DeleteChars( nDimensionIndex , pSector);
		}
		break;

	case APMMAP_CHAR_TYPE_NPC:
		{
			return DeleteNPCs( nDimensionIndex , pSector);
		}
		break;

	case APMMAP_CHAR_TYPE_MONSTER:
		{
			return DeleteMonsters( nDimensionIndex , pSector);
		}
		break;
	}

	return FALSE;
}

BOOL	ApmMap::MoveChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	)
{
	switch (eCharType) {
	case APMMAP_CHAR_TYPE_PC:
		{
			return MoveChar( nDimensionIndex , pOldSector, pNewSector, cid, cid2, posCurrent);
		}
		break;

	case APMMAP_CHAR_TYPE_NPC:
		{
			return MoveNPC( nDimensionIndex , pOldSector, pNewSector, cid, cid2, posCurrent);
		}
		break;

	case APMMAP_CHAR_TYPE_MONSTER:
		{
			return MoveMonster( nDimensionIndex , pOldSector, pNewSector, cid, cid2, posCurrent);
		}
		break;
	}

	return FALSE;
}

BOOL	ApmMap::AddChar			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2,		INT32 range		)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	//ASSERT( NULL != pSector );

	if( pSector == NULL )
	{
		// �ڵ� �ε�..
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( pos );
			z		= PosToSectorIndexZ( pos );
			pSector	= SetupSector( x , 0 , z );

			if( pSector )
			{
				TRACE( "���� �ڵ� �ε���.. " );
				// ���� ����Ÿ �ε�..
				LoadSector( pSector );
			}
			else
			{
				//TRACE( "ApmMap::AddChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
				//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::AddChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
				return FALSE;
			}
		}
		else
		{
			//TRACE( "ApmMap::AddChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
			//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::AddChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
			return FALSE;
		}
	}

	BOOL bRetval = pSector->AddChar( nDimensionIndex , pos , cid , cid2 , range );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_ADDCHAR, pSector, (PVOID)cid);

	ASSERT( bRetval );

	return bRetval;
}

BOOL	ApmMap::UpdateChar		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR cid , INT_PTR cid2	)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector		= GetSector( posPrev	);
	ApWorldSector * pSectorNext	= GetSector( posCurrent	);

	if( pSectorNext == NULL )
	{
		// �ڵ� �ε�..
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( posCurrent );
			z		= PosToSectorIndexZ( posCurrent );
			pSectorNext	= SetupSector( x , 0 , z );

			if( pSectorNext )
			{
				TRACE( "���� �ڵ� �ε���.. " );
				// ���� ����Ÿ �ε�..
				LoadSector( pSectorNext );
			}
			else
			{
				//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::UpdateChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , posCurrent.x , posCurrent.z );

				return FALSE;
			}
		}
		else
		{
			//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::UpdateChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , posCurrent.x , posCurrent.z );

			return FALSE;
		}
		/*
		#ifdef _DEBUG
		OutputDebugString( "ApmMap::UpdateChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.");
		#endif
		return FALSE;
		*/
	}

	if ( pSector == NULL )
	{
		return pSectorNext->AddChar( nDimensionIndex , posCurrent, cid, cid2 );
	}

	if( pSector == pSectorNext )
	{
		//���� �ڸ� �̹Ƿ� ������Ʈ.
		return pSector->UpdateChar( nDimensionIndex , posCurrent , cid );
	}
	else
	{
		// ���� ��ŷ üũ.
		// ������ (2004-05-11 ���� 4:30:29) : �ʿ��� üũ���� �ʴ´�..
		// ô�� ���������� ����

		//if( CheckBlockingInfo( posCurrent ) )
		//	return FALSE;

		// �ٸ��ڸ��̹Ƿ� ����-> �߰� ������ ��ħ.
		return MoveChar( nDimensionIndex , pSector, pSectorNext, cid, cid2, posCurrent);
		//pSector->DeleteChar( pid );
		//return pSectorNext->AddChar( posCurrent , pid );
	}

	return FALSE;
}

BOOL	ApmMap::DeleteChar		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid					)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	BOOL bRetval = pSector->DeleteChar( nDimensionIndex , cid );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_REMOVECHAR, pSector, (PVOID)cid);

	ASSERT( bRetval );

	return bRetval;
}

BOOL	ApmMap::DeleteChars		( INT32	nDimensionIndex , ApWorldSector * pSector )
{
	ApWorldSector::IdPos *	pUser;
	ApWorldSector::IdPos *	pNextUser;

	ASSERT( m_bInitialized );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}


	ApWorldSector::Dimension	* pDimension;
	pDimension = pSector->GetDimension( nDimensionIndex );

	if( !pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockUsers );

	for( pUser = pDimension->pUsers ; pUser ; pUser = pNextUser )
	{
		pNextUser = pUser->pNext;

		EnumCallback(APMMAP_CB_ID_REMOVECHAR, pSector, &pUser->id);

		delete pUser;
	}

	pDimension->pUsers = NULL;

	return TRUE;
}

BOOL	ApmMap::MoveChar		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,	INT_PTR cid2,	AuPOS posCurrent	)
{
	ASSERT( m_bInitialized );

	if (!pOldSector || !pNewSector)
		return FALSE;

	ApWorldSector::IdPos *	pUser = pOldSector->GetChar( nDimensionIndex , cid );
	if (!pUser)
	{
		if (!pNewSector->AddChar( nDimensionIndex , posCurrent, cid, cid2 ))
		{
			ASSERT ( FALSE );
			return FALSE;
		}
	}
	else
	{
		pOldSector->RemoveChar( nDimensionIndex , pUser , TRUE );

		pUser->pos = posCurrent;

		if (!pNewSector->AddChar( nDimensionIndex , pUser ))
		{
			delete pUser;
			ASSERT ( FALSE );
			return FALSE;
		}
	}

	stApmMapMoveSector	stMoveSector;
	stMoveSector.pOldSector = pOldSector;
	stMoveSector.pNewSector = pNewSector;
	stMoveSector.pstCurrentPos = &posCurrent;

	EnumCallback(APMMAP_CB_ID_MOVECHAR, &stMoveSector, (PVOID)cid);

	return TRUE;
}

BOOL	ApmMap::AddNPC			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range		)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	//ASSERT( NULL != pSector );

	if( pSector == NULL )
	{
		// �ڵ� �ε�..
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( pos );
			z		= PosToSectorIndexZ( pos );
			pSector	= SetupSector( x , 0 , z );

			if( pSector )
			{
				TRACE( "���� �ڵ� �ε���.. " );
				// ���� ����Ÿ �ε�..
				LoadSector( pSector );
			}
			else
			{
				//TRACE( "ApmMap::AddNPC	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
				return FALSE;
			}
		}
		else
		{
			//TRACE( "ApmMap::AddNPC	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
			return FALSE;
		}
	}

	BOOL bRetval = pSector->AddNPC( nDimensionIndex , pos , cid , cid2 , range );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_ADDCHAR, pSector, (PVOID)cid);

	ASSERT( bRetval );

	return bRetval;
}

BOOL	ApmMap::UpdateNPC		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR cid , INT_PTR cid2	)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector		= GetSector( posPrev	);
	ApWorldSector * pSectorNext	= GetSector( posCurrent	);

	if( pSectorNext == NULL )
	{
		// �ڵ� �ε�..
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( posCurrent );
			z		= PosToSectorIndexZ( posCurrent );
			pSectorNext	= SetupSector( x , 0 , z );

			if( pSectorNext )
			{
				TRACE( "���� �ڵ� �ε���.. " );
				// ���� ����Ÿ �ε�..
				LoadSector( pSectorNext );
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	if ( pSector == NULL )
	{
		return pSectorNext->AddNPC( nDimensionIndex , posCurrent, cid, cid2 );
	}

	if( pSector == pSectorNext )
	{
		//���� �ڸ� �̹Ƿ� ������Ʈ.
		return pSector->UpdateNPC( nDimensionIndex , posCurrent , cid );
	}
	else
	{
		// ������ (2004-05-11 ���� 4:30:29) : �ʿ��� üũ���� �ʴ´�..
		// ô�� ���������� ����

		// ���� ��ŷ üũ.
		// if( CheckBlockingInfo( posCurrent ) )
		// 	return FALSE;

		// �ٸ��ڸ��̹Ƿ� ����-> �߰� ������ ��ħ.
		return MoveNPC( nDimensionIndex , pSector, pSectorNext, cid, cid2, posCurrent);
		//pSector->DeleteChar( pid );
		//return pSectorNext->AddChar( posCurrent , pid );
	}

	return FALSE;
}

BOOL	ApmMap::DeleteNPC		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid					)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteNPC	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	BOOL bRetval = pSector->DeleteNPC( nDimensionIndex , cid );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_REMOVECHAR, pSector, (PVOID)cid);

	ASSERT( bRetval );

	return bRetval;
}

BOOL	ApmMap::DeleteNPCs		( INT32	nDimensionIndex , ApWorldSector * pSector )
{
	ApWorldSector::IdPos *	pNPC;
	ApWorldSector::IdPos *	pNextNPC;

	ASSERT( m_bInitialized );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteNPCs	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	ApWorldSector::Dimension	* pDimension;
	pDimension = pSector->GetDimension( nDimensionIndex );

	if( !pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockNPCs );

	for( pNPC = pDimension->pNPCs ; pNPC ; pNPC = pNextNPC )
	{
		pNextNPC = pNPC->pNext;

		EnumCallback(APMMAP_CB_ID_REMOVECHAR, pSector, &pNPC->id);

		delete pNPC;
	}

	pDimension->pNPCs = NULL;

	return TRUE;
}

BOOL	ApmMap::MoveNPC		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,	INT_PTR cid2,	AuPOS posCurrent	)
{
	ASSERT( m_bInitialized );

	if (!pOldSector || !pNewSector)
		return FALSE;

	ApWorldSector::IdPos *	pNPC = pOldSector->GetNPC( nDimensionIndex , cid );
	if (!pNPC)
	{
		if (!pNewSector->AddNPC( nDimensionIndex , posCurrent, cid, cid2 ))
		{
			ASSERT ( FALSE );
			return FALSE;
		}
	}
	else
	{
		pOldSector->RemoveNPC( nDimensionIndex , pNPC , TRUE );

		pNPC->pos = posCurrent;

		if (!pNewSector->AddNPC( nDimensionIndex , pNPC ))
		{
			delete pNPC;
			ASSERT ( FALSE );
			return FALSE;
		}
	}

	stApmMapMoveSector	stMoveSector;
	stMoveSector.pOldSector = pOldSector;
	stMoveSector.pNewSector = pNewSector;
	stMoveSector.pstCurrentPos = &posCurrent;

	EnumCallback(APMMAP_CB_ID_MOVECHAR, &stMoveSector, (PVOID)cid);

	return TRUE;
}

BOOL	ApmMap::AddMonster			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2 ,		INT32 range		)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	//ASSERT( NULL != pSector );

	if( pSector == NULL )
	{
		// �ڵ� �ε�..
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( pos );
			z		= PosToSectorIndexZ( pos );
			pSector	= SetupSector( x , 0 , z );

			if( pSector )
			{
				TRACE( "���� �ڵ� �ε���.. " );
				// ���� ����Ÿ �ε�..
				LoadSector( pSector );
			}
			else
			{
				//TRACE( "ApmMap::AddMonster	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
				return FALSE;
			}
		}
		else
		{
			//TRACE( "ApmMap::AddMonster	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , pos.x , pos.z );
			return FALSE;
		}
	}

	BOOL bRetval = pSector->AddMonster( nDimensionIndex , pos , cid , cid2 , range );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_ADDCHAR, pSector, (PVOID)cid);

	ASSERT( bRetval );

	return bRetval;
}

BOOL	ApmMap::UpdateMonster		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR cid , INT_PTR cid2	)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector		= GetSector( posPrev	);
	ApWorldSector * pSectorNext	= GetSector( posCurrent	);

	if( pSectorNext == NULL )
	{
		// �ڵ� �ε�..
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( posCurrent );
			z		= PosToSectorIndexZ( posCurrent );
			pSectorNext	= SetupSector( x , 0 , z );

			if( pSectorNext )
			{
				TRACE( "���� �ڵ� �ε���.. " );
				// ���� ����Ÿ �ε�..
				LoadSector( pSectorNext );
			}
			else
			{
				//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::UpdateMonster	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , posCurrent.x , posCurrent.z );

				return FALSE;
			}
		}
		else
		{
			//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::UpdateMonster	���Ͱ� �ε��ž����� �ʽ��ϴ�.( P %f,%f )\n" , posCurrent.x , posCurrent.z );
			return FALSE;
		}
	}

	if ( pSector == NULL )
	{
		return pSectorNext->AddMonster( nDimensionIndex , posCurrent, cid, cid2 );
	}

	if( pSector == pSectorNext )
	{
		//���� �ڸ� �̹Ƿ� ������Ʈ.
		return pSector->UpdateMonster( nDimensionIndex , posCurrent , cid );
	}
	else
	{
		// ���� ��ŷ üũ.
		// ������ (2004-05-11 ���� 4:30:29) : �ʿ��� üũ���� �ʴ´�..
		// ô�� ���������� ����

		//if( CheckBlockingInfo( posCurrent ) )
		//	return FALSE;

		// �ٸ��ڸ��̹Ƿ� ����-> �߰� ������ ��ħ.
		return MoveMonster( nDimensionIndex , pSector, pSectorNext, cid, cid2, posCurrent);
		//pSector->DeleteChar( pid );
		//return pSectorNext->AddChar( posCurrent , pid );
	}

	return FALSE;
}

BOOL	ApmMap::DeleteMonster		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid					)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteMonster	���Ͱ� �ε��ž����� �ʽ��ϴ�.");
		return FALSE;
	}

	BOOL bRetval = pSector->DeleteMonster( nDimensionIndex , cid );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_REMOVECHAR, pSector, (PVOID)cid);

	ASSERT( bRetval );

	return bRetval;
}

BOOL	ApmMap::DeleteMonsters		( INT32 nDimensionIndex , ApWorldSector * pSector )
{
	ApWorldSector::IdPos *	pMonster;
	ApWorldSector::IdPos *	pNextMonster;

	ASSERT( m_bInitialized );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteMonsters	���Ͱ� �ε��ž����� �ʽ��ϴ�.");
		return FALSE;
	}

	ApWorldSector::Dimension	* pDimension;
	pDimension = pSector->GetDimension( nDimensionIndex );

	if( !pDimension ) return FALSE;

	ApAutoWriterLock	csLock( pDimension->lockMonsters );

	for( pMonster = pDimension->pMonsters ; pMonster ; pMonster = pNextMonster )
	{
		pNextMonster = pMonster->pNext;

		EnumCallback(APMMAP_CB_ID_REMOVECHAR, pSector, &pMonster->id);

		delete pMonster;
	}

	pDimension->pMonsters = NULL;

	return TRUE;
}

BOOL	ApmMap::MoveMonster		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,	INT_PTR cid2, AuPOS posCurrent	)
{
	ASSERT( m_bInitialized );

	if (!pOldSector || !pNewSector)
		return FALSE;

	ApWorldSector::IdPos *	pMonster = pOldSector->GetMonster( nDimensionIndex , cid );
	if (!pMonster)
	{
		if (!pNewSector->AddMonster( nDimensionIndex , posCurrent, cid, cid2 ))
		{
			ASSERT ( FALSE );
			return FALSE;
		}
	}
	else
	{
		pOldSector->RemoveMonster( nDimensionIndex , pMonster , TRUE );

		pMonster->pos = posCurrent;

		if (!pNewSector->AddMonster( nDimensionIndex , pMonster ))
		{
			delete pMonster;
			ASSERT ( FALSE );
			return FALSE;
		}
	}

	stApmMapMoveSector	stMoveSector;
	stMoveSector.pOldSector = pOldSector;
	stMoveSector.pNewSector = pNewSector;
	stMoveSector.pstCurrentPos = &posCurrent;

	EnumCallback(APMMAP_CB_ID_MOVECHAR, &stMoveSector, (PVOID)cid);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////
BOOL	ApmMap::AddItem			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid					)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	if( pSector == NULL )
	{
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( pos );
			z		= PosToSectorIndexZ( pos );
			pSector	= SetupSector( x , 0 , z );

			if( pSector )
			{
				// ���� ����Ÿ �ε�..
				TRACE( "���� �ڵ� �ε���.. " );
				LoadSector( pSector );
			}
			else
			{
				//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::AddItem	���Ͱ� �ε��ž����� �ʽ��ϴ�.");
				return FALSE;
			}
		}
		else
		{
			//TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::AddItem	���Ͱ� �ε��ž����� �ʽ��ϴ�.");
			return FALSE;
		}
	}

	BOOL bRetval = pSector->AddItem( nDimensionIndex , pos , iid );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_ADDITEM, pSector, &iid);

	return bRetval;
}

BOOL	ApmMap::UpdateItem		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR iid	)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector		= GetSector( posPrev	);
	ApWorldSector * pSectorNext	= GetSector( posCurrent	);
	if( pSector == NULL || pSectorNext == NULL )
	{
		//TRACE( "ApmMap::UpdateItem	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	if( pSector == pSectorNext )
	{
		//���� �ڸ� �̹Ƿ� ������Ʈ.
		return pSector->UpdateItem( nDimensionIndex , posCurrent , iid );
	}
	else
	{
		// �ٸ��ڸ��̹Ƿ� ����-> �߰� ������ ��ħ.
		//pSector->DeleteItem( iid );
		//return pSectorNext->AddItem( posCurrent , iid );
		return MoveItem( nDimensionIndex , pSector , pSectorNext , iid , posCurrent );
	}

	return FALSE;
}

BOOL	ApmMap::DeleteItem		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid					)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteItem	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	BOOL bRetval = pSector->DeleteItem( nDimensionIndex , iid );

	if (bRetval)
		EnumCallback(APMMAP_CB_ID_REMOVEITEM, pSector, &iid);

	return bRetval;
}

BOOL	ApmMap::DeleteItems		( INT32	nDimensionIndex , ApWorldSector * pSector )
{
	ApWorldSector::IdPos *	pItem;
	ApWorldSector::IdPos *	pNextItem;

	ASSERT( m_bInitialized );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	ApWorldSector::Dimension	* pDimension;
	pDimension = pSector->GetDimension( nDimensionIndex );

	if( !pDimension ) return FALSE;

	for( pItem = pDimension->pItems ; pItem ; pItem = pNextItem )
	{
		pNextItem = pItem->pNext;

		EnumCallback(APMMAP_CB_ID_REMOVEITEM, pSector, &pItem->id);

		delete pItem;
	}

	pDimension->pItems = NULL;

	return TRUE;
}

BOOL	ApmMap::MoveItem		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR iid,	AuPOS posCurrent	)
{
	ASSERT( m_bInitialized );

	if (!pOldSector || !pNewSector)
		return FALSE;

	ApWorldSector::IdPos *	pItem = pOldSector->GetItem( nDimensionIndex , iid );
	if (!pItem)
	{
		if (!pNewSector->AddItem( nDimensionIndex , posCurrent, iid ))
			return FALSE;
	}
	else
	{
		pOldSector->RemoveItem( nDimensionIndex , pItem , TRUE );

		pItem->pos = posCurrent;

		if (!pNewSector->AddItem( nDimensionIndex , pItem ))
		{
			delete pItem;
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL	ApmMap::AddObject			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid , AuBLOCKING *pstBlocking	)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector = GetSector( pos );

	if( pSector == NULL )
	{
		/* �ϴ��� ���ư��� ����... �̺κ��� ���߿� �ٸ� ������� ��ü�Ǿ�� ��.
		if( IsAutoLoadData() )
		{
			INT32	x,z;
			x		= PosToSectorIndexX( pos );
			z		= PosToSectorIndexZ( pos );
			pSector	= SetupSector( x , 0 , z );

			if( pSector )
			{
				// ���� ����Ÿ �ε�..
				TRACE( "���� �ڵ� �ε���.. " );
				LoadSector( pSector );
			}
			else
			{
				TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::AddObject	���Ͱ� �ε��ž����� �ʽ��ϴ�.");
				return FALSE;
			}
		}
		else
		{
			TRACEFILE(ALEF_ERROR_FILENAME, "ApmMap::AddObject	���Ͱ� �ε��ž����� �ʽ��ϴ�.");
			return FALSE;
		}
		*/

		return TRUE;
	}

	return pSector->AddObject( nDimensionIndex , pos , oid , pstBlocking );
}

BOOL	ApmMap::UpdateObject		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR oid	)
{
	ASSERT( m_bInitialized );

	ApWorldSector *	pSector		= GetSector( posPrev	);
	ApWorldSector * pSectorNext	= GetSector( posCurrent	);
	if( pSector == NULL || pSectorNext == NULL )
	{
		//TRACE( "ApmMap::UpdateObject	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	if( pSector == pSectorNext )
	{
		//���� �ڸ� �̹Ƿ� ������Ʈ.
		return pSector->UpdateObject( nDimensionIndex , posCurrent , oid );
	}
	else
	{
		// �ٸ��ڸ��̹Ƿ� ����-> �߰� ������ ��ħ.
		//pSector->DeleteObject( oid );
		//return pSectorNext->AddObject( posCurrent , oid );

		return MoveObject ( nDimensionIndex , pSector , pSectorNext , oid , posCurrent );
	}

	return FALSE;
}

BOOL	ApmMap::DeleteObject		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid					)
{
	ASSERT( m_bInitialized );
	ApWorldSector *	pSector = GetSector( pos );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteObject	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	return pSector->DeleteObject( nDimensionIndex , oid );
}

BOOL	ApmMap::DeleteObjects		( INT32	nDimensionIndex , ApWorldSector * pSector )
{
	ApWorldSector::IdPos *	pObject;
	ApWorldSector::IdPos *	pNextObject;

	ASSERT( m_bInitialized );

	if( pSector == NULL )
	{
		//TRACE( "ApmMap::DeleteChar	���Ͱ� �ε��ž����� �ʽ��ϴ�.\n");
		return FALSE;
	}

	ApWorldSector::Dimension	* pDimension;
	pDimension = pSector->GetDimension( nDimensionIndex );

	if( !pDimension ) return FALSE;

	for( pObject = pDimension->pObjects ; pObject ; pObject = pNextObject )
	{
		pNextObject = pObject->pNext;

//		EnumCallback(APMMAP_CB_ID_REMOVEOBJECT, pSector, &pObject->id);

		delete pObject;
	}

	pDimension->pObjects = NULL;

	return TRUE;
}


BOOL	ApmMap::MoveObject		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR oid,	AuPOS posCurrent	)
{
	ASSERT( m_bInitialized );
	if (!pOldSector || !pNewSector)
		return FALSE;

	ApWorldSector::IdPos *	pObject = pOldSector->GetObject( nDimensionIndex , oid );
	if (!pObject)
	{
		if (!pNewSector->AddObject( nDimensionIndex , posCurrent, oid ))
			return FALSE;
	}
	else
	{
		pOldSector->RemoveObject( nDimensionIndex , pObject , TRUE );

		pObject->pos = posCurrent;

		if (!pNewSector->AddObject( nDimensionIndex , pObject ))
		{
			delete pObject;
			return FALSE;
		}
	}

	return TRUE;
}


// ��� �̺�Ʈ.
BOOL	ApmMap::OnAddModule	()
{
	m_bInitialized = TRUE;
	
	return TRUE;
}

BOOL	ApmMap::OnInit		()
{
//	for( INT32 z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; z ++ )
//		for( INT32 x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; x ++ )
//		{
//			LoadSector( 
//				ArrayIndexToSectorIndexX( x ) ,
//				ArrayIndexToSectorIndexZ( z ) );
//		}

	if (!m_csAdminSectorRemove.InitializeObject( sizeof(ApWorldSector *), m_lSectorRemovePool))
		return FALSE;

	return TRUE;
}

BOOL	ApmMap::OnIdle	( UINT32 ulClockCount )
{
	PROFILE("ApmMap::OnIdle");

	ProcessRemove(ulClockCount);

	return TRUE;
}

BOOL	ApmMap::OnDestroy	()
{
	ASSERT( m_bInitialized );
	
	try
	{
		DeleteAllSector();
	}
	catch( ... )
	{
		// �޸𸮿���..
		TRACE( "�ʵ���Ÿ ������ �޸� �����߻�..\n" );
	}
	ClearAllBindData();
	FlushUnpackManagerDivision();

	FlushDeleteSectors();

	DeleteRealAllSector	();

	m_bInitialized = FALSE;

	return TRUE;
}

void	ApmMap::FlushDeleteSectors()
{
	INT32	lIndex = 0;

	ApWorldSector			**ppSector	= (ApWorldSector **) m_csAdminSectorRemove.GetObjectSequence(&lIndex);
	while (ppSector && *ppSector)
	{
		DeleteSectorReal(*ppSector);
		ppSector	= (ApWorldSector **) m_csAdminSectorRemove.GetObjectSequence(&lIndex);
	}

	m_csAdminSectorRemove.RemoveObjectAll( TRUE );
}

void	ApmMap::ProcessRemove	( UINT32 ulClockCount )
{
	if( m_ulPrevRemoveClockCount + APMMAP_PROCESS_REMOVE_INTERVAL > ulClockCount )
		return;

	INT32	lIndex	= 0;
	ApWorldSector *		pSector		= NULL;
	ApWorldSector **	ppSector	= (ApWorldSector **) m_csAdminSectorRemove.GetObjectSequence( &lIndex );

	while( ppSector )
	{
		pSector		= *ppSector;

		if (pSector)
		{
			if( pSector->m_ulRemoveTimeMSec + m_lSectorRemoveDelay < ulClockCount )
			{
				DeleteSectorReal( pSector );
				m_csAdminSectorRemove.RemoveObject( (INT_PTR)pSector );
				lIndex = 0;
			}
		}
		else
		{
			m_csAdminSectorRemove.RemoveObject( (INT_PTR)pSector );
			lIndex = 0;
		}

		ppSector = (ApWorldSector **) m_csAdminSectorRemove.GetObjectSequence(&lIndex);
	}

	m_ulPrevRemoveClockCount = ulClockCount;

	return;
}

// ��ⵥ��Ÿ �������̽�.
INT16	ApmMap::AttachSectorData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, SECTOR_DATA, nDataSize, pfConstructor, pfDestructor);	
}
INT16	ApmMap::AttachSegmentData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, SEGMENT_DATA, nDataSize, pfConstructor, pfDestructor);
}

/*
BOOL	ApmMap::GetSectorDataChar	( AuPOS pos , INT32 ** pArray , INT32 * pSize		)
{
	ASSERT( m_bInitialized );
	ApWorldSector * pSector = GetSector( pos );
	if( pSector )
	{
		*pArray = ( INT32 * ) pSector->m_aUserID;
		*pSize	= pSector->m_nUserID;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	ApmMap::GetSectorDataItem	( AuPOS pos , INT32 ** pArray , INT32 * pSize		)
{
	ASSERT( m_bInitialized );
	ApWorldSector * pSector = GetSector( pos );
	if( pSector )
	{
		*pArray = ( INT32 * ) pSector->m_aItemID;
		*pSize	= pSector->m_nItemID;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	ApmMap::GetSectorDataObject	( AuPOS pos , INT32 ** pArray , INT32 * pSize		)
{
	ASSERT( m_bInitialized );
	ApWorldSector * pSector = GetSector( pos );
	if( pSector )
	{
		*pArray = ( INT32 * ) pSector->m_aObjectID;
		*pSize	= pSector->m_nObjectID;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	ApmMap::GetSectorDataChar	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		) // z��Ҵ� �׻�0 .. ������ʵ�.
{
	ASSERT( m_bInitialized );
	ApWorldSector * pSector = GetSector( wx , wy , wz );
	if( pSector )
	{
		*pArray = ( INT32 * ) pSector->m_aUserID;
		*pSize	= pSector->m_nUserID;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	ApmMap::GetSectorDataItem	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		)
{
	ASSERT( m_bInitialized );
	ApWorldSector * pSector = GetSector( wx , wy , wz );
	if( pSector )
	{
		*pArray = ( INT32 * ) pSector->m_aItemID;
		*pSize	= pSector->m_nItemID;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL	ApmMap::GetSectorDataObject	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		)
{
	ASSERT( m_bInitialized );
	ApWorldSector * pSector = GetSector( wx , wy , wz );
	if( pSector )
	{
		*pArray = ( INT32 * ) pSector->m_aObjectID;
		*pSize	= pSector->m_nObjectID;
		return TRUE;
	}
	else
		return FALSE;
}
*/

BOOL ApmMap::SetCallbackAddChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(APMMAP_CB_ID_ADDCHAR, pfCallback, pClass);
}

BOOL ApmMap::SetCallbackRemoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(APMMAP_CB_ID_REMOVECHAR, pfCallback, pClass);
}

BOOL ApmMap::SetCallbackMoveChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(APMMAP_CB_ID_MOVECHAR, pfCallback, pClass);
}

BOOL ApmMap::SetCallbackAddItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(APMMAP_CB_ID_ADDITEM, pfCallback, pClass);
}

BOOL ApmMap::SetCallbackRemoveItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(APMMAP_CB_ID_REMOVEITEM, pfCallback, pClass);
}

// Add by gemani (04.03.25)
BOOL ApmMap::SetCallbackLoadSector(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(APMMAP_CB_ID_LOAD_SECOTR, pfCallback, pClass);
}

BOOL ApmMap::SetCallbackClearSector(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(APMMAP_CB_ID_CLEAR_SECTOR, pfCallback, pClass);
}

BOOL	ApmMap::LoadDivision	( UINT32 uDivision , BOOL bLoadData , BOOL bServerData )
{
	// 2005/01/26 ������
	INT32	nStartX , nStartZ;

	nStartX = GetFirstSectorXInDivision( uDivision );
	nStartZ = GetFirstSectorZInDivision( uDivision );

	INT32	nSuccessCount = 0;

	for( INT32 z = 0 ; z < MAP_DEFAULT_DEPTH ; z++ )
	{
		for( INT32 x = 0 ; x < MAP_DEFAULT_DEPTH ; x++ )
		{
			SetupSector	( ArrayIndexToSectorIndexX( nStartX + x ) , 0 , ArrayIndexToSectorIndexZ( nStartZ + z ) );

			if( bLoadData )
			{
				if( LoadSector	( ArrayIndexToSectorIndexX( nStartX + x ) , 0 , ArrayIndexToSectorIndexZ( nStartZ + z ) , bServerData ) )
				{
					nSuccessCount++;
				}
			}
		}
	}

	// ���� ����üũ�� ���� ����..
	// nSuccessCount �� 256 �̾�� �ϳ� ū �ǹ̴� ����...
	// ���� �Ʈ�� ����� ����.
	// ASSERT( nSuccessCount == 256 );

	return TRUE;
}

BOOL	ApmMap::LoadAll			( BOOL bLoadData , BOOL bServerData , ProgressCallback pfCallback , void * pData )
{
//	ASSERT( m_bInitialized == FALSE );
//
//	m_bInitialized = TRUE;

	static	char	strMessage[] = "�ʷε�";
	int		nTarget		= MAP_WORLD_INDEX_HEIGHT * MAP_WORLD_INDEX_WIDTH	;
	int		nCurrent	= 1													;
/*
	for( INT32 y = 0 ; y < 500 ; ++y )
	{
		for( INT32 x = 0 ; x < 500 ; ++x )
		{
			if( pfCallback ) pfCallback( strMessage , nCurrent++ , nTarget , pData );
			SetupSector	( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( y ) );

			if( bLoadData)
				LoadSector	( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( y ) );
		}
	}
*/
	char	strFullPath[ 1024 ];

	if( bServerData )
	{
		wsprintf( strFullPath , "%s\\%s" , GetWorldDirectory() , COMPACT_SERVER_FORMAT_WILDCARD );
	}
	else
	{
		wsprintf( strFullPath , "%s\\%s" , GetWorldDirectory() , ALEF_WORLD_COMPACT_SECTOR_FILE_WILDCARD_FORMAT );
	}

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 
	//char szNewPath[MAX_PATH]; 
	//char szHome[MAX_PATH]; 

	BOOL fFinished = FALSE; 
	INT32	nCount = 0;

	hSearch = FindFirstFile( strFullPath , &FileData ); 
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		#ifdef _DEBUG
		//ASSERT( !"�ʵ���Ÿ ������ �������� �ʽ��ϴ�. ������� Ȯ���ϼ���" );
		char	strCurrentDirectory[ 1024 ];
		GetCurrentDirectory( 1024 , strCurrentDirectory );
		TRACE( "������丮 '%s'\n" , strCurrentDirectory );
		TRACE( "ã������ ���� '%s'\n" , strFullPath );
		#endif

		return FALSE;
	} 

	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	UINT32	uDivision;

	while (!fFinished) 
	{
		nCount++;
		// ã�� ����..
		// FileData.cFileName

		// �м�
		_splitpath( FileData.cFileName , drive, dir, fname, ext );

		// �ε��� �̾Ƴ�..
		uDivision = atoi( fname + 1 );

		TRACE( "'%s'���� ���� �̾Ƴ� ����� �ε��� %04d\n" , FileData.cFileName , uDivision );

		// �ε� ����
		LoadDivision( uDivision , bLoadData , bServerData );

		// �������Ϸ�..
		if (!FindNextFile(hSearch, &FileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				// �� ã����..
				fFinished = TRUE; 
			} 
			else 
			{ 
				ASSERT( !"���� �������� �����ڶ� ����" );
				return FALSE ;
			} 
		}
	} 

	// Close the search handle. 

	FindClose(hSearch);

	return TRUE;
}

BOOL	ApmMap::SaveAll			()
{
	for( INT32 z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
	{
		for( INT32 x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
		{
			if( NULL != m_ppSector[ x ][ z ] )
			{
				// m_ppSector[ x ][ z ]->SaveDetailData();

				m_ppSector[ x ][ z ]->SaveDetailDataGeometry	( );
				m_ppSector[ x ][ z ]->SaveDetailDataTile		( );
			}
		}
	}
	return TRUE;
}


void	ApmMap::DeleteAllSector	()
{
	for( INT32 z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
	{
		for( INT32 x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
		{
			/*
			// �ʵ���Ÿ ������..
			VERIFY( ClearSector	( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( z ) ) );
			*/
			// �޸𸮿��� ���� ����.
			VERIFY( DeleteSector( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( z ) ) );
		}
	}
}

void	ApmMap::DeleteRealAllSector	()
{
	for( INT32 z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
	{
		for( INT32 x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
		{
			DeleteSectorReal(m_ppSector[x][z]);

			/*
			// �ʵ���Ÿ ������..
			VERIFY( DeleteSectorReal( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( z ) ) );
			*/
		}
	}
}

void	ApmMap::ClearAllSector	()
{
	for( INT32 z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
	{
		for( INT32 x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
		{
			// �ʵ���Ÿ ������..
			VERIFY( ClearSector	( ArrayIndexToSectorIndexX( x ) , 0 , ArrayIndexToSectorIndexZ( z ) ) );
		}
	}
}

BOOL	ApmMap::ClearSector		( ApWorldSector	* pSector )
{
	if( NULL == pSector ) return TRUE;

	ASSERT( m_bInitialized );

	/*
	// ����ε� ���� ���������� Ȯ���Ѵ�.
	ApWorldSector * pRetrivedSector;
	pRetrivedSector = GetSector( pSector->GetIndexX() , 0 , pSector->GetIndexZ() );

	ASSERT( pRetrivedSector == pSector );
	if( pRetrivedSector != pSector ) return FALSE;
	*/

	EnumCallback(APMMAP_CB_ID_CLEAR_SECTOR, pSector, NULL);	

	BOOL bRet;
	bRet = pSector->RemoveAllData();

	if( bRet )
		return TRUE;

	return FALSE;
}

BOOL ApmMap::LoadSector( ApWorldSector* pSector , BOOL bServer )
{
	ASSERT( m_bInitialized );
	if (!pSector) return FALSE;

	ApAutoWriterLock	csLock( pSector->m_RWLock );

	// ����Ʈ �÷��װ� �ִ°��.. �÷��� ����..
	if (pSector->m_lStatus & ApWorldSector::STATUS_DELETED)
	{
		m_csAdminSectorRemove.RemoveObject((INT_PTR) pSector);
		pSector->m_lStatus &= ~ApWorldSector::STATUS_DELETED;
	}

	// �̹� Load �Ǿ� ������ return
	if( pSector->m_lStatus & ApWorldSector::STATUS_LOADED )
		return TRUE;

#ifdef _DEBUG
	ApWorldSector* pRetrivedSector = m_ppSector[ pSector->GetArrayIndexX() ][ pSector->GetArrayIndexZ() ];

	ASSERT( pRetrivedSector == pSector );
	if( pRetrivedSector != pSector ) return FALSE;
#endif

	ASSERT( IsLoadingCompactData() || IsLoadingDetailData() );

	// ���� ���� ����Ÿ�� �ε��Ѵ�..
	BOOL bRet;
	
	if( bServer )
	{
		bRet = pSector->LoadSectorServer();
	}
	else
	{
		bRet = pSector->LoadSector( IsLoadingCompactData() , IsLoadingDetailData() );
	}
	if( bRet )
	{
		// Sector�� ���¸� �ٲ۴�.
		pSector->m_lStatus |= ApWorldSector::STATUS_LOADED;

		EnumCallback(APMMAP_CB_ID_LOAD_SECOTR, pSector, NULL);
	}

	return bRet;
}

// ���� ��ŷ �߰� ��ǵ�..
void	ApmMap::AddObjectBlocking		( AuBLOCKING * pBlocking  , INT32 count )
{
	/*
	if( count == 0 )
	{
		// ��ŷ������ �����ϴ�.
		TRACEFILE(ALEF_ERROR_FILENAME, "AddObjectBlocking ��ŷ ������ �������� �ʽ��ϴ�.\n" );
		return;
	}

	for( int x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
	{
		for( int z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
		{
			if( m_ppSector[ x ][ z ] )
			{
				m_ppSector[ x ][ z ]->AddBlocking( pBlocking , count , TBF_OBJECT );
			}
		}
	}
	*/
}

void	ApmMap::ClearAllObjectBlocking	()	// ������Ʈ ��ŷ ���� ������ �̰� ȣ���ؼ� Ŭ������ �� �ٽ� �ۼ��Ѵ�.
{
	/*
	for( int x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
	{
		for( int z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
		{
			if( m_ppSector[ x ][ z ] )
			{
				m_ppSector[ x ][ z ]->ClearObjectBlocking();
			}
		}
	}
	*/
}

void	ApmMap::ClearAllHeightPool	()	// ������Ʈ ��ŷ ���� ������ �̰� ȣ���ؼ� Ŭ������ �� �ٽ� �ۼ��Ѵ�.
{
	for( int x = 0 ; x < MAP_WORLD_INDEX_WIDTH ; ++x )
	{
		for( int z = 0 ; z < MAP_WORLD_INDEX_HEIGHT ; ++z )
		{
			if( m_ppSector[ x ][ z ] )
			{
				m_ppSector[ x ][ z ]->FreeHeightPool();
			}
		}
	}
}

FLOAT	ApmMap::GetHeight			( FLOAT x , FLOAT z )
{
	ApWorldSector * pSector = GetSector( x , z );

	if( pSector )
	{
		return pSector->GetHeight( x , 0.0f , z );
	}

	return ALEF_SECTOR_SEGMENT_DEFAULT_HEIGHT;
}

UINT8	ApmMap::GetType				( FLOAT x , FLOAT z	)	// �ٴ��� Ÿ���� ����.
{
	ApWorldSector * pSector = GetSector( x , z );

	if( pSector )
	{
		return pSector->GetType( x , 0.0f , z );
	}

	return APMMAP_MATERIAL_SOIL;
}

CMagUnpackManager * ApmMap::GetUnpackManagerDivisionCompact( INT32 nDivisionIndex )
{
	INT32	nOldestIndex		= -1;
	UINT32	uOldestAccessTime	=  GetTickCount();
	INT32	nEmptyIndex			= -1;

	// �̹� �ε�Ǿ��ִ°� ������ ã�´�..
	for( int i = 0 ; i < MAP_UNPACK_MANAGER_BUF ; ++ i )
	{
		if( m_ArrayUnpackBufferCompact[ i ].pUnpack )
		{
			if( m_ArrayUnpackBufferCompact[ i ].nDivisionIndex == nDivisionIndex	)
			{
				// ���ð� ������Ʈ..
				m_ArrayUnpackBufferCompact[ i ].uLastAccessTime	= GetTickCount();
				return m_ArrayUnpackBufferCompact[ i ].pUnpack;
			}

			// ������ �༮�� ���..
			if( m_ArrayUnpackBufferCompact[ i ].uLastAccessTime < uOldestAccessTime )
			{
				nOldestIndex		= i;
				uOldestAccessTime	= m_ArrayUnpackBufferCompact[ i ].uLastAccessTime;
			}
		}
		else
		{
			// ���ε��� ����..
			if( nEmptyIndex == -1 ) nEmptyIndex = i;
		}
	}

	// ����!.. ���� �ε�..
	CompactDataUnpackManagerBufferElement	* pBufferElement;
	ASSERT( nEmptyIndex < MAP_UNPACK_MANAGER_BUF );
	if( nEmptyIndex >= 0 )
	{
		pBufferElement	= &m_ArrayUnpackBufferCompact[ nEmptyIndex ];
	}
	else
	{
		pBufferElement	= &m_ArrayUnpackBufferCompact[ nOldestIndex ];
	}

	// ���۸� ����..
	if( pBufferElement->pUnpack )
	{
		pBufferElement->pUnpack->CloseFile();
		delete pBufferElement->pUnpack;
		pBufferElement->pUnpack = NULL;
	}

	// ���!

	// ������ -_-;;
	char	strFilename[ 1024 ];
	char	strFullPath[ 1024 ];
	CMagUnpackManager	* pUnpack;

	wsprintf( strFilename , ALEF_WORLD_COMPACT_SECTOR_FILE_NAME_FORMAT , nDivisionIndex );
	wsprintf( strFullPath , "%s\\%s" , GetWorldDirectory() , strFilename );
		
	VERIFY( pUnpack	= new CMagUnpackManager );
	if( pUnpack->SetFile( strFullPath ) )
	{
		// ��ȣ!
		pBufferElement->pUnpack			= pUnpack		;
		pBufferElement->nDivisionIndex	= nDivisionIndex;
		pBufferElement->uLastAccessTime	= GetTickCount();
		return pUnpack;
	}
	else
	{
		// ������ ���ų� ����..
		// TRACE( "%d Division �� �ε� �� �� �����ϴ�.\n" );

		// 2005/01/11
		// ū���� �����Ƿ� Ʈ���̽� ����..

		//TRACEFILE(ALEF_ERROR_FILENAME, "%d Division �� �ε� �� �� �����ϴ�.\n" );
		delete pUnpack;
		return NULL;		
	}
}	

void				ApmMap::FlushUnpackManagerDivision()
{
	for( int i = 0 ; i < MAP_UNPACK_MANAGER_BUF ; ++ i )
	{
		if( m_ArrayUnpackBufferCompact[ i ].pUnpack )
		{
			m_ArrayUnpackBufferCompact[ i ].pUnpack->CloseFile();
			delete m_ArrayUnpackBufferCompact[ i ].pUnpack;
			m_ArrayUnpackBufferCompact[ i ].pUnpack = NULL;
		}
		if( m_ArrayUnpackBufferGeometry[ i ].pUnpack )
		{
			m_ArrayUnpackBufferGeometry[ i ].pUnpack->CloseFile();
			delete m_ArrayUnpackBufferGeometry[ i ].pUnpack;
			m_ArrayUnpackBufferGeometry[ i ].pUnpack = NULL;
		}
		if( m_ArrayUnpackBufferMoonee[ i ].pUnpack )
		{
			m_ArrayUnpackBufferMoonee[ i ].pUnpack->CloseFile();
			delete m_ArrayUnpackBufferMoonee[ i ].pUnpack;
			m_ArrayUnpackBufferMoonee[ i ].pUnpack = NULL;
		}
	}
}

CMagUnpackManager * ApmMap::GetUnpackManagerDivisionGeometry( INT32 nDivisionIndex )
{
	INT32	nOldestIndex		= -1;
	UINT32	uOldestAccessTime	=  GetTickCount();
	INT32	nEmptyIndex			= -1;

	// �̹� �ε�Ǿ��ִ°� ������ ã�´�..
	for( int i = 0 ; i < MAP_UNPACK_MANAGER_BUF ; ++ i )
	{
		if( m_ArrayUnpackBufferGeometry[ i ].pUnpack )
		{
			if( m_ArrayUnpackBufferGeometry[ i ].nDivisionIndex == nDivisionIndex	)
			{
				// ���ð� ������Ʈ..
				m_ArrayUnpackBufferGeometry[ i ].uLastAccessTime	= GetTickCount();
				return m_ArrayUnpackBufferGeometry[ i ].pUnpack;
			}

			// ������ �༮�� ���..
			if( m_ArrayUnpackBufferGeometry[ i ].uLastAccessTime < uOldestAccessTime )
			{
				nOldestIndex		= i;
				uOldestAccessTime	= m_ArrayUnpackBufferGeometry[ i ].uLastAccessTime;
			}
		}
		else
		{
			// ���ε��� ����..
			if( nEmptyIndex == -1 ) nEmptyIndex = i;
		}
	}

	// ����!.. ���� �ε�..
	CompactDataUnpackManagerBufferElement	* pBufferElement;
	ASSERT( nEmptyIndex < MAP_UNPACK_MANAGER_BUF );
	if( nEmptyIndex >= 0 )
	{
		pBufferElement	= &m_ArrayUnpackBufferGeometry[ nEmptyIndex ];
	}
	else
	{
		pBufferElement	= &m_ArrayUnpackBufferGeometry[ nOldestIndex ];
	}

	// ���۸� ����..
	if( pBufferElement->pUnpack )
	{
		pBufferElement->pUnpack->CloseFile();
		delete pBufferElement->pUnpack;
		pBufferElement->pUnpack = NULL;
	}

	// ���!

	// ������ -_-;;
	char	strFilename[ 1024 ];
	char	strFullPath[ 1024 ];
	CMagUnpackManager	* pUnpack;

	wsprintf( strFilename , SECTOR_GEOMETRY_BLOCK_FILE , nDivisionIndex );
	wsprintf( strFullPath , "%s\\%s" , GetMapGeometryDirectory() , strFilename );
		
	VERIFY( pUnpack	= new CMagUnpackManager );
	if( pUnpack->SetFile( strFullPath ) )
	{
		if( pUnpack->GetFileCount() != 256 )
		{
			// ���� ������ ���ɼ��� ����..
			TRACE( "���� ������ 256���� �ƴ� ���� �������̶� �ε� ����\n" );
			delete pUnpack;
			return NULL;
		}

		// ��ȣ!
		pBufferElement->pUnpack			= pUnpack		;
		pBufferElement->nDivisionIndex	= nDivisionIndex;
		pBufferElement->uLastAccessTime	= GetTickCount();
		return pUnpack;
	}
	else
	{
		// ������ ���ų� ����..
		//TRACE( "%d Division �� �ε� �� �� �����ϴ�.\n" );
		//TRACEFILE(ALEF_ERROR_FILENAME, "%d Division �� �ε� �� �� �����ϴ�.\n" );
		delete pUnpack;
		return NULL;		
	}
}	

CMagUnpackManager * ApmMap::GetUnpackManagerDivisionMoonee( INT32 nDivisionIndex )
{
	INT32	nOldestIndex		= -1;
	UINT32	uOldestAccessTime	=  GetTickCount();
	INT32	nEmptyIndex			= -1;

	// �̹� �ε�Ǿ��ִ°� ������ ã�´�..
	for( int i = 0 ; i < MAP_UNPACK_MANAGER_BUF ; ++ i )
	{
		if( m_ArrayUnpackBufferMoonee[ i ].pUnpack )
		{
			if( m_ArrayUnpackBufferMoonee[ i ].nDivisionIndex == nDivisionIndex	)
			{
				// ���ð� ������Ʈ..
				m_ArrayUnpackBufferMoonee[ i ].uLastAccessTime	= GetTickCount();
				return m_ArrayUnpackBufferMoonee[ i ].pUnpack;
			}

			// ������ �༮�� ���..
			if( m_ArrayUnpackBufferMoonee[ i ].uLastAccessTime < uOldestAccessTime )
			{
				nOldestIndex		= i;
				uOldestAccessTime	= m_ArrayUnpackBufferMoonee[ i ].uLastAccessTime;
			}
		}
		else
		{
			// ���ε��� ����..
			if( nEmptyIndex == -1 ) nEmptyIndex = i;
		}
	}

	// ����!.. ���� �ε�..
	CompactDataUnpackManagerBufferElement	* pBufferElement;
	ASSERT( nEmptyIndex < MAP_UNPACK_MANAGER_BUF );
	if( nEmptyIndex >= 0 )
	{
		pBufferElement	= &m_ArrayUnpackBufferMoonee[ nEmptyIndex ];
	}
	else
	{
		pBufferElement	= &m_ArrayUnpackBufferMoonee[ nOldestIndex ];
	}

	// ���۸� ����..
	if( pBufferElement->pUnpack )
	{
		pBufferElement->pUnpack->CloseFile();
		delete pBufferElement->pUnpack;
		pBufferElement->pUnpack = NULL;
	}

	// ���!

	// ������ -_-;;
	char	strFilename[ 1024 ];
	char	strFullPath[ 1024 ];
	CMagUnpackManager	* pUnpack;

	wsprintf( strFilename , SECTOR_MOONIE_BLOCK_FILE , nDivisionIndex );
	wsprintf( strFullPath , "%s\\%s" , GetMapMooneeDirectory() , strFilename );
		
	VERIFY( pUnpack	= new CMagUnpackManager );
	if( pUnpack->SetFile( strFullPath ) )
	{
		// ��ȣ!
		pBufferElement->pUnpack			= pUnpack		;
		pBufferElement->nDivisionIndex	= nDivisionIndex;
		pBufferElement->uLastAccessTime	= GetTickCount();
		return pUnpack;
	}
	else
	{
		// ������ ���ų� ����..
		//TRACE( "%d Division �� �ε� �� �� �����ϴ�.\n" );
		//TRACEFILE(ALEF_ERROR_FILENAME, "%d Division �� �ε� �� �� �����ϴ�.\n" );
		delete pUnpack;
		return NULL;		
	}
}	

INT32	ApmMap::CheckBlockingInfo		( INT32 x , INT32 z , BLOCKINGTYPE eType )
{
	ApWorldSector	* pSector;

	pSector = GetSectorByArray( x >> 4 , 0 , z >> 4 );

	if( NULL == pSector )
	{
		// ������ (2004-02-04 ���� 4:37:58) : 
		// ���� ����Ÿ�� ����!?
		// �����ص� ���� ������ �������ּ���
		ASSERT( !"���͵���Ÿ�� �����" );
		return NO_BLOCKING;
	}

	INT32	lx = ( x % 16 );
	INT32	lz = ( z % 16 );

	ApCompactSegment * pSegment = pSector->C_GetSegment( lx , lz );
	if( pSegment )
	{
		switch( eType )
		{
		default:
		case GROUND:
			if( pSegment->stTileInfo.GetGeometryBlocking() )
			{
				return GEOMETRY_BLOCKING;
			}
			else return NO_BLOCKING;
		case SKY:
			if( pSegment->stTileInfo.GetSkyBlocking() )
			{
				return GEOMETRY_BLOCKING;
			}
			else return NO_BLOCKING;
		case UNDERGROUND:
			return GEOMETRY_BLOCKING;
		case GHOST:
			return NO_BLOCKING;
		}
	}
	else
	{
		// �̻��� �̻���..
		return NO_BLOCKING;
	}
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

INT32	ApmMap::AddBindType			()
{
// return Binding Type 
// ���ε� ��� �߰��� ������ �Ҵ��ϰ� �ش� ����� �ε����� �����Ѵ�. 

	BindList	list;
	INT32		nType = m_listBind.GetCount();

	if( m_listBind.AddTail( list ) )
	{
		return nType;
	}
	else
	{
		ASSERT( !"-_-?;;;" );
		return -1;
	}
}

void	ApmMap::ClearAllBindData	()
{
	// Clean up..

	m_listBind.RemoveAll();
}

BindList	* ApmMap::GetBindList	( INT32 nBindIndex )
{
	ASSERT( nBindIndex < m_listBind.GetCount() );
	
	AuNode< BindList >	* pNode = m_listBind.GetHeadNode();
	while( pNode )
	{
		if( nBindIndex == 0 ) return &pNode->GetData();

		pNode = pNode->GetNextNode();
		--nBindIndex;
	}

	// ���ö�!..
	return NULL;
}

BOOL	ApmMap::AddBindElementRect	( INT32 nBindIndex , INT32 nData , AuPOS pos , FLOAT width , FLOAT height	)
{
	BindList * pList = GetBindList( nBindIndex );
	ASSERT( NULL != pList	);
	ASSERT( 0.0f < width	);
	ASSERT( 0 != nData		);	// 0�� �ƴϾ���Ѵ�. ���� ��������. 0�� ������.

	if( NULL == pList ) return FALSE;

	BindElement	element;
	element.nIndex			= nData	;
	element.stData.center	= pos	;
	element.stData.radius	= width	;
	element.stData.height	= height;

	pList->AddTail( element );
	return TRUE;
}

BOOL	ApmMap::AddBindElementCircle( INT32 nBindIndex , INT32 nData , AuPOS pos , FLOAT radius					)
{
	// Height �� 0 �ϰ�� �� ó���� �ϹǷ�.. �׳� �ڽ�ó�� �Լ� �̿�.
	return AddBindElementRect( nBindIndex , nData , pos , radius , 0.0f );
}

BOOL	ApmMap::IsChangedBind		( INT32 nBindIndex , AuPOS prev , AuPOS next	)
{
	INT32	nPrev	= GetBindData( nBindIndex , prev );
	INT32	nNext	= GetBindData( nBindIndex , next );

	if( nPrev != nNext )	return TRUE	;
	else					return FALSE;
}

INT32	ApmMap::GetBindData			( INT32 nBindIndex , AuPOS pos					)
{
	BindList * pList = GetBindList( nBindIndex );
	ASSERT( NULL != pList	);
	if( NULL == pList ) return 0;	// ������.

	// ����� ������ ��ġ�� ��/�ڽ��� ����ġ �ʰ� �����Ƿ����� ����սô�.
	// �׳� ����Ʈ �����鼭 ���� ���ԵŴ°Ÿ� �ش� �ε����� �����Ѵ�.

	AuNode< BindElement >	*	pNode		= pList->GetHeadNode();
	BindElement				*	pElement	;
	FLOAT						fDistance	;

	while( pNode )
	{
		pElement = &pNode->GetData();

		if( pElement->stData.height == 0.0f )
		{
			// ��Ŭ
			fDistance	=	AUPOS_DISTANCE_XZ( pElement->stData.center , pos );
			if( fDistance < pElement->stData.radius )
			{
				return pElement->nIndex;
			}
		}
		else
		{
			// ��Ʈ.
			if( pElement->stData.center.x < pos.x && pos.x < pElement->stData.center.x + pElement->stData.radius	&&
				pElement->stData.center.z < pos.z && pos.z < pElement->stData.center.z + pElement->stData.height	)
			{
				return pElement->nIndex;
			}
		}

		pNode = pNode->GetNextNode();
	}

	// �ӽö�!
	return 0;
}

// ���� Region���� Ȯ�� 2008.05.30 iluvs
BOOL	ApmMap::IsSameRegion(RegionTemplate* pstBase, RegionTemplate* pstTarget)
{
	if(!pstBase || !pstTarget)
		return FALSE;
	
	if(pstBase->nIndex == pstTarget->nIndex) 
		return TRUE;

	return FALSE;
}

// ���� Region���� Ȯ�� (�����ε����� ��). 2008.05.30 iluvs
BOOL	ApmMap::IsSameRegionInvolveParent(RegionTemplate* pstBase, RegionTemplate* pstTarget)
{
	if(!pstBase || !pstTarget)
		return FALSE;

	if(pstBase->nParentIndex >= 0)
	{
		if(pstBase->nParentIndex == pstTarget->nParentIndex)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


stLoadedDivision *	ApmMap::GetLoadedDivisionInfo	( UINT32 uDivisionIndex )
{
	AuNode< stLoadedDivision > * pNode	= m_listLoadedDivision.GetHeadNode();
	stLoadedDivision * pDivision;

	while( pNode )
	{
		pDivision = & pNode->GetData();

		if( pDivision->GetDivisionIndex() == uDivisionIndex )
			return pDivision;

		pNode = pNode->GetNextNode();
	}

	return NULL;
}

BOOL				ApmMap::EnumLoadedDivision		( ApModuleDefaultCallBack pCallback , PVOID pClass )
{
	ASSERT( NULL != pCallback );
	
	AuNode< stLoadedDivision > * pNode	= m_listLoadedDivision.GetHeadNode();
	stLoadedDivision * pDivision;

	while( pNode )
	{
		pDivision = & pNode->GetData();

		if( !pCallback( ( PVOID ) pDivision , pClass , NULL ) )
		{
			TRACE( "ApmMap::EnumLoadedDivision Return false;\n" );
			return FALSE;
		}

		pNode = pNode->GetNextNode();
	}

	// �� ���Ҵ�..
	return TRUE;
}

// Low level function
stLoadedDivision *	ApmMap::AddLoadedDivision		( UINT32 uDivisionIndex )
{
	// �ߺ� �˻�..

	stLoadedDivision * pDivision;
	pDivision = GetLoadedDivisionInfo( uDivisionIndex );

	if( pDivision )
	{
		// Ref Count Up..
		// ������ (2004-06-30 ���� 4:47:42) : �ϴ� ���⼭�� Ref Count�� üũ���� ����..
		// pDivision->AddRef();
		return pDivision;
	}
	else
	{
		stLoadedDivision	stDiv;
		stDiv.SetDivisionIndex(	uDivisionIndex );

		// ������ (2004-06-30 ���� 4:47:42) : �ϴ� ���⼭�� Ref Count�� üũ���� ����..
		// stDiv.AddRef();

		m_listLoadedDivision.AddTail( stDiv );
		AuNode< stLoadedDivision > * pNode = m_listLoadedDivision.GetTailNode();

		ASSERT( NULL != pNode );
		return & pNode->GetData();
	}
}

BOOL				ApmMap::RemoveLoadedDivision	( UINT32 uDivisionIndex )
{
//	stLoadedDivision * pDivision;
//	pDivision = GetLoadedDivisionInfo( uDivisionIndex );
//
//	if( NULL == pDivision )
//	{
//		// �ӽö� -_-;;
//		return FALSE;
//	}
//	
//	// ������ (2004-06-30 ���� 4:47:42) : �ϴ� ���⼭�� Ref Count�� üũ���� ����..
//	//	pDivision->DecreaseRef();

	AuNode< stLoadedDivision > * pNode	= m_listLoadedDivision.GetHeadNode();
	stLoadedDivision * pDivision;

	while( pNode )
	{
		pDivision = & pNode->GetData();

		if( pDivision->GetDivisionIndex() == uDivisionIndex )
		{
			m_listLoadedDivision.RemoveNode( pNode );
			return TRUE;
		}

		pNode = pNode->GetNextNode();
	}

	// �ӽö�! ...
	return FALSE;
}

BOOL				ApmMap::AddTemplate		( RegionTemplate * pTemplate )
{
	ASSERT( NULL != pTemplate );
	// �ε��� �ߺ��ϴ��� �˻���..

	AuNode< RegionTemplate > * pNode = m_listTemplate.GetHeadNode();
	RegionTemplate * pTemplateInList;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		if( pTemplateInList->nIndex	== pTemplate->nIndex )
		{
			// �ߺ�
			TRACE( "�ߺ��̻�~\n" );
			return FALSE;
		}

		pNode = pNode->GetNextNode();
	}
	
	m_listTemplate.AddTail( * pTemplate );

	return TRUE;
}

BOOL				ApmMap::RemoveTempate	( int nIndex )
{
	AuNode< RegionTemplate > * pNode = m_listTemplate.GetHeadNode();
	RegionTemplate * pTemplateInList;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		if( pTemplateInList->nIndex	== nIndex )
		{
			m_listTemplate.RemoveNode( pNode );
			return TRUE;
		}

		pNode = pNode->GetNextNode();
	}
	
	// �׷��� ����..
	return FALSE;
}

ApmMap::RegionTemplate *	ApmMap::GetTemplate		( int nIndex )
{
	if (nIndex < 0)
		return NULL;

	AuNode< RegionTemplate > * pNode = m_listTemplate.GetHeadNode();
	RegionTemplate * pTemplateInList;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		if( pTemplateInList->nIndex	== nIndex )
		{
			return pTemplateInList;
		}

		pNode = pNode->GetNextNode();
	}
	
	// �׷��� ����..
	return NULL;
}

ApmMap::RegionTemplate *	ApmMap::GetTemplate		( char * pstrRegionName )
{
	if (!pstrRegionName || !pstrRegionName[0])
		return NULL;

	AuNode< RegionTemplate > * pNode = m_listTemplate.GetHeadNode();
	RegionTemplate * pTemplateInList;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		if (strcmp(pTemplateInList->pStrName, pstrRegionName) == 0)
			return pTemplateInList;

		pNode = pNode->GetNextNode();
	}
	
	// �׷��� ����..
	return NULL;
}

BOOL	ApmMap::SaveTemplate( char * pFileName , BOOL bEncryption )
{
	AuIniManagerA	iniManager;
	iniManager.SetPath( pFileName );

	RegionTemplate * pTemplateInList;
	AuNode< RegionTemplate >	* pNode				= m_listTemplate.GetHeadNode();
	char	strSection [ 256 ];

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		wsprintf( strSection , g_strINIRegionTemplateSection , pTemplateInList->nIndex );

		iniManager.SetValue		( strSection , ( char * ) g_strINIRegionTemplateKeyName		, pTemplateInList->pStrName			);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyParentIndex	, pTemplateInList->nParentIndex		);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyItemSection , pTemplateInList->nUnableItemSectionNum	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyType		, pTemplateInList->ti.nType			);
		iniManager.SetValue		( strSection , ( char * ) g_strINIRegionTemplateKeyComment	, pTemplateInList->pStrComment		);

		iniManager.SetValueF	( strSection , ( char * ) g_strINIRegionTemplateKeyResurrectionX , pTemplateInList->stResurrectionPos.x );
		iniManager.SetValueF	( strSection , ( char * ) g_strINIRegionTemplateKeyResurrectionZ , pTemplateInList->stResurrectionPos.z );
		
		// ������ (2005-05-24 ���� 2:11:49) : 
		// ����� ���� �߰�.
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyWorldMap	, pTemplateInList->nWorldMapIndex	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeySkySet	, pTemplateInList->nSkyIndex		);

		// �þ߰Ÿ� �߰�
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyVDistance, ( INT32 ) pTemplateInList->fVisibleDistance	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyTVHeight, ( INT32 ) pTemplateInList->fMaxCameraHeight	);
		
		// �������� �߰�
		if( pTemplateInList->nLevelLimit )
		{
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyLevelLimit, pTemplateInList->nLevelLimit	);
		}
		if( pTemplateInList->nLevelMin )
		{
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyLevelMin, pTemplateInList->nLevelMin	);
		}	

		// ������ (2005-10-31 ���� 4:56:57) : 
		// ���ε� ���� �߰�
		if( pTemplateInList->ti.stType.bZoneLoadArea )
		{
			iniManager.SetValueF	( strSection , ( char * ) g_strINIRegionTemplateKeyZoneSrcX		, pTemplateInList->zi.fSrcX				);
			iniManager.SetValueF	( strSection , ( char * ) g_strINIRegionTemplateKeyZoneSrcZ		, pTemplateInList->zi.fSrcZ				);
			iniManager.SetValueF	( strSection , ( char * ) g_strINIRegionTemplateKeyZoneHeight	, pTemplateInList->zi.fHeightOffset		);
			iniManager.SetValueF	( strSection , ( char * ) g_strINIRegionTemplateKeyZoneDstX		, pTemplateInList->zi.fDstX				);
			iniManager.SetValueF	( strSection , ( char * ) g_strINIRegionTemplateKeyZoneDstZ		, pTemplateInList->zi.fDstZ				);

			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionTemplateKeyZoneRadius	, pTemplateInList->zi.nRadius			);
		}

		pNode = pNode->GetNextNode();
	}

	iniManager.WriteFile(0, bEncryption);

	return TRUE;
}

BOOL	ApmMap::LoadTemplate( char * pFileName , BOOL bDecryption )
{
	AuIniManagerA	iniManager;

	iniManager.SetPath(	pFileName	);

	if( iniManager.ReadFile(0, bDecryption) )
	{
		int		nSectionCount	;
		int		nIndex			;

		int		nKeyName		;
		int		nParentIndex	;
		int		nUnableItemSectionNum;
		int		nKeyType		;
		int		nKeyComment		;
		int		nKeyWorldMap	;
		int		nKeySkySet		;
		int		nKeyVDistance	;
		int		nKeyTVHeight	;
		int		nKeyLevelLimit	;
		int		nKeyLevelMin	;
		int		nKeyLevelResurrectionX;
		int		nKeyLevelResurrectionZ;

		nSectionCount	= iniManager.GetNumSection();	

		// �ױ��� �߰���..

		RegionTemplate	stTemplate;

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			nIndex					= atoi( iniManager.GetSectionName( i ) );

			nKeyName				= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyName		  );
			nParentIndex			= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyParentIndex );
			nUnableItemSectionNum	= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyItemSection );
			nKeyType				= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyType		  );
			nKeyComment				= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyComment	  );
			nKeyWorldMap			= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyWorldMap	  );
			nKeySkySet				= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeySkySet	  );
			nKeyVDistance			= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyVDistance	  );
			nKeyTVHeight			= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyTVHeight	  );
			nKeyLevelLimit			= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyLevelLimit  );
			nKeyLevelMin			= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyLevelMin	  );
			nKeyLevelResurrectionX	= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyResurrectionX );
			nKeyLevelResurrectionZ	= iniManager.FindKey( i , ( char * ) g_strINIRegionTemplateKeyResurrectionZ );
			

			stTemplate.nIndex		= nIndex;
			strncpy( stTemplate.pStrName , iniManager.GetValue	( i , nKeyName ) , 32 );

			if( IsKeyAvailable(nParentIndex) )
			{
				stTemplate.nParentIndex		= atoi( iniManager.GetValue	( i , nParentIndex ) );
			}
			else
			{
				stTemplate.nParentIndex		= -1;
			}

			if( IsKeyAvailable(nUnableItemSectionNum) )
			{
				stTemplate.nUnableItemSectionNum	=	atoi( iniManager.GetValue( i , nUnableItemSectionNum ) );
			}
			else
			{
				stTemplate.nUnableItemSectionNum	=	0;
			}

			if( IsKeyAvailable(nKeyLevelResurrectionX) )
			{
				stTemplate.stResurrectionPos.x		=	(float)atof( iniManager.GetValue( i , nKeyLevelResurrectionX ) );
			}

			if( IsKeyAvailable(nKeyLevelResurrectionZ) )
			{
				stTemplate.stResurrectionPos.z		=	(float)atof( iniManager.GetValue( i , nKeyLevelResurrectionZ ) );
			}


			// ������ (2005-05-24 ���� 2:11:32) : 
			// ����� �ε����� ������ �ش� ������ ����.
			if( IsKeyAvailable( nKeyWorldMap ) )
			{
				stTemplate.nWorldMapIndex	= atoi( iniManager.GetValue	( i , nKeyWorldMap ) );
			}
			else
			{
				// �ƹ��͵� ������ ������ 1��° ���� ������.
				stTemplate.nWorldMapIndex	= 1;
			}

			// ������ (2005-05-26 ���� 5:21:46) : 
			// ��ī�̼� �߰�.
			if( IsKeyAvailable( nKeySkySet ) )
			{
				stTemplate.nSkyIndex	= atoi( iniManager.GetValue	( i , nKeySkySet ) );
			}
			else
			{
				// �ƹ��͵� ������ ���� ����Ʈ �� ����.
				int nDefaultSky = 0;
				switch( nIndex )
				{
				case 0	:	nDefaultSky = 13	;break;
				case 1	:	nDefaultSky = 37	;break;
				case 2	:	nDefaultSky = 0		;break;
				case 3	:	nDefaultSky = 9		;break;
				case 4	:	nDefaultSky = 39	;break;
				case 5	:	nDefaultSky = 14	;break;
				case 6	:	nDefaultSky = 11	;break;
				case 7	:	nDefaultSky = 1		;break;
				case 8	:	nDefaultSky = 14	;break;
				case 9	:	nDefaultSky = 13	;break;
				case 10	:	nDefaultSky = 0		;break;
				case 11	:	nDefaultSky = 41	;break;
				case 12	:	nDefaultSky = 41	;break;
				case 13	:	nDefaultSky = 42	;break;
				case 14	:	nDefaultSky = 14	;break;
				case 15	:	nDefaultSky = 44	;break;
				case 16	:	nDefaultSky = 44 	;break;
				case 17	:	nDefaultSky = 44	;break;
				case 18	:	nDefaultSky = 44	;break;
				case 19	:	nDefaultSky = 44	;break;
				case 20	:	nDefaultSky = 44	;break;
				case 21	:	nDefaultSky = 44	;break;
				case 22 :	nDefaultSky = 6		;break;
				case 23 :	nDefaultSky = 44	;break;
				case 24 :	nDefaultSky = 46	;break;

				// ������ (2005-05-12 ���� 2:07:59) : 
				case 25 :	nDefaultSky = 56	;break;	// �����̾���� - ����_�������_01(��)
				case 26 :	nDefaultSky = 57	;break; // �븰 - ������(Ÿ��)�븰
				case 27 :	nDefaultSky = 51	;break; // �ٸ����� - ������_�ٸ�����
				case 28 :	nDefaultSky = 52	;break; // ���� - ������(Ÿ��)����

				default:
					nDefaultSky = 13;
				}

				stTemplate.nSkyIndex	= nDefaultSky;
			}

			if( IsKeyAvailable( nKeyVDistance ) )
			{
				stTemplate.fVisibleDistance	= ( FLOAT ) atoi( iniManager.GetValue	( i , nKeyVDistance ) );
			}
			else
			{
				stTemplate.fVisibleDistance	= 1200.0f; // ����Ʈ��..
			}

			if( IsKeyAvailable( nKeyTVHeight ) )
			{
				stTemplate.fMaxCameraHeight	= ( FLOAT ) atoi( iniManager.GetValue	( i , nKeyTVHeight ) );
			}
			else
			{
				stTemplate.fMaxCameraHeight	= 2750.0f; // ����Ʈ��..
			}

			if( IsKeyAvailable( nKeyLevelLimit ) )
			{
				stTemplate.nLevelLimit	= atoi( iniManager.GetValue	( i , nKeyLevelLimit ) );
			}
			else
			{
				stTemplate.nLevelLimit	= 0; // ����Ʈ��..
			}

			if( IsKeyAvailable( nKeyLevelMin ) )
			{
				stTemplate.nLevelMin	= atoi( iniManager.GetValue	( i , nKeyLevelMin ) );
			}
			else
			{
				stTemplate.nLevelMin	= 0; // ����Ʈ��..
			}	

			stTemplate.ti.nType			= atoi( iniManager.GetValue	( i , nKeyType ) );
			strncpy( stTemplate.pStrComment , iniManager.GetValue	( i , nKeyComment ) , 128 );

			if( stTemplate.ti.stType.bZoneLoadArea )
			{
				int nKeySrcX			= iniManager.FindKey( i , ( TCHAR * ) g_strINIRegionTemplateKeyZoneSrcX		);
				int nKeySrcZ			= iniManager.FindKey( i , ( TCHAR * ) g_strINIRegionTemplateKeyZoneSrcZ		);
				int nKeyHeightOffset	= iniManager.FindKey( i , ( TCHAR * ) g_strINIRegionTemplateKeyZoneHeight	);
				int nKeyDstX			= iniManager.FindKey( i , ( TCHAR * ) g_strINIRegionTemplateKeyZoneDstX		);
				int nKeyDstZ			= iniManager.FindKey( i , ( TCHAR * ) g_strINIRegionTemplateKeyZoneDstZ		);
				int nKeyRadius			= iniManager.FindKey( i , ( TCHAR * ) g_strINIRegionTemplateKeyZoneRadius	);

				stTemplate.zi.fSrcX				= ( FLOAT ) _tstof( iniManager.GetValue	( i , nKeySrcX			 ) );
				stTemplate.zi.fSrcZ				= ( FLOAT ) _tstof( iniManager.GetValue	( i , nKeySrcZ			 ) );
				stTemplate.zi.fHeightOffset		= ( FLOAT ) _tstof( iniManager.GetValue	( i , nKeyHeightOffset	 ) );
				stTemplate.zi.fDstX				= ( FLOAT ) _tstof( iniManager.GetValue	( i , nKeyDstX			 ) );
				stTemplate.zi.fDstZ				= ( FLOAT ) _tstof( iniManager.GetValue	( i , nKeyDstZ			 ) );
				stTemplate.zi.nRadius			= _tstoi( iniManager.GetValue	( i , nKeyRadius		 ) );
			}

			AddTemplate( &stTemplate );
		}

		return TRUE;
	}
	else
	{
		TRACE( "���ø� ������ ����!.\n" );
		return FALSE;
	}
}


BOOL						ApmMap::AddRegion( int nIndex , INT32 nStartX , INT32 nStartZ , INT32 nEndX , INT32 nEndZ , INT32 nKind )
{
	RegionElement	element;

	element.nIndex	= nIndex	;
	element.nStartX	= nStartX	;
	element.nStartZ	= nStartZ	;
	element.nEndX	= nEndX		;
	element.nEndZ	= nEndZ		;
	element.nKind	= nKind		;

	// �����ؼ� �־���ϴµ�..
	// ������...

	m_listElement.AddTail( element );

	return TRUE;
}

AuNode< ApmMap::RegionElement > *	ApmMap::GetRegion( INT32 nOffsetX , INT32 nOffsetZ )
{
	// ������Ʈ ����Ʈ �ڿ��� ���� �˻��� �ִ´�.
	// ���������� �������� ���� ���� ������Ʈ�� ���� �Ǵ� �ý���..
	// ���������� Ctrl �� ������ � ������ ���� ���� Ȯ���� �����ϴ�.

	AuNode< RegionElement > * pNode = m_listElement.GetTailNode();
	RegionElement * pElement;

	while( pNode )
	{
		pElement	= & pNode->GetData();

		if( pElement->nStartX <= nOffsetX && nOffsetX <= pElement->nEndX	&&
			pElement->nStartZ <= nOffsetZ && nOffsetZ <= pElement->nEndZ	)
		{
			return pNode;
		}

		pNode = pNode->GetPrevNode();
	}

	return NULL;
	// �׷��� ����..
}
BOOL						ApmMap::RemoveRegion( AuNode< RegionElement > * pNode )
{
	ASSERT( NULL != pNode );

	if( NULL == pNode ) return FALSE;

	m_listElement.RemoveNode( pNode );
	return TRUE;
}

void						ApmMap::RemoveAllRegion()
{
	m_listElement.RemoveAll();
}

BOOL						ApmMap::SaveRegion( char * pFileName , BOOL bEncryption )
{
	AuIniManagerA	iniManager;
	iniManager.SetPath( pFileName );

	RegionElement * pElement;
	AuNode< RegionElement >	* pNode				= m_listElement.GetHeadNode();
	char	strSection [ 256 ];

	int	nSeq = 0;

	while( pNode )
	{
		pElement	= & pNode->GetData();

		wsprintf( strSection , g_strINIRegionElementSection , nSeq++ );

		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementIndex		, pElement->nIndex	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyKind		, pElement->nKind	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyStartX	, pElement->nStartX	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyStartZ	, pElement->nStartZ	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyEndX		, pElement->nEndX	);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyEndZ		, pElement->nEndZ	);

		pNode = pNode->GetNextNode();
	}

	iniManager.WriteFile(0, bEncryption);

	return TRUE;
}

BOOL						ApmMap::LoadRegion( char * pFileName , BOOL bDecryption )
{
	AuIniManagerA	iniManager;

	iniManager.SetPath(	pFileName	);

	if( iniManager.ReadFile(0, bDecryption) )
	{
		int		nSectionCount	;

		int		nKeyIndex	;
		int		nKeyKind	;
		int		nKeyStartX	;
		int		nKeyStartZ	;
		int		nKeyEndX	;
		int		nKeyEndZ	;

		nSectionCount	= iniManager.GetNumSection();	

		// �ױ��� �߰���..

		RegionElement	stElement;

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			nKeyIndex	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementIndex		);
			nKeyKind	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyKind	);
			nKeyStartX	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyStartX	);
			nKeyStartZ	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyStartZ	);
			nKeyEndX	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyEndX	);
			nKeyEndZ	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyEndZ	);

			stElement.nIndex		= atoi( iniManager.GetValue	( i , nKeyIndex		) );
			stElement.nKind			= atoi( iniManager.GetValue	( i , nKeyKind		) );

			stElement.nStartX		= atoi( iniManager.GetValue	( i , nKeyStartX	) );
			stElement.nStartZ		= atoi( iniManager.GetValue	( i , nKeyStartZ	) );
			stElement.nEndX			= atoi( iniManager.GetValue	( i , nKeyEndX		) );
			stElement.nEndZ			= atoi( iniManager.GetValue	( i , nKeyEndZ		) );

			AddRegion( &stElement );
		}
		
		return TRUE;
	}
	else
	{
		TRACE( "���ø� ������ ����!.\n" );
		return FALSE;
	}
}

UINT16		ApmMap::GetRegion( FLOAT x , FLOAT z )
{
	ApWorldSector * pSector = GetSector( x , z );

	if( pSector )
	{
		ApCompactSegment * pSegment;
		pSegment = pSector->C_GetSegment( x , 0.0f , z );

		if( pSegment )
			return pSegment->GetRegion();
	}

	// ����~..
	// ������ (2004-09-17 ���� 4:33:54) : ����Ʈ�� 0�� �´�.
	return (-1);
}

void	ApmMap::CLS_AddSector		( ApWorldSector * pSector )
{
	AuAutoLock Lock(m_csMutexSectorList);
	if (!Lock.Result()) return;

	ASSERT( NULL != pSector );
	// ���� �ε��� ���� ���� ���..

	// ASSERT( m_nCurrentLoadedSectorCount < ALEF_MAX_LOADING_SECTOR_BUFFER );
	if( m_nCurrentLoadedSectorCount >= ALEF_MAX_LOADING_SECTOR_BUFFER )
	{
		//TRACE( "���� ���� �ʰ�!\n" );
		pSector->m_uQueueOffset	= ALEF_MAX_LOADING_SECTOR_BUFFER;
		return;
	}

	if( __IS_ADDED_SECTOR( pSector ) )
	{
		//TRACE( "�̹��߰��� �༮�̴�.. �̷���Ȳ�� �߻��ϸ� �ȵ�����.\n" );
		return;
	}

	m_pCurrentLoadedSectors[ m_nCurrentLoadedSectorCount ] = pSector;
	__SET__SECTOR_OFFSET( pSector , m_nCurrentLoadedSectorCount );
	++m_nCurrentLoadedSectorCount;
}

void	ApmMap::CLS_RemoveSector	( ApWorldSector * pSector )
{
	AuAutoLock Lock(m_csMutexSectorList);
	if (!Lock.Result()) return;

	ASSERT( NULL != pSector );
	if( ! __IS_ADDED_SECTOR( pSector ) )
	{
		// ����..
		return;
	}
	if( pSector->m_uQueueOffset >= m_nCurrentLoadedSectorCount )
		return;	// ���� �����Ǽ� �߰� ���� �༮�̴�.

	if( pSector != m_pCurrentLoadedSectors[ pSector->m_uQueueOffset ] )
	{
		TRACE( "AgcmMap::CLS_RemoveSector �� ������ ���� �ȵǴµ�..(%d)\n" , pSector->m_uQueueOffset );
		return;
	}

	// �ǳ����� ī���� �ִ´�..
	m_pCurrentLoadedSectors[ pSector->m_uQueueOffset ] = 
		m_pCurrentLoadedSectors[ m_nCurrentLoadedSectorCount - 1 ];
	__SET__SECTOR_OFFSET( m_pCurrentLoadedSectors[ pSector->m_uQueueOffset ] , pSector->m_uQueueOffset );

	// �ε��� �ʱ�ȭ..
	pSector->m_uQueueOffset	= -1;

	--m_nCurrentLoadedSectorCount;
}


//////////////////////////////////////////////////

BOOL	ApmMap::LoadDimensionTemplate( char * strFilename )
{
	return FALSE;
}

BOOL	ApmMap::SaveDimensionTemplate( char * strFilename )
{
	return FALSE;
}

BOOL	ApmMap::AddDimensionTemplate( ApmMap::DimensionTemplate * pTemplate )
{
	return FALSE;
}

BOOL	ApmMap::RemoveDimensionTemplate( INT32 nTID )
{
	return FALSE;
}

ApmMap::DimensionTemplate * ApmMap::GetDimensionTemplate( INT32 nTID )
{
	return NULL;
}

BOOL	ApmMap::CreateDimension	( INT32 nTID )
{
	return FALSE;
}

BOOL	ApmMap::DestroyDimension( INT32 nTID , INT32 nIndex )
{
	return FALSE;
}

static const char g_strINIWorldMapSection		[]	= "%d"			;
static const char g_strINIWorldMapKeyComment	[]	= "Comment"		;
static const char g_strINIWorldMapKeyXStart		[]	= "XStart"		;
static const char g_strINIWorldMapKeyZStart		[]	= "ZStart"		;
static const char g_strINIWorldMapKeyXEnd		[]	= "XEnd"		;
static const char g_strINIWorldMapKeyZEnd		[]	= "ZEnd"		;
static const char g_strINIWorldMapKeyItemID		[]	= "ItemID"		;

//////////////////////////////////////////////////////////
BOOL	ApmMap::LoadWorldMap( char * pFileName , BOOL bDecryption )
{
	RemoveAllWorldMap();

	AuIniManagerA	iniManager;

	iniManager.SetPath(	pFileName	);

	if( iniManager.ReadFile(0, bDecryption) )
	{
		int		nSectionCount	;
		int		nIndex			;

		int		nKeyComment		;
		int		nKeyXStart		;
		int		nKeyZStart		;
		int		nKeyXEnd		;
		int		nKeyZEnd		;
		int		nKeyItemID		;

		nSectionCount	= iniManager.GetNumSection();	

		WorldMap	stWorldMap;;

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			nIndex			= atoi( iniManager.GetSectionName( i ) );

			nKeyComment		= iniManager.FindKey( i , ( char * ) g_strINIWorldMapKeyComment	);
			nKeyXStart		= iniManager.FindKey( i , ( char * ) g_strINIWorldMapKeyXStart	);
			nKeyZStart		= iniManager.FindKey( i , ( char * ) g_strINIWorldMapKeyZStart	);
			nKeyXEnd		= iniManager.FindKey( i , ( char * ) g_strINIWorldMapKeyXEnd	);
			nKeyZEnd		= iniManager.FindKey( i , ( char * ) g_strINIWorldMapKeyZEnd	);
			nKeyItemID		= iniManager.FindKey( i , ( char * ) g_strINIWorldMapKeyItemID	);

			stWorldMap.nMID		= nIndex;
			strncpy( stWorldMap.strComment , iniManager.GetValue	( i , nKeyComment ) , WORLDMAP_COMMENT_LENGTH );
			stWorldMap.xStart		= ( FLOAT ) atof( iniManager.GetValue	( i , nKeyXStart ) );
			stWorldMap.zStart		= ( FLOAT ) atof( iniManager.GetValue	( i , nKeyZStart ) );
			stWorldMap.xEnd			= ( FLOAT ) atof( iniManager.GetValue	( i , nKeyXEnd	 ) );
			stWorldMap.zEnd			= ( FLOAT ) atof( iniManager.GetValue	( i , nKeyZEnd	 ) );
			stWorldMap.nMapItemID	= atoi( iniManager.GetValue	( i , nKeyItemID ) );

			m_arrayWorldMap.push_back( stWorldMap );
		}

		return TRUE;
	}
	else
	{
		TRACE( "����� ���ø� ������ ����!.\n" );
		return FALSE;
	}
}

BOOL	ApmMap::SaveWorldMap( char * pFileName , BOOL bEncryption )
{
	AuIniManagerA	iniManager;
	iniManager.SetPath( pFileName );

	WorldMap * pWorldMap;
	char	strSection [ 256 ];

	for( int i = 0 ; i < ( int ) m_arrayWorldMap.size() ; i ++ )
	{
		pWorldMap	= &m_arrayWorldMap[ i ];

		wsprintf( strSection , g_strINIWorldMapSection , pWorldMap->nMID );

		iniManager.SetValue		( strSection , ( char * ) g_strINIWorldMapKeyComment	, pWorldMap->strComment		);
		iniManager.SetValueF	( strSection , ( char * ) g_strINIWorldMapKeyXStart		, pWorldMap->xStart			);
		iniManager.SetValueF	( strSection , ( char * ) g_strINIWorldMapKeyZStart		, pWorldMap->zStart			);
		iniManager.SetValueF	( strSection , ( char * ) g_strINIWorldMapKeyXEnd		, pWorldMap->xEnd			);
		iniManager.SetValueF	( strSection , ( char * ) g_strINIWorldMapKeyZEnd		, pWorldMap->zEnd			);
		iniManager.SetValueI	( strSection , ( char * ) g_strINIWorldMapKeyItemID		, pWorldMap->nMapItemID		);
	}

	iniManager.WriteFile(0, bEncryption);

	return TRUE;
}

// Clean Up
void	ApmMap::RemoveAllWorldMap()
{
	m_arrayWorldMap.clear();
}

ApmMap::WorldMap *	ApmMap::GetWorldMapInfo( INT32 nMID )
{
	ApmMap::WorldMap * pWorld;

	for( int i = 0 ; i < ( int ) m_arrayWorldMap.size() ; i ++ )
	{
		pWorld = &m_arrayWorldMap[ i ];

		if( pWorld->nMID == nMID )
		{
			return pWorld;
		}
	}
	
	// ���� ;;
	return NULL;
}

FLOAT	ApmMap::GetMinHeight		( ApWorldSector * pSector )
{
	ASSERT( NULL != pSector );

	static ApWorldSector *	_spSectorCalculatedJustAgo	= NULL;
	static FLOAT			_sfHeightJustAgo			= 0.0f;

	if( pSector == _spSectorCalculatedJustAgo ) return _sfHeightJustAgo;
	
	_sfHeightJustAgo = pSector->D_GetMinHeight();

	EnumCallback( APMMAP_CB_ID_GET_MIN_HEIGHT , pSector , (PVOID) &_sfHeightJustAgo );

	_spSectorCalculatedJustAgo = pSector;

	return _sfHeightJustAgo;
}

#define	COLLISION_GAP	0.05f
// X������ ���� ���� ( y = a �������� ������ �������� ����� )
inline BOOL	CollisionLineToLineX(	float fGiulgi ,
									float x1 , float y1 ,
									float axisY ,
									float fRangeX1 , float fRangeX2 ,
									float &fCollX , float &fCollY ,
									float axisYPos // �ݸ��� ��ġ�� �� �ڸ�..
									)
{
	float	fCollCalcX = ( axisY - y1 ) / fGiulgi + x1;
	if( fRangeX1 < fCollCalcX && fCollCalcX < fRangeX2 )
	{
		// �ݸ��� ��ġ�� ��¦ ���.
		fCollCalcX = ( axisYPos - y1 ) / fGiulgi + x1;
		fCollX = fCollCalcX	;
		fCollY = axisYPos	;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


// X������ ���� ���� ( y = a �������� ������ �������� ����� )
inline BOOL	CollisionLineToLineY(	float fGiulgi ,
									float x1 , float y1 ,
									float axisX ,
									float fRangeY1 , float fRangeY2 ,
									float &fCollX , float &fCollY ,
									float axisXPos // �ݸ��� ��ġ�� �� �ڸ�..
									)
{
	float	fCollCalcY = fGiulgi * ( axisX - x1 ) + y1;
	if( fRangeY1 < fCollCalcY && fCollCalcY < fRangeY2 )
	{
		fCollCalcY = fGiulgi * ( axisXPos - x1 ) + y1;
		fCollX = axisXPos		;
		fCollY = fCollCalcY	;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#define _DISTANCE( x1 , x2 ) ( ( ( x2 ) - ( x1 ) ) * ( ( x2 ) - ( x1 ) ) )

BOOL	ApmMap::GetValidDestination		( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , FLOAT fRadius , BLOCKINGTYPE eType )
{
	ASSERT( pStart	);
	ASSERT( pDest	);
	ASSERT( pValid	);

	if( NULL == pStart )
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s ( %d Line ) : pStart is NULL\n" , __FILE__ , __LINE__);
		AuLogFile_s("LOG\\MapLog1110.txt", strCharBuff);

		return FALSE;
	}
	if( NULL == pDest )
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s ( %d Line ) : pDest is NULL\n" , __FILE__ , __LINE__);
		AuLogFile_s("LOG\\MapLog1110.txt", strCharBuff);
		return FALSE;
	}
	if( NULL == pValid )
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s ( %d Line ) : pValid is NULL\n" , __FILE__ , __LINE__);
		AuLogFile_s("LOG\\MapLog1110.txt", strCharBuff);
		return FALSE;
	}

	if( !IsValidPosition( pStart	) ) { * pValid = * pStart; return FALSE; }
	if( !IsValidPosition( pDest		) ) { * pValid = * pStart; return FALSE; }

	// �������̼� üũ.
	// FALSE �ϰ�� pValid �� �����°�.

	// ���� ��ǥ�� ��ģ�� �ɼ��� ����.
	// ������ ���������� ���� ó���� ������ �����.
	float x1 = ( pStart->x	+ 2560000.0f ) / MAP_STEPSIZE;
	float z1 = ( pStart->z	+ 2560000.0f ) / MAP_STEPSIZE;
	float x2 = ( pDest->x	+ 2560000.0f ) / MAP_STEPSIZE;
	float z2 = ( pDest->z	+ 2560000.0f ) / MAP_STEPSIZE;

	float dx = x2 - x1;
	float dz = z2 - z1;

	float fGiulgi = dz/dx;

	if( CheckBlockingInfo( ( int ) x1 , ( int ) z1 , eType ) )
	{
		* pValid = * pStart;
		return FALSE;
	}

	* pValid = * pDest;

	// ���� ���ϱ�..
	int nXStart = static_cast<int>( x1 );
	int nZStart = static_cast<int>( z1 );
	int nXEnd	= static_cast<int>( x2 );
	int nZEnd	= static_cast<int>( z2 );

	// ������ �ʹ� ũ�� �ϴ� ������ TRUE�� ����
	// �α׸� ���ܺ��� �̷� ��찡 ����� �Ȼ���� �Լ� ȣ�� �κ��� �����Ѵ�. (by netong)
	if (abs(nXStart - nXEnd) + abs(nZStart - nZEnd) > 6)
	{
		//AuLogFile("MapLog.txt", "5281 (%d)", abs(nXStart - nXEnd) + abs(nZStart - nZEnd));	// Ŭ���̾�Ʈ ��ġ�Ǹ� ������� ������,, �� �̻��� ���ڸ� �����Ѵ�.
		return TRUE;
	}

	int nX , nZ;
	const float _cDistanceInitValue = 999999999.9999f;
	float fCollisionDistance = _cDistanceInitValue;
	float fDistance;

	const int	cnLoopMax = 6;
	int nInnerLoop	= 0;
	int nOutterLoop	= 0;

#define	OUTTER_LOOP_CHECK	{ if( nOutterLoop > cnLoopMax ) { char strCharBuff[256] = { 0, }; sprintf_s(strCharBuff, sizeof(strCharBuff), "%s ( %d Line ) : Outter Loop Over , nZ( %d -> %d : %d -> %d )\n" , __FILE__ , __LINE__ , ( int ) pStart->z , ( int ) pDest->z , nZStart , nZEnd); AuLogFile_s("LOG\\MapLog1110.txt", strCharBuff); break; } }
	#define	INNER_LOOP_CHECK	{ if( nInnerLoop > cnLoopMax ) { char strCharBuff[256] = { 0, }; sprintf_s(strCharBuff, sizeof(strCharBuff), "%s ( %d Line ) : Inner Loop Over , nX( %d -> %d : %d -> %d )\n" , __FILE__ , __LINE__ , ( int )pStart->x , ( int ) pDest->x , nXStart , nXEnd); AuLogFile_s("LOG\\MapLog1110.txt", strCharBuff); break; } }

	AuPOS vColl;
	AuPOS vValid;

	if( dx > 0 )
	{
		if( dz > 0 )
		{
			// �Ѵ� �����ϴ� ����..
			for( nZ = nZStart , nOutterLoop = 0 ; nZ <= nZEnd ; nZ++ , nOutterLoop ++ )
			{
				OUTTER_LOOP_CHECK
				for( nX = nXStart , nInnerLoop = 0 ; nX <= nXEnd ; nX++ , nInnerLoop ++ )
				{
					INNER_LOOP_CHECK
					// ��ŷ���� ����..
					if( CheckBlockingInfo( nX , nZ , eType ) )
					{
						// �ݸ��� üũ.
						if( CollisionLineToLineX( fGiulgi , x1 , z1 , ( float ) nZ , 
							( float ) nX , ( float ) nX + 1 , 
							vColl.x , vColl.z , ( float ) nZ - COLLISION_GAP
							)
						)
						{
							if( vColl.x <= x1 ||
								vColl.z <= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						// �ݸ��� üũ.
						if( CollisionLineToLineY( fGiulgi , x1 , z1 , ( float ) nX , 
							( float ) nZ , ( float ) nZ + 1 , 
							vColl.x , vColl.z , ( float ) nX - COLLISION_GAP 
							)
						)
						{
							if( vColl.x <= x1 ||
								vColl.z <= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						
					}
				}
			}
		}
		else
		{
			// X���� Y����
			for( nZ = nZStart , nOutterLoop = 0 ; nZ >= nZEnd ; nZ-- , nOutterLoop ++ )
			{
				OUTTER_LOOP_CHECK
				for( nX = nXStart , nInnerLoop = 0 ; nX <= nXEnd ; nX++ , nInnerLoop  ++)
				{
					INNER_LOOP_CHECK
					// ��ŷ���� ����..
					if( CheckBlockingInfo( nX , nZ , eType ) )
					{
						// �ݸ��� üũ.
						if( CollisionLineToLineX( fGiulgi , x1 , z1 , ( float ) nZ + 1, 
							( float ) nX , ( float ) nX + 1 , 
							vColl.x , vColl.z , ( float ) nZ + 1 + COLLISION_GAP
							)
						)
						{
							if( vColl.x <= x1 ||
								vColl.z >= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						// �ݸ��� üũ.
						if( CollisionLineToLineY( fGiulgi , x1 , z1 , ( float ) nX , 
							( float ) nZ , ( float ) nZ + 1 , 
							vColl.x , vColl.z , ( float ) nX - COLLISION_GAP 
							)
						)
						{
							if( vColl.x <= x1 ||
								vColl.z >= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						
					}
				}
			}
		}
	}
	else
	{
		if( dz > 0 )
		{
			// X ���� Y ����
			for( nZ = nZStart , nOutterLoop = 0 ; nZ <= nZEnd ; nZ++ , nOutterLoop ++ )
			{
				OUTTER_LOOP_CHECK
				for( nX = nXStart , nInnerLoop = 0 ; nX >= nXEnd ; nX-- , nInnerLoop ++ )
				{
					INNER_LOOP_CHECK
					// ��ŷ���� ����..
					if( CheckBlockingInfo( nX , nZ , eType ) )
					{
						// �ݸ��� üũ.
						if( CollisionLineToLineX( fGiulgi , x1 , z1 , ( float ) nZ , 
							( float ) nX , ( float ) nX + 1 , 
							vColl.x , vColl.z , ( float ) nZ - COLLISION_GAP
							)
						)
						{
							if( vColl.x >= x1 ||
								vColl.z <= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						// �ݸ��� üũ.
						if( CollisionLineToLineY( fGiulgi , x1 , z1 , ( float ) nX + 1 , 
							( float ) nZ , ( float ) nZ + 1 , 
							vColl.x , vColl.z , ( float ) nX + 1 + COLLISION_GAP
							)
						)
						{
							if( vColl.x >= x1 ||
								vColl.z <= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						
					}
				}
			}
		}
		else
		{
			// X���� Y���� 
			for( nZ = nZStart , nOutterLoop = 0 ; nZ >= nZEnd ; nZ-- , nOutterLoop ++ )
			{
				OUTTER_LOOP_CHECK
				for( nX = nXStart , nInnerLoop = 0 ; nX >= nXEnd ; nX-- , nInnerLoop ++ )
				{
					INNER_LOOP_CHECK
					// ��ŷ���� ����..
					if( CheckBlockingInfo( nX , nZ , eType ) )
					{
						// �ݸ��� üũ.
						if( CollisionLineToLineX( fGiulgi , x1 , z1 , ( float ) nZ + 1, 
							( float ) nX , ( float ) nX + 1 , 
							vColl.x , vColl.z , ( float ) nZ + 1 + COLLISION_GAP 
							)
						)
						{
							if( vColl.x >= x1 ||
								vColl.z >= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						// �ݸ��� üũ.
						if( CollisionLineToLineY( fGiulgi , x1 , z1 , ( float ) nX + 1, 
							( float ) nZ , ( float ) nZ + 1 , 
							vColl.x , vColl.z , ( float ) nX + 1 + COLLISION_GAP
							)
						)
						{
							if( vColl.x >= x1 ||
								vColl.z >= z1 )
							{
								* pValid = * pStart;
								return FALSE;
							}
							else
							{
								fDistance = _DISTANCE( x1 , vColl.x );
								if( fDistance < fCollisionDistance )
								{
									fCollisionDistance = fDistance;
									vValid = vColl;
								}
							}
						}
						
					}
				}
			}
		}
	}

	if( _cDistanceInitValue != fCollisionDistance )
	{
		// ��ǥ ����س���
		pValid->x = vValid.x * MAP_STEPSIZE - 2560000.0f;
		pValid->z = vValid.z * MAP_STEPSIZE - 2560000.0f;

		// ���� ��ŷ ó��..

		switch( eType )
		{
		case GROUND:
			GetValidDestination_LineBlock( pStart , pValid , pValid , fRadius );
			break;
		default:
			break;
		}
	
		return FALSE;
	}
	else
	{
		// ���� ��ŷ ó��..
		switch( eType )
		{
		case GROUND:
			GetValidDestination_LineBlock( pStart , pValid , pValid , fRadius );
			break;
		default:
			break;
		}
		return TRUE;
	}
}

BOOL	ApmMap::GetValidDestination_LineBlock	( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , FLOAT fRadius  )
{
	INT32	nStartX , nEndX , nStartZ , nEndZ;
	INT32	nSegmentXStart	, nSegmentZStart;
	INT32	nSegmentXEnd	, nSegmentZEnd	;

	{
		INT32	nAX , nAZ , nBX , nBZ;

		nAX = PosToSectorIndexX( pStart->x );
		nAZ = PosToSectorIndexZ( pStart->z );
		nBX = PosToSectorIndexX( pDest->x );
		nBZ = PosToSectorIndexZ( pDest->z );

		if( nAX <= nBX )
		{
			nStartX	= nAX;
			nEndX	= nBX;
		}
		else
		{
			nStartX	= nBX;
			nEndX	= nAX;
		}

		if( nAZ <= nBZ )
		{
			nStartZ	= nAZ;
			nEndZ	= nBZ;
		}
		else
		{
			nStartZ	= nBZ;
			nEndZ	= nAZ;
		}

		// Ÿ�Ͽɼ�..���س�..
		FLOAT	fStart , fEnd  , fTop , fBottom;
		if( pDest->x < pStart->x )
		{
			fStart	= pDest->x;
			fEnd	= pStart->x;
		}
		else
		{
			fEnd	= pDest->x;
			fStart	= pStart->x;
		}
		
		if( pDest->z < pStart->z )
		{
			fTop	= pDest->z;
			fBottom	= pStart->z;
		}
		else
		{
			fBottom	= pDest->z;
			fTop	= pStart->z;
		}

		INT32	nXStart = ( INT32 ) GetSectorStartX( nStartX );
		INT32	nZStart = ( INT32 ) GetSectorStartX( nStartZ );
		INT32	nStepSize = ( INT32 ) MAP_STEPSIZE;

		nSegmentXStart = ( ( INT32 ) fStart - nXStart ) / nStepSize;
		nSegmentZStart = ( ( INT32 ) fTop - nZStart ) / nStepSize;

		nXStart = ( INT32 ) GetSectorStartX( nEndX );
		nZStart = ( INT32 ) GetSectorStartX( nEndZ );

		nSegmentXEnd = ( ( INT32 ) fEnd - nXStart ) / nStepSize;
		nSegmentZEnd = ( ( INT32 ) fBottom - nZStart ) / nStepSize;
	}

	INT32	nSectorX , nSectorZ;

	ApWorldSector * pSector;

	#define SECTOR_TOTAL_COL_COUNT	( SECTOR_MAX_COLLISION_OBJECT_COUNT * 10 )
	vector< ApWorldSector::AuLineBlock > vectorLineBlock;

	for( nSectorZ = nStartZ ; nSectorZ <= nEndZ /* ���ԵǾ����*/ ; nSectorZ ++ )
	{
		for( nSectorX = nStartX ; nSectorX <= nEndX /* ���ԵǾ����*/ ; nSectorX ++ )
		{
			pSector	= GetSector( nSectorX , nSectorZ );

			if( pSector )
			{
				//  ���Ϳ� �ִ� ����� �� �˻���.
				for( vector< ApWorldSector::AuLineBlock >::iterator
						iter = pSector->m_vecBlockLine.begin();
						iter != pSector->m_vecBlockLine.end();
						iter++ )
				{
					vectorLineBlock.push_back( *iter );

				}
			}
			///////////////
		}
	}

	if( vectorLineBlock.size() )
	{
		AuPOS vDirection;

		vDirection.y = 0.0f;
		vDirection.x = pDest->x - pStart->x;
		vDirection.z = pDest->z - pStart->z;

		float fMaxDistance = vDirection.Length();
		float fNearestDistance = fMaxDistance;

		BOOL bCollision = FALSE;

		for( vector< ApWorldSector::AuLineBlock >::iterator iTer = vectorLineBlock.begin() ;
			iTer != vectorLineBlock.end() ;
			iTer ++ )
		{
			ApWorldSector::AuLineBlock vLine = *iTer;

			vDirection.x = pValid->x - pStart->x;
			vDirection.z = pValid->z - pStart->z;
			vDirection.y = 0.0f;

			#ifdef _DEBUG
			static bool _sbUseLineColl = true;
			if( _sbUseLineColl )
			#endif
			bCollision |= vLine.CollLine( fRadius , pStart , &vDirection , pValid );

			#ifdef _DEBUG
			static bool _sbUseCylinderColl = true;
			if( _sbUseCylinderColl )
			#endif
			bCollision |= vLine.CollCylinder( fRadius , pStart , pValid );
		}

		return !bCollision;
	}
	else
	{
		return TRUE;
	}
}

BOOL	ApmMap::LoadRegionPerculiarity( BOOL bEncrypt /* = FALSE */ )
{

	if( bEncrypt )
	{
		AuMD5Encrypt	MD5;
		ifstream		InFileStream;
		ofstream		OutFileStream;
		size_t			nSize			=	0;
		CHAR*			pFileBuffer	=	NULL;

		// 1 - ������ �о �޸𸮷� �ҷ��´�
		InFileStream.open( "ini\\RegionPerculiarity.xml" , ios_base::in | ios_base::binary );
		if( !InFileStream )
			return FALSE;

		InFileStream.seekg( 0 , ios_base::end );
		nSize		=	(size_t)InFileStream.tellg();
		pFileBuffer	=	new CHAR [ nSize ];
		ZeroMemory( pFileBuffer , nSize );

		InFileStream.seekg( 0 , ios_base::beg );
		InFileStream.read( pFileBuffer , nSize );
		InFileStream.close();
		// 1 - END


		// 2 - �޸��� ������ ��ȣȭ �Ѵ�
		if( !MD5.DecryptString( MD5_HASH_KEY_STRING, pFileBuffer, nSize ) )
		{
			DEF_SAFEDELETE( pFileBuffer );
			return FALSE;
		}
		// 2 - END

		
		// 3 - ��ȣȭ �� ������ ���Ϸ� ����
		OutFileStream.open( "ini\\RegionPerculiarityTemp.xml" , ios_base::out | ios_base::binary );
		if( !OutFileStream )
		{
			DEF_SAFEDELETE( pFileBuffer );
			return FALSE;
		}

		OutFileStream.write( pFileBuffer , nSize );
		OutFileStream.close();
		DEF_SAFEDELETE( pFileBuffer );
		// 3 - END

		// Read
		if(!m_XmlData.LoadFile("ini\\RegionPerculiarityTemp.xml"))
		{
			::DeleteFile( "ini\\RegionPerculiarityTemp.xml" );
			return FALSE;
		}

		// �ӽ÷� ���� ������ �����
		::DeleteFile( "ini\\RegionPerculiarityTemp.xml" );
	}

	else
	{
		if( !m_XmlData.LoadFile( "ini\\RegionPerculiarity.xml" ) )
			return FALSE;
	}

	AuXmlNode *pRootNode = m_XmlData.FirstChild("RegionRule");
	if(!pRootNode)
		return FALSE;

	m_pRootRuleNode = pRootNode->FirstChild("Rule");
	if(!m_pRootRuleNode)
		return FALSE;

	m_RegionPerculiarityMap.clear();
	
	for(AuXmlNode *pNode = m_pRootRuleNode; pNode; pNode = pNode->NextSibling())
	{
		RegionPerculiarity	stRegionPerculiarity;

		AuXmlElement *pElemIndex = pNode->FirstChildElement("Index");
		if(!pElemIndex)
			continue;

		const CHAR *pUse = pElemIndex->Attribute("Use");
		if(!strcmp(pUse, "Yes"))
			stRegionPerculiarity.m_bUsePerculiarity = TRUE;
		else
			continue;

		AuXmlElement *pElemParty		= pNode->FirstChildElement("Party");
		AuXmlElement *pElemPartyBuff	= pNode->FirstChildElement("PartyBuff");
		AuXmlElement *pElemItemDrop		= pNode->FirstChildElement("ItemDrop");
		AuXmlElement *pElemSkulDrop		= pNode->FirstChildElement("SkulDrop");
		AuXmlElement *pElemRemoteBuff	= pNode->FirstChildElement("RemoteBuff");
		AuXmlElement *pElemNoSkill		= pNode->FirstChildElement("NoSkill");
		AuXmlElement *pElemNoChat		= pNode->FirstChildElement("Chat");
		AuXmlElement *pElemShowName		= pNode->FirstChildElement("ShowName");
		AuXmlElement *pElemPreserveBuff = pNode->FirstChildElement("PreserveBuff");
		AuXmlElement *pElemDropExp		= pNode->FirstChildElement("DropExp");
		AuXmlElement *pElemCriminalRule = pNode->FirstChildElement("CriminalRule");
		AuXmlElement *pElemNoItem		= pNode->FirstChildElement("NoItem");
		AuXmlElement *pElemGuildMessage	= pNode->FirstChildElement("GuildMessage");
		AuXmlElement *pElemGuildBuff	= pNode->FirstChildElement("GuildBuff");
		AuXmlElement *pElemGuildPVP		= pNode->FirstChildElement("GuildPVP");
		AuXmlElement *pDropSkulTid		= pNode->FirstChildElement("DropSkulTid");
		AuXmlElement *pSkulRestriction	= pNode->FirstChildElement("DropSkulRestriction");
		AuXmlElement *pIsAllowLoginPlace= pNode->FirstChildElement("IsAllowLoginPlace");
		AuXmlElement *pElemMobCharismaDrop	= pNode->FirstChildElement("MobCharismaDrop");
		AuXmlElement *pElemPVPCharismaDrop	= pNode->FirstChildElement("PVPCharismaDrop");

		if(pElemIndex)
		{
			 INT32 lRegionIndex = atoi(pElemIndex->GetText());
			 stRegionPerculiarity.m_lRegionIndex = lRegionIndex;

			 if(pElemParty)
			 {
				 if(!strcmp(pElemParty->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bParty = FALSE;
				 }
			 }

			 if(pElemPartyBuff)
			 {
				 if(!strcmp(pElemPartyBuff->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bPartyBuff = FALSE;
				 }
			 }

			 if(pElemItemDrop)
			 {
				 if(!strcmp(pElemItemDrop->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bItemDrop = FALSE;
				 }
			 }

			 if(pElemRemoteBuff)
			 {
				 if(!strcmp(pElemRemoteBuff->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bRemoteBuff = FALSE;
				 }
			 }
			
			 if(pElemNoChat)
			 {
				 if(!strcmp(pElemNoChat->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bEnableChat = FALSE;
				 }
			 }

			 if(pElemPreserveBuff)
			 {
				 if(!strcmp(pElemPreserveBuff->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bPreserveBuff = FALSE;
				 }
			 }

			 if(pElemDropExp)
			 {
				 if(!strcmp(pElemDropExp->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bDropExp = FALSE;
				 }
			 }

			 if(pElemCriminalRule)
			 {
				 if(!strcmp(pElemCriminalRule->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bCriminalRule = FALSE;
				 }
			 }

			 if(pElemGuildMessage)
			 {
				 if(!strcmp(pElemGuildMessage->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bGuildMessage = FALSE;
				 }
			 }

			 if(pElemGuildBuff)
			 {
				 if(!strcmp(pElemGuildBuff->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bGuildBuff = FALSE;
				 }
			 }

			 if(pElemGuildPVP)
			 {
				 if(!strcmp(pElemGuildPVP->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bGuildPVP = FALSE;
				 }
			 }

			 if(pElemSkulDrop)
			 {
				 if(!strcmp(pElemSkulDrop->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bSkulDrop = FALSE;
				 }
			 }

			 if(pSkulRestriction)
			 {
				 if(!strcmp(pSkulRestriction->GetText(),"No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bSkullRestriction = FALSE;
				 }
				 else
				 {
					 const CHAR *pUse = pSkulRestriction->Attribute("RestrictionTime");
					 INT32 ulRestrictionTime = atoi(pUse);
					 stRegionPerculiarity.m_stPerculiarity.m_ulRestrictionTime = ulRestrictionTime;
				 }
			 }

			 if(pIsAllowLoginPlace)
			 {
				 if(!strcmp(pIsAllowLoginPlace->GetText(), "No"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bAllowLoginPlace = FALSE;
				 }
			 }

			 if(pDropSkulTid)
			 {
				 INT32 lSkulTid = atoi(pDropSkulTid->GetText());
				 stRegionPerculiarity.m_stPerculiarity.m_lSkulTid = lSkulTid;
			 }

			 if(pElemShowName)
			 {
				 if(!strcmp(pElemShowName->GetText(), "AllNo"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bShowName = APMMAP_PECULIARITY_SHOW_NAME_ALL_NO;
				 }
				 else if(!strcmp(pElemShowName->GetText(), "Union"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bShowName = APMMAP_PECULIARITY_SHOW_NAME_UNION;
				 }
			 }

			 if(pElemMobCharismaDrop)
			 {
				 if(!strcmp(pElemMobCharismaDrop->GetText(), "Yes"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bMobCharismaDrop = TRUE;
				 }
			 }

			 if(pElemPVPCharismaDrop)
			 {
				 if(!strcmp(pElemPVPCharismaDrop->GetText(), "Yes"))
				 {
					 stRegionPerculiarity.m_stPerculiarity.m_bPVPCharismaDrop = TRUE;
				 }
			 }

			 if(pElemNoSkill)
			 {
				 CHAR *pToken = strtok((char*)pElemNoSkill->GetText(), ";");
				 while(pToken != NULL)
				 {
					 INT32 lDisableSkill = atoi(pToken);
					 stRegionPerculiarity.m_vDisableSkillList.push_back(lDisableSkill);

					 pToken = strtok(NULL, ";");
				 }
			 }

			 if(pElemNoItem)
			 {
				 CHAR *pTokenItem = strtok((char*)pElemNoItem->GetText(), ";");
				 while(pTokenItem != NULL)
				 {
					 INT32 lDisableItem = atoi(pTokenItem);
					 stRegionPerculiarity.m_vDisableUseItemList.push_back(lDisableItem);

					 pTokenItem = strtok(NULL, ";");
				 }
			 }

			 m_RegionPerculiarityMap.insert(RegionPerculiarityPair(lRegionIndex, stRegionPerculiarity));
		}	
	}

	return TRUE;
}

// If Region have special condition Check RegionPerculiarity
AgpmMapRegionPeculiarityReturn ApmMap::CheckRegionPerculiarity(INT32 lRegionIndex, AgpmMapRegionPeculiarity ePeculiarity, INT32 lResevedOption /* = 0 */)
{
	if(lRegionIndex < 0)
		return APMMAP_PECULIARITY_RETURN_INVALID_REGIONINDEX;

	RegionPerculiarityIter iter = m_RegionPerculiarityMap.find(lRegionIndex);
	if(iter == m_RegionPerculiarityMap.end())
		return APMMAP_PECULIARITY_RETURN_NO_PECULIARITY;

	if(iter->second.m_bUsePerculiarity == FALSE)
		return APMMAP_PECULIARITY_RETURN_NO_PECULIARITY;

	AgpmMapRegionPeculiarityReturn eReturn = APMMAP_PECULIARITY_RETURN_ENABLE_USE;

	switch (ePeculiarity)
	{
	case APMMAP_PECULIARITY_PARTY:
		{
			if(iter->second.m_stPerculiarity.m_bParty == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_PARTY_BUFF:
		{
			if(iter->second.m_stPerculiarity.m_bPartyBuff == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_CHATTING:
		{
			if(iter->second.m_stPerculiarity.m_bEnableChat == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_ITEM_DROP:
		{
			if(iter->second.m_stPerculiarity.m_bItemDrop == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_SKUL_DROP:
		{
			if(iter->second.m_stPerculiarity.m_bSkulDrop == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_REMOTE_BUFF:
		{
			if(iter->second.m_stPerculiarity.m_bRemoteBuff == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_DROP_EXP:
		{
			if(iter->second.m_stPerculiarity.m_bDropExp == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_CRIMINAL_RULE:
		{
			if(iter->second.m_stPerculiarity.m_bCriminalRule == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_PRESERVE_BUFF:
		{
			if(iter->second.m_stPerculiarity.m_bPreserveBuff == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_GUILD_MESSAGE:
		{
			if(iter->second.m_stPerculiarity.m_bGuildMessage == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_GUILD_BUFF:
		{
			if(iter->second.m_stPerculiarity.m_bGuildBuff == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_GUILD_PVP:
		{
			if(iter->second.m_stPerculiarity.m_bGuildPVP == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_SHOWNAME:
		{
			if(iter->second.m_stPerculiarity.m_bShowName == APMMAP_PECULIARITY_SHOW_NAME_UNION)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_ENABLE_USE_UNION;
			}
			else if(iter->second.m_stPerculiarity.m_bShowName == APMMAP_PECULIARITY_SHOW_NAME_ALL_NO)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY_SKILL:
		{
			if(iter->second.m_vDisableSkillList.empty() == FALSE)
			{
				for(vector<INT32>::iterator it = iter->second.m_vDisableSkillList.begin();
					it != iter->second.m_vDisableSkillList.end(); ++it)
				{
					// if m_vDisableSkillList have -1 means disable All Skill
					if(*it == APMMAP_REGIONPECULIARITY_DISABLE_ALL_SKILL)
					{
						eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
						break;
					}

					// if disable skill list have lResevedOption(Skill TID) can't use skill
					if(*it == lResevedOption)
					{
						eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
						break;
					}
				}
			}
		}break;

	case APMMAP_PECULIARITY_USE_ITEM:
		{
			if(iter->second.m_vDisableUseItemList.empty() == FALSE)
			{
				for(vector<INT32>::iterator it = iter->second.m_vDisableUseItemList.begin();
					it != iter->second.m_vDisableUseItemList.end(); ++it)
				{
					if(*it == APMMAP_REGIONPECULIARITY_DISABLE_ALL_ITEM)
					{
						eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
						break;
					}

					if(*it == lResevedOption)
					{
						eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
						break;
					}
				}
			}
		}break;

	case APMMAP_PECULIARITY_DROP_SKUL_RESTRICTION:
		{
			if(iter->second.m_stPerculiarity.m_bSkullRestriction == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;
		
	case APMMAP_PECULIARITY_IS_ALLOW_LOGIN_PLACE:
		{
			if(iter->second.m_stPerculiarity.m_bAllowLoginPlace == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;

	case APMMAP_PECULIARITY__MOB_CHARISMA_DROP:
		{
			if(iter->second.m_stPerculiarity.m_bMobCharismaDrop == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;
	case APMMAP_PECULIARITY__PVP_CHARISMA_DROP:
		{
			if(iter->second.m_stPerculiarity.m_bPVPCharismaDrop == FALSE)
			{
				eReturn = APMMAP_PECULIARITY_RETURN_DISABLE_USE;
			}
		}break;
	}

	return eReturn;
}

INT32 ApmMap::LoadRegioinPerculiaritySkulTid(INT32 lRegionIndex)
{
	if(lRegionIndex < 0)
		return 0;

	RegionPerculiarityIter iter = m_RegionPerculiarityMap.find(lRegionIndex);
	if(iter == m_RegionPerculiarityMap.end())
		return 0;

	if(iter->second.m_bUsePerculiarity == FALSE)
		return 0;

	return iter->second.m_stPerculiarity.m_lSkulTid;
}

INT32 ApmMap::LoadRegioinPerculiaritySkulRestrictionTime(INT32 lRegionIndex)
{
	if(lRegionIndex < 0)
		return 0;

	RegionPerculiarityIter iter = m_RegionPerculiarityMap.find(lRegionIndex);
	if(iter == m_RegionPerculiarityMap.end())
		return 0;

	if(iter->second.m_bUsePerculiarity == FALSE)
		return 0;

	return iter->second.m_stPerculiarity.m_ulRestrictionTime;
}