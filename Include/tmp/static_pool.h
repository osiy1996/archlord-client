#pragma once
//#include "memory_manager.h"
#include "AuStaticPool/allocator.hpp"

namespace umtl
{
	inline void clear() { memory_manager::get().clear(); }

	inline size_t size() { return memory_manager::get().current_size(); }

	inline size_t alloc_total() { return memory_manager::get().alloc_total(); }

	inline size_t dealloc_total() { return memory_manager::get().dealloc_total(); }

	class static_alloc
	{
	public :

		inline void * operator new( size_t size )
		{
#if !defined(NOT_USE_STATIC_POOL) && !defined(_DEBUG)
			return memory_manager::get().alloc(size);
#else
			return ::operator new(size);
#endif
		}

		inline void operator delete( void * p )
		{
#if !defined(NOT_USE_STATIC_POOL) && !defined(_DEBUG)
			memory_manager::get().free(p);
#else
			::operator delete(p);
#endif
		}
		/*
		inline void operator delete( void * p, size_t size)
		{
#if !defined(NOT_USE_STATIC_POOL) || defined(_DEBUG)
			memory_manager::get().free(p);
#else
			::operator delete(p,size);
#endif
		}
		*/
		inline void * operator new[](size_t size)
		{
#if !defined(NOT_USE_STATIC_POOL) && !defined(_DEBUG)
			return memory_manager::get().alloc_array(size);
#else
			return ::operator new[](size);
#endif

		}

		inline void operator delete[]( void * p )
		{
#if !defined(NOT_USE_STATIC_POOL) && !defined(_DEBUG)
			memory_manager::get().free_array(p);
#else
			::operator delete[](p);
#endif
		}
	};
}