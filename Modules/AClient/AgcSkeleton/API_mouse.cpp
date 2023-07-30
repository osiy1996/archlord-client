#ifdef USE_API

/****************************************************************************
 *
 * mouse.c
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */
#ifdef RWMOUSE

#include <rwcore.h>

#include "mouse.h"
#include "mousedat.h"
#include "platform.h"

void
AcuMouse::rsMouseVisible(RwBool visible)
{
    DrawMouse = visible;

    return;
}

void
AcuMouse::rsMouseAddDelta(RwV2d * delta)
{
    RwV2dAdd(&MousePos, &MousePos, delta);

    return;
}

void
AcuMouse::rsMouseGetPos(RwV2d * pos)
{
    *pos = MousePos;

    return;
}

void
AcuMouse::rsMouseSetPos(RwV2d * pos)
{
    MousePos = *pos;

    return;
}

void
AcuMouse::rsMouseRender(RwCamera * camera)
{
    RwInt32 crw, crh;
    RwInt32 x, y;

    crw = RwRasterGetWidth(RwCameraGetRaster(camera)) - 1;
    crh = RwRasterGetHeight(RwCameraGetRaster(camera)) - 1;

    x = (RwInt32)MousePos.x;
    y = (RwInt32)MousePos.y;

#if ( defined( SKY ) )
    /*
     * PS2 builds use a cross-hair cursor, so offset the render 
     * position so that the center of the cursor corresponds to 
     * the mouse location...
     */
    x -= CURSORWIDTH / 2;
    y -= CURSORHEIGHT / 2;
#endif /* ( defined( SKY ) ) */

    /* clamp mouse to camera limits */

#if ( defined( SKY ) )
    
    if( x < 0)
    {
        x = 0;

        MousePos.x = (RwReal)(CURSORWIDTH / 2);
    }
    else if( (x + CURSORWIDTH) > crw )
    {
        x = crw - CURSORWIDTH;

        MousePos.x = (RwReal)(crw - CURSORWIDTH / 2);
    }

    if( y < 0 )
    {
        y = 0;

        MousePos.y = (RwReal)(CURSORHEIGHT /2);
    }
    else if( (y + CURSORHEIGHT) > crh )
    {
        y = crh - CURSORHEIGHT;

        MousePos.y = (RwReal)(crh - CURSORHEIGHT / 2);
    }

#else /* ( defined( SKY ) ) */

    if( x < 0 )
    {
        x = 0;

        MousePos.x = 0.0f;
    }
    else if( x > crw )
    {
        x = crw;

        MousePos.x = (RwReal)crw;
    }

    if( y < 0 )
    {
        y = 0;

        MousePos.y = 0.0f;
    }
    else if( y > crh )
    {
        y = crh;

        MousePos.y = (RwReal)crh;
    }

#endif /* ( defined( SKY ) ) */

    if( CursorRaster && DrawMouse )
    {
        if( RwRasterPushContext(RwCameraGetRaster(camera)) )
        {
            RwRasterRender(CursorRaster, x, y);

            RwRasterPopContext();
        }
    }

    return;
}


RwBool
AcuMouse::rsMouseTerm(void)
{
    if (CursorRaster)
    {
        RwRasterDestroy(CursorRaster);
        CursorRaster = NULL;
        return (FALSE);
    }

    return (TRUE);
}

RwBool
AcuMouse::rsMouseInit(void)
{
    RwInt32             rasterHeight;
    RwInt32             rasterWidth;
    RwInt32             rasterDepth;
    RwInt32             rasterFlags;
    RwImage            *image;

    image = RwImageCreate(CURSORWIDTH, CURSORHEIGHT, 32);

    RwImageSetStride(image, CURSORWIDTH * 4);
    RwImageSetPixels(image, MouseData);

    RwImageFindRasterFormat(image, rwRASTERTYPENORMAL,
                            &rasterWidth, &rasterHeight, &rasterDepth,
                            &rasterFlags);

    /* Gamma correct the image */
    RwImageGammaCorrect(image);

    /* Create a raster */
    CursorRaster = RwRasterCreate(rasterWidth, rasterHeight,
                                  rasterDepth, rasterFlags);

    if (!CursorRaster)
    {
        RwImageDestroy(image);

        return (FALSE);
    }

    /* Convert the image into the raster */
    RwRasterSetFromImage(CursorRaster, image);
    RwImageDestroy(image);

    /* If platform already has mouse support then turn it off */
    psMouseSetVisibility(TRUE);

    return (TRUE);
}

AcuMouse::AcuMouse()
{

	MousePos.x = 0.0f;
	MousePos.y = 0.0f;
	CursorRaster = NULL;
	DrawMouse = TRUE;


}



#endif /* RWMOUSE */

#endif // USE_API