
// ChatroomServerDlg.h : header file
//
#pragma once

#include <mswsock.h>

#define MAX_BUFF_SIZE       8192
#define MAX_WORKER_THREAD	128
// CChatroomServerDlg dialog
class CChatroomServerDlg : public CDialogEx
{
// Construction
public:
	CChatroomServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATROOMSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit3();
protected:
	CString editIP;
	CString editPort;
	CButton button1;
	CListCtrl listUser;
	CListCtrl listLog;
public:
	afx_msg void OnBnClickedButton1();
};

typedef enum _IO_OPERATION {
	ClientIoAccept,
	ClientIoRead,
	ClientIoWrite
} IO_OPERATION, * PIO_OPERATION;


typedef struct _PER_IO_CONTEXT {
	WSAOVERLAPPED               Overlapped;
	char                        Buffer[MAX_BUFF_SIZE];
	WSABUF                      wsabuf;
	int                         nTotalBytes;
	int                         nSentBytes;
	IO_OPERATION                IOOperation;
	SOCKET                      SocketAccept;

	struct _PER_IO_CONTEXT* pIOContextForward;
} PER_IO_CONTEXT, * PPER_IO_CONTEXT;


typedef struct _PER_SOCKET_CONTEXT {
	SOCKET                      Socket;

	LPFN_ACCEPTEX               fnAcceptEx;

	//
	//linked list for all outstanding i/o on the socket
	//
	PPER_IO_CONTEXT             pIOContext;
	struct _PER_SOCKET_CONTEXT* pCtxtBack;
	struct _PER_SOCKET_CONTEXT* pCtxtForward;
} PER_SOCKET_CONTEXT, * PPER_SOCKET_CONTEXT;

BOOL ValidOptions(int argc, char* argv[]);

BOOL WINAPI CtrlHandler(
	DWORD dwEvent
);

BOOL CreateListenSocket(void);

BOOL CreateAcceptSocket(
	BOOL fUpdateIOCP
);

DWORD WINAPI WorkerThread(
	LPVOID WorkContext
);

PPER_SOCKET_CONTEXT UpdateCompletionPort(
	SOCKET s,
	IO_OPERATION ClientIo,
	BOOL bAddToList
);
//
// bAddToList is FALSE for listening socket, and TRUE for connection sockets.
// As we maintain the context for listening socket in a global structure, we
// don't need to add it to the list.
//

VOID CloseClient(
	PPER_SOCKET_CONTEXT lpPerSocketContext,
	BOOL bGraceful
);

PPER_SOCKET_CONTEXT CtxtAllocate(
	SOCKET s,
	IO_OPERATION ClientIO
);

VOID CtxtListFree(
);

VOID CtxtListAddTo(
	PPER_SOCKET_CONTEXT lpPerSocketContext
);

VOID CtxtListDeleteFrom(
	PPER_SOCKET_CONTEXT lpPerSocketContext
);

void LogError(CString strError);
