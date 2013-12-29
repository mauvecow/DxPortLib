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

#ifndef DXLIB_SDL2RENDER_DXINTERNAL_H_HEADER
#define DXLIB_SDL2RENDER_DXINTERNAL_H_HEADER

#include "DxInternal.h"

#include "SDL.h"

/* In the event this is not defined... */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

extern SDL_Renderer *PL_renderer;

extern int PL_Texture_RenderGetGraphTexture(int graphID, SDL_Texture **texture, SDL_Rect *rect);

#endif /* #ifndef _DXLIB_SDL2RENDER_DXINTERNAL_H */
