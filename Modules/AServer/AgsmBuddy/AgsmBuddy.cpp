#include "AgsmBuddy.h"

#include "AgpmBuddy.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgpmCharacter.h"
#include "AgsdBuddy.h"
#include "ApmMap.h"
#include "AgsmPrivateTrade.h"
#include "AgsmParty.h"

#include <algorithm>

AgsmBuddy::AgsmBuddy()
{
	m_pcsAgpmBuddy		= NULL;
	m_pcsAgsmCharacter	= NULL;

	SetModuleName("AgsmBuddy");
	EnableIdle2(FALSE);
}

AgsmBuddy::~AgsmBuddy()
{
}

BOOL AgsmBuddy::OnAddModule()
{
	m_pcsAgpmBuddy = (AgpmBuddy*)GetModule("AgpmBuddy");
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pcsApmMap = (ApmMap*)GetModule("ApmMap");
	m_pcsAgsmPrivateTrade = (AgsmPrivateTrade*)GetModule("AgsmPrivateTrade");
	m_pcsAgsmParty = (AgsmParty*)GetModule("AgsmParty");

	if (NULL == m_pcsAgpmBuddy || NULL == m_pcsAgsmCharacter || NULL == m_pcsAgsmCharManager ||
		NULL == m_pcsAgpmCharacter || NULL == m_pcsApmMap || NULL == m_pcsAgsmParty || NULL == m_pcsAgsmPrivateTrade)
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackAdd(CBAddBuddy, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackAddRequest(CBAddRequest, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackAddReject(CBAddReject, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackRemove(CBRemoveBuddy, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackOptions(CBOptionsBuddy, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackRegion(CBRegion, this))
		return FALSE;

	// Ŭ���̾�Ʈ�� �α伭������ ���Ӽ����� ó�� �����Ҷ� ������ ��Ŷ
	// �̶� �����̼����� ���𸮽�Ʈ�� ��û�Ѵ�.
	if (FALSE == m_pcsAgsmCharManager->SetCallbackConnectedChar(CBCharConnected, this))
		return FALSE;

	// Ŭ���̾�Ʈ�� ���� �ε��Ϸ��ϰ� ���� �÷��� �����ϴٰ� ���Ӽ����� �˸��� ��Ŷ
	if (FALSE == m_pcsAgsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;

	if (FALSE == m_pcsAgpmCharacter->SetCallbackRemoveChar(CBCharDisconnected, this))
		return FALSE;

	if (FALSE == m_pcsAgsmPrivateTrade->SetCallbackBuddyCheck(CBPrivateTradeCheck, this))
		return FALSE;

	if (FALSE == m_pcsAgsmParty->SetCallbackBuddyCheck(CBPartyCheck, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorRequest(CBMentorRequest, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorRequestAccept(CBMentorRequestAccept, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorRequestReject(CBMentorRequestReject, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorDelete(CBMentorDelete, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmBuddy::OnInit()
{
	return TRUE;
}

BOOL AgsmBuddy::OnDestroy()
{
	return TRUE;
}

BOOL AgsmBuddy::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmBuddy::CBCharConnected(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	// pCustData�� ulNID
	
	pThis->EnumCallback(AGSMBUDDY_DB_SELECT, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgsmBuddy::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if (FALSE == pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	pThis->SendMyBuddyList(pcsCharacter);
	pThis->SendOnlineAllBuddy(pcsCharacter, TRUE);
	pThis->SendInitEnd(pcsCharacter);

	return TRUE;
}

BOOL AgsmBuddy::CBCharDisconnected(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if (FALSE == pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBCharDisconnected"));

	pThis->SendOnlineAllBuddy(pcsCharacter, FALSE);

	return TRUE;
}

BOOL AgsmBuddy::CBPartyCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pData || NULL == pClass || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdCharacter* pcsTargetChar = (AgpdCharacter*)pCustData;

	return pThis->PartyCheck(pcsCharacter, pcsTargetChar);
}

BOOL AgsmBuddy::CBPrivateTradeCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pData || NULL == pClass || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdCharacter* pcsTargetChar = (AgpdCharacter*)pCustData;

	return pThis->PrivateTradeCheck(pcsCharacter, pcsTargetChar);
}

BOOL AgsmBuddy::CBAddBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationAddBuddy(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBAddRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationAddRequest(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBAddReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationAddReject(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBRemoveBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationRemoveBuddy(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBOptionsBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationOptionsBuddy(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBRegion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy *pThis = (AgsmBuddy*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyRegion *pBuddyRegion = (AgpdBuddyRegion*)pCustData;
	return pThis->OperationRegion(pcsCharacter, pBuddyRegion);
}

BOOL AgsmBuddy::CBMentorRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorRequest(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBMentorDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorDelete(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBMentorRequestAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorRequestAccept(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBMentorRequestReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorRequestReject(pcsCharacter, pcsBuddyElement);
}


// pcsCharacter �� ��û�� ���� ����̴�.
// Yes �� ������ �Ʒ� ���۷��̼��� �´�.
BOOL AgsmBuddy::OperationAddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	// ��û���� ����� ������ �̹� ģ����Ͽ� ���� �� �ִ�.
	// �׿� ���ؼ� �ٸ��� ó�� �ؾ� �Ѵ�.
	EnumAgpmBuddyMsgCode eMsgCode = IsValidBuddy(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, eMsgCode);

	if(pcsBuddyElement->IsFriend())
	{
		// ��û�� �� ó��
		AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
		if(!pcsTarget || pcsTarget->m_bIsReadyRemove)
			return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST);

		AuAutoLock csLock(pcsTarget->m_Mutex);

		AgpdBuddyElement BuddyElement;
		BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

		EnumAgpmBuddyMsgCode eMsgCode2 = IsValidBuddy(pcsTarget, &BuddyElement);
		
		if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode2)
		{
			SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);	// ��û�� ���� ĳ���Ϳ��Դ� �׳� �Ұ��� ���� ������
			SendMsgCodeBuddy(pcsTarget, &BuddyElement, eMsgCode2);								// ��û�� �ߴ� ĳ���Ϳ��Դ� ����� ������.
			return TRUE;
		}

		AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsTarget);
		if(!pcsBuddyADChar)
			return FALSE;

		if(pcsBuddyADChar->m_szLastRequestBuddy.CompareNoCase(pcsCharacter->m_szID) != COMPARE_EQUAL)
		{
			SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);	// �׳� �߸��� ���۷��̼����� ������.
			SendMsgCodeBuddy(pcsTarget, &BuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);		// �׳� �߸��� ���۷��̼����� ������.
			return TRUE;
		}

		// ��û�� �� ó��
		if (m_pcsAgpmBuddy->AddBuddy(pcsTarget, &BuddyElement))
		{
			// DB insert
			EnumCallback(AGSMBUDDY_DB_INSERT, pcsTarget, &BuddyElement);

			// on-line ���� Ȯ���� IsValidBuddy���� �����Ƿ� ������ �¶������� ����
			BuddyElement.SetOnline(TRUE);

			// Client���� �뺸
			SendAddBuddy(pcsTarget, &BuddyElement);
		}

		// ��û���� �� ó��.
		if (AGPMBUDDY_MSG_CODE_ALREADY_EXIST == eMsgCode)
		{
			// �̹� ��ϵǾ� �ִ� ���. �¶������θ� ���ָ� �ȴ�.
			pcsBuddyElement->SetOnline(TRUE);
			SendBuddyOnlineToMe(pcsCharacter, pcsBuddyElement);
		}
		else if(m_pcsAgpmBuddy->AddBuddy(pcsCharacter, pcsBuddyElement))
		{
			// ���� �߰����ش�.

			// DB insert
			EnumCallback(AGSMBUDDY_DB_INSERT, pcsCharacter, pcsBuddyElement);

			// on-line ���� Ȯ���� IsValidBuddy���� �����Ƿ� ������ �¶������� ����
			pcsBuddyElement->SetOnline(TRUE);

			// Client���� �뺸
			SendAddBuddy(pcsCharacter, pcsBuddyElement);
		}
	}
	else
	{
		// �ź� ����Ʈ ���
		//
		//

		// ģ�� �߰�
		if (FALSE == m_pcsAgpmBuddy->AddBuddy(pcsCharacter, pcsBuddyElement))
			return FALSE;

		// DB insert
		EnumCallback(AGSMBUDDY_DB_INSERT, pcsCharacter, pcsBuddyElement);

		// on-line ���� Ȯ���� IsValidBuddy���� �����Ƿ� ������ �¶������� ����
		pcsBuddyElement->SetOnline(TRUE);

		// Client���� �뺸
		SendAddBuddy(pcsCharacter, pcsBuddyElement);
	}

	return TRUE;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::OperationAddRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	EnumAgpmBuddyMsgCode eMsgCode = IsValidBuddy(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, eMsgCode);

	// ���濡�� ����.
	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget || pcsTarget->m_bIsReadyRemove)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElement;
	BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

	// ���浵 üũ
	EnumAgpmBuddyMsgCode eMsgCode2 = IsValidBuddy(pcsTarget, &BuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode2)
	{
		SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);
		return TRUE;
	}

	// ��û�� ����� �����Ѵ�.
	AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	pcsBuddyADChar->m_szLastRequestBuddy.SetText(pcsBuddyElement->m_szName);

	SendAddRequest(pcsTarget, &BuddyElement);

	return TRUE;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::OperationAddReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	// ��û�� ��뿡�� ����
	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget || pcsTarget->m_bIsReadyRemove)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElement;
	BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

	SendAddReject(pcsTarget, &BuddyElement);

	return TRUE;
}

BOOL AgsmBuddy::OperationRemoveBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(m_pcsAgpmBuddy->GetMentorStatus(pcsCharacter, pcsBuddyElement->m_szName) != AGSMBUDDY_MENTOR_NONE)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_NOT_DELETE_BECAUSE_MENTOR);

	// ģ�� �߰�
	if (FALSE == m_pcsAgpmBuddy->RemoveBuddy(pcsCharacter, pcsBuddyElement))
		return FALSE;

	// DB delete
	EnumCallback(AGSMBUDDY_DB_REMOVE, pcsCharacter, pcsBuddyElement);

	// client���� �뺸
	SendRemoveBuddy(pcsCharacter, pcsBuddyElement);

	if(pcsBuddyElement->IsFriend())
	{
		// ���浵 �����.
		AgpdBuddyElement BuddyElement;
		BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

		AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
		if(pcsTarget)
		{
			AuAutoLock csLock(pcsTarget->m_Mutex);

			if(m_pcsAgpmBuddy->RemoveBuddy(pcsTarget, &BuddyElement))
			{
				EnumCallback(AGSMBUDDY_DB_REMOVE, pcsTarget, &BuddyElement);

				SendRemoveBuddy(pcsTarget, &BuddyElement);
			}
		}
		else
		{
			// ���������� ��쿡�� DB ���� ���� ���ش�.
			EnumCallback(AGSMBUDDY_DB_REMOVE2, pcsBuddyElement->m_szName, &BuddyElement);
		}
	}

	return TRUE;
}

BOOL AgsmBuddy::OperationOptionsBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	// ģ�� �߰�
	if (FALSE == m_pcsAgpmBuddy->OptionsBuddy(pcsCharacter, pcsBuddyElement))
		return FALSE;

	// DB Update
	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsCharacter, pcsBuddyElement);

	// client���� �뺸
	SendOptionsBuddy(pcsCharacter, pcsBuddyElement);
	
	return TRUE;
}

BOOL AgsmBuddy::OperationRegion(AgpdCharacter* pcsCharacter, AgpdBuddyRegion* pcsBuddyRegion)
{
	AgpdCharacter *pcsTargetChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyRegion->m_BuddyElement.m_szName.GetBuffer());
	if (NULL == pcsTargetChar)
		return FALSE;

	INT16 nRegionIndex = m_pcsApmMap->GetRegion(pcsTargetChar->m_stPos.x, pcsTargetChar->m_stPos.z);
	if (-1 == nRegionIndex)
		return FALSE;

	return SendRegion(pcsCharacter, pcsTargetChar, (INT32)nRegionIndex);    
}

BOOL AgsmBuddy::OperationMentorRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	EnumAgpmBuddyMsgCode eMsgCode = IsValidMentorRequest(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_NOT_INVITE_BUDDY);

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElement;
	BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElement.SetMentorStatus(AGSMBUDDY_MENTOR_MENTOR);

	SendMentorRequest(pcsTarget, &BuddyElement);

	return TRUE;
}

BOOL AgsmBuddy::OperationMentorRequestAccept(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement) //pcsCharacter�� pcsTarget�� Mentee�� �ȴ�.
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	EnumAgpmBuddyMsgCode eMsgCode = IsValidMentorAccept(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_NOT_INVITE_BUDDY);

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElementMentor;
	BuddyElementMentor.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElementMentor.SetMentorStatus(AGSMBUDDY_MENTOR_MENTEE);

	AgpdBuddyElement BuddyElementMentee;
	BuddyElementMentee.SetValues(pcsBuddyElement->GetOptions(), pcsBuddyElement->m_szName);
	BuddyElementMentee.SetMentorStatus(AGSMBUDDY_MENTOR_MENTOR);

	if(!(m_pcsAgpmBuddy->MentorBuddy(pcsCharacter, &BuddyElementMentee) && m_pcsAgpmBuddy->MentorBuddy(pcsTarget, &BuddyElementMentor)))
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_PROCESS);

	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsCharacter, &BuddyElementMentee);
	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsTarget, &BuddyElementMentor);

	//���������� ó���Ǿ��ٰ� Ŭ�󿡰� �˸���.
	SendMentorRequestAccept(pcsCharacter, &BuddyElementMentee);
	SendMentorRequestAccept(pcsTarget, &BuddyElementMentor);

	return TRUE;
}

BOOL AgsmBuddy::OperationMentorRequestReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget)
		return FALSE;
	
	AgpdBuddyElement BuddyElementMentor;
	BuddyElementMentor.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElementMentor.SetMentorStatus(AGSMBUDDY_MENTOR_MENTEE);

	SendMentorRequestReject(pcsTarget, &BuddyElementMentor);

	return TRUE;
}

BOOL AgsmBuddy::OperationMentorDelete(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	EnumAgpmBuddyMsgCode eMsgCode = IsValidMentorDelete(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, eMsgCode);

	AgpdBuddyElement BuddyElementMentee;
	BuddyElementMentee.SetValues(pcsBuddyElement->GetOptions(), pcsBuddyElement->m_szName);
	BuddyElementMentee.SetMentorStatus(AGSMBUDDY_MENTOR_NONE);

	AgpdBuddyElement BuddyElementMentor;
	BuddyElementMentor.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElementMentor.SetMentorStatus(AGSMBUDDY_MENTOR_NONE);

	if(!m_pcsAgpmBuddy->MentorBuddy(pcsCharacter, &BuddyElementMentee))
		return SendMsgCodeBuddy(pcsCharacter, &BuddyElementMentee, AGPMBUDDY_MSG_CODE_INVALID_PROCESS);

	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsCharacter, &BuddyElementMentee);

	//���������� ó���Ǿ��ٰ� Ŭ�󿡰� �˸���.
	SendMentorDelete(pcsCharacter, &BuddyElementMentee);

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(pcsTarget)
	{
		AuAutoLock csLock(pcsTarget->m_Mutex);

		if(!m_pcsAgpmBuddy->MentorBuddy(pcsTarget, &BuddyElementMentor))
			return SendMsgCodeBuddy(pcsTarget, &BuddyElementMentor, AGPMBUDDY_MSG_CODE_INVALID_PROCESS);

		EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsTarget, &BuddyElementMentor);

		SendMentorDelete(pcsTarget, &BuddyElementMentor);
	}
	else
		EnumCallback(AGSMBUDDY_DB_OPTIONS2, pcsBuddyElement->m_szName, &BuddyElementMentor);

	return TRUE;
}

BOOL AgsmBuddy::SetCallbackDBInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_INSERT, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_REMOVE, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBRemove2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_REMOVE2, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBOptions(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_OPTIONS, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBOptions2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_OPTIONS2, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_SELECT, pfCallback, pClass);
}

BOOL AgsmBuddy::SendAddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketAddBuddy(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::SendAddRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketAddRequest(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::SendAddReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketAddReject(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendRemoveBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketRemoveBuddy(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendOptionsBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketUpdateOptions(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMsgCodeBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement, EnumAgpmBuddyMsgCode eMsgCode)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);
	ASSERT(AGPMBUDDY_MSG_CODE_NONE != eMsgCode);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMsgCode(&nPacketLength, pcsCharacter, pcsBuddyElement, eMsgCode);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendOnlineAllUser(AgpdCharacter* pcsCharacter, BOOL bOnline)
{
	ASSERT(NULL != pcsCharacter);

	if (!m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	if (m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	AgpdBuddyElement BuddyElement;
	BuddyElement.m_szName.SetText(pcsCharacter->m_szID);
	BuddyElement.SetOnline(bOnline);

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketOnline(&nPacketLength, pcsCharacter, &BuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacketAllUser(pvPacket, nPacketLength);
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmBuddy::SendOnlineAllBuddy(AgpdCharacter* pcsCharacter, BOOL bOnline)
{
	ASSERT(NULL != pcsCharacter);

	if (!m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	if (m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	AgpdBuddyElement BuddyElement;
	BuddyElement.m_szName.SetText(pcsCharacter->m_szID);
	BuddyElement.SetOnline(bOnline);

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketOnline(&nPacketLength, pcsCharacter, &BuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtFriend.begin(); iter != pcsBuddyADChar->m_vtFriend.end(); ++iter)
	{
		AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(iter->m_szName);

		if(pcsTarget)
			SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsTarget->m_lID));
	}

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtBan.begin(); iter != pcsBuddyADChar->m_vtBan.end(); ++iter)
	{
		AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(iter->m_szName);

		if(pcsTarget)
			SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsTarget->m_lID));
	}

	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::SendBuddyOnlineToMe(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketOnline(&nPacketLength, pcsCharacter, pcsBuddyElement);
	if(!pvPacket || nPacketLength < 1)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendRegion(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTargetChar, INT32 lRegionIndex)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsTargetChar);

	INT16 nPacketLength = 0;
	AgpdBuddyElement BuddyElement;
	BuddyElement.m_szName.SetText(pcsTargetChar->m_szID);

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketRegion(&nPacketLength, pcsCharacter, &BuddyElement, lRegionIndex);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendInitEnd(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	INT16 nPacketLength = 0;

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketInitEnd(&nPacketLength, pcsCharacter);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorRequest(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorRequest(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorDelete(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorDelete(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorRequestAccept(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorRequestAccept(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorRequestReject(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorRequestReject(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorUIOpen(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorUIOpen(&nPacketLength, pcsCharacter);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMyBuddyList(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtFriend.begin(); iter != pcsBuddyADChar->m_vtFriend.end(); ++iter)
	{
		// online ���� Ȯ��
		if (NULL == m_pcsAgpmCharacter->GetCharacter(iter->m_szName))
			iter->SetOnline(FALSE);
		else 
			iter->SetOnline(TRUE);

		SendAddBuddy(pcsCharacter, iter);
	}

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtBan.begin(); iter != pcsBuddyADChar->m_vtBan.end(); ++iter)
	{
		// online ���� Ȯ��
		if (NULL == m_pcsAgpmCharacter->GetCharacter(iter->m_szName))
			iter->SetOnline(FALSE);
		else 
			iter->SetOnline(TRUE);

		SendAddBuddy(pcsCharacter, iter);
	}

	return TRUE;
}

// Relay Server�� ���� ���� ���� ����Ʈ�� �ʱ�ȭ�Ѵ�.
BOOL AgsmBuddy::ReceiveBuddyDataFromRelay(AgpdCharacter* pcsCharacter, stBuddyRowset *pRowset)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pRowset);

	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);
	
	if (NULL == pcsBuddyADChar)
		return FALSE;

	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		AgpdBuddyElement BuddyElement;
		UINT32 ulCol = 0;
		CHAR *psz = NULL;
		
		if (NULL == (psz = pRowset->Get(ul, ulCol++)))		// char id
			continue;

		if (NULL == (psz = pRowset->Get(ul, ulCol++)))		// buddy id
			continue;

		BuddyElement.m_szName.SetText(psz);

		if (NULL == (psz = pRowset->Get(ul, ulCol++)))		// Options
			continue;

		BuddyElement.SetOptions(atoi(psz));

		if(NULL == (psz = pRowset->Get(ul, ulCol++)))		// MentorStatus
			continue;

		BuddyElement.SetMentorStatus(atoi(psz));

		if (BuddyElement.IsFriend())
			pcsBuddyADChar->m_vtFriend.push_back(BuddyElement);
		else
			pcsBuddyADChar->m_vtBan.push_back(BuddyElement);

		m_pcsAgpmBuddy->MentorBuddy(pcsCharacter, &BuddyElement);
	}

	return TRUE;
}

EnumAgpmBuddyMsgCode AgsmBuddy::IsValidBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	// �ڱ� �ڽ��� ����� �� ����.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// �̹� ��ϵ� ĳ���� �ΰ� Ȯ��
	if (TRUE == m_pcsAgpmBuddy->IsExistBuddy(pcsCharacter, pcsBuddyElement->m_szName.GetBuffer()))
		return AGPMBUDDY_MSG_CODE_ALREADY_EXIST;

	// ���� �¶��� ���� ĳ���� �ΰ� Ȯ��
	AgpdCharacter *pcsBuddyChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
    if (NULL == pcsBuddyChar)
		return AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST;

	// PC���� Ȯ��
	if (FALSE == m_pcsAgpmCharacter->IsPC(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// �Ʒ� Ȯ���� ģ�� �ʴ� �����϶��� �Ѵ�. 2007.07.25. steeple
	if(pcsBuddyElement->IsFriend())
	{
		// ���� �������� Ȯ��. 2007.07.12. steeple
		if (pcsBuddyChar->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_BUDDY)
			return AGPMBUDDY_MSG_CODE_REFUSE;
	}

	return AGPMBUDDY_MSG_CODE_NONE;
}

//pcsCharacter�� Mentor
EnumAgpmBuddyMsgCode AgsmBuddy::IsValidMentorRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	// �ڱ� �ڽ��� ����� �� ����.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// ģ����Ͽ� �־���Ѵ�.
	if(FALSE == m_pcsAgpmBuddy->IsExistBuddy(pcsCharacter, pcsBuddyElement->m_szName, TRUE, FALSE))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// Mentor�� ��û����(pcsCharacter)�� �ٸ� ���� Mentee�� ��쿡 Mentor�� �� �� ����.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_REQUESTOR_IS_MENTEE;

	// Mentor�� �Ǳ� �������� ����.
	if(TRUE == m_pcsAgpmBuddy->UnderMentorLevel(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTOR_LEVEL;

	// pcsCharacter�� ���� Mentee ���ڰ� �Ѱ� �̻��� ���.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeNumber(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_FULL_MENTEE_NUMBER;

	// ���� �¶��� ���� ĳ���� �ΰ� Ȯ��
	AgpdCharacter *pcsBuddyChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if (NULL == pcsBuddyChar)
		return AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST;

	// ��û���� ĳ���� �̹� �ٸ� ĳ���� Mentee�� ��.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_ALREADY_EXIST_MENTOR;

	// ��û���� ĳ���� �̹� �ٸ� ĳ���� Mentor�� ��.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentor(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_RECEIVER_IS_MENTOR;

	// Mentee�� �Ǳ� �������� ����.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeLevel(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTEE_LEVEL;

	return AGPMBUDDY_MSG_CODE_NONE;
}

//pcsCharacter�� Mentee�� �Ǵ°���.
EnumAgpmBuddyMsgCode AgsmBuddy::IsValidMentorAccept(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	// �ڱ� �ڽ��� ����� �� ����.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// ģ����Ͽ� �־���Ѵ�.
	if(FALSE == m_pcsAgpmBuddy->IsExistBuddy(pcsCharacter, pcsBuddyElement->m_szName, TRUE, FALSE))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// �ڽ��� �̹� �ٸ� ĳ���� Mentee�� ��.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_ALREADY_EXIST_MENTOR;

	// �ڽ��� �̹� �ٸ� ĳ���� Mentor�� ��.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentor(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_RECEIVER_IS_MENTOR;

	// Mentee�� �Ǳ� �������� ����.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeLevel(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTEE_LEVEL;

	// ���� �¶��� ���� ĳ���� �ΰ� Ȯ��
	AgpdCharacter *pcsBuddyChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if (NULL == pcsBuddyChar)
		return AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST;

	// Mentor�����ڰ� �ٸ� ���� Mentee�� ��쿡 Mentor�� �� �� ����.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_REQUESTOR_IS_MENTEE;

	// Mentor�� �Ǳ� �������� ����.
	if(TRUE == m_pcsAgpmBuddy->UnderMentorLevel(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTOR_LEVEL;

	// Mentor�����ڰ� ���� Mentee ���ڰ� �Ѱ� �̻��� ���.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeNumber(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_FULL_MENTEE_NUMBER;

	return AGPMBUDDY_MSG_CODE_NONE;
}

EnumAgpmBuddyMsgCode AgsmBuddy::IsValidMentorDelete(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	// �ڱ� �ڽſ��� ������ �� ����.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	if(AGSMBUDDY_MENTOR_NONE == m_pcsAgpmBuddy->GetMentorStatus(pcsCharacter, pcsBuddyElement->m_szName))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	return AGPMBUDDY_MSG_CODE_NONE;
}

BOOL AgsmBuddy::PartyCheck(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar)
{
	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsTargetChar);
	ASSERT(NULL != pcsBuddyADChar);

	ApString<AGPDCHARACTER_NAME_LENGTH> strCharName(pcsCharacter->m_szID);

	ApVectorBuddy::iterator iter = std::find(pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), strCharName.GetBuffer());
	if (iter == pcsBuddyADChar->m_vtBan.end())
		return TRUE;

	return !(iter->IsBlockInvitation());
}

BOOL AgsmBuddy::PrivateTradeCheck(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar)
{
	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsTargetChar);
	ASSERT(NULL != pcsBuddyADChar);

	ApString<AGPDCHARACTER_NAME_LENGTH> strCharName(pcsCharacter->m_szID);

	ApVectorBuddy::iterator iter = std::find(pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), strCharName.GetBuffer());
	if (iter == pcsBuddyADChar->m_vtBan.end())
		return TRUE;

	return !(iter->IsBlockTrade());
}