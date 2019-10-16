#pragma once
#include "CTool.h"


// CRESOURCE 对话框

class CRESOURCE : public CDialogEx
{
	DECLARE_DYNAMIC(CRESOURCE)

public:
	CRESOURCE(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRESOURCE();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESOURCE};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	PEINFO* peInfo;
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
	void setPeInfo(PEINFO* pInfo);
	void setTreeText();
	
	virtual BOOL OnInitDialog();
};
