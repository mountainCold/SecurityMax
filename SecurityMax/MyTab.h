#pragma once
#include "Resource.h"
#include <vector>
using namespace std;
// MyTab

class MyTab : public CTabCtrl
{
	DECLARE_DYNAMIC(MyTab)

public:
	MyTab();
	virtual ~MyTab();
	// 提供接口用于创建对话框(组: 窗口指针+ID)
	void InsertTabWnd(int count, ...);

	// 显示窗口
	void ShowTabWnd(int index);
	// 用于保存所有的窗口指针
	vector<CDialogEx*> m_WndVec;
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg void OnTcnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
};


