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


static unsigned int buildAckBitmap(unsigned int * acks)
{
    unsigned int result = 0;
    unsigned int i;
    unsigned int flag = 1;
    
    for (i=0;i<ACK_MAX;i++)
    {
        if (acks[i]==1)
        {
            result = result | flag ;
        }
    
        flag = flag << 1;
    }
    
//    printf("ack bitmap: %x\n",result);
    
    return result;
}

static void moveAckBitmap(unsigned int * acks)
{
    unsigned int i;
    
    for (i=ACK_MAX-1;i!=0;i--)
    {   
        acks[i] = acks[i-1];
    }
    
    acks[0] = 0;
}

static void setLocalAckFromBitmap(NetConnection * connection, unsigned int index, unsigned int ackbitmap)
{
    unsigned int i;
    unsigned int flag = 1 << index;
    
    for (i=index;i<ACK_MAX;i++)
    {
        if ( (ackbitmap & flag) != 0)
        {
            connection->localAcks[i] = 1;
        }
        
        flag = flag << 1;
    }
    
}

UInt32 AddressGetIp(IpAddress * addr)
{
    addr->ip = ( addr->a << 24 ) | ( addr->b << 16 ) | ( addr->c << 8 ) | addr->d;
    return addr->ip;
}

void AddressGetABCD(IpAddress * addr, UInt32 ip)
{
    addr->ip = ip;
    addr->a = (addr->ip >> 24) & 0xFF;
    addr->b = (addr->ip >> 16) & 0xFF;
    addr->c = (addr->ip >> 8 ) & 0xFF;
    addr->d = addr->ip & 0xFF;
}


Int32 InitSocketPlatform()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup( MAKEWORD(2,2), &WsaData ) == NO_ERROR;
#else
    return 1;
#endif
}

void CloseSocketPlatform()
{
#if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
#endif
}


Int32 SocketOpen(Socket * s, UInt16 port)
{
    struct sockaddr_in address;
    
    int handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( handle <= 0 )
    {
        LOG_ERR1("udp socket");
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
    
    DWORD nonBlocking = 1;
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

Int32 SocketSend(Socket * s, IpAddress * addr, const void * packet_data, UInt32 packet_size)
{
    struct sockaddr_in address;
    int handle = s->handle;
    
    unsigned int destination_address = AddressGetIp(addr);
    unsigned short destination_port = addr->port;
    
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl( destination_address );
    address.sin_port = htons( destination_port );
    
    long sent_bytes = sendto( handle, (const char*)packet_data, packet_size,
                            0, (struct sockaddr*)&address, sizeof(struct sockaddr_in) );
    
    if ( sent_bytes != packet_size )
    {
        LOG_ERR2("failed to send packet:", sent_bytes );
        return 0;
    }
    
    return 1;
}

Int32 SocketReceive(Socket *s, IpAddress *addr, void * packet_data, UInt32 maximum_packet_size)
{
#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif
    
    struct sockaddr_in from;
    socklen_t fromLength = sizeof( from );
    
    long received_bytes = recvfrom( s->handle, (char*)packet_data, maximum_packet_size,
                                  0, (struct sockaddr*)&from, &fromLength );
    
    if ( received_bytes <= 0 )
        return (Int32)received_bytes;
    
    AddressGetABCD(addr, ntohl( from.sin_addr.s_addr ));
    
    addr->port = ntohs( from.sin_port );
    
    return (Int32)received_bytes;
}

void SocketClose(Socket *s)
{
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    close( s->handle );
#elif PLATFORM == PLATFORM_WINDOWS
    closesocket( s->handle );
#endif
}


void ConnectionNew(NetConnection * connection, UInt32 id)
{
    connection->isOpen = False;
    connection->protocolId = id;
    connection->buffer = (unsigned char *)malloc(PACKET_SIZE_MAX);
}

Int32 ConnectionStart(NetConnection * connection, UInt16 port)
{
    unsigned int i;
    
    if (connection->isOpen==False)
    {
        connection->remoteSequence = 0;
        connection->localSequence = 0;
        
        for(i=0;i<ACK_MAX;i++)
        {
            connection->localAcks[i] = 0;
            connection->remoteAcks[i] = 0;
        }
        
        connection->isOpen = True;
        return SocketOpen(&connection->sock, port);
    }
    return 0;
}

void ConnectionStop(NetConnection * connection)
{
    if (connection->isOpen==True)
    {
        connection->isOpen = False;
        SocketClose(&connection->sock);
    }
}

void ConnectionConnect(NetConnection * connection, IpAddress * addr)
{
    memcpy(&connection->remote, addr, sizeof(IpAddress));
}

Int32 ConnectionSend(NetConnection * connection, const void * data, UInt32 size)
{
    int result;
    unsigned char * packet = connection->buffer;
    
    if (size>(PACKET_SIZE_MAX-CONNECTION_HEADER_SIZE))
        return 0;
    
    // create packet header with :
    // - protocol identifier
    // - local packet sequence number
    // - ack : last packet sequence number received
    // - acks bitmap from the receveid packet
    Write32bitsToBuffer(packet,connection->protocolId);
    Write32bitsToBuffer(packet+4,connection->localSequence);
    Write32bitsToBuffer(packet+8,connection->remoteSequence);
    Write32bitsToBuffer(packet+12,buildAckBitmap(connection->remoteAcks));
    
    // move the ack bitmap and don't acknowledge the packet send
    moveAckBitmap(connection->localAcks);
    connection->localAcks[0] = 0;
    
    connection->localSequence++;
    
    memcpy( &packet[CONNECTION_HEADER_SIZE], data, size );
    
    result = SocketSend(&connection->sock, &connection->remote, packet, size + CONNECTION_HEADER_SIZE );
    
    return result;
}

static int isSequenceMoreRecent(unsigned int s1, unsigned int s2)
{
    return  (( s1 > s2 ) && ( s1 - s2 <= SEQUENCE_MAX/2 )) || (( s2 > s1 ) && ( s2 - s1 > SEQUENCE_MAX/2  ));
}

Int32 ConnectionReceive(NetConnection * connection, void * data, UInt32 size)
{
    unsigned int index;
    unsigned int id;
    unsigned int remote;
    unsigned int ack;
    unsigned int ackBitmap;
    unsigned char * packet = connection->buffer;
    
    int bytes_read = SocketReceive(&connection->sock, &connection->sender, packet, size + CONNECTION_HEADER_SIZE );
    if ( bytes_read <= 0 )
        return bytes_read;
    
    if ( bytes_read <= CONNECTION_HEADER_SIZE )
    {
        LOG_ERR2("Not enough bytes for header", bytes_read);
        return 0;
    }
    
    id = (unsigned int)Read32bitsFromBuffer(packet);
    
    if (id!=connection->protocolId)
    {
        LOG_ERR2("This id is not for this connection",id);
        return 0;
    }
    
    // todo state listen and check address between sender and address ?
    
    // si server mode, alors verifier address sender
    // si client mode, mettre comme 'Connected'
    
    
    
    // read the remote packet number and check if this number is more recent than the current
    remote = (unsigned int)Read32bitsFromBuffer(packet+4);
    if (isSequenceMoreRecent(remote,connection->remoteSequence)==1)
    {
        // move the ack bitmap and acknowledge the packet received
        connection->remoteSequence = remote;
        
        moveAckBitmap(connection->remoteAcks);
        
        connection->remoteAcks[0] = 1;
    }
    else
    {
        // we receive an old packet
        index = (connection->remoteSequence - remote) & (ACK_MAX-1);
                
        connection->remoteAcks[index] = 1;
    }
    
    // read the ack (my sequence number from the point of view of the remote)
    ack = (unsigned int)Read32bitsFromBuffer(packet+8);
    ackBitmap = (unsigned int)Read32bitsFromBuffer(packet+12);
    
    
    // and check ack and our local sequence number : local - ACK_MAX < ack < local
//    if (isSequenceMoreRecent(ack, connection->localSequence)==0)
    if ( (int)ack > (int)(connection->localSequence - ACK_MAX) && ack <= connection->localSequence)
    {
        index = (connection->localSequence - ack) & (ACK_MAX-1);
        
        setLocalAckFromBitmap(connection,index,ackBitmap);        
    }
    else
    {
        // we receive a packet with a very old ack and we can't check
        LOG("error: Receive ack %d local %d, too old !\n", ack, connection->localSequence);
    }
        
    
    memcpy( data, &packet[CONNECTION_HEADER_SIZE], bytes_read - CONNECTION_HEADER_SIZE );
    
    return (bytes_read-CONNECTION_HEADER_SIZE);
}