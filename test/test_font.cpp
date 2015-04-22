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

#include "DxLib.h"

#ifdef DX_NON_FONT

#include <stdio.h>

int main(int argc, char **argv) {
    printf("DxPortLib was compiled without font support.\n");
    return -1;
}

#else /* #ifndef DX_NON_FONT */

#ifndef DXLIB_VERSION

#include <stdio.h>

int main(int argc, char **argv) {
    printf("DxPortLib was compiled without DxLib support.\n");
    return -1;
}

#else

#ifdef DXPORTLIB
#  include "SDL_main.h"
#endif

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    const int screenWidth = 1024;
    const int screenHeight = 768; 
    int fontBlendMode = DX_BLENDMODE_ALPHA;
    
    int n = 1;
    while (n < argc && argv[n][0] == '-') {
        if (!strcmp(argv[n], "-premultipliedalpha")) {
            fontBlendMode = DX_BLENDMODE_PMA_ALPHA;
            SetFontCacheUsePremulAlphaFlag(TRUE);
        }
        n += 1;
    }
     
    if (n >= argc) {
#ifdef DXPORTLIB
        printf("Usage: %s [-premultipliedalpha] fontfile.ttf\n", argv[0]);
#else
        printf("Usage: %s [-premultipliedalpha] \"Font Name\"\n", argv[0]);
#endif
        return -1;
    }
    
#ifdef DXPORTLIB
    SetUseCharSet(DX_CHARSET_EXT_UTF8);
#endif
    
    SetWindowText(_T("DxPortLib Test App"));
    SetWindowSizeChangeEnableFlag(TRUE);
    
    SetGraphMode(screenWidth, screenHeight, 32);
    ChangeWindowMode(TRUE);
    
    if (DxLib_Init() == -1) {
        return -1;
    }

    const TCHAR *fontName = "TestFont";
#ifdef DXPORTLIB
    EXT_MapFontFileToName(argv[n], fontName, -1, FALSE);
    EXT_MapFontFileToName(argv[n], fontName, 7, TRUE);
#else
    fontName = argv[n];
#endif
    
    int fonts[17];
    fonts[0] = CreateFontToHandle(fontName, 10, 4);
    fonts[1] = CreateFontToHandle(fontName, 14, 4);
    fonts[2] = CreateFontToHandle(fontName, 18, 4);
    fonts[3] = CreateFontToHandle(fontName, 22, 4);
    fonts[4] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_EDGE, -1, 1);
    fonts[5] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_EDGE, -1, 2);
    fonts[6] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_EDGE, -1, 3);
    fonts[7] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_EDGE, -1, 4);
    fonts[8] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_ANTIALIASING, -1, 1);
    fonts[9] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 1);
    fonts[10] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 2);
    fonts[11] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
    fonts[12] = CreateFontToHandle(fontName, 22, 4, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 4);
    fonts[13] = CreateFontToHandle(fontName, 22, 7);
    fonts[14] = CreateFontToHandle(fontName, 22, 7, DX_FONTTYPE_EDGE, -1, 4);
    fonts[15] = CreateFontToHandle(fontName, 22, 7, DX_FONTTYPE_ANTIALIASING, -1, 1);
    fonts[16] = CreateFontToHandle(fontName, 22, 7, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 1);
    
    while (ProcessMessage() == 0
#ifndef DX_NON_INPUT
        && CheckHitKey(KEY_INPUT_ESCAPE) == 0
#endif
    ) {
        /* Draw logic here */
        SetDrawBright(255, 255, 255);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
        DrawFillBox(0, 0, screenWidth, screenHeight, 0xFF000000);
        DrawLineBox(10, 10, screenWidth - 10, screenHeight - 10, 0xFFFFFFFF);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawFillBox(20, 20, screenWidth - 20, screenHeight - 20, GetColor(0x90, 0x80, 0x70));
        
        SetDrawBlendMode(fontBlendMode, 255);
        
        for (int i = 0; i < 17; ++i) {
            DrawStringToHandle(30, 30 + (i * 32), _T("The creeping coin does 123456789 damage!"), 0xffffff, fonts[i]);
            //DrawStringToHandle(30, 30 + (i * 32), _T("The quick brown fox jumped over the lazy dog. あいうえお"), 0xffffff, fonts[i]);
        }
        
        ScreenFlip();
        WaitTimer(16);
    }
    
    DxLib_End();
    
    return 0;
}

#endif /* #ifdef DXLIB_VERSION */

#endif /* #ifndef DX_NON_FONT */
