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

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_DXLIB_INTERFACE

#ifndef DX_NON_DXA

#include "PL/PLInternal.h"
#include "DxInternal.h"

#include "SDL.h"

#define DXA_ATTRIBUTE_DIRECTORY         0x00000010

/* What this does implement:
 * - Full support for reading content from DXA files of all versions.
 * - LZ decompression.
 * - Streaming of archive data.
 * - Codepage support.
 * - Preloading an entire archive to memory.
 * 
 * What this does not implement:
 * - Incrementally streaming compressed data. (not immediately necessary.)
 * - Asynchronous loading. (technically the streaming can do it for non-compressed data)
 */

/* ------------------------------------------------------------ DXARCHIVE INTERNAL DATA TYPES */
struct DXArchive {
    SDL_RWops *File;
    
    int Version;
    
    char *utf8Filename;
    
    unsigned long long DataAddress;
    
    unsigned int CharSet;
    
    unsigned char *DataBlob;
    
    unsigned char *FileInfoTable;
    unsigned char *DirectoryInfoTable;
    unsigned char *SearchInfoTable;
    unsigned char *FileNameInfoTable;
    
    unsigned char Key[DXA_KEY_LENGTH];
    
    unsigned char *PreloadData;
    unsigned int PreloadSize;
};

typedef struct DXArchiveHeader {
    unsigned short              DXID;
    unsigned short              Version;
    unsigned int                HeaderSize;
} DXArchiveHeader;

typedef struct DXArchiveInfo {
    unsigned long long          DataAddress;
    unsigned long long          FileNameTableAddress;
    unsigned long long          FileTableAddress;
    unsigned long long          DirectoryTableAddress;
    
    unsigned long long          CodePage;
} DXArchiveInfo;

typedef struct DXArchiveInfoV5 {
    unsigned int                DataAddress;
    unsigned int                FileNameTableAddress;
    unsigned int                FileTableAddress;
    unsigned int                DirectoryTableAddress;
    
    unsigned int                CodePage;
} DXArchiveInfoV5;

typedef struct DXArchiveFileNameInfo {
    unsigned short              Length;
    unsigned short              Parity;
} DXArchiveFileNameInfo;

typedef struct DXArchiveFileInfo {
    unsigned long long          NameAddress;
    unsigned long long          Attributes;

    unsigned long long          CreationTime;
    unsigned long long          LastAccessTime;
    unsigned long long          LastWriteTime;
    
    unsigned long long          DataAddress;
    
    unsigned long long          DataSize;
    unsigned long long          CompressedDataSize;
} DXArchiveFileInfo;

typedef struct DXArchiveFileInfoV5 {
    unsigned int                NameAddress;
    unsigned int                Attributes;

    unsigned long long          CreationTime;
    unsigned long long          LastAccessTime;
    unsigned long long          LastWriteTime;
    
    unsigned int                DataAddress;
    
    unsigned int                DataSize;
    unsigned int                CompressedDataSize;
} DXArchiveFileInfoV5;
typedef struct DXArchiveFileInfoV1 {
    unsigned int                NameAddress;
    unsigned int                Attributes;

    unsigned long long          CreationTime;
    unsigned long long          LastAccessTime;
    unsigned long long          LastWriteTime;
    
    unsigned int                DataAddress;
    
    unsigned int                DataSize;
} DXArchiveFileInfoV1;

typedef struct DXArchiveDirectoryInfo {
    unsigned long long          DirectoryAddress;
    unsigned long long          ParentDirectoryAddress;
    unsigned long long          FileInfoCount;
    unsigned long long          FileInfoAddress;
} DXArchiveDirectoryInfo;

typedef struct DXArchiveDirectoryInfoV5 {
    unsigned int                DirectoryAddress;
    unsigned int                ParentDirectoryAddress;
    unsigned int                FileInfoCount;
    unsigned int                FileInfoAddress;
} DXArchiveDirectoryInfoV5;

typedef struct DXArchiveSearchInfo {
    unsigned char               Filename[1024];
    unsigned short              Parity;
    unsigned short              FileInfoID;
} DXArchiveSearchInfo;

/* ------------------------------------------------------------ DXARCHIVE FUNCTIONS */
static int DXA_ReadAndDecode(DXArchive *archive, unsigned long long position, void *dest, unsigned long long length);
static void DXA_Decode(DXArchive *archive, const void *src, void *dest, unsigned long long length, unsigned long long offset);

static int DXA_Decompress(const void *src, void *dest, unsigned long long dest_len);

static unsigned long long DXA_GetFileAddress(DXArchive *archive, const DXCHAR *filename);

static void DXA_GetFileInfo(DXArchive *archive, unsigned long long address, DXArchiveFileInfo *info);
static void DXA_GetDirectoryInfo(DXArchive *archive, unsigned long long address, DXArchiveDirectoryInfo *info);
static void DXA_GetFileNameInfo(DXArchive *archive, unsigned long long address, DXArchiveFileNameInfo *info, const char **dName);

static int DXA_InitializeArchive(DXArchive *archive);

/* ------------------------------------------------------------ DXARCHIVE IMPLEMENTATION */
static unsigned long long DXA_GetFileAddress(DXArchive *archive, const DXCHAR *filename) {
    /* So here we have to break up the filename into pieces.
     * 
     * The rules here are:
     * - '/' and '\' are both valid for directory indices.
     * - all filenames must be uppercase.
     */
    char utf8Buf[2048];
    char fileBuf[2048];
    const char *src = utf8Buf;
    unsigned long long directoryAddress = 0;
    int charSet = (int)archive->CharSet;
    
    if (PL_Text_DxStringToString(filename, utf8Buf, 2048, charSet) <= 0) {
        return 0;
    }
    
    while (*src != '\0') {
        DXArchiveDirectoryInfo dirInfo;
        int index = 0;
        unsigned int dirAttrib = 0;
        unsigned long long fileAddress;
        unsigned int ch;
        int foundDir;
        unsigned long long i;
        unsigned int parity = 0;
        
        while ((ch = PL_Text_ReadChar(&src, charSet)) != '\0') {
            if (ch == '\\' || ch == '/') {
                dirAttrib = DXA_ATTRIBUTE_DIRECTORY;
                break;
            } else {
                if (ch >= 'a' && ch <= 'z') {
                    ch = ch + 'A' - 'a';
                }
                
                parity += (ch >> 8) + (ch & 0xff);
                
                index += PL_Text_WriteChar(fileBuf + index, ch, 2047 - index, charSet);
            }
        }
        
        fileBuf[index] = '\0';
        
        if (index == 0)
            continue;
        
        DXA_GetDirectoryInfo(archive, directoryAddress, &dirInfo);
        fileAddress = dirInfo.FileInfoAddress;
        foundDir = 0;
        for (i = 0; i < dirInfo.FileInfoCount; ++i) {
            DXArchiveFileInfo fileInfo;
            const char *filename;
            
            DXA_GetFileInfo(archive, fileAddress, &fileInfo);
            if ((fileInfo.Attributes & DXA_ATTRIBUTE_DIRECTORY) == dirAttrib) {
                DXArchiveFileNameInfo fileNameInfo;
                DXA_GetFileNameInfo(archive, fileInfo.NameAddress, &fileNameInfo, &filename);
                
                if (fileNameInfo.Parity == parity && !SDL_strcmp(filename, fileBuf)) {
                    if (dirAttrib != 0) {
                        /* Traverse into subdirectory. */
                        directoryAddress = fileInfo.DataAddress;
                        foundDir = 1;
                        break;
                    } else {
                        return fileAddress;
                    }
                }
            }
            
            if (archive->Version >= 6) {
                fileAddress += sizeof(DXArchiveFileInfo);
            } else if (archive->Version >= 1) {
                fileAddress += sizeof(DXArchiveFileInfoV5);
            } else {
                fileAddress += sizeof(DXArchiveFileInfoV1);
            }
        }
        
        if (foundDir == 0)
            break;
    }
    
    return 0;
}

static int DXA_ReadCompressedFile(
    DXArchive *archive, DXArchiveFileInfo *fileInfo, unsigned char **dData, unsigned int *dSize
) {
    unsigned char *data = (unsigned char *)DXALLOC((size_t)fileInfo->CompressedDataSize);
    unsigned char *decompressed;
    unsigned long long address = archive->DataAddress + fileInfo->DataAddress;
    
    if (archive->PreloadData != NULL) {
        if ((address + fileInfo->CompressedDataSize) > archive->PreloadSize) {
            DXFREE(data);
            return -1;
        }
        SDL_memcpy(data, archive->PreloadData + address, (size_t)fileInfo->CompressedDataSize);
    } else if (DXA_ReadAndDecode(archive, address, data, fileInfo->CompressedDataSize) < 0) {
        DXFREE(data);
        return -1;
    }
    
    decompressed = (unsigned char *)DXALLOC((size_t)fileInfo->DataSize);
    if (DXA_Decompress(data, decompressed, fileInfo->DataSize) < 0) {
        DXFREE(decompressed);
        DXFREE(data);
        return -1;
    }
    
    DXFREE(data);
    
    *dData = decompressed;
    *dSize = (unsigned int)fileInfo->DataSize;
    
    return 0;
}

int DXA_ReadFile(DXArchive *archive, const DXCHAR *filename, unsigned char **dData, unsigned int *dSize) {
    unsigned long long index = DXA_GetFileAddress(archive, filename);
    DXArchiveFileInfo fileInfo;
    if (index == 0) {
        return -1;
    }
    
    DXA_GetFileInfo(archive, index, &fileInfo);
    
    if (fileInfo.CompressedDataSize == 0xffffffff) {
        unsigned char *data = (unsigned char *)DXALLOC((size_t)fileInfo.DataSize);
        unsigned long long address = archive->DataAddress + fileInfo.DataAddress;
        
        if (archive->PreloadData != NULL) {
            if ((address + fileInfo.DataSize) > archive->PreloadSize) {
                DXFREE(data);
                return -1;
            }
            SDL_memcpy(data, archive->PreloadData + address, (size_t)fileInfo.DataSize);
        } else if (DXA_ReadAndDecode(archive, address, data, fileInfo.DataSize) < 0) {
            DXFREE(data);
            return -1;
        }
        
        *dData = data;
        *dSize = (unsigned int)fileInfo.DataSize;
        
        return 0;
    } else {
        return DXA_ReadCompressedFile(archive, &fileInfo, dData, dSize);
    }
}

int DXA_TestFile(DXArchive *archive, const DXCHAR *filename) {
    unsigned long long index = DXA_GetFileAddress(archive, filename);
    if (index == 0) {
        return -1;
    }
    return 0;
}

void DXA_CloseArchive(DXArchive *archive) {
    if (archive->DataBlob != NULL) {
        DXFREE(archive->DataBlob);
        archive->DataBlob = NULL;
    }
    archive->FileInfoTable = NULL;
    archive->DirectoryInfoTable = NULL;
    archive->SearchInfoTable = NULL;
    archive->FileNameInfoTable = NULL;
    
    SDL_free(archive->utf8Filename);
    
    if (archive->PreloadData != NULL) {
        DXFREE(archive->PreloadData);
        archive->PreloadData = NULL;
    }
    archive->PreloadSize = 0;
    
    SDL_RWclose(archive->File);
    archive->File = NULL;
    
    DXFREE(archive);
}

int DXA_PreloadArchive(DXArchive *archive) {
    if (archive->PreloadData == NULL) {
        unsigned char *preloadData;
        unsigned int preloadSize;
        
        preloadSize = (unsigned int)SDL_RWsize(archive->File);
        if (preloadSize > 0) {
            preloadData = DXALLOC(preloadSize + 12);
            
            SDL_RWseek(archive->File, 0, RW_SEEK_SET);
            if (SDL_RWread(archive->File, preloadData, preloadSize, 1) < 1) {
                DXFREE(preloadData);
                return -1;
            }
            DXA_Decode(archive, preloadData, preloadData, preloadSize, 0);
            
            archive->PreloadData = preloadData;
            archive->PreloadSize = preloadSize;
        }
    }
    return 0;
}

DXArchive *DXA_OpenArchive(const DXCHAR *filename, const char *keyString) {
    SDL_RWops *rwops;
    char utf8Buf[2048];
    DXArchive *archive;

    if (PL_Text_DxStringToString(filename, utf8Buf, 2048, DX_CHARSET_EXT_UTF8) <= 0) {
        return NULL;
    }
    
    /* - Make sure we can open the thing. */
    rwops = SDL_RWFromFile(utf8Buf, "rb");
    if (rwops == NULL) {
        return NULL;
    }
    
    archive = (DXArchive *)DXALLOC(sizeof(DXArchive));
    
    archive->utf8Filename = SDL_strdup(utf8Buf);
    archive->File = rwops;
    archive->DataBlob = NULL;
    archive->PreloadData = NULL;
    archive->PreloadSize = 0;
    
    DXA_SetArchiveKey(archive, keyString);
    
    archive->File = rwops;
    
    if (DXA_InitializeArchive(archive) >= 0) {
        return archive;
    }
    
    DXA_CloseArchive(archive);
    return NULL;
}

static int DXA_InitializeArchive(DXArchive *archive) {
    /* This function reads in all of the archive's static/indexable information,
     * and converts from the version of data within the original package.
     */
    DXArchiveHeader archiveHeader;
    int version;
    unsigned long long infoPosition;
    unsigned char *data;
    unsigned int codepage;
    
    /* - Read archive header. */
    if (DXA_ReadAndDecode(archive, 0, &archiveHeader, sizeof(archiveHeader)) < 0) {
        return -1;
    }
    
    if (archiveHeader.DXID != 0x5844) {
        return -1;
    }
    
    version = archiveHeader.Version;
    infoPosition = sizeof(archiveHeader);
    archive->Version = version;
    
    /* - Read archive tables into memory. */
    data = (unsigned char *)DXALLOC(archiveHeader.HeaderSize);
    if (version >= 6) {
        DXArchiveInfo archiveInfo;
        if (DXA_ReadAndDecode(archive, infoPosition, &archiveInfo, sizeof(archiveInfo)) < 0) {
            return -1;
        }
        
        DXA_ReadAndDecode(archive, archiveInfo.FileNameTableAddress, data, archiveHeader.HeaderSize);
        archive->DataBlob = data;
        
        codepage = (unsigned int)archiveInfo.CodePage;
    
        archive->DataAddress = archiveInfo.DataAddress;
        
        archive->FileNameInfoTable = archive->DataBlob;
        archive->FileInfoTable = archive->DataBlob + archiveInfo.FileTableAddress;
        archive->DirectoryInfoTable = archive->DataBlob + archiveInfo.DirectoryTableAddress;
    } else {
        DXArchiveInfoV5 archiveInfo;
        if (DXA_ReadAndDecode(archive, infoPosition, &archiveInfo, sizeof(archiveInfo)) < 0) {
            return -1;
        }
        
        DXA_ReadAndDecode(archive, archiveInfo.FileNameTableAddress, data, archiveHeader.HeaderSize);
        archive->DataBlob = data;
    
        codepage = archiveInfo.CodePage;
    
        archive->DataAddress = archiveInfo.DataAddress;
        
        archive->FileNameInfoTable = archive->DataBlob;
        archive->FileInfoTable = archive->DataBlob + archiveInfo.FileTableAddress;
        archive->DirectoryInfoTable = archive->DataBlob + archiveInfo.DirectoryTableAddress;
    }
    
    switch(codepage) {
#ifndef DXPORTLIB_NON_SJIS
        case 932: archive->CharSet = DX_CHARSET_SHFTJIS; break;
#endif /* #ifndef DXPORTLIB_NON_SJIS */
        case 65001: archive->CharSet = DX_CHARSET_EXT_UTF8; break;
        default: archive->CharSet = DX_CHARSET_DEFAULT; break;
    }
    
    return 0;
}

static void DXA_GetFileInfo(DXArchive *archive, unsigned long long address, DXArchiveFileInfo *info) {
    if (archive->Version >= 6) {
        DXArchiveFileInfo *src = (DXArchiveFileInfo *)(archive->FileInfoTable + address);
        
        *info = *src;
    } else if (archive->Version > 1) {
        DXArchiveFileInfoV5 *src = (DXArchiveFileInfoV5 *)(archive->FileInfoTable + address);
        
        info->NameAddress = src->NameAddress;
        info->Attributes = src->Attributes;
        
        info->CreationTime = src->CreationTime;
        info->LastAccessTime = src->LastAccessTime;
        info->LastWriteTime = src->LastWriteTime;
        
        info->DataAddress = src->DataAddress;
        info->DataSize = src->DataSize;
        info->CompressedDataSize = src->CompressedDataSize;
    } else {
        DXArchiveFileInfoV1 *src = (DXArchiveFileInfoV1 *)(archive->FileInfoTable + address);
        
        info->NameAddress = src->NameAddress;
        info->Attributes = src->Attributes;
        
        info->CreationTime = src->CreationTime;
        info->LastAccessTime = src->LastAccessTime;
        info->LastWriteTime = src->LastWriteTime;
        
        info->DataAddress = src->DataAddress;
        info->DataSize = src->DataSize;
        info->CompressedDataSize = 0xffffffff;
    }
}

static void DXA_GetDirectoryInfo(DXArchive *archive, unsigned long long address, DXArchiveDirectoryInfo *info) {
    if (archive->Version >= 6) {
        DXArchiveDirectoryInfo *src = (DXArchiveDirectoryInfo *)(archive->DirectoryInfoTable + address);
        
        *info = *src;
    } else {
        DXArchiveDirectoryInfoV5 *src = (DXArchiveDirectoryInfoV5 *)(archive->DirectoryInfoTable + address);
        info->DirectoryAddress = src->DirectoryAddress;
        info->ParentDirectoryAddress = src->ParentDirectoryAddress;
        info->FileInfoCount = src->FileInfoCount;
        info->FileInfoAddress = src->FileInfoAddress;
    }
}

static void DXA_GetFileNameInfo(DXArchive *archive, unsigned long long address, DXArchiveFileNameInfo *info, const char **dName) {
    DXArchiveFileNameInfo *src = (DXArchiveFileNameInfo *)(archive->FileNameInfoTable + address);
    
    if (info != NULL) {
        *info = *src;
    }
    
    if (dName != NULL) {
        *dName = (const char *)(archive->FileNameInfoTable + address + 4);
    }
}
    
static int DXA_ReadAndDecode(DXArchive *archive, unsigned long long position, void *dest, unsigned long long length) {
    SDL_RWops *rwops = archive->File;
    SDL_RWseek(rwops, (Sint64)position, RW_SEEK_SET);
    if (SDL_RWread(rwops, dest, (size_t)length, 1) < 1) {
        return -1;
    }
    
    DXA_Decode(archive, dest, dest, length, position);
    return 0;
}

static void DXA_Decode(DXArchive *archive, const void *vSrc, void *vDest, unsigned long long length, unsigned long long position) {
    unsigned char key[DXA_KEY_LENGTH];
    unsigned int *iKey = (unsigned int *)key;
    const unsigned char *src = (const unsigned char *)vSrc;
    unsigned char *dest = (unsigned char *)vDest;
    const unsigned int *iSrc = (const unsigned int *)src;
    unsigned int *iDest = (unsigned int *)dest;
    unsigned long long count = length / 12;
    unsigned int offset = (unsigned int)(position % DXA_KEY_LENGTH);
    unsigned long long i;
    unsigned char *srcKey = archive->Key;
    
    if (offset == 0) {
        for (i = 0; i < DXA_KEY_LENGTH; ++i) {
            key[i] = srcKey[i];
        }
    } else {
        int j = 0;
        for (i = offset; i < DXA_KEY_LENGTH; ++i, ++j) {
            key[j] = srcKey[i];
        }
        for (i = 0; i < offset; ++i, ++j) {
            key[j] = srcKey[i];
        }
    }
    
    for (i = count; i > 0; --i) {
        *iDest++ = *iSrc++ ^ iKey[0];
        *iDest++ = *iSrc++ ^ iKey[1];
        *iDest++ = *iSrc++ ^ iKey[2];
    }
    
    count = (length - (count * 12));
    src = (const unsigned char *)iSrc;
    dest = (unsigned char *)iDest;
    for (i = 0; i < count; ++i) {
        *dest++ = *src++ ^ key[i];
    }
}

void DXA_SetArchiveKey(DXArchive *archive, const char *keyString) {
    size_t len;
    unsigned char *key = archive->Key;
    if (keyString == NULL || (len = SDL_strlen(keyString)) == 0) {
        memset(key, 0xaa, 12);
    } else {
        size_t i;
        for (i = 0; i < 12; ++i) {
            key[i] = (unsigned char)keyString[i % len];
        }
    }
    
    /* Silly pointless key obfuscation algorithm. */
    key[ 0] = (unsigned char)(~key[0]);
    key[ 1] = (unsigned char)((key[1] >> 4) | (key[1] << 4));
    key[ 2] = (unsigned char)(key[2] ^ 0x8a);
    key[ 3] = (unsigned char)(~( (key[3] >> 4) | (key[3] << 4) ));
    key[ 4] = (unsigned char)(~key[4]);
    key[ 5] = (unsigned char)(key[5] ^ 0xac);
    key[ 6] = (unsigned char)(~key[6]);
    key[ 7] = (unsigned char)(~( (key[7] >> 3) | (key[7] << 5) ));
    key[ 8] = (unsigned char)((key[8] >> 5) | (key[8] << 3));
    key[ 9] = (unsigned char)(key[9] ^ 0x7f);
    key[10] = (unsigned char)(( (key[10] >> 4) | (key[10] << 4) ) ^ 0xd6);
    key[11] = (unsigned char)(key[11] ^ 0xcc);
}

void DXA_SetArchiveKeyRaw(DXArchive *archive, const unsigned char *key) {
    unsigned char *dKey = archive->Key;
    int i;
    
    for (i = 0; i < 12; ++i) {
        dKey[i] = key[i];
    }
}

static int DXA_Decompress(const void *vSrc, void *vDest, unsigned long long dest_len) {
    /* DXA LZ decompression. */
    const unsigned char *src = (const unsigned char *)vSrc;
    unsigned int src_len = *(unsigned int *)(src + 4);
    const unsigned char *src_end;
    unsigned char *dest = (unsigned char *)vDest;
    unsigned char *dest_end = dest + dest_len;
    unsigned char code = src[8];

    src += 9;
    src_end = src + src_len;

    while (src < src_end && dest < dest_end) {
        unsigned char control;
        unsigned int count;
        unsigned int d_index;
        
        /* - If this is not the control code, it's an uncompressed byte. */
        if (*src != code) {
            *dest++ = *src++;
            continue;
        }
        
        /* - Read the control byte. If so, it's a single uncompressed byte. */
        control = *(src + 1);
        src += 2;
        if (control == code) {
            *dest++ = code;
            continue;
        }
        
        /* - If the value is above the control code, subtract one. (this way it's always in range 0x00 - 0xfe) */
        if (control > code) {
            control--;
        }
        
        /* - Top 5 bits make up the count. */
        count = control >> 3;
        
        /* - If the 0x4 flag is set on the control byte, the next byte makes up the top 8 bits of the count. */
        if (control & 4) {
            count |= (unsigned int)(*src++ << 5);
        }
        
        /* - Read the dictionary index, using the bottom 2 control bits to determine size/type. */
        control &= 3;
        count += 4;
        
        d_index = 0;
        
        switch(control) {
            case 0:
                d_index = *src++;
                break;
            case 1:
                d_index = *((unsigned short *)src);
                src += 2;
                break;
            case 2:
                d_index = *((unsigned short *)src);
                src += 2;
                d_index |= (unsigned int)((*src++)<<16);
                break;
            default:
                d_index = *((unsigned int *)src);
                src += 4;
                break;
        }
        
        d_index += 1;
        
        /* - Copy from dictionary position. */
        if (d_index < count) {
            unsigned char *a = dest - d_index;
            while (count-- > 0) {
                *dest++ = *a++;
            }
        } else {
            memcpy(dest, dest - d_index, count);
            
            dest += count;
        }
    }
    
    return 0;
}

/* ------------------------------------------------------------ DXARCHIVE RWOPS STREAMING */

/* The main DXA Stream opens a separate file descriptor and reads the data in incrementally.
 * Yes, we nest RWops here, using SDL's RWops for actual file I/O.
 */
typedef struct DXAStreamRWops {
    SDL_RWops rwops;
    
    DXArchive *archive;
    
    SDL_RWops *src;
    
    Sint64 fileStartPosition;
    
    Sint64 size;
    Sint64 currentPosition;
} DXAStreamRWops;

static Sint64 SDLCALL DXA_Stream_Size(SDL_RWops *context) {
    DXAStreamRWops *dxaops = (DXAStreamRWops *)context;
    
    return dxaops->size;
}

static Sint64 SDLCALL DXA_Stream_Seek(SDL_RWops *context, Sint64 offset, int whence) {
    DXAStreamRWops *dxaops = (DXAStreamRWops *)context;
    
    switch(whence) {
        case RW_SEEK_SET:
            dxaops->currentPosition = offset;
            break;
        case RW_SEEK_CUR:
            dxaops->currentPosition += offset;
            break;
        case RW_SEEK_END:
            dxaops->currentPosition = dxaops->size + offset;
            break;
    }
    
    if (dxaops->currentPosition < 0) {
        dxaops->currentPosition = 0;
    }
    if (dxaops->currentPosition > dxaops->size) {
        dxaops->currentPosition = dxaops->size;
    }
    
    return dxaops->currentPosition;
}

static size_t SDLCALL DXA_Stream_Read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum) {
    DXAStreamRWops *dxaops = (DXAStreamRWops *)context;
    Sint64 position;
    size_t remaining;
    size_t num;
    size_t total_size = size * maxnum;
    
    if (size <= 0 || maxnum <= 0 || (size_t)(total_size / maxnum) != size) {
        return 0;
    }
    
    remaining = (size_t)(dxaops->size - dxaops->currentPosition);
    if (total_size > remaining) {
        total_size = remaining;
        if ((maxnum * size) > total_size) {
            maxnum = total_size / size;
            if (maxnum == 0) {
                return 0;
            }
        }
    }
    
    position = dxaops->fileStartPosition + dxaops->currentPosition;
    SDL_RWseek(dxaops->src, position, RW_SEEK_SET);
    num = SDL_RWread(dxaops->src, ptr, size, maxnum);
    
    if (num > 0) {
        DXA_Decode(dxaops->archive, ptr, ptr, num * size, (unsigned long long)position);
    }
    dxaops->currentPosition += num * size;
    
    return num;
} 

static size_t SDLCALL DXA_Stream_DisableWrite(SDL_RWops *context, const void *ptr, size_t size, size_t num) {
    return 0; /* writing is not supported. */
}

static int SDLCALL DXA_Stream_Close(SDL_RWops *context) {
    if (context != NULL) {
        DXAStreamRWops *dxaops = (DXAStreamRWops *)context;
        
        SDL_RWclose(dxaops->src);
        
        DXFREE(dxaops);
    }
    return 0;
}

static SDL_RWops *DXA_Stream_Open(DXArchive *archive, DXArchiveFileInfo *fileInfo) {
    SDL_RWops *src = SDL_RWFromFile(archive->utf8Filename, "rb");
    DXAStreamRWops *dxaops;
    
    if (src == NULL) {
        return NULL;
    }
    
    dxaops = (DXAStreamRWops *)DXALLOC(sizeof(DXAStreamRWops));
    
    dxaops->rwops.size = DXA_Stream_Size;
    dxaops->rwops.seek = DXA_Stream_Seek;
    dxaops->rwops.read = DXA_Stream_Read;
    dxaops->rwops.write = DXA_Stream_DisableWrite;
    dxaops->rwops.close = DXA_Stream_Close;
    
    dxaops->rwops.type = SDL_RWOPS_UNKNOWN;
    
    dxaops->archive = archive;
    dxaops->src = src;
    dxaops->fileStartPosition = (Sint64)(fileInfo->DataAddress + archive->DataAddress);
    dxaops->size = (Sint64)fileInfo->DataSize;
    dxaops->currentPosition = 0;
    
    return &dxaops->rwops;
}

/* The DXA memory stream is basically the same as SDL_FromConstMem(),
 * except we free the memory once completed.
 */
typedef struct DXAMemStreamRWops {
    SDL_RWops rwops;
    
    DXArchive *archive;
    
    int freeData;
    unsigned char *data;
    
    size_t size;
    size_t currentPosition;
} DXAMemStreamRWops;

static Sint64 SDLCALL DXA_MemStream_Size(SDL_RWops *context) {
    DXAMemStreamRWops *memstream = (DXAMemStreamRWops *)context;
    return memstream->size;
}

static Sint64 SDLCALL DXA_MemStream_Seek(SDL_RWops *context, Sint64 offset, int whence) {
    DXAMemStreamRWops *memstream = (DXAMemStreamRWops *)context;
    
    switch(whence) {
        case RW_SEEK_SET:
            memstream->currentPosition = (size_t)offset;
            break;
        case RW_SEEK_CUR:
            memstream->currentPosition += (size_t)offset;
            break;
        case RW_SEEK_END:
            memstream->currentPosition = memstream->size + (size_t)offset;
            break;
    }
    if (memstream->currentPosition < 0) {
        memstream->currentPosition = 0;
    }
    if (memstream->currentPosition > memstream->size) {
        memstream->currentPosition = memstream->size;
    }
    
    return memstream->currentPosition; /* return current position */
}


static size_t SDLCALL DXA_MemStream_Read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum) {
    DXAMemStreamRWops *memstream = (DXAMemStreamRWops *)context;
    size_t remaining;
    
    size_t total_size = size * maxnum;
    if (size <= 0 || maxnum <= 0 || (size_t)(total_size / maxnum) != size) {
        return 0;
    }
    
    remaining = memstream->size - memstream->currentPosition;
    if (total_size > remaining) {
        total_size = remaining;
    }
    
    SDL_memcpy(ptr, memstream->data + memstream->currentPosition, total_size);
    
    memstream->currentPosition += total_size;
    
    return total_size / size; /* return number of objects read */
} 

static size_t SDLCALL DXA_MemStream_DisableWrite(SDL_RWops *context, const void *ptr, size_t size, size_t num) {
    return 0; /* writing is not supported. */
}

static int SDLCALL DXA_MemStream_Close(SDL_RWops *context) {
    /* Delete data */
    if (context != NULL) {
        DXAMemStreamRWops *memstream = (DXAMemStreamRWops *)context;
    
        if (memstream->freeData) {
            DXFREE(memstream->data);
        }
        DXFREE(memstream);
    }
    return 0;
}

static SDL_RWops *DXA_MemStream_Open(unsigned char *data, size_t length, int freeData) {
    DXAMemStreamRWops *memstream = (DXAMemStreamRWops *)DXALLOC(sizeof(DXAMemStreamRWops));
    
    memstream->rwops.size = DXA_MemStream_Size;
    memstream->rwops.seek = DXA_MemStream_Seek;
    memstream->rwops.read = DXA_MemStream_Read;
    memstream->rwops.write = DXA_MemStream_DisableWrite;
    memstream->rwops.close = DXA_MemStream_Close;
    
    memstream->rwops.type = SDL_RWOPS_UNKNOWN;
    memstream->data = data;
    memstream->size = length;
    memstream->currentPosition = 0;
    memstream->freeData = freeData;
    
    return &memstream->rwops;
}

SDL_RWops *DXA_OpenStream(DXArchive *archive, const DXCHAR *filename) {
    unsigned long long fileAddress = DXA_GetFileAddress(archive, filename);
    DXArchiveFileInfo fileInfo;
    
    if (fileAddress == 0) {
        return NULL;
    }
    
    DXA_GetFileInfo(archive, fileAddress, &fileInfo);
    if (fileInfo.CompressedDataSize == 0xffffffff) {
        if (archive->PreloadData != NULL) {
            unsigned long long address = archive->DataAddress + fileInfo.DataAddress;
            if ((address + fileInfo.DataSize) > archive->PreloadSize) {
                return NULL;
            }
            return DXA_MemStream_Open(archive->PreloadData + address, (size_t)fileInfo.DataSize, DXFALSE);
        }
        return DXA_Stream_Open(archive, &fileInfo);
    } else {
        /* Compressed data streaming is not supported at this time, so just load the whole thing
         * and return a memory stream.
         */
        unsigned char *decompressed;
        unsigned int decompressedSize;
        
        if (DXA_ReadCompressedFile(archive, &fileInfo, &decompressed, &decompressedSize) >= 0) {
            return DXA_MemStream_Open(decompressed, decompressedSize, DXTRUE);
        }
        
        return NULL;
    }
}

#endif /* #ifndef DX_NOT_DXA */

#endif /* #ifdef DXPORTLIB_DXLIB_INTERFACE */
