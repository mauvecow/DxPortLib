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

#include "PLInternal.h"

#include "SDL.h"

#ifndef DX_NON_INPUT

/* ------------------------------------------------------------ INPUT, YAY */

/* Some notes about SDL's joystick system:
 * - Internally, it stores active controllers as a linked list.
 *   If a controller is removed, the ones after are bumped 'forward' an ID.
 * - All controllers have a unique instance ID.
 * - All controllers have a unique GUID.
 * - On application startup, it "adds" all controllers to the game,
 *   but I don't really think this is worth trusting quite yet!
 * - If available, GameController is preferable to Joystick (see also: DS3)
 */

#define DX_INPUT_RANGE (1000)
#define DX_MAX_PADS (16)

#define KEY_BUFFER_MAX (32)

typedef struct Keybind {
    unsigned char key1;
    unsigned char key2;
    unsigned char key3;
    unsigned char key4;
} Keybind;
typedef struct KeybindTable {
    Keybind keys[32];
} KeybindTable;

typedef struct Controller {
    SDL_Joystick *joystick;
    SDL_GameController *gamecontroller;
    int instanceID;
    
    struct Controller *next;
} Controller;

static int s_inputInitialized = DXFALSE;
static Controller *s_controllerList = NULL;
static int s_controllerCount = 0;

static int s_mouseWheelX = 0;
static int s_mouseWheelY = 0;

static unsigned char s_keyTable[256];
static unsigned int s_downKeyCount = 0;

static int s_keyBufferTable[KEY_BUFFER_MAX];
static int s_keyBufferNext = 0;
static int s_keyBufferCount = 0;

static KeybindTable s_keybinds[DX_MAX_PADS];

static Controller *s_GetController(int ID) {
    Controller *c = s_controllerList;
    
    while (c != NULL && ID > 0) {
        ID -= 1;
        c = c->next;
    }
    
    return c;
}

static Controller *s_AddController(int ID) {
    Controller *controller = s_GetController(ID);
    Controller **target;
    
    if (s_GetController(ID)) {
        return controller;
    }
    
    /* Create controller. */
    controller = DXALLOC(sizeof(Controller));
    controller->joystick = SDL_JoystickOpen(ID);
    controller->gamecontroller = NULL;
    controller->instanceID = -1;
    
    if (controller->joystick != NULL) {
        controller->instanceID = SDL_JoystickInstanceID(controller->joystick);
    }
    
    if (SDL_IsGameController(ID)) {
        controller->gamecontroller = SDL_GameControllerOpen(ID);
    }
    
    /* Insert into list. */
    target = &s_controllerList;
    while (*target != NULL && ID > 0) {
        target = &(*target)->next;
        ID -= 1;
    }
    
    controller->next = *target;
    *target = controller;
    
    s_controllerCount += 1;
    
    return controller;
}

static void s_RemoveController(int instanceID) {
    Controller **target = &s_controllerList;
    int ID = 0;
    
    while (*target != NULL) {
        Controller *controller = *target;
        if (controller->instanceID == instanceID) {
            if (controller->joystick != NULL) {
                SDL_JoystickClose(controller->joystick);
            }
            if (controller->gamecontroller != NULL) {
                SDL_GameControllerClose(controller->gamecontroller);
            }
            
            *target = controller->next;
            
            DXFREE(controller);
            break;
        }
        
        target = &controller->next;
        ID += 1;
    }
    
    s_controllerCount -= 1;
}

static void s_OpenControllers() {
    int joystickCount;
    int i;
    
    joystickCount = SDL_NumJoysticks();
    for (i = 0; i < joystickCount; ++i) {
        s_AddController(i);
    }
}
static void s_CloseControllers() {
    while (s_controllerList != NULL) {
        s_RemoveController(s_controllerList->instanceID);
    }
}

static void s_InitializeInput() {
    if (s_inputInitialized == DXTRUE) {
        return;
    }
    
    s_keyBufferCount = 0;
    s_keyBufferNext = 0;
    
    s_mouseWheelX = 0;
    s_mouseWheelY = 0;
    
    s_controllerCount = 0;
    s_OpenControllers();
    
    /* set default keybinds */
    memset(s_keybinds, 0, sizeof(&s_keybinds));
    
    s_keybinds[0].keys[0].key1 = KEY_INPUT_NUMPAD2;
    s_keybinds[0].keys[0].key2 = KEY_INPUT_DOWN;
    s_keybinds[0].keys[1].key1 = KEY_INPUT_NUMPAD4;
    s_keybinds[0].keys[1].key2 = KEY_INPUT_LEFT;
    s_keybinds[0].keys[2].key1 = KEY_INPUT_NUMPAD6;
    s_keybinds[0].keys[2].key2 = KEY_INPUT_RIGHT;
    s_keybinds[0].keys[3].key1 = KEY_INPUT_NUMPAD8;
    s_keybinds[0].keys[3].key2 = KEY_INPUT_UP;
    
    s_keybinds[0].keys[4].key1 = KEY_INPUT_Z;
    s_keybinds[0].keys[5].key1 = KEY_INPUT_X;
    s_keybinds[0].keys[6].key1 = KEY_INPUT_C;
    s_keybinds[0].keys[7].key1 = KEY_INPUT_A;
    s_keybinds[0].keys[8].key1 = KEY_INPUT_S;
    s_keybinds[0].keys[9].key1 = KEY_INPUT_D;
    s_keybinds[0].keys[10].key1 = KEY_INPUT_Q;
    s_keybinds[0].keys[11].key1 = KEY_INPUT_W;
    s_keybinds[0].keys[12].key1 = KEY_INPUT_ESCAPE;
    s_keybinds[0].keys[13].key1 = KEY_INPUT_SPACE;
    
    memset(s_keyTable, 0, 256);
    s_downKeyCount = 0;
    
    s_inputInitialized = DXTRUE;
}

/* Converts from SDL keysym to DX key mapping. */
static int s_MapSDLKeyToDXKey(const SDL_Keysym *keysym) {
    switch(keysym->scancode) {
        case SDL_SCANCODE_A: return KEY_INPUT_A;
        case SDL_SCANCODE_B: return KEY_INPUT_B;
        case SDL_SCANCODE_C: return KEY_INPUT_C;
        case SDL_SCANCODE_D: return KEY_INPUT_D;
        case SDL_SCANCODE_E: return KEY_INPUT_E;
        case SDL_SCANCODE_F: return KEY_INPUT_F;
        case SDL_SCANCODE_G: return KEY_INPUT_G;
        case SDL_SCANCODE_H: return KEY_INPUT_H;
        case SDL_SCANCODE_I: return KEY_INPUT_I;
        case SDL_SCANCODE_J: return KEY_INPUT_J;
        case SDL_SCANCODE_K: return KEY_INPUT_K;
        case SDL_SCANCODE_L: return KEY_INPUT_L;
        case SDL_SCANCODE_M: return KEY_INPUT_M;
        case SDL_SCANCODE_N: return KEY_INPUT_N;
        case SDL_SCANCODE_O: return KEY_INPUT_O;
        case SDL_SCANCODE_P: return KEY_INPUT_P;
        case SDL_SCANCODE_Q: return KEY_INPUT_Q;
        case SDL_SCANCODE_R: return KEY_INPUT_R;
        case SDL_SCANCODE_S: return KEY_INPUT_S;
        case SDL_SCANCODE_T: return KEY_INPUT_T;
        case SDL_SCANCODE_U: return KEY_INPUT_U;
        case SDL_SCANCODE_V: return KEY_INPUT_V;
        case SDL_SCANCODE_W: return KEY_INPUT_W;
        case SDL_SCANCODE_X: return KEY_INPUT_X;
        case SDL_SCANCODE_Y: return KEY_INPUT_Y;
        case SDL_SCANCODE_Z: return KEY_INPUT_Z;

        case SDL_SCANCODE_1: return KEY_INPUT_1;
        case SDL_SCANCODE_2: return KEY_INPUT_2;
        case SDL_SCANCODE_3: return KEY_INPUT_3;
        case SDL_SCANCODE_4: return KEY_INPUT_4;
        case SDL_SCANCODE_5: return KEY_INPUT_5;
        case SDL_SCANCODE_6: return KEY_INPUT_6;
        case SDL_SCANCODE_7: return KEY_INPUT_7;
        case SDL_SCANCODE_8: return KEY_INPUT_8;
        case SDL_SCANCODE_9: return KEY_INPUT_9;
        case SDL_SCANCODE_0: return KEY_INPUT_0;
        
        case SDL_SCANCODE_RETURN: return KEY_INPUT_RETURN;
        case SDL_SCANCODE_ESCAPE: return KEY_INPUT_ESCAPE;
        case SDL_SCANCODE_BACKSPACE: return KEY_INPUT_BACK;
        case SDL_SCANCODE_TAB: return KEY_INPUT_TAB;
        case SDL_SCANCODE_SPACE: return KEY_INPUT_SPACE;

        case SDL_SCANCODE_MINUS: return KEY_INPUT_MINUS;
        case SDL_SCANCODE_EQUALS: return KEY_INPUT_EQUALS;
        case SDL_SCANCODE_LEFTBRACKET: return KEY_INPUT_LBRACKET;
        case SDL_SCANCODE_RIGHTBRACKET: return KEY_INPUT_RBRACKET;
        case SDL_SCANCODE_BACKSLASH: return KEY_INPUT_BACKSLASH;
        
        case SDL_SCANCODE_SEMICOLON: return KEY_INPUT_SEMICOLON;
        case SDL_SCANCODE_COMMA: return KEY_INPUT_COMMA;
        case SDL_SCANCODE_PERIOD: return KEY_INPUT_PERIOD;
        case SDL_SCANCODE_SLASH: return KEY_INPUT_SLASH;

        case SDL_SCANCODE_CAPSLOCK: return KEY_INPUT_CAPSLOCK;

        case SDL_SCANCODE_F1: return KEY_INPUT_F1;
        case SDL_SCANCODE_F2: return KEY_INPUT_F2;
        case SDL_SCANCODE_F3: return KEY_INPUT_F3;
        case SDL_SCANCODE_F4: return KEY_INPUT_F4;
        case SDL_SCANCODE_F5: return KEY_INPUT_F5;
        case SDL_SCANCODE_F6: return KEY_INPUT_F6;
        case SDL_SCANCODE_F7: return KEY_INPUT_F7;
        case SDL_SCANCODE_F8: return KEY_INPUT_F8;
        case SDL_SCANCODE_F9: return KEY_INPUT_F9;
        case SDL_SCANCODE_F10: return KEY_INPUT_F10;
        case SDL_SCANCODE_F11: return KEY_INPUT_F11;
        case SDL_SCANCODE_F12: return KEY_INPUT_F12;

        case SDL_SCANCODE_PRINTSCREEN: return KEY_INPUT_SYSRQ;
        case SDL_SCANCODE_SCROLLLOCK: return KEY_INPUT_SCROLL;
        case SDL_SCANCODE_PAUSE: return KEY_INPUT_PAUSE;
        case SDL_SCANCODE_INSERT: return KEY_INPUT_INSERT;
        case SDL_SCANCODE_HOME: return KEY_INPUT_HOME;
        case SDL_SCANCODE_PAGEUP: return KEY_INPUT_PGUP;
        case SDL_SCANCODE_DELETE: return KEY_INPUT_DELETE;
        case SDL_SCANCODE_END: return KEY_INPUT_END;
        case SDL_SCANCODE_PAGEDOWN: return KEY_INPUT_PGDN;
        case SDL_SCANCODE_RIGHT: return KEY_INPUT_RIGHT;
        case SDL_SCANCODE_LEFT: return KEY_INPUT_LEFT;
        case SDL_SCANCODE_DOWN: return KEY_INPUT_DOWN;
        case SDL_SCANCODE_UP: return KEY_INPUT_UP;

        case SDL_SCANCODE_NUMLOCKCLEAR: return KEY_INPUT_NUMLOCK;
        
        case SDL_SCANCODE_KP_DIVIDE: return KEY_INPUT_DIVIDE;
        case SDL_SCANCODE_KP_MULTIPLY: return KEY_INPUT_MULTIPLY;
        case SDL_SCANCODE_KP_MINUS: return KEY_INPUT_SUBTRACT;
        case SDL_SCANCODE_KP_PLUS: return KEY_INPUT_ADD;
        case SDL_SCANCODE_KP_ENTER: return KEY_INPUT_NUMPADENTER;
        case SDL_SCANCODE_KP_1: return KEY_INPUT_NUMPAD1;
        case SDL_SCANCODE_KP_2: return KEY_INPUT_NUMPAD2;
        case SDL_SCANCODE_KP_3: return KEY_INPUT_NUMPAD3;
        case SDL_SCANCODE_KP_4: return KEY_INPUT_NUMPAD4;
        case SDL_SCANCODE_KP_5: return KEY_INPUT_NUMPAD5;
        case SDL_SCANCODE_KP_6: return KEY_INPUT_NUMPAD6;
        case SDL_SCANCODE_KP_7: return KEY_INPUT_NUMPAD7;
        case SDL_SCANCODE_KP_8: return KEY_INPUT_NUMPAD8;
        case SDL_SCANCODE_KP_9: return KEY_INPUT_NUMPAD9;
        case SDL_SCANCODE_KP_0: return KEY_INPUT_NUMPAD0;
        case SDL_SCANCODE_KP_PERIOD: return KEY_INPUT_DECIMAL;
        case SDL_SCANCODE_KP_COMMA: return KEY_INPUT_NUMPADCOMMA;

        case SDL_SCANCODE_LCTRL: return KEY_INPUT_LCONTROL;
        case SDL_SCANCODE_LSHIFT: return KEY_INPUT_LSHIFT;
        case SDL_SCANCODE_LALT: return KEY_INPUT_LALT;
        case SDL_SCANCODE_LGUI: return KEY_INPUT_LWIN;
        case SDL_SCANCODE_RCTRL: return KEY_INPUT_RCONTROL;
        case SDL_SCANCODE_RSHIFT: return KEY_INPUT_RSHIFT;
        case SDL_SCANCODE_RALT: return KEY_INPUT_RALT;
        case SDL_SCANCODE_RGUI: return KEY_INPUT_RWIN;

        case SDL_SCANCODE_INTERNATIONAL3: return KEY_INPUT_YEN;
        case SDL_SCANCODE_KP_AT: return KEY_INPUT_AT; /* ? */
        case SDL_SCANCODE_KP_COLON: return KEY_INPUT_COLON; /* ? */
        /* case SDL_SCANCODE_????: return KEY_INPUT_PREVTRACK; */
        
        default: return 0;
    }
}

void PL_Input_ResetKeyBuffer() {
    s_keyBufferCount = 0;
    s_keyBufferNext = 0;
}
int PL_Input_GetFromKeyBuffer() {
    if (s_keyBufferCount >= s_keyBufferNext) {
        return 0;
    }
    return s_keyBufferTable[s_keyBufferCount++];
}

void PL_Input_HandleKeyDown(const SDL_Keysym *keysym) {
    int dxKey = s_MapSDLKeyToDXKey(keysym);
    
    if (dxKey <= 0 || dxKey > 255 || s_inputInitialized == DXFALSE) {
        return;
    }
    
    if (s_keyTable[dxKey] == 0) {
        s_keyTable[dxKey] = 1;
        s_downKeyCount += 1;
    }
    
    if (s_keyBufferNext < KEY_BUFFER_MAX) {
        s_keyBufferTable[s_keyBufferNext++] = dxKey;
    }
}

void PL_Input_HandleKeyUp(const SDL_Keysym *keysym) {
    int dxKey = s_MapSDLKeyToDXKey(keysym);
    
    if (dxKey <= 0 || dxKey > 255 || s_inputInitialized == DXFALSE) {
        return;
    }
    
    if (s_keyTable[dxKey] != 0) {
        s_keyTable[dxKey] = 0;
        s_downKeyCount -= 1;
    }
}

int PL_Input_CheckHitKey(int dxKey) {
    if (s_inputInitialized == DXFALSE) {
        return 0;
    }
    
    return s_keyTable[dxKey];
}

int PL_Input_CheckHitKeyAll(int checkType) {
    if ((checkType & DX_CHECKINPUT_KEY) != 0) {
        if (s_downKeyCount != 0) {
            return 1;
        }
    }
    if ((checkType & DX_CHECKINPUT_PAD) != 0) {
        int i;
        for (i = 0; i < s_controllerCount; ++i) {
            int jState = PL_Input_GetJoypadState(DX_INPUT_PAD1 + i);
            if (jState != 0) {
                return 1;
            }
        }
    }
    if ((checkType & DX_CHECKINPUT_MOUSE) != 0) {
        if (PL_Window_GetMouseInput() != 0) {
            return 1;
        }
    }
    
    return 0;
}

int PL_Input_GetHitKeyStateAll(char *keyStateBuf) {
    if (s_inputInitialized == DXFALSE) {
        return -1;
    }
    
    memcpy(keyStateBuf, s_keyTable, 256);
    
    return 0;
}

int PL_Input_GetJoypadNum() {
    if (s_inputInitialized == DXFALSE) {
        return 0;
    }
    
    return s_controllerCount;
}

int PL_Input_GetJoypadState(int inputIndex) {
    int controllerIndex = inputIndex &~ DX_INPUT_KEY;
    int joypadState = 0;
    
    if (s_inputInitialized == DXFALSE) {
        return 0;
    }
    
    if (controllerIndex >= DX_INPUT_PAD1 && controllerIndex <= DX_INPUT_PAD16) {
        int index = controllerIndex - DX_INPUT_PAD1;
        Controller *controller = s_GetController(index);
        if (controller != NULL) {
            if (controller->gamecontroller != NULL) {
                /* For game controllers (xbox, ps3, etc, whatever) */
                SDL_GameController *gc = controller->gamecontroller;
                int X = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
                int Y = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
                if (X < -16384) {
                    joypadState |= PAD_INPUT_LEFT;
                } else if (X > 16384) {
                    joypadState |= PAD_INPUT_RIGHT;
                }
                if (Y < -16384) {
                    joypadState |= PAD_INPUT_UP;
                } else if (Y > 16384) {
                    joypadState |= PAD_INPUT_DOWN;
                }

                /* DxLib treats the dpad as a POV hat, so it is not handled here.
                 * uncomment to use dpad input as normal input.
                 * if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_UP)) { joypadState |= PAD_INPUT_UP; }
                 * if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) { joypadState |= PAD_INPUT_LEFT; }
                 * if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) { joypadState |= PAD_INPUT_RIGHT; }
                 * if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) { joypadState |= PAD_INPUT_DOWN; }
                 */

                /* DxLib maps XInput controller buttons in this order, so we do the same. */
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A)) { joypadState |= PAD_INPUT_1; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B)) { joypadState |= PAD_INPUT_2; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_X)) { joypadState |= PAD_INPUT_3; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_Y)) { joypadState |= PAD_INPUT_4; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) { joypadState |= PAD_INPUT_5; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) { joypadState |= PAD_INPUT_6; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_BACK)) { joypadState |= PAD_INPUT_7; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_START)) { joypadState |= PAD_INPUT_8; }
                
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSTICK)) { joypadState |= PAD_INPUT_9; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSTICK)) { joypadState |= PAD_INPUT_10; }
                
                /* PL extension. */
                if (SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 16384) { joypadState |= PAD_INPUT_11; }
                if (SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16384) { joypadState |= PAD_INPUT_12; }
            } else if (controller->joystick != NULL) {
                SDL_Joystick *joystick = controller->joystick;
                int nButtons = SDL_JoystickNumButtons(joystick);
                int i;
                
                for (i = 0; i < nButtons; ++i) {
                    if (SDL_JoystickGetButton(joystick, i) != 0) {
                        joypadState |= 1 << (i + 4);
                    }
                }
                
                if (SDL_JoystickNumAxes(joystick) >= 2) {
                    int X = SDL_JoystickGetAxis(joystick, 0);
                    int Y = SDL_JoystickGetAxis(joystick, 1);
                    
                    if (X < -16384) {
                        joypadState |= PAD_INPUT_LEFT;
                    } else if (X > 16384) {
                        joypadState |= PAD_INPUT_RIGHT;
                    }
                    if (Y < -16384) {
                        joypadState |= PAD_INPUT_UP;
                    } else if (Y > 16384) {
                        joypadState |= PAD_INPUT_DOWN;
                    }
                }
                
                if (SDL_JoystickNumHats(joystick) >= 1) {
                    Uint8 hat = SDL_JoystickGetHat(joystick, 0);
                    if ((hat & SDL_HAT_LEFT) != 0) {
                        joypadState |= PAD_INPUT_LEFT;
                    } else if ((hat & SDL_HAT_RIGHT) != 0) {
                        joypadState |= PAD_INPUT_RIGHT;
                    }
                    if ((hat & SDL_HAT_UP) != 0) {
                        joypadState |= PAD_INPUT_UP;
                    } else if ((hat & SDL_HAT_DOWN) != 0) {
                        joypadState |= PAD_INPUT_DOWN;
                    }
                }
            }
        }
    }
    
    if ((inputIndex & DX_INPUT_KEY) != 0)  {
        /* Keyboard is not supported right now. */
        int index = controllerIndex - DX_INPUT_PAD1;
        if (index < 0) {
            index = 0;
        }
        
        if (index < DX_MAX_PADS) {
            KeybindTable *keybindTable = &s_keybinds[index];
            int i;
            for (i = 0; i < 32; ++i) {
                if (s_keyTable[keybindTable->keys[i].key1] != 0
                    || s_keyTable[keybindTable->keys[i].key2] != 0
                    || s_keyTable[keybindTable->keys[i].key3] != 0
                    || s_keyTable[keybindTable->keys[i].key4] != 0
                ) {
                    joypadState |= 1 << i;
                }
            }
        }
    }
    
    return joypadState;
}

int PL_Input_SetJoypadInputToKeyInput(int controllerIndex, int input,
                                      int key1, int key2, int key3, int key4) {
    int index = (controllerIndex &~ DX_INPUT_KEY) - 1;
    KeybindTable *keybindTable;
    int i;
    
    if (index < 0 || index >= DX_MAX_PADS) {
        return -1;
    }
    
    keybindTable = &s_keybinds[index];
    
    for (i = 0; i < 32; ++i) {
        if ((input & (1 << i)) != 0) {
            keybindTable->keys[i].key1 = (unsigned char)key1;
            keybindTable->keys[i].key2 = (unsigned char)key2;
            keybindTable->keys[i].key3 = (unsigned char)key3;
            keybindTable->keys[i].key4 = (unsigned char)key4;
        }
    }
    
    return 0;
}

int PL_Input_GetJoypadPOVState(int controllerIndex, int povNumber) {
    if (controllerIndex >= DX_INPUT_PAD1 && controllerIndex <= DX_INPUT_PAD16) {
        int index = controllerIndex - DX_INPUT_PAD1;
        Controller *controller = s_GetController(index);
        if (controller != NULL && controller->gamecontroller != NULL) {
            SDL_GameController *gc = controller->gamecontroller;
            /* For an XInput style controller, DPad becomes the only pov. */
            if (povNumber == 0) {
                int dpadState = 0;
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_UP)) { dpadState |= PAD_INPUT_UP; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) { dpadState |= PAD_INPUT_LEFT; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) { dpadState |= PAD_INPUT_RIGHT; }
                if (SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) { dpadState |= PAD_INPUT_DOWN; }
                switch(dpadState) {
                    case (PAD_INPUT_UP): return 0; break;
                    case (PAD_INPUT_UP|PAD_INPUT_RIGHT): return 4500; break;
                    case (PAD_INPUT_RIGHT): return 9000; break;
                    case (PAD_INPUT_DOWN|PAD_INPUT_RIGHT): return 13500; break;
                    case (PAD_INPUT_DOWN): return 18000; break;
                    case (PAD_INPUT_DOWN|PAD_INPUT_LEFT): return 22500; break;
                    case (PAD_INPUT_LEFT): return 27000; break;
                    case (PAD_INPUT_UP|PAD_INPUT_LEFT): return 31500; break;
                    default: return -1; break;
                }
            } else /* if (povNumber != 0) */ {
                return -1;
            }
        } else if (controller != NULL && controller->joystick != NULL) {
            SDL_Joystick *js = controller->joystick;
            switch(SDL_JoystickGetHat(js, povNumber)) {
                case SDL_HAT_UP: return 0; break;
                case SDL_HAT_RIGHTUP: return 4500; break;
                case SDL_HAT_RIGHT: return 9000; break;
                case SDL_HAT_RIGHTDOWN: return 13500; break;
                case SDL_HAT_DOWN: return 18000; break;
                case SDL_HAT_LEFTDOWN: return 22500; break;
                case SDL_HAT_LEFT: return 27000; break;
                case SDL_HAT_LEFTUP: return 31500; break;
                default: return -1; break;
            }
        }
    }
    
    return -1;
}

int PL_Input_GetJoypadDirectInputState(int controllerIndex, DINPUT_JOYSTATE *state) {
    if (controllerIndex >= DX_INPUT_PAD1 && controllerIndex <= DX_INPUT_PAD16) {
        int index = controllerIndex - DX_INPUT_PAD1;
        Controller *controller = s_GetController(index);
        if (controller != NULL && controller->gamecontroller != NULL) {
            SDL_GameController *gc = controller->gamecontroller;
            int axis;
            
            SDL_memset(state, 0, sizeof(DINPUT_JOYSTATE));
            
            state->X = (int)SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX) * DX_INPUT_RANGE / 32767;
            state->Y = (int)SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY) * DX_INPUT_RANGE / 32767;
            state->Rx = (int)SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTX) * DX_INPUT_RANGE / 32767;
            state->Ry = (int)SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTY) * DX_INPUT_RANGE / 32767;
            
            state->Buttons[0] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A) ? 0x80 : 0x00;
            state->Buttons[1] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B) ? 0x80 : 0x00;
            state->Buttons[2] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_X) ? 0x80 : 0x00;
            state->Buttons[3] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_Y) ? 0x80 : 0x00;
            state->Buttons[4] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) ? 0x80 : 0x00;
            state->Buttons[5] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) ? 0x80 : 0x00;
            state->Buttons[6] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_BACK) ? 0x80 : 0x00;
            state->Buttons[7] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_START) ? 0x80 : 0x00;
            state->Buttons[8] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSTICK) ? 0x80 : 0x00;
            state->Buttons[9] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSTICK) ? 0x80 : 0x00;
            
            /* Triggers are treated as a single axis in DirectInput mode. */
            if ((axis = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT)) > 16384) {
                state->Z = ((axis - 16384) * DX_INPUT_RANGE) / 16383;
                state->Buttons[10] = 0x80; /* PL extension. */
            } else if ((axis = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)) > 16384) {
                state->Z = ((axis - 16384) * DX_INPUT_RANGE) / -16383;
                state->Buttons[11] = 0x80; /* PL extension. */
            } else {
                state->Z = 0;
            }
            
            state->POV[0] = PL_Input_GetJoypadPOVState(controllerIndex, 0);
            state->POV[1] = -1;
            state->POV[2] = -1;
            state->POV[3] = -1;
            
            return 0;
        } else if (controller != NULL && controller->joystick != NULL) {
            SDL_Joystick *js = controller->joystick;
            int n;
            int i;
            
            SDL_memset(state, 0, sizeof(DINPUT_JOYSTATE));
            
            /* SDL's axis data has no direct mapping to DirectInput,
             * even though it converts from DirectInput in the first place.
             * So it's best that we just don't try to guess, in the end. */
            n = SDL_JoystickNumAxes(js);
            if (n >= 0) { state->X = (int)SDL_JoystickGetAxis(js, 0) * DX_INPUT_RANGE / 32767; }
            if (n >= 1) { state->Y = (int)SDL_JoystickGetAxis(js, 1) * DX_INPUT_RANGE / 32767; }
            
            for (i = 0; i < 4; ++i) {
                state->POV[i] = PL_Input_GetJoypadPOVState(controllerIndex, i);
            }
            
            n = SDL_JoystickNumButtons(js);
            if (n > 32) { n = 32; }
            
            for (i = 0; i < n; ++i) {
                state->Buttons[n] = SDL_JoystickGetButton(js, i);
            }
            
            return 0;
        }
    }
    
    return -1;
}
int PL_Input_GetJoypadXInputState(int controllerIndex, XINPUT_STATE *state) {
    if (controllerIndex >= DX_INPUT_PAD1 && controllerIndex <= DX_INPUT_PAD16) {
        int index = controllerIndex - DX_INPUT_PAD1;
        Controller *controller = s_GetController(index);
        if (controller != NULL && controller->gamecontroller != NULL) {
            SDL_GameController *gc = controller->gamecontroller;
            
            state->Buttons[XINPUT_BUTTON_DPAD_UP] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_UP);
            state->Buttons[XINPUT_BUTTON_DPAD_DOWN] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            state->Buttons[XINPUT_BUTTON_DPAD_LEFT] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            state->Buttons[XINPUT_BUTTON_DPAD_RIGHT] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
            
            state->Buttons[XINPUT_BUTTON_START] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_START);
            state->Buttons[XINPUT_BUTTON_BACK] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_BACK);
            
            state->Buttons[XINPUT_BUTTON_LEFT_THUMB] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSTICK);
            state->Buttons[XINPUT_BUTTON_RIGHT_THUMB] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
            state->Buttons[XINPUT_BUTTON_LEFT_SHOULDER] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            state->Buttons[XINPUT_BUTTON_RIGHT_SHOULDER] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
            state->Buttons[XINPUT_BUTTON_A] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_A);
            state->Buttons[XINPUT_BUTTON_B] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_B);
            state->Buttons[XINPUT_BUTTON_X] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_X);
            state->Buttons[XINPUT_BUTTON_Y] = SDL_GameControllerGetButton(gc, SDL_CONTROLLER_BUTTON_Y);
            
            state->LeftTrigger = (unsigned char)((SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT) + 32768) / 256);
            state->RightTrigger = (unsigned char)((SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) + 32768) / 256);
            
            state->ThumbLX = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
            state->ThumbLY = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
            state->ThumbRX = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTX);
            state->ThumbRY = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTY);
            
            return 0;
        }
    }
    
    return -1;
}

void PL_Input_Init() {
    s_InitializeInput();
}

void PL_Input_End() {
    if (s_inputInitialized == DXFALSE) {
        return;
    }
    
    s_CloseControllers();
    
    s_inputInitialized = DXFALSE;
}

void PL_Input_AddController(int controllerIndex) {
    if (s_inputInitialized == DXFALSE) {
        return;
    }
    
    s_AddController(controllerIndex);
}

void PL_Input_RemoveController(int instanceID) {
    if (s_inputInitialized == DXFALSE) {
        return;
    }
    
    s_RemoveController(instanceID);
}

int PL_Input_GetMouseWheelRotVol(int clearFlag) {
    int mouseY = s_mouseWheelY;
    if (s_inputInitialized == DXFALSE) {
        return 0;
    }
    
    if (clearFlag) {
        s_mouseWheelY = 0;
    }
    return mouseY;
}
int PL_Input_GetMouseHWheelRotVol(int clearFlag) {
    int mouseX = s_mouseWheelX;
    if (s_inputInitialized == DXFALSE) {
        return 0;
    }
    
    if (clearFlag) {
        s_mouseWheelX = 0;
    }
    return mouseX;
}

void PL_Input_HandleWheelMotion(int x, int y) {
    if (s_inputInitialized == DXFALSE) {
        return;
    }
    
    s_mouseWheelX += x;
    s_mouseWheelY += y;
}

#endif /* #ifndef DX_NON_INPUT */
