#include "pch.h"
#include <stdio.h>
#include <winsock.h>
#pragma comment(lib,"wsock32.lib") 
#pragma warning(disable:4996)

// 执行函数
void DataGramServer(short nPort);

// 打印错误宏定义
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%s: %d\n", s, WSAGetLastError())



int main(int argc, char **argv)
{
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    int nRet;
    short nPort;

    //
    // 检查参数是否足够
    //
    if (argc != 2)
    {
        fprintf(stderr, "\n需要参数: 端口号\n");
        return 0;
    }

    nPort = atoi(argv[1]);

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
    DataGramServer(nPort);
    //
    // 释放 WinSock
    //
    WSACleanup();
}



void DataGramServer(short nPort)
{
    //
    // 创建一个UDP socket 并从 client 等待数据
    //
    SOCKET	ServerSocket;

    ServerSocket = socket(AF_INET,			// Address family
        SOCK_DGRAM,		                    // Socket type
        IPPROTO_UDP);		                // Protocol
    if (ServerSocket == INVALID_SOCKET)
    {
        PRINTERROR("socket()");
        return;
    }
    //
    // 填充地址结构体
    //
    SOCKADDR_IN saServer;

    saServer.sin_family = AF_INET;
    saServer.sin_addr.s_addr = INADDR_ANY;	// Let WinSock supply address
    saServer.sin_port = htons(nPort);		// Use port from command line
    //
    // 绑定 socket
    //
    int nRet;
    nRet = bind(ServerSocket,				// Socket 
        (LPSOCKADDR)&saServer,		        // Our address
        sizeof(struct sockaddr));	        // Size of address structure
    if (nRet == SOCKET_ERROR)
    {
        PRINTERROR("bind()");
        closesocket(ServerSocket);
        return;
    }


    //
    // server 与 clien 通信实例
    //
    int nLen;
    nLen = sizeof(SOCKADDR);
    char szBuf[4096];

    nRet = gethostname(szBuf, sizeof(szBuf));
    if (nRet == SOCKET_ERROR)
    {
        PRINTERROR("gethostname()");
        closesocket(ServerSocket);
        return;
    }

    //
    // 展示 server IP以及端口号
    //
    CHAR szClientIP[128], szClientPort[64];
    //strcpy(szClientIP, inet_ntoa(saServer.sin_addr));
    strcpy(szClientPort, itoa(ntohs(saServer.sin_port), szClientPort ,10));

    struct hostent *phostinfo = gethostbyname("");
    char *p = inet_ntoa(*((struct in_addr *)(*phostinfo->h_addr_list)));
    strcpy(szClientIP, p);

    printf("\n本地主机[%s](%s:%s)UDP端口[%d]，正在等候客户端．．．\n",
        szBuf, szClientIP, szClientPort, nPort);

    printf("IP列表:\n");
    int j = 0;
    for (j = 0; NULL != phostinfo && NULL != phostinfo->h_addr_list[j]; ++j)
    {
        char *pszAddr = inet_ntoa(*(struct in_addr *)phostinfo->h_addr_list[j]);
        printf("%s\n", pszAddr);
    }

    //
    // 从 client 接收数据
    //
    memset(szBuf, 0, sizeof(szBuf));
    nRet = recvfrom(ServerSocket,
        szBuf,						    // Receive buffer
        sizeof(szBuf),					// Lenght of buffer
        0,                              // Flags
        (LPSOCKADDR)&saServer,
        &nLen);

    if (nRet == INVALID_SOCKET)
    {
        PRINTERROR("recvfrom()");
        closesocket(ServerSocket);
        return;
    }

    //
    // 显示接收到的数据
    //
    printf("\n接收数据： %s", szBuf);
    printf("[%s:%d]\n", inet_ntoa((struct in_addr)saServer.sin_addr), ntohs(saServer.sin_port));
    //
    // 给 client 发送数据
    //
    char data[3072];
    int i = 0;
    while (i < 3072)
    {
        data[i] = 'S';
        i++;
    }
    data[3071] = '\0';
    strcpy(szBuf, data);
    printf("发送数据： %s\n", szBuf);
    nRet = sendto(ServerSocket,
        szBuf,					    // Data buffer
        strlen(szBuf) + 1,			// Lenght of data
        0,                          // Flags
        (LPSOCKADDR)&saServer,
        sizeof(struct sockaddr));

    //
    // 退出前关闭 sockets 
    //
    closesocket(ServerSocket);
    return;
}

