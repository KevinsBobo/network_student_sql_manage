sql_insert
refresh
findall
find
update
del
insert
insert
optdone
optdone
refresh
refresh
update
insert

// client_layerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "client_layer.h"
#include "client_layerDlg.h"
#include "afxdialogex.h"
#include "enum.h"
#include "MySocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClientLayerDlg 对话框



CClientLayerDlg::CClientLayerDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(CClientLayerDlg::IDD , pParent)
  , m_strName(_T(""))
  , m_nSex(0)
  , m_strPhone(_T(""))
  , m_strHobby(_T(""))
  , m_sServer(INVALID_SOCKET)
  , m_nSqlVer(-1)
  , m_nPreOpt(-1)
  , m_nCurOpt(-1)
  , m_pInfo(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientLayerDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX , IDC_LIST_MAIN , m_ListMain);
  DDX_Control(pDX , IDOK , m_btnOK);
  DDX_Control(pDX , IDC_BUTTON_REFRESH , m_btnRefresh);
  DDX_Control(pDX , IDC_BUTTON_LOOKALL , m_btnLookAll);
  DDX_Control(pDX , IDC_BUTTON_UPDATE , m_btnUpdate);
  DDX_Control(pDX , IDC_BUTTON_INSERT , m_btnInsert);
  DDX_Control(pDX , IDC_BUTTON_DEL , m_btnDelete);
  DDX_Control(pDX , IDC_BUTTON_PAGEUP , m_btnPageUp);
  DDX_Control(pDX , IDC_BUTTON_PAGEDOWN , m_btnPageDown);
  DDX_Control(pDX , IDC_EDIT_ID , m_EditID);
  DDX_Control(pDX , IDC_EDIT_NAME , m_EditName);
  DDX_Control(pDX , IDC_COMBO_SEX , m_ComboSex);
  DDX_Control(pDX , IDC_COMBO_PAGE_INFO , m_ComboPageInfo);
  DDX_Control(pDX , IDC_EDIT_PHONE , m_EditPhone);
  DDX_Control(pDX , IDC_EDIT_HOBBY , m_ListHobby);
  DDX_Control(pDX , IDC_STATIC_INFO , m_StaticTextInfo);
  DDX_Control(pDX , IDC_PROGRESS , m_Progress);
  //  DDX_Text(pDX , IDC_EDIT_ID , m_nId);
  DDX_Text(pDX , IDC_EDIT_NAME , m_strName);
  DDX_CBIndex(pDX , IDC_COMBO_SEX , m_nSex);
  DDX_Text(pDX , IDC_EDIT_PHONE , m_strPhone);
  DDX_Text(pDX , IDC_EDIT_HOBBY , m_strHobby);
}

BEGIN_MESSAGE_MAP(CClientLayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDOK , &CClientLayerDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL , &CClientLayerDlg::OnBnClickedCancel)
  ON_WM_TIMER()
  ON_MESSAGE(WM_MYMESSAGE, &CClientLayerDlg::OnMyMessage)
  ON_BN_CLICKED(IDC_BUTTON_INSERT , &CClientLayerDlg::OnBnClickedButtonInsert)
  ON_BN_CLICKED(IDC_BUTTON_LOOKALL , &CClientLayerDlg::OnBnClickedButtonLookall)
  ON_BN_CLICKED(IDC_BUTTON_UPDATE , &CClientLayerDlg::OnBnClickedButtonUpdate)
  ON_BN_CLICKED(IDC_BUTTON_REFRESH , &CClientLayerDlg::OnBnClickedButtonRefresh)
  ON_BN_CLICKED(IDC_BUTTON_DEL , &CClientLayerDlg::OnBnClickedButtonDel)
//  ON_NOTIFY(LVN_ITEMCHANGED , IDC_LIST_MAIN , &CClientLayerDlg::OnLvnItemchangedListMain)
ON_NOTIFY(NM_CLICK , IDC_LIST_MAIN , &CClientLayerDlg::OnNMClickListMain)
END_MESSAGE_MAP()


// CClientLayerDlg 消息处理程序

BOOL CClientLayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
  // 初始化界面
  InitInterface();
  // 连接中间层
  ConnectControl();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CClientLayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CClientLayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientLayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 初始化界面
void CClientLayerDlg::InitInterface()
{
  InitList();
  DisabledAllBtn();

  // 性别控件
  m_ComboSex.ResetContent();
  m_ComboSex.InsertString(emComboSex::unkonw, TEXT("未知"));
  m_ComboSex.InsertString(emComboSex::man, TEXT("男"));
  m_ComboSex.InsertString(emComboSex::woman, TEXT("女"));
  m_ComboSex.SetCurSel(emComboSex::unkonw);

  // 设置进度条定时器
  StartProgress();
  
}

void CClientLayerDlg::InitList()
{
  m_ListMain.ModifyStyle(0, LVS_REPORT);
  DWORD dwStye = m_ListMain.GetExtendedStyle();
  dwStye = dwStye | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
  m_ListMain.SetExtendedStyle(dwStye);
  m_ListMain.InsertColumn(emListMain::id , _T("ID"), 0, 60);
  m_ListMain.InsertColumn(emListMain::name , _T("姓名"), 0, 100);
  m_ListMain.InsertColumn(emListMain::sex , _T("性别"), 0, 60);
  m_ListMain.InsertColumn(emListMain::phone , _T("电话"), 0, 195);
  m_ListMain.InsertColumn(emListMain::hobby , _T("爱好"), 0, 200);
}


void CClientLayerDlg::OnBnClickedCancel()
{
  // TODO:  在此添加控件通知处理程序代码
  CDialogEx::OnCancel();
}



void CClientLayerDlg::DisabledAllBtn(BOOL isDis)
{
  if(isDis)
  {
    m_btnOK.EnableWindow(FALSE);
    m_btnRefresh.EnableWindow(FALSE);
    m_btnLookAll.EnableWindow(FALSE);
    m_btnUpdate.EnableWindow(FALSE);
    m_btnInsert.EnableWindow(FALSE);
    m_btnDelete.EnableWindow(FALSE);
    m_btnPageUp.EnableWindow(FALSE);
    m_btnPageDown.EnableWindow(FALSE);
    m_ComboPageInfo.EnableWindow(FALSE);
  }
  else
  {
    m_btnOK.EnableWindow(TRUE);
    m_btnRefresh.EnableWindow(TRUE);
    m_btnLookAll.EnableWindow(TRUE);
    m_btnUpdate.EnableWindow(TRUE);
    m_btnInsert.EnableWindow(TRUE);
    m_btnDelete.EnableWindow(TRUE);
    m_btnPageUp.EnableWindow(TRUE);
    m_btnPageDown.EnableWindow(TRUE);
    m_ComboPageInfo.EnableWindow(TRUE);
  }
}



void CClientLayerDlg::ManageInputInfo(InputInfo& input , bool isInput)
{
  CString strId;
  if(isInput)
  {
    UpdateData(TRUE);
    input.nId = 0;
    input.nSex = 0;
    input.strName.Empty();
    input.strPhone.Empty();
    input.strHobby.Empty();

    m_EditID.GetWindowTextW(strId);
    if(strId.IsEmpty())
    {
      input.nId = 0;
    }
    else
    {
      input.nId = _wtoi(strId.GetBuffer());
    }
    input.nSex = m_nSex;
    input.strName = m_strName;
    input.strPhone = m_strPhone;
    input.strHobby = m_strHobby;
  }
  else
  {
    strId.Format(TEXT("%d") , input.nId);
    m_EditID.SetWindowTextW(strId);
    m_nSex = input.nSex;
    m_strName = input.strName;
    m_strPhone = input.strPhone;
    m_strHobby = input.strHobby;
    UpdateData(FALSE);
  }
}


void CClientLayerDlg::OnTimer(UINT_PTR nIDEvent)
{
  // TODO:  在此添加消息处理程序代码和/或调用默认值
  switch(nIDEvent)
  {
    case emTimer::progress:
    {
      int nNum = m_Progress.GetPos();
      m_Progress.SetPos((++nNum) % 101);
    }
      break;
    default:
      break;
  }

  CDialogEx::OnTimer(nIDEvent);
}


afx_msg LRESULT CClientLayerDlg::OnMyMessage(WPARAM wParam , LPARAM lParam)
{
  switch(wParam)
  {
    case emMessage::connected:
      StopProgress();
      m_StaticTextInfo.SetWindowTextW(L"连接成功！");
      m_EditID.Clear();
      m_sServer = lParam;
      DisabledAllBtn(FALSE);
      break;
    case emMessage::connect_fail:
      StopProgress();
      m_StaticTextInfo.SetWindowTextW(L"连接失败，初始化网络错误！");
      m_EditID.Clear();
      break;
    case emMessage::optdone:
      StopProgress();
      DisabledAllBtn(FALSE);
      m_StaticTextInfo.SetWindowTextW((TCHAR*)lParam);
      break;
    case emMessage::recvinfo:
      DisposeRecv((char*)lParam);
      delete (char*)lParam;
      break;
    default:
      break;
  }

  return S_OK;
}


UINT AFX_CDECL CClientLayerDlg::ConnectThreadProc(LPVOID lpParameter)
{
  CClientLayerDlg* obj = (CClientLayerDlg*)lpParameter;
  SOCKET s = INVALID_SOCKET;

  // 1. 创建一个套接字 socket
  s = socket(AF_INET , // 网络协议簇
             SOCK_STREAM , // "流式" -- TCP, SOCK_DGRAM "数据报" -- UDP
             IPPROTO_TCP);
  if(s == INVALID_SOCKET)
  {
    obj->SendMessage(WM_MYMESSAGE , emMessage::connect_fail);
    return S_FALSE;
  }

  // 2. 设置服务端 IP 和 端口 // 客户端不需要绑定
  sockaddr_in addr; // 一个14字节大的结构体
  addr.sin_family = AF_INET;
  addr.sin_addr.S_un.S_addr = inet_addr(CONTROL_IP); // IP地址
  addr.sin_port = htons(CONTROL_PORT); // htons() 将主机字节序转为网络序，s代表sort

  // 3. 连接服务端 connect
  while(1)
  {
    int nRet = connect(s , (sockaddr*)&addr , sizeof(sockaddr));
    if(nRet == SOCKET_ERROR)
    {
      continue;
    }

    obj->SendMessage(WM_MYMESSAGE , emMessage::connected, s);
    break;
  }

  return S_OK;
}

void CClientLayerDlg::ConnectControl()
{
  AfxBeginThread(ConnectThreadProc ,
                 (LPVOID)this);
}


void CClientLayerDlg::StartProgress()
{
  m_Progress.ShowWindow(SW_SHOW);
  m_Progress.SetPos(0);
  SetTimer(emTimer::progress , 10, NULL);
}


void CClientLayerDlg::StopProgress()
{
  m_Progress.ShowWindow(SW_HIDE);
  m_Progress.SetPos(0);
  KillTimer(emTimer::progress);
}


void CClientLayerDlg::OnBnClickedButtonInsert()
{
  // TODO:  在此添加控件通知处理程序代码
  InputInfo* pInfo = new InputInfo;
  if(pInfo == NULL)
  {
    return;
  }

  ManageInputInfo(*pInfo);

  do{
    if(pInfo->strName.IsEmpty())
    {
      MessageBox(TEXT("请输入姓名！"));
    }
    else if(pInfo->nSex == 0)
    {
      MessageBox(TEXT("请选择性别！"));
    }
    else if(pInfo->strPhone.IsEmpty())
    {
      MessageBox(TEXT("请输入电话！"));
    }
    else if(pInfo->strHobby.IsEmpty())
    {
      MessageBox(TEXT("请输入爱好！"));
    }
    else
    {
      break;
    }
    
    m_StaticTextInfo.SetWindowTextW(L"输入有误！");
    delete pInfo;
    return ;
  } while(0);

  m_pInfo = pInfo;
  m_nCurOpt = emOpt::insert;

  AfxBeginThread(OptThreadProc , (LPVOID)this);

  DisabledAllBtn();
  StartProgress();
}


void CClientLayerDlg::OnBnClickedOk()
{
  // TODO:  在此添加控件通知处理程序代码
  // CDialogEx::OnOK();
  InputInfo* pInfo = new InputInfo;
  if(pInfo == NULL)
  {
    return;
  }

  ManageInputInfo(*pInfo);

  if(pInfo->nId == 0
     && pInfo->strName.IsEmpty()
     && pInfo->nSex == 0
     && pInfo->strPhone.IsEmpty()
     && pInfo->strHobby.IsEmpty()
     )
  {
    m_StaticTextInfo.SetWindowTextW(L"输入有误！");
    MessageBox(TEXT("请输入查询条件，ID 不能为 0！"));

    delete pInfo;
    return;
  }

  m_pInfo = pInfo;
  m_nCurOpt = emOpt::find;

  AfxBeginThread(OptThreadProc , (LPVOID)this);

  DisabledAllBtn();
  StartProgress();
}


void CClientLayerDlg::OnBnClickedButtonLookall()
{
  // TODO:  在此添加控件通知处理程序代码
  m_nCurOpt = emOpt::findall;
  AfxBeginThread(OptThreadProc , (LPVOID)this);
}


void CClientLayerDlg::OnBnClickedButtonUpdate()
{
  // TODO:  在此添加控件通知处理程序代码
  InputInfo* pInfo = new InputInfo;
  if(pInfo == NULL)
  {
    return;
  }

  ManageInputInfo(*pInfo);

  do{
    if(pInfo->nId == 0)
    {
      MessageBox(TEXT("请输入ID，ID 不能为 0！"));
    }
    else if(pInfo->strName.IsEmpty())
    {
      MessageBox(TEXT("请输入姓名！"));
    }
    else if(pInfo->nSex == 0)
    {
      MessageBox(TEXT("请选择性别！"));
    }
    else if(pInfo->strPhone.IsEmpty())
    {
      MessageBox(TEXT("请输入电话！"));
    }
    else if(pInfo->strHobby.IsEmpty())
    {
      MessageBox(TEXT("请输入爱好！"));
    }
    else
    {
      break;
    }
    
    m_StaticTextInfo.SetWindowTextW(L"输入有误！");
    delete pInfo;
    return ;
  } while(0);

  m_pInfo = pInfo;
  m_nCurOpt = emOpt::update;

  AfxBeginThread(OptThreadProc , (LPVOID)this);

  DisabledAllBtn();
  StartProgress();
}


void CClientLayerDlg::OnBnClickedButtonRefresh()
{
  // TODO:  在此添加控件通知处理程序代码
  m_nCurOpt = emOpt::refresh;
  AfxBeginThread(OptThreadProc , (LPVOID)this);
}


void CClientLayerDlg::OnBnClickedButtonDel()
{
  // TODO:  在此添加控件通知处理程序代码
  InputInfo* pInfo = new InputInfo;
  if(pInfo == NULL)
  {
    return;
  }

  ManageInputInfo(*pInfo);

  if(pInfo->nId == 0)
  {
    m_StaticTextInfo.SetWindowTextW(L"输入有误！");
    MessageBox(TEXT("ID 不能为 0！"));
    delete pInfo;
    return ;
  }

  m_pInfo = pInfo;
  m_nCurOpt = emOpt::del;

  AfxBeginThread(OptThreadProc , (LPVOID)this);

  DisabledAllBtn();
  StartProgress();
}


UINT AFX_CDECL CClientLayerDlg::OptThreadProc(LPVOID lpParameter)
{
  CClientLayerDlg* obj = (CClientLayerDlg*)lpParameter;
  TCHAR* pszBuff = NULL;

  int nRet = FALSE;

  switch (obj->m_nCurOpt)
  {
    case emOpt::insert:
      nRet = OptInsert(obj);
      break;
    case emOpt::del:
      nRet = OptDel(obj);
      break;
    case emOpt::update:
      nRet = OptUpdate(obj);
      break;
    case emOpt::find:
      nRet = OptFind(obj);
      break;
    case emOpt::findall:
      nRet = OptFindall(obj);
      break;
    case emOpt::refresh:
      break;
    default:
      break;
  }

  if(nRet == 2)
  {
    pszBuff = TEXT("远程数据库未变化，从本地缓存中输出！");
  }
  else if(nRet)
  {
    pszBuff = TEXT("操作完成！");
  }
  else
  {
    pszBuff = TEXT("操作失败！");
  }

  obj->m_nPreOpt = obj->m_nCurOpt;
  obj->m_nCurOpt = -1;
  if(obj->m_pInfo != NULL)
  {
    delete obj->m_pInfo;
    obj->m_pInfo = NULL;
  }

  obj->SendMessage(WM_MYMESSAGE , emMessage::optdone, (LPARAM)pszBuff );

  return 0;
}


BOOL CClientLayerDlg::OptInsert(CClientLayerDlg* obj)
{
  if(obj == NULL || obj->m_pInfo == NULL)
  {
    return FALSE;
  }

  InputInfo* pInfo = obj->m_pInfo;

  CString strSql;
  strSql.Format(TEXT("INSERT INTO tab_student (name, sex, phone, hobby) VALUES ('%s', %d, '%s', '%s')"),
                pInfo->strName,
                pInfo->nSex,
                pInfo->strPhone,
                pInfo->strHobby);
  
  SendPacket(obj->m_sServer ,
             emSocketMsg::sql_insert ,
             (char*)strSql.GetBuffer() ,
             ((strSql.GetLength() + 1) * sizeof(USHORT)));

  char* pSqlMess = RecivPacket(obj->m_sServer);
  
  if(pSqlMess == NULL)
  {
    return 0;
  }

  DWORD nLen = *(DWORD*)pSqlMess;
  BYTE nType = *(BYTE*)(pSqlMess + sizeof(DWORD));

  if(nType != emSocketMsg::sql_insert_succ)
  {
    return FALSE;
  }

  delete pSqlMess;

  return TRUE;
}


BOOL CClientLayerDlg::OptDel(CClientLayerDlg* obj)
{
  if(obj == NULL || obj->m_pInfo == NULL)
  {
    return FALSE;
  }

  InputInfo* pInfo = obj->m_pInfo;

  CString strSql;
  strSql.Format(TEXT("DELETE FROM tab_student WHERE id = %d"),
                pInfo->nId);
  
  SendPacket(obj->m_sServer ,
             emSocketMsg::sql_del ,
             (char*)strSql.GetBuffer() ,
             ((strSql.GetLength() + 1) * sizeof(USHORT)));

  char* pSqlMess = RecivPacket(obj->m_sServer);
  
  if(pSqlMess == NULL)
  {
    return 0;
  }

  DWORD nLen = *(DWORD*)pSqlMess;
  BYTE nType = *(BYTE*)(pSqlMess + sizeof(DWORD));

  if(nType != emSocketMsg::sql_del_succ)
  {
    return FALSE;
  }

  delete pSqlMess;

  return TRUE;
}


BOOL CClientLayerDlg::OptUpdate(CClientLayerDlg* obj)
{
  if(obj == NULL || obj->m_pInfo == NULL)
  {
    return FALSE;
  }

  InputInfo* pInfo = obj->m_pInfo;

  CString strSql;
  strSql.Format(TEXT("UPDATE tab_student SET name='%s', sex=%d, phone='%s', hobby='%s' WHERE id = %d"),
                pInfo->strName,
                pInfo->nSex,
                pInfo->strPhone,
                pInfo->strHobby,
                pInfo->nId);
  
  SendPacket(obj->m_sServer ,
             emSocketMsg::sql_update ,
             (char*)strSql.GetBuffer() ,
             ((strSql.GetLength() + 1) * sizeof(USHORT)));

  char* pSqlMess = RecivPacket(obj->m_sServer);
  
  if(pSqlMess == NULL)
  {
    return 0;
  }

  DWORD nLen = *(DWORD*)pSqlMess;
  BYTE nType = *(BYTE*)(pSqlMess + sizeof(DWORD));

  if(nType != emSocketMsg::sql_update_succ)
  {
    return FALSE;
  }

  delete pSqlMess;

  return TRUE;
}


BOOL CClientLayerDlg::OptFindall(CClientLayerDlg* obj)
{
  if(obj == NULL)
  {
    return FALSE;
  }

  if(obj->m_nPreOpt == obj->m_nCurOpt)
  {

    // return 2;
  }

  InputInfo* pInfo = obj->m_pInfo;

  CString strSql;
  strSql.Format(TEXT("SELECT ID, name, sex, phone, hobby FROM tab_student"));
  
  SendPacket(obj->m_sServer ,
             emSocketMsg::sql_findall ,
             (char*)strSql.GetBuffer() ,
             ((strSql.GetLength() + 1) * sizeof(USHORT)));

  char* pSqlMess = RecivPacket(obj->m_sServer);
  
  if(pSqlMess == NULL)
  {
    return 0;
  }

  DWORD nLen = *(DWORD*)pSqlMess;
  BYTE nType = *(BYTE*)(pSqlMess + sizeof(DWORD));

  if(nType != emSocketMsg::sql_findall_succ)
  {
    return FALSE;
  }

  obj->SendMessage(WM_MYMESSAGE , emMessage::recvinfo , (LPARAM)pSqlMess);
  // delete pSqlMess;

  return TRUE;
}


BOOL CClientLayerDlg::OptFind(CClientLayerDlg* obj)
{
  if(obj == NULL)
  {
    return FALSE;
  }

  InputInfo* pInfo = obj->m_pInfo;

  CString strID;
  strID.Format(TEXT("%d") , pInfo->nId);

  CString strSql;
  strSql.Format(TEXT("SELECT ID, name, sex, phone, hobby FROM tab_student WHERE \
                      ID LIKE '%%%s%%' AND\
                      name LIKE '%%%s%%' AND \
                      sex  LIKE '%%%s%%' AND \
                      phone LIKE '%%%s%%' AND \
                      hobby LIKE '%%%s%%'"),
                      pInfo->nId == 0 ? L"" : strID.GetBuffer(),
                      pInfo->strName.IsEmpty() ? L"" : pInfo->strName.GetBuffer(),
                      pInfo->nSex == 0 ? L"" : pInfo->nSex == 1 ? L"1" : L"2",
                      pInfo->strPhone.IsEmpty() ? L"" : pInfo->strPhone.GetBuffer(),
                      pInfo->strHobby.IsEmpty() ? L"" : pInfo->strHobby.GetBuffer()
                      );
  
  SendPacket(obj->m_sServer ,
             emSocketMsg::sql_find ,
             (char*)strSql.GetBuffer() ,
             ((strSql.GetLength() + 1) * sizeof(USHORT)));

  char* pSqlMess = RecivPacket(obj->m_sServer);
  
  if(pSqlMess == NULL)
  {
    return 0;
  }

  DWORD nLen = *(DWORD*)pSqlMess;
  BYTE nType = *(BYTE*)(pSqlMess + sizeof(DWORD));

  if(nType != emSocketMsg::sql_find_succ)
  {
    return FALSE;
  }

  obj->SendMessage(WM_MYMESSAGE , emMessage::recvinfo , (LPARAM)pSqlMess);
  // delete pSqlMess;

  return TRUE;
}



void CClientLayerDlg::DisposeRecv(char* pMess)
{
  m_ListMain.DeleteAllItems();

  DWORD nLen = *(DWORD*)pMess;
  UINT nItem = *(UINT*)(pMess + sizeof(DWORD) + sizeof(BYTE));

  char* pwMess = (pMess + sizeof(DWORD) + sizeof(BYTE) + sizeof(UINT));
  DWORD nRul = 0;

  UINT nValLen = 0;
  TCHAR szBuff[ MAXBYTE ] = { 0 };
  for(int i = 0; (int)nRul < (int)nLen - (int)sizeof(DWORD) - (int)sizeof(BYTE) - (int)sizeof(UINT); ++i)
  {
    memcpy_s(&nValLen , MAXBYTE , pwMess + nRul , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(szBuff , MAXBYTE , pwMess + nRul , nValLen * sizeof(USHORT));
    nRul += nValLen * sizeof(USHORT);
    m_ListMain.InsertItem(i , szBuff);

    memcpy_s(&nValLen , MAXBYTE , pwMess + nRul , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(szBuff , MAXBYTE , pwMess + nRul , nValLen * sizeof(USHORT));
    nRul += nValLen * sizeof(USHORT);
    m_ListMain.SetItemText(i , 1, szBuff);

    memcpy_s(&nValLen , MAXBYTE , pwMess + nRul , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(szBuff , MAXBYTE , pwMess + nRul , nValLen * sizeof(USHORT));
    nRul += nValLen * sizeof(USHORT);
    m_ListMain.SetItemData(i , *(UINT*)szBuff - '0');
    if(*(UINT*)szBuff - '0' == 1)
    {
      m_ListMain.SetItemText(i , 2, TEXT("男"));
    }
    else
    {
      m_ListMain.SetItemText(i , 2, TEXT("女"));
    }

    memcpy_s(&nValLen , MAXBYTE , pwMess + nRul , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(szBuff , MAXBYTE , pwMess + nRul , nValLen * sizeof(USHORT));
    nRul += nValLen * sizeof(USHORT);
    m_ListMain.SetItemText(i , 3, szBuff);

    memcpy_s(&nValLen , MAXBYTE , pwMess + nRul , sizeof(UINT));
    nRul += sizeof(UINT);
    memcpy_s(szBuff , MAXBYTE , pwMess + nRul , nValLen * sizeof(USHORT));
    nRul += nValLen * sizeof(USHORT);
    m_ListMain.SetItemText(i , 4, szBuff);

  }
}


//void CClientLayerDlg::OnLvnItemchangedListMain(NMHDR *pNMHDR , LRESULT *pResult)
//{
//  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//  // TODO:  在此添加控件通知处理程序代码
//  *pResult = 0;
//}


void CClientLayerDlg::OnNMClickListMain(NMHDR *pNMHDR , LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
  // TODO:  在此添加控件通知处理程序代码
  *pResult = 0;

  int nSel = m_ListMain.GetNextItem(-1 , LVNI_ALL | LVNI_SELECTED);
  if(nSel < 0)
  {
    return;
  }

  InputInfo* pInfo = new InputInfo;

  pInfo->nId = _wtoi(m_ListMain.GetItemText(nSel , 0));
  pInfo->nSex = m_ListMain.GetItemData(nSel);
  pInfo->strName = m_ListMain.GetItemText(nSel , 1);
  pInfo->strPhone = m_ListMain.GetItemText(nSel , 3);
  pInfo->strHobby = m_ListMain.GetItemText(nSel , 4);

  ManageInputInfo(*pInfo , FALSE);
  delete pInfo;
}
