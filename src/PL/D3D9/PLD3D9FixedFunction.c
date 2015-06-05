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

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_DRAW_DIRECT3D9

#include "PL/PLInternal.h"

#include "PLD3D9Internal.h"

#include "SDL.h"

#ifndef DXPORTLIB_DRAW_OPEND3D9_ES2

#define MAX_TEXTURE 4
static int s_pixelTexture = -1;

/* For OpenD3D9, glTexEnv* combiners can take the place of fragment programs.
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

int PLD3D9_FixedFunction_ClearPresetProgram() {
    return 0;
}

int PLD3D9_FixedFunction_SetPresetProgram(int preset, int flags,
                                        const PLMatrix *projectionMatrix,
                                        const PLMatrix *viewMatrix,
                                        int textureRefID, int textureDrawMode,
                                        float alphaTestValue) {
    return 0;
}

/* ---------------------------------------------------- VERTEX RENDERING */
int PLD3D9_FixedFunction_ApplyVertexArrayData(const VertexDefinition *def,
                                          const char *vertexData) {
    return 0;
}

int PLD3D9_FixedFunction_ClearVertexArrayData(const VertexDefinition *def) {
    return 0;
}

int PLD3D9_FixedFunction_ApplyVertexBufferData(const VertexDefinition *def) {
    return PLD3D9_FixedFunction_ApplyVertexArrayData(def, 0);
}

int PLD3D9_FixedFunction_ClearVertexBufferData(const VertexDefinition *def) {
    return PLD3D9_FixedFunction_ClearVertexArrayData(def);
}

int PLD3D9_FixedFunction_Init() {
    return 0;
}
int PLD3D9_FixedFunction_Cleanup() {
    return 0;
}

#endif

#endif /* #ifdef DXPORTLIB_DRAW_OPEND3D9 */
