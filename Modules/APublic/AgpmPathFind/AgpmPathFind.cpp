#include "AgpmPathFind.h"
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <ApBase/ApBase.h>

AgpmPathFind::AgpmPathFind()
{
	SetModuleName("AgpmPathFind");
	SetModuleType(APMODULE_TYPE_PUBLIC);

	m_bFound = false;
	m_iRevision = 0;
	m_iWidth = 0;
	m_iHeight = 0;

	m_aArraryX[0] = 0;
	m_aArraryX[1] = 1;
	m_aArraryX[2] = 1;
	m_aArraryX[3] = 1;
	m_aArraryX[4] = 0;
	m_aArraryX[5] =-1;
	m_aArraryX[6] =-1;
	m_aArraryX[7] =-1;

	m_aArraryY[0] =-1;
	m_aArraryY[1] =-1;
	m_aArraryY[2] = 0;
	m_aArraryY[3] = 1;
	m_aArraryY[4] = 1;
	m_aArraryY[5] = 1;
	m_aArraryY[6] = 0;
	m_aArraryY[7] =-1;

	m_ppcPathFindBlock	= NULL;

	initBlock( AGPMPATHFIND_RADIUS, AGPMPATHFIND_RADIUS );
	initSearchNode( AGPMPATHFIND_TRY_COUNT );

	m_Mutex.Init();

//	m_aArraryX = { 0, 1, 1, 1, 0,-1,-1,-1 };
//	m_aArraryY = {-1,-1, 0, 1, 1, 1, 0,-1 };
	m_papmMap = NULL;
}

AgpmPathFind::~AgpmPathFind()
{
//	reset();
	m_Mutex.Destroy();

	// ������ (2004-05-29 ���� 4:57:22) : �޸� �� ���� �߰�~
	if( m_ppcPathFindBlock )
	{
		for( int iCounter=0; iCounter<m_iWidth; iCounter++ )
		{
			if( m_ppcPathFindBlock[iCounter] )
			{
				delete [] m_ppcPathFindBlock[iCounter];
			}
		}

		delete [] m_ppcPathFindBlock;
	}

}

void AgpmPathFind::lock()
{
	m_Mutex.WLock();
}

void AgpmPathFind::unlock()
{
	m_Mutex.Release();
}

BOOL AgpmPathFind::OnAddModule()
{
	//Ư���� �ϴ°� ����.
	m_papmMap = (ApmMap *)	GetModule("ApmMap");

	if( !m_papmMap )
		return FALSE;

	return TRUE;
}

void AgpmPathFind::initBlock( int iHalfWidth, int iHalfHeight )
{
	m_bFound = false;
	m_iRevision = 0;

	m_bFound = false;
	m_iRevision = 0;
	m_iHalfWidth = iHalfWidth;
	m_iHalfHeight = iHalfHeight;
	m_iWidth = m_iHalfWidth *2 + 1;
	m_iHeight = m_iHalfHeight *2 + 1;

	//�� ���� �Ҵ��Ѵ�.
#ifdef _DEBUG
	if( NULL != m_ppcPathFindBlock )
	{
		TRACE( "AgpmPathFind::initBlock : �̰� ������ ô�߾����� ����\n" );
	}
#endif //ifdef _DEBUG	

	m_ppcPathFindBlock = new AgpdPathFindBlock *[m_iWidth];

	for( int iCounter=0; iCounter<m_iWidth; iCounter++ )
	{
		m_ppcPathFindBlock[iCounter] = new AgpdPathFindBlock[m_iHeight];
	}

	resetBlock();
}

void AgpmPathFind::resetBlock()
{
	if( m_ppcPathFindBlock )
	{
		int				iCounter;
		int				iTempX, iTempY;

		for( iCounter=0; iCounter<m_iWidth; ++iCounter )
		{
			m_ppcPathFindBlock[iCounter][0].m_iStatus = AGPD_PATHFIND_BLOCK;
			m_ppcPathFindBlock[iCounter][0].m_iRevision = 0xFFFFFFFF;

			m_ppcPathFindBlock[iCounter][m_iHeight-1].m_iStatus = AGPD_PATHFIND_BLOCK;
			m_ppcPathFindBlock[iCounter][m_iHeight-1].m_iRevision = 0xFFFFFFFF;
		}

		for( iCounter=0; iCounter<m_iHeight; ++iCounter )
		{
			m_ppcPathFindBlock[0][iCounter].m_iStatus = AGPD_PATHFIND_BLOCK;
			m_ppcPathFindBlock[0][iCounter].m_iRevision = 0xFFFFFFFF;

			m_ppcPathFindBlock[m_iWidth-1][iCounter].m_iStatus = AGPD_PATHFIND_BLOCK;
			m_ppcPathFindBlock[m_iWidth-1][iCounter].m_iRevision = 0xFFFFFFFF;
		}

		for( iTempY=1; iTempY<m_iHeight-1; ++iTempY )
		{
			for( iTempX=1; iTempX<m_iWidth-1; ++iTempX )
			{
				m_ppcPathFindBlock[iTempX][iTempY].m_iStatus = AGPD_PATHFIND_NOTUSED;
				m_ppcPathFindBlock[iTempX][iTempY].m_iRevision = 0;
			}
		}
	}
}

void AgpmPathFind::initSearchNode( int iNodeCount )
{
	m_cSearchList.allocSwapBuffer( iNodeCount );
	m_cSearchList.resetIndex();
}

void AgpmPathFind::reset()
{
	m_cSearchList.removeSwapBuffer();
}

void AgpmPathFind::getPathToExternalList( CSpecializeListPathFind<int, AgpdPathFindPoint *> *pcPathNodeList )
{
	AgpdPathFindPoint	cCurrentPathPoint;
	AgpdPathFindPoint	cTempPathPoint;

	pcPathNodeList->resetIndex();

	cCurrentPathPoint.m_iX = m_cTargetPoint.m_iX;
	cCurrentPathPoint.m_iY = m_cTargetPoint.m_iY;

	int				iABSX, iABSY;
	int				iTempABSX, iTempABSY;

	//0,1,-1�� �ƴ� ���� �ֱ����� �׳� 2�� �ʱ�ȭ�Ѵ�.
	iABSX = 2;
	iABSY = 2;

//	float		fStartX, fStartY;

//	fStartX = m_ppcPathFindBlock[50][50].m_fCurrentX;
//	fStartY = m_ppcPathFindBlock[50][50].m_fCurrentY;

	while( 1 )
	{
		if (cCurrentPathPoint.m_iX < 0 ||
			cCurrentPathPoint.m_iX >= m_iWidth ||
			cCurrentPathPoint.m_iY < 0 ||
			cCurrentPathPoint.m_iY >= m_iHeight)
			break;

		cTempPathPoint.m_iX = m_ppcPathFindBlock[cCurrentPathPoint.m_iX][cCurrentPathPoint.m_iY].m_cParent.m_iX;
		cTempPathPoint.m_iY = m_ppcPathFindBlock[cCurrentPathPoint.m_iX][cCurrentPathPoint.m_iY].m_cParent.m_iY;

		//�θ��尡 �������� ���� �ʴٴ� ���� ���� �������� �ʾҴٴ� ���̴�.
		iTempABSX = cTempPathPoint.m_iX - cCurrentPathPoint.m_iX;
		iTempABSY = cTempPathPoint.m_iY - cCurrentPathPoint.m_iY;

		if( (iABSX != iTempABSX) || (iABSY != iTempABSY) )
		{
			cCurrentPathPoint.m_fX = m_ppcPathFindBlock[cCurrentPathPoint.m_iX][cCurrentPathPoint.m_iY].m_fCurrentX;
			cCurrentPathPoint.m_fY = m_ppcPathFindBlock[cCurrentPathPoint.m_iX][cCurrentPathPoint.m_iY].m_fCurrentY;

			pcPathNodeList->insert( 0, &cCurrentPathPoint );

			//���� �̵������� �����Ѵ�.
			iABSX = iTempABSX;
			iABSY = iTempABSY;
		}

		if( (cTempPathPoint.m_iX == m_cStartPoint.m_iX) && (cTempPathPoint.m_iY == m_cStartPoint.m_iY) )
		{
/*			CPriorityListNode<int, AgpdPathFindPoint * > *pcsNode;
			int				iCounter;

			iCounter = 0;

			while( 1 )
			{
				pcsNode = pcPathNodeList->GetNextPath();

				if( pcsNode == NULL )
				{
					break;
				}
				else 
				{
					printf( "%f %f\n", pcsNode->data->m_fX, pcsNode->data->m_fY );
					iCounter++;
				}
			}

			printf( "\n" );

			//3�̻��̸�?
			if( iCounter > 2 )
			{
				int x;
				x = 0;
			}*/
		
			break;
		}

		cCurrentPathPoint.m_iX = cTempPathPoint.m_iX;
		cCurrentPathPoint.m_iY = cTempPathPoint.m_iY;
	}
}

void AgpmPathFind::initStartPoint()
{
	m_cStartPoint.m_iX = -1;
	m_cStartPoint.m_iY = -1;
}

void AgpmPathFind::initTargetPoint()
{
	m_cTargetPoint.m_iX = -1;
	m_cTargetPoint.m_iY = -1;
}

bool AgpmPathFind::pathFind()
{
	PROFILE("AgpmPathFind::pathFind");

	//�� �̻� ã�� ��尡 ���ٸ�?
	if( m_cSearchList.m_iIncreaseIndex == 0 )
	{
		return false;
	}

	//���� ������ ��带 ã�´�.
	AgpdPathFindPoint				PossibilityNode;

	//StartNode�� ���� ���� ���� ����!!
	PossibilityNode = *(m_cSearchList.start_node->data);

	//���� ���� ���� ã�Ƴ����� �ǵڷ� ����.
	m_cSearchList.swapHeadAndTail();

	if (PossibilityNode.m_iX >= 0 &&
		PossibilityNode.m_iX < m_iWidth &&
		PossibilityNode.m_iY >= 0 &&
		PossibilityNode.m_iY < m_iHeight)
	{
		//��带 ã������ �ڽ��� �̹� �˻��� ���·� ����� m_cSearchList���� �����.
		if( !(m_ppcPathFindBlock[PossibilityNode.m_iX][PossibilityNode.m_iY].m_iStatus == AGPD_PATHFIND_BLOCK) )
		{
			m_ppcPathFindBlock[PossibilityNode.m_iX][PossibilityNode.m_iY].m_iStatus = AGPD_PATHFIND_CLOSE;
			m_ppcPathFindBlock[PossibilityNode.m_iX][PossibilityNode.m_iY].m_iRevision = m_iRevision;
		}
	}

	int			iStartX, iStartY;
	int			iCurrentX, iCurrentY;
	float		fParentX, fParentY;

	iStartX = PossibilityNode.m_iX;
	iStartY = PossibilityNode.m_iY;

	if (PossibilityNode.m_iX >= 0 &&
		PossibilityNode.m_iX < m_iWidth &&
		PossibilityNode.m_iY >= 0 &&
		PossibilityNode.m_iY < m_iHeight)
	{
		fParentX = m_ppcPathFindBlock[iStartX][iStartY].m_fCurrentX;
		fParentY = m_ppcPathFindBlock[iStartX][iStartY].m_fCurrentY;
	}

	for( int i=0; i<8; i++ )
	{
		iCurrentX = PossibilityNode.m_iX + m_aArraryX[i];
		iCurrentY = PossibilityNode.m_iY + m_aArraryY[i];

		if (iCurrentX >= 0 &&
			iCurrentX < m_iWidth &&
			iCurrentY >= 0 &&
			iCurrentY < m_iHeight)
		{
			//Revision���� ���� Notused�� �ٲ��� �����Ѵ�.
			if( m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iRevision < m_iRevision )
			{
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus = AGPD_PATHFIND_NOTUSED;
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iPathCost = 0;
			}

			//�ش���ġ�� ���Ǿ����� Ȯ���Ѵ�.
			AuPOS				csPos;
			csPos.x = fParentX + m_aArraryX[i] * AGPMPATHFIND_BLOCKSIZE;
			csPos.z = fParentY + m_aArraryY[i] * AGPMPATHFIND_BLOCKSIZE;

			if( m_papmMap->CheckBlockingInfo( csPos , ApmMap::GROUND ) )
			{
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus = AGPD_PATHFIND_BLOCK;
			}

			//��ŷ�� �ƴ϶��~
			if( m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus != AGPD_PATHFIND_BLOCK )
			{
				//����2-1. ���� ���°� notused�� �ٷ� �մ´�.
				if( m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_NOTUSED )
				{
					//�ڽ��� ���¿� �θ� ��带 �����Ѵ�.
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iRevision = m_iRevision;
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus = AGPD_PATHFIND_OPEN;
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iX = iStartX;
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iY = iStartY;
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_fCurrentX = fParentX + m_aArraryX[i] * AGPMPATHFIND_BLOCKSIZE;
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_fCurrentY = fParentY + m_aArraryY[i] * AGPMPATHFIND_BLOCKSIZE;

					//�������� ����� �����Ѵ�.(��ü��� = �θ��Ǻ��+���� �̾����Ÿ�)
					int				iNewCost;
					int				iPathCost;

					iNewCost = abs(m_aArraryX[i])+abs(m_aArraryY[i]);
					
					if( iNewCost == 1 )
					{
						iNewCost = 10;
					}
					else //if( iNewCost == 2 )
					{
						iNewCost = 15;
					}

					iPathCost = m_ppcPathFindBlock[iStartX][iStartY].m_iPathCost+iNewCost;
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iPathCost = iPathCost;

					//�޸���ƽ�� CPathPoint�� ����ؼ� �ִ´�.
					int				iHuristic;
					int				iHuristicDX,iHuristicDY;
					AgpdPathFindPoint	cPathPoint;

					iHuristicDX = abs(m_cTargetPoint.m_iX - iCurrentX);
					iHuristicDY = abs(m_cTargetPoint.m_iY - iCurrentY );

					//�޸���ƽ�� ���� ���� ������ ���� ������ ���ȴ�.
					//�޸���ƽ�� 10��� �������Ѵ�.
					iHuristic = (iHuristicDX*iHuristicDX + iHuristicDY*iHuristicDY)*20;

					cPathPoint.m_iX = iCurrentX;
					cPathPoint.m_iY = iCurrentY;

					if( m_cSearchList.Priorityinsert( iHuristic + iPathCost, &cPathPoint ) )
					{
						if( (iCurrentX == m_cTargetPoint.m_iX) && (iCurrentY == m_cTargetPoint.m_iY) )
						{
							m_bFound = true;
							break;
						}
					}
					//�� �̻� �������� ���ٴ°� ��~ á�ٴ� �̾߱��~
					else
					{
						return false;
					}
				}
				//����2-2. ���� ���°� open�̸� �̾��� ������ ����� ����� ������ ������ �մ´�.
				else if( (m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_OPEN) || (m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_CLOSE))
				{
					int			iNewCost;
					int			iMyCost, iSearchCost;

					//�ڽ��� �̵�ġ�� ���� ���� ���Ѵ�..
					iNewCost = abs(m_aArraryX[i])+abs(m_aArraryY[i]);

					if( iNewCost == 1 )
					{
						iNewCost = 10;
					}
					else //if( iNewCost == 2 )
					{
						iNewCost = 15;
					}

					//�ڽ��� ����� ���Ѵ�.
					iMyCost = m_ppcPathFindBlock[iStartX][iStartY].m_iPathCost + iNewCost;
					//�˻��� ���� ����� ���Ѵ�.
					iSearchCost = m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iPathCost;

					//���� �˻��� ���� ����� �� �۴ٸ�?
					if( iMyCost < iSearchCost )
					{
						m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iX = iStartX;
						m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iY = iStartY;

						//���� ����� ���(���� ����� Path��....)�� �����Ѵ�.
						m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iPathCost = iMyCost;
					}

					if( (iCurrentX == m_cTargetPoint.m_iX) && (iCurrentY == m_cTargetPoint.m_iY) )
					{
						m_bFound = true;
						break;
					}
				}
			}
		}
	}

	return true;
}

/*
bool AgpmPathFind::pathFind()
{
	//�� �̻� ã�� ��尡 ���ٸ�?
	if( m_cSearchList.m_iIncreaseIndex == 0 )
	{
		return false;
	}

	//���� ������ ��带 ã�´�.
	AgpdPathFindPoint				PossibilityNode;

	//StartNode�� ���� ���� ���� ����!!
	PossibilityNode = *(m_cSearchList.start_node->data);

	//���� ���� ���� ã�Ƴ����� �ǵڷ� ����.
	m_cSearchList.swapHeadAndTail();

	//��带 ã������ �ڽ��� �̹� �˻��� ���·� ����� m_cSearchList���� �����.
	if( !((m_ppcPathFindBlock[PossibilityNode.m_iX][PossibilityNode.m_iY].m_iStatus == AGPD_PATHFIND_HALF) || (m_ppcPathFindBlock[PossibilityNode.m_iX][PossibilityNode.m_iY].m_iStatus == AGPD_PATHFIND_BLOCK)) )
	{
		m_ppcPathFindBlock[PossibilityNode.m_iX][PossibilityNode.m_iY].m_iStatus = AGPD_PATHFIND_CLOSE;
	}

	//���� ������ ��带 �߽����� ����� ���ɴ�.
	int iStartX=PossibilityNode.m_iX, iStartY=PossibilityNode.m_iY;
	int iCurrentX, iCurrentY;

	int aArraryX[8] = { 0, 1, 1, 1, 0,-1,-1,-1 };
	int aArraryY[8] = {-1,-1, 0, 1, 1, 1, 0,-1 };

	for( int i=0; i<8; i++ )
	{
		iCurrentX = PossibilityNode.m_iX + aArraryX[i];
		iCurrentY = PossibilityNode.m_iY + aArraryY[i];

		if( !((m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_BLOCK) || (m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_HALF)) )
		{
			if( m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iRevision < m_iRevision )
			{
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus = AGPD_PATHFIND_NOTUSED;
			}

			//����2-1. ���� ���°� notused�� �ٷ� �մ´�.
			if( m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_NOTUSED )
			{
				//�ڽ��� ���¿� �θ� ��带 �����Ѵ�.
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iRevision = m_iRevision;
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus = AGPD_PATHFIND_OPEN;
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iX = iStartX;
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iY = iStartY;

				//�������� ����� �����Ѵ�.(��ü��� = �θ��Ǻ��+���� �̾����Ÿ�)
				int				iNewCost;
				int				iPathCost;

				iNewCost = abs(aArraryX[i])+abs(aArraryY[i]);
				
				if( iNewCost == 1 )
				{
					iNewCost = 10;
				}
				else //if( iNewCost == 2 )
				{
					iNewCost = 15;
				}

				iPathCost = m_ppcPathFindBlock[iStartX][iStartY].m_iPathCost+iNewCost;
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iPathCost = iPathCost;

				//�޸���ƽ�� CPathPoint�� ����ؼ� �ִ´�.
				int				iHuristic;
				int				iHuristicDX,iHuristicDY;
				AgpdPathFindPoint	cPathPoint;

				iHuristicDX = abs(iCurrentX - m_cTargetPoint.m_iX);
				iHuristicDY = abs(iCurrentY - m_cTargetPoint.m_iY);

				//�޸���ƽ�� ���� ���� ������ ���� ������ ���ȴ�.
				//�޸���ƽ�� 5��� �������Ѵ�.
				iHuristic = (iHuristicDX*iHuristicDX + iHuristicDY*iHuristicDY)*5;

				cPathPoint.m_iX = iCurrentX;
				cPathPoint.m_iY = iCurrentY;

				if( m_cSearchList.Priorityinsert( iHuristic + iPathCost, &cPathPoint ) )
				{
					if( (iCurrentX == m_cTargetPoint.m_iX) && (iCurrentY == m_cTargetPoint.m_iY) )
						m_bFound = true;
				}
				//�� �̻� �������� ���ٴ°� ��~ á�ٴ� �̾߱��~
				else
				{
					return false;
				}
			}
			//����2-2. ���� ���°� open�̸� �̾��� ������ ����� ����� ������ ������ �մ´�.
			else if( (m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_OPEN) || (m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_CLOSE))
			{
				int			iNewCost;
				int			iMyCost, iSearchCost;

				//�ڽ��� �̵�ġ�� ���� ���� ���Ѵ�..
				iNewCost = abs(aArraryX[i])+abs(aArraryY[i]);

				if( iNewCost == 1 )
				{
					iNewCost = 10;
				}
				else //if( iNewCost == 2 )
				{
					iNewCost = 15;
				}

				//�ڽ��� ����� ���Ѵ�.
				iMyCost = m_ppcPathFindBlock[iStartX][iStartY].m_iPathCost + iNewCost;
				//�˻��� ���� ����� ���Ѵ�.
				iSearchCost = m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iPathCost;

				//���� �˻��� ���� ����� �� �۴ٸ�?
				if( iMyCost < iSearchCost )
				{
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iX = iStartX;
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iY = iStartY;

					//���� ����� ���(���� ����� Path��....)�� �����Ѵ�.
					m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iPathCost = iMyCost;
				}

				if( (iCurrentX == m_cTargetPoint.m_iX) && (iCurrentY == m_cTargetPoint.m_iY) )
					m_bFound = true;
			}
		}
		//dHalf�� ��쿡 ���� Ư�� ��ġ!! ^^;
		else if( m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iStatus == AGPD_PATHFIND_HALF )
		{
			if( (iCurrentX == m_cTargetPoint.m_iX) && (iCurrentY == m_cTargetPoint.m_iY) )
			{
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_iRevision = m_iRevision;
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iX = iStartX;
				m_ppcPathFindBlock[iCurrentX][iCurrentY].m_cParent.m_iY = iStartY;

				m_bFound = true;
			}
		}
	}

	return true;
}
*/

bool AgpmPathFind::pathFind( float fStartX, float fStartZ, float fEndX, float fEndZ, CSpecializeListPathFind<int, AgpdPathFindPoint *> *pcPathNodeList )
{
	lock();

	AgpdPathFindPoint	cPathPoint;

	bool				bResult;
	float				fTempStartX, fTempStartZ, fTempEndX, fTempEndZ;

	bResult = false;

	//AGPMPATHFIND_BLOCKSIZE�� 2�����̴�.
	fTempStartX = fStartX/AGPMPATHFIND_BLOCKSIZE;
	fTempStartZ = fStartZ/AGPMPATHFIND_BLOCKSIZE;
	fTempEndX = fEndX/AGPMPATHFIND_BLOCKSIZE;
	fTempEndZ = fEndZ/AGPMPATHFIND_BLOCKSIZE;

	m_cStartPoint.m_iX = AGPMPATHFIND_RADIUS;
	m_cStartPoint.m_iY = AGPMPATHFIND_RADIUS;
	m_cTargetPoint.m_iX = AGPMPATHFIND_RADIUS + (int)fTempEndX - (int)fTempStartX;
	m_cTargetPoint.m_iY = AGPMPATHFIND_RADIUS + (int)fTempEndZ - (int)fTempStartZ;

	if( (m_cStartPoint.m_iX == m_cTargetPoint.m_iX) && (m_cStartPoint.m_iY == m_cTargetPoint.m_iY) )
	{
		bResult = true;
	}
	else
	{
		if( (0 <= m_cTargetPoint.m_iX) && ( m_cTargetPoint.m_iX < (AGPMPATHFIND_RADIUS*2+1) ) &&
			(0 <= m_cTargetPoint.m_iY) && ( m_cTargetPoint.m_iY < (AGPMPATHFIND_RADIUS*2+1) ) )
		{
			//Search List�� �߰�.
			m_cSearchList.resetIndex();
			bResult = m_cSearchList.Priorityinsert( 0, &m_cStartPoint );
			m_ppcPathFindBlock[AGPMPATHFIND_RADIUS][AGPMPATHFIND_RADIUS].m_iPathCost = 0;

			if( bResult )
			{
				float			fDeltaX, fDeltaZ;

				if( fStartX < 0 )
				{
					fDeltaX = -AGPMPATHFIND_BLOCKSIZE/2.0f;
				}
				else
				{
					fDeltaX = AGPMPATHFIND_BLOCKSIZE/2.0f;
				}

				if( fStartZ < 0 )
				{
					fDeltaZ = -AGPMPATHFIND_BLOCKSIZE/2.0f;
				}
				else
				{
					fDeltaZ = AGPMPATHFIND_BLOCKSIZE/2.0f;
				}

				//m_fCurrentX,Z����, Revision����ó��
				m_ppcPathFindBlock[AGPMPATHFIND_RADIUS][AGPMPATHFIND_RADIUS].m_fCurrentX = (int)fStartX/(int)AGPMPATHFIND_BLOCKSIZE*(int)AGPMPATHFIND_BLOCKSIZE + fDeltaX;
				m_ppcPathFindBlock[AGPMPATHFIND_RADIUS][AGPMPATHFIND_RADIUS].m_fCurrentY = (int)fStartZ/(int)AGPMPATHFIND_BLOCKSIZE*(int)AGPMPATHFIND_BLOCKSIZE + fDeltaZ;

				//���� ã�´�.
				while( 1 )
				{
					if( pathFind() == false )
					{
						bResult = false;
						break;
					}

					if( m_bFound )
					{
						if (m_cTargetPoint.m_iX >= 0 &&
							m_cTargetPoint.m_iX < m_iWidth &&
							m_cTargetPoint.m_iY >= 0 &&
							m_cTargetPoint.m_iY < m_iHeight)
						{
							m_ppcPathFindBlock[m_cTargetPoint.m_iX][m_cTargetPoint.m_iY].m_fCurrentX = fEndX;
							m_ppcPathFindBlock[m_cTargetPoint.m_iX][m_cTargetPoint.m_iY].m_fCurrentY = fEndZ;
						}

						pcPathNodeList->resetIndex();
						getPathToExternalList( pcPathNodeList );

						if( m_iRevision == 0xFFFFFFFF )
						{
							resetBlock();
						}

						m_bFound = false;
						bResult = true;

						break;
					}
				}
			}
			else
			{
				//���������� ������ġ�� ����.
				m_cSearchList.resetIndex();
			}
		}
	}

	//���� ã�ƺ������� 1�� ������Ų��.
	m_iRevision++;

	unlock();

	return bResult;
}
