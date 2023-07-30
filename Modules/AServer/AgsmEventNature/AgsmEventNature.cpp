// AgsmEventNature.cpp: implementation of the AgsmEventNature class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmEventNature.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmEventNature::AgsmEventNature()
{
	SetModuleName("AgsmEventNature");

	EnableIdle2(TRUE);

	SetModuleType(APMODULE_TYPE_SERVER);

	m_ulNextWeatherClock = 0;
	m_eOriginalWeatherType = AGPDNATURE_WEATHER_TYPE_NONE;
	m_eCurrentWeatherType = AGPDNATURE_WEATHER_TYPE_NONE;
}

AgsmEventNature::~AgsmEventNature()
{

}

BOOL	AgsmEventNature::OnAddModule()
{
	// ���� Module�� �����´�.
	m_pcsAgpmEventNature	= (AgpmEventNature	*)	GetModule("AgpmEventNature");
	m_pcsAgpmTimer			= (AgpmTimer		*)	GetModule("AgpmTimer");

	if ((!m_pcsAgpmEventNature) || (!m_pcsAgpmTimer))
		return FALSE;

	// Callback�� Set�Ѵ�.
	if (!m_pcsAgpmEventNature->SetCallbackNature(CBNature, this))
		return FALSE;

	return TRUE;
}

// 2006.11.08. steeple
// �� �� �ٲ���.
BOOL	AgsmEventNature::OnIdle2(UINT32 ulClockCount)
{
	if(m_ulNextWeatherClock == 0)
		return TRUE;

	if(ulClockCount > m_ulNextWeatherClock)
	{
		// ���� ������ �����ش�.
		SetWeatherWithDuration(m_eOriginalWeatherType, 0);
	}

	return TRUE;


//	UINT64	ullThisTime = m_pcsAgpmTimer->GetGameTime();
//
//	// ���� �ð��� ���� ���� �ð��� �Ǹ�.
////	if (m_ullNextWeatherTime && m_pcsAgpmEventNature->m_csNature.m_ullTime >= m_ullNextWeatherTime)
//	if (m_ullNextWeatherTime && ullThisTime >= m_ullNextWeatherTime)
//	{
//		// ���ο� ������ random�ϰ� ����ؼ� �����ϰ�, ���� ���� ���� �ð��� ����Ѵ�.
//		AgpdNatureWeatherType eWeather = (AgpdNatureWeatherType) (rand() % (AGPDNATURE_WEATHER_TYPE_MAX - 1) + 1);
//
//		m_pcsAgpmEventNature->SetWeather(eWeather);
//
//		//m_ullNextWeatherTime = m_pcsAgpmTimer->GetGameTime() + m_pcsAgpmEventNature->m_csNature.m_ullWeatherMinDelay +
//		m_ullNextWeatherTime = ullThisTime + m_pcsAgpmEventNature->m_csNature.m_ullWeatherMinDelay +
//							   (rand() % (m_pcsAgpmEventNature->m_csNature.m_ullWeatherMaxDelay - m_pcsAgpmEventNature->m_csNature.m_ullWeatherMinDelay));
//	}
//
//	return TRUE;
}

// 2006.11.08. steeple
// �����ð����� ������ �ٲ��ִ� �Լ�
BOOL	AgsmEventNature::SetWeatherWithDuration(AgpdNatureWeatherType eWeatherType, UINT32 ulDuration)
{
	// �� ������ �ٲ۴�.
	m_eCurrentWeatherType = eWeatherType;
	
	if(ulDuration == 0)
		m_ulNextWeatherClock = 0;	// �̷��� �Ǹ� ������ �ٲ�� ����.
	else
	{
		UINT32 ulClock = GetClockCount();
		m_ulNextWeatherClock = ulClock + ulDuration;
	}

	SendNature(m_eCurrentWeatherType);

	return TRUE;
}

// 2006.11.08. steeple
// Nature ��Ŷ ��Ȱ.
// �ð��� Timer ���� �����ϱ�, ���⼭�� Weather �� ������.
BOOL	AgsmEventNature::CBNature(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmEventNature *	pThis = (AgsmEventNature *) pClass;
	AgpdNature *		pcsNature = (AgpdNature *) pData;

	return pThis->SendNature(pcsNature->m_eWeather);
}

// 2006.11.08. steeple
BOOL	AgsmEventNature::SendNature(AgpdNatureWeatherType eWeatherType, UINT32 ulNID)
{
	PVOID pvPacket= NULL;
	INT16 nSize = 0;

	pvPacket = m_pcsAgpmEventNature->m_csPacket.MakePacket(TRUE, &nSize, AGPMEVENT_NATURE_PACKET_TYPE, 
					&eWeatherType,
					//NULL,		//&pcsNature->m_ucTimeRatio,
					//NULL,		//&pcsNature->m_ullTime,
					NULL,
					NULL);
	if(!pvPacket || nSize < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if(!ulNID)
		bResult = SendPacketAllUser(pvPacket, nSize);
	else
		bResult = SendPacket(pvPacket, nSize, ulNID);

	m_pcsAgpmEventNature->m_csPacket.FreePacket(pvPacket);

	return bResult;
}