
// SecurityMaxDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SecurityMax.h"
#include "SecurityMaxDlg.h"
#include "afxdialogex.h"

#include "CDlgA.h"
#include "CDlgB.h"
#include "CDlgC.h"
#include "CDlgD.h"
#include "CDlgE.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CTool.h"
#include <Psapi.h>


// CSecurityMaxDlg 对话框
CRITICAL_SECTION g_critical;
CSecurityMaxDlg::CSecurityMaxDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SECURITYMAX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSecurityMaxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}

DWORD WINAPI ProcTh(LPVOID lpvoid)
{
	CP* cp = (CP*)lpvoid;
	DWORD dwPIDList[1000] = { 0 };
	DWORD bufSize = sizeof(dwPIDList);
	DWORD dwNeedSize = 0;
	MEMORYSTATUSEX stcMemStatusEx = { 0 };
	while (true)
	{
		EnterCriticalSection(&g_critical);
		cp->cpUse = getCpuUse();
		stcMemStatusEx.dwLength = sizeof(stcMemStatusEx);
		GlobalMemoryStatusEx(&stcMemStatusEx);
		cp->chUse= stcMemStatusEx.dwMemoryLoad;
		EnumProcesses(dwPIDList, bufSize, &dwNeedSize);
		cp->Size = dwNeedSize / sizeof(DWORD);
		LeaveCriticalSection(&g_critical);
	}

	return true;
}
BEGIN_MESSAGE_MAP(CSecurityMaxDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_COMMAND(ID_32774, &CSecurityMaxDlg::On32774)
	ON_COMMAND(ID_32775, &CSecurityMaxDlg::On32775)
	ON_COMMAND(ID_32776, &CSecurityMaxDlg::On32776)
	ON_COMMAND(ID_32777, &CSecurityMaxDlg::On32777)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSecurityMaxDlg::OnTcnSelchangeTab1)
	ON_COMMAND(ID_32779, &CSecurityMaxDlg::On32779)
	ON_COMMAND(ID_32778, &CSecurityMaxDlg::On32778)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSecurityMaxDlg 消息处理程序


BOOL CSecurityMaxDlg::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	InitStatusBar();
	InitializeCriticalSection(&g_critical);
	SetTimer(100, 1000, NULL);
	cp = { 0 };
	CreateThread(NULL, 0, ProcTh, (LPVOID)&cp, 0, NULL);
	::RegisterHotKey(m_hWnd, 0x1244, MOD_CONTROL | MOD_SHIFT, 'X');//ctrl+shift+L
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	// TODO: 在此添加额外的初始化代码
	m_Tab.InsertTabWnd(5,
		new CDlgA, IDD_DIALOG1,
		new CDlgB, IDD_DIALOG2,
		new CDlgC, IDD_DIALOG3,
		new CDlgD, IDD_DIALOG4,
		new CDlgE, IDD_DIALOG5);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSecurityMaxDlg::OnPaint()
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
HCURSOR CSecurityMaxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CSecurityMaxDlg::InitStatusBar()
{
	m_bar.Create(this);//创建状态栏
	UINT indicators[] = {
	IDS_STRING167,IDS_STRING168,
	IDS_STRING169,
	IDS_STRING170,
	IDS_STRING171,
	IDS_STRING172,
	IDS_STRING173,
	IDS_STRING174,
	};
	m_bar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	CRect rect;
	GetClientRect(&rect);
	m_bar.MoveWindow(0, rect.bottom - 20, rect.right, 20);
	CString buffer;
	//设置各栏长度
	m_bar.SetPaneInfo(0, IDS_STRING167, SBPS_STRETCH, 60);
	m_bar.SetPaneInfo(1, IDS_STRING168, SBPS_STRETCH, 100);
	m_bar.SetPaneInfo(2, IDS_STRING169, SBPS_STRETCH, 80);
	m_bar.SetPaneInfo(3, IDS_STRING170, SBPS_STRETCH, 100);
	m_bar.SetPaneInfo(4, IDS_STRING171, SBPS_STRETCH, 80);
	m_bar.SetPaneInfo(5, IDS_STRING172, SBPS_STRETCH, 100);
	m_bar.SetPaneInfo(6, IDS_STRING173, SBPS_STRETCH, 80);
	m_bar.SetPaneInfo(7, IDS_STRING174, SBPS_STRETCH, 150);
	m_bar.SetPaneText(0, L"进程数：");

	m_bar.SetPaneText(2, L"CPU利用率");
	buffer.Format(L"%d", getCpuUse());
	m_bar.SetPaneText(3, buffer);
	m_bar.SetPaneText(4, L"内存占用率");
	buffer.Format(L"%d", 00);
	m_bar.SetPaneText(5, buffer);
	m_bar.SetPaneText(6, L"时间");
	m_bar.SetPaneText(7, L"time");
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_REC);//自动填充用户区域的窗口

	m_bar.GetStatusBarCtrl().SetBkColor(RGB(180, 180, 180));//设置状态栏颜色
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	return TRUE;
}

void CSecurityMaxDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_Tab.m_hWnd)
	{
		CRect rect = { 0,0,cx,cy };
		m_Tab.MoveWindow(rect);
		for (auto& i : m_Tab.m_WndVec)
		{
			rect.DeflateRect(1, 25, 1, 1);
			i->MoveWindow(rect);
		}
	}
}

//关机
void CSecurityMaxDlg::On32774()
{
	//EnableDebugPrivilege();
	//InitiateSystemShutdownEx(NULL,NULL,5,TRUE,FALSE,
		//SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);

}

//重启
void CSecurityMaxDlg::On32775()
{
	//EnableDebugPrivilege();
	//ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
	//	SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
	//InitiateSystemShutdownEx(NULL, NULL, 0, TRUE, TRUE,
	//	SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
}

//睡眠
void CSecurityMaxDlg::On32776()
{
	//SetSuspendState(FALSE, FALSE, FALSE);
}

//锁屏
void CSecurityMaxDlg::On32777()
{
	LockWorkStation();
}
//内存优化
void CSecurityMaxDlg::On32778()
{
	MEMORYSTATUSEX stcMemStatusEx = { 0 };
	stcMemStatusEx.dwLength = sizeof(stcMemStatusEx);
	GlobalMemoryStatusEx(&stcMemStatusEx);
	DWORDLONG preUsedMem = stcMemStatusEx.ullTotalPhys - stcMemStatusEx.ullAvailPhys;
	DWORD dwPIDList[1000] = { 0 };
	DWORD bufSize = sizeof(dwPIDList);
	DWORD dwNeedSize = 0;
	EnumProcesses(dwPIDList, bufSize, &dwNeedSize);
	for (DWORD i = 0; i < dwNeedSize / sizeof(DWORD); i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_SET_QUOTA, false, dwPIDList[i]);
		SetProcessWorkingSetSize(hProcess, -1, -1);

	}
	//获取清理后的内存状态
	GlobalMemoryStatusEx(&stcMemStatusEx);
	DWORDLONG afterCleanUsedMem = stcMemStatusEx.ullTotalPhys - stcMemStatusEx.ullAvailPhys;
}

//版本信息
void CSecurityMaxDlg::On32779()
{
	OSVERSIONINFOEX os = { sizeof(OSVERSIONINFOEX) };
	//GetVersionEx((OSVERSIONINFO*)& os);
	switch (os.dwMajorVersion)
	{
	case 10:
	{
		if (VER_NT_WORKSTATION == os.wProductType)
		{
			//win10版本
			MessageBox(L"win10版本");
		}
		else
		{
			//Windows Server 2016
			MessageBox(L"Windows Server 2016");
		}
	}
	default:
		break;
	}
}
BOOL CSecurityMaxDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_HOTKEY) && (pMsg->wParam == 0x1244))
	{
		if (IsWindowVisible() == TRUE)
		{
			ShowWindow(SW_HIDE);
		}
		else
		{
			ShowWindow(SW_SHOW);
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
void CSecurityMaxDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}






void CSecurityMaxDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString buffer;
	if (nIDEvent == 100)
	{
		CTime t1;
		t1 = CTime::GetCurrentTime();
		//    m_bar.SetPaneText(1,t1.Format("%Y-%M-%F:%H:%M:%S"));
		m_bar.SetPaneText(7, t1.Format("  %Y-%m-%d  %H:%M:%S"));
		buffer.Format(L"%d", cp.chUse);
		m_bar.SetPaneText(5, buffer);
		buffer.Format(L"%d", cp.Size);
		m_bar.SetPaneText(1, buffer);
		buffer.Format(L"%d", cp.cpUse);
		m_bar.SetPaneText(3, buffer);
	}
	CDialogEx::OnTimer(nIDEvent);
}
