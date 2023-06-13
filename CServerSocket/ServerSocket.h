#pragma once
#include "..\FrameWork\framework.h"
typedef struct  SMouseEvent{
	SMouseEvent() {
		nAction = 0;
		nButton = -1;
		ptXY.x = 0;
		ptXY.y = 0;
	}
	WORD nAction;//µã»÷¡¢ÒÆ¶¯¡¢Ë«»÷
	WORD nButton;//×ó¼ü¡¢ÓÒ¼ü¡¢ÖÐ¼ü
	POINT ptXY;//×ø±ê
}MOUSEEV,PMOUSEEV;
class CServerSocket {
public:
	static CServerSocket& GetInstance(); 
	bool InitSocket();
	bool AcceptClient();
	int DealCommand();
	bool Send(const char* pData, size_t nSize);
	bool Send(CPacket& packet) {
		if (m_client == -1)return false;
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