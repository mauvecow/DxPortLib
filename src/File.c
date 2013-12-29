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

static int s_useArchiveFlag = DXTRUE;
static char s_defaultArchiveString[DXA_KEY_LENGTH + 1] = { '\0' };
static DXCHAR s_archiveExtension[64] = { '\0' };
static int s_initialized = DXFALSE;
static int s_filePriorityFlag = DXFALSE;

/* ------------------------------------------------------------- ARCHIVE MANAGER */
#ifndef DX_NON_DXA
typedef struct ArchiveListEntry {
    DXArchive *archive;
    DXCHAR *filename;
    
    struct ArchiveListEntry *next;
} ArchiveListEntry;

static ArchiveListEntry *s_archiveList = NULL;

static DXArchive *s_GetArchive(const DXCHAR *filename) {
    /* - Check to see if this archive is already open first. */
    DXArchive *archive;
    ArchiveListEntry *entry;
    
    for (entry = s_archiveList; entry != NULL; entry = entry->next) {
        if (!DXSTRCMP(entry->filename, filename)) {
            return entry->archive;
        }
    }
    
    /* - Since it isn't, try to load it up. */
    archive = DXA_OpenArchive(filename, s_defaultArchiveString);
    if (archive != NULL) {
        /* - On success, add to the open list. */
        entry = DXALLOC(sizeof(ArchiveListEntry));
        entry->archive = archive;
        entry->filename = DXSTRDUP(filename);
        entry->next = s_archiveList;
        s_archiveList = entry;
    }
    
    return archive;
}

static int s_CloseArchive(const DXCHAR *filename) {
    ArchiveListEntry **pEntry = &s_archiveList;
    ArchiveListEntry *entry;
    
    while ((entry = *pEntry) != NULL) {
        if (!DXSTRCMP(entry->filename, filename)) {
            ArchiveListEntry *nextEntry = entry->next;
            
            DXA_CloseArchive(entry->archive);
            DXFREE(entry->filename);
            DXFREE(entry);
            
            *pEntry = nextEntry;
            
            return 0;
        }
        
        pEntry = &entry->next;
    }
    return -1;
}

static void s_OpenArchives() {
    /* - Stub, nothing to do. */
}

static void s_CloseArchives() {
    /* - Close all open archives. */
    ArchiveListEntry *entry, *nEntry;
    
    nEntry = s_archiveList;
    while ((entry = nEntry) != NULL) {
        nEntry = entry->next;
        
        DXA_CloseArchive(entry->archive);
        DXFREE(entry->filename);
        DXFREE(entry);
    }
    
    s_archiveList = NULL;
}

#else

static DXArchive *s_GetArchive(const DXCHAR *filename) {
    return NULL;
}

static int s_CloseArchive(const DXCHAR *filename) {
    return -1;
}

static void s_OpenArchives() {
}

static void s_CloseArchives() {
}
#endif

static int s_GetArchiveFilename(
    const DXCHAR *filename, DXCHAR *buf, int maxLen,
    const DXCHAR **pEnd
) {
    /* Extract the archive name from the filename. */
    const DXCHAR *end = filename;
    const DXCHAR *cur = filename;
    unsigned int ch;
    int position;
    
    while ((ch = PL_Text_ReadDxChar(&cur)) != 0) {
        if (ch == '/' || ch == '\\') {
            break;
        }
        end = cur;
    }
    
    position = end - filename;
    if (position > 0 && position < (maxLen - 64)) {
        int i;
        
        for (i = 0; i < position; ++i) {
            buf[i] = filename[i];
        }
        
        if (s_archiveExtension[0] != '\0') {
            DXCHAR *c = buf + position;
            c += PL_Text_WriteDxChar(c, '.', buf + maxLen - c);
            DXSTRNCPY(c, s_archiveExtension, buf + maxLen - c);
        } else {
            DXSTRNCPYFROMSTR(buf + position, ".dxa", maxLen - position, DX_CHARSET_EXT_UTF8);
        }
        
        if (pEnd != NULL) {
            *pEnd = end;
        }
        
        return DXSTRLEN(buf);
    }
    
    return 0;
}

/* ------------------------------------------------------------ STREAM INTERFACE */
SDL_RWops *PL_File_OpenArchiveStream(const DXCHAR *filename) {
    /* Extract the archive name from the filename. */
    DXCHAR buf[2048];
    const DXCHAR *end;
    if (s_GetArchiveFilename(filename, buf, 2048, &end) > 0) {
        DXArchive *archive;
        
        archive = s_GetArchive(buf);
        if (archive != NULL) {
            SDL_RWops *rwops = DXA_OpenStream(archive, end + 1);
            /* If we can open from the stream, do that. */
            if (rwops != NULL) {
                return rwops;
            }
        }
    }
    
    return NULL;
}

SDL_RWops *PL_File_OpenDirectStream(const DXCHAR *filename) {
    char utf8Buf[2048];
    
    if (PL_Text_DxStringToString(filename, utf8Buf, 2048, DX_CHARSET_EXT_UTF8) <= 0) {
        return NULL;
    }
    
    return SDL_RWFromFile(utf8Buf, "rb");
}

SDL_RWops *PL_File_OpenStream(const DXCHAR *filename) {
    if (s_useArchiveFlag == DXFALSE) {
        return PL_File_OpenDirectStream(filename);
    } else if (s_filePriorityFlag == DXTRUE) {
        SDL_RWops *rwops = PL_File_OpenDirectStream(filename);
        if (rwops != NULL) {
            return rwops;
        }
        return PL_File_OpenArchiveStream(filename);
    } else {
        SDL_RWops *rwops = PL_File_OpenArchiveStream(filename);
        if (rwops != NULL) {
            return rwops;
        }
        return PL_File_OpenDirectStream(filename);
    }
}

int PL_File_ReadFile(const DXCHAR *filename, unsigned char **dData, unsigned int *dSize) {
    unsigned char *data;
    unsigned int size;
    int retval;
    
    SDL_RWops *rwops = PL_File_OpenStream(filename);
    
    if (rwops == NULL) {
        return -1;
    }
    
    size = (unsigned int)SDL_RWsize(rwops);
    
    retval = 0;
    data = DXALLOC(size);
    if (SDL_RWread(rwops, data, size, 1) < 1) {
        retval = -1;
        DXFREE(data);
    } else {
        *dData = data;
        *dSize = size;
    }
    
    SDL_RWclose(rwops);
    
    return retval;
}

/* ------------------------------------------------------------ PUBLIC INTERFACE */
/* Sets the "encryption" key to use for the packfile. */
int PL_File_SetDXArchiveKeyString(const DXCHAR *keyString) {
    int n = 0;
    unsigned int ch;
    while ((ch = PL_Text_ReadDxChar(&keyString)) != 0 && n < DXA_KEY_LENGTH) {
        s_defaultArchiveString[n] = (char)ch;
        n += 1;
    }
    
    s_defaultArchiveString[n] = '\0';
    
    return 0;
}

int PL_File_SetDXArchiveExtension(const DXCHAR *extension) {
    DXSTRNCPY(s_archiveExtension, extension, 64);
    
    return 0;
}

int PL_File_SetDXArchivePriority(int flag) {
    s_filePriorityFlag = (flag == DXFALSE) ? DXFALSE : DXTRUE;
    
    return 0;
}

/* Setting this flag tells the archiver to check the archives instead of a direct
 * file access.
 */
int PL_File_SetUseDXArchiveFlag(int flag) {
    flag = (flag == DXFALSE ? DXFALSE : DXTRUE);
    if (flag != s_useArchiveFlag) {
        s_useArchiveFlag = flag;
    }
    
    return 0;
}
int PL_File_GetUseDXArchiveFlag() {
    return s_useArchiveFlag;
}

int PL_File_DXArchivePreLoad(const DXCHAR *dxaFilename, int async) {
    DXCHAR buf[2048];
    DXArchive *archive;
    if (s_GetArchiveFilename(dxaFilename, buf, 2048, NULL) > 0) {
        archive = s_GetArchive(buf);
        if (archive != NULL) {
            return DXA_PreloadArchive(archive);
        }
    }
    return -1;
}
int PL_File_DXArchiveCheckIdle(const DXCHAR *dxaFilename) {
    /* This has no meaning when async is not supported,
     * so it is always idle. */
    return DXTRUE;
}
int PL_File_DXArchiveRelease(const DXCHAR *dxaFilename) {
    DXCHAR buf[2048];
    if (s_GetArchiveFilename(dxaFilename, buf, 2048, NULL) > 0) {
        return s_CloseArchive(buf);
    }
    return -1;
}

int PL_File_DXArchiveCheckFile(const DXCHAR *dxaFilename, const DXCHAR *filename) {
    DXCHAR buf[2048];
    DXArchive *archive;
    if (s_GetArchiveFilename(dxaFilename, buf, 2048, NULL) > 0) {
        archive = s_GetArchive(buf);
        if (archive != NULL) {
            return DXA_TestFile(archive, filename);
        }
    }
    return -1;
}

typedef struct FileHandle {
    SDL_RWops *rwops;
} FileHandle;

int PL_FileRead_open(const DXCHAR *filename) {
    SDL_RWops *rwops = PL_File_OpenStream(filename);
    int fileDataID;
    FileHandle *handle;
    
    if (rwops == NULL) {
        return -1;
    }

    fileDataID = PL_Handle_AcquireID(DXHANDLE_FILE);
    if (fileDataID < 0) {
        SDL_RWclose(rwops);
        return -1;
    }
    
    handle = (FileHandle *)PL_Handle_AllocateData(fileDataID, sizeof(FileHandle));
    handle->rwops = rwops;
    
    return fileDataID;
}
long long PL_FileRead_size(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        return SDL_RWsize(rwops);
    }
    return 0;
}
int PL_FileRead_close(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        SDL_RWclose(rwops);
        
        PL_Handle_ReleaseID(fileHandle, DXTRUE);
        
        return 0;
    }
    return -1;
}
long long PL_FileRead_tell(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        return SDL_RWtell(rwops);
    }
    return 0;
}
int PL_FileRead_seek(int fileHandle, long long position, int origin) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        switch(origin) {
            case 0: SDL_RWseek(rwops, position, RW_SEEK_SET); break;
            case 1: SDL_RWseek(rwops, position, RW_SEEK_CUR); break;
            case 2: SDL_RWseek(rwops, position, RW_SEEK_END); break;
        }
    }
    return 0;
}

int PL_FileRead_read(void *data, int size, int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        return (int)SDL_RWread(rwops, data, (size_t)size, 1) * size;
    }
    return 0;
}
int PL_FileRead_eof(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        return (SDL_RWsize(rwops) == SDL_RWtell(rwops)) ? DXTRUE : DXFALSE;
    }
    return DXFALSE;
}

int PL_File_Init() {
    s_initialized = DXTRUE;
    
    s_OpenArchives();
    
    return 0;
}

int PL_File_End() {
    int fileHandle;
    
    while ((fileHandle = PL_Handle_GetFirstIDOf(DXHANDLE_FILE)) >= 0) {
        PL_FileRead_close(fileHandle);
    }
    
    s_CloseArchives();
    
    return 0;
}
