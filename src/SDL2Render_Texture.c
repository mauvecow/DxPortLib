/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013 Patrick McCarthy <mauve@sandwich.net>
  
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

#include "DxInternal.h"

#ifdef DXPORTLIB_DRAW_SDL2_RENDER

#include "SDL2Render_DxInternal.h"

typedef struct TextureRef {
    SDL_Texture *texture;
    
    Uint32 format;
    
    int refCount;
} TextureRef;

static int s_AllocateTextureRefID(SDL_Texture *texture) {
    int textureID = PL_Handle_AcquireID(DXHANDLE_TEXTURE);
    TextureRef *textureref;
    
    if (textureID < 0) {
        return -1;
    }
    
    textureref = (TextureRef *)PL_Handle_AllocateData(textureID, sizeof(TextureRef));
    textureref->texture = texture;
    textureref->refCount = 0;
    
    SDL_QueryTexture(texture, &textureref->format, NULL, NULL, NULL);
    
    return textureID;
}

int PL_Texture_CreateFromSurface(SDL_Surface *surface, int hasAlphaChannel) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(PL_renderer, surface);
    if (texture == NULL) {
        return -1;
    }
    
    return s_AllocateTextureRefID(texture);
}

int PL_Texture_CreateFromDimensions(int width, int height, int hasAlphaChannel) {
    SDL_Texture *texture;
    int textureRefID;
    
    texture = SDL_CreateTexture(PL_renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                0,
                                width, height
                                );
    if (texture == NULL) {
        return -1;
    }
    
    textureRefID = s_AllocateTextureRefID(texture);
    if (textureRefID < 0) {
        SDL_DestroyTexture(texture);
        return -1;
    }
    
    return textureRefID;
}

int PL_Texture_AddRef(int textureID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
    textureref->refCount += 1;
    return 0;
}

int PL_Texture_Release(int textureID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
    textureref->refCount -= 1;
    if (textureref->refCount <= 0) {
        SDL_DestroyTexture(textureref->texture);
        PL_Handle_ReleaseID(textureID, DXTRUE);
    }
    return 0;
}

int PL_Texture_BlitSurface(int textureRefID, SDL_Surface *surface, const SDL_Rect *rect) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    SDL_Texture *texture;
    if (textureref == NULL || textureref->texture == NULL) {
        return -1;
    }
    
    texture = textureref->texture;
    
    /* Convert to target format if different. */
    if (textureref->format != surface->format->format) {
        SDL_Surface *tempSurface = SDL_ConvertSurfaceFormat(surface, textureref->format, 0);
        if (SDL_MUSTLOCK(tempSurface)) {
            SDL_LockSurface(tempSurface);
            SDL_UpdateTexture(texture, rect, tempSurface->pixels, tempSurface->pitch);
            SDL_UnlockSurface(tempSurface);
        } else {
            SDL_UpdateTexture(texture, rect, tempSurface->pixels, tempSurface->pitch);
        }
        SDL_FreeSurface(tempSurface);
    } else {
        if (SDL_MUSTLOCK(surface)) {
            SDL_LockSurface(surface);
            SDL_UpdateTexture(texture, rect, surface->pixels, surface->pitch);
            SDL_UnlockSurface(surface);
        } else {
            SDL_UpdateTexture(texture, rect, surface->pixels, surface->pitch);
        }
    }
    
    return 0;
}

int PL_Texture_RenderGetGraphTexture(int graphID, SDL_Texture **texture, SDL_Rect *rect) {
    int textureRefID = PL_Graph_GetTextureID(graphID, rect);
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    
    if (textureref == NULL) {
        return -1;
    }
    *texture = textureref->texture;
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_DRAW_SDL2_RENDER */
