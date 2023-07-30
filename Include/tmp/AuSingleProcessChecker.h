#pragma once

#include "ApBase.h"

class AuSingleProcessChecker
{
public:
	AuSingleProcessChecker(const char *i_szProcessName, const char *i_szAppWindowTile = NULL);
	~AuSingleProcessChecker(void);

	BOOL InitChecker();
	void CleanChecker();
	BOOL IsAlreadyRunning();
	void ActivateFirstApplication();

public:
	HANDLE		m_hMutex;
	const char	*m_szProcessName;
	const char	*m_szAppWindowTile;
};
