
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
	DDX_Control(pDX, IDC_LIST_FILE, m_List);
}

int CRemoteControlClientDlg::SendCommandPacket(int nCmd, bool bAutoClose, char* pData, size_t nLength) {
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
	if(bAutoClose)
		g_Client.CloseSocket();
	return nCmd;
}

BEGIN_MESSAGE_MAP(CRemoteControlClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRemoteControlClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_FILE_INFO, &CRemoteControlClientDlg::OnBnClickedButtonFileInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DIR, &CRemoteControlClientDlg::OnNMDblclkTreeDir)
	ON_NOTIFY(NM_CLICK, IDC_TREE_DIR, &CRemoteControlClientDlg::OnNMClickTreeDir)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CRemoteControlClientDlg::OnNMRClickListFile)
	ON_COMMAND(ID_32771, &CRemoteControlClientDlg::OndownloadFile)
	ON_COMMAND(ID_32772, &CRemoteControlClientDlg::OnDeletFile)
	ON_COMMAND(ID_32773, &CRemoteControlClientDlg::OnOpenFile)
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
			HTREEITEM hTemp = m_Tree.InsertItem(dr.c_str(), TVI_ROOT, TVI_LAST);
			m_Tree.InsertItem(NULL, hTemp, TVI_LAST);
			dr.clear();
			continue;
		}
		dr += i;
	}
	dr += ":";
	HTREEITEM hTemp = m_Tree.InsertItem(dr.c_str(), TVI_ROOT, TVI_LAST);
	m_Tree.InsertItem(NULL, hTemp, TVI_LAST);
}

CString CRemoteControlClientDlg::GetPath(HTREEITEM hTree) {
	CString strRet, strTmp;
	do 
	{
		strTmp = m_Tree.GetItemText(hTree);
		strRet = (strTmp + '\\' + strRet);
		hTree = m_Tree.GetParentItem(hTree);
	} while (hTree != NULL);
	return strRet;
}

void CRemoteControlClientDlg::DeleteTreeChildItem(HTREEITEM hTree) {
	HTREEITEM hSub = nullptr;
	do 
	{
		hSub = m_Tree.GetChildItem(hTree);
		if (hSub != NULL) {
			m_Tree.DeleteItem(hSub);
		}
	} while (hSub);
//	m_Tree.DeleteItem(hTree);
}

void CRemoteControlClientDlg::LoadFileInfo() {
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_Tree.ScreenToClient(&ptMouse);
	HTREEITEM hTreeSelected = m_Tree.HitTest(ptMouse, 0);
	if (hTreeSelected == NULL) {
		return;
	}
	if (m_Tree.GetChildItem(hTreeSelected) == NULL) {
		return;
	}
	CString strPath = GetPath(hTreeSelected);
	DeleteTreeChildItem(hTreeSelected);
	m_List.DeleteAllItems();
	int nCmd = SendCommandPacket(2, false, (char*)(LPCTSTR)strPath, strPath.GetLength());
	PFILEINFO pInfo = (PFILEINFO)CClientSocket::GetInstance().GetPacket().strData.c_str();
	int nCount = 0;
	CClientSocket& g_Client = CClientSocket::GetInstance();
	while (pInfo->HasNext) {
		nCount++;
		TRACE("客户端：获取文件名 %s\r\n", pInfo->szFileName);
		TRACE("客户端：获取是否是目录 %d\r\n", pInfo->IsDirectory);
		if (pInfo->IsDirectory) {
			if (CString(pInfo->szFileName) == "." || (CString)pInfo->szFileName == "..") {
				int cmd = g_Client.DealCommand();
				TRACE("acl:%d\r\n", cmd);
				if (cmd < 0) break;
				pInfo = (PFILEINFO)CClientSocket::GetInstance().GetPacket().strData.c_str();
				continue;
			}
			HTREEITEM hTemp = m_Tree.InsertItem(pInfo->szFileName, hTreeSelected, TVI_LAST);
			m_Tree.InsertItem(NULL, hTemp, TVI_LAST);
		}
		else {
			m_List.InsertItem(0, pInfo->szFileName);
		}
		
		
		int cmd = g_Client.DealCommand();
		TRACE("acl:%d\r\n", cmd);
		if (cmd < 0) break;
		pInfo = (PFILEINFO)CClientSocket::GetInstance().GetPacket().strData.c_str();
	}
	TRACE("客户端接受到%d个文件\r\n", nCount);
	g_Client.CloseSocket();
}

void CRemoteControlClientDlg::LoadFileCurrent() {
	HTREEITEM hTree = m_Tree.GetSelectedItem();
	CString strPath = GetPath(hTree);
	m_List.DeleteAllItems();
	int nCmd = SendCommandPacket(2, false, (char*)(LPCTSTR)strPath, strPath.GetLength());
	PFILEINFO pInfo = (PFILEINFO)CClientSocket::GetInstance().GetPacket().strData.c_str();

	CClientSocket& g_Client = CClientSocket::GetInstance();
	while (pInfo->HasNext) {
		TRACE("客户端：获取文件名 %s\r\n", pInfo->szFileName);
		TRACE("客户端：获取是否是目录 %d\r\n", pInfo->IsDirectory);
		if (!pInfo->IsDirectory) {
			m_List.InsertItem(0, pInfo->szFileName);
		}
		int cmd = g_Client.DealCommand();
		TRACE("acl:%d\r\n", cmd);
		if (cmd < 0) break;
		pInfo = (PFILEINFO)CClientSocket::GetInstance().GetPacket().strData.c_str();
	}
	g_Client.CloseSocket();
}

void CRemoteControlClientDlg::OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;
	LoadFileInfo();
}


void CRemoteControlClientDlg::OnNMClickTreeDir(NMHDR* pNMHDR, LRESULT* pResult) {
	*pResult = 0;
	LoadFileInfo();
}


void CRemoteControlClientDlg::OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int nItem = m_List.GetNextItem(-1, LVNI_SELECTED);
	if (nItem != -1) {
		CString strText = m_List.GetItemText(nItem, 0); // 获取第0列（即第一列）的文本
		TRACE("选中的项的文本是: %s\n", strText);
		// Do something with strText...

		// 弹出菜单
		CMenu menu;
		if (menu.LoadMenu(IDR_MENU1)) {  // 加载菜单
			CMenu* pPopup = menu.GetSubMenu(0);  // 获取弹出菜单
			if (pPopup != nullptr) {
				CPoint mousePoint;
				::GetCursorPos(&mousePoint);  // 获取鼠标位置
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, mousePoint.x, mousePoint.y, this);
			}
		}
	}
	else {
		TRACE("没有项被选中.\n");
	}

	*pResult = 0;
}


#pragma warning(disable: 4996)
void CRemoteControlClientDlg::OndownloadFile() {
	int nItem = m_List.GetNextItem(-1, LVNI_SELECTED);
	CString strFile = m_List.GetItemText(nItem, 0); // 获取第0列（即第一列）的文本
	CFileDialog fdlg(FALSE, "*", strFile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, NULL, this);
	CClientSocket& g_Client = CClientSocket::GetInstance();
	INT_PTR fDlgRet = fdlg.DoModal();
	if (fDlgRet == IDOK) {
		FILE* pFile = fopen(fdlg.GetPathName(), "wb+");
		if (pFile == NULL) {
			AfxMessageBox("本地没有权限保存该文件，或者文件无法创建！！！");
			return;
		}
		HTREEITEM hItem = m_Tree.GetSelectedItem();
		strFile = GetPath(hItem) + strFile;
		TRACE("文件的路径是: %s\n", strFile);
		int ret = SendCommandPacket(4, false, (char*)(LPCSTR)strFile, strFile.GetLength());
		if (ret < 0) {
			AfxMessageBox("下载失败");
			fclose(pFile);
			g_Client.CloseSocket();
			return;
		}
		long long nlength = *(long long*)g_Client.GetPacket().strData.c_str();
		if (nlength == 0) {
			AfxMessageBox("文件长度为0，无法下载");
			fclose(pFile);
			g_Client.CloseSocket();
			return;
		}
		long long nCount = 0;
		
		while (nCount < nlength) {
			ret = g_Client.DealCommand();
			if (ret < 0) {
				AfxMessageBox("传输失败，ret = %d\r\n", ret);
				TRACE("传输失败：ret = %d\r\n", ret);
				break;
			}
			fwrite(g_Client.GetPacket().strData.c_str(), 1, g_Client.GetPacket().strData.size(), pFile);
			nCount += g_Client.GetPacket().strData.size();
		}
		fclose(pFile);
		g_Client.CloseSocket();
	};
	
}


void CRemoteControlClientDlg::OnDeletFile() {
	// TODO: 在此添加命令处理程序代码
	int nItem = m_List.GetNextItem(-1, LVNI_SELECTED);
	CString strFile = m_List.GetItemText(nItem, 0);
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	strFile = GetPath(hItem) + strFile;
	TRACE("删除文件路径：%s", strFile.GetBuffer());
	int Ret = SendCommandPacket(9, true, strFile.GetBuffer(), strFile.GetLength());
	if (Ret < 0) {
		AfxMessageBox("打开文件命令执行失败！！！");
	}
	LoadFileCurrent();
}


void CRemoteControlClientDlg::OnOpenFile() {
	int nItem = m_List.GetNextItem(-1, LVNI_SELECTED);
	CString strFile = m_List.GetItemText(nItem, 0);
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	strFile = GetPath(hItem) + strFile;
	TRACE("删除文件路径：%s", strFile.GetBuffer());
	int Ret = SendCommandPacket(3, true, strFile.GetBuffer(), strFile.GetLength());
	if (Ret < 0) {
		AfxMessageBox("打开文件命令执行失败！！！");
	}
}
