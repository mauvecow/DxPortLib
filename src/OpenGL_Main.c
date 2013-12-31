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

#include "DxInternal.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "OpenGL_DxInternal.h"

static SDL_GLContext *s_context = NULL;

static int s_screenFrameBufferA = -1;
static int s_screenFrameBufferB = -1;

static int s_screenWidth = -1;
static int s_screenHeight = -1;

GLInfo PL_GL = { 0 };

/* ------------------------------------------------------- Load Functions */
static void s_LoadGL() {
    SDL_memset(&PL_GL, 0, sizeof(PL_GL));
    
    PL_GL.glEnable = SDL_GL_GetProcAddress("glEnable");
    PL_GL.glDisable = SDL_GL_GetProcAddress("glDisable");
    PL_GL.glGetError = SDL_GL_GetProcAddress("glGetError");
    PL_GL.glPixelStorei = SDL_GL_GetProcAddress("glPixelStorei");
    PL_GL.glFinish = SDL_GL_GetProcAddress("glFinish");
    PL_GL.glGetIntegerv = SDL_GL_GetProcAddress("glGetIntegerv");
    
    PL_GL.glGenTextures = SDL_GL_GetProcAddress("glGenTextures");
    PL_GL.glDeleteTextures = SDL_GL_GetProcAddress("glDeleteTextures");
    
    PL_GL.glBindTexture = SDL_GL_GetProcAddress("glBindTexture");
    PL_GL.glTexParameteri = SDL_GL_GetProcAddress("glTexParameteri");
    PL_GL.glTexImage2D = SDL_GL_GetProcAddress("glTexImage2D");
    PL_GL.glTexSubImage2D = SDL_GL_GetProcAddress("glTexSubImage2D");
    
    PL_GL.glClearColor = SDL_GL_GetProcAddress("glClearColor");
    PL_GL.glClear = SDL_GL_GetProcAddress("glClear");
    
    PL_GL.glColor4f = SDL_GL_GetProcAddress("glColor4f");
    PL_GL.glTexEnvf = SDL_GL_GetProcAddress("glTexEnvf");
    PL_GL.glBlendFuncSeparatei = SDL_GL_GetProcAddress("glBlendFuncSeparatei");
    
    PL_GL.glViewport = SDL_GL_GetProcAddress("glViewport");
    
    PL_GL.glDrawArrays = SDL_GL_GetProcAddress("glDrawArrays");
    PL_GL.glEnableVertexAttribArray = SDL_GL_GetProcAddress("glEnableVertexAttribArray");
    PL_GL.glDisableVertexAttribArray = SDL_GL_GetProcAddress("glDisableVertexAttribArray" );
    
    if (SDL_GL_ExtensionSupported("GL_EXT_framebuffer_object")) {
        PL_GL.hasFramebufferSupport = DXTRUE;
        
        PL_GL.glFramebufferTexture2DEXT = SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
        PL_GL.glBindFramebufferEXT = SDL_GL_GetProcAddress("glBindFramebufferEXT");
        PL_GL.glDeleteFramebuffersEXT = SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
        PL_GL.glGenFramebuffersEXT = SDL_GL_GetProcAddress("glGenFramebuffersEXT");
        PL_GL.glCheckFramebufferStatusEXT = SDL_GL_GetProcAddress("glCheckFramebufferStatusEXT");
    }
    
    if (SDL_GL_ExtensionSupported("GL_ARB_texture_rectangle")
        || SDL_GL_ExtensionSupported("GL_EXT_texture_rectangle")
    ) {
        GLint size;
        PL_GL.glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, &size);
        
        PL_GL.hasTextureRectangleSupport = DXTRUE;
        PL_GL.maxTextureWidth = size;
        PL_GL.maxTextureHeight = size;
    } else {
        GLint size;
        PL_GL.glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
        
        PL_GL.hasTextureRectangleSupport = DXFALSE;
        PL_GL.maxTextureWidth = size;
        PL_GL.maxTextureHeight = size;
    }
    
    PL_GL.isInitialized = DXTRUE;
}

/* ------------------------------------------------------- Window context */

void PL_Draw_ResizeWindow(int width, int height) {
    if (!PL_GL.isInitialized) {
        return;
    }
    
    if (width == s_screenWidth && height == s_screenHeight) {
        return;
    }
    
    s_screenWidth = width;
    s_screenHeight = height;
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        return;
    }
    
    /* Reinitialize our target backbuffers */
    PL_Texture_Release(s_screenFrameBufferA);
    PL_Texture_Release(s_screenFrameBufferB);
    
    s_screenFrameBufferA = PL_Texture_CreateFramebuffer(width, height);
    s_screenFrameBufferB = PL_Texture_CreateFramebuffer(width, height);
}

void PL_Draw_Refresh(SDL_Window *window, const SDL_Rect *targetRect) {
    if (!PL_GL.isInitialized) {
        return;
    }
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        /* without framebuffers, we can't actually refresh. */
        return;
    }
    
    /* Redraw the screen */
    PL_Texture_BindFramebuffer(-1);
    
    SDL_GL_SwapWindow(window);
    
    PL_Texture_BindFramebuffer(s_screenFrameBufferA);
}

void PL_Draw_SwapBuffers(SDL_Window *window, const SDL_Rect *targetRect) {
    int tempBuffer;
    
    if (!PL_GL.isInitialized) {
        return;
    }
    
    PL_Draw_FlushCache();
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        /* Without framebuffer support, we're drawing straight to the screen,
         * so just swap the buffers. */
        SDL_GL_SwapWindow(window);
        return;
    } else {
        /* Swap our two backbuffers, refresh */
        tempBuffer = s_screenFrameBufferB;
        s_screenFrameBufferB = s_screenFrameBufferA;
        s_screenFrameBufferA = tempBuffer;
    
        PL_Draw_Refresh(window, targetRect);
    }
}

void PL_Draw_Init(SDL_Window *window, int width, int height, int vsyncFlag) {
    if (PL_GL.isInitialized) {
        return;
    }
    
    /* Initialize the opengl context */
    s_context = SDL_GL_CreateContext(window);
    if (s_context == NULL) {
        return;
    }
    
    if (SDL_GL_MakeCurrent(window, s_context) < 0) {
        PL_Draw_End();
        return;
    }
    
    SDL_GL_SetSwapInterval((vsyncFlag != DXFALSE) ? 1 : 0);
    
    s_LoadGL();
    
    PL_Draw_ResizeWindow(width, height);
}

void PL_Draw_End() {
    if (s_context != NULL) {
        PL_Texture_ClearAllData();
        
        PL_Texture_Release(s_screenFrameBufferA);
        PL_Texture_Release(s_screenFrameBufferB);
        
        SDL_GL_DeleteContext(s_context);
    }
    
    s_screenWidth = -1;
    s_screenHeight = -1;
    
    SDL_memset(&PL_GL, 0, sizeof(PL_GL));
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
