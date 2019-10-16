#pragma once


// CDlgD 对话框
#include <Winsvc.h>
#include "resource.h"
#include "CTool.h"
class CDlgD : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgD)

public:
	CDlgD(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgD();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	SC_HANDLE hSCM;
	BOOL __stdcall StopDependentServices(SC_HANDLE schService);
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	CMenu m_menu;
	void setListTextS();
	void GetHANDLESCM();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32780();
	afx_msg void On32781();
};
