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

#ifdef DXPORTLIB_DRAW_SDL2_RENDER

#include "SDL2Render_DxInternal.h"

/* Cannot be supported:
 * - DrawModiGraph
 * - DrawTriangle/DrawQuadrangle
 * - DrawLine thickness.
 * - Blend modes other than NONE/ADD/MUL.
 * - Shader filter nonsense.
 * - 3D.
 * 
 * Most of this is the fault of SDL2_Renderer's limitations.
 */

SDL_Renderer *PL_renderer = NULL;

static const SDL_Point s_zeroPoint = { 0, 0 };
static SDL_BlendMode s_blendMode = SDL_BLENDMODE_NONE;
static Uint8 s_drawColorR = 255;
static Uint8 s_drawColorG = 255;
static Uint8 s_drawColorB = 255;
static Uint8 s_drawAlpha = 255;
static float s_drawOffsetX = 0;
static float s_drawOffsetY = 0;
static int s_windowWidth = 640;
static int s_windowHeight = 480;
static int s_builtGraphs = DXFALSE;
static int s_circleGraphID = -1;

static int s_InitTexture(int graphID, SDL_Texture **texture, SDL_Rect *texRect, int blendFlag) {
    if (PL_Texture_RenderGetGraphTexture(graphID, texture, texRect) < 0) {
        return -1;
    }

    if (blendFlag) {
        SDL_SetTextureBlendMode(*texture, s_blendMode);
        SDL_SetTextureColorMod(*texture, s_drawColorR, s_drawColorG, s_drawColorB);
        SDL_SetTextureAlphaMod(*texture, s_drawAlpha);
    } else {
        SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_NONE);
        SDL_SetTextureColorMod(*texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(*texture, 255);
    }
    
    return 0;
}

int PL_Draw_ResetSettings() {
    if (PL_renderer != NULL) {
        return -1;
    }
    
    s_blendMode = SDL_BLENDMODE_NONE;
    s_drawColorR = 255;
    s_drawColorG = 255;
    s_drawColorB = 255;
    s_drawAlpha = 255;
    s_drawOffsetX = 0;
    s_drawOffsetY = 0;
    
    return 0;
}

void PL_Draw_InitCircleGraph() {
    const int circleWidth = 128;
    const int circleRadius = circleWidth / 2;
    SDL_Surface *surface;
    SDL_Palette *palette;
    unsigned char *pixels;
    int pitch;
    int x, y;
    
    if (s_builtGraphs != DXFALSE) {
        return;
    }
    
    s_builtGraphs = DXTRUE;
    
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, circleWidth, circleWidth, 8, 0, 0, 0, 0);
    if (surface == NULL) {
        return;
    }
    
    palette = surface->format->palette;
    palette->colors[0].r = 0;
    palette->colors[0].g = 0;
    palette->colors[0].b = 0;
    palette->colors[1].r = 255;
    palette->colors[1].g = 255;
    palette->colors[1].b = 255;
    SDL_SetColorKey(surface, SDL_TRUE, 0);
    
    pixels = surface->pixels;
    pitch = surface->pitch;
    
    for (y = 0; y < circleWidth; ++y) {
        int cY = circleRadius - y;
        int w = (int)(SDL_sqrt((circleRadius * circleRadius) - (cY * cY)) + 0.5);
        int x1, x2;
        if (w <= 0) {
            continue;
        }
        
        x1 = circleRadius - w;
        x2 = circleRadius + w;
        
        if (x1 < 0) {
            x1 = 0;
        }
        if (x2 > circleWidth) {
            x2 = circleWidth;
        }
        
        for (x = x1; x < x2; ++x) {
            pixels[x] = 1;
        }
        pixels += pitch;
    }
    
    s_circleGraphID = PL_Graph_CreateFromSurface(surface);
    
    SDL_FreeSurface(surface);
}

int PL_Draw_LineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    x1 += s_drawOffsetX;
    y1 += s_drawOffsetY;
    x2 += s_drawOffsetX;
    y2 += s_drawOffsetY;
    
    SDL_SetRenderDrawColor(PL_renderer,
                           (Uint8)((color & 0xff) * s_drawColorR / 255),
                           (Uint8)(((color >> 8) & 0xff) * s_drawColorG / 255),
                           (Uint8)(((color >> 16) & 0xff) * s_drawColorB / 255),
                           s_drawAlpha);
    
    SDL_SetRenderDrawBlendMode(PL_renderer, s_blendMode);
    
    SDL_RenderSetScale(PL_renderer, 1.0f, 1.0f);
        
    SDL_RenderDrawLine(PL_renderer,
                       (int)x1, (int)y1,
                       (int)x2, (int)y2
                       );
    
    SDL_RenderSetScale(PL_renderer, 1.0f/1024.0f, 1.0f/1024.0f);
    
    return 0;
}
int PL_Draw_Line(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness) {
    return PL_Draw_LineF((float)x1, (float)y1, (float)x2, (float)y2, color, thickness);
}

int PL_Draw_OvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag) {
    if (fillFlag) {
        SDL_Rect destRect;
        SDL_Texture *texture;
        SDL_Rect texRect;
        
        PL_Draw_InitCircleGraph();
        
        if (s_InitTexture(s_circleGraphID, &texture, &texRect, DXTRUE) < 0) {
            return -1;
        }
        
        SDL_SetTextureColorMod(texture,
                           (Uint8)((color & 0xff) * s_drawColorR / 255),
                           (Uint8)(((color >> 8) & 0xff) * s_drawColorG / 255),
                           (Uint8)(((color >> 16) & 0xff) * s_drawColorB / 255)
                           );
        
        x += s_drawOffsetX;
        y += s_drawOffsetY;
        
        destRect.x = (int)((x - rx) * 1024);
        destRect.y = (int)((y - ry) * 1024);
        destRect.w = (int)((rx) * 2048);
        destRect.h = (int)((ry) * 2048);
        
        SDL_RenderCopy(PL_renderer, texture, &texRect, &destRect);
    } else {
        /* VC++ is weird and thinks the value is not actually constant. */
#define POINTS 48
        static const float angleValue = (float)M_PI * 2.0f / POINTS;
        SDL_Point p[POINTS + 1];
        float dRx = rx;
        float dRy = ry;
        int i;
        
        x += s_drawOffsetX;
        y += s_drawOffsetY;
        
        for (i = 0; i < POINTS; ++i) {
            p[i].x = (int)(x + (SDL_cosf((float)i * angleValue) * dRx));
            p[i].y = (int)(y + (SDL_sinf((float)i * angleValue) * dRy));
        }
        
        p[POINTS] = p[0];
        
        SDL_SetRenderDrawColor(PL_renderer,
                           (Uint8)((color & 0xff) * s_drawColorR / 255),
                           (Uint8)(((color >> 8) & 0xff) * s_drawColorG / 255),
                           (Uint8)(((color >> 16) & 0xff) * s_drawColorB / 255),
                           s_drawAlpha);
        
        SDL_SetRenderDrawBlendMode(PL_renderer, s_blendMode);
        
        SDL_RenderSetScale(PL_renderer, 1.0f, 1.0f);
        
        SDL_RenderDrawLines(PL_renderer, p, POINTS + 1);
        
        SDL_RenderSetScale(PL_renderer, 1.0f/1024.0f, 1.0f/1024.0f);
#undef POINTS
    }
    
    return 0;
}
int PL_Draw_Oval(int x, int y, int rx, int ry, DXCOLOR color, int fillFlag) {
    return PL_Draw_OvalF((float)x, (float)y, (float)rx, (float)ry, color, fillFlag);
}
int PL_Draw_Circle(int x, int y, int r, DXCOLOR color, int fillFlag) {
    return PL_Draw_OvalF((float)x, (float)y, (float)r, (float)r, color, fillFlag);
}
int PL_Draw_CircleF(float x, float y, float r, DXCOLOR color, int fillFlag) {
    return PL_Draw_OvalF(x, y, r, r, color, fillFlag);
}

int PL_Draw_BoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag) {
    SDL_Rect rect;
    float w = x2 - x1;
    float h = y2 - y1;
    
    x1 += s_drawOffsetX;
    y1 += s_drawOffsetY;
    
    SDL_SetRenderDrawColor(PL_renderer,
                           (Uint8)((color & 0xff) * s_drawColorR / 255),
                           (Uint8)(((color >> 8) & 0xff) * s_drawColorG / 255),
                           (Uint8)(((color >> 16) & 0xff) * s_drawColorB / 255),
                           s_drawAlpha);
    
    SDL_SetRenderDrawBlendMode(PL_renderer, s_blendMode);
    
    if (FillFlag == DXFALSE) {
        rect.x = (int)x1;
        rect.y = (int)y1;
        rect.w = (int)w;
        rect.h = (int)h;
        
        SDL_RenderSetScale(PL_renderer, 1.0f, 1.0f);
        
        SDL_RenderDrawRect(PL_renderer, &rect);
        
        SDL_RenderSetScale(PL_renderer, 1.0f/1024.0f, 1.0f/1024.0f);
    } else {
        rect.x = (int)(x1 * 1024);
        rect.y = (int)(y1 * 1024);
        rect.w = (int)(w * 1024);
        rect.h = (int)(h * 1024);
        SDL_RenderFillRect(PL_renderer, &rect);
    }
    
    return 0;
}

int PL_Draw_Box(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag) {
    return PL_Draw_BoxF((float)x1, (float)y1, (float)x2, (float)y2, color, FillFlag);
}

int PL_Draw_GraphF(float x, float y, int graphID, int blendFlag) {
    SDL_Rect destRect;
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    x += s_drawOffsetX;
    y += s_drawOffsetY;
    
    destRect.x = (int)(x * 1024);
    destRect.y = (int)(y * 1024);
    destRect.w = (int)(texRect.w * 1024);
    destRect.h = (int)(texRect.h * 1024);
    
    SDL_RenderCopy(PL_renderer, texture, &texRect, &destRect);
    
    return 0;
}

int PL_Draw_Graph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_GraphF((float)x, (float)y, graphID, blendFlag);
}

int PL_Draw_ExtendGraphF(float x1, float y1, float x2, float y2, int graphID, int blendFlag) {
    SDL_Rect destRect;
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }

    x1 += s_drawOffsetX;
    y1 += s_drawOffsetY;
    x2 += s_drawOffsetX;
    y2 += s_drawOffsetY;
    
    destRect.x = (int)(x1 * 1024);
    destRect.y = (int)(y1 * 1024);
    destRect.w = (int)((x2 - x1) * 1024);
    destRect.h = (int)((y2 - y1) * 1024);
    
    SDL_RenderCopy(PL_renderer, texture, &texRect, &destRect);
    
    return 0;
}
int PL_Draw_ExtendGraph(int x1, int y1, int x2, int y2, int graphID, int blendFlag) {
    return PL_Draw_ExtendGraphF((float)x1, (float)y1, (float)x2, (float)y2, graphID, blendFlag);
}

int PL_Draw_RectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                      int graphID, int blendFlag, int turnFlag) {
    SDL_Rect destRect, srcRect, drawRect, displayRect;
    int flipHorizFlag;
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }

    /* DxLib's behavior here is confirmably bizarre.
     * 
     * At startup, it sets up a clip window at
     * (dx, dy, dx+sw, dx+sh), and then just draws
     * the entire texture at (dx-sx, dx-sy).
     * 
     * When flipped, it instead draws the texture at
     * (dx - texRect.w + sx + w), (dx - sy)
     * 
     * Which is then clipped against the clip window,
     * to determine what is actually drawn.
     * 
     * FIXME This implementation is very slow.
     * It should really be cleaned up.
     */
    
    dx += s_drawOffsetX;
    dy += s_drawOffsetY;
    
    if (turnFlag) {
        drawRect.x = (int)((dx - (float)texRect.w + (float)sx + (float)sw) * 1024);
        flipHorizFlag = SDL_FLIP_HORIZONTAL;
    } else {
        drawRect.x = (int)((dx - (float)sx) * 1024);
        flipHorizFlag = 0;
    }
    drawRect.y = (int)((dy - (float)sy) * 1024);
    drawRect.w = texRect.w * 1024;
    drawRect.h = texRect.h * 1024;
    
    displayRect.x = (int)(dx * 1024);
    displayRect.y = (int)(dy * 1024);
    displayRect.w = (int)(sw * 1024);
    displayRect.h = (int)(sh * 1024);
    
    SDL_IntersectRect(&drawRect, &displayRect, &destRect);
    
    srcRect.x = destRect.x - drawRect.x;
    srcRect.y = destRect.y - drawRect.y;
    srcRect.w = destRect.w;
    srcRect.h = destRect.h;
    if (flipHorizFlag) {
        srcRect.x = (texRect.w * 1024) - srcRect.x - destRect.w;
    }
    
    srcRect.x /= 1024;
    srcRect.y /= 1024;
    srcRect.w /= 1024;
    srcRect.h /= 1024;
    
    SDL_RenderCopyEx(PL_renderer, texture, &srcRect, &destRect,
                      0, NULL, flipHorizFlag);
    
    return 0;
}
int PL_Draw_RectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                      int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectGraphF((float)dx, (float)dy, sx, sy, sw, sh, graphID, blendFlag, turnFlag);
}

/* Temporary DxPortLib extension because RectGraph is sloooooow) */
int PL_EXT_Draw_RectGraphFastF(
                            float dx, float dy, float dw, float dh,
                            int sx, int sy, int sw, int sh,
                            int graphID, int blendFlag) {
    SDL_Rect destRect, srcRect;
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    srcRect.x = sx;
    srcRect.y = sy;
    srcRect.w = sw;
    srcRect.h = sh;
    destRect.x = (int)((dx + s_drawOffsetX) * 1024);
    destRect.y = (int)((dy + s_drawOffsetY) * 1024);
    destRect.w = (int)(dw * 1024);
    destRect.h = (int)(dh * 1024);
    
    SDL_RenderCopy(PL_renderer, texture, &srcRect, &destRect);
    
    return 0;
}

int PL_Draw_RectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                             int sx, int sy, int sw, int sh,
                             int graphID, int blendFlag, int turnFlag) {
    SDL_Rect destRect, srcRect;
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    dx1 += s_drawOffsetX;
    dy1 += s_drawOffsetY;
    dx2 += s_drawOffsetX;
    dy2 += s_drawOffsetY;
    
    srcRect.x = sx;
    srcRect.y = sy;
    srcRect.w = sw;
    srcRect.h = sh;
    destRect.x = (int)(dx1 * 1024);
    destRect.y = (int)(dy1 * 1024);
    destRect.w = (int)((dx2 - dx1) * 1024);
    destRect.h = (int)((dy2 - dy1) * 1024);
    
    SDL_RenderCopyEx(PL_renderer, texture, &srcRect, &destRect,
                     0, NULL, turnFlag ? SDL_FLIP_HORIZONTAL : 0);
    
    return 0;
}

int PL_Draw_RectExtendGraph(int dx1, int dy1, int dx2, int dy2,
                            int sx, int sy, int sw, int sh,
                            int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectExtendGraphF(
                (float)dx1, (float)dy1, (float)dx2, (float)dy2,
                sx, sy, sw, sh,
                graphID, blendFlag, turnFlag
            );
}

static int s_Draw_RotaGraphMain(
                       SDL_Texture *texture,
                       const SDL_Rect *texRect,
                       float x, float y, 
                       float cx, float cy,
                       double xScaleFactor,
                       double yScaleFactor,
                       double angle,
                       int graphID, int blendFlag, int turn)
{
    SDL_Rect destRect;
    float realW, realH;
    
    x += s_drawOffsetX;
    y += s_drawOffsetY;
    
    realW = (float)(texRect->w * xScaleFactor);
    realH = (float)(texRect->h * yScaleFactor);
    
    destRect.x = (int)((x - (cx * xScaleFactor)) * 1024);
    destRect.y = (int)((y - (cy * yScaleFactor)) * 1024);
    destRect.w = (int)(realW * 1024);
    destRect.h = (int)(realH * 1024);
    
    angle *= 180.0 / M_PI;
    
    SDL_RenderCopyEx(PL_renderer, texture, texRect, &destRect,
                     angle, NULL, turn ? SDL_FLIP_HORIZONTAL : 0);
    
    return 0;
}


int PL_Draw_RotaGraphF(float x, float y, 
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    return s_Draw_RotaGraphMain(texture, &texRect, x, y,
                                (float)(texRect.w / 2), (float)(texRect.h / 2),
                                scaleFactor, scaleFactor, angle,
                                graphID, blendFlag, turn);
}
int PL_Draw_RotaGraph(int x, int y, 
                      double scaleFactor, double angle,
                      int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraphF((float)x, (float)y, scaleFactor, angle,
                              graphID, blendFlag, turn);
}

int PL_Draw_RotaGraph2F(float x, float y, float cx, float cy,
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    return s_Draw_RotaGraphMain(texture, &texRect, x, y,
                                cx, cy,
                                scaleFactor, scaleFactor, angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RotaGraph2(int x, int y, int cx, int cy,
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph2F(
                (float)x, (float)y, (float)cx, (float)cy,
                scaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RotaGraph3F(float x, float y, float cx, float cy,
                       double xScaleFactor, double yScaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    return s_Draw_RotaGraphMain(texture, &texRect, x, y,
                                cx, cy,
                                xScaleFactor, yScaleFactor, angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RotaGraph3(int x, int y, int cx, int cy,
                       double xScaleFactor, double yScaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph3F(
                (float)x, (float)y, (float)cx, (float)cy,
                xScaleFactor, yScaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RectRotaGraphF(float x, float y,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    texRect.x += sx;
    texRect.y += sy;
    texRect.w = sw;
    texRect.h = sh;
    
    return s_Draw_RotaGraphMain(texture, &texRect, x, y,
                                (float)(texRect.w / 2), (float)(texRect.h / 2),
                                scaleFactor, scaleFactor, angle,
                                graphID, blendFlag, turn);
}
int PL_Draw_RectRotaGraph(int x, int y, 
                          int sx, int sy, int sw, int sh,
                          double scaleFactor, double angle,
                          int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraphF((float)x, (float)y, sx, sy, sw, sh,
                                  scaleFactor, angle,
                                  graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph2F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    texRect.x += sx;
    texRect.y += sy;
    texRect.w = sw;
    texRect.h = sh;
    
    return s_Draw_RotaGraphMain(texture, &texRect, x, y,
                                cx, cy,
                                scaleFactor, scaleFactor, angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph2(int x, int y, int cx, int cy,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph2F(
                (float)x, (float)y, sx, sy, sw, sh, (float)cx, (float)cy,
                scaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_RectRotaGraph3F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double xScaleFactor, double yScaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }
    
    texRect.x += sx;
    texRect.y += sy;
    texRect.w = sw;
    texRect.h = sh;
    
    return s_Draw_RotaGraphMain(texture, &texRect, x, y,
                                cx, cy,
                                xScaleFactor, yScaleFactor, angle,
                                graphID, blendFlag, turn);
}

int PL_Draw_RectRotaGraph3(int x, int y,
                           int sx, int sy, int sw, int sh,
                           int cx, int cy,
                           double xScaleFactor, double yScaleFactor, double angle,
                           int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph3F(
                (float)x, (float)y, sx, sy, sw, sh, (float)cx, (float)cy,
                xScaleFactor, yScaleFactor, angle,
                graphID, blendFlag, turn
           );
}

int PL_Draw_TurnGraphF(float x, float y, int graphID, int blendFlag) {
    SDL_Rect destRect;
    SDL_Texture *texture;
    SDL_Rect texRect;
    
    if (s_InitTexture(graphID, &texture, &texRect, blendFlag) < 0) {
        return -1;
    }

    x += s_drawOffsetX;
    y += s_drawOffsetY;
    
    destRect.x = (int)(x * 1024);
    destRect.y = (int)(y * 1024);
    destRect.w = texRect.w * 1024;
    destRect.h = texRect.h * 1024;
    
    SDL_RenderCopyEx(PL_renderer, texture, &texRect, &destRect,
                     0, NULL, SDL_FLIP_HORIZONTAL);
    
    return 0;
}

int PL_Draw_TurnGraph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_TurnGraphF((float)x, (float)y, graphID, blendFlag);
}

int PL_Draw_SetDrawArea(int x1, int y1, int x2, int y2) {
    SDL_Rect rect;
    
    if (x1 < 0) { x1 = 0; }
    if (y1 < 0) { y1 = 0; }
    if (x2 > s_windowWidth) { x2 = s_windowWidth; }
    if (y2 > s_windowHeight) { y2 = s_windowHeight; }
    if (x1 > x2) { x2 = x1; }
    if (y1 > y2) { y2 = y1; }
    
    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1;
    rect.h = y2 - y1;
    
    s_drawOffsetX = -(float)x1;
    s_drawOffsetY = -(float)y1;
    
    SDL_RenderSetScale(PL_renderer, 1.0f, 1.0f);
    
    SDL_RenderSetViewport(PL_renderer, &rect);
    
    SDL_RenderSetScale(PL_renderer, 1.0f/1024.0f, 1.0f/1024.0f);
    
    return 0;
}

int PL_Draw_SetDrawBlendMode(int blendMode, int alpha) {
    switch(blendMode) {
        case DX_BLENDMODE_ALPHA: s_blendMode = SDL_BLENDMODE_BLEND; break;
        case DX_BLENDMODE_ADD: s_blendMode = SDL_BLENDMODE_ADD; break;
        case DX_BLENDMODE_MUL: s_blendMode = SDL_BLENDMODE_MOD; break;
        
        default: /* DX_BLENDMODE_NOBLEND */
            s_blendMode = SDL_BLENDMODE_NONE;
            break;
    }
    
    s_drawAlpha = (Uint8)alpha;
    
    return 0;
}

int PL_Draw_SetBright(int redBright, int greenBright, int blueBright) {
    s_drawColorR = (Uint8)redBright;
    s_drawColorG = (Uint8)greenBright;
    s_drawColorB = (Uint8)blueBright;
    
    return 0;
}

int PL_Draw_GetBright(int *redBright, int *greenBright, int *blueBright) {
    *redBright = s_drawColorR;
    *greenBright = s_drawColorG;
    *blueBright = s_drawColorB;
    
    return 0;
}

int PL_Draw_SetBasicBlendFlag(int blendFlag) {
    /* Reseved for software renderer, so it won't be used at all. */
    return 0;
}

DXCOLOR PL_Draw_GetColor(int red, int green, int blue) {
    return red | (green << 8) | (blue << 16);
}

/* ------------------------------------------------------------------------
 * Management of SDL_Renderer stuff goes here.
 */

static SDL_Texture *s_backbufferTextureA = NULL;
static SDL_Texture *s_backbufferTextureB = NULL;

static void s_DestroyBackbuffer() {
    if (s_backbufferTextureA != NULL) {
        SDL_DestroyTexture(s_backbufferTextureA);
    }
    if (s_backbufferTextureB != NULL) {
        SDL_DestroyTexture(s_backbufferTextureB);
    }
}

void PL_Draw_ResizeWindow(int width, int height) {
    s_DestroyBackbuffer();
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    
    s_backbufferTextureA =
        SDL_CreateTexture(
            PL_renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            width, height
        );
    s_backbufferTextureB =
        SDL_CreateTexture(
            PL_renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            width, height
        );
    
    s_windowWidth = width;
    s_windowHeight = height;
    
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    
    SDL_SetRenderTarget(PL_renderer, s_backbufferTextureA);
    SDL_RenderSetViewport(PL_renderer, NULL);
    
    SDL_RenderClear(PL_renderer);
    
    SDL_RenderSetScale(PL_renderer, 1.0f/1024.0f, 1.0f/1024.0f);
}

void PL_Draw_Refresh(SDL_Window *window, const SDL_Rect *targetRect) {
    SDL_SetRenderTarget(PL_renderer, NULL);
    SDL_RenderSetViewport(PL_renderer, NULL);
    SDL_RenderSetClipRect(PL_renderer, NULL);
    SDL_SetRenderDrawColor(PL_renderer, 0, 0, 0, 255);
    SDL_RenderClear(PL_renderer);
    SDL_RenderCopy(PL_renderer, s_backbufferTextureB, NULL, targetRect);
    
    SDL_RenderPresent(PL_renderer);
    
    SDL_SetRenderTarget(PL_renderer, s_backbufferTextureA);
    SDL_RenderSetViewport(PL_renderer, NULL);
    
    SDL_RenderClear(PL_renderer);
    
    SDL_RenderSetScale(PL_renderer, 1.0f/1024.0f, 1.0f/1024.0f);
}

void PL_Draw_SwapBuffers(SDL_Window *window, const SDL_Rect *targetRect) {
    SDL_Texture *backbuffer;
    
    /* Swap backbuffers. */
    backbuffer = s_backbufferTextureA;
    s_backbufferTextureA = s_backbufferTextureB;
    s_backbufferTextureB = backbuffer;
    
    /* Finish the current frame by drawing the backbuffer to the screen. */
    PL_Draw_Refresh(targetRect);
}

void PL_Draw_Init(SDL_Window *window, int width, int height, int vsyncFlag) {
    Uint32 rendererFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;
    rendererFlags |= SDL_RENDERER_TARGETTEXTURE;
    
    if (vsyncFlag == DXTRUE) {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }
    
    /* Prefer OpenGL driver over D3D when available.
     * SDL's D3D driver seems a bit buggy...
     */
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    
    PL_renderer = SDL_CreateRenderer(window, -1, rendererFlags);
    
    PL_Draw_ResizeWindow(width, height);
    
    PL_Draw_SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    PL_Draw_SetBright(0xff, 0xff, 0xff);
}

void PL_Draw_End() {
    if (s_builtGraphs == DXTRUE) {
        PL_Graph_Delete(s_circleGraphID);
        s_circleGraphID = -1;
        
        s_builtGraphs = DXFALSE;
    }
    
    PL_Graph_End();
    
    s_DestroyBackbuffer();
    
    SDL_DestroyRenderer(PL_renderer);
    PL_renderer = NULL;
}

#endif /* #ifdef DXPORTLIB_DRAW_SDL2_RENDER */
