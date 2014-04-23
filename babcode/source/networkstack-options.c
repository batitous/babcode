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

void freeNetworkInterface(NetInterfaceInfo *interfaces, Int32 interfaceNumber)
{
	int i;
    
	if(!interfaces || interfaceNumber <= 0)
		return;
    
	for(i=0;i<interfaceNumber;i++)
	{
		if(interfaces[i].name)
			free(interfaces[i].name);
        
		if(interfaces[i].description)
			free(interfaces[i].description);
	}
    
	free(interfaces);
}

NetworkStatus socketSetBlock(Socket *s, bool enable)
{
#if PLATFORM == PLATFORM_WINDOWS
    
	u_long ioctl_arg;
	
    if (enable==false)
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
    
    if (enable == true)//blocking mode
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

NetworkStatus socketSetTimeout(Socket *s, Int32 RecvTimeout, Int32 SendTimeout)
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
        
		if(setsockopt(s->handle, SOL_SOCKET, SO_RCVTIMEO, (const char *)&RecvTimeout, sizeof(RecvTimeout)) < 0)
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
        
		if(setsockopt(s->handle, SOL_SOCKET, SO_RCVTIMEO, (const char *)&SendTimeout, sizeof(SendTimeout)) < 0)
			return NETWORK_ERROR;
        
#endif
	}
	
	return NETWORK_OK;
}

NetworkStatus socketSetBroadcast(Socket *s, bool Enable)
{
	if(setsockopt(s->handle, SOL_SOCKET, SO_BROADCAST, (const char *)&Enable, sizeof(Enable)) < 0)
		return NETWORK_ERROR;
	return NETWORK_OK;
}

NetworkStatus socketSetKeepAlive(Socket *s, bool Enable)
{
	if(setsockopt(s->handle, SOL_SOCKET, SO_KEEPALIVE, (const char *)&Enable, sizeof(Enable)) < 0)
		return NETWORK_ERROR;
	return NETWORK_OK;
}

// set TCP_NODELAY before sending short messages that should be sent immediately
NetworkStatus socketSetTcpNoDelay(Socket *s, bool Enable)
{
	if(setsockopt(s->handle, IPPROTO_TCP, TCP_NODELAY, (const char *)&Enable, sizeof(Enable)) < 0)
		return NETWORK_ERROR;
	return NETWORK_OK;
}


NetworkStatus socketSetMulticastTTL(Socket *s, Int32 TTL)
{
	if(setsockopt(s->handle, IPPROTO_IP, IP_MULTICAST_TTL, (const char *)&TTL, sizeof(TTL)) < 0)
		return NETWORK_ERROR;
	return NETWORK_OK;
}


NetworkStatus socketSetMulticastIF(Socket *s, UInt32 ip)
{
	if(setsockopt(s->handle, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&ip, sizeof(ip)) < 0)
		return NETWORK_ERROR;
	return NETWORK_OK;
}

NetworkStatus socketSetMulticastMembership(Socket *s, UInt32 faceIP, UInt32 multicastIP)
{
	struct ip_mreq McastAddr;
	
	memset(&McastAddr, 0, sizeof(struct ip_mreq));
	McastAddr.imr_interface.s_addr = faceIP;
	McastAddr.imr_multiaddr.s_addr = multicastIP;
	
	if(setsockopt(s->handle, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&McastAddr, sizeof(struct ip_mreq)) < 0)
		return NETWORK_ERROR;
	
	return NETWORK_OK;
}


