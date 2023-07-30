#pragma once
#include "AgcAuth.h"

//-----------------------------------------------------------------------
//

class AgcHanGameAuth : public AgcAuth
{
public:
	virtual ~AgcHanGameAuth();

	virtual void error_handling();

	virtual void final_process();

private:
	virtual bool init( std::string & cmdline );
};

//-----------------------------------------------------------------------