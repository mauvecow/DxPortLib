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

#include "PLInternal.h"

static const int s_defaultCharset = DX_CHARSET_EXT_UTF8;

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

int PL_Text_WriteUTF8Char(char *buffer, unsigned int ch, int bufSize) {
    if (ch < 0x80) {
        if (bufSize >= 1) {
            buffer[0] = (char)ch;
        
            return 1;
        }
    } else if (ch < 0x800) {
        if (bufSize >= 2) {
            buffer[0] = (char)(0xc0 | ((ch >> 6) & 0x1f));
            buffer[1] = (char)(0x80 | (ch & 0x3f));
            
            return 2;
        }
    } else if (ch < 0x10000) {
        if (bufSize >= 3) {
            buffer[0] = (char)(0xe0 | ((ch >> 12) & 0x0f));
            buffer[1] = (char)(0x80 | ((ch >> 6) & 0x3f));
            buffer[2] = (char)(0x80 | (ch & 0x3f));
            
            return 3;
        }
    } else if (ch < 0x200000) {
        if (bufSize >= 4) {
            buffer[0] = (char)(0xf0 | ((ch >> 18) & 0x07));
            buffer[1] = (char)(0x80 | ((ch >> 12) & 0x3f));
            buffer[2] = (char)(0x80 | ((ch >> 6) & 0x3f));
            buffer[3] = (char)(0x80 | (ch & 0x3f));
            
            return 4;
        }
    } else if (ch < 0x4000000) {
        if (bufSize >= 5) {
            buffer[0] = (char)(0xf8 | ((ch >> 24) & 0x03));
            buffer[1] = (char)(0x80 | ((ch >> 18) & 0x3f));
            buffer[2] = (char)(0x80 | ((ch >> 12) & 0x3f));
            buffer[3] = (char)(0x80 | ((ch >> 6) & 0x3f));
            buffer[4] = (char)(0x80 | (ch & 0x3f));
            
            return 5;
        }
    } else {
        if (bufSize >= 6) {
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
        default: /* case DX_CHARSET_EXT_UTF8: */
            return PL_Text_ReadUTF8Char(textRef);
    }
}

int PL_Text_WriteChar(char *text, unsigned int ch, int bufSize, int charset) {
    switch(charset) {
#ifndef DXPORTLIB_NO_SJIS
        case DX_CHARSET_SHFTJIS:
            return PL_Text_WriteSJISChar(text, ch, bufSize);
#endif /* #ifndef DXPORTLIB_NON_SJIS */
        default: /* case DX_CHARSET_EXT_UTF8: */
            return PL_Text_WriteUTF8Char(text, ch, bufSize);
    }
}

int PL_Text_ToCharset(int charset) {
    switch(charset) {
#ifndef DXPORTLIB_NO_SJIS
        case DX_CHARSET_SHFTJIS:
            return DX_CHARSET_SHFTJIS;
#endif /* #ifndef DXPORTLIB_NON_SJIS */
        case DX_CHARSET_EXT_UTF8:
            return DX_CHARSET_EXT_UTF8;
        default:
            return s_defaultCharset;
    }
}

int PL_Text_ConvertString(const char *srcStr, int srcCharset,
                          char *dest, int destCharset,
                          int bufSize) {
    unsigned int ch;
    int count = 0;
    
    if (bufSize <= 0) {
        return 0;
    }
    
    srcCharset = PL_Text_ToCharset(srcCharset);
    destCharset = PL_Text_ToCharset(destCharset);
    
    if (srcCharset == destCharset) {
        return PL_Text_Strncpy(dest, srcStr, bufSize);
    }
    
    bufSize -= 1;
    
    while (count < bufSize && (ch = PL_Text_ReadChar(&srcStr, srcCharset)) != 0) {
        count += PL_Text_WriteChar(dest + count, ch, bufSize - count, destCharset);
    }
    
    dest[count] = '\0';
    
    return count;
}

const char *PL_Text_ConvertStringIfNecessary(
            char *dest, int destCharset,
            const char *srcStr, int srcCharset,
            int bufSize) {
    srcCharset = PL_Text_ToCharset(srcCharset);
    destCharset = PL_Text_ToCharset(destCharset);
    
    if (srcCharset == destCharset) {
        return srcStr;
    }
    
    PL_Text_ConvertStrncpy(dest, destCharset, srcStr, srcCharset, bufSize);
    
    return dest;
}


int PL_Text_WideCharToString(char *dest, int charset, const wchar_t *srcStr, int bufSize) {
    unsigned int ch;
    int count = 0;
    int index = 0;
    
    if (bufSize <= 0) {
        return 0;
    }
    
    charset = PL_Text_ToCharset(charset);
    
    bufSize -= 1;
    
    while (count < bufSize && (ch = srcStr[index]) != 0) {
        count += PL_Text_WriteChar(dest + count, ch, bufSize - count, charset);
        index += 1;
    }
    
    dest[count] = '\0';
    
    return count;
}

int PL_Text_StringToWideChar(wchar_t *dest, const char *srcStr, int charset, int bufSize) {
    unsigned int ch;
    int count = 0;
    
    if (bufSize <= 0) {
        return 0;
    }
    
    charset = PL_Text_ToCharset(charset);
    
    bufSize -= 1;
    
    while (count < bufSize && (ch = PL_Text_ReadChar(&srcStr, charset)) != 0) {
        dest[count] = ch;
        count += 1;
    }
    
    dest[count] = '\0';
    
    return count;
}

int PL_Text_Strlen(const char *str) {
    int i = 0;
    
    while (str[i] != 0) {
        ++i;
    }
    
    return i;
}

char *PL_Text_Strdup(const char *str) {
    int len = PL_Text_Strlen(str);
    char *dest = DXALLOC((unsigned int)(len + 1) * sizeof(char));
    int i;
    
    for (i = 0; i < len; ++i) {
        dest[i] = str[i];
    }
    dest[len] = 0;
    
    return dest;
}

int PL_Text_Strncat(char *str, const char *catStr, int bufSize) {
    int count = 0;
    char ch;
    
    if (bufSize <= 0) {
        return 0;
    }
    
    bufSize -= 1;
    
    while (count < bufSize && (ch = (*catStr++)) != 0) {
        str[count++] = ch;
    }
    
    str[count] = 0;
    return count;
}

int PL_Text_Strncpy(char *str, const char *cpyStr, int bufSize) {
    int count = 0;
    char ch;
    
    if (bufSize <= 0) {
        return 0;
    }
    
    bufSize -= 1;
    
    while (count < bufSize && (ch = (*cpyStr++)) != 0) {
        str[count++] = ch;
    }
    
    str[count] = 0;
    return count;
}

int PL_Text_ConvertStrncat(char *dest, int destCharset, const char *srcStr, int srcCharset, int bufSize) {
    int len = PL_Text_Strlen(dest);
    
    return PL_Text_ConvertStrncpy(dest + len, destCharset, srcStr, srcCharset, bufSize - len);
}

int PL_Text_Strcmp(const char *strA, const char *strB) {
    int v;
    
    while ((v = (*strB - *strA)) == 0 && *strA != 0) {
        strA += 1;
        strB += 1;
    }
    
    return v;
}
int PL_Text_Strcasecmp(const char *strA, const char *strB) {
    unsigned int a, b;
    int v;
    
    do {
        a = PL_Text_ReadUTF8Char(&strA);
        b = PL_Text_ReadUTF8Char(&strB);
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
int PL_Text_Strncmp(const char *strA, const char *strB, int bufSize) {
    int v = 0;
    const char *end = strA + bufSize;
    
    while (strA < end && (v = (*strB - *strA)) == 0 && *strA != 0) {
        strA += 1;
        strB += 1;
    }
    
    return v;
}
int PL_Text_Strncasecmp(const char *strA, const char *strB, int bufSize) {
    const char *endA = strA + bufSize;
    unsigned int a, b;
    int v;
    
    do {
        a = PL_Text_ReadUTF8Char(&strA);
        b = PL_Text_ReadUTF8Char(&strB);
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
const char *PL_Text_Strstr(const char *strA, const char *strB) {
    int n = PL_Text_Strlen(strB);
    while (*strA) {
        if (!PL_Text_Strncmp(strA, strB, n)) {
            return strA;
        }
        PL_Text_ReadUTF8Char(&strA);
    }
    return NULL;
}
const char *PL_Text_Strcasestr(const char *strA, const char *strB) {
    int n = PL_Text_Strlen(strB);
    while (*strA) {
        if (!PL_Text_Strncasecmp(strA, strB, n)) {
            return strA;
        }
        PL_Text_ReadUTF8Char(&strA);
    }
    return NULL;
}
int PL_Text_StrTestExt(const char *str, const char *ext) {
    int slen = PL_Text_Strlen(str);
    int elen = PL_Text_Strlen(ext);
    const char *target = str + slen - elen;
    
    if (slen < elen) {
        return DXFALSE;
    }
    
    while (str < target) {
        PL_Text_ReadUTF8Char(&str);
    }
    if (str != target) {
        return DXFALSE;
    }
    
    if (PL_Text_Strcasecmp(str, ext) == 0) {
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
        default: /* case DX_CHARSET_EXT_UTF8: */
            return PL_Text_IsIncompleteUTF8Char(string, length);
    }
}
