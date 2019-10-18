// CDlgB.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDlgB.h"
#include "afxdialogex.h"
#include "Md5.h"

#include <vector>
#include <Psapi.h>
#include "CBingd.h"
// CDlgB 对话框

IMPLEMENT_DYNAMIC(CDlgB, CDialogEx)

CDlgB::CDlgB(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CDlgB::~CDlgB()
{
}

void CDlgB::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	DDX_Control(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDC_RADIO2, m_r2);
	DDX_Control(pDX, IDC_CHECK1, m_c1);
	DDX_Control(pDX, IDC_RADIO1, m_r1);
}


BEGIN_MESSAGE_MAP(CDlgB, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgB::OnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgB::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgB::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgB::OnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgB::OnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlgB::OnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDlgB::OnClickedButton6)
	ON_WM_COPYDATA()
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CDlgB::OnRclickList2)
	ON_COMMAND(ID_32784, &CDlgB::On32784)
	ON_COMMAND(ID_32785, &CDlgB::On32785)
END_MESSAGE_MAP()


// CDlgB 消息处理程序


BOOL CDlgB::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	hDlgB = this->m_hWnd;
	m_menu.LoadMenu(IDR_MENU4);
	m_r1.SetCheck(BST_CHECKED);
	m_list1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_list1.InsertColumn(0, L"序号", 0, 80);
	m_list1.InsertColumn(1, L"文件", 0, 400);
	m_list1.InsertColumn(3, L"MD5", 0, 210);
	m_list2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	m_list2.InsertColumn(0, L"进程名", 0, 110);
	m_list2.InsertColumn(1, L"PID", 0, 60);
	m_list2.InsertColumn(2, L"文件", 0, 335);
	m_list2.InsertColumn(3, L"MD5", 0, 210);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void CDlgB::setListTextD1(CString path,DWORD* num)
{
	WIN32_FIND_DATA fileData = { 0 };
	HANDLE hFind =
		FindFirstFile(path + L"\\*", &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}
	DWORD* dwFileCount = num;
	CString buffer;
	do
	{
		if (wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0)continue;
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//wchar_t* pPath = new wchar_t[266];
			//swprintf_s(pPath, 266, L"%s\\%s", (LPCWSTR)path, fileData.cFileName);
			buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
			setListTextD1(buffer, dwFileCount);
		}
		else
		{
			buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
			CStringA str = CWTOCA(buffer);
			char* ch2 = new char[str.GetLength() + 1];
			memset(ch2, 0, str.GetLength() + 1);
			strcpy_s(ch2, str.GetLength() + 1, str.GetString());
			char* md5 = md5FileValue(ch2);
			delete[] ch2;
			for (int i = 0; i < listBD.size(); ++i)
			{
				if (!strcmp(listBD[i].md5, md5))
				{
					m_list1.InsertItem(*dwFileCount, L"");
					m_list1.SetCheck(*dwFileCount);
					buffer.Format(L"%02d", *dwFileCount+1);
					m_list1.SetItemText(*dwFileCount, 0, buffer);
					buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
					m_list1.SetItemText(*dwFileCount, 1, buffer);
					buffer.Format(L"%s",CATOCS(md5));
					m_list1.SetItemText(*dwFileCount, 2, buffer);
					FileTimeToLocalFileTime(&fileData.ftCreationTime, &fileData.ftCreationTime);
					SYSTEMTIME sysTime = { 0 };
					*dwFileCount += 1;
				}
			}			
		}
	} while (FindNextFile(hFind, &fileData));
}

void CDlgB::setListTextD2()
{
	std::vector<PROCESSINFO> ProcList;
	if (!EnumProcess(&ProcList))
	{
		return;
	}
	CString buffer;
	
	for (int i = 0; i < ProcList.size(); ++i)
	{
		TCHAR   lpExeName[MAX_PATH] = { 0 };
		m_list2.InsertItem(i, L"");
		buffer.Format(L"%s", ProcList[i].szExeFile);
		m_list2.SetItemText(i, 0, buffer);
		buffer.Format(L"%d", ProcList[i].th32ProcessID);
		m_list2.SetItemText(i, 1, buffer);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcList[i].th32ProcessID);
		GetProcessImageFileName(hProcess, lpExeName, MAX_PATH);
		switch (lpExeName[22])
		{
		case '2':
		{
			lpExeName[21] = 'C';
			lpExeName[22] = ':';
			break;
		}
		case '5':
		{
			lpExeName[21] = 'D';
			lpExeName[22] = ':';
			break;
		}
		case '4':
		{
			lpExeName[21] = 'E';
			lpExeName[22] = ':';
			break;
		}
		default:
			break;
		}
		for (size_t i = 0; i < MAX_PATH-21; i++)
		{
			lpExeName[i] = lpExeName[i + 21];
		}
		buffer.Format(L"%s", lpExeName);
		m_list2.SetItemText(i, 2, buffer);
		CStringA str = CWTOCA(buffer);
		if (!buffer.IsEmpty())
		{
			char* ch2 = new char[str.GetLength() + 1]();
			memset(ch2, 0, str.GetLength() + 1);
			strcpy_s(ch2, str.GetLength() + 1, str.GetString());
			char* md5 = md5FileValue(ch2);
			delete[] ch2;
			buffer.Format(L"%s", CATOCS(md5));
		}
		//buffer.Format(L"%s", CATOCS(md5));
		m_list2.SetItemText(i,3, buffer);
	}
}
void CDlgB::OnClickedRadio2()
{
	if (m_r2.GetCheck()== BST_CHECKED)
	{
		m_path = SelFilePathSimple();
		SetDlgItemText(IDC_EDIT1, m_path);
	}	
}

//扫描文件
void CDlgB::OnClickedButton1()
{
	m_list1.DeleteAllItems();
	listBD.swap(vector<bingd>());
	DWORD num = 0;
	CString path;
	if (m_c1.GetCheck())
	{
		Select(&client);
		DWORD dw = recvProc1(&client);
	}
	if (m_r2.GetCheck())
	{
		bingd bd;
		int bn = 0;
		ReadFile(&bd, bn,0);
		while (strcmp(bd.md5, "0"))
		{
			bn += 100;
			listBD.push_back(bd);
			ReadFile(&bd, bn,0);
		}
		setListTextD1(m_path, &num);
	}
	else
	{
		MessageBox(L"要扫描全局请先删除return;");
		return;
		TCHAR buffer[100];
		GetLogicalDriveStrings(100, buffer);
		TCHAR* p = buffer;
		while (*p != 0)
		{
			TCHAR volName[50];
			TCHAR fileSystemName[50];
			GetVolumeInformation(p, volName, sizeof(volName), NULL, NULL, NULL,
				fileSystemName, sizeof(fileSystemName));
			path.Format(L"%s", p);
			setListTextD1(path, &num);
			p += 4;
		}		
	}
}

//本地病毒库
void CDlgB::OnClickedButton2()
{
	listBD.swap(vector<bingd>());
	bingd bd;
	int bn = 0;
	ReadFile(&bd, bn, 0);
	while (strcmp(bd.md5, "0"))
	{
		bn += 100;
		listBD.push_back(bd);
		ReadFile(&bd, bn, 0);
	}
	CBingd dlgBD;
	dlgBD.setData(listBD, L"病毒库");
	dlgBD.DoModal();

}

//扫描进程
void CDlgB::OnClickedButton3()
{
	m_list2.DeleteAllItems();
	listWL.swap(vector<bingd>());
	bingd bd;
	int bn = 0;
	ReadFile(&bd, bn, 1);
	while (strcmp(bd.md5, "0"))
	{
		bn += 100;
		listWL.push_back(bd);
		ReadFile(&bd, bn, 1);
	}
	setListTextD2();
}

//一键结束
void CDlgB::OnClickedButton4()
{
	bingd bd;
	BOOL isFlag = TRUE;
	for (int i = 0; i < m_list2.GetItemCount(); ) {
		if (m_list2.GetCheck(i)) {
			CString path = m_list2.GetItemText(i, 2);
			//判断是否在白名单中
			CStringA str = CWTOCA(path);
			char* ch2 = new char[str.GetLength() + 1];
			memset(ch2, 0, str.GetLength() + 1);
			strcpy_s(ch2, str.GetLength() + 1, str.GetString());
			char* md5 = md5FileValue(ch2);
			strcpy_s(bd.md5, 50, md5);
			delete[] ch2;
			for (int j = 0; j < listWL.size(); ++j)
			{
				//
				if (!strcmp(bd.md5, listWL[j].md5))
				{
					MessageBox(L"该进程已经存在白名单中！");
					isFlag = FALSE;
				}
			}
			if (isFlag)
			{
				//不在则结束进程
				CString sPid = m_list2.GetItemText(i, 1);
				DWORD m_pid = _wtoi(sPid);
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, m_pid);
				if (hProcess == 0)
				{
					CString error;
					error.Format(L"获取进程句柄失败：%d", GetLastError());
					MessageBox(error);
				}
				else
				{
					TerminateProcess(hProcess, 0);
					CloseHandle(hProcess);
				}
				m_list2.DeleteItem(i);
			}		
		}
		++i;
	}
}

//白名单
void CDlgB::OnClickedButton5()
{
	listWL.swap(vector<bingd>());
	bingd bd;
	int bn = 0;
	ReadFile(&bd, bn, 1);
	while (strcmp(bd.md5, "0"))
	{
		bn += 100;
		listWL.push_back(bd);
		ReadFile(&bd, bn, 1);
	}
	CBingd dlgBD;
	dlgBD.setData(listWL, L"白名单");
	dlgBD.DoModal();

}

//一键清除可疑文件
void CDlgB::OnClickedButton6()
{
	for (int i = 0; i < m_list1.GetItemCount(); ) {
		if (m_list1.GetCheck(i)) {
			CString path = m_list1.GetItemText(i, 2);
			if (DeleteFile(path)) {
				m_list1.DeleteItem(i);
				continue;
			}
			m_list1.DeleteItem(i);
			continue;
		}
		++i;
	}
}


BOOL CDlgB::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	char* Nextbuffer = NULL;
	char* type = NULL;
	char* buffa = NULL;
	char* sta = NULL;
	bingd bd;
	type = strtok_s((char*)pCopyDataStruct->lpData, "\\", &Nextbuffer);
	buffa = strtok_s(NULL, "\\", &Nextbuffer);
	if (strcmp(type, "1") == 0)
	{
		char* nexda = NULL;
		char* date = NULL;
		char* dat1 = NULL;
		sta = strtok_s((char*)buffa, "\x11", &Nextbuffer);
		sta = strtok_s(NULL, "\x11", &Nextbuffer);
		while (sta != NULL)
		{
			date = strtok_s((char*)sta, "\x10", &nexda);
			dat1 = strtok_s(NULL, "\x10", &nexda);
			sprintf_s(bd.name, 50, date);
			sprintf_s(bd.md5, 50, dat1);
			listBD.push_back(bd);
			sta = strtok_s(NULL, "\x11", &Nextbuffer);
		}
	}
	return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


void CDlgB::OnRclickList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CMenu* pSubMenu = m_menu.GetSubMenu(0);
	CPoint pos;
	GetCursorPos(&pos);
	pSubMenu->TrackPopupMenu(0, pos.x, pos.y, this);
}

//加入白名单
void CDlgB::On32784()
{
	int sel = (int)m_list2.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString path = m_list2.GetItemText(sel, 2);
	bingd bd;
	CStringA str = CWTOCA(path);
	char* ch2 = new char[str.GetLength() + 1];
	memset(ch2, 0, str.GetLength() + 1);
	strcpy_s(ch2, str.GetLength() + 1, str.GetString());
	char* md5 = md5FileValue(ch2);
	strcpy_s(bd.md5, 50, md5);
	strcpy_s(bd.name, 50, ch2);
	delete[] ch2;
	BOOL isFlag = TRUE;
	for (int i = 0; i < listWL.size(); ++i)
	{
		//
		if (!strcmp(bd.md5, listWL[i].md5))
		{
			MessageBox(L"操作失败！该进程已经存在白名单中！");
			isFlag = FALSE;
		}		
	}
	if (isFlag)
	{
		listWL.push_back(bd);
		SaveFile(&bd, 1);
		MessageBox(L"添加成功！");
	}
}

//移除白名单
void CDlgB::On32785()
{
	int sel = (int)m_list2.GetFirstSelectedItemPosition();
	if (sel == 0)
	{
		return;
	}
	sel -= 1;
	CString path = m_list2.GetItemText(sel, 1);
	bingd bd;
	CStringA str = CWTOCA(path);
	char* ch2 = new char[str.GetLength() + 1];
	memset(ch2, 0, str.GetLength() + 1);
	strcpy_s(ch2, str.GetLength() + 1, str.GetString());
	char* md5 = md5FileValue(ch2);
	strcpy_s(bd.md5, 50, md5);
	delete[] ch2;
	for (int i = 0; i < listWL.size(); ++i)
	{
		//
		if (!strcmp(bd.md5, listWL[i].md5))
		{
			listWL.erase(listWL.begin() + i);
			DeleteFile(&listWL, 1);
			MessageBox(L"删除成功！");			
		}
		else
		{
			MessageBox(L"操作失败，该进程不存在白名单中！");
		}

	}
}
