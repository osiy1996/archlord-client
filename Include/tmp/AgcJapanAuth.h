#pragma once
#include "AgcAuth.h"

//-----------------------------------------------------------------------
//

class AgcJapanAuth : public AgcAuth
{
public:
	virtual ~AgcJapanAuth();

	virtual void error_handling();

	virtual void final_process();

private:
	virtual bool init( std::string & cmdline );
};

//-----------------------------------------------------------------------