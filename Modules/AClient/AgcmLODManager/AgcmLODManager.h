#ifndef	_AGCMLODMANAGER_H_
#define	_AGCMLODMANAGER_H_

#include <ApBase/ApBase.h>

#include <rwcore.h>
#include <rpworld.h>
#include <rplodatm.h>
#include <rpusrdat.h>
#include <rpskin.h>
#include <AuExcelTxtLib/AuExcelTxtLib.h>
#include <AgcSkeleton/AcDefine.h>
#include <AgcModule/AgcModule.h>
#include <AcuObject/AcuObject.h>
#include <AgcmLODManager/AgcmLODList.h>

typedef enum eAgcmLODDistanceType
{
	E_AGCM_LOD_DISTANCE_TYPE_DEFAULT = 0,
	E_AGCM_LOD_DISTANCE_TYPE_L_TREE,
	E_AGCM_LOD_DISTANCE_TYPE_M_TREE,
	E_AGCM_LOD_DISTANCE_TYPE_S_TREE,
	E_AGCM_LOD_DISTANCE_TYPE_L_ROCK,
	E_AGCM_LOD_DISTANCE_TYPE_M_ROCK,
	E_AGCM_LOD_DISTANCE_TYPE_S_ROCK,
	E_AGCM_LOD_DISTANCE_TYPE_L_STRUCTURE,
	E_AGCM_LOD_DISTANCE_TYPE_M_STRUCTURE,
	E_AGCM_LOD_DISTANCE_TYPE_S_STRUCTURE,
	E_AGCM_LOD_DISTANCE_TYPE_NUM
} eAgcmLODDistanceType;

typedef enum eAgcmLODStreamReadResult
{
	E_AGCD_LOD_STREAM_READ_RESULT_ERROR = 0,
	E_AGCD_LOD_STREAM_READ_RESULT_PASS,
	E_AGCD_LOD_STREAM_READ_RESULT_READ
} eAgcmLODStreamReadResult;

#define AGCMLOD_CLUMP_MAX_ATOMIC	20

// stream ����
#define		AGCMLOD_STREAM_LOD_MAX_DISTANCE		"LOD_MAX_DISTANCE"
#define		AGCMLOD_STREAM_LOD_DISTANCE_TYPE	"LOD_TYPE_DISTANCE"
#define		AGCMLOD_STREAM_LOD_LEVEL			"LOD_LEVEL"
#define		AGCMLOD_STREAM_LOD_DISTANCE			"LOD_DISTANCE"
#define		AGCMLOD_STREAM_LOD_BOUNDARY			"LOD_BOUNDARY"
#define		AGCMLOD_STREAM_LOD_BILLBOARD_NUM	"LOD_BILLBOARD_NUM"
#define		AGCMLOD_STREAM_LOD_BILLBOARD_INFO	"LOD_BILLBOARD_INFO"

#define		AGCMLOD_DISTANCE_COUNT				10
#define		AGCMLOD_DISTANCE_TYPE_DEFAULT		"default"
#define		AGCMLOD_DISTANCE_TYPE_L_TREE		"type1"
#define		AGCMLOD_DISTANCE_TYPE_M_TREE		"type2"
#define		AGCMLOD_DISTANCE_TYPE_S_TREE		"type3"
#define		AGCMLOD_DISTANCE_TYPE_L_ROCK		"type4"
#define		AGCMLOD_DISTANCE_TYPE_M_ROCK		"type5"
#define		AGCMLOD_DISTANCE_TYPE_S_ROCK		"type6"
#define		AGCMLOD_DISTANCE_TYPE_L_STRUCTRUE	"type7"
#define		AGCMLOD_DISTANCE_TYPE_M_STRUCTRUE	"type8"
#define		AGCMLOD_DISTANCE_TYPE_S_STRUCTRUE	"type9"

#define		AGCMLOD_DISTANCE_LEVEL_1			"level 1"
#define		AGCMLOD_DISTANCE_LEVEL_2			"level 2"
#define		AGCMLOD_DISTANCE_LEVEL_3			"level 3"
#define		AGCMLOD_DISTANCE_LEVEL_4			"level 4"
#define		AGCMLOD_DISTANCE_LEVEL_5			"level 5"

// atomic index
#define		AGCMLOD_ATOMIC_INDEX				"LODIndex"

//@{ 2006/11/14 burumal
#define		AGCMLOD_ATOMIC_INDEX_EXTRA			"AtomicIndex"
//@}


// lod distance ����
#define		AGCMLOD_LOWER_DIST		(float)0.1f

// lod	triangle cut ����
#define		AGCMLOD_LOWER_TRINUM				500
#define		AGCMLOD_MAX_TRINUM					30000
#define		AGCMLOD_DEFAULT_STANDARD_DISTANCE	40000

//. 2006. 2. 8. Nonstopdj
//. ĳ���� �þ߿� ���� LOD Distance
#define		AGCMLOD_CHARACTER_STANDARD_DISTANCE	5000

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmLODManagerD" )
#else
#pragma comment ( lib , "AgcmLODManager" )
#endif
#endif

typedef struct SearchAtomicParams
{
	PVOID		m_pvClass;
	//@{ 2006/10/10 burumal
	/*
	RpAtomic	*m_pastAtomic[AGCMLOD_CLUMP_MAX_ATOMIC];
	RwInt32		m_anAtomicIndex[AGCMLOD_CLUMP_MAX_ATOMIC];
	*/
	RpAtomic	*m_pastAtomic[AGCMLOD_CLUMP_MAX_ATOMIC + 1];
	RwInt32		m_anAtomicIndex[AGCMLOD_CLUMP_MAX_ATOMIC + 1];
	//@}
	UINT16		m_unCount;
	INT32		m_lCheckPartID;
} SearchAtomicParams;

class AgcmLODManager:public AgcModule
{
public:
	static AgcmLODManager*	m_pThisAgcmLODManager	;
	BOOL					m_bForceZeroLevel		;

public:
	AgcmLODManager();
	~AgcmLODManager();

public:
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);

//	VOID	SetMainFrame(RwFrame *pFrame);
	VOID	SetDefaultDistance(UINT32 *pulDistance);
	// �Ÿ� ���� ���.
	BOOL	SetDistanceRate(AgcdLODData *pstAgcdLODData);

	//. 2006. 2. 8. Nonstopdj
	//. AgcmCharacter�� �Ÿ� �������
	BOOL	SetCharacterDistanceRate(AgcdLODData *pstAgcdLODData);

	// �ַ� ����ϴ� �ʹ�...
	BOOL	Initailize(RpClump *pstClump, RpClump *pstInitClump, BOOL bSetNextLevel = FALSE);
	BOOL	MakeLODLevel(RpClump *pstClump, CHAR *szLODDffPath, INT32 lLevel, RpClump *pstDummyClump = NULL, BOOL bTransformGeom = FALSE, BOOL bSetNextLevel = FALSE, INT32 lPartID = 0); // ���� Initailize�ϰ� ����ؾ� �Ѵ�.
	BOOL	MakeLODLevel_Billboard(RpClump *pstClump, CHAR *szBillDffPath, INT32 lLevel, RpClump *pstDummyClump = NULL);
	VOID	SetLODCallback(RpClump *pstClump);

	BOOL	IsSetLOD(RpClump *pstClump);

	// ���������� ����ϴ� �ʹ�...
	RwInt32		GetGeometryUsrDataInt(RpAtomic *pstAtomic, RwChar *szUsrName);
	RpClump		*ReadDFF(CHAR *szDFFPath);
	RpAtomic	*SetAtomic(SearchAtomicParams *pstParams, RpAtomic *pstAtomic);

	// Callback
	static  RwInt32		LODCallback(RpAtomic *atomic);
	static	RpAtomic	*SearchAtomicCB(RpAtomic *atomic, void *data);
	static	RpAtomic	*SearchAtomicCheckPartID(RpAtomic *atomic, void *data);

//	VOID	SetDistFact(FLOAT	val)		{ m_fDistFactor = val; if(m_fDistFactor > 1.0f) m_fDistFactor = 1.0f; else if(m_fDistFactor < AGCMLOD_LOWER_DIST ) m_fDistFactor = AGCMLOD_LOWER_DIST ; }

// class
	AgcmLODList		m_csLODList;

// datas	
	RwFrame			*m_pstMainFrame;
	UINT32			*m_pulDefaultDistance;

	// Tuner ���� ����( 0 - Object, 1- character(item) )
	UINT32			m_uiLODTriCutLine[2];			// �ﰢ�� ���� �ʰ��ϴ°� lod ����
	FLOAT			m_fLODDistFactor[2];			// frame�� �ȳ����� distance�� �Ÿ� ���ҿ�

	// Stream ����
	BOOL			StreamWrite(ApModuleStream *pcsStream, AgcdLOD *pstLOD);
	INT32			StreamRead(ApModuleStream *pcsStream, AgcdLOD *pstLOD);
	AgcdLODData		*GetLODData(AgcdLOD *pstLOD, INT32 lIndex, BOOL bAdd = TRUE);

	CHAR			*GetDistanceTypeName(UINT32 ulIndex);

	VOID			SetForceZeroLevel(BOOL bForce) { m_bForceZeroLevel = bForce; }

	void			FrameMatrixSave(RpAtomic*	atomic);		// ������ �ܰ� ����ó��
	void			FrameMatrixLoad(RpAtomic*	atomic);		// ������ �ܰ� ��Żó��

protected:
//	AuExcelTxtLib	m_csAuExcelTxtLib;
	UINT32			m_aulDistance[AGCMLOD_DISTANCE_COUNT][AGPDLOD_MAX_NUM];
	UINT32			m_ulStandardDistance;

	//. 2006. 2. 8. Nonstopdj
	UINT32			m_ulCharacterStandardDistance;

public:
	BOOL			ReadLODDistanceData(CHAR *szPathName, BOOL bDecryption);
	UINT32			*GetDistanceData(eAgcmLODDistanceType eType);

	// 100�� 1�����Դϴ�.
	UINT32			GetStarndardDistance()					{return m_ulStandardDistance;}
	VOID			SetStarndardDistance(UINT32 ulDistance) {m_ulStandardDistance = ulDistance;}

protected:
	INT32			SearchLODType(CHAR *szName);

private:
	class AgcmRender		*m_pcmRender;

	BOOL			m_bUseVShader;
	int				m_iNumtest;	

	UINT32			m_ulTick[3];
};

#endif