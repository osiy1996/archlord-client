/*===============================================================

	AgsmRelay2Character.cpp

===============================================================*/


#include "AgsmRelay2.h"
#include "ApMemoryTracker.h"
#include "AgspCharacter.h"


//
//	==========		Character		==========
//
void AgsmRelay2::InitPacketCharacter()
	{
	m_csPacketCharacter.SetFlagLength(sizeof(INT32));
	m_csPacketCharacter.SetFieldType(AUTYPE_INT16,		1,							// eAgsmRelay2Operation
									 AUTYPE_CHAR,		_MAX_CHARNAME_LENGTH + 1,	// Character ID
									 AUTYPE_CHAR,		_MAX_TITLE_LENGTH + 1,		// Title
									 AUTYPE_CHAR,		_MAX_NICKNAME_LENGTH + 1,	// Nick name
									 AUTYPE_CHAR,		_MAX_POSITION_LENGTH + 1,	// Character Position
									 AUTYPE_INT8,		1,							// Criminal Status
									 AUTYPE_INT32,		1,							// Murderer Point
									 AUTYPE_INT64,		1,							// Inventory Money
								 	 AUTYPE_UINT32,		1,							// Remained criminal flag time
									 AUTYPE_UINT32,		1,							// Remained murder flag time
									 AUTYPE_INT32,		1,							// hp
									 AUTYPE_INT32,		1,							// mp
									 AUTYPE_INT32,		1,							// sp
									 AUTYPE_INT64,		1,							// exp
									 AUTYPE_INT32,		1,							// lv
									 AUTYPE_INT32,		1,							// skill point
									 AUTYPE_UINT8,		1,							// region index
									 AUTYPE_INT8,		1,							// action status
									 AUTYPE_INT32,		1,							// hair
									 AUTYPE_INT32,		1,							// face
									 AUTYPE_INT8,		1,							// dead type
									 AUTYPE_INT32,		1,							// charisma point
									 AUTYPE_INT32,		1,							// Heroic point
									 AUTYPE_UINT32,		1,							// Battle Square Last Killed Time
									 AUTYPE_END,        0
									 );
	}


BOOL AgsmRelay2::OnParamCharacter(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	AgsdRelay2Character *pcsRelayCharacter = new AgsdRelay2Character;

	CHAR	*pszName = NULL;
	CHAR	*pszSkillInit = NULL;
	CHAR	*pszNickName = NULL;
	CHAR	*pszPosition = NULL;
	INT64	llInventoryMoney = 0;

	m_csPacketCharacter.GetField(FALSE, pvPacket, 0,
								 &pcsRelayCharacter->m_eOperation,
								 &pszName,
								 &pszSkillInit,
								 &pszNickName,
								 &pszPosition,
								 &pcsRelayCharacter->m_cCriminalStatus,
								 &pcsRelayCharacter->m_lMurderPoint,
								 &llInventoryMoney,
								 &pcsRelayCharacter->m_ulRemainCriminalTime,
								 &pcsRelayCharacter->m_ulRemainMurderTime,
								 &pcsRelayCharacter->m_lHP,
								 &pcsRelayCharacter->m_lMP,
								 &pcsRelayCharacter->m_lSP,
								 &pcsRelayCharacter->m_llExp,
								 &pcsRelayCharacter->m_lLevel,
								 &pcsRelayCharacter->m_lSkillPoint,
								 &pcsRelayCharacter->m_ucRegionIndex,
								 &pcsRelayCharacter->m_ucActionStatus,
								 &pcsRelayCharacter->m_lHair,
								 &pcsRelayCharacter->m_lFace,
								 &pcsRelayCharacter->m_cDeadType,
								 &pcsRelayCharacter->m_lCharismaPoint,
								 &pcsRelayCharacter->m_lHeroicPoint,
								 &pcsRelayCharacter->m_ulBattleSquareLastKilledTime
								 );
	
	if (!pszName || _ctextend == *pszName)
		{
		pcsRelayCharacter->Release();
		return FALSE;
		}

	strncpy(pcsRelayCharacter->m_szName, pszName, _MAX_CHARNAME_LENGTH);
	strncpy(pcsRelayCharacter->m_szSkillInit, pszSkillInit? pszSkillInit : "", _MAX_TITLE_LENGTH);
	strncpy(pcsRelayCharacter->m_szNickName, pszNickName ? pszNickName : "", _MAX_NICKNAME_LENGTH);
	strncpy(pcsRelayCharacter->m_szPosition, pszPosition ? pszPosition : "", _MAX_POSITION_LENGTH);
	_i64toa(llInventoryMoney, pcsRelayCharacter->m_szInventoryMoney, 10);
	_i64toa(pcsRelayCharacter->m_llExp, pcsRelayCharacter->m_szExp, 10);

	pcsRelayCharacter->m_ulNID = ulNID;
	pcsRelayCharacter->m_nParam = nParam;

	return EnumCallback(nParam, (PVOID)pcsRelayCharacter, (PVOID)nParam);
	}

BOOL AgsmRelay2::OnParamNotifySaveAll(INT16 nParam, PVOID pvPacket, UINT32 ulNID)
	{
	CHAR*	pszName = NULL;

	m_csPacketCharacter.GetField(FALSE, pvPacket, 0,
								 NULL,
								 &pszName,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,
								 NULL,		// charisma
								 NULL,		// heroic
								 NULL		// m_ulBattleSquareLastKilledTime
								 );

	switch (m_pAgsmServerManager->GetThisServerType())
		{
		case AGSMSERVER_TYPE_RELAY_SERVER:
			return SendNotifySaveAll(pszName, ulNID);
			break;

		case AGSMSERVER_TYPE_GAME_SERVER:
			return ReceiveNotifySaveAll(pszName);
			break;

		default:
			return FALSE;
		}

	return FALSE;
	}

BOOL AgsmRelay2::CBCharacterUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	ASSERT(pData && pClass && "Invalid Parameter for Update Character in AgsmRelay2::CBCharacterUpdate()");

	AgsmRelay2		*pThis			= (AgsmRelay2 *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;
	
	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;
	
	if (pThis->m_pAgpmCharacter->IsReturnToLoginServerStatus(pAgpdCharacter))
		return TRUE;

	return pThis->SendCharacterUpdate(pAgpdCharacter);
	}


BOOL AgsmRelay2::CBUseReverserOrb(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData && !pClass)
		return FALSE;

	AgsmRelay2		*pThis			= (AgsmRelay2 *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;

	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return FALSE;

	if (pThis->m_pAgpmCharacter->IsReturnToLoginServerStatus(pAgpdCharacter))
		return TRUE;
	
	return pThis->SendUseReverseOrb(pAgpdCharacter);
	}


BOOL AgsmRelay2::CBTransform(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData && !pClass)
		return FALSE;

	AgsmRelay2		*pThis			= (AgsmRelay2 *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;

	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return FALSE;
	
	if (pThis->m_pAgpmCharacter->IsReturnToLoginServerStatus(pAgpdCharacter))
		return TRUE;

	return pThis->SendTransform(pAgpdCharacter);	
	}


BOOL AgsmRelay2::CBCharacterGameIn(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData && !pClass)
		return FALSE;

	AgsmRelay2		*pThis			= (AgsmRelay2 *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;
	
	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;

	// dpnid 가 없으면 쓰지 않는다. 2008.03.31. steeple
	if (pThis->m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter) == 0)
		return TRUE;
	
	return pThis->SendCharacterGameInOut(pAgpdCharacter, TRUE);
	}


BOOL AgsmRelay2::CBNotifySaveAll(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData && !pClass)
		return FALSE;

	AgsmRelay2		*pThis			= (AgsmRelay2 *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;

	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;

	return pThis->SendNotifySaveAll(pAgpdCharacter->m_szID, 0);
	}


BOOL AgsmRelay2::CBCharacterGameOut(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData && !pClass)
		return FALSE;

	AgsmRelay2		*pThis			= (AgsmRelay2 *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;

	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;
	
	//STOPWATCH2(pThis->GetModuleName(), _T("CBCharacterGameOut"));

	return pThis->SendCharacterGameInOut(pAgpdCharacter, FALSE);
	}

BOOL AgsmRelay2::CBOperationResultCouponUse(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(NULL == pData || NULL == pClass || NULL == pCustData)
		return FALSE;

	AgsmRelay2					*pThis				= (AgsmRelay2 *) pClass;
	AgsdRelay2CharacterCoupon	*pAgsdRelay2		= (AgsdRelay2CharacterCoupon *) pCustData;
	AuRowset					*pRowset			= (AuRowset *) pData;

	PACKET_AGSP_CHARACTER_ANSWER_COUPON_USE pResultPacket;

	pResultPacket.m_lResultCode = pAgsdRelay2->nResult;
	strncpy_s(pResultPacket.m_szCharName, sizeof(pResultPacket.m_szCharName), pAgsdRelay2->strCharName, _TRUNCATE);

	// 결과값이 1인 경우가 성공이기때문에 ITEMTID를 셋팅해준다.
	if(pResultPacket.m_lResultCode == 1)
	{
		INT32 lRowCount = pRowset->GetRowCount();

		// 0번째는 Result코드이고 그다음부터는 Item TID여서 i를 1번부터 시작
		for(int i=1; i < lRowCount; ++i)
		{
			if(i >= COUPON_ITEM_MAX)
				break;

			pResultPacket.m_lItemInfo[i].m_lItemTID = atoi((CHAR*)pRowset->Get(i, 0));
			pResultPacket.m_lItemInfo[i].m_lCount	= atoi((CHAR*)pRowset->Get(i, 1));
		}
	}
	
	AgsEngine::GetInstance()->SendPacket(pResultPacket, pAgsdRelay2->m_ulNID);

	return TRUE;
}


BOOL AgsmRelay2::SendCharacterUpdate(AgpdCharacter* pAgpdCharacter)
	{
	if (!pAgpdCharacter || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	// get values
	INT16	nPacketLength;
	INT32	lMurdererPoint = 0;
	INT32	lHP = 0;
	INT32	lMP = 0;
	INT32	lSP = 0;
	INT64	llEXP = 0;
	INT32	lLevel = 0;
	INT32	lSkillPoint = 0;
	UINT32	ulCriminalRemainTime = 0;
	UINT32	ulMurdererRemainTime = 0;
	INT32	lHeroicPoint = 0;
	CHAR	szPosition[33];

	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	llEXP	= m_pAgpmFactors->GetExp((AgpdFactor *) m_pAgpmFactors->GetFactor(&pAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));

	switch( m_pAgpmCharacter->GetCharacterLevelLimit( pAgpdCharacter ) )
	{
	case	AgpmCharacter::LLS_BLOCKED	:	// 레벨 제한 걸림.. 경험치는 얻으나 레벨은 못올라감.
		m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		break;

	case	AgpmCharacter::LLS_DOWNED	:	// 레벨 제한걸림 .. 레벨이 다운되어 있음.
		// do nothing
		m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
		m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, 
			AGPD_FACTORS_CHARSTATUS_TYPE_BEFORELEVEL // Before Level을 DB 에 기록함.
			);
		break;

	default:
		if (pAgpdCharacter->m_bIsTrasform && pAgpdCharacter->m_eTransformType != AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY)
		{
			m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactorOriginal, &lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
			m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactorOriginal, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		}
		else
		{
			m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);
			m_pAgpmFactors->GetValue(&pAgpdCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		}
		break;
	}

	lSkillPoint	= m_pAgpmCharacter->GetSkillPoint(pAgpdCharacter);
	lHeroicPoint = m_pAgpmCharacter->GetHeroicPoint(pAgpdCharacter);
	ZeroMemory(szPosition, sizeof(szPosition));
	m_pAgsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 32);
	
	ulCriminalRemainTime = m_pAgpmCharacter->GetRemainedCriminalTime(pAgpdCharacter);
	ulMurdererRemainTime = m_pAgpmCharacter->GetRemainedMurdererTime(pAgpdCharacter);

	UINT8	ucBindingRegionIndex	= (UINT8)	pAgpdCharacter->m_nLastExistBindingIndex;

	AgsdDeath	*pcsAgsdDeath	= m_pAgsmDeath->GetADCharacter(pAgpdCharacter);

	INT32 lCharismaPoint = m_pAgpmCharacter->GetCharismaPoint(pAgpdCharacter);

	AgsdCharacter	*pAgsdCharacter	= m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	
	INT16 nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID pvPacketCharacter = m_csPacketCharacter.MakePacket(FALSE, &nPacketLength, 0,
												&nOperation,
												pAgpdCharacter->m_szID,
												pAgpdCharacter->m_szSkillInit,
												pAgpdCharacter->m_szNickName,
												szPosition,
												&pAgpdCharacter->m_unCriminalStatus,
												&lMurdererPoint,
												&pAgpdCharacter->m_llMoney,
												&ulCriminalRemainTime,
												&ulMurdererRemainTime,
												&lHP,
												&lMP,
												&lSP,
												&llEXP,
												&lLevel,
												&lSkillPoint,
												&ucBindingRegionIndex,
												&pAgpdCharacter->m_unActionStatus,
												&pAgpdCharacter->m_lHairIndex,		// hair
												&pAgpdCharacter->m_lFaceIndex,		// face
												&pcsAgsdDeath->m_cDeadType,
												&lCharismaPoint,					// charisma
												&lHeroicPoint,						// Heroic Point
												&pAgpdCharacter->m_ulBattleSquareKilledTime	// 배틀 스퀘어에서 마지막으로 죽은 시간.
												);
	if (!pvPacketCharacter)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketCharacter, AGSMRELAY_PARAM_CHARACTER);
	m_csPacketCharacter.FreePacket(pvPacketCharacter);

	return bResult;
	}


BOOL AgsmRelay2::SendUseReverseOrb(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	// get values
	INT16	nPacketLength;
	INT16	nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID	pvPacketCharacter = m_csPacketCharacter.MakePacket(FALSE, &nPacketLength, 0,
												&nOperation,
												pAgpdCharacter->m_szID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// charisma
												NULL,		// heroic
												NULL	// 배틀 스퀘어에서 마지막으로 죽은 시간.
												);
	if (!pvPacketCharacter)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketCharacter, AGSMRELAY_PARAM_REVERSEORB);
	m_csPacketCharacter.FreePacket(pvPacketCharacter);

	return bResult;	
	}


BOOL AgsmRelay2::SendTransform(AgpdCharacter *pAgpdCharacter)
	{
	if (!pAgpdCharacter || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	// get values
	INT16	nPacketLength;
	INT16	nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID	pvPacketCharacter = m_csPacketCharacter.MakePacket(FALSE, &nPacketLength, 0,
												&nOperation,
												pAgpdCharacter->m_szID,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// charisma
												NULL,		// heroic
												NULL		// 배틀 스퀘어에서 마지막으로 죽은 시간.
												);
	if (!pvPacketCharacter)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketCharacter, AGSMRELAY_PARAM_TRANSFORM);
	m_csPacketCharacter.FreePacket(pvPacketCharacter);

	return bResult;	
	}


BOOL AgsmRelay2::SendCharacterGameInOut(AgpdCharacter *pAgpdCharacter, BOOL bIn)
	{
	if (!pAgpdCharacter || !m_pAgsmServerManager->GetRelayServer())
		return FALSE;

	// IP address를 땜빵으로 타이틀에 넣었다.
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

	// get values
	INT16	nPacketLength;
	INT16	nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID	pvPacketCharacter = m_csPacketCharacter.MakePacket(FALSE, &nPacketLength, 0,
												&nOperation,
												pAgpdCharacter->m_szID,
												pAgsdCharacter ? &pAgsdCharacter->m_strIPAddress[0] : NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,		// charisma
												NULL,		// heroic
												NULL	// 배틀 스퀘어에서 마지막으로 죽은 시간.
												);
	if (!pvPacketCharacter)
		return FALSE;

	BOOL bResult = MakeAndSendRelayPacket(pvPacketCharacter,
										  bIn ? AGSMRELAY_PARAM_CHARGAMEIN : AGSMRELAY_PARAM_CHARGAMEOUT);
	m_csPacketCharacter.FreePacket(pvPacketCharacter);

	return bResult;	
	}

BOOL AgsmRelay2::SendNotifySaveAll(CHAR *pszName, UINT32 ulNID)
	{
	if (!pszName || !pszName[0])
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "SendNotifySaveAll 이름이없담다 \n");
		AuLogFile_s("LOG\\ReturnToLoginErr.txt", strCharBuff);

		return FALSE;
	}

	// get values
	INT16	nPacketLength;
	INT16	nOperation = AGSMDATABASE_OPERATION_UPDATE;
	PVOID	pvPacketCharacter = m_csPacketCharacter.MakePacket(FALSE, &nPacketLength, 0,
												&nOperation,
												pszName,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,
												NULL,	// charisma
												NULL,
												NULL	// 배틀 스퀘어에서 마지막으로 죽은 시간.
												);
	if (!pvPacketCharacter)
		return FALSE;

	BOOL	bResult	= TRUE;

	switch (m_pAgsmServerManager->GetThisServerType())
		{
		case AGSMSERVER_TYPE_RELAY_SERVER:
			{
				//UINT32	ulNID	= 0;

				//INT16	lIndex	= 0;
				//AgsdServer2	*pcsAgsdServer	= m_pAgsmServerManager->GetGameServers(&lIndex);
				//if (pcsAgsdServer)
				//	ulNID	= pcsAgsdServer->m_dpnidServer;

				AgsdServer2* pcsGameServer = m_pAgsmServerManager->GetGameServerBySocketIndex(ulNID);
				if(!pcsGameServer)
					return FALSE;

				INT32 ulNewNID = pcsGameServer->m_dpnidServer;

				bResult = MakeAndSendRelayPacket(pvPacketCharacter, AGSMRELAY_PARAM_NOTIFY_SAVE_ALL, ulNewNID);
			}
			break;

		case AGSMSERVER_TYPE_GAME_SERVER:
			bResult = MakeAndSendRelayPacket(pvPacketCharacter, AGSMRELAY_PARAM_NOTIFY_SAVE_ALL);
			break;

		default:
			return FALSE;
		}

	m_csPacketCharacter.FreePacket(pvPacketCharacter);

	return bResult;
	}

BOOL AgsmRelay2::ReceiveNotifySaveAll(CHAR *pszName)
	{
		return m_pAgsmReturnToLogin->ReceiveNotifySaveAll(pszName);
	}

// Coupon
BOOL AgsmRelay2::OnParamCoupon(PACKET_HEADER* pvPacket, UINT32 ulNID)
{
	if(NULL == pvPacket)
		return FALSE;

	PACKET_AGSP_CHARACTER *pPacket = static_cast<PACKET_AGSP_CHARACTER *>(pvPacket);

	switch(pPacket->pcOperation)
	{
	// OnRecieve Relay Server
	case AGSP_CHARACTER_REQUEST_COUPON_USE:
		{
			PACKET_AGSP_CHARACTER_REQUEST_COUPON_USE *pConvertPacket = static_cast<PACKET_AGSP_CHARACTER_REQUEST_COUPON_USE *>(pPacket);

			AgsdRelay2CharacterCoupon *pcsCouponUse = new AgsdRelay2CharacterCoupon(ulNID, pConvertPacket->nParam);

			pcsCouponUse->lCouponType = pConvertPacket->stRequestCoupon.m_lCouponType;
			strncpy_s(pcsCouponUse->strCharName, sizeof(pcsCouponUse->strCharName), pConvertPacket->m_szCharName, _TRUNCATE);
			strncpy_s(pcsCouponUse->strCouponNo, sizeof(pcsCouponUse->strCouponNo), pConvertPacket->stRequestCoupon.m_szCouponNo, _TRUNCATE);

			AgsdQueryWithParam* pQuery = new AgsdQueryWithParam;
			pQuery->m_nIndex = AGSMRELAY_QUERY_REQUEST_COUPON_USE;

			pQuery->m_pParam = pcsCouponUse;
			pQuery->SetCallback(AgsmRelay2::CBOperationResultCouponUse, AgsmRelay2::CBOperationResultCouponUse, this, pcsCouponUse);

			m_pAgsmDatabasePool->Execute(pQuery);

		} break;

	case AGSP_CHARACTER_ANSWER_COUPON_USE:
		{
		// OnRecieve Game Server
			PACKET_AGSP_CHARACTER_ANSWER_COUPON_USE *pConvertPacket = static_cast<PACKET_AGSP_CHARACTER_ANSWER_COUPON_USE *>(pPacket);

			// Result Code가 1이면 Item을 지급한다.
			PACKET_CHARACTER_ANSWER_COUPON_USE stSendPacket;

			stSendPacket.m_lResultCode = pConvertPacket->m_lResultCode;

			AgpdCharacter *pcsCharacter = m_pAgpmCharacter->GetCharacter(pConvertPacket->m_szCharName);
			if(NULL == pcsCharacter)
				return FALSE;

			if(pConvertPacket->m_lResultCode == 1)
			{
				for(int i=0; i<COUPON_ITEM_MAX; ++i)
				{
					INT32 lItemTID	= pConvertPacket->m_lItemInfo[i].m_lItemTID;
					INT32 lCount	= pConvertPacket->m_lItemInfo[i].m_lCount;

					m_pAgsmItem->GiveItem(pcsCharacter, lItemTID, lCount, AGPDLOGTYPE_ITEM_COUPON_GIVE);
				}
			}

			AgsEngine::GetInstance()->SendPacket(stSendPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

		} break;
	}
}


/************************************************************/
/*		The Implementation of AgsdRelay2Character class		*/
/************************************************************/
//
AgsdRelay2Character::AgsdRelay2Character()
	{
	ZeroMemory(m_szName, sizeof(m_szName));
	ZeroMemory(m_szSkillInit, sizeof(m_szSkillInit));
	ZeroMemory(m_szNickName, sizeof(m_szNickName));
	ZeroMemory(m_szPosition, sizeof(m_szPosition));
	m_cCriminalStatus = 0;
	m_lMurderPoint = 0;
	ZeroMemory(m_szInventoryMoney, sizeof(m_szInventoryMoney));
	m_ulRemainCriminalTime = 0;
	m_ulRemainMurderTime = 0;
	m_lHP = 0;
	m_lMP = 0;
	m_lSP = 0;
	m_llExp = 0;
	m_lLevel = 0;
	m_lSkillPoint = 0;
	m_ucRegionIndex = 0;
	m_ucActionStatus = 0;
	m_lHair = 0;
	m_lFace = 0;
	m_cDeadType = 0;
	m_lCharismaPoint = 0;
	m_lHeroicPoint = 0;
	m_ulBattleSquareLastKilledTime = 0;
	
	m_nParam = AGSMRELAY_PARAM_CHARACTER;
	}


BOOL AgsdRelay2Character::SetParamUpdate(AuStatement* pStatement)
	{
	INT16 i = 0;
	if (AGSMRELAY_PARAM_CHARACTER == m_nParam)
		{
		pStatement->SetParam(i++, m_szSkillInit, sizeof(m_szSkillInit));
		pStatement->SetParam(i++, m_szNickName, sizeof(m_szNickName));
		pStatement->SetParam(i++, m_szPosition, sizeof(m_szPosition));
		pStatement->SetParam(i++, &m_cCriminalStatus);
		pStatement->SetParam(i++, &m_lMurderPoint);
		pStatement->SetParam(i++, &m_ulRemainCriminalTime);
		pStatement->SetParam(i++, &m_ulRemainMurderTime);
		pStatement->SetParam(i++, m_szInventoryMoney, sizeof(m_szInventoryMoney));
		pStatement->SetParam(i++, &m_lHP);
		pStatement->SetParam(i++, &m_lMP);
		pStatement->SetParam(i++, &m_lSP);
		pStatement->SetParam(i++, m_szExp, sizeof(m_szExp));
		pStatement->SetParam(i++, &m_lLevel);
		pStatement->SetParam(i++, (INT8 *) &m_lSkillPoint);
		pStatement->SetParam(i++, &m_ucRegionIndex);
		pStatement->SetParam(i++, &m_ucActionStatus);
		pStatement->SetParam(i++, &m_lHair);
		pStatement->SetParam(i++, &m_lFace);
		pStatement->SetParam(i++, &m_cDeadType);
		pStatement->SetParam(i++, &m_lCharismaPoint);
		pStatement->SetParam(i++, &m_lHeroicPoint);
		pStatement->SetParam(i++, &m_ulBattleSquareLastKilledTime);
		pStatement->SetParam(i++, m_szName, sizeof(m_szName));
		}
	else if (AGSMRELAY_PARAM_REVERSEORB == m_nParam)
		{
		pStatement->SetParam(i++, m_szName, sizeof(m_szName));
		}
	else if (AGSMRELAY_PARAM_TRANSFORM == m_nParam)
		{
		pStatement->SetParam(i++, m_szName, sizeof(m_szName));
		}
	else if (AGSMRELAY_PARAM_CHARGAMEIN == m_nParam)
		{
		// IP address를 땜빵으로 skill init에 넣었다.
		if (AGS_DB_VENDER_MSSQL != g_eAgsDatabaseVender)
			pStatement->SetParam(i++, m_szName, sizeof(m_szName));
		pStatement->SetParam(i++, m_szSkillInit, sizeof(m_szSkillInit));
		pStatement->SetParam(i++, m_szName, sizeof(m_szName));
		}	
	else if (AGSMRELAY_PARAM_CHARGAMEOUT == m_nParam)
		{
		pStatement->SetParam(i++, m_szName, sizeof(m_szName));
		}	

	return TRUE;
	}

AgsdRelay2CharacterCoupon::AgsdRelay2CharacterCoupon(UINT32 ulNID, INT16 Operation)
{
	m_ulNID = ulNID;
	nOperation = Operation;
	m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;
	
	lCouponType = 0;
	ZeroMemory(strCharName, sizeof(strCharName));
	ZeroMemory(strCouponNo, sizeof(strCouponNo));
	
}

AgsdRelay2CharacterCoupon::~AgsdRelay2CharacterCoupon()
{

}

void AgsdRelay2CharacterCoupon::Release()
{
	delete this;
}

BOOL AgsdRelay2CharacterCoupon::SetParamExecute(AuStatement* pStatement)
{
	if(NULL == pStatement)
		return FALSE;

	pStatement->SetParam( 0, &lCouponType);
	pStatement->SetParam( 1, strCouponNo, sizeof(strCouponNo));
	pStatement->SetParam( 2, strCharName, sizeof(strCharName));

	//Out
	pStatement->SetParam( 3, &nResult, TRUE);

	return TRUE;
}

void AgsdRelay2Character::Dump(CHAR *pszOp)
	{
	CHAR szFile[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szFile, "LOG\\%04d%02d%02d-%02d-RELAY_DUMP_CHAR.log", st.wYear, st.wMonth, st.wDay, st.wHour);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[%02d:%02d:%02d][%s] ID[%s] Lv[%d] Exp[%I64d] SP[%d] HP[%d] MP[%d] Money[%s] Pos[%s]\n",
				st.wHour, st.wMinute, st.wSecond,
				pszOp, m_szName, m_lLevel, m_llExp, m_lSkillPoint, m_lHP, m_lMP, m_szInventoryMoney, m_szPosition);
	AuLogFile_s(szFile, strCharBuff);
	}
