/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2016 Patrick McCarthy <mauve@sandwich.net>
  
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

/* Both 2D and 3D versions of LunaSprite are handled here because they are
 * almost identical.
 *
 * Various vertex effects are not supported because there's no need for
 * them.
 */

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"

LSURFACE LunaSurface::CreateDepthStencil(Uint32 width, Uint32 height, eSurfaceFormat format)
{
    int renderbufferID = PLG.Renderbuffer_Create((int)width, (int)height);
    
    return (LSURFACE)(renderbufferID);
}

void LunaSurface::Release(LSURFACE surface) {
    PLG.Renderbuffer_Release((int)surface);
}

#endif
