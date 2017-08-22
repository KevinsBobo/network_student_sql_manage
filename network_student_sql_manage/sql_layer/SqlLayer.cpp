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
int g_nSqlVer = 0;


int GetRSCount(_RecordsetPtr rs, char* pszBuff)
{
  if(pszBuff == NULL)
  {
    return 0;
  }

  int nRul = 0;

  int nCount = rs->Fields->GetCount();

  memcpy_s(pszBuff , sizeof(int) , &nCount , 1);
  nRul += sizeof(int);

  CString strID , strName , strSex , strPhone , strHobby;
  int nStrLen = 0;
  while(!rs->MyEOF)
  {
    strID = rs->GetCollect("ID");
    strName = rs->GetCollect("name");
    strSex = rs->GetCollect("sex");
    strPhone = rs->GetCollect("phone");
    strHobby = rs->GetCollect("hobby");

    nStrLen = (strID.GetLength() + 1);
    memcpy_s(pszBuff + nRul , sizeof(UINT) , &nStrLen , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(pszBuff + nRul , 8 * 1024 , strID.GetBuffer() , nStrLen * sizeof(USHORT));
    nRul += nStrLen * sizeof(USHORT);

    nStrLen = (strName.GetLength() + 1);
    memcpy_s(pszBuff + nRul , sizeof(UINT) , &nStrLen , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(pszBuff + nRul , 8 * 1024 , strName.GetBuffer() , nStrLen * sizeof(USHORT));
    nRul += nStrLen * sizeof(USHORT);

    nStrLen = (strSex.GetLength() + 1);
    memcpy_s(pszBuff + nRul , sizeof(UINT) , &nStrLen , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(pszBuff + nRul , 8 * 1024 , strSex.GetBuffer() , nStrLen * sizeof(USHORT));
    nRul += nStrLen * sizeof(USHORT);

    nStrLen = (strPhone.GetLength() + 1);
    memcpy_s(pszBuff + nRul , sizeof(UINT) , &nStrLen , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(pszBuff + nRul , 8 * 1024 , strPhone.GetBuffer() , nStrLen * sizeof(USHORT));
    nRul += nStrLen * sizeof(USHORT);

    nStrLen = (strHobby.GetLength() + 1);
    memcpy_s(pszBuff + nRul , sizeof(UINT) , &nStrLen , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(pszBuff + nRul , 8 * 1024 , strHobby.GetBuffer() , nStrLen * sizeof(USHORT));
    nRul += nStrLen * sizeof(USHORT);


    rs->MoveNext();
  }

  return nRul;
}


void Dispose(char* pMess)
{
  if(pMess == NULL)
  {
    return ;
  }

  DWORD nLen = *(DWORD*)pMess;
  BYTE nType = *(BYTE*)(pMess + sizeof(DWORD));
  SOCKET s = *(SOCKET*)(pMess + sizeof(DWORD) + sizeof(BYTE));
  BYTE nRetType = -1;
  _RecordsetPtr rs;

  switch(nType)
  {
    case emSocketMsg::sql_insert:
      nRetType = emSocketMsg::sql_insert_fail;
      break;
    case emSocketMsg::sql_del:
      nRetType = emSocketMsg::sql_del_fail;
      break;
    case emSocketMsg::sql_find:
      nRetType = emSocketMsg::sql_find_fail;
      break;
    case emSocketMsg::sql_findall:
      nRetType = emSocketMsg::sql_findall_fail;
      break;
    case emSocketMsg::sql_getver:
      nRetType = emSocketMsg::sql_retver;
      break;
    default:
      break;
  }

  if(nType == sql_getver)
  {
    unsigned long long mess = 0;
    memcpy_s(&mess , sizeof(unsigned long) , &s , 1);
    memcpy_s((char*)&mess + sizeof(unsigned long), sizeof(unsigned long) , &g_nSqlVer , 1);

    SendPacket(g_sClient , nRetType , (char*)&mess , sizeof(unsigned long long));
    return ;
  }

  try
  {
    _variant_t t;
    rs = g_conn->Execute((TCHAR*)(pMess + sizeof(DWORD) + sizeof(BYTE) + sizeof(SOCKET)),
                         &t, adCmdText);

    if(nType == emSocketMsg::sql_insert
       || nType == emSocketMsg::sql_del
       || nType == emSocketMsg::sql_update)
    {
      ++g_nSqlVer;
      SendPacket(g_sClient , nRetType - 1 , (char*)&s , sizeof(SOCKET));
    }
    else
    {
      char* pszBuff = new char[ 8 * 1024 ];
      memset(pszBuff , 0 , 8 * 1024);

      memcpy_s(pszBuff , sizeof(SOCKET) , &s , 1);

      int nLen = GetRSCount(rs, pszBuff + sizeof(SOCKET));

      SendPacket(g_sClient , nRetType - 1 , pszBuff , nLen + sizeof(SOCKET));
    }

  }
  catch (_com_error e)
  {
    wprintf(L"%s\n" , e.ErrorMessage());
    SendPacket(g_sClient , nRetType , (char*)&s , sizeof(SOCKET));
  }

}

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


long InitDatabase()
{
  _variant_t t;
  printf("初始化数据库\n");

  try
  {
    g_conn->Execute("USE CR26_student_9527", &t, adCmdText);
    printf("初始化数据库成功！\n");
  }
  catch (_com_error e)
  {
    try
    {
      char szBuff[ 1024 ] = { 0 };
      char* pszPath = _getcwd(NULL, 0);
      sprintf_s(szBuff , 1024 ,
                "CREATE DATABASE CR26_student_9527 \
                ON (NAME = 'CR26_student_9527', FILENAME = '%s\\student_9527.mdf') \
                LOG ON (NAME = 'CR26_student_9527_log', FILENAME = '%s\\student_9527.ldf')" ,
                pszPath,
                pszPath
               );

      g_conn->Execute(szBuff , &t, adCmdText);
      g_conn->Execute("USE CR26_student_9527", &t, adCmdText);

      printf("初始化数据库成功！\n");
    }
    catch (_com_error e)
    {
      wprintf(L"%s\n" , e.ErrorMessage());
      return S_FALSE;
    }
  }

  return S_OK;
}


long InitTable()
{
  _variant_t t;
  printf("初始化数据表\n");

  try
  {
    g_conn->Execute("SELECT count(ID) FROM tab_student", &t, adCmdText);
    printf("初始化数据表成功！\n");
  }
  catch (_com_error e)
  {
    try
    {
      char* pszBuff = "CREATE TABLE tab_student \
                      ( \
                        ID int IDENTITY CONSTRAINT pk_stuid PRIMARY KEY, \
                        name VARCHAR(50) NOT NULL, \
                        sex INT NOT NULL, \
                        phone VARCHAR(30) NOT NULL, \
                        hobby VARCHAR(50), \
                        del INT, \
                        modify bigint \
                      ) \
                      ";

      g_conn->Execute(pszBuff , &t, adCmdText);
      g_conn->Execute("SELECT count(ID) FROM tab_student", &t, adCmdText);

      CString strSql;
      for(int i = 0; i < 12; ++i)
      {
        strSql.Format(TEXT("INSERT INTO tab_student (name, sex, phone, hobby) VALUES ('name%d', %d, '123%d', 'hobby%d')"),
                      i,
                      i % 2 == 0 ? 1 : i % 2,
                      i,
                      i);

        g_conn->Execute(strSql.GetBuffer() , &t, adCmdText);
      }
  

      printf("初始化数据表成功！\n");
    }
    catch (_com_error e)
    {
      wprintf(L"%s\n" , e.ErrorMessage());
      return S_FALSE;
    }
  }

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

  if(InitDatabase())
  {
    printf("初始化数据库错误，按任意键退出\n");
    getchar();
    return S_FALSE;
  }

  if(InitTable())
  {
    printf("初始化数据库错误，按任意键退出\n");
    getchar();
    return S_FALSE;
  }

  if(StartServer())
  {
    printf("SOCKET错误，按任意键退出\n");
    getchar();
    return S_FALSE;
  }

  while(1)
  {
    char* pMess = RecivPacket(g_sClient);
    if(pMess == NULL)
    {
      break;
    }

    Dispose(pMess);
  }

  g_conn->Close();

  CoUninitialize();
	return 0;
}