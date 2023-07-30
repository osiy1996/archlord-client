#ifdef _AREA_GLOBAL_
#pragma once
// Cogpas�� codemasters(�Ϲ�/���� ���� ��ü)�� �����ý����Դϴ�.

#include "AgsmLoginDB.h"

class CCogpas
{
public:
	typedef int ( *LPFNAUTHENTICATE ) ( const wchar_t* const, const wchar_t* const, bool );
	typedef int ( *LPFNAUTHENTICATEUSER ) ( const wchar_t* const, bool );

	CCogpas();
	~CCogpas();

	bool LoadDLL();
	int  AuthenticateUser(char* userName, bool verbose);
	int  Authenticate(char* account, char* password);
	
private:
	HMODULE	m_dll;
	LPFNAUTHENTICATE	 m_lpfnAuthenticate;
	LPFNAUTHENTICATEUSER m_lpfnAuthUser;

	CRITICAL_SECTION	m_cs;
};

extern CCogpas g_cogpas;
#endif