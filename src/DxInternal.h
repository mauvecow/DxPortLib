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

#ifndef DX_INTERNAL_H_HEADER
#define DX_INTERNAL_H_HEADER

#include "DxBuildConfig.h"
#include "DxDefines.h"

#include <stdlib.h>
#include "SDL.h"

#ifdef __cplusplus   
extern "C" {
#endif

/* -------------------------------------------------------------- Text.c */
/* Read functions automatically advance the string pointed to.
 * Write functions return the number of bytes written.
 */
#ifdef UNICODE
extern unsigned int PL_Text_ReadUNICODEChar(const wchar_t **textRef);
extern int PL_Text_WriteUNICODEChar(wchar_t *text, unsigned int ch, int maxLen);
#endif

extern unsigned int PL_Text_ReadUTF8Char(const char **textRef);
extern int PL_Text_WriteUTF8Char(char *buffer, unsigned int ch, int maxLen);

#ifndef DXPORTLIB_NON_SJIS
extern unsigned int PL_Text_ReadSJISChar(const char **textRef);
extern int PL_Text_WriteSJISChar(char *buffer, unsigned int ch, int maxLen);
#endif /* #ifndef DXPORTLIB_NON_SJIS */

extern unsigned int PL_Text_ReadDxChar(const DXCHAR **textRef);
extern int PL_Text_WriteDxChar(DXCHAR *textRef, unsigned int ch, int maxLen);
extern unsigned int PL_Text_ReadChar(const char **textRef, int charset);
extern int PL_Text_WriteChar(char *buffer, unsigned int ch, int maxLen, int charset);

extern unsigned int PL_Text_SJISToUnicode(unsigned int sjis);
extern unsigned int PL_Text_UnicodeToSJIS(unsigned int unicode);

extern int PL_Text_DxStringToString(const DXCHAR *inString, char *outBuffer, int maxLen, int charset);
extern int PL_Text_StringToDxString(const char *inString, DXCHAR *outBuffer, int maxLen, int charset);

extern int PL_Text_DxStrlen(const DXCHAR *str);
extern DXCHAR *PL_Text_DxStrdup(const DXCHAR *str);
extern void PL_Text_DxStrncat(DXCHAR *str, const DXCHAR *catStr, int maxLen);
extern void PL_Text_DxStrncpy(DXCHAR *str, const DXCHAR *srcStr, int maxLen);
extern int PL_Text_DxStrcmp(const DXCHAR *strA, const DXCHAR *strB);
extern int PL_Text_DxStrncmp(const DXCHAR *strA, const DXCHAR *strB, int maxLen);

extern void PL_Text_DxStrncatFromString(DXCHAR *str, const char *catStr, int maxLen, int charset);
extern void PL_Text_DxStrncpyFromString(DXCHAR *str, const char *srcStr, int maxLen, int charset);

extern int PL_Text_IsIncompleteSJISChar(const char *string, int length);
extern int PL_Text_IsIncompleteUTF8Char(const char *string, int length);
extern int PL_Text_IsIncompleteMultibyte(const char *string, int length);
extern int PL_Text_SetUseCharSet(int charset);
extern int PL_Text_GetUseCharSet();

#define DXSTRLEN(str) PL_Text_DxStrlen(str)
#define DXSTRDUP(str) PL_Text_DxStrdup(str)
#define DXSTRNCPY(str, catStr, maxLen) PL_Text_DxStrncpy(str, catStr, maxLen)
#define DXSTRNCAT(str, srcStr, maxLen) PL_Text_DxStrncat(str, srcStr, maxLen)
#define DXSTRCMP(strA, strB) PL_Text_DxStrcmp(strA, strB)
#define DXSTRNCMP(strA, strB, len) PL_Text_DxStrncmp(strA, strB, len)

#define DXSTRNCATFROMSTR(str, catStr, maxLen, charset) PL_Text_DxStrncatFromString(str, catStr, maxLen, charset)
#define DXSTRNCPYFROMSTR(str, srcStr, maxLen, charset) PL_Text_DxStrncpyFromString(str, srcStr, maxLen, charset)

#ifdef UNICODE
#define DXVSNPRINTF vswprintf
#else
#define DXVSNPRINTF SDL_vsnprintf
#endif

/* ------------------------------------------------------------ Handle.c */
typedef enum {
    DXHANDLE_NONE = 0,
    DXHANDLE_TEXTURE,
    DXHANDLE_GRAPH,
    DXHANDLE_FONT,
    DXHANDLE_SOUND,
    DXHANDLE_FILE,
    DXHANDLE_FRAMEBUFFER,
    DXHANDLE_END
} HandleType;

extern void PL_Handle_Init();
extern void PL_Handle_End();

extern int PL_Handle_AcquireID(int handleType);
extern void PL_Handle_ReleaseID(int handleID, int freeData);
extern int PL_Handle_SwapHandleIDs(int handleAID, int handleBID);

extern void *PL_Handle_AllocateData(int handleID, size_t dataSize);
extern void *PL_Handle_GetData(int handleID, HandleType handleType);

extern int PL_Handle_GetFirstIDOf(HandleType handleType);
extern int PL_Handle_GetPrevID(int handleID);
extern int PL_Handle_GetNextID(int handleID);
extern int PL_Handle_SetDeleteFlag(int handleID, int *deleteFlag);

/* ------------------------------------------------------------ Window.c */
extern int PL_windowWidth; /* read only */
extern int PL_windowHeight; /* read only */

extern int PL_Window_ResetSettings();
extern int PL_Window_Init(void);
extern int PL_Window_End(void);
extern int PL_Window_SwapBuffers();
extern int PL_Window_ProcessMessages();

extern int PL_Window_SetFullscreen(int isFullscreen);
extern int PL_Window_SetDimensions(int width, int height, int colorDepth, int refreshRate);
extern int PL_Window_SetTitle(const DXCHAR *titleString);

extern int PL_Window_SetWindowResizeFlag(int flag);

extern int PL_Window_SetMouseDispFlag(int flag);
extern int PL_Window_GetMouseDispFlag();
extern int PL_Window_GetMousePoint(int *xPosition, int *yPosition);
extern int PL_Window_SetMousePoint(int xPosition, int yPosition);
extern int PL_Window_GetMouseInput();

extern int PL_Window_SetWaitVSyncFlag(int flag);
extern int PL_Window_GetWaitVSyncFlag();

extern int PL_Window_SetAlwaysRunFlag(int flag);
extern int PL_Window_GetAlwaysRunFlag();

extern int PLEXT_Window_SetIconImageFile(const DXCHAR *filename);

/* ------------------------------------------------------------- Draw.c */

extern int PL_EXT_Draw_RectGraphFastF(
                            float dx, float dy, float dw, float dh,
                            int sx, int sy, int sw, int sh,
                            int graphID, int blendFlag);

extern int PL_Draw_Pixel(int x, int y, DXCOLOR color);

extern int PL_Draw_Line(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness);
extern int PL_Draw_LineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness);

extern int PL_Draw_Box(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag);
extern int PL_Draw_BoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag);

extern int PL_Draw_Circle(int x, int y, int r, DXCOLOR color, int fillFlag);
extern int PL_Draw_CircleF(float x, float y, float r, DXCOLOR color, int fillFlag);
extern int PL_Draw_Oval(int x, int y, int rx, int ry, DXCOLOR color, int fillFlag);
extern int PL_Draw_OvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag);

extern int PL_Draw_Triangle(int x1, int y1, int x2, int y2,
                            int x3, int y3, DXCOLOR color, int fillFlag);
extern int PL_Draw_TriangleF(float x1, float y1, float x2, float y2,
                             float x3, float y3, DXCOLOR color, int fillFlag);
extern int PL_Draw_Quadrangle(int x1, int y1, int x2, int y2,
                              int x3, int y3, int x4, int y4,
                              DXCOLOR color, int fillFlag);
extern int PL_Draw_QuadrangleF(float x1, float y1, float x2, float y2,
                               float x3, float y3, float x4, float y4,
                               DXCOLOR color, int fillFlag);

extern int PL_Draw_Graph(int x, int y, int graphID, int blendFlag);
extern int PL_Draw_GraphF(float x, float y, int graphID, int blendFlag);

extern int PL_Draw_ExtendGraph(int x1, int y1, int x2, int y2, int graphID, int blendFlag);
extern int PL_Draw_ExtendGraphF(float x1, float y1, float x2, float y2, int graphID, int blendFlag);

extern int PL_Draw_RectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                             int graphID, int blendFlag, int turnFlag);
extern int PL_Draw_RectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                              int graphID, int blendFlag, int turnFlag);
extern int PL_Draw_RectExtendGraph(int dx1, int dy1, int dx2, int dy2,
                                   int sx, int sy, int sw, int sh,
                                   int graphID, int blendFlag, int turnFlag);
extern int PL_Draw_RectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                                    int sx, int sy, int sw, int sh,
                                    int graphID, int blendFlag, int turnFlag);
extern int PL_Draw_RotaGraph(int x, int y, 
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn);
extern int PL_Draw_RotaGraphF(float x, float y, 
                              double scaleFactor, double angle,
                              int graphID, int blendFlag, int turn);
extern int PL_Draw_RotaGraph2(int x, int y, int cx, int cy,
                              double scaleFactor, double angle,
                              int graphID, int blendFlag, int turn);
extern int PL_Draw_RotaGraph2F(float x, float y, float cx, float cy,
                               double scaleFactor, double angle,
                               int graphID, int blendFlag, int turn);
extern int PL_Draw_RotaGraph3(int x, int y, int cx, int cy,
                              double xScaleFactor, double yScaleFactor, double angle,
                              int graphID, int blendFlag, int turn);
extern int PL_Draw_RotaGraph3F(float x, float y, float cx, float cy,
                               double xScaleFactor, double yScaleFactor, double angle,
                               int graphID, int blendFlag, int turn);

extern int PL_Draw_RectRotaGraphF(float x, float y,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern int PL_Draw_RectRotaGraph(int x, int y, 
                          int sx, int sy, int sw, int sh,
                          double scaleFactor, double angle,
                          int graphID, int blendFlag, int turn);
extern int PL_Draw_RectRotaGraph2F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn);
extern int PL_Draw_RectRotaGraph2(int x, int y, int cx, int cy,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern int PL_Draw_RectRotaGraph3F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double xScaleFactor, double yScaleFactor,
                            double angle,
                            int graphID, int blendFlag, int turn);
extern int PL_Draw_RectRotaGraph3(int x, int y,
                           int sx, int sy, int sw, int sh,
                           int cx, int cy,
                           double xScaleFactor, double yScaleFactor,
                           double angle,
                           int graphID, int blendFlag, int turn);

extern int PL_Draw_TurnGraph(int x, int y, int graphID, int blendFlag);
extern int PL_Draw_TurnGraphF(float x, float y, int graphID, int blendFlag);

extern int PL_Draw_ModiGraph(int x1, int y1, int x2, int y2,
                             int x3, int y3, int x4, int y4,
                             int graphID, int blendFlag);
extern int PL_Draw_ModiGraphF(float x1, float y1, float x2, float y2,
                              float x3, float y3, float x4, float y4,
                              int graphID, int blendFlag);

extern int PL_Draw_SetDrawArea(int x1, int y1, int x2, int y2);

extern int PL_Draw_SetDrawMode(int drawMode);
extern int PL_Draw_GetDrawMode();
extern int PL_Draw_SetDrawBlendMode(int blendMode, int alpha);
extern int PL_Draw_GetDrawBlendMode(int *blendMode, int *alpha);
extern int PL_Draw_SetBright(int redBright, int greenBright, int blueBright);
extern int PL_Draw_GetBright(int *redBright, int *greenBright, int *blueBright);
extern int PL_Draw_SetBasicBlendFlag(int blendFlag);

extern void PL_Draw_ResizeWindow(int width, int height);
extern void PL_Draw_Refresh(SDL_Window *window, const SDL_Rect *targetRect);
extern void PL_Draw_SwapBuffers(SDL_Window *window, const SDL_Rect *targetRect);
extern void PL_Draw_Init(SDL_Window *window, int width, int height, int vsyncFlag);
extern void PL_Draw_End();
extern int PL_Draw_ResetSettings();

/* ------------------------------------------------------------- Graph.c */
extern int PL_Graph_Load(const DXCHAR *filename, int flipFlag);
extern int PL_Graph_LoadDiv(const DXCHAR *filename, int graphCount,
                            int xCount, int yCount, int xSize, int ySize,
                            int *handleBuf, int textureFlag, int flipFlag);
extern int PL_Graph_CreateFromSurface(SDL_Surface *surface);
extern int PL_Graph_FromTexture(int textureID, SDL_Rect rect);
extern int PL_Graph_Delete(int graphID);
extern int PL_Graph_DeleteSharingGraph(int graphID);
extern int PL_Graph_GetSize(int graphID, int *w, int *h);
extern int PL_Graph_Derivation(int x, int y, int w, int h, int srcGraphID);
extern int PL_Graph_GetNum();

extern int PL_Graph_SetTransColor(int r, int g, int b);
extern int PL_Graph_GetTransColor(int *r, int *g, int *b);
extern int PL_Graph_SetUseTransColor(int flag);

extern int PL_Graph_InitGraph();

extern int PL_Graph_ResetSettings();
extern void PL_Graph_End();

extern int PL_Graph_GetTextureID(int graphID, SDL_Rect *rect);

/* ----------------------------------------------------------- Texture.c */
extern int PL_Texture_CreateFromSurface(SDL_Surface *surface);
extern int PL_Texture_CreateFromDimensions(int width, int height);

extern int PL_Texture_BlitSurface(int textureID, SDL_Surface *surface, const SDL_Rect *rect);

extern int PL_Texture_AddRef(int textureID);
extern int PL_Texture_Release(int textureID);

/* -------------------------------------------------------------- Font.c */
/* Handle font functions */
extern int PL_Font_DrawStringToHandle(int x, int y, const DXCHAR *string,
                                      DXCOLOR color, int fontHandle, DXCOLOR edgeColor);
extern int PL_Font_DrawFormatStringToHandle(int x, int y, DXCOLOR color, int fontHandle,
                                            const DXCHAR *string, va_list args);
extern int PL_Font_DrawExtendStringToHandle(int x, int y, double ExRateX, double ExRateY,
                                            const DXCHAR *string,
                                            DXCOLOR color, int fontHandle, DXCOLOR edgeColor);
extern int PL_Font_DrawExtendFormatStringToHandle(int x, int y, double ExRateX, double ExRateY,
                                                  DXCOLOR color, int fontHandle,
                                                  const DXCHAR *string, va_list args);

extern int PL_Font_GetDrawStringWidthToHandle(const DXCHAR *string, int strLen, int fontHandle);
extern int PL_Font_GetDrawFormatStringWidthToHandle(int fontHandle, const DXCHAR *string, va_list args);
extern int PL_Font_GetDrawExtendStringWidthToHandle(double ExRateX, const DXCHAR *string, int strLen, int fontHandle);
extern int PL_Font_GetDrawExtendFormatStringWidthToHandle(double ExRateX, int fontHandle, const DXCHAR *string, va_list args);

extern int PL_Font_GetFontSizeToHandle(int fontHandle);
extern int PL_Font_GetFontCharInfo(int fontHandle, const DXCHAR *string,
                              int *xPos, int *yPos, int *advanceX, int *width, int *height);
extern int PL_Font_SetFontSpaceToHandle(int fontSpacing, int fontHandle);

extern int PL_Font_CreateFontToHandle(const DXCHAR *fontname,
                                 int size, int thickness, int fontType, int charset,
                                 int edgeSize, int italic
                                );
extern int PL_Font_DeleteFontToHandle(int handle);
extern int PL_Font_CheckFontHandleValid(int fontHandle);
extern int PL_Font_SetFontLostFlag(int fontHandle, int *lostFlag);

extern int PL_Font_InitFontToHandle();

/* "Default" font functions */
extern int PL_Font_DrawString(int x, int y, const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor);
extern int PL_Font_DrawFormatString(int x, int y, DXCOLOR color, const DXCHAR *string, va_list args);
extern int PL_Font_DrawExtendString(int x, int y, double ExRateX, double ExRateY, const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor);
extern int PL_Font_DrawExtendFormatString(int x, int y, double ExRateX, double ExRateY, DXCOLOR color, const DXCHAR *string, va_list args);
extern int PL_Font_GetDrawStringWidth(const DXCHAR *string, int strLen);
extern int PL_Font_GetDrawFormatStringWidth(const DXCHAR *string, va_list args);
extern int PL_Font_GetDrawExtendStringWidth(double ExRateX, const DXCHAR *string, int strLen);
extern int PL_Font_GetDrawExtendFormatStringWidth(double ExRateX, const DXCHAR *string, va_list args);

extern int PL_Font_ChangeFont(const DXCHAR *string, int charSet);
extern int PL_Font_ChangeFontType(int fontType);
extern int PL_Font_SetFontSize(int fontSize);
extern int PL_Font_GetFontSize();
extern int PL_Font_SetFontThickness(int fontThickness);
extern int PL_Font_SetFontSpace(int fontSpace);
extern int PL_Font_SetDefaultFontState(const DXCHAR *fontName, int fontSize, int fontThickness);
extern int PL_Font_GetDefaultFontHandle();

/* Font mappings and upkeep. */
extern int PLEXT_Font_MapFontFileToName(const DXCHAR *filename,
                                   const DXCHAR *fontname,
                                   int thickness, int boldFlag
                                  );
extern int PLEXT_Font_InitFontMappings();

extern void PL_Font_Init();
extern void PL_Font_End();

/* --------------------------------------------------------------- DXA.c */
#ifndef DX_NOT_DXA

/* There's a forced limit of 12 characters for the key. */
#define DXA_KEY_LENGTH        12

typedef struct DXArchive DXArchive;

extern void DXA_CloseArchive(DXArchive *archive);
extern DXArchive *DXA_OpenArchive(const DXCHAR *filename, const char *keyString);
extern int DXA_PreloadArchive(DXArchive *archive);

extern void DXA_SetArchiveKey(DXArchive *archive, const char *keystring);
extern void DXA_SetArchiveKeyRaw(DXArchive *archive, const unsigned char *key);

extern int DXA_ReadFile(DXArchive *archive, const DXCHAR *filename, unsigned char **dDest, unsigned int *dSize);
extern int DXA_TestFile(DXArchive *archive, const DXCHAR *filename);

extern SDL_RWops *DXA_OpenStream(DXArchive *archive, const DXCHAR *filename);

#else /* #ifndef DX_NOT_DXA */

typedef int DXArchive;

#endif /* #ifndef DX_NOT_DXA */

/* --------------------------------------------------------------- File.c */
extern SDL_RWops *PL_File_OpenArchiveStream(const DXCHAR *filename);
extern SDL_RWops *PL_File_OpenDirectStream(const DXCHAR *filename);
extern SDL_RWops *PL_File_OpenStream(const DXCHAR *filename);

extern int PL_File_ReadFile(const DXCHAR *filename, unsigned char **dData, unsigned int *dSize);

extern int PL_File_SetDXArchiveKeyString(const DXCHAR *keyString);
extern int PL_File_SetDXArchiveExtension(const DXCHAR *extension);

extern int PL_File_SetUseDXArchiveFlag(int flag);
extern int PL_File_GetUseDXArchiveFlag();
extern int PL_File_SetDXArchivePriority(int flag);

extern int PL_File_DXArchivePreLoad(const DXCHAR *dxaFilename, int async);
extern int PL_File_DXArchiveCheckIdle(const DXCHAR *dxaFilename);
extern int PL_File_DXArchiveRelease(const DXCHAR *dxaFilename);
extern int PL_File_DXArchiveCheckFile(const DXCHAR *dxaFilename, const DXCHAR *filename);

extern int PL_FileRead_open(const DXCHAR *filename);
extern long long PL_FileRead_size(int fileHandle);
extern int PL_FileRead_close(int fileHandle);
extern long long PL_FileRead_tell(int fileHandle);
extern int PL_FileRead_seek(int fileHandle, long long position, int origin);
extern int PL_FileRead_read(void *data, int size, int fileHandle);
extern int PL_FileRead_eof(int fileHandle);

extern int PL_FileRead_gets(DXCHAR *buffer, int bufferSize, int fileHandle);
extern DXCHAR PL_FileRead_getc(int fileHandle);
extern int PL_FileRead_vscanf(int fileHandle, const DXCHAR *format, va_list args);

extern int PL_File_Init();
extern int PL_File_End();

/* --------------------------------------------------------------- Input.c */
#ifndef DX_NON_INPUT

extern int PL_Input_CheckHitKey(int dxKey);
extern int PL_Input_CheckHitKeyAll(int checkType);
extern int PL_Input_GetHitKeyStateAll(char *keyStateBuf);
extern int PL_Input_GetJoypadNum();
extern int PL_Input_GetJoypadState(int controllerIndex);
extern int PL_Input_SetJoypadInputToKeyInput(int controllerIndex, int input,
                                             int key1, int key2, int key3, int key4);

extern int PL_Input_GetJoypadDirectInputState(int controllerIndex, DINPUT_JOYSTATE *state);
extern int PL_Input_GetJoypadXInputState(int controllerIndex, XINPUT_STATE *state);

extern void PL_Input_HandleKeyDown(const SDL_Keysym *keySym);
extern void PL_Input_HandleKeyUp(const SDL_Keysym *keySym);

extern int PL_Input_GetMouseWheelRotVol(int clearFlag);
extern int PL_Input_GetMouseHWheelRotVol(int clearFlag);

extern void PL_Input_Init();
extern void PL_Input_End();

extern void PL_Input_AddController(int controllerIndex);
extern void PL_Input_RemoveController(int instanceID);
extern void PL_Input_HandleWheelMotion(int x, int y);

#endif /* #ifndef DX_NON_INPUT */

/* --------------------------------------------------------------- Memory.c */
extern void *PL_DxAlloc(size_t allocationSize, const char *file, int line);
extern void *PL_DxCalloc(size_t allocationSize, const char *file, int line);
extern void *PL_DxRealloc(void *memory, size_t allocationSize, const char *file, int line);
extern void PL_DxFree(void *memory);

#define DXALLOC(size) (PL_DxAlloc(size, NULL, 0))
#define DXCALLOC(size) (PL_DxCalloc(size, NULL, 0))
#define DXREALLOC(memory,size) (PL_DxRealloc(memory, size, NULL, 0))
#define DXFREE(memory) (PL_DxFree(memory))

/* --------------------------------------------------------------- RNG.c */
extern int PL_Random_Get(int maxValue);
extern int PL_Random_Seed(int randomSeed);

/* --------------------------------------------------------------- Audio.c */
#ifndef DX_NON_SOUND

extern int PL_LoadSoundMem(const DXCHAR *filename);
extern int PL_LoadSoundMem2(const DXCHAR *filename, const DXCHAR *filename2);
extern int PL_DeleteSoundMem(int soundID);
extern int PL_PlaySoundMem(int soundID, int playType, int startPositionFlag);
extern int PL_StopSoundMem(int soundID);
extern int PL_CheckSoundMem(int soundID);
extern int PL_SetVolumeSoundMem(int volume, int soundID);
extern int PL_ChangeVolumeSoundMem(int volume, int soundID);
extern int PL_SetUseOldVolumeCalcFlag(int volumeFlag);
extern int PL_InitSoundMem();
extern int PL_SetCreateSoundDataType(int soundDataType);
extern int PL_GetCreateSoundDataType();

extern int PL_Audio_ResetSettings();
extern int PL_Audio_Init();
extern int PL_Audio_End();

#endif /* #ifndef DX_NON_SOUND */

#ifdef __cplusplus
};
#endif

#endif
