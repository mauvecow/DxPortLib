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

#include "DPLBuildConfig.h"

#ifdef DXPORTLIB_DRAW_OPENGL

#include "PL/PLInternal.h"

#include "PLGLInternal.h"

/* --------------------------------------------------------- Framebuffers */

int s_boundFramebufferID = -1;
GLuint s_boundTextureID = 0;
int s_boundRenderbufferID = -1;

/* Framebuffers are reused when it is possible to do so, so we
 * refcount instead.
 */
typedef struct FramebufferInfo {
    GLuint framebufferID;
    
    int width;
    int height;
    
    int refCount;
} FramebufferInfo;

typedef struct RenderbufferInfo {
    GLuint renderbufferID;
    
    int width;
    int height;
    
    int refCount;
} RenderbufferInfo;

static int s_GLFrameBuffer_Bind(int handleID, GLenum textureTarget, GLuint textureID, int renderbufferID) {
    FramebufferInfo *info;
    
    if (PL_GL.hasFramebufferSupport == DXFALSE) {
        return -1;
    }
    
    if (handleID == s_boundFramebufferID
        && renderbufferID == s_boundRenderbufferID
        && textureID == s_boundTextureID
    ) {
        return 0;
    }
    
    PLGL_Texture_Release(s_boundFramebufferID);
    PLGL_Renderbuffer_Release(s_boundRenderbufferID);
    s_boundFramebufferID = -1;
    s_boundRenderbufferID = -1;
    s_boundTextureID = 0;
    
    info = (FramebufferInfo *)PL_Handle_GetData(handleID, DXHANDLE_FRAMEBUFFER);
    
    if (info == NULL || textureID <= 0) {
        /* s_GLFrameBuffer_Bind(-1, ...) is synonymous with 'bind nothing' */
        PL_GL.glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        RenderbufferInfo *renderinfo = NULL;
        
        PL_GL.glBindFramebuffer(GL_FRAMEBUFFER, info->framebufferID);
        
        PL_GL.glFramebufferTexture2D(GL_FRAMEBUFFER,
                                     GL_COLOR_ATTACHMENT0,
                                     textureTarget,
                                     textureID,
                                     0);
        
        if (renderbufferID > 0) {
            renderinfo = (RenderbufferInfo *)PL_Handle_GetData(renderbufferID, DXHANDLE_RENDERBUFFER);
            
            if (renderinfo != NULL) {
                PL_GL.glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                    GL_RENDERBUFFER, renderinfo->renderbufferID);
                PL_GL.glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                    GL_RENDERBUFFER, renderinfo->renderbufferID);
            }
        }
        
        if (PL_GL.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            /* uhoh... */
            PL_GL.glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return -1;
        }
        
        PL_GL.glViewport(0, 0, info->width, info->height);
        
        info->refCount += 1;
        if (renderinfo != NULL) {
            renderinfo->refCount += 1;
        }
        
        s_boundTextureID = textureID;
        s_boundFramebufferID = handleID;
        s_boundRenderbufferID = renderbufferID;
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
        
        handleID = PL_Handle_GetNextID(handleID);
    }
    
    if (handleID < 0) {
        handleID = PL_Handle_AcquireID(DXHANDLE_FRAMEBUFFER);
        if (handleID < 0) {
            return -1;
        }
        info = (FramebufferInfo *)PL_Handle_AllocateData(handleID, sizeof(FramebufferInfo));
        
        PL_GL.glGenFramebuffers(1, &info->framebufferID);
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
        PL_GL.glDeleteFramebuffers(1, &info->framebufferID);
        info->framebufferID = 0;
        
        PL_Handle_ReleaseID(handleID, DXTRUE);
    }
    
    return 0;
}

int PLGL_Framebuffer_GetSurface(const PLRect *rect, SDL_Surface **dSurface) {
    SDL_Surface *surface;
    
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, rect->w, rect->h, 32,
                                   0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    
    if (surface == NULL) {
        return -1;
    }
    
    PL_GL.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glPixelStorei(GL_UNPACK_ROW_LENGTH, (surface->pitch / surface->format->BytesPerPixel));
#else
#ifdef GL_UNPACK_ROW_LENGTH_EXT
    if (PL_GL.hasEXTUnpackSubimage == DXTRUE) {
        PL_GL.glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, (surface->pitch / surface->format->BytesPerPixel));
    }
#endif
#endif
    PL_GL.glReadPixels(
        rect->x, rect->y, rect->w, rect->h,
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
        GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
#else
        GL_RGBA, GL_UNSIGNED_BYTE,
#endif
        surface->pixels
    );
    
    *dSurface = surface;
    
    return 0;
}

/* ------------------------------------------------------------- Textures */

typedef struct TextureRef {
    PLTextureBase base;
    
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
    int framebufferNeedsClear;
    
    int wrappableFlag;
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
    memset(textureref, 0, sizeof(TextureRef));
    
    textureref->textureID = textureID;
    textureref->framebufferID = -1;
    
    return textureRefID;
}

static void s_blitSurface(TextureRef *textureRef, SDL_Surface *surface, const PLRect *rect) {
    GLuint textureTarget = textureRef->glTarget;
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glEnable(textureTarget);
#endif
    PL_GL.glBindTexture(textureTarget, textureRef->textureID);
    PL_GL.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glPixelStorei(GL_UNPACK_ROW_LENGTH, (surface->pitch / surface->format->BytesPerPixel));
#else
#ifdef GL_UNPACK_ROW_LENGTH_EXT
    if (PL_GL.hasEXTUnpackSubimage == DXTRUE) {
        PL_GL.glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, (surface->pitch / surface->format->BytesPerPixel));
    }
#endif
#endif
    PL_GL.glTexSubImage2D(
        textureTarget, 0,
        rect->x, rect->y, rect->w, rect->h,
        textureRef->glFormat, textureRef->glType,
        surface->pixels
    );
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glDisable(textureTarget);
#endif
}

int PLGL_Texture_Bind(int textureRefID, int drawMode) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    GLuint textureTarget;
    if (textureref == NULL) {
        return -1;
    }
    
    textureTarget = textureref->glTarget;
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glEnable(textureTarget);
#endif
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

int PLGL_Texture_Unbind(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glDisable(textureref->glTarget);
#endif
    return 0;
}

int PLGL_Texture_BindFramebuffer(int textureRefID, int renderbufferID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    int framebufferID = -1;
    GLenum textureTarget = GL_TEXTURE_2D;
    GLuint textureID = 0;
    int retval;
    if (textureref != NULL) {
        framebufferID = textureref->framebufferID;
        textureID = textureref->textureID;
        textureTarget = textureref->glTarget;
    }
    
    retval = s_GLFrameBuffer_Bind(framebufferID, textureTarget, textureID, renderbufferID);
    
    if (retval >= 0 && textureref != NULL && textureref->framebufferNeedsClear == TRUE) {
        textureref->framebufferNeedsClear = FALSE;
        if (textureref->hasAlphaChannel == FALSE) {
            PL_GL.glClearColor(0, 0, 0, 1);
        } else {
            PL_GL.glClearColor(0, 0, 0, 0);
        }
        PL_GL.glClear(GL_COLOR_BUFFER_BIT);
    }
    
    return retval;
}

int PLGL_Texture_HasAlphaChannel(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return 0;
    }
    return textureref->hasAlphaChannel;
}

int PLGL_Texture_CreateFromSDLSurface(SDL_Surface *surface, int hasAlphaChannel) {
    int textureRefID;
    
    if (SDL_GetColorKey(surface, 0) >= 0) {
        hasAlphaChannel = DXTRUE;
    }
    
    textureRefID = PLGL_Texture_CreateFromDimensions(surface->w, surface->h, hasAlphaChannel);
    if (textureRefID < 0) {
        return -1;
    }
    
    PLGL_Texture_BlitSurface(textureRefID, surface, NULL);
    
    return textureRefID;
}

int PLGL_Texture_CreateFromDimensions(int width, int height, int hasAlphaChannel) {
    int textureRefID;
    TextureRef *textureref;
    GLint textureInternalFormat = 0;
    GLenum textureFormat = 0;
    GLenum textureType = 0;
    GLuint textureID = 0;
    GLenum textureTarget;
    int texWidth, texHeight;
    int widthpow2, heightpow2;
    int wrappableFlag;
    Uint32 sdlFormat;
    
    /* - Determine if the source is of 2^n dimensions */
    widthpow2 = s_topow2(width);
    heightpow2 = s_topow2(height);
    if (widthpow2 == width && heightpow2 == height) {
        wrappableFlag = DXTRUE;
    } else {
        wrappableFlag = DXFALSE;
    }

    /* - If not 2^n, use TEXTURE_RECTANGLE if available. */
    if (wrappableFlag == DXFALSE && PL_GL.hasNPTSupport) {
        textureTarget = GL_TEXTURE_2D;
        texWidth = width;
        texHeight = height;
    } else
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (wrappableFlag == DXFALSE && PL_GL.hasTextureRectangleSupport) {
        textureTarget = GL_TEXTURE_RECTANGLE_ARB;
        texWidth = width;
        texHeight = height;
    } else
#endif
    {
        textureTarget = GL_TEXTURE_2D;
        texWidth = widthpow2;
        texHeight = heightpow2;
    }
    
    /* - Verify if in bounds, set format */
    if (texWidth > PL_GL.maxTextureWidth || texHeight > PL_GL.maxTextureHeight) {
        return -1;
    }
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    textureInternalFormat = GL_RGBA;
    textureFormat = GL_RGBA;
    textureType = GL_UNSIGNED_BYTE;
    sdlFormat = SDL_PIXELFORMAT_ABGR8888;
#else
    textureInternalFormat = GL_RGBA;
    textureFormat = GL_RGBA;
    sdlFormat = SDL_PIXELFORMAT_ABGR8888;
    textureType = GL_UNSIGNED_BYTE;
#endif
    
    /* - Create the texture itself. */
    PL_GL.glGetError(); /* Clear the error buffer */
    PL_GL.glGenTextures(1, &textureID);
    if (PL_GL.glGetError() != GL_NO_ERROR) {
        return -1;
    }
    
    PL_GL.glGetError();
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glEnable(textureTarget);
#endif
    PL_GL.glBindTexture(textureTarget, textureID);
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    if (wrappableFlag == DXFALSE) {
        PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
        PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    
    PL_GL.glTexImage2D(
            textureTarget, 0, textureInternalFormat,
            texWidth, texHeight,
            0, textureFormat, textureType, NULL
        );
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glDisable(textureTarget);
#endif
    if (PL_GL.glGetError() != GL_NO_ERROR) {
        PL_GL.glDeleteTextures(1, &textureID);
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
    textureref->sdlFormat = sdlFormat;
    textureref->width = width;
    textureref->height = height;
    textureref->texWidth = texWidth;
    textureref->texHeight = texHeight;
    textureref->drawMode = DX_DRAWMODE_NEAREST;
    textureref->hasAlphaChannel = hasAlphaChannel;
    textureref->wrappableFlag = wrappableFlag;
    textureref->framebufferID = -1;
    textureref->framebufferNeedsClear = FALSE;
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    if (textureTarget == GL_TEXTURE_RECTANGLE_ARB) {
        textureref->widthMult = 1.0f;
        textureref->heightMult = 1.0f;
    } else
#endif
    {
        textureref->widthMult = 1.0f / (float)texWidth;
        textureref->heightMult = 1.0f / (float)texHeight;
    }
    
    return textureRefID;
}

int PLGL_Texture_CreateFramebuffer(int width, int height, int hasAlphaChannel) {
    int textureRefID = -1;
    int framebufferID = -1;
    TextureRef *textureref;
    
    textureRefID = PLGL_Texture_CreateFromDimensions(width, height, hasAlphaChannel);
    if (textureRefID < 0) {
        return -1;
    }
    
    textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    
    framebufferID = s_GLFrameBuffer_Create(width, height);
    if (framebufferID < 0) {
        PLGL_Texture_Release(textureRefID);
        return -1;
    }
    
    textureref->framebufferID = framebufferID;
    textureref->framebufferNeedsClear = TRUE;
    
    return textureRefID;
}

int PLGL_Texture_SetWrap(int textureRefID, int wrapState) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    GLuint textureTarget;
    GLenum wrapMode;
    if (textureref == NULL || textureref->textureID == 0) {
        return -1;
    }
    
    textureTarget = textureref->glTarget;
    
    wrapMode = GL_CLAMP_TO_EDGE;
    if (wrapState == DXTRUE && textureref->wrappableFlag == DXTRUE) {
        wrapMode = GL_REPEAT;
    }
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glEnable(textureTarget);
#endif
    PL_GL.glBindTexture(textureTarget, textureref->textureID);
    
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, wrapMode);
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, wrapMode);
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glDisable(textureTarget);
#endif
    
    return 0;
}

int s_glSetFilter(int textureRefID, GLint minFilter, GLint magFilter) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    GLuint textureTarget;
    if (textureref == NULL || textureref->textureID == 0) {
        return -1;
    }
    
    textureTarget = textureref->glTarget;
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glEnable(textureTarget);
#endif
    PL_GL.glBindTexture(textureTarget, textureref->textureID);
    
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, minFilter);
    PL_GL.glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, magFilter);
    
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
    PL_GL.glDisable(textureTarget);
#endif
    
    return 0;
}

int PLGL_Texture_AddRef(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
    textureref->base.refCount += 1;
    return 0;
}

int PLGL_Texture_Release(int textureRefID) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    if (textureref == NULL) {
        return -1;
    }
    
    textureref->base.refCount -= 1;
    if (textureref->base.refCount <= 0) {
        if (textureref->base.releaseFunc != NULL) {
            textureref->base.releaseFunc(textureRefID);
        }
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

int PLGL_Texture_BlitSurface(int textureRefID, SDL_Surface *surface, const PLRect *rect) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    PLRect tempRect;
    
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

int PLGL_Texture_RenderGetTextureInfo(int textureRefID, PLRect *rect, float *xMult, float *yMult) {
    TextureRef *textureref = (TextureRef*)PL_Handle_GetData(textureRefID, DXHANDLE_TEXTURE);
    
    if (textureref == NULL) {
        return -1;
    }
    
    if (rect != NULL) {
        rect->x = 0;
        rect->y = 0;
        rect->w = textureref->width;
        rect->h = textureref->height;
    }
    if (xMult != NULL) {
        *xMult = textureref->widthMult;
    }
    if (yMult != NULL) {
        *yMult = textureref->heightMult;
    }
    
    return 0;
}

#ifdef DXPORTLIB_DRAW_OPENGL_ES2
#define GL_DEPTH24_STENCIL8 (GL_RGBA4)
#endif

int PLGL_Renderbuffer_Create(int width, int height) {
    int renderbufferID = PL_Handle_AcquireID(DXHANDLE_RENDERBUFFER);
    RenderbufferInfo *info;
    
    if (renderbufferID <= 0) {
        return -1;
    }
    
    info = (RenderbufferInfo *)PL_Handle_AllocateData(renderbufferID, sizeof(RenderbufferInfo));
    PL_GL.glGenRenderbuffers(1, &info->renderbufferID);
    PL_GL.glBindRenderbuffer(GL_RENDERBUFFER, info->renderbufferID);
    PL_GL.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    PL_GL.glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    info->width = width;
    info->height = height;
    info->refCount = 1;
    
    return renderbufferID;
}

int PLGL_Renderbuffer_Release(int renderbufferID) {
    RenderbufferInfo *info;
    info = (RenderbufferInfo *)PL_Handle_GetData(renderbufferID, DXHANDLE_RENDERBUFFER);
    if (info == NULL) {
        return -1;
    }
    
    info->refCount -= 1;
    if (info->refCount > 0) {
        return 0;
    }
    
    PL_GL.glDeleteRenderbuffers(1, &info->renderbufferID);
    info->renderbufferID = 0;
    
    return 0;
}

int PLGL_Texture_ClearAllData() {
    /* We can't actually get rid of the handles,
     * but we can toast the data inside. */
    int textureRefID;
    int renderbufferID;
    
    s_GLFrameBuffer_Bind(-1, 0, 0, -1);
    
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
    
    renderbufferID = PL_Handle_GetFirstIDOf(DXHANDLE_RENDERBUFFER);
    while (renderbufferID >= 0) {
        RenderbufferInfo *renderInfo = (RenderbufferInfo *)PL_Handle_GetData(renderbufferID, DXHANDLE_RENDERBUFFER);
        
        if (renderInfo != NULL) {
            if (renderInfo->renderbufferID) {
                PL_GL.glDeleteRenderbuffers(1, &renderInfo->renderbufferID);
                renderInfo->renderbufferID = 0;
            }
        }
        
        renderbufferID = PL_Handle_GetNextID(renderbufferID);
    }
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */

