//
//  main.c
//  babcode-test
//
//  Created by Baptiste on 08/04/13.
//  Copyright (c) 2013 Baptiste. All rights reserved.
//

#include <stdio.h>

#include "../babcode/include/babcode.h"

static int dataAvailable=0;

static Mutex myMutex;
static ConditionVar myCondition;

int myRead(char * buffer, int size)
{
    int s;
    mutexLock(&myMutex);
    
    while(dataAvailable==0)
    {
        conditionVarWait(&myCondition, &myMutex);
    }
    
    s = dataAvailable;
    dataAvailable = 0;
    
    mutexUnlock(&myMutex);
    conditionVarSignal(&myCondition);
    
    return s;
}

int myWrite(const char * buffer, int size)
{
    mutexLock(&myMutex);
    
    while (dataAvailable!=0)
    {
        conditionVarWait(&myCondition, &myMutex);
    }
    
    dataAvailable = size;
    
    mutexUnlock(&myMutex);
    conditionVarSignal(&myCondition);
    
    return size;
}

void * reader(void *param)
{
    int i;
    int result;
    
    printf("--> reader !\n");
    
    for(i=0;i < 4; i++)
    {
        result = myRead(0, 0);
        printf("%d result %d\n",i,result);
        
      //  sleep(1);
    }
    
    threadExit();
    return NULL;
}

void * writer(void *param)
{
    int i;
    
    printf("--> writer !\n");
    
    for(i=0; i < 4; i++)
    {
        myWrite(0, 3+i);
        
        waitMs(200);
    }
    
    threadExit();
    return NULL;
}

void * tcpserver(void *p)
{
    Socket server;
    Socket client;
    IpAddress clientAddr;
    uint32_t sended;
        
    socketTcpInit(&server);
    serverTcpOpen(&server, 1234);
    serverTcpWaitConnection(&server, &client, &clientAddr);
    
    printf("Client connected: from %d.%d.%d.%d:%d\n", clientAddr.a,clientAddr.b,clientAddr.c,clientAddr.d,clientAddr.port);
    
    waitMs(2000);
    socketTcpSend(&client, "toto", 5, &sended);
    
    socketClose(&server);
    socketClose(&client);
    
    printf("-> end tcp server\n");
    
    threadExit();
    return NULL;
}

void * tcpclient(void *p)
{
    Socket client;
    IpAddress serverAddr;
    uint8_t buffer[16];
    uint32_t received;
    NetworkStatus status;
    
    serverAddr.a = 127;
    serverAddr.b = 0;
    serverAddr.c = 0;
    serverAddr.d = 1;
    serverAddr.port = 1234;
    
    socketTcpInit(&client);

    socketSetTimeout(&client, 100, 100);
    
    do
    {
        status=clientTcpOpen(&client,&serverAddr);
        waitMs(1);
    } while (status==NETWORK_TIMEOUT);
    
    printf("Client connection status: %d\n", status);
    
    do
    {
        status = socketTcpReceive(&client, buffer, 16, &received);
        waitMs(1);
    } while (status==NETWORK_TIMEOUT);
        
    printf("Receive: %d bytes %s\n", received, buffer);
    
    waitMs(300);
    
    socketClose(&client);
    
    printf("-> end tcp client\n");
    
    threadExit();
    return NULL;
}

void hashtabletest(void)
{
	HashTable table;
    int begin;
	int i;
	int node;
    int base;
	int end;
	
	uint32_t data;

    hashTableInit(&table,16);
    
    node = 8;
    base = 0x00001000;
    
    
    begin = getTicks();
    for( i=0;i<node;i++)
    {
        data = i * 111;
        hashTableInsert(&table, i+base, (void *)data);
    }
    end = getTicks();
    
    printf("Time to insert %d node: %4d\n",node, (end-begin) );
    
    for(i=0;i<table.size;i++)
    {
        printf("%4d key %4x\n", i, table.nodes[i].key);
    }
    
    data = (uint32_t)hashTableLookup(&table,base+4);
//    hashTableDelete(&table, inserted);
    
    printf("AFTER DELETE:\n");
    for(i=0;i<table.size;i++)
    {
        printf("%4d key %4x\n", i, table.nodes[i].key);
    }
}

void callback(void *p)
{
    printf("callback %d\n", getTicks());
}

void timerTest(void)
{
	Timer timer;

    printf("=== Timer test ===\n");
    
    printf("Timer current: %d\n", getTicks());
    
    
    timerInit(&timer,300,500,callback,0);
    timerStart(&timer);
    
    waitMs(1000);
    
    
    timerStop(&timer);
    
    printf("Timer current: %d\n", getTicks());
}


#define UDP_SRV_PORT    11223
#define UDP_CL_PORT     11224

#define PACKET_NUMBER   300
#define CAMERA_SIZE     100000

void *udpclient(void *p)
{
    Socket socket;
    int i;
    IpAddress addr;
    uint8_t * buffer = malloc(CAMERA_SIZE);
    int32_t result;
    uint32_t start;
	uint32_t end;

    addr.a = 127;
    addr.b = 0;
    addr.c = 0;
    addr.d = 1;
    
    addr.port = UDP_SRV_PORT;
    
    if (socketOpen(&socket, UDP_CL_PORT)==0)
    {
        printf("udpserver socket open failed\n");
    }
    
    start = getTicks();
    
    for(i=0;i<PACKET_NUMBER;i++)
    {
        buffer[0]=0xAA;
        if (socketSend(&socket, &addr, buffer, 16)==0)
        {
            printf("udpclient failed to send");
            continue;
        }
        
        result = socketReceive(&socket, &addr, buffer, CAMERA_SIZE);
        
        //printf("udpclient received: %d\n", result);
        
    }
    
    end = getTicks();
    
    printf("Time to received %d packets of %d bytes : %d\n", PACKET_NUMBER, CAMERA_SIZE, (end-start));
    
    
    buffer[0]=0xBB;
    socketSend(&socket, &addr, buffer, 16);
    
    socketClose(&socket);
    threadExit();
    return 0;
}

void *udpserver(void *p)
{
    int i;
    Socket socket;
    int32_t result;
    uint8_t command[16];
    IpAddress addr;
    
    uint8_t * camera = (uint8_t *)malloc(CAMERA_SIZE);
    
    if (socketOpen(&socket, UDP_SRV_PORT)==0)
    {
        printf("udpserver socket open failed\n");
    }
    
    while(1)
    {
        result = socketReceive(&socket, &addr, command, 16);
        if (result<=0)
        {
            continue;
        }
        
//        printf("Received %d\n", result);
  
        if (socketSend(&socket, &addr, camera, CAMERA_SIZE)==0)
        {
            printf("udpserver failed to send !\n");
        }
        
        if (command[0]==0xBB)
        {
            break;
        }
    }
    
    printf("udpserver exit now!\n");
    
    socketClose(&socket);
    threadExit();
    return 0;
}

void * tcpclient1(void *p)
{
    Socket socket;
    int i;
    IpAddress addr;
    uint8_t * buffer = malloc(CAMERA_SIZE);
    int32_t result;
    uint32_t start, end;

    addr.a = 127;
    addr.b = 0;
    addr.c = 0;
    addr.d = 1;
    addr.port = UDP_SRV_PORT;
    
    socketTcpInit(&socket);
    clientTcpOpen(&socket,&addr);
    
    
    start = getTicks();
    
    for(i=0;i<PACKET_NUMBER;i++)
    {
        buffer[0]=0xAA;
        if (socketTcpSendAll(&socket, buffer, 16)!=NETWORK_OK)
        {
            printf("tcpclient1 failed to send %d\n", i);
            continue;
        }
        
        if (socketTcpReceiveAll(&socket, buffer, CAMERA_SIZE)!=NETWORK_OK)
        {
            printf("tcpclient1 failed to receive\n");
        }
    }
    
    end = getTicks();
    
    printf("Time to received %d packets of %d bytes : %d\n", PACKET_NUMBER, CAMERA_SIZE, (end-start));
    
    
    buffer[0]=0xBB;
    socketSend(&socket, &addr, buffer, 16);
    
    socketClose(&socket);
    
    threadExit();
    return 0;
}

void *tcpserver1(void *p)
{
    Socket socket;
    Socket client;
    uint32_t result;
    uint8_t command[16];
    IpAddress clientAddr;
    
    uint8_t * camera = (uint8_t *)malloc(CAMERA_SIZE);
    
    socketTcpInit(&socket);
    serverTcpOpen(&socket, UDP_SRV_PORT);
    serverTcpWaitConnection(&socket, &client, &clientAddr);
    
    while(1)
    {
        socketTcpReceive(&client, command, 16, &result);
        if (result<=0)
        {
            continue;
        }
        
        //        printf("Received %d\n", result);
        
        
        if (socketTcpSendAll(&client, camera, CAMERA_SIZE)!=NETWORK_OK)
        {
            printf("tcpserver1 failed to send\n");
        }
        
        if (command[0]==0xBB)
        {
            break;
        }
    }
    
    printf("tcpserver1 exit now!\n");
    
    socketClose(&socket);
    threadExit();
    return 0;
}

void bandwithTest(void)
{
    Thread tServer, tClient;
    
    initSocketPlatform();
    
//    threadInit(&tServer, udpserver, 0);
//    threadInit(&tClient,udpclient,0);
    
    threadInit(&tServer, tcpserver1, 0);
    threadInit(&tClient,tcpclient1,0);
    
    threadJoin(&tClient);
    threadJoin(&tServer);
    
    
    closeSocketPlatform();
    
}

int main(int argc, const char * argv[])
{
	char AbsCmd[256];
    int lAbsCmd;
	char * pathTest;
    Time time;
    
    Thread writerT, readerT;
    
    // insert code here...
    printf("Hello, World!\n");
    
    pathTest = "../../../test.log";
    
    lAbsCmd = sizeof(AbsCmd);
    if (getRealPath(pathTest, AbsCmd, &lAbsCmd)==true)
    {
        printf("pathtest %s\n",pathTest);
        printf("abscmd   %s\n", AbsCmd);
    }
    
    getTime(&time);
    printf("Time: %d.%d.%d - %d.%d.%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);
    printf("isBissextile ? %d\n", time.isBissextile);
    
    printf("Size of Int32: %d\n", (int32_t)sizeof(int32_t));
    printf("Size of Int64: %ld\n", sizeof(int64_t));
    
    logOpen(pathTest);
    
    LOG_ERR1("mouarf");
    LOG("COUCOU\n");
    
    
    mutexInit(&myMutex);
    conditionVarInit(&myCondition);
    

    threadInit(&writerT,writer,NULL);
    threadInit(&readerT,reader,NULL);
    
    threadJoin(&writerT);
    threadJoin(&readerT);
    
    printf("---> test socket...\n");
    
    initSocketPlatform();
    
    threadInit(&writerT,tcpserver,NULL);
    threadInit(&readerT,tcpclient,NULL);
    
    

    
    threadJoin(&writerT);
    threadJoin(&readerT);

    
    closeSocketPlatform();

//    testNetworkStack();
    
    logClose();
    
   

    hashtabletest();
    
    
    timerTest();
    
    bandwithTest();
    
    printf("End of main at %d!\n", getTicks());
    
    return 0;
}

