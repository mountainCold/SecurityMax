#include "pch.h"
#include "CPE.h"
#include <stdio.h>
#include <ctime>

bool CPE::InitPe(TCHAR* path)
{
	m_path = path;
	HANDLE hFile = CreateFile(
		path,		//�ļ�·��
		GENERIC_READ,//�򿪷�ʽ ��
		FILE_SHARE_READ,//����ʽ
		NULL,			//��ȫ����
		OPEN_EXISTING,	//�ļ������Ѵ���
		FILE_ATTRIBUTE_NORMAL,//�ļ�����
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
		// RVA > ����������ڵ�RVA && 
		// RVA<= �������ڵ�RVA+�������ļ��еĴ�С
		if ((dwRva >= pSection[i].VirtualAddress) &&
			dwRva < (pSection[i].VirtualAddress + pSection[i].SizeOfRawData))
		{
			// ���㹫ʽ  offset = RVA - �������ڵ�RVA + �������ļ��е�ƫ��
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
	// ������ĵ�ַ
	PIMAGE_EXPORT_DIRECTORY pExport =
		(PIMAGE_EXPORT_DIRECTORY)(dwOffset + (DWORD)m_buff);
	return pExport;
}
DWORD CPE::GetExprotFOA()
{
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	//��ȡ�������RVA
	DWORD ExportRva = pNt->OptionalHeader.DataDirectory[0].VirtualAddress;
	//תFOA
	DWORD dwOffset = RvaToFoa(ExportRva);
	return dwOffset;
}
PIMAGE_IMPORT_DESCRIPTOR CPE::GetImportDirectory()
{
	// ͨ������Ŀ¼��� 1 ��
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	// ��ȡ������RVA
	DWORD dwImportRva = pNt->OptionalHeader.DataDirectory[1].VirtualAddress;
	// תFOA
	DWORD dwOffset = RvaToFoa(dwImportRva);
	// ������ĵ�ַ
	PIMAGE_IMPORT_DESCRIPTOR pImport =
		(PIMAGE_IMPORT_DESCRIPTOR)(dwOffset + (DWORD)m_buff);

	return pImport;

}
PIMAGE_RESOURCE_DIRECTORY CPE::GetResourceDirectory()
{
	// ��ȡ����Ŀ¼��� 2��
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[2].VirtualAddress;

	// ��ȡ��Դ��ṹ��
	PIMAGE_RESOURCE_DIRECTORY pRes =
		(PIMAGE_RESOURCE_DIRECTORY)(RvaToFoa(dwRva) + m_buff);

	return pRes;
}
PIMAGE_TLS_DIRECTORY32 CPE::GetTLSDirectory()
{
	// ��ȡ����Ŀ¼��� 9��
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[9].VirtualAddress;

	// ��ȡTLS��ṹ��
	PIMAGE_TLS_DIRECTORY32 pRes =
		(PIMAGE_TLS_DIRECTORY32)(RvaToFoa(dwRva) + m_buff);

	return pRes;
}
PIMAGE_BASE_RELOCATION CPE::GetRelocalDirectory()
{
	// ��ȡ����Ŀ¼��� 5��
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[5].VirtualAddress;

	// ��ȡ�ض�λ��ṹ��
	PIMAGE_BASE_RELOCATION pRelocal =
		(PIMAGE_BASE_RELOCATION)(RvaToFoa(dwRva) + m_buff);

	return pRelocal;
}

PIMAGE_DELAYLOAD_DESCRIPTOR CPE::GetDelayloadDirectory()
{
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[13].VirtualAddress;

	// ��ȡ�ض�λ��ṹ��
	PIMAGE_DELAYLOAD_DESCRIPTOR pDelayload =
		(PIMAGE_DELAYLOAD_DESCRIPTOR)(RvaToFoa(dwRva) + m_buff);
	return pDelayload;
}

void CPE::ShowFileHeader()
{
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	printf("�ļ�����ƽ̨: %08x\n", pNt->FileHeader.Machine);
	printf("����    ����: %08x\n", pNt->FileHeader.NumberOfSections);
	struct tm* p = new tm();
	time_t* t = (time_t*)& pNt->FileHeader.TimeDateStamp;
	localtime_s(p, t);
	printf("�ļ�����ʱ�䣺%04d/%02d/%02d ", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
	printf("%02d:%02d:%02d\n", p->tm_hour, p->tm_min, p->tm_sec);
}

void CPE::ShowTLSInfo()
{
	PIMAGE_TLS_DIRECTORY32 pTls = GetTLSDirectory();
	printf("Դ������ʼλ��VA��%08x\n", pTls->StartAddressOfRawData);
	printf("Դ���ݽ���λ��VA��%08x\n", pTls->EndAddressOfRawData);
	printf("�ص�������λ��VA��%08x\n", pTls->AddressOfCallBacks);
}
void CPE::ShowOptionHeader()
{
	PIMAGE_NT_HEADERS pNt = GetNtHeader();
	//��ʾ ��չͷ�е�����
	printf("OEP: %08x\nImageBase %08x\n",
		pNt->OptionalHeader.AddressOfEntryPoint,
		pNt->OptionalHeader.ImageBase
	);
	//��ʽ����Ŀ¼��
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
	// �������α���Ϣ
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
	// ������������Ϣ
	// ����dll����  RVA
	char* pDllName = (char*)(RvaToFoa(pExport->Name) + m_buff);
	printf("%s\n", pDllName);
	DWORD* pEAT = (DWORD*)(RvaToFoa(pExport->AddressOfFunctions) + m_buff);
	// �������Ʊ�
	DWORD* pENT =
		(DWORD*)(RvaToFoa(pExport->AddressOfNames) + m_buff);
	// ������ű� Ԫ����word
	WORD* pEOT =
		(WORD*)(RvaToFoa(pExport->AddressOfNameOrdinals) + m_buff);
	// ������ַ�����
	DWORD dwAddrCount = pExport->NumberOfFunctions;
	// �������Ʊ����
	DWORD dwNameCount = pExport->NumberOfNames;

	// ������ַ��
	for (int i = 0; i < dwAddrCount; i++)
	{
		//����������
		printf("��� %d ", i + pExport->Base);

		// ������ű�
		for (int j = 0; j < dwNameCount; j++)
		{
			// ��ű����е�ַ������ ����������������ֵģ�
			if (i == pEOT[j]) {
				// �������������� 
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
			// �жϵ���ĺ��������Ƶ��뻹����ŵ���
			// �ж����λ�Ƿ�λ1 �����Ϊ1��ô��Ҫ����
			if (pIAT->u1.Ordinal & 0x80000000)
			{
				printf("��� %d\n", pIAT->u1.Function & 0x7FFFFFFF);
			}
			else {
				// ����ĺ���������
				PIMAGE_IMPORT_BY_NAME pName =
					(PIMAGE_IMPORT_BY_NAME)(RvaToFoa(pIAT->u1.AddressOfData) + (DWORD)m_buff);
				printf("��� %d  ��������%s\n", pName->Hint, pName->Name);

			}
			// ������һ������
			pIAT++;
		}
		// ������һ�������
		pImport++;
	}
}
const WCHAR* RES[20] = {
	L"���",
	L"λͼ",
	L"ͼ��",
	L"�˵�",
	L"�Ի���",
	L"�ַ����б�",
	L"����Ŀ¼",
	L"����",
	L"��ݼ�",
	L"�Ǹ�ʽ����Դ",
	L"��Ϣ�б�",
	L"���ָ������",
	L"NULL",
	L"ͼ����",
	L"NULL",
	L"�汾��Ϣ",
};
void CPE::ShowResourceInfo()
{
	// ��һ�㣺��Դ��������ʲô��ͼ�꣬λͼ���˵�....��
	// ��ȡ��ԴĿ¼
	PIMAGE_RESOURCE_DIRECTORY pResDirOne = GetResourceDirectory();

	// ��ȡ��ԴĿ¼��
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResOneEntry =
		(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirOne + 1);

	//��ԴĿ¼����
	DWORD dwOneCount = pResDirOne->NumberOfIdEntries + pResDirOne->NumberOfNamedEntries;


	//������һ����Դ��Ϣ
	for (int i = 0; i < dwOneCount; i++)
	{
		// �ж���Դ��ʲô����
		// ��Դ���ַ�����Ϊ��ʶ
		if (pResOneEntry[i].NameIsString)
		{
			// ���NameIsString == 1 ��ô NameOffset��Ч
			PIMAGE_RESOURCE_DIR_STRING_U pName =
				(PIMAGE_RESOURCE_DIR_STRING_U)(pResOneEntry[i].NameOffset + (DWORD)pResDirOne);
			// �����Դ������
			WCHAR wResName[100];
			wcsncpy_s(wResName, pName->NameString, pName->Length);
			wprintf(L"%s\n", wResName);
		}
		// ��Դ��ID��Ϊ��ʶ
		else {
			// ϵͳ�ṩ����Դ����
			if (pResOneEntry[i].Id < 16)
			{
				wprintf(L"%s\n", RES[pResOneEntry[i].Id]);
			}
			else {
				wprintf(L"%08x\n", pResOneEntry[i].Id);
			}

		}


		// �ڶ��Σ���Դ�Ľ�ʲô���� ��1.png, 2.png��
		// �Ƿ��еڶ���Ŀ¼
		if (pResOneEntry[i].DataIsDirectory) {

			// �ҵ��ڶ���Ŀ¼��
			PIMAGE_RESOURCE_DIRECTORY pResDirTwo =
				(PIMAGE_RESOURCE_DIRECTORY)(pResOneEntry[i].OffsetToDirectory + (DWORD)pResDirOne);

			// �ҵ��ڶ���Ŀ¼��
			PIMAGE_RESOURCE_DIRECTORY_ENTRY pResTwoEntry =
				(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirTwo + 1);

			// ��ȡ������Դ�ж��ٸ�
			DWORD ResTowCount = pResDirTwo->NumberOfIdEntries + pResDirTwo->NumberOfNamedEntries;
			for (int j = 0; j < ResTowCount; j++)
			{
				// ��Դ���ַ�����Ϊ��ʶ
				if (pResTwoEntry[j].NameIsString)
				{
					// ���NameIsString == 1 ��ô NameOffset��Ч
					PIMAGE_RESOURCE_DIR_STRING_U pName =
						(PIMAGE_RESOURCE_DIR_STRING_U)(pResTwoEntry[j].NameOffset + (DWORD)pResDirOne);
					// �����Դ������
					WCHAR wResName[100];
					wcsncpy_s(wResName, pName->NameString, pName->Length);
					wprintf(L"		%s\n", wResName);
				}
				else {
					printf("		%08x\n", pResTwoEntry[j].Id);
				}

				// �����㣺��Դ�����ԣ���Դ����Ϣ����С���ļ���λ�ã�
				if (pResTwoEntry[j].DataIsDirectory)
				{
					// ��ȡ������Ŀ¼��
					PIMAGE_RESOURCE_DIRECTORY pResDirThree =
						(PIMAGE_RESOURCE_DIRECTORY)(pResTwoEntry[j].OffsetToDirectory + (DWORD)pResDirOne);
					// ��ȡ������Ŀ¼��
					PIMAGE_RESOURCE_DIRECTORY_ENTRY pResThreeEntry =
						(PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResDirThree + 1);

					// ��ȡ��Դ������Ϣ
					PIMAGE_RESOURCE_DATA_ENTRY pResData =
						(PIMAGE_RESOURCE_DATA_ENTRY)(pResThreeEntry->OffsetToData + (DWORD)pResDirOne);

					printf("			��Դλ��RVA�� %08X\n", pResData->OffsetToData);
					printf("			��Դλ��Size�� %08X\n", pResData->Size);
				}
			}
		}
	}
}
void CPE::ShowRelocalInfo()
{
	typedef	struct {
		WORD Offset : 12;  // (1) ��СΪ12Bit���ض�λƫ��
		WORD Type : 4;   // (2) ��СΪ4Bit���ض�λ��Ϣ����ֵ
	}*PTYPEOFFSET;	    // ����ṹ����A1Pass�ܽ��


	// ��ȡ�ض�λ��
	PIMAGE_BASE_RELOCATION pRelocal = GetRelocalDirectory();

	// �������е��ض�λ����
	while (pRelocal->SizeOfBlock)
	{
		// ��ȡ�ض�λ�����
		DWORD dwCount = (pRelocal->SizeOfBlock - 8) / 2;

		//��0x1000�ֽ��е���Ҫ�ض�λ���ݵ�ƫ�ƣ�
		printf("virtualaddress: %08x\n", pRelocal->VirtualAddress);
		// ��ȡ�ض�λ��
		PTYPEOFFSET pRelocalBlock = (PTYPEOFFSET)(pRelocal + 1);
		// �����ض�λ������
		for (int i = 0; i < dwCount; i++)
		{
			// ��x32λPE�ļ�������Ϊ0x03	��ʾ��Ҫ�ض�λ������  
			// ��x64λPE�ļ�������Ϊ0x10	��ʾ��Ҫ�ض�λ������  
			if (pRelocalBlock[i].Type == 3)
			{
				// Ҫ�޸����ݵ�RVA
				DWORD Rva = pRelocalBlock[i].Offset + pRelocal->VirtualAddress;
				printf("	RVA:%08X\n", Rva);
				printf("	FOA:%08X\n", RvaToFoa(Rva));
				printf("\n");
				// �޸��ض�λΪ0x00800000
				*(DWORD*)(RvaToFoa(Rva) + m_buff) =
					*(DWORD*)(RvaToFoa(Rva) + m_buff) - 0x00400000 + 0x00800000;
			}
		}

		// ������һ�ض�λ��
		pRelocal =
			(PIMAGE_BASE_RELOCATION)((DWORD)pRelocal + pRelocal->SizeOfBlock);
	}

	// �����ļ� - �޸��ض�λ
	//HANDLE hFile = CreateFile(
	//	L"1.exe",		//�ļ�·��
	//	GENERIC_WRITE,	//�򿪷�ʽ������
	//	FILE_SHARE_READ,//����ʽ
	//	NULL,			//��ȫ����
	//	CREATE_NEW,  //�򿪵��ļ��������
	//	FILE_ATTRIBUTE_NORMAL, //�ļ�����
	//	NULL
	//);
	//DWORD dwWriteSize;
	//WriteFile(hFile, m_buff, m_size, &dwWriteSize, 0);
	//CloseHandle(hFile);

}
