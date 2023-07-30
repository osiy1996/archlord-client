#pragma once
//#include <boost/thread.hpp>
//#include <process.h>
//#include <hash_map>
#include <WinSock2.h>
#include <map>
#include <vector>
#include <iostream>

namespace umtl
{
	class memory_manager
	{
	public:
		//typedef boost::recursive_mutex Mutex;
		typedef CRITICAL_SECTION Mutex;
		//typedef boost::lock_guard<Mutex> Locker;
		typedef struct cLocker { 
			cLocker(CRITICAL_SECTION & c) : ct(c) { EnterCriticalSection(&ct); }
			~cLocker() { LeaveCriticalSection(&ct); }
			CRITICAL_SECTION & ct;
		} Locker;

		typedef std::vector<__int64> Addresss;
		typedef std::map<size_t,Addresss> Blocks;

		inline void * alloc( size_t size );
		inline void free( void * p );
		inline void * alloc_array( size_t size );
		inline void free_array( void * p );
		inline void clear();
		inline size_t current_size();
		inline size_t alloc_total();
		inline size_t dealloc_total();
		inline static memory_manager & get();

		~memory_manager() { 
			clear();
			DeleteCriticalSection( &mutex );
		}
		

	private:
		Mutex mutex;
		Blocks blocks;
		size_t pool_size;
		size_t alloc_cumul;
		size_t dealloc_cumul;

		inline void * search_mem( Blocks::iterator& i );
		inline void * new_mem( size_t size );

		static void outOfMem()
		{
			std::cerr<< "Unable to satisfy request for memory\n";
			//std::abort();
			throw std::bad_alloc();
		}

		memory_manager() : pool_size(0), alloc_cumul(0), dealloc_cumul(0) { 
			InitializeCriticalSection(&mutex);
			std::set_new_handler( outOfMem );
		}

		static const int max_try_count = 10000;
	};


}

#include "AuStaticPool/memory_manager.hpp"