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

#include "../include/babextended.h"

#include <string.h>

#define BUFFER_SIZE         2048
#define LOG_FREQUENCY_HZ    30

#if PLATFORM==PLATFORM_MAC
    #ifdef __cplusplus
    extern "C" {
    #endif
        extern void logSetStream(FILE * file);
    #ifdef __cplusplus
    }
    #endif
#else
    extern void logSetStream(FILE * file);
#endif


Log * Log::defaultLog = new Log();


Log *Log::global()
{
    return defaultLog;
}

Log::Log()
{
    mutexInit(&mMutex);
}

Log::~Log()
{
    
}

void Log::start(const char * filename, bool enableOutput)
{
    mPrintOnOutput = enableOutput;
    mFile = fopen(filename,"w+");
    if (mFile==NULL)
    {
        return;
    }
    
    // link old log from babcode to the new log
    logSetStream(mFile);
    
    mQueue = new ByteArrayQueue(128, BUFFER_SIZE);
    
    
    mThreadRun = true;
    threadInit(&mThread, privateThreadStarter, this);
    
}

void Log::stop()
{
    mThreadRun = false;
    if (mFile!=NULL)
    {
        fflush(mFile);
        fclose(mFile);
        mFile=NULL;
    }
}

void Log::addError(const char fmt[], va_list args)
{
    char buffer[BUFFER_SIZE];
    
    memset(buffer, 0, BUFFER_SIZE);
    
    int prefix = snprintf(buffer, BUFFER_SIZE, "[%8.3f] [ERROR] ", getTicks()/1000.0);
    int size = vsnprintf(buffer+prefix, BUFFER_SIZE - prefix,fmt,args);
    
    int total = size+prefix;
    
    if (size > 0 && total < BUFFER_SIZE)
    {
        mutexLock(&mMutex);
        mQueue->write((uint8_t *)buffer, BUFFER_SIZE);
        mutexUnlock(&mMutex);
    }
}


void Log::addInfo(const char fmt[], va_list args)
{
    char buffer[BUFFER_SIZE];
    
    memset(buffer, 0, BUFFER_SIZE);
    
    int prefix = snprintf(buffer, BUFFER_SIZE, "[%8.3f] [INFO] ", getTicks()/1000.0);
    int size = vsnprintf(buffer+prefix, BUFFER_SIZE - prefix,fmt,args);
    
    int total = size+prefix;
    
    if (size > 0 && total < BUFFER_SIZE)
    {
        mutexLock(&mMutex);
        mQueue->write((uint8_t *)buffer, BUFFER_SIZE);
        mutexUnlock(&mMutex);
    }
}

void Log::error(const char fmt[], ...)
{
    va_list args;
    va_start(args, fmt);
    addError(fmt, args);
    va_end(args);
}

void Log::info(const char fmt[], ...)
{
    va_list args;
    va_start(args, fmt);
    addInfo(fmt, args);
    va_end(args);
}

void Log::privateThreadRunner(void)
{
    uint32_t begin;
    uint32_t end;
    
    uint8_t * buffer;

    bool writeSomething;
    
    while (mThreadRun==true)
    {
        begin = getTicks();
        
        writeSomething = false;
        
        mutexLock(&mMutex);
        buffer = mQueue->read();
        mutexUnlock(&mMutex);
        
        while (buffer!=0)
        {
            int size = (int)strnlen((char *)buffer, BUFFER_SIZE);
            if (size < BUFFER_SIZE)
            {
                fwrite(buffer, size, 1, mFile);
            
                if (mPrintOnOutput==true)
                {
                    printf("%s", buffer);
                }
            
                writeSomething = true;
            }
            
            mutexLock(&mMutex);
            buffer = mQueue->read();
            mutexUnlock(&mMutex);
        }
        
        
        if (writeSomething==true)
        {
            fflush(mFile);
        }
        
        
        end = getTicks();
        
        int32_t sleepTime = 1000 / LOG_FREQUENCY_HZ - (end-begin);
        if (sleepTime <= 0)
        {
            sleepTime = 0;
        }
        
        waitMs(sleepTime);
        
    }
    
}

void * Log::privateThreadStarter(void * parameter)
{
    Log * object = (Log *)parameter;
    object->privateThreadRunner();
    threadExit();
    return 0;
}


