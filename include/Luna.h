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

#ifndef LUNA_H_HEADER
#define LUNA_H_HEADER

#include "DxBuildConfig.h"

#ifdef DXPORTLIB_LUNA_INTERFACE

#include "DxDefines.h"

#define DXPORTLIB_LUNA

/* Luna programs expect some standard includes. */
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

/* Adds defines for uint32_t and such. */
#include <stdint.h>

/* Luna does not use namespaces, so we just link directly. */

/* Specify DLL export for all functions. */
#if defined _WIN32
#  define LUNACALL __declspec(dllexport)
#else
#  if __GNUC__ >= 4
#    define LUNACALL __attribute__(( visibility("default") ))
#  elif __GNUC__ >= 2
#    define LUNACALL __declspec(dllexport)
#  else
#    define LUNACALL
#  endif
#endif

/* ---------------------------------------- All base types */
typedef int8_t Sint8;
typedef uint8_t Uint8;
typedef int16_t Sint16;
typedef uint16_t Uint16;
typedef int32_t Sint32;
typedef uint32_t Uint32;
typedef int64_t Sint64;
typedef uint64_t Uint64;
typedef float Float;
typedef float Float32;
typedef double Float64;
typedef bool Bool;

// Math conversions taken from Luna's header.
#define toF(v)              ((Float)(v))
#define toI(v)              ((Sint32)(v))
#define F(V)                toF(V)
#define L(V)                toI(V)

#define PI                  (3.141592653589793238462643383279f)
#define PI2                 (6.283185307179586476925286766559f)
#define REV(v)              toF(1.0f/toF(v))

#define SIN_TABLE_BIT       (16)
#define SIN_TABLE_SIZE      (1 << SIN_TABLE_BIT)
#define SIN_TABLE_MASK      (SIN_TABLE_SIZE - 1)

#define ARCTAN_POS_SHIFT    (SIN_TABLE_BIT - 3)
#define ARCTAN_TABLE_SIZE   (1 << ARCTAN_POS_SHIFT)

#define ARCTAN_POS_1        (ARCTAN_TABLE_SIZE << 1)
#define ARCTAN_POS_2        (ARCTAN_POS_1 + ARCTAN_POS_1)
#define ARCTAN_POS_3        (ARCTAN_POS_1 + ARCTAN_POS_2)
#define ARCTAN_POS_4        (ARCTAN_POS_1 + ARCTAN_POS_3)
#define ARCTAN_SCALE(v)     (toF(v) * toF(1 << ARCTAN_POS_SHIFT))

#define GET_ANGLE_PI(a)     toI(toF(a) * toF(SIN_TABLE_SIZE) / toF(PI2))

#define MIN(v1, v2)         ((v1 < v2) ? (v1) : (v2))
#define MAX(v1, v2)         ((v1 > v2) ? (v1) : (v2))
// I don't agree with this code, but it's what Luna does...
#define SWAP(v1, v2)        { v1 = v2 - v1; v2 -= v1; v1 += v2; }

#define PRIMITIVE_Z_MAX         (65535)
#define PRIMITIVE_Z_CENTER      (PRIMITIVE_Z_MAX / 2)
#define INV_PRIMITIVE_Z_MAX     REV(PRIMITIVE_Z_MAX)

/* ---------------------------------------- Flags */
static const Sint32 OPTION_DIRECTINPUT = (1 << 0);
static const Sint32 OPTION_DIRECTSOUND = (1 << 1);

/* ---------------------------------------- DirectX types */
/* Only include if d3d stuff is not already included. */
#ifndef DIRECT3D_VERSION

#define DIRECT3D_VERSION 0x9

#define D3DCLEAR_TARGET     0x01
#define D3DCLEAR_ZBUFFER    0x02
#define D3DCLEAR_STENCIL    0x04

typedef enum _D3DRENDERSTATETYPE {
    D3DRS_ZENABLE = 7,
    D3DRS_FILLMODE = 8,
    D3DRS_ZWRITEENABLE = 14,
    D3DRS_CULLMODE = 22
} D3DRENDERSTATETYPE;

typedef enum _D3DCULL {
    D3DCULL_NONE = 1,
    D3DCULL_CW = 2,
    D3DCULL_CCW = 3
} D3DCULL;

typedef Uint32 D3DCOLOR;

#define D3DCOLOR_ARGB(a, r, g, b) \
    ((D3DCOLOR)( \
        (((a) & 0xff) << 24) | \
        (((r) & 0xff) << 16) | \
        (((g) & 0xff) << 8) | \
        ((b) & 0xff) \
    ))

#define D3DCOLOR_RGBA(r, g, b, a) D3DCOLOR_ARGB(a, r, g, b)
#define D3DCOLOR_XRGB(r, g, b) D3DCOLOR_ARGB(0xff, r, g, b)

typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;
typedef struct _D3DMATRIX {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DMATRIX;

#define DIK_ESCAPE 0x01
#define DIK_1 0x02
#define DIK_2 0x03
#define DIK_3 0x04
#define DIK_4 0x05
#define DIK_5 0x06
#define DIK_6 0x07
#define DIK_7 0x08
#define DIK_8 0x09
#define DIK_9 0x0a
#define DIK_0 0x0b
#define DIK_MINUS 0x0c
#define DIK_EQUALS 0x0d
#define DIK_BACK 0x0e
#define DIK_TAB 0x0f
#define DIK_Q 0x10
#define DIK_W 0x11
#define DIK_E 0x12
#define DIK_R 0x13
#define DIK_T 0x14
#define DIK_Y 0x15
#define DIK_U 0x16
#define DIK_I 0x17
#define DIK_O 0x18
#define DIK_P 0x19
#define DIK_LBRACKET 0x1a
#define DIK_RBRACKET 0x1b
#define DIK_RETURN 0x1c
#define DIK_LCONTROL 0x1d
#define DIK_A 0x1e
#define DIK_S 0x1f
#define DIK_D 0x20
#define DIK_F 0x21
#define DIK_G 0x22
#define DIK_H 0x23
#define DIK_J 0x24
#define DIK_K 0x25
#define DIK_L 0x26
#define DIK_SEMICOLON 0x27
#define DIK_APOSTROPHE 0x28
#define DIK_GRAVE 0x29
#define DIK_LSHIFT 0x2a
#define DIK_BACKSLASH 0x2b
#define DIK_Z 0x2c
#define DIK_X 0x2d
#define DIK_C 0x2e
#define DIK_V 0x2f
#define DIK_B 0x30
#define DIK_N 0x31
#define DIK_M 0x32
#define DIK_COMMA 0x33
#define DIK_PERIOD 0x34
#define DIK_SLASH 0x35
#define DIK_RSHIFT 0x36
#define DIK_MULTIPLY 0x37
#define DIK_LMENU 0x38
#define DIK_SPACE 0x39
#define DIK_CAPITAL 0x3a
#define DIK_F1 0x3b
#define DIK_F2 0x3c
#define DIK_F3 0x3d
#define DIK_F4 0x3e
#define DIK_F5 0x3f
#define DIK_F6 0x40
#define DIK_F7 0x41
#define DIK_F8 0x42
#define DIK_F9 0x43
#define DIK_F10 0x44
#define DIK_NUMLOCK 0x45
#define DIK_SCROLL 0x46
#define DIK_NUMPAD7 0x47
#define DIK_NUMPAD8 0x48
#define DIK_NUMPAD9 0x49
#define DIK_SUBTRACT 0x4a
#define DIK_NUMPAD4 0x4b
#define DIK_NUMPAD5 0x4c
#define DIK_NUMPAD6 0x4d
#define DIK_ADD 0x4e
#define DIK_NUMPAD1 0x4f
#define DIK_NUMPAD2 0x50
#define DIK_NUMPAD3 0x51
#define DIK_NUMPAD0 0x52
#define DIK_DECIMAL 0x53
#define DIK_OEM_102 0x56
#define DIK_F11 0x57
#define DIK_F12 0x58
#define DIK_F13 0x64
#define DIK_F14 0x65
#define DIK_F15 0x66
#define DIK_KANA 0x70
#define DIK_ABNT_C1 0x73
#define DIK_CONVERT 0x79
#define DIK_NOCONVERT 0x7b
#define DIK_YEN 0x7d
#define DIK_ABNT_C2 0x7e
#define DIK_NUMPADEQUALS 0x8d
#define DIK_PREVTRACK 0x90
#define DIK_AT 0x91
#define DIK_COLON 0x92
#define DIK_UNDERLINE 0x93
#define DIK_KANJI 0x94
#define DIK_STOP 0x95
#define DIK_AX 0x96
#define DIK_UNLABELED 0x97
#define DIK_NEXTTRACK 0x99
#define DIK_NUMPADENTER 0x9c
#define DIK_RCONTROL 0x9d
#define DIK_MUTE 0xa0
#define DIK_CALCULATOR 0xa1
#define DIK_PLAYPAUSE 0xa2
#define DIK_MEDIASTOP 0xa4
#define DIK_VOLUMEDOWN 0xae
#define DIK_VOLUMEUP 0xb0
#define DIK_WEBHOME 0xb2
#define DIK_NUMPADCOMMA 0xb3
#define DIK_DIVIDE 0xb5
#define DIK_SYSRQ 0xb7
#define DIK_RMENU 0xb8
#define DIK_PAUSE 0xc5
#define DIK_HOME 0xc7
#define DIK_UP 0xc8
#define DIK_PRIOR 0xc9
#define DIK_LEFT 0xcb
#define DIK_RIGHT 0xcd
#define DIK_END 0xcf
#define DIK_DOWN 0xd0
#define DIK_NEXT 0xd1
#define DIK_INSERT 0xd2
#define DIK_DELETE 0xd3
#define DIK_LWIN 0xdb
#define DIK_RWIN 0xdc
#define DIK_APPS 0xdd
#define DIK_POWER 0xde
#define DIK_SLEEP 0xdf
#define DIK_WAKE 0xe3
#define DIK_WEBSEARCH 0xe5
#define DIK_WEBFAVORITES 0xe6
#define DIK_WEBREFRESH 0xe7
#define DIK_WEBSTOP 0xe8
#define DIK_WEBFORWARD 0xe9
#define DIK_WEBBACK 0xea
#define DIK_MYCOMPUTER 0xeb
#define DIK_MAIL 0xec
#define DIK_MEDIASELECT 0xed

#define DIK_BACKSPACE DIK_BACK
#define DIK_NUMPADSTAR DIK_MULTIPLY
#define DIK_LALT DIK_LMENU
#define DIK_CAPSLOCK DIF_CAPITAL
#define DIK_NUMPADMINUS DIK_SUBTRACT
#define DIK_NUMPADPLUS DIK_ADD
#define DIK_NUMPADPERIOD DIK_DECIMAL
#define DIK_NUMPADSLASH DIK_DIVIDE
#define DIK_RALT DIK_RMENU
#define DIK_UPARROW DIK_UP
#define DIK_PGUP DIK_PRIOR
#define DIK_LEFTARROW DIK_LEFT
#define DIK_RIGHTARROW DIK_RIGHT
#define DIK_DOWNARROW DIK_DOWN
#define DIK_PGDN DIK_NEXT
#define DIK_CIRCUMFLEX DIK_PREVTRACK

#endif

#ifndef _WIN32
typedef struct _FILETIME {
    Uint32 dwLowDateTime;
    Uint32 dwHighDateTime;
} FILETIME;

#define MAX_PATH 4096
#endif

/* ---------------------------------------- Custom data types */
typedef int LCAMERA;
typedef int LSURFACE;
typedef int LSPRITE;
typedef int LTEXTURE;
typedef int LSOUND;
typedef int LPRIMITIVE;
typedef int LPRIMITIVE3D;
typedef int LSPRITE;
typedef int LSPRITE3D;
typedef int LFONT;
typedef int LFONTSPRITE;
typedef int LFONTSPRITE3D;

typedef enum {
    FORMAT_FROM_FILE = 0,
    FORMAT_DEPTH16,
    FORMAT_DEPTH32,
    FORMAT_DEPTHSTENCIL,
    FORMAT_DXT1,
    FORMAT_DXT3,
    FORMAT_DXT5,
    FORMAT_BACKBUFFER,
    FORMAT_TARGET16,
    FORMAT_TARGET32,
    FORMAT_TARGETFloat,
    FORMAT_ALPHAONLY,
    FORMAT_TEXTURE32,
    FORMAT_TEXTURE16,
    FORMAT_TEXTURE_2D,
    FORMAT_TEXTURE_3D
} eSurfaceFormat;

typedef enum {
    PRIM_VERTEX_UV0 = 0,
    PRIM_VERTEX_UV1,
    PRIM_VERTEX_UV2,
    PRIM_VERTEX_UV3,
    PRIM_VERTEX_UV4
} eVertexPrimitiveType;

typedef enum {
    BLEND_NONE = 0,
    BLEND_NORMAL,
    BLEND_ADD,
    BLEND_ADD_NOALPHA,
    BLEND_SUB,
    BLEND_SUB_NOALPHA,
    BLEND_MUL,
    BLEND_REVERSE
} eBlendType;

typedef enum {
    PAD_DIR_UP = 0,
    PAD_DIR_DOWN,
    PAD_DIR_LEFT,
    PAD_DIR_RIGHT,
    
    PAD_BUTTON_01,
    PAD_BUTTON_02,
    PAD_BUTTON_03,
    PAD_BUTTON_04,
    PAD_BUTTON_05,
    PAD_BUTTON_06,
    PAD_BUTTON_07,
    PAD_BUTTON_08,
    PAD_BUTTON_09,
    PAD_BUTTON_10,
    PAD_BUTTON_11,
    PAD_BUTTON_12,
    PAD_BUTTON_13,
    PAD_BUTTON_14,
    PAD_BUTTON_15,
    PAD_BUTTON_16,
    PAD_BUTTON_17,
    PAD_BUTTON_18,
    PAD_BUTTON_19,
    PAD_BUTTON_20,
    PAD_BUTTON_21,
    PAD_BUTTON_22,
    PAD_BUTTON_23,
    PAD_BUTTON_24,
    PAD_BUTTON_25,
    PAD_BUTTON_26,
    PAD_BUTTON_27,
    PAD_BUTTON_28,
    PAD_BUTTON_29,
    PAD_BUTTON_30,
    PAD_BUTTON_31,
    PAD_BUTTON_32,
    PAD_BUTTON_33,
    PAD_BUTTON_34,
    PAD_BUTTON_35,
    PAD_BUTTON_36,
    PAD_BUTTON_37,
    PAD_BUTTON_38,
    PAD_BUTTON_39,
    PAD_BUTTON_40,
    PAD_BUTTON_41,
    PAD_BUTTON_42,
    PAD_BUTTON_43,
    PAD_BUTTON_44,
    PAD_BUTTON_45,
    PAD_BUTTON_46,
    PAD_BUTTON_47,
    PAD_BUTTON_48,
    
    PAD_SORT_MAX,
    PAD_SORT_NONE,
    
    // This is in the Luna headers, but isn't this off by one?
    PAD_BUTTON_MAX = (PAD_BUTTON_48 - PAD_BUTTON_01)
} ePadSort;

typedef enum {
    MOUSE_FREE,
    MOUSE_PULL,
    MOUSE_PUSH,
    MOUSE_HOLD,
} eMouseState;

typedef struct {
    Sint32 X, Y, W;
    Sint32 Mx, My;
    
    Bool ClickL, ClickR, ClickW;
    
    Bool DoubleClickL, DoubleClickR, DoubleClickW;
    
    eMouseState StateL, StateR, StateW;
} MOUSEDATA;

typedef struct CLunaPoint {
    Float Px, Py;
    
    void LUNACALL Set(Float px, Float py) { Px = px; Py = py; }
} LUNAPOINT;
typedef struct CLunaRect {
    Float Px, Py, Sx, Sy;
    
    void LUNACALL Set(Float px, Float py, Float sx, Float sy)
    { Px = px; Py = py; Sx = sx; Sy = sy; }
    
    void LUNACALL SetCenter(Float px, Float py, Float sx, Float sy,
                   Float scale = 1.0f) {
        Set(px - ((sx * 0.5f) * scale), py - ((sy * 0.5f) * scale),
            sx * scale, sy * scale);
    }
} LUNARECT;

class CVector2D;
class CVector3D;
class CMatrix;

typedef CVector3D LVECTOR3;

typedef struct _LUNAFILEDATA {
    // no hFile, we explicitly want to break hFile based code.
    int dxPortLibFileHandle;
    BOOL IsPack;
    Uint32 Start;
    Uint32 Size;
    FILETIME CreationTime;
    FILETIME LastAccessTime;
    FILETIME LastWriteTime;
    char FileName[MAX_PATH];
} FILEDATA;

typedef struct _LUNAJOYSTICKDATA {
    Sint32 Px, Py, Pz;
    Sint32 Rx, Ry, Rz;
    Sint32 Vx, Vy, Vz;
    Sint32 VRx, VRy, VRz;
    Sint32 Ax, Ay, Az;
    Sint32 ARx, ARy, ARz;
    Sint32 Fx, Fy, Fz;
    Sint32 FRx, FRy, FRz;
    Sint32 Slider[2];
    Sint32 Pov[4];
    char Button[PAD_BUTTON_MAX];
} JOYSTICKDATA;

#define COLOR_BASE              (D3DCOLOR_XRGB(0xff, 0xff, 0xff))
#define COLOR_WHITE             (D3DCOLOR_XRGB(0xff, 0xff, 0xff))
#define COLOR_BLACK             (D3DCOLOR_XRGB(0x00, 0x00, 0x00))
#define COLOR_RED               (D3DCOLOR_XRGB(0xff, 0x00, 0x00))
#define COLOR_GREEN             (D3DCOLOR_XRGB(0x00, 0xff, 0x00))
#define COLOR_BLUE              (D3DCOLOR_XRGB(0x00, 0x00, 0xff))
#define COLOR_ALPHA(a)          (D3DCOLOR_ARGB(a, 0xff, 0xff, 0xff))
#define COLOR_BRIGHT(b)         (D3DCOLOR_XRGB(b, b, b))

#define COLORKEY_DISABLE        (D3DCOLOR_ARGB(0x00, 0x00, 0x00, 0x00))
#define COLORKEY_WHITE          (D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff))
#define COLORKEY_BLACK          (D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0x00))
#define COLORKEY_RED            (D3DCOLOR_AAGB(0xff, 0xff, 0x00, 0x00))
#define COLORKEY_GREEN          (D3DCOLOR_ARGB(0xff, 0x00, 0xff, 0x00))
#define COLORKEY_BLUE           (D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0xff))

const LSURFACE INVALID_SURFACE = -1;
const LTEXTURE INVALID_TEXTURE = -1;
const LPRIMITIVE INVALID_PRIMITIVE = -1;
const LPRIMITIVE3D INVALID_PRIMITIVE3D = -1;
const LSPRITE INVALID_SPRITE = -1;
const LSPRITE3D INVALID_SPRITE3D = -1;
const LFONT INVALID_FONT = -1;
const LFONTSPRITE INVALID_FONTSPRITE = -1;
const LFONTSPRITE3D INVALID_FONTSPRITE3D = -1;
const LSOUND INVALID_SOUND = -1;
const LCAMERA INVALID_CAMERA = -1;

/* --------------------------------------------------------- Luna.cpp */
/* So in the original library, there is class Luna and you are supposed
 * to fill out many of the functions yourself. This is incompatible with
 * having a unified library for DxLib and Luna.
 */
typedef Bool (*LunaInitFunc)();
typedef void (*LunaMainFunc)(Sint32 argc, char *argv[]);
typedef Bool (*LunaMessageProcFunc)
    ( Sint32 Msg, Sint32 wParam, Sint32 lParam );

class Luna {
private:
public:
    /* BootMain is called at the start of the application, and should
     * not be called by userspace unless you are doing WinMain/main
     * yourself.
     */
    static LUNACALL int BootMain(Sint32 argc, char *argv[],
        LunaInitFunc LunaInit, LunaMainFunc LunaMain,
        LunaMessageProcFunc LunaMessageProc
    );
    
    static LUNACALL Bool Start();
    static LUNACALL void End();
    
    static LUNACALL void Exit();
    
    static LUNACALL Bool WaitForMsgLoop(Bool isFullActive = false);
    static LUNACALL void SyncFrame();
    
    static LUNACALL void SetApplicationName(const char *name);
    static LUNACALL void SetScreenInfo(Sint32 width, Sint32 height,
                             Bool isWindow);
    static LUNACALL void SetFrameRate(Sint32 frameRate);
    static LUNACALL void SetDrawTitleInfo(void);
    static LUNACALL void SetDrawMouse(void);
    static LUNACALL void SetUseOption(Uint32 flags);

    static LUNACALL void GetScreenData(Sint32 *width, Sint32 *height,
                             bool *isWindowed);
    
    static LUNACALL void SetVirtualScreenSize(
                             Sint32 rWidth, Sint32 rHeight,
                             Sint32 vWidth, Sint32 vHeight);
    static LUNACALL void SetVirtualScreenEnable(Bool Flag);
    static LUNACALL void GetVirtualScreenSize(
                             Sint32 *pWidth, Sint32 *pHeight);
    static LUNACALL Float Virtual2RealX(Float Pos);
    static LUNACALL Float Virtual2RealY(Float Pos);
    
    static LUNACALL void ChangeScreenSize(Sint32 width,
                                          Sint32 height, Bool IsChange);
    static LUNACALL void ChangeScreenMode();
    
    // If you want to use anything other than UTF8, use this.
    static LUNACALL void EXTSetUseCharset(int dxCharset);
    
    static LUNACALL void EXTGetSaveFolder(char *buffer, int bufferLength,
                             const char *org, const char *app,
                             int destEncoding);
    static LUNACALL int EXTConvertText(char *buffer, int bufferLength,
                            const char *string,
                            int destEncoding, int srcEncoding);
    
    static LUNACALL void EXTSetFullscreenDesktop(bool flag);

    static LUNACALL void EXTSetOnlyWindowSize(int width, int height,
                                              bool isFullscreen,
                                              bool isFullscreenDesktop);
    
    static LUNACALL void EXTSetWindowIconFromFile(const char *filename);
    static LUNACALL void EXTSetVSync(bool vsyncEnabled);
    
    // Supplied by the application.
    //static LUNACALL Bool Init();
    //static LUNACALL void Main(Sint32 argc, char *argv[]);
    
    // Not actually supported or used, so please dummy it out.
    //static LUNACALL Bool MessageProc(Sint32 Msg,
    //                         Sint32 wParam, Sint32 lParam);
};

/* --------------------------------------------------------- LunaRand.cpp */
class LunaRand {
public:
    static LUNACALL void Seed(Uint32 Param);
    static LUNACALL Uint32 GetInt32();
    static LUNACALL Uint64 GetInt64();
    static LUNACALL Float GetFloat32();
    static LUNACALL Sint32 GetInt(Sint32 Min, Sint32 Max);
    static LUNACALL Float GetFloat(Float Min, Float Max);
};

/* --------------------------------------------------------- LunaFile.cpp */
class LunaFile {
public:
    static LUNACALL void SetRootPath(Uint32 Priority,
                                     const char *pRootPath,
                                     const char *pPackFile);
    static LUNACALL FILEDATA *FileOpen(const char *pFile,
                                       Bool ReadOnly = false);
    
    static LUNACALL Uint32 FileGetSize( FILEDATA *pFile );
    static LUNACALL Uint32 FileGetPosition( FILEDATA *pFile );

    static LUNACALL Uint32 FileRead(FILEDATA *pFile,
                                    Uint32 Size, void *pData);
    static LUNACALL void FileClose(FILEDATA *pFile);
};

/* --------------------------------------------------------- LunaMath.cpp */

/* LunaMath uses a lookup table for its functions instead of calling
 * sin() and cos() directly, and as such should be fully portable. */
class LunaMath {
public:
    static LUNACALL Float Sin(Sint32 angle);
    static LUNACALL Float Cos(Sint32 angle);
    static LUNACALL void SinCos(Sint32 angle, Float *fSin, Float *fCos);
    static LUNACALL Sint32 Atan(Sint32 dx, Sint32 dy);
    static LUNACALL Sint32 Atan(Float dx, Float dy);
    static LUNACALL void RotationPoint(CLunaPoint *point, Sint32 Angle);
};

/* -------------------------------------------------------- LunaInput.cpp */
class LunaInput {
public:
    static LUNACALL void KeyBufferFlush(void);
    static LUNACALL Bool GetKeyData(Uint32 Key);
    static LUNACALL void GetMouseData(MOUSEDATA *pMouse);
    static LUNACALL Bool GetJoystickData(Sint32 No, JOYSTICKDATA *pJoy);
    static LUNACALL Bool KeyBufferLoop(Uint32 *pKey);
};

/* ------------------------------------------------------- LunaCamera.cpp */
class LunaCamera {
public:
    static LUNACALL LCAMERA Create(void);
    static LUNACALL void Release(LCAMERA lCam);
    static LUNACALL void CreateProjection(LCAMERA lCam, Float fFovY,
                                          Float fNearZ, Float fFarZ,
                                          Float fAspect);
    static LUNACALL void TransformInit(LCAMERA lCam);
    static LUNACALL void TransformUpdate(LCAMERA lCam);
    static LUNACALL void LookAt(LCAMERA lCam, CVector3D *pEye,
                                CVector3D *pAt, CVector3D *pUp);
};

/* ------------------------------------------------------ LunaTexture.cpp */
class LunaTexture {
public:
    static LUNACALL LTEXTURE CreateFromFile(const char *pFileName,
                              eSurfaceFormat format,
                              D3DCOLOR keyColor = COLORKEY_DISABLE);
    static LUNACALL LTEXTURE CreateFromLAG(const char *pFileName,
                              const char *pDataName,
                              eSurfaceFormat Format );
    static LUNACALL void Release(LTEXTURE texture);
};

/* ------------------------------------------------------- LunaSprite.cpp */
class LunaSprite {
public:
    static LUNACALL LSPRITE Create(Uint32 VertexMax,
                             eVertexPrimitiveType VertexType,
                             Bool IsSortZ);
    static LUNACALL void Release(LSPRITE lSpr);
    static LUNACALL void AttatchTexture(LSPRITE lSpr,
                          Uint32 Stage, LTEXTURE lTex);
    static LUNACALL void DrawSquare(LSPRITE lSpr,
                          CLunaRect *PDstRect, Float Pz,
                          CLunaRect *pSrcRects, D3DCOLOR Color,
                          Uint32 UvCount = 1);
    static LUNACALL void EXTDrawSquare(LSPRITE lSpr,
                          CLunaRect *PDstRect, Float Pz,
                          CLunaRect *pSrcRects, D3DCOLOR Color,
                          Uint32 UvCount = 1, Bool Mirror = false);
    static LUNACALL void DrawSquareRotate(LSPRITE lSpr, CLunaRect *PDstRect,
                          Float Pz, CLunaRect *pSrcRects, D3DCOLOR Color,
                          Uint32 Angle, Uint32 UvCount = 1,
                          Float fCx = 0.0f, Float fCy = 0.0f);
    static LUNACALL void EXTDrawSquareRotate(LSPRITE lSpr, CLunaRect *PDstRect,
                          Float Pz, CLunaRect *pSrcRects, D3DCOLOR Color,
                          Uint32 Angle, Uint32 UvCount = 1,
                          Float fCx = 0.0f, Float fCy = 0.0f,
                          Bool Mirror = false);
    static LUNACALL void DrawSquareRotateXYZ(LSPRITE lSpr,
                          CLunaRect *PDstRect, Float Pz,
                          CLunaRect *pSrcRects, D3DCOLOR Color,
                          Uint32 AngleX, Uint32 AngleY, Uint32 AngleZ,
                          Uint32 UvCount = 1);
    static LUNACALL void ResetBuffer(LSPRITE lSpr);
    static LUNACALL void UpdateBuffer(LSPRITE lSpr);
    static LUNACALL void Rendering(LSPRITE lSpr);
};

/* --------------------------------------------------- LunaFontSprite.cpp */
class LunaFontSprite {
public:
    static LUNACALL LFONTSPRITE CreateFromFile(const char *pFileName,
                                 const char *pExt, Bool IsAlpha, Uint32 Num,
                                 Bool IsSortZ = false,
                                 eSurfaceFormat Format = FORMAT_TEXTURE_2D);
    static LUNACALL void Release(LFONTSPRITE lFontSpr);
    static LUNACALL void DrawString(LFONTSPRITE lFontSpr, const char *pStr,
                          Sint32 Px, Sint32 Py, Float Pz, D3DCOLOR Color);
    static LUNACALL void DrawStringP(LFONTSPRITE lFontSpr, const char *pStr,
                          Sint32 Px, Sint32 Py, Float Pz, D3DCOLOR Color);
    static LUNACALL void ResetBuffer(LFONTSPRITE lFontSpr, Sint32 Space = 0);
    static LUNACALL void UpdateBuffer(LFONTSPRITE lFontSpr);
    static LUNACALL void Rendering(LFONTSPRITE lFontSpr);
    static LUNACALL Bool GetWidth(LFONTSPRITE lFontSpr, const char *pStr,
                          Sint32 *pLeft, Sint32 *pCenter, Sint32 *pRight);
    static LUNACALL Sint32 DrawChara(LFONTSPRITE lFontSpr, const char *pStr,
                          CLunaRect *pDst, Float Pz, D3DCOLOR Color);
    static LUNACALL Sint32 DrawCharaRotate(LFONTSPRITE lFontSpr, const char *pStr,
                          CLunaRect *pDst, Float Pz, D3DCOLOR Color,
                          Sint32 Angle);
    static LUNACALL Sint32 DrawCharaRotateXYZ(LFONTSPRITE lFontSpr, const char *pStr,
                          CLunaRect *pDst, Float Pz, D3DCOLOR Color,
                          Sint32 AngleX, Sint32 AngleY, Sint32 AngleZ);
    static LUNACALL Sint32 EXTGetCharaSize(LFONTSPRITE lFontSpr, const char *pStr,
                          Sint32 *w, Sint32 *h);
};

/* ----------------------------------------------------------- Luna3D.cpp */
class Luna3D {
public:
    static LUNACALL Bool BeginScene(void);
    static LUNACALL void EndScene(void);
    static LUNACALL void Refresh(void);
    static LUNACALL void SetViewport(const RECT *rect);
    static LUNACALL void GetViewport(RECT *rect);
    static LUNACALL void SetFilterEnable(Bool Flag);
    static LUNACALL bool GetFilterEnable();
    static LUNACALL void SetColorkeyEnable(Bool Flag);
    static LUNACALL void SetBlendingType(eBlendType BlendType);
    static LUNACALL void SetZBufferEnable(Bool Flag);
    static LUNACALL void SetRenderState(D3DRENDERSTATETYPE State, Uint32 Param);
    static LUNACALL void Clear(Uint32 ClearFlags = D3DCLEAR_TARGET|
                                                   D3DCLEAR_ZBUFFER|
                                                   D3DCLEAR_STENCIL,
                          D3DCOLOR Color = 0x00000000, Float Depth = 1.0f,
                          Uint32 Stencil = 0, RECT *pDst = NULL);
    static LUNACALL void SetCamera(LCAMERA lCamera);
};

/* -------------------------------------------------------- LunaSound.cpp */
class LunaSound {
public:
    static LUNACALL LSOUND CreateFromFile(const char *filename,
                            bool IsNoStop, bool IsAyame = true,
                            bool IsHardware = false, Uint32 Layer = 1);
    static LUNACALL Bool IsPlay(LSOUND lSnd, Uint32 Layer = 0);
    static LUNACALL void Play(LSOUND lSnd, Uint32 Layer = 0);
    static LUNACALL void Loop(LSOUND lSnd, Uint32 LoopCount = 0xffffffff,
                              Uint32 LoopInSample = 0, Uint32 Layer = 0);
    static LUNACALL void Stop(LSOUND lSnd, Uint32 Layer = 0);
    static LUNACALL void Pause(LSOUND lSnd, Uint32 Layer = 0);
    static LUNACALL void Release(LSOUND lSnd);
    static LUNACALL void SetVolume(LSOUND lSnd, Float fParam,
                                   Uint32 Layer = 0);
    static LUNACALL void SetPan(LSOUND lSnd, Float fParam,
                                Uint32 Layer = 0);
    static LUNACALL void SetMax(Uint32 Max);
    static LUNACALL void SetAyamePath(const char *pPath);
    
    static LUNACALL void EXTSetLoopSamples(LSOUND lSnd,
                             Uint64 loopTarget, Uint64 loopPoint);
    static LUNACALL void EXTSetLoopTimes(LSOUND lSnd,
                             double loopTarget, double loopPoint);
};

/* ---------------------------------------------------------- Math types */

class CMatrix : D3DMATRIX {
public:
    CMatrix() { }
    CMatrix(const CMatrix &src) { memcpy(m, src.m, sizeof(m)); }
    CMatrix(
        float m11, float m12, float m13, float m14,
        float m21, float m22, float m23, float m24,
        float m31, float m32, float m33, float m34,
        float m41, float m42, float m43, float m44) {
        m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; m[0][3] = m14;
        m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; m[1][3] = m24;
        m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; m[2][3] = m34;
        m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; m[3][3] = m44;
    }
    
    void Identity() {
        m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
        m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
    }
    void Transpose(const CMatrix *src) {
        for (int x = 0; x < 4; ++x) {
            m[x][x] = src->m[x][x];
            for (int y = x + 1; y < 4; ++y) {
                float a = src->m[y][x];
                float b = src->m[x][y];
                m[x][y] = a; m[y][x] = b;
            }
        }
    }
    
    void LUNACALL Inverse(const CMatrix *src);
    void LUNACALL Normalize(const CMatrix *src);
    void LUNACALL Multiply(const CMatrix *a, const CMatrix *b);
    
    void Transpose()
    { Transpose(this); }
    void Inverse()
    { Inverse(this); }
    void Normalize()
    { Normalize(this); }
    void Multiply(const CMatrix *src)
    { Multiply(this, src); }
    
    void LUNACALL RotationX(Sint32 angle);
    void LUNACALL RotationY(Sint32 angle);
    void LUNACALL RotationZ(Sint32 angle);
    void LUNACALL RotationAxis(Sint32 angle, const CVector3D *axis);
    void LUNACALL Rotation(Sint32 rotX, Sint32 rotY, Sint32 rotZ);
    void LUNACALL RotationZXY(Sint32 angleX, Sint32 angleY, Sint32 angleZ)
    { Rotation(angleX, angleY, angleZ); }
    
    void Scaling(float sx, float sy, float sz) {
        m[0][0] = sx; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
        m[1][0] = 0; m[1][1] = sy; m[1][2] = 0; m[1][3] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = sz; m[2][3] = 0;
        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
    }
    void Translation(float px, float py, float pz) {
        m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
        m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
        m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
        m[3][0] = px; m[3][1] = py; m[3][2] = pz; m[3][3] = 1;
    }
    
    void LUNACALL RotateVector(const CVector3D *a, const CVector3D *b, const CVector3D *axis = NULL);
    
    void LUNACALL Perspective(float fov, float zNear, float zFar, float aspect);
    void LUNACALL Ortho(float w, float h, float zNear, float zFar);
    
    void LUNACALL LookAt(const CVector3D *eye, const CVector3D *at, const CVector3D *up);
    
    void LUNACALL Move(const CVector2D *v);
    void LUNACALL Move(const CVector3D *v);
    void LUNACALL Resolution(CVector3D &pos, CVector3D &scale, CMatrix &rot);
    
    CMatrix &operator=(const CMatrix &src)
    { memcpy(m, src.m, sizeof(m)); return *this; }
    CMatrix operator*(const CMatrix &src) const
    { CMatrix t(*this); t.Multiply(&src); return CMatrix(t); }
    CMatrix &operator*=(const CMatrix &src)
    { Multiply(&src); return *this; }
    
    float &operator()(Uint32 row, Uint32 col) { return m[row][col]; }
    float operator()(Uint32 row, Uint32 col) const { return m[row][col]; }
};

class CVector2D {
public:
    float x, y;
    
    CVector2D()
    { }
    CVector2D(const CVector2D &v)
    { x = v.x; y = v.y; }
    CVector2D(float pX, float pY)
    { x = pX; y = pY; }
    
    /* Unlike Luna, we implement inline when possible. */
    void LUNACALL Set(float pX, float pY)
    { x = pX; y = pY; }
    void LUNACALL Add(const CVector2D *a, const CVector2D *b)
    { Set(a->x + b->x, a->y + b->y); }
    void LUNACALL Sub(const CVector2D *a, const CVector2D *b)
    { Set(a->x - b->x, a->y - b->y); }
    
    float DotProduct(const CVector2D *v) const
    { return (x * v->x) + (y * v->y); }
    float CrossProduct(const CVector2D *a, const CVector2D *b) const
    { return (a->x * b->y) - (a->y * b->x); }
    float CrossProduct(const CVector2D *v) const
    { return CrossProduct(this, v); }
    float LengthSq() const
    { return DotProduct(this); }
    float Length() const
    { return sqrtf(LengthSq()); }
    
    /* This will be very lossy, but it's how Luna does it. */
    void Normalize(CVector2D *v)
    { float invl = 1.0f / v->Length(); Set(v->x * invl, v->y * invl); }
    void Normalize()
    { Normalize(this); }
    
    void Reflect(const CVector2D *a, const CVector2D *b) {
        float d = a->DotProduct(b);
        Set(a->x - (2 * d * b->x), a->y - (2 * d * b->y));
    }
    void Reflect(const CVector2D *v)
    { return Reflect(this, v); }
    void Scaling(float sx, float sy)
    { Set(x * sx, y * sy); }
    void Translation(float px, float py)
    { Set(x + px, y + py); }
    
    CVector2D& operator+=(const CVector2D &v)
    { Set(x + v.x, y + v.y); return *this; }
    CVector2D& operator-=(const CVector2D &v)
    { Set(x - v.x, y - v.y); return *this; }
    CVector2D& operator*=(float f)
    { Set(x * f, y * f); return *this; }
    CVector2D& operator/=(float f)
    { float invf = 1.0f / f; Set(x * invf, y * invf); return *this; }
    
    CVector2D operator+() const
    { return *this; }
    CVector2D operator-() const
    { return CVector2D(-x, -y); }

    CVector2D operator+(const CVector2D &v) const
    { return CVector2D(x + v.x, y + v.y); }
    CVector2D operator-(const CVector2D &v) const
    { return CVector2D(x - v.x, y - v.y); }
    CVector2D operator*(float f) const
    { return CVector2D(x * f, y * f); }
    CVector2D operator/(float f) const
    { float invf = 1.0f / f; return CVector2D(x * invf, y * invf); }
};

class CVector3D : public D3DVECTOR {
public:
    CVector3D()
    { }
    CVector3D(const CVector3D &v)
    { x = v.x; y = v.y; z = v.z; }
    CVector3D(float pX, float pY, float pZ)
    { x = pX; y = pY; z = pZ; }
    
    void LUNACALL Set(float pX, float pY, float pZ)
    { x = pX; y = pY; z = pZ; }
    void LUNACALL Add(const CVector3D *a, const CVector3D *b)
    { Set(a->x + b->x, a->y + b->y, a->z + b->z); }
    void LUNACALL Sub(const CVector3D *a, const CVector3D *b)
    { Set(a->x - b->x, a->y - b->y, a->z - b->z); }
    
    float DotProduct(const CVector3D *v) const
    { return (x * v->x) + (y * v->y) + (z * v->z); }
    void CrossProduct(const CVector3D *a, const CVector3D *b) {
        Set((a->y * b->z) - (a->z * b->y),
            (a->z * b->x) - (a->x * b->z),
            (a->x * b->y) - (a->y * b->x));
    }
    void CrossProduct(const CVector3D *v)
    { CrossProduct(this, v); }
    float LengthSq() const
    { return DotProduct(this); }
    float Length() const
    { return sqrtf(LengthSq()); }
    
    void Normalize(CVector3D *v)
    { float invl = 1.0f / v->Length(); Set(v->x * invl, v->y * invl, v->z * invl); }
    void Normalize()
    { Normalize(this); }
 
    void Scaling(float sx, float sy, float sz)
    { Set(x * sx, y * sy, z * sz); }
    void Translation(float px, float py, float pz)
    { Set(x + px, y + py, y + pz); }
    
    void Reflect(const CVector3D *a, const CVector3D *b) {
        float d = a->DotProduct(b);
        Set(a->x - (2 * d * b->x), a->y - (2 * d * b->y), a->z - (2 * d * b->z));
    }
    void Reflect(const CVector3D *v)
    { Reflect(this, v); }
    
    /* Implemented in LunaVecMath */
    void LUNACALL TransformNormal(const CVector3D *v, const CMatrix *m);
    void LUNACALL TransformCoord(const CVector3D *v, const CMatrix *m);
    void LUNACALL TransformProjection(const CVector3D *v, const CMatrix *m);
    
    void TransformNormal(const CMatrix *m)
    { TransformNormal(this, m); }
    void TransformCoord(const CMatrix *m)
    { TransformCoord(this, m); }
    void TransformProjection(const CMatrix *m)
    { TransformProjection(this, m); }
    
    void RotationX(Sint32 angle) {
        float fSin = LunaMath::Sin(angle);
        float fCos = LunaMath::Cos(angle);
        Set(x, (y * fCos) - (z * fSin), (z * fCos) + (y * fSin));
    }
    void RotationY(Sint32 angle) {
        float fSin = LunaMath::Sin(angle);
        float fCos = LunaMath::Cos(angle);
        Set((x * fCos) + (z * fSin), y, (z * fCos) - (x * fSin));
    }
    void RotationZ(Sint32 angle) {
        float fSin = LunaMath::Sin(angle);
        float fCos = LunaMath::Cos(angle);
        Set((x * fCos) - (y * fSin), (y * fCos) + (x * fSin), z);
    }
    void RotationAxis(Sint32 angle, const CVector3D *axis) {
        CMatrix m;
        m.RotationAxis(angle, axis);
        TransformNormal(&m);
    }
    void Rotation(Sint32 x, Sint32 y, Sint32 z) {
        CMatrix m;
        m.Rotation(x, y, z);
        TransformNormal(&m);
    }
    
    CVector3D& operator+=(const CVector3D &v)
    { Set(x + v.x, y + v.y, z + v.z); return *this; }
    CVector3D& operator-=(const CVector3D &v)
    { Set(x - v.x, y - v.y, z - v.z); return *this; }
    CVector3D& operator*=(float f)
    { Set(x * f, y * f, z * f); return *this; }
    CVector3D& operator/=(float f)
    { float invf = 1.0f / f; Set(x * invf, y * invf, z * invf); return *this; }
    
    CVector3D operator+() const
    { return *this; }
    CVector3D operator-() const
    { return CVector3D(-x, -y, -z); }

    CVector3D operator+(const CVector3D &v) const
    { return CVector3D(x + v.x, y + v.y, z + v.z); }
    CVector3D operator-(const CVector3D &v) const
    { return CVector3D(x - v.x, y - v.y, z - v.z); }
    CVector3D operator*(float f) const
    { return CVector3D(x * f, y * f, z * f); }
    CVector3D operator/(float f) const
    { float invf = 1.0f / f; return CVector3D(x * invf, y * invf, z * invf); }
};

DXINLINE void CMatrix::Move(const CVector2D *v)
{ m[3][0] += v->x; m[3][1] += v->y; }
DXINLINE void CMatrix::Move(const CVector3D *v)
{ m[3][0] += v->x; m[3][1] += v->y; m[3][2] += v->z; }

/* Part of LunaCamera */
class CStyle {
protected:
    CMatrix m_mStyle;
    CMatrix m_mStyleInverse;
    CMatrix m_mTransform;
    CMatrix m_mTransformInverse;
    CVector3D m_vUp;
    CVector3D m_vRight;
    CVector3D m_vFront;
    CVector3D m_vPosition;
    CVector3D m_vScale;
public:
    LUNACALL CStyle();
    LUNACALL CStyle(const CStyle &style);
    
    void LUNACALL Set(const CStyle *srcStyle);
    void LUNACALL Get(CStyle *srcStyle) const;
    
    void LUNACALL TransformInit();
    void LUNACALL TransformUpdate();
    
    // void LUNACALL RotationX(Uint32 rot);
    // void LUNACALL RotationY(Uint32 rot);
    // void LUNACALL RotationZ(Uint32 rot);
    // void LUNACALL Pitching(Uint32 rot);
    // void LUNACALL Heading(Uint32 rot);
    // void LUNACALL Rolling(Uint32 rot);
    // void LUNACALL RotationQuaternion(const CQuaternion *quat);
    void LUNACALL Translation(float px, float py, float pz);
    void LUNACALL Scaling(float sx, float sy, float sz);
    void LUNACALL LookAt(const CVector3D *eye, const CVector3D *at, const CVector3D *up);
    void LUNACALL FromMatrix(const CMatrix *m);
    Sint32 LUNACALL GetAngleX() const;
    Sint32 LUNACALL GetAngleY() const;
    Sint32 LUNACALL GetAngleZ() const;
    
    CStyle &operator=(const CStyle &src)            { Set(&src); return *this; }
    
    void SetPosition(const CVector3D *pPos)         { m_vPosition = *pPos; }
    void SetFront(const CVector3D *pVec)            { m_vFront = *pVec; }
    void SetRight(const CVector3D *pVec)            { m_vRight = *pVec; }
    void SetUp(const CVector3D *pVec)               { m_vUp = *pVec; }
    void GetPosition(CVector3D *pPos) const         { *pPos = m_vPosition; }
    void GetFront(CVector3D *pPos) const            { *pPos = m_vFront; }
    void GetRight(CVector3D *pPos) const            { *pPos = m_vRight; }
    void GetUp(CVector3D *pPos) const               { *pPos = m_vUp; }
    void GetStyle(CMatrix *pMat) const              { *pMat = m_mStyle; }
    void GetStyleInverse(CMatrix *pMat) const       { *pMat = m_mStyleInverse; }
    void GetTransform(CMatrix *pMat) const          { *pMat = m_mTransform; }
    void GetTransformInverse(CMatrix *pMat) const   { *pMat = m_mTransformInverse; }
};

/* ---------------------------------------- Cleanup */
#undef LUNACALL

#endif /* #ifdef DXPORTLIB_LUNA_INTERFACE */

#endif /* #ifndef LUNA_H_HEADER */
