// CThreadDlg.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CThreadDlg.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
#include <vector>
#include "CTool.h"


// CThreadDlg 对话框

IMPLEMENT_DYNAMIC(CThreadDlg, CDialogEx)

CThreadDlg::CThreadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THREAD, pParent)
{

}

CThreadDlg::~CThreadDlg()
{
}

void CThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTTHR, ThreadList);
}


BEGIN_MESSAGE_MAP(CThreadDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CThreadDlg 消息处理程序


BOOL CThreadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ThreadList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	ThreadList.InsertColumn(0, L"线程ID", 0, 215);
	ThreadList.InsertColumn(1, L"进程PID", 0, 215);
	ThreadList.InsertColumn(2, L"线程大小", 0, 215);
	ThreadList.InsertColumn(3, L"线程优先级", 0, 215);
	std::vector<THREADENTRY32> threadList;
	EnumThread( &threadList);
	// 插入到列表控件
	int index = 0;
	CString buffer;
	for (auto& i : threadList) {
		ThreadList.InsertItem(index, _T(""));
		buffer.Format(L"%08X", i.th32ThreadID);
		ThreadList.SetItemText(index, 0, buffer);

		buffer.Format(L"%05d", i.th32OwnerProcessID);
		ThreadList.SetItemText(index, 1, buffer);

		buffer.Format(L"%08X", i.dwSize);
		ThreadList.SetItemText(index, 2, buffer);

		buffer.Format(L"%08X", i.tpBasePri);
		ThreadList.SetItemText(index, 3, buffer);
		++index;
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CThreadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (ThreadList.m_hWnd) {
		ThreadList.SetWindowPos(0, 0, 0, cx, cy, SWP_NOZORDER);
	}
}
