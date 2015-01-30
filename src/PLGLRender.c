/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2014 Patrick McCarthy <mauve@sandwich.net>

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

/* GL code is very generic. Instead of doing any drawing itself, it has two
 * basic portions:
 * 
 * - Maintains the current render state information, including blending,
 *   culling, scissoring, viewport, texture sampling, shaders.
 * - Draws vertices and vertex buffers using the vertex declaration and
 *   currently available state information.
 * 
 * In theory this should be very fast and only need to update state when
 * actually necessary to do so.
 */

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "PLInternal.h"

#include "PLGLInternal.h"

#include "SDL.h"

static int s_pixelTexture = -1;

/* ------------------------------------------------------- VIEW MATRICES */
static PLMatrix s_matrixProjection;
static PLMatrix s_matrixView;
static PLMatrix s_currentMatrixProjection;
static PLMatrix s_currentMatrixView;
static int s_matrixDirty = DXTRUE;
static int s_displayUntransformed = DXFALSE;
static int s_viewportX = 0;
static int s_viewportY = 0;
static int s_viewportW = 640;
static int s_viewportH = 480;
static float s_nearZ = 0;
static float s_farZ = 1;

int PL_Render_SetViewport(int x, int y, int w, int h) {
    PL_GL.glViewport(x, y, w, h);
    
    s_viewportX = x;
    s_viewportY = y;
    s_viewportW = w;
    s_viewportH = h;
    
    if (s_displayUntransformed == DXTRUE) {
        s_matrixDirty = DXTRUE;
    }
    
    return 0;
}

int PL_Render_SetZRange(float nearZ, float farZ) {
    s_nearZ = nearZ;
    s_farZ = farZ;
    
    if (s_displayUntransformed == DXTRUE) {
        s_matrixDirty = DXTRUE;
    }
    
    return 0;
}

int PL_Render_SetMatrices(const PLMatrix *projection, const PLMatrix *view) {
    PL_Matrix_Copy(&s_matrixProjection, projection);
    PL_Matrix_Copy(&s_matrixView, view);
    
    if (s_displayUntransformed == DXFALSE) {
        s_matrixDirty = DXTRUE;
    }
    
    return 0;
}

int PL_Render_SetUntransformedFlag(int untransformedFlag) {
    if (untransformedFlag != s_displayUntransformed) {
        s_displayUntransformed = untransformedFlag;
        s_matrixDirty = DXTRUE;
    }
    
    return 0;
}

int PL_Render_SetMatrixDirtyFlag() {
    s_matrixDirty = DXTRUE;
    return 0;
}

int PL_Render_UpdateMatrices() {
    if (s_matrixDirty == DXFALSE) {
        return 0;
    }
    
    if (s_displayUntransformed == DXTRUE) {
        PL_Matrix_CreateOrthoOffCenterLH(&s_currentMatrixProjection,
                      (float)s_viewportX, (float)(s_viewportX + s_viewportW),
                      (float)s_viewportY, (float)(s_viewportY + s_viewportH),
                      s_nearZ, s_farZ);
        PL_Matrix_CreateTranslation(&s_currentMatrixView, 0.5f, 0.5f, 0.0f);
    } else {
        PL_Matrix_Copy(&s_currentMatrixProjection, &s_matrixProjection);
        PL_Matrix_Copy(&s_currentMatrixView, &s_matrixView);
    }
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glMatrixMode(GL_PROJECTION);
    PL_GL.glLoadMatrixf((const float *)&s_currentMatrixProjection);
    
    PL_GL.glMatrixMode(GL_MODELVIEW);
    PL_GL.glLoadMatrixf((const float *)&s_currentMatrixView);
#endif
    
    s_matrixDirty = DXFALSE;
    
    return 0;
}

/* ------------------------------------------------------------ BLENDING */

static GLuint BlendTypeToGL(int blendType) {
    switch(blendType) {
        default: return GL_ZERO; /* PL_BLEND_ZERO */
        case PL_BLEND_ONE: return GL_ONE;
        case PL_BLEND_SRC_COLOR: return GL_SRC_COLOR;
        case PL_BLEND_DST_COLOR: return GL_DST_COLOR;
        case PL_BLEND_SRC_ALPHA: return GL_SRC_ALPHA;
        case PL_BLEND_DST_ALPHA: return GL_DST_ALPHA;
        case PL_BLEND_ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
        case PL_BLEND_ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
        case PL_BLEND_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        case PL_BLEND_ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
    }
}

static GLuint BlendFuncToGL(int blendFunc) {
    switch(blendFunc) {
        case PL_BLENDFUNC_RSUB: return GL_FUNC_REVERSE_SUBTRACT;
        default: return GL_FUNC_ADD; /* PL_BLENDFUNC_ADD */
    }
}

void PL_Render_SetBlendMode(
    int blendEquation,
    int srcBlend, int destBlend
) {
    PL_GL.glBlendFunc(BlendTypeToGL(srcBlend), BlendTypeToGL(destBlend));
    PL_GL.glBlendEquation(BlendFuncToGL(blendEquation));
    PL_GL.glEnable(GL_BLEND);
}
void PL_Render_SetBlendModeSeparate(
    int blendEquation,
    int srcRGBBlend, int destRGBBlend,
    int srcAlphaBlend, int destAlphaBlend
) {
    PL_GL.glBlendFuncSeparate(
        BlendTypeToGL(srcRGBBlend), BlendTypeToGL(destRGBBlend),
        BlendTypeToGL(srcAlphaBlend), BlendTypeToGL(destAlphaBlend));
    PL_GL.glBlendEquation(BlendFuncToGL(blendEquation));
    PL_GL.glEnable(GL_BLEND);
}
void PL_Render_DisableBlend() {
    PL_GL.glDisable(GL_BLEND);
}

/* ---------------------------------------------------------- ALPHA TEST */
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
int PL_Render_EnableAlphaTest() {
    PL_GL.glEnable(GL_ALPHA_TEST);
    PL_GL.glAlphaFunc(GL_GREATER, 0);
    
    return 0;
}
int PL_Render_DisableAlphaTest() {
    PL_GL.glDisable(GL_ALPHA_TEST);
    return 0;
}
#else
/* OpenGL ES2 mode */
static int s_ES2_AlphaTestEnable = DXFALSE;

int PL_Render_EnableAlphaTest() {
    s_ES2_AlphaTestEnable = DXTRUE;
    return 0;
}
int PL_Render_DisableAlphaTest() {
    s_ES2_AlphaTestEnable = DXFALSE;
    return 0;
}
#endif

/* ----------------------------------------------------- SCISSOR/CULLING */

int PL_Render_SetScissor(int x, int y, int w, int h) {
    PL_GL.glEnable(GL_SCISSOR_TEST);
    PL_GL.glScissor(x, y, w, h);
    return 0;
}

int PL_Render_DisableScissor() {
    PL_GL.glDisable(GL_SCISSOR_TEST);
    return 0;
}

int PL_Render_SetScissorRect(const RECT *rect) {
    if (rect == NULL) {
        return PL_Render_DisableScissor();
    } else {
        return PL_Render_SetScissor(rect->left, rect->top,
                   rect->right - rect->left, rect->bottom - rect->top);
    }
}

/* ---------------------------------------------------- TEXTURE PROGRAMS */
#define MAX_TEXTURE 4
static int s_boundTextures[MAX_TEXTURE] = { -1 };
static unsigned int s_boundTextureCount = 0;

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
/* For OpenGL, glTexEnv* combiners can take the place of fragment programs.
 * In Direct3D, the equivalent is texture stages.
 * 
 * They're effectively interchangable, so what we do is we support a
 * fixed subset of texenv collections that are basically like shaders.
 * 
 * A sane implementation might include actual shaders in its place, but
 * not quite that far in yet.
 * 
 * Do not use Presets _and_ Shaders at the same time.
 * 
 * Most of the stuff here is DxLib only so if you don't build for DxLib
 * you don't need it.
 */

int PL_Render_SetTextureStage(unsigned int stage, int textureRefID, int textureDrawMode) {
    if (stage >= MAX_TEXTURE) {
        return -1;
    }
    
    PL_GL.glActiveTexture(GL_TEXTURE0 + stage);
    PL_Texture_Bind(textureRefID, textureDrawMode);
    //PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    s_boundTextures[stage] = textureRefID;
    
    stage += 1;
    if (stage > s_boundTextureCount) {
        s_boundTextureCount = stage;
    }
    return 0;
}

int PL_Render_ClearTextures() {
    unsigned int i;
    for (i = 0; i < s_boundTextureCount; ++i) {
        PL_GL.glActiveTexture(GL_TEXTURE0 + i);
        PL_Texture_Unbind(s_boundTextures[i]);
        s_boundTextures[i] = -1;
    }
    s_boundTextureCount = 0;
    return 0;
}

int PL_Render_ClearTexturePresetMode() {
    PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
    PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    PL_GL.glDisable(GL_BLEND);
    
    PL_Render_ClearTextures();
    
    return 0;
}

int PL_Render_SetTexturePresetMode(int preset,
                                   int textureRefID, int textureDrawMode) {
    unsigned int mainTexSlot = 0;
    int rgbScale = 1;
    
    PL_GL.glActiveTexture(GL_TEXTURE0);
    
    switch(preset) {
        case TEX_PRESET_COPY:
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            PL_GL.glDisable(GL_BLEND);
            PL_Render_SetTextureStage(0, textureRefID, textureDrawMode);
            return 0; /* return, not break! */
        case TEX_PRESET_DX_MULA:
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_ALPHA);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
            if (textureRefID < 0) {
                textureRefID = s_pixelTexture;
            } else {
                PL_Render_SetTextureStage(0, s_pixelTexture, textureDrawMode);
                
                mainTexSlot = 1;
            }
            break;
        case TEX_PRESET_DX_INVERT:
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_ONE_MINUS_SRC_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            if (textureRefID < 0) {
                textureRefID = s_pixelTexture;
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
            } else {
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_ONE_MINUS_SRC_COLOR);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
            }
            break;
        case TEX_PRESET_DX_X4:
            if (textureRefID < 0) {
                textureRefID = s_pixelTexture;
            }
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
            rgbScale = 4;
            break;
        case TEX_PRESET_DX_PMA:
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_ALPHA);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            if (textureRefID < 0) {
                textureRefID = s_pixelTexture;
            } else {
                PL_Render_SetTextureStage(0, s_pixelTexture, textureDrawMode);
                
                mainTexSlot = 1;
            }
            break;
        case TEX_PRESET_DX_PMA_INVERT:
            if (textureRefID < 0) {
                textureRefID = s_pixelTexture;
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PRIMARY_COLOR);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PRIMARY_COLOR);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_ONE_MINUS_SRC_COLOR);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_ALPHA);
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            } else {
                PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
            }
            break;
        case TEX_PRESET_DX_PMA_X4:
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PRIMARY_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_ALPHA);
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            if (textureRefID < 0) {
                textureRefID = s_pixelTexture;
            } else {
                PL_Render_SetTextureStage(0, s_pixelTexture, textureDrawMode);
                
                mainTexSlot = 1;
            }
            rgbScale = 4;
            break;
        default: /* TEX_PRESET_MODULATE */
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            break;
    }
    
    PL_Render_SetTextureStage(mainTexSlot, textureRefID, textureDrawMode);
    if (mainTexSlot != 0) {
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
        PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    }
    
    PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, rgbScale);
    
    return 0;
}
#else
/* #ifdef DXPORTLIB_DRAW_OPENGL_ES2 */
/* OpenGL ES2 Shader mode. */

int PL_Render_SetTextureStage(unsigned int stage, int textureRefID, int textureDrawMode) {
    if (stage >= MAX_TEXTURE) {
        return -1;
    }
    
    PL_GL.glActiveTexture(GL_TEXTURE0 + stage);
    PL_Texture_Bind(textureRefID, textureDrawMode);
    
    s_boundTextures[stage] = textureRefID;
    
    stage += 1;
    if (stage > s_boundTextureCount) {
        s_boundTextureCount = stage;
    }
    return 0;
}

int PL_Render_ClearTextures() {
    unsigned int i;
    for (i = 0; i < s_boundTextureCount; ++i) {
        PL_GL.glActiveTexture(GL_TEXTURE0 + i);
        PL_Texture_Unbind(s_boundTextures[i]);
        s_boundTextures[i] = -1;
    }
    s_boundTextureCount = 0;
    return 0;
}

int PL_Render_ClearTexturePresetMode() {
    PL_Render_ClearTextures();
    
    return 0;
}

int PL_Render_SetTexturePresetMode(int preset,
                                   int textureRefID, int textureDrawMode) {
    return 0;
}

#endif

/* ---------------------------------------------------- VERTEX RENDERING */
static GLenum VertexElementSizeToGL(int value) {
    switch(value) {
        case VERTEXSIZE_UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        default: /* VERTEXSIZE_FLOAT */
            return GL_FLOAT;
    }
}

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
static int PL_Render_ApplyVertexArrayData(const VertexDefinition *def,
                                          const char *vertexData) {
    int i;
    const VertexElement *e = def->elements;
    int elementCount = def->elementCount;
    int vertexDataSize = def->vertexByteSize;
    
    for (i = 0; i < elementCount; ++i, ++e) {
        GLenum vertexType = VertexElementSizeToGL(e->vertexElementSize);
        switch (e->vertexType) {
            case VERTEX_POSITION:
                PL_GL.glEnableClientState(GL_VERTEX_ARRAY);
                PL_GL.glVertexPointer(e->size, vertexType, vertexDataSize, vertexData + e->offset);
                break; 
            case VERTEX_TEXCOORD0:
            case VERTEX_TEXCOORD1:
            case VERTEX_TEXCOORD2:
            case VERTEX_TEXCOORD3:
                PL_GL.glClientActiveTexture(GL_TEXTURE0 - VERTEX_TEXCOORD0 + e->vertexType);
                PL_GL.glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                PL_GL.glTexCoordPointer(e->size, vertexType, vertexDataSize, vertexData + e->offset);
                break;
            case VERTEX_COLOR:
                PL_GL.glEnableClientState(GL_COLOR_ARRAY);
                PL_GL.glColorPointer(e->size, vertexType, vertexDataSize, vertexData + e->offset);
                break;
        }
    }
    return 0;
}

static int PL_Render_ClearVertexArrayData(const VertexDefinition *def) {
    int i;
    const VertexElement *e = def->elements;
    int elementCount = def->elementCount;
    
    for (i = 0; i < elementCount; ++i, ++e) {
        switch (e->vertexType) {
            case VERTEX_POSITION:
                PL_GL.glDisableClientState(GL_VERTEX_ARRAY);
                break; 
            case VERTEX_TEXCOORD0:
            case VERTEX_TEXCOORD1:
            case VERTEX_TEXCOORD2:
            case VERTEX_TEXCOORD3:
                PL_GL.glClientActiveTexture(GL_TEXTURE0 - VERTEX_TEXCOORD0 + e->vertexType);
                PL_GL.glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                break; 
            case VERTEX_COLOR:
                PL_GL.glDisableClientState(GL_COLOR_ARRAY);
                break;
        }
    }
    return 0;
}

static int PL_Render_ApplyVertexBufferData(const VertexDefinition *def) {
    return PL_Render_ApplyVertexArrayData(def, 0);
}

static int PL_Render_ClearVertexBufferData(const VertexDefinition *def) {
    return PL_Render_ClearVertexArrayData(def);
}
#else
/* OpenGL ES2 shader path */
static int PL_Render_ApplyVertexArrayData(const VertexDefinition *def,
                                          const char *vertexData) {
    int i;
    const VertexElement *e = def->elements;
    int elementCount = def->elementCount;
    int vertexDataSize = def->vertexByteSize;
    
    for (i = 0; i < elementCount; ++i, ++e) {
        GLenum vertexType = VertexElementSizeToGL(e->vertexElementSize);
        switch (e->vertexType) {
            case VERTEX_POSITION:
                break; 
            case VERTEX_TEXCOORD0:
            case VERTEX_TEXCOORD1:
            case VERTEX_TEXCOORD2:
            case VERTEX_TEXCOORD3:
                break;
            case VERTEX_COLOR:
                break;
        }
    }
    return 0;
}

static int PL_Render_ClearVertexArrayData(const VertexDefinition *def) {
    int i;
    const VertexElement *e = def->elements;
    int elementCount = def->elementCount;
    
    for (i = 0; i < elementCount; ++i, ++e) {
        switch (e->vertexType) {
            case VERTEX_POSITION:
                break; 
            case VERTEX_TEXCOORD0:
            case VERTEX_TEXCOORD1:
            case VERTEX_TEXCOORD2:
            case VERTEX_TEXCOORD3:
                break; 
            case VERTEX_COLOR:
                break;
        }
    }
    return 0;
}

static int PL_Render_ApplyVertexBufferData(const VertexDefinition *def) {
    return PL_Render_ApplyVertexArrayData(def, 0);
}

static int PL_Render_ClearVertexBufferData(const VertexDefinition *def) {
    return PL_Render_ClearVertexArrayData(def);
}

#endif

/* ----------------------------------------------- RENDERING VERTEX DATA */

static GLuint PrimitiveToDrawType(int primitiveType) {
    switch(primitiveType) {
        case PL_PRIM_POINTS:
            return GL_POINTS;
        case PL_PRIM_LINES:
            return GL_LINES;
        case PL_PRIM_TRIANGLEFAN:
            return GL_TRIANGLE_FAN;
        default: /* case PL_PRIM_TRIANGLES: */
            return GL_TRIANGLES;
    }
}

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
int PL_Render_DrawVertexArray(const VertexDefinition *def,
                              const char *vertexData,
                              int primitiveType, int vertexStart, int vertexCount
                             ) {
    PL_Render_UpdateMatrices();
    
    if (PL_GL.hasVBOSupport == DXTRUE) {
        if (PL_GL.useVBOARB == DXTRUE) {
            PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        } else {
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    
    PL_Render_ApplyVertexArrayData(def, vertexData);
    
    PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
    
    PL_Render_ClearVertexArrayData(def);
    
    return 0;
}

int PL_Render_DrawVertexIndexArray(const VertexDefinition *def,
                                   const char *vertexData,
                                   const unsigned short *indexData,
                                   int primitiveType, int indexStart, int indexCount
                             ) {
    PL_Render_UpdateMatrices();
    
    if (PL_GL.hasVBOSupport == DXTRUE) {
        if (PL_GL.useVBOARB == DXTRUE) {
            PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        } else {
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
            PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    
    PL_Render_ApplyVertexArrayData(def, vertexData);
    
    PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                         indexCount, GL_UNSIGNED_SHORT, indexData + indexStart);
    
    PL_Render_ClearVertexArrayData(def);
    
    return 0;
}
#else
int PL_Render_DrawVertexArray(const VertexDefinition *def,
                              const char *vertexData,
                              int primitiveType, int vertexStart, int vertexCount
                             ) {
    return 0;
}

int PL_Render_DrawVertexIndexArray(const VertexDefinition *def,
                                   const char *vertexData,
                                   const unsigned short *indexData,
                                   int primitiveType, int indexStart, int indexCount
                             ) {
    return 0;
}
#endif

int PL_Render_DrawVertexBuffer(const VertexDefinition *def,
                               int vertexBufferHandle,
                               int primitiveType, int vertexStart, int vertexCount
                               ) {
    GLuint vertexBufferID;
    
    if (PL_GL.hasVBOSupport == DXFALSE) {
        return 0;
    }
    
    vertexBufferID = PL_VertexBuffer_GetGLID(vertexBufferHandle);
    
    PL_Render_UpdateMatrices();
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.useVBOARB == DXTRUE) {
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBufferID);
    } else
#endif
    {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    }
    
    PL_Render_ApplyVertexBufferData(def);
    
    PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
    
    PL_Render_ClearVertexBufferData(def);
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.useVBOARB == DXTRUE) {
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBufferID);
    } else
#endif
    {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    return 0;
}

int PL_Render_DrawVertexIndexBuffer(const VertexDefinition *def,
                                    int vertexBufferHandle, int indexBufferHandle,
                                    int primitiveType, int indexStart, int indexCount
                                    ) {
    GLuint vertexBufferID, indexBufferID;
    
    if (PL_GL.hasVBOSupport == DXFALSE) {
        return 0;
    }
    
    vertexBufferID = PL_VertexBuffer_GetGLID(vertexBufferHandle);
    indexBufferID = PL_IndexBuffer_GetGLID(indexBufferHandle);
    
    PL_Render_UpdateMatrices();
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.useVBOARB == DXTRUE) {
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBufferID);
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufferID);
    } else
#endif
    {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    }
    
    PL_Render_ApplyVertexBufferData(def);
    
    PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                         indexCount, GL_UNSIGNED_SHORT,
                         (void *)(indexStart * sizeof(unsigned short)));
    
    PL_Render_ClearVertexBufferData(def);
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.useVBOARB == DXTRUE) {
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    } else
#endif
    {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    return 0;
}

/* ------------------------------------------------- INIT/FINISH/GENERAL */

int PL_Render_ClearColor(float r, float g, float b, float a) {
    PL_GL.glClearColor(r, g, b, a);
    return 0;
}

int PL_Render_Clear() {
    PL_GL.glClear(GL_COLOR_BUFFER_BIT);
    
    return 0;
}

int PL_Render_StartFrame() {
    PL_Render_ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    PL_Render_Clear();
    
    s_matrixDirty = DXTRUE;
    
    return 0;
}
int PL_Render_EndFrame() {
    return 0;
}

int PL_Render_Init() {
    /* Create a texture that's a single white pixel. */
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 1, 32,
                                   0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    *((unsigned int *)(surface->pixels)) = 0xffffffff;
    s_pixelTexture = PL_Texture_CreateFromSDLSurface(surface, DXFALSE);
    SDL_FreeSurface(surface);
    
    PL_Shaders_Init();
    
    return 0;
}

int PL_Render_End() {
    if (s_pixelTexture >= 0) {
        PL_Texture_Release(s_pixelTexture);
        s_pixelTexture = -1;
    }
    
    PL_Shaders_Cleanup();
    
    return 0;
}

#endif
