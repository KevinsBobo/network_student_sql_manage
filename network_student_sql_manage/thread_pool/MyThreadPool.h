// ThreadPool1.h: interface for the CThreadPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADPOOL1_H__748F502D_9838_4983_ABF7_866795541EE1__INCLUDED_)
#define AFX_THREADPOOL1_H__748F502D_9838_4983_ABF7_866795541EE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMyCommand.h"
#include "MyCommandManager.h"

class CThreadPool  
{
public:
	static DWORD ThreadFunc(LPVOID lpParam);
	BOOL Handle(IMyCommand* pCmd);
	BOOL Destroy();
	BOOL Create(int nCount);
	CThreadPool();
	virtual ~CThreadPool();

private:
  int m_nCount;
  HANDLE m_hSemaphore;
  HANDLE* m_hThreadAry;
  HANDLE m_hExitEvent;
  CMyCommandManager m_mgr;
};

#endif // !defined(AFX_THREADPOOL1_H__748F502D_9838_4983_ABF7_866795541EE1__INCLUDED_)
