/*============================================================

	AgpaMailBox.h
	
============================================================*/

#ifndef _AGPA_MAILBOX_H_
	#define _AGPA_MAILBOX_H_

#include <ApBase/ApBase.h>
#include <ApAdmin/ApAdmin.h>
#include <ApModule/ApModule.h>
#include <AgpmMailBox/AgpdMailBox.h>


/************************************************/
/*		The Definition of AgpaMailBox class		*/
/************************************************/
//
class AgpaMailBox : public ApAdmin
	{
	public:
		AgpaMailBox();
		~AgpaMailBox();
		
		BOOL		Add(AgpdMail *pAgpdMail);
		BOOL		Remove(AgpdMail *pAgpdMail);
		BOOL		Remove(INT32 lID);
		AgpdMail*	Get(INT32 lID);
	};


#endif