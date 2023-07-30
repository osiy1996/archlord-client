#include "AgpmNatureEffect.h"

AgpmNatureEffect::AgpmNatureEffect():
	m_nCurrentNatureEffect( NE_NOT_DEFINED )
{
	SetModuleName(_T("AgpmNatureEffect"));
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPMNATUREEFFECT_PACKET_TYPE);
	
	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(AUTYPE_INT32,			1,	// NatureEffect Set Number
							AUTYPE_END,				0
							);
}

AgpmNatureEffect::~AgpmNatureEffect()
{
	// -,.-
}

//	ApModule inherited
BOOL	AgpmNatureEffect::OnAddModule()
{
	// ���ұ� -,.-?..
	return TRUE;
}

BOOL	AgpmNatureEffect::OnInit()
{
	// ���ұ� -,.-?..
	return TRUE;
}

BOOL	AgpmNatureEffect::OnDestroy()
{
	// ���ұ� -,.-?..
	return TRUE;
}

BOOL	AgpmNatureEffect::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize == 0)
		return FALSE;

	INT32	nNatureEffect = NE_NOT_DEFINED;
	
	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&nNatureEffect	);

	// �ݹ� ȣ��
	EnumCallback( CHANGED_NATURE_EFFECT , ( void * ) &nNatureEffect , NULL );
	return TRUE;
}

PVOID	AgpmNatureEffect::MakePacket( INT32 nNatureEffect, INT16 *pnPacketLength)
{
	if (!pnPacketLength )
		return NULL;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMNATUREEFFECT_PACKET_TYPE,
										   &nNatureEffect
										   );
	return pvPacket;
}


INT32	AgpmNatureEffect::SetNatureEffect( INT32 nNatureEffect )
{
	INT32	nPrev = m_nCurrentNatureEffect;
	if( m_nCurrentNatureEffect != nNatureEffect )
	{
		// �ٲ𶧸� ȣ����..

		m_nCurrentNatureEffect = nNatureEffect;

		// �ݹ� ȣ��
		EnumCallback( SET_NATURE_EFFECT , ( void * ) &nNatureEffect , NULL );
	}

	return nPrev;
}
