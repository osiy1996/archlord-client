#pragma once
#include "memory_manager.h"
#include <list>
#include <vector>
#include <map>
#include <deque>
#include <hash_map>

namespace umtl
{
	class static_alloc;

	template< typename T >
	class allocator
	{
	public:
		typedef T                 value_type;
		typedef value_type*       pointer;
		typedef const value_type* const_pointer;
		typedef value_type&       reference;
		typedef const value_type& const_reference;
		typedef std::size_t       size_type;
		typedef std::ptrdiff_t    difference_type;

		inline pointer address(reference x) const { return &x; }

		inline const_pointer address(const_reference x) const { 
			return &x;
		}

		inline pointer allocate(size_type n, const_pointer = 0) {
#if !defined(NOT_USE_STATIC_POOL) && !defined(_DEBUG)
			void* p = memory_manager::get().alloc(n * sizeof(T));
#else
			void* p = ::operator new( n * sizeof(T) );
#endif
			return static_cast<pointer>(p);
		}

		inline void deallocate(pointer p, size_type) {
#if !defined(NOT_USE_STATIC_POOL) && !defined(_DEBUG)
			memory_manager::get().free(p);
#else
			::operator delete(p);
#endif
		}

		inline size_type max_size() const { 
			return static_cast<size_type>(-1) / sizeof(value_type);
		}

		inline void construct(pointer p, const value_type& x) { 
			::new(p)value_type(x);
		}
		inline void destroy(pointer p) { p->~value_type(); }

		allocator() {}
		allocator(const allocator&) {}
		template <class U> 
		allocator(const allocator<U>&) {}
		~allocator() {}

		template <class U> 
		struct rebind { typedef allocator<U> other; };

	private:
		void operator=(const allocator&);
	};

	template<> class allocator<void>
	{
		typedef void        value_type;
		typedef void*       pointer;
		typedef const void* const_pointer;

		template <class U> 
		struct rebind { typedef allocator<U> other; };
	};
	
	namespace static_pool
	{
		template< typename T >
		struct vector { typedef std::vector< T, allocator<T> > type; };

		template< typename T >
		struct list { typedef std::list< T, allocator<T> > type; };

		template< typename T >
		struct deque { typedef std::deque< T, allocator<T> > type; };

		template<class Key,
		class Elem,
		class Pr = std::less<Key>,
		class Alloc = allocator<std::pair<const Key, Elem> > >
		struct map { typedef std::map< Key, Elem, Pr, Alloc > type; };

		template<class Key,
		class Elem,
		class Pr = std::less<Key>,
		class Alloc = allocator<std::pair<const Key, Elem> > >
		struct multimap { typedef std::multimap< Key, Elem, Pr, Alloc > type; };

		template<class Key,
		class Elem,
		class Pr = std::hash_compare<Key, std::less<Key> >,
		class Alloc = allocator<std::pair<const Key, Elem> > >
		struct hash_map { typedef std::hash_map< Key, Elem, Pr, Alloc > type; };

		typedef std::basic_string<char, std::char_traits<char>, allocator<char> > string;
		typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t> > wstring;
	}
}

template <class T>
inline bool operator==(const umtl::allocator<T>&, 
	const umtl::allocator<T>&) {
		return true;
}

template <class T>
inline bool operator!=(const umtl::allocator<T>&, 
	const umtl::allocator<T>&) {
		return false;
}