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

#include "DxLib.h"
#include "DxLib_c.h"

#include "PL/PLInternal.h"
#include "DxInternal.h"

// DxLib.cpp is a gateway to the C core internal to the library.

namespace DxLib {
    
int DxLib_Init(void) {
    return ::DxLib_DxLib_Init();
}
int DxLib_End(void) {
    return ::DxLib_DxLib_End();
}

int DxLib_GlobalStructInitialize(void) {
    return ::DxLib_GlobalStructInitialize();
}
int DxLib_IsInit(void) {
    return ::DxLib_IsInit();
}

int ProcessMessage(void) {
    return ::DxLib_ProcessMessage();
}

int WaitTimer(int msTime) {
    return ::DxLib_WaitTimer(msTime);
}

int WaitKey() {
    return ::DxLib_WaitKey();
}

int GetDateTime(DATEDATA *dateBuf) {
    return ::DxLib_GetDateTime(dateBuf);
}

int GetNowCount(int UseRDTSCFlag) {
    return ::DxLib_GetNowCount(UseRDTSCFlag);
}

int GetRand(int maxValue) {
    return ::DxLib_GetRand(maxValue);
}
int SRand(int randomSeed) {
    return ::DxLib_SRand(randomSeed);
}

int SetOutApplicationLogValidFlag(int logFlag) {
    return ::DxLib_SetOutApplicationLogValidFlag(logFlag);
}

int SetDeleteHandleFlag(int handleID, int *flag) {
    return ::DxLib_SetDeleteHandleFlag(handleID, flag);
}

int SetUseCharSet(int charset) {
    return ::DxLib_SetUseCharSet(charset);
}

void EXT_SetOnlyWindowSize(int width, int height,
                           bool isFullscreen, bool isFullscreenDesktop) {
    return ::DxLib_EXT_SetOnlyWindowSize(width, height,
                                         isFullscreen ? DXTRUE : DXFALSE,
                                         isFullscreenDesktop ? DXTRUE : DXFALSE);
}

// ---------------------------------------------------- DxFile.cpp
int EXT_FileRead_SetCharSet(int charset) {
    return ::DxLib_EXT_FileRead_SetCharSet(charset);
}

int FileRead_openA(const char *filename, int ASync) {
    return ::DxLib_FileRead_openA(filename, ASync);
}
int FileRead_openW(const wchar_t *filename, int ASync) {
    return ::DxLib_FileRead_openW(filename, ASync);
}

int64_t FileRead_sizeA(const char *filename) {
    return ::DxLib_FileRead_sizeA(filename);
}
int64_t FileRead_sizeW(const wchar_t *filename) {
    return ::DxLib_FileRead_sizeW(filename);
}

int FileRead_close(int fileHandle) {
    return ::DxLib_FileRead_close(fileHandle);
}

int64_t FileRead_tell(int fileHandle) {
    return ::DxLib_FileRead_tell(fileHandle);
}

int FileRead_seek(int fileHandle, int64_t position, int origin) {
    return ::DxLib_FileRead_seek(fileHandle, position, origin);
}

int FileRead_read(void *data, int size, int fileHandle) {
    return ::DxLib_FileRead_read(data, size, fileHandle);
}

int FileRead_eof(int fileHandle) {
    return ::DxLib_FileRead_eof(fileHandle);
}

int FileRead_getsA(char *buffer, int bufferSize, int fileHandle) {
    return ::DxLib_FileRead_getsA(buffer, bufferSize, fileHandle);
}
int FileRead_getsW(wchar_t *buffer, int bufferSize, int fileHandle) {
    return ::DxLib_FileRead_getsW(buffer, bufferSize, fileHandle);
}

char FileRead_getcA(int fileHandle) {
    return ::DxLib_FileRead_getcA(fileHandle);
}
wchar_t FileRead_getcW(int fileHandle) {
    return ::DxLib_FileRead_getcW(fileHandle);
}

int FileRead_vscanfA(int fileHandle, const char *format, va_list args) {
    return ::DxLib_FileRead_vscanfA(fileHandle, format, args);
}
int FileRead_vscanfW(int fileHandle, const wchar_t *format, va_list args) {
    return ::DxLib_FileRead_vscanfW(fileHandle, format, args);
}

// ---------------------------------------------------- DxArchive.cpp
int SetUseDXArchiveFlag(int flag) {
    return ::DxLib_SetUseDXArchiveFlag(flag);
}
int SetDXArchiveKeyStringW(const wchar_t *keyString) {
    return ::DxLib_SetDXArchiveKeyStringW(keyString);
}
int SetDXArchiveKeyStringA(const char *keyString) {
    return ::DxLib_SetDXArchiveKeyStringA(keyString);
}
int SetDXArchiveExtensionW(const wchar_t *extension) {
    return ::DxLib_SetDXArchiveExtensionW(extension);
}
int SetDXArchiveExtensionA(const char *extension) {
    return ::DxLib_SetDXArchiveExtensionA(extension);
}

int SetDXArchivePriority(int priority) {
    return ::DxLib_SetDXArchivePriority(priority);
}
int DXArchivePreLoadA(const char *dxaFilename, int async) {
    return ::DxLib_DXArchivePreLoadA(dxaFilename, async);
}
int DXArchivePreLoadW(const wchar_t *dxaFilename, int async) {
    return ::DxLib_DXArchivePreLoadW(dxaFilename, async);
}
int DXArchiveCheckIdleA(const char *dxaFilename) {
    return ::DxLib_DXArchiveCheckIdleA(dxaFilename);
}
int DXArchiveCheckIdleW(const wchar_t *dxaFilename) {
    return ::DxLib_DXArchiveCheckIdleW(dxaFilename);
}
int DXArchiveReleaseA(const char *dxaFilename) {
    return ::DxLib_DXArchiveReleaseA(dxaFilename);
}
int DXArchiveReleaseW(const wchar_t *dxaFilename) {
    return ::DxLib_DXArchiveReleaseW(dxaFilename);
}
int DXArchiveCheckFileA(const char *dxaFilename, const char *filename) {
    return ::DxLib_DXArchiveCheckFileA(dxaFilename, filename);
}
int DXArchiveCheckFileW(const wchar_t *dxaFilename, const wchar_t *filename) {
    return ::DxLib_DXArchiveCheckFileW(dxaFilename, filename);
}

// ---------------------------------------------------- DxInput.cpp
#ifndef DX_NON_INPUT

int CheckHitKey(int keyCode) {
    return ::DxLib_CheckHitKey(keyCode);
}
int CheckHitKeyAll(int checkType) {
    return ::DxLib_CheckHitKeyAll(checkType);
}
int GetHitKeyStateAll(char *table) {
    return ::DxLib_GetHitKeyStateAll(table);
}
int GetJoypadNum() {
    return ::DxLib_GetJoypadNum();
}
int GetJoypadInputState(int controllerIndex) {
    return ::DxLib_GetJoypadInputState(controllerIndex);
}
int GetJoypadAnalogInput(int *x, int *y, int controllerIndex) {
    return ::DxLib_GetJoypadAnalogInput(x, y, controllerIndex);
}
int GetJoypadAnalogInputRight(int *x, int *y, int controllerIndex) {
    return ::DxLib_GetJoypadAnalogInputRight(x, y, controllerIndex);
}
int GetJoypadPOVState(int controllerIndex, int povNumber) {
    return ::DxLib_GetJoypadPOVState(controllerIndex, povNumber);
}
int GetJoypadDirectInputState(int controllerIndex, DINPUT_JOYSTATE *state) {
    return ::DxLib_GetJoypadDirectInputState(controllerIndex, state);
}
int GetJoypadXInputState(int controllerIndex, XINPUT_STATE *state) {
    return ::DxLib_GetJoypadXInputState(controllerIndex, state);
}

int SetJoypadInputToKeyInput(int controllerIndex, int input,
                             int key1, int key2,
                             int key3, int key4) {
    return ::DxLib_SetJoypadInputToKeyInput(
        controllerIndex, input, key1, key2, key3, key4);
}

int GetMousePoint(int *xPosition, int *yPosition) {
    return ::DxLib_GetMousePoint(xPosition, yPosition);
}
int SetMousePoint(int xPosition, int yPosition) {
    return ::DxLib_SetMousePoint(xPosition, yPosition);
}
int GetMouseInput() {
    return ::DxLib_GetMouseInput();
}

int SetValidMousePointerWindowOutClientAreaMoveFlag(int flag) {
    return ::DxLib_SetValidMousePointerWindowOutClientAreaMoveFlag(flag);
}

int GetMouseWheelRotVol(int clearFlag) {
    return ::DxLib_GetMouseWheelRotVol(clearFlag);
}
int DxLib_GetMouseHWheelRotVol(int clearFlag) {
    return ::DxLib_GetMouseHWheelRotVol(clearFlag);
}
float GetMouseWheelRotVolF(int clearFlag) {
    return ::DxLib_GetMouseWheelRotVolF(clearFlag);
}
float GetMouseHWheelRotVolF(int clearFlag) {
    return ::DxLib_GetMouseHWheelRotVolF(clearFlag);
}
#endif /* #ifndef DX_NON_INPUT */

// ---------------------------------------------------- DxWindow.cpp
int SetGraphMode(int width, int height, int bitDepth, int FPS) {
    return ::DxLib_SetGraphMode(width, height, bitDepth, FPS);
}
int SetWindowSizeChangeEnableFlag(int windowResizeFlag, int fitScreen) {
    return ::DxLib_SetWindowSizeChangeEnableFlag(windowResizeFlag, fitScreen);
}

// Windows can define SetWindowText to SetWindowTextA/W,
// so get rid of that.
#ifdef SetWindowText
#  undef SetWindowText
#endif

int SetWindowTextA(const char *windowName) {
    return ::DxLib_SetWindowTextA(windowName);
}
int SetWindowTextW(const wchar_t *windowName) {
    return ::DxLib_SetWindowTextW(windowName);
}

int ScreenFlip() {
    return ::DxLib_ScreenFlip();
}
int ChangeWindowMode(int fullscreenFlag) {
    return ::DxLib_ChangeWindowMode(fullscreenFlag);
}
int GetWindowModeFlag() {
    return ::DxLib_GetWindowModeFlag();
}
int SetDrawScreen(int screen) {
    return ::DxLib_SetDrawScreen(screen);
}
int GetDrawScreen() {
    return ::DxLib_GetDrawScreen();
}
int GetActiveGraph() {
    return ::DxLib_GetDrawScreen();
}

int SetWaitVSyncFlag(int flag) {
    return ::DxLib_SetWaitVSyncFlag(flag);
}
int GetWaitVSyncFlag() {
    return ::DxLib_GetWaitVSyncFlag();
}

int SetMouseDispFlag(int flag) {
    return ::DxLib_SetMouseDispFlag(flag);
}
int GetMouseDispFlag() {
    return ::DxLib_GetMouseDispFlag();
}
int EXT_SetIconImageFileA(const char *filename) {
    return ::DxLib_EXT_SetIconImageFileA(filename);
}
int EXT_SetIconImageFileW(const wchar_t *filename) {
    return ::DxLib_EXT_SetIconImageFileW(filename);
}
int SetAlwaysRunFlag(int flag) {
    return ::DxLib_SetAlwaysRunFlag(flag);
}
int GetAlwaysRunFlag() {
    return ::DxLib_GetAlwaysRunFlag();
}
int GetWindowActiveFlag() {
    return ::DxLib_GetWindowActiveFlag();
}
int GetActiveFlag() {
    return ::DxLib_GetActiveFlag();
}

int EXT_MessageBoxErrorA(const char *title, const char *text) {
    return ::DxLib_EXT_MessageBoxErrorA(title, text);
}
int EXT_MessageBoxErrorW(const wchar_t *title, const wchar_t *text) {
    return ::DxLib_EXT_MessageBoxErrorW(title, text);
}
int EXT_MessageBoxYesNoA(const char *title, const char *text,
                         const char *button1, const char *button2) {
    return ::DxLib_EXT_MessageBoxYesNoA(title, text, button1, button2);
}
int EXT_MessageBoxYesNoW(const wchar_t *title, const wchar_t *text,
                         const wchar_t *button1, const wchar_t *button2) {
    return ::DxLib_EXT_MessageBoxYesNoW(title, text, button1, button2);
}

// ---------------------------------------------------- DxGraphics.cpp

int MakeScreen(int width, int height, int hasAlphaChannel) {
    return ::DxLib_MakeScreen(width, height, hasAlphaChannel);
}
int LoadGraphA(const char *name, int notUse3DFlag) {
    return ::DxLib_LoadGraphA(name, notUse3DFlag);
}
int LoadGraphW(const wchar_t *name, int notUse3DFlag) {
    return ::DxLib_LoadGraphW(name, notUse3DFlag);
}
int LoadReverseGraphA(const char *name, int notUse3DFlag) {
    return ::DxLib_LoadReverseGraphA(name, notUse3DFlag);
}
int LoadReverseGraphW(const wchar_t *name, int notUse3DFlag) {
    return ::DxLib_LoadReverseGraphW(name, notUse3DFlag);
}
int LoadDivGraphA(const char *filename, int graphCount,
                  int xCount, int yCount, int xSize, int ySize,
                  int *handleBuf, int notUse3DFlag) {
    return ::DxLib_LoadDivGraphA(filename, graphCount, xCount, yCount,
                                xSize, ySize, handleBuf, notUse3DFlag);
}
int LoadDivGraphW(const wchar_t *filename, int graphCount,
                  int xCount, int yCount, int xSize, int ySize,
                  int *handleBuf, int notUse3DFlag) {
    return ::DxLib_LoadDivGraphW(filename, graphCount, xCount, yCount,
                                xSize, ySize, handleBuf, notUse3DFlag);
}
int LoadDivBmpGraphA(const char *filename, int graphCount,
                     int xCount, int yCount, int xSize, int ySize,
                     int *handleBuf, int textureFlag, int flipFlag) {
    return ::DxLib_LoadDivBmpGraphA(filename, graphCount, xCount, yCount,
                             xSize, ySize, handleBuf,
                             textureFlag, flipFlag);
}
int LoadDivBmpGraphW(const wchar_t *filename, int graphCount,
                     int xCount, int yCount, int xSize, int ySize,
                     int *handleBuf, int textureFlag, int flipFlag) {
    return ::DxLib_LoadDivBmpGraphW(filename, graphCount, xCount, yCount,
                             xSize, ySize, handleBuf,
                             textureFlag, flipFlag);
}
int LoadReverseDivGraphA(const char *filename, int graphCount,
                         int xCount, int yCount, int xSize, int ySize,
                         int *handleBuf, int notUse3DFlag) {
    return ::DxLib_LoadReverseDivGraphA(filename, graphCount, xCount, yCount,
                                       xSize, ySize, handleBuf, notUse3DFlag);
}
int LoadReverseDivGraphW(const wchar_t *filename, int graphCount,
                         int xCount, int yCount, int xSize, int ySize,
                         int *handleBuf, int notUse3DFlag) {
    return ::DxLib_LoadReverseDivGraphW(filename, graphCount, xCount, yCount,
                                       xSize, ySize, handleBuf, notUse3DFlag);
}
int DeleteGraph(int graphID, int LogOutFlag) {
    return ::DxLib_DeleteGraph(graphID, LogOutFlag);
}
int DeleteSharingGraph(int graphID) {
    return ::DxLib_DeleteSharingGraph(graphID);
}
int InitGraph(int LogOutFlag) {
    return ::DxLib_InitGraph(LogOutFlag);
}
int DerivationGraph(int x, int y, int w, int h, int graphID) {
    return ::DxLib_DerivationGraph(x, y, w, h, graphID);
}
int GetGraphNum() {
    return ::DxLib_GetGraphNum();
}

int GetGraphSize(int graphID, int *width, int *height) {
    return ::DxLib_GetGraphSize(graphID, width, height);
}

int SetTransColor(int r, int g, int b) {
    return ::DxLib_SetTransColor(r, g, b);
}
int GetTransColor(int *r, int *g, int *b) {
    return ::DxLib_GetTransColor(r, g, b);
}
int SetUseTransColor(int flag) {
    return ::DxLib_SetUseTransColor(flag);
}

int SetUsePremulAlphaConvertLoad(int flag) {
    return ::DxLib_SetUsePremulAlphaConvertLoad(flag);
}

int DrawPixel(int x, int y, DXCOLOR color) {
    return ::DxLib_DrawPixel(x, y, color);
}

int DrawLine(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness) {
    return ::DxLib_DrawLine(x1, y1, x2, y2, color, thickness);
}
int DrawLineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness) {
    return ::DxLib_DrawLineF(x1, y1, x2, y2, color, thickness);
}

int DrawBox(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag) {
    return ::DxLib_DrawBox(x1, y1, x2, y2, color, FillFlag);
}
int DrawBoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag) {
    return ::DxLib_DrawBoxF(x1, y1, x2, y2, color, FillFlag);
}
int DrawFillBox(int x1, int y1, int x2, int y2, DXCOLOR color) {
    return ::DxLib_DrawFillBox(x1, y1, x2, y2, color);
}
int DrawFillBoxF(float x1, float y1, float x2, float y2, DXCOLOR color) {
    return ::DxLib_DrawFillBoxF(x1, y1, x2, y2, color);
}
int DrawLineBox(int x1, int y1, int x2, int y2, DXCOLOR color) {
    return ::DxLib_DrawLineBox(x1, y1, x2, y2, color);
}
int DrawLineBoxF(float x1, float y1, float x2, float y2, DXCOLOR color) {
    return ::DxLib_DrawLineBoxF(x1, y1, x2, y2, color);
}

int DrawCircle(int x, int y, int r, DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawCircle(x, y, r, color, fillFlag);
}
int DrawCircleF(float x, float y, float r, DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawCircleF(x, y, r, color, fillFlag);
}
int DrawOval(int x, int y, int rx, int ry, DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawOval(x, y, rx, ry, color, fillFlag);
}
int DrawOvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawOvalF(x, y, rx, ry, color, fillFlag);
}

int DrawTriangle(int x1, int y1, int x2, int y2,
                       int x3, int y3,
                       DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawTriangle(x1, y1, x2, y2, x3, y3, color, fillFlag);
}
int DrawTriangleF(float x1, float y1, float x2, float y2,
                        float x3, float y3,
                        DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawTriangleF(x1, y1, x2, y2, x3, y3, color, fillFlag);
}
int DrawQuadrangle(int x1, int y1, int x2, int y2,
                         int x3, int y3, int x4, int y4,
                         DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawQuadrangle(x1, y1, x2, y2, x3, y3, x4, y4, color, fillFlag);
}
int DrawQuadrangleF(float x1, float y1, float x2, float y2,
                          float x3, float y3, float x4, float y4,
                          DXCOLOR color, int fillFlag) {
    return ::DxLib_DrawQuadrangleF(x1, y1, x2, y2, x3, y3, x4, y4, color, fillFlag);
}

int DrawGraph(int x, int y, int graphID, int blendFlag) {
    return ::DxLib_DrawGraph(x, y, graphID, blendFlag);
}
int DrawGraphF(float x, float y, int graphID, int blendFlag) {
    return ::DxLib_DrawGraphF(x, y, graphID, blendFlag);
}
int DrawExtendGraph(int x1, int y1, int x2, int y2,
                    int graphID, int blendFlag) {
    return ::DxLib_DrawExtendGraph(x1, y1, x2, y2,
                                   graphID, blendFlag);
}
int DrawExtendGraphF(float x1, float y1, float x2, float y2,
                     int graphID, int blendFlag) {
    return ::DxLib_DrawExtendGraphF(x1, y1, x2, y2,
                                    graphID, blendFlag);
}

int DrawRectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                  int graphID, int blendFlag, int turnFlag) {
    return ::DxLib_DrawRectGraph(dx, dy,
                                 sx, sy, sw, sh,
                                 graphID, blendFlag, turnFlag);
}
int DrawRectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                   int graphID, int blendFlag, int turnFlag) {
    return ::DxLib_DrawRectGraphF(dx, dy,
                                  sx, sy, sw, sh,
                                  graphID, blendFlag, turnFlag);
}
                     
int DrawRectExtendGraph(int dx1, int dy1, int dx2, int dy2,
                        int sx, int sy, int sw, int sh,
                        int graphID, int blendFlag, int turnFlag) {
    return ::DxLib_DrawRectExtendGraph(dx1, dy1, dx2, dy2,
                                       sx, sy, sw, sh,
                                       graphID, blendFlag, turnFlag);
}
int DrawRectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                         int sx, int sy, int sw, int sh,
                         int graphID, int blendFlag, int turnFlag) {
    return ::DxLib_DrawRectExtendGraphF(dx1, dy1, dx2, dy2,
                                        sx, sy, sw, sh,
                                        graphID, blendFlag, turnFlag);
}
                     
int DrawRotaGraph(int x, int y,
                  double scaleFactor, double angle,
                  int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRotaGraph(x, y,
                                 scaleFactor, angle,
                                 graphID, blendFlag, turn);
}
int DrawRotaGraphF(float x, float y,
                   double scaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRotaGraphF(x, y,
                                  scaleFactor, angle,
                                  graphID, blendFlag, turn);
}
int DrawRotaGraph2(int x, int y, int cx, int cy,
                   double scaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRotaGraph2(x, y, cx, cy,
                                  scaleFactor, angle,
                                  graphID, blendFlag, turn);
}
int DrawRotaGraph2F(float x, float y, float cx, float cy,
                    double scaleFactor, double angle,
                    int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRotaGraph2F(x, y, cx, cy,
                                   scaleFactor, angle,
                                   graphID, blendFlag, turn);
}
int DrawRotaGraph3(int x, int y, int cx, int cy,
                   double xScaleFactor, double yScaleFactor, double angle,
                   int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRotaGraph3(x, y, cx, cy,
                                  xScaleFactor, yScaleFactor, angle,
                                  graphID, blendFlag, turn);
}
int DrawRotaGraph3F(float x, float y, float cx, float cy,
                    double xScaleFactor, double yScaleFactor, double angle,
                    int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRotaGraph3F(x, y, cx, cy,
                                   xScaleFactor, yScaleFactor, angle,
                                   graphID, blendFlag, turn);
}

int DrawRectRotaGraph(int x, int y,
                            int sx, int sy, int sw, int sh,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRectRotaGraph(x, y, sx, sy, sw, sh,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DrawRectRotaGraphF(float x, float y,
                             int sx, int sy, int sw, int sh,
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRectRotaGraphF(x, y, sx, sy, sw, sh,
                             scaleFactor, angle,
                             graphID, blendFlag, turn);
}
int DrawRectRotaGraph2(int x, int y,
                             int sx, int sy, int sw, int sh,
                             int cx, int cy,
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRectRotaGraph2(x, y, sx, sy, sw, sh, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DrawRectRotaGraph2F(float x, float y,
                              int sx, int sy, int sw, int sh,
                              float cx, float cy,
                              double scaleFactor, double angle,
                              int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRectRotaGraph2F(x, y, sx, sy, sw, sh, cx, cy,
                              scaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DrawRectRotaGraph3(int x, int y,
                             int sx, int sy, int sw, int sh,
                             int cx, int cy, 
                             double xScaleFactor, double yScaleFactor,
                             double angle,
                             int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRectRotaGraph3(x, y, sx, sy, sw, sh, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}
int DrawRectRotaGraph3F(float x, float y,
                              int sx, int sy, int sw, int sh,
                              float cx, float cy,
                              double xScaleFactor, double yScaleFactor,
                              double angle,
                              int graphID, int blendFlag, int turn) {
    return ::DxLib_DrawRectRotaGraph3F(x, y, sx, sy, sw, sh, cx, cy,
                              xScaleFactor, yScaleFactor, angle,
                              graphID, blendFlag, turn);
}

int DrawTurnGraph(int x, int y, int graphID, int blendFlag) {
    return ::DxLib_DrawTurnGraph(x, y, graphID, blendFlag);
}
int DrawTurnGraph(float x, float y, int graphID, int blendFlag) {
    return ::DxLib_DrawTurnGraphF(x, y, graphID, blendFlag);
}

int DrawModiGraph(int x1, int y1, int x2, int y2,
                        int x3, int y3, int x4, int y4,
                        int graphID, int blendFlag) {
    return ::DxLib_DrawModiGraph(x1, y1, x2, y2, x3, y3, x4, y4, graphID, blendFlag);
}
int DrawModiGraphF(float x1, float y1, float x2, float y2,
                         float x3, float y3, float x4, float y4,
                         int graphID, int blendFlag) {
    return ::DxLib_DrawModiGraphF(x1, y1, x2, y2, x3, y3, x4, y4, graphID, blendFlag);
}

int SetDrawArea(int x1, int y1, int x2, int y2) {
    return ::DxLib_SetDrawArea(x1, y1, x2, y2);
}

int SetDrawMode(int drawMode) {
    return ::DxLib_SetDrawMode(drawMode);
}
int GetDrawMode() {
    return ::DxLib_GetDrawMode();
}
int SetDrawBlendMode(int blendMode, int alpha) {
    return ::DxLib_SetDrawBlendMode(blendMode, alpha);
}
int GetDrawBlendMode(int *blendMode, int *alpha) {
    return ::DxLib_GetDrawBlendMode(blendMode, alpha);
}
int SetDrawBright(int redBright, int greenBright, int blueBright) {
    return ::DxLib_SetDrawBright(redBright, greenBright, blueBright);
}

int SetBasicBlendFlag(int blendFlag) {
    return ::DxLib_SetBasicBlendFlag(blendFlag);
}

int SetBackgroundColor(int red, int green, int blue) {
    return ::DxLib_SetBackgroundColor(red, green, blue);
}
int ClearDrawScreen(const RECT *clearRect) {
    return ::DxLib_ClearDrawScreen(clearRect);
}
int ClsDrawScreen() {
    return ::DxLib_ClsDrawScreen();
}

int SaveDrawScreenA(int x1, int y1, int x2, int y2,
                    const char *filename, int saveType,
                    int jpegQuality, int jpegSample2x1,
                    int pngCompressionLevel) {
    return ::DxLib_SaveDrawScreenA(x1, y1, x2, y2, filename,
                                  saveType, jpegQuality, jpegSample2x1,
                                  pngCompressionLevel);
}
int SaveDrawScreenW(int x1, int y1, int x2, int y2,
                    const wchar_t *filename, int saveType,
                    int jpegQuality, int jpegSample2x1,
                    int pngCompressionLevel) {
    return ::DxLib_SaveDrawScreenW(x1, y1, x2, y2, filename,
                                  saveType, jpegQuality, jpegSample2x1,
                                  pngCompressionLevel);
}
int SaveDrawScreenToBMPA(int x1, int y1, int x2, int y2,
                        const char *filename) {
    return ::DxLib_SaveDrawScreenToBMPA(x1, y1, x2, y2, filename);
}
int SaveDrawScreenToBMPW(int x1, int y1, int x2, int y2,
                         const wchar_t *filename) {
    return ::DxLib_SaveDrawScreenToBMPW(x1, y1, x2, y2, filename);
}
int SaveDrawScreenToJPEGA(int x1, int y1, int x2, int y2,
                          const char *filename,
                          int quality, int sample2x1) {
    return ::DxLib_SaveDrawScreenToJPEGA(x1, y1, x2, y2, filename,
                                        quality, sample2x1);
}
int SaveDrawScreenToJPEGW(int x1, int y1, int x2, int y2,
                          const wchar_t *filename,
                          int quality, int sample2x1) {
    return ::DxLib_SaveDrawScreenToJPEGW(x1, y1, x2, y2, filename,
                                        quality, sample2x1);
}
int SaveDrawScreenToPNGA(int x1, int y1, int x2, int y2,
                         const char *filename,
                         int compressionLevel) {
    return ::DxLib_SaveDrawScreenToPNGA(x1, y1, x2, y2, filename,
                                       compressionLevel);
}
int SaveDrawScreenToPNGW(int x1, int y1, int x2, int y2,
                         const wchar_t *filename,
                         int compressionLevel) {
    return ::DxLib_SaveDrawScreenToPNGW(x1, y1, x2, y2, filename,
                                       compressionLevel);
}

// ---------------------------------------------------- DxFont.cpp
#ifndef DX_NON_FONT

int EXT_MapFontFileToNameA(
        const char *filename, const char *fontname,
        int thickness,int boldFlag,
        double exRateX, double exRateY
) {
    return ::DxLib_EXT_MapFontFileToNameA(filename, fontname,
                                         thickness, boldFlag,
                                         exRateX, exRateY
                                        );
}
int EXT_MapFontFileToNameW(
        const wchar_t *filename, const wchar_t *fontname,
        int thickness,int boldFlag,
        double exRateX, double exRateY
) {
    return ::DxLib_EXT_MapFontFileToNameW(filename, fontname,
                                         thickness, boldFlag,
                                         exRateX, exRateY
                                        );
}
int EXT_InitFontMappings() {
    return ::DxLib_EXT_InitFontMappings();
}

/* Handle font functions */
int DrawStringToHandleA(int x, int y, const char *text,
                        DXCOLOR color, int fontHandle, DXCOLOR edgeColor,
                        int verticalFlag) {
    return ::DxLib_DrawStringToHandleA(x, y, text, color, fontHandle,
                                      edgeColor, verticalFlag);
}
int DrawStringToHandleW(int x, int y, const wchar_t *text,
                        DXCOLOR color, int fontHandle, DXCOLOR edgeColor,
                        int verticalFlag) {
    return ::DxLib_DrawStringToHandleW(x, y, text, color, fontHandle,
                                      edgeColor, verticalFlag);
}
int DrawFormatVStringToHandleA(
    int x, int y, DXCOLOR color, int fontHandle,
    const char *formatString, va_list args
) {
    return ::DxLib_DrawFormatVStringToHandleA(x, y, color, fontHandle, formatString, args);
}
int DrawFormatVStringToHandleW(
    int x, int y, DXCOLOR color, int fontHandle,
    const wchar_t *formatString, va_list args
) {
    return ::DxLib_DrawFormatVStringToHandleW(x, y, color, fontHandle, formatString, args);
}
int DrawExtendStringToHandleA(int x, int y, double ExRateX, double ExRateY,
                             const char *text,
                             DXCOLOR color, int fontHandle, DXCOLOR edgeColor,
                             int VerticalFlag) {
    return ::DxLib_DrawExtendStringToHandleA(x, y, ExRateX, ExRateY, text, color, fontHandle,
                                            edgeColor, VerticalFlag);
}
int DrawExtendStringToHandleW(int x, int y, double ExRateX, double ExRateY,
                             const wchar_t *text,
                             DXCOLOR color, int fontHandle, DXCOLOR edgeColor,
                             int VerticalFlag) {
    return ::DxLib_DrawExtendStringToHandleW(x, y, ExRateX, ExRateY, text, color, fontHandle,
                                            edgeColor, VerticalFlag);
}
int DrawExtendFormatVStringToHandleA(
    int x, int y, double ExRateX, double ExRateY,
    DXCOLOR color, int fontHandle,
    const char *formatString, va_list args
) {
    return ::DxLib_DrawExtendFormatVStringToHandleA(x, y, ExRateX, ExRateY, color, fontHandle, formatString, args);
}
int DrawExtendFormatVStringToHandleW(
    int x, int y, double ExRateX, double ExRateY,
    DXCOLOR color, int fontHandle,
    const wchar_t *formatString, va_list args
) {
    return ::DxLib_DrawExtendFormatVStringToHandleW(x, y, ExRateX, ExRateY, color, fontHandle, formatString, args);
}

int GetDrawStringWidthToHandleA(const char *string, int strLen, int fontHandle,
                               int VerticalFlag) {
    return ::DxLib_GetDrawStringWidthToHandleA(string, strLen, fontHandle, VerticalFlag);
}
int GetDrawStringWidthToHandleW(const wchar_t *string, int strLen, int fontHandle,
                               int VerticalFlag) {
    return ::DxLib_GetDrawStringWidthToHandleW(string, strLen, fontHandle, VerticalFlag);
}
int GetDrawFormatVStringWidthToHandleA(
    int fontHandle, const char *formatString, va_list args
) {
    return DxLib_GetDrawFormatVStringWidthToHandleA(fontHandle, formatString, args);
}
int GetDrawFormatVStringWidthToHandleW(
    int fontHandle, const wchar_t *formatString, va_list args
) {
    return DxLib_GetDrawFormatVStringWidthToHandleW(fontHandle, formatString, args);
}
int GetDrawExtendStringWidthToHandleA(double ExRateX, const char *string, int strLen,
                                     int fontHandle, int VerticalFlag) {
    return ::DxLib_GetDrawExtendStringWidthToHandleA(ExRateX, string, strLen, fontHandle, VerticalFlag);
}
int GetDrawExtendStringWidthToHandleW(double ExRateX, const wchar_t *string, int strLen,
                                     int fontHandle, int VerticalFlag) {
    return ::DxLib_GetDrawExtendStringWidthToHandleW(ExRateX, string, strLen, fontHandle, VerticalFlag);
}
int GetDrawExtendFormatVStringWidthToHandleA(
    double ExRateX, int fontHandle, const char *formatString, va_list args
) {
    return ::DxLib_GetDrawExtendFormatVStringWidthToHandleA(ExRateX, fontHandle, formatString, args);
}
int GetDrawExtendFormatVStringWidthToHandleW(
    double ExRateX, int fontHandle, const wchar_t *formatString, va_list args
) {
    return ::DxLib_GetDrawExtendFormatVStringWidthToHandleW(ExRateX, fontHandle, formatString, args);
}

int GetFontSizeToHandle(int fontHandle) {
    return ::DxLib_GetFontSizeToHandle(fontHandle);
}
int GetFontCharInfoA(int fontHandle, const char *string,
                     int *xPos, int *yPos, int *advanceX,
                     int *width, int *height) {
    return ::DxLib_GetFontCharInfoA(fontHandle, string, xPos, yPos, advanceX, width, height);
}
int GetFontCharInfoW(int fontHandle, const wchar_t *string,
                     int *xPos, int *yPos, int *advanceX,
                     int *width, int *height) {
    return ::DxLib_GetFontCharInfoW(fontHandle, string, xPos, yPos, advanceX, width, height);
}
int SetFontSpaceToHandle(int fontSpacing, int fontHandle) {
    return ::DxLib_SetFontSpaceToHandle(fontSpacing, fontHandle);
}

int CreateFontToHandleA(const char *fontname,
                        int size, int thickness, int fontType, int charSet,
                        int edgeSize, int Italic, int handle) {
    return ::DxLib_CreateFontToHandleA(
        fontname, size, thickness, fontType, charSet,
        edgeSize, Italic, handle
    );
}
int CreateFontToHandleW(const wchar_t *fontname,
                        int size, int thickness, int fontType, int charSet,
                        int edgeSize, int Italic, int handle) {
    return ::DxLib_CreateFontToHandleW(
        fontname, size, thickness, fontType, charSet,
        edgeSize, Italic, handle
    );
}
int DeleteFontToHandle(int fontHandle) {
    return ::DxLib_DeleteFontToHandle(fontHandle);
}
int CheckFontHandleValid(int fontHandle) {
    return ::DxLib_CheckFontHandleValid(fontHandle);
}
int SetFontLostFlag(int fontHandle, int *lostFlag) {
    return ::DxLib_SetFontLostFlag(fontHandle, lostFlag);
}

int InitFontToHandle() {
    return ::DxLib_InitFontToHandle();
}

/* "Default" font functions */
int ChangeFontA(const char *string, int charSet) {
    return ::DxLib_ChangeFontA(string, charSet);
}
int ChangeFontW(const wchar_t *string, int charSet) {
    return ::DxLib_ChangeFontW(string, charSet);
}

int ChangeFontType(int fontType) {
    return ::DxLib_ChangeFontType(fontType);
}
int SetFontSize(int fontSize) {
    return ::DxLib_SetFontSize(fontSize);
}
int GetFontSize() {
    return ::DxLib_GetFontSize();
}
int SetFontThickness(int fontThickness) {
    return ::DxLib_SetFontThickness(fontThickness);
}
int SetFontSpace(int fontSpace) {
    return ::DxLib_SetFontSpace(fontSpace);
}
int SetDefaultFontStateA(const char *fontName, int fontSize, int fontThickness) {
    return ::DxLib_SetDefaultFontStateA(fontName, fontSize, fontThickness);
}
int SetDefaultFontStateW(const wchar_t *fontName, int fontSize, int fontThickness) {
    return ::DxLib_SetDefaultFontStateW(fontName, fontSize, fontThickness);
}
int GetDefaultFontHandle() {
    return ::DxLib_GetDefaultFontHandle();
}

int SetFontCacheUsePremulAlphaFlag(int flag) {
    return ::DxLib_SetFontCacheUsePremulAlphaFlag(flag);
}
int GetFontCacheUsePremulAlphaFlag() {
    return ::DxLib_GetFontCacheUsePremulAlphaFlag();
}

#endif /* #ifndef DX_NON_FONT */

// ---------------------------------------------------- DxAudio.cpp
#ifndef DX_NON_SOUND

int PlaySoundMem(int soundID, int playType, int startPositionFlag) {
    return ::DxLib_PlaySoundMem(soundID, playType, startPositionFlag);
}
int StopSoundMem(int soundID) {
    return ::DxLib_StopSoundMem(soundID);
}
int CheckSoundMem(int soundID) {
    return ::DxLib_CheckSoundMem(soundID);
}

int SetVolumeSoundMem(int volume, int soundID) {
    return ::DxLib_SetVolumeSoundMem(volume, soundID);
}
int ChangeVolumeSoundMem(int volume, int soundID) {
    return ::DxLib_ChangeVolumeSoundMem(volume, soundID);
}
int SetUseOldVolumeCalcFlag(int volumeFlag) {
    return ::DxLib_SetUseOldVolumeCalcFlag(volumeFlag);
}

int LoadSoundMemA(const char *filename, int bufferNum, int unionHandle) {
    return ::DxLib_LoadSoundMemA(filename, bufferNum, unionHandle);
}
int LoadSoundMemW(const wchar_t *filename, int bufferNum, int unionHandle) {
    return ::DxLib_LoadSoundMemW(filename, bufferNum, unionHandle);
}
int LoadSoundMem2A(const char *filename, const char *filename2) {
    return ::DxLib_LoadSoundMem2A(filename, filename2);
}
int LoadSoundMem2W(const wchar_t *filename, const wchar_t *filename2) {
    return ::DxLib_LoadSoundMem2W(filename, filename2);
}
int DeleteSoundMem(int soundID, int LogOutFlag) {
    return ::DxLib_DeleteSoundMem(soundID, LogOutFlag);
}
int InitSoundMem(int LogOutFlag) {
    return ::DxLib_InitSoundMem(LogOutFlag);
}

int SetCreateSoundDataType(int soundDataType) {
    return ::DxLib_SetCreateSoundDataType(soundDataType);
}
int GetCreateSoundDataType() {
    return ::DxLib_GetCreateSoundDataType();
}

#endif /* #ifndef DX_NON_SOUND */

// ---------------------------------------------------- DxMemory.cpp

void *DxAlloc(size_t allocationSize, const char *file, int line) {
    return ::DxLib_DxAlloc(allocationSize, file, line);
}
void *DxCalloc(size_t allocationSize, const char *file, int line) {
    return ::DxLib_DxCalloc(allocationSize, file, line);
}
void *DxRealloc(void *memory, size_t allocationSize, const char *file, int line) {
    return ::DxLib_DxRealloc(memory, allocationSize, file, line);
}
void DxFree(void *memory) {
    return ::DxLib_DxFree(memory);
}

}

#endif /* #ifdef DXPORTLIB_DXLIB_INTERFACE */
