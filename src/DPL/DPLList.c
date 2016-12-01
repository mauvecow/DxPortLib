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

#include "DPLInternal.h"

#define addressof(type, element) ( &(((type *)0)->element) )

void DPL_List_Init(DPL_List_t *list) {
    list->node.next = &list->node;
    list->node.prev = &list->node;
    list->node.list = list;
    list->node.value = 0;
    list->count = 0;
}

void DPL_List_Add(DPL_List_t *list, DPL_ListNode_t *node, void *value, DPL_ListNode_t *after) {
    node->value = value;
    node->prev = after;
    node->next = after->next;
    after->next = node;
    node->next->prev = node;
    node->list = list;
    list->count += 1;
}
void DPL_List_AddFirst(DPL_List_t *list, DPL_ListNode_t *node, void *value) {
    DPL_List_Add(list, node, value, &list->node);
}
void DPL_List_AddLast(DPL_List_t *list, DPL_ListNode_t *node, void *value) {
    DPL_List_Add(list, node, value, list->node.prev);
}

DPL_ListNode_t *DPL_List_First(DPL_List_t *list) {
    DPL_ListNode_t *node = list->node.next;
    if (node == &list->node) {
        return NULL;
    }
    return node;
}
DPL_ListNode_t *DPL_List_Last(DPL_List_t *list) {
    DPL_ListNode_t *node = list->node.prev;
    if (node == &list->node) {
        return NULL;
    }
    return node;
}

void DPL_List_Clear(DPL_List_t *list) {
    while (list->node.next != &list->node) {
        DPL_ListNode_Unlink(list->node.next);
    }
}

void DPL_ListNode_Init(DPL_ListNode_t *node) {
    node->value = NULL;
    node->prev = NULL;
    node->next = NULL;
}
DPL_ListNode_t *DPL_ListNode_Prev(DPL_ListNode_t *node) {
    node = node->prev;
    if (node == &node->list->node) {
        return NULL;
    }
    return node;
}
DPL_ListNode_t *DPL_ListNode_Next(DPL_ListNode_t *node) {
    node = node->next;
    if (node == &node->list->node) {
        return NULL;
    }
    return node;
}

void DPL_ListNode_Unlink(DPL_ListNode_t *node) {
    DPL_List_t *list = node->list;
    if (list == NULL) {
        return;
    }
    
    node->next->prev = node->prev;
    node->prev->next = node->next;
    
    node->next = NULL;
    node->prev = NULL;
    node->list = NULL;
    list->count -= 1;
}
