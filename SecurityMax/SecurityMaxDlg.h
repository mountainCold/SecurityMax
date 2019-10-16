
// SecurityMaxDlg.h: 头文件
//

#pragma once
#include "MyTab.h"
#include "resource.h"

// CSecurityMaxDlg 对话框
struct CP
{
	WORD cpUse;
	WORD chUse;
	DWORD Size;
};
class CSecurityMaxDlg : public CDialogEx
{
// 构造
public:
	CSecurityMaxDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECURITYMAX_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	MyTab m_Tab;
	CStatusBar  m_bar;
	//进程大小 DWORD
	CP cp;
	BOOL InitStatusBar();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void On32774();
	afx_msg void On32775();
	afx_msg void On32776();
	afx_msg void On32777();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32779();
	afx_msg void On32778();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
