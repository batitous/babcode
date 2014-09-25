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

#include <stdlib.h>
#include <string.h>

#include "../include/babcode.h"


ByteStream * newByteStream(uint8_t * input, uint32_t size)
{
    ByteStream * stream = (ByteStream *)malloc(sizeof(ByteStream));

    stream->buffer = input;
    stream->current = stream->buffer;
    stream->end = input + size;

    return stream;
}

void resetByteStream(ByteStream * stream)
{
    stream->current = stream->buffer;
}

bool isEndOfStream(ByteStream * stream)
{
    if (stream->current == stream->end)
    {
        return true;
    }
    
    return false;
}

uint32_t getByteStreamSize(ByteStream * stream)
{
    return (uint32_t)(stream->current - stream->buffer);
}

void write8BitsToStream(ByteStream * stream, uint8_t data)
{
    *stream->current = data;
    stream->current++;
}

void write16BitsToStream(ByteStream * stream, uint16_t data)
{
    *stream->current = (uint16_t)(data);
    *(stream->current+1) = (data>>8UL);
    stream->current += 2;
}

void write32BitsToStream(ByteStream * stream, uint32_t data)
{
    *stream->current = (data);
    *(stream->current+1) = (data>>8UL);
    *(stream->current+2) = (data>>16UL);
    *(stream->current+3) = (data>>24UL);
    stream->current += 4;
}

void writeRawBytesToStream(ByteStream * stream, uint8_t * input, uint32_t size)
{
    memcpy(stream->current, input,size);
    stream->current += size;
}

uint8_t read8BitsFromStream(ByteStream * stream)
{
    uint8_t data = *stream->current;
    stream->current++;

    return data;
}

uint16_t read16BitsFromStream(ByteStream * stream)
{
    uint16_t integer = (stream->current[1]<<8UL)| (stream->current[0]);
    
    stream->current +=2;
    return integer;
}

uint32_t read32BitsFromStream(ByteStream * stream)
{
    uint32_t integer;
    uint32_t temp;
    
    temp = stream->current[3];
    integer = temp << 24UL;
    
    temp = stream->current[2];
    integer = (temp << 16UL) | integer;
    
    temp = stream->current[1];
    integer = (temp << 8UL) | integer;
    
    temp = stream->current[0];
    integer = (temp) | integer;
    
    stream->current +=4;
    return integer;
}

void readRawBytesFromStream(ByteStream * stream, uint8_t * output, uint32_t size)
{
    memcpy(output, stream->current,size);
    stream->current += size;
}
