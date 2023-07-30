#pragma once

#include "AgpmEventGacha.h"
#include "AgsmCharacter.h"
#include "AgpdDropItem2.h"
#include "AgpmItem.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"

class AgsmEventGacha : public AgsModule
{
public:
	AgpmCharacter		*m_pcsAgpmcharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmDropItem2		*m_pcsAgpmDropItem2;
	AgpmEventGacha		*m_pcsAgpmEventGacha;

	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmItem			*m_pcsAgsmItem;
	AgsmItemManager		*m_pcsAgsmItemManager;

public:
	AgsmEventGacha();
	virtual ~AgsmEventGacha();

	// ... ApModule inherited		
	BOOL OnAddModule();

	BOOL IsGachaBlocked(AgpdCharacter* pcsCharacter);

	INT32	ProcessGacha( AgpdCharacter * pcsCharacter , INT32 nGachaType );
	// ���ϰ��� ������ Item TID
	// ���� ��� ������. ���� �Լ��� �������� �˻縦��.
	// �������� �־��ִ� ��������..

	static BOOL CBCheckAllBlock(PVOID pData, PVOID pClass, PVOID pCustData);

	// ... Callback
	static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGacha(PVOID pData, PVOID pClass, PVOID pCustData);

	// ��í ������ ������ �ð��ڿ� �־��ֱ� ���� �ݹ�.
	static BOOL	CBGachaItemUpdate(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
};
