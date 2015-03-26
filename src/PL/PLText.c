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

#include "PLInternal.h"

static int s_useCharSet = DX_CHARSET_EXT_UTF8;

unsigned int PL_Text_ReadUTF8Char(const char **textRef) {
    const unsigned char *text = (const unsigned char *)*textRef;
    unsigned char ch = *text;
    unsigned int value;
    
    if (ch == 0) {
        return 0;
    }
    
    ++text;
    
    if ((ch & 0x80) == 0) {
        value = ch;
    } else {
        int bytes;
        
        if ((ch & 0xe0) == 0xc0) {
            value = ch & 0x1f;
            bytes = 2;
        } else if ((ch & 0xf0) == 0xe0) {
            value = ch & 0x0f;
            bytes = 3;
        } else if ((ch & 0xf8) == 0xf0) {
            value = ch & 0x07;
            bytes = 4;
        } else if ((ch & 0xfc) == 0xf8) {
            value = ch & 0x03;
            bytes = 5;
        } else if ((ch & 0xfe) == 0xfc) {
            value = ch & 0x01;
            bytes = 6;
        } else {
            value = '?';
            bytes = 0;
        }
        
        while (bytes > 0 && ((ch = *text) & 0xc0) == 0x80) {
            value = (value << 6) | (ch & 0x3f);
            ++text;
            --bytes;
        }
    }
    
    *textRef = (const char *)text;
    
    return value;
}

int PL_Text_WriteUTF8Char(char *buffer, unsigned int ch, int maxLen) {
    if (ch < 0x80) {
        if (maxLen >= 1) {
            buffer[0] = (char)ch;
        
            return 1;
        }
    } else if (ch < 0x800) {
        if (maxLen >= 2) {
            buffer[0] = (char)(0xc0 | ((ch >> 6) & 0x1f));
            buffer[1] = (char)(0x80 | (ch & 0x3f));
            
            return 2;
        }
    } else if (ch < 0x10000) {
        if (maxLen >= 3) {
            buffer[0] = (char)(0xe0 | ((ch >> 12) & 0x0f));
            buffer[1] = (char)(0x80 | ((ch >> 6) & 0x3f));
            buffer[2] = (char)(0x80 | (ch & 0x3f));
            
            return 3;
        }
    } else if (ch < 0x200000) {
        if (maxLen >= 4) {
            buffer[0] = (char)(0xf0 | ((ch >> 18) & 0x07));
            buffer[1] = (char)(0x80 | ((ch >> 12) & 0x3f));
            buffer[2] = (char)(0x80 | ((ch >> 6) & 0x3f));
            buffer[3] = (char)(0x80 | (ch & 0x3f));
            
            return 4;
        }
    } else if (ch < 0x4000000) {
        if (maxLen >= 5) {
            buffer[0] = (char)(0xf8 | ((ch >> 24) & 0x03));
            buffer[1] = (char)(0x80 | ((ch >> 18) & 0x3f));
            buffer[2] = (char)(0x80 | ((ch >> 12) & 0x3f));
            buffer[3] = (char)(0x80 | ((ch >> 6) & 0x3f));
            buffer[4] = (char)(0x80 | (ch & 0x3f));
            
            return 5;
        }
    } else {
        if (maxLen >= 6) {
            buffer[0] = (char)(0xfc | ((ch >> 30) & 0x01));
            buffer[1] = (char)(0x80 | ((ch >> 24) & 0x3f));
            buffer[2] = (char)(0x80 | ((ch >> 18) & 0x3f));
            buffer[3] = (char)(0x80 | ((ch >> 12) & 0x3f));
            buffer[4] = (char)(0x80 | ((ch >> 6) & 0x3f));
            buffer[5] = (char)(0x80 | (ch & 0x3f));
            
            return 6;
        }
    }
    
    return 0;    
}

int PL_Text_IsIncompleteUTF8Char(const char *buffer, int length) {
    unsigned char ch = (unsigned char)buffer[0];
    
    if ((ch & 0x80) == 0) {
        return DXFALSE;
    }
    if ((ch & 0xe0) == 0xc0) {
        return (length >= 2) ? DXFALSE : DXTRUE;
    } else if ((ch & 0xf0) == 0xe0) {
        return (length >= 3) ? DXFALSE : DXTRUE;
    } else if ((ch & 0xf8) == 0xf0) {
        return (length >= 4) ? DXFALSE : DXTRUE;
    } else if ((ch & 0xfc) == 0xf8) {
        return (length >= 5) ? DXFALSE : DXTRUE;
    } else if ((ch & 0xfe) == 0xfc) {
        return (length >= 6) ? DXFALSE : DXTRUE;
    }
    
    return DXFALSE;
}

unsigned int PL_Text_ReadChar(const char **textRef, int charset) {
    switch(charset) {
#ifndef DXPORTLIB_NO_SJIS
        case DX_CHARSET_SHFTJIS:
            return PL_Text_ReadSJISChar(textRef);
#endif /* #ifndef DXPORTLIB_NON_SJIS */
        case DX_CHARSET_EXT_UTF8:
            return PL_Text_ReadUTF8Char(textRef);
        default:
            return PL_Text_ReadChar(textRef, s_useCharSet);
    }
}

int PL_Text_WriteChar(char *text, unsigned int ch, int maxLen, int charset) {
    switch(charset) {
#ifndef DXPORTLIB_NO_SJIS
        case DX_CHARSET_SHFTJIS:
            return PL_Text_WriteSJISChar(text, ch, maxLen);
#endif /* #ifndef DXPORTLIB_NON_SJIS */
        case DX_CHARSET_EXT_UTF8:
            return PL_Text_WriteUTF8Char(text, ch, maxLen);
        default:
            return PL_Text_WriteChar(text, ch, maxLen, s_useCharSet);
    }
}

#ifdef UNICODE
unsigned int PL_Text_ReadUNICODEChar(const wchar_t **textRef) {
    const wchar_t *text = *textRef;
    unsigned int ch = *text;
    
    *textRef = text + 1;
    
    return ch;
}
int PL_Text_WriteUNICODEChar(wchar_t *text, unsigned int ch, int maxLen) {
    if (maxLen >= 1) {
        *text = ch;
        return 1;
    }
    
    return 0;
}

/* DXCHAR is wchar_t */
unsigned int PL_Text_ReadDxChar(const DXCHAR **textRef) {
    return PL_Text_ReadUNICODEChar(textRef);
}
int PL_Text_WriteDxChar(DXCHAR *text, unsigned int ch, int maxLen) {
    return PL_Text_WriteUNICODEChar(text, ch, maxLen);
}
#else
/* DXCHAR is CharSet */
unsigned int PL_Text_ReadDxChar(const DXCHAR **textRef) {
    return PL_Text_ReadChar((const char **)textRef, s_useCharSet);
}
int PL_Text_WriteDxChar(DXCHAR *text, unsigned int ch, int maxLen) {
    return PL_Text_WriteChar((char *)text, ch, maxLen, s_useCharSet);
}

#endif

/* glorified strcpys go here */
int PL_Text_DxStringToString(const DXCHAR *inString, char *outBuffer, int maxLen, int charset) {
    unsigned int ch;
    int count = 0;
    
    maxLen -= 1;
    
    while (count < maxLen && (ch = PL_Text_ReadDxChar(&inString)) != 0) {
        count += PL_Text_WriteChar(outBuffer + count, ch, maxLen - count, charset);
    }
    
    outBuffer[count] = '\0';
    
    return count;
}

int PL_Text_StringToDxString(const char *inString, DXCHAR *outBuffer, int maxLen, int charset) {
    unsigned int ch;
    int count = 0;
    
    maxLen -= 1;
    
    while (count < maxLen && (ch = PL_Text_ReadChar(&inString, charset)) != 0) {
        count += PL_Text_WriteDxChar(outBuffer + count, ch, maxLen - count);
    }
    
    outBuffer[count] = '\0';
    
    return count;
}

int PL_Text_DxStrlen(const DXCHAR *str) {
    int i = 0;
    
    while (str[i] != 0) {
        ++i;
    }
    
    return i;
}

DXCHAR *PL_Text_DxStrdup(const DXCHAR *str) {
    int len = PL_Text_DxStrlen(str);
    DXCHAR *dest = DXALLOC((unsigned int)(len + 1) * sizeof(DXCHAR));
    int i;
    
    for (i = 0; i < len; ++i) {
        dest[i] = str[i];
    }
    dest[len] = 0;
    
    return dest;
}

void PL_Text_DxStrncat(DXCHAR *str, const DXCHAR *catStr, int maxLen) {
    int len = PL_Text_DxStrlen(str);
    DXCHAR *end = str + maxLen - 1;
    DXCHAR ch;
    
    str += len;
    
    while (str < end && (ch = (*catStr++)) != 0) {
        *str++ = ch;
    }
    
    *str = 0;
}

void PL_Text_DxStrncpy(DXCHAR *str, const DXCHAR *cpyStr, int maxLen) {
    DXCHAR *end = str + maxLen - 1;
    DXCHAR ch;
    
    while (str < end && (ch = (*cpyStr++)) != 0) {
        *str++ = ch;
    }
    
    *str = 0;
}

void PL_Text_DxStrncatFromString(DXCHAR *str, const char *catStr, int maxLen, int charSet) {
    int len = PL_Text_DxStrlen(str);
    unsigned int ch;
    
    str += len;
    maxLen -= len + 1;
    
    while ((ch = PL_Text_ReadChar(&catStr, charSet)) != 0 && maxLen > 0) {
        int count = PL_Text_WriteDxChar(str, ch, maxLen);
        maxLen -= count;
        str += count;
    }
    
    *str = 0;
}

void PL_Text_DxStrncpyFromString(DXCHAR *str, const char *cpyStr, int maxLen, int charSet) {
    unsigned int ch;
    
    maxLen -= 1;
    
    while ((ch = PL_Text_ReadChar(&cpyStr, charSet)) != 0 && maxLen > 0) {
        int count = PL_Text_WriteDxChar(str, ch, maxLen);
        maxLen -= count;
        str += count;
    }
    
    *str = 0;
}

int PL_Text_DxStrcmp(const DXCHAR *strA, const DXCHAR *strB) {
    int v;
    
    while ((v = (*strB - *strA)) == 0 && *strA != 0) {
        strA += 1;
        strB += 1;
    }
    
    return v;
}
int PL_Text_DxStrcasecmp(const DXCHAR *strA, const DXCHAR *strB) {
    unsigned int a, b;
    int v;
    
    do {
        a = PL_Text_ReadDxChar(&strA);
        b = PL_Text_ReadDxChar(&strB);
        v = b - a;
        if (v != 0) {
            if (a >= 'A' && a <= 'Z') {
                a += 'a' - 'A';
            }
            if (b >= 'A' && b <= 'Z') {
                b += 'a' - 'A';
            }
            v = b - a;
        }
    } while (v == 0 && a != 0 && b != 0);
    
    return v;
}
int PL_Text_DxStrncmp(const DXCHAR *strA, const DXCHAR *strB, int maxLen) {
    int v = 0;
    const DXCHAR *end = strA + maxLen;
    
    while (strA < end && (v = (*strB - *strA)) == 0 && *strA != 0) {
        strA += 1;
        strB += 1;
    }
    
    return v;
}
int PL_Text_DxStrncasecmp(const DXCHAR *strA, const DXCHAR *strB, int maxLen) {
    const DXCHAR *endA = strA + maxLen;
    unsigned int a, b;
    int v;
    
    do {
        a = PL_Text_ReadDxChar(&strA);
        b = PL_Text_ReadDxChar(&strB);
        v = b - a;
        if (v != 0) {
            if (a >= 'A' && a <= 'Z') {
                a += 'a' - 'A';
            }
            if (b >= 'A' && b <= 'Z') {
                b += 'a' - 'A';
            }
            v = b - a;
        }
    } while (v == 0 && a != 0 && b != 0 && strA < endA);
    
    return v;
}
const DXCHAR *PL_Text_DxStrstr(const DXCHAR *strA, const DXCHAR *strB) {
    int n = PL_Text_DxStrlen(strB);
    while (*strA) {
        if (!PL_Text_DxStrncmp(strA, strB, n)) {
            return strA;
        }
        PL_Text_ReadDxChar(&strA);
    }
    return NULL;
}
const DXCHAR *PL_Text_DxStrcasestr(const DXCHAR *strA, const DXCHAR *strB) {
    int n = PL_Text_DxStrlen(strB);
    while (*strA) {
        if (!PL_Text_DxStrncasecmp(strA, strB, n)) {
            return strA;
        }
        PL_Text_ReadDxChar(&strA);
    }
    return NULL;
}
int PL_Text_DxStrTestExt(const DXCHAR *str, const DXCHAR *ext) {
    int slen = PL_Text_DxStrlen(str);
    int elen = PL_Text_DxStrlen(ext);
    const DXCHAR *target = str + slen - elen;
    
    if (slen < elen) {
        return DXFALSE;
    }
    
    while (str < target) {
        PL_Text_ReadDxChar(&str);
    }
    if (str != target) {
        return DXFALSE;
    }
    
    if (PL_Text_DxStrcasecmp(str, ext) == 0) {
        return DXTRUE;
    }
    return DXFALSE;
}

int PL_Text_IsIncompleteMultibyte(const char *string, int length, int charset) {
    switch(charset) {
#ifndef DXPORTLIB_NO_SJIS
        case DX_CHARSET_SHFTJIS:
            return PL_Text_IsIncompleteSJISChar(string, length);
#endif
        case DX_CHARSET_EXT_UTF8:
            return PL_Text_IsIncompleteUTF8Char(string, length);
        default:
            return PL_Text_IsIncompleteMultibyte(string, length, s_useCharSet);
    }
}

int PL_Text_SetUseCharSet(int charset) {
    switch(charset) {
#ifndef DXPORTLIB_NO_SJIS
        case DX_CHARSET_SHFTJIS: break;
#endif /* #ifndef DXPORTLIB_NON_SJIS */
        case DX_CHARSET_EXT_UTF8: break;
        default: return -1;
    }
    
    s_useCharSet = charset;
    
    return 0;
}

int PL_Text_GetUseCharSet(int charset) {
    return s_useCharSet;
}
