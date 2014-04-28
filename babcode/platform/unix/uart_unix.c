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


extern int get_fd_uart(void);

uint32_t UARTGetStatus()
{
    int status;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    return status;
}

void UARTClearDTR()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    status |= TIOCM_DTR;
    
    ioctl(get_fd_uart(), TIOCMSET, &status);
}

void UARTSetDTR()
{
    uint32_t status = 0 ;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    status &= ~TIOCM_DTR;
    
    ioctl(get_fd_uart(), TIOCMSET, &status);
}

void UARTClearRTS()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    status |= TIOCM_RTS;
    
    ioctl(get_fd_uart(), TIOCMSET, &status);
}

void UARTSetRTS()
{
    uint32_t status = 0 ;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    status &= ~TIOCM_RTS;
    
    ioctl(get_fd_uart(), TIOCMSET, &status);
}



uint8_t UARTisDSRset()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    if(status & TIOCM_DSR)
        return 0;
    else
        return 1;
}

uint8_t UARTisDTRset()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    if(status & TIOCM_DTR)
        return 0;
    else
        return 1;
}

uint8_t UARTisRTSset()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    if(status & TIOCM_RTS)
        return 0;
    else
        return 1;
}

uint8_t UARTisCTSset()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    if(status & TIOCM_CTS)
        return 0;
    else
        return 1;
}

uint8_t UARTisDCDset()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    if(status & TIOCM_CAR)
        return 0;
    else
        return 1;
}

uint8_t UARTisRNGset()
{
    uint32_t status = 0;
    
    ioctl(get_fd_uart(), TIOCMGET, &status);
    
    if(status & TIOCM_RNG)
        return 0;
    else
        return 1;
}
