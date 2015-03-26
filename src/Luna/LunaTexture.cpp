/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2015 Patrick McCarthy <mauve@sandwich.net>
  
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
    
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required. 
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
 */

/* This implementation of LunaTexture is just a frontend to PLGraph. */

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "PL/PLInternal.h"
#include "LunaInternal.h"

LTEXTURE LunaTexture::CreateFromFile(const DXCHAR *pFileName,
                                     eSurfaceFormat format,
                                     D3DCOLOR keyColor) {
    int surfaceID = PL_Surface_Load(pFileName);
    int handle;
    
    if (surfaceID < 0) {
        return -1;
    }
    
    if (keyColor != COLORKEY_DISABLE) {
        PL_Surface_ApplyTransparentColor(surfaceID, keyColor | 0xff000000);
    }
    
    handle = PL_Surface_ToTexture(surfaceID);
    PL_Surface_Delete(surfaceID);
    
    PL_Texture_SetWrap(handle, DXTRUE);
    
    return handle;
}

LTEXTURE LunaTexture::CreateFromLAG(const DXCHAR *pFileName,
                                    const DXCHAR *pDataName,
                                    eSurfaceFormat format) {
    /* Not supported yet. */
    return INVALID_TEXTURE;
}

void LunaTexture::Release(LTEXTURE texture) {
    PL_Texture_Release(texture);
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
