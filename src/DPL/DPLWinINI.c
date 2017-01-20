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

#include "DxPortLib_c.h"

#include "DPL/DPLInternal.h"

#include "PL/PLInternal.h"

static const uint32_t s_wininiID = 0x494e4970; /* pINI */

typedef struct s_Entry_t {
    char *name;
    char *value;
    
    struct s_Entry_t *next;
} s_Entry_t;

typedef struct s_Section_t {
    char *name;
    
    s_Entry_t *entries;
    
    struct s_Section_t *next;
} s_Section_t;

typedef struct s_WinINI_t {
    uint32_t id;
    
    s_Section_t *sections;
} s_WinINI_t;

static s_WinINI_t *s_GetFromHandle(int handle) {
    s_WinINI_t *ini = (s_WinINI_t *)PL_Handle_GetData(handle, DXHANDLE_MISC);
    if (ini != NULL) {
        if (ini->id != s_wininiID) {
            return NULL;
        }
    }
    return ini;
}

static void s_DeleteEntry(s_Entry_t *entry, s_Section_t *section, s_Entry_t *prevEntry) {
    /* Remove from list */
    if (prevEntry == NULL && section->entries != entry) {
        prevEntry = section->entries;
        while (prevEntry != NULL && prevEntry->next != entry) {
            prevEntry = prevEntry->next;
        }
    }
    
    if (prevEntry != NULL) {
        prevEntry->next = entry->next;
    } else if (section->entries == entry) {
        section->entries = entry->next;
    } else {
        /* ??? BUG */
    }
    
    /* Delete. */
    if (entry->value != NULL) {
        DXFREE(entry->value);
    }
    DXFREE(entry->name);
    DXFREE(entry);
}

static void s_DeleteSection(s_Section_t *section, s_WinINI_t *ini, s_Section_t *prevSection) {
    /* Remove from list */
    if (prevSection == NULL && ini->sections != section) {
        prevSection = ini->sections;
        while (prevSection != NULL && prevSection->next != section) {
            prevSection = prevSection->next;
        }
    }
    
    if (prevSection != NULL) {
        prevSection->next = section->next;
    } else if (ini->sections == section) {
        ini->sections = section->next;
    } else {
        /* ??? BUG */
    }
    
    /* Delete. */
    while (section->entries != NULL) {
        s_DeleteEntry(section->entries, section, NULL);
    }
    DXFREE(section->name);
    DXFREE(section);
}

static void s_SetEntryValue(s_Entry_t *entry, const char *value) {
    if (entry->value != NULL) {
        DXFREE(entry->value);
        entry->value = NULL;
    }
    if (value != NULL) {
        entry->value = PL_Text_Strdup(value);
    }
}

static s_Entry_t *s_FindEntry(s_Section_t *section, const char *name, int addIfNotFound, s_Entry_t **pPrevEntry) {
    s_Entry_t *e = section->entries;
    s_Entry_t *prev = NULL;
    while (e != NULL) {
        if (PL_Text_Strcmp(e->name, name) == 0) {
            if (pPrevEntry != NULL) {
                *pPrevEntry = prev;
            }
            return e;
        }
        
        prev = e;
        e = e->next;
    }
    
    if (addIfNotFound == 0) {
        return NULL;
    }
    
    e = (s_Entry_t *)DXALLOC(sizeof(s_Entry_t));
    e->name = PL_Text_Strdup(name);
    e->value = NULL;
    e->next = section->entries;
    section->entries = e;
    
    return e;
}

static s_Section_t *s_FindSection(s_WinINI_t *ini, const char *sectionName, int addIfNotFound, s_Section_t **pPrevSection) {
    s_Section_t *s = ini->sections;
    s_Section_t *prev = NULL;
    while (s != NULL) {
        if (PL_Text_Strcmp(s->name, sectionName) == 0) {
            if (pPrevSection != NULL) {
                *pPrevSection = prev;
            }
            return s;
        }
        
        prev = s;
        s = s->next;
    }
    
    if (addIfNotFound == 0) {
        return NULL;
    }
    
    s = (s_Section_t *)DXALLOC(sizeof(s_Section_t));
    s->name = PL_Text_Strdup(sectionName);
    s->next = ini->sections;
    s->entries = NULL;
    ini->sections = s;
    
    if (pPrevSection != NULL) {
        *pPrevSection = NULL;
    }
    
    return s;
}

static int s_ReadText(char *dest, int maxLen, const char **pText, int encoding, unsigned int haltChar) {
    unsigned int ch;
    const char *text = *pText;
    const char *last = text;
    char *start = dest;
    char *end = dest + maxLen - 1;
    
    while ((ch = PL_Text_ReadChar(&text, encoding)) != 0) {
        if (ch == haltChar) {
            break;
        }
        if (ch == '\\') {
            ch = PL_Text_ReadChar(&text, encoding);
            if (ch == 0) {
                break;
            }
        }
        dest += PL_Text_WriteChar(dest, ch, end - dest, encoding);
        
        last = text;
    }
    
    *dest = '\0';
    
    *pText = last;
    
    return dest - start;
}

static int s_ParseText(s_WinINI_t *ini, const char *data, int fileEncoding) {
    int localEncoding = g_dplDefaultEncoding;
    s_Section_t *section = NULL;
    
    if (fileEncoding <= DPL_CHARSET_DEFAULT) {
        fileEncoding = g_dplDefaultEncoding;
    }
    
    /* Text parsing is ugly no matter what, really. Maybe I should regex it. */
    while (*data) {
        char textBuf[4096];
        const char *start = textBuf;
        int len = PL_Text_ReadLine(textBuf, 4096, &data, localEncoding, fileEncoding);
        const char *t = start;
        unsigned int ch;
        
        if (len == 0) {
            continue;
        }
        
        ch = PL_Text_ReadChar(&t, localEncoding);
        
        while (ch == ' ') {
            start = t;
            ch = PL_Text_ReadChar(&t, localEncoding);
        }
        
        if (ch == '#' || ch == ';') {
            continue;
        }
        
        if (ch == '[') {
            char sectionName[4096];
            int len = s_ReadText(sectionName, 4096, &t, localEncoding, ']');
            if (len != 0) {
                section = s_FindSection(ini, sectionName, DPLTRUE, NULL);
            }
        } else if (section != NULL) {
            char entryName[4096];
            char entryValue[4096];
            int len;
            unsigned int haltChar = '=';
            
            if (ch == '\"') {
                haltChar = '\"';
            } else {
                t = start;
            }
            
            len = s_ReadText(entryName, 4096, &t, localEncoding, haltChar);
            if (len != 0 && *t != 0) {
                while ((ch = PL_Text_ReadChar(&t, localEncoding)) != 0) {
                    if (ch == '=') {
                        break;
                    }
                }
                
                do {
                    start = t;
                    ch = PL_Text_ReadChar(&t, localEncoding);
                } while (ch == ' ');
                
                haltChar = ';';
                if (ch == '\"') {
                    haltChar = '\"';
                } else {
                    t = start;
                }
                
                len = s_ReadText(entryValue, 4096, &t, localEncoding, haltChar);
                if (len != 0) {
                    s_Entry_t *entry = s_FindEntry(section, entryName, DPLTRUE, NULL);
                    s_SetEntryValue(entry, entryValue);
                }
            }
        }
    }
    
    return 0;
}

int DPL_WinINI_Create() {
    s_WinINI_t *ini;
    int handle = PL_Handle_AcquireID(DXHANDLE_MISC);
    if (handle <= 0) {
        return -1;
    }
    
    ini = (s_WinINI_t *)PL_Handle_AllocateData(handle, sizeof(s_WinINI_t));
    ini->id = s_wininiID;
    ini->sections = NULL;
    
    return handle;
}

int DPL_WinINI_CreateAndReadFile(const char *filename, int fileEncoding) {
    int handle = DPL_WinINI_Create();
    if (handle <= 0) {
        return 0;
    }
    
    DPL_WinINI_ReadFile(handle, filename, fileEncoding);
    
    return handle;
}

int DPL_WinINI_ReadFile(int handle, const char *filename, int fileEncoding) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    int fileHandle;
    int64_t size;
    
    if (ini == NULL) {
        return -1;
    }
    
    fileHandle = PL_File_OpenRead(filename);
    if (fileHandle <= 0) {
        return -1;
    }
    
    size = PL_File_GetSize(fileHandle);
    if (size < (1024*1024)) { /* 1MB limit */
        char *data = DXALLOC((size_t)(size + 1));
        
        PL_File_Read(fileHandle, data, size);
        data[size] = 0;
        
        s_ParseText(ini, data, fileEncoding);
        
        DXFREE(data);
    }
    
    PL_File_Close(fileHandle);
    
    return 0;
}

int DPL_WinINI_WriteFile(int handle, const char *filename, int fileEncoding) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    char textBuf[4096];
    int fileHandle;
    const s_Section_t *s;
    
    if (ini == NULL) {
        return -1;
    }
    
    fileHandle = PL_File_OpenWrite(filename);
    if (fileHandle <= 0) {
        return -1;
    }
    
    if (fileEncoding <= DPL_CHARSET_DEFAULT) {
        fileEncoding = g_dplDefaultEncoding;
    }
    
    for (s = ini->sections; s != NULL; s = s->next) {
        const s_Entry_t *e;
        int length = PL_Text_Snprintf(textBuf, 4096, fileEncoding, "[%s]\r\n", s->name);
        PL_File_Write(fileHandle, textBuf, length);
        
        for (e = s->entries; e != NULL; e = e->next) {
            if (e->value == NULL) {
                continue;
            }
            
            length = PL_Text_Snprintf(textBuf, 4096, fileEncoding, "\"%s\"=\"%s\"\r\n", e->name, e->value);
            PL_File_Write(fileHandle, textBuf, length);
        }
        
        length = PL_Text_Strncpy(textBuf, "\r\n", 4096);
        PL_File_Write(fileHandle, textBuf, length);
    }
    
    PL_File_Close(fileHandle);
    
    return 0;
}

int DPL_WinINI_Release(int handle) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    if (ini == NULL) {
        return -1;
    }
    
    DPL_WinINI_Clear(handle);
    
    PL_Handle_ReleaseID(handle, DPLTRUE);
    
    return 0;
}

int DPL_WinINI_Clear(int handle) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    if (ini == NULL) {
        return -1;
    }
    
    while (ini->sections != NULL) {
        s_DeleteSection(ini->sections, ini, NULL);
    }
    
    return 0;
}

int DPL_WinINI_GetInt(int handle, const char *sectionName, const char *name, int defaultValue) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    s_Section_t *section;
    
    if (ini == NULL) {
        return defaultValue;
    }
    
    section = s_FindSection(ini, sectionName, DPLFALSE, NULL);
    if (section != NULL) {
        s_Entry_t *entry = s_FindEntry(section, name, DPLFALSE, NULL);
        if (entry != NULL && entry->value != NULL) {
            int value;
            if (PL_Text_Sscanf(entry->value, g_dplDefaultEncoding, "%d", &value) == 1) {
                return value;
            }
        }
    }
    
    return defaultValue;
}

const char *DPL_WinINI_GetString(int handle, const char *sectionName, const char *name, const char *defaultValue) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    s_Section_t *section;
    
    if (ini == NULL) {
        return defaultValue;
    }
    
    section = s_FindSection(ini, sectionName, DPLFALSE, NULL);
    if (section != NULL) {
        s_Entry_t *entry = s_FindEntry(section, name, DPLFALSE, NULL);
        if (entry != NULL && entry->value != NULL) {
            return entry->value;
        }
    }
    
    return defaultValue;
}

int DPL_WinINI_SetInt(int handle, const char *sectionName, const char *name, int value) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    s_Section_t *section;
    s_Entry_t *entry;
    char buf[128];
    
    if (ini == NULL) {
        return -1;
    }
    
    section = s_FindSection(ini, sectionName, DPLTRUE, NULL);
    entry = s_FindEntry(section, name, DPLTRUE, NULL);
    PL_Text_Snprintf(buf, 128, g_dplDefaultEncoding, "%d", value);
    s_SetEntryValue(entry, buf);
    
    return 0;
}

int DPL_WinINI_SetString(int handle, const char *sectionName, const char *name, const char *value) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    s_Section_t *section;
    s_Entry_t *entry;
    
    if (ini == NULL) {
        return -1;
    }
    
    section = s_FindSection(ini, sectionName, DPLTRUE, NULL);
    entry = s_FindEntry(section, name, DPLTRUE, NULL);
    s_SetEntryValue(entry, value);
    
    return 0;
}

int DPL_WinINI_DeleteValue(int handle, const char *sectionName, const char *name) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    s_Section_t *prevS = NULL;
    s_Section_t *s = NULL;
    
    if (ini == NULL) {
        return -1;
    }
    
    s = s_FindSection(ini, sectionName, DPLFALSE, &prevS);
    if (s != NULL) {
        s_Entry_t *prevE = NULL;
        s_Entry_t *e = s_FindEntry(s, name, DPLFALSE, &prevE);
        if (e != NULL) {
            s_DeleteEntry(e, s, prevE);
            if (s->entries == NULL) {
                s_DeleteSection(s, ini, prevS);
            }
        }
    }
    
    return 0;
}

int DPL_WinINI_DeleteSection(int handle, const char *sectionName) {
    s_WinINI_t *ini = s_GetFromHandle(handle);
    s_Section_t *prevS = NULL;
    s_Section_t *s = NULL;
    
    if (ini == NULL) {
        return -1;
    }
    
    s = s_FindSection(ini, sectionName, DPLFALSE, &prevS);
    if (s != NULL) {
        s_DeleteSection(s, ini, prevS);
    }
    
    return 0;
}
