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
// * Neither the name of the University of California, Berkeley nor the
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

#include <string.h>

// code cames from https://github.com/preshing/CompareIntegerMaps, public domain.


#define FIRST_CELL(table, hash) (table->nodes + ((hash) & (table->size - 1)))
#define CIRCULAR_NEXT(table, c) ((c) + 1 != table->nodes + table->size ? (c) + 1 : table->nodes)
#define CIRCULAR_OFFSET(table, a, b) ((b) >= (a) ? (b) - (a) : table->size + (b) - (a))


static UInt32 integerHash(UInt32 h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

static void HashTableRepopulate(HashTable * table, UInt32 desiredSize)
{
	HashNode * node;
	HashNode * n;

    // Get start/end pointers of old array
    HashNode * oldNodes = table->nodes;
    HashNode * end = table->nodes + table->size;
    
    // Allocate new array
    table->size = desiredSize;
    table->nodes = (HashNode *)malloc(sizeof(HashNode)*table->size);
    memset(table->nodes, 0, sizeof(HashNode) * table->size);
    
    // Iterate through old array
    for (n = oldNodes; n != end; n++)
    {
        if (n->key)
        {
            // Insert this element into new array
            for (node = FIRST_CELL(table,integerHash(n->key));; node = CIRCULAR_NEXT(table,node))
            {
                if (!node->key)
                {
                    // Insert here
                    *node = *n;
                    break;
                }
            }
        }
    }
    
    // Delete old array
    free(oldNodes);
}


void HashTableInit(HashTable * table, UInt32 size)
{
    table->size = size;
    table->population = 0;    
    table->nodes = (HashNode *)malloc(sizeof(HashNode)*table->size);
    memset(table->nodes, 0, sizeof(HashNode) * table->size);
}

HashNode * HashTableLookup(HashTable * table, UInt32 key)
{
	HashNode * n;

    // Check regular cells
    for (n = FIRST_CELL(table,integerHash(key));; n = CIRCULAR_NEXT(table,n))
    {
        if (n->key == key)
            return n;
        if (!n->key)
            return NULL;
    }
}



void HashTableDelete(HashTable * table, HashNode * node)
{
	HashNode * ideal;
	HashNode * neighbor;

    // Delete from regular cells
    
    // Remove this cell by shuffling neighboring cells so there are no gaps in anyone's probe chain
    for (neighbor = CIRCULAR_NEXT(table,node);; neighbor = CIRCULAR_NEXT(table,neighbor))
    {
        if (!neighbor->key)
        {
            // There's nobody to swap with. Go ahead and clear this cell, then return
            node->key = 0;
            node->value = 0;
            table->population--;
            return;
        }
        ideal = FIRST_CELL(table,integerHash(neighbor->key));
        if (CIRCULAR_OFFSET(table, ideal, node) < CIRCULAR_OFFSET(table, ideal, neighbor))
        {
            // Swap with neighbor, then make neighbor the new cell to remove.
            *node = *neighbor;
            node = neighbor;
        }
    }
}

HashNode * HashTableInsert(HashTable * table, UInt32 key)
{
	HashNode* node;

    // Check regular cells
    for (;;)
    {
        for (node = FIRST_CELL(table,integerHash(key));; node = CIRCULAR_NEXT(table,node))
        {
            if (node->key == key)
                return node;        // Found
            if (node->key == 0)
            {
                // Insert here
                if ((table->population + 1) * 4 >= table->size * 3)
                {
                    // Time to resize
                    HashTableRepopulate(table,table->size * 2);
                    break;
                }
                ++table->population;
                node->key = key;
                return node;
            }
        }
    }
}
