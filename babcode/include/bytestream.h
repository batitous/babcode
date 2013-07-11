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

#ifndef BABCODE_BYTESTREAM_H
#define BABCODE_BYTESTREAM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _byte_stream_
{
    unsigned char * buffer;
    unsigned char * current;
} ByteStream;

extern void set4ToBuffer(unsigned char *buffer, unsigned int integer);


extern ByteStream * newByteStream(unsigned char * input);

extern void resetByteStream(ByteStream * stream);

extern unsigned int getByteStreamSize(ByteStream * stream);

extern void write1ToByteStream(ByteStream * stream, unsigned char data);

extern void write2ToByteStream(ByteStream * stream, unsigned short data);

extern void write4ToByteStream(ByteStream * stream, unsigned int data);

extern void writeBufferToByteStream(ByteStream * stream, unsigned char * input, unsigned int size);


extern unsigned char read1FromByteStream(ByteStream * stream);
extern unsigned int read4FromByteStream(ByteStream * stream);
extern void readBufferFromByteStream(ByteStream * stream, unsigned char * output, unsigned int size);
    
#ifdef __cplusplus
}
#endif

#endif
