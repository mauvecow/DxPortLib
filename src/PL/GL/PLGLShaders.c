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

#include "PL/PLInternal.h"

#include "PLGLInternal.h"

#include "SDL.h"

static const char s_stockVertexShaderNocolorTex1[] = {
        "attribute vec4 position;\n"
        "attribute vec2 texcoord;\n"
        "uniform mat4 modelView;\n"
        "uniform mat4 projection;\n"
        "varying vec2 outTexcoord;\n"
        "void main() {\n"
        "    gl_Position = projection * (modelView * position);\n"
        "    outTexcoord = texcoord;\n"
        "}\n"
};
static const char s_stockVertexShaderColorNotex[] = {
        "attribute vec4 position;\n"
        "attribute vec4 color;\n"
        "uniform mat4 modelView;\n"
        "uniform mat4 projection;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    gl_Position = projection * (modelView * position);\n"
        "    outColor = color;\n"
        "}\n"
};
static const char s_stockVertexShaderColorTex1[] = {
        "attribute vec4 position;\n"
        "attribute vec2 texcoord;\n"
        "attribute vec4 color;\n"
        "uniform mat4 modelView;\n"
        "uniform mat4 projection;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    gl_Position = projection * (modelView * position);\n"
        "    outColor = color;\n"
        "    outTexcoord = texcoord;\n"
        "}\n",
};

static const PLGLShaderDefinition s_stockShaderDefinitions[PLGL_SHADER_END] = {
    /* PLGL_SHADER_BASIC_NOCOLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderNocolorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(texture, outTexcoord);\n"
        "}\n",
        1, 1, 0
    },
    /* PLGL_SHADER_BASIC_COLOR_NOTEX */
    {
        /* vertex shader */
        s_stockVertexShaderColorNotex,
        /* fragment shader */
        "precision mediump float;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    gl_FragColor = outColor;\n"
        "}\n",
        0, 0, 1
    },
    /* PLGL_SHADER_BASIC_COLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderColorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(texture, outTexcoord) * outColor;\n"
        "}\n",
        1, 1, 1
    },
    /* PLGL_SHADER_DX_INVERT_COLOR_NOTEX */
    {
        /* vertex shader */
        s_stockVertexShaderColorNotex,
        /* fragment shader */
        "precision mediump float;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 c = outColor;\n"
        "    gl_FragColor = vec4(1.0 - c.rgb, c.a);\n"
        "}\n",
        0, 0, 1
    },
    /* PLGL_SHADER_DX_INVERT_COLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderColorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 c = texture2D(texture, outTexcoord);\n"
        "    gl_FragColor = vec4((1.0 - c.rgb) * (1.0 - outColor.rgb), c.a * outColor.a);\n"
        "}\n",
        1, 1, 1
    },
    /* PLGL_SHADER_DX_MULA_COLOR_NOTEX */
    {
        /* vertex shader */
        s_stockVertexShaderColorNotex,
        /* fragment shader */
        "precision mediump float;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 c = outColor;\n"
        "    gl_FragColor = vec4(c.rgb * c.a, c.a);\n"
        "}\n",
        0, 0, 1
    },
    /* PLGL_SHADER_DX_MULA_COLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderColorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 c = texture2D(texture, outTexcoord) * outColor;\n"
        "    gl_FragColor = vec4(c.rgb * c.a, c.a);\n"
        "}\n",
        1, 1, 1
    },
    /* PLGL_SHADER_DX_X4_COLOR_NOTEX */
    {
        /* vertex shader */
        s_stockVertexShaderColorNotex,
        /* fragment shader */
        "precision mediump float;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 c = outColor;\n"
        "    gl_FragColor = vec4(c.rgb * 4.0, c.a);\n"
        "}\n",
        0, 0, 1
    },
    /* PLGL_SHADER_DX_X4_COLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderColorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 c = texture2D(texture, outTexcoord) * outColor;\n"
        "    gl_FragColor = vec4(c.rgb * 4.0, c.a);\n"
        "}\n",
        1, 1, 1
    },
    /* PLGL_SHADER_DX_PMA_COLOR_NOTEX */
    {
        /* vertex shader */
        s_stockVertexShaderColorNotex,
        /* fragment shader */
        "precision mediump float;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    gl_FragColor = vec4(outColor.rgb * outColor.a, outColor.a);\n"
        "}\n",
        0, 0, 1
    },
    /* PLGL_SHADER_DX_PMA_COLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderColorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 oc = vec4(outColor.rgb * outColor.a, outColor.a);\n"
        "    gl_FragColor = texture2D(texture, outTexcoord) * oc;\n"
        "}\n",
        1, 1, 1
    },
    /* PLGL_SHADER_DX_PMA_INVERT_COLOR_NOTEX */
    {
        /* vertex shader */
        s_stockVertexShaderColorNotex,
        /* fragment shader */
        "precision mediump float;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    gl_FragColor = vec4((1.0 - outColor.rgb) * outColor.a, outColor.a);;\n"
        "}\n",
        0, 0, 1
    },
    /* PLGL_SHADER_DX_PMA_INVERT_COLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderColorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 c = texture2D(texture, outTexcoord);\n"
        "    gl_FragColor = vec4(outColor.rgb * (1.0 - c.rgb), c.a * outColor.a);\n"
        "}\n",
        1, 1, 1
    },
    /* PLGL_SHADER_DX_PMA_X4_COLOR_NOTEX */
    {
        /* vertex shader */
        s_stockVertexShaderColorNotex,
        /* fragment shader */
        "precision mediump float;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    gl_FragColor = vec4(outColor.rgb * outColor.a * 4.0, outColor.a);\n"
        "}\n",
        0, 0, 1
    },
    /* PLGL_SHADER_DX_PMA_X4_COLOR_TEX1 */
    {
        /* vertex shader */
        s_stockVertexShaderColorTex1,
        /* fragment shader */
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 outTexcoord;\n"
        "varying vec4 outColor;\n"
        "void main() {\n"
        "    vec4 oc = vec4(outColor.rgb * outColor.a * 4.0, outColor.a);\n"
        "    gl_FragColor = texture2D(texture, outTexcoord) * oc;\n"
        "}\n",
        1, 1, 1
    }
};

int PLGL_Shaders_CompileDefinition(const PLGLShaderDefinition *definition) {
    GLuint glVertexShaderID = 0;
    GLuint glFragmentShaderID = 0;
    GLuint glProgramID = 0;
    int shaderHandle;
    PLGLShaderInfo *info;
    
    do {
        GLint status;
        /* Clear any GL errors out before we do anything. */
        PL_GL.glGetError();

        glProgramID = PL_GL.glCreateProgram();
        if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
        
        if (definition->vertexShader != NULL) {
            glVertexShaderID = PL_GL.glCreateShader(GL_VERTEX_SHADER);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
            PL_GL.glShaderSource(glVertexShaderID, 1, &definition->vertexShader, NULL);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
            PL_GL.glCompileShader(glVertexShaderID);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
            PL_GL.glGetShaderiv(glVertexShaderID, GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) { break; }
            
            PL_GL.glAttachShader(glProgramID, glVertexShaderID);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
        }
        
        if (definition->fragmentShader != NULL) {
            glFragmentShaderID = PL_GL.glCreateShader(GL_FRAGMENT_SHADER);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
            PL_GL.glShaderSource(glFragmentShaderID, 1, &definition->fragmentShader, NULL);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
            PL_GL.glCompileShader(glFragmentShaderID);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
            PL_GL.glGetShaderiv(glFragmentShaderID, GL_COMPILE_STATUS, &status);
            if (status != GL_TRUE) { break; }
            
            PL_GL.glAttachShader(glProgramID, glFragmentShaderID);
            if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
        }
        
        PL_GL.glLinkProgram(glProgramID);
        if (PL_GL.glGetError() != GL_NO_ERROR) { break; }
        
        shaderHandle = PL_Handle_AcquireID(DXHANDLE_SHADER);
        info = (PLGLShaderInfo *)PL_Handle_AllocateData(shaderHandle, sizeof(PLGLShaderInfo));
        memset(info, 0, sizeof(PLGLShaderInfo));
        
        memcpy(&info->definition, definition, sizeof(PLGLShaderDefinition));
        info->definition.vertexShader = NULL;
        info->definition.fragmentShader = NULL;
        
        info->glVertexShaderID = glVertexShaderID;
        info->glFragmentShaderID = glFragmentShaderID;
        info->glProgramID = glProgramID;
        
        info->glProjectionUniformID = PL_GL.glGetUniformLocation(glProgramID, "projection");
        info->glModelViewUniformID = PL_GL.glGetUniformLocation(glProgramID, "modelView");
        
        info->glVertexAttribID = PL_GL.glGetAttribLocation(glProgramID, "position");
        info->glTextureUniformID[0] = PL_GL.glGetUniformLocation(glProgramID, "texture");
        info->glTextureUniformID[1] = PL_GL.glGetUniformLocation(glProgramID, "texture1");
        info->glTextureUniformID[2] = PL_GL.glGetUniformLocation(glProgramID, "texture2");
        info->glTextureUniformID[3] = PL_GL.glGetUniformLocation(glProgramID, "texture3");
        info->glTexcoordAttribID[0] = PL_GL.glGetAttribLocation(glProgramID, "texcoord");
        info->glTexcoordAttribID[1] = PL_GL.glGetAttribLocation(glProgramID, "texcoord2");
        info->glTexcoordAttribID[2] = PL_GL.glGetAttribLocation(glProgramID, "texcoord3");
        info->glTexcoordAttribID[3] = PL_GL.glGetAttribLocation(glProgramID, "texcoord4");
        info->glColorAttribID = PL_GL.glGetAttribLocation(glProgramID, "color");
        
        return shaderHandle;
    } while(0);
    
    if (glVertexShaderID != 0) {
        PL_GL.glDeleteShader(glVertexShaderID);
    }
    if (glFragmentShaderID != 0) {
        PL_GL.glDeleteShader(glFragmentShaderID);
    }
    if (glProgramID != 0) {
        PL_GL.glDeleteProgram(glProgramID);
    }
    
    return -1;
}

void PLGL_Shaders_DeleteShader(int shaderHandle) {
    PLGLShaderInfo *info = (PLGLShaderInfo *)PL_Handle_GetData(shaderHandle, DXHANDLE_SHADER);
    
    if (info != NULL) {
        if (info->glVertexShaderID != 0) {
            PL_GL.glDeleteShader(info->glVertexShaderID);
        }
        if (info->glFragmentShaderID != 0) {
            PL_GL.glDeleteShader(info->glFragmentShaderID);
        }
        PL_GL.glDeleteProgram(info->glProgramID);
        
        PL_Handle_ReleaseID(shaderHandle, DXTRUE);
    }
}

static GLenum VertexElementSizeToGL(int value) {
    switch(value) {
        case VERTEXSIZE_UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        default: /* VERTEXSIZE_FLOAT */
            return GL_FLOAT;
    }
}

void PLGL_Shaders_UseProgram(int shaderHandle) {
    if (shaderHandle < 0) {
        PL_GL.glUseProgram(0);
    } else {
        PLGLShaderInfo *info = (PLGLShaderInfo *)PL_Handle_GetData(shaderHandle, DXHANDLE_SHADER);
        
        if (info == NULL) {
            PL_GL.glUseProgram(0);
            return;
        }
        
        PL_GL.glUseProgram(info->glProgramID);
    }
}

void PLGL_Shaders_ApplyProgramMatrices(int shaderHandle,
                               const PLMatrix *projectionMatrix, const PLMatrix *viewMatrix) {
    PLGLShaderInfo *info = (PLGLShaderInfo *)PL_Handle_GetData(shaderHandle, DXHANDLE_SHADER);
    
    if (info == NULL) {
        return;
    }
    
    PL_GL.glUniformMatrix4fv(info->glProjectionUniformID, 1, GL_FALSE, (GLfloat *)projectionMatrix);
    PL_GL.glUniformMatrix4fv(info->glModelViewUniformID, 1, GL_FALSE, (GLfloat *)viewMatrix);
}

void PLGL_Shaders_ApplyProgramVertexData(int shaderHandle,
                             const char *vertexData, const VertexDefinition *definition)
{
    PLGLShaderInfo *info = (PLGLShaderInfo *)PL_Handle_GetData(shaderHandle, DXHANDLE_SHADER);
    int i;
    
    if (info == NULL) {
        return;
    }
    
    if (definition != NULL) {
        const VertexElement *e = definition->elements;
        int elementCount = definition->elementCount;
        int vertexDataSize = definition->vertexByteSize;
        
        for (i = 0; i < elementCount; ++i, ++e) {
            GLenum vertexType = VertexElementSizeToGL(e->vertexElementSize);
            switch (e->vertexType) {
                case VERTEX_POSITION:
                    PL_GL.glVertexAttribPointer(info->glVertexAttribID,
                                                e->size, vertexType, GL_FALSE,
                                                vertexDataSize, vertexData + e->offset);
                    PL_GL.glEnableVertexAttribArray(info->glVertexAttribID);
                    break; 
                case VERTEX_TEXCOORD0:
                case VERTEX_TEXCOORD1:
                case VERTEX_TEXCOORD2:
                case VERTEX_TEXCOORD3: {
                        int slot = e->vertexType - VERTEX_TEXCOORD0;
                        GLint attribID = info->glTexcoordAttribID[slot];
                        if (attribID > 0) {
                            PL_GL.glVertexAttribPointer(attribID,
                                                        e->size, vertexType, GL_FALSE,
                                                        vertexDataSize, vertexData + e->offset);
                            PL_GL.glUniform1i(info->glTextureUniformID[slot], slot);
                            PL_GL.glEnableVertexAttribArray(attribID);
                        }
                        break;
                    }
                case VERTEX_COLOR:
                    if (info->glColorAttribID > 0) {
                        PL_GL.glVertexAttribPointer(info->glColorAttribID,
                                                    e->size, vertexType, GL_TRUE,
                                                    vertexDataSize, vertexData + e->offset);
                        PL_GL.glEnableVertexAttribArray(info->glColorAttribID);
                    }
                    break;
            }
        }
    }
}

void PLGL_Shaders_ClearProgramVertexData(int shaderHandle, const VertexDefinition *definition) {
    PLGLShaderInfo *info = (PLGLShaderInfo *)PL_Handle_GetData(shaderHandle, DXHANDLE_SHADER);
    int i;
    
    if (info == NULL) {
        return;
    }
    
    if (definition != NULL) {
        const VertexElement *e = definition->elements;
        int elementCount = definition->elementCount;
        
        for (i = 0; i < elementCount; ++i, ++e) {
            switch (e->vertexType) {
                case VERTEX_POSITION:
                    PL_GL.glDisableVertexAttribArray(info->glVertexAttribID);
                    break; 
                case VERTEX_TEXCOORD0:
                case VERTEX_TEXCOORD1:
                case VERTEX_TEXCOORD2:
                case VERTEX_TEXCOORD3: {
                        GLint attribID = info->glTexcoordAttribID[e->vertexType - VERTEX_TEXCOORD0];
                        if (attribID > 0) {
                            PL_GL.glDisableVertexAttribArray(attribID);
                        }
                        break;
                    }
                case VERTEX_COLOR:
                    if (info->glColorAttribID > 0) {
                        PL_GL.glDisableVertexAttribArray(info->glColorAttribID);
                    }
                    break;
            }
        }
    }
}

static int s_stockShaderIDs[PLGL_SHADER_END] = { 0 };

int PLGL_Shaders_GetStockProgramForID(PLGLShaderPresetType shaderType) {
    return s_stockShaderIDs[shaderType];
}

void PLGL_Shaders_Init() {
    int i;
    
    for (i = 0; i < PLGL_SHADER_END; ++i) {
        s_stockShaderIDs[i] = PLGL_Shaders_CompileDefinition(&s_stockShaderDefinitions[i]);
    }
}

void PLGL_Shaders_Cleanup() {
    int i;
    
    for (i = 0; i < PLGL_SHADER_END; ++i) {
        PLGL_Shaders_DeleteShader(s_stockShaderIDs[i]);
        s_stockShaderIDs[i] = 0;
    }
}

#endif
