// Copyright (c) 2015, Baptiste Burles
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

#ifndef babextended_udpconnection_h
#define babextended_udpconnection_h

#include <semaphore.h>


#define UDP_CONNECTION_HEADER_SIZE          20          /**< Size of connection header */
#define UDP_CONNECTION_PACKET_DATA_MAX      1024        /**< Size of connection payload */
#define UDP_CONNECTION_PACKET_SIZE_MAX      (UDP_CONNECTION_PACKET_DATA_MAX+UDP_CONNECTION_HEADER_SIZE)        /**< Size of UDP packet */
#define SEQUENCE_MAX                        0xFFFFFFFF
#define ACK_MAX                             32          /**< Number of ack maximum we can check */



/** An UDP Connection object
 *
 */
class UdpConnection
{
public:
    /** Create a new UDP connection with the specified protocol identifier
     *
     * @param id    Protocol identifier
     */
    UdpConnection(uint32_t id);
    ~UdpConnection();
    
    /** Start the local connection with the specified socket
     *
     * The specified socket MUST BE ready to use (binded to local port)
     *
     * @param socket
     */
    void startWithSocket(const Socket* socket);
   
    
    /** Start the local connection on the specified port
     *
     * @param port      Local connection port
     * @return On success: NETWORK_OK, else error code.
     */
    int32_t start(uint16_t port);
    
    
    /** Stop the connection
     */
    void stop();

    
    /** Connect the local connection to the specified remote address
     *
     * @param remoteAddr        Remote address
     */
    void connect(const IpAddress * remoteAddr);
    
    
    /** Send a large array of bytes : The array will be segmented
     *
     * @param data
     * @param size      Size of data
     * @return On success, true.
     */
    bool sendLargePacket(const void* data, uint32_t size);
    
    /** Send bytes to the remote connection
     *
     * @param data
     * @param size      Size of data, MUST BE LOWER THAN 1024 bytes
     * @return NETWORK_OK on success.
     */
    int32_t send(const void * data, uint32_t size);
    
    
    /** Receive bytes from the remote connection
     *
     * @param data      Array where store the incoming bytes
     * @param size      Array size, MUST BE LOWER THAN 1024 bytes
     * @param Size of data received, else error code
     */
    int32_t receive(void * data, uint32_t size);
    
    
    /** Wait something on the connection, then receive
     */
    int32_t waitAndReceive(void * data, uint32_t size);
    
    
    /** Get the local socket object
     */
    Socket * getSocket();
    
    /** Get the last ip address received
     */
    const IpAddress & getIpReceiver();
    
    /** Get the time between 2 send's call (Warning: received by the remote !)
     */
    uint32_t getTimeOfSender();
    
    /** Get the time between 2 receive's call
     */
    uint32_t getTimeBetweenReceiveCall();
    
    
private:
    Socket      mSock;              /**< connection's socket */
    IpAddress   mRemote;            /**< remote address where send data */
    IpAddress   mSender;            /**< last address received from sender */
    uint32_t    mProtocolId;        /**< protocol identifier */
    bool        mIsOpen;            /**< is this connection open ? */
    uint8_t *   mBuffer;            /**< temporary buffer */
    uint32_t    mLocalSequence;     /**< local sequence number */
    uint32_t    mLocalAcks[ACK_MAX]; /**< ack received for my packet send */
    uint32_t    mRemoteSequence;    /**< remote sequence number: last packet number received (more recent) */
    uint32_t    mRemoteAcks[ACK_MAX]; /**< ack of the last packets received */
    uint32_t    mSendDeltaTime;     /**< Time between 2 send */
    uint32_t    mSenderDeltaTime;   /**< Time received from the remote */
    uint32_t    mReceiverDeltaTime; /**< Time between 2 receive */
    uint32_t    mReceiverLastTime;
    fd_set      mReadSocketDescriptor;/**< wait selector */
    
    int isSequenceMoreRecent(unsigned int s1, unsigned int s2);
    unsigned int buildAckBitmap(unsigned int * acks);
    void moveAckBitmap(unsigned int * acks);
    void setLocalAckFromBitmap(unsigned int index, unsigned int ackbitmap);
    
};

#endif
