// IMyCommand.h: interface for the ICommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOMMAND_H__358C0861_2A23_4F67_9CA5_5978CBC71877__INCLUDED_)
#define AFX_ICOMMAND_H__358C0861_2A23_4F67_9CA5_5978CBC71877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IMyCommand  
{
public:
	virtual BOOL Exec() = 0;
	IMyCommand();
	virtual ~IMyCommand();

};

#endif // !defined(AFX_ICOMMAND_H__358C0861_2A23_4F67_9CA5_5978CBC71877__INCLUDED_)
