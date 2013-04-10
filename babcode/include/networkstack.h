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

#ifndef BABCODE_NETWORK_STACK_H
#define BABCODE_NETWORK_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _bab_socket_
{
    int handle;     /**< internal socket descriptor */
    UInt16 port;    /**< local socket port */
} Socket;

typedef struct _bab_ip_address_
{
    UInt32 a;
    UInt32 b;
    UInt32 c;
    UInt32 d;
    UInt32 ip;
    UInt16 port;
} IpAddress;

#define PACKET_SIZE_MAX         1024
#define CONNECTION_HEADER_SIZE  16
#define SEQUENCE_MAX            0xFFFFFFFF
#define ACK_MAX                 32

typedef struct _net_connection_
{
    Socket      sock;           /**< connection's socket */
    IpAddress   remote;         /**< remote address where send data */
    IpAddress   sender;         /**< last address received from sender */
    UInt32      protocolId;     /**< protocol identifier */
    Bool        isOpen;         /**< is this connection open ? */
    UInt8 *     buffer;         /**< temporary buffer */
    UInt32      localSequence;  /**< local sequence number */
    UInt32      localAcks[ACK_MAX]; /**< ack received for my packet send */
    UInt32      remoteSequence; /**< remote sequence number: last packet number received (more recent) */
    UInt32      remoteAcks[ACK_MAX]; /**< ack of the last packets received */
} NetConnection;

typedef enum _network_status_
{
    NETWORK_ERROR     = 0,
    NETWORK_OK        = 1,
    NETWORK_TIMEOUT   = 2
} NetworkStatus;
    
extern int InitSocketPlatform();
extern void CloseSocketPlatform();
    
// get ip from a.b.c.d address
extern UInt32 AddressGetIp(IpAddress * addr);

// get a.b.c.d from ip
extern void AddressGetABCD(IpAddress * addr, UInt32 ip);


// socket option
extern NetworkStatus SocketSetBlock(Socket *s, Bool enable);
extern NetworkStatus SocketSetTimeout(Socket *s, Int32 RecvTimeout, Int32 SendTimeout);
    
// tcp socket
extern NetworkStatus SocketTcpInit(Socket * s);
extern NetworkStatus ServerTcpOpen(Socket *s, UInt16 port);
extern NetworkStatus ServerTcpWaitConnection(Socket * server, Socket * client, IpAddress * clientAddr);
extern NetworkStatus ClientTcpOpen(Socket * client, IpAddress * server);
    
extern NetworkStatus SocketTcpSend(Socket * s, const void * packet_data, UInt32 packet_size, UInt32 * sended);
extern NetworkStatus SocketTcpReceive(Socket *s,void * packet_data, UInt32 maximum_packet_size, UInt32 * received);
    
// udp socket
extern Int32 SocketOpen(Socket * s, UInt16 port);
extern Int32 SocketSend(Socket * s, IpAddress * addr, const void * packet_data, UInt32 packet_size);
extern Int32 SocketReceive(Socket *s, IpAddress *addr, void * packet_data, UInt32 maximum_packet_size);

extern void SocketClose(Socket *s);


extern void ConnectionNew(NetConnection * connection, UInt32 id);
extern Int32 ConnectionStart(NetConnection * connection, UInt16 port);
extern void ConnectionStop(NetConnection * connection);

// link the address's parameter to the remote connection address
extern void ConnectionConnect(NetConnection * connection, IpAddress * remoteAddr);

extern Int32 ConnectionSend(NetConnection * connection, const void * data, UInt32 size);

extern Int32 ConnectionReceive(NetConnection * connection, void * data, UInt32 size);

#define MAC_ADDR_SIZE	6
    
typedef struct _interface_info_
{
    Int8 *	name;		/**< interface name */
    Int8 *	description; /**< description (null on linux) */
    UInt32	ip;			/**< ip address */
    UInt32	netmask;	/**< ip mask */
    UInt8	mac[MAC_ADDR_SIZE]; /**< MAC address */
    UInt32	pkSize;		/**< MTU maximum size */
} NetInterfaceInfo;

extern Bool GetNetworkInterface(NetInterfaceInfo **interfaces,Int32 * interfaceNumber);
extern void FreeNetworkInterface(NetInterfaceInfo *interfaces, Int32 interfaceNumber);


#ifdef __cplusplus
}
#endif

#endif
