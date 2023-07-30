#ifndef __AGSMSYSTEMINFO_H_
#define __AGSMSYSTEMINFO_H_

#include "AgsEngine.h"
#include "AgpmCharacter.h"
#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmSystemInfoD" )
#else
#pragma comment ( lib , "AgsmSystemInfo" )
#endif
#endif

class AgsmSystemInfo : public AgsModule
{
	AgpmCharacter		*m_pagpmCharacter;
	AgsmCharacter		*m_pagsmCharacter;

	AuPacket			m_csPacket;   //SystemInfo ��Ŷ.

public:

	AgsmSystemInfo();
	~AgsmSystemInfo();

	BOOL SendServerInfo( INT16 nServerType, INT32 lCID, AgsdServer *pcsAgsdServer );

	//��𿡳� �ִ� OnAddModule�̴�. Ư���Ұ� ����. Ư���ϸ� �ȵǳ�? ���d�d~
	BOOL OnAddModule();
	//��Ŷ�� �ް� �Ľ�����~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
};

#endif