// AgcEngine.h: INT32erface for the AgcEngine class.
// Ŭ���̾�Ʈ ���� Ŭ����.
// �ۼ� : �����
// ���� : 2002/04/11
// -= Update Log =-
// ������ (2002-08-20 ���� 4:56:23) : ���̺귯�� �������� ����.. ��ӹ޾Ƽ� �޽��� �������̵� �ϴ� ����� ����Ѵ�.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCENGINE_H__8BCC5208_F2F4_4E6C_B3DE_B52CB449BB5C__INCLUDED_)
#define AFX_AGCENGINE_H__8BCC5208_F2F4_4E6C_B3DE_B52CB449BB5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

#include <ApModule/ApModuleManager.h>
#include <AuList.h>
#include <AgcModule/AcUtils.h>
#include <ApBase/MagDebug.h>
#include <AgcSkeleton/skeleton.h>
#include <AgcSkeleton/camera.h>
#include <AgcSkeleton/menu.h>
#include <AcClientSocket/AcSocketManager.h>
#include <AuCrypt/AuCryptManager.h>
#include <AgcSkeleton/AcDefine.h>
#include <AcuTexture/AcuTexture.h>
#include <queue>
#include <AuLua/AuLua.h>
#include <AgcModule/AgcWindow.h>

class AgcModule;
enum CAMERASTATECHANGETYPE;

#define	LUA_RETURN( result )	{ pLua->PushNumber( ( double ) result ); return 1; }

#define	MAX_CLIENT_MODULE			10

#define DEFAULT_SCREEN_WIDTH		(1024)
#define DEFAULT_SCREEN_HEIGHT		(768)
#define DEFAULT_ASPECTRATIO_WIDE	(16.0f/9.0f)
#define DEFAULT_ASPECTRATIO_NORMAL	(4.0f/3.0f)
#define DEFAULT_VIEWWINDOW			(0.7f)


typedef BOOL	(*AgcEngineDefaultCallBack) (PVOID pData, PVOID pClass, PVOID pCustData);		// AgcEngine Default Callback Function Type 

struct Im2DScreenData			// font�� ���� �ʿ��� ������ ������ ����(DrawIm2DInWorld2�� ����)
{
	RwV3d	Screen_Pos;				// x,y ȭ�� ��ǥ z -  screenz
	float	Recip_Z;
};

class AgcEngine : public ApModuleManager
{
protected:
	ApMutualEx			m_csMutexRender;	// Render Lock
	ApCriticalSection	m_csMutexFrame;		// Frame Lock
	ApCriticalSection	m_csMutexTexture;	// Texture Lock
	ApCriticalSection	m_csMutexResArena;	// Resource Arena Lock
	ApCriticalSection	m_csMutexClump;		// Clump Lock

	BOOL				m_bReceivedMouseMessage;	// ���콺 �Է� ó��������..
	
	HWND				m_hWnd;				// ������ �ڵ�..

	class AgcmImGui *	m_pcsAgcmImGui;

public:
	// RenderWare Datas
	enum ACTIVATION_FLAG
	{
		AGCENGINE_NORMAL	,
		AGCENGINE_SUSPENDED	,
		AGCENGINE_MINOR		
	};

	INT32		m_nActivated		;	// ��Ƽ���̼� �÷��׷� �̿�..
	RwBool		m_FPSOn				;	// FPS ǥ�� On/off			--> AgcmRender�� �̵�

	RwInt32		m_nFrameCounter		;
	RwInt32		m_nFramesPerSecond	;

	RwRGBA		m_rgbForegroundColor;
	RwRGBA		m_rgbBackgroundColor;

	RpWorld		*m_pWorld			;
	RwCamera	*m_pCamera			;

	RwReal		m_fNearClipPlane	;
	RwReal		m_fFarClipPlane		; // ����� ��ġ.

	BOOL		m_bFogEnable		;

	RpLight		*m_pLightAmbient	;	// ����
	RpLight		*m_pLightDirect		;

	AcuCamera	*m_clCamera			;
	BOOL		m_bWideScreenMode	;	// ���̵� ���

	AgcWindow	*m_pCurrentFullUIModule		;	// ���� ����� Full UI Module

	
	AcSocketManager m_SocketManager;

	AgcWindow	*m_pFocusedWindow	;	// Focus�� ������ �ִ� Window

	BOOL		m_bRenderWorld			;
	BOOL		m_bRenderWindow			;

	INT32		m_lIdleModal			;

	AuList<AgcWindowNode *>	m_listWindowToDelete;

	BOOL			m_bModalMessage		;

	
	CHAR			m_szLanguage[128];		// �Ϻο��� �����.
	RsMouseStatus	m_stLastMouseMoveState; // �ֱٿ� ���콺 �̵� ���� ����..

protected:
	// Alef Datas
	AgcModule*	m_apFullUIModule[ MAX_CLIENT_MODULE ];
	AgcModule*	m_apWindowUIModule[ MAX_CLIENT_MODULE ];
	AgcModule*	m_apNetworkModule[ MAX_CLIENT_MODULE ];
	AgcModule*	m_apDataModule[ MAX_CLIENT_MODULE ];

	INT16		m_nFullUIModuleCount;
	INT16		m_nWindowUIModuleCount;
	INT16		m_nNetworkModuleCount;
	INT16		m_nDataModuleCount;

	// �� ����Ÿ �ɹ����� ������ ü�������� �����ϱ� ����
	// �������� �з��Ͽ� �����ͷ� ������ �ֱ� ���ؼ��̴�. �� ����Ʈ��
	// ��� ���������Ҷ� �ڵ����� ���̰� �Ÿ� , ����� ���۵ɶ�
	// Ÿ���� �����ؾ��Ѵ�.
	
	bool		m_bSuspendedFlag		; // ī�޶� �������н� �������� �÷��� ����.
	UINT		m_uCurrentTick			; // ���� ƽ.
	UINT		m_uPastTick				; // �������ӿ��� �� ������ ������ ƽ.

	bool		m_bNeedWindowListUpdate	;

	INT32		m_lOneTimePopCount;
	
	BOOL		m_bIMEComposingState	;
	BOOL		m_bCharCheckState		;

	void		WindowListUpdate()		;

	CHAR*		m_szImagePath;

	// Set Cursor Mouse Position �� ���� Member
	AgcEngineDefaultCallBack	m_pSetCursorPositionCallback;
	PVOID						m_pSetCursorPositionCallbackClass;

	UINT32		m_uDebugFlag;
	FLOAT		m_fCameraProjection;

	INT32		m_lMaxPacketBufferSize;

	UINT32		m_ulPrevProcessClock;
	BOOL		m_bImmediateIdleTerminateFlag;

	static BOOL	m_bLCtrl;
	static BOOL	m_bRCtrl;
	static BOOL	m_bLAlt;
	static BOOL	m_bRAlt;
	static BOOL	m_bLShift;
	static BOOL	m_bRShift;
	static BOOL m_bMouseLeftBtn;
	static BOOL m_bMouseRightBtn;
	static BOOL m_bMouseMiddleBtn;

public:
	enum	DEBUGFLAG
	{
		SHOW_GRID				= 0x01,
		SHOW_GEOMETRY_COLLISION	= 0x02,
		SHOW_GEOMETRY_BLOCKING	= 0x04,
		SLEEP_MAIN_THREAD		= 0x08,
		SLEEP_BACK_THREAD		= 0x10,
		CAMERA_ANGLE_CHANGE		= 0x20,	// Ctrl + Wheel ������ ī�޶� ȭ�� ����.
		AUTO_HUNTING			= 0x40,
		OBJECT_LOADING_ERROR	= 0x80,	// ������Ʈ �ε��� �����ϸ�..
	};

	AgcEngine();
	virtual ~AgcEngine();

	HWND	GethWnd()				{	return m_hWnd;				}
	BOOL	ShowFps( BOOL bShow	)	{	return m_FPSOn = bShow;		}
	BOOL	GetFpsMode()			{	return m_FPSOn; }
	RwInt32	GetFps()				{	return m_nFramesPerSecond;	}

	UINT32	SetDebugFlag( UINT32 uFlag )	{	return m_uDebugFlag = uFlag; }
	UINT32	GetDebugFlag()					{	return m_uDebugFlag;		}

	// �������� �����Լ�.
	BOOL	SetProjection( FLOAT fRate ); // 0.2~ 2.0 ���� ���� �����ϴ�.
	BOOL	SetProjection()			{	return SetProjection( m_fCameraProjection );	}
	FLOAT	GetProjection()			{	return m_fCameraProjection;	}
	BOOL	SetWideScreen( BOOL bWide = TRUE );
	BOOL	GetWideScreen()			{	return m_bWideScreenMode;	}

	BOOL	GetIMEComposingState()	{	return m_bIMEComposingState;	}

	void	SetCharCheckOut()		{	m_bCharCheckState = TRUE;	}
	void	SetCharCheckIn()		{	m_bCharCheckState = FALSE;	}
	BOOL	GetCharCheckState()		{	return m_bCharCheckState;	}

	// ������ ���� Operations!
	INT32	SetActivate( INT32 nActivated );
	INT32	GetActivate( void );

	void	SetImmediateIdleTerminateFlag()	{	m_bImmediateIdleTerminateFlag = TRUE;	}
	BOOL	GetImmediateIdleTerminateFlag()	{
		BOOL	bRet = m_bImmediateIdleTerminateFlag;
		m_bImmediateIdleTerminateFlag = FALSE;
		return bRet;
	}

	BOOL	AddChild( AgcWindow* pModule, INT32 nid = 0	);
	BOOL	CloseWindowUIModule( AgcWindow* pModule ); // ž������ üũ��.
	BOOL	SetFullUIModule( AgcWindow* pModule	);
	
	// Idle Time�� �Լ���
	UINT	GetTickCount()			{	return m_uCurrentTick;	}
	UINT	GetPastTime()			{	return m_uPastTick;		}
	UINT	SetTickCount( UINT tick );

	INT32	GetOneTimePopCount()	{	return m_lOneTimePopCount;	}
	VOID	SetOneTimePopCount(INT32 lOneTimePopCount)	{	m_lOneTimePopCount = lOneTimePopCount;	}

	// Initialize Operations
	BOOL		RegisterModule		( AgcModule * pModule	);	// ����� �����.
	RwCamera	*CreateCamera		( RpWorld *world		);
	RpWorld		*CreateWorld		();	// Empty World ����.
	void		CreateLights		(void);
	void		DestroyLights		(void);

	AgcWindow *	WindowFromPoint		( INT32 x , INT32 y, BOOL bCheckUseInput = TRUE );

	// Focussing!
	BOOL		SetFocus( AgcWindow * pWindow	);
	BOOL		SetCapture( AgcWindow * pWindow	);
	BOOL		ReleaseCapture();
	
	// Client Socket �ִ� ���� ���� ����
	BOOL		SetMaxConnection( INT32 lMaxConnection	, INT32 lMaxPacketBufferSize	);
	
	// ���� ���� ����
	INT16	connect( CHAR* pszServerAddr , INT16 nServerType	, PVOID pClass, ApModuleDefaultCallBack fpConnect, ApModuleDefaultCallBack fpDisconnect, ApModuleDefaultCallBack fpError);
	BOOL	disconnect( INT16 nNID );

	// Render Lock/Unlock (Lock �ϸ� OnRender�� �ȺҸ���.)
	inline VOID		LockRender	()		{		}
	inline VOID		UnlockRender()		{		}

	// Render Lock/Unlock (Lock �ϰ� RwFrame �����ؾ��Ѵ�.)
	inline	VOID	LockFrame()			{		}
	inline	VOID	UnlockFrame()		{		}

	// 2004.11.20	gemani
	static	void 	RwLockFrame();
	static	void 	RwUnlockFrame();

	static	void	RwLockTexture();
	static	void	RwUnlockTexture();

	static	void	RwLockGeometry();
	static	void	RwUnlockGeometry();

	static	void*	RwAllocFreeListCB();
	static	void	RwFreeFreeListCB(void*	pCriticalSection);
	static	void	RwLockFreeList(void*	pCriticalSection);
	static	void	RwUnlockFreeList(void*	pCriticalSection);

	static	void	RwLockResArena();
	static	void	RwUnlockResArena();

	static	void	RwLockClump();
	static	void	RwUnlockClump();

	static	void*	RwResEntryCreateCB();
	static	void	RwResEntryFreeCB(void*	pCriticalSection);
	static	void	RwLockResEntry(void*	pCriticalSection);
	static	void	RwUnlockResEntry(void*	pCriticalSection);

	// Message
	virtual BOOL		OnInit()				{	return TRUE;	}
	virtual	BOOL		OnAttachPlugins()		{	return TRUE;	}
	virtual	BOOL		OnRegisterModulePreCreateWorld()	{	return TRUE;	}
	virtual	RpWorld *	OnCreateWorld(); // ���带 �����ؼ� �������ش�.

	virtual	BOOL		OnPreRegisterModule()	{	return TRUE;	}
	virtual	BOOL		OnRegisterModule()		{	return TRUE;	}
	virtual BOOL		OnPostRegisterModule()	{	return TRUE;	}
	virtual	BOOL		OnPostInitialize()		{	return TRUE;	}

	virtual	RwCamera *	OnCreateCamera( RpWorld * pWorld );	// ���� ī�޶� Create���ش�
	virtual BOOL		OnCameraResize( RwRect * pRect );
	virtual BOOL		OnDestroyCamera();
	
	virtual	BOOL		OnCreateLight();	// ������ �����ؼ�. �� ������ �������ش�. m_pLightAmbient , m_pLightDirect
	virtual	void		OnDestroyLight();
	virtual	AgcWindow *	OnSetMainWindow()		{	return NULL;	}

	// ��Ŷ ����
	virtual BOOL		SendPacket(PVOID pvPacket, INT16 nLength, UINT32 nFlag = ACDP_SEND_GAMESERVERS, PACKET_PRIORITY ePriority = PACKET_PRIORITY_4, INT16 ulNID = 0);

	static BOOL			SendPacket( PACKET_HEADER& pPacket);

	// �������� ó���ϴ� �޽�����.
	// ������ ū�� ������ �������̵� ���� �ʴ´�.
	virtual	BOOL			OnInitialize3D( PVOID param ); // �׷��� ��� �ʱ�ȭ
	virtual	BOOL			OnAttachInputDevices();	// �Է±� ������ ���⼭
	virtual	BOOL			OnIdle(); // �������� ��Ʈ���� ���� idleŸ��ó��
	virtual BOOL			OnRenderPreCameraUpdate()	{	return TRUE;	}
	virtual void			OnRender();	// ������ ���� 
	virtual	void			OnWorldRender()				{					}
	virtual	BOOL			OnInitialize();	//���� ��ü , �׷��� ������ �ʱ�ȭ
	virtual	void			OnTerminate();	// ���α׷� ��������
	virtual BOOL			OnSelectDevice(); // ���÷��� ����̽��� �ػ󵵸� �����ϴ°�..


	// near,far,viewwindow���� �� ī�޶� ������ ����Ǿ��� ���
	// 0 - View Window size�����
	// 1 - Near�����
	// 2 - Far�����
	virtual void			OnCameraStateChange		(CAMERASTATECHANGETYPE ChangeType);	
	
	virtual RsEventStatus	OnMessageSink			( RsEvent event , PVOID param ); // �޽����� ó���Ͽ� ���鿡�� �Ѱ���
	virtual UINT32			OnWindowEventHandler	(HWND window, UINT message, WPARAM wParam, LPARAM lParam );

	static RsEventStatus	KeyboardHandler	( RsEvent event , PVOID param ); // Ű���� �޽��� ����
	static RsEventStatus	MouseHandler	( RsEvent event , PVOID param ); // ���콺 �޽��� ����;

	virtual BOOL SetSelfCID		( INT32 lCID			);	// SelfCID�� DP Module�� ���.

	virtual VOID OnEndIdle		(						){return;}	// Idle Tick �� �ѹ� ������ �Ҹ���. 

	// Camera���� �Լ�
	void				SetWorldCameraNearClipPlane	(float	fNear);
	void				SetWorldCameraFarClipPlane	(float	fFar);
	void				SetWorldCameraViewWindow	(int w,int h);

	// Cursor Mouse Position Set Call back funciton
	void				SetCallbackSetCursorMousePosition( AgcEngineDefaultCallBack pfCallback, PVOID pClass );

	VOID				SetRenderMode	( BOOL bRenderWorld = TRUE	, BOOL bRenderWindow = TRUE	)
	{
		m_bRenderWorld	= bRenderWorld	;
		m_bRenderWindow	= bRenderWindow	;
	}

protected:
	void	DisplayOnScreenInfo		();	// Display Frame Rate
	void	ProcessDeleteWindow		();

public:
	// Modal Windowó��.. ���� ȣ�����ָ� ȥ����.
	AuList< AgcModalWindow * >	m_listModalStack;

	// ����Ʈ�� �����ϴ� �Լ��̴�.. ����!..
	BOOL	OpenModalWindow		( AgcModalWindow * pWindow );
	BOOL	CloseModalWindow	( AgcModalWindow * pWindow );
	BOOL	IsModalWindow		( AgcModalWindow * pWindow );
	AgcModalWindow *	GetTopModalWindow();

	// Seong Yon - jun
	//AgcWindow		*m_pMonopolyUI			;	// Monopoly Window ( ���� �޼����� �����Ѵ� ) �� Pointer
	vector< AgcWindow * >	m_vecMonopolyUI	;
	BOOL			m_bDraging				;	// � �����찡 Draging ����
	AgcWindow		*m_pMouseOverUI			;	// Focused Window�� Pointer - ���ӳ������� ���� ���콺�� ���� ���� �����찡 Focus�� 
	AgcModalWindow	*m_pWaitingDialog		;	// Waiting Dialog
	AgcModalWindow	*m_pMessageDialog		;	// Message Dialog
	AgcModalWindow	*m_pMessageDialog2		;	// Message Dialog
	AgcModalWindow	*m_pOkCancelDialog		;	// OK Cancel Dialog
	AgcModalWindow	*m_pEditOkDialog		;	// Edit + OK Dialog
#ifdef _AREA_GLOBAL_
	AgcModalWindow	*m_pThreeBtnDialog		;	// Three Button Dialog
#endif
	AgcWindow		*m_pActiveEdit			;	// ���� Active�� Edit Control

	void	SetSoundAlertMSGFile( char* pszFileName );	// Alert MSG���� �̸��� Setting�Ѵ�
	CHAR	m_szSoundAlertMSG[256]			;			// Alert MSG Sound ���� �̸� - �̰����� Ư���� Path���� ���� �־��ش� 

	AgcWindow*	GetMonopolyWindow()
	{
		if( m_vecMonopolyUI.size() )
		{
			vector< AgcWindow * >::reverse_iterator	rIter = m_vecMonopolyUI.rbegin();
			AgcWindow * pMonopolyUI	= *rIter;		
			return pMonopolyUI;
		}
		else return NULL;
	}

	BOOL	PushMonopolyWindow( AgcWindow* pWindow )	;	// Monopoly Window�� �����Ѵ�
	BOOL	PopMonopolyWindow	( AgcWindow* pWindow )	;	// Monopoly Window���� �����Ѵ� 

	INT32	WaitingDialog( AgcWindow* pWindow, char* pszmessage )		;	// Waiting Dialog�� ����ش�
	INT32	WaitingDialogEnd()											;	// Waiting Dialog�� �ݴ´�.
	INT32	MessageDialog( AgcWindow* pWindow, char* pszmessage )		;	// Message Dialog�� ����ش�
	INT32	MessageDialog2( AgcmUIManager2* pWindow, char* pszmessage)		;	// No Blocking Messsage Dialog
	INT32	EditMessageDialog( AgcWindow* pWindow, char* pszmessage, CHAR* pEditString)	;	// Messsage Dialog width EditBox
	INT32	EditMessageDialog2( AgcWindow* pWindow, char* pszmessage, CHAR* pEditString)	;	// Messsage Dialog width EditBox
	INT32	OkCancelDialog( AgcWindow* pWindow, char* pszmessage )		;	// Ok Cancel Dialog�� ����ش�
	INT32	OkCancelDialog2( AgcWindow* pWindow, char* pszmessage )		;	// Ok Cancel Dialog�� ����ش�
#ifdef _AREA_GLOBAL_
	INT32	ThreeBtnDialog( AgcWindow* pWindow, char* pszmessage );		// ��ư 3��¥�� ���̾�α׸� ����ش�.
#endif
	BOOL	SetMeActiveEdit( AgcWindow*	pEdit	)						;	// �� Edit�� Active Edit�� �ȴ�.
	BOOL	ReleaseMeActiveEdit( AgcWindow* pEdit )						;	// �� Edit�� Deactive�Ѵ�.

	AgcWindow*	GetFocusedWindow	()	{ return m_pFocusedWindow; }	// ���� Focus�� ���� Window�� �����´�.

	My2DVertex		m_vBoxFan[4];
	RwReal			m_fRecipZ;

	BOOL			m_bDrawFPS;
	BOOL			m_bSaveScreen;

	IDirect3DDevice9*	m_pCurD3D9Device;
	bool		GetVideoMemoryState( DWORD * pTotal , DWORD * pFree );

	void		ScreenShotSave();
	void		ScreenShotSaveGDI();
	int			SaveToBmp(HANDLE file,LPDIRECT3DSURFACE9 pSurf);

	void		PrintRenderState();

	BOOL		DrawRwTexture( RwTexture* pTexture, INT32 nX, INT32 nY, DWORD dwColor = 0xffffffff/*ARGB*/ );
	BOOL		Draw2DPanel( INT32 nX, INT32 nY, INT32 nWidth, INT32 nHeight, DWORD dwColor, LPDIRECT3DTEXTURE9 pTex = NULL );
	bool		DrawIm2D( RwTexture* pTex, 
							float x, float y, float w, float h,
							float st_u = 0.f, float st_v = 0.f, float off_u = 1.f, float off_v = 1.f,
							DWORD color = 0xffffffff, UINT8 alpha = 255 );
	BOOL		DrawIm2DPixel( RwTexture* pTexture,
							INT32 nX, INT32 nY, INT32 nWidth = -1, INT32 nHeight = -1,
							INT32 nUStart = 0, INT32 nVStart = 0, INT32 nUEnd = -1, INT32 nVEnd	= -1,
							DWORD uColor = 0xffffffff, UINT8 uAlpha = 255 );
	BOOL		DrawIm2DPixelMask( RwTexture* pTexture, RwTexture* pMask , 
							INT32 nX, INT32 nY, INT32 nWidth = -1, INT32 nHeight = -1,
							INT32 nUStart = 0, INT32 nVStart = 0, INT32 nUEnd = -1, INT32 nVEnd	= -1,
							DWORD uColor = 0xffffffff, UINT8 uAlpha = 255 );
	BOOL		DrawIm2DPixelRaster( RwRaster*	pRaster , RwTexture* pTexture ,
							INT32 nX, INT32 nY, INT32 nWidth = -1, INT32 nHeight = -1,
							INT32 nUStart = 0, INT32 nVStart = 0, INT32 nUEnd = -1, INT32 nVEnd	= -1,
							DWORD uColor = 0xffffffff, UINT8 uAlpha = 255 );
	bool		DrawIm2DRotate( RwTexture* pTex,
								float x, float y, float w, float h,
								float st_u = 0.f, float st_v = 0.f, float off_u = 1.f, float off_v = 1.f,
								float fCenterX = 0.f, float fCenterY = 0.f, float fDegree = 0.f,
								DWORD color = 0xffffffff, UINT8 alpha = 255 );

	// In World Space Render
	// offset�� ���� ��ǥ�� ��ȯ�� ȭ�� ��ǥ������ ����� �̵�ġ�̴�. (ex. offset.x = -10 �̶�� ȭ�鿡 ������ x - 10 ���� ��´�
	bool		DrawIm2DInWorld(RwTexture* pTex,RwV3d* pWPos,RwV2d* offset,float w,float h,
								float st_u = 0.0f, float st_v = 0.0f, float off_u = 1.0f, float off_v = 1.0f,
								DWORD color=0xffffffff,UINT8	alpha = 255);

	bool		DrawIm2DInWorld(RwTexture* pTex, RwV3d* pCamPos, RwV3d* pScreenPos, float recipZ, RwV2d* offset, float w,float h,
								float st_u = 0.0f, float st_v = 0.0f, float off_u = 1.0f, float off_v = 1.0f,
								DWORD color=0xffffffff,UINT8	alpha = 255);

	// Im2DScreenData �� agcmfont DrawTextIMMode2DWorld�� ������ ���ڵ��� ������ �ִ�
	bool		DrawIm2DInWorld2(Im2DScreenData* pData,RwTexture* pTex,RwV3d* pWPos,RwV2d* offset,float w,float h,
								float st_u = 0.0f, float st_v = 0.0f, float off_u = 1.0f, float off_v = 1.0f,
								DWORD color=0xffffffff,UINT8	alpha = 255);
		
	void		GetWorldPosToScreenPos		( RwV3d * pWorldPos ,RwV3d* pCameraPos, RwV3d* pScreenPos , float* recip_z);	

	// Message Dialog
	void	SetWaitingDialog( AgcModalWindow* pWindow )		{	m_pWaitingDialog = pWindow;		}
	void	SetMessageDialog( AgcModalWindow* pWindow )		{	m_pMessageDialog = pWindow;		}
	void	SetMessageDialog2( AgcModalWindow* pWindow )	{	m_pMessageDialog2	= pWindow;	}
	void	SetOkCancelDialog( AgcModalWindow* pWindow )	{	m_pOkCancelDialog = pWindow;	}
	void	SetEditOkDialog( AgcModalWindow* pWindow )		{	m_pEditOkDialog = pWindow;	}
#ifdef _AREA_GLOBAL_	
	void	SetThreeBtnDialog( AgcModalWindow* pWindow )		{	m_pThreeBtnDialog = pWindow;	}
#endif

	VOID	SetGameServerID(INT32 lNID)						{	m_SocketManager.SetGameServerID(lNID);	}
	VOID	SetMaxPacketBufferSize(INT32 lMaxPacketBufferSize)	{	m_lMaxPacketBufferSize = lMaxPacketBufferSize;	}

	static inline	BOOL	IsLCtrlDown()	{ return m_bLCtrl;	}
	static inline	BOOL	IsRCtrlDown()	{ return m_bRCtrl;	}
	static inline	BOOL	IsCtrlDown()	{ return m_bLCtrl || m_bRCtrl;	}

	static inline	BOOL	IsLAltDown()	{ return m_bLAlt;	}
	static inline	BOOL	IsRAltDown()	{ return m_bRAlt;	}
	static inline	BOOL	IsAltDown()		{ return m_bLAlt || m_bRAlt;	}

	static inline	BOOL	IsLShiftDown()	{ return m_bLShift;	}
	static inline	BOOL	IsRShiftDown()	{ return m_bRShift;	}
	static inline	BOOL	IsShiftDown()	{ return m_bLShift || m_bRShift;	}

	static inline BOOL		IsMouseLeftBtnDown()		{ return m_bMouseLeftBtn; }
	static inline BOOL		IsMouseRightBtnDown()		{ return m_bMouseRightBtn; }
	static inline BOOL		IsMouseMiddleBtnDown()		{ return m_bMouseMiddleBtn; }

public:
	// ������ Lua ó��
	struct	stLuaWaitInfo
	{
		enum TYPE
		{
			NONE		,
			WAITTIME	,	// ���� �ð��� ��ٸ���.
			WAITFORKEY	,	// Ű�Է��� ������ Ʈ���� �´�.
			WAITTIMEKEY		// Ű�Է��� ������ Ʈ���� �Ű� , ���� �ð��� ������ Ʈ���� �ȴ�.
		};

		TYPE	eType;
		UINT32	uTime;

		stLuaWaitInfo():eType( NONE ),uTime(0){}

		BOOL	IsWaitTime( UINT32 uCurrentTick )
		{
			switch( eType )
			{
			case NONE:
				return FALSE;
			case WAITTIME:
				if( uCurrentTick < uTime )	return TRUE	;
				else						return FALSE;
			default:
				return FALSE;
			}
		}
	};
	
	std::queue< std::string >	m_queueLuaCommand	;
	stLuaWaitInfo				m_luaWaitInfo		;

	void	LuaInitialize();

	BOOL	Lua_RunScript( std::string strFilename , BOOL bEncrypted );
	BOOL	Lua_IsRunning();

	BOOL	Lua_EnQueue( std::string str = "" );
	BOOL	Lua_PushWait( UINT32 uWaitTime );
	void	Lua_EndWait() { m_luaWaitInfo.eType = stLuaWaitInfo::NONE; }
	void	Lua_Flush_Queue();

	void	ProcessIdleLuaCommand();
	static	void	LuaErrorHandler( const char * pStr );

	virtual	void	LuaErrorMessage( const char * pStr ){}

	BOOL	Debug_Idle(UINT32 ulClockCount);
	BOOL	Debug_Idle2(UINT32 ulClockCount);

	bool	IsCheckSlowIdle() { return m_uSlowIdleIndicator ? true : false; }
	UINT32	m_uSlowIdleIndicator;	// �� ƽ ���� �������
			// ���� ����� ���̵� ( �α� ��� ���� ) �� ������ �Ǹ�
			// �� ������ ū ƽ�� �߻��Ұ��
			// SlowIdle.txt �� �α׷� ���´�.
};

// ���� �ν��Ͻ��� �ϳ��� �����ؾ��Ѵ�.
// ó�� �����Ŵ� �ν��Ͻ� �����͸� �����Ͽ� ���� �ݹ鿡 ����Ѵ�.
// ��� ������ �����ϸ� ���´�. - _-+
extern AgcEngine *	g_pEngine;

// ������..
RsEventStatus AppEventHandler(RsEvent event, PVOID param);

#endif // !defined(AFX_AGCENGINE_H__8BCC5208_F2F4_4E6C_B3DE_B52CB449BB5C__INCLUDED_)
