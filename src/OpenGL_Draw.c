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

#ifdef DXPORTLIB_DRAW_OPENGL

#include "OpenGL_DxInternal.h"

/* Similar to DxLib, we maintain a rolling vertex buffer that is used to
 * store information in sequence as we go.
 */

static int s_blendMode = DX_BLENDMODE_ALPHA;
static Uint32 s_drawColorR = 0xff;
static Uint32 s_drawColorG = 0xff;
static Uint32 s_drawColorB = 0xff;
static Uint32 s_drawColorA = 0xff000000;

enum VertexElementType {
    VERTEX_POSITION,
    VERTEX_TEXCOORD0,
    VERTEX_COLOR
};

typedef struct VertexDefinition {
    int vertexType;
    GLint size;
    GLenum type;
    int offset;
} VertexDefinition;

typedef struct VertexPositionColor {
    float x, y;
    Uint32 color;
} VertexPositionColor;

static const VertexDefinition s_defVertexPositionColor[] = {
    { VERTEX_POSITION, 2, GL_FLOAT, offsetof(VertexPositionColor, x) },
    { VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, offsetof(VertexPositionColor, color) }
};

typedef struct VertexCache {
    const VertexDefinition *defArray;
    int defCount;
    int vertexSize;
    
    int vertexCount;
    
    int textureRefID;
    
    GLenum drawMode;
    
    void *vertexData;
    int vertexDataPosition;
    int vertexDataSize;
} VertexCache;

static VertexCache s_cache;

/* given a call with a vertex definition and the number of vertices,
 * returns the starting vertex pointer.
 */
static void *s_beginCache(
    const VertexDefinition *definitionArray, int defCount,
    int vertexSize, int vertexCount,
    GLenum drawMode, int textureRefID
) {
    if (s_cache.defArray == definitionArray
        && s_cache.drawMode == drawMode
        && s_cache.textureRefID == textureRefID
    ) {
        int n = vertexCount * vertexSize;
        int pos = s_cache.vertexDataPosition;
        int newPos = pos + n;
        if (newPos < s_cache.vertexDataSize) {
            /* got enough room, just return another vertex. */
            s_cache.vertexDataPosition = newPos;
            s_cache.vertexCount += vertexCount;
            
            return s_cache.vertexData + pos;
        }
    }

    /* flush the current cache */
    PL_Draw_FlushCache();
    
    s_cache.defArray = definitionArray;
    s_cache.defCount = defCount;
    s_cache.drawMode = drawMode;
    s_cache.textureRefID = textureRefID;
    s_cache.vertexSize = vertexSize;
    s_cache.vertexCount = vertexCount;
    s_cache.vertexDataPosition = vertexCount * vertexSize;
    
    return s_cache.vertexData;
}

#define START(vertexName, VertexType, drawMode, textureRefID, vertexCount) \
    VertexType *vertexName = (VertexType *)s_beginCache( \
                               s_def ## VertexType, elementsof(s_def ## VertexType), \
                               sizeof(VertexType), vertexCount, \
                               drawMode, textureRefID)

int PL_Draw_ResetSettings() {
    return 0;
}

int PL_Draw_FlushCache() {
    int i;
    int vertexSize;
    void *vertexData;
    const VertexDefinition *def;
    
    if (s_cache.defArray == NULL || s_cache.vertexCount == 0) {
        s_cache.vertexDataPosition = 0;
        return 0;
    }
    
    /* State vertex info */
    vertexSize = s_cache.vertexSize;
    vertexData = s_cache.vertexData;
    def = s_cache.defArray;
    for (i = 0; i < s_cache.defCount; ++i, ++def) {
        switch (def->vertexType) {
            case VERTEX_POSITION:
                PL_GL.glEnableClientState(GL_VERTEX_ARRAY);
                PL_GL.glVertexPointer(def->size, def->type, vertexSize, vertexData + def->offset);
                break; 
            case VERTEX_TEXCOORD0:
                PL_GL.glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                PL_GL.glTexCoordPointer(def->size, def->type, vertexSize, vertexData + def->offset);
                break;
            case VERTEX_COLOR:
                PL_GL.glEnableClientState(GL_COLOR_ARRAY);
                PL_GL.glColorPointer(def->size, def->type, vertexSize, vertexData + def->offset);
                break;
        }
    }
    
    
    if (PL_GL.glActiveTexture != 0) {
        PL_GL.glActiveTexture(GL_TEXTURE0);
    }
    PL_Texture_Bind(s_cache.textureRefID);
    
    /* Draw! */
    /* This could be optimized a bit by storing a list of drawMode changes,
     * and just reusing vertices.
     */
    PL_GL.glDrawArrays(s_cache.drawMode, 0, s_cache.vertexCount);
    
    /* Clean up */
    PL_Texture_Unbind(s_cache.textureRefID);
    
    def = s_cache.defArray;
    for (i = 0; i < s_cache.defCount; ++i, ++def) {
        switch (def->vertexType) {
            case VERTEX_POSITION:
                PL_GL.glDisableClientState(GL_VERTEX_ARRAY);
                break; 
            case VERTEX_TEXCOORD0:
                PL_GL.glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                break; 
            case VERTEX_COLOR:
                PL_GL.glDisableClientState(GL_COLOR_ARRAY);
                break;
        }
    }
    
    s_cache.vertexCount = 0;
    s_cache.vertexDataPosition = 0;
    
    return 0;
}

int PL_Draw_InitCache() {
    SDL_memset(&s_cache, 0, sizeof(s_cache));
    
    s_cache.defArray = NULL;
    s_cache.defCount = 0;
    s_cache.vertexSize = 1;
    s_cache.vertexCount = 0;
    
    /* 256kb should be enough for anybody */
    s_cache.vertexDataPosition = 0;
    s_cache.vertexDataSize = 256 * 1024;
    s_cache.vertexData = SDL_malloc(s_cache.vertexDataSize);
    
    return 0;
}

int PL_Draw_DestroyCache() {
    if (s_cache.vertexData != NULL) {
        SDL_free(s_cache.vertexData);
    }
    
    SDL_memset(&s_cache, 0, sizeof(s_cache));
    
    return 0;
}

static inline Uint32 s_modulateColor(DXCOLOR color) {
    return s_drawColorA
           | (((color & 0xff) * s_drawColorR) / 0xff)
           | (((color & 0xff00) * s_drawColorG) / 0xff)
           | (((color & 0xff0000) * s_drawColorB) / 0xff);
}

int PL_Draw_LineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    Uint32 vColor = s_modulateColor(color);
    START(v, VertexPositionColor, GL_LINES, -1, 2);
    
    v[0].x = x1 - 0.5f; v[0].y = y1 - 0.5f; v[0].color = vColor;
    v[1].x = x2 - 0.5f; v[1].y = y2 - 0.5f; v[1].color = vColor;
    
    return 0;
}

int PL_Draw_Line(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness) {
    return PL_Draw_LineF((float)x1, (float)y1, (float)x2, (float)y2, color, thickness);
}

int PL_Draw_OvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag) {
    /* FIXME WRITEME */
    return 0;
}
int PL_Draw_Oval(int x, int y, int rx, int ry, DXCOLOR color, int fillFlag) {
    return PL_Draw_OvalF((float)x, (float)y, (float)rx, (float)ry, color, fillFlag);
}
int PL_Draw_Circle(int x, int y, int r, DXCOLOR color, int fillFlag) {
    return PL_Draw_OvalF((float)x, (float)y, (float)r, (float)r, color, fillFlag);
}
int PL_Draw_CircleF(float x, float y, float r, DXCOLOR color, int fillFlag) {
    return PL_Draw_OvalF(x, y, r, r, color, fillFlag);
}

int PL_Draw_BoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag) {
    Uint32 vColor = s_modulateColor(color);
    x1 -= 0.5f; x2 -= 0.5f;
    y1 -= 0.5f; y2 -= 0.5f;
    
    if (FillFlag) {
        /* TRIANGLES instead of TRIANGLE_STRIP so that we can batch. */
        START(v, VertexPositionColor, GL_TRIANGLES, -1, 6);
        
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y1; v[1].color = vColor;
        v[2].x = x1; v[2].y = y2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x2; v[5].y = y2; v[5].color = vColor;
    } else {
        /* LINES instead of LINE_LOOP so that we can batch. */
        START(v, VertexPositionColor, GL_LINES, -1, 8);
        
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y1; v[1].color = vColor;
        v[2] = v[1];
        v[3].x = x2; v[3].y = y2; v[3].color = vColor;
        v[4] = v[3];
        v[5].x = x1; v[5].y = y2; v[5].color = vColor;
        v[6] = v[5];
        v[7] = v[0];
    }
    
    return 0;
}

int PL_Draw_Box(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag) {
    return PL_Draw_BoxF((float)x1, (float)y1, (float)x2, (float)y2, color, FillFlag);
}

int PL_Draw_GraphF(float x, float y, int graphID, int blendFlag) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_Graph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_GraphF((float)x, (float)y, graphID, blendFlag);
}

int PL_Draw_ExtendGraphF(float x1, float y1, float x2, float y2, int graphID, int blendFlag) {
    /* FIXME WRITEME */
    return 0;
}
int PL_Draw_ExtendGraph(int x1, int y1, int x2, int y2, int graphID, int blendFlag) {
    return PL_Draw_ExtendGraphF((float)x1, (float)y1, (float)x2, (float)y2, graphID, blendFlag);
}

int PL_Draw_RectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                      int graphID, int blendFlag, int turnFlag) {
    /* FIXME WRITEME */
    return 0;
}
int PL_Draw_RectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                      int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectGraphF((float)dx, (float)dy, sx, sy, sw, sh, graphID, blendFlag, turnFlag);
}

/* Temporary DxPortLib extension because RectGraph is sloooooow) */
int PL_EXT_Draw_RectGraphFastF(
                            float dx, float dy, float dw, float dh,
                            int sx, int sy, int sw, int sh,
                            int graphID, int blendFlag) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_RectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                             int sx, int sy, int sw, int sh,
                             int graphID, int blendFlag, int turnFlag) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_RectExtendGraph(int dx1, int dy1, int dx2, int dy2,
                            int sx, int sy, int sw, int sh,
                            int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectExtendGraphF(
                (float)dx1, (float)dy1, (float)dx2, (float)dy2,
                sx, sy, sw, sh,
                graphID, blendFlag, turnFlag
            );
}

static int s_Draw_RotaGraphMain(
                       SDL_Texture *texture,
                       const SDL_Rect *texRect,
                       float x, float y, 
                       float cx, float cy,
                       double xScaleFactor,
                       double yScaleFactor,
                       double angle,
                       int graphID, int blendFlag, int turn)
{
    /* FIXME WRITEME */
    return 0;
}


int PL_Draw_RotaGraphF(float x, float y, 
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    /* FIXME WRITEME */
    return 0;
}
int PL_Draw_RotaGraph(int x, int y, 
                      double scaleFactor, double angle,
                      int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraphF((float)x, (float)y, scaleFactor, angle,
                              graphID, blendFlag, turn);
}

int PL_Draw_RotaGraph2F(float x, float y, float cx, float cy,
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_RotaGraph2(int x, int y, int cx, int cy,
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph2F(
                (float)x, (float)y, (float)cx, (float)cy,
                scaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RotaGraph3F(float x, float y, float cx, float cy,
                       double xScaleFactor, double yScaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_RotaGraph3(int x, int y, int cx, int cy,
                       double xScaleFactor, double yScaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph3F(
                (float)x, (float)y, (float)cx, (float)cy,
                xScaleFactor, yScaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RectRotaGraphF(float x, float y,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    /* FIXME WRITEME */
    return 0;
}
int PL_Draw_RectRotaGraph(int x, int y, 
                          int sx, int sy, int sw, int sh,
                          double scaleFactor, double angle,
                          int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraphF((float)x, (float)y, sx, sy, sw, sh,
                                  scaleFactor, angle,
                                  graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph2F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_RectRotaGraph2(int x, int y, int cx, int cy,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph2F(
                (float)x, (float)y, sx, sy, sw, sh, (float)cx, (float)cy,
                scaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RectRotaGraph3F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double xScaleFactor, double yScaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_RectRotaGraph3(int x, int y,
                           int sx, int sy, int sw, int sh,
                           int cx, int cy,
                           double xScaleFactor, double yScaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph3F(
                (float)x, (float)y, sx, sy, sw, sh, (float)cx, (float)cy,
                xScaleFactor, yScaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_TurnGraphF(float x, float y, int graphID, int blendFlag) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_TurnGraph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_TurnGraphF((float)x, (float)y, graphID, blendFlag);
}

int PL_Draw_SetDrawArea(int x1, int y1, int x2, int y2) {
    PL_Draw_FlushCache();
    
    if (x1 == 0 && y1 == 0 && x2 == PL_drawScreenWidth && y2 == PL_drawScreenHeight) {
        PL_GL.glDisable(GL_SCISSOR_TEST);
    } else {
        PL_GL.glEnable(GL_SCISSOR_TEST);
        PL_GL.glScissor(x1, y1, x2 - x1, y2 - y1);
    }
    
    return 0;
}

int PL_Draw_SetDrawBlendMode(int blendMode, int alpha) {
    /* Changing blend mode forces a cache flush. */
    if (blendMode != s_blendMode) {
        PL_Draw_FlushCache();
        
        s_blendMode = blendMode;
        
        switch(blendMode) {
        case DX_BLENDMODE_ALPHA:
            PL_GL.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            PL_GL.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            PL_GL.glEnable(GL_BLEND);
            break;
        case DX_BLENDMODE_ADD:
            PL_GL.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            PL_GL.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
            PL_GL.glEnable(GL_BLEND);
            break;
        default: /* NOBLEND */
            PL_GL.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            PL_GL.glDisable(GL_BLEND);
            blendMode = DX_BLENDMODE_NOBLEND;
            break;
        }
    }
    
    /* Changing draw color does not cause a cache flush. */
    s_drawColorA = (Uint32)alpha << 24;
    
    return 0;
}

int PL_Draw_SetBright(int redBright, int greenBright, int blueBright) {
    s_drawColorR = redBright & 0xff;
    s_drawColorG = greenBright & 0xff;
    s_drawColorB = blueBright & 0xff;
    return 0;
}

int PL_Draw_GetBright(int *redBright, int *greenBright, int *blueBright) {
    *redBright = s_drawColorR;
    *greenBright = s_drawColorG;
    *blueBright = s_drawColorB;
    
    return 0;
}

int PL_Draw_SetBasicBlendFlag(int blendFlag) {
    /* Reseved for software renderer, so it won't be used at all. */
    return 0;
}

DXCOLOR PL_Draw_GetColor(int red, int green, int blue) {
    return red | (green << 8) | (blue << 16);
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
