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

#ifndef DXLIB_BUILDCONFIG_H_HEADER
#define DXLIB_BUILDCONFIG_H_HEADER

#define DXPORTLIB
#define DXPORTLIB_VERSION "0.3.0"

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
 */
#define DXPORTLIB_DXLIB_INTERFACE
#define DXPORTLIB_LUNA_INTERFACE

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

/* ------------------------------------------------------------------------
 * DxLib-specific features.
 */
/* Disables the archive format. */
/* #define DX_NON_DXA */

/* Disables the sound backend. */
/* #define DX_NON_SOUND */

/* Disables Ogg Vorbis support. (implied by NON_SOUND) */
/* #define DX_NON_OGGVORBIS */

/* Disables the input backend. */
/* #define DX_NON_INPUT */

/* Disables the font backend. */
/* #define DX_NON_FONT */

/* ------------------------------------------------------------------------
 * Luna-specific features.
 * No effect if DXPORTLIB_LUNA_INTERFACE is disabled.
 */

/* Instead of storing the math table in the binary, dynamically generates
 * it on startup. This is identical to LunaDx8's functionality, but is not
 * portable.
 * Reduces binary size by ~300k. */
/* #define DXPORTLIB_LUNA_DYNAMIC_MATH_TABLE */

/* ------------------------------------------------------------------------
 * These are features not supported by DxPortLib at this time.
 * 
 * Do not modify these.
 */

/* There is no inline asm in DxPortLib.
 * Modern compilers are smart, mmkay. */
#define DX_NON_INLINE_ASM

/* Disables built-in strings. We have none, so. */
#define DX_NON_LITERAL_STRING

/* Logging is not supported. */
#define DX_NON_LOG

/* Audio is always multithreaded. */
/* #define DX_NON_MULTITHREAD */

/* Handle error checking does not exist. */
#define DX_NON_HANDLE_ERROR_CHECK

/* DxLib normally has thread safety check functions that are enabled by this.
 * DxPortLib is not thread safe at this time.
 */
/* #define DX_THREAD_SAFE */

/* Because we are always thread safe, this is not necessary. */
/* #define DX_THREAD_SAFE_NETWORK_ONLY */

/* Async loading is not supported. */
#define DX_NON_ASYNCLOAD

/* Software image management is not supported. */
#define DX_NON_SOFTIMAGE

/* Movie playback (and thus OGG Theora) is not supported. */
#define DX_NON_MOVIE
#define DX_NON_OGGTHEORA

/* SDL2_image always supports these. */
/* #define DX_NON_TGA */
/* #define DX_NON_JPEGREAD */
/* #define DX_NON_PNGREAD */

/* ACM playback is not supported. */
#define DX_NON_ACM

/* Networking is not supported. */
#define DX_NON_NETWORK

/* GraphFilter/GraphBlend is not supported. */
#define DX_NON_FILTER

/* Software rendering is not supported. */
#define DX_NON_2DDRAW

/* Masking is not supported. */
#define DX_NON_MASK

/* DirectShow is not supported. */
#define DX_NON_DSHOW_MP3
#define DX_NON_DSHOW_MOVIE

/* KEYEX and INPUTSTRING are not currently supported. */
#define DX_NON_KEYEX
#define DX_NON_INPUTSTRING

/* Model loading is not supported. */
#define DX_NON_MODEL

/* FBX Models are not supported. */
#define DX_LOAD_FBX_MODEL

/* Only Mersenne Twister is supported. */
/* #define DX_NON_MERSENNE_TWISTER */

/* DxLib memory dumping is not supported. */
/* #define DX_USE_DXLIB_MEM_DUMP */

/* Bullet Physics is not supported. */
#define DX_NON_BULLET_PHYSICS

/* Beep sound is not supported. */
#define DX_NON_BEEP

/* Task switching must always be enabled. */
/* #define DX_NON_STOPTASKSWITCH */

/* No save functions are supported. */
#define DX_NON_SAVEFUNCTION

/* printfDx is not currently supported. */
#define DX_NON_PRINTF_DX

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

#endif /* _DXLIB_BUILDCONFIG_H */
