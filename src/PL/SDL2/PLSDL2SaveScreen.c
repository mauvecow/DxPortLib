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

#include "PL/PLInternal.h"

#ifdef DXPORTLIB_PLATFORM_SDL2

#include "PL/GL/PLGLInternal.h"

#include "SDL_image.h"

int PL_SaveDrawScreenToBMP(int x1, int y1, int x2, int y2,
                           const DXCHAR *filename) {
    return -1;
}

int PL_SaveDrawScreenToJPEG(int x1, int y1, int x2, int y2,
                            const DXCHAR *filename,
                            int quality, int sample2x1) {
    return -1;
}

int PL_SaveDrawScreenToPNG(int x1, int y1, int x2, int y2,
                           const DXCHAR *filename,
                           int compressionLevel) {
    SDL_Surface *surface;
    char namebuf[4096];
    PLRect rect;
    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;
    
    if (PLGL_Framebuffer_GetSurface(&rect, &surface) < 0) {
        return -1;
    }
    
    PL_Text_DxStringToString(filename, namebuf, 4096, DX_CHARSET_EXT_UTF8);
    
    return IMG_SavePNG(surface, namebuf);
}

#endif
