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

#ifndef babextended_queueimpl_h
#define babextended_queueimpl_h



template <typename T>
Queue<T>::Queue(T* buffer, uint32_t size)
{
    mHead = 0;
    mTail = 0;
    mSize = size;
    mPending = buffer;
}

template <typename T>
void Queue<T>::reset()
{
    mHead = 0;
    mTail = 0;
}

template <typename T>
uint32_t Queue<T>::elementNumber()
{
    return mTail - mHead;
}

template <typename T>
bool Queue<T>::write(T value)
{
    uint32_t size = mSize-1;
    if ( ((mTail+1) & (size)) == ((mHead) & (size)) )
    {
        return false;
    }
    
    mPending[mTail & (size)] = value;
    mTail++;
    
    return true;
}

template <typename T>
bool Queue<T>::read(T* value)
{
    if (mHead == mTail)
    {
        return false;
    }
    
    *value= mPending[mHead & (mSize-1)];
    mHead++;
    
    return true;
}

template <typename T>
void Queue<T>::read(T *array, uint32_t want)
{
    T * ptr = array;
    uint32_t size = mSize -1;
    for(uint32_t i=0;i<want;i++)
    {
        *ptr = mPending[mHead & (size)];
        ptr++;
        mHead++;
    }
}


#endif
