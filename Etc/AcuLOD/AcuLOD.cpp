#include "AcuLOD.h"
#include "AcuObject.h"

static RwFrame	*g_pstMainFrame			= NULL;
//static RwFrame	*g_pstCameraFrame		= NULL;
static UINT32	*g_pulDefaultDistance	= NULL;

typedef struct RpClumpForAllAtomicsSet
{
	RpAtomic	*m_pastAtomic[ACULOD_CLUMP_MAX_ATOMIC];
	RwInt32		m_anAtomicIndex[ACULOD_CLUMP_MAX_ATOMIC];
	UINT16		m_unCount;
} RpClumpForAllAtomicsSet;

/******************************************************************************
* Purpose : LOD �Ÿ��� ����� �� ���̴� ī�޶� frame�� �����Ѵ�.
*
* 030503. Bob Jung
******************************************************************************/
/*VOID AcuLOD::SetCameraFrame(RwFrame *pFrame)
{
	g_pstCameraFrame = pFrame;
}*/

/******************************************************************************
* Purpose : LOD �Ÿ��� ����� ���� frame�� �����Ѵ�.
*
* 022403. Bob Jung
******************************************************************************/
VOID AcuLOD::SetMainFrame(RwFrame *pFrame)
{
	g_pstMainFrame = pFrame;
}

/******************************************************************************
* Purpose : ������ Ÿ���� ���� ����� g_pulDefaultDistance���� �Ÿ��� ��!
*
* Desc : g_pulDefaultDistance�� NULL�̸�, LOD callbakc�� ���ϰ��� 0 level�� �ȴ�.
*
* 022403. Bob Jung
******************************************************************************/
VOID AcuLOD::SetDefaultDistance(UINT32 *pulDistance)
{
	g_pulDefaultDistance = pulDistance;
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
BOOL AcuLOD::Initailize(RpClump *pstClump, CHAR *szInitDFFPath, BOOL bSetNextLevel)
{
	RpGeometry *pstInitGeom = NULL;

	if(szInitDFFPath)
	{
		RpClump *pstInitLODClump = ReadDFF(szInitDFFPath);
		if(!pstInitLODClump)
		{
			OutputDebugString("AcuLOD::Initailize() Error (1) !!!\n");
			return FALSE;
		}

		RpClumpForAllAtomicsSet LODAtomicSet;
		memset(&LODAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));	
		RpClumpForAllAtomics(pstInitLODClump, SetBaseAtomicCB, &LODAtomicSet);

		pstInitGeom = RpAtomicGetGeometry(LODAtomicSet.m_pastAtomic[0]);
		if(!pstInitGeom)
		{
			OutputDebugString("AcuLOD::Initailize() Error (2) !!!\n");
			return FALSE;
		}
	}

	INT16		nLODIndex		= 0;
	INT16		nAtomicCount	= 0;

	RpClumpForAllAtomicsSet BaseAtomicSet;
	memset(&BaseAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstClump, SetBaseAtomicCB, &BaseAtomicSet);

	// 0������ �⺻ geometry�� ����!
	for(nAtomicCount = 0; nAtomicCount < BaseAtomicSet.m_unCount; ++nAtomicCount)
	{
		RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, RpAtomicGetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount]));
	}

	// 1���� (RPLODATOMICMAXLOD - 1)������ ������ �����Ѵ�.
	for(nLODIndex = 1; nLODIndex < RPLODATOMICMAXLOD; ++nLODIndex)
	{
		for(nAtomicCount = 0; nAtomicCount < BaseAtomicSet.m_unCount; ++nAtomicCount)
		{
			if(bSetNextLevel)
			{
				RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, RpAtomicGetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount]));
			}
			else
			{
				if(pstInitGeom)
				{
					RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, pstInitGeom);
				}
				else
				{
					RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nAtomicCount], nLODIndex, NULL);
				}
			}
		}
	}

	return TRUE;
}

/******************************************************************************
* Purpose : Clump�� atomic �����Ϳ� atomic������ �����ϴ� callback.
*
* 022403. Bob Jung
******************************************************************************/
RpAtomic *AcuLOD::SetBaseAtomicCB(RpAtomic *atomic, void *data)
{
	RpClumpForAllAtomicsSet *pBaseAtomicSet = (RpClumpForAllAtomicsSet *)(data);

	pBaseAtomicSet->m_pastAtomic[pBaseAtomicSet->m_unCount] = atomic;
	++pBaseAtomicSet->m_unCount;

	return atomic;
}

/******************************************************************************
* Purpose : Clump�� LOD�� �����Ų��.
*
* Params :	pstClump		- �����Ű�����ϴ� clump
*			szLODDffName	- LOD �������� DFF name
*			nLevel			- LOD level
*			szDummyDffPath	- �⺻ Ŭ������ ������� ������ LOD Ŭ������ ����� ���� ���� ���� ���,
							�� �༮�� ������Ʈ���� �����ȴ�(����� ������ �Ѱ���� �����Ѵ�)
*			bSetNextLevel	- nLevel ���� ������ geometry�� LOD data�� geometry�� �����Ұ���?
*
* 022403. Bob Jung
******************************************************************************/
BOOL AcuLOD::MakeLODLevel(RpClump *pstClump, CHAR *szLODDffPath, INT16 nLevel, CHAR *szDummyDffPath, BOOL bSetNextLevel)
{
	RpClump		*pstLODClump	= NULL;
	RpClump		*pstDummyClump	= NULL;

	// Base clump
	RpClumpForAllAtomicsSet BaseAtomicSet, LODAtomicSet, DummyAtomicSet;
	memset(&BaseAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstClump, SetBaseAtomicCB, &BaseAtomicSet);

	// LOD data
	pstLODClump = ReadDFF(szLODDffPath);
	if(!pstLODClump)
	{
		OutputDebugString("AcuLOD::MakeLODLevel() Error (1) !!!\n");
		return FALSE;
	}

	memset(&LODAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstLODClump, SetBaseAtomicCB, &LODAtomicSet);

	// Dummy
	pstDummyClump = ReadDFF(szDummyDffPath);
	if(!pstDummyClump)
	{
		RpClumpDestroy(pstLODClump);

		OutputDebugString("AcuLOD::MakeLODLevel() Error (2) !!!\n");
		return FALSE;
	}

	memset(&DummyAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstDummyClump, SetBaseAtomicCB, &DummyAtomicSet);

	INT16 nNextLODIndex = nLevel + 1;
	if(bSetNextLevel)
		nNextLODIndex = RPLODATOMICMAXLOD;

	for(INT16 nLODIndex = nLevel; nLODIndex < nNextLODIndex; ++nLODIndex)	
	{
		for(INT16 nBaseAtomicCount = 0; nBaseAtomicCount < BaseAtomicSet.m_unCount; ++nBaseAtomicCount)
		{
			RpGeometry *pstGeom =	(nBaseAtomicCount >= LODAtomicSet.m_unCount) ? 
									(RpAtomicGetGeometry(DummyAtomicSet.m_pastAtomic[0])) : // Dummy�� atomic ������ 1����� �����Ѵ�.
									(RpAtomicGetGeometry(LODAtomicSet.m_pastAtomic[nBaseAtomicCount]));

			if(!pstGeom)
			{
				RpClumpDestroy(pstDummyClump);
				RpClumpDestroy(pstLODClump);

				OutputDebugString("AcuLOD::MakeLODLevel() Error (3) !!!\n");
				return FALSE;
			}
									
			RpLODAtomicSetGeometry(BaseAtomicSet.m_pastAtomic[nBaseAtomicCount], nLODIndex, pstGeom);
		}
	}

	RpClumpDestroy(pstDummyClump);
	RpClumpDestroy(pstLODClump);

	return TRUE;
}

/******************************************************************************
* Purpose : DFF file�� �д´�.
*
* 022403. Bob Jung
******************************************************************************/
RpClump *AcuLOD::ReadDFF(CHAR *szDFFPath)
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
RwInt32 AcuLOD::GetAtomicIndex(RpAtomic *pstAtomic)
{
	RwInt32 nAtomicIndex	= 0;
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

				if(!strcmp(pszUserDataArrayName, "AtomicIndex"))
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
VOID AcuLOD::SetLODCallback(RpClump *pstClump)
{
	RpClumpForAllAtomicsSet BaseAtomicSet;

	memset(&BaseAtomicSet, 0, sizeof(RpClumpForAllAtomicsSet));
	RpClumpForAllAtomics(pstClump, SetBaseAtomicCB, &BaseAtomicSet);

	for(INT16 nAtomicCount = 0; nAtomicCount < BaseAtomicSet.m_unCount; ++nAtomicCount)
	{
		RpLODAtomicSetLODCallBack(BaseAtomicSet.m_pastAtomic[nAtomicCount], LODCallback);
		RpLODAtomicHookRender(BaseAtomicSet.m_pastAtomic[nAtomicCount]);
	}
}

/******************************************************************************
* Purpose : LOD callback!
*
* 022403. Bob Jung
******************************************************************************/
RwInt32 AcuLOD::LODCallback(RpAtomic *atomic)
{
	if((!g_pstMainFrame)/* || (!g_pstCameraFrame)*/)
	{
		OutputDebugString("LODCallback() Error AcuLOD::g_pstMainFrame�̳� AcuLOD::g_pstCameraFrame�� �����Ǿ� ���� �ʽ��ϴ�!!!\n");
		return 0; // error
	}

	RwV3d	atomicPos, mainPos, temp;
	RwReal	distance;

/*	RwV3d	cameraAt;

    atomicPos	= RwFrameGetLTM(RpAtomicGetFrame(atomic))->pos;
	mainPos		= RwFrameGetLTM(g_pstMainFrame)->pos;
	cameraAt	= RwFrameGetLTM(g_pstCameraFrame)->at;

	RwV3dSub(&temp, &atomicPos, &mainPos);
	distance = RwV3dDotProduct(&temp, &cameraAt);*/

	atomicPos	= RwFrameGetLTM(RpAtomicGetFrame(atomic))->pos;
	mainPos		= RwFrameGetLTM(g_pstMainFrame)->pos;

	RwV3dSub(&temp, &atomicPos, &mainPos);
	distance = RwV3dLength(&temp);

	///////////////////////////////////////////////////////
	// ���� distance�� �����´�.
	UINT32	*pulDistance;

	PVOID	pData			= NULL;
	RpClump *parentClump	= RpAtomicGetClump(atomic);

	AcuObject::GetClumpType(parentClump, NULL, NULL, &pData);

	if(pData)
	{
		pulDistance = (UINT32 *)(pData);
	}
	else
	{
		if(!g_pulDefaultDistance)
			return 0;
		
		pulDistance = g_pulDefaultDistance;
	}
	///////////////////////////////////////////////////////

    for(RwInt32 lodIndex = 0; lodIndex < AGPDLOD_MAX_NUM; lodIndex++)
    {
		if(distance < pulDistance[lodIndex])
        {
            return lodIndex;
        }
    }

    return AGPDLOD_MAX_NUM - 1;
}

/******************************************************************************
******************************************************************************/