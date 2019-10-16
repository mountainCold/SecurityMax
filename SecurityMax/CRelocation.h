#pragma once
#include "CTool.h"


// CRelocation 对话框

class CRelocation : public CDialogEx
{
	DECLARE_DYNAMIC(CRelocation)

public:
	CRelocation(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRelocation();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RELOCATION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	PEINFO* peInfo;
	DECLARE_MESSAGE_MAP()
public:
	void setPeInfo(PEINFO* pInfo);
	void setListTextR();
	CListCtrl m_list1;
	CListCtrl m_list2;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
};
