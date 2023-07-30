#ifdef USE_API

/****************************************************************************
 *
 * skeleton.c
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include <rwcore.h>
#include <rtpng.h>
#include <rtbmp.h>
#include <rttiff.h>
//#include <rtcharse.h>
#if (defined(RWPATCH))
#include "rppatch.h"
#endif /* (defined(RWPATCH)) */



#include "skeleton.h"
#include "platform.h"

#ifdef RWMOUSE
#include "mouse.h"
#endif

#ifdef RWTERMINAL
#include "terminal.h"
#endif /* RWTERMINAL */

#include "rtfsyst.h"

/* Default arena size depending on platform. */
// << 20 == *1048576

#if (defined(SKY))
#define rwRESOURCESDEFAULTARENASIZE (8 << 20)
#elif (defined(_XBOX))
#define rwRESOURCESDEFAULTARENASIZE (8 << 20)
#elif (defined(DOLPHIN))
#define rwRESOURCESDEFAULTARENASIZE (4 << 20)
#elif (defined(D3D8_DRVMODEL_H))
#define rwRESOURCESDEFAULTARENASIZE (8 << 20)
#else
#define rwRESOURCESDEFAULTARENASIZE (16 << 20)
//#define rwRESOURCESDEFAULTARENASIZE ( 10 << 20)
#endif

/*
 * TODO: we need a pikeyboa.c equivalent to take care of
 * scancode to rs conversion...
 */

#define NUMVIEWFILECOLUMNS (7)
#define DEFAULTVIEWPREFIX (RWSTRING("View"))

/* *INDENT-OFF* */
static RwUInt8
KeysNormal[]=
{
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 0-7   */
    rsNULL,rsNULL,0xd   ,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 8-15  */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 16-23 */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 24-31 */
    ' ','!','"','#', '$','%','&','\'',                          /* 32-39 */
    '(',')','*','+',',','-','.','/',                            /* 40-47 */
    '0','1','2','3','4','5','6','7',                            /* 48-55 */
    '8','9',':',';','<','=','>','?',                            /*   -3f */
    '@','A','B','C','D','E','F','G',                            /* 40-47 */
    'H','I','J','K','L','M','N','O',                            /* 48-4f */
    'P','Q','R','S','T','U','V','W',                            /* 50-57 */
    'X','Y','Z','[','\\',']','^','_',                           /* 58-5f */
    '`','a','b','c','d','e','f','g',                            /* 60-67 */
    'h','i','j','k','l','m','n','o',                            /* 68-6f */
    'p','q','r','s','t','u','v','w',                            /* 70-77 */
    'x','y','z','{','|','}','~',0x7f,                           /* 78-7f */

    27,rsF1,rsF2,rsF3,rsF4,rsF5,rsF6,rsF7,                      /* 80-87 */
    rsF8,rsF9,rsF10,rsF11, rsF12,rsINS,0x7f,rsHOME,             /* 88-8f */
    rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,rsRIGHT,rsINS,       /* 90-97 */
    0x7f,rsHOME,rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,         /* 98-9f */
    rsRIGHT,rsNUMLOCK,'/','*', '-','+',0xd,' ',                 /* a0-a7 */
    0x8,0x9,rsCAPSLK,0xd, rsNULL,rsNULL,rsNULL,rsNULL,          /* a8-af */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b0-b7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b8-bf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c0-c7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c8-cf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d0-d7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d8-df */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e0-e7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e8-ef */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* f0-f7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL    /* f8-ff */
};

static RwUInt8
KeysShifted[]=
{
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 0-7   */
    rsNULL,rsNULL,0xd,   rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 8-15  */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 16-23 */
    rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,rsNULL,    /* 24-31 */
    ' ','!','"','#', '$','%','&','\"',                          /* 32-39 */
    '(',')','*','+','<','_','>','?',                            /* 40-47 */
    ')','!','@','#','$','%','^','&',                            /* 48-55 */
    '*','(',':',':','<','+','>','?',                            /*   -3f */
    '@','A','B','C','D','E','F','G',                            /* 40-47 */
    'H','I','J','K','L','M','N','O',                            /* 48-4f */
    'P','Q','R','S','T','U','V','W',                            /* 50-57 */
    'X','Y','Z','{','|','}','^','_',                            /* 58-5f */
    '~','A','B','C','D','E','F','G',                            /* 60-67 */
    'H','I','J','K','L','M','N','O',                            /* 68-6f */
    'P','Q','R','S','T','U','V','W',                            /* 70-77 */
    'X','Y','Z','{','|','}','~',0x7f,                           /* 78-7f */

    27,rsF1,rsF2,rsF3,rsF4,rsF5,rsF6,rsF7,                      /* 80-87 */
    rsF8,rsF9,rsF10,rsF11, rsF12,rsINS,0x7f,rsHOME,             /* 88-8f */
    rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,rsRIGHT,rsINS,       /* 90-97 */
    0x7f,rsHOME,rsEND,rsPGUP,rsPGDN,rsUP,rsDOWN,rsLEFT,         /* 98-9f */
    rsRIGHT,rsNUMLOCK,'/','*', '-','+',0xd,' ',                 /* a0-a7 */
    0x8,0x9,rsCAPSLK,0xd, rsNULL,rsNULL,rsNULL,rsNULL,          /* a8-af */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b0-b7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* b8-bf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c0-c7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* c8-cf */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d0-d7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* d8-df */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e0-e7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* e8-ef */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL,   /* f0-f7 */
    rsNULL,rsNULL,rsNULL,rsNULL, rsNULL,rsNULL,rsNULL,rsNULL    /* f8-ff */
};
/* *INDENT-ON* */

typedef struct PresetView PresetView;
struct PresetView
{
    RwV3d               pos;
    RwReal              el;
    RwReal              az;
    RwReal              nearClip;
    RwReal              farClip;
    RwChar             *description;
    PresetView         *next;
};

static RwV3d        Xaxis = { 1.0f, 0.0f, 0.0f };
static RwV3d        Yaxis = { 0.0f, 1.0f, 0.0f };
static RwV3d        Zaxis = { 0.0f, 0.0f, 1.0f };

static RwChar       ViewsFileName[] = RWSTRING("./views.txt");

static PresetView  *PresetViews = 0;
static RwInt32      NumPresetViews = 0;
static RwInt32      CurrentPresetView = -1;
static RwBool       DefaultVideoMode = TRUE;

RsGlobalType        RsGlobal;

// 2004.01.12. steeple
DIALOG_MSG_FUNC GlobalDialogMSGFunc = NULL;
PVOID GlobalDialogMSGFuncClass = NULL;

RwBool
psSelectDevice(RwBool useDefault __RWUNUSED__);

/*
 *****************************************************************************
 */
RwUInt8
RsKeyFromScanCode(RwUInt8 scan, RwBool shiftKeyDown)
{
    return shiftKeyDown ? KeysShifted[scan] : KeysNormal[scan];
}

/*
 *****************************************************************************
 */
#ifdef RWSPLASH
RwBool
RsDisplaySplashScreen(RwBool state)
{
    /* state == FALSE: videomode not available
     * state == TRUE: RenderWare video mode selected
     */
    return psDisplaySplashScreen(state);
}
#endif

/*
 *****************************************************************************
 */
RwUInt32
RsTimer(void)
{
    return psTimer();
}

/*
 *****************************************************************************
 */
void
RsWindowSetText(const RwChar * text)
{
    psWindowSetText(text);

    return;
}

/*
 *****************************************************************************
 */
void
RsErrorMessage(const RwChar * text)
{
    /*  KEYUP message lost on dialogue box -
     *  send NULL message to prevent multiple dbox instances
     */
    RsKeyStatus         ks;

    ks.keyScanCode = rsNULL;
    ks.keyCharCode = rsNULL;

    RsKeyboardEventHandler(rsKEYUP, &ks);

    psErrorMessage(text);

    return;
}

/*
 *****************************************************************************
 */
void
RsWarningMessage(const RwChar * text)
{
    /*  KEYUP message lost on dialogue box -
     *  send NULL message to prevent multiple dbox instances
     */
    RsKeyStatus         ks;

    ks.keyScanCode = rsNULL;
    ks.keyCharCode = rsNULL;

    RsKeyboardEventHandler(rsKEYUP, &ks);

    psWarningMessage(text);

    return;
}

/*
 *****************************************************************************
 */
RwChar
RsPathGetSeparator(void)
{
    return psPathGetSeparator();
}

/*
 *****************************************************************************
 */
void
RsCameraShowRaster(RwCamera * camera)
{
#ifdef RWMOUSE
    rsMouseRender(camera);
#endif
    psCameraShowRaster(camera);

    return;
}

/*
 *****************************************************************************
 */
RwBool
RsAlwaysOnTop(RwBool alwaysOnTop)
{
    return psAlwaysOnTop(alwaysOnTop);
}

/*
 *****************************************************************************
 */
RwBool
RsRegisterImageLoader(void)
{
	// Client Export�� �Ŀ� ���� Format
	/*
    if (!RwImageRegisterImageFormat(RWSTRING("tx2"), RtPNGImageRead, 0))
    {
        return FALSE;
    }

    if (!RwImageRegisterImageFormat(RWSTRING("tx3"), RtTIFFImageRead, 0))
    {
        return FALSE;
    }

    if (!RwImageRegisterImageFormat(RWSTRING("tx4"), RtBMPImageRead, 0))
    {
        return FALSE;
    }
	*/

	// Client Export�ϱ� ���� ���� Format
    if (!RwImageRegisterImageFormat(RWSTRING("bmp"), RtBMPImageRead, 0))
    {
        return FALSE;
    }

    if (!RwImageRegisterImageFormat(RWSTRING("png"), RtPNGImageRead, 0))
    {
        return FALSE;
    }

    if (!RwImageRegisterImageFormat(RWSTRING("tif"), RtTIFFImageRead, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
static              RwBool
RsSetDebug(void)
{
    RwDebugSetHandler(psDebugMessageHandler);

    RwDebugSendMessage(rwDEBUGMESSAGE, RsGlobal.appName,
                       RWSTRING("Debugging Initialized"));

    return TRUE;
}

/*
 *****************************************************************************
 */
void
RsMouseSetVisibility(RwBool visible)
{
#ifdef RWMOUSE
    rsMouseVisible(visible);
#else
    psMouseSetVisibility(visible);
#endif

    return;
}

/*
 *****************************************************************************
 */
void
RsMouseSetPos(RwV2d * pos)
{
#ifdef RWMOUSE
    rsMouseSetPos(pos);
#endif
    psMouseSetPos(pos);

    return;
}

/*
 *****************************************************************************
 */
RwBool
RsSelectDevice(void)
{
    return psSelectDevice(DefaultVideoMode);
}

/*
 *****************************************************************************
 */
RwBool
RsInputDeviceAttach(RsInputDeviceType inputDevice,
                    RsInputEventHandler inputEventHandler)
{
    switch (inputDevice)
    {
        case rsKEYBOARD:
            {
                RsGlobal.keyboard.inputEventHandler = inputEventHandler;
                RsGlobal.keyboard.used = TRUE;
                break;
            }
        case rsMOUSE:
            {
                RsGlobal.mouse.inputEventHandler = inputEventHandler;
                RsGlobal.mouse.used = TRUE;
                break;
            }
        case rsPAD:
            {
                RsGlobal.pad.inputEventHandler = inputEventHandler;
                RsGlobal.pad.used = TRUE;
                break;
            }
        default:
            {
                return FALSE;
            }
    }

    return TRUE;
}


/*
 *****************************************************************************
 */
static RwBool
rsCommandLine(RwChar *arg)
{
    RsEventHandler(rsFILELOAD, arg);

    return TRUE;
}


/*
 *****************************************************************************
 */
static RwBool
rsPreInitCommandLine(RwChar *arg)
{
    if( !strcmp(arg, RWSTRING("-vms")) )
    {
        DefaultVideoMode = FALSE;

        return TRUE;
    }

    return FALSE;
}

/*
 *****************************************************************************
 */
RsEventStatus
RsKeyboardEventHandler(RsEvent event, void *param)
{
    if (RsGlobal.keyboard.used)
    {
        return RsGlobal.keyboard.inputEventHandler(event, param);
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
RsMouseEventHandler(RsEvent event, void *param)
{
#ifdef RWMOUSE
    /* snoop the mouse move event to update the mouse position for
     * a skeleton drawn cursor
     */
    if (event == rsMOUSEMOVE)
    {
		PROFILE("RsMouseEventHandler - MouseMove");

        rsMouseAddDelta(&((RsMouseStatus *) param)->delta);
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }

    if (
		(event == rsLEFTBUTTONDOWN		)	||
		(event == rsRIGHTBUTTONDOWN		)	||
		(event == rsLEFTBUTTONDBLCLK	)	||
		(event == rsRIGHTBUTTONDBLCLK	)	||
		(event == rsMIDDLEBUTTONDOWN	)	||
		(event == rsMIDDLEBUTTONDBLCLK	)	||
		(event == rsMOUSEWHEELMOVE		)	)
    {
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }
#endif

    if (RsGlobal.mouse.used)
    {
        return RsGlobal.mouse.inputEventHandler(event, param);
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */
RsEventStatus
RsPadEventHandler(RsEvent event, void *param)
{
    if (RsGlobal.pad.used)
    {
        return RsGlobal.pad.inputEventHandler(event, param);
    }

    return rsEVENTNOTPROCESSED;
}

/*
 *****************************************************************************
 */

/*  Message intercept used for test purposes */
#ifdef RWTEST
extern RwBool       _rsTestMessageIntercept(RsEvent *, void **);
#endif /* RWTEST */

RsEventStatus
RsEventHandler(RsEvent event, void *param)
{
    RsEventStatus       result;
    RsEventStatus       es;

#ifdef RWTEST
    /*
     * If we are in test mode then all messages and parameters are passed
     * to the test harness message intercept handler for processing.
     * The handler may modify messages and params and/or prevent them from
     * being passed to the application.
     */
    if (_rsTestMessageIntercept(&event, &param) == FALSE)
        return rsEVENTPROCESSED;
#endif /* RWTEST */

#ifdef RWSPLASH
    /*
     * Before rendering the first frame, play the splash screen.
     * Note - there are 2 styles of splash screen - those that change the
     * video mode and those that use the existing one. The choice of which
     * to use os driver specific so the handler gets a look in for both
     * cases. If it doesn't change the video mode then it needs to wait
     * until RenderWare has started - this is a good place to be sure this
     * has happened.
     */
    static RwBool       splashPlayed = FALSE;

    if (!splashPlayed && (event == rsIDLE))
    {
        RsEventHandler(rsDISPLAYSPLASH, (void *) TRUE);
        splashPlayed = TRUE;
    }
#endif

#ifdef RWMOUSE
    /*
     * Snoop the mouse move event to update the mouse position for
     * a skeleton drawn cursor
     */
    if (event == rsMOUSEMOVE)
    {
		PROFILE("RsEventHandler() MouseMove");

        rsMouseAddDelta(&((RsMouseStatus *) param)->delta);
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }

    if (
		(event == rsLEFTBUTTONDOWN		)	||
		(event == rsRIGHTBUTTONDOWN		)	||
		(event == rsLEFTBUTTONDBLCLK	)	||
		(event == rsRIGHTBUTTONDBLCLK	)	||
		(event == rsMIDDLEBUTTONDOWN	)	||
		(event == rsMIDDLEBUTTONDBLCLK	)	||
		(event == rsMOUSEWHEELMOVE		)	)
    {
        rsMouseGetPos(&((RsMouseStatus *) param)->pos);
    }
#endif

    /*
     * Give the application an opportunity to override any events...
     */
    es = AppEventHandler(event, param);

    /*
     * We never allow the app to replace the quit behaviour,
     * only to intercept...
     */
    if (event == rsQUITAPP)
    {
        /*
         * Set the flag which causes the event loop to exit...
         */
        RsGlobal.quit = TRUE;
    }

    if (es == rsEVENTNOTPROCESSED)
    {
        switch (event)
        {
            case rsSELECTDEVICE:
                result =
                    (RsSelectDevice()? rsEVENTPROCESSED : rsEVENTERROR);
                break;

#ifdef RWSPLASH
            case rsDISPLAYSPLASH:
                result = (RsDisplaySplashScreen((RwBool) param) ?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;
#endif
            case rsCOMMANDLINE:
                result = (rsCommandLine((RwChar *) param) ?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;
            case rsPREINITCOMMANDLINE:
                result = (rsPreInitCommandLine((RwChar *) param) ?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;
            case rsINITDEBUG:
                result =
                    (RsSetDebug()? rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsREGISTERIMAGELOADER:
                result = (RsRegisterImageLoader()?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsRWTERMINATE:
                RsRwTerminate();
                result = (rsEVENTPROCESSED);
                break;

            case rsRWINITIALIZE:
                result = (RsRwInitialize(param) ?
                          rsEVENTPROCESSED : rsEVENTERROR);
                break;

            case rsTERMINATE:
                RsTerminate();
                result = (rsEVENTPROCESSED);
                break;

            case rsINITIALIZE:
                result =
                    (RsInitialize()? rsEVENTPROCESSED : rsEVENTERROR);
                break;

            default:
                result = (es);
                break;

        }
    }
    else
    {
        result = (es);
    }

    return result;
}

/*
 *****************************************************************************
 */
void
RsPathnameDestroy(RwChar * buffer)
{
    psPathnameDestroy(buffer);

    return;
}

/*
 *****************************************************************************
 */
RwChar             *
RsPathnameCreate(const RwChar * srcBuffer)
{
    return psPathnameCreate(srcBuffer);
}

/*
 *****************************************************************************
 */
static RwTexDictionary *
texDictDestroyCB(RwTexDictionary *dict,
                 void *data __RWUNUSED__)
{
    RwTexDictionaryDestroy(dict);
    /* Invalid now yes but we don't want to early-out */

    return dict;
}

/*
 *****************************************************************************
 */
#ifdef RWMOUSE
#define RSMOUSETERM()  rsMouseTerm()
#define RSMOUSEINIT()  rsMouseInit()
#endif

#if (!defined(RSMOUSETERM))
#define RSMOUSETERM()          /* No op */
#endif /* (!defined(RSMOUSETERM)) */

#if (!defined(RSMOUSEINIT))
#define RSMOUSEINIT()          /* No op */
#endif /* (!defined(RSMOUSEINIT)) */

void
RsRwTerminate(void)
{
#ifdef RWTERMINAL
    if (NULL != RsGlobal.terminal)
    {
        RsTerminalDestroy(RsGlobal.terminal);
        RsGlobal.terminal = (RsTerminal *)NULL;
    }
#endif /* RWTERMINAL */

    RtCharsetClose();

    RSMOUSETERM();

    /* Destroy any texture dictionaries still lying around */
    RwTexDictionaryForAllTexDictionaries(texDictDestroyCB, NULL);

	// ������ (2005-05-09 ���� 7:28:29) : 
	// ���� �ִ°� �´°Ŵ�~
    psTerminate();
	RtFSManagerClose();

    /* Close RenderWare */

	//::ExitProcess( 1 );
	PostQuitMessage(1);

	//RwEngineStop();
	//RwEngineClose();
	//RwEngineTerm();

    return;
}

/*
 *****************************************************************************
 */
//@{ Jaewon 20050105
// numLevels - number of multisampling levels
RwBool
RsRwInitialize(void *displayID, RwUInt32 numLevels)
//@} Jaewon
{
    RwEngineOpenParams  openParams;

    /*
     * Start RenderWare...
     */

	//@{ 20050513 DDonSS : Threadsafe ó���� ���� ���� �߰���( ������.. �ι�° ���ڿ� Lock �Լ� �����Ҽ� �ֵ��� )
	// ���� ���� �ִ� ������... rwENGINEINITNOFREELISTS �ɼ� ����.
    //if (!RwEngineInit(psGetMemoryFunctions(), rwENGINEINITNOFREELISTS, rwRESOURCESDEFAULTARENASIZE))
    if (!RwEngineInit(psGetMemoryFunctions(), NULL, 0, rwRESOURCESDEFAULTARENASIZE))
    //@} DDonSS
    {
        RsErrorMessage(RWSTRING("Error initializing Engine."));
        return (FALSE);
    }

	//@{ 20050513 DDonSS : Threadsafe Test��.. ���ÿ��� ó������ ����..
#if ( ( defined RWDEBUG ) && ( defined USE_THREADSAFE_CHECK ) )
	RWSRCGLOBAL( debugMainThreadId ) = GetCurrentThreadId();
#endif // ( ( defined RWDEBUG ) && ( defined USE_THREADSAFE_CHECK ) )
	//@} DDonSS

    /*
     * Install any platform specific file systems...
     */
  //  psInstallFileSystem();

    /*
     * Initialize debug message handling...
     */

#ifdef _DEBUG
	RsEventHandler(rsINITDEBUG, NULL);
#endif

    /*
     * Attach all plugins...
     */
    if (RsEventHandler(rsPLUGINATTACH, NULL) == rsEVENTERROR)
    {
        RsErrorMessage(RWSTRING("Error attaching plugin."));
        return (FALSE);
    }

#if (defined(RWPATCH))
    if( !RpPatchPluginAttach() )
    {
        return (FALSE);
    }
#endif /* (defined(RWPATCH)) */

    /*
     * Attach input devices...
     */
    if (RsEventHandler(rsINPUTDEVICEATTACH, NULL) == rsEVENTERROR)
    {
        RsErrorMessage(RWSTRING("Error attaching device."));
        return (FALSE);
    }

#ifdef _DEBUG
	RwGlobals * pGlobal = ( RwGlobals * ) RwEngineInstance;
#endif

    openParams.displayID = displayID;

    if (!RwEngineOpen(&openParams))
    {
        RsErrorMessage(RWSTRING("Error opening engine."));
        RwEngineTerm();
        return (FALSE);
    }


#ifdef RWSPLASH
    /*
     * Before rendering the first frame, play the splash screen.
     * Note - there are 2 styles of splash screen - those that change the
     * video mode and those that use the existing one. The choice of which
     * to use os driver specific so the handler gets a look in for both
     * cases. If it doesn't change the video mode then it needs to wait
     * until RenderWare has started - this is a good place to be sure this
     * has happened.
     */
    if (RsEventHandler(rsDISPLAYSPLASH, FALSE) == rsEVENTERROR)
    {
        RsErrorMessage(RWSTRING("Error displaying splash."));
        RwEngineClose();
        RwEngineTerm();
        return (FALSE);
    }
#endif

    if (RsEventHandler(rsSELECTDEVICE, displayID) == rsEVENTERROR)
    {
        RsErrorMessage(RWSTRING("Error selecting device."));
        RwEngineClose();
        RwEngineTerm();
        return (FALSE);
    }

	RwD3D9EngineSetMultiThreadSafe(TRUE);

	{
/*
		psSelectDevice( FALSE );
		RwInt32				i					= 0;
		RwInt32				iAutoRender			= 0;
		RwUInt32			dwFindIndex			= 0;
		RwSubSystemInfo *	pstSubSysInfo		;
		RwUInt32			dwNumSubSystems		;
		RwUInt32			dwCurSysIndex		;
		RwUInt32			dwNumVideoModes		;
		RwVideoMode *		pstVideoModes		;
		RwUInt32			dwCurVideoModeIndex	;

		VERIFY( dwNumSubSystems = (RwUInt32)(RwEngineGetNumSubSystems()) );
		if(!dwNumSubSystems)
			return FALSE;

		// Setting Sub System
		pstSubSysInfo = (RwSubSystemInfo *) malloc(sizeof(RwSubSystemInfo) * dwNumSubSystems);

		for(i = 0; i < (RwInt32)(dwNumSubSystems); ++i)
		{
			RwEngineGetSubSystemInfo( &pstSubSysInfo[ i ] , i);
		}

		dwCurSysIndex = (RwUInt32)(RwEngineGetCurrentSubSystem());

		{
			// Collecting Video Modes.
			dwNumVideoModes = RwEngineGetNumVideoModes();

			pstVideoModes = (RwVideoMode *) malloc(sizeof(RwVideoMode) * dwNumVideoModes);

			for( RwUInt32 i = 0; i < ( RwUInt32 ) dwNumVideoModes ; ++i)
			{
				RwEngineGetVideoModeInfo( &pstVideoModes[ i ] , i );
			}

			dwCurVideoModeIndex = (RwUInt32)( RwEngineGetCurrentVideoMode() );
		}

		RwEngineSetSubSystem( ( RwInt32 ) ( dwCurSysIndex ) );

		{
			// Find Video Mode Index

			dwFindIndex = -1;

			if(!PsGlobal.fullScreen) // windows.
			{
				dwFindIndex = 0;
			}
			else
			{
				for( RwUInt32 dw = 0 ; dw < ( RwUInt32 ) dwNumVideoModes; ++dw )
				{
					if(	pstVideoModes[dw].width		== (RwInt32)(RsGlobal.maximumWidth	)	&&
						pstVideoModes[dw].height	== (RwInt32)(RsGlobal.)	&&
						pstVideoModes[dw].depth		== (RwInt32)(dwDepth	)	&&
						(pstVideoModes[dw].flags & rwVIDEOMODEEXCLUSIVE )		)

					{
						dwFindIndex = dw;
						break;
					}
				}
			}

			// Finding Video Mode Fails.
			ASSERT( dwFindIndex != -1 && "������� ã�� ����" );
			if( dwFindIndex == -1 )
			{
				TRACE( "������带 ã�� ���ؼ� ���� ������.\n" );
				return FALSE;
			}
		}

		// Setting Video Mode
		if(!RwEngineSetVideoMode(dwFindIndex))
			return FALSE;

		//return TRUE;

		// Old Codes...
		// RwEngineSetVideoMode((RwInt32)0);
*/
	}

	//@{ Jaewon 20050105
	// adjust the multisampling level.
	RwD3D9EngineSetMultiSamplingLevels(numLevels);
	//@} Jaewon

    if (!RwEngineStart())
    {
        RsErrorMessage(RWSTRING("Error starting Engine."));
        RwEngineClose();
        RwEngineTerm();
        return (FALSE);
    }

    /*
     * Register loaders for an image with a particular file extension...
     */
    RsEventHandler(rsREGISTERIMAGELOADER, NULL);

    psNativeTextureSupport();

    RSMOUSEINIT();

    /*
     * Set up the charset toolkit for buffered rendering
     * used by both the menu and terminal...
     */
    //if (FALSE == RtCharsetOpen())
    //{
    //    RwEngineStop();
    //    RwEngineClose();
    //    RwEngineTerm();
    //    return (FALSE);
    //}

	/*
     * Initialise file system manager and install any 
     * platform specific file systems...
     */
    if (RtFSManagerOpen(RTFSMAN_UNLIMITED_NUM_FS) != FALSE)
    {
        if (!psInstallFileSystem())
        {
	        RsErrorMessage(RWSTRING("Error installing file system."));
            return (FALSE);
        }
    }
    else
    {
        RsErrorMessage(RWSTRING("Error opening file system."));
        return (FALSE);
    }

#ifdef RWTERMINAL
    /* Start up the terminal */
    {
        static RwRGBA       fore = { 0, 0xff, 0xff, 0xff };
        static RwRGBA       back = { 0, 0, 0, 0 };
        RwSubSystemInfo     si;

        RsGlobal.terminal = RsTerminalCreate(80, 16, &fore, &back,
                                             (RsTerminalDisplayCallBack)NULL,
                                             NULL);

        if (!RsGlobal.terminal)
        {
	        RsErrorMessage(RWSTRING("Error creating terminal."));
            RwEngineStop();
            RwEngineClose();
            RwEngineTerm();
            return (FALSE);
        }

#ifdef RWDEBUG
        RsTerminalPrintf(RsGlobal.terminal,
                         RWSTRING("RenderWare(TM) V%d.%d.%d.%d DEBUG started.\n"),
                         (RwEngineGetVersion() & 0xf0000) >> 16,
                         (RwEngineGetVersion() & 0x0f000) >> 12,
                         (RwEngineGetVersion() & 0x00f00) >> 8,
                         (RwEngineGetVersion() & 0x000ff) );
#else
        RsTerminalPrintf(RsGlobal.terminal,
                         RWSTRING("RenderWare(TM) V%d.%d.%d.%d started.\n"),
                         (RwEngineGetVersion() & 0xf0000) >> 16,
                         (RwEngineGetVersion() & 0x0f000) >> 12,
                         (RwEngineGetVersion() & 0x00f00) >> 8,
                         (RwEngineGetVersion() & 0x000ff) );
#endif
        RwEngineGetSubSystemInfo(&si, RwEngineGetCurrentSubSystem());
        RsTerminalPrintf(RsGlobal.terminal,
                         RWSTRING("Subsystem: %s.\n"), si.name);
    }
#endif /* RWTERMINAL */

    return TRUE;
}

/*
 *****************************************************************************
 */
void
RsTerminate(void)
{
	/*
	// ������ (2005-05-09 ���� 7:27:47) : 
	// �海�������� �̾߱��ؼ� �ű�.
    psTerminate();

	RtFSManagerClose();
	*/

    return;
}

/*
 *****************************************************************************
 */
RwBool
RsInitialize(void)
{
    /*
     * Initialize Platform independent data...
     */
    RwBool              result;

    RsGlobal.appName = RWSTRING("RenderWare Application");

	// 2006/07/07 Magoja
    //RsGlobal.maximumWidth = 0;
    //RsGlobal.maximumHeight = 0;
    RsGlobal.maxFPS = 0;
#ifdef RWTERMINAL
    RsGlobal.terminal = (RsTerminal *)NULL;
#endif /* RWTERMINAL */
    RsGlobal.quit = FALSE;

    /* setup the keyboard */
    RsGlobal.keyboard.inputDeviceType = rsKEYBOARD;
    RsGlobal.keyboard.inputEventHandler = 0;
    RsGlobal.keyboard.used = FALSE;

    /* setup the mouse */
    RsGlobal.mouse.inputDeviceType = rsMOUSE;
    RsGlobal.mouse.inputEventHandler = 0;
    RsGlobal.mouse.used = FALSE;

    /* setup the pad */
    RsGlobal.pad.inputDeviceType = rsPAD;
    RsGlobal.pad.inputEventHandler = 0;
    RsGlobal.pad.used = FALSE;

    result = psInitialize();

    return result;
}

/*
 *****************************************************************************
 */
RwBool
RsSetModelTexturePath(const RwChar *objectPathName)
{
    RwChar imagePath[1024], separator;
    RwChar texturesPath[1024];
    RwChar temp[16];
    RwInt32 i;

    separator = RsPathGetSeparator();

    /*
     * Create an image path with the same name as the file object...
     */
    rwstrcpy(imagePath, objectPathName);

    i = rwstrlen(imagePath)-1;

    while( i >= 0 )
    {
        if( imagePath[i] == '.' )
        {
            imagePath[i] = separator;
            imagePath[i+1] = ';';
            imagePath[i+2] = '\0';

            break;
        }

        i--;
    }

    /*
     * Create an image path called 'basedir/textures/'.
     */
    rwstrcpy(texturesPath, objectPathName);

    i = rwstrlen(texturesPath)-1;
    while( i >= 0 )
    {
        if( texturesPath[i] == separator )
        {
            texturesPath[i+1] = '\0';
            break;
        }

        i--;
    }

    /*
     * Insert the basedir if we have one.
     */
    if (i > 0)
    {
	    rwstrcat(imagePath, texturesPath);
	}

    RsSprintf(temp, RWSTRING("texture%c"), separator);
    rwstrcat(imagePath, temp);

    RwImageSetPath(imagePath);

    return TRUE;
}


/*
 *****************************************************************************
 */
RwBool
RsSetPresetView(RwCamera * camera, RwInt32 viewNum)
{
    RwFrame            *frame;
    RwMatrix           *matrix;
    RwV3d              *pos;
    PresetView         *view;
    RwInt32             count;

    if (!camera || !NumPresetViews || viewNum >= NumPresetViews
        || viewNum < 0)
    {
        return (FALSE);
    }

    CurrentPresetView = viewNum;

    frame = RwCameraGetFrame(camera);
    matrix = RwFrameGetMatrix(frame);
    pos = RwMatrixGetPos(matrix);

    view = PresetViews;
    count = NumPresetViews - viewNum - 1;
    while (count > 0 && view)
    {
        view = view->next;
        count--;
    }

    RwFrameSetIdentity(frame);
    RwFrameRotate(frame, &Xaxis, -view->el, rwCOMBINEREPLACE);
    RwFrameRotate(frame, &Yaxis, view->az, rwCOMBINEPOSTCONCAT);
    RwFrameTranslate(frame, &view->pos, rwCOMBINEPOSTCONCAT);
    RwFrameUpdateObjects(frame);

    RwCameraSetNearClipPlane(camera, view->nearClip);
    RwCameraSetFarClipPlane(camera, view->farClip);

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
RsSetNextPresetView(RwCamera * camera)
{
    if (!camera || !NumPresetViews)
    {
        return FALSE;
    }

    if (++CurrentPresetView >= NumPresetViews)
    {
        CurrentPresetView = 0;
    }

    if (!RsSetPresetView(camera, CurrentPresetView))
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
RsSetPreviousPresetView(RwCamera * camera)
{
    if (!camera || !NumPresetViews)
    {
        return FALSE;
    }

    if (--CurrentPresetView < 0)
    {
        CurrentPresetView = NumPresetViews - 1;
    }

    if (!RsSetPresetView(camera, CurrentPresetView))
    {
        return FALSE;
    }

    return TRUE;
}

/*
 *****************************************************************************
 */
RwBool
RsDestroyPresetViews(void)
{
    PresetView         *view, *ptr;

    /*
     * Delete all preset views
     */
    if (PresetViews)
    {
        ptr = PresetViews;
        while (ptr)
        {
            view = ptr->next;
            if (ptr->description)
            {
                RwFree(ptr->description);
            }
            RwFree(ptr);
            ptr = view;
        }
    }

    PresetViews = 0;
    NumPresetViews = 0;

    return TRUE;
}

//
//###################################################################################
//
/*RwBool
RsLoadPresetViews(void)
{
    RwChar             *path;
    RwFileFunctions    *fileFuncs;
    RwChar              line[1024];
    void               *fp;
    RwReal              el, az;
    RwV3d               pos;
    RwReal              farClip;
    RwReal              nearClip;
    PresetView         *view;
    RwChar              description[64];
    RwInt32             result;
    RwInt32             length;

    // blow away any existing views 
    RsDestroyPresetViews();
    CurrentPresetView = -1;

    fileFuncs = RwOsGetFileInterface();
    if (fileFuncs)
    {
        path = RsPathnameCreate(ViewsFileName);
        fp = fileFuncs->rwfopen(path, RWSTRING("r"));
        RsPathnameDestroy(path);

        if (fp)
        {
            //
            // Load preset views from the file, one per line
            //
            while (fileFuncs->rwfgets(line, 1023, fp))
            {
                RwChar             *src, *dst;

                // delete any control characters 
                length = rwstrlen(line);
                src = dst = line;
                while (*src != '\0')
                {
                    // is displayable 
                    if (isprint(*src))
                    {
                        *dst++ = *src;
                    }
                    src++;
                }

                // zero terminated.... 
                *dst = '\0';

                result =
                    rwsscanf(line,
                             RWSTRING("%f%f%f%f%f%f%f %[^\0]"),
                             &pos.x, &pos.y, &pos.z,
                             &el, &az, &nearClip, &farClip,
                             description);

                // ignore any invalid lines 
                if (result >= NUMVIEWFILECOLUMNS)
                {
                    if (result == NUMVIEWFILECOLUMNS)
                    {
                         //
                         // The user has deleted the description
                         // in the file; use the default one
                         //
                        RsSprintf(description,
                                  "%s%d",
                                  DEFAULTVIEWPREFIX, NumPresetViews);
                        result++;
                    }

                    //view = (PresetView *) RwMalloc(sizeof(PresetView));
                    view = (PresetView *) malloc(sizeof(PresetView));
                    if (!view)
                    {
                        RsErrorMessage(RWSTRING
                                       ("Out of memory - not all preset views loaded"));

                        return FALSE;
                    }
                    view->pos = pos;
                    view->el = el;
                    view->az = az;
                    view->nearClip = nearClip;
                    view->farClip = farClip;
                    view->description =
                        //(RwChar *) RwMalloc(rwstrlen(description) + 1);
                        (RwChar *) malloc(rwstrlen(description) + 1);
                    if (!view->description)
                    {
                        RsErrorMessage(RWSTRING
                                       ("Out of memory - not all preset views loaded"));
                        RwFree(view);

                        return FALSE;
                    }
                    rwstrcpy(view->description, description);

                    // add to head of list 
                    view->next = PresetViews;
                    PresetViews = view;
                    NumPresetViews++;
                }
            }
            fileFuncs->rwfclose(fp);
        }
    }

    return NumPresetViews ? TRUE : FALSE;
}*/

/*
 *****************************************************************************
 */
static void
CameraElAzPosFromLTM(RwCamera * camera,
                     RwReal * elevation, RwReal * azimuth, RwV3d * pos)
{
    RwMatrix           *matrix;
    RwV3d              *at, projAt;
    RwReal              dot;
    RwFrame            *frame;

    /*
     * Use the camera frame LTM,
     * as we want the absolute elevation and azimuth parameters,
     * not relative to the base frame that may be following a spline path...
     */
    frame = RwCameraGetFrame(camera);
    matrix = RwFrameGetLTM(frame);
    at = RwMatrixGetAt(matrix);

    *pos = *RwMatrixGetPos(matrix);

    dot = RwV3dDotProduct(at, &Yaxis);

    /*
     * Make sure the dot product is not out of range (due to numerical
     * inccuracies) so the arc-cos doesn't crash out on us. Usually, if
     * it's outside the useable range, then it is just marginal...
     */
    if (dot > 1.0f)
    {
        dot = 1.0f;
    }
    else if (dot < -1.0f)
    {
        dot = -1.0f;
    }

    *elevation = 90.0f - 180.0f * (RwReal) RwACos(dot) / rwPI;

    RwV3dScale(&projAt, &Yaxis, dot);
    RwV3dSub(&projAt, at, &projAt);
    RwV3dNormalize(&projAt, &projAt);

    dot = RwV3dDotProduct(&projAt, &Zaxis);

    /*
     * Numerical inaccuracies here are more likely due the 
     * approximate square root use by RwV3dNormalize...
     */
    if (dot > 1.0f)
    {
        dot = 1.0f;
    }
    else if (dot < -1.0f)
    {
        dot = -1.0f;
    }

    *azimuth = 180.0f * (RwReal) RwACos(dot) / rwPI;

    if (RwV3dDotProduct(&projAt, &Xaxis) < 0.0f)
    {
        *azimuth = -*azimuth;
    }

    return;
}

/*
 *****************************************************************************
 */
RwChar             *
RsGetPresetViewDescription(void)
{
    PresetView         *view;
    RwInt32             count;

    if (!NumPresetViews || CurrentPresetView == -1)
    {
        return 0;
    }

    view = PresetViews;
    count = NumPresetViews - CurrentPresetView - 1;

    while (count > 0 && view)
    {
        view = view->next;
        count--;
    }

    return view->description;
}

//
//################################################################################
//
/*
RwBool
RsSavePresetView(RwCamera * camera)
{
    RwChar             *path;
    RwFileFunctions    *fileFuncs;
    RwChar              line[1024];
    void               *fp;
    RwReal              el, az;
    RwV3d               pos;
    RwUInt32            length;
    RwBool              result;

    result = FALSE;
    fileFuncs = RwOsGetFileInterface();
    if (fileFuncs)
    {
        path = RsPathnameCreate(ViewsFileName);
        fp = fileFuncs->rwfopen(path, RWSTRING("a"));
        RsPathnameDestroy(path);

        if (fp)
        {
            CameraElAzPosFromLTM(camera, &el, &az, &pos);

            RsSprintf(line,
                      RWSTRING
                      ("%0.6f %0.6f %0.6f  %0.6f %0.6f  %0.6f %0.6f %s%d\n"),
                      pos.x, pos.y, pos.z, el, az,
                      RwCameraGetNearClipPlane(camera),
                      RwCameraGetFarClipPlane(camera),
                      DEFAULTVIEWPREFIX, NumPresetViews);

            length = rwstrlen(line);
            if (fileFuncs->rwfputs(line, fp) > 0)
            {
                result = RsLoadPresetViews();
                if (result)
                {
                    RsSetPresetView(camera, NumPresetViews - 1);
                }
            }
            else
            {
                RsErrorMessage(RWSTRING
                               ("Cannot write to preset view file"));
            }

            fileFuncs->rwfclose(fp);

        }
        else
        {
            RsErrorMessage(RWSTRING("Cannot open preset view file"));
        }
    }

    return result;
}
*/

/*
 *****************************************************************************
 */
RwImage *
RsGrabScreen(RwCamera *camera)
{
    return psGrabScreen(camera);
}


/*
 *****************************************************************************
 */
RtCharset *
RsCharsetPrint(RtCharset *charSet,
               const RwChar *string,
               RwInt32 x, RwInt32 y,
               RsPrintPos pos)
{
    RtCharsetDesc charsetDesc;
    RwUInt32  width,   height;
    RwUInt32  strLen;
    RwRaster *camRas;

    RSASSERT(pos >= rsPRINTPOSMIDDLE);
    RSASSERT(pos <= rsPRINTPOSBOTTOMRIGHT);
    RSASSERT(!((pos & rsPRINTPOSLEFT) && (pos & rsPRINTPOSRIGHT )));
    RSASSERT(!((pos & rsPRINTPOSTOP ) && (pos & rsPRINTPOSBOTTOM)));

    RSASSERT(NULL != RWSRCGLOBAL(curCamera));
    camRas = RwCameraGetRaster((RwCamera *)RWSRCGLOBAL(curCamera));
    RSASSERT(NULL != camRas);

    width  = RwRasterGetWidth(camRas);
    height = RwRasterGetHeight(camRas);

    RtCharsetGetDesc(charSet, &charsetDesc);
    strLen = rwstrlen(string);

    x *= charsetDesc.width;
    y *= charsetDesc.height;

    /* Depending on where on-screen text is being displayed, we position
     * it inside a platform-dependent safety margin, which helps stop
     * text being lost of the side of TVs (i.e for consoles). x and y
     * are now just offsets from the position we calc, used to group
     * strings together. Yeah, it's not foolproof... but hey. You can
     * always use RtCharsetPrint() directly. */
    if (pos & rsPRINTPOSLEFT)
    {
        x += charsetDesc.width*rsPRINTMARGINLEFT;
    }
    else if (pos & rsPRINTPOSRIGHT)
    {
        x += width - charsetDesc.width*(strLen + rsPRINTMARGINRIGHT);
    }

    if ((rsPRINTPOSLEFT   == pos) ||
        (rsPRINTPOSRIGHT  == pos) ||
        (rsPRINTPOSMIDDLE == pos) )
    {
        /* Centre the string vertically */
        y += (height - charsetDesc.height) / 2;
    }

    if (pos & rsPRINTPOSTOP)
    {
        y += charsetDesc.height*rsPRINTMARGINTOP;
    }
    else if (pos & rsPRINTPOSBOTTOM)
    {
        y += height - charsetDesc.height*(1 + rsPRINTMARGINBOTTOM);
    }

    if ((rsPRINTPOSTOP    == pos) ||
        (rsPRINTPOSBOTTOM == pos) ||
        (rsPRINTPOSMIDDLE == pos) )
    {
        /* Centre the string horizontally */
        x += (width - charsetDesc.width*strLen) / 2;
    }

    return(RtCharsetPrint(charSet, string, x, y));
}


/*
 *****************************************************************************
 */

// 2004.01.12. steeple
void
RsSetDialogMsgFunc(DIALOG_MSG_FUNC pfDialogMsgFunc)
{
	GlobalDialogMSGFunc = pfDialogMsgFunc;
}

// 2004.01.12. steeple
void
RsSetDialogMsgFuncClass(PVOID pClass)
{
	GlobalDialogMSGFuncClass = pClass;
}

// 2004.01.12. steeple
BOOL
RsIsDialogMessage(MSG* pMessage)
{
	if(!GlobalDialogMSGFunc || !GlobalDialogMSGFuncClass)
		return FALSE;

	return GlobalDialogMSGFunc(pMessage, GlobalDialogMSGFuncClass);
}

/*
 *****************************************************************************
 */

#endif // USE_API