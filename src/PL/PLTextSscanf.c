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

static int s_isSpace(unsigned int c) {
    return (c == ' ') || (c == '\n') || (c == '\r') || (c == '\t') || (c == '\f') || (c == '\v');
}

static int s_scanUnsignedLong(const char *str, int charset, int radix, unsigned long *value) {
    const char *start = str;
    int negFlag = 0;
    unsigned long v = 0;
    unsigned int c;
    
    /* Test for 0x */
    if (radix == 16) {
        const char *tmp = str;
        c = PL_Text_ReadChar(&str, charset);
        if (c == '0' && radix == 16) {
            c = PL_Text_ReadChar(&str, charset);
            if (c == 'x' || c == 'X') {
                tmp = str;
            }
        }
        str = tmp;
    }
     
    /* Break down the numbers. */
    while (*str) {
        const char *thisStart = str;
        int cv = 0;

        c = PL_Text_ReadChar(&str, charset);
        if (c >= '0' && c <= '9') {
            cv = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            cv = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            cv = c - 'A' + 10;
        } else {
            str = thisStart;
            break;
        }
        if (cv >= 0 && cv < radix) {
            v = (v * radix) + cv;
        } else {
            str = thisStart;
            break;
        }
    }
    
    if (negFlag) {
        *value = (unsigned long)-(long)v;
    } else {
        *value = v;
    }
    
    return str - start;
}

static int s_scanLong(const char *str, int charset, int radix, long *value) {
    const char *start = str;
    unsigned int c = PL_Text_ReadChar(&str, charset);
    int negFlag = 0;
    unsigned long uv;
    
    if (c == '-') {
        negFlag = 1;
    } else {
        str = start;
    }
    
    str += s_scanUnsignedLong(str, charset, radix, &uv);
    
    if (negFlag) {
        *value = -(long)uv;
    } else {
        *value = (long)uv;
    }
    
    return str - start;
}

static int s_scanUint64(const char *str, int charset, int radix, uint64_t *value) {
    const char *start = str;
    int negFlag = 0;
    uint64_t v = 0;
    unsigned int c;
    
    /* Test for 0x */
    if (radix == 16) {
        const char *tmp = str;
        c = PL_Text_ReadChar(&str, charset);
        if (c == '0' && radix == 16) {
            c = PL_Text_ReadChar(&str, charset);
            if (c == 'x' || c == 'X') {
                tmp = str;
            }
        }
        str = tmp;
    }
     
    /* Break down the numbers. */
    while (*str) {
        const char *thisStart = str;
        int cv = 0;

        c = PL_Text_ReadChar(&str, charset);
        if (c >= '0' && c <= '9') {
            cv = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            cv = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            cv = c - 'A' + 10;
        } else {
            str = thisStart;
            break;
        }
        if (cv >= 0 && cv < radix) {
            v = (v * radix) + cv;
        } else {
            str = thisStart;
            break;
        }
    }
    
    if (negFlag) {
        *value = (uint64_t)-(int64_t)v;
    } else {
        *value = v;
    }
    
    return str - start;
}

static int s_scanSint64(const char *str, int charset, int radix, int64_t *value) {
    const char *start = str;
    unsigned int c = PL_Text_ReadChar(&str, charset);
    int negFlag = 0;
    uint64_t uv;
    
    if (c == '-') {
        negFlag = 1;
    } else {
        str = start;
    }
    
    str += s_scanUint64(str, charset, radix, &uv);
    
    if (negFlag) {
        *value = -(int64_t)uv;
    } else {
        *value = (int64_t)uv;
    }
    
    return str - start;
}

static int s_scanDouble(const char *str, int charset, int radix, double *value) {
    const char *start = str;
    const char *tmp;
    unsigned int c = PL_Text_ReadChar(&str, charset);
    double v = 0;
    int negFlag = 0;
    
    if (c == '-') {
        negFlag = 1;
    } else {
        str = start;
    }
    
    while (*str) {
        int cv = 0;

        tmp = str;
        c = PL_Text_ReadChar(&str, charset);
        if (c >= '0' && c <= '9') {
            cv = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            cv = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            cv = c - 'A' + 10;
        } else {
            str = tmp;
            break;
        }
        
        if (cv >= radix) {
            str = tmp;
            break;
        }
        
        v = (v * radix) + cv;
    }
    
    tmp = str;
    c = PL_Text_ReadChar(&str, charset);
    if (c == '.') {
        double divisor = 1.0 / radix;
        double d = divisor;
        while (*str) {
            int cv = 0;

            tmp = str;
            c = PL_Text_ReadChar(&str, charset);
            if (c >= '0' && c <= '9') {
                cv = c - '0';
            } else if (c >= 'a' && c <= 'z') {
                cv = c - 'a' + 10;
            } else if (c >= 'A' && c <= 'Z') {
                cv = c - 'A' + 10;
            } else {
                str = tmp;
                break;
            }
            
            if (cv >= radix) {
                str = tmp;
                break;
            }
            
            v += cv * d;
            d *= divisor;
        }
    }
    
    if (negFlag) {
        v = -v;
    }
    
    *value = v;
    
    return str - start;
}

int PL_Text_Vsscanf(const char *str, int charset, const char *format, va_list args) {
    int count = 0;
    char ch;
    
    while ((ch = *format) != 0) {
        if (ch == ' ') {
            while (s_isSpace(*str) != 0) {
                str += 1;
            }
            format += 1;
            continue;
        } else if (ch == '%') {
            int radix = 10;
            int intLevel = 1;
            long size;
            int contFlag;
            int ignoreFlag = 0;
            
            format += 1;
            if (*format == '%') {
                if (*str == '%') {
                    str += 1;
                    format += 1;
                }
                continue;
            }
            if (*format == '*') {
                ignoreFlag = 1;
                format += 1;
            }
            
            format += s_scanLong(format, charset, 10, &size);
            if (size <= 0) {
                size = 1 << 20; /* Just blow it all up */
            }
            
            while (s_isSpace(*str) != 0) {
                PL_Text_ReadChar(&str, charset);
            }
            
            do {
                contFlag = 0;
                switch(*format) {
                    case '*':
                        ignoreFlag = 1;
                        contFlag = 1;
                        break;
                    case 'h':
                        if (intLevel > 0) {
                            intLevel -= 1;
                        }
                        contFlag = 1;
                        break;
                    case 'l':
                        if (intLevel < 3) {
                            intLevel += 1;
                        }
                        contFlag = 1;
                        break;
                    case 'I':
                        if (PL_Text_Strncmp(format, "I64", 3) == 0) {
                            intLevel = 3;
                            format += 2;
                            contFlag = 1;
                        }
                        break;
                    
                    case 'i':
                        {
                            /* for %i, extract radix automatically */
                            const char *tmp = str;
                            if (*tmp == '-') {
                                tmp += 1;
                            }
                            if (*tmp == '0') {
                                if (tmp[1] == 'x' || tmp[1] == 'X') {
                                    radix = 16;
                                } else {
                                    radix = 8;
                                }
                            }
                        }
                        /* Fall through */
                    case 'd':
                        if (intLevel == 3) {
                            int64_t value;
                            str += s_scanSint64(str, charset, radix, &value);
                            if (ignoreFlag == 0) {
                                long long *target = va_arg(args, long long *);
                                *target = value;
                                count += 1;
                            }
                        } else {
                            long value;
                            str += s_scanLong(str, charset, radix, &value);
                            if (ignoreFlag == 0) {
                                if (intLevel == 2) {
                                    long *target = va_arg(args, long *);
                                    *target = value;
                                } else if (intLevel == 1) {
                                    int *target = va_arg(args, int *);
                                    *target = (int)value;
                                } else if (intLevel == 0) {
                                    short *target = va_arg(args, short *);
                                    *target = (short)value;
                                }
                                count += 1;
                            }
                        }
                        break;
                    case 'o':
                        if (radix == 10) {
                            radix = 8;
                        }
                        /* Fall through */
                    case 'x':
                    case 'X':
                        if (radix == 10) {
                            radix = 16;
                        }
                        /* Fall through */
                    case 'u':
                        if (intLevel == 3) {
                            uint64_t value;
                            str += s_scanUint64(str, charset, radix, &value);
                            if (ignoreFlag == 0) {
                                unsigned long long *target = va_arg(args, unsigned long long *);
                                *target = value;
                                count += 1;
                            }
                        } else {
                            unsigned long value;
                            str += s_scanUnsignedLong(str, charset, radix, &value);
                            if (ignoreFlag == 0) {
                                if (intLevel == 2) {
                                    unsigned long *target = va_arg(args, unsigned long *);
                                    *target = value;
                                } else if (intLevel == 1) {
                                    unsigned int *target = va_arg(args, unsigned int *);
                                    *target = (int)value;
                                } else if (intLevel == 0) {
                                    unsigned short *target = va_arg(args, unsigned short *);
                                    *target = (short)value;
                                }
                                count += 1;
                            }
                        }
                        break;
                    case 'p':
                        {
                            unsigned long value;
                            str += s_scanUnsignedLong(str, charset, 16, &value);
                            if (ignoreFlag == 0) {
                                void **target = va_arg(args, void **);
                                *target = (void *)value;
                                count += 1;
                            }
                        }
                        break;
                    
                    case 'f':
                        {
                            double value;
                            str += s_scanDouble(str, charset, radix, &value);
                            if (ignoreFlag == 0) {
                                if (intLevel > 1) {
                                    double *target = va_arg(args, double *);
                                    *target = value;
                                } else {
                                    float *target = va_arg(args, float *);
                                    *target = (float)value;
                                }
                                count += 1;
                            }
                        }
                        
                        break;
                                        
                    case 'c':
                        if (size <= 0) {
                            size = 1;
                        }
                        if (ignoreFlag != 0) {
                            while (*str != 0 && size > 0) {
                                str += 1;
                                size -= 1;
                            }
                        } else {
                            char *target = va_arg(args, char *);
                            char *end = target + size;
                            while (*str != 0) {
                                unsigned int c = PL_Text_ReadChar(&str, charset);
                                target += PL_Text_WriteChar(target, c, end - target, charset);
                                if (target == end) {
                                    break;
                                }
                            }
                            count += 1;
                        }
                        break;
                    case 's':
                        if (ignoreFlag == 0 && intLevel <= 1) {
                            char *target = va_arg(args, char *);
                            char *end = target + size;
                            while (*str != 0 && s_isSpace(*str) == 0) {
                                unsigned int c = PL_Text_ReadChar(&str, charset);
                                target += PL_Text_WriteChar(target, c, end - target, charset);
                                if (target == end) {
                                    break;
                                }
                            }
                            *target = '\0';
                            count += 1;
                            break;
                        }
                        /* Fall through to widechar string / ignore logic */
                    case 'S':
                        if (ignoreFlag != 0) {
                            while (*str != 0 && s_isSpace(*str) == 0) {
                                str += 1;
                                if (size > 0) {
                                    size -= 1;
                                    if (size == 0) {
                                        break;
                                    }
                                }
                            }
                        } else {
                            wchar_t *target = va_arg(args, wchar_t *);
                            wchar_t *end = target + size;
                            while (*str != 0 && s_isSpace(*str) == 0) {
                                unsigned int c = PL_Text_ReadChar(&str, charset);
                                *target++ = c;
                                if (target == end) {
                                    break;
                                }
                            }
                            *target = '\0';
                            count += 1;
                        }
                        break;
                    default:
                        break;
                }
                format += 1;
            } while (contFlag != 0);
        }
    }
    
    return count;
}

int PL_Text_Sscanf(const char *str, int charset, const char *format, ...) {
    va_list args;
    int retval;
    
    va_start(args, format);
    retval = PL_Text_Vsscanf(str, charset, format, args);
    va_end(args);
    
    return retval;
}
int PL_Text_Wvsscanf(const wchar_t *str, int charset, const wchar_t *format, va_list args) {
    char strbuf[4096];
    char formatbuf[4096];
    
    PL_Text_WideCharToString(strbuf, charset, str, 4096);
    PL_Text_WideCharToString(formatbuf, charset, format, 4096);
    
    return PL_Text_Vsscanf(strbuf, charset, formatbuf, args);
}

int PL_Text_Wsscanf(const wchar_t *str, int charset, const wchar_t *format, ...) {
    va_list args;
    int retval;
    
    va_start(args, format);
    retval = PL_Text_Wvsscanf(str, charset, format, args);
    va_end(args);
    
    return retval;
}

