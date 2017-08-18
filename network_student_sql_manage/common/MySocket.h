#pragma once

#include <winsock.h>
// win 初始化环境
#pragma comment(lib, "ws2_32.lib")

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
