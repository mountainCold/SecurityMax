#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "CPE.h"
#include <vector>

struct PROCESSINFO : public PROCESSENTRY32
{
	DWORD dwMemoryUsage; //�ڴ�ʹ����
	double dCpuUsage = 0;// cpu ʹ����
};
struct MYMODULEENTRY32
{
	DWORD dwPid;
	MODULEENTRY32 modul;
};
struct PEINFO
{
	DWORD* m_buff;
	DWORD oep;		//��ڵ�ַ
	DWORD ImageBase; //�����ַ
	DWORD SizeofImage; //�����ַ��С
	DWORD BaseOfCode; //�����ַ
	DWORD BaseOfData; //���ݻ�ַ
	DWORD SectionAlignment; //�����
	DWORD FileAlignment; //�ļ������
	WORD Magic;			//��־��
	DWORD Subsystemm;  //��ϵͳ
	WORD  NumberOfSections; //�������� �ļ�ͷ��
	DWORD TimeDateStamp;	//����ʱ���־ �ļ�ͷ��
	DWORD SizeOfHeaders;    //���״�С
	WORD Characteristics;	//����ֵ	�ļ�ͷ��
	DWORD CheckSum;			//У���
	WORD SizeOfOptionalHeader;//��ѡͷ��С �ļ�ͷ��
	DWORD NumberOfRvaAndSizes; //����Ŀ¼����
	PIMAGE_SECTION_HEADER sectionHead;//���α�
	PIMAGE_DATA_DIRECTORY DataDirectory;//����Ŀ¼��
	PIMAGE_EXPORT_DIRECTORY exportT; //������
	DWORD exFoa;					//
	DWORD exname;					//
	DWORD* pEAT;
	DWORD* pENT;
	WORD* pEOT;
	PIMAGE_IMPORT_DESCRIPTOR importT;//�����
	PIMAGE_RESOURCE_DIRECTORY resource;//��Դ��
	PIMAGE_BASE_RELOCATION relocation;//�ض�λ��
	PIMAGE_TLS_DIRECTORY32 TLS;//TLS��
	PIMAGE_DELAYLOAD_DESCRIPTOR delayload;//��ʱ���ر�
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
// ������: EnableDebugPrivilege
// ��  ��: �������Ȩ������ΪSeDebugȨ��
// ȫ  ��: EnableDebugPrivilege
// Ȩ  ��: public
// ����ֵ: bool
// ��  ��: BOOL bIsOpen
//************************************************************************/
bool EnableDebugPrivilege();

CStringA CWTOCA(CStringW wstr);
CStringW CATOCS(CStringA str);
//PE�ļ��������ֺ���
BOOL GetPEHead(TCHAR* path, PEINFO* peHead);
DWORD RvaToFoa(PEINFO* peInfo, DWORD dwRva);
DWORD GetSection(PEINFO* peInfo, DWORD dwRva);
//CPU������
int getCpuUse();

