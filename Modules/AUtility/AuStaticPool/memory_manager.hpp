#pragma once
#include <algorithm>
#include <ctime>

namespace umtl
{
	inline memory_manager & memory_manager::get()
	{
		static memory_manager m;
		return m;
	}

	inline size_t memory_manager::current_size()
	{
		Locker l(mutex);
		return pool_size;
	}

	inline size_t memory_manager::alloc_total()
	{
		Locker l(mutex);
		return alloc_cumul;
	}

	inline size_t memory_manager::dealloc_total()
	{
		return dealloc_cumul;
	}

	inline void * memory_manager::alloc( size_t size )
	{
		Locker l(mutex);

		void * p;

		Blocks::iterator i = blocks.find( size );
		//auto i = blocks.upper_bound( size );

		if( i == blocks.end() || i->second.empty() )
		{
			p = new_mem( size );
			if(p)
				alloc_cumul += size;
			else
				throw std::bad_alloc();
		}
		else
		{
			p = search_mem( i );
			//pool_size -= *((size_t*)p - 1);
			pool_size -= size;
		}

		return p;
	}

	inline void memory_manager::free( void * p )
	{
		if(!p) return;

		__int64 address = (__int64)p;

		size_t * rp = (size_t *)(p) - 1;

		{
			Locker l(mutex);

			//blocks[ rp[0] ].push_back( address );

			Blocks::iterator iter = blocks.find( rp[0] );

			if( iter != blocks.end() )
			{
				iter->second.push_back( address );
			}
			else
			{
				Addresss addresss;
				addresss.push_back( address );
				blocks.insert( std::make_pair( rp[0], addresss ) );
			}

			pool_size += rp[0];
		}
	}

	inline void * memory_manager::alloc_array( size_t size )
	{
		return alloc(size);
	}

	inline void memory_manager::free_array( void * p )
	{
		return free(p);
	}

	inline void * memory_manager::search_mem( Blocks::iterator& i )
	{
		void * p = (void*)( i->second.back() );

		i->second.pop_back();

		return p;
	}

	inline void * memory_manager::new_mem( size_t size )
	{
		int try_count = 0;

		void * block = 0;
		
		for(;;)
		{
			block = malloc( size+sizeof(size_t) );

			if( block )
				break;

			if( ++try_count > max_try_count )
				break;

			// delay
			clock_t startTime = clock();
			static const clock_t max_delay_time = 5;
			for(;;)
			{
				if( clock() - startTime > max_delay_time )
					break;
			}
		}

		if( block )
		{
			size_t * s = (size_t*)block;

			s[0] = size;

			block = (void*)(&s[1]);
		}

		return block;
	}

	inline void memory_manager::clear()
	{
		/*
		std::for_each( blocks.begin(), blocks.end(), 
			[]( Blocks::value_type & i ) {
				std::for_each( i.second.begin(), i.second.end(),
					[]( __int64 a ) {
						__int64 p = a - sizeof(size_t);
						::free( (void*)p );
					}
				);
			}
		);
		*/

		for( Blocks::iterator i = blocks.begin(); i != blocks.end(); ++i )
		{
			for( Addresss::iterator a = i->second.begin(); a != i->second.end(); ++a )
			{
				__int64 address = *a;
				__int64 p = address - sizeof(size_t);
				::free( (void*)p );
			}
		}

		blocks.clear();

		dealloc_cumul += pool_size;

		pool_size = 0;
	}
}