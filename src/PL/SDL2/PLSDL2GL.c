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
#include "PLSDL2Internal.h"

static SDL_GLContext *s_context = NULL;
static int s_initialized = 0;

/* ------------------------------------------------------- Window context */

static void *s_GLGetProcAddress(const char *string) {
    return SDL_GL_GetProcAddress(string);
}
static int s_GLIsExtSupported(const char *string) {
    return SDL_GL_ExtensionSupported(string);
}

void PL_SDL2GL_Init(SDL_Window *window, int width, int height, int vsyncFlag) {
    int majorVersion, minorVersion;
    
    if (s_initialized) {
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
    
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);
    
    PLGL_Init(s_GLGetProcAddress, s_GLIsExtSupported,
              majorVersion, minorVersion);
    
    s_initialized = DXTRUE;
}

void PL_SDL2GL_UpdateVSync(int vsyncFlag) {
    if (s_initialized == DXFALSE) {
        return;
    }

    SDL_GL_SetSwapInterval((vsyncFlag != DXFALSE) ? 1 : 0);
}

void PL_SDL2GL_End() {
    if (s_initialized == DXFALSE) {
        return;
    }

    PLG.End();
    
    if (s_context != NULL) {
        SDL_GL_DeleteContext(s_context);
    }
    
    s_initialized = DXFALSE;
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
#endif /* #ifdef DXPORTLIB_PLATFORM_SDL2 */
