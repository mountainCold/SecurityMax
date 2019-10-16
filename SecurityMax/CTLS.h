#pragma once
#include "CTool.h"


// CTLS 对话框

class CTLS : public CDialogEx
{
	DECLARE_DYNAMIC(CTLS)

public:
	CTLS(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTLS();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TLS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	PEINFO* peInfo;
	DECLARE_MESSAGE_MAP()
public:
	void setPeInfo(PEINFO* pInfo);
	void setEditTextT();
	virtual BOOL OnInitDialog();
};
