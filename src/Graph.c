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

#include "SDL_image.h"

/* ---------------------------------------------------------- GRAPH DATA */
/* DerivationGraph is the messiest part of this implementation.
 * 
 * As multiple graphs can reference a single texture,
 * we need to recognize this and store texture information separately.
 */
static unsigned int s_transparentColor = 0x000000;
static int s_useTransparency = DXTRUE;

typedef struct Graph {
    SDL_Rect rect;
    
    int textureRefID;
} Graph;

static int s_AllocateGraphID(int textureRefID, SDL_Rect rect) {
    int graphID;
    Graph *graph;
    
    graphID = PL_Handle_AcquireID(DXHANDLE_GRAPH);
    if (graphID < 0) {
        return -1;
    }
    
    PL_Texture_AddRef(textureRefID);
    
    graph = (Graph *)PL_Handle_AllocateData(graphID, sizeof(Graph));
    graph->textureRefID = textureRefID;
    graph->rect = rect;
    
    return graphID;
}

static Graph *s_GetGraph(int graphID) {
    return (Graph *)PL_Handle_GetData(graphID, DXHANDLE_GRAPH);
}

static void s_ApplyTransparentColor(SDL_Surface *surface) {
    SDL_PixelFormat *format;
    
    if (s_useTransparency == DXFALSE) {
        return;
    }
    
    format = surface->format;
    
    if (format->format == SDL_PIXELFORMAT_INDEX8) {
        SDL_Palette *palette = format->palette;
        SDL_Color *colors = palette->colors;
        SDL_Color transColor;
        int ncolors = palette->ncolors;
        int i;
        
        transColor.r = (s_transparentColor >> 16) & 0xff;
        transColor.g = (s_transparentColor >> 8) & 0xff;
        transColor.b = (s_transparentColor) & 0xff;
        transColor.a = 255;
        
        for (i = 0; i < ncolors; ++i) {
            SDL_Color c = colors[i];
            
            if (c.r == transColor.r
                && c.g == transColor.g
                && c.b == transColor.b
            ) {
                c.a = 0;
                colors[i] = c;
            }
            
        }
    } else if (surface->format->BitsPerPixel == 32) {
        unsigned int *pixels = surface->pixels;
        unsigned int transColor;
        int width = surface->w;
        int height = surface->h;
        int pitch = surface->pitch / 4;
        int x, y;

        transColor = (s_transparentColor >> 16) << format->Rshift
                     | (s_transparentColor >> 8) << format->Gshift
                     | (s_transparentColor) << format->Bshift
                     | (unsigned int)(0xff) << format->Ashift;
        
        for (y = 0; y < height; ++y) {
            for (x = 0; x < width; ++x) {
                if (pixels[x] == transColor) {
                    pixels[x] = 0;
                }
            }
            pixels += pitch;
        }
    }
}

int PL_Graph_CreateFromSurface(SDL_Surface *surface) {
    int textureRefID;
    int graphID;
    SDL_Rect rect;
    
    textureRefID = PL_Texture_CreateFromSurface(surface);
    if (textureRefID < 0) {
        return -1;
    }
    
    /* Success, fill out the new graph entry. */
    rect.x = 0;
    rect.y = 0;
    rect.w = surface->w;
    rect.h = surface->h;
    graphID = s_AllocateGraphID(textureRefID, rect);
    
    if (graphID < 0) {
        PL_Texture_Release(textureRefID);
    }
    
    return graphID;
}

int PL_Graph_Load(const DXCHAR *filename) {
    SDL_RWops *file;
    SDL_Surface *surface;
    int graphID;
    
    /* Open file stream. */
    file = PL_File_OpenStream(filename);
    if (file == NULL) {
        return -1;
    }

    /* Attempt to load surface via SDL2_image */
    surface = IMG_Load_RW(file, SDL_TRUE);
    if (surface == NULL) {
        return -1;
    }
    
    /* Convert to 32bpp from 24bpp. */
    if (surface->format->BitsPerPixel == 24) {
        SDL_Surface *newSurface;
        newSurface = SDL_ConvertSurfaceFormat(
            surface, SDL_PIXELFORMAT_ARGB8888, 0);
        SDL_FreeSurface(surface);
        
        if (newSurface == NULL) {
            return -1;
        }
        
        surface = newSurface;
        s_ApplyTransparentColor(surface);
    } else if (surface->format->BitsPerPixel == 8) {
        s_ApplyTransparentColor(surface);
    }
    
    graphID = PL_Graph_CreateFromSurface(surface);
    
    SDL_FreeSurface(surface);
    
    return graphID;
}

int PL_Graph_FromTexture(int textureRefID, SDL_Rect rect) {
    int graphID = s_AllocateGraphID(textureRefID, rect);
    
    return graphID;
}

int PL_Graph_Delete(int graphID) {
    /* Fetch the graph entry, if avialable. */
    Graph *graph = s_GetGraph(graphID);
    if (graph == NULL) {
        return -1;
    }
    
    PL_Texture_Release(graph->textureRefID);
    
    PL_Handle_ReleaseID(graphID, DXTRUE);
    
    return 0;
}

int PL_Graph_GetSize(int graphID, int *w, int *h) {
    Graph *graph = s_GetGraph(graphID);
    if (graph == NULL) {
        return -1;
    }
    
    *w = graph->rect.w;
    *h = graph->rect.h;
    
    return 0;
}

int PL_Graph_Derivation(int x, int y, int w, int h, int srcGraphID) {
    Graph *srcGraph;
    SDL_Rect rect;
    
    srcGraph = s_GetGraph(srcGraphID);
    if (srcGraph == NULL) {
        return -1;
    }
    
    rect = srcGraph->rect;
    rect.x += x;
    rect.y += y;
    rect.w = w;
    rect.h = h;
    
    return s_AllocateGraphID(srcGraph->textureRefID, rect);
}

int PL_Graph_InitGraph() {
    int graphID;
    
    while ((graphID = PL_Handle_GetFirstIDOf(DXHANDLE_GRAPH)) >= 0) {
        PL_Graph_Delete(graphID);
    }
    
    return 0;
}

int PL_Graph_SetTransColor(int r, int g, int b) {
    s_transparentColor = (unsigned int)((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16));
    return 0;
}
int PL_Graph_GetTransColor(int *r, int *g, int *b) {
    *r = (s_transparentColor >> 16) & 0xff;
    *g = (s_transparentColor >> 8) & 0xff;
    *b = (s_transparentColor) & 0xff;
    return 0;
}

int PL_Graph_SetUseTransColor(int flag) {
    s_useTransparency = (flag == 0) ? DXFALSE : DXTRUE;
    return 0;
}

int PL_Graph_ResetSettings() {
    s_transparentColor = 0x000000;
    s_useTransparency = DXTRUE;
    
    return 0;
}

void PL_Graph_End() {
    PL_Graph_InitGraph();
    
    /* That should clear all texture refs. If they're not 0, we have a bug. */
}

int PL_Graph_GetTextureID(int graphID, SDL_Rect *rect) {
    Graph *graph = s_GetGraph(graphID);
    if (graph == NULL) {
        return -1;
    }
    
    if (rect) {
        *rect = graph->rect;
    }
    
    return graph->textureRefID;
}
