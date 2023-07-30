#pragma once

//////////////////////////////////////////////////////////////////////////////
// Generic A* - 2010.10.15 kdi
//-----------------------------------------------------------------------
// A* 알고리즘을 이용하여 최단경로를 찾습니다.
// 노드의 타입은 public 으로 참조 가능한 다음의 멤버 인터페이스를 만족해야 합니다.
//
// float x,y 좌표
// float dist( point const & other ) // 거리
// 순방향 탐색이 가능한 노드 리스트 neighbor
//
//-----------------------------------------------------------------------


#include <set>
#include <map>
#include <functional>
#include <algorithm>


namespace Agpm
{
	//////////////////////////////////////////////////////////////////////////////
	// A* Node

	template< typename PointType >
	struct astar_node;

	template< typename NodeType >
	struct astar_node_less
		: public std::binary_function< NodeType*, NodeType*, bool>
	{
		bool operator()(const NodeType * a, NodeType * b) const {
			if( a && b )
			{
				if( a->f == b->f )
					return a < b;
				else
					return a->f < b->f;
			}
			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////////////
	// A*

	template< typename PointType >
	class astar
	{

	public:

		typedef astar_node< PointType > Node;
		typedef std::set< Node*, astar_node_less<Node> > Nodes;
		typedef std::map< PointType*, Node* > NodeManager;

		astar(size_t max_count = UINT_MAX);
		~astar();




		Node * find( PointType * start, PointType * goal );

		void setMaxCount( size_t count ) { max_count_ = count; }

		size_t getMaxCount() { return max_count_; }













	private:

		struct scan_func : public std::unary_function< PointType*, void > {
			scan_func() {}
			void operator()( PointType * point ) { inst->scan( point ); }
			astar * inst;
		};

		PointType * goal_;
		Node * best_;
		size_t max_count_;
		scan_func scan_func_;
		Nodes openNodes;
		Nodes closedNodes;
		NodeManager nodeManager;


		void scan( PointType * neighbor );

		void insertOpenNode( Node * n );

		void calcNode( Node * n, int g, PointType * goal );

		Node * isOpen( PointType * point );

		Node * isClose( PointType * point );

		Node * getNode( PointType * point );

		void closed_proc( Node* ni );

		void open_proc( Node * ni );

		void new_proc( PointType * neighbor );

		void clear();
	};

	//////////////////////////////////////////////////////////////////////////////
}

#include "astar.hpp"