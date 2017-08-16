// CommandManager.cpp: implementation of the CMyCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyCommandManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyCommandManager::CMyCommandManager()
{

}

CMyCommandManager::~CMyCommandManager()
{

}

BOOL CMyCommandManager::Insert(IMyCommand *pCmd)
{

  m_cs.Lock();
  m_Lst.AddTail(pCmd);
  m_cs.Unlock();

  return TRUE;
}

BOOL CMyCommandManager::Delete(IMyCommand *pCmd)
{


  return TRUE;
}

BOOL CMyCommandManager::GetCommand(IMyCommand*& pCmd)
{

  if (m_Lst.GetCount() <= 0)
  {
    pCmd = NULL;
    return FALSE;
  }

  m_cs.Lock();
  
  pCmd = m_Lst.RemoveHead();
  
  m_cs.Unlock();


  return TRUE;
}
