/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013 Patrick McCarthy <mauve@sandwich.net>
  
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

#ifndef DXLIB_OPENGL_DXINTERNAL_H_HEADER
#define DXLIB_OPENGL_DXINTERNAL_H_HEADER

#include "DxInternal.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "SDL.h"
#include "SDL_opengl.h"

#ifdef __MACOSX__
#include <OpenGL/OpenGL.h>
#endif

typedef struct GLInfo_t {
    int isInitialized;
    
    int hasTextureRectangleSupport;
    int maxTextureWidth;
    int maxTextureHeight;

    /* Main GL functions */
    void (APIENTRY *glEnable)(GLenum cap);
    void (APIENTRY *glDisable)(GLenum cap);
    GLenum (APIENTRY *glGetError)(void);
    void (APIENTRY *glPixelStorei)( GLenum pname, GLint param );
    void (APIENTRY *glFinish)(void);
    void (APIENTRY *glGetIntegerv)( GLenum pname, GLint *params );
    
    /* Texture functions */
    void (APIENTRY *glGenTextures)( GLsizei n, GLuint *textures );
    void (APIENTRY *glDeleteTextures)( GLsizei n, const GLuint *textures);

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

    /* Drawing functions */
    void (APIENTRY *glClearColor)( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
    void (APIENTRY *glClear)( GLbitfield mask );
    
    void (APIENTRY *glColor4f)( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
    
    void (APIENTRY *glTexEnvf)( GLenum target, GLenum pname, GLfloat param );
    void (APIENTRY *glBlendFuncSeparatei) ( GLuint buf,
                                              GLenum srcRGB, GLenum dstRGB,
                                              GLenum srcAlpha, GLenum dstAlpha);
    
    void (APIENTRY *glViewport)( GLint x, GLint y, GLsizei width, GLsizei height );
    void (APIENTRY *glScissor)( GLint x, GLint y, GLsizei width, GLsizei height );
    
    void (APIENTRY *glDrawArrays)( GLenum mode, GLint first, GLsizei count );
 
    void (APIENTRY *glEnableVertexAttribArray) (GLuint index);   
    void (APIENTRY *glDisableVertexAttribArray) (GLuint index);   
    
    /* Framebuffer functions */
    int hasFramebufferSupport;
    
    void (APIENTRY *glFramebufferTexture2DEXT) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    void (APIENTRY *glBindFramebufferEXT) (GLenum target, GLuint framebuffer);
    void (APIENTRY *glDeleteFramebuffersEXT) (GLsizei n, const GLuint *framebuffers);
    void (APIENTRY *glGenFramebuffersEXT) (GLsizei n, GLuint *framebuffers);
    GLenum (APIENTRY *glCheckFramebufferStatusEXT) (GLenum target);
} GLInfo;

extern GLInfo PL_GL;

extern int PL_Draw_FlushCache();

extern int PL_Texture_CreateFramebuffer(int width, int height);
extern int PL_Texture_BindFramebuffer(int textureRefID);
extern int PL_Texture_RenderGetGraphTexture(int graphID, GLuint *textureID, SDL_Rect *rect);
extern int PL_Texture_ClearAllData();

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */

#endif /* #ifndef _DXLIB_OPENGL_DXINTERNAL_H */

