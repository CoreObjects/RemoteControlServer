
// RemoteControlClientDlg.h: 头文件
//

#pragma once
#include "..\CClientSocket\ClientSocket.h"

#define WM_SEND_PACKET (WM_USER + 1)//发送数据包
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
	/*
		1:查看磁盘分区
		2:查看目录文件
		3:打开文件
		4:下载文件
		5:鼠标操作
		6:发送屏幕内容==>发送屏幕的截图
		7:锁机
		8:解锁
		1981:连接测试
	*/
	int SendCommandPacket(int nCmd, bool bAutoClose = true, char* pData = NULL, size_t nLength = 0);
	CString GetPath(HTREEITEM hTree);
	void DeleteTreeChildItem(HTREEITEM hTree);
	void LoadFileInfo();
	void LoadFileCurrent();
	static void threadEnrtyDownLoadFile(void* arg);
	void ThreadDownFile();
	static void threadEntryForWatchData(void* arg);
	void threadWatchData();
private:
	CImage m_image; //缓存
	bool m_isFull;//缓存是否有数据，true表示有缓存数据，false表示没有缓存数据
public:
	bool isFull()const {
		return m_isFull;
	}
	CImage& GetImage() {
		return m_image;
	}
	void SetImageStatus(bool bisFull = false) {
		m_isFull = bisFull;
	}
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
	afx_msg void OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickTreeDir(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_List;
	afx_msg void OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OndownloadFile();
	afx_msg void OnDeletFile();
	afx_msg void OnOpenFile();
	afx_msg LRESULT OnSendPacket(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
