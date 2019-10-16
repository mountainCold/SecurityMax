// MyTab.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "MyTab.h"


// MyTab

IMPLEMENT_DYNAMIC(MyTab, CTabCtrl)

MyTab::MyTab()
{

}

MyTab::~MyTab()
{
}


BEGIN_MESSAGE_MAP(MyTab, CTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, &MyTab::OnTcnSelchange)
END_MESSAGE_MAP()



// MyTab 消息处理程序




void MyTab::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	SetPadding(58);
	InsertItem(0, L"基本信息");
	InsertItem(1, L"病毒查杀");
	InsertItem(2, L"垃圾清理");
	InsertItem(3, L"系统服务");
	InsertItem(4, L"文件解析");
	CTabCtrl::PreSubclassWindow();
}
void MyTab::InsertTabWnd(int count, ...)
{
	va_list VaList = nullptr;
	va_start(VaList, count);

	for (int i = 0; i < count; ++i)
	{
		// 获取到窗口对象指针和窗口的ID
		CDialogEx* Dialog = va_arg(VaList, CDialogEx*);
		int id = va_arg(VaList, int);

		// 创建窗口并添加到容器中
		Dialog->Create(id, this);
		m_WndVec.push_back(Dialog);
	}

	// 应该以选项卡为父窗口计算出新的位置并且移动所有的窗口
		// 以选项卡为准，重新设置窗口的位置
	CRect Rect = { };
	this->GetClientRect(&Rect);
	Rect.DeflateRect(0, 23, 0, 0);
	for (int i = 0; i < m_WndVec.size(); ++i)
		m_WndVec[i]->MoveWindow(&Rect);

	va_end(VaList);

	ShowTabWnd(0);
}
void MyTab::ShowTabWnd(int index)
{
	for (int i = 0; i < m_WndVec.size(); ++i)
	{
		m_WndVec[i]->ShowWindow(i == index ? SW_SHOWNORMAL : SW_HIDE);
		if (i == 2)
		{
			m_WndVec[i]->SendMessage(WM_UPDATEUISTATE);
		}
	}
}


void MyTab::OnTcnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	ShowTabWnd(this->GetCurSel());
	//*pResult = 0;
}
