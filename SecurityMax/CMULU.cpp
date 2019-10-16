// CMULU.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CMULU.h"
#include "afxdialogex.h"


// CMULU 对话框

IMPLEMENT_DYNAMIC(CMULU, CDialogEx)

CMULU::CMULU(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MULU, pParent)
{

}

CMULU::~CMULU()
{
}

void CMULU::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CMULU::setPeInfo(PEINFO* pInfo)
{
	peInfo = pInfo;
}

void CMULU::setEditTextM()
{
	int n = 0;
	CString buffer;
	PIMAGE_DATA_DIRECTORY pData = peInfo->DataDirectory;
	for (int i = 0; i < peInfo->NumberOfRvaAndSizes; i++)
	{
		buffer.Format(L"%08X", pData->VirtualAddress);
		SetDlgItemText(list[n++], buffer);
		buffer.Format(L"%08X", pData->Size);
		SetDlgItemText(list[n++], buffer);
		pData++;
	}
}


BEGIN_MESSAGE_MAP(CMULU, CDialogEx)
END_MESSAGE_MAP()


// CMULU 消息处理程序


INT_PTR CMULU::DoModal()
{
	

	return CDialogEx::DoModal();
}


BOOL CMULU::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	setEditTextM();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
