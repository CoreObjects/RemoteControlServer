// RemoteCtrl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "..\FrameWork\framework.h"
#include "..\CServerSocket\ServerSocket.h"
#include "..\Resource\resource.h"
#include "..\Page\LockDialog.h"
#include <direct.h>
#include <io.h>
#include <list>
#include <atlimage.h>

CWinApp theApp;

using namespace std;
void Dump(const char* pData, size_t nSize) {
	std::string strOut;
	for (size_t i = 0; i < nSize; i++) {
		char buf[8] = "";
		snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xff);
		strOut += buf;
	}
	strOut += "\n";
	OutputDebugStringA(strOut.c_str());
}
int MakeDirverInfo() {
	std::string result;
	for (int i = 1; i <= 26; i++) {
		if (_chdrive(i) == 0) {
			if (result.size() > 0) result += ',';
			result += 'A' + i - 1;
		}
	}
	CPacket packet(1, result.c_str(), result.size());
	Dump(packet.Data(), packet.Size());
	CServerSocket::GetInstance().Send(packet);
	return 0;
}
int RunFile() {
	std::string strPath;
	CServerSocket::GetInstance().GetFilePath(strPath);
	ShellExecuteA(NULL, NULL, strPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	CPacket packet(3, NULL, 0);
	CServerSocket::GetInstance().Send(packet);
	return 0;
}
#pragma warning(disable:4996)
int DownloadFile() {
	std::string strPath;
	CServerSocket::GetInstance().GetFilePath(strPath);
	long long data = 0;
	FILE* pFile = nullptr;
	errno_t err = fopen_s(&pFile, strPath.c_str(), "rb");

	if (err != 0) {
		CPacket packet(4, (char*)&data, 8);
		CServerSocket::GetInstance().Send(packet);
		return -1;
	}
	if (pFile != NULL) {
		fseek(pFile, 0, SEEK_END);
		data = _ftelli64(pFile);
		CPacket head(4, (char*)&data, 8);
		CServerSocket::GetInstance().Send(head);
		fseek(pFile, 0, SEEK_SET);
		char buffer[0x400]{ 0 };
		size_t rLen = 0;
		do {
			rLen = fread(buffer, 1, 1024, pFile);
			CPacket packet(4, buffer, rLen);
			CServerSocket::GetInstance().Send(packet);
		} while (rLen >= 1024);
		fclose(pFile);
	}
	CPacket packet(4, NULL, 0);
	CServerSocket::GetInstance().Send(packet);
	return 0;
}
int MakeDirectoryInfo() {
	/*std::list<FILEINFO> lstFileInfos;*/
	std::string strPath;
	if (CServerSocket::GetInstance().GetFilePath(strPath) == false) {
		OutputDebugString(L"当前命令不是获取文件列表，命令解析错误！！！");
		return -1;
	}
	if (_chdir(strPath.c_str()) != 0) {
		FILEINFO fInfo;
		fInfo.IsInvalid = TRUE;
		fInfo.IsDirectory = TRUE;
		fInfo.HasNext = FALSE;
		memcpy(fInfo.szFileName, strPath.c_str(), strPath.size());
		/*lstFileInfos.push_back(fInfo);*/
		TRACE("服务器：获取文件名 %s\r\n", fInfo.szFileName);
		CPacket packet(2, (const char*)&fInfo, sizeof(fInfo));
		CServerSocket::GetInstance().Send(packet);
		OutputDebugString(L"没有权限，访问目录！！！");
		return -2;
	}
	_finddata_t fdata;
	int hfind = 0;
	
	if ((hfind = _findfirst("*", &fdata)) == -1) {
		OutputDebugString(L"没有找到任何文件！！！");
		FILEINFO fInfo;
		fInfo.HasNext = FALSE;
		TRACE("服务器：获取文件名 %s\r\n", fInfo.szFileName);
		TRACE("服务器：获取是否是目录 %d\r\n", fInfo.IsDirectory);
		CPacket packet(2, (const char*)&fInfo, sizeof(fInfo));
		CServerSocket::GetInstance().Send(packet);
		return -3;
	}
	int nCount = 0;
	do {
		FILEINFO fInfo;
		fInfo.IsDirectory = (fdata.attrib & _A_SUBDIR) != 0;
		memcpy(fInfo.szFileName, fdata.name, strlen(fdata.name));
		/*lstFileInfos.push_back(fInfo);*/
		TRACE("服务器：获取文件名 %s\r\n", fInfo.szFileName);
		TRACE("服务器：获取是否是目录 %d\r\n", fInfo.IsDirectory);
		CPacket packet(2, (const char*)&fInfo, sizeof(fInfo));
		CServerSocket::GetInstance().Send(packet);
		nCount++;
	} while (!_findnext(hfind, &fdata));
	//发送信息到控制端。
	FILEINFO fInfo;
	fInfo.HasNext = FALSE;
	TRACE("服务器一共发送%d个文件\r\n", nCount);
	CPacket packet(2, (const char*)&fInfo, sizeof(fInfo));
	CServerSocket::GetInstance().Send(packet);
	return 0;
}
int MouseEvent() {
	MOUSEEV mouse;
	if (CServerSocket::GetInstance().GetMouseEvent(mouse)) {
		DWORD nFlag = 0;
		switch (mouse.nButton) {
		case 0://左键
			nFlag = 1;
			break;
		case 1://右键
			nFlag = 2;
			break;
		case 2://中键
			nFlag = 4;
			break;
		case 4://没有按键
			nFlag = 8;
			break;
		}
		if (nFlag != 8) {
			SetCursorPos(mouse.ptXY.x, mouse.ptXY.y);
		}
		switch (mouse.nAction) {
		case 0://单击
			nFlag |= 0x10;
			break;
		case 1://双击
			nFlag |= 0x20;
			break;
		case 2://按下
			nFlag |= 0x40;
			break;
		case 3: //放开
			nFlag |= 0x80;
			break;
		}
		switch (nFlag) {
		case 0x21://左键双机
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
		case 0x11://左键单机
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x41://左键按下
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x81://左键放开
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x22://右键双机
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
		case 0x12://→键单机
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x42://右键按下
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x82://右键放开
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x24://中键双机
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
		case 0x14://中键单机
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x44://中键按下
			mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x84://中键放开
			mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
			break;
		case 0x08://单纯的鼠标移动
			mouse_event(MOUSEEVENTF_MOVE, mouse.ptXY.x, mouse.ptXY.y, 0, GetMessageExtraInfo());
			break;
		}
		CPacket packet(4, NULL, 0);
		CServerSocket::GetInstance().Send(packet);
	}
	else {
		OutputDebugString(_T("获取鼠标操作参数失败！！！"));
		return -1;
	}
	return 0;
}
int SendScreen() {
	CImage screen; //GDI
	HDC hScreen = ::GetDC(NULL);
	int nBitPerPixel = ::GetDeviceCaps(hScreen, BITSPIXEL);
	int nWidth = GetDeviceCaps(hScreen, HORZRES);
	int nHeigth = GetDeviceCaps(hScreen, VERTRES);
	screen.Create(nWidth, nHeigth, nBitPerPixel);
	BitBlt(screen.GetDC(), 0, 0, 1920, 1020, hScreen, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hScreen);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	IStream* pStream = NULL;
	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (ret == S_OK) {
		screen.Save(pStream, Gdiplus::ImageFormatJPEG);
		LARGE_INTEGER bg{ 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);
		char* pData = (char*)GlobalLock(hMem);
		size_t nSize = GlobalSize(hMem);
		CPacket packet(6, pData, nSize);
		CServerSocket::GetInstance().Send(packet);
		GlobalUnlock(hMem);
	}

	/*	screen.Save(_T("test2023.jpg"), Gdiplus::ImageFormatJPEG);*/
	pStream->Release();
	screen.ReleaseDC();
	GlobalFree(hMem);
	return 0;
}
CLockDialog dlg;
void ThreadLockDlg(void* arg) {
	BOOL bRet = dlg.Create(IDD_DIALOG_INFO);
	dlg.ShowWindow(SW_SHOW);
	CRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXFULLSCREEN);
	rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN) * 1.1;
	dlg.MoveWindow(rect);
	//窗口置顶
	dlg.SetWindowPos(&dlg.wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	//限制鼠标功能
	ShowCursor(FALSE);
	//隐藏任务栏
	::ShowWindow(::FindWindow(_T("Shell_TrayWnd"), NULL), SW_HIDE);
	//限制鼠标活动范围；
	dlg.GetWindowRect(rect);
	ClipCursor(rect);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_KEYDOWN) {
			TRACE("msg:%08x wParam:%08x lParam:%08x\r\n", msg.message, msg.wParam, msg.lParam);
			if (msg.wParam == 0x1b || msg.wParam == 0x41) {//按下esc退出
				break;
			}
		}
	}
	dlg.DestroyWindow();
	ShowCursor(true);
	::ShowWindow(::FindWindow(_T("Shell_TrayWnd"), NULL), SW_SHOW);
	_endthread();
}
int LockMachine() {
	/*MessageBox(NULL,_T(""), )*/
	if (dlg.m_hWnd == NULL || dlg.m_hWnd == INVALID_HANDLE_VALUE) {
		_beginthread(ThreadLockDlg, 0, NULL);
	}
	CPacket packet(7, NULL, 0);
	CServerSocket::GetInstance().Send(packet);
	return 0;
}
int UnlockMachine() {
	dlg.PostMessage(WM_KEYDOWN, 0x41, 0x01E0001);
	return 0;
}
int TestConnect() {
	CPacket packet(1981, NULL, 0);
	bool ret = CServerSocket::GetInstance().Send(packet);
	TRACE("Testconnet Send ret = %d\r\n", ret);
	return 0;
}
int DeleteLocalFile() {
	//TODO:
	std::string strPath;
	CServerSocket::GetInstance().GetFilePath(strPath);
	//mbstowcs()
	DeleteFileA(strPath.c_str());
	CPacket packet(9, NULL, 0);
	bool ret = CServerSocket::GetInstance().Send(packet);
	TRACE("Testconnet Send ret = %d\r\n", ret);
	return 0;
}
int ExcuteCommand(int nCmd) {
	int ret = 0;
	switch (nCmd) {
	case 1://查看磁盘分区
		ret = MakeDirverInfo();
		break;
	case 2://查看目录文件
		ret = MakeDirectoryInfo();
		break;
	case 3: //打开文件
		ret = RunFile();
		break;
	case 4://下载文件
		ret = DownloadFile();
		break;
	case 5://鼠标操作
		ret = MouseEvent();
		break;
	case 6://发送屏幕内容==>发送屏幕的截图
		ret = SendScreen();
		break;
	case 7://锁机
		ret = LockMachine();
		Sleep(100);
		ret = LockMachine();
		break;
	case 8://解锁
		ret = UnlockMachine();
		break;
	case 9://删除文件
		ret = DeleteLocalFile();
	case 1981://连接测试
		ret = TestConnect();
		break;
	}
	return ret;
}

int main() {
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(nullptr);
	if (hModule != nullptr) {
		//初始化MFC并在失败时显示错误
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0)) {
			MessageBoxA(NULL, "错误", "MFC初始化失败", MB_OK);
			nRetCode = 1;
		}
		else {
			//TODO:socket、bind、listen、accept、read、write、close
			CServerSocket& serverSocket = CServerSocket::GetInstance();
			if (serverSocket.InitSocket() == false) {
				MessageBox(NULL, _T("网络初始化异常，未能成功初始化，请检查网络状态！"), _T("网络初始化失败"), MB_OK | MB_ICONERROR);
				exit(0);
			}
			int nCount = 0;
			while (1) {
				if (serverSocket.AcceptClient() == false) {
					if (nCount >= 3) {
						MessageBox(NULL, _T("失败三次，退出！！！"), _T("网络初始化失败"), MB_OK | MB_ICONERROR);
						exit(0);
					}
					MessageBox(NULL, _T("接入用户失败，无法正常接入用户，自动重试"), _T("网络初始化失败"), MB_OK | MB_ICONERROR);
					nCount++;
				}
				nCount = 0;
				TRACE("AccepetClient return True \r\n");
				int ret = serverSocket.DealCommand();
				TRACE("Server DealCommand ret = %d\r\n", ret);
				if (ret > 0) {
					ret = ExcuteCommand(ret);
					if (ret != 0) {
						TRACE("执行命令失败，%d ret = %d\r\n", serverSocket.GetPacket().wCmd, ret);
					}
					serverSocket.CloseClient();
					TRACE("Command has done!\r\n");
				}
			}
		}
	}
	else {
		MessageBoxA(NULL, "错误", "GetModuleHandle失败", MB_OK);
		nRetCode = 1;
	}
	return nRetCode;
}


