#include "AgcmObjectList.h"

#include <ApMemoryTracker/ApMemoryTracker.h>

#define	AGCM_OBJECT_GROUP_POOL				4500
#define	AGCM_OBJECT_TEMPLATE_GROUP_POOL		1000

AgcmObjectList::AgcmObjectList()
{
	m_csGroupPool.Initialize(sizeof(AgcdObjectGroupList), AGCM_OBJECT_GROUP_POOL);

//	m_pstAgcdAnimObjectGroupDataList = NULL;
}

AgcmObjectList::~AgcmObjectList()
{
}

///////////////////////////////////////////////////////////////////////////////
// Animation
/*BOOL AgcmObjectList::RemoveAllAnimObjectGroupData()
{
	AgcdAnimObjectGroupDataList	*pstEntry	= m_pstAgcdAnimObjectGroupDataList;
    AgcdAnimObjectGroupDataList	*pstTemp;

    while(pstEntry)
    {
        pstTemp = pstEntry->m_pstNext;
        free(pstEntry);
	    pstEntry = pstTemp;
    }

	m_pstAgcdAnimObjectGroupDataList		= NULL;

	return TRUE;
}

BOOL AgcmObjectList::AddAnimObjectGroupData(AgcdObjectGroupData *pcsData)
{
	if(CheckAnimObjectGroupData(pcsData))
		return FALSE;

	AgcdAnimObjectGroupDataList	*pstEntry		= (AgcdAnimObjectGroupDataList *)malloc(sizeof(AgcdAnimObjectGroupDataList));

	pstEntry->m_pcsData							= pcsData;
	pstEntry->m_pstNext							= m_pstAgcdAnimObjectGroupDataList;
	pstEntry->m_ulPrevTick						= 0;

	m_pstAgcdAnimObjectGroupDataList			= pstEntry;
	
	return TRUE;
}

BOOL AgcmObjectList::CheckAnimObjectGroupData(AgcdObjectGroupData *pcsData)
{
	AgcdAnimObjectGroupDataList	*pstEntry		= m_pstAgcdAnimObjectGroupDataList;

	while(pstEntry)
	{
		if (pcsData == pstEntry->m_pcsData)
			return TRUE;

		pstEntry								= pstEntry->m_pstNext;
	}

	return FALSE;
}

BOOL AgcmObjectList::RemoveAnimObjectGroupData(AgcdObjectGroupData *pcsData)
{
	AgcdAnimObjectGroupDataList	*pstEntry		= m_pstAgcdAnimObjectGroupDataList;
	AgcdAnimObjectGroupDataList	*pstTemp;

	if(!pstEntry)
		return FALSE;

	if(pstEntry->m_pcsData == pcsData)
	{
		m_pstAgcdAnimObjectGroupDataList		= pstEntry->m_pstNext;
		free(pstEntry);
		return TRUE;
	}

	pstTemp										= pstEntry;
	pstEntry									= pstTemp->m_pstNext;

	while(pstEntry)
	{
		if(pstEntry->m_pcsData == pcsData)
		{
			pstTemp->m_pstNext					= pstEntry->m_pstNext; 
			free(pstEntry);
			return TRUE;
		}
		else 
		{
			pstTemp								= pstEntry;
			pstEntry							= pstTemp->m_pstNext;
		}
	}

	return FALSE;
}*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Object Group
BOOL AgcmObjectList::RemoveAllObjectGroup(AgcdObjectGroup *pstGroup)
{
	AgcdObjectGroupList	*pstList	= pstGroup->m_pstList;
    AgcdObjectGroupList	*pstTemp;

	// ����Ʈ�� �Ѱ��� ������ ���ƶ�!!!
    while(pstList)
    {
		// ���� ����Ʈ�� temp�� ����
        pstTemp						= pstList->m_pstNext;
		// ���� ����Ʈ�� ������,
        m_csGroupPool.Free(pstList);
		// temp�� ����� ���� ����Ʈ�� ����!
	    pstList						= pstTemp;
    }

	// ������ NULL�̰�����, Ȯ�λ��!
	pstGroup->m_pstList							= NULL;
	// �׷��� ������ ����...
	pstGroup->m_lNum				= 0;

	return TRUE;
}

AgcdObjectGroupData *AgcmObjectList::AddObjectGroup(AgcdObjectGroup *pstGroup/*, AgcdObjectGroupData *pcsData*/)
{
	// Group-info ����
	AgcdObjectGroupList	*pstList		= (AgcdObjectGroupList *) m_csGroupPool.Alloc();
	// ������ ����!
	memset(&pstList->m_csData, 0, sizeof(AgcdObjectGroupData));
	// �ε��� ����
	pstList->m_csData.m_lIndex			= pstGroup->m_lNum;
	// Next ����
	pstList->m_pstNext					= pstGroup->m_pstList;
	// List ������ ����
	pstGroup->m_pstList					= pstList;	
	// ���� ����
	++pstGroup->m_lNum;
	
	return &pstList->m_csData;
}

/*BOOL AgcmObjectList::RemoveObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex)
{
	AgcdObjectGroupList	*pstList		= pstGroup->m_pstList;	

	if(!pstList)
		return FALSE; // ���ﲲ ����.. ��.��

	// ã�� �����Ͱ� ���� ����Ʈ�� �����Ͷ��..
	if(pstList->m_csData.m_lIndex == lIndex)
	{
		// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��...
		pstGroup->m_pstList				= pstList->m_pstNext;
		// ���� ����Ʈ�� ����������!
		m_csGroupPool.Free(pstList);

		// �ε����� ���ҽ�Ų��.
		pstList = pstGroup->m_pstList;
		while(pstList)
		{
			--pstList->m_csData.m_lIndex;

			pstList = pstList->m_pstNext;
		}

		--pstGroup->m_lNum;

		return TRUE;
	}

	// �ӽ� ������ ���� ����Ʈ�� �����ϰ�...
	AgcdObjectGroupList	*pstTemp		= pstList;
	// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��...
	pstList								= pstTemp->m_pstNext;

	// ���� ����Ʈ�� ���������� ���ƶ�!
	while(pstList)
	{
		// ã�� �����Ͱ� ���� ����Ʈ�� �����Ͷ��..
		if(pstList->m_csData.m_lIndex == lIndex)
		{
			// �� ����Ʈ�� ���� ����Ʈ�� ���� �����ϰ�.
			pstTemp->m_pstNext			= pstList->m_pstNext; 
			// ���� ����Ʈ�� ����BoA��~
			m_csGroupPool.Free(pstList);

			// �ε����� ���ҽ�Ų��.
			pstList = pstTemp->m_pstNext;
			while(pstList)
			{
				--pstList->m_csData.m_lIndex;

				pstList = pstList->m_pstNext;
			}

			--pstGroup->m_lNum;

			return TRUE;
		}
		else
		{
			// �ӽú����� ���縮��Ʈ�� �����ϰ�...
			pstTemp						= pstList;
			// ���縮��Ʈ�� �������� �ѱ��!
			pstList						= pstTemp->m_pstNext;
		}
	}

	return FALSE;
}*/

AgcdObjectGroupData *AgcmObjectList::GetObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex)
{
	AgcdObjectGroupList	*pstList	= pstGroup->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
	while(pstList)
	{
		// ã�� �ε����� ����Ʈ�� �ִ�!
		if(pstList->m_csData.m_lIndex == lIndex)
			return &pstList->m_csData;

		// ���� ����Ʈ�� �������� �ѱ��!
		pstList						= pstList->m_pstNext;
	}

	return NULL;
}

/*
BOOL AgcmObjectList::CheckObjectGroup(AgcdObjectGroup *pstGroup, INT32 lIndex)
{
	AgcdObjectGroupList	*pstList	= pstGroup->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
	while(pstList)
	{
		// ã�� �ε����� ����Ʈ�� �ִ�!
		if(pstList->m_stData.m_lIndex == lIndex)
			return TRUE;

		// ���� ����Ʈ�� �������� �ѱ��!
		pstList								= pstList->m_pstNext;
	}

	return FALSE;
}*/
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// ObjectTemplate Group
BOOL AgcmObjectList::RemoveAllObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup)
{
	AgcdObjectTemplateGroupList	*pstList	= pstGroup->m_pstList;
    AgcdObjectTemplateGroupList	*pstTemp;

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
	pstGroup->m_pstList						= NULL;
	// �׷��� ������ ����...
	pstGroup->m_lNum						= 0;

	return TRUE;
}

AgcdObjectTemplateGroupData	*AgcmObjectList::AddObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup/*, AgcdObjectTemplateGroupData *pcsData*/)
{
	// Group-info ����
	AgcdObjectTemplateGroupList	*pstList		= new AgcdObjectTemplateGroupList;
	// ������ ����!
/*	if (pcsData)
		memcpy(&pstList->m_csData, pcsData, sizeof(AgcdObjectTemplateGroupData));
	else*/
		memset(&pstList->m_csData, 0, sizeof(AgcdObjectTemplateGroupData));
	// �ε��� ����
	pstList->m_csData.m_lIndex			= pstGroup->m_lNum;
	// Next ����
	pstList->m_pstNext					= pstGroup->m_pstList;
	// List ������ ����
	pstGroup->m_pstList					= pstList;	
	// ���� ����
	++pstGroup->m_lNum;
	
	return &pstList->m_csData;
}

BOOL AgcmObjectList::RemoveObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex)
{
	if (!pstGroup->m_pstList)
		return FALSE; // ����� ����~

	AgcdObjectTemplateGroupList *pstTemp;
	AgcdObjectTemplateGroupList	*pstCurrent		= pstGroup->m_pstList;
	AgcdObjectTemplateGroupList *pstPrevious	= NULL;

	while (pstCurrent)
	{
		// ã���� �ϴ� �ε����� �´ٸ�...
		if (pstCurrent->m_csData.m_lIndex == lIndex)
		{
			if (!pstPrevious)
			{ // ����Ʈ�� ó���̶�� Group�� head������ �����Ѵ�.
				pstGroup->m_pstList				= pstCurrent->m_pstNext;
			}
			else
			{ // �� ����Ʈ�� ���� ����Ʈ�� ���� ����Ʈ�� ���� ����Ʈ�� �����Ѵ�.
				pstPrevious->m_pstNext			= pstCurrent->m_pstNext;

				
				{ // �ε����� �ٽ� �����Ѵ�.
					pstTemp							= pstGroup->m_pstList;
					while (pstTemp)
					{
						--pstTemp->m_csData.m_lIndex;

						if (pstTemp == pstPrevious)
							break;

						pstTemp						= pstTemp->m_pstNext;
					}
				}
			}

			// ã�� ����Ʈ�� �����Ѵ�.
			delete pstCurrent;
			// �׷��� ������ �ִ� ����Ʈ�� ������ �����Ѵ�.
			--pstGroup->m_lNum;

			return TRUE;
		}

		// �� ����Ʈ ������ �����ϰ�...
		pstPrevious								= pstCurrent;
		// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��.
		pstCurrent								= pstPrevious->m_pstNext;
	}

	return FALSE;

/*	AgcdObjectTemplateGroupList	*pstList		= pstGroup->m_pstList;	

	if (!pstList)
		return FALSE; // ���ﲲ ����.. ��.��

	// ã�� �����Ͱ� ���� ����Ʈ�� �����Ͷ��..
	if (pstList->m_csData.m_lIndex == lIndex)
	{
		// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��...
		pstGroup->m_pstList						= pstList->m_pstNext;
		// ���� ����Ʈ�� ����������!
		free(pstList);

		// �ε����� ���ҽ�Ų��.
		pstList = pstGroup->m_pstList;
		while (pstList)
		{
			--pstList->m_csData.m_lIndex;

			pstList = pstList->m_pstNext;
		}

		--pstGroup->m_lNum;

		return TRUE;
	}

	// �ӽ� ������ ���� ����Ʈ�� �����ϰ�...
	AgcdObjectTemplateGroupList	*pstTemp		= pstList;
	// ���� ����Ʈ�� ���� ����Ʈ�� �ѱ��...
	pstList										= pstTemp->m_pstNext;

	// ���� ����Ʈ�� ���������� ���ƶ�!
	while (pstList)
	{
		// ã�� �����Ͱ� ���� ����Ʈ�� �����Ͷ��..
		if (pstList->m_csData.m_lIndex == lIndex)
		{
			// �� ����Ʈ�� ���� ����Ʈ�� ���� �����ϰ�.
			pstTemp->m_pstNext					= pstList->m_pstNext; 
			// ���� ����Ʈ�� ����BoA��~
			free(pstList);

			// �ε����� ���ҽ�Ų��.
			pstList = pstTemp->m_pstNext;
			while (pstList)
			{
				--pstList->m_csData.m_lIndex;

				pstList = pstList->m_pstNext;
			}

			--pstGroup->m_lNum;

			return TRUE;
		}
		else
		{
			// �ӽú����� ���縮��Ʈ�� �����ϰ�...
			pstTemp								= pstList;
			// ���縮��Ʈ�� �������� �ѱ��!
			pstList								= pstTemp->m_pstNext;
		}
	}

	return FALSE;*/
}

AgcdObjectTemplateGroupData	*AgcmObjectList::GetObjectTemplateGroup(AgcdObjectTemplateGroup *pstGroup, INT32 lIndex)
{
	if( pstGroup->m_lNum <= lIndex )	return NULL;

	AgcdObjectTemplateGroupList	*pstList	= pstGroup->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
	while(pstList)
	{
		// ã�� �ε����� ����Ʈ�� �ִ�!
		if(pstList->m_csData.m_lIndex == lIndex)
			return &pstList->m_csData;

		// ���� ����Ʈ�� �������� �ѱ��!
		pstList								= pstList->m_pstNext;
	}

	return NULL;
}

// ����!!! : pstDest->m_pstList�� pstDest->m_lNum�� �ʱ�ȭ ��Ų��.
/*BOOL AgcmObjectList::CopyObjectTemplateGroup(AgcdObjectTemplateGroup *pstDest, AgcdObjectTemplateGroup *pstSrc)
{
	pstDest->m_pstList						= NULL;
	pstDest->m_lNum							= 0;

//	AgcdObjectTemplateGroupList	*pstList	= pstSrc->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
//	while(pstList)
	AgcdObjectTemplateGroupData *pcsData;
	for(INT32 lIndex = 0;; ++lIndex)
	{
		pcsData = GetObjectTemplateGroup(pstSrc, lIndex);
		if(!pcsData)
			break;

		// SRC �������� DEST�� Add�Ѵ�~
		if(!AddObjectTemplateGroup(pstDest, pcsData))
			return FALSE;

		// ���� ����Ʈ�� �������� �ѱ��!
//		pstList					= pstList->m_pstNext;
	}

	return TRUE;
}*/

/*
BOOL AgcmObjectList::CheckObjectTemplateGroup(AgcdObjectTemplateGroupInfo *pstList, INT32 lIndex)
{
	AgcdObjectTemplateGroupList	*pstList	= pstGroup->m_pstList;

	// ����Ʈ�� ���� ������ ����BoA��~
	while(pstList)
	{
		// ã�� �ε����� ����Ʈ�� �ִ�!
		if(pstList->m_stData.m_lIndex == lIndex)
			return TRUE;

		// ���� ����Ʈ�� �������� �ѱ��!
		pstList								= pstList->m_pstNext;
	}

	return FALSE;
}*/
///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
******************************************************************************/