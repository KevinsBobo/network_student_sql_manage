// control_layer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MySocket.h"
#include "MyThreadPool.h"
#include "IMyCommand.h"


SOCKET g_Sck = INVALID_SOCKET;
SOCKET g_sClient = INVALID_SOCKET; 
SOCKET g_sSqlServer = INVALID_SOCKET;
sockaddr_in g_addr = { 0 };


long StartServer()
{
  // 1. 创建一个套接字 socket
  g_Sck = socket(AF_INET , // 网络协议簇
                 SOCK_STREAM , // "流式" -- TCP, SOCK_DGRAM "数据报" -- UDP
                 IPPROTO_TCP);
  if(g_Sck == INVALID_SOCKET)
  {
    return S_FALSE;
  }

  // 2. 绑定IP 和 端口 bind
  // sockaddr_in addr; // 一个14字节大的结构体
  // 可用性更高的结构体
  g_addr.sin_family = AF_INET;
  g_addr.sin_addr.S_un.S_addr = 0; // IP地址
  g_addr.sin_port = htons(CONTROL_PORT); // htons() 将主机字节序转为网络序，s代表sort
                               // 网络序为大端序


  int nRet = bind(g_Sck , (sockaddr*)&g_addr , sizeof(sockaddr));
  if(nRet == SOCKET_ERROR)
  {
    printf("绑定错误\n");
    return S_FALSE;
  }

  // 3. 监听 listen
  nRet = listen(g_Sck , SOMAXCONN);
  if(nRet == SOCKET_ERROR)
  {
    printf("监听错误！\n");
    return S_FALSE;
  }

  printf("监听成功，等待连接\n");

  // 4. 等待连接
  int nLength = sizeof(sockaddr);
  g_sClient = accept(g_Sck , (sockaddr*)&g_addr , &nLength);
  if(g_sClient == INVALID_SOCKET)
  {
    printf("连接错误\n");
    return S_FALSE;
  }

  printf("连接成功\n");
  return S_OK;
}


long ConnSQLServer()
{
  // 1. 创建一个套接字 socket
  g_sSqlServer = socket(AF_INET , // 网络协议簇
             SOCK_STREAM , // "流式" -- TCP, SOCK_DGRAM "数据报" -- UDP
             IPPROTO_TCP);
  if(g_sSqlServer == INVALID_SOCKET)
  {
    printf("创建SOCKET失败\n");
    return S_FALSE;
  }

  // 2. 设置服务端 IP 和 端口 // 客户端不需要绑定
  sockaddr_in addr; // 一个14字节大的结构体
  addr.sin_family = AF_INET;
  addr.sin_addr.S_un.S_addr = inet_addr(SQLSERVER_IP); // IP地址
  addr.sin_port = htons(SQLSERVER_PORT); // htons() 将主机字节序转为网络序，s代表sort

  // 3. 连接服务端 connect
  printf("连接SQL端\n");
  while(1)
  {
    int nRet = connect(g_sSqlServer , (sockaddr*)&addr , sizeof(sockaddr));
    if(nRet == SOCKET_ERROR)
    {
      continue;
    }

    printf("连接成功\n");
    break;
  }

  return S_OK;
}


int _tmain(int argc, _TCHAR* argv[])
{
  if(!MyWSAStartup())
  {
    printf("SOCKET错误，按任意键退出\n");
    getchar();
    return S_FALSE;
  }

  if(ConnSQLServer())
  {
    printf("连接失败，按任意键退出\n");
    getchar();
    return S_FALSE;
  }

  if(StartServer())
  {
    printf("SOCKET错误，按任意键退出\n");
    getchar();
    return S_FALSE;
  }
  


  //创建线程池
  CThreadPool pool;
  BOOL bRet = pool.Create(5);

  if (!bRet)
  {
    return 0;
  }


  // pool.Handle(xxx);

  Sleep(100);

  pool.Destroy();


  getchar();

	return 0;
}