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

/* --------------------------------------------------------------- File.c */
extern SDL_RWops *Dx_File_OpenArchiveStream(const DXCHAR *filename);
extern SDL_RWops *Dx_File_OpenDirectStream(const DXCHAR *filename);
extern SDL_RWops *Dx_File_OpenStream(const DXCHAR *filename);

extern int Dx_File_ReadFile(const DXCHAR *filename, unsigned char **dData, unsigned int *dSize);

extern int Dx_File_SetDXArchiveKeyString(const DXCHAR *keyString);
extern int Dx_File_SetDXArchiveExtension(const DXCHAR *extension);

extern int Dx_File_SetUseDXArchiveFlag(int flag);
extern int Dx_File_GetUseDXArchiveFlag();
extern int Dx_File_SetDXArchivePriority(int flag);

extern int Dx_File_DXArchivePreLoad(const DXCHAR *dxaFilename, int async);
extern int Dx_File_DXArchiveCheckIdle(const DXCHAR *dxaFilename);
extern int Dx_File_DXArchiveRelease(const DXCHAR *dxaFilename);
extern int Dx_File_DXArchiveCheckFile(const DXCHAR *dxaFilename, const DXCHAR *filename);

extern int PLEXT_FileRead_SetCharSet(int charset);

extern int Dx_FileRead_open(const DXCHAR *filename);
extern long long Dx_FileRead_size(int fileHandle);
extern int Dx_FileRead_close(int fileHandle);
extern long long Dx_FileRead_tell(int fileHandle);
extern int Dx_FileRead_seek(int fileHandle, long long position, int origin);
extern int Dx_FileRead_read(void *data, int size, int fileHandle);
extern int Dx_FileRead_eof(int fileHandle);

extern int Dx_FileRead_gets(DXCHAR *buffer, int bufferSize, int fileHandle);
extern DXCHAR Dx_FileRead_getc(int fileHandle);
extern int Dx_FileRead_vscanf(int fileHandle, const DXCHAR *format, va_list args);

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

/* ------------------------------------------------------------- Graph.c */
extern int Dx_Graph_MakeScreen(int width, int height, int hasAlphaChannel);
extern int Dx_Graph_Load(const DXCHAR *filename, int flipFlag);
extern int Dx_Graph_LoadDiv(const DXCHAR *filename, int graphCount,
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
};
#endif

#endif // #ifdef DXPORTLIB_DXLIB_INTERFACE

#endif
