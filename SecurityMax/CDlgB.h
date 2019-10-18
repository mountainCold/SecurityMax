#pragma once
#include "Client.h"
#include <vector>
#include "CTool.h"
// CDlgB 对话框

class CDlgB : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgB)

public:
	CDlgB(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgB();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	std::vector<bingd> listBD;
	std::vector<bingd> listWL;
	CMenu   m_menu;
	CListCtrl m_list1;
	CListCtrl m_list2;
	CButton m_r1;
	CButton m_r2;
	CButton m_c1;
	CString m_path;
	virtual BOOL OnInitDialog();
	void setListTextD1(CString path, DWORD* num);
	void setListTextD2();
	afx_msg void OnClickedRadio2();
	afx_msg void OnClickedButton1();
	afx_msg void OnClickedButton2();
	afx_msg void OnClickedButton3();
	afx_msg void OnClickedButton4();
	afx_msg void OnClickedButton5();
	afx_msg void OnClickedButton6();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnRclickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32784();
	afx_msg void On32785();
};
