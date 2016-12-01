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

// This is a header for a C++ interface to DxPortLib.
//
// It is purely optional, and does not depend on STL or other external libraries.

#ifndef DXPORTLIB_H_HEADER
#define DXPORTLIB_H_HEADER

#ifndef DPLCOMMON_H_HEADER
#  include "DPLCommon.h"
#endif

#include "DxPortLib_c.h"

namespace DPL {
    
namespace Text { // DPL::Text

int DPLCALL SetDefaultEncoding(int encoding);

int DPLCALL ConvertStringEncoding(
    char *buffer, int bufferLength,
    const char *string,
    int destEncoding = -1, int srcEncoding = -1);

int DPLCALL ConvertStringToChar(
    char *buffer, int bufferLength,
    const wchar_t *string,
    int destEncoding = -1);

int DPLCALL ConvertStringToWideChar(
    wchar_t *buffer, int bufferLength,
    const char *string,
    int srcEncoding = -1);

} // namespace DPL::Text

class WinINI {
public:
    DPLCALL WinINI();
    DPLCALL WinINI(const char *inputFilename, int fileEncoding = -1);
    DPLCALL ~WinINI();
    
    DPLCALL int Clear();
    
    DPLCALL int ReadFile(const char *inputFilename, int fileEncoding = -1);
    DPLCALL int WriteFile(const char *outputFilename, int fileEncoding = -1);
    
    DPLCALL int GetInt(const char *section, const char *name, int defaultValue);
    DPLCALL const char * GetString(const char *section, const char *name, const char *defaultValue);
    
    DPLCALL int SetInt(const char *section, const char *name, int value);
    DPLCALL int SetString(const char *section, const char *name, const char *value);
    
    DPLCALL int DeleteValue(const char *section, const char *name);
    DPLCALL int DeleteSection(const char *section);
private:
    int m_handle;
}; // class WinINI

template<class T>
class List;

template<class T>
class ListNode {
private:
    friend class List<T>;
    
    DPL_ListNode_t m_node;
public:
    DPLCALL ListNode() {
        DPL_ListNode_Init(&m_node);
    }
    DPLCALL ~ListNode() {
        Unlink();
    }
    DPLCALL void Unlink() {
        DPL_ListNode_Unlink(&m_node);
    }
    
    DPLCALL ListNode<T> *Next() {
        return (ListNode<T> *)DPL_ListNode_Next(&m_node);
    }
    DPLCALL ListNode<T> *Prev() {
        return (ListNode<T> *)DPL_ListNode_Prev(&m_node);
    }
    
    DPLCALL T* Value() {
        return (T*)(m_node.value);
    }
};

template<class T>
class List { // DPL::List<T>
private:
    DPL_List_t m_list;
public:
    DPLCALL List() {
        DPL_List_Init(&m_list);
    }
    DPLCALL ~List() {
        DPL_List_Clear(&m_list);
    }
    
    DPLCALL void Add(ListNode<T> *node, T *value, ListNode<T> *after) {
        DPL_List_Add(&m_list, &node->m_node, value, after);
    }
    DPLCALL void AddFirst(ListNode<T> *node, T *value) {
        DPL_List_AddFirst(&m_list, &node->m_node, value);
    }
    DPLCALL void AddLast(ListNode<T> *node, T *value) {
        DPL_List_AddLast(&m_list, &node->m_node, value);
    }
    
    DPLCALL ListNode<T> *First() {
        return (ListNode<T> *)DPL_List_First(&m_list);
    }
    DPLCALL ListNode<T> *Last() {
        return (ListNode<T> *)DPL_List_Last(&m_list);
    }
    
    DPLCALL void Clear() {
        DPL_List_Clear(&m_list);
    }
    
    DPLCALL int Count() const {
        return m_list.count;
    }
    
}; // DPL::List<T>

struct _DPL_ListNode_t {
    DPL_ListNode_t *prev;
    DPL_ListNode_t *next;
    DPL_List_t *list;
    void *value;
};

struct _DPL_List_t {
    DPL_ListNode_t node;
    int count;
};

extern DPLCALL void DPL_List_Init(DPL_List_t *list);
extern DPLCALL void DPL_List_Add(DPL_List_t *list, DPL_ListNode_t *node, void *value, DPL_ListNode_t *after);
extern DPLCALL void DPL_List_AddFirst(DPL_List_t *list, DPL_ListNode_t *node, void *value);
extern DPLCALL void DPL_List_AddLast(DPL_List_t *list, DPL_ListNode_t *node, void *value);
extern DPLCALL DPL_ListNode_t * DPL_List_First(DPL_List_t *list);
extern DPLCALL DPL_ListNode_t * DPL_List_Last(DPL_List_t *list);
extern DPLCALL DPL_ListNode_t * DPL_List_Prev(DPL_ListNode_t *node);
extern DPLCALL DPL_ListNode_t * DPL_List_Next(DPL_ListNode_t *node);
extern DPLCALL void DPL_List_Unlink(DPL_ListNode_t *node);
extern DPLCALL void DPL_List_Clear(DPL_List_t *list);


} // namespace DPL

#endif // #ifndef DXPORTLIB_H_HEADER
