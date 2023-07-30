#pragma once
#include <algorithm>

namespace AuAuthUtill
{
	//-----------------------------------------------------------------------
	//

	inline void	tolower	( std::string & str ) {
		std::for_each( str.begin(), str.end(), std::tolower );
	}

	//-----------------------------------------------------------------------
	//

	template< typename Value >
	struct caster {
		Value operator()( std::string & property ) { return property; }
	};

	//-----------------------------------------------------------------------
	//

#define specialization_caster( value_type, command ) \
	template<> struct caster<value_type> { value_type operator()( std::string & property ) command; }

	//-----------------------------------------------------------------------
	//

	specialization_caster( int, 
		{
			 return property.empty() ? 0 : atoi( property.c_str() );
		}
	);

	//-----------------------------------------------------------------------
	//

	specialization_caster( bool, 
		{
			if( property.empty() )
				return false;

			tolower( property );

			if( property.empty() || property == "false" || property == "0" )
				return false;

			return true;
		}
	);

	//-----------------------------------------------------------------------
	//

	specialization_caster( char const*, 
		{
			return property.c_str();
		}
	);

	//-----------------------------------------------------------------------
	//

	specialization_caster( char*, 
		{
			caster< const char * > ct;
			return const_cast< char* >( ct( property ) );
		}
	);

	//-----------------------------------------------------------------------
	//

#undef specialization_caster

	//-----------------------------------------------------------------------
}