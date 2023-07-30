#include <ApBase/MagDebug.h>
#include "AuOTree.h"
#include <ApMemoryTracker/ApMemoryTracker.h>

AuOTree::AuOTree() : m_nType ( NOT_READY ) , m_pParent ( NULL )
{
	// �ʱ�ȭ����..
}

AuOTree::~AuOTree()
{
	// 
	DeleteLeaf();
}

// Parn �۾�, Destroy�� �ʿ��ϴ�. �� ����� NOT_READY ���·�
void		AuOTree::Destroy		()
{
	DeleteLeaf();

	m_nType = NOT_READY;
	m_pParent = NULL;
}

AuOTree *	AuOTree::GetRoot		() const
{
	ASSERT( GetType() == LEAF || GetType() == BRANCH );

	AuOTree * pParent = ( AuOTree * ) this;

	// ��Ʈ�� ã��..
	while( pParent->GetParent() )
	{
		pParent = GetParent();
	}

	return pParent;
}

BOOL		AuOTree::Init		( float x , float y , float z , float width , float unitsize , BOOL bBlock )
{
	ASSERT( GetType() == NOT_READY );
	if( GetType() != NOT_READY )
	{
		return FALSE;
	}

	m_fStartX			= x			;
	m_fStartY			= y			;
	m_fStartZ			= z			;
	m_fWidth			= width		;
	m_fUnitSize			= unitsize	;

	m_nType				= LEAF		;
	m_pParent			= NULL		;	// ��Ʈ�� �����ϱ�.

	m_Data.bBlocking	= bBlock	;

	return TRUE;
}


void		AuOTree::Optimize	()
{
	// Do no op yet...
	ASSERT( GetType() == LEAF || GetType() == BRANCH );

	if( GetType() == LEAF )
	{
		// do no op..
		return;
	}
	else
	{
		// BRANCH
		ASSERT( NULL != m_Data.pLeaf );

		int		countLeaf = 0	;
		int		nBlocking = 0	;

		// Leaf ������ 
		for( int i = 0 ; i < 8 ; ++i )
		{
			ASSERT( LEAF == m_Data.pLeaf[ i ].GetType() || BRANCH == m_Data.pLeaf[ i ].GetType() );

			// BRANCH���.. ��Ƽ����� ������..
			if( BRANCH == m_Data.pLeaf[ i ].GetType() )
				m_Data.pLeaf[ i ].Optimize();

			// ��Ƽ������ �� ����� ����..
			if( LEAF == m_Data.pLeaf[ i ].GetType() )
			{
				++countLeaf ;

				// Blocking �ϰ�� 
				if( m_Data.pLeaf[ i ].m_Data.bBlocking )
					++nBlocking ;
			}
			else
			{
				// BRANCH
				// Do no op
			}
		}

		// ���δ� Leaf��..
		if( countLeaf == 8 )
		{
			if( nBlocking == 8 )
			{
				// ���� TRUE
				DeleteLeaf( TRUE	);
			}
			else if( nBlocking == 0 )
			{
				// ���� FALSE
				DeleteLeaf( FALSE	);
			}
		}

		// ���� Leaf�� �ƴϸ� �ƹ��� ����..
		return;
	}
}

BOOL		AuOTree::CreateLeaf	(	BOOL bBlock1,
										BOOL bBlock2,
										BOOL bBlock3,
										BOOL bBlock4,
										BOOL bBlock5,
										BOOL bBlock6,
										BOOL bBlock7,
										BOOL bBlock8
									)
{
	ASSERT( GetType() == LEAF		);
	ASSERT( m_fWidth > m_fUnitSize	);

	if( ( GetType() != LEAF			) ||
		( m_fWidth <= m_fUnitSize	)	)
	{
		// �ӽö�� -_-;;
		return FALSE;
	}

	AuOTree *	pTree	= new AuOTree[ 8 ]	;
	float		fWidth	= m_fWidth / 2.0f		;

	ASSERT( NULL != pTree );

	// Set Up Leaves..

	pTree[ 0 ].SetUpLeaf(	m_fStartX			,	m_fStartY			,	m_fStartZ			, fWidth , m_fUnitSize , bBlock1 , this );
	pTree[ 1 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY			,	m_fStartZ			, fWidth , m_fUnitSize , bBlock2 , this );
	pTree[ 2 ].SetUpLeaf(	m_fStartX			,	m_fStartY			,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock3 , this );
	pTree[ 3 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY			,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock4 , this );
	pTree[ 4 ].SetUpLeaf(	m_fStartX			,	m_fStartY + fWidth	,	m_fStartZ			, fWidth , m_fUnitSize , bBlock5 , this );
	pTree[ 5 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY + fWidth	,	m_fStartZ			, fWidth , m_fUnitSize , bBlock6 , this );
	pTree[ 6 ].SetUpLeaf(	m_fStartX			,	m_fStartY + fWidth	,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock7 , this );
	pTree[ 7 ].SetUpLeaf(	m_fStartX + fWidth	,	m_fStartY + fWidth	,	m_fStartZ + fWidth	, fWidth , m_fUnitSize , bBlock8 , this );

	// �귻ġ�� �����..
	m_nType			= BRANCH	;
	m_Data.pLeaf	= pTree		;

	return TRUE;
}

// Parn �۾��� CreateLeafAll()
BOOL		AuOTree::CreateLeafAll	()
{
	ASSERT( GetType() == LEAF		);
	ASSERT( m_fWidth > m_fUnitSize	);

	if( ( GetType() != LEAF			) ||
		( m_fWidth  <= m_fUnitSize	)	)
	{
		// �ӽö�� -_-;;
		return FALSE;
	}

	if ( !CreateLeaf() )
		return FALSE;

	for( int i = 0 ; i < 8 ; ++i )
	{
		m_Data.pLeaf[ i ].CreateLeafAll();
	}

	return TRUE;
}

void		AuOTree::SetUpLeaf	( float x , float y , float z , float width , float unitsize , BOOL bBlock , AuOTree *pParent )
{
	ASSERT( GetType() == NOT_READY );

	m_fStartX			= x			;
	m_fStartY			= y			;
	m_fStartZ			= z			;
	m_fWidth			= width		;
	m_fUnitSize			= unitsize	;

	m_nType				= LEAF		;
	m_pParent			= pParent	;	// ��Ʈ�� �����ϱ�.

	m_Data.bBlocking	= bBlock	;
}

BOOL		AuOTree::DeleteLeaf	(	BOOL bBlock  )
{
	// �ʱ�ȭ �ž� �ִ��� ����..
	ASSERT( GetType() == LEAF || GetType() == BRANCH );
	if( GetType() != LEAF && GetType() != BRANCH ) return FALSE;

	if( GetType() == LEAF )
	{
		m_Data.bBlocking = bBlock;
		return TRUE;
	}
	else
	{
		// BRANCH
		ASSERT( NULL != m_Data.pLeaf );

		for( int i = 0 ; i < 8 ; ++i )
		{
			m_Data.pLeaf[ i ].DeleteLeaf( bBlock );
		}
		// �޸� ����.
		delete [] m_Data.pLeaf;

		m_nType				= LEAF		;
		m_Data.bBlocking	= bBlock	;

		return TRUE;
	}

}

BOOL		AuOTree::SetBlocking	( BOOL bBlock					)
{
	// LEAF������ ������ �����ϵ���.
	ASSERT( GetType() == LEAF );
	if( GetType() != LEAF )
	{
		// -_-;;;..

		return FALSE;
	}

	BOOL	bPrev		= m_Data.bBlocking	;
	m_Data.bBlocking	= bBlock			;
	return bPrev;
}

// Parn �۾�, Blocking ��Ҹ� �־ ���� LEAF ��忡 ���� ����� �Ѵ�.
BOOL		AuOTree::AddBlocking	( AuBLOCKING *pstBlocking		)
{
	ASSERT( NULL != pstBlocking );

	// Leaf�̸�, ���� �簢���� ������ Blocking���� �ƴ����� Test�ؼ� Blocking������ �����.
	if( GetType() == LEAF )
	{
		AuPOS	center;

		center.x = m_fStartX + (float) ( m_fWidth / 2.0 );
		center.y = m_fStartY + (float) ( m_fWidth / 2.0 );
		center.z = m_fStartZ + (float) ( m_fWidth / 2.0 );

		switch( pstBlocking->type )
		{
		case AUBLOCKING_TYPE_BOX:
			m_Data.bBlocking	= AUTEST_POS_IN_BOX(center, pstBlocking->data.box);
			break;
		case AUBLOCKING_TYPE_SPHERE:
			m_Data.bBlocking	= AUTEST_POS_IN_SPHERE(center, pstBlocking->data.sphere);
			break;
		case AUBLOCKING_TYPE_CYLINDER:
			m_Data.bBlocking	= AUTEST_POS_IN_CYLINDER(center, pstBlocking->data.cylinder);
			break;
		}

		return TRUE;
	}

	// Leaf�� �ƴϸ�, recursive�ϰ� ��� Call
	for( int i = 0 ; i < 8 ; ++i )
		if( m_Data.pLeaf )
			m_Data.pLeaf[ i ].AddBlocking( pstBlocking );

	return TRUE;
}

AuOTree *	AuOTree::GetLeaf		( int index 					) const
{
	// �귻ġ���� �ϰ� , �ε����� ������ 8 �����ο����Ѵ�.
	ASSERT( GetType() == BRANCH		);
	ASSERT( index >= 0 && index < 8	);
	if( GetType() != BRANCH || index < 0 || index >= 8 ) return NULL;

	return m_Data.pLeaf + index;
}

BOOL		AuOTree::GetBlocking	( float x , float y , float z	) const
{
	ASSERT( GetType() == LEAF || GetType() == BRANCH );
	if( GetType() != LEAF && GetType() != BRANCH ) return FALSE;

	AuOTree	* pLeaf			;

	pLeaf	= GetLeaf();

	ASSERT( NULL != pLeaf );

	for( int i = 0 ; i < 8 ; ++i )
	{
		if( pLeaf[ i ].IsInThisBox( x , y , z ) )
		{
			return pLeaf[ i ].GetBlocking( x , y , z );
		}
	}

	ASSERT( !"��Ʈ�� ������ �̻��մϴ�. ��ŷ ������ ã������ ����." );

	return FALSE;
}
