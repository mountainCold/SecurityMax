// CDAORU.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDAORU.h"
#include "afxdialogex.h"


// CDAORU 对话框

IMPLEMENT_DYNAMIC(CDAORU, CDialogEx)

CDAORU::CDAORU(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DAORU, pParent)
{

}

CDAORU::~CDAORU()
{
}

void CDAORU::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
}

void CDAORU::setPeInfo(PEINFO* pInfo)
{
	peInfo = pInfo;
}

void CDAORU::setListText()
{
	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list1.InsertColumn(0, L"DLL名称", 0, 160);
	m_list1.InsertColumn(1, L"导入名称表RVA", 0, 120);
	m_list1.InsertColumn(2, L"日期时间标志", 0, 120);
	m_list1.InsertColumn(3, L"ForwarderString", 0, 120);
	m_list1.InsertColumn(4, L"名称", 0, 120);
	m_list1.InsertColumn(5, L"导入地址表RVA", 0,180);

	m_list2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list2.InsertColumn(0, L"ThunkRVA", 0, 120);
	m_list2.InsertColumn(1, L"Thunk偏移", 0, 120);
	m_list2.InsertColumn(2, L"Thunk值", 0, 120);
	m_list2.InsertColumn(3, L"hint", 0, 100);
	m_list2.InsertColumn(4, L"API名称", 0, 220);

	// 插入到列表控件
	int index = 0;
	CString buffer;
	PIMAGE_IMPORT_DESCRIPTOR pImport = peInfo->importT;
	if (pImport->OriginalFirstThunk == 0)
	{
		MessageBox(L"不存在该表");
		return;
	}
	while (pImport->Name)
	{
		m_list1.InsertItem(index, L"");
		char* pDllname = (char*)(RvaToFoa(peInfo,pImport->Name) + (DWORD)peInfo->m_buff);
		buffer.Format(L"%s", CATOCS(pDllname));
		m_list1.SetItemText(index, 0, buffer);
		buffer.Format(L"%08X", pImport->OriginalFirstThunk);
		m_list1.SetItemText(index,1, buffer);
		buffer.Format(L"%08X", pImport->TimeDateStamp);
		m_list1.SetItemText(index, 2, buffer);
		buffer.Format(L"%08X", pImport->ForwarderChain);
		m_list1.SetItemText(index, 3, buffer);
		buffer.Format(L"%08X", pImport->Name);
		m_list1.SetItemText(index, 4, buffer);
		buffer.Format(L"%08X", pImport->FirstThunk);
		m_list1.SetItemText(index, 5, buffer);
		
		// 解析下一个导入表
	
		pImport++;
		index++;
	}
}


BEGIN_MESSAGE_MAP(CDAORU, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDAORU::OnClickList1)
END_MESSAGE_MAP()


// CDAORU 消息处理程序


BOOL CDAORU::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	setListText();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDAORU::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int index =(int)m_list1.GetFirstSelectedItemPosition();
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
			buffer.Format(L"%s",CATOCS(pName->Name));
			m_list2.SetItemText(num, 4, buffer);

		}
		// 解析下一个函数
		pIAT++;
		num++;
	}
	for (size_t i = 0; i < num; i++)
	{
		buffer.Format(L"%08X", pImport->OriginalFirstThunk+i*4);
		m_list2.SetItemText(i, 0, buffer);
		DWORD dwSize = RvaToFoa(peInfo, pImport->OriginalFirstThunk)+i*4;
		buffer.Format(L"%08X", dwSize);
		m_list2.SetItemText(i, 1, buffer);
		DWORD* pstr = (DWORD*)(dwSize +(DWORD)peInfo->m_buff);
		buffer.Format(L"%08X",*pstr );
		m_list2.SetItemText(i, 2, buffer);
	}
}
