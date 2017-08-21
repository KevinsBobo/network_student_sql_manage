// sql_layer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MySocket.h"
#include <direct.h>
#include <comutil.h>
#include <comdef.h>

#import "C:/Program Files (x86)/Common Files/System/ado/msado15.dll" no_namespace rename("EOF", "MyEOF")


SOCKET g_Sck = INVALID_SOCKET;
SOCKET g_sClient = INVALID_SOCKET; 
_ConnectionPtr g_conn;


long StartServer()
{
  if(!MyWSAStartup())
  {
    return S_FALSE;
  }

  // 1. 创建一个套接字 socket
  g_Sck = socket(AF_INET , // 网络协议簇
                 SOCK_STREAM , // "流式" -- TCP, SOCK_DGRAM "数据报" -- UDP
                 IPPROTO_TCP);
  if(g_Sck == INVALID_SOCKET)
  {
    return S_FALSE;
  }

  // 2. 绑定IP 和 端口 bind
  sockaddr_in addr; // 一个14字节大的结构体
   // 可用性更高的结构体
  addr.sin_family = AF_INET;
  addr.sin_addr.S_un.S_addr = 0; // IP地址
  addr.sin_port = htons(SQLSERVER_PORT); // htons() 将主机字节序转为网络序，s代表sort
                               // 网络序为大端序


  int nRet = bind(g_Sck , (sockaddr*)&addr , sizeof(sockaddr));
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
  g_sClient = accept(g_Sck , (sockaddr*)&addr , &nLength);
  if(g_sClient == INVALID_SOCKET)
  {
    printf("连接错误\n");
    return S_FALSE;
  }

  printf("连接成功\n");
  return S_OK;
}


long ConnDataBase()
{
	//"adodb.Connection"
	//_uuidof(Connection)
	HRESULT hr = g_conn.CreateInstance(_uuidof(Connection));
	if (FAILED(hr)){
		return S_FALSE;
	}
	// OutputDebugString(TEXT("数据库对象实例初始化成功\r\n"));

  char szPath[ MAXBYTE ] = "File Name=";
  char *pszPath = NULL;

	try{
    pszPath = _getcwd(NULL , 0);
    if(pszPath == NULL)
    {
      return FALSE;
    }
    strcat_s(szPath , MAXBYTE , pszPath);
    strcat_s(szPath , MAXBYTE , "\\connect.udl");
    // OutputDebugStringA(szPath);
		hr = g_conn->Open( szPath , "", "", adConnectUnspecified);
		if (FAILED(hr))
		{
			printf("conn err");
      return S_FALSE;
		}
	}
	catch (_com_error e){
		//
		_bstr_t bstr = e.Description();
		wprintf(L"%s\n", bstr);
    return S_FALSE;
	}

  printf("数据库连接成功\n");
	return S_OK;
}


int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);

  if(ConnDataBase())
  {
    printf("连接数据库错误，按任意键退出\n");
    getchar();
    return S_FALSE;
  }

  if(StartServer())
  {
    printf("SOCKET错误，按任意键退出\n");
    getchar();
    return S_FALSE;
  }

  getchar();

  CoUninitialize();
	return 0;
}

