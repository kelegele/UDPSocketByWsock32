#include "pch.h"
#include <stdio.h>
#include <winsock.h>
#pragma comment(lib,"wsock32.lib") 
#pragma warning(disable:4996)

// 执行函数
void DataGramClient(short nPort, char* nIP);

// 打印错误宏定义
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%s: %d\n", s, WSAGetLastError())


int main(int argc, char **argv)
{
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    int nRet;
    short nPort;
    char* nIP;

    //
    // 检查端口参数
    //
    if (argc != 3)
    {
        fprintf(stderr, "\n需要参数: server IP 和 端口号\n");

        return 0;
    }

    nPort = atoi(argv[2]);
    nIP = argv[1];

    //
    // 初始化 WinSock 并检查版本
    //
    nRet = WSAStartup(wVersionRequested, &wsaData);
    if (wsaData.wVersion != wVersionRequested)
    {
        fprintf(stderr, "\n Wrong version\n");
        return 0;
    }
    //
    // 执行
    //
    DataGramClient(nPort, nIP);
    //
    // 释放 WinSock
    WSACleanup();
}


void DataGramClient(short nPort, char* nIP)
{
    //
    // 创建一个 UDP socket 并等待数据
    //
    SOCKET	ClientSocket = socket(AF_INET, SOCK_DGRAM, 0);;

    // Address family
    // Socket type
    // Protocol
    if (ClientSocket == INVALID_SOCKET)
    {
        PRINTERROR("socket()");
        return;
    }
    //
    // 填充地址结构体
    //
    SOCKADDR_IN saClient;

    saClient.sin_family = AF_INET;
    //saClient.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");	// Let WinSock supply address
    //saClient.sin_port = htons(8888);		                // Use port from command line
    saClient.sin_addr.S_un.S_addr = inet_addr(nIP);	
    saClient.sin_port = htons(nPort);

    //
    // 向 server 发送数据 
    //
    char sendBuf[4096];
    char data[3072];
    int i = 0;
    while (i < 3072)
    {
        data[i] = 'C';
        i++;
    }
    data[3071] = '\0';
    strcpy(sendBuf, data);
    sendto(ClientSocket,
        sendBuf,
        strlen(sendBuf) + 1,
        0,
        (SOCKADDR*)&saClient,
        sizeof(SOCKADDR));
    printf("发送数据： %s\n", sendBuf);

    int nLen;
    nLen = sizeof(SOCKADDR);
    char szBuf[4096];
    memset(szBuf, 0, sizeof(szBuf));
    recvfrom(ClientSocket,
        szBuf,						    // Receive buffer
        sizeof(szBuf),					// Lenght of buffer
        0,                              // Flags
        (LPSOCKADDR)&saClient,
        &nLen);
    printf("\n接收数据： %s", szBuf);
    printf("[%s:%d]\n", inet_ntoa((struct in_addr)saClient.sin_addr), ntohs(saClient.sin_port));
    //
    // 退出前关闭 sockets
    //
    closesocket(ClientSocket);
    return;
}