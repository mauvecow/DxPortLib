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

#include "DPLBuildConfig.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "PL/PLInternal.h"

#include "PLGLInternal.h"

#include "SDL.h"

/* ----------------------------------------------------- VIEW INFORMATION */
int PLGL_SetViewport(int x, int y, int w, int h) {
    PL_GL.glViewport(x, y, w, h);
    
    return 0;
}

int PLGL_SetZRange(float nearZ, float farZ) {
    if (PL_GL.glDepthRangef != 0) {
        PL_GL.glDepthRangef(nearZ, farZ);
    } else if (PL_GL.glDepthRange != 0) {
        PL_GL.glDepthRange(nearZ, farZ);
    }
    
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

void PLGL_SetBlendMode(
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
void PLGL_SetBlendModeSeparate(
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
void PLGL_DisableBlend() {
    PL_GL.glDisable(GL_BLEND);
}

/* ----------------------------------------------------- SCISSOR/CULLING */

int PLGL_SetScissor(int x, int y, int w, int h) {
    PL_GL.glEnable(GL_SCISSOR_TEST);
    PL_GL.glScissor(x, y, w, h);
    return 0;
}

int PLGL_DisableScissor() {
    PL_GL.glDisable(GL_SCISSOR_TEST);
    return 0;
}

int PLGL_SetScissorRect(const RECT *rect) {
    if (rect == NULL) {
        return PLGL_DisableScissor();
    } else {
        return PLGL_SetScissor(rect->left, rect->top,
                   rect->right - rect->left, rect->bottom - rect->top);
    }
}

int PLGL_DisableCulling() {
    PL_GL.glDisable(GL_CULL_FACE);
    return 0;
}

int PLGL_SetDepthFunc(PLDepthFunc depthFunc) {
    GLenum func;
    switch(depthFunc) {
        case PL_DEPTHFUNC_NONE: func = GL_NONE; break;
        case PL_DEPTHFUNC_LESS: func = GL_LESS; break;
        case PL_DEPTHFUNC_LEQUAL: func = GL_LEQUAL; break;
        case PL_DEPTHFUNC_EQUAL: func = GL_EQUAL; break;
        case PL_DEPTHFUNC_GEQUAL: func = GL_GEQUAL; break;
        case PL_DEPTHFUNC_GREATER: func = GL_GREATER; break;
        case PL_DEPTHFUNC_NOTEQUAL: func = GL_NOTEQUAL; break;
        default: return -1;
    }
    
    PL_GL.glDepthFunc(func);
    
    return 0;
}

int PLGL_EnableDepthTest() {
    PL_GL.glEnable(GL_DEPTH_TEST);
    return 0;
}

int PLGL_DisableDepthTest() {
    PL_GL.glDisable(GL_DEPTH_TEST);
    return 0;
}

int PLGL_EnableDepthWrite() {
    PL_GL.glDepthMask(GL_TRUE);
    return 0;
}
int PLGL_DisableDepthWrite() {
    PL_GL.glDepthMask(GL_FALSE);
    return 0;
}

/* ---------------------------------------------------- TEXTURE PROGRAMS */
#define MAX_TEXTURE 4
/* #ifdef DXPORTLIB_DRAW_OPENGL_ES2 */
/* OpenGL ES2 Shader mode. */
static int s_boundTextures[MAX_TEXTURE] = { -1 };
static unsigned int s_boundTextureCount = 0;

static int s_activeTexturePreset = -1;
static int s_activeShaderProgram = -100;
static int s_useFixedFunction = 0;

int PLGL_SetTextureStage(unsigned int stage, int textureRefID, int textureDrawMode) {
    if (stage >= MAX_TEXTURE) {
        return -1;
    }
    
    PL_GL.glActiveTexture(GL_TEXTURE0 + stage);
    PLGL_Texture_Bind(textureRefID, textureDrawMode);
    
    s_boundTextures[stage] = textureRefID;
    
    stage += 1;
    if (stage > s_boundTextureCount) {
        s_boundTextureCount = stage;
    }
    return 0;
}

int PLGL_ClearTextures() {
    unsigned int i;
    for (i = 0; i < s_boundTextureCount; ++i) {
        PL_GL.glActiveTexture(GL_TEXTURE0 + i);
        PLGL_Texture_Unbind(s_boundTextures[i]);
        s_boundTextures[i] = -1;
    }
    s_boundTextureCount = 0;
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

int PLGL_SetPresetProgram(int preset, int flags,
                          const PLMatrix *projectionMatrix, const PLMatrix *viewMatrix,
                          int textureRefID, int textureDrawMode,
                          float alphaTestValue) {
    int presetID;
    int newShaderProgram;
    
    s_activeTexturePreset = preset;
    
    if (textureRefID <= 0) {
        presetID = s_stockShaderPresetLookupNotex[preset];
    } else {
        presetID = s_stockShaderPresetLookupTex1[preset];
    }
    
    newShaderProgram = PLGL_Shaders_GetStockProgramForID(presetID);
    PLGL_Shaders_UseProgram(newShaderProgram);
    
    s_activeShaderProgram = newShaderProgram;
    if (newShaderProgram > 0) {
        s_useFixedFunction = DXFALSE;
        PLGL_SetTextureStage(0, textureRefID, textureDrawMode);
        
        PLGL_Shaders_ApplyProgramMatrices(
            newShaderProgram,
            projectionMatrix, viewMatrix);
        
        return 0;
    } else {
        /* Fixed function fallback */
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
        s_useFixedFunction = DXTRUE;
        return PLGL_FixedFunction_SetPresetProgram(preset, flags,
                                                   projectionMatrix, viewMatrix,
                                                   textureRefID, textureDrawMode,
                                                   alphaTestValue);
#else
        return -1;
#endif
    }
}

int PLGL_ClearPresetProgram() {
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        PLGL_FixedFunction_ClearPresetProgram();
    }
#endif
    
    PLGL_ClearTextures();
    
    return 0;
}

/* -------------------------------------------------- FULL ES2 EMULATION */
/* OpenGL, as of 3.0 core, requires a bound vbo/ibo for calls to
 * glDrawArrays/glDrawElements.
 */
#define MAX_EMULATED_BUFFERS    24
static int s_emulatedVBOs[MAX_EMULATED_BUFFERS];
static int s_emulatedIBOs[MAX_EMULATED_BUFFERS];

/* Returns valid VBO/IBOs for the given inputs. */
static int s_emulateVertexBuffer(const VertexDefinition *def,
                                 const char *vertexData,
                                 int vertexStart, int vertexCount) {
    int n = 5;
    int size = 1 << n;
    int vertexByteSize = def->vertexByteSize;
    int targetSize = (vertexStart + vertexCount) * vertexByteSize;
    int vboID;
    
    while (size < targetSize && n < MAX_EMULATED_BUFFERS) { size <<= 1; n += 1; }
    if (n >= MAX_EMULATED_BUFFERS) {
        return -1;
    }
    
    vboID = s_emulatedVBOs[n];
    if (vboID <= 0) {
        vboID = PLGL_VertexBuffer_CreateBytes(1, 0, size, DXFALSE);
        s_emulatedVBOs[n] = vboID;
    }

    PLGL_VertexBuffer_SetDataBytes(vboID,
                                 vertexData,
                                 vertexStart * vertexByteSize,
                                 vertexCount * vertexByteSize,
                                 DXTRUE);
    
    return vboID;
}
static int s_emulateIndexBuffer(const unsigned short *indexData,
                                int indexStart, int indexCount) {
    int n = 5;
    int size = 1 << n;
    int targetSize = (indexStart + indexCount);
    int iboID;
    
    while (size < targetSize && n < MAX_EMULATED_BUFFERS) { size <<= 1; n += 1; }
    if (n >= MAX_EMULATED_BUFFERS) {
        return -1;
    }
    
    iboID = s_emulatedIBOs[n];
    if (iboID <= 0) {
        iboID = PLGL_IndexBuffer_Create(0, size, DXFALSE);
        s_emulatedIBOs[n] = iboID;
    }
    
    PLGL_IndexBuffer_ResetBuffer(iboID);
    PLGL_IndexBuffer_SetData(iboID, indexData, indexStart, indexCount, DXTRUE);
    
    return iboID;
}

static int s_emulateBuffersInit() {
    int i;
    for (i = 0; i < MAX_EMULATED_BUFFERS; ++i) {
        s_emulatedVBOs[i] = -1;
        s_emulatedIBOs[i] = -1;
    }
    return 0;
}
static int s_emulateBuffersCleanup() {
    int i;
    for (i = 0; i < MAX_EMULATED_BUFFERS; ++i) {
        PLGL_VertexBuffer_Delete(s_emulatedVBOs[i]);
        s_emulatedVBOs[i] = -1;
        
        PLGL_IndexBuffer_Delete(s_emulatedIBOs[i]);
        s_emulatedIBOs[i] = -1;
    }
    return 0;
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

int PLGL_DrawVertexArray(const VertexDefinition *def,
                              const char *vertexData,
                              int primitiveType, int vertexStart, int vertexCount
                             ) {
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.hasVBOSupport == DXTRUE) {
#endif
        return PLGL_DrawVertexBuffer(
                def,
                s_emulateVertexBuffer(def, vertexData + (vertexStart * def->vertexByteSize),
                                      0, vertexCount),
                primitiveType, 0, vertexCount);
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    }

    if (PL_GL.hasVBOSupport == DXTRUE) {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    if (s_useFixedFunction == DXTRUE) {
        PLGL_FixedFunction_ApplyVertexArrayData(def, vertexData);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
        
        PLGL_FixedFunction_ClearVertexArrayData(def);
    } else {
        /* TESTME This might be invalid on some drivers, but works okay on nvidia. */
        PLGL_Shaders_ApplyProgramVertexData(
            s_activeShaderProgram,
            vertexData, def);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
        
        PLGL_Shaders_ClearProgramVertexData(
            s_activeShaderProgram, def);
    }
    
    return 0;
#endif
}

int PLGL_DrawVertexIndexArray(const VertexDefinition *def,
                                   const char *vertexData,
                                   int vertexStart, int vertexCount,
                                   const unsigned short *indexData,
                                   int primitiveType, int indexStart, int indexCount
                             ) {
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.hasVBOSupport == DXTRUE) {
#endif
        PLGL_DrawVertexIndexBuffer(
            def,
            s_emulateVertexBuffer(def, vertexData, vertexStart, vertexCount), vertexStart, vertexCount,
            s_emulateIndexBuffer(indexData + indexStart, 0, indexCount),
            primitiveType, 0, indexCount);
        
        return 0;
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    }
    
    if (PL_GL.hasVBOSupport == DXTRUE) {
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    if (s_useFixedFunction == DXTRUE) {
        PLGL_FixedFunction_ApplyVertexArrayData(def, vertexData);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT, indexData + indexStart);
        
        PLGL_FixedFunction_ClearVertexArrayData(def);
    } else {
        /* TESTME This might be invalid on some drivers, but works okay on nvidia. */
        PLGL_Shaders_ApplyProgramVertexData(
            s_activeShaderProgram,
            vertexData, def);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT,
                            (void *)(indexStart * sizeof(unsigned short)));
        
        PLGL_Shaders_ClearProgramVertexData(
            s_activeShaderProgram, def);
    }
    
    return 0;
#endif
}

int PLGL_DrawVertexBuffer(const VertexDefinition *def,
                               int vertexBufferHandle,
                               int primitiveType, int vertexStart, int vertexCount
                               ) {
    GLuint vertexBufferID;
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.hasVBOSupport == DXFALSE) {
        return PLGL_DrawVertexArray(
                    def, PLGL_VertexBuffer_GetFallback(vertexBufferHandle),
                    primitiveType, vertexStart, vertexCount);
    }
#endif
    
    vertexBufferID = PLGL_VertexBuffer_GetGLID(vertexBufferHandle);
    
    PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        PLGL_FixedFunction_ApplyVertexBufferData(def);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
        
        PLGL_FixedFunction_ClearVertexBufferData(def);
    } else
#endif
    {
        PLGL_Shaders_ApplyProgramVertexData(
            s_activeShaderProgram,
            0, def);
        
        PL_GL.glDrawArrays(PrimitiveToDrawType(primitiveType), vertexStart, vertexCount);
    }
    
    return 0;
}

int PLGL_DrawVertexIndexBuffer(const VertexDefinition *def,
                                    int vertexBufferHandle,
                                    int vertexStart, int vertexCount,
                                    int indexBufferHandle,
                                    int primitiveType, int indexStart, int indexCount
                                    ) {
    GLuint vertexBufferID, indexBufferID;
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.hasVBOSupport == DXFALSE) {
        /* Incredibly slow in some situations. */
        return PLGL_DrawVertexIndexArray(
                    def,
                    PLGL_VertexBuffer_GetFallback(vertexBufferHandle), vertexStart, vertexCount,
                    (const unsigned short *)PLGL_IndexBuffer_GetFallback(indexBufferHandle),
                    primitiveType, indexStart, indexCount);
    }
#endif
    
    vertexBufferID = PLGL_VertexBuffer_GetGLID(vertexBufferHandle);
    indexBufferID = PLGL_IndexBuffer_GetGLID(indexBufferHandle);
    
    PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (s_useFixedFunction == DXTRUE) {
        PLGL_FixedFunction_ApplyVertexBufferData(def);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT,
                            (void *)(indexStart * sizeof(unsigned short)));
        
        PLGL_FixedFunction_ClearVertexBufferData(def);
    } else
#endif
    {
        PLGL_Shaders_ApplyProgramVertexData(
            s_activeShaderProgram,
            0, def);
        
        PL_GL.glDrawElements(PrimitiveToDrawType(primitiveType),
                            indexCount, GL_UNSIGNED_SHORT,
                            (void *)(indexStart * sizeof(unsigned short)));
    }
    
    return 0;
}

/* ------------------------------------------------- INIT/FINISH/GENERAL */

int PLGL_ClearDepth(float d) {
    PL_GL.glClearDepth(d);
    return 0;
}

int PLGL_ClearColor(float r, float g, float b, float a) {
    PL_GL.glClearColor(r, g, b, a);
    return 0;
}

int PLGL_Clear(PLClearType clearType) {
    GLbitfield flags = 0;
    if ((clearType & PL_CLEAR_COLOR) != 0) {
        flags |= GL_COLOR_BUFFER_BIT;
    }
    if ((clearType & PL_CLEAR_DEPTH) != 0) {
        flags |= GL_DEPTH_BUFFER_BIT;
    }
    
    PL_GL.glClear(flags);
    PL_GL.glDepthFunc(GL_LEQUAL);
    
    return 0;
}

int PLGL_StartFrame() {
    return 0;
}
int PLGL_EndFrame() {
    return 0;
}

int PLGL_Finish() {
    PL_GL.glFinish();
    
    return 0;
}

int PLGL_Render_Init() {
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PLGL_FixedFunction_Init();
#endif
    PLGL_Shaders_Init();
    
    /* Set a default so it won't crash. */
    PLGL_SetPresetProgram(PLGL_SHADER_BASIC_COLOR_NOTEX, 0,
                          &g_matrixIdentity, &g_matrixIdentity,
                          -1, 0, 0);
    
    s_emulateBuffersInit();
    
    return 0;
}

int PLGL_Render_End() {
    s_emulateBuffersCleanup();
    
    PLGL_Shaders_Cleanup();
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PLGL_FixedFunction_Cleanup();
#endif
    
    return 0;
}

#endif
