
// ChatroomServerDlg.h : header file
//
#pragma once

#include <mswsock.h>

// CChatroomServerDlg dialog

typedef struct _MultiCardThreadParam {
	CWnd* wndHandler;
	LPVOID lpParam;
} MultiCardThreadParam;

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
	SOCKET ListenSocket;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CWinThread* hMFCWorkerThread;
public:
	CListCtrl listLog;
protected:
	CString strEditIP;
	CString strEditPort;
	CButton button1;
	CListCtrl listUser;
public:
	MultiCardThreadParam mctParam;
	static UINT MFCWorkerThread(LPVOID lpParam);
	BOOL serverStopStatus = TRUE;
	afx_msg void OnBnClickedButton1();
};
