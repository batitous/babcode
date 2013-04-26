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

#ifndef BABCODE_CONVERSION_H
#define BABCODE_CONVERSION_H

#ifdef __cplusplus
extern "C" {
#endif


/** @brief Convert ascii string to integer
 *
 * @param str			Ascii string to convert.
 * @param [out] num		Integer's pointer.
 * @return True if success.
 */
extern Bool String2Int(Int8 const * str, Int32 * num);

extern Bool StringToFloat(Int8 * str, float* floating);

    
/** @brief Convert binary to decimal ascii.
 *
 * @param bin			Binary to convert.
 * @param [out] result  Buffer where store the result. You have to allocated this array.
 * @result	Lenght of converted string.
 */
extern UInt32 Bin2DecimalAscii(Int32 bin,Int8 *result);

/** @brief Convert byte to hexadecimal.
 *
 * @param bin		Byte to convert.
 * @param [out] result	Buffer where store the result. You have to allocated this array.
 */
extern void Bin2Hex(UInt8 bin,Int8 *result);

/** @brief Convert a byte in hexadecimal to binary byte.
 *
 * @param hex		Hexadecimal to convert.
 * @param [out] bin	Byte where store the result.
 * @result False on error.
 */
extern Bool Hex2Bin(UInt8 *hex,UInt8 *bin);

extern Bool StringIsHex(Int8 * str, int* integer);

#ifdef __cplusplus
  }
#endif

#endif
