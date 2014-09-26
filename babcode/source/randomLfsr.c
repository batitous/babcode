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

#include <time.h>


static const uint64_t crc64ecma  = 0x42f0e1eba9ea3693;
static const uint64_t crc64jones = 0xad93d23594c935a9;

void lfsrInit(RandomLfsr * lfsr)
{
    lfsr->lfsr = crc64ecma;
}

uint64_t lsfrRandom(RandomLfsr * lfsr)
{
    return lfsr->lfsr = (lfsr->lfsr >> 1) ^ (-(lfsr->lfsr & 1) & crc64jones);
}

void lfSrSeed(RandomLfsr * lfsr, const uint64_t seed)
{
    lfsr->lfsr = seed;
    for(uint32_t n = 0; n < 8; n++)
    {
        lsfrRandom(lfsr);
    }
}

void lfsrSeedWithTime(RandomLfsr * lfsr)
{
    lfSrSeed(lfsr, time(NULL));
}


