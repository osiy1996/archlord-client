// AgcmMinimap.cpp: implementation of the AgcmMinimap class.
//
//////////////////////////////////////////////////////////////////////

#include <AgcmMinimap/AgcmMinimap.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmItem/AgcmItem.h>
#include <AgcmUIOption/AgcmUIOption.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcModule/AgcEngine.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FLOAT	__sRate	= 0.20f;

inline FLOAT	__GetDivisionStartX( INT32 nDivision , INT32 nPartIndex )
{
	INT32	lFirstSectorIndexX	= ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivision ) );
	INT32	lFirstSectorIndexZ	= ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision ) );

	if( nPartIndex % 2 == 0 )
		return GetSectorStartX( lFirstSectorIndexX );
	else
		return GetSectorStartX( lFirstSectorIndexX ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
}

inline FLOAT	__GetDivisionStartZ( INT32 nDivision , INT32 nPartIndex )
{
	INT32	lFirstSectorIndexZ	= ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivision ) );

	if( nPartIndex / 2 == 0 )
		return GetSectorStartZ( lFirstSectorIndexZ );
	else
		return GetSectorStartZ( lFirstSectorIndexZ ) + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH / 2.0f;
}


inline INT32 __GetMMPos( FLOAT fPos )
{
	return		( INT32 ) ( fPos /
		( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH ) *
		( ACUIMINIMAP_TEXTURE_SIZE << 1 ) );
}

inline INT32 __GetRaiderPos( FLOAT fPos )
{
	return		( INT32 ) ( fPos / 
		( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH ) *
		( ACUIRAIDER_RANGE_SIZE << 1 ) );
}

inline FLOAT __GetMMPosINV( INT32 nPos )
{
	return		( FLOAT ) ( nPos ) / ( ACUIMINIMAP_TEXTURE_SIZE << 1 ) * ( MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH );
}

inline RwTexture * __LoadTextureMacro( AgcmMinimap * pThis , TCHAR * pFolder , TCHAR * pStr )
{
	RwTexture * pTexture;
	int nRetryCount = 2;
	do
	{
		pTexture = pThis->m_pcsAgcmResourceLoader->LoadTexture
					( pStr , NULL , NULL , NULL , -1 , pFolder );

		if( pTexture ) return pTexture;
	}
	while( nRetryCount-- );

	return NULL;
}

#define LOADTEXTUREMACRO( filename )			__LoadTextureMacro( this , AGCMMAP_MINIMAP_FOLDER , filename )


AgcmMinimap::AgcmMinimap():m_bShowAllCharacter( false )
{
	SetModuleName("AgcmMinimap");

	InitMinimapTextureBuffer	();

	m_pcsAgcmUIManager2		= NULL;
	m_pcsAgpmParty			= NULL;
	m_pcsAgcmResourceLoader	= NULL;
	m_pcsAgcmFont			= NULL;
	m_pcsApmMap				= NULL;
	m_pcsAgpmCharacter		= NULL;

	m_pDirectionCursor		= NULL;
	m_pWorldMap				= NULL;
	m_pMemberPos			= NULL;
	m_pPcPos				= NULL;
	m_pNpcNormal			= NULL;	//��Ƽ�ɹ� ������.
	m_pNpcQuestNew	= NULL;	//��Ƽ�ɹ� ������.
	m_pNpcQuestIncomplete	= NULL;	//��Ƽ�ɹ� ������.

	m_pSiegeNormal			= NULL;
	m_pSiegeDestroied		= NULL;

	m_nWorldMap				= -1;

	ZeroMemory(&m_stSelfCharacterPos, sizeof(m_stSelfCharacterPos));
	
	m_pcsSelfCharacter		= NULL;
	m_pstUDPosition		= NULL;

	m_bIsOpenMiniMap	= TRUE;
	m_bIsOpenRaider		= TRUE;

	m_pclMinimapWindow1	= new AcUIMinimap	;
	m_pclMinimapWindow2	= new AcUIMinimap	;
	m_pclMinimapWindow3	= new AcUIMinimap	;
	m_pclWorldWindow	= new AcUIWorldmap	;
	m_pclRaiderWindow	= new AcUIRaider	;

	m_lEventMinimapForceClose	= 0;
	m_lEventMinimapYouCanOpen	= 0;

	m_bMurderer		= TRUE;
	m_bCriminal		= TRUE;
	m_bEnemyGuild		= TRUE;
}

AgcmMinimap::~AgcmMinimap()
{
	DEF_SAFEDELETE( m_pclMinimapWindow1	);
	DEF_SAFEDELETE( m_pclMinimapWindow2	);
	DEF_SAFEDELETE( m_pclMinimapWindow3	);
	DEF_SAFEDELETE( m_pclWorldWindow	);
	DEF_SAFEDELETE( m_pclRaiderWindow	);
}

// �̴ϸ� �ؽ��� ����.
void			AgcmMinimap::InitMinimapTextureBuffer	()
{
	for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
		m_pMinimapTextureBuffer[ i ].pTexture = NULL;
}

RwTexture	*	AgcmMinimap::GetMinimapDummyTexture		()
{
	RwTexture	* pTexture;

	// �̴ϸ� ������..
	char		strFilename[ 256 ];

	// ���� �̹��� �ϵ��ڵ�.
	wsprintf( strFilename , AGCMMAP_MINIMAP_FILE_FORMAT , 1722 , 0 + 'a' );

	pTexture	= LOADTEXTUREMACRO( strFilename );

	ASSERT( pTexture );

	return pTexture;
}

RwTexture	*	AgcmMinimap::GetMinimapTexture	( INT32 nDivisionIndex , INT32 nPartIndex )
{
	// �켱 �ִ��� �˻�..

	for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
	{
		if( m_pMinimapTextureBuffer[ i ].nIndex	== nDivisionIndex	&&
			m_pMinimapTextureBuffer[ i ].nPart	== nPartIndex		)
		{
			// �߰�!
			m_pMinimapTextureBuffer[ i ].uLastAccess	= GetTickCount();
			return m_pMinimapTextureBuffer[ i ].pTexture;
		}
	}

	// ������ ���Ͽ��� �о����..

	RwTexture	* pTexture;

	// �̴ϸ� ������..
	char		strFilename[ 256 ];
	wsprintf( strFilename , AGCMMAP_MINIMAP_FILE_FORMAT , nDivisionIndex , nPartIndex + 'a' );

	pTexture	= LOADTEXTUREMACRO( strFilename );

	if( NULL == pTexture )
	{
		// 2005/01/12
		// �ؽ��İ� ���°�� ���� �ý��ĸ� ����Ѵ�.
		// ����� �׳� �� �̹����� �ϵ��ڵ��ϴµ�..
		// �Ŀ� ������ �̹����� ���� �غ��ϵ��� �Ѵ�.
		
		pTexture	= GetMinimapDummyTexture();

		// �׷��� �ȳ����� ��¿�� ���� -_-;
	}

	{
		// �ؽ��İ� ��� �����Ѵ�.

		INT32	nOldIndex = 0;

		// Find Older One
		for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
		{
			//if( NULL == m_pMinimapTextureBuffer[ i ].pTexture	)
			//{
			//	nOldIndex = i;
			//	break;
			//}

			if( m_pMinimapTextureBuffer[ i ].uLastAccess < m_pMinimapTextureBuffer[ nOldIndex ].uLastAccess)
			{
				nOldIndex = i;
			}
		}

		// ����.;
		if( m_pMinimapTextureBuffer[ nOldIndex ].pTexture )
		{
			// �ؽ��� ����.
			RwTextureDestroy( m_pMinimapTextureBuffer[ nOldIndex ].pTexture );
			m_pMinimapTextureBuffer[ nOldIndex ].pTexture	= NULL	;
			m_pMinimapTextureBuffer[ nOldIndex ].nIndex	= -1	;
		}
		else
		{
			// do nothing.
		}
		
		if( pTexture )
		{
			RwTextureSetFilterMode( pTexture , rwFILTERNEAREST );
			RwTextureSetAddressing ( pTexture , rwTEXTUREADDRESSCLAMP );
		}

		// �����ſ� ������.
		// ���� �տ� ����.
		ASSERT( nOldIndex < MAX_MINIMAP_TEXTURE_BUFFER );
		
		m_pMinimapTextureBuffer[ nOldIndex ].pTexture		= pTexture		;
		m_pMinimapTextureBuffer[ nOldIndex ].nPart			= nPartIndex	;
		m_pMinimapTextureBuffer[ nOldIndex ].nIndex			= nDivisionIndex;
		m_pMinimapTextureBuffer[ nOldIndex ].uLastAccess	= GetTickCount();

		return m_pMinimapTextureBuffer[ nOldIndex ].pTexture;		
	}	
}

void			AgcmMinimap::FlushMinimapTexture	()
{
	for( int i = 0 ; i < MAX_MINIMAP_TEXTURE_BUFFER ; ++ i )
	{
		if( m_pMinimapTextureBuffer[ i ].pTexture )
		{
			RwTextureDestroy( m_pMinimapTextureBuffer[ i ].pTexture );
			m_pMinimapTextureBuffer[ i ].pTexture = NULL;
		}

		m_pMinimapTextureBuffer[ i ].nIndex	= -1;
	}
}

BOOL	AgcmMinimap::OnAddModule	()
{
	m_pcsAgcmUIManager2		= ( AgcmUIManager2		* )	GetModule( "AgcmUIManager2"		);
	m_pcsAgpmParty			= ( AgpmParty			* )	GetModule( "AgpmParty"			);
	m_pcsAgcmResourceLoader	= ( AgcmResourceLoader	* )	GetModule( "AgcmResourceLoader"	);
	m_pcsAgcmFont			= ( AgcmFont			* )	GetModule( "AgcmFont"			);
	m_pcsApmMap				= ( ApmMap				* )	GetModule( "ApmMap"				);
	m_pcsAgpmCharacter		= ( AgpmCharacter		* ) GetModule( "AgpmCharacter"		);
	

	ASSERT( NULL != m_pcsAgcmUIManager2		);
	ASSERT( NULL != m_pcsAgpmParty			);
	ASSERT( NULL != m_pcsAgcmResourceLoader	);
	ASSERT( NULL != m_pcsAgcmFont			);
	ASSERT( NULL != m_pcsApmMap				);
	ASSERT( NULL != m_pcsAgpmCharacter		);
	
	AgcmCharacter	*pAgcmCharacer = ( AgcmCharacter * ) GetModule( "AgcmCharacter" );
	ASSERT( NULL != pAgcmCharacer );


	if( pAgcmCharacer )
	{
		// �ɸ��� �̵� �ݹ� ����.
		VERIFY( pAgcmCharacer->SetCallbackSelfUpdatePosition( CBSelfCharacterPositionCallback , this ) );
		VERIFY( pAgcmCharacer->SetCallbackSetSelfCharacter	( CBSetSelfCharacter, this ) );

		// ���� ��ȭ�� üũ.
		VERIFY( pAgcmCharacer->SetCallbackSelfRegionChange	( CBBindingRegionChange , this ) );
	}
	else
	{
		return FALSE;
	}

	m_pclMinimapWindow1->SetMinimapModule( this );
	m_pclMinimapWindow2->SetMinimapModule( this );
	m_pclMinimapWindow3->SetMinimapModule( this );
	m_pclRaiderWindow->SetMinimapModule( this );

	m_pclMinimapWindow1->SetCharacterModule( m_pcsAgpmCharacter );
	m_pclMinimapWindow2->SetCharacterModule( m_pcsAgpmCharacter );
	m_pclMinimapWindow3->SetCharacterModule( m_pcsAgpmCharacter );
	m_pclRaiderWindow->SetCharacterModule( m_pcsAgpmCharacter );

	if (m_pcsAgcmUIManager2)
	{
		// ��Ʈ�� 1
		if (!m_pcsAgcmUIManager2->AddCustomControl("MiniMap1", m_pclMinimapWindow1))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "MiniMap_Pos1", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("MiniMap_Pos1", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;

		// ��Ʈ�� 2
		if (!m_pcsAgcmUIManager2->AddCustomControl("MiniMap2", m_pclMinimapWindow2))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "MiniMap_Pos2", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("MiniMap_Pos2", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;

		// ��Ʈ�� 3
		if (!m_pcsAgcmUIManager2->AddCustomControl("MiniMap3", m_pclMinimapWindow3))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "MiniMap_Pos3", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("MiniMap_Pos3", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "MiniMap_ToggleUI", CBToggleMiniMapUI, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "WorldMap_ToggleUI", CBToggleWorldMapUI, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Raider_ToggleUI", CBToggleRaiderUI, 0))
			return FALSE;

		// Raider ��Ʈ��
		if( !m_pcsAgcmUIManager2->AddCustomControl( "Raider" , m_pclRaiderWindow ) )
			return FALSE;

		if( !m_pcsAgcmUIManager2->AddDisplay(this, "Raider_Pos" , 0 , CBDisplayPos , AGCDUI_USERDATA_TYPE_POS ))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData( "Raider_Pos" , &m_stSelfCharacterPos , sizeof(AuPOS) , 1 , AGCDUI_USERDATA_TYPE_POS );
		if( !m_pstUDPosition )
			return FALSE;

		
	}

	m_pclWorldWindow->SetMinimapModule( this );
	m_pclWorldWindow->SetCharacterModule( m_pcsAgpmCharacter );

	if (m_pcsAgcmUIManager2)
	{
		if (!m_pcsAgcmUIManager2->AddCustomControl("WorldMap", m_pclWorldWindow))
			return FALSE;

		/*
		if (!m_pcsAgcmUIManager2->AddDisplay(this, "WorldMap_Pos", 0, CBDisplayPos, AGCDUI_USERDATA_TYPE_POS))
			return FALSE;

		m_pstUDPosition = m_pcsAgcmUIManager2->AddUserData("World_Pos", &m_stSelfCharacterPos, sizeof(AuPOS), 1, AGCDUI_USERDATA_TYPE_POS);
		if (!m_pstUDPosition)
			return FALSE;
		*/
	}


	if( m_pcsAgcmUIManager2 )
	{
//		m_lEventPlusButton = m_pcsAgcmUIManager2->AddEvent("MiniMap_PlusButton");
//		if (m_lEventPlusButton < 0)
//			return FALSE;
//		m_lEventMinusButton = m_pcsAgcmUIManager2->AddEvent("MiniMap_MinusButton");
//		if (m_lEventMinusButton < 0)
//			return FALSE;

		m_lEventOpenMiniMapUI = m_pcsAgcmUIManager2->AddEvent("MiniMap_OpenUI");
		if (m_lEventOpenMiniMapUI < 0)
			return FALSE;
		m_lEventCloseMiniMapUI = m_pcsAgcmUIManager2->AddEvent("MiniMap_CloseUI");
		if (m_lEventCloseMiniMapUI < 0)
			return FALSE;

		m_lEventOpenRaiderUI	=	m_pcsAgcmUIManager2->AddEvent( "Raider_OpenUI" );
		if( m_lEventOpenRaiderUI < 0 )
			return FALSE;

		m_lEventCloseRaiderUI	=	m_pcsAgcmUIManager2->AddEvent( "Raider_CloseUI" );
		if( m_lEventCloseRaiderUI < 0 )
			return FALSE;

//		if (!m_pcsAgcmUIManager2->AddFunction(this, "MiniMap_SetPlusButton", CBPlusButton, 0))
//			return FALSE;
//		if (!m_pcsAgcmUIManager2->AddFunction(this, "MiniMap_SetMinusButton", CBMinusButton, 0))
//			return FALSE;

		m_lEventMinimapForceClose = m_pcsAgcmUIManager2->AddEvent("MiniMap_ForcedClose");
		if (m_lEventMinimapForceClose < 0)
			return FALSE;
		m_lEventMinimapYouCanOpen = m_pcsAgcmUIManager2->AddEvent("MiniMap_YouCanOpenUI");
		if (m_lEventMinimapYouCanOpen < 0)
			return FALSE;

		m_lEventOpenWorldMapUI = m_pcsAgcmUIManager2->AddEvent("WorldMap_OpenUI");
		if (m_lEventOpenWorldMapUI < 0)
			return FALSE;

		// ����� �������� �����������.
		m_lEventWorldMapNoItem = m_pcsAgcmUIManager2->AddEvent("WorldMap_NoItem");
		if (m_lEventWorldMapNoItem < 0)
			return FALSE;

		m_lEventCloseWorldMapUI = m_pcsAgcmUIManager2->AddEvent("WorldMap_CloseUI");
		if (m_lEventCloseWorldMapUI < 0)
			return FALSE;

		if( m_pclWorldWindow )
		{
			m_pclWorldWindow->OnAddEvent( m_pcsAgcmUIManager2 );
			m_pclWorldWindow->OnAddFunction( m_pcsAgcmUIManager2 );
			m_pclWorldWindow->OnAddDisplay( m_pcsAgcmUIManager2 );
			m_pclWorldWindow->OnAddUserData( m_pcsAgcmUIManager2 );
		}
	}

	{
		AgcmItem	* pAgcmItem = static_cast< AgcmItem * > ( GetModule( "AgcmItem" ) );
		ASSERT( NULL != pAgcmItem );

		if( pAgcmItem )
		{
			VERIFY( pAgcmItem->SetCallbackUseMapItem( CBUseMapItem , this ) );
		}
		else
		{
			return FALSE;
		}
	}

	// ������ (2005-07-21 ���� 12:19:29) : 
	// ����� ���� �о����.
	{
		ApmMap	* pcsApmMap = m_pcsApmMap;
		// ����� ����
		#ifdef _BIN_EXEC_
		if( pcsApmMap->LoadWorldMap( "Ini\\" WORLDMAPTEMPLATE , FALSE ) )
		#else
		if( pcsApmMap->LoadWorldMap( "Ini\\" WORLDMAPTEMPLATE , TRUE ) )
		#endif // USE_MFC
		{
			// ���� ���� �߰�.
			ApmMap::WorldMap	* pWorldMap;
			for( int i = 0 ; i < ( int ) pcsApmMap->m_arrayWorldMap.size() ; i ++ )
			{
				pWorldMap = &pcsApmMap->m_arrayWorldMap[ i ];

				m_pclWorldWindow->SetWorldMapInfo( pWorldMap );
			}

			/// �ۺ����� ����.
			pcsApmMap->RemoveAllWorldMap();

			m_pclWorldWindow->m_bThereIsNoWorldMapTemplate = FALSE;
		}
		else
		{
			// ������ ��¼��..
			ASSERT( "����� ���ø��� �����ϴ�." );
			// �ȳ����°��� �� ~ >_<;;;

			// �����Ŷ� ������ �Ѵ�.
			// �ϵ��ڵ���~
			m_pclWorldWindow->SetWorldMapInfo( NULL ); // ����

			m_pclWorldWindow->m_bThereIsNoWorldMapTemplate = TRUE;
		}
	}
	/*
	// ����׿�..
	AuPOS pos;
	pos.x	= -412041.0f;
	pos.z	= 148215.0f;
	pos.y	= 0.0f;
	AddPoint( 0 , "������" , &pos );

	pos.x	= -396807.0f;
	pos.z	= 154223.0f;
	pos.y	= 0.0f;
	AddPoint( 0 , "ũ����" , &pos );
	*/

	return TRUE;
}

BOOL	AgcmMinimap::OnInit()
{
	m_pcsAgpmWantedCriminal	= ( AgpmWantedCriminal	* ) GetModule( "AgpmWantedCriminal" );
	m_pcsAgpmPVP			= ( AgpmPvP	*	)			GetModule( "AgpmPvP" );
	m_pcsAgcmCharacter		= ( AgcmCharacter* )		GetModule( "AgcmCharacter" );
	m_pcsAgpmGuild			= ( AgpmGuild*		)		GetModule( "AgpmGuild" );
	m_pcsAgpmBattleGround	= ( AgpmBattleGround* )		GetModule( "AgpmBattleGround" );

	return TRUE;
}

BOOL	AgcmMinimap::OnDestroy		()
{
	FlushMinimapTexture();

	if( m_pDirectionCursor	)
	{
		RwTextureDestroy( m_pDirectionCursor	);
		m_pDirectionCursor = NULL;
	}

	if( m_pWorldMap			)
	{
		RwTextureDestroy( m_pWorldMap			);
		m_pWorldMap = NULL;
	}

	if( m_pMemberPos		)
	{
		RwTextureDestroy( m_pMemberPos			);
		m_pMemberPos = NULL;
	}

	if( m_pPcPos			)
	{
		RwTextureDestroy( m_pPcPos				);
		m_pPcPos = NULL;
	}

	if( m_pNpcNormal		)
	{
		RwTextureDestroy( m_pNpcNormal			);
		m_pNpcNormal = NULL;
	}

	if( m_pNpcQuestNew		)
	{
		RwTextureDestroy( m_pNpcQuestNew		);
		m_pNpcQuestNew = NULL;
	}

	if( m_pNpcQuestIncomplete)
	{
		RwTextureDestroy( m_pNpcQuestIncomplete	);
		m_pNpcQuestIncomplete = NULL;
	}

	if( m_pSiegeNormal		)
	{
		RwTextureDestroy( m_pSiegeNormal		);
		m_pSiegeNormal = NULL;
	}

	if( m_pSiegeDestroied	)
	{
		RwTextureDestroy( m_pSiegeDestroied		);
		m_pSiegeDestroied = NULL;
	}

	return TRUE;
}

BOOL	AgcmMinimap::CBSelfCharacterPositionCallback(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmMinimap::CBSelfCharacterPositionCallback");

	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
	AgpdCharacter	*	pcsAgpdCharacter	= (AgpdCharacter *) pData					;
	//AgcdCharacter	*	pcsAgcdCharacter	= pThis->GetCharacterData(pcsAgpdCharacter)	;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsAgpdCharacter );
	if( NULL == pThis ) return TRUE;

	if (pcsAgpdCharacter)
	{
		pThis->m_stSelfCharacterPos	= pcsAgpdCharacter->m_stPos;
	}
	// �̵� ���� ������Ʈ .
	pThis->m_pstUDPosition->m_stUserData.m_pvData	= &pThis->m_stSelfCharacterPos;

	if( pThis->m_pcsAgcmUIManager2 )
	{
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDPosition)	;
	}

	pThis->m_pcsSelfCharacter	= pcsAgpdCharacter;
	//pThis->m_pclMinimapWindow.ProcessPositionChange( pcsAgpdCharacter->m_stPos.x , pcsAgpdCharacter->m_stPos.z );

	pThis->m_pclMinimapWindow1->PositionUpdate();
	pThis->m_pclMinimapWindow2->PositionUpdate();
	pThis->m_pclMinimapWindow3->PositionUpdate();

	pThis->m_pclRaiderWindow->RaiderPositionUpdate();

	return TRUE;
}

BOOL AgcmMinimap::CBBindingRegionChange(PVOID	pData,PVOID	pClass,PVOID	pCustData)
{
	AgcmMinimap*	pThis		= (AgcmMinimap*) pClass;
	AgpdCharacter*	pdCharacter = (AgpdCharacter*)	pData;
	INT16 nPrevRegionIndex = pCustData ? *(INT16*)pCustData : -1;	// NULL �� �ö��� ADD_CHARACTER_TO_MAP ���� �Ҹ�����.

	// ���ø� ����..
	ApmMap::RegionTemplate*	pTemplate		= pThis->m_pcsApmMap->GetTemplate( pdCharacter->m_nBindingRegionIndex );
	ApmMap::RegionTemplate*	pTemplatePrev	= pThis->m_pcsApmMap->GetTemplate( nPrevRegionIndex );

	if( pTemplate && pTemplatePrev )
	{
		if( pTemplate->ti.stType.bDisableMinimap )
		{
			// �̴ϸ� ��� �ȉ�.

			if(pThis->m_bIsOpenMiniMap)
			{
				// �������ϱ� ����..
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseMiniMapUI);
				pThis->m_bIsOpenMiniMap	= FALSE;

				// ���� �ݱ� �޽��� ǥ��.
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinimapForceClose);
			}
		}
		else
		{
			// �̴ϸ� ����� �����Ѱ�..
			if( pThis->m_bIsOpenMiniMap )
			{
				// do nothing.
			}
			else
			{
				if( pTemplatePrev->ti.stType.bDisableMinimap )
				{
					// ������ �ȵƴµ�
					// ������ ������ ����.

					// ���� ������ �˷���.
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinimapYouCanOpen);
				}
			}
		}

		if( pTemplate->nWorldMapIndex != pTemplatePrev->nWorldMapIndex )
		{
			// ����� ����
			if( pThis->m_pclWorldWindow->m_bIsOpenWorldMap )
			{
				// �ݱ�..
				//pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
				//pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );

				if( pThis->m_pclWorldWindow->SetWorldMapIndex( pTemplate->nWorldMapIndex ) )
				{
					AgpmItem	* pAgpmItem = static_cast< AgpmItem * > ( pThis->GetModule( "AgpmItem" ) );

					ASSERT( NULL != pAgpmItem );

					// do nothing..

					AcUIWorldmap::stWorldMap * pWorldmap = pThis->m_pclWorldWindow->GetCurrentWorldMap();
					ASSERT( NULL != pWorldmap );

					// ������ �ִ��� �˻�..
					if( pWorldmap->nMapItemID == -1 || 				
						pAgpmItem->GetInventoryItemByTID( pThis->m_pcsSelfCharacter , pWorldmap->nMapItemID ) )
					{
						// do nothing 
					}
					else
					{
						// �������� ����.
						// ������ (2005-07-21 ���� 5:04:44) : 
						// ���⼭ return FALSE�� �ϴ��� UI�� ������ �ʴ´�.

						pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
						pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );
						pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
					}
				}
				else
				{
					// ��Ī�Ŵ� ���� ����.. ������ �ȵŴ� ����.
					pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
					pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );
					pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
				}
			}
		}
	}

    return	TRUE;
}

BOOL	AgcmMinimap::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmMinimap		*	pThis				= (AgcmMinimap *)	pClass					;
	AgpdCharacter	*	pcsCharacter		= (AgpdCharacter *)	pData					;
	
	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsCharacter );
	if( NULL == pThis ) return TRUE;

	if (pcsCharacter)
	{
		pThis->m_stSelfCharacterPos	= pcsCharacter->m_stPos;
	}

	if( pThis->m_pstUDPosition )
	{
		pThis->m_pstUDPosition->m_stUserData.m_pvData	= &pThis->m_stSelfCharacterPos;
	}

	if( pThis->m_pcsAgcmUIManager2 )
	{
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDPosition)	;
	}

	pThis->m_pcsSelfCharacter	= pcsCharacter;

	return TRUE;
}

RwTexture	*	AgcmMinimap::GetCursorTexture			()
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;

	if( m_pDirectionCursor ) return m_pDirectionCursor;
	{ 
		m_pDirectionCursor	= LOADTEXTUREMACRO( AGCMMAP_MINIMAP_FILE_CURSOR );

		// ������ (2004-05-26 ���� 4:07:14) : Ŀ���� ���Ͼ� �Ⱦ�.
		//RwTextureSetFilterMode( m_pDirectionCursor , rwFILTERNEAREST );
		if( m_pDirectionCursor )
			RwTextureSetAddressing ( m_pDirectionCursor , rwTEXTUREADDRESSCLAMP );
		
		return m_pDirectionCursor;
	}
}

RwTexture	*	AgcmMinimap::GetWorldMapTexture			( int nIndex )
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;

	if( nIndex < 0 ) return NULL;

	// ������ (2004-07-18 ���� 9:27:50) : �̹� �ε��Ǿ� ������..
	if( m_nWorldMap == nIndex && m_pWorldMap ) return m_pWorldMap;

	if( m_pWorldMap )
	{
		// ������ (2004-07-18 ���� 9:29:36) : û��~
		RwTextureDestroy( m_pWorldMap );
		m_pWorldMap	= NULL;
	}

	char	str[ 1024 ];
	wsprintf( str , AGCMMAP_WORLDMAP_TEST , nIndex );

	AgcmUIOption * pcsAgcmUIOption = ( AgcmUIOption * ) this->GetModule( "AgcmUIOption" );
	ASSERT( NULL != pcsAgcmUIOption );

	// m_pWorldMap	= LOADTEXTUREMACRO( str );

	UINT32 uUIType = 0;
	if( pcsAgcmUIOption )
	{
		uUIType = pcsAgcmUIOption->m_ulUISkinType;
	}

	const int nStringMax = 256;
	char	pFolder[ nStringMax ];


	switch( uUIType )
	{
	default:
	case 0:
		{
			strncpy( pFolder , AGCMMAP_WORLDMAP_FOLDER_DEFAULT , nStringMax );
		}
		break;

	/*
		// 1�̻��� ��쿡�� �ϵ��ڵ带 Ǯ���־����.
	case 1:
		{
			wsprintf( pFolder , AGCMMAP_WORLDMAP_FOLDER , uUIType );
		}
		break;
	*/
	}

	m_pWorldMap	= __LoadTextureMacro( this , pFolder , str );

	if( m_pWorldMap )
	{
		RwTextureSetFilterMode( m_pWorldMap , rwFILTERNEAREST );
		RwTextureSetAddressing ( m_pWorldMap , rwTEXTUREADDRESSCLAMP );
		m_nWorldMap = nIndex;
	}
	else
	{
		TRACE( "AgcmMinimap::GetWorldMapTexture = '%s' ������ �������� �ʾƿ�\n" , str );
	}

	return m_pWorldMap;
}

RwTexture	*	AgcmMinimap::GetMemberPosTexture			()
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;

	if( m_pMemberPos ) return m_pMemberPos;
	{
		m_pMemberPos	= LOADTEXTUREMACRO( AGCMMAP_MEMBERPOSITION );

		// RwTextureSetFilterMode( m_pMemberPos , rwFILTERNEAREST );
		if( m_pMemberPos )
			RwTextureSetAddressing ( m_pMemberPos , rwTEXTUREADDRESSCLAMP );
		
		return m_pMemberPos;
	}
}

#define TEXTURE_LOAD_MODULE( TEXTURE , FILE__NAME )									\
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;						\
	if( TEXTURE ) return TEXTURE;													\
	{																				\
		TEXTURE	=																	\
			LOADTEXTUREMACRO( FILE__NAME	);										\
		if( TEXTURE )																\
			RwTextureSetAddressing ( TEXTURE , rwTEXTUREADDRESSCLAMP );				\
		return TEXTURE;																\
	}

RwTexture	*	AgcmMinimap::GetPcPosTexture				()
{
	if( NULL == this->m_pcsAgcmResourceLoader ) return NULL;
	
	if( m_pPcPos ) return m_pPcPos;
	{
		m_pPcPos	= LOADTEXTUREMACRO( AGCMMAP_PCPOSITION );

		// RwTextureSetFilterMode( m_pPcPos , rwFILTERNEAREST );
		if( m_pPcPos )
			RwTextureSetAddressing ( m_pPcPos , rwTEXTUREADDRESSCLAMP );

		return m_pPcPos;
	}
}

RwTexture	*		AgcmMinimap::GetNpcNormalTexture			()
{
	TEXTURE_LOAD_MODULE( m_pNpcNormal , AGCMMAP_NPC_NORMAL )
}

RwTexture	*		AgcmMinimap::GetNpcQuestNewTexture	()
{
	TEXTURE_LOAD_MODULE( m_pNpcQuestNew , AGCMMAP_NPC_QUESTNEW )
}
RwTexture	*		AgcmMinimap::GetNpcQuestIncompleteTexture	()
{
	TEXTURE_LOAD_MODULE( m_pNpcQuestIncomplete , AGCMMAP_NPC_QUESTINCOMPLETE )
}

RwTexture	*		AgcmMinimap::GetTextureSiegeNormal		()
{
	TEXTURE_LOAD_MODULE( m_pSiegeNormal , AGCMMAP_SIEGE_NORMAL )
}
RwTexture	*		AgcmMinimap::GetTextureSiegeDestroied	()
{
	TEXTURE_LOAD_MODULE( m_pSiegeDestroied , AGCMMAP_SIEGE_DESTROIED )
}

BOOL	AgcmMinimap::CBDisplayPos(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
	AuPOS *				pos					= (AuPOS *) pData							;

	if (pos)
		sprintf(szDisplay, "%d,%d", (INT32) pos->x, (INT32) pos->z);
	else
		szDisplay[0] = '\0';

	return TRUE;
}

//BOOL	AgcmMinimap::CBPlusButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
//
//	if( NULL == pThis ) return FALSE;
//	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
//
//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventPlusButton);
//}
//
//BOOL	AgcmMinimap::CBMinusButton(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//{
//	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;
//
//	if( NULL == pThis ) return FALSE;
//	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
//
//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinusButton);
//}

BOOL	AgcmMinimap::CBToggleRaiderUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{

	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;

	if( NULL == pThis ) return FALSE;
	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
	if( NULL == pThis->m_pcsSelfCharacter ) return FALSE;

	if (pThis->m_bIsOpenRaider)
	{
		// �������ϱ� ����..
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseRaiderUI);
		pThis->m_bIsOpenRaider	= FALSE;
	}
	else
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenRaiderUI);
		pThis->m_bIsOpenRaider	= TRUE;
	}

	return TRUE;
}


BOOL	AgcmMinimap::CBToggleMiniMapUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	// �̴ϸ� ��� ��ư�� ���������� ȣ���
	// �� . 'm'Ű�� ������ ����� �Ҹ�..
	// ���⼭ UI�� �Ѱ� ���� ó����.

	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;

	if( NULL == pThis ) return FALSE;
	if( NULL == pThis->m_pcsAgcmUIManager2 ) return FALSE;
	if( NULL == pThis->m_pcsSelfCharacter ) return FALSE;
	
	if (pThis->m_bIsOpenMiniMap)
	{
		// �������ϱ� ����..
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseMiniMapUI);
		pThis->m_bIsOpenMiniMap	= FALSE;
	}
	else
	{
		// ���ø� ����..
		ApmMap::RegionTemplate*	pTemplate = pThis->m_pcsApmMap->GetTemplate( pThis->m_pcsSelfCharacter->m_nBindingRegionIndex );

		if( pTemplate )
		{
			if( pTemplate->ti.stType.bDisableMinimap )
			{
				// �̴ϸ� ��� �ȉ�.

				// do nothing..

				// �̴ϸ� �������� �޽��� ǥ��.
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMinimapForceClose);
			}
			else
			{
				// ���������ϱ� �Ѱ�..
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenMiniMapUI);
				pThis->m_bIsOpenMiniMap	= TRUE;
			}
		}
	}

	return TRUE;
}

BOOL	AgcmMinimap::CBToggleWorldMapUI				(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmMinimap		*	pThis				= (AgcmMinimap *) pClass					;

	if( pThis->m_pclWorldWindow->m_bIsOpenWorldMap )
	{
		// �ݱ�..
		pThis->m_pclWorldWindow->m_bIsOpenWorldMap = FALSE;
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseWorldMapUI );
		return TRUE;
	}
	else
	{
		// �����ε�...
		// ���⼭ ������ üũ..
		if( pThis->m_pclWorldWindow->SetWorldMap( &pThis->m_stSelfCharacterPos ) )
		{
			AgpmItem	* pAgpmItem = static_cast< AgpmItem * > ( pThis->GetModule( "AgpmItem" ) );

			ASSERT( NULL != pAgpmItem );

			// do nothing..

			AcUIWorldmap::stWorldMap * pWorldmap = pThis->m_pclWorldWindow->GetCurrentWorldMap();
			ASSERT( NULL != pWorldmap );

			// ������ �ִ��� �˻�..
			if( pWorldmap->nMapItemID == -1 || 				
				pAgpmItem->GetInventoryItemByTID( pThis->m_pcsSelfCharacter , pWorldmap->nMapItemID ) )
			{
				// �������� �����Ƿ� UIǥ��
				pThis->m_pclWorldWindow->m_bIsOpenWorldMap = TRUE;
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenWorldMapUI );
				pThis->m_pclWorldWindow->OnGetControls( pThis->m_pcsAgcmUIManager2 );
				pThis->m_pclWorldWindow->OnUpdateWorldMapAlpha();
				return TRUE;
			}
			else
			{
				// �������� ����.
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
				return FALSE;
			}
		}
		else
		{
			// ��Ī�Ŵ� ���� ����.. ������ �ȵŴ� ����.
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventWorldMapNoItem );
			return FALSE;
		}
	}
	}

INT32		AgcmMinimap::GetDisabledIndex()
{
	MPInfo *	pArray = GetMPArray();
	INT32		nCount = GetMPCount();

	for( int i = 0 ; i < nCount ; i ++ )
	{
		if( pArray[ i ].bDisabled ) return i ;
	}

	return (-1); // �����..
}

INT32 				AgcmMinimap::GetPoint		( char * strText )
{
	if( !strText )
		return (-1);

	// �̹� �ִ� ������ �˻�
	AgcmMinimap::MPInfo *	pArray = GetMPArray();

	for( int i = 0 ; i < GetMPCount() ; i ++ )
	{
		if( pArray[ i ].strText &&
			strcmp(strText, pArray[ i ].strText) == 0 )
		{
			return i;
		}
	}

	return (-1);
}

INT32				AgcmMinimap::AddPoint		( INT32 nType , char * strText , AuPOS * pPos , INT32 nControlGroupID )
{
	// �ִ� ���� üũ..

	MPInfo	newInfo;

	newInfo.nType	= nType;
	newInfo.pos		= *pPos;
	strncpy( newInfo.strText , strText , AGCMMAP_MINIMAP_POINT_TEXT_LENGTH );
	newInfo.nControlGroupID	= nControlGroupID;

	// �̹� �ִ� ������ �˻�
	// �̸��� ���°͵� �����Ƿ� �˻����..
	/*
	INT32	nExistIndex	= GetPoint(strText);
	if( nExistIndex != -1 )
	{
		MPInfo * pExistInfo = GetMPInfo( nExistIndex );
		*pExistInfo = newInfo;
		return nExistIndex;
	}
	*/

	// �ϴ� ��� �ִ��� �˻�..

	INT32	nDisabled = GetDisabledIndex();
	if( nDisabled != -1 )
	{
		MPInfo * pInfo = GetMPInfo( nDisabled );
		*pInfo = newInfo;
		return nDisabled;
	}
	else
	{
        m_arrayMPInfo.push_back( newInfo );
		return GetMPCount() - 1;
	}
}

BOOL				AgcmMinimap::RemovePoint		( INT32 nIndex )
{
	ASSERT( nIndex < GetMPCount() );
	if( nIndex >= GetMPCount() ) return FALSE;

	MPInfo * pInfo = GetMPInfo( nIndex );
	pInfo->bDisabled = TRUE;
	return TRUE;
}

INT32				AgcmMinimap::RemovePointGroup		( INT32 nControlGroupID )
{
	// �Ѱ����� �������� ��ω� ��� , ������� ID�� ����ؼ�
	// �Ѳ����� �����ϰ��Ѵ�.
	if( nControlGroupID == -1 ) return FALSE; // �ӽö�!

	int nCount = 0;

	MPInfo * pArray = GetMPArray();
	for ( int i = 0 ; i < GetMPCount() ; i ++ )
	{
		if( pArray[ i ].nControlGroupID == nControlGroupID )
		{
			pArray[ i ].bDisabled = TRUE;
			nCount ++;
		}
	}

	return nCount;
}

AgcmMinimap::MPInfo *	AgcmMinimap::GetPointInfo		( INT32 nIndex )
{
	// �ɱ� �����ؼ� �߰�..
	// �߸��Ȱ� ���� ���� ������ Validation Check�� (Parn)
	if(  nIndex < 0 || nIndex >= GetMPCount() ) return NULL;

	MPInfo * pInfo = GetMPInfo( nIndex );
	if( pInfo->bDisabled ) return NULL;
	else return pInfo;
}

BOOL				AgcmMinimap::UpdatePointPosition( INT32 nIndex , AuPOS * pPos )
{
	MPInfo * pInfo = GetPointInfo( nIndex );
	if( pInfo )
	{
		pInfo->pos = * pPos;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL	AgcmMinimap::CBUseMapItem(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmMinimap	* pThis		= static_cast< AgcmMinimap * > ( pClass );
	AgpdItem	* pcsItem	= static_cast< AgpdItem * > ( pData );

	ASSERT( NULL != pThis );
	ASSERT( NULL != pcsItem );
	ASSERT( NULL != pcsItem->m_pcsItemTemplate );
	if( NULL == pThis ) return FALSE;
	if( NULL == pcsItem ) return FALSE;
	if( NULL == pcsItem->m_pcsItemTemplate ) return FALSE;

	{
		// ���� �� ������..
		// �����ۿ� ����� ���� Ȯ����.
		if( pThis->m_pclWorldWindow->SetWorldMapByItem( pcsItem->m_pcsItemTemplate->m_lID ) )
		{
			// ����� UI Open
			if( pThis->m_pclWorldWindow->m_bIsOpenWorldMap )
			{
				// do nothing
			}
			else
			{
				// �ȶ� ������츸 ����.
				pThis->m_pclWorldWindow->m_bIsOpenWorldMap = TRUE;
				pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenWorldMapUI );		
			}
		}
		else
		{
			// ��Ī�Ŵ� �������� ����!?
			// ������ �ȵŴ� ����
			ASSERT( !"��Ī�Ŵ� ���� ����!?" );
		}
	}

	return TRUE;
}

LuaGlue	LG_TDC( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	bUse		= ( INT32 ) pLua->GetNumberArgument( 1 , 1 );

	AgcmMinimap * pAgcmMinimap = ( AgcmMinimap * ) g_pEngine->GetModule( "AgcmMinimap" );

	pAgcmMinimap->m_bShowAllCharacter = bUse ? true : false;

	LUA_RETURN( TRUE );
}


void AgcmMinimap::OnLuaInitialize( AuLua * pLua )
{
	luaDef MVLuaGlue[] = 
	{
		{"TDC"		,	LG_TDC	},
		{NULL				,	NULL			},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}
