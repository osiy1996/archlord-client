#include "AgpmResourceInfo.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

const int	AGPMRESOURCE_MAJOR_VERSION			= 0;
const int	AGPMRESOURCE_MINOR_VERSION			= 1;

const char* LOCAL_SERVER_STRING					= "LocalServer.txt";

AgpmResourceInfo::AgpmResourceInfo()
{
	SetModuleName("AgpmResourceInfo");

	// �ϴ� version�� �ڵ忡 �ھ� �ִ´�.
	// ���� �̰� ���Ͽ��� ������ ������Ʈ������ ������ �����ؼ� �ε��ϴ� ������� �ٲٴ��� �Ѵ�.

	m_lMajorVersion		= AGPMRESOURCE_MAJOR_VERSION;
	m_lMinorVersion		= AGPMRESOURCE_MINOR_VERSION;

	if(_access(LOCAL_SERVER_STRING, 00) == 0)
		g_lLocalServer = 1;
}

AgpmResourceInfo::~AgpmResourceInfo()
{
}

INT32 AgpmResourceInfo::GetMajorVersion()
{
	return m_lMajorVersion;
}

INT32 AgpmResourceInfo::GetMinorVersion()
{
	return m_lMinorVersion;
}

BOOL AgpmResourceInfo::SetMajorVersion(INT32 lMajorVersion)
{
	return (m_lMajorVersion = lMajorVersion);
}

BOOL AgpmResourceInfo::SetMinorVersion(INT32 lMinorVersion)
{
	return (m_lMinorVersion = lMinorVersion);
}

BOOL AgpmResourceInfo::CheckValidVersion(INT32 lMajorVersion, INT32 lMinorVersion)
{
	// 2005.04.29. steeple
	// �系�׽�Ʈ������ �� �α��� ������~
	if(g_lLocalServer)
	{
		return TRUE;
	}

	if (m_lMajorVersion != lMajorVersion ||
		m_lMinorVersion != lMinorVersion)
	{
		printf("Checked Client Version [%d.%d]\n",lMajorVersion,lMinorVersion);
		return FALSE;
	}

	return TRUE;
}