#ifndef _AU_SPECIALIZE_LIST_FOR_PATHFIND
#define _AU_SPECIALIZE_LIST_FOR_PATHFIND

#include <PLinkedList.h>

//[��ã��� Ư�� �ڷᱸ��]
template<class T, class U>
class CSpecializeListPathFind : public CPriorityList<T, U>
{
public:
	int m_iIncreaseIndex;
	int m_iMaxSwapIndex;

    CPriorityListNode < T, U > *index_end_node;

	//���� �����
    CPriorityListNode < T, U > *m_pcsRealEndNode; //Insert�� �ϸ� ��������� EndNode;
	//���� ��ã�Ⱑ �ǰ��ִ� ���.
	CPriorityListNode < T, U > *m_pcsCurrentPathNode;

//Static Priority Swap List
	CSpecializeListPathFind();
	~CSpecializeListPathFind();
	bool insert(const T & KEY, const U & DATA);
	bool Priorityinsert(const T & KEY, const U & DATA);
	CPriorityListNode < T, U > *swapHeadAndTail();
	void resetIndex();
	void allocSwapBuffer( int iMaxNum );
	void deleteSwapBuffer();
	void removeSwapBuffer();

	CPriorityListNode < T, U > *GetNextPath();
};

template < class T, class U > 
CSpecializeListPathFind < T, U >::CSpecializeListPathFind()
{
    // ó������ ���۰� ���� ��� t_node��
	m_iIncreaseIndex = 0;
	m_iMaxSwapIndex = 0;
	index_end_node = NULL;
    m_pcsRealEndNode = NULL;
	m_pcsCurrentPathNode = NULL;
}

template < class T, class U > 
CSpecializeListPathFind < T, U >::~CSpecializeListPathFind()
{
    removeAllNode();
}

template < class T, class U >
void CSpecializeListPathFind < T,U >::allocSwapBuffer( int iMaxNum )
{
	lock();
	m_iMaxSwapIndex = iMaxNum;
	unlock();

	for( int i=0; i< iMaxNum; i++ )
	{
		U		data;

		data = new AgpdPathFindPoint;

		//���� ���۸� ����� ���̹Ƿ� �׳� insert�� ���°� �´�.
		insertNode( 0, data );
	}

}

template < class T, class U >
void CSpecializeListPathFind < T,U >::removeSwapBuffer()
{

	removeAllNode();

	lock();

	m_iIncreaseIndex = 0;
	m_iMaxSwapIndex = 0;
	index_end_node = NULL;
	m_pcsRealEndNode = NULL;
	m_pcsCurrentPathNode = NULL;

	unlock();
}

template < class T, class U >
void CSpecializeListPathFind < T,U >::resetIndex()
{
	lock();
	m_iIncreaseIndex = 0;
	index_end_node = NULL;
	m_pcsRealEndNode = NULL;
	m_pcsCurrentPathNode = NULL;
	unlock();
}

template < class T, class U > 
CPriorityListNode < T, U > *CSpecializeListPathFind < T,U >::swapHeadAndTail()
{
	lock();

	CPriorityListNode < T, U > *pcResultNode;

	//�ƹ��͵� ���� ���¿��� Drop and insert to tail�� �ǹ̰� ����.
	//�ּ��� 1���� ����־���Ѵ�. �׸��� �ε��� ���۵� 1�� �̻��̾���Ѵ�.
	if( (m_iIncreaseIndex < 1) || (m_iMaxSwapIndex < 1) )
	{
		pcResultNode = NULL;
	}
	else
	{
		//���� 1���� ���� �̷���� ����Ʈ���?
		if( m_iMaxSwapIndex == 1 )
		{
			pcResultNode = start_node;
			m_iIncreaseIndex--;
		}
		//
		else
		{
			//���� �۾� ��尡 ���۳��� ���� ��带 �۾� ���� ���س��´�.
			if( index_end_node == start_node )
			{
				index_end_node = start_node->NextNode;
			}

			CPriorityListNode < T, U > *pcTmpNode;
			pcTmpNode = start_node;

			//��带 �����.
			start_node = pcTmpNode->NextNode;
			pcTmpNode->NextNode->PrevNode = NULL;

			//���� ������ ��忡 �ٿ��ִ´�.
			pcTmpNode->PrevNode = end_node;
			pcTmpNode->NextNode = NULL;
			pcTmpNode->PrevNode->NextNode = pcTmpNode;

			end_node = pcTmpNode;

			pcResultNode = pcTmpNode;
			m_iIncreaseIndex--;
		}
	}

	unlock();

	return pcResultNode;
}

template < class T, class U > 
bool CSpecializeListPathFind < T,U >::insert(const T & KEY, const U & DATA)
{
	bool			bResult;

	bResult = false;

	lock();

	CPriorityListNode < T, U > *pcTmpNode;

	//���ۿ� ������ �ִ� �ִ� ������ �Ѿ��.
	if( m_iIncreaseIndex >= m_iMaxSwapIndex )
	{
		unlock();
		return bResult;
	}

	//End ��带 ã�´�.( ���� �ٲٰ� ����� ���� )
	pcTmpNode = end_node;

	//pcTmpNode�� �˻��Ѵ�.
	if( pcTmpNode == NULL )
	{
		unlock();
		return bResult;
	}
	else
	{
		pcTmpNode->key = KEY;
		pcTmpNode->data->m_iX = DATA->m_iX;
		pcTmpNode->data->m_iY = DATA->m_iY;
		pcTmpNode->data->m_fX = DATA->m_fX;
		pcTmpNode->data->m_fY = DATA->m_fY;
	}

	//Swap �ϱ����� EndNode�� �����.
	end_node = pcTmpNode->PrevNode;
	pcTmpNode->PrevNode->NextNode = NULL;

	//���� �� ��忡 ��� �����͸� �ִ´�.
	CPriorityListNode < T, U > *pcCurrentNode = start_node;

	pcTmpNode->PrevNode = NULL;
	pcTmpNode->NextNode = pcCurrentNode;
	pcCurrentNode->PrevNode = pcTmpNode;

	start_node = pcTmpNode;

	//���������� Swap�Ǿ����� index�� ������Ų��.

	if( m_iIncreaseIndex == 0 )
	{
		m_pcsRealEndNode = pcTmpNode;
	}

	m_iIncreaseIndex++;
	bResult = true;

	unlock();

	return bResult;
}

template < class T, class U > 
bool CSpecializeListPathFind < T,U >::Priorityinsert(const T & KEY, const U & DATA)
{
	bool			bResult;

	bResult = false;

	lock();

	CPriorityListNode < T, U > *pcTmpNode;

	//���ۿ� ������ �ִ� �ִ� ������ �Ѿ��.
	if( m_iIncreaseIndex >= m_iMaxSwapIndex )
	{
		unlock();
		return bResult;
	}

	//Swap�� ��带 ã�´�.
	if( index_end_node == NULL )
	{
		pcTmpNode = start_node;
		index_end_node = start_node->NextNode;
	}
	else
	{
		pcTmpNode = index_end_node;
		index_end_node = pcTmpNode->NextNode;
	}

	//pcTmpNode�� �˻��Ѵ�.
	if( pcTmpNode == NULL )
	{
		unlock();
		return bResult;
	}
	else
	{
		pcTmpNode->key = KEY;
		pcTmpNode->data->m_iX = DATA->m_iX;
		pcTmpNode->data->m_iY = DATA->m_iY;
	}

	//Swap �ϱ����� ��带 �����.
	if( pcTmpNode == start_node )
	{
		start_node = pcTmpNode->NextNode;
		pcTmpNode->NextNode->PrevNode = NULL;
	}
	else if( pcTmpNode == end_node )
	{
		end_node = pcTmpNode->PrevNode;
		pcTmpNode->PrevNode->NextNode = NULL;
	}
	else
	{
		pcTmpNode->PrevNode->NextNode = pcTmpNode->NextNode;
		pcTmpNode->NextNode->PrevNode = pcTmpNode->PrevNode;
	}

	//���� �� ��忡 �ִ°��.
	if( m_iIncreaseIndex == 0 )
	{
		pcTmpNode->PrevNode = NULL;
		pcTmpNode->NextNode = start_node;
		start_node->PrevNode = pcTmpNode;

		start_node = pcTmpNode;
	}
	else
	{
		bool		bFound = false;
		CPriorityListNode < T, U > *pcCurrentNode = start_node;

		for( int iCounter = 0; iCounter < m_iIncreaseIndex; iCounter++ )
		{
			if( KEY < pcCurrentNode->key )
			{
				//���� �տ� ���� ���
				if( pcCurrentNode == start_node )
				{
					//Ư�� ��ġ�� �ִ´�.
					pcTmpNode->PrevNode = NULL;
					pcTmpNode->NextNode = pcCurrentNode;
					pcCurrentNode->PrevNode = pcTmpNode;

					start_node = pcTmpNode;
				}
				//����� �ٴ°��
				else
				{
					pcCurrentNode->PrevNode->NextNode = pcTmpNode;
					pcTmpNode->PrevNode = pcCurrentNode->PrevNode;
					pcTmpNode->NextNode = pcCurrentNode;
					pcCurrentNode->PrevNode = pcTmpNode;
				}

				bFound = true;
				break;
			}

			pcCurrentNode = pcCurrentNode->NextNode;
		}

		//���� ���� �ٴ� ���
		if( bFound == false )
		{
			if( (pcCurrentNode == end_node) || (pcCurrentNode == NULL))
			{
				pcTmpNode->PrevNode = end_node;
				pcTmpNode->NextNode = NULL;
				pcTmpNode->PrevNode->NextNode = pcTmpNode;

				end_node = pcTmpNode;
			}
			else
			{
				//Ư�� ��ġ�� �ִ´�.
				pcCurrentNode->PrevNode->NextNode = pcTmpNode;
				pcTmpNode->PrevNode = pcCurrentNode->PrevNode;
				pcTmpNode->NextNode = pcCurrentNode;
				pcCurrentNode->PrevNode = pcTmpNode;
			}
		}
	}

	//���������� Swap�Ǿ����� index�� ������Ų��.
	m_iIncreaseIndex++;
	bResult = true;

	unlock();

	return bResult;
}

template < class T, class U > 
CPriorityListNode < T, U > *CSpecializeListPathFind < T,U >::GetNextPath()
{
	CPriorityListNode < T, U > *pcsResultNode;

	lock();

	if( m_pcsCurrentPathNode == m_pcsRealEndNode )
	{
		pcsResultNode = NULL;
	}
	else
	{
		if( m_pcsCurrentPathNode == NULL )
		{
			m_pcsCurrentPathNode = start_node;
		}
		else 
		{
			m_pcsCurrentPathNode = m_pcsCurrentPathNode->NextNode;
		}

		pcsResultNode = m_pcsCurrentPathNode;
	}

	unlock();

	return pcsResultNode;
}

#endif