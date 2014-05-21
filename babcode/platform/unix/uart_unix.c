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

#include <sys/ioctl.h>


uint32_t UARTGetStatus(Uart * uart)
{
    int status;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    return status;
}

void UARTClearDTR(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    status |= TIOCM_DTR;
    
    ioctl(uart->handle, TIOCMSET, &status);
}

void UARTSetDTR(Uart * uart)
{
    uint32_t status = 0 ;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    status &= ~TIOCM_DTR;
    
    ioctl(uart->handle, TIOCMSET, &status);
}

void UARTClearRTS(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    status |= TIOCM_RTS;
    
    ioctl(uart->handle, TIOCMSET, &status);
}

void UARTSetRTS(Uart * uart)
{
    uint32_t status = 0 ;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    status &= ~TIOCM_RTS;
    
    ioctl(uart->handle, TIOCMSET, &status);
}



uint8_t UARTisDSRset(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    if(status & TIOCM_DSR)
        return 0;
    else
        return 1;
}

uint8_t UARTisDTRset(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    if(status & TIOCM_DTR)
        return 0;
    else
        return 1;
}

uint8_t UARTisRTSset(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    if(status & TIOCM_RTS)
        return 0;
    else
        return 1;
}

uint8_t UARTisCTSset(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    if(status & TIOCM_CTS)
        return 0;
    else
        return 1;
}

uint8_t UARTisDCDset(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    if(status & TIOCM_CAR)
        return 0;
    else
        return 1;
}

uint8_t UARTisRNGset(Uart * uart)
{
    uint32_t status = 0;
    
    ioctl(uart->handle, TIOCMGET, &status);
    
    if(status & TIOCM_RNG)
        return 0;
    else
        return 1;
}
