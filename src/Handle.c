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

#include "DxInternal.h"

/* DxLib keeps a listing of assigned handles, which are
 * shared and reassigned as needed.
 *
 * Similar to it, we use a doubly-linked list, except
 * it is statically allocated to reduce page separation.
 */

typedef struct HandleData {
    void *data;
    
    int *deleteFlag;
    
    HandleType handleType;
    
    int prevID;
    int nextID;
} HandleData;

static int s_initialized = DXFALSE;
static HandleData *s_handleTable = NULL;
static int s_handleCount = 0;
static int s_handleLists[DXHANDLE_END];

static void s_Enlarge() {
    int n = s_handleCount + 512;
    int first = s_handleCount;
    int last;
    int i;
    HandleData *handle;
    
    if (s_handleTable == NULL) {
        s_handleTable = DXALLOC((size_t)n * sizeof(HandleData));
    } else {
        s_handleTable = DXREALLOC(s_handleTable, (size_t)n * sizeof(HandleData));
    }
    s_handleCount = n;
    
    last = -1;
    for (i = n - 1; i >= first; --i) {
        handle = &s_handleTable[i];
        handle->data = NULL;
        handle->handleType = DXHANDLE_NONE;
        handle->nextID = last;
        handle->prevID = i - 1;
        last = i;
    }
    
    handle->prevID = -1;
    
    s_handleLists[DXHANDLE_NONE] = first;
}

static void s_Unlink(int id) {
    HandleData *handle = &s_handleTable[id];
    
    if (handle->prevID >= 0) {
        s_handleTable[handle->prevID].nextID = handle->nextID;
    } else if (s_handleLists[handle->handleType] == id) {
        s_handleLists[handle->handleType] = handle->nextID;
    }
    
    if (handle->nextID >= 0) {
        s_handleTable[handle->nextID].prevID = handle->prevID;
    }
    
    handle->prevID = -1;
    handle->nextID = -1;
}

static void s_Link(int id) {
    HandleData *handle = &s_handleTable[id];
    
    handle->nextID = s_handleLists[handle->handleType];
    if (handle->nextID >= 0) {
        s_handleTable[handle->nextID].prevID = id;
    }
    
    s_handleLists[handle->handleType] = id;
}

void PL_Handle_Init() {
    int i;
    
    if (s_initialized == DXTRUE) {
        return;
    }
    
    s_handleTable = NULL;
    s_handleCount = 0;
    for (i = 0; i < DXHANDLE_END; ++i) {
        s_handleLists[i] = -1;
    }
    
    s_initialized = DXTRUE;
}

void PL_Handle_End() {
    int i;
    
    if (s_initialized == DXFALSE) {
        return;
    }
    
    if (s_handleTable != NULL) {
        DXFREE(s_handleTable);
    }
    
    s_handleTable = NULL;
    s_handleCount = 0;
    for (i = 0; i < DXHANDLE_END; ++i) {
        s_handleLists[i] = -1;
    }
    
    s_initialized = DXFALSE;
}

int PL_Handle_AcquireID(int handleType) {
    HandleData *handle;
    int freeID;
    
    if (handleType <= DXHANDLE_NONE || handleType >= DXHANDLE_END) {
        return -1;
    }
    
    freeID = s_handleLists[DXHANDLE_NONE];
    if (freeID == -1) {
        s_Enlarge();
        freeID = s_handleLists[DXHANDLE_NONE];
    }
    
    s_Unlink(freeID);
    
    handle = &s_handleTable[freeID];
    handle->handleType = handleType;
    handle->deleteFlag = 0;
    
    s_Link(freeID);
    
    return freeID;
}

void PL_Handle_ReleaseID(int handleID, int freeData) {
    HandleData *handle;
    if (handleID < 0 || handleID >= s_handleCount) {
        return;
    }
    
    handle = &s_handleTable[handleID];
    
    if (freeData != DXFALSE && handle->data != NULL) {
        DXFREE(handle->data);
        handle->data = NULL;
    }
    
    if (handle->deleteFlag != 0) {
        *(handle->deleteFlag) = -1;
        handle->deleteFlag = 0;
    }
    
    s_Unlink(handleID);
    
    handle->handleType = DXHANDLE_NONE;
    
    s_Link(handleID);
}

void *PL_Handle_AllocateData(int handleID, size_t dataSize) {
    HandleData *handle;
    
    if (handleID < 0 || handleID >= s_handleCount) {
        return NULL;
    }
    
    handle = &s_handleTable[handleID];
    handle->data = DXALLOC((size_t)dataSize);
    
    return handle->data;
}

void *PL_Handle_GetData(int handleID, HandleType handleType) {
    HandleData *handle;
    if (handleID < 0 || handleID >= s_handleCount) {
        return NULL;
    }
    
    handle = &s_handleTable[handleID];
    if (handle->handleType != handleType) {
        return NULL;
    }
    return handle->data;
}

int PL_Handle_SwapHandleIDs(int handleAID, int handleBID) {
    HandleData *handleA, *handleB;
    HandleData tempHandle;
    
    if (handleAID < 0 || handleAID >= s_handleCount
        || handleBID < 0 || handleBID >= s_handleCount
    ) {
        return -1;
    }
    
    handleA = &s_handleTable[handleAID];
    handleB = &s_handleTable[handleBID];
    
    SDL_memcpy(&tempHandle, handleA, sizeof(HandleData));
    SDL_memcpy(handleA, handleB, sizeof(HandleData));
    SDL_memcpy(handleB, &tempHandle, sizeof(HandleData));
    
    return 0;
}

int PL_Handle_GetPrevID(int handleID) {
    HandleData *handle;
    if (handleID < 0 || handleID >= s_handleCount) {
        return -1;
    }
    
    handle = &s_handleTable[handleID];
    return handle->prevID;
}

int PL_Handle_GetNextID(int handleID) {
    HandleData *handle;
    if (handleID < 0 || handleID >= s_handleCount) {
        return -1;
    }
    
    handle = &s_handleTable[handleID];
    return handle->nextID;
}

int PL_Handle_GetFirstIDOf(HandleType handleType) {
    if (handleType < 0 || handleType >= DXHANDLE_END) {
        return -1;
    }
    
    return s_handleLists[handleType];
}

int PL_Handle_SetDeleteFlag(int handleID, int *deleteFlag) {
    HandleData *handle;
    if (handleID < 0 || handleID >= s_handleCount) {
        return -1;
    }
    
    handle = &s_handleTable[handleID];
    if (handle->handleType == DXHANDLE_NONE) {
        return -1;
    }
    
    handle->deleteFlag = deleteFlag;
    
    return 0;
}
