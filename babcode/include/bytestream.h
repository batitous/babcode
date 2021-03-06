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

#ifndef BABCODE_BYTESTREAM_H
#define BABCODE_BYTESTREAM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _byte_stream_
{
    uint8_t * buffer;
    uint8_t * current;
    uint8_t * end;
} ByteStream;

extern ByteStream * newByteStream(uint8_t * input, uint32_t size);
    
extern void resetByteStream(ByteStream * stream);
extern bool isEndOfStream(ByteStream * stream);
extern uint32_t getByteStreamSize(ByteStream * stream);

extern void write8BitsToStream(ByteStream * stream, uint8_t data);
extern void write16BitsToStream(ByteStream * stream, uint16_t data);
extern void write32BitsToStream(ByteStream * stream, uint32_t data);
extern void writeRawBytesToStream(ByteStream * stream, uint8_t * input, uint32_t size);

extern uint8_t read8BitsFromStream(ByteStream * stream);
extern uint16_t read16BitsFromStream(ByteStream * stream);
extern uint32_t read32BitsFromStream(ByteStream * stream);
extern void readRawBytesFromStream(ByteStream * stream, uint8_t * output, uint32_t size);
    
    
    
#ifdef __cplusplus
}
#endif

#endif
