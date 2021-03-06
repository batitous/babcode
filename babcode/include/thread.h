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

#ifndef BABCODE_THREAD_H
#define BABCODE_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#if PLATFORM == PLATFORM_WINDOWS
	
	#define _WINSOCKAPI_
	#   include <windows.h>

	typedef struct _thread
	{
		DWORD id ;
		HANDLE h ;
	} Thread;
	
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

#   include <pthread.h>

	typedef pthread_t Thread;
    
    typedef enum _thread_priority_
    {
        THREAD_PRIORITY_NORMAL = 0,
        THREAD_PRIORITY_HIGH = 1,
        THREAD_PRIORITY_REALTIME = 2
    } ThreadPriority;
	
#endif

extern void threadInit( Thread * t, void *(* func)(void *), void *param );

extern void threadJoin( Thread * t );

extern bool threadSetPriority( Thread * t, ThreadPriority p);
    
extern bool threadTerminate( Thread * t );
    
extern void threadExit( void );

#ifdef __cplusplus
 }
#endif

#endif
