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

#include "networkstack-private.h"



uint32_t addressGetIp(IpAddress * addr)
{
    addr->ip = ( addr->a << 24 ) | ( addr->b << 16 ) | ( addr->c << 8 ) | addr->d;
    return addr->ip;
}

void addressGetABCD(IpAddress * addr, uint32_t ip)
{
    addr->ip = ip;
    addr->a = (addr->ip >> 24) & 0xFF;
    addr->b = (addr->ip >> 16) & 0xFF;
    addr->c = (addr->ip >> 8 ) & 0xFF;
    addr->d = addr->ip & 0xFF;
}


int32_t initSocketPlatform()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2), &WsaData ) == NO_ERROR;
#else
    return 1;
#endif
}

void closeSocketPlatform()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif
}

int32_t socketOpenInBroadcastMode(Socket * s)
{
    int handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( handle <= 0 )
    {
        LOG_ERR1("udp socket");
        return 0;
    }
    
    s->handle = handle;
    
    if (socketSetTimeout(s,30,0)!=NETWORK_OK)
    {
        LOG_ERR1("udp socket: set timeout");
        return 0;
    }
    
    if (socketSetBroadcast(s,1)!=NETWORK_OK)
    {
        LOG_ERR1("udp socket: set broadcast");
        return 0;
    }
    
    return 1;
}

int32_t socketOpen(Socket * s, uint16_t port)
{
#if PLATFORM == PLATFORM_WINDOWS
	DWORD nonBlocking = 1;
#endif
    int k;
    struct sockaddr_in address;
    
    int handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( handle <= 0 )
    {
        LOG_ERR1("udp socket");
        return 0;
    }
    
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    // set the close-on-exec flag to 1
	fcntl(handle, F_SETFD, 1);
#endif
    
    //http://stackoverflow.com/questions/14388706/socket-options-so-reuseaddr-and-so-reuseport-how-do-they-differ-do-they-mean-t
    k = 1;
	if(setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (char*)&k, sizeof(k)) == -1)
	{
		LOG_ERR1("setsockopt");
		return 0;
	}
    
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( (unsigned short) port );
    
    if ( bind( handle, (const struct sockaddr*) &address, sizeof(struct sockaddr_in) ) < 0 )
    {
        LOG_ERR1("bind" );
        return 0;
    }
    
    // set the socket handle in no blocking state
    
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    
    int nonBlocking = 1;
    if ( fcntl( handle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
    {
        LOG_ERR1("set non-blocking socket" );
        return 0;
    }
    
#elif PLATFORM == PLATFORM_WINDOWS
    
    if ( ioctlsocket( handle, FIONBIO, &nonBlocking ) != 0 )
    {
        LOG_ERR1("set non-blocking socket" );
        return 0;
    }
    
#endif
    
    s->port = port;
    s->handle = handle;
    
    return 1;
}

int32_t socketSend(Socket * s, IpAddress * addr, const void * packet_data, uint32_t packet_size)
{
	long sent_bytes;
    struct sockaddr_in address;
    int handle = s->handle;
    
    unsigned int destination_address = addressGetIp(addr);
    unsigned short destination_port = addr->port;
    
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl( destination_address );
    address.sin_port = htons( destination_port );
    
    sent_bytes = sendto( handle, (const char*)packet_data, packet_size,
                            0, (struct sockaddr*)&address, sizeof(struct sockaddr_in) );
    
    if ( sent_bytes != packet_size )
    {
        LOG_ERR2("failed to send packet:", sent_bytes );
        return 0;
    }
    
    return 1;
}

int32_t socketReceive(Socket *s, IpAddress *addr, void * packet_data, uint32_t maximum_packet_size)
{
#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif
    
    struct sockaddr_in from;
    socklen_t fromLength = sizeof( from );
    
    long received_bytes = recvfrom( s->handle, (char*)packet_data, maximum_packet_size,
                                  0, (struct sockaddr*)&from, &fromLength );
    
    if ( received_bytes <= 0 )
        return (int32_t)received_bytes;
    
    addressGetABCD(addr, ntohl( from.sin_addr.s_addr ));
    
    addr->port = ntohs( from.sin_port );
    
    return (int32_t)received_bytes;
}

void socketClose(Socket *s)
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close( s->handle );
#elif PLATFORM == PLATFORM_WINDOWS
    closesocket( s->handle );
#endif
}

NetworkStatus socketGetLastError(void)
{
    int ErrorID = Network_GetLastError();

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    if (ErrorID==EAGAIN)
#else
    if (ErrorID==WSAETIMEDOUT)
#endif
    {
        return NETWORK_TIMEOUT;
    }
    
    LOG_ERR2("get last error", ErrorID);

    return NETWORK_ERROR;
}

