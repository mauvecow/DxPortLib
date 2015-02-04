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
    
    /* FIXME Untransformed vertices should be handled on the front-end, not here. */
    if (s_displayUntransformed == DXTRUE) {
        PL_Matrix_CreateOrthoOffCenterLH(&s_currentMatrixProjection,
                      (float)s_viewportX, (float)(s_viewportX + s_viewportW),
                      (float)(s_viewportY + s_viewportH), (float)s_viewportY, 
                      s_nearZ, s_farZ);
        PL_Matrix_CreateTranslation(&s_currentMatrixView, 0.5f, 0.5f, 0.0f);
    } else {
        PL_Matrix_Copy(&s_currentMatrixProjection, &s_matrixProjection);
        PL_Matrix_Copy(&s_currentMatrixView, &s_matrixView);
    }
    
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
    if (blendEquation == PL_BLENDFUNC_DISABLE) {
        PL_GL.glDisable(GL_BLEND);
        return;
    }
    PL_GL.glBlendFunc(BlendTypeToGL(srcBlend), BlendTypeToGL(destBlend));
    PL_GL.glBlendEquation(BlendFuncToGL(blendEquation));
    PL_GL.glEnable(GL_BLEND);
}
void PL_Render_SetBlendModeSeparate(
    int blendEquation,
    int srcRGBBlend, int destRGBBlend,
    int srcAlphaBlend, int destAlphaBlend
) {
    if (blendEquation == PL_BLENDFUNC_DISABLE) {
        PL_GL.glDisable(GL_BLEND);
        return;
    }
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
static int s_alphaTestEnable = DXFALSE;

int PL_Render_EnableAlphaTest() {
    s_alphaTestEnable = DXTRUE;
    return 0;
}
int PL_Render_DisableAlphaTest() {
    s_alphaTestEnable = DXFALSE;
    return 0;
}

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

int PL_Render_DisableCulling() {
    PL_GL.glDisable(GL_CULL_FACE);
    return 0;
}

int PL_Render_DisableDepthTest() {
    PL_GL.glDisable(GL_DEPTH_TEST);
    return 0;
}

/* ---------------------------------------------------- TEXTURE PROGRAMS */
#define MAX_TEXTURE 4
/* #ifdef DXPORTLIB_DRAW_OPENGL_ES2 */
/* OpenGL ES2 Shader mode. */
static int s_boundTextures[MAX_TEXTURE] = { -1 };
static unsigned int s_boundTextureCount = 0;

static int s_activeTexturePreset = -1;
static int s_activeShaderProgram = 0;
static int s_useFixedFunction = 0;

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
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        PL_GLFixedFunction_ClearTexturePresetMode();
    }
#endif
    PL_Render_ClearTextures();
    
    return 0;
}

static int s_stockShaderPresetLookupNotex[TEX_PRESET_END] = {
    PLGL_SHADER_BASIC_COLOR_NOTEX,
    PLGL_SHADER_DX_MULA_COLOR_NOTEX,
    PLGL_SHADER_DX_INVERT_COLOR_NOTEX,
    PLGL_SHADER_DX_X4_COLOR_NOTEX,
    PLGL_SHADER_DX_PMA_COLOR_NOTEX,
    PLGL_SHADER_DX_PMA_INVERT_COLOR_NOTEX,
    PLGL_SHADER_DX_PMA_X4_COLOR_NOTEX
};
static int s_stockShaderPresetLookupTex1[TEX_PRESET_END] = {
    PLGL_SHADER_BASIC_COLOR_TEX1,
    PLGL_SHADER_DX_MULA_COLOR_TEX1,
    PLGL_SHADER_DX_INVERT_COLOR_TEX1,
    PLGL_SHADER_DX_X4_COLOR_TEX1,
    PLGL_SHADER_DX_PMA_COLOR_TEX1,
    PLGL_SHADER_DX_PMA_INVERT_COLOR_TEX1,
    PLGL_SHADER_DX_PMA_X4_COLOR_TEX1
};

int PL_Render_SetTexturePresetMode(int preset,
                                   int textureRefID, int textureDrawMode) {
    int presetID;
    
    s_activeTexturePreset = preset;
    
    if (textureRefID <= 0) {
        presetID = s_stockShaderPresetLookupNotex[preset];
    } else {
        presetID = s_stockShaderPresetLookupTex1[preset];
    }
    
    s_activeShaderProgram = PL_Shaders_GetStockProgramForID(presetID);
    if (s_activeShaderProgram >= 0) {
        s_useFixedFunction = DXFALSE;
        PL_Render_SetTextureStage(0, textureRefID, textureDrawMode);
        return 0;
    } else {
        /* Fixed function fallback */
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
        s_useFixedFunction = DXTRUE;
        return PL_GLFixedFunction_SetTexturePresetMode(preset, textureRefID, textureDrawMode);
#else
        return -1;
#endif
    }
}

/* ----------------------------------------------- RENDERING VERTEX DATA */

static GLuint PrimitiveToDrawType(int primitiveType) {
    switch(primitiveType) {
        case PL_PRIM_POINTS:
            return GL_POINTS;
        case PL_PRIM_LINES:
            return GL_LINES;
        case PL_PRIM_TRIANGLEFAN:
            return GL_TRIANGLE_FAN;
        case PL_PRIM_TRIANGLESTRIP:
            return GL_TRIANGLE_STRIP;
        default: /* case PL_PRIM_TRIANGLES: */
            return GL_TRIANGLES;
    }
}

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
static void s_InitFixedFunctionState() {
    PL_GL.glMatrixMode(GL_PROJECTION);
    PL_GL.glLoadMatrixf((const float *)&s_currentMatrixProjection);
    
    PL_GL.glMatrixMode(GL_MODELVIEW);
    PL_GL.glLoadMatrixf((const float *)&s_currentMatrixView);

    if (s_alphaTestEnable) {
        PL_GL.glEnable(GL_ALPHA_TEST);
        PL_GL.glAlphaFunc(GL_GREATER, 0);
    } else {
        PL_GL.glDisable(GL_ALPHA_TEST);
    }
}
static void s_ClearFixedFunctionState() {
}
#endif

int PL_Render_DrawVertexArray(const VertexDefinition *def,
                              const char *vertexData,
                              int primitiveType, int vertexStart, int vertexCount
                             ) {
    PL_Render_UpdateMatrices();
    
    if (PL_GL.hasVBOSupport == DXTRUE) {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        s_InitFixedFunctionState();
        
        PL_GLFixedFunction_ApplyVertexArrayData(def, vertexData);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
        
        PL_GLFixedFunction_ClearVertexArrayData(def);
        
        s_ClearFixedFunctionState();
    } else
#endif
    {
        PL_Shaders_ApplyProgram(
            s_activeShaderProgram,
            &s_currentMatrixProjection, &s_currentMatrixView,
            vertexData, def);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
        
        PL_Shaders_ClearProgram(
            s_activeShaderProgram,
            def);
    }
    
    return 0;
}

int PL_Render_DrawVertexIndexArray(const VertexDefinition *def,
                                   const char *vertexData,
                                   const unsigned short *indexData,
                                   int primitiveType, int indexStart, int indexCount
                             ) {
    PL_Render_UpdateMatrices();
    
    if (PL_GL.hasVBOSupport == DXTRUE) {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        s_InitFixedFunctionState();
        
        PL_GLFixedFunction_ApplyVertexArrayData(def, vertexData);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT, indexData + indexStart);
        
        PL_GLFixedFunction_ClearVertexArrayData(def);
        
        s_ClearFixedFunctionState();
    } else
#endif
    {
        PL_Shaders_ApplyProgram(
            s_activeShaderProgram,
            &s_currentMatrixProjection, &s_currentMatrixView,
            vertexData, def);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT, indexData + indexStart);
        
        PL_Shaders_ClearProgram(
            s_activeShaderProgram,
            def);
    }
    
    return 0;
}

int PL_Render_DrawVertexBuffer(const VertexDefinition *def,
                               int vertexBufferHandle,
                               int primitiveType, int vertexStart, int vertexCount
                               ) {
    GLuint vertexBufferID;
    
    if (PL_GL.hasVBOSupport == DXFALSE) {
        return PL_Render_DrawVertexArray(
                    def, PL_VertexBuffer_GetFallback(vertexBufferHandle),
                    primitiveType, vertexStart, vertexCount);
    }
    
    vertexBufferID = PL_VertexBuffer_GetGLID(vertexBufferHandle);
    
    PL_Render_UpdateMatrices();
    
    PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        s_InitFixedFunctionState();
        
        PL_GLFixedFunction_ApplyVertexBufferData(def);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
        
        PL_GLFixedFunction_ClearVertexBufferData(def);
        
        s_ClearFixedFunctionState();
    } else
#endif
    {
        PL_Shaders_ApplyProgram(
            s_activeShaderProgram,
            &s_currentMatrixProjection, &s_currentMatrixView,
            0, def);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
        
        PL_Shaders_ClearProgram(
            s_activeShaderProgram,
            def);
    }
    
    PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return 0;
}

int PL_Render_DrawVertexIndexBuffer(const VertexDefinition *def,
                                    int vertexBufferHandle, int indexBufferHandle,
                                    int primitiveType, int indexStart, int indexCount
                                    ) {
    GLuint vertexBufferID, indexBufferID;
    
    if (PL_GL.hasVBOSupport == DXFALSE) {
        /* FIXME this breaks if the index buffer is not 16 bit shorts */
        return PL_Render_DrawVertexIndexArray(
                    def, PL_VertexBuffer_GetFallback(vertexBufferHandle),
                    (const unsigned short *)PL_IndexBuffer_GetFallback(indexBufferHandle),
                    primitiveType, indexStart, indexCount);
    }
    
    vertexBufferID = PL_VertexBuffer_GetGLID(vertexBufferHandle);
    indexBufferID = PL_IndexBuffer_GetGLID(indexBufferHandle);
    
    PL_Render_UpdateMatrices();
    
    PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        s_InitFixedFunctionState();
        
        PL_GLFixedFunction_ApplyVertexBufferData(def);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT,
                            (void *)(indexStart * sizeof(unsigned short)));
        
        PL_GLFixedFunction_ClearVertexBufferData(def);
        
        s_ClearFixedFunctionState();
    } else
#endif
    {
        PL_Shaders_ApplyProgram(
            s_activeShaderProgram,
            &s_currentMatrixProjection, &s_currentMatrixView,
            0, def);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT,
                            (void *)(indexStart * sizeof(unsigned short)));
        
        PL_Shaders_ClearProgram(
            s_activeShaderProgram,
            def);
    }
    PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
    PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
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
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GLFixedFunction_Init();
#endif
    PL_Shaders_Init();
    
    PL_Render_SetTexturePresetMode(PLGL_SHADER_BASIC_COLOR_NOTEX, -1, 0);
    
    return 0;
}

int PL_Render_End() {
    PL_Shaders_Cleanup();
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GLFixedFunction_Cleanup();
#endif
    
    return 0;
}

#endif
