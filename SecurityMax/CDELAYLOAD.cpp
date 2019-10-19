// CDELAYLOAD.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDELAYLOAD.h"
#include "afxdialogex.h"


// CDELAYLOAD 对话框

IMPLEMENT_DYNAMIC(CDELAYLOAD, CDialogEx)

CDELAYLOAD::CDELAYLOAD(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DELAYLOAD, pParent)
{

}

CDELAYLOAD::~CDELAYLOAD()
{
}

void CDELAYLOAD::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
}

void CDELAYLOAD::setPeInfo(PEINFO* pInfo)
{
	peInfo = pInfo;
}

void CDELAYLOAD::setListTextDL()
{
	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list1.InsertColumn(0, L"Attributes", 0, 80);
	m_list1.InsertColumn(1, L"DALLNameRVA", 0, 120);
	m_list1.InsertColumn(2, L"ModuleHandleRVA", 0, 120);
	m_list1.InsertColumn(3, L"延时载入IAT的RVA", 0, 120);
	m_list1.InsertColumn(4, L"延时载入INT的RVA", 0, 120);
	m_list1.InsertColumn(5, L"绑定IAT的RVA", 0, 120);
	m_list1.InsertColumn(6, L"原始IAT的可选拷贝的RVA", 0, 180);
	m_list1.InsertColumn(7, L"延迟载入DLL的时间戳", 0, 180);

	m_list2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list2.InsertColumn(0, L"ThunkRVA", 0, 120);
	m_list2.InsertColumn(1, L"Thunk偏移", 0, 120);
	m_list2.InsertColumn(2, L"Thunk值", 0, 120);
	m_list2.InsertColumn(3, L"hint", 0, 100);
	m_list2.InsertColumn(4, L"API名称", 0, 220);
	// 插入到列表控件
	int index = 0;
	CString buffer;
	PIMAGE_DELAYLOAD_DESCRIPTOR pDelayload = peInfo->delayload;
	if (pDelayload->ImportNameTableRVA == 0)
	{
		MessageBox(L"不存在该表");
		return;
	}
	while (pDelayload->DllNameRVA)
	{
		m_list1.InsertItem(index, L"");
		buffer.Format(L"%08X", pDelayload->Attributes);
		m_list1.SetItemText(index, 0, buffer);
		char* pDllname = (char*)(RvaToFoa(peInfo, pDelayload->DllNameRVA) + (DWORD)peInfo->m_buff);
		buffer.Format(L"%s", CATOCS(pDllname));
		m_list1.SetItemText(index, 1, buffer);
		buffer.Format(L"%08X", pDelayload->ModuleHandleRVA);
		m_list1.SetItemText(index, 2, buffer);
		buffer.Format(L"%08X", pDelayload->ImportAddressTableRVA);
		m_list1.SetItemText(index, 3, buffer);
		buffer.Format(L"%08X", pDelayload->ImportAddressTableRVA);
		m_list1.SetItemText(index, 4, buffer);
		buffer.Format(L"%08X", pDelayload->BoundImportAddressTableRVA);
		m_list1.SetItemText(index, 5, buffer);
		buffer.Format(L"%08X", pDelayload->UnloadInformationTableRVA);
		m_list1.SetItemText(index, 6, buffer);
		buffer.Format(L"%08X", pDelayload->TimeDateStamp);
		m_list1.SetItemText(index, 7, buffer);

		// 解析下一个导入表

		pDelayload++;
		index++;
	}
}


BEGIN_MESSAGE_MAP(CDELAYLOAD, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDELAYLOAD::OnClickList1)
END_MESSAGE_MAP()


// CDELAYLOAD 消息处理程序


BOOL CDELAYLOAD::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	setListTextDL();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDELAYLOAD::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int index = (int)m_list1.GetFirstSelectedItemPosition();
	if (index == 0) {
		return;
	}
	index -= 1;
	CString buffer;
	PIMAGE_IMPORT_DESCRIPTOR pImport = peInfo->importT;
	pImport += index;
	PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(RvaToFoa(peInfo, pImport->FirstThunk) + (DWORD)peInfo->m_buff);
	int num = 0;
	while (pIAT->u1.Ordinal)
	{
		m_list2.InsertItem(num, L"");
		// 判断导入的函数是名称导入还是序号导入
		// 判断最高位是否位1 ，如果为1那么需要导入
		if (pIAT->u1.Ordinal & 0x80000000)
		{
			//printf("序号 %d\n", pIAT->u1.Function & 0x7FFFFFFF);
			buffer.Format(L"%08X", pIAT->u1.Function & 0x7FFFFFFF);
			m_list2.SetItemText(num, 3, buffer);

		}
		else {
			// 导入的函数是名称
			PIMAGE_IMPORT_BY_NAME pName =
				(PIMAGE_IMPORT_BY_NAME)(RvaToFoa(peInfo, pIAT->u1.AddressOfData) + (DWORD)peInfo->m_buff);
			//printf("序号 %d  函数名：%s\n", pName->Hint, pName->Name);
			buffer.Format(L"%04X", pName->Hint);
			m_list2.SetItemText(num, 3, buffer);
			buffer.Format(L"%s", CATOCS(pName->Name));
			m_list2.SetItemText(num, 4, buffer);

		}
		// 解析下一个函数
		pIAT++;
		num++;
	}
	for (size_t i = 0; i < num; i++)
	{
		buffer.Format(L"%08X", pImport->OriginalFirstThunk + i * 4);
		m_list2.SetItemText(i, 0, buffer);
		DWORD dwSize = RvaToFoa(peInfo, pImport->OriginalFirstThunk) + i * 4;
		buffer.Format(L"%08X", dwSize);
		m_list2.SetItemText(i, 1, buffer);
		DWORD* pstr = (DWORD*)(dwSize + (DWORD)peInfo->m_buff);
		buffer.Format(L"%08X", *pstr);
		m_list2.SetItemText(i, 2, buffer);
	}
}
