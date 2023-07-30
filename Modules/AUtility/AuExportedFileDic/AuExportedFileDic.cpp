#include <AuExportedFileDic/AuExportedFileDic.h>
#include <AuZpackLoader/AuZpackLoader.h>
#include <sstream>

//---------------------------------------------------------
//

FileNameDic & FileNameDic::inst()
{
	static FileNameDic dic;
	dic.load();
	return dic;
}

//---------------------------------------------------------
//

FileNameDic::FileNameDic()
{}

//---------------------------------------------------------
//

void FileNameDic::clear()
{
	FileNameDic & dic = inst();

	dic.exportedDic_.clear();
	dic.originalDic_.clear();
}

//---------------------------------------------------------
//

void FileNameDic::load()
{
	if( !exportedDic_.empty() && !originalDic_.empty() )
		return;

	size_t size = 0;

	unsigned char * buf = GetZpackStream( L"ini.zp", "INI/ExportedFileNameDic.txt" , size );

	if( buf && size )
	{
		char line[2048];

		std::string lineStr;
		std::string original;
		std::string exported;
		std::stringstream stream( (char*)buf );

		while( stream.good() )
		{
			stream.getline( line, sizeof(line) );
			
			lineStr = line;

			size_t splitPos = lineStr.find( '\t' );

			original = lineStr.substr( 0, splitPos );
			exported = lineStr.substr( splitPos + 1, std::string::npos );

			Insert( original, exported );
		}

		ReleaseZpackStream( L"ini.zp", buf );
	}
}

//---------------------------------------------------------
//

char const * FileNameDic::FindExportedName( std::string & name )
{
	FileNameDic & dic = inst();

	OriginalDic::iterator iter = dic.originalDic_.find( name );

	return iter != dic.originalDic_.end() ? iter->second : 0 ;
}

//---------------------------------------------------------
//

char const * FileNameDic::FindOriginalName( std::string & name )
{
	FileNameDic & dic = inst();

	ExportedDic::iterator iter = dic.exportedDic_.find( name );

	return iter != dic.exportedDic_.end() ? iter->second.c_str() : 0 ;
}

char const * FileNameDic::FindExportedName( umtl::static_pool::string & name )
{
	FileNameDic & dic = inst();

	OriginalDic::iterator iter = dic.originalDic_.find( name.c_str() );

	return iter != dic.originalDic_.end() ? iter->second : 0 ;
}

//---------------------------------------------------------
//

char const * FileNameDic::FindOriginalName( umtl::static_pool::string & name )
{
	FileNameDic & dic = inst();

	ExportedDic::iterator iter = dic.exportedDic_.find( name.c_str() );

	return iter != dic.exportedDic_.end() ? iter->second.c_str() : 0 ;
}

//-----------------------------------------------------------------------
//

void FileNameDic::Insert( std::string org, std::string expored )
{
	FileNameDic & dic = inst();

	dic.exportedDic_[expored] = org;
	dic.originalDic_[org] = expored.c_str();
}

void FileNameDic::save()
{

}

//---------------------------------------------------------