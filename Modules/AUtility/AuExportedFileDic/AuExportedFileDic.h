#pragma once

#include <ApBase/ApBase.h>
#include <hash_map>
#include <string>
#include <AuStaticPool/static_pool.h>

//---------------------------------------------------------
//

class FileNameDic
{
public:
	static char const * FindExportedName( std::string & name );
	static char const * FindOriginalName( std::string & name );
	static char const * FindExportedName( umtl::static_pool::string & name );
	static char const * FindOriginalName( umtl::static_pool::string & name );
	static void			Insert( std::string org, std::string expored );
	static void			save();
	static void			clear();

private:
	static FileNameDic& inst();
	void				load();

	typedef hash_map< std::string, std::string > ExportedDic;
	typedef hash_map< std::string, const char * > OriginalDic;
	ExportedDic exportedDic_;
	OriginalDic originalDic_;

	FileNameDic();
};

//---------------------------------------------------------