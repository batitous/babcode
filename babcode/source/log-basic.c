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

#include "../include/babcode.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static FILE * global_log_file = NULL;


static void log_add(const char fmt[], va_list args)
{
	Time time;

    if (global_log_file==NULL)
        return;
    
    getTime(&time);
    
    fprintf( global_log_file,"%02u:%02u:%02u ",time.hour,time.minute,time.second);
    vfprintf(global_log_file,fmt,args);
    
    fflush(global_log_file);
}

void logSetStream(FILE * file)
{
    global_log_file = file;
}

void logOpen(const char * filename)
{
    global_log_file = fopen(filename,"w+");
	if (global_log_file==0)
		return;
}

void logClose(void)
{
    if (global_log_file!=NULL)
    {
        fflush(global_log_file);
        fclose(global_log_file);
        global_log_file=NULL;
    }
}

void logError (const char fmt[], ...)
{
    va_list args;
    va_start(args, fmt);
    log_add(fmt, args);
    va_end(args);
}
