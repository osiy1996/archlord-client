#include "AgcmAnimation.h"
#include <ApMemoryTracker/ApMemoryTracker.h>

/*
AgcmAnimation::AgcmAnimation()
{
	m_pcsList					= NULL;
	m_lNumCurCB					= 0;
	m_lNumMaxCB					= 0;
	m_fCurrentTime				= 0;
	m_fDuration					= 0;
}

AgcmAnimation::~AgcmAnimation()
{
	ResetAnimCB(FALSE);
}

VOID AgcmAnimation::SetMaxCallback(INT32 lMax)
{
	m_lNumMaxCB	= lMax;
}

BOOL AgcmAnimation::AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData)
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

BOOL AgcmAnimation::AddAnimCB(AgcdAnimationCallbackData *pcsData)
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

			// ���� ����Ʈ�� �����Ѵ�...
			pcsPreList	= pcsCurList;
			pcsCurList	= pcsCurList->m_pcsNext;
		}

		// �����Ϸ��� �ð����� ����Ʈ�� �ִ� �ð����� ��� ���� ������, �� �������� �ִ´�.
		return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, NULL, pcsData);
	}

	return FALSE;
}

VOID AgcmAnimation::ResetAnimCB(BOOL InfluenceNextAnimation)
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

AgcdAnimationCallbackList *AgcmAnimation::HeadMoveToTail(AgcdAnimationCallbackList *pstHead)
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

AgcdAnimationCallbackList *AgcmAnimation::RemoveAnimList(AgcdAnimationCallbackList *pstCurList)
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

BOOL AgcmAnimation::AddTime(FLOAT fTime)
{
	m_fCurrentTime	+= fTime;
	if (m_fCurrentTime > m_fDuration)
		m_fCurrentTime = m_fDuration;

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

VOID AgcmAnimation::InitializeAnimation(FLOAT fDuration)
{
	ResetAnimCB();

	m_fCurrentTime	= 0.0f;
	m_fDuration		= fDuration;
}

INT32 AgcmAnimation::ParseAnimPoint(CHAR *szAnimPoint, INT32 *plDest)
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
}*/