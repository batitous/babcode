//
//  udpconnection.cpp
//  babextended
//
//  Created by Baptiste on 21/01/2015.
//  Copyright (c) 2015 Evotion. All rights reserved.
//

#include "../../babcode/include/babcode.h"
#include "../include/udpconnection.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>


UdpConnection::UdpConnection(uint32_t id)
{
    mIsOpen = false;
    mProtocolId = id;
    mBuffer = new uint8_t[UDP_CONNECTION_PACKET_SIZE_MAX];
}

void UdpConnection::startWithSocket(Socket *s)
{
    mSock.handle = s->handle;
    mSock.port = s->port;
    
    if (mIsOpen==false)
    {
        mRemoteSequence = 0;
        mLocalSequence = 0;
        mSendDeltaTime = 0;
        
        for(int i=0;i<ACK_MAX;i++)
        {
            mLocalAcks[i] = 0;
            mRemoteAcks[i] = 0;
        }
        
        mIsOpen = true;
    }
    
}

int32_t UdpConnection::start(uint16_t port)
{
    unsigned int i;
    
    if (mIsOpen==false)
    {
        mRemoteSequence = 0;
        mLocalSequence = 0;
        mSendDeltaTime = 0;
        
        for(i=0;i<ACK_MAX;i++)
        {
            mLocalAcks[i] = 0;
            mRemoteAcks[i] = 0;
        }
        
        mIsOpen = true;
        return socketOpen(&mSock, port);
    }
    
    return 0;
}

void UdpConnection::stop()
{
    if (mIsOpen==true)
    {
        mIsOpen = false;
        socketClose(&mSock);
    }
}

Socket * UdpConnection::getSocket()
{
    return &mSock;
}

const IpAddress & UdpConnection::getIpReceiver()
{
    return mSender;
}

uint32_t UdpConnection::getReceiverTime()
{
    return mRemoteDeltaTime;
}

void UdpConnection::connect(const IpAddress * addr)
{
    memcpy(&mRemote, addr, sizeof(IpAddress));
}

bool UdpConnection::sendLargePacket(const void* data, uint32_t size)
{
    uint32_t pkSize = UDP_CONNECTION_PACKET_DATA_MAX;
    uint32_t pkNumber = size / pkSize;
    uint32_t remind = size - (pkNumber * pkSize);
    
    uint32_t i;
    uint8_t * ptr;
    
    ptr = (uint8_t *)data;
    
    
    
    for (i=0; i < pkNumber; i++)
    {
        if (send((const void *)ptr, pkSize) != NETWORK_OK)
        {
            return false;
        }
        
        ptr += pkSize;
    }
    
    if (remind!=0)
    {
        if (send((const void *)ptr, remind) != NETWORK_OK)
        {
            return false;
        }
    }
    
    return true;
}

int32_t UdpConnection::send(const void * data, uint32_t size)
{
    uint32_t last;
    uint32_t delta = 0;
    int result;
    unsigned char * packet = mBuffer;
    
    if (size>(UDP_CONNECTION_PACKET_SIZE_MAX-CONNECTION_HEADER_SIZE))
    {
        return 0;
    }
    
    last = getTicks();
    if (mSendDeltaTime!=0)
    {
        delta = last - mSendDeltaTime;
    }
    
    mSendDeltaTime = last;
    
    
    // create packet header with :
    // - protocol identifier
    // - local packet sequence number
    // - ack : last packet sequence number received
    // - acks bitmap from the receveid packet
    // - timestamp between 2 send call
    write32bitsToBuffer(packet,mProtocolId);
    write32bitsToBuffer(packet+4,mLocalSequence);
    write32bitsToBuffer(packet+8,mRemoteSequence);
    write32bitsToBuffer(packet+12,buildAckBitmap(mRemoteAcks));
    write32bitsToBuffer(packet+16, delta);
    
    // move the ack bitmap and don't acknowledge the packet send
    moveAckBitmap(mLocalAcks);
    mLocalAcks[0] = 0;
    
    mLocalSequence++;
    
    memcpy( &packet[CONNECTION_HEADER_SIZE], data, size );
    
    result = socketSend(&mSock, &mRemote, packet, size + CONNECTION_HEADER_SIZE );
    
    return result;
}

int32_t UdpConnection::receive(void * data, uint32_t size)
{
    unsigned int index;
    unsigned int id;
    unsigned int remote;
    unsigned int ack;
    unsigned int ackBitmap;
    unsigned char * packet = mBuffer;
    
    int bytes_read = socketReceive(&mSock, &mSender, packet, size + CONNECTION_HEADER_SIZE );
    if ( bytes_read <= 0 )
    {
        return bytes_read;
    }
    
    if ( bytes_read <= CONNECTION_HEADER_SIZE )
    {
        LOG_ERR2("Not enough bytes for header", bytes_read);
        return 0;
    }
    
    id = (unsigned int)read32bitsFromBuffer(packet);
    
    if (id!=mProtocolId)
    {
        LOG_ERR2("This id is not for this connection",id);
        return 0;
    }
    
    // todo state listen and check address between sender and address ?
    
    // si server mode, alors verifier address sender
    // si client mode, mettre comme 'Connected'
    
    
    
    // read the remote packet number and check if this number is more recent than the current
    remote = (unsigned int)read32bitsFromBuffer(packet+4);
    if (isSequenceMoreRecent(remote,mRemoteSequence)==1)
    {
        // move the ack bitmap and acknowledge the packet received
        mRemoteSequence = remote;
        
        moveAckBitmap(mRemoteAcks);
        
        mRemoteAcks[0] = 1;
    }
    else
    {
        // we receive an old packet
        index = (mRemoteSequence - remote) & (ACK_MAX-1);
        
        mRemoteAcks[index] = 1;
    }
    
    // read the ack (my sequence number from the point of view of the remote)
    ack = (unsigned int)read32bitsFromBuffer(packet+8);
    ackBitmap = (unsigned int)read32bitsFromBuffer(packet+12);
    mRemoteDeltaTime = (uint32_t)read32bitsFromBuffer(packet+16);
    
    
    // and check ack and our local sequence number : local - ACK_MAX < ack < local
    //    if (isSequenceMoreRecent(ack, connection->localSequence)==0)
    if ( (int)ack > (int)(mLocalSequence - ACK_MAX) && ack <= mLocalSequence)
    {
        index = (mLocalSequence - ack) & (ACK_MAX-1);
        
        setLocalAckFromBitmap(index,ackBitmap);
    }
    else
    {
        // we receive a packet with a very old ack and we can't check
        LOG("error: Receive ack %d local %d, too old !\n", ack, mLocalSequence);
    }
    
    
    memcpy( data, &packet[CONNECTION_HEADER_SIZE], bytes_read - CONNECTION_HEADER_SIZE );
    
    return (bytes_read-CONNECTION_HEADER_SIZE);
}


int32_t UdpConnection::waitAndReceive(void * data, uint32_t size)
{
    int max_sd;
    int s;
    int activity;
    
    // clear the socket set
    FD_ZERO(&mReadSocketDescriptor);
    
    // add socket to set
    s = mSock.handle;
    FD_SET(s, &mReadSocketDescriptor);
    max_sd = s;
    
    // wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
    activity = select( max_sd + 1 , &mReadSocketDescriptor , NULL , NULL , NULL);
    
    if ((activity < 0) && (errno!=EINTR))
    {
        LOG_ERR1("error: on select");
        return 0;
    }
    
    if (FD_ISSET(mSock.handle, &mReadSocketDescriptor))
    {
        printf("wait %d \n", size);
        
        return receive(data, size);
    }
    
    return 0;
}

int UdpConnection::isSequenceMoreRecent(unsigned int s1, unsigned int s2)
{
    return  (( s1 > s2 ) && ( s1 - s2 <= SEQUENCE_MAX/2 )) || (( s2 > s1 ) && ( s2 - s1 > SEQUENCE_MAX/2  ));
}

unsigned int UdpConnection::buildAckBitmap(unsigned int * acks)
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

void UdpConnection::moveAckBitmap(unsigned int * acks)
{
    unsigned int i;
    
    for (i=ACK_MAX-1;i!=0;i--)
    {
        acks[i] = acks[i-1];
    }
    
    acks[0] = 0;
}

void UdpConnection::setLocalAckFromBitmap(unsigned int index, unsigned int ackbitmap)
{
    unsigned int i;
    unsigned int flag = 1 << index;
    
    for (i=index;i<ACK_MAX;i++)
    {
        if ( (ackbitmap & flag) != 0)
        {
            mLocalAcks[i] = 1;
        }
        
        flag = flag << 1;
    }
    
}
