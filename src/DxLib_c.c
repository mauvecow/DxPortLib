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

#include "DxLib_c.h"

#include "DxInternal.h"

#include "SDL.h"
#include "SDL_opengl.h"

/* DxLib_c.c is C bindings to the core internal library. */

static int s_calledMainReady = DXFALSE;
static int s_initialized = DXFALSE;

int DxLib_DxLib_Init(void) {
    if (s_initialized == DXTRUE) {
        return 0;
    }
    
    if (s_calledMainReady == DXFALSE) {
        s_calledMainReady = DXTRUE;
        SDL_SetMainReady();
    }
    
    SDL_Init(
        SDL_INIT_VIDEO
        | SDL_INIT_TIMER
#ifndef DX_NON_INPUT
        | SDL_INIT_JOYSTICK
        | SDL_INIT_GAMECONTROLLER
#endif  /* #ifndef DX_NON_INPUT */
#ifndef DX_NON_SOUND
        | SDL_INIT_AUDIO
#endif  /* #ifndef DX_NON_SOUND */
    );
    
    PL_Handle_Init();
    PL_File_Init();
#ifndef DX_NON_INPUT
    PL_Input_Init();
#endif  /* #ifndef DX_NON_INPUT */
    PL_Window_Init();
#ifndef DX_NON_SOUND
    PL_Audio_Init();
#endif /* #ifndef DX_NON_SOUND */
#ifndef DX_NON_FONT
    PL_Font_Init();
#endif /* #ifndef DX_NON_FONT */
    
    s_initialized = DXTRUE;
    
    return 0;
}
int DxLib_DxLib_End(void) {
    if (s_initialized == DXFALSE) {
        return 0;
    }
    
#ifndef DX_NON_FONT
    PL_Font_End();
#endif /* #ifndef DX_NON_FONT */
#ifndef DX_NON_SOUND
    PL_Audio_End();
#endif /* #ifndef DX_NON_SOUND */
    PL_Window_End();
#ifndef DX_NON_INPUT
    PL_Input_End();
#endif /* #ifndef DX_NON_INPUT */
    PL_File_End();
    PL_Handle_End();
    
    SDL_Quit();
    
    s_initialized = DXFALSE;
    
    return 0;
}

int DxLib_GlobalStructInitialize(void) {
    if (s_initialized == DXTRUE) {
        return -1;
    }
    
    PL_Window_ResetSettings();
    PL_Draw_ResetSettings();
    PL_Graph_ResetSettings();
#ifndef DX_NON_SOUND
    PL_Audio_ResetSettings();
#endif
    
    return 0;
}
int DxLib_IsInit(void) {
    return s_calledMainReady ? 1 : 0;
}

int DxLib_ProcessMessage(void) {
    return PL_Window_ProcessMessages();
}

int DxLib_WaitTimer(int msTime) {
    SDL_Delay((Uint32)msTime);
    
    return 0;
}

int DxLib_WaitKey() {
#ifdef DX_NON_INPUT
    return 0;
#else
    /* DxLib waits for any key to be pressed,
     * and then it waits for all keys to be released. */
    int lastHitState = 0;
    
    while (DxLib_ProcessMessage() == 0) {
        int hitState = DxLib_CheckHitKeyAll(DX_CHECKINPUT_ALL);
        if (hitState != lastHitState) {
            if (lastHitState != 0 || hitState < 0) {
                break;
            }
            
            lastHitState = hitState;
        }
        
        SDL_Delay(1);
    }
    
    return 0;
#endif
}

int DxLib_GetNowCount() {
    return (int)SDL_GetTicks();
}

int DxLib_GetRand(int maxValue) {
    return PL_Random_Get(maxValue);
}
int DxLib_SRand(int randomSeed) {
    return PL_Random_Seed(randomSeed);
}

int DxLib_SetOutApplicationLogValidFlag(int logFlag) {
    return 0;
}

int DxLib_SetDeleteHandleFlag(int handleID, int *flag) {
    return PL_Handle_SetDeleteFlag(handleID, flag);
}

int DxLib_SetUseCharSet(int charset) {
    return PL_Text_SetUseCharSet(charset);
}

/* ---------------------------------------------------- DxFile.cpp */
int DxLib_FileRead_open(const DXCHAR *filename) {
    return PL_FileRead_open(filename);
}

long long DxLib_FileRead_size(int fileHandle) {
    return PL_FileRead_size(fileHandle);
}

int DxLib_FileRead_close(int fileHandle) {
    return PL_FileRead_close(fileHandle);
}

long long DxLib_FileRead_tell(int fileHandle) {
    return PL_FileRead_tell(fileHandle);
}

int DxLib_FileRead_seek(int fileHandle, long long position, int origin) {
    return PL_FileRead_seek(fileHandle, position, origin);
}

int DxLib_FileRead_read(void *data, int size, int fileHandle) {
    return PL_FileRead_read(data, size, fileHandle);
}

int DxLib_FileRead_eof(int fileHandle) {
    return PL_FileRead_eof(fileHandle);
}

/* ---------------------------------------------------- DxArchive.cpp */
/* (actually DxFile) */
int DxLib_SetUseDXArchiveFlag(int flag) {
    PL_File_SetUseDXArchiveFlag(flag);
    return 0;
}
int DxLib_SetDXArchiveKeyString(const DXCHAR *keyString) {
    return PL_File_SetDXArchiveKeyString(keyString);
}
int DxLib_SetDXArchiveExtension(const DXCHAR *extension) {
    return PL_File_SetDXArchiveExtension(extension);
}
int DxLib_SetDXArchivePriority(int flag) {
    return PL_File_SetDXArchivePriority(flag);
}
int DxLib_DXArchivePreLoad(const DXCHAR *dxaFilename, int async) {
    return PL_File_DXArchivePreLoad(dxaFilename, async);
}
int DxLib_DXArchiveCheckIdle(const DXCHAR *dxaFilename) {
    return PL_File_DXArchiveCheckIdle(dxaFilename);
}
int DxLib_DXArchiveRelease(const DXCHAR *dxaFilename) {
    return PL_File_DXArchiveRelease(dxaFilename);
}
int DxLib_DXArchiveCheckFile(const DXCHAR *dxaFilename, const DXCHAR *filename) {
    return PL_File_DXArchiveCheckFile(dxaFilename, filename);
}

/* ---------------------------------------------------- DxInput.cpp */
#ifndef DX_NON_INPUT

int DxLib_CheckHitKey(int keyCode) {
    return PL_Input_CheckHitKey(keyCode);
}
int DxLib_CheckHitKeyAll(int checkType) {
    return PL_Input_CheckHitKeyAll(checkType);
}
int DxLib_GetHitKeyStateAll(char *table) {
    return PL_Input_GetHitKeyStateAll(table);
}
int DxLib_GetJoypadNum() {
    return PL_Input_GetJoypadNum();
}
int DxLib_GetJoypadInputState(int controllerIndex) {
    return PL_Input_GetJoypadState(controllerIndex);
}
int DxLib_GetJoypadDirectInputState(int controllerIndex, DINPUT_JOYSTATE *state) {
    return PL_Input_GetJoypadDirectInputState(controllerIndex, state);
}
int DxLib_GetJoypadXInputState(int controllerIndex, XINPUT_STATE *state) {
    return PL_Input_GetJoypadXInputState(controllerIndex, state);
}

int DxLib_SetJoypadInputToKeyInput(int controllerIndex, int input,
                             int key1, int key2,
                             int key3, int key4) {
    return PL_Input_SetJoypadInputToKeyInput(
        controllerIndex, input, key1, key2, key3, key4);
}

int DxLib_GetMousePoint(int *xPosition, int *yPosition) {
    return PL_Window_GetMousePoint(xPosition, yPosition);
}
int DxLib_SetMousePoint(int xPosition, int yPosition) {
    return PL_Window_SetMousePoint(xPosition, yPosition);
}
int DxLib_GetMouseInput() {
    return PL_Window_GetMouseInput();
}

int DxLib_GetMouseWheelRotVol(int clearFlag) {
    return PL_Input_GetMouseWheelRotVol(clearFlag);
}
int DxLib_GetMouseHWheelRotVol(int clearFlag) {
    return PL_Input_GetMouseHWheelRotVol(clearFlag);
}
/* SDL2 always returns non-floating values, so... */
float DxLib_GetMouseWheelRotVolF(int clearFlag) {
    return (float)PL_Input_GetMouseWheelRotVol(clearFlag);
}
float DxLib_GetMouseHWheelRotVolF(int clearFlag) {
    return (float)PL_Input_GetMouseHWheelRotVol(clearFlag);
}
#endif /* #ifndef DX_NON_INPUT */

/* ---------------------------------------------------- DxWindow.cpp */
int DxLib_SetGraphMode(int width, int height, int bitDepth, int FPS) {
    PL_Window_SetDimensions(width, height, bitDepth, FPS);
    return 0;
}
int DxLib_SetWindowSizeChangeEnableFlag(int windowResizeFlag) {
    PL_Window_SetWindowResizeFlag(windowResizeFlag);
    return 0;
}
int DxLib_SetWindowText(const DXCHAR *windowName) {
    PL_Window_SetTitle(windowName);
    return 0;
}
int DxLib_ScreenFlip() {
    PL_Window_SwapBuffers();
    return 0;
}
int DxLib_ChangeWindowMode(int fullscreenFlag) {
    PL_Window_SetFullscreen(fullscreenFlag ? 0 : 1);
    return 0;
}
int DxLib_SetDrawScreen(int flag) {
    /* This sets if we render to back or front buffer,
     * but there is no front buffer so we ignore this.
     */
    return 0;
}

int DxLib_EXT_SetIconImageFile(const DXCHAR *filename) {
    return PLEXT_Window_SetIconImageFile(filename);
}

int DxLib_SetMouseDispFlag(int flag) {
    return PL_Window_SetMouseDispFlag(flag);
}
int DxLib_GetMouseDispFlag() {
    return PL_Window_GetMouseDispFlag();
}
int DxLib_SetWaitVSyncFlag(int flag) {
    return PL_Window_SetWaitVSyncFlag(flag);
}
int DxLib_GetWaitVSyncFlag() {
    return PL_Window_GetWaitVSyncFlag();
}
int DxLib_SetAlwaysRunFlag(int flag) {
    return PL_Window_SetAlwaysRunFlag(flag);
}
int DxLib_GetAlwaysRunFlag() {
    return PL_Window_GetAlwaysRunFlag();
}

/* ---------------------------------------------------- DxGraphics.cpp */

int DxLib_LoadGraph(const DXCHAR *name) {
    return PL_Graph_Load(name);
}
int DxLib_DeleteGraph(int graphID) {
    return PL_Graph_Delete(graphID);
}
int DxLib_InitGraph() {
    return PL_Graph_InitGraph();
}
int DxLib_DerivationGraph(int x, int y, int w, int h, int graphID) {
    return PL_Graph_Derivation(x, y, w, h, graphID);
}

int DxLib_GetGraphSize(int graphID, int *width, int *height) {
    return PL_Graph_GetSize(graphID, width, height);
}

int DxLib_SetTransColor(int r, int g, int b) {
    return PL_Graph_SetTransColor(r, g, b);
}
int DxLib_GetTransColor(int *r, int *g, int *b) {
    return PL_Graph_GetTransColor(r, g, b);
}
int DxLib_SetUseTransColor(int flag) {
    return PL_Graph_SetUseTransColor(flag);
}

int DxLib_DrawPixel(int x, int y, DXCOLOR color) {
    return PL_Draw_Pixel(x, y, color);
}

int DxLib_DrawLine(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness) {
    return PL_Draw_Line(x1, y1, x2, y2, color, thickness);
}
int DxLib_DrawLineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    return PL_Draw_LineF(x1, y1, x2, y2, color, thickness);
}

int DxLib_DrawBox(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag) {
    return PL_Draw_Box(x1, y1, x2, y2, color, FillFlag);
}
int DxLib_DrawBoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag) {
    return PL_Draw_BoxF(x1, y1, x2, y2, color, FillFlag);
}
int DxLib_DrawFillBox(int x1, int y1, int x2, int y2, DXCOLOR color) {
    return PL_Draw_Box(x1, y1, x2, y2, color, DXTRUE);
}
int DxLib_DrawFillBoxF(float x1, float y1, float x2, float y2, DXCOLOR color) {
    return PL_Draw_BoxF(x1, y1, x2, y2, color, DXTRUE);
}
int DxLib_DrawLineBox(int x1, int y1, int x2, int y2, DXCOLOR color) {
    return PL_Draw_Box(x1, y1, x2, y2, color, DXFALSE);
}
int DxLib_DrawLineBoxF(float x1, float y1, float x2, float y2, DXCOLOR color) {
    return PL_Draw_BoxF(x1, y1, x2, y2, color, DXFALSE);
}

int DxLib_DrawCircle(int x, int y, int r, DXCOLOR color, int fillFlag) {
    return PL_Draw_Circle(x, y, r, color, fillFlag);
}
int DxLib_DrawCircleF(float x, float y, float r, DXCOLOR color, int fillFlag) {
    return PL_Draw_CircleF(x, y, r, color, fillFlag);
}
int DxLib_DrawOval(int x, int y, int rx, int ry, DXCOLOR color, int fillFlag) {
    return PL_Draw_Oval(x, y, rx, ry, color, fillFlag);
}
int DxLib_DrawOvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag) {
    return PL_Draw_OvalF(x, y, rx, ry, color, fillFlag);
}

int DxLib_DrawTriangle(int x1, int y1, int x2, int y2,
                       int x3, int y3,
                       DXCOLOR color, int fillFlag) {
    return PL_Draw_Triangle(x1, y1, x2, y2, x3, y3, color, fillFlag);
}
int DxLib_DrawTriangleF(float x1, float y1, float x2, float y2,
                        float x3, float y3,
                        DXCOLOR color, int fillFlag) {
    return PL_Draw_TriangleF(x1, y1, x2, y2, x3, y3, color, fillFlag);
}
int DxLib_DrawQuadrangle(int x1, int y1, int x2, int y2,
                         int x3, int y3, int x4, int y4,
                         DXCOLOR color, int fillFlag) {
    return PL_Draw_Quadrangle(x1, y1, x2, y2, x3, y3, x4, y4, color, fillFlag);
}
int DxLib_DrawQuadrangleF(float x1, float y1, float x2, float y2,
                          float x3, float y3, float x4, float y4,
                          DXCOLOR color, int fillFlag) {
    return PL_Draw_QuadrangleF(x1, y1, x2, y2, x3, y3, x4, y4, color, fillFlag);
}

int DxLib_DrawGraph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_Graph(x, y, graphID, blendFlag);
}
int DxLib_DrawGraphF(float x, float y, int graphID, int blendFlag) {
    return PL_Draw_GraphF(x, y, graphID, blendFlag);
}
int DxLib_DrawExtendGraph(int x1, int y1, int x2, int y2,
                    int graphID, int blendFlag) {
    return PL_Draw_ExtendGraph(x1, y1, x2, y2,
                               graphID, blendFlag);
}
int DxLib_DrawExtendGraphF(float x1, float y1, float x2, float y2,
                    int graphID, int blendFlag) {
    return PL_Draw_ExtendGraphF(x1, y1, x2, y2,
                               graphID, blendFlag);
}

int DxLib_DrawRectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                  int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectGraph(dx, dy,
                             sx, sy, sw, sh,
                             graphID, blendFlag, turnFlag);
}
int DxLib_DrawRectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                  int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectGraphF(dx, dy,
                             sx, sy, sw, sh,
                             graphID, blendFlag, turnFlag);
}
                     
int DxLib_DrawRectExtendGraph(int dx1, int dy1, int dx2, int dy2,
                        int sx, int sy, int sw, int sh,
                        int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectExtendGraph(dx1, dy1, dx2, dy2,
                                   sx, sy, sw, sh,
                                   graphID, blendFlag, turnFlag);
}
int DxLib_DrawRectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                        int sx, int sy, int sw, int sh,
                        int graphID, int blendFlag, int turnFlag) {
    return PL_Draw_RectExtendGraphF(dx1, dy1, dx2, dy2,
                                   sx, sy, sw, sh,
                                   graphID, blendFlag, turnFlag);
}
                     
int DxLib_DrawRotaGraph(int x, int y,
                  double scaleFactor, double angle,
                  int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph(x, y,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraphF(float x, float y,
                  double scaleFactor, double angle,
                  int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraphF(x, y,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph2(int x, int y, int cx, int cy,
                   double scaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph2(x, y, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph2F(float x, float y, float cx, float cy,
                   double scaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph2F(x, y, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph3(int x, int y, int cx, int cy,
                   double xScaleFactor, double yScaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph3(x, y, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph3F(float x, float y, float cx, float cy,
                   double xScaleFactor, double yScaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return PL_Draw_RotaGraph3F(x, y, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}

int DxLib_DrawRectRotaGraph(int x, int y,
                            int sx, int sy, int sw, int sh,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph(x, y, sx, sy, sw, sh,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraphF(float x, float y,
                             int sx, int sy, int sw, int sh,
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraphF(x, y, sx, sy, sw, sh,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph2(int x, int y,
                             int sx, int sy, int sw, int sh,
                             int cx, int cy,
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph2(x, y, sx, sy, sw, sh, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph2F(float x, float y,
                              int sx, int sy, int sw, int sh,
                              float cx, float cy,
                              double scaleFactor, double angle,
                              int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph2F(x, y, sx, sy, sw, sh, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph3(int x, int y,
                             int sx, int sy, int sw, int sh,
                             int cx, int cy, 
                             double xScaleFactor, double yScaleFactor,
                             double angle,
                             int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph3(x, y, sx, sy, sw, sh, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph3F(float x, float y,
                              int sx, int sy, int sw, int sh,
                              float cx, float cy,
                              double xScaleFactor, double yScaleFactor,
                              double angle,
                              int graphID, int blendFlag, int turn) {
    return PL_Draw_RectRotaGraph3F(x, y, sx, sy, sw, sh, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}

int DxLib_DrawTurnGraph(int x, int y, int graphID, int blendFlag) {
    return PL_Draw_TurnGraph(x, y, graphID, blendFlag);
}
int DxLib_DrawTurnGraphF(float x, float y, int graphID, int blendFlag) {
    return PL_Draw_TurnGraphF(x, y, graphID, blendFlag);
}

int DxLib_DrawModiGraph(int x1, int y1, int x2, int y2,
                        int x3, int y3, int x4, int y4,
                        int graphID, int blendFlag) {
    return PL_Draw_ModiGraph(x1, y1, x2, y2, x3, y3, x4, y4, graphID, blendFlag);
}
int DxLib_DrawModiGraphF(float x1, float y1, float x2, float y2,
                         float x3, float y3, float x4, float y4,
                         int graphID, int blendFlag) {
    return PL_Draw_ModiGraphF(x1, y1, x2, y2, x3, y3, x4, y4, graphID, blendFlag);
}

int DxLib_SetDrawArea(int x1, int y1, int x2, int y2) {
    return PL_Draw_SetDrawArea(x1, y1, x2, y2);
}

int DxLib_SetDrawBlendMode(int blendMode, int alpha) {
    return PL_Draw_SetDrawBlendMode(blendMode, alpha);
}
int DxLib_SetDrawBright(int redBright, int greenBright, int blueBright) {
    return PL_Draw_SetBright(redBright, greenBright, blueBright);
}

int DxLib_SetBasicBlendFlag(int blendFlag) {
    return PL_Draw_SetBasicBlendFlag(blendFlag);
}

DXCOLOR DxLib_GetColor(int red, int green, int blue) {
    return PL_Draw_GetColor(red, green, blue);
}

/* ---------------------------------------------------- DxFont.cpp */
#ifndef DX_NON_FONT

int DxLib_EXT_MapFontFileToName(const DXCHAR *filename,
                                const DXCHAR *fontname,
                                int thickness,
                                int boldFlag
                               ) {
    return PLEXT_Font_MapFontFileToName(filename,
                                   fontname, thickness, boldFlag);
}
int DxLib_EXT_InitFontMappings() {
    return PLEXT_Font_InitFontMappings();
}

/* Handle font functions */
int DxLib_DrawStringToHandle(int x, int y, const DXCHAR *text,
                             DXCOLOR color, int fontHandle, DXCOLOR edgeColor) {
    return PL_Font_DrawStringToHandle(x, y, text, color, fontHandle, edgeColor);
}
int DxLib_DrawFormatStringToHandle(
    int x, int y, DXCOLOR color, int fontHandle,
    const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_DrawFormatStringToHandle(x, y, color, fontHandle, formatString, args);
    va_end(args);
    return retval;
}
int DxLib_DrawExtendStringToHandle(int x, int y, double ExRateX, double ExRateY,
                                   const DXCHAR *text,
                                   DXCOLOR color, int fontHandle, DXCOLOR edgeColor) {
    return PL_Font_DrawExtendStringToHandle(x, y, ExRateX, ExRateY, text, color, fontHandle, edgeColor);
}
int DxLib_DrawExtendFormatStringToHandle(
    int x, int y, double ExRateX, double ExRateY, DXCOLOR color, int fontHandle,
    const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_DrawExtendFormatStringToHandle(x, y, ExRateX, ExRateY, color, fontHandle, formatString, args);
    va_end(args);
    return retval;
}

int DxLib_GetDrawStringWidthToHandle(const DXCHAR *string, int strLen, int fontHandle) {
    return PL_Font_GetDrawStringWidthToHandle(string, strLen, fontHandle);
}
int DxLib_GetDrawFormatStringWidthToHandle(
    int fontHandle, const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_GetDrawFormatStringWidthToHandle(fontHandle, formatString, args);
    va_end(args);
    return retval;
}
int DxLib_GetDrawExtendStringWidthToHandle(double ExRateX, const DXCHAR *string, int strLen, int fontHandle) {
    return PL_Font_GetDrawExtendStringWidthToHandle(ExRateX, string, strLen, fontHandle);
}
int DxLib_GetDrawExtendFormatStringWidthToHandle(
    double ExRateX, int fontHandle, const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_GetDrawExtendFormatStringWidthToHandle(ExRateX, fontHandle, formatString, args);
    va_end(args);
    return retval;
}


int DxLib_GetFontSizeToHandle(int fontHandle) {
    return PL_Font_GetFontSizeToHandle(fontHandle);
}
int DxLib_GetFontCharInfo(int fontHandle, const DXCHAR *string,
                          int *xPos, int *yPos, int *advanceX,
                          int *width, int *height) {
    return PL_Font_GetFontCharInfo(fontHandle, string, xPos, yPos, advanceX, width, height);
}
int DxLib_SetFontSpaceToHandle(int fontSpacing, int fontHandle) {
    return PL_Font_SetFontSpaceToHandle(fontSpacing, fontHandle);
}

int DxLib_CreateFontToHandle(const DXCHAR *fontname,
                       int size, int thickness, int fontType, int charSet,
                       int edgeSize, int Italic) {
    return PL_Font_CreateFontToHandle(
        fontname, size, thickness, fontType, charSet,
        edgeSize, Italic
    );
}
int DxLib_DeleteFontToHandle(int fontHandle) {
    return PL_Font_DeleteFontToHandle(fontHandle);
}
int DxLib_CheckFontHandleValid(int fontHandle) {
    return PL_Font_CheckFontHandleValid(fontHandle);
}
int DxLib_SetFontLostFlag(int fontHandle, int *lostFlag) {
    return PL_Font_SetFontLostFlag(fontHandle, lostFlag);
}

int DxLib_InitFontToHandle() {
    return PL_Font_InitFontToHandle();
}

/* "Default" font functions */
int DxLib_DrawString(int x, int y, const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor) {
    return PL_Font_DrawString(x, y, string, color, edgeColor);
}
int DxLib_DrawFormatString(
    int x, int y, DXCOLOR color,
    const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_DrawFormatString(x, y, color, formatString, args);
    va_end(args);
    return retval;
}
int DxLib_DrawExtendString(int x, int y, double ExRateX, double ExRateY,
                           const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor) {
    return PL_Font_DrawExtendString(x, y, ExRateX, ExRateY, string, color, edgeColor);
}
int DxLib_DrawExtendFormatString(
    int x, int y, double ExRateX, double ExRateY,
    DXCOLOR color,
    const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_DrawExtendFormatString(x, y, ExRateX, ExRateY, color, formatString, args);
    va_end(args);
    return retval;
}

int DxLib_GetDrawStringWidth(const DXCHAR *string, int strLen) {
    return PL_Font_GetDrawStringWidth(string, strLen);
}
int DxLib_GetDrawFormatStringWidth(
    const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_GetDrawFormatStringWidth(formatString, args);
    va_end(args);
    return retval;
}
int DxLib_GetDrawExtendStringWidth(double ExRateX, const DXCHAR *string, int strLen) {
    return PL_Font_GetDrawExtendStringWidth(ExRateX, string, strLen);
}
int DxLib_GetDrawExtendFormatStringWidth(
    double ExRateX, const DXCHAR *formatString, ...
) {
    va_list args;
    int retval;
    va_start(args, formatString);
    retval = PL_Font_GetDrawExtendFormatStringWidth(ExRateX, formatString, args);
    va_end(args);
    return retval;
}

int DxLib_ChangeFont(const DXCHAR *string, int charSet) {
    return PL_Font_ChangeFont(string, charSet);
}
int DxLib_ChangeFontType(int fontType) {
    return PL_Font_ChangeFontType(fontType);
}
int DxLib_SetFontSize(int fontSize) {
    return PL_Font_SetFontSize(fontSize);
}
int DxLib_GetFontSize() {
    return PL_Font_GetFontSize();
}
int DxLib_SetFontThickness(int fontThickness) {
    return PL_Font_SetFontThickness(fontThickness);
}
int DxLib_SetFontSpace(int fontSpace) {
    return PL_Font_SetFontSpace(fontSpace);
}
int DxLib_SetDefaultFontState(const DXCHAR *fontName, int fontSize, int fontThickness) {
    return PL_Font_SetDefaultFontState(fontName, fontSize, fontThickness);
}
int DxLib_GetDefaultFontHandle() {
    return PL_Font_GetDefaultFontHandle();
}

#endif /* #ifndef DX_NON_FONT */

/* ---------------------------------------------------- DxAudio.cpp */
#ifndef DX_NON_SOUND

int DxLib_PlaySoundMem(int soundID, int playType, int startPositionFlag) {
    return PL_PlaySoundMem(soundID, playType, startPositionFlag);
}
int DxLib_StopSoundMem(int soundID) {
    return PL_StopSoundMem(soundID);
}
int DxLib_CheckSoundMem(int soundID) {
    return PL_CheckSoundMem(soundID);
}

int DxLib_SetVolumeSoundMem(int volume, int soundID) {
    return PL_SetVolumeSoundMem(volume, soundID);
}
int DxLib_ChangeVolumeSoundMem(int volume, int soundID) {
    return PL_ChangeVolumeSoundMem(volume, soundID);
}
int DxLib_SetUseOldVolumeCalcFlag(int volumeFlag) {
    return PL_SetUseOldVolumeCalcFlag(volumeFlag);
}

int DxLib_LoadSoundMem(const DXCHAR *filename) {
    return PL_LoadSoundMem(filename);
}
int DxLib_LoadSoundMem2(const DXCHAR *filename, const DXCHAR *filename2) {
    return PL_LoadSoundMem2(filename, filename2);
}
int DxLib_DeleteSoundMem(int soundID) {
    return PL_DeleteSoundMem(soundID);
}
int DxLib_InitSoundMem() {
    return PL_InitSoundMem();
}

#endif /* #ifndef DX_NON_SOUND */

/* ---------------------------------------------------- DxMemory.cpp */

void *DxLib_DxAlloc(size_t allocationSize, const char *file, int line) {
    return PL_DxAlloc(allocationSize, file, line);
}
void *DxLib_DxCalloc(size_t allocationSize, const char *file, int line) {
    return PL_DxCalloc(allocationSize, file, line);
}
void *DxLib_DxRealloc(void *memory, size_t allocationSize, const char *file, int line) {
    return PL_DxRealloc(memory, allocationSize, file, line);
}
void DxLib_DxFree(void *memory) {
    PL_DxFree(memory);
}

