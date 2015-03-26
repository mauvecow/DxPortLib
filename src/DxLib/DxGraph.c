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

#ifdef DXPORTLIB_DXLIB_INTERFACE

#include "PL/PLInternal.h"
#include "DxInternal.h"

/* ---------------------------------------------------------- GRAPH DATA */
/* DerivationGraph is the messiest part of this implementation.
 * 
 * As multiple graphs can reference a single texture,
 * we need to recognize this and store texture information separately.
 */
static unsigned int s_transparentColor = 0x000000;
static int s_useTransparency = DXTRUE;
static int s_graphCount = 0;
static int s_applyPMA = DXFALSE;

typedef struct Graph {
    PLRect rect;
    
    int textureRefID;
    
    int prevLinkedGraphID;
    int nextLinkedGraphID;
} Graph;

static Graph *s_GetGraph(int graphID) {
    return (Graph *)PL_Handle_GetData(graphID, DXHANDLE_GRAPH);
}

static int s_AllocateGraphID(int textureRefID, PLRect rect, int linkToGraphID) {
    int graphID;
    Graph *graph, *linkedGraph;
    
    graphID = PL_Handle_AcquireID(DXHANDLE_GRAPH);
    if (graphID < 0) {
        return -1;
    }
    
    PL_Texture_AddRef(textureRefID);
    
    graph = (Graph *)PL_Handle_AllocateData(graphID, sizeof(Graph));
    graph->textureRefID = textureRefID;
    graph->rect = rect;
    
    if (linkToGraphID < 0 || (linkedGraph = s_GetGraph(linkToGraphID)) == NULL) {
        graph->prevLinkedGraphID = -1;
        graph->nextLinkedGraphID = -1;
    } else {
        Graph *nextGraph = s_GetGraph(linkedGraph->nextLinkedGraphID);
        graph->prevLinkedGraphID = linkToGraphID;
        graph->nextLinkedGraphID = linkedGraph->nextLinkedGraphID;
        linkedGraph->nextLinkedGraphID = graphID;
        if (nextGraph != NULL) {
            nextGraph->prevLinkedGraphID = graphID;
        }
    }
    
    s_graphCount += 1;
    
    return graphID;
}

int Dx_Graph_CreateFromSurface(int surfaceID) {
    int textureRefID;
    int graphID;
    PLRect rect;
    int hasAlphaChannel = PL_Surface_HasTransparency(surfaceID);
    
    if (hasAlphaChannel != DXFALSE && s_applyPMA != DXFALSE) {
        PL_Surface_ApplyPMAToSurface(surfaceID);
    }
    
    textureRefID = PL_Surface_ToTexture(surfaceID);
    if (textureRefID < 0) {
        return -1;
    }
    
    /* Success, fill out the new graph entry. */
    rect.x = 0;
    rect.y = 0;
    PL_Surface_GetSize(surfaceID, &rect.w, &rect.h);
    graphID = s_AllocateGraphID(textureRefID, rect, -1);
    
    if (graphID < 0) {
        PL_Texture_Release(textureRefID);
    }
    
    return graphID;
}

int Dx_Graph_MakeScreen(int width, int height, int hasAlphaChannel) {
    int textureRefID;
    int graphID;
    PLRect rect;
    
    textureRefID = PL_Texture_CreateFramebuffer(width, height, hasAlphaChannel);
    if (textureRefID < 0) {
        return -1;
    }
    
    /* Success, fill out the new graph entry. */
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;
    graphID = s_AllocateGraphID(textureRefID, rect, -1);
    
    if (graphID < 0) {
        PL_Texture_Release(textureRefID);
    }
    
    return graphID;
}

int Dx_Graph_Load(const DXCHAR *filename, int flipFlag) {
    int surfaceID = PL_Surface_Load(filename);
    int graphID;
    
    if (surfaceID < 0) {
        return -1;
    }
    
    PL_Surface_HasTransparency(surfaceID);
    
    if (s_useTransparency) {
        PL_Surface_ApplyTransparentColor(surfaceID, s_transparentColor);
    }
    
    if (flipFlag) {
        PL_Surface_FlipSurface(surfaceID);
    }
    
    graphID = Dx_Graph_CreateFromSurface(surfaceID);
    
    PL_Surface_Delete(surfaceID);
    
    return graphID;
}

int Dx_Graph_FromTexture(int textureRefID, PLRect rect) {
    int graphID = s_AllocateGraphID(textureRefID, rect, -1);
    
    return graphID;
}

int Dx_Graph_Delete(int graphID) {
    /* Fetch the graph entry, if available. */
    Graph *graph = s_GetGraph(graphID);
    Graph *relGraph;
    if (graph == NULL) {
        return -1;
    }
    
    /* Unlink from list. */
    relGraph = s_GetGraph(graph->prevLinkedGraphID);
    if (relGraph != NULL) {
        relGraph->nextLinkedGraphID = graph->nextLinkedGraphID;
    }
    relGraph = s_GetGraph(graph->nextLinkedGraphID);
    if (relGraph != NULL) {
        relGraph->prevLinkedGraphID = graph->prevLinkedGraphID;
    }
    
    PL_Texture_Release(graph->textureRefID);
    
    PL_Handle_ReleaseID(graphID, DXTRUE);
    
    s_graphCount -= 1;
    
    return 0;
}

int Dx_Graph_DeleteSharingGraph(int graphID) {
    Graph *graph = s_GetGraph(graphID);
    int prevID, nextID, tempID;
    if (graph == NULL) {
        return -1;
    }
    
    prevID = graph->prevLinkedGraphID;
    nextID = graph->nextLinkedGraphID;
    
    Dx_Graph_Delete(graphID);
    
    while ((tempID = prevID) >= 0 && (graph = s_GetGraph(tempID)) != NULL) {
        prevID = graph->prevLinkedGraphID;
        Dx_Graph_Delete(tempID);
    }
    while ((tempID = nextID) >= 0 && (graph = s_GetGraph(tempID)) != NULL) {
        nextID = graph->nextLinkedGraphID;
        Dx_Graph_Delete(tempID);
    }
    
    return 0;
}

int Dx_Graph_GetNum() {
    return s_graphCount;
}

int Dx_Graph_GetSize(int graphID, int *w, int *h) {
    Graph *graph = s_GetGraph(graphID);
    if (graph == NULL) {
        return -1;
    }
    
    *w = graph->rect.w;
    *h = graph->rect.h;
    
    return 0;
}

int Dx_Graph_Derivation(int x, int y, int w, int h, int srcGraphID) {
    Graph *srcGraph;
    PLRect rect;
    
    srcGraph = s_GetGraph(srcGraphID);
    if (srcGraph == NULL) {
        return -1;
    }
    
    rect = srcGraph->rect;
    rect.x += x;
    rect.y += y;
    rect.w = w;
    rect.h = h;
    
    return s_AllocateGraphID(srcGraph->textureRefID, rect, srcGraphID);
}

int Dx_Graph_LoadDiv(const DXCHAR *filename, int graphCount,
                     int xCount, int yCount, int xSize, int ySize,
                     int *handleBuf, int textureFlag, int flipFlag) {
    int graphID = Dx_Graph_Load(filename, flipFlag);
    int x, y, n;
    
    if (graphID < 0) {
        return -1;
    }
    
    n = 0;
    for (y = 0; y < yCount; ++y) {
        for (x = 0; x < xCount && n < graphCount; ++x, ++n) {
            handleBuf[n] = Dx_Graph_Derivation(x * xSize, y * ySize,
                                               xSize, ySize, graphID);
        }
    }
    
    Dx_Graph_Delete(graphID);
    
    return 0;
}

int Dx_Graph_SetWrap(int graphID, int wrapState) {
    int textureID = Dx_Graph_GetTextureID(graphID, NULL);
    
    PL_Texture_SetWrap(textureID, wrapState);
    
    return 0;
}

int Dx_Graph_InitGraph() {
    int graphID;
    
    while ((graphID = PL_Handle_GetFirstIDOf(DXHANDLE_GRAPH)) >= 0) {
        Dx_Graph_Delete(graphID);
    }
    
    return 0;
}

int Dx_Graph_SetTransColor(int r, int g, int b) {
    s_transparentColor = (unsigned int)((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16));
    return 0;
}
int Dx_Graph_GetTransColor(int *r, int *g, int *b) {
    *r = (s_transparentColor >> 16) & 0xff;
    *g = (s_transparentColor >> 8) & 0xff;
    *b = (s_transparentColor) & 0xff;
    return 0;
}

int Dx_Graph_SetUseTransColor(int flag) {
    s_useTransparency = (flag == 0) ? DXFALSE : DXTRUE;
    return 0;
}
int Dx_Graph_SetUsePremulAlphaConvertLoad(int flag) {
    s_applyPMA = (flag == 0) ? DXFALSE : DXTRUE;
    return 0;
}

int Dx_Graph_ResetSettings() {
    s_transparentColor = 0x000000;
    s_useTransparency = DXTRUE;
    s_applyPMA = DXFALSE;
    
    return 0;
}

void Dx_Graph_End() {
    Dx_Graph_InitGraph();
    
    /* That should clear all texture refs. If they're not 0, we have a bug. */
}

int Dx_Graph_GetTextureID(int graphID, PLRect *rect) {
    Graph *graph = s_GetGraph(graphID);
    if (graph == NULL) {
        return -1;
    }
    
    if (rect) {
        *rect = graph->rect;
    }
    
    return graph->textureRefID;
}

int Dx_Graph_GetTextureInfo(int graphID, int *dTextureRefID, PLRect *rect, float *xMult, float *yMult) {
    Graph *graph = s_GetGraph(graphID);
    if (graph == NULL) {
        return -1;
    }
   
    if (dTextureRefID != NULL) {
        *dTextureRefID = graph->textureRefID;
    }
    
    if (rect != NULL) {
        *rect = graph->rect;
    }
    
    return PL_Texture_RenderGetTextureInfo(graph->textureRefID, NULL, xMult, yMult);
}

#endif /* #ifdef DXPORTLIB_DXLIB_INTERFACE */
