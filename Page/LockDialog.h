﻿#pragma once


// CLockDialog 对话框
#include "..\Resource\resource.h"
class CLockDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CLockDialog)

public:
	CLockDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CLockDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
