// sql_layer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MySocket.h"


int _tmain(int argc, _TCHAR* argv[])
{
  if(MyWSAStartup())
  {
    return -1;
  }

	return 0;
}

