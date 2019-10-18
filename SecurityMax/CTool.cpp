#include "pch.h"
#include "CTool.h"
#include <Psapi.h>
#include <vector>
BOOL EnumProcess(std::vector<PROCESSINFO>* proclist)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE) {
		return false;
	}
	PROCESSENTRY32 stcPe = { sizeof(stcPe) };
	//��ȡͷ�ڵ�
	if (Process32First(hSnap, &stcPe))
	{
		//ѭ������
		do
		{
			PROCESSINFO pi = *(PROCESSINFO*)& stcPe;

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, stcPe.th32ParentProcessID);
			if (hProcess == NULL)
			{
				pi.dwMemoryUsage = 0;
			}
			else
			{
				PROCESS_MEMORY_COUNTERS pmc = { sizeof(PROCESS_MEMORY_COUNTERS) };
				GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
				pi.dwMemoryUsage = pmc.WorkingSetSize;
				CloseHandle(hProcess);
			}
			proclist->push_back(pi);
		} while (Process32Next(hSnap, &stcPe));
	}
	CloseHandle(hSnap);
	return true;
}
BOOL EnumThread(std::vector<THREADENTRY32>* ThreadList)
{
	//1 ��������
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); //0��ʾ��ǰ����
	// �״α���
	THREADENTRY32 thread32 = { sizeof(thread32) };
	if (Thread32First(hSnap, &thread32))
	{
		do
		{
			ThreadList->push_back(thread32);
		} while (Thread32Next(hSnap, &thread32));

	}
	CloseHandle(hSnap);
	return true;
}
BOOL EnumModul(std::vector<MYMODULEENTRY32>* moduleList)
{
	std::vector<PROCESSINFO> proclist;
	EnumProcess(&proclist);
	for (size_t i = 0; i < proclist.size(); i++)
	{
		MYMODULEENTRY32 myInfo;
		DWORD dwPid = proclist[i].th32ProcessID;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
		if (hSnap == INVALID_HANDLE_VALUE) continue;
		MODULEENTRY32 mInfo = { sizeof(MODULEENTRY32) };
		if (!Module32First(hSnap, &mInfo))continue;
		do
		{
			myInfo.dwPid = dwPid;
			myInfo.modul = mInfo;
			moduleList->push_back(myInfo);
		} while (Module32Next(hSnap, &mInfo));
	}
	
	return true;
}
BOOL EnumFile(CString path)
{

	return 0;
}
BOOL GetPEHead(TCHAR* path, PEINFO* peHead)
{
	CPE pe;
	pe.InitPe(path);
	PIMAGE_NT_HEADERS pNT = pe.GetNtHeader();
	peHead->m_buff = (DWORD*)pe.m_buff;
	//��ȡͷ��Ϣ
	peHead->oep = pNT->OptionalHeader.AddressOfEntryPoint;
	peHead->BaseOfData = pNT->OptionalHeader.BaseOfData;
	peHead->BaseOfCode = pNT->OptionalHeader.BaseOfCode;
	peHead->Characteristics = pNT->FileHeader.Characteristics;
	peHead->CheckSum = pNT->OptionalHeader.CheckSum;
	peHead->FileAlignment = pNT->OptionalHeader.FileAlignment;
	peHead->ImageBase = pNT->OptionalHeader.ImageBase;
	peHead->Magic = pNT->OptionalHeader.Magic;
	peHead->NumberOfRvaAndSizes = pNT->OptionalHeader.NumberOfRvaAndSizes;
	peHead->NumberOfSections = pNT->FileHeader.NumberOfSections;
	peHead->SectionAlignment = pNT->OptionalHeader.SectionAlignment;
	peHead->SizeOfHeaders = pNT->OptionalHeader.SizeOfHeaders;
	peHead->SizeofImage = pNT->OptionalHeader.SizeOfImage;
	peHead->SizeOfOptionalHeader = pNT->FileHeader.SizeOfOptionalHeader;
	peHead->Subsystemm = pNT->OptionalHeader.Subsystem;
	peHead->TimeDateStamp = pNT->FileHeader.TimeDateStamp;
	//��ȡ����Ŀ¼����Ϣ
	peHead->DataDirectory = pNT->OptionalHeader.DataDirectory;
	peHead->sectionHead = pe.GetSectionHeader();
	//��ȡ�������Ϣ
	peHead->exportT = pe.GetExprotDirectory();
	peHead->exFoa = pe.GetExprotFOA();
	peHead->exname = pe.RvaToFoa(pe.GetExprotDirectory()->Name)+(DWORD)pe.m_buff;
	peHead->pEAT = pe.GetExportEAT();
	peHead->pENT = pe.GetExportENT();
	peHead->pEOT = pe.GetExportEOT();
	peHead->importT = pe.GetImportDirectory();
	peHead->TLS = pe.GetTLSDirectory();
	peHead->resource = pe.GetResourceDirectory();
	peHead->relocation=pe.GetRelocalDirectory();
	peHead->delayload = pe.GetDelayloadDirectory();
	return TRUE;
}
DWORD RvaToFoa(PEINFO* peInfo, DWORD dwRva)
{
	PIMAGE_SECTION_HEADER pSection =peInfo->sectionHead;
	DWORD dwSectionCount = peInfo->NumberOfSections;
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
DWORD GetSection(PEINFO* peInfo, DWORD dwRva)
{
	PIMAGE_SECTION_HEADER pSection = peInfo->sectionHead;
	DWORD dwSectionCount = peInfo->NumberOfSections;
	DWORD Offset = 0;
	for (size_t i = 0; i < dwSectionCount; i++)
	{
		// RVA > ����������ڵ�RVA && 
		// RVA<= �������ڵ�RVA+�������ļ��еĴ�С
		if ((dwRva >= pSection[i].VirtualAddress) &&
			dwRva < (pSection[i].VirtualAddress + pSection[i].SizeOfRawData))
		{
			// ���㹫ʽ  offset = RVA - �������ڵ�RVA + �������ļ��е�ƫ��
			Offset =  i;
		}
	}
	return Offset;
}
CStringW CATOCS(CStringA str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len];
	memset(wstr, 0, len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
	return wstr;
}
CStringA CWTOCA(CStringW wstr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(wstr), -1, NULL, 0, NULL, NULL);
	char* str = new char[len];
	memset(str, 0, len * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(wstr), -1, str, len, NULL, NULL);
	return str;
}

BOOL CALLBACK EnumWindowProcM(HWND hWnd, LPARAM lParam)
{
	TCHAR buff[200];
	std::vector<DWORD>* vName = (std::vector<DWORD>*)lParam;
	GetWindowText(hWnd, buff, 200);
	if (/*IsWindowVisible(hWnd)==*/TRUE)
	{
		if (wcslen(buff) != 0)
		{
			vName->push_back((DWORD)hWnd);
		}
	}
	return true;
}
double getProcessMemoryUsage(DWORD dwPid)
{
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		dwPid);
	double usage = 0;
	if (hProc != NULL) {

		PROCESS_MEMORY_COUNTERS pmc = { sizeof(PROCESS_MEMORY_COUNTERS) };;
		GetProcessMemoryInfo(hProc, &pmc, sizeof(pmc));
		usage = pmc.WorkingSetSize;
		CloseHandle(hProc);
	}
	return usage / 1024;
}
double	FILETIME2Double(const _FILETIME& fileTime)
{
	return	double(fileTime.dwHighDateTime * 4.294967296e9) + double(fileTime.dwLowDateTime);
}
#include <Winternl.h>
void getProcessTime(DWORD dwPid, LARGE_INTEGER* kernelTIme, LARGE_INTEGER* userTime)
{
	typedef struct _KERNEL_USERTEXTIMES
	{
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER ExitTime;
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
	} KERNEL_USERTEXTIMES, * PKERNEL_USERTEXTIMES;
	KERNEL_USERTEXTIMES time;
#define ProcessTimes  4

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
	if (hProc == NULL) {
		return;
	}
	decltype(NtQueryInformationProcess)* fnNtQueryInformationProcess;
	fnNtQueryInformationProcess = (decltype(NtQueryInformationProcess)*)
		(GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQueryInformationProcess"));

	DWORD read = 0;
	fnNtQueryInformationProcess(hProc,
		(PROCESSINFOCLASS)ProcessTimes,
		&time,
		sizeof(time),
		&read);
	*kernelTIme = time.KernelTime;
	*userTime = time.UserTime;
	CloseHandle(hProc);
}
double getProcessCpuUsage(DWORD dwPid)
{
	// ��ʽ: (�����ں�ʱ�� + �����û�ʱ��) / (CPU�ں�ʱ�� + CPU�û�ʱ��)

	LARGE_INTEGER kernelTime = { 0 };
	LARGE_INTEGER userTime = { 0 };
	getProcessTime(dwPid, &kernelTime, &userTime);

	//			����ʱ��		�ں�ʱ��		�û�ʱ��
	FILETIME cpuIdleTime = {}, cpuKernelTime = {}, cpuUserTime = {};

	// ��ȡʱ��
	GetSystemTimes((FILETIME*)& cpuIdleTime, (FILETIME*)& cpuKernelTime, (FILETIME*)& cpuUserTime);

	double proc = FILETIME2Double(*(FILETIME*)& kernelTime) + FILETIME2Double(*(FILETIME*)& userTime);
	double cpu = FILETIME2Double(cpuKernelTime) + FILETIME2Double(cpuKernelTime);
	return proc / cpu * 100.0;
}
double getCpuUsage()
{
	//			����ʱ��		�ں�ʱ��		�û�ʱ��
	_FILETIME	idleTime, kernelTime, userTime;
	// ��ȡʱ��
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	// �ȴ�1000����
	HANDLE	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// �ȴ�1000����,ʹ���ں˶���ȴ������ȷ
	WaitForSingleObject(hEvent, 1000);
	CloseHandle(hEvent);

	// ��ȡ�µ�ʱ��
	_FILETIME	newIdleTime, newKernelTime, newUserTime;
	GetSystemTimes(&newIdleTime, &newKernelTime, &newUserTime);
	// ������ʱ��ת��
	double	dOldIdleTime = FILETIME2Double(idleTime);
	double	dNewIdleTime = FILETIME2Double(newIdleTime);
	double	dOldKernelTime = FILETIME2Double(kernelTime);
	double	dNewKernelTime = FILETIME2Double(newKernelTime);
	double	dOldUserTime = FILETIME2Double(userTime);
	double	dNewUserTime = FILETIME2Double(newUserTime);
	// �����ʹ����
	//		ʹ���� = 100.0 - (�¿���ʱ�� - �ɿ���ʱ��) / (���ں�ʱ��-���ں�ʱ��  + ���û�ʱ�� - ���û�ʱ��) *100.0
	return	int(100.0 - (dNewIdleTime - dOldIdleTime) / (dNewKernelTime - dOldKernelTime + dNewUserTime - dOldUserTime) * 100.0);
}
int getCpuUse()
{
	FILETIME idleTime, kernelTime, userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	WaitForSingleObject(hEvent, 1000);

	FILETIME preidleTime, prekernelTime, preuserTime;
	GetSystemTimes(&preidleTime, &prekernelTime, &preuserTime);

	double tidleTime = FILETIME2Double(idleTime);
	double tkernelTime = FILETIME2Double(kernelTime);
	double tuserTime = FILETIME2Double(userTime);
	double tidleTime2 = FILETIME2Double(preidleTime);
	double tkernelTime2 = FILETIME2Double(prekernelTime);
	double tuserTime2 = FILETIME2Double(preuserTime);

	int res = (int)(100 - (tidleTime2 - tidleTime) / (tkernelTime2 - tkernelTime + tuserTime2 - tuserTime) * 100.0);
	return res;
}
//��ѯȨ��
BOOL QueryPrivileges()
{
	// 1. ��ñ����̵�����
	HANDLE hToken = NULL;
	if (!OpenProcessToken(
		GetCurrentProcess(),	// ��ǰ���̾��
		TOKEN_QUERY,			// �������������
		&hToken))				// �������ƾ��
		return false;
	// 2. ��ȡ��������
	TOKEN_ELEVATION_TYPE ElevationType = TokenElevationTypeDefault;
	BOOL                 bIsAdmin = false;							// �Ƿ����Ա
	DWORD                dwSize = 0;								// Ȩ����Ϣ�ṹ���С

	// ��ȡ������Ϣ
	if (GetTokenInformation(
		hToken,							// ���ƾ��
		TokenElevationType,				// ��ȡ������Ϣ����
		&ElevationType,					// ����������Ϣ���Ͷ�Ӧ�Ľṹ��
		sizeof(TOKEN_ELEVATION_TYPE),	// �ṹ���С
		&dwSize))						// ʵ�ʴ�С
	{
		// 2.1 ��������Ա��Ķ�ӦSID
		BYTE adminSID[SECURITY_MAX_SID_SIZE];		//����Ա��SID��ʶ
		dwSize = sizeof(adminSID);					//����Ա��SID��ʶ�Ĵ�С
		// ��ȡ����Ա��SID
		CreateWellKnownSid(
			WinBuiltinAdministratorsSid,			// ��ѯ����
			NULL,									// NULL
			&adminSID,								// ��Ź���Ա��SID������
			&dwSize);								// ��������С


		// 2.2 �жϵ�ǰ���������û���ɫ�Ƿ�Ϊ����Ա
		// ��ǰȨ���������ƣ���Ȩ�����ƣ�
		if (ElevationType == TokenElevationTypeLimited) {

			// a. ��ȡ�������Ƶľ��
			HANDLE hUnfilteredToken = NULL;

			GetTokenInformation(hToken,			//���ƾ��
				TokenLinkedToken,				//����������Ϣ���˻����ƣ�
				(PVOID)& hUnfilteredToken,		//�������Ƶľ��
				sizeof(HANDLE),
				&dwSize);

			// b. ������ԭʼ�������Ƿ��������Ա��SID
			if (!CheckTokenMembership(
				hUnfilteredToken,		// �������ƾ��
				&adminSID,				// ����Ա��SID
				&bIsAdmin))				// �����Ƿ����Ա
			{
				return false;
			}
			CloseHandle(hUnfilteredToken);		//�رվ��
		}
		else {
			// ������������Ȩ��-���߹ر�UAC
			// ��ôֱ�ӻ�ȡ��ǰ�û��Ƿ��ǹ���Ա
			bIsAdmin = IsUserAnAdmin();
		}
		CloseHandle(hToken);
	}
	// 3. �жϾ����Ȩ��״��
	BOOL bFullToken = false;
	// �ж���������
	switch (ElevationType) {
	case TokenElevationTypeDefault: /* Ĭ�ϵ��û���UAC������ */
		if (IsUserAnAdmin())  bFullToken = true; // Ĭ���û��й���ԱȨ��
		else                  bFullToken = false;// Ĭ���û����ǹ���Ա��
		break;
	case TokenElevationTypeFull:    /* �Ѿ��ɹ���߽���Ȩ�� */
		if (IsUserAnAdmin())  bFullToken = true; //��ǰ�Թ���ԱȨ������
		else                  bFullToken = false;//��ǰδ�Թ���ԱȨ������
		break;
	case TokenElevationTypeLimited: /* �����������޵�Ȩ������ */
		if (bIsAdmin)  bFullToken = false;//�û��й���ԱȨ�ޣ�������Ȩ������
		else           bFullToken = false;//�û����ǹ���Ա�飬�ҽ���Ȩ������
	}


	return 0;
}
bool EnableDebugPrivilege()
{
	HANDLE hToken = NULL;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	TOKEN_ELEVATION_TYPE ElevationType;
	DWORD dwSize = 0;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		MessageBoxA(NULL, "��ʧ��", 0, 0);
		return   FALSE;
	}


	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		MessageBoxA(NULL, "��ȡʧ��", 0, 0);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // ѡ���ǹر�

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		MessageBoxA(NULL, "��ȡȨ��ʧ��", 0, 0);
		return false;
	}
	if (GetTokenInformation(hToken, TokenElevationType, &ElevationType, sizeof(TOKEN_ELEVATION_TYPE), &dwSize))
	{
		// ��������������޵�Ȩ������ (TokenElevationTypeLimited) ,
		if (ElevationType == TokenElevationTypeLimited)
		{
			//MessageBox( NULL , L"�������Թ���Ա�������,�����޷���ȡ��ȡϵͳ���ڴ�ʹ����" , 0 , 0 );
			return	FALSE;
		}
	}
	return true;
}
CString SelFilePathSimple()
{
	TCHAR szFolderPath[MAX_PATH] = { 0 };
	CString strFolderPath = _T("");

	BROWSEINFO sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));

	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
		::CoTaskMemFree(lpidlBrowse);
	}
	return strFolderPath;

}

void SaveFile(bingd* bd, int type)
{
	CString path;
	if (type == 0)
	{
		path = L"E:\\15pb\\�׶ζ�\\SecurityMax\\SecurityMax\\data\\������.txt";
	}
	else
	{
		path = L"E:\\15pb\\�׶ζ�\\SecurityMax\\SecurityMax\\data\\������.txt";
	}
	
	HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwSize = 0;
	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile,bd, sizeof(bingd), &dwSize, NULL);
	CloseHandle(hFile);
}
void DeleteFile(std::vector<bingd>* list, int type)
{
	CString path;
	if (type == 0)
	{
		path = L"E:\\15pb\\�׶ζ�\\SecurityMax\\SecurityMax\\data\\������.txt";
	}
	else
	{
		path = L"E:\\15pb\\�׶ζ�\\SecurityMax\\SecurityMax\\data\\������.txt";
	}

	HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwSize = 0;
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	WriteFile(hFile, list, sizeof(bingd), &dwSize, NULL);
	CloseHandle(hFile);
}
void ReadFile(bingd* bd,int off,int type)
{
	CString path;
	if (type == 0)
	{
		path = L"E:\\15pb\\�׶ζ�\\SecurityMax\\SecurityMax\\data\\������.txt";
	}
	else
	{
		path = L"E:\\15pb\\�׶ζ�\\SecurityMax\\SecurityMax\\data\\������.txt";
	}
	HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwSize = 0;
	SetFilePointer(hFile, off, NULL, FILE_BEGIN);
	ReadFile(hFile, bd, sizeof(bingd), &dwSize, NULL);
	if (dwSize==0)
	{
		//*bd->name =  "0" ;
		sprintf_s(bd->md5, 50, "%s", "0");
	}
	CloseHandle(hFile);
}


