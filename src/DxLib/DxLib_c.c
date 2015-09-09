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

#include "DxLib_c.h"

#include "PL/PLInternal.h"
#include "DxInternal.h"

/* DxLib_c.c is C bindings to the core internal library. */

static int s_calledMainReady = DXFALSE;
static int s_initialized = DXFALSE;

static int s_fullscreenDesktopFlag = DXTRUE;
static int s_realWidth = 640;
static int s_realHeight = 480;
static int s_screenWidth = 640;
static int s_screenHeight = 480;

/* For setting the floating precision to the system value.
 * There is no real architecture agnostic answer here, so
 * add more of these as needed. */
static DXINLINE void s_SetFPUState() {
#if defined(__GNUC__) && defined(i386)
    unsigned int mode;
    
    __asm__ volatile ("fnstcw %0" : "=m" (*&mode));
    
    mode &= ~0x300;
    mode |= 0x200;
    
    __asm__ volatile ("fldcw %0" :: "m" (*&mode));
#endif
}

int DxLib_DxLib_Init(void) {
    if (s_initialized == DXTRUE) {
        return 0;
    }
    
    if (s_calledMainReady == DXFALSE) {
        s_calledMainReady = DXTRUE;
        PL_Platform_Boot();
    }
    
    s_SetFPUState();
    
    PL_Platform_Init();
    
    PL_Handle_Init();
    PL_File_Init();
    Dx_File_Init();
#ifndef DX_NON_INPUT
    PL_Input_Init();
#endif  /* #ifndef DX_NON_INPUT */
    
    PL_Window_Init();
    Dx_Draw_InitCache();
    Dx_Draw_ResetDrawScreen();
    
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
    Dx_File_End();
    PL_Handle_End();
    
    PL_Platform_Finish();
    
    s_initialized = DXFALSE;
    
    return 0;
}

int DxLib_GlobalStructInitialize(void) {
    if (s_initialized == DXTRUE) {
        return -1;
    }
    
    PL_Window_ResetSettings();
    Dx_Draw_ResetSettings();
    Dx_Graph_ResetSettings();
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
    PL_Platform_Wait(msTime);
    
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
        
        PL_Platform_Wait(1);
    }
    
    return 0;
#endif
}

int DxLib_GetDateTime(DATEDATA *dateBuf) {
    return PL_Platform_GetDateTime(dateBuf);
}

int DxLib_GetNowCount(int UseRDTSCFlag) {
    /* FIXME: UseRDTSCFlag is not supported. */
    return (int)PL_Platform_GetTicks();
}

int DxLib_GetRand(int maxValue) {
    return PL_Random_Get(maxValue);
}
int DxLib_SRand(int randomSeed) {
    return PL_Random_SeedDx(randomSeed);
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

void DxLib_EXT_SetOnlyWindowSize(int width, int height, int isFullscreen, int isFullscreenDesktop) {
    int newDesktopFlag;
    if (isFullscreenDesktop) {
        newDesktopFlag = DXTRUE;
    } else {
        newDesktopFlag = DXFALSE;
    }

    s_realWidth = width;
    s_realHeight = height;

    if (isFullscreen) {
        /* Momentarily go to windowed mode if already fullscreen. */
        if (s_fullscreenDesktopFlag != newDesktopFlag
            && PL_Window_GetWindowModeFlag() == DXFALSE)
        {
            PL_Window_SetFullscreen(DXFALSE, s_fullscreenDesktopFlag);
        }

        PL_Window_ChangeOnlyWindowSize(s_screenWidth, s_screenHeight);
        PL_Window_SetFullscreen(DXTRUE, newDesktopFlag);
    } else {
        PL_Window_SetFullscreen(DXFALSE, newDesktopFlag);
        PL_Window_ChangeOnlyWindowSize(width, height);
    }
    s_fullscreenDesktopFlag = newDesktopFlag;
}

/* ---------------------------------------------------- DxFile.cpp */
int DxLib_EXT_FileRead_SetCharSet(int charset) {
    return PLEXT_FileRead_SetCharSet(charset);
}

int DxLib_FileRead_open(const DXCHAR *filename, int ASync) {
    /* FIXME: ASync not supported */
    return Dx_FileRead_open(filename);
}

int64_t DxLib_FileRead_size(const DXCHAR *filename) {
    return Dx_FileRead_size(filename);
}

int DxLib_FileRead_close(int fileHandle) {
    return Dx_FileRead_close(fileHandle);
}

int64_t DxLib_FileRead_tell(int fileHandle) {
    return Dx_FileRead_tell(fileHandle);
}

int DxLib_FileRead_seek(int fileHandle, int64_t position, int origin) {
    return Dx_FileRead_seek(fileHandle, position, origin);
}

int DxLib_FileRead_read(void *data, int size, int fileHandle) {
    return Dx_FileRead_read(data, size, fileHandle);
}

int DxLib_FileRead_eof(int fileHandle) {
    return Dx_FileRead_eof(fileHandle);
}

int DxLib_FileRead_gets(DXCHAR *buffer, int bufferSize, int fileHandle) {
    return Dx_FileRead_gets(buffer, bufferSize, fileHandle);
}
DXCHAR DxLib_FileRead_getc(int fileHandle) {
    return Dx_FileRead_getc(fileHandle);
}
int DxLib_FileRead_scanf(int fileHandle, const DXCHAR *format, ...) {
    va_list args;
    int retval;
    
    va_start(args, format);
    retval = Dx_FileRead_vscanf(fileHandle, format, args);
    va_end(args);
    
    return retval;
}

/* ---------------------------------------------------- DxArchive.cpp */
/* (actually DxFile) */
int DxLib_SetUseDXArchiveFlag(int flag) {
    Dx_File_SetUseDXArchiveFlag(flag);
    return 0;
}
int DxLib_SetDXArchiveKeyString(const DXCHAR *keyString) {
    return Dx_File_SetDXArchiveKeyString(keyString);
}
int DxLib_SetDXArchiveExtension(const DXCHAR *extension) {
    return Dx_File_SetDXArchiveExtension(extension);
}
int DxLib_SetDXArchivePriority(int priority) {
    return Dx_File_SetDXArchivePriority(priority);
}
int DxLib_DXArchivePreLoad(const DXCHAR *dxaFilename, int async) {
    /* FIXME: async is unsupported */
    return Dx_File_DXArchivePreLoad(dxaFilename, async);
}
int DxLib_DXArchiveCheckIdle(const DXCHAR *dxaFilename) {
    return Dx_File_DXArchiveCheckIdle(dxaFilename);
}
int DxLib_DXArchiveRelease(const DXCHAR *dxaFilename) {
    return Dx_File_DXArchiveRelease(dxaFilename);
}
int DxLib_DXArchiveCheckFile(const DXCHAR *dxaFilename, const DXCHAR *filename) {
    return Dx_File_DXArchiveCheckFile(dxaFilename, filename);
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
int DxLib_GetJoypadPOVState(int controllerIndex, int povNumber) {
    return PL_Input_GetJoypadPOVState(controllerIndex, povNumber);
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
int DxLib_SetValidMousePointerWindowOutClientAreaMoveFlag(int flag) {
    return PL_Window_GrabMouse(flag == DXFALSE ? DXTRUE : DXFALSE);
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
    s_screenWidth = width;
    s_screenHeight = height;
    PL_Window_SetDimensions(width, height, bitDepth, FPS);
    return 0;
}
int DxLib_SetWindowSizeChangeEnableFlag(int windowResizeFlag, int fitScreen) {
    /* FIXME: fitScreen is unsupported */
    PL_Window_SetWindowResizeFlag(windowResizeFlag);
    return 0;
}
int DxLib_SetWindowText(const DXCHAR *windowName) {
    PL_Window_SetTitle(windowName);
    return 0;
}
int DxLib_SetMainWindowText(const DXCHAR *windowName) {
    PL_Window_SetTitle(windowName);
    return 0;
}
int DxLib_ScreenFlip() {
    if (s_initialized == DXFALSE) {
        return -1;
    }
    
    Dx_Draw_FlushCache();
    Dx_Draw_ForceUpdate();
    PLG.EndFrame();
    
    PL_Window_SwapBuffers();
    
    Dx_Draw_ResetDrawScreen();
    return 0;
}
int DxLib_ChangeWindowMode(int fullscreenFlag) {
    PL_Window_SetFullscreen(fullscreenFlag ? 0 : 1, DXTRUE);
    return 0;
}
int DxLib_GetWindowModeFlag() {
    return PL_Window_GetWindowModeFlag();
}
int DxLib_SetDrawScreen(int screen) {
    return Dx_Draw_SetDrawScreen(screen);
}
int DxLib_GetDrawScreen() {
    return Dx_Draw_GetDrawScreen();
}
int DxLib_GetActiveGraph() {
    return Dx_Draw_GetDrawScreen();
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
int DxLib_GetWindowActiveFlag() {
    return PL_Window_GetActiveFlag();
}
int DxLib_GetActiveFlag() {
    return PL_Window_GetActiveFlag();
}

int DxLib_EXT_MessageBoxError(const DXCHAR *title, const DXCHAR *text) {
    return PLEXT_Window_MessageBoxError(title, text);
}
int DxLib_EXT_MessageBoxYesNo(const DXCHAR *title, const DXCHAR *text,
                              const DXCHAR *button1, const DXCHAR *button2) {
    return PLEXT_Window_MessageBoxYesNo(title, text, button1, button2);
}

/* ---------------------------------------------------- DxGraphics.cpp */

int DxLib_MakeScreen(int width, int height, int hasAlphaChannel) {
    return Dx_Graph_MakeScreen(width, height, hasAlphaChannel);
}
int DxLib_LoadGraph(const DXCHAR *name, int notUse3DFlag) {
    /* FIXME: notUse3DFlag is unsupported */
    return Dx_Graph_Load(name, DXFALSE);
}
int DxLib_LoadReverseGraph(const DXCHAR *name, int notUse3DFlag) {
    /* FIXME: notUse3DFlag is unsupported */
    return Dx_Graph_Load(name, DXTRUE);
}
int DxLib_LoadDivGraph(const DXCHAR *filename, int graphCount,
                       int xCount, int yCount, int xSize, int ySize,
                       int *handleBuf, int notUse3DFlag) {
    /* FIXME: notUse3DFlag is unsupported */
    return Dx_Graph_LoadDiv(filename, graphCount, xCount, yCount,
                            xSize, ySize, handleBuf,
                            DXFALSE, DXFALSE);
}
int DxLib_LoadDivBmpGraph(const DXCHAR *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int textureFlag, int flipFlag) {
    return Dx_Graph_LoadDiv(filename, graphCount, xCount, yCount,
                            xSize, ySize, handleBuf,
                            textureFlag, flipFlag);
}
int DxLib_LoadReverseDivGraph(const DXCHAR *filename, int graphCount,
                              int xCount, int yCount, int xSize, int ySize,
                              int *handleBuf, int notUse3DFlag) {
    /* FIXME: notUse3DFlag is unsupported */
    return Dx_Graph_LoadDiv(filename, graphCount, xCount, yCount,
                            xSize, ySize, handleBuf,
                            DXFALSE, DXTRUE);
}
int DxLib_DeleteGraph(int graphID, int LogOutFlag) {
    /* FIXME: LogOutFlag is unsupported */
    return Dx_Graph_Delete(graphID);
}
int DxLib_DeleteSharingGraph(int graphID) {
    return Dx_Graph_DeleteSharingGraph(graphID);
}
int DxLib_InitGraph(int LogOutFlag) {
    /* FIXME: LogOutFlag is unsupported */
    return Dx_Graph_InitGraph();
}
int DxLib_DerivationGraph(int x, int y, int w, int h, int graphID) {
    return Dx_Graph_Derivation(x, y, w, h, graphID);
}
int DxLib_GetGraphNum() {
    return Dx_Graph_GetNum();
}

int DxLib_GetGraphSize(int graphID, int *width, int *height) {
    return Dx_Graph_GetSize(graphID, width, height);
}

int DxLib_SetTransColor(int r, int g, int b) {
    return Dx_Graph_SetTransColor(r, g, b);
}
int DxLib_GetTransColor(int *r, int *g, int *b) {
    return Dx_Graph_GetTransColor(r, g, b);
}
int DxLib_SetUseTransColor(int flag) {
    return Dx_Graph_SetUseTransColor(flag);
}

int DxLib_SetUsePremulAlphaConvertLoad(int flag) {
    return Dx_Graph_SetUsePremulAlphaConvertLoad(flag);
}

int DxLib_DrawPixel(int x, int y, DXCOLOR color) {
    return Dx_Draw_Pixel(x, y, color);
}

int DxLib_DrawLine(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness) {
    return Dx_Draw_Line(x1, y1, x2, y2, color, thickness);
}
int DxLib_DrawLineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    return Dx_Draw_LineF(x1, y1, x2, y2, color, thickness);
}

int DxLib_DrawBox(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag) {
    return Dx_Draw_Box(x1, y1, x2, y2, color, FillFlag);
}
int DxLib_DrawBoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag) {
    return Dx_Draw_BoxF(x1, y1, x2, y2, color, FillFlag);
}
int DxLib_DrawFillBox(int x1, int y1, int x2, int y2, DXCOLOR color) {
    return Dx_Draw_Box(x1, y1, x2, y2, color, DXTRUE);
}
int DxLib_DrawFillBoxF(float x1, float y1, float x2, float y2, DXCOLOR color) {
    return Dx_Draw_BoxF(x1, y1, x2, y2, color, DXTRUE);
}
int DxLib_DrawLineBox(int x1, int y1, int x2, int y2, DXCOLOR color) {
    return Dx_Draw_Box(x1, y1, x2, y2, color, DXFALSE);
}
int DxLib_DrawLineBoxF(float x1, float y1, float x2, float y2, DXCOLOR color) {
    return Dx_Draw_BoxF(x1, y1, x2, y2, color, DXFALSE);
}

int DxLib_DrawCircle(int x, int y, int r, DXCOLOR color, int fillFlag) {
    return Dx_Draw_Circle(x, y, r, color, fillFlag);
}
int DxLib_DrawCircleF(float x, float y, float r, DXCOLOR color, int fillFlag) {
    return Dx_Draw_CircleF(x, y, r, color, fillFlag);
}
int DxLib_DrawOval(int x, int y, int rx, int ry, DXCOLOR color, int fillFlag) {
    return Dx_Draw_Oval(x, y, rx, ry, color, fillFlag);
}
int DxLib_DrawOvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag) {
    return Dx_Draw_OvalF(x, y, rx, ry, color, fillFlag);
}

int DxLib_DrawTriangle(int x1, int y1, int x2, int y2,
                       int x3, int y3,
                       DXCOLOR color, int fillFlag) {
    return Dx_Draw_Triangle(x1, y1, x2, y2, x3, y3, color, fillFlag);
}
int DxLib_DrawTriangleF(float x1, float y1, float x2, float y2,
                        float x3, float y3,
                        DXCOLOR color, int fillFlag) {
    return Dx_Draw_TriangleF(x1, y1, x2, y2, x3, y3, color, fillFlag);
}
int DxLib_DrawQuadrangle(int x1, int y1, int x2, int y2,
                         int x3, int y3, int x4, int y4,
                         DXCOLOR color, int fillFlag) {
    return Dx_Draw_Quadrangle(x1, y1, x2, y2, x3, y3, x4, y4, color, fillFlag);
}
int DxLib_DrawQuadrangleF(float x1, float y1, float x2, float y2,
                          float x3, float y3, float x4, float y4,
                          DXCOLOR color, int fillFlag) {
    return Dx_Draw_QuadrangleF(x1, y1, x2, y2, x3, y3, x4, y4, color, fillFlag);
}

int DxLib_DrawGraph(int x, int y, int graphID, int blendFlag) {
    return Dx_Draw_Graph(x, y, graphID, blendFlag);
}
int DxLib_DrawGraphF(float x, float y, int graphID, int blendFlag) {
    return Dx_Draw_GraphF(x, y, graphID, blendFlag);
}
int DxLib_DrawExtendGraph(int x1, int y1, int x2, int y2,
                    int graphID, int blendFlag) {
    return Dx_Draw_ExtendGraph(x1, y1, x2, y2,
                               graphID, blendFlag);
}
int DxLib_DrawExtendGraphF(float x1, float y1, float x2, float y2,
                    int graphID, int blendFlag) {
    return Dx_Draw_ExtendGraphF(x1, y1, x2, y2,
                               graphID, blendFlag);
}

int DxLib_DrawRectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                  int graphID, int blendFlag, int turnFlag) {
    return Dx_Draw_RectGraph(dx, dy,
                             sx, sy, sw, sh,
                             graphID, blendFlag, turnFlag);
}
int DxLib_DrawRectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                  int graphID, int blendFlag, int turnFlag) {
    return Dx_Draw_RectGraphF(dx, dy,
                             sx, sy, sw, sh,
                             graphID, blendFlag, turnFlag);
}
                     
int DxLib_DrawRectExtendGraph(int dx1, int dy1, int dx2, int dy2,
                        int sx, int sy, int sw, int sh,
                        int graphID, int blendFlag, int turnFlag) {
    return Dx_Draw_RectExtendGraph(dx1, dy1, dx2, dy2,
                                   sx, sy, sw, sh,
                                   graphID, blendFlag, turnFlag);
}
int DxLib_DrawRectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                        int sx, int sy, int sw, int sh,
                        int graphID, int blendFlag, int turnFlag) {
    return Dx_Draw_RectExtendGraphF(dx1, dy1, dx2, dy2,
                                   sx, sy, sw, sh,
                                   graphID, blendFlag, turnFlag);
}
                     
int DxLib_DrawRotaGraph(int x, int y,
                  double scaleFactor, double angle,
                  int graphID, int blendFlag, int turn) {
    return Dx_Draw_RotaGraph(x, y,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraphF(float x, float y,
                  double scaleFactor, double angle,
                  int graphID, int blendFlag, int turn) {
    return Dx_Draw_RotaGraphF(x, y,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph2(int x, int y, int cx, int cy,
                   double scaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return Dx_Draw_RotaGraph2(x, y, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph2F(float x, float y, float cx, float cy,
                   double scaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return Dx_Draw_RotaGraph2F(x, y, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph3(int x, int y, int cx, int cy,
                   double xScaleFactor, double yScaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return Dx_Draw_RotaGraph3(x, y, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRotaGraph3F(float x, float y, float cx, float cy,
                   double xScaleFactor, double yScaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return Dx_Draw_RotaGraph3F(x, y, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}

int DxLib_DrawRectRotaGraph(int x, int y,
                            int sx, int sy, int sw, int sh,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    return Dx_Draw_RectRotaGraph(x, y, sx, sy, sw, sh,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraphF(float x, float y,
                             int sx, int sy, int sw, int sh,
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn) {
    return Dx_Draw_RectRotaGraphF(x, y, sx, sy, sw, sh,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph2(int x, int y,
                             int sx, int sy, int sw, int sh,
                             int cx, int cy,
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn) {
    return Dx_Draw_RectRotaGraph2(x, y, sx, sy, sw, sh, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph2F(float x, float y,
                              int sx, int sy, int sw, int sh,
                              float cx, float cy,
                              double scaleFactor, double angle,
                              int graphID, int blendFlag, int turn) {
    return Dx_Draw_RectRotaGraph2F(x, y, sx, sy, sw, sh, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph3(int x, int y,
                             int sx, int sy, int sw, int sh,
                             int cx, int cy, 
                             double xScaleFactor, double yScaleFactor,
                             double angle,
                             int graphID, int blendFlag, int turn) {
    return Dx_Draw_RectRotaGraph3(x, y, sx, sy, sw, sh, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DxLib_DrawRectRotaGraph3F(float x, float y,
                              int sx, int sy, int sw, int sh,
                              float cx, float cy,
                              double xScaleFactor, double yScaleFactor,
                              double angle,
                              int graphID, int blendFlag, int turn) {
    return Dx_Draw_RectRotaGraph3F(x, y, sx, sy, sw, sh, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}

int DxLib_DrawTurnGraph(int x, int y, int graphID, int blendFlag) {
    return Dx_Draw_TurnGraph(x, y, graphID, blendFlag);
}
int DxLib_DrawTurnGraphF(float x, float y, int graphID, int blendFlag) {
    return Dx_Draw_TurnGraphF(x, y, graphID, blendFlag);
}

int DxLib_DrawModiGraph(int x1, int y1, int x2, int y2,
                        int x3, int y3, int x4, int y4,
                        int graphID, int blendFlag) {
    return Dx_Draw_ModiGraph(x1, y1, x2, y2, x3, y3, x4, y4, graphID, blendFlag);
}
int DxLib_DrawModiGraphF(float x1, float y1, float x2, float y2,
                         float x3, float y3, float x4, float y4,
                         int graphID, int blendFlag) {
    return Dx_Draw_ModiGraphF(x1, y1, x2, y2, x3, y3, x4, y4, graphID, blendFlag);
}

int DxLib_SetDrawArea(int x1, int y1, int x2, int y2) {
    return Dx_Draw_SetDrawArea(x1, y1, x2, y2);
}

int DxLib_SetDrawMode(int drawMode) {
    return Dx_Draw_SetDrawMode(drawMode);
}
int DxLib_GetDrawMode() {
    return Dx_Draw_GetDrawMode();
}
int DxLib_SetDrawBlendMode(int blendMode, int alpha) {
    return Dx_Draw_SetDrawBlendMode(blendMode, alpha);
}
int DxLib_GetDrawBlendMode(int *blendMode, int *alpha) {
    return Dx_Draw_GetDrawBlendMode(blendMode, alpha);
}
int DxLib_SetDrawBright(int redBright, int greenBright, int blueBright) {
    return Dx_Draw_SetBright(redBright, greenBright, blueBright);
}

int DxLib_SetBasicBlendFlag(int blendFlag) {
    return Dx_Draw_SetBasicBlendFlag(blendFlag);
}

int DxLib_SetBackgroundColor(int red, int green, int blue) {
    return Dx_Draw_SetBackgroundColor(red, green, blue);
}
int DxLib_ClearDrawScreen(const RECT *clearRect) {
    return Dx_Draw_ClearDrawScreen(clearRect);
}
int DxLib_ClsDrawScreen() {
    return Dx_Draw_ClearDrawScreen(NULL);
}

int DxLib_SaveDrawScreen(int x1, int y1, int x2, int y2,
                         const DXCHAR *filename, int saveType,
                         int jpegQuality, int jpegSample2x1,
                         int pngCompressionLevel) {
    switch(saveType) {
        case DX_IMAGESAVETYPE_BMP:
            return DxLib_SaveDrawScreenToBMP(x1, y1, x2, y2, filename);
        case DX_IMAGESAVETYPE_JPEG:
            return DxLib_SaveDrawScreenToJPEG(x1, y1, x2, y2, filename,
                                              jpegQuality, jpegSample2x1);
        case DX_IMAGESAVETYPE_PNG:
            return DxLib_SaveDrawScreenToPNG(x1, y1, x2, y2, filename,
                                             pngCompressionLevel);
        default:
            return -1;
    }
}

int DxLib_SaveDrawScreenToBMP(int x1, int y1, int x2, int y2,
                              const DXCHAR *filename) {
    return PL_SaveDrawScreenToBMP(x1, y1, x2, y2, filename);
}
int DxLib_SaveDrawScreenToJPEG(int x1, int y1, int x2, int y2,
                               const DXCHAR *filename,
                               int quality, int sample2x1) {
    return PL_SaveDrawScreenToJPEG(x1, y1, x2, y2, filename,
                                   quality, sample2x1);

}
int DxLib_SaveDrawScreenToPNG(int x1, int y1, int x2, int y2,
                              const DXCHAR *filename,
                              int compressionLevel) {
    return PL_SaveDrawScreenToPNG(x1, y1, x2, y2, filename, compressionLevel);
}

DXCOLOR DxLib_GetColor(int red, int green, int blue) {
    return red | (green << 8) | (blue << 16);
}

/* ---------------------------------------------------- DxFont.cpp */
#ifndef DX_NON_FONT

int DxLib_EXT_MapFontFileToName(const DXCHAR *filename,
                                const DXCHAR *fontname,
                                int thickness,
                                int boldFlag,
                                double exRateX,
                                double exRateY
                               ) {
    return PLEXT_Font_MapFontFileToName(filename,
                                   fontname, thickness, boldFlag,
                                   exRateX, exRateY);
}
int DxLib_EXT_InitFontMappings() {
    return PLEXT_Font_InitFontMappings();
}

/* Handle font functions */
int DxLib_DrawStringToHandle(int x, int y, const DXCHAR *text,
                             DXCOLOR color, int fontHandle, DXCOLOR edgeColor,
                             int VerticalFlag) {
    /* FIXME: VerticalFlag unsupported */
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
                                   DXCOLOR color, int fontHandle, DXCOLOR edgeColor,
                                   int VerticalFlag) {
    /* FIXME: VerticalFlag unsupported */
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

int DxLib_GetDrawStringWidthToHandle(const DXCHAR *string, int strLen, int fontHandle,
                                     int VerticalFlag) {
    /* FIXME: VerticalFlag unsupported */
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
int DxLib_GetDrawExtendStringWidthToHandle(double ExRateX, const DXCHAR *string, int strLen,
                                           int fontHandle, int VerticalFlag) {
    /* FIXME: VerticalFlag unsupported */
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
                       int edgeSize, int Italic, int handle) {
    /* FIXME: handle not supported */
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

int DxLib_GetDrawStringWidth(const DXCHAR *string, int strLen, int VerticalFlag) {
    /* FIXME: handle not supported */
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
int DxLib_GetDrawExtendStringWidth(double ExRateX, const DXCHAR *string, int strLen,
                                   int VerticalFlag) {
    /* FIXME: handle not supported */
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

int DxLib_SetFontCacheUsePremulAlphaFlag(int flag) {
    return PL_Font_SetFontCacheUsePremulAlphaFlag(flag);
}
int DxLib_GetFontCacheUsePremulAlphaFlag() {
    return PL_Font_GetFontCacheUsePremulAlphaFlag();
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

int DxLib_LoadSoundMem(const DXCHAR *filename, int bufferNum, int unionHandle) {
    /* FIXME: bufferNum and unionHandle are unsupported */
    return PL_LoadSoundMem(filename);
}
int DxLib_LoadSoundMem2(const DXCHAR *filename, const DXCHAR *filename2) {
    return PL_LoadSoundMem2(filename, filename2);
}
int DxLib_DeleteSoundMem(int soundID, int LogOutFlag) {
    /* FIXME: LogOutFlag is unsupported */
    return PL_DeleteSoundMem(soundID);
}
int DxLib_InitSoundMem(int LogOutFlag) {
    /* FIXME: LogOutFlag is unsupported */
    return PL_InitSoundMem();
}

int DxLib_SetCreateSoundDataType(int soundDataType) {
    return PL_SetCreateSoundDataType(soundDataType);
}
int DxLib_GetCreateSoundDataType() {
    return PL_GetCreateSoundDataType();
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

#endif /* #ifdef DXPORTLIB_DXLIB_INTERFACE */
