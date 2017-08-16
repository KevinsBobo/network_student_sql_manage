// CommandManager.h: interface for the CMyCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDMANAGER_H__49D699EC_93A8_4B56_848F_9EDCEAD26F2C__INCLUDED_)
#define AFX_COMMANDMANAGER_H__49D699EC_93A8_4B56_848F_9EDCEAD26F2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "IMyCommand.h"
//

class CMyCommandManager  
{
public:
	BOOL GetCommand(IMyCommand*& pCmd);
	BOOL Delete(IMyCommand* pCmd);
	BOOL Insert(IMyCommand* pCmd);
	CMyCommandManager();
	virtual ~CMyCommandManager();

  

private:
  CCriticalSection m_cs;
	CList<IMyCommand*, IMyCommand*> m_Lst;
};

#endif // !defined(AFX_COMMANDMANAGER_H__49D699EC_93A8_4B56_848F_9EDCEAD26F2C__INCLUDED_)
