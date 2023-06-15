#pragma once
#include "..\FrameWork\framework.h"
void Dump(const char* pData, size_t nSize);
typedef struct  SMouseEvent{
	SMouseEvent() {
		nAction = 0;
		nButton = -1;
		ptXY.x = 0;
		ptXY.y = 0;
	}
	WORD nAction;//点击、移动、双击
	WORD nButton;//左键、右键、中键
	POINT ptXY;//坐标
}MOUSEEV,PMOUSEEV;
typedef struct file_info {
	file_info() {
		IsInvalid = 0;
		IsDirectory = -1;
		HasNext = TRUE;
		memset(szFileName, 0, MAX_PATH);
	}
	BOOL IsInvalid;//是否无效
	BOOL IsDirectory;
	BOOL HasNext;//是否还有后续0没有，1有
	char szFileName[MAX_PATH];
}FILEINFO, * PFILEINFO;
class CServerSocket {
public:
	static CServerSocket& GetInstance(); 
	bool InitSocket();
	bool AcceptClient();
	int DealCommand();
	bool Send(const char* pData, size_t nSize);
	bool Send(CPacket& packet) {
		if (m_client == -1)return false;
		Dump(packet.Data(), packet.Size());
		return send(m_client, packet.Data(), packet.dwLength + 6, 0) > 0;
	}
	bool GetFilePath(std::string& strPath) {
		if ((m_Packet.wCmd <= 4 && m_Packet.wCmd >= 0x2)) {
			strPath = m_Packet.strData;
			return true;
		}
		else {
			return false;
		}
	}
	bool GetMouseEvent(const MOUSEEV& mouse) {
		if (m_Packet.wCmd == 5) {
			memcpy((void*)&mouse, m_Packet.strData.c_str(), sizeof(MOUSEEV));
			return true;
		}
		return false;
	}
	CPacket& GetPacket() {
		return m_Packet;
	}
	void CloseClient() {
		closesocket(m_client);
		m_client = INVALID_SOCKET;
	}
private:
	// Delete copy constructor and assignment operator.
	// This is usually done as part of making a class a Singleton.
	CServerSocket(CServerSocket const&) = delete;
	CServerSocket& operator=(CServerSocket const&) = delete;
	CServerSocket();
	~CServerSocket(); 
	BOOL InitSocketEnv() {
		WSADATA data;
		if (WSAStartup(MAKEWORD(1, 1), &data) != 0) {
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	SOCKET m_socket;
	SOCKET m_client;
	CPacket m_Packet;
};
extern CServerSocket& g_socket;