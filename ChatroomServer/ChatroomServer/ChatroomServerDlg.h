
// ChatroomServerDlg.h : header file
//

#pragma once
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