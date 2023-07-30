#include "mapped_memory.h"

#if defined( _WIN32 ) || defined( _WIN64 )

namespace umtl
{
	//////////////////////////////////////////////////////////////////////////
	//

	mapped_memory::mapped_memory( TCHAR * filename, bool readOnly )
		: file_	(0)
		, map_	(0)
		, size_	(0)
		, data_	(0)
		, filename_( filename ? filename : _T("") )
		, isReadOnly_( readOnly )
	{
		if( !filename_.empty() )
		{
			file_ = CreateFile( filename, readOnly ? (GENERIC_READ) : (GENERIC_WRITE | GENERIC_READ)
				, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		}
		else
		{
			file_ = INVALID_HANDLE_VALUE;
		}

		if( filename_.empty() || file_ != INVALID_HANDLE_VALUE )
		{
			if( file_ != INVALID_HANDLE_VALUE )
				size_ = (size_type)GetFileSize( file_, 0 );
			else
				size_ = 512 * 1024 * 1024;

			map_ = CreateFileMapping( file_, 0, (isReadOnly_ ? PAGE_READONLY : PAGE_READWRITE), 0, size_, (filename_.empty() ? 0 : filename_.c_str()) );

			if( map_ == 0 )
			{
				CloseHandle( file_ );
				file_ = 0;
				size_ = 0;
			}
			else
			{
				data_ = (char*)MapViewOfFile( map_
					, isReadOnly_ ? (FILE_MAP_READ) : (FILE_MAP_ALL_ACCESS)
					, 0, 0, 0 );

				if( data_ == 0 )
				{
					CloseHandle( map_ );
					CloseHandle( file_ );
					map_ = 0;
					file_ = 0;
					size_ = 0;
				}
			}			
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	//

	mapped_memory::~mapped_memory() 
	{
		if( data_ )
			UnmapViewOfFile( data_ );

		if( map_ )
			CloseHandle(map_);

		if( file_ && file_ != INVALID_HANDLE_VALUE )
			CloseHandle(file_);
	}

	//////////////////////////////////////////////////////////////////////////
}

#endif