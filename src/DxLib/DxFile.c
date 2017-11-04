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

#include "DPLBuildConfig.h"

#ifdef DXPORTLIB_DXLIB_INTERFACE

#include "PL/PLInternal.h"
#include "DxInternal.h"

#include "PL/SDL2/PLSDL2Internal.h"

#include "SDL.h"

#define DXPORTLIB_USE_GLOB

#ifdef DXPORTLIB_USE_GLOB
#  include <glob.h>
#  include <sys/stat.h>
#  include <time.h>
#endif

#if defined(__WIN32__) && !defined(__GNUC__)
/* For vsscanf hack down below. */
#include <stdio.h>
#endif

static int s_useArchiveFlag = DXTRUE;
static int s_allowDirectFlag = DXTRUE;
static char s_defaultArchiveString[DXA_KEY_LENGTH + 1] = { '\0' };
static char s_archiveExtension[64] = { '\0' };
static int s_initialized = DXFALSE;
static int s_filePriorityFlag = DXFALSE;
static int s_fileUseCharset = DX_CHARSET_DEFAULT;

/* ------------------------------------------------------------- ARCHIVE MANAGER */
#ifndef DX_NON_DXA
typedef struct ArchiveAliasEntry {
    char *srcArchiveName;
    char *destArchiveName;
    
    struct ArchiveAliasEntry *next;
} ArchiveAliasEntry;

typedef struct ArchiveListEntry {
    DXArchive *archive;
    char *filename;
    
    struct ArchiveListEntry *next;
} ArchiveListEntry;

static ArchiveAliasEntry *s_archiveAliases = NULL;
static ArchiveListEntry *s_archiveList = NULL;

static DXArchive *s_GetArchive(const char *filename) {
    /* - Check to see if this archive is already open first. */
    DXArchive *archive;
    ArchiveListEntry *entry;
    
    for (entry = s_archiveList; entry != NULL; entry = entry->next) {
        if (!PL_Text_Strcmp(entry->filename, filename)) {
            return entry->archive;
        }
    }
    
    /* - Since it isn't, try to load it up. */
    archive = DXA_OpenArchive(filename, s_defaultArchiveString);
    if (archive != NULL) {
        /* - On success, add to the open list. */
        entry = DXALLOC(sizeof(ArchiveListEntry));
        entry->archive = archive;
        entry->filename = PL_Text_Strdup(filename);
        entry->next = s_archiveList;
        s_archiveList = entry;
    }
    
    return archive;
}

static int s_CloseArchive(const char *filename) {
    ArchiveListEntry **pEntry = &s_archiveList;
    ArchiveListEntry *entry;
    
    while ((entry = *pEntry) != NULL) {
        if (!PL_Text_Strcmp(entry->filename, filename)) {
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

static DXArchive *s_GetArchive(const char *filename) {
    return NULL;
}

static int s_CloseArchive(const char *filename) {
    return -1;
}

static void s_OpenArchives() {
}

static void s_CloseArchives() {
}
#endif

static int s_GetArchiveFilename(
    const char *filename, char *buf, int maxLen,
    const char **pEnd, int aliasFlag
) {
    /* Extract the archive name from the filename. */
    const char *end = filename;
    const char *cur = filename;
    unsigned int ch;
    int position;
    
    while ((ch = PL_Text_ReadUTF8Char(&cur)) != 0) {
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
            position += PL_Text_WriteUTF8Char(buf + position, '.', maxLen - position);
            position += PL_Text_Strncpy(buf + position, s_archiveExtension, maxLen - position);
        } else {
            position += PL_Text_ConvertStrncpy(buf + position, -1,
                                   ".dxa", DX_CHARSET_EXT_UTF8,
                                   maxLen - position);
        }
        
        if (s_archiveAliases != NULL && aliasFlag != 0) {
            ArchiveAliasEntry *entry = s_archiveAliases;
            while (entry != NULL) {
                if (PL_Text_Strcmp(buf, entry->srcArchiveName) == 0) {
                    PL_Text_Strncpy(buf, entry->destArchiveName, maxLen);
                    break;
                }
                entry = entry->next;
            }
        }
        
        if (pEnd != NULL) {
            *pEnd = end;
        }
        
        return position;
    }
    
    return 0;
}

/* ------------------------------------------------------------ STREAM INTERFACE */
SDL_RWops *Dx_File_OpenArchiveStream(const char *filename) {
    /* Extract the archive name from the filename. */
    char buf[2048];
    const char *end;
    if (s_GetArchiveFilename(filename, buf, 2048, &end, 1) > 0) {
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
    if (s_archiveAliases != NULL) {
        if (s_GetArchiveFilename(filename, buf, 2048, &end, 0) > 0) {
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
    }
    
    return NULL;
}

SDL_RWops *Dx_File_OpenDirectStream(const char *filename) {
    if (s_allowDirectFlag == DXFALSE) {
        return NULL;
    }

    return SDL_RWFromFile(filename, "rb");
}

SDL_RWops *Dx_File_OpenStream(const char *filename) {
    if (s_useArchiveFlag == DXFALSE) {
        return Dx_File_OpenDirectStream(filename);
    } else if (s_filePriorityFlag == DXTRUE) {
        SDL_RWops *rwops = Dx_File_OpenDirectStream(filename);
        if (rwops != NULL) {
            return rwops;
        }
        return Dx_File_OpenArchiveStream(filename);
    } else {
        SDL_RWops *rwops = Dx_File_OpenArchiveStream(filename);
        if (rwops != NULL) {
            return rwops;
        }
        return Dx_File_OpenDirectStream(filename);
    }
}

int Dx_File_ReadFile(const char *filename, unsigned char **dData, unsigned int *dSize) {
    unsigned char *data;
    unsigned int size;
    int retval;
    
    SDL_RWops *rwops = Dx_File_OpenStream(filename);
    
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

/* ------------------------------------------------------------ STREAMING CODE */
typedef struct LocalFindData {
#ifdef DXPORTLIB_USE_GLOB
    int globFlag;
    glob_t globData;
    int globIndex;
#endif
#ifndef DX_NON_DXA
    int dxaFlag;

    DXAFindData *dxaData;
#endif
} LocalFindData;

int Dx_FileRead_findNext(DWORD_PTR fileHandle, FILEINFOA *fileInfo) {
    LocalFindData *data = (LocalFindData *)fileHandle;

    if (data == 0) {
        return -1;
    }

#ifndef DX_NON_DXA
    if (data->dxaFlag == DXTRUE) {
        return DXA_findNext(data->dxaData, fileInfo);
    }
#endif

#ifdef DXPORTLIB_USE_GLOB
    if (data->globFlag == DXTRUE) {
        struct stat sb;

        if (data->globIndex >= data->globData.gl_pathc) {
            return -1;
        }

        memset(fileInfo, 0, sizeof(FILEINFOW));

        PL_Text_ConvertStrncpy(fileInfo->Name, g_DxUseCharSet,
            data->globData.gl_pathv[data->globIndex], -1,
            FILEINFONAMELEN);

        if (stat(data->globData.gl_pathv[data->globIndex], &sb) == 0) {
            struct tm *lt;

            if (S_ISDIR(sb.st_mode) != 0) {
                fileInfo->DirFlag = DXTRUE;
            } else {
                fileInfo->Size = (LONGLONG)sb.st_size;
            }

            lt = localtime(&sb.st_mtim.tv_sec);
            fileInfo->CreationTime.Year = lt->tm_year + 1900;
            fileInfo->CreationTime.Mon = lt->tm_mon;
            fileInfo->CreationTime.Day = lt->tm_mday;
            fileInfo->CreationTime.Hour = lt->tm_hour;
            fileInfo->CreationTime.Min = lt->tm_min;
            fileInfo->CreationTime.Sec = lt->tm_sec;

            memcpy(&fileInfo->CreationTime, &fileInfo->LastWriteTime, sizeof(DATEDATA));
        }

        data->globIndex += 1;

        return 0;
    }
#endif

    return -1;
}

DWORD_PTR Dx_FileRead_findFirst(const char *filePath, FILEINFOA *fileInfo) {
    LocalFindData *data = DXCALLOC(sizeof(LocalFindData));

#ifndef DX_NON_DXA
    if (s_useArchiveFlag == DXTRUE) {
        char buf[2048];
        const char *end;
        if (s_GetArchiveFilename(filePath, buf, 2048, &end, 1) > 0) {
            DXArchive *archive;
            
            archive = s_GetArchive(buf);
            if (archive != NULL) {
                data->dxaData = DXA_findFirst(archive, end, fileInfo);
                if (data->dxaData != 0) {
                    data->dxaFlag = DXTRUE;
                    return (DWORD_PTR)data;
                }
            }
        }
    }
#endif

#ifdef DXPORTLIB_USE_GLOB
    if (s_allowDirectFlag == DXTRUE) {
        char pathBuf[DX_STRMAXLEN];
        int retval;
        /* glob is technically inaccurate here, since this is a clone of Windows'
        * FindFirstFile. FindFirstFile does not allow wildcards in pathnames, it
        * only seeks over a single directory's worth of contents.
        *
        * So we just assume the source code respects that. Because it should. */
        retval = glob(
            PL_Text_ConvertStrncpyIfNecessary(pathBuf, -1,
                    filePath, g_DxUseCharSet, DX_STRMAXLEN),
            0, NULL, &data->globData);

        if (retval == 0) {
            data->globIndex = 0;
            if (Dx_FileRead_findNext((DWORD_PTR)data, fileInfo) == 0) {
                data->globFlag = DXTRUE;

                return (DWORD_PTR)data;
            }
            globfree(&data->globData);
        }
    }
#endif

    DXFREE(data);
    return 0;
}

int Dx_FileRead_findClose(DWORD_PTR fileHandle) {
    LocalFindData *data = (LocalFindData *)fileHandle;
    if (data == 0) {
        return -1;
    }

#ifndef DX_NON_DXA
    if (data->dxaFlag == DXTRUE) {
        DXA_findClose(data->dxaData);
    }
#endif

#ifdef DXPORTLIB_USE_GLOB
    if (data->globFlag == DXTRUE) {
        globfree(&data->globData);
    }
#endif

    DXFREE(data);

    return 0;
}

/* ------------------------------------------------------------ PUBLIC INTERFACE */
int Dx_File_EXTSetDXArchiveAlias(const char *srcName, const char *destName) {
    ArchiveAliasEntry **pEntry = &s_archiveAliases;
    
    while (*pEntry != NULL) {
        if (PL_Text_Strcmp((*pEntry)->srcArchiveName, srcName) == 0) {
            DXFREE((*pEntry)->destArchiveName);
            if (destName == NULL) {
                ArchiveAliasEntry *next = (*pEntry)->next;
                DXFREE((*pEntry)->srcArchiveName);
                DXFREE(*pEntry);
                
                *pEntry = next;
            } else {
                (*pEntry)->destArchiveName = PL_Text_Strdup(destName);
            }
            return 0;
        }
        pEntry = &(*pEntry)->next;
    }
    
    if (srcName == NULL || destName == NULL) {
        return 0;
    }
    
    *pEntry = DXALLOC(sizeof(ArchiveAliasEntry));
    (*pEntry)->srcArchiveName = PL_Text_Strdup(srcName);
    (*pEntry)->destArchiveName = PL_Text_Strdup(destName);
    (*pEntry)->next = NULL;
    return 0;
}

/* Sets the "encryption" key to use for the packfile. */
int Dx_File_SetDXArchiveKeyString(const char *keyString) {
    int n = 0;
    
    if (keyString != 0) {
        unsigned int ch;
        while ((ch = PL_Text_ReadUTF8Char(&keyString)) != 0 && n < DXA_KEY_LENGTH) {
            s_defaultArchiveString[n] = (char)ch;
            n += 1;
        }
    }
    
    s_defaultArchiveString[n] = '\0';
    
    return 0;
}

int Dx_File_SetDXArchiveExtension(const char *extension) {
    if (extension != 0) {
        PL_Text_Strncpy(s_archiveExtension, extension, sizeof(s_archiveExtension));
    } else {
        memset(s_archiveExtension, 0, sizeof(s_archiveExtension));
    }
    
    return 0;
}

int Dx_File_SetDXArchivePriority(int priority) {
    s_filePriorityFlag = (priority == DXFALSE) ? DXFALSE : DXTRUE;
    
    return 0;
}

/* Setting this flag tells the archiver to check the archives instead of a direct
 * file access.
 */
int Dx_File_SetUseDXArchiveFlag(int flag) {
    flag = (flag == DXFALSE ? DXFALSE : DXTRUE);
    if (flag != s_useArchiveFlag) {
        s_useArchiveFlag = flag;
    }

    return 0;
}
int Dx_File_GetUseDXArchiveFlag() {
    return s_useArchiveFlag;
}

int Dx_File_DXArchivePreLoad(const char *dxaFilename, int async) {
    char buf[2048];
    DXArchive *archive;
    if (s_GetArchiveFilename(dxaFilename, buf, 2048, NULL, 1) > 0) {
        archive = s_GetArchive(buf);
        if (archive != NULL) {
            /* FIXME async not supported */
            return DXA_PreloadArchive(archive);
        }
    }
    return -1;
}
int Dx_File_DXArchiveCheckIdle(const char *dxaFilename) {
    /* This has no meaning when async is not supported,
     * so it is always idle. */
    return DXTRUE;
}
int Dx_File_DXArchiveRelease(const char *dxaFilename) {
    char buf[2048];
    if (s_GetArchiveFilename(dxaFilename, buf, 2048, NULL, 1) > 0) {
        return s_CloseArchive(buf);
    }
    return -1;
}

int Dx_File_DXArchiveCheckFile(const char *dxaFilename, const char *filename) {
    char buf[2048];
    DXArchive *archive;
    if (s_GetArchiveFilename(dxaFilename, buf, 2048, NULL, 1) > 0) {
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

int PLEXT_FileRead_SetCharSet(int charset) {
    s_fileUseCharset = charset;
    
    return 0;
}

int Dx_FileRead_open(const char *filename) {
    SDL_RWops *rwops = Dx_File_OpenStream(filename);
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
int64_t Dx_FileRead_size(const char *filename) {
    SDL_RWops *rwops = Dx_File_OpenStream(filename);
    if (rwops != NULL) {
        int64_t size = rwops->size(rwops);
        rwops->close(rwops);
        return size;
    }
    return 0;
}
int Dx_FileRead_close(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        SDL_RWclose(rwops);
        
        PL_Handle_ReleaseID(fileHandle, DXTRUE);
        
        return 0;
    }
    return -1;
}
int64_t Dx_FileRead_tell(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        return SDL_RWtell(rwops);
    }
    return 0;
}
int Dx_FileRead_seek(int fileHandle, int64_t position, int origin) {
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

int Dx_FileRead_read(void *data, int size, int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        return (int)SDL_RWread(rwops, data, (size_t)size, 1) * size;
    }
    return 0;
}
int Dx_FileRead_eof(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        return (SDL_RWsize(rwops) == SDL_RWtell(rwops)) ? DXTRUE : DXFALSE;
    }
    return DXFALSE;
}

static int Dx_FileRead_getWholeChar(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        char buffer[8];
        const char *reader = buffer;
        int pos = 0;
        
        do {
            if (SDL_RWread(rwops, buffer + pos, sizeof(char), 1) < 1) {
                return -1;
            }
            pos += 1;
        } while (pos < 7 && PL_Text_IsIncompleteMultibyte(buffer, pos, s_fileUseCharset));
        buffer[pos] = '\0';
        
        return PL_Text_ReadChar(&reader, s_fileUseCharset);
    }
    return -1;
}

int Dx_FileRead_getsA(char *buffer, int bufferSize, int fileHandle) {
    int ch;
    int remaining = bufferSize - 1;
    
    if (Dx_FileRead_eof(fileHandle)) {
        return -1;
    }
    
    while (remaining > 0 && (ch = Dx_FileRead_getWholeChar(fileHandle)) != -1) {
        int chSize;
        
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            break;
        }
        chSize = PL_Text_WriteChar(buffer, ch, remaining, g_DxUseCharSet);
        remaining -= chSize;
        buffer += chSize;
    }
    
    *buffer = '\0';
    
    return bufferSize - remaining - 1;
}
int Dx_FileRead_getsW(wchar_t *buffer, int bufferSize, int fileHandle) {
    int ch;
    int remaining = bufferSize - 1;
    
    if (Dx_FileRead_eof(fileHandle)) {
        return -1;
    }
    
    /* TODO: support and skip 0xfeff header. */
    
    while (remaining > 0 && (ch = Dx_FileRead_getWholeChar(fileHandle)) != -1) {
        if (ch == '\r') {
            continue;
        }
        if (ch == '\n') {
            break;
        }
        *buffer = ch;
        remaining -= 1;
        buffer += 1;
    }
    
    *buffer = '\0';
    
    return bufferSize - remaining - 1;
}

char Dx_FileRead_getcA(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        char ch;
        if (SDL_RWread(rwops, &ch, sizeof(char), 1) < 1) {
            return (char)-1;
        }
        
        return ch;
    }
    return (char)-1;
}
wchar_t Dx_FileRead_getcW(int fileHandle) {
    FileHandle *handle = (FileHandle *)PL_Handle_GetData(fileHandle, DXHANDLE_FILE);
    if (handle != NULL) {
        SDL_RWops *rwops = handle->rwops;
        wchar_t ch;
        if (SDL_RWread(rwops, &ch, sizeof(wchar_t), 1) < 1) {
            return (wchar_t)-1;
        }
        
        return (wchar_t)ch;
    }
    return (wchar_t)-1;
}

int Dx_FileRead_vscanfA(int fileHandle, const char *format, va_list args) {
    char buffer[4096];
    int len;
    
    len = Dx_FileRead_getsA(buffer, 4096, fileHandle);
    if (len < 0) {
        return 0;
    }
    
    return PL_Text_Vsscanf(buffer, g_DxUseCharSet, format, args);
}
int Dx_FileRead_vscanfW(int fileHandle, const wchar_t *format, va_list args) {
    wchar_t buffer[4096];
    int len;
    
    len = Dx_FileRead_getsW(buffer, 4096, fileHandle);
    if (len < 0) {
        return 0;
    }
    
    return PL_Text_Wvsscanf(buffer, g_DxUseCharSet, format, args);
}

int Dx_File_OpenRead(const char *filename) {
    SDL_RWops *rwops = Dx_File_OpenStream(filename);
    if (rwops != NULL) {
        return PLSDL2_RWopsToFile(rwops);
    }
    return -1;
}

int Dx_File_Init() {
    s_initialized = DXTRUE;

    s_OpenArchives();
    
    PL_File_SetOpenReadFunction(Dx_File_OpenRead);
    
    return 0;
}

int Dx_File_End() {
    int fileHandle;
    
    while ((fileHandle = PL_Handle_GetFirstIDOf(DXHANDLE_FILE)) >= 0) {
        Dx_FileRead_close(fileHandle);
    }
    
    s_CloseArchives();
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_DXLIB_INTERFACE */
