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
    UInt32 sended;
        
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
    UInt8 buffer[16];
    UInt32 received;
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
    
    wait(300);
    
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

    hashTableInit(&table,16);
    
    node = 8;
    base = 0x00001000;
    
    UInt32 data;
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
    
    data = (UInt32)hashTableLookup(&table,base+4);
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
    printf("=== Timer test ===\n");
    
    printf("Timer current: %d\n", getTicks());
    
    
    Timer timer;
    
    timerInit(&timer,300,500,callback,0);
    timerStart(&timer);
    
    waitMs(10000);
    
    
    timerStop(&timer);
    
    printf("Timer current: %d\n", getTicks());
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
    
    printf("Size of Int32: %d\n", (Int32)sizeof(Int32));
    printf("Size of Int64: %ld\n", sizeof(Int64));
    
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
    
    printf("End of main at %d!\n", getTicks());
    
    return 0;
}

