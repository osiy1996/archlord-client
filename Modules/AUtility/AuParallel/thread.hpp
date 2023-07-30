#pragma once
#include <Windows.h>
#include <process.h>

namespace parallel
{
	//////////////////////////////////////////////////////////////////////////////
	//

	class critical_section
	{
		CRITICAL_SECTION ct;
		int lockCount;
		critical_section& operator=(critical_section const & other); // 복사 금지
		critical_section(critical_section const & other);

		inline void enter()				{ EnterCriticalSection(&ct); ++lockCount; }
		inline void leave()				{ if(lockCount) { --lockCount; LeaveCriticalSection(&ct); } }

	public:
		critical_section() : lockCount(0){ InitializeCriticalSection(&ct); }
		~critical_section()				{ leave(); DeleteCriticalSection(&ct); }

		struct section_locker
		{
			critical_section & ct_;

			section_locker( critical_section & ct ) : ct_(ct)			{ ct_.enter(); }
			~section_locker()											{ ct_.leave(); }

		private:
			section_locker( section_locker & other );
			section_locker & operator=(section_locker & other);
		};

		inline int count()				{ return lockCount; }
	};

	//////////////////////////////////////////////////////////////////////////////
	//

	template< typename Routine >
	class thread
	{
		HANDLE handle;
		unsigned int id;
		Routine routine;
		static unsigned WINAPI run( void * p ) { ((thread*)p)->routine(); return 0; }
	public:
		thread( Routine f ) : handle(0), id(0), routine(f) { handle = (HANDLE)_beginthreadex(0,0,run,this,0,&id);	}
		~thread() { 
			if( handle != 0 ) 
			{
				WaitForSingleObject(handle,INFINITE);
				CloseHandle( handle );
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////////
}

#define lock_section( __critical_section_ ) \
	parallel::critical_section::section_locker __critical_section_##section_locker( __critical_section_ );