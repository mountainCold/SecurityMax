// CModuleDlg.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CModuleDlg.h"
#include "afxdialogex.h"
#include <vector>
#include "CTool.h"


// CModuleDlg 对话框

IMPLEMENT_DYNAMIC(CModuleDlg, CDialogEx)

CModuleDlg::CModuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODULE, pParent)
{

}

CModuleDlg::~CModuleDlg()
{
}

void CModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTMOD, m_ModulList);
}


BEGIN_MESSAGE_MAP(CModuleDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CModuleDlg 消息处理程序


BOOL CModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ModulList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ModulList.InsertColumn(0, L"模块名", 0, 160);
	m_ModulList.InsertColumn(1, L"进程PID", 0, 160);
	m_ModulList.InsertColumn(2, L"加载基址", 0, 120);
	m_ModulList.InsertColumn(3, L"大小", 0, 120);
	m_ModulList.InsertColumn(4, L"路径", 0, 480);
	std::vector<MYMODULEENTRY32> moduleList;
	EnumModul(&moduleList);
	// 插入到列表控件
	int index = 0;
	CString buffer;
	for (auto& i : moduleList) {
		m_ModulList.InsertItem(index, L"");
		buffer.Format(L"%08X", i.modul.szModule);
		m_ModulList.SetItemText(index, 0,buffer);
		buffer.Format(L"%05d", i.dwPid);
		m_ModulList.SetItemText(index,1, buffer);

		buffer.Format(L"%08X", i.modul.modBaseAddr);
		m_ModulList.SetItemText(index, 2, buffer);

		buffer.Format(L"%08X", i.modul.modBaseSize);
		m_ModulList.SetItemText(index, 3, buffer);

		m_ModulList.SetItemText(index, 4, i.modul.szExePath);
		++index;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CModuleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_ModulList.m_hWnd) {
		m_ModulList.SetWindowPos(0, 0, 0, cx, cy, SWP_NOZORDER);
	}
}
