/******************************************************************************
Module:  AgsmAccountManager.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 27
******************************************************************************/

#include "AgsmAccountManager.h"
#include "AgsmBillInfo.h"

AgsmAccountManager* AgsmAccountManager::m_pInstance = NULL;

AgsmAccountManager::AgsmAccountManager()
{
	m_pInstance = this;

	SetModuleName("AgsmAccountManager");

	//SetPacketType(AGSMACCOUNT_PACKET_TYPE);

	SetModuleData(sizeof(AgsdAccount), AGSMACCOUNT_DATA_TYPE_ACCOUNT);

	/*
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,									// operation
							AUTYPE_INT32,		1,									// account id
							AUTYPE_CHAR,		AGSMACCOUNT_MAX_ACCOUNT_NAME,		// account name
							AUTYPE_INT32,		AGSMACCOUNT_MAX_ACCOUNT_CHARACTER,	// ������ character id
							AUTYPE_INT32,		1,									// selected character (�÷������� ĳ����)
							AUTYPE_END,			0);
	*/

	EnableIdle2(TRUE);

	m_ulPrevRemoveClockCount	= 0;
	m_pcsAgsmBillInfo			= NULL;
}

AgsmAccountManager::~AgsmAccountManager()
{
}

BOOL AgsmAccountManager::OnAddModule()
{
	m_pagpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmBillInfo		= (AgpmBillInfo *)		GetModule("AgpmBillInfo");

	if (!m_pagpmCharacter ||
		!m_pcsAgpmBillInfo)
		return FALSE;

	return TRUE;
}

BOOL AgsmAccountManager::OnInit()
{
	if (!m_csAccountAdmin.InitializeObject(sizeof(AgsdAccount *), m_csAccountAdmin.GetCount()))
		return FALSE;

	if (!m_csAdminAccountRemove.InitializeObject(sizeof(AgsdAccount *), m_csAdminAccountRemove.GetCount()))
		return FALSE;
	
	m_csGenerateID.Initialize();

	m_pcsAgsmBillInfo = (AgsmBillInfo *) GetModule("AgsmBillInfo");

	if(!m_pcsAgsmBillInfo)
		return FALSE;

	return TRUE;
}

BOOL AgsmAccountManager::OnDestroy()
{
	INT32	lIndex = 0;
	AgsdAccount **ppcsAccount = (AgsdAccount **) m_csAccountAdmin.GetObjectSequence(&lIndex);

	while (ppcsAccount && *ppcsAccount)
	{
		DestroyAccount(*ppcsAccount);

		ppcsAccount = (AgsdAccount **) m_csAccountAdmin.GetObjectSequence(&lIndex);
	}

	lIndex = 0;
	ppcsAccount	= (AgsdAccount **) m_csAdminAccountRemove.GetObjectSequence(&lIndex);
	while (ppcsAccount && *ppcsAccount)
	{
		DestroyAccount(*ppcsAccount);

		ppcsAccount	= (AgsdAccount **) m_csAdminAccountRemove.GetObjectSequence(&lIndex);
	}

	m_csAccountAdmin.RemoveObjectAll();
	m_csAdminAccountRemove.RemoveObjectAll();

	return TRUE;
}

BOOL AgsmAccountManager::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmAccountManager::OnIdle2(UINT32 ulClockCount)
{
	ProcessRemove(ulClockCount);

	return TRUE;
}

BOOL AgsmAccountManager::SetMaxAccount(INT32 lCount)
{
	return m_csAccountAdmin.SetCount(lCount);
}

/*
BOOL AgsmAccountManager::InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag)
{
	return m_csGenerateID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag);
}
*/

AgsdAccount* AgsmAccountManager::CreateAccount()
{
	AgsdAccount *pcsAccount = (AgsdAccount *) CreateModuleData(AGSMACCOUNT_DATA_TYPE_ACCOUNT);
	if (pcsAccount)
	{
		pcsAccount->m_Mutex.Init((PVOID) pcsAccount);

		pcsAccount->m_lID			= AP_INVALID_CID;
		//pcsAccount->m_lSelectCharacter = AP_INVALID_CID;
		pcsAccount->m_nNumChar = 0;
		ZeroMemory(pcsAccount->m_szName, sizeof(CHAR) * (AGSMACCOUNT_MAX_ACCOUNT_NAME + 1));
		//ZeroMemory(pcsAccount->m_lCID, sizeof(INT32) * AGSMACCOUNT_MAX_ACCOUNT_CHARACTER);
		pcsAccount->m_lCID.MemSetAll();
		ZeroMemory(pcsAccount->m_szCharName, sizeof(CHAR) * AGSMACCOUNT_MAX_ACCOUNT_CHARACTER * (AGPACHARACTER_MAX_ID_STRING + 1));

		pcsAccount->m_ulNID			= 0;

		pcsAccount->m_eLoginStep	= AGPMLOGIN_STEP_NONE;

		ZeroMemory(pcsAccount->m_szSocialNumber, sizeof(CHAR) * (AGSMACCOUNT_MAX_SOCIAL_NUMBER + 1));

		pcsAccount->m_ulRemoveTimeMSec	= 0;

		return pcsAccount;
	}

	return NULL;
}

BOOL AgsmAccountManager::DestroyAccount(AgsdAccount *pcsAccount)
{
	if (!pcsAccount)
		return FALSE;

	if (pcsAccount->m_csCoupon.m_pcsCouponInfo)
		delete [] pcsAccount->m_csCoupon.m_pcsCouponInfo;

	pcsAccount->m_Mutex.Destroy();

	return DestroyModuleData((PVOID) pcsAccount, AGSMACCOUNT_DATA_TYPE_ACCOUNT);
}

AgsdAccount* AgsmAccountManager::AddAccount(CHAR *szAccountID, UINT32 ulNID)
{
	if (!szAccountID || ulNID == 0)
		return NULL;

	AgsdAccount	*pcsAccount	= CreateAccount();
	if (pcsAccount)
	{
		strncpy(pcsAccount->m_szName, szAccountID, AGSMACCOUNT_MAX_ACCOUNT_NAME);

		pcsAccount->m_lID	= m_csGenerateID.GetID();
		pcsAccount->m_ulNID	= ulNID;

		if (!m_csAccountAdmin.AddAccount(pcsAccount, pcsAccount->m_lID, pcsAccount->m_szName))
		{
			if (m_csAdminAccountRemove.GetCount() > 0)
				AddRemoveAccount(pcsAccount);
			else
				DestroyAccount(pcsAccount);

			pcsAccount	= NULL;
		}

		return pcsAccount;
	}

	return NULL;
}

AgsdAccount* AgsmAccountManager::GetAccount(CHAR *szAccountID)
{
	return m_csAccountAdmin.GetAccount(szAccountID);
}

AgsdAccount* AgsmAccountManager::GetAccount(INT32 lAccountID)
{
	return m_csAccountAdmin.GetAccount(lAccountID);
}

AgsdAccount* AgsmAccountManager::GetAccountLock(CHAR *szAccountID)
{
	if (!szAccountID || !szAccountID[0])
		return NULL;

	if (!m_csAccountAdmin.GlobalWLock())
		return NULL;

	AgsdAccount	*pcsAccount	= GetAccount(szAccountID);
	if (!pcsAccount)
	{
		m_csAccountAdmin.GlobalRelease();
		return NULL;
	}

	if (!pcsAccount->m_Mutex.WLock())
	{
		m_csAccountAdmin.GlobalRelease();
		return NULL;
	}

	if (!m_csAccountAdmin.GlobalRelease())
	{
		pcsAccount->m_Mutex.Release();
		return NULL;
	}

	return pcsAccount;
}

AgsdAccount* AgsmAccountManager::GetAccountLock(INT32 lAccountID)
{
	if (lAccountID == 0)
		return NULL;

	if (!m_csAccountAdmin.GlobalWLock())
		return NULL;

	AgsdAccount	*pcsAccount	= GetAccount(lAccountID);
	if (!pcsAccount)
	{
		m_csAccountAdmin.GlobalRelease();
		return NULL;
	}

	if (!pcsAccount->m_Mutex.WLock())
	{
		m_csAccountAdmin.GlobalRelease();
		return NULL;
	}

	if (!m_csAccountAdmin.GlobalRelease())
	{
		pcsAccount->m_Mutex.Release();
		return NULL;
	}

	return pcsAccount;
}

BOOL AgsmAccountManager::RemoveAccount(CHAR *szAccountID, BOOL bRemoveCharacter)
{
	m_csAccountAdmin.GlobalWLock();

	AgsdAccount	*pcsAccount	= GetAccount(szAccountID);
	if (!pcsAccount)
	{
		m_csAccountAdmin.GlobalRelease();
		return FALSE;
	}

	pcsAccount->m_Mutex.RemoveLock();

	m_csAccountAdmin.GlobalRelease();

	if (bRemoveCharacter)
	{
		for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
		{
			EnumCallback(AGSMACCOUNT_CB_ID_REMOVE_CHARACTER, &pcsAccount->m_lCID[i], NULL);
		}

		pcsAccount->m_nNumChar	= 0;
		pcsAccount->m_lCID[0]	= 0;
	}

	m_csAccountAdmin.RemoveAccount(szAccountID);

	pcsAccount->m_Mutex.SafeRelease();

	if (m_csAdminAccountRemove.GetCount() > 0)
		return AddRemoveAccount(pcsAccount);
	else
		return DestroyAccount(pcsAccount);

	return TRUE;
}

BOOL AgsmAccountManager::RemoveAccount(INT32 lAccountID, BOOL bRemoveCharacter)
{
	m_csAccountAdmin.GlobalWLock();

	AgsdAccount	*pcsAccount	= GetAccount(lAccountID);
	if (!pcsAccount)
	{
		m_csAccountAdmin.GlobalRelease();
		return FALSE;
	}

	pcsAccount->m_Mutex.RemoveLock();

	m_csAccountAdmin.GlobalRelease();

	if (bRemoveCharacter)
	{
		for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
		{
			EnumCallback(AGSMACCOUNT_CB_ID_REMOVE_CHARACTER, &pcsAccount->m_lCID[i], NULL);
		}

		pcsAccount->m_nNumChar	= 0;
		pcsAccount->m_lCID[0]	= 0;
	}

	m_csAccountAdmin.RemoveAccount(lAccountID);

	pcsAccount->m_Mutex.SafeRelease();

	if (m_csAdminAccountRemove.GetCount() > 0)
		return AddRemoveAccount(pcsAccount);
	else
		return DestroyAccount(pcsAccount);

	return TRUE;
}

BOOL AgsmAccountManager::AddCharacterToAccount(CHAR *szAccountID, INT32 lCID, CHAR *szCharName, BOOL bIsSetBillInfo)
{
	if (!szAccountID || !szAccountID[0] ||
		lCID == AP_INVALID_CID ||
		!szCharName || !szCharName[0])
		return FALSE;

	AgsdAccount	*pcsAccount	= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return FALSE;

	if (pcsAccount->m_nNumChar >= AGSMACCOUNT_MAX_ACCOUNT_CHARACTER)
	{
		pcsAccount->m_Mutex.Release();
		return FALSE;
	}

	pcsAccount->m_lCID[pcsAccount->m_nNumChar]	= lCID;
	ZeroMemory(pcsAccount->m_szCharName[pcsAccount->m_nNumChar], sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	strncpy(pcsAccount->m_szCharName[pcsAccount->m_nNumChar], szCharName, AGPACHARACTER_MAX_ID_STRING);

	if (bIsSetBillInfo)
	{
		AgpdCharacter	*pcsMember	= m_pagpmCharacter->GetCharacterLock(lCID);
		if (pcsMember)
		{
			AgpdBillInfo	*pcsAttachBillInfo	= m_pcsAgpmBillInfo->GetADCharacter(pcsMember);
			*pcsAttachBillInfo	= pcsAccount->m_csBillInfo;

			pcsMember->m_Mutex.Release();
		}
	}

	++pcsAccount->m_nNumChar;

	pcsAccount->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAccountManager::RemoveCharacterFromAccount(CHAR *szAccountID, CHAR *szCharName)
{
	if (!szAccountID || !szAccountID[0] ||
		!szCharName || !szCharName[0])
		return FALSE;

	AgsdAccount	*pcsAccount	= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return FALSE;

	if (pcsAccount->m_nNumChar <= 0)
	{
		pcsAccount->m_Mutex.Release();
		return FALSE;
	}

	for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
	{
		if (strncmp(pcsAccount->m_szCharName[i], szCharName, AGPACHARACTER_MAX_ID_STRING) == 0)
		{
			//CopyMemory(pcsAccount->m_lCID + i, pcsAccount->m_lCID + i + 1, sizeof(INT32) * (pcsAccount->m_nNumChar - i - 1));
			pcsAccount->m_lCID.MemCopy(i, &pcsAccount->m_lCID[i + 1], pcsAccount->m_nNumChar - i - 1);
			CopyMemory((CHAR *) pcsAccount->m_szCharName + i * (AGPACHARACTER_MAX_ID_STRING + 1),
					   (CHAR *) pcsAccount->m_szCharName + (i + 1) * (AGPACHARACTER_MAX_ID_STRING + 1),
					   sizeof(CHAR) * ((pcsAccount->m_nNumChar - i - 1) * (AGPACHARACTER_MAX_ID_STRING + 1)));

			--pcsAccount->m_nNumChar;
		
			pcsAccount->m_Mutex.Release();

			return TRUE;
		}
	}

	pcsAccount->m_Mutex.Release();

	return FALSE;
}

INT32 AgsmAccountManager::GetCIDFromAccount(CHAR *szAccountID, CHAR *szCharName)
{
	if (!szAccountID || !szAccountID[0] ||
		!szCharName || !szCharName[0])
		return 0;

	AgsdAccount	*pcsAccount	= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return 0;

	if (pcsAccount->m_nNumChar <= 0)
	{
		pcsAccount->m_Mutex.Release();
		return 0;
	}

	for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
	{
		if (strncmp(pcsAccount->m_szCharName[i], szCharName, AGPACHARACTER_MAX_ID_STRING) == 0)
		{
			INT32	lCID	= pcsAccount->m_lCID[i];

			pcsAccount->m_Mutex.Release();

			return lCID;
		}
	}

	pcsAccount->m_Mutex.Release();

	return 0;
}

BOOL AgsmAccountManager::RemoveAllCharacters(CHAR *szAccountID)
{
	if (!szAccountID || !szAccountID[0])
		return FALSE;

	AgsdAccount	*pcsAccount	= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return FALSE;

	if (pcsAccount->m_nNumChar <= 0)
	{
		pcsAccount->m_Mutex.Release();
		return TRUE;
	}

	for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
	{
		EnumCallback(AGSMACCOUNT_CB_ID_REMOVE_CHARACTER, &pcsAccount->m_lCID[i], NULL);
	}

	pcsAccount->m_nNumChar	= 0;
	//ZeroMemory(pcsAccount->m_lCID, sizeof(INT32) * AGSMACCOUNT_MAX_ACCOUNT_CHARACTER);
	pcsAccount->m_lCID.MemSetAll();
	ZeroMemory(pcsAccount->m_szCharName, sizeof(CHAR) * AGSMACCOUNT_MAX_ACCOUNT_CHARACTER * (AGPACHARACTER_MAX_ID_STRING + 1));

	pcsAccount->m_Mutex.Release();
	return TRUE;
}

BOOL AgsmAccountManager::RemoveCharacterExceptOne(CHAR *szAccountID, CHAR *szCharName)
{
	if (!szAccountID || !szAccountID[0] ||
		!szCharName || !szCharName[0])
		return 0;

	AgsdAccount	*pcsAccount	= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return 0;

	if (pcsAccount->m_nNumChar <= 0)
	{
		pcsAccount->m_Mutex.Release();
		return 0;
	}

	INT32	lCID	= 0;

	for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
	{
		if (strncmp(pcsAccount->m_szCharName[i], szCharName, AGPACHARACTER_MAX_ID_STRING) == 0)
		{
			lCID	= pcsAccount->m_lCID[i];
		}
		else
		{
			AgpdCharacter	*pcsRemoveCharacter	= m_pagpmCharacter->GetCharacter(pcsAccount->m_lCID[i]);
			if (pcsRemoveCharacter)
			{
				EnumCallback(AGSMACCOUNT_CB_ID_RESET_NID, pcsRemoveCharacter, NULL);

				EnumCallback(AGSMACCOUNT_CB_ID_REMOVE_CHARACTER, &pcsAccount->m_lCID[i], NULL);
			}
		}
	}

	pcsAccount->m_nNumChar	= 1;
	pcsAccount->m_lCID[0]	= lCID;
	ZeroMemory(pcsAccount->m_szCharName[0], sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	strncpy(pcsAccount->m_szCharName[0], szCharName, AGPACHARACTER_MAX_ID_STRING);

	pcsAccount->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAccountManager::SetLoginStep(CHAR *szAccountID, AgpdLoginStep eNewStep)
{
	if (!szAccountID)
		return FALSE;

	AgsdAccount	*pcsAccount		= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return FALSE;

	pcsAccount->m_eLoginStep	= eNewStep;

	pcsAccount->m_Mutex.Release();

	return TRUE;
}

AgpdLoginStep AgsmAccountManager::GetLoginStep(CHAR *szAccountID)
{
	if (!szAccountID)
		return AGPMLOGIN_STEP_NONE;

	AgsdAccount	*pcsAccount		= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return AGPMLOGIN_STEP_NONE;

	AgpdLoginStep	eLoginStep	= pcsAccount->m_eLoginStep;

	pcsAccount->m_Mutex.Release();

	return eLoginStep;
}

BOOL AgsmAccountManager::SetCallbackResetNID(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMACCOUNT_CB_ID_RESET_NID, pfCallback, pClass);
}

BOOL AgsmAccountManager::SetCallbackRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMACCOUNT_CB_ID_REMOVE_CHARACTER, pfCallback, pClass);
}

BOOL AgsmAccountManager::SetBillInfo(CHAR *pszAccountID, AgpdBillInfo *pcsBillInfo)
{
	if (!pszAccountID || !pszAccountID[0] || !pcsBillInfo)
		return FALSE;

	AgsdAccount	*pcsAccount	= GetAccountLock(pszAccountID);
	if (!pcsAccount)
		return FALSE;

	pcsAccount->m_csBillInfo	= *pcsBillInfo;

	for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pagpmCharacter->GetCharacter(pcsAccount->m_lCID[i]);
		if (!pcsMember)
			continue;

		AgpdBillInfo	*pcsAttachBillInfo	= m_pcsAgpmBillInfo->GetADCharacter(pcsMember);
		*pcsAttachBillInfo	= *pcsBillInfo;
	}

	pcsAccount->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAccountManager::SetCouponNum(AgsdAccount *pcsAccount, INT32 lCoupon)
{
	if (!pcsAccount || lCoupon <= 0 || lCoupon > AGSMACCOUNT_MAX_COUPON)
		return FALSE;

	if (pcsAccount->m_csCoupon.m_pcsCouponInfo)
		delete [] pcsAccount->m_csCoupon.m_pcsCouponInfo;

	pcsAccount->m_csCoupon.m_pcsCouponInfo	= new CouponInfo[lCoupon];
	ZeroMemory(pcsAccount->m_csCoupon.m_pcsCouponInfo, sizeof(CouponInfo) * lCoupon);

	if (!pcsAccount->m_csCoupon.m_pcsCouponInfo)
	{
		pcsAccount->m_csCoupon.m_lNumCoupon	= 0;
		return FALSE;
	}

	pcsAccount->m_csCoupon.m_lNumCoupon	= lCoupon;

	return TRUE;
}

CouponInfo* AgsmAccountManager::AddCouponInfo(AgsdAccount *pcsAccount, CouponInfo *pcsCouponInfo)
{
	if (!pcsAccount || pcsAccount->m_csCoupon.m_lNumCoupon < 1 || !pcsAccount->m_csCoupon.m_pcsCouponInfo || !pcsCouponInfo)
		return NULL;

	for (int i = 0; i < pcsAccount->m_csCoupon.m_lNumCoupon; ++i)
	{
		if (!pcsAccount->m_csCoupon.m_pcsCouponInfo[i].m_szCouponNo[0])
		{
			pcsAccount->m_csCoupon.m_pcsCouponInfo[i]	= *pcsCouponInfo;

			return &pcsAccount->m_csCoupon.m_pcsCouponInfo[i];
		}
	}

	return NULL;
}

INT32 AgsmAccountManager::GetAccountNum()
{
	return m_csAccountAdmin.m_csObject.m_lDataCount;
}

BOOL AgsmAccountManager::ProcessRemove(UINT32 ulClockCount)
{
	PROFILE("AgsmAccountManager::ProcessRemove");

	if (m_ulPrevRemoveClockCount + 5000 > ulClockCount)
		return TRUE;

	INT32	lIndex	= 0;
	AgsdAccount	*pcsAccount	= NULL;

	AgsdAccount	**ppcsAccount = (AgsdAccount **) m_csAdminAccountRemove.GetObjectSequence(&lIndex);

	while (ppcsAccount && *ppcsAccount)
	{
		INT32	lRemovedAccountID	= AP_INVALID_CID;
		pcsAccount					= *ppcsAccount;

		if (pcsAccount->m_ulRemoveTimeMSec + 30000 < ulClockCount)
		{
			pcsAccount->m_Mutex.WLock();
			pcsAccount->m_Mutex.Release();

			// ���� ��� ����Ÿ�� ������ ���� �Ǿ���.
			m_csAdminAccountRemove.RemoveObject((INT_PTR) pcsAccount);

			DestroyAccount(pcsAccount);
		}

		ppcsAccount = (AgsdAccount **) m_csAdminAccountRemove.GetObjectSequence(&lIndex);
	}

	m_ulPrevRemoveClockCount = ulClockCount;

	return TRUE;
}

BOOL AgsmAccountManager::AddRemoveAccount(AgsdAccount *pcsAccount)
{
	if (!pcsAccount)
		return FALSE;

	pcsAccount->m_ulRemoveTimeMSec	= GetClockCount();

	if (!m_csAdminAccountRemove.AddObject(&pcsAccount, (INT_PTR) pcsAccount))
	{
		AgsdAccount	**ppcsAccount = (AgsdAccount **) m_csAdminAccountRemove.GetObject((INT_PTR) pcsAccount);
		if (ppcsAccount && *ppcsAccount)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAccountManager::AddRemoveAccount() Already exist in Pool !!!\n");
			AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
		}
		else
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAccountManager::AddRemoveAccount() Remove Pool Full !!!\n");
			AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);

			if (pcsAccount)
				DestroyAccount(pcsAccount);
		}

		return FALSE;
	}

	return TRUE;
}

BOOL AgsmAccountManager::SetMaxAccountRemove(INT32 nCount)
{
	return m_csAdminAccountRemove.SetCount(nCount);
}

AgpdCharacter* AgsmAccountManager::GetPlayingCharacter(CHAR *szAccountID)
{
	if(NULL == szAccountID)
		return NULL;


	AgsdAccount	*pcsAccount	= GetAccountLock(szAccountID);
	if (!pcsAccount)
		return FALSE;

	AgpdCharacter *pcsPlayingCharacter = NULL;

	for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
	{
		AgpdCharacter	*pcsMember	= m_pagpmCharacter->GetCharacter(pcsAccount->m_lCID[i]);
		if (!pcsMember)
			continue;

		if(pcsMember)
		{
			pcsPlayingCharacter = pcsMember;
			break;
		}
	}

	pcsAccount->m_Mutex.Release();

	return pcsPlayingCharacter;	
}

BOOL AgsmAccountManager::UpdateTPackType(CHAR* szAccount, BOOL bAdd)
{
	if(NULL == szAccount)
		return FALSE;

	AgpdCharacter*		pcsPlayingCharacter = GetPlayingCharacter(szAccount);
	if(NULL == pcsPlayingCharacter)
		return FALSE;

	AgpdBillInfo*		pcsAttachBillInfo	= m_pcsAgpmBillInfo->GetADCharacter(pcsPlayingCharacter);
	if(NULL == pcsAttachBillInfo)
		return FALSE;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Call Expired TPack Callback -> UpdateTPackType!!! User : %s, bAdd : %d\n", pcsPlayingCharacter->m_szID, (INT32)bAdd);
	AuLogFile_s(TPACK_EXPIRED_CALLBACK_LOG_FILENAME, strCharBuff);

	if(bAdd)
	{
		pcsAttachBillInfo->m_ulPCRoomType |= AGPDPCROOMTYPE_HANGAME_TPACK;
	}
	else
	{
		pcsAttachBillInfo->m_ulPCRoomType &= ~AGPDPCROOMTYPE_HANGAME_TPACK;		
	}

	// T-PC���̳� S-PC�� ���� �ϳ��� Ÿ���̸� PC�� ������ ���ְ�
	if(pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK ||
	   pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_S)
	{
		if(pcsAttachBillInfo->m_bIsPCRoom == FALSE)
		{
			m_pcsAgpmBillInfo->UpdateIsPCRoom(pcsPlayingCharacter, TRUE);
		}
	}
	// T-PC���̳� S-PC�� ���� �ϳ��� ������ �ȵǾ������� PC���� �ƴϰ� ó���Ѵ�.
	else
	{
		if(pcsAttachBillInfo->m_bIsPCRoom == TRUE)
		{
			m_pcsAgpmBillInfo->UpdateIsPCRoom(pcsPlayingCharacter, FALSE);
		}
	}

	m_pcsAgsmBillInfo->SendCharacterBillingInfo(pcsPlayingCharacter);

	return TRUE;
}
#ifdef _AREA_KOREA_
int AgsmAccountManager::OnBillingEventNotified(const Dorian::LPBILLING_NOTI_INFO pInfo)
{
	if(NULL == pInfo)
		return 0;

	AgsmAccountManager *pThis = AgsmAccountManager::GetInstance();
	if(!pThis)
		return 0;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "Call Expired TPack Callback!!! User : %s, Code : %d\n", pInfo->szUserID, pInfo->nEventCode);
	AuLogFile_s(TPACK_EXPIRED_CALLBACK_LOG_FILENAME, strCharBuff);

	if ( pInfo->nEventCode == Dorian::BILLING_EXPIRE )
	{
		// ��ǰ�� ���� ó��
		// pInfo->szUserID, pInfo->szUserIP �ʵ� ���� �̿��Ͽ� ��ǰ�� ����� �Ѱ��ӻ���ڸ� ó���Ѵ�.
		pThis->UpdateTPackType(pInfo->szUserID, FALSE);

	}
	else if ( pInfo->nEventCode == Dorian::BILLING_REMAINING_TIME_ALRAM )
	{
		// ��ǰ�� �ܿ� �ð� 5�� ���� ���� ó��
		// pInfo->szUserID, pInfo->szUserIP, pInfo->nSpecificCode(���� �ð�) �ʵ� ���� �̿��Ͽ�
		// �Ѱ��ӻ���ڿ��� �ܿ� �ð��� �뺸�Ѵ�.
	}
	else if ( pInfo->nEventCode == Dorian::CONFIRM_USER_EXIST )
	{

	}

	else if ( pInfo->nEventCode >= Dorian::BILLING_GAME_SPECIFIC )
	{
		// �η�A. ȯ�� ���� ������ ���� ��,
		// SPECIFIC_EVENT_CODE_INFO �׷��� �̺�Ʈ �ڵ带 ���⼭ ó���Ѵ�
		switch ( pInfo-> nSpecificCode )
		{
		case 0:
			// ��ǰ event code 0�� ó��
			break;
		case 1:
			// ��ǰ event code 1�� ó��
			break;
		}
	}

	return 0;
}
#endif