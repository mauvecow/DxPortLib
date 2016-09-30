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

#include "PLInternal.h"

/* Not all platforms have these, so we implement our own, with custom
 * locale support. Suffering.
 *
 * Loosely based on SDL2's code, also MIT license.
 */
 
typedef struct s_PrintParams {
    int justifyLeft;
    int forceSign;
    int forceType;
    char pad;
    int caseLevel;

    int width;
    int precision;
    int radix;
    int charset;
} s_PrintParams;

static int s_printString(char *dest, const char *end, s_PrintParams *params, const char *str) {
    char *start = dest;
    
    if (params->width > 0) {
        char filler = params->pad;
        int len = PL_Text_Strlen(str);
        int width = params->width;
        
        while (width > len && dest < end) {
            *dest++ = filler;
            width -= 1;
        }
    }
    
    PL_Text_Strncpy(dest, str, end - dest + 1);
    
    if (params->caseLevel == -1) {
        PL_Text_StrLower(dest, params->charset);
    } else if (params->caseLevel == 1) {
        PL_Text_StrUpper(dest, params->charset);
    }
    
    return dest - start;
}

static int s_printWString(char *dest, const char *end, s_PrintParams *params, const wchar_t *str) {
    char *start = dest;
    
    if (params->width > 0) {
        char filler = params->pad;
        int len = PL_Text_StrlenW(str);
        int width = params->width;
        
        while (width > len && dest < end) {
            *dest++ = filler;
            width -= 1;
        }
    }
    
    PL_Text_WideCharToString(dest, params->charset, str, end - dest + 1);
    
    if (params->caseLevel == -1) {
        PL_Text_StrLower(dest, params->charset);
    } else if (params->caseLevel == 1) {
        PL_Text_StrUpper(dest, params->charset);
    }
    
    return dest - start;
}

static int s_printStringW(wchar_t *dest, const wchar_t *end, s_PrintParams *params, const char *str) {
    wchar_t *start = dest;
    
    if (params->width > 0) {
        char filler = params->pad;
        int len = PL_Text_Strlen(str);
        int width = params->width;
        
        while (width > len && dest < end) {
            *dest++ = filler;
            width -= 1;
        }
    }
    
    PL_Text_StringToWideChar(dest, str, params->charset, end - dest + 1);
    
    if (params->caseLevel == -1) {
        PL_Text_StrLowerW(dest);
    } else if (params->caseLevel == 1) {
        PL_Text_StrUpperW(dest);
    }
    
    return dest - start;
}
static int s_printWStringW(wchar_t *dest, const wchar_t *end, s_PrintParams *params, const wchar_t *str) {
    wchar_t *start = dest;
    
    if (params->width > 0) {
        char filler = params->pad;
        int len = PL_Text_StrlenW(str);
        int width = params->width;
        
        while (width > len && dest < end) {
            *dest++ = filler;
            width -= 1;
        }
    }
    
    PL_Text_StrncpyW(dest, str, end - dest + 1);
    
    if (params->caseLevel == -1) {
        PL_Text_StrLowerW(dest);
    } else if (params->caseLevel == 1) {
        PL_Text_StrUpperW(dest);
    }
    
    return dest - start;
}

static const unsigned char s_radixTable[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z'
};
static const int s_radixTableLen = sizeof(s_radixTable) / sizeof(s_radixTable[0]);

static void s_reverseString(char *start, char *end) {
    end -= 1;
    while (start < end) {
        char c = *end;
        *end = *start;
        *start = c;
        
        start += 1;
        end -= 1;
    }
}

static int s_printUnsignedLong(char *dest, const char *end, s_PrintParams *params, unsigned long value) {
    char *start = dest;
    if (dest == end) {
        return dest - start;
    }
    
    if (params->radix < 0 || params->radix >= s_radixTableLen) {
        params->radix = 10;
    }
    
    if (params->radix == 16) {
        if ((end-dest) >= 2) {
            *dest++ = '0';
            *dest++ = 'x';
        }
    }
    
    if (value == 0) {
        *dest++ = '0';
    } else {
        char *numstart = dest;
        int radix = params->radix;
        while (value > 0 && dest < end) {
            *dest++ = s_radixTable[value % radix];
            value /= radix;
        }
        s_reverseString(numstart, dest);
    }
    
    *dest = 0;
    return dest - start;
}

static int s_printUint64(char *dest, const char *end, s_PrintParams *params, uint64_t value) {
    char *start = dest;
    if (dest == end) {
        return dest - start;
    }
    
    if (params->radix < 0 || params->radix >= s_radixTableLen) {
        params->radix = 10;
    }
    
    if (params->radix == 16) {
        if ((end-dest) >= 2) {
            *dest++ = '0';
            *dest++ = 'x';
        }
    }
    
    if (value == 0) {
        *dest++ = '0';
    } else {
        char *numstart = dest;
        int radix = params->radix;
        while (value > 0 && dest < end) {
            *dest++ = s_radixTable[value % radix];
            value /= radix;
        }
        s_reverseString(numstart, dest);
    }
    *dest = 0;
    return dest - start;
}
static int s_printLong(char *dest, const char *end, s_PrintParams *params, long value) {
    char *start = dest;
    if (dest == end) {
        return dest - start;
    }
    if (value < 0) {
        *dest++ = '-';
        value = -value;
    } else if (params->forceSign != 0) {
        *dest++ = '+';
    }
    s_printUnsignedLong(dest, end, params, (unsigned long)value);
    return dest - start;
}
static int s_printSint64(char *dest, const char *end, s_PrintParams *params, int64_t value) {
    char *start = dest;
    if (dest == end) {
        return dest - start;
    }
    if (value < 0) {
        *dest++ = '-';
        value = -value;
    } else if (params->forceSign != 0) {
        *dest++ = '+';
    }
    s_printUint64(dest, end, params, (uint64_t)value);
    return dest - start;
}

static int s_printDouble(char *dest, const char *end, s_PrintParams *params, double value) {
    char *start = dest;
    double floorV;
    int radix = params->radix;
    
    if (dest == end) {
        return dest - start;
    }
    
    if (radix < 0 || radix >= s_radixTableLen) {
        radix = 10;
    }
    
    if (value < 0) {
        *dest++ = '-';
        value = -value;
    } else if (params->forceSign != 0) {
        *dest++ = '+';
    }
    
    if (dest == end) {
        return dest - start;
    }
    
    /* Handle numeric part. */
    floorV = floor(value);
    if (floorV == 0) {
        *dest++ = '0';
    } else {
        char *start = dest;
        value -= floorV;
        
        /* Slooooow */
        do {
            double v = floorV / radix;
            int c;
            
            floorV = floor(v);
            c = (int)floor((v - floorV) * radix);
            
            *dest++ = s_radixTable[c];
        } while (floorV > 0 && dest < end);
        
        s_reverseString(start, dest);
    }
    
    /* Handle fractional part. */
    if (value > 0 && dest < end && params->precision != 0) {
        int precision = params->precision;
        if (precision < 0) {
            precision = 6;
        }
        
        *dest++ = '.';
        
        while (value > 0 && precision != 0 && dest < end) {
            int c;
            value *= radix;
            
            c = (int)floor(value);
            *dest++ = s_radixTable[c];
            
            value -= c;
        }
    }
    
    *dest = '\0';
    return dest - start;
}

static const int s_numBufSize = 255;

int PL_Text_Vsnprintf(char *dest, int bufSize, int charset, const char *format, va_list args) {
    char ch;
    char *start = dest;
    const char *end = dest + bufSize - 1;
    
    while ((ch = *format) != 0 && dest < end) {
        if (ch != '%') {
            *dest++ = ch;
            format += 1;
        } else {
            s_PrintParams params;
            char numBuf[s_numBufSize + 1];
            int intLevel = 0;
            int contFlag;

            params.justifyLeft = 0;
            params.forceSign = 0;
            params.forceType = 0;
            params.pad = ' ';
            params.precision = -1;
            params.caseLevel = 0;
            params.width = 0;
            params.charset = charset;
            params.radix = 10;
            
            /* Read flags for printing */
            contFlag = 1;
            do {
                format += 1;
                switch (*format) {
                    case '0':
                        params.pad = '0';
                        break;
                    case '-':
                        params.justifyLeft = 1;
                        break;
                    case '+':
                        params.forceSign = 1;
                        break;
                    case '#':
                        params.forceType = 1;
                        break;
                    default:
                        contFlag = 0;
                }
            } while (contFlag != 0);
            
            if (*format >= '0' && *format <= '9') {
                params.width = 0;
                do {
                    params.width *= 10;
                    params.width += *format - '0';
                    format += 1;
                } while (*format >= '0' && *format <= '9');
            }
            if (*format == '.') {
                format += 1;
                params.precision = 0;
                do {
                    params.precision *= 10;
                    params.precision += *format - '0';
                    format += 1;
                } while (*format >= '0' && *format <= '9');
            }
            
            do {
                contFlag = 0;
                switch(*format) {
                    case '%': /* %, as-is */
                        *dest++ = '%';
                        break;
                    case 'c': /* char */
                        *dest++ = (char)va_arg(args, int);
                        break;
                    case 's': /* string */
                        if (intLevel == 0) {
                            dest += s_printString(dest, end, &params, va_arg(args, const char *));
                        } else {
                            dest += s_printWString(dest, end, &params, va_arg(args, const wchar_t *));
                        }
                    case 'S':
                        dest += s_printWString(dest, end, &params, va_arg(args, const wchar_t *));
                        break;
                    
                    case 'h': /* prefix for short */
                        contFlag = 1; /* short becomes int */
                        format += 1;
                        break;
                    case 'l': /* long prefix, can stack */
                        if (intLevel < 2) {
                            intLevel += 1;
                        }
                        contFlag = 1;
                        format += 1;
                        break;
                    case 'I': /* I64 prefix */
                        if (PL_Text_Strncmp(format, "I64", 3) == 0) {
                            format += 3;
                            intLevel = 2;
                            contFlag = 1;
                        }
                        break;
                    
                    case 'i': /* integer */
                    case 'd': /* also integer */
                        switch(intLevel) {
                            case 0: /* int */
                                s_printLong(numBuf, numBuf + s_numBufSize, &params, (long)va_arg(args, int));
                                dest += s_printString(dest, end, &params, numBuf);
                                break;
                            case 1: /* long */
                                s_printLong(numBuf, numBuf + s_numBufSize, &params, va_arg(args, long));
                                dest += s_printString(dest, end, &params, numBuf);
                                break;
                            case 2: /* long long */
                                s_printSint64(numBuf, numBuf + s_numBufSize, &params, va_arg(args, long long));
                                dest += s_printString(dest, end, &params, numBuf);
                                break;
                        }
                        break;
                        
                    /* Hex, unsigned, octal, etc printing modes, all fall through */
                    case 'p': /* pointer */
                        intLevel = 1; /* pointers are always longs */
                    case 'x': /* hexadecimal */
                        params.caseLevel = -1;
                    case 'X': /* capitalized hex */
                        if (params.caseLevel == 0) {
                            params.caseLevel = 1;
                        }
                        if (params.radix == 10) {
                            params.radix = 16;
                        }
                    case 'o': /* octal */
                        if (params.radix == 10) {
                            params.radix = 8;
                        }
                    case 'u': /* unsigned */
                        params.pad = '0';
                        switch(intLevel) {
                            case 0: /* int */
                                s_printUnsignedLong(numBuf, numBuf + s_numBufSize, &params, (unsigned long)va_arg(args, unsigned int));
                                dest += s_printString(dest, end, &params, numBuf);
                                break;
                            case 1: /* long */
                                s_printUnsignedLong(numBuf, numBuf + s_numBufSize, &params, va_arg(args, unsigned long));
                                dest += s_printString(dest, end, &params, numBuf);
                                break;
                            case 2: /* long long */
                                s_printUint64(numBuf, numBuf + s_numBufSize, &params, (uint64_t)va_arg(args, unsigned long long));
                                dest += s_printString(dest, end, &params, numBuf);
                                break;
                        }
                        break;
                    case 'f': /* float */
                        s_printDouble(numBuf, numBuf + s_numBufSize, &params, va_arg(args, double));
                        dest += s_printString(dest, end, &params, numBuf);
                        break;
                    default:
                        break;
                }
            } while (contFlag != 0);
        }
    }
    
    *dest = 0;
    
    return dest - start;
}

int PL_Text_Wvsnprintf(wchar_t *dest, int bufSize, int charset,
                       const wchar_t *format, va_list args)
{
    wchar_t ch;
    wchar_t *start = dest;
    const wchar_t *end = dest + bufSize - 1;
    
    while ((ch = *format) != 0 && dest < end) {
        if (ch != '%') {
            *dest++ = ch;
            format += 1;
        } else {
            s_PrintParams params;
            char numBuf[s_numBufSize + 1];
            int intLevel = 0;
            int contFlag;

            params.justifyLeft = 0;
            params.forceSign = 0;
            params.forceType = 0;
            params.pad = ' ';
            params.precision = -1;
            params.caseLevel = 0;
            params.width = 0;
            params.charset = charset;
            params.radix = 10;
            
            /* Read flags for printing */
            contFlag = 1;
            do {
                format += 1;
                switch (*format) {
                    case '0':
                        params.pad = '0';
                        break;
                    case '-':
                        params.justifyLeft = 1;
                        break;
                    case '+':
                        params.forceSign = 1;
                        break;
                    case '#':
                        params.forceType = 1;
                        break;
                    default:
                        contFlag = 0;
                }
            } while (contFlag != 0);
            
            if (*format >= '0' && *format <= '9') {
                params.width = 0;
                do {
                    params.width *= 10;
                    params.width += *format - '0';
                    format += 1;
                } while (*format >= '0' && *format <= '9');
            }
            if (*format == '.') {
                format += 1;
                params.precision = 0;
                do {
                    params.precision *= 10;
                    params.precision += *format - '0';
                    format += 1;
                } while (*format >= '0' && *format <= '9');
            }
            
            do {
                contFlag = 0;
                switch(*format) {
                    case '%': /* %, as-is */
                        *dest++ = '%';
                        break;
                    case 'c': /* char */
                        *dest++ = (char)va_arg(args, int);
                        break;
                    case 's': /* string */
                        if (intLevel == 0) {
                            dest += s_printStringW(dest, end, &params, va_arg(args, const char *));
                        } else {
                            dest += s_printWStringW(dest, end, &params, va_arg(args, const wchar_t *));
                        }
                    case 'S':
                        dest += s_printWStringW(dest, end, &params, va_arg(args, const wchar_t *));
                        break;
                    
                    case 'h': /* prefix for short */
                        contFlag = 1; /* short becomes int */
                        format += 1;
                        break;
                    case 'l': /* long prefix, can stack */
                        if (intLevel < 2) {
                            intLevel += 1;
                        }
                        break;
                    case 'I': /* I64 prefix */
                        if (PL_Text_StrncmpW(format, L"I64", 3) == 0) {
                            format += 3;
                            intLevel = 2;
                        }
                        break;
                    
                    case 'i': /* integer */
                    case 'd': /* also integer */
                        switch(intLevel) {
                            case 0: /* int */
                                s_printLong(numBuf, numBuf + s_numBufSize, &params, (long)va_arg(args, int));
                                dest += s_printStringW(dest, end, &params, numBuf);
                                break;
                            case 1: /* long */
                                s_printLong(numBuf, numBuf + s_numBufSize, &params, va_arg(args, long));
                                dest += s_printStringW(dest, end, &params, numBuf);
                                break;
                            case 2: /* long long */
                                s_printSint64(numBuf, numBuf + s_numBufSize, &params, va_arg(args, long long));
                                dest += s_printStringW(dest, end, &params, numBuf);
                                break;
                        }
                        break;
                        
                    /* Hex, unsigned, octal, etc printing modes, all fall through */
                    case 'p': /* pointer */
                        intLevel = 1; /* pointers are always longs */
                    case 'x': /* hexadecimal */
                        params.caseLevel = -1;
                    case 'X': /* capitalized hex */
                        if (params.caseLevel == 0) {
                            params.caseLevel = 1;
                        }
                        if (params.radix == 10) {
                            params.radix = 16;
                        }
                    case 'o': /* octal */
                        if (params.radix == 10) {
                            params.radix = 8;
                        }
                    case 'u': /* unsigned */
                        params.pad = '0';
                        switch(intLevel) {
                            case 0: /* int */
                                s_printUnsignedLong(numBuf, numBuf + s_numBufSize, &params, (unsigned long)va_arg(args, unsigned int));
                                dest += s_printStringW(dest, end, &params, numBuf);
                                break;
                            case 1: /* long */
                                s_printUnsignedLong(numBuf, numBuf + s_numBufSize, &params, va_arg(args, unsigned long));
                                dest += s_printStringW(dest, end, &params, numBuf);
                                break;
                            case 2: /* long long */
                                s_printUint64(numBuf, numBuf + s_numBufSize, &params, (uint64_t)va_arg(args, unsigned long long));
                                dest += s_printStringW(dest, end, &params, numBuf);
                                break;
                        }
                        break;
                    case 'f': /* float */
                        s_printDouble(numBuf, numBuf + s_numBufSize, &params, va_arg(args, double));
                        dest += s_printStringW(dest, end, &params, numBuf);
                        break;
                    default:
                        break;
                }
            } while (contFlag != 0);
        }
    }
    
    *dest = 0;
    
    return dest - start;
}
