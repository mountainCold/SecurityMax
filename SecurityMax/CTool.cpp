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
	//获取头节点
	if (Process32First(hSnap, &stcPe))
	{
		//循环遍历
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
	//1 创建快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); //0表示当前进程
	// 首次遍历
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
	//获取头信息
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
	//获取数据目录表信息
	peHead->DataDirectory = pNT->OptionalHeader.DataDirectory;
	peHead->sectionHead = pe.GetSectionHeader();
	//获取各类表信息
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
DWORD GetSection(PEINFO* peInfo, DWORD dwRva)
{
	PIMAGE_SECTION_HEADER pSection = peInfo->sectionHead;
	DWORD dwSectionCount = peInfo->NumberOfSections;
	DWORD Offset = 0;
	for (size_t i = 0; i < dwSectionCount; i++)
	{
		// RVA > 这个区段所在的RVA && 
		// RVA<= 区段所在的RVA+区段在文件中的大小
		if ((dwRva >= pSection[i].VirtualAddress) &&
			dwRva < (pSection[i].VirtualAddress + pSection[i].SizeOfRawData))
		{
			// 计算公式  offset = RVA - 区段所在的RVA + 区段在文件中的偏移
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
	// 公式: (进程内核时间 + 进程用户时间) / (CPU内核时间 + CPU用户时间)

	LARGE_INTEGER kernelTime = { 0 };
	LARGE_INTEGER userTime = { 0 };
	getProcessTime(dwPid, &kernelTime, &userTime);

	//			空闲时间		内核时间		用户时间
	FILETIME cpuIdleTime = {}, cpuKernelTime = {}, cpuUserTime = {};

	// 获取时间
	GetSystemTimes((FILETIME*)& cpuIdleTime, (FILETIME*)& cpuKernelTime, (FILETIME*)& cpuUserTime);

	double proc = FILETIME2Double(*(FILETIME*)& kernelTime) + FILETIME2Double(*(FILETIME*)& userTime);
	double cpu = FILETIME2Double(cpuKernelTime) + FILETIME2Double(cpuKernelTime);
	return proc / cpu * 100.0;
}
double getCpuUsage()
{
	//			空闲时间		内核时间		用户时间
	_FILETIME	idleTime, kernelTime, userTime;
	// 获取时间
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	// 等待1000毫秒
	HANDLE	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// 等待1000毫秒,使用内核对象等待会更精确
	WaitForSingleObject(hEvent, 1000);
	CloseHandle(hEvent);

	// 获取新的时间
	_FILETIME	newIdleTime, newKernelTime, newUserTime;
	GetSystemTimes(&newIdleTime, &newKernelTime, &newUserTime);
	// 将各个时间转换
	double	dOldIdleTime = FILETIME2Double(idleTime);
	double	dNewIdleTime = FILETIME2Double(newIdleTime);
	double	dOldKernelTime = FILETIME2Double(kernelTime);
	double	dNewKernelTime = FILETIME2Double(newKernelTime);
	double	dOldUserTime = FILETIME2Double(userTime);
	double	dNewUserTime = FILETIME2Double(newUserTime);
	// 计算出使用率
	//		使用率 = 100.0 - (新空闲时间 - 旧空闲时间) / (新内核时间-旧内核时间  + 新用户时间 - 旧用户时间) *100.0
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
//查询权限
BOOL QueryPrivileges()
{
	// 1. 获得本进程的令牌
	HANDLE hToken = NULL;
	if (!OpenProcessToken(
		GetCurrentProcess(),	// 当前进程句柄
		TOKEN_QUERY,			// 打开这个令牌作用
		&hToken))				// 返回令牌句柄
		return false;
	// 2. 获取提升类型
	TOKEN_ELEVATION_TYPE ElevationType = TokenElevationTypeDefault;
	BOOL                 bIsAdmin = false;							// 是否管理员
	DWORD                dwSize = 0;								// 权限信息结构体大小

	// 获取令牌信息
	if (GetTokenInformation(
		hToken,							// 令牌句柄
		TokenElevationType,				// 获取令牌信息类型
		&ElevationType,					// 返回令牌信息类型对应的结构体
		sizeof(TOKEN_ELEVATION_TYPE),	// 结构体大小
		&dwSize))						// 实际大小
	{
		// 2.1 创建管理员组的对应SID
		BYTE adminSID[SECURITY_MAX_SID_SIZE];		//管理员的SID标识
		dwSize = sizeof(adminSID);					//管理员的SID标识的大小
		// 获取管理员的SID
		CreateWellKnownSid(
			WinBuiltinAdministratorsSid,			// 查询类型
			NULL,									// NULL
			&adminSID,								// 存放管理员的SID缓冲区
			&dwSize);								// 缓冲区大小


		// 2.2 判断当前进程运行用户角色是否为管理员
		// 当前权限是滤令牌（低权限令牌）
		if (ElevationType == TokenElevationTypeLimited) {

			// a. 获取连接令牌的句柄
			HANDLE hUnfilteredToken = NULL;

			GetTokenInformation(hToken,			//令牌句柄
				TokenLinkedToken,				//链接令牌信息（账户令牌）
				(PVOID)& hUnfilteredToken,		//链接令牌的句柄
				sizeof(HANDLE),
				&dwSize);

			// b. 检查这个原始的令牌是否包含管理员的SID
			if (!CheckTokenMembership(
				hUnfilteredToken,		// 链接令牌句柄
				&adminSID,				// 管理员的SID
				&bIsAdmin))				// 返回是否管理员
			{
				return false;
			}
			CloseHandle(hUnfilteredToken);		//关闭句柄
		}
		else {
			// 如果令牌是最高权限-或者关闭UAC
			// 那么直接获取当前用户是否是管理员
			bIsAdmin = IsUserAnAdmin();
		}
		CloseHandle(hToken);
	}
	// 3. 判断具体的权限状况
	BOOL bFullToken = false;
	// 判断提升类型
	switch (ElevationType) {
	case TokenElevationTypeDefault: /* 默认的用户或UAC被禁用 */
		if (IsUserAnAdmin())  bFullToken = true; // 默认用户有管理员权限
		else                  bFullToken = false;// 默认用户不是管理员组
		break;
	case TokenElevationTypeFull:    /* 已经成功提高进程权限 */
		if (IsUserAnAdmin())  bFullToken = true; //当前以管理员权限运行
		else                  bFullToken = false;//当前未以管理员权限运行
		break;
	case TokenElevationTypeLimited: /* 进程在以有限的权限运行 */
		if (bIsAdmin)  bFullToken = false;//用户有管理员权限，但进程权限有限
		else           bFullToken = false;//用户不是管理员组，且进程权限有限
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
		MessageBoxA(NULL, "打开失败", 0, 0);
		return   FALSE;
	}


	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		MessageBoxA(NULL, "获取失败", 0, 0);
		return false;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // 选择还是关闭

	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		CloseHandle(hToken);
		MessageBoxA(NULL, "获取权限失败", 0, 0);
		return false;
	}
	if (GetTokenInformation(hToken, TokenElevationType, &ElevationType, sizeof(TOKEN_ELEVATION_TYPE), &dwSize))
	{
		// 如果令牌是以受限的权限运行 (TokenElevationTypeLimited) ,
		if (ElevationType == TokenElevationTypeLimited)
		{
			//MessageBox( NULL , L"您必须以管理员身份运行,否则无法获取获取系统的内存使用率" , 0 , 0 );
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
		path = L"E:\\15pb\\阶段二\\SecurityMax\\SecurityMax\\data\\病毒库.txt";
	}
	else
	{
		path = L"E:\\15pb\\阶段二\\SecurityMax\\SecurityMax\\data\\白名单.txt";
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
		path = L"E:\\15pb\\阶段二\\SecurityMax\\SecurityMax\\data\\病毒库.txt";
	}
	else
	{
		path = L"E:\\15pb\\阶段二\\SecurityMax\\SecurityMax\\data\\白名单.txt";
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
		path = L"E:\\15pb\\阶段二\\SecurityMax\\SecurityMax\\data\\病毒库.txt";
	}
	else
	{
		path = L"E:\\15pb\\阶段二\\SecurityMax\\SecurityMax\\data\\白名单.txt";
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


