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

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    const int screenWidth = 1024;
    const int screenHeight = 768; 
    
    if (argc < 2) {
        printf("Usage: %s blah.ttf\n", argv[0]);
        return -1;
    }
    
    SetUseCharSet(DX_CHARSET_EXT_UTF8);
    
    SetWindowText("DxPortLib Test App");
    SetWindowSizeChangeEnableFlag(DXTRUE);
    
    SetGraphMode(screenWidth, screenHeight, 32);
    ChangeWindowMode(DXTRUE);
    
    if (DxLib_Init() == -1) {
        return -1;
    }
    
    EXT_MapFontFileToName(argv[1], "TestFont", -1, DXFALSE);
    EXT_MapFontFileToName(argv[1], "TestFontBold", -1, DXTRUE);
    
    int fonts[17];
    fonts[0] = CreateFontToHandle("TestFont", 10);
    fonts[1] = CreateFontToHandle("TestFont", 14);
    fonts[2] = CreateFontToHandle("TestFont", 18);
    fonts[3] = CreateFontToHandle("TestFont", 22);
    fonts[4] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_EDGE, -1, 1);
    fonts[5] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_EDGE, -1, 2);
    fonts[6] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_EDGE, -1, 3);
    fonts[7] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_EDGE, -1, 4);
    fonts[8] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_ANTIALIASING, -1, 1);
    fonts[9] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 1);
    fonts[10] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 2);
    fonts[11] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 3);
    fonts[12] = CreateFontToHandle("TestFont", 22, -1, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 4);
    fonts[13] = CreateFontToHandle("TestFontBold", 22);
    fonts[14] = CreateFontToHandle("TestFontBold", 22, -1, DX_FONTTYPE_EDGE, -1, 4);
    fonts[15] = CreateFontToHandle("TestFontBold", 22, -1, DX_FONTTYPE_ANTIALIASING, -1, 1);
    fonts[16] = CreateFontToHandle("TestFontBold", 22, -1, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 1);
    
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
        
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        
        for (int i = 0; i < 17; ++i) {
            DrawStringToHandle(30, 30 + (i * 32), "The quick brown fox jumped over the lazy dog. あいうえお", 0xffffff, fonts[i]);
        }
        
        ScreenFlip();
        WaitTimer(16);
    }
    
    DxLib_End();
    
    return 0;
}
