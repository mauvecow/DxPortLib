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

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_DRAW_DIRECT3D9

#include "PL/PLInternal.h"

#include "PLD3D9Internal.h"

int PLD3D9_Texture_Bind(int textureRefID, int drawMode) {
    return 0;
}

int PLD3D9_Texture_Unbind(int textureRefID) {
    return 0;
}

int PLD3D9_Texture_BindFramebuffer(int textureRefID) {
    return -1;
}

int PLD3D9_Texture_HasAlphaChannel(int textureRefID) {
    return 0;
}

int PLD3D9_Texture_CreateFromSDLSurface(SDL_Surface *surface, int hasAlphaChannel) {
    int textureRefID;
    
    if (SDL_GetColorKey(surface, 0) >= 0) {
        hasAlphaChannel = DXTRUE;
    }
    
    textureRefID = PLD3D9_Texture_CreateFromDimensions(surface->w, surface->h, hasAlphaChannel);
    if (textureRefID < 0) {
        return -1;
    }
    
    PLD3D9_Texture_BlitSurface(textureRefID, surface, NULL);
    
    return textureRefID;
}

int PLD3D9_Texture_CreateFromDimensions(int width, int height, int hasAlphaChannel) {
    return -1;
}

int PLD3D9_Texture_CreateFramebuffer(int width, int height, int hasAlphaChannel) {
    return -1;
}

int PLD3D9_Texture_SetWrap(int textureRefID, int wrapState) {
    return 0;
}

int PLD3D9_Texture_AddRef(int textureRefID) {
    return 0;
}

int PLD3D9_Texture_Release(int textureRefID) {
    return 0;
}

int PLD3D9_Texture_BlitSurface(int textureRefID, SDL_Surface *surface, const PLRect *rect) {
    return 0;
}

int PLD3D9_Texture_RenderGetTextureInfo(int textureRefID, PLRect *rect, float *xMult, float *yMult) {
    return 0;
}

int PLD3D9_Texture_ClearAllData() {
    return 0;
}

#endif /* #ifdef DXPORTLIB_DRAW_OPEND3D9 */

