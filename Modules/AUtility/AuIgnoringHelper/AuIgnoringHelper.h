#pragma once

class AuIgnoringHelper
{
public:
	~AuIgnoringHelper();

	static bool IgnoreToFile( char * file );
	static bool IsAllow();
	static AuIgnoringHelper & Instance();

private:
	AuIgnoringHelper();

	bool allow_;
};