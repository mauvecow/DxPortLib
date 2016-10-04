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

/* Front-end to DxPortLib's interface.
 *
 * For a C++ interface, see DxPortLib.h.
 */

#ifndef DXPORTLIB_C_H_HEADER
#define DXPORTLIB_C_H_HEADER

#ifndef DPLCOMMON_H_HEADER
#  include "DPLCommon.h"
#endif

#ifndef DXPORTLIB_DPL_INTERFACE
#  undef DPLCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

int DPLCALL DPL_Text_ConvertStringEncoding(
    char *buffer, int bufferLength,
    const char *string,
    int destEncoding, int srcEncoding);

int DPLCALL DPL_Text_ConvertStringToChar(
    char *buffer, int bufferLength,
    const wchar_t *string,
    int destEncoding);

int DPLCALL DPL_Text_ConvertStringToWideChar(
    wchar_t *buffer, int bufferLength,
    const char *string,
    int srcEncoding);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef DXPORTLIB_C_H_HEADER */
