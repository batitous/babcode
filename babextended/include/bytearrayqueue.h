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

#ifndef babextended_bytearrayqueue_h
#define babextended_bytearrayqueue_h

/**
 * A queue of byte array.
 * Byte array have fixed size !
 *
 */
class ByteArrayQueue
{
public:
    
    /**
     * @brief Create a new Byte array Queue object
     * @param queueSize         Size of the queue
     * @param arraySize         Size of array
     */
    ByteArrayQueue(uint32_t queueSize, uint32_t arrraySize);
    
    ~ByteArrayQueue();
    
    /**
     * @brief Reset the internal state of the queue
     */
    void reset();
    
    /**
     * @brief Get number of element in queue
     * @return
     */
    uint32_t elementNumber() const;
    
    
    /**
     * @brief Write an array of the specified size into the queue
     * @param value     Value to be written
     * @param size      Size of array
     * @return False if queue full
     */
    bool write(const uint8_t * array, uint32_t size);
    
    /** @brief Write an array into the queue
     * @param value     Array to be written
     * @return False if queue full
     */
    bool write(const uint8_t * value);
    
    /**
     * @brief Read a value from the queue
     * @return 0 if queue empty, else a pointer to an array. You can copy it for later use... DON'T FREE THE POINTER
     */
    uint8_t * read();
    
    
private:
    uint32_t    mHead;  /**< queue head */
    uint32_t    mTail;  /**< queue tail */
    uint32_t    mSize;  /**< queue size */
    uint8_t**   mPending;   /**< buffer where store the value */
    uint32_t    mArraySize; /**< size of array */
};

#endif
