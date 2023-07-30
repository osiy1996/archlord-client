/*============================================================================

	AgcmAuction.h

============================================================================*/

#ifndef _AGCM_AUCTION_H_
	#define _AGCM_AUCTION_H_


#include <ApBase/ApBase.h>
#include <AgcModule/AgcModule.h>
#include <AgpmCharacter/AgpdCharacter.h>
#include <AgpmItem/AgpdItem.h>


class AgcmCharacter;

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum AGCMAUCTION_CB
	{
	AGCMAUCTION_CB_NONE = 0,
	AGCMAUCTION_CB_SELECT,
	AGCMAUCTION_CB_AUCTION_UI_OPEN,
	AGCMAUCTION_CB_SALESBOX2_ADD,
	AGCMAUCTION_CB_SALESBOX2_REMOVE,
	AGCMAUCTION_CB_SALESBOX2_UPDATE,
	};


struct ApdEvent;

/************************************************/
/*		The Definition of AgcmAuction class		*/
/************************************************/
//
class AgcmAuction : public AgcModule
	{
	private:
		class ApmMap				*m_pApmMap;
		class ApmEventManager		*m_pApmEventManager;
		class AgpmCharacter			*m_pAgpmCharacter;
		class AgpmFactors			*m_pAgpmFactors;
		class AgpmItem				*m_pAgpmItem;
		class AgpmAuction			*m_pAgpmAuction;
		class AgcmConnectManager	*m_pAgcmConnectManager;
		class AgcmEventManager		*m_pAgcmEventManager;
		class AgcmCharacter			*m_pAgcmCharacter;

	public:
		AgcmAuction();
		virtual ~AgcmAuction();

		//	ApModule inherited
		BOOL	OnAddModule();

		//	Callback setting
		BOOL	SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackAuctionUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSalesBox2Add(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSalesBox2Remove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSalesBox2Update(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		//	Event callback
		static BOOL		CBRequestOpenAuction(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Module Callback
		static BOOL		CBAdd(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBRemove(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBUpdate(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL		CBSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBAuctionOpen(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBOpenAnywhere(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Packet
		BOOL	SendSelect(INT32 lCID, INT32 lItemTID, UINT64 ullDocID, INT16 nFlag, INT32 lCustom = 0);
		BOOL	SendSell(INT32 lCID, INT32 lItemID, INT16 nQuantity, INT32 lPrice);
		BOOL	SendCancel(INT32 lCID, INT32 lSalesID);
		BOOL	SendConfirm(INT32 lCID, INT32 lSalesID);
		BOOL	SendBuy(INT32 lCID, UINT64 ullDocID, INT16 nQuantity , INT64 nPrice , INT32 nItemTID );
		BOOL	SendOpenAuction(INT32 lCID, ApdEvent *pApdEvent);
		BOOL	SendOpenAnywhere(INT32 lCID);
	};


#endif
