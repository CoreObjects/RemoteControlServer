
// RemoteControlClientDlg.cpp: 实现文件
//

#include "..\FrameWork\framework.h"
#include "RemoteControlClient.h"
#include "RemoteControlClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRemoteControlClientDlg 对话框



CRemoteControlClientDlg::CRemoteControlClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECONTROLCLIENT_DIALOG, pParent)
	, m_Server_address(0) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteControlClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS_SERVER, m_Server_address);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Control(pDX, IDC_TREE_DIR, m_Tree);
}

int CRemoteControlClientDlg::SendCommandPacket(int nCmd, char* pData, size_t nLength) {
	UpdateData(TRUE);
	CClientSocket& g_Client = CClientSocket::GetInstance();
	bool ret = g_Client.InitSocket(m_Server_address, m_nPort);//TODO返回值处理
	if (!ret) {
		AfxMessageBox(_T("网络初始化失败"));
		return -1;
	}
	CPacket packet(nCmd, pData, nLength);
	ret = g_Client.Send(packet);
	TRACE("ret = %d\r\n", ret);
	nCmd = g_Client.DealCommand();
	TRACE("ack:%d\r\n", g_Client.GetPacket().wCmd);
	g_Client.CloseSocket();
	return nCmd;
}

BEGIN_MESSAGE_MAP(CRemoteControlClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRemoteControlClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_FILE_INFO, &CRemoteControlClientDlg::OnBnClickedButtonFileInfo)
END_MESSAGE_MAP()


// CRemoteControlClientDlg 消息处理程序

BOOL CRemoteControlClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	UpdateData();
	m_Server_address = 0x7f000001;
	m_nPort = 12138;
	UpdateData(FALSE);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteControlClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteControlClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRemoteControlClientDlg::OnBnClickedButton1() {
	// TODO: 在此添加控件通知处理程序代码
	SendCommandPacket(1981);
}


void CRemoteControlClientDlg::OnBnClickedButtonFileInfo() {
	// TODO: 在此添加控件通知处理程序代码
	int ret = SendCommandPacket(1);
	if (ret == -1) {
		AfxMessageBox(_T("命令处理失败！！！"));
		return;
	}
	CClientSocket& g_Client = CClientSocket::GetInstance();
	std::string drivers = g_Client.GetPacket().strData;
	std::string dr;
	m_Tree.DeleteAllItems();
	for (char i : drivers) {
		if (i == ',') {
			dr += ":";
			m_Tree.InsertItem(dr.c_str(), TVI_ROOT, TVI_LAST);
			dr.clear();
			continue;
		}
		dr += i;
	}
	dr += ":";
	m_Tree.InsertItem(dr.c_str(), TVI_ROOT, TVI_LAST);
}
