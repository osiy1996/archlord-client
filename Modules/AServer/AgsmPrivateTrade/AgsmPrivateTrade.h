/******************************************************************************
Module:  AgpmItem.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 3
******************************************************************************/

#if !defined(__AGSMPRIVATETRADE_H__)
#define __AGSMPRIVATETRADE_H__

#include "ApModule.h"
#include "ApBase.h"
#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
#include "AgsmItem.h"
#include "AgpmLog.h"
#include "AgsmCharacter.h"
#include "AgpmGrid.h"

//PrivateTrade�� 4���� �ȿ����� �ŷ��� �����ϴ�.
#define	AGPMPRIVATETRADE_MAX_USE_RANGE		400

typedef enum
{
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT = 0x00,			 //�ŷ� ����� �޴°��(����,����,Error��...)
	AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_TRADE,			 //�ŷ� ��û.
	AGPMPRIVATETRADE_PACKET_TRADE_WAIT_CONFIRM,				 //�ŷ����� confirm�� ��ٸ�.		
	AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_CONFIRM,			 //confirm��û.
	AGPMPRIVATETRADE_PACKET_TRADE_CONFIRM,					 //confirm����.
	AGPMPRIVATETRADE_PACKET_TRADE_CANCEL,					 //���.
	AGPMPRIVATETRADE_PACKET_TRADE_START,					 //�ŷ� ����.

	AGPMPRIVATETRADE_PACKET_TRADE_EXECUTE,					 //�ŷ� ����
	AGPMPRIVATETRADE_PACKET_TRADE_END,						 //�ŷ� ����

	AGPMPRIVATETRADE_PACKET_ADD_TO_TRADE_GRID,				
	AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TRADE_GRID,			
	AGPMPRIVATETRADE_PACKET_MOVE_TRADE_GRID,				
	AGPMPRIVATETRADE_PACKET_MOVE_INVEN_GRID,				
	AGPMPRIVATETRADE_PACKET_ADD_TO_TARGET_TRADE_GRID,		
	AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TARGET_TRADE_GRID,	
	AGPMPRIVATETRADE_PACKET_MOVE_TARGET_TRADE_GRID,			
	AGPMPRIVATETRADE_PACKET_LOCK,							//���� �ŷ��� �������� �� �ø��� ok!��ư ��������.
	AGPMPRIVATETRADE_PACKET_TARGET_LOCKED,					//�ŷ��ڰ� �ŷ��� �������� �� �ø��� ok!��ư ��������.
	AGPMPRIVATETRADE_PACKET_UNLOCK,							//���� ok! ��ư ���. -_-;(�������� Grid���� �����ų� �ö�� ��쵵...)
	AGPMPRIVATETRADE_PACKET_TARGET_UNLOCKED,				//�ŷ��ڰ� ok! ��ư ���. -_-;(�������� Grid���� �����ų� �ö�� ��쵵...)
	AGPMPRIVATETRADE_PACKET_ACTIVE_READY_TO_EXCHANGE,		//���� �ŷ� ���ι�ư Ȱ��ȭ.
	AGPMPRIVATETRADE_PACKET_READY_TO_EXCHANGE,				//�ŷ����ڰ� ���������� ����.
	AGPMPRIVATETRADE_PACKET_TARGET_READY_TO_EXCHANGE,		//������ �ŷ����ڰ� ���������� �����ߴ�.
	AGPMPRIVATETRADE_PACKET_UPDATE_MONEY,					//�� �ŷ�â�� ���� �����Ѵ�.
	AGPMPRIVATETRADE_PACKET_TARGET_UPDATE_MONEY,			//�ŷ������ �ŷ�â�� ���� �����Ѵ�.
	AGPMPRIVATETRADE_PACKET_TRADE_REFUSE,					//�ŷ���밡 �ŷ��ź� �����̴�.
} eAgsmPrivateTradeOperation;

typedef enum
{
	AGPMPRIVATETRADE_PACKET_TRADE_REUSLT_00	= 0x00,			 //�ŷ�����!
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_01,				 //Ÿ���� ���°��
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_02,				 //�ŷ��� �� ���� Ÿ��. 
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_03,				 //�Ÿ��� �־ �ȵǴ°��.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_05,				 //���� �ŷ��� ���.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_06,				 //������ �ŷ��� ���.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_07,				 //���ù�ư�� ��Ȱ��ȭ��Ų��.

	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_08,				 //���� �̹� �ŷ����ε� �� �ŷ���û
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_09,				 //������ �̹� �ŷ����ε� �ŷ� ��û.
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_0A,				 //���� ���� �ŷ��Ҽ� ���� �����ϰ��.(�׾��ų�? �̹� �ŷ��ߵ�...)
	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_0B,				 //��밡 ���� �ŷ��Ҽ� ���� �����ϰ��.(�׾��ų�? �̹� �ŷ��ߵ�...)

	AGPMPRIVATETRADE_PACKET_TRADE_RESULT_FF = 0xFF			 //�˼� ���� ����
} eAgsmPrivateTradeErrorCode;

typedef enum
{
	AGPMPRIVATETRADE_CALLBACK_NONE = 0x00,
	AGPMPRIVATETRADE_CALLBACK_TRADE_BUDDY_CHECK,
} eAgsmPrivateTradeCallback;

class AgsmCharManager;
class AgpmReturnToLogin;

class AgsmPrivateTrade : public AgsModule
{
	AgpmGrid			*m_pagpmGrid;

	ApmMap				*m_papmMap;
	AgpmCharacter		*m_pagpmCharacter;
	AgpmFactors			*m_pagpmFactors;
	AgpmItem			*m_pagpmItem;
	AgsmItem			*m_pagsmItem;
	AgpmItemConvert		*m_pagpmItemConvert;
	//AgpmItemLog*	m_pagpmItemLog;
	AgpmLog				*m_pagpmLog;

	AgsmCharacter		*m_pagsmCharacter;
	AgsmCharManager		*m_pagsmCharManager;
	AgpmReturnToLogin	*m_pcsAgpmReturnToLogin;

	//�ŷ��� ��Ŷ
	AuPacket		m_csPacket;
	AuPacket		m_csOriginPos;
	AuPacket		m_csGridPos;

	BOOL AddItemFromTradeBoxToInventory( AgpdCharacter  *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nTradeLayer, INT16 nTradenRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus );
	BOOL AddItemFromInventoryToTradeBox( AgpdCharacter  *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn );

	BOOL ParseGridPos( PVOID pGridPos, INT16 *pnTradeLayer, INT16 *pnTradeRow, INT16 *pnTradeCol);

public:
	AgsmPrivateTrade();
	//��𿡳� �ִ� OnAddModule�̴�.
	BOOL OnAddModule();
	//lCID���� �ŷ����(����,����,������)�� �˷��ش�.
	BOOL SendTradeResult(AgpdCharacter *pcsAgpdCharacter, INT32 lResult);
	//lCID���� �ŷ���û�� �߰� ��ٸ��� ���̶�� �˸���.
	BOOL SendWaitConfirm(AgpdCharacter *pcsAgpdCharacter);
	//TargetID���� lCID�� �ŷ��� ��û�ߴٰ� �˸���.
	BOOL SendRequestConfirm(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter);
	//TargetID�� �ŷ��ź� ���̶�� �˸���.
	BOOL SendRequestRefuse(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter);
	//Trade�� ���۵Ǿ��ٰ� �˸���.
	BOOL SendTradeStart(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter);
	//�ŷ� ��� ��Ŷ�� lCID���Գ�����.
	BOOL SendTradeCancel( INT32 lCID );

	//�� �ŷ�â�� �ݾ��� �����Ѵ�.
	BOOL SendTradeUpdateMoney( AgpdCharacter *pcsAgpdCharacter, INT32 lMoney );
	//��� �ŷ�â�� �ݾ��� �����Ѵ�.
	BOOL SendTradeUpdateTargetMoney( AgpdCharacter *pcsAgpdTargetCharacter, INT32 lMoney );

	//�� �κ����� Trade Grid�� ����
	BOOL SendTradeMoveInventoryToTradeGrid( INT32 lCID, INT32 lIID, AgpdItem *pcsAgpdItem );
	//�ŷ����� Trade Grid���� �κ����� ����
	BOOL SendTradeMoveTargetInventoryToTargetTradeGrid( INT32 lCID, INT32 lIID, AgpdItem *pcsAgpdItem );
	//�� TradeGrid���� �κ����� �̵��Ҷ�
	BOOL SendTradeMoveTradeGridToInventory( INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn );
	//�ŷ����� TradeGrid���� �κ����� �̵��Ҷ�
	BOOL SendTradeMoveTargetTradeGridToTargetInventory( INT32 lCID, INT32 lIID, INT16 nLayer, INT16 nRow, INT16 nColumnm, INT16 nIvenLayer, INT16 nIvenRow, INT16 nIvenColumn  );

	//���� �������� �� �ø��� ok��ư�� ��������.
	BOOL SendTradeLock(AgpdCharacter *pcsAgpdCharacter);
	//��밡 �������� �� �ø��� ok��ư�� ��������.
	BOOL SendTradeTargetLocked(AgpdCharacter *pcsAgpdCharacter);
	//���� �������� �� �ø��� ok��ư�� ������~ ����ϰ� ������...
	BOOL SendTradeUnlock(AgpdCharacter *pcsCharacter);
	//��밡 �������� �� �ø��� ok��ư�� ������~ ����Ҷ�...
	BOOL SendTradeTargetUnlocked(AgpdCharacter *pcsCharacter);
	//�����ŷ� ���ι�ư�� Ȱ��ȭ ��Ű��� ��Ŷ�� ������.
	BOOL SendActiveReadyToExchangeButton(AgpdCharacter *pcsAgpdCharacter);
	//�����ŷ� ������Ŷ�� ������.
	BOOL SendReadyToExchange(AgpdCharacter *pcsAgpdCharacter);
	//��밡 �����ŷ� ������Ŷ�� ���ȴٰ� �˷��ش�.
	BOOL SendTargetReadyToExchange(AgpdCharacter *pcsAgpdCharacter);

	//�ŷ���û�� ó�����ش�.
	BOOL ProcessTradeRequest( INT32 lCID, INT32 lTargetCID );
	//�ŷ��ڷκ��� Trade�� �ϰڴٴ� ������ȣ�� �Դ�.
	BOOL ProcessTradeConfirm( INT32 lCID );
	//(lCID��)�ŷ����¸� �ʱ�ȭ�Ѵ�.
	BOOL ProcessTradeCancel( INT32 lCID );
	//���� �ø���.
	BOOL ProcessTradeUpdateMoney( INT32 lCID, INT32 lMoneyCount );
	//�ּҽ� ���� �������� ���󺹱���Ŵ.
	BOOL RestoreTradeItems( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar );

	BOOL ProcessAddToTradeGrid( INT32 lCID, INT32 lIID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn );
	BOOL ProcessRemoveFromTradeGrid( INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus );

	BOOL TestGridCopyInven(AgpdItemADChar *pcsAgpdItemADChar, AgpdGrid *pcTradeGrid);
	BOOL TestGridCopyTradeGrid(AgpdItemADChar *pcsAgpdItemADChar, AgpdGrid *pcTradeGrid);
	BOOL MoveTradeGridItemToInventory( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, INT32 lTimeStamp = 0);

	//JK_�ŷ��߱���
	BOOL RestoreTradeItemsMyself( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar);
	BOOL ProcessTradeCancelUnExpected( INT32 lCID );


	//�������� ��ȯ���ش�.
	BOOL ProcessExchangeItem(AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar );
	//�ŷ��غ�(��� �������� �ŷ�â�� �ø�����)�� �� �Ǿ��ٰ� �˷��ش�.
	BOOL ProcessLock( INT32 lCID );
	//�ŷ��غ�(��� �������� �ŷ�â�� �ø�����)�� �� �Ǿ��ٰ� �˷��ش�.
	BOOL ProcessUnlock( INT32 lCID );
	//�����ŷ� ����!! ��� ����~
	BOOL ProcessReadyToExchange( INT32 lCID );

	BOOL CheckTradeStatus( INT32 lCID );
	void ResetTradeStatus( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar );

	BOOL ResetPrivateTradeOptionGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdADChar);

	//��Ŷ�� �ް� �Ľ�����~
	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	
	//////////////////////////////////////////////////////////////////////////
	// Log ���� - 2004.05.04. steeple
	INT32	GetCurrentTimeStamp();
	BOOL	WriteTradeItemLog(AgpdCharacter* pcsSeller, AgpdCharacter* pcsBuyer, AgpdItem* pcsItem, INT32 lTimeStamp = 0);
	BOOL	WriteTradeMoneyLog(AgpdCharacter* pcsSeller, AgpdCharacter* pcsBuyer, INT32 lMoney, INT32 lTimeStamp = 0);

	static BOOL	CBRemoveTradeGrid(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL CBDisconnect(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	// callback function
	BOOL SetCallbackBuddyCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);

private:
	BOOL CheckInventorySpace(AgpdCharacter* pcsCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter* pcsTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar);
	void GetItemCountByCoinType(AgpdGrid* pGrid, short* normalItemCount, short* cashItemCount);	// �Ϲ� �����۰� ĳ���������� ������ ���Ѵ�.
	INT32 NumberOfBoundOnOwnerItemInTradeGrid(AgpdItemADChar *pcsAgpdItemADChar);
};

#endif
