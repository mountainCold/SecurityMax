// CProcessDlg.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CProcessDlg.h"
#include "afxdialogex.h"
#include "CModuleDlg.h"


// CProcessDlg 对话框
CRITICAL_SECTION g_critical_section;
IMPLEMENT_DYNAMIC(CProcessDlg, CDialogEx)

CProcessDlg::CProcessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROCESS, pParent)
{
	InitializeCriticalSection(&g_critical_section);
}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CProcessDlg, CDialogEx)
	
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CProcessDlg::OnRclickList1)
	ON_COMMAND(ID_32772, &CProcessDlg::On32772)
	ON_COMMAND(ID_32771, &CProcessDlg::On32771)
	ON_COMMAND(ID_32773, &CProcessDlg::On32773)
	ON_COMMAND(ID_32786, &CProcessDlg::On32786)
END_MESSAGE_MAP()

// CProcessDlg 消息处理程序


int IndexOfProcessList(std::vector<PROCESSINFO>& proclist, DWORD pid)
{
	DWORD size = proclist.size();
	for (int i = 0; i < size; ++i) {
		if (proclist[i].th32ProcessID == pid) {
			return i;
		}
	}
	return -1;
}

void CProcessDlg::updateProcessList()
{

	// 获取最新的进程列表
	std::vector<PROCESSINFO> newProcList;
	if (!EnumProcess(&newProcList))
	{
		return;
	}
	// 删除和增加属于原子操作, 所以需要进入临界区
	EnterCriticalSection(&g_critical_section);

	// 1. 找到已退出进程的名字,然后在列表控件中删除掉.
	// 1.1 使用旧列表的元素到新的列表中查找,没有找到,说明进程已退出
	int index = 0;
	for (auto itr = m_procList.begin();
		itr != m_procList.end();
		)
	{
		// 判断旧列表的元素是否存在于新列表中.
		if (-1 == IndexOfProcessList(newProcList, itr->th32ProcessID)) {
			// 不存在, 说明进程已退出, 将已退出的进程在
			//  数组和列表控件中删除,
			itr = m_procList.erase(itr);
			m_list.DeleteItem(index);
			continue;
		}
		// 刷新内存使用率
		CString buffer;
		DWORD memUsage = getProcessMemoryUsage(itr->th32ProcessID);
		if (itr->dwMemoryUsage != memUsage)
		{
			buffer.Format(_T("%12dKb"), memUsage);
			m_list.SetItemText(index, 3, buffer);
			itr->dwMemoryUsage = memUsage;
		}

		// 刷新CPU使用率
		double cpuUsage = getProcessCpuUsage(itr->th32ProcessID);

		if (abs(itr->dCpuUsage - cpuUsage) >= 0.001)
		{
			buffer.Format(_T("%.1lf%%"), cpuUsage);
			m_list.SetItemText(index, 2, buffer);
			itr->dCpuUsage = cpuUsage;
		}

		// 下一个
		++index;
		++itr;
	}

	// 2. 找到新创建的进程, 插入到列表控件中.
	// 2.1 使用新列表的元素到旧列表中查找, 没有找到,说明进程是新建的.		
	index = m_list.GetItemCount();
	for (auto& i : newProcList) {
		if (-1 == IndexOfProcessList(m_procList, i.th32ProcessID)) {
			// 插入到数组中
			m_procList.push_back(i);
			// 插入到列表控件中
			CString buffer;
			m_list.InsertItem(index, _T(""));
			m_list.UpdateWindow();
			buffer.Format(_T("%d"), i.th32ProcessID);
			m_list.SetItemText(index, 0, i.szExeFile);
			m_list.SetItemText(index, 1, buffer);
			m_list.SetItemText(index, 2, L"");
			buffer.Format(_T("%12dKb"), (double)i.dwMemoryUsage);
			m_list.SetItemText(index, 3, buffer);
			++index;
		}
	}

	// 离开临界区
	LeaveCriticalSection(&g_critical_section);
}


void CProcessDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_list.m_hWnd) {
		m_list.SetWindowPos(0, 0, 0, cx, cy, SWP_NOZORDER);
	}
}


BOOL CProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_menu.LoadMenu(IDR_MENU1);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_list.InsertColumn(0, L"进程名", 0, 170);
	m_list.InsertColumn(1, L"PID", 0, 210);
	m_list.InsertColumn(2, L"CPU", 0, 220);
	m_list.InsertColumn(3, L"内存使用率", 0, 240);
	updateProcessList();
	SetTimer(0,/*定时器的ID,功能类似于控件ID*/
		1000,/*间隔时间*/
		NULL/*回调函数, 如果为空,那么本窗口将接收到WM_TIMER的消息*/);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CProcessDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	updateProcessList();
	CDialogEx::OnTimer(nIDEvent);
}


void CProcessDlg::OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CMenu* pSubMenu = m_menu.GetSubMenu(0);
	CPoint pos;
	GetCursorPos(&pos);
	pSubMenu->TrackPopupMenu(0, pos.x, pos.y, this);
}

//查看模块
void CProcessDlg::On32773()
{
	int sel = (int)m_list.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString sPid = m_list.GetItemText(sel, 1);
	CString stitle = m_list.GetItemText(sel, 0);
	DWORD m_pid = _wtoi(sPid);


}
DWORD WINAPI GetMainThreadID(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	DWORD dwThreadID = 0;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	te32.dwSize = sizeof(THREADENTRY32);
	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);
	}
	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			dwThreadID = te32.th32ThreadID;
			break;
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return(dwThreadID);
}
//查看线程
void CProcessDlg::On32772()
{
	int sel = (int)m_list.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString sPid = m_list.GetItemText(sel, 1);
	CString stitle = m_list.GetItemText(sel, 0);
	DWORD m_pid = _wtoi(sPid);
	GetMainThreadID(m_pid);

}

//结束进程
void CProcessDlg::On32771()
{
	int sel = (int)m_list.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString sPid = m_list.GetItemText(sel, 1);
	m_pid = _wtoi(sPid);
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, m_pid);
	if (hProcess == 0)
	{
		CString error;
		error.Format(L"获取进程句柄失败：%d", GetLastError());
		MessageBox(error);
	}
	else
	{
		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
	}
	updateProcessList();
}

//保护进程
void CProcessDlg::On32786()
{
	int sel = (int)m_list.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString sPid = m_list.GetItemText(sel, 1);
	m_pid = _wtoi(sPid);
	//3.打开进程，获取进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_pid);
	WCHAR szDllPath[] = L"E:\\15pb\\阶段二\\SecurityMax\\Debug\\HOOkPRoc.dll";
	//4.在目标进程中申请空间
	LPVOID pBuff = VirtualAllocEx(
		hProcess,
		0,
		sizeof(szDllPath),
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
	);

	//5.将路径写入到目标进程中
	DWORD dwSize;
	WriteProcessMemory(
		hProcess,
		pBuff,			//在指申请的地址上
		szDllPath,		//写入的内容
		sizeof(szDllPath),//写入大小
		&dwSize
	);

	//6.使用关键函数加载目标dll
	// 利用远程创建线程函数，实现目标进程加载dll
	// 远程线程执行函数直接指向LoadLibaray函数，同时参数指向dll路径，完美实现加载dll
	//EnableDebugPrivilege();
	HANDLE hThread = CreateRemoteThread(
		hProcess,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)LoadLibrary,		//线程执行地址指向LoadLibrary
		pBuff,										//线程的附加参数dll路径
		NULL, NULL
	);
	//7 释放句柄
	if (hThread==NULL)
	{
		MessageBox(L"操作失败");
	}
	else
	{
		CloseHandle(hProcess);
		CloseHandle(hThread);
		MessageBox(L"添加成功");
	}

}
