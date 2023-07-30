/******************************************************************************
Module:  AgsmAuctionServer.cpp
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#include "AgsmAuctionServer.h"

AgsmAuctionServer::AgsmAuctionServer()
{
	m_iReservedQueueCount = 0;

	SetModuleName( "AgsmAuctionServer" );

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGSMAUCTION_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  ����
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1, // lSellerID  ���ǿ� �ø��� ID
							AUTYPE_INT32,			1, // lSellerID  ���ǿ� �ø��� ID
							AUTYPE_INT32,			1, // lBuyerID   ���ǿ� �ö�� �� ��� �� ID
							AUTYPE_INT32,			1, // lIID		 Item IID
							AUTYPE_INT64,			1, // ullDBID    Item DB ID
							AUTYPE_INT32,			1, // lITID      Item Type ID
							AUTYPE_PACKET,			1, // csInvenPos
							AUTYPE_PACKET,			1, // csSalesBoxPos
							AUTYPE_PACKET,			1, // csFactorInfo
							AUTYPE_INT32,			1, // bConnected;
							AUTYPE_INT32,			1, // lResult    Result
		                    AUTYPE_END,				0
							);


	//Inven�� �ְ� ���� ����Ѵ�.
	m_csInvenPos.SetFlagLength(sizeof(INT8));
	m_csInvenPos.SetFieldType(
		                         AUTYPE_INT16,		1, //Layer
		                         AUTYPE_INT16,		1, //Row
		                         AUTYPE_INT16,		1, //Column
		                         AUTYPE_END,		0
								 );

	//SalesBox�� �ְ� ���� ����Ѵ�.
	m_csSalesBoxPos.SetFlagLength(sizeof(INT8));
	m_csSalesBoxPos.SetFieldType(
		                         AUTYPE_INT16,		1, //Layer
		                         AUTYPE_INT16,		1, //Row
		                         AUTYPE_INT16,		1, //Column
		                         AUTYPE_END,		0
								 );
}

AgsmAuctionServer::~AgsmAuctionServer()
{
}

BOOL AgsmAuctionServer::OnAddModule()
{
	m_csRegisterCID.InitializeObject( sizeof(INT32), MAX_AUCTION_CONNECTION_COUNT );

	m_pagpmFactors = (AgpmFactors *)GetModule( "AgpmFactors" );
	m_pagpmItem = (AgpmItem *)GetModule( "AgpmItem" );
	m_paAuOLEDBManager = (AuOLEDBManager *)GetModule( "AuOLEDBManager" );
	m_pAgsmServerManager = (AgsmServerManager *) GetModule( "AgsmServerManager2" );

	if( !m_pagpmFactors || !m_pagpmItem || !m_paAuOLEDBManager || !m_pAgsmServerManager )
	{
		return FALSE;
	}

	return TRUE;
}

void AgsmAuctionServer::IncreaseQueueCount()
{
	m_iReservedQueueCount++;
}

void AgsmAuctionServer::DecreaseQueueCount()
{
	m_iReservedQueueCount--;
}

BOOL AgsmAuctionServer::CheckQueueCount()
{
	BOOL		bResult;
	int			iCurrentCount;
	int			iTotalCount;

	iCurrentCount = m_paAuOLEDBManager->m_pcQueue->getCount();

	iTotalCount = iCurrentCount+m_iReservedQueueCount;

	if( iTotalCount >= MAX_AUCTION_QUEUE_COUNT )
	{
		bResult =  FALSE;
	}
	else
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL AgsmAuctionServer::PushToQueue( char *pstrQuery, INT8 nOperation, INT32 lSellerID, INT32 lBuyerID, INT32 lErrorCode )
{
	char			*pstrQueueQuery;

	bool			bResult;
	int				iQueryLen;

	AgsmAuctionQueueInfo	*pcsAgsmAuctionQueueInfo;

	pcsAgsmAuctionQueueInfo = new AgsmAuctionQueueInfo;

	iQueryLen = strlen( pstrQuery );
	pstrQueueQuery = new char[iQueryLen+1];
	memset( pstrQueueQuery, 0, sizeof(char) * (iQueryLen+1) );
	strcat( pstrQueueQuery, pstrQuery );

	pcsAgsmAuctionQueueInfo->m_nOperation = nOperation;
	pcsAgsmAuctionQueueInfo->m_lBuyerID = lBuyerID;
	pcsAgsmAuctionQueueInfo->m_lSellerID = lSellerID;
	pcsAgsmAuctionQueueInfo->m_pstrQueury = pstrQueueQuery;
	pcsAgsmAuctionQueueInfo->m_lErrorCode = lErrorCode;

	bResult = m_paAuOLEDBManager->PushToQueue( pcsAgsmAuctionQueueInfo, __FILE__, __LINE__  );

	return bResult;
}

BOOL AgsmAuctionServer::SetCallbackSell(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AUCTION_CB_ID_SELL, pfCallback, pClass);
}

BOOL AgsmAuctionServer::SetCallbackSold(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AUCTION_CB_ID_SOLD, pfCallback, pClass);
}

BOOL AgsmAuctionServer::SetCallbackBuy(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AUCTION_CB_ID_BUY, pfCallback, pClass);
}

BOOL AgsmAuctionServer::SetCallbackBackOutToInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AUCTION_CB_ID_BACKOUT_TO_INVENTORY, pfCallback, pClass);
}

BOOL AgsmAuctionServer::SetCallbackBackOutToSalesBox(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AUCTION_CB_ID_BACKOUT_TO_SALESBOX, pfCallback, pClass);
}

//���ǿ� �������� �Ǵٰ� �˸���.(Sales Box�� ������ ���°Ŵ�.)
/*BOOL AgsmAuctionServer::SendSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lItemID, INT32 lItemTypeID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_SELL_ITEM;
	INT16	nPacketLength;
	void	*pvInvenPos;
	void	*pvSalesBoxPos;

	pvInvenPos  = m_csInvenPos.MakePacket(FALSE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
										&nInvenLayer,
										&nInvenRow,
										&nInvenColumn );

	pvSalesBoxPos  = m_csSalesBoxPos.MakePacket(FALSE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
										&nSaleLayer,
										&nSaleRow,
										&nSaleColumn );

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
													&lOperation,     //lOperation
													pstrSellerID,      //pstrSellerID
													&lSellerID,		//lSellerID
													NULL,            //lBuyerID
													&lItemID,        //lIID
													NULL,			//ullDBID
													&lItemTypeID,    //lTID
													pvInvenPos,		//pvInvenPos
													pvSalesBoxPos,   //pvGridPos
													NULL,			 //pvFieldInfo
													NULL,			//bConnected
													NULL );          //lResult

	if( pvPacket != NULL )
	{
		AgsdServer		*pagsdServer;
		
		pagsdServer = m_pAgsmServerManager->GetServer( lControlServerID );

		if( pagsdServer != NULL )
		{
			bResult = SendPacket( pvPacket, nPacketLength, pagsdServer->m_dpnidServer );
		}
		else
		{
			bResult = FALSE;
		}
		
		m_csInvenPos.FreePacket(pvInvenPos);
		m_csSalesBoxPos.FreePacket(pvSalesBoxPos);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;	
}

///���ǿ� �ö�� �������� ��ٰ� Ŭ���̾�Ʈ�� �˸���.
BOOL AgsmAuctionServer::SendBuyItem( INT32 lControlServerID, char *pstrSellerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lItemTypeID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_SELL_ITEM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
													&lOperation,     //lOperation
													pstrSellerID,      //pstrSellerID
													NULL,			//lSellerID
													NULL,            //lBuyerID
													NULL,	        //lIID
													&ullDBID,		//ullDBID
													&lItemTypeID,    //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			 //pvFieldInfo
													NULL,			//bConnected
													NULL );          //lResult

	if( pvPacket != NULL )
	{
		AgsdServer		*pagsdServer;
		
		pagsdServer = m_pAgsmServerManager->GetServer( lControlServerID );

		if( pagsdServer != NULL )
		{
			bResult = SendPacket( pvPacket, nPacketLength, pagsdServer->m_dpnidServer );
		}
		else
		{
			bResult = FALSE;
		}
		
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;	
}

//���ǿ� �ø� �������� ������Ų�ٰ� �˸���.(Sales Box���� ������ ����.)
BOOL AgsmAuctionServer::SendBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lItemTypeID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn, INT8 lOperation )
{
	BOOL			bResult;

	bResult = FALSE;

	INT16	nPacketLength;
	void	*pvInvenPos;
	void	*pvSalesBoxPos;

	pvInvenPos  = m_csInvenPos.MakePacket(FALSE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
										&nInvenLayer,
										&nInvenRow,
										&nInvenColumn );

	pvSalesBoxPos  = m_csSalesBoxPos.MakePacket(FALSE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
										&nSaleLayer,
										&nSaleRow,
										&nSaleColumn );

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMAUCTION_PACKET_TYPE,
													&lOperation,     //lOperation
													pstrSellerID,      //pstrSellerID
													&lSellerID,		//lSellerID
													NULL,            //lBuyerID
													NULL,			//lIID
													&ullDBID,		//ullDBID
													&lItemTypeID,    //lTID
													pvInvenPos,
													pvSalesBoxPos,       //pvSalesBoxPos
													NULL,			 //pvFieldInfo
													NULL,			//bConnected
													NULL );          //lResult

	if( pvPacket != NULL )
	{
		AgsdServer		*pagsdServer;
		
		pagsdServer = m_pAgsmServerManager->GetServer( lControlServerID );

		if( pagsdServer != NULL )
		{
			bResult = SendPacket( pvPacket, nPacketLength, pagsdServer->m_dpnidServer );
		}
		else
		{
			bResult = FALSE;
		}
		
		m_csInvenPos.FreePacket(pvInvenPos);
		m_csSalesBoxPos.FreePacket(pvSalesBoxPos);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;	
}

BOOL AgsmAuctionServer::PreprocessSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn )
{
	BOOL				bResult;

	//���� DB���� ť�� ������ Ȯ���Ѵ�.

	//��Ŷ�� �״�� �����Ѵ�. �ϴ��� ����.
	bResult = SendSellItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, nInvenLayer, nInvenRow, nInvenColumn, nSaleLayer, nSaleRow, nSaleColumn );

	return bResult;
}

BOOL AgsmAuctionServer::PreprocessBuyItem( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID )
{
	BOOL				bResult;
	char				*pstrSellerID;
	
	//���� DB���� ť�� ������ Ȯ���Ѵ�.

	//DB�� ����� �������� �ִ�������.

	//DB�� ����� �������� LOCK�Ѵ�. Update

	//lSellerID�� ���Ѵ�.
	pstrSellerID = 0; //DB�� ����� �̸��� ���Ѵٴ��� �ؾ��Ѵ�.

	bResult = SendBuyItem( lControlServerID, pstrSellerID, lBuyerID, ullDBID, lTID );

	return TRUE;
}

BOOL AgsmAuctionServer::PreprocessBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lTID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn, INT8 lOperation )
{
	BOOL				bResult;

	//���� DB���� ť�� ������ Ȯ���Ѵ�.

	//��Ŷ�� �״�� �����Ѵ�. �ϴ��� ����.
	bResult = SendBackOutItem( lControlServerID, pstrSellerID, lSellerID, ullDBID, lTID, nInvenLayer, nInvenRow, nInvenColumn, nSaleLayer, nSaleRow, nSaleColumn, lOperation );

	return TRUE;
}

BOOL AgsmAuctionServer::ProcessSellItem(char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lTID,void *pvFieldInfo )
{
	INT8				lOperation;
	char				*pstrQueryString;

	lOperation = AGPMAUCTION_SELL_ITEM;
	pstrQueryString = "select * from account";

	//DB�� ������ ���.
	PushToQueue( pstrQueryString, lOperation, lSellerID, 0, 0 );

	return FALSE;
}

BOOL AgsmAuctionServer::ProcessSoldItem(char *pstrSellerID, INT32 lSellerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID )
{
	INT8				lOperation;
	char				*pstrQueryString;

	lOperation = AGPMAUCTION_SOLD_ITEM;
	pstrQueryString = "select * from account";

	//DB�� ������ ������ �߰��Ѵ�.
	PushToQueue( pstrQueryString, lOperation, lSellerID, lBuyerID, 0 );

	return FALSE;
}

BOOL AgsmAuctionServer::ProcessBuyItem(char *pstrSellerID, INT32 lSellerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID )
{
	//DB ������ �ʿ����.

	//AgsmAuctionClient�� CallBack�� ȣ���Ͽ� ��ٰ� �˷��ش�.
	EnumCallback( AUCTION_CB_ID_BUY, &lBuyerID, NULL );

	return FALSE;
}

BOOL AgsmAuctionServer::ProcessBackOutItem(char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lTID, INT8 lOperation )
{
	char				*pstrQueryString;

	pstrQueryString = "select * from account";

	//DB���� Item�� �����Ѵ�.
	PushToQueue( pstrQueryString, lOperation, lSellerID, 0, 0 );
	
	return FALSE;
}

//��Ŷ�� �ް� �Ľ�����~
BOOL AgsmAuctionServer::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
{
	//�ŷ���� Ȥ�� Error�޽����� ������ش�~ ����غ���~ ����غ���~
	BOOL			bResult;

	INT8			lOperation;
	char			*pstrSellerID;
	INT32			lSellerID;
	INT32			lBuyerID;
	INT32			lIID;
	UINT64			ullDBID;
	INT32			lTID;
	void			*pvInvenPos;		//Inven �׸��� ��ġ.
	void			*pvSalesBoxPos;		//Sales Box �׸��� ��ġ.
	void			*pvFactor;			//������ ����.
	BOOL			bConnected;
	INT32			lResult;

	bResult = FALSE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&lOperation,
						&pstrSellerID,
						&lSellerID,
						&lBuyerID,
						&lIID,
						&ullDBID,
						&lTID,
						&pvInvenPos,
						&pvSalesBoxPos,
						&pvFactor,
						&bConnected,
						&lResult );

	//�������� �Ǵ�.(Sales Box)�� �ø���.
	if( lOperation == AGPMAUCTION_SELL_ITEM )
	{
		bResult = ProcessSellItem( pstrSellerID, lSellerID, lIID, lTID, pvFactor );
	}
	else if( lOperation == AGPMAUCTION_SOLD_ITEM )
	{
		bResult = ProcessSoldItem( pstrSellerID, lSellerID, lBuyerID, ullDBID, lTID );
	}
	//�������� ���.(�Խ��ǿ� �ö�� �������� ���.)
	else if( lOperation == AGPMAUCTION_BUY_ITEM )
	{
		bResult = ProcessBuyItem( pstrSellerID, lSellerID, lBuyerID, ullDBID, lTID );
	}
	//�������� ������Ų��.(�κ����� �ִ´�.)
	else if( lOperation == AGPMAUCTION_BACK_OUT_TO_INVENTORY )
	{
		bResult = ProcessBackOutItem( pstrSellerID, lSellerID, ullDBID, lTID, lOperation );
	}
	//�������� ������Ų��.
	else if( lOperation == AGPMAUCTION_BACK_OUT_TO_SALESBOX)
	{
		bResult = ProcessBackOutItem( pstrSellerID, lSellerID, ullDBID, lTID, lOperation );
	}
	//���������� ��Ŷ�� �޾Ҵ�. ��ġ���ؿ�����!! �� �׷��� �̰� �� �߸����ݾ�? -_-; ���d�d~
	else
	{
		//�̹� bResult = FALSE�̴�. �׿��� ������ ����!!
	}

	return bResult;
}
*/