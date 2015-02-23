// Copyright (c) 2015, Baptiste Burles
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

#ifndef babextended_circularbuffer_h
#define babextended_circularbuffer_h


/** A circular byte buffer
 *
 */
class CircularBuffer
{
public:
    /** @brief Create a new circular buffer object
     *
     * @param size              Size of the internal buffer
     * @param numberOfReadByte  Number of bytes to read when call the read method
     * @param numberOfWriteByte Number of bytes to write when call the write method
     */
    CircularBuffer(uint32_t size, uint32_t numberOfReadByte, uint32_t numberOfWriteByte);
    ~CircularBuffer();
    
    /** @brief Write raw bytes into the circular buffer
     *
     * @param input     Byte array to write
     * @param size      Size of array
     */
    void write(const uint8_t * input, uint32_t size);
    
    /** @brief Write raw bytes into the circular buffer
     *
     * @param input     Byte array to write
     */
    void write(const uint8_t * input );
    
    /** @brief Read raw bytes from the circular buffer
     * @param output    Array where write the read bytes
     * @param size      Number of bytes to read
     * @return true if we can read the number of bytes requested, else false
     */
    bool read(uint8_t * output, uint32_t size);
    
    /** @brief Read raw bytes from the circular buffer.
     * @param output    Array where write the read bytes
     * @return false if we can't read enough bytes.
     */
    bool read(uint8_t * output);
    
    
private:
    uint32_t    mRead;
    uint32_t    mWrite;
    
    uint32_t    mWriteSize;
    uint32_t    mReadSize;
    
    uint8_t *   mBuffer;
    uint32_t    mBufferSize;
};

#endif
