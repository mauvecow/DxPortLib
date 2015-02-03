/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2014 Patrick McCarthy <mauve@sandwich.net>
  
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

#ifndef DXPORTLIB_PLINTERNAL_H_HEADER
#define DXPORTLIB_PLINTERNAL_H_HEADER

#include "DxBuildConfig.h"
#include "DxDefines.h"

#include <stdlib.h>
#include "SDL.h"

#ifdef __cplusplus   
extern "C" {
#endif

/* some helpful defines */
#ifndef offsetof
#  define offsetof(x, y) ((int)&(((x *)0)->y))
#endif

#ifndef elementsof
#  define elementsof(x) (sizeof(x) / sizeof(x[0]))
#endif

/* ------------------------------------------------------------ Platform */
extern void PL_Platform_Boot();
extern void PL_Platform_Init();
extern void PL_Platform_Finish();
extern int PL_Platform_GetTicks();
extern void PL_Platform_Wait(int ticks);

extern int PL_Platform_FileOpenReadDirect(const DXCHAR *filename);
extern int PL_Platform_GetSaveFolder(DXCHAR *buffer, int bufferLength,
                                     const DXCHAR *org, const DXCHAR *app);

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
extern unsigned int PL_Text_ToSJIS(int ch);
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
extern int PL_Text_DxStrcasecmp(const DXCHAR *strA, const DXCHAR *strB);
extern int PL_Text_DxStrncmp(const DXCHAR *strA, const DXCHAR *strB, int maxLen);
extern int PL_Text_DxStrncasecmp(const DXCHAR *strA, const DXCHAR *strB, int maxLen);
extern const DXCHAR *PL_Text_DxStrstr(const DXCHAR *strA, const DXCHAR *strB);
extern const DXCHAR *PL_Text_DxStrcasestr(const DXCHAR *strA, const DXCHAR *strB);
extern int PL_Text_DxStrTestExt(const DXCHAR *str, const DXCHAR *ext);

extern void PL_Text_DxStrncatFromString(DXCHAR *str, const char *catStr, int maxLen, int charset);
extern void PL_Text_DxStrncpyFromString(DXCHAR *str, const char *srcStr, int maxLen, int charset);

extern int PL_Text_IsIncompleteSJISChar(const char *string, int length);
extern int PL_Text_IsIncompleteUTF8Char(const char *string, int length);
extern int PL_Text_IsIncompleteMultibyte(const char *string, int length, int charset);
extern int PL_Text_SetUseCharSet(int charset);
extern int PL_Text_GetUseCharSet();

#define DXSTRLEN(str) PL_Text_DxStrlen(str)
#define DXSTRDUP(str) PL_Text_DxStrdup(str)
#define DXSTRNCPY(str, catStr, maxLen) PL_Text_DxStrncpy(str, catStr, maxLen)
#define DXSTRNCAT(str, srcStr, maxLen) PL_Text_DxStrncat(str, srcStr, maxLen)
#define DXSTRCMP(strA, strB) PL_Text_DxStrcmp(strA, strB)
#define DXSTRCASECMP(strA, strB) PL_Text_DxStrcasecmp(strA, strB)
#define DXSTRNCMP(strA, strB, len) PL_Text_DxStrncmp(strA, strB, len)
#define DXSTRNCASECMP(strA, strB, len) PL_Text_DxStrncasecmp(strA, strB, len)
#define DXSTRSTR(strA, strB) PL_Text_DxStrstr(strA, strB)
#define DXSTRCASESTR(strA, strB) PL_Text_DxStrcasestr(strA, strB)
#define DXSTRTESTEXT(str, ext) PL_Text_DxStrTestExt(str, ext)

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
    DXHANDLE_SURFACE,
    DXHANDLE_FONT,
    DXHANDLE_SOUND,
    DXHANDLE_FILE,
    DXHANDLE_PLFILE,
    DXHANDLE_FRAMEBUFFER,
    DXHANDLE_VERTEXBUFFER,
    DXHANDLE_INDEXBUFFER,
    DXHANDLE_SHADER,
    
    /* dxlib handles */
#ifdef DXPORTLIB_DXLIB_INTERFACE
#endif
    
    /* luna handles */
#ifdef DXPORTLIB_LUNA_INTERFACE
    DXHANDLE_LUNASPRITE,
    DXHANDLE_LUNAFONTSPRITE,
    DXHANDLE_LUNACAMERA,
#endif
    
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

/* ------------------------------------------------------------ File.c */
typedef int (*PLFileOpenFileFunction)(const DXCHAR *filename);

typedef struct _PL_FileFunctions {
    long long (*getSize)(void *userdata);
    long long (*tell)(void *userdata);
    int (*seek)(void *userdata, long long position, int origin);
    int (*read)(void *userdata, void *data, int size);
    int (*close)(void *userdata);
} PL_FileFunctions;

extern void PL_File_SetOpenReadFunction(PLFileOpenFileFunction func);
extern int PL_File_OpenRead(const DXCHAR *filename);
extern int PL_File_CreateHandle(const PL_FileFunctions *funcs, void *userdata);
extern int PL_File_CreateHandleFromMemory(void *data, int length, int freeOnClose);
extern int PL_File_CreateHandleSubsection(int srcFileHandle,
                                          long long start, long long size,
                                          int closeOnClose);
extern long long PL_File_GetSize(int fileHandle);
extern long long PL_File_Tell(int fileHandle);
extern long long PL_File_Seek(int fileHandle, long long position, int origin);
extern long long PL_File_Read(int fileHandle, void *data, int size);
extern int PL_File_IsEOF(int fileHandle);
extern int PL_File_Close(int fileHandle);
extern int PL_File_Init();
extern int PL_File_End();

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

extern int PL_Window_GetActiveFlag();
extern int PL_Window_GetWindowModeFlag();

extern int PLEXT_Window_SetIconImageFile(const DXCHAR *filename);

extern int PLEXT_Window_MessageBoxError(const DXCHAR *title, const DXCHAR *text);
extern int PLEXT_Window_MessageBoxYesNo(const DXCHAR *title, const DXCHAR *text,
                                        const DXCHAR *yes, const DXCHAR *no);

extern int PL_Window_GetFramebuffer();

/* -------------------------------------------------------------- Math.c */
typedef struct _PLMatrix {
    union {
        struct {
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        float m[4][4];
    };
} PLMatrix;

typedef struct _PLVector3 {
    float x, y, z;
} PLVector3;

extern const PLMatrix g_matrixIdentity;

PLVector3 *PL_Vector3_Set(PLVector3 *o, float x, float y, float z);
PLVector3 *PL_Vector3_Copy(PLVector3 *o, const PLVector3 *v);
float PL_Vector3_Dot(const PLVector3 *a, const PLVector3 *b);
PLVector3 *PL_Vector3_Add(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
PLVector3 *PL_Vector3_Sub(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
PLVector3 *PL_Vector3_Mul(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
PLVector3 *PL_Vector3_Cross(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
PLVector3 *PL_Vector3_Normalize(PLVector3 *o, const PLVector3 *v);

PLMatrix *PL_Matrix_Copy(PLMatrix *o, const PLMatrix *m);
PLMatrix *PL_Matrix_Transpose(PLMatrix *o, const PLMatrix *m);
PLMatrix *PL_Matrix_Multiply(PLMatrix *o, const PLMatrix *a, const PLMatrix *b);
PLMatrix *PL_Matrix_Inverse(PLMatrix *o, float *dDeterminant, const PLMatrix *m);

PLMatrix *PL_Matrix_CreateIdentity(PLMatrix *o);
PLMatrix *PL_Matrix_CreateScale(PLMatrix *o, float x, float y, float z);
PLMatrix *PL_Matrix_CreateTranslation(PLMatrix *o, float x, float y, float z);

PLMatrix *PL_Matrix_CreateOrthoLH(PLMatrix *o, float w, float h, float zNear, float zFar);
PLMatrix *PL_Matrix_CreateOrthoRH(PLMatrix *o, float w, float h, float zNear, float zFar);
PLMatrix *PL_Matrix_CreateOrthoOffCenterLH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar);
PLMatrix *PL_Matrix_CreateOrthoOffCenterRH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar);
PLMatrix *PL_Matrix_CreatePerspectiveFovLH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar);
PLMatrix *PL_Matrix_CreatePerspectiveFovRH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar);
PLMatrix *PL_Matrix_CreateLookAtLH(PLMatrix *o, const PLVector3 *eye, const PLVector3 *at, const PLVector3 *up);
PLMatrix *PL_Matrix_CreateLookAtRH(PLMatrix *o, const PLVector3 *eye, const PLVector3 *at, const PLVector3 *up);

/* ------------------------------------------------------------ Render.c */

enum PL_BlendType {
    PL_BLEND_ZERO,
    PL_BLEND_ONE,
    PL_BLEND_SRC_COLOR,
    PL_BLEND_DST_COLOR,
    PL_BLEND_SRC_ALPHA,
    PL_BLEND_DST_ALPHA,
    PL_BLEND_ONE_MINUS_SRC_COLOR,
    PL_BLEND_ONE_MINUS_DST_COLOR,
    PL_BLEND_ONE_MINUS_SRC_ALPHA,
    PL_BLEND_ONE_MINUS_DST_ALPHA
};
enum PL_BlendFunctions {
    PL_BLENDFUNC_DISABLE,
    PL_BLENDFUNC_ADD,
    PL_BLENDFUNC_RSUB,
};
enum VertexElementType {
    VERTEX_POSITION,
    VERTEX_TEXCOORD0,
    VERTEX_TEXCOORD1,
    VERTEX_TEXCOORD2,
    VERTEX_TEXCOORD3,
    VERTEX_NORMAL,
    VERTEX_COLOR
};
enum VertexElementSize {
    VERTEXSIZE_UNSIGNED_BYTE,
    VERTEXSIZE_FLOAT
};
typedef struct VertexElement {
    int vertexType;
    int size;
    int vertexElementSize;
    int offset;
} VertexElement;

typedef struct VertexDefinition {
    const VertexElement *elements;
    int elementCount;
    int vertexByteSize;
} VertexDefinition;

#define VERTEX_DEFINITION(vertex) \
static const VertexDefinition s_ ## vertex ## Definition = \
    { s_ ## vertex ## Elements, elementsof(s_ ## vertex ## Elements), sizeof(vertex) };

typedef enum _TexturePreset {
    TEX_PRESET_MODULATE,
    TEX_PRESET_DX_MULA,
    TEX_PRESET_DX_INVERT,
    TEX_PRESET_DX_X4,
    TEX_PRESET_DX_PMA,
    TEX_PRESET_DX_PMA_INVERT,
    TEX_PRESET_DX_PMA_X4,
    TEX_PRESET_END
} TexturePreset;

typedef enum _PrimitiveType {
    PL_PRIM_POINTS,
    PL_PRIM_LINES,
    PL_PRIM_TRIANGLES,
    PL_PRIM_TRIANGLEFAN,
    PL_PRIM_TRIANGLESTRIP
} PrimitiveType;

extern void PL_Render_SetBlendMode(
                int blendEquation,
                int srcBlend, int destBlend);
extern void PL_Render_SetBlendModeSeparate(
                int blendEquation,
                int srcRGBBlend, int destRGBBlend,
                int srcAlphaBlend, int destAlphaBlend);
extern void PL_Render_DisableBlend();
extern int PL_Render_EnableAlphaTest();
extern int PL_Render_DisableAlphaTest();

extern int PL_Render_SetTextureStage(unsigned int stage,
                                     int textureRefID, int textureDrawMode);
extern int PL_Render_SetTexturePresetMode(int preset,
                                     int textureRefID, int textureDrawMode);
extern int PL_Render_ClearTextures();
extern int PL_Render_ClearTexturePresetMode();

extern int PL_Render_SetScissor(int x, int y, int w, int h);
extern int PL_Render_SetScissorRect(const RECT *rect);
extern int PL_Render_DisableScissor();

extern int PL_Render_DrawVertexArray(const VertexDefinition *def,
               const char *vertexData,
               int primitiveType, int vertexStart, int vertexCount);
extern int PL_Render_DrawVertexIndexArray(const VertexDefinition *def,
               const char *vertexData,
               const unsigned short *indexData,
               int primitiveType, int indexStart, int indexCount);

extern int PL_Render_DrawVertexBuffer(const VertexDefinition *def,
               int vertexBufferHandle,
               int primitiveType, int vertexStart, int vertexCount);
extern int PL_Render_DrawVertexIndexBuffer(const VertexDefinition *def,
               int vertexBufferHandle, int indexBufferHandle,
               int primitiveType, int indexStart, int indexCount);

extern int PL_Render_SetViewport(int x, int y, int w, int h);
extern int PL_Render_SetZRange(float nearZ, float farZ);
extern int PL_Render_SetMatrices(const PLMatrix *projection, const PLMatrix *view);
extern int PL_Render_SetUntransformedFlag(int untransformedFlag);

extern int PL_Render_ClearColor(float r, float g, float b, float a);
extern int PL_Render_Clear();

extern int PL_Render_SetMatrices(const PLMatrix *projection, const PLMatrix *view);
extern int PL_Render_StartFrame();
extern int PL_Render_EndFrame();

extern int PL_Render_Init();
extern int PL_Render_End();

/* ----------------------------------------------------------- Buffers.c */

extern int PL_VertexBuffer_Create(const VertexDefinition *def,
                                  const char *vertexData, int vertexCount,
                                  int isStatic);
extern int PL_VertexBuffer_SetData(int vboHandle,
                                   const char *vertices,
                                   int start, int count);
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
extern char *PL_VertexBuffer_Lock(int vboHandle);
extern int PL_VertexBuffer_Unlock(int vboHandle, char *buffer);
#endif
extern int PL_VertexBuffer_Delete(int vboHandle);

extern int PL_IndexBuffer_Create(const unsigned short *indexData,
                                 int indexCount, int isStatic);
extern int PL_IndexBuffer_SetData(int iboHandle,
                                  const unsigned short *indices,
                                  int start, int count);
#ifndef DXPORTLIB_DRAW_OPENGL_ES2
extern unsigned short *PL_IndexBuffer_Lock(int iboHandle);
extern int PL_IndexBuffer_Unlock(int iboHandle);
#endif
extern int PL_IndexBuffer_Delete(int iboHandle);

/* ----------------------------------------------------------- Surface.c */
extern int PL_Surface_ApplyTransparentColor(int surfaceID,
                                            unsigned int color);
extern int PL_Surface_ApplyPMAToSDLSurface(SDL_Surface *sdlSurface);
extern int PL_Surface_ApplyPMAToSurface(int surfaceID);
extern int PL_Surface_FlipSurface(int surfaceID);

extern int PL_Surface_GetSize(int surfaceID, int *w, int *h);
extern int PL_Surface_HasTransparency(int surfaceID);

extern int PL_Surface_Load(const DXCHAR *filename);
extern int PL_Surface_Delete(int surfaceID);

extern int PL_Surface_ToTexture(int surfaceID);

extern int PL_Surface_GetCount();
extern int PL_Surface_InitSurface();
extern void PL_Surface_End();

/* ----------------------------------------------------------- Texture.c */
extern int PL_Texture_CreateFromSDLSurface(SDL_Surface *surface, int hasAlphaChannel);
extern int PL_Texture_CreateFromDimensions(int width, int height, int hasAlphaChannel);
extern int PL_Texture_CreateFramebuffer(int width, int height, int hasAlphaChannel);

extern int PL_Texture_BlitSurface(int textureID, SDL_Surface *surface, const SDL_Rect *rect);

extern int PL_Texture_RenderGetTextureInfo(int textureRefID, SDL_Rect *rect, float *xMult, float *yMult);

extern int PL_Texture_SetWrap(int textureRefID, int wrapState);

extern int PL_Texture_HasAlphaChannel(int textureRefID);

extern int PL_Texture_BindFramebuffer(int textureRefID);

extern int PL_Texture_AddRef(int textureID);
extern int PL_Texture_Release(int textureID);

/* -------------------------------------------------------- SaveScreen.c */
extern int PL_SaveDrawScreenToBMP(int x1, int y1, int x2, int y2,
                                  const DXCHAR *filename);
extern int PL_SaveDrawScreenToJPEG(int x1, int y1, int x2, int y2,
                                   const DXCHAR *filename,
                                   int quality, int sample2x1);
extern int PL_SaveDrawScreenToPNG(int x1, int y1, int x2, int y2,
                                  const DXCHAR *filename,
                                  int compressionLevel);

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
                                   int thickness, int boldFlag,
                                   double exRateX, double exRateY
                                  );
extern int PLEXT_Font_InitFontMappings();

extern void PL_Font_Init();
extern void PL_Font_End();

extern int PL_Font_SetFontCacheUsePremulAlphaFlag(int flag);
extern int PL_Font_GetFontCacheUsePremulAlphaFlag();

/* --------------------------------------------------------------- Input.c */
#ifndef DX_NON_INPUT

extern void PL_Input_ResetKeyBuffer();
extern int PL_Input_GetFromKeyBuffer();

extern int PL_Input_CheckHitKey(int dxKey);
extern int PL_Input_CheckHitKeyAll(int checkType);
extern int PL_Input_GetHitKeyStateAll(char *keyStateBuf);
extern int PL_Input_GetJoypadNum();
extern int PL_Input_GetJoypadState(int controllerIndex);
extern int PL_Input_SetJoypadInputToKeyInput(int controllerIndex, int input,
                                             int key1, int key2, int key3, int key4);
extern int PL_Input_GetJoypadPOVState(int controllerIndex, int povNumber);
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
extern unsigned int PL_Random_Get32();
extern int PL_Random_Get(int maxValue);
extern int PL_Random_SeedDx(int randomSeed);
extern int PL_Random_SeedLuna(int randomSeed);

/* --------------------------------------------------------------- Audio.c */
#ifndef DX_NON_SOUND

extern int PL_LoadSoundMem(const DXCHAR *filename);
extern int PL_LoadSoundMem2(const DXCHAR *filenameA, const DXCHAR *filenameB);
extern int PL_DeleteSoundMem(int soundID);
extern int PL_PlaySoundMem(int soundID, int playType, int startPositionFlag);
extern int PL_StopSoundMem(int soundID);
extern int PL_CheckSoundMem(int soundID);
extern int PL_SetVolumeSoundMemDirect(int volume, int soundID);
extern int PL_SetVolumeSoundMem(int volume, int soundID);
extern int PL_ChangeVolumeSoundMem(int volume, int soundID);
extern int PL_SetUseOldVolumeCalcFlag(int volumeFlag);
extern int PL_InitSoundMem();
extern int PL_SetCreateSoundDataType(int soundDataType);
extern int PL_GetCreateSoundDataType();
extern int PL_Audio_SetLoopSamples(int soundID,
                                   unsigned long long loopTarget,
                                   unsigned long long loopPoint);
extern int PL_Audio_SetLoopTimes(int soundID,
                                 double loopTarget,
                                 double loopPoint);
extern int PL_Audio_RemoveLoopPoints(int soundID);

extern int PL_Audio_ResetSettings();
extern int PL_Audio_Init();
extern int PL_Audio_End();

#endif /* #ifndef DX_NON_SOUND */

#ifdef __cplusplus
};
#endif

#endif
