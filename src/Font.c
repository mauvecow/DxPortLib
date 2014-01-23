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

#include "SDL_ttf.h"

#ifndef DX_NON_FONT

/* DxLib itself uses Windows font functions for most of its work.
 * 
 * This is obviously not something we can support on other platforms,
 * so the font code needs to have a number of extensions in place
 * to support loading of font files and setting of properties.
 */

/*
 * The general structure here is as follows:
 * 
 * - Mappings exist to give names to ttf files.
 * - Font handles are instances of individual font files.
 * - Every font instance maintains a glyph texture and a glyph cache.
 * - As glyphs are cached, they are drawn onto the glyph texture
 *   dynamically.
 * - Extraneous edging of glyphs is stored as a separate glyph on the
 *   texture.
 * - If the glyph texture runs out of room, it is automatically grown
 *   and all glyphs are redrawn onto the new texture.
 * - The "default" font handle has a tiny system to update it on
 *   demand.
 * 
 * The only features that exist in DxLib that are not currently supported:
 * - ANTIALIASING_4x4 and ANTIALIASING_8x8 are not supported.
 * - Vertical drawing.
 */

/* ---------------------------------------------------------- DATA TYPES */

typedef struct FontMapping {
    DXCHAR *filename;
    int directFileAccessOnly;
    
    DXCHAR *fontname;
    
    int thickness;
    int boldFlag;
    
    struct FontMapping *next;
} FontMapping;

typedef struct GlyphRect {
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
} GlyphRect;

/* 20 bytes/glyph */
typedef struct GlyphData {
    unsigned int glyphID;
    
    GlyphRect rect;
    GlyphRect edgeRect;
    
    short xOffset;
    short advance;
    unsigned int nextGlyph;
} GlyphData;

typedef struct GlyphTexture {
    int textureID;
    int graphID;
    
    int width;
    int height;
    
    int Y;
    int X;
    int NextY;
} GlyphTexture;

typedef struct FontData {
    TTF_Font *font;
    int ptSize;
    int fontType;
    int edgeSize;
    int boldFlag;
    
    int spacing;
    
    int charset;
    
    GlyphTexture glyphTexture;
    
    GlyphData *glyphData;
    unsigned int glyphDataSize;
    unsigned int glyphDataCount;
    unsigned int glyphHash[256];
    unsigned int glyphTotal;
} FontData;

typedef struct DefaultFontInfo {
    DXCHAR *name;
    int charSet;
    int fontSize;
    int fontType;
    int fontSpacing;
    int fontThickness;
    int edgeSize;
} DefaultFontInfo;

static int s_fontInitialized = DXFALSE;
static FontMapping *s_fontMappings = NULL;
static SDL_PixelFormat *s_pixelFormat = 0;

static int s_defaultFontHandle = -1;
static DefaultFontInfo s_defaultFontInfo = {
    NULL,
    DX_CHARSET_DEFAULT,
    22,
    DX_FONTTYPE_NORMAL,
    0,
    6,
    -1
};
static int s_defaultFontRefreshFlag = DXTRUE;

/* ------------------------------------------------------- FONT MAPPINGS */

static void s_Initialize() {
    if (s_fontInitialized == DXTRUE) {
        return;
    }
    
    s_pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    s_defaultFontHandle = -1;
    
    TTF_Init();
    s_fontMappings = NULL;
    
    s_fontInitialized = DXTRUE;
}

static FontData *s_GetFontData(int ID) {
    return (FontData *)PL_Handle_GetData(ID, DXHANDLE_FONT);
}

static int s_AllocateFontDataID(
    TTF_Font *font, FontMapping *mapping,
    int ptSize
) {
    int fontDataID = PL_Handle_AcquireID(DXHANDLE_FONT);
    FontData *fontData;
    int i;
    
    if (fontDataID < 0) {
        return -1;
    }
    
    fontData = (FontData *)PL_Handle_AllocateData(fontDataID, sizeof(FontData));
    fontData->font = font;
    
    fontData->ptSize = ptSize;
    fontData->edgeSize = 1;
    fontData->boldFlag = mapping->boldFlag;
    fontData->spacing = 0;
    
    fontData->glyphTexture.textureID = -1;
    fontData->glyphTexture.graphID = -1;
    fontData->glyphTexture.width = 0;
    fontData->glyphTexture.height = 0;
    fontData->glyphTexture.X = 0;
    fontData->glyphTexture.Y = 0;
    fontData->glyphTexture.NextY = 0;
    
    fontData->glyphData = 0;
    fontData->glyphDataSize = 0;
    fontData->glyphDataCount = 0;
    for (i = 0; i < 256; ++i) {
        fontData->glyphHash[i] = 0xffff;
    }
    fontData->glyphTotal = 0;
    
    return fontDataID;
}

static int s_LoadFontRW(
    SDL_RWops *rwops, FontMapping *mapping,
    int size
) {
    TTF_Font *font;
    
    if (rwops == NULL) {
        return -1;
    }
    
    s_Initialize();
    
    font = TTF_OpenFontRW(rwops, 1, size);
    if (font != NULL) {
        return s_AllocateFontDataID(font, mapping, size);
    }
    
    return -1;
}

static int s_LoadFontFile(
    const DXCHAR *filename, FontMapping *mapping,
    int size
) {
    return s_LoadFontRW(PL_File_OpenStream(filename), mapping, size);
}

static int s_LoadFontFileDirect(
    const DXCHAR *filename, FontMapping *mapping,
    int size
) {
    return s_LoadFontRW(PL_File_OpenDirectStream(filename), mapping, size);
}


/* Given a font filename, maps it to a font name and weight/fonttype.
 * 
 * This will not load the font file, because we're just creating
 * a mapping for later use.
 */
int PLEXT_Font_MapFontFileToName(
    const DXCHAR *filename,
    const DXCHAR *fontname,
    int thickness,
    int boldFlag
) {
    FontMapping *mapping;
    
    s_Initialize();
    
    mapping = DXALLOC(sizeof(FontMapping));
    
    mapping->filename = DXSTRDUP(filename);
    mapping->fontname = DXSTRDUP(fontname);
    mapping->directFileAccessOnly = !PL_File_GetUseDXArchiveFlag();
    mapping->thickness = thickness;
    mapping->boldFlag = boldFlag;
    mapping->next = s_fontMappings;
    
    s_fontMappings = mapping;
    
    return 0;
}

int PLEXT_Font_InitFontMappings() {
    FontMapping *mapping;
    
    mapping = s_fontMappings;
    while (mapping != NULL) {
        FontMapping *nextMapping = mapping->next;
        DXFREE(mapping->filename);
        DXFREE(mapping->fontname);
        DXFREE(mapping);
        mapping = nextMapping;
    }
    s_fontMappings = NULL;
    
    return 0;
}

/* ---------------------------------------------------------- GLYPH CACHE */
/* DxLib is really too smart for its own good about this, and has
 * a full glyph cache for its glyphs, that is dynamically updated
 * as characters are recognized.
 * 
 * Unlike DxLib we keep a much smaller set of data for glyph information,
 * with a 256 entry hash table and no limit on available glyphs in existence.
 */

static void s_AddGlyphToTexture(FontData *fontData, GlyphData *glyph);

static GlyphData *s_GetGlyph(FontData *fontData, unsigned int glyphID) {
    unsigned int s = fontData->glyphHash[glyphID & 0xff];
    while (s != 0xffff) {
        GlyphData *glyph = &fontData->glyphData[s];
        if (glyph->glyphID == glyphID) {
            return glyph;
        }
        
        s = glyph->nextGlyph;
    }
    
    return NULL;
}

static GlyphData *s_AllocateGlyph(FontData *fontData, unsigned int glyphID) {
    GlyphData *glyph;
    unsigned int glyphSlot;
    
    if (fontData->glyphDataCount == fontData->glyphDataSize) {
        fontData->glyphDataSize += 1024;
        if (fontData->glyphData == NULL) {
            fontData->glyphData = DXALLOC(sizeof(GlyphData) * fontData->glyphDataSize);
        } else {
            fontData->glyphData = DXREALLOC(fontData->glyphData,
                                            sizeof(GlyphData) * fontData->glyphDataSize);
        }
    }
    
    glyphSlot = fontData->glyphDataCount;
    fontData->glyphDataCount += 1;
    
    glyph = &fontData->glyphData[glyphSlot];
    
    glyph->advance = 0;
    glyph->glyphID = glyphID;
    glyph->nextGlyph = fontData->glyphHash[glyphID & 0xff];
    fontData->glyphHash[glyphID & 0xff] = glyphSlot;
    
    fontData->glyphTotal += 1;
    
    glyph->xOffset = 0;
    
    return glyph;
}

static void s_GrowGlyphTexture(FontData *fontData) {
    static int nestCount = 0;
    GlyphTexture *glyphTexture = &fontData->glyphTexture;
    unsigned int glyphsLeft;
    SDL_Rect texRect;
    int nest;
    int i;
    
    if (glyphTexture->graphID < 0) {
        glyphTexture->width = 256;
        glyphTexture->height = 256;
    } else {
        if (glyphTexture->width <= glyphTexture->height) {
            glyphTexture->width *= 2;
        } else {
            glyphTexture->height *= 2;
        }
        
        PL_Texture_Release(glyphTexture->textureID);
        PL_Graph_Delete(glyphTexture->graphID);
    }
    
    glyphTexture->textureID = PL_Texture_CreateFromDimensions(
            glyphTexture->width,
            glyphTexture->height
        );
    
    PL_Texture_AddRef(glyphTexture->textureID);
    
    texRect.x = 0;
    texRect.y = 0;
    texRect.w = glyphTexture->width;
    texRect.h = glyphTexture->height;
    
    glyphTexture->graphID = PL_Graph_FromTexture(
        glyphTexture->textureID, texRect
        );
    
    glyphTexture->X = 0;
    glyphTexture->Y = 0;
    glyphTexture->NextY = 0;
    
    /* Redo all glyphs. */
    glyphsLeft = fontData->glyphTotal;
    nestCount += 1;
    nest = nestCount;
    for (i = 0; i < 256 && glyphsLeft > 0; ++i) {
        unsigned int glyphID = fontData->glyphHash[i];
        
        while (glyphID != 65535) {
            GlyphData *glyph = &fontData->glyphData[glyphID];
            
            s_AddGlyphToTexture(fontData, glyph);
            
            if (nest != nestCount) {
                return;
            }
            
            glyphsLeft -= 1;
            
            glyphID = glyph->nextGlyph;
        }
    }
}

static const unsigned char s_fontEdgePattern2[5 * 5] = {
    0, 1, 1, 1, 0,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1,
    0, 1, 1, 1, 0
};
static const unsigned char s_fontEdgePattern3[7 * 7] = {
    0, 0, 0, 1, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 0, 0, 0
};
static const unsigned char s_fontEdgePattern4[9 * 9] = {
    0, 0, 1, 1, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0, 0
};
static const unsigned char *s_fontEdgePatternTable[3] = {
    s_fontEdgePattern2,
    s_fontEdgePattern3,
    s_fontEdgePattern4
};

static SDL_Surface *s_DrawEdge(FontData *fontData, SDL_Surface *surface) {
    SDL_Surface *textSurface;
    unsigned int *srcPixels;
    unsigned int *destPixels;
    int srcPitch = surface->pitch / 4;
    int destPitch;
    int edgeSize = fontData->edgeSize;
    int width = surface->w + edgeSize + edgeSize;
    int height = surface->h + edgeSize + edgeSize;
    int x, y;
    
    textSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                                       0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    if (textSurface == NULL) {
        return NULL;
    }
    
    destPitch = textSurface->pitch / 4;
    destPixels = textSurface->pixels;
    
    /* Pass one - create the edge mask. */
    if (edgeSize == 1) {
        srcPixels = surface->pixels;
        destPixels += destPitch + 1;
        for (y = 0; y < surface->h; ++y) {
            for (x = 0; x < surface->w; ++x) {
                unsigned int alpha = srcPixels[x];
                if (alpha > 0xffffff) {
                    destPixels[x] = 0xffffffff;
                    
                    if (destPixels[x + 1] < alpha) { destPixels[x + 1] = alpha; }
                    if (destPixels[x - 1] < alpha) { destPixels[x - 1] = alpha; }
                    if (destPixels[x + destPitch] < alpha) { destPixels[x + destPitch] = alpha; }
                    if (destPixels[x - destPitch] < alpha) { destPixels[x - destPitch] = alpha; }
                }
            }
            srcPixels += srcPitch;
            destPixels += destPitch;
        }
    } else if (edgeSize >= 2 && edgeSize <= 4) {
        const unsigned char *edgePattern;
        int ec = edgeSize + edgeSize + 1;
        int offset = (edgeSize * destPitch) + edgeSize;
        
        edgePattern = s_fontEdgePatternTable[edgeSize - 2];
        
        srcPixels = surface->pixels;
        destPixels += offset;
        for (y = 0; y < surface->h; ++y) {
            for (x = 0; x < surface->w; ++x) {
                unsigned int alpha = srcPixels[x];
                if (alpha > 0xffffff) {
                    const unsigned char *pattern = edgePattern;
                    unsigned int *dest;
                    int bx, by;
                    
                    destPixels[x] = alpha;
                    
                    dest = destPixels + x - offset;
                    
                    for (by = 0; by < ec; ++by) {
                        for (bx = 0; bx < ec; ++bx) {
                            if (*pattern++ > 0 && dest[bx] < alpha) {
                                dest[bx] = alpha;
                            }
                        }
                        dest += destPitch;
                    }
                }
            }
            srcPixels += srcPitch;
            destPixels += destPitch;
        }
    }
    
    return textSurface;
}

static SDL_Surface *s_GetGlyphSurface(FontData *fontData, GlyphData *glyph) {
    SDL_Color color;
    color.r = 0xff;
    color.g = 0xff;
    color.b = 0xff;
    color.a = 0xff;
    switch((fontData->fontType &~ DX_FONTTYPE_EDGE)) {
        case DX_FONTTYPE_NORMAL: {
            SDL_Surface *BWsurface;
            SDL_Surface *textSurface = NULL;
            BWsurface = TTF_RenderGlyph_Solid(fontData->font, (Uint16)glyph->glyphID, color);
            if (BWsurface != NULL) {
                textSurface = SDL_ConvertSurface(BWsurface, s_pixelFormat, 0);
                SDL_FreeSurface(BWsurface);
            }
            return textSurface;
        }
        default:
            /* Any of the DX_FONTTYPE_ANTIALIASING* entries */
            return TTF_RenderGlyph_Blended(fontData->font, (Uint16)glyph->glyphID, color);
    }
}

static int s_FitSurface(FontData *fontData, SDL_Surface *surface, GlyphRect *rect) {
    GlyphTexture *glyphTexture = &fontData->glyphTexture;
    int x, y, w, h, nextY;
    SDL_Rect texRect;
    
    /* - Fit glyph onto the texture, updating the next position. */
    x = glyphTexture->X;
    y = glyphTexture->Y;
    w = surface->w;
    h = surface->h;
    if ((x + w) > glyphTexture->width) {
        if (w > glyphTexture->width) {
            SDL_FreeSurface(surface);
            
            s_GrowGlyphTexture(fontData);
            
            return -1;
        }
        x = 0;
        y = glyphTexture->NextY;
        
        glyphTexture->X = x;
        glyphTexture->Y = y;
    }
    
    nextY = y + h;
    if (nextY > glyphTexture->height) {
        s_GrowGlyphTexture(fontData);
        
        return -1;
    }
    
    if (nextY > glyphTexture->NextY) {
        glyphTexture->NextY = nextY;
    }
    
    rect->x = (unsigned short)x;
    rect->y = (unsigned short)y;
    rect->w = (unsigned short)w;
    rect->h = (unsigned short)h;
    texRect.x = x;
    texRect.y = y;
    texRect.w = w;
    texRect.h = h;
    
    PL_Texture_BlitSurface(glyphTexture->textureID, surface, &texRect);
    
    glyphTexture->X = x + w;
    
    return 0;
}

static void s_AddGlyphToTexture(FontData *fontData, GlyphData *glyph) {
    SDL_Surface *surface;
    int retval;
    
    /* - Get glyph. */
    surface = s_GetGlyphSurface(fontData, glyph);
    if (surface == NULL) {
        return;
    }
    
    retval = s_FitSurface(fontData, surface, &glyph->rect);
    glyph->edgeRect = glyph->rect;
    
    if (retval >= 0 && (fontData->fontType & DX_FONTTYPE_EDGE) != 0) {
        SDL_Surface *edgeSurface = s_DrawEdge(fontData, surface);
        if (edgeSurface != NULL) {
            s_FitSurface(fontData, edgeSurface, &glyph->edgeRect);
            
            SDL_FreeSurface(edgeSurface);
        }
    }
    
    SDL_FreeSurface(surface);
}

static GlyphData *s_CacheGlyph(FontData *fontData, unsigned int glyphID) {
    /* - Check to see if the font has a glyph. */
    GlyphTexture *glyphTexture = &fontData->glyphTexture;
    int minX, maxX, minY, maxY, advance;
    int retval;
    GlyphData *glyph;
    
    if (glyphID > 65535) {
        return NULL;
    }
    
    glyph = s_GetGlyph(fontData, glyphID);
    if (glyph != NULL) {
        return glyph;
    }
    
    if (TTF_GlyphIsProvided(fontData->font, (Uint16)glyphID) == FALSE) {
        return NULL;
    }
    
    /* - Get glyph metrics and information. */
    retval = TTF_GlyphMetrics(fontData->font, (Uint16)glyphID, 
                              &minX, &maxX, &minY, &maxY, &advance);
    if (retval < 0) {
        return NULL;
    }
    
    /* - Get a new glyph slot for use, if available. */
    glyph = s_AllocateGlyph(fontData, glyphID);
    if (glyph == NULL) {
        return NULL;
    }

    glyph->rect.x = 0;
    glyph->rect.y = 0;
    glyph->rect.w = (unsigned short)(maxX - minX);
    glyph->rect.h = (unsigned short)(maxY - minY);
    glyph->advance = (short)advance;
    
    if ((fontData->fontType & DX_FONTTYPE_EDGE) != 0) {
        glyph->advance = (short)(glyph->advance + (fontData->edgeSize * 2));
    }
    
    if (maxX == minX && maxY == minY) {
        /* Probably a blank space. */
        return NULL;
    }
    
    /* - Create default texture if needed. */
    if (glyphTexture->textureID < 0) {
        s_GrowGlyphTexture(fontData);
        return glyph;
    }
    
    s_AddGlyphToTexture(fontData, glyph);
    
    if (minX < 0) {
        glyph->xOffset = (short)-minX;
    }
    
    return glyph;
}

/* ---------------------------------------- DRAWING AND PUBLIC FUNCTIONS */

int PL_Font_DrawExtendStringToHandle(int x, int y, double exRateX, double exRateY,
                                     const DXCHAR *string, DXCOLOR color, int fontHandle,
                                     DXCOLOR edgeColor
) {
    FontData *fontData;
    const DXCHAR *s;
    unsigned int ch;
    int graphID;
    int spacing;
    int edgeSize;
    int loop;
    float scaleX = (float)exRateX;
    float scaleY = (float)exRateY;
#ifndef UNICODE
    int charset;
#endif
    int redBright, greenBright, blueBright;
    
    if (string == NULL || *string == 0) {
        return -1;
    }
    
    fontData = s_GetFontData(fontHandle);
    if (fontData == NULL) {
        return -1;
    }
    
    /* - Cache all glyphs beforehand, so as not to thrash the texture. */
    s = string;
    
#ifdef UNICODE
    while ((ch = PL_Text_ReadDxChar(&s)) != 0) {
#else
    charset = fontData->charset;
    
    while ((ch = PL_Text_ReadChar(&s, charset)) != 0) {
#endif
        s_CacheGlyph(fontData, ch);
    }
    
    /* - Calculate draw position and get basic information */
    spacing = fontData->spacing;
    edgeSize = fontData->edgeSize;
    
    graphID = fontData->glyphTexture.graphID;
    if (graphID < 0) {
        /* Nothing to do. */
        return 0;
    }
    
    PL_Draw_GetBright(&redBright, &greenBright, &blueBright);
    
    /* - Loop twice: First for 'edge' textures, then for 'front' textures. */
    for (loop = 0; loop < 2; ++loop) {
        float cx, cy;
        int plus;
        
        if (loop == 0) {
            /* edge pass */
            if ((fontData->fontType & DX_FONTTYPE_EDGE) == 0) {
                continue;
            }
            PL_Draw_SetBright(edgeColor & 0xff, (edgeColor >> 8) & 0xff, (edgeColor >> 16) & 0xff);
            plus = 0;
        } else if (loop == 1) {
            /* solid pass */
            PL_Draw_SetBright(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff);
            plus = edgeSize;
        }
        
        s = string;
        cx = (float)x + ((float)plus * scaleX);
        cy = (float)y + ((float)plus * scaleY);
        
#ifdef UNICODE
        while ((ch = PL_Text_ReadDxChar(&s)) != 0) {
#else
        while ((ch = PL_Text_ReadChar(&s, charset)) != 0) {
#endif
            if (ch == '\n') {
                cy += (float)fontData->ptSize;
                cx = (float)(x + plus) * scaleY;
            } else {
                GlyphData *glyph = s_GetGlyph(fontData, ch);
                if (glyph != NULL) {
                    GlyphRect *gRect;
                    if (loop == 0) {
                        gRect = &glyph->edgeRect;
                    } else {
                        gRect = &glyph->rect;
                    }
                    
                    PL_EXT_Draw_RectGraphFastF(
                        cx + (glyph->xOffset * scaleX), cy, gRect->w * scaleX, gRect->h * scaleY,
                        gRect->x, gRect->y, gRect->w, gRect->h,
                        graphID,
                        DXTRUE
                    );
                    
                    cx = cx + ((float)(glyph->advance + spacing) * scaleX);
                }
            }
        }
    }
    
    PL_Draw_SetBright(redBright, greenBright, blueBright);
    
    return 0;
}
int PL_Font_DrawStringToHandle(int x, int y, const DXCHAR *string,
                               DXCOLOR color, int fontHandle, DXCOLOR edgeColor) {
    return PL_Font_DrawExtendStringToHandle(x, y, 1.0, 1.0, string, color, fontHandle, edgeColor);
}

int PL_Font_DrawFormatStringToHandle(
    int x, int y, DXCOLOR color, int fontHandle,
    const DXCHAR *formatString, va_list args
) {
    return PL_Font_DrawExtendFormatStringToHandle(x, y, 1.0, 1.0, color, fontHandle, formatString, args);
}

int PL_Font_DrawExtendFormatStringToHandle(
    int x, int y, double exRateX, double exRateY,
    DXCOLOR color, int fontHandle,
    const DXCHAR *formatString, va_list args
) {
    DXCHAR buf[4096];
    DXVSNPRINTF(buf, 4096, formatString, args);
    
    return PL_Font_DrawExtendStringToHandle(x, y, exRateX, exRateY, buf, color, fontHandle, 0);
}

int PL_Font_GetDrawStringWidthToHandle(const DXCHAR *string, int strLen, int fontHandle) {
    FontData *fontData;
    const DXCHAR *end;
    int spacing;
    unsigned int ch;
#ifndef UNICODE
    int charset;
#endif
    int x = 0, w = 0;
    
    if (string == NULL || *string == 0) {
        return -1;
    }
    
    fontData = s_GetFontData(fontHandle);
    if (fontData == NULL) {
        return -1;
    }
    if (strLen < 0) {
        strLen = 1000000; /* should be sufficient */
    }
    
    end = string + strLen;
    spacing = fontData->spacing;
    
#ifdef UNICODE
    while (string < end && (ch = PL_Text_ReadDxChar(&string)) != 0) {
#else
    charset = fontData->charset;
    
    while (string < end && (ch = PL_Text_ReadChar(&string, charset)) != 0) {
#endif
        GlyphData *glyph = s_CacheGlyph(fontData, ch);
        
        if (ch == '\n') {
            x = 0;
        } else {
            int x2 = x + glyph->xOffset + glyph->edgeRect.w;
            if (x2 > w) {
                w = x;
            }
            
            x = x + glyph->advance + spacing;
            if (x > w) {
                w = x;
            }
        }
    }
    
    return w;
}

int PL_Font_GetDrawFormatStringWidthToHandle(int fontHandle, const DXCHAR *formatString, va_list args) {
    DXCHAR buf[4096];
    DXVSNPRINTF(buf, 4096, formatString, args);
    
    return PL_Font_GetDrawStringWidthToHandle(buf, -1, fontHandle);
}

int PL_Font_GetDrawExtendStringWidthToHandle(double ExRateX, const DXCHAR *string, int strLen, int fontHandle) {
    return (int)SDL_ceil(PL_Font_GetDrawStringWidthToHandle(string, strLen, fontHandle) * ExRateX);
}
int PL_Font_GetDrawExtendFormatStringWidthToHandle(double ExRateX, int fontHandle, const DXCHAR *formatString, va_list args) {
    return (int)SDL_ceil(PL_Font_GetDrawFormatStringWidthToHandle(fontHandle, formatString, args) * ExRateX);
}

int PL_Font_GetFontSizeToHandle(int fontHandle) {
    FontData *fontData = s_GetFontData(fontHandle);
    if (fontData == NULL) {
        return -1;
    }
    
    return fontData->ptSize;
}

int PL_Font_GetFontCharInfo(int fontHandle, const DXCHAR *string, int *xPos, int *yPos, int *advanceX, int *width, int *height) {
    FontData *fontData = s_GetFontData(fontHandle);
    unsigned int ch;
    GlyphData *glyph;
    GlyphRect rect;
    
    if (fontData == NULL) {
        return -1;
    }
    
#ifdef UNICODE
    ch = PL_Text_ReadDxChar(&string);
#else
    ch = PL_Text_ReadChar(&string, fontData->charset);
#endif
    
    glyph = s_CacheGlyph(fontData, ch);
    if (glyph == NULL) {
        return -1;
    }
    
    rect = glyph->edgeRect;
    
    *xPos = glyph->xOffset;
    *yPos = 0;
    *width = rect.w;
    *height = rect.h;
    *advanceX = glyph->advance;
    
    return 0;
}

int PL_Font_SetFontSpaceToHandle(int fontSpacing, int fontHandle) {
    FontData *fontData = s_GetFontData(fontHandle);
    if (fontData == NULL) {
        return -1;
    }
    
    fontData->spacing = fontSpacing;
    
    return 0;
}

/* Implementation of the handle-less fonts, which just makes a font handle.
 * 
 * The real trick here is the interface is bad, so we want to cache the
 * information and update handle only as needed.
 */

static int s_GetDefaultFontHandle() {
    if (s_defaultFontRefreshFlag == DXFALSE) {
        return s_defaultFontHandle;
    }
    
    if (s_defaultFontHandle >= 0) {
        PL_Font_DeleteFontToHandle(s_defaultFontHandle);
    }
    
    if (s_defaultFontInfo.name == NULL) {
        return -1;
    }
    
    s_defaultFontRefreshFlag = DXFALSE;
    
    s_defaultFontHandle =
        PL_Font_CreateFontToHandle(
            s_defaultFontInfo.name,
            s_defaultFontInfo.fontSize,
            s_defaultFontInfo.fontThickness,
            s_defaultFontInfo.fontType,
            s_defaultFontInfo.charSet,
            s_defaultFontInfo.edgeSize,
            DXFALSE
        );
    PL_Font_SetFontSpaceToHandle(s_defaultFontInfo.fontSpacing, s_defaultFontHandle);
    
    return s_defaultFontHandle;
}

int PL_Font_DrawString(int x, int y, const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor) {
    return PL_Font_DrawStringToHandle(x, y, string, color, s_GetDefaultFontHandle(), edgeColor);
}

int PL_Font_DrawFormatString(
    int x, int y, DXCOLOR color,
    const DXCHAR *formatString, va_list args
) {
    return PL_Font_DrawFormatStringToHandle(x, y, color, s_GetDefaultFontHandle(), formatString, args);
}

int PL_Font_DrawExtendString(int x, int y, double ExRateX, double ExRateY,
                             const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor) {
    return PL_Font_DrawExtendStringToHandle(x, y, ExRateX, ExRateY, string, color, s_GetDefaultFontHandle(), edgeColor);
}

int PL_Font_DrawExtendFormatString(
    int x, int y, double ExRateX, double ExRateY, DXCOLOR color,
    const DXCHAR *formatString, va_list args
) {
    return PL_Font_DrawExtendFormatStringToHandle(x, y, ExRateX, ExRateY, color, s_GetDefaultFontHandle(), formatString, args);
}

int PL_Font_GetDrawStringWidth(const DXCHAR *string, int strLen) {
    return PL_Font_GetDrawStringWidthToHandle(string, strLen, s_GetDefaultFontHandle());
}

int PL_Font_GetDrawFormatStringWidth(const DXCHAR *string, va_list args) {
    return PL_Font_GetDrawFormatStringWidthToHandle(s_GetDefaultFontHandle(), string, args);
}

int PL_Font_GetDrawExtendStringWidth(double ExRateX, const DXCHAR *string, int strLen) {
    return PL_Font_GetDrawExtendStringWidthToHandle(ExRateX, string, strLen, s_GetDefaultFontHandle());
}

int PL_Font_GetDrawExtendFormatStringWidth(double ExRateX, const DXCHAR *string, va_list args) {
    return PL_Font_GetDrawExtendFormatStringWidthToHandle(ExRateX, s_GetDefaultFontHandle(), string, args);
}

int PL_Font_ChangeFont(const DXCHAR *string, int charSet) {
    if (s_defaultFontInfo.name != NULL && DXSTRCMP(s_defaultFontInfo.name, string)) {
        DXFREE(s_defaultFontInfo.name);
    }
    
    if (s_defaultFontInfo.name == NULL) {
        s_defaultFontInfo.name = DXSTRDUP(string);
        s_defaultFontRefreshFlag = DXTRUE;
    }
    
    if (s_defaultFontInfo.charSet != charSet) {
        s_defaultFontInfo.charSet = charSet;
        s_defaultFontRefreshFlag = DXTRUE;
    }
    
    return 0;
}

int PL_Font_ChangeFontType(int fontType) {
    if (s_defaultFontInfo.fontType != fontType) {
        s_defaultFontInfo.fontType = fontType;
        s_defaultFontRefreshFlag = DXTRUE;
    }
    return 0;
}

int PL_Font_SetFontSize(int fontSize) {
    if (s_defaultFontInfo.fontSize != fontSize) {
        s_defaultFontInfo.fontSize = fontSize;
        s_defaultFontRefreshFlag = DXTRUE;
    }
    return 0;
}

int PL_Font_GetFontSize() {
    return s_defaultFontInfo.fontSize;
}

int PL_Font_SetFontThickness(int fontThickness) {
    if (s_defaultFontInfo.fontThickness != fontThickness) {
        s_defaultFontInfo.fontThickness = fontThickness;
        s_defaultFontRefreshFlag = DXTRUE;
    }
    return 0;
}

int PL_Font_SetFontSpace(int fontSpace) {
    s_defaultFontInfo.fontSpacing = fontSpace;
    PL_Font_SetFontSpaceToHandle(fontSpace, s_defaultFontHandle);
    return 0;
}

int PL_Font_SetDefaultFontState(const DXCHAR *fontName, int fontSize, int fontThickness) {
    PL_Font_ChangeFont(fontName, fontSize);
    PL_Font_SetFontThickness(fontThickness);
    return 0;
}

int PL_Font_GetDefaultFontHandle() {
    return s_GetDefaultFontHandle();
}

/* ----------------------------------------------- FONT HANDLE MANAGEMENT */

int PL_Font_CreateFontToHandle(const DXCHAR *fontname,
            int size, int thickness, int fontType, int charset,
            int edgeSize, int italic
) {
    FontMapping *mapping;
    FontMapping *bestMapping = NULL;
    int fontID;
    
    if (s_fontInitialized == DXFALSE) {
        return -1;
    }
    
    /* apply defaults */
    if (fontType < 0) {
        fontType = DX_FONTTYPE_NORMAL;
    }
    if (size < 0) {
        size = 16;
    }
    if (thickness < 0) {
        thickness = 6;
    }
    if (edgeSize < 1 || edgeSize > 4) {
        edgeSize = 1;
    }
    
    for (mapping = s_fontMappings; mapping != NULL; mapping = mapping->next) {
        if (!DXSTRCMP(fontname, mapping->fontname)) {
            if (bestMapping == NULL
                || (bestMapping != NULL && mapping->thickness > bestMapping->thickness)
            ) {
                bestMapping = mapping;
            }
        }
    }
    
    if (bestMapping == NULL) {
        return -1;
    }
    
    if (bestMapping->directFileAccessOnly) {
        fontID = s_LoadFontFileDirect(bestMapping->filename, bestMapping, size);
    } else {
        fontID = s_LoadFontFile(bestMapping->filename, bestMapping, size);
    }
    
    if (fontID >= 0) {
        FontData *fontData = (FontData *)PL_Handle_GetData(fontID, DXHANDLE_FONT);
        int fontStyle = 0;
        
        fontData->fontType = fontType;
        
        fontData->edgeSize = edgeSize;
        
        if ((fontType & DX_FONTTYPE_ANTIALIASING) == 0) {
            TTF_SetFontHinting(fontData->font, TTF_HINTING_MONO);
        }
        if (bestMapping->boldFlag != 0) {
            fontStyle |= TTF_STYLE_BOLD;
        }
        if (italic) {
            fontStyle |= TTF_STYLE_ITALIC;
        }
        TTF_SetFontStyle(fontData->font, fontStyle);
        
        fontData->charset = charset;
    }
    
    return fontID;
}

int PL_Font_DeleteFontToHandle(int handle) {
    FontData *fontData = s_GetFontData(handle);
    if (fontData == NULL) {
        return -1;
    }
    
    if (fontData->glyphTexture.graphID >= 0) {
        PL_Graph_Delete(fontData->glyphTexture.graphID);
    }
    if (fontData->glyphTexture.textureID >= 0) {
        PL_Texture_Release(fontData->glyphTexture.textureID);
    }
    
    if (fontData->glyphData != NULL) {
        DXFREE(fontData->glyphData);
        fontData->glyphData = 0;
    }
    
    TTF_CloseFont(fontData->font);
    
    PL_Handle_ReleaseID(handle, DXTRUE);
    
    if (s_defaultFontHandle == handle) {
        s_defaultFontHandle = -1;
        s_defaultFontRefreshFlag = DXTRUE;
    }
    
    return 0;
}

int PL_Font_SetFontLostFlag(int fontHandle, int *lostFlag) {
    return PL_Handle_SetDeleteFlag(fontHandle, lostFlag);
}

int PL_Font_CheckFontHandleValid(int fontHandle) {
    FontData *fontData = s_GetFontData(fontHandle);
    if (fontData == NULL) {
        return -1;
    }
    return 0;
}

int PL_Font_InitFontToHandle() {
    int handle;
    
    if (s_fontInitialized == DXFALSE) {
        return -1;
    }
    
    while ((handle = PL_Handle_GetFirstIDOf(DXHANDLE_FONT)) >= 0) {
        PL_Font_DeleteFontToHandle(handle);
    }
    
    return 0;
}

void PL_Font_Init() {
    /* Do nothing */
}

void PL_Font_End() {
    PL_Font_InitFontToHandle();
    
    PLEXT_Font_InitFontMappings();
    
    TTF_Quit();
    
    SDL_FreeFormat(s_pixelFormat);
    
    if (s_defaultFontInfo.name != NULL) {
        DXFREE(s_defaultFontInfo.name);
        s_defaultFontInfo.name = NULL;
    }
    
    s_fontInitialized = DXFALSE;
}

#endif /* #ifndef DX_NON_FONT */
