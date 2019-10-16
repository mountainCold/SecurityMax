// CWindowDlg.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CWindowDlg.h"
#include "afxdialogex.h"
#include "CTool.h"


// CWindowDlg 对话框

IMPLEMENT_DYNAMIC(CWindowDlg, CDialogEx)

CWindowDlg::CWindowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WINDOW, pParent)
{

}

CWindowDlg::~CWindowDlg()
{
}

void CWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_WinTree);
}


BEGIN_MESSAGE_MAP(CWindowDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CWindowDlg::OnClickTree1)
	ON_WM_UPDATEUISTATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CWindowDlg::OnTvnSelchangedTree1)
END_MESSAGE_MAP()


// CWindowDlg 消息处理程序


void CWindowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_WinTree.m_hWnd)
	{
		m_WinTree.SetWindowPos(0, 0, 0, cx, cy, SWP_NOZORDER);
	}
}


BOOL CWindowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdataWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CWindowDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//UpdataWindow();
	CDialogEx::OnTimer(nIDEvent);
}
void CWindowDlg::UpdataWindow()
{
	m_WinTree.DeleteAllItems();
	std::vector<DWORD> vHwnd;
	EnumWindows(EnumWindowProcM, (LPARAM)& vHwnd);
	//EnumChildWindows(hFindWnd, EnumProc, NULL)
	for (int i = 0; i < vHwnd.size(); ++i)
	{
		HWND hWnd = (HWND)vHwnd[i];
		TCHAR buff[300];
		TCHAR buff1[200];
		TCHAR buff2[200];
		::GetClassName(hWnd, buff2, 200);
		::GetWindowText(hWnd, buff1, 200);
		_stprintf_s(buff, 300, L"%s %08x \"%s\" %s", L"窗口", (int)hWnd, buff1, buff2);
		HTREEITEM hItem = m_WinTree.InsertItem(buff);
		m_WinTree.SetItemData(hItem, (DWORD_PTR)hWnd);
	}
	vOldHwnd.swap(vHwnd);

}



void CWindowDlg::OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint pos;
	GetCursorPos(&pos);
	m_WinTree.ScreenToClient(&pos);
	HTREEITEM hItem = m_WinTree.HitTest(pos);
	HTREEITEM hChild = m_WinTree.GetNextItem(hItem, TVGN_CHILD);
	if (hChild != NULL) return;
	HWND hwnd2 = (HWND)m_WinTree.GetItemData(hItem);
	std::vector<DWORD> vHwnd;
	EnumChildWindows(hwnd2, EnumWindowProcM, (LPARAM)& vHwnd);
	for (int i = 0; i < vHwnd.size(); ++i)
	{
		HWND hWnd = (HWND)vHwnd[i];
		TCHAR buff[300];
		TCHAR buff1[200];
		::GetWindowText(hWnd, buff1, 200);
		_stprintf_s(buff, 300, L"%s %d %s", L"窗口", (int)hWnd, buff1);
		HTREEITEM hItem2 = m_WinTree.InsertItem(buff, hItem);
		m_WinTree.SetItemData(hItem2, (DWORD_PTR)hWnd);
	}
	vOldHwnd.swap(vHwnd);
}


void CWindowDlg::OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/)
{
	// 该功能要求使用 Windows 2000 或更高版本。
	// 符号 _WIN32_WINNT 和 WINVER 必须 >= 0x0500。
	// TODO: 在此处添加消息处理程序代码
	UpdataWindow();
}


void CWindowDlg::OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
