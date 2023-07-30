#include "AgsmSystemInfo.h"

AgsmSystemInfo::AgsmSystemInfo()
{
	SetModuleName( "AgsmSystemInfo" );

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGPMSYSTEMINFO_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));

	m_csPacket.SetFieldType(AUTYPE_CHAR,			23, // IP���� 
							AUTYPE_INT16,			1,  // Server Type
							AUTYPE_INT32,			1,  // Server Type
							AUTYPE_INT32,			1,  // lCID
		                    AUTYPE_END,				0
							);
							
}

AgsmSystemInfo::~AgsmSystemInfo()
{
	;
}

BOOL AgsmSystemInfo::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter	*) GetModule( "AgpmCharacter" );
	m_pagsmCharacter = (AgsmCharacter	*) GetModule( "AgsmCharacter" );
	//m_pagsmServerManager = (AgsmServerManager	*) GetModule( "AgsmServerManager" );

	return TRUE;
}

BOOL AgsmSystemInfo::SendServerInfo( INT16 nServerType, INT32 lCID, AgsdServer *pcsAgsdServer )
{
	AgpdCharacter		*pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	if (!pcsAgsdServer)
		return bResult;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter != NULL )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);
		if (!pcsAgpdCharacter)
			return FALSE;

		INT16	nPacketLength;

		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMSYSTEMINFO_PACKET_TYPE, 
														pcsAgsdServer->m_szIP,	//IPAddr
														&nServerType,							//ServerType
														&pcsAgsdServer->m_lServerID,			//ServerID
														&lCID									//lCID
														);							

		if( pvPacket != NULL )
		{
			AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
			
			m_csPacket.FreePacket(pvPacket);

			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

/*
BOOL AgsmSystemInfo::SendServerInfo( INT16 nServerType, INT32 lCID, AgsdServer *pcsAgsdServer )
{
	AgpdCharacter		*pcsAgpdCharacter;


	AgsdServerTemplate	*pcsAgsdServerTemplate;
	
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	if (!pcsAgsdServer)
		return bResult;

	pcsAgsdServerTemplate = (AgsdServerTemplate	*)pcsAgsdServer->m_pcsTemplate;

	if( pcsAgsdServerTemplate != NULL )
	{
		//ĳ���������͸� ����.
		if( m_pagpmCharacter != NULL )
		{
			pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);
			if (!pcsAgpdCharacter)
				return FALSE;

			INT16	nPacketLength;

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMSYSTEMINFO_PACKET_TYPE, 
															pcsAgsdServerTemplate->m_szIPv4Addr,	//IPAddr
															&nServerType,							//ServerType
															&pcsAgsdServer->m_lServerID,			//ServerID
															&lCID									//lCID
															);							

			if( pvPacket != NULL )
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

				bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
				
				m_csPacket.FreePacket(pvPacket);

				bResult = TRUE;
			}
			else
			{
				bResult = FALSE;
			}
		}
		else
		{
			bResult = FALSE;
		}
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}
*/

//����� Ư���� ������ �ϴ� �Լ��� �ƴϴ�.
BOOL AgsmSystemInfo::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	BOOL			bResult;

	bResult = FALSE;

	return bResult;
}
