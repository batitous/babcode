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

#ifndef BABCODE_UTILS_H
#define BABCODE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#define FREEMEM( ptr )  { free( ptr ) ; ptr = 0 ;}

    
extern uint16_t crc16Compute(const uint8_t* data, const uint32_t length);
extern uint32_t crc32Compute(const uint8_t* data, const uint32_t length);
    
extern void write32bitsToBuffer(uint8_t * buffer, uint32_t integer);
extern uint32_t read32bitsFromBuffer(const uint8_t * buffer);
    
extern void write16bitsToBuffer(uint8_t * buffer, uint16_t integer);
extern uint16_t read16bitsFromBuffer(const uint8_t * buffer);
    
    
extern uint32_t getExecutablePath(char * buff, uint32_t buffSize);
extern bool getRealPath(const char *path, char *resolvedPath, int32_t * resolvePathSize);

    
#ifdef __cplusplus
  }
#endif

#endif
