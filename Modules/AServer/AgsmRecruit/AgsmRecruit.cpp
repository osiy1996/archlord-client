#include "AgsmRecruit.h"

AgsmRecruit::AgsmRecruit()
{
	SetModuleName( "AgsmRecruit" );

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGSMRECRUIT_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  ����
							AUTYPE_INT32,			1, // lLFPID  ����Ʈ�� �ø��� ID
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
}

AgsmRecruit::~AgsmRecruit()
{
	;
}

//��𼭳� �� �� �ִ� OnAddModule()
BOOL AgsmRecruit::OnAddModule()
{
	m_papmMap = (ApmMap *)GetModule("ApmMap");
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmParty = (AgpmParty *) GetModule("AgpmParty");
	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pAgsmServerManager = (AgsmServerManager *)GetModule( "AgsmServerManager2" );

	if( !m_papmMap || !m_pagpmCharacter || !m_pagpmParty || !m_pagpmFactors || !m_pagpmItem || !m_pAgsmServerManager )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmRecruit::ProcessRegisterLFP( INT32 lLFPID )
{
	//lLFPID�� ������� ���� ���ο� ID(String)�� ����.
	AgpdCharacter			*pcsAgpdCharacter;
	AgpdFactorCharStatus	*pcsAgpdFactorCharStatus;
	AgpdFactorCharType		*pcsAgpdFactorCharType;

	BOOL					bResult;
	char					strCharName[AGPACHARACTER_MAX_ID_STRING];
	INT32					lLevel;
	INT32					lClass;

	bResult = FALSE;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFPID );

	if( pcsAgpdCharacter )
	{
		memset( strCharName, 0, AGPACHARACTER_MAX_ID_STRING );

		//AccountID�� Copy�Ѵ�.
		strcat( strCharName, pcsAgpdCharacter->m_szID );

		//�˾Ƴ� ������ Recruit������ ������.
		INT32	lLevel	= m_pagpmFactors->GetLevel(&pcsAgpdCharacter->m_csFactor);
		INT32	lClass	= m_pagpmFactors->GetClass(&pcsAgpdCharacter->m_csFactor);

		bResult = SendRegisterLFP( lLFPID, strCharName, lLevel, lClass, 0 );
	}

	return bResult;
}

BOOL AgsmRecruit::ProcessRegisterLFM( INT32 lLFMID, void *pvLFMInfo )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pvLFMInfo && m_pagpmParty )
	{
		AgpdCharacter			*pcsAgpdCharacter;

		char			*pstrPurpose;
		char			*pstrCharName;

		INT32			lRequireMember;
		INT32			lMinLV;
		INT32			lMaxLV;

		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFMID );

		m_csLFMInfo.GetField( FALSE, pvLFMInfo, 0,
								NULL,				//lTotalCount
								NULL,				//lIndex
								&pstrPurpose,		//purpose
								&lRequireMember,	//lRequireMember
								&lMinLV,			//MinLV
								&lMaxLV,			//MaxLV
								NULL,				//Level
								NULL,				//Class
								&pstrCharName,		//CharName
								NULL				//lLeaderID
								);
								

		if( pcsAgpdCharacter )
		{
			//if( m_pagpmParty->GetLeaderCID( pcsAgpdCharacter ) == lLFMID )
			{
				if( (pstrCharName != NULL) && (pstrPurpose != NULL ) )
				{
					//lLFPID�� ������� ���� ���ο� ID(String)�� ����.
					AgpdFactorCharStatus	*pcsAgpdFactorCharStatus;
					AgpdFactorCharType		*pcsAgpdFactorCharType;

					INT32					lLevel	= m_pagpmFactors->GetLevel(&pcsAgpdCharacter->m_csFactor);
					INT32					lClass	= m_pagpmFactors->GetClass(&pcsAgpdCharacter->m_csFactor);

					//�˾Ƴ� ������ Recruit������ ������.
					bResult = SendRegisterLFM( lLFMID, pstrPurpose, lRequireMember, lMinLV, lMaxLV, lLevel, lClass, pstrCharName, lLFMID, 1 );
				}
			}
		}
	}

	return bResult;
}

BOOL AgsmRecruit::SendRegisterLFP( INT32 lLFPID, char *pstrLFPID, INT32 lLevel, INT32 lClass, INT32 lResult )
{
	AgsdServer		*pcsAgsdServer;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFP;
	INT16	nPacketLength;
	void	*pvLFPInfo;

	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	pvLFPInfo = m_csLFPInfo.MakePacket(FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													NULL,				//lTotalCount
													NULL,				//lIndex
													&lLevel,			//Level
													&lClass,			//Calss
													pstrLFPID,			//pstrLFPID
													NULL				//lPlayerID
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													pvLFPInfo,			//LFPInfo
													NULL,				//LFMInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
		
		m_csLFMInfo.FreePacket(pvLFPInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruit::SendRegisterLFM( INT32 lLFMID, char *pstrPurpose, INT32 lRequireMember, INT32 lMinLV, INT32 lMaxLV, INT32 lLevel, INT32 lClass, char *pstrCharName, INT32 lLeaderID, INT32 lResult )
{
	AgsdServer		*pcsAgsdServer;
	void			*pvLFMInfo;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_REGISTER_LFM;
	INT16	nPacketLength;

	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	pvLFMInfo = m_csLFMInfo.MakePacket( FALSE, &nPacketLength, AGPMRECRUIT_PACKET_TYPE,
											NULL,				//lTotalCount
											NULL,				//Index
											pstrPurpose,		//Purpose
											&lRequireMember,	//Require Members
											&lMinLV,			//Min Lv
											&lMaxLV,			//Max Lv
											&lLevel,			//LV
											&lClass,			//lClass
											pstrCharName,		//pstrCharName
											&lLeaderID );		//LeaderID

	if( (pcsAgsdServer != NULL) && (pvLFMInfo != NULL) )
	{
		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														&lOperation,		//lOperation
														&lLFMID,			//lLFPID
														NULL,				//LFPInfo
														pvLFMInfo,			//LFMInfo
														&lResult			//lResult
														);

		if( pvPacket != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL AgsmRecruit::SendUpdateLFPInfo( INT32 lLFPID, char *pstrLFPID, INT32 lLevel, INT32 lClass, INT32 lResult )
{
	AgsdServer		*pcsAgsdServer;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_UPDATE_LFP;
	INT16	nPacketLength;
	void	*pvLFPInfo;

	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	pvLFPInfo = m_csLFPInfo.MakePacket(FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													NULL,				//lTotalCount
													NULL,				//lIndex
													&lLevel,			//Level
													&lClass,				//Class
													pstrLFPID,			//pstrLFPID
													NULL				//PlayerID
													);

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
													&lOperation,		//lOperation
													&lLFPID,			//lLFPID
													pvLFPInfo,			//pvLFPInfo
													NULL,				//pvLFMInfo
													&lResult			//lResult
													);

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
		
		m_csLFMInfo.FreePacket(pvLFPInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmRecruit::SendCancelLFP( INT32 lLFPID )
{
	AgsdServer		*pcsAgsdServer;
	AgpdCharacter	*pcsAgpdCharacter;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_CANCEL_LFP;
	INT16	nPacketLength;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFPID );
	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	if( pcsAgpdCharacter && pcsAgsdServer )
	{
		void	*pvLFPInfo = m_csLFPInfo.MakePacket( FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														NULL,						//lTotalCount
														NULL,						//lIndex
														NULL,						//lLevel
														NULL,						//Class
														pcsAgpdCharacter->m_szID,	//CharName
														NULL						//lPlayerID
														);

		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														&lOperation,		//lOperation
														&lLFPID,			//lLFPID
														pvLFPInfo,			//pvLFPInfo.
														NULL,				//pvLFMInfo
														NULL				//lResult
														);

		if( pvPacket != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL AgsmRecruit::SendCancelLFM( INT32 lLFMID )
{
	AgsdServer		*pcsAgsdServer;
	AgpdCharacter	*pcsAgpdCharacter;
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMRECRUIT_CANCEL_LFM;
	INT16	nPacketLength;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lLFMID );
	pcsAgsdServer = m_pAgsmServerManager->GetRecruitServer();

	if( pcsAgpdCharacter && pcsAgsdServer )
	{
		void	*pvLFMInfo = m_csLFMInfo.MakePacket( FALSE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														NULL,						//lTotalCount
														NULL,						//lIndex
														NULL,						//Purpose
														NULL,						//RequireMemeber
														NULL,						//MinLV
														NULL,						//MaxLV
														NULL,						//lLevel
														NULL,						//lClass
														pcsAgpdCharacter->m_szID,	//CharName
														NULL						//lLeaderID
														);

		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMRECRUIT_PACKET_TYPE,
														&lOperation,		//lOperation
														&lLFMID,			//lLFPID
														NULL,				//pvLFPInfo.
														pvLFMInfo,			//pvLFMInfo
														NULL				//lResult
														);

		if( pvPacket != NULL )
		{
			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdServer->m_dpnidServer );
			
			m_csPacket.FreePacket(pvPacket);
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

//��Ŷ�� �ް� �Ľ�����~
BOOL AgsmRecruit::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	//�ŷ���� Ȥ�� Error�޽����� ������ش�~ ����غ���~ ����غ���~
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

	//�������� �Ǵ�.(Sales Box)�� �ø���.
	if( lOperation == AGPMRECRUIT_REGISTER_LFP )
	{
		bResult = ProcessRegisterLFP( lCID );
	}
	else if( lOperation == AGPMRECRUIT_REGISTER_LFM )
	{
		bResult = ProcessRegisterLFM( lCID, pvLFMInfo );
	}
	//���������� ��Ŷ�� �޾Ҵ�. ��ġ���ؿ�����!! �� �׷��� �̰� �� �߸����ݾ�? -_-; ���d�d~
	else
	{
		//�̹� bResult = FALSE�̴�. �׿��� ������ ����!!
	}

	return bResult;
}
