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

// This is a header for a C++ interface to DxPortLib.
//
// It is purely optional, and does not depend on STL or other external libraries.

#ifndef DXPORTLIB_H_HEADER
#define DXPORTLIB_H_HEADER

#ifndef DPLCOMMON_H_HEADER
#  include "DPLCommon.h"
#endif

namespace DPL {
    
namespace Text { // DPL::Text

int DPLCALL ConvertStringEncoding(
    char *buffer, int bufferLength,
    const char *string,
    int destEncoding = -1, int srcEncoding = -1);

int DPLCALL ConvertStringToChar(
    char *buffer, int bufferLength,
    const wchar_t *string,
    int destEncoding = -1);

int DPLCALL ConvertStringToWideChar(
    wchar_t *buffer, int bufferLength,
    const char *string,
    int srcEncoding = -1);

} // namespace DPL::Text

} // namespace DPL

#endif // #ifndef DXPORTLIB_H_HEADER
