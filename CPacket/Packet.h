#pragma once
#include "..\FrameWork\framework.h"
#include "..\CRC\CRC16.h"
#pragma pack(push)
#pragma pack(1)
class CPacket {
public: 
	CPacket() :wHeader(0),dwLength(0),wCmd(0),wSum(0){}
	CPacket(WORD Cmd, const char* pData, size_t nSize) {
		wHeader = 0xFEFF;
		dwLength = nSize + 4;
		wCmd = Cmd;
		if (nSize > 0) {
			strData.resize(nSize);
			memcpy((void*)strData.c_str(), pData, nSize);
			strData.resize(nSize);
			memcpy((void*)strData.c_str(), pData, nSize);
			CRC16 crc16;
			wSum = crc16.calculate(pData, nSize);
		}
		else {
			strData.clear();
			wSum = 0;
		}
		
	}
	CPacket(const char* pData, size_t& nSize) {
		size_t i = 0;
		for (; i < nSize; i++) {
			if (*(WORD*)(pData + i) == 0xFEFF) {
				wHeader = *(WORD*)(pData + i);
				i += 2;
				break;
			}
		}
		if (i + 4 + 2 + 2 > nSize) {
			nSize = 0;
			return;
		}
		dwLength = *(DWORD*)(pData + i); i += 4;
		if (dwLength + i > nSize) {
			nSize = 0;
			return;
		}
		wCmd = *(WORD*)(pData + i); i += 2;
		if (dwLength > 4) {
			strData.resize(dwLength - 2 - 2);
			memcpy((void*)strData.c_str(), pData + i, dwLength - 2 - 2);
			i += dwLength - 4;
		}
		wSum = *(WORD*)(pData + i);
		i += 2;
		CRC16 crc16;
		WORD wCrcSum = crc16.calculate(strData.c_str(), strData.size());
		if (wSum == wCrcSum) {
			nSize = i;
		}
		nSize = 0;
	}
	CPacket(const CPacket& packet) {
		wHeader = packet.wHeader;
		dwLength = packet.dwLength;
		wCmd = packet.wCmd;
		strData = packet.strData;
		wSum = packet.wSum;
	}
	CPacket& operator=(const CPacket& packet){
		if (this != &packet) {
			wHeader = packet.wHeader;
			dwLength = packet.dwLength;
			wCmd = packet.wCmd;
			strData = packet.strData;
			wSum = packet.wSum;
		}
		return *this;
	}
	int Size() {
		return dwLength + 6;
	}
	const char* Data() {
		strOut.resize(dwLength + 6);
		char* pData = (char*)strOut.c_str();
		*(WORD*)pData = wHeader; pData += 2;
		*(DWORD*)pData = dwLength; pData += 4;
		*(WORD*)pData = wCmd; pData += 2;
		memcpy(pData, strData.c_str(), strData.size()); pData += strData.size();
		*(WORD*)pData = wSum;
		return strOut.c_str();
	}
	~CPacket() {}
	WORD wHeader;//固定位FEFF
	DWORD dwLength;		//包长度
	WORD wCmd;	//命令
	std::string strData;	//包数据
	WORD wSum;	//校验和
	std::string strOut;//整个包的数据
};
#pragma pack(pop)
