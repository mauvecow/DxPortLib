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

#ifndef DXPORTLIB_SDL2_DXINTERNAL_H_HEADER
#define DXPORTLIB_SDL2_DXINTERNAL_H_HEADER

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_PLATFORM_SDL2

#include "PL/PLInternal.h"

extern SDL_RWops *PLSDL2_FileToRWops(int fileHandle);
extern int PLSDL2_RWopsToFile(SDL_RWops *rwops);
extern SDL_RWops *PLSDL2_FileOpenReadDirect(const DXCHAR *filename);

/* ------------------------------------------------------------- Screen.c */

extern void PL_SDL2GL_Init(SDL_Window *window, int width, int height, int vsyncFlag);
extern void PL_SDL2GL_End();

extern int PL_drawOffscreen;

#endif /* #ifdef DXPORTLIB_PLATFORM_SDL2 */

#endif /* #ifndef DXPORTLIB_SDL2_DXINTERNAL_H_HEADER */
