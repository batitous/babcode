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

#include "../../include/babcode.h"

#define SIGNAL      0
#define BROADCAST   1

// Condition variables are available in Vista, Win7... not on WinXP :-(

// code comes from http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

void ConditionVarInit(ConditionVar * cv)
{
    // Initialize the count to 0.
    cv->waiters_count_ = 0;
    
    InitializeCriticalSection(cv->waiters_count_lock_);
    
    // Create an auto-reset event.
    cv->events_[SIGNAL] = CreateEvent (NULL,  // no security
                                       FALSE, // auto-reset event
                                       FALSE, // non-signaled initially
                                       NULL); // unnamed
    
    // Create a manual-reset event.
    cv->events_[BROADCAST] = CreateEvent (NULL,  // no security
                                          TRUE,  // manual-reset
                                          FALSE, // non-signaled initially
                                          NULL); // unnamed
}

void ConditionVarWait(ConditionVar * cv, Mutex * m)
{
    // Avoid race conditions.
    EnterCriticalSection (&cv->waiters_count_lock_);
    cv->waiters_count_++;
    LeaveCriticalSection (&cv->waiters_count_lock_);
    
    // It's ok to release the <external_mutex> here since Win32
    // manual-reset events maintain state when used with
    // <SetEvent>.  This avoids the "lost wakeup" bug...
    LeaveCriticalSection (external_mutex);
    
    // Wait for either event to become signaled due to <pthread_cond_signal>
    // being called or <pthread_cond_broadcast> being called.
    int result = WaitForMultipleObjects (2, ev->events_, FALSE, INFINITE);
    
    EnterCriticalSection (&cv->waiters_count_lock_);
    cv->waiters_count_--;
    int last_waiter =
    result == WAIT_OBJECT_0 + BROADCAST
    && cv->waiters_count_ == 0;
    LeaveCriticalSection (&cv->waiters_count_lock_);
    
    // Some thread called <pthread_cond_broadcast>.
    if (last_waiter)
        // We're the last waiter to be notified or to stop waiting, so
        // reset the manual event.
        ResetEvent (cv->events_[BROADCAST]);
    
    // Reacquire the <external_mutex>.
    EnterCriticalSection (external_mutex, INFINITE);
}

void ConditionVarSignal(ConditionVar * cv)
{
    // Avoid race conditions.
    EnterCriticalSection (&cv->waiters_count_lock_);
    int have_waiters = cv->waiters_count_ > 0;
    LeaveCriticalSection (&cv->waiters_count_lock_);
    
    if (have_waiters)
        SetEvent (cv->events_[SIGNAL]);
}

void ConditionVarDelete(ConditionVar * cv)
{
    DeleteCriticalSection(cv->waiters_count_lock_);
}
