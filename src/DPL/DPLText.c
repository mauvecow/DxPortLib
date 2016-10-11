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

#include "DxPortLib_c.h"

#include "PL/PLInternal.h"

int g_dplDefaultEncoding = DPL_CHARSET_DEFAULT;

int DPL_Text_SetDefaultEncoding(int encoding) {
    g_dplDefaultEncoding = encoding;
    return 0;
}

int DPL_Text_ConvertStringEncoding(
    char *buffer, int bufferLength,
    const char *string,
    int destEncoding, int srcEncoding)
{
    if (destEncoding <= DPL_CHARSET_DEFAULT) {
        destEncoding = g_dplDefaultEncoding;
    }
    if (srcEncoding <= DPL_CHARSET_DEFAULT) {
        srcEncoding = g_dplDefaultEncoding;
    }
    return PL_Text_ConvertStrncpy(
            buffer, destEncoding,
            string, srcEncoding,
            bufferLength);
}

int DPL_Text_ConvertStringToChar(
    char *buffer, int bufferLength,
    const wchar_t *string,
    int destEncoding)
{
    if (destEncoding <= DPL_CHARSET_DEFAULT) {
        destEncoding = g_dplDefaultEncoding;
    }
    return PL_Text_WideCharToString(buffer, destEncoding, string, bufferLength);
}

int DPL_Text_ConvertStringToWideChar(
    wchar_t *buffer, int bufferLength,
    const char *string,
    int srcEncoding)
{
    if (srcEncoding <= DPL_CHARSET_DEFAULT) {
        srcEncoding = g_dplDefaultEncoding;
    }
    return PL_Text_StringToWideChar(buffer, string, srcEncoding, bufferLength);
}
