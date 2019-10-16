#pragma once
#include<Windows.h>
class CPE
{
public:
	bool InitPe(TCHAR* path);
	//获取DOS头
	PIMAGE_DOS_HEADER GetDosHeader();
	// 获取NT头
	PIMAGE_NT_HEADERS GetNtHeader();
	// 是否是PE文件
	// 相对虚拟内存转换文件偏移
	DWORD RvaToFoa(DWORD dwRva);
	bool IsPeFile();
	DWORD GetExprotFOA();
	DWORD* CPE::GetExportEAT();
	DWORD* CPE::GetExportENT();
	WORD* CPE::GetExportEOT();
	// 获取区段表，是一个数组
	PIMAGE_SECTION_HEADER GetSectionHeader();
	//获取导出表
	PIMAGE_EXPORT_DIRECTORY GetExprotDirectory();
	//获取导入表
	PIMAGE_IMPORT_DESCRIPTOR GetImportDirectory();
	//获取资源表
	PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory();
	//获取TLS表
	PIMAGE_TLS_DIRECTORY32 GetTLSDirectory();
	//获取重定位表
	PIMAGE_BASE_RELOCATION GetRelocalDirectory();
	PIMAGE_DELAYLOAD_DESCRIPTOR GetDelayloadDirectory();
	//文件头信息
	void ShowFileHeader();
	// 显示扩展头中的数据
	void ShowOptionHeader();
	// 显示区段信息
	void ShowSectionInfo();
	//显示导出表信息
	void ShowExprotInfo();
	//显示导入表信息
	void ShowImportInfo();
	// 显示资源表信息
	void ShowResourceInfo();
	// 显示TLS表信息
	void ShowTLSInfo();
	//显示重定位表信息
	void ShowRelocalInfo();
	char* m_buff;
private:
	TCHAR* m_path;
	DWORD m_size;
	
};
