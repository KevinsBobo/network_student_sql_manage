// control_layer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MySocket.h"
#include "MyThreadPool.h"
#include "IMyCommand.h"

int _tmain(int argc, _TCHAR* argv[])
{
  if(!MyWSAStartup())
  {
    return -1;
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