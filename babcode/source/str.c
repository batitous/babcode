// Copyright (c) 2013, Sylvain Fay-Chatelard
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
// THIS SOFTWARE IS PROVIDED BY Sylvain Fay-Chatelard AND CONTRIBUTORS ``AS IS'' AND ANY
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Int8 ** stringSplit(Int8 *str, Int8 *token)
{
	char *saveptr, *ptr, *start;
	long length, i;
	char** dst=NULL;
    
	if (!(saveptr=strdup(str)))
		return NULL;
	
	length=strlen(token);
	i=0;
	
	for (start=saveptr,ptr=start;;ptr++) {
		if (!strncmp(ptr,token,length) || !*ptr) {
			while (!strncmp(ptr,token,length)) {
				*ptr=0;
				ptr+=length;
			}
			
			if (!(dst=(char **)realloc(dst,(i+2)*sizeof(char*))))
            {
				return 0;
            }
			dst[i]=start;
			dst[i+1]=NULL;
			i++;
			
			if (!*ptr)
				break;
			start=ptr;
		}
	}
	return dst;
}

Int8* stringDuplicate(Int8* src, Int32 size)
{
	char* str = (char*)calloc(size+1, sizeof(char));
	
	memcpy(str, src, size);
    
	str[size] = 0;
    
	return str;
}

Int8* stringReplace(const Int8 *string, const Int8 *substr, const Int8 *replacement)
{
    char *newstr = NULL;
    
    const char * tok = strstr(string, (const char *)substr);
    if(tok == NULL) return strdup(string);
    newstr = (char *)calloc(strlen(string) - strlen(substr) + strlen(replacement) + 1, sizeof(char));
    if(newstr == NULL) return NULL;
    memcpy(newstr, string, tok - string);
    memcpy(newstr + (tok - string), replacement, strlen(replacement));
    memcpy(newstr + (tok - string) + strlen(replacement), tok + strlen(substr), strlen(string) - strlen(substr) - (tok - string));
    memset(newstr + strlen(string) - strlen(substr) + strlen(replacement), 0, 1);
    return newstr;
}

Int8 * stringInsert(const Int8 *string, const Int8 *substr, const Int32 offset)
{
	char *newstr = (char*)calloc(strlen(string) + strlen(substr) + 2, sizeof(char));
	if(newstr == NULL) return NULL;
    memcpy(newstr, string, offset);
    memcpy(newstr + offset, substr, strlen(substr));
    memcpy(newstr + offset + strlen(substr), string + offset, strlen(string)-offset + 1);
    return newstr;
}
