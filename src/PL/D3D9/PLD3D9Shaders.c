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

int PLD3D9_Shaders_CompileDefinition(const PLD3D9ShaderDefinition *definition) {
    return -1;
}

void PLD3D9_Shaders_DeleteShader(int shaderHandle) {
}

void PLD3D9_Shaders_UseProgram(int shaderHandle) {
}

void PLD3D9_Shaders_ApplyProgramMatrices(int shaderHandle,
                               const PLMatrix *projectionMatrix, const PLMatrix *viewMatrix) {
}

void PLD3D9_Shaders_ApplyProgramVertexData(int shaderHandle,
                             const char *vertexData, const VertexDefinition *definition)
{
}

void PLD3D9_Shaders_ClearProgramVertexData(int shaderHandle, const VertexDefinition *definition) {
}

int PLD3D9_Shaders_GetStockProgramForID(PLD3D9ShaderPresetType shaderType) {
    return -1;
}

void PLD3D9_Shaders_Init() {
}

void PLD3D9_Shaders_Cleanup() {
}

#endif
