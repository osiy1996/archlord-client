//@{ Jaewon 20041206
// copied from rtltmap & modified.
//@} Jaewon
/*===========================================================================*
 *-                                                                         -*
 *-  Module  :   sample.c                                                   -*
 *-                                                                         -*
 *-  Purpose :   RtAmbOcclMap toolkit                                       -*
 *-                                                                         -*
 *===========================================================================*/

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

#include "image.h"
#include "sample.h"
#include "ltmapvar.h"
#include "vis.h"

/*===========================================================================*
 *--- Local Global Variables ------------------------------------------------*
 *===========================================================================*/

/*===========================================================================*
 *--- Local defines ---------------------------------------------------------*
 *===========================================================================*/

/* MINBARY is used to detect lightmap samples outside of polygons.
 * NOTE: MINBARY being greater than zero means that in rare cases
 *       two triangles can think that they *both* own texels along an
 *       edge that they share... if they're co-planar it'll be ok. */
#define MINBARY (0.0001f)




/* Detect loops in the area light group chains
 * (technique learned in my interview at nVidia! :) )*/
#if (defined(RWDEBUG))
#define LIGHTGROUPLOOPDETECT(lights, err)                                   \
MACRO_START                                                                 \
{                                                                           \
    RtLtMapAreaLightGroup *group1, *group2;                                 \
    RwUInt32 _ctr = 0;                                                      \
                                                                            \
    group1 = group2 = (lights);                                             \
                                                                            \
    while (NULL != group1)                                                  \
    {                                                                       \
        group1 = group1->next;                                              \
        if (_ctr&1)                                                         \
            group2 = group2->next;                                          \
        if (group1 == group2)                                               \
        {                                                                   \
            RwDebugSendMessage(rwDEBUGERROR, "",                            \
                "The current chain of area light groups contains a loop");  \
            (err) = TRUE;                                                   \
            break;                                                          \
        }                                                                   \
        _ctr++;                                                             \
    }                                                                       \
}                                                                           \
MACRO_STOP
#else  /* (defined(RWDEBUG)) */
#define LIGHTGROUPLOOPDETECT(lights, err) /* no op */
#endif /* (defined(RWDEBUG)) */


/*===========================================================================*
 *--- Local types -----------------------------------------------------------*
 *===========================================================================*/



/*===========================================================================*
 *--- Local variables -------------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- External variables ----------------------------------------------------*
 *===========================================================================*/


/*===========================================================================*
 *--- Local functions -------------------------------------------------------*
 *===========================================================================*/


/****************************************************************************
 LtMapTriangleMakeSphere
 */
RwSphere *
_rtLtMapTriangleMakeSphere(RwSphere *sphere, RwV3d *triVerts)
{
    RwV3d    temp;
    RwReal   newRad;

    RWFUNCTION(RWSTRING("_rtLtMapTriangleMakeSphere"));

    RWASSERT(NULL != sphere);

    /* This is just a guess! I haven't proven it will bound the triangle,
     * though my intuition tells me it will and it seems to work. I
     * suspect it can be recast more simply in terms of the centroid */
    RwV3dSub(&(sphere->center), &(triVerts[0]), &(triVerts[1]));
    RwV3dScale(&(sphere->center), &(sphere->center), 0.5f);
    sphere->radius = RwV3dLength(&(sphere->center));
    RwV3dAdd(&(sphere->center), &(sphere->center), &(triVerts[1]));

    /* By now, the sphere bounds the edge 0-1. Next, we expand
     * the sphere in the direction of vert 2, to contain it */
    RwV3dSub(&temp, &(triVerts[2]), &(sphere->center));
    newRad = RwV3dLength(&temp);

    if (newRad > sphere->radius)
    {
        RwV3dNormalize(&temp, &temp);
        newRad = 0.5f * (sphere->radius + newRad);
        RwV3dScale(&temp, &temp, (newRad - sphere->radius));
        RwV3dAdd(&(sphere->center), &(sphere->center), &temp);
        sphere->radius = newRad;
    }

    RWRETURN(sphere);
}

///****************************************************************************
// LtMapLightCB
// */
//static RpLight *
//LtMapLightCB(LtMapSamplePoint *sp, RpLight *light)
//{
//    RWFUNCTION(RWSTRING("LtMapLightCB"));
//
///* TODO[5]: POINT, SPOT AND SOFTSPOT SHARE THE SAME CODE UP UNTIL
// *          VISIBILITY IS DETERMINED TO BE TRUE (THEN THE
// *          ATTENTUATION CALCULATION DIFFERS), SO SHARE THIS CODE! */
//
//    if (RpLightGetType(light) == rpLIGHTPOINT)
//    {
//        RwV3d   *lightPos = RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(light)));
//        RwReal  radius    = RpLightGetRadius(light);
//        RwV3d   delta;
//
//        /* Are we within the light's ROI? */
//        RwV3dSub(&delta, lightPos, sp->pos);
//        if (RwV3dDotProduct(&delta, &delta) < radius * radius)
//        {
//            RwReal  dotProd;
//
//            /* Is the surface facing the light source? */
//            dotProd = RwV3dDotProduct(&delta, sp->normal);
//            if (dotProd > 0.0f)
//            {
//                RwRGBAReal result = {1, 1, 1, 1};
//                RwBool     visible;
//
//                visible = rtLtMapGlobals.visCallBack(
//                              rtLtMapGlobals.lightWorld, &result, sp->pos, lightPos, light);
//
//                if (FALSE != visible)
//                {
//                    /* RpLight lighting works in the 0-1 range, deliberately.
//                     * (the area light version, above, works in the 0-255
//                     * range) */
//                    const RwRGBAReal *lightColour = RpLightGetColor(light);
//                    RwReal length, atten;
//
//                    /* Attenuate for distance */
///* TODO[5]: DIVISION BY radius SHOULD BE SHARED OVER MANY SAMPLES FOR THIS
// *          LIGHT-SOURCE */
///* TODO[5]: length AND (1 / length) SHOULD BE GOT FROM rwSqrtInvSqrt() */
//                    length = RwV3dLength(&delta);
//                    atten = 1.0f - (length / radius);
//
//                    /* Attenuate for angle of incidence */
//                    atten *= dotProd / length;
//
//                    /* Add the new contribution */
//                    sp->color->red   += lightColour->red   * atten * result.red;
//                    sp->color->green += lightColour->green * atten * result.green;
//                    sp->color->blue  += lightColour->blue  * atten * result.blue;
//                    /* We don't care about alpha, it's used for flagging
//                     * lightmap texels as 'initialized' */
//                }
//            }
//        }
//    }
//    else if (RpLightGetType(light) == rpLIGHTAMBIENT)
//    {
//        /* RpLight lighting works in the 0-1 range, deliberately.
//         * (the area light version, above, works in the 0-255 range) */
//        RwRGBARealAdd(sp->color, sp->color, RpLightGetColor(light));
//    }
//    else if (RpLightGetType(light) == rpLIGHTDIRECTIONAL)
//    {
//        RwV3d           lightPos, *direction;
//        RwReal          dotProd, radius;
//        const RwBBox    *box;
//
//        /* We push the light outside the world so that in doing a line
//         * test to it we can be sure there's nothing between the sample
//         * and "infinity" (which is where the light's *meant* to be) */
//        box = RpWorldGetBBox(rtLtMapGlobals.lightWorld);
//        radius = RwRealAbs(box->sup.x - box->inf.x) +
//                 RwRealAbs(box->sup.y - box->inf.y) +
//                 RwRealAbs(box->sup.z - box->inf.z);
//        direction = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(light)));
//        RwV3dScale(&lightPos, direction, -radius);
//        RwV3dAdd(&lightPos, &lightPos, sp->pos);
//
//        /* Is the surface facing the light source? */
//        dotProd = RwV3dDotProduct(direction, sp->normal);
//        if (dotProd < 0.0f)
//        {
//            RwRGBAReal result = {1, 1, 1, 1};
//            RwBool     visible;
//
//            visible = rtLtMapGlobals.visCallBack(
//                          rtLtMapGlobals.lightWorld, &result, sp->pos, &lightPos, light);
//
//            if (FALSE != visible)
//            {
//                /* RpLight lighting works in the 0-1 range, deliberately.
//                 * (the area light version, above, works in the 0-255
//                 * range) */
//                const RwRGBAReal *lightColor = RpLightGetColor(light);
//                RwReal atten;
//
//                /* Attenuate for angle of incidence */
//                atten = -dotProd;
//
//                /* Add the new contribution */
//                sp->color->red   += atten * lightColor->red   * result.red;
//                sp->color->green += atten * lightColor->green * result.green;
//                sp->color->blue  += atten * lightColor->blue  * result.blue;
//                /* We don't care about alpha, it's used for flagging lightmap
//                 * texels as 'initialized' */
//            }
//        }
//    }
//    else if (RpLightGetType(light) == rpLIGHTSPOT)
//    {
//        RwV3d *lightPos = RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(light)));
//        RwV3d *lightAt  = RwMatrixGetAt( RwFrameGetLTM(RpLightGetFrame(light)));
//        RwReal radius   = RpLightGetRadius(light);
//        RwV3d  delta;
//
//        /* Are we within the light's ROI? */
//        RwV3dSub(&delta, lightPos, sp->pos);
//        if (RwV3dDotProduct(&delta, &delta) < radius*radius)
//        {
//            RwReal dotProd;
//
//            /* Is the surface facing the light source? */
//            dotProd = RwV3dDotProduct(&delta, sp->normal);
//            if (dotProd > 0.0f)
//            {
//                RwRGBAReal result = {1, 1, 1, 1};
//                RwBool     visible;
//
//                visible = rtLtMapGlobals.visCallBack(
//                              rtLtMapGlobals.lightWorld, &result, sp->pos, lightPos, light);
//
//                if (FALSE != visible)
//                {
//                    /* RpLight lighting works in the 0-1 range, deliberately.
//                     * (the area light version, above, works in the 0-255
//                     * range) */
//                    const RwRGBAReal *lightColour = RpLightGetColor(light);
//                    RwReal length, maxDist, projDist, atten;
//
//                    /* Work out if the sample is inside the spotlight's cone
//                     * (we handily already have the trig done for us!),
//                     * allowing for cone angles up to 180 degrees (a point
//                     * light in effect). */
//                    length = RwV3dLength(&delta);
//
//                    /* Note the "minus" - that's why the test is "<" */
//                    maxDist = length * light->minusCosAngle;
//                    projDist = RwV3dDotProduct(&delta, lightAt);
//
//                    if (projDist < maxDist)
//                    {
//                        /* Attenuate for distance */
///* TODO[5]: DIVISION BY radius SHOULD BE SHARED OVER MANY SAMPLES FOR THIS
// *          LIGHT-SOURCE */
///* TODO[5]: length AND (1 / length) SHOULD BE GOT FROM rwSqrtInvSqrt() */
//                        atten = 1.0f - (length / radius);
//
//                        /* Attenuate for angle of incidence */
//                        atten *= dotProd / length;
//
//                        /* Add the new contribution */
//                        sp->color->red   += lightColour->red   * atten * result.red;
//                        sp->color->green += lightColour->green * atten * result.green;
//                        sp->color->blue  += lightColour->blue  * atten * result.blue;
//                        /* We don't care about alpha, it's used for flagging
//                         * lightmap texels as 'initialized' */
//                    }
//                }
//            }
//        }
//    }
//    else if (RpLightGetType(light) == rpLIGHTSPOTSOFT)
//    {
//        RwV3d *lightPos = RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(light)));
//        RwV3d *lightAt  = RwMatrixGetAt( RwFrameGetLTM(RpLightGetFrame(light)));
//        RwReal radius   = RpLightGetRadius(light);
//        RwV3d  delta;
//
//        /* Are we within the light's ROI? */
//        RwV3dSub(&delta, lightPos, sp->pos);
//        if (RwV3dDotProduct(&delta, &delta) < radius*radius)
//        {
//            RwReal dotProd;
//
//            /* Is the surface facing the light source? */
//            dotProd = RwV3dDotProduct(&delta, sp->normal);
//            if (dotProd > 0.0f)
//            {
//                RwRGBAReal result = {1, 1, 1, 1};
//                RwBool     visible;
//
//                visible = rtLtMapGlobals.visCallBack(
//                        rtLtMapGlobals.lightWorld, &result, sp->pos, lightPos, light);
//                if (FALSE != visible)
//                {
//                    /* RpLight lighting works in the 0-1 range, deliberately.
//                     * (the area light version, above, works in the 0-255
//                     * range) */
//                    const RwRGBAReal *lightColour = RpLightGetColor(light);
//                    RwReal length, maxDist, projDist, normalize, atten;
//
//                    /* Work out if the sample is inside the spotlight's cone
//                     * (we handily already have the trig done for us!),
//                     * allowing for cone angles up to 180 degrees (a point
//                     * light in effect). */
//                    length = RwV3dLength(&delta);
//
//                    /* Note the "minus" - that's why the test is "<" */
//                    maxDist = length * light->minusCosAngle;
//                    projDist = RwV3dDotProduct(&delta, lightAt);
//
//                    if (projDist < maxDist)
//                    {
//                        /* Attenuate for distance */
///* TODO[5]: DIVISION BY radius SHOULD BE SHARED OVER MANY SAMPLES FOR THIS
// *          LIGHT-SOURCE */
///* TODO[5]: length AND (1 / length) SHOULD BE GOT FROM rwSqrtInvSqrt() */
//                        atten = 1.0f - (length / radius);
//
//                        /* Attenuate for spotlight falloff (quadratic falloff
//                         * across the cone, kinda), note that "+ maxDist" is
//                         * really "- maxDist" since we used *minus*CosAngle.
//                         * Hence falloff is zero at the centre of the cone and
//                         * complete at the boundary of the cone */
//                        normalize = (length + maxDist);
//
///* TODO[5]: IF YOU ADD 0.0001F TO NORMALIZE, YOU CAN AVOID THE TEST BELOW
// *          (YOU STILL GET ZERO WHEN YOU SHOULD AND THE COLOUR ERROR IS TINY
// *          IN ALL OTHER CASES) */
//                        RWASSERT(normalize >= 0.0f);
//
//                        if (normalize > 0.0f)
//                        {
//                            normalize = (length + projDist) / normalize;
//                            atten *= (1.0f - normalize * normalize);
//                        }
//
//                        /* Attenuate for angle of incidence */
//                        atten *= dotProd / length;
//
//                        /* Add the new contribution */
//                        sp->color->red   += lightColour->red   * atten * result.red;
//                        sp->color->green += lightColour->green * atten * result.green;
//                        sp->color->blue  += lightColour->blue  * atten * result.blue;
//                        /* We don't care about alpha, it's used for flagging
//                         * lightmap texels as 'initialized' */
//                    }
//                }
//            }
//        }
//    }
//#ifdef RWDEBUG
//    else
//    {
//        static RwBool first = TRUE;
//        if (FALSE != first)
//        {
//            RwDebugSendMessage(rwDEBUGMESSAGE, "LtMapLightCB",
//                "Unrecognised light type encountered during illumination");
//        }
//    }
//#endif /* RWDEBUG */
//
//    RWRETURN(light);
//}


/*===========================================================================*
 *--- Local functions -------------------------------------------------------*
 *===========================================================================*/

static RpLight *
LtMapGatherLightCB(RpLight *light, void *data)
{
    RWFUNCTION(RWSTRING("LtMapGatherLightCB"));

    if ((light->lightFrame != RWSRCGLOBAL(lightFrame)) &&
        (RpLightGetFlags(light) & rtLtMapGlobals.lightObjectFlag) )
    {
        RwSList *list = (RwSList *)data;
        *(RpLight **)rwSListGetNewEntry(list,
            rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_EVENT) = light;
        light->lightFrame = RWSRCGLOBAL(lightFrame);
    }
    RWRETURN(light);
}

static RpWorldSector *
LtMapWorldSectorGatherLightCB(RpWorldSector *sector, void *data)
{
    RWFUNCTION(RWSTRING("LtMapWorldSectorGatherLightCB"));

    RpWorldSectorForAllLights(sector, LtMapGatherLightCB, data);

    RWRETURN(sector);
}




/*===========================================================================*
 *--- Local functions -------------------------------------------------------*
 *===========================================================================*/


static RwBool
LtMapObjectForAllSamples(RtAmbOcclMapLightingSession *session,
                         LtMapObjectSampleData *sampleData,
                         LtMapLightingData *lightingData,
                         RwBool compressedNormals)
{
    RpTriangle *triangles;
    RwImage    *image = (RwImage *)NULL;
    RwV3d      *worldPos;
    RwV3d      *normals;
    RwV3d      *triBary;
    RwRGBA     *results;
    RpMaterial *material;
    RwBool      hasLightMap;
    RwUInt32    width = 0, height = 0, arraySize = 0;
    RwUInt32    numSamples, numTriangles;
    RwUInt32    i, j, k;
    RwBool      result = TRUE;
    AmbOcclMapWorldData *worldData;

    RWFUNCTION(RWSTRING("LtMapObjectForAllSamples"));

    worldData = RPAMBOCCLMAPWORLDGETDATA(sampleData->world);

    /* Flag area light meshes which are not visible from this object */
    //LtMapBackFaceCullALMeshWRTObject(sampleData->world, sampleData->bSphere);

    if (NULL != sampleData->objectData->lightMap)
    {
        RWASSERT(sampleData->objectData->numSamples > 0);

        /* Reuse/update the pixels in the image associated with
         * the object's lightmap (may be partially lit already) */

        /* To do proper incremental lighting, you need the alpha channel
         * of the lightmap-parallel RwImages, so when lightmaps are loaded
         * from disk, you don't have this info. Hence we clear the lightmap
         * here in this case. In subsequent re-lights, the lighting values
         * will get overwritten as you'd expect, with no extraneous
         * clearing */

/* TODO[6][ACT]: SHOULD USE _RpLMRGBRenderTriangle() HERE TO ONLY CLEAR TEXELS
 *              WRITTEN TO BY *THIS* OBJECT (REMEMBER, OBJECTS MAY SHARE
 *              LIGHTMAPS!) (MADE OBSOLETE BY THE BELOW) */

/* TODO[6][ACT]: YOU COULD PUT IN A HUGE AMOUNT OF EFFORT AND "WORK OUT" THE
 *              ALPHA CHANNEL... SO YOU COPY THE RASTER'S RGB INTO THE
 *              LIGHTMAP (UNDOING DARKMAPPING) AND THEN MAYBE CALL
 *              LtMapWorldObjectForAllSamples()
 *              FROM INSIDE ITSELF HERE, W/ A NEW BOOL PARAMETER TO SAY "ONLY
 *              WRITE TO THE ALPHA CHANNEL, DON'T BOTHER CALLING THE LIGHTING
 *              CALLBACKS", SO THAT THE CORRECT ALPHA VALUES GET SET UP
 *              (DON'T FORGET, YOU'D HAVE TO CACHE, MODIFY (TO ENSURE ALL THE
 *              IMAGE GETS INITIALISED) AND RESTORE currentSamples, ETC) */

        image = session->lightMapImg;

        width  = RwImageGetWidth(image);
        height = RwImageGetHeight(image);

    }
    else
    {
        RWASSERT(NULL == sampleData->objectData->lightMap);
        RWASSERT(   0 == sampleData->objectData->numSamples);
    }

    numTriangles = sampleData->numTriangles;

    arraySize = (width * height) / 2;

    /* Ensure we have enough room to light all the object's
     * vertices in one go, if we're doing vertex lighting */
    if ((sampleData->objectData->flags & rtAMBOCCLMAPOBJECTVERTEXLIGHT) &&
        (arraySize < sampleData->numVertices))
    {
        arraySize = sampleData->numVertices;
        /* There's never any need to alloc more samples than we'll do per
         * slice */

    }

    /* This function should never have been called if none
     * of this object's samples needed to be processed! */
    RWASSERT(arraySize > 0);

    worldPos =  (RwV3d *)RwMalloc(arraySize * sizeof(RwV3d),
        rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
    if( NULL == worldPos )
    {
        return FALSE;
    }

    normals  =  (RwV3d *)RwMalloc(arraySize * sizeof(RwV3d),
        rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
    if( NULL == normals )
    {
        RwFree(worldPos);
        return FALSE;
    }

    triBary  =  (RwV3d *)RwMalloc(arraySize * sizeof(RwV3d),
        rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
    if( NULL == triBary )
    {
        RwFree(worldPos);
        RwFree(normals);
        return FALSE;
    }

    results  = (RwRGBA *)RwMalloc(arraySize * sizeof(RwRGBA),
        rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
    if( NULL == results )
    {
        RwFree(worldPos);
        RwFree(normals);
        RwFree(triBary);
        return FALSE;
    }

    triangles = sampleData->triangles;
    for (i = 0;i < sampleData->numTriangles;i++)
    {
        /* For each triangle, generate world-space positions of lightmap
         * texels */
        RwV3d    triVerts[3], triNormals[3], edge[2];
        RwPlane  triPlane;
        RwV2d    vScreen[3];
        RwReal   length;
        RwUInt16 matIndex;
        RwInt32 vertIndex[3];
        RwSphere triSphere;
        RwV3d   *pos, bias;
        RwReal   scalar;

        if (TRUE == compressedNormals)
        {
            for (j = 0;j < 3;j++)
            {
                RpVertexNormal packedNormal;

                vertIndex[j] = triangles->vertIndex[j];
                if (NULL != sampleData->texCoords)
                {
                    vScreen[j].x = width  * sampleData->texCoords[vertIndex[j]].u;
                    vScreen[j].y = height * sampleData->texCoords[vertIndex[j]].v;
                }

                triVerts[j]  = sampleData->vertices[vertIndex[j]];
                if (NULL != sampleData->packedNormals)
                {
                    packedNormal = sampleData->packedNormals[vertIndex[j]];
                }
                RPV3DFROMVERTEXNORMAL(triNormals[j], packedNormal);
            }
            matIndex = triangles->matIndex;
            triangles++;
        }
        else
        {
            for (j = 0;j < 3;j++)
            {
                vertIndex[j] = triangles->vertIndex[j];
                if (NULL != sampleData->texCoords)
                {
                    vScreen[j].x  = width  * sampleData->texCoords[vertIndex[j]].u;
                    vScreen[j].y  = height * sampleData->texCoords[vertIndex[j]].v;
                }

                triVerts[j] = sampleData->vertices[vertIndex[j]];
                if (NULL != sampleData->normals)
                {
                    triNormals[j] = sampleData->normals[vertIndex[j]];
                }
            }
            matIndex = triangles->matIndex;
            triangles++;
        }

        /* Transform the verts/normals into world-space */
        if (NULL != sampleData->matrix)
        {
            RwV3dTransformPoints(triVerts, triVerts, 3, sampleData->matrix);
            RwV3dTransformVectors(triNormals, triNormals, 3, sampleData->matrix);
        }

        /* Create a sphere bounding the current world triangle */
        _rtLtMapTriangleMakeSphere(&triSphere, triVerts);

        /* Frustum-test the triangle */
        if (NULL != lightingData->camera)
        {
            RwFrustumTestResult testFrustum = rwSPHEREINSIDE;

            testFrustum = RwCameraFrustumTestSphere(lightingData->camera, &triSphere);
            if (rwSPHEREOUTSIDE == testFrustum)
            {
                continue;
            }
        }

        material = rpMaterialListGetMaterial(sampleData->matList, sampleData->matListWindowBase + matIndex);
        hasLightMap = RtAmbOcclMapMaterialGetFlags(material) & rtAMBOCCLMAPMATERIALAMBOCCLMAP;

        /*
         * Do any vertex lighting first.
         */
        if ((sampleData->objectData->flags & rtAMBOCCLMAPOBJECTVERTEXLIGHT) ||
            (RtAmbOcclMapMaterialGetFlags(material) & rtAMBOCCLMAPMATERIALVERTEXLIGHT))
        {
            RWASSERT(NULL != sampleData->preLights);

            {
                for (j=0; j<3; j++)
                {
                    worldPos[j] = triVerts[j];
                    normals[j] = triNormals[j];
                }

                /* NOTE: we set the triBary array to NULL, since we are
                *       sampling at vertices, not inside triangles */
                if (!lightingData->sampleCB(results, worldPos, NULL, 3,
                        sampleData->lights, sampleData->numLights, normals) )
                {
                    result = FALSE;

                    i = sampleData->numTriangles + 1;

                    break;
                }
            }

            /* Write back RGBA into the vertex prelights (protect alpha) */
            for (j = 0; j<3; j++)
            {
                results[j].alpha = sampleData->preLights[vertIndex[j]].alpha;
                sampleData->preLights[vertIndex[j]] = results[j];
            }
        }

        /*
         * The following is lightmap generation. So continue only if there
         * are lightmaps.
         */
        if (NULL == sampleData->objectData->lightMap)
        {
            if (NULL != lightingData->progressCB)
            {
                RwReal progress;

                progress = (RwReal) (session->numTriNVert + i) * session->invTotalTriNVert;

/* TODO[6][AAB]: PASS SECTOR/ATOMIC POINTER TO THE PROGRESSCB SO PEOPLE CAN WORK OUT WHERE THEY ARE */
                if (!lightingData->progressCB(rtAMBOCCLMAPPROGRESSUPDATE, progress))
                {
                    result = FALSE;

                    i = sampleData->numTriangles + 1;

                    break;
                }
            }
            continue;
        }

        if ((vScreen[0].x == vScreen[1].x) && (vScreen[0].y == vScreen[1].y))
            continue;
        if ((vScreen[1].x == vScreen[2].x) && (vScreen[1].y == vScreen[2].y))
            continue;

        if ((vScreen[0].x == vScreen[1].x) && (vScreen[1].x == vScreen[2].x))
            continue;
        if ((vScreen[0].y == vScreen[1].y) && (vScreen[1].y == vScreen[2].y))
            continue;

        /* Generate a normal (before we flip facedness) */
        RwV3dSub(&(edge[0]), &triVerts[2], &triVerts[1]);
        RwV3dSub(&(edge[1]), &triVerts[0], &triVerts[1]);

        RwV3dCrossProduct(&triPlane.normal, &edge[0], &edge[1]);
        length = RwV3dDotProduct(&triPlane.normal, &triPlane.normal);
        length *= worldData->lightMapDensity * worldData->lightMapDensity;
        if (material)
        {
            RwReal density = RtAmbOcclMapMaterialGetAmbOcclMapDensityModifier(material);
            length *= density * density;
        }
        /* Slivers get filled in by the 'dilate' process in RtLtMapIlluminate
         * (numerical precision makes the calculated normal unsafe here) */
/* TODO[2][ABI]: BE MORE RIGOROUS -> THIS SHOULD WORK OUT FLOATING-POINT NORMAL
 *              ANGLE ERROR AND REJECT ON THAT BASIS. ALSO ENSURE THAT TRIS WIDER
 *              THAN 2 TEXELS ARE NEVER FLAGGED AS SLIVERS (IF THEY ARE, DILATE WILL
 *              FAIL TO FILL ALL THE PIXELS, SO THE LIGHTMAP RESOLUTION IS TOO HIGH) */
        if (length < _rpAmbOcclMapGlobals.sliverAreaThreshold)
        {
            continue;
        }

        /* finish triPlane */
        rwInvSqrt(&scalar, length);
        RwV3dScale(&triPlane.normal, &triPlane.normal, scalar);
        triPlane.distance = RwV3dDotProduct(&triPlane.normal, &triVerts[0]);

        /* Flag area light triangles which are back-face-culled
         * w.r.t the current world triangle as non-visible */
/* TODO[5][AAH]: AT LEAST SOME OF THIS EN-MASSE CULLING IS PROBABLY NOT WORTH IT
*         IF NUMSAMPLES IS LOW (E.G 1), SO TEST NUMSAMPLES HERE AND DO A
*         CUT-DOWN VERSION (CLEAR ALL RELEVANT FLAGS TO VISIBLE OR SET A MASK TO
*         SEND TO THE SAMPLECB SO WE DON'T HAVE TO RUN THROUGH ALL LIGHT-MESHES) */
        //LtMapBackFaceCullALTriWRTWorldPoly(&triPlane, &triSphere);

        /* Deal without vertex normals and allow for flat-shading */
        if ((RtAmbOcclMapMaterialGetFlags(material) & rtAMBOCCLMAPMATERIALFLATSHADE) ||
            ((NULL == sampleData->normals) &&
             (NULL == sampleData->packedNormals)) )
        {
            triNormals[0] = triNormals[1] = triNormals[2] = triPlane.normal;
/* TODO[2][ABC]: FOR FLAT-SHADING, SHOULD WE USE THE AVERAGE OF THE THREE VERTEX
 *              NORMALS? (I MEAN, IF IT'S GOT VARYING VERTEX NORMALS THEN THEY'RE
 *              THERE FOR A REASON, RIGHT?)
 *                SHOULD WE NOT MODIFY BACK-FACE CULLING FOR NON-ORTHOGONAL NORMALS?
 *              WE'RE TRYING TO SIMULATE A CURVED SURFACE, SO A LIGHT WHICH IS BACK-FACE
 *              CULLED BY THE TRIANGLE ISN'T NECESSARILY IN THE BACK-SPACE OF THE THE
 *              SURFACE AT THE VERTICES. */
        }

        /* We move our sample points slightly away from the surface,
         * so that collision rays cast from these points won't
         * intersect the surface due to numerical inaccuracy. */

/* TODO[3][ABE][AAY]: NEED TO REASSESS WHETHER/HOW TO DO THIS SHIFTING
*              NOTE: I TRIED IT ON RAGE'S AIRPORT LEVEL AND IT CAUSED ERRORS
*              AT AIRPORT WING EDGES DUE TO SAMPLES ENDING UP ON THE WRONG SIDES
*              OF THE WING (PUSHING IN THE OPPOSITE DIRECTION WOULD CAUSE PROBLEMS
*              FOR AN INVERTED WING, THOUGH ADMITTEDLY YOU WOULDN'T SEE THAT AS
*              OBVIOUSLY).
*               PUSHING CLAMPED (OR EVEN NON-CLAMPED!) POINTS SLIGHTLY INSIDE
*              TRIANGLE BOUNDARIES MAY HELP.
*               WE MIGHT EXPOSE A VARIABLE TO DEFINE THIS SHIFT DISTANCE
*              (CAN BE ZERO OR NEGATIVE). */
        RwV3dScale(&bias, &triPlane.normal, 0.005f);
        RwV3dAdd(&triVerts[0], &triVerts[0], &bias);
        RwV3dAdd(&triVerts[1], &triVerts[1], &bias);
        RwV3dAdd(&triVerts[2], &triVerts[2], &bias);

        /* Flip back-facing triangles */
        if (((vScreen[0].x - vScreen[1].x) * (vScreen[2].y - vScreen[1].y)) <=
            ((vScreen[0].y - vScreen[1].y) * (vScreen[2].x - vScreen[1].x)))
        {
            RwV3d tmp3d;
            RwV2d tmp2d;

            tmp2d         = vScreen[2];
            vScreen[2]    = vScreen[1];
            vScreen[1]    = tmp2d;

            tmp3d         = triVerts[2];
            triVerts[2]   = triVerts[1];
            triVerts[1]   = tmp3d;

            tmp3d         = triNormals[2];
            triNormals[2] = triNormals[1];
            triNormals[1] = tmp3d;
        }

        /* Generate world-space positions across triangle surface */
        pos = _rtLtMapLMWCRenderTriangle(worldPos, vScreen, triVerts);
        numSamples = pos - worldPos;

        /* NOTE: numSamples could feasibly be zero for non-slivers and non-zero
         *       for slivers, it depends on sample density. Sliver-rejection is
         *       about inaccurate polygon normal determination. */

        /* Should we start lighting samples yet? */

        if  (numSamples > 0)
        {
            RtBaryTransform transform;
            RtBaryV4d bary;
            RwReal area;
            RwBool success;

            /* Use RtBary to convert these into barycentric coords */

/* TODO[2][ACV]: SHOULD HAVE GLOBAL STATE TO TOGGLE BARYCOORD PRODUCTION OFF
*              (MAYBE IT'S SLOW -> WASTEFUL IF UNUSED) */
            success = RtBaryGetTransform(transform, &area,
                    &(triVerts[0]), &(triVerts[1]), &(triVerts[2]));
            RWASSERT(FALSE != success);

            for (j = 0;j < numSamples;j++)
            {
                RtBaryWeightsFromV3d(bary, transform, &(worldPos[j]));
                triBary[j].x = bary[0];
                triBary[j].y = bary[1];
                triBary[j].z = bary[2];
            }

            /* This code clamps greedy sample points to triangle boundaries
             * (it was put in in lieu of Rabin's G_CLAMP update to the expren
             *  rasteriser but this worked and we had to ship, so...)
             *
             * The method is:
             *   o if we're outside the tri then at least one barycentric coord will be negative
             *   o the point will be outside the edge opposite the point for this coord
             *   o to shift the point *perpendicular* to that edge, do some vector maths (project)
             *   o points can be outside of more than one edge at a time, so you need
             *     to deal with those cases (this time you just shift to the appropriate corner)
             *
             * Note that this still doesn't solve the case where two triangles share a
             * exclusively-greedy sample. Their respective clamped versions of this sample
             * could be pretty far apart (and on either side of a shadow :o/ ). But hey,
             * that's the price of tri-strips! :) */

            for (j = 0;j < numSamples;j++)
            {
                RwBool greedy = FALSE;
                RwV3d  newPos;

                for (k = 0;k < 3;k++)
                {
/* TODO[5][ACA]: GREEDY-SAMPLE-DETECTION COULD BE OPTIMISED IF THE RASTERIZER FLAGGED
*          THESE SAMPLES BY FILLING A BIT-FIELD (WE COULD ALSO THEN USE THAT TO FILL
*          THE ALPHA CHANNEL AFTER THE SampleCB, SO THAT *COULD* OVERWRITE ALPHA SAFELY)
*           ALSO, IF WE COULD FILL RESULTS[] FROM THE EXISTING IMAGE WE COULD AVOID
*          CLAMPING/USING GREEDY SAMPLES ALREADY FILLED BY NON-GREEDY SAMPLES. */
                    if (rtLTMAPGETCOORD(&(triBary[j]), k) < MINBARY)
                    {
                        RwUInt8 incWrap[4] = {1, 2, 0};
                        RwV3d corner2ToPoint, edge2To3;
                        RwReal temp;
                        RwUInt32 two, three;

                        two   = incWrap[k];
                        three = incWrap[two];

                        greedy = TRUE;

                        /* Do some vector maths (project the point onto the edge associated
                         * with (perpendicular to) this barycentric coord), barycentric
                         * coords SUCK for moving perpendicular to triangle edges */
                        RwV3dSub(&edge2To3, &triVerts[three], &triVerts[two]);
                        RwV3dSub(&corner2ToPoint, &worldPos[j], &triVerts[two]);
                        temp  = RwV3dDotProduct(&edge2To3, &corner2ToPoint);
                        temp /= RwV3dDotProduct(&edge2To3, &edge2To3);
                        RwV3dScale(&newPos, &edge2To3, temp);
                        RwV3dAdd(&newPos, &newPos, &triVerts[two]);

                        /* Deal with corner cases: */
                        if (temp < 0)
                        {
                            /* Nearer to the second corner */
                            newPos = triVerts[two];
                        }
                        else
                        {
                            RwV3d corner3ToPoint, edge3To2;

                            RwV3dSub(&edge3To2, &triVerts[two], &triVerts[three]);
                            RwV3dSub(&corner3ToPoint, &worldPos[j], &triVerts[three]);
                            temp = RwV3dDotProduct(&edge3To2, &corner3ToPoint);

                            if (temp < 0)
                            {
                                /* Nearer to the third corner */
                                newPos = triVerts[three];
                            }
                        }
                        break;
                    }
                }

                if (FALSE != greedy)
                {

                    /* Calc the baryCentric coords of the new worldPos */
                    RtBaryWeightsFromV3d(bary, transform, &newPos);

                    worldPos[j] = newPos;
                    triBary[j].x = bary[0];
                    triBary[j].y = bary[1];
                    triBary[j].z = bary[2];

                    /* We now do the greedy/non-greedy alpha flagging here, when we
                     * know whether texels are greedy or not - this is so that clamped
                     * texels can't overwrite non-clamped ones. This now absolutely
                     * relies on alpha NOT being overwritten in the sampleCallBack */
                    results[j].alpha = rpAMBOCCLMAPGREEDILYUSEDALPHA;
                }
                else
                {
                    results[j].alpha = rpAMBOCCLMAPUSEDALPHA;
                }
            }

            /* The hasLightMap can't really be triggered, or can it ? */
            if (FALSE == hasLightMap)
            {
                /* Non-emitter, non-lightmapped objets sont noir. */
                RwRGBA fillColor = {0, 0, 0, 0};

                for (j = 0;j < numSamples;j++)
                {
                    /* NOTE: we still use the appropriate greedy/non
                     *       alpha values, as calculated above */
                    fillColor.alpha = results[j].alpha;
                    results[j] = fillColor;
                }
            }
            else
            {
                /* Generate interpolated normals */
                for (j = 0;j < numSamples;j++)
                {
                    RtBaryV3dFromWeights(&(normals[j]), (RwReal *)&(triBary[j]),
                        &(triNormals[0]), &(triNormals[1]), &(triNormals[2]));
                    RwV3dNormalize(&(normals[j]), &(normals[j]));
                }

                if (!lightingData->sampleCB(results, worldPos, triBary, numSamples,
                         sampleData->lights, sampleData->numLights, normals) )
                {
                    result = FALSE;

                    i = sampleData->numTriangles + 1;

                    break;
                }
            }

            /* Write back RGBA into LM */
            RWASSERT(NULL != image);

            _rtLtMapLMRGBRenderTriangle(image, vScreen, results);

            if (NULL != lightingData->progressCB)
            {
                RwReal progress;

                progress = (RwReal) (session->numTriNVert + i) * session->invTotalTriNVert;

/* TODO[6][AAB]: PASS SECTOR/ATOMIC POINTER TO THE PROGRESSCB SO PEOPLE CAN WORK OUT WHERE THEY ARE */
                if (!lightingData->progressCB(rtAMBOCCLMAPPROGRESSUPDATE, progress))
                {
                    result = FALSE;

                    i = sampleData->numTriangles + 1;

                    break;
                }
            }
        } /* if (startedLightingSamples) */
    } /* for (triangles) */

    if (TRUE == result)
        session->numTriNVert += sampleData->numTriangles;

    RwFree(results);
    RwFree(triBary);
    RwFree(normals);
    RwFree(worldPos);

    /* NOTE: The lightmap raster gets updated during the dilate process */

    RWRETURN(result);
}


/****************************************************************************
 LtMapAtomicForAllSamples
 */
static RwBool
LtMapAtomicForAllSamples(
    RtAmbOcclMapLightingSession *session,
    RpWorld           *world,
    RpAtomic          *atomic,
    LtMapLightingData *lightingData)
{
    LtMapObjectSampleData sampleData =
        {NULL, NULL, NULL, 0, NULL, NULL, NULL,
         0, 0, NULL, NULL, NULL, NULL, NULL, NULL, 0};
    AmbOcclMapObjectData *objectData;
    RpGeometry      *geom;
    RwInt32          i, numSamples;
    RwBool          result;

    RWFUNCTION(RWSTRING("LtMapAtomicForAllSamples"));

    RWASSERT(NULL != atomic);
    RWASSERT(RpAtomicGetFlags(atomic) & rpATOMICRENDER);


    /* NOTE: this takes into account vertex lighting samples */
    numSamples = RtAmbOcclMapAtomicGetNumSamples(atomic);
    RWASSERT(numSamples > 0);

    geom = RpAtomicGetGeometry(atomic);
    RWASSERT(NULL != geom);

    objectData = RPAMBOCCLMAPATOMICGETDATA(atomic);
    RWASSERT((NULL == objectData->lightMap) || (objectData->flags & rtAMBOCCLMAPOBJECTAMBOCCLMAP));

    sampleData.world        = world;
    sampleData.objectData   = objectData;
    sampleData.matList      = &(geom->matList);
    sampleData.numTriangles = RpGeometryGetNumTriangles(geom);
    sampleData.triangles    = RpGeometryGetTriangles(geom);
    sampleData.bSphere      = RpAtomicGetWorldBoundingSphere(atomic);
    sampleData.matrix       = (RwMatrix *) RwFrameGetLTM(RpAtomicGetFrame(atomic));
    sampleData.numVertices  = RpGeometryGetNumVertices(geom);
    sampleData.vertices     = RpMorphTargetGetVertices(RpGeometryGetMorphTarget(geom, 0));
    sampleData.normals      = RpMorphTargetGetVertexNormals(RpGeometryGetMorphTarget(geom, 0));
    sampleData.texCoords    = RpGeometryGetVertexTexCoords(geom, 2);
    sampleData.preLights    = RpGeometryGetPreLightColors(geom);

    if (objectData->flags & rtAMBOCCLMAPOBJECTVERTEXLIGHT)
    {
        RWASSERT(NULL != sampleData.preLights);
        RWASSERT(NULL != sampleData.normals);

        /* Get the prelights reinstanced */
        geom->lockedSinceLastInst |= rpGEOMETRYLOCKPRELIGHT;
    }

    /* Gather up all the RpLights affecting this object */
    if (RpGeometryGetFlags(geom) & rpGEOMETRYLIGHT)
    {
        RwSList *list = rwSListCreate(sizeof(RpLight *),
                                      rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
        RWASSERT(NULL != list);

        RWSRCGLOBAL(lightFrame)++;
        rtLtMapGlobals.lightObjectFlag = rpLIGHTLIGHTATOMICS;
        RpAtomicForAllWorldSectors(atomic, LtMapWorldSectorGatherLightCB, (void *)list);
        sampleData.numLights = rwSListGetNumEntries(list);
        if (sampleData.numLights > 0)
        {
            sampleData.lights = (RpLight **) RwMalloc(sampleData.numLights * sizeof(RpLight *),
                                                      rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
            RWASSERT(NULL != sampleData.lights);

            for (i = 0;i < (RwInt32)sampleData.numLights;i++)
            {
                sampleData.lights[i] = *(RpLight **)rwSListGetEntry(list, i);
            }
        }

        rwSListDestroy(list);
    }
    else
    {
        sampleData.numLights = 0;
        sampleData.lights = (RpLight **)NULL;
    }

    result = LtMapObjectForAllSamples(session, &sampleData, lightingData, FALSE);

    if (NULL != sampleData.lights)
    {
        RwFree(sampleData.lights);
    }

    RWRETURN(result);
}


/*===========================================================================*
 *--- Toolkit Stealth API Functions -----------------------------------------*
 *===========================================================================*/

static RpWorldSector *
IlluminateSamplesBoxSectorCollisCB(RpIntersection *is,
                                   RpWorldSector  *sector,
                                   void           *data)
{
    RWFUNCTION(RWSTRING("IlluminateSamplesBoxSectorCollisCB"));

    rtLtMapGlobals.lightObjectFlag = rpLIGHTLIGHTWORLD | rpLIGHTLIGHTATOMICS;

    RpWorldSectorForAllLights(sector, LtMapGatherLightCB, data);

    RWRETURN(sector);
}

/* This function (currently stealth, maybe API later) allows you to light an
 * arbitrary set of (world-space) points, rather than just atomics/sectors. */
RwRGBA *
_rtLtMapIlluminateSamples(RwV3d   *samplePositions,
                          RwV3d   *sampleNormals,
                          RwRGBA  *results,
                          RwUInt32 numSamples,
                          RtAmbOcclMapLightingSession *session)
{
    RtAmbOcclMapIlluminateSampleCallBack sampleCB;
    RwSList  *lightList;
    RpLight **lights = (RpLight **)NULL;
    RwUInt32  numLights;
    RwBBox    bBox;
    RwSphere  sphere;
    RpIntersection is;
    RwV3d     vTmp;
    RwUInt32  i;

    RWFUNCTION(RWSTRING("_rtLtMapIlluminateSamples"));

    RWASSERT(NULL != samplePositions);
    RWASSERT(NULL != sampleNormals);
    RWASSERT(NULL != results);
    RWASSERT(NULL != session);
    RWASSERT(NULL != session->world);

    /* Set up globals, callbacks... */
    rtLtMapGlobals.lightWorld  = session->world;
    rtLtMapGlobals.visCallBack = session->visCallBack;
    if (NULL == rtLtMapGlobals.visCallBack)
    {
        rtLtMapGlobals.visCallBack = RtAmbOcclMapDefaultVisCallBack;
    }

    sampleCB = session->sampleCallBack;
    if (NULL == session->sampleCallBack)
    {
        sampleCB = RtAmbOcclMapDefaultSampleCallBack;
    }
    /* Sod the progress CB in here... */

    /* Create a bounding box/sphere for the sample points */
    RwBBoxInitialize(&bBox, &(samplePositions[0]));
    for (i = 1; i < numSamples;i++)
    {
        RwBBoxAddPoint(&bBox, &(samplePositions[i]));
    }
    /* Put a sphere around it */
    RwV3dSub(&vTmp, &(bBox.sup), &(bBox.inf));
    RwV3dScale(&vTmp, &vTmp, 0.5f);
    RwV3dAdd(&(sphere.center), &vTmp, &(bBox.inf));
    sphere.radius = RwV3dLength(&vTmp);
    /* If we only have one point, I think zero radius is OK for the sphere. */

    /* Gather up the RpLights affecting the sample points */
    lightList = rwSListCreate(sizeof(RpLight *),
        rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
    RWASSERT(NULL != lightList);
    RWSRCGLOBAL(lightFrame)++;

    /* Find all sectors that the BBox intersects and
     * gather lights from each one: */
    is.t.box = bBox;
    is.type  = rpINTERSECTBOX;
    RpWorldForAllWorldSectorIntersections(session->world, &is,
        IlluminateSamplesBoxSectorCollisCB, (void *)lightList);

    numLights = rwSListGetNumEntries(lightList);
    if (numLights > 0)
    {
        lights = (RpLight **)RwMalloc(numLights * sizeof(RpLight *),
                                      rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
        RWASSERT(NULL != lights);
        for (i = 0;i < numLights;i++)
        {
            lights[i] = *(RpLight **)rwSListGetEntry(lightList, i);
        }
    }

    rwSListDestroy(lightList);

    results = sampleCB(results, samplePositions,
            (RwV3d *)NULL, numSamples, lights, numLights, sampleNormals);
    RWASSERT(NULL != results);

    if (NULL != lights)
        RwFree(lights);

    RWRETURN(results);
}

/*===========================================================================*
 *--- Toolkit Stealth API Functions -----------------------------------------*
 *===========================================================================*/

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapDefaultSampleCallBack is the default
 * \ref RtAmbOcclMapIlluminateSampleCallBack callback, called from within
 * \ref RtAmbOcclMapIlluminate.
 *
 * This callback calculates lightmap sample values using standard
 * \ref RpLight lighting equations and area lighting equations.
 * The results, for a scene containing no area lights, should look
 * similar to a scene, with only vertex lighting, which has been
 * tesselated to the resolution of lightmaps. The main visual
 * difference would be that shadows are calculated for lightmaps
 * but not for vertex lighting.
 *
 * This function makes use of the \ref RtAmbOcclMapIlluminateVisCallBack
 * passed to \ref RtAmbOcclMapIlluminate, for determining visibility
 * between lightmap samples and lights. Object flags (see
 * \ref RtAmbOcclMapObjectFlags) and material flags (see \ref RtAmbOcclMapMaterialFlags)
 * are taken into account.
 *
 * See \ref RtAmbOcclMapIlluminate for further details on lighting
 * calculations.
 *
 * The matd3dfx plugin must be attached before using this function.
 *
 * \param  results          A pointer to an array of \ref RwRGBA sample color values
 * \param  samplePositions  A pointer to an array of \ref RwV3d values specifying the
 *                          world-space positions of each of the samples in the results array
 * \param  baryCoords       A pointer to an array of \ref RwV3d values specifying the
 *                          barycentric coordinates (within the current polygon) of
 *                          each of the samples in the results array
 * \param  numSamples       The length of the results, samplePositions, baryCoords and normals arrays
 * \param  lights           An array of pointers to the \ref RpLight's affecting the current object
 * \param  numLights        The length of the lights array
 * \param  normals          A pointer to an array of \ref RwV3d values specifying the
 *                          unit normals of each of the samples in the results array
 *
 * \return  A pointer to the results array on success, otherwise NULL
 *
 * \see RtAmbOcclMapIlluminate
 * \see RtAmbOcclMapDefaultVisCallBack
 */
//RwRGBA *
///* TODO[5]: PUT ALL PARAMS IN A STRUCT SO THEY NEEDN'T COST UNLESS THEY'RE GONNA BE USED */
//RtAmbOcclMapDefaultSampleCallBack(RwRGBA    *results,
//								  RwV3d     *samplePositions,
//								  RwV3d *   baryCoords,
//								  RwUInt32  numSamples,
//								  RpLight   **lights,
//								  RwUInt32  numLights,
///* TODO[2][ACV]: THE PARAMETERS HERE NEED TO SUPPORT RESAMPLING OF EXISTING LIGHTMAPS... HOW?
// *         PASS IN OBJECT AND TYPE? AND THEN EITHER TRIANGLE OR POLYGON POINTER FOR USE W/ BARYCOORDS?
// *         ARGH... YOU ALSO NEED SAMPLE AREA (CAN YOU GET IT ELSEWHERE?) SO YOU CAN USE A BOX FILTER
// *        AAAH, FAR TOO MANY PARAMETERS JUST TO SUPPORT EXISTING-LIGHTMAP RESAMPLING!!!
// *        HMM, IT'D BE SO MUCH EASIER TO DO RESAMPLING IN THE VISCALLBACK! IF YOU HAD ONE
// *        WHITE RPLIGHT LIGHT IN THE SCENE, YOU COULD INTERSECT W/ THE SOURCE POLY AND GET A
// *        NICE ATOMIC/SECTOR COLLISION... SURELY THERE'S A SIMPLE WAY TO DO THIS AND DEMO IT IN THE
// *        EXAMPLE BY "RESAMPLE LIGHTING" MERELY DOING A LOW-RES COPY OF THE BASE TEXTURES
// *                             RpTriangle __RWUNUSED__ *triangle, */
//                             RwV3d         *normals)
//{
//    LtMapSamplePoint sp;
//    RwUInt32 i, j;
//
//    RWAPIFUNCTION(RWSTRING("RtAmbOcclMapDefaultSampleCallBack"));
//
//    for (i = 0;i < numSamples;i++)
//    {
//        RwRGBAReal newPrelit = {0, 0, 0, 1};
//        RwReal     maxCol;
//
//        sp.pos    = &(samplePositions[i]);
//        sp.color  = &(newPrelit);
//        sp.normal = &(normals[i]);
//
//        /* Now calculate RpLight contributions */
//        for (j = 0;j < numLights;j++)
//        {
//            LtMapLightCB(&sp, lights[j]);
//        }
//
//        /* Hue-constant clamping again */
//        maxCol = newPrelit.red;
//        if (newPrelit.green > maxCol)
//            maxCol = newPrelit.green;
//        if (newPrelit.blue > maxCol)
//            maxCol = newPrelit.blue;
//        if (maxCol > 1)
//        {
//            RwRGBARealScale(&newPrelit, &newPrelit, 1 / maxCol);
//        }
//
//        /* Dither (doesn't seem to make much difference).
//         * Add a random number between 0 and 1 such that 0.1 has a
//         * 1 in 10 chance of being rounded up to 1... kinda thing. */
//        newPrelit.red += (1 / 255.0f) * (RpRandom() / (RwReal)RPRANDMAX);
//        if (newPrelit.red < 0)
//            newPrelit.red = 0;
//        if (newPrelit.red > 1)
//            newPrelit.red = 1;
//        newPrelit.green += (1 / 255.0f) * (RpRandom() / (RwReal)RPRANDMAX);
//        if (newPrelit.green < 0)
//            newPrelit.green = 0;
//        if (newPrelit.green > 1)
//            newPrelit.green = 1;
//        newPrelit.blue += (1 / 255.0f) * (RpRandom() / (RwReal)RPRANDMAX);
//        if (newPrelit.blue < 0)
//            newPrelit.blue = 0;
//        if (newPrelit.blue > 1)
//            newPrelit.blue = 1;
//
//        /* Huzzah! */
//        results[i].red   = (RwUInt8)RwFastRealToUInt32(255 * newPrelit.red);
//        results[i].green = (RwUInt8)RwFastRealToUInt32(255 * newPrelit.green);
//        results[i].blue  = (RwUInt8)RwFastRealToUInt32(255 * newPrelit.blue);
//    }
//
//    RWRETURN(results);
//}
// ambient occlusion sampling
static void generateRandomDirectionWithRejectionSampling(RwV3d *direction, RwV3d *normal)
{
	RwV3d v;
	while(1)
	{
		v.x = 2.0f * (RpRandom() / (RwReal)RPRANDMAX) - 1.0f;
		v.y = 2.0f * (RpRandom() / (RwReal)RPRANDMAX) - 1.0f;
		v.z = 2.0f * (RpRandom() / (RwReal)RPRANDMAX) - 1.0f;
		if(v.x*v.x + v.y*v.y + v.z*v.z > 1.0f) 
			continue;
		if(RwV3dDotProduct(&v, normal) < 0)
			continue;

		if(RwV3dNormalize(&v, &v) > 0)
		{
			RwV3dAssign(direction, &v);
			return;
		}
		else
			continue;
	}
}
RwRGBA *
RtAmbOcclMapDefaultSampleCallBack(RwRGBA    *results,
								  RwV3d     *samplePositions,
								  RwV3d *   baryCoords,
								  RwUInt32  numSamples,
								  RpLight   **lights,
								  RwUInt32  numLights,
								  RwV3d         *normals)
{
	LtMapSamplePoint sp;
	RwUInt32 i, j;

	RWAPIFUNCTION(RWSTRING("RtAmbOcclMapDefaultSampleCallBack"));

	for(i = 0;i < numSamples; ++i)
	{
		RwRGBAReal newPrelit = {0, 0, 0, 1};
		RwBool visible;
		const RwBBox *box;
		RwReal radius, accessibility;
		RwV3d direction, targetPos, avgUnoccluded = {0, 0, 0};
		//@{ Jaewon 20050408
		// RwUInt32 -> RwReal
		RwReal numUnoccluded = 0;
		//@} Jaewon
		//@{ Jaewon 20050408
		RwReal t;
		//@} Jaewon

		sp.pos    = &(samplePositions[i]);
		sp.color  = &(newPrelit);
		sp.normal = &(normals[i]);

		/* Now calculate accessibility & average unoccluded direction */ 

        /* We push the target outside the world so that in doing a line
         * test to it we can be sure there's nothing between the sample
         * and "infinity" (which is where the target's *meant* to be) */
        box = RpWorldGetBBox(rtLtMapGlobals.lightWorld);
        radius = RwRealAbs(box->sup.x - box->inf.x) +
                 RwRealAbs(box->sup.y - box->inf.y) +
                 RwRealAbs(box->sup.z - box->inf.z);
		for(j=0; j < _rpAmbOcclMapGlobals.numRays; ++j)
		{
			generateRandomDirectionWithRejectionSampling(&direction, sp.normal);
			RwV3dScale(&targetPos, &direction, radius);
			RwV3dAdd(&targetPos, &targetPos, sp.pos);
			//@{ Jaewon 20050408
			// A parameter '&t' added. 
			visible = rtLtMapGlobals.visCallBack(rtLtMapGlobals.lightWorld, NULL, sp.pos, &targetPos, NULL, &t);
			//@} Jaewon

			//@{ Jaewon 20050408
			if(_rpAmbOcclMapGlobals.renderStyle & rpAMBOCCLMAPSTYLEINDOOR)
			{
				RwV3dScale(&direction, &direction, t);
				RwV3dAdd(&avgUnoccluded, &avgUnoccluded, &direction);
				numUnoccluded += t;
			}
			else
			//@} Jaewon
			if(visible)
			{
				RwV3dAdd(&avgUnoccluded, &avgUnoccluded, &direction);
				//@{ Jaewon 20050408
				// RwUInt32 -> RwReal
				//++numUnoccluded;
				numUnoccluded += 1.0f;
				//@} Jaewon
			}
		}

		RwV3dNormalize(&avgUnoccluded, &avgUnoccluded);
		accessibility = (float)numUnoccluded / (float)_rpAmbOcclMapGlobals.numRays;

		sp.color->red = 0.5f * (avgUnoccluded.x*accessibility + 1.0f);
		sp.color->green = 0.5f * (avgUnoccluded.y*accessibility + 1.0f);
		sp.color->blue = 0.5f * (avgUnoccluded.z*accessibility + 1.0f);

		/* Huzzah! */
		results[i].red   = (RwUInt8)RwFastRealToUInt32(255 * newPrelit.red);
		results[i].green = (RwUInt8)RwFastRealToUInt32(255 * newPrelit.green);
		results[i].blue  = (RwUInt8)RwFastRealToUInt32(255 * newPrelit.blue);
	}

	RWRETURN(results);
}

/**
 * \ingroup rtambocclmap
 * \ref RtAmbOcclMapIlluminate performs illumination of the lightmaps
 * attached to objects within a specified \ref RtAmbOcclMapLightingSession.
 *
 * This function traverses the objects specified by the received
 * \ref RtAmbOcclMapLightingSession structure. Only atomics flagged as rpATOMICRENDER
 * will be used. Note that the camera member of this structure is used.
 *
 * Lightmaps must have been created, by \ref RtAmbOcclMapAmbOcclMapsCreate,
 * for all of the sectors and atomics to be illuminated which have the
 * \ref rtAMBOCCLMAPOBJECTAMBOCCLMAP flag set. Vertex normals and prelight colors
 * should be present for objects with the \ref rtAMBOCCLMAPOBJECTVERTEXLIGHT flag
 * set - lightmap and vertex 'sample' colors will both be calculated within
 * this function. Lightmapped objects without vertex normals will be lit as
 * if flat-shaded (i.e polygon normals will be used rather than interpolated
 * vertex normals).
 *
 * The received \ref RtAmbOcclMapLightingSession can be used to cause this function to
 * light only a subset, or 'slice', of the objects contained within the session.
 * This may be useful to display realtime visual updates of the lighting
 * calculations as they are in progress.
 *
 * For example, within an app displaying a lightmapped world, calling
 * RtAmbOcclMapIlluminate, with the session's numObj member set to 1 and startObj
 * member set to zero, will cause the first object, world sector or atomic, in
 * the session to be lit and the lightmaps will appear updated onscreen. Setting
 * numObj to 0 will cause all objects in the session to be processed.
 *
 * Note that this sort of incremental lighting updates lightmap rasters after each
 * 'slice', such that the effect of moved lights will be visible in the regions
 * where the light received from the light has noticeably increased or decreased.
 * Internally, RwImages are used to keep track of state information such that this
 * incremental lighting is possible. When lightmaps are loaded from disk, these
 * images are not present, hence RtAmbOcclMapIlluminate will clear each lightmap before
 * performing the first illumination 'slice' that affects it.
 *
 * Note also that it is important not to change the world, camera or object lists
 * in the lighting session between slices, or RtLtMapIlluminate will not be able
 * to correctly keep track of progress. \ref RwCameraClone may be used to create
 * an unchanging clone camera for the session, such that the original camera (that
 * which is actually used to render into the currently displayed framebuffer) may
 * be moved around during lighting without causing problems.
 *
 * In addition to a set of objects to be illuminated, the \ref RtAmbOcclMapLightingSession
 * structure contains three (optional) pointers to callbacks which may be used to
 * overload the lighting process. The \ref RtAmbOcclMapIlluminateSampleCallBack may be
 * used to completely change the calculation of lightmap or vertex prelight samples.
 * The \ref RtAmbOcclMapIlluminateVisCallBack (which is called from within the default
 * sample callback) may be used to alter visibility calculations (visibility being
 * between lights and lightmap or vertex prelight sample points). The
 * \ref RtAmbOcclMapIlluminateProgressCallBack may be used to track lighting progress.
 * If any of these pointers in the lighting session are NULL, the default callback
 * is used instead (note that there is no default progress callback).
 *
 * The default sample and vis callbacks (\ref RtAmbOcclMapDefaultSampleCallBack and
 * \ref RtAmbOcclMapDefaultVisCallBack) implement lighting models for all RpLight
 * types and additionally area light sources. Sets of area light source samples
 * (created by \ref RtAmbOcclMapAreaLightGroupCreate) may be passed to this function,
 * to be used during lighting calculations. Light gathered from area light sources
 * is post-processed using a logarithmic scaling function which takes into account
 * the relationship between light intensity and the eye's perception of brightness.
 * This allows a high dynamic range of intensity, from 1 to 1024 (in units of the
 * emitter intensity specified by \ref RtAmbOcclMapMaterialSetAreaLightColor), with 255
 * mapping to 204. See \ref RtAmbOcclMapAreaLightGroupCreate for further details.
 *
 * Although lightmaps are illuminated for only a subset of the objects in a world,
 * all of the objects in the world will be used during the lighting calculations.
 * Hence a non-lightmapped atomic in the world will still cast shadows and shadows
 * may be cast from out-of frustum lights, by out-of-frustum objects.
 *
 * The flags rpGEOMETRYLIGHT and rpWORLDLIGHT are respected during lighting. Only
 * the lightmaps of atomics and sectors which are flagged as being lit will be
 * affected by \ref RpLight's during the lighting calculations. Similarly, only
 * \ref RpLight's flagged with rpLIGHTLIGHTATOMICS will affect the lightmaps of
 * atomics and only those flagged with rpLIGHTLIGHTWORLD will affect the lightmaps
 * of world sectors.
 *
 * RtAmbOcclMapIlluminate creates collision data for atomics and world sectors if it
 * is not present already (see \ref RpCollisionGeometryBuildData). If this data
 * cannot be built then lighting calculations will proceed much more slowly.
 *
 * The lightmaps can be sampled at a higher resolution and sampled down
 * to a lower resolution for display for better results. The sampling
 * resolution can be increased by setting the superSample parameter > 1. This
 * sets the sampling resolution as a scale relative to the lightmap's
 * size.
 *
 * The matd3dfx plugin must be attached before using this function.
 *
 * \param  session  A pointer to an \ref RtAmbOcclMapLightingSession structure specifying the
 *                  set of objects to be lit
 * \param  lights   An (optional) pointer to an \ref RtAmbOcclMapAreaLightGroup (potentially
 *                  chained to other such structures), containing area lights to use in
 *                  this illumination
 * \param  superSample  A \ref RwUInt32 specifying the sampling resolution
 *                      per lightmap texels
 *
 * \return  A RwInt32 value representing the number of objects processed
 *          on success, otherwise '-1'.
 *
 * \see RtAmbOcclMapLightingSessionInitialize
 * \see RtAmbOcclMapAmbOcclMapsCreate
 * \see RtAmbOcclMapAmbOcclMapsClear
 * \see RtAmbOcclMapImagesPurge
 * \see RtAmbOcclMapAreaLightGroupCreate
 * \see RtAmbOcclMapMaterialSetAreaLightColor
 * \see RtAmbOcclMapGetSliverAreaThreshold
 * \see RtAmbOcclMapSetSliverAreaThreshold
 */
RwInt32
RtAmbOcclMapIlluminate(RtAmbOcclMapLightingSession *session,
					   RwUInt32 superSample)
{
    LtMapLightingData lightingData;
    LtMapSessionInfo  sessionInfo;
    AmbOcclMapObjectData     *objectData;
    RwTexture       **touched;
    rpLtMapSampleMap  **touchedSampleMap;
    RwUInt32          numTouched, iwidth;
    RwBool            err = FALSE, result;
    RwInt32           i, j, startObj, endObj, numObj, count;

    RWAPIFUNCTION(RWSTRING("RtAmbOcclMapIlluminate"));

    RWASSERT(NULL != session);

    /* This is required to do collisions during lighting */
    RWASSERT(NULL != session->world);

    LtMapCollisionInitCache();
    
    /* Make sure the camera (if given) corresponds to this world
     * (otherwise you'll end up with a list of sectors from the
     *  wrong world, since it uses RwCameraForAllWorldSectorInFrustum) */
    if (NULL != session->camera)
    {
        RWASSERT(session->world == RwCameraGetWorld(session->camera));
    }

    RWASSERT(session->lightMapImg == NULL);
    RWASSERT(session->lightMap == NULL);
    RWASSERT(session->sampleMap == NULL);

    result = TRUE;

    session->superSample = superSample;
    iwidth = 0;

    /* Okay, so we use RwImages all the way through lightmap illumination.
     * RtAmbOcclMapAmbOcclMapsClear() clears rasters and creates images
     * (critically clearing the images alpha channel to an 'unused' value).
     * This gets called from RtLtMapLightMapsCreate.
     * When each sector (or portion thereof) is illuminated, it's image is
     * modified and, at the end, its raster is set from this image.
     * The only time during illumination that images won't exist and/or
     * correspond to the rasters is if you've just loaded a world off disk
     * and have decided to do incremental lighting to modify the lightmaps
     * that you've loaded. In this case, lightmaps will be cleared per-sector,
     * so as long as you relight a whole sector you'll be ok. */

    /* Count up how many samples we'll be doing (-> % progress) */
    RtAmbOcclMapLightingSessionGetNumSamples(session);

    /* Do we actually have anything to light? */
    if (session->totalTriNVert == 0)
    {
        RWRETURN(session->numObj);
    }

    session->invTotalTriNVert = (RwReal) (100.0) / (RwReal) session->totalTriNVert;
    session->numTriNVert = 0;

    /* Build up a struct holding data to be passed to the callbacks */
    lightingData.camera        = session->camera;
    lightingData.sampleCB      = session->sampleCallBack;
    if (NULL == lightingData.sampleCB)
    {
        lightingData.sampleCB  = RtAmbOcclMapDefaultSampleCallBack;
    }

    lightingData.progressCB    = session->progressCallBack;

    /* We always start at the beginning and skip sectors/polys up
     * to startSample (we don't know, before rasterization, how many
     * samples each poly requires, unfortunately) */
    /* lightingData.currentSample = 0; */

    /* Set up some stuff for our default callbacks */
    rtLtMapGlobals.visCallBack = session->visCallBack;
    if (NULL == rtLtMapGlobals.visCallBack)
        rtLtMapGlobals.visCallBack = RtAmbOcclMapDefaultVisCallBack;
    rtLtMapGlobals.lightWorld = session->world;

    /* Get local lists of sectors/atomics from the session
     * (culled w.r.t the incoming camera and object lists, if present) */
    _rtLtMapLightingSessionInfoCreate(&sessionInfo, session, TRUE);

    /* We keep track of which lightmaps are touched during lighting
     * and so which need updating with 'dilate' below. */
    numTouched = sessionInfo.numAtomics;
    touched = (RwTexture **) RwMalloc(numTouched * 2 * sizeof(RwTexture *),
                                      rwID_AMBOCCLMAPPLUGIN | rwMEMHINTDUR_FUNCTION);
    RWASSERT(NULL != touched);

    memset(touched, (RwUInt32)NULL, numTouched * 2 * sizeof(RwTexture *));
    touchedSampleMap = (rpLtMapSampleMap **) (((RwChar *) touched) + numTouched * sizeof(RwTexture *));
    numTouched = 0;

    if (NULL != session->progressCallBack)
    {
        session->progressCallBack(rtAMBOCCLMAPPROGRESSSTART, 0.0f);

        session->progressCallBack(rtAMBOCCLMAPPROGRESSSTARTSLICE, 0.0f);
    }

    /* Do it! */

    /* Number of objects processed. Can be different to the number objects
     * requested due to error or interrupt. */
    count = 0;

    startObj = (RwInt32) session->startObj;
    numObj = 0;
    endObj = 0;

    /* Do all the atomics second (no reason) */
    if ((TRUE == result) && ((session->numObj == 0) || (numObj < (RwInt32) session->numObj)))
    {
        startObj = session->startObj;
        if (startObj < 0)
            startObj = 0;

        if (session->numObj == 0)
        {
            endObj = sessionInfo.numAtomics;
        }
        else
        {
            endObj = startObj + (session->numObj - numObj);

            if (endObj > (RwInt32) sessionInfo.numAtomics)
                endObj = sessionInfo.numAtomics;
        }

        numObj = endObj - startObj;

        for (i = startObj; i < endObj; i++)
        {
            RpGeometry  *geom;
            RpAtomic    *atomic;
            RwTexture       *lightMap;
            RwUInt32        twidth;

            atomic = *(RpAtomic **)rwSListGetEntry(sessionInfo.localAtomics, i);

            /* Skip atomics without a lightmap (and without rtAMBOCCLMAPOBJECTVERTEXLIGHT set) */
            if ((RpAtomicGetFlags(atomic) & rpATOMICRENDER) == 0 || (RtAmbOcclMapAtomicGetNumSamples(atomic) == 0)
				//@{ Jaewon 20050110
				|| (RtAmbOcclMapAtomicGetFlags(atomic) & rtAMBOCCLMAPOBJECTNEEDILLUMINATION) == 0
				//@} Jaewon
				)
            {
                /* Still count this even if there were no samples. */
                count++;
                continue;
            }

            /* Make sure collision data exists for the current atomic */
            geom = RpAtomicGetGeometry(atomic);
            if (FALSE == RpCollisionGeometryQueryData(geom))
            {
                RpCollisionGeometryBuildData(geom, (RpCollisionBuildParam *)NULL);
            }

            objectData = RPAMBOCCLMAPATOMICGETDATA(atomic);
            lightMap = objectData->lightMap;

            /* Check if we need to download the sample image. This is due to
            * the texture changed between sectors. */
            if (lightMap != NULL)
            {
                twidth = RwRasterGetWidth(RwTextureGetRaster(lightMap));

                /* Create a new vismap for this texture and add it to the SList. */
                if (NULL == objectData->sampleMap)
                    objectData->sampleMap = _rpLtMapSampleMapCreate(twidth, twidth);

                if (lightMap != session->lightMap)
                {

                    /*
                    * Download the image into the texture.
                    */
                    if (session->lightMapImg != NULL)
                        _rtLtMapTextureSetFromLightMapImage(session->lightMap,
                            session->lightMapImg, session->sampleMap);

                    /* Check if we need to change the size of the sample image. Only
                    * need to check one dimension since the lightmap is square.
                    */
                    if ((twidth * superSample) > iwidth)
                    {
                        /* Destroy the old image. */
                        if (session->lightMapImg != NULL)
                        {
                            RwImageDestroy(session->lightMapImg);
                        }

                        /* Create the new image. */
                        iwidth = twidth * superSample;
                        session->lightMapImg = RwImageCreate(iwidth, iwidth, 32);
                        RwImageAllocatePixels(session->lightMapImg);

                        /* Should really exit and report error rather than
                        * assert.
                        */
                        RWASSERT(session->lightMapImg);
                    }

                    /* Upload the texture to the sample image. */
                    _rtLtMapLightMapImageSetFromTexture(lightMap,
                        session->lightMapImg, objectData->sampleMap);

                    /* Record the current active lightmap. */
                    session->lightMap = lightMap;
                    session->sampleMap = objectData->sampleMap;
                }
            }

            result = LtMapAtomicForAllSamples(session, session->world, atomic, &lightingData);

            if (FALSE == result)
            {
                /* Recieved a termination signal so stop further processing.
                * Could be an error or an interrrupt. So we do not count
                * this obj towards the objects processed.
                */

                i = endObj + 1;
            }
            else
            {
                objectData = RPAMBOCCLMAPATOMICGETDATA(atomic);
                lightMap = objectData->lightMap;

                /* Some samples were lit, so we need to update
                * the lightmap image/raster for this atomic */
                for (j = 0;j < (RwInt32) numTouched;j++)
                {
                    if (lightMap == touched[j])
                    {
                        lightMap = (RwTexture *)NULL;
                        break;
                    }
                }
                if (NULL != lightMap)
                {
                    touched         [numTouched] = lightMap;
                    touchedSampleMap[numTouched] = objectData->sampleMap;
                    numTouched++;
                }

                count++;

				//@{ Jaewon 20050110
				objectData->flags &= ~rtAMBOCCLMAPOBJECTNEEDILLUMINATION;
				//@} Jaewon
            }
        }
    }

    /* Download the last sample image. */
    if (session->lightMapImg != NULL)
        _rtLtMapTextureSetFromLightMapImage(session->lightMap,
            session->lightMapImg, session->sampleMap);

    if (NULL != session->progressCallBack)
    {
        session->progressCallBack(rtAMBOCCLMAPPROGRESSENDSLICE, 100.0f);

        session->progressCallBack(rtAMBOCCLMAPPROGRESSEND, 100.0f);
    }

    /* Dilate texels in order to fill the texels of sliver polygons,
     * which cannot be trusted during lighting since they likely have
     * dodgy surface normals. For each uninitialised texel, we merely
     * copy the average colour of its initialised neighbours. */
    for (i = 0;i < (RwInt32) numTouched;i++)
    {
        _rtLtMapDilate(touched[i], touchedSampleMap[i]);
    }

    RwFree(touched);

    /* Destroy the super sample image. */
    if (session->lightMapImg)
    {
        RwImageDestroy(session->lightMapImg);
    }
    session->lightMap = NULL;
    session->lightMapImg = NULL;
    session->sampleMap = NULL;

    _rtLtMapLightingSessionInfoDestroy(&sessionInfo);

    /* Return the number of object done */
    RWRETURN(count);
}
