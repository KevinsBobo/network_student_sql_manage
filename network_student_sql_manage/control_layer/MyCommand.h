#pragma once
#include "IMyCommand.h"
#include "MySocket.h"

class CMyCommand :
  public IMyCommand
{
public:
  CMyCommand();
  virtual ~CMyCommand();
  virtual BOOL Exec();
  SOCKET m_sClient;
  SOCKET m_sServer;
  char* m_pMess;
};

