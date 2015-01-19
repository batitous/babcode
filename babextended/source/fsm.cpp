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
#include "../include/fsm.h"

Fsm::Fsm()
{
    mInit = false;
    mCurrent = 0;
    mOld = 0;
    mCall = 0;
    mInitCall = 0;
}

void Fsm::set(int newstate, FsmCallback call, FsmCallback init)
{
    mCurrent = newstate;
    mCall = call;
    mInitCall = init;
}

void Fsm::init(int state, FsmCallback call, FsmCallback init)
{
    this->set(state, call, init);
    
    mInit = true;
    mOld = mCurrent;
}

void Fsm::update()
{
    if (mInit == true)
    {
        if (mInitCall != 0)
        {
            (this->*mInitCall)();
        }
        
        mInit = false;
    }
    
    if (mCall != 0)
    {
        (this->*mCall)();
    }
    
    if (mCurrent != mOld)
    {
        mInit = true;
        mOld = mCurrent;
    }
}

bool Fsm::isInState(int state)
{
    if (mCurrent == state)
    {
        return true;
    }
    
    return false;
}

bool Fsm::isStateInitialized(void)
{
    return mInit;
}

