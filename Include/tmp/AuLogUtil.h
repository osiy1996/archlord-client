#pragma once
#include <string>

namespace AuLogUtil
{
	//////////////////////////////////////////////////////////////////////////////
	//

	void string2wstring(std::wstring &dest,const std::string &src);
	void wstring2string(std::string &dest,const std::wstring &src);

	//////////////////////////////////////////////////////////////////////////////
	//

	struct FTP
	{
		FTP( std::string domain = "119.205.223.154", std::string id = "kp4alord", std::string pw = "dkRmfhem1@#" );
		~FTP();

		void send( std::string filename );
	};

	//////////////////////////////////////////////////////////////////////////////
};