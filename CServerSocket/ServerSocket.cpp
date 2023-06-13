#include "..\FrameWork\framework.h"
#include "ServerSocket.h"

CServerSocket& g_socket = CServerSocket::GetInstance();

CServerSocket& CServerSocket::GetInstance() {
	static CServerSocket instance;
	return instance;
}

bool CServerSocket::InitSocket() {
	if (m_socket == -1)return false;
	//TODO：校验
	sockaddr_in serv_adr{ 0 };
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.S_un.S_addr = INADDR_ANY;
	serv_adr.sin_port = htons(12138);
	//绑定
	if (bind(m_socket, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
		return false;
	}
	//监听
	if (listen(m_socket, 1) == -1) {
		return false;
	}
	return true;
}

bool CServerSocket::AcceptClient() {
	//accept
	sockaddr_in client_adr{ 0 };
	int cli_sz = sizeof(client_adr);
	m_client = accept(m_socket, (sockaddr*)&client_adr, &cli_sz);
	if (m_client == -1) {
		return false;
	}
	return true;
	//recv(sServSocket, szBuffer, sizeof(szBuffer), 0);
	//send(sServSocket, szBuffer, sizeof(szBuffer), 0);
	
}
#define BUFFER_SIZE 0x1000
int CServerSocket::DealCommand() {
	if (m_client == -1)return -1;
	char buffer[BUFFER_SIZE]{ 0 };
	size_t index = 0;
	while (true) {
		size_t nLength = recv(m_client, buffer + index, BUFFER_SIZE - index, 0);
		if (nLength <= 0)return -1;
		index += nLength;
		nLength = index;
		m_Packet = CPacket(buffer, nLength);
		if (nLength > 0) {
			memmove(buffer, buffer + nLength, BUFFER_SIZE - nLength);
			index += nLength;
			return m_Packet.wCmd;
		}
	}
	return -1;
}

bool CServerSocket::Send(const char* pData, size_t nSize) {
	if (m_client == -1)return false;
	return send(m_client, pData, nSize, 0) > 0;
}

CServerSocket::CServerSocket() {
	m_socket = INVALID_SOCKET;
	m_client = INVALID_SOCKET;
	if (InitSocketEnv() == FALSE) {
		MessageBox(NULL, _T("初始化套接字环境失败,请检查网络设置！"), \
			_T("初始化错误！"), MB_OK | MB_ICONERROR);
	}
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
}

CServerSocket::~CServerSocket() {
	closesocket(m_socket);
	WSACleanup();
}
