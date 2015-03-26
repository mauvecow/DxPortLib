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

#ifdef DXPORTLIB_DRAW_OPENGL

#include "PLGLInternal.h"

GLInfo PL_GL = { 0 };

extern int PL_drawOffscreen;

static void s_debugPrint(const char *string) {
    /* fprintf(stderr, "%s\n", string); */
}

int PLGL_End() {
    PLGL_Render_End();
    
    PLGL_Texture_ClearAllData();
    
    memset(&PL_GL, 0, sizeof(PL_GL));
    
    return 0;
}

int PLGL_Init(PLGLGetGLFunction GetGLFunction,
              PLGLIsGLExtSupported IsGLExtSupported,
              int majorVersion, int minorVersion
             ) {
    SDL_memset(&PL_GL, 0, sizeof(PL_GL));
    
    PL_GL.glGetError = GetGLFunction("glGetError");
    PL_GL.glEnable = GetGLFunction("glEnable");
    PL_GL.glDisable = GetGLFunction("glDisable");
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glEnableClientState = GetGLFunction("glEnableClientState");
    PL_GL.glDisableClientState = GetGLFunction("glDisableClientState");
#endif
    
    PL_GL.glGetError = GetGLFunction("glGetError");
    PL_GL.glPixelStorei = GetGLFunction("glPixelStorei");
    PL_GL.glFinish = GetGLFunction("glFinish");
    PL_GL.glGetIntegerv = GetGLFunction("glGetIntegerv");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glMatrixMode = GetGLFunction("glMatrixMode");
    PL_GL.glLoadIdentity = GetGLFunction("glLoadIdentity");
    PL_GL.glLoadMatrixf = GetGLFunction("glLoadMatrixf");
#endif
    
    PL_GL.glGenTextures = GetGLFunction("glGenTextures");
    PL_GL.glDeleteTextures = GetGLFunction("glDeleteTextures");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glClientActiveTexture = GetGLFunction("glClientActiveTexture");
#endif
    PL_GL.glActiveTexture = GetGLFunction("glActiveTexture");
    PL_GL.glBindTexture = GetGLFunction("glBindTexture");
    PL_GL.glTexParameteri = GetGLFunction("glTexParameteri");
    PL_GL.glTexImage2D = GetGLFunction("glTexImage2D");
    PL_GL.glTexSubImage2D = GetGLFunction("glTexSubImage2D");
    PL_GL.glReadPixels = GetGLFunction("glReadPixels");
    
    PL_GL.glClearColor = GetGLFunction("glClearColor");
    PL_GL.glClear = GetGLFunction("glClear");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glColor4f = GetGLFunction("glColor4f");
#endif
    
    PL_GL.glLineWidth = GetGLFunction("glLineWidth");
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glTexEnvf = GetGLFunction("glTexEnvf");
    PL_GL.glTexEnvi = GetGLFunction("glTexEnvi");
#endif
    PL_GL.glBlendFuncSeparate = GetGLFunction("glBlendFuncSeparate");
    PL_GL.glBlendFunc = GetGLFunction("glBlendFunc");
    PL_GL.glBlendEquationSeparate = GetGLFunction("glBlendEquationSeparate");
    PL_GL.glBlendEquation = GetGLFunction("glBlendEquation");
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glAlphaFunc = GetGLFunction("glAlphaFunc");
#endif
    
    PL_GL.glViewport = GetGLFunction("glViewport");
    PL_GL.glScissor = GetGLFunction("glScissor");
    
    PL_GL.glDrawArrays = GetGLFunction("glDrawArrays");
    PL_GL.glDrawElements = GetGLFunction("glDrawElements");

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glVertexPointer = GetGLFunction("glVertexPointer");
    PL_GL.glColorPointer = GetGLFunction("glColorPointer");
    PL_GL.glTexCoordPointer = GetGLFunction("glTexCoordPointer");
#endif
    
    PL_GL.glGenBuffers = GetGLFunction("glGenBuffers");
    PL_GL.glDeleteBuffers = GetGLFunction("glDeleteBuffers");
    PL_GL.glBufferData = GetGLFunction("glBufferData");
    PL_GL.glBufferSubData = GetGLFunction("glBufferSubData");
    
    PL_GL.glBindBuffer = GetGLFunction("glBindBuffer");
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glMapBuffer = GetGLFunction("glMapBuffer");
    PL_GL.glUnmapBuffer = GetGLFunction("glUnmapBuffer");
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
    else if (IsGLExtSupported("GL_ARB_vertex_buffer_object")) {
        PL_GL.hasVBOSupport = DXTRUE;
        
        PL_GL.glGenBuffers = GetGLFunction("glGenBuffersARB");
        PL_GL.glDeleteBuffers = GetGLFunction("glDeleteBuffersARB");
        PL_GL.glBufferData = GetGLFunction("glBufferDataARB");
        PL_GL.glBufferSubData = GetGLFunction("glBufferSubDataARB");
        PL_GL.glMapBuffer = GetGLFunction("glMapBufferARB");
        PL_GL.glUnmapBuffer = GetGLFunction("glUnmapBufferARB");
        PL_GL.glBindBuffer = GetGLFunction("glBindBufferARB");
        s_debugPrint("s_LoadGL: using GL_ARB_vertex_buffer_object");
    }
#endif

    PL_GL.glFramebufferTexture2D = GetGLFunction("glFramebufferTexture2DEXT");
    PL_GL.glBindFramebuffer = GetGLFunction("glBindFramebufferEXT");
    PL_GL.glDeleteFramebuffers = GetGLFunction("glDeleteFramebuffersEXT");
    PL_GL.glGenFramebuffers = GetGLFunction("glGenFramebuffersEXT");
    PL_GL.glCheckFramebufferStatus = GetGLFunction("glCheckFramebufferStatusEXT");
    
    if (PL_GL.glFramebufferTexture2D != 0 && PL_GL.glBindFramebuffer != 0
        && PL_GL.glDeleteFramebuffers != 0 && PL_GL.glGenFramebuffers != 0
        && PL_GL.glCheckFramebufferStatus != 0
    ) {
        PL_GL.hasFramebufferSupport = DXTRUE;
        s_debugPrint("s_LoadGL: has framebuffer support");
    }
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    else if (IsGLExtSupported("GL_EXT_framebuffer_object")) {
        PL_GL.hasFramebufferSupport = DXTRUE;
        
        PL_GL.glFramebufferTexture2D = GetGLFunction("glFramebufferTexture2DEXT");
        PL_GL.glBindFramebuffer = GetGLFunction("glBindFramebufferEXT");
        PL_GL.glDeleteFramebuffers = GetGLFunction("glDeleteFramebuffersEXT");
        PL_GL.glGenFramebuffers = GetGLFunction("glGenFramebuffersEXT");
        PL_GL.glCheckFramebufferStatus = GetGLFunction("glCheckFramebufferStatusEXT");
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
    if (IsGLExtSupported("GL_ARB_texture_rectangle")
        || IsGLExtSupported("GL_EXT_texture_rectangle")
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

    PL_GL.glCreateShader = GetGLFunction("glCreateShader");
    PL_GL.glDeleteShader = GetGLFunction("glDeleteShader");
    PL_GL.glShaderSource = GetGLFunction("glShaderSource");
    PL_GL.glCompileShader = GetGLFunction("glCompileShader");
    PL_GL.glGetShaderiv = GetGLFunction("glGetShaderiv");
    PL_GL.glGetUniformLocation = GetGLFunction("glGetUniformLocation");
    PL_GL.glGetAttribLocation = GetGLFunction("glGetAttribLocation");
    
    PL_GL.glCreateProgram = GetGLFunction("glCreateProgram");
    PL_GL.glDeleteProgram = GetGLFunction("glDeleteProgram");
    PL_GL.glUseProgram = GetGLFunction("glUseProgram");
    PL_GL.glLinkProgram = GetGLFunction("glLinkProgram");
    PL_GL.glAttachShader = GetGLFunction("glAttachShader");
    
    PL_GL.glEnableVertexAttribArray = GetGLFunction("glEnableVertexAttribArray");
    PL_GL.glDisableVertexAttribArray = GetGLFunction("glDisableVertexAttribArray");
    PL_GL.glVertexAttribPointer = GetGLFunction("glVertexAttribPointer");
    PL_GL.glUniform1i = GetGLFunction("glUniform1i");
    PL_GL.glUniformMatrix4fv = GetGLFunction("glUniformMatrix4fv");
    
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
    if (IsGLExtSupported("GL_EXT_unpack_subimage")) {
        PL_GL.hasEXTUnpackSubimage = DXTRUE;
        s_debugPrint("s_LoadGL: has GL_EXT_unpack_subimage");
    }
    if (IsGLExtSupported("GL_EXT_texture_format_BGRA8888")) {
        PL_GL.hasEXTBGRA = DXTRUE;
        s_debugPrint("s_LoadGL: has GL_EXT_texture_format_BGRA8888");
    }
#endif
    
    PL_GL.isInitialized = DXTRUE;
    
    memset(&PLG, 0, sizeof(PLG));
    PLG.SetBlendMode = PLGL_SetBlendMode;
    PLG.SetBlendModeSeparate = PLGL_SetBlendModeSeparate;
    PLG.DisableBlend = PLGL_DisableBlend;
    PLG.EnableAlphaTest = PLGL_EnableAlphaTest;
    PLG.DisableAlphaTest = PLGL_DisableAlphaTest;
    PLG.SetScissor = PLGL_SetScissor;
    PLG.SetScissorRect = PLGL_SetScissorRect;
    PLG.DisableScissor = PLGL_DisableScissor;
    PLG.DisableCulling = PLGL_DisableCulling;
    PLG.DisableDepthTest = PLGL_DisableDepthTest;
    PLG.SetTextureStage = PLGL_SetTextureStage;
    PLG.SetTexturePresetMode = PLGL_SetTexturePresetMode;
    PLG.ClearTextures = PLGL_ClearTextures;
    PLG.ClearTexturePresetMode = PLGL_ClearTexturePresetMode;
    
    PLG.VertexBuffer_CreateBytes = PLGL_VertexBuffer_CreateBytes;
    PLG.VertexBuffer_Create = PLGL_VertexBuffer_Create;
    PLG.VertexBuffer_ResetBuffer = PLGL_VertexBuffer_ResetBuffer;
    PLG.VertexBuffer_SetDataBytes = PLGL_VertexBuffer_SetDataBytes;
    PLG.VertexBuffer_SetData = PLGL_VertexBuffer_SetData;
    PLG.VertexBuffer_Lock = PLGL_VertexBuffer_Lock;
    PLG.VertexBuffer_Unlock = PLGL_VertexBuffer_Unlock;
    PLG.VertexBuffer_Delete = PLGL_VertexBuffer_Delete;
    
    PLG.IndexBuffer_Create = PLGL_IndexBuffer_Create;
    PLG.IndexBuffer_ResetBuffer = PLGL_IndexBuffer_ResetBuffer;
    PLG.IndexBuffer_SetData = PLGL_IndexBuffer_SetData;
    PLG.IndexBuffer_Lock = PLGL_IndexBuffer_Lock;
    PLG.IndexBuffer_Unlock = PLGL_IndexBuffer_Unlock;
    PLG.IndexBuffer_Delete = PLGL_IndexBuffer_Delete;

    PLG.Texture_CreateFromSDLSurface = PLGL_Texture_CreateFromSDLSurface;
    PLG.Texture_CreateFromDimensions = PLGL_Texture_CreateFromDimensions;
    PLG.Texture_CreateFramebuffer = PLGL_Texture_CreateFramebuffer;
    PLG.Texture_BlitSurface = PLGL_Texture_BlitSurface;
    PLG.Texture_RenderGetTextureInfo = PLGL_Texture_RenderGetTextureInfo;
    PLG.Texture_SetWrap = PLGL_Texture_SetWrap;
    PLG.Texture_HasAlphaChannel = PLGL_Texture_HasAlphaChannel;
    PLG.Texture_BindFramebuffer = PLGL_Texture_BindFramebuffer;
    PLG.Texture_AddRef = PLGL_Texture_AddRef;
    PLG.Texture_Release = PLGL_Texture_Release;
    
    PLG.DrawVertexArray = PLGL_DrawVertexArray;
    PLG.DrawVertexIndexArray = PLGL_DrawVertexIndexArray;
    PLG.DrawVertexBuffer = PLGL_DrawVertexBuffer;
    PLG.DrawVertexIndexBuffer = PLGL_DrawVertexIndexBuffer;
    PLG.SetViewport = PLGL_SetViewport;
    PLG.SetZRange = PLGL_SetZRange;
    PLG.SetMatrices = PLGL_SetMatrices;
    PLG.SetMatrixDirtyFlag = PLGL_SetMatrixDirtyFlag;
    PLG.SetUntransformedFlag = PLGL_SetUntransformedFlag;
    PLG.ClearColor = PLGL_ClearColor;
    PLG.Clear = PLGL_Clear;
    PLG.SetMatrices = PLGL_SetMatrices;
    PLG.StartFrame = PLGL_StartFrame;
    PLG.EndFrame = PLGL_EndFrame;
    
    PLG.End = PLGL_End;
    
    PLGL_Render_Init();
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */

