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

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>



static struct termios gOriginalTTYAttrs;


uint32_t initUART(Uart * uart, const char *tty_name, uint32_t baudrate)
{
	struct termios  options;
	int fd_uart;
	
	fd_uart = open((char *)tty_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
	
    if (fd_uart < 0)
    {
        LOG_BASIC("error: open uart %s\n", tty_name);
        return UART_OPEN_FAILED;
    }
	
    //DebugPrintf(3, "COM-Port %s opened...\n", (int8_t *)tty_name);
	
    /* clear input & output buffers, then switch to "blocking mode" */
    tcflush(fd_uart, TCOFLUSH);
    tcflush(fd_uart, TCIFLUSH);
    fcntl(fd_uart, F_SETFL, fcntl(fd_uart, F_GETFL) & ~O_NONBLOCK);
	
    tcgetattr(fd_uart, &gOriginalTTYAttrs); /* save current port settings */
	
    bzero(&options, sizeof(options));
    options.c_cflag = CS8 | CLOCAL | CREAD;
	
    options.c_iflag = IGNPAR | IGNBRK | IXON | IXOFF;
    options.c_oflag = 0;
	
    /* set input mode (non-canonical, no echo,...) */
    options.c_lflag = 0;
	
    cfmakeraw(&options);
    options.c_cc[VTIME]    = 1;   /* inter-character timer used */
    options.c_cc[VMIN]     = 0;   /* blocking read until 0 chars received */
	
	cfsetspeed(&options, uint32_tToSpeed_t(baudrate));
	
    tcflush(fd_uart, TCIFLUSH);
    if(tcsetattr(fd_uart, TCSANOW, &options))
    {
		LOG_ERR1("could not change uart port behaviour (wrong baudrate?)");
		return UART_CONF_FAILED;
    }
    
    uart->handle = fd_uart;
    
	return UART_OK;
}

speed_t uint32_tToSpeed_t(uint32_t baudrate)
{
	switch (baudrate)
    {
#ifdef B1152000
		case 1152000: return B1152000; break;
#endif // B1152000
#ifdef B576000
		case  576000: return B576000; break;
#endif // B576000
#ifdef B230400
		case  230400: return B230400; break;
#endif // B230400
#ifdef B115200
		case  115200: return B115200; break;
#endif // B115200
#ifdef B57600
		case   57600: return B57600; break;
#endif // B57600
#ifdef B38400
		case   38400: return B38400; break;
#endif // B38400
#ifdef B19200
		case   19200: return B19200; break;
#endif // B19200
#ifdef B9600
		case    9600: return B9600; break;
#endif // B9600
		default: return B115200; break;
    }
}

uint32_t sendByteToUART(Uart * uart, uint8_t Byte)
{
    uint8_t tmp0[1];
    
	tmp0[0]=Byte;

	if(write(uart->handle,tmp0,1)==-1)
	{
		return UART_WRITE_FAILED;
	}
	return UART_OK;
}

uint32_t sendBufferToUART(Uart * uart, const uint8_t *Buffer, uint32_t Count)
{
	if(write(uart->handle,Buffer,Count)==-1)
	{
		return UART_WRITE_FAILED;
	}
	return UART_OK;
}

uint32_t getByteFromUART(Uart * uart, uint8_t *data)
{
    uint8_t tmp1[1];
    
	if(read(uart->handle,tmp1,1)==-1)
	{
		return UART_READ_FAILED;
	}

	*data = tmp1[0];

	return UART_OK;
}

uint32_t getByteFromUARTNoWait(Uart * uart, uint8_t *data)
{
    uint8_t tmp1[1];
    
	*data=0;
	
    int32_t result = (int32_t)read(uart->handle,tmp1,1);
	if (result == 0) {
		return UART_TIMEOUT_ERROR;
	}
	else if(result==-1)
	{
		if (errno == EAGAIN) {
			return UART_TIMEOUT_ERROR;
		}
		else {
			return UART_READ_FAILED;
		}
	}
	
	*data = tmp1[0];
	
	return UART_OK;
}

uint32_t getBufferFromUART(Uart * uart, uint8_t *Buffer, uint32_t Count)
{
	if(read(uart->handle,Buffer,Count)==-1)
	{
		return UART_READ_FAILED;
	}
	
	return UART_OK;
}

void closeUART(Uart * uart )
{
    close(uart->handle);
    
	tcflush(uart->handle, TCOFLUSH);
    tcflush(uart->handle, TCIFLUSH);
    tcsetattr(uart->handle, TCSANOW, &gOriginalTTYAttrs);
}



