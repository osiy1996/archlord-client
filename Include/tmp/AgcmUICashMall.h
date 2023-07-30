#pragma once

#include "AgcmCashMall.h"
#include "AgcmUIManager2.h"
#include "AgcmUIMain.h"

#define	AGCMUICASHMALL_MAX_DISPLAY_COUNT		6
#define	AGCMUICASHMALL_MAX_PAGE_COUNT			5
#define	AGCMUICASHMALL_MAX_DISPLAY_PAGE_SLOT	9

#define	AGCMUIMESSAGE_CASHMALL_KEEP_CASH		"KeepCash"			// 보유로니
#ifdef _AREA_GLOBAL_
#define	AGCMUIMESSAGE_CASHMALL_C_CASH		"CCash"			// 신용카드충전금액
#define	AGCMUIMESSAGE_CASHMALL_P_CASH		"PCash"			// PP카드충전금액
#endif
#define	AGCMUIMESSAGE_CASHMALL_UNIT				"Unit"				// 개
#define	AGCMUIMESSAGE_CASHMALL_USE_CASH			"UseCash"			// 사용할 로니
#define	AGCMUIMESSAGE_CASHMALL_BUY_RESULT		"BuyResult"			// 결제 후 잔액
#define	AGCMUIMESSAGE_CASHMALL_CHARGE_URL		"CashChargeURL"
#define	AGCMUIMESSAGE_CASHMALL_NOT_COMMIT_ON_DEAD	"NotCommitOnDead"
#ifdef _AREA_GLOBAL_
#define AGCMUIMESSAGE_CASHMALL_SELECT_BUY_METHOD	"SelectBuyMethod" //글로벌 서비스의 결제 수단인 W Coin 과 PP 둘중에 어느것으로 결제 할지를 물어보는 문구
#endif

class AgcmUICashMall : public AgcModule {
private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmGrid		*m_pcsAgpmGrid;
	AgpmItem		*m_pcsAgpmItem;
	AgpmCashMall	*m_pcsAgpmCashMall;
#ifdef _AREA_GLOBAL_
	AgpmBillInfo	*m_pagpmBillInfo;
#endif

	AgcmCharacter	*m_pcsAgcmCharacter;
	AgcmCashMall	*m_pcsAgcmCashMall;
	AgcmUIManager2	*m_pcsAgcmUIManager2;
	AgcmUIMain		*m_pcsAgcmUIMain;

	AgpdGrid		m_acsItemGrid[AGCMUICASHMALL_MAX_DISPLAY_COUNT];

	INT32			m_lCurrentTab;
	INT32			m_lStartPage;
	INT32			m_lCurrentPage;
	INT32			m_lCash;
	INT32			m_lCurrentPageItemCount;
	INT32			m_lMaxPage;
	INT32			m_lUserDataMaxPage;
//#ifdef _AREA_GLOBAL_
	INT32			m_iCashType;//글로벌 빌링에서 쓰이는 유저가 선택한 캐쉬 타입값이 저장되는 변수.
//#endif
	AgcdUIUserData	*m_pcsUserDataCurrentTab;
	AgcdUIUserData	*m_pcsUserDataCurrentPage;
	AgcdUIUserData	*m_pcsUserDataItemGrid;
	AgcdUIUserData	*m_pcsUserDataCash;
	AgcdUIUserData	*m_pcsUserDataBuyProduct;
	AgcdUIUserData	*m_pcsUserDataMaxPage;
#ifdef _AREA_GLOBAL_
	AcUIEdit		*m_pcsEditCash;//pp카드
	AcUIEdit		*m_pcsEditCash2;//신용카드
	AcUIEdit		*m_pcsEditCash3;//W coin ( pp카드 + 신용카드 )
#else
	AcUIEdit		*m_pcsEditCash;
#endif

	AcUIList		*m_pcsListControl;

	INT32			m_lEventGetEditControl;
#ifdef _AREA_GLOBAL_
	INT32			m_lEventGetEditControl2;
	INT32			m_lEventGetEditControl3;
#endif
	INT32			m_lEventGetListControl;
	INT32			m_lEventOpenCashMallUI;
	INT32			m_lEventCloseCashMallUI;

	INT32			m_lEventNormalProduct;
	INT32			m_lEventNewProduct;
	INT32			m_lEventHotProduct;
	INT32			m_lEventEventProduct;
	INT32			m_lEventSaleProduct;

#ifndef _AREA_GLOBAL_
	INT32			m_lEventSPCProduct;
	INT32			m_lEventTPackProduct;
#endif

	INT32			m_lEventSelectPageSlot[AGCMUICASHMALL_MAX_DISPLAY_PAGE_SLOT];

	INT32			m_lEventBuyProductConfirm;
	INT32			m_lEventNotEnoughCash;

	INT32			m_lEventBuyResultSuccess;
	INT32			m_lEventBuyResultNotEnoughCash;
	INT32			m_lEventBuyResultInventoryFull;
	INT32			m_lEventBuyResultNeedNewItemList;
	INT32			m_lEventBuyResultPCBangOnly;
	INT32			m_lEventBuyResultLimitedCount;
#ifdef _AREA_GLOBAL_
	INT32			m_lEventBuyResultNotPremiumService;
#else
	INT32			m_lEventBuyResultNotTPCBang;
	INT32			m_lEventBuyResultNotSPCBang;
#endif

	INT32			m_lEventOpenRefreshCashUI;

	BOOL			m_bReceiveMallInfo;

	BOOL			m_bIsOpenedUI;

	AgpdItem		*m_pcsBuyItem;

public:
	AgcmUICashMall();
	virtual ~AgcmUICashMall();

	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnDestroy();

private:
	BOOL	AddFunction();
	BOOL	AddUserData();
	BOOL	AddDisplay();
	BOOL	AddEvent();
	BOOL	AddBoolean();

	BOOL	SetItemGrid(INT32 lTab, INT32 lStartPage, INT32 lSelectPage);

	BOOL	RefreshCashEdit();
	BOOL	SetSelectPageSlot(INT32 lSelectPage);

public:
	static BOOL	CBUpdateMallList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUpdateCash(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBResponseBuyResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);

public:
	static BOOL	CBGetEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
#ifdef _AREA_GLOBAL_
	static BOOL	CBGetEditControl2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGetEditControl3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
#endif
	static BOOL	CBGetListControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCashCharge(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBCheckCash(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBBuyProductConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBBuyProduct(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBOpenCashMallUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSelectTab(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBPageLeft(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBPageRight(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBGoPage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL CBIsShowTabButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsShowChargeButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActivePage(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL CBIsActiveBuy(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

	static BOOL	CBDisplayProductName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayProductDesc(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayProductPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayTabName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayPageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayMaxPageNumber(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailValuePrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailValueUseCash(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyProductDetailValueRemainCash(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL	CBDisplayBuyMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	#ifdef _AREA_GLOBAL_
	static BOOL CBDisplayBuyProduct_PPCardCash(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	#endif
	// event return callback functions
	///////////////////////////////////////////////////////////////////////
	static BOOL CBReturnRefresh(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
};