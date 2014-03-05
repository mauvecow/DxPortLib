/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013 Patrick McCarthy <mauve@sandwich.net>
  
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

#include "DxInternal.h"

#include "SDL.h"

#include "SDL_image.h"

static int s_initialized = 0;
int PL_windowWidth = 640;
int PL_windowHeight = 480;
static int s_windowRefreshRate = 60;
static int s_windowDepth = 32;
static SDL_Window *s_window = NULL;
static SDL_Rect s_targetRect = { 0, 0, 640, 480 };
static int s_windowRealWidth = 640;
static int s_windowRealHeight = 480;
static SDL_Surface *s_windowIcon = NULL;
static int s_windowVSync = DXTRUE;
static int s_alwaysRunFlag = DXFALSE;

static Uint32 s_windowFlags =
        SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE;

static char *s_windowTitle = NULL;

static int s_mouseVisible = DXTRUE;

int PL_Window_ResetSettings() {
    if (s_initialized == DXTRUE) {
        return -1;
    }
    
    PL_windowWidth = 640;
    PL_windowHeight = 480;
    s_windowRefreshRate = 60;
    s_windowDepth = 32;
    s_windowVSync = DXTRUE;
    s_alwaysRunFlag = DXFALSE;
    s_windowFlags = SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE;
    s_mouseVisible = DXTRUE;
    if (s_windowTitle != NULL) {
        DXFREE(s_windowTitle);
    }
    s_windowTitle = NULL;
    
    return 0;
}

static void PL_Window_HandleResize(int realWidth, int realHeight) {
    /* Update target rectangle. */
    SDL_Rect fullRect;
    SDL_Rect destRect;
    int aspectCorrectWidth;
    
    fullRect.x = 0;
    fullRect.y = 0;
    fullRect.w = realWidth;
    fullRect.h = realHeight;
    
    destRect = fullRect;
    
    aspectCorrectWidth = realHeight * PL_windowWidth / PL_windowHeight;
    if (aspectCorrectWidth < realWidth) {
        int left = (realWidth - aspectCorrectWidth) / 2;
        destRect.x = left;
        destRect.w = aspectCorrectWidth;
    } else {
        int aspectCorrectHeight = realWidth * PL_windowHeight / PL_windowWidth;
        if (aspectCorrectHeight < realHeight) {
            int top = (realHeight - aspectCorrectHeight) / 2;
            destRect.y = top;
            destRect.h = aspectCorrectHeight;
        }
    }
    
    s_targetRect = destRect;
    
    s_windowRealWidth = realWidth;
    s_windowRealHeight = realHeight;
}

static void PL_Window_Refresh() {
    int wWidth, wHeight;
    SDL_GetWindowSize(s_window, &wWidth, &wHeight);
    if (wWidth != s_windowRealWidth || wHeight != s_windowRealHeight) {
        PL_Window_HandleResize(wWidth, wHeight);
    }
    
    PL_Draw_Refresh(s_window, &s_targetRect);
}

int PL_Window_Init(void) {
    const char *windowTitle = s_windowTitle;
    if (windowTitle == NULL) {
        windowTitle = "DxPortLib App";
    }
    s_windowFlags |= 
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;
    
    s_window = SDL_CreateWindow(
        windowTitle,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        PL_windowWidth, PL_windowHeight,
        s_windowFlags
        );
    
    if (s_windowIcon != NULL) {
        SDL_SetWindowIcon(s_window, s_windowIcon);
    }
    
    SDL_ShowWindow(s_window);
    
    SDL_DisableScreenSaver();
    
    PL_Draw_Init(s_window, PL_windowWidth, PL_windowHeight, s_windowVSync);
    
    s_windowRealWidth = 0;
    s_windowRealHeight = 0;
    
    s_initialized = DXTRUE;
    
    SDL_ShowCursor(s_mouseVisible);
    PL_Window_Refresh();
    
    return 0;
}

int PL_Window_End(void) {
    if (s_initialized == DXFALSE) {
        return -1;
    }
    
    s_initialized = DXFALSE;
    
    PL_Draw_End();
    
    SDL_EnableScreenSaver();
    
    SDL_DestroyWindow(s_window);
    s_window = NULL;
    
    return 0;
}

int PL_Window_SwapBuffers() {
    if (s_initialized == DXTRUE) {
        PL_Draw_SwapBuffers(s_window, &s_targetRect);
    }
    return 0;
}

int PL_Window_SetFullscreen(int isFullscreen) {
    Uint32 newFlags = s_windowFlags;
    newFlags &= (Uint32)~SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (isFullscreen != 0) {
        newFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    
    if (newFlags != s_windowFlags) {
        s_windowFlags = newFlags;
        if (s_initialized == DXTRUE) {
            SDL_SetWindowFullscreen(s_window, newFlags);
        }
    }
    
    return 0;
}

int PL_Window_SetDimensions(int width, int height, int colorDepth, int refreshRate) {
    if (colorDepth == 32) {
        s_windowDepth = 32;
    } else {
        s_windowDepth = 16;
    }
    
    s_windowRefreshRate = refreshRate;
    
    if (width != PL_windowWidth || height != PL_windowHeight) {
        PL_windowWidth = width;
        PL_windowHeight = height;
        if (s_initialized == DXTRUE) {
            /* Remember that we are changing the backbuffer size here, not the
             * window size. (unless it's smaller?) */
            SDL_SetWindowSize(s_window, width, height);
            
            PL_Draw_ResizeWindow(width, height);
        }
    }
    
    return 0;
}

int PL_Window_SetTitle(const DXCHAR *titleString) {
    if (s_windowTitle != NULL) {
        DXFREE(s_windowTitle);
        s_windowTitle = NULL;
    }
    
    if (titleString != NULL) {
        char utf8Buf[2048];
        
        if (PL_Text_DxStringToString(titleString, utf8Buf, 2048, DX_CHARSET_EXT_UTF8) > 0) {
            s_windowTitle = SDL_strdup(utf8Buf);
            
            if (s_initialized == DXTRUE) {
                SDL_SetWindowTitle(s_window, s_windowTitle);
            }
        }
    }
    return 0;
}

static int s_lacksFocus = 0;

int PL_Window_ProcessMessages() {
    SDL_Event event;
    
    if (s_initialized == DXFALSE) {
        return -1;
    }
    
    do {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_WINDOWEVENT:
                    switch(event.window.event) {
                        case SDL_WINDOWEVENT_ENTER:
                            SDL_DisableScreenSaver();
                            s_lacksFocus &= ~1;
                            break;
                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            SDL_DisableScreenSaver();
                            s_lacksFocus &= ~2;
                            break;
                        case SDL_WINDOWEVENT_LEAVE:
                            SDL_EnableScreenSaver();
                            s_lacksFocus |= 1;
                            break;
                        case SDL_WINDOWEVENT_FOCUS_LOST:
                            SDL_EnableScreenSaver();
                            s_lacksFocus |= 2;
                            break;
                        case SDL_WINDOWEVENT_EXPOSED:
                        case SDL_WINDOWEVENT_RESTORED:
                            PL_Window_Refresh();
                            break;
                        case SDL_WINDOWEVENT_RESIZED:
                            PL_Window_HandleResize(event.window.data1, event.window.data2);
                            break;
                        case SDL_WINDOWEVENT_CLOSE:
                            PL_Window_End();
                            return -1;
                        default:
                            break;
                    }
                    break;
#ifndef DX_NON_INPUT
                case SDL_KEYDOWN:
                    PL_Input_HandleKeyDown(&event.key.keysym);
                    break;
                case SDL_KEYUP:
                    PL_Input_HandleKeyUp(&event.key.keysym);
                    break;
                case SDL_JOYDEVICEADDED:
                    PL_Input_AddController(event.jdevice.which);
                    break;
                case SDL_JOYDEVICEREMOVED:
                    PL_Input_RemoveController(event.jdevice.which);
                    break;
                case SDL_MOUSEWHEEL:
                    PL_Input_HandleWheelMotion(event.wheel.x, event.wheel.y);
                    break;
#endif /* #ifndef DX_NON_INPUT */
                case SDL_QUIT:
                    return -1;
            }
        }
        
        if (s_lacksFocus) {
            if (s_alwaysRunFlag) {
                s_lacksFocus = 0;
            } else {
                SDL_Delay(1);
            }
        }
    } while (s_lacksFocus);
    
    return 0;
}

int PL_Window_GetActiveFlag() {
    return s_lacksFocus == 0 ? DXTRUE : DXFALSE;
}
int PL_Window_GetWindowModeFlag() {
    if ((s_windowFlags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0) {
        return DXFALSE;
    }
    return DXTRUE;
}

int PL_Window_SetWindowResizeFlag(int flag) {
    if (flag == DXFALSE) {
        s_windowFlags &= (Uint32)~SDL_WINDOW_RESIZABLE;
    } else {
        s_windowFlags |= SDL_WINDOW_RESIZABLE;
    }
    
    /* SDL does not support updating this on demand. */
    
    return 0;
}

int PL_Window_SetMouseDispFlag(int flag) {
    s_mouseVisible = (flag == DXFALSE) ? DXFALSE : DXTRUE;
    
    if (s_initialized == DXTRUE) {
        SDL_ShowCursor(flag);
    }
    
    return 0;
}
int PL_Window_GetMouseDispFlag() {
    return s_mouseVisible;
}

int PL_Window_GetMousePoint(int *xPosition, int *yPosition) {
    int x, y;
    
    if (s_initialized == DXFALSE) {
        return -1;
    }
    
    if (s_targetRect.w == 0 || s_targetRect.h == 0) {
        *xPosition = 0;
        *yPosition = 0;
    } else {
        SDL_GetMouseState(&x, &y);
        
        *xPosition = ((x - s_targetRect.x) * PL_windowWidth) / s_targetRect.w;
        *yPosition = ((y - s_targetRect.y) * PL_windowHeight) / s_targetRect.h;
    }
    
    return 0;
}

int PL_Window_SetMousePoint(int xPosition, int yPosition) {
    if (s_initialized == DXFALSE) {
        return -1;
    }
    
    if (PL_windowWidth != 0) {
        xPosition = ((xPosition * s_targetRect.w) / PL_windowWidth) + s_targetRect.x;
    } else {
        xPosition = 0;
    }
    if (PL_windowHeight != 0) {
        yPosition = ((yPosition * s_targetRect.h) / PL_windowHeight) + s_targetRect.y;
    } else {
        yPosition = 0;
    }
    
    SDL_WarpMouseInWindow(s_window, xPosition, yPosition);
    
    return 0;
}

int PL_Window_GetMouseInput() {
    Uint32 buttons;
    int retval;
    
    if (s_initialized == DXFALSE) {
        return 0;
    }
    
    buttons = SDL_GetMouseState(NULL, NULL);
    
    /* SDL's button mappings line up identically, so here we go */
    retval = (int)buttons;
    
    return retval;
}

int PLEXT_Window_SetIconImageFile(const DXCHAR *filename) {
    SDL_Surface *surface;
    SDL_RWops *file;
    
    file = PL_File_OpenStream(filename);
    if (file == NULL) {
        return -1;
    }

    surface = IMG_Load_RW(file, SDL_TRUE);
    if (surface == NULL) {
        return -1;
    }
    
    if (s_windowIcon != NULL) {
        SDL_FreeSurface(s_windowIcon);
    }
    s_windowIcon = surface;
    
    if (s_initialized == DXTRUE) {
        SDL_SetWindowIcon(s_window, s_windowIcon);
    }
    
    return 0;
}

int PL_Window_SetWaitVSyncFlag(int flag) {
    s_windowVSync = (flag == DXFALSE) ? DXFALSE : DXTRUE;
    return 0;
}

int PL_Window_GetWaitVSyncFlag() {
    return s_windowVSync;
}

int PL_Window_SetAlwaysRunFlag(int flag) {
    s_alwaysRunFlag = (flag == DXFALSE) ? DXFALSE : DXTRUE;
    return 0;
}

int PL_Window_GetAlwaysRunFlag() {
    return s_alwaysRunFlag;
}

/* System agnostic message box stuff */
int PLEXT_Window_MessageBoxError(
            const DXCHAR *title,
            const DXCHAR *text
) {
    char titleBuf[1024];
    char textBuf[1024];
    
    PL_Text_DxStringToString(title, titleBuf, 1024, DX_CHARSET_EXT_UTF8);
    PL_Text_DxStringToString(text, textBuf, 1024, DX_CHARSET_EXT_UTF8);
    
    return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, s_window);
}

int PLEXT_Window_MessageBoxYesNo(
            const DXCHAR *title,
            const DXCHAR *text,
            const DXCHAR *yes,
            const DXCHAR *no
) {
    SDL_MessageBoxButtonData buttons[2];
    SDL_MessageBoxData data;
    int resultButton;
    char titleBuf[1024];
    char textBuf[1024];
    char yesBuf[512];
    char noBuf[512];
    
    PL_Text_DxStringToString(title, titleBuf, 1024, DX_CHARSET_EXT_UTF8);
    PL_Text_DxStringToString(text, textBuf, 1024, DX_CHARSET_EXT_UTF8);
    PL_Text_DxStringToString(yes, yesBuf, 1024, DX_CHARSET_EXT_UTF8);
    PL_Text_DxStringToString(no, noBuf, 1024, DX_CHARSET_EXT_UTF8);
    
    buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
    buttons[0].buttonid = 1;
    buttons[0].text = yesBuf;
    buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    buttons[1].buttonid = 0;
    buttons[1].text = noBuf;
    
    data.flags = SDL_MESSAGEBOX_INFORMATION;
    data.window = s_window;
    data.title = title;
    data.message = text;
    data.numbuttons = 2;
    data.buttons = buttons;
    data.colorScheme = NULL;
    
    if (SDL_ShowMessageBox(&data, &resultButton) < 0) {
        return -1;
    }
    
    return resultButton;
}
