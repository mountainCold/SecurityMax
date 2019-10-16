#pragma once
#include "CTool.h"


// CDAORU 对话框

class CDAORU : public CDialogEx
{
	DECLARE_DYNAMIC(CDAORU)

public:
	CDAORU(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDAORU();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DAORU };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	PEINFO* peInfo;
	DECLARE_MESSAGE_MAP()
public:
	void setPeInfo(PEINFO* pInfo);
	void setListText();
	CListCtrl m_list1;
	virtual BOOL OnInitDialog();
	CListCtrl m_list2;
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
};
