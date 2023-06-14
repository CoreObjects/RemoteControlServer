
// RemoteControlClientDlg.h: 头文件
//

#pragma once
#include "..\CClientSocket\ClientSocket.h"

// CRemoteControlClientDlg 对话框
class CRemoteControlClientDlg : public CDialogEx
{
// 构造
public:
	CRemoteControlClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECONTROLCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	int SendCommandPacket(int nCmd, char* pData = NULL, size_t nLength = 0);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	DWORD m_Server_address;
	int m_nPort;
	afx_msg void OnBnClickedButtonFileInfo();
	CTreeCtrl m_Tree;
};
