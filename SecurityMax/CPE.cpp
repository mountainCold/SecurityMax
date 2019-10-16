#include "pch.h"
#include "CPE.h"
#include <stdio.h>
#include <ctime>

bool CPE::InitPe(TCHAR* path)
{
	m_path = path;
	HANDLE hFile = CreateFile(
		path,		//文件路径
		GENERIC_READ,//打开方式 读
		FILE_SHARE_READ,//共享方式
		NULL,			//安全属性
		OPEN_EXISTING,	//文件必须已存在
		FILE_ATTRIBUTE_NORMAL,//文件属性
		NULL
	);
	m_size = GetFileSize(hFile, 0);
	m_buff = new char[m_size];
	DWORD dwReadSize;
	ReadFile(hFile, m_buff, m_size, &dwReadSize, 0);
	return true;
}
PIMAGE_DOS_HEADER CPE::GetDosHeader()
{
	return (PIMAGE_DOS_HEADER)m_buff;
}
PIMAGE_NT_HEADERS CPE::GetNtHeader()
{
	PIMAGE_DOS_HEADER pDOS = GetDosHeader();
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)(pDOS->e_lfanew + (DWORD)m_buff);

	return pNT;
}
DWORD CPE::RvaToFoa(DWORD dwRva)
{
	PIMAGE_SECTION_HEADER pSection = GetSectionHeader();
	PIMAGE_FILE_HEADER pFileHeader = &GetNtHeader()->FileHeader;
	DWORD dwSectionCount = pFileHeader->NumberOfSections;
	DWORD Offset = 0;
	for (size_t i = 0; i < dwSectionCount; i++)
	{
		// RVA > 这个区段所在的RVA && 
		// RVA<= 区段所在的RVA+区段在文件中的大小
		if ((dwRva >= pSection[i].VirtualAddress) &&
			dwRva < (pSection[i].VirtualAddress + pSection[i].SizeOfRawData))
		{
			// 计算公式  offset = RVA - 区段所在的RVA + 区段在文件中的偏移
			Offset = dwRva - pSection[i].VirtualAddress + pSection[i].PointerToRawData;
		}
	}
	return Offset;
}
bool CPE::IsPeFile()
{
	PIMAGE_DOS_HEADER pDos = GetDosHeader();
	if (pDos->e_lfanew != IMAGE_DOS_SIGNATURE)
	{
		return false;
	}
	PIMAGE_NT_HEADERS pNT = GetNtHeader();
	if (pNT->Signature != IMAGE_NT_SIGNATURE)
	{
		return false;
	}
	return true;
}

PIMAGE_SECTION_HEADER CPE::GetSectionHeader()
{
	PIMAGE_NT_HEADERS pNT = GetNtHeader();
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNT);
	return pSection;
}

PIMAGE_EXPORT_DIRECTORY CPE::GetExprotDirectory()
{
	DWORD dwOffset = GetExprotFOA();
	// 导出表的地址
	PIMAGE_EXPORT_DIRECTORY pExport =
		(PIMAGE_EXPORT_DIRECTORY)(dwOffset + (DWORD)m_buff);
	return pExport;
}
DWORD CPE::GetExprotFOA()
{
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	//获取导出表的RVA
	DWORD ExportRva = pNt->OptionalHeader.DataDirectory[0].VirtualAddress;
	//转FOA
	DWORD dwOffset = RvaToFoa(ExportRva);
	return dwOffset;
}
PIMAGE_IMPORT_DESCRIPTOR CPE::GetImportDirectory()
{
	// 通过数据目录表第 1 项
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	// 获取导入表的RVA
	DWORD dwImportRva = pNt->OptionalHeader.DataDirectory[1].VirtualAddress;
	// 转FOA
	DWORD dwOffset = RvaToFoa(dwImportRva);
	// 导出表的地址
	PIMAGE_IMPORT_DESCRIPTOR pImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(dwOffset + (DWORD)m_buff);

	return pImport;

}
PIMAGE_RESOURCE_DIRECTORY CPE::GetResourceDirectory()
{
	// 获取数据目录表第 2项
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[2].VirtualAddress;

	// 获取资源表结构体
	PIMAGE_RESOURCE_DIRECTORY pRes =
		(PIMAGE_RESOURCE_DIRECTORY)(RvaToFoa(dwRva) + m_buff);

	return pRes;
}
PIMAGE_TLS_DIRECTORY32 CPE::GetTLSDirectory()
{
	// 获取数据目录表第 9项
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[9].VirtualAddress;

	// 获取TLS表结构体
	PIMAGE_TLS_DIRECTORY32 pRes =
		(PIMAGE_TLS_DIRECTORY32)(RvaToFoa(dwRva) + m_buff);

	return pRes;
}
PIMAGE_BASE_RELOCATION CPE::GetRelocalDirectory()
{
	// 获取数据目录表第 5项
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[5].VirtualAddress;

	// 获取重定位表结构体
	PIMAGE_BASE_RELOCATION pRelocal =
		(PIMAGE_BASE_RELOCATION)(RvaToFoa(dwRva) + m_buff);

	return pRelocal;
}

PIMAGE_DELAYLOAD_DESCRIPTOR CPE::GetDelayloadDirectory()
{
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[13].VirtualAddress;

	// 获取重定位表结构体
	PIMAGE_DELAYLOAD_DESCRIPTOR pDelayload =
		(PIMAGE_DELAYLOAD_DESCRIPTOR)(RvaToFoa(dwRva) + m_buff);
	return pDelayload;
}

void CPE::ShowFileHeader()
{
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	printf("文件运行平台: %08x\n", pNt->FileHeader.Machine);
	printf("区段    数量: %08x\n", pNt->FileHeader.NumberOfSections);
	struct tm* p = new tm();
	time_t* t = (time_t*)& pNt->FileHeader.TimeDateStamp;
	localtime_s(p, t);
	printf("文件创建时间：%04d/%02d/%02d ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
	printf("%02d:%02d:%02d\n", p->tm_hour, p->tm_min, p->tm_sec);
}

void CPE::ShowTLSInfo()
{
	PIMAGE_TLS_DIRECTORY32 pTls = GetTLSDirectory();
	printf("源数据起始位置VA：%08x\n", pTls->StartAddressOfRawData);
	printf("源数据结束位置VA：%08x\n", pTls->EndAddressOfRawData);
	printf("回调函数表位置VA：%08x\n", pTls->AddressOfCallBacks);
}
void CPE::ShowOptionHeader()
{
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	//显示 扩展头中的数据
	printf("OEP: %08x\nImageBase %08x\n",
		pNt->OptionalHeader.AddressOfEntryPoint,
		pNt->OptionalHeader.ImageBase
	);
	//显式数据目录表
	for (int i = 0; i < 16; ++i)
	{
		printf("Directory[%d].VirtualAddress = %08x  ",
			i,
			pNt->OptionalHeader.DataDirectory[i].VirtualAddress);

		printf("Directory[%d].Size = %08x \n",
			i,
			pNt->OptionalHeader.DataDirectory[i].Size);
	}
}

void CPE::ShowSectionInfo()
{
	PIMAGE_SECTION_HEADER pSection = GetSectionHeader();
	DWORD dwSize = GetNtHeader()->FileHeader.NumberOfSections;
	// 遍历区段表信息
	for (int i = 0; i < dwSize; i++)
	{
		printf("Name :%s", pSection[i].Name);
		printf(" VSize:%08X", pSection[i].Misc.VirtualSize);
		printf(" VOffset:%08X", pSection[i].VirtualAddress);
		printf(" RSize:%08X", pSection[i].SizeOfRawData);
		printf(" ROffset:%08X", pSection[i].PointerToRawData);
		printf("\n");
	}
}

void CPE::ShowExprotInfo()
{
	PIMAGE_EXPORT_DIRECTORY pExport = GetExprotDirectory();
	// 解析导出表信息
	// 解析dll名字  RVA
	char* pDllName = (char*)(RvaToFoa(pExport->Name) + m_buff);
	printf("%s\n", pDllName);
	DWORD* pEAT = (DWORD*)(RvaToFoa(pExport->AddressOfFunctions) + m_buff);
	// 导出名称表
	DWORD* pENT =
		(DWORD*)(RvaToFoa(pExport->AddressOfNames) + m_buff);
	// 导出序号表 元素是word
	WORD* pEOT =
		(WORD*)(RvaToFoa(pExport->AddressOfNameOrdinals) + m_buff);
	// 导出地址表个数
	DWORD dwAddrCount = pExport->NumberOfFunctions;
	// 导出名称表个数
	DWORD dwNameCount = pExport->NumberOfNames;

	// 遍历地址表
	for (int i = 0; i < dwAddrCount; i++)
	{
		//输出函数序号
		printf("序号 %d ", i + pExport->Base);

		// 遍历序号表
		for (int j = 0; j < dwNameCount; j++)
		{
			// 序号表中有地址表的序号 （这个函数是有名字的）
			if (i == pEOT[j]) {
				// 输出这个函数名字 
				char* pFunName =
					(char*)(RvaToFoa(pENT[j]) + m_buff);
				printf("%s", pFunName);
				break;
			}
		}

		printf("\n");
	}
}
DWORD* CPE::GetExportENT()
{
	PIMAGE_EXPORT_DIRECTORY pExport = GetExprotDirectory();
	DWORD* pENT =
		(DWORD*)(RvaToFoa(pExport->AddressOfNames) + m_buff);
	return pENT;
}
DWORD* CPE::GetExportEAT()
{
	PIMAGE_EXPORT_DIRECTORY pExport = GetExprotDirectory();
	DWORD* pEAT = (DWORD*)(RvaToFoa(pExport->AddressOfFunctions) + m_buff);
	return pEAT;
}
WORD* CPE::GetExportEOT()
{
	PIMAGE_EXPORT_DIRECTORY pExport = GetExprotDirectory();
	WORD* pEOT =
		(WORD*)(RvaToFoa(pExport->AddressOfNameOrdinals) + m_buff);
	return pEOT;
}
void CPE::ShowImportInfo()
{
	PIMAGE_IMPORT_DESCRIPTOR pImport = GetImportDirectory();
	while (pImport->Name)
	{
		char* pDllname = (char*)(RvaToFoa(pImport->Name) + (DWORD)m_buff);
		printf("%s\n", pDllname);

		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(RvaToFoa(pImport->FirstThunk) + (DWORD)m_buff);
		while (pIAT->u1.Ordinal)
		{
			// 判断导入的函数是名称导入还是序号导入
			// 判断最高位是否位1 ，如果为1那么需要导入
			if (pIAT->u1.Ordinal & 0x80000000)
			{
				printf("序号 %d\n", pIAT->u1.Function & 0x7FFFFFFF);
			}
			else {
				// 导入的函数是名称
				PIMAGE_IMPORT_BY_NAME pName =
					(PIMAGE_IMPORT_BY_NAME)(RvaToFoa(pIAT->u1.AddressOfData) + (DWORD)m_buff);
				printf("序号 %d  函数名：%s\n", pName->Hint, pName->Name);

			}
			// 解析下一个函数
			pIAT++;
		}
		// 解析下一个导入表
		pImport++;
	}
}
const WCHAR* RES[20] = {
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
void CPE::ShowResourceInfo()
{
	// 第一层：资源的类型是什么（图标，位图，菜单....）
	// 获取资源目录
	PIMAGE_RESOURCE_DIRECTORY pResDirOne = GetResourceDirectory();

	// 获取资源目录项
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResOneEntry =
		(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirOne + 1);

	//资源目录个数
	DWORD dwOneCount = pResDirOne->NumberOfIdEntries + pResDirOne->NumberOfNamedEntries;


	//遍历第一层资源信息
	for (int i = 0; i < dwOneCount; i++)
	{
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
			wprintf(L"%s\n", wResName);
		}
		// 资源是ID作为标识
		else {
			// 系统提供的资源类型
			if (pResOneEntry[i].Id < 16)
			{
				wprintf(L"%s\n", RES[pResOneEntry[i].Id]);
			}
			else {
				wprintf(L"%08x\n", pResOneEntry[i].Id);
			}

		}


		// 第二次：资源的叫什么名字 （1.png, 2.png）
		// 是否有第二层目录
		if (pResOneEntry[i].DataIsDirectory) {

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
					wprintf(L"		%s\n", wResName);
				}
				else {
					printf("		%08x\n", pResTwoEntry[j].Id);
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

					printf("			资源位置RVA： %08X\n", pResData->OffsetToData);
					printf("			资源位置Size： %08X\n", pResData->Size);
				}
			}
		}
	}
}
void CPE::ShowRelocalInfo()
{
	typedef	struct {
		WORD Offset : 12;  // (1) 大小为12Bit的重定位偏移
		WORD Type : 4;   // (2) 大小为4Bit的重定位信息类型值
	}*PTYPEOFFSET;	    // 这个结构体是A1Pass总结的


	// 获取重定位表
	PIMAGE_BASE_RELOCATION pRelocal = GetRelocalDirectory();

	// 遍历所有的重定位数据
	while (pRelocal->SizeOfBlock)
	{
		// 获取重定位项个数
		DWORD dwCount = (pRelocal->SizeOfBlock - 8) / 2;

		//（0x1000字节中的需要重定位数据的偏移）
		printf("virtualaddress: %08x\n", pRelocal->VirtualAddress);
		// 获取重定位块
		PTYPEOFFSET pRelocalBlock = (PTYPEOFFSET)(pRelocal + 1);
		// 遍历重定位项数据
		for (int i = 0; i < dwCount; i++)
		{
			// （x32位PE文件）类型为0x03	表示需要重定位的数据  
			// （x64位PE文件）类型为0x10	表示需要重定位的数据  
			if (pRelocalBlock[i].Type == 3)
			{
				// 要修复数据的RVA
				DWORD Rva = pRelocalBlock[i].Offset + pRelocal->VirtualAddress;
				printf("	RVA:%08X\n", Rva);
				printf("	FOA:%08X\n", RvaToFoa(Rva));
				printf("\n");
				// 修复重定位为0x00800000
				*(DWORD*)(RvaToFoa(Rva) + m_buff) =
					*(DWORD*)(RvaToFoa(Rva) + m_buff) - 0x00400000 + 0x00800000;
			}
		}

		// 遍历下一重定位表
		pRelocal =
			(PIMAGE_BASE_RELOCATION)((DWORD)pRelocal + pRelocal->SizeOfBlock);
	}

	// 保存文件 - 修复重定位
	//HANDLE hFile = CreateFile(
	//	L"1.exe",		//文件路径
	//	GENERIC_WRITE,	//打开方式（读）
	//	FILE_SHARE_READ,//共享方式
	//	NULL,			//安全属性
	//	CREATE_NEW,  //打开的文件必须存在
	//	FILE_ATTRIBUTE_NORMAL, //文件属性
	//	NULL
	//);
	//DWORD dwWriteSize;
	//WriteFile(hFile, m_buff, m_size, &dwWriteSize, 0);
	//CloseHandle(hFile);

}
