/*======================================================================

	AgcmUIProductSkill.h
	
======================================================================*/

#ifndef	_AGCM_UI_PRODUCT_SKILL_H_
	#define	_AGCM_UI_PRODUCT_SKILL_H_

#include <AgcModule/AgcModule.h>
#include <AgpmGrid/AgpdGrid.h>
#include <AgcmUIControl/AcUIToolTip.h>


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_GRAY		0XFFAEA59C
#define COLOR_RED		0xFFFF0000

enum eAGCMUIPRODUCTSKILL_EVENT
	{
	AGCMUIPRODUCTSKILL_EVENT_OPEN		 = 0,
	AGCMUIPRODUCTSKILL_EVENT_CLOSE,
	AGCMUIPRODUCTSKILL_EVENT_OPEN_NPCDIALOG,
	AGCMUIPRODUCTSKILL_EVENT_CLOSE_NPCDIALOG,
	AGCMUIPRODUCTSKILL_EVENT_BUY_ACTIVE_ITEM,
	AGCMUIPRODUCTSKILL_EVENT_BUY_DISABLE_ITEM,
	AGCMUIPRODUCTSKILL_EVENT_BUY_LEARNED_ITEM,
	AGCMUIPRODUCTSKILL_EVENT_BUY_SELECTED_ITEM,
	AGCMUIPRODUCTSKILL_EVENT_BUY_UNSELECTED_ITEM,
	AGCMUIPRODUCTSKILL_EVENT_BUY_SUCCESS,
	AGCMUIPRODUCTSKILL_EVENT_BUY_FAIL,
	AGCMUIPRODUCTSKILL_EVENT_SUCCESS,
	AGCMUIPRODUCTSKILL_EVENT_FAIL,
	AGCMUIPRODUCTSKILL_EVENT_MAX,
	};

enum eAGCMUIPRODUCTSKILL_DISPLAY_ID
	{
	AGCMUIPRODUCTSKILL_DISPLAY_ID_NAME		= 0,
	AGCMUIPRODUCTSKILL_DISPLAY_ID_LVEXP,
	AGCMUIPRODUCTSKILL_DISPLAY_ID_EXP_DEGREE,
	AGCMUIPRODUCTSKILL_DISPLAY_ID_EXP_DEGREE_MAX,
	AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_NAME,
	AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_PRICE,
	AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_INVENMONEY,
	AGCMUIPRODUCTSKILL_DISPLAY_ID_BUY_TITLE,
	};
	
#define AGCMUIPRODUCTSKILL_MAX_LIST			60

#define AGCMUIPRODUCTSKILL_CLOSE_UI_DISTANCE		150

struct AgcdUIUserData;
class AgcdUIControl;
enum AgcdUIDataType;
class AgpdCharacter;
struct ApdEvent;
enum eAGPMEVENT_PRODUCT_RESULT;
class AgpdComposeTemplate;
class AgpdSkill;

/********************************************************/
/*		The Definition of AgcmUIProductSkill class		*/
/********************************************************/
//
class AgcmUIProductSkill : public AgcModule
	{
	private:
		// related modules
		class AgpmGrid				*m_pAgpmGrid;
		class AgpmFactors			*m_pAgpmFactors;
		class AgpmCharacter			*m_pAgpmCharacter;
		class AgpmItem				*m_pAgpmItem;
		class AgpmSkill				*m_pAgpmSkill;
		class AgpmProduct			*m_pAgpmProduct;
		class AgpmEventProduct		*m_pAgpmEventProduct;
		class AgcmCharacter			*m_pAgcmCharacter;
		class AgcmItem				*m_pAgcmItem;
		class AgcmSkill				*m_pAgcmSkill;
		class AgcmProduct			*m_pAgcmProduct;
		class AgcmEventProduct		*m_pAgcmEventProduct;
		class AgcmUIManager2		*m_pAgcmUIManager2;
		class AgcmUICharacter		*m_pAgcmUICharacter;
		class AgcmUIMain			*m_pAgcmUIMain;
		class AgcmChatting2			*m_pAgcmChatting;
		class AgcmUIItem			*m_pAgcmUIItem;
		class AgcmUISkill2			*m_pAgcmUISkill2;

		//	Tooltip
		AcUIToolTip				m_csTooltip;

		// user data
		AgcdUIUserData			*m_pUDGrid;
		AgcdUIUserData			*m_pUDBuyGrid;
		AgcdUIUserData			*m_pUDInvenMoney;
		AgcdUIUserData			*m_pUDActiveBuyButton;
		AgcdUIUserData			*m_pUDTitle;

		// events
		INT32					m_lEvents[AGCMUIPRODUCTSKILL_EVENT_MAX];
		INT32					m_lEventConfirm;
		static CHAR				s_szEvents[AGCMUIPRODUCTSKILL_EVENT_MAX][64];

		// grid
		AgpdGrid				m_AgpdGridList[AGCMUIPRODUCTSKILL_MAX_LIST];
		INT32					m_lList[AGCMUIPRODUCTSKILL_MAX_LIST];
		INT16					m_nListCount;
		AgpdGrid				m_AgpdGridBuyList[AGCMUIPRODUCTSKILL_MAX_LIST];
		INT32					m_eCategory;
		ApdEvent				*m_pApdEvent;
		
		AgpdCharacter			*m_pSelfCharacter;
		INT64					m_llInvenMoney;
		INT32					m_lBuySelectedItemIndex;
		INT32					m_lDummy;
		AuPOS					m_stPosition;
		BOOL					m_bIsOpen;
		BOOL					m_bIsBuyOpen;

		// OnAddModule helper
		BOOL			AddFunction();
		BOOL			AddEvent();
		BOOL			AddUserData();
		BOOL			AddDisplay();
		BOOL			AddBoolean();

		// init UI
		BOOL			SetList();
		BOOL			SetBuyList();

		// Display helper
		eAGPMEVENT_PRODUCT_RESULT	IsActiveBuyItem(INT32 lIndex);
		eAGPMEVENT_PRODUCT_RESULT	IsActiveBuyItem(AgpdGrid *pAgpdGrid);
		BOOL						SetStatusBuyItem(INT32 lIndex);

		BOOL						SetRecastSkillTimeToGridItem(INT32 lSkillID, UINT32 ulDuration, UINT32 ulRecastDelay);

	public:
		AgcmUIProductSkill();
		virtual ~AgcmUIProductSkill();

		// ApModule inherited
		BOOL		OnAddModule();
		BOOL		OnInit();
		BOOL		OnDestroy();

		// Module callbacks
		static BOOL		CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBUpdateInvenMoney(PVOID pData, PVOID pClass, PVOID pCustData);	
		
		static BOOL		CBAddSkill(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBUpdateSkillPoint(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBReceiveSkillAction(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL		CBGrant(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBSelfUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL		CBBuyComposeResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBLearnSkillResult(PVOID pData, PVOID pClass, PVOID pCustData);

		// Function callbacks
		static BOOL		CBOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);	
		static BOOL		CBOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		static BOOL		CBBuyOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBBuySelectGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBBuyBuy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBBuyUpdateListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBBuyClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBBuyOpenTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
		static BOOL		CBBuyCloseTooltip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

		// Display callbacks
		static BOOL		CBDisplayName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
		static BOOL		CBDisplayLvExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
		static BOOL		CBDisplayExpBar(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
		
		static BOOL		CBDisplayBuyName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
		static BOOL		CBDisplayBuyPrice(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
		static BOOL		CBDisplayBuyInvenMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
		static BOOL		CBDisplayTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

		// Event return callbacks
		static BOOL		CBConfirmBuy(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

		//	Boolean callbacks
		static BOOL		CBIsActiveBuyButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);

		//	System driven UI close callback
		static BOOL CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Open
		BOOL	Open();

		//	Result		
		BOOL	OnResult( AgpdSkill *pAgpdSkill, INT32 lResult, INT32 lItemTID, INT32 lExp, int nCount );
		BOOL	OnBuyComposeResult(INT32 lResult, INT32 lID);
		BOOL	OnLearnSkillResult(INT32 lResult, INT32 lID);
		BOOL	AddSystemMessage(CHAR* pszMsg);
		
		//	Tooltip
		BOOL	SetTooltipText(AcUIToolTip *pcsTooltip, AgpdComposeTemplate *pAgpdComposeTemplate, BOOL bShowReceipe = FALSE);
	};

#endif