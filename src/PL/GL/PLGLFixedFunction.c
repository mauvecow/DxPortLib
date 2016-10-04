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

#ifndef DXPORTLIB_DRAW_OPENGL_ES2

#define MAX_TEXTURE 4
static int s_pixelTexture = -1;

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

int PLGL_FixedFunction_ClearPresetProgram() {
    return 0;
}

int PLGL_FixedFunction_SetPresetProgram(int preset, int flags,
                                        const PLMatrix *projectionMatrix,
                                        const PLMatrix *viewMatrix,
                                        int textureRefID, int textureDrawMode,
                                        float alphaTestValue) {
    unsigned int mainTexSlot = 0;
    int rgbScale = 1;
    
    PL_GL.glMatrixMode(GL_PROJECTION);
    PL_GL.glLoadMatrixf((const float *)projectionMatrix);
    
    PL_GL.glMatrixMode(GL_MODELVIEW);
    PL_GL.glLoadMatrixf((const float *)viewMatrix);
    
    if ((flags & PL_PRESETFLAG_ALPHATEST_MASK) != 0) {
        PL_GL.glEnable(GL_ALPHA_TEST);
        switch(flags & PL_PRESETFLAG_ALPHATEST_MASK) {
            case PL_PRESETFLAG_ALPHATEST_EQUAL:
                PL_GL.glAlphaFunc(GL_EQUAL, alphaTestValue);
                break;
            case PL_PRESETFLAG_ALPHATEST_NOTEQUAL:
                PL_GL.glAlphaFunc(GL_NOTEQUAL, alphaTestValue);
                break;
            case PL_PRESETFLAG_ALPHATEST_LESS:
                PL_GL.glAlphaFunc(GL_LESS, alphaTestValue);
                break;
            case PL_PRESETFLAG_ALPHATEST_LEQUAL:
                PL_GL.glAlphaFunc(GL_LEQUAL, alphaTestValue);
                break;
            case PL_PRESETFLAG_ALPHATEST_GREATER:
                PL_GL.glAlphaFunc(GL_GREATER, alphaTestValue);
                break;
            case PL_PRESETFLAG_ALPHATEST_GEQUAL:
                PL_GL.glAlphaFunc(GL_GEQUAL, alphaTestValue);
                break;
            default:
                /* Never mind. */
                PL_GL.glDisable(GL_ALPHA_TEST);
                break;
        }
    } else {
        PL_GL.glDisable(GL_ALPHA_TEST);
    }
    
    PL_GL.glActiveTexture(GL_TEXTURE0);
    
    switch(preset) {
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
                PLGL_SetTextureStage(0, s_pixelTexture, textureDrawMode);
                
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
                PLGL_SetTextureStage(0, s_pixelTexture, textureDrawMode);
                
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
                PLGL_SetTextureStage(0, s_pixelTexture, textureDrawMode);
                
                mainTexSlot = 1;
            }
            rgbScale = 4;
            break;
        default: /* TEX_PRESET_MODULATE */
            PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            break;
    }
    
    PLGL_SetTextureStage(mainTexSlot, textureRefID, textureDrawMode);
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

/* ---------------------------------------------------- VERTEX RENDERING */
static GLenum VertexElementSizeToGL(int value) {
    switch(value) {
        case VERTEXSIZE_UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        default: /* VERTEXSIZE_FLOAT */
            return GL_FLOAT;
    }
}

int PLGL_FixedFunction_ApplyVertexArrayData(const VertexDefinition *def,
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

int PLGL_FixedFunction_ClearVertexArrayData(const VertexDefinition *def) {
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

int PLGL_FixedFunction_ApplyVertexBufferData(const VertexDefinition *def) {
    return PLGL_FixedFunction_ApplyVertexArrayData(def, 0);
}

int PLGL_FixedFunction_ClearVertexBufferData(const VertexDefinition *def) {
    return PLGL_FixedFunction_ClearVertexArrayData(def);
}

int PLGL_FixedFunction_Init() {
    /* Create a texture that's a single white pixel. */
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 1, 32,
                                   0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    *((unsigned int *)(surface->pixels)) = 0xffffffff;
    s_pixelTexture = PLGL_Texture_CreateFromSDLSurface(surface, DXFALSE);
    SDL_FreeSurface(surface);
    
    return 0;
}
int PLGL_FixedFunction_Cleanup() {
    if (s_pixelTexture >= 0) {
        PLGL_Texture_Release(s_pixelTexture);
        s_pixelTexture = -1;
    }
    
    return 0;
}

#endif

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
