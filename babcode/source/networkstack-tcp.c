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


Int32 SocketTcpInit(Socket * s)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
	if( sock < 0 )
	{
		LOG("error: socket %d\n",Network_GetLastError());
		return 0;
	}
    
    s->handle = sock;
    
    return 1;
}

Int32 ServerTcpOpen(Socket *s, UInt16 port)
{
    int k = 1;
    int sock = s->handle;
	struct sockaddr_in addr ;
    
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
	// set the close-on-exec flag to 1
	fcntl(sock, F_SETFD, 1);
#endif
	
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&k, sizeof(k)) == -1)
	{
		LOG("error: setsockopt(): %d\n", Network_GetLastError());
		return 0;
	}
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr		= INADDR_ANY;
	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(port);
    
	if( bind( sock, (struct sockaddr *)&addr, sizeof( struct sockaddr) ) < 0 )
	{
		LOG("error: bind %d\n", Network_GetLastError() );
		return 0;
	}
    
	if( listen( sock, SOMAXCONN) < 0 )
	{
		LOG("error: listen %d\n",Network_GetLastError());
		return 0;
	}
    
    s->port = port;
    
	return 1;
}

Int32 ServerTcpWaitConnection(Socket * server, Socket * client, IpAddress * clientAddr)
{
    int csock = 0;
	struct sockaddr_in addr ;
    
	int size = sizeof( struct sockaddr);
    
    memset(&addr,0,size);
    
    csock = accept(server->handle, (struct sockaddr *)&addr, (socklen_t *)&size);
    if (csock < 0)
    {
        LOG("error: accept %d\n", Network_GetLastError());
        return 0;
    }

    AddressGetABCD(clientAddr, ntohl( addr.sin_addr.s_addr ));
        
	clientAddr->port = htons(addr.sin_port);
    client->handle = csock;
    client->port = clientAddr->port;
	
	return 1;
}

Int32 ClientTcpOpen(Socket * client, IpAddress * server)
{
	int err;
	struct sockaddr_in addr ;
    
    SocketSetBlockMode(client, 0);
    
    memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= htonl(AddressGetIp(server));
	addr.sin_port			= htons( server->port );
    
	if(connect(client->handle, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
	{
		err = Network_GetLastError();
		switch(err)
		{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
 			case EINPROGRESS:
 			case EALREADY:
#else
 			case WSAEWOULDBLOCK:
#endif
                return -1;
				break;
                
			default:
				LOG("error: connect(): %d\n", Network_GetLastError());
				return 0;
		}
	}
    	
	return 1;
}

Int32 SocketTcpSend(Socket * s, const void * packet_data, UInt32 packet_size)
{
    return (int)send(s->handle,packet_data,packet_size,0);
}

Int32 SocketTcpReceive(Socket *s, void * buffer, UInt32 buffer_size)
{
	int result;
	
_again_:
	result = (int)recv(s->handle, (char*)buffer, buffer_size, 0);
	if (result < 0)
	{
		int ErrorID = Network_GetLastError();
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
		if(ErrorID == EINTR)
#else
        if(ErrorID == WSAEINTR)
#endif
                goto _again_;
		
        LOG("error: read: %d\n", ErrorID);
		return -1;
	}
    
	return result;
}





