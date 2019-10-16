#pragma once


// CWindowDlg 对话框
#include <vector>
class CWindowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWindowDlg)

public:
	CWindowDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CWindowDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_WinTree;
	std::vector<DWORD> vOldHwnd;
	void UpdataWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	afx_msg void OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
};
