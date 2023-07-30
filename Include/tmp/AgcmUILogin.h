#pragma once

#include "ApModule.h"
#include "AgpdCharacter.h"
#include "AgcmUIManager2.h"
#include "AgcmLogin.h"

#include "AgcuUIControlCamera.h"
#include "AgcuUIControlCharacter.h"
#include "AgcuUIControlPostFX.h"
#include "AgcuUILoadingWindow.h"

#include "AgcmUIAccountCheck.h"
#include "AgcmUIServerSelect.h"
#include "AgcmUICharacterCreate.h"
#include "AgcmUICharacterSelect.h"
#include "AgcmUILoginSetting.h"

#include "AgcmUILoginSelect.h"

#define			AGCMUILOGIN_MY_CHARACTER_NUM			3
#define			AGCMUILOGIN_MY_CHARACTER_INFO			AGPACHARACTER_MAX_ID_STRING + 1
#ifdef _AREA_GLOBAL_
#define			AGCMUILOGIN_TERM_UPDATE_SERVER			50000		
#else
#define			AGCMUILOGIN_TERM_UPDATE_SERVER			10000
#endif

enum
{
	AGCMUILOGIN_CAMERA_HUMAN_SELECT	= 0,
	AGCMUILOGIN_CAMERA_HUMAN_CREATE,
	AGCMUILOGIN_CAMERA_ORC_SELECT, 
	AGCMUILOGIN_CAMERA_ORC_CREATE						
};

//------------------- Event ------------------------
const enum eAGCMUILOGIN_EVENT
{
	AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_OPEN = 0,
	AGCMUILOGIN_EVENT_ID_PASSWORD_1280_INPUT_OPEN,
	AGCMUILOGIN_EVENT_ID_PASSWORD_1600_INPUT_OPEN,
	AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_CLOSE,
	AGCMUILOGIN_EVENT_2D_OPEN,
	AGCMUILOGIN_EVENT_2D_1280_OPEN,
	AGCMUILOGIN_EVENT_2D_1600_OPEN,
	AGCMUILOGIN_EVENT_2D_CLOSE,
	AGCMUILOGIN_EVENT_UNION_SELECT_OPEN,
	AGCMUILOGIN_EVENT_UNION_SELECT_CLOSE,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN_1280,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_OPEN_1600,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_CLOSE,
	AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_OPEN,
	AGCMUILOGIN_EVENT_MAIN_LOGIN_BUTTONS_CLOSE,
	AGCMUILOGIN_EVENT_ID_PASSWORD_FAIL,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAIL_SAME_NAME,
	AGCMUILOGIN_EVENT_CONNECT_LOGIN_SERVER,
	AGCMUILOGIN_EVENT_LOADING_WINDOW_OPEN,
	AGCMUILOGIN_EVENT_LOADING_WINDOW_CLOSE,
	AGCMUILOGIN_EVENT_GAME_SERVER_NOT_READY,
	AGCMUILOGIN_EVENT_INVALID_CHAR_NAME_INPUT,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_HELP_OPEN,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_HELP_CLOSE,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAIL_INVALID_NAME,
	AGCMUILOGIN_EVENT_DISCONNECT_BY_INVALID_PW,
	AGCMUILOGIN_EVENT_CANNOT_REMOVE_DUPLICATE_ACCOUNT,
	AGCMUILOGIN_EVENT_DISCONNECT_BY_ANOTHER_USER,
	AGCMUILOGIN_EVENT_OPEN_EXIT_WINDOW,
	AGCMUILOGIN_EVENT_CONNECT_FAILED_LOGIN_SERVER,
	AGCMUILOGIN_EVENT_PASSWORD_INCORRECT,
	AGCMUILOGIN_EVENT_OPEN_SELECT_SERVER_WINDOW,
	AGCMUILOGIN_EVENT_CLOSE_SELECT_SERVER_WINDOW,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_NOT_SELECTED,
	AGCMUILOGIN_EVENT_ENTER_GAME_NOT_SELECTED_CHAR,
	AGCMUILOGIN_EVENT_LOADING_HUMAN_BACKGROUND,
	AGCMUILOGIN_EVENT_LOADING_ORC_BACKGROUND,
	AGCMUILOGIN_EVENT_LOADING_MOONELF_BACKGROUND,
	AGCMUILOGIN_EVENT_ON_CHARACTER_MARK,
	AGCMUILOGIN_EVENT_OFF_CHARACTER_MARK,
	AGCMUILOGIN_EVENT_ON_SELECT_SERVER_BAR,
	AGCMUILOGIN_EVENT_OFF_SELECT_SERVER_BAR,
	AGCMUILOGIN_EVENT_SERVER_STATUS_GOOD,		// status
	AGCMUILOGIN_EVENT_SERVER_STATUS_NORMAL,
	AGCMUILOGIN_EVENT_SERVER_STATUS_BUSY,
	AGCMUILOGIN_EVENT_SERVER_STATUS_FULL,
	AGCMUILOGIN_EVENT_SERVER_STATUS_CLOSE,
	AGCMUILOGIN_EVENT_SERVER_RATE_15,			//
	AGCMUILOGIN_EVENT_SERVER_RATE_17,			//
	AGCMUILOGIN_EVENT_SERVER_RATE_NONE,
	AGCMUILOGIN_EVENT_SERVER_OPEN_EVENT,
	AGCMUILOGIN_EVENT_SERVER_OPEN_NEW,
	AGCMUILOGIN_EVENT_SERVER_OPEN_OLD,	//event or new or old...
	AGCMUILOGIN_EVENT_SERVER_OPEN_TEST	,
	AGCMUILOGIN_EVENT_SERVER_PLUS_ON,
	AGCMUILOGIN_EVENT_SERVER_PLUS_OFF,
	AGCMUILOGIN_EVENT_SERVER_MINUS_ON,
	AGCMUILOGIN_EVENT_SERVER_MINUS_OFF,
	AGCMUILOGIN_EVENT_SERVER_WORLD,
	AGCMUILOGIN_EVENT_SERVER_SERVER,
	AGCMUILOGIN_EVENT_SLOT01_NOT_VIEW,			// slot
	AGCMUILOGIN_EVENT_SLOT01_UNSELECT,
	AGCMUILOGIN_EVENT_SLOT01_SELECT,
	AGCMUILOGIN_EVENT_SLOT02_NOT_VIEW,
	AGCMUILOGIN_EVENT_SLOT02_UNSELECT,
	AGCMUILOGIN_EVENT_SLOT02_SELECT,
	AGCMUILOGIN_EVENT_SLOT03_NOT_VIEW,
	AGCMUILOGIN_EVENT_SLOT03_UNSELECT,
	AGCMUILOGIN_EVENT_SLOT03_SELECT,		
	AGCMUILOGIN_EVENT_CONFIRM_PASSWORD,			//
	AGCMUILOGIN_EVENT_CONFIRM_PASSWORD_FAIL,
	AGCMUILOGIN_EVENT_DUPLICATE_ACCOUNT,
	AGCMUILOGIN_EVENT_GAME_SERVER_FULL,
	AGCMUILOGIN_EVENT_CHARACTER_CREATE_FAIL_BUSY,
	AGCMUILOGIN_EVENT_NOT_ENOUGH_AGE,
	AGCMUILOGIN_EVENT_CHARACTER_RENAME_OPEN,
	AGCMUILOGIN_EVENT_CHARACTER_RENAME_CLOSE,
	AGCMUILOGIN_EVENT_CHARACTER_RENAME_FAIL,
	AGCMUILOGIN_EVENT_CHARACTER_RENAME_FAIL_SAME_OLD_NEW_ID,
	AGCMUILOGIN_EVENT_SHOW_COMPEN_CHAR01,
	AGCMUILOGIN_EVENT_SHOW_COMPEN_CHAR02,
	AGCMUILOGIN_EVENT_SHOW_COMPEN_CHAR03,
	AGCMUILOGIN_EVENT_HIDE_COMPEN,
	AGCMUILOGIN_EVENT_END,
	AGCMUILOGIN_EVENT_REMOVE_MYCHARACTER_QUESTION,
	AGCMUILOGIN_EVENT_EXIST_DUPLICATE_ACCOUNT,
	AGCMUILOGIN_EVENT_DISCONNECTED_BY_SERVER,
	AGCMUILOGIN_EVENT_ID_FALIED,
	AGCMUILOGIN_EVENT_PASSWORD_FALIED,
	AGCMUILOGIN_EVENT_NOT_BETA_TESTER,
	AGCMUILOGIN_EVENT_NOT_APPLIED,
	AGCMUILOGIN_EVENT_OPEN_MOVE_HANGAME,
	AGCMUILOGIN_EVENT_MAX
};

//------------------- Message ------------------------
#define	UI_MESSAGE_ID_LG_RACE_HUMAN							"Human"
#define	UI_MESSAGE_ID_LG_RACE_ORC							"Orc"
#define	UI_MESSAGE_ID_LG_RACE_MOONELF						"MoonElf"
#define	UI_MESSAGE_ID_LG_RACE_DRAGONSCION					"DragonScion"
#define	UI_MESSAGE_ID_LG_CLASS_KNIGHT						"Knight"
#define	UI_MESSAGE_ID_LG_CLASS_ARCHER						"Archer"
#define	UI_MESSAGE_ID_LG_CLASS_MAGE							"Mage"
#define	UI_MESSAGE_ID_LG_CLASS_BERSERKER					"Berserker"
#define	UI_MESSAGE_ID_LG_CLASS_HUNTER						"Hunter"
#define	UI_MESSAGE_ID_LG_CLASS_SORCERER						"Sorcerer"
#define UI_MESSAGE_ID_LG_CLASS_SWASHBUCKLER					"SwashBuckler"
#define	UI_MESSAGE_ID_LG_CLASS_RANGER						"Ranger"
#define	UI_MESSAGE_ID_LG_CLASS_ELEMENTALIST					"Elementalist"
#define UI_MESSAGE_ID_LG_CLASS_SCION						"Scion"
#define UI_MESSAGE_ID_LG_CLASS_SLAYER						"Slayer"
#define UI_MESSAGE_ID_LG_CLASS_OBITER						"Obiter"
#define UI_MESSAGE_ID_LG_CLASS_SUMMERNER					"Summerner"
#define	UI_MESSAGE_ID_LG_HEAD_TYPE_1						"HeadType1"
#define	UI_MESSAGE_ID_LG_FACE_TYPE_1						"FaceType1"
#define	UI_MESSAGE_ID_SERVERSTATUS_GOOD						"ServerStatusGood"
#define	UI_MESSAGE_ID_SERVERSTATUS_ABOVE_NORMAL				"ServerStatusAboveNormal"
#define	UI_MESSAGE_ID_SERVERSTATUS_NORMAL					"ServerStatusNormal"
#define	UI_MESSAGE_ID_SERVERSTATUS_BELOW_NORMAL				"ServerStatusBelowNormal"
#define	UI_MESSAGE_ID_SERVERSTATUS_BAD						"ServerStatusBad"
#define	UI_MESSAGE_ID_SERVERSTATUS_UNKNOWN					"ServerStatusUnknown"
#define	UI_MESSAGE_ID_LOGIN_SLOT_CHAR_LEVEL					"LoginSlotCharLevel"
#define	UI_MESSAGE_ID_LOGIN_SLOT_CHAR_EXP					"LoginSlotCharExp"
#define	UI_MESSAGE_ID_LOGIN_CONFIRM_PASSWORD				"LoginConfirmPassword"
#define	UI_MESSAGE_ID_LOGIN_ZOOM_IN							"LoginZoomIn"
#define	UI_MESSAGE_ID_LOGIN_ZOOM_OUT						"LoginZoomOut"
#define UI_MESSAGE_ID_RACE_DESC_HUMAN						"RaceDescHuman"
#define UI_MESSAGE_ID_RACE_DESC_ORC							"RaceDescOrc"
#define UI_MESSAGE_ID_RACE_DESC_MOONELF						"RaceDescMoonElf"
#define UI_MESSAGE_ID_RACE_DESC_DRAGONSCION					"RaceDescDragonScion"
#define UI_MESSAGE_ID_ALEART_NOT_SERVICE					"AleartNotSeriveYet"
#define	UI_MESSAGE_ID_CHAR_DESC_HUMAN_KNIGHT				"CharDescHumanKnight"
#define	UI_MESSAGE_ID_CHAR_DESC_HUMAN_ARCHER				"CharDescHumanArcher"
#define	UI_MESSAGE_ID_CHAR_DESC_HUMAN_WIZARD				"CharDescHumanWizard"
#define	UI_MESSAGE_ID_CHAR_DESC_ORC_BERSERKER				"CharDescOrcBerserker"
#define	UI_MESSAGE_ID_CHAR_DESC_ORC_HUNTER					"CharDescOrcHunter"
#define	UI_MESSAGE_ID_CHAR_DESC_ORC_SORCERER				"CharDescOrcSorcerer"
#define	UI_MESSAGE_ID_CHAR_DESC_MOONELF_KNIGHT				"CharDescMoonElfKnight"
#define	UI_MESSAGE_ID_CHAR_DESC_MOONELF_ARCHER				"CharDescMoonElfArcher"
#define	UI_MESSAGE_ID_CHAR_DESC_MOONELF_WIZARD				"CharDescMoonElfWizard"
#define	UI_MESSAGE_ID_CHAR_DESC_DRAGONSCION_SCION			"CharDescDragonScionScion"
#define UI_MESSAGE_ID_LOGIN_RENAME							"LoginRename"
#define UI_MESSAGE_ID_LOGIN_CONNECT							"LoginConnect"
#define UI_MESSAGE_ID_COMPENSATION_CONFIRM					"LoginCompenConfirm"
#define UI_MESSAGE_ID_COMPENSATION_CANCEL					"LoginCompenCancel"
#define UI_MESSAGE_ID_COMPEN_CAPTION_PAY					"LoginCompenCapPay"
#define UI_MESSAGE_ID_COMPENSATION_ITEM_PAID				"LoginCompenItemPaid"
#define UI_MESSAGE_ID_COMPEN_CAPTION_CANCEL					"LoginCompenCapCancel"
#define UI_MESSAGE_ID_ACCOUNT_BLOCKED						"LoginAccountBlocked"
#define UI_MESSAGE_ID_CANT_DELETE_1DAY						"LoginCantDelete1Day"
#define UI_MESSAGE_ID_CANT_DELETE_MASTER					"GuildBattleMember_NotEraseMaster"
#define UI_MESSAGE_ID_JUMPING_PROMOTION						"JumpingPromotion"


class AgcmUILogin : public AgcModule, public AgcmUIAccountCheck, public AgcmUIServerSelect,
	public AgcmUICharacterCreate
{
	// ����
public :
	INT32			m_lEvent[AGCMUILOGIN_EVENT_MAX];
	static TCHAR	s_szEvent[AGCMUILOGIN_EVENT_MAX][50];

	AgcuUIControlCharacter			m_ControlCharacter;	// ĳ���� ��Ʈ�ѷ�
	AgcuUIControlCamera				m_ControlCamera;	// ī�޷� ��Ʈ�ѷ�
	AgcuUIControlPostFX				m_ControlPostFX;	// PostFX ��Ʈ�ѷ�
	AgcuUILoadingWindow				m_LoadingWindow;	// �ε������� ��Ʈ�ѷ�
	AgcmLogin*						m_pcmLogin;
	AgcmUIManager2*					m_pcsAgcmUIManager2;

	INT32			m_iBackUpDrawRange;					// ������ ������ �����
	ApWorldSector*	m_pPrevSector;						// Login�߿� camera��ġ�� ���� ���� ��ġ ����

	BOOL			m_bLoadSImpleUI;					// ������ UI �� �ε��� ������ �ƴ���.. ( ó���� �߱��������� ������ �ѵ�.. )

	// �ε�ȭ��
public :
	//	User Data
	INT32			m_lDummyData;						// ���̵����� ( ���µ� ���� )

	// UserData
public :
	AgcdUIUserData*	m_pstUserDataDummy;
	AgcdUIUserData*	m_pstUserDataOldCharID;
	AgcdUIUserData*	m_pstCompen;

	INT32			m_lCompenData;

	// ĳ���� ����
public :
	INT32			m_lUnion;							// �ʱ⿡�� AUUNION_TYPE_NONE, ������ ���� �޴��� NONE�� ��� Setting�� ���� �Էµȴ� 
	INT32			m_lLoginMainSelectCID;				// My Character Select�� ������ Character�� CID
	INT32			m_lAddMyCharacterCount;				// My Character�� AddCharacter�� �� 

	BOOL			m_bHumanBaseCharacterAddRequest;	// Humna Base Character�� ��û�ߴ°�?
	BOOL			m_bOrcBaseCharacterAddRequest;		// Orc Base Character�� ��û�ߴ°�?

	AgpdCharacter*	m_pcsPreSelectedCharacter;			// ������ �����ߴ� �ɸ���
	INT32			m_lOldCharID;						// �̸������ ����ϴ� ���� �̸�

	BOOL			m_bRenameBlock;						// �̸������� ����� ���ΰ�

	TCHAR			m_szMyCharacterInfo[AGCMUILOGIN_MY_CHARACTER_NUM][AGCMUILOGIN_MY_CHARACTER_INFO]	;	// DB�� ��ϵ� ���� Character Name 
	BOOL			m_bMyCharacterFlag[AGCMUILOGIN_MY_CHARACTER_NUM]									;	// DB�� ��ϵ� ���� Character�� Index Info Flag

	TCHAR			m_szItemDetails[100 + 1];
	INT32			m_lCharacterIndex;

	AcUIToolTip						m_CompenTooltip;
	AgcdLoginCompenMaster			m_AgcdLoginCompenMaster;

	AgcmUILoginSelect				m_LoginSelect;

public:
	AgcmUILogin();
	~AgcmUILogin();

	AcUIEdit*	m_pIDControl;
	AcUIEdit*	m_pPWControl;

	//	ApModule inherited
	virtual BOOL	OnAddModule();
	virtual	BOOL	OnInit();
	virtual BOOL	OnIdle(UINT32 ulClockCount);

	//	AddModule helper
	BOOL			AddEvent();
	BOOL			AddFunction();
	BOOL			AddUserData();
	BOOL			AddDisplay();

public :
	void			InitAgcmUILogin();

	BOOL			LoadTemplates(TCHAR *pszPath);	
	void			SetLoginMode(INT32 lLoginMode);
	INT32			GetLoginMode();

	INT32			LoadLoginMap(UINT32 ulDivision);	// �α������ε� ( Select , Create ��� ���ÿ� �ε�.. )
	INT32			LoadSelectMap();					// ������ ���� �ε�
	INT32			LoadCreateMap();					// �ɸ��� �������� �ε�..

	BOOL			SetLoginProcess();
	BOOL			StartLoginProcess();
	void			OpenMoveToHangame();
	void			CharacterClick(INT32 lCID);
	void			CharacterDoubleClick(INT32 lCID);
	BOOL			SelectCharacterAtLoginMain(INT32 lCID);	// Character�� �����ߴ�

	void			CreateCharacter( AgpdCharacter* pcsCharacter, INT32 lID, INT32 lRace, INT32 lClass );

	//	My Character
	BOOL			AddMyCharacterInfo(TCHAR *pszName, INT32 lIndex);
	BOOL			RemoveMyCharacterInfo(TCHAR *pszName);
	BOOL			UpdateSlotCharInfo();
	INT32			GetMyCharacterCount();						// CBGetMyCharacterInfo �� ���ؼ� ��ϵ� Character Coutn�� Return �Ѵ� 
	INT32			GetMyCharacterInfoIndex(TCHAR* pszName);
	INT32			GetMyCharacterInfoIndex(INT32 lID);
	void			MyCharacterAllUpdatePosition();	// ��� MyCharacter Update Position
	void			DeleteAllMyCharacter();	// ��� My Character�� Delete �Ѵ� 
	void			RemoveSelectedMyCharacter();
	BOOL			UpdateCompenInfo();

	BOOL			OpenLoadingWindow(INT32 lRace);
	void			CloseLoadingWindow();
	BOOL			OnLoginResult(INT32 lResult, CHAR *psz);
	INT32			CalcMurdererLevelByPoint( INT32 nPoint );

	void			SetSimpleUI( BOOL bSimple = TRUE )	{	m_bLoadSImpleUI = bSimple;	}

	// SelectAnimtion
	INT32			GetSelectAnimationType( void* ppdCharacter, void* pcdCharacter, void* pFactorModule, void* pItemModule, BOOL bIsReturnToBack = FALSE )
	{
		return m_ControlCharacter.GetSelectAnimationType( ppdCharacter, pcdCharacter, pFactorModule, pItemModule, bIsReturnToBack );
	}

	void			PreLoadAllCharacterTypes( void );
	VOID			ConnectLoginServer		( VOID );		//	Login Server ����
	virtual BOOL	ReConnectLoginServer	( VOID );		//	Login Server ���� �ٽ� �õ�

public:
	//	character select(login main window)
	static BOOL		CBLoginMainSelectButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBLoginMainDeleteButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCharInfoClick01(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCharInfoClick02(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCharInfoClick03(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCompensationClick01(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCompensationClick02(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCompensationClick03(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	BOOL			OnCompensationClick(INT32 lIndex);
	static BOOL		CBCompensationSetFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCompensationKillFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//	character create
	static BOOL		CBLoginMainCreateButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBLoginMainCancelButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBUnionSelectWindowReturnIDPassworldInput(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBConfirmPassword(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBConfirmDeleteChar(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCancelDeleteConfirm(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBCharRenameOkButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCharRenameCancelButtonClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//	in game
	static BOOL		CBOpenExitWindow( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBReturnGame(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBReturnLoginCharacterSelectMode(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//	Event(dialog) Callbacks
	static BOOL	    CBLoginReturnRemoveCharacter(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBLoginReturnRemoveDuplicateAccount(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBLoginReturnDisconnect(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBReturnNotTester(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);	
	static BOOL		CBResultNotApplied(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBIDFailed(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBPasswordFailed(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBNotIsProtected(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);
	static BOOL		CBOpenHangameWeb(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage);

public:
	//	enter game
	static BOOL		CBEncryptCode(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBInvalidClientVersion(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGetUnionInfo(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGetMyCharacterName(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGetMyCharacterNameFinish(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGetMyCharacterInfoFinish(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEnterGameEnd(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBCompensationInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	//	create/remove
	static BOOL		CBGetNewCharacterName(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGetNewCharacterInfoFinish(PVOID pvData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRemoveCharacter(PVOID pvData, PVOID pClass, PVOID pCustData);

	//	common login result
	static BOOL		CBLoginResult(PVOID pvData, PVOID pClass, PVOID pCustData);

	//	map
	static BOOL		CBMapSectorChangePreCallback(PVOID pData, PVOID pClass, PVOID pCustData);	// Map���� Sector Change�Ǳ� ���� �´�.
	static BOOL		CBMapSectorChangeCallback(PVOID pData, PVOID pClass, PVOID pCustData);		// Map���� Sector Change�ɶ� �´�.
	static BOOL		CBMapLoadSector(PVOID pData, PVOID pClass, PVOID pCustData);				// AgcmMap���� Sector�� Load�Ǹ� Callback�� �´�.
	//	character
	static BOOL		CBAddCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGameServerError(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBResultDisconnectByAnotherUser(PVOID pData, PVOID pClass, PVOID pCustData);	
	static BOOL		CBDisconnectGameServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBLoadingComplete(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	//	return to login
	static BOOL		CBReturnToLoginEndProcess(PVOID pData, PVOID pClass, PVOID pCustData);
	//	world
	static BOOL		CBGetWorldAll(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBUpdateWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBGetCharCount(PVOID pData, PVOID pClass, PVOID pCustData);
	//	main UI
	static BOOL		CBKeydownCloseAllUI( PVOID pData, PVOID pClass, PVOID pCustData	);

	// AgpmAdmin
	static BOOL		CBPing(PVOID pData, PVOID pClass, PVOID pCustData);

	//	Display Callbacks
	static BOOL		CBDisplaySlotCharName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplaySlotCharLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplaySlotCharExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplaySlotCharGuild(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplaySlotCharVillian(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplayGameServerName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplayOldCharID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplayConnectRename(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBDisplayCompenCaption(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);

public :
	static BOOL		ParsePatchCodeString( char* pString );
};
