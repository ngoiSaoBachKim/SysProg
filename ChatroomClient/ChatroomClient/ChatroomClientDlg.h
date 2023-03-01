
// ChatroomClientDlg.h : header file
//

#pragma once


// CChatroomClientDlg dialog
class CChatroomClientDlg : public CDialogEx
{
// Construction
public:
	CChatroomClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATROOMCLIENT_DIALOG };
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
	CEdit editIP;
	CEdit editPort;
	CButton buttonConnect;
public:
	afx_msg void OnBnClickedButton1();
protected:
	CEdit editShowChannel;
	CEdit editMessage;
	CButton buttonSend;
};
