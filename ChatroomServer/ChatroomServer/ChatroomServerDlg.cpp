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
d
Báo cáo với người hướng dẫn để nhận flag

*/

// ChatroomServerDlg.cpp : implementation file

#include "pch.h"
#include "framework.h"
#include "ChatroomServer.h"
#include "ChatroomServerDlg.h"
#include "afxdialogex.h"

#include <Windows.h>
#include <WS2tcpip.h>
#include <cstring>
#include <string>
#include <winsock2.h>
#include <atlconv.h>

// ServerDlg.cpp : implementation file

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
	DDX_Text(pDX, IDC_EDIT2, strEditIP);
	DDX_Text(pDX, IDC_EDIT3, strEditPort);
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


SOCKET ListenSocket;

//kinda main
void CChatroomServerDlg::OnBnClickedButton1()
{
    UpdateData(TRUE);
    SYSTEMTIME time;
    WSADATA wsaData;
    CString strStatus, strTime, strError;
    UINT indexLog;
    button1.GetWindowText(strStatus);
    // Initialize Winsock
    if (strStatus == _T("Start")) {
        memset(&wsaData, 0, sizeof wsaData);
        if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            indexLog = listLog.GetItemCount();
            listLog.InsertItem(indexLog, strTime);
            listLog.SetItemText(indexLog, 1, _T("WSAStartup failed"));
            return;
        }

        // config server address
        addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the local address and port to be used by the server
        addrinfo* result;
        if (getaddrinfo((PCSTR)(CStringA)strEditIP.GetString(), (PCSTR)(CStringA)strEditPort.GetString(), &hints, &result) != 0) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            indexLog = listLog.GetItemCount();
            listLog.InsertItem(indexLog, strTime);
            listLog.SetItemText(indexLog, 1, _T("getaddrinfo failed"));
            return;
        };

        // Create a SOCKET for the server to listen for client connections
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            indexLog = listLog.GetItemCount();
            listLog.InsertItem(indexLog, strTime);
            strError.Format(_T("Socket creating failed with error code: %d"), WSAGetLastError());
            listLog.SetItemText(indexLog, 1, strError);
            freeaddrinfo(result);
            WSACleanup();
            return;
        }
        //bind socket
        if (bind(ListenSocket, result->ai_addr, result->ai_addrlen)) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            indexLog = listLog.GetItemCount();
            listLog.InsertItem(indexLog, strTime);
            strError.Format(_T("Socket binding failed with error code: %d"), WSAGetLastError());
            listLog.SetItemText(indexLog, 1, strError);
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }
        freeaddrinfo(result);

        // enter listening state
        if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            indexLog = listLog.GetItemCount();
            listLog.InsertItem(indexLog, strTime);
            strError.Format(_T("Listening failed with error code: %d"), WSAGetLastError());
            listLog.SetItemText(indexLog, 1, strError);
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        // create master thread for server
        mctParam.wndHandler = this;
        mctParam.lpParam = (LPVOID)ListenSocket;
        hMFCWorkerThread = AfxBeginThread(MFCWorkerThread, (LPVOID)&mctParam);

        // waiting for connections
        GetLocalTime(&time);
        strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
        indexLog = listLog.GetItemCount();
        listLog.InsertItem(indexLog, strTime);
        listLog.SetItemText(indexLog, 1, _T("Waiting for connections..."));
        button1.SetWindowText(_T("Stop"));
        serverStopStatus = FALSE;
    }
    else {
        serverStopStatus = TRUE;
        closesocket(ListenSocket);
        ListenSocket = INVALID_SOCKET;
        CloseHandle(hMFCWorkerThread->m_hThread);
        hMFCWorkerThread->m_hThread = NULL;
        listUser.DeleteAllItems();

        indexLog = listLog.GetItemCount();
        GetLocalTime(&time);
        strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
        listLog.InsertItem(indexLog, strTime);
        listLog.SetItemText(indexLog, 1, _T("Server closed"));

        button1.SetWindowText(_T("Start"));
    }
    UpdateData(FALSE);
}

UINT CChatroomServerDlg::MFCWorkerThread(LPVOID lpParam) {
    MultiCardThreadParam* mctParam = (MultiCardThreadParam*)lpParam;
    CChatroomServerDlg* dlgServer = (CChatroomServerDlg*)mctParam->wndHandler;
    SOCKET ListenSocket = (SOCKET)mctParam->lpParam;
    SYSTEMTIME time;
    CString strStatus, strTime, strError, strEditIP, strEditPort, strMsg;
    CStringA straMsg, straStatus;
    int indexChat, indexUser;
    char cstrEditIP[INET_ADDRSTRLEN];
    USES_CONVERSION;

    // manage active sockets
    fd_set master;
    FD_ZERO(&master);
    FD_SET(ListenSocket, &master);
    SOCKET max_socket = ListenSocket;

    while (!dlgServer->serverStopStatus) {
        fd_set reads;
        reads = master;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
            GetLocalTime(&time);
            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
            indexChat = dlgServer->listLog.GetItemCount();
            dlgServer->listLog.InsertItem(indexChat, strTime);
            strError.Format(_T("Socket selecting failed with error code: %d"), WSAGetLastError());
            dlgServer->listLog.SetItemText(indexChat, 1, strError);
            return FALSE;
        }

        SOCKET iSock;
        for (iSock = 1; iSock <= max_socket; ++iSock)
            if (FD_ISSET(iSock, &reads)) {
                // accept connection, add to master
                if (iSock == ListenSocket) {
                    sockaddr_in client_address;
                    socklen_t client_len = sizeof client_address;
                    SOCKET sock_client = accept(ListenSocket, (sockaddr*)&client_address, &client_len);
                    if (sock_client == INVALID_SOCKET)
                        if (!dlgServer->serverStopStatus) {
                            GetLocalTime(&time);
                            strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
                            indexChat = dlgServer->listLog.GetItemCount();
                            dlgServer->listLog.InsertItem(indexChat, strTime);
                            strError.Format(_T("Socket accepting failed with error code: %d"), WSAGetLastError());
                            dlgServer->listLog.SetItemText(indexChat, 1, strError);
                            return FALSE;
                        }
                        else {
                            //server closed
                            for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                                if (FD_ISSET(jSock, &master))
                                    closesocket(jSock);
                            return FALSE;

                        }

                    FD_SET(sock_client, &master);
                    if (sock_client > max_socket)
                        max_socket = sock_client;

                    inet_ntop(AF_INET, &client_address.sin_addr, cstrEditIP, INET_ADDRSTRLEN);
                    strEditIP = cstrEditIP;
                    strEditPort.Format(_T("%d"), ntohs(client_address.sin_port));

                    // update server logs
                    GetLocalTime(&time);
                    strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
                    indexChat = dlgServer->listLog.GetItemCount();
                    dlgServer->listLog.InsertItem(indexChat, strTime);
                    strStatus.Format(_T("New connection from IP %s"), (PCTSTR)strEditIP);
                    dlgServer->listLog.SetItemText(indexChat, 1, strStatus);

                    indexUser = dlgServer->listUser.GetItemCount();
                    dlgServer->listLog.InsertItem(indexUser, strEditIP);
                    dlgServer->listLog.SetItemText(indexUser, 1, strEditPort);

                    // push notifications to chatroom
                    strStatus.Format(_T("IP %s joined the chat"), (PCTSTR)strEditIP);
                    //const char* cstrStatus = (PCSTR)(PCTSTR)strStatus;
                    for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                        if (FD_ISSET(jSock, &master))
                            if (jSock == ListenSocket)
                                continue;
                            else {
                                std::string stt = CT2A(strStatus);
                                send(jSock, stt.c_str(), (int)stt.size(), 0);
                                //send(jSock, straStatus, sizeof straStatus, 0);
                            }
                }
                // read request for an established connection and send data
                else {
                    char read[1024];
                    int bytes_received = recv(iSock, read, 1024, 0);

                    // client disconnected
                    if (bytes_received < 1) {
                        // send new log to server
                        GetLocalTime(&time);
                        strTime.Format(_T("%02d:%02d:%02d"), time.wHour, time.wMinute, time.wSecond);
                        indexChat = dlgServer->listLog.GetItemCount();
                        dlgServer->listLog.InsertItem(indexChat, strTime);
                        strStatus.Format(_T("IP %s disconnected"), (LPCTSTR)strEditIP);
                        dlgServer->listLog.SetItemText(indexChat, 1, strStatus);

                        // send notification to chat room
                        strStatus.Format(_T("IP %s left the chat"), (LPCTSTR)strEditIP);
                        for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                            if (FD_ISSET(jSock, &master))
                                if (jSock == ListenSocket || jSock == iSock)
                                    continue;
                                else {
                                    std::string stt = CT2A(strStatus);
                                    send(jSock, stt.c_str(), stt.size(), 0);
                                }

                        // delete from user list
                        for (int iItem = 0; iItem < dlgServer->listUser.GetItemCount(); ++iItem) {
                            if (dlgServer->listUser.GetItemText(iItem, 0) == strEditIP && dlgServer->listUser.GetItemText(iItem, 1) == strEditPort)
                                dlgServer->listUser.DeleteItem(iItem);
                        }

                        FD_CLR(iSock, &master);
                        closesocket(iSock);
                        continue;
                    }
                    // send messages to chat room
                    for (SOCKET jSock = 1; jSock <= max_socket; ++jSock)
                        if (FD_ISSET(jSock, &master))
                            if (jSock == ListenSocket)
                                continue;
                    // self-message
                            else if (jSock == iSock) {
                                straMsg.Format("You: %.*s", bytes_received, read);
                                std::string msg = straMsg;
                                send(jSock, msg.c_str(), msg.size(), 0);
                            }
                    // other ip see
                            else {
                                straMsg.Format("%s: %.*s", cstrEditIP, bytes_received, read);
                                std::string msg = straMsg;
                                send(jSock, msg.c_str(), msg.size(), 0);
                            }
                }
            }
    }
    return TRUE;
}
