#include "AgpmPath.h"
#include "AgpdPathPoint.h"
#include "astar.h"
#include <functional>
#include <algorithm>
#include "AuXmlParser/TinyXML/TinyXML.h"

namespace Agpm
{
	//////////////////////////////////////////////////////////////////////////////
	//

	Path::Path(PathOwner::eType owner, size_t index)
		: finder_(new Finder())
		, owner_(owner)
		, index_(index)
		, lastPointIndex_(0)
		, from_( PathPoint::None )
		, to_( PathPoint::None )
		, before_cur_(0)
		, before_goal_(0)
	{}

	//////////////////////////////////////////////////////////////////////////////
	//

	Path::~Path() { clear(); if(finder_) delete finder_; }

	//////////////////////////////////////////////////////////////////////////////
	//

	Agpd::PathPoint * Path::insert( float x, float y, Agpd::PathPoint::eTag tag /*= PathPoint::Normal*/, Agpd::PathPoint * before /*= 0*/ )
	{
		size_t index = makeIndex();

		PathPoint * point = new PathPoint( x, y, index );
		point->tag = tag;

		points_.insert(point);
		indexMap_.insert( std::make_pair( index, point ) );

		if( before )
			link( point, before );

		return point;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::link( Agpd::PathPoint * a, Agpd::PathPoint * b )
	{
		if( a && b && (a!=b) ) {
			a->InsertNeighbor( b );
			b->InsertNeighbor( a );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::del( Agpd::PathPoint * point ) {

		if( point )	{

			struct neighbor_deletor : public std::binary_function< PathPoint*, PathPoint*, void > 
			{
				void operator()( PathPoint * point, PathPoint * neighbor ) const {
					if(point) point->DeleteNeighbor( neighbor );
				}
			};

			// 이 노드가 이웃으로 설정되있는 경우를 제거합니다.
			std::for_each( points_.begin(), points_.end(), std::bind2nd( neighbor_deletor(), point ) );

			// 인덱스 맵에서 제거합니다.
			eraseIndex( point->index );

			// 포인트 리스트에서 제거합니다.
			Points::iterator iter = points_.find( point );
			if( iter != points_.end() ) {
				delete *iter;
				points_.erase( iter );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::clear() 
	{
		struct clear_func : public std::unary_function< PathPoint*, void > {
			void operator()( PathPoint * point ) const { if( point ) delete point; }
		};

		std::for_each( points_.begin(), points_.end(), clear_func() );

		points_.clear();
		indexMap_.clear();
		lastPointIndex_ = 0;

		before_goal_ = 0;
		before_cur_ = 0;
		path_.clear();
	}

	//////////////////////////////////////////////////////////////////////////////
	// tag로 검색 합니다.

	Agpd::PathPoint * Path::find( Agpd::PathPoint::eTag tag ) const
	{
		PathPoint * point = 0;

		for( Points::const_iterator iter = points_.begin(); iter != points_.end(); ++iter )
		{
			if( *iter && (*iter)->tag == tag ) {
				point = *iter;
				break;
			}
		}
		return point;
	}

	//////////////////////////////////////////////////////////////////////////////
	// index 로 검색합니다.

	Agpd::PathPoint * Path::find( size_t index ) const
	{
		IndexMap::const_iterator iter = indexMap_.find( index );
		return ( iter != indexMap_.end() ) ? iter->second : 0;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 가장 가까운 Point를 검색 합니다.

	Agpd::PathPoint * Path::find( float x, float y ) const {

		PathPoint * point = 0;
		float dist = FLT_MAX;
		PathPoint tmp(x,y,0);

		// 노드가 많으면...
		for( Points::const_iterator iter = points_.begin(); iter != points_.end(); ++iter )
		{
			float d = tmp.dist( *(*iter) );
			if( d < dist ) {
				dist = d;
				point = *iter;
			}
		}

		return point;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 목적지 까지의 최단 경로를 반환합니다.

	Path::PointList const & Path::find( Agpd::PathPoint * cur_point, Agpd::PathPoint * goal_point ) const {

		if( (before_cur_ != cur_point) && (before_goal_ != goal_point) )
		{
			path_.clear();

			Finder::Node * node = finder_->find( goal_point, cur_point );

			for( ; node; node = node->prev )
				path_.push_back( node->point );

			before_cur_ = cur_point;
			before_goal_ = goal_point;
		}

		return path_;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 시작점부터 끝점까지 최단경로로 정렬된 포인트 목록을 반환합니다.

	Path::PointList const & Path::getSortedPoints() const
	{
		PathPoint * start = find( from_ );
		PathPoint * end = find( to_ );

		return find( start, end );
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::reIndexing()
	{
		PathPoint * start = find( from_ );

		if( start )
		{
			indexMap_.clear();

			lastPointIndex_ = 0;

			std::set< PathPoint * > complatedPoint;

			reIndexing( start, complatedPoint );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::reIndexing(PathPoint * point, std::set< PathPoint* > & complatedPoint )
	{
		if( point && (complatedPoint.count( point ) == 0) )
		{
			complatedPoint.insert( point );

			point->index = lastPointIndex_;

			indexMap_[lastPointIndex_] = point;

			++lastPointIndex_;


			typedef PathPoint::Neighbor Neighbor;

			Neighbor & neighbor = point->neighbor;

			for( Neighbor::iterator iter = neighbor.begin(); iter != neighbor.end(); ++iter )
			{
				reIndexing( *iter, complatedPoint );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::save( TiXmlDocument & xml )
	{
		if( points_.empty() )
			return;

		reIndexing();

		TiXmlElement * pathData = new TiXmlElement("PATH");

		xml.LinkEndChild(pathData);

		pathData->SetAttribute( "owner", owner_ );
		pathData->SetAttribute( "index", index_ );
		pathData->SetAttribute( "from", from_ );
		pathData->SetAttribute( "to", to_ );


		Points complatedPoints;

		PathPoint * point = find( from_ );

		savePoint( pathData, point, complatedPoints );


		for( Points::iterator iter = points_.begin(); iter != points_.end(); ++iter ) {

			savePoint( pathData, *iter, complatedPoints );
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::savePoint( TiXmlElement * pathData, Agpd::PathPoint * point, Path::Points & complatePoints )
	{
		if( pathData && point && ( complatePoints.count(point) == 0 ) ) {

			TiXmlElement * pointData = new TiXmlElement("POINT");

			pathData->LinkEndChild(pointData);

			pointData->SetDoubleAttribute( "x", point->x );
			pointData->SetDoubleAttribute( "y", point->y );
			pointData->SetAttribute( "tag", point->tag );
			pointData->SetAttribute( "index", point->index );

			
			for( PathPoint::Neighbor::iterator n_iter = point->neighbor.begin();
				n_iter != point->neighbor.end(); ++n_iter ) {

				if( *n_iter ) {

					TiXmlElement * neighbor = new TiXmlElement( "NEIGHBOR" );
					pointData->LinkEndChild( neighbor );
					neighbor->SetAttribute( "index", (*n_iter)->index );
				}
			}

			complatePoints.insert( point );

			for( PathPoint::Neighbor::iterator n_iter = point->neighbor.begin();
				n_iter != point->neighbor.end(); ++n_iter ) 
			{
				savePoint( pathData, *n_iter, complatePoints );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::load( TiXmlNode * xml )
	{
		if( !xml )
			return;

		clear();

		typedef std::set< size_t > NeighborIndices;
		typedef std::map< size_t, NeighborIndices > Indices;
		Indices indices;

		for( TiXmlNode * node = xml->FirstChild("POINT"); node; node = node->NextSibling() ) 
		{
			double x,y;
			int tag,idx;

			node->ToElement()->Attribute("x", &x);
			node->ToElement()->Attribute("y", &y);
			node->ToElement()->Attribute("tag", &tag);
			node->ToElement()->Attribute("index", &idx);

			size_t index = (size_t)idx;

			PathPoint * point = insert( (float)x, (float)y, (PathPoint::eTag)tag );

			if( point )
			{
				eraseIndex( point->index );

				point->index = index;

				indexMap_[index] = point;

				// 마지막 인덱스를 갱신합니다.
				lastPointIndex_ = index > lastPointIndex_ ? index : lastPointIndex_;

				for( TiXmlNode * neighbor = node->FirstChild("NEIGHBOR"); neighbor; neighbor = neighbor->NextSibling() ) 
				{
					neighbor->ToElement()->Attribute("index", &idx);
					indices[point->index].insert((size_t)idx);
				}
			}
		}

		// 이웃 정보 세팅
		Indices::iterator iter = indices.begin();
		for( ; iter != indices.end(); ++iter ) {

			PathPoint * point = find( iter->first );

			NeighborIndices::iterator niter = iter->second.begin();
			for( ; niter != iter->second.end(); ++niter )
			{
				link( point, find(*niter) );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void Path::eraseIndex(size_t index)
	{
		IndexMap::iterator iter = indexMap_.find( index );
		if( iter != indexMap_.end() )
		{
			indexMap_.erase(iter);
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	size_t Path::makeIndex()
	{
		while( indexMap_.count(lastPointIndex_) )
		{
			++lastPointIndex_;
		}
		return lastPointIndex_;
	}

	//////////////////////////////////////////////////////////////////////////////
}

namespace Agpm
{
	//////////////////////////////////////////////////////////////////////////////
	//

	PathManager::PathManager() {}

	//////////////////////////////////////////////////////////////////////////////
	//

	PathManager::~PathManager() {
		clear();
	}

	//////////////////////////////////////////////////////////////////////////////
	// 탐색하고 없으면 생성합니다. 유효하지 않은 인덱스인 경우 새로운 인덱스로 생성합니다.

	Path * PathManager::get( PathOwner::eType ownerType, int index )
	{
		Path * path = 0;

		Paths * paths = get( ownerType );

		if( paths )
		{
			if( index > 0 && paths->size() > (size_t)index )
			{
				path = (*paths)[index];
			}
			else
			{
				path = getEmptyPath(paths);

				if( !path )
				{
					path = new Path( ownerType, paths->size() );

					paths->push_back( path );
				}
			}
		}

		return path;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 패스 목록중 비어있는 패스를 리턴합니다.

	Path * PathManager::getEmptyPath( PathManager::Paths * paths )
	{
		Path * path = 0;

		if( paths )
		{
			for( Paths::iterator iter = paths->begin(); iter != paths->end(); ++iter )
			{
				if( *iter )
				{
					if( (*iter)->getPoints().empty() )
					{
						path = *iter;
						break;
					}
				}
			}
		}

		return path;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	PathManager::Paths * PathManager::get( PathOwner::eType ownerType )
	{
		OwnerMap::iterator iter = paths_.find( ownerType );

		Paths * paths = 0;

		if( ( iter != paths_.end() ) && ( iter->second ) )
		{
			paths = iter->second;
		}
		else
		{
			paths = new Paths();
			paths_[ownerType] = paths;
		}

		return paths;
	}

	//////////////////////////////////////////////////////////////////////////////
	// 목적지 타입으로 검색하여 Paths 목록을 반환합니다.
	// flag == true ? dest = to : dest = from

	void PathManager::find( PathManager::Paths & pathList, PathOwner::eType ownerType, PathPoint::eTag dest, bool flag /* = false */ )
	{
		Paths * paths = get( ownerType );

		for( Paths::iterator iter = paths->begin(); iter != paths->end(); ++iter )
		{
			if( *iter )
			{
				PathPoint::eTag d = ( flag ? ((*iter)->to()) : ((*iter)->from()) );

				if( d == dest )
					pathList.push_back( *iter );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void PathManager::find( PathManager::Paths & pathList, PathOwner::eType ownerType, PathPoint::eTag from, PathPoint::eTag to )
	{
		Paths list;

		find( list, ownerType, from, false );

		for( Paths::iterator iter = list.begin(); iter != list.end(); ++iter )
		{
			if( (*iter)->to() == to )
			{
				pathList.push_back( *iter );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void PathManager::clear() {

		for( OwnerMap::iterator iter = paths_.begin(); iter != paths_.end(); ++iter )
		{
			Paths * paths = iter->second;

			if( paths )
			{
				for( Paths::iterator piter = paths->begin(); piter != paths->end(); ++piter )
				{
					if( *piter )
						delete *piter;
				}

				delete paths;
			}
		}

		paths_.clear();
	}	

	//////////////////////////////////////////////////////////////////////////////
	//

	bool PathManager::save( char const * file_name )
	{
		bool result = false;

		if( file_name )
		{
			TiXmlDocument xml;


			TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "euc-kr", "" );

			xml.LinkEndChild(decl);


			TiXmlElement * body = new TiXmlElement("PathData");

			xml.LinkEndChild(body);


			for( OwnerMap::iterator iter = paths_.begin(); iter != paths_.end(); ++iter ) 
			{
				Paths * paths = iter->second;

				if( paths ) 
				{
					for( Paths::iterator piter = paths->begin(); piter != paths->end(); ++piter )
					{
						if( *piter )
							(*piter)->save( xml );
					}
				}
			}

			if( xml.SaveFile(file_name) )
				result = true;
		}

		return result;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	bool PathManager::load( char const * file_name )
	{
		bool result = false;

		if( file_name )
		{
			clear();

			TiXmlDocument xml( file_name );

			if( xml.LoadFile() ) {

				TiXmlNode * node = 0;

				node = xml.FirstChild( "PathData" );

				node = node ? node->FirstChild( "PATH" ) : xml.FirstChild( "PATH" );

				for( ; node; node = node->NextSibling() ) 
				{
					int owner, index, from, to;
					node->ToElement()->Attribute("owner", &owner);
					node->ToElement()->Attribute("index", &index);
					node->ToElement()->Attribute("from", &from);
					node->ToElement()->Attribute("to", &to);

					Path * path = get( (PathOwner::eType)owner, index );

					path->from( (PathPoint::eTag)from );
					path->to( (PathPoint::eTag)to );

					if( path )
						path->load( node );
				}

				result = true;
			}
		}

		return result;
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	PathManager & PathManager::Instance() {
		static bool isLoad = false;
		static PathManager inst;
		if( !isLoad )
		{
			isLoad = true;
			inst.load( "ini/pathdata.xml" );
		}
		return inst;
	}

	//////////////////////////////////////////////////////////////////////////////
}