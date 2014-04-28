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

#ifndef BABCODE_NETWORK_STACK_H
#define BABCODE_NETWORK_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _bab_socket_
{
    int handle;     /**< internal socket descriptor */
    uint16_t port;    /**< local socket port */
} Socket;

typedef struct _bab_ip_address_
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t ip;
    uint16_t port;
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
    uint32_t      protocolId;     /**< protocol identifier */
    bool        isOpen;         /**< is this connection open ? */
    uint8_t *     buffer;         /**< temporary buffer */
    uint32_t      localSequence;  /**< local sequence number */
    uint32_t      localAcks[ACK_MAX]; /**< ack received for my packet send */
    uint32_t      remoteSequence; /**< remote sequence number: last packet number received (more recent) */
    uint32_t      remoteAcks[ACK_MAX]; /**< ack of the last packets received */
} NetConnection;

typedef enum _network_status_
{
    NETWORK_ERROR     = 0,
    NETWORK_OK        = 1,
    NETWORK_TIMEOUT   = 2
} NetworkStatus;
    
// initialize / close the socket layer
extern int initSocketPlatform();
extern void closeSocketPlatform();
    
// get ip from a.b.c.d address
extern uint32_t addressGetIp(IpAddress * addr);

// get a.b.c.d from ip
extern void addressGetABCD(IpAddress * addr, uint32_t ip);

// socket option
extern NetworkStatus socketSetBlock(Socket *s, bool enable);
extern NetworkStatus socketSetTimeout(Socket *s, int32_t RecvTimeout, int32_t SendTimeout);
extern NetworkStatus socketSetBroadcast(Socket *s, int Enable);
extern NetworkStatus socketSetKeepAlive(Socket *s, bool Enable);
extern NetworkStatus socketSetTcpNoDelay(Socket *s, bool Enable);

extern NetworkStatus socketGetLastError(void);
    
// tcp socket
extern NetworkStatus socketTcpInit(Socket * s);
extern NetworkStatus serverTcpOpen(Socket *s, uint16_t port);
extern NetworkStatus serverTcpWaitConnection(Socket * server, Socket * client, IpAddress * clientAddr);
extern NetworkStatus clientTcpOpen(Socket * client, IpAddress * server);
    
extern NetworkStatus socketTcpSend(Socket * s, const void * packet_data, uint32_t packet_size, uint32_t * sended);
extern NetworkStatus socketTcpReceive(Socket *s,void * packet_data, uint32_t maximum_packet_size, uint32_t * received);
    
extern NetworkStatus socketTcpSendAll(Socket *s, void *buffer, uint32_t buffer_size);
extern NetworkStatus socketTcpReceiveAll(Socket *s, void *buffer, uint32_t buffer_size);
    
// udp socket
extern int32_t socketOpen(Socket * s, uint16_t port);
extern int32_t socketOpenInBroadcastMode(Socket * s);
extern int32_t socketSend(Socket * s, IpAddress * addr, const void * packet_data, uint32_t packet_size);
extern int32_t socketReceive(Socket *s, IpAddress *addr, void * packet_data, uint32_t maximum_packet_size);

extern void socketClose(Socket *s);

// connection object
extern void connectionNew(NetConnection * connection, uint32_t id);
extern int32_t connectionStart(NetConnection * connection, uint16_t port);
extern void connectionStop(NetConnection * connection);

// link the address's parameter to the remote connection address
extern void connectionConnect(NetConnection * connection, IpAddress * remoteAddr);

extern int32_t connectionSend(NetConnection * connection, const void * data, uint32_t size);

extern int32_t connectionReceive(NetConnection * connection, void * data, uint32_t size);

#define MAC_ADDR_SIZE	6
    
typedef struct _interface_info_
{
    char *	name;		/**< interface name */
    int8_t *	description; /**< description (null on linux) */
    uint32_t	ip;			/**< ip address */
    uint32_t	netmask;	/**< ip mask */
    uint8_t	mac[MAC_ADDR_SIZE]; /**< MAC address */
    uint32_t	pkSize;		/**< MTU maximum size */
} NetInterfaceInfo;

extern bool getNetworkInterface(NetInterfaceInfo **interfaces,int32_t * interfaceNumber);
extern void freeNetworkInterface(NetInterfaceInfo *interfaces, int32_t interfaceNumber);

#ifdef __cplusplus
}
#endif

#endif
