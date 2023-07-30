#include <ApBase/ApBase.h>
#include "AgcmPreLODAdmin.h"

AgcmPreLODAdmin::AgcmPreLODAdmin()
{
}

AgcmPreLODAdmin::~AgcmPreLODAdmin()
{
}

BOOL AgcmPreLODAdmin::RemoveAllPreLODData(AgcdPreLOD *pstLOD)
{
	AgcdPreLODList	*pstList	= pstLOD->m_pstList;
    AgcdPreLODList	*pstTemp;

	// ����Ʈ�� �Ѱ��� ������ ���ƶ�!!!
    while(pstList)
    {
		// ���� ����Ʈ�� temp�� ����
        pstTemp								= pstList->m_pstNext;
		// ���� ����Ʈ�� ������,
        free(pstList);
		// temp�� ����� ���� ����Ʈ�� ����!
	    pstList								= pstTemp;
    }

	// ������ NULL�̰�����, Ȯ�λ��!
	pstLOD->m_pstList						= NULL;
	// �׷��� ������ ����...
	pstLOD->m_lNum							= 0;

	return TRUE;
}

AgcdPreLODData	*AgcmPreLODAdmin::AddPreLODData(AgcdPreLOD *pstLOD, AgcdPreLODData *pcsData)
{
	// Group-info ����
	AgcdPreLODList	*pstList			= (AgcdPreLODList *)malloc(sizeof(AgcdPreLODList));
	// ������ ����!
	if(pcsData)
		memcpy(&pstList->m_csData, pcsData, sizeof(AgcdPreLODData));
	else
		memset(&pstList->m_csData, 0, sizeof(AgcdPreLODData));
	// �ε��� ����
	pstList->m_csData.m_lIndex			= pstLOD->m_lNum;
	// Next ����
	pstList->m_pstNext					= pstLOD->m_pstList;
	// List ������ ����
	pstLOD->m_pstList					= pstList;	
	// ���� ����
	++pstLOD->m_lNum;
	
	return &pstList->m_csData;
}

BOOL AgcmPreLODAdmin::RemovePreLODData(AgcdPreLOD *pstLOD, INT32 lIndex)
{
	AgcdPreLODList	*pstList	= pstLOD->m_pstList;	

	if(!pstList)
		return FALSE; // ���ﲲ ����.. ��.��

	// ã�� �����Ͱ� ���� ����Ʈ�� �����Ͷ��..
	if(pstList->m_csData.m_lIndex == lIndex)
	{
		// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��...
		pstLOD->m_pstList		= pstList->m_pstNext;
		// ���� ����Ʈ�� ����������!
		free(pstList);
		return TRUE;
	}

	// �ӽ� ������ ���� ����Ʈ�� �����ϰ�...
	AgcdPreLODList	*pstTemp		= pstList;
	// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��...
	pstList						= pstTemp->m_pstNext;

	// ���� ����Ʈ�� ���������� ���ƶ�!
	while(pstList)
	{
		// ã�� �����Ͱ� ���� ����Ʈ�� �����Ͷ��..
		if(pstList->m_csData.m_lIndex == lIndex)
		{
			// �� ����Ʈ�� ���� ����Ʈ�� ���� �����ϰ�.
			pstTemp->m_pstNext	= pstList->m_pstNext; 
			// ���� ����Ʈ�� ����BoA��~
			free(pstList);
			return TRUE;
		}
		else
		{
			// �ӽú����� ���縮��Ʈ�� �����ϰ�...
			pstTemp				= pstList;
			// ���縮��Ʈ�� �������� �ѱ��!
			pstList				= pstTemp->m_pstNext;
		}
	}

	return FALSE;
}

AgcdPreLODData	*AgcmPreLODAdmin::GetPreLODData(AgcdPreLOD *pstLOD, INT32 lIndex)
{
	AgcdPreLODList	*pstList	= pstLOD->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
	while(pstList)
	{
		// ã�� �ε����� ����Ʈ�� �ִ�!
		if(pstList->m_csData.m_lIndex == lIndex)
			return &pstList->m_csData;

		// ���� ����Ʈ�� �������� �ѱ��!
		pstList					= pstList->m_pstNext;
	}

	return NULL;
}

// ����!!! : pstDest->m_pstList�� pstDest->m_lNum�� �ʱ�ȭ ��Ų��.
BOOL AgcmPreLODAdmin::CopyPreLOD(AgcdPreLOD *pstDest, AgcdPreLOD *pstSrc)
{
	pstDest->m_pstList			= NULL;
	pstDest->m_lNum				= 0;

//	AgcdPreLODList	*pstList	= pstSrc->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
//	while(pstList)
	AgcdPreLODData	*pcsData;
	for(INT32 lIndex = 0;; ++lIndex)
	{
		pcsData	= GetPreLODData(pstSrc, lIndex);
		if(!pcsData)
			break;

		// SRC �������� DEST�� Add�Ѵ�~
		if(!AddPreLODData(pstDest, pcsData))
			return FALSE;

		// ���� ����Ʈ�� �������� �ѱ��!
		//pstList					= pstList->m_pstNext;
	}

	return TRUE;
}