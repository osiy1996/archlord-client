#include "AgcmIntervalAnimAdmin.h"

/*IntervalAnimList::IntervalAnimList()
{
	m_pcsNext			= NULL;
	m_pcsParentModule	= NULL;
}

IntervalAnimList::~IntervalAnimList()
{
}*/

/*AgcmIntervalAnimAdmin::AgcmIntervalAnimAdmin()
{
	m_ppstHierarchy				= NULL;
//	m_pfIntervalAnimationCB		= NULL;
	m_pcsList					= NULL;
	m_lCurListNum				= 0;
	m_lMaxListNum				= 0;
}

AgcmIntervalAnimAdmin::~AgcmIntervalAnimAdmin()
{
	ResetIntervalData();
}

RtAnimInterpolator *AgcmIntervalAnimAdmin::IntervalAnimationCB(RtAnimInterpolator *animInstance, PVOID pvData)
{
	AgcdAnimationCallbackList	*pcsList	= (AgcdAnimationCallbackList *)(pvData);
	if (pcsList)
	{
//		if (pcsList->m_pcsParentModule)
		{
			pcsList->m_csData.m_pfCallback((PVOID)(&pcsList->m_csData));
//			pcsList->m_pcsParentModule->m_pfIntervalAnimationCB((PVOID)(&pcsList->m_csData));

			// �����̸�, ��带 ���������� �ű��.
			if (pcsList->m_csData.m_bLoop)
				pcsList = (AgcdAnimationCallbackList *)(pcsList->m_pfLoopFunc((PVOID)(pcsList)));
//				pcsList = pcsList->m_pcsParentModule->HeadMoveToTail(pcsList);
			else // ������ �ƴϸ�, �����Ѵ�.
				pcsList = (AgcdAnimationCallbackList *)(pcsList->m_pfEndFunc((PVOID)(pcsList)));
				//pcsList = pcsList->m_pcsParentModule->RemoveAnimList(pcsList);

			// ���� �ݹ��� �����Ѵ�.
			if ((pcsList) && (pcsList->m_pfCustFunc))
				pcsList->m_pfCustFunc(pcsList->m_pvClass);
			else
				RtAnimInterpolatorSetAnimCallBack(animInstance, NULL, NULL, NULL);
		}
	}

	return animInstance;
}

BOOL AgcmIntervalAnimAdmin::AddIntervalData(AgcdAnimationCallbackData *pcsData)
{
	// �� á��!!!
	if ((m_lMaxListNum) && (m_lMaxListNum <= m_lCurListNum))
		return FALSE;

	if ((!m_ppstHierarchy) || (!*m_ppstHierarchy))
		return FALSE;

	AgcdAnimationCallbackList	*pcsCurList, *pcsPreList;

	// ���� �ִϸ��̼��� ����ð����� ũ�ų� ������...
	if ((*m_ppstHierarchy)->currentAnim->currentTime <= pcsData->m_fTime)
	{
		// ����Ʈ�� ������...
		if (m_pcsList)
		{
			// ���� ����Ʈ�� ����ִ� �ð����� �۴ٸ�...
			if (m_pcsList->m_csData.m_fTime > pcsData->m_fTime)
			{
				// ��� pcsCurList�� �����Ѵ�.
				pcsCurList = m_pcsList;

				// ���� ����Ʈ�� ���� �༮�� �߰��Ѵ�.
				if (!AddAnimList(NULL, &m_pcsList, pcsCurList, pcsData))
					return FALSE;

				// �ݹ��� �ٽ� �����Ѵ�.
				SetIntervalAnimationCallback();

				return TRUE;
			}
			else if (m_pcsList->m_csData.m_fTime == pcsData->m_fTime) // ���� ����Ʈ�� ����ִ� �ð��� ���ٸ�...
			{
				// replace!!!
				memcpy(&m_pcsList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
			}

			pcsPreList	= m_pcsList;
			pcsCurList	= m_pcsList->m_pcsNext;
			// ����Ʈ�� ����...
			while (pcsCurList)
			{
				// ����Ʈ�� �ð����� �۴ٸ�...
				if (pcsCurList->m_csData.m_fTime > pcsData->m_fTime)
				{
					// �� ����Ʈ�� ���� ����Ʈ ���̿� �߰��Ѵ�...
					return AddAnimList(pcsPreList, &pcsPreList->m_pcsNext, pcsCurList, pcsData);
				}
				else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // ����Ʈ�� �ð��� ���ٸ�...
				{
					// replace!!!
					memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

					return TRUE;
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

			// �ݹ��� ����Ѵ�.
			return SetIntervalAnimationCallback();
		}
	}
	else // ���� �ִϸ��̼��� ����ð����� ������...
	{
		// ����Ʈ�� ������...
		if (!m_pcsList)
		{
			if(!AddAnimList(NULL, &m_pcsList, NULL, pcsData))
				return FALSE;

			// �ݹ��� ����Ѵ�.
			return SetIntervalAnimationCallback();
		}

		pcsPreList	= m_pcsList;
		pcsCurList	= m_pcsList->m_pcsNext;
		// ����Ʈ�� ����...
		while (pcsCurList)
		{
			// ����Ʈ�� �ð����� ũ�ٸ�...
			if (pcsCurList->m_csData.m_fTime < pcsData->m_fTime)
			{
				// ���� ����Ʈ�� ���� ����Ʈ ���̿� �߰��Ѵ�...
				return AddAnimList(pcsCurList, &pcsCurList->m_pcsNext, pcsCurList->m_pcsNext, pcsData);
			}
			else if (pcsCurList->m_csData.m_fTime == pcsData->m_fTime) // ����Ʈ�� �ð��� ���ٸ�...
			{
				// replace!!!
				memcpy(&pcsCurList->m_csData, pcsData, sizeof(AgcdAnimationCallbackData));

				return TRUE;
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

VOID AgcmIntervalAnimAdmin::SetIntervalAnimHierarchy(RpHAnimHierarchy **ppstHierarchy)
{
	m_ppstHierarchy = ppstHierarchy;
}

VOID AgcmIntervalAnimAdmin::SetMaxIntervalData(INT32 lMax)
{
	m_lMaxListNum = lMax;
}

VOID AgcmIntervalAnimAdmin::ResetIntervalData()
{
	// ����Ʈ�� �ִ� �༮���� �� ������,
	// RtAnimInterpolatorSetAnimCallBack()�� �ʱ�ȭ�Ѵ�!
	AgcdAnimationCallbackList	*pcsNext = m_pcsList;
	AgcdAnimationCallbackList	*pcsTemp;
	while (pcsNext)
	{
		pcsTemp = pcsNext->m_pcsNext;

		delete pcsNext;

		pcsNext = pcsTemp;
	}

	m_pcsList		= NULL;
	m_lCurListNum	= 0;

	if ((m_ppstHierarchy) && (*m_ppstHierarchy))
		RtAnimInterpolatorSetAnimCallBack((*m_ppstHierarchy)->currentAnim, NULL, 0.0f, NULL);
}

BOOL AgcmIntervalAnimAdmin::AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData)
{
	if(ppstCurList)
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
//		(*(ppstCurList))->m_pcsParentModule	= this;

		// �ʿ��� callback�� �����Ѵ�.
		(*(ppstCurList))->m_pfLoopFunc	= SetLoopAnimCB;
		(*(ppstCurList))->m_pfEndFunc	= SetEndAnimCB;
		(*(ppstCurList))->m_pfCustFunc	= SetSetNextAnimCB;

		// �� ����Ʈ�� ������ �����Ѵ�.
		if (pstPreList)
		{
			pstPreList->m_pcsNext = *(ppstCurList);
		}

		++m_lCurListNum;
	}

	return TRUE;
}

AgcdAnimationCallbackList *AgcmIntervalAnimAdmin::RemoveAnimList(AgcdAnimationCallbackList *pstCurList)
{
	if (!pstCurList)
		return NULL;
	// ������ �����ϰ�...
	m_pcsList = pstCurList->m_pcsNext;
	// ����������~ �d�d~ -_-;;
	delete pstCurList;

	--m_lCurListNum;

	return m_pcsList;
}

BOOL AgcmIntervalAnimAdmin::SetIntervalAnimationCallback()
{
	if ((!m_ppstHierarchy) || (!*m_ppstHierarchy))
		return FALSE;

	if (!m_pcsList)
		RtAnimInterpolatorSetAnimCallBack((*m_ppstHierarchy)->currentAnim, NULL, NULL, NULL);
	else
		RtAnimInterpolatorSetAnimCallBack((*m_ppstHierarchy)->currentAnim, IntervalAnimationCB, m_pcsList->m_csData.m_fTime, (PVOID)(m_pcsList));

	return TRUE;
}

AgcdAnimationCallbackList *AgcmIntervalAnimAdmin::HeadMoveToTail(AgcdAnimationCallbackList *pstHead)
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

PVOID AgcmIntervalAnimAdmin::SetLoopAnimCB(PVOID pvData)
{
	if (!pvData)
		return NULL;

	AgcdAnimationCallbackList	*pcsHead	= (AgcdAnimationCallbackList *)(pvData);

	return (PVOID)(
		((AgcmIntervalAnimAdmin *)(pcsHead->m_pvClass))->HeadMoveToTail(pcsHead)
		);
}

PVOID AgcmIntervalAnimAdmin::SetEndAnimCB(PVOID pvData)
{
	if (!pvData)
		return NULL;

	AgcdAnimationCallbackList	*pcsCurrent	= (AgcdAnimationCallbackList *)(pvData);

	return (PVOID)(
		((AgcmIntervalAnimAdmin *)(pcsCurrent->m_pvClass))->RemoveAnimList(pcsCurrent)
		);
}

PVOID AgcmIntervalAnimAdmin::SetSetNextAnimCB(PVOID pvData)
{
	if (!pvData)
		return NULL;

	AgcmIntervalAnimAdmin		*pcsThis	= (AgcmIntervalAnimAdmin *)(pvData);

	pcsThis->SetIntervalAnimationCallback();

	return NULL;
}*/