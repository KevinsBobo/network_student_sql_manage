#pragma once

#include <winsock2.h>
// win 初始化环境
#pragma comment(lib, "ws2_32.lib")

#define CONTROL_IP "127.0.0.1"
#define CONTROL_PORT 12306
#define SQLSERVER_IP "127.0.0.1"
#define SQLSERVER_PORT 12307

enum emSocketMsg
{
  // 客户端
  iam_clent = 0,
  sql_insert,
  sql_del,
  sql_find,
  sql_findall,
  sql_update,
  sql_getver,

  // 中间层
  iam_control = 50,

  // 服务器
  iam_sqlserver = 100,
  sql_insert_succ,
  sql_insert_fail,
  sql_del_succ,
  sql_del_fail,
  sql_find_succ,
  sql_find_fail,
  sql_findall_succ,
  sql_findall_fail,
  sql_update_succ,
  sql_update_fail,
  sql_retver,
};

inline BOOL MyWSAStartup()
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;
  
  wVersionRequested = MAKEWORD( 2, 2 );
  
  err = WSAStartup( wVersionRequested, &wsaData );
  if ( err != 0 ) {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    return FALSE;
  }
  
  /* Confirm that the WinSock DLL supports 2.2.*/
  /* Note that if the DLL supports versions greater    */
  /* than 2.2 in addition to 2.2, it will still return */
  /* 2.2 in wVersion since that is the version we      */
  /* requested.                                        */
  
  if ( LOBYTE( wsaData.wVersion ) != 2 ||
    HIBYTE( wsaData.wVersion ) != 2 ) {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    WSACleanup( );
    return FALSE; 
  }

  return TRUE;
}


inline char* RecivPacket(SOCKET sSocket)
{
    DWORD dwPacketSize = 0;
    BYTE eDataType;

    //接收DWORD的长度
    int nTotalRecivByte = 0;
    int nRealRecivByte = 0;
    int nShouldReciveByte = sizeof(DWORD);
    while (nTotalRecivByte < nShouldReciveByte)
    {
        nRealRecivByte =
            recv(sSocket, (char*)&dwPacketSize + nRealRecivByte, sizeof(DWORD)-nTotalRecivByte, 0);
        if (nRealRecivByte == 0 || nRealRecivByte == SOCKET_ERROR)
        {
            return NULL;
        }
        nTotalRecivByte += nRealRecivByte;
    }

    //创建数据包
    char* packet = new char[dwPacketSize + sizeof(DWORD)];
    *(DWORD*)packet = dwPacketSize;

    //接收eunm数据类型长度
    nTotalRecivByte = 0;
    nRealRecivByte = 0;
    nShouldReciveByte = sizeof(BYTE);
    while (nTotalRecivByte < nShouldReciveByte)
    {
        nRealRecivByte =
            recv(sSocket, (char*)&eDataType + nRealRecivByte, sizeof(BYTE)-nTotalRecivByte, 0);
        if (nRealRecivByte == 0 || nRealRecivByte == SOCKET_ERROR)
        {
            return NULL;
        }
        nTotalRecivByte += nRealRecivByte;
    }
    *(BYTE*)(packet + sizeof(DWORD)) = eDataType;

    //接收数据长度
    if(dwPacketSize > sizeof(BYTE))
    {
      nTotalRecivByte = 0;
      nRealRecivByte = 0;
      nShouldReciveByte = dwPacketSize - sizeof(BYTE);
      while (nTotalRecivByte < nShouldReciveByte)
      {
          nRealRecivByte =
              recv(sSocket, packet + sizeof(DWORD) + sizeof(BYTE) + nRealRecivByte, nShouldReciveByte - nTotalRecivByte, 0);
          if (nRealRecivByte == 0 || nRealRecivByte == SOCKET_ERROR)
          {               
              return NULL;
          }
          nTotalRecivByte += nRealRecivByte;
      }

    }
    //接收完毕
    return packet;
}

inline int SendPacket(SOCKET sSocket, BYTE emType, char* szSendData, int nSendDataBytes)
{
    //根据数据构造柔性数组
    int nTotaBytes = sizeof(DWORD)+sizeof(BYTE)+nSendDataBytes;
    char* packet =
        new char[nTotaBytes];
    if (packet == NULL)
    {
        return SOCKET_ERROR;
    }

    *(DWORD*)packet = nSendDataBytes + sizeof(BYTE);
    *(BYTE*)(packet + sizeof(DWORD)) = emType;
    if(szSendData != NULL)
    {
      memcpy(packet + sizeof(DWORD) + sizeof(BYTE), szSendData, nSendDataBytes);
    }

    //发送数据包
    int nTotoSentBytes = 0;
    int nSentBytes = 0;
    while (nTotoSentBytes < nTotaBytes)
    {
        nSentBytes = send(sSocket, (char*)packet + nSentBytes, nTotaBytes - nSentBytes, 0);
        if (nSentBytes == SOCKET_ERROR)
        {
            if (packet != NULL)
            {
                delete packet;
            }
            return SOCKET_ERROR;
        }
        nTotoSentBytes += nSentBytes;
    }

    if (packet != NULL)
    {
        delete packet;
    }
    
    return 0;
}
