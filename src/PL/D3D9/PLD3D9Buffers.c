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

#include "DPLBuildConfig.h"

#ifdef DXPORTLIB_DRAW_DIRECT3D9

#include "PL/PLInternal.h"

#include "PLD3D9Internal.h"

/* ------------------------------------------------------ Vertex Buffers */

int PLD3D9_VertexBuffer_CreateBytes(int vertexByteSize,
                                  const char *vertexData, int bufferSize,
                                  int isStatic) {
    return -1;
}

int PLD3D9_VertexBuffer_Create(const VertexDefinition *def,
                             const char *vertexData, int vertexCount,
                             int isStatic) {
    return PLD3D9_VertexBuffer_CreateBytes(def->vertexByteSize, vertexData,
                                         vertexCount * def->vertexByteSize,
                                         isStatic);
}

int PLD3D9_VertexBuffer_ResetBuffer(int vboHandle) {
    return 0;
}

int PLD3D9_VertexBuffer_SetDataBytes(int vboHandle, const char *vertices, int start, int count,
                                 int resetBufferFlag) {
    return -1;
}

int PLD3D9_VertexBuffer_SetData(int vboHandle, const char *vertices, int start, int count,
                              int resetBufferFlag) {
    return -1;
}

char *PLD3D9_VertexBuffer_Lock(int vboHandle) {
    return NULL;
}

int PLD3D9_VertexBuffer_Unlock(int vboHandle, char *buffer) {
    return -1;
}

int PLD3D9_VertexBuffer_Delete(int vboHandle) {
    return -1;
}

/* ------------------------------------------------------- Index Buffers */

int PLD3D9_IndexBuffer_Create(const unsigned short *indexData, int indexCount,
                          int isStatic) {
    return -1;
}

int PLD3D9_IndexBuffer_ResetBuffer(int iboHandle) {
    return 0;
}

int PLD3D9_IndexBuffer_SetData(int iboHandle, const unsigned short *indices, int start, int count,
                           int resetBufferFlag) {
    return -1;
}

unsigned short *PLD3D9_IndexBuffer_Lock(int iboHandle) {
    return NULL;
}

int PLD3D9_IndexBuffer_Unlock(int iboHandle) {
    return -1;
}

int PLD3D9_IndexBuffer_Delete(int iboHandle) {
    return -1;
}

#endif
