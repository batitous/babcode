// Copyright (c) 2013, Baptiste Burles
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
// THIS SOFTWARE IS PROVIDED BY Baptiste Burles AND CONTRIBUTORS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Baptiste Burles AND CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "../include/babcode.h"

#define DEFAULT_CAPACITY    32
#define CAPACITY_INCREMENT  2

void arrayInit(Array *a)
{
    a->size = 0;
    a->capacity = DEFAULT_CAPACITY;
    a->array = (unsigned long *)malloc(a->capacity*sizeof(unsigned long));
}

void arrayFree(Array *a)
{
    free(a->array);
    a->array = 0;
    a->size = 0;
}

void arrayAppend(Array *a, unsigned long element)
{
    if (a->size == a->capacity)
    {
        a->capacity = a->capacity * CAPACITY_INCREMENT;
        a->array = (unsigned long *)realloc( a->array, a->capacity*sizeof(unsigned long));
    }
    
    a->array[a->size] = element;
    a->size++;
}

void arrayRemove(Array *a, unsigned long element)
{
    unsigned long i;
    
    for (i=0; i < a->size; i++)
    {
        if (a->array[i] == element)
        {
            a->array[i] = 0;
        }
    }
    
}
