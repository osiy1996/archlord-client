#pragma once

#if defined( _WIN32 ) || defined( _WIN64 )

#include <windows.h>
#include <tchar.h>
#include <string>

namespace umtl
{
	//////////////////////////////////////////////////////////////////////////
	//

	class mapped_memory
	{
	public:
		typedef unsigned int size_type;

		size_type		size	()	{ return size_; }
		char *			get		()	{ return data_; }
		TCHAR const *	name	()	{ return filename_.empty() ? 0 : filename_.c_str(); }
		bool			isReadOnly() { return isReadOnly_; }

		// filename 이 null이면 임시 파일을 생성하여 가상메모리풀 처럼 사용됨
		mapped_memory( TCHAR * filename=0, bool readOnly = false );
		~mapped_memory();


	private:
		HANDLE	file_;
		HANDLE	map_;
		size_type size_;
		char *	data_;
		bool isReadOnly_;

		typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > string;
		string filename_;

		// 복사는 금지
		mapped_memory( mapped_memory const & other );
		void operator=( mapped_memory const & other );
	};

	//////////////////////////////////////////////////////////////////////////
}

#endif