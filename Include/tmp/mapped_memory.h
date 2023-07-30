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

		// filename �� null�̸� �ӽ� ������ �����Ͽ� ����޸�Ǯ ó�� ����
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

		// ����� ����
		mapped_memory( mapped_memory const & other );
		void operator=( mapped_memory const & other );
	};

	//////////////////////////////////////////////////////////////////////////
}

#endif