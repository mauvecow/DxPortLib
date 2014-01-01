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

/* Rather than unnecessarily duplicating code, we include vertex
 * definitions for the various kinds we'll use.
 * 
 * This is not going to be much for now, but if Graph Filters or other
 * shaders ever make their way in here we'll regret not having them.
 */

typedef struct VertexDefinition {
    int vertexType;
    GLint size;
    GLenum type;
    int offset;
} VertexDefinition;

typedef struct VertexPosition2Color {
    float x, y;
    Uint32 color;
} VertexPosition2Color;

static const VertexDefinition s_defVertexPosition2Color[] = {
    { VERTEX_POSITION, 2, GL_FLOAT, offsetof(VertexPosition2Color, x) },
    { VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, offsetof(VertexPosition2Color, color) }
};

typedef struct VertexPosition2Tex2Color {
    float x, y;
    float tcx, tcy;
    Uint32 color;
} VertexPosition2Tex2Color;

static const VertexDefinition s_defVertexPosition2Tex2Color[] = {
    { VERTEX_POSITION, 2, GL_FLOAT, offsetof(VertexPosition2Tex2Color, x) },
    { VERTEX_TEXCOORD0, 2, GL_FLOAT, offsetof(VertexPosition2Tex2Color, tcx) },
    { VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, offsetof(VertexPosition2Tex2Color, color) }
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

/* Given a call with a vertex definition and the number of vertices,
 * returns the starting vertex pointer.
 */
static void *s_beginCache(
    const VertexDefinition *definitionArray, int defCount,
    int vertexSize, int vertexCount,
    GLenum drawMode, int textureRefID
) {
    /* - If this is the same as the last definition, try to continue it. */
    if (s_cache.defArray == definitionArray
        && s_cache.drawMode == drawMode
        && s_cache.textureRefID == textureRefID
    ) {
        int n = vertexCount * vertexSize;
        int pos = s_cache.vertexDataPosition;
        int newPos = pos + n;
        if (newPos <= s_cache.vertexDataSize) {
            s_cache.vertexDataPosition = newPos;
            s_cache.vertexCount += vertexCount;
            
            return s_cache.vertexData + pos;
        }
    }

    /* - Flush the current cache */
    PL_Draw_FlushCache();
    
    /* - Set up the new definition. */
    s_cache.defArray = definitionArray;
    s_cache.defCount = defCount;
    s_cache.drawMode = drawMode;
    s_cache.textureRefID = textureRefID;
    s_cache.vertexSize = vertexSize;
    s_cache.vertexCount = vertexCount;
    s_cache.vertexDataPosition = vertexCount * vertexSize;
    
    return s_cache.vertexData;
}

/* Helpful start macro.
 * 
 * Can be called multiple times in the same function to fetch more
 * vertices as needed.
 */
#define START(vertexName, VertexType, drawMode, textureRefID, vertexCount) \
    VertexType *vertexName = (VertexType *)s_beginCache( \
                               s_def ## VertexType, elementsof(s_def ## VertexType), \
                               sizeof(VertexType), vertexCount, \
                               drawMode, textureRefID);

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
    
    /* 64kb should be enough for anybody */
    s_cache.vertexDataPosition = 0;
    s_cache.vertexDataSize = 64 * 1024;
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

static inline Uint32 s_getColor() {
    return (s_drawColorR) | (s_drawColorG << 8) | (s_drawColorB << 16) | s_drawColorA;
}
static inline Uint32 s_modulateColor(DXCOLOR color) {
    return s_drawColorA
           | (((color & 0xff) * s_drawColorR) / 0xff)
           | (((color & 0xff00) * s_drawColorG) / 0xff)
           | (((color & 0xff0000) * s_drawColorB) / 0xff);
}

int PL_Draw_PixelF(float x, float y, DXCOLOR color) {
    Uint32 vColor = s_modulateColor(color);
    START(v, VertexPosition2Color, GL_POINTS, -1, 1);
    v[0].x = x - 0.5f; v[0].y = y - 0.5f; v[0].color = vColor;
    
    return 0;
}

int PL_Draw_Pixel(int x, int y, DXCOLOR color) {
    return PL_Draw_PixelF((float)x, (float)y, color);
}

int PL_Draw_LineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    Uint32 vColor = s_modulateColor(color);
    START(v, VertexPosition2Color, GL_LINES, -1, 2);
    
    v[0].x = x1 - 0.5f; v[0].y = y1 - 0.5f; v[0].color = vColor;
    v[1].x = x2 - 0.5f; v[1].y = y2 - 0.5f; v[1].color = vColor;
    
    return 0;
}

int PL_Draw_Line(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness) {
    return PL_Draw_LineF((float)x1, (float)y1, (float)x2, (float)y2, color, thickness);
}

int PL_Draw_OvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag) {
    /* DxLib's circle/oval drawing functions are not mimiced accurately.
     * 
     * DxLib draws them one pixel around the circumference at a time.
     * And filled ellipses are lines extending across the diameter.
     * 
     * Yes, really.
     * 
     * We may want to adjust the number of points based on the
     * circumference of the ellipse.
     */
    Uint32 vColor = s_modulateColor(color);
    x -= 0.5f; y -= 0.5f;
    
    if (fillFlag) {
        int points = 36;
        float amount = ((float)M_PI * 2) / points;
        START(v, VertexPosition2Color, GL_TRIANGLE_FAN, -1, points);
        int i;
        
        for (i = 0; i < points; ++i) {
            v[i].x = x + (SDL_cosf(i * amount) * rx);
            v[i].y = y + (SDL_sinf(i * amount) * ry);
            v[i].color = vColor;
        }
        
        /* we don't want triangle fan to bleed over, so clear it out now. */
        PL_Draw_FlushCache();
    } else {
        int points = 36;
        float amount = ((float)M_PI * 2) / points;
        START(v, VertexPosition2Color, GL_LINES, -1, points * 2);
        VertexPosition2Color *sv = v;
        int i;
        
        v[0].x = x + rx;
        v[0].y = y;
        v[0].color = vColor;
        v += 1;
        
        for (i = 1; i < points; ++i) {
            v->x = x + (SDL_cosf(i * amount) * rx);
            v->y = y + (SDL_sinf(i * amount) * ry);
            v->color = vColor;
            
            v[1] = v[0];
            v += 2;
        }
        
        v[0] = sv[0];
    }
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

int PL_Draw_TriangleF(
    float x1, float y1, float x2, float y2, float x3, float y3,
    DXCOLOR color, int fillFlag
) {
    Uint32 vColor = s_modulateColor(color);
    x1 -= 0.5f; y1 -= 0.5f;
    x2 -= 0.5f; y2 -= 0.5f;
    x3 -= 0.5f; y3 -= 0.5f;
    
    if (fillFlag) {
        START(v, VertexPosition2Color, GL_TRIANGLES, -1, 3);
    
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y2; v[1].color = vColor;
        v[2].x = x3; v[2].y = y3; v[2].color = vColor;
    } else {
        START(v, VertexPosition2Color, GL_LINES, -1, 6);
    
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[5] = v[0];
        v[2].x = x2; v[2].y = y2; v[2].color = vColor;
        v[1] = v[2];
        v[4].x = x3; v[4].y = y3; v[4].color = vColor;
        v[3] = v[4];
    }
    
    return 0;
}

int PL_Draw_Triangle(
    int x1, int y1, int x2, int y2, int x3, int y3,
    DXCOLOR color, int fillFlag
) {
    return PL_Draw_TriangleF((float)x1, (float)y1, (float)x2, (float)y2,
                             (float)x3, (float)y3, color, fillFlag);
}

int PL_Draw_QuadrangleF(
    float x1, float y1, float x2, float y2,
    float x3, float y3, float x4, float y4,
    DXCOLOR color, int fillFlag
) {
    Uint32 vColor = s_modulateColor(color);
    x1 -= 0.5f; y1 -= 0.5f;
    x2 -= 0.5f; y2 -= 0.5f;
    x3 -= 0.5f; y3 -= 0.5f;
    x3 -= 0.5f; y4 -= 0.5f;
    
    if (fillFlag) {
        START(v, VertexPosition2Color, GL_TRIANGLES, -1, 6);
    
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y2; v[1].color = vColor;
        v[2].x = x3; v[2].y = y3; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x4; v[5].y = y4; v[5].color = vColor;
    } else {
        START(v, VertexPosition2Color, GL_LINES, -1, 8);
    
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[7] = v[0];
        v[2].x = x2; v[2].y = y2; v[2].color = vColor;
        v[1] = v[2];
        v[4].x = x3; v[4].y = y3; v[4].color = vColor;
        v[3] = v[4];
        v[6].x = x4; v[6].y = y4; v[6].color = vColor;
        v[5] = v[6];
    }
    
    return 0;
}

int PL_Draw_Quadrangle(
    int x1, int y1, int x2, int y2,
    int x3, int y3, int x4, int y4,
    DXCOLOR color, int fillFlag
) {
    return PL_Draw_QuadrangleF((float)x1, (float)y1, (float)x2, (float)y2,
                               (float)x3, (float)y3, (float)x4, (float)y4,
                               color, fillFlag);
}

int PL_Draw_BoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag) {
    Uint32 vColor = s_modulateColor(color);
    x1 -= 0.5f; x2 -= 0.5f;
    y1 -= 0.5f; y2 -= 0.5f;
    
    if (FillFlag) {
        /* TRIANGLES instead of TRIANGLE_STRIP so that we can batch. */
        START(v, VertexPosition2Color, GL_TRIANGLES, -1, 6);
        
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y1; v[1].color = vColor;
        v[2].x = x1; v[2].y = y2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x2; v[5].y = y2; v[5].color = vColor;
    } else {
        /* LINES instead of LINE_LOOP so that we can batch. */
        START(v, VertexPosition2Color, GL_LINES, -1, 8);
        
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

int PL_Draw_GraphF(float x1, float y1, int graphID, int blendFlag) {
    SDL_Rect texRect;
    int textureRefID;
    float xMult, yMult;
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) >= 0) {
        Uint32 vColor = s_getColor();
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
        float x2, y2;
        float tx1 = texRect.x * xMult;
        float ty1 = texRect.y * yMult;
        float tx2 = tx1 + (texRect.w * xMult);
        float ty2 = ty1 + (texRect.h * yMult);
        
        x2 = x1 + texRect.w; y2 = y1 + texRect.h;
        
        v[0].x = x1; v[0].y = y1; v[0].tcx = tx1; v[0].tcy = ty1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y1; v[1].tcx = tx2; v[1].tcy = ty1; v[1].color = vColor;
        v[2].x = x1; v[2].y = y2; v[2].tcx = tx1; v[2].tcy = ty2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x2; v[5].y = y2; v[5].tcx = tx2; v[5].tcy = ty2; v[5].color = vColor;
    }
    
    return 0;
}

int PL_Draw_Graph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_GraphF((float)x, (float)y, graphID, blendFlag);
}

int PL_Draw_ExtendGraphF(float x1, float y1, float x2, float y2, int graphID, int blendFlag) {
    SDL_Rect texRect;
    int textureRefID;
    float xMult, yMult;
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) >= 0) {
        Uint32 vColor = s_getColor();
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
        float tx1 = texRect.x * xMult;
        float ty1 = texRect.y * yMult;
        float tx2 = tx1 + (texRect.w * xMult);
        float ty2 = ty1 + (texRect.h * yMult);
        
        v[0].x = x1; v[0].y = y1; v[0].tcx = tx1; v[0].tcy = ty1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y1; v[1].tcx = tx2; v[1].tcy = ty1; v[1].color = vColor;
        v[2].x = x1; v[2].y = y2; v[2].tcx = tx1; v[2].tcy = ty2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x2; v[5].y = y2; v[5].tcx = tx2; v[5].tcy = ty2; v[5].color = vColor;
    }
    
    return 0;
}
int PL_Draw_ExtendGraph(int x1, int y1, int x2, int y2, int graphID, int blendFlag) {
    return PL_Draw_ExtendGraphF((float)x1, (float)y1, (float)x2, (float)y2, graphID, blendFlag);
}

int PL_Draw_RectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                      int graphID, int blendFlag, int turnFlag) {
    /* DxLib has very, very strange behavior for DrawRectGraph.
     * 
     * Unlike basically all other drawing functions, it does not
     * simply create a subsection of the graph to draw. Instead,
     * it creates a clip window on screen and then draws the entire
     * graph over it.
     * 
     * When it flips horizontally, it flips the whole graph, too,
     * which is where real trouble begins.
     * 
     * This is actually important, because it means you can set the
     * source coordinates to be partially outside of the texture and
     * it will still be valid!
     */
    SDL_Rect texRect;
    int textureRefID;
    float xMult, yMult;
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) >= 0) {
        Uint32 vColor = s_getColor();
        float dx1, dy1, dx2, dy2;
        float tx1, ty1, tx2, ty2;
        
        /* - only do the fancy clipping calculations if we need to */
        if ((sx + sw) > texRect.w || (sy + sh) > texRect.h) {
            /* - calculate non-flipped clip window */
            /* we make sure to clip both the window and the texture's bounds */
            float dx2a, dy2a;
            float tx, ty;
            tx = dx - (float)sx;
            ty = dy - (float)sy;
            
            dx1 = dx;
            dy1 = dy;
            
            dx2 = tx + (float)texRect.w;
            dx2a = dx + (float)sw;
            if (dx2a < dx2) { dx2 = dx2a; }
            
            dy2 = ty + (float)texRect.h;
            dy2a = dy + (float)sh;
            if (dy2a < dy2) { dy2 = dy2a; }
            
            if (dx2 <= dx1 || dy2 <= dy1) {
                return 0;
            }
    
            /* - calculate texture coordinates */
            tx -= (float)texRect.x;
            ty -= (float)texRect.y;
            
            tx1 = (dx1 - tx) * xMult;
            ty1 = (dy1 - ty) * yMult;
            tx2 = (dx2 - tx) * xMult;
            ty2 = (dy2 - ty) * yMult;
            
            /* - if flipping, offset x coordinates from the other side */
            if (turnFlag) {
                dx1 = dx + (float)sw;
                dx2 = dx1 + dx - dx2;
            }
        } else {
            /* - simple version, no clip needed */
            dx1 = dx;
            dy1 = dy;
            dx2 = dx + (float)sw;
            dy2 = dy + (float)sh;
            
            tx1 = (texRect.x + (float)sx) * xMult;
            ty1 = (texRect.y + (float)sy) * yMult;
            tx2 = tx1 + ((float)sw * xMult);
            ty2 = ty1 + ((float)sh * yMult);
            
            if (turnFlag) { float tmp = dx1; dx1 = dx2; dx2 = tmp; }
        }
        
        /* - draw! */
        {
            START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
            
            v[0].x = dx1; v[0].y = dy1; v[0].tcx = tx1; v[0].tcy = ty1; v[0].color = vColor;
            v[1].x = dx2; v[1].y = dy1; v[1].tcx = tx2; v[1].tcy = ty1; v[1].color = vColor;
            v[2].x = dx1; v[2].y = dy2; v[2].tcx = tx1; v[2].tcy = ty2; v[2].color = vColor;
            v[3] = v[2];
            v[4] = v[1];
            v[5].x = dx2; v[5].y = dy2; v[5].tcx = tx2; v[5].tcy = ty2; v[5].color = vColor;
        }
    }
    
    return 0;
}
int PL_Draw_RectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                      int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectGraphF((float)dx, (float)dy, sx, sy, sw, sh, graphID, blendFlag, turnFlag);
}

/* Temporary DxPortLib extension because RectGraph is sloooooow) */
int PL_EXT_Draw_RectGraphFastF(
                            float dx1, float dy1, float dw, float dh,
                            int sx, int sy, int sw, int sh,
                            int graphID, int blendFlag) {
    SDL_Rect texRect;
    int textureRefID;
    float xMult, yMult;
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) >= 0) {
        Uint32 vColor = s_getColor();
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
        float dx2, dy2;
        float tx1 = (texRect.x + (float)sx) * xMult;
        float ty1 = (texRect.y + (float)sy) * yMult;
        float tx2 = tx1 + ((float)sw * xMult);
        float ty2 = ty1 + ((float)sh * yMult);
        
        dx2 = dx1 + sw; dy2 = dy1 + sh;
        
        v[0].x = dx1; v[0].y = dy1; v[0].tcx = tx1; v[0].tcy = ty1; v[0].color = vColor;
        v[1].x = dx2; v[1].y = dy1; v[1].tcx = tx2; v[1].tcy = ty1; v[1].color = vColor;
        v[2].x = dx1; v[2].y = dy2; v[2].tcx = tx1; v[2].tcy = ty2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = dx2; v[5].y = dy2; v[5].tcx = tx2; v[5].tcy = ty2; v[5].color = vColor;
    }
    
    return 0;
}

int PL_Draw_RectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                             int sx, int sy, int sw, int sh,
                             int graphID, int blendFlag, int turnFlag) {
    SDL_Rect texRect;
    int textureRefID;
    float xMult, yMult;
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) >= 0) {
        Uint32 vColor = s_getColor();
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
        float tx1 = (texRect.x + sx) * xMult;
        float ty1 = (texRect.y + sy) * yMult;
        float tx2 = tx1 + (sw * xMult);
        float ty2 = ty1 + (sh * yMult);
        
        v[0].x = dx1; v[0].y = dy1; v[0].tcx = tx1; v[0].tcy = ty1; v[0].color = vColor;
        v[1].x = dx2; v[1].y = dy1; v[1].tcx = tx2; v[1].tcy = ty1; v[1].color = vColor;
        v[2].x = dx1; v[2].y = dy2; v[2].tcx = tx1; v[2].tcy = ty2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = dx2; v[5].y = dy2; v[5].tcx = tx2; v[5].tcy = ty2; v[5].color = vColor;
    }
    
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
                       int textureRefID,
                       const SDL_Rect *texRect,
                       float xMult, float yMult,
                       float x, float y, 
                       float cx, float cy,
                       float xScaleFactor,
                       float yScaleFactor,
                       float angle,
                       int graphID, int blendFlag, int turn)
{
    /* I looked at DxLib's source and there was this gigantic
     * mess of an algorithm. So here's a much simpler one:
     *
     * - Offset to center of quad.
     * - Calculate the two x extents and the two y extents.
     * - Draw!
     */
    Uint32 vColor = s_getColor();
    START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
    float tx1 = texRect->x * xMult;
    float ty1 = texRect->y * yMult;
    float tx2 = tx1 + (texRect->w * xMult);
    float ty2 = ty1 + (texRect->h * yMult);
    float fSin = (float)SDL_sin(angle);
    float fCos = (float)SDL_cos(angle);
    float dx, dy;
    float halfW, halfH;
    float halfWcos, halfHcos, halfWsin, halfHsin;
    float xext1, xext2;
    float yext1, yext2;
    
    halfW = (float)(texRect->w * xScaleFactor) * 0.5f;
    halfH = (float)(texRect->h * yScaleFactor) * 0.5f;
    cx *= xScaleFactor;
    cy *= yScaleFactor;
    
    /* Offset x/y to center */
    dx = halfW - cx;
    dy = halfH - cy;
    
    x += (dx * fCos) - (dy * fSin);
    y += (dy * fCos) + (dx * fSin);

    /* If flipping, swap x texture coords. */
    if (turn) { float tmp = tx1; tx1 = tx2; tx2 = tmp; }
    
    /* Calculate extents */
    halfWcos = halfW * fCos;
    halfHsin = halfH * fSin;
    xext1 = halfWcos + halfHsin;
    xext2 = halfWcos - halfHsin;
    halfHcos = halfH * fCos;
    halfWsin = halfW * fSin;
    yext1 = halfHcos + halfWsin;
    yext2 = halfHcos - halfWsin;
    
    /* Write vertices! */
    v[0].x = x - xext1; v[0].y = y - yext1; v[0].tcx = tx1; v[0].tcy = ty1; v[0].color = vColor;
    v[1].x = x + xext2; v[1].y = y - yext2; v[1].tcx = tx2; v[1].tcy = ty1; v[1].color = vColor;
    v[2].x = x - xext2; v[2].y = y + yext2; v[2].tcx = tx1; v[2].tcy = ty2; v[2].color = vColor;
    v[3] = v[2];
    v[4] = v[1];
    v[5].x = x + xext1; v[5].y = y + yext1; v[5].tcx = tx2; v[5].tcy = ty2; v[5].color = vColor;
    
    return 0;
}

int PL_Draw_RotaGraphF(float x, float y, 
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    int textureRefID;
    SDL_Rect texRect;
    float xMult, yMult;
    
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) < 0) {
        return -1;
    }
    
    return s_Draw_RotaGraphMain(textureRefID, &texRect, xMult, yMult, x, y,
                                (float)(texRect.w / 2), (float)(texRect.h / 2),
                                (float)scaleFactor, (float)scaleFactor, (float)angle,
                                graphID, blendFlag, turn);
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
    int textureRefID;
    SDL_Rect texRect;
    float xMult, yMult;
    
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) < 0) {
        return -1;
    }
    
    return s_Draw_RotaGraphMain(textureRefID, &texRect, xMult, yMult, x, y,
                                cx, cy,
                                (float)scaleFactor, (float)scaleFactor, (float)angle,
                                graphID, blendFlag, turn);
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
    int textureRefID;
    SDL_Rect texRect;
    float xMult, yMult;
    
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) < 0) {
        return -1;
    }
    
    return s_Draw_RotaGraphMain(textureRefID, &texRect, xMult, yMult, x, y,
                                cx, cy,
                                (float)xScaleFactor, (float)yScaleFactor, (float)angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RotaGraph3(int x, int y, int cx, int cy,
                       double xScaleFactor, double yScaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph3F(
                (float)x, (float)y, (float)cx, (float)cy,
                xScaleFactor, yScaleFactor, (float)angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RectRotaGraphF(float x, float y,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    int textureRefID;
    SDL_Rect texRect;
    float xMult, yMult;
    
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) < 0) {
        return -1;
    }
    
    texRect.x += sx;
    texRect.y += sy;
    texRect.w = sw;
    texRect.h = sh;
    
    return s_Draw_RotaGraphMain(textureRefID, &texRect, xMult, yMult, x, y,
                                (float)(texRect.w / 2), (float)(texRect.h / 2),
                                (float)scaleFactor, (float)scaleFactor, (float)angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph(int x, int y, 
                          int sx, int sy, int sw, int sh,
                          double scaleFactor, double angle,
                          int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraphF((float)x, (float)y, sx, sy, sw, sh,
                                  scaleFactor, (float)angle,
                                  graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph2F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    int textureRefID;
    SDL_Rect texRect;
    float xMult, yMult;
    
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) < 0) {
        return -1;
    }
    
    texRect.x += sx;
    texRect.y += sy;
    texRect.w = sw;
    texRect.h = sh;
    
    return s_Draw_RotaGraphMain(textureRefID, &texRect, xMult, yMult, x, y,
                                cx, cy,
                                (float)scaleFactor, (float)scaleFactor, (float)angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph2(int x, int y, int cx, int cy,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph2F(
                (float)x, (float)y, sx, sy, sw, sh,
                (float)cx, (float)cy,
                scaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RectRotaGraph3F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double xScaleFactor, double yScaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    int textureRefID;
    SDL_Rect texRect;
    float xMult, yMult;
    
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) < 0) {
        return -1;
    }
    
    texRect.x += sx;
    texRect.y += sy;
    texRect.w = sw;
    texRect.h = sh;
    
    return s_Draw_RotaGraphMain(textureRefID, &texRect, xMult, yMult, x, y,
                                cx, cy,
                                (float)xScaleFactor, (float)yScaleFactor, (float)angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph3(int x, int y,
                           int sx, int sy, int sw, int sh,
                           int cx, int cy,
                           double xScaleFactor, double yScaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph3F(
                (float)x, (float)y, sx, sy, sw, sh,
                (float)cx, (float)cy,
                xScaleFactor, yScaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_ModiGraphF(
    float x1, float y1, float x2, float y2,
    float x3, float y3, float x4, float y4,
    int graphID, int blendFlag
) {
    SDL_Rect texRect;
    int textureRefID;
    float xMult, yMult;
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) >= 0) {
        Uint32 vColor = s_getColor();
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
        float tx1 = texRect.x * xMult;
        float ty1 = texRect.y * yMult;
        float tx2 = tx1 + (texRect.w * xMult);
        float ty2 = ty1 + (texRect.h * yMult);
        
        v[0].x = x1; v[0].y = y1; v[0].tcx = tx1; v[0].tcy = ty1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y2; v[1].tcx = tx2; v[1].tcy = ty1; v[1].color = vColor;
        v[2].x = x4; v[2].y = y4; v[2].tcx = tx1; v[2].tcy = ty2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x3; v[5].y = y3; v[5].tcx = tx2; v[5].tcy = ty2; v[5].color = vColor;
    }
    
    return 0;
}

int PL_Draw_ModiGraph(
    int x1, int y1, int x2, int y2,
    int x3, int y3, int x4, int y4,
    int graphID, int blendFlag
) {
    return PL_Draw_ModiGraphF((float)x1, (float)y1, (float)x2, (float)y2,
                              (float)x3, (float)y3, (float)x4, (float)y4,
                              graphID, blendFlag);
}

int PL_Draw_TurnGraphF(float x1, float y1, int graphID, int blendFlag) {
    SDL_Rect texRect;
    int textureRefID;
    float xMult, yMult;
    if (PL_Texture_RenderGetGraphTextureInfo(graphID, &textureRefID, &texRect, &xMult, &yMult) >= 0) {
        Uint32 vColor = s_getColor();
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6);
        float x2, y2;
        float tx1 = texRect.x * xMult;
        float ty1 = texRect.y * yMult;
        float tx2 = tx1 + (texRect.w * xMult);
        float ty2 = ty1 + (texRect.h * yMult);
        
        x1 -= 0.5f; y1 -= 0.5f;
        x2 = x1 + texRect.w; y2 = y1 + texRect.h;
        
        v[0].x = x1; v[0].y = y1; v[0].tcx = tx2; v[0].tcy = ty1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y1; v[1].tcx = tx1; v[1].tcy = ty1; v[1].color = vColor;
        v[2].x = x1; v[2].y = y2; v[2].tcx = tx2; v[2].tcy = ty2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x2; v[5].y = y2; v[5].tcx = tx1; v[5].tcy = ty2; v[5].color = vColor;
    }
    
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
