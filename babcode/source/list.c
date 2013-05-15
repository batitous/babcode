// Copyright (c) 2013, Sylvain Fay-Chatelard
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the University of California, Berkeley nor the
//   names of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sylvain Fay-Chatelard AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Baptiste Burles AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdlib.h>
#include <stdio.h>

#include "../include/babcode.h"

void listInit(List * l)
{
    l->first = NULL;
    l->last = NULL;
    l->size = 0;
}

ListNode* listAddElement(List *l, void *e)
{
    if (l == NULL) return NULL;
    
    ListNode *node = (ListNode*)calloc(1, sizeof(ListNode));
    
    node->next = NULL;
    node->prev = l->last;
    node->data = e;
    
    if (l->last != NULL)
        l->last->next = node;
    
    if (l->first == NULL)
        l->first = node;
    
    l->last = node;
    
    l->size++;
    
    return node;
}

void *listRemoveNode(List *l, ListNode* node)
{
    if (node == NULL) return NULL;
    
    ListNode * prev = node->prev;
    ListNode * next = node->next;
    
    if (prev != NULL)
        prev->next = next;
    else
        l->first = next;
    
    if (next != NULL)
        next->prev = prev;
    else
        l->last = prev;
    
    node->prev = NULL;
    node->next = NULL;
    
    l->size--;
    
    return next;
}

void *listRemoveLastNode(List *l)
{
    ListNode *node = l->last;
    
    if (node == NULL) return NULL;
    
    listRemoveNode(l, l->last);
    
    return node->data;
}

void *listGetElementAt(List* l, Int32 index)
{
    int i=0;
    ListNode *n = l->first;
    while (n != NULL)
    {
        if (i == index)
            return n->data;
        
        n = n->next;
        i++;
    }
    return NULL;
}

void listRemoveElementAt(List* l, Int32 index)
{
    int i=0;
    ListNode *n = l->first;
    while (n != NULL)
    {
        if (i == index)
        {
            listRemoveNode(l, n);
            return;
        }
        
        n = n->next;
        i++;
    }
}
