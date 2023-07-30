#pragma once

//////////////////////////////////////////////////////////////////////////////
// Generic A* ����


namespace Agpm
{
	//////////////////////////////////////////////////////////////////////////////
	// A* Node

	template< typename PointType >
	struct astar_node
	{
		int g; // �Ÿ��Լ� : ��������� ����ġ���� ����
		float h; // �߰��� �Լ� : ��ǥ������ �Ÿ�
		float f; // ���Լ� : �߰��� �Լ� + �Ÿ� �Լ�

		PointType * point;
		astar_node * prev;

		astar_node() : g(0), h(0), f(0), point(0), prev(0) {}
		astar_node(PointType * point) : g(0), h(0), f(0), point(point), prev(0) {}
	};

	//////////////////////////////////////////////////////////////////////////////
	// A*

	template< typename PointType >
	astar<PointType>::astar(size_t max_count = UINT_MAX) : goal_(0), best_(0), max_count_(max_count) { scan_func_.inst = this; }

	//////////////////////////////////////////////////////////////////////////////
	//

	template< typename PointType >
	astar<PointType>::~astar() { clear(); }

	//////////////////////////////////////////////////////////////////////////////
	//

	template< typename PointType >
	typename astar<PointType>::Node * astar<PointType>::find( PointType * start, PointType * goal )
	{
		if( !start || !goal )
			return 0;

		clear();

		goal_ = goal;

		best_ = getNode(start);

		openNodes.insert( best_ );

		for( size_t count = 0; count < max_count_; ++count )
		{
			if( openNodes.empty() )
				break;

			best_ = *openNodes.begin();

			if( best_->point == goal_ )
				break;

			closedNodes.insert( best_ );

			openNodes.erase( best_ );

			// N ��忡 ������ ������ �˻��մϴ�.
			std::for_each( best_->point->neighbor.begin(), best_->point->neighbor.end(), scan_func_ );
			// ------
		}

		return best_;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template< typename PointType >
	void astar<PointType>::scan( PointType * neighbor )
	{
		Node * ni = 0;

		// ������ ��尡 ������ �� ��带 Ni�� �θ���
		// ni ��尡 Ŭ���� ����Ʈ�� �ִ��� �����Ѵ�

		if( ni = isClose( neighbor ) )
		{
			closed_proc( ni );
		}
		else if( ni = isOpen( neighbor ) ) // ���� ����Ʈ���� Ni ��尡 �ִ��� �����Ѵ�.
		{
			open_proc( ni );
		}
		else
		{
			new_proc( neighbor );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template< typename PointType >
	void astar<PointType>::insertOpenNode( typename astar<PointType>::Node * n )
	{
		if( !n )
			return;

		Nodes::iterator next = openNodes.lower_bound( n );

		if( n != *next ) {

			if( next != openNodes.end() )
			{
				//n->next = *next;
				(*next)->prev = n;
			}

			openNodes.insert(n);
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	void astar<PointType>::calcNode( typename astar<PointType>::Node * n, int g, PointType * goal )
	{
		if( n->h == 0 ) 
			n->h = n->point->dist( *goal );
		n->g = g + 1;
		n->f = n->g + n->h;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	typename astar<PointType>::Node * astar<PointType>::isOpen( PointType * point ) {

		if( !point )
			return 0;

		Node * ni = getNode(point);

		if( openNodes.count( ni ) )
			return ni;

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	typename astar<PointType>::Node * astar<PointType>::isClose( PointType * point ) {

		if( !point )
			return 0;

		Node * ni = getNode(point);

		if( closedNodes.count(ni) )
			return ni;

		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	typename astar<PointType>::Node * astar<PointType>::getNode( PointType * point ) {

		if( !point )
			return 0;

		NodeManager::iterator iter = nodeManager.find( point );

		if( iter == nodeManager.end() ) {
			Node * node = new Node( point );
			nodeManager.insert( std::make_pair( point, node ) );
			return node;
		}

		return iter->second;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	void astar<PointType>::closed_proc( typename astar<PointType>::Node * ni )
	{
		if(!ni)
			return;

		// �����ϰ�. �ѹ� �������� ������..

		/*
		// Ni ��带 N ����� �ڽ����� ���
		//node->next = ni;
		// ���� N�� g + 1 �� Ni�� g ���� �� ���ٸ�
		if( node->g + 1 < ni->g ) {
		// ni �� �θ� n ���� ����� ni�� g �� f �� �ٽ� ����Ѵ�.
		ni->prev = node;
		calcNode( ni, node->g+1, goal );

		int cg = ni->g * + 1;
		// �׸��� ni�� �ڽĵ��� g�� f�� ���� �ٽ� ����Ѵ�.
		for( Node * c = ni->next; c != 0; c = c->next ) {
		calcNode( c, cg++, goal );
		}
		}*/
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	void astar<PointType>::open_proc( typename astar<PointType>::Node * ni )
	{
		if( !ni )
			return;

		// Ni ��带 N ����� �ڽ����� ���
		// N�� g+1�� Ni�� g���� �� ���ٸ�
		// Ni�� �θ�� N��带 ��´�.
		//best_->next = ni;
		if( best_->g + 1 < ni->g ) {
			ni->prev = best_;
			// �׸��� Ni�� g �� f �� �ٽ� ����Ѵ�.
			calcNode( ni, best_->g+1, goal_ );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	void astar<PointType>::new_proc( PointType * neighbor )
	{
		// ni ��带 ���� ����� n�� ���踦 ����� 
		// ���� ����Ʈ�� f�� ���Ͽ� ������������ �Է��Ѵ�.
		Node * ni = getNode( neighbor );

		ni->prev = best_;

		calcNode( ni, best_->g+1, goal_ );

		insertOpenNode( ni );
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	void astar<PointType>::clear()
	{
		openNodes.clear();
		closedNodes.clear();

		goal_ = 0;
		best_ = 0;

		struct clear_func : public std::unary_function< NodeManager::value_type &, void > { 
			void operator()( NodeManager::value_type & elem ) const { if( elem.second ) delete elem.second; }
		};

		std::for_each( nodeManager.begin(), nodeManager.end(), clear_func() );

		nodeManager.clear();
	}

	//////////////////////////////////////////////////////////////////////////////
}