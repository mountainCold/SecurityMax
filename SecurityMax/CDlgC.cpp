// CDlgC.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDlgC.h"
#include "afxdialogex.h"


// CDlgC 对话框

IMPLEMENT_DYNAMIC(CDlgC, CDialogEx)

CDlgC::CDlgC(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG3, pParent)
{

}

CDlgC::~CDlgC()
{
}

void CDlgC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_COMBO1, m_mBox);
	DDX_Control(pDX, IDC_CHECK1, m_c1);
	DDX_Control(pDX, IDC_CHECK2, m_c2);
	DDX_Control(pDX, IDC_CHECK3, m_c3);
}


BEGIN_MESSAGE_MAP(CDlgC, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgC::OnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgC::OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgC::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgC::OnClickedButton3)
END_MESSAGE_MAP()


// CDlgC 消息处理程序


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
//获取系统垃圾
void CDlgC::setListTextF1(CString path, DWORD* num)
{
	WIN32_FIND_DATA fileData = { 0 };
	HANDLE hFind =
		FindFirstFile(path + L"\\*", &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return ;
	}
	DWORD*  dwFileCount = num;
	CString buffer;
	do
	{
		if (wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0)continue;
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//wchar_t* pPath = new wchar_t[266];
			//swprintf_s(pPath, 266, L"%s\\%s", (LPCWSTR)path, fileData.cFileName);
			buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
			setListTextF1(buffer, dwFileCount);
		}
		else
		{
			LPWSTR suff = PathFindExtension(fileData.cFileName);
			CString buff = L".tlog.obj.pch.ilk.pdb";
			
			//if (!lstrcmp(suff, L".tlog") || !lstrcmp(suff, L".obj") || !lstrcmp(suff, L".pch")
			//	|| !lstrcmp(suff, L".ilk") || !lstrcmp(suff, L".pdb"))
			if(buff.Find(suff) != -1)
			{
				m_list.InsertItem(*dwFileCount, L"");
				buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
				m_list.SetItemText(*dwFileCount, 0, buffer);
				ULONG64 size = (fileData.nFileSizeHigh << 32) | fileData.nFileSizeLow;
				buffer.Format(L"%lldKb", size / 1024);
				m_list.SetItemText(*dwFileCount, 1, buffer);
				FileTimeToLocalFileTime(&fileData.ftCreationTime, &fileData.ftCreationTime);
				SYSTEMTIME sysTime = { 0 };
				FileTimeToSystemTime(&fileData.ftCreationTime, &sysTime);
				buffer.Format(L"%04d/%02d/%02d %02d:%02d:%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);
				m_list.SetItemText(*dwFileCount, 2, buffer);
				*dwFileCount+=1;
			}
		}
	} while (FindNextFile(hFind, &fileData));
}
//浏览器垃圾
void CDlgC::setListTextF2(CString path, DWORD* num)
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
			setListTextF1(buffer, dwFileCount);
		}
		else
		{
			m_list.InsertItem(*dwFileCount, L"");
			buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
			m_list.SetItemText(*dwFileCount, 0, buffer);
			ULONG64 size = (fileData.nFileSizeHigh << 32) | fileData.nFileSizeLow;
			buffer.Format(L"%lldKb", size / 1024);
			m_list.SetItemText(*dwFileCount, 1, buffer);
			FileTimeToLocalFileTime(&fileData.ftCreationTime, &fileData.ftCreationTime);
			SYSTEMTIME sysTime = { 0 };
			FileTimeToSystemTime(&fileData.ftCreationTime, &sysTime);
			buffer.Format(L"%04d/%02d/%02d %02d:%02d:%02d",
				sysTime.wYear,
				sysTime.wMonth,
				sysTime.wDay,
				sysTime.wHour,
				sysTime.wMinute,
				sysTime.wSecond);
			m_list.SetItemText(*dwFileCount, 2, buffer);
			*dwFileCount+=1;

		}
	} while (FindNextFile(hFind, &fileData));
}
//VS垃圾
void CDlgC::setListTextF3(CString path,DWORD* num)
{
	WIN32_FIND_DATA fileData = { 0 };
	HANDLE hFind =
		FindFirstFile(path + L"\\*", &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}
	DWORD* dwFileCount = num;
	CString m_filter = L".ilk .pdb .obj .log .pch .tlog .lastbuildstate .sdf .idb .ipch .res .o .lst .knl .img .bin .db";
	CString buffer;
	do
	{
		if (wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0)continue;
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//wchar_t* pPath = new wchar_t[266];
			//swprintf_s(pPath, 266, L"%s\\%s", (LPCWSTR)path, fileData.cFileName);
			buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
			setListTextF1(buffer, dwFileCount);
		}
		else
		{
			LPWSTR suff = PathFindExtension(fileData.cFileName);

			if (m_filter.Find(suff)!=-1)
			{
				m_list.InsertItem(*dwFileCount, fileData.cFileName);
				buffer.Format(L"%s%s%s", path, L"\\", fileData.cFileName);
				m_list.SetItemText(*dwFileCount, 0, buffer);
				ULONG64 size = (fileData.nFileSizeHigh << 32) | fileData.nFileSizeLow;
				buffer.Format(L"%lldKb", size / 1024);
				m_list.SetItemText(*dwFileCount, 1, buffer);
				FileTimeToLocalFileTime(&fileData.ftCreationTime, &fileData.ftCreationTime);
				SYSTEMTIME sysTime = { 0 };
				FileTimeToSystemTime(&fileData.ftCreationTime, &sysTime);
				buffer.Format(L"%04d/%02d/%02d %02d:%02d:%02d",
					sysTime.wYear,
					sysTime.wMonth,
					sysTime.wDay,
					sysTime.wHour,
					sysTime.wMinute,
					sysTime.wSecond);
				m_list.SetItemText(*dwFileCount, 2, buffer);
				*dwFileCount+=1;
			}
			
		}
	} while (FindNextFile(hFind, &fileData));
}

BOOL CDlgC::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_mBox.AddString(L"全路径");
	m_mBox.AddString(L"指定路径");
	m_mBox.SetCurSel(0);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_list.InsertColumn(0, L"文件", 0, 600);
	m_list.InsertColumn(1, L"大小", 0, 100);
	m_list.InsertColumn(2, L"创建时间", 0, 120);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CDlgC::OnSelchangeCombo1()
{
	int index = m_mBox.GetCurSel();
	if (index ==1)
	{
		m_path = SelFilePathSimple();
		SetDlgItemText(IDC_EDIT1, m_path);
	}
}
//清空回收站
void CDlgC::OnClickedButton1()
{
	SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
}


//扫描
void CDlgC::OnClickedButton2()
{
	m_list.DeleteAllItems();
	int a=m_c1.GetCheck();
	int b=m_c2.GetCheck();
	int c=m_c3.GetCheck();
	DWORD aaa = 0;
	DWORD*  num =&aaa;
	CString buffer;
	if (a == 0 && b == 0 && c == 0)
	{
		buffer.Format(L"%s", L"请选择扫描文件类型");
		MessageBox(buffer);
	}
	else
	{
		if (a)
		{
			CString path1 = L"C:\\Windows\\Temp";//系统临时文件
			CString path2 = L"C:\\Users\\kevin\\AppData\\Local\\Temp";//用户临时文件
			CString path3 = L"C:\\Users\\kevin\\AppData\\Local\\Microsoft\\Windows\\WER\\ReportQueue";
			setListTextF1(path1, num);
			setListTextF1(path2, num);
			setListTextF1(path3, num);
		}
		if (b)
		{
			//谷歌缓存垃圾
			CString path1 = L"C:\\Users\\hasee\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Cache";
			CString path2 = L"C:\\Users\\hasee\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Session Storage\\*.log";
			CString path3 = L"C:\\Users\\hasee\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Session Storage\\LOG.old";
			setListTextF2(path1,num);
			setListTextF2(path2,num);
			setListTextF2(path3,num);
		}
		if (c)
		{
			if (m_path.IsEmpty() || m_path == L"")MessageBox(L"请选择需要扫描的VS路径");
			setListTextF3(m_path,num);
		}
	}

}

//清除
void CDlgC::OnClickedButton3()
{
	for (int i = 0; i < m_list.GetItemCount(); ) {
		if (m_list.GetCheck(i)) {

			CString path = m_list.GetItemText(i, 0);
			if (DeleteFile(path)) {
				m_list.DeleteItem(i);
				continue;
			}
			m_list.DeleteItem(i);
			continue;
		}
		++i;
	}
}
