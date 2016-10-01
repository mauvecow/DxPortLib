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

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"

static const int s_buttonCount = (PAD_BUTTON_MAX > 32 ? 32 : PAD_BUTTON_MAX);

static bool s_mouseFirst = false;
static int s_mouseLastX = -1;
static int s_mouseLastY = -1;
static int s_mouseLastInput = 0;
static int s_mouseInput = 0;
static int s_mousePressed = 0;
static int s_mouseReleased = 0;
static int s_mouseLastClicks[3] = { 0, 0, 0 };
static const int s_mouseDoubleClickThreshold = 100;

void LunaInput::KeyBufferFlush() {
    PL_Input_ResetKeyBuffer();
}

Bool LunaInput::GetKeyData(Uint32 Key) {
    if (PL_Input_CheckHitKey(Key) <= 0) {
        return false;
    } else {
        return true;
    }
}

static void s_DoClick(int index, int t, Bool *pClick, Bool *pDoubleClick, eMouseState *pState) {
    int flag = 1 << index;
    *pDoubleClick = false;
    if ((s_mousePressed & flag) != 0) {
        *pClick = true;
        *pState = MOUSE_PUSH;
        if ((t - s_mouseLastClicks[index]) > s_mouseDoubleClickThreshold) {
            *pDoubleClick = true;
        }
    } else if ((s_mouseReleased & flag) != 0) {
        *pClick = false;
        *pState = MOUSE_PULL;
    } else if ((s_mouseInput & flag) != 0) {
        *pClick = false;
        *pState = MOUSE_HOLD;
    } else {
        *pClick = false;
        *pState = MOUSE_FREE;
    }
}

void LunaInput::GetMouseData(MOUSEDATA *pMouse) {
    int x, y;
    PL_Window_GetMousePoint(&x, &y);
    
    // cursor data
    pMouse->X = x;
    pMouse->Y = y;
    pMouse->W = PL_Input_GetMouseWheelRotVol(DXFALSE);
    
    if (s_mouseFirst == false) {
        pMouse->Mx = x - s_mouseLastX;
        pMouse->My = y - s_mouseLastY;
    } else {
        s_mouseFirst = false;
        pMouse->Mx = 0;
        pMouse->My = 0;
    }
    s_mouseLastX = x;
    s_mouseLastY = y;
    
    // click data
    int t = PL_Platform_GetTicks();
    s_mouseLastInput = s_mouseInput;
    s_mouseInput = PL_Window_GetMouseInput();
    s_mousePressed = s_mouseInput & ~s_mouseLastInput;
    s_mouseReleased = ~s_mouseInput & s_mouseLastInput;
    
    s_DoClick(0, t, &pMouse->ClickL, &pMouse->DoubleClickL, &pMouse->StateL);
    s_DoClick(1, t, &pMouse->ClickR, &pMouse->DoubleClickR, &pMouse->StateR);
    s_DoClick(2, t, &pMouse->ClickW, &pMouse->DoubleClickW, &pMouse->StateW);
}

Bool LunaInput::GetJoystickData(Sint32 No, JOYSTICKDATA *pJoy) {
    DINPUT_JOYSTATE dxState;
    int i;
    
    if (PL_Input_GetJoypadDirectInputState(No + 1, &dxState) < 0) {
        return false;
    }
    
    memset(pJoy, 0, sizeof(JOYSTICKDATA));
    
    pJoy->Px = dxState.X * 32767 / 1000;
    pJoy->Py = dxState.Y * 32767 / 1000;
    pJoy->Pz = dxState.Z * 32767 / 1000;
    pJoy->Rx = dxState.Rx * 32767 / 1000;
    pJoy->Ry = dxState.Ry * 32767 / 1000;
    pJoy->Rz = dxState.Rz * 32767 / 1000;
    pJoy->Slider[0] = dxState.Slider[0];
    pJoy->Slider[1] = dxState.Slider[1];
    pJoy->Pov[0] = (Sint32)dxState.POV[0];
    pJoy->Pov[1] = (Sint32)dxState.POV[1];
    pJoy->Pov[2] = (Sint32)dxState.POV[2];
    pJoy->Pov[3] = (Sint32)dxState.POV[3];
    
    for (i = 0; i < 4; ++i) {
        if (pJoy->Pov[i] >= 0) {
            pJoy->Pov[i] /= 100;
        }
    }
    
    for (i = 0; i < s_buttonCount; ++i) {
        pJoy->Button[i] = (dxState.Buttons[i] != 0) ? 1 : 0;
    }
    
    return true;
}

Bool LunaInput::KeyBufferLoop(Uint32 *pKey) {
    int key = PL_Input_GetFromKeyBuffer();
    if (key == 0) {
        return false;
    }
    
    *pKey = key;
    return true;
}

void LunaInput_Refresh() {
    PL_Input_ResetKeyBuffer();
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
