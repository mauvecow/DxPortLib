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

/* --------------------------------------------------------- Framebuffers */

/* Framebuffers are reused when it is possible to do so, so we
 * refcount instead.
 */
typedef struct FramebufferInfo {
    GLuint framebufferID;
    
    int width;
    int height;
    
    int refCount;
} FramebufferInfo;

static int s_GLFrameBuffer_Bind(int handleID, GLenum textureTarget, GLuint textureID) {
    FramebufferInfo *info;
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        return -1;
    }
    
    info = (FramebufferInfo *)PL_Handle_GetData(handleID, DXHANDLE_FRAMEBUFFER);
    if (info == NULL || textureID < 0) {
        /* s_GLFrameBuffer_Bind(-1, -1) is synonymous with 'bind nothing' */
        PL_GL.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    } else {
        PL_GL.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, info->framebufferID);
        
        PL_GL.glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                        GL_COLOR_ATTACHMENT0_EXT,
                                        textureTarget,
                                        textureID,
                                        0);
        
        if (PL_GL.glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
            /* uhoh... */
            PL_GL.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            return -1;
        }
    }
    
    return 0;
}

static int s_GLFrameBuffer_Create(int width, int height) {
    int handleID;
    FramebufferInfo *info;
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        return -1;
    }
    
    /* Reuse existing framebuffers when available. */
    handleID = PL_Handle_GetFirstIDOf(DXHANDLE_FRAMEBUFFER);
    while (handleID >= 0) {
        info = (FramebufferInfo *)PL_Handle_GetData(handleID, DXHANDLE_FRAMEBUFFER);
        if (info != NULL && info->width == width && info->height == height) {
            info->refCount += 1;
            
            break;
        }
        
        handleID = PL_Handle_GetNextID(DXHANDLE_FRAMEBUFFER);
    }
    
    if (handleID < 0) {
        handleID = PL_Handle_AcquireID(DXHANDLE_FRAMEBUFFER);
        if (handleID < 0) {
            return -1;
        }
        info = (FramebufferInfo *)PL_Handle_AllocateData(handleID, sizeof(FramebufferInfo));
        
        PL_GL.glGenFramebuffersEXT(1, &info->framebufferID);
        info->width = width;
        info->height = height;
        info->refCount = 1;
    }
    
    return handleID;
}

static int s_GLFrameBuffer_Release(int handleID) {
    FramebufferInfo *info;
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        return -1;
    }
    
    info = (FramebufferInfo *)PL_Handle_GetData(handleID, DXHANDLE_FRAMEBUFFER);
    if (info == NULL) {
        return -1;
    }
    
    info->refCount -= 1;
    if (info->refCount <= 0) {
        PL_GL.glDeleteFramebuffersEXT(1, &info->framebufferID);
        info->framebufferID = 0;
        
        PL_Handle_ReleaseID(handleID, DXTRUE);
    }
    
    return 0;
}

/* ------------------------------------------------------------- Textures */

typedef struct TextureRef {
    GLuint textureID;
    
    int drawMode;
    int hasAlphaChannel;
    
    GLint glInternalFormat;
    GLuint glTarget;
    GLuint glFormat;
    GLuint glType;
    
    int width;
    int height;
    float widthMult;
    float heightMult;
    
    Uint32 sdlFormat;
    
    int texWidth;
    int texHeight;
    
    int framebufferID;
    
    int refCount;
} TextureRef;

static int s_topow2(int v) {
    int n = 1;
    while (n < v) {
        n <<= 1;
    }
    return n;
}

static int s_AllocateTextureRefID(GLuint textureID) {
    int textureRefID = PL_Handle_AcquireID(DXHANDLE_TEXTURE);
    TextureRef *textureref;
    
    if (textureRefID < 0) {
        return -1;
    }
    
    textureref = (TextureRef *)PL_Handle_AllocateData(textureRefID, sizeof(TextureRef));
    textureref->textureID = textureID;
    textureref->framebufferID = 0;
    textureref->refCount = 0;
    
    return textureRefID;
}

static void s_blitSurface(TextureRef *textureRef, SDL_Surface *surface, const SDL_Rect *rect) {
    GLuint textureTarget = textureRef->glTarget;
    
    PL_GL.glEnable(textureTarget);
    PL_GL.glBindTexture(textureTarget, textureRef->textureID);
    PL_GL.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    PL_GL.glPixelStorei(GL_UNPACK_ROW_LENGTH, (surface->pitch / surface->format->BytesPerPixel));
    PL_GL.glTexSubImage2D(
        textureTarget, 0,
        rect->x, rect->y, rect->w, rect->h,
        textureRef->glFormat, textureRef->glType,
        surface->pixels
    );
    
    PL_GL.glDisable(textureTarget);
}

int PL_Texture_Bind(int textureRefID, int drawMode) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    GLuint textureTarget;
    if (textureref == NULL) {
        return -1;
    }
    
    textureTarget = textureref->glTarget;
    PL_GL.glEnable(textureTarget);
    PL_GL.glBindTexture(textureTarget, textureref->textureID);
    
    if (drawMode != textureref->drawMode) {
        textureref->drawMode = drawMode;
        switch(drawMode) {
            case DX_DRAWMODE_NEAREST:
            default:
                PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case DX_DRAWMODE_BILINEAR:
                PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }
    }
    
    return 0;
}

int PL_Texture_Unbind(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
    PL_GL.glDisable(textureref->glTarget);
    return 0;
}

int PL_Texture_BindFramebuffer(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    int framebufferID = -1;
    GLenum textureTarget = GL_TEXTURE_2D;
    GLuint textureID = 0;
    if (textureref != NULL) {
        framebufferID = textureref->framebufferID;
        textureID = textureref->textureID;
        textureTarget = textureref->glTarget;
    }
    
    return s_GLFrameBuffer_Bind(framebufferID, textureTarget, textureID);
}

int PL_Texture_HasAlphaChannel(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return 0;
    }
    return textureref->hasAlphaChannel;
}

int PL_Texture_CreateFromSurface(SDL_Surface *surface, int hasAlphaChannel) {
    int textureRefID;
    
    if (SDL_GetColorKey(surface, 0) >= 0) {
        hasAlphaChannel = DXTRUE;
    }
    
    textureRefID = PL_Texture_CreateFromDimensions(surface->w, surface->h, hasAlphaChannel);
    if (textureRefID < 0) {
        return -1;
    }
    
    PL_Texture_BlitSurface(textureRefID, surface, NULL);
    
    return textureRefID;
}

int PL_Texture_CreateFromDimensions(int width, int height, int hasAlphaChannel) {
    int textureRefID;
    TextureRef *textureref;
    GLint textureInternalFormat = 0;
    GLenum textureFormat = 0;
    GLenum textureType = 0;
    GLuint textureID = 0;
    GLenum textureTarget;
    int texWidth, texHeight;
    
    /* - Get format/dimensions, make sure it's valid. */
    if (PL_GL.hasTextureRectangleSupport) {
        textureTarget = GL_TEXTURE_RECTANGLE_ARB;
        texWidth = width;
        texHeight = height;
    } else {
        textureTarget = GL_TEXTURE_2D;
        texWidth = s_topow2(width);
        texHeight = s_topow2(height);
    }
    
    if (texWidth > PL_GL.maxTextureWidth || texHeight > PL_GL.maxTextureHeight) {
        return -1;
    }
    
    textureInternalFormat = GL_RGBA8;
    textureFormat = GL_BGRA;
    textureType = GL_UNSIGNED_INT_8_8_8_8_REV;
    
    /* - Create the texture itself. */
    PL_GL.glGenTextures(1, &textureID);
    if (PL_GL.glGetError() != GL_NO_ERROR) {
        return -1;
    }
    
    PL_GL.glEnable(textureTarget);
    PL_GL.glBindTexture(textureTarget, textureID);
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    if (textureTarget != GL_TEXTURE_RECTANGLE_ARB) {
        PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    PL_GL.glTexImage2D(
            textureTarget, 0, textureInternalFormat,
            texWidth, texHeight,
            0, textureFormat, textureType, NULL
        );
    
    PL_GL.glDisable(textureTarget);
    if (PL_GL.glGetError() != GL_NO_ERROR) {
        return -1;
    }

    /* - Assign to texture reference. */
    textureRefID = s_AllocateTextureRefID(textureID);
    if (textureRefID < 0) {
        PL_GL.glDeleteTextures(1, &textureID);
        return -1;
    }
    
    textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    
    textureref->textureID = textureID;
    textureref->glInternalFormat = textureInternalFormat;
    textureref->glTarget = textureTarget;
    textureref->glFormat = textureFormat;
    textureref->glType = textureType;
    textureref->sdlFormat = SDL_PIXELFORMAT_ARGB8888;
    textureref->width = width;
    textureref->height = height;
    textureref->texWidth = texWidth;
    textureref->texHeight = texHeight;
    textureref->drawMode = DX_DRAWMODE_NEAREST;
    textureref->hasAlphaChannel = hasAlphaChannel;
    
    if (textureTarget == GL_TEXTURE_RECTANGLE_ARB) {
        textureref->widthMult = 1.0f;
        textureref->heightMult = 1.0f;
    } else {
        textureref->widthMult = 1.0f / (float)texWidth;
        textureref->heightMult = 1.0f / (float)texHeight;
    }
    
    return textureRefID;
}

int PL_Texture_CreateFramebuffer(int width, int height, int hasAlphaChannel) {
    int textureRefID = -1;
    int framebufferID = -1;
    TextureRef *textureref;
    
    textureRefID = PL_Texture_CreateFromDimensions(width, height, hasAlphaChannel);
    if (textureRefID < 0) {
        return -1;
    }
    
    textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    
    framebufferID = s_GLFrameBuffer_Create(width, height);
    if (framebufferID < 0) {
        PL_Texture_Release(textureRefID);
        return -1;
    }
    
    textureref->framebufferID = framebufferID;
    
    return textureRefID;
}

int s_glSetFilter(int textureRefID, GLint minFilter, GLint magFilter) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    GLuint textureTarget;
    if (textureref == NULL || textureref->textureID == 0) {
        return -1;
    }
    
    textureTarget = textureref->glTarget;
    
    PL_GL.glEnable(textureTarget);
    PL_GL.glBindTexture(textureTarget, textureref->textureID);
    
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, minFilter);
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, magFilter);
    
    PL_GL.glDisable(textureTarget);
    
    return 0;
}

int PL_Texture_AddRef(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
    textureref->refCount += 1;
    return 0;
}

int PL_Texture_Release(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
    textureref->refCount -= 1;
    if (textureref->refCount <= 0) {
        if (textureref->textureID > 0) {
            PL_GL.glDeleteTextures(1, &textureref->textureID);
            textureref->textureID = 0;
        }
        if (textureref->framebufferID >= 0) {
            s_GLFrameBuffer_Release(textureref->framebufferID);
        }
        PL_Handle_ReleaseID(textureRefID, DXTRUE);
    }
    return 0;
}

int PL_Texture_BlitSurface(int textureRefID, SDL_Surface *surface, const SDL_Rect *rect) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    SDL_Rect tempRect;
    
    if (textureref == NULL || textureref->textureID == 0) {
        return -1;
    }
    
    if (rect == NULL) {
        tempRect.x = 0;
        tempRect.y = 0;
        tempRect.w = surface->w;
        tempRect.h = surface->h;
        rect = &tempRect;
    }
    
    /* Convert to target format if different. */
    if (textureref->sdlFormat != surface->format->format) {
        SDL_Surface *tempSurface = SDL_ConvertSurfaceFormat(surface, textureref->sdlFormat, 0);
        if (SDL_MUSTLOCK(tempSurface)) {
            SDL_LockSurface(tempSurface);
            s_blitSurface(textureref, tempSurface, rect);
            SDL_UnlockSurface(tempSurface);
        } else {
            s_blitSurface(textureref, tempSurface, rect);
        }
        SDL_FreeSurface(tempSurface);
    } else {
        if (SDL_MUSTLOCK(surface)) {
            SDL_LockSurface(surface);
            s_blitSurface(textureref, surface, rect);
            SDL_UnlockSurface(surface);
        } else {
            s_blitSurface(textureref, surface, rect);
        }
    }
    
    return 0;
}

int PL_Texture_RenderGetTextureInfo(int textureRefID, SDL_Rect *rect, float *xMult, float *yMult) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    
    if (textureref == NULL) {
        return -1;
    }
    rect->x = 0;
    rect->y = 0;
    rect->w = textureref->width;
    rect->h = textureref->height;
    *xMult = textureref->widthMult;
    *yMult = textureref->heightMult;
    
    return 0;
}
int PL_Texture_RenderGetGraphTextureInfo(int graphID, int *dTextureRefID, SDL_Rect *rect, float *xMult, float *yMult) {
    int textureRefID = PL_Graph_GetTextureID(graphID, rect);
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    
    if (textureref == NULL) {
        return -1;
    }
    *dTextureRefID = textureRefID;
    *xMult = textureref->widthMult;
    *yMult = textureref->heightMult;
    
    return 0;
}

int PL_Texture_ClearAllData() {
    /* We can't actually get rid of the handles,
     * but we can toast the data inside. */
    int textureRefID;
    
    textureRefID = PL_Handle_GetFirstIDOf(DXHANDLE_TEXTURE);
    while (textureRefID >= 0) {
        TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
        
        if (textureref != NULL) {
            if (textureref->textureID > 0) {
                PL_GL.glDeleteTextures(1, &textureref->textureID);
                textureref->textureID = 0;
            }
            
            if (textureref->framebufferID >= 0) {
                s_GLFrameBuffer_Release(textureref->framebufferID);
                textureref->framebufferID = -1;
            }
        }
        
        textureRefID = PL_Handle_GetNextID(textureRefID);
    }
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */

