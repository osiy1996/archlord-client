// XTPSoundManager.cpp : implementation of the CXTPSoundManager class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "XTPSoundManager.h"

#pragma warning (disable : 4201)
	#include <mmsystem.h>
#pragma warning (default : 4201)

#pragma message("   Automatically linking with WinMM library")
#pragma message("      (Windows Multimedia System)")
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CXTPSoundManager::CXTPSoundManager()
{
	m_hThread = 0;
	m_bSystemSounds = TRUE;
	m_bTerminate = FALSE;
	m_soundState = xtpSoundNone;
}

CXTPSoundManager* AFX_CDECL XTPSoundManager()
{
	static CXTPSoundManager s_managerInstance;
	return &s_managerInstance;
}

CXTPSoundManager::~CXTPSoundManager()
{
	StopThread();
}

DWORD WINAPI CXTPSoundManager::SoundThreadProc(LPVOID)
{
	XTPSoundManagerState& soundState = XTPSoundManager()->m_soundState;

	while (!XTPSoundManager()->m_bTerminate)
	{
		switch (soundState)
		{
		case xtpSoundMenuCommand:
			::PlaySound (_T("MenuCommand"), NULL, (SND_SYNC | SND_NODEFAULT | SND_ALIAS | SND_NOWAIT));
			soundState = xtpSoundNone;
			break;

		case xtpSoundMenuPopup:
			::PlaySound (_T("MenuPopup"), NULL, (SND_SYNC | SND_NODEFAULT | SND_ALIAS | SND_NOWAIT));
			soundState = xtpSoundNone;
			break;
		}

		::Sleep (5);
	}

	::PlaySound(NULL, NULL, SND_PURGE);

	return 0;
}

void CXTPSoundManager::StartThread()
{
	ASSERT (m_hThread == NULL);

	m_soundState = xtpSoundNone;
	m_bTerminate = FALSE;

	DWORD dwThreadID;

	m_hThread = ::CreateThread(NULL, 0, SoundThreadProc, NULL, CREATE_SUSPENDED, &dwThreadID);
	if (m_hThread != NULL)
	{
		::SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);
		::ResumeThread(m_hThread);
	}

}

void CXTPSoundManager::StopThread()
{
	if (m_hThread)
	{
		m_bTerminate = TRUE;

		DWORD dwCount = 0, dwExitCode = 0;

		while (GetExitCodeThread(m_hThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
		{
			dwCount++;

			if (dwCount > 10)
			{
				TerminateThread(m_hThread, 0);
				break;
			}

			::WaitForSingleObject(m_hThread, 100);
		}

		::CloseHandle(m_hThread);
		m_hThread = 0;
	}
}

void CXTPSoundManager::PlaySystemSound(XTPSoundManagerState state)
{
	if (m_bSystemSounds)
	{
		if (m_hThread == 0)
			StartThread();

		m_soundState = state;
	}
}
void CXTPSoundManager::EnableSystemSounds(BOOL bEnable)
{
	m_bSystemSounds = bEnable;
}
