
#include <AgcmAnimation/AgcaAnimation.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <ApMemoryTracker/ApMemoryTracker.h>

extern	AgcmResourceLoader*		g_pcsAgcmResourceLoader;

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcaAnimation2::AgcaAnimation2()
{
	m_ulNumFlags					= 0;
	m_ulNumAttachedData				= 0;
	m_pszPath						= NULL;
	m_pszExtension					= NULL;
	
	memset(m_apszKeyAttachedData, 0, sizeof (CHAR *) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_aulSizeAttachedData, 0, sizeof (UINT32) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_pafRemoveAttachedDataCB, 0, sizeof (AcCallbackData2) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_pavRemoveAttachedDataCBData, 0, sizeof (PVOID) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcaAnimation2::~AgcaAnimation2()
{
	if (m_pszPath)
		delete [] m_pszPath;
	if (m_pszExtension)
		delete [] m_pszExtension;

	for (UINT32 lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
	{
		if (m_apszKeyAttachedData[lIndex])
		{
			delete [] m_apszKeyAttachedData[lIndex];
			m_apszKeyAttachedData[lIndex]	= NULL;
		}
	}

	RemoveAllRtAnim();

	m_csAdminFlags.RemoveObjectAll();
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
VOID AgcaAnimation2::SetAnimationPathAndExtension(CHAR *pszPath, CHAR *pszExtension)
{
	if (m_pszPath)
		delete [] m_pszPath;
	if (m_pszExtension)
		delete [] m_pszExtension;

	m_pszPath		= new CHAR [strlen(pszPath) + 1];
	strcpy(m_pszPath, pszPath);

	if (pszExtension)
	{
		m_pszExtension	= new CHAR [strlen(pszExtension) + 1];
		strcpy(m_pszExtension, pszExtension);
	}
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::SetMaxRtAnim(UINT32 ulMaxAnimation)
{
	return m_csAdminRtAnim.InitializeObject(sizeof (AgcdRtAnim), ulMaxAnimation);
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::SetMaxFlags(UINT32 ulMaxFlags)
{
	return m_csAdminFlags.InitializeObject(sizeof (AgcdAnimationFlag), ulMaxFlags);
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::SetAttachedData(CHAR *pszKey, UINT32 ulDataSize, AcCallbackData2 pfRemoveCB, PVOID pvRemoveCBData)
{
	if (m_ulNumAttachedData >= D_AGCA_ANIMATION_MAX_ATTACHED_DATA)
		return FALSE;

	if (	(m_apszKeyAttachedData[m_ulNumAttachedData]) &&
			(m_aulSizeAttachedData[m_ulNumAttachedData]) &&
			(m_pafRemoveAttachedDataCB[m_ulNumAttachedData]) &&
			(m_pavRemoveAttachedDataCBData[m_ulNumAttachedData])	)
		return FALSE;

	m_apszKeyAttachedData[m_ulNumAttachedData]	= new CHAR [strlen(pszKey) + 1];
	strcpy(m_apszKeyAttachedData[m_ulNumAttachedData], pszKey);

	m_aulSizeAttachedData[m_ulNumAttachedData]			= ulDataSize;
	m_pafRemoveAttachedDataCB[m_ulNumAttachedData]		= pfRemoveCB;
	m_pavRemoveAttachedDataCBData[m_ulNumAttachedData]	= pvRemoveCBData;

	++m_ulNumAttachedData;

	return TRUE;
}

/******************************************************************************
* Purpose :
* Desc : �̳༮�� reference count�� ���ҽ�Ų��.
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveRtAnim(AgcdAnimData2 *pcsAnim)
{
	if ((!pcsAnim) || (!pcsAnim->m_pcsRtAnim))
		return FALSE;

	AuAutoLock	lock(m_csCSection);
	if (!lock.Result()) return FALSE;
	//m_csCSection.Lock();

	--pcsAnim->m_pcsRtAnim->m_lReference;

	/*
	FILE	*fp = fopen("anim_log.txt", "at");
	fprintf(fp, "Remove Animation '%s' %x (RefCount = %d)\n", pcsAnim->m_pszRtAnimName, pcsAnim->m_pcsRtAnim, pcsAnim->m_pcsRtAnim->m_lReference);
	fclose(fp);
	*/

	if (pcsAnim->m_pcsRtAnim->m_lReference < 1)
	{
		if (pcsAnim->m_pcsRtAnim->m_pstAnimation)
		{
			RtAnimAnimationDestroy(pcsAnim->m_pcsRtAnim->m_pstAnimation);
			pcsAnim->m_pcsRtAnim->m_pstAnimation = NULL;
		}

		m_csAdminRtAnim.RemoveObject(pcsAnim->m_pszRtAnimName);
	}

	pcsAnim->m_pcsRtAnim	= NULL;

	//m_csCSection.Unlock();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveRtAnim(AgcdAnimation2 *pcsAnimation2)
{
	if (!pcsAnimation2)
		return FALSE;

	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;
	while (pcsCurrent)
	{
		RemoveRtAnim(pcsCurrent);

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
* Desc : �̳༮�� reference count�� ������Ų��.
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::ReadRtAnim(AgcdAnimData2 *pcsAnim)
{
	if (!pcsAnim)
		return FALSE;

	AuAutoLock	lock(m_csCSection);
	if (!lock.Result()) return FALSE;
	//m_csCSection.Lock();

	/*
	FILE	*fp = fopen("anim_log.txt", "at");
	fprintf(fp, "Read Animation '%s' %x (RefCount = %d)\n", pcsAnim->m_pszRtAnimName, pcsAnim->m_pcsRtAnim, pcsAnim->m_pcsRtAnim ? pcsAnim->m_pcsRtAnim->m_lReference : 0);
	fclose(fp);
	*/

	if (!pcsAnim->m_pcsRtAnim)
	{
		if (!pcsAnim->m_pszRtAnimName)
		{
			ASSERT(!"AgcaAnimation2::ReadRtAnim() No AnimName!!!");
			//m_csCSection.Unlock();

			return FALSE;
		}

		pcsAnim->m_pcsRtAnim	= GetRtAnim(pcsAnim->m_pszRtAnimName);
		if (!pcsAnim->m_pcsRtAnim)
		{
			ASSERT(!"AgcaAnimation2::ReadRtAnim() No Anim!!!");
			//m_csCSection.Unlock();

			return FALSE;
		}
	}
	else if (!pcsAnim->m_pszRtAnimName)
	{
		ASSERT(!"AgcaAnimation2::ReadRtAnim() No AnimName!!!");
		//m_csCSection.Unlock();

		return FALSE;
	}

	++pcsAnim->m_pcsRtAnim->m_lReference;

	if (pcsAnim->m_pcsRtAnim->m_pstAnimation)
	{
		//m_csCSection.Unlock();
		return TRUE;
	}

	//m_csCSection.Unlock();

	CHAR		szFullPath[256];
	memset(szFullPath, 0, sizeof (CHAR) * 256);
	
	if (m_pszExtension)
		sprintf(szFullPath, "%s%s.%s", m_pszPath, pcsAnim->m_pszRtAnimName, m_pszExtension);
	else
		sprintf(szFullPath, "%s%s", m_pszPath, pcsAnim->m_pszRtAnimName);

	//pcsAnim->m_pcsRtAnim->m_pstAnimation	= RtAnimAnimationRead(szFullPath);
	pcsAnim->m_pcsRtAnim->m_pstAnimation	= g_pcsAgcmResourceLoader->LoadRtAnim(szFullPath);
	if (!pcsAnim->m_pcsRtAnim->m_pstAnimation)
		return FALSE;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
BOOL AgcaAnimation2::ReadRtAnim(AgcdAnimation2 *pcsAnimation2)
{
	if (!pcsAnimation2)
		return FALSE;

	if (!m_pszPath)
	{
		ASSERT(!"Path�� ���� �����ؾߵ�!!!");
		return FALSE;
	}

//	CHAR		szFullPath[256];
//	memset(szFullPath, 0, sizeof (CHAR) * 256);

	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;
	while (pcsCurrent)
	{
		if (!ReadRtAnim(pcsCurrent))
			return FALSE;

		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
AgcdRtAnim *AgcaAnimation2::AddRtAnim(CHAR *pszName)
{
	AgcdRtAnim	csRtAnim;
	AgcdRtAnim	*pcsRtAnim;

	pcsRtAnim	= (AgcdRtAnim *)(m_csAdminRtAnim.AddObject((PVOID)(&csRtAnim), pszName));

	return pcsRtAnim;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcdRtAnim *AgcaAnimation2::GetRtAnim(CHAR *pszName, BOOL bAdd)
{
	AgcdRtAnim	*pcsRtAnim	= (AgcdRtAnim *)(m_csAdminRtAnim.GetObject(pszName));

	if ((!pcsRtAnim) && (bAdd))
	{
		pcsRtAnim	= AddRtAnim(pszName);
	}

	return pcsRtAnim;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::CreateAnimData(AgcdRtAnim *pcsRtAnim, BOOL bAttachData)
{
	AgcdAnimData2	*pcsAnimData2	= new AgcdAnimData2();
	pcsAnimData2->m_pcsRtAnim		= pcsRtAnim;

	if ((bAttachData) && (m_ulNumAttachedData > 0))
	{
		pcsAnimData2->m_pavAttachedData	= new PVOID [m_ulNumAttachedData];

		for (UINT32 lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
		{
			if (m_aulSizeAttachedData[lIndex])
			{
				pcsAnimData2->m_pavAttachedData[lIndex]	= new BYTE [m_aulSizeAttachedData[lIndex]];
				memset(pcsAnimData2->m_pavAttachedData[lIndex], 0, m_aulSizeAttachedData[lIndex]);
			}
		}
	}

	return pcsAnimData2;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::AddAnimData(CHAR *pszName, BOOL bAttachData)
{
	/*AgcdRtAnim		*pcsRtAnim		= GetRtAnim(pszName);
	if (!pcsRtAnim)
		return NULL;

	AgcdAnimData2	*pcsAnimData	= CreateAnimData(pcsRtAnim, bAttachData);*/
	AgcdAnimData2	*pcsAnimData	= CreateAnimData(NULL, bAttachData);
	if (!pcsAnimData)
		return NULL;

	if (pcsAnimData->m_pszRtAnimName)
		return NULL;

	pcsAnimData->m_pszRtAnimName	= new CHAR [strlen(pszName) + 1];
	strcpy(pcsAnimData->m_pszRtAnimName, pszName);

	return pcsAnimData;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::AddAnimation(AgcdAnimation2 *pcsAnimation2, AgcdRtAnim *pcsRtAnim, CHAR *pszName)
{
	AgcdAnimData2	*pcsAnimData2	= CreateAnimData(pcsRtAnim, TRUE);
	if (!pcsAnimData2)
		return NULL;

	if (pcsAnimData2->m_pszRtAnimName)
		return NULL;
	
	pcsAnimData2->m_pszRtAnimName	= new CHAR [strlen(pszName) + 1];
	strcpy(pcsAnimData2->m_pszRtAnimName, pszName);

	if (!pcsAnimation2->m_pcsTail)
	{
		pcsAnimation2->m_pcsHead	= pcsAnimation2->m_pcsTail	= pcsAnimData2;
	}
	else
	{
		pcsAnimation2->m_pcsTail->m_pcsNext	= pcsAnimData2;
		pcsAnimation2->m_pcsTail	= pcsAnimData2;
	}

	return pcsAnimData2;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::AddAnimation(AgcdAnimation2 **ppcsAnimation2, CHAR *pszName)
{
	if (!(*ppcsAnimation2))
		(*ppcsAnimation2)	= new AgcdAnimation2();

	/*AgcdRtAnim	*pcsRtAnim	= GetRtAnim(pszName);
	if (!pcsRtAnim)
		return NULL;

	return AddAnimation(*ppcsAnimation2, pcsRtAnim, pszName);*/
	return AddAnimation(*ppcsAnimation2, NULL, pszName);
}

/******************************************************************************
* Purpose :
*
* 240205. BOB
******************************************************************************/
BOOL AgcaAnimation2::CopyAnimation(AgcdAnimation2 *pcsSrc, AgcdAnimation2 **ppcsDest)
{
	if (!pcsSrc)
		return TRUE; // skip

	if (!(*ppcsDest))
		(*ppcsDest)	= new AgcdAnimation2();

	AgcdAnimData2	*pcsCurrentSrcAnimData	= pcsSrc->m_pcsHead;
	while (pcsCurrentSrcAnimData)
	{
		if (pcsCurrentSrcAnimData->m_pszRtAnimName)
		{
			if (!AddAnimation(*(ppcsDest), NULL, pcsCurrentSrcAnimData->m_pszRtAnimName))
				return FALSE;
		}

		pcsCurrentSrcAnimData	= pcsCurrentSrcAnimData->m_pcsNext;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::DestroyAnimData(AgcdAnimData2 *pcsAnimData)
{
	if (!pcsAnimData)
		return FALSE;

	{ // AgcdRtAnim ����.
		RemoveRtAnim(pcsAnimData);

		if (pcsAnimData->m_pszRtAnimName)
		{
			delete [] pcsAnimData->m_pszRtAnimName;
			pcsAnimData->m_pszRtAnimName	= NULL;
		}
	}

	if (pcsAnimData->m_pavAttachedData)
	{ // AttachedData ����.

		for (UINT32 lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
		{
			if (pcsAnimData->m_pavAttachedData[lIndex])
			{
				if (m_pafRemoveAttachedDataCB[lIndex])
				{
					m_pafRemoveAttachedDataCB[lIndex](
						pcsAnimData->m_pavAttachedData[lIndex],
						m_pavRemoveAttachedDataCBData[lIndex]		);
				}

				delete [] pcsAnimData->m_pavAttachedData[lIndex];
				pcsAnimData->m_pavAttachedData[lIndex]	= NULL;
			}
		}

		delete [] pcsAnimData->m_pavAttachedData;		
		pcsAnimData->m_pavAttachedData	= NULL;
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 221204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAnimData(AgcdAnimData2 **ppcsAnimData)
{
	if (!ppcsAnimData)
		return FALSE;

	if (!(*ppcsAnimData))
		return TRUE; // skip

	if (!DestroyAnimData(*ppcsAnimData))
		return FALSE;

	delete (*ppcsAnimData);
	(*ppcsAnimData)	= NULL;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 301204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAnimation(AgcdAnimation2 *pcsAnimation2, CHAR *pszName)
{
	if (!pcsAnimation2)
		return FALSE;

	AgcdAnimData2	*pcsPrev	= NULL;
	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;
	while (pcsCurrent)
	{
		if (!strcmp(pcsCurrent->m_pszRtAnimName, pszName))
		{
			DestroyAnimData(pcsCurrent);

			if (pcsPrev)
			{
				pcsPrev->m_pcsNext			= pcsCurrent->m_pcsNext;
			}
			else
			{
				pcsAnimation2->m_pcsHead	= pcsCurrent->m_pcsNext;
				if( !pcsAnimation2->m_pcsHead )
					pcsAnimation2->m_pcsTail = NULL;
			}

			delete pcsCurrent;

			return TRUE;
		}

		pcsPrev		= pcsCurrent;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return FALSE;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAllAnimation(AgcdAnimation2 **ppcsAnimation2)
{
	if (	(!ppcsAnimation2) ||
			(!(*ppcsAnimation2))	)
		return TRUE; // skip.

	AgcdAnimData2	*pcsNext	= NULL;
	AgcdAnimData2	*pcsCurrent	= (*ppcsAnimation2)->m_pcsHead;
	while (pcsCurrent)
	{
		pcsNext		= pcsCurrent->m_pcsNext;

		DestroyAnimData(pcsCurrent);

		delete pcsCurrent;

		pcsCurrent	= pcsNext;
	}

	delete (*ppcsAnimation2);

	(*ppcsAnimation2)	= NULL;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 031204. BOB
******************************************************************************/
BOOL AgcaAnimation2::RemoveAllRtAnim()
{
	INT32	lIndex = 0;

	for (	AgcdRtAnim *pcsRtAnim = (AgcdRtAnim *)(m_csAdminRtAnim.GetObjectSequence(&lIndex));
			pcsRtAnim;
			pcsRtAnim = (AgcdRtAnim *)(m_csAdminRtAnim.GetObjectSequence(&lIndex))				)
	{
		if (pcsRtAnim->m_pstAnimation)
		{
			RtAnimAnimationDestroy(pcsRtAnim->m_pstAnimation);
			pcsRtAnim->m_pstAnimation	= NULL;
		}
	}

	m_csAdminRtAnim.RemoveObjectAll();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 071204. BOB
******************************************************************************/
AgcdAnimData2 *AgcaAnimation2::GetAnimData(AgcdAnimation2 *pcsAnimation2, INT32 lIndex)
{
	if (!pcsAnimation2)
		return NULL;

	INT32			lCount		= 0;
	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;

	while (pcsCurrent)
	{
		if (lCount == lIndex)
			return pcsCurrent;

		++lCount;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return NULL;
}

/******************************************************************************
* Purpose :
* Desc : ���н� -1�� �����Ѵ�.
* 071204. BOB
******************************************************************************/
INT32 AgcaAnimation2::GetAnimDataIndex(AgcdAnimation2 *pcsAnimation2, AgcdAnimData2 *pcsAnimData2)
{
	if (!pcsAnimation2)
		return -1;

	INT32			lCount		= 0;
	AgcdAnimData2	*pcsCurrent	= pcsAnimation2->m_pcsHead;

	while (pcsCurrent)
	{
		if (pcsCurrent == pcsAnimData2)
			return lCount;

		++lCount;
		pcsCurrent	= pcsCurrent->m_pcsNext;
	}

	return -1;
}

/******************************************************************************
* Purpose :
*
* 091204. BOB
******************************************************************************/
BOOL AgcaAnimation2::AddFlags(AgcdAnimationFlag **ppFlags)
{
	++m_ulNumFlags;

	AgcdAnimationFlag	csFlags;
	*(ppFlags)	= (AgcdAnimationFlag *)(m_csAdminFlags.AddObject((PVOID)(&csFlags), m_ulNumFlags));

	return (*ppFlags) ? (TRUE) : (FALSE);
}

/******************************************************************************
* Purpose :
*
* 171204. BOB
******************************************************************************/
PVOID AgcaAnimation2::GetAttachedData(CHAR *pszKey, AgcdAnimData2 *pcsData)
{
	//@{ kday 20050403
	//check validation
	//if (!pcsData->m_pavAttachedData)
	if (!pcsData || !pcsData->m_pavAttachedData)
	//@} kday
		return NULL;

	UINT32 lIndex;
	for (lIndex = 0; lIndex < m_ulNumAttachedData; ++lIndex)
	{
		if (!m_apszKeyAttachedData[lIndex])
			return NULL;

		if (!strcmp(m_apszKeyAttachedData[lIndex], pszKey))
			break;
	}

	return pcsData->m_pavAttachedData[lIndex];
}

/******************************************************************************
******************************************************************************/



AgcdAnimationTime::AgcdAnimationTime()
{
	m_pcsList					= NULL;
	m_lNumCurCB					= 0;
	m_lNumMaxCB					= 0;
	m_fCurrentTime				= 0;
	m_fDuration					= 0;
}

AgcdAnimationTime::~AgcdAnimationTime()
{
	ResetAnimCB(FALSE);
}

VOID AgcdAnimationTime::SetMaxCallback(INT32 lMax)
{
	m_lNumMaxCB	= lMax;
}

BOOL AgcdAnimationTime::AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData)
{
	if (ppstCurList)
	{
		// ����Ʈ�� �Ҵ��Ѵ�.
		*(ppstCurList)	= new AgcdAnimationCallbackList();

		if (!(*ppstCurList))
			return FALSE;

		// �Ҵ�� ����Ʈ�� �����͸� copy�Ѵ�...
		memcpy(&((*(ppstCurList))->m_csData), pcsData, sizeof(AgcdAnimationCallbackData));

		// ���� ����Ʈ�� �����Ѵ�.
		(*(ppstCurList))->m_pcsNext		= pstNextList;

		// ����� �����Ѵ�.
		(*(ppstCurList))->m_pvClass		= this;

		// �ʿ��� callback�� �����Ѵ�.
		(*(ppstCurList))->m_pfLoopFunc	= NULL;
		(*(ppstCurList))->m_pfEndFunc	= NULL;
		(*(ppstCurList))->m_pfCustFunc	= NULL;

		// �� ����Ʈ�� ������ �����Ѵ�.
		if (pstPreList)
		{
			pstPreList->m_pcsNext = *(ppstCurList);
		}

		++m_lNumCurCB;
	}

	return TRUE;
}

BOOL AgcdAnimationTime::AddAnimCB(AgcdAnimationCallbackData *pcsData)
{
	// ��ü duration���� ū�� �˻�.
	if (m_fDuration < pcsData->m_fTime)
		return FALSE;

	// �� á��?
	if ((m_lNumMaxCB) && (m_lNumMaxCB <= m_lNumCurCB))
		return FALSE;

	AgcdAnimationCallbackList	*pcsCurList, *pcsPreList;

	// ���� �ִϸ��̼��� ����ð����� ũ�ų� ������...
	if (m_fCurrentTime <= pcsData->m_fTime)
	{
		// ����Ʈ�� ������...
		if (m_pcsList)
		{
			// ���� ����Ʈ�� ����ִ� �ð����� �۴ٸ�...
			if (m_pcsList->m_csData.m_fTime >= pcsData->m_fTime)
			{
				// ��� pcsCurList�� �����Ѵ�.
				pcsCurList = m_pcsList;

				// ���� ����Ʈ�� ���� �༮�� �߰��Ѵ�.
				if (!AddAnimList(NULL, &m_pcsList, pcsCurList, pcsData))
					return FALSE;

				return TRUE;
			}
/*			else if (m_pcsList->m_csData.m_fTime == pcsData->m_fTime) // ���� ����Ʈ�� ����ִ� �ð��� ���ٸ�...
			{
				// replace!!! -_-/
				memcpy(&m_pcsList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
			}*/

			pcsPreList	= m_pcsList;
			pcsCurList	= m_pcsList->m_pcsNext;
			// ����Ʈ�� ����...
			while (pcsCurList)
			{
				// ����Ʈ�� �ð����� �۴ٸ�...
				if (pcsCurList->m_csData.m_fTime >= pcsData->m_fTime)
				{
					// �� ����Ʈ�� ���� ����Ʈ ���̿� �߰��Ѵ�...
					return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, pcsCurList, pcsData);
				}
/*				else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // ����Ʈ�� �ð��� ���ٸ�...
				{
					// replace!!! -_-/
					memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

					return TRUE;
				}*/

				// ���� ����Ʈ�� �����Ѵ�...
				pcsPreList	= pcsCurList;
				pcsCurList	= pcsCurList->m_pcsNext;
			}

			return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, NULL, pcsData);
		}
		else // ����Ʈ�� ������...
		{
			if (!AddAnimList(NULL, &m_pcsList, NULL, pcsData))
				return FALSE;

			return TRUE;
		}
	}
	else // ���� �ִϸ��̼��� ����ð����� ������...
	{
		// ����Ʈ�� ������...
		if (!m_pcsList)
		{
			if(!AddAnimList(NULL, &m_pcsList, NULL, pcsData))
				return FALSE;

			return TRUE;
		}

		pcsPreList	= m_pcsList;
		pcsCurList	= m_pcsList->m_pcsNext;
		// ����Ʈ�� ����...
		while (pcsCurList)
		{
			// ����Ʈ�� �ð����� ũ�ٸ�...
			if (pcsCurList->m_csData.m_fTime <= pcsData->m_fTime)
			{
				// ���� ����Ʈ�� ���� ����Ʈ ���̿� �߰��Ѵ�...
				return AddAnimList(pcsCurList, &pcsCurList->m_pcsNext, pcsCurList->m_pcsNext, pcsData);
			}
/*			else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // ����Ʈ�� �ð��� ���ٸ�...
			{
				// replace!!! -_-/
				memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
			}*/

			// ���� ����Ʈ�� �����Ѵ�...
			pcsPreList	= pcsCurList;
			pcsCurList	= pcsCurList->m_pcsNext;
		}

		// �����Ϸ��� �ð����� ����Ʈ�� �ִ� �ð����� ��� ���� ������, �� �������� �ִ´�.
		return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, NULL, pcsData);
	}

	return FALSE;
}

VOID AgcdAnimationTime::ResetAnimCB(BOOL InfluenceNextAnimation)
{
	INT32						lNum = 0;
	AgcdAnimationCallbackList	*pcsCurrent = m_pcsList;
	AgcdAnimationCallbackList	*pcsPre = NULL, *pcsNext = NULL;
	while (pcsCurrent)
	{
		pcsNext = pcsCurrent->m_pcsNext;

		if (InfluenceNextAnimation)
		{
			if (!pcsCurrent->m_csData.m_bInfluenceNextAnimation)
			{
				if (pcsPre)
					pcsPre->m_pcsNext	= pcsCurrent->m_pcsNext;
				else
					m_pcsList			= pcsNext;

				delete pcsCurrent;
			}
			else
			{
				++lNum;

				pcsCurrent->m_csData.m_fTime					-= m_fCurrentTime;
				if (pcsCurrent->m_csData.m_fTime < 0.0f)
					pcsCurrent->m_csData.m_fTime				= 0.0f;
				// ���� �ݺ� ����~ -_-;;
				pcsCurrent->m_csData.m_bInfluenceNextAnimation	= FALSE;

				pcsPre											= pcsCurrent;
			}
		}
		else
		{
			delete pcsCurrent;
		}

		pcsCurrent	= pcsNext;
	}

//	m_pcsList		= NULL;
//	m_lNumCurCB		= 0;

	if (!InfluenceNextAnimation)
		m_pcsList	= NULL;

	m_lNumCurCB		= lNum;
}

AgcdAnimationCallbackList *AgcdAnimationTime::HeadMoveToTail(AgcdAnimationCallbackList *pstHead)
{
	// ����� ���� ����Ʈ�� ������...(������ ����Ʈ�� 1���̹Ƿ�, ������ �ʿ����.)
	if (pstHead->m_pcsNext)
	{
		// ���� ����Ʈ�� �����Ѵ�.
		m_pcsList								= pstHead->m_pcsNext;

		AgcdAnimationCallbackList	*pstTemp	= m_pcsList;

		// ������ ����Ʈ���� ����.
		while (pstTemp->m_pcsNext)
		{
			pstTemp								= pstTemp->m_pcsNext;
		}

		// ��带 ������ ����Ʈ�� �����ϰ�.
		pstTemp->m_pcsNext						= pstHead;
		// ����� ���� ����Ʈ�� NULL�� �ʱ�ȭ�Ѵ�.
		pstHead->m_pcsNext						= NULL;

		// ���� ����Ʈ�� �����Ѵ�!
		return m_pcsList;
	}

	// ��尡 �� ���� ����Ʈ�̴�! (����Ʈ�� 1��)
	return pstHead;
}

AgcdAnimationCallbackList *AgcdAnimationTime::RemoveAnimList(AgcdAnimationCallbackList *pstCurList)
{
	if (!pstCurList)
		return NULL;
	// ������ �����ϰ�...
	m_pcsList = pstCurList->m_pcsNext;
	// ����������~ �d�d~ -_-;;
	delete pstCurList;

	--m_lNumCurCB;

	return m_pcsList;
}

BOOL AgcdAnimationTime::AddTime(FLOAT fTime)
{
	m_fCurrentTime	+= fTime;
	if (m_fCurrentTime > m_fDuration)
		m_fCurrentTime = m_fDuration;

/*	if (m_pcsList)
	{
		if (m_fCurrentTime >= m_pcsList->m_csData.m_fTime)
		{
			BOOL bInit = (BOOL)(m_pcsList->m_csData.m_pfCallback((PVOID)(m_pcsList->m_csData.m_pavData)));

			if (m_pcsList->m_csData.m_bLoop)
				HeadMoveToTail(m_pcsList);
			else if (!bInit)
				RemoveAnimList(m_pcsList);
		}
	}*/

	AgcdAnimationCallbackList	*pcsCur		= m_pcsList;
	AgcdAnimationCallbackList	*pcsNext;
	while (pcsCur)
	{
		pcsNext	= pcsCur->m_pcsNext;

		if (m_fCurrentTime >= pcsCur->m_csData.m_fTime)
		{
			BOOL bInit = (BOOL)(pcsCur->m_csData.m_pfCallback((PVOID)(pcsCur->m_csData.m_pavData)));
			if (bInit)
				break;

			if (pcsCur->m_csData.m_bLoop)
				HeadMoveToTail(pcsCur);
			else
				RemoveAnimList(pcsCur);
		}
		else
		{
			break;
		}

		pcsCur = pcsNext;
	}

	return TRUE;
}

VOID AgcdAnimationTime::InitializeAnimation(FLOAT fDuration)
{
	ResetAnimCB();

	m_fCurrentTime	= 0.0f;
	m_fDuration		= fDuration;
}

INT32 AgcdAnimationTime::ParseAnimPoint(CHAR *szAnimPoint, INT32 *plDest)
{
	CHAR	szTemp[256];
	INT32	lProgress, lIndex, lLen, lNumField;

	lLen	= strlen(szAnimPoint);

	for (lProgress = 0, lIndex = 0, lNumField = 0; ; ++lProgress)
	{
		if ((szAnimPoint[lProgress] == ':') || ((lProgress != 0) && (lProgress == lLen)))
		{
			szTemp[lIndex]		= '\0';
			plDest[lNumField]	= atoi(szTemp);

			lIndex = 0;
			++lNumField;

			if (lProgress == lLen)
				break;
			else
				continue;
		}

		szTemp[lIndex] = szAnimPoint[lProgress];
		++lIndex;
	}

	return lNumField;
}

/******************************************************************************
******************************************************************************/










/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
AgcaAnimation::AgcaAnimation()
{
	m_lNumAttachedData			= 0;
	m_lAllocAnimData			= 0;

	memset(m_pavRemoveAnimationCBClass, 0, sizeof(PVOID) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_pafRemoveAnimationCB, 0, sizeof(ApModuleDefaultCallBack) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_aulAttachedDataSize, 0, sizeof(INT32) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
	memset(m_apszAttachedDataKey, 0, sizeof(CHAR *) * D_AGCA_ANIMATION_MAX_ATTACHED_DATA);
}

/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
AgcaAnimation::~AgcaAnimation()
{
	for (INT32 lIndex = 0; lIndex < D_AGCA_ANIMATION_MAX_ATTACHED_DATA; ++lIndex)
	{
		if (m_apszAttachedDataKey[lIndex])
		{
			delete [] m_apszAttachedDataKey[lIndex];
			m_apszAttachedDataKey[lIndex] = NULL;
		}
	}
}

/******************************************************************************
* Purpose :
*
* 041603. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::Initialize(INT32 lMaxAdminAnimation, INT32 lAllocAnimData)
{
	if (!InitializeObject(sizeof(AgcdAnimation), lMaxAdminAnimation))
		return FALSE;

	m_lAllocAnimData	= lAllocAnimData;

	return m_csAnimDataAdmin.InitializeObject(sizeof(AgcdAnimData), lMaxAdminAnimation * lAllocAnimData);
}

/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
/*AgcdAnimation *AgcaAnimation::AddAnimation(AgcdAnimation *pstAnim, CHAR *szFile, CHAR *szPath)
{
	CHAR			szFullPath[256];	
	strncpy(stAnim.m_szANMName, szFile, AGCD_ANIMATION_NAME_LENGTH);
	sprintf(szFullPath, "%s%s", szPath, szFile);
	stAnim.m_pAnim = RtAnimAnimationRead(szFullPath);

	if(!stAnim.m_pAnim)
		return NULL;

	pstAnim = (AgcdAnimation *) AddObject((PVOID) &stAnim, szFile);
	if(!pstAnim)
		return NULL;

//	pstAnim->m_lCustData					= -1;
	pstAnim->m_stAnimFlag.m_unAnimFlag		= AGCD_ANIMATION_FLAG_EMPTY;
	pstAnim->m_stAnimFlag.m_unPreference	= 0;

	return pstAnim;
}*/

/******************************************************************************
* Purpose :
*
* 041503. Bob Jung
*******************************************************************************
/*AgcdAnimation *AgcaAnimation::GetAnimation(CHAR *szFile, CHAR *szPath)
{
	AgcdAnimation *pstAnim = NULL;

	pstAnim = (AgcdAnimation *) GetObject(szFile);
	if((!pstAnim) && (szPath[0]))
	{
		pstAnim = AddAnimation(szFile, szPath);
	}

	return pstAnim;
}*/

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimation *AgcaAnimation::AddAnimation(CHAR *szName)
{
	AgcdAnimation	*pstAnimation;
	AgcdAnimation	stAnimation;

	// �ʱ�ȭ...
	memset(&stAnimation, 0, sizeof(AgcdAnimation));

	// Attached�� custom data�� �Ҵ��Ѵ�.
	if (!AllocateAttachedData(&stAnimation))
		return NULL;

	// AgcdAnimData�� ������ �迭�� �Ҵ��Ѵ�.
	stAnimation.m_ppastAnimData			= (AgcdAnimData **) new PVOID[m_lAllocAnimData];
	memset(stAnimation.m_ppastAnimData, 0, sizeof(AgcdAnimData *) * m_lAllocAnimData);

//	stAnimation.m_ppastBlendingAnimData	= (AgcdAnimData **)(malloc(sizeof(AgcdAnimData *) * m_lAllocAnimData));
//	memset(stAnimation.m_ppastBlendingAnimData, 0, sizeof(AgcdAnimData *) * m_lAllocAnimData);

	// Admin�� �߰��Ѵ�.
	pstAnimation = (AgcdAnimation *)(AddObject((PVOID)(&stAnimation), szName));

	return pstAnimation;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimation *AgcaAnimation::GetAnimation(CHAR *szName, BOOL bAdd)
{
	AgcdAnimation *pstAnimation = NULL;
	pstAnimation = (AgcdAnimation *)(GetObject(szName));

	if ((!pstAnimation) && (bAdd))
	{
		pstAnimation = AddAnimation(szName);
	}

	return pstAnimation;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimData *AgcaAnimation::AddAnimData(CHAR *szName, CHAR *szPath)
{
	AgcdAnimData	*pstAnimData;
	AgcdAnimData	stAnimData;
	CHAR			szFullPath[256];

//	strncpy(stAnimData.m_szANMName, szName, AGCD_ANIMATION_NAME_LENGTH);

	// �ִϸ��̼� �����͸� �д´�.
	sprintf(szFullPath, "%s%s", szPath, szName);
	stAnimData.m_pstAnim	= RtAnimAnimationRead(szFullPath);	
	if (!stAnimData.m_pstAnim)
		return NULL;

	// ���۷��� ī��Ʈ�� �ʱ�ȭ�Ѵ�.
	stAnimData.m_ulRefCount	= 1;

	// Admin�� �߰��Ѵ�.
	pstAnimData = (AgcdAnimData *)(m_csAnimDataAdmin.AddObject((PVOID)(&stAnimData), szName));

	return pstAnimData;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
AgcdAnimData *AgcaAnimation::GetAnimData(CHAR *szName, CHAR *szPath)
{
	AgcdAnimData *pstAnimData = NULL;

	// �̹� �ִ��� ã�´�.
	pstAnimData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObject(szName));

	// ã�� �����Ͱ� ����...
	if (!pstAnimData)
	{
		// ��η� �־����� ���� �߰��Ѵ�.
		if(szPath)
			pstAnimData = AddAnimData(szName, szPath);
		else // ã�� �����͵� ����, ��ε� ������ �����͸� ������ �� ����.
			return NULL;
	}
	else // ã�� �����Ͱ� ������ �ش� �������� ���۷��� ī��Ʈ�� �����Ѵ�.
	{
		++pstAnimData->m_ulRefCount;
	}

	return pstAnimData;
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
VOID AgcaAnimation::RemoveAnimData(AgcdAnimData *pstAnimData, CHAR *szKey)
{
	// �ִϸ��̼� �����Ͱ� �����Ǿ� ������ �����Ѵ�!
	if (pstAnimData->m_pstAnim)
	{
		RtAnimAnimationDestroy(pstAnimData->m_pstAnim);
	}

	// Admin������ �����Ѵ�.
	if (szKey)
		m_csAnimDataAdmin.RemoveObject(szKey);
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::RemoveAnimData(CHAR *szKey)
{
	// �����͸� ã�´�.
	AgcdAnimData *pstData;
	pstData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObject(szKey));
	if (!pstData)
		return FALSE;

	// ���۷��� ī��Ʈ�� �����Ѵ�.
	--pstData->m_ulRefCount;

	// ���̻� ����ϴ� �༮�� ���ٸ�?
	if (pstData->m_ulRefCount < 1)
		RemoveAnimData(pstData, szKey);

	return TRUE;

//	return RemoveAnimData(pstData);
}

/******************************************************************************
* Purpose :
*
* 090804. BOB
*******************************************************************************
VOID AgcaAnimation::RemoveAnimation(AgcdAnimation *pstAnim, CHAR *szKey)
{
	if (pstAnim->m_ppastAnimData)
	{
		delete [] pstAnim->m_ppastAnimData;
		pstAnim->m_ppastAnimData = NULL;
	}

//	if (pstAnim->m_ppastBlendingAnimData)
//	{
//		free(pstAnim->m_ppastBlendingAnimData);
//		pstAnim->m_ppastBlendingAnimData = NULL;
//	}

	if (pstAnim->m_pavAttachedData)
	{
		// ���� ������ŭ ������~
		for (INT32 lIndex = 0; lIndex < m_lNumAttachedData; ++lIndex)
		{
			if (m_pafRemoveAnimationCB[lIndex])
				m_pafRemoveAnimationCB[lIndex](pstAnim->m_pavAttachedData[lIndex], m_pavRemoveAnimationCBClass[lIndex], NULL);

			delete [] pstAnim->m_pavAttachedData[lIndex];
		}

		// �����͹迭�� ������~
		delete [] pstAnim->m_pavAttachedData;

		pstAnim->m_pavAttachedData	= NULL;
	}

	// Key�� ������ admin������ ������~
	if (szKey);
		RemoveObject(szKey);
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::RemoveAnimation(CHAR *szKey)
{
	AgcdAnimation	*pstAnim	= (AgcdAnimation *)(GetObject(szKey));
	if (!pstAnim)
		return FALSE;

	RemoveAnimation(pstAnim, szKey);

	return TRUE;

//	return RemoveObject(szKey);
}

/******************************************************************************
* Purpose :
*
* 070903. Bob Jung
*******************************************************************************
BOOL AgcaAnimation::RemoveAll()
{
	INT32 lIndex = 0;
	for (	AgcdAnimData *pstData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObjectSequence(&lIndex));
			pstData;
			pstData = (AgcdAnimData *)(m_csAnimDataAdmin.GetObjectSequence(&lIndex))				)
	{
		RemoveAnimData(pstData);
	}

	if (!m_csAnimDataAdmin.RemoveObjectAll())
		return FALSE;

	lIndex	= 0;
	for (	AgcdAnimation *pstAnim	= (AgcdAnimation *)(GetObjectSequence(&lIndex));
			pstAnim;
			pstAnim					= (AgcdAnimation *)(GetObjectSequence(&lIndex))		)
	{
		RemoveAnimation(pstAnim);
	}

	return RemoveObjectAll();
}

/******************************************************************************
* Purpose :
*
* 090804. BOB
*******************************************************************************
BOOL AgcaAnimation::AttachData(CHAR *szKey, UINT32 ulDataSize)
{
	// Key ���̸� �����Ѵ�.
	if (strlen(szKey) >= D_AGCA_ANIMATION_MAX_ATTACHED_DATA_KEY)
		return FALSE;

	// �̹� �Ҵ�Ǿ� ������ �ȵȴ�.
	if (m_apszAttachedDataKey[m_lNumAttachedData])
		return FALSE;

	// �Ҵ��Ѵ�.
	m_apszAttachedDataKey[m_lNumAttachedData]	= new CHAR[D_AGCA_ANIMATION_MAX_ATTACHED_DATA_KEY];
	if (!m_apszAttachedDataKey[m_lNumAttachedData])
		return FALSE;

	// Ű�� copy�Ѵ�.
	strcpy(m_apszAttachedDataKey[m_lNumAttachedData], szKey);
	// Size�� �����Ѵ�.
	m_aulAttachedDataSize[m_lNumAttachedData]	= ulDataSize;

	// Attched�� ������ �����Ѵ�.
	++m_lNumAttachedData;

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 090804. BOB
*******************************************************************************
BOOL AgcaAnimation::AllocateAttachedData(AgcdAnimation *pstAnimation)
{
	// Attached�� �����Ͱ� ������ skip~
	if (!m_lNumAttachedData)
		return TRUE; // skip

	// �̹� �Ҵ�Ǿ� ������ ����! -_-;;
	if (pstAnimation->m_pavAttachedData)
		return FALSE;

	// ������ �迭�� �̸� �Ҵ�~
	pstAnimation->m_pavAttachedData = new PVOID[m_lNumAttachedData];
	if (!pstAnimation->m_pavAttachedData)
		return FALSE;

	// �Ҵ�� ������ �迭�� ���� ������ ũ�⸸ŭ �ٽ� �Ҵ��Ѵ�.
	for (INT32 lIndex = 0; lIndex < m_lNumAttachedData; ++lIndex)
	{
		pstAnimation->m_pavAttachedData[lIndex]		= new BYTE[m_aulAttachedDataSize[lIndex]];
		if (!pstAnimation->m_pavAttachedData[lIndex])
			return FALSE;

		memset(pstAnimation->m_pavAttachedData[lIndex], 0, m_aulAttachedDataSize[lIndex]);
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
* Desc : Ű������ ã�� ���ϴ� ��� -1�� �����Ѵ�.(failed)
* 180804. BOB
*******************************************************************************
INT32 AgcaAnimation::GetAttachedDataIndex(CHAR *szKey)
{
	for (INT32 lIndex = 0; lIndex < m_lNumAttachedData; ++lIndex)
	{
		if (!strcmp(m_apszAttachedDataKey[lIndex], szKey))
			break;
	}

	return (lIndex == m_lNumAttachedData) ? (-1) : (lIndex);
}

/******************************************************************************
* Purpose :
*
* 100804. BOB
*******************************************************************************
PVOID AgcaAnimation::GetAttachedData(AgcdAnimation *pstAnim, CHAR *szKey)
{
	if (!pstAnim) 
		return NULL;

	INT32 lIndex = GetAttachedDataIndex(szKey);
	if (lIndex == -1)
		return NULL;

	return pstAnim->m_pavAttachedData[lIndex];
}

/******************************************************************************
* Purpose :
*
* 110804. BOB
*******************************************************************************
BOOL AgcaAnimation::SetCallbackRemoveAnimation(CHAR *szKey, ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	INT32 lIndex = GetAttachedDataIndex(szKey);
	if (lIndex == -1)
		return FALSE;

	m_pavRemoveAnimationCBClass[lIndex]	= pClass;
	m_pafRemoveAnimationCB[lIndex]		= pfCallback;

	return TRUE;
}

/******************************************************************************
******************************************************************************/