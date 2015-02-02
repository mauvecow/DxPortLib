/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013 Patrick McCarthy <mauve@sandwich.net>
  
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
 */

/* - Any comments starting with "-" are relevant to DxPortLib. */
#include "DxLib.h"

#ifndef DXLIB_VERSION

#include <stdio.h>

int main(int argc, char **argv) {
    printf("DxPortLib was compiled without DxLib support.\n");
    return -1;
}

#else

#ifdef _WIN32
#  ifdef DXPORTLIB
/* - OPTIONAL: If you want to use main() on Windows, include this
 *   and link SDL2main.lib. Otherwise ignore. */
#    include "SDL_main.h"
#  endif
#endif

/* - Non-Windows platforms do not use WinMain. */
int main(int argc, char **argv) {
    /* - DxPortLib requires the text encoding to be set early.
     * An extension is included that supports UTF8, which is the
     * ideal text encoding to use. This is the default.
     *
     * If you are porting a DX_CHARSET_SHFTJIS app, you will have to
     * set this.
     */
#ifdef DXPORTLIB
    SetUseCharSet(DX_CHARSET_EXT_UTF8);
#endif

    /* - DxPortLib cannot set WindowResize after the window has been
     *   created. */
    SetWindowSizeChangeEnableFlag(TRUE);
    
    /* - DxPortLib cannot set the VSync flag after the window has been
     *   created. */
    SetWaitVSyncFlag(TRUE);
    
    /* - DxPortLib does not support setting the color depth, so the given
     *   value will be ignored. It will always be 32bpp. */
    SetGraphMode(640, 480, 32);
    
    /* - DxPortLib defines the _T() and _TEXT() macro for all platforms.
     *   This chooses between Unicode or Multibyte text automatically. */
    SetWindowText(_T("Ported Application"));
    
    /* Normal DxLib initialization... */
    ChangeWindowMode(TRUE);
    
    DxLib_Init();
    
    /* - DxPortLib requires fonts be "mapped" before they can be used.
     *   System fonts are not accessible, so you must include a font
     *   and use EXT_MapFontFileToName.
     *   (all extensions have EXT_ somewhere.)
     *   Loading TTF files from DXA is supported.
     *
     * Usage:
     * EXT_MapFontFileToName(
     *     "filename.ttf",
     *     "Font Name",
     *     minimumThickness, 
     *     boldFontFlag
     * );
     *
     * The font mapping list may be cleared with
     * EXT_InitFontMappings();
     *
     * Clearing the mappings will not destroy any font handles.
     */
#ifdef DXPORTLIB
    EXT_MapFontFileToName("NotAnArialClone.ttf", "Arial", -1, FALSE);
    EXT_MapFontFileToName("BoldArialClone.ttf", "Arial", 8, TRUE);
#endif
    
    /* Creates a handle to the NotAnArialClone font. */
    int fontHandle = CreateFontToHandle("Arial", 22, -1);
    
    /* - DxPortLib does not have a "default" font.
     *   If you use the default font, you must set it manually.
     *
     * The default font will be used by any unspecified drawing
     * in the program.
     */
    
    /* Creates the BoldArialClone font as the default font. */
    SetFontSize(22);
    SetFontThickness(8);
    ChangeFont(_T("Arial"));
    /* - Note: DxPortLib will not actually create the default font
     *   until it is used. */
    
    SRand(0);
    /* Run a main loop. */
    while (ProcessMessage() == 0) {
        DrawFillBox(0, 0, 640, 480, GetColor(0x10, 0x20, 0x30));
        
        DrawString(
            20, 20,
            _T("This is the BoldArialClone font."),
            GetColor(GetRand(255), GetRand(255), GetRand(255))
        );
        DrawStringToHandle(
            20+GetRand(3), 80+GetRand(3),
            _T("This is the NotAnArialClone font."),
            GetColor(0xff, 0xff, 0x80),
            fontHandle
        );
        
        ScreenFlip();
        
        WaitTimer(16);
    }
    
    /* Finish up as normal. */
    DxLib_End();
    
    return 0;
}

#endif /* #ifdef DXLIB_VERSION */

