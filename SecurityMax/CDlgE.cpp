// CDlgE.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CDlgE.h"
#include "afxdialogex.h"
#include "CMULU.h"
#include "CDAOCHU.h"
#include "CTLS.h"
#include "CDAORU.h"
#include "CRESOURCE.h"
#include "CDELAYLOAD.h"
#include "CRelocation.h"


// CDlgE 对话框

IMPLEMENT_DYNAMIC(CDlgE, CDialogEx)

CDlgE::CDlgE(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG5, pParent)
	, m_path(_T(""))
{

}

CDlgE::~CDlgE()
{
}

void CDlgE::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_path);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CDlgE, CDialogEx)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgE::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDlgE::OnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgE::OnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgE::OnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlgE::OnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDlgE::OnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CDlgE::OnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CDlgE::OnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CDlgE::OnClickedButton9)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgE 消息处理程序


void CDlgE::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR filePath[MAX_PATH] = { 0 };
	//单文件操作，第二参数置0，第三个参数返回文件路径                           

	DragQueryFile(hDropInfo, 0, filePath, MAX_PATH - 1);
	SetDlgItemText(IDC_EDIT1, filePath);
	DragFinish(hDropInfo); //释放拖放占用内存，必须！
	CDialogEx::OnDropFiles(hDropInfo);
}

void CDlgE::OnClickedButton1()
{
	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY,
		_T("Describe Files (*.exe,*.dll)|*.exe|All Files (*.*)|*.*||"), NULL);
	if (dlgFile.DoModal())
	{
		m_path =dlgFile.GetPathName();
	}
	SetDlgItemText(IDC_EDIT1, m_path);
	UpdateData(true);
	//CStringA str = CWTOCA(m_path);
	
}
void CDlgE::setEditTextE(PEINFO* peHead)
{
	CString buffer;
	buffer.Format(L"%08X", peHead->oep);
	SetDlgItemText(IDC_EDIT2, buffer);
	buffer.Format(L"%08X", peHead->ImageBase);
	SetDlgItemText(IDC_EDIT3, buffer);
	buffer.Format(L"%08X", peHead->SizeofImage);
	SetDlgItemText(IDC_EDIT4, buffer);
	buffer.Format(L"%08X", peHead->BaseOfCode);
	SetDlgItemText(IDC_EDIT5, buffer);
	buffer.Format(L"%08X", peHead->BaseOfData);
	SetDlgItemText(IDC_EDIT6, buffer);
	buffer.Format(L"%08X", peHead->SectionAlignment);
	SetDlgItemText(IDC_EDIT7, buffer);
	buffer.Format(L"%08X", peHead->FileAlignment);
	SetDlgItemText(IDC_EDIT8, buffer);
	buffer.Format(L"%04X", peHead->Magic);
	SetDlgItemText(IDC_EDIT9, buffer);
	buffer.Format(L"%04X", peHead->Subsystemm);
	SetDlgItemText(IDC_EDIT10,buffer);
	buffer.Format(L"%04X", peHead->NumberOfSections);
	SetDlgItemText(IDC_EDIT11,buffer);
	buffer.Format(L"%08X", peHead->TimeDateStamp);
	SetDlgItemText(IDC_EDIT12,buffer);
	buffer.Format(L"%08X", peHead ->SizeOfOptionalHeader );
	SetDlgItemText(IDC_EDIT13,buffer);
	buffer.Format(L"%04X", peHead ->Characteristics );
	SetDlgItemText(IDC_EDIT14,buffer);
	buffer.Format(L"%08X", peHead ->CheckSum );
	SetDlgItemText(IDC_EDIT15,buffer);
	buffer.Format(L"%04X", peHead ->SizeOfOptionalHeader );
	SetDlgItemText(IDC_EDIT16,buffer);
	buffer.Format(L"%08X", peHead ->NumberOfRvaAndSizes );
	SetDlgItemText(IDC_EDIT17,buffer);

	m_list.DeleteAllItems();
	PIMAGE_SECTION_HEADER sectionHead = peHead->sectionHead;
	for (size_t i = 0; i < peHead->NumberOfSections; i++)
	{
		m_list.InsertItem(i, _T(""));
		char* str = (char*)sectionHead->Name;
		buffer.Format(_T("%s"),CATOCS( str));
		m_list.SetItemText(i, 0, buffer);
		buffer.Format(_T("%08X"), sectionHead->VirtualAddress);
		m_list.SetItemText(i, 1, buffer);
		buffer.Format(_T("%08X"), sectionHead->SizeOfRawData);
		m_list.SetItemText(i,2, buffer);
		buffer.Format(_T("%08X"), sectionHead->PointerToRawData);
		m_list.SetItemText(i,3, buffer);
		buffer.Format(_T("%08X"), sectionHead->Misc.VirtualSize);
		m_list.SetItemText(i, 4, buffer);
		buffer.Format(_T("%08X"), sectionHead->Characteristics);
		m_list.SetItemText(i, 5, buffer);
		sectionHead++;
	}
}

//目录
void CDlgE::OnClickedButton2()
{
	if (m_path == "" || m_path.IsEmpty())
	{
		return;
	}
	CMULU cDlg;
	cDlg.setPeInfo(&peHead);
	cDlg.DoModal();
}

//导出表
void CDlgE::OnClickedButton3()
{
	if (m_path == "" || m_path.IsEmpty())
	{
		return;
	}
	CDAOCHU cDlg;
	cDlg.setPeInfo(&peHead);
	cDlg.DoModal();
}

//导入表
void CDlgE::OnClickedButton4()
{
	if (m_path == "" || m_path.IsEmpty())
	{
		return;
	}
	CDAORU cDlg;
	cDlg.setPeInfo(&peHead);
	cDlg.DoModal();
}

//资源表
void CDlgE::OnClickedButton5()
{
	if (m_path == "" || m_path.IsEmpty())
	{
		return;
	}
	CRESOURCE cDlg;
	cDlg.setPeInfo(&peHead);
	cDlg.DoModal();
}

//重定位表
void CDlgE::OnClickedButton6()
{
	if (m_path == "" || m_path.IsEmpty())
	{
		return;
	}
	CRelocation cDlg;
	cDlg.setPeInfo(&peHead);
	cDlg.DoModal();
}

//TLS表
void CDlgE::OnClickedButton7()
{
	if (m_path == "" || m_path.IsEmpty())
	{
		return;
	}
	CTLS cDlg;
	cDlg.setPeInfo(&peHead);
	cDlg.DoModal();
}

//延时加载表
void CDlgE::OnClickedButton8()
{
	if (m_path == "" || m_path.IsEmpty())
	{
		return;
	}
	CDELAYLOAD cDlg;
	cDlg.setPeInfo(&peHead);
	cDlg.DoModal();
}

//解析按钮
void CDlgE::OnClickedButton9()
{
	UpdateData(true);
	if (m_path == "" || m_path.IsEmpty())
	{
		MessageBox(L"请选择地址");
		return;
	}
	GetPEHead((LPTSTR)(LPCTSTR)m_path, &peHead);
	setEditTextE(&peHead);
}


BOOL CDlgE::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_list.InsertColumn(0, L"名称", 0, 100);
	m_list.InsertColumn(1, L"Voffset", 0, 110);
	m_list.InsertColumn(2, L"VSzie", 0, 120);
	m_list.InsertColumn(3, L"ROffset", 0, 120);
	m_list.InsertColumn(4, L"Rsize", 0, 120);
	m_list.InsertColumn(5, L"标志", 0, 120);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgE::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}
