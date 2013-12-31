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

#ifdef DXPORTLIB_DRAW_OPENGL

#include "OpenGL_DxInternal.h"

int PL_Draw_ResetSettings() {
    return 0;
}

int PL_Draw_FlushCache() {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_LineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    /* FIXME WRITEME */
    return 0;
}
int PL_Draw_Line(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness) {
    return PL_Draw_LineF((float)x1, (float)y1, (float)x2, (float)y2, color, thickness);
}

int PL_Draw_OvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag) {
    /* FIXME WRITEME */
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
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_Box(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag) {
    return PL_Draw_BoxF((float)x1, (float)y1, (float)x2, (float)y2, color, FillFlag);
}

int PL_Draw_GraphF(float x, float y, int graphID, int blendFlag) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_Graph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_GraphF((float)x, (float)y, graphID, blendFlag);
}

int PL_Draw_ExtendGraphF(float x1, float y1, float x2, float y2, int graphID, int blendFlag) {
    /* FIXME WRITEME */
    return 0;
}
int PL_Draw_ExtendGraph(int x1, int y1, int x2, int y2, int graphID, int blendFlag) {
    return PL_Draw_ExtendGraphF((float)x1, (float)y1, (float)x2, (float)y2, graphID, blendFlag);
}

int PL_Draw_RectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                      int graphID, int blendFlag, int turnFlag) {
    /* FIXME WRITEME */
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
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_RectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                             int sx, int sy, int sw, int sh,
                             int graphID, int blendFlag, int turnFlag) {
    /* FIXME WRITEME */
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
    /* FIXME WRITEME */
    return 0;
}


int PL_Draw_RotaGraphF(float x, float y, 
                       double scaleFactor, double angle,
                       int graphID, int blendFlag, int turn) {
    /* FIXME WRITEME */
    return 0;
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
    /* FIXME WRITEME */
    return 0;
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
    /* FIXME WRITEME */
    return 0;
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
    /* FIXME WRITEME */
    return 0;
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
    /* FIXME WRITEME */
    return 0;
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
    /* FIXME WRITEME */
    return 0;
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
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_TurnGraph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_TurnGraphF((float)x, (float)y, graphID, blendFlag);
}

int PL_Draw_SetDrawArea(int x1, int y1, int x2, int y2) {
    PL_Draw_FlushCache();
    
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_SetDrawBlendMode(int blendMode, int alpha) {
    PL_Draw_FlushCache();
    
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_SetBright(int redBright, int greenBright, int blueBright) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_GetBright(int *redBright, int *greenBright, int *blueBright) {
    /* FIXME WRITEME */
    return 0;
}

int PL_Draw_SetBasicBlendFlag(int blendFlag) {
    /* Reseved for software renderer, so it won't be used at all. */
    return 0;
}

DXCOLOR PL_Draw_GetColor(int red, int green, int blue) {
    return red | (green << 8) | (blue << 16);
}

#endif /* #ifdef DXPORTLIB_DRAW_OPENGL */
