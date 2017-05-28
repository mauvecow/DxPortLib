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

#ifndef DPLBUILDCONFIG_H_HEADER
#define DPLBUILDCONFIG_H_HEADER

#define DXPORTLIB
#define DXPORTLIB_VERSION "0.5.0"

#if defined(__APPLE__)
#  include "TargetConditionals.h"
#endif

/* ------------------------------------------------------------------------
 * These should be set at build time.
 *
 * In the original library, DX_NON_WHATEVER should be
 * treated as DX_WITHOUT_WHATEVER, and omits the given
 * section from the build entirely.
 */

/* DxPortLib - Enables and disables interfaces to the library.
 *
 * Disabling DxLib or Luna interfaces prevents compilation.
 *
 * Disabling the DPL interface will only prevent function exports.
 */
#define DXPORTLIB_DXLIB_INTERFACE
#define DXPORTLIB_LUNA_INTERFACE
#define DXPORTLIB_DPL_INTERFACE

/* DxPortLib - Choose one platform target.
 */
#define DXPORTLIB_PLATFORM_SDL2

/* DxPortLib - Disables SHIFT-JIS codepage. (-74kB binary size) */
/* #define DXPORTLIB_NO_SJIS */

/* DxPortLib extension - Sets the drawing backend.
 */
/* #define DXPORTLIB_DRAW_DIRECT3D9 */
#define DXPORTLIB_DRAW_OPENGL

/* For OpenGL, define this to use the OpenGL ES 2.0 support.
 * This is automatically enforced for Android, IOS, and Emscripten targets.
 */
/* #define DXPORTLIB_DRAW_OPENGL_ES2 */

/* Disables support for sound.
 */
/* #define DXPORTLIB_NO_SOUND */

/* Disables support for input
 */
/* #define DXPORTLIB_NO_INPUT */

/* Disables TTF font support.
 *
 * Currently only used by DxLib, but should be exported eventually.
 */
/* #define DXPORTLIB_NO_TTF_FONT */

/* DXPORTLIB_DXLIB_INTERFACE only: Disables the static math table for Luna.
 * Disables support for DxLib's archive format.
 */
/* #define DXPORTLIB_NO_DXLIB_DXA */

/* DXPORTLIB_LUNA_INTERFACE only: Disables the static math table for Luna.
 *
 * The static math table is required for 1:1 floating point compatibility
 * with the original library, but is otherwise a waste of 300k binary
 * space.
 *
 * Note that Luna's Dx8 version was always dynamic and had no static
 * table.
 */
/* #define DXPORTLIB_LUNA_DYNAMIC_MATH_TABLE */

/* ------------------------------------------------------------------------
 * Implicit/required supports are handled down here.
 */
#if defined(ANDROID) || defined(__ANDROID__) || defined(TARGET_OS_IPHONE) || defined(EMSCRIPTEN)
#  ifndef DXPORTLIB_DRAW_OPENGL_ES2
#    define DXPORTLIB_DRAW_OPENGL_ES2
#  endif
#endif

/* D3D9 not available on non-Windows platforms */
#if !defined(WIN32)
#  ifdef DXPORTLIB_DRAW_DIRECT3D9
#    undef DXPORTLIB_DRAW_DIRECT3D9
#  endif
#endif

#endif /* #ifndef DPLLIB_BUILDCONFIG_H_HEADER */
