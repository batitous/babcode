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

#ifndef babextended_vectorimpl_h
#define babextended_vectorimpl_h

#include <stdlib.h>


template<typename Data>
Vector<Data>::Vector()
{
    mSize = 0;
    mCapacity = 0;
    mData = 0;
}

template<typename Data>
Vector<Data>::Vector(Vector const &other) : mSize(other.mSize), mCapacity(other.mCapacity), mData(0)
{
//    mData = new Data[mCapacity];
    mData = (Data *)malloc(mCapacity*sizeof(Data));
    
    memcpy(mData, other.mData, mSize*sizeof(Data));
}

template<typename Data>
Vector<Data>::~Vector()
{
    delete mData;
}

template<typename Data>
Vector<Data> & Vector<Data>::operator=(Vector const &other)
{
    delete mData;
    mSize = other.mSize;
    mCapacity = other.mCapacity;

    mData = (Data *)malloc(mCapacity*sizeof(Data));
    
    memcpy(mData, other.mData, mSize*sizeof(Data));
    
    return *this;
}


template<typename Data>
void Vector<Data>::push(Data const &x)
{
    if (mCapacity == mSize)
    {
        resize();
    }
    mData[mSize++] = x;
}

template<typename Data>
size_t Vector<Data>::size() const
{
    return mSize;
}

template<typename Data>
Data &Vector<Data>::get(size_t idx)
{
    return mData[idx];
}

template<typename Data>
Data const & Vector<Data>::operator[](size_t idx) const
{
    return mData[idx];
}

template<typename Data>
Data & Vector<Data>::operator[](size_t idx)
{
    return mData[idx];
}

template<typename Data>
void Vector<Data>::resize()
{
    mCapacity = mCapacity ? mCapacity*2 : 1;
    
    Data *newdata = (Data *)malloc(mCapacity*sizeof(Data));
    
    if (mData!=0)
    {
        memcpy(newdata, mData, mSize * sizeof(Data));
        delete mData;
    }
    
    mData = newdata;
}

#endif
