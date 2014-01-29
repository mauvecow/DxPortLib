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

static int s_blendMode = DX_BLENDMODE_NOBLEND;
static int s_lastBlendMode = -1;
static int s_drawMode = DX_DRAWMODE_NEAREST;
static Uint32 s_blendFlags;
static Uint32 s_drawColorR = 0xff;
static Uint32 s_drawColorG = 0xff;
static Uint32 s_drawColorB = 0xff;
static Uint32 s_drawColorA = 0xff000000;
static Uint32 s_bgColorR = 0x00;
static Uint32 s_bgColorG = 0x00;
static Uint32 s_bgColorB = 0x00;

int PL_Draw_ResetSettings() {
    s_blendMode = DX_BLENDMODE_NOBLEND;
    s_drawMode = DX_DRAWMODE_NEAREST;
    s_lastBlendMode = -1;
    
    s_drawColorR = 0xff;
    s_drawColorG = 0xff;
    s_drawColorB = 0xff;
    s_drawColorA = 0xff000000;
    
    s_bgColorR = 0x00;
    s_bgColorG = 0x00;
    s_bgColorB = 0x00;
    
    return 0;
}

/* ------------------------------------------------------- BLENDING MODES */

typedef struct BlendInfo {
    GLfloat texEnvParam;
    GLenum blendEquation;
    GLenum srcRGBBlend;
    GLenum destRGBBlend;
    GLenum srcAlphaBlend;
    GLenum destAlphaBlend;
    Uint32 blendFlags;
} BlendInfo;

#define NOBLEND 0xfffffff

#define BLENDFLAG_4X    (0x1)

static const BlendInfo s_blendModeTable[DX_BLENDMODE_NUM] = {
    { GL_MODULATE,      GL_FUNC_ADD, NOBLEND, NOBLEND, NOBLEND, NOBLEND, 0 }, /* NOBLEND */
    { GL_MODULATE,      GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, 0 }, /* ALPHA */
    { GL_MODULATE,      GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE, 0 }, /* ADD */
    
    /* This is different from DxLib's implementation of DX_BLENDMODE_SUB,
     * but it looks the same as far as I know. */
    { GL_MODULATE,      GL_FUNC_REVERSE_SUBTRACT, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE, 0 }, /* SUB */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE, 0 }, /* MUL */
    { GL_MODULATE,      GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE, 0 }, /* SUB2 */
    
    { GL_MODULATE,      GL_FUNC_ADD, NOBLEND, NOBLEND, NOBLEND, NOBLEND, 0 }, /* XOR (unsupported) */
    { GL_MODULATE,      GL_FUNC_ADD, NOBLEND, NOBLEND, NOBLEND, NOBLEND, 0 }, /* reserved */
    
    { GL_MODULATE,      GL_FUNC_ADD, GL_ZERO, GL_ONE, GL_ZERO, GL_ONE, 0 }, /* DESTCOLOR */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE_MINUS_DST_COLOR, GL_ZERO, GL_ZERO, GL_ONE, 0 }, /* INVDESTCOLOR */
    { GL_BLEND,         GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE, 0 }, /* INVSRC */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE, 0 }, /* MULA */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, BLENDFLAG_4X }, /* ALPHA_X4 */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE, BLENDFLAG_4X }, /* ADD_X4 */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE, GL_ZERO, GL_ONE, GL_ZERO, 0 }, /* SRCCOLOR */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE, 0 }, /* HALF_ADD */
    { GL_MODULATE,      GL_FUNC_REVERSE_SUBTRACT, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE, 0 }, /* SUB1 */
    
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, 0 }, /* PMA_ALPHA */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE, GL_ONE, GL_ONE, GL_ONE, 0 }, /* PMA_ADD */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_REVERSE_SUBTRACT, GL_ONE, GL_ONE, GL_ONE, GL_ONE, 0 }, /* PMA_SUB */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE, 0 }, /* PMA_INVSRC */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, BLENDFLAG_4X }, /* PMA_ALPHA_X4 */ /* FIXME */
    { GL_MODULATE,      GL_FUNC_ADD, GL_ONE, GL_ONE, GL_ONE, GL_ONE, BLENDFLAG_4X }, /* PMA_ADD_X4 */ /* FIXME */
};

static int s_ApplyBlendMode(int blendMode, int forceBlend) {
    const BlendInfo *blend;
    
    if (forceBlend != 0 && blendMode == DX_BLENDMODE_NOBLEND) {
        blendMode = DX_BLENDMODE_ALPHA;
    }
    
    if (blendMode == s_lastBlendMode) {
        return 0;
    }
    if (blendMode < 0 || blendMode >= DX_BLENDMODE_NUM) {
        blendMode = DX_BLENDMODE_NOBLEND;
        if (blendMode == s_lastBlendMode) {
            return 0;
        }
    }
    
    s_lastBlendMode = blendMode;
    
    blend = &s_blendModeTable[blendMode];
    s_blendFlags = blend->blendFlags;
    
    if (blend->srcRGBBlend == NOBLEND) {
        PL_GL.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        PL_GL.glDisable(GL_BLEND);
    } else {
        PL_GL.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, blend->texEnvParam);
        PL_GL.glBlendFuncSeparate(blend->srcRGBBlend, blend->destRGBBlend, blend->srcAlphaBlend, blend->destAlphaBlend);
        PL_GL.glBlendEquation(blend->blendEquation);
        PL_GL.glEnable(GL_BLEND);
    }
    
    return 0;
}

/* --------------------------------------------------------- VERTEX CACHE */

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
    int blendFlag;
    
    GLenum drawMode;
    
    unsigned char *vertexData;
    int vertexDataPosition;
    int vertexDataSize;
} VertexCache;

static VertexCache s_cache;


/* Given a call with a vertex definition and the number of vertices,
 * returns the starting vertex pointer.
 */
static void *s_BeginCache(
    const VertexDefinition *definitionArray, int defCount,
    int vertexSize, int vertexCount,
    GLenum drawMode, int textureRefID, int blendFlag
) {
    /* - If this is the same as the last definition, try to continue it. */
    if (s_cache.defArray == definitionArray
        && s_cache.drawMode == drawMode
        && s_cache.textureRefID == textureRefID
        && s_cache.blendFlag == blendFlag
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
    s_cache.blendFlag = blendFlag;
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
#define START(vertexName, VertexType, drawMode, textureRefID, vertexCount, blendFlag) \
    VertexType *vertexName = (VertexType *)s_BeginCache( \
                               s_def ## VertexType, elementsof(s_def ## VertexType), \
                               sizeof(VertexType), vertexCount, \
                               drawMode, textureRefID, blendFlag)

int PL_Draw_FlushCache() {
    int i;
    int vertexSize;
    unsigned char *vertexData;
    const VertexDefinition *def;
    
    if (s_cache.defArray == NULL || s_cache.vertexCount == 0) {
        s_cache.vertexDataPosition = 0;
        return 0;
    }
    
    /* Apply blending mode */
    if (s_cache.blendFlag) {
        s_ApplyBlendMode(s_blendMode, PL_Texture_HasAlphaChannel(s_cache.textureRefID));
    } else {
        s_ApplyBlendMode(DX_BLENDMODE_NOBLEND, DXFALSE);
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
    PL_Texture_Bind(s_cache.textureRefID, s_drawMode);
    
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
    s_cache.vertexData = SDL_malloc((size_t)s_cache.vertexDataSize);
    
    return 0;
}

int PL_Draw_DestroyCache() {
    if (s_cache.vertexData != NULL) {
        SDL_free(s_cache.vertexData);
    }
    
    SDL_memset(&s_cache, 0, sizeof(s_cache));
    
    return 0;
}

/* --------------------------------------------------------- DRAWING CODE */

static inline Uint32 s_getColor() {
    return (s_drawColorR) | (s_drawColorG << 8) | (s_drawColorB << 16) | s_drawColorA;
}
static SDL_INLINE Uint32 s_modulateColor(DXCOLOR color) {
    Uint32 r = ((color & 0xff) * s_drawColorR) / 0xff;
    Uint32 g = (((color & 0xff00) * s_drawColorG) / 0xff) & 0x0000ff00;
    Uint32 b = (((color & 0xff0000) * s_drawColorB) / 0xff) & 0x00ff0000;
    
    return s_drawColorA | r | g | b;
}

int PL_Draw_PixelF(float x, float y, DXCOLOR color) {
    Uint32 vColor = s_modulateColor(color);
    START(v, VertexPosition2Color, GL_POINTS, -1, 1, DXTRUE);
    v[0].x = x; v[0].y = y; v[0].color = vColor;
    
    return 0;
}

int PL_Draw_Pixel(int x, int y, DXCOLOR color) {
    return PL_Draw_PixelF((float)x, (float)y, color);
}

int PL_Draw_LineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    Uint32 vColor = s_modulateColor(color);
    if (thickness <= 1) {
        START(v, VertexPosition2Color, GL_LINES, -1, 2, DXTRUE);
    
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y2; v[1].color = vColor;
    } else {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float l = (float)SDL_sqrt((dx * dx) + (dy * dy));
        
        if (l > 0) {
            START(v, VertexPosition2Color, GL_TRIANGLES, -1, 6, DXTRUE);
            float t = (float)thickness * 0.5f;
            float nx = (dx / l) * t;
            float ny = (dy / l) * t;
            
            v[0].x = x1 - ny; v[0].y = y1 + nx; v[0].color = vColor;
            v[1].x = x2 - ny; v[1].y = y2 + nx; v[1].color = vColor;
            v[2].x = x1 + ny; v[2].y = y1 - nx; v[2].color = vColor;
            v[3] = v[2];
            v[4] = v[1];
            v[5].x = x2 + ny; v[5].y = y2 - nx; v[5].color = vColor;
        }
    }
    
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
    
    if (fillFlag) {
        int points = 36;
        float amount = ((float)M_PI * 2) / (float)points;
        START(v, VertexPosition2Color, GL_TRIANGLE_FAN, -1, points, DXTRUE);
        int i;
        
        for (i = 0; i < points; ++i) {
            v[i].x = x + (SDL_cosf((float)i * amount) * rx);
            v[i].y = y + (SDL_sinf((float)i * amount) * ry);
            v[i].color = vColor;
        }
        
        /* we don't want triangle fan to bleed over, so clear it out now. */
        PL_Draw_FlushCache();
    } else {
        int points = 36;
        float amount = ((float)M_PI * 2) / (float)points;
        START(v, VertexPosition2Color, GL_LINES, -1, points * 2, DXTRUE);
        VertexPosition2Color *sv = v;
        int i;
        
        v[0].x = x + rx;
        v[0].y = y;
        v[0].color = vColor;
        v += 1;
        
        for (i = 1; i < points; ++i) {
            v->x = x + (SDL_cosf((float)i * amount) * rx);
            v->y = y + (SDL_sinf((float)i * amount) * ry);
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
    
    if (fillFlag) {
        START(v, VertexPosition2Color, GL_TRIANGLES, -1, 3, DXTRUE);
    
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y2; v[1].color = vColor;
        v[2].x = x3; v[2].y = y3; v[2].color = vColor;
    } else {
        START(v, VertexPosition2Color, GL_LINES, -1, 6, DXTRUE);
    
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
    
    if (fillFlag) {
        START(v, VertexPosition2Color, GL_TRIANGLES, -1, 6, DXTRUE);
    
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y2; v[1].color = vColor;
        v[2].x = x3; v[2].y = y3; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x4; v[5].y = y4; v[5].color = vColor;
    } else {
        START(v, VertexPosition2Color, GL_LINES, -1, 8, DXTRUE);
    
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
    
    if (FillFlag) {
        /* TRIANGLES instead of TRIANGLE_STRIP so that we can batch. */
        START(v, VertexPosition2Color, GL_TRIANGLES, -1, 6, DXTRUE);
        
        v[0].x = x1; v[0].y = y1; v[0].color = vColor;
        v[1].x = x2; v[1].y = y1; v[1].color = vColor;
        v[2].x = x1; v[2].y = y2; v[2].color = vColor;
        v[3] = v[2];
        v[4] = v[1];
        v[5].x = x2; v[5].y = y2; v[5].color = vColor;
    } else {
        /* LINES instead of LINE_LOOP so that we can batch. */
        START(v, VertexPosition2Color, GL_LINES, -1, 8, DXTRUE);
        
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
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
        float x2, y2;
        float tx1 = (float)texRect.x * xMult;
        float ty1 = (float)texRect.y * yMult;
        float tw = (float)texRect.w;
        float th = (float)texRect.h;
        float tx2 = tx1 + (tw * xMult);
        float ty2 = ty1 + (th * yMult);
        
        x2 = x1 + tw; y2 = y1 + th;
        
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
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
        float tx1 = (float)texRect.x * xMult;
        float ty1 = (float)texRect.y * yMult;
        float tx2 = tx1 + ((float)texRect.w * xMult);
        float ty2 = ty1 + ((float)texRect.h * yMult);
        
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
            
            tx1 = (float)(texRect.x + sx) * xMult;
            ty1 = (float)(texRect.y + sy) * yMult;
            tx2 = tx1 + ((float)sw * xMult);
            ty2 = ty1 + ((float)sh * yMult);
            
            if (turnFlag) { float tmp = dx1; dx1 = dx2; dx2 = tmp; }
        }
        
        /* - draw! */
        {
            START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
            
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
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
        float dx2, dy2;
        float tx1 = (float)(texRect.x + sx) * xMult;
        float ty1 = (float)(texRect.y + sy) * yMult;
        float tw = (float)sw;
        float th = (float)sh;
        float tx2 = tx1 + (tw * xMult);
        float ty2 = ty1 + (th * yMult);
        
        dx2 = dx1 + dw; dy2 = dy1 + dh;
        
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
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
        float tx1 = (float)(texRect.x + sx) * xMult;
        float ty1 = (float)(texRect.y + sy) * yMult;
        float tx2 = tx1 + ((float)sw * xMult);
        float ty2 = ty1 + ((float)sh * yMult);
        
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
    START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
    float tw = (float)texRect->w;
    float th = (float)texRect->h;
    float tx1 = (float)texRect->x * xMult;
    float ty1 = (float)texRect->y * yMult;
    float tx2 = tx1 + (tw * xMult);
    float ty2 = ty1 + (th * yMult);
    float fSin = SDL_sinf(angle);
    float fCos = SDL_cosf(angle);
    float dx, dy;
    float halfW = (tw * xScaleFactor) * 0.5f;
    float halfH = (th * yScaleFactor) * 0.5f;
    float halfWcos, halfHcos, halfWsin, halfHsin;
    float xext1, xext2;
    float yext1, yext2;
    
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
    halfHcos = halfH * fCos;
    halfWsin = halfW * fSin;
    xext1 = halfWcos - halfHsin;
    xext2 = halfWcos + halfHsin;
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
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
        float tx1 = (float)texRect.x * xMult;
        float ty1 = (float)texRect.y * yMult;
        float tx2 = tx1 + ((float)texRect.w * xMult);
        float ty2 = ty1 + ((float)texRect.h * yMult);
        
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
        START(v, VertexPosition2Tex2Color, GL_TRIANGLES, textureRefID, 6, blendFlag);
        float x2, y2;
        float tx1 = (float)texRect.x * xMult;
        float ty1 = (float)texRect.y * yMult;
        float tw = (float)texRect.w;
        float th = (float)texRect.h;
        float tx2 = tx1 + (tw * xMult);
        float ty2 = ty1 + (th * yMult);
        
        x2 = x1 + tw; y2 = y1 + th;
        
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

static int s_scissorEnabled = DXFALSE;
static int s_scissorX = 0;
static int s_scissorY = 0;
static int s_scissorW = 0;
static int s_scissorH = 0;

static void s_RefreshScissor() {
    if (s_scissorEnabled == DXFALSE) {
        PL_GL.glDisable(GL_SCISSOR_TEST);
    } else {
        PL_GL.glEnable(GL_SCISSOR_TEST);
        PL_GL.glScissor(s_scissorX, s_scissorY, s_scissorW, s_scissorH);
    }
}

int PL_Draw_SetDrawArea(int x1, int y1, int x2, int y2) {
    PL_Draw_FlushCache();
    
    if (x1 == 0 && y1 == 0 && x2 == PL_drawScreenWidth && y2 == PL_drawScreenHeight) {
        s_scissorEnabled = DXFALSE;
    } else {
        
        s_scissorEnabled = DXTRUE;
        s_scissorX = x1;
        s_scissorY = y1;
        s_scissorW = x2 - x1;
        s_scissorH = y2 - y1;
    }
    
    s_RefreshScissor();
    
    return 0;
}

int PL_Draw_SetBackgroundColor(int red, int green, int blue) {
    s_bgColorR = red;
    s_bgColorG = green;
    s_bgColorB = blue;
    return 0;
}

int PL_Draw_ClearDrawScreen(const RECT *rect) {
    PL_Draw_FlushCache();
    
    PL_GL.glClearColor(s_bgColorR / 255.0f, s_bgColorG / 255.0f, s_bgColorB / 255.0f, 1);
    if (rect == NULL) {
        PL_GL.glDisable(GL_SCISSOR_TEST);
        PL_GL.glClear(GL_COLOR_BUFFER_BIT);
    } else {
        PL_GL.glEnable(GL_SCISSOR_TEST);
        PL_GL.glScissor(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);
        PL_GL.glClear(GL_COLOR_BUFFER_BIT);
    }
    
    s_RefreshScissor();
    
    return 0;
}

int PL_Draw_SetDrawMode(int drawMode) {
    if (drawMode != s_drawMode) {
        PL_Draw_FlushCache();
        
        s_drawMode = drawMode;
    }
    
    return 0;
}
int PL_Draw_GetDrawMode() {
    return s_drawMode;
}

int PL_Draw_SetDrawBlendMode(int blendMode, int alpha) {
    /* Changing blend mode forces a cache flush. */
    if (blendMode != s_blendMode) {
        PL_Draw_FlushCache();
        
        s_blendMode = blendMode;
    }
    
    if (blendMode == DX_BLENDMODE_NOBLEND) {
        alpha = 255;
    }
    
    /* Changing draw color does not cause a cache flush. */
    s_drawColorA = (Uint32)alpha << 24;
    
    return 0;
}

int PL_Draw_GetDrawBlendMode(int *blendMode, int *alpha) {
    *blendMode = s_blendMode;
    *alpha = (int)(s_drawColorA >> 24);
    
    return 0;
}

int PL_Draw_SetBright(int redBright, int greenBright, int blueBright) {
    s_drawColorR = redBright & 0xff;
    s_drawColorG = greenBright & 0xff;
    s_drawColorB = blueBright & 0xff;
    return 0;
}

int PL_Draw_GetBright(int *redBright, int *greenBright, int *blueBright) {
    *redBright = (int)s_drawColorR;
    *greenBright = (int)s_drawColorG;
    *blueBright = (int)s_drawColorB;
    
    return 0;
}

int PL_Draw_SetBasicBlendFlag(int blendFlag) {
    /* Reseved for software renderer, so it won't be used at all. */
    return 0;
}

int PL_Draw_ForceUpdate() {
    s_lastBlendMode = -1;
    
    s_RefreshScissor();
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
