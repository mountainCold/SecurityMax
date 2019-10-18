// hook   MessageBoxW
#include <windows.h>

void OnInlineHook();
void UnInlineHook();

// 保存hook地址前5个字节
BYTE g_oldcode[5] = {};

// 保存hook的5个指令  jmp 0xxxxxxxx
BYTE g_newcode[5] = { 0xE9 };

// hook后的新代码
BOOL WINAPI  MyTerminateProcess(
	_In_ HANDLE hProcess,
	_In_ UINT uExitCode)
{
	// 卸载钩子
	UnInlineHook();

	//调用函数
	MessageBox(NULL, L"此进程已被保护！", 0, 0);

	//设置钩子
	OnInlineHook();

	return FALSE;
}


// 开启InlineHook
void OnInlineHook()
{
	// 1.获取函数地址
	HMODULE hModule = LoadLibraryA("C:\\WINDOWS\\system32\\Kernel32.dll");
	LPVOID lpMsgAddr = GetProcAddress(hModule, "TerminateProcess");

	// 2. 保存原始指令5个字节
	memcpy(g_oldcode, lpMsgAddr, 5);

	// 3. 计算跳转偏移，构建跳转 newcode[5]
	// 跳转偏移  = 目标地址 - 指令所在- 指令长度
	DWORD dwOffset = (DWORD)MyTerminateProcess - (DWORD)lpMsgAddr - 5;
	*(DWORD*)(g_newcode + 1) = dwOffset;

	// 4. 写入跳转偏移
	// 修改目标页属性
	DWORD dwOldProtect;
	VirtualProtect(lpMsgAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// 修改MessageBoxW指令前5个字节
	memcpy(lpMsgAddr, g_newcode, 5);
	// 恢复页属性
	VirtualProtect(lpMsgAddr, 5, dwOldProtect, &dwOldProtect);
}

// 关闭InlineHook
void UnInlineHook()
{
	// 还原MessageBoxW前5个字节
		// 1.获取函数地址
	HMODULE hModule = LoadLibraryA("C:\\WINDOWS\\system32\\Kernel32.dll");
	LPVOID lpMsgAddr = GetProcAddress(hModule, "TerminateProcess");

	// 2.还原指令前5字节
	// 修改目标页属性
	DWORD dwOldProtect;
	VirtualProtect(lpMsgAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// 修改MessageBoxW指令前5个字节
	memcpy(lpMsgAddr, g_oldcode, 5);
	// 恢复页属性
	VirtualProtect(lpMsgAddr, 5, dwOldProtect, &dwOldProtect);

}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		MessageBox(0, L"已经注入", L"标题", 0);
		OnInlineHook();
		break;
	}
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		UnInlineHook();
		break;
	}
	//  dllmain必须返回true,才可以加载
	return TRUE;
}