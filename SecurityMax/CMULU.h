#pragma once
#include "CTool.h"


// CMULU 对话框

class CMULU : public CDialogEx
{
	DECLARE_DYNAMIC(CMULU)

public:
	CMULU(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMULU();
	int list[32] = { IDC_EDITT1,IDC_EDITT2, IDC_EDITT3, IDC_EDITT4, IDC_EDITT5, IDC_EDITT6, IDC_EDITT7, IDC_EDITT8,
				IDC_EDITT9,IDC_EDITT10, IDC_EDITT11, IDC_EDITT12, IDC_EDITT13, IDC_EDITT14, IDC_EDITT15, IDC_EDITT16,
				IDC_EDITT17,IDC_EDITT18, IDC_EDITT19, IDC_EDITT20, IDC_EDITT21, IDC_EDITT22, IDC_EDITT23, IDC_EDITT24,
				IDC_EDITT25,IDC_EDITT26, IDC_EDITT27, IDC_EDITT28, IDC_EDITT29, IDC_EDITT30, IDC_EDITT31, IDC_EDITT32 };
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MULU };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	PEINFO* peInfo;
	void setPeInfo(PEINFO* pInfo);
	void setEditTextM();
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();
};

