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

#ifndef BABCODE_RANDOM_H
#define BABCODE_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Init random seed.
 *
 */
extern void  randomInit(void);

/** @brief Get random between low and high boundary.
 *
 * @param lowBoundary		Low.
 * @param highBoundary		High.
 * @return A random number.
 */
extern int32_t getRandomBetween(int32_t lowBoundary, int32_t highBoundary);

/** @brief Get a random number between 0 and high boundary.
 *
 * @param highBoundary		High.
 * @return A random number.
 */
extern int32_t getRandom(int32_t highBoundary);

    
//Galois LFSR using CRC64 polynomials
    
typedef struct _lfsr_crc64_
{
    uint64_t lfsr;
} RandomLfsr;

extern void lfsrInit(RandomLfsr * lfsr);

extern uint64_t lsfrRandom(RandomLfsr * lfsr);

extern void lfSrSeed(RandomLfsr * lfsr, const uint64_t seed);
    
extern void lfsrSeedWithTime(RandomLfsr * lfsr);
    
    
#ifdef __cplusplus
 }
#endif

#endif
