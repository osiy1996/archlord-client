#pragma once
#include <fstream>
#include <string>
#include "AuParallel/thread.hpp"

class AuLog
{
public:
	AuLog();
	~AuLog();

	void write( char const * text );
	void write( wchar_t const * text );
	void sendLog();
	
	static void dump( std::string filename );

private:

	AuLog & instance() {
		static AuLog inst;
		return inst;
	}

	std::string filename_;
	std::ofstream file_;

	parallel::critical_section critical_section_;
};

#ifndef WriteLog
#define WriteLog( text ) \
	AuLog::instance().write( text )
#endif

#ifndef SendLog
#define SendLog() \
	AuLog::instance().sendLog()
#endif