//@{ Jaewon 20041206
// copied from rtltmap & modified.
//@} Jaewon
/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   sample.h                                                   -*
 *-                                                                         -*
 *-  Purpose :   RtAmbOcclMap toolkit                                       -*
 *-                                                                         -*
 *===========================================================================*/

#ifndef __SAMPLE_H__
#define __SAMPLE_H__

/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/

/* Needed for memset */
#include "string.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rprandom.h"

#include "rpcollis.h"
#include "rtbary.h"

#include "rpdbgerr.h"

#include "rppvs.h"
#include "AcuRpMatD3DFx.h"
#include "AcuRtAmbOcclMap.h"

/*===========================================================================*
 *--- Local Global Variables ------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Local defines ---------------------------------------------------------*
 *===========================================================================*/

/* This is used to access vectors as arrays (handy for
 * for processing all three axes in a compact loop) */
#define rtLTMAPGETCOORD(_vctr, _axs) ( ((RwReal *)_vctr)[_axs] )

/*===========================================================================*
 *--- Local types -----------------------------------------------------------*
 *===========================================================================*/

typedef struct
{
    RwV3d      *pos;
    RwV3d      *normal;
    RwRGBAReal *color;
}
LtMapSamplePoint;

typedef struct
{
    RtAmbOcclMapIlluminateSampleCallBack   sampleCB;
    RtAmbOcclMapIlluminateProgressCallBack progressCB;

    RwCamera *camera;
}
LtMapLightingData;

typedef struct
{
    RpWorld         *world;
    AmbOcclMapObjectData *objectData;
    RpMaterialList  *matList;
    RwUInt32         numTriangles;
    RpTriangle      *triangles;
    const RwSphere  *bSphere;
    RwMatrix        *matrix;
    RwUInt32         matListWindowBase;
    RwUInt32         numVertices;
    RwV3d           *vertices;
    RwV3d           *normals;
    RpVertexNormal  *packedNormals;
    RwTexCoords     *texCoords;
    RwRGBA          *preLights;
    RpLight        **lights;
    RwUInt32         numLights;
}
LtMapObjectSampleData;

/*===========================================================================*
 *--- Toolkit SPI Functions -------------------------------------------------*
 *===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwSphere *
_rtLtMapTriangleMakeSphere(RwSphere *sphere, RwV3d *triVerts);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SAMPLE_H__ */

