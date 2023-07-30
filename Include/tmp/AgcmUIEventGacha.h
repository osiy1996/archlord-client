#pragma once

#include "ApmEventManager.h"
#include "AgcmUIManager2.h"
#include "AgcmEventManager.h"
#include "AcUIBase.h"
#include "AgcmEventGacha.h"

class AgcmUIEventGacha;

class AcUIGacha : public AcUIBase
{
public:
	AgcmUIEventGacha	*m_pcsAgcmUIEventGacha;
public:
	struct	SlotMachine
	{
		INT32		nTID		;
		RwTexture	*pTexture	;
		string		strName		;

		SlotMachine():nTID( 0 ) , pTexture( NULL ) {}
	};

	vector< SlotMachine >	m_vecSlot;
	SlotMachine	* m_pCurrentSlot;

	enum RollingMode
	{
		RM_DISPLAY,    // ���÷�����.. �׳� �����ӵ��� ��� ���ư�
		RM_ROLLING ,    // �Ѹ������� �� ����� �̷����.
		RM_RESULT        // ����� ���°��
	};
	RollingMode    m_eRollingMode; 

    INT32	m_nPosition		;
    INT32	m_nMaxPosition	;
	UINT32	m_uPrevTick		;

	UINT32	m_uRollingStartTime;
	INT32	m_nResultPosition;

	INT32	m_nResultStartOffset;

	INT32	m_nResultTID;

	AgcuPathWork *	m_pSplineData;

	BOOL	LoadGachaSetting( const char * pFileName );
	BOOL	SetSlotItems( vector< INT32 > * pVector );
    BOOL    StartRoll( INT32 nTIDResult );

	BOOL	IsRollingNow() { return m_eRollingMode == RM_ROLLING ? TRUE : FALSE; }

public:
	AcUIGacha();
	virtual ~AcUIGacha();

	virtual	void OnWindowRender	()	;
	virtual BOOL OnInit			()	;

	virtual BOOL OnPostInit		()	;

	virtual BOOL OnIdle			( UINT32 ulClockCount	);

	virtual void OnCloseUI();
};

class AgcmUIEventGacha : public AgcModule
{
public:
	ApmEventManager		*m_pcsApmEventManager	;
	AgcmUIManager2		*m_pcsAgcmUIManager2	;
	AgcmEventGacha		*m_pcsAgcmEventGacha	;

	INT32	m_lEventGachaOpen	;
	INT32	m_lEventGachaError	;
	INT32	m_lEventGachaRoll	;
	INT32	m_lEventGachaClose	;

	AcUIGacha	m_cUIGacha	;
	string		m_stringName;	// ���÷��̵Ǵ� �ؽ�Ʈ.

	AgpmEventGacha::GachaInfo	m_stGachaInfo;

	AgcdUIUserData* m_pcsUDItemName	;		
	AgcdUIUserData* m_pcsUDNotice	;	

	AuPOS		m_posGachaOpenPosition;
	BOOL		m_bOpenGachaUI;

public:
	void	UpdateControlNotice();	// ���� ���� ������Ʈ
	void	UpdateControlItem();	// ������ �̸� ������Ʈ

	BOOL				AddEvent();

	enum Const
	{
		GACHA_MESSAGE_LENGTH = 64
	};

	static const int	AGCMUIEVENTGACHA_CLOSE_UI_DISTANCE			= 150;


	void	ShowErrorReport( AgpmEventGacha::ERROR_CODE eErrorCode );

	AgcmUIEventGacha();
	virtual ~AgcmUIEventGacha();

	BOOL			OnAddModule();
	BOOL			OnRollEnd();	// ��í ���� �� ������ ����� ���°��

	virtual void OnLuaInitialize	( AuLua * pLua			);

	static BOOL		CBOpenGachaUI(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReceiveResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSelfCharacterUpdatePosition(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	 	CBGachaRoll	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	 	CBGachaClose	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplayGachaMessage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplayGachaItem(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl);
};
