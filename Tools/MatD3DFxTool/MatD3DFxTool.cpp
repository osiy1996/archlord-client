//@{ Jaewon 20040823
// created.
// the main framework of MatD3DFxTool(base on AgcSkeleton).
//@} Jaewon

#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>

#include <rwcore.h>
#include <rpworld.h>
#include <rpskin.h>
#include <rpcollis.h>
#include <rtcharse.h>
#include <rtfsyst.h>
#include <rtbmp.h>
#include <rtras.h>
#include <rtpng.h>
#include <rttiff.h>
//@{ Jaewon 20041004
#include <rpusrdat.h>
//@} Jaewon
//@{ Jaewon 20041215
#include <rprandom.h>
#include "AcuRtAmbOcclMap.h"
#include <rppvs.h>
//@} Jaewon
//@{ Jaewon 20050107
#include "polypack.h"
//@} Jaewon

#include <AcuRpMatD3DFx.h>

#include "skel\skeleton.h"
#include "skel\menu.h"
#include "skel\camera.h"
#include "skel\win.h"

#include "Scene.h"
#include "Control.h"

#include "DXUTgui\DXUTgui.h"
#include "FXUI.h"

//@{ Jaewon 20041103
#include "MemoryManager\mmgr.h"
//@} Jaewon

#define DEFAULT_ASPECTRATIO (4.0f/3.0f)

#define DEFAULT_SCREEN_WIDTH (640)
#define DEFAULT_SCREEN_HEIGHT (480)

// UI control IDs
#define IDC_FPS						1
#define IDC_SAVE					2
#define IDC_STATUS					3
#define IDC_PLAY_ANIM				4
#define IDC_WIREFRAME_ONSELECTION	5
//@{ Jaewon 20040909
#define IDC_OPEN					6
//@} Jaewon
//@{ Jaewon 20040923
#define IDC_AMBIENT_LIGHT			7	
#define IDC_DIRECTIONAL_LIGHT		8
//@} Jaewon
//@{ Jaewon 20041103
#define IDC_MODEL_STATS				9
//@} Jaewon
//@{ Jaewon 20041201
#define IDC_MERGE_TEXTURES			10
//@} Jaewon
//@{ Jaewon 20041215
#define IDC_GENERATE_AMBOCCLMAPS	11
//@} Jaewon
//@{ Jaewon 20041216
#define IDC_SAVE_AMBOCCLMAPS		12
#define IDC_ROTATE_LIGHT			13
//@} Jaewon
//@{ Jaewon 20050105
#define IDC_GENERATE_AMBOCCLMAPUVS	14
//@} Jaewon
//@{ Jaewon 20050708
#define IDC_SHOW_BACKGROUND			15
//@} Jaewon
//@{ Jaewon 20050825
#define IDC_REMOVE_FX				16
//@} Jaewon

struct IdleState
{
    RwUInt32 m_cCall;
    RwUInt32 m_iLastFrameTime;
	RwUInt32 m_iLastAnimTime;
};

static RwBool _bShowFPS = TRUE;
//@{ Jaewon 20050708
// ;)
static bool _bShowBackground = true;
//@} Jaewon

static RwInt32 _cFrame = 0;
static RwInt32 _nFramesPerSecond = 0;

static RwRGBA _foregroundColor = {200, 200, 200, 255};
static RwRGBA _backgroundColor = { 64,  64,  64,   0};

RtCharset *_pCharset = (RwRaster *)NULL;

// storage for the name of the last opened file.
//@{ Jaewon 20050216
// _openFilePath -> _openFileName
TCHAR _openFileName[MAX_PATH];
//@} Jaewon

Scene* _pScene = NULL;

FXUI* _pFXUI = NULL;

// dialog for standard control
CDXUTDialog _HUD; 

static RwTextureCallBackRead _originalTextureReadCB = NULL;

//@{ Jaewon 20041102
// initial directory for model loading
extern TCHAR _modelLoadPath[MAX_PATH];
// initial directory for model saving
extern TCHAR _modelSavePath[MAX_PATH];
//@} Jaewon

//@{ Jaewon 20041104
// brand new exception handler - XCrashReport
#include "XCrashReport/ExceptionHandler.h"
static LPTOP_LEVEL_EXCEPTION_FILTER _originalExceptionHandler = NULL;
static LONG __stdcall xCrashExceptionHandler(EXCEPTION_POINTERS* pExPtrs)
{
	// additional log for graphics card
	TCHAR buf[1024];
	buf[0] = '\0';
/*	sprintf(buf, "Graphics card: %s\r\nDriver version: %d.%d.%d.%d", 
					AcuDeviceCheck::m_strDeviceName,
					AcuDeviceCheck::m_iProduct,
					AcuDeviceCheck::m_iVersion,
					AcuDeviceCheck::m_iSubVersion,
					AcuDeviceCheck::m_iBuild);*/

	// now, the XCrashReport comes in.
	return RecordExceptionInfo(pExPtrs, _T("MatD3DFxTool"), buf);
}
//@} Jaewon

//@{ Jaewon 20041217
// for the incremental ambient occlusion illumination
static bool _bIllum = false;
static bool _bCreated = false;
//@} Jaewon

static void onLostDevice()
{
	DXUTGetGlobalDialogResourceManager()->OnLostDevice();
}
static void onResetDevice()
{
	DXUTGetGlobalDialogResourceManager()->OnResetDevice();

	if(_pScene == NULL)
		return;

	int w = RwRasterGetWidth(RwCameraGetRaster(_pScene->getCamera()));
	int h = RwRasterGetHeight(RwCameraGetRaster(_pScene->getCamera()));
	_HUD.SetLocation(0, 0);
	_HUD.SetSize(w, h);
	if(_HUD.GetControl(IDC_STATUS))
		_HUD.GetControl(IDC_STATUS)->SetLocation(20, h-25);
	
	//@{ Jaewon 20041103
	if(_HUD.GetControl(IDC_MODEL_STATS))
		_HUD.GetControl(IDC_MODEL_STATS)->SetSize(w-400, h-100);
	//@} Jaewon

	_pFXUI->resize(w, h);
}

static rwD3D9DeviceRestoreCallBack _oldD3D9RestoreDeviceCB = NULL;
static rwD3D9DeviceReleaseCallBack _oldD3D9ReleaseDeviceCB = NULL;
static void newD3D9ReleaseDeviceCB()
{
	onLostDevice();

	if(_oldD3D9ReleaseDeviceCB)
		_oldD3D9ReleaseDeviceCB();
}
static void newD3D9RestoreDeviceCB()
{
	if(_oldD3D9RestoreDeviceCB)
		_oldD3D9RestoreDeviceCB();

	onResetDevice();
}                 

LRESULT UIMsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* pbNoFurtherProcessing)
{
	// Give the dialogs a chance to handle the message first.
	*pbNoFurtherProcessing = _HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if(*pbNoFurtherProcessing)
		return 0;

	if(_pFXUI)
	{
	*pbNoFurtherProcessing = _pFXUI->msgProc(hWnd, uMsg, wParam, lParam);
	if(*pbNoFurtherProcessing)
		return 0;
	}

	return 0;
}

void CALLBACK onGUIEvent(UINT nEvent, int nControlID, CDXUTControl *pControl)
{
	switch(nControlID)
	{
	//@{ Jaewon 20050708
	// ;)
	case IDC_SHOW_BACKGROUND:
		{
			if(((CDXUTCheckBox*)pControl)->GetChecked())
				_bShowBackground = true;
			else
				_bShowBackground = false;
		}
		break;
	//@} Jaewon
	case IDC_WIREFRAME_ONSELECTION:
		{
			if(((CDXUTCheckBox*)pControl)->GetChecked())
				_pFXUI->setWireframeOnSelection(TRUE);
			else
				_pFXUI->setWireframeOnSelection(FALSE);
		}
		break;
	//@{ Jaewon 20040909
	case IDC_OPEN:
		{
		TCHAR pathname[MAX_PATH];
		TCHAR filetitle[MAX_PATH];
		_tcscpy(pathname, TEXT("*.rws"));
		//@{ Jaewon 20041102
		// _modelLoadPath
		if(doFileDlg(TRUE, TEXT("rws"), pathname, NULL, 
					 TEXT("RenderWare Stream Files (*.rws)\0.rws\0All Files (*.*)\0*.*\0\0"),
					 psWindowGetHandle(),
					 filetitle, NULL, _modelLoadPath))
		//@} Jaewon
		{
			AppEventHandler(rsFILELOAD, pathname);
		}
		}
		break;
	//@} Jaewon
	case IDC_SAVE:
		{
		TCHAR pathname[MAX_PATH];
		TCHAR filetitle[MAX_PATH];
		//@{ Jaewon 20050216
		// Use _openFileName instead of TEXT("*.rws".
		_tcscpy(pathname, _openFileName);
		//@} Jaewon
		//@{ Jaewon 20041102
		// _modelSavePath
		if(doFileDlg(FALSE, TEXT("rws"), pathname, OFN_OVERWRITEPROMPT, 
					 TEXT("RenderWare Stream Files (*.rws)\0*.rws\0All Files (*.*)\0*.*\0\0"),
					 psWindowGetHandle(),
					 filetitle, NULL, _modelSavePath))
		 //@} Jaewon
		{
 			bool bOK = _pScene->saveRWS(pathname);
			if(bOK)
				_HUD.GetStatic(IDC_STATUS)->SetText(TEXT("Successfully saved."));
			else
			{
				TCHAR tmp[MAX_PATH];
				_tcscpy(tmp, TEXT("Cannot save "));
				_tcscat(tmp, pathname);
				_tcscat(tmp, TEXT("!"));
				_HUD.GetStatic(IDC_STATUS)->SetText(tmp);
			}
		}
		}
		break;
	//@{ Jaewon 20040923
	case IDC_AMBIENT_LIGHT:
	case IDC_DIRECTIONAL_LIGHT:
		{
			static COLORREF acrCustClr[16];
			D3DCOLOR d3dCol;
			if(nControlID == IDC_AMBIENT_LIGHT)
			{
				const RwRGBAReal *pCol = RpLightGetColor(_pScene->getAmbientLight());
				d3dCol = D3DCOLOR_COLORVALUE(pCol->red, pCol->green, pCol->blue, pCol->alpha);
			}
			else
			{
				const RwRGBAReal *pCol = RpLightGetColor(_pScene->getMainLight());
				d3dCol = D3DCOLOR_COLORVALUE(pCol->red, pCol->green, pCol->blue, pCol->alpha);
			}
			COLORREF rgbCurrent;
			rgbCurrent = RGB(0xff & (d3dCol>>16), 
							 0xff & (d3dCol>>8),
							 0xff & d3dCol);

			CHOOSECOLOR chooseColorStruct = 
			{
				sizeof(CHOOSECOLOR),
				psWindowGetHandle(),
				NULL,
				rgbCurrent,
				acrCustClr,
				CC_FULLOPEN | CC_RGBINIT,
				NULL,
				NULL,
				NULL
			};

			if(ChooseColor(&chooseColorStruct))
			{
				BYTE R = GetRValue(chooseColorStruct.rgbResult);
				BYTE G = GetGValue(chooseColorStruct.rgbResult);
				BYTE B = GetBValue(chooseColorStruct.rgbResult);

				RwRGBAReal col;
				col.red = float(R)/255.0f;
				col.green = float(G)/255.0f;
				col.blue = float(B)/255.0f;
				col.alpha = 1.0f;

				// adjust the scene light.
				if(nControlID == IDC_AMBIENT_LIGHT)
					RpLightSetColor(_pScene->getAmbientLight(), &col);
				else
					RpLightSetColor(_pScene->getMainLight(), &col);
			}	
		}		
		break;

	case IDC_MERGE_TEXTURES:
		{
			_pScene->mergeTextures(512, 512);
		}
		break;

	case IDC_GENERATE_AMBOCCLMAPS:
		{
			bool result;
			if(_bCreated == false)
			{
				// get the size scale.
				Scene::AmbOcclMapSizeScale scale = Scene::AOMSS_ONE;
				if(GetAsyncKeyState('1')&0x8000)
					scale = Scene::AOMSS_QUATER;
				else if(GetAsyncKeyState('2')&0x8000)
					scale = Scene::AOMSS_HALF;
				else if(GetAsyncKeyState('3')&0x8000)
					scale = Scene::AOMSS_ONE;
				else if(GetAsyncKeyState('4')&0x8000)
					scale = Scene::AOMSS_DOUBLE;
				else if(GetAsyncKeyState('5')&0x8000)
					scale = Scene::AOMSS_QUADRUPLE;

				// If 'c' key is pressed, it means that the user wants clearing for the resolution preview only.
				bool justClear = false;
				if(GetAsyncKeyState(VK_SPACE)&0x8000)
					justClear = true;

				// scale parameter added.
				result = _pScene->createAmbOcclMaps(false, scale);
				assert(result);
				result = _pScene->clearAmbOcclMaps();
				assert(result);

				_bIllum = true;
				_HUD.GetButton(IDC_OPEN)->SetEnabled(false);

				// If the user wants clearing only, make the computation end immediately.
				if(justClear)
					_pScene->m_lightingSession.startObj = _pScene->m_lightingSession.totalObj;

				// report the size scale.
				const TCHAR *scaleText[] = { _T("quater"), _T("half"), _T("one"), _T("double"), _T("quadruple") };
				TCHAR buf[256];
				_stprintf(buf, _T("Ambient occlusion map size scale = %s"), scaleText[(int)scale]);
				_HUD.GetStatic(IDC_STATUS)->SetText(buf);

			}
			else
			// destroy
			{
				result = _pScene->destroyAmbOcclMaps();
				assert(result);

				_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPS)->SetText(TEXT("Generate AOMaps(G)"));
				_HUD.GetButton(IDC_SAVE_AMBOCCLMAPS)->SetEnabled(false);
				// enable the uv generating button.
				_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPUVS)->SetEnabled(true);
				_bCreated = false;
			}

			// rebuild fx UIs.
			_pFXUI->clear();
			_pFXUI->rebuild(_pScene->getClump());
		}
		break;

	case IDC_SAVE_AMBOCCLMAPS:
		{
			bool result;
			result = _pScene->saveAmbOcclMaps("G:\\svn\\Alef\\BIN\\TEXTURE\\AmbOccl\\");
		}
		break;

	case IDC_GENERATE_AMBOCCLMAPUVS:
		{
			// get the size scale.
			Scene::AmbOcclMapSizeScale scale = Scene::AOMSS_ONE;
			if(GetAsyncKeyState('1')&0x8000)
				scale = Scene::AOMSS_QUATER;
			else if(GetAsyncKeyState('2')&0x8000)
				scale = Scene::AOMSS_HALF;
			else if(GetAsyncKeyState('3')&0x8000)
				scale = Scene::AOMSS_ONE;
			else if(GetAsyncKeyState('4')&0x8000)
				scale = Scene::AOMSS_DOUBLE;
			else if(GetAsyncKeyState('5')&0x8000)
				scale = Scene::AOMSS_QUADRUPLE;

			bool result;
			// scale parameter added.
			result = _pScene->createAmbOcclMaps(true, scale);
			assert(result);

			RpAtomic *cur_atomic, *end_atomic, *next_atomic;

			if(_pScene->getClump()->atomicList)
			{
				cur_atomic = _pScene->getClump()->atomicList;
				end_atomic = cur_atomic;
				do
				{
					RpAtomic *pAtomic = cur_atomic;
					next_atomic = cur_atomic->next;

					// check, check, check!
					assert(RpGeometryGetVertexTexCoords(RpAtomicGetGeometry(pAtomic), (RwTextureCoordinateIndex)2));

					/* Destroy the PolySet array associated with the geometry */
					AmbOcclMapGeometryData *geometryData = RPAMBOCCLMAPGEOMETRYGETDATA(RpAtomicGetGeometry(pAtomic));
					if(geometryData->PolySetArray != NULL)
					{
						RwInt32 j;

						/* Release the polysets */
						for(j = 0; j < geometryData->numSets; ++j)
						{
							rwSListDestroy(geometryData->PolySetArray[j].members);
						}
				        
						RwFree(geometryData->PolySetArray);
						geometryData->PolySetArray = NULL;
					}

					/* Onto the next atomic */
					cur_atomic = next_atomic;
				}
				while(cur_atomic != end_atomic);
			}

			// report the size scale.
			const TCHAR *scaleText[] = { _T("quater"), _T("half"), _T("one"), _T("double"), _T("quadruple") };
			TCHAR buf[256];
			_stprintf(buf, _T("Ambient occlusion map UVs generated(size scale = %s)."), scaleText[(int)scale]);
			_HUD.GetStatic(IDC_STATUS)->SetText(buf);

			// You should reload the rws file so as to preview ambient occlusion maps.
			_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPS)->SetEnabled(false);
		}
		break;

	case IDC_REMOVE_FX:
		{
			RpAtomic *cur_atomic, *end_atomic, *next_atomic;

			if(_pScene->getClump()->atomicList)
			{
				cur_atomic = _pScene->getClump()->atomicList;
				end_atomic = cur_atomic;
				do
				{
					RpAtomic *pAtomic = cur_atomic;
					next_atomic = cur_atomic->next;

					RpAtomicFxDisable(pAtomic);
					RpAtomicFxRelease(pAtomic);

					/* Onto the next atomic */
					cur_atomic = next_atomic;
				}
				while(cur_atomic != end_atomic);
			}

			_pFXUI->clear();
			bool hasFX = _pFXUI->rebuild(_pScene->getClump());
			assert(!hasFX);

			_HUD.GetStatic(IDC_STATUS)->SetText(TEXT("Removed All FXs."));
			_HUD.GetButton(IDC_REMOVE_FX)->SetEnabled(false);
		}
		break;
	}
}

static RwBool initialize()
{
	if(RsInitialize())
	{
		if(!RsGlobal.maximumWidth)
		{
			RsGlobal.maximumWidth = DEFAULT_SCREEN_WIDTH;
		}

		if(!RsGlobal.maximumHeight)
		{
			RsGlobal.maximumHeight = DEFAULT_SCREEN_HEIGHT;
		}

		RsGlobal.appName = TEXT("MatD3DFxTool");

		DXUTGetGlobalDialogResourceManager();
		
		RsSetMsgProcCB(UIMsgProc);

		return TRUE;
	}

	return FALSE;
}

static RwBool initializeUI()
{
	_HUD.SetCallback(onGUIEvent); int iY = 10; 
	_HUD.AddStatic(IDC_FPS, TEXT("fps"), 20, 20, 50, 30);
	_HUD.GetStatic(IDC_FPS)->SetTextColor(D3DCOLOR_ARGB( 255, 255, 0, 0 ));
	_HUD.GetControl(IDC_FPS)->GetElement(0)->dwTextFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;
	_HUD.AddButton(IDC_OPEN, TEXT("Open...(O)"), 20, 50, 100, 23, TEXT('O'));
	_HUD.AddButton(IDC_SAVE, TEXT("Save...(S)"), 20, 80, 100, 23, TEXT('S'));
	_HUD.GetButton(IDC_SAVE)->SetEnabled(false);
	_HUD.AddStatic(IDC_STATUS, TEXT("Ready"), 20, RsGlobal.maximumHeight-25, 500, 25);
	_HUD.GetControl(IDC_STATUS)->GetElement(0)->dwTextFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;
	_HUD.AddCheckBox(IDC_PLAY_ANIM, TEXT("Play animation(A)"), 20, 110, 150, 23, true, TEXT('A'), false);
	_HUD.GetCheckBox(IDC_PLAY_ANIM)->SetEnabled(false);
	_HUD.AddCheckBox(IDC_WIREFRAME_ONSELECTION, TEXT("Wireframe on selection(W)"), 20, 140, 200, 23, true, TEXT('W'), false);
	_HUD.AddButton(IDC_AMBIENT_LIGHT, TEXT("Ambient light...(M)"), 20, 170, 150, 23, TEXT('M'));
	_HUD.AddButton(IDC_DIRECTIONAL_LIGHT, TEXT("Directional light...(D)"), 20, 200, 150, 23, TEXT('D'));
	_HUD.AddButton(IDC_MERGE_TEXTURES, TEXT("Merge textures...(T)"), 20, 230, 150, 23, TEXT('T'));
	_HUD.GetButton(IDC_MERGE_TEXTURES)->SetEnabled(false);
	_HUD.AddButton(IDC_GENERATE_AMBOCCLMAPS, TEXT("Generate AOMaps(G)"), 20, 260, 150, 23, TEXT('G'));
	_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPS)->SetEnabled(false);
	_HUD.AddButton(IDC_SAVE_AMBOCCLMAPS, TEXT("Save AOMaps...(V)"), 20, 290, 150, 23, TEXT('V'));
	_HUD.GetButton(IDC_SAVE_AMBOCCLMAPS)->SetEnabled(false);
	_HUD.AddCheckBox(IDC_ROTATE_LIGHT, TEXT("Rotate light(R)"), 20, 320, 150, 23, false, TEXT('R'), false);
	_HUD.AddButton(IDC_GENERATE_AMBOCCLMAPUVS, TEXT("Generate AOMap UVs(U)"), 20, 350, 150, 23, TEXT('U'));
	_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPUVS)->SetEnabled(false);
	_HUD.AddCheckBox(IDC_SHOW_BACKGROUND, TEXT("Show background(B)"), 20, 380, 150, 23, true, TEXT('B'), false);
	if(_pScene && _pScene->getBackTex())
		_HUD.GetCheckBox(IDC_SHOW_BACKGROUND)->SetEnabled(true);
	else
		_HUD.GetCheckBox(IDC_SHOW_BACKGROUND)->SetEnabled(false);
	_HUD.AddButton(IDC_REMOVE_FX, TEXT("Remove FX(X)"), 20, 410, 100, 23, TEXT('X'));
	_HUD.GetButton(IDC_REMOVE_FX)->SetEnabled(false);

	_HUD.AddStatic(IDC_MODEL_STATS, TEXT(""), 200, 50, RsGlobal.maximumWidth-400, RsGlobal.maximumHeight-100);
	_HUD.GetControl(IDC_MODEL_STATS)->GetElement(0)->dwTextFormat = DT_LEFT|DT_TOP|DT_WORDBREAK;
	_HUD.GetControl(IDC_MODEL_STATS)->SetVisible(false);

    return TRUE;
}

static RwTexture *textureReadCB(const RwChar *name, const RwChar *maskName)
{
	RwTexture *texture;

	texture = RwD3D9DDSTextureRead(name, NULL);
	if(texture)
	{
		RwTextureSetName(texture, name);
		return texture;
	}
	if(_originalTextureReadCB)
	{
		texture = _originalTextureReadCB(name, maskName);
		if(texture)
			RwTextureSetName(texture, name);
		return texture;
	}
	return NULL;
}

static RwBool initialize3D(void *param)
{	
	if(!RsRwInitialize(param))
	{
		RsErrorMessage(RWSTRING("Error initializing RenderWare."));

		return FALSE;
	}

	// set cull mode to none.
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void *)rwCULLMODECULLNONE);

	_pCharset = RtCharsetCreate(&_foregroundColor, &_backgroundColor);
	if(_pCharset == NULL)
	{
		RsErrorMessage(RWSTRING("Cannot create raster charset."));

		return FALSE;
	}

    // Call the GUI resource device created function
	HRESULT hr;
    hr = DXUTGetGlobalDialogResourceManager()->OnCreateDevice((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice());
	assert(SUCCEEDED(hr));
	hr = DXUTGetGlobalDialogResourceManager()->OnResetDevice();
	assert(SUCCEEDED(hr));
	
    _oldD3D9ReleaseDeviceCB = _rwD3D9DeviceGetReleaseCallback();
	_oldD3D9RestoreDeviceCB = _rwD3D9DeviceGetRestoreCallback();
	_rwD3D9DeviceSetReleaseCallback(newD3D9ReleaseDeviceCB);
    _rwD3D9DeviceSetRestoreCallback(newD3D9RestoreDeviceCB);

	_pScene = new Scene;
	if(NULL == _pScene)
	{
		RsErrorMessage(RWSTRING("Error initializing the scene."));

		return FALSE;
	}

	_pFXUI = new FXUI;
	if(NULL == _pFXUI)
	{
		RsErrorMessage(RWSTRING("Error initializing the FXUI."));

		return FALSE;
	}
	int w = RwRasterGetWidth(RwCameraGetRaster(_pScene->getCamera()));
	int h = RwRasterGetHeight(RwCameraGetRaster(_pScene->getCamera()));
	_pFXUI->resize(w, h);

	if(!initializeUI())
	{
		RsErrorMessage(RWSTRING("Error initializing UI."));

		return FALSE;
	}

    _originalTextureReadCB = RwTextureGetReadCallBack();
    RwTextureSetReadCallBack(textureReadCB);

	return TRUE;
}


static void terminate3D()
{
	if(_pScene)
	{
		delete _pScene;
		_pScene = NULL;
	}

	if(_pFXUI)
	{
		delete _pFXUI;
		_pFXUI = NULL;
	}

    DXUTGetGlobalDialogResourceManager()->OnLostDevice();
    DXUTGetGlobalDialogResourceManager()->OnDestroyDevice();

	if(_pCharset)
	{
		RwRasterDestroy(_pCharset);
	}

	RsRwTerminate();

	return;
}

static RwBool attachPlugins()
{
	if(!RpWorldPluginAttach())
	{
		return FALSE;
	}

	if(!RpSkinPluginAttach())
	{
		RsErrorMessage(RWSTRING("Skin plugin attach failed."));

		return FALSE;
	}

	if(!RpUserDataPluginAttach())
	{		
		return FALSE;
	}

	if(!RpMaterialD3DFxPluginAttach())
	{
		RsErrorMessage(RWSTRING("D3DFx plugin attach failed."));

		return FALSE;
	}

	if(!RpCollisionPluginAttach())
	{
		return FALSE;
	}

	if(!RtAnimInitialize())
	{
		return FALSE;
	}

	if(!RpHAnimPluginAttach())
	{
		return FALSE;
	}

	if(!RpRandomPluginAttach())
	{
		return FALSE;
	}
	if(!RpAmbOcclMapPluginAttach())
	{
		return FALSE;
	}
	if(!RpPVSPluginAttach())
	{
		return FALSE;
	}
	
	return TRUE;
}

static void displayHUD(float fElapsedTime)
{
    TCHAR caption[128];    

    if(_bShowFPS)
    {
        _stprintf(caption, TEXT("fps: %03d"), _nFramesPerSecond);

		_HUD.GetStatic(IDC_FPS)->SetText(caption);	
    }

	_HUD.OnRender(fElapsedTime);

    return;
}

// Draw the 2D background.
static void drawBack()
{
	if(!_bShowBackground || NULL == _pScene->getBackTex())
		return;

	int w = RwRasterGetWidth(RwCameraGetRaster(_pScene->getCamera()));
	int h = RwRasterGetHeight(RwCameraGetRaster(_pScene->getCamera()));

	RwIm2DVertex vertices[4];

	RwReal FarZ = RwIm2DGetFarScreenZ();
	RwReal recipCameraZ = 1.0f / RwCameraGetFarClipPlane(_pScene->getCamera());
	for(int i=0;i<4; ++i)
	{
		vertices[i].emissiveColor = 0xffffffff;
		vertices[i].z = FarZ;
		vertices[i].rhw = recipCameraZ;
	}
	
	vertices[0].x = 0;
	vertices[0].y = 0; 
	vertices[0].u = 0;
	vertices[0].v = 0;

	vertices[1].x = w;
	vertices[1].y = 0; 
	vertices[1].u = 1.0f;
	vertices[1].v = 0;

	vertices[2].x = w;
	vertices[2].y = h; 
	vertices[2].u = 1.0f;
	vertices[2].v = 1.0f;

	vertices[3].x = 0;
	vertices[3].y = h; 
	vertices[3].u = 0;
	vertices[3].v = 1.0f;

	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (PVOID)RwTextureGetRaster(_pScene->getBackTex()));

	RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, vertices, 4);
}

static void render(float fElapsedTime)
{
	RwCameraClear(_pScene->getCamera(), &_backgroundColor,
		rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

	if(RwCameraBeginUpdate(_pScene->getCamera()))
	{
		drawBack();

		if(_pScene->getClump())
		{
			RpClumpRender(_pScene->getClump());
		}
		
		displayHUD(fElapsedTime);

		_pFXUI->render(fElapsedTime);

		RwCameraEndUpdate(_pScene->getCamera());
	}

	RsCameraShowRaster(_pScene->getCamera());

	++_cFrame;

	return;
}

// progress callback for ambient occlusion maps
static RwBool illuminateProgressCB(RwInt32 message, RwReal value)
{
	TCHAR buf[32];
	_stprintf(buf, TEXT("%03.1f%%(%03d/%03d)"), value, _pScene->m_lightingSession.startObj+1, _pScene->m_lightingSession.totalObj);
	_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPS)->SetText(buf);

	render(0.03f);

	return TRUE;
}

static void idle()
{
	static IdleState state = {
		(RwUInt32) 0 /*  Calls */,
		(RwUInt32) 0 /*  LastFrameTime */,
		(RwUInt32) 0 /*  LastAnimTime */
	};

	RwUInt32 thisTime;
	RwReal delta;

	if(!state.m_cCall++ )
	{
		/*
		 * Initialize the timers variables that control clump animation and
		 * estimating the number of rendered frames per second...
		 */
		state.m_iLastFrameTime = RsTimer();
	}

	thisTime = RsTimer();

	delta = (thisTime - state.m_iLastAnimTime) * 0.001f;
	state.m_iLastAnimTime = thisTime;

	/*
	 * Has a second elapsed since we last updated the FPS...
	 */
	if(thisTime > (state.m_iLastFrameTime + 1000))
	{
		/*
		 * Capture the frame counter...
		 */
		_nFramesPerSecond = _cFrame;

		/*
		 * ...and reset...
		 */
		_cFrame = 0;

		state.m_iLastFrameTime = thisTime;
	}

	_pScene->updateCamera();

	updateClumpControl(delta);

	if(_HUD.GetCheckBox(IDC_PLAY_ANIM)->GetChecked())
		_pScene->updateAnimation(delta);

	//@{ Jaewon 20041216
	if(_HUD.GetCheckBox(IDC_ROTATE_LIGHT)->GetChecked())
	// rotate the main light.
	{
		RpLight *pLight = _pScene->getMainLight();
		RwFrame *pFrame = RpLightGetFrame(pLight);
		RwV3d yaxis = { 0.0f , 1.0f , 0.0f };
		RwFrameRotate(pFrame, &yaxis, delta*90.0f, rwCOMBINEPOSTCONCAT);
	}
	//@} Jaewon

	render(delta);

	//@{ Jaewon 20041217
	// incremental illumination
	if(_bIllum)
	{
		if(_pScene->m_lightingSession.totalObj > 0
			&& _pScene->m_lightingSession.startObj >= _pScene->m_lightingSession.totalObj)
		{
			_bIllum = false;
			_bCreated = true;
			_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPS)->SetText(TEXT("Destroy AOMaps(G)"));
			_HUD.GetButton(IDC_OPEN)->SetEnabled(true);
			_HUD.GetButton(IDC_SAVE_AMBOCCLMAPS)->SetEnabled(true);
			//@{ Jaewon 20050112
			// disable the uv generating button.
			_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPUVS)->SetEnabled(false);
			//@} Jaewon
		}
		bool result = _pScene->computeAmbOcclMaps(illuminateProgressCB);
		assert(result);
	}
	//@} Jaewon

	return;
}

//@{ Jaewon 20041103
void toggleModelStatsDisplay()
{
	static TCHAR buf[10240];

	CDXUTStatic *pStatic = (CDXUTStatic*)(_HUD.GetControl(IDC_MODEL_STATS));

	if(pStatic == NULL)
		return;

	if(pStatic->GetVisible())
	{
		pStatic->SetVisible(false);
	}
	else
	{
		pStatic->SetText(_pScene->getStatsString());
		pStatic->SetVisible(true);
	}
}
//@} Jaewon

RsEventStatus AppEventHandler(RsEvent event, void *param)
{
	switch(event)
	{
		case rsINITIALIZE:
		{
			return initialize() ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsREGISTERIMAGELOADER:
		{
			RwImageRegisterImageFormat("ras", RtRASImageRead, RtRASImageWrite);
			RwImageRegisterImageFormat("bmp", RtBMPImageRead, RtBMPImageWrite);
			RwImageRegisterImageFormat("png", RtPNGImageRead, RtPNGImageWrite);
			RwImageRegisterImageFormat("tif", RtTIFFImageRead, NULL);

			return (rsEVENTPROCESSED);
		}

		case rsCAMERASIZE:
		{
			CameraSize(_pScene->getCamera(), 
				(RwRect *)param, _pScene->getCurrentViewWindow(), DEFAULT_ASPECTRATIO);

			return rsEVENTPROCESSED;
		}

		case rsRWINITIALIZE:
		{
			//@{ Jaewon 20041104
			// brand new exception handler - XCrashReport
			_originalExceptionHandler = SetUnhandledExceptionFilter(xCrashExceptionHandler);
			//@} Jaewon
			
			return initialize3D(param) ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsRWTERMINATE:
		{
			terminate3D();

			//@{ Jaewon 20041028
			// brand new exception handler - XCrashReport
			// restore the original handlers.
			if(_originalExceptionHandler)
			{
				SetUnhandledExceptionFilter(_originalExceptionHandler);
				_originalExceptionHandler = NULL;
			}
			//@} Jaewon

			return rsEVENTPROCESSED;
		}

		case rsPLUGINATTACH:
		{
			return attachPlugins() ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsINPUTDEVICEATTACH:
		{
			attachInputDevices();

			return rsEVENTPROCESSED;
		}

		case rsFILELOAD:
		{
			bool bOK = false;

			if(strstr((const TCHAR *)param, TEXT(".rws")) ||
			   strstr((const TCHAR *)param, TEXT(".RWS")))
			{
				if(_bCreated)
					onGUIEvent(0, IDC_GENERATE_AMBOCCLMAPS, NULL);
				
				const char* buf;
				buf = ( char* )param;
				if( strlen( buf ) )
					bOK = _pScene->loadRWS((const char *)buf);
				if(bOK)
				{
					const TCHAR *path = (const TCHAR *)param;
					size_t len = strlen(path);
					while(len>0)
					{
						--len;
						if(path[len] == '\\')
							break;
					}
					strcpy(_openFileName, path+len+1);
					//@} Jaewon

					// rebuild fx UIs.
					_pFXUI->clear();
					
					onGUIEvent( 0 , IDC_GENERATE_AMBOCCLMAPUVS , NULL );

					// It returns true if the clump has fx.
					bool hasFX = _pFXUI->rebuild(_pScene->getClump());
					//@} Jaewon

					TCHAR windowText[1024];

					strcpy(windowText, RsGlobal.appName);
					strcat(windowText, TEXT(" - "));
					strcat(windowText, (const TCHAR *)param);

					buf = windowText;
					RsWindowSetText(buf);

					// output the status.
					_HUD.GetStatic(IDC_STATUS)->SetText(TEXT("Successfully loaded."));

					// now, enable the save button.
					if(false == _HUD.GetButton(IDC_SAVE)->GetEnabled())
						_HUD.GetButton(IDC_SAVE)->SetEnabled(true);

					//@{ Jaewon 20041201
					if(false == _HUD.GetButton(IDC_MERGE_TEXTURES)->GetEnabled())
						_HUD.GetButton(IDC_MERGE_TEXTURES)->SetEnabled(true);
					//@} Jaewon

					//@{ Jaewon 20041215
					if(false == _HUD.GetButton(IDC_GENERATE_AMBOCCLMAPS)->GetEnabled())
						_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPS)->SetEnabled(true);
					//@} Jaewon

					//@{ Jaewon 20050105
					if(false == _HUD.GetButton(IDC_GENERATE_AMBOCCLMAPUVS)->GetEnabled())
						_HUD.GetButton(IDC_GENERATE_AMBOCCLMAPUVS)->SetEnabled(true);
					//@} Jaewon

					// if there is a animation, enable the play button.
					if(_pScene->getAnimation())
						_HUD.GetCheckBox(IDC_PLAY_ANIM)->SetEnabled(true);
					else
						_HUD.GetCheckBox(IDC_PLAY_ANIM)->SetEnabled(false);

					//@{ Jaewon 20050825
					// If the clump has fx, enable the remove button.
					if(hasFX)
						_HUD.GetButton(IDC_REMOVE_FX)->SetEnabled(true);
					else
						_HUD.GetButton(IDC_REMOVE_FX)->SetEnabled(false);
					//@} Jaewon

					//@{ Jaewon 20041103
					// update the model stats.
					toggleModelStatsDisplay();
					toggleModelStatsDisplay();
					//@} Jaewon
				}
				else
				{
					TCHAR tmp[MAX_PATH];
					strcpy(tmp, TEXT("Cannot load "));
					strcat(tmp, (const TCHAR *)param);
					strcat(tmp, TEXT("!"));
					_HUD.GetStatic(IDC_STATUS)->SetText(tmp);
				}
				
				_bSpinOn = FALSE;
			}
			else if(strstr((const TCHAR *)param, TEXT(".fx")) ||
				    strstr((const TCHAR *)param, TEXT(".FX")) )
			{
				if(_pScene->getClump())
				{
					char buf[MAX_PATH];
					sprintf_s(buf, (const char *)param, strlen((const char *)param)+1);
					//@{ Jaewon 20050708
					// (GetAsyncKeyState(VK_MENU)&0x8000) check added
					bool bOK = _pFXUI->applyFx(_pScene->getClump(), buf, (GetAsyncKeyState(VK_MENU)&0x8000)?true:false);
					//@} Jaewon
					if(bOK)
					{
						_HUD.GetStatic(IDC_STATUS)->SetText(TEXT("Successfully applyed."));
						//@{ Jaewon 20050825
						// If the fx has been applied successfully, enable the remove button.
						_HUD.GetButton(IDC_REMOVE_FX)->SetEnabled(true);
						//@} Jaewon
					}
					else
					{
						TCHAR tmp[MAX_PATH];
						_tcscpy(tmp, TEXT("Cannot apply "));
						_tcscat(tmp, (const TCHAR *)param);
						_tcscat(tmp, TEXT("!"));
						_HUD.GetStatic(IDC_STATUS)->SetText(tmp);
					}
				}
			}
			//@{ Jaewon 20050708
			// Process the background image loading.
			else if(_tcsstr((const TCHAR *)param, TEXT(".bmp")) ||
					_tcsstr((const TCHAR *)param, TEXT(".BMP")) ||
					_tcsstr((const TCHAR *)param, TEXT(".png")) ||
					_tcsstr((const TCHAR *)param, TEXT(".PNG")) ||
					_tcsstr((const TCHAR *)param, TEXT(".tif")) ||
					_tcsstr((const TCHAR *)param, TEXT(".TIF")))
			{
				char buf[MAX_PATH];
				wcstombs(buf, (const wchar_t *)param, MAX_PATH-1);
				bool bOK = _pScene->loadBackTex(buf);
				if(bOK)
				{
					_HUD.GetStatic(IDC_STATUS)->SetText(TEXT("Successfully loaded."));
					_HUD.GetCheckBox(IDC_SHOW_BACKGROUND)->SetEnabled(true);
				}
				else
				{
					TCHAR tmp[MAX_PATH];
					_tcscpy(tmp, TEXT("Cannot load "));
					_tcscat(tmp, (const TCHAR *)param);
					_tcscat(tmp, TEXT("!"));
					_HUD.GetStatic(IDC_STATUS)->SetText(tmp);
					_HUD.GetCheckBox(IDC_SHOW_BACKGROUND)->SetEnabled(false);
				}
			}
			//@} Jaewon
			else
			{
				_HUD.GetStatic(IDC_STATUS)->SetText(TEXT("Unknown file type!"));
			}

			return bOK ? rsEVENTPROCESSED : rsEVENTERROR;
		}

		case rsIDLE:
		{
			idle();

			return rsEVENTPROCESSED;
		}

		default:
		{
			return rsEVENTNOTPROCESSED;
		}
	}
}
