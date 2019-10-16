#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "CPE.h"
#include <vector>

struct PROCESSINFO : public PROCESSENTRY32
{
	DWORD dwMemoryUsage; //内存使用率
	double dCpuUsage = 0;// cpu 使用率
};
struct MYMODULEENTRY32
{
	DWORD dwPid;
	MODULEENTRY32 modul;
};
struct PEINFO
{
	DWORD* m_buff;
	DWORD oep;		//入口地址
	DWORD ImageBase; //镜像基址
	DWORD SizeofImage; //镜像基址大小
	DWORD BaseOfCode; //代码基址
	DWORD BaseOfData; //数据基址
	DWORD SectionAlignment; //块对齐
	DWORD FileAlignment; //文件块对齐
	WORD Magic;			//标志字
	DWORD Subsystemm;  //子系统
	WORD  NumberOfSections; //区段数量 文件头中
	DWORD TimeDateStamp;	//日期时间标志 文件头中
	DWORD SizeOfHeaders;    //部首大小
	WORD Characteristics;	//特征值	文件头中
	DWORD CheckSum;			//校验和
	WORD SizeOfOptionalHeader;//可选头大小 文件头中
	DWORD NumberOfRvaAndSizes; //数组目录个数
	PIMAGE_SECTION_HEADER sectionHead;//区段表
	PIMAGE_DATA_DIRECTORY DataDirectory;//数据目录表
	PIMAGE_EXPORT_DIRECTORY exportT; //导出表
	DWORD exFoa;					//
	DWORD exname;					//
	DWORD* pEAT;
	DWORD* pENT;
	WORD* pEOT;
	PIMAGE_IMPORT_DESCRIPTOR importT;//导入表
	PIMAGE_RESOURCE_DIRECTORY resource;//资源表
	PIMAGE_BASE_RELOCATION relocation;//重定位表
	PIMAGE_TLS_DIRECTORY32 TLS;//TLS表
	PIMAGE_DELAYLOAD_DESCRIPTOR delayload;//延时加载表
};
BOOL EnumProcess(std::vector<PROCESSINFO>* proclist);
//int  EnumProcessNum();
BOOL EnumThread(std::vector<THREADENTRY32>* ThreadList);
BOOL EnumModul(std::vector<MYMODULEENTRY32>* moduleList);
BOOL EnumFile(CString path);
BOOL CALLBACK EnumWindowProcM(HWND hWnd, LPARAM lParam);
double getProcessMemoryUsage(DWORD dwPid);
double getProcessCpuUsage(DWORD dwPid);
double getCpuUsage();
/************************************************************************
// 函数名: EnableDebugPrivilege
// 功  能: 将程序的权限提升为SeDebug权限
// 全  名: EnableDebugPrivilege
// 权  限: public
// 返回值: bool
// 形  参: BOOL bIsOpen
//************************************************************************/
bool EnableDebugPrivilege();

CStringA CWTOCA(CStringW wstr);
CStringW CATOCS(CStringA str);
//PE文件解析部分函数
BOOL GetPEHead(TCHAR* path, PEINFO* peHead);
DWORD RvaToFoa(PEINFO* peInfo, DWORD dwRva);
DWORD GetSection(PEINFO* peInfo, DWORD dwRva);
//CPU利用率
int getCpuUse();

