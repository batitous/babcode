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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>


UInt32 FileWrite(UInt8 *filename, UInt8 *buffer, UInt32 size )
{
	int file ;
	UInt32 result ;

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	file = open( (Int8 *)filename, O_WRONLY | O_CREAT | O_TRUNC, mode );
	if( file == -1 )
	{
		LOG("error: open file %s !\n", filename);
		return FILE_OPEN_ERROR ;
	}

	result = (UInt32)write(file,  buffer, size);
	if( result != size )
	{
		LOG("error: write (%d writed instead of %ld).\n",result,size);
		return FILE_IO_ERROR;
	}

	close( file );

	return FILE_OK ;
}

static long int GetFileSize( int fp )
{
    long int save_pos, size_of_file;

    save_pos = lseek(fp,0,SEEK_CUR);

	lseek( fp, 0L, SEEK_SET );

	size_of_file = lseek(fp, 0L,SEEK_END);

	lseek( fp, save_pos, SEEK_SET);

	return( size_of_file );
}


UInt8 *FileMmapRead(UInt8 *filename, UInt32 *len)
{
    UInt8 * ptr;
    int fileDescriptor;
    struct stat statInfo;

    // from apple docs
    
    ptr = NULL;
    *len = 0;
    
    // Open the file.
    fileDescriptor = open( (char *)filename, O_RDONLY, 0 );
    if( fileDescriptor < 0 )
    {
        LOG("error: open file %s\n", filename );
        return NULL;
    }
    else
    {
        // We now know the file exists. Retrieve the file size.
        if( fstat( fileDescriptor, &statInfo ) != 0 )
        {
           return NULL;
        }
        else
        {
            // Map the file into a read-only memory region.
            ptr = mmap(NULL,statInfo.st_size,PROT_READ,MAP_SHARED,fileDescriptor,0);
            if( ptr == MAP_FAILED )
            {
                LOG("error: mmap file %s\n", filename );
                return NULL;
            }
            else
            {
                // On success, return the size of the mapped file.
                *len = (UInt32)statInfo.st_size;
            }
        }
        
        // Now close the file. The kernel doesn’t use our file descriptor.
        close( fileDescriptor );
    }
   
    return ptr;
}

UInt8 *FileRead( UInt8 *filename, UInt32 *len )
{
	UInt32 size ;
	UInt8 *buffer ;
	int file ;

	//open the file
	file = open((Int8 *)filename,O_RDONLY);
	if( file == -1 )
	{
		LOG("error: open file %s\n", filename );
		return NULL ;
	}

	//get its size
	size=(UInt32)GetFileSize(file);

	//alloc the size to a target buffer
	buffer =(UInt8*) malloc(size);
	if( buffer == NULL )
	{
		LOG("error: malloc failed !\n");
		return NULL ;
	}

	//read the file and put the data in the target buffer
	if( read( file, buffer, size ) == -1 )
	{
		LOG("error: read failed\n");
		return NULL ;
	}

	//close the file
	close(file);

	*len = size ;
	return buffer;
}

UInt8 *FileReadAtIndex( UInt8 *filename, UInt32 index, UInt32 *len_to_read )
{
	int f;
	UInt8 *buffer;
	UInt32 size = 0 ;

	//open the file
	f = open( (Int8 *)filename , O_RDONLY);
	if( f == -1 )
	{
		LOG("error: open file %s!\n",filename);
		return NULL ;
	}

	buffer = (UInt8 *)malloc( *len_to_read);
	if( buffer == NULL )
	{
		LOG("error: malloc failed");
		return NULL ;
	}

	if(lseek(f,index,SEEK_SET) == -1 )
	{
		LOG("error: lseek failed\n");
		return NULL ;
	}

	//read the file and put the data in the target buffer
	if( (size = (UInt32)read( f, buffer, *len_to_read )) == -1 )
	{
		LOG("error: read failed\n");
		return NULL ;
	}

	close(f);

	*len_to_read = size ;

	return buffer ;
}

