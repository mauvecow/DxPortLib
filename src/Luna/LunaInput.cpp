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

Bool LunaInput::GetKeyData(Uint32 Key) {
    if (PL_Input_CheckHitKey(Key) <= 0) {
        return false;
    } else {
        return true;
    }
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
