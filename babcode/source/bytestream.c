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

unsigned int get4FromBuffer(unsigned char *buffer)
{
	unsigned int integer = ((buffer)[0]<<24)| ((buffer)[1]<<16)|((buffer)[2]<<8)|((buffer)[3]);

	return integer;
}

void set4ToBuffer(unsigned char *buffer, unsigned int integer)
{
	buffer[0] = (integer>>24);
	buffer[1] = (integer>>16);
	buffer[2] = (integer>>8);
	buffer[3] = (integer);
}

unsigned short get2FromBuffer(unsigned char *buffer)
{
	unsigned short integer = ((buffer)[0]<<8)|((buffer)[1]);

	return integer;
}

void set2ToBuffer(unsigned char *buffer, unsigned short integer)
{
	buffer[0] = (integer>>8);
	buffer[1] = (unsigned char)(integer);
}


ByteStream * newByteStream(unsigned char * input)
{
    ByteStream * stream = (ByteStream *)malloc(sizeof(ByteStream));

    stream->buffer = input;
    stream->current = stream->buffer;

    return stream;
}

void resetByteStream(ByteStream * stream)
{
    stream->current = stream->buffer;
}

unsigned int getByteStreamSize(ByteStream * stream)
{
    return (unsigned int)(stream->current - stream->buffer);
}

void write1ToByteStream(ByteStream * stream, unsigned char data)
{
    *stream->current = data;
    stream->current++;
}

void write2ToByteStream(ByteStream * stream, unsigned short data)
{
    set2ToBuffer(stream->current, data);
    stream->current += 2;
}

void write4ToByteStream(ByteStream * stream, unsigned int data)
{
    set4ToBuffer(stream->current, data);
    stream->current += 4;
}

void writeBufferToByteStream(ByteStream * stream, unsigned char * input, unsigned int size)
{
    memcpy(stream->current, input,size);
    stream->current += size;
}

unsigned char read1FromByteStream(ByteStream * stream)
{
    unsigned char data = *stream->current;
    stream->current++;

    return data;
}

unsigned short read2FromByteStream(ByteStream * stream)
{
    uint16_t integer = (stream->current[0]<<8)| (stream->current[1]);

	stream->current +=2;
    return integer;
}

unsigned int read4FromByteStream(ByteStream * stream)
{
    int integer = (stream->current[0]<<24)| (stream->current[1]<<16)|(stream->current[2]<<8)|(stream->current[3]);

	stream->current +=4;
    return integer;
}

void readBufferFromByteStream(ByteStream * stream, unsigned char * output, unsigned int size)
{
    memcpy(output, stream->current,size);
    stream->current += size;
}
