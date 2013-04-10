//
//  main.c
//  babcode-test
//
//  Created by Baptiste on 08/04/13.
//  Copyright (c) 2013 Baptiste. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>

#include "../babcode/include/babcode.h"

static int dataAvailable=0;

static Mutex myMutex;
static ConditionVar myCondition;

int myRead(char * buffer, int size)
{
    int s;
    MutexLock(&myMutex);
    
    while(dataAvailable==0)
    {
        ConditionVarWait(&myCondition, &myMutex);
    }
    
    s = dataAvailable;
    dataAvailable = 0;
    
    MutexUnlock(&myMutex);
    ConditionVarSignal(&myCondition);
    
    return s;
}

int myWrite(const char * buffer, int size)
{
    MutexLock(&myMutex);
    
    while (dataAvailable!=0)
    {
        ConditionVarWait(&myCondition, &myMutex);
    }
    
    dataAvailable = size;
    
    MutexUnlock(&myMutex);
    ConditionVarSignal(&myCondition);
    
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
    
    ThreadExit();
    return NULL;
}

void * writer(void *param)
{
    int i;
    
    printf("--> writer !\n");
    
    for(i=0; i < 4; i++)
    {
        myWrite(0, 3+i);
        
        Wait(200);
    }
    
    ThreadExit();
    return NULL;
}

void * tcpserver(void *p)
{
    Socket server;
    Socket client;
    IpAddress clientAddr;
    UInt32 sended;
        
    SocketTcpInit(&server);
    ServerTcpOpen(&server, 1234);
    ServerTcpWaitConnection(&server, &client, &clientAddr);
    
    printf("Client connected: from %d.%d.%d.%d:%d\n", clientAddr.a,clientAddr.b,clientAddr.c,clientAddr.d,clientAddr.port);
    
    Wait(2000);
    SocketTcpSend(&client, "toto", 5, &sended);
    
    SocketClose(&server);
    SocketClose(&client);
    
    printf("-> end tcp server\n");
    
    ThreadExit();
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
    
    SocketTcpInit(&client);

    SocketSetTimeout(&client, 100, 100);
    
    do
    {
        status=ClientTcpOpen(&client,&serverAddr);
        Wait(1);
    } while (status==NETWORK_TIMEOUT);
    
    printf("Client connection status: %d\n", status);
    
    do
    {
        status = SocketTcpReceive(&client, buffer, 16, &received);
        Wait(1);
    } while (status==NETWORK_TIMEOUT);
        
    printf("Receive: %d bytes %s\n", received, buffer);
    
    Wait(300);
    
    SocketClose(&client);
    
    printf("-> end tcp client\n");
    
    ThreadExit();
    return NULL;
}



int main(int argc, const char * argv[])
{    
    Time time;
    
    Thread writerT, readerT;
    
    // insert code here...
    printf("Hello, World!\n");
    
    char AbsCmd[256];
    int lAbsCmd;
    
    lAbsCmd = sizeof(AbsCmd);
    GetRealPath(argv[0], AbsCmd, &lAbsCmd);
    
    printf("argv0 %s\n",argv[0]);
    printf("abscmd %s\n", AbsCmd);

    GetTime(&time);
    printf("Time: %d.%d.%d - %d.%d.%d\n", time.day, time.month, time.year, time.hour, time.minute, time.second);
    printf("isBissextile ? %d\n", time.isBissextile);
    
    printf("Size of Int32: %d\n", (Int32)sizeof(Int32));
    printf("Size of Int64: %ld\n", sizeof(Int64));
    
    LogOpen("/Users/baptiste/Desktop/Baptiste/projects/babcode/test/test.log");
    
    LOG_ERR1("mouarf");
    LOG("COUCOU\n");
    
    
    
    MutexInit(&myMutex);
    ConditionVarInit(&myCondition);
    

    ThreadInit(&writerT,writer,NULL);
    ThreadInit(&readerT,reader,NULL);
    
    ThreadJoin(&writerT);
    ThreadJoin(&readerT);
    
    printf("---> test socket...\n");
    
    InitSocketPlatform();
    
    ThreadInit(&writerT,tcpserver,NULL);
    ThreadInit(&readerT,tcpclient,NULL);
    
    

    
    ThreadJoin(&writerT);
    ThreadJoin(&readerT);

    
    CloseSocketPlatform();

//    testNetworkStack();
    
    LogClose();
    
    printf("End of main!\n");
    
    return 0;
}

