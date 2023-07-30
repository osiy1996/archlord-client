#ifndef	__AGCMUISPLITITEM_H__
#define	__AGCMUISPLITITEM_H__

#include <AgcModule/AgcModule.h>
#include <AgpmItem/AgpdItem.h>

const int AGCMUISPLITITEM_MAX_TITLE				= 128;

typedef enum _AgcmUISplitItemDisplayID {
	AGCMUISPLITITEM_DISPLAY_ID_TITLE			= 0,
	AGCMUISPLITITEM_DISPLAY_ID_STACKCOUNT,
} AgcmUISplitItemDisplayID;

enum eSplitItemCondition
{
	SplitItem_Item = 0,		// �Ϲ����� ������ ���� ����
	SplitItem_SalesTrue,	// �ŷ� �Խ��ǿ� ���� �ø���
	SplitItem_SalesWrong,	// �ŷ� �Խ��ǿ� ���� �ø��� ������ ������ ���
};

struct AgcdUIUserData;
class AgcdUIControl;
enum AgcdUIDataType;

class AgcmUISplitItem : public AgcModule {
private:
	class AgpmCharacter			*m_pcsAgpmCharacter;
	class AgpmItem				*m_pcsAgpmItem;
	class AgcmItem				*m_pcsAgcmItem;
	class AgcmUIManager2		*m_pcsAgcmUIManager2;
	class AgcmCharacter			*m_pcsAgcmCharacter;
	class AgcmUISystemMessage	*m_pcsAgcmUISystemMessage;

	INT32					m_lMinStackCount;
	INT32					m_lMaxStackCount;

	INT32					m_lStackCount;

	BOOL					m_bIsModalStatus;
	eSplitItemCondition		m_eTitleCondition;
	INT32					m_lStatus;	// Item

	CHAR					m_szTitle[AGCMUISPLITITEM_MAX_TITLE + 1];

	ApModuleDefaultCallBack	m_fpCallbackFunction;
	PVOID					m_pvClass;

	INT32					m_lDummyData;

	AgcdUIUserData			*m_pcsUserDataDummy;

	INT32					m_lEventOpenSplitItem;
	INT32					m_lEventGetStackCountEditControl;
	INT32					m_lEventChangeSplitItemTitle;
	INT32					m_lEventCloseUI;

private:
	BOOL		AddDisplay();
	BOOL		AddUserData();
	BOOL		AddFunction();
	BOOL		AddEvent();

public:
	AgcmUISplitItem();
	virtual ~AgcmUISplitItem();

	BOOL		OnAddModule();
	BOOL		OnInit();

	BOOL		SetCondition(eSplitItemCondition titleCondition, ApModuleDefaultCallBack fpCallback, PVOID pvClass,
							INT32 lMinStackCount, INT32 lMaxStackCount, INT32 lStartCount = 1, BOOL bIsModalStatus = FALSE,
							INT32 lStatus = AGPDITEM_STATUS_INVENTORY);

	BOOL		OpenSplitItemUI();

	static BOOL	CBButtonOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey4(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey5(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey6(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey7(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey8(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKey9(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKeyClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBButtonKeyBack(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBGetStackCountEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBChangeTitleEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
};


#endif	//__AGCMUISPLITITEM_H__