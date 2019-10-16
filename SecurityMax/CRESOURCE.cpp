// CRESOURCE.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CRESOURCE.h"
#include "afxdialogex.h"


// CRESOURCE 对话框

IMPLEMENT_DYNAMIC(CRESOURCE, CDialogEx)

CRESOURCE::CRESOURCE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESOURCE, pParent)
{

}

CRESOURCE::~CRESOURCE()
{
}


void CRESOURCE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
}

void CRESOURCE::setPeInfo(PEINFO* pInfo)
{
	peInfo = pInfo;
}
const WCHAR* RESW[20] = {
	L"光标",
	L"位图",
	L"图标",
	L"菜单",
	L"对话框",
	L"字符串列表",
	L"字体目录",
	L"字体",
	L"快捷键",
	L"非格式化资源",
	L"消息列表",
	L"鼠标指针数组",
	L"NULL",
	L"图标组",
	L"NULL",
	L"版本信息",
};
void CRESOURCE::setTreeText()
{
	PIMAGE_RESOURCE_DIRECTORY pResDirOne = peInfo->resource;

	// 获取资源目录项
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResOneEntry =
		(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirOne + 1);

	//资源目录个数
	DWORD dwOneCount = pResDirOne->NumberOfIdEntries + pResDirOne->NumberOfNamedEntries;

	CString buffer;
	//遍历第一层资源信息
	for (int i = 0; i < dwOneCount; i++)
	{
		HTREEITEM hItem = 0;
		// 判断资源是什么类型
		// 资源是字符串作为标识
		if (pResOneEntry[i].NameIsString)
		{
			// 如果NameIsString == 1 那么 NameOffset有效
			PIMAGE_RESOURCE_DIR_STRING_U pName =
				(PIMAGE_RESOURCE_DIR_STRING_U)(pResOneEntry[i].NameOffset + (DWORD)pResDirOne);
			// 输出资源类型名
			WCHAR wResName[100];
			wcsncpy_s(wResName, pName->NameString, pName->Length);
			buffer.Format(L"%s%s", L"资源类型:", wResName);
			hItem = m_Tree.InsertItem(buffer, hItem);
			//hItem=m_Tree.InsertItem(wResName, hItem);
			//wprintf(L"%s\n", wResName);
		}
		// 资源是ID作为标识
		else {
			// 系统提供的资源类型
			if (pResOneEntry[i].Id < 16)
			{
				buffer.Format(L"%s%s", L"资源类型:", RESW[pResOneEntry[i].Id]);
				hItem = m_Tree.InsertItem(buffer, hItem);
				//wprintf(L"%s\n", RESW[pResOneEntry[i].Id]);
			}
			else {
				buffer.Format(L"%s%08x", L"资源类型:", pResOneEntry[i].Id);
				hItem = m_Tree.InsertItem(buffer, hItem);
				//wprintf(L"%08x\n", pResOneEntry[i].Id);
			}

		}


		// 第二次：资源的叫什么名字 （1.png, 2.png）
		// 是否有第二层目录
		if (pResOneEntry[i].DataIsDirectory) {
			HTREEITEM hItem2 = hItem;
			// 找到第二层目录表
			PIMAGE_RESOURCE_DIRECTORY pResDirTwo =
				(PIMAGE_RESOURCE_DIRECTORY)(pResOneEntry[i].OffsetToDirectory + (DWORD)pResDirOne);

			// 找到第二层目录项
			PIMAGE_RESOURCE_DIRECTORY_ENTRY pResTwoEntry =
				(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirTwo + 1);

			// 获取这种资源有多少个
			DWORD ResTowCount = pResDirTwo->NumberOfIdEntries + pResDirTwo->NumberOfNamedEntries;
			for (int j = 0; j < ResTowCount; j++)
			{
				// 资源是字符串作为标识
				if (pResTwoEntry[j].NameIsString)
				{
					// 如果NameIsString == 1 那么 NameOffset有效
					PIMAGE_RESOURCE_DIR_STRING_U pName =
						(PIMAGE_RESOURCE_DIR_STRING_U)(pResTwoEntry[j].NameOffset + (DWORD)pResDirOne);
					// 输出资源类型名
					WCHAR wResName[100];
					wcsncpy_s(wResName, pName->NameString, pName->Length);
					buffer.Format(L"%s%s", L"资源类型名:", wResName);
					hItem2 = m_Tree.InsertItem(buffer, hItem);
					//wprintf(L"		%s\n", wResName);
				}
				else {
					buffer.Format(L"%s%08x", L"资源类型名:", pResTwoEntry[j].Id);
					hItem2 = m_Tree.InsertItem(buffer, hItem);
					//printf("		%08x\n", pResTwoEntry[j].Id);
				}

				// 第三层：资源的语言，资源的信息（大小，文件中位置）
				if (pResTwoEntry[j].DataIsDirectory)
				{
					// 获取第三层目录表
					PIMAGE_RESOURCE_DIRECTORY pResDirThree =
						(PIMAGE_RESOURCE_DIRECTORY)(pResTwoEntry[j].OffsetToDirectory + (DWORD)pResDirOne);
					// 获取第三层目录项
					PIMAGE_RESOURCE_DIRECTORY_ENTRY pResThreeEntry =
						(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirThree + 1);

					// 获取资源真正信息
					PIMAGE_RESOURCE_DATA_ENTRY pResData =
						(PIMAGE_RESOURCE_DATA_ENTRY)(pResThreeEntry->OffsetToData + (DWORD)pResDirOne);
					//buffera.Format(L"%08X", pResData->OffsetToData);
					//bufferb.Format(L"%08X", pResData->Size);
					buffer.Format(L"%s%08X,%s%08X", L"资源位置RVA：",  pResData->OffsetToData,L"资源位置Size：", pResData->Size);
					m_Tree.InsertItem(buffer, hItem2);
					//printf("			资源位置RVA： %08X\n", pResData->OffsetToData);
					//printf("			资源位置Size： %08X\n", pResData->Size);
				}
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CRESOURCE, CDialogEx)
END_MESSAGE_MAP()


// CRESOURCE 消息处理程序


BOOL CRESOURCE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	setTreeText();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
