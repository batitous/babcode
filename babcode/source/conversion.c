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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>


bool stringToInt(Int8 const * str, Int32 * num)
{
    unsigned int pos;
    unsigned int i;
    int val;
    unsigned int pval;
	int neg = 0;

    if (!str || !num)
        return false;
    for (pos=0; str[pos]==' ' || str[pos]=='\t'; pos++);
    if (str[pos]=='+')
        pos++;

	if(str[pos]=='-')
	{
		neg = 1;
		pos++;
	}

    val = 0;
    for (i=pos; str[i]!='\0'; i++)
    {
	pval = val;
        val *= 10;
	if (val/10!=pval) /* check for overflow */
	    return false;

	pval = val;
	switch (str[i])
	{
	case '0':
	    break;
	case '1':
	    val += 1;
	    break;
	case '2':
	    val += 2;
	    break;
	case '3':
	    val += 3;
	    break;
	case '4':
	    val += 4;
	    break;
	case '5':
	    val += 5;
	    break;
	case '6':
	    val += 6;
	    break;
	case '7':
	    val += 7;
	    break;
	case '8':
	    val += 8;
	    break;
	case '9':
	    val += 9;
	    break;
	default:
	    return false;
	}
	if (val<pval) /* check for overflow */
	    return false;
    }

	if (neg==1)
		val = -val;

    *num = val;
    return true;
}

UInt32 binToDecimalAscii(Int32 bin,Int8 *result)
{
	int i=0;
	char bCount, bPrinted;
	UInt32 lTmp,lDigit;

	bPrinted = 0;
	if(bin < 0)
	{
		bin = -bin;
		result[i++]='-';
	}

	lDigit = 1000000000L;
	for(bCount = 0; bCount < 9; bCount++)
	{
		lTmp = (char)(bin/lDigit);
		if(lTmp)
		{
			result[i++]=(char)(lTmp)+'0';
			bPrinted = 1;
		}
		else if(bPrinted)
		{
			result[i++]=(char)lTmp+'0';
		}
		bin -= ((long)lTmp)*lDigit;
		lDigit = lDigit/10;
	}

	lTmp = (char)(bin/lDigit);

	result[i++]=(char)lTmp+'0';

	return i;
}

void binToHex(UInt8 data,Int8 *result)
{
	UInt8 temp;

	temp = ((data >> 4) & 0x0f) + '0';
	if(temp>'9')
		temp += 7;

	result[0]=temp;

	temp = (data&0x0f) + '0';
	if(temp>'9')
		temp += 7;

	result[1]=temp;
}

bool hexToBin(UInt8 *hex,UInt8 *bin)
{
	unsigned char i;
	unsigned char tmp=0;

	for(i=0;i<2;i++)
	{
		if(hex[i] >= '0' && hex[i] <= '9' )
			tmp += hex[i] - '0';
		else if(hex[i] >= 'A' && hex[i] <= 'F')
			tmp += hex[i] - 55; //65 - ... = 10
		else if(hex[i] >= 'a' && hex[i] <= 'f')
			tmp += hex[i] - 87; //97 - ... = 10
		else
			return false;

		if(i==0)
			tmp = tmp << 4;
	}

	*bin = tmp;

	return true;
}

bool stringToFloat(Int8 * str, float* floating)
{
	int i;
	int len;
	int divider=1;
    int integer=0;
	int decimal=0;
    
    char* pos = strchr(str, '.');
    if (pos == NULL && stringToInt(str, &integer) == 0) {
        *floating=integer;
        return true;
    }
    else if (pos == NULL && stringToInt(str, &integer) != 0) {
        return false;
    }
    
    pos[0]='\0';
    
    if (stringToInt(str, &integer) != 0) return false;
    if (stringToInt(pos+1, &decimal) != 0) return false;
    
    
    len = (int)strlen(pos+1);
    for (i=0; i<len; i++)
        divider*=10;
    *floating = (float)decimal/(float)divider;
    *floating += integer;
    
    return true;
}

bool stringIsHex(Int8 * str, Int32 * integer)
{
	long size;
	char* pEnd=NULL;
	int val;
	long i;

    if (str[0] != '0') return false;
    if (str[1] != 'x') return false;
    
    size=strlen(str);
    for (i=2; i<size; i++)
    {
        if (str[i] < '0' || str[i] > '9')
            if (str[i] < 'A' || str[i] > 'F')
                if (str[i] < 'a' || str[i] > 'f')
                    return false;
    }
    
    val = (int)strtol(str, &pEnd, 16);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
        return false;
    
    *integer = val;
    
    return true;
}

