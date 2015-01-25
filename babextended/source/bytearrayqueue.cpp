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

#include "../../babcode/include/babcode.h"
#include "../include/bytearrayqueue.h"
#include <string.h>


ByteArrayQueue::ByteArrayQueue(uint32_t queueSize, uint32_t arraySize)
{
    mHead = 0;
    mTail = 0;
    mSize = queueSize;
    mArraySize = arraySize;
    mPending = new uint8_t*[mSize];
    
    for (int i=0 ; i < mSize; i++)
    {
        mPending[i] = new uint8_t[mArraySize];
    }
}

ByteArrayQueue::~ByteArrayQueue()
{
    for (int i=0 ; i < mSize ; i++)
    {
        delete mPending[i];
    }
    delete mPending;
}

void ByteArrayQueue::reset()
{
    mHead = 0;
    mTail = 0;
}

uint32_t ByteArrayQueue::elementNumber() const
{
    return mTail - mHead;
}

bool ByteArrayQueue::write(const uint8_t * array, uint32_t arraySize)
{
    uint32_t size = mSize-1;
    if ( ((mTail+1) & (size)) == ((mHead) & (size)) )
    {
        return false;
    }
    
    memcpy(mPending[mTail & (size)], array, arraySize);
    mTail++;
    
    return true;
}

bool ByteArrayQueue::write(const uint8_t * value)
{
    uint32_t size = mSize-1;
    if ( ((mTail+1) & (size)) == ((mHead) & (size)) )
    {
        return false;
    }
    
    memcpy(mPending[mTail & (size)], value, mArraySize);
    mTail++;
    
    return true;
}

uint8_t * ByteArrayQueue::read()
{
    if (mHead == mTail)
    {
        return 0;
    }
    
    uint8_t * value= mPending[mHead & (mSize-1)];
    mHead++;
    
    return value;
}
