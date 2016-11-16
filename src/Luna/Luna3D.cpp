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

PLAlphaFunc s_alphaTestFunc = PL_ALPHAFUNC_ALWAYS;
float s_alphaTestValue = 0.0f;
bool s_alphaTestEnable = false;

int g_lunaTexturePreset = TEX_PRESET_MODULATE;
PLMatrix g_lunaUntransformedProjectionMatrix;
PLMatrix g_lunaUntransformedViewMatrix;

int s_prevRenderTexture = -2;
int s_renderTexture = -1;
int s_renderbufferID = -1;
bool s_depthWrite = false;
bool s_depthRead = false;

int s_blendType = 0; // 0 = Luna, 1 = DxLib blend
eBlendType s_blendLunaMode = BLEND_NORMAL;
int s_blendDxMode = DX_BLENDMODE_ALPHA;

RECT g_viewportRect;

static void s_UpdateRenderTexture() {
    if (s_renderTexture != s_prevRenderTexture) {
        if (s_renderTexture < 0) {
            PL_Window_BindMainFramebuffer();
            Luna3D::SetViewport(NULL);
        } else {
            PLG.Texture_BindFramebuffer(s_renderTexture, s_renderbufferID);
            
            PLRect txr;
            PLG.Texture_RenderGetTextureInfo(s_renderTexture, &txr, NULL, NULL);
            
            RECT r = { 0, 0, txr.w, txr.h };
            Luna3D::SetViewport(&r);
        }
        
        // Force reset ZBuffer state, in case something else modified it.
        Luna3D::SetZBufferEnable(s_depthRead);
        Luna3D::SetZWriteEnable(s_depthWrite);
        PLG.SetDepthFunc(PL_DEPTHFUNC_LEQUAL);
        PLG.Clear((PLClearType) (PL_CLEAR_DEPTH)); // I do not know why I have to do this, but it's necessary.
        
        s_prevRenderTexture = s_renderTexture;
    }
}

Bool Luna3D::BeginScene(void) {
    if (PL_Window_GetWaitVSyncFlag() == DXTRUE) {
        int ticks = PL_Platform_GetTicks();
        PLG.Finish();
        ticks = PL_Platform_GetTicks() - ticks;
        
        if (ticks > 4) {
            g_lunaSyncOffset = ticks - 4;
        }
    }
    
    PLG.StartFrame();
    s_prevRenderTexture = -2;
    
    PLG.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    if (s_blendType == 0) {
        Luna3D::SetBlendingType(s_blendLunaMode);
    } else {
        Luna3D::EXTSetDxBlendingType(s_blendDxMode);
    }
    
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
        s_alphaTestEnable = true;
        s_alphaTestFunc = PL_ALPHAFUNC_GREATER;
        s_alphaTestValue = 0.0f;
    } else {
        s_alphaTestEnable = false;
    }
}
void Luna3D::SetBlendingType(eBlendType BlendType) {
    g_lunaTexturePreset = TEX_PRESET_MODULATE;
    
    s_blendType = 0;
    s_blendLunaMode = BlendType;
    
    switch(BlendType) {
        case BLEND_NONE:
            EXTSetDxBlendingType(DX_BLENDMODE_NOBLEND);
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
    s_depthRead = Flag;
    if (Flag) {
        PLG.EnableDepthTest();
    } else {
        PLG.DisableDepthTest();
    }
}

void Luna3D::SetZWriteEnable(Bool Flag) {
    s_depthWrite = Flag;
    if (Flag) {
        PLG.EnableDepthWrite();
    } else {
        PLG.DisableDepthWrite();
    }
}

void Luna3D::SetRenderState(D3DRENDERSTATETYPE State, Uint32 Param) {
    switch(State) {
        case D3DRS_ZENABLE:
            SetZBufferEnable(Param > 0 ? true : false);
            break;
        case D3DRS_ZWRITEENABLE:
            SetZWriteEnable(Param > 0 ? true : false);
            break;
        case D3DRS_CULLMODE:
            // uhoh
            break;
        case D3DRS_FILLMODE:
            break;
    }
    return;
}

void Luna3D::Clear(Uint32 ClearFlags, D3DCOLOR Color, Float Depth,
                          Uint32 Stencil, RECT *pDst)
{
    PLClearType flags = PL_CLEAR_NONE;
    if ((ClearFlags & D3DCLEAR_ZBUFFER) != 0) {
        flags = (PLClearType)(flags | PL_CLEAR_DEPTH);
        PLG.ClearDepth(Depth);
    }
    if ((ClearFlags & D3DCLEAR_TARGET) != 0) {
        flags = (PLClearType)(flags | PL_CLEAR_COLOR);
        PLG.ClearColor(
            ((Color >> 16) & 0xff) / 255.0f,
            ((Color >> 8) & 0xff) / 255.0f,
            ((Color >> 0) & 0xff) / 255.0f,
            ((Color >> 24) & 0xff) / 255.0f
        );
    }
    
    if (flags != 0) {
        s_UpdateRenderTexture();
        
        PLG.Clear(flags);
    }
}

void Luna3D::SetCamera(LCAMERA lCamera) {
    g_luna3DCamera = lCamera;
    
    LunaCamera_SetDevice(lCamera);
    
    /* Set the global view/perspective matrices here. */
}

void Luna3D::EXTSetDxBlendingType(int blendMode) {
    if ((blendMode < 0 || blendMode >= DX_BLENDMODE_NUM) && (blendMode < DX_BLENDMODE_EXT || blendMode >= DX_BLENDMODE_EXT_END)) {
        blendMode = DX_BLENDMODE_NOBLEND;
    }
    
    const BlendInfo *blend;
    
    if (blendMode >= DX_BLENDMODE_EXT) {
        blend = &s_blendModeEXTTable[blendMode - DX_BLENDMODE_EXT];
    } else {
        blend = &s_blendModeTable[blendMode];
    }
    
    if (blendMode == DX_BLENDMODE_NOBLEND) {
        PLG.DisableBlend();
    } else {
        PLG.SetBlendModeSeparate(
            blend->blendEquation,
            blend->srcRGBBlend, blend->destRGBBlend,
            blend->srcAlphaBlend, blend->destAlphaBlend);
    }
    g_lunaTexturePreset = blend->texturePreset;
    
    s_blendType = 1;
    s_blendDxMode = blendMode;
}

void Luna3D::EnableFullColorMode() {
    // stub
}
void Luna3D::SetCallbackDeviceReset(void(*pCallback)(void)) {
    // stub
}
void Luna3D::SetCallbackDeviceRestore(void(*pCallback)(void)) {
    // stub
}

void Luna3D::SetRenderTargetTexture(LTEXTURE lTex) {
    if (lTex < 0) {
        ResetRenderTarget();
    } else {
        s_renderTexture = (int)lTex;
        
        ResetDepthStencil();
    }
}
LTEXTURE Luna3D::GetRenderTargetTexture() {
    return (LTEXTURE)s_renderTexture;
}

void Luna3D::SetRenderTargetSurface(LSURFACE lSurf) {
    // stub
}
LSURFACE Luna3D::GetRenderTargetSurface() {
    // stub
    return INVALID_SURFACE;
}

void Luna3D::SetDepthStencilSurface(LSURFACE lSurf) {
    s_renderbufferID = (int)lSurf;
    
    PL_Window_SetDefaultRenderbuffer(s_renderbufferID);
    
    s_prevRenderTexture = -9999;
    
    s_UpdateRenderTexture();
}

void Luna3D::ResetRenderTarget() {
    s_renderTexture = -1;
    
    ResetDepthStencil();
}

void Luna3D::ResetDepthStencil() {
    s_UpdateRenderTexture();
    
    SetZBufferEnable(s_depthRead);
    SetZWriteEnable(s_depthWrite);
}

void Luna3DStartDraw(int textureID) {
    s_UpdateRenderTexture();
    
    PLG.SetPresetProgram(
        g_lunaTexturePreset,
        &g_lunaUntransformedProjectionMatrix,
        &g_lunaUntransformedViewMatrix,
        textureID, g_lunaFilterMode,
        (s_alphaTestEnable == true) ? s_alphaTestFunc : PL_ALPHAFUNC_ALWAYS,
        s_alphaTestValue);
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
