// CRelocation.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CRelocation.h"
#include "afxdialogex.h"


// CRelocation 对话框

IMPLEMENT_DYNAMIC(CRelocation, CDialogEx)

CRelocation::CRelocation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RELOCATION, pParent)
{

}

CRelocation::~CRelocation()
{
}

void CRelocation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
}
typedef	struct {
	WORD Offset : 12;  // (1) 大小为12Bit的重定位偏移
	WORD Type : 4;   // (2) 大小为4Bit的重定位信息类型值
}*PTYPEOFFSET;	    // 这个结构体是A1Pass总结的
void CRelocation::setPeInfo(PEINFO* pInfo)
{
	peInfo = pInfo;
}

void CRelocation::setListTextR()
{
	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list1.InsertColumn(0, L"索引", 0, 60);
	m_list1.InsertColumn(1, L"区段", 0, 120);
	m_list1.InsertColumn(2, L"RVA", 0, 120);
	m_list1.InsertColumn(3, L"项目", 0, 120);

	m_list2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list2.InsertColumn(0, L"索引", 0, 60);
	m_list2.InsertColumn(1, L"RVA", 0, 120);
	m_list2.InsertColumn(2, L"偏移", 0, 120);
	m_list2.InsertColumn(3, L"类型", 0, 80);
	m_list2.InsertColumn(4, L"FAR地址", 0, 120);
	m_list2.InsertColumn(5, L"数据解释", 0, 220);

	int index = 0;
	CString buffer;
	PIMAGE_BASE_RELOCATION pRelocal = peInfo->relocation;
	while (pRelocal->SizeOfBlock)
	{
		m_list1.InsertItem(index, L"");
		buffer.Format(L"%d",index+1);
		m_list1.SetItemText(index, 0, buffer);
		DWORD pS = GetSection(peInfo, pRelocal->VirtualAddress);
		buffer.Format(L"%s",CATOCS((char*)peInfo->sectionHead[pS].Name));
		m_list1.SetItemText(index, 1, buffer);
		buffer.Format(L"%08X", pRelocal->VirtualAddress);
		m_list1.SetItemText(index, 2, buffer);
		// 获取重定位项个数
		DWORD dwCount = (pRelocal->SizeOfBlock - 8) / 2;
		buffer.Format(L"%02X%s%02d%s", dwCount,L"h / ",dwCount,"d");
		m_list1.SetItemText(index, 3, buffer);
		//（0x1000字节中的需要重定位数据的偏移）



		// 遍历下一重定位表
		index++;
		pRelocal =
			(PIMAGE_BASE_RELOCATION)((DWORD)pRelocal + pRelocal->SizeOfBlock);
	}
}


BEGIN_MESSAGE_MAP(CRelocation, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CRelocation::OnClickList1)
END_MESSAGE_MAP()


// CRelocation 消息处理程序


BOOL CRelocation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	setListTextR();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CRelocation::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult)
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
	PIMAGE_BASE_RELOCATION pRelocal = peInfo->relocation;
	PTYPEOFFSET pRelocalBlock = (PTYPEOFFSET)(pRelocal + index);
	// 遍历重定位项数据
	for (int i = 0; i < (pRelocal->SizeOfBlock - 8) / 2; i++)
	{
		m_list2.InsertItem(i, L"");
		buffer.Format(L"%d", i + 1);
		m_list2.SetItemText(i, 0, buffer);
		// （x32位PE文件）类型为0x03	表示需要重定位的数据  
		// （x64位PE文件）类型为0x10	表示需要重定位的数据  
			// 要修复数据的RVA
		DWORD Rva = pRelocalBlock[i].Offset + pRelocal->VirtualAddress;
		buffer.Format(L"%08X", Rva);
		m_list2.SetItemText(i, 1, buffer);
		DWORD foa = RvaToFoa(peInfo, Rva);
		buffer.Format(L"%08X", foa);
		m_list2.SetItemText(i, 2, buffer);
		buffer.Format(L"%d", pRelocalBlock[i].Type);
		m_list2.SetItemText(i, 3, buffer);
		DWORD* pa = (DWORD*)(foa + (char*)peInfo->m_buff);
		buffer.Format(L"%08X",*pa );
		m_list2.SetItemText(i, 4, buffer);
		DWORD off = *pa - peInfo->ImageBase;
		DWORD* str = (DWORD*)(RvaToFoa(peInfo, off)+(char*)peInfo->m_buff);
		buffer.Format(L"%08X", *str);
		m_list2.SetItemText(i, 5, buffer);
			//printf("	RVA:%08X\n", Rva);
			//printf("	FOA:%08X\n", RvaToFoa(Rva));
			//printf("\n");
			// 修复重定位为0x00800000
			//*(DWORD*)(RvaToFoa(Rva) + m_buff) =
				//*(DWORD*)(RvaToFoa(Rva) + m_buff) - 0x00400000 + 0x00800000;
	}
}
