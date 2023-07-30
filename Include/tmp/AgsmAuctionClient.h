/******************************************************************************
Module:  AgsmAuctionClient.h
Notices: Copyright (c) NHN Studio 2003 Ashulam
Purpose: 
Last Update: 2003. 1. 14
******************************************************************************/

#ifndef __AGSM_AUCTION_CLIENT_H__
#define __AGSM_AUCTION_CLIENT_H__

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgsEngine.h"
#include "AgpmAuction.h"
#include "AgsmAuctionServer.h"

#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAuctionClientD" )
#else
#pragma comment ( lib , "AgsmAuctionClient" )
#endif

class AgsmAuctionClient : public AgsModule
{
	AuPacket			m_csPacket;   //Auction ��Ŷ.
	AuPacket			m_csInvenPos;
	AuPacket			m_csSalesBoxPos;  //SalesBox Grid ��ġ
	AuPacket			m_csItemInfo;  //Item����.
	AuPacket			m_csSearchInfo; //�˻� ����.
	AuPacket			m_csCategoryCount; //ī�װ� ����

	AgsmAuctionServer	*m_pagsmAuctionServer;

	ApAdmin				m_csCIDManager;

public:
	AgsmAuctionClient();
	~AgsmAuctionClient();

	//���Ǽ������� ĳ����ID�� dpnid�� ����Ѵ�. ondestroy�� CID�� �ޱ� ���ؼ��̴�.
	BOOL ProcessAddCIDToAuctionServer( INT32 lSellerID, UINT32 ulNID );

	UINT32 GetulNID( INT32 lCID );

	static BOOL	CBAuctionSell(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionSold(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionBuy(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionBackoutToInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBAuctionBackOutToSalesBox(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL ProcessGetItemCount( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID );
	BOOL ProcessGetItemInfo( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID );
	BOOL ProcessSellItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos );
	BOOL ProcessBuyItem( INT32 lControlServerID, INT32 lBuyerID, UINT64 ullDBID, INT32 lTID );
	BOOL ProcessBackOutItem( INT32 lControlServerID, char *pstrSellerID, INT32 lSellerID, INT32 lIID, INT32 lTID, void *pvInvenPos, void *pvSalesBoxPos, INT8 lOperation );
	BOOL ProcessSearchByPage( INT32 lControlServerID, INT32 lBuyerID, INT32 lTID, void *pvSearchInfo );

	BOOL SendItemCount( INT32 lBuyerID, INT32 lItemTypeID, INT32 lItemCount );
	BOOL SendGetItemInfo();

	//���ǿ� �������� �Ǵٰ� Client�� �˸���.(Sales Box�� ������ ���°Ŵ�.)
	BOOL SendSellItem( INT32 lSellerID, INT32 lItemTypeID );
	//���ǿ� �ø� �������� �ȷȴٰ� Client�� �˸���
	BOOL SendSoldItem( INT32 lSellerID, INT32 lItemTypeID );
	///���ǿ� �ö�� �������� ��ٰ� Ŭ���̾�Ʈ�� �˸���.
	BOOL SendBuyItem( INT32 lBuyerID, INT32 lItemTypeID );
	//���ǿ� �ø� �������� ������Ų�ٰ� Client�� �˸���.(Sales Box���� ������ ����.)
	BOOL SendBackOutItem( INT32 lSellerID, INT32 lItemTypeID, INT8 lOperation );
	BOOL SendSearchByPage( INT32 lBuyerID, INT32 lIndex, char *pstrSellerID, UINT64 ullDBID, INT32 lItemTypeID, INT32 lPrice, char *pstrDate );
	
	//��𿡳� �ִ� OnAddModule�̴�. Ư���Ұ� ����. Ư���ϸ� �ȵǳ�? ���d�d~
	BOOL OnAddModule();

	BOOL ParseInvenPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol);
	BOOL ParseSalesBoxPos( PVOID pGridPos, INT16 *pnLayer, INT16 *pnRow, INT16 *pnCol);
	BOOL ParseSearchInfo( PVOID pvSearchInfo, bool *pbSearchByMyLevel, INT32 *plPage );

	//��Ŷ�� �ް� �Ľ�����~
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer);
	BOOL OnDisconnect(INT32 lAccountID);
};

#endif
