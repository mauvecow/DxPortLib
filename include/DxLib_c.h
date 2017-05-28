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

#ifndef DXLIB_C_H_HEADER
#define DXLIB_C_H_HEADER

#ifndef DPLBUILDCONFIG_H_HEADER
#  include "DPLBuildConfig.h"
#endif

#ifdef DXPORTLIB_DXLIB_INTERFACE

#include "DxDefines.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* C interface to DxLib, suitable for C programs or bindings to other
 * languages.
 *
 * See DxLib.h for comments on function usage and purpose.
 */

/* ------------------------------------------------------------ DxLib.cpp */

extern DXCALL int DxLib_DxLib_Init(void);
extern DXCALL int DxLib_DxLib_End(void);

extern DXCALL int DxLib_GlobalStructInitialize(void);
extern DXCALL int DxLib_IsInit(void);

extern DXCALL int DxLib_ProcessMessage(void);

extern DXCALL int DxLib_WaitTimer(int msTime);
extern DXCALL int DxLib_WaitKey();

extern DXCALL int DxLib_GetDateTime(DATEDATA *dateBuf);

extern DXCALL int DxLib_GetNowCount(int UseRDTSCFlag);

extern DXCALL int DxLib_GetRand(int maxValue);

extern DXCALL int DxLib_SRand(int randomSeed);

extern DXCALL int DxLib_SetOutApplicationLogValidFlag(int logFlag);

extern DXCALL int DxLib_SetDeleteHandleFlag(int handleID, int *flag);

extern DXCALL int DxLib_SetUseCharSet(int charset);

extern DXCALL void DxLib_EXT_SetOnlyWindowSize(int width, int height,
                                              int isFullscreen,
                                              int isFullscreenDesktop);

/* ----------------------------------------------------------- DxFile.cpp */
extern DXCALL int DxLib_EXT_FileRead_SetCharSet(int charset);

extern DXCALL int DxLib_FileRead_openW(const wchar_t *filename, int ASync);
extern DXCALL int DxLib_FileRead_openA(const char *filename, int ASync);
DXUNICALL_WRAP(int, DxLib_FileRead_open,
               (const TCHAR *filename, int ASync),
               (filename, ASync))

extern DXCALL int64_t DxLib_FileRead_sizeW(const wchar_t *filename);
extern DXCALL int64_t DxLib_FileRead_sizeA(const char *filename);
DXUNICALL_WRAP(int64_t, DxLib_FileRead_size,
               (const TCHAR *filename),
               (filename))

extern DXCALL int DxLib_FileRead_close(int fileHandle);

extern DXCALL int64_t DxLib_FileRead_tell(int fileHandle);

extern DXCALL int DxLib_FileRead_seek(int fileHandle,
                                      int64_t position, int origin);

extern DXCALL int DxLib_FileRead_read(void *data, int size, int fileHandle);

extern DXCALL int DxLib_FileRead_eof(int fileHandle);

extern DXCALL int DxLib_FileRead_getsW(wchar_t *buffer,
                                      int bufferSize, int fileHandle);
extern DXCALL int DxLib_FileRead_getsA(char *buffer,
                                      int bufferSize, int fileHandle);
DXUNICALL_WRAP(int, DxLib_FileRead_gets,
               (TCHAR *buffer, int bufferSize, int fileHandle),
               (buffer, bufferSize, fileHandle))

extern DXCALL wchar_t DxLib_FileRead_getcW(int fileHandle);
extern DXCALL char DxLib_FileRead_getcA(int fileHandle);
DXUNICALL_WRAP(TCHAR, DxLib_FileRead_getc,
               (int fileHandle), (fileHandle));

extern DXCALL int DxLib_FileRead_vscanfW(int fileHandle,
                                         const wchar_t *format, va_list args);
extern DXCALL int DxLib_FileRead_vscanfA(int fileHandle,
                                         const char *format, va_list args);
DXUNICALL_WRAP(int, DxLib_FileRead_vscanf,
        (int fileHandle, const TCHAR *format, va_list args),
        (fileHandle, format, args))

extern DXCALL int DxLib_FileRead_scanfW(int fileHandle,
                                        const wchar_t *format, ...);
extern DXCALL int DxLib_FileRead_scanfA(int fileHandle,
                                        const char *format, ...);
DXUNICALL_VA_WRAPTO(int, 
    DxLib_FileRead_scanf, (int fileHandle, const TCHAR *format, ...),
    DxLib_FileRead_vscanf, (fileHandle, format, args), format)

/* -------------------------------------------------------- DxArchive.cpp */
extern DXCALL int DxLib_SetUseDXArchiveFlag(int flag);

extern DXCALL int DxLib_SetDXArchiveKeyStringW(const wchar_t *keyString);
extern DXCALL int DxLib_SetDXArchiveKeyStringA(const char *keyString);
DXUNICALL_WRAP(int, DxLib_SetDXArchiveKeyString,
               (const TCHAR *keyString), (keyString))

extern DXCALL int DxLib_SetDXArchiveExtensionW(const wchar_t *extension);
extern DXCALL int DxLib_SetDXArchiveExtensionA(const char *extension);
DXUNICALL_WRAP(int, DxLib_SetDXArchiveExtension,
               (const TCHAR *extension), (extension))

extern DXCALL int DxLib_SetDXArchivePriority(int priority);

extern DXCALL int DxLib_DXArchivePreLoadW(const wchar_t *dxaFilename, int async);
extern DXCALL int DxLib_DXArchivePreLoadA(const char *dxaFilename, int async);
DXUNICALL_WRAP(int, DxLib_DXArchivePreLoad,
               (const TCHAR *dxaFilename, int async),
               (dxaFilename, async))

extern DXCALL int DxLib_DXArchiveCheckIdleW(const wchar_t *dxaFilename);
extern DXCALL int DxLib_DXArchiveCheckIdleA(const char *dxaFilename);
DXUNICALL_WRAP(int, DxLib_DXArchiveCheckIdle,
               (const TCHAR *dxaFilename), (dxaFilename))

extern DXCALL int DxLib_DXArchiveReleaseW(const wchar_t *dxaFilename);
extern DXCALL int DxLib_DXArchiveReleaseA(const char *dxaFilename);
DXUNICALL_WRAP(int, DxLib_DXArchiveRelease,
               (const TCHAR *dxaFilename), (dxaFilename))

extern DXCALL int DxLib_DXArchiveCheckFileW(const wchar_t *dxaFilename,
                                            const wchar_t *filename);
extern DXCALL int DxLib_DXArchiveCheckFileA(const char *dxaFilename,
                                            const char *filename);
DXUNICALL_WRAP(int, DxLib_DXArchiveCheckFile,
               (const TCHAR *dxaFilename, const TCHAR *filename),
               (dxaFilename, filename))

/* ---------------------------------------------------------- DxInput.cpp */
#ifndef DX_NON_INPUT

extern DXCALL int DxLib_CheckHitKey(int keyCode);

extern DXCALL int DxLib_CheckHitKeyAll(int checkType);

extern DXCALL int DxLib_GetHitKeyStateAll(char *table);

extern DXCALL int DxLib_GetJoypadNum();

extern DXCALL int DxLib_GetJoypadInputState(int controllerIndex);
extern DXCALL int DxLib_GetJoypadAnalogInput(int *x, int *y, int controllerIndex);
extern DXCALL int DxLib_GetJoypadAnalogInputRight(int *x, int *y, int controllerIndex);
extern DXCALL int DxLib_GetJoypadPOVState(int controllerIndex, int povNumber);
extern DXCALL int DxLib_GetJoypadDirectInputState(int controllerIndex,
                                                  DINPUT_JOYSTATE *state);
extern DXCALL int DxLib_GetJoypadXInputState(int controllerIndex,
                                             XINPUT_STATE *state);

extern DXCALL int DxLib_SetJoypadInputToKeyInput(
                                    int controllerIndex, int input,
                                    int key1, int key2,
                                    int key3, int key4);

extern DXCALL int DxLib_GetMousePoint(int *xPosition, int *yPosition);
extern DXCALL int DxLib_SetMousePoint(int xPosition, int yPosition);
extern DXCALL int DxLib_GetMouseInput();
extern DXCALL int DxLib_SetValidMousePointerWindowOutClientAreaMoveFlag(int flag);

extern DXCALL int DxLib_GetMouseWheelRotVol(int clearFlag);
extern DXCALL int DxLib_GetMouseHWheelRotVol(int clearFlag);
extern DXCALL float DxLib_GetMouseWheelRotVolF(int clearFlag);
extern DXCALL float DxLib_GetMouseHWheelRotVolF(int clearFlag);

#endif /* #ifndef DX_NON_INPUT */

/* --------------------------------------------------------- DxWindow.cpp */
extern DXCALL int DxLib_SetGraphMode(int width, int height,
                                     int bitDepth, int FPS);
extern DXCALL int DxLib_SetWindowSizeChangeEnableFlag(int windowResizeFlag,
                                                      int fitScreen);
extern DXCALL int DxLib_SetWindowTextW(const wchar_t *windowName);
extern DXCALL int DxLib_SetWindowTextA(const char *windowName);
DXUNICALL_WRAP(int, DxLib_SetWindowText, (const TCHAR *windowName),
               (windowName))
DXUNICALL_WRAPTO(int, DxLib_SetMainWindowText, (const TCHAR *windowName),
                 DxLib_SetWindowText, (windowName))

extern DXCALL int DxLib_ScreenFlip();
extern DXCALL int DxLib_ChangeWindowMode(int fullscreenFlag);
extern DXCALL int DxLib_GetWindowModeFlag();
extern DXCALL int DxLib_SetDrawScreen(int flag);
extern DXCALL int DxLib_GetDrawScreen();
extern DXCALL int DxLib_GetDrawScreenSize(int *xBuf, int *yBuf);
extern DXCALL int DxLib_GetActiveGraph();
extern DXCALL int DxLib_SetMouseDispFlag(int flag);
extern DXCALL int DxLib_GetMouseDispFlag();
extern DXCALL int DxLib_SetWaitVSyncFlag(int flag);
extern DXCALL int DxLib_GetWaitVSyncFlag();
extern DXCALL int DxLib_SetAlwaysRunFlag(int flag);
extern DXCALL int DxLib_GetAlwaysRunFlag();
extern DXCALL int DxLib_GetWindowActiveFlag();
extern DXCALL int DxLib_GetActiveFlag();
extern DXCALL int DxLib_GetWindowCloseFlag();
extern DXCALL int DxLib_GetWindowUserCloseFlag(int resetFlag);


extern DXCALL int DxLib_GetDisplayModeNum();
extern DXCALL DISPLAYMODEDATA DxLib_GetDisplayMode(int modeIndex);

extern DXCALL int DxLib_EXT_SetIconImageFileW(const wchar_t *filename);
extern DXCALL int DxLib_EXT_SetIconImageFileA(const char *filename);
DXUNICALL_WRAP(int, DxLib_EXT_SetIconImageFile,
               (const TCHAR *filename), (filename))

extern DXCALL int DxLib_EXT_MessageBoxErrorA(const char *title,
                                             const char *text);
extern DXCALL int DxLib_EXT_MessageBoxErrorW(const wchar_t *title,
                                             const wchar_t *text);
DXUNICALL_WRAP(int, DxLib_EXT_MessageBoxError,
               (const TCHAR *title, const TCHAR *text), (title, text))

extern DXCALL int DxLib_EXT_MessageBoxYesNoW(const wchar_t *title,
                                             const wchar_t *text,
                                             const wchar_t *button1,
                                             const wchar_t *button2);
extern DXCALL int DxLib_EXT_MessageBoxYesNoA(const char *title,
                                             const char *text,
                                             const char *button1,
                                             const char *button2);
DXUNICALL_WRAP(int, DxLib_EXT_MessageBoxYesNo,
               (const TCHAR *title, const TCHAR *text,
                const TCHAR *button1, const TCHAR *button2),
               (title, text, button1, button2))

/* ------------------------------------------------------- DxGraphics.cpp */
extern DXCALL int DxLib_MakeGraph(int width, int height,
                                  int hasAlphaChannel);
extern DXCALL int DxLib_MakeScreen(int width, int height,
                                   int hasAlphaChannel);

extern DXCALL int DxLib_LoadGraphW(const wchar_t *name, int notUse3DFlag);
extern DXCALL int DxLib_LoadGraphA(const char *name, int notUse3DFlag);
DXUNICALL_WRAP(int, DxLib_LoadGraph,
               (const TCHAR *name, int NotUse3DFlag),
               (name, NotUse3DFlag))

extern DXCALL int DxLib_LoadReverseGraphW(const wchar_t *name, int notUse3DFlag);
extern DXCALL int DxLib_LoadReverseGraphA(const char *name, int notUse3DFlag);
DXUNICALL_WRAP(int, DxLib_LoadReverseGraph,
               (const TCHAR *name, int NotUse3DFlag),
               (name, NotUse3DFlag))

extern DXCALL int DxLib_LoadDivGraphW(
                          const wchar_t *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag);
extern DXCALL int DxLib_LoadDivGraphA(
                          const char *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag);
DXUNICALL_WRAP(int, DxLib_LoadDivGraph,
               (const TCHAR *filename, int graphCount,
                int xCount, int yCount, int xSize, int ySize,
                int *handleBuf, int notUse3DFlag),
               (filename, graphCount, xCount, yCount,
                xSize, ySize, handleBuf, notUse3DFlag))

extern DXCALL int DxLib_LoadDivBmpGraphW(
                          const wchar_t *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int textureFlag, int flipFlag);
extern DXCALL int DxLib_LoadDivBmpGraphA(
                          const char *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int textureFlag, int flipFlag);
DXUNICALL_WRAP(int, DxLib_LoadDivBmpGraph,
               (const TCHAR *filename, int graphCount,
                int xCount, int yCount, int xSize, int ySize,
                int *handleBuf, int textureFlag, int flipFlag),
               (filename, graphCount, xCount, yCount,
                xSize, ySize, handleBuf, textureFlag, flipFlag))

extern DXCALL int DxLib_LoadReverseDivGraphW(
                          const wchar_t *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag);
extern DXCALL int DxLib_LoadReverseDivGraphA(
                          const char *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag);
DXUNICALL_WRAP(int, DxLib_LoadReverseDivGraph,
               (const TCHAR *filename, int graphCount,
                int xCount, int yCount, int xSize, int ySize,
                int *handleBuf, int notUse3DFlag),
               (filename, graphCount, xCount, yCount,
                xSize, ySize, handleBuf, notUse3DFlag))

extern DXCALL int DxLib_DeleteGraph(int graphID, int LogOutFlag);
extern DXCALL int DxLib_DeleteSharingGraph(int graphID);
extern DXCALL int DxLib_InitGraph(int LogOutFlag);
extern DXCALL int DxLib_DerivationGraph(int x, int y, int w, int h,
                                        int graphID);
extern DXCALL int DxLib_GetGraphNum();

extern DXCALL int DxLib_GetGraphSize(int graphID, int *width, int *height);

extern DXCALL int DxLib_SetTransColor(int r, int g, int b);
extern DXCALL int DxLib_GetTransColor(int *r, int *g, int *b);
extern DXCALL int DxLib_SetUseTransColor(int flag);

extern DXCALL int DxLib_SetUsePremulAlphaConvertLoad(int flag);

extern DXCALL int DxLib_DrawPixel(int x, int y, DXCOLOR color);

extern DXCALL int DxLib_DrawLine(int x1, int y1, int x2, int y2,
                                 DXCOLOR color, int thickness);
extern DXCALL int DxLib_DrawLineF(float x1, float y1, float x2, float y2,
                                  DXCOLOR color, int thickness);

extern DXCALL int DxLib_DrawBox(int x1, int y1, int x2, int y2,
                                DXCOLOR color, int FillFlag);
extern DXCALL int DxLib_DrawBoxF(float x1, float y1, float x2, float y2,
                                 DXCOLOR color, int FillFlag);
extern DXCALL int DxLib_DrawFillBox(int x1, int y1, int x2, int y2,
                                    DXCOLOR color);
extern DXCALL int DxLib_DrawFillBoxF(float x1, float y1, float x2, float y2,
                                     DXCOLOR color);
extern DXCALL int DxLib_DrawLineBox(int x1, int y1, int x2, int y2,
                                    DXCOLOR color);
extern DXCALL int DxLib_DrawLineBoxF(float x1, float y1, float x2, float y2,
                                     DXCOLOR color);

extern DXCALL int DxLib_DrawCircle(int x, int y, int r,
                                   DXCOLOR color, int fillFlag);
extern DXCALL int DxLib_DrawCircleF(float x, float y, float r,
                                    DXCOLOR color, int fillFlag);
extern DXCALL int DxLib_DrawOval(int x, int y, int rx, int ry,
                                 DXCOLOR color, int fillFlag);
extern DXCALL int DxLib_DrawOvalF(float x, float y, float rx, float ry,
                                  DXCOLOR color, int fillFlag);

extern DXCALL int DxLib_DrawTriangle(int x1, int y1, int x2, int y2,
                                     int x3, int y3,
                                     DXCOLOR color, int fillFlag);
extern DXCALL int DxLib_DrawTriangleF(float x1, float y1, float x2, float y2,
                                      float x3, float y3,
                                      DXCOLOR color, int fillFlag);
extern DXCALL int DxLib_DrawQuadrangle(int x1, int y1, int x2, int y2,
                                       int x3, int y3, int x4, int y4,
                                       DXCOLOR color, int fillFlag);
extern DXCALL int DxLib_DrawQuadrangleF(float x1, float y1, float x2, float y2,
                                        float x3, float y3, float x4, float y4,
                                        DXCOLOR color, int fillFlag);

extern DXCALL int DxLib_DrawGraph(int x, int y,
                                  int graphID, int blendFlag);
extern DXCALL int DxLib_DrawGraphF(float x, float y,
                                   int graphID, int blendFlag);

extern DXCALL int DxLib_DrawExtendGraph(int x1, int y1,
                                        int x2, int y2,
                                        int graphID, int blendFlag);
extern DXCALL int DxLib_DrawExtendGraphF(float x1, float y1,
                                         float x2, float y2,
                                         int graphID, int blendFlag);
                     
extern DXCALL int DxLib_DrawRectGraph(int dx, int dy,
                                      int sx, int sy, int sw, int sh,
                                      int graphID, int blendFlag,
                                      int turnFlag);
extern DXCALL int DxLib_DrawRectGraphF(float dx, float dy,
                                       int sx, int sy, int sw, int sh,
                                       int graphID, int blendFlag,
                                       int turnFlag);
extern DXCALL int DxLib_DrawRectExtendGraph(
                                      int dx1, int dy1, int dx2, int dy2,
                                      int sx, int sy, int sw, int sh,
                                      int graphID, int blendFlag,
                                      int turnFlag);
extern DXCALL int DxLib_DrawRectExtendGraphF(
                                      float dx1, float dy1,
                                      float dx2, float dy2,
                                      int sx, int sy, int sw, int sh,
                                      int graphID, int blendFlag,
                                      int turnFlag);

extern DXCALL int DxLib_DrawRotaGraph(int x, int y,
                                      double scaleFactor,
                                      double angle,
                                      int graphID, int blendFlag,
                                      int turn);
extern DXCALL int DxLib_DrawRotaGraphF(float x, float y,
                                       double scaleFactor,
                                       double angle,
                                       int graphID, int blendFlag,
                                       int turn);
extern DXCALL int DxLib_DrawRotaGraph2(int x, int y, int cx, int cy,
                                       double scaleFactor,
                                       double angle,
                                       int graphID, int blendFlag,
                                       int turn);
extern DXCALL int DxLib_DrawRotaGraph2F(float x, float y,
                                        float cx, float cy,
                                        double scaleFactor,
                                        double angle,
                                        int graphID, int blendFlag,
                                        int turn);
extern DXCALL int DxLib_DrawRotaGraph3(int x, int y, int cx, int cy,
                                       double xScaleFactor,
                                       double yScaleFactor,
                                       double angle,
                                       int graphID,
                                       int blendFlag, int turn);
extern DXCALL int DxLib_DrawRotaGraph3F(float x, float y,
                                        float cx, float cy,
                                        double xScaleFactor,
                                        double yScaleFactor,
                                        double angle,
                                        int graphID,
                                        int blendFlag, int turn);

extern DXCALL int DxLib_DrawRectRotaGraph(int x, int y,
                          int sx, int sy, int sw, int sh,
                          double scaleFactor, double angle,
                          int graphID, int blendFlag, int turn);
extern DXCALL int DxLib_DrawRectRotaGraphF(float x, float y,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern DXCALL int DxLib_DrawRectRotaGraph2(int x, int y, int cx, int cy,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern DXCALL int DxLib_DrawRectRotaGraph2F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn);
extern DXCALL int DxLib_DrawRectRotaGraph3(int x, int y,
                           int sx, int sy, int sw, int sh,
                           int cx, int cy,
                           double xScaleFactor, double yScaleFactor,
                           double angle,
                           int graphID, int blendFlag, int turn);
extern DXCALL int DxLib_DrawRectRotaGraph3F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double xScaleFactor, double yScaleFactor,
                            double angle,
                            int graphID, int blendFlag, int turn);

extern DXCALL int DxLib_DrawTurnGraph(int x, int y,
                                      int graphID, int blendFlag);
extern DXCALL int DxLib_DrawTurnGraphF(float x, float y,
                                       int graphID, int blendFlag);

extern DXCALL int DxLib_DrawModiGraph(int x1, int y1, int x2, int y2,
                                      int x3, int y3, int x4, int y4,
                                      int graphID, int blendFlag);
extern DXCALL int DxLib_DrawModiGraphF(float x1, float y1, float x2, float y2,
                                       float x3, float y3, float x4, float y4,
                                       int graphID, int blendFlag);

extern DXCALL int DxLib_SetDrawArea(int x1, int y1, int x2, int y2);

extern DXCALL int DxLib_SetDrawMode(int drawMode);
extern DXCALL int DxLib_GetDrawMode();
extern DXCALL int DxLib_SetDrawBlendMode(int blendMode, int alpha);
extern DXCALL int DxLib_GetDrawBlendMode(int *blendMode, int *alpha);
extern DXCALL int DxLib_SetDrawBright(int redBright,
                                      int greenBright,
                                      int blueBright);

extern DXCALL int DxLib_SetBasicBlendFlag(int blendFlag);

extern DXCALL int DxLib_SetBackgroundColor(int red, int green, int blue);
extern DXCALL int DxLib_ClearDrawScreen(const RECT *clearRect);
extern DXCALL int DxLib_ClsDrawScreen();

extern DXCALL int DxLib_SaveDrawScreenA(int x1, int y1, int x2, int y2,
                                        const char *filename, int saveType,
                                        int jpegQuality, int jpegSample2x1,
                                        int pngCompressionLevel);
extern DXCALL int DxLib_SaveDrawScreenW(int x1, int y1, int x2, int y2,
                                        const wchar_t *filename, int saveType,
                                        int jpegQuality, int jpegSample2x1,
                                        int pngCompressionLevel);
DXUNICALL_WRAP(int, DxLib_SaveDrawScreen,
               (int x1, int y1, int x2, int y2,
                const TCHAR *filename,
                int saveType, int jpegQuality, int jpegSample2x1,
                int pngCompressionLevel),
               (x1, y1, x2, y2, filename, saveType,
                jpegQuality, jpegSample2x1, pngCompressionLevel))

extern DXCALL int DxLib_SaveDrawScreenToBMPA(int x1, int y1, int x2, int y2,
                                             const char *filename);
extern DXCALL int DxLib_SaveDrawScreenToBMPW(int x1, int y1, int x2, int y2,
                                             const wchar_t *filename);
DXUNICALL_WRAP(int, DxLib_SaveDrawScreenToBMP,
               (int x1, int y1, int x2, int y2, const TCHAR *filename),
               (x1, y1, x2, y2, filename))

extern DXCALL int DxLib_SaveDrawScreenToJPEGA(int x1, int y1, int x2, int y2,
                                              const char *filename,
                                              int quality, int sample2x1);
extern DXCALL int DxLib_SaveDrawScreenToJPEGW(int x1, int y1, int x2, int y2,
                                              const wchar_t *filename,
                                              int quality, int sample2x1);
DXUNICALL_WRAP(int, DxLib_SaveDrawScreenToJPEG,
               (int x1, int y1, int x2, int y2,
                const TCHAR *filename, int quality, int sample2x1),
               (x1, y1, x2, y2, filename, quality, sample2x1))

extern DXCALL int DxLib_SaveDrawScreenToPNGA(int x1, int y1, int x2, int y2,
                                             const char *filename,
                                             int compressionLevel);
extern DXCALL int DxLib_SaveDrawScreenToPNGW(int x1, int y1, int x2, int y2,
                                             const wchar_t *filename,
                                             int compressionLevel);
DXUNICALL_WRAP(int, DxLib_SaveDrawScreenToPNG,
               (int x1, int y1, int x2, int y2,
                const TCHAR *filename, int compressionLevel),
               (x1, y1, x2, y2, filename, compressionLevel))

extern DXCALL DXCOLOR DxLib_GetColor(int red, int green, int blue);

/* ----------------------------------------------------------- DxFont.cpp */
#ifndef DX_NON_FONT

extern DXCALL int DxLib_EXT_MapFontFileToNameW(const wchar_t *filename,
                                               const wchar_t *fontname,
                                               int thickness,
                                               int boldFlag,
                                               double exRateX,
                                               double exRateY);
extern DXCALL int DxLib_EXT_MapFontFileToNameA(const char *filename,
                                               const char *fontname,
                                               int thickness,
                                               int boldFlag,
                                               double exRateX,
                                               double exRateY);
DXUNICALL_WRAP(int, DxLib_EXT_MapFontFileToName,
               (const TCHAR *filename, const TCHAR *fontname,
                int thickness, int boldFlag,
                double exRateX, double exRateY),
               (filename, fontname, thickness, boldFlag,
                exRateX, exRateY))

extern DXCALL int DxLib_EXT_InitFontMappings();

/* Handle font functions */
extern DXCALL int DxLib_DrawStringToHandleW(int x, int y,
                                            const wchar_t *string,
                                            DXCOLOR color, int fontHandle,
                                            DXCOLOR edgeColor,
                                            int VerticalFlag);
extern DXCALL int DxLib_DrawStringToHandleA(int x, int y,
                                            const char *string,
                                            DXCOLOR color, int fontHandle,
                                            DXCOLOR edgeColor,
                                            int VerticalFlag);
DXUNICALL_WRAP(int, DxLib_DrawStringToHandle,
               (int x, int y, const TCHAR *string,
                DXCOLOR color, int fontHandle,
                DXCOLOR edgeColor,
                int VerticalFlag),
               (x, y, string, color, fontHandle,
                edgeColor, VerticalFlag))

extern DXCALL int DxLib_DrawFormatVStringToHandleW(int x, int y,
                                                  DXCOLOR color,
                                                  int fontHandle,
                                                  const wchar_t *formatString,
                                                  va_list args);
extern DXCALL int DxLib_DrawFormatVStringToHandleA(int x, int y,
                                                  DXCOLOR color,
                                                  int fontHandle,
                                                  const char *formatString,
                                                  va_list args);
DXUNICALL_WRAP(int, DxLib_DrawFormatVStringToHandle,
               (int x, int y, DXCOLOR color, int fontHandle,
                const TCHAR *formatString, va_list args),
               (x, y, color, fontHandle, formatString, args))

DXUNICALL_VA_WRAPTO(int, DxLib_DrawFormatStringToHandle,
                    (int x, int y, DXCOLOR color,
                     int fontHandle,
                     const TCHAR *formatString, ...),
                    DxLib_DrawFormatVStringToHandle,
                    (x, y, color, fontHandle, formatString, args),
                    formatString)

extern DXCALL int DxLib_DrawExtendStringToHandleW(int x, int y,
                    double ExRateX, double ExRateY,
                    const wchar_t *string, DXCOLOR color,
                    int fontHandle, DXCOLOR edgeColor, int VerticalFlag);
extern DXCALL int DxLib_DrawExtendStringToHandleA(int x, int y,
                    double ExRateX, double ExRateY,
                    const char *string, DXCOLOR color,
                    int fontHandle, DXCOLOR edgeColor, int VerticalFlag);
DXUNICALL_WRAP(int, DxLib_DrawExtendStringToHandle,
               (int x, int y, double ExRateX, double ExRateY,
                const TCHAR *string, DXCOLOR color, int fontHandle,
                DXCOLOR edgeColor, int VerticalFlag),
               (x, y, ExRateX, ExRateY, string,
                color, fontHandle, edgeColor, VerticalFlag))

extern DXCALL int DxLib_DrawExtendFormatVStringToHandleA(
                    int x, int y, double ExRateX, double ExRateY, DXCOLOR color,
                    int fontHandle, const char *formatString, va_list args);
extern DXCALL int DxLib_DrawExtendFormatVStringToHandleW(
                    int x, int y, double ExRateX, double ExRateY, DXCOLOR color,
                    int fontHandle, const wchar_t *formatString, va_list args);
DXUNICALL_VA_WRAPTO(int, DxLib_DrawExtendFormatStringToHandle,
                    (int x, int y, double ExRateX, double ExRateY,
                     DXCOLOR color, int fontHandle, const TCHAR *formatString, ...),
                    DxLib_DrawExtendFormatVStringToHandle,
                    (x, y, ExRateX, ExRateY,
                     color, fontHandle, formatString, args),
                    formatString)

extern DXCALL int DxLib_GetDrawStringWidthToHandleA(
                    const char *string, int strLen,
                    int fontHandle, int VerticalFlag);
extern DXCALL int DxLib_GetDrawStringWidthToHandleW(
                    const wchar_t *string, int strLen,
                    int fontHandle, int VerticalFlag);
DXUNICALL_WRAP(int, DxLib_GetDrawStringWidthToHandle,
                (const TCHAR *string, int strLen, int fontHandle,
                 int VerticalFlag),
                (string, strLen, fontHandle, VerticalFlag))

extern DXCALL int DxLib_GetDrawFormatVStringWidthToHandleW(
                    int fontHandle, const wchar_t *string, va_list args);
extern DXCALL int DxLib_GetDrawFormatVStringWidthToHandleA(
                    int fontHandle, const char *string, va_list args);
DXUNICALL_VA_WRAPTO(int, DxLib_GetDrawFormatStringWidthToHandle,
                    (int fontHandle, const TCHAR *string, ...),
                    DxLib_GetDrawFormatVStringWidthToHandle,
                    (fontHandle, string, args),
                    string)

extern DXCALL int DxLib_GetDrawExtendStringWidthToHandleW(
                    double ExRateX, const wchar_t *string,
                    int strLen, int fontHandle, int VerticalFlag);
extern DXCALL int DxLib_GetDrawExtendStringWidthToHandleA(
                    double ExRateX, const char *string,
                    int strLen, int fontHandle, int VerticalFlag);
DXUNICALL_WRAP(int, DxLib_GetDrawExtendStringWidthToHandle,
               (double ExRateX, const TCHAR *string,
                int strLen, int fontHandle, int VerticalFlag),
               (ExRateX, string, strLen, fontHandle, VerticalFlag))

extern DXCALL int DxLib_GetDrawExtendFormatVStringWidthToHandleW(
                    double ExRateX, int fontHandle,
                    const wchar_t *string, va_list args);
extern DXCALL int DxLib_GetDrawExtendFormatVStringWidthToHandleA(
                    double ExRateX, int fontHandle,
                    const char *string, va_list args);
DXUNICALL_VA_WRAPTO(int, DxLib_GetDrawExtendFormatStringWidthToHandle,
                    (double ExRateX, int fontHandle, const TCHAR *string, ...),
                    DxLib_GetDrawExtendFormatVStringWidthToHandle,
                    (ExRateX, fontHandle, string, args),
                    string)

extern DXCALL int DxLib_GetFontSizeToHandle(int fontHandle);
extern DXCALL int DxLib_GetFontCharInfoA(
                    int fontHandle, const char *string,
                    int *xPos, int *yPos, int *advanceX,
                    int *width, int *height);
extern DXCALL int DxLib_GetFontCharInfoW(
                    int fontHandle, const wchar_t *string,
                    int *xPos, int *yPos, int *advanceX,
                    int *width, int *height);
DXUNICALL_WRAP(int, DxLib_GetFontCharInfo,
               (int fontHandle, const TCHAR *string,
                int *xPos, int *yPos, int *advanceX,
                int *width, int *height),
               (fontHandle, string, xPos, yPos, advanceX, width, height))

extern DXCALL int DxLib_SetFontSpaceToHandle(int fontSpacing,
                                             int fontHandle);

extern DXCALL int DxLib_CreateFontToHandleA(
                    const char *fontname, int size, int thickness,
                    int fontType, int charset, int edgeSize, int italic,
                    int handle);
extern DXCALL int DxLib_CreateFontToHandleW(
                    const wchar_t *fontname, int size, int thickness,
                    int fontType, int charset, int edgeSize, int italic,
                    int handle);
DXUNICALL_WRAP(int, DxLib_CreateFontToHandle,
               (const TCHAR *fontname, int size, int thickness,
                int fontType, int charset, int edgeSize,
                int italic, int handle),
               (fontname, size, thickness, fontType, charset, edgeSize,
                italic, handle))
extern DXCALL int DxLib_DeleteFontToHandle(int handle);
extern DXCALL int DxLib_CheckFontHandleValid(int fontHandle);
extern DXCALL int DxLib_SetFontLostFlag(int fontHandle, int *lostFlag);

extern DXCALL int DxLib_InitFontToHandle();

/* "Default" font functions */
extern DXCALL int DxLib_GetDefaultFontHandle();

DXUNICALL_WRAPTO(int, DxLib_DrawString,
                 (int x, int y, const TCHAR *string,
                  DXCOLOR color, DXCOLOR edgeColor),
                 DxLib_DrawStringToHandle,
                 (x, y, string, color, DxLib_GetDefaultFontHandle(),
                  edgeColor, FALSE))
DXUNICALL_VA_WRAPTO(int, DxLib_DrawFormatString,
                    (int x, int y, DXCOLOR color, const TCHAR *formatString, ...),
                    DxLib_DrawFormatVStringToHandle,
                    (x, y, color, DxLib_GetDefaultFontHandle(), formatString, args),
                    formatString)

DXUNICALL_WRAPTO(int, DxLib_DrawExtendString,
                 (int x, int y, double ExRateX, double ExRateY,
                  const TCHAR *string, DXCOLOR color, DXCOLOR edgeColor),
                 DxLib_DrawExtendStringToHandle,
                 (x, y, ExRateX, ExRateY, string, color,
                  DxLib_GetDefaultFontHandle(), edgeColor, FALSE))
DXUNICALL_VA_WRAPTO(int, DxLib_DrawExtendFormatString,
                    (int x, int y, double ExRateX, double ExRateY,
                     DXCOLOR color, const TCHAR *formatString, ...),
                    DxLib_DrawExtendFormatVStringToHandle,
                    (x, y, ExRateX, ExRateY, color,
                     DxLib_GetDefaultFontHandle(), formatString, args),
                    formatString)

DXUNICALL_WRAPTO(int, DxLib_GetDrawStringWidth,
                 (const TCHAR *string, int strLen, int VerticalFlag),
                 DxLib_GetDrawStringWidthToHandle,
                 (string, strLen, DxLib_GetDefaultFontHandle(), VerticalFlag))
DXUNICALL_VA_WRAPTO(int, DxLib_GetDrawFormatStringWidth,
                    (const TCHAR *string, ...),
                    DxLib_GetDrawFormatVStringWidthToHandle,
                    (DxLib_GetDefaultFontHandle(), string, args),
                    string)

DXUNICALL_WRAPTO(int, DxLib_GetDrawExtendStringWidth,
                 (double ExRateX, const TCHAR *string,
                  int strLen, int VerticalFlag),
                 DxLib_GetDrawExtendStringWidthToHandle,
                 (ExRateX, string, strLen,
                  DxLib_GetDefaultFontHandle(), VerticalFlag))

DXUNICALL_VA_WRAPTO(int, DxLib_GetDrawExtendFormatStringWidth,
                    (double ExRateX, const TCHAR *string, ...),
                    DxLib_GetDrawExtendFormatVStringWidthToHandle,
                    (ExRateX, DxLib_GetDefaultFontHandle(), string, args),
                    string)

extern DXCALL int DxLib_ChangeFontW(const wchar_t *fontName, int charSet);
extern DXCALL int DxLib_ChangeFontA(const char *fontName, int charSet);
DXUNICALL_WRAP(int, DxLib_ChangeFont,
               (const TCHAR *fontName, int charSet),
               (fontName, charSet))

extern DXCALL int DxLib_ChangeFontType(int fontType);
extern DXCALL int DxLib_SetFontSize(int fontSize);
extern DXCALL int DxLib_GetFontSize();
extern DXCALL int DxLib_SetFontThickness(int fontThickness);
extern DXCALL int DxLib_SetFontSpace(int fontSpace);
extern DXCALL int DxLib_SetDefaultFontStateW(
                const wchar_t *fontName, int fontSize, int fontThickness);
extern DXCALL int DxLib_SetDefaultFontStateA(
                const char *fontName, int fontSize, int fontThickness);
DXUNICALL_WRAP(int, DxLib_SetDefaultFontState,
               (const TCHAR *fontName, int fontSize, int fontThickness),
               (fontName, fontSize, fontThickness))

extern DXCALL int DxLib_SetFontCacheUsePremulAlphaFlag(int flag);
extern DXCALL int DxLib_GetFontCacheUsePremulAlphaFlag();

#endif /* #ifndef DX_NON_FONT */

/* ---------------------------------------------------------- DxAudio.cpp */
#ifndef DX_NON_SOUND

extern DXCALL int DxLib_PlaySoundMem(int soundID,
                                     int playType,
                                     int startPositionFlag);
extern DXCALL int DxLib_StopSoundMem(int soundID);
extern DXCALL int DxLib_CheckSoundMem(int soundID);

extern DXCALL int DxLib_SetVolumeSoundMem(int volume, int soundID);
extern DXCALL int DxLib_ChangeVolumeSoundMem(int volume, int soundID);
extern DXCALL int DxLib_SetUseOldVolumeCalcFlag(int volumeFlag);

extern DXCALL int DxLib_LoadSoundMemW(
                const wchar_t *filename, int bufferNum, int unionHandle);
extern DXCALL int DxLib_LoadSoundMemA(
                const char *filename, int bufferNum, int unionHandle);
DXUNICALL_WRAP(int, DxLib_LoadSoundMem,
               (const TCHAR *filename, int bufferNum, int unionHandle),
               (filename, bufferNum, unionHandle))

extern DXCALL int DxLib_LoadSoundMem2W(
                const wchar_t *filename, const wchar_t *filename2);
extern DXCALL int DxLib_LoadSoundMem2A(
                const char *filename, const char *filename2);
DXUNICALL_WRAP(int, DxLib_LoadSoundMem2,
               (const TCHAR *filename, const TCHAR *filename2),
               (filename, filename2))

extern DXCALL int DxLib_DeleteSoundMem(int soundID,
                                       int LogOutFlag);

extern DXCALL int DxLib_InitSoundMem(int LogOutFlag);

extern DXCALL int DxLib_SetCreateSoundDataType(int soundDataType);
extern DXCALL int DxLib_GetCreateSoundDataType();

extern DXCALL int DxLib_GetCurrentPositionSoundMem(int soundID);

#endif /* #ifndef DX_NON_SOUND */

/* --------------------------------------------------------- DxMemory.cpp */
extern DXCALL void *DxLib_DxAlloc(size_t allocationSize,
                                  const char *file, int line);
extern DXCALL void *DxLib_DxCalloc(size_t allocationSize,
                                   const char *file, int line);
extern DXCALL void *DxLib_DxRealloc(void *memory, size_t allocationSize,
                                    const char *file, int line);
extern DXCALL void DxLib_DxFree(void *memory);

#ifdef __cplusplus
}
#endif

#endif /* #ifdef DXPORTLIB_DXLIB_INTERFACE */

#endif
