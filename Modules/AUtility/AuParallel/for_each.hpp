#pragma once
#include <algorithm>
#include <cmath>
//#include <vector>
#include <exception>
#include "thread.hpp"

namespace parallel
{
	//////////////////////////////////////////////////////////////////////////////
	//
/*
	template< typename Iter, typename Func >
	class for_each_functor
	{
		Iter first_;
		Iter last_;
		Func func_;
	public:
		for_each_functor( Iter first, Iter last, Func func ) : first_(first), last_(last), func_(func) {}
		void operator()() { std::for_each( first_, last_, func_ ); }
	};

	//////////////////////////////////////////////////////////////////////////////
	//

	template<class Iter, class Func>
	void for_each(Iter first, Iter last, size_t iter_count, Func func, size_t thread_num)
	{
		if( thread_num < 1 )
			return;

		if( iter_count < 1 )
			return;

		typedef for_each_functor<Iter, Func> functor;
		typedef thread< functor > thread;
		
		std::vector<thread*> threads;		
		threads.reserve(thread_num);

		float iter_count_per_thread = iter_count / (float)thread_num;
		float before_idx=0;
		float cur_idx=0;
		float idx=0;

		for(size_t i=0;i<thread_num;++i)
		{
			cur_idx += iter_count_per_thread;

			if( cur_idx > iter_count )
				cur_idx = (float)iter_count;

			idx = floor( cur_idx + 0.5f ) - floor( before_idx + 0.5f );

			last = first+(int)(idx);

			threads.push_back( new thread( functor( first, last, func ) ) );

			first = last;

			before_idx = cur_idx;
		}

		for(size_t i=0;i<thread_num;++i)
		{
			delete threads[i];
		}
	}
*/
	//////////////////////////////////////////////////////////////////////////////
	//

	template< typename Func >
	class p_for_functor
	{
		bool & breakFlag_;
		int startIdx_;
		int endIdx_;
		Func func_;
	public:
		p_for_functor( bool & breakFlag, int startIdx, int endIdx, Func func ) : breakFlag_(breakFlag), startIdx_(startIdx), endIdx_(endIdx), func_(func) {};
		p_for_functor( p_for_functor const & other ) : breakFlag_(other.breakFlag_), startIdx_(other.startIdx_), endIdx_(other.endIdx_), func_(other.func_) {};

		void operator()() {
			for( int i=startIdx_; i<endIdx_; ++i )
			{
				if( breakFlag_ )
					break;
				func_( i, breakFlag_ );
			}
		}

	private:
		void operator=( p_for_functor const & other );
	};

	//////////////////////////////////////////////////////////////////////////////
	//

	template<class Func>
	void _for(int start, int end, Func func, int thread_num)
	{
		if( end <= start )
			return;

		if( thread_num < 1 )
			return;

		typedef p_for_functor< Func > functor;
		typedef thread<functor> thread;

		int startIdx = 0;
		int endIdx = start;
		bool breakFlag = false;

		thread ** threads = new thread*[thread_num];

		int idx_per_thread = int((end-start) / thread_num);

		for(int i=0;i<thread_num;++i)
		{
			startIdx = endIdx;

			if( i == thread_num - 1 )
				endIdx = end;
			else
				endIdx = startIdx + idx_per_thread;

			threads[i] = new thread( functor( breakFlag, startIdx, endIdx, func ) );
		}

		for(int i=0;i<thread_num;++i)
		{
			delete threads[i];
		}

		delete[] threads;
	}

	//////////////////////////////////////////////////////////////////////////////
}

//////////////////////////////////////////////////////////////////////////////
//

#define _create_functor__( return_type, name, args , code ) \
struct _##name##_functor__ { \
	typedef return_type result_type; \
	return_type operator() args { code }; \
}; \
_##name##_functor__ name

//////////////////////////////////////////////////////////////////////////////