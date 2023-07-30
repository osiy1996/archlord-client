#include "mapped_pool.h"

namespace umtl
{
	parallel::critical_section mapped_pool::_cts;

	void * mapped_pool::alloc( size_type size )
	{
		char * result = 0;

		mapped_pool & inst = instance();

		{
			lock_section( _cts );

			Pool::iterator iter = inst.pool_.find( size );
			if( iter != inst.pool_.end() )
			{
				result = iter->second;
				inst.pool_.erase(iter);
			}
			else
			{
				result = inst.lastPos_;
				*(size_type*)result = size;
				result += sizeof(size_type);

				inst.lastPos_ += size + sizeof(size_type);
			}
		}

		return result;
	}

	void mapped_pool::free( char * p )
	{
		if( !p )
			return;

		lock_section( _cts );

		mapped_pool & inst = instance();

		size_type size = *((size_type*)p - 1);

		inst.pool_.insert( std::make_pair( size, p ) );
	}


}