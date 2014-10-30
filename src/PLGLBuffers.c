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

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "PLInternal.h"

#include "PLGLInternal.h"

/* ------------------------------------------------------ Vertex Buffers */

typedef struct _VBufferData {
    GLuint vboID;
    
    int vertexByteSize;
    
    const VertexDefinition *vertexDefinition;
} VBufferData;

int PL_VertexBuffer_Create(const VertexDefinition *def,
                           const char *vertexData, int vertexCount,
                           int isStatic) {
    int byteSize = def->vertexByteSize;
    GLuint vboID = 0;
    int vboHandle;
    VBufferData *vb;
    
    if (PL_GL.glGenBuffersARB != NULL) {
        GLenum bufferUsage = GL_DYNAMIC_DRAW_ARB;
        if (isStatic) {
            bufferUsage = GL_STATIC_DRAW_ARB;
        }
        
        PL_GL.glGenBuffersARB(1, &vboID);
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboID);
        PL_GL.glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                              vertexCount * byteSize,
                              vertexData, bufferUsage);
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }
    
    vboHandle = PL_Handle_AcquireID(DXHANDLE_VERTEXBUFFER);
    vb = (VBufferData *)PL_Handle_AllocateData(vboHandle, sizeof(VBufferData));
    vb->vboID = vboID;
    vb->vertexByteSize = byteSize;
    vb->vertexDefinition = def;
    
    return vboHandle;
}

int PL_VertexBuffer_SetData(int vboHandle, const char *vertices, int start, int count) {
    VBufferData *vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL && vb->vboID > 0 && PL_GL.glBufferSubDataARB != NULL) {
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, vb->vboID);
        PL_GL.glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                                 vb->vertexByteSize * start,
                                 vb->vertexByteSize * count,
                                 vertices);
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        return 0;
    }
    return -1;
}

char *PL_VertexBuffer_Lock(int vboHandle) {
    VBufferData *vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL && PL_GL.glMapBufferARB != NULL) {
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, vb->vboID);
        return (char *)PL_GL.glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    }
    return NULL;
}

int PL_VertexBuffer_Unlock(int vboHandle, char *buffer) {
    VBufferData *vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL && PL_GL.glUnmapBufferARB != NULL) {
        PL_GL.glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
        PL_GL.glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        return 0;
    }
    return -1;
}

int PL_VertexBuffer_Delete(int vboHandle) {
    VBufferData *vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        if (PL_GL.glDeleteBuffersARB != NULL) {
            PL_GL.glDeleteBuffersARB(1, &vb->vboID);
        }
        
        PL_Handle_ReleaseID(vboHandle, DXTRUE);
        return 0;
    }
    return -1;
}

GLuint PL_VertexBuffer_GetGLID(int vboHandle) {
    VBufferData *vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        return vb->vboID;
    }
    return 0;
}

/* ------------------------------------------------------- Index Buffers */

typedef struct _IBufferData {
    GLuint iboID;
    
    int indexByteSize;
} IBufferData;

int PL_IndexBuffer_Create(const unsigned short *indexData, int indexCount,
                          int isStatic) {
    int byteSize = 2;
    GLuint iboID = 0;
    int iboHandle;
    IBufferData *ib;
    
    if (PL_GL.glGenBuffersARB != NULL) {
        GLenum bufferUsage = GL_DYNAMIC_DRAW_ARB;
        if (isStatic) {
            bufferUsage = GL_STATIC_DRAW_ARB;
        }
        
        PL_GL.glGenBuffersARB(1, &iboID);
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, iboID);
        PL_GL.glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                              indexCount * byteSize,
                              indexData, bufferUsage);
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
    
    iboHandle = PL_Handle_AcquireID(DXHANDLE_INDEXBUFFER);
    ib = (IBufferData *)PL_Handle_AllocateData(iboHandle, sizeof(IBufferData));
    ib->iboID = iboID;
    ib->indexByteSize = byteSize;
    
    return iboHandle;
}

int PL_IndexBuffer_SetData(int iboHandle, const unsigned short *indices, int start, int count) {
    IBufferData *ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL && ib->iboID > 0 && PL_GL.glBufferSubDataARB != NULL) {
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ib->iboID);
        PL_GL.glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                                 ib->indexByteSize * start,
                                 ib->indexByteSize * count,
                                 indices);
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        return 0;
    }
    return -1;
}

unsigned short *PL_IndexBuffer_Lock(int iboHandle) {
    IBufferData *ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL && PL_GL.glMapBufferARB != NULL) {
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, ib->iboID);
        return (unsigned short *)PL_GL.glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    }
    return NULL;
}

int PL_IndexBuffer_Unlock(int iboHandle) {
    IBufferData *ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL && PL_GL.glUnmapBufferARB != NULL) {
        PL_GL.glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
        PL_GL.glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
        return 0;
    }
    return -1;
}

int PL_IndexBuffer_Delete(int iboHandle) {
    IBufferData *ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) {
        if (PL_GL.glDeleteBuffersARB != NULL) {
            PL_GL.glDeleteBuffersARB(1, &ib->iboID);
        }
        
        PL_Handle_ReleaseID(iboHandle, DXTRUE);
        return 0;
    }
    return -1;
}

GLuint PL_IndexBuffer_GetGLID(int iboHandle) {
    IBufferData *ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) {
        return ib->iboID;
    }
    return 0;
}

#endif
