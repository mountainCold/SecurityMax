// CDAOCHU.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDAOCHU.h"
#include "afxdialogex.h"

// CDAOCHU 对话框

IMPLEMENT_DYNAMIC(CDAOCHU, CDialogEx)

CDAOCHU::CDAOCHU(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DAOCHU, pParent)
{

}

CDAOCHU::~CDAOCHU()
{
}

void CDAOCHU::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

void CDAOCHU::setPeInfo(PEINFO* pInfo)
{
	peInfo = pInfo;
}
void CDAOCHU::setEditTextE()
{
	CString buffer;
	PIMAGE_EXPORT_DIRECTORY pExport = peInfo->exportT;
	if (peInfo->exFoa == 0)
	{
		MessageBox(L"不存在导出表");
		return ;
	}
	buffer.Format(L"%08X", peInfo->exFoa);
	SetDlgItemText(IDC_EDIT1, buffer);
	buffer.Format(L"%08X", pExport->Characteristics);
	SetDlgItemText(IDC_EDIT2, buffer);
	buffer.Format(L"%08X", pExport->Base);
	SetDlgItemText(IDC_EDIT3, buffer);
	buffer.Format(L"%08X", pExport->Name);
	SetDlgItemText(IDC_EDIT4, buffer);
	char* str =(char*) peInfo->exname;
	buffer.Format(L"%s", CATOCS(str));
	SetDlgItemText(IDC_EDIT5, buffer);
	buffer.Format(L"%08X", pExport->NumberOfFunctions);
	SetDlgItemText(IDC_EDIT6, buffer);
	buffer.Format(L"%08X", pExport->NumberOfNames);
	SetDlgItemText(IDC_EDIT7, buffer);
	buffer.Format(L"%08X", pExport->AddressOfFunctions);
	SetDlgItemText(IDC_EDIT8, buffer);
	buffer.Format(L"%08X", pExport->AddressOfNames);
	SetDlgItemText(IDC_EDIT9, buffer);
	buffer.Format(L"%08X", pExport->AddressOfNameOrdinals);
	SetDlgItemText(IDC_EDIT10, buffer);

	DWORD dwNameCount = pExport->NumberOfNames;
	for (size_t i = 0; i < pExport->NumberOfNames; i++)
	{
		m_list.InsertItem(i, _T(""));
		buffer.Format(_T("%04X"), i+pExport->Base);
		m_list.SetItemText(i, 0,buffer);
		buffer.Format(_T("%08X"), peInfo->pEAT[i]);
		m_list.SetItemText(i, 1, buffer);
		buffer.Format(_T("%08X"), RvaToFoa(peInfo,peInfo->pEAT[i]));
		m_list.SetItemText(i, 2, buffer);
		for (int j = 0; j < dwNameCount; j++)
		{
			// 序号表中有地址表的序号 （这个函数是有名字的）
			if (i == peInfo->pEOT[j]) {
				// 输出这个函数名字 
				char* pFunName =
					(char*)(RvaToFoa(peInfo, peInfo->pENT[j]) +(char*) peInfo->m_buff);
				buffer.Format(_T("%s"),CATOCS(pFunName));
				m_list.SetItemText(i, 3, buffer);
				break;
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CDAOCHU, CDialogEx)
END_MESSAGE_MAP()


// CDAOCHU 消息处理程序


BOOL CDAOCHU::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_list.InsertColumn(0, L"序号", 0, 70);
	m_list.InsertColumn(1, L"RVA", 0, 110);
	m_list.InsertColumn(2, L"偏移", 0, 120);
	m_list.InsertColumn(3, L"函数名", 0, 140);
	setEditTextE();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
