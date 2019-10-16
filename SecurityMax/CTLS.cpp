// CTLS.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CTLS.h"
#include "afxdialogex.h"


// CTLS 对话框

IMPLEMENT_DYNAMIC(CTLS, CDialogEx)

CTLS::CTLS(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TLS, pParent)
{

}

CTLS::~CTLS()
{
}

void CTLS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CTLS::setPeInfo(PEINFO* pInfo)
{
	peInfo = pInfo;
}

void CTLS::setEditTextT()
{
	CString buffer;
	PIMAGE_TLS_DIRECTORY32 pTLS = peInfo->TLS;
	buffer.Format(L"%08X", pTLS->StartAddressOfRawData);
	SetDlgItemText(IDC_EDIT1, buffer);
	buffer.Format(L"%08X", pTLS->EndAddressOfRawData);
	SetDlgItemText(IDC_EDIT2, buffer);
	buffer.Format(L"%08X", pTLS->AddressOfIndex);
	SetDlgItemText(IDC_EDIT3, buffer);
	buffer.Format(L"%08X", pTLS->AddressOfCallBacks);
	SetDlgItemText(IDC_EDIT4, buffer);
	buffer.Format(L"%08X", pTLS->SizeOfZeroFill);
	SetDlgItemText(IDC_EDIT5, buffer);
	buffer.Format(L"%08X", pTLS->Characteristics);
	SetDlgItemText(IDC_EDIT6, buffer);
}


BEGIN_MESSAGE_MAP(CTLS, CDialogEx)
END_MESSAGE_MAP()


// CTLS 消息处理程序


BOOL CTLS::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	setEditTextT();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
