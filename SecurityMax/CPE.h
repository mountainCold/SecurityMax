#pragma once
#include<Windows.h>
class CPE
{
public:
	bool InitPe(TCHAR* path);
	//��ȡDOSͷ
	PIMAGE_DOS_HEADER GetDosHeader();
	// ��ȡNTͷ
	PIMAGE_NT_HEADERS GetNtHeader();
	// �Ƿ���PE�ļ�
	// ��������ڴ�ת���ļ�ƫ��
	DWORD RvaToFoa(DWORD dwRva);
	bool IsPeFile();
	DWORD GetExprotFOA();
	DWORD* CPE::GetExportEAT();
	DWORD* CPE::GetExportENT();
	WORD* CPE::GetExportEOT();
	// ��ȡ���α���һ������
	PIMAGE_SECTION_HEADER GetSectionHeader();
	//��ȡ������
	PIMAGE_EXPORT_DIRECTORY GetExprotDirectory();
	//��ȡ�����
	PIMAGE_IMPORT_DESCRIPTOR GetImportDirectory();
	//��ȡ��Դ��
	PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory();
	//��ȡTLS��
	PIMAGE_TLS_DIRECTORY32 GetTLSDirectory();
	//��ȡ�ض�λ��
	PIMAGE_BASE_RELOCATION GetRelocalDirectory();
	PIMAGE_DELAYLOAD_DESCRIPTOR GetDelayloadDirectory();
	//�ļ�ͷ��Ϣ
	void ShowFileHeader();
	// ��ʾ��չͷ�е�����
	void ShowOptionHeader();
	// ��ʾ������Ϣ
	void ShowSectionInfo();
	//��ʾ��������Ϣ
	void ShowExprotInfo();
	//��ʾ�������Ϣ
	void ShowImportInfo();
	// ��ʾ��Դ����Ϣ
	void ShowResourceInfo();
	// ��ʾTLS����Ϣ
	void ShowTLSInfo();
	//��ʾ�ض�λ����Ϣ
	void ShowRelocalInfo();
	char* m_buff;
private:
	TCHAR* m_path;
	DWORD m_size;
	
};
