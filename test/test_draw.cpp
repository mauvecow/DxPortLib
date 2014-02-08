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

static const int BOUNCETHINGCOUNT = 50;

struct BounceThing {
    int x, y, w, h;
    int dx, dy;
    DXCOLOR color;
    int isCircle;
    int isFilled;
};

static BounceThing things[BOUNCETHINGCOUNT];

static void InitBounceThings() {
    for (int i = 0; i < BOUNCETHINGCOUNT; ++i) {
        BounceThing *thing = &things[i];
        int j;
        
        thing->w = 10 + GetRand(50);
        thing->h = 10 + GetRand(50);
        thing->x = 20 + GetRand(600 - thing->w);
        thing->y = 20 + GetRand(460 - thing->w);
        thing->dx = GetRand(4) - 2;
        thing->dy = GetRand(4) - 2;
        thing->color = GetColor(GetRand(255), GetRand(255), GetRand(255));
        
        j = i;
        thing->isCircle = 0;
        if (i >= (BOUNCETHINGCOUNT / 2)) {
            j = i - (BOUNCETHINGCOUNT / 2);
            thing->isCircle = 1;
        }
        thing->isFilled = (j < (BOUNCETHINGCOUNT / 4)) ? 1 : 0;
    }
}

static void MoveBounceThings() {
    for (int i = 0; i < BOUNCETHINGCOUNT; ++i) {
        BounceThing *thing = &things[i];
        
        thing->x += thing->dx;
        thing->y += thing->dy;
        if (thing->dx < 0 && thing->x < 20) {
            thing->dx = -thing->dx;
        }
        if (thing->dx > 0 && thing->x > (620 - thing->w)) {
            thing->dx = -thing->dx;
        }
        if (thing->dy < 0 && thing->y < 20) {
            thing->dy = -thing->dy; 
        }
        if (thing->dy > 0 && thing->y > (460 - thing->h)) {
            thing->dy = -thing->dy; 
        }
    }
}

static void DrawBounceThings() {
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    
    for (int i = 0; i < BOUNCETHINGCOUNT; ++i) {
        const BounceThing *thing = &things[i];
        
        if (thing->isCircle) {
            float halfW = (float)thing->w * 0.5f;
            float halfH = (float)thing->h * 0.5f;
            DrawOvalF(
                (float)thing->x + halfW, (float)thing->y + halfH,
                halfW, halfH,
                thing->color, thing->isFilled
            );
        } else {
            DrawBox(
                thing->x, thing->y,
                thing->x + thing->w, thing->y + thing->h,
                thing->color, thing->isFilled
            );
        }
    }
}

int main(int argc, char **argv) {
    SetUseCharSet(DX_CHARSET_EXT_UTF8);
    
    SetWindowText(_T("DxPortLib Test App"));
    SetWindowSizeChangeEnableFlag(DXTRUE);
    
    SetGraphMode(640, 480, 32);
    ChangeWindowMode(DXTRUE);
    
    if (DxLib_Init() == -1) {
        return -1;
    }
    
    SRand(0);
    InitBounceThings();
    
    int isWindowed = DXTRUE;
    int wasPressed = 0;
    int timerDelta = 0;
    int timeLast = GetNowCount();
    int screenshotWasPressed = 0;
    int drawScreen = MakeScreen(640, 480, DXFALSE);
    
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
        
        /* Game logic here */
        MoveBounceThings();
        
        /* Draw logic here */
        SetDrawScreen(drawScreen);
        SetDrawBright(255, 255, 255);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
        DrawFillBox(0, 0, 640, 480, 0xFF000000);
        DrawLineBox(10, 10, 630, 470, 0xFFFFFFFF);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawFillBox(20, 20, 620, 460, GetColor(0x90, 0x80, 0x70));
        
        DrawBounceThings();
        
        if (CheckHitKey(KEY_INPUT_S)) {
            if (screenshotWasPressed == 0) {
                SaveDrawScreenToPNG(0, 0, 640, 480, "test_draw_screenshot.png");
                screenshotWasPressed = 1;
            }
        } else {
            screenshotWasPressed = 0;
        }
        
        SetDrawScreen(DX_SCREEN_BACK);
        DrawGraph(0, 0, drawScreen, DXFALSE);
        
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
