// CBingd.cpp: 实现文件
//

#include "pch.h"
#include "SecurityMax.h"
#include "CBingd.h"
#include "afxdialogex.h"


// CBingd 对话框

IMPLEMENT_DYNAMIC(CBingd, CDialogEx)

CBingd::CBingd(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG6, pParent)
{

}

CBingd::~CBingd()
{
}

void CBingd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(CBingd, CDialogEx)
END_MESSAGE_MAP()


// CBingd 消息处理程序


void CBingd::setData(std::vector<bingd> listB, CString t)
{
	listD = listB;
	title = t;
}

BOOL CBingd::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(title);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_list.InsertColumn(0, L"序号", 0, 80);
	m_list.InsertColumn(1, L"文件", 0, 200);
	m_list.InsertColumn(2, L"MD5", 0, 200);
	CString buffer;
	for (int i = 0; i < listD.size(); i++)
	{
		m_list.InsertItem(i, L"");
		buffer.Format(L"%d", i);
		m_list.SetItemText(i, 0, buffer);
		buffer.Format(L"%s",(CATOCS)( listD[i].name));
		m_list.SetItemText(i, 1, buffer);
		buffer.Format(L"%s", (CATOCS)(listD[i].md5));
		m_list.SetItemText(i, 2, buffer);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
