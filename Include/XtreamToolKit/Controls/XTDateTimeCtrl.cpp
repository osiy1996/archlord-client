// XTDateTimeCtrl.cpp : implementation of the CXTDateTimeCtrl class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#include "Common/XTPVC80Helpers.h"  // Visual Studio 2005 helper functions
#include "Common/XTPVC50Helpers.h"  // Visual Studio 2005 helper functions

#include "XTDefines.h"
#include "XTGlobal.h"
#include "XTDateTimeCtrl.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static BOOL GetAsSystemTime(const CTime& tm, SYSTEMTIME& timeDest)
{
	struct tm ttm;
	struct tm *ptm = tm.GetLocalTm(&ttm);

	if (!ptm)
		return FALSE;

	timeDest.wYear = (WORD) (1900 + ptm->tm_year);
	timeDest.wMonth = (WORD) (1 + ptm->tm_mon);
	timeDest.wDayOfWeek = (WORD) ptm->tm_wday;
	timeDest.wDay = (WORD) ptm->tm_mday;
	timeDest.wHour = (WORD) ptm->tm_hour;
	timeDest.wMinute = (WORD) ptm->tm_min;
	timeDest.wSecond = (WORD) ptm->tm_sec;
	timeDest.wMilliseconds = 0;

	return TRUE;
}

static BOOL GetAsSystemTime(const CTime* ptm, SYSTEMTIME& timeDest)
{
	return GetAsSystemTime(*ptm, timeDest);
}

static BOOL GetAsSystemTime(const COleDateTime& dtm, SYSTEMTIME& sysTime)
{
#if _MSC_VER < 1200
	return CXTPDateTimeHelper::GetAsSystemTime(dtm, sysTime);
#else
	return dtm.GetAsSystemTime(sysTime);
#endif
}

static BOOL GetAsSystemTime(const COleDateTime* pdtm, SYSTEMTIME& sysTime)
{
	return GetAsSystemTime(*pdtm, sysTime);
}

/////////////////////////////////////////////////////////////////////////////
// CXTDateTimeCtrl class
/////////////////////////////////////////////////////////////////////////////
BOOL CXTDateTimeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0

	// initialize common controls
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_DATE_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));

	#else

	// initialize common controls
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_DATE_REG));

	#endif//#if (_MSC_VER <= 1100)

	CWnd* pWnd = this;
	return pWnd->Create(DATETIMEPICK_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
}

DWORD CXTDateTimeCtrl::GetRange(CTime* pMinTime, CTime* pMaxTime) const
{
	ASSERT(::IsWindow(m_hWnd));
	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	//IA64: Assume retval of DTM_GETRANGE is still 32-bit
	DWORD dwResult = (DWORD)::SendMessage(m_hWnd, DTM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (pMinTime != NULL)
	{
		if (dwResult & GDTR_MIN)
			*pMinTime = CTime(sysTimes[0]);
	}

	if (pMaxTime != NULL)
	{
		if (dwResult & GDTR_MAX)
			*pMaxTime = CTime(sysTimes[1]);
	}

	return dwResult;

}

DWORD CXTDateTimeCtrl::GetRange(COleDateTime* pMinTime, COleDateTime* pMaxTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwResult = (DWORD)::SendMessage(m_hWnd, DTM_GETRANGE, 0, (LPARAM) &sysTimes);
	if (pMinTime != NULL)
	{
		if (dwResult & GDTR_MIN)
			*pMinTime = COleDateTime(sysTimes[0]);
		else
			pMinTime->SetStatus(COleDateTime::null);
	}

	if (pMaxTime != NULL)
	{
		if (dwResult & GDTR_MAX)
			*pMaxTime = COleDateTime(sysTimes[1]);
		else
			pMaxTime->SetStatus(COleDateTime::null);
	}

	return dwResult;
}

BOOL CXTDateTimeCtrl::SetRange(const CTime* pMinTime, const CTime* pMaxTime)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];

	WPARAM wFlags = 0;
	if (pMinTime != NULL && GetAsSystemTime(pMinTime, sysTimes[0]))
		wFlags |= GDTR_MIN;

	if (pMaxTime != NULL && GetAsSystemTime(pMaxTime, sysTimes[1]))
		wFlags |= GDTR_MAX;

	return (BOOL) ::SendMessage(m_hWnd, DTM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

BOOL CXTDateTimeCtrl::SetRange(const COleDateTime* pMinTime, const COleDateTime* pMaxTime)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pMinTime == NULL || pMinTime->GetStatus() != COleDateTime::invalid);
	ASSERT(pMaxTime == NULL || pMaxTime->GetStatus() != COleDateTime::invalid);

	SYSTEMTIME sysTime[2];

	WPARAM wFlags = 0;
	if (pMinTime != NULL && pMinTime->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMinTime, sysTime[0]))
			wFlags |= GDTR_MIN;
	}

	if (pMaxTime != NULL && pMaxTime->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMaxTime, sysTime[1]))
			wFlags |= GDTR_MAX;
	}

	return (BOOL) ::SendMessage(m_hWnd, DTM_SETRANGE, wFlags, (LPARAM) &sysTime);
}

BOOL CXTDateTimeCtrl::SetTime(LPSYSTEMTIME pTimeNew /* = NULL */)
{
	ASSERT(::IsWindow(m_hWnd));
	WPARAM wParam = (pTimeNew == NULL) ? GDT_NONE : GDT_VALID;
	return (BOOL) ::SendMessage(m_hWnd, DTM_SETSYSTEMTIME,
		wParam, (LPARAM) pTimeNew);
}

BOOL CXTDateTimeCtrl::SetTime(const COleDateTime& timeNew)
{
	BOOL bRetVal = FALSE;

	// make sure the time isn't invalid
	ASSERT(timeNew.GetStatus() != COleDateTime::invalid);
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	WPARAM wParam = GDT_NONE;
	if (timeNew.GetStatus() == COleDateTime::valid &&
		GetAsSystemTime(timeNew, sysTime))
	{
		wParam = GDT_VALID;
	}

	bRetVal = (BOOL) ::SendMessage(m_hWnd,
			DTM_SETSYSTEMTIME, wParam, (LPARAM) &sysTime);

	return bRetVal;
}

BOOL CXTDateTimeCtrl::SetTime(const CTime* pTimeNew)
{
	BOOL bRetVal = FALSE;

	// make sure the time isn't invalid
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	WPARAM wParam = GDT_NONE;
	if (pTimeNew != NULL && GetAsSystemTime(pTimeNew, sysTime))
	{
		wParam = GDT_VALID;
	}

	bRetVal = (BOOL) ::SendMessage(m_hWnd,
			DTM_SETSYSTEMTIME, wParam, (LPARAM) &sysTime);

	return bRetVal;
}

BOOL CXTDateTimeCtrl::GetTime(COleDateTime& timeDest) const
{
	SYSTEMTIME sysTime;
	BOOL bRetVal = TRUE;

	LRESULT result = ::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM) &sysTime);
	if (result == GDT_VALID)
	{
		timeDest = COleDateTime(sysTime);
		bRetVal = TRUE;
		ASSERT(timeDest.GetStatus() == COleDateTime::valid);
	}
	else if (result == GDT_NONE)
	{
		timeDest.SetStatus(COleDateTime::null);
		bRetVal = TRUE;
	}
	else
		timeDest.SetStatus(COleDateTime::invalid);
	return bRetVal;
}

DWORD CXTDateTimeCtrl::GetTime(CTime& timeDest) const
{
	SYSTEMTIME sysTime;
	DWORD dwResult = (DWORD)
		::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM) &sysTime);

	if (dwResult == GDT_VALID)
		timeDest = CTime(sysTime);

	return dwResult;
}

CXTDateTimeCtrl::~CXTDateTimeCtrl()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CXTMonthCalCtrl class
/////////////////////////////////////////////////////////////////////////////
BOOL CXTMonthCalCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	#if (_MSC_VER <= 1100) // Using Visual C++ 5.0

	// initialize common controls
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_DATE_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));

	#else

	// initialize common controls
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_DATE_REG));

	#endif//#if (_MSC_VER <= 1100)

	CWnd* pWnd = this;
	return pWnd->Create(MONTHCAL_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
}

BOOL CXTMonthCalCtrl::Create(DWORD dwStyle, const POINT& pt, CWnd* pParentWnd, UINT nID)
{
	BOOL bWasVisible = (dwStyle & WS_VISIBLE);
	dwStyle &= ~WS_VISIBLE;

	CRect rect(pt.x, pt.y, 0, 0);

	BOOL bRetVal = FALSE;
	if (Create(dwStyle, rect, pParentWnd, nID))
	{
		if (SizeMinReq())
		{
			if (bWasVisible)
				ShowWindow(SW_SHOWNA);
			bRetVal = TRUE;
		}
		else
			DestroyWindow();
	}

	return bRetVal;
}

BOOL CXTMonthCalCtrl::SizeMinReq(BOOL bRepaint /* = TRUE */)
{
	CRect rect;
	BOOL bRetVal = FALSE;
	if (GetMinReqRect(rect))
	{
		DWORD dwFlags = SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOMOVE | SWP_NOACTIVATE;
		if (!bRepaint)
			dwFlags |= SWP_NOREDRAW;
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), dwFlags);
		bRetVal = TRUE;
	}

	return bRetVal;
}

void CXTMonthCalCtrl::SetToday(const COleDateTime& refTime)
{
	ASSERT_VALID(this);

	// make sure the time isn't invalid
	ASSERT(refTime.GetStatus() != COleDateTime::invalid);
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	LPSYSTEMTIME pSysTime = NULL;
	WPARAM wParam = GDT_NONE;

	// if the passed time is null or out of range,
	// we'll set the control to NULL

	if (GetAsSystemTime(refTime, sysTime))
	{
		pSysTime = &sysTime;
		wParam = GDT_VALID;
	}

	if (::IsWindow(m_hWnd))
		::SendMessage(m_hWnd, MCM_SETTODAY, wParam, (LPARAM) pSysTime);
}

void CXTMonthCalCtrl::SetToday(const CTime* pDateTime)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT_VALID(this);

	// if the passed time is NULL, we'll set the
	// control to NULL

	WPARAM wParam = GDT_NONE;
	LPSYSTEMTIME pSysTime = NULL;
	SYSTEMTIME sysTime;

	if (pDateTime != NULL && GetAsSystemTime(pDateTime, sysTime))
	{
		wParam = GDT_VALID;
		pSysTime = &sysTime;
	}

	if (::IsWindow(m_hWnd))
		::SendMessage(m_hWnd, MCM_SETTODAY, wParam, (LPARAM) pSysTime);
}

BOOL CXTMonthCalCtrl::SetCurSel(const COleDateTime& refTime)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	BOOL bRetVal = FALSE;

	// if the passed time is null or out of range,
	// we'll set the control to NULL

	if (GetAsSystemTime(refTime, sysTime) &&
		refTime.GetStatus() == COleDateTime::valid)
	{
		bRetVal = (BOOL)
			::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM) &sysTime);
	}

	return bRetVal;
}

BOOL CXTMonthCalCtrl::SetCurSel(const CTime& refTime)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTime;
	BOOL bRetVal = FALSE;

	if (GetAsSystemTime(refTime, sysTime))
	{
		bRetVal = (BOOL)
			::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM) &sysTime);
	}

	return bRetVal;
}

BOOL CXTMonthCalCtrl::GetCurSel(COleDateTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = GetCurSel(&sysTime);

	if (bResult)
		refTime = COleDateTime(sysTime);

	return bResult;
}

BOOL CXTMonthCalCtrl::GetToday(COleDateTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM) &sysTime);

	if (bResult)
		refTime = COleDateTime(sysTime);

	return bResult;
}

BOOL CXTMonthCalCtrl::GetCurSel(CTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = GetCurSel(&sysTime);

	if (bResult)
		refTime = CTime(sysTime);

	return bResult;
}

BOOL CXTMonthCalCtrl::GetToday(CTime& refTime) const
{
	ASSERT(::IsWindow(m_hWnd));

	// can't use this method on multiple selection controls
	ASSERT(!(GetStyle() & MCS_MULTISELECT));

	SYSTEMTIME sysTime;
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM) &sysTime);

	if (bResult)
		refTime = CTime(sysTime);

	return bResult;
}

CXTMonthCalCtrl::~CXTMonthCalCtrl()
{
	DestroyWindow();
}

int CXTMonthCalCtrl::GetFirstDayOfWeek(BOOL* pbLocal /* = NULL */) const
{
	ASSERT(::IsWindow(m_hWnd));
	DWORD dwResult;
	dwResult = (DWORD) ::SendMessage(m_hWnd, MCM_GETFIRSTDAYOFWEEK, 0, 0);

	// set *pbLocal to reflect if the first day of week
	// matches current locale setting

	if (pbLocal)
		*pbLocal = HIWORD(dwResult);
	return LOWORD(dwResult);
}

BOOL CXTMonthCalCtrl::SetFirstDayOfWeek(int iDay, int* lpnOld /* = NULL */)
{
	ASSERT(::IsWindow(m_hWnd));
	DWORD dwResult;
	dwResult = (DWORD) ::SendMessage(m_hWnd, MCM_SETFIRSTDAYOFWEEK, 0, (WPARAM) iDay);

	if (lpnOld != NULL)
		*lpnOld = LOWORD(dwResult);

	return (BOOL) HIWORD(dwResult);
}

BOOL CXTMonthCalCtrl::SetDayState(int nMonths, LPMONTHDAYSTATE pStates)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(GetStyle()&MCS_DAYSTATE);
	ASSERT(AfxIsValidAddress(pStates, nMonths * sizeof(MONTHDAYSTATE), FALSE));
	return (BOOL) ::SendMessage(m_hWnd, MCM_SETDAYSTATE, (WPARAM) nMonths, (LPARAM) pStates);
}

BOOL CXTMonthCalCtrl::SetRange(const COleDateTime* pMinRange, const COleDateTime* pMaxRange)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pMinRange == NULL || pMinRange->GetStatus() != COleDateTime::invalid);
	ASSERT(pMaxRange == NULL || pMaxRange->GetStatus() != COleDateTime::invalid);

	SYSTEMTIME sysTimes[2];
	WPARAM wFlags = 0;

	if (pMinRange != NULL && pMinRange->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMinRange, sysTimes[0]))
			wFlags |= GDTR_MIN;
	}

	if (pMaxRange != NULL && pMaxRange->GetStatus() != COleDateTime::null)
	{
		if (GetAsSystemTime(pMaxRange, sysTimes[1]))
			wFlags |= GDTR_MAX;
	}

	return (BOOL)
		::SendMessage(m_hWnd, MCM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

BOOL CXTMonthCalCtrl::SetRange(const LPSYSTEMTIME pMinRange, const LPSYSTEMTIME pMaxRange)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	WPARAM wFlags = 0;

	if (pMinRange != NULL)
	{
		MEMCPY_S(&sysTimes[0], pMinRange, sizeof(SYSTEMTIME));
		wFlags |= GDTR_MIN;
	}

	if (pMaxRange != NULL)
	{
		MEMCPY_S(&sysTimes[1], pMaxRange, sizeof(SYSTEMTIME));
		wFlags |= GDTR_MAX;
	}

	return (BOOL)
		::SendMessage(m_hWnd, MCM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

DWORD CXTMonthCalCtrl::GetRange(COleDateTime* pMinRange, COleDateTime* pMaxRange) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwRanges = (DWORD)
		::SendMessage(m_hWnd, MCM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (dwRanges & GDTR_MIN && pMinRange)
		*pMinRange = COleDateTime(sysTimes[0]);

	if (dwRanges & GDTR_MAX && pMaxRange)
		*pMaxRange = COleDateTime(sysTimes[1]);

	return dwRanges;
}

DWORD CXTMonthCalCtrl::GetRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwRanges = (DWORD)
		::SendMessage(m_hWnd, MCM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (dwRanges & GDTR_MIN && pMinRange)
	{
		MEMCPY_S(pMinRange, &sysTimes[0], sizeof(SYSTEMTIME));
	}

	if (dwRanges & GDTR_MAX && pMaxRange)
	{
		MEMCPY_S(pMaxRange, &sysTimes[1], sizeof(SYSTEMTIME));
	}

	return dwRanges;
}

BOOL CXTMonthCalCtrl::SetRange(const CTime* pMinRange, const CTime* pMaxRange)
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	WPARAM wFlags = 0;
	if (pMinRange != NULL && GetAsSystemTime(pMinRange, sysTimes[0]))
		wFlags |= GDTR_MIN;

	if (pMaxRange != NULL && GetAsSystemTime(pMaxRange, sysTimes[1]))
		wFlags |= GDTR_MAX;

	return (BOOL)
		::SendMessage(m_hWnd, MCM_SETRANGE, wFlags, (LPARAM) &sysTimes);
}

DWORD CXTMonthCalCtrl::GetRange(CTime* pMinRange, CTime* pMaxRange) const
{
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));

	DWORD dwRanges = (DWORD)
		::SendMessage(m_hWnd, MCM_GETRANGE, 0, (LPARAM) &sysTimes);

	if (dwRanges & GDTR_MIN && pMinRange)
		*pMinRange = CTime(sysTimes[0]);

	if (dwRanges & GDTR_MAX && pMaxRange)
		*pMaxRange = CTime(sysTimes[1]);

	return dwRanges;
}

int CXTMonthCalCtrl::GetMonthRange(COleDateTime& refMinRange, COleDateTime& refMaxRange, DWORD dwFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(dwFlags == GMR_DAYSTATE || dwFlags == GMR_VISIBLE);

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	int nCount = (int) ::SendMessage(m_hWnd, MCM_GETMONTHRANGE,
		(WPARAM) dwFlags, (LPARAM) &sysTimes);

	refMinRange = COleDateTime(sysTimes[0]);
	refMaxRange = COleDateTime(sysTimes[1]);

	return nCount;
}

int CXTMonthCalCtrl::GetMonthRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange, DWORD dwFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT_POINTER(pMinRange, SYSTEMTIME);
	ASSERT_POINTER(pMaxRange, SYSTEMTIME);

	SYSTEMTIME sysTimes[2];

	int nCount = (int) ::SendMessage(m_hWnd, MCM_GETMONTHRANGE,
		(WPARAM) dwFlags, (LPARAM) &sysTimes);

	MEMCPY_S(pMinRange, &sysTimes[0], sizeof(SYSTEMTIME));
	MEMCPY_S(pMaxRange, &sysTimes[1], sizeof(SYSTEMTIME));

	return nCount;
}

int CXTMonthCalCtrl::GetMonthRange(CTime& refMinRange, CTime& refMaxRange, DWORD dwFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(dwFlags == GMR_DAYSTATE || dwFlags == GMR_VISIBLE);

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	int nCount = (int) ::SendMessage(m_hWnd, MCM_GETMONTHRANGE,
		(WPARAM) dwFlags, (LPARAM) &sysTimes);

	refMinRange = CTime(sysTimes[0]);
	refMaxRange = CTime(sysTimes[1]);

	return nCount;
}

BOOL CXTMonthCalCtrl::GetSelRange(LPSYSTEMTIME pMinRange, LPSYSTEMTIME pMaxRange) const
{
	ASSERT(m_hWnd != NULL);
	ASSERT((GetStyle() & MCS_MULTISELECT));

	ASSERT_POINTER(pMinRange, SYSTEMTIME);
	ASSERT_POINTER(pMaxRange, SYSTEMTIME);

	SYSTEMTIME sysTimes[2];
	BOOL bReturn = (BOOL) ::SendMessage(m_hWnd, MCM_GETSELRANGE,
										0, (LPARAM) &sysTimes);

	if (bReturn)
	{
		MEMCPY_S(pMinRange, &sysTimes[0], sizeof(SYSTEMTIME));
		MEMCPY_S(pMaxRange, &sysTimes[1], sizeof(SYSTEMTIME));
	}

	return bReturn;
}

BOOL CXTMonthCalCtrl::SetSelRange(const LPSYSTEMTIME pMinRange, const LPSYSTEMTIME pMaxRange)
{
	ASSERT(m_hWnd != NULL);
	ASSERT((GetStyle() & MCS_MULTISELECT));

	ASSERT_POINTER(pMinRange, SYSTEMTIME);
	ASSERT_POINTER(pMaxRange, SYSTEMTIME);

	SYSTEMTIME sysTimes[2];
	MEMCPY_S(&sysTimes[0], pMinRange, sizeof(SYSTEMTIME));
	MEMCPY_S(&sysTimes[1], pMaxRange, sizeof(SYSTEMTIME));

	return (BOOL) ::SendMessage(m_hWnd, MCM_SETSELRANGE,
		0, (LPARAM) &sysTimes);
}

BOOL CXTMonthCalCtrl::SetSelRange(const COleDateTime& refMinRange, const COleDateTime& refMaxRange)
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	BOOL bResult = FALSE;

	if (refMinRange.GetStatus() == COleDateTime::valid &&
		refMinRange.GetStatus() == COleDateTime::valid)
	{
		if (GetAsSystemTime(refMinRange, sysTimes[0]) &&
			GetAsSystemTime(refMaxRange, sysTimes[1]))
		{
			bResult = (BOOL)
				::SendMessage(m_hWnd, MCM_SETSELRANGE, 0, (LPARAM)sysTimes);
		}
	}

	return bResult;
}

BOOL CXTMonthCalCtrl::GetSelRange(COleDateTime& refMinRange, COleDateTime& refMaxRange) const
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETSELRANGE, 0, (LPARAM) &sysTimes);

	if (bResult)
	{
		refMinRange = COleDateTime(sysTimes[0]);
		refMaxRange = COleDateTime(sysTimes[1]);
	}
	return bResult;
}

BOOL CXTMonthCalCtrl::SetSelRange(const CTime& refMinRange, const CTime& refMaxRange)
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	BOOL bResult = FALSE;

	if (GetAsSystemTime(refMinRange, sysTimes[0]) &&
		GetAsSystemTime(refMaxRange, sysTimes[1]))
	{
		bResult = (BOOL)
			::SendMessage(m_hWnd, MCM_SETSELRANGE, 0, (LPARAM)sysTimes);
	}

	return bResult;
}

BOOL CXTMonthCalCtrl::GetSelRange(CTime& refMinRange, CTime& refMaxRange) const
{
	// control must have multiple select
	ASSERT((GetStyle() & MCS_MULTISELECT));
	ASSERT(::IsWindow(m_hWnd));

	SYSTEMTIME sysTimes[2];
	memset(sysTimes, 0, sizeof(sysTimes));
	BOOL bResult = (BOOL)
		::SendMessage(m_hWnd, MCM_GETSELRANGE, 0, (LPARAM) &sysTimes);

	if (bResult)
	{
		refMinRange = CTime(sysTimes[0]);
		refMaxRange = CTime(sysTimes[1]);
	}
	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
// DDX_ routines
/////////////////////////////////////////////////////////////////////////////
_XTP_EXT_CLASS void AFXAPI DDX_XTDateTimeCtrl(CDataExchange* pDX, int nIDC, COleDateTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CXTDateTimeCtrl* pWnd = (CXTDateTimeCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetTime(value);
	else
		pWnd->SetTime(value);
}

_XTP_EXT_CLASS void AFXAPI DDX_XTDateTimeCtrl(CDataExchange* pDX, int nIDC, CTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CXTDateTimeCtrl* pWnd = (CXTDateTimeCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetTime(value);
	else
		pWnd->SetTime(&value);
}

_XTP_EXT_CLASS void AFXAPI DDX_XTMonthCalCtrl(CDataExchange* pDX, int nIDC,
	COleDateTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CXTMonthCalCtrl* pWnd = (CXTMonthCalCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetCurSel(value);
	else
		pWnd->SetCurSel(value);
}

_XTP_EXT_CLASS void AFXAPI DDX_XTMonthCalCtrl(CDataExchange* pDX, int nIDC, CTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CXTMonthCalCtrl* pWnd = (CXTMonthCalCtrl*) CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
		pWnd->GetCurSel(value);
	else
		pWnd->SetCurSel(value);
}
/////////////////////////////////////////////////////////////////////////////
// DDV_ routines
/////////////////////////////////////////////////////////////////////////////
_XTP_EXT_CLASS void AFXAPI DDV_XTMinMaxDateTime(CDataExchange* pDX, CTime& refValue,
	const CTime* pMinRange, const CTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);
#if _MSC_VER > 1200 //MFC 7.0
	CXTDateTimeCtrl* pWnd =
		(CXTDateTimeCtrl*) pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl);
#else
	CXTDateTimeCtrl* pWnd =
		(CXTDateTimeCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
#endif //MFC 7.0

	if (!pDX->m_bSaveAndValidate)
	{
		if ((pMinRange != NULL && *pMinRange > refValue) ||
			(pMaxRange != NULL && *pMaxRange < refValue))
		{
#if _MSC_VER > 1200 // MFC 7.0
			TRACE(traceAppMsg, 0, "Warning: initial dialog data is out of range in "
				"control ID %d.\n", pDX->m_idLastControl);
#else
#ifdef _DEBUG
			int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
			TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
#endif
#endif//MFC 7.0
			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

_XTP_EXT_CLASS void AFXAPI DDV_XTMinMaxDateTime(CDataExchange* pDX, COleDateTime& refValue,
	const COleDateTime* pMinRange, const COleDateTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);
#if _MSC_VER > 1200 //MFC 7.0
	CXTDateTimeCtrl* pWnd =
		(CXTDateTimeCtrl*) pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl);
#else
	CXTDateTimeCtrl* pWnd =
		(CXTDateTimeCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
#endif //MFC 7.0

	if (!pDX->m_bSaveAndValidate)
	{
		if ((pMinRange != NULL && *pMinRange > refValue) ||
			(pMaxRange != NULL && *pMaxRange < refValue))
		{
#if _MSC_VER > 1200 //MFC 7.0
			TRACE(traceAppMsg, 0, "Warning: initial dialog data is out of range in "
				"control ID %d.\n", pDX->m_idLastControl);
#else
#ifdef _DEBUG
			int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
			TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
#endif
#endif //MFC 7.0
			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

_XTP_EXT_CLASS void AFXAPI DDV_XTMinMaxMonth(CDataExchange* pDX, CTime& refValue,
	const CTime* pMinRange, const CTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);
#if _MSC_VER > 1200 //MFC 7.0
	CXTMonthCalCtrl* pWnd =
		(CXTMonthCalCtrl*) pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl);
#else
	CXTMonthCalCtrl* pWnd =
		(CXTMonthCalCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
#endif //MFC 7.0

	if (!pDX->m_bSaveAndValidate)
	{
		if ((pMinRange != NULL && *pMinRange > refValue) ||
			(pMaxRange != NULL && *pMaxRange < refValue))
		{
#if _MSC_VER > 1200 //MFC 7.0
			TRACE(traceAppMsg, 0, "Warning: initial dialog data is out of range in "
				"control ID %d.\n", pDX->m_idLastControl);
#else
#ifdef _DEBUG
			int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
			TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
#endif
#endif //MFC 7.0
			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

_XTP_EXT_CLASS void AFXAPI DDV_XTMinMaxMonth(CDataExchange* pDX, COleDateTime& refValue,
	const COleDateTime* pMinRange, const COleDateTime* pMaxRange)
{
	ASSERT(pMinRange == NULL || pMaxRange == NULL || *pMinRange <= *pMaxRange);
#if _MSC_VER > 1200 //MFC 7.0
	CXTMonthCalCtrl* pWnd =
		(CXTMonthCalCtrl*) pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl);
#else
	CXTMonthCalCtrl* pWnd =
		(CXTMonthCalCtrl*) CWnd::FromHandle(pDX->m_hWndLastControl);
#endif //MFC 7.0

	if (!pDX->m_bSaveAndValidate)
	{
		if ((pMinRange != NULL && *pMinRange > refValue) ||
			(pMaxRange != NULL && *pMaxRange < refValue))
		{
#if _MSC_VER > 1200 //MFC 7.0
			TRACE(traceAppMsg, 0, "Warning: initial dialog data is out of range in "
				"control ID %d.\n", pDX->m_idLastControl);
#else
#ifdef _DEBUG
			int nIDC = GetWindowLong(pDX->m_hWndLastControl, GWL_ID);
			TRACE1("Warning: initial dialog data is out of range in control ID %d.\n", nIDC);
#endif
#endif //MFC 7.0
			return;     // don't stop now
		}
	}

	pWnd->SetRange(pMinRange, pMaxRange);
}

IMPLEMENT_DYNAMIC(CXTDateTimeCtrl, CWnd)

IMPLEMENT_DYNAMIC(CXTMonthCalCtrl, CWnd)
