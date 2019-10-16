// CDlgD.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDlgD.h"
#include "afxdialogex.h"


// CDlgD 对话框

IMPLEMENT_DYNAMIC(CDlgD, CDialogEx)

CDlgD::CDlgD(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG4, pParent)
{

}

CDlgD::~CDlgD()
{
}

void CDlgD::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

void CDlgD::setListTextS()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"名称", 0, 160);
	m_list.InsertColumn(1, L"路径信息", 0, 340);
	m_list.InsertColumn(2, L"状态", 0, 160);
	m_list.InsertColumn(3, L"启动类型", 0, 160);
	GetHANDLESCM();
}
char* GetServerType(DWORD statue)
{
	switch (statue)
	{
	case SERVICE_PAUSE_PENDING:
		return "正在暂停";
	case SERVICE_PAUSED:
		return "已挂起";
	case SERVICE_RUNNING:
		return "正在运行";
	case SERVICE_START_PENDING:
		return "正在启动";
	case SERVICE_STOP_PENDING:
		return "正在停止";
	case SERVICE_STOPPED:
		return "已停止";
	case SERVICE_CONTINUE_PENDING:
		return "将继续";
	default:
		return "";
	}
}
char* GetServerType2(DWORD statue)
{
	switch (statue)
	{
	case SERVICE_AUTO_START:
		return "自动启动";
	case SERVICE_BOOT_START:
		return "加载启动";
	case SERVICE_DEMAND_START:
		return "调用启动";
	case SERVICE_DISABLED:
		return "无法启动";
	case SERVICE_SYSTEM_START:
		return "手动启动";
	default:
		return "";
	}
}
void CDlgD::GetHANDLESCM()
{
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	DWORD dwServiceNum = 0;
	DWORD dwSize = 0;
	CString buffer;
	EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &dwSize, &dwServiceNum, NULL, NULL);
	LPENUM_SERVICE_STATUS_PROCESS pEnumSerice = (LPENUM_SERVICE_STATUS_PROCESS)new char[dwSize];
	bool bStatus = FALSE;
	bStatus = EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32, SERVICE_STATE_ALL, (PBYTE)pEnumSerice, dwSize, &dwSize, &dwServiceNum, NULL, NULL);
	for (DWORD i = 0; i < dwServiceNum; i++)
	{
		m_list.InsertItem(i, L"");
		buffer.Format(L"%s",pEnumSerice[i].lpServiceName);
		m_list.SetItemText(i, 0, buffer);
		char* str = GetServerType(pEnumSerice[i].ServiceStatusProcess.dwCurrentState);
		buffer.Format(L"%s",CATOCS(str));
		m_list.SetItemText(i, 2, buffer);
		SC_HANDLE hService = OpenService(hSCM, pEnumSerice[i].lpServiceName,
			SERVICE_QUERY_CONFIG);
		QueryServiceConfig(hService, NULL, 0, &dwSize);
		LPQUERY_SERVICE_CONFIG pServiceConfig = (LPQUERY_SERVICE_CONFIG)new char[dwSize];
		QueryServiceConfig(hService, pServiceConfig, dwSize, &dwSize);
		char* stra = GetServerType2(pServiceConfig->dwStartType);
		buffer.Format(L"%s", CATOCS(stra));
		m_list.SetItemText(i, 3, buffer);
		buffer.Format(L"%s", pServiceConfig->lpBinaryPathName);
		m_list.SetItemText(i, 1, buffer);
	
	}
}


BEGIN_MESSAGE_MAP(CDlgD, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CDlgD::OnRclickList1)
	ON_COMMAND(ID_32780, &CDlgD::On32780)
	ON_COMMAND(ID_32781, &CDlgD::On32781)
END_MESSAGE_MAP()


// CDlgD 消息处理程序


BOOL CDlgD::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_menu.LoadMenu(IDR_MENU3);
	setListTextS();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgD::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_list.m_hWnd) {
		m_list.SetWindowPos(0, 0, 0, cx, cy, SWP_NOZORDER);
	}
}


void CDlgD::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CMenu* pSubMenu = m_menu.GetSubMenu(0);
	CPoint pos;
	GetCursorPos(&pos);
	pSubMenu->TrackPopupMenu(0, pos.x, pos.y, this);
}

//启动
void CDlgD::On32780()
{
	int sel = (int)m_list.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString sPid = m_list.GetItemText(sel, 0);
	SC_HANDLE hSvc = ::OpenService(hSCM, sPid,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if (hSvc == NULL)
	{
		return;
	}
	// 获得服务的状态
	SERVICE_STATUS status;
	if (::QueryServiceStatus(hSvc, &status) == FALSE)
	{
		::CloseServiceHandle(hSvc);
		return;
	}
	if (status.dwCurrentState == SERVICE_STOPPED)
	{
		// 启动服务
		if (::StartService(hSvc, NULL, NULL) == FALSE)
		{
			MessageBox(L"start service error。");
			::CloseServiceHandle(hSvc);
			return;
		}

		// 等待服务启动
		while (::QueryServiceStatus(hSvc, &status) == TRUE)
		{
			::Sleep(status.dwWaitHint);
			if (status.dwCurrentState == SERVICE_RUNNING)
			{
				MessageBox(L"start success。");
				::CloseServiceHandle(hSvc);
				GetHANDLESCM();
				return;
			}
		}
	}
}


//关闭
void CDlgD::On32781()
{
	int sel = (int)m_list.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString sPid = m_list.GetItemText(sel, 0);
	SC_HANDLE hSvc = ::OpenService(hSCM, sPid,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
	if (hSvc == NULL)
	{
		return;
	}
	// 获得服务的状态
	SERVICE_STATUS status;
	if (::QueryServiceStatus(hSvc, &status) == FALSE)
	{
		::CloseServiceHandle(hSvc);
		return;
	}
	if (status.dwCurrentState == SERVICE_RUNNING)
	{
		if (!ControlService(
			hSvc,
			SERVICE_CONTROL_STOP,
			(LPSERVICE_STATUS)&status))
		{
			MessageBox(L"ControlService failed");
			::CloseServiceHandle(hSvc);
			return;
		}
		while (::QueryServiceStatus(hSvc, &status) == TRUE)
		{
			::Sleep(status.dwWaitHint);
			if (status.dwCurrentState == SERVICE_STOPPED)
			{
				MessageBox(L"stop success。");
				::CloseServiceHandle(hSvc);
				GetHANDLESCM();
				return;
			}
		}
	}	
}
