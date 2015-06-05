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

#include "../../include/babcode.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void threadInit( Thread * t, void *(* func)(void *), void *param )
{
	int error = pthread_create( t, NULL, func, param );
	if (error!= 0)
	{
		LOG_ERR2("error: pthread_create", error);
	}
}

void threadJoin( Thread *t )
{
	void *ret;

	pthread_join( *t, &ret);

}

bool threadSetPriority( Thread * t, ThreadPriority p)
{
    int current_policy;
    struct sched_param current_param;
    int ans;
    if (pthread_getschedparam( *t , &current_policy, &current_param))
    {
        return false;
    }
    
    switch(p)
    {
        case THREAD_PRIORITY_REALTIME:
            current_policy = SCHED_RR;
            break;
        case THREAD_PRIORITY_HIGH:
            current_policy = SCHED_RR;
            break;
        case THREAD_PRIORITY_NORMAL:
            current_policy = SCHED_OTHER;
            break;
    }
    
    current_param.sched_priority = current_policy;
    if ( (ans = pthread_setschedparam( *t, current_policy, &current_param)) )
    {
        return false;
    }
    return  true;
}

bool threadTerminate( Thread * t )
{
    return pthread_cancel( *t )==0? true: false;
}

void threadExit( void )
{
	 pthread_exit(0);
}


