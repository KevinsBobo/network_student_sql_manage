#pragma once
#include <afxstr.h>

struct InputInfo
{
  unsigned int nId;
  CString strName;
  int nSex;
  CString strPhone;
  CString strHobby;

  inline InputInfo()
    : nId(0)
    , strName("")
    , nSex(0)
    , strPhone("")
    , strHobby("")
  {}
};