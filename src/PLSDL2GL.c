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

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_PLATFORM_SDL2
#ifdef DXPORTLIB_DRAW_OPENGL

#include "PLInternal.h"
#include "PLGLInternal.h"
#include "PLSDL2Internal.h"

static SDL_GLContext *s_context = NULL;

static int s_screenFrameBufferA = -1;
static int s_screenFrameBufferB = -1;

int PL_drawScreenWidth = -1;
int PL_drawScreenHeight = -1;

GLInfo PL_GL = { 0 };

/* ------------------------------------------------------- Load Functions */
static void s_debugPrint(const char *string) {
    fprintf(stderr, "%s\n", string);
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
        PL_GL.useVBOARB = DXTRUE;
        
        PL_GL.glGenBuffersARB = SDL_GL_GetProcAddress("glGenBuffersARB");
        PL_GL.glDeleteBuffersARB = SDL_GL_GetProcAddress("glDeleteBuffersARB");
        PL_GL.glBufferDataARB = SDL_GL_GetProcAddress("glBufferDataARB");
        PL_GL.glBufferSubDataARB = SDL_GL_GetProcAddress("glBufferSubDataARB");
        PL_GL.glMapBufferARB = SDL_GL_GetProcAddress("glMapBufferARB");
        PL_GL.glUnmapBufferARB = SDL_GL_GetProcAddress("glUnmapBufferARB");
        PL_GL.glBindBufferARB = SDL_GL_GetProcAddress("glBindBufferARB");
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
        PL_GL.useFramebufferEXT = DXTRUE;
        
        PL_GL.glFramebufferTexture2DEXT = SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
        PL_GL.glBindFramebufferEXT = SDL_GL_GetProcAddress("glBindFramebufferEXT");
        PL_GL.glDeleteFramebuffersEXT = SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
        PL_GL.glGenFramebuffersEXT = SDL_GL_GetProcAddress("glGenFramebuffersEXT");
        PL_GL.glCheckFramebufferStatusEXT = SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
        s_debugPrint("s_LoadGL: using GL_EXT_framebuffer_object");
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
    PL_GL.glAttachShader = SDL_GL_GetProcAddress("glAttachShader");
    
    PL_GL.glEnableVertexAttribArray = SDL_GL_GetProcAddress("glEnableVertexAttribArray");
    PL_GL.glDisableVertexAttribArray = SDL_GL_GetProcAddress("glDisableVertexAttribArray");
    PL_GL.glVertexAttribPointer = SDL_GL_GetProcAddress("glVertexAttribPointer");
    PL_GL.glUniform1i = SDL_GL_GetProcAddress("glUniform1i");
    PL_GL.glUniformMatrix4fv = SDL_GL_GetProcAddress("glUniformMatrix4fv");
    
    /* help this is wrong. For ES2 and OGL >2.0 I should assume correct, otherwise...
     * Should I support the ARB version as well? */
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
#endif
    
    PL_GL.isInitialized = DXTRUE;
}

/* ------------------------------------------------------- Window context */

int PL_Window_GetFramebuffer() {
    return s_screenFrameBufferA;
}

void PL_SDL2GL_ResizeWindow(int width, int height) {
    if (!PL_GL.isInitialized) {
        return;
    }
    
    if (width == PL_drawScreenWidth && height == PL_drawScreenHeight) {
        return;
    }
    
    PL_drawScreenWidth = width;
    PL_drawScreenHeight = height;
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        return;
    }
    
    /* Reinitialize our target backbuffers */
    PL_Texture_Release(s_screenFrameBufferA);
    PL_Texture_Release(s_screenFrameBufferB);
    
    s_screenFrameBufferA = PL_Texture_CreateFramebuffer(width, height, DXFALSE);
    PL_Texture_AddRef(s_screenFrameBufferA);
    s_screenFrameBufferB = PL_Texture_CreateFramebuffer(width, height, DXFALSE);
    PL_Texture_AddRef(s_screenFrameBufferB);
    
    PL_GL.glClearColor(0, 0, 0, 1);
    PL_Texture_BindFramebuffer(s_screenFrameBufferB);
    PL_GL.glClear(GL_COLOR_BUFFER_BIT);
    
    PL_Texture_BindFramebuffer(s_screenFrameBufferA);
    PL_GL.glClear(GL_COLOR_BUFFER_BIT);
}

/* FIXME genericize this code a bit... */
typedef struct RectVertex {
    float x, y;
    float tcx, tcy;
} RectVertex;

static void s_drawRect(const SDL_Rect *rect) {
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    float x1 = (float)rect->x;
    float y1 = (float)rect->y;
    float x2 = x1 + (float)rect->w;
    float y2 = y1 + (float)rect->h;
    RectVertex v[4];
    float tcx1, tcy1, tcx2, tcy2;
    SDL_Rect texRect;
    float xMult, yMult;
    
    PL_GL.glActiveTexture(GL_TEXTURE0);
    PL_GL.glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    PL_Texture_Bind(s_screenFrameBufferB, DX_DRAWMODE_BILINEAR);
    
    PL_Texture_RenderGetTextureInfo(s_screenFrameBufferB, &texRect, &xMult, &yMult);

    tcx1 = (float)texRect.x * xMult;
    tcy1 = (float)texRect.y * yMult;
    tcx2 = tcx1 + ((float)texRect.w * xMult);
    tcy2 = tcy1 + ((float)texRect.h * yMult);
    
    v[0].x = x1; v[0].y = y1; v[0].tcx = tcx1; v[0].tcy = tcy1;
    v[1].x = x2; v[1].y = y1; v[1].tcx = tcx2; v[1].tcy = tcy1;
    v[2].x = x1; v[2].y = y2; v[2].tcx = tcx1; v[2].tcy = tcy2;
    v[3].x = x2; v[3].y = y2; v[3].tcx = tcx2; v[3].tcy = tcy2;
    
    PL_GL.glColor4f(1, 1, 1, 1);
    
    PL_GL.glEnableClientState(GL_VERTEX_ARRAY);
    PL_GL.glVertexPointer(2, GL_FLOAT, sizeof(RectVertex), (unsigned char *)v + offsetof(RectVertex, x));
    PL_GL.glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    PL_GL.glTexCoordPointer(2, GL_FLOAT, sizeof(RectVertex), (unsigned char *)v + offsetof(RectVertex, tcx));
    
    PL_GL.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    PL_GL.glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    PL_GL.glDisableClientState(GL_VERTEX_ARRAY);
    
    PL_Texture_Unbind(s_screenFrameBufferB);
#endif
}

void PL_SDL2GL_Refresh(SDL_Window *window, const SDL_Rect *targetRect) {
    int wWidth, wHeight;
    PLMatrix matrix;
    
    if (!PL_GL.isInitialized) {
        return;
    }
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        /* without framebuffers, we can't actually refresh. */
        SDL_GL_SwapWindow(window);
        return;
    }
    
    /* PL_Draw_FlushCache(); */
    
    /* Set up the main screen for drawing. */
    PL_Texture_BindFramebuffer(-1);
    
    SDL_GetWindowSize(window, &wWidth, &wHeight);
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glDisable(GL_DEPTH_TEST);
    PL_GL.glDisable(GL_CULL_FACE);
    
    PL_GL.glDisable(GL_SCISSOR_TEST);
    
    PL_Matrix_CreateOrthoOffCenterRH(&matrix,
        0, (float)wWidth, (float)wHeight, 0, 0.0, 1.0f);
    
    PL_GL.glViewport(0, 0, wWidth, wHeight);
    
    PL_GL.glMatrixMode(GL_PROJECTION);
    PL_GL.glLoadMatrixf((float *)&matrix);
    
    PL_GL.glMatrixMode(GL_MODELVIEW);
    PL_GL.glLoadIdentity();
    
    PL_GL.glClearColor(0, 0, 0, 1);
    PL_GL.glClear(GL_COLOR_BUFFER_BIT);
    
    s_drawRect(targetRect);
#endif
    
    /* Swap! */
    SDL_GL_SwapWindow(window);
    
    /* Rebind the new buffer. */
    PL_Texture_BindFramebuffer(s_screenFrameBufferA);
    
    PL_Render_SetMatrixDirtyFlag();
}

void PL_SDL2GL_SwapBuffers(SDL_Window *window, const SDL_Rect *targetRect) {
    int tempBuffer;
    if (!PL_GL.isInitialized) {
        return;
    }
    
    //PL_Draw_FlushCache();
    
    tempBuffer = s_screenFrameBufferB;
    s_screenFrameBufferB = s_screenFrameBufferA;
    s_screenFrameBufferA = tempBuffer;
    
    PL_SDL2GL_Refresh(window, targetRect);
}

void PL_SDL2GL_SetAttributes() {
#ifdef DXPORTLIB_DRAW_OPENGL_ES2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
}

void PL_SDL2GL_Init(SDL_Window *window, int width, int height, int vsyncFlag) {
    if (PL_GL.isInitialized) {
        return;
    }
    
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
    
    PL_SDL2GL_ResizeWindow(width, height);
    PL_GL.glClearColor(0, 0, 0, 1);
    PL_GL.glClear(GL_COLOR_BUFFER_BIT);
}

void PL_SDL2GL_End() {
    PL_Render_End();
    
    if (s_context != NULL) {
        PL_Texture_Release(s_screenFrameBufferA);
        PL_Texture_Release(s_screenFrameBufferB);
        
        PL_Texture_ClearAllData();
        
        SDL_GL_DeleteContext(s_context);
    }
    
    PL_drawScreenWidth = -1;
    PL_drawScreenHeight = -1;
    
    SDL_memset(&PL_GL, 0, sizeof(PL_GL));
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
#endif /* #ifdef DXPORTLIB_PLATFORM_SDL2 */
