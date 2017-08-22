#include "stdafx.h"
#include "MyCommand.h"


CMyCommand::CMyCommand()
  : m_sClient(INVALID_SOCKET)
  , m_sServer(INVALID_SOCKET)
  , m_pMess(NULL)
{
}


CMyCommand::~CMyCommand()
{
}


BOOL CMyCommand::Exec()
{
  if(m_sClient == INVALID_SOCKET
     || m_sClient == NULL
     || m_pMess == NULL)
  {
    return FALSE;
  }

  DWORD nLen  = 0;
  BYTE  nType = 0;

  nLen = *(DWORD*)m_pMess;
  nType = *(BYTE*)(m_pMess + sizeof(DWORD));

  memset(m_pMess + sizeof(BYTE) , 0 , sizeof(SOCKET));
  memcpy_s((m_pMess + sizeof(BYTE)) , sizeof(SOCKET) , &m_sClient , 1);

  SendPacket(m_sServer , nType , m_pMess + sizeof(BYTE) , nLen + sizeof(SOCKET));

  printf("收到客户端长度为 %3u 的 %3d 消息\n", nLen , (int)nType);


  delete m_pMess;
  return TRUE;
}
