#pragma once
#include <AgcAuth/AgcAuth.h>

//-----------------------------------------------------------------------
//

class AgcTaiwanAuth : public AgcAuth
{
public:
	virtual ~AgcTaiwanAuth();

	virtual void error_handling();
	virtual void final_process();
	virtual bool encrypt( char * code, char * account, size_t account_size, char * password, size_t password_size );

private:
	virtual bool init( std::string & cmdline );
};

//-----------------------------------------------------------------------