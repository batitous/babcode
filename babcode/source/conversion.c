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

#include "../include/babcode.h"

#include <stdio.h>
#include <stdlib.h>


Bool String2Int(Int8 const * str, Int32 * num)
{
    unsigned int pos;
    unsigned int i;
    int val;
    unsigned int pval;
	int neg = 0;

    if (!str || !num)
        return -1;
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
	    return False;

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
	    return -1;
	}
	if (val<pval) /* check for overflow */
	    return False;
    }

	if (neg==1)
		val = -val;

    *num = val;
    return True;
}

UInt32 Bin2DecimalAscii(Int32 bin,Int8 *result)
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

void Bin2Hex(UInt8 data,Int8 *result)
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

Bool Hex2Bin(UInt8 *hex,UInt8 *bin)
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
			return False;

		if(i==0)
			tmp = tmp << 4;
	}

	*bin = tmp;

	return True;
}

