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

/* This file handles both the main LunaFile:: functions as well as
 * the archive format. Only reading is supported at this time, not writing.
 */

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"
#include "PL/PLInternal.h"

typedef struct _ArchiveHeader {
    Uint32 ID;
    Uint32 entrycount;
} ArchiveHeader;

typedef struct _ArchiveEntry {
    char filename[64];
    Uint32 namecrc;
    Uint32 datacrc;
    Uint32 address;
    Uint32 size;
} ArchiveEntry;

typedef struct _LunaArchive {
    char *filename;
    char *rootPath;
    Uint32 priority;
    
    ArchiveEntry *entries;
    int entrycount;
    
    struct _LunaArchive *next;
} LunaArchive;

static LunaArchive *s_archiveList = NULL;

static Uint32 s_CalcCrc(const Uint8 *data, int length) {
    Uint32 crc = 0xffffffff;
    int i, j;
    
    for (i = 0; i < length; ++i) {
        crc ^= data[i];
        for (j = 0; j < 8; ++j) {
            if ((crc & 1) != 0) {
                crc = (crc >> 1) ^ 0xedb88320;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc ^ 0xffffffff;
}

static int s_ProcessFilename(char *targetBuffer, int len, const char *src) {
    unsigned int ch;
    char *start = targetBuffer;
    char *end = targetBuffer + len - 1;
    
    while (targetBuffer < end && (ch = PL_Text_ReadUTF8Char(&src)) != 0) {
        if (ch >= 'a' && ch <= 'z') {
            ch -= 'a' - 'A';
        } else if (ch == '/') {
            ch = '\\';
        }
        targetBuffer += PL_Text_WriteChar(targetBuffer, ch, end - targetBuffer, DX_CHARSET_SHFTJIS);
    }
    *targetBuffer = '\0';
    
    return targetBuffer - start;
}

static Uint32 s_CalcCrcOfString(const char *string) {
    char namebuf[65];
    int namelen;
    
    namelen = s_ProcessFilename(namebuf, 65, string);
    return s_CalcCrc((const Uint8 *)namebuf, namelen);
}

static int s_FindIndexOfCRC(LunaArchive *archive, Uint32 crc) {
    ArchiveEntry *entries = archive->entries;
    int left = 0;
    int right = archive->entrycount - 1;
    
    if (entries == NULL) {
        return -1;
    }
    
    if (crc < entries[0].namecrc || crc > entries[right].namecrc) {
        return -1;
    }
    
    /* Binary split. Finds the entry in 2^n time.
     * Luna's somewhat odd behavior is mimiced identically:
     * - Luna does not check for duplicate crcs for different filenames.
     * - When there are fewer than 5 elements remaining, it does them
     *   all in numerical order.
     * - Due to a bug, if it does not do that, it will loop infinitely.
     */
    while (left <= right) {
        int center = left + ((right - left) / 2);
        if ((right - left) < 5) {
            int i;
            for (i = left; i <= right; ++i) {
                if (entries[i].namecrc == crc) {
                    return i;
                }
            }
            return -1;
        } else if (entries[center].namecrc == crc) {
            return center;
        } else if (entries[center].namecrc > crc) {
            right = center - 1;
        } else {
            left = center;
        }
    }
    
    return -1;
}

static LunaArchive *LunaArchive_Open(Uint32 priority,
                                     const char *pRootPath,
                                     const char *pFilename) {
    int fileHandle;
    int entrySize;
    ArchiveHeader header;
    ArchiveEntry *entries;
    LunaArchive *archive;
    
    archive = (LunaArchive *)DXALLOC(sizeof(LunaArchive));
    archive->filename = NULL;
    archive->rootPath = NULL;
    archive->priority = priority;
    archive->entries = NULL;
    archive->entrycount = 0;
    archive->next = NULL;

    if (pRootPath != NULL) {
        char path[4096];
        PL_Text_Strncpy(path, pRootPath, 4096);
        PL_Text_Strncat(path, "/", 4096);
        
        archive->rootPath = PL_Text_Strdup(path);
    }
    
    do {
        /* As far as I know, Luna does not allow nesting of packfiles. */
        fileHandle = PL_Platform_FileOpenReadDirect(pFilename);
        if (fileHandle < 0) {
            break;
        }
        
        if (PL_File_Read(fileHandle, &header, sizeof(ArchiveHeader)) != sizeof(ArchiveHeader)) {
            break;
        }
        
        /* put a hard cap on file entries for sanity's sake */
        if (memcmp(&header.ID, "PACK", 4) || header.entrycount > 65536) {
            break;
        }
        
        entrySize = sizeof(ArchiveEntry) * header.entrycount;
        
        if (PL_File_GetSize(fileHandle) < (int64_t)(entrySize + sizeof(ArchiveHeader))) {
            break;
        }
        
        entries = (ArchiveEntry *)DXALLOC(entrySize);
        if (PL_File_Read(fileHandle, entries, entrySize) != entrySize) {
            DXFREE(entries);
            break;
        }
        
        archive->filename = PL_Text_Strdup(pFilename);
        archive->entries = entries;
        archive->entrycount = header.entrycount;
    } while(0);
    
    PL_File_Close(fileHandle);
    
    return archive;
}
static void LunaArchive_Close(LunaArchive *archive) {
    DXFREE(archive->entries);
    if (archive->filename != NULL) {
        DXFREE(archive->filename);
    }
    if (archive->rootPath != NULL) {
        DXFREE(archive->rootPath);
    }
    DXFREE(archive);
}

static int LunaArchive_OpenFileIndex(LunaArchive *archive, int index) {
    int archiveHandle;
    ArchiveEntry *entry;
    
    if (index < 0 || index >= archive->entrycount) {
        return -1;
    }
    
    archiveHandle = PL_Platform_FileOpenReadDirect(archive->filename);
    if (archiveHandle < 0) {
        return -1;
    }
    
    /* Streams do not support compression, so no need to handle it. */
    
    entry = &archive->entries[index];
    return PL_File_CreateHandleSubsection(archiveHandle,
                                          entry->address, entry->size,
                                          DXTRUE);
}

static int LunaFile_OpenRead(const char *filename, int *isPacked) {
    int handle = -1;
    int packed = 0;
    LunaArchive *archive;
    Uint32 crc = s_CalcCrcOfString(filename);
    
    for (archive = s_archiveList; archive != NULL; archive = archive->next) {
        /* Attempt to open file directly. */
        char pathname[4096];
        int index;
        
        if (archive->rootPath != NULL) {
            PL_Text_Strncpy(pathname, archive->rootPath, 4096);
            PL_Text_Strncat(pathname, filename, 4096);
            handle = PL_Platform_FileOpenReadDirect(pathname);
            if (handle >= 0) {
                break;
            }
        }
        
        /* Attempt to open from archive. */
        index = s_FindIndexOfCRC(archive, crc);
        if (index >= 0) {
            handle = LunaArchive_OpenFileIndex(archive, index);
            if (handle >= 0) {
                packed = 1;
                break;
            }
        }
    }
    
    if (handle < 0) {
        handle = PL_Platform_FileOpenReadDirect(filename);
    }
    
    if (isPacked != NULL) {
        *isPacked = packed;
    }
    
    return handle;
}
static int LunaFile_OpenReadNoPack(const char *filename) {
    return LunaFile_OpenRead(filename, NULL);
}

void LunaFile::SetRootPath(Uint32 Priority,
                           const char *pRootPath,
                           const char *pPackFile) {
    LunaArchive *archive = LunaArchive_Open(Priority, pRootPath, pPackFile);
    if (archive != NULL) {
        LunaArchive **da = &s_archiveList;
        
        while (*da != NULL && (*da)->priority < Priority) {
            da = &((*da)->next);
        }
        
        archive->next = *da;
        *da = archive;
    }
}

FILEDATA * LunaFile::FileOpen(const char *pFile, Bool ReadOnly) {
    /* NOTICE: ReadOnly is ignored. We do not support writing. */
    /* NOTICE: File creation/modification times not supported. */
    int isPacked;
    char filename[4096];
    const char *filebuf = PL_Text_ConvertStrncpyIfNecessary(
        filename, -1, pFile, g_lunaUseCharSet, 4096);
    int handle = LunaFile_OpenRead(filebuf, &isPacked);
    
    if (handle >= 0) {
        FILEDATA *filedata = (FILEDATA *)DXALLOC(sizeof(FILEDATA));
        int len = PL_Text_Strlen(filebuf);
        
        memset(filedata, 0, sizeof(FILEDATA));
        filedata->dxPortLibFileHandle = handle;
        filedata->IsPack = isPacked;
        filedata->Start = 0;
        filedata->Size = (Uint32)PL_File_GetSize(handle);
        
        if (len >= MAX_PATH) {
            len = MAX_PATH - 1;
        }
        memcpy(filedata->FileName, pFile, len);
        filedata->FileName[len] = '\0';
        
        return filedata;
    }
    
    return 0;
}

Uint32 LunaFile::FileGetSize(FILEDATA *pFile) {
    if (pFile == NULL) {
        return 0;
    }
    return (Uint32)PL_File_GetSize(pFile->dxPortLibFileHandle);
}
Uint32 LunaFile::FileGetPosition(FILEDATA *pFile) {
    if (pFile == NULL) {
        return 0;
    }
    return (Uint32)PL_File_Tell(pFile->dxPortLibFileHandle);
}
Uint32 LunaFile::FileRead(FILEDATA *pFile, Uint32 Size, void *pData) {
    if (pFile == NULL) {
        return 0;
    }
    return (Uint32)PL_File_Read(pFile->dxPortLibFileHandle, pData, Size);
}

void LunaFile::FileClose(FILEDATA *pFile) {
    if (pFile != NULL) {
        PL_File_Close(pFile->dxPortLibFileHandle);
        DXFREE(pFile);
    }
}

void LunaFile_Init() {
    PL_File_SetOpenReadFunction(LunaFile_OpenReadNoPack);
}

void LunaFile_End() {
    if (s_archiveList != NULL) {
        LunaArchive *anext, *a;
        anext = s_archiveList;
        while ((a = anext) != NULL) {
            anext = a->next;
            LunaArchive_Close(a);
        }
        s_archiveList = NULL;
    }
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
