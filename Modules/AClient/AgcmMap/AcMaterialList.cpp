// MaterialList.cpp: implementation of the AcMaterialList class.
//
//////////////////////////////////////////////////////////////////////

#include <ApBase/ApBase.h>

#include <rwcore.h>
#include <rpworld.h>
#include <AgcmMap/AgcmMap.h>
#include "AcMaterialList.h"
#include <AcuRpMTexture/AcuRpMTexture.h>
#include <ApBase/MagDebug.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


AcTextureList *	AcMaterialList::m_pTextureList = NULL	;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcMaterialList::AcMaterialList()
{
	m_pGeometry	= NULL;
}

AcMaterialList::~AcMaterialList()
{
	// Material ����..
	RemoveAll();
}

INT32	AcMaterialList::GetMatIndex	(	UINT32 firsttextureindex		,
										UINT32 secondtextureindex		,
										UINT32 thirdtexutreindex		,
										UINT32 fourthtextureindex		,
										UINT32 fifthtextureindex		,
										UINT32 sixthtextureindex		)
{
	ASSERT( NULL != AcMaterialList::m_pTextureList );

	if( firsttextureindex == ALEF_SECTOR_DEFAULT_TILE_INDEX )
	{
		return 0;
	}

	// Offset ������.
	firsttextureindex	= UPDATE_TEXTURE_OFFSET( firsttextureindex	);
	secondtextureindex	= UPDATE_TEXTURE_OFFSET( secondtextureindex	);
	thirdtexutreindex	= UPDATE_TEXTURE_OFFSET( thirdtexutreindex	);
	fourthtextureindex	= UPDATE_TEXTURE_OFFSET( fourthtextureindex	);
	fifthtextureindex	= UPDATE_TEXTURE_OFFSET( fifthtextureindex	);
	sixthtextureindex	= UPDATE_TEXTURE_OFFSET( sixthtextureindex	);

	// �켱 ����Ʈ�� ����ִ��� �����Ѵ�.
	MaterialData *	pMatInfo;
	pMatInfo	= Find( firsttextureindex , secondtextureindex , thirdtexutreindex , fourthtextureindex , fifthtextureindex , sixthtextureindex );

	if( pMatInfo ) return pMatInfo->matindex;

	// ������ �ؽ��� ����Ʈ���� �̾ƿ;��Ѵ�.

	RwTexture * pTexture[ TD_DEPTH ];
	if( firsttextureindex != ALEF_TEXTURE_NO_TEXTURE )
	{
		pTexture[ TD_FIRST ] = AcMaterialList::m_pTextureList->GetTexture( firsttextureindex );
	}
	else
		pTexture[ TD_FIRST ] = NULL;
	
	if( secondtextureindex != ALEF_TEXTURE_NO_TEXTURE )
	{
		pTexture[ TD_SECOND ] = AcMaterialList::m_pTextureList->GetTexture( secondtextureindex );
	}
	else
		pTexture[ TD_SECOND ] = NULL;

	if( thirdtexutreindex != ALEF_TEXTURE_NO_TEXTURE )
	{
		pTexture[ TD_THIRD ] = AcMaterialList::m_pTextureList->GetTexture( thirdtexutreindex );
	}
	else
		pTexture[ TD_THIRD ] = NULL;

	if( fourthtextureindex != ALEF_TEXTURE_NO_TEXTURE )
	{
		pTexture[ TD_FOURTH ] = AcMaterialList::m_pTextureList->GetTexture( fourthtextureindex );
	}
	else
		pTexture[ TD_FOURTH ] = NULL;

	if( fifthtextureindex != ALEF_TEXTURE_NO_TEXTURE )
	{
		pTexture[ TD_FIFTH ] = AcMaterialList::m_pTextureList->GetTexture( fifthtextureindex );
	}
	else
		pTexture[ TD_FIFTH ] = NULL;

	if( sixthtextureindex != ALEF_TEXTURE_NO_TEXTURE )
	{
		pTexture[ TD_SIXTH ] = AcMaterialList::m_pTextureList->GetTexture( sixthtextureindex );
	}
	else
		pTexture[ TD_SIXTH ] = NULL;

	// �ؽ��� �ε� ��..
	// ���� ����ó���� ���� ����.


	// ���°Ÿ� ���� ����Ѵ�.
	pMatInfo = GetEmptyMaterial( );
	if( pMatInfo )
	{
		// ��d!...
		RpMaterialSetTexture					( pMatInfo->pMateterial ,		pTexture[ TD_FIRST	]	);
		if( pTexture[ TD_SECOND	] )
			RpMTextureMaterialSetTexture		( pMatInfo->pMateterial , 0	,	pTexture[ TD_SECOND	]	, rpMTEXTURE_TYPE_ALPHA			);
		if( pTexture[ TD_THIRD	] )
			RpMTextureMaterialSetTexture		( pMatInfo->pMateterial , 1	,	pTexture[ TD_THIRD	]	, rpMTEXTURE_TYPE_NORMAL		);
		if( pTexture[ TD_FOURTH	] )
			RpMTextureMaterialSetTexture		( pMatInfo->pMateterial , 2	,	pTexture[ TD_FOURTH	]	, rpMTEXTURE_TYPE_ALPHA			);
		if( pTexture[ TD_FIFTH	] )
			RpMTextureMaterialSetTexture		( pMatInfo->pMateterial , 3 ,	pTexture[ TD_FIFTH	]	, rpMTEXTURE_TYPE_NORMAL		);
		if( pTexture[ TD_SIXTH	] )
			RpMTextureMaterialSetTexture		( pMatInfo->pMateterial , 4	,	pTexture[ TD_SIXTH	]	, rpMTEXTURE_TYPE_NORMALALPHA	);
		
		pMatInfo->pIndex[ TD_FIRST	]		= firsttextureindex		;
		pMatInfo->pIndex[ TD_SECOND	]		= secondtextureindex	;
		pMatInfo->pIndex[ TD_THIRD	]		= thirdtexutreindex		;
		pMatInfo->pIndex[ TD_FOURTH	]		= fourthtextureindex	;
		pMatInfo->pIndex[ TD_FIFTH	]		= fifthtextureindex		;
		pMatInfo->pIndex[ TD_SIXTH	]		= sixthtextureindex		;

		return pMatInfo->matindex;
	}
	else
	{
		// �� ��Ƽ������ ���ٳ�;
		TRACE( "�� ��Ƽ������ �����\n" );
		return ALEF_MATERIAL_LIST_NO_MATERIAL;
	}
}

void		AcMaterialList::RemoveTexture	( int matindex		)
{
	ASSERT( !"�������� ����" );

//	MaterialData *	pMatInfo;
//	pMatInfo	= Find( matindex );
//	if( pMatInfo )
//	{
//		RemoveTexture( pMatInfo->pMateterial );
//		RpMTextureMaterialSetTexture	( pMatInfo->pMateterial , 0 , NULL	);
//		RpMTextureMaterialSetTexture	( pMatInfo->pMateterial , 1 , NULL	);
//		RpMTextureMaterialSetTexture	( pMatInfo->pMateterial , 2 , NULL	);
//	}
}

void		AcMaterialList::RemoveTexture	( RpMaterial * pMat	)
{
	ASSERT( !"�������� ����" );
//	RpMaterialSetTexture( pMat , NULL );
//	RpMTextureMaterialSetTexture	( pMat , 0 , NULL	);
//	RpMTextureMaterialSetTexture	( pMat , 1 , NULL	);
//	RpMTextureMaterialSetTexture	( pMat , 2 , NULL	);
}

// Private Functions..
AcMaterialList::MaterialData *	AcMaterialList::Find( int			matindex		)									// ��Ƽ���� �ε����� ã��.
{
	MaterialData *				pMatInfo;
	AuNode< MaterialData > *	pNode	;

	pNode = list.GetHeadNode();

	while( pNode )
	{
		pMatInfo = & pNode->GetData();
		if( pMatInfo->matindex	== matindex )
		{
			return pMatInfo;
		}

		list.GetNext( pNode );
	}

	// ã�� Material�� �����.
	return NULL;
}

AcMaterialList::MaterialData *	AcMaterialList::Find( RpMaterial *	pMat			)									// ��Ƽ���� �����ͷ� ã��.
{
	MaterialData *				pMatInfo;
	AuNode< MaterialData > *	pNode	;

	pNode = list.GetHeadNode();

	while( pNode )
	{
		pMatInfo = & pNode->GetData();
		if( pMatInfo->pMateterial == pMat )
		{
			return pMatInfo;
		}

		list.GetNext( pNode );
	}

	// ã�� Material�� �����.
	return NULL;
}

AcMaterialList::MaterialData *	AcMaterialList::Find(
										UINT32 firsttextureindex		,
										UINT32 secondtextureindex		,
										UINT32 thirdtexutreindex		,
										UINT32 fourthtextureindex		,
										UINT32 fifthtextureindex		,
										UINT32 sixthtextureindex		)
{
	// �ؽ��� �ε����� ã��.

	MaterialData *				pMatInfo;
	AuNode< MaterialData > *	pNode	;

	pNode = list.GetHeadNode();

	while( pNode )
	{
		pMatInfo = & pNode->GetData();
		if( IsSameTexture( pMatInfo->pIndex[ TD_FIRST	]	, firsttextureindex		)	&&
			IsSameTexture( pMatInfo->pIndex[ TD_SECOND	]	, secondtextureindex	)	&&
			IsSameTexture( pMatInfo->pIndex[ TD_THIRD	]	, thirdtexutreindex		)	&&
			IsSameTexture( pMatInfo->pIndex[ TD_FOURTH	]	, fourthtextureindex	)	&&
			IsSameTexture( pMatInfo->pIndex[ TD_FIFTH	]	, fifthtextureindex		)	&&
			IsSameTexture( pMatInfo->pIndex[ TD_SIXTH	]	, sixthtextureindex		)	)
		{
			return pMatInfo;
		}

		list.GetNext( pNode );
	}

	// ã�� Material�� �����.
	return NULL;
}

AcMaterialList::MaterialData *	AcMaterialList::GetEmptyMaterial( void )
{
	MaterialData *				pMatInfo;
	AuNode< MaterialData > *	pNode	;

	pNode = list.GetHeadNode();

	while( pNode )
	{
		pMatInfo = & pNode->GetData();
		if( pMatInfo->bDeleted )
		{
			pMatInfo->bDeleted = true; // ����Ѵٴ� üũ�� �־����.
			return pMatInfo;
		}

		list.GetNext( pNode );
	}

	// �� Material�� �����.

	// �� ��Ƽ������ �����Ͽ� ����ִ´�.

	//TRACE( "�� ��Ƽ������ �����Ͽ� ���忡 �߰��մϴ�\n" );

    RpMaterial *			material	;

    material	= RpMaterialCreate();

	ASSERT( NULL != material );

    RpMaterialSetTexture	( material , NULL );

	ASSERT( NULL != m_pGeometry );

	int	matIndex = _rpMaterialListAppendMaterial( & m_pGeometry->matList , material );

	RwSurfaceProperties		surfProp	;

	const BOOL bUseMaterialColor = FALSE;
	if( bUseMaterialColor )
	{
		surfProp.ambient	= 1.0f;
		surfProp.diffuse	= 1.0f;
		surfProp.specular	= 1.0f;
	}
	else
	{
		// ���� ����.
		surfProp.ambient	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_AMBIENT		;
		surfProp.diffuse	= 1.0f												;
		surfProp.specular	= ALEF_MATERIAL_DEFAULT_SURFACEPROPERTY_SPECULAR	;
	}

	RpMaterialSetSurfaceProperties(material, &surfProp);

	RwRGBA	rgb;
	rgb.alpha	= 255;
	rgb.blue	= 255;
	rgb.green	= 255;
	rgb.red		= 255;

	RpMaterialSetColor( material , &rgb );

	// ����Ʈ�� ����Ÿ �߰���.
	MaterialData			mdata		;

	mdata.bDeleted				= false						;
	mdata.pIndex[ TD_FIRST	]	= ALEF_TEXTURE_NO_TEXTURE	;
	mdata.pIndex[ TD_SECOND	]	= ALEF_TEXTURE_NO_TEXTURE	;
	mdata.pIndex[ TD_THIRD	]	= ALEF_TEXTURE_NO_TEXTURE	;
	mdata.pIndex[ TD_FOURTH	]	= ALEF_TEXTURE_NO_TEXTURE	;
	mdata.pIndex[ TD_FIFTH	]	= ALEF_TEXTURE_NO_TEXTURE	;
	mdata.pIndex[ TD_SIXTH	]	= ALEF_TEXTURE_NO_TEXTURE	;
	mdata.pMateterial			= material					;
	mdata.matindex				= matIndex					;

	list.AddTail( mdata );

	return &list.GetTail();
}

void	AcMaterialList::RemoveAll		(					)
{
	MaterialData * pMat;
	while( list.GetHeadNode() )
	{
		pMat = & list.GetHead();
		if( pMat->pMateterial )
		{
			RpMaterialDestroy( pMat->pMateterial );
		}

		list.RemoveHead();
	}
	
	m_pGeometry	= NULL;
}