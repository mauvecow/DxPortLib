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

/* Unlike the rest of the library, this is intended as a replacement for the
 * Luna library used by many doujin games.
 * 
 * As the two libraries have much in common, this is simply a wrapper to
 * DxLib's functionality.
 */

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "Luna.h"

#include "PLInternal.h"
#include "LunaInternal.h"

#include <time.h>

static int s_initialized = DXFALSE;
static int s_forcequit = DXFALSE;

static int s_frameRate = 0;
static int s_timerLastTicks = 0;
static int s_drawTitleInfo = 0;
static int s_lunaUseFlags = 0;

static int s_virtualWidth = 640;
static int s_virtualHeight = 640;
static float s_lunaBaseVirtualXmult = 1.0f;
static float s_lunaBaseVirtualYmult = 1.0f;
float g_lunaVirtualXmult = 1.0f;
float g_lunaVirtualYmult = 1.0f;

static LunaInitFunc s_LunaInit = NULL;
static LunaMainFunc s_LunaMain = NULL;
static LunaMessageProcFunc s_LunaMessageProc = NULL;

int Luna::BootMain(Sint32 argc, char **argv,
                   LunaInitFunc LunaInit, LunaMainFunc LunaMain,
                   LunaMessageProcFunc LunaMessageProc
                  ) {
    s_LunaInit = LunaInit;
    s_LunaMain = LunaMain;
    s_LunaMessageProc = LunaMessageProc;
    
    PL_Platform_Boot();
    PL_Platform_Init();
    
    PL_Handle_Init();
    PL_File_Init();
    LunaFile_Init();
    LunaMath_Init();
    
    LunaRand::Seed((unsigned int)::time(NULL) * PL_Platform_GetTicks());
    
    s_LunaInit();
    
    s_LunaMain(argc, argv);
    
    Luna::End();
    LunaFile_End();
    PL_File_End();
    PL_Handle_End();
    
    PL_Platform_Finish();
    
    return 0;
}

Bool Luna::Start() {
    if (s_initialized) {
        return false;
    }
    
#ifndef DX_NON_INPUT
    if ((s_lunaUseFlags & OPTION_DIRECTINPUT) != 0) {
        PL_Input_Init();
    }
#endif  /* #ifndef DX_NON_INPUT */
    PL_Window_Init();
#ifndef DX_NON_SOUND
    if ((s_lunaUseFlags & OPTION_DIRECTSOUND) != 0) {
        PL_Audio_Init();
    }
#endif /* #ifndef DX_NON_SOUND */
#ifndef DX_NON_FONT
    PL_Font_Init();
#endif /* #ifndef DX_NON_FONT */
    
    Luna3D::SetViewport(NULL);
    
    s_initialized = DXTRUE;
    
    return true;
}
void Luna::End() {
    if (!s_initialized) {
        return;
    }
    
    s_initialized = DXFALSE;
    
#ifndef DX_NON_FONT
    PL_Font_End();
#endif /* #ifndef DX_NON_FONT */
#ifndef DX_NON_SOUND
    if ((s_lunaUseFlags & OPTION_DIRECTSOUND) != 0) {
        PL_Audio_End();
    }
#endif /* #ifndef DX_NON_SOUND */
    PL_Window_End();
#ifndef DX_NON_INPUT
    if ((s_lunaUseFlags & OPTION_DIRECTINPUT) != 0) {
        PL_Input_End();
    }
#endif /* #ifndef DX_NON_INPUT */
}

void Luna::Exit() {
    s_forcequit = DXTRUE;
}

static int s_handleAltEnter() {
    static int returnState = 0;
    if (PL_Input_CheckHitKey(DIK_RETURN)) {
        if (returnState == 0) {
            if (PL_Input_CheckHitKey(DIK_LALT) || PL_Input_CheckHitKey(DIK_RALT)) {
                Luna::ChangeScreenMode();
            }
            returnState = 1;
        }
    } else {
        returnState = 0;
    }
    
    return 0;
}

Bool Luna::WaitForMsgLoop(Bool isFullActive) {
    if (s_forcequit) {
        return false;
    }
    
    int alwaysRunFlag = (isFullActive == true) ? DXTRUE : DXFALSE;
    PL_Window_SetAlwaysRunFlag(alwaysRunFlag);
    
    if (PL_Window_ProcessMessages() < 0) {
        return false;
    }
    
    Luna::SyncFrame();
    
    LunaInput_Refresh();
    
    s_handleAltEnter();
    
    return true;
}

void Luna::SyncFrame() {
    Sint32 ticks = (Sint32)PL_Platform_GetTicks();
    Sint32 diff = ticks - s_timerLastTicks;
    
    if (s_frameRate > 0) {
        Sint32 waitTime = 1000 / s_frameRate;
        
        while (waitTime > diff) {
            Sint32 amount = waitTime - diff;
            PL_Platform_Wait((amount > 1) ? (amount - 1) : 1);
            
            ticks = (Sint32)PL_Platform_GetTicks();
            diff = ticks - s_timerLastTicks;
        }
    }
    
    s_timerLastTicks = (Sint32)PL_Platform_GetTicks();
    
    // WRITEME: Update FPS
}

void Luna::SetApplicationName(const DXCHAR *name) {
    PL_Window_SetTitle(name);
}

void Luna::SetVirtualScreenSize(Sint32 rWidth, Sint32 rHeight,
                                Sint32 vWidth, Sint32 vHeight) {
    s_virtualWidth = vWidth;
    s_virtualHeight = vHeight;
    s_lunaBaseVirtualXmult = toF(rWidth) / toF(vWidth);
    s_lunaBaseVirtualYmult = toF(rHeight) / toF(vHeight);
    g_lunaVirtualXmult = s_lunaBaseVirtualXmult;
    g_lunaVirtualYmult = s_lunaBaseVirtualYmult;
}
void Luna::SetVirtualScreenEnable(Bool Flag) {
    if (Flag) {
        g_lunaVirtualXmult = s_lunaBaseVirtualXmult;
        g_lunaVirtualYmult = s_lunaBaseVirtualYmult;
    } else {
        g_lunaVirtualXmult = 1.0f;
        g_lunaVirtualYmult = 1.0f;
    }
}
void Luna::GetVirtualScreenSize(Sint32 *pWidth, Sint32 *pHeight) {
    *pWidth = s_virtualWidth;
    *pHeight = s_virtualHeight;
}
Float Luna::Virtual2RealX(Float Pos) {
    return (Pos * g_lunaVirtualXmult);
}
Float Luna::Virtual2RealY(Float Pos) {
    return (Pos * g_lunaVirtualYmult);
}

void Luna::SetScreenInfo(Sint32 width, Sint32 height, Bool isWindow) {
    PL_Window_SetDimensions(width, height, 32, 60);
    PL_Window_SetFullscreen((isWindow == true) ? DXFALSE : DXTRUE);
    
    SetVirtualScreenSize(width, height, width, height);
}
void Luna::ChangeScreenSize(Sint32 width, Sint32 height, Bool IsChange) {
    PL_Window_SetDimensions(width, height, 32, 60);
    
    s_lunaBaseVirtualXmult = toF(width) / toF(s_virtualWidth);
    s_lunaBaseVirtualYmult = toF(height) / toF(s_virtualHeight);
    g_lunaVirtualXmult = s_lunaBaseVirtualXmult;
    g_lunaVirtualYmult = s_lunaBaseVirtualYmult;
    
    if (IsChange == true) {
        if (PL_Window_GetWindowModeFlag() == DXTRUE) {
            PL_Window_SetFullscreen(DXTRUE);
        } else {
            PL_Window_SetFullscreen(DXFALSE);
        }
    }
}
void Luna::ChangeScreenMode() {
    ChangeScreenSize(PL_windowWidth, PL_windowHeight, DXTRUE);
}

void Luna::SetFrameRate(Sint32 frameRate) {
    s_frameRate = frameRate;
}
void Luna::SetDrawTitleInfo(void) {
    s_drawTitleInfo = DXTRUE;
}
void Luna::SetDrawMouse(void) {
    PL_Window_SetMouseDispFlag(DXTRUE);
}
void Luna::SetUseOption(Uint32 flags) {
    s_lunaUseFlags = flags;
}

void Luna::EXTSetUseCharset(int dxCharset) {
    PL_Text_SetUseCharSet(dxCharset);
}
void Luna::GetScreenData(Sint32 *width, Sint32 *height, bool *isWindowed) {
    if (width != NULL) {
        *width = PL_windowWidth;
    }
    if (height != NULL) {
        *height = PL_windowHeight;
    }
    if (isWindowed != NULL) {
        *isWindowed = PL_Window_GetWindowModeFlag() ? true : false;
    }
}

void Luna::EXTGetSaveFolder(DXCHAR *buffer, int bufferLength,
                            const DXCHAR *org, const DXCHAR *app) {
    PL_Platform_GetSaveFolder(buffer, bufferLength, org, app);
}

int Luna::EXTConvertText(DXCHAR *buffer, int bufferLength,
                          const DXCHAR *string,
                          int destEncoding, int srcEncoding) {
    unsigned int ch;
    int pos = 0;
    
    while ((ch = PL_Text_ReadChar(&string, srcEncoding)) != 0) {
        pos += PL_Text_WriteChar(buffer + pos, ch, bufferLength - pos, destEncoding);
    }
    
    buffer[pos] = '\0';
    
    return pos;
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
