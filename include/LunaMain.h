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

/* Replicates the Luna's Main functionality.
 * 
 * To use, do the following:
 * - Replace Luna::Main with LUNAMAIN
 * - Replace Luna::Init with LUNAINIT
 * - Replace Luna::MessageProc with LUNAMESSAGEPROC
 *
 * Compatible with both standard Luna and DxPortLib Luna.
 *
 * If you're doing another platform with a different entrypoint, replace
 * as needed.
 */

#ifndef LUNAPORTLIB_MAIN_H
#define LUNAPORTLIB_MAIN_H

#include "Luna.h"

#if defined(DXPORTLIB_LUNA)

/* DxPortLib does not use LunaMessageProc. */

#define LUNAMAIN LunaMain
#define LUNAINIT LunaInit
#define LUNAMESSAGEPROC LunaMessageProc

extern Bool LunaInit();
extern void LunaMain(Sint32 argc, char *argv[]);
extern Bool LunaMessageProc( Sint32 Msg, Sint32 wParam, Sint32 lParam );

#ifdef _WIN32
/* for _argc/_argv */
#include <stdlib.h>

/* WinMain - Used for non-Console build so stacking with main() is okay */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR Cmd, INT Show) {
    return Luna::BootMain(__argc, __argv, &LUNAINIT, &LUNAMAIN, &LUNAMESSAGEPROC);
}
#endif

/* main() */
int main(int argc, char **argv) {
    return Luna::BootMain(argc, argv, &LUNAINIT, &LUNAMAIN, &LUNAMESSAGEPROC);
}

#else

#define LUNAMAIN Luna::Main
#define LUNAINIT Luna::Init
#define LUNAMESSAGEPROC Luna::MessageProc

#endif /* #if defined(___LUNA_DX9___) */

#endif /* #define LUNAPORTLIB_MAIN_H */
