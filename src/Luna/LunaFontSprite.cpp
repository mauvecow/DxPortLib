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

/* So FontSprite is completely specific to SJIS, to an absurd level.
 * This means that we can't use it without CP932 support compiled in.
 */

enum {
    CODE_ASCII_START = 0x0000,
    CODE_ASCII_END = 0x00ff,
    CODE_SJIS_A_START = 0x8140,
    CODE_SJIS_A_END = 0x9fff,
    CODE_SJIS_B_START = 0xe040,
    CODE_SJIS_B_END = 0xeeff,
    CODE_SJIS_C_START = 0xf8a0,
    CODE_SJIS_C_END = 0xfc4f,
    
    CODE_TABLE_SIZE =
        (CODE_ASCII_END - CODE_ASCII_START) +
        (CODE_SJIS_A_END - CODE_SJIS_A_START) +
        (CODE_SJIS_B_END - CODE_SJIS_B_START) +
        (CODE_SJIS_C_END - CODE_SJIS_C_START)
};

typedef struct _LFDHeader {
    char id[4];
    Uint8 fontSize;
    Uint8 sheetCount;
    Uint16 sheetWidth;
    Uint16 sheetHeight;
    Uint16 fontMax;
} LFDHeader;

typedef struct _LFDCharEntry {
    Sint8 sheetNo;
    Uint8 xOffset;
    Uint8 pAdvance;
    Uint8 unused;
    Uint16 x1;
    Uint16 y1;
    Uint16 x2;
    Uint16 y2;
} LFDCharEntry;

typedef struct _LunaFontSprData {
    /* Instead of breaking up the lfd data, we just
     * store it in memory whole. */
    unsigned char *lfdData;
    
    const LFDHeader *lfdHeader;
    const Uint16 *lfdIndex;
    const LFDCharEntry *lfdCharEntries;
    
    unsigned int charMax;
    
    int *sheetGraphs;
    int *sheetSprites;
    int sheetCount;
    
    int space;
} LunaFontSprData;

static const char lfd_id[4] = { 'L', 'F', 'D', 0x00 };

LFONTSPRITE LunaFontSprite::CreateFromFile(const char *pFileName,
                                 const char *pExt, Bool IsAlpha, Uint32 Num,
                                 Bool IsSortZ,
                                 eSurfaceFormat Format) {
    char filebuf[4096];
    int file = PL_File_OpenRead(
        PL_Text_ConvertStrncpyIfNecessary(
            filebuf, -1, pFileName, g_lunaUseCharSet, 4096)
    );
    unsigned char *lfdData = NULL;
    
    do {
        int64_t fileSize = PL_File_GetSize(file);
        const LFDHeader *lfdHeader;
        LunaFontSprData *fontspr;
        int fontSprHandle;
        int fontBaseSize;
        int i;
        
        /* don't allow files beyond a certain size */
        if (file < 0 || fileSize <= 0 || fileSize > 65536) {
            break;
        }
        
        lfdData = (unsigned char *)DXALLOC((int)fileSize);
        if (PL_File_Read(file, lfdData, (int)fileSize) != (int)fileSize) {
            break;
        }
        PL_File_Close(file);
        file = -1;
        
        /* sanity checks to verify LFD data is valid */
        lfdHeader = (const LFDHeader *)lfdData;
        if (memcmp(lfdHeader, lfd_id, 4) != 0 || lfdHeader->sheetCount == 0 || lfdHeader->sheetCount > 32) {
            break;
        }
        
        fontBaseSize = sizeof(LFDHeader) + (sizeof(Uint16) * CODE_TABLE_SIZE) + (32 * lfdHeader->sheetCount);
        if (fileSize < (int64_t)(fontBaseSize + (sizeof(LFDCharEntry) * lfdHeader->fontMax))) {
            break;
        }
        
        /* create sprite handle and data */
        fontSprHandle = PL_Handle_AcquireID(DXHANDLE_LUNAFONTSPRITE);
        if (fontSprHandle < 0) {
            break;
        }
        
        fontspr = (LunaFontSprData *)PL_Handle_AllocateData(fontSprHandle, sizeof(LunaFontSprData));
        fontspr->lfdData = lfdData;
        fontspr->lfdHeader = lfdHeader;
        fontspr->lfdIndex = (const Uint16 *)(lfdData + sizeof(LFDHeader));
        fontspr->lfdCharEntries = (const LFDCharEntry *)(lfdData + fontBaseSize);
        fontspr->charMax = lfdHeader->fontMax;
        
        fontspr->sheetCount = lfdHeader->sheetCount;
        fontspr->sheetGraphs = (int *)DXALLOC(sizeof(int) * fontspr->sheetCount);
        fontspr->sheetSprites = (int *)DXALLOC(sizeof(int) * fontspr->sheetCount);
        
        fontspr->space = 0;
        
        for (i = 0; i < fontspr->sheetCount; ++i) {
            char buf[64];
            memcpy(buf, lfdData + sizeof(LFDHeader) + (sizeof(Uint16) * CODE_TABLE_SIZE) + (32 * i), 32);
            buf[32] = '\0';
            if (pExt != NULL) {
                char newBuf[2048];
                PL_Text_ConvertStrncpy(
                    newBuf, -1, buf, g_lunaUseCharSet, 2048);
                PL_Text_ConvertStrncat(
                    newBuf, -1, pExt, g_lunaUseCharSet, 2048);
                
                int surface = PL_Surface_Load(newBuf);
                fontspr->sheetGraphs[i] = PL_Surface_ToTexture(surface);
                PL_Surface_Delete(surface);
            } else {
                /* FIXME: LAG support */
                fontspr->sheetGraphs[i] = -1;
            }
            fontspr->sheetSprites[i] =
                LunaSprite::Create(Num, PRIM_VERTEX_UV1, IsSortZ);
            
            LunaSprite::AttatchTexture(fontspr->sheetSprites[i], 0, fontspr->sheetGraphs[i]);
        }
        
        return fontSprHandle;
    } while(0);
    
    PL_File_Close(file);
    if (lfdData != NULL) {
        DXFREE(lfdData);
    }
        
    return INVALID_FONTSPRITE;
}

void LunaFontSprite::Release(LFONTSPRITE lFontSpr) {
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int i;
        if (fontspr->sheetGraphs != NULL) {
            for (i = 0; i < fontspr->sheetCount; ++i) {
                PLG.Texture_Release(fontspr->sheetGraphs[i]);
            }
            DXFREE(fontspr->sheetGraphs);
        }
        if (fontspr->sheetSprites != NULL) {
            for (i = 0; i < fontspr->sheetCount; ++i) {
                LunaSprite::Release(fontspr->sheetSprites[i]);
            }
            DXFREE(fontspr->sheetSprites);
        }
        DXFREE(fontspr->lfdData);
        
        PL_Handle_ReleaseID(lFontSpr, DXTRUE);
    }
}

static unsigned int s_CharToLookup(unsigned int ch) {
#ifndef DXPORTLIB_NO_SJIS
    unsigned int s = PL_Text_ToSJIS(ch);
    
    if (s <= CODE_ASCII_END) {
        return s;
    }
    
    if (s >= CODE_SJIS_A_START && s <= CODE_SJIS_A_END) {
        return s - (CODE_SJIS_A_START
                     - (CODE_ASCII_END - CODE_ASCII_START));
    }
    if (s >= CODE_SJIS_B_START && s <= CODE_SJIS_B_END) {
        return s - (CODE_SJIS_B_START
                     - (CODE_ASCII_END - CODE_ASCII_START)
                     - (CODE_SJIS_A_END - CODE_SJIS_A_START));
    }
    if (s >= CODE_SJIS_C_START && s <= CODE_SJIS_C_END) {
        return s - (CODE_SJIS_C_START
                     - (CODE_ASCII_END - CODE_ASCII_START)
                     - (CODE_SJIS_A_END - CODE_SJIS_A_START)
                     - (CODE_SJIS_B_END - CODE_SJIS_B_START));
    }
    return 0;
#else
    if (ch <= CODE_ASCII_END) {
        return ch;
    }
    return 0;
#endif
}

static unsigned int s_CharToIndex(LunaFontSprData *fontspr, unsigned int ch) {
    unsigned int lookup = s_CharToLookup(ch);
    return fontspr->lfdIndex[lookup];
}

void LunaFontSprite::DrawString(LFONTSPRITE lFontSpr, const char *pStr,
                          Sint32 Px, Sint32 Py, Float Pz, D3DCOLOR Color) {
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        unsigned int ch;
        unsigned int charMax = fontspr->charMax;
        float x = F(Px);
        float y = F(Py);
        float z = F(Pz);
        float fontSize = F(fontspr->lfdHeader->fontSize);
        float spacing = F(fontSize + fontspr->space);
        int sheetCount = fontspr->sheetCount;
        int charset = g_lunaUseCharSet;
        while ((ch = PL_Text_ReadChar(&pStr, charset)) != 0) {
            if (ch == '\n' || ch == '\r') {
                x = F(Px);
                y += fontSize;
            } else if (ch == '\t') {
                x += spacing * 4;
            } else if (ch == '\b') {
                x -= spacing;
            } else if (ch == ' ') {
                x += spacing * 0.5f;
            } else {
                unsigned int index = s_CharToIndex(fontspr, ch);
                if (index < charMax) {
                    const LFDCharEntry *entry = &fontspr->lfdCharEntries[index];
                    if (entry->sheetNo >= 0 && entry->sheetNo < sheetCount) {
                        CLunaRect dst, src;
                        float w = F(entry->x2 - entry->x1);
                        float h = F(entry->y2 - entry->y1);
                        dst.Px = x + F(entry->xOffset); dst.Py = y;
                        dst.Sx = w; dst.Sy = h;
                        src.Px = F(entry->x1); src.Py = F(entry->y1);
                        src.Sx = w; src.Sy = h;
                    
                        LunaSprite::DrawSquare(
                            fontspr->sheetSprites[entry->sheetNo],
                            &dst, z, &src, Color, 1);
                        
                        if (ch < 0xff) {
                            x += spacing * 0.5f;
                        } else {
                            x += spacing;
                        }
                    } else {
                        x += fontspr->space;
                    }
                }
            }
        }
    }
}

void LunaFontSprite::DrawStringP(LFONTSPRITE lFontSpr, const char *pStr,
                          Sint32 Px, Sint32 Py, Float Pz, D3DCOLOR Color) {
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        unsigned int ch;
        unsigned int charMax = fontspr->charMax;
        float x = F(Px);
        float y = F(Py);
        float z = F(Pz);
        float fontSize = F(fontspr->lfdHeader->fontSize);
        float spacing = F(fontSize + fontspr->space);
        int sheetCount = fontspr->sheetCount;
        int charset = g_lunaUseCharSet;
        while ((ch = PL_Text_ReadChar(&pStr, charset)) != 0) {
            if (ch == '\n' || ch == '\r') {
                x = F(Px);
                y += fontSize;
            } else if (ch == '\t') {
                x += spacing * 4;
            } else if (ch == '\b') {
                x -= spacing;
            } else if (ch == ' ') {
                x += spacing * 0.5f;
            } else {
                unsigned int index = s_CharToIndex(fontspr, ch);
                if (index < charMax) {
                    const LFDCharEntry *entry = &fontspr->lfdCharEntries[index];
                    if (entry->sheetNo >= 0 && entry->sheetNo < sheetCount) {
                        CLunaRect dst, src;
                        float w = F(entry->x2 - entry->x1);
                        float h = F(entry->y2 - entry->y1);
                        dst.Px = x + 1; dst.Py = y;
                        dst.Sx = w; dst.Sy = h;
                        src.Px = F(entry->x1); src.Py = F(entry->y1);
                        src.Sx = w; src.Sy = h;
                    
                        LunaSprite::DrawSquare(
                            fontspr->sheetSprites[entry->sheetNo],
                            &dst, z, &src, Color, 1);
                        
                        x += F(entry->pAdvance + 1);
                    } else {
                        x += fontSize * 0.5f;
                    }
                }
            }
        }
    }
}

void LunaFontSprite::ResetBuffer(LFONTSPRITE lFontSpr, Sint32 Space) {
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int i;
        for (i = 0; i < fontspr->sheetCount; ++i) {
            LunaSprite::ResetBuffer(fontspr->sheetSprites[i]);
        }
        
        fontspr->space = Space;
    }
}

void LunaFontSprite::UpdateBuffer(LFONTSPRITE lFontSpr) {
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int i;
        for (i = 0; i < fontspr->sheetCount; ++i) {
            LunaSprite::UpdateBuffer(fontspr->sheetSprites[i]);
        }
    }
}

void LunaFontSprite::Rendering(LFONTSPRITE lFontSpr) {
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int i;
        for (i = 0; i < fontspr->sheetCount; ++i) {
            LunaSprite::Rendering(fontspr->sheetSprites[i]);
        }
    }
}

Bool LunaFontSprite::GetWidth(LFONTSPRITE lFontSpr, const char *pStr,
                         Sint32 *pLeft, Sint32 *pCenter, Sint32 *pRight) {
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int ch = PL_Text_ReadChar(&pStr, g_lunaUseCharSet);
        if (ch != 0) {
            unsigned int index = s_CharToIndex(fontspr, ch);
            if (index < fontspr->charMax) {
                const LFDCharEntry *entry = &fontspr->lfdCharEntries[index];
                if (pLeft != NULL) {
                    *pLeft = entry->xOffset;
                }
                if (pCenter != NULL) {
                    *pCenter = entry->pAdvance;
                }
                if (pRight != NULL) {
                    *pRight = entry->unused;
                }
                return true;
            }
        }
    }
    return false;
}

Sint32 LunaFontSprite::EXTGetCharaSize(LFONTSPRITE lFontSpr, const char *pStr,
                      Sint32 *w, Sint32 *h)
{
    // barf
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr == NULL) {
        return 0;
    }
    
    int ch = PL_Text_ReadChar(&pStr, g_lunaUseCharSet);
    if (ch == 0) {
        return 0;
    }
    
    unsigned int index = s_CharToIndex(fontspr, ch);
    if (index >= fontspr->charMax) {
        return 0;
    }
    
    const LFDCharEntry *entry = &fontspr->lfdCharEntries[index];
    *w = entry->x2 - entry->x1;
    *h = entry->y2 - entry->y1;
    
    if (ch < 256 || (ch >= 0xff00 && ch < 0xffff)) {
        return 1; // Half-width
    }
    return 2; // Full-width
}

void LunaFontSprite::DrawChara(LFONTSPRITE lFontSpr, const char *pStr,
                    CLunaRect *pDst, Float Pz, D3DCOLOR Color)
{
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int ch = PL_Text_ReadChar(&pStr, g_lunaUseCharSet);
        if (ch != 0) {
            unsigned int index = s_CharToIndex(fontspr, ch);
            if (index < fontspr->charMax) {
                const LFDCharEntry *entry = &fontspr->lfdCharEntries[index];
                if (entry->sheetNo >= 0 && entry->sheetNo < sheetCount) {
                    CLunaRect src;
                    float w = F(entry->x2 - entry->x1);
                    float h = F(entry->y2 - entry->y1);
                    src.Px = F(entry->x1); src.Py = F(entry->y1);
                    src.Sx = w; src.Sy = h;
                    
                    LunaSprite::DrawSquare(
                        fontspr->sheetSprites[entry->sheetNo],
                        &dst, z, &src, Color,
                        1);
                }
            }
        }
    }
    
    return 0;
}

void LunaFontSprite::DrawCharaRotate(LFONTSPRITE lFontSpr, const char *pStr,
                    CLunaRect *pDst, Float Pz, D3DCOLOR Color,
                    Sint32 Angle);
{
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int ch = PL_Text_ReadChar(&pStr, g_lunaUseCharSet);
        if (ch != 0) {
            unsigned int index = s_CharToIndex(fontspr, ch);
            if (index < fontspr->charMax) {
                const LFDCharEntry *entry = &fontspr->lfdCharEntries[index];
                if (entry->sheetNo >= 0 && entry->sheetNo < sheetCount) {
                    CLunaRect src;
                    float w = F(entry->x2 - entry->x1);
                    float h = F(entry->y2 - entry->y1);
                    src.Px = F(entry->x1); src.Py = F(entry->y1);
                    src.Sx = w; src.Sy = h;
                    
                    LunaSprite::DrawSquare(
                        fontspr->sheetSprites[entry->sheetNo],
                        &dst, z, &src, Color,
                        Angle, 1);
                }
            }
        }
    }
    
    return 0;
}

void LunaFontSprite::DrawCharaRotateXYZ(LFONTSPRITE lFontSpr, const char *pStr,
                    CLunaRect *pDst, Float Pz, D3DCOLOR Color,
                    Sint32 AngleX, Sint32 AngleY, Sint32 AngleZ);
{
    LunaFontSprData *fontspr = (LunaFontSprData *)PL_Handle_GetData((int)lFontSpr, DXHANDLE_LUNAFONTSPRITE);
    if (fontspr != NULL) {
        int ch = PL_Text_ReadChar(&pStr, g_lunaUseCharSet);
        if (ch != 0) {
            unsigned int index = s_CharToIndex(fontspr, ch);
            if (index < fontspr->charMax) {
                const LFDCharEntry *entry = &fontspr->lfdCharEntries[index];
                if (entry->sheetNo >= 0 && entry->sheetNo < sheetCount) {
                    CLunaRect src;
                    float w = F(entry->x2 - entry->x1);
                    float h = F(entry->y2 - entry->y1);
                    src.Px = F(entry->x1); src.Py = F(entry->y1);
                    src.Sx = w; src.Sy = h;
                    
                    LunaSprite::DrawSquareXYZ(
                        fontspr->sheetSprites[entry->sheetNo],
                        &dst, z, &src, Color,
                        AngleX, AngleY, AngleZ, 1);
                }
            }
        }
    }
    
    return 0;
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
