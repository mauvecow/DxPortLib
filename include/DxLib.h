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

#ifndef DXLIB_H_HEADER
#define DXLIB_H_HEADER

// This is the C++ interface. It's just a wrapper to the C core.

// EXT_ is used for DxPortLib extensions.

// All UNICODE support is handled in the header, so multiple libraries
// are not required for different configurations.

#ifndef DPLBUILDCONFIG_H_HEADER
#  include "DPLBuildConfig.h"
#endif

#ifdef DXPORTLIB_DXLIB_INTERFACE

#include "DxDefines.h"

// For size_t.
#include <stdlib.h>

namespace DxLib {

// -------------------------------------------------------------- DxLib.cpp

// - Open the DxLib wrapper library.
// After setting initialization parameters, call this function.
extern DXCALL int DxLib_Init(void);

// - Closes the DxLib wrapper library.
// Call when your program is finished.
extern DXCALL int DxLib_End(void);

// - Reset initalization parameters to default.
// Can only be called before DxLib_Init.
extern DXCALL int DxLib_GlobalStructInitialize(void);

// - Returns TRUE if DxLib_Init has been called, FALSE otherwise.
extern DXCALL int DxLib_IsInit(void);

// - Handle window messages and updates as neecessary.
// Call this as part of your main loop.
// Returns -1 when the program is finished.
extern DXCALL int ProcessMessage(void);

// - Waits the given number of milliseconds.
extern DXCALL int WaitTimer(int msTime);

// - Waits for a key to be pressed.
extern DXCALL int WaitKey();

// - Gets the current date and time.
extern DXCALL int GetDateTime(DATEDATA *dateBuf);

// - Gets the time since program start, in milliseconds.
extern DXCALL int GetNowCount(int UseRDTSCFlag = DXFALSE);

// - Gets a random value from [0...RandMax]
extern DXCALL int GetRand(int maxValue);

// - Seeds the random number generator with the given seed value.
// Internally, the Mersenne Twister is used.
extern DXCALL int SRand(int randomSeed);

// - Sets if logging is enabled or not.
// NOTICE: Because Log has not been written, this does nothing.
extern DXCALL int SetOutApplicationLogValidFlag(int logFlag);

// - Sets the contents of flag to -1 when the handle is deleted.
extern DXCALL int SetDeleteHandleFlag(int handleID, int *flag);

// - Sets the default character set used.
// DxPortLib uses UTF8 by default.
// It is highly recommended to build your application to use UTF8.
extern DXCALL int SetUseCharSet(int charset);

// - DxPortLib extension: Sets the displayed window parameters.
// This does not change the framebuffer size set at SetGraphMode,
// only the display information.
extern DXCALL void EXT_SetOnlyWindowSize(int width, int height,
                                         bool isFullscreen,
                                         bool isFullscreenDesktop);

// ------------------------------------------------------------- DxFile.cpp
// - DxPortLib extension: Sets the charset to use with FileRead strings.
// Defaults to the currently active character set.
extern DXCALL int EXT_FileRead_SetCharSet(int charset);

// - Opens a file stream handle to the given file.
// Returns -1 on failure, otherwise returns the stream handle.
extern DXCALL int FileRead_openW(const wchar_t *filename, int ASync = DXFALSE);
extern DXCALL int FileRead_openA(const char *filename, int ASync = DXFALSE);
DXUNICALL_WRAP(FileRead_open, (const TCHAR *filename, int ASync = DXFALSE),
               (filename, ASync))

// - Returns the size of the file.
extern DXCALL int64_t FileRead_sizeW(const wchar_t *filename);
extern DXCALL int64_t FileRead_sizeA(const char *filename);
DXUNICALL_WRAP(FileRead_size, (const TCHAR *filename),
               (filename))

// - Closes the file stream handle.
extern DXCALL int FileRead_close(int fileHandle);

// - Returns the current position within the file stream.
extern DXCALL int64_t FileRead_tell(int fileHandle);

// - Seeks to a given position within the file.
// Origin may have the following values:
// SEEK_SET: Seeks to the given position.
// SEEK_CUR: Seek to an offset from the current position.
// SEEK_END: Seek to an offset from the end of the file.
// Returns the new position within the file.
extern DXCALL int FileRead_seek(int fileHandle,
                                int64_t position, int origin);

// - Reads size bytes from the file, storing in the data pointer.
// Returns the number of bytes read.
extern DXCALL int FileRead_read(void *data, int size, int fileHandle);

// - Returns TRUE if the current position is at the end of the file.
extern DXCALL int FileRead_eof(int fileHandle);

// - Reads a single line of text from the file, returns length.
extern DXCALL int FileRead_getsW(wchar_t *buffer,
                                 int bufferSize, int fileHandle);
extern DXCALL int FileRead_getsA(char *buffer,
                                 int bufferSize, int fileHandle);
DXUNICALL_WRAP(FileRead_gets, (TCHAR *buffer, int bufferSize, int fileHandle),
               (buffer, bufferSize, fileHandle))

// - Reads a single character.
extern DXCALL wchar_t FileRead_getcW(int fileHandle);
extern DXCALL char FileRead_getcA(int fileHandle);
static DXINLINE TCHAR FileRead_getc(int fileHandle) {
    return DXUNICALL(FileRead_getc)(fileHandle);
}

// - Performas a vscanf() on the next line of text in the file.
extern DXCALL int FileRead_vscanfW(int fileHandle,
                                  const wchar_t *format, va_list args);
extern DXCALL int FileRead_vscanfA(int fileHandle,
                                  const char *format, va_list args);
DXUNICALL_WRAP(FileRead_vscanf,
        (int fileHandle, const TCHAR *format, va_list args),
        (fileHandle, format, args))

// - Performas a scanf() on the next line of text in the file.
DXUNICALL_VA_WRAPTO(
    FileRead_scanf, (int fileHandle, const TCHAR *format, ...),
    FileRead_vscanf, (fileHandle, format, args), format)

// ---------------------------------------------------------- DxArchive.cpp
// - If true, will attempt to load data from dirname.dxa.
// e.g. "directory/blah.txt" looks for "blah.txt" in "directory.dxa"
extern DXCALL int SetUseDXArchiveFlag(int flag);

// - Sets the encryption key to be used for DXA files.
extern DXCALL int SetDXArchiveKeyStringW(const wchar_t *keyString = NULL);
extern DXCALL int SetDXArchiveKeyStringA(const char *keyString = NULL);
DXUNICALL_WRAP(SetDXArchiveKeyString, (const TCHAR *keyString = NULL), (keyString))

// - Sets the filename extension to be used for DXA files.
// Default is "dxa".
// NOTICE: Non-Windows platforms are case sensitive!
extern DXCALL int SetDXArchiveExtensionW(const wchar_t *extension = NULL);
extern DXCALL int SetDXArchiveExtensionA(const char *extension = NULL);
DXUNICALL_WRAP(SetDXArchiveExtension, (const TCHAR *extension = NULL), (extension))

// - If FALSE, tries to load from dxa files then normal files.
//   If TRUE, tries to load from normal files then dxa files.
extern DXCALL int SetDXArchivePriority(int priority = 0);

// - Preloads the dxa archive to memory.
// NOTICE: async is not currently supported and will be ignored.
extern DXCALL int DXArchivePreLoadW(const wchar_t *dxaFilename,
                                    int async = DXFALSE);
extern DXCALL int DXArchivePreLoadA(const char *dxaFilename,
                                    int async = DXFALSE);
DXUNICALL_WRAP(DXArchivePreLoad, (const TCHAR *dxaFilename, int async = DXFALSE),
               (dxaFilename, async))

// - Returns TRUE if preloading has been completed.
// NOTICE: As async is not currently supported, this will always be TRUE.
extern DXCALL int DXArchiveCheckIdleW(const wchar_t *dxaFilename);
extern DXCALL int DXArchiveCheckIdleA(const char *dxaFilename);
DXUNICALL_WRAP(DXArchiveCheckIdle, (const TCHAR *dxaFilename), (dxaFilename))

// - Releases the archive data from memory.
extern DXCALL int DXArchiveReleaseW(const wchar_t *dxaFilename);
extern DXCALL int DXArchiveReleaseA(const char *dxaFilename);
DXUNICALL_WRAP(DXArchiveRelease, (const TCHAR *dxaFilename), (dxaFilename))

// - Returns TRUE if filename is contained within the
//   archive pointed to by dxaFilename.
extern DXCALL int DXArchiveCheckFileW(const wchar_t *dxaFilename,
                                      const wchar_t *filename);
extern DXCALL int DXArchiveCheckFileA(const char *dxaFilename,
                                      const char *filename);
DXUNICALL_WRAP(DXArchiveCheckFile, (const TCHAR *dxaFilename, const TCHAR *filename),
               (dxaFilename, filename))

// ------------------------------------------------------------ DxInput.cpp
#ifndef DX_NON_INPUT

// - Checks to see if the given EY_INPUT code has been pressed.
extern DXCALL int CheckHitKey(int keyCode);

// - Checks to see if any key is hit.
//   Depending on CHECKINPUT flags, will also check for
//   joypad and mouse input.
extern DXCALL int CheckHitKeyAll(int checkType = DX_CHECKINPUT_ALL);

// - Gets the table of all pressed keys.
extern DXCALL int GetHitKeyStateAll(char *table);

// - Gets number of connected joypads.
extern DXCALL int GetJoypadNum();

// - Gets the bitmasked state of the given joypad index.
extern DXCALL int GetJoypadInputState(int controllerIndex);

// - Gets the state of the left analog stick for the given joypad index.
extern DXCALL int GetJoypadAnalogInput(int *x, int *y, int controllerIndex);

// - Gets the state of the right analog stick for the given joypad index.
extern DXCALL int GetJoypadAnalogInputRight(int *x, int *y, int controllerIndex);

// - Gets the DirectInput POV of a given joypad index.
// Returns -1 if POV is in neutral, otherwise its position.
extern DXCALL int GetJoypadPOVState(int controllerIndex, int povNumber);

// - Gets the DirectInput state of a given joypad index.
// Returns -1 if invalid.
extern DXCALL int GetJoypadDirectInputState(int controllerIndex,
                                            DINPUT_JOYSTATE *state);

// - Gets the XInput state of a given joypad index.
// Returns -1 if invalid.
extern DXCALL int GetJoypadXInputState(int controllerIndex,
                                       XINPUT_STATE *state);

// - Sets the associated keybinds for a given joystick input.
extern DXCALL int SetJoypadInputToKeyInput(int controllerIndex, int input,
                                    int key1, int key2 = -1,
                                    int key3 = -1, int key4 = -1);

// - Gets the mouse position.
extern DXCALL int GetMousePoint(int *xPosition, int *yPosition);
// - Sets the mouse positon.
extern DXCALL int SetMousePoint(int xPosition, int yPosition);
// - Gets the bitmasked state of mouse buttons.
extern DXCALL int GetMouseInput();

// - Sets if the mouse can leave the window or not.
// Internally, this prevents all mouse motion and handles mouse input in a
// customized way.
extern DXCALL int SetValidMousePointerWindowOutClientAreaMoveFlag(int flag);

// - Gets the Vertical or Horizontal mouse wheel states.
// If clearFlag, resets internal value to zero.
extern DXCALL int GetMouseWheelRotVol(int clearFlag = DXTRUE);
extern DXCALL int GetMouseHWheelRotVol(int clearFlag = DXTRUE);
extern DXCALL float GetMouseWheelRotVolF(int clearFlag = DXTRUE);
extern DXCALL float GetMouseHWheelRotVolF(int clearFlag = DXTRUE);

#endif /* #ifndef DX_NON_INPUT */

// ----------------------------------------------------------- DxWindow.cpp
// - Sets the size of the target draw buffer.
// NOTICE: For DxPortLib, this may not be the actual window size!
// NOTICE: bitDepth and FPS are ignored in DxPortLib! Always 32, 60.
extern DXCALL int SetGraphMode(int width, int height,
                               int bitDepth, int FPS = 60);

// - Sets if the window can be resized or not.
// NOTICE: Can only be set before DxLib_Init is called!
extern DXCALL int SetWindowSizeChangeEnableFlag(int windowResizeFlag,
                                                int fitScreen = DXTRUE);

// Avoid conflicts with windows headers that will define SetWindowText.
#ifdef SetWindowText
#  undef SetWindowText
#endif

// - Sets the title of the window.
extern DXCALL int SetWindowTextW(const wchar_t *windowName);
extern DXCALL int SetWindowTextA(const char *windowName);
DXUNICALL_WRAP(SetWindowText, (const TCHAR *windowName), (windowName))

// - SetMainWindowText is an alias for SetWindowText.
DXUNICALL_WRAPTO(SetMainWindowText, (const TCHAR *windowName),
                 SetWindowText, (windowName))

// - Flips the backbuffer to the front buffer.
// Call when drawing operations for a frame are finished.
extern DXCALL int ScreenFlip();

// - TRUE to use a window, FALSE(default) for fullscreen mode.
extern DXCALL int ChangeWindowMode(int fullscreenFlag);
// - If TRUE, is windowed. Otherwise, fullscreen.
extern DXCALL int GetWindowModeFlag();

// - Sets the screen graph that is currently drawn to.
// NOTICE: DxPortLib treats all internally named buffers as identical.
extern DXCALL int SetDrawScreen(int screenName);

// - Gets the screen graph that is currently drawn to.
// Both functions are identical.
// NOTICE: DxPortLib always returns DX_SCREEN_BACK for the main screen.
extern DXCALL int GetDrawScreen();
extern DXCALL int GetActiveGraph();

// - If TRUE, waits for VSync to draw, in order to remove tearing.
// NOTICE: Can only be called before DxLib_Init!
extern DXCALL int SetWaitVSyncFlag(int flag);
extern DXCALL int GetWaitVSyncFlag();

// - DxPortLib Extension: Sets the icon file to use.
// NOTICE: Cannot use resource icons. Supply a .png.
extern DXCALL int EXT_SetIconImageFileW(const wchar_t *filename);
extern DXCALL int EXT_SetIconImageFileA(const char *filename);
DXUNICALL_WRAP(EXT_SetIconImageFile, (const TCHAR *filename), (filename))

// - If TRUE, runs even if the window is not focused.
// Default is FALSE.
extern DXCALL int SetAlwaysRunFlag(int flag);
extern DXCALL int GetAlwaysRunFlag();

// - If TRUE, displays the mouse cursor, othersise hides.
// Default is TRUE.
extern DXCALL int SetMouseDispFlag(int flag);
extern DXCALL int GetMouseDispFlag();

// - If TRUE, window is currently in focus and active.
extern DXCALL int GetWindowActiveFlag();
// - If TRUE, window is currently in focus and active.
extern DXCALL int GetActiveFlag();

// - Creates an error message box.
extern DXCALL int EXT_MessageBoxErrorW(const wchar_t *title,
                                       const wchar_t *text);
extern DXCALL int EXT_MessageBoxErrorA(const char *title,
                                       const char *text);
DXUNICALL_WRAP(EXT_MessageBoxError, (const TCHAR *title, const TCHAR *text),
               (title, text))

// - Creates a message box asking a yes/no question.
// Default selected button is button1.
// MessageBoxYesNo returns -1 on error, 0 for button1, 1 for button2.
extern DXCALL int EXT_MessageBoxYesNoW(const wchar_t *title,
                                       const wchar_t *text,
                                       const wchar_t *button1,
                                       const wchar_t *button2);
extern DXCALL int EXT_MessageBoxYesNoA(const char *title,
                                       const char *text,
                                       const char *button1,
                                       const char *button2);
DXUNICALL_WRAP(EXT_MessageBoxYesNo,
               (const TCHAR *title, const TCHAR *text,
                const TCHAR *button1, const TCHAR *button2),
               (title, text, button1, button2))

// --------------------------------------------------------- DxGraphics.cpp
// - Creates a screen graph that can be used with SetDrawScreen.
// Note that you can't use it to draw with while bound.
extern DXCALL int MakeScreen(int width, int height,
                             int hasAlphaChannel = DXFALSE);

// - Loads the given image file into the returned handle.
extern DXCALL int LoadGraphW(const wchar_t *name,
                             int notUse3DFlag = DXFALSE);
extern DXCALL int LoadGraphA(const char *name,
                             int notUse3DFlag = DXFALSE);
DXUNICALL_WRAP(LoadGraph, (const TCHAR *name, int NotUse3DFlag = DXFALSE),
               (name, NotUse3DFlag))

// - Loads the given image flipped horizontally into the returned handle.
extern DXCALL int LoadReverseGraphW(const wchar_t *name,
                                    int notUse3DFlag = DXFALSE);
extern DXCALL int LoadReverseGraphA(const char *name,
                                    int notUse3DFlag = DXFALSE);
DXUNICALL_WRAP(LoadReverseGraph, (const TCHAR *name, int NotUse3DFlag = DXFALSE),
               (name, NotUse3DFlag))

// - Loads an image, and then derives into graphCount graphics handles,
//   placing them into handleBuf.
// e.g.:
//   int handles[16];
//   LoadDivGraph("image.png", 16, 4, 4, 64, 64, handles);
extern DXCALL int LoadDivGraphW(
                          const wchar_t *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag = DXFALSE);
extern DXCALL int LoadDivGraphA(
                          const char *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int notUse3DFlag = DXFALSE);
DXUNICALL_WRAP(LoadDivGraph,
               (const TCHAR *filename, int graphCount,
                int xCount, int yCount, int xSize, int ySize,
                int *handleBuf, int notUse3DFlag = DXFALSE),
               (filename, graphCount, xCount, yCount,
                xSize, ySize, handleBuf, notUse3DFlag))

extern DXCALL int LoadDivBmpGraphW(
                          const wchar_t *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int textureFlag, int flipFlag);
extern DXCALL int LoadDivBmpGraphA(
                          const char *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf, int textureFlag, int flipFlag);
DXUNICALL_WRAP(LoadDivBmpGraph,
               (const TCHAR *filename, int graphCount,
                int xCount, int yCount, int xSize, int ySize,
                int *handleBuf, int textureFlag, int flipFlag),
               (filename, graphCount, xCount, yCount,
                xSize, ySize, handleBuf, textureFlag, flipFlag))

extern DXCALL int LoadReverseDivGraphW(
                          const wchar_t *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf,
                          int notUse3DFlag = DXFALSE);
extern DXCALL int LoadReverseDivGraphA(
                          const char *filename, int graphCount,
                          int xCount, int yCount, int xSize, int ySize,
                          int *handleBuf,
                          int notUse3DFlag = DXFALSE);
DXUNICALL_WRAP(LoadReverseDivGraph,
               (const TCHAR *filename, int graphCount,
                int xCount, int yCount, int xSize, int ySize,
                int *handleBuf, int notUse3DFlag = DXFALSE),
               (filename, graphCount, xCount, yCount,
                xSize, ySize, handleBuf, notUse3DFlag))

// - Deletes a Graph handle.
extern DXCALL int DeleteGraph(int graphID, int LogOutFlag = DXFALSE);
// - Deletes all graph handles that were based on the same graph as this.
extern DXCALL int DeleteSharingGraph(int graphID);
// - Deletes ALL Graph handles.
extern DXCALL int InitGraph(int LogOutFlag = DXFALSE);

// - Gets the number of currently active graphs.
extern DXCALL int GetGraphNum();

// - Creates a new Graph handle that is a subset of another Graph.
// This does not create a separate image.
// The original graph may be safely deleted.
extern DXCALL int DerivationGraph(int x, int y, int w, int h,
                                  int graphID);

// - Puts the dimensions of the graph into the values pointed to by
//   width and height.
extern DXCALL int GetGraphSize(int graphID, int *width, int *height);

// - Sets the transparent color key to be used when loading images.
// NOTICE: This is not valid after the image has been loaded.
// Default is (0,0,0).
extern DXCALL int SetTransColor(int r, int g, int b);
// - Gets the current transparent color key.
extern DXCALL int GetTransColor(int *r, int *g, int *b);
// - TRUE if the transparent color key is to be used.
// Default is TRUE.
extern DXCALL int SetUseTransColor(int flag);

// - TRUE if loaded graphs are to be automatically converted to
//   premultiplied alpha.
// Default is FALSE.
extern DXCALL int SetUsePremulAlphaConvertLoad(int flag);

// NOTICE: For all drawing functions, the following applies:
// - FillFlag, if TRUE, will draw a solid. Otherwise, edges only.
// - blendFlag, if TRUE, draws with blending enabled.
// - turn or turnFlag, if TRUE, will flip the graph horizontally.

// - Draws a pixel at (x,y) with the given color.
extern DXCALL int DrawPixel(int x, int y, DXCOLOR color);

// - Draws a line from (x1,y1) to (x2,y2) with the given color.
extern DXCALL int DrawLine(int x1, int y1, int x2, int y2,
                           DXCOLOR color, int thickness = 1);
// DrawLineF is a PL Extension.
extern DXCALL int DrawLineF(float x1, float y1, float x2, float y2,
                            DXCOLOR color, int thickness = 1);

// - Draws a box from (x1,y1) to (x2,y2) with the given color.
//   If FillFlag is set, draws a filled box, otherwise a rectangle.
extern DXCALL int DrawBox(int x1, int y1, int x2, int y2,
                          DXCOLOR color, int FillFlag);
// DrawBoxF is a PL extension.
extern DXCALL int DrawBoxF(float x1, float y1, float x2, float y2,
                           DXCOLOR color, int FillFlag);
extern DXCALL int DrawFillBox(int x1, int y1, int x2, int y2,
                              DXCOLOR color);
// DrawFillBoxF is a PL extension.
extern DXCALL int DrawFillBoxF(float x1, float y1, float x2, float y2,
                               DXCOLOR color);
extern DXCALL int DrawLineBox(int x1, int y1, int x2, int y2,
                              DXCOLOR color);
// DrawLineBoxF is a PL extension.
extern DXCALL int DrawLineBoxF(float x1, float y1, float x2, float y2,
                               DXCOLOR color);

// - Draws a circle centered at (x,y), with radius r.
extern DXCALL int DrawCircle(int x, int y, int r,
                             DXCOLOR color, int fillFlag = DXTRUE);
extern DXCALL int DrawCircleF(float x, float y, float r,
                              DXCOLOR color, int fillFlag = DXTRUE);
// - Draws an ellipse centered at (x,y), with extents (rx,ry).
extern DXCALL int DrawOval(int x, int y, int rx, int ry,
                           DXCOLOR color, int fillFlag);
extern DXCALL int DrawOvalF(float x, float y, float rx, float ry,
                            DXCOLOR color, int fillFlag);

// - Draws a triangle at the given coordinates.
extern DXCALL int DrawTriangle(int x1, int y1, int x2, int y2,
                               int x3, int y3,
                               DXCOLOR color, int fillFlag);
extern DXCALL int DrawTriangleF(float x1, float y1, float x2, float y2,
                                float x3, float y3,
                                DXCOLOR color, int fillFlag);
extern DXCALL int DrawQuadrangle(int x1, int y1, int x2, int y2,
                                 int x3, int y3, int x4, int y4,
                                 DXCOLOR color, int fillFlag);
extern DXCALL int DrawQuadrangleF(float x1, float y1, float x2, float y2,
                                  float x3, float y3, float x4, float y4,
                                  DXCOLOR color, int fillFlag);

// - Draws a graph whole to at (x,y).
extern DXCALL int DrawGraph(int x, int y, int graphID, int blendFlag);
extern DXCALL int DrawGraphF(float x, float y, int graphID, int blendFlag);

// - Draws a graph whole from (x1,y1) to (x2,y2).
extern DXCALL int DrawExtendGraph(int x1, int y1, int x2, int y2,
                                  int graphID, int blendFlag);
extern DXCALL int DrawExtendGraphF(float x1, float y1, float x2, float y2,
                                   int graphID, int blendFlag);
                     
// - Draws a subsection of the graph
//   (sx, sy, sx+sw, sy+sh) to (dx,dy).
extern DXCALL int DrawRectGraph(int dx, int dy,
                                int sx, int sy, int sw, int sh,
                                int graphID, int blendFlag, int turnFlag);
extern DXCALL int DrawRectGraphF(float dx, float dy,
                                 int sx, int sy, int sw, int sh,
                                 int graphID, int blendFlag, int turnFlag);

// - Draws a subsection of the graph
//   (sx, sy, sx+sw, sy+sh) to (dx1,dy1,dx2,dy2).
extern DXCALL int DrawRectExtendGraph(
                               int dx1, int dy1, int dx2, int dy2,
                               int sx, int sy, int sw, int sh,
                               int graphID, int blendFlag, int turnFlag);
extern DXCALL int DrawRectExtendGraphF(
                               float dx1, float dy1, float dx2, float dy2,
                               int sx, int sy, int sw, int sh,
                               int graphID, int blendFlag, int turnFlag);

// - Draws the graph rotated from (x,y), with graph center point (cx,cy),
//   and scales by (scaleFactorX, scaleFactorY).
// (cx,cy) defaults to the graph's center.
// For functions where they are not separate, both scaleFactors are equal.
extern DXCALL int DrawRotaGraph(int x, int y,
                                double scaleFactor, double angle,
                                int graphID, int blendFlag,
                                int turn = DXFALSE);
extern DXCALL int DrawRotaGraphF(float x, float y,
                                 double scaleFactor, double angle,
                                 int graphID, int blendFlag,
                                 int turn = DXFALSE);
extern DXCALL int DrawRotaGraph2(int x, int y, int cx, int cy,
                                 double scaleFactor, double angle,
                                 int graphID, int blendFlag,
                                 int turn = DXFALSE);
extern DXCALL int DrawRotaGraph2F(float x, float y, float cx, float cy,
                                  double scaleFactor, double angle,
                                  int graphID, int blendFlag,
                                  int turn = DXFALSE);
extern DXCALL int DrawRotaGraph3(int x, int y, int cx, int cy,
                                 double xScaleFactor, double yScaleFactor,
                                 double angle, int graphID, int blendFlag,
                                 int turn = DXFALSE);
extern DXCALL int DrawRotaGraph3F(float x, float y, float cx, float cy,
                                  double xScaleFactor, double yScaleFactor,
                                  double angle, int graphID, int blendFlag,
                                  int turn = DXFALSE);

// - Draws a section of a graph (sx, sy, sx+sw, sy+sh)
//   rotated from (x,y), with graph center point (cx,cy),
//   and scales by (scaleFactorX, scaleFactorY).
// (cx,cy) defaults to the graph's center.
// For functions where they are not separate, both scaleFactors are equal.
extern DXCALL int DrawRectRotaGraph(int x, int y,
                          int sx, int sy, int sw, int sh,
                          double scaleFactor, double angle,
                          int graphID, int blendFlag, int turn);
extern DXCALL int DrawRectRotaGraphF(float x, float y,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern DXCALL int DrawRectRotaGraph2(int x, int y, int cx, int cy,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern DXCALL int DrawRectRotaGraph2F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn);
extern DXCALL int DrawRectRotaGraph3(int x, int y,
                           int sx, int sy, int sw, int sh,
                           int cx, int cy,
                           double xScaleFactor, double yScaleFactor,
                           double angle,
                           int graphID, int blendFlag, int turn);
extern DXCALL int DrawRectRotaGraph3F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double xScaleFactor, double yScaleFactor,
                            double angle,
                            int graphID, int blendFlag, int turn);

// - Draws a horizontally flipped graph to (x,y).
extern DXCALL int DrawTurnGraph(int x, int y,
                                int graphID, int blendFlag);
extern DXCALL int DrawTurnGraphF(float x, float y,
                                 int graphID, int blendFlag);

// - Draws a graph to the given quad.
extern DXCALL int DrawModiGraph(int x1, int y1, int x2, int y2,
                                int x3, int y3, int x4, int y4,
                                int graphID, int blendFlag);
extern DXCALL int DrawModiGraphF(float x1, float y1, float x2, float y2,
                                 float x3, float y3, float x4, float y4,
                                 int graphID, int blendFlag);

// - Clips the drawable area of the screen to (x1, y1, x2, y2).
extern DXCALL int SetDrawArea(int x1, int y1, int x2, int y2);

// - Sets the current texture filtering mode from DX_DRAWMODE_*
extern DXCALL int SetDrawMode(int drawMode);
extern DXCALL int GetDrawMode();

// - Sets the current blending mode from DX_BLENDMODE_*
extern DXCALL int SetDrawBlendMode(int blendMode, int alpha);
extern DXCALL int GetDrawBlendMode(int *blendMode, int *alpha);

// - Sets the drawing color, which all displayed colors are multiplied by.
extern DXCALL int SetDrawBright(int redBright,
                                int greenBright,
                                int blueBright);

// - Uses simple blending for software mode.
// NOTICE: This does nothing, as software rendering is not supported.
extern DXCALL int SetBasicBlendFlag(int blendFlag);

// - Sets background color for use with ClearDrawScreen.
extern DXCALL int SetBackgroundColor(int red, int green, int blue);

// - Clears the current screen. If clearRect is NULL, clears whole screen.
extern DXCALL int ClearDrawScreen(const RECT *clearRect = NULL);
extern DXCALL int ClsDrawScreen();

// - Saves a region of the current screen to an image file.
extern DXCALL int SaveDrawScreenW(int x1, int y1, int x2, int y2,
                                  const wchar_t *filename,
                                  int saveType = DX_IMAGESAVETYPE_BMP,
                                  int jpegQuality = 80,
                                  int jpegSample2x1 = DXTRUE,
                                  int pngCompressionLevel = -1);
extern DXCALL int SaveDrawScreenA(int x1, int y1, int x2, int y2,
                                  const char *filename,
                                  int saveType = DX_IMAGESAVETYPE_BMP,
                                  int jpegQuality = 80,
                                  int jpegSample2x1 = DXTRUE,
                                  int pngCompressionLevel = -1);
DXUNICALL_WRAP(SaveDrawScreen,
               (int x1, int y1, int x2, int y2,
                const TCHAR *filename,
                int saveType = DX_IMAGESAVETYPE_BMP,
                int jpegQuality = 80, int jpegSample2x1 = DXTRUE,
                int pngCompressionLevel = -1),
               (x1, y1, x2, y2, filename, saveType,
                jpegQuality, jpegSample2x1, pngCompressionLevel))

extern DXCALL int SaveDrawScreenToBMPW(int x1, int y1, int x2, int y2,
                                       const wchar_t *filename);
extern DXCALL int SaveDrawScreenToBMPA(int x1, int y1, int x2, int y2,
                                       const char *filename);
DXUNICALL_WRAP(SaveDrawScreenToBMP,
               (int x1, int y1, int x2, int y2, const TCHAR *filename),
               (x1, y1, x2, y2, filename))

extern DXCALL int SaveDrawScreenToJPEGW(int x1, int y1, int x2, int y2,
                                        const wchar_t *filename,
                                        int quality = 80,
                                        int sample2x1 = DXTRUE);
extern DXCALL int SaveDrawScreenToJPEGA(int x1, int y1, int x2, int y2,
                                        const char *filename,
                                        int quality = 80,
                                        int sample2x1 = DXTRUE);
DXUNICALL_WRAP(SaveDrawScreenToJPEG,
               (int x1, int y1, int x2, int y2,
                const TCHAR *filename, int quality = 80,
                int sample2x1 = DXTRUE),
               (x1, y1, x2, y2, filename, quality, sample2x1))

extern DXCALL int SaveDrawScreenToPNGW(int x1, int y1, int x2, int y2,
                                       const wchar_t *filename,
                                       int compressionLevel = -1);
extern DXCALL int SaveDrawScreenToPNGA(int x1, int y1, int x2, int y2,
                                       const char *filename,
                                       int compressionLevel = -1);
DXUNICALL_WRAP(SaveDrawScreenToPNG,
               (int x1, int y1, int x2, int y2,
                const TCHAR *filename, int compressionLevel = -1),
               (x1, y1, x2, y2, filename, compressionLevel))

// - Given the three RGB components, returns a color value.
static DXINLINE DXCOLOR GetColor(int red, int green, int blue) {
    return red | (green << 8) | (blue << 16);
}

// ------------------------------------------------------------- DxFont.cpp
#ifndef DX_NON_FONT

// - DxPortLib Extension.
//   Given a .ttf filename, it will 'map' that .ttf to a fontname,
//   a minimum thickness value, and a flag for whether or not the font is
//   bolded.
//   exRateX/exRateY are a global Extend multiplier used.
// NOTICE: This must be used for all fonts.
extern DXCALL int EXT_MapFontFileToNameW(const wchar_t *filename,
                                         const wchar_t *fontname,
                                         int thickness,
                                         int boldFlag,
                                         double exRateX = 1.0,
                                         double exRateY = 1.0);
extern DXCALL int EXT_MapFontFileToNameA(const char *filename,
                                         const char *fontname,
                                         int thickness,
                                         int boldFlag,
                                         double exRateX = 1.0,
                                         double exRateY = 1.0);
DXUNICALL_WRAP(EXT_MapFontFileToName,
               (const TCHAR *filename, const TCHAR *fontname,
                int thickness, int boldFlag,
                double exRateX = 1.0, double exRateY = 1.0),
               (filename, fontname, thickness, boldFlag,
                exRateX, exRateY))

// - DxPortLib Extension.
//   Deletes all existing font mappings.
//   Will not delete font handles.
extern DXCALL int EXT_InitFontMappings();

// ------ Font handle functions

// - Draws a string at the given position, with the given font and color.
extern DXCALL int DrawStringToHandleW(int x, int y, const wchar_t *string,
                                      DXCOLOR color, int fontHandle,
                                      DXCOLOR edgeColor = 0,
                                      int VerticalFlag = DXFALSE);
extern DXCALL int DrawStringToHandleA(int x, int y, const char *string,
                                      DXCOLOR color, int fontHandle,
                                      DXCOLOR edgeColor = 0,
                                      int VerticalFlag = DXFALSE);
DXUNICALL_WRAP(DrawStringToHandle,
               (int x, int y, const TCHAR *string,
                DXCOLOR color, int fontHandle,
                DXCOLOR edgeColor = 0,
                int VerticalFlag = DXFALSE),
               (x, y, string, color, fontHandle,
                edgeColor, VerticalFlag))

extern DXCALL int DrawFormatVStringToHandleA(
                                     int x, int y, DXCOLOR color,
                                     int fontHandle,
                                     const char *formatString, va_list args);
extern DXCALL int DrawFormatVStringToHandleW(
                                     int x, int y, DXCOLOR color,
                                     int fontHandle,
                                     const wchar_t *formatString, va_list args);
DXUNICALL_VA_WRAPTO(DrawFormatStringToHandle,
                    (int x, int y, DXCOLOR color,
                     int fontHandle,
                     const TCHAR *formatString, ...),
                    DrawFormatVStringToHandle,
                    (x, y, color, fontHandle, formatString, args),
                    formatString)

// - Draws a scaled string at the given position, with the given font/color.
extern DXCALL int DrawExtendStringToHandleA(double ExRateX, double ExRateY,
                                            int x, int y,
                                            const char *string,
                                            DXCOLOR color, int fontHandle,
                                            DXCOLOR edgeColor = 0,
                                            int VerticalFlag = DXFALSE);
extern DXCALL int DrawExtendStringToHandleW(double ExRateX, double ExRateY,
                                            int x, int y,
                                            const wchar_t *string,
                                            DXCOLOR color,
                                            int fontHandle,
                                            DXCOLOR edgeColor = 0,
                                            int VerticalFlag = DXFALSE);
DXUNICALL_WRAP(DrawExtendStringToHandle,
               (double ExRateX, double ExRateY, int x, int y,
                const TCHAR *string, DXCOLOR color, int fontHandle,
                DXCOLOR edgeColor = 0, int VerticalFlag = DXFALSE),
               (ExRateX, ExRateY, x, y, string,
                color, fontHandle, edgeColor, VerticalFlag))

extern DXCALL int DrawExtendFormatVStringToHandleA(
                                           double ExRateX, double ExRateY,
                                           int x, int y,
                                           DXCOLOR color,
                                           int fontHandle,
                                           const char *formatString, va_list args);
extern DXCALL int DrawExtendFormatVStringToHandleW(
                                           double ExRateX, double ExRateY,
                                           int x, int y,
                                           DXCOLOR color,
                                           int fontHandle,
                                           const wchar_t *formatString, va_list args);
DXUNICALL_VA_WRAPTO(DrawExtendFormatStringToHandle,
                    (double ExRateX, double ExRateY, int x, int y,
                     DXCOLOR color, int fontHandle, const TCHAR *formatString, ...),
                    DrawExtendFormatVStringToHandle,
                    (ExRateX, ExRateY, x, y,
                     color, fontHandle, formatString, args),
                    formatString)

// - Gets the width of a string that would be drawn.
extern DXCALL int GetDrawStringWidthToHandleA(const char *string,
                                             int strLen, int fontHandle,
                                             int VerticalFlag = DXFALSE);
extern DXCALL int GetDrawStringWidthToHandleW(const wchar_t *string,
                                             int strLen, int fontHandle,
                                             int VerticalFlag = DXFALSE);
DXUNICALL_WRAP(GetDrawStringWidthToHandle,
               (const TCHAR *string, int strLen, int fontHandle,
                int VerticalFlag = DXFALSE),
               (string, strLen, fontHandle, VerticalFlag))

extern DXCALL int GetDrawFormatVStringWidthToHandleA(int fontHandle,
                                                     const char *string,
                                                     va_list args);
extern DXCALL int GetDrawFormatVStringWidthToHandleW(int fontHandle,
                                                     const wchar_t *string,
                                                     va_list args);
DXUNICALL_VA_WRAPTO(GetDrawFormatStringWidthToHandle,
                    (int fontHandle, const TCHAR *string, ...),
                    GetDrawFormatVStringWidthToHandle,
                    (fontHandle, string, args),
                    string)

// - Gets the scaled width of a string that would be drawn.
extern DXCALL int GetDrawExtendStringWidthToHandleA(double ExRateX,
                                                    const char *string,
                                                    int strLen,
                                                    int fontHandle,
                                                    int VerticalFlag = DXFALSE);
extern DXCALL int GetDrawExtendStringWidthToHandleW(double ExRateX,
                                                    const wchar_t *string,
                                                    int strLen,
                                                    int fontHandle,
                                                    int VerticalFlag = DXFALSE);
DXUNICALL_WRAP(GetDrawExtendStringWidthToHandle,
               (double ExRateX, const TCHAR *string,
                int strLen, int fontHandle, int VerticalFlag = DXFALSE),
               (ExRateX, string, strLen, fontHandle, VerticalFlag))

extern DXCALL int GetDrawExtendFormatVStringWidthToHandleA(
                                                   double ExRateX,
                                                   int fontHandle,
                                                   const char *string,
                                                   va_list args);
extern DXCALL int GetDrawExtendFormatVStringWidthToHandleW(
                                                   double ExRateX,
                                                   int fontHandle,
                                                   const wchar_t *string,
                                                   va_list args);
DXUNICALL_VA_WRAPTO(GetDrawExtendFormatStringWidthToHandle,
                    (double ExRateX, int fontHandle, const TCHAR *string, ...),
                    GetDrawExtendFormatVStringWidthToHandle,
                    (ExRateX, fontHandle, string, args),
                    string)

// - Gets the font handle's size.
extern DXCALL int GetFontSizeToHandle(int fontHandle);

// - Gets the dimensions and advance information of the first
//   character of the given string.
extern DXCALL int GetFontCharInfoA(int fontHandle, const char *string,
                                   int *xPos, int *yPos, int *advanceX,
                                   int *width, int *height);
extern DXCALL int GetFontCharInfoW(int fontHandle, const wchar_t *string,
                                   int *xPos, int *yPos, int *advanceX,
                                   int *width, int *height);
DXUNICALL_WRAP(GetFontCharInfo,
               (int fontHandle, const TCHAR *string,
                int *xPos, int *yPos, int *advanceX,
                int *width, int *height),
               (fontHandle, string, xPos, yPos, advanceX, width, height))

// - Sets the spacing between characters for a font handle.
// Negative values are valid.
// Does not refresh font information so there is no performance cost.
extern DXCALL int SetFontSpaceToHandle(int fontSpacing, int fontHandle);

// - Creates a font handle.
// NOTICE: Use EXT_MapFontFileToName to map ttf files to font names.
// fontType is one of DX_FONTTYPE_*.
//   Default fontType DX_FONTTYPE_NORMAL.
// thickness selects a mapping, but has no other purpose.
//   Default thickness is 6.
// charset is the character set to use for this handle.
//   -1 or DX_CHARSET_DEFAULT uses the current charset.
// edgeSize is valid for DX_FONTTYPE*_EDGE and may be 1 through 4.
//   Default edgeSize is 1.
// italics draws the font italicized. default is DXFALSE.
extern DXCALL int CreateFontToHandleW(const wchar_t *fontname,
                                      int size, int thickness,
                                      int fontType = -1, int charset = -1,
                                      int edgeSize = -1, int italic = DXFALSE,
                                      int handle = -1);
extern DXCALL int CreateFontToHandleA(const char *fontname,
                                      int size, int thickness,
                                      int fontType = -1, int charset = -1,
                                      int edgeSize = -1, int italic = DXFALSE,
                                      int handle = -1);
DXUNICALL_WRAP(CreateFontToHandle,
               (const TCHAR *fontname, int size, int thickness,
                int fontType = -1, int charset = -1, int edgeSize = -1,
                int italic = DXFALSE, int handle = -1),
               (fontname, size, thickness, fontType, charset, edgeSize,
                italic, handle))

// - Deletes a font handle.
extern DXCALL int DeleteFontToHandle(int handle);
// - Returns TRUE if a font handle is still valid.
extern DXCALL int CheckFontHandleValid(int fontHandle);
// - When the font handle has been deleted, sets lostFlag to -1.
extern DXCALL int SetFontLostFlag(int fontHandle, int *lostFlag);

// - Deletes all font handles.
extern DXCALL int InitFontToHandle();

// ------ "Default" font functions
// DxPortLib maintains a font handle internally for the default font.
// NOTICE: Unlike DxLib, there is no font set by default!
//
// Most of these functions do not actually exist in code, and are aliases
// that grab the current default font handle and use that. The C version
// includes some basic hardcoded aliases for external linking.
//
// Changing any font information will cause it to refresh that font handle
// the next time it is used, which is slow.
//
// DxPortLib will use this default font internally for anything that
// requires one, such as the DxLog functions.
//
// You should set this only once in your program and then use handles
// for everything else.

// - Gets the current default font handle.
// NOTICE: This will change if the font is changed.
extern DXCALL int GetDefaultFontHandle();

// - Draws a string with the default font to (x,y).
DXUNICALL_WRAPTO(DrawString,
                 (int x, int y, const TCHAR *string,
                  DXCOLOR color, DXCOLOR edgeColor = 0),
                 DrawStringToHandle,
                 (x, y, string, color, GetDefaultFontHandle(),
                  edgeColor, FALSE))
DXUNICALL_VA_WRAPTO(DrawFormatString,
                    (int x, int y, DXCOLOR color, const TCHAR *formatString, ...),
                    DrawFormatVStringToHandle,
                    (x, y, color, GetDefaultFontHandle(), formatString, args),
                    formatString)

// - Draws a scaled string with the default font.
DXUNICALL_WRAPTO(DrawExtendString,
                 (int x, int y, double ExRateX, double ExRateY,
                  const TCHAR *string, DXCOLOR color, DXCOLOR edgeColor = 0),
                 DrawExtendStringToHandle,
                 (ExRateX, ExRateY, x, y, string, color,
                  GetDefaultFontHandle(), edgeColor, FALSE))
DXUNICALL_VA_WRAPTO(DrawExtendFormatString,
                    (int x, int y, double ExRateX, double ExRateY,
                     DXCOLOR color, const TCHAR *formatString, ...),
                    DrawExtendFormatVStringToHandle,
                    (ExRateX, ExRateY, x, y, color,
                     GetDefaultFontHandle(), formatString, args),
                    formatString)

// - Gets the width of a string using the default font.
DXUNICALL_WRAPTO(GetDrawStringWidth,
                 (const TCHAR *string, int strLen, int VerticalFlag = DXFALSE),
                 GetDrawStringWidthToHandle,
                 (string, strLen, GetDefaultFontHandle(), VerticalFlag))
DXUNICALL_VA_WRAPTO(GetDrawFormatStringWidth,
                    (const TCHAR *string, ...),
                    GetDrawFormatVStringWidthToHandle,
                    (GetDefaultFontHandle(), string, args),
                    string)

// - Gets the scaled width of a string using the default font.
DXUNICALL_WRAPTO(GetDrawExtendStringWidth,
                 (double ExRateX, const TCHAR *string,
                  int strLen, int VerticalFlag = DXFALSE),
                 GetDrawExtendStringWidthToHandle,
                 (ExRateX, string, strLen,
                  GetDefaultFontHandle(), VerticalFlag))

DXUNICALL_VA_WRAPTO(GetDrawExtendFormatStringWidth,
                    (double ExRateX, const TCHAR *string, ...),
                    GetDrawExtendFormatVStringWidthToHandle,
                    (ExRateX, GetDefaultFontHandle(), string, args),
                    string)

// - Changes the default font and charset.
extern DXCALL int ChangeFontW(const wchar_t *fontName, int charSet = -1);
extern DXCALL int ChangeFontA(const char *fontName, int charSet = -1);
DXUNICALL_WRAP(ChangeFont,
               (const TCHAR *fontName, int charSet = -1),
               (fontName, charSet))

// - Changes the default font type.
extern DXCALL int ChangeFontType(int fontType);

// - Changes the default font size.
extern DXCALL int SetFontSize(int fontSize);
// - Gets the default font's size.
extern DXCALL int GetFontSize();
// - Sets the default font's thickness.
extern DXCALL int SetFontThickness(int fontThickness);
// - Sets the default font's spacing.
extern DXCALL int SetFontSpace(int fontSpace);
// - Sets the default font's name, size, and thickness all at once.
extern DXCALL int SetDefaultFontStateW(const wchar_t *fontName,
                                       int fontSize, int fontThickness);
extern DXCALL int SetDefaultFontStateA(const char *fontName,
                                       int fontSize, int fontThickness);
DXUNICALL_WRAP(SetDefaultFontState,
               (const TCHAR *fontName, int fontSize, int fontThickness),
               (fontName, fontSize, fontThickness))

// - Sets if font textures will be generated with premultiplied alpha.
// Default is FALSE.
// NOTICE: Will not change already loaded fonts.
extern DXCALL int SetFontCacheUsePremulAlphaFlag(int flag);
// - Gets if font textures will be generated with premultiplied alpha.
extern DXCALL int GetFontCacheUsePremulAlphaFlag();

#endif /* #ifndef DX_NON_FONT */

// ------------------------------------------------------------ DxAudio.cpp
#ifndef DX_NON_SOUND

// - Plays a given sound handle.
// If DX_PLAYTYPE_NORMAL is used, it will wait until the sound finished.
// If DX_PLAYTYPE_BACK is used, it will play the sound in the background.
// If DX_PLAYTYPE_LOOP is set, it will loop until it is stopped.
// If startPositionFlag is TRUE, it will reset the sound's current position.
extern DXCALL int PlaySoundMem(int soundID, int playType,
                               int startPositionFlag = DXTRUE);

// - Stops the playback of a sound handle.
extern DXCALL int StopSoundMem(int soundID);

// - Returns TRUE if a sound handle is playing.
extern DXCALL int CheckSoundMem(int soundID);

// - Sets the volume of the sound in decibels from -10000 to 0.
extern DXCALL int SetVolumeSoundMem(int volume, int soundID);
// - Sets the volume of the sound linearly from 0 to 255.
extern DXCALL int ChangeVolumeSoundMem(int volume, int soundID);
// - If TRUE, uses the older (incorrect) volume calculation mode.
// Default: FALSE.
extern DXCALL int SetUseOldVolumeCalcFlag(int volumeFlag);

// - Loads a sound file, returning a playable handle.
// WAV files are loaded whole, OGG files are streamed.
extern DXCALL int LoadSoundMemW(const wchar_t *filename,
                                int bufferNum = 3, int unionHandle = -1);
extern DXCALL int LoadSoundMemA(const char *filename,
                                int bufferNum = 3, int unionHandle = -1);
DXUNICALL_WRAP(LoadSoundMem,
               (const TCHAR *filename, int bufferNum = 3, int unionHandle = -1),
               (filename, bufferNum, unionHandle))

// - Loads two sound files, an intro segment and then a second segment.
// If this is played with DX_PLAYTYPE_LOP, only the second part will loop.
extern DXCALL int LoadSoundMem2W(const wchar_t *filename,
                                 const wchar_t *filename2);
extern DXCALL int LoadSoundMem2A(const char *filename,
                                 const char *filename2);
DXUNICALL_WRAP(LoadSoundMem2,
               (const TCHAR *filename, const TCHAR *filename2),
               (filename, filename2))

// - Deletes a sound handle.
extern DXCALL int DeleteSoundMem(int soundID, int LogOutFlag = DXFALSE);

// - Deletes all sound handles.
extern DXCALL int InitSoundMem(int LogOutFlag = DXFALSE);

// - Sets the data storage type for loaded sounds.
// NOTICE: DxPortLib currently only supports DX_SOUNDDATATYPE_MEMNOPRESS.
extern DXCALL int SetCreateSoundDataType(int soundDataType);
extern DXCALL int GetCreateSoundDataType();

#endif /* #ifndef DX_NON_SOUND */

// ----------------------------------------------------------- DxMemory.cpp
// NOTICE: DxPortLib does not actually use file/line at this time.

// - Allocates allocationSize bytes of memory.
extern DXCALL void * DxAlloc(size_t allocationSize,
                             const char *file = NULL, int line = -1);
// - Allocates allocationSize bytes of memory, and zeroes them out.
extern DXCALL void * DxCalloc(size_t allocationSize,
                              const char *file = NULL, int line = -1);
// - Resizes a block of memory, returning the pointer to its new location.
//   The previous pointer is released and becomes invalid.
extern DXCALL void * DxRealloc(void *memory, size_t allocationSize,
                               const char *file = NULL, int line = -1);
// - Frees a block of memory.
extern DXCALL void DxFree(void *memory);

}

#endif // #ifdef DXPORTLIB_DXLIB_INTERFACE

#endif
