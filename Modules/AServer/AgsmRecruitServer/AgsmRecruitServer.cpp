#include <string.h>
#include <stdio.h>
#include "AgsmRecruitServer.h"

AgsmRecruitServer::AgsmRecruitServer()
{
	SetModuleName( "AgsmRecruitServer" );

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGSMRECRUIT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  ����
							AUTYPE_INT32,			1, // lLFPID  Recruit�� �ø��� ID
							AUTYPE_PACKET,			1, // LFP�� ���� ��������.
							AUTYPE_PACKET,			1, // LFM�� ���� ��������.
							AUTYPE_INT32,			1, // lResult
		                    AUTYPE_END,				0
							);

	m_csLFPInfo.SetFlagLength( sizeof(INT8) );
	m_csLFPInfo.SetFieldType(
								AUTYPE_INT32,		1, //TotalCount
		                        AUTYPE_INT32,		1, //lIndex
		                        AUTYPE_INT32,		1, //LV
		                        AUTYPE_INT32,		1, //Class
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1, // pstrPlayerID;
								AUTYPE_INT32,		1, // lPlayerID
		                        AUTYPE_END,			0
							);

	m_csLFMInfo.SetFlagLength( sizeof(INT16) );
	m_csLFMInfo.SetFieldType(
								AUTYPE_INT32,		1, //lTotalCount
		                        AUTYPE_INT32,		1, //lIndex
		                        AUTYPE_CHAR,		20, //Purpose
		                        AUTYPE_INT32,		1, //lRequire Member
		                        AUTYPE_INT32,		1, //Min LV
		                        AUTYPE_INT32,		1, //Max LV
		                        AUTYPE_INT32,		1, //LV
		                        AUTYPE_INT32,		1, //Class
								AUTYPE_CHAR,		AGPACHARACTER_MAX_ID_STRING + 1, // pstrLeaderID;
								AUTYPE_INT32,		1, //lLeaderID
		                        AUTYPE_END,			0
							);

	m_csSearchInfo.SetFlagLength(sizeof(INT8));
	m_csSearchInfo.SetFieldType(
								 AUTYPE_INT8,		1,  //�� ������ �������� ã�´�?
								 AUTYPE_INT32,		1,  //Level��?
								 AUTYPE_INT32,		1,  //���° ������ �ΰ�?
								 AUTYPE_INT32,		1,  //Class BitFlag
		                         AUTYPE_END,		0
								 );

}

AgsmRecruitServer::~AgsmRecruitServer()
{
}

//��𼭳� �� �� �ִ� OnAddModule()
BOOL AgsmRecruitServer::OnAddModule()
{
	m_csRegisterCID.InitializeObject( sizeof(INT32), MAX_RECRUIT_CONNECTION_COUNT );

	m_paAuOLEDBManager = (AuOLEDBManager *)GetModule( "AuOLEDBManager" );
	m_pagsmServerManager = (AgsmServerManager *)GetModule( "AgsmServerManager2" );

	if( !m_paAuOLEDBManager || !m_pagsmServerManager )
	{
		return FALSE;
	}

	return TRUE;
}

//GameServer(ControlServer)�� ���� �ִ��� Ȯ���ϰ� �� ������ �޾ƿ´�.
BOOL AgsmRecruitServer::SendRegisterLFP( INT32 lControlServerID, INT32 lLFPID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFP;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													NULL,				//LFPInfo
													NULL,				//LFMInfo
													NULL				//lResult
													);

	if( pvPacket != NULL )
	{
		AgsdServer			*pcsAgsdServer;

		//����� ��Ʈ�� ������ �Ӹ��մϴ�.
		pcsAgsdServer = m_pagsmServerManager->GetServer( lControlServerID );

		//��Ʈ�� ������ ������ ��������!! ���d�d~
		if( pcsAgsdServer != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;	
}

BOOL AgsmRecruitServer::SendRegisterLFM( INT32 lControlServerID, INT32 lLFPID, void *pvLFMInfo )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													NULL,				//LFPInfo
													pvLFMInfo,			//LFMInfo
													NULL				//lResult
													);

	if( pvPacket != NULL )
	{
		AgsdServer			*pcsAgsdServer;

		//����� ��Ʈ�� ������ �Ӹ��մϴ�.
		pcsAgsdServer = m_pagsmServerManager->GetServer( lControlServerID );

		//��Ʈ�� ������ ������ ��������!! ���d�d~
		if( pcsAgsdServer != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;	
}

BOOL AgsmRecruitServer::PreProcessRegisterLFP( INT32 lControlServerID, INT32 lLFPID )
{
	BOOL			bResult;
	void			*pvResult;

	//��ϵ� ������ Ȯ���Ѵ�.
	pvResult = m_csRegisterCID.GetObject( lLFPID );

	//����� �� ���̱�. �� ���? ������ش�.
	if( pvResult != NULL )
	{
	}
	//�ƴ϶�� ������ �ִ�. GameServer�� ������ ��û�Ѵ�.
	else
	{
		bResult = SendRegisterLFP( lControlServerID, lLFPID );
	}

	return bResult;
}

BOOL AgsmRecruitServer::PreProcessRegisterLFM( INT32 lControlServerID, INT32 lLFPID, void *pvLFMInfo )
{
	BOOL			bResult;
	void			*pvResult;

	//��ϵ� ������ Ȯ���Ѵ�.
	pvResult = m_csRegisterCID.GetObject( lLFPID );

	//����� �� ���̱�. �� ���? ������ش�.
	if( pvResult != NULL )
	{
	}
	//�ƴ϶�� ������ �ִ�. GameServer�� ������ ��û�Ѵ�.
	else
	{
		bResult = SendRegisterLFM( lControlServerID, lLFPID, pvLFMInfo );
	}

	return bResult;
}

//DB Insert �Ѵ�.
BOOL AgsmRecruitServer::ProcessRegisterLFP( INT32 lLFPID, void *pvLFPInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFPInfo != NULL )
	{
		char			*pstrCharName;

		INT32			lLevel;
		INT32			lClass;

		pstrCharName = NULL;

		m_csLFPInfo.GetField( FALSE, pvLFPInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								&lLevel,			//lLevel
								&lClass,			//lClass
								&pstrCharName,		//CharName
								NULL				//lPlaerID
								);

		if( pstrCharName != NULL )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			int				lCharNameLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_INSERT_LFP;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			pcQueryQueue->m_lLV = lLevel;

			//Ŭ���� ���ǰ� ��� �Ʒ��� ���� ����� ����!
			//�� ����� ��������� ���� ����߰���?
			if( lClass == 1 )
			{
				lClass = 1;
			}
			else if( lClass == 2 )
			{
				lClass = 2;
			}
			else if( lClass == 3 )
			{
				lClass = 4;
			}
			else if( lClass == 4 )
			{
				lClass = 8;
			}

			pcQueryQueue->m_lClass = lClass;

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

//DB Insert �Ѵ�.
BOOL AgsmRecruitServer::ProcessRegisterLFM( INT32 lLFMID, void *pvLFMInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFMInfo != NULL )
	{
		char			*pstrCharName;
		char			*pstrPurpose;

		INT32			lRequireMember;
		INT32			lMinLV;
		INT32			lMaxLV;
		INT32			lLevel;
		INT32			lClass;
		INT32			lLeaderID;

		pstrCharName = NULL;

		m_csLFMInfo.GetField( FALSE, pvLFMInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								&pstrPurpose,		//purpose
								&lRequireMember,	//lRequireMember
								&lMinLV,			//MinLV
								&lMaxLV,			//MaxLV
								&lLevel,			//Level
								&lClass,			//Class
								&pstrCharName,		//CharName
								&lLeaderID			//lLeaderID
								);

		if( (pstrCharName != NULL) && (pstrPurpose != NULL) )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			INT32				lCharNameLen;
			INT32				lPurposeLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;
			lPurposeLen = strlen( pstrPurpose ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_INSERT_LFM;
			pcQueryQueue->m_lCID = lLFMID;

			pcQueryQueue->m_pstrPurpose = new char[lPurposeLen];
			memset( pcQueryQueue->m_pstrPurpose, 0, lPurposeLen );
			strcat( pcQueryQueue->m_pstrPurpose, pstrPurpose );

			pcQueryQueue->m_lRequireMember = lRequireMember;
			pcQueryQueue->m_lMinLV = lMinLV;
			pcQueryQueue->m_lMaxLV = lMaxLV;
			pcQueryQueue->m_lLV = lLevel;

			//Ŭ���� ���ǰ� ��� �Ʒ��� ���� ����� ����!
			//�� ����� ��������� ���� ����߰���?
			if( lClass == 1 )
			{
				lClass = 1;
			}
			else if( lClass == 2 )
			{
				lClass = 2;
			}
			else if( lClass == 3 )
			{
				lClass = 4;
			}
			else if( lClass == 4 )
			{
				lClass = 8;
			}

			pcQueryQueue->m_lClass = lClass;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			pcQueryQueue->m_lLeaderID = lLeaderID;

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessCancelLFP( INT32 lLFPID, void *pvLFPInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFPInfo != NULL )
	{
		char			*pstrCharName;

		pstrCharName = NULL;

		m_csLFPInfo.GetField( FALSE, pvLFPInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								NULL,				//lLevel
								NULL,				//lClass
								&pstrCharName,		//CharName
								NULL				//lPlaerID
								);

		if( pstrCharName != NULL )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			INT32			lCharNameLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_DELETE_LFP;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessCancelLFM( INT32 lLFMID, void *pvLFMInfo, BOOL bCheckQueue )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvLFMInfo )
	{
		char			*pstrCharName;

		pstrCharName = NULL;

		m_csLFMInfo.GetField(FALSE, pvLFMInfo, 0,
									NULL,			//lTotalCount
									NULL,			//lIndex
									NULL,			//Purpose
									NULL,			//lRequireMemeber
									NULL,			//MinLV
									NULL,			//MaxLV
									NULL,			//LV
									NULL,			//Class
									&pstrCharName,	//CharName
									NULL			//lLeaderID
									);

		if( pstrCharName != NULL )
		{
			AgsmRecruitQueueInfo		*pcQueryQueue;

			INT32			lCharNameLen;

			pcQueryQueue = new AgsmRecruitQueueInfo;

			lCharNameLen = strlen( pstrCharName ) + 1;

			pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_DELETE_LFM;
			pcQueryQueue->m_lCID = lLFMID;

			pcQueryQueue->m_pstrCharName = new char[lCharNameLen];
			memset( pcQueryQueue->m_pstrCharName, 0, lCharNameLen );
			strcat( pcQueryQueue->m_pstrCharName, pstrCharName );

			if( bCheckQueue )
			{
				bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
			else
			{
				bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessSearchFromLFP( INT32 lLFPID, void *pvSearchInfo, BOOL bCheckQueue )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvSearchInfo != NULL )
	{
		AgsmRecruitQueueInfo		*pcQueryQueue;

		bool				bSearchByLevel;
		INT32				lLevel;
		INT32				lPage;
		INT32				lClass;

		m_csSearchInfo.GetField( FALSE, pvSearchInfo, 0,
								&bSearchByLevel,
								&lLevel,
								&lPage,
								&lClass
								);

		pcQueryQueue = new AgsmRecruitQueueInfo;
		pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_SEARCH_LFP;
		pcQueryQueue->m_bSearchByLevel = bSearchByLevel;
		pcQueryQueue->m_lLV = lLevel;
		pcQueryQueue->m_lPage = lPage;
		pcQueryQueue->m_lCID = lLFPID;
		pcQueryQueue->m_lClass = lClass;

		if( bCheckQueue )
		{
			bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
		else
		{
			bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::ProcessSearchFromLFM( INT32 lLFMID, void *pvSearchInfo, BOOL bCheckQueue )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvSearchInfo != NULL )
	{
		AgsmRecruitQueueInfo		*pcQueryQueue;

		bool				bSearchByLevel;
		INT32				lLevel;
		INT32				lPage;
		INT32				lClass;

		m_csSearchInfo.GetField( FALSE, pvSearchInfo, 0,
								&bSearchByLevel,
								&lLevel,
								&lPage,
								&lClass
								);

		pcQueryQueue = new AgsmRecruitQueueInfo;
		pcQueryQueue->m_nOperation = AGSM_RECRUIT_OP_SEARCH_LFM;
		pcQueryQueue->m_bSearchByLevel = bSearchByLevel;
		pcQueryQueue->m_lCID = lLFMID;
		pcQueryQueue->m_lClass = lClass;
		pcQueryQueue->m_lLV = lLevel;
		pcQueryQueue->m_lPage = lPage;

		if( bCheckQueue )
		{
			bResult = m_paAuOLEDBManager->CheckAndPushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
		else
		{
			bResult = m_paAuOLEDBManager->PushToQueue( pcQueryQueue, __FILE__, __LINE__ );
		}
	}

	return bResult;
}

BOOL AgsmRecruitServer::SetCallbackRegisterLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_REGISTER_LFP, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackCancelLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_CANCEL_LFP, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackSearchLFP(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_SEARCH_LFP, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackRegisterLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_REGISTER_LFM, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackCancelLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_CANCEL_LFM, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackSearchLFM(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_SEARCH_LFM, pfCallback, pClass);
}

BOOL AgsmRecruitServer::SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(RECRUIT_CB_ID_RESULT, pfCallback, pClass);
}


BOOL AgsmRecruitServer::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	BOOL			bResult;

	INT8			lOperation;
	INT32			lCID;
	void			*pvLFPInfo;
	void			*pvLFMInfo;
	INT32			lResult;

	bResult = FALSE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&lOperation,
						&lCID,
						&pvLFPInfo,
						&pvLFMInfo,
						&lResult );

	if( lOperation == AGPMRECRUIT_REGISTER_LFP )
	{
		bResult = ProcessRegisterLFP( lCID, pvLFPInfo, FALSE );
	}
	else if( lOperation == AGPMRECRUIT_REGISTER_LFM )
	{
		bResult = ProcessRegisterLFM( lCID, pvLFMInfo, FALSE );
	}
	else if( lOperation == AGPMRECRUIT_UPDATE_LFP )
	{
		//bResult = ProcessUpdateLFP( lCID, pvLFPInfo );
	}
	//DB�� �ø� PlayerID�� �����.
	else if( lOperation == AGPMRECRUIT_CANCEL_LFP )
	{
		bResult = ProcessCancelLFP( lCID, pvLFPInfo, FALSE );
	}
	else if( lOperation == AGPMRECRUIT_CANCEL_LFM )
	{
		bResult = ProcessCancelLFM( lCID, pvLFPInfo, FALSE );
	}
	//���������� ��Ŷ�� �޾Ҵ�. �߸� ����? ������ش�. ���� �߸��Ѱǵ�? ����~
	else
	{
		
	}

	return bResult;
}
