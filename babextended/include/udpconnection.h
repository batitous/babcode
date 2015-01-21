//
//  udpconnection.h
//  babextended
//
//  Created by Baptiste on 21/01/2015.
//  Copyright (c) 2015 Evotion. All rights reserved.
//

#ifndef babextended_udpconnection_h
#define babextended_udpconnection_h


#define UDP_CONNECTION_PACKET_SIZE_MAX      1040        /**< Size of UDP packet */
#define CONNECTION_HEADER_SIZE              16          /**< Size of connection header */
#define UDP_CONNECTION_PACKET_DATA_MAX      (UDP_CONNECTION_PACKET_SIZE_MAX - CONNECTION_HEADER_SIZE)
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
    
    
    
    int32_t waitAndReceive(void * data, uint32_t size);
    
    
    /** Get the local socket object
     */
    Socket * getSocket();
    
    
    /** Get the last ip address received
     */
    const IpAddress & ipReceiver();
    
    
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
    fd_set      mReadSocketDescriptor;/**< wait selector */
    
    int isSequenceMoreRecent(unsigned int s1, unsigned int s2);
    unsigned int buildAckBitmap(unsigned int * acks);
    void moveAckBitmap(unsigned int * acks);
    void setLocalAckFromBitmap(unsigned int index, unsigned int ackbitmap);
    
};

#endif
