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

#ifndef BABCODE_CONDITIONVAR_H
#define BABCODE_CONDITIONVAR_H

#ifdef __cplusplus
extern "C" {
#endif
    
    
#if PLATFORM == PLATFORM_WINDOWS

typedef struct _ConditionVar_
{
    UInt32 waiters_count_;
    CRITICAL_SECTION waiters_count_lock_;
    HANDLE events_[2];
} ConditionVar;
	
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        
	typedef pthread_cond_t ConditionVar;
	
#endif

extern void conditionVarInit(ConditionVar * cv);
    
extern void conditionVarWait(ConditionVar * cv, Mutex * m);
    
extern void conditionVarSignal(ConditionVar * cv);
    
extern void conditionVarDelete(ConditionVar * cv);
    
#ifdef __cplusplus
}
#endif

#endif
