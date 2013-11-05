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
// * Neither the name of Baptiste Burles nor the
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

#ifndef babcode_list_h
#define babcode_list_h

#ifdef __cplusplus
extern "C" {
#endif
    
/* how to use list

 ListNode *n = list->first;
 while (n != NULL)
 {
    void * data = n->data;
    
    ...
 
 
    n = n->next;
 }
*/
    
typedef struct _list_node_
{
    struct _list_node_ *next;
    struct _list_node_ *prev;
    void *data;
} ListNode;
    
typedef struct _list_
{
    ListNode *first;
    ListNode *last;
    int size;
} List;
    
extern void listInit(List * l);

extern ListNode * listAddElement(List *l, void *e);

extern void * listRemoveNode(List *l, ListNode* node);

extern void * listRemoveLastNode(List *l);
    
/** Use with caution */
extern void * listGetElementAt(List* l, Int32 index);
    
extern void listRemoveElementAt(List* l, Int32 index);
    
#ifdef __cplusplus
}
#endif

#endif
