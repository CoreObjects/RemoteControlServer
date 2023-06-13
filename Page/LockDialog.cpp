﻿// LockDialog.cpp: 实现文件
//
#include "..\Resource\resource.h"
#include "..\FrameWork\framework.h"
#include "LockDialog.h"
#include "afxdialogex.h"


// CLockDialog 对话框

IMPLEMENT_DYNAMIC(CLockDialog, CDialogEx)

CLockDialog::CLockDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INFO, pParent)
{

}

CLockDialog::~CLockDialog()
{
}

void CLockDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLockDialog, CDialogEx)
END_MESSAGE_MAP()


// CLockDialog 消息处理程序
