#pragma once
#include "AgpdPathPoint.h"
#include <map>
#include <set>
#include <vector>


class TiXmlDocument;
class TiXmlNode;
class TiXmlElement;

namespace Agpd
{
	struct PathPoint;
}


namespace Agpm
{
	//////////////////////////////////////////////////////////////////////////////
	// a* Finder

	template< typename PointType >
	class astar;

	//////////////////////////////////////////////////////////////////////////////
	// path의 타입

	struct PathOwner {
		enum eType {
			Normal,
			BattleGround,
		};
	};

	//////////////////////////////////////////////////////////////////////////////
	// 패스 데이터
	//-----------------------------------------------------------------------
	// 경로를 나타내는 좌표리스트를 가지고 있습니다.
	// 추가적으로 이 패스의 주인 타입과 인덱스 정보를 포함합니다.
	// 포인트의 추가 및 삭제, 검색 등의 기능을 가집니다.
	// 목적지까지의 최단경로를 a*로 검색합니다.
	// xml형식으로 저장 및 로드 기능을 가집니다.
	// 이웃노드를 제한없이 가질 수 있습니다.
	//-----------------------------------------------------------------------

	class Path 
	{
	public:

		typedef Agpd::PathPoint PathPoint;					// 패스 좌표를 저장할 데이터 형식
		typedef std::set< PathPoint * > Points;				// 좌표 목록 형식
		typedef std::map< size_t, PathPoint * > IndexMap;	// 좌표 인덱싱을 위한 맵
		typedef astar< PathPoint > Finder;					// 길찾기 모듈 (a*)
		typedef std::vector< PathPoint * > PointList;		// 길찾기 결과를 넣을 수 있는 리스트 타입


		Path(PathOwner::eType owner, size_t index);

		~Path();		


		//-----------------------------------------------------------------------
		//

		// tag로 검색 합니다.
		Agpd::PathPoint * find( Agpd::PathPoint::eTag tag ) const;

		// index로 검색 합니다.
		Agpd::PathPoint * find( size_t index ) const;

		// 가장 가까운 Point를 검색 합니다.
		Agpd::PathPoint * find( float x, float y ) const;




		// 목적지 까지의 최단 경로를 반환합니다.
		Path::PointList const & find( Agpd::PathPoint * cur_point, Agpd::PathPoint * goal_point ) const;

		// 시작점부터 끝점까지 최단경로로 정렬된 포인트 목록을 반환합니다.
		Path::PointList const & getSortedPoints() const;

		// 정렬되지 않은 모든 포인트 목록 반환
		Points const & getPoints() const { return points_; }





		void save( TiXmlDocument & xml );

		void load( TiXmlNode * xml );




		size_t getIndex() const { return index_; }

		PathOwner::eType getOwner() const { return owner_; }

		inline void from( PathPoint::eTag tag ) { from_ = tag; }

		inline void to( PathPoint::eTag tag ) { to_ = tag; }

		inline PathPoint::eTag from() const { return from_; }

		inline PathPoint::eTag to() const { return to_; }




		//-----------------------------------------------------------------------
		//

		Agpd::PathPoint * insert( float x, float y, Agpd::PathPoint::eTag tag = Agpd::PathPoint::Normal, Agpd::PathPoint * before = 0 );

		void link( Agpd::PathPoint * a, Agpd::PathPoint * b );

		void del( Agpd::PathPoint * point );

		void clear();

		//-----------------------------------------------------------------------
		//





	private:
		Points points_;
		PathOwner::eType owner_;
		size_t index_;
		IndexMap indexMap_;
		size_t lastPointIndex_;
		Finder * finder_;
		PathPoint::eTag from_; // 출발지 태그
		PathPoint::eTag to_; // 목적지 태그
		mutable PointList path_; // 검색된 패스를 담는 버퍼
		mutable PathPoint * before_cur_; // 이전에 검색한 포인트
		mutable PathPoint * before_goal_; // 이전에 검색한 포인트

		// 복사 금지
		Path(Path const & other);
		Path& operator=(Path const & other);

		void eraseIndex(size_t index);
		size_t makeIndex();

		void reIndexing();
		void reIndexing(PathPoint * point, std::set< PathPoint* > & complatedPoint);

		void savePoint( TiXmlElement * pathData, Agpd::PathPoint * point, Path::Points & complatePoints );
	};

	//////////////////////////////////////////////////////////////////////////////
	// 패스 목록을 관리하는 클래스입니다.

	class PathManager
	{
	public:

		typedef Agpd::PathPoint PathPoint;
		typedef std::vector< Path * > Paths;

		PathManager();
		~PathManager();


		// 싱글턴으로 사용할때 사용합니다.
		static PathManager & Instance();


		// 탐색하고 없으면 생성합니다. index가 유효하지 않은 경우 새로운 인덱스로 생성합니다.
		Path * get( PathOwner::eType ownerType, int index );
		PathManager::Paths * get( PathOwner::eType ownerType );


		// 목적지 타입으로 검색하여 Paths 목록을 반환합니다.
		// flag == true ? dest = to : dest = from
		void find( PathManager::Paths & pathList, PathOwner::eType ownerType, PathPoint::eTag dest, bool flag = false );
		void find( PathManager::Paths & pathList, PathOwner::eType ownerType, PathPoint::eTag from, PathPoint::eTag to );


		// xml로 저장
		bool save( char const * file_name );
		// xml 불러오기
		bool load( char const * file_name );


		// 초기화
		void clear();




	private:

		typedef std::map< int, Paths* > OwnerMap;

		OwnerMap paths_;

		Path * getEmptyPath( PathManager::Paths * paths ); // 패스목록중 비어있는 패스를 반환합니다.

		// 복사 금지
		PathManager(PathManager const & other);
		PathManager& operator=(PathManager const & other);
	};

	//////////////////////////////////////////////////////////////////////////////
}

#ifndef _AgpmPathManager
#define _AgpmPathManager \
	Agpm::PathManager::Instance()
#endif