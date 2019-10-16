#pragma once


// CDlgC 对话框

class CDlgC : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgC)

public:
	CDlgC(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgC();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_path;
	CListCtrl m_list;
	CComboBox m_mBox;
	void setListTextF1(CString path, DWORD* num);
	void setListTextF2(CString path, DWORD* num);
	void setListTextF3(CString path, DWORD* num);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedButton1();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnClickedButton2();
	afx_msg void OnClickedButton3();
	CButton m_c1;
	CButton m_c2;
	CButton m_c3;
};
