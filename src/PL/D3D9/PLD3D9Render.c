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

/* D3D9 code is very generic. Instead of doing any drawing itself, it has two
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

#ifdef DXPORTLIB_DRAW_DIRECT3D9

#include "PL/PLInternal.h"

#include "PLD3D9Internal.h"

#include "SDL.h"

/* ----------------------------------------------------- VIEW INFOMATION */
int PLD3D9_SetViewport(int x, int y, int w, int h) {
    return 0;
}

int PLD3D9_SetZRange(float nearZ, float farZ) {
    
    return 0;
}

/* ------------------------------------------------------------ BLENDING */

void PLD3D9_SetBlendMode(
    int blendEquation,
    int srcBlend, int destBlend
) {
}
void PLD3D9_SetBlendModeSeparate(
    int blendEquation,
    int srcRGBBlend, int destRGBBlend,
    int srcAlphaBlend, int destAlphaBlend
) {
}
void PLD3D9_DisableBlend() {
}

/* ----------------------------------------------------- SCISSOR/CULLING */

int PLD3D9_SetScissor(int x, int y, int w, int h) {
    return 0;
}

int PLD3D9_DisableScissor() {
    return 0;
}

int PLD3D9_SetScissorRect(const RECT *rect) {
    if (rect == NULL) {
        return PLD3D9_DisableScissor();
    } else {
        return PLD3D9_SetScissor(rect->left, rect->top,
                   rect->right - rect->left, rect->bottom - rect->top);
    }
}

int PLD3D9_DisableCulling() {
    return 0;
}

int PLD3D9_DisableDepthTest() {
    return 0;
}

/* ---------------------------------------------------- TEXTURE PROGRAMS */

int PLD3D9_SetTextureStage(unsigned int stage, int textureRefID, int textureDrawMode) {
    return 0;
}

int PLD3D9_ClearTextures() {
    return 0;
}

static int s_stockShaderPresetLookupNotex[TEX_PRESET_END] = {
    PLD3D9_SHADER_BASIC_COLOR_NOTEX,
    PLD3D9_SHADER_DX_MULA_COLOR_NOTEX,
    PLD3D9_SHADER_DX_INVERT_COLOR_NOTEX,
    PLD3D9_SHADER_DX_X4_COLOR_NOTEX,
    PLD3D9_SHADER_DX_PMA_COLOR_NOTEX,
    PLD3D9_SHADER_DX_PMA_INVERT_COLOR_NOTEX,
    PLD3D9_SHADER_DX_PMA_X4_COLOR_NOTEX
};
static int s_stockShaderPresetLookupTex1[TEX_PRESET_END] = {
    PLD3D9_SHADER_BASIC_COLOR_TEX1,
    PLD3D9_SHADER_DX_MULA_COLOR_TEX1,
    PLD3D9_SHADER_DX_INVERT_COLOR_TEX1,
    PLD3D9_SHADER_DX_X4_COLOR_TEX1,
    PLD3D9_SHADER_DX_PMA_COLOR_TEX1,
    PLD3D9_SHADER_DX_PMA_INVERT_COLOR_TEX1,
    PLD3D9_SHADER_DX_PMA_X4_COLOR_TEX1
};

int PLD3D9_SetPresetProgram(int preset, int flags,
                          const PLMatrix *projectionMatrix, const PLMatrix *viewMatrix,
                          int textureRefID, int textureDrawMode,
                          float alphaTestValue) {
}

int PLD3D9_ClearPresetProgram() {
    return 0;
}

/* ----------------------------------------------- RENDERING VERTEX DATA */

int PLD3D9_DrawVertexArray(const VertexDefinition *def,
                              const char *vertexData,
                              int primitiveType, int vertexStart, int vertexCount
                             ) {
    return 0;
}

int PLD3D9_DrawVertexIndexArray(const VertexDefinition *def,
                                   const char *vertexData,
                                   int vertexStart, int vertexCount,
                                   const unsigned short *indexData,
                                   int primitiveType, int indexStart, int indexCount
                             ) {
    return 0;
}

int PLD3D9_DrawVertexBuffer(const VertexDefinition *def,
                               int vertexBufferHandle,
                               int primitiveType, int vertexStart, int vertexCount
                               ) {
    return 0;
}

int PLD3D9_DrawVertexIndexBuffer(const VertexDefinition *def,
                                    int vertexBufferHandle,
                                    int vertexStart, int vertexCount,
                                    int indexBufferHandle,
                                    int primitiveType, int indexStart, int indexCount
                                    ) {
    return 0;
}

/* ------------------------------------------------- INIT/FINISH/GENERAL */

int PLD3D9_ClearColor(float r, float g, float b, float a) {
    return 0;
}

int PLD3D9_Clear() {
    return 0;
}

int PLD3D9_StartFrame() {
    return 0;
}
int PLD3D9_EndFrame() {
    return 0;
}

int PLD3D9_Render_Init() {
#ifndef DXPORTLIB_DRAW_OPEND3D9_ES2
    PLD3D9_FixedFunction_Init();
#endif
    PLD3D9_Shaders_Init();
    
    /* Set a default so it won't crash. */
    PLD3D9_SetPresetProgram(PLD3D9_SHADER_BASIC_COLOR_NOTEX, 0,
                          &g_matrixIdentity, &g_matrixIdentity,
                          -1, 0, 0);
    
    return 0;
}

int PLD3D9_Render_End() {
    PLD3D9_Shaders_Cleanup();
#ifndef DXPORTLIB_DRAW_OPEND3D9_ES2
    PLD3D9_FixedFunction_Cleanup();
#endif
    
    return 0;
}

#endif
