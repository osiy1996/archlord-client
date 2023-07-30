#ifndef			_AGCMWATER_H_
#define			_AGCMWATER_H_

#include "AgcModule.h"

#include "AgcmMap.h"
#include "AgcmRender.h"

#include "ApmMap.h"

#include "AgcmOcTree.h"
#include "ApmOcTree.h"

#include "AgcmEventNature.h"

#include "AgcmLogin.h"
#include "AuPackingManager.h"

#include "AgpmReturnToLogin.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmWaterD" )
#else
#pragma comment ( lib , "AgcmWater" )
#endif
#endif

class AgcmUIOption;

// sector offset���� ������ ���� ���� offset in file
#define WMAP_WATER_BYTE_SIZE	16
#define WMAP_WAVE_BYTE_SIZE		40

#define WMAP_FILE_NUM			10	
#define	WMAP_STATUS_NUM			20

#define WMAP_HWATER_STATUS_NUM	20

#define	WAVE_VIEW_SECTOR_RANGE	2
#define WATER_REFLECT_TEX_SIZE	512

#define WATER_INI_STATUS_ID				"ID"
#define WATER_INI_STATUS_WTTYPE			"WTTYPE"
#define WATER_INI_STATUS_BMODEL1		"BMODE_L1"
#define WATER_INI_STATUS_BOPL1			"BOP_L1"
#define WATER_INI_STATUS_RENDERBMODEL1  "RENDERBMODE_L1"
#define WATER_INI_STATUS_WTFILEL1		"WATERFILE_L1"
#define WATER_INI_STATUS_REDL1			"RED_L1"
#define WATER_INI_STATUS_GREENL1		"GREEN_L1"
#define WATER_INI_STATUS_BLUEL1			"BLUE_L1"
#define WATER_INI_STATUS_ALPHAL1		"ALPHA_L1"
#define WATER_INI_STATUS_UL1			"U_L1"
#define WATER_INI_STATUS_VL1			"V_L1"
#define WATER_INI_STATUS_TSIZEL1		"TILESIZE_L1"
#define WATER_INI_STATUS_BMODEL2		"BMODE_L2"
#define WATER_INI_STATUS_BLENDSRCL2		"BLENDSRC_L2"
#define WATER_INI_STATUS_BLENDDESTL2	"BLENDDEST_L2"
#define WATER_INI_STATUS_WTFILEL2		"WATERFILE_L2"
#define WATER_INI_STATUS_UL2			"U_L2"
#define WATER_INI_STATUS_VL2			"V_L2"

#define WATER_INI_STATUS_WVFILE			"WAVEFILE"
#define WATER_INI_STATUS_WVWIDTH		"WAVEWIDTH"
#define WATER_INI_STATUS_WVHEIGHT		"WAVEHEIGHT"
#define WATER_INI_STATUS_WVLIFETIME		"WAVELIFETIME"
#define WATER_INI_STATUS_WVSCALEX		"WAVESCALEX"
#define WATER_INI_STATUS_WVSCALEZ		"WAVESCALEZ"
#define WATER_INI_STATUS_WVRED			"WAVERED"
#define WATER_INI_STATUS_WVGREEN		"WAVEGREEN"
#define WATER_INI_STATUS_WVBLUE			"WAVEBLUE"
#define WATER_INI_STATUS_WVMINNUM		"WAVEMINNUM"
#define WATER_INI_STATUS_WVMAXNUM		"WAVEMAXNUM"

typedef struct tag_WaterStatus			// Type1�� ���� �Ӽ� ����ü
{
	int		WaterType;

	int		BOP_L1;
	int		BMode_L1;
	int		RenderBMode_L1;				// Render ���� ��ȣ
	int		WaterTexID_L1;
	int		Red_L1;
	int		Green_L1;
	int		Blue_L1;
	int		Alpha_L1;
	float	U_L1;
	float	V_L1;
	int		TileSize_L1;
	
	int		BMode_L2;
	int		BModeSrc_L2;
	int		BModeDest_L2;
	int		WaterTexID_L2;
	float	U_L2;
	float	V_L2;
	
	int		WaveTexID;
	int		WaveWidth;
	int		WaveHeight;
	int		WaveLifeTime;
	float	WaveScaleX;
	float	WaveScaleZ;
	int		WaveRed;
	int		WaveGreen;
	int		WaveBlue;

	int		WaveMinNum;
	int		WaveMaxNum;
}WaterStatus;

typedef struct tag_HWaterStatus			// High Quality Water status
{
	float	Red;
	float	Green;
	float	Blue;
	float	DirX0;
	float	DirX1;
	float	DirX2;
	float	DirX3;
	float	DirY0;
	float	DirY1;
	float	DirY2;
	float	DirY3;
	float	Height0;
	float	Height1;
	float	Height2;
	float	Height3;
	float	Speed0;
	float	Speed1;
	float	Speed2;
	float	Speed3;
	float	TexX0;
	float	TexX1;
	float	TexY0;
	float	TexY1;
	float	ReflectionWeight;

	float	Alpha_Min;
	float	Alpha_Max;
	float	Alpha_DecreaseHeight;				// ���� ���� ���� ������..

	float	Height_Min;
	float	Height_Max;
	float	Height_DecreaseHeight;				// �ĵ� ���� ���� ������..

	int		VertexPerTile;				// Ÿ�ϴ� ���ؽ���

	int 	BumpTexID;
	int		FresnelTexID;

	//@{ Jaewon 20050706
	// Base color
	float BaseR;
	float BaseG;
	float BaseB;
	//@} Jaewon
	//@{ Jaewon 20050705
	// A index for ps.1.4 shader(0 or 1) 
	int Shader14ID;
	// A index for ps.1.1 shader(0 or 1)
	int Shader11ID;
	//@} Jaewon
}HWaterStatus;

typedef struct tag_WaterFileInfo
{
	char	strFileName[32];
	BOOL	bReload;
}WaterFileInfo;

enum WATERTYPE { WATER_RIVER = 0, WATER_SEA };

typedef	struct tag_WaveFileInfo
{
	char	strFileName[32];
	BOOL	bReload;						
}WaveFileInfo;

// WaterLayer1 - �ٴ� .. Just Texture Animation
typedef struct tag_water
{
	INT32		StatusID;

	INT16		XOffset;				// in Sector
	INT16		ZOffset;				// in Sector

	INT16		TileXLength;			// Plane x ����
	INT16		TileZLength;			// Plane z ����	

	FLOAT		Height;					// �� ����
	
	/////////////////////////////////////////////////// �� ���� file�� ����
	RwV3d		pos_origin;
	
	RpMaterial*	pMaterial;
	RpAtomic*	AtomicL1;				// layer1(dual uv transform)
	
	OcTreeRenderData2	stRenderData;

	tag_water*	next;
}Water;

struct HWATER_VERTEX {
	D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    D3DXVECTOR3 p2;									// Wave Height Scale(x), Water Alpha(y)
    FLOAT       tu1, tv1, tw1;
    D3DXVECTOR3 t;
};

typedef	struct tag_HWater
{
	INT32					StatusID;

	INT16					XOffset;				// in Sector
	INT16					ZOffset;				// in Sector

	INT16					TileXLength;			// Plane x ����
	INT16					TileZLength;			// Plane z ����	

	FLOAT					Height;
	RwSphere				BS;						// add ������ ���Ѵ�..���� ����
	
	///////////////////////////////////////////////// �� ���� file�� ����

	UINT32					iVertexOffset;
	LPDIRECT3DVERTEXBUFFER9	pvVB;
	LPDIRECT3DINDEXBUFFER9	pvIB;
	INT32					numTris;
	INT32					numVerts;

	tag_HWater*				next;
}HWater;

typedef struct tag_wave								// Decoration Layer
{
	INT32		StatusID;

	INT16		XOffset;							// in Sector
	INT16		ZOffset;							// in Sector

	INT32		Width;
	INT32		Height;
	INT32		SpawnCount;

	FLOAT		YRotAngle;					
	FLOAT		TranslationSpeed;

	RwV3d		pos_origin;
	
	////////////////////////////////////////////////// �� ���� file�� ����
	INT32		UpdateCount;
	
	RwV3d		pos;
	RwV3d		MoveAmount;
	RwSphere	BS;
		
	//*******************  For Animation  *********************************
	FLOAT		WaveScaleX;
	FLOAT		WaveScaleZ;
	FLOAT		WaveScaleXOffset;
	FLOAT		WaveScaleZOffset;
	
	FLOAT		WaveHeight;
	INT32		WaveDir;

	FLOAT		Alpha;
	FLOAT		AlphaDiff;
	INT32		AlphaDir;
	
	RwMatrix		mat_final;
	RwIm3DVertex	vert[6];

	tag_wave*	next;
}Wave;

#define		DRAW_WAVE_NODE_SIZE		8
typedef struct tag_DrawWave
{
	Wave*			wave;
	tag_DrawWave*	next;
}DrawWave;

#define		DRAW_HWATER_NODE_SIZE		8
typedef struct tag_DrawHWater
{
	HWater*			hwater;
	tag_DrawHWater*	next;
}DrawHWater;

typedef struct tag_Sector
{
	INT32					SectorXIndex;
	INT32					SectorZIndex;

	INT32					Detail;					// SECTOR_LOWDETAIL - 1 , SECTOR_HIGHDETAIL - 2
	bool					AddedRenderWave;		// Wave �ߺ� add������
	bool					AddedRenderWater;		// Water �ߺ� add������
	bool					AddedRenderHWater;		// HWater �ߺ� add������
		
	Water*					WaterList;
	Wave*					WaveList;

	//High Quality Water
	HWater*					HWaterList;

	tag_Sector*				next;
}Sector;

typedef enum
{
	WATER_DETAIL_HIGH			= 0,
	WATER_DETAIL_MEDIUM			= 1,
	WATER_DETAIL_LOW			= 2,
	WATER_DETAIL_OFF
} enumWaterDetail;

/*#define			WATER_FO_NUM		8
// �ִ� �������� LRU �˰��� ���� �����Ѵ�. fileopen�� ���̱� ���ؼ� handle���� .. FOList == FileOpenedList ^^..
typedef struct tag_WaterFOList
{
	INT32					lx;
	INT32					lz;

	HANDLE					fd;
	tag_WaterFOList*		next;
}WaterFOList;*/

typedef enum
{
	WATER_JOB_ADDWAVE = 0,
	WATER_JOB_REMOVEWAVE,
	WATER_JOB_ADDWATER,
	WATER_JOB_MAKEANDADDHWATER,
	
	WATER_JOB_WAVEADDTOSECTOR,
	WATER_JOB_WATERADDTOSECTOR,
	WATER_JOB_SECTORADDTOSECTORLIST,

	WATER_JOB_REMOVESECTOR,
	// sector ������ ����
	WATER_JOB_REMOVED												// sector release�� job
} enumWaterJobType;

typedef struct tag_WaterJobQueue
{
	enumWaterJobType	type;
	void*				data1;							// �ظ��ϸ� sector ���� (sector release �˻��)
	void*				data2;
} WaterJobQueue;

class AgcmWater : public AgcModule
{
public:
	static AgcmWater *	m_pThis		;
	
	AgcmMap*			m_pcsAgcmMap	;
	AgcmRender*			m_pcsAgcmRender	;
	AgcmOcTree*			m_pcsAgcmOcTree	;
	ApmOcTree*			m_pcsApmOcTree	;
	AgcmEventNature*	m_pcsAgcmEventNature;
	AgpmReturnToLogin*	m_pcsAgpmReturnToLogin;
	AgcmUIOption*		m_pcsAgcmUIOption;

	INT32				m_iTemp;					// wave �߰��� icameraindex��

	Sector*				m_listSector  ;
	INT32				m_iSectorCount;

	UINT32				m_ulTickDiff;
	UINT32				m_uiLastTick;

	enumWaterDetail		m_iWaterDetail;

	DrawWave*			m_listDrawWave;
	DrawHWater*			m_listDrawHWater;
		
	RwTexture*			m_prWaterT[WMAP_FILE_NUM];							// ��ũ����Ʈ�� ���͹ٴ�
	RwTexture*			m_prWaveT[WMAP_FILE_NUM];							// �ĵ�
	
	RwV2d				m_TexuvL1[WMAP_STATUS_NUM];							// uv�̵���
	RwV2d				m_TexuvL2[WMAP_STATUS_NUM];							// uv�̵���
	RwMatrix			m_TexMatL1[WMAP_STATUS_NUM];					
	RwMatrix			m_TexMatL2[WMAP_STATUS_NUM];							
	
	WaterStatus			m_stWaterStatus[WMAP_STATUS_NUM];
	WaterFileInfo		m_stWaterFInfo[WMAP_FILE_NUM];
	WaveFileInfo		m_stWaveFInfo[WMAP_FILE_NUM];

	RwTexture*			m_prHQBumpSrc[WMAP_FILE_NUM];	// HQ Water Bump Src
	LPDIRECT3DTEXTURE9	m_prHQBump[WMAP_FILE_NUM];		// HQ Water Bump(U8V8 ����)
	RwTexture*			m_prHQFresnel[WMAP_FILE_NUM];						// HQ Water Fresnel
	//@{ Jaewon 20050704
	// Normalization cube map texture
	RwTexture			*m_pNormCubeMap;
	//@} Jaewon

	HWaterStatus		m_stHWaterStatus[WMAP_HWATER_STATUS_NUM];
	WaterFileInfo		m_stHQBumpFInfo[WMAP_FILE_NUM];
	WaterFileInfo		m_stHQFresnelFInfo[WMAP_FILE_NUM];

	INT32				m_iWaterFileNum;										//WMAP_L1T1_FILE_NUM�� �ʰ��� �� ����
	INT32				m_iWaveFileNum;

	INT32				m_iHQBumpFileNum;
	INT32				m_iHQFresnelFileNum;

	INT32				m_iMaxWaterIDUsed;
	INT32				m_iMaxHWaterIDUsed;

	FLOAT				m_fSinTable[360];									// 0 - 360' �� sin table�� �̸� ���´�.

	IDirect3DDevice9*			m_pCurD3D9Device;
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDecl;

	RwCamera*			m_pMainCamera;
	RwCamera*			m_pReflectionCamera;
	RwRaster*			m_pReflectionRaster;								
	RwRaster*			m_pReflectionRasterZ;
	RwTexture*			m_pReflectionTexture;
	FLOAT				m_fReflectionWaterHeight;
	D3DMATRIX			m_matReflectionTrafoMatrix;
	D3DMATRIX			m_matReflectionViewProjection;

	BOOL				m_bReflectionUpdate;
	INT32				m_iReflectionFreq;
	INT32				m_iReflectionUpdateCount;
	
	BOOL				m_bEnableHWater;
	
	//@{ Jaewon 20050705
	// There are 2 versions of the shader pair.
	PVOID				m_pHWaterPS11[2];
	PVOID				m_pHWaterVS11[2];
	PVOID				m_pHWaterPS14[2];
	PVOID				m_pHWaterVS14[2];
	//@} Jaewon

	FLOAT				m_fTime;
	BOOL				m_bEnableUserClipPlane;

	// oblique near clip set��
	D3DXMATRIX			m_matSetProj;
	D3DXMATRIX			m_matSetViewProj;

	D3DXMATRIX			m_matBackupProj;
	D3DXMATRIX			m_matBackupViewProj;

	ApCriticalSection	m_csCSection;
	WaterJobQueue*		m_pJobQueue;
	INT32				m_iJobQueueCount;
	INT32				m_iJobQueueMaxCount;

	//@{ Jaewon 20050705
	// Make it use the ps.1.1 shader irrelevant of the hardware capability.
	bool m_bForce11Shader;
	//@} Jaewon

public:
	AgcmWater();
	~AgcmWater();

public:
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	void	OnCameraStateChange(CAMERASTATECHANGETYPE	ctype);

	inline void	SetWorldCamera(RwCamera*	camera){ m_pMainCamera = camera; }

	// d3d stuff
	void onLostDevice();
	void onResetDevice();

	void		SetWaterDetail(enumWaterDetail	detail);

	static void	OnLoadSectorWaterLevelCallback		( ApWorldSector* pSector , INT32 nDetail );
	static void	OnReleaseSectorWaterLevelCallback	( ApWorldSector* pSector , INT32 nDetail );

	INT32		GetWaterType(FLOAT x,FLOAT z);								// �ش� Ÿ���� watertype�� �����Ѵ�. -1�̸� ����
	BOOL		GetWaterHeight(FLOAT x,FLOAT z , FLOAT * pHeight );			// ������ ���Ͱ� �ִ��� �Ǻ�.

	Water*		GetWater(FLOAT x,FLOAT z);

	static BOOL	CB_W1Update ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_W1Release ( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL	CB_W2Update ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_W2Render ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_W2Release ( PVOID pData, PVOID pClass, PVOID pCustData );

	//@{ 2006/11/17 burumal
	///*
	static BOOL CB_DistCorrect( PVOID pDistFloat, PVOID pNull1, PVOID pNull2);
	//*/
	//@}

	static BOOL	CB_POST_RENDER_REFLECTION_DRAW ( PVOID pData, PVOID pClass, PVOID pCustData );	// HQWater �ݻ�� ����
	static BOOL	CB_POST_RENDER2 ( PVOID pData, PVOID pClass, PVOID pCustData );			// �ĵ� �׸���.

	static BOOL	CBDeleteAllQueue ( PVOID pData, PVOID pClass, PVOID pCustData );

	void		WaveUpdate(Wave*	wave);
	void		WaveDataSet(Wave*	wave);					// ���Ͽ��� �ε��� data�� wave data ���� ä���.

	void		DeleteAll(Sector*	sec);					// �ش� ������ ��� ���� �ĵ��� �����.
	void		ReleaseWater(Water*	water);					// water���� ��ü�� ����
	void		ReleaseWave(Wave*	wave);					// wave���� ��ü�� ����

	void		AddWaterToRender(Water*	water);
	void		RemoveWaterFromRender(Water*	water);
	
	void		AddWaveToRender(Wave*	wave);
	void		RemoveWaveFromRender(Wave*	wave);

	void		RemoveAll();

	void		JobQueueAdd(enumWaterJobType		type,PVOID data1,PVOID data2);		// ���� data�˻��ؼ� skipó��
	void		JobQueueRemove(INT32	six,INT32	siz);											// sector release �ҷ��� ��� 
	void		JobQueueRemoveAll();
		
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	High Quality Water Methods
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	PVOID		GetShader(const char* pSrc, BOOL bPixel = FALSE) const;
	BOOL		InitBumpMaps();

	void		ChangeHWaterStatus(int status_id);
	
	void		AddHWaterToRender(HWater*	hwater);
	void		RemoveHWaterFromRender(HWater*	hwater);
	void		ReleaseHWater(HWater*	hwater);

	static BOOL	CB_HWUpdate ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_HWRender ( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_HWRelease ( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL	CB_POST_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData );		// ȯ��ʰ� �ڱ� ĳ���� ��ο�..��.
	
	void		ChangeHWaterMode(BOOL	bOn);								// high water mode�� �׸����� TRUE,�ƴϸ� FALSE
	BOOL		MakeHWaterFromWater();										// ���� WaterList���� ���� HWater �����

	void		ChangeHWaterMode(BOOL	bOn,Sector*	pSec);								// high water mode�� �׸����� TRUE,�ƴϸ� FALSE
	BOOL		MakeHWaterFromWater(Sector*	pSec);										// ���� WaterList���� ���� HWater �����
	
	void		SetShaderCommonConstant();
	void		SetShaderConstant(INT32 StatusID);

	//void		SetReflectShaderCommonConstant();
	//void		SetReflectShaderConstant(RpAtomic*	atomic);

	void		RenderHQWater(HWater*	pHW);
	void		RenderHQWater();								// maptool ��

	BOOL		LoadHWaterStatusFromINI(char*	szFileName, BOOL bDecryption);
	BOOL		SaveHWaterStatusToINI(char*		szFileName, BOOL bEncryption);

	void		UpdateReflectionMap();

	float		sgn(float a);
	void		ModifyD3DProjectionMatrix(D3DXMATRIX& view,D3DXMATRIX& proj);

	//@{ Jaewon 20050608
	// OcClumpAtomicList -> RpAtomic
	static	void	ImmediateHWaterAtomicListRender(RpAtomic*	pList);
	//@} Jaewon
	static	void	ImmediateHWaterAtomicRender(RpAtomic*	atomic);
	static	void	ImmediateHWaterTerrainRender(RpAtomic*	atomic,INT32 SectorDist);

	void		SkinAtomicDraw(RpAtomic*	atomic);
				
	// Maptool��	
	BOOL		AddWater(FLOAT x,FLOAT z,FLOAT xlength,FLOAT zlength,FLOAT height,INT32 type,INT32	status_id=0);
	BOOL		DeleteWater(FLOAT x,FLOAT z);

	void		MakeWater(Water* pw,INT32	status_id,BOOL bAdd = TRUE);		
	void		ChangeWater(Water* pw,INT32 type,INT32 nDetail);				// �ش� Ÿ���� Ÿ�Ժ���

	BOOL		AddWave(Sector*	sec,FLOAT x,FLOAT z,FLOAT height,FLOAT dir,INT32 count,INT32 status_id);

	BOOL		MakeWave(Wave* pw,INT32 status_id,BOOL bAdd = TRUE);								// �ش� Ÿ�Ͽ� ���ڷ��̼� ���̾ �����.
	BOOL		DeleteWave(FLOAT x,FLOAT z);									// �ش� Ÿ���� ���ڷ��̼� ���̾ �����.

	void		AutoMakeWave(Sector* sec);//,INT32 SIndexX , INT32 SIndexZ);	// �ش� ������ wave ����(update) ��� L1�� ����

	void		SaveToFiles(char*	szDir);												// ���� ���͵��� ���Ͽ� ����

	BOOL		RemoveWaterRange( FLOAT fX , FLOAT fZ , FLOAT fWidth , FLOAT fHeight );

	BOOL		LoadStatusInfoT1FromINI(char*	szFileName, BOOL bDecryption);
	BOOL		SaveStatusInfoT1ToINI(char*		szFileName, BOOL bEncryption);

	void		ChangeStatus(int status_id);		//	�ش� status_id�� ��� atomic���� ... �ð� �����ɸ����� lock�Ұ���
	int			GetWaterStatus( FLOAT x, FLOAT z);

//	WaterFOList*		m_listFO;
//	INT32				m_iFOCount;

//	HANDLE		GetHandle(INT32 lx,INT32 lz);
	//void		CloseHandleList(INT32 lx,INT32 lz);
};

#endif       //_AGCMWATER_H_	