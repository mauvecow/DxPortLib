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

static int64_t SDLCALL PLSDL2_RWops_GetSize(void *userdata) {
    SDL_RWops *rwops = (SDL_RWops *)userdata;
    return SDL_RWsize(rwops);
}
static int64_t SDLCALL PLSDL2_RWops_Tell(void *userdata) {
    SDL_RWops *rwops = (SDL_RWops *)userdata;
    return SDL_RWtell(rwops);
}
static int SDLCALL PLSDL2_RWops_Seek(void *userdata, int64_t position, int origin) {
    SDL_RWops *rwops = (SDL_RWops *)userdata;
    switch(origin) {
        case 0: SDL_RWseek(rwops, position, RW_SEEK_SET); break;
        case 1: SDL_RWseek(rwops, position, RW_SEEK_CUR); break;
        case 2: SDL_RWseek(rwops, position, RW_SEEK_END); break;
    }
    return 0;
}
static int SDLCALL PLSDL2_RWops_Read(void *userdata, void *data, int size) {
    SDL_RWops *rwops = (SDL_RWops *)userdata;
    return SDL_RWread(rwops, data, (size_t)size, 1) * size;
}
static int SDLCALL PLSDL2_RWops_Close(void *userdata) {
    SDL_RWops *rwops = (SDL_RWops *)userdata;
    SDL_RWclose(rwops);
    return 0;
}

static const PL_FileFunctions PLSDL2_FileFunctions = {
    PLSDL2_RWops_GetSize,
    PLSDL2_RWops_Tell,
    PLSDL2_RWops_Seek,
    PLSDL2_RWops_Read,
    PLSDL2_RWops_Close
};

typedef struct _NestedRWops {
    SDL_RWops rwops;
    
    int fileHandle;
} NestedRWops;

static Sint64 SDLCALL PLSDL2_NestedFile_Size(SDL_RWops *context) {
    NestedRWops *nested = (NestedRWops *)context;
    return PL_File_GetSize(nested->fileHandle);
}

static Sint64 SDLCALL PLSDL2_NestedFile_Seek(SDL_RWops *context, Sint64 offset, int whence) {
    NestedRWops *nested = (NestedRWops *)context;
    switch(whence) {
        case RW_SEEK_SET: PL_File_Seek(nested->fileHandle, offset, 0); break;
        case RW_SEEK_CUR: PL_File_Seek(nested->fileHandle, offset, 1); break;
        case RW_SEEK_END: PL_File_Seek(nested->fileHandle, offset, 2); break;
    }
    return PL_File_Tell(nested->fileHandle);
}

static size_t SDLCALL PLSDL2_NestedFile_Read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum) {
    NestedRWops *nested = (NestedRWops *)context;
    return (int)PL_File_Read(nested->fileHandle, ptr, size * maxnum) / size;
} 

static size_t SDLCALL PLSDL2_NestedFile_DisableWrite(SDL_RWops *context, const void *ptr, size_t size, size_t num) {
    return 0; /* writing is not supported. */
}

static int SDLCALL PLSDL2_NestedFile_Close(SDL_RWops *context) {
    if (context != NULL) {
        NestedRWops *nested = (NestedRWops *)context;
        PL_File_Close(nested->fileHandle);
        
        DXFREE(nested);
    }
    return 0;
}

SDL_RWops *PLSDL2_FileToRWops(int fileHandle) {
    /* It is entirely possible to have RWOPs -> file handle -> RWOPS
     * going on here, and just nest this indefinitely. Oh well? */
    NestedRWops *nested;
    
    if (fileHandle < 0) {
        return NULL;
    }
    
    nested = (NestedRWops *)DXALLOC(sizeof(NestedRWops));
    
    nested->rwops.size = PLSDL2_NestedFile_Size;
    nested->rwops.seek = PLSDL2_NestedFile_Seek;
    nested->rwops.read = PLSDL2_NestedFile_Read;
    nested->rwops.write = PLSDL2_NestedFile_DisableWrite;
    nested->rwops.close = PLSDL2_NestedFile_Close;
    
    nested->rwops.type = SDL_RWOPS_UNKNOWN;
    
    nested->fileHandle = fileHandle;
    
    return &nested->rwops;
}

int PLSDL2_RWopsToFile(SDL_RWops *rwops) {
    if (rwops != NULL) {
        return PL_File_CreateHandle(&PLSDL2_FileFunctions, rwops);
    } else {
        return -1;
    }
}

SDL_RWops *PLSDL2_FileOpenReadDirect(const char *filename) {
    return SDL_RWFromFile(filename, "rb");
}

int PL_Platform_FileOpenReadDirect(const char *filename) {
    SDL_RWops *rwops = PLSDL2_FileOpenReadDirect(filename);
    
    return PLSDL2_RWopsToFile(rwops);
}

#endif /* #ifdef DXPORTLIB_PLATFORM_SDL2 */
