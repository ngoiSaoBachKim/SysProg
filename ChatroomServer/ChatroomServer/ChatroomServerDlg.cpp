/*SysProg #3 - Chatroom
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
#include <vector>
#include <atlconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>
#include <Windows.h>
// ServerDlg.cpp : implementation file

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#define xfree(p)   HeapFree(GetProcessHeap(),0,(p))
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

BOOL g_bEndServer = FALSE;			// set to TRUE on CTRL-C
BOOL g_bRestart = TRUE;				// set to TRUE to CTRL-BRK
BOOL g_bVerbose = FALSE;
DWORD g_dwThreadCount = 0;		//worker thread count
HANDLE g_hIOCP = INVALID_HANDLE_VALUE;
SOCKET g_sdListen = INVALID_SOCKET;
HANDLE g_ThreadHandles[MAX_WORKER_THREAD];
WSAEVENT g_hCleanupEvent[1];
PPER_SOCKET_CONTEXT g_pCtxtListenSocket = NULL;
PPER_SOCKET_CONTEXT g_pCtxtList = NULL;
CRITICAL_SECTION g_CriticalSection;

//kinda main
void CChatroomServerDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	// Update button text and state
	CString strStartStatus, strTime, strError;
	button1.GetWindowText(strStartStatus);
	if (strStartStatus == _T("Start"))
	{
		SYSTEM_INFO systemInfo;
		WSADATA wsaData;
		DWORD dwThreadCount = 0;
		int nRet = 0;

		g_ThreadHandles[0] = (HANDLE)WSA_INVALID_EVENT;

		for (int i = 0; i < MAX_WORKER_THREAD; i++) {
			g_ThreadHandles[i] = INVALID_HANDLE_VALUE;
		}

		GetSystemInfo(&systemInfo);
		dwThreadCount = systemInfo.dwNumberOfProcessors * 2;
		
		if (WSA_INVALID_EVENT == (g_hCleanupEvent[0] = WSACreateEvent()))
		{
			//special?
			LogError(_T("WSACreateEvent() failed: %d\n", WSAGetLastError()));
			return;
		}

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			LogError(_T("Successfully failed to initializing Winsock"));
			return;
		}

		__try
		{
			InitializeCriticalSection(&g_CriticalSection);

			g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
			if (g_hIOCP == NULL) {
				LogError(_T("CreateIoCompletionPort() failed to create I/O completion port: %d\n",
					GetLastError()));
				__leave;
			}

			for (DWORD dwCPU = 0; dwCPU < dwThreadCount; dwCPU++) {

				//
				// Create worker threads to service the overlapped I/O requests.  The decision
				// to create 2 worker threads per CPU in the system is a heuristic.  Also,
				// note that thread handles are closed right away, because we will not need them
				// and the worker threads will continue to execute.
				//
				HANDLE  hThread;
				DWORD   dwThreadId;

				hThread = CreateThread(NULL, 0, WorkerThread, g_hIOCP, 0, &dwThreadId);
				if (hThread == NULL) {
					myprintf("CreateThread() failed to create worker thread: %d\n",
						GetLastError());
					__leave;
				}
				g_ThreadHandles[dwCPU] = hThread;
				hThread = INVALID_HANDLE_VALUE;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			LogError(_T("InitializeCriticalSection raised an exception"));
			return;
		}

		while (g_bRestart) {
			g_bRestart = FALSE;
			g_bEndServer = FALSE;
			WSAResetEvent(g_hCleanupEvent[0]);
			__try {

			}
			__finally {

			}
		}
		button1.SetWindowText(_T("Stop"));
	}
	else {
		// Stop the server

		// Update button text and state
		button1.SetWindowText(_T("Start"));
	}
	UpdateData(FALSE);
}

void LogError(CString strError) {

}

/*
 		SetEvent(g_exitEvent);
		CloseHandle(g_exitEvent);
		closesocket(ListenSocket);
		ListenSocket = INVALID_SOCKET;
void CChatroomServerDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	CString strStartStatus, strTime, strError;
	button1.GetWindowText(strStartStatus);
	SYSTEMTIME st;
	int indexLog;
	if (strStartStatus == _T("Start"))
	{
		// Start the server
		WSADATA wsaData;
		memset(&wsaData, 0, sizeof(wsaData));
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			GetLocalTime(&st);
			strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			indexLog = listLog.GetItemCount();
			listLog.InsertItem(indexLog, strTime);
			strError.Format(_T("Successfully failed to initializing Winsock);
				listLog.SetItemText(indexLog, 1, strError);
			return;
		}
		struct addrinfo* serverAddrInfo = NULL, hints;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		CString ip = editIP.GetString();
		CString port = editPort.GetString();

		if (getaddrinfo((PCSTR)(CStringA)ip, (PCSTR)(CStringA)port, &hints, &serverAddrInfo) != 0) {
			GetLocalTime(&st);
			strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			indexLog = listLog.GetItemCount();
			listLog.InsertItem(indexLog, strTime);
			strError.Format(_T("Successfully failed to get address info(error code %ld)."), WSAGetLastError());
			listLog.SetItemText(indexLog, 1, strError);
			WSACleanup();
			return;
		}
		// Create a socket for listening to clients
		//SOCKET ListenSocket = INVALID_SOCKET;
		ListenSocket = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			GetLocalTime(&st);
			strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			indexLog = listLog.GetItemCount();
			listLog.InsertItem(indexLog, strTime);
			strError.Format(_T("Successfully managed to get error at socket(): %ld."), WSAGetLastError());
			listLog.SetItemText(indexLog, 1, strError);
			freeaddrinfo(serverAddrInfo);
			WSACleanup();
			return;
		}
		// Bind the socket to a local address and port
		if (bind(ListenSocket, serverAddrInfo->ai_addr, (int)serverAddrInfo->ai_addrlen) == SOCKET_ERROR) {
			GetLocalTime(&st);
			strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			indexLog = listLog.GetItemCount();
			listLog.InsertItem(indexLog, strTime);
			strError.Format(_T("Successfully failed to bind with error code: %ld."), WSAGetLastError());
			listLog.SetItemText(indexLog, 1, strError);
			freeaddrinfo(serverAddrInfo);
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		freeaddrinfo(serverAddrInfo);
		// Listening on a Socket
		if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
			GetLocalTime(&st);
			strTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"),
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			indexLog = listLog.GetItemCount();
			listLog.InsertItem(indexLog, strTime);
			strError.Format(_T("Successfully failed to listen with error code: %ld."), WSAGetLastError());
			listLog.SetItemText(indexLog, 1, strError);
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}

		HANDLE completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		SYSTEM_INFO systemInfo{};
		GetSystemInfo(&systemInfo);
		g_threads.resize(systemInfo.dwNumberOfProcessors * 2);
		for (DWORD i = 0; i < systemInfo.dwNumberOfProcessors * 2; ++i) {
			HANDLE thread = CreateThread(NULL, 0, [](LPVOID lpThreadParameter) -> DWORD {
				HANDLE completionPort = reinterpret_cast<HANDLE>(lpThreadParameter);
			while (true) {
				// Wait for an I/O completion packet or the exit event
				DWORD bytesTransferred;
				SOCKET completionKey;
				LPOVERLAPPED overlapped;
				DWORD waitResult = WaitForSingleObject(g_exitEvent, 0);
				if (waitResult == WAIT_OBJECT_0) {
					// Exit the thread if the exit event is signaled
					break;
				}
				waitResult = GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
				if (waitResult != 0) {
					// Process the I/O completion packet
					closesocket(completionKey);
					delete overlapped;
				}
			}
			return 0;
				}, completionPort, 0, NULL);
			g_threads[i] = thread;
		}

		// Update button text and state
		button1.SetWindowText(_T("Stop Server"));
	}
	else
	{
		// Stop the server
		// Update button text and state
		// Stop the server
		SetEvent(g_exitEvent);
		CloseHandle(g_exitEvent);
		closesocket(ListenSocket);
		ListenSocket = INVALID_SOCKET;

		// Update button text and state
		button1.SetWindowText(_T("Start Server"));
	}
}
*/