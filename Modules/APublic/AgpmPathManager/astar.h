#pragma once

//////////////////////////////////////////////////////////////////////////////
// Generic A* - 2010.10.15 kdi
//-----------------------------------------------------------------------
// A* �˰����� �̿��Ͽ� �ִܰ�θ� ã���ϴ�.
// ����� Ÿ���� public ���� ���� ������ ������ ��� �������̽��� �����ؾ� �մϴ�.
//
// float x,y ��ǥ
// float dist( point const & other ) // �Ÿ�
// ������ Ž���� ������ ��� ����Ʈ neighbor
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