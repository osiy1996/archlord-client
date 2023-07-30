//@{ Jaewon 20041207
// copied from rtltmap & modified.
//@} Jaewon
/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   polypack.h                                                 -*
 *-                                                                         -*
 *-  Purpose :   RtAmbOcclMap toolkit                                       -*
 *-                                                                         -*
 *===========================================================================*/


#ifndef __POLYPACK_H__
#define __POLYPACK_H__


/*===========================================================================*
 *--- Include files ---------------------------------------------------------*
 *===========================================================================*/

/* Needed for memset */
#include <string.h>
#include <rwcore.h>
#include <rpworld.h>
#include <rprandom.h>
#include <rpcollis.h>
#include <rtbary.h>
#include <rpdbgerr.h>
#include <rppvs.h>
#include <AcuRpMatD3DFx/AcuRpMatD3DFx.h>
#include <AcuRtAmbOcclMap/AcuRtAmbOcclMap.h>


/*===========================================================================*
 *--- Local Global Variables ------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Local defines ---------------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Local types -----------------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*/

typedef union
{
    RwTexCoords uv;
    RwUInt32    intUV[2];
}
LtMapIntUV;

/* Mesh topology stuff */
#define VERTTRILISTSIZE  7
#define TRIEDGELISTSIZE  3
#define EDGETRILISTSIZE  8

typedef struct LtMapVertTriList LtMapVertTriList;
struct LtMapVertTriList
{
    RwUInt32         list[VERTTRILISTSIZE];
    LtMapVertTriList *next;
};

typedef struct LtMapTriEdgeList LtMapTriEdgeList;
struct LtMapTriEdgeList
{
    RwUInt32 list[TRIEDGELISTSIZE];
    RwUInt32 pad;
};

typedef struct LtMapEdgeTriList LtMapEdgeTriList;
struct LtMapEdgeTriList
{
    RwUInt32 list [EDGETRILISTSIZE];
    RwUInt8  side [EDGETRILISTSIZE];
    LtMapEdgeTriList *next;
};

typedef struct LtMapTopology LtMapTopology;
struct LtMapTopology
{
    RwUInt32     numTris;
    RwUInt32     numEdges;

    RwUInt32     numVerts; /* This is welded vertices count */
    RwUInt32     *wVerts;

    LtMapVertTriList    *vertTriLists;
    LtMapVertTriList    *weldvertTriLists;
    LtMapTriEdgeList    *triEdgeLists;
    LtMapEdgeTriList    *edgeTriLists;
};

typedef struct
{
    RxVertexIndex   vertIndex[3];
    RpMaterial      *mat;
}
LtMapPoly;

typedef struct LtMapPolySet
{
    RwSList        *members;
    RwV3d           worldOrigin;  /* The origin in world-space of this
                                   * polyset's bounding rectangle */
    RwV2d           size;         /* The dimensions of this polyset's bounding
                                   * rectangle */
    RwTexCoords     uvOrigin;     /* The origin in lightmap-space of this
                                   * polyset's bounding rectangle */
    RwReal          density;      /* World-space sample density of this
                                   * polyset */
    RwV3d          *vertices;     /* [sector|geom]->vertices (handy once
                                   * objects share a lightmap) */
    RwTexCoords    *texCoords;    /* [sector|geom]->texCoords */
} LtMapPolySet;

/* With the new packing scheme, the difference in space-wastage between
 * MAXRECTS 16 and 32 is a couple of percent (8 isn't too bad either),
 * but in some cases that'll be 'the straw', so we'll leave it :) */
#define MAXRECTS 32

typedef struct
{
    RwTexture *lightMap;
    RwUInt32   size;
    RwUInt32   count;
    RwRect     rects[MAXRECTS];

    rpLtMapSampleMap  *sampleMap;
}
LtMapSlot;

typedef struct
{
    RpWorld    *world;
    LtMapSlot  *slots;
}
LtMapAllocUVData;

typedef struct
{
    AmbOcclMapObjectData *objectData;
    RpMaterialList  *matList;
    RwUInt32         matListWindowBase;
    RwInt32          numVertices;
    RwV3d           *vertices;
    RwTexCoords     *texCoords;
    RwInt32          numTriangles;
    RpTriangle      *triangles;
    RwMatrix        *matrix;
    RpGeometry      *geomSource;
}
LtMapAllocUVObjectData;


/*===========================================================================*
 *--- Toolkit SPI Functions -------------------------------------------------*
 *===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern RwUInt32
_rtLtMapClassifyNormal(RwV3d *normal);

extern RpGeometry *
_rtLtMapGeometryResetUVs(RpGeometry *geom);

//@{ Jaewon 20050105
// createUVsOnly parameter added.
extern RpAtomic *
_rtLtMapAtomicAllocateUV(RtAmbOcclMapLightingSession *session,
                      RpAtomic *atomic, LtMapAllocUVData *uvData,
					  RwBool createUVsOnly);
//@} Jaewon

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __POLYPACK_H__ */

