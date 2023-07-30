/******************************************************************************
Module:  AgpmItem.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 2
******************************************************************************/

#include "AgcmPrivateTrade.h"
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmGrid/AgpmGrid.h>

AgcmPrivateTrade::AgcmPrivateTrade()
{
	SetModuleName( "AgcmPrivateTrade" );

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGPMPRIVATETRADE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  ����
							AUTYPE_INT32,			1, // lCID �����
							AUTYPE_INT32,			1, // lCTargetID �����
							AUTYPE_INT32,			1, // lIID       Item ID
							AUTYPE_INT32,			1, // lITID      Item Type ID
							AUTYPE_INT32,			1, // Money Count
							AUTYPE_PACKET,			1, // csOriginPos
							AUTYPE_PACKET,			1, // csGridPos
							AUTYPE_INT32,			1, // lResult    Result
		                    AUTYPE_END,				0
							);

	//Add, Clear ���� �Ҷ� ����Ѵ�
	m_csOriginPos.SetFlagLength(sizeof(INT8));
	m_csOriginPos.SetFieldType(
		                         AUTYPE_INT16,		1, //Layer
		                         AUTYPE_INT16,		1, //Row
		                         AUTYPE_INT16,		1, //Column
		                         AUTYPE_END,		0
								 );

	//Add, Clear ���� �Ҷ� ����Ѵ�
	m_csGridPos.SetFlagLength(sizeof(INT8));
	m_csGridPos.SetFieldType(
		                         AUTYPE_INT16,		1, //Layer
		                         AUTYPE_INT16,		1, //Row
		                         AUTYPE_INT16,		1, //Column
		                         AUTYPE_END,		0
								 );

	m_pagcmCharacter	=	NULL	;
	m_pagcmItem			=	NULL	;
	m_pagpmGrid			=	NULL	;
}

BOOL AgcmPrivateTrade::OnAddModule()
{
	m_papmMap = (ApmMap *)GetModule("ApmMap");
	ASSERT( NULL != m_papmMap );
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	ASSERT( NULL != m_pagpmCharacter );
	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	ASSERT( NULL != m_pagpmFactors );
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	ASSERT( NULL != m_pagpmItem );
	m_pagcmCharacter = (AgcmCharacter*) GetModule ("AgcmCharacter");
	ASSERT ( NULL != m_pagcmCharacter );
	m_pagcmItem = (AgcmItem*) GetModule( "AgcmItem" );
	ASSERT ( NULL != m_pagcmItem );
	m_pagpmGrid = (AgpmGrid*) GetModule( "AgpmGrid" );
	ASSERT ( NULL != m_pagpmGrid );

	memset( m_strDebugMessage, 0, sizeof( m_strDebugMessage ) );
	strcat( m_strDebugMessage, "AgcmPrivate Debug Test!" );

	// SetCallback

	return TRUE;
}

//�� Confirm Button�� ��۵ɶ�~
BOOL AgcmPrivateTrade::SetCallbackUpdateLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UPDATE_LOCK_BUTTON, pfCallback, pClass);
}

//������ Confirm Button�� ��������~
BOOL AgcmPrivateTrade::SetCallbackUpdateTargetLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UPDATE_TARGET_LOCK_BUTTON, pfCallback, pClass);
}

//�������� �ø���
BOOL AgcmPrivateTrade::SetCallbackUpdateTradeMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UPDATE_TRADE_MONEY, pfCallback, pClass);
}

//��밡 �ø����� �ٲ�
BOOL AgcmPrivateTrade::SetCallbackUpdateTargetTradeMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UPDATE_TARGET_TRADE_MONEY, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackUpdateTradeSelfCC(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UPDATE_TRADE_SELF_CC, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackUpdateTradePeerCC(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UPDATE_TRADE_PEER_CC, pfCallback, pClass);
}

//Trade Start
BOOL AgcmPrivateTrade::SetCallbackTradeStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_START, pfCallback, pClass);
}

//Trade�� �������� �˸��� CB
BOOL AgcmPrivateTrade::SetCallbackTradeEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_END, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_CANCEL, pfCallback, pClass);
}

//Trade UI�ݹ�.
BOOL AgcmPrivateTrade::SetCallbackTradeOpenUIYesNo(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE_YES_NO, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeOpenUI(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeCloseUI(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_CLOSE_UI_PRIVATETRADE, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeUpdateTargetLockButton(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_UI_UPDATE_TARGET_LOCK_BUTTON, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeActiveReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_UI_ACTIVE_READY_TO_EXCHANGE, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_UI_READY_TO_EXCHANGE, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeClientReadyToExchange(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_UI_CLIENT_READY_TO_EXCHANGE, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeMSGComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_UI_MSG_TRADE_COMPELTE, pfCallback, pClass);
}

BOOL AgcmPrivateTrade::SetCallbackTradeMSGCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_UI_MSG_TRADE_CANCEL, pfCallback, pClass);
}

/*
	2005.06.02 By SungHoon
	Ʈ���̵带 ��û�� ����ڰ� �ź����� �� �Ҹ����� �ݹ�
*/
BOOL AgcmPrivateTrade::SetCallbackTradeRequestRefuseUser(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_TRADE_REQUESE_REFUSE_USER, pfCallback, pClass);
}

AgpdCharacter *AgcmPrivateTrade::GetAgpdCharacter( INT32 lCID )
{
	AgpdCharacter *pcAgpdCharacter;

	if( m_pagpmCharacter != NULL )
	{
		pcAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);
	}
	else
	{
		pcAgpdCharacter = NULL;
	}

	return pcAgpdCharacter;
}

BOOL AgcmPrivateTrade::ParseGridPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnColumn )
{
	INT16			nLayer, nRow, nColumn;

	m_csGridPos.GetField(FALSE, pGridPos, 0,
								&nLayer,
								&nRow,
								&nColumn);

	if(pnLayer)
	{
		*pnLayer = nLayer;
	}

	if(pnRow)
	{
		*pnRow = nRow;
	}

	if(pnColumn)
	{
		*pnColumn = nColumn;
	}

	return TRUE;	
}

//�������� TradeGrid�� �������� �߰��ȴ�.
BOOL AgcmPrivateTrade::AddItemToTradeGrid( INT32 lCID, INT32 lIID, void *pGridPos )
{
	AgpdCharacter  *pcsAgpdCharacter;
	AgpdItem       *pcsItem = m_pagpmItem->GetItem(lIID);

	BOOL			bResult;

	pcsAgpdCharacter = GetAgpdCharacter( lCID );

	if( pcsAgpdCharacter == NULL )
	{
		bResult = FALSE;
	}
	else
	{
		if( pGridPos != NULL )
		{
			INT16			nTradeLayer, nTradeRow, nTradeColumn;

			bResult = ParseGridPos( pGridPos, &nTradeLayer, &nTradeRow, &nTradeColumn );

			if( bResult )
			{
				AgpdItemADChar *pcsAgpdItemADChar;
				
				pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

				if( pcsAgpdItemADChar != NULL )
				{
					//bResult = pcsAgpdItemADChar->m_csTradeGrid.Add( nTradeLayer, nTradeRow, nTradeColumn, AGPDITEM_MAKE_ITEM_ID( lIID, pcsItem->m_lTID ), 1, 1 );
					
					bResult = m_pagpmGrid->Add( &(pcsAgpdItemADChar->m_csTradeGrid), nTradeLayer, nTradeRow, nTradeColumn, pcsItem->m_pcsGridItem, 1, 1 );

					//�κ����� ������!
					AgpdItem		*pcAgpdItem;

					pcAgpdItem = m_pagpmItem->GetItem( lIID );

					if( pcAgpdItem != NULL )
					{
						m_pagpmItem->RemoveItemFromInventory( pcsAgpdCharacter, pcAgpdItem );

						bResult = TRUE;
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
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

//�������� TradeGrid���� �������� ������.
BOOL AgcmPrivateTrade::RemoveItemFromTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos )
{
	AgpdCharacter  *pcsAgpdCharacter;
	AgpdItem       *pcsItem = m_pagpmItem->GetItem( lIID );

	BOOL			bResult;

	pcsAgpdCharacter = GetAgpdCharacter( lCID );

	if( pcsAgpdCharacter == NULL )
	{
		bResult = FALSE;
	}
	else
	{
		if( pGridPos != NULL )
		{
			INT16			nInvenLayer, nInvenRow, nInvenColumn;
			INT16			nTradeLayer, nTradeRow, nTradeColumn;

			bResult = ParseGridPos( pOriginPos, &nTradeLayer, &nTradeRow, &nTradeColumn );

			if( bResult )
			{
				bResult = ParseGridPos( pGridPos, &nInvenLayer, &nInvenRow, &nInvenColumn );

				if( bResult )
				{
					AgpdItemADChar *pcsAgpdItemADChar;
					
					pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

					if( pcsAgpdItemADChar != NULL )
					{
						bResult = m_pagpmGrid->Clear( &(pcsAgpdItemADChar->m_csTradeGrid), nTradeLayer, nTradeRow, nTradeColumn, 1, 1 );

						//�κ����� ����.
						//bResult = pcsAgpdItemADChar->m_csInventory.Add( nInvenLayer, nInvenRow, nInvenColumn, lIID, 1, 1 );
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
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

//Target TradeGrid�� �������� �߰��ȴ�.
BOOL AgcmPrivateTrade::AddItemToTargetTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos )
{
	AgpdCharacter  *pcsAgpdCharacter;
	AgpdItem       *pcsItem = m_pagpmItem->GetItem(lIID);

	BOOL			bResult;

	pcsAgpdCharacter = GetAgpdCharacter( lCID );

	if( pcsAgpdCharacter == NULL )
	{
		bResult = FALSE;
	}
	else
	{
		if( pGridPos != NULL )
		{
			INT16			nLayer, nRow, nColumn;

			bResult = ParseGridPos( pGridPos, &nLayer, &nRow, &nColumn );

			if( bResult )
			{
				AgpdItemADChar *pcsAgpdItemADChar;
				
				pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

				if( pcsAgpdItemADChar != NULL )
				{
					bResult = m_pagpmGrid->Add( &(pcsAgpdItemADChar->m_csClientTradeGrid), nLayer, nRow, nColumn, pcsItem->m_pcsGridItem, 1, 1 );
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
	}

	return bResult;
}

//Target TradeGrid���� �������� ������.
BOOL AgcmPrivateTrade::RemoveItemFromTargetTradeGrid( INT32 lCID, INT32 lIID, void *pOriginPos, void *pGridPos )
{
	AgpdCharacter  *pcsAgpdCharacter;
	AgpdItem       *pcsItem = m_pagpmItem->GetItem(lIID);

	BOOL			bResult;

	pcsAgpdCharacter = GetAgpdCharacter( lCID );

	if( pcsAgpdCharacter == NULL )
	{
		bResult = FALSE;
	}
	else
	{
		if( pGridPos != NULL )
		{
			INT16			nTradeLayer, nTradeRow, nTradeColumn;

			bResult = ParseGridPos( pOriginPos, &nTradeLayer, &nTradeRow, &nTradeColumn );

			if( bResult )
			{
				AgpdItemADChar *pcsAgpdItemADChar;
				
				pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

				if( pcsAgpdItemADChar != NULL )
				{
					bResult = m_pagpmGrid->Clear( &(pcsAgpdItemADChar->m_csClientTradeGrid), nTradeLayer, nTradeRow, nTradeColumn, 1, 1 );
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
	}

	return bResult;
}

//����(lCID) ������(lCTargetID)���� �ŷ��� ��û�Ѵ�.
BOOL AgcmPrivateTrade::SendTradeRequest( INT32 lCID, INT32 lTargetCID )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

		if( pcsAgpdCharacter != NULL && m_pagpmCharacter->IsPC(pcsAgpdCharacter))
		{
			//lCID�� NORMAL�����϶��� �ŷ��� ��û�Ҽ� �ִ�.
			if( pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_NORMAL
				|| AGPDCHAR_STATUS_NOT_ACTION == pcsAgpdCharacter->m_unActionStatus )
			{
				INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_TRADE;
				INT16	nPacketLength;

				PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE,
																&nOperation,		//Operation
																&lCID,				//lCID
																&lTargetCID,		//lCTargetID
																NULL,				//lIID
																NULL,				//lTID
																NULL,				//llMoneyCount
																NULL,				//csOriginPos
																NULL,				//csGridPos
																NULL );				//lResult

				if( pvPacket != NULL )
				{
					bResult = SendPacket(pvPacket, nPacketLength);
					
					m_csPacket.FreePacket(pvPacket);
				}
			}
		}
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

//Trade��û�� �޾��ش�.
BOOL AgcmPrivateTrade::SendTradeRequestConfirm( INT32 lCID )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_CONFIRM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&lCID,				//lCID
													NULL,				//lCTargetID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength);
		
		m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}


//Trade���!
BOOL AgcmPrivateTrade::SendTradeCancel( INT32 lCID )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

		if( pcsAgpdCharacter != NULL )
		{
			//lCID�� NORMAL�����϶��� �ŷ��� ��û�Ҽ� �ִ�.
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_CANCEL;
			INT16	nPacketLength;

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//lOperation
															&lCID,				//lCID
															NULL,				//lCTargetID
															NULL,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															NULL,				//csOriginPos
															NULL,				//csGridPos
															NULL );				//lResult

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

//���� �ø���.
BOOL AgcmPrivateTrade::SendTradeUpdateMoney( INT32 lCID, INT32 lMoneyCount )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

		if( pcsAgpdCharacter != NULL )
		{
			INT64	llCurrentMoney	= 0;
			if (!m_pagpmCharacter->GetMoney(pcsAgpdCharacter, &llCurrentMoney))
				return FALSE;

			if (llCurrentMoney < lMoneyCount)
				lMoneyCount	= (INT32) llCurrentMoney;

			//lCID�� NORMAL�����϶��� �ŷ��� ��û�Ҽ� �ִ�.
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_UPDATE_MONEY;
			INT16	nPacketLength;

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//lOperation
															&lCID,				//lCID
															NULL,				//lCTargetID
															NULL,				//lIID
															NULL,				//lTID
															&lMoneyCount,		//llMoneyCount
															NULL,				//csOriginPos
															NULL,				//csGridPos
															NULL );				//lResult

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmPrivateTrade::SendTradeUpdateCC(INT32 lCID, INT32 cc)
{
	AgpdCharacter *	pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;
	if( m_pagpmCharacter ) {
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);
		if( pcsAgpdCharacter != NULL ) {
			INT64	current_cc = m_pagpmCharacter->GetCash(pcsAgpdCharacter);
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_UPDATE_SELF_CC;
			INT16	nPacketLength;
			PVOID	pvPacket;

			if (current_cc < cc)
				cc = (INT32)current_cc;
			pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, 
				AGPMPRIVATETRADE_PACKET_TYPE, 
				&nOperation,		//lOperation
				&lCID,				//lCID
				NULL,				//lCTargetID
				NULL,				//lIID
				NULL,				//lTID
				&cc,				//llMoneyCount
				NULL,				//csOriginPos
				NULL,				//csGridPos
				NULL );				//lResult

			if( pvPacket != NULL ) {
				bResult = SendPacket(pvPacket, nPacketLength);
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

//TradeGrid�� Item�� �ִ´�
BOOL AgcmPrivateTrade::SendTradeAddItemToTradeGrid(  eAGPM_PRIVATE_TRADE_ITEM eType , INT32 lCID, INT32 lIID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_ADD_TO_TRADE_GRID;
			INT16	nPacketLength;

			void	*pvOriginPos;
			void	*pvTradeGridPos;

			pvOriginPos  = m_csOriginPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nInvenLayer,
												&nInvenRow,
												&nInvenColumn );

			pvTradeGridPos  = m_csGridPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nTradeLayer,
												&nTradeRow,
												&nTradeColumn );


			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//nOperation
															&lCID,				//lCID
															NULL,				//lCTargetID
															&lIID,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															pvOriginPos,
															pvTradeGridPos,		//csGridPos
															&eType );			//lResult

			if (pvOriginPos)
				m_csPacket.FreePacket(pvOriginPos);
			if (pvTradeGridPos)
				m_csPacket.FreePacket(pvTradeGridPos);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

//TradeGrid���� Item�� �����
BOOL AgcmPrivateTrade::SendTradeRemoveItemFromTradeGrid(INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	// 2008.07.
	if(lStatus != AGPDITEM_STATUS_INVENTORY && lStatus != AGPDITEM_STATUS_SUB_INVENTORY)
		lStatus = AGPDITEM_STATUS_INVENTORY;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TRADE_GRID;
			INT16	nPacketLength;

			void	*pvTradeGrid;
			void	*pvInvenGridPos;

			pvTradeGrid  = m_csOriginPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nTradeLayer,
												&nTradeRow,
												&nTradeColumn );

			pvInvenGridPos  = m_csGridPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nInvenLayer,
												&nInvenRow,
												&nInvenColumn );

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lCTargetID
															&lIID,				//lIID
															NULL,				//lTID
															&lStatus,			//llMoneyCount
															pvTradeGrid,
															pvInvenGridPos,		//csGridPos
															NULL );				//lResult

			if (pvTradeGrid)
				m_csPacket.FreePacket(pvTradeGrid);
			if (pvInvenGridPos)
				m_csPacket.FreePacket(pvInvenGridPos);

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmPrivateTrade::SendTradeLock()
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;
	INT32			lCID = m_pagcmCharacter->m_lSelfCID;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lCID );

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_LOCK;
			INT16	nPacketLength;

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lCTargetID
															NULL,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															NULL,		//csOriginPos
															NULL,			//csGridPos
															NULL );				//lResult

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmPrivateTrade::SendTradeUnlock()
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;
	INT32			lCID = m_pagcmCharacter->m_lSelfCID;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_UNLOCK;
			INT16	nPacketLength;

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lCTargetID
															NULL,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															NULL,		//csOriginPos
															NULL,			//csGridPos
															NULL );				//lResult

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmPrivateTrade::SendReadyToExchange()
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;
	INT32			lCID = m_pagcmCharacter->m_lSelfCID;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_READY_TO_EXCHANGE;
			INT16	nPacketLength;

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lCTargetID
															NULL,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															NULL,		//csOriginPos
															NULL,			//csGridPos
															NULL );				//lResult

			if( pvPacket != NULL )
			{
				bResult = SendPacket(pvPacket, nPacketLength);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgcmPrivateTrade::ProcessLockButton( INT32 lCRequesterID, bool bPushDown )
{
	AgpdCharacter		*pcsAgpdCharacter;

	BOOL				bResult;

	bResult = FALSE;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lCRequesterID );

	if( pcsAgpdCharacter != NULL )
	{
		AgpdItemADChar		*pcsAgpdItemADChar;

		pcsAgpdItemADChar = m_pagpmItem->GetADCharacter( pcsAgpdCharacter );

		if( pcsAgpdItemADChar != NULL )
		{
			pcsAgpdItemADChar->m_bConfirmButtonDown = bPushDown;

			OnUpdateLockButton();
			//CallBack�� ȣ���Ѵ�.
			bResult = EnumCallback( ITEM_CB_ID_UPDATE_LOCK_BUTTON, NULL, NULL );
		}
	}

	return bResult;
}

BOOL AgcmPrivateTrade::ProcessTargetLockButton( INT32 lCRequesterID, bool bPushDown )
{
	BOOL				bResult;

	bResult = FALSE;

	bResult = EnumCallback( ITEM_CB_ID_TRADE_UI_UPDATE_TARGET_LOCK_BUTTON, &bPushDown, NULL );

	return bResult;
}

BOOL AgcmPrivateTrade::ProcessActiveReadyToExchange( INT32 lCRequesterID )
{
	BOOL				bResult;

	bResult = FALSE;

	bResult = EnumCallback( ITEM_CB_ID_TRADE_UI_ACTIVE_READY_TO_EXCHANGE, NULL, NULL );

	return bResult;
}

BOOL AgcmPrivateTrade::ProcessReadyToExchange( INT32 lCRequesterID )
{
	BOOL				bResult;

	bResult = FALSE;

	bResult = EnumCallback( ITEM_CB_ID_TRADE_UI_READY_TO_EXCHANGE, NULL, NULL );

	return bResult;
}

BOOL AgcmPrivateTrade::ProcessClientReadyToExchange( INT32 lCRequesterID )
{
	BOOL				bResult;

	bResult = FALSE;

	bResult = EnumCallback( ITEM_CB_ID_TRADE_UI_CLIENT_READY_TO_EXCHANGE, NULL, NULL );

	return bResult;
}

BOOL AgcmPrivateTrade::ProcessCancel( INT32 lCRequesterID )
{
	AgpdCharacter		*pcsAgpdCharacter;

	BOOL				bResult;

	bResult = FALSE;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lCRequesterID );

	if( pcsAgpdCharacter != NULL )
	{
		AgpdItemADChar		*pcsAgpdItemADChar;

		pcsAgpdItemADChar = m_pagpmItem->GetADCharacter( pcsAgpdCharacter );

		if( pcsAgpdItemADChar != NULL )
		{
			//pcsAgpdItemADChar->ResetTradeStats();
			m_pagpmItem->ResetTradeStats( pcsAgpdItemADChar );

			//CallBack�� ȣ���Ѵ�.

			//�ŷ� UIâ�� �ݴ´�.
			EnumCallback( ITEM_CB_ID_TRADE_CLOSE_UI_PRIVATETRADE, NULL, NULL );
			EnumCallback( ITEM_CB_ID_TRADE_UI_MSG_TRADE_CANCEL, NULL, NULL );
		}
	}

	return bResult;
}

//�ŷ� ���� ��Ŷ�� �޾�����~ ó��~
BOOL AgcmPrivateTrade::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	//�ŷ���� Ȥ�� Error�޽����� ������ش�~ ����غ���~ ����غ���~
	BOOL			bResult;

	INT8			nOperation;
	INT32			lCRequesterID;
	INT32			lCTargetID;
	INT32			lIID;
	INT32			lTID;
	INT32			lMoneyCount;
	void			*pOriginPos;
	void			*pGridPos;
	INT32			lResult;

	bResult = FALSE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&nOperation,
						&lCRequesterID,
						&lCTargetID,
						&lIID,
						&lTID,
						&lMoneyCount,
						&pOriginPos,
						&pGridPos,
						&lResult );

	//Confirm�� ��ٸ�����....
	if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_WAIT_CONFIRM )
	{
		//Confirm�� ��ٸ��� ���̶�� ���¸� ������ ǥ����������ٵ�.....
		//�ŷ��� �Ϸ��ٰ� ������ ���ؼ� ��밡 �����ϱ� ���� ��Ҹ� �Ҽ��� �ְ���?
		//�̷��� SendTradeCancel()�� �����ش�.
		//memset( m_strDebugMessage, 0, sizeof( m_strDebugMessage ) );
		//strcat( m_strDebugMessage, "����� ������ ��ٸ�����" );
	}
	//�������� ������ �ŷ��� ���ڰ� ��û����.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_CONFIRM )
	{
		//UI�� �������� CallBack ȣ��.
		EnumCallback(ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE_YES_NO, &lCTargetID, NULL);

	}
	//Trade�� ���۵Ǿ����� �����κ��� �޴´�.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_START )
	{
		AgpdCharacter		*pcsAgpdCharacter;
		AgpdCharacter		*pcsAgpdTarget;

		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter( lCRequesterID );
		pcsAgpdTarget	 = m_pagpmCharacter->GetCharacter( lCTargetID );

		if( pcsAgpdCharacter != NULL && pcsAgpdTarget )
		{
			AgpdItemADChar		*pcsItemADChar;
			AgpdItemADChar		*pcsItemADTarget;

			pcsItemADChar = m_pagpmItem->GetADCharacter( pcsAgpdCharacter );
			pcsItemADTarget = m_pagpmItem->GetADCharacter( pcsAgpdTarget );

			m_pagpmItem->ResetTradeStats( pcsItemADChar );
			m_pagpmItem->ResetTradeStats( pcsItemADTarget );

			pcsItemADChar->m_lTradeTargetID = lCTargetID;

			EnumCallback(ITEM_CB_ID_TRADE_OPEN_UI_PRIVATETRADE, &lCTargetID, NULL);
		}
	}
	//�ŷ����!
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_CANCEL )
	{
		//��Ȳ�� ���� ��Ҹ� ���ش�. 
		//�������̽��� ���ְų� �� �׷��ϵ���...... Ŭ���̾�Ʈ���� ����� �� �ϵ�......
		ProcessCancel( lCRequesterID );
	}
	//���� �ŷ��� �������� �� �ø��� ok!��ư ��������.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_LOCK)
	{
		bResult = ProcessLockButton( lCRequesterID, true );
	}
	//�ŷ��ڰ� �ŷ��� �������� �� �ø��� ok!��ư ��������.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TARGET_LOCKED)
	{
		//Ŭ���̾�Ʈó��
		bResult = ProcessTargetLockButton( lCRequesterID, true );
	}
	else if( nOperation == AGPMPRIVATETRADE_PACKET_UPDATE_MONEY )
	{
		//OnUpdateMyTradeMoney( lMoneyCount );
		bResult = EnumCallback( ITEM_CB_ID_UPDATE_TRADE_MONEY, &lMoneyCount, NULL );
	}
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TARGET_UPDATE_MONEY )
	{
		//OnUpdateHisTradeMoney( lMoneyCount );
		bResult = EnumCallback( ITEM_CB_ID_UPDATE_TARGET_TRADE_MONEY, &lMoneyCount, NULL );
	}
	//���� Lock ���. -_-;(�������� Grid���� �����ų� �ö�� ��쵵...)
	else if( nOperation == AGPMPRIVATETRADE_PACKET_UNLOCK)
	{
		//Ŭ���̾�Ʈó��
		bResult = ProcessLockButton( lCRequesterID, false );
	}
	//�ŷ��ڰ� Lock ���. -_-;(�������� Grid���� �����ų� �ö�� ��쵵...)
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TARGET_UNLOCKED)
	{
		//Ŭ���̾�Ʈó��
		bResult = ProcessTargetLockButton( lCRequesterID, false );
	}
	else if( nOperation == AGPMPRIVATETRADE_PACKET_ACTIVE_READY_TO_EXCHANGE )
	{
		bResult = ProcessActiveReadyToExchange( lCRequesterID );
	}
	else if( nOperation == AGPMPRIVATETRADE_PACKET_READY_TO_EXCHANGE )
	{
		bResult = ProcessReadyToExchange( lCRequesterID );
	}
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TARGET_READY_TO_EXCHANGE )
	{
		bResult = ProcessClientReadyToExchange( lCRequesterID );
	}
	//��Ÿ ���(������ Error)ó��......
	else if ( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_REFUSE )
	{
		bResult = EnumCallback( ITEM_CB_ID_TRADE_REQUESE_REFUSE_USER, &lCRequesterID, &lCTargetID);
	}
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_RESULT )
	{
		//�ŷ� ����
		if( lResult == AGPMPRIVATETRADE_PACKET_TRADE_END )
		{			//�ŷ��Ϸ�!!
			AgpdCharacter  *pcsAgpdCharacter;
			AgpdItemADChar *pcsAgpdItemADChar;

			pcsAgpdCharacter = GetAgpdCharacter( lCRequesterID );

			if( pcsAgpdCharacter != NULL )
			{
				if( m_pagpmItem != NULL )
				{
					pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

					if( pcsAgpdItemADChar != NULL )
					{
						m_pagpmItem->ResetTradeStats( pcsAgpdItemADChar );

						//�ŷ� UIâ�� �ݴ´�.
						EnumCallback( ITEM_CB_ID_TRADE_CLOSE_UI_PRIVATETRADE, NULL, NULL );
						EnumCallback( ITEM_CB_ID_TRADE_UI_MSG_TRADE_COMPELTE, NULL, NULL );
					}
				}
			}
		}
		//Ÿ���� ���°��
		else if( lResult == AGPMPRIVATETRADE_PACKET_TRADE_RESULT_01 )
		{
//			bResult = CancelTrade( lCID );
		}
		//�ŷ��� �� ���� Ÿ��. 
		else if( lResult == AGPMPRIVATETRADE_PACKET_TRADE_RESULT_02 )
		{
//			bResult = CancelTrade( lCID );
		}
		//�Ÿ��� �־ �ȵǴ°��.
		else if( lResult == AGPMPRIVATETRADE_PACKET_TRADE_RESULT_03 )
		{
//			bResult = CancelTrade( lCID );
		}
		//���� �ŷ��� ���.
		else if( lResult == AGPMPRIVATETRADE_PACKET_TRADE_RESULT_05 )
		{
//			bResult = CancelTrade( lCID );
		}
		//������ �ŷ��� ���.
		else if( lResult == AGPMPRIVATETRADE_PACKET_TRADE_RESULT_06 )
		{
//			bResult = CancelTrade( lCID );
		}
		//�׿�..... ���߿� �߰��Ѵ�.
		else if( lResult == AGPMPRIVATETRADE_PACKET_TRADE_RESULT_07 )
		{
//			bResult = CancelTrade( lCID );
		}
		//�̿��� �����ڵ�� �����Ѵ�~
		else
		{
		}
	}
	else if (nOperation == AGPMPRIVATETRADE_PACKET_UPDATE_SELF_CC) {
		bResult = EnumCallback( ITEM_CB_ID_UPDATE_TRADE_SELF_CC, &lMoneyCount, NULL );
	}
	else if (nOperation == AGPMPRIVATETRADE_PACKET_UPDATE_PEER_CC) {
		bResult = EnumCallback( ITEM_CB_ID_UPDATE_TRADE_PEER_CC, &lMoneyCount, NULL );
	}
	//�̿��� ���� �����Ѵ�.
	else
	{

	}

	return TRUE;
}

/*****************************************************************
*   Function : OnTradeStart
*   Comment  : OnTradeStart
*   Date&Time : 2003-05-02, ���� 4:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmPrivateTrade::OnTradeStart()
{
	if ( NULL == m_pagcmCharacter ) 
	{
		ASSERT( 0 );
		return FALSE;
	}

	AgpdItemADChar*		pcsAgpdItemADChar = m_pagpmItem->GetADCharacter( m_pagcmCharacter->m_pcsSelfCharacter );
	if ( NULL == pcsAgpdItemADChar ) return FALSE;

	AgpdCharacter*		pcsAgpdCharacterTarget = m_pagpmCharacter->GetCharacter( pcsAgpdItemADChar->m_lTradeTargetID );
	if ( NULL == pcsAgpdCharacterTarget ) return FALSE;

	AgpdItemADChar*		pcsAgpdItemADCharTarget = m_pagpmItem->GetADCharacter( pcsAgpdCharacterTarget );
	if ( NULL == pcsAgpdItemADCharTarget ) return FALSE;

	// Tradeconfirm Setting
/*	if ( FALSE == m_clMyTradeconfirm.m_bInitReady )
		m_clMyTradeconfirm.SetInitModulePointer( &(pcsAgpdItemADChar)->m_csTradeGrid, m_pagcmItem );
	else
		m_clMyTradeconfirm.SetReSettingItemGridPointer( &(pcsAgpdItemADChar)->m_csTradeGrid ); 
	
	m_clMyTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lMoneyCount );
	
	if ( true == pcsAgpdItemADChar->m_bConfirmButtonDown ) m_clMyTradeconfirm.m_clConfirmButton.SetButtonMode( 2 );
	else m_clMyTradeconfirm.m_clConfirmButton.SetButtonMode( 0 );

	m_clMyTradeconfirm.SetInventoryItemTexture();

	if ( FALSE == m_clHisTradeconfirm.m_bInitReady )
		m_clHisTradeconfirm.SetInitModulePointer( &(pcsAgpdItemADCharTarget->m_csTradeGrid), m_pagcmItem );
	else 
		m_clHisTradeconfirm.SetReSettingItemGridPointer( &(pcsAgpdItemADCharTarget->m_csTradeGrid) );

	m_clHisTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lClientMoneyCount );

	if ( true == pcsAgpdItemADChar->m_bTargetConfirmButtonDown ) m_clHisTradeconfirm.m_clConfirmButton.SetButtonMode( 2 );
	else m_clHisTradeconfirm.m_clConfirmButton.SetButtonMode( 0 );

	m_clHisTradeconfirm.SetInventoryItemTexture();	*/

	return TRUE;
}

/*****************************************************************
*   Function : OnUpdateConfirmButton
*   Comment  : OnUpdateConfirmButton
*   Date&Time : 2003-05-02, ���� 4:43
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmPrivateTrade::OnUpdateLockButton()
{
	AgpdItemADChar*		pcsAgpdItemADChar = NULL	;
	pcsAgpdItemADChar = m_pagpmItem->GetADCharacter( m_pagcmCharacter->m_pcsSelfCharacter );
	if ( NULL == pcsAgpdItemADChar ) return FALSE;

	// My Tradeconfirm 
//	if ( true == pcsAgpdItemADChar->m_bConfirmButtonDown ) m_clMyTradeconfirm.m_clConfirmButton.SetButtonMode( 2 );
//	else m_clMyTradeconfirm.m_clConfirmButton.SetButtonMode( 0 );		
	
	// His Tradeconfirm �̸� 
//	if ( true == pcsAgpdItemADChar->m_bTargetConfirmButtonDown ) m_clHisTradeconfirm.m_clConfirmButton.SetButtonMode( 2 );
//	else m_clHisTradeconfirm.m_clConfirmButton.SetButtonMode( 0 );
	
	return TRUE;
}

/*****************************************************************
*   Function : OnUpdateTradeMoney
*   Comment  : OnUpdateTradeMoney
*   Date&Time : 2003-05-02, ���� 4:51
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AgcmPrivateTrade::OnUpdateTradeMoney()	
{
	AgpdItemADChar*		pcsAgpdItemADChar;
	pcsAgpdItemADChar = m_pagpmItem->GetADCharacter( m_pagcmCharacter->m_pcsSelfCharacter );
	if ( NULL == pcsAgpdItemADChar ) return FALSE;

//	m_clMyTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lMoneyCount );
//	m_clHisTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lClientMoneyCount );
	
	return TRUE;
}

BOOL	AgcmPrivateTrade::OnUpdateMyTradeMoney( INT32 lMoney )	
{
	AgpdItemADChar*		pcsAgpdItemADChar;
	pcsAgpdItemADChar = m_pagpmItem->GetADCharacter( m_pagcmCharacter->m_pcsSelfCharacter );
	if ( NULL == pcsAgpdItemADChar ) return FALSE;

	pcsAgpdItemADChar->m_lMoneyCount = lMoney;
//	m_clMyTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lMoneyCount );
//	m_clHisTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lClientMoneyCount );
	
	return TRUE;
}

BOOL	AgcmPrivateTrade::OnUpdateHisTradeMoney( INT32 lMoney )
{
	AgpdItemADChar*		pcsAgpdItemADChar;
	pcsAgpdItemADChar = m_pagpmItem->GetADCharacter( m_pagcmCharacter->m_pcsSelfCharacter );
	if ( NULL == pcsAgpdItemADChar ) return FALSE;

	pcsAgpdItemADChar->m_lClientMoneyCount = lMoney;
//	m_clMyTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lMoneyCount );
//	m_clHisTradeconfirm.m_clMoney.SetNumber( pcsAgpdItemADChar->m_lClientMoneyCount );
	
	return TRUE;
}
