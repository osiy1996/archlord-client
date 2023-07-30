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
	// path�� Ÿ��

	struct PathOwner {
		enum eType {
			Normal,
			BattleGround,
		};
	};

	//////////////////////////////////////////////////////////////////////////////
	// �н� ������
	//-----------------------------------------------------------------------
	// ��θ� ��Ÿ���� ��ǥ����Ʈ�� ������ �ֽ��ϴ�.
	// �߰������� �� �н��� ���� Ÿ�԰� �ε��� ������ �����մϴ�.
	// ����Ʈ�� �߰� �� ����, �˻� ���� ����� �����ϴ�.
	// ������������ �ִܰ�θ� a*�� �˻��մϴ�.
	// xml�������� ���� �� �ε� ����� �����ϴ�.
	// �̿���带 ���Ѿ��� ���� �� �ֽ��ϴ�.
	//-----------------------------------------------------------------------

	class Path 
	{
	public:

		typedef Agpd::PathPoint PathPoint;					// �н� ��ǥ�� ������ ������ ����
		typedef std::set< PathPoint * > Points;				// ��ǥ ��� ����
		typedef std::map< size_t, PathPoint * > IndexMap;	// ��ǥ �ε����� ���� ��
		typedef astar< PathPoint > Finder;					// ��ã�� ��� (a*)
		typedef std::vector< PathPoint * > PointList;		// ��ã�� ����� ���� �� �ִ� ����Ʈ Ÿ��


		Path(PathOwner::eType owner, size_t index);

		~Path();		


		//-----------------------------------------------------------------------
		//

		// tag�� �˻� �մϴ�.
		Agpd::PathPoint * find( Agpd::PathPoint::eTag tag ) const;

		// index�� �˻� �մϴ�.
		Agpd::PathPoint * find( size_t index ) const;

		// ���� ����� Point�� �˻� �մϴ�.
		Agpd::PathPoint * find( float x, float y ) const;




		// ������ ������ �ִ� ��θ� ��ȯ�մϴ�.
		Path::PointList const & find( Agpd::PathPoint * cur_point, Agpd::PathPoint * goal_point ) const;

		// ���������� �������� �ִܰ�η� ���ĵ� ����Ʈ ����� ��ȯ�մϴ�.
		Path::PointList const & getSortedPoints() const;

		// ���ĵ��� ���� ��� ����Ʈ ��� ��ȯ
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
		PathPoint::eTag from_; // ����� �±�
		PathPoint::eTag to_; // ������ �±�
		mutable PointList path_; // �˻��� �н��� ��� ����
		mutable PathPoint * before_cur_; // ������ �˻��� ����Ʈ
		mutable PathPoint * before_goal_; // ������ �˻��� ����Ʈ

		// ���� ����
		Path(Path const & other);
		Path& operator=(Path const & other);

		void eraseIndex(size_t index);
		size_t makeIndex();

		void reIndexing();
		void reIndexing(PathPoint * point, std::set< PathPoint* > & complatedPoint);

		void savePoint( TiXmlElement * pathData, Agpd::PathPoint * point, Path::Points & complatePoints );
	};

	//////////////////////////////////////////////////////////////////////////////
	// �н� ����� �����ϴ� Ŭ�����Դϴ�.

	class PathManager
	{
	public:

		typedef Agpd::PathPoint PathPoint;
		typedef std::vector< Path * > Paths;

		PathManager();
		~PathManager();


		// �̱������� ����Ҷ� ����մϴ�.
		static PathManager & Instance();


		// Ž���ϰ� ������ �����մϴ�. index�� ��ȿ���� ���� ��� ���ο� �ε����� �����մϴ�.
		Path * get( PathOwner::eType ownerType, int index );
		PathManager::Paths * get( PathOwner::eType ownerType );


		// ������ Ÿ������ �˻��Ͽ� Paths ����� ��ȯ�մϴ�.
		// flag == true ? dest = to : dest = from
		void find( PathManager::Paths & pathList, PathOwner::eType ownerType, PathPoint::eTag dest, bool flag = false );
		void find( PathManager::Paths & pathList, PathOwner::eType ownerType, PathPoint::eTag from, PathPoint::eTag to );


		// xml�� ����
		bool save( char const * file_name );
		// xml �ҷ�����
		bool load( char const * file_name );


		// �ʱ�ȭ
		void clear();




	private:

		typedef std::map< int, Paths* > OwnerMap;

		OwnerMap paths_;

		Path * getEmptyPath( PathManager::Paths * paths ); // �н������ ����ִ� �н��� ��ȯ�մϴ�.

		// ���� ����
		PathManager(PathManager const & other);
		PathManager& operator=(PathManager const & other);
	};

	//////////////////////////////////////////////////////////////////////////////
}

#ifndef _AgpmPathManager
#define _AgpmPathManager \
	Agpm::PathManager::Instance()
#endif