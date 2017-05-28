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

#ifndef DXPORTLIB_DXBLENDMODES_H_HEADER
#define DXPORTLIB_DXBLENDMODES_H_HEADER

#include "PL/PLInternal.h"

typedef struct _BlendInfo {
    TexturePreset texturePreset;
    int blendEquation;
    int srcRGBBlend;
    int destRGBBlend;
    int srcAlphaBlend;
    int destAlphaBlend;
} BlendInfo;

#define NOBLEND (0xffffffff)

static const BlendInfo s_blendModeTable[DX_BLENDMODE_NUM] = {
    /* PL_BLEND_ONE = s */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_DISABLE, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ONE },
    /* ALPHA = (d*(1-a)) + (s*a) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ONE },
    /* ADD = d + (s*a) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* SUB = d - (s*a) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_RSUB, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* MUL = (d*s) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_ZERO, PL_BLEND_SRC_COLOR, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* SUB2 = d + (s*a) */ /* It's the same as ADD. I don't get it, either. */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    
    /* XOR (unsupported) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_DISABLE, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ONE },
    /* reserved */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_DISABLE, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ONE },

    /* DESTCOLOR = d */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_ZERO, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* INVDESTCOLOR = 1 - d */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_ONE_MINUS_DST_COLOR, PL_BLEND_ZERO, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* INVSRC = (d*(1-a)) + ((1-s)*a) */
    { TEX_PRESET_DX_INVERT, PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ZERO, PL_BLEND_ONE },
    
    /* MULA = (d*(1-a)) + (s*d*a) */
    { TEX_PRESET_DX_MULA,   PL_BLENDFUNC_ADD, PL_BLEND_DST_COLOR, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ZERO, PL_BLEND_ONE },
    
    /* ALPHA_X4 = (d*(1-a)) + (clamp(s*4)*a) */
    { TEX_PRESET_DX_X4,     PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ONE_MINUS_SRC_ALPHA },
    /* ADD_X4 = d + ((clamp(s*4)*a) */
    { TEX_PRESET_DX_X4,     PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* SRCCOLOR = s */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_DISABLE, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE, PL_BLEND_ZERO },
    /* HALF_ADD = (d*(1-a)) + s */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_ONE, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ONE },
    /* SUB1 = d - (s*(1-a)) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_RSUB, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    
    /* PMA_ALPHA = (d*(1-a)) + s */
    { TEX_PRESET_DX_PMA,    PL_BLENDFUNC_ADD, PL_BLEND_ONE, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* PMA_ADD = d + s */
    { TEX_PRESET_DX_PMA,    PL_BLENDFUNC_ADD, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* PMA_SUB = d - s */
    { TEX_PRESET_DX_PMA,    PL_BLENDFUNC_RSUB, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* PMA_INVSRC = d - (1 - s) */
    { TEX_PRESET_DX_PMA_INVERT,PL_BLENDFUNC_ADD, PL_BLEND_ONE, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* PMA_ALPHA_X4 = (d*(1-a)) + clamp(s*4) */
    { TEX_PRESET_DX_PMA_X4, PL_BLENDFUNC_ADD, PL_BLEND_ONE, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ZERO, PL_BLEND_ONE },
    /* PMA_ADD_X4 = d + clamp(s*4) */
    { TEX_PRESET_DX_PMA_X4, PL_BLENDFUNC_ADD, PL_BLEND_ONE, PL_BLEND_ONE, PL_BLEND_ZERO, PL_BLEND_ONE},
};

static const BlendInfo s_blendModeEXTTable[DX_BLENDMODE_EXT_END - DX_BLENDMODE_EXT] = {
    /* PL_BLEND_EXT_PS_ALPHA = (c = (d*(1-a)) + (s*a)), (a = (d*(1-a)) + s) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ONE, PL_BLEND_ONE_MINUS_SRC_ALPHA },
    /* PL_BLEND_EXT_DSTALPHA = (c = (d*(1-a)) + (s*a)), (a = d) */
    { TEX_PRESET_MODULATE,  PL_BLENDFUNC_ADD, PL_BLEND_SRC_ALPHA, PL_BLEND_ONE_MINUS_SRC_ALPHA, PL_BLEND_ZERO, PL_BLEND_ONE },
};

#endif
