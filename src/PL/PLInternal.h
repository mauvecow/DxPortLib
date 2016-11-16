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

#ifndef DXPORTLIB_PLINTERNAL_H_HEADER
#define DXPORTLIB_PLINTERNAL_H_HEADER

#include "DPLBuildConfig.h"
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
extern int PL_Platform_GetDateTime(DATEDATA *dateBuf);

extern int PL_Platform_FileOpenReadDirect(const char *filename);
extern int PL_Platform_FileOpenWriteDirect(const char *filename);
extern int PL_Platform_GetSaveFolder(char *buffer, int bufferLength,
                                     const char *org, const char *app,
                                     int destEncoding);

extern int PL_Platform_MessageBoxError(const char *title, const char *text);
extern int PL_Platform_MessageBoxYesNo(const char *title, const char *text,
                                       const char *yes, const char *no);

/* -------------------------------------------------------------- Text.c */
/* Read functions automatically advance the string pointed to.
 * Write functions return the number of bytes written.
 */

extern int PL_Text_ToCharset(int charset);

extern unsigned int PL_Text_ReadUTF8Char(const char **textRef);
extern int PL_Text_WriteUTF8Char(char *buffer, unsigned int ch, int bufSize);
extern int PL_Text_IsIncompleteUTF8Char(const char *string, int length);

#ifndef DXPORTLIB_NO_SJIS
extern unsigned int PL_Text_ToSJIS(int ch);
extern unsigned int PL_Text_ReadSJISChar(const char **textRef);
extern int PL_Text_WriteSJISChar(char *buffer, unsigned int ch, int bufSize);
extern int PL_Text_IsIncompleteSJISChar(const char *string, int length);
#endif /* #ifndef DXPORTLIB_NO_SJIS */

extern int PL_Text_IsIncompleteMultibyte(const char *string, int length, int charset);

extern unsigned int PL_Text_ReadChar(const char **textRef, int charset);
extern int PL_Text_WriteChar(char *buffer, unsigned int ch, int bufSize, int charset);

extern int PL_Text_Strlen(const char *dest);
extern int PL_Text_StrlenW(const wchar_t *dest);
extern char *PL_Text_Strdup(const char *dest);
extern wchar_t *PL_Text_StrdupW(const wchar_t *dest);
extern int PL_Text_Strncat(char *dest, const char *srcStr, int bufSize);
extern int PL_Text_StrncatW(wchar_t *dest, const wchar_t *srcStr, int bufSize);
extern int PL_Text_Strncpy(char *dest, const char *srcStr, int bufSize);
extern int PL_Text_StrncpyW(wchar_t *str, const wchar_t *cpyStr, int bufSize);
extern int PL_Text_Strcmp(const char *strA, const char *strB);
extern int PL_Text_StrcmpW(const wchar_t *strA, const wchar_t *strB);
extern int PL_Text_Strcasecmp(const char *strA, const char *strB);
extern int PL_Text_StrcasecmpW(const wchar_t *strA, const wchar_t *strB);
extern int PL_Text_Strncmp(const char *strA, const char *strB, int bufSize);
extern int PL_Text_StrncmpW(const wchar_t *strA, const wchar_t *strB, int bufSize);
extern int PL_Text_Strncasecmp(const char *strA, const char *strB, int bufSize);
extern int PL_Text_StrncasecmpW(const wchar_t *strA, const wchar_t *strB, int bufSize);
extern const char *PL_Text_Strstr(const char *strA, const char *strB);
extern const wchar_t *PL_Text_StrstrW(const wchar_t *strA, const wchar_t *strB);
extern const char *PL_Text_Strcasestr(const char *strA, const char *strB);
extern const wchar_t *PL_Text_StrcasestrW(const wchar_t *strA, const wchar_t *strB);
extern int PL_Text_StrTestExt(const char *str, const char *ext);
extern int PL_Text_StrTestExtW(const wchar_t *str, const wchar_t *ext);

extern int PL_Text_ConvertStrncpy(char *dest, int destCharset,
                                 const char *src, int srcCharset,
                                 int bufSize);
extern const char *PL_Text_ConvertStrncpyIfNecessary(
                                 char *dest, int destCharset,
                                 const char *src, int srcCharset,
                                 int bufSize);

extern int PL_Text_ConvertStrncat(char *dest, int destCharset,
                                  const char *srcStr, int srcCharset,
                                  int bufSize);

extern int PL_Text_WideCharToString(char *dest, int charset, const wchar_t *srcStr, int bufSize);
extern int PL_Text_StringToWideChar(wchar_t *dest, const char *srcStr, int charset, int bufSize);

extern void PL_Text_StrUpper(char *str, int charset);
extern void PL_Text_StrLower(char *str, int charset);
extern void PL_Text_StrUpperW(wchar_t *str);
extern void PL_Text_StrLowerW(wchar_t *str);

extern int PL_Text_ReadLine(char *dest, int maxLen, const char **pSrc, int destEncoding, int srcEncoding);

extern int PL_Text_Vsnprintf(char *dest, int bufSize, int charset, const char *format, va_list args);
extern int PL_Text_Snprintf(char *dest, int bufSize, int charset, const char *format, ...);
extern int PL_Text_Wvsnprintf(wchar_t *dest, int bufSize, int charset, const wchar_t *format, va_list args);
extern int PL_Text_Wsnprintf(wchar_t *dest, int bufSize, int charset, const wchar_t *format, ...);
extern int PL_Text_Vsscanf(const char *str, int charset, const char *format, va_list args);
extern int PL_Text_Sscanf(const char *str, int charset, const char *format, ...);
extern int PL_Text_Wvsscanf(const wchar_t *str, int charset, const wchar_t *format, va_list args);
extern int PL_Text_Wsscanf(const wchar_t *str, int charset, const wchar_t *format, ...);

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
    DXHANDLE_RENDERBUFFER,
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
    
    DXHANDLE_MISC,
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
typedef int (*PLFileOpenFileFunction)(const char *filename);

typedef struct _PL_FileFunctions {
    int64_t (*getSize)(void *userdata);
    int64_t (*tell)(void *userdata);
    int (*seek)(void *userdata, int64_t position, int origin);
    int (*read)(void *userdata, void *data, int size);
    int (*write)(void *userdata, void *data, int size);
    int (*close)(void *userdata);
} PL_FileFunctions;

extern void PL_File_SetOpenReadFunction(PLFileOpenFileFunction func);
extern int PL_File_OpenRead(const char *filename);
extern int PL_File_OpenWrite(const char *filename);
extern int PL_File_CreateHandle(const PL_FileFunctions *funcs, void *userdata);
extern int PL_File_CreateHandleFromMemory(void *data, int length, int freeOnClose);
extern int PL_File_CreateHandleSubsection(int srcFileHandle,
                                          int64_t start, int64_t size,
                                          int closeOnClose);
extern int64_t PL_File_GetSize(int fileHandle);
extern int64_t PL_File_Tell(int fileHandle);
extern int64_t PL_File_Seek(int fileHandle, int64_t position, int origin);
extern int64_t PL_File_Read(int fileHandle, void *data, int size);
extern int64_t PL_File_Write(int fileHandle, void *data, int size);
extern int PL_File_CopyDirect(const char *src, const char *dest, int dontCopyIfExists);
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

extern int PL_Window_SetFullscreen(int isFullscreen, int fullscreenDesktop);
extern int PL_Window_SetDimensions(int width, int height, int colorDepth, int refreshRate);
extern int PL_Window_SetTitle(const char *titleString);

extern int PL_Window_SetWindowResizeFlag(int flag);
extern int PL_Window_ChangeOnlyWindowSize(int width, int height);

extern int PL_Window_GetRefreshRate();

extern int PL_Window_SetMouseDispFlag(int flag);
extern int PL_Window_GetMouseDispFlag();
extern int PL_Window_GetMousePoint(int *xPosition, int *yPosition);
extern int PL_Window_SetMousePoint(int xPosition, int yPosition);
extern int PL_Window_GetMouseInput();
extern int PL_Window_GrabMouse(int flag);

extern int PL_Window_SetWaitVSyncFlag(int flag);
extern int PL_Window_GetWaitVSyncFlag();

extern int PL_Window_SetAlwaysRunFlag(int flag);
extern int PL_Window_GetAlwaysRunFlag();

extern int PL_Window_GetActiveFlag();
extern int PL_Window_GetWindowModeFlag();

extern int PL_Window_SetIconFromFile(const char *filename);

extern int PL_Window_BindMainFramebuffer();
extern int PL_Window_GetFramebuffer();
extern int PL_Window_SetDefaultRenderbuffer(int renderbufferID);

/* -------------------------------------------------------------- Math.c */
typedef struct _PLRect {
    int x, y, w, h;
} PLRect;
typedef union _PLMatrix {
    struct {
        float m11, m12, m13, m14;
        float m21, m22, m23, m24;
        float m31, m32, m33, m34;
        float m41, m42, m43, m44;
    } v;
    float m[4][4];
} PLMatrix;

typedef struct _PLVector3 {
    float x, y, z;
} PLVector3;

extern const PLMatrix g_matrixIdentity;

extern PLVector3 *PL_Vector3_Set(PLVector3 *o, float x, float y, float z);
extern PLVector3 *PL_Vector3_Copy(PLVector3 *o, const PLVector3 *v);
extern float PL_Vector3_Dot(const PLVector3 *a, const PLVector3 *b);
extern PLVector3 *PL_Vector3_Add(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
extern PLVector3 *PL_Vector3_Sub(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
extern PLVector3 *PL_Vector3_Mul(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
extern PLVector3 *PL_Vector3_Cross(PLVector3 *o, const PLVector3 *a, const PLVector3 *b);
extern PLVector3 *PL_Vector3_Normalize(PLVector3 *o, const PLVector3 *v);

extern PLMatrix *PL_Matrix_Copy(PLMatrix *o, const PLMatrix *m);
extern PLMatrix *PL_Matrix_Transpose(PLMatrix *o, const PLMatrix *m);
extern PLMatrix *PL_Matrix_Multiply(PLMatrix *o, const PLMatrix *a, const PLMatrix *b);
extern PLMatrix *PL_Matrix_Invert(PLMatrix *o, float *dDeterminant, const PLMatrix *m);

extern PLMatrix *PL_Matrix_CreateIdentity(PLMatrix *o);
extern PLMatrix *PL_Matrix_CreateScale(PLMatrix *o, float x, float y, float z);
extern PLMatrix *PL_Matrix_CreateTranslation(PLMatrix *o, float x, float y, float z);
extern PLMatrix *PL_Matrix_CreateRotationX(PLMatrix *o, float x);
extern PLMatrix *PL_Matrix_CreateRotationY(PLMatrix *o, float y);
extern PLMatrix *PL_Matrix_CreateRotationZ(PLMatrix *o, float z);
extern PLMatrix *PL_Matrix_CreateFromYawPitchRoll(PLMatrix *o, float yaw, float pitch, float roll);

extern PLMatrix *PL_Matrix_CreateOrthoLH(PLMatrix *o, float w, float h, float zNear, float zFar);
extern PLMatrix *PL_Matrix_CreateOrthoRH(PLMatrix *o, float w, float h, float zNear, float zFar);
extern PLMatrix *PL_Matrix_CreateOrthoOffCenterLH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar);
extern PLMatrix *PL_Matrix_CreateOrthoOffCenterRH(PLMatrix *o, float left, float right,
                                        float bottom, float top, float zNear, float zFar);
extern PLMatrix *PL_Matrix_CreatePerspectiveFovLH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar);
extern PLMatrix *PL_Matrix_CreatePerspectiveFovRH(PLMatrix *o, float fovY, float aspectRatio, float zNear, float zFar);
extern PLMatrix *PL_Matrix_CreateLookAtLH(PLMatrix *o, const PLVector3 *eye, const PLVector3 *at, const PLVector3 *up);
extern PLMatrix *PL_Matrix_CreateLookAtRH(PLMatrix *o, const PLVector3 *eye, const PLVector3 *at, const PLVector3 *up);

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
    PL_BLENDFUNC_RSUB
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

typedef enum _PresetProgramFlags {
    PL_PRESETFLAG_ALPHATEST_NONE = 0,
    
    PL_PRESETFLAG_ALPHATEST_SHIFT = 0,
    
    PL_PRESETFLAG_ALPHATEST_EQUAL = (1 << PL_PRESETFLAG_ALPHATEST_SHIFT),
    PL_PRESETFLAG_ALPHATEST_NOTEQUAL = (2 << PL_PRESETFLAG_ALPHATEST_SHIFT),
    PL_PRESETFLAG_ALPHATEST_LESS = (3 << PL_PRESETFLAG_ALPHATEST_SHIFT),
    PL_PRESETFLAG_ALPHATEST_LEQUAL = (4 << PL_PRESETFLAG_ALPHATEST_SHIFT),
    PL_PRESETFLAG_ALPHATEST_GREATER = (5 << PL_PRESETFLAG_ALPHATEST_SHIFT),
    PL_PRESETFLAG_ALPHATEST_GEQUAL = (6 << PL_PRESETFLAG_ALPHATEST_SHIFT),
    
    PL_PRESETFLAG_ALPHATEST_MASK = (7 << PL_PRESETFLAG_ALPHATEST_SHIFT)
} PresetProgramFlags;

typedef struct _PLIGraphics {
    void (*SetBlendMode)(
                int blendEquation,
                int srcBlend, int destBlend);
    void (*SetBlendModeSeparate)(
                int blendEquation,
                int srcRGBBlend, int destRGBBlend,
                int srcAlphaBlend, int destAlphaBlend);
    void (*DisableBlend)();

    int (*SetScissor)(int x, int y, int w, int h);
    int (*SetScissorRect)(const RECT *rect);
    int (*DisableScissor)();

    int (*DisableCulling)();
    int (*EnableDepthTest)();
    int (*DisableDepthTest)();
    int (*EnableDepthWrite)();
    int (*DisableDepthWrite)();

    int (*SetPresetProgram)(int preset, int flags,
                            const PLMatrix *projection, const PLMatrix *view,
                            int textureRefID, int textureDrawMode,
                            float alphaTestValue);
    int (*ClearPresetProgram)();

    int (*VertexBuffer_CreateBytes)(int vertexByteSize,
                                    const char *vertexData, int bufferSize,
                                    int isStatic);
    int (*VertexBuffer_Create)(const VertexDefinition *def,
                               const char *vertexData, int vertexCount,
                               int isStatic);
    int (*VertexBuffer_ResetBuffer)(int vboHandle);
    int (*VertexBuffer_SetDataBytes)(int vboHandle, const char *vertices,
                                     int start, int count, int resetBufferFlag);
    int (*VertexBuffer_SetData)(int vboHandle, const char *vertices,
                                int start, int count, int resetBufferFlag);
    char *(*VertexBuffer_Lock)(int vboHandle);
    int (*VertexBuffer_Unlock)(int vboHandle, char *buffer);
    int (*VertexBuffer_Delete)(int vboHandle);

    int (*IndexBuffer_Create)(const unsigned short *indexData,
                                 int indexCount, int isStatic);
    int (*IndexBuffer_ResetBuffer)(int iboHandle);
    int (*IndexBuffer_SetData)(int iboHandle,
                                  const unsigned short *indices,
                                  int start, int count, int resetBufferFlag);
    unsigned short *(*IndexBuffer_Lock)(int iboHandle);
    int (*IndexBuffer_Unlock)(int iboHandle);
    int (*IndexBuffer_Delete)(int iboHandle);

    int (*Texture_CreateFromSDLSurface)(SDL_Surface *surface, int hasAlphaChannel);
    int (*Texture_CreateFromDimensions)(int width, int height, int hasAlphaChannel);
    int (*Texture_CreateFramebuffer)(int width, int height, int hasAlphaChannel);

    int (*Texture_BlitSurface)(int textureID, SDL_Surface *surface, const PLRect *rect);

    int (*Texture_RenderGetTextureInfo)(int textureRefID, PLRect *rect, float *xMult, float *yMult);

    int (*Texture_SetWrap)(int textureRefID, int wrapState);

    int (*Texture_HasAlphaChannel)(int textureRefID);

    int (*Texture_BindFramebuffer)(int textureRefID, int renderbufferID);

    int (*Texture_AddRef)(int textureID);
    int (*Texture_Release)(int textureID);
    
    int (*Renderbuffer_Create)(int width, int height);
    int (*Renderbuffer_Release)(int renderbufferID);
    
    int (*DrawVertexArray)(const VertexDefinition *def,
               const char *vertexData,
               int primitiveType, int vertexStart, int vertexCount);
    int (*DrawVertexIndexArray)(const VertexDefinition *def,
               const char *vertexData, int vertexStart, int vertexCount,
               const unsigned short *indexData,
               int primitiveType, int indexStart, int indexCount);

    int (*DrawVertexBuffer)(const VertexDefinition *def,
               int vertexBufferHandle,
               int primitiveType, int vertexStart, int vertexCount);
    int (*DrawVertexIndexBuffer)(const VertexDefinition *def,
               int vertexBufferHandle, int vertexStart, int vertexCount,
               int indexBufferHandle,
               int primitiveType, int indexStart, int indexCount);

    int (*SetViewport)(int x, int y, int w, int h);
    int (*SetZRange)(float nearZ, float farZ);
    int (*SetUntransformedFlag)(int untransformedFlag);

    int (*ClearColor)(float r, float g, float b, float a);
    int (*Clear)();
    
    int (*Finish)();

    int (*StartFrame)();
    int (*EndFrame)();

    int (*Init)();
    int (*End)();
} PLIGraphics;

extern PLIGraphics PLG;

#ifdef DXPORTLIB_DRAW_OPENGL
typedef void *(*PLGLGetGLFunction)(const char *name);
typedef int (*PLGLIsGLExtSupported)(const char *name);

extern int PLGL_Init(PLGLGetGLFunction GetGLFunction,
                     PLGLIsGLExtSupported IsGLExtSupported,
                     int majorVersion, int minorVersion
                    );
#endif

#ifdef DXPORTLIB_DRAW_DIRECT3D9
extern int PLD3D9_Init();
#endif

/* ----------------------------------------------------------- Surface.c */
extern int PL_Surface_Create(int width, int height);

extern int PL_Surface_ApplyTransparentColor(int surfaceID,
                                            unsigned int color);
extern int PL_Surface_ApplyPMAToSDLSurface(SDL_Surface *sdlSurface);
extern int PL_Surface_ApplyPMAToSurface(int surfaceID);
extern int PL_Surface_FlipSurface(int surfaceID);

extern int PL_Surface_GetSize(int surfaceID, int *w, int *h);
extern int PL_Surface_HasTransparency(int surfaceID);

extern int PL_Surface_Load(const char *filename);
extern int PL_Surface_Delete(int surfaceID);

extern int PL_Surface_ToTexture(int surfaceID);

extern int PL_Surface_DrawToTexture(int surfaceID, int textureID,
                                    const PLRect *rect);

extern int PL_Surface_FillWithColor(int surfaceID, unsigned int color);

extern int PL_Surface_GetCount();
extern int PL_Surface_InitSurface();
extern void PL_Surface_End();

/* -------------------------------------------------------- SaveScreen.c */
extern int PL_SaveDrawScreenToBMP(int x1, int y1, int x2, int y2,
                                  const char *filename);
extern int PL_SaveDrawScreenToJPEG(int x1, int y1, int x2, int y2,
                                   const char *filename,
                                   int quality, int sample2x1);
extern int PL_SaveDrawScreenToPNG(int x1, int y1, int x2, int y2,
                                  const char *filename,
                                  int compressionLevel);


/* --------------------------------------------------------------- Input.c */
#ifndef DXPORTLIB_NO_INPUT

extern void PL_Input_ResetKeyBuffer();
extern int PL_Input_GetFromKeyBuffer();

extern int PL_Input_CheckHitKey(int dxKey);
extern int PL_Input_CheckHitKeyAll(int checkType);
extern int PL_Input_GetHitKeyStateAll(char *keyStateBuf);
extern int PL_Input_GetJoypadNum();
extern int PL_Input_GetJoypadState(int controllerIndex);
extern int PL_Input_SetJoypadInputToKeyInput(int controllerIndex, int input,
                                             int key1, int key2, int key3, int key4);
extern int PL_Input_GetJoypadAnalogInput(int *x, int *y, int controllerIndex);
extern int PL_Input_GetJoypadAnalogInputRight(int *x, int *y, int controllerIndex);
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

#endif /* #ifndef DXPORTLIB_NO_INPUT */

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
#ifndef DXPORTLIB_NO_SOUND

extern int PL_LoadSoundMem(const char *filename);
extern int PL_LoadSoundMem2(const char *filenameA, const char *filenameB);
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
                                   uint64_t loopTarget,
                                   uint64_t loopPoint);
extern int PL_Audio_SetLoopTimes(int soundID,
                                 double loopTarget,
                                 double loopPoint);
extern int PL_Audio_RemoveLoopPoints(int soundID);

extern int PL_Audio_ResetSettings();
extern int PL_Audio_Init();
extern int PL_Audio_End();

#endif /* #ifndef DXPORTLIB_NO_SOUND */

#ifdef __cplusplus
}
#endif

#endif
