/*****************************************************************************
* File : AcuLOD.h
*
* Desc :
*
* 022403 Bob Jung.
*****************************************************************************/

#ifndef __ACU_LOD_H__
#define __ACU_LOD_H__

#include <windows.h>

#include <rwcore.h>
#include <rpworld.h>
#include <rplodatm.h>
#include <rpusrdat.h>

#ifdef	_DEBUG
#pragma comment ( lib , "AcuLODD" )
#else
#pragma comment ( lib , "AcuLOD" )
#endif

#define ACULOD_CLUMP_MAX_ATOMIC 20

class AcuLOD
{
public:
			AcuLOD()	{}
	virtual	~AcuLOD()	{}

	// ���� �ҷ���� �� �ʹ�...
//	static VOID SetCameraFrame(RwFrame *pFrame);
	static VOID	SetMainFrame(RwFrame *pFrame);
	static VOID	SetDefaultDistance(UINT32 *pulDistance);

	// �ַ� ����ϴ� �ʹ�...
	static BOOL Initailize(RpClump *pstClump, CHAR *szInitDFFPath = NULL, BOOL bSetNextLevel = FALSE);
	static BOOL MakeLODLevel(RpClump *pstClump, CHAR *szLODDffPath, INT16 nLevel, CHAR *szDummyDffPath, BOOL bSetNextLevel = FALSE); // ���� Initailize�ϰ� ����ؾ� �Ѵ�.
	static VOID SetLODCallback(RpClump *pstClump);

	// ���������� ����ϴ� �ʹ�...
	static RwInt32	GetAtomicIndex(RpAtomic *pstAtomic);
	static RpClump	*ReadDFF(CHAR *szDFFPath);

	// Callback
	static RwInt32	LODCallback(RpAtomic *atomic);
	static RpAtomic *SetBaseAtomicCB(RpAtomic *atomic, void *data);
};

#endif // __ACU_LOD_H__

/******************************************************************************
******************************************************************************/