/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2014 Patrick McCarthy <mauve@sandwich.net>

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

/* Replicates the Luna's Main functionality. Include in your main.cpp
 * equivalent as follows:

#ifdef DXPORTLIB_LUNA
#  include "LunaMain.h"
#endif

 * If you're doing another platform with a different entrypoint, replace
 * as needed.
 */

#ifndef LUNAPORTLIB_MAIN_H
#define LUNAPORTLIB_MAIN_H

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

/* DxPortLib does not use LunaMessageProc. */

#define LUNAMAIN LunaMain
#define LUNAINIT LunaInit
#define LUNAMESSAGEPROC LunaMessageProc

extern Bool LunaInit();
extern void LunaMain(Sint32 argc, char *argv[]);

#ifdef WIN32
/* for _argc/_argv */
#include <stdlib.h>

/* WinMain */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR Cmd, INT Show) {
    return Luna::BootMain(__argc, __argv, &LUNAINIT, &LUNAMAIN);
}
#else
/* main() */
int main(int argc, char **argv) {
    return Luna::BootMain(argc, argv, &LUNAINIT, &LUNAMAIN);
}
#endif

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */

#endif /* #define LUNAPORTLIB_MAIN_H */
