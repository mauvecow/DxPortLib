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

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"

/* All of this code functions in SJIS space. */

void LunaChara::Code2Str(Uint16 Code, char *pStr) {
    if (IsKanji((unsigned char)(Code & 0xff))) {
        pStr[0] = Code >> 8;
        pStr[1] = Code & 0xff;
        pStr[2] = 0;
    } else {
        pStr[0] = Code & 0xff;
        pStr[1] = 0;
    }
}

Bool LunaChara::IsKanji(unsigned char Chara) {
    return (Chara >= 0x81 && Chara <= 0x9f) || (Chara >= 0xe0 && Chara <= 0xfe);
}
Bool LunaChara::IsKana(unsigned char Chara) {
    return (Chara >= 0xa6 && Chara <= 0xdf);
}

Uint16 LunaChara::GetCharCode(const unsigned char *pStr) {
    if (IsKanji(pStr[0])) {
        return (pStr[1] | (pStr[0] << 8));
    } else {
        return pStr[0];
    }
}
Sint32 LunaChara::GetCharCodeEx(const unsigned char *pStr, Uint16 *pCode) {
    if (IsKanji(pStr[0])) {
        *pCode = (pStr[1] | (pStr[0] << 8));
        return 2;
    } else {
        *pCode = pStr[0];
        return 1;
    }
}

#endif
