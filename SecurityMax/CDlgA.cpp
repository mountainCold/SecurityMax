// CDlgA.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDlgA.h"
#include "afxdialogex.h"
#include "CProcessDlg.h"
#include "CWindowDlg.h"
#include "CModuleDlg.h"
#include "CThreadDlg.h"

// CDlgA 对话框

IMPLEMENT_DYNAMIC(CDlgA, CDialogEx)

CDlgA::CDlgA(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDlgA::~CDlgA()
{
}

void CDlgA::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB2, m_Tab);
}

void CDlgA::addTabWnd(const CString& title, CDialogEx* pSubWnd, UINT uId)
{
	m_Tab.SetPadding(89);
	m_Tab.InsertItem(m_Tab.GetItemCount(),
		title);
	// 创建子窗口,设置父窗口
	pSubWnd->Create(uId, &m_Tab);
	CRect rect;
	m_Tab.GetClientRect(rect);
	rect.DeflateRect(1, 23, 1, 1);
	pSubWnd->MoveWindow(rect);

	m_tabSubWnd.push_back(pSubWnd);
	m_Tab.SetCurSel(m_tabSubWnd.size() - 1);
}


BEGIN_MESSAGE_MAP(CDlgA, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CDlgA::OnSelchangeTab2)
END_MESSAGE_MAP()


// CDlgA 消息处理程序


BOOL CDlgA::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	addTabWnd(L"进程",
		new CProcessDlg,
		IDD_PROCESS);
	addTabWnd(L"线程",
		new CThreadDlg,
		IDD_THREAD);
	addTabWnd(L"模块",
		new CModuleDlg,
		IDD_MODULE);
	addTabWnd(L"窗口",
		new CWindowDlg,
		IDD_WINDOW);
	m_Tab.SetCurSel(0);
	m_tabSubWnd[m_Tab.GetCurSel()]->ShowWindow(SW_SHOW);
	m_tabSubWnd[m_Tab.GetCurSel()]->UpdateWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgA::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (m_Tab.m_hWnd)
	{
		CRect rect = { 0,0,cx,cy };
		m_Tab.MoveWindow(rect);
		rect.DeflateRect(1, 30, 1, 1);
		for (auto& i : m_tabSubWnd) {
			i->MoveWindow(rect);
		}
	}
}


void CDlgA::OnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//*pResult = 0;
	for (auto& i : m_tabSubWnd) {
		i->ShowWindow(SW_HIDE);
	}
	m_tabSubWnd[m_Tab.GetCurSel()]->ShowWindow(SW_SHOW);
	m_tabSubWnd[m_Tab.GetCurSel()]->UpdateWindow();
}

