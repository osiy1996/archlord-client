// AuHanIPCheckForServer.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2007.11.05.

//#define _INCLUDE_HANGAME_MODULE_

#include "AuHanIPCheckForServer.h"

#if defined(_WIN64) && defined(_AREA_KOREA_)
	#define _INCLUDE_HANGAME_MODULE_
#endif

#ifdef _INCLUDE_HANGAME_MODULE_
	#include "HanIPCheckForSvr.h"
#endif

#define CRM_STRING				"CRM"
#define CRM_STRING_BUFFER		32

#define ARCHLORD_GAME_ID		"K_ARCHLORD"
#define ARHCLORD_TEST_GAME_ID	"K_ARCHTEST"

AuHanIPCheckForServer::AuHanIPCheckForServer()
{
	m_bInit = FALSE;
}

AuHanIPCheckForServer::~AuHanIPCheckForServer()
{
#ifdef _INCLUDE_HANGAME_MODULE_
	if(m_bInit)
		HanIPCheckTerm();
#endif
}

int AuHanIPCheckForServer::AuHanIPCheckInit(BOOL bTestServer)
{
#ifdef _INCLUDE_HANGAME_MODULE_
	int iResult = -1;
	if(!bTestServer)
		iResult = HanIPCheckInit(ARCHLORD_GAME_ID);
	else
		iResult = HanIPCheckInit(ARHCLORD_TEST_GAME_ID);

	// iResult ���� �ѹ� �м��غ���
	if(iResult == HAN_IPCHECK_OK)
	{
		m_bInit = TRUE;
	}

	return iResult;
#else
	return 0;
#endif
}

int AuHanIPCheckForServer::AuHanIPCheckRequest(const char *szIP, char* szCRMCode, char* szGrade)
{
#ifdef _INCLUDE_HANGAME_MODULE_
	if(!szIP)
		return -1;

	CHAR szResult[256];
	memset(szResult, 0, sizeof(szResult));

	int iResult = HanIPCheckRequest(szResult, (int)sizeof(szResult), szIP);

	// iResult ���� �ѹ� �м��غ���
	if(iResult == HAN_IPCHECK_OK)
	{
		if(szGrade)
		{
			szGrade[0] = szResult[2];
			szGrade[1] = '\0';
		}

		// szResult �� Y|T|CRM0000000000... �̷��� �Ǿ�߸� �Ѱ��� S ��� �Ѵ�.
		// szResult ���� string ���� �м��ؼ� S PC ������ ���� ������ �� ����.
		if(szResult[0] == 'Y' && szResult[2] == 'T' && szCRMCode)
		{
			CHAR* pDest = strstr(szResult, CRM_STRING);
			if(pDest)
			{
				int iLength = (int)strlen(szResult);
				int iCount = (int)iLength - (int)(pDest - szResult);
				if(iCount > 0)
					strncpy_s(szCRMCode, CRM_STRING_BUFFER, pDest, iCount);
			}

			return 0;
		}
	}

	// ���ϰ��� 0 �̾�� �Ѱ��� S �ΰ��̹Ƿ�, �ٸ� Gold, Plusnum PC ���� 1�� ����.
	// �ٸ� ������ �׳� lResult ������ ����.
	return iResult == 0 ? 1 : iResult;
#else
	return 0;
#endif
}

int AuHanIPCheckForServer::AuHanIPCheckGetLastError()
{
#ifdef _INCLUDE_HANGAME_MODULE_
	return HanIPCheckGetLastError();
#else
	return 0;
#endif
}

std::string AuHanIPCheckForServer::GetGameString(BOOL bTestServer)
{
#ifdef _INCLUDE_HANGAME_MODULE_
	std::string szGameString;
	if(!bTestServer)
		szGameString = ARCHLORD_GAME_ID;
	else
		szGameString = ARHCLORD_TEST_GAME_ID;

	return szGameString;
#else
	return std::string("");
#endif
}