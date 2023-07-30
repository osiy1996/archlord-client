#ifndef _AGCMRIDE_H_
#define _AGCMRIDE_H_

#include <ApModule/ApModule.h>


class AgcmRide : public ApModule
{
public:
	AgcmRide();
	~AgcmRide();

	virtual BOOL		OnAddModule();
	virtual BOOL		OnInit();
	virtual BOOL		OnDestroy();

	// Callback functions
	static BOOL			CBRideAck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRideDismountAck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRideTimeOut(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRideTID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	static AgcmRide*	m_pThis;

	class AgpmItem*			m_pcsAgpmItem;
	class AgcmItem*			m_pcsAgcmItem;
	class AgpmCharacter*	m_pcsAgpmCharacter;
	class AgcmCharacter*	m_pcsAgcmCharacter;
	class AgpmRide*			m_pcsAgpmRide;
};

#endif
