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

#include "PLSDL2Internal.h"

#ifdef DXPORTLIB_PLATFORM_SDL2

#include "SDL.h"

#include <time.h>

int PL_Platform_GetTicks() {
    return SDL_GetTicks();
}

void PL_Platform_Wait(int ticks) {
    SDL_Delay(ticks);
}

int PL_Platform_GetDateTime(DATEDATA *dateBuf) {
    /* SDL does not have a function for getting system date and time,
     * so we use time/localtime for this. */
    time_t t;
    struct tm *format;
    
    time(&t);
    format = localtime(&t);
    
    dateBuf->Year = format->tm_year + 1900;
    dateBuf->Mon = format->tm_mon + 1;
    dateBuf->Day = format->tm_mday;
    dateBuf->Hour = format->tm_hour;
    dateBuf->Min = format->tm_min;
    dateBuf->Sec = format->tm_sec;
    
    return 0;
}

int PL_Platform_GetSaveFolder(char *buffer, int bufferLength,
                              const char *org, const char *app,
                              int destEncoding) {
    char *prefPath;
    
    if (bufferLength == 0) {
        return -1;
    }

    prefPath = SDL_GetPrefPath(org, app);
    if (prefPath == NULL) {
        buffer[0] = '\0';
        return 0;
    }
    
    PL_Text_ConvertStrncpy(buffer, destEncoding,
                           prefPath, DX_CHARSET_EXT_UTF8,
                           bufferLength);

    SDL_free(prefPath);
    return 0;
}

/* System agnostic message box stuff */
int PL_Platform_MessageBoxError(
        const char *title,
        const char *text
) {
    return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, PL_window);
}

int PL_Platform_MessageBoxYesNo(
        const char *title,
        const char *text,
        const char *yes,
        const char *no
) {
    SDL_MessageBoxButtonData buttons[2];
    SDL_MessageBoxData data;
    int resultButton;
    
    buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
    buttons[0].buttonid = 1;
    buttons[0].text = yes;
    buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    buttons[1].buttonid = 0;
    buttons[1].text = no;
    
    data.flags = SDL_MESSAGEBOX_INFORMATION;
    data.window = PL_window;
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

#endif
