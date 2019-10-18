#include"APIHOOK.h"
#include<ImageHlp.h>  //Ϊ�˵���ImageDirectoryEntryToData����
#include <TlHelp32.h>
#pragma comment(lib, "ImageHlp")



//CAPIHook���������ͷָ��

CAPIHook* CAPIHook::sm_pHeader = NULL;



//////////////////////////////////////////////////////////////////////

// Construction/Destruction

//////////////////////////////////////////////////////////////////////



CAPIHook::CAPIHook(LPSTR pszModName, LPSTR pszFuncName,

	PROC pfnHook, BOOL bExcludeAPIHookMod/* =TRUE */)

{

	//�������Hook��������Ϣ

	m_bExcludeAPIHookMod = bExcludeAPIHookMod;

	m_pszModName = pszModName;

	m_pszFuncName = pszFuncName;

	m_pfnHook = pfnHook;

	m_pfnOrig = ::GetProcAddress(::GetModuleHandleA(pszModName), pszFuncName);



	//���˶�����ӵ�������

	m_pNext = sm_pHeader;

	sm_pHeader = this;

	//�����е�ǰ�Ѽ��ص�ģ����HOOK�������

	ReplaceIATEntryInAllMods(m_pszModName, m_pfnOrig, m_pfnHook,

		bExcludeAPIHookMod);

}



CAPIHook::~CAPIHook()

{

	//ȡ��������ģ���к�����HOOK

	ReplaceIATEntryInAllMods(m_pszModName, m_pfnHook, m_pfnOrig,

		m_bExcludeAPIHookMod);

	CAPIHook* p = sm_pHeader;

	//���������Ƴ��˶���

	if (p == this)

	{

		sm_pHeader = p->m_pNext;

	}

	else

	{

		while (p != NULL)

		{

			if (p->m_pNext == this)

			{

				p->m_pNext = this->m_pNext;

				break;

			}

			p = p->m_pNext;
		}

	}
}



void CAPIHook::ReplaceIATEntryInOneMod(LPSTR pszExportMod, PROC pfnCurrent,

	PROC pfnNew, HMODULE hModCaller)

{

	//ȡ��ģ��ĵ����(import descriptor)���׵�ַ

	//ImageDirectoryEntryToData�������Է��ص�����ַ

	ULONG ulSize;

	PIMAGE_IMPORT_DESCRIPTOR pImportDesc =

		(PIMAGE_IMPORT_DESCRIPTOR)::ImageDirectoryEntryToData(hModCaller, TRUE,

			IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

	if (pImportDesc == NULL)      //���ģ��û�е������

		return;

	//���Ұ���pszExportModģ���к���������Ϣ�ĵ������

	while (pImportDesc->Name != 0)

	{

		LPSTR pszMod = (LPSTR)((DWORD)hModCaller + pImportDesc->Name);

		if (lstrcmpiA(pszMod,pszExportMod) == 0)      //�ҵ�

		{

			break;

		}

		pImportDesc++;

	}



	if (pImportDesc->Name == 0)    //hModCallerģ��û�д�pszExportModģ�鵼���κκ���

	{

		return;

	}



	//ȡ�õ����ߵĵ����ַ��(import address table, IAT)

	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)(pImportDesc->FirstThunk +

		(DWORD)hModCaller);

	//��������ҪHOOK�ĺ����������ĵ�ַ���º����ĵ�ַ�滻��

	while (pThunk->u1.Function)

	{

		//lpAddrָ����ڴ汣���˺����ĵ�ַ

		PDWORD lpAddr = (PDWORD) & (pThunk->u1.Function);

		if (*lpAddr == (DWORD)pfnCurrent)

		{

			//�޸�ҳ�ı�������

			DWORD dwOldProtect;

			MEMORY_BASIC_INFORMATION mbi;

			::VirtualQuery(lpAddr, &mbi, sizeof(mbi));

			::VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);



			//�޸��ڴ��ַ���൱��"lpAddr = (DWORD)pfnNew;"

			::WriteProcessMemory(::GetCurrentProcess(), lpAddr,

				&pfnNew, sizeof(DWORD), NULL);

			::VirtualProtect(lpAddr, sizeof(DWORD), dwOldProtect, 0);

			break;

		}

		pThunk++;

	}

}



void CAPIHook::ReplaceIATEntryInAllMods(LPSTR pszExportMod, PROC pfnCurrent,

	PROC pfnNew, BOOL bExcludeAPIHookMod)

{

	//ȡ�õ�ǰģ��ľ��

	HMODULE hModThis = NULL;

	if (bExcludeAPIHookMod)

	{

		MEMORY_BASIC_INFORMATION mbi;

		if (::VirtualQuery(ReplaceIATEntryInAllMods, &mbi, sizeof(mbi)) != 0)

			hModThis = (HMODULE)mbi.AllocationBase;

	}



	//ȡ�ñ����̵�ģ���б�

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ::GetCurrentProcessId());
	//��������ģ�飬�ֱ�����ǵ���ReplaceIATEntryInOnMod�������޸ĵ����ַ��
	MODULEENTRY32 me = { sizeof(MODULEENTRY32) };

	BOOL bOK = ::Module32First(hSnap, &me);

	while (bOK)
	{
		//ע�⣬���ǲ�HOOK��ǰģ��ĺ���
		if (me.hModule != hModThis)
		{

			ReplaceIATEntryInOneMod(pszExportMod, pfnCurrent, pfnNew, me.hModule);

		}



		bOK = ::Module32Next(hSnap, &me);

	}

	::CloseHandle(hSnap);

}



//�ҹ�LoadLibrary��GetProcAddress�������Ա�����Щ�����������Ժ󣬹ҹ��ĺ���Ҳ�ܹ�����ȷ�Ĵ���

CAPIHook CAPIHook::sm_LoadLibraryA(CCTOC("Kernel32.dll"), CCTOC("LoadLibraryA"),

(PROC)CAPIHook::LoadLibraryA, TRUE);

CAPIHook CAPIHook::sm_LoadLibraryW(CCTOC("Kernel32.dll"), CCTOC("LoadLibraryW"),

(PROC)CAPIHook::LoadLibraryW, TRUE);

CAPIHook CAPIHook::sm_LoadLibraryExA(CCTOC("Kernel32.dll"), CCTOC("LoadLibraryA"),

(PROC)CAPIHook::LoadLibraryExA, TRUE);

CAPIHook CAPIHook::sm_LoadLibraryExW(CCTOC("Kernel32.dll"), CCTOC("LoadLibraryW"),

(PROC)CAPIHook::LoadLibraryExW, TRUE);

CAPIHook CAPIHook::sm_GetProcAddress(CCTOC("Kernel32.dll"), CCTOC("GetProcAddress"),
(PROC)CAPIHook::GetProcAddress, TRUE);



void WINAPI CAPIHook::HookNewlyLoadedModule(HMODULE hModule, DWORD dwFlags)

{

	//���һ���µ�ģ�鱻���أ��ҹ���CAPIHook����Ҫ���API����

	if ((hModule != NULL) && ((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0))

	{

		CAPIHook* p = sm_pHeader;

		while (p != NULL)

		{

			ReplaceIATEntryInOneMod(p->m_pszModName, p->m_pfnOrig,

				p->m_pfnHook, hModule);

			p = p->m_pNext;

		}

	}

}



HMODULE WINAPI CAPIHook::LoadLibraryA(PCSTR pszModulePath)

{

	HMODULE hModule = ::LoadLibraryA(pszModulePath);

	HookNewlyLoadedModule(hModule, 0);

	return hModule;

}



HMODULE WINAPI CAPIHook::LoadLibraryW(PCWSTR pszModulePath)

{

	HMODULE hModule = ::LoadLibraryW(pszModulePath);

	HookNewlyLoadedModule(hModule, 0);

	return hModule;

}



HMODULE WINAPI CAPIHook::LoadLibraryExA(PCSTR pszModulePath, HANDLE hFile, DWORD dwFlags)

{

	HMODULE hModule = ::LoadLibraryExA(pszModulePath, hFile, dwFlags);

	HookNewlyLoadedModule(hModule, dwFlags);

	return hModule;

}



HMODULE WINAPI CAPIHook::LoadLibraryExW(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags)

{

	HMODULE hModule = ::LoadLibraryExW(pszModulePath, hFile, dwFlags);

	HookNewlyLoadedModule(hModule, dwFlags);

	return hModule;

}



FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hModule, PCSTR pszProcName)

{

	//�õ���������ʵ��ַ

	FARPROC pfn = ::GetProcAddress(hModule, pszProcName);

	//�����Ƿ�����ҪHOOK�ĺ���

	CAPIHook* p = sm_pHeader;

	while (p != NULL)

	{

		if (p->m_pfnOrig == pfn)

		{

			pfn = p->m_pfnHook;

			break;

		}

		p = p->m_pNext;

	}

	return pfn;

}
