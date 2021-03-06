
#include <stdio.h>
#include <string.h>

#include "../babcode/include/babcode.h"


void test1(void)
{
    Socket sock;
    IpAddress addr;
    int counter = 0;
    
    unsigned char buffer[16];
    
    initSocketPlatform();
    
    if (socketOpen(&sock, 10003)==0)
    {
        printf("failed to create socket !\n");
        return;
    }
    
    addr.a = 127;
    addr.b = 0;
    addr.c = 0;
    addr.d = 1;
    
    addr.port = 10003;
    
    while(1)
    {
        /*        buffer[0] = counter;
         SocketSend(&sock, &addr, buffer, 16);
         counter++;
         sleep(1);
         */
        counter = socketReceive(&sock, &addr, buffer, 16);
        if (counter>0)
        {
            printf("Receive %d bytes from %d.%d.%d.%d:%d\n",counter,addr.a,addr.b,addr.c,addr.d,addr.port);
        }
        
    }
    
    socketClose(&sock);
    
    closeSocketPlatform();
    
}

int run = 1;
int ackDump = 0;

#define PORT_SERVER     10002
#define PORT_CLIENT     10001

void * server(void *p)
{
    int i = 0;
    
    int result;
    unsigned char rcv[16];
    
    int counter = 1;
    unsigned char send[16];
    
    IpAddress remote;
    NetConnection server;
    
    connectionNew(&server, 10);
    connectionStart(&server, PORT_SERVER);
    
/*    remote.a = 127;
    remote.b = 0;
    remote.c = 0;
    remote.d = 1;
    remote.port = PORT_CLIENT;
    ConnectionConnect(&server, &remote);
*/
    while (run!=0)
    {
        result = connectionReceive(&server, rcv, 16);
        if( result > 0)
        {
            //printf("[SERVER] received %d bytes re %d l %d\n", result,server.remoteSequence, server.localSequence);
            
            counter++;
            
            /*  printf("----\n");
             for(i=0;i<ACK_MAX;i++)
             {
             result = (int)server.localSequence-i;
             if( result < 0 )
             break;
             printf("[SERVER] ack packet %d : %d\n", server.localSequence-i,server.localAcks[i]);
             }
             */
            if (counter>8)
            {
                memcpy(&server.remote,&server.sender, sizeof(IpAddress));
                connectionSend(&server,send,counter);
                counter = 1;
            }
        }
        
        
        /*        if (ackDump==1)
         {
         ackDump=0;
         
         for(i=0;i<ACK_MAX;i++)
         {
         printf("[SERVER] ack packet %d : %d\n", server.localSequence-i,server.acks[i]);
         }
         }
         */
        //        wait(0.1);
    }
    
    
    connectionStop(&server);
    threadExit();
    return 0;
}

void *client(void *p)
{
    int i;
    
    int counter = 1;
    unsigned char send[16];
    
    int result;
    unsigned char rcv[16];
    
    IpAddress remote;
    NetConnection client;
    
    connectionNew(&client, 10);
    connectionStart(&client, PORT_CLIENT);
    
    remote.a = 127;
    remote.b = 0;
    remote.c = 0;
    remote.d = 1;
    remote.port = PORT_SERVER;
    connectionConnect(&client,&remote);
    
    while (run!=0)
    {
        send[0] = counter;
        connectionSend(&client, send, counter);
        
        counter++;
        if (counter>15)
        {
            counter = 1;
        }
        
        result = connectionReceive(&client, rcv, 16);
        if( result > 0)
        {
            /*            printf("----\n");
             for(i=0;i<ACK_MAX;i++)
             {
             result = (int)client.remoteSequence-i;
             if( result < 0 )
             break;
             printf("[CLIENT] ack packet %d : %d\n", client.remoteSequence-i,client.remoteAcks[i]);
             }
             */
            //printf("[CLIENT] received %d bytes re %d l %d\n", result,client.remoteSequence,client.localSequence);
        }
        
        
        if (ackDump==1)
        {
            ackDump=0;
            
            for(i=0;i<ACK_MAX;i++)
            {
                result = (int)client.localSequence-i;
                if( result < 0 )
                    break;
                printf("[CLIENT] ack packet %d : %d\n", client.localSequence-i,client.localAcks[i]);
            }
        }
        
        waitMs(100);
    }
    
    
    connectionStop(&client);
    threadExit();
    return 0;
}

int testNetworkStack(void)
{
    int command;
    Thread tServer, tClient;
    
    printf("Testing network stack !\n");
    
    //test1();
    
    
    initSocketPlatform();
    
    threadInit(&tServer, server, 0);
    threadInit(&tClient,client,0);
    
    
    while(run!=0)
    {
        command = getchar();
        
        
        if (command=='0')
        {
            run = 0;
        }
        else if(command=='a')
        {
            ackDump = 1;
        }
        
    }
    
    printf("Exit test program!\n");
    
    
    
    closeSocketPlatform();
    
    return 0;
}

