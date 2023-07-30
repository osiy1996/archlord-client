#include "AgcmLODList.h"
#include <ApMemoryTracker/ApMemoryTracker.h>

AgcmLODList::AgcmLODList()
{
}

AgcmLODList::~AgcmLODList()
{
}

BOOL AgcmLODList::RemoveAllLODData(AgcdLOD *pstLOD)
{
	AgcdLODList	*pstList	= pstLOD->m_pstList;
    AgcdLODList	*pstTemp;

	// ����Ʈ�� �Ѱ��� ������ ���ƶ�!!!
    while(pstList)
    {
		// ���� ����Ʈ�� temp�� ����
        pstTemp								= pstList->m_pstNext;
		// ���� ����Ʈ�� ������,
        delete pstList;
		// temp�� ����� ���� ����Ʈ�� ����!
	    pstList								= pstTemp;
    }

	// ������ NULL�̰�����, Ȯ�λ��!
	pstLOD->m_pstList						= NULL;
	// �׷��� ������ ����...
	pstLOD->m_lNum							= 0;

	return TRUE;
}

AgcdLODData	*AgcmLODList::AddLODData(AgcdLOD *pstLOD, AgcdLODData *pcsData)
{
	// Group-info ����
	AgcdLODList	*pstList		= new AgcdLODList;
	// ������ ����!
	if(pcsData)
		memcpy(&pstList->m_csData, pcsData, sizeof(AgcdLODData));
	else
		memset(&pstList->m_csData, 0, sizeof(AgcdLODData));
	// �ε��� ����
	pstList->m_csData.m_lIndex			= pstLOD->m_lNum;
	// Next ����
	pstList->m_pstNext					= pstLOD->m_pstList;
	// List ������ ����
	pstLOD->m_pstList					= pstList;	
	// ���� ����
	++pstLOD->m_lNum;

	pstList->m_csData.m_pstAgcdLOD		= pstLOD;
	
	return &pstList->m_csData;
}

BOOL AgcmLODList::RemoveLODData(AgcdLOD *pstLOD, INT32 lIndex)
{
	AgcdLODList	*pstList		= pstLOD->m_pstList;	

	if(!pstList)
		return FALSE; // ���ﲲ ����.. ��.��

	// ã�� �����Ͱ� ���� ����Ʈ�� �����Ͷ��..
	if(pstList->m_csData.m_lIndex == lIndex)
	{
		// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��...
		pstLOD->m_pstList		= pstList->m_pstNext;
		// ���� ����Ʈ�� ����������!
		delete pstList;

		// �ε����� ���ҽ�Ų��.
		pstList = pstLOD->m_pstList;
		while(pstList)
		{
			--pstList->m_csData.m_lIndex;

			pstList = pstList->m_pstNext;
		}

		--pstLOD->m_lNum;

		return TRUE;
	}

	// �ӽ� ������ ���� ����Ʈ�� �����ϰ�...
	AgcdLODList	*pstTemp		= pstList;
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
			delete pstList;

			// �ε����� ���ҽ�Ų��.
			pstList = pstTemp->m_pstNext;
			while(pstList)
			{
				--pstList->m_csData.m_lIndex;

				pstList = pstList->m_pstNext;
			}

			--pstLOD->m_lNum;

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

AgcdLODData	*AgcmLODList::GetLODData(AgcdLOD *pstLOD, INT32 lIndex)
{
	AgcdLODList	*pstList	= pstLOD->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
	while(pstList)
	{
		// ã�� �ε����� ����Ʈ�� �ִ�!
		if(pstList->m_csData.m_lIndex == lIndex)
			return &pstList->m_csData;

		// ���� ����Ʈ�� �������� �ѱ��!
		pstList				= pstList->m_pstNext;
	}

	return NULL;
}
