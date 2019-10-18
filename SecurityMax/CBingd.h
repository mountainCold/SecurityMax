#pragma once
#include "CTool.h"
#include <vector>

// CBingd 对话框

class CBingd : public CDialogEx
{
	DECLARE_DYNAMIC(CBingd)

public:
	CBingd(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBingd();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG6 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString title;
	CListCtrl m_list;
	std::vector<bingd> listD;
	void setData(std::vector<bingd> listB, CString t);
	virtual BOOL OnInitDialog();
};
