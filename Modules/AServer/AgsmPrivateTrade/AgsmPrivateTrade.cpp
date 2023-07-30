#include "AgsmPrivateTrade.h"
#include "AgsmCharManager.h"
#include "AgpmReturnToLogin.h"

AgsmPrivateTrade::AgsmPrivateTrade()
{
	SetModuleName( "AgsmPrivateTrade" );

	SetModuleType(APMODULE_TYPE_SERVER);

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGPMPRIVATETRADE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  ����
							AUTYPE_INT32,			1, // lCID �����
							AUTYPE_INT32,			1, // lTargetCID �����
							AUTYPE_INT32,			1, // lIID       Item ID
							AUTYPE_INT32,			1, // lITID      Item Type ID
							AUTYPE_INT32,			1, // llMoneyCount
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

	m_pagpmLog = NULL;
}

BOOL AgsmPrivateTrade::OnAddModule()
{
	m_pagpmGrid = (AgpmGrid *) GetModule("AgpmGrid");

	m_papmMap = (ApmMap *)GetModule("ApmMap");
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pagsmItem = (AgsmItem *) GetModule("AgsmItem");
	m_pagpmItemConvert = (AgpmItemConvert*)GetModule("AgpmItemConvert");
	//m_pagpmItemLog = (AgpmItemLog*)GetModule("AgpmItemLog");
	m_pagpmLog = (AgpmLog*)GetModule("AgpmLog");

	m_pagsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pagsmCharManager = (AgsmCharManager *) GetModule("AgsmCharManager");
	m_pcsAgpmReturnToLogin = (AgpmReturnToLogin *) GetModule("AgpmReturnToLogin");

	if (!m_papmMap || !m_pagpmCharacter || !m_pagpmFactors || !m_pagpmItem || !m_pagsmCharacter || !m_pagsmCharManager || !m_pcsAgpmReturnToLogin)
		return FALSE;

	if(!m_pagsmItem)// || !m_pagpmItemLog)
		return FALSE;

	if (!m_pagpmItem->SetCallbackRemoveTradeGrid(CBRemoveTradeGrid, this))
		return FALSE;

	if (!m_pagsmCharManager->SetCallbackDisconnectCharacter(CBDisconnect, this))
		return FALSE;

	if (!m_pcsAgpmReturnToLogin->SetCallbackRequest(CBDisconnect, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmPrivateTrade::ParseGridPos( PVOID pGridPos, INT16 *pnTradeLayer, INT16 *pnTradeRow, INT16 *pnTradeCol)
{
	BOOL			bResult;

	bResult = FALSE;

	if( pGridPos != NULL )
	{
		INT16  lTempTradeIndex, lTempTradeRow, lTempTradeColumn;

		m_csGridPos.GetField(FALSE, pGridPos, 0,
									&lTempTradeIndex,
									&lTempTradeRow,
									&lTempTradeColumn);

		if(pnTradeLayer)
		{
			*pnTradeLayer = lTempTradeIndex;
		}

		if(pnTradeRow)
		{
			*pnTradeRow = lTempTradeRow;
		}

		if(pnTradeCol)
		{
			*pnTradeCol = lTempTradeColumn;
		}

		bResult = TRUE;	
	}

	return bResult;
}

//TradeBox���� �κ����� �ű涧~
BOOL AgsmPrivateTrade::AddItemFromTradeBoxToInventory( AgpdCharacter  *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus )
{
	if(lStatus == AGPDITEM_STATUS_SUB_INVENTORY)
		return m_pagpmItem->AddItemToSubInventory(pcsAgpdCharacter, pcsAgpdItem);

	return m_pagpmItem->AddItemToInventory(pcsAgpdCharacter, pcsAgpdItem);
}

//�κ����� TradeBox�� �ű涧~
BOOL AgsmPrivateTrade::AddItemFromInventoryToTradeBox( AgpdCharacter  *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn )
{
	BOOL			bResult;
	BOOL			bCheck;
	AgpdItemADChar *pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

	bResult = FALSE;
	bCheck = FALSE;

	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AAddItemFromTradeBoxToInventory() Error (2) !!!\n");
		return bResult;
	}

	AgpdItemTemplate *pTemplate = m_pagpmItem->GetItemTemplate(pcsAgpdItem->m_lTID);

	if( pTemplate != NULL )
	{
		return m_pagpmItem->AddItemToTradeGrid(pcsAgpdCharacter, pcsAgpdItem, -1, -1, -1);

		/*
		//�κ����� �����.
		m_pagpmItem->RemoveItemFromInventory( pcsAgpdCharacter, pcsAgpdItem );
		//TradeGrid�� �ִ´�.
		m_pagpmItem->AddItemToTradeGrid( pcsAgpdCharacter, pcsAgpdItem );

		bResult = TRUE;
		*/
	}

	return bResult;
}


//�ŷ� ��� ��Ŷ�� lCID���Գ�����.
BOOL AgsmPrivateTrade::SendTradeCancel( INT32 lCID )
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
			//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_CANCEL;
			INT16	nPacketLength;

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,	//Operation
															&lCID,			//lCID
															NULL,			//lTargetID
															NULL,			//lIID
															NULL,			//lTID
															NULL,			//llMoneyCount
															NULL,			//csOriginPos
															NULL,			//csGridPos
															NULL );			//lResult

			if( pvPacket != NULL )
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

				bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

BOOL AgsmPrivateTrade::SendTradeUpdateMoney( AgpdCharacter *pcsAgpdCharacter, INT32 lMoneyCount )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdCharacter != NULL )
	{
		//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
		INT8	nOperation = AGPMPRIVATETRADE_PACKET_UPDATE_MONEY;
		INT16	nPacketLength;

		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
														&nOperation,	//Operation
														&pcsAgpdCharacter->m_lID, //lCID
														NULL,			//lTargetID
														NULL,			//lIID
														NULL,			//lTID
														&lMoneyCount,	//lMoneyCount
														NULL,			//csOriginPos
														NULL,			//csGridPos
														NULL );			//lResult

		if( pvPacket != NULL )
		{
			AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
			
			m_csPacket.FreePacket(pvPacket);
		}
	}

	return bResult;
}

BOOL AgsmPrivateTrade::SendTradeUpdateTargetMoney( AgpdCharacter *pcsAgpdTargetCharacter, INT32 lMoneyCount )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdTargetCharacter != NULL )
	{
		//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
		INT8	nOperation = AGPMPRIVATETRADE_PACKET_TARGET_UPDATE_MONEY;
		INT16	nPacketLength;

		PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
														&nOperation,	//Operation
														&pcsAgpdTargetCharacter->m_lID,	//lCID
														NULL,			//lTargetID
														NULL,			//lIID
														NULL,			//lTID
														&lMoneyCount,	//lMoneyCount
														NULL,			//csOriginPos
														NULL,			//csGridPos
														NULL );			//lResult

		if( pvPacket != NULL )
		{
			AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdTargetCharacter);

			bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
			
			m_csPacket.FreePacket(pvPacket);
		}
	}

	return bResult;
}

//lCID���� �ŷ����(����,����,������)�� �˷��ش�.
BOOL AgsmPrivateTrade::SendTradeResult(AgpdCharacter *pcsAgpdCharacter, INT32 lResult)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_RESULT;
	INT16	nPacketLength = 0;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsAgpdCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													&lResult );			//lResult;

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
		
	m_csPacket.FreePacket(pvPacket);

	return	bResult;
}

//lCID���� �ŷ���û�� �߰� ��ٸ��� ���̶�� �˸���.
BOOL AgsmPrivateTrade::SendWaitConfirm( AgpdCharacter *pcsAgpdCharacter )
{
	if (!pcsAgpdCharacter)
		return FALSE;

	//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_WAIT_CONFIRM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,	//Operation
													&pcsAgpdCharacter->m_lID,			//lCID
													NULL,			//lTargetID
													NULL,			//lIID
													NULL,			//lTID
													NULL,			//llMoneyCount
													NULL,			//csOriginPos
													NULL,			//csGridPos
													NULL );			//lResult;

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
						
	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

/*
	2005.06.01. By SungHoon
	TargetID�� �ŷ��ź����̶�� �˷��ش�.
*/
BOOL AgsmPrivateTrade::SendRequestRefuse(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter)
{
	if (!pcsAgpdCharacter || !pcsAgpdTargetCharacter)
		return FALSE;

	//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_REFUSE;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,	//Opeartion
													&pcsAgpdCharacter->m_lID,			//�ŷ��� ��û�� �༮�� ID�̴�. ��û�޴� �ʿ��� TargetID�� lCID�̴�.
													&pcsAgpdTargetCharacter->m_lID, 
													NULL,			//lIID
													NULL,			//lTID
													NULL,			//llMoneyCount
													NULL,			//csOriginPos
													NULL,			//csGridPos
													NULL );			//lResult;

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}
//TargetID���� lCID�� �ŷ��� ��û�ߴٰ� �˸���.
BOOL AgsmPrivateTrade::SendRequestConfirm(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter)
{
	if (!pcsAgpdCharacter || !pcsAgpdTargetCharacter)
		return FALSE;

	//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_CONFIRM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,	//Opeartion
													&pcsAgpdTargetCharacter->m_lID,	
													&pcsAgpdCharacter->m_lID, //�ŷ��� ��û�� �༮�� ID�̴�. ��û�޴� �ʿ��� TargetID�� lCID�̴�.
													NULL,			//lIID
													NULL,			//lTID
													NULL,			//llMoneyCount
													NULL,			//csOriginPos
													NULL,			//csGridPos
													NULL );			//lResult;

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdTargetCharacter));

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

//Trade�� ���۵Ǿ��ٰ� �˸���.
BOOL AgsmPrivateTrade::SendTradeStart(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter)
{
	//���� lCID�� NORMAL�϶��� Trade ��û�� �� �� �ִ�.
	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TRADE_START;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,	//Operation
													&pcsAgpdCharacter->m_lID,			//lCID
													&pcsAgpdTargetCharacter->m_lID,	//lTargetCID
													NULL,			//lIID
													NULL,			//lTID
													NULL,			//llMoneyCount
													NULL,			//csOriginPos
													NULL,			//csGridPos
													NULL );			//lResult

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

//�� �κ����� Trade Grid�� ����
BOOL AgsmPrivateTrade::SendTradeMoveInventoryToTradeGrid( INT32 lCID, INT32 lIID, AgpdItem *pcsAgpdItem )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID); //�ŷ��� �޾Ƶ��̴����̴ϱ� TargetID�� �´�.

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_ADD_TO_TRADE_GRID;
			INT16	nPacketLength;

			void	*pvTradeGridPos;

			printf( "[Debug:%d]\n", lIID );

			pvTradeGridPos  = m_csGridPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
												&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
												&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN] 
												);

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lTargetCID
															&lIID,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															NULL,				//csOriginPos
															pvTradeGridPos,		//csGridPos
															NULL );				//lResult;

			m_csGridPos.FreePacket(pvTradeGridPos);

			if( pvPacket != NULL )
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

				bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

//�ŷ������κ����� Trade Grid�� ����
BOOL AgsmPrivateTrade::SendTradeMoveTargetInventoryToTargetTradeGrid( INT32 lCID, INT32 lIID, AgpdItem *pcsAgpdItem )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID); //�ŷ��� �޾Ƶ��̴����̴ϱ� TargetID�� �´�.

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_ADD_TO_TARGET_TRADE_GRID;
			INT16	nPacketLength;

			PVOID	pvTradeGridPos;

			pvTradeGridPos  = m_csGridPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
												&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
												&pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN] 
												);

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lTargetCID
															&lIID,				//lIID
															NULL,				//lTID
															NULL,			//llMoneyCount
															NULL,			//csOriginPos
															pvTradeGridPos,		//csGridPos
															NULL );				//lResult

			m_csGridPos.FreePacket(pvTradeGridPos);
			if( pvPacket != NULL )
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

				bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

//�� TradeGrid���� �κ����� �̵��Ҷ�
BOOL AgsmPrivateTrade::SendTradeMoveTradeGridToInventory( INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID); //�ŷ��� �޾Ƶ��̴����̴ϱ� TargetID�� �´�.

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TRADE_GRID;
			INT16	nPacketLength;

			void	*pvTradePos;
			void	*pvInvenPos;

			pvTradePos  = m_csOriginPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nTradeLayer,
												&nTradeRow,
												&nTradeColumn );

			pvInvenPos  = m_csGridPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nInvenLayer,
												&nInvenRow,
												&nInvenColumn );

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lTargetCID
															&lIID,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															pvTradePos,			//csOriginPos
															pvInvenPos,			//csGridPos
															NULL );				//lResult;
			m_csOriginPos.FreePacket(pvTradePos);
			m_csGridPos.FreePacket(pvInvenPos);

			if( pvPacket != NULL )
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

				bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

//�ŷ����� TradeGrid���� �κ����� �̵��Ҷ�
BOOL AgsmPrivateTrade::SendTradeMoveTargetTradeGridToTargetInventory( INT32 lCID, INT32 lIID, INT16 nLayer, INT16 nRow, INT16 nColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn  )
{
	AgpdCharacter  *pcsAgpdCharacter;
	BOOL			bResult;

	pcsAgpdCharacter = NULL;
	bResult = FALSE;

	//ĳ���������͸� ����.
	if( m_pagpmCharacter )
	{
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID); //�ŷ��� �޾Ƶ��̴����̴ϱ� TargetID�� �´�.

		if( pcsAgpdCharacter != NULL )
		{
			INT8	nOperation = AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TARGET_TRADE_GRID;
			INT16	nPacketLength;

			PVOID	pvOriginPos;
			PVOID	pvGridPos;

			pvOriginPos  = m_csOriginPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nLayer,
												&nRow,
												&nColumn );

			pvGridPos  = m_csGridPos.MakePacket(FALSE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
												&nInvenLayer,
												&nInvenRow,
												&nInvenColumn );

			PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
															&nOperation,		//Operation
															&lCID,				//lCID
															NULL,				//lTargetID
															&lIID,				//lIID
															NULL,				//lTID
															NULL,				//llMoneyCount
															pvOriginPos,		//csOriginPos
															pvGridPos,			//csGridPos
															NULL );				//lResult

			m_csOriginPos.FreePacket(pvOriginPos);
			m_csGridPos.FreePacket(pvGridPos);
			if( pvPacket != NULL )
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

				bResult = SendPacket(pvPacket, nPacketLength, pcsAgsdCharacter->m_dpnidCharacter);
				
				m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return bResult;
}

//���� �������� �� �ø��� ok��ư�� ��������.
BOOL AgsmPrivateTrade::SendTradeLock(AgpdCharacter *pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	INT8	nOperation = AGPMPRIVATETRADE_PACKET_LOCK;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsAgpdCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

//��밡 �������� �� �ø��� ok��ư�� ��������.
BOOL AgsmPrivateTrade::SendTradeTargetLocked(AgpdCharacter *pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TARGET_LOCKED;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsAgpdCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
		
	m_csPacket.FreePacket(pvPacket);

	return	bResult;
}

//���� �������� �� �ø��� ok��ư�� ������~ ����ϰ� ������...
BOOL AgsmPrivateTrade::SendTradeUnlock(AgpdCharacter *pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	INT8	nOperation = AGPMPRIVATETRADE_PACKET_UNLOCK;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsAgpdCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult;

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	
	m_csPacket.FreePacket(pvPacket);

	return	bResult;
}

//��밡 �������� �� �ø��� ok��ư�� ������~ ����Ҷ�...
BOOL AgsmPrivateTrade::SendTradeTargetUnlocked(AgpdCharacter *pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TARGET_UNLOCKED;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsAgpdCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmPrivateTrade::SendActiveReadyToExchangeButton(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT8	nOperation = AGPMPRIVATETRADE_PACKET_ACTIVE_READY_TO_EXCHANGE;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsCharacter));
	
	m_csPacket.FreePacket(pvPacket);
	
	return	bResult;
}

//�����ŷ� ������Ŷ�� ������.
BOOL AgsmPrivateTrade::SendReadyToExchange(AgpdCharacter *pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	INT8	nOperation		= AGPMPRIVATETRADE_PACKET_READY_TO_EXCHANGE;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsAgpdCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	
	m_csPacket.FreePacket(pvPacket);

	return	bResult;
}

//��밡 �����ŷ� ������Ŷ�� ���ȴٰ� �˷��ش�.
BOOL AgsmPrivateTrade::SendTargetReadyToExchange(AgpdCharacter *pcsAgpdCharacter)
{
	if (!pcsAgpdCharacter)
		return FALSE;

	INT8	nOperation = AGPMPRIVATETRADE_PACKET_TARGET_READY_TO_EXCHANGE;
	INT16	nPacketLength	= 0;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRIVATETRADE_PACKET_TYPE, 
													&nOperation,		//Operation
													&pcsAgpdCharacter->m_lID,				//lCID
													NULL,				//lTargetCID
													NULL,				//lIID
													NULL,				//lTID
													NULL,				//llMoneyCount
													NULL,				//csOriginPos
													NULL,				//csGridPos
													NULL );				//lResult

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	BOOL	bResult = SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
		
	m_csPacket.FreePacket(pvPacket);

	return	bResult;
}


//(lCID��)�ŷ����¸� �ʱ�ȭ�Ѵ�.
BOOL AgsmPrivateTrade::ProcessTradeCancel( INT32 lCID )
{
	AgpdCharacter  *pcsAgpdCharacter;
	AgpdCharacter  *pcsAgpdTargetCharacter;
	AgpdItemADChar *pcsAgpdItemADChar;
	AgpdItemADChar *pcsAgpdItemADTargetChar;

	BOOL			bResult;
	INT32			lTargetCID;

	pcsAgpdCharacter = NULL;
	pcsAgpdTargetCharacter = NULL;
	pcsAgpdItemADChar = NULL;
	pcsAgpdItemADTargetChar = NULL;

	bResult = FALSE;
	lTargetCID = 0;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacterLock(lCID);

	if( pcsAgpdCharacter != NULL )
	{
		pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

		//���� �ŷ����̳� �ŷ��غ����϶�...�� üũ�� �����~
		if( (pcsAgpdItemADChar != NULL) )
		{
			lTargetCID = pcsAgpdItemADChar->m_lTradeTargetID;

			pcsAgpdTargetCharacter = m_pagpmCharacter->GetCharacterLock( lTargetCID );

			if( pcsAgpdTargetCharacter != NULL )
			{
				pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);
			}
		}
	}

	if (!pcsAgpdCharacter || !pcsAgpdTargetCharacter)
		return FALSE;

	if (pcsAgpdCharacter && m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) ||
		pcsAgpdTargetCharacter && m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	//�ھ� ����~ Grid� �ö󰬴ٴ��� �ϴ� �͵��� ���� ���󺹱��Ѵ�.
	RestoreTradeItems( pcsAgpdCharacter, pcsAgpdItemADChar, pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar );
	RestoreTradeItems( pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar, pcsAgpdCharacter, pcsAgpdItemADChar );

	//�ŷ����¸� �ʱ�ȭ��Ų��.
	ResetTradeStatus( pcsAgpdCharacter, pcsAgpdItemADChar );
	ResetTradeStatus( pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar );

	//�ŷ� �ɼ�â�� ���õ� �͵鵵 �� �ʱ�ȭ �����ش�.
	ResetPrivateTradeOptionGrid(pcsAgpdCharacter, pcsAgpdItemADChar);
	ResetPrivateTradeOptionGrid(pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar);


	//�ѿ��� �ŷ� ��� ��Ŷ�� ������.
	if (pcsAgpdCharacter && pcsAgpdTargetCharacter)
	{
		if( lCID != 0 )
			SendTradeCancel( lCID );

		if( lTargetCID != 0 )
			SendTradeCancel( lTargetCID );
	}

	if (pcsAgpdTargetCharacter)
		pcsAgpdTargetCharacter->m_Mutex.Release();
	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}
//JK_�ŷ��߱���
//(lCID��)�ŷ����¸� �ʱ�ȭ�Ѵ�...����ġ ���� ��Ȳ�� �ŷ� ���...
BOOL AgsmPrivateTrade::ProcessTradeCancelUnExpected( INT32 lCID )
{
	AgpdCharacter  *pcsAgpdCharacter;
	AgpdCharacter  *pcsAgpdTargetCharacter;
	AgpdItemADChar *pcsAgpdItemADChar;
	AgpdItemADChar *pcsAgpdItemADTargetChar;

	BOOL			bResult;
	INT32			lTargetCID;

	pcsAgpdCharacter = NULL;
	pcsAgpdTargetCharacter = NULL;
	pcsAgpdItemADChar = NULL;
	pcsAgpdItemADTargetChar = NULL;

	bResult = FALSE;
	lTargetCID = 0;

	pcsAgpdCharacter = m_pagpmCharacter->GetCharacterLock(lCID);

	if( pcsAgpdCharacter != NULL )
	{
		pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

		//���� �ŷ����̳� �ŷ��غ����϶�...�� üũ�� �����~
		if( (pcsAgpdItemADChar != NULL) )
		{
			lTargetCID = pcsAgpdItemADChar->m_lTradeTargetID;

			pcsAgpdTargetCharacter = m_pagpmCharacter->GetCharacterLock( lTargetCID );

			if( pcsAgpdTargetCharacter != NULL )
			{
				pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);
			}
		}
	}

	if (pcsAgpdCharacter && m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) ||
		pcsAgpdTargetCharacter && m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	if (pcsAgpdCharacter && pcsAgpdItemADChar)
	{
		RestoreTradeItemsMyself( pcsAgpdCharacter, pcsAgpdItemADChar );

		ResetTradeStatus( pcsAgpdCharacter, pcsAgpdItemADChar);

		ResetPrivateTradeOptionGrid(pcsAgpdCharacter, pcsAgpdItemADChar);

		if( lCID != 0 )
			SendTradeCancel( lCID );

		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();
	}

	if (pcsAgpdTargetCharacter && pcsAgpdItemADTargetChar)
	{
		RestoreTradeItemsMyself(pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar );

		ResetTradeStatus( pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar );

		ResetPrivateTradeOptionGrid(pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar);

		if( lTargetCID != 0 )
			SendTradeCancel( lTargetCID );

		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();

	}

	return TRUE;
}


BOOL AgsmPrivateTrade::ProcessTradeUpdateMoney( INT32 lCID, INT32 lMoneyCount )
{
	BOOL			bResult	= FALSE;;
	INT32			lTargetCID	= AP_INVALID_CID;

	if (!m_pagpmItem)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter = m_pagpmCharacter->GetCharacterLock( lCID );

	if( pcsAgpdCharacter == NULL ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter))
	{
		return bResult;
	}
	else
	{
		AgpdItemADChar *pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

		//���� �ŷ����̳� �ŷ��غ����϶�...�� üũ�� �����~
		lTargetCID = pcsAgpdItemADChar->m_lTradeTargetID;

		AgpdCharacter *pcsAgpdTargetCharacter = m_pagpmCharacter->GetCharacterLock( lTargetCID );
		if (!pcsAgpdTargetCharacter || m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
		{
			pcsAgpdCharacter->m_Mutex.Release();
			return FALSE;
		}

		AgpdItemADChar *pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

		//99999999���� ���� �ִ°��.
		if( lMoneyCount >= 999999999 )
		{
			lMoneyCount = 999999999;
		}

		//Target�� �ŷ����̰ų� �ŷ��غ����϶�...�� üũ�� �����~
		INT64			llMoney;
		m_pagpmCharacter->GetMoney( pcsAgpdCharacter, &llMoney );
		
		if( lMoneyCount > llMoney )
		{
			lMoneyCount = (INT32)llMoney;
		}

		//���� �ٵ�.
		pcsAgpdItemADChar->m_lMoneyCount = lMoneyCount;
		//���濡�� �������������Ѵ�.
		pcsAgpdItemADTargetChar->m_lClientMoneyCount = lMoneyCount;

		SendTradeUpdateMoney( pcsAgpdCharacter, lMoneyCount );
		SendTradeUpdateTargetMoney( pcsAgpdTargetCharacter, lMoneyCount );

		pcsAgpdTargetCharacter->m_Mutex.Release();
		pcsAgpdCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmPrivateTrade::RestoreTradeItems( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar )
{
	BOOL			bResult;

	bResult = FALSE;

	if( (pcsAgpdCharacter != NULL) && (pcsAgpdItemADChar != NULL) )
	{
		AgpdItem		*pcsAgpdItem;

		AgpdGridItem	*pcsGridItem;

		//�������� ���󺹱��Ѵ�.
		for( int iLayer=0; iLayer<AGPMITEM_TRADEBOX_LAYER; ++iLayer )
		{
			for( int iColumn=0; iColumn<AGPMITEM_TRADEBOX_COLUMN; ++iColumn )
			{
				for( int iRow=0; iRow<AGPMITEM_TRADEBOX_ROW; ++iRow )
				{
					pcsGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csTradeGrid, iLayer, iRow, iColumn );

					if( pcsGridItem != NULL )
					{
						pcsAgpdItem = m_pagpmItem->GetItem( pcsGridItem );

						if( pcsAgpdItem != NULL )
						{
							//TradeGrid���� ����...
							m_pagpmItem->RemoveItemFromTradeGrid( pcsAgpdCharacter, pcsAgpdItem );
							//Inven�� AutoInsert�� �ٽ� �ִ´�.
							if(m_pagpmItem->AddItemToInventory( pcsAgpdCharacter, pcsAgpdItem ) == FALSE)
							{
								// 2008.07.03. steeple
								// If you fail in previous job, insert item to a sub inventory.
								m_pagpmItem->AddItemToSubInventory( pcsAgpdCharacter, pcsAgpdItem );
							}

							if( (pcsAgpdTargetCharacter != NULL ) && (pcsAgpdItemADTargetChar != NULL) )
							{
								//Target���Դ� �������� ������ �˸���.
								AgsdCharacter *pcsAgsdTargetCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdTargetCharacter);
							}
						}
					}
				}
			}
		}
				
		//�ŷ����¸� �ʱ�ȭ�Ѵ�.
		ResetTradeStatus( pcsAgpdCharacter, pcsAgpdItemADChar );

		bResult = TRUE;
	}
	
	return bResult;
}

//JK_�ŷ��߱���..���� ������� �� �ŷ� ���¸� ����ϰ� �κ� ����
BOOL AgsmPrivateTrade::RestoreTradeItemsMyself( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar)
{
	BOOL			bResult;

	bResult = FALSE;

	if( (pcsAgpdCharacter != NULL) && (pcsAgpdItemADChar != NULL) )
	{
		AgpdItem		*pcsAgpdItem;

		AgpdGridItem	*pcsGridItem;

		//�������� ���󺹱��Ѵ�.
		for( int iLayer=0; iLayer<AGPMITEM_TRADEBOX_LAYER; ++iLayer )
		{
			for( int iColumn=0; iColumn<AGPMITEM_TRADEBOX_COLUMN; ++iColumn )
			{
				for( int iRow=0; iRow<AGPMITEM_TRADEBOX_ROW; ++iRow )
				{
					pcsGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csTradeGrid, iLayer, iRow, iColumn );

					if( pcsGridItem != NULL )
					{
						pcsAgpdItem = m_pagpmItem->GetItem( pcsGridItem );

						if( pcsAgpdItem != NULL )
						{
							//TradeGrid���� ����...
							m_pagpmItem->RemoveItemFromTradeGrid( pcsAgpdCharacter, pcsAgpdItem );
							//Inven�� AutoInsert�� �ٽ� �ִ´�.
							if(m_pagpmItem->AddItemToInventory( pcsAgpdCharacter, pcsAgpdItem ) == FALSE)
							{
								// 2008.07.03. steeple
								// If you fail in previous job, insert item to a sub inventory.
								m_pagpmItem->AddItemToSubInventory( pcsAgpdCharacter, pcsAgpdItem );
							}
						}
					}
				}
			}
		}

		//�ŷ����¸� �ʱ�ȭ�Ѵ�.
		ResetTradeStatus( pcsAgpdCharacter, pcsAgpdItemADChar );

		bResult = TRUE;
	}

	return bResult;
}

//�ŷ���û�� ó�����ش�.
BOOL AgsmPrivateTrade::ProcessTradeRequest(INT32 lCID, INT32 lTargetCID)
{
	//���� �𳢸� �ŷ��� ���� ����.
	if (lCID == lTargetCID)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter		= m_pagpmCharacter->GetCharacterLock(lCID);
	AgpdCharacter	*pcsAgpdTargetCharacter	= m_pagpmCharacter->GetCharacterLock(lTargetCID);

	// 2005.03.20. steeple
	// �׾��ִ� ���� �ŷ� ��û�� ������ ���Ѵ�.
	if (!pcsAgpdCharacter || m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) ||
		!pcsAgpdTargetCharacter || m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter) ||
		!m_pagpmCharacter->IsPC(pcsAgpdTargetCharacter) ||
		pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD || pcsAgpdTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
		m_pagpmCharacter->HasPenalty(pcsAgpdCharacter, AGPMCHAR_PENALTY_PRVTRADE) ||
		m_pagpmCharacter->IsInJail(pcsAgpdCharacter) || m_pagpmCharacter->IsInJail(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	// Ban list���� �ŷ� ������ Ȱ��ȭ�� ��� �ŷ��� �ߴ��Ѵ�.
	//�ŷ��źλ������� Ȯ���Ѵ�.	2005.06.01. By SungHoon
	if (FALSE == EnumCallback(AGPMPRIVATETRADE_CALLBACK_TRADE_BUDDY_CHECK, pcsAgpdCharacter, pcsAgpdTargetCharacter) ||
		m_pagpmCharacter->IsOptionFlag(pcsAgpdTargetCharacter, AGPDCHAR_OPTION_REFUSE_TRADE) == TRUE)
	{
		SendRequestRefuse(pcsAgpdCharacter, pcsAgpdTargetCharacter);

		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	//�켱 NORMAL���¿����Ѵ�.
	if( m_pagpmCharacter->UpdateActionStatus( pcsAgpdCharacter, AGPDCHAR_STATUS_TRADE, TRUE ) && m_pagpmCharacter->UpdateActionStatus( pcsAgpdTargetCharacter, AGPDCHAR_STATUS_TRADE, TRUE ) )
	{
		//����... Trade�غ������� Ȯ���Ѵ�.
		AgpdItemADChar *pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);
		AgpdItemADChar *pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

		//�� �� �ŷ� ���°� �ƴ϶��?
		if( (pcsAgpdItemADChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_NONE) && (pcsAgpdItemADTargetChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_NONE) )
		{
			//�ŷ� ��û�ڿ��� ��ٸ�����~ �̶�� �޽�����~
			if( SendWaitConfirm( pcsAgpdCharacter ) )
			{
				//�ŷ� �غ� ���·� �ٲٰ� �ŷ� ��븦 �����Ѵ�.
				pcsAgpdItemADChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_WAIT_CONFIRM;
				pcsAgpdItemADChar->m_lTradeTargetID = lTargetCID;

				//�ŷ��ڿ��Դ� ���� �ŷ��� ��û�ߴ�~ ��� ������ �����ش�.
				if( SendRequestConfirm( pcsAgpdCharacter, pcsAgpdTargetCharacter ) )
				{
					//�ŷ� �غ� ���·� �ٲٰ� �ŷ� ��븦 �����Ѵ�.
					pcsAgpdItemADTargetChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_WAIT_CONFIRM;
					pcsAgpdItemADTargetChar->m_lTradeTargetID = lCID;
				}
				else
				{
					if (pcsAgpdTargetCharacter)
						pcsAgpdTargetCharacter->m_Mutex.Release();
					if (pcsAgpdCharacter)
						pcsAgpdCharacter->m_Mutex.Release();

					return FALSE;
				}
			}
			else
			{
				if (pcsAgpdTargetCharacter)
					pcsAgpdTargetCharacter->m_Mutex.Release();
				if (pcsAgpdCharacter)
					pcsAgpdCharacter->m_Mutex.Release();

				return FALSE;
			}
		}
	}

	if (pcsAgpdTargetCharacter)
		pcsAgpdTargetCharacter->m_Mutex.Release();
	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	//������ ������ �� �ȰŴ�~ ��.��
	return TRUE;
}

//�ŷ��ڷκ��� Trade�� �ϰڴٴ� ������ȣ�� �Դ�.
BOOL AgsmPrivateTrade::ProcessTradeConfirm(INT32 lCID)
{
	if (!m_pagpmItem)
		return FALSE;

	AgpdCharacter *pcsAgpdCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter || m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) ||
		m_pagpmCharacter->HasPenalty(pcsAgpdCharacter, AGPMCHAR_PENALTY_PRVTRADE))
		return FALSE;

	AgpdItemADChar	*pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

	AgpdCharacter	*pcsAgpdTargetCharacter = m_pagpmCharacter->GetCharacterLock(pcsAgpdItemADChar->m_lTradeTargetID);
	if (!pcsAgpdTargetCharacter || m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	//�켱 Trade���¿����Ѵ�.
	if( (pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) && (pcsAgpdTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) )
	{
		//����... Trade�غ������� Ȯ���Ѵ�.
		AgpdItemADChar *pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

		//�ŷ� ��밡 �´��� Ȯ���Ѵ�.
		if( (pcsAgpdItemADChar->m_lTradeTargetID == pcsAgpdTargetCharacter->m_lID) && (pcsAgpdItemADTargetChar->m_lTradeTargetID == lCID) )
		{
			//�� �� �ŷ��غ� ���¶��~
			if( (pcsAgpdItemADChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_WAIT_CONFIRM) && (pcsAgpdItemADTargetChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_WAIT_CONFIRM) )
			{
				//�ŷ��� ���۵Ǿ��ٰ� �˸���.
				if (SendTradeStart(pcsAgpdCharacter, pcsAgpdTargetCharacter))
				{
					//1:1�ŷ� ���·� �ٲ۴�.
					pcsAgpdItemADChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_TRADING;

					//�ŷ��� ���۵Ǿ��ٰ� �˸���.
					if (SendTradeStart(pcsAgpdTargetCharacter, pcsAgpdCharacter))
					{
						//1:1�ŷ� ���·� �ٲ۴�.
						pcsAgpdItemADTargetChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_TRADING;

						// �ɼ�â�� ���� �ִٸ� �ʱ�ȭ �����ش�.
						ResetPrivateTradeOptionGrid(pcsAgpdCharacter, pcsAgpdItemADChar);
						ResetPrivateTradeOptionGrid(pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar);
					}
					else
					{
					}
				}
				else
				{
				}
			}
			else
			{
				if (pcsAgpdTargetCharacter)
					pcsAgpdTargetCharacter->m_Mutex.Release();
				if (pcsAgpdCharacter)
					pcsAgpdCharacter->m_Mutex.Release();

				return FALSE;
			}
		}
	}

	if (pcsAgpdTargetCharacter)
		pcsAgpdTargetCharacter->m_Mutex.Release();
	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	//������ ������ �� �ȰŴ�~ ��.��
	return TRUE;
}

BOOL AgsmPrivateTrade::ProcessAddToTradeGrid( INT32 lCID, INT32 lIID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn )
{
	if (!m_pagpmItem)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter))
	{
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);
	
	AgpdCharacter	*pcsAgpdTargetCharacter	= m_pagpmCharacter->GetCharacterLock(pcsAgpdItemADChar->m_lTradeTargetID);
	if (!pcsAgpdTargetCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

	AgpdItem	*pcsAgpdItem = m_pagpmItem->GetItem(lIID);
	if (!pcsAgpdItem ||
		(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType) &&
		pcsAgpdItem->m_lCashItemUseCount > 0 &&
		m_pagpmItem->IsUsingStamina(pcsAgpdItem->m_pcsItemTemplate) == FALSE) ||
		(pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		 pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_SUB_INVENTORY &&
		 pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY))	// �κ��丮�� �ִ� ������ Ȯ���Ѵ�. 2006.03.23. steeple.
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	// �����ֹ����� ���ؼ� �ͼ� �����۵� �ŷ�â�� �ø��� �ְ� �Ǿ���.
	if(m_pagpmItem->IsBoundOnOwner(pcsAgpdItem) || m_pagpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND)
	{
		// �ͼ��ε� �̺�Ʈ �������̸� �ŷ��� �Ұ����ϴ�.
		if(m_pagpmItem->IsEventItem(pcsAgpdItem))
		{
			if (pcsAgpdTargetCharacter)
				pcsAgpdTargetCharacter->m_Mutex.Release();
			if (pcsAgpdCharacter)
				pcsAgpdCharacter->m_Mutex.Release();

			return FALSE;
		}

		INT32 nCount = NumberOfBoundOnOwnerItemInTradeGrid(pcsAgpdItemADChar);	
		// �ͼӾ������� �ϳ��� �ö� �ִٸ� ���̻� �ͼ� �������� �ŷ��Ҽ� ����.
		if(nCount != 0)
		{
			if (pcsAgpdTargetCharacter)
				pcsAgpdTargetCharacter->m_Mutex.Release();
			if (pcsAgpdCharacter)
				pcsAgpdCharacter->m_Mutex.Release();
			
			return FALSE;
		}

		BOOL	bHaveTradeOptionData = FALSE;
		
		for(int i=0; i<AGPDITEM_PRIVATE_TRADE_OPTION_MAX; i++)
		{
			if(pcsAgpdItemADChar->m_stTradeOptionItem[i].m_lItemIID != 0)
			{
				if(pcsAgpdItemADChar->m_stTradeOptionItem[i].m_bUsed == TRUE || pcsAgpdItemADChar->m_stTradeOptionItem[i].m_bOnGrid == FALSE) 
					continue;

				AgpdItem *pcsTradeOptionItem = m_pagpmItem->GetItem(pcsAgpdItemADChar->m_stTradeOptionItem[i].m_lItemIID);
				if(pcsTradeOptionItem == NULL)
					continue;

				if(pcsTradeOptionItem->m_eStatus == AGPDITEM_STATUS_TRADE_OPTION_GRID && pcsTradeOptionItem->m_pcsItemTemplate &&
				   pcsTradeOptionItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE && 
				   ((AgpdItemTemplateUsable*)pcsTradeOptionItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION)
				{
					// ������� �������� ���� �ŷ��ɼ�â�� �����ֹ����� �ִ°Ŵ�
					AgpdItem *pcsItem = m_pagpmItem->GetExistPrivateTradeOptionItem(pcsAgpdCharacter, pcsAgpdItemADChar->m_stTradeOptionItem[i].m_lItemIID);
					if(pcsItem && pcsItem->m_lID == pcsTradeOptionItem->m_lID)
					{
						bHaveTradeOptionData = TRUE;
						break;
					}
				}
			}
		}

		if(bHaveTradeOptionData == FALSE)
		{
			if (pcsAgpdTargetCharacter)
				pcsAgpdTargetCharacter->m_Mutex.Release();
			if (pcsAgpdCharacter)
				pcsAgpdCharacter->m_Mutex.Release();

			return FALSE;
		}
	}

	// ĳ�� �������߿� ���� ������� �������� �ŷ��Ұ����̴�.
	if( IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType) &&
		m_pagpmItem->IsUsingItem(pcsAgpdItem) == TRUE)
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	// 2006.03.23. steeple
	// �ٽ��ѹ� �κ� �׸����� �������̶� ���� ���� Ȯ���غ���.
	AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csInventoryGrid, nInvenLayer, nInvenRow, nInvenColumn);
	if(!pcsGridItem || pcsGridItem->m_lItemID != lIID)
	{
		pcsGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csCashInventoryGrid, nInvenLayer, nInvenRow, nInvenColumn);
		if(!pcsGridItem || pcsGridItem->m_lItemID != lIID)
		{
			// One more time
			pcsGridItem = m_pagpmGrid->GetItem(&pcsAgpdItemADChar->m_csSubInventoryGrid, nInvenLayer, nInvenRow, nInvenColumn);
			if(!pcsGridItem || pcsGridItem->m_lItemID != lIID)
			{
				if (pcsAgpdTargetCharacter)
					pcsAgpdTargetCharacter->m_Mutex.Release();
				if (pcsAgpdCharacter)
					pcsAgpdCharacter->m_Mutex.Release();

				return FALSE;
			}
		}
	}

	//�κ����� �������� ����� TradeGrid�� �������� �ø���.
	if( AddItemFromInventoryToTradeBox( pcsAgpdCharacter, pcsAgpdItem, nInvenLayer, nInvenRow, nInvenColumn, nTradeLayer, nTradeRow, nTradeColumn ) )
	{
		//lCID���� �κ����� TradeGrid�� �������� �Ű�ٰ� �˷��ش�.
		//SendTradeMoveInventoryToTradeGrid( lCID, lIID, pcsAgpdItem );

		//�ŷ��ڿ��� �� ������ ������ �����ش�.
		m_pagsmItem->SendPacketItem( pcsAgpdItem, m_pagsmCharacter->GetCharDPNID(pcsAgpdTargetCharacter));
	}

	if (pcsAgpdTargetCharacter)
		pcsAgpdTargetCharacter->m_Mutex.Release();
	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmPrivateTrade::ProcessRemoveFromTradeGrid( INT32 lCID, INT32 lIID, INT16 nTradeLayer, INT16 nTradeRow, INT16 nTradeColumn, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT32 lStatus )
{
	if (!m_pagpmItem)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter))
	{
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	// Check a privilege of a sub inventory 2008.07.07. steeple
	if(lStatus == AGPDITEM_STATUS_SUB_INVENTORY && m_pagpmItem->IsEnableSubInventory(pcsAgpdCharacter) == FALSE)
	{
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

	AgpdCharacter	*pcsAgpdTargetCharacter	= m_pagpmCharacter->GetCharacterLock(pcsAgpdItemADChar->m_lTradeTargetID);
	if (!pcsAgpdTargetCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

	AgpdItem	*pcsAgpdItem = m_pagpmItem->GetItem(lIID);
	if (!pcsAgpdItem)
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	//TradeGrid���� ����� �κ����� �������� �ø���.
	BOOL	bResult	= AddItemFromTradeBoxToInventory( pcsAgpdCharacter, pcsAgpdItem, nTradeLayer, nTradeRow, nTradeColumn, nInvenLayer, nInvenRow, nInvenColumn, lStatus );

	if (pcsAgpdTargetCharacter)
		pcsAgpdTargetCharacter->m_Mutex.Release();
	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	return bResult;
}

BOOL AgsmPrivateTrade::TestGridCopyInven(AgpdItemADChar *pcsAgpdItemADChar, AgpdGrid *pcTradeGrid)
{
	AgpdItem		*pcsAgpdItem;

	BOOL			bResult;
	INT32			i;
	AgpdGridItem	*pcsGridItem;

	bResult = TRUE;

	//�κ��� �������� TradeGrid�� �����Ѵ�.
	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csInventoryGrid, &i))
	{
		pcsAgpdItem = m_pagpmItem->GetItem( pcsGridItem );
		if( pcsAgpdItem )
		{
			bResult = m_pagpmGrid->Add(pcTradeGrid,
							pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
							pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
							pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
							pcsAgpdItem->m_pcsGridItem,
							1,
							1 );

			if( bResult == FALSE )
				break;
		}
	}

	return bResult;
}

BOOL AgsmPrivateTrade::TestGridCopyTradeGrid(AgpdItemADChar *pcsAgpdItemADChar, AgpdGrid *pcTradeGrid)
{
	BOOL			bResult;
//	INT16			iStartLayer, iStartRow, iStartColumn;
//	INT16			iLayer, iRow, iColumn;
	INT32			i;
	AgpdGridItem	*pcsGridItem;
	AgpdItem		*pcsItem;

/*	iStartLayer = 0;
	iStartRow = 0;
	iStartColumn = 0;*/

	bResult = TRUE;

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csTradeGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csTradeGrid, &i))
	{
		pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if( pcsItem )
		{
/*			bResult = m_pagpmGrid->Add( pcTradeGrid,
										iStartLayer,
										iStartRow,
										iStartColumn,
										&iLayer,
										&iRow,
										&iColumn,
										pcsItem->m_pcsGridItem,
										1,
										1 );*/

			bResult = m_pagpmGrid->Add( pcTradeGrid,
										pcsItem->m_pcsGridItem,
										1,
										1 );

			if( bResult == FALSE )
			{
				break;
			}
			else //bResult == TRUE )
			{
/*				iStartLayer = iLayer;
				iStartRow = iRow;
				iStartColumn = iColumn;*/
			}
		}
	}

	return bResult;
}

BOOL AgsmPrivateTrade::MoveTradeGridItemToInventory( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, INT32 lTimeStamp )
{
	AgpdItem		*pcsAgpdItem;

	BOOL			bResult;
	INT32			i;
	INT16			nTradeLayer, nTradeRow, nTradeColumn;
	INT16			nClientTradeLayer, nClientTradeRow, nClientTradeColumn;

	AgpdGridItem	*pcsGridItem;

	bResult = TRUE;

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csTradeGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csTradeGrid, &i))
	{
		pcsAgpdItem = m_pagpmItem->GetItem( pcsGridItem );
		if( pcsAgpdItem && pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID )
		{
			nTradeLayer = pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
			nTradeRow = pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
			nTradeColumn = pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];
			nClientTradeLayer = 0;
			nClientTradeRow = 0;
			nClientTradeColumn = 0;

			BOOL	 bHavePrivateTradeItem = FALSE;
			AgpdItem *pcsTradeOptionItem = NULL;

			// Trade Grid�� ���鼭 ���� �������� �ͼ��̸� �����ֹ����� �ִ��� Ȯ���� �ؾ��Ѵ�.
			if(m_pagpmItem->IsBoundOnOwner(pcsAgpdItem) || m_pagpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND)
			{
				for(int i=0; i<AGPDITEM_PRIVATE_TRADE_OPTION_MAX; i++)
				{
					if(pcsAgpdItemADChar->m_stTradeOptionItem[i].m_lItemIID != 0)
					{
						if(pcsAgpdItemADChar->m_stTradeOptionItem[i].m_bUsed == TRUE || 
						   pcsAgpdItemADChar->m_stTradeOptionItem[i].m_bOnGrid == FALSE) 
							continue;

						pcsTradeOptionItem = m_pagpmItem->GetItem(pcsAgpdItemADChar->m_stTradeOptionItem[i].m_lItemIID);
						if(pcsTradeOptionItem == NULL)
							continue;

						if(pcsTradeOptionItem->m_eStatus == AGPDITEM_STATUS_TRADE_OPTION_GRID && pcsTradeOptionItem->m_pcsItemTemplate &&
							pcsTradeOptionItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE && 
							((AgpdItemTemplateUsable*)pcsTradeOptionItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION)
						{
							// ������� �������� ���� �ŷ��ɼ�â�� �����ֹ����� �ִ°Ŵ�.
							// ��� �� �����ֹ����� ����ؼ� �ͼ����� �ŷ��Ѵ�.
							AgpdItem *pcsItem = m_pagpmItem->GetExistPrivateTradeOptionItem(pcsAgpdCharacter, pcsAgpdItemADChar->m_stTradeOptionItem[i].m_lItemIID);
							if(pcsItem && pcsItem->m_lID == pcsTradeOptionItem->m_lID)
							{
								pcsTradeOptionItem = m_pagpmItem->GetItem(pcsAgpdItemADChar->m_stTradeOptionItem[i].m_lItemIID);
								if(pcsTradeOptionItem)
								{
									pcsAgpdItemADChar->m_stTradeOptionItem[i].m_bUsed = TRUE;
									bHavePrivateTradeItem = TRUE;
								}
								break;
							}
						}
					}
				}
			}
			if(m_pagpmItem->IsBoundOnOwner(pcsAgpdItem) || m_pagpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND)
			{
				if(bHavePrivateTradeItem == FALSE)
					continue;
			}

			/* ReleaseItem���� Callback �޾Ƽ� ������. �ϴ��� RemoveItem Packet ���� ������ ó��... ���� �Ͼ�� �ٲ���.*/
			AgsdCharacter *pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
			m_pagsmItem->SendPacketItemRemove( pcsAgpdItem->m_lID, pcsAgsdCharacter->m_dpnidCharacter );

			//Auto Insert�� �κ��� �ִ´�.
			if(m_pagpmItem->AddItemToInventory( pcsAgpdTargetCharacter, pcsAgpdItem ))
			{
				//����� �κ��丮�� ������ �����ֹ����� ���ش�.
				if(bHavePrivateTradeItem == TRUE && pcsTradeOptionItem)
				{
					BOOL bSuccess = m_pagsmItem->UseItemPrivateTradeOption(pcsTradeOptionItem);
					if(bSuccess)
					{
						CHAR szLog[MAX_PATH] = {0,};
						SYSTEMTIME st;
						GetLocalTime(&st);
						sprintf_s(szLog, sizeof(szLog), "LOG\\BoundOnOwnerItemTrade-%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

						// �����̸� �α׸� �����ش�.
						CHAR strCharBuff[512] = {0,};
						sprintf_s(strCharBuff, sizeof(strCharBuff), "Seller=[%s] Buyer=[%s] Item=[%s]\n",
								  pcsAgpdCharacter->m_szID, pcsAgpdTargetCharacter->m_szID, 
								  pcsAgpdItem->m_pcsItemTemplate->m_szName);
						AuLogFile_s(szLog, strCharBuff);

						// DB���� �α׸� �����ش�
						m_pagsmItem->WriteUseLog(pcsTradeOptionItem->m_pcsCharacter->m_lID, pcsTradeOptionItem);
					}
					else
					{
						// �����ϸ� �ٽ� ���� ������ �κ����ٰ� �־��ش�.
						m_pagpmItem->AddItemToInventory( pcsAgpdCharacter, pcsAgpdItem);
					}
				}
			}
			else
				continue;

			if( bResult == FALSE )
			{
				break;
			}

			// 2004.05.04. steeple
			WriteTradeItemLog(pcsAgpdCharacter, pcsAgpdTargetCharacter, pcsAgpdItem, lTimeStamp);
		}
	}

	return bResult;
}

BOOL AgsmPrivateTrade::ProcessExchangeItem(AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter *pcsAgpdTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar )
{
	AgpdGrid		cTradeGrid, TargetCTradeGrid;

	BOOL			bResult;
	BOOL			bCheckMoney;
	INT64			llMoneyCount, llTargetMoneyCount;

	bCheckMoney = FALSE;
	bResult = FALSE;

	//���� ���� ����صд�.
	m_pagpmCharacter->GetMoney( pcsAgpdCharacter, &llMoneyCount );
	m_pagpmCharacter->GetMoney( pcsAgpdTargetCharacter, &llTargetMoneyCount );

	//�� �ŷ��� �������� �˻��Ѵ�.
	INT64 myBalance = llMoneyCount - pcsAgpdItemADChar->m_lMoneyCount;
	INT64 yourBalance = llTargetMoneyCount - pcsAgpdItemADTargetChar->m_lMoneyCount;

	if (myBalance >= 0 && yourBalance >= 0)
	{
		myBalance += pcsAgpdItemADTargetChar->m_lMoneyCount;
		yourBalance += pcsAgpdItemADChar->m_lMoneyCount;

		if (myBalance <= AGPDCHARACTER_MAX_INVEN_MONEY && yourBalance <= AGPDCHARACTER_MAX_INVEN_MONEY)
		{
			m_pagpmCharacter->SetMoney(pcsAgpdCharacter, myBalance);
			m_pagpmCharacter->SetMoney(pcsAgpdTargetCharacter, yourBalance);

			bCheckMoney = TRUE;
		}
		else
		{
			if (AgsmSystemMessage* pcsAgsmSystemMessage = (AgsmSystemMessage*)GetModule("AgsmSystemMessage"))
			{
				pcsAgsmSystemMessage->SendSystemMessage(pcsAgpdCharacter, AGPMSYSTEMMESSAGE_CODE_INVEN_MONEY_FULL);
				pcsAgsmSystemMessage->SendSystemMessage(pcsAgpdTargetCharacter, AGPMSYSTEMMESSAGE_CODE_INVEN_MONEY_FULL);
			}
		}
	}

	//���� ��ȯ�ϸ� �����߻�!
	if( bCheckMoney == FALSE )
	{
		//������� ������ FALSE return!!
		m_pagpmCharacter->SetMoney( pcsAgpdCharacter, llMoneyCount );
		m_pagpmCharacter->SetMoney( pcsAgpdTargetCharacter, llTargetMoneyCount );

		bResult = FALSE;
	}
	//���� ����� ��ȯ�� �Ǿ���.
	else
	{
		bResult = CheckInventorySpace(pcsAgpdCharacter, pcsAgpdItemADChar, pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar);

		//�κ��� �������� ������ �����ϱ����� ��������!
		m_pagpmGrid->Init(&cTradeGrid, AGPMITEM_MAX_INVENTORY, AGPMITEM_INVENTORY_ROW, AGPMITEM_INVENTORY_COLUMN );
		//�κ��� �������� ������ �����ϱ����� ��������!
		m_pagpmGrid->Init(&TargetCTradeGrid, AGPMITEM_MAX_INVENTORY, AGPMITEM_INVENTORY_ROW, AGPMITEM_INVENTORY_COLUMN );

		if( bResult )
			bResult = TestGridCopyInven(pcsAgpdItemADChar, &cTradeGrid);

		if( bResult )
		{
			bResult = TestGridCopyTradeGrid(pcsAgpdItemADTargetChar, &cTradeGrid);
		}

		if( bResult )
		{
			bResult = TestGridCopyInven(pcsAgpdItemADTargetChar, &TargetCTradeGrid);
		}

		if( bResult )
		{
			bResult = TestGridCopyTradeGrid(pcsAgpdItemADChar, &TargetCTradeGrid);
		}

		//�ùķ��̼� ��� �ִµ� ������ �����ٴ� �̾߱��. �׷� ����!!
		if( bResult )
		{
			BOOL		bCopyResult;

			INT32 lCurrentTimeStamp = GetCurrentTimeStamp();

			bCopyResult = MoveTradeGridItemToInventory( pcsAgpdCharacter, pcsAgpdItemADChar, pcsAgpdTargetCharacter, lCurrentTimeStamp );

			if( bCopyResult == FALSE )
			{
				//�ùķ��̼Ǳ����ߴµ�? ������ �߻��Ҹ��� ������.... �α� ����� Ŭ���̾�Ʈ�� �˸���.
				bResult = FALSE;
			}

			bCopyResult = MoveTradeGridItemToInventory( pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar, pcsAgpdCharacter, lCurrentTimeStamp );

			if( bCopyResult == FALSE )
			{
				//�ùķ��̼Ǳ����ߴµ�? ������ �߻��Ҹ��� ������.... �α� ����� Ŭ���̾�Ʈ�� �˸���.
				bResult = FALSE;
			}
			else
			{
				// �Ʒ� �� �� ���� �����ΰ�?????
				AgpdItem			*pcsAgpdItem;
				pcsAgpdItem = m_pagpmItem->GetItem( 16 );
				

				//�Ϻ��ϰ� �ŷ��� �Ϸ�Ǿ����� ���� ������ �����Ѵ�.
				m_pagpmCharacter->UpdateMoney( pcsAgpdCharacter );
				m_pagpmCharacter->UpdateMoney( pcsAgpdTargetCharacter );

				// 2004.05.04. steeple.
				if (pcsAgpdItemADChar->m_lMoneyCount > 0)
					WriteTradeMoneyLog(pcsAgpdCharacter, pcsAgpdTargetCharacter, pcsAgpdItemADChar->m_lMoneyCount, lCurrentTimeStamp);
				
				if (pcsAgpdItemADTargetChar->m_lMoneyCount > 0)
					WriteTradeMoneyLog(pcsAgpdTargetCharacter, pcsAgpdCharacter, pcsAgpdItemADTargetChar->m_lMoneyCount, lCurrentTimeStamp);
			}
		}
		//�ù����̼��� ������ �κ��� �����ؼ� ��ȯ�Ҽ� ���ٴ� ��п� �̸���.
		else
		{
			//�׷� ���� ������� ������!!
			m_pagpmCharacter->SetMoney( pcsAgpdCharacter, llMoneyCount );
			m_pagpmCharacter->SetMoney( pcsAgpdTargetCharacter, llTargetMoneyCount );

			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL AgsmPrivateTrade::ProcessLock( INT32 lCID )
{
	if (!m_pagpmItem)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) ||
		pcsAgpdCharacter->m_unActionStatus != AGPDCHAR_STATUS_TRADE)
	{
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

	AgpdCharacter	*pcsAgpdTargetCharacter	= m_pagpmCharacter->GetCharacterLock(pcsAgpdItemADChar->m_lTradeTargetID);
	if (!pcsAgpdTargetCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

	BOOL	bResult	= FALSE;

	if( pcsAgpdItemADChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_TRADING )
	{
		//��� ���ʸ� Lock�̶��?
		if( (pcsAgpdTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) && (pcsAgpdItemADTargetChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_TRADING) )
		{
			pcsAgpdItemADChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_LOCK;

			SendTradeLock(pcsAgpdCharacter);
			SendTradeTargetLocked(pcsAgpdTargetCharacter);

			bResult = TRUE;
		}
		//�Ѵ� ���̶��?
		else if( (pcsAgpdTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) && (pcsAgpdItemADTargetChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_LOCK) )
		{
			pcsAgpdItemADChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_LOCK;

			SendActiveReadyToExchangeButton(pcsAgpdCharacter);
			SendActiveReadyToExchangeButton(pcsAgpdTargetCharacter);

			bResult = TRUE;
		}
	}

	if (pcsAgpdTargetCharacter)
		pcsAgpdTargetCharacter->m_Mutex.Release();
	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	return bResult;
}

BOOL AgsmPrivateTrade::ProcessUnlock( INT32 lCID )
{
	if (!m_pagpmItem)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) ||
		pcsAgpdCharacter->m_unActionStatus != AGPDCHAR_STATUS_TRADE)
	{
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

	AgpdCharacter	*pcsAgpdTargetCharacter = m_pagpmCharacter->GetCharacterLock(pcsAgpdItemADChar->m_lTradeTargetID);
	if (!pcsAgpdTargetCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter) ||
		pcsAgpdTargetCharacter->m_unActionStatus != AGPDCHAR_STATUS_TRADE)
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdTargetItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

	//���� ���� ��� ��� Lock�ߴٸ� Unlock�Ҽ� ����.
	if( (pcsAgpdItemADChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_LOCK) && (pcsAgpdTargetItemADChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_LOCK) )
	{
		//�ϴ��� ����.
	}
	//�׷��� �ʴٸ� Unlock����.
	else
	{
		pcsAgpdItemADChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_TRADING;

		//���� Unlock�Ѱ� ���ڽſ��� �˸���.
		SendTradeUnlock(pcsAgpdCharacter);
		//���� Unlock�Ѱ� �ŷ���뿡�� �˸���.
		SendTradeTargetUnlocked(pcsAgpdTargetCharacter);
	}

	if (pcsAgpdTargetCharacter)
		pcsAgpdTargetCharacter->m_Mutex.Release();
	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmPrivateTrade::ProcessReadyToExchange( INT32 lCID )
{
	if (!m_pagpmItem)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsAgpdCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdCharacter))
	{
		if (pcsAgpdCharacter)
			pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

	if ((pcsAgpdCharacter->m_unActionStatus != AGPDCHAR_STATUS_TRADE) || (pcsAgpdItemADChar->m_lTradeStatus != AGPDITEM_TRADE_STATUS_LOCK))
	{
		pcsAgpdCharacter->m_Mutex.Release();
		return FALSE;
	}

	AgpdCharacter	*pcsAgpdTargetCharacter	= m_pagpmCharacter->GetCharacterLock(pcsAgpdItemADChar->m_lTradeTargetID);
	if (!pcsAgpdTargetCharacter ||
		m_pagpmCharacter->IsAllBlockStatus(pcsAgpdTargetCharacter))
	{
		if (pcsAgpdTargetCharacter)
			pcsAgpdTargetCharacter->m_Mutex.Release();

		pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	AgpdItemADChar	*pcsAgpdItemADTargetChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);
	if (pcsAgpdItemADTargetChar->m_lTradeTargetID != lCID)
	{
		pcsAgpdTargetCharacter->m_Mutex.Release();
		pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	if( (pcsAgpdTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) && (pcsAgpdItemADTargetChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_LOCK) )
	{
		pcsAgpdItemADChar->m_lTradeStatus =AGPDITEM_TRADE_STATUS_READY_TO_EXCHANGE;

		SendReadyToExchange(pcsAgpdCharacter);
		SendTargetReadyToExchange(pcsAgpdTargetCharacter);
	}
	else if( (pcsAgpdTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) && (pcsAgpdItemADTargetChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_READY_TO_EXCHANGE ) )
	{
		//����(lCID, lTargetCID)�� �������� ��ȯ�Ѵ�.
		if (!ProcessExchangeItem( pcsAgpdCharacter, pcsAgpdItemADChar, pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar ))
		{
			//�̰��� �κ��� �����ؼ� ������ ���°���̴�.

			/*
			RestoreTradeItems( pcsAgpdCharacter, pcsAgpdItemADChar, pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar ); 
			RestoreTradeItems( pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar, pcsAgpdCharacter, pcsAgpdItemADChar ); 
			*/
			ProcessTradeCancel( pcsAgpdCharacter->m_lID );
		}
		//�� �� ���. �ŷ��� �Ϸ�Ǿ��ٰ� �� Ŭ���̾�Ʈ�� �˷��ش�.
		else
		{
			//�ŷ����¸� �ʱ�ȭ�Ѵ�.
			ResetTradeStatus( pcsAgpdCharacter, pcsAgpdItemADChar );
			ResetTradeStatus( pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar );

			//�ŷ� �ɼ�â�� ���õ� �͵鵵 �� �ʱ�ȭ �����ش�.
			ResetPrivateTradeOptionGrid(pcsAgpdCharacter, pcsAgpdItemADChar);
			ResetPrivateTradeOptionGrid(pcsAgpdTargetCharacter, pcsAgpdItemADTargetChar);

			SendTradeResult( pcsAgpdCharacter, AGPMPRIVATETRADE_PACKET_TRADE_END );
			SendTradeResult( pcsAgpdTargetCharacter, AGPMPRIVATETRADE_PACKET_TRADE_END );
		}
	}
	//������.
	else
	{
		pcsAgpdTargetCharacter->m_Mutex.Release();
		pcsAgpdCharacter->m_Mutex.Release();

		return FALSE;
	}

	pcsAgpdTargetCharacter->m_Mutex.Release();
	pcsAgpdCharacter->m_Mutex.Release();

	return TRUE;
}

//�ŷ� ���� �������� Ȯ���Ѵ�.
BOOL AgsmPrivateTrade::CheckTradeStatus( INT32 lCID )
{
	AgpdCharacter  *pcsAgpdCharacter;
	AgpdCharacter  *pcsAgpdTargetCharacter;

	AgpdItemADChar *pcsAgpdItemADChar;
	AgpdItemADChar *pcsAgpdItemTargetADChar;

	BOOL			bResult;

	bResult = FALSE;
	pcsAgpdCharacter = m_pagpmCharacter->GetCharacterLock( lCID );

	if( pcsAgpdCharacter && m_pagpmItem )
	{
		pcsAgpdItemADChar = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);

		if( (pcsAgpdItemADChar != NULL) )
		{
			pcsAgpdTargetCharacter = m_pagpmCharacter->GetCharacterLock( pcsAgpdItemADChar->m_lTradeTargetID );

			if( pcsAgpdTargetCharacter != NULL )
			{
				pcsAgpdItemTargetADChar = m_pagpmItem->GetADCharacter(pcsAgpdTargetCharacter);

				if( pcsAgpdItemTargetADChar != NULL )
				{
					if( (pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) && (pcsAgpdTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE) )
					{
						if( (pcsAgpdItemADChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_TRADING) && (pcsAgpdItemTargetADChar->m_lTradeStatus == AGPDITEM_TRADE_STATUS_TRADING) )
						{
							bResult = TRUE;
						}
					}
				}

				pcsAgpdTargetCharacter->m_Mutex.Release();
			}
		}
	}

	if (pcsAgpdCharacter)
		pcsAgpdCharacter->m_Mutex.Release();

	return bResult;
}

void AgsmPrivateTrade::ResetTradeStatus( AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdItemADChar )
{
	if( (pcsAgpdCharacter != NULL) && (pcsAgpdItemADChar != NULL) )
	{
		if (pcsAgpdCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
			m_pagpmCharacter->UpdateActionStatus( pcsAgpdCharacter, AGPDCHAR_STATUS_NORMAL, FALSE );

		pcsAgpdItemADChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_NONE;
		pcsAgpdItemADChar->m_lTradeTargetID = 0;
		pcsAgpdItemADChar->m_lMoneyCount = 0;
		pcsAgpdItemADChar->m_lClientMoneyCount = 0;

		m_pagpmGrid->Reset(&pcsAgpdItemADChar->m_csTradeGrid);
		m_pagpmGrid->Reset(&pcsAgpdItemADChar->m_csClientTradeGrid);
	}
}

BOOL AgsmPrivateTrade::ResetPrivateTradeOptionGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItemADChar *pcsAgpdADChar)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdADChar == NULL)
		return FALSE;

	BOOL	bHaveTradeOptionData = FALSE;

	for(int i=0; i<AGPDITEM_PRIVATE_TRADE_OPTION_MAX; i++)
	{
		if(pcsAgpdADChar->m_stTradeOptionItem[i].m_lItemIID != 0)
		{
			AgpdItem *pcsTradeOptionItem = m_pagpmItem->GetItem(pcsAgpdADChar->m_stTradeOptionItem[i].m_lItemIID);
			if(pcsTradeOptionItem == NULL)
				continue;

			if(pcsTradeOptionItem->m_eStatus == AGPDITEM_STATUS_TRADE_OPTION_GRID && pcsTradeOptionItem->m_pcsItemTemplate &&
				pcsTradeOptionItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE && 
				((AgpdItemTemplateUsable*)pcsTradeOptionItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION)
			{
				// ������� �������� ���� �ŷ��ɼ�â�� �����ֹ����� �ִ°Ŵ�
				pcsAgpdADChar->m_stTradeOptionItem[i].m_lItemIID = 0;
				pcsAgpdADChar->m_stTradeOptionItem[i].m_lItemTID = 0;
				pcsAgpdADChar->m_stTradeOptionItem[i].m_bUsed = FALSE;
				pcsAgpdADChar->m_stTradeOptionItem[i].m_bOnGrid = FALSE;
				pcsTradeOptionItem->m_eStatus = AGPDITEM_STATUS_CASH_INVENTORY;
			}
		}
	}

	return TRUE;
}


BOOL AgsmPrivateTrade::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	//�ŷ���� Ȥ�� Error�޽����� ������ش�~ ����غ���~ ����غ���~
	BOOL			bResult;

//	INT32			lCID;
	INT8			nOperation;
	INT32			lCRequesterID;
	INT32			lTargetCID;
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
						&lTargetCID,
						&lIID,
						&lTID,
						&lMoneyCount,
						&pOriginPos,
						&pGridPos,
						&lResult );

	//�ŷ� ��û�� ���Դ�~
	if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_REQUEST_TRADE )
	{
		bResult = ProcessTradeRequest( lCRequesterID, lTargetCID );
	}
	//�ŷ� ��û�� ���ߴ�.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_CONFIRM )
	{
		bResult = ProcessTradeConfirm( lCRequesterID );
	}
	//�ŷ� ��Ҹ� �ߴ�.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_TRADE_CANCEL )
	{
		bResult = ProcessTradeCancel( lCRequesterID );
	}
	//���� �ø��°��.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_UPDATE_MONEY )
	{
		bResult = CheckTradeStatus( lCRequesterID );

		if( bResult )
			bResult = ProcessTradeUpdateMoney( lCRequesterID, lMoneyCount );
	}
	//Trade Grid�� �������� �ִ°��.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_ADD_TO_TRADE_GRID )
	{
		bResult = CheckTradeStatus( lCRequesterID );

		// �����ֹ���(Ʈ���̵� �ɼǾ�����)�� ������ ��� ���� �׸����̵��� �ִ°��� �ƴϴ�.
		if(lResult == AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER_ENABLE_TRADE)
		{
			AgpdCharacter	*pcsAgpdCharacter	= m_pagpmCharacter->GetCharacterLock(lCRequesterID);
			if(pcsAgpdCharacter == NULL)
				return FALSE;

			AgpdItem *pcsItem = m_pagpmItem->GetExistPrivateTradeOptionItem(pcsAgpdCharacter, lIID);
			if(pcsItem)
			{
				BOOL	bHaveTradeOptionItem = FALSE;

				AgpdItemADChar *pcsAgpdADCharacter = m_pagpmItem->GetADCharacter(pcsAgpdCharacter);
				if(pcsAgpdADCharacter == NULL)
					return FALSE;

				for(int i=0; i<AGPDITEM_PRIVATE_TRADE_OPTION_MAX; i++)
				{
					if(pcsAgpdADCharacter->m_stTradeOptionItem[i].m_lItemIID == 0)
					{
						pcsAgpdADCharacter->m_stTradeOptionItem[i].m_lItemTID = pcsItem->m_lTID;
						pcsAgpdADCharacter->m_stTradeOptionItem[i].m_lItemIID = pcsItem->m_lID;
						pcsAgpdADCharacter->m_stTradeOptionItem[i].m_bOnGrid = TRUE;
						bHaveTradeOptionItem = TRUE;
						break;
					}
				}

				if(bHaveTradeOptionItem == FALSE)
					return FALSE;

				// �������� ���°��� ��ȭ�����ش�.
				pcsItem->m_eStatus = AGPDITEM_STATUS_TRADE_OPTION_GRID;

				// �켱 ������ �÷������� �ִٰ� ��Ŷ�� �����ش�.
				m_pagsmItem->SendPacketItem(pcsItem, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

				// �� ���� ���濡�� ���� �ŷ��ɼǾ����� ������ �����ش�.
				AgpdCharacter *pcsTargetCharacter = m_pagpmCharacter->GetCharacter(pcsAgpdADCharacter->m_lTradeTargetID);
				if(pcsTargetCharacter == NULL)
					return FALSE;

				m_pagsmItem->SendPacketItem(pcsItem, m_pagsmCharacter->GetCharDPNID(pcsTargetCharacter));
			}
		}

		if( bResult && lResult != AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER_ENABLE_TRADE)
		{
			INT16			nInvenLayer, nInvenRow, nInvenColumn;
			INT16			nTradeLayer, nTradeRow, nTradeColumn;

			bResult = ParseGridPos( pOriginPos, &nInvenLayer, &nInvenRow, &nInvenColumn );
		
			if( bResult )
			{
				bResult = ParseGridPos( pGridPos, &nTradeLayer, &nTradeRow, &nTradeColumn );

				if( bResult )
				{
					bResult = ProcessAddToTradeGrid( lCRequesterID, lIID, nInvenLayer, nInvenRow, nInvenColumn, nTradeLayer, nTradeRow, nTradeColumn );
				}
			}
		}
	}
	//Trade Grid���� �������� ���°��.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_REMOVE_FROM_TRADE_GRID )
	{
		bResult = CheckTradeStatus( lCRequesterID );

		if( bResult )
		{
			INT16			nInvenLayer, nInvenRow, nInvenColumn;
			INT16			nTradeLayer, nTradeRow, nTradeColumn;

			bResult = ParseGridPos( pOriginPos, &nTradeLayer, &nTradeRow, &nTradeColumn );

			if( bResult )
			{
				bResult = ParseGridPos( pGridPos, &nInvenLayer, &nInvenRow, &nInvenColumn );

				if( bResult )
				{
					bResult = ProcessRemoveFromTradeGrid( lCRequesterID, lIID, nTradeLayer, nTradeRow, nTradeColumn, nInvenLayer, nInvenRow, nInvenColumn, lMoneyCount );
				}
			}
		}
	}
	//�ŷ��� �� �غ� �� �Ǿ� ok��ư�� ��������.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_LOCK )
	{
		bResult = ProcessLock( lCRequesterID );
	}
	//�ŷ��� �غ� �� �Ǽ� ok��ư ������ ����ѻ���.
	else if( nOperation == AGPMPRIVATETRADE_PACKET_UNLOCK )
	{
		bResult = ProcessUnlock( lCRequesterID );
	}
	else if( nOperation == AGPMPRIVATETRADE_PACKET_READY_TO_EXCHANGE )
	{
		bResult = ProcessReadyToExchange( lCRequesterID );
	}
	//�׹��� lOperation?? ��¥ ��Ŷ�̶� ���³�?..... �α� ���ܼ� ��¡�� ������!!
	else 
	{
	}

	return bResult;
}

INT32 AgsmPrivateTrade::GetCurrentTimeStamp()
{
	time_t timeval;
	(void)time(&timeval);
	
	return (INT32)timeval;
}

BOOL AgsmPrivateTrade::WriteTradeItemLog(AgpdCharacter* pcsSeller, AgpdCharacter* pcsBuyer, AgpdItem* pcsItem, INT32 lTimeStamp)
{
	if(!m_pagpmItemConvert || !m_pagpmLog)
		return TRUE;

	if(!pcsSeller || !pcsBuyer || !pcsItem)
		return FALSE;

	AgsdItem* pcsAgsdItem = m_pagsmItem->GetADItem(pcsItem);
	if(!pcsAgsdItem)
		return FALSE;

	AgpdItemConvertADItem* pcsItemConvertADItem = m_pagpmItemConvert->GetADItem(pcsItem);
	if(!pcsItemConvertADItem)
		return FALSE;

	CHAR szConvert[AGPDLOG_MAX_ITEM_CONVERT_STRING+1];
	ZeroMemory(szConvert, sizeof(szConvert));
	m_pagpmItemConvert->EncodeConvertHistory(pcsItemConvertADItem, szConvert, AGPDLOG_MAX_ITEM_CONVERT_STRING);

	CHAR szOption[AGPDLOG_MAX_ITEM_FULL_OPTION + 1];
	ZeroMemory(szOption, sizeof(szOption));

	CHAR szNormalOption[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szNormalOption, sizeof(szNormalOption));
	m_pagsmItem->EncodingOption(pcsItem, szNormalOption, AGPDLOG_MAX_ITEM_OPTION);

	//##########################
	CHAR szSkillPlus[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szSkillPlus, sizeof(szSkillPlus));
	m_pagsmItem->EncodingSkillPlus(pcsItem, szSkillPlus, AGPDLOG_MAX_ITEM_OPTION);	

	sprintf(szOption, "%s::%s", szNormalOption, szSkillPlus);

	AgsdCharacter* pcsAgsdSeller = m_pagsmCharacter->GetADCharacter(pcsSeller);
	AgsdCharacter* pcsAgsdBuyer = m_pagsmCharacter->GetADCharacter(pcsBuyer);
	if(!pcsAgsdSeller || !pcsAgsdBuyer)
		return FALSE;

	CHAR szPositionS[33];
	ZeroMemory(szPositionS, sizeof(szPositionS));
	m_pagsmCharacter->EncodingPosition(&pcsSeller->m_stPos, szPositionS, 32);

	CHAR szPositionB[33];
	ZeroMemory(szPositionB, sizeof(szPositionB));
	m_pagsmCharacter->EncodingPosition(&pcsBuyer->m_stPos, szPositionB, 32);

	BOOL bIsCashItem = IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType);

	eAGPDLOGTYPE_ITEM eType = AGPDLOGTYPE_ITEM_TRADE_GIVE;
	m_pagpmLog->WriteLog_Item(bIsCashItem,
							eType,
							lTimeStamp,
							&pcsAgsdSeller->m_strIPAddress[0],
							pcsAgsdSeller->m_szAccountID,
							pcsAgsdSeller->m_szServerName,
							pcsSeller->m_szID,
							((AgpdCharacterTemplate*)pcsSeller->m_pcsCharacterTemplate)->m_lID,
							m_pagpmCharacter->GetLevel(pcsSeller),
							m_pagpmCharacter->GetExp(pcsSeller),
							pcsSeller->m_llMoney,
							pcsSeller->m_llBankMoney,
							pcsAgsdItem->m_ullDBIID,
							((AgpdItemTemplate*)pcsItem->m_pcsItemTemplate)->m_lID,
							pcsItem->m_nCount,
							szConvert,
							szOption,
							0,
							pcsBuyer->m_szID,
							pcsItem->m_nInUseItem,
							pcsItem->m_lCashItemUseCount,
							pcsItem->m_lRemainTime,
							pcsItem->m_lExpireTime,
							pcsItem->m_llStaminaRemainTime,
							m_pagpmItem->GetItemDurabilityCurrent(pcsItem),
							m_pagpmItem->GetItemDurabilityMax(pcsItem),
							szPositionS
							);

	eType = AGPDLOGTYPE_ITEM_TRADE_TAKE;
	m_pagpmLog->WriteLog_Item(bIsCashItem,
							eType,
							lTimeStamp,
							&pcsAgsdBuyer->m_strIPAddress[0],
							pcsAgsdBuyer->m_szAccountID,
							pcsAgsdBuyer->m_szServerName,
							pcsBuyer->m_szID,
							((AgpdCharacterTemplate*)pcsBuyer->m_pcsCharacterTemplate)->m_lID,
							m_pagpmCharacter->GetLevel(pcsBuyer),
							m_pagpmCharacter->GetExp(pcsBuyer),
							pcsBuyer->m_llMoney,
							pcsBuyer->m_llBankMoney,
							pcsAgsdItem->m_ullDBIID,
							((AgpdItemTemplate*)pcsItem->m_pcsItemTemplate)->m_lID,
							pcsItem->m_nCount,
							szConvert,
							szOption,
							0,
							pcsSeller->m_szID,
							pcsItem->m_nInUseItem,
							pcsItem->m_lCashItemUseCount,
							pcsItem->m_lRemainTime,
							pcsItem->m_lExpireTime,
							pcsItem->m_llStaminaRemainTime,
							m_pagpmItem->GetItemDurabilityCurrent(pcsItem),
							m_pagpmItem->GetItemDurabilityMax(pcsItem),
							szPositionB
							);

	return TRUE;
}

BOOL AgsmPrivateTrade::WriteTradeMoneyLog(AgpdCharacter* pcsSeller, AgpdCharacter* pcsBuyer, INT32 lMoney, INT32 lTimeStamp)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pcsSeller || !pcsBuyer)
		return FALSE;

	AgsdCharacter* pcsAgsdSeller = m_pagsmCharacter->GetADCharacter(pcsSeller);
	AgsdCharacter* pcsAgsdBuyer = m_pagsmCharacter->GetADCharacter(pcsBuyer);
	if(!pcsAgsdSeller || !pcsAgsdBuyer)
		return FALSE;

	eAGPDLOGTYPE_GHELD eType = AGPDLOGTYPE_GHELD_TRADE_GIVE;
	m_pagpmLog->WriteLog_Gheld(eType,
							lTimeStamp,
							&pcsAgsdSeller->m_strIPAddress[0],
							pcsAgsdSeller->m_szAccountID,
							pcsAgsdSeller->m_szServerName,
							pcsSeller->m_szID,
							((AgpdCharacterTemplate*)pcsSeller->m_pcsCharacterTemplate)->m_lID,
							m_pagpmCharacter->GetLevel(pcsSeller),
							m_pagpmCharacter->GetExp(pcsSeller),
							pcsSeller->m_llMoney,
							pcsSeller->m_llBankMoney,
							lMoney,
							pcsBuyer->m_szID
							);

	eType = AGPDLOGTYPE_GHELD_TRADE_TAKE;
	m_pagpmLog->WriteLog_Gheld(eType,
							lTimeStamp,
							&pcsAgsdBuyer->m_strIPAddress[0],
							pcsAgsdBuyer->m_szAccountID,
							pcsAgsdBuyer->m_szServerName,
							pcsBuyer->m_szID,
							((AgpdCharacterTemplate*)pcsBuyer->m_pcsCharacterTemplate)->m_lID,
							m_pagpmCharacter->GetLevel(pcsBuyer),
							m_pagpmCharacter->GetExp(pcsBuyer),
							pcsBuyer->m_llMoney,
							pcsBuyer->m_llBankMoney,
							lMoney,
							pcsSeller->m_szID
							);

	return TRUE;
}

BOOL AgsmPrivateTrade::CBRemoveTradeGrid(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgsmPrivateTrade *	pThis			= (AgsmPrivateTrade *) pvClass;
	AgpdItem *			pcsItem			= (AgpdItem *) pvData;
	AgpdCharacter *		pcsCharacter	= (AgpdCharacter *) pvCustData;
	AgpdCharacter *		pcsTargetCharacter;
	AgpdItemADChar *	pcsAgpdItemADChar;
	AgsdCharacter *		pcsAgsdTargetCharacter;

	if (!pcsItem || !pcsCharacter)
		return FALSE;

	pcsAgpdItemADChar	= pThis->m_pagpmItem->GetADCharacter(pcsCharacter);
	pcsTargetCharacter	= pThis->m_pagpmCharacter->GetCharacter(pcsAgpdItemADChar->m_lTradeTargetID);
	if (!pcsTargetCharacter)
		return FALSE;

	pcsAgsdTargetCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsTargetCharacter);

	pThis->m_pagsmItem->SendPacketItemRemove( pcsItem->m_lID, pcsAgsdTargetCharacter->m_dpnidCharacter );

	return TRUE;
}

BOOL AgsmPrivateTrade::CBDisconnect(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	if(NULL == pvData || NULL == pvClass)
		return FALSE;

	AgsmPrivateTrade	*pThis				= (AgsmPrivateTrade *) pvClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *) pvData;

	AgpdItemADChar		*pcsAgpdItemADChar  = pThis->m_pagpmItem->GetADCharacter(pcsCharacter);
	if(NULL == pcsAgpdItemADChar)
		return FALSE;

	if(pcsAgpdItemADChar->m_lTradeStatus != AGPDITEM_TRADE_STATUS_NONE)
	{
		pThis->ProcessTradeCancel(pcsCharacter->m_lID);
	}

	return TRUE;
}

BOOL AgsmPrivateTrade::SetCallbackBuddyCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMPRIVATETRADE_CALLBACK_TRADE_BUDDY_CHECK, pfCallback, pClass);
}

BOOL AgsmPrivateTrade::CheckInventorySpace(AgpdCharacter* pcsCharacter, AgpdItemADChar *pcsAgpdItemADChar, AgpdCharacter* pcsTargetCharacter, AgpdItemADChar *pcsAgpdItemADTargetChar)
{
	short invenSpace      = pcsAgpdItemADChar->m_csInventoryGrid.m_lGridCount - pcsAgpdItemADChar->m_csInventoryGrid.m_lItemCount;

	// 2008.07.11. steeple
	// If he use a sub inventory, add a remain sub inventory count to an invenSpace variable.
	if(m_pagpmItem->IsEnableSubInventory(pcsCharacter))
		invenSpace += pcsAgpdItemADChar->m_csSubInventoryGrid.m_lGridCount - pcsAgpdItemADChar->m_csSubInventoryGrid.m_lItemCount;

	short cashInvenSpace  = pcsAgpdItemADChar->m_csCashInventoryGrid.m_lGridCount - pcsAgpdItemADChar->m_csCashInventoryGrid.m_lItemCount;
	short normalItemCount = 0,
		  cashItemCount   = 0;
	
	GetItemCountByCoinType(&pcsAgpdItemADTargetChar->m_csTradeGrid, &normalItemCount, &cashItemCount);

	if ( (invenSpace < normalItemCount) || (cashInvenSpace < cashItemCount) )
		return FALSE;

	invenSpace      = pcsAgpdItemADTargetChar->m_csInventoryGrid.m_lGridCount - pcsAgpdItemADTargetChar->m_csInventoryGrid.m_lItemCount;

	// 2008.07.11. steeple
	// If he use a sub inventory, add a remain sub inventory count to an invenSpace variable.
	if(m_pagpmItem->IsEnableSubInventory(pcsTargetCharacter))
		invenSpace += pcsAgpdItemADTargetChar->m_csSubInventoryGrid.m_lGridCount - pcsAgpdItemADTargetChar->m_csSubInventoryGrid.m_lItemCount;

	cashInvenSpace  = pcsAgpdItemADTargetChar->m_csCashInventoryGrid.m_lGridCount - pcsAgpdItemADTargetChar->m_csCashInventoryGrid.m_lItemCount;
	normalItemCount = 0;
	cashItemCount   = 0;
	
	GetItemCountByCoinType(&pcsAgpdItemADChar->m_csTradeGrid, &normalItemCount, &cashItemCount);

	if ( (invenSpace < normalItemCount) || (cashInvenSpace < cashItemCount) )
		return FALSE;

	return TRUE;
}

void AgsmPrivateTrade::GetItemCountByCoinType(AgpdGrid* pGrid, short* normalItemCount, short* cashItemCount)
{
	int i = 0;

	AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItemSequence(pGrid, &i);
	while ( pcsGridItem )
	{
		AgpdItemTemplate* pItemTemplate = m_pagpmItem->GetItemTemplate(pcsGridItem->m_lItemTID);
		if ( IS_CASH_ITEM(pItemTemplate->m_eCashItemType) )
			++(*cashItemCount);
		else
			++(*normalItemCount);
			
		pcsGridItem = m_pagpmGrid->GetItemSequence(pGrid, &i);
	}
}

INT32 AgsmPrivateTrade::NumberOfBoundOnOwnerItemInTradeGrid(AgpdItemADChar *pcsAgpdItemADChar)
{
	if(pcsAgpdItemADChar == NULL)
		return -1;

	AgpdGridItem	*pcsGridItem;
	AgpdItem		*pcsAgpdItem;

	INT32 nBoundOnOwnerItemCount = 0;

	INT32 i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csTradeGrid, &i);
		pcsGridItem;
		pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsAgpdItemADChar->m_csTradeGrid, &i))
	{
		pcsAgpdItem = m_pagpmItem->GetItem( pcsGridItem );
		if(pcsAgpdItem)
		{
			if(m_pagpmItem->IsBoundOnOwner(pcsAgpdItem) || m_pagpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND)
			{
				nBoundOnOwnerItemCount++;
			}
		}
	}

	return nBoundOnOwnerItemCount;
}
