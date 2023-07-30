// AuTickCounter.cpp: implementation of the AuTickCounter class.
//
//////////////////////////////////////////////////////////////////////

#include <ApBase/ApBase.h>
#include <stdio.h>
#include "AuTickCounter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuTickCounter::AuTickCounter(): m_ullTickPerMillisecond(0),m_ullLastElapsedTime(0)
{
// 	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &m_ullTickPerMillisecond))
// 		m_ullTickPerMillisecond	= 0;
// 
// 	m_hThread				= NULL	;
}

AuTickCounter::~AuTickCounter()
{
// 	if( m_hThread )
// 	{
// 		WaitForSingleObject( m_hThread , INFINITE );
// 
// 		// TerminateThread( m_hThread , 0 );
// //		CloseHandle( m_hThread );
// 	}
}

//Rdtsc�� �����°��� ����CPU�� �ƴ� Ÿ �÷����ϰ�쿣 ������ ���� ������ �־� �ٲ�.
//���ʿ��� ����������� ����
void AuTickCounter::Initialize()
{
	LARGE_INTEGER qwTickPerSec, qwTime;

	if( 0 != QueryPerformanceFrequency( &qwTickPerSec ) )
	{
		m_ullTickPerMillisecond	= qwTickPerSec.QuadPart;
		m_ullLastElapsedTime = QueryPerformanceCounter( &qwTime );
	}


// 	UINT dwThreadID;
// 
// 	if (!m_ullTickPerMillisecond)
// 	{
// 		m_hThread = (HANDLE)_beginthreadex(NULL, 0, AuTickCounter::CalcTPM, &m_ullTickPerMillisecond, 0, &dwThreadID);
// 		SetThreadName(dwThreadID, "AuTickCounter");
// 	}
}
UINT32 AuTickCounter::GetTickCount()
{
	UINT64 ullTick;
	QueryPerformanceCounter( (LARGE_INTEGER*)&ullTick );

	//�̹� �ڵ忡�� 32��Ʈ�� ó�����̶� ��¿�� ���� (UINT32)�� ����ȯ�� ...©�����ѵ� ...�� �Ѥ�;;; supertj@20100208
	return (UINT32)( ullTick * 1000  / m_ullTickPerMillisecond ); 
}

float AuTickCounter::GetElapsedTime()
{
	LARGE_INTEGER ullTick;
	QueryPerformanceCounter( &ullTick );

	float fElapsedTime = (float) ( (double)(ullTick.QuadPart - m_ullLastElapsedTime ) / (double)m_ullTickPerMillisecond );

	m_ullLastElapsedTime = ullTick.QuadPart;

	return fElapsedTime; 
}


// _inline UINT64 AuTickCounter::Rdtsc()
// {
// #ifdef _M_X64
// 	// ....................
// 	// ....................
// 	// How can I solve this problem in AMD x64 architecture?
// 	//
// 	//
// 	return 0L;
// #else
// //     _asm    _emit 0x0F
// //     _asm    _emit 0x31
// 	_asm	rdtsc
// #endif
// }
// 
// UINT WINAPI AuTickCounter::CalcTPM(LPVOID lpParameter)
// {
// 	UINT32 *	pulTPM = (UINT32 *) lpParameter;
// 	UINT64		ulTPS;
// 	UINT32		ulTPM;
// 	INT32		lIndex;
// 
// 	*pulTPM = -1;
// 
// 	for (lIndex = 0; lIndex < 5; ++lIndex)
// 	{
// 		ulTPS = AuTickCounter::Rdtsc();
// 		Sleep(1000);
// 		ulTPS = (AuTickCounter::Rdtsc() - ulTPS);
// 
// 		ulTPM = (UINT32) ( ulTPS / 1000000 ) * 1000;
// 
// 		if (*pulTPM > ulTPM)
// 			*pulTPM = ulTPM;
// 	}
// 
// 	ExitThread( 0 );
// 	return 0;
// }
