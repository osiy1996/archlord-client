#ifndef _AGCMUI_CASHINVEN_H_
#define _AGCMUI_CASHINVEN_H_

#include <AgcModule/AgcModule.h>
#include <AuPacket/AuPacket.h>
#include <AgpmGrid/AgpdGrid.h>
#include <AgpmItem/AgpdItem.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUICashInvenD" )
#else
#pragma comment ( lib , "AgcmUICashInven" )
#endif
#endif

#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO						"CashInven_ItemDetailInfo"		//	" %s%s �������� : %d ��%s ���� ���ð� : %s"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_DAY					"CashInven_ItemDetailInfoDay"	//	" %s%s �������� : %d ��%s ���� ���ð� : %d�� %d�ð�"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_HOUR				"CashInven_ItemDetailInfoHour"	//	" %s%s �������� : %d ��%s ���� ���ð� : %d�ð� %d��"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_MINUTE				"CashInven_ItemDetailInfoMinute"//	" %s%s �������� : %d ��%s ���� ���ð� : %d��"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_SECOND				"CashInven_ItemDetailInfoSecond"//	" %s%s �������� : %d ��%s ���� ���ð� : %d��"
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_REAL_TIME			"CashInven_ItemDetailInfoRealTime"//" %s%s �������� : %d ��%s ���� �ð�: %s"
#define UI_MESSAGE_ID_CASH_ITEM_USE								"CashInven_ItemUse"				//	"���"
#define UI_MESSAGE_ID_CASH_ITEM_NO_USE							"CashInven_ItemNoUse"			//	"��� ����"
#define UI_MESSAGE_ID_CASH_ITEM_USING							"CashInven_ItemUsing"			//	"�����"
#define UI_MESSAGE_ID_CASH_ITEM_PAUSE							"CashInven_ItemPause"			//	"�Ͻ� ����"
#define UI_MESSAGE_ID_CASH_ITEM_WAIT							"CashInven_ItemWait"			// ��Ⱑ��
#define UI_MESSAGE_ID_CASH_ITEM_NO_USE_YET						"CashInven_ItemNoUseYet"		// �̻��
#define UI_MESSAGE_ID_CASH_ITEM_REUSE							"CashInven_ItemReUse"			// ����
#define UI_MESSAGE_ID_CASH_ITEM_HOLDING							"CashInven_ItemHolding"		// �����
#define UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_COUNT				"CashInven_ItemDetailInfo_Count"//	"������ �̸�"
#define	UI_MESSAGE_ID_CASH_ITEM_REMAIN_COUNT					"CashInven_ItemRemainCount"

#define	AGCMUICASHINVEN_TAB_ID_ALL						0

#define AGCMUICASHINVENTORY_MAX_ITEM					AGPMITEM_CASH_INVENTORY_ROW

enum eAgcmUICashInven_CallType
{
	AGCMUICASHINVEN_CALLTYPE_CASH_INVEN = 0,
	AGCMUICASHINVEN_CALLTYPE_BUFF_ICON,
	AGCMUICASHINVEN_CALLTYPE_TOOLTIP,
};

class AgcdUIControl;
struct AgcdUIUserData;
class AgpdCharacter;
enum AgcdUIDataType;

/*
	2005.11.10 By SungHoon
	ĳ�� �κ��丮 ���õ� UI ó�� �۾��� �´´�.
*/
class AgcmUICashInven : public AgcModule
{
private:
	class AgpmCharacter		*m_pcsAgpmCharacter;
	class AgpmGrid			*m_pcsAgpmGrid;
	class AgpmItem			*m_pcsAgpmItem;
	class AgpmCashMall		*m_pcsAgpmCashMall;
	class AgpmSkill			*m_pcsAgpmSkill;
	class AgpmSystemMessage* m_pcsAgpmSystemMessage;

	class AgcmCharacter		*m_pcsAgcmCharacter;
	class AgcmCashMall		*m_pcsAgcmCashMall;
	class AgcmUIManager2	*m_pcsAgcmUIManager2;
	class AgcmItem			*m_pcsAgcmItem;
	class AgcmUIMain		*m_pcsAgcmUIMain;
	class AgcmUIItem		*m_pcsAgcmUIItem;
	class AgcmUIControl		*m_pcsAgcmUIControl;

public:
	AgcmUICashInven();
	virtual	~AgcmUICashInven();

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
	static	BOOL	CBDisplayCashInvenItemInfoTime(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.11.14. By SungHoon
	static	BOOL	CBDisplayCashInvenItemInfoName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.12.16. By SungHoon
	static	BOOL	CBDisplayCashInvenItemInfoCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.12.16. By SungHoon
	static	BOOL	CBDisplayCashInvenUsingItem(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);	// 2005.11.14. By SungHoon
	static	BOOL	CBDisplayCashInvenEnableTrade(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);	// 2005.12.02. By SungHoon

	static	BOOL	CBDisplayCashInvenStaminaMax(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);		// 2008.06.16. steeple
	static	BOOL	CBDisplayCashInvenStaminaCurrent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);		// 2008.06.16. steeple

	static	BOOL	CBUpdateItemCashInventory(PVOID pData, PVOID pClass, PVOID pCustData);

	static	BOOL	CBAddCashItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBRemoveCashItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static	BOOL	CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static	BOOL	CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);

	static	BOOL	CBGetItemTimeInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL	CBGetItemStaminaTime(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetItemInfoTime(AgpdItem *pcsItem, CHAR *pszText, eAgcmUICashInven_CallType eCallType = AGCMUICASHINVEN_CALLTYPE_CASH_INVEN);
	BOOL	GetItemStaminaTime(AgpdItem* pcsItem, CHAR* pszText);

	BOOL	CheckPetStateGrid();

protected:
	BOOL	CashInvenOpenUI();
	BOOL	AddCashItem(AgpdItem *pItem, AgpdCharacter *pCharacter);
	BOOL	RemoveCashItem(AgpdItem *pItem, AgpdCharacter *pCharacter);

	BOOL	IsSelfOperation(AgpdItem* pItem, AgpdCharacter* pCharacter);	// 2006.01.06. steeple
private:
	AgcdUIUserData* m_pcsUDCashItemList;		//	2005.11.14. By SungHoon

//	Grid�� �迭�� ������ List�� ���������� ���� ������ ������ �ϳ��� Grid�� �ʿ�� �ؼ��̴�.
	AgpdGrid	m_acsGridCashItemList[AGCMUICASHINVENTORY_MAX_ITEM];

	INT32	m_lCashItemCount;
	INT32	m_lEventOpenCashInvenUI;				//	2005.11.14. By SungHoon
	INT32	m_lEventCloseCashInvenUI;				//	2005.11.14. By SungHoon
	INT32	m_lEventOpenCouponBox;
	INT32	m_lCurrentTab;
	BOOL	m_bCashInvenOpenedUI;

	AgcdUIUserData* m_pcsUDStamina;					// 2008.06.16. steeple. Stamina UserData
	INT32	m_lUDStamina;							// 2008.06.16. steeple. Stamina UserData dummy value.

	AgpdGridItem* m_pcsPetState;					// 2008.06.20. steeple. Show pet's state on a system grid
	BOOL	m_bAddPetStateGrid;
};


#endif
