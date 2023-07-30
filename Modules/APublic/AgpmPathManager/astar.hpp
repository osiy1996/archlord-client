#pragma once

//////////////////////////////////////////////////////////////////////////////
// Generic A* 구현


namespace Agpm
{
	//////////////////////////////////////////////////////////////////////////////
	// A* Node

	template< typename PointType >
	struct astar_node
	{
		int g; // 거리함수 : 출발점에서 현위치까지 깊이
		float h; // 발견적 함수 : 목표까지의 거리
		float f; // 평가함수 : 발견적 함수 + 거리 함수

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

			// N 노드에 인접한 노드들을 검사합니다.
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

		// 인접한 노드가 있으면 그 노드를 Ni라 부르고
		// ni 노드가 클로즈 리스트에 있는지 검토한다

		if( ni = isClose( neighbor ) )
		{
			closed_proc( ni );
		}
		else if( ni = isOpen( neighbor ) ) // 오픈 리스트에서 Ni 노드가 있는지 검토한다.
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

		// 간략하게. 한번 닫힌노드는 무시함..

		/*
		// Ni 노드를 N 노드의 자식으로 삼고
		//node->next = ni;
		// 만약 N의 g + 1 이 Ni의 g 보다 더 적다면
		if( node->g + 1 < ni->g ) {
		// ni 의 부모를 n 노드로 만들며 ni의 g 와 f 를 다시 계산한다.
		ni->prev = node;
		calcNode( ni, node->g+1, goal );

		int cg = ni->g * + 1;
		// 그리고 ni의 자식들의 g와 f를 전부 다시 계산한다.
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

		// Ni 노드를 N 노드의 자식으로 삼고
		// N의 g+1이 Ni의 g보다 더 적다면
		// Ni의 부모로 N노드를 삼는다.
		//best_->next = ni;
		if( best_->g + 1 < ni->g ) {
			ni->prev = best_;
			// 그리고 Ni의 g 와 f 를 다시 계산한다.
			calcNode( ni, best_->g+1, goal_ );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	template<typename PointType>
	void astar<PointType>::new_proc( PointType * neighbor )
	{
		// ni 노드를 새로 만들고 n과 관계를 만들며 
		// 오픈 리스트에 f를 비교하여 오름차순으로 입력한다.
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