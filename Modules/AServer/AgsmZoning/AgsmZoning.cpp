#include "AgsmZoning.h"

AgsmZoning::AgsmZoning()
{
	SetModuleName("AgsmZoning");

	SetPacketType(AGSMZONING_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,		// operation
							AUTYPE_INT32,			1,		// character id
							AUTYPE_INT8,			1,		// operation�� ���� status
							AUTYPE_CHAR,			23,		// zone server address (xxx.xxx.xxx.xxx:port)
							AUTYPE_INT32,			1,		// zone server id
							AUTYPE_END,				0
							);
}

AgsmZoning::~AgsmZoning()
{
}

BOOL AgsmZoning::OnAddModule()
{
	m_pcsApmMap				= (ApmMap *)			GetModule("ApmMap");
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgsmCharacter		= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pAgsmServerManager	= (AgsmServerManager *)	GetModule("AgsmServerManager2");
	m_pcsAgsmMap			= (AgsmMap *)			GetModule("AgsmMap");
	m_pcsAgsmAOIFilter		= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");

	if (!m_pcsApmMap ||
		!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgsmCharacter ||
		!m_pAgsmServerManager ||
		!m_pcsAgsmMap ||
		!m_pcsAgsmAOIFilter)
		return FALSE;

	// setting attached data(m_nKeeping) in character module
	m_nIndexADCharacter = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgsdZoning), ConAgsdZoning, DesAgsdZoning);
	if (m_nIndexADCharacter < 0)
		return FALSE;

	if (!m_pcsAgsmCharacter->SetCallbackEnterGameworld(CBEnterGameworld, this))
		return FALSE;

	/*
	if (!m_pcsAgsmCharacter->SetCallbackMoveSector(CBUpdatePosition, this))
		return FALSE;
	*/

	/*
	if (!m_pcsAgsmAOIFilter->SetCallbackAddChar(CBAddSector, this))
		return FALSE;
	if (!m_pcsAgsmAOIFilter->SetCallbackRemoveChar(CBRemoveSector, this))
		return FALSE;
	*/

	if (!m_pcsAgsmAOIFilter->SetCallbackAddChar(CBAddCell, this))
		return FALSE;
	if (!m_pcsAgsmAOIFilter->SetCallbackRemoveChar(CBRemoveCell, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmZoning::OnInit()
{
	return TRUE;
}

BOOL AgsmZoning::OnDestroy()
{
	return TRUE;
}

BOOL AgsmZoning::ConAgsdZoning(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
		// invalid constructor parameter
		return FALSE;

	AgsmZoning		*pThis				= (AgsmZoning *)	pClass;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pData;
	AgsdZoning		*pcsAgsdZoning		= pThis->GetADCharacter(pcsCharacter);

//	ZeroMemory(pcsAgsdZoning->m_stZoningStatus, sizeof(stZoningStatus) * 5);
	pcsAgsdZoning->m_stZoningStatus.MemSetAll();

	pcsAgsdZoning->m_bZoning			= FALSE;
	pcsAgsdZoning->m_bDisconnectZoning	= FALSE;

	return TRUE;
}

BOOL AgsmZoning::DesAgsdZoning(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmZoning::CBEnterGameworld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	return TRUE;

	AgsmZoning		*pThis			= (AgsmZoning *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	// �������� �˻��Ѵ�.
	// ���͸� �ƹ��͵� �Ұ� ����.

	UINT32	dpnid	= pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter);
	if (dpnid == 0)
		return TRUE;

	pThis->AllServerAdd(pcsCharacter);

	// �� ĳ������ ��ġ�� ���� �� ������ �����ϴ� ������ �˻��Ѵ�.
	// �� �������� �����ϴ� ���� �ƴϸ� �ش� ������ ���׽�Ų��.
	AgsdServer	*pcsControlServer	= pThis->m_pAgsmServerManager->GetControlServer(pcsCharacter->m_stPos);
	if (!pcsControlServer)
	{
//		TRACEFILE(TRACEFILE"AgsmZoning::EnterGameWorld() GetControlServer failed...");
		return FALSE;
	}

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return FALSE;

	if (pcsAgsdCharacter->m_ulServerID != pcsControlServer->m_lServerID)
	{
		pThis->PassControl(pcsCharacter, pcsControlServer);
		return TRUE;
	}

	return TRUE;
}

/*
BOOL AgsmZoning::CBUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmZoning*		pThis				= (AgsmZoning *)		pClass;
	AgpdCharacter*	pcsAgpdCharacter	= (AgpdCharacter *)		pData;
	ApWorldSector**	paForwardSector		= (ApWorldSector **)	pCustData;

	AgsdCharacter*	pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);
	AgsdZoning*		pcsAgsdZoning		= pThis->GetADCharacter(pcsAgpdCharacter);

	// ���͸� �̵��ߴ�. ������ �ʿ�����... �ʿ��ϸ� ������ �Ѵ�.
	INT32 lThisServer = pThis->m_pcsAgsmMap->GetThisServer();
	INT32 lNewControlServer = pThis->m_pcsAgsmMap->GetControlServer(pcsAgsdCharacter->m_pCurrentSector);

	if (pcsAgsdCharacter->m_ulServerID == lThisServer && pcsAgsdCharacter->m_dpnidCharacter != 0)	// �� ������ �����ϴ� ĳ���Ͷ�� ��ũ�� ���߾��ش�.
		pThis->SendPacketCharSync(pcsAgpdCharacter, pThis->m_pAgsmServerManager->GetGameServersDPNID());

	if (pcsAgsdCharacter->m_ulServerID == lThisServer)
	{
		if (pcsAgsdZoning->m_bZoning && !pThis->m_pcsAgsmMap->IsNeedZoning(pcsAgsdCharacter->m_pCurrentSector) 
			&& !pThis->m_pcsAgsmMap->IsNeedZoning(pcsAgsdCharacter->m_pPrevSector) && lNewControlServer == lThisServer)
		{
			// ������ ��� ������.
			pThis->EndZoning(pcsAgpdCharacter);
		}

		pThis->ProcessZoning(pcsAgpdCharacter, pcsAgsdCharacter->m_pCurrentSector, paForwardSector);
	}

	return TRUE;
}
*/

/*
BOOL AgsmZoning::CBAddSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmZoning			*pThis		= (AgsmZoning *)	pClass;
	ApWorldSector		*pSector	= (ApWorldSector *)	pData;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pCustData;

	return pThis->SyncAddChar(pcsCharacter, pSector);
}

BOOL AgsmZoning::CBRemoveSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmZoning			*pThis		= (AgsmZoning *)	pClass;
	ApWorldSector		*pSector	= (ApWorldSector *) pData;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pCustData;

	return pThis->SyncRemoveChar(pcsCharacter, pSector);
}
*/

BOOL AgsmZoning::CBAddCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmZoning			*pThis			= (AgsmZoning *)	pClass;
	AgsmAOICell			*pcsCell		= (AgsmAOICell *)	pData;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pCustData;

	return pThis->SyncAddChar(pcsCharacter, pcsCell);
}

BOOL AgsmZoning::CBRemoveCell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmZoning			*pThis			= (AgsmZoning *)	pClass;
	AgsmAOICell			*pcsCell		= (AgsmAOICell *)	pData;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pCustData;

	return pThis->SyncRemoveChar(pcsCharacter, pcsCell);
}

BOOL AgsmZoning::SetCallbackZoningStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMZONING_CB_ZONING_START, pfCallback, pClass);
}

BOOL AgsmZoning::SetCallbackZoningPassControl(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMZONING_CB_ZONING_PASS_CONTROL, pfCallback, pClass);
}

BOOL AgsmZoning::SetCallbackSyncAddChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMZONING_CB_SYNCADDCHAR, pfCallback, pClass);
}

AgsdZoning* AgsmZoning::GetADCharacter(AgpdCharacter *pcsCharacter)
{
	return (AgsdZoning *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, pcsCharacter);
}

//		ProcessZoning
//	Functions
//		- ���� ó���� �Ѵ�.
//			pSector�� �˻��ؼ� �� ������ �ʿ����� �˻��Ѵ�.
//			lOldSectorControlServer, lCurSectorControlServer�� �˻��� �̹� ������ �ǰ� �־����� �˻��Ѵ�.
//		- ������ �ʿ��ϴٸ� ó���� ���ش�.
//	Arguments
//		- pcsCharacter : ó���� ĳ����
//		- pSector : ������ �ʿ����� �˻��� ���� ������
//		- paForwardSector : ���� ���⿡ ��ġ�� ���͵� ������ �迭
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmZoning::ProcessZoning(AgpdCharacter *pcsCharacter, ApWorldSector *pSector, ApWorldSector **paForwardSector)
{
	if (!pcsCharacter || !pSector)
		return FALSE;

	if (!m_pcsAgsmMap->IsNeedZoning(pSector))
		return FALSE;							// ������ �ʿ䰡 ����.

	INT32	lThisServer = m_pcsAgsmMap->GetThisServer();

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
	if (pcsAgsdCharacter->m_ulServerID != lThisServer)
		return TRUE;							// ���� �� ĳ���͸� ó���ϴ� ���� �ٸ� ������. ���... �� ���д�.

	INT32	lControlServer = m_pcsAgsmMap->GetControlServer(pSector);
	if (lControlServer == AP_INVALID_SERVERID)
		return FALSE;

	AgsdServer *pcsControlServer = m_pAgsmServerManager->GetServer(lControlServer);
	if (!pcsControlServer)
		return FALSE;			

	if (lControlServer != lThisServer)
	{
		// �̼������� �ٸ� ������ �Ѿ ����̴�. �÷��̾� ��Ʈ���� �ѱ��.
		if (!PassControl(pcsCharacter, pcsControlServer))
		{
			SendZoningFail(pcsCharacter, pcsControlServer->m_lServerID);

			EndZoning(pcsCharacter);
		}
	}
	else
	{
		AgsdZoning	*pcsAgsdZoning	= GetADCharacter(pcsCharacter);

		if (pcsAgsdZoning->m_bZoning)
			return TRUE;					// �̹� ������ �ϰ� �ִ� ���̴�. �� �Ѿ��.

		// ���� ������ �����Ѵ�.
		if (paForwardSector)
		{
			for (int i = 0; i < 9; i++)
			{
				if (!paForwardSector[i])
					continue;

				lControlServer = m_pcsAgsmMap->GetControlServer(paForwardSector[i]);
				if (lControlServer == AP_INVALID_SERVERID)
					continue;

				if (lControlServer == lThisServer)
					continue;

				pcsControlServer = m_pAgsmServerManager->GetServer(lControlServer);
				if (!pcsControlServer)
					continue;

				// �� �������� ������ �̹� ���۵Ǿ� �ִ��� �˻��Ѵ�.
				int j = 0;
				for (j = 0; j < 5; j++)
				{
					if (pcsAgsdZoning->m_stZoningStatus[j].lServerID == lControlServer)
						break;
				}

				if (j != 5)		// �̹� �� ������ ���� ����Ʈ�� ����ִ�.
					return TRUE;

				// ���� �� ������ ã�´�.
				for (j = 0; j < 5; j++)
				{
					if (pcsAgsdZoning->m_stZoningStatus[j].lServerID == AP_INVALID_SERVERID)
						break;
				}

				if (j == 5) // �̷� �󽽷��� ����. ���� �̷� ���� ���� �� ������.. Ȥ�ó�.. �̷� ���� �� �Ѿ��.
					return FALSE;

				if (!CheckZoning(pcsControlServer))	// �⺻���� üũ�� �Ѵ�. ���� ���� ���δ� SendCharacterData() ����� �Ǵ��Ѵ�.
					return FALSE;	// ���� �Ұ���. ���� �����ϰ� �� �Ѿ��.

				pcsAgsdZoning->m_stZoningStatus[j].lServerID = lControlServer;
				pcsAgsdZoning->m_stZoningStatus[j].fZoningStatus = AGSMZONING_ZONING_START_ZONING;
				pcsAgsdZoning->m_bZoning = TRUE;

				// ���� �����̴�. ����� ������ ������ ���۵ȴٰ� �˷��ش�.
				if (!SendStartZoning(pcsCharacter, pcsControlServer))
				{
					pcsAgsdZoning->m_stZoningStatus[j].lServerID = AP_INVALID_SERVERID;
					pcsAgsdZoning->m_stZoningStatus[j].fZoningStatus = AGSMZONING_NOT_ZONING;
					pcsAgsdZoning->m_bZoning = FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL AgsmZoning::CheckZoning(AgsdServer *pcsServer)
{
	// ���⼱ �� ������ ������ �ִ� ����Ÿ�� ������ üũ�� �Ѵ�.
	if (!pcsServer)
		return FALSE;

	if (pcsServer->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
		return FALSE;
	/*
	if (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
		return FALSE;
	*/

	//if (pcsServer->m_nStatus == ) // ���� ���� �������� �˻�
	//
	//		���Ŀ� ����
	//

	return TRUE;
}

BOOL AgsmZoning::SendStartZoning(AgpdCharacter *pcsCharacter, AgsdServer *pcsServer)
{
	if (!pcsCharacter || !pcsServer)
		return FALSE;

	INT16	nPacketLength;
	INT8	cOperation = AGSMZONING_PACKET_OPERATION_START_ZONING;
	INT32	lThisServerID = m_pcsAgsmMap->GetThisServer();

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE, 
											&cOperation,
											&pcsCharacter->m_lID,
											NULL,
											NULL,
											&lThisServerID,
											NULL);

	if (!pvPacket)
		return FALSE;

	if (!SendPacketSvr(pvPacket, nPacketLength, (INT16)pcsServer->m_dpnidServer))
	{
//		TRACEFILE("AgsmZoning::SendCheckZoning() ���� SendPacketSvr() ����");

		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	// OnIdle Event�� �ִ�� ������ ��ٸ��� �ð��� ������ ���´�.
	//
	//

	return TRUE;
}

BOOL AgsmZoning::SendStartZoningResult(INT32 lCID, INT32 lServerID)
{
	if (!lCID || !lServerID)
		return FALSE;

	AgsdServer	*pcsServer = m_pAgsmServerManager->GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	INT16	nPacketLength;
	INT8	cOperation = AGSMZONING_PACKET_OPERATION_START_ZONING_RESULT;
	INT32	lThisServerID = m_pcsAgsmMap->GetThisServer();
	INT8	cStatus = (INT8) GetServerStatus();

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE, 
											&cOperation,
											&lCID,
											&cStatus,
											NULL,
											&lThisServerID,
											NULL);

	if (!pvPacket)
		return FALSE;

	if (!SendPacketSvr(pvPacket, nPacketLength, (INT16)pcsServer->m_dpnidServer))
	{
//		TRACEFILE("AgsmZoning::SendCheckZoning() ���� SendPacketSvr() ����");

		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmZoning::SendZoneServerAddr(INT32 lCID, INT32 lServerID)
{
	if (!lCID || !lServerID)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);
	if (!pcsCharacter)
		return FALSE;

	return SendZoneServerAddr(pcsCharacter, lServerID);
}

BOOL AgsmZoning::SendZoneServerAddr(AgpdCharacter *pcsCharacter, INT32 lServerID)
{
	if (!pcsCharacter || !lServerID)
		return FALSE;

	AgsdServer *pcsServer = m_pAgsmServerManager->GetServer(lServerID);
	if (!pcsServer)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	INT16	nPacketLength;
	INT8	cOperation		= AGSMZONING_PACKET_OPERATION_SEND_ZONESERVER_ADDR;
	INT32	lThisServerID = m_pcsAgsmMap->GetThisServer();

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE,
											&cOperation,
											&pcsCharacter->m_lID,
											NULL,
											pcsServer->m_szIP,
											&lThisServerID,
											NULL);
	/*
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE,
											&cOperation,
											&pcsCharacter->m_lID,
											NULL,
											((AgsdServerTemplate *)pcsServer->m_pcsTemplate)->m_szIPv4Addr,
											&lThisServerID,
											NULL);
	*/

	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter))
	{
//		TRACEFILE("AgsmZoning::SendZoneServerAddr() failed SendPacket()");

		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmZoning::SendZoningFail(INT32 lCharacterID, INT32 lServerID)
{
	if (!lCharacterID || !lServerID)
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCharacterID);

	return SendZoningFail(pcsCharacter, lServerID);
}

BOOL AgsmZoning::SendZoningFail(AgpdCharacter *pcsCharacter, INT32 lServerID)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdServer	*pcsServer = NULL;
	if (lServerID != AP_INVALID_SERVERID)
	{
		pcsServer = m_pAgsmServerManager->GetServer(lServerID);
		if (!pcsServer)
			return FALSE;
	}

	INT16	nPacketLength;
	INT8	cOperation		= AGSMZONING_PACKET_OPERATION_ZONING_FAIL;
	INT32	lThisServerID = m_pcsAgsmMap->GetThisServer();

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE,
											&cOperation,
											&pcsCharacter->m_lID,
											NULL,
											NULL,
											&lThisServerID,
											NULL);

	if (!pvPacket)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	// lServerID �� AP_INVALID_SERVERID �� �ƴϸ� lServerID�� ���� ������ ������ �׷��� �ʴٸ�
	// pcsCharacter�� ������.
	if (lServerID != AP_INVALID_SERVERID)
	{
		if (!SendPacketSvr(pvPacket, nPacketLength, (INT16)pcsServer->m_dpnidServer))
		{
//			TRACEFILE("AgsmCharacter::SendZoneServerAddr() failed SendPacketSvr()");

			m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}
	}
	else if (lServerID == AP_INVALID_SERVERID || pcsAgsdCharacter->m_ulServerID == m_pcsAgsmMap->GetThisServer())
	{
		if (!SendPacketSvr(pvPacket, nPacketLength, (INT16)pcsAgsdCharacter->m_dpnidCharacter))
		{
//			TRACEFILE("AgsmCharacter::SendZoneServerAddr() failed SendPacketSvr()");

			m_csPacket.FreePacket(pvPacket);

			return FALSE;
		}
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmZoning::PassControl(AgpdCharacter *pCharacter, AgsdServer *pcsServer)
{
	if (!pCharacter || !pcsServer)
	{
		return FALSE;
	}

	AgsdZoning	*pcsAgsdZoning = GetADCharacter(pCharacter);

	int i = 0;
	for (i = 0; i < 5; i++)
	{
		if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == pcsServer->m_lServerID)
		{
			if (!pcsAgsdZoning->m_stZoningStatus[i].bConnectZoneServer)
			{
				// �ڷ���Ʈ�� ����̴�. ���Ӻ��� �����Ѵ�.
				pcsAgsdZoning->m_stZoningStatus[i].bTeleport = TRUE;

				return SendStartZoning(pCharacter, pcsServer);
			}

			break;
		}
	}
	if (i == 5)
	{
		for (i = 0; i < 5; i++)
		{
			if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == AP_INVALID_SERVERID)
				break;
		}

		if (i == 5)
		{
			return FALSE;		// �� ������ ����.
		}

		pcsAgsdZoning->m_stZoningStatus[i].lServerID = pcsServer->m_lServerID;
		// �ڷ���Ʈ�� ����̴�. ���Ӻ��� �����Ѵ�.
		pcsAgsdZoning->m_stZoningStatus[i].bTeleport = TRUE;

		return SendStartZoning(pCharacter, pcsServer);
	}

	INT32	lThisServerID = m_pcsAgsmMap->GetThisServer();

	AgsdCharacter	*pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pCharacter);
	// �� ������ ��Ʈ�� ������ �ƴ϶�� �ѱ� ��Ʈ���� �����Ƿ� �� FALSE�� �����Ѵ�.
	if (pcsAgsdCharacter->m_ulServerID != lThisServerID)
	{
		return FALSE;
	}

	// ���� ����� ������ ��Ʈ���� �ѱ��.
	INT8	cOperation = AGSMZONING_PACKET_OPERATION_PASSCONTROL;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE, 
											&cOperation,
											&pCharacter->m_lID,
											NULL,
											NULL,
											&lThisServerID,
											NULL);

	if (!pvPacket)
	{
		return FALSE;
	}

	if (!SendPacketSvr(pvPacket, nPacketLength, (INT16)pcsServer->m_dpnidServer))
	{
//		TRACEFILE("AgsmZoning : PassControl() ���� SendPacketSvr() ����");

		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	// ��� ������ �Ϸ�Ǹ� control server �Ƶڸ� ���� �Ѵ�.
	//pcsAgsdCharacter->m_ulServerID = pcsServer->m_lServerID;

	pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus = AGSMZONING_ZONING_PASSCONTROL;

	return TRUE;
}

BOOL AgsmZoning::EndZoning(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	INT32	lThisServerID =  m_pcsAgsmMap->GetThisServer();
	if (pcsAgsdCharacter->m_ulServerID != lThisServerID)
		return FALSE;

	INT8	cOperation = AGSMZONING_PACKET_OPERATION_END_ZONING;
	INT16	nPacketLength;

	AgsdZoning *pcsAgsdZoning = GetADCharacter(pcsCharacter);

	// ���� ���� �������� ��� ������ ���� ���� �˸���.
	for (int i = 0; i < 5; i++)
	{
		if (pcsAgsdZoning->m_stZoningStatus[i].lServerID != AP_INVALID_SERVERID)
		{
			AgsdServer	*pcsServer = m_pAgsmServerManager->GetServer(pcsAgsdZoning->m_stZoningStatus[i].lServerID);
			if (!pcsServer)
				continue;

			PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE, 
													&cOperation,
													&pcsCharacter->m_lID,
													NULL,
													NULL,
													&lThisServerID,
													NULL);

			if (!pvPacket)
				continue;

			if (!SendPacketSvr(pvPacket, nPacketLength, (INT16)pcsServer->m_dpnidServer))
			{
//				TRACEFILE("AgsmZoning : EndZoning() ���� ������ ��� ����. SendPacketSvr() ����!!");
			}

			m_csPacket.FreePacket(pvPacket);
		}
	}

	//ZeroMemory(pcsAgsdZoning->m_stZoningStatus, sizeof(stZoningStatus) * 5);
	pcsAgsdZoning->m_stZoningStatus.MemSetAll();
	pcsAgsdZoning->m_bZoning = FALSE;
	pcsAgsdZoning->m_bDisconnectZoning = FALSE;

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	// ĳ���Ϳ��� ������ �����ٰ� �˷��ش�.
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE, 
											&cOperation,
											&pcsCharacter->m_lID,
											NULL,
											NULL,
											&lThisServerID,
											NULL);

	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter))
	{
//		TRACEFILE("AgsmZoning : EndZoning() ���� ĳ���Ϳ��� ����Ÿ ���� ���� ");

		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmZoning::SendPassControl(INT32 lCID, INT32 lServerID)
{
	if (!lCID || !lServerID)
		return FALSE;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacter(lCID);
	AgsdServer		*pcsServer		= m_pAgsmServerManager->GetServer(lServerID);

	if (!pcsCharacter || !pcsServer)
		return FALSE;

	INT16	nPacketLength;
	INT8	cOperation		= AGSMZONING_PACKET_OPERATION_RECV_PASSCONTROL;
	INT32	lThisServerID = m_pcsAgsmMap->GetThisServer();

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMZONING_PACKET_TYPE,
											&cOperation,
											&pcsCharacter->m_lID,
											NULL,
											NULL,
											&lThisServerID,
											NULL);

	if (!pvPacket)
		return FALSE;

	BOOL	bSend = TRUE;
	if (!SendPacketSvr(pvPacket, nPacketLength, (INT16)pcsServer->m_dpnidServer))
	{
		bSend = FALSE;
//		TRACEFILE("AgsmZoning::SendPassControlOK() failed SendPacketSvr()");
	}

	m_csPacket.FreePacket(pvPacket);

	//if (m_pAgsmServerManager->GetGameServersDPNID() == 0)
	//	return FALSE;

	pvPacket = m_pcsAgsmCharacter->MakePacketAgsdCharacter(pcsCharacter, &nPacketLength);

	if (!pvPacket)
		return FALSE;

	bSend	= m_pAgsmServerManager->SendPacketGameServers(pvPacket, nPacketLength, PACKET_PRIORITY_1);

	/*
	if (!m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, m_pAgsmServerManager->GetGameServersDPNID(), PACKET_PRIORITY_1))
	{
		bSend = FALSE;
	}
	*/

	m_csPacket.FreePacket(pvPacket);

	return bSend;
}

BOOL AgsmZoning::SendConnectResult(AgpdCharacter *pCharacter, BOOL bResult)
{
	if (!pCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return FALSE;

	INT16	nPacketLength;
	INT8	cOperation		= AGSMZONING_PACKET_OPERATION_RESULT_CONNECT_ZONESERVER;
	INT8	cStatus			= bResult;
	INT32	lThisServerID = m_pcsAgsmMap->GetThisServer();
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARACTER_PACKET_TYPE, 
											&cOperation,
											&pCharacter->m_lID,
											&cStatus,
											NULL,
											&lThisServerID,
											NULL);

	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter))
	{
//		TRACEFILE("AgsmZoning::SendConnectResult() failed SendPacket()");

		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmZoning::AllServerAdd(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	//if (m_pAgsmServerManager->GetGameServersDPNID() == 0)
	//	return FALSE;

	PVOID	pvPacketFactor = m_pcsAgpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor);
	if (!pvPacketFactor)
		return FALSE;

	INT16	nPacketLength;
	PVOID	pvPacket = m_pcsAgsmCharacter->MakePacketAddCharacter(pcsCharacter, &nPacketLength, pvPacketFactor);

	m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket || !nPacketLength)
		return FALSE;

	m_pAgsmServerManager->SendPacketGameServers(pvPacket, nPacketLength, PACKET_PRIORITY_1);
	//m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, m_pAgsmServerManager->GetGameServersDPNID(), PACKET_PRIORITY_1);

	m_csPacket.FreePacket(pvPacket);

	pvPacket = m_pcsAgsmCharacter->MakePacketAgsdCharacterLogin(pcsCharacter, &nPacketLength);
	if (!pvPacket || !nPacketLength)
		return FALSE;

	m_pAgsmServerManager->SendPacketGameServers(pvPacket, nPacketLength, PACKET_PRIORITY_1);
	//m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nPacketLength, m_pAgsmServerManager->GetGameServersDPNID(), PACKET_PRIORITY_1);

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmZoning::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	INT32	lServerID			= 0;
	INT8	cOperation			= (-1);
	INT32	lCharacterID		= 0;
	INT8	cStatus				= (-1);
	CHAR*	szAddress			= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCharacterID,
						&cStatus,
						&szAddress,
						&lServerID);

	switch (cOperation) {
	case AGSMZONING_PACKET_OPERATION_START_ZONING:
		{
			if (!lCharacterID)
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (23) !!!\n");

				SendZoningFail(lCharacterID, lServerID);

				return FALSE; 
			}

			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (24) !!!\n");

				SendZoningFail(lCharacterID, lServerID);

				return FALSE;
			}

			AgsdServer *pcsServer = m_pAgsmServerManager->GetServer(lServerID);
			if (!pcsServer)
			{
				SendZoningFail(pcsCharacter, lServerID);

				pcsCharacter->m_Mutex.Release();

				return FALSE;
			}

			AgsdZoning *pcsAgsdZoning = GetADCharacter(pcsCharacter);
			int i = 0;
			for (i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == AP_INVALID_SERVERID)
				{
					pcsAgsdZoning->m_stZoningStatus[i].lServerID = lServerID;
					pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus = AGSMZONING_ZONING_SUCCESS_PASSCONTROL;
					pcsAgsdZoning->m_bZoning = TRUE;

					break;
				}
			}

			if (i == 5)
			{
				SendZoningFail(pcsCharacter, lServerID);

				pcsCharacter->m_Mutex.Release();

				return FALSE;
			}

//			TRACEFILE("Receive Start Zoning Packet");

			SendStartZoningResult(lCharacterID, lServerID);

			m_pcsAgsmCharacter->EnterGameWorld(pcsCharacter, TRUE);

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMZONING_PACKET_OPERATION_START_ZONING_RESULT:
		{
			if (!lCharacterID || cStatus == (-1))
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (21) !!!\n");

				SendZoningFail(lCharacterID, lServerID);

				return FALSE;
			}

			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				SendZoningFail(lCharacterID, lServerID);

				return FALSE;
			}

			AgsdZoning *pcsAgsdZoning = GetADCharacter(pcsCharacter);
			int i = 0;
			for (i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == lServerID)
				{
					//pcsAgsdZoning->m_stZoningStatus[i].lServerID = lServerID;
					pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus = AGSMZONING_ZONING_PROCESS_ZONING;

					break;
				}
			}

			if (i == 5)
			{
				SendZoningFail(pcsCharacter, lServerID);

				pcsCharacter->m_Mutex.Release();

				return FALSE;
			}

//			TRACEFILE("Receive Start Zoning Result Packet");

			SendZoneServerAddr(pcsCharacter, lServerID);

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMZONING_PACKET_OPERATION_PASSCONTROL:
		{
			if (!lCharacterID)
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (3) !!!\n");

				SendZoningFail(lCharacterID, lServerID);

				return FALSE;
			}

			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (4) !!\n");

				SendZoningFail(lCharacterID, lServerID);

				return FALSE;
			}

			AgsdZoning *pcsAgsdZoning = GetADCharacter(pcsCharacter);
			int i = 0;
			for (i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == lServerID)
				{
					pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus = AGSMZONING_ZONING_PROCESS_ZONING;

					break;
				}
			}

			if (i == 5)
			{
				SendZoningFail(pcsCharacter, lServerID);

				pcsCharacter->m_Mutex.Release();

				return FALSE;
			}

			AgsdCharacter	*pcsAgsdCharacter	= m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
			pcsAgsdCharacter->m_ulServerID = m_pcsAgsmMap->GetThisServer();

//			TRACEFILE("Receive PassControl Packet");

			SendPassControl(lCharacterID, lServerID);

			//SendPacketCharSync(pcsCharacter, &pcsCharacter->m_stPos, pcsAgsdCharacter->m_dpnidCharacter);

			ApWorldSector *pcsSector = m_pcsApmMap->GetSector(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);
			if (pcsSector)
			{
				INT32 lControlServer = m_pcsAgsmMap->GetControlServer(pcsSector);

				if (lControlServer == m_pcsAgsmMap->GetThisServer() && !m_pcsAgsmMap->IsNeedZoning(pcsSector))
					EndZoning(pcsCharacter);
			}

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMZONING_PACKET_OPERATION_RECV_PASSCONTROL:
		{
			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (11)\n");

				SendZoningFail(lCharacterID, lServerID);

				return FALSE;
			}

			AgsdServer *pcsServer = m_pAgsmServerManager->GetServer(lServerID);
			if (!pcsServer)
			{
				SendZoningFail(pcsCharacter, lServerID);
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			AgsdZoning *pcsAgsdZoning = GetADCharacter(pcsCharacter);
			int i = 0;
			for (i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == lServerID)
				{
					if (pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus != AGSMZONING_ZONING_PASSCONTROL)
					{
						SendZoningFail(pcsCharacter, lServerID);
						pcsCharacter->m_Mutex.Release();
						return FALSE;
					}

					pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus = AGSMZONING_ZONING_SUCCESS_PASSCONTROL;

					break;
				}
			}

			if (i == 5)
			{
				SendZoningFail(pcsCharacter, lServerID);
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

//			TRACEFILE("Receive PassControl Result Packet");

			EnumCallback(AGSMZONING_CB_ZONING_PASS_CONTROL, pcsCharacter, pcsServer);

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMZONING_PACKET_OPERATION_END_ZONING:
		{
			if (!lCharacterID)
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (5) !!\n");
				return FALSE;
			}

			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				OutputDebugString("AgsmZoning::OnReceive() Error (6) !!\n");
				return FALSE;
			}

			AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter)
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			AgsdZoning	*pcsAgsdZoning	= GetADCharacter(pcsCharacter);

			pcsAgsdZoning->m_bDisconnectZoning = TRUE;			// �� ĳ���� ����Ÿ�� �����϶�� ���Ѵ�.

			if (pcsAgsdCharacter->m_dpnidCharacter != 0)
			{
				if (!m_pcsAgsmCharacter->DestroyClient(pcsAgsdCharacter->m_dpnidCharacter))
				{
					//if (m_pcsAgpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
					//	AuLogFile("RemoveNPC.log", "Removed by AGSMZONING_PACKET_OPERATION_END_ZONING\n");

//					TRACEFILE("AgsmZoning : DestroyClient() error");
					pcsCharacter->m_Mutex.Release();

					m_pcsAgpmCharacter->RemoveCharacter(lCharacterID);
					return FALSE;
				}
			}

			int i = 0;
			for (i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == lServerID)
				{
					pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus = AGSMZONING_NOT_ZONING;
					pcsAgsdZoning->m_stZoningStatus[i].lServerID = AP_INVALID_SERVERID;
					pcsAgsdZoning->m_stZoningStatus[i].bConnectZoneServer = FALSE;
					pcsAgsdZoning->m_bZoning = FALSE;
 
					break;
				}
			}

			if (i == 5)
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

//			TRACEFILE("Receive End Zoning Packet");

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMZONING_PACKET_OPERATION_CONNECT_ZONESERVER:
		{
			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				return FALSE;
			}

			AgsdCharacter	*pcsAgsdCharacter	= m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
			AgsdZoning		*pcsAgsdZoning		= GetADCharacter(pcsCharacter);

			/*
			if (!m_pagsmAccountManager->LockAdminAccount())
			{
				return FALSE;
			}

			AgsdAccount *pcsAccount = m_pagsmAccountManager->GetAccount(pcsAgsdCharacter->m_szAccountID);
			if (!pcsAccount)
			{
				WriteLog(AS_LOG_DEBUG, "failed GetAccount()");
				DestroyClient(pcsAgsdCharacter->m_dpnidCharacter);
				//m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter);

				m_pagsmAccountManager->ReleaseAdminAccount();

				return FALSE;
			}

			if (!pcsAccount->m_Mutex.WLock())
			{
				m_pagsmAccountManager->ReleaseAdminAccount();

				return FALSE;
			}

			if (!m_pagsmAccountManager->ReleaseAdminAccount())
			{
				pcsAccount->m_Mutex.Release();

				return FALSE;
			}
			*/

			pcsAgsdCharacter->m_dpnidCharacter = ulNID;
			GetConnectionInfo(pcsAgsdCharacter->m_dpnidCharacter, &pcsAgsdCharacter->m_ulRoundTripLatencyMS, NULL);

			//SetAccountNameToPlayerContext(pcsAccount->m_szName, ulNID);
			SetIDToPlayerContext(lCharacterID, ulNID);

			for (int i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID != AP_INVALID_SERVERID &&
					pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus == AGSMZONING_ZONING_SUCCESS_PASSCONTROL)
					pcsAgsdZoning->m_stZoningStatus[i].bConnectZoneServer = TRUE;
			}

//			TRACEFILE("Receive Connect ZoneServer Packet");

			SendConnectResult(pcsCharacter, TRUE);

			pcsCharacter->m_Mutex.Release();
			//pcsAccount->m_Mutex.Release();
		}
		break;

	case AGSMZONING_PACKET_OPERATION_RESULT_CONNECT_ZONESERVER:
		{
			if (cStatus == FALSE)	// ���ῡ �����ߴ�. 
			{
				SendZoningFail(lCharacterID, lServerID);

				return FALSE;
			}

			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				return FALSE;
			}

			AgsdZoning *pcsAgsdZoning = GetADCharacter(pcsCharacter);

			AgsdServer *pcsServer = m_pAgsmServerManager->GetServer(lServerID);
			if (!pcsServer)
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			for (int i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == pcsServer->m_lServerID)
				{
					pcsAgsdZoning->m_stZoningStatus[i].bConnectZoneServer = TRUE;

					if (pcsAgsdZoning->m_stZoningStatus[i].bTeleport)
					{
						PassControl(pcsCharacter, pcsServer);
						pcsAgsdZoning->m_stZoningStatus[i].bTeleport = FALSE;
					}
					else
					{
						EnumCallback(AGSMZONING_CB_ZONING_START, pcsCharacter, pcsServer);
					}

					break;
				}
			}

//			TRACEFILE("Receive Connect ZoneServer Result Packet");

			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMZONING_PACKET_OPERATION_ZONING_FAIL:
		{
			if (!lCharacterID)
				return FALSE;

			// ���� �ϰ� �ִ� ���� ���� ���Ÿ����� ����Ѵ�. ���� ���� ����~~~~

			AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCharacterID);
			if (!pcsCharacter)
			{
				return FALSE;
			}

			AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
			AgsdZoning *pcsAgsdZoning = GetADCharacter(pcsCharacter);

			// lServerID�� ����� ���� ����Ÿ�� �ʱ�ȭ�Ѵ�.
			BOOL	bEndZoning = TRUE;
			for (int i = 0; i < 5; i++)
			{
				if (pcsAgsdZoning->m_stZoningStatus[i].lServerID == lServerID)
				{
					pcsAgsdZoning->m_stZoningStatus[i].bConnectZoneServer = FALSE;
					pcsAgsdZoning->m_stZoningStatus[i].bTeleport = FALSE;
					pcsAgsdZoning->m_stZoningStatus[i].fZoningStatus = AGSMZONING_NOT_ZONING;
					pcsAgsdZoning->m_stZoningStatus[i].lServerID = AP_INVALID_SERVERID;
				}
				else if (pcsAgsdZoning->m_stZoningStatus[i].lServerID != AP_INVALID_SERVERID)
				{
					bEndZoning = FALSE;
				}
			}
			
			if (bEndZoning)
				pcsAgsdZoning->m_bZoning = FALSE;

			if (pcsAgsdCharacter->m_ulServerID == m_pcsAgsmMap->GetThisServer())
			{
				SendZoningFail(pcsCharacter, AP_INVALID_SERVERID);
			}

			pcsCharacter->m_Mutex.Release();
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

/*
BOOL AgsmZoning::SyncAddChar(AgpdCharacter *pCharacter, ApWorldSector *pSector)
{
	if (!pCharacter || !pSector)
		return FALSE;

	AgsdServer	*pcsServer	= m_pAgsmServerManager->GetThisServer();

	//##### modified by raska, 2004.05.11
  #ifdef _AGSM_SERVER_MANAGER2_
	if (pcsServer->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
		return TRUE;
  #else
	if (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
		return FALSE;
  #endif

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pCharacter);

	for (int i = (int) pSector->GetIndexX() - 1; i <= (int) pSector->GetIndexX() + 1; i++)
	{
		for (int j = (int) pSector->GetIndexZ() - 1; j <= (int) pSector->GetIndexZ() + 1; j++)
		{
			ApWorldSector *pAroundSector = m_pcsAgsmCharacter->GetSectorBySectorIndex(i , 0, j);
			if (!pAroundSector)
				continue;

			m_pcsAgsmCharacter->SendCharToSector(pCharacter, pAroundSector, TRUE);
			m_pcsAgsmCharacter->SendSectorToChar(pAroundSector, pCharacter);

			//ProcessZoning(pCharacter, pAroundSector);
		}
	}

	EnumCallback(AGSMZONING_CB_SYNCADDCHAR, pCharacter, pSector);

	if (pcsAgsdCharacter->m_dpnidCharacter != 0)
		SendPacketCharSync(pCharacter, m_pAgsmServerManager->GetGameServersDPNID());

	return TRUE;
}

BOOL AgsmZoning::SyncRemoveChar(AgpdCharacter *pCharacter, ApWorldSector *pSector)
{
	if (!pCharacter || !pSector)
		return FALSE;

	INT32	lThisServer = m_pcsAgsmMap->GetThisServer();

	AgsdCharacter	*pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pCharacter);

	if (pcsAgsdCharacter->m_ulServerID != lThisServer)
	{
		EndZoning(pCharacter);
	}

	// �ڽŰ� �ֺ��� �ִ� 8���� ���͸� ������ ���ʷ� ���ͺ��� ����Ÿ�� �����Ѵ�.
	for (int i = (int) pSector->GetIndexX() - 1; i <= (int) pSector->GetIndexX() + 1; i++)
	{
		for (int j = (int) pSector->GetIndexZ() - 1; j <= (int) pSector->GetIndexZ() + 1; j++)
		{
			m_pcsAgsmCharacter->SendCharRemoveToSector(pCharacter, m_pcsAgsmCharacter->GetSectorBySectorIndex(i, 0, j));
		}
	}

	SendPacketCharRemoveSync(pCharacter);

	return TRUE;
}
*/

BOOL AgsmZoning::SyncAddChar(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell)
{
	if (!pCharacter || !pcsCell)
		return FALSE;

	AgsdServer	*pcsServer	= m_pAgsmServerManager->GetThisServer();

	if (pcsServer->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
		return TRUE;

	//if (((AgsdServerTemplate *) pcsServer->m_pcsTemplate)->m_cType != AGSMSERVER_TYPE_GAME_SERVER)
	//	return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pCharacter);

	INT32	lRegionIndex	= pCharacter->m_nBindingRegionIndex;

	if(lRegionIndex < 0)
		lRegionIndex = m_pcsApmMap->GetRegion(pCharacter->m_stPos.x, pCharacter->m_stPos.z);

	// 2006.07.05. steeple
	// ��Ŷ �������� ������ �� ������ ���ϴ� �Լ��� �������.
	INT32	lNearCellCount = m_pcsAgsmAOIFilter->GetNearCellCountByRegion(lRegionIndex);

	for (int i = (int) pcsCell->GetIndexX() - lNearCellCount; i <= (int) pcsCell->GetIndexX() + lNearCellCount; i++)
	{
		for (int j = (int) pcsCell->GetIndexZ() - lNearCellCount; j <= (int) pcsCell->GetIndexZ() + lNearCellCount; j++)
		{
			AgsmAOICell *pcsAroundCell = m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			m_pcsAgsmCharacter->SendCharToCell(pCharacter, pcsAroundCell, (pcsAgsdCharacter->m_bIsNotLogout) ? FALSE : TRUE);
			m_pcsAgsmCharacter->SendCellToChar(pcsAroundCell, pCharacter);

			//ProcessZoning(pCharacter, pAroundSector);
		}
	}

	for (int i = (int) pcsCell->GetIndexX() - lNearCellCount * 2; i <= (int) pcsCell->GetIndexX() + lNearCellCount * 2; i++)
	{
		for (int j = (int) pcsCell->GetIndexZ() - lNearCellCount * 2; j <= (int) pcsCell->GetIndexZ() + lNearCellCount * 2; j++)
		{
			AgsmAOICell *pcsAroundCell = m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(i, j);
			if (!pcsAroundCell)
				continue;

			m_pcsAgsmCharacter->SendCellNPCToChar(pcsAroundCell, pCharacter);
		}
	}

	EnumCallback(AGSMZONING_CB_SYNCADDCHAR, pCharacter, pcsCell);

	/*
	if (pcsAgsdCharacter->m_dpnidCharacter != 0)
		SendPacketCharSync(pCharacter, m_pAgsmServerManager->GetGameServersDPNID());
	*/

	return TRUE;
}

BOOL AgsmZoning::SyncRemoveChar(AgpdCharacter *pCharacter, AgsmAOICell *pcsCell)
{
	if (!pCharacter || !pcsCell)
		return FALSE;

	//INT32	lThisServer = m_pcsAgsmMap->GetThisServer();

	AgsdCharacter	*pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pCharacter);

	//if (pcsAgsdCharacter->m_ulServerID != lThisServer)
	//{
		//EndZoning(pCharacter);
	//}

	INT32	lRegionIndex	= pCharacter->m_nBindingRegionIndex;

	if(lRegionIndex < 0)
		lRegionIndex = m_pcsApmMap->GetRegion(pCharacter->m_stPos.x, pCharacter->m_stPos.z);

	// 2006.07.05. steeple
	// ��Ŷ �������� ������ �� ������ ���ϴ� �Լ��� �������.
	INT32	lNearCellCount = m_pcsAgsmAOIFilter->GetNearCellCountByRegion(lRegionIndex);

	for (int i = (int) pcsCell->GetIndexX() - lNearCellCount; i <= (int) pcsCell->GetIndexX() + lNearCellCount; i++)
	{
		for (int j = (int) pcsCell->GetIndexZ() - lNearCellCount; j <= (int) pcsCell->GetIndexZ() + lNearCellCount; j++)
		{
			m_pcsAgsmCharacter->SendCharRemoveToCell(pCharacter, m_pcsAgsmAOIFilter->m_csAOIMap.GetCell(i, j), (pcsAgsdCharacter->m_bIsNotLogout) ? FALSE : TRUE);
		}
	}

	//SendPacketCharRemoveSync(pCharacter);

	return TRUE;
}

//		SendPacketCharSync
//	Functions
//		- send character status & position etc... (������ ��ũ ���⶧ ����Ѵ�.)
//	Arguments
//		- pcsCharacter : character data pointer
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmZoning::SendPacketCharSync(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_ulServerID != m_pcsAgsmMap->GetThisServer())
		return FALSE;

	INT16	nSize;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;
	INT8	cMoveFlag;

	//if (pcsAgsdCharacter->m_bZoning)
	//{
		cMoveFlag = AGPMCHARACTER_MOVE_FLAG_STOP | AGPMCHARACTER_MOVE_FLAG_SYNC;
	//}
	//else
	//{
	//	cMoveFlag = (pcsCharacter->m_bPathFinding ? AGPMCHARACTER_MOVE_FLAG_PATHFINDING : 0) |
	//				(pcsCharacter->m_bMove ? 0 : AGPMCHARACTER_MOVE_FLAG_STOP) |
	//				(pcsCharacter->m_bSync ? AGPMCHARACTER_MOVE_FLAG_SYNC : 0);
	//}

	PVOID pvPacketMove = m_pcsAgpmCharacter->m_csPacketMove.MakePacket(FALSE, &nSize, 0, 
														&pcsCharacter->m_stPos,
														&pcsCharacter->m_stPos,
														NULL,
														NULL,
														&pcsCharacter->m_fTurnX,
														&pcsCharacter->m_fTurnY,
														&cMoveFlag,
														NULL);
	if (!pvPacketMove)
		return FALSE;

	PVOID pvPacketFactor = m_pcsAgpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor);

	INT8	cIsTransform	= 1;

	INT8	cFaceIndex	= (INT8)	pcsCharacter->m_lFaceIndex;
	INT8	cHairIndex	= (INT8)	pcsCharacter->m_lHairIndex;

	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
														&cOperation,															// Operation
														&pcsCharacter->m_lID,													// Character ID
														NULL,																	// Character Template ID
														NULL,																	// Game ID
														&pcsCharacter->m_unCurrentStatus,										// Character Status
														pvPacketMove,															// Move Packet
														NULL,																	// Action Packet
														pvPacketFactor,															// Factor Packet
														NULL,																	// llMoney
														NULL,																	// bank money
														NULL,																	// cash
														&pcsCharacter->m_unActionStatus,										// character action status
														&pcsCharacter->m_unCriminalStatus,										// character criminal status
														NULL,																	// attacker id (������� ������ �ʿ�)
														NULL,																	// ���� �����Ǽ� �ʿ� �������� ����
														NULL,																	// region index
														NULL,																	// social action index
														&pcsCharacter->m_ulSpecialStatus,										// special status
														(pcsCharacter->m_bIsTrasform) ? &cIsTransform : NULL,					// is transform status
														pcsCharacter->m_szSkillInit,											// skill initialization text
														&cFaceIndex,															// face index
														&cHairIndex,															// hair index
														NULL,																	// Option Flag
														NULL,																	// bank size
														&pcsCharacter->m_unEventStatusFlag,										// event status flag
														NULL,																	// remained criminal status time
														NULL,																	// remained murderer point time
														pcsCharacter->m_szNickName,												// nick name
														NULL,																	// gameguard
														NULL																	// last killed time in battlesquare
														);

	m_pcsAgpmCharacter->m_csPacketMove.FreePacket(pvPacketMove);
	m_pcsAgpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	if (!pvPacket)
		return FALSE;

	if (!m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nSize, ulNID, PACKET_PRIORITY_1))
	{
		OutputDebugString("AgsmZoning::SendPacketCharSync() Error(1) !!!\n");
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//		SendPacketCharRemoveSync
//	Functions
//		- send character remove packet (������ ��ũ ���⶧ ����Ѵ�.)
//	Arguments
//		- pcsCharacter : character data pointer
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmZoning::SendPacketCharRemoveSync(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_ulServerID != m_pcsAgsmMap->GetThisServer())
		return FALSE;

	//UINT32 ulNID = m_pAgsmServerManager->GetGameServersDPNID();
	//if (ulNID == 0)
	//	return FALSE;

	INT16	nSize;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_REMOVE;

	PVOID pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE, 
														&cOperation,						// Operation
														&pcsCharacter->m_lID,				// Character ID
														NULL,								// Character Template ID
														NULL,								// Game ID
														NULL,								// Character Status
														NULL,								// Move Packet
														NULL,								// Action Packet
														NULL,								// Factor Packet
														NULL,								// llMoney
														NULL,								// bank money
														NULL,								// cash
														NULL,								// character action status
														NULL,								// character criminal status
														NULL,								// attacker id (������� ������ �ʿ�)
														NULL,								// ���� �����Ǽ� �ʿ� �������� ����
														NULL,								// region index
														NULL,								// social action index
														NULL,								// special status
														NULL,								// is transform status
														NULL,								// skill initialization text
														NULL,								// face index
														NULL,								// hair index
														NULL,								// Option Flag
														NULL,								// bank size
														NULL,								// event status flag
														NULL,								// remained criminal status time
														NULL,								// remained murderer point time
														NULL,								// nick name
														NULL,								// gameguard
														NULL								// last killed time in battlesquare
														);

	if (!pvPacket)
		return FALSE;

	m_pAgsmServerManager->SendPacketGameServers(pvPacket, nSize, PACKET_PRIORITY_1);

	/*
	if (!m_pcsAgsmAOIFilter->SendPacketGroup(pvPacket, nSize, ulNID, PACKET_PRIORITY_1))
	{
		OutputDebugString("AgsmZoning::SendPacketCharRemoveSync() Error(1) !!!\n");
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}
	*/

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}
