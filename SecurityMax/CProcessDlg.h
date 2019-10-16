#pragma once

#include <vector>
#include "CTool.h"
// CProcessDlg 对话框

class CProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessDlg)

public:
	CProcessDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProcessDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CMenu m_menu;
	DWORD m_pid;
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	void updateProcessList();
	std::vector<PROCESSINFO> m_procList;
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32772();
	afx_msg void On32771();
	afx_msg void On32773();
};
