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

#include "PLInternal.h"

#include "SDL2/PLSDL2Internal.h"

#include "SDL_image.h"

/* -------------------------------------------------------- SURFACE DATA */
/* Handles raw pixel surface data.
 * Very minimalistic right now, plans to remove SDL from this code
 * eventually... */

static int s_surfaceCount = 0;

typedef struct Surface {
    SDL_Surface *sdlSurface;
    
    int hasTransparencyFlag;
} Surface;

static Surface *s_GetSurface(int surfaceID) {
    return (Surface *)PL_Handle_GetData(surfaceID, DXHANDLE_SURFACE);
}

static int s_AllocateSurfaceID(SDL_Surface *sdlSurface, int hasTransparencyFlag) {
    int surfaceID;
    Surface *surface;
    
    surfaceID = PL_Handle_AcquireID(DXHANDLE_SURFACE);
    if (surfaceID < 0) {
        return -1;
    }
    
    surface = (Surface *)PL_Handle_AllocateData(surfaceID, sizeof(Surface));
    surface->sdlSurface = sdlSurface;
    surface->hasTransparencyFlag = hasTransparencyFlag;
    
    s_surfaceCount += 1;
    
    return surfaceID;
}

int PL_Surface_HasTransparency(int surfaceID) {
    Surface *surface = s_GetSurface(surfaceID);
    if (surface == NULL) {
        return DXFALSE;
    }
    
    return surface->hasTransparencyFlag;
}

int PL_Surface_ApplyTransparentColor(int surfaceID, unsigned int color) {
    Surface *surface = s_GetSurface(surfaceID);
    SDL_Surface *sdlSurface;
    SDL_PixelFormat *format;
    int hasAlphaChannel;
    
    if (surface == NULL) {
        return DXFALSE;
    }
    
    sdlSurface = surface->sdlSurface;
    hasAlphaChannel = DXFALSE;
    
    format = sdlSurface->format;
    
    if (format->format == SDL_PIXELFORMAT_INDEX8) {
        SDL_Palette *palette = format->palette;
        SDL_Color *colors = palette->colors;
        SDL_Color transColor;
        int ncolors = palette->ncolors;
        int i;
        
        transColor.r = (color >> 16) & 0xff;
        transColor.g = (color >> 8) & 0xff;
        transColor.b = (color) & 0xff;
        transColor.a = 255;
        
        for (i = 0; i < ncolors; ++i) {
            SDL_Color c = colors[i];
            
            if (c.r == transColor.r
                && c.g == transColor.g
                && c.b == transColor.b
            ) {
                c.a = 0;
                colors[i] = c;
                hasAlphaChannel = DXTRUE;
            }
        }
    } else if (sdlSurface->format->BitsPerPixel == 32) {
        unsigned int *pixels = sdlSurface->pixels;
        unsigned int transColor;
        int width = sdlSurface->w;
        int height = sdlSurface->h;
        int pitch = sdlSurface->pitch / 4;
        int x, y;
        
        transColor = (color >> 16) << format->Rshift
                     | (color >> 8) << format->Gshift
                     | (color) << format->Bshift
                     | (unsigned int)(0xff) << format->Ashift;
        
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                if (pixels[x] == transColor) {
                    pixels[x] = 0;
                    hasAlphaChannel = DXTRUE;
                }
            }
            pixels += pitch;
        }
    }
    
    if (hasAlphaChannel) {
        surface->hasTransparencyFlag = DXTRUE;
    }
    
    return surface->hasTransparencyFlag;
}

int PL_Surface_ApplyPMAToSDLSurface(SDL_Surface *sdlSurface) {
    if (sdlSurface->format->BitsPerPixel == 8) {
        SDL_Palette *palette = sdlSurface->format->palette;
        SDL_Color *colors = palette->colors;
        int ncolors = palette->ncolors;
        int i;
        
        for (i = 0; i < ncolors; ++i) {
            SDL_Color c = colors[i];
            
            if (c.a != 0xff) {
                c.r = (c.r * c.a) / 0xff;
                c.g = (c.g * c.a) / 0xff;
                c.b = (c.b * c.a) / 0xff;
            }
        }
    } else if (sdlSurface->format->BitsPerPixel == 32) {
        unsigned int *pixels = sdlSurface->pixels;
        int width = sdlSurface->w;
        int height = sdlSurface->h;
        int pitch = sdlSurface->pitch / 4;
        int x, y;
        
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                unsigned int p = pixels[x];
                unsigned int a = p >> 24;
                if (a != 0xff) {
                    if (a == 0) {
                        pixels[x] = 0;
                    } else {
                        pixels[x] = (((p & 0xff0000) * a / 0xff) & 0xff0000)
                                    | (((p & 0xff00) * a / 0xff) & 0xff00)
                                    | ((p & 0xff) * a / 0xff)
                                    | (a << 24);
                    }
                }
            }
            pixels += pitch;
        }
    }
    return 0;
}

int PL_Surface_ApplyPMAToSurface(int surfaceID) {
    Surface *surface = s_GetSurface(surfaceID);
    
    if (surface == NULL) {
        return 0;
    }
    return PL_Surface_ApplyPMAToSDLSurface(surface->sdlSurface);
}

int PL_Surface_FlipSurface(int surfaceID) {
    Surface *surface = s_GetSurface(surfaceID);
    SDL_Surface *sdlSurface;
    int w, h, pitch, x, y;
    unsigned char *pixels;
    
    if (surface == NULL) {
        return DXFALSE;
    }
    sdlSurface = surface->sdlSurface;
    
    w = sdlSurface->w;
    h = sdlSurface->h;
    pitch = sdlSurface->pitch;
    pixels = sdlSurface->pixels;
    
    switch(sdlSurface->format->BytesPerPixel) {
        case 1:
            for (y = h; y > 0; --y) {
                unsigned char *dest = pixels;
                unsigned char *src = pixels + w - 1;
                for (x = w; x >= 0; --x) {
                    dest[w-x] = src[x];
                }
                
                pixels += pitch;
            } 
            break;
        case 2:
            for (y = h; y > 0; --y) {
                unsigned short *dest = (unsigned short *)pixels;
                unsigned short *src = (unsigned short *)pixels + (w - 1);
                for (x = w - 1; x >= 0; --x) {
                    dest[w-x] = src[x];
                }
                
                pixels += pitch;
            } 
            break;
        case 3:
            for (y = h; y > 0; --y) {
                unsigned char *dest = pixels;
                unsigned char *src = pixels + ((w - 1) * 3);
                for (x = w; x > 0; --x) {
                    dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2];
                    dest += 3; src -= 3;
                }
                
                pixels += pitch;
            } 
            break;
        case 4:
            for (y = h; y > 0; --y) {
                unsigned int *dest = (unsigned int *)pixels;
                unsigned int *src = (unsigned int *)pixels + (w - 1);
                for (x = w - 1; x >= 0; --x) {
                    dest[w-x] = src[x];
                }
                
                pixels += pitch;
            } 
            break;
    }
    
    return DXTRUE;
}

int PL_Surface_Load(const DXCHAR *filename) {
    SDL_RWops *file;
    SDL_Surface *sdlSurface;
    int surfaceID;
    int hasTransparencyFlag = DXFALSE;
    
    /* Open file stream. */
    file = PLSDL2_FileToRWops(PL_File_OpenRead(filename));
    if (file == NULL) {
        return -1;
    }

    /* Attempt to load surface via SDL2_image */
    sdlSurface = IMG_Load_RW(file, SDL_FALSE);
    if (sdlSurface == NULL) {
        /* Get around an SDL2_image bug */
        if (DXSTRTESTEXT(filename, TEXT(".tga"))) {
            sdlSurface = IMG_LoadTGA_RW(file);
        }
    }
    SDL_RWclose(file);
    
    if (sdlSurface == NULL) {
        return -1;
    }
    
    hasTransparencyFlag = DXFALSE;
    if (sdlSurface->format->BitsPerPixel == 8) {
        /* Determine transparency state. */
        SDL_Palette *palette = sdlSurface->format->palette;
        SDL_Color *colors = palette->colors;
        int ncolors = palette->ncolors;
        int i;
        
        for (i = 0; i < ncolors; ++i) {
            if (colors[i].a != 0xff) {
                hasTransparencyFlag = DXTRUE;
                break;
            }
        }
    } else if (sdlSurface->format->BitsPerPixel == 24) {
        /* 24bpp is not supported so convert to 32 */
        SDL_Surface *newSurface;
        newSurface = SDL_ConvertSurfaceFormat(
            sdlSurface, SDL_PIXELFORMAT_ARGB8888, 0);
        SDL_FreeSurface(sdlSurface);
        sdlSurface = newSurface;
        
        if (newSurface == NULL) {
            return -1;
        }
    } else if (sdlSurface->format->BitsPerPixel == 32) {
        /* Just assume this is a transparent texture. */
        hasTransparencyFlag = DXTRUE;
    }
    
    surfaceID = s_AllocateSurfaceID(sdlSurface, hasTransparencyFlag);
    
    if (surfaceID < 0) {
        SDL_FreeSurface(sdlSurface);
        return -1;
    }
    
    return surfaceID;
}

int PL_Surface_Delete(int surfaceID) {
    /* Fetch the surface entry, if available. */
    Surface *surface = s_GetSurface(surfaceID);
    if (surface == NULL) {
        return -1;
    }
    
    SDL_FreeSurface(surface->sdlSurface);
    
    PL_Handle_ReleaseID(surfaceID, DXTRUE);
    
    s_surfaceCount -= 1;
    
    return 0;
}

int PL_Surface_ToTexture(int surfaceID) {
    Surface *surface = s_GetSurface(surfaceID);
    if (surface == NULL) {
        return -1;
    }
    
    return PL_Texture_CreateFromSDLSurface(
        surface->sdlSurface, surface->hasTransparencyFlag);
}

int PL_Surface_GetCount() {
    return s_surfaceCount;
}

int PL_Surface_GetSize(int surfaceID, int *w, int *h) {
    Surface *surface = s_GetSurface(surfaceID);
    if (surface == NULL) {
        return -1;
    }
    
    *w = surface->sdlSurface->w;
    *h = surface->sdlSurface->h;
    
    return 0;
}

int PL_Surface_InitSurface() {
    int surfaceID;
    
    while ((surfaceID = PL_Handle_GetFirstIDOf(DXHANDLE_SURFACE)) >= 0) {
        PL_Surface_Delete(surfaceID);
    }
    
    return 0;
}

void PL_Surface_End() {
    PL_Surface_InitSurface();
}
