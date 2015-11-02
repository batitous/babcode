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
#include "../include/circularbuffer.h"

#include <string.h>


CircularBuffer::CircularBuffer(uint32_t size, uint32_t numberOfReadByte, uint32_t numberOfWriteByte)
{
    mRead = 0;
    mWrite = 0;
    
    mWriteSize = numberOfWriteByte;
    mReadSize = numberOfReadByte;
    
    mBufferSize = size;
    mBuffer = new uint8_t[size];
}

CircularBuffer::~CircularBuffer()
{
    delete mBuffer;
}

void CircularBuffer::write(const uint8_t * input, uint32_t size)
{
    /*uint32_t oki = 0;
    if (c->mRead == c->mWrite)
    {
        oki = c->mBufferSize;
    }
    else if (c->mWrite < c->mRead )
    {
        oki = c->mRead - c->mWrite;
    }
    else
    {
        oki = c->mBufferSize - c->mWrite + c->mRead;
    }
    
    if (oki < size)
    {
        return 0;
    }
    */
    
    if ( (mWrite+size) < mBufferSize)
    {
        memcpy(&mBuffer[mWrite], input, size);
        
        mWrite = mWrite + size;
    }
    else
    {
        uint32_t byteToCopy = mBufferSize - mWrite;
        
        memcpy(&mBuffer[mWrite], input, byteToCopy);
        
        memcpy(&mBuffer[0], input+byteToCopy, size - byteToCopy);
        
        mWrite = size - byteToCopy;
    }
}

void CircularBuffer::write(const uint8_t * input )
{
    write(input, mWriteSize);
}

bool CircularBuffer::read(uint8_t * output)
{
    return read(output, mReadSize);
}


bool CircularBuffer::read(uint8_t * output, uint32_t size)
{
    uint32_t oki = 0;
    if (mRead < mWrite)
    {
        oki = (mWrite - mRead);
    }
    else if (mRead > mWrite)
    {
        oki = mBufferSize - mRead + mWrite;
    }
    
    //        printf("mRead %d mWrite %d\n", mRead, mWrite);
    
    if (oki >= size)
    {
        if ( (mRead+size) < mBufferSize)
        {
            memcpy(output, &mBuffer[mRead], size);
            
            mRead = mRead + size;
        }
        else
        {
            uint32_t byteToCopy = mBufferSize - mRead;
            
            memcpy(output, &mBuffer[mRead], byteToCopy);
            
            memcpy(output + byteToCopy, &mBuffer[0], size - byteToCopy);
            
            mRead = size - byteToCopy;
        }
        
        return true;
    }
    
    return false;
}
