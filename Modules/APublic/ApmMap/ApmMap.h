////////////////////////////////////////////////////////////////////
// Created  : ������ (2002-04-23 ���� 6:22:14)
// Note     : 
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMMAP_H__D09E90BC_BF4C_482A_AE9C_E19838224685__INCLUDED_)
#define AFX_APMMAP_H__D09E90BC_BF4C_482A_AE9C_E19838224685__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ApModule/ApModule.h>
#include <ApAdmin/ApAdmin.h>
#include <ApmMap/ApMapBlocking.h>
#include <ApmMap/ApWorldSector.h>
#include <ApmMap/MagUnpackManager.h>
#include <ApMemory/ApMemoryPool.h>
#include <AuXmlParser/AuXmlParser.h>
#include <AuLua/AuLua.h>

#include <vector>
#include <map>

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApmMapD" )
#else
#pragma comment ( lib , "ApmMap" )
#endif
#endif
//@} Jaewon

#define	MAP_WORLD_INDEX_WIDTH	800			// ��ü�� ũ���� ����.	x�� 
#define	MAP_WORLD_INDEX_HEIGHT	800			// ��ü�� ũ���� ����.	z��
#define	MAP_SECTOR_WIDTH		(6400.0f)	// �Ѽ����� �ʺ�.
#define	MAP_SECTOR_HEIGHT		(6400.0f)	// �Ѽ����� ����.
#define MAP_DEFAULT_DEPTH		16
#define MAP_STEPSIZE			(400.0f)
#define	MAP_UNPACK_MANAGER_BUF	9

#define INVALID_HEIGHT					(-20000.0f)

#define	ALEF_MAX_LOADING_SECTOR_BUFFER	65535

// �ִ���� ��������.. ������ ��.
#define	SECTOR_MAX_HEIGHT				100000.0f
#define	SECTOR_MIN_HEIGHT				INVALID_HEIGHT

#define MAKEDWORD(lo,hi)	( (DWORD) ( ( (hi) << 32 ) | (lo) ) )

// Utility Inline Functions..
inline	INT32 ArrayIndexToSectorIndexX	( INT32 arrayindex_X		) { return  arrayindex_X	- ( INT32 ) ( MAP_WORLD_INDEX_WIDTH		>> 1 ); }
inline	INT32 ArrayIndexToSectorIndexZ	( INT32 arrayindex_Z		) { return  arrayindex_Z	- ( INT32 ) ( MAP_WORLD_INDEX_HEIGHT	>> 1 ); }
inline	INT32 SectorIndexToArrayIndexX	( INT32 sectorindex_X		) { return  sectorindex_X	+ ( INT32 ) ( MAP_WORLD_INDEX_WIDTH		>> 1 ); }
inline	INT32 SectorIndexToArrayIndexZ	( INT32 sectorindex_Z		) { return  sectorindex_Z	+ ( INT32 ) ( MAP_WORLD_INDEX_HEIGHT	>> 1 ); }

#define REGIONTEMPLATEFILE	"regiontemplate.ini"
#define REGIONFILE			"region.ini"
#define WORLDMAPTEMPLATE	"worldmap.ini"

#define	DIMENSION_NAME_LENGTH		32
#define	WORLDMAP_FILENAME_LENGTH	32
#define	WORLDMAP_COMMENT_LENGTH		256

#define APMMAP_REGIONPECULIARITY_DISABLE_ALL_SKILL		0
#define APMMAP_REGIONPECULIARITY_DISABLE_ALL_ITEM		0

enum AgpmMapRestrictPlace
{
	AGPMMAP_RESTRICT_CASTLE_ARCHLORD = 81,
	AGPMMAP_RESTRICT_CONSULT_OFFICE	 = 177,
	AGPMMAP_RESTRICT_EVENT_BATTLE_GROUND = 178,
	AGPMMAP_RESTRICT_EVENT_BATTLE_GROUND_TOWN = 179,
};

typedef enum ApmMapCallbackPoint			// Callback ID in Map Module 
{
	APMMAP_CB_ID_ADDCHAR			= 0,
	APMMAP_CB_ID_REMOVECHAR				,
	APMMAP_CB_ID_MOVECHAR				,
	APMMAP_CB_ID_ADDITEM				,
	APMMAP_CB_ID_REMOVEITEM				,
	APMMAP_CB_ID_LOAD_SECOTR			,
	APMMAP_CB_ID_CLEAR_SECTOR			,
	APMMAP_CB_ID_SAVEGEOMETRY			,
	APMMAP_CB_ID_LOADGEOMETRY			,
	APMMAP_CB_ID_SAVEMOONEE				,
	APMMAP_CB_ID_LOADMOONEE				,
	APMMAP_CB_ID_GET_MIN_HEIGHT			
} ApmMapCallbackPoint;

enum AgpmMapRegionPeculiarity
{
	APMMAP_PECULIARITY_NONE = 0,
	APMMAP_PECULIARITY_PARTY,
	APMMAP_PECULIARITY_PARTY_BUFF,
	APMMAP_PECULIARITY_CHATTING,
	APMMAP_PECULIARITY_ITEM_DROP,
	APMMAP_PECULIARITY_SKUL_DROP,
	APMMAP_PECULIARITY_REMOTE_BUFF,
	APMMAP_PECULIARITY_SKILL,
	APMMAP_PECULIARITY_SHOWNAME,
	APMMAP_PECULIARITY_PRESERVE_BUFF,
	APMMAP_PECULIARITY_DROP_EXP,
	APMMAP_PECULIARITY_CRIMINAL_RULE,
	APMMAP_PECULIARITY_USE_ITEM,
	APMMAP_PECULIARITY_GUILD_MESSAGE,
	APMMAP_PECULIARITY_GUILD_BUFF,
	APMMAP_PECULIARITY_GUILD_PVP,
	APMMAP_PECULIARITY_DROP_SKUL_TID,
	APMMAP_PECULIARITY_DROP_SKUL_RESTRICTION,
	APMMAP_PECULIARITY_DROP_SKUL_RESTRICTION_TIME,
	APMMAP_PECULIARITY_IS_ALLOW_LOGIN_PLACE,
	APMMAP_PECULIARITY__MOB_CHARISMA_DROP,
	APMMAP_PECULIARITY__PVP_CHARISMA_DROP,
	APMMAP_PECULIARITY_MAX
};

enum AgpmMapRegionPeculiarityReturn
{
	APMMAP_PECULIARITY_RETURN_INVALID_REGIONINDEX = 0,
	APMMAP_PECULIARITY_RETURN_NO_PECULIARITY,
	APMMAP_PECULIARITY_RETURN_ENABLE_USE,
	APMMAP_PECULIARITY_RETURN_DISABLE_USE,
	APMMAP_PECULIARITY_RETURN_ENABLE_USE_UNION
};

enum AgpmMapRegionPeculiarityShowName
{
	APMMAP_PECULIARITY_SHOW_NAME_ALL	= 0x00000001,
	APMMAP_PECULIARITY_SHOW_NAME_UNION	= 0x00000002,
	APMMAP_PECULIARITY_SHOW_NAME_ALL_NO = 0x00000004,
};


// move callback �� ��� pData�� �� ����ü �����͸� �Ѱ��ش�.
typedef struct _stApmMapMoveSector {
	ApWorldSector	*pOldSector;			// �̵��ϱ� ���� �ִ� ����
	ApWorldSector	*pNewSector;			// �̵��� ����
	AuPOS			*pstCurrentPos;			// �̵��� ���� ������
} stApmMapMoveSector, *pstApmMapMoveSector;

struct	BindElement
{
	INT32		nIndex	;	// �ش� �ε���.
	
	AuCYLINDER	stData	;
	// stData.height �� 0�ϰ�� �ڽ��� ó����..
	// ��.. stData.center �� lefttop ,
	// radius �� width , height �� height ��..
};

typedef enum _ApmMapCharType {
	APMMAP_CHAR_TYPE_PC				= 0x01,
	APMMAP_CHAR_TYPE_NPC			= 0x02,
	APMMAP_CHAR_TYPE_MONSTER		= 0x04,
} ApmMapCharType;

typedef	AuList< BindElement >	BindList		;
typedef AuList< BindList >		BindTypeList	;

// Bob�� �߰�(071203)
typedef enum _eApmMapMaterial
{
	APMMAP_MATERIAL_SOIL		,
	APMMAP_MATERIAL_SWAMP		,
	APMMAP_MATERIAL_GRASS		,
	APMMAP_MATERIAL_SAND		,
	APMMAP_MATERIAL_LEAF		,
	APMMAP_MATERIAL_SNOW		,
	APMMAP_MATERIAL_WATER		,
	APMMAP_MATERIAL_STONE		,
	APMMAP_MATERIAL_WOOD		,
	APMMAP_MATERIAL_METAL		,
	APMMAP_MATERIAL_BONE		,
	APMMAP_MATERIAL_MUD			,	// ����
	APMMAP_MATERIAL_SOILGRASS	,	// Ǯ + ��
	APMMAP_MATERIAL_SOLIDSOIL	,	// �� + ��
	APMMAP_MATERIAL_SPORE		,	// ����
	APMMAP_MATERIAL_MOSS		,	// �̳�
	APMMAP_MATERIAL_GRANITE		,	// ȭ����.
	APMMAP_MATERIAL_NUM
} eApmMapMaterial;

// ������ (2004-06-30 ���� 4:03:37) : 
// ��ǻ� ������Ʈ ������ �����ϱ� ���� ����Ÿ ��Ʈ����..
struct stLoadedDivision
{
public:
	enum
	{
		DATA_NODATA		= 0x00,
		DATA_OBJECT		= 0x01,
		DATA_MAX
	};

protected:
	INT32	nDivisionIndex	;
	UINT32	uRefCount		;
	UINT32	uSetUnSavedFlag	;	// ������ �� ���� �ִ���..
	UINT32	uLoadDataFlag	;	// ����Ÿ�� �ε� �Ǿ��ִ���..

public:
	stLoadedDivision():nDivisionIndex( 0 ) , uRefCount( 0 ) , uSetUnSavedFlag( DATA_NODATA ) , uLoadDataFlag( DATA_NODATA ) {}

	INT32	GetDivisionIndex() { return nDivisionIndex;	}
	INT32	SetDivisionIndex( INT32 nDivision ) { return nDivisionIndex = nDivision;	}

	// ������ (2004-06-30 ���� 4:08:26) : ���۷��� ī��Ʈ ����..
	UINT32	GetRefCount	()	{ return uRefCount; }
	void	AddRef		()	{ uRefCount++;	}
	BOOL	DecreaseRef	()	{ uRefCount--; return TRUE;	}

	UINT32	GetUnSavedFlag()				{ return uSetUnSavedFlag; }
	UINT32	SetUnSavedFlag( UINT32 uFlag )	{ return uSetUnSavedFlag |= uFlag; }

	UINT32	GetLoadDataFlag()				{ return uLoadDataFlag; }
	UINT32	SetLoadDataFlag( UINT32 uFlag )	{ return uLoadDataFlag |= uFlag; }
};

struct DivisionInfo;

//��� ��� �и�.
//
//ApmMap - �� �ۺ� ���
//- ����Ÿ �̵�.
//- ��ŷ ó��
//- ����Ʈ ����
//
//AgcmMap - �� Ŭ���̾�Ʈ ���
//- �� �ε�
//
//AgsmMap - �� ���� ���
//- �� �ε�

//@{ kday 20050823
// ;)
typedef void (* FPTR_SkillDbg_Ready)(const ApWorldSector*, const AuMATRIX*, const AuBOX*, const AuPOS* );
typedef void (* FPTR_SkillDbg_PushBox)(const AuPOS&, const AuPOS& );
typedef void (* FPTR_SkillDbg_PushPos)(const AuPOS&);
//@} kday

// Special Rule On Region
struct RegionPerculiarity
{	
	struct  Perculiarity
	{
		BYTE	m_bParty			: 1;
		BYTE	m_bPartyBuff		: 1;
		BYTE	m_bItemDrop			: 1;
		BYTE	m_bSkulDrop			: 1;
		BYTE	m_bRemoteBuff		: 1;
		BYTE	m_bEnableChat		: 1;

		BYTE	m_bShowName			: 3;
		BYTE	m_bPreserveBuff		: 1;

		BYTE	m_bDropExp			: 1;
		BYTE	m_bCriminalRule		: 1;
		BYTE	m_bGuildMessage		: 1;
		BYTE	m_bGuildBuff		: 1;
		BYTE	m_bGuildPVP			: 1;
		BYTE	m_bSkullRestriction	: 1;
		BYTE	m_bAllowLoginPlace	: 1;
		BYTE	m_bMobCharismaDrop	: 1;
		BYTE	m_bPVPCharismaDrop	: 1;

		INT32	m_lSkulTid;
		INT32	m_ulRestrictionTime;

		Perculiarity()
		{
			m_bParty		= TRUE;
			m_bPartyBuff	= TRUE;
			m_bItemDrop		= TRUE;
			m_bSkulDrop		= TRUE;
			m_bRemoteBuff	= TRUE;
			m_bEnableChat	= TRUE;
			m_bPreserveBuff = TRUE;
			m_bDropExp		= TRUE;
			m_bCriminalRule	= TRUE;
			m_bGuildMessage	= TRUE;
			m_bGuildBuff	= TRUE;
			m_bGuildPVP		= TRUE;
			m_bAllowLoginPlace	= TRUE;
			m_bMobCharismaDrop	= FALSE;
			m_bPVPCharismaDrop	= FALSE;
			m_bShowName		= APMMAP_PECULIARITY_SHOW_NAME_ALL;
			m_lSkulTid		= 0;

			m_bSkullRestriction	= TRUE;
			m_ulRestrictionTime	= 0;
		}
	};

	INT32			m_lRegionIndex;
	BOOL			m_bUsePerculiarity;
	Perculiarity	m_stPerculiarity;
	vector<INT32>	m_vDisableSkillList;
	vector<INT32>	m_vDisableUseItemList;

	RegionPerculiarity()
	{
		m_lRegionIndex = 0;
		m_bUsePerculiarity = FALSE;
		m_vDisableSkillList.clear();
		m_vDisableUseItemList.clear();
	}
};

typedef map<INT32, RegionPerculiarity>	RegionPerculiarityMap;
typedef pair<INT32, RegionPerculiarity> RegionPerculiarityPair;
typedef RegionPerculiarityMap::iterator	RegionPerculiarityIter;	
class ApmMap : public ApModule  
{
//@{ kday 20050823
// ;)
private:
	FPTR_SkillDbg_Ready		m_fptrSkillDbg_Ready;
	FPTR_SkillDbg_PushBox	m_fptrSkillDbg_PushBox;
	FPTR_SkillDbg_PushPos	m_fptrSkillDbg_PushPos;
public:
	VOID	SetSkillDbgCB( FPTR_SkillDbg_Ready		fptrSkillDbg_Ready
		, FPTR_SkillDbg_PushBox	fptrSkillDbg_PushBox
		, FPTR_SkillDbg_PushPos	fptrSkillDbg_PushPos);
//@} kday

protected:
	BOOL	m_bInitialized				;
	char	*m_strMapGeometryDirectory	;	// Detail Data , Geometry�� �ִ� ��ġ..
	char	*m_strMapMooneeDirectory	;	// Detail Data , Moonee�� �ִ� ��ġ..
	char	*m_strMapCompactDirectory	;	// Compact Data�� Ǯ���� ��ġ..
	char	*m_strWorldDirectory		;	// ����Ʈ ����Ÿ�� ����ž��ִ���ġ..

	// Attributes
	// ApWorldSector ����Ÿ�� �������� �����Ҽ� �ִ� ����Ÿ ����.
	ApWorldSector * m_ppSector[ MAP_WORLD_INDEX_WIDTH ][ MAP_WORLD_INDEX_HEIGHT ];
		// [x][y]
		// ���� ����Ÿ�� �����ϱ� ���� �������� ������ �迭. 
		// �迭���� �ε����� ���� �ε����ΰ��� �ƴϴ�.

	BOOL	m_bLoadCompactData				;
	BOOL	m_bLoadDetailData				;
	BOOL	m_bAutoLoadData					;	// ���� �ڵ� �ε�.. ������ ���� OFF

	ApAdmin	m_csAdminSectorRemove			;
	INT32	m_lSectorRemoveDelay			;
	INT32	m_lSectorRemovePool				;
	UINT32	m_ulPrevRemoveClockCount		;

	// ���ͷε�/��Ʈ���� �ݹ�..
	void	( *	m_pfSectorLoadCallback		)( ApWorldSector * pSector );
	void	( *	m_pfSectorDestroyCallback	)( ApWorldSector * pSector );


protected:
	// �ε��Ǿ��ִ� ���͸� ��Ƶδ� ����.
	ApWorldSector *	m_pCurrentLoadedSectors		[ ALEF_MAX_LOADING_SECTOR_BUFFER ];
	UINT32			m_nCurrentLoadedSectorCount	;
	ApMutualEx		m_csMutexSectorList;

public:
	ApMemoryPool	m_csMemoryPool;
	ApWorldSector **GetCurrentLoadedSectors		() { return m_pCurrentLoadedSectors		; }
	UINT32			GetCurrentLoadedSectorCount	() { return m_nCurrentLoadedSectorCount	; }

	void			CLS_AddSector		( ApWorldSector * pSector );
	void			CLS_RemoveSector	( ApWorldSector * pSector );
	
public:
	// ��� ����Ÿ �ε���
	enum APMMAP_DATA_INDEX
	{
		SECTOR_DATA,
		SEGMENT_DATA
	};

	ApmMap();
	virtual ~ApmMap();

	inline BOOL		IsAutoLoadData			(					) { return m_bAutoLoadData			;	}
	inline void		SetAutoLoadData			( BOOL bAuto		) { m_bAutoLoadData			= bAuto	;	}

	inline	BOOL	IsLoadingCompactData	() { return m_bLoadCompactData	;	}
	inline	BOOL	IsLoadingDetailData		() { return m_bLoadDetailData	;	}
	inline	void	SetLoadingMode			( BOOL bLoadCompactData , BOOL bLoadDetailData )
	{ m_bLoadCompactData = bLoadCompactData ; m_bLoadDetailData = bLoadDetailData ;	}
	inline	void	SetSectorCallback		( void	( *	pLoadCallback )( ApWorldSector * pSector ) , void ( * pDestroyCallback )( ApWorldSector * pSector ) )
	{ m_pfSectorLoadCallback = pLoadCallback; m_pfSectorDestroyCallback = pDestroyCallback; }

	// ���� ����Ÿ�� �ε��ϰ������ , ����Ÿ �б����� ���� ���־�� �Ѵ�.
	// ���߿� �ϳ��� �аԵŸ� , �������� ������� ����Ʈ�� ���õȴ�.

	// Operations
	// �ش� �÷��̾ �ִ� ���� ���ͷ� ���� �Ѱ��ִ� ��ƾ��.

	char *	GetMapGeometryDirectory			() { return m_strMapGeometryDirectory	; }
	char *	GetMapMooneeDirectory			() { return m_strMapMooneeDirectory		; }
	char *	GetMapCompactDirectory			() { return m_strMapCompactDirectory	; }
	char *	GetWorldDirectory				() { return m_strWorldDirectory			; }

	BOOL	Init					(	char *strGeometryDir	= NULL ,
										char *strMooneeDir		= NULL ,
										char *strCompactDir		= NULL ,
										char *strWorldDir		= NULL );	// ����Ÿ �ʱ�ȭ.
									// "C:\Alef\Map" �������� �������־����.

	enum	BLOCKINGTYPE
	{
		GROUND		,	// ���붥.
		SKY			,	// �ϴ�.. ������ Ż��
		UNDERGROUND	,	// ����.. �Ƹ� �Ⱦ��� ������
		GHOST			// ����.. ���ŷ�� �����Ѵ�.
	};

	BOOL	GetValidDestination				( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , FLOAT fRadius , BLOCKINGTYPE eType );
	BOOL	GetValidDestination_LineBlock	( AuPOS * pStart , AuPOS * pDest , AuPOS * pValid , FLOAT fRadius );

	BOOL	CheckBlockingInfo		( AuPOS pos , BLOCKINGTYPE eType );	// TRUE �����°� FALSE ���°�.
protected:
	INT32	CheckBlockingInfo		( INT32 x , INT32 z , BLOCKINGTYPE eType );
		// ���忡�� (0,0) �� �߽����� �ɼ����� ���� �Է��ϸ� , 
		// ���������� ���͸� ���� ���� ã���ش�.
		// ���ϰ��� ���� enum�� ����.
public:
	enum MapBlocking
	{
		NO_BLOCKING			= 0x00	,
		GEOMETRY_BLOCKING	= 0x01	,
		OBJECT_BLOCKING		= 0x02	
	};
	
	BOOL	IsPassThis				( AuPOS pos1 , AuPOS pos2 , AuPOS *pCollisionPoint  = NULL );

	// Serialization Functions..

	ApWorldSector *	GetSector		( AuPOS pos , INT32 *px = NULL , INT32 *py = NULL , INT32 *pz = NULL	);	// ��ǥ�� �����ִ� �ε����� �˾Ƴ���.
	ApWorldSector *	GetSector		( INT32 wx			, INT32 wy			, INT32 wz				);	// ���� �ε���.
	inline ApWorldSector *	GetSector( INT32 wx	, INT32 wz ) { return GetSector( wx , 0 , wz ); }
	inline ApWorldSector *	GetSectorByArrayIndex( INT32 ix, INT32 iz )
	{
		if ( ix < 0 || iz < 0 || ix >= MAP_WORLD_INDEX_WIDTH || iz >= MAP_WORLD_INDEX_HEIGHT)
			return NULL;

		return m_ppSector[ ix ][ iz ];
	}
	inline ApWorldSector *	GetSector( FLOAT fx	, FLOAT fz )
	{	
		AuPOS pos;
		pos.x = fx ; pos.y = 0.0f ; pos.z = fz;
		return GetSector( pos );
	}

	INT32	GetSectorList		( AuPOS pos , float range , ApWorldSector * apSectors[], INT32 size);

	ApWorldSector * GetSectorByArray( INT32 arrayindexX	, INT32 arrayindexY	, INT32 arrayindexZ		);	// ppSector �迭 �ε���

	INT32	GetItemList			( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size ); // return ����� ����;

	INT32	GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 = NULL , INT32 size2 = 0 ); // �� �ȿ��� ����. return ����� ����;
	INT32	GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 = NULL , INT32 size2 = 0 ); // �� ���� Ư�� ������. return ����� ����;
	INT32	GetCharList			( INT32	nDimensionIndex , INT32	lCharType ,	AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 = NULL , INT32 size2 = 0, BOOL bDbgSkill=FALSE ); // �簢�� �ȿ���. return ����� ����;

	INT32	GetCharList			( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �� �ȿ��� ����. return ����� ����;
	INT32	GetCharList			( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �� ���� Ư�� ������. return ����� ����;
	INT32	GetCharList			( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �簢�� �ȿ���. return ����� ����;

	INT32	GetNPCList			( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �� �ȿ��� ����. return ����� ����;
	INT32	GetNPCList			( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �� ���� Ư�� ������. return ����� ����;
	INT32	GetNPCList			( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �簢�� �ȿ���. return ����� ����;

	INT32	GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �� �ȿ��� ����. return ����� ����;
	INT32	GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , FLOAT range , AuMATRIX * matrix , FLOAT sinhalfthetaby2 , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2 ); // �� ���� Ư�� ������. return ����� ����;
	INT32	GetMonsterList		( INT32	nDimensionIndex , AuPOS pos , AuMATRIX * matrix , AuBOX * box , INT_PTR * array , INT32 size , INT32 * array2 , INT32 size2, BOOL bDbgSkill=FALSE ); // �簢�� �ȿ���. return ����� ����;

	INT32	GetObjectList		( INT32	nDimensionIndex , AuPOS pos , float range , INT_PTR * array , INT32 size ); // return ����� ����;

	/*
	BOOL	GetSectorDataChar	( AuPOS pos , INT32 ** pArray , INT32 * pSize		);
	BOOL	GetSectorDataItem	( AuPOS pos , INT32 ** pArray , INT32 * pSize		);
	BOOL	GetSectorDataObject	( AuPOS pos , INT32 ** pArray , INT32 * pSize		);

	BOOL	GetSectorDataChar	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		); // z��Ҵ� �׻�0 .. ������ʵ�.
	BOOL	GetSectorDataItem	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		);
	BOOL	GetSectorDataObject	( INT32 wx , INT32 wy , INT32 wz , INT32 ** pArray , INT32 * pSize		);
	*/

	FLOAT	GetHeight			( FLOAT x , FLOAT z );
	UINT8	GetType				( FLOAT x , FLOAT z	);	// �ٴ��� Ÿ���� ����.
	

	// �ӵ� ������ ���ؼ�..
	CMagUnpackManager * GetUnpackManagerDivisionCompact		( INT32 nDivisionIndex );
	CMagUnpackManager * GetUnpackManagerDivisionGeometry	( INT32 nDivisionIndex );
	CMagUnpackManager * GetUnpackManagerDivisionMoonee		( INT32 nDivisionIndex );

	void				FlushUnpackManagerDivision();

	struct	CompactDataUnpackManagerBufferElement
	{
		CMagUnpackManager * pUnpack			;
		INT32				nDivisionIndex	;
		UINT32				uLastAccessTime	;
	};

	CompactDataUnpackManagerBufferElement	m_ArrayUnpackBufferCompact	[ MAP_UNPACK_MANAGER_BUF ];
	CompactDataUnpackManagerBufferElement	m_ArrayUnpackBufferGeometry	[ MAP_UNPACK_MANAGER_BUF ];
	CompactDataUnpackManagerBufferElement	m_ArrayUnpackBufferMoonee	[ MAP_UNPACK_MANAGER_BUF ];
	
public:
	// �ش� ���Ϳ����� ������ �����Ѵ�.

	// ������ ������ �޸𸮿� ����� �������.�� �����ϴ� �� �Լ��̴�.
	// ������ ������� �ٸ� ����..
	ApWorldSector *
			SetupSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);
		// ���͸� �غ���.. ���͸� �ε������� ����.
		// ���� �޸𸮸� �Ҵ��ϰ� , �ε��� �غ� ��.  �� �������� add char �� ���� ����� ����� �� ����.
	
	BOOL	DeleteSector	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);
	BOOL	DeleteSector	( ApWorldSector	* pSector );// RemovePool�� �ִ´�.
	BOOL	DeleteSectorReal( ApWorldSector	* pSector );// �޸𸮿��� ������.
		// Setup Sector�� �ݴ�Ŵ� �Լ� , ������ü�� �޸𸮿��� ������..
		// �����԰� ���ÿ� ���� ����Ÿ���� ���� ����.

	void	FlushDeleteSectors();
		// ������ (2004-12-03 ���� 12:38:27) : ����Ʈ�� �ö󰣰�ó����..


	// ����Ÿ �ε� ���..
	// ���ʹ� �¾��� �ž� �־����..
	BOOL			LoadSector		( ApWorldSector	* pSector , BOOL bServer = FALSE );

	// �ε����� �߸� ����� ���̳� ���ɼ��� ����..
	inline	BOOL	LoadSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z , BOOL bServer = FALSE )
	{
		return LoadSector	( m_ppSector[ SectorIndexToArrayIndexX( sectorindex_x ) ][ SectorIndexToArrayIndexZ( sectorindex_z ) ] , bServer );
	}
		// ������ ���� ������ �޸𸮿� �Ҵ��ϰ� , �ϵ忡�� �ε���.
		// ���ʹ� Setup �ž��־����..
		// �ε� �ݹ��� ȣ���,.

	BOOL			ClearSector		( ApWorldSector	* pSector											);
	inline	BOOL	ClearSector		( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	)
	{
		return ClearSector	( m_ppSector[ SectorIndexToArrayIndexX( sectorindex_x ) ][ SectorIndexToArrayIndexZ( sectorindex_z ) ] );
	}
		// �ε� �ž��ִ� ������������ �޸𸮿��� ������.
		// ��Ʈ���� �ݹ��� ȣ���..

	// ������.. �Ⱦ�..
	//BOOL	_LoadSectorOld	( INT32 sectorindex_x , INT32 sectorindex_y , INT32 sectorindex_z	);

	// ��ü �ε�..
	BOOL	LoadAll			( BOOL bLoadData = TRUE , BOOL bServerData = FALSE , ProgressCallback pfCallback = NULL , void * pData = NULL );
	BOOL	SaveAll			();
	void	DeleteAllSector	();
	void	DeleteRealAllSector	();
	void	ClearAllSector	();

	BOOL	LoadDivision	( UINT32 uDivision , BOOL bLoadData , BOOL bServerData );

	BOOL	AddChar			( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS pos,		INT_PTR cid,		INT_PTR cid2,			INT32 range	= 0);
	BOOL	UpdateChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteChars		( INT32	nDimensionIndex , ApmMapCharType eCharType,		ApWorldSector * pSector							);
	BOOL	MoveChar		( INT32	nDimensionIndex , ApmMapCharType eCharType,		ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddChar			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2,			INT32 range = 0		);
	BOOL	UpdateChar		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteChar		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteChars		( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveChar		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddNPC			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2,			INT32 range = 0		);
	BOOL	UpdateNPC		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteNPC		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteNPCs		( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveNPC			( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddMonster		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid ,		INT_PTR cid2,			INT32 range = 0		);
	BOOL	UpdateMonster	( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent ,	INT_PTR cid,			INT_PTR cid2	);
	BOOL	DeleteMonster	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR cid						);
	BOOL	DeleteMonsters	( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveMonster		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR cid,		INT_PTR cid2,		AuPOS posCurrent	);

	BOOL	AddItem			( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid						);
	BOOL	UpdateItem		( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR iid	);
	BOOL	DeleteItem		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR iid						);
	BOOL	DeleteItems		( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveItem		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR iid,	AuPOS posCurrent	);

	// Parn �۾� Blocking ���� �߰�
	BOOL	AddObject		( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid ,						AuBLOCKING *pstBlocking = NULL);
	BOOL	UpdateObject	( INT32	nDimensionIndex , AuPOS posPrev ,	AuPOS posCurrent , INT_PTR oid	);
	BOOL	DeleteObject	( INT32	nDimensionIndex , AuPOS pos ,		INT_PTR oid						);
	BOOL	DeleteObjects	( INT32	nDimensionIndex , ApWorldSector * pSector							);
	BOOL	MoveObject		( INT32	nDimensionIndex , ApWorldSector * pOldSector,	ApWorldSector * pNewSector,		INT_PTR oid,	AuPOS posCurrent	);


	// ���� ��ŷ �߰� ��ǵ�..
	void	AddObjectBlocking		( AuBLOCKING * pBlocking  , INT32 count );
	void	ClearAllObjectBlocking	();	// ������Ʈ ��ŷ ���� ������ �̰� ȣ���ؼ� Ŭ������ �� �ٽ� �ۼ��Ѵ�.
	void	ClearAllHeightPool		();

	// Remove�Ǵ� Sector�� ó�� ���� ��ǵ�..
	void	SetRemoveSector	( INT32 lRemoveDelay ,	INT32 lRemovePool			) { m_lSectorRemoveDelay = lRemoveDelay; m_lSectorRemovePool = lRemovePool; }
	void	ProcessRemove	( UINT32 ulClockCount								);

	// ���ε� ��� �߰� ���. 
	// ������ (2004-03-08 ���� 11:54:03) : 
	BindTypeList	m_listBind;

	INT32	AddBindType			(); 
	void	ClearAllBindData	();
	BindList	* GetBindList	( INT32 nBindIndex );

	// return Binding Type 
	// ���ε� ��� �߰��� ������ �Ҵ��ϰ� �ش� ����� �ε����� �����Ѵ�. 
	BOOL	AddBindElementRect	( INT32 nBindIndex , INT32 nData , AuPOS pos , FLOAT width , FLOAT height	); 
	BOOL	AddBindElementCircle( INT32 nBindIndex , INT32 nData , AuPOS pos , FLOAT radius					); 

	BOOL	IsChangedBind		( INT32 nBindIndex , AuPOS prev , AuPOS next	);  
	INT32	GetBindData			( INT32 nBindIndex , AuPOS pos					); 

	// ���ε� �ɸ��°� ������ 0 ����.

	FLOAT	GetMinHeight		( ApWorldSector * pSector );

public:
	// ��ⵥ��Ÿ �߰� �������̽� ���
	INT16	AttachSectorData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16	AttachSegmentData	( PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

public:
	// ��� �޴����� ���� �̺�Ʈ��.
	virtual BOOL	OnAddModule	();
	virtual BOOL	OnInit		();
	virtual BOOL	OnDestroy	();
	virtual BOOL	OnIdle(UINT32 ulClockCount);

	BOOL	SetCallbackAddChar		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveChar	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackMoveChar		(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackAddItem		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveItem	(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackLoadSector	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackClearSector	(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// ������ (2005-03-23 ���� 3:32:18) : 
	// pData �� FILE *
	// pCustom �� ApWorldSector *
	BOOL	SetCallbackSaveGeometry	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_SAVEGEOMETRY	, pfCallback, pClass); }
	BOOL	SetCallbackLoadGeometry	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_LOADGEOMETRY	, pfCallback, pClass); }
	BOOL	SetCallbackSaveMoonee	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_SAVEMOONEE		, pfCallback, pClass); }
	BOOL	SetCallbackLoadMoonee	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_LOADMOONEE		, pfCallback, pClass); }
	BOOL	SetCallbackGetMinHeight	(ApModuleDefaultCallBack pfCallback, PVOID pClass){ return SetCallback( APMMAP_CB_ID_GET_MIN_HEIGHT	, pfCallback, pClass); }
	
	////////////////////////////////////////////////////////
	// ������Ʈ ����...
	////////////////////////////////////////////////////////
	AuList< stLoadedDivision >	m_listLoadedDivision;
	stLoadedDivision *	GetLoadedDivisionInfo	( UINT32 uDivisionIndex );
	BOOL				EnumLoadedDivision		( ApModuleDefaultCallBack pCallback , PVOID pClass );
												// pCustomData �� ������� ����..
												// pClass = �Է� Ŭ����..
												// pData = stLoadedDivision *

	// Low level function
	stLoadedDivision *	AddLoadedDivision		( UINT32 uDivisionIndex );
	BOOL				RemoveLoadedDivision	( UINT32 uDivisionIndex );

	//////////////////////////////////////////////
	// ���� ��Ʈ��..

	enum FieldType
	{
		FT_FIELD	= 0x00000000,
		FT_TOWN		= 0x00000001,
		FT_PVP		= 0x00000002
	};

	enum SafetyType
	{
		ST_SAFE		= 0x00000000,
		ST_FREE		= 0x00000001,
		ST_DANGER	= 0x00000002
	};

	struct RegionTemplate
	{
		INT32	nIndex					;
		char	pStrName[ 32 ]			;
		INT32	nParentIndex			;	// Region�� ������ ���� ���� Region
		INT32	nWorldMapIndex			;	// ����� ��ȣ ����
		INT32	nSkyIndex				;
		FLOAT	fVisibleDistance		;	// �þ߰Ÿ�.
		FLOAT	fMaxCameraHeight		;	// Top View ī�޶� �ִ� ����.
		INT32	nLevelLimit				;	// ��������.. 0�̸� ����.
		INT32	nLevelMin				;	// ���� �̴ϸ�.. 0�̸� ����
		INT32	nUnableItemSectionNum	;	// �������� ���ѵǴ� ItemSectionNumber
		AuPOS	stResurrectionPos		;	// ��Ȱ ����

		struct PropertyList
		{
			BYTE	uFieldType		: 2;	// ���� �ʵ� ���� 
			BYTE	uSafetyType		: 2;	// ���� �������� ��������

			BYTE	bRidable		: 1;	// Ż�� ����
			BYTE	bPet			: 1;	// �� ��� ����..
			BYTE	bItemWangbok	: 1;	// �պ����� ��� ����.
			BYTE	bItemPotion		: 1;	// ȸ�� ���� ���..
			// 1Byte

			BYTE	bItemResurrect	: 1;	// ��Ȱ ������..
			BYTE	bDisableMinimap	: 1;	// �̴ϸ� ������ �ʰ���
			BYTE	bJail			: 1;	// �����÷���.
			BYTE	bZoneLoadArea	: 1;	// ���ε� ����
			BYTE	bCharacterBlock	: 1;	// �ɸ��� ��ŷ.
			BYTE	bItemPotion2	: 1;	// ���� Ÿ��2
			BYTE	bItemPotionGuild: 1;	// ������� ����.
			BYTE	bUnderRoof		: 1;	// õ���� �ִ°�.. ���̳� �� �ȿ����ϴ°�..
			BYTE	bRecallItem		: 1;	// ��ȯ���� , ȯ���ǹ� ��� ����
			BYTE	bVitalPotion	: 1;	// Ȱ�� ���� ��� ����
			BYTE	bCurePotion		: 1;	// ġ�� ���� ��� ����
			BYTE	bRecoveryPotion : 1;	// ȸ�� ���� ��� ����

			// 1Byte
			BYTE	uReserved2		;		// �����ʵ�~
			// 1Byte
			BYTE	uReserved3		;		// �����ʵ�~
			// 1Byte
		};

		struct	ZoneLoadInfo
		{
			FLOAT	fSrcX			;	// ������ ������ ��ġ
			FLOAT	fSrcZ			;
			FLOAT	fHeightOffset	;	// ������ ������ ����
			FLOAT	fDstX			;	// ��ǥ���� 
			FLOAT	fDstZ			;	// ��ǥ���� 
			INT32	nRadius			;	// �ε��� ����.

			ZoneLoadInfo():fSrcX( 0.0f ),fSrcZ( 0.0f ),fHeightOffset( 0.0f ),fDstX( 0 ),fDstZ( 0 ),nRadius( 0 ){}
		};

		union TypeInfo
		{
			INT32			nType	;
			PropertyList	stType	;
		};

		TypeInfo		ti;
		ZoneLoadInfo	zi;

		char	pStrComment[ 128 ]	;

		RegionTemplate():
			nIndex			( -1	),
			nParentIndex	( -1	),
			nWorldMapIndex	( 0		),
			nSkyIndex		( 13	),
			fVisibleDistance( 1200.0f),	// 12����.
			fMaxCameraHeight( 2750.0f),
			nLevelLimit		( 0 ),
			nLevelMin		( 0 ), // ���� ����
			nUnableItemSectionNum( 0 )
		{
			ti.nType		= 0		;

			strcpy( pStrName	, "" );
			strcpy( pStrComment	, "" );

			ti.stType.uFieldType		= FT_FIELD	;
			ti.stType.uSafetyType		= ST_SAFE	;
			ti.stType.bRidable			= TRUE		;	// Ż�� ����
			ti.stType.bPet				= TRUE		;	// �� ��� ����..
			ti.stType.bItemWangbok		= TRUE		;	// �պ������� üũ��..
			ti.stType.bItemPotion		= TRUE		;	// ȸ�� ���� ���..
			ti.stType.bItemResurrect	= TRUE		;	// ��Ȱ ������..
			ti.stType.bDisableMinimap	= FALSE		;	// �̴ϸ� ����
			ti.stType.bJail				= FALSE		;	// ���� ����.
			ti.stType.bItemPotion2		= FALSE		;
			ti.stType.bItemPotionGuild	= FALSE		;
			ti.stType.bRecallItem		= TRUE		;
			ti.stType.bVitalPotion		= FALSE		;
			ti.stType.bCurePotion		= TRUE		;
			ti.stType.bRecoveryPotion	= TRUE		;

			stResurrectionPos.x			= 0			;
			stResurrectionPos.y			= 0			;
			stResurrectionPos.z			= 0			;

		}
	};

	AuList< RegionTemplate > m_listTemplate;
	BOOL				AddTemplate		( RegionTemplate * pTemplate );
	BOOL				RemoveTempate	( int nIndex );
	RegionTemplate *	GetTemplate		( int nIndex );
	RegionTemplate *	GetTemplate		( char * pstrRegionName );

	BOOL	SaveTemplate( char * pFileName , BOOL bEncryption = FALSE);
	BOOL	LoadTemplate( char * pFileName , BOOL bDecryption = FALSE);

	// ���� Region���� Ȯ���Ѵ� 
	BOOL	IsSameRegion		( RegionTemplate* pstBase, RegionTemplate* pstTarget);
	// ���� Region���� Ȯ���Ѵ� (���� �ε������α��� Ȯ��)
	BOOL	IsSameRegionInvolveParent( RegionTemplate* pstBase, RegionTemplate* pstTarget);

	enum REGION_KIND
	{
		RK_BOX		,
		RK_CIRCLE	,
		RK_MAX
	};

	struct RegionElement
	{
		INT32	nIndex	;
		INT32	nKind	;

		INT32	nStartX	;
		INT32	nStartZ	;
		INT32	nEndX	;
		INT32	nEndZ	;

		RegionElement()
		{
			// ���� �Ŀ� �߰�.. ������� �ʵ�.
			nKind = RK_BOX;
		}
	};

	AuList< RegionElement > m_listElement;
	
	BOOL						AddRegion( RegionElement * pElement )	{	return AddRegion( pElement->nIndex , pElement->nStartX , pElement->nStartZ , pElement->nEndX , pElement->nEndZ , pElement->nKind ); }
	BOOL						AddRegion( int nIndex , INT32 nStartX , INT32 nStartZ , INT32 nEndX , INT32 nEndZ , INT32 nKind = RK_BOX );
	AuNode< RegionElement > *	GetRegion( INT32 nOffsetX , INT32 nOffsetZ );
	BOOL						RemoveRegion( AuNode< RegionElement > * pNode );

	void						RemoveAllRegion();
	BOOL						SaveRegion( char * pFileName , BOOL bEncryption = FALSE);
	BOOL						LoadRegion( char * pFileName , BOOL bDecryption = FALSE);

	UINT16						GetRegion( FLOAT x , FLOAT z );

public:
	/////////////////////////////////////////
	// ��ó�� ���� ��Ʈ����..

	struct	DimensionTemplate
	{
		INT32	nTID	;
		char	strName[ DIMENSION_NAME_LENGTH ]	;	// ���̸�..
		INT32	nStartX	;
		INT32	nStartZ	;
		INT32	nEndX	;
		INT32	nEndZ	;

		// property..
		std::vector< INT32 > arrayCreatedDimension;
	};

	// ���ø� ����..
	std::vector< DimensionTemplate > m_arrayDimensionTemplate;

	BOOL	LoadDimensionTemplate( char * strFilename );
	BOOL	SaveDimensionTemplate( char * strFilename );

	BOOL	AddDimensionTemplate( DimensionTemplate * pTemplate );
	BOOL	RemoveDimensionTemplate( INT32 nTID );
	DimensionTemplate * GetDimensionTemplate( INT32 nTID );

	BOOL	CreateDimension	( INT32 nTID );
	BOOL	DestroyDimension( INT32 nTID , INT32 nIndex );

public:

	// ������ (2005-05-24 ���� 2:29:02) : 
	// ����� ����..
	// �����ڵ�� ������������ ����� �����̹Ƿ�.. 
	// �ڵ� ���̻� ���⿡ ��ġ.
	struct	WorldMap
	{
		INT32	nMID;	// Map ID, �����̸��� wmap0001 �������� ������ ���̵�� �Ѵ�. ���� ī���� �����ʰ� ����Ÿ�� �غ�ž� �־����.
		char	strComment[ WORLDMAP_COMMENT_LENGTH ];

		// Ŀ�� ����
		FLOAT	xStart		;
		FLOAT	zStart		;
		FLOAT	xEnd		;
		FLOAT	zEnd		;

		INT32	nMapItemID	;	// -1 �̸� NO ITEM . ������ ���̵� ǥ�õ�.

		WorldMap() :	nMID ( 0 ) , xStart ( 0.0f ) , zStart ( 0.0f ) ,
						xEnd ( 0.0f ) , zEnd ( 0.0f ) , nMapItemID ( -1 )			
		{
			strcpy( strComment , "No Map" );
		}
	};

	std::vector< WorldMap > m_arrayWorldMap;
	BOOL	LoadWorldMap( char * pFileName , BOOL bDecryption = FALSE );
	BOOL	SaveWorldMap( char * pFileName , BOOL bEncryption = FALSE );

	WorldMap *	GetWorldMapInfo( INT32 nMID );

	// Clean Up
	void	RemoveAllWorldMap();

	inline INT32	GetTargetPositionLevelLimit( AuPOS &stPos )
	{
		RegionTemplate	* pTemplate = GetTemplate( GetRegion( stPos.x , stPos.z ) );
		return pTemplate ? pTemplate->nLevelLimit : 0 ;
	}

	RegionPerculiarityMap	m_RegionPerculiarityMap;
	AuXmlDocument			m_XmlData;
	AuXmlNode*				m_pRootRuleNode;

	BOOL							LoadRegionPerculiarity( BOOL bEncrypt = FALSE );
	AgpmMapRegionPeculiarityReturn	CheckRegionPerculiarity(INT32 lRegionIndex, AgpmMapRegionPeculiarity ePeculiarity, INT32 lResevedOption = 0);
	INT32							LoadRegioinPerculiaritySkulTid(INT32 lRegionIndex);
	INT32							LoadRegioinPerculiaritySkulRestrictionTime(INT32 lRegionIndex);
};

// �۷ι� ��ġ�� ���� �ε��� ����..

// posZ += MAP_SECTOR_HEIGHT * ( float )( MAP_WORLD_INDEX_HEIGHT / 2 );
// return ( int ) ( ( posZ ) / MAP_SECTOR_HEIGHT  ) - MAP_WORLD_INDEX_HEIGHT / 2;

// �÷�Ʈ ����� �ӿ����� �־ , ��Ʈ�� �ٲ��� ����Ѵ�.
inline INT32 PosToSectorIndexX			( FLOAT posX	) { return ( ( ( int ) posX + 2560000 ) / 6400 ) - 400; }
inline INT32 PosToSectorIndexX			( AuPOS pos		) { return PosToSectorIndexX( pos.x ); }
inline INT32 PosToSectorIndexZ			( FLOAT posZ	) { return ( ( ( int ) posZ + 2560000 ) / 6400 ) - 400; }
inline INT32 PosToSectorIndexZ			( AuPOS pos		) { return PosToSectorIndexZ( pos.z ); }

// ������ (2004-03-30 ���� 2:29:15) : �Ʒ� �ڵ�� ������..
//inline	INT32 PosToSectorIndexX			( FLOAT posX	)
//{
//	if( posX >= 0.0f )	return MagFToI ( posX / MAP_SECTOR_WIDTH  )							;
//	else				return MagFToI ( ( posX - MAP_SECTOR_WIDTH + 1.0f	) / MAP_SECTOR_WIDTH )	;
//}
//inline	INT32 PosToSectorIndexZ			( FLOAT posZ	)
//{
//	if( posZ >= 0.0f )	return MagFToI ( posZ / MAP_SECTOR_HEIGHT )							;
//	else				return MagFToI ( ( posZ - MAP_SECTOR_HEIGHT + 1.0f	) / MAP_SECTOR_HEIGHT )	;
//}

// �ε��� �ý��� ����.. 
// ������� �����Ѵ�.. ��ʿ�..

#define	TEXTURE_DIMENTION_DEFAULT	0

// �̰� Ŭ���̾�Ʈ ��� ����� ���پ�..
//inline int GET_TEXTURE_DIMENSION( int index  )
//{
//	int dim = ( ( index & 0x0f000000 ) >> 24	);
//
//	if( dim == TEXTURE_DIMENTION_DEFAULT )
//	{
//		switch( GET_TEXTURE_TYPE( index ) )
//		{
//		case	TT_FLOORTILE	:	return	TT_FLOOR_DEPTH	;
//		case	TT_UPPERTILE	:	return	TT_UPPER_DEPTH	;
//		case	TT_ONE			:	return	TT_ONE_DEPTH	;
//		case	TT_ALPHATILE	:	return	TT_ALPHA_DEPTH	;
//		default:
//			ASSERT( !"-__-!" );
//			break;
//		}
//	}
//	else	return dim;
//}
inline int GET_TEXTURE_CATEGORY	( int index  ) { return ( ( index & 0x00ff0000 ) >> 16	); }
inline int GET_TEXTURE_INDEX	( int index  ) { return ( ( index & 0x0000ff00 ) >> 8	); }
inline int GET_TEXTURE_TYPE		( int index  ) { return ( ( index & 0x000000c0 ) >> 6	); }
inline int GET_TEXTURE_OFFSET	( int index  ) { return (   index & 0x0000003f			); }

inline int GET_TEXTURE_OFFSET_MASKING_OUT
								( int index  ) { return ( index & 0x0fffffc0			); }
inline int GET_TEXTURE_DIMENTION_MASKING_OUT
								( int index  ) { return ( index & 0x00ffffff			); }


inline int MAKE_TEXTURE_INDEX	( int dimension , int category , int index , int type , int offset )
{
#ifdef _DEBUG
	ASSERT( dimension	<=	8	);
	ASSERT( category	<	256	);
	ASSERT( index		<	256	);
	ASSERT( type		<	4	);
	ASSERT( offset		<	64	);
#endif

	return (
				( dimension	<< 24	) |
				( category	<< 16	) |
				( index		<< 8	) |
				( type		<< 6	) |
				( offset			) );
}

inline int UPDATE_TEXTURE_OFFSET( int index , int offset = 0 )
{
	return ( index & 0xffffffc0 ) | ( offset & 0x0000003f );
}

inline BOOL CHECK_MAP_ARRAY_INDEX( int arrayx , int arrayz )
{
	if( 0 <= arrayx && arrayx <= MAP_WORLD_INDEX_WIDTH &&
		0 <= arrayz && arrayz <= MAP_WORLD_INDEX_HEIGHT	)
		return TRUE	;
	else
		return FALSE;
}

inline BOOL CHECK_MAP_SECTOR_INDEX( int sectorx , int sectorz )
{
	if( 0 <= SectorIndexToArrayIndexX( sectorx ) && SectorIndexToArrayIndexX( sectorx ) <= MAP_WORLD_INDEX_WIDTH &&
		0 <= SectorIndexToArrayIndexZ( sectorz ) && SectorIndexToArrayIndexZ( sectorz ) <= MAP_WORLD_INDEX_HEIGHT	)
		return TRUE	;
	else
		return FALSE;
}

// �ӽü���
/*inline FLOAT	GetSectorStartX	( int indexX )	{ 
	if( indexX >= 0 )	return ( float ) indexX			* MAP_SECTOR_WIDTH	;
	else				return ( float ) indexX			* MAP_SECTOR_WIDTH	- MAP_SECTOR_WIDTH;
}
inline FLOAT	GetSectorStartZ	( int indexZ )	{
	if( indexZ >= 0 )	return ( float ) indexZ			* MAP_SECTOR_HEIGHT	;
	else				return ( float ) indexZ			* MAP_SECTOR_HEIGHT	- MAP_SECTOR_HEIGHT;
}

inline FLOAT	GetSectorEndX	( int indexX )	{ return ( float ) GetSectorStartX(indexX+1); }
inline FLOAT	GetSectorEndZ	( int indexZ )	{ return ( float ) GetSectorStartZ(indexZ+1); }
*/

inline FLOAT	GetSectorStartX	( int indexX )  { return ( float ) indexX			* MAP_SECTOR_WIDTH	; }
inline FLOAT	GetSectorStartZ	( int indexZ )	{ return ( float ) indexZ			* MAP_SECTOR_HEIGHT	; }
inline FLOAT	GetSectorEndX	( int indexX )	{ return ( float ) ( indexX + 1 )	* MAP_SECTOR_WIDTH	; }
inline FLOAT	GetSectorEndZ	( int indexZ )	{ return ( float ) ( indexZ + 1 )	* MAP_SECTOR_HEIGHT	; }

inline INT32	MakeDivisionIndex( INT32 x , INT32 z ){ return ( x * 100 + z );		}

inline INT32	GetDivisionIndex( int sectorArrayIndexX , int sectorArrayIndexZ )
{
	INT32	x = sectorArrayIndexX / MAP_DEFAULT_DEPTH;
	INT32	z = sectorArrayIndexZ / MAP_DEFAULT_DEPTH;
	return MakeDivisionIndex( x , z );
}

inline INT32	GetDivisionIndexF( FLOAT fPosX , FLOAT fPosZ )
{
	INT32	nSectorX	=	PosToSectorIndexX( fPosX );
	INT32	nSectorZ	=	PosToSectorIndexZ( fPosZ );

	return GetDivisionIndex( 
		SectorIndexToArrayIndexX( nSectorX )	,
		SectorIndexToArrayIndexZ( nSectorZ )	);
}


inline INT32	GetDivisionXIndex( INT32 nDivision )	{ return ( nDivision / 100 );	}
inline INT32	GetDivisionZIndex( INT32 nDivision )	{ return ( nDivision % 100 );	}

// Return ArraySectorIndex
#define	GetFirstSectorXInDivision( nDivision )	( GetDivisionXIndex( nDivision ) * MAP_DEFAULT_DEPTH )
#define	GetFirstSectorZInDivision( nDivision )	( GetDivisionZIndex( nDivision ) * MAP_DEFAULT_DEPTH )

struct DivisionInfo
{
	INT32	nIndex	;
	FLOAT	fStartX	;
	FLOAT	fStartZ	;
	FLOAT	fWidth	;

	INT32	nX		;// Array Index
	INT32	nZ		;// Array Index
	INT32	nDepth	;

	void	GetDivisionInfo( INT32 nDivision )
	{
		nX		= GetFirstSectorXInDivision( nDivision );
		nZ		= GetFirstSectorZInDivision( nDivision );
		nDepth	= MAP_DEFAULT_DEPTH	;

		fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( nX ) );
		fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexX( nZ ) );
		fWidth	= MAP_SECTOR_WIDTH * nDepth;

		nIndex	= nDivision;
	}
};

typedef BOOL	(*DivisionCallback ) ( DivisionInfo * pDivisionInfo , PVOID pData );

#endif // !defined(AFX_APMMAP_H__D09E90BC_BF4C_482A_AE9C_E19838224685__INCLUDED_)

