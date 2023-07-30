#include <rwcore.h>
#include <rpworld.h>
#ifdef RWLOGO
#include <rplogo.h>
#endif
#include <rtcharse.h>
#include <AgcSkeleton/skeleton.h>
#include <AgcSkeleton/menu.h>
#include <AgcSkeleton/camera.h>
#include <rtbmp.h>
#include <rtpng.h>
#include <rttiff.h>
#include <d3d9.h>
#include <AgcModule/AgcEngine.h>
#include <ApBase/ApLockManager.h>
#include <time.h>
#include <AgcSkeleton/win.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AcuDeviceCheck/AcuDeviceCheck.h>
#include <ApModule/ApModuleStream.h>
#include <AgcmUIOption/AgcmUIOptionDefine.h>
#include <AgcmMap/bmp.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmImgui/AgcmImGui.h>
#include <AgcmUIControl/AcUIMessageDialog.h>

extern "C"
{
	typedef struct
	{
		RwUInt32 value;
		RwBool changed;
	} RxStateCache;

	RxStateCache	RenderStateCache[210];
	RxStateCache	TextureStageStateCache[8][33];

	extern void    *RwEngineInstance;

	void *	_rpClumpClose(void *instance,RwInt32 offset,RwInt32 size);
}

#define AGCENGINE_DEFAULT_MAX_PACKET_BUFFER_SIZE	1024000


// �۷ι� ���� ����.
AgcEngine*	g_pEngine			= NULL;
BOOL	AgcEngine::m_bLCtrl		= FALSE;
BOOL	AgcEngine::m_bRCtrl		= FALSE;
BOOL	AgcEngine::m_bLAlt		= FALSE;
BOOL	AgcEngine::m_bRAlt		= FALSE;
BOOL	AgcEngine::m_bLShift	= FALSE;
BOOL	AgcEngine::m_bRShift	= FALSE;
BOOL	AgcEngine::m_bMouseLeftBtn		= FALSE;
BOOL	AgcEngine::m_bMouseRightBtn		= FALSE;
BOOL	AgcEngine::m_bMouseMiddleBtn	= FALSE;

extern MSG g_msgLastReceived;

RsEventStatus AppEventHandler(RsEvent event, void *param)
{
	return g_pEngine ? g_pEngine->OnMessageSink( event , param ) : rsEVENTERROR;
}

UINT32 AppWindowEventHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
	return g_pEngine ? g_pEngine->OnWindowEventHandler( window , message , wParam , lParam ) : 0;
}

AgcEngine::AgcEngine():m_uSlowIdleIndicator( 0 )
{
#ifdef _DEBUG
	m_FPSOn					= TRUE;
#else
	m_FPSOn					= FALSE;
#endif

	m_nFrameCounter			= 0;
	m_nFramesPerSecond		= 0;

	RwRGBA ForegroundColor	= {200, 200, 200, 255};
	RwRGBA BackgroundColor	= { 64,  64,  64,   0};

	m_rgbForegroundColor	= ForegroundColor;
	m_rgbBackgroundColor	= BackgroundColor;

	m_pWorld				= NULL;
	m_pCamera				= NULL;

	m_pLightAmbient			= NULL;	// ����
	m_pLightDirect			= NULL;

	m_clCamera				= NULL;

	m_nFullUIModuleCount	= 0;
	m_nWindowUIModuleCount	= 0;
	m_nNetworkModuleCount	= 0;
	m_nDataModuleCount		= 0;

	m_fNearClipPlane		= 0.1f;
	m_fFarClipPlane			= 10000.0f;

	m_pCurrentFullUIModule	= NULL;

	m_bSuspendedFlag		= false;
	
	m_uCurrentTick			= GetTickCount(); // ���� ƽ.
	m_uPastTick				= 0; // �������ӿ��� �� ������ ������ ƽ.

	m_nActivated			= AGCENGINE_NORMAL;

	g_pEngine				= this;
	m_bNeedWindowListUpdate	= false;
	
	m_bIMEComposingState	= FALSE;

	m_bCharCheckState		= FALSE;

	SetOneTimePopCount(100);

	m_csMutexRender.Init();
	m_csMutexFrame.Init();
	m_csMutexTexture.Init();
	m_csMutexResArena.Init();
	m_csMutexClump.Init();

	m_pMouseOverUI			= NULL;
	m_pWaitingDialog		= NULL;
	m_pMessageDialog		= NULL;
	m_pMessageDialog2		= NULL;
	m_pOkCancelDialog		= NULL;
#ifdef _AREA_GLOBAL_
	m_pThreeBtnDialog		= NULL;
#endif
	m_pEditOkDialog			= NULL;
	m_bDraging				= FALSE;
	m_pActiveEdit			= NULL;

	m_szImagePath			= NULL;
	m_pFocusedWindow		= NULL;

	memset( m_szSoundAlertMSG, 0, sizeof(m_szSoundAlertMSG) );
	
	m_pSetCursorPositionCallback		= NULL;
	m_pSetCursorPositionCallbackClass	= NULL;

	m_uDebugFlag			= 0;

	m_bRenderWorld			= TRUE;
	m_bRenderWindow			= TRUE;
	m_bReceivedMouseMessage	= FALSE;

	m_bDrawFPS				= FALSE;
	m_bSaveScreen			= FALSE;

	m_pCurD3D9Device		= NULL;
	m_hWnd					= NULL;
	m_bFogEnable			= FALSE;	

	m_lMaxPacketBufferSize	= AGCENGINE_DEFAULT_MAX_PACKET_BUFFER_SIZE;

	m_bLCtrl				= FALSE;
	m_bRCtrl				= FALSE;
	m_bLAlt					= FALSE;
	m_bRAlt					= FALSE;
	m_bLShift				= FALSE;
	m_bRShift				= FALSE;
	m_bMouseLeftBtn			= FALSE;
	m_bMouseRightBtn		= FALSE;
	m_bMouseMiddleBtn		= FALSE;

	m_ulPrevProcessClock	= 0;

	m_lIdleModal			= 0;

	m_bModalMessage			= FALSE;
	m_bWideScreenMode		= FALSE;
	m_fCameraProjection		= DEFAULT_VIEWWINDOW;

	memset( m_szLanguage, 0, 128 );
	
	::CoInitialize( NULL );
}
AgcEngine::~AgcEngine()
{
	if( m_clCamera )
		delete m_clCamera;
}
BOOL	AgcEngine::RegisterModule( AgcModule* pModule )
{
	ASSERT( !"�������� �ʾҽ��ϴ�" );
	return TRUE;
}

BOOL	AgcEngine::OnInitialize()
{
    m_clCamera	= new AcuCamera;
	
	if( RsInitialize() )
    {
        if( !RsGlobal.maximumWidth )	RsGlobal.maximumWidth = DEFAULT_SCREEN_WIDTH;
        if( !RsGlobal.maximumHeight )	RsGlobal.maximumHeight = DEFAULT_SCREEN_HEIGHT;

		RsGlobal.appName	= RWSTRING("Archlord");
        RsGlobal.maxFPS		= 120;

        return TRUE;
    }

    return FALSE;
}

static void debugLogFunc(const RwChar *message)
{
	MD_SetErrorMessage(const_cast<RwChar*>(message));
}

BOOL	AgcEngine::OnInitialize3D		( void *param ) // �׷��� ��� �ʱ�ȭ
{
	if( !OnInit() ) return FALSE;

#ifndef _RW_FOR_MFC_	
	//@{ Jaewon 20050106
	// At this point, the AgcmUIOption module are not ready...
	// So we should read COption.ini so as to get the multisampling option.
	INT32 useMultisampling = 0;
	ApModuleStream stream;
	stream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if(stream.Open(PATCHCLIENT_OPTION_FILENAME))
	{
		for(int i=0; i<stream.GetNumSections(); ++i)
		{
			const CHAR *sectionName = stream.ReadSectionName(i);
			stream.SetValueID(-1);
	
			if(!strcmp(sectionName, "Video_Option"))
			{
				while(stream.ReadNextValue())
				{
					const CHAR* valueName = stream.GetValueName();
					if(!strcmp(valueName, "Multisampling"))
					{
						stream.GetValue((INT32*)&useMultisampling);
						break;
					}
				}
				break;
			}
		}
	}

	if( !RsRwInitialize( param, useMultisampling ? 6 : 1 ) )
    {
        RsErrorMessage(RWSTRING("Error initializing RenderWare."));
        return FALSE;
    }
#endif

	if ( m_szImagePath )
		RwImageSetPath( m_szImagePath );

#ifdef RWMETRICS
    RsMetricsOpen(Camera);
#endif

	if( !OnRegisterModulePreCreateWorld() )
	{
        RsErrorMessage(RWSTRING("OnRegisterModulePreCreateWorld() Failed"));
        return FALSE;
	}

	m_pWorld = OnCreateWorld();
    if( !m_pWorld )
    {
        RsErrorMessage(RWSTRING("Cannot create world."));
        return FALSE;
    }

	RpWorldSetRenderOrder( m_pWorld, rpWORLDRENDERBACK2FRONT );

    m_pCamera = OnCreateCamera(m_pWorld);
    if( !m_pCamera )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));
        return FALSE;
    }
	
	// ���� ����!!
	if( !OnCreateLight() )
	{
        RsErrorMessage(RWSTRING("Cannot create lights."));
        return FALSE;
	}

	if( !OnPreRegisterModule() )
	{
        RsErrorMessage(RWSTRING("OnPreRegisterModule() Failed"));
        return FALSE;
	}

	if( !OnRegisterModule() )
	{
        RsErrorMessage(RWSTRING("OnRegisterModule() Failed"));
        return FALSE;
	}

	if( !OnPostRegisterModule() )
	{
        RsErrorMessage(RWSTRING("OnPostRegisterModule() Failed"));
        return FALSE;
	}

	LuaInitialize();
	((AgcmImGui *)GetModule("AgcmImGui"))->SetHwnd(m_hWnd);

	// ��� �޴��� �ʱ�ȭ.
	if( !ApModuleManager::Initialize() )
	{
        RsErrorMessage(RWSTRING("ApModuleManager::Initialize() Failed"));
        return FALSE;
	}

	if( !OnPostInitialize() )
	{
		RsErrorMessage(RWSTRING("OnPostInitialize() Failed"));
        return FALSE;
	}

	OnCameraStateChange( CSC_INIT );

	AgcWindow* pWnd = OnSetMainWindow();
	if( !pWnd )
	{
        RsErrorMessage(RWSTRING("Cannot create Main Window."));
        return FALSE;
	}

	SetFullUIModule( pWnd );
	m_pCurD3D9Device = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice ();

	INT32 myVendorID = AcuDeviceCheck::m_iVendorID;
	INT32 myDeviceID = AcuDeviceCheck::m_iDeviceID;

	// 2005.3.31 gemani
	// 4098 - ATI , 20825 - radeon 7000 , 20823 - radeon 7500, 19543 - Mobility Radeon 7500
	if( (myVendorID == 4098 && myDeviceID == 20825) ||
		(myVendorID == 4098 && myDeviceID == 20823) ||
		(myVendorID == 4098 && myDeviceID == 19543))
	{
		((RwGlobals*)RwEngineInstance)->bLockSpecial1 = TRUE;
	}

	((RwGlobals*)RwEngineInstance)->debugLogFunc = debugLogFunc;

    return TRUE;
}

void	AgcEngine::OnTerminate()
{
	m_SocketManager.DisconnectAll();		// Direct Input Teminate;
  
#ifdef RWMETRICS
    RsMetricsClose();
#endif

	ApModuleManager::Destroy();

	// �������� �����ȣ ����.
	if( m_pCurrentFullUIModule )
	{
		m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_CLOSE );
		m_pCurrentFullUIModule->WindowListUpdate();
	}

	ProcessDeleteWindow();

	OnDestroyCamera();

	OnDestroyLight();

    if( m_pWorld )
		RpWorldDestroy( m_pWorld );

	_rpClumpClose( NULL, 0, 0 );

    RsRwTerminate();

	if ( m_clCamera )
	{
		delete m_clCamera;
		m_clCamera = NULL;
	}

	::CoUninitialize();
}

////////////////////////////////////////////////////////////////////
// Function : AgcEngine::OnRender
// Return   :
// Created  : ������ (2002-04-17 ���� 2:06:29)
// Parameter: 
// Note     : ��ü ȭ�� �׸��� ������ ���⼭��. ������â�� ��������
//			������� ȭ�鿡 ����ֱ� ���� �۾��� ��.
// 
// -= Update Log =-
// Raster�� ��� �׷��ִ� ����� �������(�� �Ĺ� â��� �̵�)
////////////////////////////////////////////////////////////////////
void	AgcEngine::OnRender				()
{
	PROFILE("AgcEngine::OnRender");
    ++m_nFrameCounter;

	static bool bRenderStateSet = false;
	
	switch( m_nActivated )
	{
	case	AGCENGINE_NORMAL	:
	case	AGCENGINE_MINOR		:
		// do no op
		break;
	case	AGCENGINE_SUSPENDED	:
	default:
		return;
	}

	if (m_bRenderWorld)
	{
		PROFILE("AgcEngine::OnRender-OnRenderPreCameraUpdate()");
		// ī�޶� ������Ʈ �ű� �� , ���x ���..
		OnRenderPreCameraUpdate	();
	}

	{
		PROFILE("AgcEngine::OnRender-Etc1");

		if( m_pCamera == NULL )
			return;

		RwCameraClear(m_pCamera, &m_rgbBackgroundColor, rwCAMERACLEARZ);//|rwCAMERACLEARIMAGE);

		if ( m_pCurrentFullUIModule && m_bRenderWindow )
			m_pCurrentFullUIModule->OnPreRender( RwCameraGetRaster( m_pCamera ) );

		LockFrame();
	}

	{
		PROFILE("AgcEngine::OnRender - MainRender()");

		if( RwCameraBeginUpdate( g_pEngine->m_pCamera ) )
		{
			UnlockFrame();
			if(!bRenderStateSet)
			{
				RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLNONE	);

				RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE , TRUE);
				RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF   , (void*) 0  );
				RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION , (void*) rwALPHATESTFUNCTIONGREATER );

				bRenderStateSet = true;
			}

			if (m_bRenderWorld)
			{
				OnWorldRender();
			}

				//if( m_pCurrentFullUIModule ) m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_RENDER , NULL);

			if (m_bRenderWindow)
			{
				PROFILE("UI Rendering");

				RwD3D9SetRenderState( D3DRS_FOGENABLE , FALSE );

				//RwD3D9SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
				//RwD3D9SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				//RwD3D9SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

				RwRenderStateSet( rwRENDERSTATETEXTUREFILTER , (void *)rwFILTERNEAREST );
				RwRenderStateSet( rwRENDERSTATETEXTUREADDRESS	, ( void * )rwTEXTUREADDRESSCLAMP  );
				RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)false );
				RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)false );

				RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
				RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

				RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
				RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

				RwD3D9SetTexture( NULL , 1 );

				{
				PROFILE("UI WindowRender");
				if( m_pCurrentFullUIModule )
					m_pCurrentFullUIModule->WindowRender();
				}

				// Modal Window ó��
				AgcModalWindow*		pModalWindow = g_pEngine->GetTopModalWindow();
				if ( NULL != pModalWindow )
					pModalWindow->WindowRender();

				if( m_pCurrentFullUIModule )
					m_pCurrentFullUIModule->OnRender( NULL );
				m_pcsAgcmImGui->Render();

				if( m_bFogEnable )
					RwD3D9SetRenderState( D3DRS_FOGENABLE , TRUE );
			}
								
			RwCameraEndUpdate( g_pEngine->m_pCamera );
		}
		else
			UnlockFrame();
	}

// ������ (2004-07-20 ���� 1:01:15) : ������� �����Ƿ� ����..
//	if( m_pCurrentFullUIModule && m_bRenderWindow )
//	{
//		PROFILE("AgcEngine::OnRender - m_pCurrentFullUIModule->OnPostRender");
//
//		m_pCurrentFullUIModule->OnPostRender( RwCameraGetRaster( m_pCamera ) );
//	}

	// Display Camera Raster
	{
		PROFILE("AgcEngine::OnRender - RsCameraShowRaster");

		RsCameraShowRaster( m_pCamera );	// Flip the Raster! - -+
	}

	// ScreenShot Save
	if(m_bSaveScreen)
	{
		m_bSaveScreen = FALSE;

		ScreenShotSaveGDI();
		//ScreenShotSave();
	}
}

BOOL	AgcEngine::OnIdle				() // �������� ��Ʈ���� ���� idleŸ��ó��
{
	UINT32 ulFrameTick;

	PROFILE("AgcEngine::OnIdle");

	SetTickCount( ::GetTickCount() );
	ulFrameTick = NextFrameTick();
	AuPacket::SetFrameTick(ulFrameTick);

	if( !m_pCurrentFullUIModule ) return FALSE;

	#ifdef _DEBUG
	// ������ (2004-06-25 ���� 4:16:15) : Idle Time ó����..
	if( MagDebug_IsCrashed() ) return FALSE;
	#endif

	if( GetDebugFlag() & AgcEngine::SLEEP_MAIN_THREAD )
	{
		if( GetAsyncKeyState( VK_F5 ) < 0 ) 
		{
			Sleep( 300 );
			return TRUE;
		}
	}

	// Ű���� Ctrl , Alt , Shift �� ������ �ִ� ������ ��������
	// ���̵�Ÿ�ӿ��� �Ź� Ȯ����
	{
		if( ::GetAsyncKeyState( VK_CONTROL ) < 0 )
			AgcEngine::m_bLCtrl = TRUE	;
		else
			AgcEngine::m_bLCtrl = FALSE	;

		if( ::GetAsyncKeyState( VK_MENU ) < 0 )
			AgcEngine::m_bLAlt = TRUE	;
		else
			AgcEngine::m_bLAlt = FALSE	;

		if( ::GetAsyncKeyState( VK_SHIFT ) < 0 )
			AgcEngine::m_bLShift = TRUE	;
		else
			AgcEngine::m_bLShift = FALSE	;
	}

	// ������ (2004-07-20 ���� 5:42:34) : 
	// ���̵� �����ϱ����� ���콺 �Է��� �о� �־��ش�.
	if( g_pEngine->m_bReceivedMouseMessage )
	{
		g_pEngine->m_bReceivedMouseMessage = FALSE;

		RsMouseStatus * param = &m_stLastMouseMoveState;

		// �켱 Set Cursor Mouse Position Callback ȣ�� 
		if ( NULL != g_pEngine->m_pSetCursorPositionCallback  )
		{	
			(g_pEngine->m_pSetCursorPositionCallback)( (PVOID)param, (PVOID)(g_pEngine->m_pSetCursorPositionCallbackClass), NULL );
		}

		AgcModalWindow * pModalWindow;
		if( pModalWindow = g_pEngine->GetTopModalWindow() )
		{
			pModalWindow->UserInputMessageProc( rsMOUSEMOVE, param );
		}
		else
		{
			// Mouse Over ���� üũ 
			RsMouseStatus * pMStatus = (RsMouseStatus *)param;
			AgcWindow* pMouseOverWindow = g_pEngine->WindowFromPoint( (INT32)( pMStatus->pos.x ), (INT32)( pMStatus->pos.y ) );
			
			if ( NULL != pMouseOverWindow && g_pEngine->m_pMouseOverUI != pMouseOverWindow )	// Focus�� �ٲ�!
			{
				if ( NULL != g_pEngine->m_pMouseOverUI )
					g_pEngine->m_pMouseOverUI->OnKillFocus();

				g_pEngine->m_pMouseOverUI = pMouseOverWindow;
				g_pEngine->m_pMouseOverUI->OnSetFocus();
			}
			else if ( NULL == pMouseOverWindow )
			{
				if ( NULL != g_pEngine->m_pMouseOverUI ) 
				{
					g_pEngine->m_pMouseOverUI->OnKillFocus();
					g_pEngine->m_pMouseOverUI = NULL;
				}
			}

			// ������ UI ���� ���콺 �޽����� �ѱ�.
			AgcWindow * pMonopolyWindow = GetMonopolyWindow();
			if ( pMonopolyWindow )	
				pMonopolyWindow->UserInputMessageProc( rsMOUSEMOVE, param );
			else if( g_pEngine->m_pCurrentFullUIModule ) 
				g_pEngine->m_pCurrentFullUIModule->UserInputMessageProc( rsMOUSEMOVE , param );
		}
	}

	BOOL	bRet = TRUE;

	switch( m_nActivated )
	{
	case	AGCENGINE_NORMAL	:
		// do no op
		break;
	case	AGCENGINE_SUSPENDED	:
	case	AGCENGINE_MINOR		:
	default:
		bRet = FALSE;
		break;
	}

    RwUInt32 thisTime;
    //RwReal deltaTime;

    static RwBool firstCall = TRUE;
    static RwUInt32 lastAnimTime , lastFrameTime;

    if( firstCall )
    {
        lastFrameTime = lastAnimTime = GetClockCount();
        //lastFrameTime = lastAnimTime = RsTimer();

        firstCall = FALSE;
    }

    thisTime = GetClockCount();

     //Has a second elapsed since we last updated the FPS...
     //
    if( thisTime > (lastFrameTime + 1000) )
    {
         // 
         // Capture the frame counter...
         //
        m_nFramesPerSecond = m_nFrameCounter;
        
        //
        // ...and reset...
        //
        m_nFrameCounter = 0;
        
        lastFrameTime = thisTime;
    }


    /*
     * Animation update time in seconds...
     */
	//deltaTime = (thisTime - lastAnimTime) * 0.001f;

    /*
     * Update any animations here...
     */

    lastAnimTime = thisTime;

	{
		PROFILE("AgcEngine::OnIdle - Packet Dispatch");
	// Packet Dispatch;
	
		INT16	nCount = 0;
		AcPacketData *pPacketData = NULL;
/*
	// �̰� Casper�� ���� �ڵ忡��~~~~ ������
	if (g_AcReceiveQueue.GetCount() > 5000)
	{
		// Packet Data Drop
		while (!g_AcReceiveQueue.IsEmpty())
		{
			pPacketData = g_AcReceiveQueue.Pop();
			delete pPacketData;
		}

		pPacketData = NULL;
	}
*/	
		DispatchArg	stDispatchArg;
		ZeroMemory(&stDispatchArg, sizeof(stDispatchArg));

		stDispatchArg.bReceivedFromServer	= TRUE;

		while (!g_AcReceiveQueue.IsEmpty())
		{
			pPacketData = g_AcReceiveQueue.Pop();
			if (pPacketData)
			{
				Dispatch((char*)pPacketData->m_Buffer, ( INT16 ) pPacketData->m_lLength, pPacketData->m_lNID, &stDispatchArg);

				//delete pPacketData;
				//m_SocketManager.FreePacket(pPacketData);
			}
			else break;

			if (nCount > m_lOneTimePopCount)
				break;

			++nCount;
		}
	}

	if( GetImmediateIdleTerminateFlag() ) return TRUE;

	if (thisTime != m_ulPrevProcessClock)
	{
		// ��ϵ� Module���� ���� �ҷ����� �ɰ� ������.... By Parn
		if( IsCheckSlowIdle() )
		{
			Debug_Idle( thisTime );
			Debug_Idle2( thisTime );
		}
		else
		{
			ApModuleManager::Idle( thisTime );
			ApModuleManager::Idle2( thisTime );
		}
		
		if( !m_bSuspendedFlag )
		{
			// ���ϵ� ���̵� ȣ��
			// �켱 ǽ������ ���� ( thisTime �־��.  By Parn) 
			if( m_pCurrentFullUIModule ) 
			{
				PROFILE("AgcEngine::OnIdle - Window Message");

				//m_pCurrentFullUIModule->OnIdle( thisTime );
				m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_IDLE , ( void * ) thisTime , NULL );
			}

			{
				PROFILE("AgcEngine::OnIdle - WindowListUpdate");
				WindowListUpdate();
			}
			// ������ ������ ����Ŭ.

			{
			PROFILE("AgcEngine::OnIdle - Render");
			// Mutex�� Unlocked�̾�� Render�Ѵ�.
			OnRender();
			}
		}
	}

	if (!m_lIdleModal)
	{
		PROFILE("AgcEngine::OnIdle - DeleteWindows");

		ProcessDeleteWindow();
	}

	// Lua Main Thread Processing.
	ProcessIdleLuaCommand();

	OnEndIdle();

	m_ulPrevProcessClock	= thisTime;

	// ������ (2004-10-06 ���� 11:01:02) : 
	// ����üũ ���..
	// �޽��� �ڽ��� ���� ������ ���� ��������� �����ؾ���.
//	MD_ErrorCheck();

    return	bRet;
}

BOOL	AgcEngine::OnAttachInputDevices	()
{
    RsInputDeviceAttach(	rsKEYBOARD,	AgcEngine::KeyboardHandler	);
    RsInputDeviceAttach(	rsMOUSE,	AgcEngine::MouseHandler		);

	return TRUE;
}

BOOL		AgcEngine::OnCameraResize		( RwRect * pRect )
{
	LockFrame();

	m_bSuspendedFlag = m_clCamera->CameraSize( m_pCamera , pRect , DEFAULT_VIEWWINDOW , DEFAULT_ASPECTRATIO_NORMAL ) ? false : true;
	SetProjection();

	UnlockFrame();

	if (m_pCurrentFullUIModule)
		m_pCurrentFullUIModule->MoveWindow(0, 0, pRect->w - pRect->x, pRect->h - pRect->y);

	OnCameraStateChange(CSC_RESIZE);
	
    return TRUE;
}

BOOL		AgcEngine::OnDestroyCamera		()
{
    if( m_pCamera )
    {
        RpWorldRemoveCamera(m_pWorld, m_pCamera);

        m_clCamera->CameraDestroy(m_pCamera);
		m_pCamera = NULL;
    }

	return TRUE;
}

UINT32	AgcEngine::OnWindowEventHandler	(HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

RsEventStatus	AgcEngine::OnMessageSink	( RsEvent event, void *param ) // �޽����� ó���Ͽ� ���鿡�� �Ѱ���
{
    switch( event )
    {
	case rsIMEStartComposition:
		{
			m_bIMEComposingState	= TRUE	;
			return rsEVENTPROCESSED;
		}
	case rsIMEEndComposition:
		{
			m_bIMEComposingState	= FALSE	;
			return rsEVENTPROCESSED;
		}
	case rsINITIALIZE:
        {
            return OnInitialize() ? rsEVENTPROCESSED : rsEVENTERROR;
        }
		
	case rsCAMERASIZE:
        {
            return OnCameraResize( (RwRect *)param ) ? rsEVENTPROCESSED : rsEVENTERROR;
        }
		
	case rsRWINITIALIZE:
        {
			// ������ (2004-06-23 ���� 5:22:50) : ������ ��� ����..
			// RW�� �ʱ�ȭ �ű� ���� ȣ��´�.
			if( RsGlobal.ps )
			{
				m_hWnd = (((psGlobalType *)(RsGlobal.ps))->window);
			}
			else
			{
				m_hWnd = NULL;
			}

            return OnInitialize3D(param) ? rsEVENTPROCESSED : rsEVENTERROR;
        }
		
	case rsRWTERMINATE:
        {
            OnTerminate();
			
            return rsEVENTPROCESSED;
        }
	case rsPLUGINATTACH:
        {
            return OnAttachPlugins() ? rsEVENTPROCESSED : rsEVENTERROR;
        }
		
	case rsINPUTDEVICEATTACH:
        {
            OnAttachInputDevices();
			
            return rsEVENTPROCESSED;
        }
	case rsIDLE:
        {
			// ���̵� Ÿ�� ����.
			SetTickCount( ( UINT ) param );

            if( OnIdle() )
				return rsEVENTPROCESSED	;
			else
				return rsEVENTERROR		;
        }
	case rsREGISTERIMAGELOADER:
        {
			RwImageRegisterImageFormat( RWSTRING("bmp") , RtBMPImageRead	, RtBMPImageWrite	);
			RwImageRegisterImageFormat( RWSTRING("png") , RtPNGImageRead	, RtPNGImageWrite	);
			RwImageRegisterImageFormat( RWSTRING("tif") , RtTIFFImageRead	, NULL				);
            return rsEVENTPROCESSED;
		}
			
	case rsACTIVATE:
		{			
			SetActivate( ( INT32 ) param );
			return rsEVENTPROCESSED;
		}
	case rsSELECTDEVICE:
		{
			// ��ġ ����..
			return OnSelectDevice() ? rsEVENTPROCESSED : rsEVENTNOTPROCESSED;
		}
	default:
		{
			return rsEVENTNOTPROCESSED;
		}
	}
}

RwCamera *AgcEngine::CreateCamera			( RpWorld *world )
{
    RwCamera* camera = m_clCamera->CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);
	if( !camera )		return NULL;

	LockFrame();

    RwCameraSetNearClipPlane( camera , m_fNearClipPlane	);
    RwCameraSetFarClipPlane( camera , m_fFarClipPlane );
    RpWorldAddCamera( world , camera );

	UnlockFrame();

	RwReal NearZ = RwIm2DGetNearScreenZ();
	RwReal recipCameraZ = 1.0f / RwCameraGetNearClipPlane(camera);

	for( int i=0; i<4; ++i )
	{
		RwIm2DVertexSetScreenZ(&m_vBoxFan[i], NearZ);
		RwIm2DVertexSetRecipCameraZ(&m_vBoxFan[i], recipCameraZ);
	}

	return camera;
}

RpWorld		*AgcEngine::CreateWorld()
{
	RwBBox bb;
	bb.inf.x = bb.inf.y = bb.inf.z = -100.0f;
	bb.sup.x = bb.sup.y = bb.sup.z = 100.0f;

    return RpWorldCreate( & bb );
}

void	AgcEngine::CreateLights(void)
{
    m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);

	LockFrame();

    if (m_pLightAmbient)
    {
		RwRGBAReal color = {1.0f, 1.0f, 1.0f, 1.0f};
		RpLightSetColor(m_pLightAmbient, &color);
		RpWorldAddLight(m_pWorld, m_pLightAmbient);
    }

    m_pLightDirect = RpLightCreate(rpLIGHTDIRECTIONAL);

    if (m_pLightDirect)
    {
        RwFrame *f = RwFrameCreate();
        RpLightSetFrame( m_pLightDirect , f);

		RwV3d v = { 0.0f , 10.0f , 0.0f };
		RwV3d xaxis = { 1.0f , 0.0f , 0.0f };

		RwFrameTranslate( f , & v , rwCOMBINEREPLACE );
		RwFrameRotate( f , & xaxis , 90.0f , rwCOMBINEREPLACE );

		RwRGBAReal color = {1.0f, 1.0f, 1.0f, 1.0f};
		RpLightSetColor(m_pLightDirect, &color);
		RpWorldAddLight(m_pWorld, m_pLightDirect);
    }

	UnlockFrame();
}

void		AgcEngine::DestroyLights		(void)
{
	if (m_pLightDirect)
	{
		RwFrame *f;

		RpWorldRemoveLight	( m_pWorld , m_pLightDirect );

		f					= RpLightGetFrame	( m_pLightDirect );
		RpLightSetFrame		( m_pLightDirect , NULL );
		RwFrameDestroy		( f );
		RpLightDestroy		( m_pLightDirect );

		m_pLightDirect		= NULL;
	}
	
	if (m_pLightAmbient)
	{
		RpWorldRemoveLight	( m_pWorld , m_pLightAmbient );
		RpLightDestroy		( m_pLightAmbient );
		m_pLightAmbient		= NULL;
	}
	
}

RsEventStatus	AgcEngine::KeyboardHandler	( RsEvent event , void *param )
{
	RsEventStatus	eStatus = rsEVENTNOTPROCESSED;
    /*
     * Let the menu system have a look-in first...
     */

	// ���� ��ü������ Shift/Ctrl/Alt Key�� ���� status�� �ľ��ؾ� �Ѵ�. (Parn, 2004/10/24 03:17)
	if( event == rsKEYDOWN )
	{		
		switch ( ((RsKeyStatus *) param)->keyCharCode )
		{
		case rsLSHIFT:
			TRACE( "m_bLShift = TRUE\n" );
			m_bLShift = TRUE;
			break;
		case rsRSHIFT:
			m_bRShift = TRUE;
			break;
		case rsLCTRL:
			m_bLCtrl = TRUE;
			break;
		case rsRCTRL:
			m_bRCtrl = TRUE;
			break;
		case rsLALT:
			m_bLAlt = TRUE;
			break;
		case rsRALT:
			m_bRAlt = TRUE;
			break;
		case rsESC:
			m_bLShift	= FALSE;
			m_bRShift	= FALSE;
			m_bLCtrl	= FALSE;
			m_bRCtrl	= FALSE;
			m_bLAlt		= FALSE;
			m_bRAlt		= FALSE;

			// Ȥ�� ��� ��ũ�� �������̸� �����Ѵ�.
			g_pEngine->Lua_Flush_Queue();

			break;
		}

		//if(((RsKeyStatus *) param)->keyScanCode == 242 || ((RsKeyStatus *) param)->keyCharCode == rsRALT) // �ѿ�Ű
		//{
		//	m_iInputKeyMode = !m_iInputKeyMode;
		//}
	}
	else if( event == rsKEYUP )
	{
		switch ( ((RsKeyStatus *) param)->keyCharCode )
		{
		case rsLSHIFT:
			TRACE( "m_bLShift = FALSE\n" );
			m_bLShift = FALSE;
			break;
		case rsRSHIFT:
			m_bRShift = FALSE;
			break;
		case rsLCTRL:
			m_bLCtrl = FALSE;
			break;
		case rsRCTRL:
			m_bRCtrl = FALSE;
			break;
		case rsLALT:
			m_bLAlt = FALSE;
			break;
		case rsRALT:
			m_bRAlt = FALSE;
			break;
		}
	}

	// Ű �Է� ������Ʈ�� ���� ���͸�.

	// ������ (2005-03-08 ���� 12:13:54) : 
	// ī�޶� Ű�Է�ó������ , ����Ʈâ �Է½� Ű�� ���޴� ������ �ִ�..
	// ���� �ڵ�δ� ����Ʈ â�� �߸� �ٸ� keyup �޽����� ���޵��� �ʴµ�..
	// �̰� �� ��������� �� ����� �ȳ�����..
	// �ϴ� ����� �׽�Ʈ�غ��� �ߵŴ°� ���Ƽ� �ּ�ó���ϰ� üũ��..
	/*
	if( event == rsKEYDOWN		&& g_pEngine->GetCharCheckState() )
	{
		RsKeyStatus *	pstKeyStatus	=	(RsKeyStatus *) param;

		if (!pstKeyStatus->keyScanCode == VK_LEFT &&
			!pstKeyStatus->keyScanCode == VK_RIGHT &&
			!pstKeyStatus->keyScanCode == VK_UP &&
			!pstKeyStatus->keyScanCode == VK_RIGHT)
			return rsEVENTPROCESSED;
	}
	if( event == rsKEYUP		&& g_pEngine->GetCharCheckState() )
	{
		return rsEVENTPROCESSED;
	}
	*/

	if( g_pEngine->GetCharCheckState() )
	{
		// ������ (2006-05-12 ���� 6:21:14) : 
		// Ű�Է��� ����� ��츸 Ʈ��������Ʈ�� ��.
		if( TranslateMessage(&g_msgLastReceived) )
		{
			switch( g_msgLastReceived.wParam )
			{
			case VK_UP		:
			case VK_LEFT	:
			case VK_RIGHT	:
			case VK_DOWN	:
			case VK_DELETE	:
				break;

			default:
				// Ű���� �ִ°�� vk �н�
				return rsEVENTPROCESSED;
			}
		}
	}

	if( event == rsCHAR			&& !g_pEngine->GetCharCheckState() )
	{
		char * pStr = ( char * ) param;
		if( * pStr == 13 ) // Return
		{
			// do nothing..
		}
		else
			// �׿ܿ� �����ع���..
			return rsEVENTPROCESSED;
	}
	if( event == rsIMECOMPOSING	&& !g_pEngine->GetCharCheckState() )
	{
		return rsEVENTPROCESSED;
	}

	AgcModalWindow * pModalWindow;
	if( pModalWindow = g_pEngine->GetTopModalWindow() )
	{
		g_pEngine->m_bModalMessage	= FALSE;
		eStatus	= pModalWindow->UserInputMessageProc( event, param );
		g_pEngine->m_bModalMessage	= TRUE;

		// �޽����� �ѱ��� �ʴ´�. �ѱ�����, UserInputMessageProc�� ���ؼ� �ɷ�����.
		if (eStatus == rsEVENTPROCESSED)
			return rsEVENTPROCESSED;
	}
	else
		g_pEngine->m_bModalMessage	= FALSE;

	// ������ UI ���� Ű���� �޽����� �ѱ�.

	// �ٺ����� �������� Ǯ�����̿� Ű���� �޽����� �ѱ�.
	AgcWindow * pMonopolyWindow = g_pEngine->GetMonopolyWindow();
	if ( pMonopolyWindow )	
		pMonopolyWindow->UserInputMessageProc( event, param );
	else if( g_pEngine->m_pCurrentFullUIModule ) 
		eStatus	= g_pEngine->m_pCurrentFullUIModule->UserInputMessageProc( event , param );
	


	return eStatus;
}

RsEventStatus	AgcEngine::MouseHandler		( RsEvent event , void *param )
{
	RsEventStatus	eStatus = rsEVENTNOTPROCESSED;

	//@{ 2006/08/29 burumal
	if ( event == rsLEFTBUTTONDOWN )
		m_bMouseLeftBtn	= TRUE;
	else
	if ( event == rsLEFTBUTTONUP )
		m_bMouseLeftBtn	= FALSE;

	if  (event == rsRIGHTBUTTONDOWN )
		m_bMouseRightBtn = TRUE;
	else
	if  (event == rsRIGHTBUTTONUP )
		m_bMouseRightBtn = FALSE;

	if ( event == rsMIDDLEBUTTONDOWN )
		m_bMouseMiddleBtn = TRUE;
	else
	if ( event == rsMIDDLEBUTTONUP )
		m_bMouseMiddleBtn = FALSE;
	//@}

	// ������ (2004-07-20 ���� 5:42:01) : �۽�~..
	if( rsMOUSEMOVE == event )
	{
		RsMouseStatus * pMStatus = (RsMouseStatus *)param;
		g_pEngine->m_stLastMouseMoveState	= * pMStatus;
		g_pEngine->m_bReceivedMouseMessage = TRUE;

		return rsEVENTNOTPROCESSED;
	}
	/*
     * Let the menu system have a look-in first...
     */
//    if( g_pEngine->m_clMenu->MenuMouseHandler(event, param) == rsEVENTPROCESSED )
  //  {
    //    return rsEVENTPROCESSED;
    //}
			
	AgcModalWindow * pModalWindow;
	if( pModalWindow = g_pEngine->GetTopModalWindow() )
	{
		g_pEngine->m_bModalMessage	= FALSE;
		eStatus	= pModalWindow->UserInputMessageProc( event, param );
		g_pEngine->m_bModalMessage	= TRUE;

		// �޽����� �ѱ��� �ʴ´�. �ѱ�����, UserInputMessageProc�� ���ؼ� �ɷ�����.
		if (eStatus == rsEVENTPROCESSED)
			return rsEVENTPROCESSED;
	}
	else
		g_pEngine->m_bModalMessage	= FALSE;

	if (rsLEFTBUTTONDOWN == event || rsLEFTBUTTONDBLCLK == event || 
		rsRIGHTBUTTONDOWN == event || rsRIGHTBUTTONDBLCLK == event ||
		rsMIDDLEBUTTONDOWN == event || rsMIDDLEBUTTONDBLCLK == event)
	{
		//@{ Jaewon 20050218
		// g_pEngine->m_pActiveEdit->m_bUIWindow
		// In case of console, do not release.
		if ( g_pEngine->GetCharCheckState() && g_pEngine->m_pActiveEdit 
			&& g_pEngine->m_pActiveEdit->m_bUIWindow)
		//@} Jaewon
			g_pEngine->ReleaseMeActiveEdit(g_pEngine->m_pActiveEdit);
	}

	// ������ UI ���� ���콺 �޽����� �ѱ�.
	AgcWindow * pMonopolyWindow = g_pEngine->GetMonopolyWindow();
	if ( pMonopolyWindow )	
		pMonopolyWindow->UserInputMessageProc( event, param );
	else if( g_pEngine->m_pCurrentFullUIModule ) 
		eStatus = g_pEngine->m_pCurrentFullUIModule->UserInputMessageProc( event , param );

    return eStatus;
}

BOOL	AgcEngine::AddChild( AgcWindow * pModule , INT32 nid )
	// ������ UI �� ����. pParent �� NULL �ϰ�� , ž�����̴�.;
	// Close�� ��⿡�� �˾Ƽ�..
{
	// pModule �� Open�ž� �ִ��� ����
	if( pModule->m_bOpened ) return FALSE;// �̹� �����ִ�!
	else
	{
		ASSERT( NULL != m_pCurrentFullUIModule );
		if( NULL == m_pCurrentFullUIModule ) return FALSE;

		pModule->m_bOpened	= TRUE;

		// FullUIWindow�� Child�� ���� �ȴ�.

		m_pCurrentFullUIModule->AddChild( pModule , nid );
	}

	return TRUE;
}

BOOL	AgcEngine::CloseWindowUIModule( AgcWindow * pModule )	// ž������ üũ��.
{
	// ž������ �����쿡�� �ش� �����Ͱ� �ִ°�� ������� Ŭ���� ��Ŵ.

	pModule->WindowControlMessageProc( AgcWindow::MESSAGE_CLOSE );
	pModule->m_bOpened = FALSE;// �𽺿��̺� ���ѳ���..

	return TRUE;
}

////////////////////////////////////////////////////////////////////
// Function : AgcEngine::SetFullUIModule
// Return   :
// Created  : ������ (2002-04-17 ���� 12:08:54)
// Parameter: 
// Note     : 
// 
// -= Update Log =-
////////////////////////////////////////////////////////////////////
BOOL	AgcEngine::SetFullUIModule			( AgcWindow *pModule )
{
	// Full UI��� �����͸� �����ϰ� , 
	// ���� UI ����� ����Ÿ�� ����
	// �� UI ����� ����Ÿ �ε�
	// ������ ����.

	if( m_pCurrentFullUIModule )
	{
		// UI����Ÿ ����
		m_pCurrentFullUIModule->WindowControlMessageProc( AgcWindow::MESSAGE_CLOSE );

		// ������ �ʱ�ȭ
		m_pCurrentFullUIModule	= NULL;
	}

	// �� UI ��� �ʱ�ȭ
	if( pModule )
	{
		pModule->m_bOpened	= TRUE;
		pModule->WindowControlMessageProc( AgcWindow::MESSAGE_INIT );
	}

	m_pCurrentFullUIModule	= pModule;

	return TRUE;
}


UINT	AgcEngine::SetTickCount			( UINT tick )
{
	m_uPastTick		= tick - m_uCurrentTick	;
	m_uCurrentTick	= tick					;
	return m_uCurrentTick;
}

BOOL	AgcEngine::OnCreateLight		()	// ������ �����ؼ�. �� ������ �������ش�. m_pLightAmbient , m_pLightDirect
{
	// �⺻ ����Ʈ ó��.
	CreateLights();
	return TRUE;
}

RpWorld *	AgcEngine::OnCreateWorld			() // ���带 �����ؼ� �������ش�.
{
	return CreateWorld();
}
RwCamera *	AgcEngine::OnCreateCamera			( RpWorld * pWorld )	// ���� ī�޶� Create���ش�.
{
	return CreateCamera( pWorld );
}

void		AgcEngine::OnDestroyLight	()
{
	DestroyLights();
}

INT32		AgcEngine::SetActivate			( RwBool nActivated		)
{
	INT32	prev = m_nActivated;
	m_nActivated = nActivated;

	if(nActivated == AGCENGINE_NORMAL)
		OnCameraStateChange(CSC_ACTIVATE);

	return prev;
}

INT32		AgcEngine::GetActivate			( void					)
{
	return m_nActivated;
}


//************************************
// Method:    SendPacket
// FullName:  AgcModule::SendPacket
// Access:    public 
// Returns:   BOOL
// Qualifier:   ����ü�� �̿��� ��Ŷ����(PACKET_HEADER)
// Author: arycoat - 2008.03.
// Parameter: PACKET_HEADER & pPacket
// Parameter: UINT32 ulNID
// Parameter: PACKET_PRIORITY ePriority
// Parameter: INT16 nFlag
//************************************
BOOL AgcEngine::SendPacket( PACKET_HEADER& pPacket )
{
	UINT16	nLength = pPacket.unPacketLength + 1; // end guard byte

	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// ��Ŷ �յڷ� �������Ʈ�� ���δ�.

	if (!pvPacketRaw) return FALSE;

	CopyMemory(pvPacketRaw, &pPacket, nLength);
	
	// set guard byte
	*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
	*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;
	
	((PACKET_HEADER *) pvPacketRaw)->unPacketLength		= nLength;
	((PACKET_HEADER *) pvPacketRaw)->lOwnerID			= 0;
	((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= 0;
	((PACKET_HEADER *) pvPacketRaw)->cType				= pPacket.cType;

	return g_pEngine->SendPacket(pvPacketRaw, nLength);
}

BOOL AgcEngine::SendPacket(PVOID pvPacket, INT16 nLength, UINT32 nFlag, PACKET_PRIORITY ePriority, INT16 ulNID)
{
	INT32 result = m_SocketManager.Send((char*)pvPacket, nLength, nFlag, ulNID);
	return ( (result != SOCKET_ERROR) && (0 != result) );
}

BOOL AgcEngine::SetSelfCID(INT32 lCID)
{
	m_SocketManager.SetSelfCID(lCID);
	return  TRUE;
}

BOOL	AgcEngine::SetMaxConnection	( INT32 lMaxConnection, INT32 lMaxPacketBufferSize	)
{
	return m_SocketManager.SetMaxConnection(lMaxConnection, lMaxPacketBufferSize);
}

INT16	AgcEngine::connect( CHAR *pszServerAddr, INT16 nServerType, PVOID pClass, ApModuleDefaultCallBack fpConnect, ApModuleDefaultCallBack fpDisconnect, ApModuleDefaultCallBack fpError )
{
	return m_SocketManager.Connect( pszServerAddr, nServerType, pClass, fpConnect, fpDisconnect, fpError );
}

BOOL	AgcEngine::disconnect	( INT16 nNID			)
{
	m_SocketManager.Disconnect( nNID );
	return TRUE;
}

void	AgcEngine::WindowListUpdate()
{

	if( m_pCurrentFullUIModule )
	{
		m_pCurrentFullUIModule->WindowListUpdate();
		{
			PROFILE("AgcEngine::WindowListUpdate() - MessageQueueProcessing()");
			m_pCurrentFullUIModule->MessageQueueProcessing();
		}
	}
}

void	AgcEngine::RwLockFrame()
{
	g_pEngine->m_csMutexFrame.Lock();
}

void	AgcEngine::RwUnlockFrame()
{
	g_pEngine->m_csMutexFrame.Unlock();
}

void	AgcEngine::RwLockTexture()
{
	g_pEngine->m_csMutexTexture.Lock();
}

void	AgcEngine::RwUnlockTexture()
{
	g_pEngine->m_csMutexTexture.Unlock();
}

void	AgcEngine::RwLockResArena()
{
	g_pEngine->m_csMutexResArena.Lock();
}

void	AgcEngine::RwUnlockResArena()
{
	g_pEngine->m_csMutexResArena.Unlock();
}

void	AgcEngine::RwLockClump()
{
	g_pEngine->m_csMutexClump.Lock();
}

void	AgcEngine::RwUnlockClump()
{
	g_pEngine->m_csMutexClump.Unlock();
}

void*	AgcEngine::RwAllocFreeListCB()
{
	LPCRITICAL_SECTION	pSection = new CRITICAL_SECTION;

	InitializeCriticalSection(pSection);
	return (void*)pSection;
}

void	AgcEngine::RwFreeFreeListCB(void*	pCriticalSection)
{
	if(pCriticalSection)
	{
		DeleteCriticalSection((LPCRITICAL_SECTION)pCriticalSection);
		delete pCriticalSection;
	}
}

void	AgcEngine::RwLockFreeList(void*	pCriticalSection)
{
	EnterCriticalSection((LPCRITICAL_SECTION)pCriticalSection);
}

void	AgcEngine::RwUnlockFreeList(void*	pCriticalSection)
{
	LeaveCriticalSection((LPCRITICAL_SECTION)pCriticalSection);
}

void*	AgcEngine::RwResEntryCreateCB()
{
	LPCRITICAL_SECTION	pSection = new CRITICAL_SECTION;

	InitializeCriticalSection(pSection);
	return (void*)pSection;
}

void	AgcEngine::RwResEntryFreeCB(void*	pCriticalSection)
{
	if(pCriticalSection)
	{
		DeleteCriticalSection((LPCRITICAL_SECTION)pCriticalSection);
		delete pCriticalSection;
	}
}

void	AgcEngine::RwLockResEntry(void*	pCriticalSection)
{
	EnterCriticalSection((LPCRITICAL_SECTION)pCriticalSection);
}

void	AgcEngine::RwUnlockResEntry(void*	pCriticalSection)
{
	LeaveCriticalSection((LPCRITICAL_SECTION)pCriticalSection);
}

AgcWindow *	AgcEngine::WindowFromPoint		( INT32 x , INT32 y, BOOL bCheckUseInput )
{
	// ������ UI ���� ���콺 �޽����� �ѱ�.
	if( m_pCurrentFullUIModule )
	{
		return m_pCurrentFullUIModule->WindowFromPoint( x , y , bCheckUseInput );
	}
	else
	{
		return NULL;
	}
}

BOOL	AgcEngine::PushMonopolyWindow( AgcWindow* pWindow )
{
	PopMonopolyWindow( pWindow );
	m_vecMonopolyUI.push_back( pWindow );

	return TRUE;
}
	
BOOL	AgcEngine::PopMonopolyWindow( AgcWindow* pWindow )
{
	vector< AgcWindow * >::iterator	iter;

	for( iter = m_vecMonopolyUI.begin() ;
		iter != m_vecMonopolyUI.end();
		iter++ )
	{
		AgcWindow * pMonopolyUI	= *iter;

		if ( pMonopolyUI == pWindow )
		{ // Modal Window�� �����Ѵ�. 
			m_vecMonopolyUI.erase( iter );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	AgcEngine::OpenModalWindow		( AgcModalWindow * pWindow )
{
	// ��ġ���� �˻�!..
	AuNode< AgcModalWindow * >	* pNode = m_listModalStack.GetHeadNode();
	AgcModalWindow * pModalWindow;
	while( pNode )
	{
		pModalWindow	= pNode->GetData();
		if( pModalWindow == pWindow )
		{
			// ��ġ�°� �ִ�!
			ASSERT( !"��������찡 �ߺ� ���µž����ϴ�\n" );
			return FALSE;
		}

		m_listModalStack.GetNext( pNode );
	}

	// �ߺ� �˻� ��..
	// �ڿ��� �߰���.
	m_listModalStack.AddTail( pWindow );
	return TRUE;
}

BOOL	AgcEngine::CloseModalWindow		( AgcModalWindow * pWindow )
{
	AuNode< AgcModalWindow * >	* pNode = m_listModalStack.GetHeadNode();
	AgcModalWindow * pModalWindow;
	while( pNode )
	{
		pModalWindow	= pNode->GetData();
		if( pModalWindow == pWindow )
		{
			// ��ġ�°� �ִ�!
			// �ش� ��� ����.
			// �Ϲ������� �� �� �༮�̾����.
			m_listModalStack.RemoveNode( pNode );
			return TRUE;
		}

		m_listModalStack.GetNext( pNode );
	}

	ASSERT( !"��޿� ���� �� �ݱ� ��û!" );
	return FALSE;
}

AgcModalWindow *	AgcEngine::GetTopModalWindow()
{
	if( m_listModalStack.GetCount() )
	{
		AuNode< AgcModalWindow * >	* pNode = m_listModalStack.GetTailNode();
		ASSERT( NULL != pNode );
		return pNode->GetData();
	}
	else
		return NULL;
}

BOOL	AgcEngine::IsModalWindow		( AgcModalWindow * pWindow )
{
	AuNode< AgcModalWindow * >	* pNode = m_listModalStack.GetHeadNode();
	AgcModalWindow * pModalWindow;
	while( pNode )
	{
		pModalWindow	= pNode->GetData();
		if( pModalWindow == pWindow )
		{
			return TRUE;
		}

		m_listModalStack.GetNext( pNode );
	}
	return FALSE;
}

INT32	AgcEngine::WaitingDialog( AgcWindow* pWindow, char* pszMessage )
{
	if ( NULL == pWindow ) 
	{
		if ( NULL == m_pCurrentFullUIModule ) return -1;
		pWindow = m_pCurrentFullUIModule;
	}
	
	if ( NULL != m_pWaitingDialog )
	{
		m_pWaitingDialog->ShowWindow( TRUE );
		//m_pWaitingDialog->SetButtonName( "����" );
		m_pWaitingDialog->SetMessage( pszMessage );
		return m_pWaitingDialog->DoModal( pWindow );
	}
	else
		return -1;
}

INT32	AgcEngine::WaitingDialogEnd()
{
	if( !m_pWaitingDialog )		return -1;

	m_pWaitingDialog->EndDialog(0);
	return 0;
}

INT32	AgcEngine::MessageDialog( AgcWindow* pWindow, char* pszMessage )
{
	if ( NULL == pWindow ) 
	{
		if ( NULL == m_pCurrentFullUIModule ) return -1;
		pWindow = m_pCurrentFullUIModule;
	}
	
	if ( NULL != m_pMessageDialog )
	{
		m_pMessageDialog->ShowWindow( TRUE );
		INT32 lReturn = -1;
		m_pMessageDialog->SetMessage( pszMessage );
		lReturn = m_pMessageDialog->DoModal( pWindow );
		//m_pMessageDialog->ShowWindow( FALSE );
		return lReturn;
	}
	else
		return -1;
}

INT32	AgcEngine::MessageDialog2( AgcmUIManager2* pWindow, char* pszMessage )
{
	if( NULL != m_pMessageDialog2 )
	{
		INT32 lReturn = -1;
		m_pMessageDialog2->SetMessage( pszMessage );
		m_pMessageDialog2->ShowWindow( TRUE );
		pWindow->AddWindow( m_pMessageDialog2 );
				
		return lReturn;
	}

	return -1;

}

INT32	AgcEngine::EditMessageDialog( AgcWindow* pWindow, char* pszMessage, CHAR* pEditString )
{
	if ( NULL == pWindow ) 
	{
		if ( NULL == m_pCurrentFullUIModule ) return -1;
		pWindow = m_pCurrentFullUIModule;
	}

	if( NULL != m_pEditOkDialog )
	{
		INT32 lReturn = -1;
		m_pEditOkDialog->SetMessage( pszMessage );
		m_pEditOkDialog->ShowWindow( TRUE );

		AcUIEditOKDialog* pEditOKDialog = ( AcUIEditOKDialog* )m_pEditOkDialog;
		pEditOKDialog->m_clEdit.ShowWindow( TRUE );

		lReturn = m_pEditOkDialog->DoModal( pWindow );
		const CHAR* pEditBoxText = pEditOKDialog->m_clEdit.GetText();
		if( pEditBoxText && strlen( pEditBoxText ) > 0 )
		{
			strcpy_s( pEditString, sizeof( char ) * 256, pEditOKDialog->m_clEdit.GetText() );
			pEditOKDialog->m_clEdit.ClearText();
		}

		return lReturn;
	}

	return -1;
}

INT32	AgcEngine::EditMessageDialog2( AgcWindow* pWindow, char* pszMessage, CHAR* pEditString )
{
	if ( NULL == pWindow ) 
	{
		if ( NULL == m_pCurrentFullUIModule ) return -1;
		pWindow = m_pCurrentFullUIModule;
	}

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	if( m_pEditOkDialog && pcmUIManager )
	{
		m_pEditOkDialog->SetMessage( pszMessage );
		m_pEditOkDialog->ShowWindow( TRUE );

		AcUIEditOKDialog* pEditOKDialog = ( AcUIEditOKDialog* )m_pEditOkDialog;
		pEditOKDialog->m_clEdit.ShowWindow( TRUE );

		m_pEditOkDialog->ShowWindow( TRUE );
		pcmUIManager->AddWindow( m_pEditOkDialog );

		const CHAR* pEditBoxText = pEditOKDialog->m_clEdit.GetText();
		if( pEditBoxText && strlen( pEditBoxText ) > 0 )
		{
			strcpy_s( pEditString, sizeof( char ) * 256, pEditOKDialog->m_clEdit.GetText() );
			pEditOKDialog->m_clEdit.ClearText();
		}

		return m_pOkCancelDialog->GetReturnValue();
	}

	return -1;
}


INT32	AgcEngine::OkCancelDialog( AgcWindow* pWindow, char* pszmessage )
{
	if ( NULL == pWindow ) 
	{
		if ( NULL == m_pCurrentFullUIModule ) return -1;
		pWindow = m_pCurrentFullUIModule;
	}

	if ( NULL != m_pOkCancelDialog )
	{
		m_pOkCancelDialog->ShowWindow( TRUE );
		INT32 lReturn = -1;
		m_pOkCancelDialog->SetMessage( pszmessage );
		lReturn = m_pOkCancelDialog->DoModal( pWindow );
		//m_pOkCancelDialog->ShowWindow( FALSE );
		return lReturn;
	}
	else 
		return -1;
}

INT32	AgcEngine::OkCancelDialog2( AgcWindow* pWindow, char* pszmessage )
{
	if ( NULL == pWindow ) 
	{
		if ( NULL == m_pCurrentFullUIModule ) return -1;
		pWindow = m_pCurrentFullUIModule;
	}

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );

	if ( m_pOkCancelDialog && pcmUIManager )
	{
		m_pOkCancelDialog->SetMessage( pszmessage );
		m_pOkCancelDialog->ShowWindow( TRUE );
		pcmUIManager->AddWindow( m_pOkCancelDialog );

		return m_pOkCancelDialog->GetReturnValue();
	}
	else 
		return -1;
}

#ifdef _AREA_GLOBAL_
INT32	AgcEngine::ThreeBtnDialog( AgcWindow* pWindow, char* pszmessage )
{
	if ( NULL == pWindow ) 
	{
		if ( NULL == m_pCurrentFullUIModule ) return -1;
		pWindow = m_pCurrentFullUIModule;
	}

	if ( NULL != m_pThreeBtnDialog )
	{
		m_pThreeBtnDialog->ShowWindow( TRUE );
		m_pThreeBtnDialog->SetMessage( pszmessage );
		INT32 iRet = m_pThreeBtnDialog->DoModal( pWindow );

		return iRet;
	}
	else 
		return -1;
}
#endif

/*****************************************************************
*   Function : SetMeActiveEdit
*   Comment  : pbActive�� ������ �ִ� Edit�� Active��Ų�� 
*   Date&Time : 2003-07-07, ���� 4:31
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL	AgcEngine::SetMeActiveEdit( AgcWindow*	pEdit	)
{
	// ���� Active Edit�� ���ٸ� 
	if ( NULL == m_pActiveEdit )
	{
		m_pActiveEdit					=	pEdit	;
		m_pActiveEdit->m_bActiveEdit	=	TRUE	;
		m_pActiveEdit->WindowControlMessageProc( AgcWindow::MESSAGE_EDIT_ACTIVE	);
	
		if( !g_pEngine->GetCharCheckState() )
			SetCharCheckOut();
	}
	// �̹� �ٸ� Active Edit�� �ִٸ� 
	else 
	{
		m_pActiveEdit->m_bActiveEdit	=	FALSE	;
		m_pActiveEdit->WindowControlMessageProc( AgcWindow::MESSAGE_EDIT_DEACTIVE );

		m_pActiveEdit					=	pEdit	;
		m_pActiveEdit->m_bActiveEdit	=	TRUE	;
		m_pActiveEdit->WindowControlMessageProc( AgcWindow::MESSAGE_EDIT_ACTIVE );

		if( !g_pEngine->GetCharCheckState() )
			SetCharCheckOut();
	}

	return FALSE;
}

/*****************************************************************
*   Function : ReleaseMeActiveEdit
*   Comment  : pbActive�� ������ �ִ� Edit�� Active���� �����Ѵ�
*   Date&Time : 2003-07-07, ���� 5:05
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
BOOL	AgcEngine::ReleaseMeActiveEdit( AgcWindow*	pEdit )
{
	if (pEdit)
	{
		pEdit->m_bActiveEdit	=	FALSE	;

		// ���� Active�� Edit�ݱ�
		if ( m_pActiveEdit == pEdit )
		{
			m_pActiveEdit->WindowControlMessageProc( AgcWindow::MESSAGE_EDIT_DEACTIVE );
			m_pActiveEdit	=		NULL	;
				
			if( g_pEngine->GetCharCheckState() )
				SetCharCheckIn();
		}
	}
	else
	{
		if (m_pActiveEdit)
		{
			m_pActiveEdit->m_bActiveEdit	=	FALSE	;
			m_pActiveEdit->WindowControlMessageProc( AgcWindow::MESSAGE_EDIT_DEACTIVE );
			m_pActiveEdit = NULL;
		}

		if( g_pEngine->GetCharCheckState() )
			SetCharCheckIn();
	}

	return TRUE;
}

/*****************************************************************
*   Function : SetSoundAlertMSGFile
*   Comment  : Alert MSG���� �̸��� Setting�Ѵ�
*   Date&Time : 2003-07-11, ���� 6:43
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void	AgcEngine::SetSoundAlertMSGFile( char* pszFileName )
{
	strncpy( m_szSoundAlertMSG, pszFileName, 256 );
}

void	AgcEngine::SetWorldCameraFarClipPlane(float fFar)
{
	LockFrame();

	RwCameraSetFarClipPlane (m_pCamera,fFar);	

	OnCameraStateChange(CSC_FAR);

	UnlockFrame();
}

void	AgcEngine::SetWorldCameraNearClipPlane(float fNear)
{
	LockFrame();

	RwCameraSetNearClipPlane (m_pCamera,fNear);

//	RwCameraSetFogDistance (m_pCamera,fNear);

	OnCameraStateChange(CSC_NEAR);

	UnlockFrame();
}

void	AgcEngine::SetWorldCameraViewWindow	(int w,int h)
{
	OnCameraStateChange(CSC_RESIZE);
}

void	AgcEngine::OnCameraStateChange(CAMERASTATECHANGETYPE	type)
{
	for(int i=0;i<4;++i)
	{
		m_vBoxFan[i].z = 0.0f;
		m_vBoxFan[i].rhw = 1.0f;
	}
	
	ApModuleList *		pList = ApModuleManager::m_listModule;
	while(pList)
	{
		if(pList->pModule->GetModuleType() == APMODULE_TYPE_CLIENT )
			((AgcModule*)(pList->pModule))->OnCameraStateChange(type);

		pList = pList->next;
	}
}

BOOL	AgcEngine::DrawRwTexture( RwTexture* pTexture, INT32 nX, INT32 nY, DWORD dwColor )
{
	if( !pTexture )		return FALSE;

	SIZE size = { RwRasterGetWidth( pTexture->raster ), RwRasterGetHeight( pTexture->raster ) };

	m_vBoxFan[0].x = (float)nX - 0.5f;
    m_vBoxFan[0].y = (float)nY - 0.5f;
    m_vBoxFan[0].u = 0.f;
	m_vBoxFan[0].v = 0.f;
	m_vBoxFan[0].color = dwColor;

	m_vBoxFan[1].x = (float)(nX + size.cx) - 0.5f;
    m_vBoxFan[1].y = (float)nY - 0.5f;
    m_vBoxFan[1].u = 1.f;
	m_vBoxFan[1].v = 0.f;
	m_vBoxFan[1].color = dwColor;

	m_vBoxFan[2].x = (float)(nX + size.cx) - 0.5f;
    m_vBoxFan[2].y = (float)(nY + size.cy) - 0.5f;
    m_vBoxFan[2].u = 1.f;
	m_vBoxFan[2].v = 1.f;
	m_vBoxFan[2].color = dwColor;

	m_vBoxFan[3].x = (float)nX - 0.5f;
    m_vBoxFan[3].y = (float)(nY + size.cy) - 0.5f;
    m_vBoxFan[3].u = 0.f;
	m_vBoxFan[3].v = 1.f;
	m_vBoxFan[3].color = dwColor;
	
	RwD3D9SetTexture( pTexture, 0);
	
	RwD3D9SetFVF( MY2D_VERTEX_FLAG );
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_vBoxFan, SIZE_MY2D_VERTEX );

	return TRUE;
}

BOOL AgcEngine::Draw2DPanel( INT32 nX, INT32 nY, INT32 nWidth, INT32 nHeight, DWORD dwColor, LPDIRECT3DTEXTURE9 pTex /*= NULL */ )
{
	m_vBoxFan[0].x = (float)nX - 0.5f;
	m_vBoxFan[0].y = (float)nY - 0.5f;
	m_vBoxFan[0].z = 0.0f;
	m_vBoxFan[0].u = 0.f;
	m_vBoxFan[0].v = 0.f;
	m_vBoxFan[0].color = dwColor;

	m_vBoxFan[1].x = (float)(nX + nWidth) - 0.5f;
	m_vBoxFan[1].y = (float)nY - 0.5f;
	m_vBoxFan[1].z = 0.0f;
	m_vBoxFan[1].u = 1.f;
	m_vBoxFan[1].v = 0.f;
	m_vBoxFan[1].color = dwColor;

	m_vBoxFan[2].x = (float)(nX + nWidth) - 0.5f;
	m_vBoxFan[2].y = (float)(nY + nHeight) - 0.5f;
	m_vBoxFan[2].z = 0.0f;
	m_vBoxFan[2].u = 1.f;
	m_vBoxFan[2].v = 1.f;
	m_vBoxFan[2].color = dwColor;

	m_vBoxFan[3].x = (float)nX - 0.5f;
	m_vBoxFan[3].y = (float)(nY + nHeight) - 0.5f;
	m_vBoxFan[3].z = 0.0f;
	m_vBoxFan[3].u = 0.f;
	m_vBoxFan[3].v = 1.f;
	m_vBoxFan[3].color = dwColor;

	UINT		unColorOP		=	0;
	UINT		unColorArg1		=	0;
	UINT		unAlphaOP		=	0;
	UINT		unAlphaArg1		=	0;

	UINT		unSrcBlend		=	0;
	UINT		unDestBlend		=	0;
	UINT		unAlphaEnable	=	0;

	RwD3D9GetTextureStageState( 0 , D3DTSS_COLOROP		, &unColorOP	);
	RwD3D9GetTextureStageState( 0 , D3DTSS_COLORARG1	, &unColorArg1	);
	RwD3D9GetTextureStageState( 0 , D3DTSS_ALPHAOP		, &unAlphaOP	);
	RwD3D9GetTextureStageState( 0 , D3DTSS_ALPHAARG1	, &unAlphaArg1	);

	RwD3D9GetRenderState( D3DRS_SRCBLEND , &unSrcBlend );
	RwD3D9GetRenderState( D3DRS_DESTBLEND , &unDestBlend );

	RwD3D9GetRenderState( D3DRS_ALPHABLENDENABLE , &unAlphaEnable );

	if( NULL == pTex )
	{
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );

		RwD3D9SetTextureStageState( 0 , D3DTSS_COLOROP		, D3DTOP_SELECTARG1 );
		RwD3D9SetTextureStageState( 0 , D3DTSS_COLORARG1	, D3DTA_DIFFUSE );
		RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAOP		, D3DTOP_SELECTARG1 );
		RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAARG1	, D3DTA_DIFFUSE );
	}
	else
	{
		RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
		RwD3D9SetTextureStageState( 0 , D3DTSS_COLOROP		, D3DTOP_SELECTARG1 );
		RwD3D9SetTextureStageState( 0 , D3DTSS_COLORARG1	, D3DTA_TEXTURE );

		RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAOP		, D3DTOP_SELECTARG1 );
		RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAARG1	, D3DTA_TEXTURE );

		RwD3D9SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RwD3D9SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
	}

	((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice())->SetTexture( 0, pTex );

	RwD3D9SetFVF( MY2D_VERTEX_FLAG );
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, m_vBoxFan, sizeof(My2DVertex) );

	RwD3D9SetTextureStageState( 0 , D3DTSS_COLOROP		, unColorOP		);
	RwD3D9SetTextureStageState( 0 , D3DTSS_COLORARG1	, unColorArg1	);
	RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAOP		, unAlphaOP		);
	RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAARG1	, unAlphaArg1	);

	RwD3D9SetRenderState( D3DRS_SRCBLEND , unSrcBlend );
	RwD3D9SetRenderState( D3DRS_DESTBLEND , unDestBlend );
	RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE , unAlphaEnable );

	RwD3D9SetTexture( NULL , 0 );

	return TRUE;
}

bool	AgcEngine::DrawIm2D(RwTexture* pTex,float x,float y,float w,float h,
							float st_u, float st_v, float off_u, float off_v,
							DWORD color,UINT8	alpha)
{
	PROFILE("AgcEngine::DrawIm2D");

	DWORD	mycolor = ( color & 0x00ffffff ) | (alpha << 24);

	//@{ Jaewon 20050811
	// Half texel offset for the correct texel-to-pixel matching in D3D.
	
	//@{ 2006/05/17 burumal
	/*
	st_u += 0.5f/w;
	st_v += 0.5f/h;
	off_u += 0.5f/w;
	off_v += 0.5f/h;
	*/
	//@}

	//@} Jaewon

	m_vBoxFan[0].color = mycolor;
	m_vBoxFan[1].color = mycolor;
	m_vBoxFan[2].color = mycolor;
	m_vBoxFan[3].color = mycolor;	

	//@{ 2006/05/17 burumal	
	
	/*
	m_vBoxFan[0].x = x;
	m_vBoxFan[0].y = y;
	m_vBoxFan[0].u = st_u;
	m_vBoxFan[0].v = st_v;
	
	m_vBoxFan[1].x = x+w;
	m_vBoxFan[1].y = y; 
	m_vBoxFan[1].u = off_u;
	m_vBoxFan[1].v = st_v;
	
	m_vBoxFan[2].x = x+w;
	m_vBoxFan[2].y = y+h;
	m_vBoxFan[2].u = off_u;
	m_vBoxFan[2].v = off_v;
	
	m_vBoxFan[3].x = x;
	m_vBoxFan[3].y = y+h; 
	m_vBoxFan[3].u = st_u;
	m_vBoxFan[3].v = off_v;
	*/

	#define fD3D_PIXEL_CORRECTION_OFFSET	0.5f

	m_vBoxFan[0].x = x - fD3D_PIXEL_CORRECTION_OFFSET;
    m_vBoxFan[0].y = y - fD3D_PIXEL_CORRECTION_OFFSET;	
    m_vBoxFan[0].u = st_u;
	m_vBoxFan[0].v = st_v;

	m_vBoxFan[1].x = x+w - fD3D_PIXEL_CORRECTION_OFFSET;
    m_vBoxFan[1].y = y - fD3D_PIXEL_CORRECTION_OFFSET;	
    m_vBoxFan[1].u = off_u;
	m_vBoxFan[1].v = st_v;

	m_vBoxFan[2].x = x+w - fD3D_PIXEL_CORRECTION_OFFSET;
    m_vBoxFan[2].y = y+h - fD3D_PIXEL_CORRECTION_OFFSET; 	
    m_vBoxFan[2].u = off_u;
	m_vBoxFan[2].v = off_v;

	m_vBoxFan[3].x = x - fD3D_PIXEL_CORRECTION_OFFSET;
    m_vBoxFan[3].y = y+h - fD3D_PIXEL_CORRECTION_OFFSET; 	
    m_vBoxFan[3].u = st_u;
	m_vBoxFan[3].v = off_v;
	//@}
	
	RwD3D9SetTexture (pTex,0);
	
	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vBoxFan , SIZE_MY2D_VERTEX );

	return true;
}

inline void __Rotate2DVector( My2DVertex * pVector , FLOAT kX , FLOAT kY , FLOAT fDegree )
{
	pVector->x	-= kX;
	pVector->y	-= kY;

	FLOAT	radian = fDegree / 180.0f * 3.1415927f;

	FLOAT	fNewX , fNewY;

	fNewX	= ( FLOAT ) cos( radian ) * pVector->x - ( FLOAT ) sin( radian ) * pVector->y + kX;
	fNewY	= ( FLOAT ) sin( radian ) * pVector->x + ( FLOAT ) cos( radian ) * pVector->y + kY;

	pVector->x	= fNewX;
	pVector->y	= fNewY;
}

bool		AgcEngine::DrawIm2DRotate( RwTexture * pTexture ,
									  float fX		, float fY		,
									  float fWidth	, float fHeight	,
									  float fUStart , float fVStart ,
									  float fUEnd	, float fVEnd	,
									  float fCenterX , float fCenterY , float fDegree,
									  DWORD uColor	, UINT8	uAlpha	)
{
	DWORD	mycolor = ( uColor & 0x00ffffff ) | ( uAlpha << 24 );

	m_vBoxFan[0].color = mycolor;
	m_vBoxFan[1].color = mycolor;
	m_vBoxFan[2].color = mycolor;
	m_vBoxFan[3].color = mycolor;
	
	m_vBoxFan[0].x = fX;
    m_vBoxFan[0].y = fY; 
    m_vBoxFan[0].u = fUStart;
	m_vBoxFan[0].v = fVStart;

	m_vBoxFan[1].x = fX + fWidth;
    m_vBoxFan[1].y = fY; 
    m_vBoxFan[1].u = fUEnd;
	m_vBoxFan[1].v = fVStart;

	m_vBoxFan[2].x = fX + fWidth;
    m_vBoxFan[2].y = fY + fHeight; 
    m_vBoxFan[2].u = fUEnd;
	m_vBoxFan[2].v = fVEnd;

	m_vBoxFan[3].x = fX;
    m_vBoxFan[3].y = fY + fHeight; 
    m_vBoxFan[3].u = fUStart;
	m_vBoxFan[3].v = fVEnd;
	
	for(int i = 0 ; i < 4 ; ++ i )
	{
		__Rotate2DVector( &m_vBoxFan[ i ] , fCenterX , fCenterY , fDegree );
	}	

	RwRenderStateSet		( rwRENDERSTATETEXTURERASTER ,
								( void * ) pTexture ? RwTextureGetRaster(pTexture) : NULL);
			
	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vBoxFan , SIZE_MY2D_VERTEX );

	return true;
}


void	AgcEngine::GetWorldPosToScreenPos		( RwV3d * pWorldPos ,RwV3d* pCameraPos, RwV3d* pScreenPos , float* recip_z)
{
	RwMatrix *viewMatrix = RwCameraGetViewMatrix(m_pCamera);
	RwRaster*	raster = RwCameraGetRaster( m_pCamera );
	RwV3dTransformPoints(pCameraPos, pWorldPos, 1, viewMatrix);
	
	float recipZ = 1.0f/pCameraPos->z;

	pScreenPos->x = pCameraPos->x * recipZ * RwRasterGetWidth( raster ) + m_pCamera->viewOffset.x;
	pScreenPos->y = pCameraPos->y * recipZ * RwRasterGetHeight( raster ) + m_pCamera->viewOffset.y;
	pScreenPos->z = m_pCamera->zScale * recipZ + m_pCamera->zShift;

	*recip_z = recipZ;
}

bool	AgcEngine::DrawIm2DInWorld(RwTexture* pTex,RwV3d* pWPos,RwV2d* offset,float w,float h,
								float st_u, float st_v, float off_u, float off_v,
								DWORD color,UINT8	alpha)
{
	RwV3d	CamPos;
	RwV3d	ScreenPos;
	float	RecipZ;

	GetWorldPosToScreenPos(pWPos,&CamPos,&ScreenPos,&RecipZ);

	return DrawIm2DInWorld(pTex, &CamPos, &ScreenPos, RecipZ, offset, w, h, st_u, st_v, off_u, off_v, color, alpha);
}

bool	AgcEngine::DrawIm2DInWorld(RwTexture* pTex, RwV3d* pCamPos, RwV3d* pScreenPos, float recipZ, RwV2d* offset, float w,float h,
								float st_u, float st_v, float off_u, float off_v,
								DWORD color,UINT8	alpha)
{
	UINT32	red = (color & 0x00ff0000) >> 16;
	UINT32	green = (color & 0x0000ff00) >> 8;
	UINT32	blue = color & 0x000000ff;

	RwIm2DVertex	BoxFan[4];

	int i;
	for(i=0;i<4;++i)
		RwIm2DVertexSetIntRGBA(&BoxFan[i], red, green, blue, alpha);

	for(i=0;i<4;++i)
	{
		RwIm2DVertexSetCameraX(&BoxFan[i],CamPos.x);
		RwIm2DVertexSetCameraY(&BoxFan[i],CamPos.y);
		RwIm2DVertexSetCameraZ(&BoxFan[i],CamPos.z);

		RwIm2DVertexSetRecipCameraZ(&BoxFan[i], recipZ);
		RwIm2DVertexSetScreenZ(&BoxFan[i],pScreenPos->z);
	}

	float stx = pScreenPos->x + offset->x;
	float sty = pScreenPos->y + offset->y;

	RwIm2DVertexSetScreenX(&BoxFan[0],stx);
    RwIm2DVertexSetScreenY(&BoxFan[0],sty); 
    RwIm2DVertexSetU(&BoxFan[0], st_u, recipZ);
    RwIm2DVertexSetV(&BoxFan[0], st_v, recipZ);
	
	
	RwIm2DVertexSetScreenX(&BoxFan[1],stx+w);
    RwIm2DVertexSetScreenY(&BoxFan[1],sty); 
    RwIm2DVertexSetU(&BoxFan[1], st_u + off_u, recipZ);
    RwIm2DVertexSetV(&BoxFan[1], st_v, recipZ);
	
	RwIm2DVertexSetScreenX(&BoxFan[2],stx+w);
    RwIm2DVertexSetScreenY(&BoxFan[2],sty+h); 
    RwIm2DVertexSetU(&BoxFan[2], st_u + off_u , recipZ);
    RwIm2DVertexSetV(&BoxFan[2], st_v + off_v , recipZ);
	
	RwIm2DVertexSetScreenX(&BoxFan[3],stx);
    RwIm2DVertexSetScreenY(&BoxFan[3],sty+h); 
    RwIm2DVertexSetU(&BoxFan[3], st_u, recipZ);
    RwIm2DVertexSetV(&BoxFan[3], st_v + off_v, recipZ);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)RwTextureGetRaster(pTex));
			
	RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, BoxFan, 4);

	return true;
}

bool	AgcEngine::DrawIm2DInWorld2(Im2DScreenData* pData,RwTexture* pTex,RwV3d* pWPos,RwV2d* offset,float w,float h,
								float st_u, float st_v, float off_u, float off_v,
								DWORD color,UINT8	alpha)
{
	RwV3d	CamPos;
	
	GetWorldPosToScreenPos(pWPos,&CamPos,&pData->Screen_Pos,&pData->Recip_Z);

	UINT32	red = (color & 0x00ff0000) >> 16;
	UINT32	green = (color & 0x0000ff00) >> 8;
	UINT32	blue = color & 0x000000ff;
	INT32	i;

	RwIm2DVertex	BoxFan[4];

	for(i=0;i<4;++i)
		RwIm2DVertexSetIntRGBA(&BoxFan[i], red, green, blue, alpha);

	for(i=0;i<4;++i)
	{
		RwIm2DVertexSetCameraX(&BoxFan[i],CamPos.x);
		RwIm2DVertexSetCameraY(&BoxFan[i],CamPos.y);
		RwIm2DVertexSetCameraZ(&BoxFan[i],CamPos.z);

		RwIm2DVertexSetRecipCameraZ(&BoxFan[i], pData->Recip_Z);
		RwIm2DVertexSetScreenZ(&BoxFan[i],pData->Screen_Pos.z);
	}

	float stx = pData->Screen_Pos.x + offset->x;
	float sty = pData->Screen_Pos.y + offset->y;

	RwIm2DVertexSetScreenX(&BoxFan[0],stx);
    RwIm2DVertexSetScreenY(&BoxFan[0],sty); 
    RwIm2DVertexSetU(&BoxFan[0], st_u, pData->Recip_Z);
    RwIm2DVertexSetV(&BoxFan[0], st_v, pData->Recip_Z);
	
	RwIm2DVertexSetScreenX(&BoxFan[1],stx+w);
    RwIm2DVertexSetScreenY(&BoxFan[1],sty); 
    RwIm2DVertexSetU(&BoxFan[1], st_u + off_u, pData->Recip_Z);
    RwIm2DVertexSetV(&BoxFan[1], st_v, pData->Recip_Z);
	
	RwIm2DVertexSetScreenX(&BoxFan[2],stx+w);
    RwIm2DVertexSetScreenY(&BoxFan[2],sty+h); 
    RwIm2DVertexSetU(&BoxFan[2], st_u + off_u , pData->Recip_Z);
    RwIm2DVertexSetV(&BoxFan[2], st_v + off_v , pData->Recip_Z);
	
	RwIm2DVertexSetScreenX(&BoxFan[3],stx);
    RwIm2DVertexSetScreenY(&BoxFan[3],sty+h); 
    RwIm2DVertexSetU(&BoxFan[3], st_u, pData->Recip_Z);
    RwIm2DVertexSetV(&BoxFan[3], st_v + off_v, pData->Recip_Z);

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER,(void *)RwTextureGetRaster(pTex));
			
	RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, BoxFan, 4);

	return true;
}

/*****************************************************************
*   Function : SetCallbackSetCursorMousePosition
*   Comment  : Cursor Mouse Position Set Call back funciton
*   Date&Time : 2003-11-18, ���� 8:56
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void	AgcEngine::SetCallbackSetCursorMousePosition( AgcEngineDefaultCallBack pfCallback, PVOID pClass )
{
	m_pSetCursorPositionCallback		= pfCallback	;
	m_pSetCursorPositionCallbackClass	= pClass		;
}

BOOL	AgcEngine::SetWideScreen			( BOOL bWide )
{
	// ���̵� ��ũ�� ���� ����..
	m_bWideScreenMode = bWide;

	SetProjection();
	return TRUE;
}

// �������� �����Լ�.
BOOL	AgcEngine::SetProjection			( FLOAT fRate			)
{
	// Prespective 
	RwV2d	ViewWindow	;

	ViewWindow.x	= fRate								;

	RwInt32	nCurrentVideoMode = RwEngineGetCurrentVideoMode();
	RwVideoMode videoMode;
	RwEngineGetVideoModeInfo(&videoMode, nCurrentVideoMode);

#ifdef _AREA_CHINA_
	if( videoMode.flags & rwVIDEOMODEEXCLUSIVE )
	{
		// ��üȭ�� ����̸�..
		if( m_bWideScreenMode )
		{
			ViewWindow.y	= ViewWindow.x / DEFAULT_ASPECTRATIO_WIDE;
		}
		else
		{
			ViewWindow.y	= ViewWindow.x / DEFAULT_ASPECTRATIO_NORMAL;
		}
	}
	else
#endif
	{
		// â����̸�..
		FLOAT	fRatio = ( FLOAT ) videoMode.width / ( FLOAT ) videoMode.height;
		RwRaster*				pRas = RwCameraGetRaster(m_pCamera);

		if( pRas )
		{
			fRatio = ( FLOAT ) RwRasterGetWidth( pRas ) / ( FLOAT ) RwRasterGetHeight( pRas );
		}

		ViewWindow.y	= ViewWindow.x / fRatio;
	}

	LockFrame();
	RwCameraSetViewWindow( g_pEngine->m_pCamera , &ViewWindow );
	// ������ (2004-01-15 ���� 2:44:47) : ���� ����ó�� ���� ����..;

	UnlockFrame();

	m_fCameraProjection = fRate;

	return TRUE;
}

BOOL		AgcEngine::DrawIm2DPixel(
					RwTexture * pTexture						,
					INT32 nX				, INT32 nY			,
					INT32 nWidth			, INT32 nHeight		,
					INT32 nUStart			, INT32 nVStart		,
					INT32 nUEnd				, INT32 nVEnd		,
					DWORD uColor								,
					UINT8 uAlpha								)
{
	PROFILE("AgcEngine::DrawIm2DPixel");

	if ( !pTexture || !RwTextureGetRaster( pTexture ) )
		return FALSE;

	INT32	nImageWidth		= RwRasterGetWidth	( RwTextureGetRaster( pTexture ) );
	INT32	nImageHeight	= RwRasterGetHeight	( RwTextureGetRaster( pTexture ) );

	if( -1 == nWidth	) nWidth	= nImageWidth	;
	if( -1 == nHeight	) nHeight	= nImageHeight	;

	if( -1 == nUEnd		) nUEnd		= nWidth	;
	if( -1 == nVEnd		) nVEnd		= nHeight	;

	// ������ (2004-05-26 ���� 6:35:56) : �̰� ����� ����� ���´�.
	nUEnd ++;
	nVEnd ++;

	FLOAT fUStart	= ( FLOAT ) nUStart / ( FLOAT ) nImageWidth		;
	FLOAT fVStart	= ( FLOAT ) nVStart / ( FLOAT ) nImageHeight	;
	FLOAT fUEnd		= ( FLOAT ) nUEnd	/ ( FLOAT ) nImageWidth		;
	FLOAT fVEnd		= ( FLOAT ) nVEnd	/ ( FLOAT ) nImageHeight	;

	UINT32	uRed	= ( uColor & 0x00ff0000	) >> 16	;
	UINT32	uGreen	= ( uColor & 0x0000ff00	) >> 8	;
	UINT32	uBlue	= ( uColor & 0x000000ff	)		;

	DWORD	mycolor = ( uColor & 0x00ffffff ) | ( uAlpha << 24 );
	m_vBoxFan[0].color = mycolor;
	m_vBoxFan[1].color = mycolor;
	m_vBoxFan[2].color = mycolor;
	m_vBoxFan[3].color = mycolor;

	#define fD3D_PIXEL_CORRECTION_OFFSET	0.5f

	m_vBoxFan[0].x = ( float ) ( nX + nWidth - fD3D_PIXEL_CORRECTION_OFFSET	);
    m_vBoxFan[0].y = ( float ) ( nY + nHeight - fD3D_PIXEL_CORRECTION_OFFSET ); 
    m_vBoxFan[0].u = fUEnd						;
	m_vBoxFan[0].v = fVEnd						;

	m_vBoxFan[1].x = ( float ) ( nX + nWidth - fD3D_PIXEL_CORRECTION_OFFSET );
    m_vBoxFan[1].y = ( float ) ( nY	- fD3D_PIXEL_CORRECTION_OFFSET );
    m_vBoxFan[1].u = fUEnd						;
	m_vBoxFan[1].v = fVStart					;	

	m_vBoxFan[2].x = ( float ) ( nX	- fD3D_PIXEL_CORRECTION_OFFSET );
    m_vBoxFan[2].y = ( float ) ( nY + nHeight - fD3D_PIXEL_CORRECTION_OFFSET ); 
    m_vBoxFan[2].u = fUStart					;
	m_vBoxFan[2].v = fVEnd						;

	m_vBoxFan[3].x = ( float ) ( nX	- fD3D_PIXEL_CORRECTION_OFFSET );
    m_vBoxFan[3].y = ( float ) ( nY - fD3D_PIXEL_CORRECTION_OFFSET );
    m_vBoxFan[3].u = fUStart					;
	m_vBoxFan[3].v = fVStart					;
	
	RwD3D9SetTexture (pTexture,0);

	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	//RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE , FALSE );
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , m_vBoxFan , SIZE_MY2D_VERTEX );
	//RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );

	return TRUE;
}

BOOL		AgcEngine::DrawIm2DPixelMask(
	RwTexture * pTexture , RwTexture* pMask , 
	INT32 nX				, INT32 nY			,
	INT32 nWidth			, INT32 nHeight		,
	INT32 nUStart			, INT32 nVStart		,
	INT32 nUEnd				, INT32 nVEnd	,
	DWORD uColor								,
	UINT8 uAlpha								)
{
	if ( !pTexture || !RwTextureGetRaster( pTexture ) )
		return FALSE;

	INT32	nImageWidth		= RwRasterGetWidth	( RwTextureGetRaster( pTexture ) );
	INT32	nImageHeight	= RwRasterGetHeight	( RwTextureGetRaster( pTexture ) );

	if( -1 == nWidth	) nWidth	= nImageWidth	;
	if( -1 == nHeight	) nHeight	= nImageHeight	;

	if( -1 == nUEnd		) nUEnd		= nWidth	;
	if( -1 == nVEnd		) nVEnd		= nHeight	;

	// ������ (2004-05-26 ���� 6:35:56) : �̰� ����� ����� ���´�.
	nUEnd ++;
	nVEnd ++;

	FLOAT fUStart	= ( FLOAT ) nUStart / ( FLOAT ) nImageWidth		;
	FLOAT fVStart	= ( FLOAT ) nVStart / ( FLOAT ) nImageHeight	;
	FLOAT fUEnd		= ( FLOAT ) nUEnd	/ ( FLOAT ) nImageWidth		;
	FLOAT fVEnd		= ( FLOAT ) nVEnd	/ ( FLOAT ) nImageHeight	;

	UINT32	uRed	= ( uColor & 0x00ff0000	) >> 16	;
	UINT32	uGreen	= ( uColor & 0x0000ff00	) >> 8	;
	UINT32	uBlue	= ( uColor & 0x000000ff	)		;

	DWORD	mycolor = ( uColor & 0x00ffffff ) | ( uAlpha << 24 );
	m_vBoxFan[0].color = mycolor;
	m_vBoxFan[1].color = mycolor;
	m_vBoxFan[2].color = mycolor;
	m_vBoxFan[3].color = mycolor;

#define fD3D_PIXEL_CORRECTION_OFFSET	0.5f

	m_vBoxFan[0].x = ( float ) ( nX + nWidth - fD3D_PIXEL_CORRECTION_OFFSET	);
	m_vBoxFan[0].y = ( float ) ( nY + nHeight - fD3D_PIXEL_CORRECTION_OFFSET ); 
	m_vBoxFan[0].u = fUEnd						;
	m_vBoxFan[0].v = fVEnd						;

	m_vBoxFan[1].x = ( float ) ( nX + nWidth - fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[1].y = ( float ) ( nY	- fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[1].u = fUEnd						;
	m_vBoxFan[1].v = fVStart					;	

	m_vBoxFan[2].x = ( float ) ( nX	- fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[2].y = ( float ) ( nY + nHeight - fD3D_PIXEL_CORRECTION_OFFSET ); 
	m_vBoxFan[2].u = fUStart					;
	m_vBoxFan[2].v = fVEnd						;

	m_vBoxFan[3].x = ( float ) ( nX	- fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[3].y = ( float ) ( nY - fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[3].u = fUStart					;
	m_vBoxFan[3].v = fVStart					;

	RwD3D9SetTexture( pTexture	, 0 );
	RwD3D9SetTexture( pMask		, 1 );

	UINT		OneColopOP;
	UINT		OneColorArg1;
	UINT		OneAlphaOP;
	UINT		OneAhphaArg1;

	UINT		TwoTexCoordIndex;
	UINT		TwoColorOP;
	UINT		TwoColorArg1;
	UINT		TwoAhphaOP;
	UINT		TwoAhphaArg1;

	RwD3D9GetTextureStageState( 0 , D3DTSS_COLOROP , &OneColopOP );
	RwD3D9GetTextureStageState( 0 , D3DTSS_COLORARG1 , &OneColorArg1  );
	RwD3D9GetTextureStageState( 0 , D3DTSS_ALPHAOP , &OneAlphaOP  );
	RwD3D9GetTextureStageState( 0 , D3DTSS_ALPHAARG1 , &OneAhphaArg1  );

	RwD3D9GetTextureStageState( 1 , D3DTSS_TEXCOORDINDEX, &TwoTexCoordIndex  ); 
	RwD3D9GetTextureStageState( 1 , D3DTSS_COLOROP , &TwoColorOP  );
	RwD3D9GetTextureStageState( 1 , D3DTSS_COLORARG1 , &TwoColorArg1  );
	RwD3D9GetTextureStageState( 1 , D3DTSS_ALPHAOP , &TwoAhphaOP  );
	RwD3D9GetTextureStageState( 1 , D3DTSS_ALPHAARG1 , &TwoAhphaArg1  );

	RwD3D9SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_SELECTARG1 );
	RwD3D9SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE );
	RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );
	RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );

	RwD3D9SetTextureStageState( 1 , D3DTSS_TEXCOORDINDEX, 0 ); 
	RwD3D9SetTextureStageState( 1 , D3DTSS_COLOROP , D3DTOP_SELECTARG1 );
	RwD3D9SetTextureStageState( 1 , D3DTSS_COLORARG1 , D3DTA_CURRENT );
	RwD3D9SetTextureStageState( 1 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );
	RwD3D9SetTextureStageState( 1 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );

	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,
		(void *) rwTEXTUREADDRESSCLAMP);

	RwRenderStateSet(rwRENDERSTATESHADEMODE,
		(void *) rwSHADEMODEGOURAUD);

	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , m_vBoxFan , SIZE_MY2D_VERTEX );

	RwD3D9SetTextureStageState( 0 , D3DTSS_COLOROP , OneColopOP );
	RwD3D9SetTextureStageState( 0 , D3DTSS_COLORARG1 , OneColorArg1 );
	RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAOP , OneAlphaOP );
	RwD3D9SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , OneAhphaArg1 );

	RwD3D9SetTextureStageState( 1 , D3DTSS_TEXCOORDINDEX, TwoTexCoordIndex ); 
	RwD3D9SetTextureStageState( 1 , D3DTSS_COLOROP , TwoColorOP );
	RwD3D9SetTextureStageState( 1 , D3DTSS_COLORARG1 , TwoColorArg1 );
	RwD3D9SetTextureStageState( 1 , D3DTSS_ALPHAOP , TwoAhphaOP );
	RwD3D9SetTextureStageState( 1 , D3DTSS_ALPHAARG1 , TwoAhphaArg1 );

	RwD3D9SetTexture( NULL , 0  );
	RwD3D9SetTexture( NULL , 1	);

	return TRUE;
}



BOOL		AgcEngine::DrawIm2DPixelRaster(
									 RwRaster*	pRaster , RwTexture * pTexture	,
									 INT32 nX				, INT32 nY			,
									 INT32 nWidth			, INT32 nHeight		,
									 INT32 nUStart			, INT32 nVStart		,
									 INT32 nUEnd				, INT32 nVEnd	,
									 DWORD uColor								,
									 UINT8 uAlpha								)
{
	PROFILE("AgcEngine::DrawIm2DPixelRaster");

	if ( !pTexture || !RwTextureGetRaster( pTexture ) )
		return FALSE;

	INT32	nImageWidth		= RwRasterGetWidth	( RwTextureGetRaster( pTexture ) );
	INT32	nImageHeight	= RwRasterGetHeight	( RwTextureGetRaster( pTexture ) );

	if( -1 == nWidth	) nWidth	= nImageWidth	;
	if( -1 == nHeight	) nHeight	= nImageHeight	;

	if( -1 == nUEnd		) nUEnd		= nWidth	;
	if( -1 == nVEnd		) nVEnd		= nHeight	;

	// ������ (2004-05-26 ���� 6:35:56) : �̰� ����� ����� ���´�.
	nUEnd ++;
	nVEnd ++;

	FLOAT fUStart	= ( FLOAT ) nUStart / ( FLOAT ) nImageWidth		;
	FLOAT fVStart	= ( FLOAT ) nVStart / ( FLOAT ) nImageHeight	;
	FLOAT fUEnd		= ( FLOAT ) nUEnd	/ ( FLOAT ) nImageWidth		;
	FLOAT fVEnd		= ( FLOAT ) nVEnd	/ ( FLOAT ) nImageHeight	;

	UINT32	uRed	= ( uColor & 0x00ff0000	) >> 16	;
	UINT32	uGreen	= ( uColor & 0x0000ff00	) >> 8	;
	UINT32	uBlue	= ( uColor & 0x000000ff	)		;

	DWORD	mycolor = ( uColor & 0x00ffffff ) | ( uAlpha << 24 );
	m_vBoxFan[0].color = mycolor;
	m_vBoxFan[1].color = mycolor;
	m_vBoxFan[2].color = mycolor;
	m_vBoxFan[3].color = mycolor;

#define fD3D_PIXEL_CORRECTION_OFFSET	0.5f

	m_vBoxFan[0].x = ( float ) ( nX + nWidth - fD3D_PIXEL_CORRECTION_OFFSET	);
	m_vBoxFan[0].y = ( float ) ( nY + nHeight - fD3D_PIXEL_CORRECTION_OFFSET ); 
	m_vBoxFan[0].u = fUEnd						;
	m_vBoxFan[0].v = fVEnd						;

	m_vBoxFan[1].x = ( float ) ( nX + nWidth - fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[1].y = ( float ) ( nY	- fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[1].u = fUEnd						;
	m_vBoxFan[1].v = fVStart					;	

	m_vBoxFan[2].x = ( float ) ( nX	- fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[2].y = ( float ) ( nY + nHeight - fD3D_PIXEL_CORRECTION_OFFSET ); 
	m_vBoxFan[2].u = fUStart					;
	m_vBoxFan[2].v = fVEnd						;

	m_vBoxFan[3].x = ( float ) ( nX	- fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[3].y = ( float ) ( nY - fD3D_PIXEL_CORRECTION_OFFSET );
	m_vBoxFan[3].u = fUStart					;
	m_vBoxFan[3].v = fVStart					;

	RwD3D9SetTexture (pTexture,0);
	RwD3D9SetRenderTarget( 0 , pRaster );

	RwD3D9SetFVF(MY2D_VERTEX_FLAG);
	//RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE , FALSE );
	RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , m_vBoxFan , SIZE_MY2D_VERTEX );
	//RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );

	return TRUE;
}

BOOL	AgcEngine::OnSelectDevice()
{
	// ��ġ ��������..
	return FALSE; // ����Ʈ ���̽� ����Ʈ�� ȣ���� ( �˰��ִ� Dialog.. )
}

void	AgcEngine::ScreenShotSaveGDI()
{
	HWND	hWnd = GethWnd();
	RECT	rect;
	::GetClientRect( hWnd , &rect );

	#ifdef _DEBUG
	{
		char	str[ 256 ] = "";
		sprintf( str , "Width %d , Height %d\n" , rect.right , rect.bottom );
		OutputDebugString( str );
	}
	#endif

	HDC	hdc = ::GetDC( hWnd );

	CBmp	bmp;
	bmp.Create( rect.right - rect.left , rect.bottom - rect.top );
	::BitBlt( bmp.GetDC() , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdc , 0 , 0 , SRCCOPY );
	::ReleaseDC( hWnd , hdc );

	time_t timeval;
	(void)time(&timeval);
	struct tm* tm_ptr;
	tm_ptr = localtime(&timeval);

	char	szTimeBuf[40];

	strftime(szTimeBuf, 40, "%y%m%d-%H%M%S", tm_ptr);

	char		fstr[100];
	memset(fstr,'\0',100);

	// ���� ���� ���丮�� ������ ������.
	CreateDirectory( "Screen", NULL );
		
	sprintf(fstr,"screen\\SC%s.bmp",szTimeBuf);

	bmp.Save( fstr );
}

void	AgcEngine::ScreenShotSave()
{
	LPDIRECT3DDEVICE9       pd3dDevice = m_pCurD3D9Device;
	LPDIRECT3DSURFACE9		pBackBuf = (LPDIRECT3DSURFACE9)RwD3D9GetCurrentD3DRenderTarget(0) ;

	RwRaster*				pRas = RwCameraGetRaster(m_pCamera);
		
	//////////////////////////////////////////////////  ��� ����	
	D3DSURFACE_DESC desc;
	ZeroMemory(&desc,sizeof(desc));
	
	pBackBuf->GetDesc(&desc);

	D3DFORMAT format = desc.Format;

	int ScrW = desc.Width;
	int ScrH = desc.Height;

	int myw = ScrW;//1024;
	int myh = ScrH;//768;

	LPDIRECT3DSURFACE9 pResult;
	if(D3D_OK != pd3dDevice->CreateOffscreenPlainSurface(myw,myh,desc.Format,D3DPOOL_DEFAULT ,&pResult,NULL))
	{
		return;
	}

	D3DLOCKED_RECT pDstLock;
	if(D3D_OK != pResult->LockRect(&pDstLock,NULL,0))
	{
		pResult->Release();
		return;
	}

	int endW  = ScrW;
	int endH  = ScrH;

	int pixsize = 0;
	if(format == D3DFMT_A8R8G8B8 || format == D3DFMT_X8R8G8B8 )
	{
		pixsize = 4;
	}
	else if(format == D3DFMT_R8G8B8)
	{
		pixsize = 3;
	}
	else if(format == D3DFMT_R5G6B5 || format == D3DFMT_X1R5G5B5 || format == D3DFMT_A1R5G5B5)
	{
		pixsize = 2;
	}

	BYTE*  pSrc = (BYTE*) RwRasterLock(pRas,0,rwRASTERLOCKREAD);
	if(pSrc == NULL)
	{
		pResult->UnlockRect();
		pResult->Release();
		return;
	}

	BYTE*  pDst = (BYTE*) pDstLock.pBits;

	int		SrcPitch = RwRasterGetStride(pRas);
	int		DstPitch = pDstLock.Pitch;

	int offx = 0;
	int offy = 0;

	int dstoffx = 0;
	int dstoffy = 0;

	float stridex = (float)ScrW/(float)myw;
	float stridey = (float)ScrH/(float)myh;

	float nextx = 0;
	float nexty = 0;

	for(;offy <endH;offy++)
	{
		for(offx = 0;offx < endW ; offx++)
		{
			if(((offx >= nextx ) && (offy >= nexty)) )
			{
				for(int j=0;j<pixsize;j++)
				{
					if(dstoffx < myw && dstoffy < myh)
						pDst[DstPitch*dstoffy+dstoffx*pixsize+j] =pSrc[SrcPitch*offy+offx*pixsize+j];
				}
				++dstoffx;
				nextx += stridex;
			}
		}
		dstoffx = 0;
		nextx = 0;

		if((offy > nexty) )
		{
			nexty += stridey;
			++dstoffy;	
		}
	}
				
	RwRasterUnlock(pRas);
	pResult->UnlockRect();

	time_t timeval;
	(void)time(&timeval);
	struct tm* tm_ptr;
	tm_ptr = localtime(&timeval);

	char	szTimeBuf[40];

	strftime(szTimeBuf, 40, "%y%m%d-%H%M%S", tm_ptr);

	char		fstr[100];
	memset(fstr,'\0',100);
		
	sprintf(fstr,"screen\\SC%s.bmp",szTimeBuf);

	HANDLE fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(fd == INVALID_HANDLE_VALUE)
	{
		char	fdirc[100];
		memset(fdirc,'\0',100);
			 
		sprintf(fdirc,"screen");
		CreateDirectory(fdirc,NULL);

		fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	}

	SaveToBmp(fd,pResult);

	CloseHandle(fd);
				
	pResult->Release();
}

int		AgcEngine::SaveToBmp(HANDLE file_out,LPDIRECT3DSURFACE9 pSurf)
{
	//HANDLE file_out;
    DWORD numwrite;
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER bi;
    DWORD outpixel;
    int outbyte;
    BYTE * WriteBuffer; 
    int BufferIndex;
    int loop, loop2;
    int Width, Height, Pitch;
    WORD i, padding;
	
	// Make sure its not a NULL filename
	//if (strlen(szFilename) == 0)
	//	return -1;
	
	// Attempt to create the output
	//file_out = CreateFile(szFilename,
		//GENERIC_WRITE,
		//FILE_SHARE_WRITE,
		//NULL,
		//CREATE_ALWAYS,
		//FILE_ATTRIBUTE_NORMAL,
		//NULL);
	
	//if (file_out == INVALID_HANDLE_VALUE)
		//return -2;
	
    D3DLOCKED_RECT lrect;
	pSurf->LockRect(&lrect,NULL,0);

	D3DSURFACE_DESC desc;
	ZeroMemory(&desc,sizeof(desc));
	
	pSurf->GetDesc(&desc);
		
    // Setup output buffer stuff, since Windows has paging and we're in flat mode,
    // I just made it as big as the bitmap 
    BufferIndex = 0;
    Width       = desc.Width;
    Height      = desc.Height;
    Pitch       = lrect.Pitch;
    WriteBuffer = new BYTE[((Width * 3) + 3) * Height];
		
    // Write the file header
	((char *)&(fh.bfType))[0] = 'B';
	((char *)&(fh.bfType))[1] = 'M';
    fh.bfSize          = (long)(sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + Width*Height*3); // Size in BYTES
    fh.bfReserved1     = 0;
    fh.bfReserved2     = 0;
    fh.bfOffBits       = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
    bi.biSize          = sizeof(BITMAPINFOHEADER);
    bi.biWidth         = Width;
    bi.biHeight        = Height;
    bi.biPlanes        = 1;
    bi.biBitCount      = 24;
    bi.biCompression   = BI_RGB;
    bi.biSizeImage     = 0;
    bi.biXPelsPerMeter = 10000;
    bi.biYPelsPerMeter = 10000;
    bi.biClrUsed       = 0;
    bi.biClrImportant  = 0;
	
    WriteFile(file_out, (char *) &fh, sizeof(BITMAPFILEHEADER), &numwrite, NULL);
    WriteFile(file_out, (char *) &bi, sizeof(BITMAPINFOHEADER), &numwrite, NULL);
	
    // 32 bit surfaces
    if (desc.Format == D3DFMT_A8R8G8B8 || desc.Format == D3DFMT_X8R8G8B8)	
    {
        // lock the surface and start filling the output buffer
        BYTE* Bitmap_in = (BYTE*)lrect.pBits;
		
        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if (padding > 0)
            padding = 4 - padding;
		
        // Loop bottom up
        for (loop =Height - 1; loop >= 0; loop--)
        {
            for (loop2 = 0; loop2 < Width; loop2++)
            {
                // Load a WORD
                outpixel = *((DWORD *)(Bitmap_in + loop2*4 + loop * Pitch));
				
                // Load up the Blue component and output it
                outbyte = (((outpixel)&0x000000ff));
                WriteBuffer[BufferIndex++] = outbyte;
				
                // Load up the green component and output it 
                outbyte = (((outpixel >> 8)&0x000000ff)); 
                WriteBuffer[BufferIndex++] = outbyte;
				
                // Load up the red component and output it 
                outbyte = (((outpixel >> 16)&0x000000ff));
                WriteBuffer[BufferIndex++] = outbyte;
            }
			
            for (i = 0; i < padding; ++i)
                WriteBuffer[BufferIndex++] = (CHAR)0x00;
        }
	
        // At this point the buffer should be full, so just write it out
        WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);
    }
	
    // 24 bit surfaces
    else if (desc.Format == D3DFMT_R8G8B8)	
    {
        BYTE* Bitmap_in = (BYTE*)lrect.pBits;
		
        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if (padding > 0)
            padding = 4 - padding;
		
        // Loop bottom up
        for (loop =Height - 1; loop >= 0; loop--)  
        {
            for (loop2 = 0; loop2 < Width; loop2++)
            {
                // Load up the Blue component and output it
                WriteBuffer[BufferIndex++] = *(Bitmap_in + loop2*3   + loop * Pitch); // Bug fix 6-5
				
                // Load up the green component and output it 
                WriteBuffer[BufferIndex++] = *(Bitmap_in + loop2*3 + 1 + loop * Pitch); // Bug fix 6-5
				
                // Load up the red component and output it 
                WriteBuffer[BufferIndex++] = *(Bitmap_in + loop2*3 + 2 + loop * Pitch);
            }
			
            for (i = 0; i < padding; ++i)
                WriteBuffer[BufferIndex++] = (CHAR)0x00;
        }
		
        // At this point the buffer should be full, so just write it out
        WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);
        
    }
	
    // 16 bit surfaces
    else if (desc.Format == D3DFMT_R5G6B5 )	
    {
        BYTE* Bitmap_in = (BYTE*)lrect.pBits;
		
        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if (padding > 0)
            padding = 4 - padding;
		
        // If the green bitmask equals 565 mode, do 16-bit mode, otherwise do 15-bit mode
        // NOTE: We are reversing the component order (ie. BGR instead of RGB)
		//    and we are outputting it bottom up because BMP files are backwards and upside down.
		
		
        // Loop bottom up
		for (loop =Height - 1; loop >= 0; loop--)
        {
			for (loop2 = 0; loop2 < Width; loop2++)
            {
				outpixel = *((WORD *)(Bitmap_in + loop2*2 + loop * Pitch));
				
				// Load up the Blue component and output it
				outbyte = (8*((outpixel)&0x001f));// blue
				WriteBuffer[BufferIndex++] = outbyte;
				
				// Load up the green component and output it 
				outbyte = (4*((outpixel >> 5)&0x003f)); 
				WriteBuffer[BufferIndex++] = outbyte;
				
				// Load up the red component and output it 
				outbyte = (8*((outpixel >> 11)&0x001f));
				WriteBuffer[BufferIndex++] = outbyte;
			}
			
               for (i = 0; i < padding; ++i)
                   WriteBuffer[BufferIndex++] = (CHAR)0x00;
        }
		// At this point the buffer should be full, so just write it out
		WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);
	}
    else if (desc.Format == D3DFMT_X1R5G5B5 || desc.Format == D3DFMT_A1R5G5B5 )	// Assume 555 mode. 15-bit mode
    {
		BYTE* Bitmap_in = (BYTE*)lrect.pBits;
		
        // Determine the padding at the end of a row.
        padding = (Width * 3) % 4;
        if (padding > 0)
            padding = 4 - padding;
            // Loop bottom up
		for (loop =Height - 1; loop >= 0; loop--)  
        {
			for (loop2 = 0; loop2 < Width; loop2++)
            {
				outpixel = *((WORD *)(Bitmap_in + loop2*2 + loop * Pitch));
					
                // Load up the Blue component and output it
                outbyte = (8*((outpixel)&0x001f));// blue
                WriteBuffer[BufferIndex++] = outbyte;
					
                // Load up the green component and output it 
                outbyte = (8*((outpixel >> 5)&0x001f)); 
                WriteBuffer[BufferIndex++] = outbyte;
					
                // Load up the red component and output it 
                outbyte = (8*((outpixel >> 10)&0x001f));  // BUG FIX here
                WriteBuffer[BufferIndex++] = outbyte;
			}
				
            for (i = 0; i < padding; ++i)
                WriteBuffer[BufferIndex++] = (CHAR)0x00;
		}
		// At this point the buffer should be full, so just write it out
		WriteFile(file_out, WriteBuffer, BufferIndex, &numwrite, NULL);
    }
	pSurf->UnlockRect();		

    delete[] WriteBuffer;
	
    //CloseHandle(file_out);
    return 1;	
}

void	AgcEngine::ProcessDeleteWindow()
{
	AuNode<AgcWindowNode *>	*	pcsNode = m_listWindowToDelete.GetHeadNode();
	AgcWindowNode *				pcsWindowNode;

	while (pcsNode)
	{
		pcsWindowNode = pcsNode->GetData();

		if (pcsWindowNode->pWindow)
			delete pcsWindowNode->pWindow;

		delete pcsWindowNode;

		pcsNode = pcsNode->GetNextNode();
	}

	m_listWindowToDelete.RemoveAll();
}

//////////////////////////////////////////////////////
// Lua
//////////////////////////////////////////////////////

LuaGlue	LG_Wait( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	UINT32 uWaitTime	= ( UINT32 ) pLua->GetNumberArgument( 1 , 0 );

	g_pEngine->Lua_PushWait( uWaitTime );

	return 0;
}

LuaGlue	LG_SetProjection( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	float fProjection = ( float ) pLua->GetNumberArgument( 1 , 0.7 );

	g_pEngine->SetProjection( fProjection );

	return 0;
}

LuaGlue	LG_GetProjection( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	float fProjection = g_pEngine->GetProjection();

	LUA_RETURN( fProjection )
}

LuaGlue	LG_RunScript( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	const char *pName = pLua->GetStringArgument( 1 , "*empty*" );

	if( !strncmp( pName , "*empty*" , 7 ) )
	{
		LUA_RETURN( TRUE )
	}
	else
	{
		BOOL bRet = g_pEngine->Lua_RunScript( pName , FALSE );
		LUA_RETURN( bRet )
	}
}

LuaGlue	LG_SlowIdle( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	UINT32 uDelay	= ( UINT32 ) pLua->GetNumberArgument( 1 , 0 );

	g_pEngine->m_uSlowIdleIndicator = uDelay;

	if( uDelay == 0 )
	{
		g_pEngine->LuaErrorMessage( "Turn off slow Idle Check" );
	}
	else
	{
		char str[ 256 ];
		sprintf( str , "finding over '%d ms' Idle Modules...." ,  uDelay );
		g_pEngine->LuaErrorMessage( str );
	}

	LUA_RETURN( TRUE )
}

void	AgcEngine::LuaInitialize()
{
	AuLua * pLua = AuLua::GetPointer();

	ASSERT( pLua );
	if( NULL == pLua ) return;

	pLua->SetErrorHandler( AgcEngine::LuaErrorHandler );

	// �⺻ Glue Function�� ����ϰ�
	// ��� �۷� ��ǵ� ���⼭ ����..
	luaDef MVLuaGlue[] = 
	{
		{"Wait"				,	LG_Wait					},
		{"GetProjection"	,	LG_GetProjection		},
		{"SetProjection"	,	LG_SetProjection		},
		{"RunScript"		,	LG_RunScript			},
		{"SlowIdle"			,	LG_SlowIdle				},
		{NULL				,	NULL					},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}

	pLua->RunString( "TRUE = 1" );
	pLua->RunString( "FALSE = 0" );

	// �� ��⿡ Lua Initialize�� ȣ�����ش�.
	ApModuleList *		pList = ApModuleManager::m_listModule;
	while(pList)
	{
		if(pList->pModule->GetModuleType() == APMODULE_TYPE_CLIENT )
			((AgcModule*)(pList->pModule))->OnLuaInitialize( pLua );

		pList = pList->next;
	}
}

BOOL	AgcEngine::Lua_RunScript( std::string strFilename , BOOL bEncrypted )
{
	Lua_Flush_Queue();

	// TODO : ��� ��ũ��Ʈ �ε�
	// ������ ���� �� �� ���κ��� �м��ؼ�
	// Queue �� �о� ����.
	// �̹� ��ũ��Ʈ�� �ε����̸� ������ ��½�Ŵ.

    char   szModName[ MAX_PATH + 1 ] ;
	//char	dir[ MAX_PATH ] , drive[ MAX_PATH ];

	GetCurrentDirectory( MAX_PATH , szModName );

  //  if ( 0 == GetModuleFileNameA ( NULL , szModName , MAX_PATH ) )
  //  {
		//LuaErrorMessage( "Cannot find Module Name" );
		//return FALSE;
  //  }

	//_splitpath( szModName , drive , dir , NULL , NULL );
	std::string	strPath;

	strPath =	szModName		;
	strPath +=	"\\"			;
	strPath +=	strFilename		;

	FILE	* pFile = fopen( strPath.c_str() , "r" );
	if( !pFile )
	{
		LuaErrorMessage( "Cannot read Lua Script File" );
		return FALSE;
	}
	int nEndPos = 0;;	
	
	fseek(pFile, 0, SEEK_END);
	nEndPos = ftell(pFile);
	fseek(pFile, 0 , SEEK_SET);

	if( bEncrypted )
	{
		// Decryption �۾�..

	}

	while( !feof( pFile ) )
	{
		char	strBuffer[ 65535 ];
		if ( fgets( strBuffer , 65535 , pFile ) )
		{
			Lua_EnQueue( strBuffer );
		}
	}

	fclose( pFile );
	return TRUE;
}

BOOL	AgcEngine::Lua_IsRunning()
{
	// ť�� ����Ÿ�� ���� ������ TRUE
	if( m_queueLuaCommand.size() )	return TRUE;
	else							return FALSE;
}

BOOL	AgcEngine::Lua_EnQueue( std::string str )
{
	// ������ �Ұž��� -_-;;;
	m_queueLuaCommand.push( str );
	return TRUE;
}

BOOL	AgcEngine::Lua_PushWait( UINT32	uWaitTime )
{
	// �̹� ���� �ϰ� �ְų� �ϴ°� �״��� �Ű澲�� �ʴ´�.

	m_luaWaitInfo.eType	= stLuaWaitInfo::WAITTIME;
	m_luaWaitInfo.uTime	= uWaitTime + ::GetTickCount();

	return TRUE;
}

void	AgcEngine::Lua_Flush_Queue()
{
	// Ŀ�ǵ� ť�� �ѹ濡 ����������.
	while( !m_queueLuaCommand.empty() ) m_queueLuaCommand.pop();
	Lua_EndWait();
}

void	AgcEngine::ProcessIdleLuaCommand()
{
	// ������
	UINT32	uCurrentTime = ::GetTickCount();
	AuLua * pLua = AuLua::GetPointer();

	ASSERT( pLua );
	if( NULL == pLua ) return;

	std::string	strCommandBuffer;
	INT32 nCount = 0;
	while( m_queueLuaCommand.size() )
	{
		if( m_luaWaitInfo.IsWaitTime( uCurrentTime ) )
			break;

		std::string strCommand = m_queueLuaCommand.front();
		m_queueLuaCommand.pop();

		strCommandBuffer += strCommand;
		//strCommandBuffer += "\r\n";
		nCount ++;
		if( _strnicmp( strCommand.c_str() , "Wait" , 4 ) == 0 )
			break;
	}

	if( nCount )
	{
		#ifdef _DEBUG
		OutputDebugString( "Run Lua String....\n" );
		OutputDebugString( strCommandBuffer.c_str() );
		#endif

		if( pLua->RunString( strCommandBuffer.c_str() ) )
		{
			// do nothing
		}
		else
		{
			// �߰��� ������ ������ ��ũ�θ� �����Ѵ�.
			Lua_Flush_Queue();
		}
	}
}

void	AgcEngine::LuaErrorHandler( const char * pStr )
{
	if( g_pEngine ) g_pEngine->LuaErrorMessage( pStr );
}

BOOL	AgcEngine::Debug_Idle(UINT32 ulClockCount)
{
	m_ulClockCount = ulClockCount;

	ApModuleList*		cur_node= m_listModuleIdle;

	while(cur_node)
	{
		cur_node->pModule->Idle(ulClockCount);
		cur_node = cur_node->next;
	}

	return TRUE;
}
BOOL	AgcEngine::Debug_Idle2(UINT32 ulClockCount)
{
	m_ulClockCount = ulClockCount;

	ApModuleList*		cur_node= m_listModuleIdle2;

	while(cur_node)
	{
		cur_node->pModule->Idle2(ulClockCount);
		cur_node = cur_node->next;
	}

	return TRUE;
}

bool		AgcEngine::GetVideoMemoryState( DWORD * pTotal , DWORD * pFree )
{
	/*
	LPDIRECTDRAW7 lpDD = NULL;
	DDSCAPS2      ddsCaps2; 
	DWORD         dwTotal; 
	DWORD         dwFree;
	HRESULT       hr; 

	hr = DirectDrawCreateEx(NULL, (VOID**)&lpDD, IID_IDirectDraw7, NULL );
	if (FAILED(hr))
	   return false; 

	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	static	bool	_sbTexture = false;
	if( _sbTexture )
		ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM; 
	else
		ddsCaps2.dwCaps = DDSCAPS_TEXTURE;

	hr = lpDD->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree); 
	if (FAILED(hr))
	   return false;

	if(lpDD)
	   lpDD->Release();

	if( pTotal	) *pTotal	= dwTotal	;
	if( pFree	) *pFree	= dwFree	;

	*/

	return true;
}
