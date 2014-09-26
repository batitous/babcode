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

#include <string.h>

#if PLATFORM == PLATFORM_WINDOWS
#	include <direct.h>
#	define	getcwd	_getcwd
#endif

#if	PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#	include <unistd.h>
#endif


uint16_t crc16_adjust(uint16_t crc16, uint8_t data)
{
    for(uint32_t n = 0; n < 8; n++)
    {
        if((crc16 & 1) ^ (data & 1))
        {
            crc16 = (crc16 >> 1) ^ 0x8408;
        }
        else
        {
            crc16 >>= 1;
        }
        
        data >>= 1;
    }
    
    return crc16;
}

uint16_t crc16Compute(const uint8_t* data, uint32_t length)
{
    uint16_t crc16 = ~0;

    for(uint32_t n = 0; n < length; n++)
    {
        crc16 = crc16_adjust(crc16, data[n]);
    }
    
    return ~crc16;
}


void write32bitsToBuffer(uint8_t * buffer, uint32_t integer)
{
    buffer[0] = (uint8_t)(integer & 0xFF);
    buffer[1] = (uint8_t)((integer>>8UL) & 0xFF);
    buffer[2] = (uint8_t)((integer>>16UL) & 0xFF);
    buffer[3] = (uint8_t)((integer>>24UL) & 0xFF);
}

uint32_t read32bitsFromBuffer(uint8_t * buffer)
{
    uint32_t integer;
    uint32_t temp;
    
    temp = buffer[3];
    integer = temp << 24UL;
    
    temp = buffer[2];
    integer = (temp << 16UL) | integer;
    
    temp = buffer[1];
    integer = (temp << 8UL) | integer;
    
    temp = buffer[0];
    integer = (temp) | integer;
    
    return integer;
}

void write16bitsToBuffer(uint8_t * buffer, uint16_t integer)
{
    buffer[0] = (uint8_t)(integer);
    buffer[1] = (integer>>8UL);
}

uint16_t read16bitsFromBuffer(uint8_t * buffer)
{
    uint16_t integer = (buffer[1]<<8UL)| (buffer[0]);
    
    return integer;
}

#if PLATFORM == PLATFORM_WINDOWS

#   define DEFAULT_SLASH					'/'
#   define DEFAULT_DIR_SEPARATOR			';'
#   define IS_SLASH(c)						((c) == '/' || (c) == '\\')
#   define IS_ABSOLUTE_PATH(path, len)		(len >= 2 && ((isalpha(path[0]) && path[1] == ':')))

#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX

#   define DEFAULT_SLASH					'/'
#   define DEFAULT_DIR_SEPARATOR			':'
#   define IS_SLASH(c)						((c) == '/')
#   define IS_ABSOLUTE_PATH(path, len)		(IS_SLASH(path[0]))

#endif

static int _Path_Cleanup(char *AbsPath)
{
	char *pBufWrite, *pBuf;
	int SlashFound = 0;
	
	
	pBuf = &AbsPath[0];
	pBufWrite = pBuf;
	while(*pBuf)
	{
		if(IS_SLASH(*pBuf))
		{
			if(SlashFound)
			{
				pBuf++;
				continue;
			}
			SlashFound = 1;
			*pBufWrite++ = DEFAULT_SLASH;
		}
		else
		{
			SlashFound = 0;
			
			if(pBufWrite != pBuf)
				*pBufWrite = *pBuf;
			pBufWrite++;
		}
		
		pBuf++;
	}
	*pBufWrite = 0;
	
	return (int)(pBufWrite - &AbsPath[0]);
}


bool getRealPath(const char *path, char *resolvedPath, int32_t * resolvePathSize)
{
	char AbsPath[512], *Tokens[512], *pBuf;
	int i, lPath, lAbsPath, lTokens, lResolvedPath, szResolvedPath;
	
	
	if(!path || !resolvedPath || !resolvePathSize)
	{
		return false;
	}
	
	szResolvedPath = *resolvePathSize;
	lPath = (int)strlen(path);
	
	if(IS_ABSOLUTE_PATH(path, lPath))
	{
		// copy string
		
		if(lPath >= szResolvedPath - 1)
			return false;
		
		memcpy(AbsPath, path, lPath);
		AbsPath[lPath] = 0;
	}
	else
	{
		//	copy cwd then concatenate string
		if(!getcwd(AbsPath, sizeof(AbsPath)))
			return false;
		
		lAbsPath = (int)strlen(AbsPath);
		if(lAbsPath + lPath + 1 >= sizeof(AbsPath))
			return false;
		
		AbsPath[lAbsPath] = DEFAULT_SLASH;
		memcpy(&AbsPath[lAbsPath + 1], path, lPath + 1);
		lAbsPath += 1 + lPath;
	}
	
	// override default slash, and drop duplicate
	lAbsPath = _Path_Cleanup(AbsPath);
	
	if(lAbsPath >= szResolvedPath - 1)
		return false;
	
	//printf("[%s]\n", AbsPath);
	
	// build tree
	{
		lTokens = 0;
		pBuf = &AbsPath[0];
		memset(Tokens, 0, sizeof(Tokens));
		for(i=0;i<lAbsPath;i++)
		{
			if(IS_SLASH(AbsPath[i]))
			{
				Tokens[lTokens++] = pBuf;
				AbsPath[i] = 0;
				pBuf = &AbsPath[i + 1];
			}
		}
		Tokens[lTokens++] = pBuf;
	}
	
	/*
     for(i=0;i<lTokens;i++)
     {
     printf("[%s]", Tokens[i]);
     }
     printf("\n");
     */
	
	// simplify tree
	// skip first token (root), should check for error
	for(i=1;i<lTokens;i++)
	{
		/*
         int k;
         printf("(%d/%d) ", i, lTokens);
         for(k=0;k<lTokens;k++)
         {
         if(k != i)
         printf("[%s]", Tokens[k]);
         else
         printf(">%s<", Tokens[k]);
         }
         printf("\n");
         */
		
		// throw "."
		if(strcmp(Tokens[i], ".") == 0)
		{
			// shift down
			memmove(&Tokens[i], &Tokens[i + 1], (lTokens - i - 1) * sizeof(char*));
			lTokens--;
			i--;
			continue;
		}
		
		if(strcmp(Tokens[i], "..") == 0)
		{
			if(i <= 1)
			{
				// .. conducts to /, just shift all > 1 to 0 offset
				// shift down
				memmove(&Tokens[i], &Tokens[i + 1], (lTokens - i - 1) * sizeof(char*));
				lTokens--;
				i--;
				continue;
			}
			else
			{
				// shift all > i to i-1 offset
				//printf("move %d tokens from %d to %d\n", lTokens - i - 1, i + 1, i - 1);
				memmove(&Tokens[i - 1], &Tokens[i + 1], (lTokens - i - 1) * sizeof(char*));
				lTokens -= 2;
				i -= 2;
				continue;
			}
		}
	}
	
	/*
     for(i=0;i<lTokens;i++)
     {
     printf("[%s]", Tokens[i]);
     }
     printf("\n");
     */
	
	// rebuild string
	{
		int lToken;
		
		
		pBuf = &resolvedPath[0];
		for(i=0;i<lTokens;i++)
		{
			lToken = (int)strlen(Tokens[i]);
			
			memcpy(pBuf, Tokens[i], lToken);
			pBuf += lToken;
			*pBuf++ = DEFAULT_SLASH;
		}
		// remove trailing '/', only if Tokens[0] is not empty (linux) and lTokens > 0 (at least 1 folder)
		if(lTokens && Tokens[0][0])
			pBuf[-1] = 0;
		else
			*pBuf = 0;
	}
	
	//printf("[%s]\n", ResolvedPath);
	// cleanup again
	lResolvedPath = _Path_Cleanup(resolvedPath);
	*resolvePathSize = lResolvedPath;
	
	return true;
}


