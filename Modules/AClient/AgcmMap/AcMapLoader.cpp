// AcMapLoader.cpp: implementation of the AcMapLoader class.
//
//////////////////////////////////////////////////////////////////////

#include <ApBase/ApBase.h>

#include <process.h>

#include "AcMapLoader.h"
#include <AgcmMap/AgcmMap.h>

#include <ApMemoryTracker/ApMemoryTracker.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcMapLoader::AcMapLoader()
{
	m_bEnd		= FALSE		;

	//m_csMutex.Init();

	m_nUnpackCount	= 0		;
}

AcMapLoader::~AcMapLoader()
{
	FlushUnpackManager();
}

/*
BOOL	AcMapLoader::StartThread()
{
	m_hThread = _beginthread(Process, 0, this);
	if (m_hThread == -1)
		return FALSE;

	return TRUE;
}

BOOL	AcMapLoader::StopThread()
{
	m_bEnd	= TRUE	;

//	WaitForSingleObject(m_hThread, 2000);

	return TRUE;
}

VOID	AcMapLoader::Process(PVOID pvArgs)
{
	AcMapLoader *	pThis = (AcMapLoader *) pvArgs;

	while (!pThis->m_bEnd)
	{
		pThis->m_csMutex.WLock();

		if (!pThis->IsEmpty())
		{
			pThis->m_csMutex.Release();

			pThis->OnIdleLoad();
		}
		else
		{
			pThis->m_csMutex.Release();

			Sleep(500);
		}
	}

	_endthread();
}
*/

BOOL	AcMapLoader::IsEmpty()
{
	if ( m_listSectorQueue.IsEmpty() )
		return TRUE;

	return FALSE;
}

BOOL	AcMapLoader::OnIdleLoad()
{
	PROFILE("AcMapLoader::OnIdleLoad");

	if( m_listSectorQueue.IsEmpty() ) return TRUE; // Queue Empty;
	
	LoadBox *		pLoadBox	= &m_listSectorQueue.GetHead()	;

	ASSERT( NULL != AGCMMAP_THIS );
	if( NULL == pLoadBox->pSector )
	{
		// TRACE( "AcMapLoader::OnIdleLoad ���� �����Ͱ� ���̿���\n" );
		m_listSectorQueue.RemoveHead();

		// Reculsive Call
		return OnIdleLoad();
	}
	else
	{
		TRACE("Get Load Queue ( Waiting Queue %d ) : %x\n", m_listSectorQueue.GetCount() , pLoadBox->pSector->GetArrayIndexDWORD() );

		switch( pLoadBox->nDetail )
		{
		case SECTOR_LOWDETAIL	:	AGCMMAP_THIS->OnLoadRough	( pLoadBox->pSector );	break;
		case SECTOR_HIGHDETAIL	:	AGCMMAP_THIS->OnLoadDetail	( pLoadBox->pSector );	break;
		default:
			// ���� ���� �ȵŴµ�..;;;
			break;
		}
				
		// ������ ������ �����Ϸ� ��������..
		// AGCMMAP_THIS->SetCurrentDetail( pLoadBox->pSector , pLoadBox->pSector->GetCurrentDetail() , TRUE );
	}

	m_listSectorQueue.RemoveHead();

	return TRUE;
}

BOOL	AcMapLoader::LoadDWSector( ApWorldSector * pvSector , INT32 nDetail )
{
	if( ! AGCMMAP_THIS->m_pcsApmMap->IsAutoLoadData() )
	{
		AGCMMAP_THIS->SetCurrentDetail( pvSector , nDetail );
		return TRUE;
	}
	
	//m_csMutex.WLock();

	AgcmMap *	pcsAgcmMap = AGCMMAP_THIS;

	if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , nDetail )	||
		DoExistSectorInfo			( pvSector , nDetail )	)	return TRUE;
		// �̹� �ε��Ǿ��ְų� , �ε����̴�.

	// �ɸ����� ��ġ�� ��..
	ApWorldSector *	pSector = AGCMMAP_THIS->m_pcsApmMap->GetSector(AGCMMAP_THIS->m_posCurrentAvatarPosition);
	
	if (pSector == pvSector)
	{
		// �ɸ��Ͱ� �־���ϴ� ���� ť�� ���� �ʰ� �ٷ� �ε���..
		switch( nDetail )
		{
		case SECTOR_LOWDETAIL	:	AGCMMAP_THIS->OnLoadRough	( pvSector );	break;
		case SECTOR_HIGHDETAIL	:	AGCMMAP_THIS->OnLoadDetail	( pvSector );	break;
		default:
			// ���� ���� �ȵŴµ�..;;;
			ASSERT( !"Unreachable code" );
			break;
		}
		
		return TRUE;
	}
	
	if( nDetail < 0 )
	{
		// ���δ� �ε�..
		if( FALSE == AddLoadSectorInfo( pvSector , SECTOR_LOWDETAIL		) )	return FALSE;
		if( FALSE == AddLoadSectorInfo( pvSector , SECTOR_HIGHDETAIL	) )	return FALSE;
	}
	else if( nDetail < SECTOR_DETAILDEPTH && nDetail >= nDetail)
	{
		if( FALSE == AddLoadSectorInfo( pvSector , nDetail				) )	return FALSE;
	}
	else
	{
		ASSERT( !"������ �̻�!" );
	}

	//m_csMutex.Release();

	return TRUE;
}

BOOL	AcMapLoader::AddLoadSectorInfo( ApWorldSector * pSector, INT32 nDetail )
{
	if ( m_listSectorQueue.GetCount() >= ACMAPLOADER_MAX_QUEUE )
	{
		//m_csMutex.Release();

		TRACE("Load Queue Full!!!\n");
		return FALSE;
	}
	
	// �ߺ� �ž����� ����..
	if( DoExistSectorInfo( pSector , nDetail ) ) return FALSE;

	// �ߺ����������� �ε�ť�� �߰�..

	LoadBox lb;
	lb.pSector	= pSector;
	lb.nDetail	= nDetail;

	TRACE("Insert Load Queue ( Waiting Queue %d ) : %d\n"	, 
		m_listSectorQueue.GetCount()						,
		pSector->GetArrayIndexDWORD()						);

	m_listSectorQueue.AddTail( lb );

	return TRUE;
}

BOOL	AcMapLoader::DoExistSectorInfo( ApWorldSector * pSector, INT32 nDetail )
{
	if ( m_listSectorQueue.GetCount() == 0 )
	{
		return FALSE;
	}
	
	// �ߺ� �ž����� ����..
	LoadBox *		pLoadBox;

	AuNode< LoadBox >	*pNode			= m_listSectorQueue.GetHeadNode();

	while( pNode )
	{
		pLoadBox	= & pNode->GetData();

		if( pLoadBox->pSector == pSector	&&
			pLoadBox->nDetail == nDetail	)
		{
			return TRUE;
		}

		m_listSectorQueue.GetNext( pNode );
	}

	return FALSE;
}

CMagUnpackManager	*	AcMapLoader::GetUnpackManager( INT32 nDetail , UINT nBlockIndex , char * pFilename )
{
	INT32				i;
	CMagUnpackManager * pUnpackManager = NULL;

	for( i = 0 ; i < m_nUnpackCount ; ++i  )
	{
		if( m_aUnpackBlock[ i ].pUnpackManager				&&
			nDetail		== m_aUnpackBlock[ i ].nDetail		&&
			nBlockIndex	== m_aUnpackBlock[ i ].uBlockIndex	)
		{
			// �ε��� �ȰŸ�...
			pUnpackManager	= m_aUnpackBlock[ i ].pUnpackManager;

			// �＼��Ÿ�� ������Ʈ..
			m_aUnpackBlock[ i ].uLastAccessTime	= GetTickCount();

			return pUnpackManager;
		}
	}

	// ����� ������.. �ε��°� ���ٴ� �Ҹ���..
	// 1,�󵥴ٰ� �ִµ�
	// 2,���� �＼�� ���� �༮�� ������ �ִ´�.

	// �󵥰� �ֳ�..
	if( m_nUnpackCount < ACMAPLOADER_PACK_QUEUE )
	{
		ASSERT( NULL == m_aUnpackBlock[ m_nUnpackCount ].pUnpackManager );//�̰� �ɸ��� �޸� ħ��.

		// �󵥴�!..
		pUnpackManager = new CMagUnpackManager;
		ASSERT( NULL != pUnpackManager );
		if( NULL == pUnpackManager ) return NULL;

		// ������ �ε��������� Ȯ��..
		if( !pUnpackManager->SetFile( pFilename ) )
		{
			// ������ ��������.
			delete pUnpackManager;
			return NULL;
		}

		// �� �����Ѵ�.
		m_aUnpackBlock[ m_nUnpackCount ].pUnpackManager	= pUnpackManager;
		m_aUnpackBlock[ m_nUnpackCount ].nDetail		= nDetail		;
		m_aUnpackBlock[ m_nUnpackCount ].uBlockIndex	= nBlockIndex	;
		m_aUnpackBlock[ m_nUnpackCount ].uLastAccessTime= GetTickCount();
		m_nUnpackCount++;

		return pUnpackManager;
	}

	ASSERT( m_nUnpackCount == ACMAPLOADER_PACK_QUEUE );

	// �� ���� �����Ȱ� ����.
	int		nOld		= 0;
	UINT	uOldTime	= m_aUnpackBlock[ 0 ].uLastAccessTime;

	for( i = 1 ; i < m_nUnpackCount ; ++i )
	{
		if( m_aUnpackBlock[ i ].pUnpackManager				&&
			uOldTime > m_aUnpackBlock[ i ].uLastAccessTime	)
		{
			nOld		= i;
			uOldTime	= m_aUnpackBlock[ i ].uLastAccessTime;
		}
	}

	// �� ���� �߰�..
	pUnpackManager = new CMagUnpackManager;
	ASSERT( NULL != pUnpackManager );
	if( NULL == pUnpackManager ) return NULL;

	// ������ �ε��������� Ȯ��..
	if( !pUnpackManager->SetFile( pFilename ) )
	{
		// ������ ��������.
		delete pUnpackManager;
		return NULL;
	}

	TRACE( "\"%s\"Pack���� ���� , \"%s\"���� �ε�.\n"				, 
		m_aUnpackBlock[ nOld ].pUnpackManager->GetPackFileName()	,
		pFilename													);

	ASSERT( NULL != m_aUnpackBlock[ nOld ].pUnpackManager );//�̰� �ɸ��� �޸� ħ��.
	if( m_aUnpackBlock[ nOld ].pUnpackManager ) delete m_aUnpackBlock[ nOld ].pUnpackManager;

	// �� �����Ѵ�.
	m_aUnpackBlock[ nOld ].pUnpackManager	= pUnpackManager;
	m_aUnpackBlock[ nOld ].nDetail			= nDetail		;
	m_aUnpackBlock[ nOld ].uBlockIndex		= nBlockIndex	;
	m_aUnpackBlock[ nOld ].uLastAccessTime	= GetTickCount();

	return pUnpackManager;
}

void		AcMapLoader::EmptyQueue		()	// ť�� �ִ� �ε� ������ �����Ѵ�..
{
	m_listSectorQueue.RemoveAll();
}

BOOL		AcMapLoader::RemoveQueue		( ApWorldSector * pvSector , INT32 nDetail )	// �ش� ������ �ε� ť�� ��� �����Ѵ�.
{
	PROFILE("AcMapLoader::RemoveQueue");

	// �̹� �ε��� �Ǿ� �ִ� �༮�̸� �ٷιٷ� ô���Ŵ..
	if( ! AGCMMAP_THIS->m_pcsApmMap->IsAutoLoadData() )
	{
		if( nDetail > 0 )
			AGCMMAP_THIS->SetCurrentDetail( pvSector , nDetail - 1 );

		return TRUE;
	}

	if( nDetail != -1 )
	{
		if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , nDetail )	)
		{
			// �ε��� ���� �༮�� Destroy �ݺ��� ȣ������..
			switch( nDetail )
			{
			case SECTOR_HIGHDETAIL	:	AGCMMAP_THIS->OnDestroyDetail( pvSector );	break;
			case SECTOR_LOWDETAIL	:	AGCMMAP_THIS->OnDestroyRough	( pvSector );	break;
			case SECTOR_EMPTY		:												break;	// do nothing..
			}

			return TRUE;
		}
	}
	else
	{
		// ��� �������.
		// �ε��ȳ��� ��� ���̰�... ���� ����Ʈ������ ������..
		if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , SECTOR_HIGHDETAIL	) ) AGCMMAP_THIS->OnDestroyDetail( pvSector );
		if( AGCMMAP_THIS->IsLoadedDetail	( pvSector , SECTOR_LOWDETAIL	) ) AGCMMAP_THIS->OnDestroyRough	( pvSector );
	}

	// �ε��� ���� �ʾҴµ� , Ȥ�� �ε�ť�� ��������� �ʴ��� ����.
	LoadBox				*pLoadBox		;

	AuNode< LoadBox >	*pNode			= m_listSectorQueue.GetHeadNode();
	AuNode< LoadBox >	*pNodeDelete	;
	//BOOL				bFound			= FALSE;

	while( pNode )
	{
		pLoadBox	= & pNode->GetData();

		pNodeDelete	= pNode;

		m_listSectorQueue.GetNext( pNode );

		if( pLoadBox->pSector == pvSector )
		{
			if( nDetail == -1 )
			{
				// �ش� �༮ ��� ����..
				m_listSectorQueue.RemoveNode( pNodeDelete );
			}
			else
			// �ش� �����ϸ� ����.. �ΰ��
			if( pLoadBox->nDetail	== nDetail	)
			{
				// �ϳ� �����ϸ� ����..
				m_listSectorQueue.RemoveNode( pNodeDelete );
				return TRUE;
			}
		}
	}

	// �浹�ϴ°� ����..
	return FALSE;
}

void	AcMapLoader::FlushUnpackManager()
{
	for( int i = 0 ; i < ACMAPLOADER_PACK_QUEUE ; ++ i)
	{
		if( m_aUnpackBlock[ i ].pUnpackManager ) 
			delete m_aUnpackBlock[ i ].pUnpackManager;
		m_aUnpackBlock[ i ].pUnpackManager		= NULL;
		m_aUnpackBlock[ i ].nDetail				= 0;
		m_aUnpackBlock[ i ].uBlockIndex			= 0;
		m_aUnpackBlock[ i ].uLastAccessTime		= 0;
	}

	m_nUnpackCount = 0;
}