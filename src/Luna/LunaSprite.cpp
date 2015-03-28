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

/* Both 2D and 3D versions of LunaSprite are handled here because they are
 * almost identical.
 * 
 * Various vertex effects are not supported because there's no need for
 * them.
 */

#include "Luna.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "LunaInternal.h"

typedef struct _LunaSpriteData {
    char *vertexData;
    
    int vertexPtr;
    int indexPtr;
    int vertexMax;
    
    int vertexStride;
    
    int vboHandle;
    int iboHandle;
    
    int textureIDs[4];
    unsigned int maxGraph;
    
    const LunaVertexInfo *vertexInfo;
} LunaSpriteData;

static unsigned short *s_createStaticSpriteIndexBuffer(int indexCount) {
    unsigned short *indices;
    int i, j;
    
    indices = (unsigned short *)DXALLOC(indexCount * sizeof(unsigned short));
    for (i = 0, j = 0; i < indexCount; i += 6, j += 4) {
        indices[i + 0] = j + 0;
        indices[i + 1] = j + 1;
        indices[i + 2] = j + 2;
        indices[i + 3] = j + 0;
        indices[i + 4] = j + 2;
        indices[i + 5] = j + 3;
    }
    
    return indices;
}

static LSPRITE s_SpriteCreate(const LunaVertexInfo *vertexInfo,
                              Uint32 VertexMax, bool IsSortZ) {
    int spriteID = -1;
    LunaSpriteData *sprite;
    int vboID = -1;
    unsigned short *indices;
    int iboID = -1;
    
    if (VertexMax < 4 || vertexInfo == NULL) {
        return INVALID_SPRITE;
    }
    
    VertexMax = (VertexMax + 3) & ~3;
    do {
        /* round down to nearest multiple of 4. Not accurate to Luna, but
         * simplifies calculations a bit. */
        int spriteCount = VertexMax / 4;
        int indexCount = spriteCount * 6;
        
        vboID = PLG.VertexBuffer_Create(vertexInfo->def,
                                    NULL, VertexMax, DXFALSE);
        if (vboID < 0) {
            break;
        }
        
        indices = s_createStaticSpriteIndexBuffer(indexCount);
        iboID = PLG.IndexBuffer_Create(indices, indexCount, DXTRUE);
        DXFREE(indices);
        
        if (iboID < 0) {
            break;
        }
        
        spriteID = PL_Handle_AcquireID(DXHANDLE_LUNASPRITE);
        if (spriteID < 0) {
            break;
        }
        
        sprite = (LunaSpriteData *)PL_Handle_AllocateData(spriteID, sizeof(LunaSpriteData));
        sprite->vertexData = (char *)DXALLOC(vertexInfo->def->vertexByteSize * VertexMax);
        sprite->vertexPtr = 0;
        sprite->vertexMax = VertexMax;
        sprite->vertexStride = vertexInfo->def->vertexByteSize;
        sprite->vboHandle = vboID;
        
        sprite->iboHandle = iboID;
        sprite->indexPtr = 0;
        
        sprite->textureIDs[0] = -1;
        sprite->textureIDs[1] = -1;
        sprite->textureIDs[2] = -1;
        sprite->textureIDs[3] = -1;
        
        sprite->maxGraph = vertexInfo->texCount;
        
        sprite->vertexInfo = vertexInfo;
        
        return spriteID;
    } while (0);
    
    PLG.VertexBuffer_Delete(vboID);
    PLG.IndexBuffer_Delete(iboID);
    
    return INVALID_SPRITE;
}

LSPRITE LunaSprite::Create(Uint32 VertexMax, eVertexPrimitiveType vertexType,
                     bool IsSortZ) {
    return s_SpriteCreate(s_GetLunaVertex2DInfo(vertexType), VertexMax, IsSortZ);
}

#if 0
LSPRITE LunaSprite3D::Create(Uint32 VertexMax, eVertexPrimitiveType vertexType,
                     bool IsSortZ) {
    return s_SpriteCreate(s_GetLunaVertex3DInfo(vertexType), VertexMax, IsSortZ);
}
#endif

void LunaSprite::Release(LSPRITE lSpr) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL) {
        PLG.VertexBuffer_Delete(sprite->vboHandle);
        PLG.IndexBuffer_Delete(sprite->iboHandle);
        
        DXFREE(sprite->vertexData);
        
        PL_Handle_ReleaseID((int)lSpr, DXTRUE);
    }
}
void LunaSprite::AttatchTexture(LSPRITE lSpr, Uint32 Stage, LTEXTURE lTex) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL && Stage < sprite->maxGraph) {
        sprite->textureIDs[Stage] = (int)lTex;
    }
}

static void s_drawSquare2D(LunaSpriteData *sprite,
                           float x1, float y1,
                           float x2, float y2,
                           float x3, float y3,
                           float x4, float y4,
                           float z, CLunaRect *srcRects, unsigned int color,
                           Uint32 UvCount) {
    LunaVertex2DTex4 *v[4];
    char *vertexData = sprite->vertexData;
    int vN = sprite->vertexPtr;
    int stride = sprite->vertexStride;
    unsigned int i;
    const float ofs = -0.5f; /* D3D does this thing where everything is offset by 0.5f */
    float tfs;
    float sXMul = g_lunaVirtualXmult;
    float sYMul = g_lunaVirtualYmult;
    
    if (UvCount > sprite->maxGraph) {
        UvCount = sprite->maxGraph;
    }
    
    for (i = 0; i < 4; ++i, ++vN) {
        v[i] = (LunaVertex2DTex4 *)(vertexData + (stride * vN));
    }
    
    z *= INV_PRIMITIVE_Z_MAX;
    
    /* assign vertices */
    v[0]->x = (x1 * sXMul) + ofs; v[0]->y = (y1 * sYMul) + ofs; v[0]->z = z; v[0]->w = 1.0f; v[0]->color = color;
    v[1]->x = (x2 * sXMul) + ofs; v[1]->y = (y2 * sYMul) + ofs; v[1]->z = z; v[1]->w = 1.0f; v[1]->color = color;
    v[2]->x = (x3 * sXMul) + ofs; v[2]->y = (y3 * sYMul) + ofs; v[2]->z = z; v[2]->w = 1.0f; v[2]->color = color;
    v[3]->x = (x4 * sXMul) + ofs; v[3]->y = (y4 * sYMul) + ofs; v[3]->z = z; v[3]->w = 1.0f; v[3]->color = color;
    
    /* match Luna's buggy rendering when filtering is enabled */
    tfs = g_lunaFilterOffset;
    
    /* assign textures */
    for (i = 0; i < UvCount; ++i) {
        int texRefID = sprite->textureIDs[i];
        float xMult, yMult;
        float tx1, ty1, tx2, ty2;
        if (PLG.Texture_RenderGetTextureInfo(texRefID, NULL, &xMult, &yMult) < 0) {
            tx1 = 0; ty1 = 0;
            tx2 = 0; ty2 = 0;
        } else {
            CLunaRect *rect = &srcRects[i];
            tx1 = (rect->Px + tfs) * xMult;
            ty1 = (rect->Py + tfs) * yMult;
            tx2 = (rect->Px + rect->Sx - tfs) * xMult;
            ty2 = (rect->Py + rect->Sy - tfs) * yMult;
        }
        
        v[0]->t[i].u = tx1; v[0]->t[i].v = ty1;
        v[1]->t[i].u = tx2; v[1]->t[i].v = ty1;
        v[2]->t[i].u = tx2; v[2]->t[i].v = ty2;
        v[3]->t[i].u = tx1; v[3]->t[i].v = ty2;
    }
    
    /* advance pointers */
    sprite->vertexPtr += 4;
    sprite->indexPtr += 6;
}

void LunaSprite::DrawSquare(LSPRITE lSpr, CLunaRect *pDstRect, Float Pz,
                      CLunaRect *pSrcRects, D3DCOLOR Color,
                      Uint32 UvCount) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL && (sprite->vertexPtr + 4) <= sprite->vertexMax) {
        float x1 = pDstRect->Px;
        float y1 = pDstRect->Py;
        float x2 = pDstRect->Px + pDstRect->Sx;
        float y2 = pDstRect->Py + pDstRect->Sy;
        
        s_drawSquare2D(sprite,
                       x1, y1,
                       x2, y1,
                       x2, y2,
                       x1, y2,
                       Pz, pSrcRects, Color, UvCount);
    }
}
void LunaSprite::DrawSquareRotate(LSPRITE lSpr, CLunaRect *pDstRect, Float Pz,
                            CLunaRect *pSrcRects, D3DCOLOR Color,
                            Uint32 Angle, Uint32 UvCount,
                            float fCx, float fCy) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL && (sprite->vertexPtr + 4) < sprite->vertexMax) {
        float cx = (pDstRect->Px + (pDstRect->Sx * 0.5f)) + fCx;
        float cy = (pDstRect->Py + (pDstRect->Sy * 0.5f)) + fCy;
        float exX1 = pDstRect->Px - cx;
        float exY1 = pDstRect->Py - cy;
        float exX2 = pDstRect->Px - cx + pDstRect->Sx;
        float exY2 = pDstRect->Py - cy + pDstRect->Sy;
        float fSin = LunaMath::Sin(Angle);
        float fCos = LunaMath::Cos(Angle);
        
        s_drawSquare2D(sprite,
                       cx + (exX1 * fCos) - (exY1 * fSin),
                       cy + (exX1 * fSin) + (exY1 * fCos),
                       cx + (exX2 * fCos) - (exY1 * fSin),
                       cy + (exX2 * fSin) + (exY1 * fCos),
                       cx + (exX2 * fCos) - (exY2 * fSin),
                       cy + (exX2 * fSin) + (exY2 * fCos),
                       cx + (exX1 * fCos) - (exY2 * fSin),
                       cy + (exX1 * fSin) + (exY2 * fCos),
                       Pz, pSrcRects, Color, UvCount);
    }
}

void LunaSprite::DrawSquareRotateXYZ(LSPRITE lSpr, CLunaRect *pDstRect, Float Pz,
                            CLunaRect *pSrcRects, D3DCOLOR Color,
                            Uint32 AngleX, Uint32 AngleY, Uint32 AngleZ,
                            Uint32 UvCount) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL && (sprite->vertexPtr + 4) < sprite->vertexMax) {
        /* Need an actual matrix for this. */
    }
}

void LunaSprite::ResetBuffer(LSPRITE lSpr) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL) {
        sprite->vertexPtr = 0;
        sprite->indexPtr = 0;
    }
}
void LunaSprite::UpdateBuffer(LSPRITE lSpr) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL && sprite->vertexPtr > 0) {
        PLG.VertexBuffer_SetData(sprite->vboHandle,
                                sprite->vertexData,
                                0, sprite->vertexPtr, DXTRUE);
    }
}
void LunaSprite::Rendering(LSPRITE lSpr) {
    LunaSpriteData *sprite = (LunaSpriteData *)PL_Handle_GetData((int)lSpr, DXHANDLE_LUNASPRITE);
    if (sprite != NULL && sprite->vertexPtr > 0) {
        PLG.SetPresetProgram(
            TEX_PRESET_MODULATE, g_lunaAlphaTestPreset,
            &g_lunaUntransformedProjectionMatrix,
            &g_lunaUntransformedViewMatrix,
            sprite->textureIDs[0], g_lunaFilterMode,
            g_lunaAlphaTestValue);
        
        PLG.DrawVertexIndexBuffer(
            sprite->vertexInfo->def,
            sprite->vboHandle, 0, sprite->vertexPtr,
            sprite->iboHandle,
            PL_PRIM_TRIANGLES, 0, sprite->indexPtr);
        
        PLG.ClearPresetProgram();
    }
}

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */
