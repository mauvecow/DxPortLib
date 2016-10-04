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

 /* Common defines used in DxPortLib headers.
  *
  * Written to plain C standard.
  */

#ifndef DPLCOMMON_H_HEADER
#define DPLCOMMON_H_HEADER

#include "DPLBuildConfig.h"

#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
#  define TRUE (1)
#endif
#ifndef FALSE
#  define FALSE (0)
#endif

#if defined(__GNUC__)
#  define DPLINLINE __inline__
#elif defined(_MSC_VER) || defined(__BORLANDC__)
#  define DPLINLINE __inline
#else
#  define DPLINLINE inline
#endif

/* Specify DLL export for all functions. */
#if defined _WIN32
#  define DPLEXPORTFUNCTION __declspec(dllexport)
#else
#  if __GNUC__ >= 4
#    define DPLEXPORTFUNCTION __attribute__(( visibility("default") ))
#  elif __GNUC__ >= 2
#    define DPLEXPORTFUNCTION __declspec(dllexport)
#  else
#    define DPLEXPORTFUNCTION
#  endif
#endif

#ifdef DXPORTLIB_DPL_INTERFACE
#  define DPLCALL DPLEXPORTFUNCTION
#else
#  define DPLCALL
#endif

#ifndef NULL
#  define NULL (void *)(0)
#endif

#define DPLFALSE            (0)
#define DPLTRUE             (1)

#define DPL_CHARSET_DEFAULT      (0)

#ifndef DPLPORTLIB_NO_SJIS
#  define DPL_CHARSET_SHIFTJIS    (1)
#endif /* #ifndef DXPORTLIB_NO_SJIS */

#define DPL_CHARSET_EXT_UTF8     (0xff)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #ifndef DPLCOMMON_H_HEADER */
