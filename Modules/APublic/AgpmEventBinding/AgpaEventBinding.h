//	AgpaEventBinding.h
//////////////////////////////////////////////////////////////////////

#ifndef	__AGPAEVENTBINDING_H__
#define	__AGPAEVENTBINDING_H__

#include <AgpmEventBinding/AgpdEventBinding.h>
#include <ApAdmin/ApAdmin.h>

class AgpaEventBinding : public ApAdmin
{
public:
	AgpaEventBinding();
	~AgpaEventBinding();

	AgpdBinding*		AddBinding(AgpdBinding *pcsBinding, INT32 lID, CHAR *szBindingName);
	AgpdBinding*		GetBinding(CHAR *szBindingName);
	AgpdBinding*		GetBinding(INT32 lID);
	BOOL				RemoveBinding(INT32 lID, CHAR *szBindingName);
};

#endif	//__AGPAEVENTBINDING_H__