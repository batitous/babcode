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

#include <windows.h>
#include <stdio.h>


static uint32_t getComPortNumber(const char *portcom)
{
	uint32_t i,j;
	char buffer[4];
    int32_t port;

	i=0;
	while(portcom[i]!='M')
	{
		i++;
	}
	i++;

	j=0;
	while(portcom[i]!=0)
	{
		buffer[j] = portcom[i];
		j++;
		i++;
	}

	buffer[j] = 0;

	stringToInt(buffer,&port);

	return port;
}

uint32_t initUART(Uart * uart, const char * name, uint32_t baudrate)
{
    HANDLE hCom ;
    
	uint32_t comport;
	DCB dcb_structure;
	COMMTIMEOUTS commtimeouts;

	char port[24];

	comport = getComPortNumber(name);

	sprintf(port,"\\\\.\\COM%d",comport);


	hCom=CreateFileA(port,GENERIC_WRITE|GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hCom == INVALID_HANDLE_VALUE)
	{
		return UART_OPEN_FAILED ;
	}

	GetCommState (hCom,&dcb_structure);

	//configuration du port
	dcb_structure.BaudRate=baudrate ; //115200;
	dcb_structure.ByteSize=8;
	dcb_structure.StopBits=ONESTOPBIT;
	dcb_structure.Parity=NOPARITY;
	dcb_structure.fDtrControl=DTR_CONTROL_DISABLE;
	dcb_structure.fRtsControl=RTS_CONTROL_DISABLE ;
	dcb_structure.fOutxCtsFlow =FALSE;
	dcb_structure.fOutxDsrFlow =FALSE;

	if( SetCommState (hCom,&dcb_structure) == 0 )
	{
		return UART_CONF_FAILED ;
	}


	commtimeouts.ReadIntervalTimeout         =MAXDWORD;
    commtimeouts.ReadTotalTimeoutMultiplier  =0;
    commtimeouts.ReadTotalTimeoutConstant    =1;
    commtimeouts.WriteTotalTimeoutMultiplier =0;
    commtimeouts.WriteTotalTimeoutConstant   =0;
    SetCommTimeouts(hCom, &commtimeouts);

	PurgeComm(hCom,PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT  | PURGE_TXABORT );

    uart->handle = hCom;
    
	return UART_OK ;
}


uint32_t sendBufferToUART (Uart * uart, const uint8_t *Buffer, uint32_t Count)
{
	unsigned long buffer;
//	FlushFileBuffers(hCom);
	if (!WriteFile(uart->handle,Buffer,Count,&buffer,NULL))
	{
		return UART_WRITE_FAILED;
	}
	FlushFileBuffers(uart->handle);
	return UART_OK;
}

uint32_t sendByteToUART(Uart * uart, uint8_t byte)
{
	unsigned long buffer;
//	FlushFileBuffers(hCom);
	if (!WriteFile(uart->handle,&byte,1,&buffer,NULL))
	{
		return UART_WRITE_FAILED;
	}
	FlushFileBuffers(uart->handle);
	return UART_OK;
}


uint32_t getByteFromUART (Uart * uart, uint8_t *Val)
{
	unsigned long buffer;

	if(ReadFile(uart->handle,Val,1,&buffer,NULL)==0)
	{
		return UART_READ_FAILED;
	}
//	FlushFileBuffers(hCom);
	return UART_OK;
}

uint32_t getByteFromUARTNoWait(Uart * uart, uint8_t *mot)
{
	unsigned long len=0;
	unsigned long time=0;

	do
	{
		ReadFile (uart->handle,mot,1,&len,NULL);
		if (!len) time++;

		if (time>50) return UART_TIMEOUT_ERROR;

	}while(!len);

	return UART_OK;
}

uint32_t getBufferFromUART (Uart * uart, uint8_t *Buffer,uint32_t Count)
{
	unsigned long buffer;
	if(ReadFile(uart->handle,Buffer,Count,&buffer,NULL)==0)
	{
		return UART_READ_FAILED;
	}

//	FlushFileBuffers(hCom);

	return UART_OK;
}


void closeUART(Uart * uart)
{
	CloseHandle(uart->handle);
}

