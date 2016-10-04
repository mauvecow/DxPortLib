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

/* Unlike the rest of the library, this is intended as a replacement for the
 * Luna library used by many doujin games.
 *
 * As the two libraries have much in common, this is simply a wrapper to
 * DxLib's functionality.
 */

#ifndef DXPORTLIB_LUNAINTERNAL_H

#include "DPLBuildConfig.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "Luna.h"

#include "PL/PLInternal.h"

/* types, vertex definitions, etc */
typedef struct _LunaTexcoords {
    float u, v;
} LunaTexcoords;

typedef struct _LunaVertex2DTex0 {
    float x, y, z, w;
    unsigned int color;
} LunaVertex2DTex0;
static const VertexElement s_LunaVertex2DTex0Elements[] = {
    { VERTEX_POSITION, 4, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex0, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex2DTex0, color) },
};
VERTEX_DEFINITION(LunaVertex2DTex0)

typedef struct _LunaVertex2DTex1 {
    float x, y, z, w;
    unsigned int color;
    LunaTexcoords t[1];
} LunaVertex2DTex1;
static const VertexElement s_LunaVertex2DTex1Elements[] = {
    { VERTEX_POSITION, 4, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex1, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex2DTex1, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex1, t[0]) },
};
VERTEX_DEFINITION(LunaVertex2DTex1)

typedef struct _LunaVertex2DTex2 {
    float x, y, z, w;
    unsigned int color;
    LunaTexcoords t[2];
} LunaVertex2DTex2;
static const VertexElement s_LunaVertex2DTex2Elements[] = {
    { VERTEX_POSITION, 4, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex2, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex2DTex2, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex2, t[0]) },
    { VERTEX_TEXCOORD1, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex2, t[1]) },
};
VERTEX_DEFINITION(LunaVertex2DTex2)

typedef struct _LunaVertex2DTex3 {
    float x, y, z, w;
    unsigned int color;
    LunaTexcoords t[3];
} LunaVertex2DTex3;
static const VertexElement s_LunaVertex2DTex3Elements[] = {
    { VERTEX_POSITION, 4, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex3, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex2DTex3, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex3, t[0]) },
    { VERTEX_TEXCOORD1, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex3, t[1]) },
    { VERTEX_TEXCOORD2, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex3, t[2]) },
};
VERTEX_DEFINITION(LunaVertex2DTex3)

typedef struct _LunaVertex2DTex4 {
    float x, y, z, w;
    unsigned int color;
    LunaTexcoords t[4];
} LunaVertex2DTex4;
static const VertexElement s_LunaVertex2DTex4Elements[] = {
    { VERTEX_POSITION, 4, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex4, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex2DTex4, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex4, t[0]) },
    { VERTEX_TEXCOORD1, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex4, t[1]) },
    { VERTEX_TEXCOORD2, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex4, t[2]) },
    { VERTEX_TEXCOORD3, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex2DTex4, t[3]) },
};
VERTEX_DEFINITION(LunaVertex2DTex4)

typedef struct _LunaVertex3DTex0 {
    float x, y, z;
    unsigned int color;
} LunaVertex3DTex0;
static const VertexElement s_LunaVertex3DTex0Elements[] = {
    { VERTEX_POSITION, 3, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex0, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex3DTex0, color) },
};
VERTEX_DEFINITION(LunaVertex3DTex0)

typedef struct _LunaVertex3DTex1 {
    float x, y, z;
    unsigned int color;
    LunaTexcoords t[1];
} LunaVertex3DTex1;
static const VertexElement s_LunaVertex3DTex1Elements[] = {
    { VERTEX_POSITION, 3, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex1, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex3DTex1, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex1, t[0]) },
};
VERTEX_DEFINITION(LunaVertex3DTex1)

typedef struct _LunaVertex3DTex2 {
    float x, y, z;
    unsigned int color;
    LunaTexcoords t[2];
} LunaVertex3DTex2;
static const VertexElement s_LunaVertex3DTex2Elements[] = {
    { VERTEX_POSITION, 3, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex2, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex3DTex2, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex2, t[0]) },
    { VERTEX_TEXCOORD1, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex2, t[1]) },
};
VERTEX_DEFINITION(LunaVertex3DTex2)

typedef struct _LunaVertex3DTex3 {
    float x, y, z;
    unsigned int color;
    LunaTexcoords t[3];
} LunaVertex3DTex3;
static const VertexElement s_LunaVertex3DTex3Elements[] = {
    { VERTEX_POSITION, 3, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex3, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex3DTex3, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex3, t[0]) },
    { VERTEX_TEXCOORD1, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex3, t[1]) },
    { VERTEX_TEXCOORD2, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex3, t[2]) },
};
VERTEX_DEFINITION(LunaVertex3DTex3)

typedef struct _LunaVertex3DTex4 {
    float x, y, z;
    unsigned int color;
    LunaTexcoords t[4];
} LunaVertex3DTex4;
static const VertexElement s_LunaVertex3DTex4Elements[] = {
    { VERTEX_POSITION, 3, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex4, x) },
    { VERTEX_COLOR, 4, VERTEXSIZE_UNSIGNED_BYTE, offsetof(LunaVertex3DTex4, color) },
    { VERTEX_TEXCOORD0, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex4, t[0]) },
    { VERTEX_TEXCOORD1, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex4, t[1]) },
    { VERTEX_TEXCOORD2, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex4, t[2]) },
    { VERTEX_TEXCOORD3, 2, VERTEXSIZE_FLOAT, offsetof(LunaVertex3DTex4, t[3]) },
};
VERTEX_DEFINITION(LunaVertex3DTex4)

typedef struct _LunaVertexInfo {
    const VertexDefinition *def;
    unsigned int texCount;
} LunaVertexInfo;
static const LunaVertexInfo s_LunaVertex2DLookup[] = {
    { &s_LunaVertex2DTex0Definition, 0 },
    { &s_LunaVertex2DTex1Definition, 1 },
    { &s_LunaVertex2DTex2Definition, 2 },
    { &s_LunaVertex2DTex3Definition, 3 },
    { &s_LunaVertex2DTex4Definition, 4 },
};

static const LunaVertexInfo s_LunaVertex3DLookup[5] = {
    { &s_LunaVertex3DTex0Definition, 0 },
    { &s_LunaVertex3DTex1Definition, 1 },
    { &s_LunaVertex3DTex2Definition, 2 },
    { &s_LunaVertex3DTex3Definition, 3 },
    { &s_LunaVertex3DTex4Definition, 4 },
};

static inline const LunaVertexInfo *s_GetLunaVertex2DInfo(eVertexPrimitiveType type) {
    if (type < PRIM_VERTEX_UV0 || type > PRIM_VERTEX_UV4) {
        return NULL;
    }
    return &s_LunaVertex2DLookup[type - PRIM_VERTEX_UV0];
}
static inline const LunaVertexInfo *s_GetLunaVertex3DInfo(eVertexPrimitiveType type) {
    if (type < PRIM_VERTEX_UV0 || type > PRIM_VERTEX_UV4) {
        return NULL;
    }
    return &s_LunaVertex3DLookup[type - PRIM_VERTEX_UV0];
}

extern float g_lunaFilterOffset;
extern int g_lunaFilterMode;
extern int g_luna3DCamera;
extern float g_lunaVirtualXmult;
extern float g_lunaVirtualYmult;
extern int g_lunaUseCharSet;
extern int g_lunaSyncOffset;

extern int g_lunaAlphaTestPreset;
extern float g_lunaAlphaTestValue;
extern int g_lunaTexturePreset;
extern PLMatrix g_lunaUntransformedProjectionMatrix;
extern PLMatrix g_lunaUntransformedViewMatrix;
extern PLMatrix g_lunaProjectionMatrix;
extern PLMatrix g_lunaViewMatrix;

extern RECT g_viewportRect;

extern void LunaCamera_SetDevice(LCAMERA lCam);

extern void LunaInput_Refresh();

extern void LunaFile_Init();
extern void LunaFile_End();

extern void LunaMath_Init();

extern void Luna3DStartDraw(int textureID);

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */

#endif /* #ifndef DXPORTLIB_LUNAINTERNAL_H */
