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

#ifndef DXPORTLIB_OPEND3D9_DXINTERNAL_H_HEADER
#define DXPORTLIB_OPEND3D9_DXINTERNAL_H_HEADER

#include "DPLBuildConfig.h"

#ifdef DXPORTLIB_DRAW_DIRECT3D9

#include "PL/PLInternal.h"

#include "SDL.h"

/* In the event this is not defined... */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

typedef enum {
    PLD3D9_SHADER_BASIC_NOCOLOR_TEX1 = 0,
    PLD3D9_SHADER_BASIC_COLOR_NOTEX,
    PLD3D9_SHADER_BASIC_COLOR_TEX1,
    PLD3D9_SHADER_DX_INVERT_COLOR_NOTEX,
    PLD3D9_SHADER_DX_INVERT_COLOR_TEX1,
    PLD3D9_SHADER_DX_MULA_COLOR_NOTEX,
    PLD3D9_SHADER_DX_MULA_COLOR_TEX1,
    PLD3D9_SHADER_DX_X4_COLOR_NOTEX,
    PLD3D9_SHADER_DX_X4_COLOR_TEX1,
    PLD3D9_SHADER_DX_PMA_COLOR_NOTEX,
    PLD3D9_SHADER_DX_PMA_COLOR_TEX1,
    PLD3D9_SHADER_DX_PMA_INVERT_COLOR_NOTEX,
    PLD3D9_SHADER_DX_PMA_INVERT_COLOR_TEX1,
    PLD3D9_SHADER_DX_PMA_X4_COLOR_NOTEX,
    PLD3D9_SHADER_DX_PMA_X4_COLOR_TEX1,
    PLD3D9_SHADER_END
} PLD3D9ShaderPresetType;

typedef struct _PLD3D9ShaderDefinition {
    const char * vertexShader;
    const char * fragmentShader;
    int textureCount;
    int texcoordCount;
    int hasColor;
} PLD3D9ShaderDefinition;

typedef struct _PLD3D9ShaderInfo {
    PLD3D9ShaderDefinition definition;
} PLD3D9ShaderInfo;

extern int PL_drawScreenWidth;
extern int PL_drawScreenHeight;

// external

extern void PLD3D9_SetBlendMode(
                int blendEquation,
                int srcBlend, int destBlend);
extern void PLD3D9_SetBlendModeSeparate(
                int blendEquation,
                int srcRGBBlend, int destRGBBlend,
                int srcAlphaBlend, int destAlphaBlend);
extern void PLD3D9_DisableBlend();

extern int PLD3D9_SetScissor(int x, int y, int w, int h);
extern int PLD3D9_SetScissorRect(const RECT *rect);
extern int PLD3D9_DisableScissor();

extern int PLD3D9_DisableCulling();
extern int PLD3D9_DisableDepthTest();

extern int PLD3D9_SetTextureStage(unsigned int stage,
                                     int textureRefID, int textureDrawMode);
extern int PLD3D9_SetPresetProgram(int preset, int flags,
                                 const PLMatrix *projectionMatrix,
                                 const PLMatrix *viewMatrix,
                                 int textureRefID, int textureDrawMode,
                                 float alphaTestValue);
extern int PLD3D9_ClearTextures();
extern int PLD3D9_ClearPresetProgram();

extern int PLD3D9_DrawVertexArray(const VertexDefinition *def,
               const char *vertexData,
               int primitiveType, int vertexStart, int vertexCount);
extern int PLD3D9_DrawVertexIndexArray(const VertexDefinition *def,
               const char *vertexData, int vertexStart, int vertexCount,
               const unsigned short *indexData,
               int primitiveType, int indexStart, int indexCount);

extern int PLD3D9_DrawVertexBuffer(const VertexDefinition *def,
               int vertexBufferHandle,
               int primitiveType, int vertexStart, int vertexCount);
extern int PLD3D9_DrawVertexIndexBuffer(const VertexDefinition *def,
               int vertexBufferHandle, int vertexStart, int vertexCount,
               int indexBufferHandle,
               int primitiveType, int indexStart, int indexCount);

extern int PLD3D9_SetViewport(int x, int y, int w, int h);
extern int PLD3D9_SetZRange(float nearZ, float farZ);

extern int PLD3D9_ClearColor(float r, float g, float b, float a);
extern int PLD3D9_Clear();

extern int PLD3D9_StartFrame();
extern int PLD3D9_EndFrame();

extern int PLD3D9_VertexBuffer_CreateBytes(int vertexByteSize,
                                       const char *vertexData, int bufferSize,
                                       int isStatic);
extern int PLD3D9_VertexBuffer_Create(const VertexDefinition *def,
                                  const char *vertexData, int vertexCount,
                                  int isStatic);
extern int PLD3D9_VertexBuffer_ResetBuffer(int vboHandle);
extern int PLD3D9_VertexBuffer_SetDataBytes(int vboHandle, const char *vertices,
                                        int start, int count, int resetBufferFlag);
extern int PLD3D9_VertexBuffer_SetData(int vboHandle, const char *vertices,
                                   int start, int count, int resetBufferFlag);
extern char *PLD3D9_VertexBuffer_Lock(int vboHandle);
extern int PLD3D9_VertexBuffer_Unlock(int vboHandle, char *buffer);
extern int PLD3D9_VertexBuffer_Delete(int vboHandle);

extern int PLD3D9_IndexBuffer_Create(const unsigned short *indexData,
                                 int indexCount, int isStatic);
extern int PLD3D9_IndexBuffer_ResetBuffer(int iboHandle);
extern int PLD3D9_IndexBuffer_SetData(int iboHandle,
                                  const unsigned short *indices,
                                  int start, int count, int resetBufferFlag);
extern unsigned short *PLD3D9_IndexBuffer_Lock(int iboHandle);
extern int PLD3D9_IndexBuffer_Unlock(int iboHandle);
extern int PLD3D9_IndexBuffer_Delete(int iboHandle);

extern int PLD3D9_Texture_CreateFromSDLSurface(SDL_Surface *surface, int hasAlphaChannel);
extern int PLD3D9_Texture_CreateFromDimensions(int width, int height, int hasAlphaChannel);
extern int PLD3D9_Texture_CreateFramebuffer(int width, int height, int hasAlphaChannel);

extern int PLD3D9_Texture_BlitSurface(int textureID, SDL_Surface *surface, const PLRect *rect);

extern int PLD3D9_Texture_RenderGetTextureInfo(int textureRefID, PLRect *rect, float *xMult, float *yMult);

extern int PLD3D9_Texture_SetWrap(int textureRefID, int wrapState);

extern int PLD3D9_Texture_HasAlphaChannel(int textureRefID);

extern int PLD3D9_Texture_BindFramebuffer(int textureRefID);

extern int PLD3D9_Texture_AddRef(int textureID);
extern int PLD3D9_Texture_Release(int textureID);

// internal

extern int PLD3D9_Render_Init();
extern int PLD3D9_Render_End();
extern int PLD3D9_Render_UpdateMatrices();

extern int PLD3D9_Texture_Bind(int textureRefID, int drawMode);
extern int PLD3D9_Texture_Unbind(int textureRefID);
extern int PLD3D9_Texture_ClearAllData();

extern int PLD3D9_Framebuffer_GetSurface(const PLRect *rect, SDL_Surface **dSurface);

extern int PLD3D9_FixedFunction_ClearPresetProgram();
extern int PLD3D9_FixedFunction_SetPresetProgram(int preset, int flags,
                                   const PLMatrix *projectionMatrix,
                                   const PLMatrix *viewMatrix,
                                   int textureRefID, int textureDrawMode,
                                   float alphaTestValue);
extern int PLD3D9_FixedFunction_ApplyVertexArrayData(const VertexDefinition *def,
                                          const char *vertexData);
extern int PLD3D9_FixedFunction_ClearVertexArrayData(const VertexDefinition *def);
extern int PLD3D9_FixedFunction_ApplyVertexBufferData(const VertexDefinition *def);
extern int PLD3D9_FixedFunction_ClearVertexBufferData(const VertexDefinition *def);
extern int PLD3D9_FixedFunction_Init();
extern int PLD3D9_FixedFunction_Cleanup();

extern int PLD3D9_Shaders_CompileDefinition(const PLD3D9ShaderDefinition *definition);
extern void PLD3D9_Shaders_DeleteShader(int shaderHandle);
extern void PLD3D9_Shaders_UseProgram(int shaderHandle);
/* This doesn't make sense outside of presets. */
extern void PLD3D9_Shaders_ApplyProgramMatrices(int shaderHandle,
                                      const PLMatrix *projectionMatrix,
                                      const PLMatrix *viewMatrix);
extern void PLD3D9_Shaders_ApplyProgramVertexData(int shaderHandle,
                                    const char *vertexData,
                                    const VertexDefinition *definition);
extern void PLD3D9_Shaders_ClearProgramVertexData(int shaderHandle,
                                    const VertexDefinition *definition);
extern int PLD3D9_Shaders_GetStockProgramForID(PLD3D9ShaderPresetType shaderType);
extern void PLD3D9_Shaders_Init();
extern void PLD3D9_Shaders_Cleanup();

#endif /* #ifdef DXPORTLIB_DRAW_OPEND3D9 */

#endif /* #ifndef DXPORTLIB_OPEND3D9_DXINTERNAL_H */

