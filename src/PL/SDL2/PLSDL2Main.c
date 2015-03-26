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

#include "PLSDL2Internal.h"

#ifdef DXPORTLIB_PLATFORM_SDL2

#include "SDL.h"

#include "SDL_image.h"

PLIGraphics PLG;

/* For setting the floating precision to the system value.
 * There is no real architecture agnostic answer here, so
 * add more of these as needed. */
static SDL_INLINE void s_SetFPUState() {
#if defined(__GNUC__) && defined(i386)
    unsigned int mode;
    
    __asm__ volatile ("fnstcw %0" : "=m" (*&mode));
    
    mode &= ~0x300;
    mode |= 0x200;
    
    __asm__ volatile ("fldcw %0" :: "m" (*&mode));
#endif
}

void PL_Platform_Boot() {
    SDL_SetMainReady();
    
    s_SetFPUState();
}

void PL_Platform_Init() {
    SDL_Init(
        SDL_INIT_VIDEO
        | SDL_INIT_TIMER
#ifndef DX_NON_INPUT
        | SDL_INIT_JOYSTICK
        | SDL_INIT_GAMECONTROLLER
#endif  /* #ifndef DX_NON_INPUT */
#ifndef DX_NON_SOUND
        | SDL_INIT_AUDIO
#endif  /* #ifndef DX_NON_SOUND */
    );
}

void PL_Platform_Finish() {
    SDL_Quit();
}

int PL_Platform_GetTicks() {
    return SDL_GetTicks();
}

void PL_Platform_Wait(int ticks) {
    SDL_Delay(ticks);
}

int PL_Platform_GetSaveFolder(DXCHAR *buffer, int bufferLength,
                              const DXCHAR *org, const DXCHAR *app) {
    char *prefPath;
    
    if (bufferLength == 0) {
        return -1;
    }

#ifdef UNICODE
    {
        char orgBuf[4096];
        char appBuf[4096];
        PL_Text_DxStringToString(buffer, orgBuf, 4096, DX_CHARSET_EXT_UTF8);
        PL_Text_DxStringToString(buffer, appBuf, 4096, DX_CHARSET_EXT_UTF8);
        prefPath = SDL_GetPrefPath(orgBuf, appBuf);
    }
#else
    prefPath = SDL_GetPrefPath(org, app);
#endif
    if (prefPath == NULL) {
        buffer[0] = '\0';
        return 0;
    }
    
    PL_Text_DxStrncpyFromString(buffer, prefPath, bufferLength,
                                DX_CHARSET_EXT_UTF8);
    SDL_free(prefPath);
    return 0;
}

#endif /* #ifdef DXPORTLIB_PLATFORM_SDL2 */
