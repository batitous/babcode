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


NetworkStatus socketTcpInit(Socket * s)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
	if( sock < 0 )
	{
		LOG_ERR2("socket",Network_GetLastError());
		return NETWORK_ERROR;
	}
    
    s->handle = sock;
    
    return NETWORK_OK;
}

NetworkStatus serverTcpOpen(Socket *s, UInt16 port)
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
		LOG_ERR2("setsockopt", Network_GetLastError());
		return NETWORK_ERROR;
	}
    
    memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_addr.s_addr		= INADDR_ANY;
	addr.sin_family				= AF_INET;
	addr.sin_port				= htons(port);
    
	if( bind( sock, (struct sockaddr *)&addr, sizeof( struct sockaddr) ) < 0 )
	{
		LOG_ERR2("bind", Network_GetLastError() );
		return NETWORK_ERROR;
	}
    
	if( listen( sock, SOMAXCONN) < 0 )
	{
		LOG_ERR2("listen",Network_GetLastError());
		return NETWORK_ERROR;
	}
    
    s->port = port;
    
	return NETWORK_OK;
}

NetworkStatus serverTcpWaitConnection(Socket * server, Socket * client, IpAddress * clientAddr)
{
#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif

	int csock = 0;
	struct sockaddr_in addr ;
    
	int size = sizeof( struct sockaddr);
    
    memset(&addr,0,size);
    
    csock = accept(server->handle, (struct sockaddr *)&addr, (socklen_t *)&size);
    if (csock < 0)
    {
        LOG_ERR2("accept", Network_GetLastError());
        return NETWORK_ERROR;
    }

    addressGetABCD(clientAddr, ntohl( addr.sin_addr.s_addr ));
        
	clientAddr->port = htons(addr.sin_port);
    client->handle = csock;
    client->port = clientAddr->port;
	
	return NETWORK_OK;
}

NetworkStatus clientTcpOpen(Socket * client, IpAddress * server)
{
	int err;
	struct sockaddr_in addr ;
    
    memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= htonl(addressGetIp(server));
	addr.sin_port			= htons( server->port );
    
    err = connect(client->handle, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
	if( err < 0)
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
                return NETWORK_TIMEOUT;
				break;
                
			default:
				LOG_ERR2("connect: %d", Network_GetLastError());
				return NETWORK_ERROR;
		}
	}
    
	return NETWORK_OK;
}

NetworkStatus socketTcpSend(Socket * s, const void * packet_data, UInt32 packet_size, UInt32 * sended)
{
    int result = (int)send(s->handle,(const char *)packet_data,packet_size,0);

    if (result < 0)
    {
        int errorID = Network_GetLastError();
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        if (errorID==EINTR || errorID==EAGAIN)
#else
		if (errorID==WSAEINTR || errorID==WSAETIMEDOUT)
#endif
            return NETWORK_TIMEOUT;
        
        LOG_ERR2("send", errorID);
        return NETWORK_ERROR;
    }
    
    *sended = result;
    
    return NETWORK_OK;
}

NetworkStatus socketTcpReceive(Socket *s, void * buffer, UInt32 buffer_size, UInt32 * received)
{
	int result;
	
_again_:
	result = (int)recv(s->handle, (char*)buffer, buffer_size, 0);
	if (result < 0)
	{
		int ErrorID = Network_GetLastError();
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
		if (ErrorID == EINTR)
#else
        if (ErrorID == WSAEINTR)
#endif
                goto _again_;
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        if (ErrorID==EAGAIN)
#else
		if (ErrorID==WSAETIMEDOUT)
#endif
        {
            return NETWORK_TIMEOUT;
        }
		
        LOG_ERR2("recv", ErrorID);
		return NETWORK_ERROR;
	}
    
    *received = result;
    
	return NETWORK_OK;
}

NetworkStatus socketTcpSendAll(Socket *s, void *buffer, UInt32 buffer_size)
{
    NetworkStatus status;
    UInt32 sended;
	int toWrite;
	
	toWrite = buffer_size;
	while(toWrite > 0)
	{
		status = socketTcpSend(s, (UInt8 *)buffer + buffer_size - toWrite, toWrite, &sended);
		if (status == NETWORK_ERROR)
		{
			LOG_ERR1("SocketTcpSendAll");
			return NETWORK_ERROR;
		}
		toWrite -= sended;
	}
	
	return NETWORK_OK;
}

NetworkStatus socketTcpReceiveAll(Socket *s, void *buffer, UInt32 buffer_size)
{
    NetworkStatus status;
	UInt32 received;
	int toRead;
    
	toRead = buffer_size;
	while (toRead > 0)
	{
		status = socketTcpReceive(s, (UInt8 *)buffer + buffer_size - toRead, toRead, &received);
		if (status == NETWORK_ERROR)
		{
			LOG_ERR1("SocketTcpReceiveAll");
			return NETWORK_ERROR;
		}
		toRead -= received;
	}
	
	return NETWORK_OK;
}
