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
#include <malloc.h>
#include <windows.h>

uint32_t fileWrite(const char *filename, uint8_t *buffer, uint32_t size )
{
	FILE *file ;
	unsigned int result ;

	/* MSDN file = CreateFile(TEXT("myfile.txt"),     // file to create
                   GENERIC_WRITE,          // open for writing
                   0,                      // do not share
                   NULL,                   // default security
                   CREATE_ALWAYS,          // overwrite existing
                   FILE_ATTRIBUTE_NORMAL | // normal file
                   FILE_FLAG_OVERLAPPED,   // asynchronous I/O
                   NULL);   */

	file = fopen( (const char *)filename, "wb" );
	if( file == NULL )
	{
        LOG("error open file %s !\n", filename );
		return FILE_OPEN_ERROR ;
	}

	result = fwrite(buffer,sizeof(uint8_t),size,file);
	if( result != size )
	{
		LOG("I/O Error (write : %d demand : %d) !\n",result,size);
		return FILE_IO_ERROR;
	}

	fclose( file );

	return FILE_OK ;
}

uint8_t * fileRead( const char *filename, uint32_t *len )
{
	uint32_t size = 0 ;
	uint8_t *buffer = 0 ;
	HANDLE f ;

	f = CreateFileA((LPCSTR)filename,GENERIC_READ,FILE_SHARE_READ,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);

	if( f == INVALID_HANDLE_VALUE )
	{
		LOG("Error to open file %s!\n",filename);
		return NULL ;
	}

	if( (size = GetFileSize(f,NULL)) == INVALID_FILE_SIZE )
	{
		LOG_ERR1("get file size");
		return NULL ;
	}

	//allow the size to a target buffer
	buffer =(uint8_t *) malloc(size);
	if( buffer == NULL )
	{
		LOG_ERR1("malloc");
		return NULL ;
	}

	if( ReadFile(f,buffer,size,(LPDWORD)len,NULL) == 0 )
	{
		LOG_ERR1("read");
		return NULL ;
	}

	CloseHandle(f);

	return buffer ;
}

uint8_t * fileMmapRead( const char * path, uint32_t *len)
{
   void    *result;
   HANDLE   file=CreateFileA((LPCSTR)path,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
   HANDLE   mapping;

   if(file==INVALID_HANDLE_VALUE)
    return 0;

   *len=GetFileSize(file,NULL);
   if(*len==0xFFFFFFFF)
   {
	   CloseHandle(file);
	   return 0;
   }
   if(*len==0)
   {
	   return 0;
   }
   mapping=CreateFileMapping(file,NULL,PAGE_READONLY,0,0,NULL);
   if(mapping==NULL)
   {
	   CloseHandle(file);
	   return 0;
   }

   result=MapViewOfFile(mapping,FILE_MAP_READ,0,0,0);
   CloseHandle(mapping);
   CloseHandle(file);

   return (uint8_t *)result;
}

uint8_t * fileReadAtIndex( const char *filename, uint32_t index, uint32_t *len_to_read )
{
	FILE *f;
	uint8_t *buffer;
	uint32_t size = 0 ;

	//open the file
	f = fopen( (const char *)filename , "rb");
	if( f == NULL )
	{
		LOG("Error to open file %s!\n",filename);
		return NULL ;
	}

	buffer = (uint8_t *)malloc( *len_to_read);
	if( buffer == NULL )
	{
		LOG_ERR1("malloc");
		return NULL ;
	}

	if( fseek(f,index,SEEK_SET) != 0 )
	{
		LOG_ERR1("fseek");
		return NULL ;
	}


	//read the file and put the data in the target buffer
	size = fread( buffer, *len_to_read ,sizeof(unsigned char),f);

	fclose(f);

	*len_to_read = size ;

	return buffer ;
}
