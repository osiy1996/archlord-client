#include "AgcmLODManager.h"
#include <AgcModule/AgcEngine.h>
#include <AcuRpMatD3DFx/AcuRpMatD3DFx.h>

#include <ApMemoryTracker/ApMemoryTracker.h>
#include <ApModule/ApModuleStream.h>
#include <AgcmRender/AgcmRender.h>

AgcmLODManager*	AgcmLODManager::m_pThisAgcmLODManager	= NULL;

static CHAR	 *g_paszLODDistanceTypeName[E_AGCM_LOD_DISTANCE_TYPE_NUM] = 
{
	"DEFAULT",
	"L_TREE",
	"M_TREE",
	"S_TREE",
	"L_ROCK",
	"M_ROCK",
	"S_ROCK",
	"L_STRUCTURE",
	"M_STRUCTURE",
	"S_STRUCTURE"
};

CHAR *AgcmLODManager::GetDistanceTypeName(UINT32 ulIndex)
{
	if (ulIndex >= E_AGCM_LOD_DISTANCE_TYPE_NUM)
		return NULL;

	return g_paszLODDistanceTypeName[ulIndex];
}

AgcmLODManager::AgcmLODManager()
{
	//Set Module Name 
	SetModuleName("AgcmLODManager");

	EnableIdle(TRUE);

	AgcmLODManager::m_pThisAgcmLODManager		= this;
	m_bForceZeroLevel							= FALSE;

	memset(m_aulDistance, 0, sizeof(UINT32) * AGCMLOD_DISTANCE_COUNT * AGPDLOD_MAX_NUM);

	m_pstMainFrame								= NULL;
	m_pulDefaultDistance						= NULL;

	ZeroMemory(m_uiLODTriCutLine, sizeof(UINT32) * 2);
	ZeroMemory(m_fLODDistFactor, sizeof(FLOAT) * 2);

	m_bUseVShader								= FALSE;
	m_iNumtest									= 0;

	ZeroMemory(m_ulTick, sizeof(UINT32) * 3);

	m_pcmRender									= NULL;
	m_ulStandardDistance						= AGCMLOD_DEFAULT_STANDARD_DISTANCE;

	m_ulCharacterStandardDistance				= AGCMLOD_CHARACTER_STANDARD_DISTANCE;
}

AgcmLODManager::~AgcmLODManager()
{

}

BOOL AgcmLODManager::OnAddModule()
{
	m_pcmRender = (AgcmRender* ) GetModule( "AgcmRender" );

	return TRUE;
}

BOOL AgcmLODManager::OnDestroy()
{

	return TRUE;
}

BOOL AgcmLODManager::OnInit()
{
	// Render�� oninit�� ���� ��ģ�ٴ� �����Ͽ�
	m_bUseVShader = ( BOOL ) m_pcmRender->GetVertexShaderEnable(); 
	
	for(int i=0;i<2;++i)
	{
		m_uiLODTriCutLine[i] = AGCMLOD_MAX_TRINUM;
		m_fLODDistFactor[i] = 1.0f;
	}
	
	return TRUE;
}

BOOL AgcmLODManager::OnIdle(UINT32 ulClockCount)
{
	m_ulTick[2] = m_ulTick[1];
	m_ulTick[1] = m_ulTick[0];
	m_ulTick[0] = ulClockCount;
	
	return TRUE;
}

/******************************************************************************
* Purpose : LOD �Ÿ��� ����� ���� frame�� �����Ѵ�.
*
* 022403. Bob Jung
******************************************************************************/
//VOID AgcmLODManager::SetMainFrame(RwFrame *pFrame)
//{
	//m_pstMainFrame = pFrame;
//}

/******************************************************************************
* Purpose : ������ Ÿ���� ���� ����� g_pulDefaultDistance���� �Ÿ��� ��!
*
* Desc : g_pulDefaultDistance�� NULL�̸�, LOD callbakc�� ���ϰ��� 0 level�� �ȴ�.
*
* 022403. Bob Jung
******************************************************************************/
VOID AgcmLODManager::SetDefaultDistance(UINT32 *pulDistance)
{
	m_pulDefaultDistance = pulDistance;
}

/******************************************************************************
* Purpose :
*
* 211004. BOB
******************************************************************************/
BOOL AgcmLODManager::SetDistanceRate(AgcdLODData *pstAgcdLODData)
{
	if (!pstAgcdLODData->m_ulMaxDistanceRatio)
		return FALSE;

	if (!m_ulStandardDistance)
		return FALSE;

	UINT32	uiMaxDistance	= (m_ulStandardDistance / 100) * pstAgcdLODData->m_ulMaxDistanceRatio;
	UINT32	uiLinearOffset	= uiMaxDistance / (pstAgcdLODData->m_ulMaxLODLevel - 1);

	for (UINT32 uiDistLevel = 0; uiDistLevel < pstAgcdLODData->m_ulMaxLODLevel; ++uiDistLevel)
	{
		pstAgcdLODData->m_aulLODDistance[uiDistLevel]	= uiLinearOffset * (UINT32)(uiDistLevel + 1);
	}

	return TRUE;
}

//. 2006. 2. 8. Nonstopdj
//. ref. AgcmCharacter::InitCharacterMainThread()
BOOL AgcmLODManager::SetCharacterDistanceRate(AgcdLODData *pstAgcdLODData)
{
	if (!pstAgcdLODData->m_ulMaxDistanceRatio)
	{
		//. �������� ������ LOD_MAX_DISTANCE�� ���� '0'�� ��� '100'�� ����.
		pstAgcdLODData->m_ulMaxDistanceRatio = 100;
	}

	if (!m_ulCharacterStandardDistance)
		return FALSE;

	UINT32	uiMaxDistance	= (m_ulCharacterStandardDistance / 100) * pstAgcdLODData->m_ulMaxDistanceRatio;
	UINT32	uiLinearOffset	= uiMaxDistance / (pstAgcdLODData->m_ulMaxLODLevel - 1);

	for (UINT32 uiDistLevel = 0; uiDistLevel < pstAgcdLODData->m_ulMaxLODLevel; ++uiDistLevel)
	{
		pstAgcdLODData->m_aulLODDistance[uiDistLevel]	= uiLinearOffset * (UINT32)(uiDistLevel + 1);
	}

	return TRUE;
}

/******************************************************************************
* Purpose : 0 ~ (RPLODATOMICMAXLOD - 1)���� �⺻ geometry�� �����Ѵ�.
*
* params :	szInitDFFPath	-	1�������� LOD data�� ���� DFF ���.
*								(�ش� clump�� atomic ������ 1���̰�, bSetNextLevel�� FALSE���߸� �Ѵ�.
								�׸���, szInitDFFPath�� NULL�� ���� 1���� ������ geometry�� NULL�� �����Ѵ�.)
*			bSetNextLevel	-	TRUE�̸� 1���� ������ geometry�� �⺻ geometry�� �����ϰ�,
*								FALSE�̸� NULL�� �����Ѵ�.
*
* 022403. Bob Jung
******************************************************************************/
BOOL AgcmLODManager::Initailize(RpClump *pstClump, RpClump *pstInitClump, BOOL bSetNextLevel)
{
	LockFrame();

	RpGeometry *pstInitGeom = NULL;

//	if(szInitDFFPath)
	if(pstInitClump)
	{
//		RpClump *pstInitLODClump = ReadDFF(szInitDFFPath);
//		if(!pstInitLODClump)
//		{
//			OutputDebugString("AgcmLODManager::Initailize() Error (1) !!!\n");
//			return FALSE;
//		}

		SearchAtomicParams InitAtomicParams;
		memset(&InitAtomicParams, 0, sizeof(SearchAtomicParams));
		InitAtomicParams.m_pvClass = this;
		RpClumpForAllAtomics(pstInitClump, SearchAtomicCB, &InitAtomicParams);

		pstInitGeom = RpAtomicGetGeometry(InitAtomicParams.m_pastAtomic[0]);
		if(!pstInitGeom)
		{
			UnlockFrame();

			OutputDebugString("AgcmLODManager::Initailize() Error (2) !!!\n");
			return FALSE;
		}

		// ������ (2004-03-03 ���� 3:07:23) : ���̿�����Ʈ
		ASSERT( 0 < pstInitGeom->refCount		&& "������Ʈ ���� ����� ���� �ʰ�" );
		ASSERT( pstInitGeom->refCount < 0x7fff	&& "������Ʈ ���� ����� ���� �ʰ�" );
	}

	INT16		nLODIndex		= 0;
	INT16		nAtomicCount	= 0;

	SearchAtomicParams BaseAtomicParams;
	memset(&BaseAtomicParams, 0, sizeof(SearchAtomicParams));
	BaseAtomicParams.m_pvClass = this;
	RpClumpForAllAtomics(pstClump, SearchAtomicCB, &BaseAtomicParams);

	// 0������ �⺻ geometry�� ����!
	for(nAtomicCount = 0; nAtomicCount < BaseAtomicParams.m_unCount; ++nAtomicCount)
	{
		RpLODAtomicSetGeometry(BaseAtomicParams.m_pastAtomic[nAtomicCount], nLODIndex, RpAtomicGetGeometry(BaseAtomicParams.m_pastAtomic[nAtomicCount]));
		
		// LODCache�� renderCallBack�� NULL�� ��찡 �־.. 2005.2.11 gemani
		//RpLODAtomicUnHookRender(BaseAtomicParams.m_pastAtomic[nAtomicCount]);
	}

	// 1���� (RPLODATOMICMAXLOD - 1)������ ������ �����Ѵ�.
	for(nLODIndex = 1; nLODIndex < RPLODATOMICMAXLOD; ++nLODIndex)
	{
		for(nAtomicCount = 0; nAtomicCount < BaseAtomicParams.m_unCount; ++nAtomicCount)
		{
			if(bSetNextLevel)
			{
				RpLODAtomicSetGeometry(BaseAtomicParams.m_pastAtomic[nAtomicCount], nLODIndex, RpAtomicGetGeometry(BaseAtomicParams.m_pastAtomic[nAtomicCount]));
			}
			else
			{
				if(pstInitGeom)
				{
					RpLODAtomicSetGeometry(BaseAtomicParams.m_pastAtomic[nAtomicCount], nLODIndex, pstInitGeom);
				}
				else
				{
					RpLODAtomicSetGeometry(BaseAtomicParams.m_pastAtomic[nAtomicCount], nLODIndex, NULL);
				}
			}

			// LODCache�� renderCallBack�� NULL�� ��찡 �־.. 2005.2.11 gemani
			//RpLODAtomicUnHookRender(BaseAtomicParams.m_pastAtomic[nAtomicCount]);
		}
	}

	UnlockFrame();

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 081203. BOB
******************************************************************************/
RpAtomic *AgcmLODManager::SetAtomic(SearchAtomicParams *pstParams, RpAtomic *pstAtomic)
{
	//@{ 2006/04/27 burumal
	if ( pstParams->m_unCount > AGCMLOD_CLUMP_MAX_ATOMIC )
		return NULL;
	//@}

	pstParams->m_pastAtomic[pstParams->m_unCount] = pstAtomic;
	++pstParams->m_unCount;

	return pstAtomic;
}

/******************************************************************************
* Purpose :
*
* 081203. BOB
******************************************************************************/
RpAtomic *AgcmLODManager::SearchAtomicCheckPartID(RpAtomic *atomic, void *data)
{
	SearchAtomicParams *pstParams = (SearchAtomicParams *)(data);
	
	if(pstParams->m_lCheckPartID == ((AgcmLODManager *)(pstParams->m_pvClass))->GetGeometryUsrDataInt(atomic, "PartID"))
	{
		//@{ 2006/04/27 burumal
		if ( pstParams->m_unCount > AGCMLOD_CLUMP_MAX_ATOMIC )
			return NULL;
		//@}

		pstParams->m_pastAtomic[pstParams->m_unCount] = atomic;
		++pstParams->m_unCount;
	}
//		((AgcmLODManager *)(pstParams->m_pvClass))->SetAtomic(pstParams, atomic);

	return atomic;
}

/******************************************************************************
* Purpose :
*
* 022403. Bob Jung
******************************************************************************/
RpAtomic *AgcmLODManager::SearchAtomicCB(RpAtomic *atomic, void *data)
{
	SearchAtomicParams *pstParams = (SearchAtomicParams *)(data);

	//@{ 2006/04/27 burumal
	if ( pstParams->m_unCount > AGCMLOD_CLUMP_MAX_ATOMIC )
		return NULL;
	//@}

	pstParams->m_pastAtomic[pstParams->m_unCount] = atomic;
	++pstParams->m_unCount;

	return atomic;

//	return ((AgcmLODManager *)(pstParams->m_pvClass))->SetAtomic(pstParams, atomic);
}

/******************************************************************************
* Purpose : Clump�� LOD�� �����Ų��.
*
* 022403. Bob Jung
******************************************************************************/
BOOL AgcmLODManager::MakeLODLevel(RpClump *pstClump, CHAR *szLODDffPath, INT32 lLevel, RpClump *pstDummyClump, BOOL bTransformGeom, BOOL bSetNextLevel, INT32 lPartID)
{
	RpClump		*pstLODClump	= NULL;
//	RpClump		*pstDummyClump	= NULL;

	// Base clump
	SearchAtomicParams BaseAtomicParams, LODAtomicParams, DummyAtomicParams;
	memset(&BaseAtomicParams, 0, sizeof(SearchAtomicParams));
	BaseAtomicParams.m_pvClass		= this;
	BaseAtomicParams.m_lCheckPartID	= lPartID;

	memcpy(&LODAtomicParams, &BaseAtomicParams, sizeof(SearchAtomicParams));
	memcpy(&DummyAtomicParams, &BaseAtomicParams, sizeof(SearchAtomicParams));

	RpClumpForAllAtomics(pstClump, SearchAtomicCB, &BaseAtomicParams);

	// LOD data
	pstLODClump = ReadDFF(szLODDffPath);
	if (!pstLODClump)
	{
		OutputDebugString("AgcmLODManager::MakeLODLevel() Error (1) !!!\n");
		return FALSE;
	}

	RpClumpForAllAtomics(pstLODClump, (lPartID) ? (SearchAtomicCheckPartID) : (SearchAtomicCB), &LODAtomicParams);

	// Dummy
	if (pstDummyClump)
	{
//		pstDummyClump = ReadDFF(szDummyDffPath);
//		if(!pstDummyClump)
//		{
			//AcuObject::DestroyClumpData(pstLODClump);RenderWare�� UD�� �ڵ����� ������

//			RpClumpDestroy(pstLODClump);

//			OutputDebugString("AgcmLODManager::MakeLODLevel() Error (2) !!!\n");
//			return FALSE;
//		}

		RpClumpForAllAtomics(pstDummyClump, SearchAtomicCB, &DummyAtomicParams);
	}

	//@{ Jaewon 20051006
	// If the LOD clump has only one atomic,
	// make the root atomic of the base clump the first in the base atomic list
	// so that the LOD atomic can be linked to the proper base atomic.
	if(LODAtomicParams.m_unCount == 1)
	{
		RwFrame *pClumpFrame = RpClumpGetFrame(pstClump);
		for(RwInt32 k=1; k< BaseAtomicParams.m_unCount; ++k)
		{ 
			if(pClumpFrame == RwFrameGetParent(RpAtomicGetFrame(BaseAtomicParams.m_pastAtomic[k])))
			{
				RpAtomic *temp = BaseAtomicParams.m_pastAtomic[0];
				BaseAtomicParams.m_pastAtomic[0] = BaseAtomicParams.m_pastAtomic[k];
				BaseAtomicParams.m_pastAtomic[k] = temp;
				break;
			}
		}
	}
	//@} Jaewon

	INT16 nNextLODIndex = lLevel + 1;
	if (bSetNextLevel)
		nNextLODIndex = RPLODATOMICMAXLOD;

	RpGeometry	*pstGeom;
	for (INT16 nLODIndex = lLevel; nLODIndex < nNextLODIndex; ++nLODIndex)	
	{
		for (INT16 nBaseAtomicCount = 0; nBaseAtomicCount < BaseAtomicParams.m_unCount; ++nBaseAtomicCount)
		{
			pstGeom = NULL;

			if (nBaseAtomicCount >= LODAtomicParams.m_unCount)
			{
				if (pstDummyClump)
					pstGeom = RpAtomicGetGeometry(DummyAtomicParams.m_pastAtomic[0]);
			}
			else
			{
				INT16	nAtomicIndex		= GetGeometryUsrDataInt(BaseAtomicParams.m_pastAtomic[nBaseAtomicCount], AGCMLOD_ATOMIC_INDEX);

				//@{ 2006/11/14 burumal
				if ( nAtomicIndex == - 1)
					nAtomicIndex = GetGeometryUsrDataInt(BaseAtomicParams.m_pastAtomic[nBaseAtomicCount], AGCMLOD_ATOMIC_INDEX_EXTRA);
				//@}

				//@{ 2006/10/10 burumal
				/*
				//. 2006. 2. 7. Nonstopdj
				//. not use UDA.
				//if(LODAtomicParams.m_pastAtomic[nBaseAtomicCount])
				//	pstGeom = RpAtomicGetGeometry(LODAtomicParams.m_pastAtomic[nBaseAtomicCount]);
				*/
				/*
				if(LODAtomicParams.m_pastAtomic[nBaseAtomicCount])
					pstGeom = RpAtomicGetGeometry(LODAtomicParams.m_pastAtomic[nBaseAtomicCount]);
				*/
				//@}

				//. 2006. 3. 13. Nonstopdj
				//. use UDA. we have not setting custom UDA, UDA always set default property.
				if (nAtomicIndex > -1)
				{
					INT16 nLODAtomicIndex;
					for (INT16 nLODAtomicCount = 0; nLODAtomicCount < LODAtomicParams.m_unCount; ++nLODAtomicCount)
					{
						nLODAtomicIndex = GetGeometryUsrDataInt(LODAtomicParams.m_pastAtomic[nLODAtomicCount], AGCMLOD_ATOMIC_INDEX);
						if (nLODAtomicIndex == -1)
						{
							//@{ 2006/11/14 burumal
							//break;
							nLODAtomicIndex = GetGeometryUsrDataInt(LODAtomicParams.m_pastAtomic[nLODAtomicCount], AGCMLOD_ATOMIC_INDEX_EXTRA);
							if ( nLODAtomicIndex == -1 )
								continue;
							//@}
						}

						if (nAtomicIndex == nLODAtomicIndex)
						{
							pstGeom				= RpAtomicGetGeometry(LODAtomicParams.m_pastAtomic[nLODAtomicCount]);
							break;
						}
					}
				}
				else
				{
					pstGeom = RpAtomicGetGeometry(LODAtomicParams.m_pastAtomic[nBaseAtomicCount]);
				}
				
				if ((pstGeom) && (bTransformGeom))
				{
					RwMatrix *pstBaseMat	= RwFrameGetMatrix(RpAtomicGetFrame(BaseAtomicParams.m_pastAtomic[nBaseAtomicCount]));
					RwMatrix *pstLODMat		= RwFrameGetMatrix(RpAtomicGetFrame(LODAtomicParams.m_pastAtomic[nBaseAtomicCount]));

					RwMatrix stInv;
					RwMatrixInvert(&stInv, pstBaseMat);
					RwMatrix stRt;
					RwMatrixMultiply(&stRt, pstLODMat, &stInv);

					RpGeometryTransform(pstGeom, &stRt);
				}
			}
									
			RpLODAtomicSetGeometry(BaseAtomicParams.m_pastAtomic[nBaseAtomicCount], nLODIndex, pstGeom);
			
			// LODCache�� renderCallBack�� NULL�� ��찡 �־.. 2005.2.11 gemani
			//RpLODAtomicUnHookRender(BaseAtomicParams.m_pastAtomic[nBaseAtomicCount]);
		}
	}

//	if(pstDummyClump)
//	{
//		AcuObject::DestroyClumpData(pstDummyClump);
//		RpClumpDestroy(pstDummyClump);
//	}

	if (pstLODClump)
	{
		//AcuObject::DestroyClumpData(pstLODClump);RenderWare�� UD�� �ڵ����� ������
		LockFrame();
		RpClumpDestroy(pstLODClump);
		UnlockFrame();
	}

	return TRUE;
}

/******************************************************************************
* Purpose :
*
* 170204. Bob Jung
******************************************************************************/
BOOL AgcmLODManager::MakeLODLevel_Billboard(RpClump *pstClump, CHAR *szBillDffPath, INT32 lLevel, RpClump *pstDummyClump)
{
	SearchAtomicParams AtomicParams;

	if (pstDummyClump)
	{
		memset(&AtomicParams, 0, sizeof(SearchAtomicParams));
		AtomicParams.m_pvClass	= this;
		RpClumpForAllAtomics(pstDummyClump, SearchAtomicCB, &AtomicParams);
	}

	RpGeometry	*pstGeom;

	if (pstDummyClump)
	{
		// ���� Ŭ������ ����� ������ �ϳ���� �����Ѵ�.
		pstGeom					= RpAtomicGetGeometry(AtomicParams.m_pastAtomic[0]);
		if (!pstGeom)
			return FALSE;
	}
	else
	{
		pstGeom					= NULL;
	}

	LockFrame();

	// ������� �����.
	RpAtomic	*pstAtomic		= RpAtomicCreate();
	if (!pstAtomic)
	{
		UnlockFrame();
		return FALSE;
	}

	RpAtomicSetGeometry(pstAtomic, pstGeom, 0);

	RwFrame		*pstFrame		= RwFrameCreate();
	if (!pstFrame)
	{
		UnlockFrame();
		return FALSE;
	}

	RpAtomicSetFrame(pstAtomic, pstFrame);

	// �⺻ geometry�� �����Ѵ�.
	for (INT32 lCount = 0; lCount < RPLODATOMICMAXLOD; ++lCount)
	{
		RpLODAtomicSetGeometry(pstAtomic, lCount, pstGeom);
	}

	// ������ Ŭ������ �ε��Ѵ�.
	RpClump		*pstBillClump	= ReadDFF(szBillDffPath);
	if (!pstBillClump)
	{
		UnlockFrame();
		return FALSE;
	}

	memset(&AtomicParams, 0, sizeof(SearchAtomicParams));
	AtomicParams.m_pvClass = this;
	RpClumpForAllAtomics(pstBillClump, SearchAtomicCB, &AtomicParams);

	// ������ Ŭ������ ����� ������ �ϳ���� �����Ѵ�.

	//@{ kday 20051011
	// ;)
	// �����غ��� pstBillClump �� ������ (*pstBillClump).atomicList == NULL ��찡 ����
	if(AtomicParams.m_pastAtomic[0] == NULL)
	{
		RpClumpDestroy(pstBillClump);
		UnlockFrame();
		return FALSE;
	}
	//@} kday
	pstGeom						= RpAtomicGetGeometry(AtomicParams.m_pastAtomic[0]);

	// �ش� ������ ������ geometry�� �����Ѵ�.
	RpLODAtomicSetGeometry(pstAtomic, lLevel, pstGeom);

	// LODCache�� renderCallBack�� NULL�� ��찡 �־.. 2005.2.11 gemani
	//RpLODAtomicUnHookRender(pstAtomic);

	// Ŭ������ ������� �߰��Ѵ�.
	RpClumpAddAtomic(pstClump, pstAtomic);
	RwFrameAddChild(RpClumpGetFrame(pstClump), RpAtomicGetFrame(pstAtomic));

	// ������ Ŭ������ �����Ѵ�.
	RpClumpDestroy(pstBillClump);

	UnlockFrame();

	// Render UDA�� �����Ѵ�.
	AcuObject::SetAtomicRenderUDA(pstAtomic, R_BLEND_SORT, 0, 0, 0, 0);

	return TRUE;
}

/******************************************************************************
* Purpose : DFF file�� �д´�.
*
* 022403. Bob Jung
******************************************************************************/
RpClump *AgcmLODManager::ReadDFF(CHAR *szDFFPath)
{
	RwStream	*pstStream;
    RpClump		*pstClump = NULL;

    pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, szDFFPath);

    if(pstStream)
    {
        if(RwStreamFindChunk(pstStream, rwID_CLUMP, NULL, NULL)) 
        {
            pstClump = RpClumpStreamRead(pstStream);
        }

        RwStreamClose(pstStream, NULL);
    }

    return pstClump;
}

/******************************************************************************
* Purpose : Atomic geometry�� UserData�� ������ 'AtomicIndex"�� �����´�.
*
* Desc : 'AtomicIndex'�� 2���� �����ϱ� ������ return ���� 0�̸� fail�̴�.
*
* 022403. Bob Jung
******************************************************************************/
RwInt32 AgcmLODManager::GetGeometryUsrDataInt(RpAtomic *pstAtomic, RwChar *szUsrName)
{
	RwInt32 nAtomicIndex	= -1;
	RpGeometry *pGeom		= RpAtomicGetGeometry(pstAtomic);	

	if(pGeom)
	{
		RwInt32 nUsrDatNum = RpGeometryGetUserDataArrayCount(pGeom);

		RpUserDataArray *pstUserDataArray;
		RwChar			*pszUserDataArrayName;

		for(RwInt32 nCount = 0; nCount < nUsrDatNum; ++nCount)
		{
			pstUserDataArray		= RpGeometryGetUserDataArray(pGeom, nCount);

			if(pstUserDataArray)
			{
				pszUserDataArrayName	= RpUserDataArrayGetName(pstUserDataArray);

				if(!strcmp(pszUserDataArrayName, szUsrName))
				{
					nAtomicIndex = RpUserDataArrayGetInt(pstUserDataArray, 0);
					break;
				}
			}
		}
	}

	return nAtomicIndex;
}

/******************************************************************************
* Purpose : Clump�� ��� atomic�� RpLODAtomicSetLODCallBack, RpLODAtomicHookRender�� �Ѵ�~
*
* 022403. Bob Jung
******************************************************************************/
VOID AgcmLODManager::SetLODCallback(RpClump *pstClump)
{
	SearchAtomicParams BaseAtomicParams;
	memset(&BaseAtomicParams, 0, sizeof(SearchAtomicParams));
	BaseAtomicParams.m_pvClass = this;
	RpClumpForAllAtomics(pstClump, SearchAtomicCB, &BaseAtomicParams);

	for(INT16 nAtomicCount = 0; nAtomicCount < BaseAtomicParams.m_unCount; ++nAtomicCount)
	{
		RpLODAtomicSetLODCallBack(BaseAtomicParams.m_pastAtomic[nAtomicCount], LODCallback);
		RpLODAtomicHookRender(BaseAtomicParams.m_pastAtomic[nAtomicCount]);
	}

	//@{ Jaewon 20050602
	// Just use atomic->renderCallBack.
	//m_pcmRender->UpdateRenderCallBack(pstClump);
	//@} Jaewon
}

void	AgcmLODManager::FrameMatrixSave(RpAtomic*	atomic)
{
	float*	fFrame = AcuObject::MakeAtomicBackFrameInfo(atomic);

	RwMatrix*		src;
	RwFrame*		pframe = RpAtomicGetFrame(atomic);

	src = RwFrameGetMatrix(pframe);
	
	RwV3d*	temp = RwMatrixGetRight(src);
	fFrame[0] = temp->x;
	fFrame[1] = temp->y;
	fFrame[2] = temp->z;

	temp = RwMatrixGetUp(src);
	fFrame[3] = temp->x;
	fFrame[4] = temp->y;
	fFrame[5] = temp->z;

	temp = RwMatrixGetAt(src);
	fFrame[6] = temp->x;
	fFrame[7] = temp->y;
	fFrame[8] = temp->z;
}

void	AgcmLODManager::FrameMatrixLoad(RpAtomic*	atomic)
{
	float*	fFrame = AcuObject::GetAtomicBackFrameInfo(atomic);

	RwMatrix*		src;
	RwFrame*		pframe = RpAtomicGetFrame(atomic);

	src = RwFrameGetMatrix(pframe);
	
	RwV3d*	temp = RwMatrixGetRight(src);
	temp->x = fFrame[0];
	temp->y = fFrame[1];
	temp->z = fFrame[2];

	temp = RwMatrixGetUp(src);
	temp->x = fFrame[3];
	temp->y = fFrame[4];
	temp->z = fFrame[5];

	temp = RwMatrixGetAt(src);
	temp->x = fFrame[6];
	temp->y = fFrame[7];
	temp->z = fFrame[8];

	LockFrame();

	RwFrameUpdateObjects(pframe);
	RwFrameGetLTM(pframe);

	UnlockFrame();
}

///////////////////////////////////////////////////////////////////////
//
// 2004.10.20 gemani .. lodcallback ����..
//	AgcmTuner���� DistanceFactor ó���� �ﰢ�� ���� ���� �� ���� ó���� ����..
//
///////////////////////////////////////////////////////////////////////
RwInt32	AgcmLODManager::LODCallback(RpAtomic*	atomic)
{
	ASSERT( NULL != atomic );
	if (m_pThisAgcmLODManager->m_bForceZeroLevel)	return 0;
	
	AgcdLODData*	pstAgcdLODData;
	UINT32*			puiDistance;
	INT32			iMaxLev;
	INT32			iReturnLev	= -1;
	INT32			iBillBoardNum = 0;
	INT32*			piBillBoardLev = NULL;
	PVOID			pData		= NULL;
	
	RpClump*		parentClump	= RpAtomicGetClump(atomic);
	if(!parentClump) return 0;
		
	INT32	lType = AcuObject::GetClumpType(parentClump, NULL, NULL, NULL, &pData);
		
	if(pData)
	{
		pstAgcdLODData	= (AgcdLODData *)(pData);
		puiDistance		= m_pThisAgcmLODManager->GetDistanceData((eAgcmLODDistanceType)(pstAgcdLODData->m_pstAgcdLOD->m_lDistanceType));
		iMaxLev			= pstAgcdLODData->m_ulMaxLODLevel;
		iBillBoardNum	= pstAgcdLODData->m_lHasBillNum;
		piBillBoardLev	= pstAgcdLODData->m_alBillInfo;

		// ������ ��û���� ����(BOB, 211004)
		if (pstAgcdLODData->m_ulMaxDistanceRatio)
			puiDistance	= pstAgcdLODData->m_aulLODDistance;
	}
	else
	{
		if(!m_pThisAgcmLODManager->m_pulDefaultDistance) return 0;
		
		puiDistance = m_pThisAgcmLODManager->m_pulDefaultDistance;
		iMaxLev = AGPDLOD_MAX_NUM;
	}

	INT32	myType = AcuObject::GetType(lType);
	INT32	distfactor_index;

	if(myType == ACUOBJECT_TYPE_OBJECT)
	{
		distfactor_index = 0;
	}
	else if(myType == ACUOBJECT_TYPE_CHARACTER || myType == ACUOBJECT_TYPE_ITEM)
	{
		distfactor_index = 1;
	}

	RwReal		df = m_pThisAgcmLODManager->m_fLODDistFactor[distfactor_index];
	RwReal		fdistance = parentClump->stUserData.calcDistance;
	for(int iLODIndex = 0; iLODIndex < iMaxLev; ++iLODIndex)
    {
		if(fdistance < puiDistance[iLODIndex] * df)
        {
            iReturnLev = iLODIndex;
			break;
        }
    }
	
	if(iReturnLev == -1) iReturnLev = iMaxLev-1;

	//@{ Jaewon 20041020
	// A skinning pipe atomic shouldn't use a non-skinned lod geometry.
	RpGeometry * pGeom = RpLODAtomicGetGeometry(atomic, 0);

	if( pGeom && RpSkinGeometryGetSkin( pGeom ) )
	{
		while(
			iReturnLev > 0 &&
			(
				NULL == ( pGeom = RpLODAtomicGetGeometry(atomic, iReturnLev) ) ||
				NULL == RpSkinGeometryGetSkin( pGeom )
			)
		)
		{
			--iReturnLev;
		}
	}
	//@} Jaewon

	if( pGeom  )
	{
		while(
			iReturnLev > 0 &&
			(
				NULL == ( pGeom = RpLODAtomicGetGeometry(atomic, iReturnLev) ) 
			)
		)
		{
			--iReturnLev;
		}
	}
	
	if(atomic->stRenderInfo.beforeLODLevel == -2) // ó�� ����
	{		
		if(iBillBoardNum)
		{
			m_pThisAgcmLODManager->FrameMatrixSave(atomic);
		}

		if(iReturnLev == 0)		// level0�� ��쿡�� fx on .. skin �� ��찡 �ƴ϶� ����..
		{
			if(!RpAtomicFxIsEnabled(atomic))
				m_pThisAgcmLODManager->m_pcmRender->AtomicFxOn(atomic);
		}
		else
		{
			if(RpAtomicFxIsEnabled(atomic))
				m_pThisAgcmLODManager->m_pcmRender->AtomicFxOff(atomic);
		}

		if(myType == ACUOBJECT_TYPE_CHARACTER || myType == ACUOBJECT_TYPE_ITEM)		// character���..
		{
			//m_pThisAgcmLODManager->m_pcmRender->StartFadeInClump(RpAtomicGetClump(atomic),iReturnLev);

			pGeom = RpLODAtomicGetGeometry(atomic,iReturnLev);
			if(pGeom)
			{
				m_pThisAgcmLODManager->m_pcmRender->StartFadeIn(atomic,iReturnLev,2000);
			}
		}
		else 
		{
			//if(atomic->stRenderInfo.addTick != m_pThisAgcmLODManager->m_ulTick[2])		// ó�� �����Ѱ� �ƴϴ�. �׷��� addtick������� �ʾƼ� ����.. �ʿ��ϸ� atomic�� �߰�..
			//{
			//	atomic->stRenderInfo.beforeLODLevel = iReturnLev;
			//	atomic->stRenderInfo.shaderUseType = 0;
			//	return ReturnLev;
			//}

			pGeom = RpAtomicGetGeometry(atomic);
			if(pGeom)
			{
				if(RpGeometryGetNumTriangles(pGeom) < 2)									// Dummy���Խ�..
					m_pThisAgcmLODManager->m_pcmRender->StartFadeInClump(RpAtomicGetClump(atomic),iReturnLev);
			}
		}

		atomic->stRenderInfo.beforeLODLevel = iReturnLev;

		return iReturnLev;
	}

	if(atomic->stRenderInfo.shaderUseType > 0)
	{
		iReturnLev = atomic->stRenderInfo.beforeLODLevel;
		return	iReturnLev;
	}

	if(atomic->stRenderInfo.beforeLODLevel != iReturnLev)		// LOD���� ó��(RI_BEFORELOD	= 5)
	{
		if(iBillBoardNum)
		{
			BOOL	bB1 = FALSE;	// �������� ���������� üũ
			BOOL	bB2 = FALSE;	// ������� ���������� üũ
			for(int i=0;i<iBillBoardNum;++i)
			{
				if(piBillBoardLev[i] == iReturnLev)			bB2 = TRUE;
				if(piBillBoardLev[i] == atomic->stRenderInfo.beforeLODLevel)	bB1 = TRUE;
			}

			if(!bB1 && bB2 )			// ������ ����..
			{
				m_pThisAgcmLODManager->FrameMatrixSave(atomic);

				atomic->stRenderInfo.isNowBillboard = 1;

				if(atomic->stRenderInfo.shaderUseType == 0 && (AcuObject::GetProperty(lType) & ACUOBJECT_TYPE_USE_FADE_IN_OUT))
				{
					pGeom = RpLODAtomicGetGeometry(atomic,iReturnLev);
					if(pGeom)
					{
						m_pThisAgcmLODManager->m_pcmRender->StartFadeInOut(atomic,iReturnLev,atomic->stRenderInfo.beforeLODLevel);
				
						int		temp = atomic->stRenderInfo.beforeLODLevel;
						atomic->stRenderInfo.beforeLODLevel = iReturnLev;
	
						iReturnLev = temp;
						return iReturnLev;
					}
				}
			}
			else if(bB1 && !bB2)	// ������ Ż��..
			{
				m_pThisAgcmLODManager->FrameMatrixLoad(atomic);
				
				atomic->stRenderInfo.isNowBillboard = 0;
				
				if(atomic->stRenderInfo.shaderUseType == 0 && (AcuObject::GetProperty(lType) & ACUOBJECT_TYPE_USE_FADE_IN_OUT))
				{
					pGeom = RpLODAtomicGetGeometry(atomic,iReturnLev);
					if(pGeom)
					{
						m_pThisAgcmLODManager->m_pcmRender->StartFadeInOut(atomic,iReturnLev,atomic->stRenderInfo.beforeLODLevel);
				
						int		temp = atomic->stRenderInfo.beforeLODLevel;
						atomic->stRenderInfo.beforeLODLevel = iReturnLev;

						iReturnLev = temp;
						return iReturnLev;
					}
				}
			}
		}

		//if(atomic->stRenderInfo.shaderUseType == 0 && (AcuObject::GetProperty(lType) & ACUOBJECT_TYPE_USE_FADE_IN_OUT))
		//{
		//	m_pThisAgcmLODManager->m_pcmRender->StartFadeInOut(atomic,iReturnLev,atomic->stRenderInfo.beforeLODLevel);

		//	int		temp = atomic->stRenderInfo.beforeLODLevel;
		//	atomic->stRenderInfo.beforeLODLevel = iReturnLev;

		//	iReturnLev = temp;
		//}
		//else
		if(atomic->stRenderInfo.shaderUseType == 0)
		{
			if(iReturnLev == 0)		// level0�� ��쿡�� fx on
			{
				if(!RpAtomicFxIsEnabled(atomic))
				{
					m_pThisAgcmLODManager->m_pcmRender->AtomicFxOn(atomic);
				}
			}
			else
			{
				if(RpAtomicFxIsEnabled(atomic))
				{
					m_pThisAgcmLODManager->m_pcmRender->AtomicFxOff(atomic);
				}
			}

			atomic->stRenderInfo.beforeLODLevel = iReturnLev;
		}
	}

	return iReturnLev;
}

AgcdLODData *AgcmLODManager::GetLODData(AgcdLOD *pstLOD, INT32 lIndex, BOOL bAdd)
{
	AgcdLODData *pstData = m_csLODList.GetLODData(pstLOD, lIndex);
	if((!pstData) && (bAdd))
	{
		AgcdLODData stData;
		pstData = m_csLODList.AddLODData(pstLOD, &stData);
	}

	return pstData;
}

INT32 AgcmLODManager::StreamRead(ApModuleStream *pcsStream, AgcdLOD *pstLOD)
{
	AgcdLODData	*pstData;
	INT32		lIndex;
	CHAR		szValue[256];
	INT32		lTemp1, lTemp2;

	if (!strcmp(pcsStream->GetValueName(), AGCMLOD_STREAM_LOD_LEVEL))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d", &lIndex, &lTemp1);

		pstData	= GetLODData(pstLOD, lIndex);
		if (!pstData)
			return E_AGCD_LOD_STREAM_READ_RESULT_ERROR;

		pstData->m_ulMaxLODLevel = lTemp1;

		return E_AGCD_LOD_STREAM_READ_RESULT_READ;
	}
/*	else if (!strncmp(pcsStream->GetValueName(), AGCMLOD_STREAM_LOD_DISTANCE, strlen(AGCMLOD_STREAM_LOD_DISTANCE)))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d:%d", &lIndex, &lTemp1, &lTemp2);

		pstData	= GetLODData(pstLOD, lIndex);
		if (!pstData)
			return E_AGCD_LOD_STREAM_READ_RESULT_ERROR;

		pstData->m_aulLODDistance[lTemp1] = lTemp2;

		return E_AGCD_LOD_STREAM_READ_RESULT_READ;
	}*/
	else if (!strcmp(pcsStream->GetValueName(), AGCMLOD_STREAM_LOD_BILLBOARD_NUM))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d", &lIndex, &lTemp1);

		pstData	= GetLODData(pstLOD, lIndex);
		if (!pstData)
			return E_AGCD_LOD_STREAM_READ_RESULT_ERROR;

		pstData->m_lHasBillNum	= lTemp1;

		return E_AGCD_LOD_STREAM_READ_RESULT_READ;
	}
	else if (!strncmp(pcsStream->GetValueName(), AGCMLOD_STREAM_LOD_BILLBOARD_INFO, strlen(AGCMLOD_STREAM_LOD_BILLBOARD_INFO)))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d:%d", &lIndex, &lTemp1, &lTemp2);

		pstData	= GetLODData(pstLOD, lIndex);
		if (!pstData)
			return E_AGCD_LOD_STREAM_READ_RESULT_ERROR;

		pstData->m_alBillInfo[lTemp1] = lTemp2;

		return E_AGCD_LOD_STREAM_READ_RESULT_READ;
	}
	else if (!strcmp(pcsStream->GetValueName(), AGCMLOD_STREAM_LOD_BOUNDARY))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d", &lIndex, &lTemp1);

		pstData	= GetLODData(pstLOD, lIndex);
		if (!pstData)
			return E_AGCD_LOD_STREAM_READ_RESULT_ERROR;

		pstData->m_ulBoundary = lTemp1;

		return E_AGCD_LOD_STREAM_READ_RESULT_READ;
	}
	else if (!strcmp(pcsStream->GetValueName(), AGCMLOD_STREAM_LOD_DISTANCE_TYPE))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d", &lTemp1);

		pstLOD->m_lDistanceType = lTemp1;
	}
	else if (!strcmp(pcsStream->GetValueName(), AGCMLOD_STREAM_LOD_MAX_DISTANCE))
	{
		pcsStream->GetValue(szValue, 256);
		sscanf(szValue, "%d:%d", &lIndex, &lTemp1);

		pstData = GetLODData(pstLOD, lIndex);
		if (!pstData)
			return E_AGCD_LOD_STREAM_READ_RESULT_ERROR;

		pstData->m_ulMaxDistanceRatio	= lTemp1;
	}

	return E_AGCD_LOD_STREAM_READ_RESULT_PASS;
}

BOOL AgcmLODManager::StreamWrite(ApModuleStream *pcsStream, AgcdLOD *pstLOD)
{
	INT32			lCount;
	CHAR			szValueName[256], szValue[256];
//	AgcdLODList		*pstList	= pstLOD->m_pstList;
//	while(pstList)
	AgcdLODData		*pcsData;
	for(INT32 lGroupIndex = 0;; ++lGroupIndex)
	{
		pcsData = m_csLODList.GetLODData(pstLOD, lGroupIndex);
		if(!pcsData)
			break;

		if(pcsData->m_ulMaxLODLevel > 0)
		{
			// LOD level
			sprintf(szValue, "%d:%d", pcsData->m_lIndex, pcsData->m_ulMaxLODLevel);
			pcsStream->WriteValue(AGCMLOD_STREAM_LOD_LEVEL, szValue);

			// LOD Distance
/*			for(lCount = 0; lCount < (INT32)(pcsData->m_ulMaxLODLevel); ++lCount)
			{
				sprintf(szValueName, "%s%d", AGCMLOD_STREAM_LOD_DISTANCE, lCount);
				sprintf(szValue, "%d:%d:%d",	pcsData->m_lIndex,
												lCount,
												pcsData->m_aulLODDistance[lCount]);

				pcsStream->WriteValue(szValueName, szValue);
			}*/

			// Billboard
			if(pcsData->m_lHasBillNum > 0)
			{
				// Billboard Num
				sprintf(szValue, "%d:%d", pcsData->m_lIndex, pcsData->m_lHasBillNum);
				pcsStream->WriteValue(AGCMLOD_STREAM_LOD_BILLBOARD_NUM, szValue);

				for(lCount = 0; lCount < (INT32)(pcsData->m_ulMaxLODLevel); ++lCount)
				{
					if(pcsData->m_alBillInfo[lCount])
					{
						sprintf(szValueName, "%s%d", AGCMLOD_STREAM_LOD_BILLBOARD_INFO, lCount);
						sprintf(szValue, "%d:%d:%d", pcsData->m_lIndex, lCount, pcsData->m_alBillInfo[lCount]);

						pcsStream->WriteValue(szValueName, szValue);
					}
				}
			}

			// Boundary
			sprintf(szValue, "%d:%d", pcsData->m_lIndex, pcsData->m_ulBoundary);
			pcsStream->WriteValue(AGCMLOD_STREAM_LOD_BOUNDARY, szValue);

			// Max distance
			sprintf(szValue, "%d:%d", pcsData->m_lIndex, pcsData->m_ulMaxDistanceRatio);
			pcsStream->WriteValue(AGCMLOD_STREAM_LOD_MAX_DISTANCE, szValue);
		}

//		pstList = pstList->m_pstNext;
	}

	if (pstLOD->m_lDistanceType)
	{
		sprintf(szValue, "%d", pstLOD->m_lDistanceType);
		pcsStream->WriteValue(AGCMLOD_STREAM_LOD_DISTANCE_TYPE, szValue);
	}

	return TRUE;
}

INT32 AgcmLODManager::SearchLODType(CHAR *szName)
{
	if (!stricmp(AGCMLOD_DISTANCE_TYPE_DEFAULT, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_DEFAULT;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_L_TREE, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_L_TREE;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_M_TREE, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_M_TREE;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_S_TREE, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_S_TREE;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_L_ROCK, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_L_ROCK;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_M_ROCK, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_M_ROCK;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_S_ROCK, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_S_ROCK;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_L_STRUCTRUE, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_L_STRUCTURE;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_M_STRUCTRUE, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_M_STRUCTURE;
	}
	else if (!stricmp(AGCMLOD_DISTANCE_TYPE_S_STRUCTRUE, szName))
	{
		return E_AGCM_LOD_DISTANCE_TYPE_S_STRUCTURE;
	}

	return -1;
}

BOOL AgcmLODManager::ReadLODDistanceData(CHAR *szPathName, BOOL bDecryption)
{
	AuExcelTxtLib	csAuExcelTxtLib;
	if (!csAuExcelTxtLib.OpenExcelFile(szPathName, TRUE, bDecryption))
	{
		OutputDebugString("AgcmLODManager::ReadLODDistanceData() Error (1) !!!\n");
		csAuExcelTxtLib.CloseFile();
		return FALSE;
	}

	INT32					lDistanceType;
	const INT32				lKeyColumn		= 0;
	const INT32				lKeyRow			= 0;
	CHAR					*pszData;
	
	for (INT32 lCol = 1; lCol < csAuExcelTxtLib.GetColumn(); ++lCol)
	{
		pszData				= csAuExcelTxtLib.GetData(lCol, lKeyRow);
		if (!pszData)
			continue;

		lDistanceType		= SearchLODType(pszData);
		if (lDistanceType < 0)
			continue;

		for (INT32 lRow = 1; lRow < csAuExcelTxtLib.GetRow(); ++lRow)
		{
			pszData = csAuExcelTxtLib.GetData(lKeyColumn, lRow);
			if (!pszData)
				continue;

			if (!strcmp(pszData, AGCMLOD_DISTANCE_LEVEL_1))
			{
				pszData = csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				m_aulDistance[lDistanceType][0] = atoi(pszData);
			}
			else if (!strcmp(pszData, AGCMLOD_DISTANCE_LEVEL_2))
			{
				pszData = csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				m_aulDistance[lDistanceType][1] = atoi(pszData);
			}
			else if (!strcmp(pszData, AGCMLOD_DISTANCE_LEVEL_3))
			{
				pszData = csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				m_aulDistance[lDistanceType][2] = atoi(pszData);
			}
			else if (!strcmp(pszData, AGCMLOD_DISTANCE_LEVEL_4))
			{
				pszData = csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				m_aulDistance[lDistanceType][3] = atoi(pszData);
			}
			else if (!strcmp(pszData, AGCMLOD_DISTANCE_LEVEL_5))
			{
				pszData = csAuExcelTxtLib.GetData(lCol, lRow);
				if (!pszData)
					continue;

				m_aulDistance[lDistanceType][4] = atoi(pszData);
			}
		}
	}

	csAuExcelTxtLib.CloseFile();
	return TRUE;
}

UINT32 *AgcmLODManager::GetDistanceData(eAgcmLODDistanceType eType)
{
	return m_aulDistance[eType];
}

BOOL AgcmLODManager::IsSetLOD(RpClump *pstClump)
{
	SearchAtomicParams BaseAtomicParams;
	memset(&BaseAtomicParams, 0, sizeof(SearchAtomicParams));
	BaseAtomicParams.m_pvClass = this;
	RpClumpForAllAtomics(pstClump, SearchAtomicCB, &BaseAtomicParams);

	if (!BaseAtomicParams.m_unCount)
		return FALSE;

	// �̹� LOD�� �����Ǿ� �ִ�.
	if (RpLODAtomicGetCurrentLOD(BaseAtomicParams.m_pastAtomic[0]) > -1)
		return TRUE; // skip!

	return FALSE;
}
