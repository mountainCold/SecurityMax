#pragma once


// CDlgE 对话框
#include "CTool.h"
class CDlgE : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgE)

public:
	CDlgE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void setEditTextE(PEINFO* peHead);
	DECLARE_MESSAGE_MAP()
public:
	PEINFO peHead;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnClickedButton1();
	afx_msg void OnClickedButton2();
	afx_msg void OnClickedButton3();
	afx_msg void OnClickedButton4();
	afx_msg void OnClickedButton5();
	afx_msg void OnClickedButton6();
	afx_msg void OnClickedButton7();
	afx_msg void OnClickedButton8();
	afx_msg void OnClickedButton9();
	CString m_path;
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
