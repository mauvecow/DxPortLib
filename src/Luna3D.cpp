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

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"
#include "PLInternal.h"

float g_lunaFilterOffset = 0.5f;
int g_lunaFilterMode = DX_DRAWMODE_BILINEAR;
int g_luna3DCamera = INVALID_CAMERA;

RECT g_viewportRect;

Bool Luna3D::BeginScene(void) {
    PL_Render_StartFrame();
    return true;
}
void Luna3D::EndScene(void) {
    PL_Render_EndFrame();
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
    
    PL_Render_SetViewport(
        rect->left, rect->top,
        rect->right - rect->left,
        rect->bottom - rect->top);
    PL_Render_SetZRange(0.0f, 1.0f);
    
    g_viewportRect = *rect;
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
void Luna3D::SetColorkeyEnable(Bool Flag) {
    if (Flag) {
        PL_Render_EnableAlphaTest();
    } else {
        PL_Render_DisableAlphaTest();
    }
}
void Luna3D::SetBlendingType(eBlendType BlendType) {
    switch(BlendType) {
        case BLEND_NONE:
            PL_Render_DisableBlend();
            break;
        case BLEND_NORMAL:
            PL_Render_SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_SRC_ALPHA,
                PL_BLEND_ONE_MINUS_SRC_ALPHA,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_ADD:
            PL_Render_SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_SRC_ALPHA,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_ADD_NOALPHA:
            PL_Render_SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_ONE,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_SUB:
            PL_Render_SetBlendModeSeparate(
                PL_BLENDFUNC_RSUB,
                PL_BLEND_SRC_ALPHA,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_SUB_NOALPHA:
            PL_Render_SetBlendModeSeparate(
                PL_BLENDFUNC_RSUB,
                PL_BLEND_ONE,
                PL_BLEND_ONE,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_MUL:
            PL_Render_SetBlendModeSeparate(
                PL_BLENDFUNC_ADD,
                PL_BLEND_ZERO,
                PL_BLEND_SRC_COLOR,
                PL_BLEND_ZERO,
                PL_BLEND_ONE);
            break;
        case BLEND_REVERSE:
            PL_Render_SetBlendModeSeparate(
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

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
