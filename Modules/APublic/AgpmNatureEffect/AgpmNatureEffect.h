#pragma once

#include <ApBase/ApBase.h>
#include <ApModule/ApModule.h>
#include <AuPacket/AuPacket.h>

class AgpmNatureEffect : public ApModule
{
public:
	enum CBList
	{
		SET_NATURE_EFFECT		,	// �ɵ�
		CHANGED_NATURE_EFFECT		// ���� ( ��Ŷ ���ú갰�� ���.. )
			// ���������� pData�� INT32 * ������ ������ Ÿ���� ����.
	};

	enum	NEList
	{
		NE_NOT_DEFINED	= -1,
		NE_SNOW			= 0	,
		NE_RAIN			= 1
	}; // �׳� �������.. Ŭ���̾�Ʈ���� �ֵ������� �ž� �ִ�.. �⺻���ΰ͸� �����..

protected:
	INT32	m_nCurrentNatureEffect;

public:
	//	Packet
	AuPacket		m_csPacket;

public:
	AgpmNatureEffect();
	virtual ~AgpmNatureEffect();

	//	ApModule inherited
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// Callback
	BOOL	SetCallbackSetNatureEffect		(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(SET_NATURE_EFFECT			, pfnCallback, pvClass); }
	BOOL	SetCallbackChangedNatureEffect	(ApModuleDefaultCallBack pfnCallback, PVOID pvClass) { return SetCallback(CHANGED_NATURE_EFFECT		, pfnCallback, pvClass); }

	// Methods
	INT32	SetNatureEffect( INT32 nNatureEffect = NE_NOT_DEFINED );
		// return prev nature effect index

	//	Event Packet
	PVOID	MakePacket( INT32 nNatureEffect, INT16 *pnPacketLength);
};

