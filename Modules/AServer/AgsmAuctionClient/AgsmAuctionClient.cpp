/******************************************************************************
Module:  AgsmAuctionClient.cpp
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#include <stdio.h>
#include "AgsmAuctionClient.h"

AgsmAuctionClient::AgsmAuctionClient()
{
	SetModuleName( "AgsmAuctionClient" );

	//��������~ ��������~ ��~��~ � ��Ŷ�� ����������? by ġ�佺��~
	SetPacketType(AGPMAUCTION_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT16));
	m_csPacket.SetFieldType(
		                    AUTYPE_INT8,			1, // Operation  ����
							AUTYPE_INT32,			1, // lControlServerID;
							AUTYPE_CHAR,			AGPACHARACTER_MAX_ID_STRING + 1, // pstrSellerID;
							AUTYPE_INT32,			1, // lSellerID;
							AUTYPE_INT32,			1, // lBuyerID;
							AUTYPE_INT32,			1, // lIID       Item ID
							AUTYPE_UINT64,			1, // ullDBID	 Item DBID
							AUTYPE_INT32,			1, // lITID      Item Type ID
							AUTYPE_PACKET,			1, // csInvenPos
							AUTYPE_PACKET,			1, // csSalesBoxPos
							AUTYPE_PACKET,			1, // csItemInfo
		                    AUTYPE_PACKET,			1, // csSearchInfo
							AUTYPE_PACKET,			1, // csCategoryCount
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

	m_csItemInfo.SetFlagLength(sizeof(INT8));
	m_csItemInfo.SetFieldType(
								 AUTYPE_INT32,		1,  //�Խù� ��ȣ.
		                         AUTYPE_CHAR,		16, //�Խù� �ø���ID(�������̸��̴�.)
		                         AUTYPE_INT32,		1,  //�Խ�Item ����.
								 AUTYPE_PACKET,		1,  //�Խþ����� Field(m_csFieldInfo) ����.
		                         AUTYPE_CHAR,		30, //�Խù� �ø��ð� 
		                         AUTYPE_END,		0
								 );

	m_csSearchInfo.SetFlagLength(sizeof(INT8));
	m_csSearchInfo.SetFieldType(
								 AUTYPE_INT8,		1,  //�� ������ �������� ã�´�?
								 AUTYPE_INT32,		1,  //���° ������ �ΰ�?
		                         AUTYPE_END,		0
								 );

	m_csCategoryCount.SetFlagLength(sizeof(INT8));
	m_csCategoryCount.SetFieldType(
								 AUTYPE_INT32,		1,  //Item TypeID
								 AUTYPE_INT32,		1,  //Item Count
		                         AUTYPE_END,		0
								 );
}

AgsmAuctionClient::~AgsmAuctionClient()
{
}


BOOL AgsmAuctionClient::OnAddModule()
{
	m_csCIDManager.InitializeObject( sizeof(INT32), MAX_AUCTION_CONNECTION_COUNT );

	m_pagsmAuctionServer = (AgsmAuctionServer *)GetModule( "AgsmAuctionServer" );

	if( !m_pagsmAuctionServer )
		return FALSE;

	m_pagsmAuctionServer->SetCallbackSell( CBAuctionSell, this );
	m_pagsmAuctionServer->SetCallbackSold( CBAuctionSold, this );
	m_pagsmAuctionServer->SetCallbackBuy( CBAuctionBuy, this );
	m_pagsmAuctionServer->SetCallbackBackOutToInventory( CBAuctionBackoutToInventory, this );
	m_pagsmAuctionServer->SetCallbackBackOutToSalesBox( CBAuctionBackOutToSalesBox, this );

	return TRUE;
}

//���Ǽ������� ĳ����ID�� dpnid�� ����Ѵ�. ondestroy�� CID�� �ޱ� ���ؼ��̴�.
BOOL AgsmAuctionClient::ProcessAddCIDToAuctionServer( INT32 lCID, UINT32 ulNID )
{
	BOOL			bResult;

	bResult = SetIDToPlayerContext( lCID, ulNID );

	if( bResult == TRUE )
	{
		m_csCIDManager.AddObject( &lCID, ulNID );
	}

	return bResult;
}

UINT32 AgsmAuctionClient::GetulNID( INT32 lCID )
{
	UINT32		*pulNID;
	
	pulNID = (UINT32 *)m_csCIDManager.GetObject( lCID );

	if( pulNID != NULL )
	{
		return *pulNID;
	}
	else
	{
		return 0;
	}
}

BOOL AgsmAuctionClient::ParseInvenPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol)
{
	BOOL			bResult;

	bResult = FALSE;

	if( pGridPos != NULL )
	{
		INT16  lTempIndex, lTempRow, lTempColumn;

		m_csInvenPos.GetField(FALSE, pGridPos, 0, NULL,
									&lTempIndex,
									&lTempRow,
									&lTempColumn);

		if(pnLayer)
		{
			*pnLayer = lTempIndex;
		}

		if(pnRow)
		{
			*pnRow = lTempRow;
		}

		if(pnCol)
		{
			*pnCol = lTempColumn;
		}

		bResult = TRUE;	
	}

	return bResult;
}

BOOL AgsmAuctionClient::ParseSalesBoxPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol)
{
	BOOL			bResult;

	bResult = FALSE;

	if( pGridPos != NULL )
	{
		INT16  lTempIndex, lTempRow, lTempColumn;

		m_csSalesBoxPos.GetField(FALSE, pGridPos, 0, NULL,
									&lTempIndex,
									&lTempRow,
									&lTempColumn);

		if(pnLayer)
		{
			*pnLayer = lTempIndex;
		}

		if(pnRow)
		{
			*pnRow = lTempRow;
		}

		if(pnCol)
		{
			*pnCol = lTempColumn;
		}

		bResult = TRUE;	
	}

	return bResult;
}

BOOL AgsmAuctionClient::ParseSearchInfo( PVOID pvSearchInfo, bool *pbSearchByMyLevel, INT32 *plPage )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pvSearchInfo != NULL )
	{
		bool			bTempSearchByMyLevel;
		INT32			lTempPage;


		m_csSearchInfo.GetField(FALSE, pvSearchInfo, 0, NULL,
									&bTempSearchByMyLevel,
									&lTempPage	);

		if(pbSearchByMyLevel)
		{
			*pbSearchByMyLevel = bTempSearchByMyLevel;
		}

		if(plPage)
		{
			*plPage = lTempPage;
		}

		bResult = TRUE;	
	}

	return bResult;
}

BOOL AgsmAuctionClient::CBAuctionSell(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;

	pThis->SendSellItem( *plCID, 0 );

	return TRUE;
}

BOOL AgsmAuctionClient::CBAuctionSold(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;

	pThis->SendSoldItem( *plCID, 0 );

	return TRUE;
}

BOOL AgsmAuctionClient::CBAuctionBuy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;

	pThis->SendSellItem( *plCID, 0 );

	return TRUE;
}

BOOL AgsmAuctionClient::CBAuctionBackoutToInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;
	INT8					*pnOperation;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;
	pnOperation = (INT8 *)pCustData;

	pThis->SendBackOutItem( *plCID, 0, *pnOperation );

	return TRUE;
}


BOOL AgsmAuctionClient::CBAuctionBackOutToSalesBox(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgsmAuctionClient		*pThis;
	INT32					*plCID;
	INT8					*pnOperation;

	pThis = (AgsmAuctionClient *)pClass;
	plCID = (INT32 *)pData;
	pnOperation = (INT8 *)pCustData;

	pThis->SendBackOutItem( *plCID, 0, *pnOperation );

	return TRUE;
}

/*BOOL AgsmAuctionClient::SendItemCount( INT32 lBuyerID, INT32 lItemTypeID, INT32 lItemCount )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_GET_ITEM_COUNT;
	INT16	nPacketLength;
	void	*pvCategoryCount;

	pvCategoryCount  = m_csCategoryCount.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
										&lItemTypeID,
										&lItemCount );

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,     //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													NULL,           //lSellerID
													&lBuyerID,      //lBuyerID
													NULL,           //lIID
													NULL,			//ullDBID
													NULL,           //lTID
													NULL,			//pvGridPos
													NULL,           //pvItemInfo
													NULL,			//pvSearchInfo
													pvCategoryCount,//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lBuyerID) );
		
		m_csCategoryCount.FreePacket(pvCategoryCount);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmAuctionClient::SendGetItemInfo()
{
	return TRUE;
}

//���ǿ� �������� �Ǵٰ� �˸���.(Sales Box�� ������ ���°Ŵ�.)
BOOL AgsmAuctionClient::SendSellItem( INT32 lSellerID, INT32 lItemTypeID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_SELL_ITEM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													&lSellerID,      //lSellerID
													NULL,			//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lSellerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

//SalesBox�� �ø� �������� �ȷȴٰ� �˸���.
BOOL AgsmAuctionClient::SendSoldItem( INT32 lSellerID, INT32 lItemTypeID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_SOLD_ITEM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													&lSellerID,      //lSellerID
													NULL,			//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lSellerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

///���ǿ� �ö�� �������� ��ٰ� �˸���.
BOOL AgsmAuctionClient::SendBuyItem( INT32 lBuyerID, INT32 lItemTypeID )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_BUY_ITEM;
	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													NULL,			//lSellerID
													&lBuyerID,		//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lBuyerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

//���ǿ� �ø� �������� ������Ų�ٰ� �˸���.(Sales Box���� ������ ����.)
BOOL AgsmAuctionClient::SendBackOutItem( INT32 lSellerID, INT32 lItemTypeID, INT8 lOperation )
{
	BOOL			bResult;

	bResult = FALSE;

	INT16	nPacketLength;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,    //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													&lSellerID,      //lSellerID
													NULL,			//lBuyerID
													NULL,			//lIID
													NULL,			//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													NULL,			//pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lSellerID) );
		
		m_csPacket.FreePacket(pvPacket);

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AgsmAuctionClient::SendSearchByPage( INT32 lBuyerID, INT32 lIndex, char *pstrSellerID, UINT64 ullDBID, INT32 lItemTypeID, INT32 lPrice, char *pstrDate )
{
	BOOL			bResult;

	bResult = FALSE;

	INT8	lOperation = AGPMAUCTION_GET_ITEM_COUNT;
	INT16	nPacketLength;
	void	*pvItemInfo;

	pvItemInfo  = m_csItemInfo.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
										lIndex,
										pstrSellerID,
										lPrice,
										NULL,
										pstrDate );

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,     //lOperation
													NULL,			//lControlServerID
													NULL,			//pstrSellerID
													NULL,           //lSellerID
													&lBuyerID,      //lBuyerID
													NULL,			//lIID
													&ullDBID,		//ullDBID
													&lItemTypeID,   //lTID
													NULL,			//pvInvenPos
													NULL,			//pvSalesBoxPos
													pvItemInfo,     //pvItemInfo
													NULL,			//pvSearchInfo
													NULL,			//pvCategoryCount
													NULL );         //pvlResult

	if( pvPacket != NULL )
	{
		bResult = SendPacket(pvPacket, nPacketLength, GetulNID(lBuyerID) );
		
		m_csItemInfo.FreePacket(pvItemInfo);
		m_csPacket.FreePacket(pvPacket);
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

//TID�� �ش��ϴ� ������ ������ Ȯ���ϰ� �����ش�.
BOOL AgsmAuctionClient::ProcessGetItemCount( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID )
{
	//DB�� �ش� �������� ������ ��´�.

	//������ �����ش�.
	//SendItemCount();

	return TRUE;
}

BOOL AgsmAuctionClient::ProcessGetItemInfo( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID )
{
	//DB���� �ش� �������� ����(Factor)�� ����.

	//SendItemInfo();
	
	return TRUE;
}

BOOL AgsmAuctionClient::ProcessSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos )
{
	BOOL			bResult;
	INT16			nInvenLayer, nInvenRow, nInvenColumn;
	INT16			nSaleLayer, nSaleRow, nSaleColumn;

	bResult = ParseInvenPos( pvInvenPos, &nInvenLayer, &nInvenRow, &nInvenColumn );
	
	if( bResult == TRUE )
	{
		bResult = ParseSalesBoxPos( pvSalesBoxPos, &nSaleLayer, &nSaleRow, &nSaleColumn );

		if( bResult == TRUE )
		{
			bResult = m_pagsmAuctionServer->PreprocessSellItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, nInvenLayer, nInvenRow, nInvenColumn, nSaleLayer, nSaleRow, nSaleColumn );
		}
	}
	
	return bResult;
}

BOOL AgsmAuctionClient::ProcessBuyItem( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID )
{
	return m_pagsmAuctionServer->PreprocessBuyItem( lControlServerID, lBuyerID, ullDBID, lTID );
}

BOOL AgsmAuctionClient::ProcessBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos, INT8 lOperation )
{
	BOOL			bResult;
	INT16			nInvenLayer, nInvenRow, nInvenColumn;
	INT16			nSaleLayer, nSaleRow, nSaleColumn;

	bResult = ParseInvenPos( pvInvenPos, &nInvenLayer, &nInvenRow, &nInvenColumn );
	
	if( bResult == TRUE )
	{
		bResult = ParseSalesBoxPos( pvSalesBoxPos, &nSaleLayer, &nSaleRow, &nSaleColumn );

		if( bResult == TRUE )
		{
			m_pagsmAuctionServer->PreprocessBackOutItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, nSaleLayer, nInvenLayer, nInvenRow, nInvenColumn, nSaleRow, nSaleColumn, lOperation );
		}
	}
	
	return bResult;
}

BOOL AgsmAuctionClient::ProcessSearchByPage( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID, void *pvSearchInfo )
{
	BOOL			bResult;
	bool			bSearchByMyLevel;
	INT32			lPage;

	bResult = ParseSearchInfo( pvSearchInfo, &bSearchByMyLevel, &lPage );

	if( bResult == TRUE )
	{
		//���ǿ� ���� �˻��Ѵ�.

		//�˻� �� ����� �����ش�.
		//SendSearchByPage();
	}

	return bResult;
}

//��Ŷ�� �ް� �Ľ�����~
BOOL AgsmAuctionClient::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
{
	//�ŷ���� Ȥ�� Error�޽����� ������ش�~ ����غ���~ ����غ���~
	BOOL			bResult;

	INT8			lOperation;
	INT32			lControlServerID;
	char			*pstrSellerID;
	INT32			lSellerID;
	INT32			lBuyerID;
	INT32			lIID;
	UINT64			ullDBID;
	INT32			lTID;
	void			*pvInvenPos;
	void			*pvSalesBoxPos;   //Sales Box �׸��� ��ġ.
	void			*pvItemInfo;  //������ ����.
	void			*pvSearchInfo;
	void			*pvCategoryCount;
	INT32			lResult;

	bResult = FALSE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&lOperation,
						&lControlServerID,
						&pstrSellerID,
						&lSellerID,
						&lBuyerID,
						&lIID,
						&ullDBID,
						&lTID,
						&pvInvenPos,
						&pvSalesBoxPos,
						&pvItemInfo,
						&pvSearchInfo,
						&pvCategoryCount,
						&lResult );

	//���� ���� ������ ��Ͻ�����~
	if( lOperation == AGPMAUCTION_ADD_TO_AUCTIONSERVER )
	{
		bResult = ProcessAddCIDToAuctionServer( lSellerID, ulNID );
	}
	//�����κ��� ���ϴ� �з��� ������ ������ ����.
	else if( lOperation == AGPMAUCTION_GET_ITEM_COUNT )
	{
		bResult = ProcessGetItemCount( lControlServerID, lBuyerID, lTID );
	}
	else if( lOperation == AGPMAUCTION_GET_ITEM_INFO )
	{
		bResult = ProcessGetItemInfo( lControlServerID, lBuyerID, lIID );
	}
	//�������� �Ǵ�.(Sales Box)�� �ø���.
	else if( lOperation == AGPMAUCTION_SELL_ITEM )
	{
		bResult = ProcessSellItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, pvInvenPos, pvSalesBoxPos );
	}
	//�������� ���.(�Խ��ǿ� �ö�� �������� ���.)
	else if( lOperation == AGPMAUCTION_BUY_ITEM )
	{
		bResult = ProcessBuyItem( lControlServerID, lBuyerID, ullDBID, lTID );
	}
	//�����Ѵ�. (2�� ���ĺ��� ���δ�. ������ �Ⱦ���.)
	else if( lOperation == AGPMAUCTION_BIDDING_ITEM )
	{
	}
	//�������� ������Ų��.
	else if( lOperation == AGPMAUCTION_BACK_OUT_TO_INVENTORY )
	{
		bResult = ProcessBackOutItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, pvInvenPos, pvSalesBoxPos, lOperation );
	}
	//�������� ������Ų��.
	else if( lOperation == AGPMAUCTION_BACK_OUT_TO_SALESBOX )
	{
		bResult = ProcessBackOutItem( lControlServerID, pstrSellerID, lSellerID, lIID, lTID, pvInvenPos, pvSalesBoxPos, lOperation );
	}
	//������ ������ �˻��Ѵ�.
	else if( lOperation == AGPMAUCTION_SEARCH_BY_PAGE )
	{
		bResult = ProcessSearchByPage(lControlServerID, lBuyerID, lTID, pvSearchInfo );
	}
	//���������� ��Ŷ�� �޾Ҵ�. ���d�d~
	else
	{
	}

	return TRUE;
}

//������ ����� �̸���´�. ���d�d~
BOOL AgsmAuctionClient::OnDisconnect(INT32 lAccountID)
{
	//lAccountID�� AGPMAUCTION_ADD_TO_AUCTIONSERVER�� �ް� CID, dpnid�� �����ڿ� 
	//SetIDToPlayerContext�� ���յǿ� �߻��� ���̴�. AccountID == CID�̴�.
	return m_csCIDManager.RemoveObject( lAccountID );
}
*/