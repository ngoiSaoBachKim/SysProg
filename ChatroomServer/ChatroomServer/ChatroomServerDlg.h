
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
	SOCKET sock_listen;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CWinThread* hMasterThread;
public:
	CListCtrl listLog;
protected:
	CString editIP;
	CString editPort;
	CButton button1;
	CListCtrl listUser;
public:
	typedef struct _MultiCardThreadParam {
		CWnd* wndHandler;
		LPVOID lpParam;
	} MultiCardThreadParam;
	MultiCardThreadParam mctParam;

	void Log(CString strError);
	static UINT MasterThread(LPVOID lpParam);
	BOOL serverStopStatus;
	afx_msg void OnBnClickedButton1();
};