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

#include "DPLBuildConfig.h"

#ifdef DXPORTLIB_DRAW_DIRECT3D9

#include "PLD3D9Internal.h"

extern int PL_drawOffscreen;

static void s_debugPrint(const char *string) {
    /* fprintf(stderr, "%s\n", string); */
}

int PLD3D9_End() {
    PLD3D9_Render_End();
    
    PLD3D9_Texture_ClearAllData();
    
    return 0;
}

int PLD3D9_Init() {
    memset(&PLG, 0, sizeof(PLG));
    PLG.SetBlendMode = PLD3D9_SetBlendMode;
    PLG.SetBlendModeSeparate = PLD3D9_SetBlendModeSeparate;
    PLG.DisableBlend = PLD3D9_DisableBlend;
    PLG.SetScissor = PLD3D9_SetScissor;
    PLG.SetScissorRect = PLD3D9_SetScissorRect;
    PLG.DisableScissor = PLD3D9_DisableScissor;
    PLG.DisableCulling = PLD3D9_DisableCulling;
    PLG.DisableDepthTest = PLD3D9_DisableDepthTest;
    
    PLG.SetPresetProgram = PLD3D9_SetPresetProgram;
    PLG.ClearPresetProgram = PLD3D9_ClearPresetProgram;
    
    PLG.VertexBuffer_CreateBytes = PLD3D9_VertexBuffer_CreateBytes;
    PLG.VertexBuffer_Create = PLD3D9_VertexBuffer_Create;
    PLG.VertexBuffer_ResetBuffer = PLD3D9_VertexBuffer_ResetBuffer;
    PLG.VertexBuffer_SetDataBytes = PLD3D9_VertexBuffer_SetDataBytes;
    PLG.VertexBuffer_SetData = PLD3D9_VertexBuffer_SetData;
    PLG.VertexBuffer_Lock = PLD3D9_VertexBuffer_Lock;
    PLG.VertexBuffer_Unlock = PLD3D9_VertexBuffer_Unlock;
    PLG.VertexBuffer_Delete = PLD3D9_VertexBuffer_Delete;
    
    PLG.IndexBuffer_Create = PLD3D9_IndexBuffer_Create;
    PLG.IndexBuffer_ResetBuffer = PLD3D9_IndexBuffer_ResetBuffer;
    PLG.IndexBuffer_SetData = PLD3D9_IndexBuffer_SetData;
    PLG.IndexBuffer_Lock = PLD3D9_IndexBuffer_Lock;
    PLG.IndexBuffer_Unlock = PLD3D9_IndexBuffer_Unlock;
    PLG.IndexBuffer_Delete = PLD3D9_IndexBuffer_Delete;

    PLG.Texture_CreateFromSDLSurface = PLD3D9_Texture_CreateFromSDLSurface;
    PLG.Texture_CreateFromDimensions = PLD3D9_Texture_CreateFromDimensions;
    PLG.Texture_CreateFramebuffer = PLD3D9_Texture_CreateFramebuffer;
    PLG.Texture_BlitSurface = PLD3D9_Texture_BlitSurface;
    PLG.Texture_RenderGetTextureInfo = PLD3D9_Texture_RenderGetTextureInfo;
    PLG.Texture_SetWrap = PLD3D9_Texture_SetWrap;
    PLG.Texture_HasAlphaChannel = PLD3D9_Texture_HasAlphaChannel;
    PLG.Texture_BindFramebuffer = PLD3D9_Texture_BindFramebuffer;
    PLG.Texture_AddRef = PLD3D9_Texture_AddRef;
    PLG.Texture_Release = PLD3D9_Texture_Release;
    
    PLG.DrawVertexArray = PLD3D9_DrawVertexArray;
    PLG.DrawVertexIndexArray = PLD3D9_DrawVertexIndexArray;
    PLG.DrawVertexBuffer = PLD3D9_DrawVertexBuffer;
    PLG.DrawVertexIndexBuffer = PLD3D9_DrawVertexIndexBuffer;
    PLG.SetViewport = PLD3D9_SetViewport;
    PLG.SetZRange = PLD3D9_SetZRange;
    PLG.ClearColor = PLD3D9_ClearColor;
    PLG.Clear = PLD3D9_Clear;
    PLG.StartFrame = PLD3D9_StartFrame;
    PLG.EndFrame = PLD3D9_EndFrame;
    
    PLG.End = PLD3D9_End;
    
    PLD3D9_Render_Init();
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_DRAW_OPEND3D9 */

