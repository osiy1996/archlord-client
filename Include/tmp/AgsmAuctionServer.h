/******************************************************************************
Module:  AgsmAuctionServer.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#ifndef __AGSM_AUCTION_SERVER_H__
#define __AGSM_AUCTION_SERVER_H__

#include "ApBase.h"
#include "AgsEngine.h"
#include "AuOLEDB.h"
#include "AgpmAuction.h"
#include "AgpmFactors.h"
#include "AgpmItem.h"
#include "AgsmServerManager2.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAuctionServerD" )
#else
#pragma comment ( lib , "AgsmAuctionServer" )
#endif

//Auction ť�� �ִ� ����.
class AgsmAuctionQueueInfo
{
public:
	INT8			m_nOperation;
	char			*m_pstrQueury;

	INT32			m_lSellerID;
	INT32			m_lBuyerID;
	INT32			m_lErrorCode;

	AgsmAuctionQueueInfo()
	{
		m_pstrQueury = NULL;
	}
	~AgsmAuctionQueueInfo()
	{
		if( m_pstrQueury != NULL )
			delete [] m_pstrQueury;
	}
};

enum eAgsmAuctionServerCB_ID
{
	AUCTION_CB_ID_SELL					= 0,	//
	AUCTION_CB_ID_SOLD,							//
	AUCTION_CB_ID_BUY,							//
	AUCTION_CB_ID_BID,							//
	AUCTION_CB_ID_BACKOUT_TO_SALESBOX,			//
	AUCTION_CB_ID_BACKOUT_TO_INVENTORY,			//
	AUCTION_CB_ID_SEARCH_BY_PAGE				//
};

class AgsmAuctionServer : public AgsModule
{
	AuPacket		m_csPacket;   //Auction ��Ŷ.
	AuPacket		m_csInvenPos;  //inven Grid ��ġ
	AuPacket		m_csSalesBoxPos;  //SalesBox Grid ��ġ

	AgpmFactors		*m_pagpmFactors;
	AuOLEDBManager	*m_paAuOLEDBManager;
	AgpmItem		*m_pagpmItem;
	AgsmServerManager	*m_pAgsmServerManager;

	ApAdmin			m_csRegisterCID; //��ϵ� CID

	int				m_iReservedQueueCount;

public:

	AgsmAuctionServer();
	~AgsmAuctionServer();

	void IncreaseQueueCount();
	void DecreaseQueueCount();
	BOOL CheckQueueCount();
	BOOL PushToQueue( char *pstrQuery, INT8 nOperation, INT32 lSellerID, INT32 lBuyerID, INT32 lErrorCode  );

	BOOL PreprocessSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn );
	BOOL PreprocessBuyItem( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID );
	BOOL PreprocessBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lTID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn, INT8 lOperation );

	BOOL ProcessSellItem(char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lTID, void *pvFieldInfo );
	BOOL ProcessSoldItem(char *pstrSellerID, INT32 lSellerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID );
	BOOL ProcessBuyItem(char *pstrSellerID, INT32 lSellerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID );
	BOOL ProcessBackOutItem(char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lTID, INT8 lOperation );

	//��� �ݹ�~
	BOOL SetCallbackSell(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSold(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBuy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBackOutToInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBackOutToSalesBox(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//���ǿ� �������� �Ǵٰ� �˸���.(Sales Box�� ������ ���°Ŵ�.)
	BOOL SendSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lItemID, INT32 lItemTypeID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn );
	///���ǿ� �ö�� �������� ��ٰ� �˸���.
	BOOL SendBuyItem( INT32 lControlServerID, char *pstrSellerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lItemTypeID );
	//���ǿ� �ø� �������� ������Ų�ٰ� �˸���.(Sales Box���� ������ ����.)
	BOOL SendBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, UINT64 ullDBID, INT32 lItemTypeID, INT16 nInvenLayer, INT16 nInvenRow, INT16 nInvenColumn, INT16 nSaleLayer, INT16 nSaleRow, INT16 nSaleColumn, INT8 lOperation );

	//��𿡳� �ִ� OnAddModule�̴�. Ư���Ұ� ����. Ư���ϸ� �ȵǳ�? ���d�d~
	BOOL OnAddModule();
	//��Ŷ�� �ް� �Ľ�����~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);
};

#endif
