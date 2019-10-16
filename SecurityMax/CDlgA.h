#pragma once

#include "resource.h"
#include <vector>
// CDlgA 对话框

class CDlgA : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgA)

public:
	CDlgA(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgA();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void addTabWnd(const CString& title, CDialogEx* pSubWnd, UINT uId);
	std::vector<CDialogEx*> m_tabSubWnd;
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_Tab;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult);
};
