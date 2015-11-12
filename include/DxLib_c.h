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

#include "DxBuildConfig.h"

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

extern DXCALL int DxLib_FileRead_open(const DXCHAR *filename, int ASync);

extern DXCALL int64_t DxLib_FileRead_size(const DXCHAR *filename);

extern DXCALL int DxLib_FileRead_close(int fileHandle);

extern DXCALL int64_t DxLib_FileRead_tell(int fileHandle);

extern DXCALL int DxLib_FileRead_seek(int fileHandle,
                                      int64_t position, int origin);

extern DXCALL int DxLib_FileRead_read(void *data, int size, int fileHandle);

extern DXCALL int DxLib_FileRead_eof(int fileHandle);

extern DXCALL int DxLib_FileRead_gets(DXCHAR *buffer,
                                      int bufferSize, int fileHandle);
extern DXCALL DXCHAR DxLib_FileRead_getc(int fileHandle);
extern DXCALL int DxLib_FileRead_scanf(int fileHandle,
                                       const DXCHAR *format, ...);

/* -------------------------------------------------------- DxArchive.cpp */
extern DXCALL int DxLib_SetUseDXArchiveFlag(int flag);

extern DXCALL int DxLib_SetDXArchiveKeyString(const DXCHAR *keyString);

extern DXCALL int DxLib_SetDXArchiveExtension(const DXCHAR *extension);

extern DXCALL int DxLib_SetDXArchivePriority(int priority);

extern DXCALL int DxLib_DXArchivePreLoad(const DXCHAR *dxaFilename,
                                         int async);
extern DXCALL int DxLib_DXArchiveCheckIdle(const DXCHAR *dxaFilename);
extern DXCALL int DxLib_DXArchiveRelease(const DXCHAR *dxaFilename);
extern DXCALL int DxLib_DXArchiveCheckFile(const DXCHAR *dxaFilename,
                                           const DXCHAR *filename);

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
extern DXCALL int DxLib_SetWindowText(const DXCHAR *windowName);
extern DXCALL int DxLib_SetMainWindowText(const DXCHAR *windowName);
extern DXCALL int DxLib_ScreenFlip();
extern DXCALL int DxLib_ChangeWindowMode(int fullscreenFlag);
extern DXCALL int DxLib_GetWindowModeFlag();
extern DXCALL int DxLib_SetDrawScreen(int flag);
extern DXCALL int DxLib_GetDrawScreen();
extern DXCALL int DxLib_GetActiveGraph();
extern DXCALL int DxLib_SetMouseDispFlag(int flag);
extern DXCALL int DxLib_GetMouseDispFlag();
extern DXCALL int DxLib_SetWaitVSyncFlag(int flag);
extern DXCALL int DxLib_GetWaitVSyncFlag();
extern DXCALL int DxLib_EXT_SetIconImageFile(const DXCHAR *filename);
extern DXCALL int DxLib_SetAlwaysRunFlag(int flag);
extern DXCALL int DxLib_GetAlwaysRunFlag();
extern DXCALL int DxLib_GetWindowActiveFlag();
extern DXCALL int DxLib_GetActiveFlag();

extern DXCALL int DxLib_EXT_MessageBoxError(const DXCHAR *title,
                                            const DXCHAR *text);
extern DXCALL int DxLib_EXT_MessageBoxYesNo(const DXCHAR *title,
                                            const DXCHAR *text,
                                            const DXCHAR *button1,
                                            const DXCHAR *button2);

/* ------------------------------------------------------- DxGraphics.cpp */
extern DXCALL int DxLib_MakeScreen(int width, int height,
                                   int hasAlphaChannel);

extern DXCALL int DxLib_LoadGraph(const DXCHAR *name, int notUse3DFlag);

extern DXCALL int DxLib_LoadReverseGraph(const DXCHAR *name, int notUse3DFlag);
extern DXCALL int DxLib_LoadDivGraph(
                          const DXCHAR *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag);
extern DXCALL int DxLib_LoadDivBmpGraph(
                          const DXCHAR *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int textureFlag, int flipFlag);
extern DXCALL int DxLib_LoadReverseDivGraph(
                          const DXCHAR *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag);
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

extern DXCALL int DxLib_SaveDrawScreen(int x1, int y1, int x2, int y2,
                                       const DXCHAR *filename, int saveType,
                                       int jpegQuality, int jpegSample2x1,
                                       int pngCompressionLevel);
extern DXCALL int DxLib_SaveDrawScreenToBMP(int x1, int y1, int x2, int y2,
                                            const DXCHAR *filename);
extern DXCALL int DxLib_SaveDrawScreenToJPEG(int x1, int y1, int x2, int y2,
                                             const DXCHAR *filename,
                                             int quality, int sample2x1);
extern DXCALL int DxLib_SaveDrawScreenToPNG(int x1, int y1, int x2, int y2,
                                            const DXCHAR *filename,
                                            int compressionLevel);

extern DXCALL DXCOLOR DxLib_GetColor(int red, int green, int blue);

/* ----------------------------------------------------------- DxFont.cpp */
#ifndef DX_NON_FONT

extern DXCALL int DxLib_EXT_MapFontFileToName(const DXCHAR *filename,
                                              const DXCHAR *fontname,
                                              int thickness,
                                              int boldFlag,
                                              double exRateX,
                                              double exRateY
                                             );
extern DXCALL int DxLib_EXT_InitFontMappings();

/* Handle font functions */
extern DXCALL int DxLib_DrawStringToHandle(int x, int y,
                                           const DXCHAR *string,
                                           DXCOLOR color, int fontHandle,
                                           DXCOLOR edgeColor,
                                           int VerticalFlag);
extern DXCALL int DxLib_DrawFormatStringToHandle(int x, int y,
                                                 DXCOLOR color,
                                                 int fontHandle,
                                                 const DXCHAR *formatString,
                                                 ...);
extern DXCALL int DxLib_DrawExtendStringToHandle(int x, int y,
                                                 double ExRateX,
                                                 double ExRateY,
                                                 const DXCHAR *string,
                                                 DXCOLOR color,
                                                 int fontHandle,
                                                 DXCOLOR edgeColor,
                                                 int VerticalFlag);
extern DXCALL int DxLib_DrawExtendFormatStringToHandle(
                                                 int x, int y,
                                                 double ExRateX,
                                                 double ExRateY,
                                                 DXCOLOR color,
                                                 int fontHandle,
                                                 const DXCHAR *formatString,
                                                 ...);

extern DXCALL int DxLib_GetDrawStringWidthToHandle(const DXCHAR *string,
                                                   int strLen,
                                                   int fontHandle,
                                                   int VerticalFlag);
extern DXCALL int DxLib_GetDrawFormatStringWidthToHandle(
                                                   int fontHandle,
                                                   const DXCHAR *string,
                                                   ...);
extern DXCALL int DxLib_GetDrawExtendStringWidthToHandle(
                                                   double ExRateX,
                                                   const DXCHAR *string,
                                                   int strLen,
                                                   int fontHandle,
                                                   int VerticalFlag);
extern DXCALL int DxLib_GetDrawExtendFormatStringWidthToHandle(
                                                   double ExRateX,
                                                   int fontHandle,
                                                   const DXCHAR *string,
                                                   ...);

extern DXCALL int DxLib_GetFontSizeToHandle(int fontHandle);
extern DXCALL int DxLib_GetFontCharInfo(int fontHandle,
                                        const DXCHAR *string,
                                        int *xPos, int *yPos,
                                        int *advanceX,
                                        int *width, int *height);
extern DXCALL int DxLib_SetFontSpaceToHandle(int fontSpacing,
                                             int fontHandle);

extern DXCALL int DxLib_CreateFontToHandle(const DXCHAR *fontname,
                                           int size, int thickness,
                                           int fontType, int charset,
                                           int edgeSize, int italic,
                                           int handle
                                           );
extern DXCALL int DxLib_DeleteFontToHandle(int handle);
extern DXCALL int DxLib_CheckFontHandleValid(int fontHandle);
extern DXCALL int DxLib_SetFontLostFlag(int fontHandle, int *lostFlag);

extern DXCALL int DxLib_InitFontToHandle();

/* "Default" font functions */
extern DXCALL int DxLib_DrawString(int x, int y,
                                   const DXCHAR *string,
                                   DXCOLOR color,
                                   DXCOLOR edgeColor
                                  );
extern DXCALL int DxLib_DrawFormatString(int x, int y, DXCOLOR color,
                                         const DXCHAR *formatString, ...);
extern DXCALL int DxLib_DrawExtendString(int x, int y,
                                         double ExRateX, double ExRateY,
                                         const DXCHAR *string, DXCOLOR color,
                                         DXCOLOR edgeColor);
extern DXCALL int DxLib_DrawExtendFormatString(
                                         int x, int y,
                                         double ExRateX, double ExRateY,
                                         DXCOLOR color,
                                         const DXCHAR *formatString, ...);
extern DXCALL int DxLib_GetDrawStringWidth(const DXCHAR *string,
                                           int strLen, int VerticalFlag);
extern DXCALL int DxLib_GetDrawFormatStringWidth(const DXCHAR *string, ...);
extern DXCALL int DxLib_GetDrawExtendStringWidth(double ExRateX,
                                                 const DXCHAR *string,
                                                 int strLen, int VerticalFlag);
extern DXCALL int DxLib_GetDrawExtendFormatStringWidth(
                                                 double ExRateX,
                                                 const DXCHAR *string, ...);

extern DXCALL int DxLib_ChangeFont(const DXCHAR *string, int charSet);
extern DXCALL int DxLib_ChangeFontType(int fontType);
extern DXCALL int DxLib_SetFontSize(int fontSize);
extern DXCALL int DxLib_GetFontSize();
extern DXCALL int DxLib_SetFontThickness(int fontThickness);
extern DXCALL int DxLib_SetFontSpace(int fontSpace);
extern DXCALL int DxLib_SetDefaultFontState(const DXCHAR *fontName,
                                            int fontSize,
                                            int fontThickness);
extern DXCALL int DxLib_GetDefaultFontHandle();

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

extern DXCALL int DxLib_LoadSoundMem(const DXCHAR *filename,
                                     int bufferNum,
                                     int unionHandle);
extern DXCALL int DxLib_LoadSoundMem2(const DXCHAR *filename,
                                      const DXCHAR *filename2);
extern DXCALL int DxLib_DeleteSoundMem(int soundID,
                                       int LogOutFlag);

extern DXCALL int DxLib_InitSoundMem(int LogOutFlag);

extern DXCALL int DxLib_SetCreateSoundDataType(int soundDataType);
extern DXCALL int DxLib_GetCreateSoundDataType();

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
