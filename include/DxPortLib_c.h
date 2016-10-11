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

extern int DPLCALL DPL_Text_SetDefaultEncoding(int encoding);

extern int DPLCALL DPL_Text_ConvertStringEncoding(
    char *buffer, int bufferLength,
    const char *string,
    int destEncoding, int srcEncoding);

extern int DPLCALL DPL_Text_ConvertStringToChar(
    char *buffer, int bufferLength,
    const wchar_t *string,
    int destEncoding);

extern int DPLCALL DPL_Text_ConvertStringToWideChar(
    wchar_t *buffer, int bufferLength,
    const char *string,
    int srcEncoding);

extern DPLCALL int DPL_WinINI_Create();
extern DPLCALL int DPL_WinINI_CreateAndReadFile(
    const char *filename, int fileEncoding);
extern DPLCALL int DPL_WinINI_ReadFile(
    int handle, const char *filename, int fileEncoding);
extern DPLCALL int DPL_WinINI_WriteFile(
    int handle, const char *filename, int fileEncoding);
extern DPLCALL int DPL_WinINI_Release(
    int handle);
extern DPLCALL int DPL_WinINI_GetInt(
    int handle, const char *sectionName, const char *name, int defaultValue);
extern DPLCALL const char *DPL_WinINI_GetString(
    int handle, const char *sectionName, const char *name, const char *defaultValue);
extern DPLCALL int DPL_WinINI_SetInt(
    int handle, const char *sectionName, const char *name, int value);
extern DPLCALL int DPL_WinINI_SetString(
    int handle, const char *sectionName, const char *name, const char *value);
extern DPLCALL int DPL_WinINI_DeleteValue(
    int handle, const char *sectionName, const char *name);
extern DPLCALL int DPL_WinINI_DeleteSection(
    int handle, const char *sectionName);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef DXPORTLIB_C_H_HEADER */
