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

#ifndef DXLIB_DEFINES_H_HEADER
#define DXLIB_DEFINES_H_HEADER

#include "DxBuildConfig.h"

#ifdef UNICODE
#include <wchar.h>
#endif

#if defined _WIN32
#  include <windows.h>
#  include <tchar.h>
#else
/* On non-Windows, replicate some of the common types used. */
typedef struct _RECT {
    int left;
    int top;
    int right;
    int bottom;
} RECT;

typedef int BOOL;
typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;

#  ifdef UNICODE
#    define _TEXT(s) L ## s
typedef wchar_t TCHAR;
#  else
#    define _TEXT(s) s
typedef char TCHAR;
#  endif
#  define _T(s) _TEXT(s)
#endif

#ifdef __cplusplus
namespace DxLib {
#endif

#define DXPORTLIB
#define DXPORTLIB_VERSION "0.1.0"

/* This library is compatible with DxLib v3.11. */
#define DXLIB_VERSION 0x3110
#define DXLIB_VERSION_STR "3.11 "

/* Various type macros and build defines */
#ifdef UNICODE
#define DXCHAR wchar_t
#else
#define DXCHAR char
#endif
#define DXCOLOR int
#define DXTRUE (1)
#define DXFALSE (0)

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

/* Specify DLL export for all functions. */
#if defined _WIN32
#  define DXCALL __declspec(dllexport)
#else
#  if __GNUC__ >= 4
#    define DXCALL __attribute__(( visibility("default") ))
#  elif __GNUC__ >= 2
#    define DXCALL __declspec(dllexport)
#  else
#    define DXCALL
#  endif
#endif

/* DxPortLib only supports Shift-JIS and UTF8.
 * Please use UTF8 when available. Please.
 */
#define DX_CHARSET_DEFAULT      (0)

#ifndef DXPORTLIB_NON_SJIS
#  define DX_CHARSET_SHFTJIS    (1)
#endif /* #ifndef DXPORTLIB_NON_SJIS */

#define DX_CHARSET_EXT_UTF8     (0xff)

/* ---------------------------------------------------- GRAPHICS DEFINES */
/* These are chosen to match DxLib itself when possible.
 */

/* DxPortLib treats all of these screens as identical, so there is no
 * difference. */
#define DX_SCREEN_FRONT         (0xfffffffc)
#define DX_SCREEN_BACK          (0xfffffffe)
#define DX_SCREEN_WORK          (0xfffffffd)
#define DX_SCREEN_TEMPFRONT     (0xfffffffb)

#define DX_NONE_GRAPH           (0xfffffffb)

/* Placeholders for now. */
#define DX_IMAGESAVETYPE_BMP            (0)
#define DX_IMAGESAVETYPE_JPEG           (1)
#define DX_IMAGESAVETYPE_PNG            (2)

/* Only nearest/bilinear are supported at current time. */
#define DX_DRAWMODE_NEAREST             (0)
#define DX_DRAWMODE_BILINEAR            (1)

/* Only these blend modes are supported at the moment. */
#define DX_BLENDMODE_NOBLEND            (0)
#define DX_BLENDMODE_ALPHA              (1)
#define DX_BLENDMODE_ADD                (2)
#define DX_BLENDMODE_SUB                (3)
#define DX_BLENDMODE_MUL                (4)
#define DX_BLENDMODE_SUB2               (5)
/* XOR is not supported. */
/* 7 is reserved. */
#define DX_BLENDMODE_DESTCOLOR          (8)
#define DX_BLENDMODE_INVDESTCOLOR       (9)
#define DX_BLENDMODE_INVSRC             (10)
#define DX_BLENDMODE_MULA               (11)
#define DX_BLENDMODE_ALPHA_X4           (12)
#define DX_BLENDMODE_ADD_X4             (13)
#define DX_BLENDMODE_SRCCOLOR           (14)
#define DX_BLENDMODE_HALF_ADD           (15)
#define DX_BLENDMODE_SUB1               (16)
#define DX_BLENDMODE_PMA_ALPHA          (17)
#define DX_BLENDMODE_PMA_ADD            (18)
#define DX_BLENDMODE_PMA_SUB            (19)
#define DX_BLENDMODE_PMA_INVSRC         (20)
#define DX_BLENDMODE_PMA_ALPHA_X4       (21)
#define DX_BLENDMODE_PMA_ADD_X4         (22)
#define DX_BLENDMODE_NUM                (23)

/* Font types. Internally, we only do antialiasing though. */
#define DX_FONTTYPE_NORMAL                      (0x00)
#define DX_FONTTYPE_EDGE                        (0x01)
#define DX_FONTTYPE_ANTIALIASING                (0x02)
#define DX_FONTTYPE_ANTIALIASING_4X4            (0x12)
#define DX_FONTTYPE_ANTIALIASING_8X8            (0x22)
#define DX_FONTTYPE_ANTIALIASING_EDGE           (0x03)
#define DX_FONTTYPE_ANTIALIASING_EDGE_4X4       (0x13)
#define DX_FONTTYPE_ANTIALIASING_EDGE_8X8       (0x23)

/* ----------------------------------------------------- INPUT DEFINES */
typedef struct DINPUT_JOYSTATE {
    int X;
    int Y;
    int Z;
    int Rx;
    int Ry;
    int Rz;
    int Slider[2];
    unsigned int POV[4];
    unsigned char Buttons[32];
} DINPUT_JOYSTATE;

typedef struct XINPUT_STATE {
    unsigned char Buttons[16];
    unsigned char LeftTrigger;
    unsigned char RightTrigger;
    short ThumbLX;
    short ThumbLY;
    short ThumbRX;
    short ThumbRY;
} XINPUT_STATE;

#define DX_CHECKINPUT_KEY       (0x01)
#define DX_CHECKINPUT_PAD       (0x02)
#define DX_CHECKINPUT_MOUSE     (0x04)
#define DX_CHECKINPUT_ALL       (DX_CHECKINPUT_KEY | DX_CHECKINPUT_PAD | DX_CHECKINPUT_MOUSE)

/* -------------------------------------------------- JOYSTICK DEFINES */
#define DX_INPUT_PAD1           (0x0001)
#define DX_INPUT_PAD2           (0x0002)
#define DX_INPUT_PAD3           (0x0003)
#define DX_INPUT_PAD4           (0x0004)
#define DX_INPUT_PAD5           (0x0005)
#define DX_INPUT_PAD6           (0x0006)
#define DX_INPUT_PAD7           (0x0007)
#define DX_INPUT_PAD8           (0x0008)
#define DX_INPUT_PAD9           (0x0009)
#define DX_INPUT_PAD10          (0x000a)
#define DX_INPUT_PAD11          (0x000b)
#define DX_INPUT_PAD12          (0x000c)
#define DX_INPUT_PAD13          (0x000d)
#define DX_INPUT_PAD14          (0x000e)
#define DX_INPUT_PAD15          (0x000f)
#define DX_INPUT_PAD16          (0x0010)
#define DX_INPUT_KEY            (0x1000)
#define DX_INPUT_KEY_PAD1       (0x1001)

#define PAD_INPUT_DOWN          (0x00000001)
#define PAD_INPUT_LEFT          (0x00000002)
#define PAD_INPUT_RIGHT         (0x00000004)
#define PAD_INPUT_UP            (0x00000008)
#define PAD_INPUT_1             (0x00000010)
#define PAD_INPUT_2             (0x00000020)
#define PAD_INPUT_3             (0x00000040)
#define PAD_INPUT_4             (0x00000080)
#define PAD_INPUT_5             (0x00000100)
#define PAD_INPUT_6             (0x00000200)
#define PAD_INPUT_7             (0x00000400)
#define PAD_INPUT_8             (0x00000800)
#define PAD_INPUT_9             (0x00001000)
#define PAD_INPUT_10            (0x00002000)
#define PAD_INPUT_11            (0x00004000)
#define PAD_INPUT_12            (0x00008000)
#define PAD_INPUT_13            (0x00010000)
#define PAD_INPUT_14            (0x00020000)
#define PAD_INPUT_15            (0x00040000)
#define PAD_INPUT_16            (0x00080000)
#define PAD_INPUT_17            (0x00100000)
#define PAD_INPUT_18            (0x00200000)
#define PAD_INPUT_19            (0x00400000)
#define PAD_INPUT_20            (0x00800000)
#define PAD_INPUT_21            (0x01000000)
#define PAD_INPUT_22            (0x02000000)
#define PAD_INPUT_23            (0x04000000)
#define PAD_INPUT_24            (0x08000000)
#define PAD_INPUT_25            (0x10000000)
#define PAD_INPUT_26            (0x20000000)
#define PAD_INPUT_27            (0x40000000)
#define PAD_INPUT_28            (0x80000000)

#define PAD_INPUT_A             PAD_INPUT_1
#define PAD_INPUT_B             PAD_INPUT_2
#define PAD_INPUT_C             PAD_INPUT_3
#define PAD_INPUT_X             PAD_INPUT_4
#define PAD_INPUT_Y             PAD_INPUT_5
#define PAD_INPUT_Z             PAD_INPUT_6
#define PAD_INPUT_L             PAD_INPUT_7
#define PAD_INPUT_R             PAD_INPUT_8
#define PAD_INPUT_START         PAD_INPUT_9
#define PAD_INPUT_M             PAD_INPUT_10
#define PAD_INPUT_D             PAD_INPUT_11
#define PAD_INPUT_F             PAD_INPUT_12
#define PAD_INPUT_G             PAD_INPUT_13
#define PAD_INPUT_H             PAD_INPUT_14
#define PAD_INPUT_I             PAD_INPUT_15
#define PAD_INPUT_J             PAD_INPUT_16
#define PAD_INPUT_K             PAD_INPUT_17
#define PAD_INPUT_LL            PAD_INPUT_18
#define PAD_INPUT_N             PAD_INPUT_19
#define PAD_INPUT_O             PAD_INPUT_20
#define PAD_INPUT_P             PAD_INPUT_21
#define PAD_INPUT_RR            PAD_INPUT_22
#define PAD_INPUT_S             PAD_INPUT_23
#define PAD_INPUT_T             PAD_INPUT_24
#define PAD_INPUT_U             PAD_INPUT_25
#define PAD_INPUT_V             PAD_INPUT_26
#define PAD_INPUT_W             PAD_INPUT_27
#define PAD_INPUT_XX            PAD_INPUT_28

#define XINPUT_BUTTON_DPAD_UP           (0)
#define XINPUT_BUTTON_DPAD_DOWN         (1)
#define XINPUT_BUTTON_DPAD_LEFT         (2)
#define XINPUT_BUTTON_DPAD_RIGHT        (3)
#define XINPUT_BUTTON_START             (4)
#define XINPUT_BUTTON_BACK              (5)
#define XINPUT_BUTTON_LEFT_THUMB        (6)
#define XINPUT_BUTTON_RIGHT_THUMB       (7)
#define XINPUT_BUTTON_LEFT_SHOULDER     (8)
#define XINPUT_BUTTON_RIGHT_SHOULDER    (9)
#define XINPUT_BUTTON_A                 (10)
#define XINPUT_BUTTON_B                 (11)
#define XINPUT_BUTTON_X                 (12)
#define XINPUT_BUTTON_Y                 (13)

/* -------------------------------------------------- KEYBOARD DEFINES */
/* DIK uses a direct ANSI scancode mapping,
 * while SDL uses a USB scancode mapping.
 * 
 * These are ANSI scancodes, we convert in code as necessary.
 */
#define KEY_INPUT_ESCAPE        (0x01)
#define KEY_INPUT_1             (0x02)
#define KEY_INPUT_2             (0x03)
#define KEY_INPUT_3             (0x04)
#define KEY_INPUT_4             (0x05)
#define KEY_INPUT_5             (0x06)
#define KEY_INPUT_6             (0x07)
#define KEY_INPUT_7             (0x08)
#define KEY_INPUT_8             (0x09)
#define KEY_INPUT_9             (0x0a)
#define KEY_INPUT_0             (0x0b)
#define KEY_INPUT_MINUS         (0x0c)
#define KEY_INPUT_EQUALS        (0x0d)
#define KEY_INPUT_BACK          (0x0e)
#define KEY_INPUT_TAB           (0x0f)
#define KEY_INPUT_Q             (0x10)
#define KEY_INPUT_W             (0x11)
#define KEY_INPUT_E             (0x12)
#define KEY_INPUT_R             (0x13)
#define KEY_INPUT_T             (0x14)
#define KEY_INPUT_Y             (0x15)
#define KEY_INPUT_U             (0x16)
#define KEY_INPUT_I             (0x17)
#define KEY_INPUT_O             (0x18)
#define KEY_INPUT_P             (0x19)
#define KEY_INPUT_LBRACKET      (0x1a)
#define KEY_INPUT_RBRACKET      (0x1b)
#define KEY_INPUT_RETURN        (0x1c)
#define KEY_INPUT_LCONTROL      (0x1d)
#define KEY_INPUT_A             (0x1e)
#define KEY_INPUT_S             (0x1f)
#define KEY_INPUT_D             (0x20)
#define KEY_INPUT_F             (0x21)
#define KEY_INPUT_G             (0x22)
#define KEY_INPUT_H             (0x23)
#define KEY_INPUT_J             (0x24)
#define KEY_INPUT_K             (0x25)
#define KEY_INPUT_L             (0x26)
#define KEY_INPUT_SEMICOLON     (0x27)
/* APOSTROPHE 0x28 */
/* GRAVE 0x29 */
#define KEY_INPUT_LSHIFT        (0x2a)
#define KEY_INPUT_BACKSLASH     (0x2b)
#define KEY_INPUT_Z             (0x2c)
#define KEY_INPUT_X             (0x2d)
#define KEY_INPUT_C             (0x2e)
#define KEY_INPUT_V             (0x2f)
#define KEY_INPUT_B             (0x30)
#define KEY_INPUT_N             (0x31)
#define KEY_INPUT_M             (0x32)
#define KEY_INPUT_COMMA         (0x33)
#define KEY_INPUT_PERIOD        (0x34)
#define KEY_INPUT_SLASH         (0x35)
#define KEY_INPUT_RSHIFT        (0x36)
#define KEY_INPUT_MULTIPLY      (0x37)
#define KEY_INPUT_LALT          (0x38)
#define KEY_INPUT_SPACE         (0x39)
#define KEY_INPUT_CAPSLOCK      (0x3a)
#define KEY_INPUT_F1            (0x3b)
#define KEY_INPUT_F2            (0x3c)
#define KEY_INPUT_F3            (0x3d)
#define KEY_INPUT_F4            (0x3e)
#define KEY_INPUT_F5            (0x3f)
#define KEY_INPUT_F6            (0x40)
#define KEY_INPUT_F7            (0x41)
#define KEY_INPUT_F8            (0x42)
#define KEY_INPUT_F9            (0x43)
#define KEY_INPUT_F10           (0x44)
#define KEY_INPUT_NUMLOCK       (0x45)
#define KEY_INPUT_SCROLL        (0x46)
#define KEY_INPUT_NUMPAD7       (0x47)
#define KEY_INPUT_NUMPAD8       (0x48)
#define KEY_INPUT_NUMPAD9       (0x49)
#define KEY_INPUT_SUBTRACT      (0x4a)
#define KEY_INPUT_NUMPAD4       (0x4b)
#define KEY_INPUT_NUMPAD5       (0x4c)
#define KEY_INPUT_NUMPAD6       (0x4d)
#define KEY_INPUT_ADD           (0x4e)
#define KEY_INPUT_NUMPAD1       (0x4f)
#define KEY_INPUT_NUMPAD2       (0x50)
#define KEY_INPUT_NUMPAD3       (0x51)
#define KEY_INPUT_NUMPAD0       (0x52)
#define KEY_INPUT_DECIMAL       (0x53)

#define KEY_INPUT_F11           (0x57)
#define KEY_INPUT_F12           (0x58)

#define KEY_INPUT_KANA          (0x70) /* unsupported */
#define KEY_INPUT_CONVERT       (0x79) /* unsupported */
#define KEY_INPUT_NOCONVERT     (0x7b) /* unsupported */
#define KEY_INPUT_YEN           (0x7d)

#define KEY_INPUT_PREVTRACK     (0x90) /* unsupported */
#define KEY_INPUT_AT            (0x91) /* maybe supported? */
#define KEY_INPUT_COLON         (0x92) /* maybe supported? */
#define KEY_INPUT_KANJI         (0x94) /* unsupported */

#define KEY_INPUT_NUMPADENTER   (0x9c)
#define KEY_INPUT_RCONTROL      (0x9d)
#define KEY_INPUT_NUMPADCOMMA   (0xb3)
#define KEY_INPUT_DIVIDE        (0xb5)
#define KEY_INPUT_SYSRQ         (0xb7)
#define KEY_INPUT_RALT          (0xb8)

#define KEY_INPUT_PAUSE         (0xc5)
#define KEY_INPUT_HOME          (0xc7)
#define KEY_INPUT_UP            (0xc8)
#define KEY_INPUT_PGUP          (0xc9)
#define KEY_INPUT_LEFT          (0xcb)
#define KEY_INPUT_RIGHT         (0xcd)
#define KEY_INPUT_END           (0xcf)
#define KEY_INPUT_DOWN          (0xd0)
#define KEY_INPUT_PGDN          (0xd1)
#define KEY_INPUT_INSERT        (0xd2)
#define KEY_INPUT_DELETE        (0xd3)

#define KEY_INPUT_LWIN          (0xdb)
#define KEY_INPUT_RWIN          (0xdc)
#define KEY_INPUT_APPS          (0xdd) /* unsupported */

/* ----------------------------------------------------- MOUSE DEFINES */
#define MOUSE_INPUT_LEFT        (0x01)
#define MOUSE_INPUT_MIDDLE      (0x02)
#define MOUSE_INPUT_RIGHT       (0x04)
#define MOUSE_INPUT_1           (0x01)
#define MOUSE_INPUT_2           (0x02)
#define MOUSE_INPUT_3           (0x04)
#define MOUSE_INPUT_4           (0x08)
#define MOUSE_INPUT_5           (0x10)
#define MOUSE_INPUT_6           (0x20)
#define MOUSE_INPUT_7           (0x40)
#define MOUSE_INPUT_8           (0x80)

/* ----------------------------------------------------- SOUND DEFINES */
#define DX_PLAYTYPE_LOOPBIT     (0x0002)
#define DX_PLAYTYPE_BACKBIT     (0x0001)

#define DX_PLAYTYPE_NORMAL      (0x0000)
#define DX_PLAYTYPE_BACK        (DX_PLAYTYPE_BACKBIT)
#define DX_PLAYTYPE_LOOP        (DX_PLAYTYPE_BACKBIT | DX_PLAYTYPE_LOOPBIT)

/* Only MEMPRESS is supported. */
#define DX_SOUNDDATATYPE_MEMNOPRESS             (0)
#define DX_SOUNDDATATYPE_MEMNOPRESS_PLUS        (1)
#define DX_SOUNDDATATYPE_MEMPRESS               (2)
#define DX_SOUNDDATATYPE_FILE                   (3)

#ifdef __cplusplus   
}
#endif

#endif

