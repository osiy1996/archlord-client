// UndoManager.cpp: implementation of the CUndoManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapTool.h"
#include "MainWindow.h"
#include "MainFrm.h"
#include "UndoManager.h"

#include "ApmObject.h"
#include "AgcmObject.h"

#include "AcuRpMTexture.h"
//#include "AcuRpDWSector.h"

#include "MyEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern MainWindow			g_MainWindow	;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUndoManager::ActionBlock::~ActionBlock()
{
	ActionUnit * pUnit;
	AuNode< ActionUnit * > *pNode = listUnit.GetHeadNode();
	while( pNode = listUnit.GetHeadNode() )
	{
		pUnit = pNode->GetData(); 

		ASSERTE( pUnit != NULL );
		ASSERTE( !IsBadReadPtr( pUnit , sizeof( ActionUnit ) ) );

		delete pUnit;
		listUnit.RemoveHead();
	}
}

CUndoManager::CUndoManager()
{
	m_pCurrentActionBlock	= NULL;
	m_pCurrentNode			= NULL;
}

CUndoManager::~CUndoManager()
{
	ActionBlock * pBlock;
	AuNode< ActionBlock * >		* pNode;
	while( pNode = listUndo.GetHeadNode() )
	{
		pBlock = pNode->GetData();

		ASSERTE( pBlock != NULL );
		ASSERTE( !IsBadReadPtr( pBlock , sizeof( ActionBlock ) ) );

		delete pBlock;
		listUndo.RemoveHead();
	}
}


void	CUndoManager::StartActionBlock		( int actiontype )
{
	ActionBlock	* pActionBlock;

	pActionBlock	= _StartActionBlock( actiontype );

	ASSERT( pActionBlock != NULL );

	m_pCurrentActionBlock = pActionBlock;

	TRACE( "��� �� ��������..\n" );
}

void	CUndoManager::EndActionBlock		()
{
	if( m_pCurrentActionBlock == NULL ) return;
	// ��� ���ϰ��..
	// Ŀ��Ʈ ���ķ� ��� �����Ѵ�..
	ActionBlock * pBlock;
	AuNode< ActionBlock * >		* pNode	= m_pCurrentNode;
	while( pNode )
	{
		m_pCurrentNode = pNode->GetNextNode();

		pBlock = pNode->GetData();

		ASSERTE( pBlock != NULL );
		ASSERTE( !IsBadReadPtr( pBlock , sizeof( ActionBlock ) ) );
		
		delete pBlock;
		listUndo.RemoveNode( pNode );

		pNode = m_pCurrentNode;
	}

	_EndActionBlock( m_pCurrentActionBlock );
	m_pCurrentActionBlock = NULL;
	TRACE( "��� �� ��ϿϷ�....\n" );
}

BOOL	CUndoManager::Undo()
{
	if( listUndo.GetCount() <= 0 ) return FALSE;

	// ���..
	ActionBlock	*	pActionBlock;
	//rsDWSectorInfo	dwSectorInfo;
	
	// �������� ��θ� �ϴ°��.. ����Ÿ�� �ִ°��...
	if( m_pCurrentNode )
	{
		if( m_pCurrentNode->GetPrevNode() )
		{
			// �������� ������...
			// Ŀ��Ʈ�� �������� ó���ϰ���..
			pActionBlock = m_pCurrentNode->GetPrevNode()->GetData();
		}
		else
		{
			// �������� ���ٸ�.. ť �� ������� �������� �뺸��.
			DisplayMessage( AEM_NOTIFY , "��� ����Ʈ�� ���� �����!\n" );
			return FALSE;
		}
	}
	else
	{
		// ù ��δ� , ����Ʈ�� ���� ������ ������..
		pActionBlock = listUndo.GetTailNode()->GetData();
	}

	//ApWorldSector * arraySector[ 100 ];
	//int			countSector = 0 , i ;
	
	// ���� ����Ÿ �غ�.
	ActionBlock	* pRedoData = _StartActionBlock( pActionBlock->nType );
	
	switch( pActionBlock->nType )
	{
	case	GEOMETRY	:
		{
			
			// ������Ʈ�� ���.
			ActionUnit_Geometry * pUnit;
			AuNode< ActionUnit * >	*pNode = pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_Geometry * ) pNode->GetData();

				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���������� ��������..
				if( AGCMMAP_THIS->D_SetHeight( pUnit->pSector , SECTOR_HIGHDETAIL ,
					pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->fOriginal ) )
				{
					// ���� ����Ÿ �ۼ�.
					_AddGeometryActionUnit( pRedoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->fChanged );
				}
				else
				{
					// ���Ͱ� �������� ����..
					TRACE( "��ε���Ÿ �̻�.. �������..\n" );
				}


				pActionBlock->listUnit.GetNext( pNode );
			}

			{
				AuNode< AgcmMap::stLockSectorInfo >	* pNode = AGCMMAP_THIS->m_listLockedSectorList.GetHeadNode();
				AgcmMap::stLockSectorInfo	* pLockedSector;
				while( pNode )
				{
					pLockedSector = &pNode->GetData();
					AGCMMAP_THIS->RecalcNormal( pLockedSector->pSector , SECTOR_HIGHDETAIL , FALSE );
					g_pMainFrame->m_pTileList->m_pOthers->ChangeSectorGeometry( pLockedSector->pSector );
					pNode = pNode->GetNextNode();
				}
			}

		}
		break;
	case	VERTEXCOLOR	:
		{
			
			// ������Ʈ�� ���.
			ActionUnit_VertexColor * pUnit;
			AuNode< ActionUnit * >	*pNode = pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_VertexColor * ) pNode->GetData();

				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���������� ��������..
				if( AGCMMAP_THIS->D_SetValue( pUnit->pSector , pUnit->pSector->GetCurrentDetail() ,
					pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->rgbOriginal ) )
				{
					// ���� ����Ÿ �ۼ�.
					_AddVertexColorActionunit( pRedoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->rgbChanged );
				}
				else
				{
					// ���Ͱ� �������� ����..
					TRACE( "��ε���Ÿ �̻�.. �������..\n" );
				}

				pActionBlock->listUnit.GetNext( pNode );
			}
		}
		break;	
	case	TILE		:
		{
			ActionUnit_Tile *			pUnit		;
			AuNode< ActionUnit * > *	pNode		= pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_Tile * ) pNode->GetData();
				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���� ����Ÿ �ۼ�.
				_AddTileActionUnit( pRedoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->anOriginalIndex[ TD_FIRST ] , pUnit->anOriginalIndex[ TD_SECOND	] ,
					pUnit->anOriginalIndex[ TD_THIRD ] , pUnit->anOriginalIndex[ TD_FOURTH	] ,
					pUnit->anOriginalIndex[ TD_FIFTH ] , pUnit->anOriginalIndex[ TD_SIXTH	] );
				// ���������� ��������..
				AGCMMAP_THIS->D_SetTile( pUnit->pSector , pUnit->pSector->GetCurrentDetail() ,
					pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->anOriginalIndex[ TD_FIRST ] , pUnit->anOriginalIndex[ TD_SECOND	] ,
					pUnit->anOriginalIndex[ TD_THIRD ] , pUnit->anOriginalIndex[ TD_FOURTH	] ,
					pUnit->anOriginalIndex[ TD_FIFTH ] , pUnit->anOriginalIndex[ TD_SIXTH	]	);
				
				pActionBlock->listUnit.GetNext( pNode );
			}

			g_MainWindow.UnlockSectors( FALSE , TRUE );
		}
		break;
	case	OBJECTMANAGE :
		{
			// ������Ʈ ���..
			ActionUnit_Object * pUnit;
			AuNode< ActionUnit * >	*pNode = pActionBlock->listUnit.GetHeadNode();
			ApdObject *		pstApdObject	;
			AgcdObject *	pstAgcdObject	;

			while( pNode )
			{
				pUnit = ( ActionUnit_Object * ) pNode->GetData();
				pstApdObject	= g_pcsApmObject->	GetObject		( pUnit->oid	);
				if( NULL != pstApdObject )
				{
					pstAgcdObject	= g_pcsAgcmObject->GetObjectData	( pstApdObject	);

					// ���� �̵� ����..

					// ������Ʈ ���� �����Ŵ.
					pstApdObject->m_stScale		= pUnit->stScale	;
					pstApdObject->m_stPosition	= pUnit->stPosition	;
					pstApdObject->m_fDegreeX	= pUnit->fDegreeX	;
					pstApdObject->m_fDegreeY	= pUnit->fDegreeY	;

					g_pcsAgcmObject->SetupObjectClump_TransformOnly( pstApdObject );

					// ���� �����Ͻ�Ŵ..

					ObjectUndoInfo	original , changed;
					original.oid		= pUnit->oid				;
					original.stScale	= pUnit->stChangedScale		;
					original.stPosition	= pUnit->stChangedPosition	;
					original.fDegreeX	= pUnit->fChangedDegreeX	;
					original.fDegreeY	= pUnit->fChangedDegreeY	;

					changed.oid			= pUnit->oid		;
					changed.stScale		= pUnit->stScale	;
					changed.stPosition	= pUnit->stPosition	;
					changed.fDegreeX	= pUnit->fDegreeX	;
					changed.fDegreeY	= pUnit->fDegreeY	;

					_AddObjectActionUnit( pRedoData , &original , &changed );
				}

				pActionBlock->listUnit.GetNext( pNode );
			}
		}
		break;
	case	EDGETURN	:
		{
			ActionUnit_EdgeTurn *		pUnit		;
			AuNode< ActionUnit * > *	pNode		= pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_EdgeTurn * ) pNode->GetData();
				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���� ����Ÿ �ۼ�.
				_AddEdgeTurnActionUnit( pRedoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->bTurnOriginal );
				// ���������� ��������..
				AGCMMAP_THIS->D_SetEdgeTurn( pUnit->pSector , pUnit->pSector->GetCurrentDetail() ,
					pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->bTurnOriginal	);
				
				pActionBlock->listUnit.GetNext( pNode );
			}

			{
				AuNode< AgcmMap::stLockSectorInfo >	* pNode = AGCMMAP_THIS->m_listLockedSectorList.GetHeadNode();
				AgcmMap::stLockSectorInfo	* pLockedSector;
				while( pNode )
				{
					pLockedSector = &pNode->GetData();
					g_pMainFrame->m_pTileList->m_pOthers->ChangeSectorGeometry( pLockedSector->pSector );
					pNode = pNode->GetNextNode();
				}
			}
		}
		break;
	case	SUBDIVISION	:
		break;
	default:
		return FALSE;
	}

	// ������Ʈ..
	g_MainWindow.UnlockSectors( TRUE , TRUE );
	
	// ���� ����Ÿ ���.
	if( m_pCurrentNode )
	{
		AuNode< ActionBlock * >	* pPrevNode = m_pCurrentNode->GetPrevNode();
		delete pPrevNode->GetData();
		listUndo.RemoveNode( pPrevNode );
		listUndo.InsertBefore( m_pCurrentNode , pRedoData );
		m_pCurrentNode = m_pCurrentNode->GetPrevNode();
	}
	else
	{
		// ������.. ���� �����ϰ�. ������ ���.
		delete listUndo.GetTailNode()->GetData();
		listUndo.RemoveTail();
		listUndo.AddTail( pRedoData );
		m_pCurrentNode = listUndo.GetTailNode();
	}
	
	return TRUE;
}

BOOL	CUndoManager::Redo()
{
	if( m_pCurrentNode == NULL ) return FALSE;

	// ���..
	ActionBlock	*	pActionBlock;
	//rsDWSectorInfo	dwSectorInfo;
	
	// �������� ��θ� �ϴ°��.. ����Ÿ�� �ִ°��...
	pActionBlock = m_pCurrentNode->GetData();

	//ApWorldSector * arraySector[ 100 ];
	//int			countSector = 0 , i ;
	
	// ���� ����Ÿ �غ�.
	ActionBlock	* pUndoData = _StartActionBlock( pActionBlock->nType );
	
	switch( pActionBlock->nType )
	{
	case	VERTEXCOLOR	:
		{
			
			// ������Ʈ�� ���.
			ActionUnit_VertexColor * pUnit;
			AuNode< ActionUnit * >	*pNode = pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_VertexColor * ) pNode->GetData();

				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���������� ��������..
				if( AGCMMAP_THIS->D_SetValue( pUnit->pSector , pUnit->pSector->GetCurrentDetail() ,
					pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->rgbOriginal ) )
				{
					// ���� ����Ÿ �ۼ�.
					_AddVertexColorActionunit( pUndoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->rgbChanged );
				}
				else
				{
					// ���Ͱ� �������� ����..
					TRACE( "��ε���Ÿ �̻�.. �������..\n" );
				}

				pActionBlock->listUnit.GetNext( pNode );
			}
		}
		break;	
	case	TILE		:
		{
			ActionUnit_Tile *			pUnit		;
			AuNode< ActionUnit * > *	pNode		= pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_Tile * ) pNode->GetData();
				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���� ����Ÿ �ۼ�.
				_AddTileActionUnit( pUndoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->anOriginalIndex[ TD_FIRST ] , pUnit->anOriginalIndex[ TD_SECOND	] ,
					pUnit->anOriginalIndex[ TD_THIRD ] , pUnit->anOriginalIndex[ TD_FOURTH	] ,
					pUnit->anOriginalIndex[ TD_FIFTH ] , pUnit->anOriginalIndex[ TD_SIXTH	] );
				// ���������� ��������..
				AGCMMAP_THIS->D_SetTile( pUnit->pSector , pUnit->pSector->GetCurrentDetail() ,
					pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->anOriginalIndex[ TD_FIRST ] , pUnit->anOriginalIndex[ TD_SECOND	] ,
					pUnit->anOriginalIndex[ TD_THIRD ] , pUnit->anOriginalIndex[ TD_FOURTH	] ,
					pUnit->anOriginalIndex[ TD_FIFTH ] , pUnit->anOriginalIndex[ TD_SIXTH	] );
				
				pActionBlock->listUnit.GetNext( pNode );
			}

			g_MainWindow.UnlockSectors( FALSE , TRUE );
		}
		break;
	case	GEOMETRY	:
		{
			
			// ������Ʈ�� ���.
			ActionUnit_Geometry * pUnit;
			AuNode< ActionUnit * >	*pNode = pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_Geometry * ) pNode->GetData();

				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���������� ��������..
				if( AGCMMAP_THIS->D_SetHeight( pUnit->pSector , SECTOR_HIGHDETAIL ,
					pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->fOriginal ) )
				{
					// ���� ����Ÿ �ۼ�.
					_AddGeometryActionUnit( pUndoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ , pUnit->fChanged );
				}
				else
				{
					// ���Ͱ� �������� ����..
					TRACE( "��ε���Ÿ �̻�.. �������..\n" );
				}


				pActionBlock->listUnit.GetNext( pNode );
			}

			{
				AuNode< AgcmMap::stLockSectorInfo >	* pNode = AGCMMAP_THIS->m_listLockedSectorList.GetHeadNode();
				AgcmMap::stLockSectorInfo	* pLockedSector;
				while( pNode )
				{
					pLockedSector = &pNode->GetData();
					AGCMMAP_THIS->RecalcNormal( pLockedSector->pSector , SECTOR_HIGHDETAIL , FALSE );
					g_pMainFrame->m_pTileList->m_pOthers->ChangeSectorGeometry( pLockedSector->pSector );
					pNode = pNode->GetNextNode();
				}
			}
		}
		break;
	case	OBJECTMANAGE :
		{
			// ������Ʈ ���..
			ActionUnit_Object * pUnit;
			AuNode< ActionUnit * >	*pNode = pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_Object * ) pNode->GetData();
				{
					ApdObject *		pstApdObject	;
					AgcdObject *	pstAgcdObject	;

					pstApdObject	= g_pcsApmObject->	GetObject		( pUnit->oid	);
					pstAgcdObject	= g_pcsAgcmObject->	GetObjectData	( pstApdObject	);

					// ���� �̵� ����..

					// ������Ʈ ���� �����Ŵ.
					pstApdObject->m_stScale		= pUnit->stScale	;
					pstApdObject->m_stPosition	= pUnit->stPosition	;
					pstApdObject->m_fDegreeX	= pUnit->fDegreeX	;
					pstApdObject->m_fDegreeY	= pUnit->fDegreeY	;

					g_pcsAgcmObject->SetupObjectClump_TransformOnly( pstApdObject );

					// ���� �����Ͻ�Ŵ..

					ObjectUndoInfo	original , changed;
					original.oid		= pUnit->oid				;
					original.stScale	= pUnit->stChangedScale		;
					original.stPosition	= pUnit->stChangedPosition	;
					original.fDegreeX	= pUnit->fChangedDegreeX	;
					original.fDegreeY	= pUnit->fChangedDegreeY	;

					changed.oid			= pUnit->oid		;
					changed.stScale		= pUnit->stScale	;
					changed.stPosition	= pUnit->stPosition	;
					changed.fDegreeX	= pUnit->fDegreeX	;
					changed.fDegreeY	= pUnit->fDegreeY	;

					_AddObjectActionUnit( pUndoData , &original , &changed );
				}

				pActionBlock->listUnit.GetNext( pNode );
			}
		}
		break;
	case	EDGETURN	:
		{
			ActionUnit_EdgeTurn *		pUnit		;
			AuNode< ActionUnit * > *	pNode		= pActionBlock->listUnit.GetHeadNode();

			while( pNode )
			{
				pUnit = ( ActionUnit_EdgeTurn * ) pNode->GetData();
				ASSERT( NULL != pUnit			);
				ASSERT( NULL != pUnit->pSector	);

				// ���� ����Ÿ �ۼ�.
				_AddEdgeTurnActionUnit( pUndoData , pUnit->pSector , pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->bTurnOriginal );
				// ���������� ��������..
				AGCMMAP_THIS->D_SetEdgeTurn( pUnit->pSector , pUnit->pSector->GetCurrentDetail() ,
					pUnit->nSegmentX , pUnit->nSegmentZ ,
					pUnit->bTurnOriginal	);
				
				pActionBlock->listUnit.GetNext( pNode );
			}

			{
				AuNode< AgcmMap::stLockSectorInfo >	* pNode = AGCMMAP_THIS->m_listLockedSectorList.GetHeadNode();
				AgcmMap::stLockSectorInfo	* pLockedSector;
				while( pNode )
				{
					pLockedSector = &pNode->GetData();
					g_pMainFrame->m_pTileList->m_pOthers->ChangeSectorGeometry( pLockedSector->pSector );
					pNode = pNode->GetNextNode();
				}
			}
		}
		break;
	case	SUBDIVISION	:
		break;
	default:
		return FALSE;
	}

	// ������Ʈ..
	g_MainWindow.UnlockSectors( TRUE , TRUE );
	
	// ��� ����Ÿ ���.

	AuNode< ActionBlock * >	* pNextNode = m_pCurrentNode->GetNextNode();
	listUndo.InsertBefore( m_pCurrentNode , pUndoData );
	delete m_pCurrentNode->GetData();
	listUndo.RemoveNode( m_pCurrentNode );
	m_pCurrentNode = pNextNode;

	return TRUE;
}

CUndoManager::ActionBlock *	CUndoManager::_StartActionBlock		( int actiontype )
{
	// �׼� ������..
	ActionBlock	* pActionBlock = new ActionBlock;

	ASSERT( pActionBlock != NULL );

	pActionBlock->nType	= actiontype;

	return pActionBlock;
}

void	CUndoManager::_EndActionBlock		( ActionBlock * pBlock )
{
	if( pBlock )
	{
		if( pBlock->listUnit.GetCount() <= 0 )
		{
			// ���ڵ� �� �༮�� �����..
			delete pBlock;
			return;
		}

		listUndo.AddTail( pBlock );

		if( listUndo.GetCount() > MAX_UNDO_BLOCK_SIZE )
		{
			ActionBlock	* pActionBlock = listUndo.GetHeadNode()->GetData();
			listUndo.RemoveHead();
			delete pActionBlock;
		}
	}
}



void	CUndoManager::AddObjectActionUnit		( ObjectUndoInfo * original , ObjectUndoInfo * changed )
{
	if( m_pCurrentActionBlock && m_pCurrentActionBlock->nType == OBJECTMANAGE )
	{
		_AddObjectActionUnit( m_pCurrentActionBlock , original , changed );
	}	
}

void	CUndoManager::AddEdgeTurnActionunit( ApWorldSector * pSector , int nSegmentX , int nSegmentZ , BOOL bTurnChanged )
{
	if( m_pCurrentActionBlock && m_pCurrentActionBlock->nType == EDGETURN )
	{
		_AddEdgeTurnActionUnit( m_pCurrentActionBlock , pSector , nSegmentX , nSegmentZ , bTurnChanged );
	}	
}


void	CUndoManager::_AddObjectActionUnit	(  ActionBlock * pBlock , ObjectUndoInfo * original , ObjectUndoInfo * changed )
{
	if(	original->fDegreeX			== changed->fDegreeX		&&
		original->fDegreeY			== changed->fDegreeY		&&
		original->stScale.x			== changed->stScale.x		&&
		original->stScale.y			== changed->stScale.y		&&
		original->stScale.z			== changed->stScale.z		&&
		original->stPosition.x		== changed->stPosition.x	&&
		original->stPosition.y		== changed->stPosition.y	&&
		original->stPosition.z		== changed->stPosition.z	)
	{
		// ideal..

		return;
	}

	ActionUnit_Object * pActionUnit = new ActionUnit_Object;
	ASSERT( pActionUnit != NULL );

	pActionUnit->pSector			= NULL					;
	pActionUnit->nType				= OBJECTMANAGE			;

	pActionUnit->oid				= original->oid			;
	
	pActionUnit->stScale			= original->stScale		;
	pActionUnit->stPosition			= original->stPosition	;
	pActionUnit->fDegreeX			= original->fDegreeX	;
	pActionUnit->fDegreeY			= original->fDegreeY	;

	pActionUnit->stChangedScale		= changed->stScale		;
	pActionUnit->stChangedPosition	= changed->stPosition	;
	pActionUnit->fChangedDegreeX	= changed->fDegreeX		;
	pActionUnit->fChangedDegreeY	= changed->fDegreeY		;

	pBlock->listUnit.AddHead( pActionUnit );

}

void	CUndoManager::ClearObjectUndoData		( int oid )
{
	AuNode< ActionBlock * >		* pNode	= listUndo.GetHeadNode();
	ActionBlock * pActionBlock;

	while( pNode )
	{
		pActionBlock = pNode->GetData();

		if( pActionBlock->nType == OBJECTMANAGE )
		{
			// oid �� ���� ������ üũ�Ѵ�.
			AuNode< ActionUnit * > *	pNodeUnit	;
			ActionUnit_Object	*		pUnit		;

			pNodeUnit = pActionBlock->listUnit.GetHeadNode();

			while( pNodeUnit )
			{
				pUnit = ( ActionUnit_Object	* ) pNodeUnit->GetData();

				if( pUnit->oid == oid )
				{
					// �� ��ũ�θ� ������..
					break;
				}

				pActionBlock->listUnit.GetNext( pNodeUnit );
			}

			// �����Ͽ���..
			if( pNodeUnit )
			{
				AuNode< ActionBlock * >		* pNodeDelete = pNode;
				listUndo.GetNext( pNode );
				listUndo.RemoveNode( pNodeDelete );
				continue;
			}
		}
		listUndo.GetNext( pNode );
	}
	
}

void	CUndoManager::ClearAllUndoData		(			)
{
	AuNode< ActionBlock * >		* pNode;
	while( pNode = listUndo.GetHeadNode() )
	{
		delete pNode->GetData();
		listUndo.RemoveHead();
	}

	DisplayMessage( AEM_NOTIFY , "���α׷� ������ ���ؼ� ��� ����Ÿ�� �ʱ�ȭ �ž����ϴ�." );
}

void	CUndoManager::AddGeometryActionUnit	( ApWorldSector * pSector , int nSegmentX , int nSegmentZ , float fChangedValue )
{
	if( m_pCurrentActionBlock && m_pCurrentActionBlock->nType == GEOMETRY )
	{
		_AddGeometryActionUnit( m_pCurrentActionBlock , pSector , nSegmentX , nSegmentZ , fChangedValue );
	}

}

void	CUndoManager::AddTileActionUnit		( ApWorldSector * pSector , int nSegmentX , int nSegmentZ ,
											UINT32 nChangedIndex	, UINT32 nChangedIndex2	,
											UINT32 nChangedIndex3	, UINT32 nChangedIndex4	,
											UINT32 nChangedIndex5	, UINT32 nChangedIndex6	)
{
	if( m_pCurrentActionBlock && m_pCurrentActionBlock->nType == TILE )
	{
		_AddTileActionUnit( m_pCurrentActionBlock , pSector , nSegmentX , nSegmentZ ,
			nChangedIndex , nChangedIndex2 , nChangedIndex3 , nChangedIndex4 , nChangedIndex5 , nChangedIndex6 );
	}
}
void	CUndoManager::AddVertexColorActionunit( ApWorldSector * pSector , int nSegmentX , int nSegmentZ , ApRGBA rgbChanged )
{
	if( m_pCurrentActionBlock && m_pCurrentActionBlock->nType == VERTEXCOLOR )
	{
		_AddVertexColorActionunit( m_pCurrentActionBlock , pSector , nSegmentX , nSegmentZ , rgbChanged );
	}
}

void	CUndoManager::_AddGeometryActionUnit	( ActionBlock * pBlock , ApWorldSector * pSector ,
												 int nSegmentX , int nSegmentZ , float fChangedValue )
{
	// ���� ��� ���� �߰�.
	ASSERT( NULL != pBlock	);
	ASSERT( NULL != pSector	);

	ApDetailSegment	* pSegment = pSector->D_GetSegment( nSegmentX , nSegmentZ );

	ASSERT( NULL != pSegment	);

	ActionUnit_Geometry * pActionUnit = new ActionUnit_Geometry;

	ASSERT( pActionUnit != NULL );
	
	pActionUnit->pSector					= pSector					;
	pActionUnit->nType						= TILE						;
	pActionUnit->nSegmentX					= nSegmentX					;
	pActionUnit->nSegmentZ					= nSegmentZ					;

	pActionUnit->fOriginal					= pSegment->height		;
	pActionUnit->fChanged					= fChangedValue				;

	pBlock->listUnit.AddHead( pActionUnit );
}

void	CUndoManager::_AddTileActionUnit		( ActionBlock * pBlock , ApWorldSector * pSector ,
												int nSegmentX , int nSegmentZ ,
												UINT32 nChangedIndex	, UINT32 nChangedIndex2	,
												UINT32 nChangedIndex3	, UINT32 nChangedIndex4	,
												UINT32 nChangedIndex5	, UINT32 nChangedIndex6	)
{
	// Ÿ�� ��� ���� �߰�.
	ASSERT( NULL != pBlock	);
	ASSERT( NULL != pSector	);

	ApDetailSegment	* pSegment = pSector->D_GetSegment( nSegmentX , nSegmentZ );

	ASSERT( NULL != pSegment			);

	// ������ �������� ����..
	if(	pSegment->pIndex[ TD_FIRST	] == nChangedIndex	&&
		pSegment->pIndex[ TD_SECOND	] == nChangedIndex2	&&
		pSegment->pIndex[ TD_THIRD	] == nChangedIndex3	&&
		pSegment->pIndex[ TD_FOURTH	] == nChangedIndex4	&&
		pSegment->pIndex[ TD_FIFTH	] == nChangedIndex5	&&
		pSegment->pIndex[ TD_SIXTH	] == nChangedIndex6	)
		return;

	ActionUnit_Tile * pActionUnit = new ActionUnit_Tile;

	ASSERT( pActionUnit != NULL );
	
	pActionUnit->pSector					= pSector					;
	pActionUnit->nType						= TILE						;
	pActionUnit->nSegmentX					= nSegmentX					;
	pActionUnit->nSegmentZ					= nSegmentZ					;

	pActionUnit->anOriginalIndex	[ TD_FIRST	]	= pSegment->pIndex[ TD_FIRST	]	;
	pActionUnit->anChangedIndex		[ TD_FIRST	]	= nChangedIndex						;

	pActionUnit->anOriginalIndex	[ TD_SECOND	]	= pSegment->pIndex[ TD_SECOND	]	;
	pActionUnit->anChangedIndex		[ TD_SECOND	]	= nChangedIndex2					;

	pActionUnit->anOriginalIndex	[ TD_THIRD	]	= pSegment->pIndex[ TD_THIRD	]	;
	pActionUnit->anChangedIndex		[ TD_THIRD	]	= nChangedIndex3					;

	pActionUnit->anOriginalIndex	[ TD_FOURTH	]	= pSegment->pIndex[ TD_FOURTH	]	;
	pActionUnit->anChangedIndex		[ TD_FOURTH	]	= nChangedIndex4					;

	pActionUnit->anOriginalIndex	[ TD_FIFTH	]	= pSegment->pIndex[ TD_FIFTH	]	;
	pActionUnit->anChangedIndex		[ TD_FIFTH	]	= nChangedIndex5					;

	pActionUnit->anOriginalIndex	[ TD_SIXTH	]	= pSegment->pIndex[ TD_SIXTH	]	;
	pActionUnit->anChangedIndex		[ TD_SIXTH	]	= nChangedIndex6					;

	pBlock->listUnit.AddHead( pActionUnit );

}
void	CUndoManager::_AddVertexColorActionunit( ActionBlock * pBlock , ApWorldSector * pSector ,
												int nSegmentX , int nSegmentZ , ApRGBA rgbChanged )
{
	// ���ؽ� Į�� ��� ���� �߰���.
	ASSERT( NULL != pBlock	);
	ASSERT( NULL != pSector	);

	ApDetailSegment	* pSegment = pSector->D_GetSegment( nSegmentX , nSegmentZ );

	ASSERT( NULL != pSegment	);

	ActionUnit_VertexColor * pActionUnit = new ActionUnit_VertexColor;

	ASSERT( pActionUnit != NULL );
	
	pActionUnit->pSector					= pSector					;
	pActionUnit->nType						= TILE						;
	pActionUnit->nSegmentX					= nSegmentX					;
	pActionUnit->nSegmentZ					= nSegmentZ					;

	pActionUnit->rgbOriginal				= pSegment->vertexcolor	;
	pActionUnit->rgbChanged					= rgbChanged				;

	pBlock->listUnit.AddHead( pActionUnit );
}

void	CUndoManager::_AddEdgeTurnActionUnit	(  ActionBlock * pBlock , ApWorldSector * pSector , int nSegmentX , int nSegmentZ , BOOL bTurnChanged )
{
	ASSERT( NULL != pBlock	);
	ASSERT( NULL != pSector	);

	ApDetailSegment	* pSegment = pSector->D_GetSegment( nSegmentX , nSegmentZ );

	ASSERT( NULL != pSegment	);

	if( NULL == pSegment ) return;

	// ������ (2004-06-15 ���� 3:31:55) : // ������ ó������..
	if( pSegment->stTileInfo.GetEdgeTurn() == bTurnChanged ) return;

	ActionUnit_EdgeTurn * pActionUnit = new ActionUnit_EdgeTurn;

	ASSERT( pActionUnit != NULL );
	
	pActionUnit->pSector					= pSector					;
	pActionUnit->nType						= TILE						;
	pActionUnit->nSegmentX					= nSegmentX					;
	pActionUnit->nSegmentZ					= nSegmentZ					;

	pActionUnit->bTurnOriginal				= pSegment->stTileInfo.GetEdgeTurn()	;
	pActionUnit->bTurnChanged				= bTurnChanged							;

	pBlock->listUnit.AddHead( pActionUnit );
}