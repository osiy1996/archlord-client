// AgsmAdminStatistic.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 12.

#include "AgsmAdmin.h"

void AgsmAdmin::PrintCurrentUserCount(FILE* pfFile)
{
/*
	if(!pfFile)
		return;

	UserLock();

	// Realy�� ���.	
	EnumCallback(AGSMADMIN_CB_CONCURRENT_USER, &m_stTotalUser, NULL);

	UserUnlock();

	// �ϴ� ����, ���� ��.
	return;

	// ��ü ������ ���� �Ѹ���.
	//fprintf(pfFile, "[Total]�� User : %4d\tHuman : %4d\tOrc : %4d\tHighest : %4d\n",
	//						m_stTotalUser.m_lTotalUserCount,
	//						m_stTotalUser.m_lHumanUserCount,
	//						m_stTotalUser.m_lOrcUserCount,
	//						m_stTotalUser.m_lHighestUserCount);

	//stAgsdAdminCurrentUser* pstCurrentUser = NULL;
	//stAgsdAdminCurrentUser** ppstCurrentUser = NULL;
	//INT32 lIndex = 0;
	//for(ppstCurrentUser = (stAgsdAdminCurrentUser**)m_csCurrentUserAdmin.GetObjectSequence(&lIndex); ppstCurrentUser;
	//	ppstCurrentUser = (stAgsdAdminCurrentUser**)m_csCurrentUserAdmin.GetObjectSequence(&lIndex))
	//{
	//	pstCurrentUser = *ppstCurrentUser;
	//	if(!pstCurrentUser)
	//		break;

	//	// �ڱⰡ ó���ϴ� ������� �� �� �ܴ�.
	//	if(pstCurrentUser->m_lServerID == GetThisServerID())
	//		fprintf(pfFile, "[Svr%2d]�� ", pstCurrentUser->m_lServerID);
	//	else
	//		fprintf(pfFile, "[Svr%2d]�� ", pstCurrentUser->m_lServerID);

	//	fprintf(pfFile, "User : %4d\tHuman : %4d\tOrc : %4d\tHighest : %4d\n",
	//						pstCurrentUser->m_lTotalUserCount,
	//						pstCurrentUser->m_lHumanUserCount,
	//						pstCurrentUser->m_lOrcUserCount,
	//						pstCurrentUser->m_lHighestUserCount);
	//}

	//fprintf(pfFile, "\n\n");

	fprintf(pfFile, "[Total]�� User : %5d\n", m_stTotalUser.m_lTotalUserCount);
	for (INT16 i=0; i<AGSMADMIN_CHARTID_MAX; ++i)
		fprintf(pfFile, "\tTID(%4d) : %5d\n", AGSDADMIN_CHARTID[i], m_stTotalUser.m_lUserCountTID[i]);
	fprintf(pfFile, "Highest : %5d\n", m_stTotalUser.m_lHighestUserCount);
	
	//stAgsdAdminCurrentUser* pstCurrentUser = NULL;
	//stAgsdAdminCurrentUser** ppstCurrentUser = NULL;
	//INT32 lIndex = 0;
	//for(ppstCurrentUser = (stAgsdAdminCurrentUser**)m_csCurrentUserAdmin.GetObjectSequence(&lIndex); ppstCurrentUser;
	//	ppstCurrentUser = (stAgsdAdminCurrentUser**)m_csCurrentUserAdmin.GetObjectSequence(&lIndex))
	//{
	//	pstCurrentUser = *ppstCurrentUser;
	//	if(!pstCurrentUser)
	//		break;

	//	// �ڱⰡ ó���ϴ� ������� �� �� �ܴ�.
	//	if(pstCurrentUser->m_lServerID == GetThisServerID())
	//		fprintf(pfFile, "[Svr%2d]�� ", pstCurrentUser->m_lServerID);
	//	else
	//		fprintf(pfFile, "[Svr%2d]�� ", pstCurrentUser->m_lServerID);

	//	for (INT16 i=0; i<AGSMADMIN_CHARTID_MAX; ++i)
	//		fprintf(pfFile, "TID(%2d) : %5d\t", m_stTotalUser.m_lUserCountTID[i]);

	//	fprintf(pfFile, "Highest : %5d\n", m_stTotalUser.m_lHighestUserCount);	
	//}

	fprintf(pfFile, "\n\n");	

	UserUnlock();
*/
}

// 2004.04.12. ���� ������ �� �α׸� �����.
void AgsmAdmin::WriteLogCurrentUserCount()
{
	if(!IsGameServer())
		return;

	BOOL bFirst = FALSE;
	DWORD dwNowTickCount = GetTickCount();

	// �ð��� ����ؼ� ���� �� �����.
	if(m_dwLastWriteLogCurrentUserTickCount != 0)
	{
		if(dwNowTickCount - m_dwLastWriteLogCurrentUserTickCount < AGSMADMIN_TIMEINTERVAL_CURRENT_USERCOUNT)
			return;
	}
	else
		bFirst = TRUE;

	m_dwLastWriteLogCurrentUserTickCount = dwNowTickCount;

	UserLock();

	// Realy�� ���.	
	EnumCallback(AGSMADMIN_CB_CONCURRENT_USER, &m_stTotalUser, NULL);

	UserUnlock();


	//CHAR szFileName[128], szTimeDate[32];
	//memset(szFileName, 0, 128);
	//memset(szTimeDate, 0, 32);

	//if(!m_pagpmAdmin->GetCurrentTimeDate(szTimeDate))
	//	return;

	//sprintf(szFileName, "./log/%s", AGSMADMIN_FILENAME_CURRENT_USERCOUNT);
	//memcpy(szFileName+strlen(szFileName), szTimeDate, 8);	// ex)20040412
	//strcat(szFileName, ".log");

	//ASSERT(strlen(szFileName) < 128);

	//FILE* pfFile = fopen(szFileName, "a+");
	//if(!pfFile)
	//	return;

	//if(bFirst)
	//{
	//	fprintf(pfFile, "=========================================\n");
	//	fprintf(pfFile, "= Server Started at %s\n\n", szTimeDate);
	//}
	//else
	//{
	//	fprintf(pfFile, "Now Time : %s\n", szTimeDate);
	//	PrintCurrentUserCount(pfFile);	// ���ο��� UserLock �� ���ϰ� �ȴ�.
	//}

	//fclose(pfFile);
}

void AgsmAdmin::WriteLogCurrentUserCountForExcel()
{
	return;
	if(!IsGameServer())
		return;

	DWORD dwNowTickCount = GetTickCount();

	// �ð��� ����ؼ� ���� �� �����.
	if(m_dwLastWriteLogCurrentUserTickCountForExcel != 0)
	{
		if(dwNowTickCount - m_dwLastWriteLogCurrentUserTickCountForExcel < AGSMADMIN_TIMEINTERVAL_CURRENT_USERCOUNT_FOR_EXCEL)
			return;
	}

	m_dwLastWriteLogCurrentUserTickCountForExcel = dwNowTickCount;

	CHAR szFileName[128], szTimeDate[32];
	memset(szFileName, 0, 128);
	memset(szTimeDate, 0, 32);

	if(!m_pagpmAdmin->GetCurrentTimeDate(szTimeDate))
		return;

	if(strlen(szTimeDate) != 12)	// ex) 200405240323
		return;

	INT32 lNowMinute = atoi(szTimeDate+10);
	if(lNowMinute != 0 && lNowMinute != 30)
		return;

	// �� �п� �ι� ������ �ϸ� ������.
	BOOL bFirst = FALSE;
	if(m_lLastMinuteWriteLogCurrentUserForExcel != -1)
	{
		if(m_lLastMinuteWriteLogCurrentUserForExcel == lNowMinute)
			return;
	}
	else
		bFirst = TRUE;

	m_lLastMinuteWriteLogCurrentUserForExcel = lNowMinute;

	sprintf(szFileName, "./log/%s", AGSMADMIN_FILENAME_CURRENT_USERCOUNT_FOR_EXCEL);
	memcpy(szFileName+strlen(szFileName), szTimeDate, 8);	// ex)20040524
	strcat(szFileName, ".log");
	
	ASSERT(strlen(szFileName) < 128);

	FILE* pfFile = fopen(szFileName, "a+");
	if(!pfFile)
		return;

	if(bFirst)
		fprintf(pfFile, "�ð�|���絿��|�ְ���|\n");

	CHAR szExcelTime[128];
	memset(szExcelTime, 0, 128);
	MakeExcelTime(szExcelTime, 128, szTimeDate);

	UserLock();
	fprintf(pfFile, "%s|%4d|%4d|\n", szExcelTime, m_stTotalUser.m_lTotalUserCount, m_stTotalUser.m_lHighestUserCount);
	UserUnlock();

	fclose(pfFile);
}

void AgsmAdmin::MakeExcelTime(CHAR* szExcelTime, INT32 lExcelTimeLength, CHAR* szTimeDate)
{
	if(!szExcelTime || !szTimeDate)
		return;

	if(strlen(szTimeDate) != 12)
		return;

	CHAR szBuf[16];
	memset(szBuf, 0, 16);

	memcpy(szBuf, szTimeDate, 4);
	szBuf[4] = '\0';;
	strcpy(szExcelTime, szBuf);
	strcat(szExcelTime, "-");

	memcpy(szBuf, szTimeDate+4, 2);
	szBuf[2] = '\0';;
	strcat(szExcelTime, szBuf);
	strcat(szExcelTime, "-");

	memcpy(szBuf, szTimeDate+6, 2);
	szBuf[2] = '\0';;
	strcat(szExcelTime, szBuf);
	strcat(szExcelTime, " ");

	memcpy(szBuf, szTimeDate+8, 2);
	szBuf[2] = '\0';;
	strcat(szExcelTime, szBuf);
	strcat(szExcelTime, ":");
	
	memcpy(szBuf, szTimeDate+10, 2);
	szBuf[2] = '\0';;
	strcat(szExcelTime, szBuf);

	ASSERT(strlen(szExcelTime) < lExcelTimeLength);
}

/*
BOOL AgsmAdmin::CBGetCurrentUserCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	INT32* plCurrentUserCount = (INT32*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	if(!plCurrentUserCount || !pThis)
		return FALSE;

	// 2004.05.20. steeple
	// UserLock() ���� �ʰ� ������ �ɷ� �ٲ�.
	// Lock �� ���� ����. ��������~~
	*plCurrentUserCount = pThis->m_lThisServerUserCount;
	return TRUE;
}
*/