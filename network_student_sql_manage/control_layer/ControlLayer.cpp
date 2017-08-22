// control_layer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MySocket.h"
#include "MyThreadPool.h"
#include "IMyCommand.h"
#include "afxwin.h"
#include "MyCommand.h"


SOCKET g_Sck = INVALID_SOCKET;
// SOCKET g_sClient = INVALID_SOCKET; 
SOCKET g_sSqlServer = INVALID_SOCKET;
sockaddr_in g_addr = { 0 };
CList<SOCKET> g_ListSck;
CThreadPool pool;


void DisPose(SOCKET sClient, char* pMess)
{
  CMyCommand* cmd = new CMyCommand;
  cmd->m_sClient = sClient;
  cmd->m_pMess = pMess;
  cmd->m_sServer = g_sSqlServer;
  pool.Handle(cmd);
}

UINT AFX_CDECL DisSqlPoseThreadProc(LPVOID lpParameter)
{
  while(1)
  {
    char* pSqlMess = RecivPacket(g_sSqlServer);
    
    if(pSqlMess == NULL)
    {
      return 0;
    }

    DWORD nLen = *(DWORD*)pSqlMess;
    BYTE nType = *(BYTE*)(pSqlMess + sizeof(DWORD));

    printf("收到服务端长度为 %3u 的 %3d 消息\n", nLen , (int)nType);

    SOCKET s = *(SOCKET*)(pSqlMess + sizeof(DWORD) + sizeof(BYTE));

    SendPacket(s , nType ,
               pSqlMess + sizeof(DWORD) + sizeof(BYTE) + sizeof(SOCKET) ,
               nLen - sizeof(BYTE) - sizeof(SOCKET));

    delete pSqlMess;
  }

  return 0;
}


UINT AFX_CDECL RecvThreadProc(LPVOID lpParameter)
{
  SOCKET sClient = INVALID_SOCKET;
  int nLength = sizeof(sockaddr);
  char szBuff[ MAXBYTE ] = { 0 };
  int nRecv = 0;
  char* pMess = NULL;
  u_long UnBlockMode = 1;
  u_long BlockMode = 0;

  while(1)
  {
    POSITION pos = g_ListSck.GetHeadPosition();

    SOCKET sClient = INVALID_SOCKET;
    while(pos != NULL)
    {
      sClient = g_ListSck.GetNext(pos);
      ioctlsocket(sClient ,FIONBIO,&UnBlockMode);

      nRecv = recv(sClient , szBuff , 1 , MSG_PEEK);

      if(nRecv > 0)
      {
        ioctlsocket(sClient,FIONBIO,&BlockMode);
        pMess = RecivPacket(sClient);
        if(pMess == NULL)
        {
          POSITION pos = g_ListSck.Find(sClient , g_ListSck.GetHeadPosition());
          if(pos != NULL)
          {
            g_ListSck.RemoveAt(pos);
            printf("%u 连接断开\n", sClient);
          }
          continue;
        }

        // 处理消息
        DisPose(sClient, pMess);
      }

      nRecv = 0;
      pMess = NULL;
      sClient = INVALID_SOCKET;
    }
  }
}


UINT AFX_CDECL ConnectThreadProc(LPVOID lpParameter)
{
  SOCKET sClient = INVALID_SOCKET;
  int nLength = sizeof(sockaddr);
  while(1)
  {
    sClient = accept(g_Sck , (sockaddr*)&g_addr , &nLength);
    if(sClient == INVALID_SOCKET || sClient == NULL)
    {
      printf("连接错误\n");
      return S_FALSE;
    }
    g_ListSck.AddTail(sClient);

    printf("%u 连接成功\n", sClient);
  }
}


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
  printf("输入'D'退出中间层\n");

  // 4. 等待连接
  AfxBeginThread(ConnectThreadProc , NULL);

  // 5. 检查消息
  AfxBeginThread(RecvThreadProc , NULL);

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
    
    AfxBeginThread(DisSqlPoseThreadProc , NULL);

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
  BOOL bRet = pool.Create(4);

  if (!bRet)
  {
    return 0;
  }


  // pool.Handle(xxx);


  while(char c = getchar())
  {
    if(c == 'D')
    {
      break;
    }
  }

  pool.Destroy();

  POSITION pos = g_ListSck.GetHeadPosition();
  SOCKET sClient = INVALID_SOCKET;
  while(pos != NULL)
  {
    sClient = g_ListSck.GetNext(pos);
    closesocket(sClient);
    sClient = INVALID_SOCKET;
  }
  g_ListSck.RemoveAll();

  closesocket(g_sSqlServer);
  closesocket(g_Sck);
	return 0;
}