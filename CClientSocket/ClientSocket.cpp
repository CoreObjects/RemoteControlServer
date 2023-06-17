#include "ClientSocket.h"
#include "..\FrameWork\framework.h"
#pragma warning(disable : 4996)

CClientSocket& g_socket = CClientSocket::GetInstance();

CClientSocket& CClientSocket::GetInstance() {
	static CClientSocket instance;
	return instance;
}
std::string GetErrInfo(int wsaErrCode) {
	std::string ret;
	LPVOID lpMsgBuffer = nullptr;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,\
		NULL, wsaErrCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
		(LPSTR)&lpMsgBuffer, 0, NULL);
	LocalFree(lpMsgBuffer);
	ret = (char*)lpMsgBuffer;
	return ret;
}
bool CClientSocket::InitSocket(DWORD dwIp,WORD wPort) {
	if (m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
	}
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (m_socket == -1)return false;
	//TODO：校验
	sockaddr_in serv_adr{ 0 };
	serv_adr.sin_family = AF_INET;
//	serv_adr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	serv_adr.sin_addr.S_un.S_addr = htonl(dwIp);
	TRACE("Server IP Adderss = %x\r\n", serv_adr.sin_addr.S_un.S_addr);
	serv_adr.sin_port = htons(wPort);
	//链接
	if (serv_adr.sin_addr.S_un.S_addr == INADDR_NONE) {
		AfxMessageBox(_T("指定的IP地址不存在"));
		TRACE("指定的IP地址不存在%d %s\r\n", WSAGetLastError(), GetErrInfo(WSAGetLastError()));
		return false;
	}
	if (connect(m_socket, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
		AfxMessageBox(_T("连接失败"));
		TRACE("连接失败%d %s\r\n", WSAGetLastError(), GetErrInfo(WSAGetLastError()).c_str());
		return false;
	}
	return true;
}
#define BUFFER_SIZE 0x80000
int CClientSocket::DealCommand() {
	if (m_socket == -1)return -1;
	char* buffer = m_buffer.data();
	static size_t index = 0;
	while (true) {
		size_t nLength = recv(m_socket, buffer + index, BUFFER_SIZE - index, 0);
		if (nLength <= 0 && index == 0)return -1;
		TRACE("Client recv len:%d \r\n", nLength);
		index += nLength;
		nLength = index;
		m_Packet = CPacket(buffer, nLength);
		if (nLength > 0) {
			memmove(buffer, buffer + nLength, index - nLength);
			index -= nLength;
			return m_Packet.wCmd;
		}
	}
	return -1;
}

bool CClientSocket::Send(const char* pData, size_t nSize) {
	if (m_socket == -1)return false;
	return send(m_socket, pData, nSize, 0) > 0;
}

CClientSocket::CClientSocket() {
	m_socket = INVALID_SOCKET;
	if (InitSocketEnv() == FALSE) {
		MessageBox(NULL, _T("初始化套接字环境失败,请检查网络设置！"), \
			_T("初始化错误！"), MB_OK | MB_ICONERROR);
	}
	m_buffer.resize(BUFFER_SIZE);
	memset(m_buffer.data(), 0, BUFFER_SIZE);
}

CClientSocket::~CClientSocket() {
	if (m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
	}
	WSACleanup();
}
