#pragma once

#ifdef _AREA_TAIWAN_

#include "AgpmSkill.h"
#include "AgpmSystemMessage.h"

#include "AgcmCashMall.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmUIManager2.h"
#include "AuPacket.h"
#include "AgcModule.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgcmUIControl.h"

#define UI_MESSAGE_ID_COUPON_USE								"CashInven_ItemUse"				//	"사용"
#define UI_MESSAGE_ID_COUPON_DETAIL								"CashInven_ItemDetailInfo_Count"//	"아이템 이름"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO						"CashInven_ItemDetailInfo"		//	" %s%s 소지개수 : %d 개%s 남은 사용시간 : %s"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_DAY					"CashInven_ItemDetailInfoDay"	//	" %s%s 소지개수 : %d 개%s 남은 사용시간 : %d일 %d시간"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_HOUR				"CashInven_ItemDetailInfoHour"	//	" %s%s 소지개수 : %d 개%s 남은 사용시간 : %d시간 %d분"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_MINUTE				"CashInven_ItemDetailInfoMinute"//	" %s%s 소지개수 : %d 개%s 남은 사용시간 : %d분"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_SECOND				"CashInven_ItemDetailInfoSecond"//	" %s%s 소지개수 : %d 개%s 남은 사용시간 : %d초"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_REAL_TIME			"CashInven_ItemDetailInfoRealTime"//" %s%s 소지개수 : %d 개%s 만료 시간: %s"
#define UI_MESSAGE_ID_CASH_ITEM_USE								"CashInven_ItemUse"				//	"사용"
#define UI_MESSAGE_ID_CASH_ITEM_NO_USE							"CashInven_ItemNoUse"			//	"사용 중지"
#define UI_MESSAGE_ID_CASH_ITEM_USING							"CashInven_ItemUsing"			//	"사용중"
#define UI_MESSAGE_ID_CASH_ITEM_PAUSE							"CashInven_ItemPause"			//	"일시 정지"
#define UI_MESSAGE_ID_CASH_ITEM_WAIT							"CashInven_ItemWait"			// 대기가능
#define UI_MESSAGE_ID_CASH_ITEM_NO_USE_YET						"CashInven_ItemNoUseYet"		// 미사용
#define UI_MESSAGE_ID_CASH_ITEM_REUSE							"CashInven_ItemReUse"			// 재사용
#define UI_MESSAGE_ID_CASH_ITEM_HOLDING							"CashInven_ItemHolding"		// 사용대기
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_COUNT				"CashInven_ItemDetailInfo_Count"//	"아이템 이름"
#define	UI_MESSAGE_ID_CASH_ITEM_REMAIN_COUNT					"CashInven_ItemRemainCount"

#define AGCMUI_COUPON_BOX_MAX_COUNT								200

enum eAgcmUICouponBox_CallType
{
	AgcmUICouponBox_CALLTYPE_CASH_INVEN = 0,
	AgcmUICouponBox_CALLTYPE_BUFF_ICON,
	AgcmUICouponBox_CALLTYPE_TOOLTIP,
};

class AgcmCashMall;

class AgcmUICouponBox : public AgcModule
{
private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmGrid		*m_pcsAgpmGrid;
	AgpmItem		*m_pcsAgpmItem;
	AgpmCashMall	*m_pcsAgpmCashMall;
	AgpmSkill		*m_pcsAgpmSkill;
	AgpmSystemMessage* m_pcsAgpmSystemMessage;

	AgcmCharacter	*m_pcsAgcmCharacter;
	AgcmCashMall	*m_pcsAgcmCashMall;
	AgcmUIManager2	*m_pcsAgcmUIManager2;
	AgcmItem		*m_pcsAgcmItem;
	AgcmUIMain		*m_pcsAgcmUIMain;
	AgcmUIItem		*m_pcsAgcmUIItem;
	AgcmUIControl	*m_pcsAgcmUIControl;

	struct CouponItem
	{
		CouponItem() : item(0) {}
		CouponItem( stCouponInfo const & info, AgpdItem * item ) : info(info), item(item) {}

		stCouponInfo info;
		AgpdItem * item;
	};

	typedef std::vector< CouponItem > CouponItems;
	CouponItems couponItems_;

	typedef std::map< int, AgpdItem* > EraseCouponList;
	EraseCouponList eraseCouponList_;

public:
	AgcmUICouponBox();
	virtual	~AgcmUICouponBox();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

	BOOL	AddEvent();
	BOOL	AddFunction();
	BOOL	AddDisplay();
	BOOL	AddUserData();
	BOOL	AddBoolean();

	BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL	OnUpdateCashItemCount( void );

public:
	BOOL	RefreshItemGrid();

	static	BOOL	CBCashInvenClickUseItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBCashInvenOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static	BOOL	CBDisplayCashInvenUseItemButtonText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.11.14. By SungHoon
	//static	BOOL	CBDisplayCashInvenItemInfoTime(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.11.14. By SungHoon
	static	BOOL	CBDisplayCashInvenItemInfoName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.12.16. By SungHoon
	static	BOOL	CBDisplayCashInvenItemInfoCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.12.16. By SungHoon
	static	BOOL	CBDisplayCashInvenUsingItem(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);	// 2005.11.14. By SungHoon
	static	BOOL	CBDisplayCashInvenEnableTrade(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.12.02. By SungHoon

	static	BOOL	CBDisplayCashInvenStaminaMax(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);		// 2008.06.16. steeple
	static	BOOL	CBDisplayCashInvenStaminaCurrent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);		// 2008.06.16. steeple

	static	BOOL	CBUpdateItemCashInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBAddCoupon(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBUseCoupon(PVOID pData, PVOID pClass, PVOID pCustData);


	static	BOOL	CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static	BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);

	static	BOOL	CBGetItemTimeInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBGetItemStaminaTime(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	CheckPetStateGrid();

	bool	IsFull() { return m_lCashItemCount >= AGCMUI_COUPON_BOX_MAX_COUNT; }

public:
	BOOL	CashInvenOpenUI();
	void	Open();
	void	Close();
	BOOL	AddCoupon( stCouponInfo & couponInfo );
	BOOL	RemoveCoupon( size_t idx );

	void	EraseCouponItem( size_t idx );
	void	DestroyCouponItemAll();

	BOOL	IsSelfOperation(AgpdItem* pItem, AgpdCharacter* pCharacter);	// 2006.01.06. steeple
	void	EraseCouponItemAll();
public:
	AgcdUIUserData* m_pcsUDCashItemList;		//	2005.11.14. By SungHoon

//	Grid를 배열로 잡은건 List에 아이템으로 들어가기 때문에 각각이 하나의 Grid를 필요로 해서이다.
	AgpdGrid	m_acsGridCashItemList[AGCMUI_COUPON_BOX_MAX_COUNT];

	INT32	m_lCashItemCount;
	INT32	m_lEventOpenCashInvenUI;				//	2005.11.14. By SungHoon
	INT32	m_lEventCloseCashInvenUI;				//	2005.11.14. By SungHoon
	INT32	m_lCurrentTab;
	BOOL	m_bCashInvenOpenedUI;

	AgcdUIUserData* m_pcsUDStamina;					// 2008.06.16. steeple. Stamina UserData
	INT32	m_lUDStamina;							// 2008.06.16. steeple. Stamina UserData dummy value.

	AgpdGridItem* m_pcsPetState;					// 2008.06.20. steeple. Show pet's state on a system grid
	BOOL	m_bAddPetStateGrid;

	int usedCouponIdx_;
};

//-----------------------------------------------------------------------
//

class CouponDisplayInfo
{
public:
	typedef std::map< int, CouponDisplayInfo > CouponDisplayInfos;

	int				typeID;
	int				iconItemID;
	std::string		toolTipID;

	CouponDisplayInfo();

	static CouponDisplayInfo const & get( int couponType );

private:
	static CouponDisplayInfos _displayInfos;

	static void load();
};

//-----------------------------------------------------------------------

#endif