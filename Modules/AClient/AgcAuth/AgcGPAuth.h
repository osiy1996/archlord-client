#pragma once

#include <AgcAuth/AgcAuth.h>

//-----------------------------------------------------------------------
//

class AgcGPAuth : public AgcAuth
{
public:
	virtual ~AgcGPAuth();

	virtual void error_handling();

	virtual void final_process();

	virtual bool login( AgcmLogin * cmLogin, AgcmUILogin * uiLogin );

private:
	virtual bool init( std::string & cmdline );
};

//-----------------------------------------------------------------------