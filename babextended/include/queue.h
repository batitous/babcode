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

#ifndef babextended_queue_h
#define babextended_queue_h



/**
 * A queue object
 */
template<typename T>
class Queue
{
public:
    
    /**
     * @brief Create a new Queue object
     * @param buffer    Storing's buffer
     * @param size_t    Size of buffer
     */
    Queue<T>(T* buffer, uint32_t size);
    
    /**
     * @brief Reset the internal state of the queue
     */
    void reset();
    
    /**
     * @brief Get number of element in queue
     * @return
     */
    uint32_t elementNumber();
    
    
    /**
     * @brief Write a value into the queue
     * @param value     Value to be written
     * @return False if queue full
     */
    bool write(T value);
    
    /**
     * @brief Read a value from the queue
     * @param value     Pointer to a value
     * @return false if queue empty
     */
    bool read(T* value);
    
    /**
     * @brief Read an array of element.
     * Warning : you have to be sure there is enough element to be read with elementNumber
     *
     * @param array     Array where store the element read
     * @param want      Number of element to read
     */
    void read(T *array, uint32_t want);
    
private:
    uint32_t    mHead;  /**< queue head */
    uint32_t    mTail;  /**< queue tail */
    uint32_t    mSize;  /**< queue size */
    T*          mPending;   /**< buffer where store the value */
    
};


#include "queueimpl.h"


template Queue<uint8_t>::Queue(uint8_t * buffer, uint32_t size);
template void Queue<uint8_t>::reset();
template uint32_t Queue<uint8_t>::elementNumber();
template bool Queue<uint8_t>::write(uint8_t value);
template bool Queue<uint8_t>::read(uint8_t* value);
template void Queue<uint8_t>::read(uint8_t *array, uint32_t want);

template Queue<uint16_t>::Queue(uint16_t * buffer, uint32_t size);
template void Queue<uint16_t>::reset();
template uint32_t Queue<uint16_t>::elementNumber();
template bool Queue<uint16_t>::write(uint16_t value);
template bool Queue<uint16_t>::read(uint16_t* value);
template void Queue<uint16_t>::read(uint16_t *array, uint32_t want);

template Queue<uint32_t>::Queue(uint32_t * buffer, uint32_t size);
template void Queue<uint32_t>::reset();
template uint32_t Queue<uint32_t>::elementNumber();
template bool Queue<uint32_t>::write(uint32_t value);
template bool Queue<uint32_t>::read(uint32_t* value);
template void Queue<uint32_t>::read(uint32_t *array, uint32_t want);


#endif
