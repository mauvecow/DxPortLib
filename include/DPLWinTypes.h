/*
  DxPortLib - A portability library for DxLib-based software.
  Copyright (C) 2013-2016 Patrick McCarthy <mauve@sandwich.net>
  
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

#ifndef DPLWINTYPES_H_HEADER
#define DPLWINTYPES_H_HEADER

#include <stdint.h>

#ifdef _WIN32
#  define _WIN32_WINNT 0x0400
#  define WIN32_LEAN_AND_MEAN
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
typedef struct _POINT {
    int x;
    int y;
} POINT;

typedef int BOOL;
typedef int64_t LONGLONG;
typedef int32_t LONG;
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;

typedef unsigned long int ULONG_PTR;
typedef unsigned long int DWORD_PTR;

#  ifdef UNICODE
typedef wchar_t TCHAR;
#  else
typedef char TCHAR;
#  endif
#endif

/* Verify that all tchar defines exist. */
#ifndef _TEXT
#  ifdef UNICODE
#    define _TEXT(s) L ## s
#  else
#    define _TEXT(s) s
#  endif
#endif

#ifndef _T
#  define _T(s) _TEXT(s)
#endif
#ifndef __TEXT
#  define __TEXT(s) _TEXT(s)
#endif
#ifndef TEXT
#  define TEXT(s) _TEXT(s)
#endif

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

typedef uint32_t D3DCOLOR;

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
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME;

#define MAX_PATH 4096
#endif

#endif /* #ifndef DPLWINTYPES_H_HEADER */
