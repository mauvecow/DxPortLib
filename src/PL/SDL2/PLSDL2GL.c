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

#ifdef DXPORTLIB_PLATFORM_SDL2
#ifdef DXPORTLIB_DRAW_OPENGL

#include "PL/PLInternal.h"
#include "PL/GL/PLGLInternal.h"
#include "PLSDL2Internal.h"

static SDL_GLContext *s_context = NULL;

GLInfo PL_GL = { 0 };

/* ------------------------------------------------------- Load Functions */
static void s_debugPrint(const char *string) {
    /* fprintf(stderr, "%s\n", string); */
}

static void s_LoadGL() {
    SDL_memset(&PL_GL, 0, sizeof(PL_GL));
    
    PL_GL.glGetError = SDL_GL_GetProcAddress("glGetError");
    PL_GL.glEnable = SDL_GL_GetProcAddress("glEnable");
    PL_GL.glDisable = SDL_GL_GetProcAddress("glDisable");
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glEnableClientState = SDL_GL_GetProcAddress("glEnableClientState");
    PL_GL.glDisableClientState = SDL_GL_GetProcAddress("glDisableClientState");
#endif
    
    PL_GL.glGetError = SDL_GL_GetProcAddress("glGetError");
    PL_GL.glPixelStorei = SDL_GL_GetProcAddress("glPixelStorei");
    PL_GL.glFinish = SDL_GL_GetProcAddress("glFinish");
    PL_GL.glGetIntegerv = SDL_GL_GetProcAddress("glGetIntegerv");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glMatrixMode = SDL_GL_GetProcAddress("glMatrixMode");
    PL_GL.glLoadIdentity = SDL_GL_GetProcAddress("glLoadIdentity");
    PL_GL.glLoadMatrixf = SDL_GL_GetProcAddress("glLoadMatrixf");
#endif
    
    PL_GL.glGenTextures = SDL_GL_GetProcAddress("glGenTextures");
    PL_GL.glDeleteTextures = SDL_GL_GetProcAddress("glDeleteTextures");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glClientActiveTexture = SDL_GL_GetProcAddress("glClientActiveTexture");
#endif
    PL_GL.glActiveTexture = SDL_GL_GetProcAddress("glActiveTexture");
    PL_GL.glBindTexture = SDL_GL_GetProcAddress("glBindTexture");
    PL_GL.glTexParameteri = SDL_GL_GetProcAddress("glTexParameteri");
    PL_GL.glTexImage2D = SDL_GL_GetProcAddress("glTexImage2D");
    PL_GL.glTexSubImage2D = SDL_GL_GetProcAddress("glTexSubImage2D");
    PL_GL.glReadPixels = SDL_GL_GetProcAddress("glReadPixels");
    
    PL_GL.glClearColor = SDL_GL_GetProcAddress("glClearColor");
    PL_GL.glClear = SDL_GL_GetProcAddress("glClear");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glColor4f = SDL_GL_GetProcAddress("glColor4f");
#endif
    
    PL_GL.glLineWidth = SDL_GL_GetProcAddress("glLineWidth");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glTexEnvf = SDL_GL_GetProcAddress("glTexEnvf");
    PL_GL.glTexEnvi = SDL_GL_GetProcAddress("glTexEnvi");
#endif
    PL_GL.glBlendFuncSeparate = SDL_GL_GetProcAddress("glBlendFuncSeparate");
    PL_GL.glBlendFunc = SDL_GL_GetProcAddress("glBlendFunc");
    PL_GL.glBlendEquationSeparate = SDL_GL_GetProcAddress("glBlendEquationSeparate");
    PL_GL.glBlendEquation = SDL_GL_GetProcAddress("glBlendEquation");
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glAlphaFunc = SDL_GL_GetProcAddress("glAlphaFunc");
#endif
    
    PL_GL.glViewport = SDL_GL_GetProcAddress("glViewport");
    PL_GL.glScissor = SDL_GL_GetProcAddress("glScissor");
    
    PL_GL.glDrawArrays = SDL_GL_GetProcAddress("glDrawArrays");
    PL_GL.glDrawElements = SDL_GL_GetProcAddress("glDrawElements");

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glVertexPointer = SDL_GL_GetProcAddress("glVertexPointer");
    PL_GL.glColorPointer = SDL_GL_GetProcAddress("glColorPointer");
    PL_GL.glTexCoordPointer = SDL_GL_GetProcAddress("glTexCoordPointer");
#endif
    
    PL_GL.glGenBuffers = SDL_GL_GetProcAddress("glGenBuffers");
    PL_GL.glDeleteBuffers = SDL_GL_GetProcAddress("glDeleteBuffers");
    PL_GL.glBufferData = SDL_GL_GetProcAddress("glBufferData");
    PL_GL.glBufferSubData = SDL_GL_GetProcAddress("glBufferSubData");
    
    PL_GL.glBindBuffer = SDL_GL_GetProcAddress("glBindBuffer");
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glMapBuffer = SDL_GL_GetProcAddress("glMapBuffer");
    PL_GL.glUnmapBuffer = SDL_GL_GetProcAddress("glUnmapBuffer");
#endif
    
    if (PL_GL.glGenBuffers != 0 && PL_GL.glDeleteBuffers != 0
        && PL_GL.glBufferData != 0 && PL_GL.glBufferSubData != 0
        && PL_GL.glBindBuffer != 0
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
        && PL_GL.glMapBuffer != 0 && PL_GL.glUnmapBuffer != 0
#endif
    ) {
        PL_GL.hasVBOSupport = DXTRUE;
        s_debugPrint("s_LoadGL: has VBO support");
    }
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    else if (SDL_GL_ExtensionSupported("GL_ARB_vertex_buffer_object")) {
        PL_GL.hasVBOSupport = DXTRUE;
        
        PL_GL.glGenBuffers = SDL_GL_GetProcAddress("glGenBuffersARB");
        PL_GL.glDeleteBuffers = SDL_GL_GetProcAddress("glDeleteBuffersARB");
        PL_GL.glBufferData = SDL_GL_GetProcAddress("glBufferDataARB");
        PL_GL.glBufferSubData = SDL_GL_GetProcAddress("glBufferSubDataARB");
        PL_GL.glMapBuffer = SDL_GL_GetProcAddress("glMapBufferARB");
        PL_GL.glUnmapBuffer = SDL_GL_GetProcAddress("glUnmapBufferARB");
        PL_GL.glBindBuffer = SDL_GL_GetProcAddress("glBindBufferARB");
        s_debugPrint("s_LoadGL: using GL_ARB_vertex_buffer_object");
    }
#endif

    PL_GL.glFramebufferTexture2D = SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
    PL_GL.glBindFramebuffer = SDL_GL_GetProcAddress("glBindFramebufferEXT");
    PL_GL.glDeleteFramebuffers = SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
    PL_GL.glGenFramebuffers = SDL_GL_GetProcAddress("glGenFramebuffersEXT");
    PL_GL.glCheckFramebufferStatus = SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
    
    if (PL_GL.glFramebufferTexture2D != 0 && PL_GL.glBindFramebuffer != 0
        && PL_GL.glDeleteFramebuffers != 0 && PL_GL.glGenFramebuffers != 0
        && PL_GL.glCheckFramebufferStatus != 0
    ) {
        PL_GL.hasFramebufferSupport = DXTRUE;
        s_debugPrint("s_LoadGL: has framebuffer support");
    }
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    else if (SDL_GL_ExtensionSupported("GL_EXT_framebuffer_object")) {
        PL_GL.hasFramebufferSupport = DXTRUE;
        
        PL_GL.glFramebufferTexture2D = SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
        PL_GL.glBindFramebuffer = SDL_GL_GetProcAddress("glBindFramebufferEXT");
        PL_GL.glDeleteFramebuffers = SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
        PL_GL.glGenFramebuffers = SDL_GL_GetProcAddress("glGenFramebuffersEXT");
        PL_GL.glCheckFramebufferStatus = SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
        s_debugPrint("s_LoadGL: using GL_EXT_framebuffer_object");
    }
#endif

    PL_drawOffscreen = DXFALSE;
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (PL_GL.hasFramebufferSupport == DXTRUE) {
        PL_drawOffscreen = DXTRUE;
    }
#endif
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (SDL_GL_ExtensionSupported("GL_ARB_texture_rectangle")
        || SDL_GL_ExtensionSupported("GL_EXT_texture_rectangle")
    ) {
        GLint size;
        PL_GL.glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &size);
        
        PL_GL.hasTextureRectangleSupport = DXTRUE;
        PL_GL.hasNPTSupport = DXTRUE;
        PL_GL.maxTextureWidth = size;
        PL_GL.maxTextureHeight = size;
        s_debugPrint("s_LoadGL: using GL_ARB_texture_rectangle");
    } else {
        GLint size;
        PL_GL.glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
        
        PL_GL.hasTextureRectangleSupport = DXFALSE;
        PL_GL.hasNPTSupport = DXFALSE;
        PL_GL.maxTextureWidth = size;
        PL_GL.maxTextureHeight = size;
        s_debugPrint("s_LoadGL: NO NPT or GL_ARB_texture_rectangle");
    }
#else
    {
        GLint size;
        PL_GL.glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
        
        PL_GL.hasTextureRectangleSupport = DXFALSE;
        PL_GL.hasNPTSupport = DXTRUE;
        PL_GL.maxTextureWidth = size;
        PL_GL.maxTextureHeight = size;
        s_debugPrint("s_LoadGL: has NPT support");
    }
#endif

    PL_GL.glCreateShader = SDL_GL_GetProcAddress("glCreateShader");
    PL_GL.glDeleteShader = SDL_GL_GetProcAddress("glDeleteShader");
    PL_GL.glShaderSource = SDL_GL_GetProcAddress("glShaderSource");
    PL_GL.glCompileShader = SDL_GL_GetProcAddress("glCompileShader");
    PL_GL.glGetShaderiv = SDL_GL_GetProcAddress("glGetShaderiv");
    PL_GL.glGetUniformLocation = SDL_GL_GetProcAddress("glGetUniformLocation");
    PL_GL.glGetAttribLocation = SDL_GL_GetProcAddress("glGetAttribLocation");
    
    PL_GL.glCreateProgram = SDL_GL_GetProcAddress("glCreateProgram");
    PL_GL.glDeleteProgram = SDL_GL_GetProcAddress("glDeleteProgram");
    PL_GL.glUseProgram = SDL_GL_GetProcAddress("glUseProgram");
    PL_GL.glLinkProgram = SDL_GL_GetProcAddress("glLinkProgram");
    PL_GL.glAttachShader = SDL_GL_GetProcAddress("glAttachShader");
    
    PL_GL.glEnableVertexAttribArray = SDL_GL_GetProcAddress("glEnableVertexAttribArray");
    PL_GL.glDisableVertexAttribArray = SDL_GL_GetProcAddress("glDisableVertexAttribArray");
    PL_GL.glVertexAttribPointer = SDL_GL_GetProcAddress("glVertexAttribPointer");
    PL_GL.glUniform1i = SDL_GL_GetProcAddress("glUniform1i");
    PL_GL.glUniformMatrix4fv = SDL_GL_GetProcAddress("glUniformMatrix4fv");
    
    /* help this is wrong. For ES2 and OGL >2.0 I should assume correct, otherwise...
     * Should I support the ARB version as well, just in case? */
    if (PL_GL.glCreateShader != 0 && PL_GL.glDeleteShader != 0
        && PL_GL.glShaderSource != 0 && PL_GL.glCompileShader != 0
        && PL_GL.glGetShaderiv != 0 && PL_GL.glGetUniformLocation != 0
        && PL_GL.glGetAttribLocation != 0 && PL_GL.glCreateProgram != 0
        && PL_GL.glDeleteProgram != 0 && PL_GL.glUseProgram != 0
        && PL_GL.glAttachShader != 0 && PL_GL.glEnableVertexAttribArray != 0
        && PL_GL.glDisableVertexAttribArray != 0 && PL_GL.glVertexAttribPointer != 0
        && PL_GL.glUniform1i != 0 && PL_GL.glUniformMatrix4fv != 0)
    {
        s_debugPrint("s_LoadGL: has shader support");
        
        PL_GL.hasShaderSupport = DXTRUE;
    }
    
    /* ES2 stuff */
#ifdef DXPORTLIB_DRAW_OPENGL_ES2
    if (SDL_GL_ExtensionSupported("GL_EXT_unpack_subimage")) {
        PL_GL.hasEXTUnpackSubimage = DXTRUE;
        s_debugPrint("s_LoadGL: has GL_EXT_unpack_subimage");
    }
    if (SDL_GL_ExtensionSupported("GL_EXT_texture_format_BGRA8888")) {
        PL_GL.hasEXTBGRA = DXTRUE;
        s_debugPrint("s_LoadGL: has GL_EXT_texture_format_BGRA8888");
    }
#endif
    
    PL_GL.isInitialized = DXTRUE;
}

/* ------------------------------------------------------- Window context */

void PL_SDL2GL_Init(SDL_Window *window, int width, int height, int vsyncFlag) {
    if (PL_GL.isInitialized) {
        return;
    }
    
#ifdef DXPORTLIB_DRAW_OPENGL_ES2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    
    /* Initialize the opengl context */
    s_context = SDL_GL_CreateContext(window);
    if (s_context == NULL) {
        return;
    }
    
    if (SDL_GL_MakeCurrent(window, s_context) < 0) {
        PL_SDL2GL_End();
        return;
    }
    
    SDL_GL_SetSwapInterval((vsyncFlag != DXFALSE) ? 1 : 0);
    
    s_LoadGL();
    
    PL_Render_Init();
}

void PL_SDL2GL_End() {
    PL_Render_End();
    
    if (s_context != NULL) {
        PL_Texture_ClearAllData();
        
        SDL_GL_DeleteContext(s_context);
    }
    
    PL_drawScreenWidth = -1;
    PL_drawScreenHeight = -1;
    
    SDL_memset(&PL_GL, 0, sizeof(PL_GL));
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
#endif /* #ifdef DXPORTLIB_PLATFORM_SDL2 */
