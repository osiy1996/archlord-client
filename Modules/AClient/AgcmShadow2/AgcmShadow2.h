#ifndef			_AGCMSHADOW2_H_
#define			_AGCMSHADOW2_H_

#include <AgcModule/AgcModule.h>

#define		SHADOW2_TEXTURE_SIZE	512
#define		SHADOW2_NODE_SIZE		8
#define		SHADOW2_RADIUS_WEIGHT	(0.707f)		// (0.707 -- 1.414(root2) * 0.5f(������))

struct Shadow2Node
{
	RpClump*		pClump;
	Shadow2Node*	next;
};

enum AgcmShadow2CallbackPoint
{
	AGCMSHADOW2_CB_ID_TEXTURE_UPDATE = 0,
};

class ApWorldSector;
struct SectorRenderList;

class AgcmShadow2 : public AgcModule
{
public:
	class AgcmRender*		m_pcRender;
	class ApmMap*			m_pcApmMap;
	class AgcmMap*			m_pcAgcmMap;
	class AgcmShadow*		m_pcShadow;
	class ApmOcTree*		m_pcsApmOcTree;
	class AgcmOcTree*		m_pcsAgcmOcTree;
		
	//Shadow Render Status
	RwRaster*			m_pcRenderRaster;
	RwTexture*			m_pcShadowTexture;
	
	RwTexture*			m_pcsDXTTexture1;
	RwRaster*			m_pstRasterTemp;
	RwTexture*			m_pcsDXTTexture2;

	BOOL				m_bUse1;

	RpLight*			m_pGLightAmbient;	// ����
	RpLight*			m_pGLightDirect	;
	RwCamera*			m_pWorldCamera	;
	RpWorld*			m_pWorld		;

	RwFrame	*			m_pFrameLight	;	// ������ Frame
		
	//Shadow Render Devices
	RwCamera*			m_pcShadowCamera2;

	INT32				m_iShadowTexUpdate;			// 0 - false, 1 - ��� , 2 - update

	UINT32				m_ulLastTick;
	UINT32				m_ulUpdateTick;				//2���� ������ �ڵ����� texture update��(light direction ����ǹǷ�)

	int					m_iDrawRange;
	ApWorldSector*		m_pWSector;

	RwMatrix			m_IMatrix;

	Shadow2Node*		m_listShadow2;

	INT32				m_lLoaderID;

	INT32				m_lShadowDimension;

	BYTE				m_cShadowBuffer[64][512];		// 512 ������ �ؽ��ĸ� bit������(0,1) ����..access�� 
	BYTE				m_cMaskShadowBuffer[8];

	INT32				m_nShadowDarkenValue;

private:
	BOOL				m_bEnableShadow;

public:
	AgcmShadow2();
	~AgcmShadow2();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);

	void	OnCameraStateChange( CAMERASTATECHANGETYPE ctype );

	void	AddWorldCamera( RwCamera* cam )				{	m_pWorldCamera = cam;		}
	void	AddWorldDirectLight( RpLight* light )		{	m_pGLightDirect = light;	}
	void	AddWorldAmbientLight( RpLight* light )		{	m_pGLightAmbient = light;	}
	void	AddRpWorld( RpWorld* world )				{	m_pWorld = world;			}

	void	BrightenShadow(RwRaster* pRaster);
	void	DarkerRaster(RwRaster* pRas);
	void	InvertRaster(RwRaster* pRas);
	BOOL	ShadowTextureRender();			// shadow texture �׸���.
	
	void	AddToShadow2List( RpAtomic* atomic );
	
	VOID	ApplyMatrix();

	void	UpdateShadowBuffer();			// ���� shadow texture�� shadow buffer�� set
	BOOL	GetShadowBuffer(RwV3d*	pos);	// ���ڷ� �ѱ� world pos�� �ش��ϴ� �ؽ��� ���۰��� 1���� �ƴ��� üũ 

	BOOL	SetCallbackTextureUpdate( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	void	ChangeDarkenValue( int nValue );
	void	ChangeLightAngle( float fValue );

	SectorRenderList*	GetSectorData( ApWorldSector *pSector );

	BOOL	IsEnable()		{	return m_bEnableShadow;		}
	BOOL	SetEnable( BOOL bEnable );

	static	BOOL		CB_SECTOR_CAHNGE( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL		CB_POST_RENDER( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL		CBBlitter(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static	RpClump*	ClumpCBSetShadowList(RpClump * clump, void *data);

private:
	void	RenderRaster( RwRaster* pRaster, RwUInt8 byVal );
};


#endif       //_AGCMSHADOW2_H_	