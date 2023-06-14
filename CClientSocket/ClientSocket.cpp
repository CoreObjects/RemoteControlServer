#include "ClientSocket.h"
#include "..\FrameWork\framework.h"
#pragma warning(disable : 4996)

CClientSocket& g_socket = CClientSocket::GetInstance();

CClientSocket& CClientSocket::GetInstance() {
	static CClientSocket instance;
	return instance;
}
std::string GetErrorInfo(int wsaErrCode) {
	std::string ret;
	LPVOID lpMsgBuffer = nullptr;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,\
		NULL, wsaErrCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
		(LPSTR)&lpMsgBuffer, 0, NULL);
	LocalFree(lpMsgBuffer);
	ret = (char*)lpMsgBuffer;
	return ret;
}
bool CClientSocket::InitSocket(const std::string& strIPAddress) {
	if (m_socket == -1)return false;
	//TODO��У��
	sockaddr_in serv_adr{ 0 };
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.S_un.S_addr = inet_addr(strIPAddress.c_str());
	serv_adr.sin_port = htons(12138);
	//����
	if (serv_adr.sin_addr.S_un.S_addr == INADDR_NONE) {
		AfxMessageBox(L"ָ����IP��ַ������");
		TRACE("ָ����IP��ַ������%d %s\r\n", WSAGetLastError(), GetErrorInfo(WSAGetLastError()));
		return false;
	}
	if (connect(m_socket, (sockaddr*)&serv_adr, sizeof(serv_adr)) == -1) {
		AfxMessageBox(L"����ʧ��");
		TRACE("����ʧ��%d %s\r\n", WSAGetLastError(), GetErrorInfo(WSAGetLastError()));
		return false;
	}
	return true;
}
#define BUFFER_SIZE 0x1000
int CClientSocket::DealCommand() {
	if (m_socket == -1)return -1;
	char buffer[BUFFER_SIZE]{ 0 };
	size_t index = 0;
	while (true) {
		size_t nLength = recv(m_socket, buffer + index, BUFFER_SIZE - index, 0);
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

bool CClientSocket::Send(const char* pData, size_t nSize) {
	if (m_socket == -1)return false;
	return send(m_socket, pData, nSize, 0) > 0;
}

CClientSocket::CClientSocket() {
	m_socket = INVALID_SOCKET;
	if (InitSocketEnv() == FALSE) {
		MessageBox(NULL, _T("��ʼ���׽��ֻ���ʧ��,�����������ã�"), \
			_T("��ʼ������"), MB_OK | MB_ICONERROR);
	}
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
}

CClientSocket::~CClientSocket() {
	closesocket(m_socket);
	WSACleanup();
}
