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

#include "SDL_main.h"

int main(int argc, char **argv) {
    SetUseCharSet(DX_CHARSET_EXT_UTF8);
    
    SetWindowText(_T("DxPortLib Test App"));
    SetWindowSizeChangeEnableFlag(DXTRUE);
    
    SetGraphMode(640, 480, 32);
    ChangeWindowMode(DXTRUE);
    
    if (DxLib_Init() == -1) {
        return -1;
    }
    
    int isWindowed = DXTRUE;
    int wasPressed = 0;
    int timerDelta = 0;
    int timeLast = GetNowCount();
    
    int pixelGraph = LoadGraph("pixel.png");
    
    while (ProcessMessage() == 0
#ifndef DX_NON_INPUT
        && CheckHitKey(KEY_INPUT_ESCAPE) == 0
#endif
    ) {
        /* If Alt+Enter is pressed, flip to fullscreen mode. */
        if (CheckHitKey(KEY_INPUT_RETURN)
            && (CheckHitKey(KEY_INPUT_LALT) || CheckHitKey(KEY_INPUT_RALT))
        ) {
            if (wasPressed == 0) {
                isWindowed = 1 - isWindowed;
                ChangeWindowMode(isWindowed);
            }
            wasPressed = 1;
        } else {
            wasPressed = 0;
        }
        
        /* Draw logic here */
        SetDrawBright(255, 255, 255);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
        DrawFillBox(0, 0, 640, 480, 0x000000);
        DrawLineBox(10, 10, 630, 470, 0xFFFFFF);
        DrawFillBox(20, 20, 620, 460, GetColor(0x48, 0x40, 0x38));
        
        for (int x = 20 - 440; x < 620 + 440; x += 40) {
            int x1, y1, x2, y2;
            
            x1 = x;
            y1 = 20;
            if (x1 < 20) {
                y1 += 20 - x1;
                x1 = 20;
            }
            if (x1 > 620) {
                y1 += x1 - 620;
                x1 = 620;
            }
            
            y2 = 460;
            x2 = x + (460 - 20);
            if (x2 > 620) {
                y2 -= (x2 - 620);
                x2 = 620;
            }
            if (y2 > 20) {
                DrawLine(x1, y1, x2, y2, GetColor(0xc0, 0xc0, 0x00), 2);
            }
            
            y2 = 460;
            x2 = x - (460 - 20);
            if (x2 < 20) {
                y2 -= (20 - x2);
                x2 = 20;
            }
            if (y2 > 20) {
                DrawLine(x1, y1, x2, y2, GetColor(0xc0, 0xc0, 0x00), 2);
            }
        }
        
        for (int i = 1; i < DX_BLENDMODE_NUM; ++i) {
            int color = GetColor(0x00, 0x80, 0x80);
            int x = ((i - 1) % 6);
            int y = ((i - 1) / 6);
            int x1 = ((640 - 540) / 2) + (90 * x);
            int y1 = 40 + (90 * y);
            SetDrawBlendMode(i, 128);
            SetDrawBright(0xff, 0xff, 0xff);
            DrawFillBox(x1, y1, x1 + 70, y1 + 35, color);
            SetDrawBright(0x00, 0x80, 0x80);
            DrawExtendGraph(x1, y1 + 35, x1 + 70, y1 + 70, pixelGraph, DXTRUE);
        }
        
        ScreenFlip();

        /* Time to next frame automatically... */
        int newTime = GetNowCount();
        timerDelta += newTime - timeLast;
        timeLast = newTime;

        int n = timerDelta;
        if (n > 16) {
            n = 16;
        }
        timerDelta -= n;

        WaitTimer(16 - n);
    }
    
    DxLib_End();
    
    return 0;
}
