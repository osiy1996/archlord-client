#pragma once

#include <d3dx9core.h>
#include <AgcEngine.h>

#include "AgcmEff2.h"
#include "MainWindow.h"


#include "Axis.h"
#include "Grid.h"
#include "EffUt_HFMap.h"
#include "ShowFrm.h"
USING_EFFUT;

#include "AgcmFont.h"

extern AgcmEff2*			g_pAgcmEff2;
extern AgcmFont*			g_pAgcmFont;


typedef std::list<RwV3d>	STLLIST_RwV3d;

RwReal GetMapHeight(RwReal x, RwReal y, RwReal z);//����Ʈ ���� AgcmMap�� �߰� �Ǳ� ������ �̳��� ����.

class CDlgGridToolOption;
class MyEngine : public AgcEngine  
{
	friend class CDlgGridToolOption;
	friend RwReal GetMapHeight(RwReal x, RwReal y, RwReal z);
	
public:
	void	bSetDevice(LPDIRECT3DDEVICE9 lpDev) { m_lpDev=lpDev; };
	void	bLoadTex(LPCSTR fName, INT* pnWidth, INT* pnHeight, INT* pnDepth, LPCTSTR* szFmt, LPCTSTR szDir=NULL);

	HRESULT	bRenderTexPreview(HWND hWnd=NULL);
	HRESULT bRenderTexForUVSelect(HWND hWnd, RwTexture* pTex, float fWidth, float fHeight, INT nRow, INT nCol, INT nSelRow, INT nSelCol);
	
public:
	void	bOnOffAxisCone(BOOL OnOff)	{ m_cAxisWorld.bSetupShowCone(OnOff);}
	void	bOnOffEffFrame(BOOL OnOff);

public:
	BOOL		vPickPlane(RwV3d* pV3dPicked, const RwV3d& center, BOOL bXZ=TRUE);
public:
	VOID		bInitAccessory();
	VOID		bDestroyAcessory();

	CAxis*		bGetPtrFrmAxis() { return &m_cAxisFrm; };

	LPSHOWFRM bPickShowFrm(POINT ptMouse);
	VOID bMoveShowFrm(POINT ptMouse);
	VOID bLBtnUp() { m_lpPickedShowFrm = NULL; };
	VOID bRotShowFrm(int dx, int dy);
	VOID bBackToOrgShowFrm(void);

	VOID bSetFrmHeight(void);

private:
	INT32		vRenderAcessory();
	INT32		vRenderAxisGrid();
	INT32		vRenderHMap();

	BOOL		OnRenderPreCameraUpdate	();

public:
	MyEngine();
	virtual ~MyEngine();

	// Message
	BOOL				OnInit					();
	BOOL				OnAttachPlugins			();	// �÷����� ���� �� ���⼭
	RpWorld *			OnCreateWorld			(); // ���带 �����ؼ� �������ش�.
	RwCamera *			OnCreateCamera			( RpWorld * pWorld );	// ���� ī�޶� Create���ش�.
	BOOL				OnCreateLight			();	// ������ �����ؼ�. �� ������ �������ش�. m_pLightAmbient , m_pLightDirect
	AgcWindow *			OnSetMainWindow			();	// ����Ʈ �����츦 �������ִ� �޽���.. �����츦 �ʱ�ȭ�ؼ� ������ �ѱ�� �´�.

	BOOL				OnRegisterModule		();

	VOID				OnWorldRender			(); // World Render �κп��� AgcmRender�� ����Ѵ�.

	VOID				OnTerminate				();

	VOID				OnEndIdle				();

public:
	CGrid				m_cGrid1;
	CGrid				m_cGrid10;
	CAxis				m_cAxisWorld;
	CEffUt_HFMap<VTX_PNDT1>		m_cHeightMap;
	RwTexture*			m_pTexHeightMap;
	CAxis				m_cAxisFrm;
	LPSHOWFRM			m_lpPickedShowFrm;

	LPDIRECT3DDEVICE9	m_lpDev;
	RwTexture*			m_lpTexture;
};

extern MyEngine			g_MyEngine;
extern MainWindow		g_MainWindow;
