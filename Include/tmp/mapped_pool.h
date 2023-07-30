#pragma once
#include "mapped_memory.h"
#include "AuParallel/thread.hpp"
#include <map>
#include <list>
#include <vector>
#include <deque>
#include <hash_map>

namespace umtl
{
	//////////////////////////////////////////////////////////////////////////
	//

	class mapped_pool
	{
	public:
		typedef unsigned int size_type;

		static void * alloc( size_type size );
		static void free( char * p );

	private:
		typedef std::multimap< size_type, char * > Pool;
		Pool pool_;
		mapped_memory memory_;
		char * lastPos_;

		static mapped_pool & instance() {
			static mapped_pool inst;
			return inst;
		}

		static parallel::critical_section _cts;

		mapped_pool()
			: lastPos_( memory_.get() )
		{}
	};

	//////////////////////////////////////////////////////////////////////////
	//

	template< typename T >
	class mapped_allocator
	{
	public:
		typedef T                 value_type;
		typedef value_type*       pointer;
		typedef const value_type* const_pointer;
		typedef value_type&       reference;
		typedef const value_type& const_reference;
		typedef unsigned int      size_type;
		typedef std::ptrdiff_t    difference_type;

		inline pointer address(reference x) const { return &x; }

		inline const_pointer address(const_reference x) const { 
			return &x;
		}

		inline pointer allocate(size_type n, const_pointer = 0) {
			void* p = mapped_pool::alloc(n * sizeof(T));
			return static_cast<pointer>(p);
		}

		inline void deallocate(pointer p, size_type) {
			mapped_pool::free((char*)p);
		}

		inline size_type max_size() const { 
			return static_cast<size_type>(-1) / sizeof(value_type);
		}

		inline void construct(pointer p, const value_type& x) { 
			::new(p)value_type(x);
		}
		inline void destroy(pointer p) { p->~value_type(); }

		mapped_allocator() {}
		mapped_allocator(const mapped_allocator&) {}
		template <class U> 
		mapped_allocator(const mapped_allocator<U>&) {}
		~mapped_allocator() {}

		template <class U> 
		struct rebind { typedef mapped_allocator<U> other; };

	private:
		void operator=(const mapped_allocator&);
	};

	template<> class mapped_allocator<void>
	{
		typedef void        value_type;
		typedef void*       pointer;
		typedef const void* const_pointer;

		template <class U> 
		struct rebind { typedef mapped_allocator<U> other; };
	};

	//////////////////////////////////////////////////////////////////////////
	//

	namespace mapped
	{
		template< typename T >
		struct vector { typedef std::vector< T, mapped_allocator<T> > type; };

		template< typename T >
		struct list { typedef std::list< T, mapped_allocator<T> > type; };

		template< typename T >
		struct deque { typedef std::deque< T, mapped_allocator<T> > type; };

		template<class Key,
		class Elem,
		class Pr = std::less<Key>,
		class Alloc = mapped_allocator<std::pair<const Key, Elem> > >
		struct map { typedef std::map< Key, Elem, Pr, Alloc > type; };

		template<class Key,
		class Elem,
		class Pr = std::hash_compare<Key, std::less<Key> >,
		class Alloc = mapped_allocator<std::pair<const Key, Elem> > >
		struct hash_map { typedef std::hash_map< Key, Elem, Pr, Alloc > type; };

		typedef std::basic_string<char, std::char_traits<char>, mapped_allocator<char> > string;
		typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, mapped_allocator<wchar_t> > wstring;
	}

	//////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////
//

template <class T>
inline bool operator==(const umtl::mapped_allocator<T>&, 
	const umtl::mapped_allocator<T>&) {
		return true;
}

template <class T>
inline bool operator!=(const umtl::mapped_allocator<T>&, 
	const umtl::mapped_allocator<T>&) {
		return false;
}

//////////////////////////////////////////////////////////////////////////