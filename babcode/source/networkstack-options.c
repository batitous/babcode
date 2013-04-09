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

#include "networkstack-private.h"


NetworkStatus SocketSetBlock(Socket *s, Bool enable)
{
#if PLATFORM == PLATFORM_WINDOWS
    
	u_long ioctl_arg;
	
    if (enable==False)
        ioctl_arg = 1;
    else
        ioctl_arg = 0;
    
    ioctlsocket(s->handle, FIONBIO, (u_long*)&ioctl_arg);

#else

    int flags;
    int r;
    
    flags = fcntl (s->handle, F_GETFL);
    if (flags < 0)
	{
		LOG_ERR2("fcntl(F_GETFL)", Network_GetLastError());
		return NETWORK_ERROR;
	}
    
    if (enable == True)//blocking mode
        r = fcntl (s->handle, F_SETFL, flags & ~O_NONBLOCK);
    else//non blocking mode
        r = fcntl (s->handle, F_SETFL, flags | O_NONBLOCK);
    
    if (r < 0)
	{
		LOG_ERR2("fcntl(F_SETFL)", Network_GetLastError());
		return NETWORK_ERROR;
	}
    
#endif
	return NETWORK_OK;
}

NetworkStatus SocketSetTimeout(Socket *s, Int32 RecvTimeout, Int32 SendTimeout)
{
	if(RecvTimeout > 0)
	{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        
		struct timeval TV;
		
		TV.tv_sec = RecvTimeout / 1000;
		TV.tv_usec = (RecvTimeout % 1000) * 1000;
		
		if(setsockopt(s->handle, SOL_SOCKET, SO_RCVTIMEO, (void*)&TV, sizeof(TV)) < 0)
			return NETWORK_ERROR;
#else
        
		if(setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (void*)&RecvTimeout, sizeof(RecvTimeout)) < 0)
			return NETWORK_ERROR;
        
#endif
	}
	
	if(SendTimeout > 0)
	{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        
		struct timeval TV;
		
		TV.tv_sec = SendTimeout / 1000;
		TV.tv_usec = (SendTimeout % 1000) * 1000;
		
		if(setsockopt(s->handle, SOL_SOCKET, SO_SNDTIMEO, (void*)&TV, sizeof(TV)) < 0)
			return NETWORK_ERROR;
#else
        
		if(setsockopt(s->handle, SOL_SOCKET, SO_RCVTIMEO, (void*)&SendTimeout, sizeof(SendTimeout)) < 0)
			return NETWORK_ERROR;
        
#endif
	}
	
	return NETWORK_OK;
}

Int32 SocketSetBroadcast(Socket *s, Int32 Enable)
{
	if(setsockopt(s->handle, SOL_SOCKET, SO_BROADCAST, (void*)&Enable, sizeof(Enable)) < 0)
		return 0;
	return 1;
}

Int32 SocketSetKeepAlive(Socket *s, Int32 Enable)
{
	if(setsockopt(s->handle, SOL_SOCKET, SO_KEEPALIVE, (void*)&Enable, sizeof(Enable)) < 0)
		return 0;
	return 1;
}


Int32 SocketSetTcpNoDelay(Socket *s, Int32 Enable)
{
	if(setsockopt(s->handle, IPPROTO_TCP, TCP_NODELAY, (void*)&Enable, sizeof(Enable)) < 0)
		return 0;
	return 1;
}


Int32 SocketSetMulticastTTL(Socket *s, Int32 TTL)
{
	if(setsockopt(s->handle, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&TTL, sizeof(TTL)) < 0)
		return 0;
	return 1;
}


Int32 SocketSetMulticastIF(Socket *s, UInt32 ip)
{
	if(setsockopt(s->handle, IPPROTO_IP, IP_MULTICAST_IF, (void*)&ip, sizeof(ip)) < 0)
		return 0;
	return 1;
}

Int32 SocketSetMulticastMembership(Socket *s, UInt32 faceIP, UInt32 multicastIP)
{
	struct ip_mreq McastAddr;
	
	memset(&McastAddr, 0, sizeof(struct ip_mreq));
	McastAddr.imr_interface.s_addr = faceIP;
	McastAddr.imr_multiaddr.s_addr = multicastIP;
	
	if(setsockopt(s->handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&McastAddr, sizeof(struct ip_mreq)) < 0)
		return 0;
	
	return 1;
}

