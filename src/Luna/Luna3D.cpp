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

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"
#include "PL/PLInternal.h"

#include "DxLib/DxBlendModes.h"

float g_lunaFilterOffset = 0.5f;
int g_lunaFilterMode = DX_DRAWMODE_BILINEAR;
int g_luna3DCamera = INVALID_CAMERA;
int g_lunaAlphaTestPreset = PL_PRESETFLAG_ALPHATEST_GREATER;
float g_lunaAlphaTestValue = 0.0f;

int g_lunaTexturePreset = TEX_PRESET_MODULATE;
PLMatrix g_lunaUntransformedProjectionMatrix;
PLMatrix g_lunaUntransformedViewMatrix;

int s_prevRenderTexture = -2;
int s_renderTexture = -1;

RECT g_viewportRect;

static void s_UpdateRenderTexture() {
    if (s_renderTexture != s_prevRenderTexture) {
        if (s_renderTexture < 0) {
            PL_Window_BindMainFramebuffer();
            Luna3D::SetViewport(NULL);
        } else {
            PLG.Texture_BindFramebuffer(s_renderTexture);
            
            PLRect txr;
            PLG.Texture_RenderGetTextureInfo(s_renderTexture, &txr, NULL, NULL);
            
            RECT r = { 0, 0, txr.w, txr.h };
            Luna3D::SetViewport(&r);
        }
        
        s_prevRenderTexture = s_renderTexture;
    }
}

Bool Luna3D::BeginScene(void) {
    PLG.StartFrame();
    s_prevRenderTexture = -2;
    
    PLG.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //PL_Window_BindMainFramebuffer();
    return true;
}
void Luna3D::EndScene(void) {
    PLG.EndFrame();
}

void Luna3D::Refresh(void) {
    PL_Window_SwapBuffers();
}

void Luna3D::SetViewport(const RECT *rect) {
    RECT r;
    if (rect == NULL) {
        Sint32 w, h;
        Luna::GetScreenData(&w, &h, NULL);

        rect = &r;
        r.top = 0;
        r.left = 0;
        r.right = w;
        r.bottom = h;
    }
    
    PLG.SetViewport(
        rect->left, rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top);
    PLG.SetZRange(0.0f, 1.0f);
    
    g_viewportRect = *rect;
    
    PL_Matrix_CreateOrthoOffCenterLH(&g_lunaUntransformedProjectionMatrix,
                      (float)rect->left, (float)rect->right,
                      (float)rect->top, (float)rect->bottom,
                      0.0f, 1.0f);
    PL_Matrix_CreateTranslation(&g_lunaUntransformedViewMatrix, 0.5f, 0.5f, 0.0f);
}

void Luna3D::GetViewport(RECT *rect) {
    *rect = g_viewportRect;
}

void Luna3D::SetFilterEnable(Bool Flag) {
    if (Flag) {
        g_lunaFilterMode = DX_DRAWMODE_BILINEAR;
        g_lunaFilterOffset = 0.5f;
    } else {
        g_lunaFilterMode = DX_DRAWMODE_NEAREST;
        g_lunaFilterOffset = 0.0f;
    }
}

Bool Luna3D::GetFilterEnable() {
    if (g_lunaFilterMode == DX_DRAWMODE_BILINEAR) {
        return true;
    } else {
        return false;
    }
}

void Luna3D::SetColorkeyEnable(Bool Flag) {
    if (Flag) {
        g_lunaAlphaTestPreset = PL_PRESETFLAG_ALPHATEST_GREATER;
        g_lunaAlphaTestValue = 0.0f;
    } else {
        g_lunaAlphaTestPreset = 0;
    }
}
void Luna3D::SetBlendingType(eBlendType BlendType) {
    g_lunaTexturePreset = TEX_PRESET_MODULATE;
    
    switch(BlendType) {
        case BLEND_NONE:
            PLG.DisableBlend();
            break;
        case BLEND_NORMAL:
            PLG.SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_SRC_ALPHA,
                PL_BLEND_ONE_MINUS_SRC_ALPHA,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_ADD:
            PLG.SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_SRC_ALPHA,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_ADD_NOALPHA:
            PLG.SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_ONE,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_SUB:
            PLG.SetBlendModeSeparate(
                PL_BLENDFUNC_RSUB,
                PL_BLEND_SRC_ALPHA,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_SUB_NOALPHA:
            PLG.SetBlendModeSeparate(
                PL_BLENDFUNC_RSUB,
                PL_BLEND_ONE,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_MUL:
            PLG.SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_ZERO,
                PL_BLEND_SRC_COLOR,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_REVERSE:
            PLG.SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_ONE_MINUS_DST_COLOR,
                PL_BLEND_ZERO,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
    }
    return;
}

void Luna3D::SetZBufferEnable(Bool Flag) {
    /* Ignore */
}
void Luna3D::SetRenderState(D3DRENDERSTATETYPE State, Uint32 Param) {
    return;
}

void Luna3D::Clear(Uint32 ClearFlags, D3DCOLOR Color, Float Depth,
                          Uint32 Stencil, RECT *pDst) {
    /* Ignore for now */
}

void Luna3D::SetCamera(LCAMERA lCamera) {
    g_luna3DCamera = lCamera;
    
    LunaCamera_SetDevice(lCamera);
    
    /* Set the global view/perspective matrices here. */
}

void Luna3D::EXTSetDxBlendingType(int blendMode) {
    if (blendMode < 0 || blendMode >= DX_BLENDMODE_NUM) {
        blendMode = DX_BLENDMODE_NOBLEND;
    }
    
    const BlendInfo *blend = &s_blendModeTable[blendMode];
    
    PLG.SetBlendModeSeparate(
        blend->blendEquation,
        blend->srcRGBBlend, blend->destRGBBlend,
        blend->srcAlphaBlend, blend->destAlphaBlend);
    g_lunaTexturePreset = blend->texturePreset;
}

void Luna3D::SetRenderTargetTexture(LTEXTURE lTex) {
    if (lTex < 0) {
        ResetRenderTarget();
    } else {
        s_renderTexture = (int)lTex;
        
        ResetDepthStencil();
    }
}

void Luna3D::ResetRenderTarget() {
    s_renderTexture = -1;
    
    ResetDepthStencil();
}

void Luna3D::ResetDepthStencil() {
    s_UpdateRenderTexture();
    
    // - Set DepthStencil Surface
    
    // - Enable Z testing
    // - Enable Z writing
}

void Luna3DStartDraw(int textureID) {
    s_UpdateRenderTexture();
    
    PLG.SetPresetProgram(
        g_lunaTexturePreset, g_lunaAlphaTestPreset,
        &g_lunaUntransformedProjectionMatrix,
        &g_lunaUntransformedViewMatrix,
        textureID, g_lunaFilterMode,
        g_lunaAlphaTestValue);
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
