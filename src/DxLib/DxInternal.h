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

#ifndef DXPORTLIB_DXINTERNAL_H_HEADER
#define DXPORTLIB_DXINTERNAL_H_HEADER

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_DXLIB_INTERFACE

#include "DxDefines.h"

#ifdef __cplusplus   
extern "C" {
#endif

/* ---------------------------------------------------------------Globals */
extern int g_DxUseCharSet;

/* --------------------------------------------------------------- File.c */
extern SDL_RWops *Dx_File_OpenArchiveStream(const char *filename);
extern SDL_RWops *Dx_File_OpenDirectStream(const char *filename);
extern SDL_RWops *Dx_File_OpenStream(const char *filename);

extern int Dx_File_ReadFile(const char *filename, unsigned char **dData, unsigned int *dSize);

extern int Dx_File_SetDXArchiveKeyString(const char *keyString);
extern int Dx_File_SetDXArchiveExtension(const char *extension);

extern int Dx_File_SetUseDXArchiveFlag(int flag);
extern int Dx_File_GetUseDXArchiveFlag();
extern int Dx_File_SetDXArchivePriority(int flag);

extern int Dx_File_DXArchivePreLoad(const char *dxaFilename, int async);
extern int Dx_File_DXArchiveCheckIdle(const char *dxaFilename);
extern int Dx_File_DXArchiveRelease(const char *dxaFilename);
extern int Dx_File_DXArchiveCheckFile(const char *dxaFilename, const char *filename);

extern int PLEXT_FileRead_SetCharSet(int charset);

extern int Dx_FileRead_open(const char *filename);
extern int64_t Dx_FileRead_size(const char *filename);
extern int Dx_FileRead_close(int fileHandle);
extern int64_t Dx_FileRead_tell(int fileHandle);
extern int Dx_FileRead_seek(int fileHandle, int64_t position, int origin);
extern int Dx_FileRead_read(void *data, int size, int fileHandle);
extern int Dx_FileRead_eof(int fileHandle);

extern int Dx_FileRead_getsA(char *buffer, int bufferSize, int fileHandle);
extern char Dx_FileRead_getcA(int fileHandle);
extern int Dx_FileRead_vscanfA(int fileHandle, const char *format, va_list args);

extern int Dx_File_Init();
extern int Dx_File_End();

/* ------------------------------------------------------------- DxDXA.c */
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

/* ------------------------------------------------------------- Draw.c */

extern int Dx_EXT_Draw_RectGraphFastF(
                            float dx, float dy, float dw, float dh,
                            int sx, int sy, int sw, int sh,
                            int graphID, int blendFlag);

extern int Dx_Draw_Pixel(int x, int y, DXCOLOR color);

extern int Dx_Draw_Line(int x1, int y1, int x2, int y2, DXCOLOR color, int thickness);
extern int Dx_Draw_LineF(float x1, float y1, float x2, float y2, DXCOLOR color, int thickness);

extern int Dx_Draw_Box(int x1, int y1, int x2, int y2, DXCOLOR color, int FillFlag);
extern int Dx_Draw_BoxF(float x1, float y1, float x2, float y2, DXCOLOR color, int FillFlag);

extern int Dx_Draw_Circle(int x, int y, int r, DXCOLOR color, int fillFlag);
extern int Dx_Draw_CircleF(float x, float y, float r, DXCOLOR color, int fillFlag);
extern int Dx_Draw_Oval(int x, int y, int rx, int ry, DXCOLOR color, int fillFlag);
extern int Dx_Draw_OvalF(float x, float y, float rx, float ry, DXCOLOR color, int fillFlag);

extern int Dx_Draw_Triangle(int x1, int y1, int x2, int y2,
                            int x3, int y3, DXCOLOR color, int fillFlag);
extern int Dx_Draw_TriangleF(float x1, float y1, float x2, float y2,
                             float x3, float y3, DXCOLOR color, int fillFlag);
extern int Dx_Draw_Quadrangle(int x1, int y1, int x2, int y2,
                              int x3, int y3, int x4, int y4,
                              DXCOLOR color, int fillFlag);
extern int Dx_Draw_QuadrangleF(float x1, float y1, float x2, float y2,
                               float x3, float y3, float x4, float y4,
                               DXCOLOR color, int fillFlag);

extern int Dx_Draw_Graph(int x, int y, int graphID, int blendFlag);
extern int Dx_Draw_GraphF(float x, float y, int graphID, int blendFlag);

extern int Dx_Draw_ExtendGraph(int x1, int y1, int x2, int y2, int graphID, int blendFlag);
extern int Dx_Draw_ExtendGraphF(float x1, float y1, float x2, float y2, int graphID, int blendFlag);

extern int Dx_Draw_RectGraph(int dx, int dy, int sx, int sy, int sw, int sh,
                             int graphID, int blendFlag, int turnFlag);
extern int Dx_Draw_RectGraphF(float dx, float dy, int sx, int sy, int sw, int sh,
                              int graphID, int blendFlag, int turnFlag);
extern int Dx_Draw_RectExtendGraph(int dx1, int dy1, int dx2, int dy2,
                                   int sx, int sy, int sw, int sh,
                                   int graphID, int blendFlag, int turnFlag);
extern int Dx_Draw_RectExtendGraphF(float dx1, float dy1, float dx2, float dy2,
                                    int sx, int sy, int sw, int sh,
                                    int graphID, int blendFlag, int turnFlag);
extern int Dx_Draw_RotaGraph(int x, int y, 
                             double scaleFactor, double angle,
                             int graphID, int blendFlag, int turn);
extern int Dx_Draw_RotaGraphF(float x, float y, 
                              double scaleFactor, double angle,
                              int graphID, int blendFlag, int turn);
extern int Dx_Draw_RotaGraph2(int x, int y, int cx, int cy,
                              double scaleFactor, double angle,
                              int graphID, int blendFlag, int turn);
extern int Dx_Draw_RotaGraph2F(float x, float y, float cx, float cy,
                               double scaleFactor, double angle,
                               int graphID, int blendFlag, int turn);
extern int Dx_Draw_RotaGraph3(int x, int y, int cx, int cy,
                              double xScaleFactor, double yScaleFactor, double angle,
                              int graphID, int blendFlag, int turn);
extern int Dx_Draw_RotaGraph3F(float x, float y, float cx, float cy,
                               double xScaleFactor, double yScaleFactor, double angle,
                               int graphID, int blendFlag, int turn);

extern int Dx_Draw_RectRotaGraphF(float x, float y,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern int Dx_Draw_RectRotaGraph(int x, int y, 
                          int sx, int sy, int sw, int sh,
                          double scaleFactor, double angle,
                          int graphID, int blendFlag, int turn);
extern int Dx_Draw_RectRotaGraph2F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double scaleFactor, double angle,
                            int graphID, int blendFlag, int turn);
extern int Dx_Draw_RectRotaGraph2(int x, int y, int cx, int cy,
                           int sx, int sy, int sw, int sh,
                           double scaleFactor, double angle,
                           int graphID, int blendFlag, int turn);
extern int Dx_Draw_RectRotaGraph3F(float x, float y,
                            int sx, int sy, int sw, int sh,
                            float cx, float cy,
                            double xScaleFactor, double yScaleFactor,
                            double angle,
                            int graphID, int blendFlag, int turn);
extern int Dx_Draw_RectRotaGraph3(int x, int y,
                           int sx, int sy, int sw, int sh,
                           int cx, int cy,
                           double xScaleFactor, double yScaleFactor,
                           double angle,
                           int graphID, int blendFlag, int turn);

extern int Dx_Draw_TurnGraph(int x, int y, int graphID, int blendFlag);
extern int Dx_Draw_TurnGraphF(float x, float y, int graphID, int blendFlag);

extern int Dx_Draw_ModiGraph(int x1, int y1, int x2, int y2,
                             int x3, int y3, int x4, int y4,
                             int graphID, int blendFlag);
extern int Dx_Draw_ModiGraphF(float x1, float y1, float x2, float y2,
                              float x3, float y3, float x4, float y4,
                              int graphID, int blendFlag);

extern int Dx_Draw_SetDrawArea(int x1, int y1, int x2, int y2);

extern int Dx_Draw_SetDrawMode(int drawMode);
extern int Dx_Draw_GetDrawMode();
extern int Dx_Draw_SetDrawBlendMode(int blendMode, int alpha);
extern int Dx_Draw_GetDrawBlendMode(int *blendMode, int *alpha);
extern int Dx_Draw_SetBright(int redBright, int greenBright, int blueBright);
extern int Dx_Draw_GetBright(int *redBright, int *greenBright, int *blueBright);
extern int Dx_Draw_SetBasicBlendFlag(int blendFlag);

extern int Dx_Draw_SetBackgroundColor(int red, int green, int blue);
extern int Dx_Draw_ClearDrawScreen(const RECT *clearRect);

extern int Dx_Draw_ResetSettings();

extern int Dx_Draw_UpdateDrawScreen();
extern int Dx_Draw_FlushCache();
extern int Dx_Draw_InitCache();
extern int Dx_Draw_DestroyCache();

extern int Dx_Draw_ForceUpdate();

extern int Dx_Draw_SetDrawScreen(int drawScreen);
extern int Dx_Draw_GetDrawScreen();
extern int Dx_Draw_ResetDrawScreen();

/* -------------------------------------------------------------- Font.c */
/* Handle font functions */
extern int Dx_Font_DrawStringToHandle(int x, int y, const DXCHAR *string,
                                      DXCOLOR color, int fontHandle, DXCOLOR edgeColor);
extern int Dx_Font_DrawFormatStringToHandle(int x, int y, DXCOLOR color, int fontHandle,
                                            const DXCHAR *string, va_list args);
extern int Dx_Font_DrawExtendStringToHandle(int x, int y, double ExRateX, double ExRateY,
                                            const DXCHAR *string,
                                            DXCOLOR color, int fontHandle, DXCOLOR edgeColor);
extern int Dx_Font_DrawExtendFormatStringToHandle(int x, int y, double ExRateX, double ExRateY,
                                                  DXCOLOR color, int fontHandle,
                                                  const DXCHAR *string, va_list args);

extern int Dx_Font_GetDrawStringWidthToHandle(const DXCHAR *string, int strLen, int fontHandle);
extern int Dx_Font_GetDrawFormatStringWidthToHandle(int fontHandle, const DXCHAR *string, va_list args);
extern int Dx_Font_GetDrawExtendStringWidthToHandle(double ExRateX, const DXCHAR *string, int strLen, int fontHandle);
extern int Dx_Font_GetDrawExtendFormatStringWidthToHandle(double ExRateX, int fontHandle, const DXCHAR *string, va_list args);

extern int Dx_Font_GetFontSizeToHandle(int fontHandle);
extern int Dx_Font_GetFontCharInfo(int fontHandle, const DXCHAR *string,
                              int *xPos, int *yPos, int *advanceX, int *width, int *height);
extern int Dx_Font_SetFontSpaceToHandle(int fontSpacing, int fontHandle);

extern int Dx_Font_CreateFontToHandle(const DXCHAR *fontname,
                                 int size, int thickness, int fontType, int charset,
                                 int edgeSize, int italic
                                );
extern int Dx_Font_DeleteFontToHandle(int handle);
extern int Dx_Font_CheckFontHandleValid(int fontHandle);
extern int Dx_Font_SetFontLostFlag(int fontHandle, int *lostFlag);

extern int Dx_Font_InitFontToHandle();

/* "Default" font functions */
extern int Dx_Font_DrawString(int x, int y, const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor);
extern int Dx_Font_DrawFormatString(int x, int y, DXCOLOR color, const DXCHAR *string, va_list args);
extern int Dx_Font_DrawExtendString(int x, int y, double ExRateX, double ExRateY, const DXCHAR *string, DXCOLOR color, DXCOLOR edgeColor);
extern int Dx_Font_DrawExtendFormatString(int x, int y, double ExRateX, double ExRateY, DXCOLOR color, const DXCHAR *string, va_list args);
extern int Dx_Font_GetDrawStringWidth(const DXCHAR *string, int strLen);
extern int Dx_Font_GetDrawFormatStringWidth(const DXCHAR *string, va_list args);
extern int Dx_Font_GetDrawExtendStringWidth(double ExRateX, const DXCHAR *string, int strLen);
extern int Dx_Font_GetDrawExtendFormatStringWidth(double ExRateX, const DXCHAR *string, va_list args);

extern int Dx_Font_ChangeFont(const DXCHAR *string, int charSet);
extern int Dx_Font_ChangeFontType(int fontType);
extern int Dx_Font_SetFontSize(int fontSize);
extern int Dx_Font_GetFontSize();
extern int Dx_Font_SetFontThickness(int fontThickness);
extern int Dx_Font_SetFontSpace(int fontSpace);
extern int Dx_Font_SetDefaultFontState(const DXCHAR *fontName, int fontSize, int fontThickness);
extern int Dx_Font_GetDefaultFontHandle();

/* Font mappings and upkeep. */
extern int PLEXT_Font_MapFontFileToName(const DXCHAR *filename,
                                   const DXCHAR *fontname,
                                   int thickness, int boldFlag,
                                   double exRateX, double exRateY
                                  );
extern int PLEXT_Font_InitFontMappings();

extern void Dx_Font_Init();
extern void Dx_Font_End();

extern int Dx_Font_SetFontCacheUsePremulAlphaFlag(int flag);
extern int Dx_Font_GetFontCacheUsePremulAlphaFlag();

/* ------------------------------------------------------------- Graph.c */
extern int Dx_Graph_MakeScreen(int width, int height, int hasAlphaChannel);
extern int Dx_Graph_Load(const char *filename, int flipFlag);
extern int Dx_Graph_LoadDiv(const char *filename, int graphCount,
                            int xCount, int yCount, int xSize, int ySize,
                            int *handleBuf, int textureFlag, int flipFlag);
extern int Dx_Graph_CreateFromSurface(int surfaceID);
extern int Dx_Graph_FromTexture(int textureID, PLRect rect);
extern int Dx_Graph_Delete(int graphID);
extern int Dx_Graph_DeleteSharingGraph(int graphID);
extern int Dx_Graph_GetSize(int graphID, int *w, int *h);
extern int Dx_Graph_Derivation(int x, int y, int w, int h, int srcGraphID);
extern int Dx_Graph_GetNum();

extern int Dx_Graph_SetTransColor(int r, int g, int b);
extern int Dx_Graph_GetTransColor(int *r, int *g, int *b);
extern int Dx_Graph_SetUseTransColor(int flag);

extern int Dx_Graph_SetUsePremulAlphaConvertLoad(int flag);

extern int Dx_Graph_SetWrap(int graphID, int wrapFlag);

extern int Dx_Graph_InitGraph();

extern int Dx_Graph_ResetSettings();
extern void Dx_Graph_End();

extern int Dx_Graph_GetTextureID(int graphID, PLRect *rect);
extern int Dx_Graph_GetTextureInfo(int graphID, int *dTextureRefID,
                                   PLRect *rect, float *xMult, float *yMult);

#ifdef __cplusplus
}
#endif

#endif /* #ifdef DXPORTLIB_DXLIB_INTERFACE */

#endif
