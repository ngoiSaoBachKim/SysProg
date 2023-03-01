﻿/*SysProg #3 - Chatroom
100
Viết chương trình chat client-server Yêu cầu:

Hệ điều hành: Windows
Ứng dụng MFC có giao diện
Giao thức: TCP
Sử dụng Windows Socket
Protocol chat tự định nghĩa
**Đối với server gồm các control:**
Editbox nhập địa chỉ IP sẽ listen (VD: 0.0.0.0)
Editbox nhập port sẽ listen (VD: 4444)
Button START: khi chưa listen hiện START, click sẽ listen và chuyển thành STOP
CListCtrl hiện danh sách các client đang kết nối, gồm các thông tin: địa chỉ IP, port
CListCtrl hiện log server, gồm các cột: thời gian, nội dung (log các thông tin: server start/stop, client connected/disconnected)
**Đối với client gồm các control:**
Editbox nhập địa chỉ server
Editbox nhập port
Nút CONNECT: ban đầu hiện CONNECT, sau khi click thực hiện kết nối đến server và hiện DISCONNECT
Editbox: chatroom
Editbox: ô nhập nội dung chat
Button SEND: để gửi nội dung chat
Ghi chú:

Báo cáo với người hướng dẫn để nhận flag

*/

// ChatroomServerDlg.cpp : implementation file

#include "pch.h"
#include "framework.h"
#include "ChatroomServer.h"
#include "ChatroomServerDlg.h"
#include "afxdialogex.h"
#include <atlstr.h>
#include <cstring>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatroomServerDlg dialog



CChatroomServerDlg::CChatroomServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATROOMSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatroomServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT2, editIP);
	DDX_Text(pDX, IDC_EDIT3, editPort);
	DDX_Control(pDX, IDC_BUTTON1, button1);
	DDX_Control(pDX, IDC_LIST1, listUser);
	DDX_Control(pDX, IDC_LIST2, listLog);
}

BEGIN_MESSAGE_MAP(CChatroomServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CChatroomServerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CChatroomServerDlg message handlers

BOOL CChatroomServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	listLog.InsertColumn(0, _T("Time"), LVCFMT_LEFT, 100);
	listLog.InsertColumn(1, _T("Event"), LVCFMT_LEFT, 380);
	listLog.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	listUser.InsertColumn(0, _T("IP Address"), LVCFMT_LEFT, 150);
	listUser.InsertColumn(1, _T("Port"), LVCFMT_LEFT, 100);
	listUser.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatroomServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatroomServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChatroomServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChatroomServerDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	CString strStartStatus;
	button1.GetWindowText(strStartStatus);

	if (strStartStatus == _T("Start")) {
		WSADATA wsaData;

		// Initialize Winsock
		memset(&wsaData, 0, sizeof(wsaData));
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			MessageBox((LPCWSTR)L"Successfully failed to start server(Initializing Winsock failed), try again.", (LPCWSTR)L"Error", MB_ICONWARNING | MB_DEFBUTTON2);
			return;
		}
		struct addrinfo* result = NULL, * ptr = NULL, hints;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		CString ip = editIP.GetString();
		CString port = editPort.GetString();

		if (getaddrinfo((PCSTR)(CStringA)ip, (PCSTR)(CStringA)port, &hints, &result) != 0) {
			MessageBox((LPCWSTR)L"Successfully failed to start server(Get address info failed), try again.", (LPCWSTR)L"Error", MB_ICONWARNING | MB_DEFBUTTON2);
			WSACleanup();
			return;
		}
		SOCKET ListenSocket = INVALID_SOCKET;
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return;
		}
		// Setup the TCP listening socket
		if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
			MessageBox((LPCWSTR)L"Successfully failed to start server(bind failed), try again.", (LPCWSTR)L"Error", MB_ICONWARNING | MB_DEFBUTTON2);
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		freeaddrinfo(result);
		button1.SetWindowText(_T("Stop"));
	}
	else {
		button1.SetWindowText(_T("Start"));
	}
	UpdateData(FALSE);
	return;
}
