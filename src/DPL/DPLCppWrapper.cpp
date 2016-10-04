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

// This file contains the wrapper from C++ code to C.
//
// As there are only tail calls, each one will compile down to just a
// single jump instruction.

#include "DxPortLib.h"
#include "DxPortLib_c.h"

namespace DPL {

int Text::ConvertStringEncoding(
    char *buffer, int bufferLength,
    const char *string,
    int destEncoding, int srcEncoding)
{
    return DPL_Text_ConvertStringEncoding(
        buffer, bufferLength, string, destEncoding, srcEncoding);
}

int Text::ConvertStringToChar(
    char *buffer, int bufferLength,
    const wchar_t *string,
    int destEncoding)
{
    return DPL_Text_ConvertStringToChar(
        buffer, bufferLength, string, destEncoding);
}

int Text::ConvertStringToWideChar(
    wchar_t *buffer, int bufferLength,
    const char *string,
    int srcEncoding)
{
    return DPL_Text_ConvertStringToWideChar(
        buffer, bufferLength, string, srcEncoding);
}

} // namespace DPL
