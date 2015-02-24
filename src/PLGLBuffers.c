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
    
    int bufferSize;
    GLenum bufferUsage;
    char *fallbackData; /* If VBOs are not supported, we emulate with this. */
} VBufferData;

int PL_VertexBuffer_CreateBytes(int vertexByteSize,
                                const char *vertexData, int bufferSize,
                                int isStatic) {
    GLuint vboID = 0;
    int vboHandle;
    VBufferData *vb;
    GLenum bufferUsage;
    
    vboHandle = PL_Handle_AcquireID(DXHANDLE_VERTEXBUFFER);
    vb = (VBufferData *)PL_Handle_AllocateData(vboHandle, sizeof(VBufferData));
    vb->vertexByteSize = vertexByteSize;
    vb->fallbackData = NULL;
    vb->bufferSize = bufferSize;
    
    bufferUsage = GL_STREAM_DRAW;
    if (isStatic) {
        bufferUsage = GL_STATIC_DRAW;
    }
    vb->bufferUsage = bufferUsage;
    
    if (PL_GL.hasVBOSupport == DXTRUE) {
        PL_GL.glGenBuffers(1, &vboID);
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vboID);
        PL_GL.glBufferData(GL_ARRAY_BUFFER,
                        vb->bufferSize,
                        vertexData, bufferUsage);
        PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
    } else {
        vb->fallbackData = DXALLOC(vb->bufferSize);
    }
    vb->vboID = vboID;
    
    return vboHandle;
}

int PL_VertexBuffer_Create(const VertexDefinition *def,
                           const char *vertexData, int vertexCount,
                           int isStatic) {
    return PL_VertexBuffer_CreateBytes(def->vertexByteSize, vertexData,
                                       vertexCount * def->vertexByteSize,
                                       isStatic);
}

int PL_VertexBuffer_ResetBuffer(int vboHandle) {
    VBufferData *vb;
    
    vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        if (vb->vboID > 0) {
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vb->vboID);
            PL_GL.glBufferData(GL_ARRAY_BUFFER, vb->bufferSize, NULL, vb->bufferUsage);
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    
    return 0;
}

int PL_VertexBuffer_SetDataBytes(int vboHandle, const char *vertices, int start, int count,
                                 int resetBufferFlag) {
    VBufferData *vb;
    
    vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        if (vb->vboID > 0) {
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vb->vboID);
            if (resetBufferFlag == DXTRUE) {
                PL_GL.glBufferData(GL_ARRAY_BUFFER, vb->bufferSize, NULL, vb->bufferUsage);
            }
            PL_GL.glBufferSubData(GL_ARRAY_BUFFER, start, count, vertices);
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        } else if (vb->fallbackData != NULL) {
            if ((start + count) <= vb->bufferSize) {
                memcpy(vb->fallbackData + start, vertices, count);
            }
        }
        return 0;
    }
    return -1;
}

int PL_VertexBuffer_SetData(int vboHandle, const char *vertices, int start, int count,
                            int resetBufferFlag) {
    VBufferData *vb;
    
    vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        if (vb->vboID > 0) {
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vb->vboID);
            if (resetBufferFlag == DXTRUE) {
                PL_GL.glBufferData(GL_ARRAY_BUFFER, vb->bufferSize, NULL, vb->bufferUsage);
            }
            PL_GL.glBufferSubData(GL_ARRAY_BUFFER,
                                vb->vertexByteSize * start,
                                vb->vertexByteSize * count,
                                vertices);
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
        } else if (vb->fallbackData != NULL) {
            int bufStart = vb->vertexByteSize * start;
            int bufSize = vb->vertexByteSize * count;
            
            if ((bufStart + bufSize) <= vb->bufferSize) {
                memcpy(vb->fallbackData + bufStart, vertices, bufSize);
            }
        }
        return 0;
    }
    return -1;
}

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
char *PL_VertexBuffer_Lock(int vboHandle) {
    VBufferData *vb;
    
    vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        if (PL_GL.hasVBOSupport == DXTRUE && PL_GL.glMapBuffer != NULL) {
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, vb->vboID);
            return (char *)PL_GL.glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        } else if (vb->fallbackData != NULL) {
            return vb->fallbackData;
        }
    }
    return NULL;
}
#endif

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
int PL_VertexBuffer_Unlock(int vboHandle, char *buffer) {
    VBufferData *vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        if (PL_GL.hasVBOSupport == DXTRUE && PL_GL.glUnmapBuffer != NULL) {
            PL_GL.glUnmapBuffer(GL_ARRAY_BUFFER);
            PL_GL.glBindBuffer(GL_ARRAY_BUFFER, 0);
            return 0;
        } else if (vb->fallbackData == buffer) {
            return 0;
        }
    }
    return -1;
}
#endif

int PL_VertexBuffer_Delete(int vboHandle) {
    VBufferData *vb;
    
    vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        if (vb->vboID > 0 && PL_GL.hasVBOSupport == DXTRUE) {
            PL_GL.glDeleteBuffers(1, &vb->vboID);
        }
        if (vb->fallbackData != NULL) {
            DXFREE(vb->fallbackData);
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

char *PL_VertexBuffer_GetFallback(int vboHandle) {
    VBufferData *vb = (VBufferData *)PL_Handle_GetData(vboHandle, DXHANDLE_VERTEXBUFFER);
    if (vb != NULL) {
        return vb->fallbackData;
    }
    return 0;
}

/* ------------------------------------------------------- Index Buffers */

typedef struct _IBufferData {
    GLuint iboID;
    
    int indexByteSize;
    
    int bufferSize;
    GLenum bufferUsage;
    
    char *fallbackData; /* If VBOs are not supported, we emulate with this. */
} IBufferData;

int PL_IndexBuffer_Create(const unsigned short *indexData, int indexCount,
                          int isStatic) {
    int byteSize = 2;
    GLuint iboID = 0;
    int iboHandle;
    IBufferData *ib;
    GLenum bufferUsage;
    
    iboHandle = PL_Handle_AcquireID(DXHANDLE_INDEXBUFFER);
    ib = (IBufferData *)PL_Handle_AllocateData(iboHandle, sizeof(IBufferData));
    ib->indexByteSize = byteSize;
    ib->bufferSize = indexCount * byteSize;
    ib->fallbackData = NULL;
    
    bufferUsage = GL_STREAM_DRAW;
    if (isStatic) {
        bufferUsage = GL_STATIC_DRAW;
    }
    ib->bufferUsage = bufferUsage;
    
    if (PL_GL.hasVBOSupport == DXTRUE) {
        PL_GL.glGenBuffers(1, &iboID);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
        PL_GL.glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        ib->bufferSize,
                        indexData, bufferUsage);
        PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } else {
        ib->fallbackData = DXALLOC(ib->bufferSize);
    }
    
    ib->iboID = iboID;
    
    return iboHandle;
}

int PL_IndexBuffer_ResetBuffer(int iboHandle) {
    IBufferData *ib;
    
    ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) {
        if (ib->iboID > 0) {
            PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->iboID);
            PL_GL.glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->bufferSize, NULL, ib->bufferUsage);
            PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    
    return 0;
}

int PL_IndexBuffer_SetData(int iboHandle, const unsigned short *indices, int start, int count,
                           int resetBufferFlag) {
    IBufferData *ib;
    ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) {
        if (ib->iboID > 0) {
            PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->iboID);
            if (resetBufferFlag == DXTRUE) {
                PL_GL.glBufferData(GL_ELEMENT_ARRAY_BUFFER, ib->bufferSize, NULL, ib->bufferUsage);
            }
            PL_GL.glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                                    ib->indexByteSize * start,
                                    ib->indexByteSize * count,
                                    indices);
            PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return 0;
        } else if (ib->fallbackData != NULL) {
            int bufStart = ib->indexByteSize * start;
            int bufSize = ib->indexByteSize * count;
            
            if ((bufStart + bufSize) <= ib->bufferSize) {
                memcpy(ib->fallbackData + bufStart, indices, bufSize);
            }
        }
    }
    return -1;
}

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
unsigned short *PL_IndexBuffer_Lock(int iboHandle) {
    IBufferData *ib;
    ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) { 
        if (PL_GL.hasVBOSupport == DXTRUE && PL_GL.glMapBuffer != NULL) {
            PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->iboID);
            return (unsigned short *)PL_GL.glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        } else if (ib->fallbackData != NULL) {
            return (unsigned short *)ib->fallbackData;
        }
    }
    return NULL;
}
#endif

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
int PL_IndexBuffer_Unlock(int iboHandle) {
    IBufferData *ib;
    ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) {
        if (PL_GL.hasVBOSupport == DXTRUE && PL_GL.glUnmapBuffer != NULL) {
            PL_GL.glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            PL_GL.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return 0;
        } else if (ib->fallbackData != NULL) {
            return 0;
        }
    }
    return -1;
}
#endif

int PL_IndexBuffer_Delete(int iboHandle) {
    IBufferData *ib;
    if (PL_GL.hasVBOSupport == DXFALSE) {
        return -1;
    }
    
    ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) {
        if (PL_GL.hasVBOSupport == DXTRUE && ib->iboID > 0) {
            PL_GL.glDeleteBuffers(1, &ib->iboID);
        }
        if (ib->fallbackData != NULL) {
            DXFREE(ib->fallbackData);
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

char *PL_IndexBuffer_GetFallback(int iboHandle) {
    IBufferData *ib = (IBufferData *)PL_Handle_GetData(iboHandle, DXHANDLE_INDEXBUFFER);
    if (ib != NULL) {
        return ib->fallbackData;
    }
    return 0;
}

#endif
