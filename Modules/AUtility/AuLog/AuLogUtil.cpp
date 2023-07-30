#include "AuLogUtil.h"
#include <WinSock2.h>
#include "AuParallel/for_each.hpp"

namespace AuLogUtil
{
	//////////////////////////////////////////////////////////////////////////////
	//

	void string2wstring(std::wstring &dest,const std::string &src)
	{
		struct functor
		{
			functor(std::wstring & dest, const std::string & src ) : dest(dest), src(src) {}

			void operator()( int i, bool & breakFlag )
			{
				dest[i] = static_cast<unsigned char>(src[i]);
			}

			std::wstring & dest;
			const std::string & src;
		};

		dest.resize(src.size());

		parallel::_for( 0, (int)src.size(), functor( dest, src ), 8 );
	}

	void wstring2string(std::string &dest,const std::wstring &src)
	{
		struct functor
		{
			functor( std::string & dest, const std::wstring & src )
				: dest(dest), src(src)
			{}

			void operator()( int i, bool & breakFlag )
			{
				dest[i] = src[i] < 256 ? src[i] : ' ';
			}

			std::string & dest;
			const std::wstring & src;
		};

		dest.resize(src.size());

		parallel::_for( 0, (int)src.size(), functor(dest,src), 8 );
	}

	//////////////////////////////////////////////////////////////////////////////
	//

	FTP::FTP( std::string domain, std::string id, std::string pw )
	{

	}

	//////////////////////////////////////////////////////////////////////////////
	//

	FTP::~FTP()
	{

	}

	//////////////////////////////////////////////////////////////////////////////
	//

	void FTP::send(	std::string filename )
	{
		
	}

	//////////////////////////////////////////////////////////////////////////////
}