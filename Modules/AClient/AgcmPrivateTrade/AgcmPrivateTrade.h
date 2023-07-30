/******************************************************************************
Module:  AgpmItem.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 2
******************************************************************************/

//99,105,109 Ȯ�ι�ư RGB��.

#if !defined(__AGCMPRIVATETRADE_H__)
#define __AGCMPRIVATETRADE_H__

#include <AgcModule/AgcModule.h>
#include <ApBase/ApBase.h>
#include <AgpmCharacter/AgpdCharacter.h>
#include <AgpmItem/AgpdItem.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmPrivateTradeD" )
#else
#pragma comment ( lib , "AgcmPrivateTrade" )
#endif
#endif

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
	AGPMPRIVATETRADE_PACKET_TRADE_REFUSE,					//��밡 �ŷ��ź����̴�.	2005.06.02. By SungHoon
	AGPMPRIVATETRADE_PACKET_UPDATE_SELF_CC,
	AGPMPRIVATETRADE_PACKET_UPDATE_PEER_CC,
} eAgcmPrivateTradeOperation;

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
} eAgcmPrivateTradeErrorCode;

// Command Message
enum 
{
	UICM_ACUITRADECONFIRM_COLSE_ME		=		0		// Window Close �� ��û�Ѵ� 
};

enum eAgcmPrivateTradeCB_ID				// callback id in item module
{
	ITEM_CB_ID_UPDATE_LOCK_BUTTON			= 0,
	ITEM_CB_ID_UPDATE_TARGET_LOCK_BUTTON,
	ITEM_CB_ID_UPDATE_TRADE_MONEY,
	ITEM_CB_ID_UPDATE_TARGET_TRADE_MONEY,
	ITEM_CB_ID_TRADE_START,
	ITEM_CB_ID_TRADE_END,
	ITEM_CB_ID_TRADE_CANCEL,

//���ο� UI�� ���� ���� �߰��Ǵ� CallBack
	ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE_YES_NO,
	ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE,
	ITEM_CB_ID_TRADE_CLOSE_UI_PRIVATETRADE,
	ITEM_CB_ID_TRADE_UI_UPDATE_TARGET_LOCK_BUTTON,
	ITEM_CB_ID_TRADE_UI_ACTIVE_READY_TO_EXCHANGE,
	ITEM_CB_ID_TRADE_UI_READY_TO_EXCHANGE,
	ITEM_CB_ID_TRADE_UI_CLIENT_READY_TO_EXCHANGE,

//PrivateTradeó���޼���
	ITEM_CB_ID_TRADE_UI_MSG_TRADE_COMPELTE,
	ITEM_CB_ID_TRADE_UI_MSG_TRADE_CANCEL,

	ITEM_CB_ID_TRADE_REQUESE_REFUSE_USER,			//	2005.06.02. By SungHoon

	ITEM_CB_ID_UPDATE_TRADE_SELF_CC,
	ITEM_CB_ID_UPDATE_TRADE_PEER_CC,
};

enum eAGPM_PRIVATE_TRADE_ITEM;

class AgcmPrivateTrade : public AgcModule
{
	class ApmMap		*m_papmMap;
	class AgpmCharacter	*m_pagpmCharacter;
	class AgcmCharacter	*m_pagcmCharacter;
	class AgpmFactors	*m_pagpmFactors;
	class AgpmItem		*m_pagpmItem;
	class AgcmItem		*m_pagcmItem;
	class AgpmGrid		*m_pagpmGrid;

	//�ŷ��� ��Ŷ
	AuPacket		m_csPacket;
	AuPacket		m_csOriginPos;
	AuPacket		m_csGridPos;

	AgpdCharacter *GetAgpdCharacter( INT32 lCID );

	//TradeGrid�� �������� ����.
	BOOL AddItemToTradeGrid( INT32 lCID, INT32 lIID, void *pGridPos );
	//TradeGrid���� �������� ��������.
	BOOL RemoveItemFromTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos );
	//TradeGrid���� lIID�� �̵��Ѵ�.
	BOOL MoveItemTradeGrid( INT32 lCID, INT32 lIID, void *pOriginGridPos, void *pGridPos );
	//TargetTradeGrid�� �������� ����.
	BOOL AddItemToTargetTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos );
	//TargetTradeGrid���� �������� ��������.
	BOOL RemoveItemFromTargetTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos );

	//GridPos�� Ǯ���.
	BOOL ParseGridPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnColumn );

	BOOL ProcessLockButton( INT32 lCRequesterID, bool bPushDown );
	BOOL ProcessTargetLockButton( INT32 lCRequesterID, bool bPushDown );
	BOOL ProcessActiveReadyToExchange( INT32 lCRequesterID );
	BOOL ProcessReadyToExchange( INT32 lCRequesterID );
	BOOL ProcessClientReadyToExchange( INT32 lCRequesterID );

	BOOL ProcessCancel( INT32 lCRequesterID );

public:
	//Seong Yon-jun - Private Trade confirm Windows
	
	//AgcmPrivateTrade�� ������
	AgcmPrivateTrade();
	//��𿡳� �ִ� OnAddModule�̴�. Ư���Ұ� ����. ��.��
	BOOL OnAddModule();
	//����(lCID) ������(lCTargetID)���� �ŷ��� ��û�Ѵ�.
	BOOL SendTradeRequest( INT32 lCID, INT32 lTargetCID );
	//Trade��û�� �޾��ش�.
	BOOL SendTradeRequestConfirm( INT32 lCID );
	//Trade���!
	BOOL SendTradeCancel( INT32 lCID );
	//���� �ø���.
	BOOL SendTradeUpdateMoney( INT32 lCID, INT32 lMoneyCount );
	BOOL SendTradeUpdateCC( INT32 lCID, INT32 cc );

	//TradeGrid�� Item�� �ִ´�( ���� 2009. 3. 3. )
	// ù��°���ڿ� ������ ������ ����
	BOOL SendTradeAddItemToTradeGrid( eAGPM_PRIVATE_TRADE_ITEM eType , INT32 lCID, INT32 lIID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn );
	//TradeGrid���� Item�� �����
	BOOL SendTradeRemoveItemFromTradeGrid(INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus );
	//�������� �� �ø��� ok!��ư�� ��������.
	BOOL SendTradeLock();
	//�������� �� �ø��� ok!��ư�� �������� ����ϰ� ������.... ��.��
	BOOL SendTradeUnlock();
	//���������� �ŷ�����! �Ҷ�~
	BOOL SendReadyToExchange();

	//[CallBacks]
	//�� Lock Button�� ��۵ɶ�~
	BOOL SetCallbackUpdateLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//������ Lock Button�� ��������~
	BOOL SetCallbackUpdateTargetLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//�������� �ø���
	BOOL SetCallbackUpdateTradeMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//��밡 �ø����� �ٲ�
	BOOL SetCallbackUpdateTargetTradeMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateTradeSelfCC(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateTradePeerCC(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//Trade�� ���۵Ǿ����� �˸��� CB
	BOOL SetCallbackTradeStart(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//Trade�� �������� �˸��� CB
	BOOL SetCallbackTradeEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//Trade�� ��� �Ǿ����� �˸��� CB
	BOOL SetCallbackTradeCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//Trade UI�ݹ�.
	BOOL SetCallbackTradeOpenUIYesNo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeOpenUI(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeCloseUI(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeUpdateTargetLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeActiveReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeClientReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//Trade MSG Callback
	BOOL SetCallbackTradeMSGComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackTradeMSGCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackTradeRequestRefuseUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);		//	2005.06.02. By SungHoon
	
	//��Ŷ�� �ް� �Ľ�����~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	char			m_strDebugMessage[256];

	// Trade confirm UI ���� �Լ� 
	BOOL	OnTradeStart()			;			// Trade�� ���۽�..
	BOOL	OnUpdateLockButton()	;			// Confirm ��ư�� ���ŵǸ�..
	BOOL	OnUpdateTradeMoney()	;			// Money�� ���ŵǸ�
	BOOL	OnUpdateMyTradeMoney( INT32 lMoney )	;
	BOOL	OnUpdateHisTradeMoney( INT32 lMoney )	;
};

#endif
