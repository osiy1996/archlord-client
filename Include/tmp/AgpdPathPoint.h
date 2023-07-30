#pragma once

#include <set>


namespace Agpd
{
	//////////////////////////////////////////////////////////////////////////////
	// ��ǥ ������

	struct PathPoint
	{
		enum eTag {
			Normal,
			Orc,
			Elf,
			Human,
			Dragon,
			None
		};

		typedef std::set< PathPoint * > Neighbor;

		// ��ǥ
		float x;
		float y;

		// ����Ǵ� ����
		Neighbor neighbor;

		// �ױ�
		eTag tag;

		// �ε���
		size_t index;

		PathPoint( float x, float y, size_t index )
			: x(x), y(y), tag(Normal), index(index) {}

		// ���� ��� ���� ----
		void InsertNeighbor( PathPoint * point ) {
			neighbor.insert(point);
		}
		void DeleteNeighbor( PathPoint * point ) {
			Neighbor::iterator iter = neighbor.find( point );
			if( iter != neighbor.end() )
				neighbor.erase( iter );
		}
		// -------

		// �� ����Ʈ ������ �Ÿ� ( sqrt ���� ) ---
		inline float dist( PathPoint const & b ) const {
			float dx = (x-b.x);
			float dy = (y-b.y);
			return (dx*dx + dy*dy);
		} // ---------
	};

	//////////////////////////////////////////////////////////////////////////////
}