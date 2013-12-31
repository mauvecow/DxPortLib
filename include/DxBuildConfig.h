/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013 Patrick McCarthy <mauve@sandwich.net>
  
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

/* ------------------------------------------------------------------------
 * These should be set at build time.
 * 
 * In the original library, DX_NON_WHATEVER should be
 * treated as DX_WITHOUT_WHATEVER, and omits the given
 * section from the build entirely.
 */

/* DxPortLib extension - Disables SHIFT-JIS codepage. (-74kB binary size)
 * If you do not use SJIS it is recommended to disable this.
 */
/* #define DXPORTLIB_NON_SJIS */

/* DxPortLib extension - Sets the drawing backend.
 * --- SELECT ONLY ONE --- */
/* #define DXPORTLIB_DRAW_SDL2_RENDER */
#define DXPORTLIB_DRAW_OPENGL

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

#endif /* _DXLIB_BUILDCONFIG_H */
