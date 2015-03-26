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

#ifndef DXPORTLIB_OPENGL_DXINTERNAL_H_HEADER
#define DXPORTLIB_OPENGL_DXINTERNAL_H_HEADER

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "PL/PLInternal.h"

#include "SDL.h"

#ifdef DXPORTLIB_DRAW_OPENGL_ES2
#include "SDL_opengles2.h"
#else
#include "SDL_opengl.h"
#endif

/* In the event this is not defined... */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

typedef struct GLInfo_t {
    int isInitialized;
    
    int hasNPTSupport;
    int hasTextureRectangleSupport;
    int maxTextureWidth;
    int maxTextureHeight;

    /* Main GL functions */
    void (APIENTRY *glEnable)( GLenum cap );
    void (APIENTRY *glDisable)( GLenum cap );
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    void (APIENTRY *glEnableClientState)( GLenum cap );
    void (APIENTRY *glDisableClientState)( GLenum cap );
#endif
    
    GLenum (APIENTRY *glGetError)(void);
    void (APIENTRY *glPixelStorei)( GLenum pname, GLint param );
    void (APIENTRY *glFinish)(void);
    void (APIENTRY *glGetIntegerv)( GLenum pname, GLint *params );
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    void (APIENTRY *glMatrixMode)( GLenum mode );
    void (APIENTRY *glLoadIdentity)( void );
    void (APIENTRY *glLoadMatrixf)( const GLfloat *m );
#endif
    
    /* Texture functions */
    void (APIENTRY *glGenTextures)( GLsizei n, GLuint *textures );
    void (APIENTRY *glDeleteTextures)( GLsizei n, const GLuint *textures);

    void (APIENTRY *glActiveTexture)( GLenum texture );
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    void (APIENTRY *glClientActiveTexture)( GLenum texture );
#endif
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
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    void (APIENTRY *glColor4f)( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
#endif
    
    void (APIENTRY *glLineWidth)( GLfloat width );

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    void (APIENTRY *glTexEnvf)( GLenum target, GLenum pname, GLfloat param );
    void (APIENTRY *glTexEnvi)( GLenum target, GLenum pname, GLint param );
#endif
    void (APIENTRY *glBlendFuncSeparate) ( GLenum srcRGB, GLenum dstRGB,
                                           GLenum srcAlpha, GLenum dstAlpha);
    void (APIENTRY *glBlendFunc) ( GLenum src, GLenum srcAlpha );
    void (APIENTRY *glBlendEquationSeparate) ( GLenum modeRGB, GLenum modeAlpha );
    void (APIENTRY *glBlendEquation) ( GLenum mode );
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    void (APIENTRY *glAlphaFunc)( GLenum func, GLclampf ref );
#endif
    
    void (APIENTRY *glViewport)( GLint x, GLint y, GLsizei width, GLsizei height );
    void (APIENTRY *glScissor)( GLint x, GLint y, GLsizei width, GLsizei height );
    
    void (APIENTRY *glDrawArrays)( GLenum mode, GLint first, GLsizei count );
    void (APIENTRY *glDrawElements)( GLenum mode, GLsizei count,
                                     GLenum type, const GLvoid *indices );
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    void (APIENTRY *glVertexPointer)( GLint size, GLenum type,  
                                      GLsizei stride, const GLvoid *ptr );  
    void (APIENTRY *glColorPointer)( GLint size, GLenum type,
                                     GLsizei stride, const GLvoid *ptr );
    void (APIENTRY *glTexCoordPointer)( GLint size, GLenum type,
                                        GLsizei stride, const GLvoid *ptr );
#endif
    
    /* Vertex buffer functions */
    int hasVBOSupport;
    void (APIENTRY *glBindBuffer)( GLenum target, GLuint buffer );
    void (APIENTRY *glDeleteBuffers)( GLsizei n, const GLuint *buffers );
    void (APIENTRY *glGenBuffers)( GLsizei n, GLuint *buffers );
    void (APIENTRY *glBufferData)( GLenum target, GLsizeiptr size,
                                      const GLvoid *data, GLenum usage );
    void (APIENTRY *glBufferSubData)( GLenum target, GLintptr offset,
                                         GLsizeiptr size, const GLvoid *data );
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    GLvoid *(APIENTRY *glMapBuffer)( GLenum target, GLenum access );
    GLboolean (APIENTRY *glUnmapBuffer)( GLenum target );
#endif
    
    /* Framebuffer functions */
    int hasFramebufferSupport;
    
    void (APIENTRY *glFramebufferTexture2D)(GLenum target, GLenum attachment,
                                             GLenum textarget, GLuint texture, GLint level);
    void (APIENTRY *glBindFramebuffer)(GLenum target, GLuint framebuffer);
    void (APIENTRY *glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
    void (APIENTRY *glGenFramebuffers)(GLsizei n, GLuint *framebuffers);
    GLenum (APIENTRY *glCheckFramebufferStatus)(GLenum target);
    
    /* Shader functions */
    int hasShaderSupport;
    
    GLuint (APIENTRY *glCreateShader)(GLenum type);
    GLuint (APIENTRY *glDeleteShader)(GLuint shader);
    void (APIENTRY *glShaderSource)(GLuint shader, GLsizei count,
                                    const GLchar * const *string,
                                    const GLint *length);
    void (APIENTRY *glCompileShader)(GLuint shader);
    void (APIENTRY *glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
    GLint (APIENTRY *glGetUniformLocation)(GLuint program, const GLchar *name);
    GLint (APIENTRY *glGetAttribLocation)(GLuint program, const GLchar *name);
    
    GLuint (APIENTRY *glCreateProgram)(void);
    void (APIENTRY *glDeleteProgram)(GLuint program);
    void (APIENTRY *glUseProgram)(GLuint program);
    void (APIENTRY *glLinkProgram)(GLuint program);
    void (APIENTRY *glAttachShader)(GLuint program, GLuint shader);
    
    void (APIENTRY *glEnableVertexAttribArray)(GLuint index);
    void (APIENTRY *glDisableVertexAttribArray)(GLuint index);
    void (APIENTRY *glVertexAttribPointer)(GLuint index, GLint size, GLenum type,
                                           GLboolean normalized, GLsizei stride,
                                           const void *pointer);
    
    void (APIENTRY *glUniform1i)(GLint location, GLint v0);
    void (APIENTRY *glUniformMatrix4fv)(GLint location, GLsizei count,
                                        GLboolean transpose, const GLfloat *value);
    
    /* ES2 stuff */
#ifdef DXPORTLIB_DRAW_OPENGL_ES2
    int hasEXTUnpackSubimage;
    int hasEXTBGRA;
#endif
} GLInfo;

extern GLInfo PL_GL;

typedef enum {
    PLGL_SHADER_BASIC_NOCOLOR_TEX1 = 0,
    PLGL_SHADER_BASIC_COLOR_NOTEX,
    PLGL_SHADER_BASIC_COLOR_TEX1,
    PLGL_SHADER_DX_INVERT_COLOR_NOTEX,
    PLGL_SHADER_DX_INVERT_COLOR_TEX1,
    PLGL_SHADER_DX_MULA_COLOR_NOTEX,
    PLGL_SHADER_DX_MULA_COLOR_TEX1,
    PLGL_SHADER_DX_X4_COLOR_NOTEX,
    PLGL_SHADER_DX_X4_COLOR_TEX1,
    PLGL_SHADER_DX_PMA_COLOR_NOTEX,
    PLGL_SHADER_DX_PMA_COLOR_TEX1,
    PLGL_SHADER_DX_PMA_INVERT_COLOR_NOTEX,
    PLGL_SHADER_DX_PMA_INVERT_COLOR_TEX1,
    PLGL_SHADER_DX_PMA_X4_COLOR_NOTEX,
    PLGL_SHADER_DX_PMA_X4_COLOR_TEX1,
    PLGL_SHADER_END
} PLGLShaderPresetType;

typedef struct _PLGLShaderDefinition {
    const char * vertexShader;
    const char * fragmentShader;
    int textureCount;
    int texcoordCount;
    int hasColor;
} PLGLShaderDefinition;

typedef struct _PLGLShaderInfo {
    PLGLShaderDefinition definition;
    GLuint glVertexShaderID;
    GLuint glFragmentShaderID;
    GLuint glProgramID;
    
    GLuint glVertexAttribID;
    GLuint glTextureUniformID[4];
    GLuint glTexcoordAttribID[4];
    GLuint glColorAttribID;
    
    GLuint glModelViewUniformID;
    GLuint glProjectionUniformID;
} PLGLShaderInfo;

extern int PL_drawScreenWidth;
extern int PL_drawScreenHeight;

extern int PL_Render_UpdateMatrices();

extern int PL_Texture_Bind(int textureRefID, int drawMode);
extern int PL_Texture_Unbind(int textureRefID);
extern int PL_Texture_ClearAllData();

extern int PL_Framebuffer_GetSurface(const PLRect *rect, SDL_Surface **dSurface);

extern GLuint PL_VertexBuffer_GetGLID(int vertexBufferID);
extern char *PL_VertexBuffer_GetFallback(int vboHandle);
extern GLuint PL_IndexBuffer_GetGLID(int vertexBufferID);
extern char *PL_IndexBuffer_GetFallback(int vboHandle);

#ifndef DXPORTLIB_DRAW_OPENGL_ES2
extern int PL_GLFixedFunction_ClearTexturePresetMode();
extern int PL_GLFixedFunction_SetTexturePresetMode(int preset,
                                   int textureRefID, int textureDrawMode);
extern int PL_GLFixedFunction_ApplyVertexArrayData(const VertexDefinition *def,
                                          const char *vertexData);
extern int PL_GLFixedFunction_ClearVertexArrayData(const VertexDefinition *def);
extern int PL_GLFixedFunction_ApplyVertexBufferData(const VertexDefinition *def);
extern int PL_GLFixedFunction_ClearVertexBufferData(const VertexDefinition *def);
extern int PL_GLFixedFunction_Init();
extern int PL_GLFixedFunction_Cleanup();
#endif

extern int PL_Shaders_CompileDefinition(const PLGLShaderDefinition *definition);
extern void PL_Shaders_DeleteShader(int shaderHandle);
extern void PL_Shaders_ApplyProgram(int shaderHandle,
                                    PLMatrix *projectionMatrix, PLMatrix *viewMatrix,
                                    const char *vertexData,
                                    const VertexDefinition *definition);
extern void PL_Shaders_ClearProgram(int shaderHandle,
                                    const VertexDefinition *definition);
extern int PL_Shaders_GetStockProgramForID(PLGLShaderPresetType shaderType);
extern void PL_Shaders_Init();
extern void PL_Shaders_Cleanup();

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */

#endif /* #ifndef DXPORTLIB_OPENGL_DXINTERNAL_H */

