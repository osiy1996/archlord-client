#pragma once

#include <set>


namespace Agpd
{
	//////////////////////////////////////////////////////////////////////////////
	// 좌표 데이터

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

		// 좌표
		float x;
		float y;

		// 연결되는 노드들
		Neighbor neighbor;

		// 테그
		eTag tag;

		// 인덱스
		size_t index;

		PathPoint( float x, float y, size_t index )
			: x(x), y(y), tag(Normal), index(index) {}

		// 인접 노드 설정 ----
		void InsertNeighbor( PathPoint * point ) {
			neighbor.insert(point);
		}
		void DeleteNeighbor( PathPoint * point ) {
			Neighbor::iterator iter = neighbor.find( point );
			if( iter != neighbor.end() )
				neighbor.erase( iter );
		}
		// -------

		// 두 포인트 사이의 거리 ( sqrt 생략 ) ---
		inline float dist( PathPoint const & b ) const {
			float dx = (x-b.x);
			float dy = (y-b.y);
			return (dx*dx + dy*dy);
		} // ---------
	};

	//////////////////////////////////////////////////////////////////////////////
}