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

#ifndef DXPORTLIB_OPENGL_DXINTERNAL_H_HEADER
#define DXPORTLIB_OPENGL_DXINTERNAL_H_HEADER

#include "PLInternal.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "SDL.h"
#include "SDL_opengl.h"

#ifdef __MACOSX__
#include <OpenGL/OpenGL.h>
#endif

/* In the event this is not defined... */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

typedef struct GLInfo_t {
    int isInitialized;
    
    int hasTextureRectangleSupport;
    int maxTextureWidth;
    int maxTextureHeight;

    /* Main GL functions */
    void (APIENTRY *glEnable)( GLenum cap );
    void (APIENTRY *glDisable)( GLenum cap );
    void (APIENTRY *glEnableClientState)( GLenum cap );
    void (APIENTRY *glDisableClientState)( GLenum cap );
    
    GLenum (APIENTRY *glGetError)(void);
    void (APIENTRY *glPixelStorei)( GLenum pname, GLint param );
    void (APIENTRY *glFinish)(void);
    void (APIENTRY *glGetIntegerv)( GLenum pname, GLint *params );
    
    void (APIENTRY *glMatrixMode)( GLenum mode );
    void (APIENTRY *glLoadIdentity)( void );
    /* We do not use the matrix stack. */
    /* void (APIENTRY *glPushMatrix)( void ); */
    /* void (APIENTRY *glPopMatrix)( void ); */
    void (APIENTRY *glLoadMatrixf)( const GLfloat *m );
    /* We use PL's internal matrix support. */
    /* void (APIENTRY *glOrtho)( GLdouble left, GLdouble right,
                             GLdouble bottom, GLdouble top,
                             GLdouble near_val, GLdouble far_val ); */
    /* void (APIENTRY *glTranslatef)( GLfloat x, GLfloat y, GLfloat z ); */
    
    /* Texture functions */
    void (APIENTRY *glGenTextures)( GLsizei n, GLuint *textures );
    void (APIENTRY *glDeleteTextures)( GLsizei n, const GLuint *textures);

    void (APIENTRY *glActiveTexture)( GLenum texture );
    void (APIENTRY *glClientActiveTexture)( GLenum texture );
    void (APIENTRY *glBindTexture)( GLenum target, GLuint texture );
    void (APIENTRY *glTexParameteri)( GLenum target, GLenum pname, GLint param );
    void (APIENTRY *glTexImage2D)( GLenum target, GLint level,
                                     GLint internalFormat,
                                     GLsizei width, GLsizei height,
                                     GLint border, GLenum format, GLenum type,
                                     const GLvoid *pixels );
    void (APIENTRY *glTexSubImage2D)( GLenum target, GLint level,
                                        GLint xoffset, GLint yoffset,
                                        GLsizei width, GLsizei height,
                                        GLenum format, GLenum type,
                                        const GLvoid *pixels );
    void (APIENTRY *glReadPixels)( GLint x, GLint y,
                                   GLsizei width, GLsizei height,
                                   GLenum format, GLenum type,
                                   GLvoid *pixels );

    /* Drawing functions */
    void (APIENTRY *glClearColor)( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
    void (APIENTRY *glClear)( GLbitfield mask );
    
    void (APIENTRY *glColor4f)( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
    
    void (APIENTRY *glLineWidth)( GLfloat width );

    void (APIENTRY *glTexEnvf)( GLenum target, GLenum pname, GLfloat param );
    void (APIENTRY *glTexEnvi)( GLenum target, GLenum pname, GLint param );
    void (APIENTRY *glBlendFuncSeparate) ( GLenum srcRGB, GLenum dstRGB,
                                           GLenum srcAlpha, GLenum dstAlpha);
    void (APIENTRY *glBlendFunc) ( GLenum src, GLenum srcAlpha );
    void (APIENTRY *glBlendEquationSeparate) ( GLenum modeRGB, GLenum modeAlpha );
    void (APIENTRY *glBlendEquation) ( GLenum mode );
    void (APIENTRY *glAlphaFunc)( GLenum func, GLclampf ref );
    
    void (APIENTRY *glViewport)( GLint x, GLint y, GLsizei width, GLsizei height );
    void (APIENTRY *glScissor)( GLint x, GLint y, GLsizei width, GLsizei height );
    
    void (APIENTRY *glDrawArrays)( GLenum mode, GLint first, GLsizei count );
    void (APIENTRY *glDrawElements)( GLenum mode, GLsizei count,
                                     GLenum type, const GLvoid *indices );
    
    void (APIENTRY *glVertexPointer)( GLint size, GLenum type,  
                                      GLsizei stride, const GLvoid *ptr );  
    void (APIENTRY *glColorPointer)( GLint size, GLenum type,
                                     GLsizei stride, const GLvoid *ptr );
    void (APIENTRY *glTexCoordPointer)( GLint size, GLenum type,
                                        GLsizei stride, const GLvoid *ptr );
    
    /* Vertex buffer functions */
    void (APIENTRY *glBindBufferARB)( GLenum target, GLuint buffer );
    void (APIENTRY *glDeleteBuffersARB)( GLsizei n, const GLuint *buffers );
    void (APIENTRY *glGenBuffersARB)( GLsizei n, GLuint *buffers );
    void (APIENTRY *glBufferDataARB)( GLenum target, GLsizeiptrARB size,
                                      const GLvoid *data, GLenum usage );
    void (APIENTRY *glBufferSubDataARB)( GLenum target, GLintptrARB offset,
                                         GLsizeiptrARB size, const GLvoid *data );
    GLvoid *(APIENTRY *glMapBufferARB)( GLenum target, GLenum access );
    GLboolean (APIENTRY *glUnmapBufferARB)( GLenum target );

    /* Framebuffer functions */
    int hasFramebufferSupport;
    
    void (APIENTRY *glFramebufferTexture2DEXT) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    void (APIENTRY *glBindFramebufferEXT) (GLenum target, GLuint framebuffer);
    void (APIENTRY *glDeleteFramebuffersEXT) (GLsizei n, const GLuint *framebuffers);
    void (APIENTRY *glGenFramebuffersEXT) (GLsizei n, GLuint *framebuffers);
    GLenum (APIENTRY *glCheckFramebufferStatusEXT) (GLenum target);
} GLInfo;

extern GLInfo PL_GL;

extern int PL_drawScreenWidth;
extern int PL_drawScreenHeight;

extern int PL_Render_SetMatrixDirtyFlag();
extern int PL_Render_UpdateMatrices();

extern int PL_Texture_Bind(int textureRefID, int drawMode);
extern int PL_Texture_Unbind(int textureRefID);
extern int PL_Texture_ClearAllData();


extern int PL_Framebuffer_GetSurface(const SDL_Rect *rect, SDL_Surface **dSurface);

extern GLuint PL_VertexBuffer_GetGLID(int vertexBufferID);
extern GLuint PL_IndexBuffer_GetGLID(int vertexBufferID);

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */

#endif /* #ifndef DXPORTLIB_OPENGL_DXINTERNAL_H */

