#ifdef _AREA_KOREA_
#ifndef _AUHANGAMETPACK_
#define _AUHANGAMETPACK_

#include "ApDefine.h"
#include "../Server/HanGameForServer/HangameTPack/DORIAN_Connector/Include/DORIAN_3.0.h"
#include "ApMutualEx.h"

class AuHangameTPack
{
private:
	ApMutualEx				m_csMutex;
			
public:
	AuHangameTPack();
	~AuHangameTPack();

	BOOL Create(INT32 lServerID);
	void Destroy();

	BOOL CheckIn(CHAR* szAccount, CHAR* szIP);
	BOOL CheckOut(CHAR* szAccount);
};

#endif // _AUHANGAMETPACK_
#endif // _AREA_KOREA_