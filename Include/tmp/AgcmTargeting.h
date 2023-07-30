#ifndef		_AGCMTARGETING_H_
#define		_AGCMTARGETING_H_

#include "AgcModule.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "rtpick.h"

#include "AgcmGlyph.h"
#include "AgcmMap.h"
#include "AgcmSkill.h"
#include "AgcmEventManager.h"
#include "AgcmCharacter.h"
#include "AgcmPrivateTrade.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "AgcmLogin.h"
#include "AgcmRender.h"
#include "AgcmParty.h"
#include "AgpmFactors.h"
#include "AgcmUIManager2.h"
#include "AgcmUILogin.h"
#include "AgcmUICharacter.h"
#include "AgcmUIMain.h"
#include "AgcmSiegeWar.h"

#include "AgcmShadow.h"
#include "AgcmOcTree.h"
#include "ApmOcTree.h"

#include "AgcSenderWindow.h"
#include "AgcmReturnToLogin.h"

#include <string>

#if _MSC_VER < 1300
	#ifdef	_DEBUG
		#pragma comment ( lib , "AgcmTargetingD" )
	#else
		#pragma comment ( lib , "AgcmTargeting" )
	#endif
#endif

#define AGCMTARGETING_TARGETLOCK_COLOR					0xff001fff

#define	AGCMTARGETING_INI_FILENAME						"FN"

#define AGCMTARGETING_DESTMOVE_IMAGE_NUM				5
#define AGCMTARGETING_DESTTARGET_IMAGE_NUM				9
#define AGCMTARGETING_DESTTARGET_LOCK_IMAGE_NUM			1
//@{ 2006/10/26 burumal
#define AGCMTARGETING_AREATARGETING_IMAGE_NUM			6
//@}

// 0.8�ʸ��� ������ ��ȯ
#define AGCMTARGETING_MCURSOR_NEXTTICK					800

#define	AGCMTARGETING_MCURSOR_NUM						5
#define	AGCMTARGETING_MCURSOR_MOVE_NUM					2
#define	AGCMTARGETING_MCURSOR_ATTACK_NUM				2
#define	AGCMTARGETING_MCURSOR_TALK_NUM					2
#define	AGCMTARGETING_MCURSOR_PICK_NUM					2
#define AGCMTARGETING_MCURSOR_NOMOVE_NUM				2

// Login ���� Callback
typedef enum AgcmTargetingCallbackPoint
{
	AGCMTARGETING_CB_ID_LBUTTONDOWN_PICK_SOMETHING	= 0,
	AGCMTARGETING_CB_ID_LBUTTONDOWN_PICK_NOTHING,	
} AgcmTargetingCallbackPoint;

typedef enum AgcmTargetingTargetMode
{
	AGCMTARGETING_TARGET_MODE_NONE	= 0 ,
	AGCMTARGETING_TARGET_MODE_GROUNDCLICK,
	AGCMTARGETING_TARGET_MODE_ATTACK,
	AGCMTARGETING_TARGET_MODE_ATTACKLOCK,
	//@{ 2006/10/26 burumal
	AGCMTARGETING_TARGET_MODE_AREATARGETING,
	//@}
} AgcmTargetingTargetMode;

typedef struct tag_PickedObject
{
	FLOAT				fDistance;
	INT32				eType;
	INT32				lID;

	//@{ 2006/03/06 burumal
	// ��Ƽ PickingAtomic�� Character Ÿ�Կ��� ����Ǿ� �����Ƿ� �ٸ� Ÿ���� �� pNewFirstPickAtomic�� ������� �ʴ´�
	RpAtomic*			pNewFirstPickAtomic;
	//@}

	RpClump*			pClump;

	tag_PickedObject*	next;

	ApBase *			pcsBase;

} PickedObject;

class AgcmTextBoardMng;

// AgcmTargeting
class AgcmTargeting : public AgcModule
{
public :
	
	AgcmTargeting();
	~AgcmTargeting();


	// module...
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);
	void	OnLuaInitialize( AuLua * pLua );

	// mouse event...
	void	OnMouseMove(RwV2d* pos);

	void	OnLButtonDown(RwV2d* pos);
	void	OnLButtonUp(RwV2d* pos);
	void	OnLButtonDblClk(RwV2d* pPos);

	void	OnRButtonDown(RwV2d* pos);
	void	OnRButtonDblClk(RwV2d* pPos);	


	// others...
	void	SetRwCamera(RwCamera* cam);

	void	SetRpWorld(RpWorld*	world) { m_pWorld = world; }


	// ������ (2005-03-16 ���� 2:43:37) : 
	// �ɸ��� �̵� ��ɸ� �и�. �ܺο��� Ư����ġ�� �̵� ����� �ٶ� �����.
	void	MoveSelfCharacter(AuPOS* pPos);
	

	static RpAtomic* CBIntersectAtomic(
							RpIntersection*	intersection,
							RpWorldSector* sector,
							RpAtomic* atomic,
							RwReal distance,
							void* data);				// Clump Type �˾Ƴ���


	static RpCollisionTriangle*	CBDetailCollision(
									RpIntersection*	intersection,
									RpCollisionTriangle* collTriangle,
									RwReal distance,
									void* data);		// Collision


	void	ClearPickList();

	//@{ 2006/03/06 burumal
	//void	AddPickedObject(RpClump* pClump, ApBase* pcsBase, INT32 eType, INT32 lID, FLOAT fDistance);
	void	AddPickedObject(RpClump* pClump, RpAtomic* pNewFirstPickAtomic, ApBase* pBase, INT32 eType, INT32 lID, FLOAT fDistance);
	//@}

	BOOL	StreamReadTemplate(CHAR* szFile, BOOL bDecryption);
	BOOL	StreamWriteTemplate(CHAR* szFile, BOOL bEncryption);

	
	static	BOOL	CBEncryptCodeSuccess(PVOID pData, PVOID pClass, PVOID pCustData);	// Login ������ �������� ���� �Ѵ� 
	static	BOOL	CBLoginProcessEnd(PVOID pData, PVOID pClass, PVOID pCustData);		// Login ������ ������
	static	BOOL	CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	

	static	BOOL	CBRButtonSkill( PVOID pData, PVOID pClass, PVOID pCustData )		;
	static	BOOL	CB_UpdateActionStatus( PVOID pData, PVOID pClass, PVOID pCustData ) ;

	static	BOOL	CB_LockTarget( PVOID pData, PVOID pClass, PVOID pCustData )			;
	static	BOOL	CB_UnlockTarget( PVOID pData, PVOID pClass, PVOID pCustData )		;

	static	BOOL	CBReturnToLoginEndProcess( PVOID pData, PVOID pClass, PVOID pCustData )	;
	
	//. 2006. 3. 7. Nonstopdj
	//. �������� ĳ���ͼ���ȭ������ ��ȯ�� ������ Target����� ������� NONE����.. 
	static	BOOL	CBSetTargetMode( PVOID pData, PVOID pClass, PVOID pCustData );

	VOID			SetShowCursor(BOOL bShow);

// data
	AgpmCharacter*		m_pAgpmCharacter		;
	AgcmGlyph*			m_pAgcmGlyph			;
	AgcmCharacter*		m_pAgcmCharacter		;
	ApmEventManager*	m_pApmEventManager		;
	AgcmEventManager*	m_pAgcmEventManager		;
	AgcmMap*			m_pAgcmMap				;
	AgcmSkill*			m_pAgcmSkill			;
	AgpmItem*			m_pAgpmItem				;
	AgcmItem*			m_pAgcmItem				;
	AgcmPrivateTrade*	m_pAgcmPrivateTrade		;
	AgcmRender*			m_pAgcmRender			;
	AgcmParty*			m_pAgcmParty			;
	AgpmFactors*		m_pAgpmFactors			;
	AgcmUIManager2*		m_pAgcmUIManager2		;
	AgcmUILogin*		m_pAgcmUILogin			;
	AgcmUICharacter*	m_pAgcmUICharacter		;
	AgcmOcTree*			m_pAgcmOcTree			;
	ApmOcTree*			m_pApmOcTree			;
	AgcmTextBoardMng*		m_pAgcmTextBoardMng		;
	ApmMap*				m_pApmMap				;

	AgcmShadow*			m_pAgcmShadow			;
	AgcmReturnToLogin*	m_pAgcmReturnToLogin	;

	AgpmSiegeWar*		m_pAgpmSiegeWar			;
	AgcmSiegeWar*		m_pAgcmSiegeWar			;

	RwCamera*			m_pCamera;
	RpWorld*			m_pWorld;

	INT32				m_iPickedNum;
	PickedObject*		m_listPicking;
	
	RwTexture*			m_pDestMove[AGCMTARGETING_DESTMOVE_IMAGE_NUM];

	//@{ 2006/10/26 burumal
	RwTexture*			m_pAreaTargetingTex[AGCMTARGETING_AREATARGETING_IMAGE_NUM];
	//@}
	
	HCURSOR				m_hMouseCursorMove	[ AGCMTARGETING_MCURSOR_MOVE_NUM	];
	HCURSOR				m_hMouseCursorAttack[ AGCMTARGETING_MCURSOR_ATTACK_NUM	];
	HCURSOR				m_hMouseCursorTalk	[ AGCMTARGETING_MCURSOR_TALK_NUM	];
	HCURSOR				m_hMouseCursorPick	[ AGCMTARGETING_MCURSOR_PICK_NUM	];
	HCURSOR				m_hMouseCursorNoMove[ AGCMTARGETING_MCURSOR_NOMOVE_NUM	];
//
//	// �̰� �� �ʿ��ϴ�... �ٸ������� Ŀ�� ����� �� �ֱ� ������.. (parn)
	INT32				m_lCursorMove	;
	INT32				m_lCursorAttack	;
	INT32				m_lCursorTalk	;
	INT32				m_lCursorPick	;
	INT32				m_lCursorNoMove	;

	INT32				m_iMouseCursorMode	;											// ���콺 Ŀ�� ���
	UINT32				m_uiLastTick;

	RwV2d				m_vCurMousePos;
	BOOL				m_bMouseLDown;

	AgcSenderWindow		m_clSenderWindow;

	INT32				m_iLastTargetImage;
	INT32				m_iTargetMode;					// 0 - none , 1 - ����Ŭ��(�̵�), 2 - ��Ŭ�� , 3 - Ÿ�ٶ�
	GlyphData*			m_pMoveTarget;					// �����ø� ���� ����

	//@{ 2006/10/26 burumal
	GlyphData*			m_pAreaTarget;
	FLOAT				m_fAreaTargetWidth, m_fAreaTargetHeight;
	//@}

	RpClump*			m_pEnemyTargetClump;			// �����ÿ� �˻縦 ���� clump

	RpClump*			m_pEnemyMouseOnClump;

	BOOL				m_bShowCursor;					// �⺻ Ŀ���� ���̰� �� ���ΰ�

	BOOL				m_bProcessMouseMove;			// OnMouseMove�� �ѹ��� ó���ϱ� ����
	BOOL				m_bProtectMove;					// ���� ĳ���� Ŭ���� �̵� ����

	BOOL				m_bShowGlyph;					// Glyph�� ���̰� �� ���ΰ�

	// ������ (2005-03-11 ���� 6:58:45) : 
	// Ŭ���̾�Ʈ �н����ε��� ����Ұų� ������
	INT32				m_lUsePathFind;

	INT32				m_lTargetType;
	INT32				m_lTargetID;
	INT32				m_lTargetTID;
	std::string			m_szTargetName;

	// ������ (2005-05-17 ���� 6:35:25) : 
	// �������� ������ NPC�� CID�� ����ص� ( ���߿� NPCDialog���� wave ��¿� ����� )
	INT32				m_lLastClickNPCID;
	INT32				GetLastNPCID() { return m_lLastClickNPCID; }

	// 2003 - 10 - 22 98pastel
	BOOL				m_bCurrentLoginMode	;			// Login ���ΰ��� ���� 
	BOOL				GetCurrentLoginMode() { return m_bCurrentLoginMode; }	// Login ���ΰ��� ���θ� ���´� 
	VOID				SetCurrentLoginMode(BOOL bMode) { m_bCurrentLoginMode = bMode; }
	// 2003 - 11 - 03 98pastel
	BOOL				SetCallbackLButtonDownPickSomething(ApModuleDefaultCallBack pfCallback, PVOID pClass );
	// 2003.12.30. steeple
	BOOL				SetCallbackLButtonDownPickNothing(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	RpClump*			GetCharacterClump( INT32	lCID )	;
	VOID				SetTargetBase(ApBase *	pcsBase);

	VOID				ShowTarget();

	//@{ 2006/03/08 burumal
	INT32				GetPickedNum() { return m_iPickedNum; }
	const PickedObject*	GetPickedObject() { return m_listPicking; }
	//@}

	//@{ 2006/10/26 burumal
	BOOL				StartAreaTargeting(FLOAT fAreaWidth, FLOAT fAreaHeight);
	VOID				StopAreaTargeting();
	VOID				UpdateAreaTargeting(AuPOS* pCurrentDestPos);
	BOOL				OnAreaTargetingDestPosDetected(RwV2d* pScreenPos);
	AgcmTargetingTargetMode	GetTargetMode() { return (AgcmTargetingTargetMode) m_iTargetMode; }
	//@}

	// 2006.10.27. steeple
	static BOOL			CBStartGroundTarget(PVOID pData, PVOID pClass, PVOID pCustData);

	// 2006/05/23 ������
	// Ÿ���� ��� ����.
	enum KeyTargetOption
	{
		KTO_PC		= 0x01,
		KTO_NPC		= 0x02,
		KTO_MOB		= 0x04,	// �ϴ� �̰� ���ַ� ����
		KTO_OBJ		= 0x08,
		// �������� ��..

		KTO_ALL		= 0x0F
	};

	enum SearchOption
	{
		KSO_NEAREST	,
		KSO_NEXT	,
		KSO_PREV
	};

	//BOOL	TargetNearest	( KeyTargetOption eOption = KTO_MOB );
	//BOOL	TargetNext		( KeyTargetOption eOption = KTO_MOB );
	//BOOL	TargetPrev		( KeyTargetOption eOption = KTO_MOB );

	FLOAT	m_fKeyTargettingRange;
	BOOL	TargetNear		( KeyTargetOption eOption = KTO_MOB , SearchOption sOption = KSO_NEXT );

	BOOL	TargetLast		();

	BOOL	TargetCID		( INT32 nCID , BOOL bAttack = FALSE );

	void	ClearTarget		();

	#ifdef _DEBUG
	VOID				TargetNearestDebug();
	VOID				TargetNextDebug();
	#endif // _DEBUG

	//@{ 2006/08/11 burumal
	static	BOOL		CBAutoTargetingForward(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	static	BOOL		CBAutoTargetingBackward(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pcsTarget, AgcdUIControl* pcsSourceControl);
	//@}

	//@{ 2006/10/31 burumal
	static	BOOL		CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData);
	//@}
};

#endif