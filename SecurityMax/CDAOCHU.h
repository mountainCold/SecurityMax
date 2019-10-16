#pragma once
#include "CTool.h"


// CDAOCHU 对话框

class CDAOCHU : public CDialogEx
{
	DECLARE_DYNAMIC(CDAOCHU)

public:
	CDAOCHU(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDAOCHU();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DAOCHU };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	PEINFO* peInfo;
	void setPeInfo(PEINFO* pInfo);
	void setEditTextE();
	virtual BOOL OnInitDialog();
	CListCtrl m_list;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
