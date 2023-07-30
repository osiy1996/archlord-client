#include <AuLog/AuLog.h>
#include "AuLogUtil.h"
#include "dbghelp.h"

//////////////////////////////////////////////////////////////////////////////
// 속성 설정

#define _AuLog_Write_		// 로그를 남김
#define _AuLog_Send_		// 로그를 FTP로 전송
#define _AuLog_MultiThread_ // 멀티 스레드에서 사용

//////////////////////////////////////////////////////////////////////////////
//

AuLog::AuLog()
	: filename_( "alef.log" )
{
	DeleteFile( filename_.c_str() );

	file_.open( filename_.c_str() );
}

//////////////////////////////////////////////////////////////////////////////
//

AuLog::~AuLog()
{
	if( file_.is_open() )
		file_.close();
}

//////////////////////////////////////////////////////////////////////////////
//

void AuLog::write( char const * text )
{
#ifndef _AuLog_Write_
	return;
#endif

	if( !file_.is_open() )
		return;

	if( !text )
		return;

#ifdef _AuLog_MultiThread_
	lock_section( critical_section_ );
#endif

	file_ << text << std::endl;
}

//////////////////////////////////////////////////////////////////////////////
//

void AuLog::write( wchar_t const * text )
{
#ifndef _AuLog_Write_
	return;
#endif

	if( !file_.is_open() )
		return;

	if( !text )
		return;

#ifdef _AuLog_MultiThread_
	lock_section( critical_section_ );
#endif

	std::wstring org = text;
	std::string str;

	AuLogUtil::wstring2string( str, org );

	file_ << str << std::endl;
}

//////////////////////////////////////////////////////////////////////////////
//

void AuLog::sendLog()
{
#ifndef _AuLog_Send_
	return;
#endif

#ifdef _AuLog_MultiThread_
	lock_section( critical_section_ );
#endif

	AuLogUtil::FTP ftp;

	ftp.send( filename_ );
}

//-----------------------------------------------------------------------
//

void AuLog::dump( std::string filename )
{
	if( filename.empty() )
		filename = "DUMP";

	filename += ".dmp";

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwProcessID = GetCurrentProcessId();

	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	/*
	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId = GetCurrentThreadId();
	eInfo.ExceptionPointers = 0;
	eInfo.ClientPointers = FALSE;
	*/

	MiniDumpWriteDump(hProcess, dwProcessID, hFile, MiniDumpNormal, NULL, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////////////